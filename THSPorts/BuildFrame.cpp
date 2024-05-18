#include "StdAfx.h"
#include "BuildFrame.h"
#include "DbgMsg.h"
#include "interal.h"


int    CBuildFrame::F1_Encode(PUCHAR pSrc, PUCHAR   pDes, int SrcLength)
{
	int   nLen = 0;
	
	for( int j = 0 ; j < SrcLength ; j ++ )
	{
		if( pSrc[j] >= m_LowRange && pSrc[j] <= m_UpRange )
		{
			pDes[nLen++] = m_MapFlag;
			pDes[nLen++] = pSrc[j] - m_MapFlag;
		}
		else
			pDes[nLen++] = pSrc[j];
	}
	return nLen;
}

int   CBuildFrame::F1_Decode(PUCHAR  pSrc, PUCHAR   pDes, int SrcLength)
{
	int                 nLen = 0;
	
	for( int j = 0 ; j < SrcLength ; j ++ )
	{
		if( pSrc[j] == m_MapFlag )
		{
			pDes[nLen] = pSrc[j+1] + m_MapFlag;
			j++;
		}
		else
			pDes[nLen] = pSrc[j];
		
		nLen ++;
	}
	return nLen;
}


UCHAR   CBuildFrame::F1_GetXorValue(PUCHAR p, int nLen)
{
	UCHAR   x_value = 0;
	
	for( int j = 0 ; j < nLen ; j ++ )
	{
		x_value ^= p[j];
	}
	
	return x_value;
}

int   CBuildFrame::F1_GetBuildCmdBufferLength(int nBufferLength)
{
	return   (nBufferLength + sizeof(FRAME1_FULL_COMMAND)) << 1;
}

int  CBuildFrame::F1_GetParseCmdBuferLength(int nCmdBufferLength)
{
	return   (nCmdBufferLength - sizeof(FRAME1_FULL_COMMAND) + 1);
}

int CBuildFrame::F1_BuildCmdData(PCHAR  pOrgContent, int nLength, PCHAR  pBuildCmdBuffer)
{
	int                      nLen;
	char                     TempBuffer[512] = {0};
	PFRAME_HEADER_TYPE1      header;
	PFRAME_TAIL_TYPE1        tail;
	PCHAR                    pContent;
	
	nLength --;
	if( m_Frame_Verify_Way == XOR_VERIFY_WAY )
		nLen   = sizeof(FRAME1_FULL_COMMAND) + nLength;
	else
		nLen  =  sizeof(FRAME_HEADER_TYPE1) + sizeof(FRAME_TAIL_TYPE2) + nLength;

	header = (PFRAME_HEADER_TYPE1)TempBuffer;

	header->FrameHeader = m_HeaderFlag;
	header->FrameCMD    = pOrgContent[0];
	pContent            = (PCHAR)((ULONG)header + sizeof(FRAME_HEADER_TYPE1));
	tail                = (PFRAME_TAIL_TYPE1)((ULONG)pContent + nLength);
	
	if( nLength )
		memcpy(pContent, &pOrgContent[1], nLength);
	
	if( m_Frame_Verify_Way == XOR_VERIFY_WAY )
		tail->nXorCode = F1_GetXorValue((PUCHAR)&header->FrameCMD, nLength + sizeof(FRAME_HEADER_TYPE1) - 1);
		
	else
	{
		PFRAME_TAIL_TYPE2  tail2 = (PFRAME_TAIL_TYPE2)tail;
		tail2->nSumCode = F1_CheckSumValue((PUCHAR)&header->FrameCMD, nLength + sizeof(FRAME_HEADER_TYPE1) - 1);
	}
	
	ZeroMemory(pBuildCmdBuffer, nLen << 1);
	memcpy(pBuildCmdBuffer, header, sizeof(FRAME_HEADER_TYPE1));
	nLen  = F1_Encode((PUCHAR)pContent, (PUCHAR)&pBuildCmdBuffer[sizeof(FRAME_HEADER_TYPE1)], nLength + sizeof(FRAME_TAIL_TYPE1) - 1);
	tail  = (PFRAME_TAIL_TYPE1)&pBuildCmdBuffer[nLen + sizeof(FRAME_HEADER_TYPE1) - 1];
	if( m_Frame_Verify_Way == XOR_VERIFY_WAY )
	{
		tail->FrameTail = m_TailFalg;
		nLen += sizeof(FRAME_HEADER_TYPE1) + (sizeof(FRAME_TAIL_TYPE1) - 1);
	}
	else
	{
		PFRAME_TAIL_TYPE2  tail2 = (PFRAME_TAIL_TYPE2)tail;
		tail2->FrameTail = m_TailFalg;
		nLen += sizeof(FRAME_HEADER_TYPE1) + (sizeof(FRAME_TAIL_TYPE2) - 1);
	}

	return nLen;
	
}

