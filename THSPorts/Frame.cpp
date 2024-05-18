#include "StdAfx.h"
#include "Frame.h"
#include <malloc.h>
#include "DbgMsg.h"
#include "interal.h"
#include "BuildFrame.h"


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>

#include <crtdbg.h>

#endif



#define  INVAILD_VALUE  0XFFFFFFFF

#define  FRAME_HEADER   0xFF
#define  FRAME_TAIL     0XFE

CFrame::CFrame()
{

}

void  WINAPI CFrame::FrameRequestCompletion(PRP  prp, PVOID  pContext, KSTATUS   status)
{
	ULONG              nInSize;
	KSTATUS            status1;
	PRCV_BUFFER_PROC   rcv_buffer;

	status1 = FMMC_GetPRPParmeters(prp, (PVOID*)&rcv_buffer, &nInSize, NULL, NULL);
	if( status1 == STATUS_SUCCESS && nInSize == sizeof(RCV_BUFFER_PROC) )
		free(rcv_buffer->pRcvBuffer);


	FMMC_FreePRP(prp);

}

BOOL  CFrame::AnsyFrameRequest(PUCHAR  pBuffer, ULONG  nLength, BOOLEAN bParsePro,PROGRAM_OBJECT  TargetObject)
{
	PRP               prp = NULL;
	KSTATUS           status;
	RCV_BUFFER_PROC   rcv = {0};


	rcv.pRcvBuffer = (PCHAR)malloc(nLength);
	if( rcv.pRcvBuffer == NULL )
		return FALSE;

	rcv.nRcvSize   = nLength;
	rcv.bParsePro  = bParsePro;
	memcpy(rcv.pRcvBuffer, pBuffer, nLength);

	status = FMMC_AsynchronousBuildPRP(TargetObject, 
		                               NULL, 
									   PRP_AC_DEVIO, 
									   RCV_SERIAL_PORT,
									   &rcv,
									   sizeof(RCV_BUFFER_PROC),
									   NULL,
									   0,
									   &prp);

	if( status != STATUS_SUCCESS )
		goto send_fail;

	status = FMMC_SetPRPCompleteDispatch(prp, FrameRequestCompletion, NULL);
	if( status != STATUS_SUCCESS )
		goto send_fail;


	status = FMMC_PassRequest(TargetObject, prp);

send_fail:

	if( status != STATUS_SUCCESS )
	{
		if( prp )
			FMMC_FreePRP(prp);

		free(rcv.pRcvBuffer);
	}

	return (status == STATUS_SUCCESS);
}

BOOL CFrame::SynFrameRequest( PUCHAR pBuffer, ULONG nLength, BOOLEAN bParsePro,PROGRAM_OBJECT TargetObject )
{
	PRP               prp = NULL;
	RCV_BUFFER_PROC   rcv = {0};
	
	
	rcv.nRcvSize   = nLength;
	rcv.bParsePro  = bParsePro;
	rcv.pRcvBuffer = (PCHAR)pBuffer;
	
	return   FMMC_EasySynchronousBuildPRP(TargetObject, 
		                                  NULL, 
		                                  PRP_AC_DEVIO, 
		                                  RCV_SERIAL_PORT,
		                                  &rcv,
		                                  sizeof(RCV_BUFFER_PROC),
		                                  NULL,
		                                  0,
		                                  FALSE,
									      FALSE);
}

BOOL   CFrame::PickupFrame(PUCHAR  pBuffer, ULONG  &nLength, PROGRAM_OBJECT  TargetObject, USHORT  FrameType)
{

	if( FrameType == DATA_PARSE_TYPE1 )
		return PickupFrameForType1(pBuffer, nLength, TargetObject);
	else if( FrameType == DATA_PRASE_TYPE2 )
		return PickupFrameForType2(pBuffer, nLength, TargetObject);
	else if( FrameType == DATA_PRASE_TYPE3 )
		return PickupFrameForType3(pBuffer, nLength, TargetObject);
	else
	{
		ULONG    n = nLength;
		nLength = 0;

		return SynFrameRequest(pBuffer, n, FALSE, TargetObject);
	}
}

BOOL  CFrame::PickupFrameForType1(PUCHAR  pBuffer, ULONG  &nLength, PROGRAM_OBJECT  TargetObject)
{
	char   ContentBuffer[512];
	int    nContent_Len;
	ULONG  nHeadOffset =INVAILD_VALUE, nTailOffset = INVAILD_VALUE, j;

	for( j = 0 ; j < nLength ; j ++ )
	{
		if( pBuffer[j] == m_HeaderFlag )
			nHeadOffset = j;
		else if( pBuffer[j] == m_TailFalg )
		{
			nTailOffset = j;
			if( nHeadOffset != INVAILD_VALUE && nTailOffset > (nHeadOffset + 2) )
			{
				nContent_Len = F1_ParseCmdData((PCHAR)&pBuffer[nHeadOffset], nTailOffset-nHeadOffset + 1, ContentBuffer);
				if( nContent_Len )
					AnsyFrameRequest((PUCHAR)ContentBuffer, nContent_Len, TRUE, TargetObject);

				nHeadOffset = nTailOffset = INVAILD_VALUE;
			}
		}
	}

	if( nHeadOffset == INVAILD_VALUE )  //只要是nHeadOffset == INVAILD_VALUE肯定是垃圾数据,直接丢弃
		nLength = 0;
	else
	{
		//为0就不进行任何处理
		if( nHeadOffset )
		{
			nLength = nLength - nHeadOffset; //保留余留数据
			memcpy(pBuffer, &pBuffer[nHeadOffset], nLength);
		}
	}

	return TRUE;
}

