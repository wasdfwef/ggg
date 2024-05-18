#ifndef   _USARC_INCLUDE
#define   _USARC_INCLUDE

#include "..\..\inc\THFrameDef.h"
#include "interal.h"
#include "QueueManager.h"
#include "..\..\inc\AnalysePic10x.h"
#include "BillHelper.h"
#include "Threadpool.h"
#include <vector>
#include <stack>
#include <map>
#include "opencv2/opencv.hpp"
#include <Shlwapi.h>
#include <iostream>

#pragma comment(lib, "Shlwapi.lib")

 using namespace std;
 using namespace cv;



using namespace TOOLUTIL;

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
	int num;
	int nValue;	
}FOCUS_LAYERS;

typedef struct CellInfo
{
	int score;
	int left;
	int top;
	int right;
	int bottom;
};

typedef struct _N_PIC_TARGET_TASK
{
	int nID;
	int picCountActually;
}N_PIC_TARGET_TASK,PN_PIC_TARGET_TASK;


/*
天海尿液标注名称：
1.白细胞      ：b
2.细胞团      ：xbt
3.吞噬细胞    ：ts
4.红细胞      ：h
5.异形红细胞  ：yh
6.上皮细胞    :sp
7.小圆上皮细胞：xy
8.霉菌        :mj
9.草酸钙结晶  ：cs
10类似红细胞草酸钙结晶：csj
11磷酸铵镁结晶：ls
12尿酸结晶    ：ns
13透明管型    ：tm
14颗粒管型    ：kl
15粘液丝      ：ny
*/
/*
天海尿液标注名称[2020.12.09新20x识别]：
1.白细胞: b
2.红细胞: h
3.异形红细胞:yxh
4.白细胞团:  bxbt
5.上皮细胞:  sp
6.粘液丝:    nys
7.透明管型:  tmg
8.草酸钙结晶:csgj
9.类似红细胞草酸钙结晶: csj
10尿酸结晶: nsj
11磷酸铵镁结晶: lsj
12霉菌:    mj
*/
typedef struct AllCellsInfo {
	int SPCount;
	struct CellInfo SP[DET_MAX_CELLS_COUNT_SUPPORT]; // 上皮细胞 sp
	int TSCount;
	struct CellInfo TS[DET_MAX_CELLS_COUNT_SUPPORT]; // 吞噬细胞 ts
	int XBTCount;
	struct CellInfo XBT[DET_MAX_CELLS_COUNT_SUPPORT];// 细胞团/白细胞团 xbt/bxbt
	int XYCount;
	struct CellInfo XY[DET_MAX_CELLS_COUNT_SUPPORT]; // 小圆上皮细胞 xy
	int MJCount;
	struct CellInfo MJ[DET_MAX_CELLS_COUNT_SUPPORT]; // 菌丝 mj
	int CSCount;
	struct CellInfo CS[DET_MAX_CELLS_COUNT_SUPPORT]; // 草酸钙结晶 cs/csgj
	int CSJCount;
	struct CellInfo CSJ[DET_MAX_CELLS_COUNT_SUPPORT];// 类似红细胞草酸钙结晶 csj
	int NSCount;
	struct CellInfo NS[DET_MAX_CELLS_COUNT_SUPPORT]; // 尿酸结晶 ns/nsj
	int LSCount;
	struct CellInfo LS[DET_MAX_CELLS_COUNT_SUPPORT]; // 磷酸铵镁结晶 ls/lsj
	int KLCount;
	struct CellInfo KL[DET_MAX_CELLS_COUNT_SUPPORT]; // 颗粒管型 kl
	int TMCount;
	struct CellInfo TM[DET_MAX_CELLS_COUNT_SUPPORT]; // 透明管型 tm/tmg
	int NYCount;
	struct CellInfo NY[DET_MAX_CELLS_COUNT_SUPPORT]; // 粘液丝 ny/nys
	int GJCount;
	struct CellInfo GJ[DET_MAX_CELLS_COUNT_SUPPORT]; // 杆菌 gj
	int JZCount;
	struct CellInfo JZ[DET_MAX_CELLS_COUNT_SUPPORT]; // 精子 jz
	int HCount;
	struct CellInfo H[DET_MAX_CELLS_COUNT_SUPPORT];  // 白细胞 h
	int BCount;
	struct CellInfo B[DET_MAX_CELLS_COUNT_SUPPORT];  // 白细胞 b
	int YHCount;
	struct CellInfo YH[DET_MAX_CELLS_COUNT_SUPPORT]; // 异形红细胞 yh/yxh
	int QCount;
	struct CellInfo Q[DET_MAX_CELLS_COUNT_SUPPORT]; // 异形红细胞 q
};