int CBuildFrame::F1_ParseCmdData(PCHAR  pCmdBuffer, int nLength, PCHAR  pContent)
{
	int                      nLen;
	char                     TempBuffer[512];
//	UCHAR                    xor_value;
	PFRAME_HEADER_TYPE1      header;
//	PFRAME_TAIL_TYPE1        tail;


	nLen      = F1_Decode((PUCHAR)pCmdBuffer, (PUCHAR)TempBuffer, nLength);
	header    = (PFRAME_HEADER_TYPE1)TempBuffer;
	if( m_Frame_Verify_Way == XOR_VERIFY_WAY )
	{
//		tail      = (PFRAME_TAIL_TYPE1)&TempBuffer[nLen-sizeof(FRAME_TAIL_TYPE1)];
		nLen = nLen - sizeof(FRAME1_FULL_COMMAND) + 1;
	}
	else
	{
//		tail     = (PFRAME_TAIL_TYPE1)&TempBuffer[nLen- sizeof(FRAME_TAIL_TYPE2)];
		nLen = nLen - sizeof(FRAME_HEADER_TYPE1) - sizeof(FRAME_TAIL_TYPE2) + 1;
	}
//	xor_value = F1_GetXorValue((PUCHAR)&header->FrameCMD, nLen - (sizeof(FRAME_HEADER_TYPE1) - 1) - sizeof(FRAME_TAIL_TYPE1));
//	if( xor_value != tail->nXorCode )
//		return 0;

	

	memcpy(pContent, &header->FrameCMD,  nLen);
	return nLen;
}





#define  FRAME_FLAG_SESSION   "FrameFormat"
#define  HEADER_KEY           "Header"
#define  TAIL_KEY             "Tail"
#define  MAP_KEY              "Map"
#define  CMD_OFFSET_KEY       "cmd_off"
#define  FRAME_VERAY_KEY      "FrameVerify"

void CBuildFrame::SetFrameFlag( PCHAR pCfgPath )
{

	m_HeaderFlag = GetPrivateProfileInt(FRAME_FLAG_SESSION, HEADER_KEY, F1_HEADER_CHAR, pCfgPath);
	m_TailFalg   = GetPrivateProfileInt(FRAME_FLAG_SESSION, TAIL_KEY, F1_TAIL_CHAR, pCfgPath);
	m_MapFlag    = GetPrivateProfileInt(FRAME_FLAG_SESSION, MAP_KEY, F1_MAP_CHAR, pCfgPath);
	m_CmdOffset  = GetPrivateProfileInt(FRAME_FLAG_SESSION,CMD_OFFSET_KEY, 1, pCfgPath);
	m_Frame_Verify_Way = GetPrivateProfileInt(FRAME_FLAG_SESSION,FRAME_VERAY_KEY, XOR_VERIFY_WAY, pCfgPath);

	m_LowRange   = min(m_HeaderFlag, m_TailFalg);
	m_LowRange   = min(m_MapFlag, m_LowRange);
	m_UpRange    = max(m_HeaderFlag, m_TailFalg);
	m_UpRange    = max(m_UpRange, m_MapFlag);

}