BOOL CFrame::GetFrameHeaderForType2(PUCHAR  pBuffer, ULONG  &nLength)
{
	ULONG  nStart;
	for( nStart = 0 ; nStart < nLength ; nStart++ )
	{
		if( pBuffer[nStart] == FRAME_HEADER )
			break;
	}

	if( nStart == nLength )
	{
		nLength = 0;
		return FALSE;
	}

	if( nStart == 0 )
		return TRUE;
	
	nLength = nLength - nStart;
	memcpy(pBuffer, &pBuffer[nStart], nLength);
	return TRUE;
}

BOOL CFrame::IsVaildFrameForType2(PUCHAR  pBuffer, ULONG &nLength)
{
	UCHAR               nCheck;
	PCOMMON_HEADER_1    header;

	if( nLength < sizeof(COMMON_HEADER_1) - 1 )
		return FALSE;

retry_vaild:
	header = (PCOMMON_HEADER_1)pBuffer;
	if( header->h_len != (sizeof(COMMON_HEADER_1) - 1) )
	{
invaild_header:
		nLength --;
		if( nLength == 0 )
			return FALSE;

		memcpy(pBuffer, &pBuffer[1], nLength);
		if( GetFrameHeaderForType2(pBuffer, nLength) == FALSE )
			return FALSE;
		goto retry_vaild;
	}

	nCheck = CBuildFrame::F1_GetXorValue(pBuffer, header->h_len - 1);

	if( nCheck != header->h_check )
		goto invaild_header;
	
	if( (ULONG)(header->d_len + header->h_len) > nLength )
		return FALSE;  

	nCheck = CBuildFrame::F1_GetXorValue((PUCHAR)header->data,header->d_len);
	if( nCheck != header->d_check )
		goto invaild_header;

	return TRUE;

}

BOOL CFrame::IsVaildFrameForType3(PUCHAR  pBuffer, ULONG &nLength)
{
	UCHAR               nCheck;
	PCOMMON_HEADER_2    header;
	int					nHdrLen = sizeof(COMMON_HEADER_2)-1;

	if( nLength < nHdrLen )
		return FALSE;

retry_vaild:
	header = (PCOMMON_HEADER_2)pBuffer;
	nCheck = CBuildFrame::F1_GetXorValue(pBuffer, nHdrLen - 1);
	if( nCheck != header->h_check )
	{
invaild_header:
		nLength --;
		if( nLength == 0 )
			return FALSE;

		memcpy(pBuffer, &pBuffer[1], nLength);
		if( GetFrameHeaderForType2(pBuffer, nLength) == FALSE )
			return FALSE;
		goto retry_vaild;
	}

	if( (ULONG)(header->d_len + nHdrLen) > nLength )
		return FALSE;  

	nCheck = CBuildFrame::F1_GetXorValue((PUCHAR)header->data, header->d_len);
	if( nCheck != header->d_check )
		goto invaild_header;

	return TRUE;

}

BOOL CFrame::PickupFrameForType2(PUCHAR  pBuffer, ULONG  &nLength, PROGRAM_OBJECT  TargetObject)
{
	int   nContentLength;
	char  Content[512];

	while( TRUE )
	{
		if( GetFrameHeaderForType2(pBuffer, nLength) == FALSE )
			break;

		if( IsVaildFrameForType2(pBuffer, nLength) == FALSE )
			break;

		nContentLength = CBuildFrame::F3_ParseCmdData((PCHAR)pBuffer, nLength, Content);
		AnsyFrameRequest((PUCHAR)Content, nContentLength, TRUE, TargetObject);
	}

	return TRUE;
}

BOOL CFrame::PickupFrameForType3(PUCHAR pBuffer, ULONG &nLength, PROGRAM_OBJECT TargetObject)
{
	int   nContentLength;
	char  Content[512];

	while( TRUE )
	{
		if( GetFrameHeaderForType2(pBuffer, nLength) == FALSE )
			break;

		if( IsVaildFrameForType3(pBuffer, nLength) == FALSE )
			break;

		nContentLength = CBuildFrame::F3_ParseCmdData((PCHAR)pBuffer, nLength, Content);
		AnsyFrameRequest((PUCHAR)Content, nContentLength, TRUE, TargetObject);
	}
 
	return TRUE;
}

BOOL CFrame::TestPickupFrameForType2(PUCHAR  pBuffer, ULONG  &nLength, ULONG &nRightNum)
{
	int   nContentLength;
	char  Content[512];
	
	while( TRUE )
	{
		if( GetFrameHeaderForType2(pBuffer, nLength) == FALSE )
			break;

		if( IsVaildFrameForType2(pBuffer, nLength) == FALSE )
			break;

		nRightNum++;
		
		nContentLength = CBuildFrame::F2_ParseCmdData((PCHAR)pBuffer, nLength, Content);
		
	}
	
	return TRUE;
}