typedef struct CellsIn20x {

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
	int NSCount;
	struct CellInfo NS[DET_MAX_CELLS_COUNT_SUPPORT];
	int LSCount;
	struct CellInfo LS[DET_MAX_CELLS_COUNT_SUPPORT];
	int KLCount;
	struct CellInfo KL[DET_MAX_CELLS_COUNT_SUPPORT];
	int TMCount;
	struct CellInfo TM[DET_MAX_CELLS_COUNT_SUPPORT];
	int NYCount;
	struct CellInfo NY[DET_MAX_CELLS_COUNT_SUPPORT];
	int HCount;
	struct CellInfo H[DET_MAX_CELLS_COUNT_SUPPORT];
	int BCount;
	struct CellInfo B[DET_MAX_CELLS_COUNT_SUPPORT];
	int YHCount;
	struct CellInfo YH[DET_MAX_CELLS_COUNT_SUPPORT];
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

typedef struct _FINISH_STANDING_INFO
{
	UCHAR HoleNum;
	UCHAR ChannelNum;
	WORD TaskID;
}FINISH_STANDING_INFO, *PFINISH_STANDING_INFO;

typedef struct _LOCATE_10_MICROSCOPE_INFO
{
	UCHAR XPicNum;
	UCHAR YPicNum;
	UCHAR ChannelNum;
	UCHAR IsReturnTakePicCommand;    //yes: 1; no: 0
}LOCATE_10_MICROSCOPE_INFO, *PLOCATE_10_MICROSCOPE_INFO;


typedef struct _LC_LOCATE_10_MICROSCOPE_INFO
{
	LC_HEADER   lc_header;
	LOCATE_10_MICROSCOPE_INFO LocateInfo;

}LC_LOCATE_10_MICROSCOPE_INFO, *PLC_LOCATE_10_MICROSCOPE_INFO;

#pragma pack(1)

typedef struct {
	std::mutex mutex;
	int nSample40xCounter;     // 40倍采图总数
	int nTotalCounter;        // 40倍有细胞的图片总数
	int nRecCompleteCount;     // 识别完成的数量
}ASYNC_REC_CONTEXT, *PASYNC_REC_CONTEXT;

typedef struct
{
	PASYNC_REC_CONTEXT recCtx;
	void* pContext;
	void* pContext2;
	void* pContext3;
	REC_GRAP_FULL_RECORD rgfr;
}REC_GRAP_CONTEXT_WARP, *PREC_GRAP_CONTEXT_WARP;

#pragma pack()

//
typedef struct tagFOCUSIMAGE
{
	string  path;     // 图片路径
	Mat		image;    // 图片
    HBITMAP	hImage;    // 图片
	double	gradient; // 梯度
} FOCUSIMAGE, *PFOCUSIMAGE;

class  CUSArc
{
	friend VOID WINAPI PRP_REC_COMPLETE_CALLBACK(PRP prp, PVOID pContext, KSTATUS status);
	friend VOID PRP_REC_COMPLETE_CALLBACK2(CUSArc *pUsArc, PREC_GRAP_CONTEXT_WARP pContext);

	using _Point = struct{
		int x;
		int y;
	};

	HINSTANCE        m_hDll;
	std::vector<FOCUS_PARAMS> m_vecFocus;
	std::vector<FOCUS_LAYERS> m_vecLayers;
	std::vector<FOCUS_LAYERS> m_vecBvalue;//保存黑色像素点个数

	std::vector<N_PIC_TARGET_TASK> m_vecNpicTask;
	LONG             m_ExitThread;
	HANDLE           m_hPauseEvent;
	HANDLE           m_Request_Event;
	HANDLE           m_work_thread;
	LONG			 m_LCPicHaveEnd;
	CBillHelper     *m_BillHelper;
	QUEUE_HANDLE     m_USSampleTaskQue;
	PROGRAM_OBJECT   m_BillPrgObj;

	HANDLE           m_Barcode_Request_Event;
	QUEUE_HANDLE     m_BarcodeSampleTaskQue;

	static   void    USWorkThread(PVOID  pContext);
	void     USSampleOne( PFINISH_STANDING_INFO pStandInfo );
	void     LCUSSampleFinish( PROGRAM_OBJECT lc_obj, UCHAR  nChannel);
	KSTATUS  Move10x(PROGRAM_OBJECT  lc_obj, UCHAR  n10xIndex, PFINISH_STANDING_INFO  finish_info);
	KSTATUS  Move40x(PROGRAM_OBJECT  lc_obj, UCHAR  nChannel, PSAMPLE_40X_INFO s_40);
	KSTATUS  Move40x(PROGRAM_OBJECT  lc_obj, UCHAR  nChannel, UCHAR nX, UCHAR nY);

	KSTATUS StartMoveNewRow(PROGRAM_OBJECT lc_obj, UCHAR nCha, UCHAR nRow, uint16_t hSpeed);
	KSTATUS EndMoveNewRow(PROGRAM_OBJECT lc_obj, UCHAR nCha, UCHAR nRow);
	KSTATUS  StartFocus(PROGRAM_OBJECT lc_obj);
	KSTATUS  StopFocus(PROGRAM_OBJECT lc_obj);

	KSTATUS  MoveZ(PROGRAM_OBJECT  lc_obj,UCHAR Direction,WORD StepCount);
	KSTATUS  MoveZBack(PROGRAM_OBJECT lc_obj);
	KSTATUS  SaveZ(PROGRAM_OBJECT lc_obj);

	KSTATUS  Sample10xGrapControl( PROGRAM_OBJECT  lc_obj, PROGRAM_OBJECT  ca_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info , HANDLE  &hA10x);
	KSTATUS  Sample40xGrapControl(PROGRAM_OBJECT lc_obj, PROGRAM_OBJECT ca_obj, PROGRAM_OBJECT rec_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info);
	KSTATUS  Sample40xGrapControl_noPrecipitate(PROGRAM_OBJECT lc_obj, PROGRAM_OBJECT ca_obj, PROGRAM_OBJECT rec_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info);
	KSTATUS  CaptureCameraGrap(PROGRAM_OBJECT CaObj,UCHAR  nCaIndex,PCHAR  pFilePath);
	KSTATUS  CaptureCameraGrapBuffer(PROGRAM_OBJECT CaObj, UCHAR  nCaIndex, HBITMAP &hBitmap);
	KSTATUS  CaptureCameraGrapBuffer(PROGRAM_OBJECT CaObj, UCHAR  nCaIndex, PCHAR  pFilePath, FRAME_FILE_INFO &ffi);
	KSTATUS  SaveCaptureCameraGrapBuffer(PROGRAM_OBJECT CaObj, FRAME_FILE_INFO &ffi);

	void     BarcodeReco(PBARCODE_REQ_CONTEXT ctx); // 性状条码识别
	KSTATUS  RotatioNextAngle(PROGRAM_OBJECT lc_obj); // 选择到下一角度
	KSTATUS  BarcodeRecoFinish(PROGRAM_OBJECT lc_obj);// 性状条码识别完成

	void	 GetAllPicCells(std::map<int, AllCellsInfo> &AllPicCells, PCHAR szIniPath);

	bool     LocationCellPos(HBITMAP hBitmap, std::vector<cv::Rect> &contours);
	bool     LocationCellPos(HBITMAP hBitmap, std::vector<cv::Rect> &contours,PCHAR  pFilePath);

	HANDLE           m_RecQue;
	ULONG            m_nFocusID;
	int              m_nExperienceLayer,m_nTempLayer;
	BOOL             m_bFindBAST;
	MOVE_NEW_ROW_REQ m_PicHaveEndRet;
	Threadpool		 m_worker;
	Threadpool		 m_SavePicWorker;

public:
	CUSArc();
	bool	 m_PicHaveEndThenPicNew;
	void     USSampleControl();
	void     BarcodeSampleControl();
	void     StopUSArc();
	LONG	 m_picCountActually;
	KSTATUS  InitializeUSArc(PROGRAM_OBJECT  BillPrgObj, CBillHelper*pBillHelper);
	KSTATUS  StartUSEvent(PFINISH_STANDING_INFO pStandInfo);
	KSTATUS  ControlUSSample(BOOL  bPause);

	KSTATUS  StartBarcodeEvent(PBARCODE_REQ_CONTEXT ctx);
	
	char     focusini[MAX_PATH];
	char	 m_FocusIniPath[MAX_PATH];
	int				 m_nLayer ;
	int				 m_nLayerStep;
	int              m_nTotalFocusLyar;
	BOOL     LoadInfo();
	void     FindCell(int type,int num,PCHAR Path,PFOCUS_PARAMS params, BOOL bCache = TRUE);//找细胞多的位置
	void     FindCell(int num, HBITMAP hBitmap, PFOCUS_PARAMS params, FOCUSIMAGE &fi, BOOL bCache = TRUE);//找细胞多的位置
	void     BinarizaFindCell(int num,PCHAR Path);//二值化找细胞
	BOOL	CutPic(PCHAR pPicPath,PCHAR pFileName,int nRow,int nCol);
	int		IdentifyEX(PCHAR pPicPath,int nRow,int nCol,int nMAXLayer);

	void	GetFocusResultEX(ULONG nID);
	BOOL     GetFocusResult(PROGRAM_OBJECT rec_obj, ULONG nID, vector<FOCUSIMAGE> &vecFi, BOOL &bFocusBySmallCell);
	BOOL     GetBeforeFocusResult(PCHAR szPicPath, int &nPos);
	BOOL     GetBeforeFocusResult(PCHAR szPicPath, int &nPos, std::map<int, _Point> &mapBFocusPos, std::map<int, std::vector<cv::Rect>> &mapBFocusResult);
	BOOL     GetFocusPostResult(ULONG nID);
	int      GetClearNum(int nLayerCount,std::vector<FOCUS_PARAMS> vecFocus,int nDirection);
	int    GetFocusResultByAIRec(PROGRAM_OBJECT rec_obj,ULONG nID,PCHAR pFocusPath,int nCleanLayer, vector<FOCUSIMAGE> &vecFi, BOOL &bFocusBySmallCell);
	void   GetAiRecResult(PCHAR path,int nCleanLayer,AllCellInfo1 & CellImg,int & ret);
	void   GetAiRecResult(PCHAR path, int nCleanLayer, CellsIn20x & CellImg, int & ret);
	void   GetAiRecResult(PCHAR path, int nCleanLayer, AllCellsInfo & Cells, int & ret);
	bool   GetAiRecResult(PCHAR szPicPath, int nPicCount, AllCellsInfo & Cells);
	int    GetCleanLayer(PCHAR pPicPath,PCHAR pIniPath,RECT CutRect,int nBegin,int nEnd,int nCleanLayer,int nCellIndex,BOOL bFocusEP = FALSE);
	int    GetCleanLayer(vector<FOCUSIMAGE> &vecFi, PCHAR pPicPath, PCHAR pIniPath, RECT CutRect, int nBegin, int nEnd, int nCleanLayer, int nCellIndex, BOOL bFocusEP = FALSE);
	KSTATUS USStartNewRow(PMOVE_NEW_ROW_REQ);
	BOOL QueryLCPicHaveEnd();
	void ResetLCPicHaveEnd();
	int PixelCounter(Mat src, int nflag);
};


#endif