USHORT CBuildFrame::F1_CheckSumValue( PUCHAR p, int nLen )
{
	USHORT   s_value = 0;
	
	for( int j = 0 ; j < nLen ; j ++ )
		s_value += p[j];
	
	return s_value;
	
}

int CBuildFrame::F2_ParseCmdData(PCHAR  pCmdBuffer, ULONG& nLength, PCHAR  pContent)
{
	int                 nVaildLen = 1;
	PCOMMON_HEADER_1    header = (PCOMMON_HEADER_1)pCmdBuffer;


	nVaildLen += header->d_len;
	pContent[0] = (CHAR)header->cmd;
	if( header->d_len )
		memcpy(&pContent[1], header->data, header->d_len);

	nLength -= (header->h_len + header->d_len);
	if( nLength )
		memcpy(pCmdBuffer, &pCmdBuffer[header->h_len+header->d_len], nLength);

	
	return nVaildLen;

}

int  CBuildFrame::F2_BuildCmdData(PCHAR  pOrgContent, int nLength, PCHAR  pBuildCmdBuffer)
{

	PCOMMON_HEADER_1   cmd_header = (PCOMMON_HEADER_1)pBuildCmdBuffer;


	ZeroMemory(cmd_header, sizeof(COMMON_HEADER_1));
	cmd_header->flag  = F1_HEADER_CHAR;
	cmd_header->h_len = sizeof(COMMON_HEADER_1) - 1;
	cmd_header->cmd   = pOrgContent[0];
	cmd_header->d_len = (UCHAR)(nLength - 1);

	if( cmd_header->d_len )
	{
		memcpy(cmd_header->data, &pOrgContent[1], cmd_header->d_len);
		cmd_header->d_check = F1_GetXorValue((PUCHAR)cmd_header->data,cmd_header->d_len);
	}

	cmd_header->h_check = F1_GetXorValue((PUCHAR)cmd_header, sizeof(COMMON_HEADER_1) - 2);
	return  (cmd_header->h_len + cmd_header->d_len);

}

int CBuildFrame::F3_ParseCmdData(PCHAR  pCmdBuffer, ULONG& nLength, PCHAR  pContent)
{
	int                 nVaildLen = 1;
	PCOMMON_HEADER_2    header = (PCOMMON_HEADER_2)pCmdBuffer;
	int nHdrLen = sizeof(COMMON_HEADER_2)-1;

	nVaildLen += header->d_len;
	pContent[0] = (CHAR)header->cmd;
	if( header->d_len )
		memcpy(&pContent[1], header->data, header->d_len);
	
	nLength -= (nHdrLen + header->d_len);
	if( nLength )
		memcpy(pCmdBuffer, &pCmdBuffer[nHdrLen+header->d_len], nLength);

	return nVaildLen;
}

int  CBuildFrame::F3_BuildCmdData(PCHAR  pOrgContent, int nLength, PCHAR  pBuildCmdBuffer)
{

	PCOMMON_HEADER_2   cmd_header = (PCOMMON_HEADER_2)pBuildCmdBuffer;
	int nHdrLen = sizeof(COMMON_HEADER_2)-1;

	ZeroMemory(cmd_header, sizeof(COMMON_HEADER_2));
	cmd_header->flag  = F1_HEADER_CHAR;
	cmd_header->cmd   = pOrgContent[0];
	cmd_header->d_len = (UCHAR)(nLength - 1);

	if( cmd_header->d_len )
	{
		memcpy(cmd_header->data, &pOrgContent[1], cmd_header->d_len);
		cmd_header->d_check = F1_GetXorValue((PUCHAR)cmd_header->data,cmd_header->d_len);
	}

	cmd_header->h_check = F1_GetXorValue((PUCHAR)cmd_header, nHdrLen-1);
	return  (nHdrLen + cmd_header->d_len);
}
