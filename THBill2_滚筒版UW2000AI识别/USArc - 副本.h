#ifndef   _USARC_INCLUDE
#define   _USARC_INCLUDE

#include "..\inc\THFrameDef.h"
#include "interal.h"
#include "QueueManager.h"
#include "..\inc\AnalysePic10x.h"
#include "BillHelper.h"

#include <vector>
#include <stack>

#define	DET_MAX_CELLS_COUNT_SUPPORT		256

enum CMotorType
{
	XMotorID = 5,
	YMotorID = 9,
	ZMotorID = 10,
};

enum CDirection
{
	Backward = 0,
	Foreward = 1,
};

typedef struct _FOCUS_PARAMS
{
	float  fGradient;
	float  fSharpness;
	char   nLayer;
	int    nIndex;
	int    nPoor;
}FOCUS_PARAMS,*PFOCUS_PARAMS;


typedef struct _FOCUS_LAYERS
{
	int nValue;
	int num;
}FOCUS_LAYERS;

typedef struct CellInfo
{
	int score;
	int left;
	int top;
	int right;
	int bottom;
};

typedef struct AllCellInfo1 {
	CHAR             nRecCell20xRet;	
	CHAR             nRecCell20xSplitRet;
	int HCount;
	struct CellInfo H[DET_MAX_CELLS_COUNT_SUPPORT];
	int BCount;
	struct CellInfo B[DET_MAX_CELLS_COUNT_SUPPORT];
	int YHCount;
	struct CellInfo YH[DET_MAX_CELLS_COUNT_SUPPORT];
	int SPCount;
	struct CellInfo SP[DET_MAX_CELLS_COUNT_SUPPORT];
	int TSCount;
	struct CellInfo TS[DET_MAX_CELLS_COUNT_SUPPORT];
	int XBTCount;
	struct CellInfo XBT[DET_MAX_CELLS_COUNT_SUPPORT];
	int XYCount;
	struct CellInfo XY[DET_MAX_CELLS_COUNT_SUPPORT];
	int MJCount;
	struct CellInfo MJ[DET_MAX_CELLS_COUNT_SUPPORT];
	int CSCount;
	struct CellInfo CS[DET_MAX_CELLS_COUNT_SUPPORT];
	int CSJCount;
	struct CellInfo CSJ[DET_MAX_CELLS_COUNT_SUPPORT];
};

typedef struct _LAYER_COUNT
{	
	char    nLayer;
	char    nCount;
}LAYER_COUNT,*PLAYER_COUNT;

class  CUSArc
{
	HINSTANCE        m_hDll;
	std::vector<FOCUS_PARAMS> m_vecFocus;
	std::vector<FOCUS_LAYERS> m_vecLayers;

	LONG             m_ExitThread;
	HANDLE           m_hPauseEvent;
	HANDLE           m_Request_Event;
	HANDLE           m_work_thread;
	CBillHelper     *m_BillHelper;
	QUEUE_HANDLE     m_USSampleTaskQue;
	PROGRAM_OBJECT   m_BillPrgObj;

	static   void    USWorkThread(PVOID  pContext);
	void     USSampleOne( PFINISH_STANDING_INFO pStandInfo );
	void     LCUSSampleFinish( PROGRAM_OBJECT lc_obj, UCHAR  nChannel);
	KSTATUS  Move10x(PROGRAM_OBJECT  lc_obj, UCHAR  n10xIndex, PFINISH_STANDING_INFO  finish_info);
	KSTATUS  Move40x(PROGRAM_OBJECT  lc_obj, UCHAR  nChannel, PSAMPLE_40X_INFO s_40);

	KSTATUS  MoveZ(PROGRAM_OBJECT  lc_obj,UCHAR Direction,WORD StepCount);
	KSTATUS  MoveZBack(PROGRAM_OBJECT lc_obj);
	

	KSTATUS  Sample10xGrapControl( PROGRAM_OBJECT  lc_obj, PROGRAM_OBJECT  ca_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info , HANDLE  &hA10x);
	KSTATUS  Sample40xGrapControl( PROGRAM_OBJECT lc_obj, PROGRAM_OBJECT ca_obj, PROGRAM_OBJECT rec_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info, HANDLE hA10x );
	KSTATUS  CaptureCameraGrap(PROGRAM_OBJECT CaObj,UCHAR  nCaIndex,PCHAR  pFilePath);


	HANDLE           m_RecQue;
	ULONG            m_nFocusID;
	int              m_nExperienceLayer,m_nTempLayer;
	BOOL             m_bFindBAST;


public:
	CUSArc();

	void     USSampleControl();
	void     StopUSArc();
	
	KSTATUS  InitializeUSArc(PROGRAM_OBJECT  BillPrgObj, CBillHelper*pBillHelper);
	KSTATUS  StartUSEvent(PFINISH_STANDING_INFO pStandInfo);
	KSTATUS  ControlUSSample(BOOL  bPause);
	
	char     focusini[MAX_PATH];
	char	 m_FocusIniPath[MAX_PATH];
	int				 m_nLayer ;
	int				 m_nLayerStep;
	int              m_nTotalFocusLyar;
	BOOL     LoadInfo();
	void     FindCell(int type,int num,PCHAR Path);//找细胞多的位置
	BOOL   CutPic(PCHAR pPicPath,PCHAR pFileName,int nRow,int nCol);
	int    IdentifyEX(PCHAR pPicPath,int nRow,int nCol,int nMAXLayer);

	void   GetFocusResultEX(ULONG nID);
	BOOL     GetFocusResult(PROGRAM_OBJECT rec_obj,ULONG nID);
	BOOL     GetFocusPostResult(ULONG nID);
	int      GetCleanNum(int nLayerCount,std::vector<FOCUS_PARAMS> vecFocus,int nDirection);
	int    GetFocusResultByAIRec(PROGRAM_OBJECT rec_obj,ULONG nID,PCHAR pFocusPath,int nCleanLayer);
	void   GetAiRecResult(PCHAR path,int nCleanLayer,AllCellInfo1 & CellImg,int & ret);
	int    GetCleanLayer(PCHAR pPicPath,PCHAR pIniPath,RECT CutRect,int nBegin,int nEnd,int nCleanLayer,int nCellIndex,BOOL bFocusEP = FALSE);

};


#endif