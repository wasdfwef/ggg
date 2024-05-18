#ifndef    _BUILD_FRAME_INCLUDE
#define    _BUILD_FRAME_INCLUDE

class   CBuildFrame
{

#define  XOR_VERIFY_WAY   0X1
#define  SUM_VERIFY_WAY   0X2

#define  F1_HEADER_CHAR   0XFF
#define  F1_TAIL_CHAR     0XFE
#define  F1_MAP_CHAR      0XFD

#pragma pack(1)

	typedef struct _FRAME_HEADER_TYPE1
	{
		UCHAR     FrameHeader;
		UCHAR     FrameCMD;
	}FRAME_HEADER_TYPE1, *PFRAME_HEADER_TYPE1;
	
	typedef struct _FRAME_TAIL_TYPE1
	{
		UCHAR    nXorCode;
		UCHAR    FrameTail;
		
	}FRAME_TAIL_TYPE1, *PFRAME_TAIL_TYPE1;

	typedef struct _FRAME_TAIL_TYPE2
	{
		USHORT   nSumCode;
		UCHAR    FrameTail;
		
	}FRAME_TAIL_TYPE2, *PFRAME_TAIL_TYPE2;

	typedef struct _FRAME1_FULL_COMMAND
	{
		FRAME_HEADER_TYPE1     header;
		FRAME_TAIL_TYPE1       tail;
		
	}FRAME1_FULL_COMMAND, *PFRAME1_FULL_COMMAND;

#pragma pack()


	int     F1_Encode(PUCHAR pSrc, PUCHAR   pDes, int SrcLength);
	int     F1_Decode(PUCHAR  pSrc, PUCHAR   pDes, int SrcLength);
	
	UCHAR   m_LowRange,  m_UpRange;
	
public:
	UCHAR   m_HeaderFlag, m_TailFalg, m_MapFlag;
	UCHAR   m_CmdOffset,  m_Frame_Verify_Way;

public:
	int     F1_GetBuildCmdBufferLength(int nBufferLength);
	int     F1_GetParseCmdBuferLength(int nCmdBufferLength);
	int     F1_BuildCmdData(PCHAR  pOrgContent, int nLength, PCHAR  pBuildCmdBuffer);
	int     F1_ParseCmdData(PCHAR  pCmdBuffer, int nLength, PCHAR  pContent);
	int     F2_ParseCmdData(PCHAR  pCmdBuffer, ULONG &nLength, PCHAR  pContent);
	int     F2_BuildCmdData(PCHAR  pOrgContent, int nLength, PCHAR  pBuildCmdBuffer);
	int     F3_ParseCmdData(PCHAR  pCmdBuffer, ULONG &nLength, PCHAR  pContent);
	int     F3_BuildCmdData(PCHAR  pOrgContent, int nLength, PCHAR  pBuildCmdBuffer);
	void    SetFrameFlag(PCHAR  pCfgPath);
	UCHAR   F1_GetXorValue(PUCHAR p, int nLen);
	USHORT  F1_CheckSumValue(PUCHAR p, int nLen);
	
};



#endif