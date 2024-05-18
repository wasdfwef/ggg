#include "StdAfx.h"
#include "USArc.h"
#include "DbgMsg.h"
#include <shlwapi.h>
#include <MATH.H>
#include <malloc.h>
#include <memory>
#include <mutex>
#include <map>
#include <future>
#include "THBarcodeRecDLL.h"
#include <sstream>
#include <exception>
#include "ximage.h"

#pragma comment(lib, "..\\..\\lib\\cximage.lib")
#pragma comment(lib, "..\\..\\lib\\png.lib")
#pragma comment(lib, "..\\..\\lib\\Jpeg.lib")
#pragma comment(lib, "..\\..\\lib\\zlib.lib")
#include "utils/ByteUtil.h"
#include <atlimage.h>
#include <atomic>

using namespace cv;

#define max2(a, b) ( (a) > (b) ) ?  (a) : (b)
#define max3(a,b,c) ( ( (a) > (b) ) ?  max2( a, c) : max2( b, c ) )

#define min2(a, b)  ( (a) < (b) ) ?  (a) : (b)
#define min3(a,b,c) ( ( (a) < (b) ) ?  min2( a, c ) : min2( b, c ) )

// #define NO_PRECIPITATE_PIC
// 特殊情况：版本为2.3但不需要高速采图
//#define   LOC_GRADIDENT_AND_AI

/*
#if (_LC_V <= 2190)
#define   LOC_GRADIDENT_AND_AI
#elif (_LC_V >= 2200)
#define	  LOC_BINAIIZE
#define	  HSPEED_PIC
#else
#endif
*/

/////////2022.02.14//////
/////////默认情况下软件版本大于2190就是高速采图版本
#if (_LC_V > 2190)
	BOOL g_isHspeedCaptureImage = TRUE; // 可以在配置里修改（高速/低速）
	BOOL g_isBibaiize = TRUE;
#else 
//#	define LOC_GRADIDENT_AND_AI
	BOOL g_isHspeedCaptureImage = FALSE;
	BOOL g_isBibaiize = FALSE;
#endif
///////////////////////

#define	  ASYNC_REC

#define   REC_GRAP_PATH               "RecGrapReslut"
#define   ADDITION_INFO_FILE          "Addition.ini"
#define   SECTION_10X                 "10xInfo"
#define   SECTION_40X                 "40xInfo"
#define   COUNTER_KEY                 "Count"
#define   CELL_COUNT_KEY              "CellCount"
#define   RESULT_FILE				  "Result.ini"


#define RETURN_SAMPLE_CMD 1
#define NOT_RETURN_SAMPLE_CMD 0


#define LC_DETECT_PIPE_ON         0X0000002C
#define LC_DETECT_PIPE_OFF        0X0000002D
#define LC_START_SAMPLING         0X0000002E				 //开始采图
#define LC_REPORT_ACTION          0X0000002F
#define LC_RETRY_ACTION           0X00000030
#define LC_SAMPLE_FINISH          0X00000031				 //采图完成
#define LC_MOVE_POS               0X00000032			     //移动采图区域
#define LC_Z_ADJUST               0X00000033			     //Z轴调焦
#define LC_START_BD				  0X00000034
#define LC_AUTO_DETECT            0X00000035			     //下位机自动检测串口配置
#define LC_READ_CPU_CARD_INFO	  0X00000036	
#define LC_START_UDC_CHECK		  0X00000037	
#define LC_BARCODE   			  0X00000038	
#define LC_QUERY_Z_POS			  0X00000039	
#define LC_RETURN_XY_40_MICRO_DEVIA		  0X0000003A	
#define LC_SEND_TASKID			  0X0000003B	
#define LC_LOCATE_10_MICROSCOPE   0X0000003C
#define LC_LOCATE_40_MICROSCOPE   0X0000003D
#define LC_ENABLE_AUTO_CHECK	  0X0000003E
#define LC_SPECIAL_TEST			  0X0000003F
#define LC_MOVE_SCAN_PLATFORM     0X00000040
#define LC_CONTROL_LIGHT		  0X00000041
#define LC_SWITCH_CHANNEL		  0X00000042
#define LC_SWITCH_MICROSCOPE	  0X00000043
#define LC_TEST_HARDWARE		  0X00000044
#define LC_CLEAR_TROUBLE		  0X00000045
#define LC_QUERY_LC_PARAMETERS    0X00000046
#define LC_SET_LC_PARAMETERS	  0X00000047
#define LC_UDC_CORRECT_MODULE	  0X00000048
#define LC_UDC_TEST_PAPER		  0X00000049
#define LC_UDC_SPECIFY_PAPER_TYPE 0X0000004A
#define LC_UDC_CONTROL_LIGHT	  0X0000004B
#define LC_SELF_CHECK			  0X0000004C
#define LC_DISABLE_AUTO_CHECK	  0X0000004D
#define LC_RESET_X				  0X0000004E
#define LC_RESET_Y				  0X0000004F
#define LC_RESET_Z				  0X00000050
#define LC_ENTER_HARDWARE_DEBUG   0X00000051
#define LC_EXIT_HARDWARE_DEBUG    0X00000052
#define LC_RESTART_LC			  0X00000053
#define LC_MOVE_Z				  0X00000054
#define LC_UDC_CORRECT_MODULE_EX  0X00000055

#define LC_SCAN_BARCODE           0X00000056
#define LC_STOP_SCAN_BARCODE      0X00000057
#define LC_FOCUS_Z                0X00000058
#define LC_GET_AD                 0X00000059
#define LC_CLEAR_MAINTAIN         0X0000005A 
#define LC_ALLOW_TEST             0X0000005B

#define LC_CONSUMABLE_SURPLUS     0X0000005C

#pragma pack(1)
struct LOCATE_40_MICROSCOPE_INFO
{
	UCHAR XPicNumFor10;
	UCHAR YPicNumFor10;
	UCHAR PicNumFor40;
	UCHAR XOffset;
	UCHAR YOffset;
	UCHAR ChannelNum;
	UCHAR IsReturnTakePicCommand;    //yes: 1; no: 0
};
typedef LOCATE_40_MICROSCOPE_INFO *PLOCATE_40_MICROSCOPE_INFO;



typedef struct _LC_LOCATE_40_MICROSCOPE_INFO
{
	LC_HEADER   lc_header;
	LOCATE_40_MICROSCOPE_INFO LocateInfo;

}LC_LOCATE_40_MICROSCOPE_INFO, *PLC_LOCATE_40_MICROSCOPE_INFO;

struct MOVE_SCAN_PLATFORM_INFO
{
	UCHAR MotorID;                  //X:5；Y:9；Z:10
	UCHAR Direction;
	UCHAR StepCountLow;
	UCHAR StepCountHigh;
};
typedef MOVE_SCAN_PLATFORM_INFO *PMOVE_SCAN_PLATFORM_INFO;

typedef struct _LC_MOVE_PLATFORM_INFO
{
	LC_HEADER   lc_header;
	MOVE_SCAN_PLATFORM_INFO MoveInfo;

}LC_MOVE_PLATFORM_INFO, *PLC_MOVE_PLATFORM_INFO;
struct MOVE_Z_AXIS
{
	UCHAR StepCountLow;
	UCHAR StepCountHigh;
};
typedef MOVE_Z_AXIS   *PMOVE_Z_AXIS;


typedef struct _LC_MOVE_Z_AXIS
{
	LC_HEADER   lc_header;
	MOVE_Z_AXIS MoveInfo;

}LC_MOVE_Z_AXIS, *PLC_MOVE_Z_AXIS;
struct FINISH_TAKE_PIC_INFO
{
	UCHAR ChannelNum;
};
typedef FINISH_TAKE_PIC_INFO *PFINISH_TAKE_PIC_INFO;

typedef struct _LC_SAMPLE_FINISH_TYPE
{
	LC_HEADER    lc_header;
	FINISH_TAKE_PIC_INFO       FinishSampleInfo;

}LC_SAMPLE_FINISH_TYPE, *PLC_SAMPLE_FINISH_TYPE;



#pragma pack()

// 图像质量
typedef struct tagIMAGEQUALITY
{
	float            fBrightness;        // 亮  度
	float            fWhiteBalance;      // 白平衡
	float            fGradient;          // 梯  度
	float            fSharpness;         // 清晰度
	float            fPollution;         // 污染度
	unsigned int     nCount;             // 粒子数
	unsigned int     nErrorCode;         // 错误码
} IMAGEQUALITY, *PIMAGEQUALITY;

extern "C" typedef float( *lpRecFun)(const wchar_t *); //宏定义函数指针类型

lpRecFun RecFun;

extern "C" typedef bool( *AnalysisFile)(const wchar_t *lpszImage, IMAGEQUALITY &ImgQuality); //宏定义函数指针类型

AnalysisFile AnalysisFileFun;

#pragma pack(1)

typedef struct _QUERY_TASKID_CONTEXT
{
	PSAMPLING_PROGRESSING_STATUS    tid;
	BOOLEAN                         bRemoveItem;	
	BOOLEAN                         bTackCardPic;
	BOOLEAN                         bScanBarcode;
}QUERY_TASKID_CONTEXT, *PQUERY_TASKID_CONTEXT;

typedef  struct _REC_GRAP_CONTEXT
{
	ULONG     nTaskID;
	ULONG     nGrapIndex;
	UCHAR     nGrapSize;
	HANDLE    hRawGrapBuffer;
	CHAR      sPicPath[MAX_PATH];
	BOOLEAN   bFailSampling;
	UCHAR     nLayer;

}REC_GRAP_CONTEXT, *PREC_GRAP_CONTEXT;

#pragma pack()



//////////////////////////////////////////////////////////////////////////////////////////
class AutoRallThreadpoolPause
{
public:
	AutoRallThreadpoolPause(Threadpool &tp)
		:_tp(tp)
	{
		_tp.pause();
	}
	~AutoRallThreadpoolPause()
	{
		_tp.resume();
	}
	AutoRallThreadpoolPause(const AutoRallThreadpoolPause&) = delete;
	AutoRallThreadpoolPause &operator=(const AutoRallThreadpoolPause&) = delete;
private:
	Threadpool &_tp;
};
//////////////////////////////////////////////////////////////////////////////////////////

BOOL HBitmapToMat(HBITMAP& _hBmp, cv::Mat& _mat) {
	//BITMAP操作
	BITMAP bmp;
	GetObject(_hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	//mat操作
	cv::Mat v_mat;
	v_mat.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
	GetBitmapBits(_hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, v_mat.data);
	_mat = v_mat;
	return TRUE;
}

void fillHole(const cv::Mat srcimage, cv::Mat &dstimage)
{
	cv::Size m_Size = srcimage.size();
	cv::Mat temimage = cv::Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcimage.type());//延展图像  
	srcimage.copyTo(temimage(cv::Range(1, m_Size.height + 1), cv::Range(1, m_Size.width + 1)));
	floodFill(temimage, cv::Point(0, 0), cv::Scalar(255));
	cv::Mat cutImg;//裁剪延展的图像  
	temimage(cv::Range(1, m_Size.height + 1), cv::Range(1, m_Size.width + 1)).copyTo(cutImg);
	dstimage = srcimage | (~cutImg);
}

float lightGray(cv::Mat &img)
{
	cv::Mat imgGray;
	if (img.channels() == 3)
		cv::cvtColor(img, imgGray, CV_BGR2GRAY);
	else
		imgGray = img;
	cv::Scalar grayScalar = cv::mean(imgGray);
	float imgGrayLight = grayScalar.val[0];
	return imgGrayLight;
}

float Tenengrad(cv::Mat &img)
{
	double meanValue = 0.0;
	cv::Mat imgGray;
	if (img.channels() == 3) 
		cv::cvtColor(img, imgGray, CV_BGR2GRAY);
	else 
		imgGray = img;
	cv::Mat imageSobel;
	cv::Sobel(imgGray, imageSobel, CV_16S, 1, 1);
	cv::convertScaleAbs(imageSobel, imageSobel);
	meanValue = cv::mean(imageSobel)[0];

	return meanValue;
}

//////////////////////////////////

VOID WINAPI PRP_REC_COMPLETE_CALLBACK( PRP prp, PVOID pContext, KSTATUS status );

CUSArc::CUSArc()
	: m_worker(std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency()/2 : std::thread::hardware_concurrency())
	, m_SavePicWorker(1)
{
	m_BillPrgObj = NULL;
	m_hPauseEvent = m_Request_Event = NULL;
	m_BillHelper = NULL;
	m_LCPicHaveEnd = 0;
	m_PicHaveEndRet = { 0 };
	m_PicHaveEndThenPicNew = false;
	m_picCountActually = 0;
	LoadInfo();
}

KSTATUS CUSArc::InitializeUSArc( PROGRAM_OBJECT BillPrgObj, CBillHelper*pBillHelper )
{
	DWORD                  trd;
	PBILL3_EXTENSION       pExtension;

	m_USSampleTaskQue = Que_Initialize(sizeof(FINISH_STANDING_INFO));
	if( m_USSampleTaskQue == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;
	
	m_Request_Event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if( m_Request_Event == NULL )
		return STATUS_CREATE_EVENT_FAIL;
	
	m_hPauseEvent   = CreateEvent(NULL, TRUE, TRUE, NULL);
	if( m_hPauseEvent == NULL )
		return STATUS_CREATE_EVENT_FAIL;
	
	m_work_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)USWorkThread, this, 0, &trd);
	if( m_work_thread == NULL )
		return STATUS_CREATE_THREAD_FAIL;

	m_BarcodeSampleTaskQue = Que_Initialize(sizeof(BARCODE_REQ_CONTEXT));
	if (m_BarcodeSampleTaskQue == NULL)
		return STATUS_ALLOC_MEMORY_ERROR;

	m_Barcode_Request_Event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_Barcode_Request_Event == NULL)
		return STATUS_CREATE_EVENT_FAIL;

	FMMC_GetPrgObjectExtension(BillPrgObj, (PVOID*)&pExtension);
	GetModuleFileName(NULL,pExtension->prg_status.GrapPath, MAX_PATH);
	PathRemoveFileSpec(pExtension->prg_status.GrapPath);


	strcpy_s(focusini,pExtension->prg_status.GrapPath);
	PathAppend(focusini,"Config\\Focus.ini");//聚焦配置文件2019
    WritePrivateProfileString(_T("Focus"), _T("EnableQCRec"), _T("0"), focusini);//初始化时重置质控识别标志

	PathAppend(pExtension->prg_status.GrapPath, REC_GRAP_PATH);

	m_BillPrgObj = BillPrgObj;
	m_BillHelper = pBillHelper;

	auto f = std::async(std::launch::async, &CUSArc::BarcodeSampleControl, this);

	m_SavePicWorker.start();
	m_worker.start();

	return STATUS_SUCCESS;
}

void CUSArc::BinarizaFindCell(int num,PCHAR tPath)
{
	char Path[MAX_PATH] = {0};
	memcpy(Path, tPath, MAX_PATH);
	if (!PathFileExists(Path))
	{
		return;
	}

	Mat img = imread(Path);
	if (img.empty())
	{
		DBG_MSG("debug:Error: Could not load image");
		return ;
	}
	
	Mat gray;
	Mat dst;
	cvtColor(img, gray, CV_RGB2GRAY);//先转为灰度图
	// 	Mat dst, dst2, dst3, dst4, dst5;
	// 	threshold(gray, dst, 127, 255, THRESH_BINARY);//二值化阈值处理
	// 	threshold(gray, dst2, 127, 255, THRESH_BINARY_INV);//反二值化阈值处理
	// 	threshold(gray, dst3, 127, 255, THRESH_TRUNC);//截断阈值化处理
	// 	threshold(gray, dst4, 127, 255, THRESH_TOZERO_INV);//超阈值零处理
	// 	threshold(gray, dst5, 127, 255, THRESH_TOZERO);//低阈值零处理

	// 	threshold(gray, dst, 0, 255, THRESH_OTSU);//OTSU
	adaptiveThreshold(gray, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 7, 10);//自适应阈值法
	//threshold(gray, dst, 0, 255, THRESH_OTSU);//OTSU 大津法
	//显示图像
	
// 	imshow("gray_image", gray);
// 	cvResizeWindow("gray_image", 720, 480);
// 	cvMoveWindow("gray_image", 0, 0);
// 	
// 	imshow("THRESH_BINARY", dst);
// 	cvResizeWindow("THRESH_BINARY", 720, 480);
// 	cvMoveWindow("THRESH_BINARY", 720, 0);
// 	imshow("THRESH_BINARY_INV", dst2);
// 	imshow("THRESH_TRUNC", dst3);
// 	imshow("THRESH_TOZERO_INV", dst4);
// 	imshow("THRESH_TOZERO", dst5);
// 	//计算白像素个数
// 	int nWhiteCount = PixelCounter(dst, 1);

	//计算黑像素个数
	int nBlackCount = PixelCounter(dst, 2);
	DBG_MSG("debug:第%d张图黑色像素为%d个", num, nBlackCount);
	
	FOCUS_LAYERS lp = { num,nBlackCount };

	m_vecBvalue.push_back(lp);
	
	PathRemoveFileSpecA(Path);
	char temp[MAX_PATH] = {0};
	sprintf_s(temp,MAX_PATH,"%s\\%d_bin.tif",Path,num);
	
	imwrite(temp,dst);

	//printf("\n白像素:%d\n黑像素:%d\n总像素:%d\n", nWhiteCount, nBlackCount, (nBlackCount + nWhiteCount));
	
}

int CUSArc::PixelCounter(Mat src, int nflag)
{

	int nCount_White = 0;//白
	int nCount_Black = 0;//黑

						 //通过迭代器访问图像的像素点
	Mat_<uchar>::iterator itor = src.begin<uchar>();
	Mat_<uchar>::iterator itorEnd = src.end<uchar>();
	for (; itor != itorEnd; ++itor)
	{
		if ((*itor) > 0)
		{
			//白：像素值 ptr:255
			nCount_White += 1;
		}
		else
		{
			//黑：像素值 ptr:0
			nCount_Black += 1;
		}

	}

	//根据nflag返回黑或白像素个数
	if (nflag == 1)
	{
		//白
		return nCount_White;
	}
	else
	{
		//黑
		return nCount_Black;
	}

}

void CUSArc::USWorkThread( PVOID pContext )
{
	CUSArc *us_arc = (CUSArc*)pContext;
	us_arc->USSampleControl();
}

void CUSArc::USSampleControl()         //US采集控制过程
{
	DBG_MSG(":USSampleControl()//US开始采集控制过程");
	FINISH_STANDING_INFO   finish_stnd_info;

	while( TRUE )
	{
		
		WaitForSingleObject(m_Request_Event, INFINITE);
		DBG_MSG("WaitForSingleObject(m_Request_Event, INFINITE) 开始采图;");
		if( m_BillHelper->QueryThreadExit() )
			break;

		while (Que_GetHeaderData(m_USSampleTaskQue, &finish_stnd_info))
		{
			USSampleOne(&finish_stnd_info);
		}
	}
}

void CUSArc::BarcodeSampleControl()
{
	BARCODE_REQ_CONTEXT   ctx;

	while (TRUE)
	{
		WaitForSingleObject(m_Barcode_Request_Event, INFINITE);

		if (m_BillHelper->QueryThreadExit())
			break;

		while (Que_GetHeaderData(m_BarcodeSampleTaskQue, &ctx))
		{
			BarcodeReco(&ctx);
		}
	}
}

// 采图
/*
void CUSArc::USSampleOne( PFINISH_STANDING_INFO pStandInfo )
{
	ULONG                  n40xCounter, nTotalCounter;
	HANDLE                 hA10x = NULL;
	KSTATUS                status;
	PROGRAM_OBJECT         lc_obj, ca_obj, rec_obj;
	PBILL3_EXTENSION       pExtension;
	SAMPLING_INFORMATION   s_info = {0};
	
	FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
	EnterCriticalSection(&pExtension->cs);
	lc_obj   = pExtension->o_dev.s_lc_dev;
	ca_obj   = pExtension->o_dev.s_ca_dev;
	rec_obj  = pExtension->o_dev.s_rec_dev;
	LeaveCriticalSection(&pExtension->cs);
	
/ *	status = m_BillHelper->UpdateUSStatus(pStandInfo->TaskID, &bFullTaskFinish);
	if( status != STATUS_SUCCESS )
	{
		//由于缓冲队列中没有该记录,则不需要发送消息到UI
		DBG_MSG("StartSampleUSTask->UpdateUSStatus Not Found nTaskID = %u\n", pStandInfo->TaskID);
		LCUSSampleFinish(lc_obj, pStandInfo->ChannelNum);
		return;
	}* /

	DBG_MSG("开始10x倍镜采图...\n");
	
	status = Sample10xGrapControl(lc_obj, ca_obj, pExtension, pStandInfo, hA10x);
	if( status != STATUS_SUCCESS )
		goto s_finish_loc;

	DBG_MSG("开始40x倍镜采图...\n");

	n40xCounter = AnalysePic10x_GetSample40xGrapCounter(hA10x);
	nTotalCounter = AnalysePic10x_GetSample40xTotaleGrapCounter(hA10x);
	
	status = Sample40xGrapControl(lc_obj, ca_obj, rec_obj, pExtension, pStandInfo, hA10x);

	if( hA10x )
		AnalysePic10x_Destory(hA10x);
	
s_finish_loc:
	// 通知下位机采图完成
	LCUSSampleFinish(lc_obj, pStandInfo->ChannelNum); 
	
	s_info.nTaskID = pStandInfo->TaskID;
	DBG_MSG("尿沉渣采图任务完成...TaskID = %u\n", s_info.nTaskID);
	
	if( status != STATUS_SUCCESS )
	{
		//m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFailByLC, &s_info, sizeof(s_info));
		ALLOW_TEST_CONTEXT ctx = { pStandInfo->TaskID };
		m_BillHelper->AddStatusInformation(ThmeInformationType, RespondTaskFail, &ctx, sizeof(ALLOW_TEST_CONTEXT));
//		m_BillHelper->RemoveTaskID(s_info.nTaskID);
	}
	else
	{
		/ *if( n40xCounter == 0 )
			s_info.Coefficient = 1.0f;
		else
			s_info.Coefficient = (24.0f / (float)pExtension->s10x_range.nSample10xCount) * ((float)nTotalCounter / (float)n40xCounter);
		m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));* /
	}

	m_BillHelper->SetFailOverTask(FALSE);
}*/

void CUSArc::USSampleOne(PFINISH_STANDING_INFO pStandInfo)
{
	ULONG                  n40xCounter, nTotalCounter;
	HANDLE                 hA10x = NULL;
	KSTATUS                status;
	PROGRAM_OBJECT         lc_obj, ca_obj, rec_obj;
	PBILL3_EXTENSION       pExtension;
	SAMPLING_INFORMATION   s_info = { 0 };

	FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
	EnterCriticalSection(&pExtension->cs);
	lc_obj = pExtension->o_dev.s_lc_dev;
	ca_obj = pExtension->o_dev.s_ca_dev;
	rec_obj = pExtension->o_dev.s_rec_dev;
	LeaveCriticalSection(&pExtension->cs);

	DBG_MSG("开始40x倍镜采图...\n");
#ifdef NO_PRECIPITATE_PIC
	status = Sample40xGrapControl_noPrecipitate(lc_obj, ca_obj, rec_obj, pExtension, pStandInfo);
#else
	status = Sample40xGrapControl(lc_obj, ca_obj, rec_obj, pExtension, pStandInfo);
#endif

	// 通知下位机采图完成
	LCUSSampleFinish(lc_obj, pStandInfo->ChannelNum);

	s_info.nTaskID = pStandInfo->TaskID;
	DBG_MSG("尿沉渣采图任务完成...TaskID = %u\n", s_info.nTaskID);

	if (status != STATUS_SUCCESS)
	{
		//m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFailByLC, &s_info, sizeof(s_info));
		ALLOW_TEST_CONTEXT ctx = { pStandInfo->TaskID };
		m_BillHelper->AddStatusInformation(ThmeInformationType, RespondTaskFail, &ctx, sizeof(ALLOW_TEST_CONTEXT));
		//		m_BillHelper->RemoveTaskID(s_info.nTaskID);
	}
	else
	{
		/*if( n40xCounter == 0 )
		s_info.Coefficient = 1.0f;
		else
		s_info.Coefficient = (24.0f / (float)pExtension->s10x_range.nSample10xCount) * ((float)nTotalCounter / (float)n40xCounter);
		m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));*/
	}

	//m_BillHelper->SetFailOverTask(FALSE);
}

void CUSArc::BarcodeReco(PBARCODE_REQ_CONTEXT ctx)
{
	/*UINT                   nCapCount = 0;
	KSTATUS                status;
	PROGRAM_OBJECT         lc_obj, ca_obj, rec_obj;
	PBILL3_EXTENSION       pExtension;
	BARCODE_INFORMATION    b_info = { 0 };
	char task_path[MAX_PATH] = { 0 }, barcodePicPath[MAX_PATH] = { 0 }, temp[MAX_PATH];
	char buf[256] = { 0 };

	FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
	EnterCriticalSection(&pExtension->cs);
	lc_obj = pExtension->o_dev.s_lc_dev;
	ca_obj = pExtension->o_dev.s_ca_dev;
	rec_obj = pExtension->o_dev.s_rec_dev;
	LeaveCriticalSection(&pExtension->cs);

	b_info.nTaskID = ctx->TaskId;
	nCapCount = ctx->NeedCaptureImageCount;
	m_BillHelper->GetRecGrapReslutPath(task_path);
	if (PathFileExists(task_path) == FALSE)
	{
		if (CreateDirectory(task_path, NULL) == FALSE)
		{
			DBG_MSG("CreateDirectory = %s Fail\n", task_path);
		}
	}
	sprintf_s(temp, "%u", ctx->TaskId);
	PathAppend(task_path, temp);
	CreateDirectory(task_path, NULL);
	PathAppend(task_path, "Barcode");
	CreateDirectory(task_path, NULL);

	DBG_MSG("开始性状条码采图，max=%u...\n", nCapCount);

	for (size_t i = 0; i < nCapCount; i++)
	{
		strcpy_s(barcodePicPath, task_path);
		sprintf_s(temp, "%u.jpg", i);
		PathAppend(barcodePicPath, temp);
		// 采图
		status = CaptureCameraGrap(ca_obj, 1, barcodePicPath);
		if (status != STATUS_SUCCESS)
		{
			DBG_MSG("性状条码采图失败:0x%x (%s)\n", status, barcodePicPath);
			continue;
		}
		// 识别条码,如果识别成功，则返回，否则继续执行到最大次数
		bool bSuccess = BarcodeRec_BarcodeParse(barcodePicPath, buf, BR_TYPE_2);
		if (bSuccess && strlen(buf) > 6)
		{
			DBG_MSG("识别到条码：%s [%s]\n", buf, barcodePicPath);
		}
		else
		{
			bSuccess = false;
			DBG_MSG("未识别到条码：[%s]\n", barcodePicPath);
		}
		if (bSuccess)
		{
			strcpy_s(b_info.BarCode, buf);
			break;
		}

		// 识别浊度，待实现！！！！！！！！！！！！！！

		status = RotatioNextAngle(lc_obj);
		if (status != STATUS_SUCCESS)
		{
			DBG_MSG("BarcodeReco RotatioNextAngle failed. code=0x%x\n", status);
		}
	}
	status = BarcodeRecoFinish(lc_obj);
	if (status != STATUS_SUCCESS)
	{
		DBG_MSG("BarcodeReco BarcodeRecoFinish failed. code=0x%x\n", status);
	}
	status = m_BillHelper->AddStatusInformation(ThmeInformationType, BarcodeRequest, &b_info, sizeof(BARCODE_INFORMATION));
	if (status != STATUS_SUCCESS)
	{
		DBG_MSG("BarcodeReco AddStatusInformation failed. code=0x%x\n", status);
	}*/
}

KSTATUS CUSArc::RotatioNextAngle(PROGRAM_OBJECT lc_obj)
{
	LC_COMMAND lc_cmd = { 0 };
	lc_cmd.lc_header.flag = LC_FLAG;
	lc_cmd.lc_header.nLCCmd = UC_ROTATE_NEXT_ANGLE_REQ;

	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

KSTATUS CUSArc::BarcodeRecoFinish(PROGRAM_OBJECT lc_obj)
{
	LC_COMMAND lc_cmd = { 0 };
	lc_cmd.lc_header.flag = LC_FLAG;
	lc_cmd.lc_header.nLCCmd = UC_END_BARCODE_TURBIDITY;

	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

void CUSArc::GetAllPicCells(std::map<int, AllCellsInfo> &AllPicCells, PCHAR szIniPath)
{
	char SessionName[32], str[32], temp[MAX_PATH];

	int nEndIndex = GetPrivateProfileInt("AdditionInfo", "EndIndex", 0, szIniPath);
	for (int i = 0; i <= nEndIndex; i++)
	{
		_snprintf_s(SessionName, sizeof(SessionName) - 1, "%d", i);
		int ncount = GetPrivateProfileInt(SessionName, "Count", 0, szIniPath);

		AllCellsInfo Cells = { 0 };
		for (int n = 0; n < ncount; n++)
		{
			int left = 0, right = 0, top = 0, bottom = 0, nClass = 0, score = 0;
			_snprintf_s(str, sizeof(str) - 1, "%u", n);
			GetPrivateProfileString(SessionName, str, "", temp, MAX_PATH, szIniPath);
			sscanf(temp, "%d,%d,%d,%d,%d,%d",
				&nClass,
				&left,
				&top,
				&right,
				&bottom,
				&score);

			if (nClass == CELL_QC_TYPE)
			{
				if (Cells.QCount >= sizeof(Cells.Q) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.Q[Cells.QCount].left = left;
				Cells.Q[Cells.QCount].right = right;
				Cells.Q[Cells.QCount].top = top;
				Cells.Q[Cells.QCount].bottom = bottom;
				Cells.Q[Cells.QCount].score = score;
				Cells.QCount++;
			}
			else if (nClass == CELL_EP_TYPE)
			{
				if (Cells.SPCount >= sizeof(Cells.SP)/sizeof(CellInfo))
				{
					continue;
				}
				Cells.SP[Cells.SPCount].left = left;
				Cells.SP[Cells.SPCount].right = right;
				Cells.SP[Cells.SPCount].top = top;
				Cells.SP[Cells.SPCount].bottom = bottom;
				Cells.SP[Cells.SPCount].score = score;
				Cells.SPCount++;
			}
			else if (nClass == CELL_PHAGOCYTES_TYPE)
			{
				if (Cells.TSCount >= sizeof(Cells.TS) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.TS[Cells.TSCount].left = left;
				Cells.TS[Cells.TSCount].right = right;
				Cells.TS[Cells.TSCount].top = top;
				Cells.TS[Cells.TSCount].bottom = bottom;
				Cells.TS[Cells.TSCount].score = score;
				Cells.TSCount++;
			}
			else if (nClass == CELL_WBCC_TYPE)
			{
				if (Cells.XBTCount >= sizeof(Cells.XBT) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.XBT[Cells.XBTCount].left = left;
				Cells.XBT[Cells.XBTCount].right = right;
				Cells.XBT[Cells.XBTCount].top = top;
				Cells.XBT[Cells.XBTCount].bottom = bottom;
				Cells.XBT[Cells.XBTCount].score = score;
				Cells.XBTCount++;
			}
			else if (nClass == CELL_XY_TYPE)
			{
				if (Cells.XYCount >= sizeof(Cells.XY) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.XY[Cells.XYCount].left = left;
				Cells.XY[Cells.XYCount].right = right;
				Cells.XY[Cells.XYCount].top = top;
				Cells.XY[Cells.XYCount].bottom = bottom;
				Cells.XY[Cells.XYCount].score = score;
				Cells.XYCount++;
			}
			else if (nClass == CELL_BACT_TYPE)
			{
				if (Cells.MJCount >= sizeof(Cells.MJ) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.MJ[Cells.MJCount].left = left;
				Cells.MJ[Cells.MJCount].right = right;
				Cells.MJ[Cells.MJCount].top = top;
				Cells.MJ[Cells.MJCount].bottom = bottom;
				Cells.MJ[Cells.MJCount].score = score;
				Cells.MJCount++;
			}
			else if (nClass == CELL_CAOX_TYPE)
			{
				if (Cells.CSCount >= sizeof(Cells.CS) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.CS[Cells.CSCount].left = left;
				Cells.CS[Cells.CSCount].right = right;
				Cells.CS[Cells.CSCount].top = top;
				Cells.CS[Cells.CSCount].bottom = bottom;
				Cells.CS[Cells.CSCount].score = score;
				Cells.CSCount++;
			}
			else if (nClass == CELL_RED_CAOX_TYPE)
			{
				if (Cells.CSJCount >= sizeof(Cells.CSJ) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.CSJ[Cells.CSJCount].left = left;
				Cells.CSJ[Cells.CSJCount].right = right;
				Cells.CSJ[Cells.CSJCount].top = top;
				Cells.CSJ[Cells.CSJCount].bottom = bottom;
				Cells.CSJ[Cells.CSJCount].score = score;
				Cells.CSJCount++;
			}
			else if (nClass == CELL_URIC_TYPE)
			{
				if (Cells.NSCount >= sizeof(Cells.NS) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.NS[Cells.NSCount].left = left;
				Cells.NS[Cells.NSCount].right = right;
				Cells.NS[Cells.NSCount].top = top;
				Cells.NS[Cells.NSCount].bottom = bottom;
				Cells.NS[Cells.NSCount].score = score;
				Cells.NSCount++;
			}
			else if (nClass == CELL_LS_TYPE)
			{
				if (Cells.LSCount >= sizeof(Cells.LS) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.LS[Cells.LSCount].left = left;
				Cells.LS[Cells.LSCount].right = right;
				Cells.LS[Cells.LSCount].top = top;
				Cells.LS[Cells.LSCount].bottom = bottom;
				Cells.LS[Cells.LSCount].score = score;
				Cells.LSCount++;
			}
			else if (nClass == CELL_GRAN_TYPE)
			{
				if (Cells.KLCount >= sizeof(Cells.KL) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.KL[Cells.KLCount].left = left;
				Cells.KL[Cells.KLCount].right = right;
				Cells.KL[Cells.KLCount].top = top;
				Cells.KL[Cells.KLCount].bottom = bottom;
				Cells.KL[Cells.KLCount].score = score;
				Cells.KLCount++;
			}
			else if (nClass == CELL_HYAL_TYPE)
			{
				if (Cells.TMCount >= sizeof(Cells.TM) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.TM[Cells.TMCount].left = left;
				Cells.TM[Cells.TMCount].right = right;
				Cells.TM[Cells.TMCount].top = top;
				Cells.TM[Cells.TMCount].bottom = bottom;
				Cells.TM[Cells.TMCount].score = score;
				Cells.TMCount++;
			}
			else if (nClass == CELL_MUCS_TYPE)
			{
				if (Cells.NYCount >= sizeof(Cells.NY) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.NY[Cells.NYCount].left = left;
				Cells.NY[Cells.NYCount].right = right;
				Cells.NY[Cells.NYCount].top = top;
				Cells.NY[Cells.NYCount].bottom = bottom;
				Cells.NY[Cells.NYCount].score = score;
				Cells.NYCount++;
			}
			else if (nClass == CELL_BACILLUS_TYPE)
			{
				if (Cells.GJCount >= sizeof(Cells.GJ) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.GJ[Cells.GJCount].left = left;
				Cells.GJ[Cells.GJCount].right = right;
				Cells.GJ[Cells.GJCount].top = top;
				Cells.GJ[Cells.GJCount].bottom = bottom;
				Cells.GJ[Cells.GJCount].score = score;
				Cells.GJCount++;
			}
			else if (nClass == CELL_SPRM_TYPE)
			{
				if (Cells.JZCount >= sizeof(Cells.JZ) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.JZ[Cells.JZCount].left = left;
				Cells.JZ[Cells.JZCount].right = right;
				Cells.JZ[Cells.JZCount].top = top;
				Cells.JZ[Cells.JZCount].bottom = bottom;
				Cells.JZ[Cells.JZCount].score = score;
				Cells.JZCount++;
			}
			else if (nClass == CELL_RED_TYPE)
			{
				if (Cells.HCount >= sizeof(Cells.H) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.H[Cells.HCount].left = left;
				Cells.H[Cells.HCount].right = right;
				Cells.H[Cells.HCount].top = top;
				Cells.H[Cells.HCount].bottom = bottom;
				Cells.H[Cells.HCount].score = score;
				Cells.HCount++;
			}
			else if (nClass == CELL_WHITE_TYPE)
			{
				if (Cells.BCount >= sizeof(Cells.B) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.B[Cells.BCount].left = left;
				Cells.B[Cells.BCount].right = right;
				Cells.B[Cells.BCount].top = top;
				Cells.B[Cells.BCount].bottom = bottom;
				Cells.B[Cells.BCount].score = score;
				Cells.BCount++;
			}
			else if (nClass == CELL_DRED_TYPE) //异型红细胞
			{
				if (Cells.YHCount >= sizeof(Cells.YH) / sizeof(CellInfo))
				{
					continue;
				}
				Cells.YH[Cells.YHCount].left = left;
				Cells.YH[Cells.YHCount].right = right;
				Cells.YH[Cells.YHCount].top = top;
				Cells.YH[Cells.YHCount].bottom = bottom;
				Cells.YH[Cells.YHCount].score = score;
				Cells.YHCount++;
			}
		}
		AllPicCells[i] = Cells;
	}
}

bool CUSArc::LocationCellPos(HBITMAP hBitmap, std::vector<cv::Rect> &contours)
{
	bool ret = false;
	cv::Mat src, tmp;
	
	try
	{
		do
		{
			if (hBitmap == NULL)
			{
				break;
			}
			if (!HBitmapToMat(hBitmap, tmp) || tmp.empty())
			{
				break;
			}
			cv::resize(tmp, src, cv::Size(720, 480));
			cv::Mat gray, dst;
			cv::cvtColor(src, gray, CV_BGR2GRAY);
			cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0); // 高斯降噪
			cv::Canny(gray, gray, 100, 250); // 查找边缘
			
			cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			// 膨胀腐蚀操作
			for (int i = 0; i < 3; i++) {
				cv::morphologyEx(gray, gray, cv::MORPH_DILATE, element);
			}
			for (int i = 0; i < 3; i++) {
				cv::morphologyEx(gray, gray, cv::MORPH_ERODE, element);
			}
			// Holes填充
			for (int i = 0; i < 5; i++) // 填充10次
			{
				fillHole(gray, gray);
			}
			// 阈值处理
			double  Otsu = 0;
			Otsu = cv::threshold(gray, dst, 40, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
			if (Otsu > 200)
				cv::threshold(gray, dst, 150, 255, CV_THRESH_BINARY);
			std::vector<std::vector<cv::Point> > _contours;
			std::vector<cv::Vec4i> hierarchy;
			cv::findContours(dst, _contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			int ctsSum = _contours.size();
			for (int i = 0; i < ctsSum; i++)
			{
				cv::Rect rt = cv::boundingRect(cv::Mat(_contours[i]));
				// filter by area for small cell target
				int area = rt.area();
				if (area >= 15 * 15 && area <= 40 * 40)
				{
					contours.push_back(rt);
				}
			}
			// find hierarchy tree top root level for CV_RETR_TREE
			/*int idxTreeTop = -1;
			for (int i = 0; i < ctsSum; i++)
			{
				if (hierarchy[i][3] == -1 && hierarchy[i][2] != -1)
				{
					idxTreeTop = i;
					break;
				}
			}
			if (idxTreeTop == -1)
			{
				DBG_MSG("idxTreeTop not find\n");
				break;
			}
			// find hierarchy tree second level is target
			for (int i = 0; i < ctsSum; i++)
			{
				if (hierarchy[i][3] == idxTreeTop)
				{
					cv::Rect rt = cv::boundingRect(cv::Mat(_contours[i]));
					// filter by area for small cell target
					int area = rt.area();
					if (area >= 15 * 15 && area <= 40 * 40)
					{
						contours.push_back(rt);
					}
				}
			}*/

			ret = true;
		} while (0);
	}
	catch (std::exception & ex)
	{
		DBG_MSG("Exception: %s\n", ex.what());
	}

	return ret;
}

bool CUSArc::LocationCellPos(HBITMAP hBitmap, std::vector<cv::Rect> &contours, PCHAR pFilePath)
{
	bool ret = false;
	cv::Mat src, tmp;

	try
	{
		do
		{
			if (hBitmap == NULL)
			{
				break;
			}
			if (!HBitmapToMat(hBitmap, tmp) || tmp.empty())
			{
				break;
			}
			cv::resize(tmp, src, cv::Size(720, 480));
			cv::imwrite(pFilePath, src);
			cv::Mat gray, dst;
			cv::cvtColor(src, gray, CV_BGR2GRAY);
			cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0); // 高斯降噪
			cv::Canny(gray, gray, 100, 250); // 查找边缘
			
			cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			// 膨胀腐蚀操作
			for (int i = 0; i < 3; i++) {
				cv::morphologyEx(gray, gray, cv::MORPH_DILATE, element);
			}
			for (int i = 0; i < 3; i++) {
				cv::morphologyEx(gray, gray, cv::MORPH_ERODE, element);
			}
			// Holes填充
			for (int i = 0; i < 5; i++) // 填充10次
			{
				fillHole(gray, gray);
			}
			// 阈值处理
			double  Otsu = 0;
			Otsu = cv::threshold(gray, dst, 40, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
			if (Otsu > 200)
				cv::threshold(gray, dst, 150, 255, CV_THRESH_BINARY);
			std::vector<std::vector<cv::Point> > _contours;
			std::vector<cv::Vec4i> hierarchy;
			cv::findContours(dst, _contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			int ctsSum = _contours.size();
			for (int i = 0; i < ctsSum; i++)
			{
				cv::Rect rt = cv::boundingRect(cv::Mat(_contours[i]));
				// filter by area for small cell target
				int area = rt.area();
				if (area >= 15 * 15 && area <= 40 * 40)
				{
					contours.push_back(rt);
				}
			}

			ret = true;
		} while (0);
	}
	catch (std::exception & ex)
	{
		DBG_MSG("Exception: %s\n", ex.what());
	}

	return ret;
}

//10倍镜移动
KSTATUS CUSArc::Move10x( PROGRAM_OBJECT  lc_obj, UCHAR n10xIndex, PFINISH_STANDING_INFO finish_info )
{
	LC_COMMAND                             lc_cmd = { 0 };
	/*PLC_LOCATE_10_MICROSCOPE_INFO          l10 = (PLC_LOCATE_10_MICROSCOPE_INFO) &lc_cmd;
	
	l10->lc_header.flag = LC_FLAG;
	l10->lc_header.nLCCmd = LC_LOCATE_10_MICROSCOPE;
	l10->LocateInfo.ChannelNum = finish_info->ChannelNum;
	l10->LocateInfo.IsReturnTakePicCommand = RETURN_SAMPLE_CMD;
	l10->LocateInfo.YPicNum = n10xIndex >> 3;
	l10->LocateInfo.XPicNum = n10xIndex & 0x7;*/

	PMOVE_NEXT_AREA_REQ_TYPE ctx = (PMOVE_NEXT_AREA_REQ_TYPE)&lc_cmd;
	ctx->lc_header.flag = LC_FLAG;
	ctx->lc_header.nLCCmd = UC_MOVE_NEXT_AREA_REQ;
	ctx->CmdCtx.ChannelNumber = finish_info->ChannelNum;
	ctx->CmdCtx.MicroLens = MLENS_10X;
	ctx->CmdCtx.M10LensX = n10xIndex & 0x7;
	ctx->CmdCtx.M10LensY = n10xIndex >> 3;
	
	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

//40倍镜移动
KSTATUS CUSArc::Move40x( PROGRAM_OBJECT lc_obj, UCHAR  nChannel, PSAMPLE_40X_INFO s_40 )
{
	LC_COMMAND                             lc_cmd = {0};
	/*PLC_LOCATE_40_MICROSCOPE_INFO          l40 = (PLC_LOCATE_40_MICROSCOPE_INFO) &lc_cmd;
	
	l40->lc_header.flag = LC_FLAG;
	l40->lc_header.nLCCmd = LC_LOCATE_40_MICROSCOPE;
	l40->LocateInfo.ChannelNum = nChannel;
	l40->LocateInfo.XPicNumFor10 = s_40->idx_10x & 0x7;
	l40->LocateInfo.YPicNumFor10 = s_40->idx_40x >> 3;
	l40->LocateInfo.PicNumFor40 = s_40->idx_40x;
	l40->LocateInfo.YOffset     = s_40->dy;
	
	if( s_40->dX < 0 )
		l40->LocateInfo.XOffset     = abs(s_40->dX);
	else if( s_40->dX > 0 )
		l40->LocateInfo.XOffset     =  s_40->dX + 100;
	
	if( s_40->dy < 0 )
		l40->LocateInfo.YOffset     = abs(s_40->dy);
	else if( s_40->dy > 0 )
		l40->LocateInfo.YOffset     =  s_40->dy + 100;
	
	l40->LocateInfo.IsReturnTakePicCommand = RETURN_SAMPLE_CMD;*/

	// 10倍镜 X坐标0~7 Y坐标0~2
	// 40倍镜 X坐标0~3 Y坐标0~3
	PMOVE_NEXT_AREA_REQ_TYPE ctx = (PMOVE_NEXT_AREA_REQ_TYPE)&lc_cmd;
	ctx->lc_header.flag = LC_FLAG;
	ctx->lc_header.nLCCmd = UC_MOVE_NEXT_AREA_REQ;
	ctx->CmdCtx.ChannelNumber = nChannel;
	ctx->CmdCtx.MicroLens = MLENS_40X;
	ctx->CmdCtx.M10LensX = s_40->idx_10x & 0x7;
	ctx->CmdCtx.M10LensY = s_40->idx_10x >> 3;
	ctx->CmdCtx.M40LensX = s_40->idx_40x & 0x3;
	ctx->CmdCtx.M40LensY = s_40->idx_40x >> 2;

	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

KSTATUS CUSArc::Move40x(PROGRAM_OBJECT lc_obj, UCHAR nChannel, UCHAR nX, UCHAR nY)
{
	LC_COMMAND lc_cmd = { 0 };
	PMOVE_NEXT_AREA_REQ_TYPE2 ctx = (PMOVE_NEXT_AREA_REQ_TYPE2)&lc_cmd;
	ctx->lc_header.flag = LC_FLAG;
	ctx->lc_header.nLCCmd = UC_MOVE_NEXT_AREA_REQ;
	ctx->ChannelNumber = nChannel;
	ctx->M40LensX = nX;
	ctx->M40LensY = nY;

	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

KSTATUS CUSArc::StartMoveNewRow(PROGRAM_OBJECT lc_obj,UCHAR nCha,UCHAR nRow,uint16_t hSpeed)
{
	LC_COMMAND lc_cmd = { 0 };
	PMOVE_NEW_ROW ctx = (PMOVE_NEW_ROW)&lc_cmd;
	ctx->lc_header.flag = LC_FLAG;
	ctx->lc_header.nLCCmd = UC_START_PIC_NEW_ROW;
	ctx->CandRow.ChannelNumber = nCha;
	ctx->CandRow.nRow = nRow;
	ctx->hSpeedValue = hSpeed;
	ctx->hSpeedValue = SwitchByte(ctx->hSpeedValue);
	
	MOVE_NEW_ROW_REQ tRet = {0};
	int ret = STATUS_SUCCESS;
	ret = FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), &tRet, sizeof(MOVE_NEW_ROW_REQ), FALSE, FALSE);
	if((tRet.ChannelNumber != nCha || tRet.nRow != nRow))
	{
		DBG_MSG("debug:UC_START_PIC_NEW_ROW ERROR");
		return STATUS_UNKNOW_ERROR;
	}
	return ret;
}

KSTATUS CUSArc::EndMoveNewRow(PROGRAM_OBJECT lc_obj,UCHAR nCha,UCHAR nRow)
{
	LC_COMMAND lc_cmd = { 0 };
	PMOVE_NEW_ROW_END ctx = (PMOVE_NEW_ROW_END)&lc_cmd;
	ctx->lc_header.flag = LC_FLAG;
	ctx->lc_header.nLCCmd = UC_END_PIC_NEW_ROW;
	ctx->CandRow.ChannelNumber = nCha;
	ctx->CandRow.nRow = nRow;

	MOVE_NEW_ROW_REQ tRet = {0};
	int ret = STATUS_SUCCESS;
	ret = FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), &tRet, sizeof(MOVE_NEW_ROW_REQ), FALSE, FALSE);
	if ((tRet.ChannelNumber!= nCha|| tRet.nRow != nRow))
	{
		DBG_MSG("debug:UC_END_PIC_NEW_ROW ERROR");
		return STATUS_UNKNOW_ERROR;
	}
	return ret;
}


KSTATUS CUSArc::StartFocus(PROGRAM_OBJECT lc_obj)
{
	LC_COMMAND lc_cmd = { 0 };
	lc_cmd.lc_header.flag = LC_FLAG;
	lc_cmd.lc_header.nLCCmd = UC_FOCUSING_REQ;

	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

KSTATUS CUSArc::StopFocus(PROGRAM_OBJECT lc_obj)
{
	LC_COMMAND lc_cmd = { 0 };
	lc_cmd.lc_header.flag = LC_FLAG;
	lc_cmd.lc_header.nLCCmd = UC_EXITFOCUSING;

	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

KSTATUS CUSArc::MoveZ(PROGRAM_OBJECT lc_obj,UCHAR Direction,WORD StepCount)
{
	LC_COMMAND                             lc_cmd = {0};
	/*PLC_MOVE_PLATFORM_INFO               lp =(PLC_MOVE_PLATFORM_INFO)&lc_cmd;

	lp->lc_header.flag = LC_FLAG;
	lp->lc_header.nLCCmd = LC_MOVE_SCAN_PLATFORM;
	lp->MoveInfo.MotorID= (UCHAR)ZMotorID;
	lp->MoveInfo.Direction =(UCHAR)Direction;
	lp->MoveInfo.StepCountHigh = UCHAR(StepCount >> 8);
	lp->MoveInfo.StepCountLow= UCHAR(StepCount & 0XFF);*/

	PFOCUS_STEP_REQ_TYPE ctx = (PFOCUS_STEP_REQ_TYPE)&lc_cmd;
	ctx->lc_header.flag = LC_FLAG;
	ctx->lc_header.nLCCmd = UC_FOCUS_STEP_REQ;
	ctx->CmdCtx.Direction = Direction;
	ctx->CmdCtx.Step = StepCount;

	return FMMC_EasySynchronousBuildPRP(lc_obj,NULL,PRP_AC_DEVIO,CONTROL_CMD_REQUEST,&lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

KSTATUS CUSArc::MoveZBack(PROGRAM_OBJECT lc_obj)
{
	LC_COMMAND                             lc_cmd;
	PLC_MOVE_Z_AXIS          lz = (PLC_MOVE_Z_AXIS) &lc_cmd;

	lz->lc_header.flag = LC_FLAG;
	lz->lc_header.nLCCmd = LC_MOVE_Z;
	//l10->LocateInfo.ChannelNum = nChannel;
	lz->MoveInfo.StepCountHigh = 0;
	lz->MoveInfo.StepCountLow = 0;

	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);

}

KSTATUS CUSArc::SaveZ(PROGRAM_OBJECT lc_obj)
{
	LC_COMMAND lc_cmd = { 0 };
	lc_cmd.lc_header.flag = LC_FLAG;
	lc_cmd.lc_header.nLCCmd = UC_SAVE_FOCALLENGTH_REQ;

	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

#define RETRY_COUNTER 3

//10倍镜采图
KSTATUS CUSArc::Sample10xGrapControl( PROGRAM_OBJECT  lc_obj, PROGRAM_OBJECT  ca_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info , HANDLE  &hA10x)
{
	char                task_path[MAX_PATH], ini_path[MAX_PATH];
	char                key_name[50], v[MAX_PATH];
	float               nPos;
	LONG                nRecNumber;
	KSTATUS             status;
	SAMPLING_PROGRESS   s_prg;

	m_BillHelper->GetRecGrapReslutPath(task_path);
	
	if( PathFileExists(task_path) == FALSE )
	{
		if( CreateDirectory(task_path, NULL) == FALSE )
		{
			DBG_MSG("CreateDirectory = %s Fail\n", task_path);
			return STATUS_MKDIR_FAIL;
		}
	}

	sprintf_s(key_name, "%u", finish_info->TaskID);
	PathAppend(task_path, key_name);
	CreateDirectory(task_path, NULL);
	
	hA10x = AnalysePic10x_Create(pExtension->prg_status.nTotalSamplingNum);
	if( hA10x == NULL )
	{
		DBG_MSG("AnalysePic10x_Create err...\n");
		return  STATUS_ALLOC_MEMORY_ERROR;
	}
	
	strcpy_s(ini_path, task_path);
	PathAppend(ini_path, ADDITION_INFO_FILE);
	DeleteFile(ini_path);
	sprintf_s(v, "%u", pExtension->s10x_range.nSample10xCount);
	WritePrivateProfileString(SECTION_10X, COUNTER_KEY, v, ini_path);
	
	s_prg.nTaskID = finish_info->TaskID;

	DBG_MSG("10x采图开始:pExtension->s10x_range.nSample10xCount = %u, nTaskID = %u", pExtension->s10x_range.nSample10xCount, s_prg.nTaskID);
	
	for( USHORT j = 0 ; j < pExtension->s10x_range.nSample10xCount ; j ++ )
	{
		WaitForSingleObject(m_hPauseEvent, INFINITE);
		if( m_BillHelper->QueryTaskOverByFail() )
		{
			status = STATUS_CANCEL_ACTION;
			DBG_MSG("Sample10xGrapControl STATUS_CANCEL_ACTION\n");
			break;
		}
		for( USHORT nRetry = 0 ; nRetry < RETRY_COUNTER ; nRetry ++ )
		{
			status = Move10x(lc_obj, (UCHAR)pExtension->s10x_range.pSample10xIndexArray[j], finish_info);
			if( status != STATUS_SUCCESS )
			{
				DBG_MSG("Move10x->Fail...nRetryCounter = %u, npos = \n", nRetry, pExtension->s10x_range.pSample10xIndexArray[j]);
				if( nRetry + 1 == RETRY_COUNTER )
					return status;
				continue;
			}

			break;
		}
		
		sprintf_s(key_name,"%.3u_%.4u.jpg", 10, j);
		PathAppend(task_path, key_name);
		status = CaptureCameraGrap(ca_obj, 0, task_path);
		sprintf_s(key_name, "%u", j);
		if( status != STATUS_SUCCESS )
			strcpy_s(v, "10x采图 CaptureCameraGrap error");
		else
			sprintf_s(v, "%u", pExtension->s10x_range.pSample10xIndexArray[j]);
		
		WritePrivateProfileString(SECTION_10X, key_name, v, ini_path);
		if( status != STATUS_SUCCESS )
			DBG_MSG("10x Capture Grap Error = %s\n", task_path);
		else
		{
			// 10倍镜图片分析
			if( AnalysePic10x_Entry(hA10x, task_path, (UCHAR)pExtension->s10x_range.pSample10xIndexArray[j]) == FALSE )
				DBG_MSG("AnalysePic10x_Entry = %s err\n ",task_path);
		}
		
		PathRemoveFileSpec(task_path);
		nPos  = (float)(j + 1) / (float)pExtension->s10x_range.nSample10xCount;
		nPos *= 50.0f;
		s_prg.nPos = (int)nPos;
		m_BillHelper->AddStatusInformation(ThmeInformationType, SamplingProgress, &s_prg, sizeof(s_prg));
	}

	AnalysePic10x_Finish(hA10x); // 所有10倍镜图片分析完成

	nRecNumber = 0;
	AnalysePic10x_RecCellNumber(hA10x, &nRecNumber);
	sprintf_s(v, "%u", nRecNumber);
	WritePrivateProfileString(SECTION_10X, CELL_COUNT_KEY, v, ini_path);

	return STATUS_SUCCESS;
}

VOID PRP_REC_COMPLETE_CALLBACK2(CUSArc *pUsArc, PREC_GRAP_CONTEXT_WARP pContext)
{
	RECOGNITION_PROGRESS     rec_prg = { 0 };
	SAMPLING_INFORMATION     s_info = { 0 };
	PBILL3_EXTENSION         pExtension = NULL;
	int nSample40xCounter = 0;
	int nRecCompleteCount = 0;
	int nTotalCounter = 0;
	float pos = 0.0f;
	CUSArc *_this = NULL;
	PREC_GRAP_CONTEXT_WARP ptr = NULL;
	PASYNC_REC_CONTEXT ctx = NULL;

	do
	{
		std::unique_ptr<REC_GRAP_CONTEXT_WARP, void(*)(void*)> ptr((PREC_GRAP_CONTEXT_WARP)pContext, [](void* p) {free(p); });
		if (ptr == NULL || pUsArc == NULL)
		{
			DBG_MSG("Async Rec Grap Error\n");
			break;
		}
		if (pUsArc->m_BillHelper->QueryTaskOverByFail())
		{
			DBG_MSG("PRP_REC_COMPLETE_CALLBACK2 STATUS_CANCEL_ACTION\n");
			break;
		}
		std::lock_guard<std::mutex> lck(ptr->recCtx->mutex);
		if (ptr->recCtx->nRecCompleteCount < 0)
		{
			break;
		}
		ptr->recCtx->nRecCompleteCount++;
		nSample40xCounter = ptr->recCtx->nSample40xCounter;
		nRecCompleteCount = ptr->recCtx->nRecCompleteCount;
		nTotalCounter = ptr->recCtx->nTotalCounter;


		if (nSample40xCounter != 0)
		{
			pos = (float)nRecCompleteCount / (float)nSample40xCounter;
		}
		else
		{
			pos = 1.0f;
		}

		std::vector<N_PIC_TARGET_TASK>::iterator it;
		for (it = pUsArc->m_vecNpicTask.begin(); it !=  pUsArc->m_vecNpicTask.end(); it++)
		{
			if (ptr->rgfr.rTaskID == it->nID)
			{
				DBG_MSG("debug:采图数未达到设定数，实际采图数为=%d",it->picCountActually);
				pos = (float)nRecCompleteCount / (float)it->picCountActually;
				if (pos >= 1.0f)
					pUsArc->m_vecNpicTask.erase(it);

				break;
			}
		}

		rec_prg.nPos = pos * 100;
		rec_prg.nTaskID = ptr->rgfr.rTaskID;
		pUsArc->m_BillHelper->AddStatusInformation(ThmeInformationType, RecognitionProgress, &rec_prg, sizeof(RECOGNITION_PROGRESS));
		DBG_MSG("[%d]识别完成 %d/%d\n", pContext->rgfr.rTaskID, pContext->recCtx->nRecCompleteCount, pContext->recCtx->nSample40xCounter);
		s_info.nTaskID = ptr->rgfr.rTaskID;
		s_info.nSampleIndex = ptr->rgfr.g_rc.rIndex;
		s_info.nGrapSize = ptr->rgfr.g_rc.rGrapSize;
		if ((pos >= 1.0f) && ptr->recCtx != NULL)
		{
			FMMC_GetPrgObjectExtension(pUsArc->m_BillPrgObj, (PVOID*)&pExtension);
			s_info.Coefficient = 1.0f;
			pUsArc->m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));

			// 识别完成
			ctx = ptr->recCtx;
		}
	} while (0);

	if (ctx)
	{
		delete ctx;
	}
}

VOID WINAPI PRP_REC_COMPLETE_CALLBACK( PRP prp, PVOID pContext, KSTATUS status )
{
	RECOGNITION_PROGRESS     rec_prg = { 0 };
	SAMPLING_INFORMATION     s_info = { 0 };
	PBILL3_EXTENSION         pExtension = NULL;
	int nSample40xCounter = 0;
	int nRecCompleteCount = 0;
	int nTotalCounter = 0;
	float pos = 0.0f;
	CUSArc *_this = NULL;
	PREC_GRAP_CONTEXT_WARP ptr = NULL;

	do 
	{
		std::unique_ptr<REC_GRAP_CONTEXT_WARP, void(*)(void*)> ptr((PREC_GRAP_CONTEXT_WARP)pContext, [](void* p) {free(p); });
		if (ptr == NULL)
		{
			DBG_MSG("Async Rec Grap Error\n");
			break;
		}
		if (status != STATUS_SUCCESS)
		{
			DBG_MSG("Async Rec Grap Error = %lu [%lu, %s]\n", status, ptr->rgfr.rTaskID, ptr->rgfr.GrapPath);
		}
		_this = (CUSArc*)ptr->pContext;
		std::lock_guard<std::mutex> lck(ptr->recCtx->mutex);
		ptr->recCtx->nRecCompleteCount++;
		nSample40xCounter = ptr->recCtx->nSample40xCounter;
		nRecCompleteCount = ptr->recCtx->nRecCompleteCount;
		nTotalCounter = ptr->recCtx->nTotalCounter;

		if (nSample40xCounter != 0)
		{
			pos = (float)nRecCompleteCount / (float)nSample40xCounter;
		}
		else
		{
			pos = 1.0f;
		}
		rec_prg.nPos = pos * 100;
		rec_prg.nTaskID = ptr->rgfr.rTaskID;
		_this->m_BillHelper->AddStatusInformation(ThmeInformationType, RecognitionProgress, &rec_prg, sizeof(RECOGNITION_PROGRESS));

		s_info.nTaskID = ptr->rgfr.rTaskID;
		s_info.nSampleIndex = ptr->rgfr.g_rc.rIndex; 
		s_info.nGrapSize = ptr->rgfr.g_rc.rGrapSize;
		if (nRecCompleteCount == nSample40xCounter && ptr->recCtx != NULL)
		{
			FMMC_GetPrgObjectExtension(_this->m_BillPrgObj, (PVOID*)&pExtension);
			s_info.Coefficient = 1.0f;
			_this->m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));

			// 识别完成
			delete (ptr->recCtx);
			ptr->recCtx = NULL;
		}
	} while (0);

	FMMC_FreePRP(prp);
}



//40倍镜采图
KSTATUS CUSArc::Sample40xGrapControl( PROGRAM_OBJECT lc_obj, PROGRAM_OBJECT ca_obj, PROGRAM_OBJECT rec_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info)
{
	char                     GrapPath[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 };
	char					 BeforceFocusPath[MAX_PATH] = { 0 };
	float                    pos = 0.0f;
	ULONG                    nSample40xCounter = 0, nTotalPicCounter = 0;
	KSTATUS                  status = STATUS_SUCCESS;
	SAMPLING_PROGRESS        s_prg = { 0 };
	SAMPLING_INFORMATION     s_info = { 0 };
	REC_GRAP_FULL_RECORD     rec_full_record = {0};
	REC_GRAP_FULL_RECORD     rec_focus_full_record = {0};
	BOOL					 m_bCancelTask = FALSE;
	m_picCountActually = 0;
	unsigned short taskID =	finish_info->TaskID;
	int ChannelNum = finish_info->ChannelNum;
	char buff[64] = { 0 };
	char confPath[256] = { 0 };
	GetModuleFileNameA(NULL, confPath, MAX_PATH);
	PathRemoveFileSpecA(confPath);
	PathAppendA(confPath, "\\Conf.ini");

	WritePrivateProfileString(_T("noRecPos"), _T("curcha"), itoa(ChannelNum,buff,10), confPath);

	char  FocusPicPath[MAX_PATH] = {0};//聚焦图片路径
	int nStep  = GetPrivateProfileInt(_T("Focus"),_T("step"), 8,focusini);
	int nEnd   = GetPrivateProfileInt(_T("Focus"),_T("end"), 160,focusini);
	int nTime  = GetPrivateProfileInt(_T("Focus"),_T("time"),1,focusini);
	int Offset = GetPrivateProfileInt(_T("Focus"),_T("offset"), 1000, focusini);
	int nSwitch = GetPrivateProfileInt(_T("Focus"),_T("switch"), 1, focusini); 
	int nFocusWay = GetPrivateProfileInt(_T("Focus"),_T("FocusWay"), 0, focusini); 
	int nSavePic  = GetPrivateProfileInt(_T("Focus"),_T("SavePic"), 0, focusini); 
	int AutoLight = GetPrivateProfileInt(_T("Focus"), _T("AutoLight"), 0, focusini);
	int nBeforeFocusPosXOffset = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosXOffset"), 0, focusini);
	int nBeforeFocusPosYOffset = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosYOffset"), 0, focusini);
	int nBeforeFocusPosXSpace = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosXSpace"), 0, focusini);
	int nBeforeFocusPosYSpace = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosYSpace"), 0, focusini);
	int nBeforeFocusPosRow = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosRow"), 0, focusini);
	int nBeforeFocusPosColumn = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosColumn"), 0, focusini);
	int nHSpeedValue = GetPrivateProfileInt(_T("Focus"), _T("HSpeedValue"), 1500, focusini);

	int nBeforeFocusDelayMs = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusDelayMs"), 100, focusini);
	int nCapturePicDelayMs = GetPrivateProfileInt(_T("Focus"), _T("CapturePicDelayMs"), 10, focusini);
	int nCapturePicDelayMsAutoFocus = GetPrivateProfileInt(_T("Focus"), _T("CapturePicDelayMsAutoFocus"), 0, focusini);
	int nFocusSelLayer = GetPrivateProfileInt(_T("Focus"), _T("FoucsSelLayer"), 1, focusini); 

	int EnableQCRec = GetPrivateProfileInt(_T("Focus"), _T("EnableQCRec"), 0, focusini);//质控识别
	int AutoFocusResultOpt = GetPrivateProfileInt(_T("Focus"), _T("AutoFocusResultOpt"), 0, focusini);//自动聚焦修订
	
	GetPrivateProfileString(_T("Focus"), _T("FoucsSharpness"), "0", temp, sizeof(temp)-1, focusini);
	float fFoucsSharpness = atof(temp);
	int nTotalStep = 0;
	int picNum = 1; 
	
	int row = GetPrivateProfileInt("Focus", "CaptureImageRow", 4, focusini);
	int column = GetPrivateProfileInt("Focus", "CaptureImageColumn", 8, focusini);
	nTotalPicCounter = nSample40xCounter = row*column;

	int locationSmallCellThreshold = abs((int)GetPrivateProfileInt("Focus", "locationSmallCellThreshold", 5, focusini));

	// 设置聚焦条件
	// 比如：1.每隔多少个标本聚焦一次（默认情况下每个标本都会聚焦）。
	//      2.聚焦好一次就不再聚焦直接采图。(聚焦好的判断标准？[由小细胞确定的聚焦层，就表示是聚好焦的])
	static UINT32 nCountSamplesFromStartup = 0;		// 记录本次启动软件后镜检测试的标本数量
	++nCountSamplesFromStartup;
	static UINT32 nNumberOfNextSample = 1;			// 下一个聚焦的标本
	// 每间隔多少个标本聚焦一次
	UINT32 nNumberOfSamplesSeparateFocus = GetPrivateProfileInt(_T("Focus"), _T("nNumberOfSamplesSeparateFocus"), 0, focusini);
	// 是否只聚焦好一次后就不再聚焦
	bool bIsFocusOkOnlyOnce = (bool)GetPrivateProfileInt(_T("Focus"), _T("bIsFocusOkOnlyOnce"), 0, focusini);
	static std::map<int, int> chanelIsFoucusOk;	// 是否是聚焦好的状态,保存通道最后一次聚焦正确的层数
	//static CDirection enumFocusDirect = Backward;	// 聚焦好的状态下的初始位置移动方向
	//static WORD nFocusDistance = 0;				// 聚焦好的状态下的注释位置移动距离
	static auto lastSampleTimepoint = std::chrono::system_clock::now(); // 记录最后一个镜检标本的测试时间
	UINT32 nMaxFocusIntervalTime = GetPrivateProfileInt(_T("Focus"), _T("nMaxFocusIntervalTime"), 0, focusini); // 和最后一个镜检标本间隔达到最大时间，必须重新聚焦,默认值0表示不启用
	BOOL bSnycFocusAndRec = (BOOL)GetPrivateProfileInt(_T("Focus"), _T("SnycFocusAndRec"), 0, focusini);// 是否同步聚焦识别和细胞识别
	BOOL bFocusPostionFailStillFocus = (BOOL)GetPrivateProfileInt(_T("Focus"), _T("FocusPostionFailStillFocus"), 0, focusini);  // 定位失败是否任然聚焦，默认定位失败后不聚焦

	BOOL bSaveZposAfterFocus =(BOOL)GetPrivateProfileInt(_T("Focus"), _T("SaveZposAfterFocus"), 0, focusini);  // 聚焦好后，是否保存Z轴位置

#if (_LC_V > 2190)
	g_isHspeedCaptureImage = (BOOL)GetPrivateProfileInt("Focus", "HSpeed", 0, focusini);
	g_isBibaiize = g_isHspeedCaptureImage;
#endif

	// 设置聚焦图片路径
	m_BillHelper->GetRecGrapReslutPath(GrapPath); // RecGrapReslut
	strcpy_s(FocusPicPath, GrapPath);
	_snprintf_s(temp, sizeof(temp) - 1, "%d\\FocusPic", taskID);  // RecGrapReslut/[id]/FocusPic
	PathAppend(FocusPicPath, temp);

    DBG_MSG("FocusPicPath Path: %s\n", FocusPicPath);
	_snprintf_s(temp, sizeof(temp) - 1, "%s\\%d", GrapPath, taskID);
	if (PathFileExists(temp) == FALSE)//创建聚焦图片文件夹
	{
		CreateDirectory(temp, NULL);
	}
	if( PathFileExists(FocusPicPath) == FALSE )//创建聚焦图片文件夹
	{
		CreateDirectory(FocusPicPath, NULL);
	}

	// 设置细胞图片路径
	_snprintf_s(temp, sizeof(temp) - 1, "%d\\"RESULT_FILE, taskID);
	DBG_MSG("debug: 设置细胞图片路径 = %s",temp);
	PathAppend(GrapPath, temp);
	DeleteFile(GrapPath); // 删除已经存在的识别结果文件
	PathRemoveFileSpec(GrapPath); 
	strcpy_s(rec_full_record.ReslutRelativePath, GrapPath);
	
	s_prg.nTaskID = taskID;

	int bopen = 0;

#ifdef ASYNC_REC
	std::unique_ptr<ASYNC_REC_CONTEXT> pArctx = std::make_unique<ASYNC_REC_CONTEXT>();
	if (pArctx == NULL)
	{ 
		DBG_MSG("Sample40xGrapControl alloc memeory Failed.\n");
		return STATUS_ALLOC_MEMORY_ERROR;
	}
	PASYNC_REC_CONTEXT pCtx = pArctx.get();
	//memset(pArctx.get(), 0, sizeof(ASYNC_REC_CONTEXT));
	pArctx->nRecCompleteCount = 0;
	pArctx->nSample40xCounter = nTotalPicCounter;
	pArctx->nTotalCounter = nTotalPicCounter;
#endif 

	DBG_MSG("是否聚焦选层 = %s \n是否聚焦好一次后就不再聚焦 = %s\n", nFocusSelLayer != 0 ? "Yes" : "No", bIsFocusOkOnlyOnce != 0 ? "Yes" : "No");
	bool bIsCurrentNeedFocus = true;
	if (bIsFocusOkOnlyOnce)
	{
		auto itr = chanelIsFoucusOk.find(ChannelNum);
		if (itr != chanelIsFoucusOk.end()) bIsCurrentNeedFocus = false;
	}
	else
	{
		if (nNumberOfNextSample <= nCountSamplesFromStartup) nNumberOfNextSample = nCountSamplesFromStartup + nNumberOfSamplesSeparateFocus + 1;
		else bIsCurrentNeedFocus = false;
	}
	auto now = std::chrono::system_clock::now();
	auto tt = std::chrono::duration_cast<std::chrono::seconds>(now - lastSampleTimepoint).count();
	if (nMaxFocusIntervalTime > 0 && tt > nMaxFocusIntervalTime) bIsCurrentNeedFocus = true; // 距离最后一个镜检标本测试xx时间以上，需要重新聚焦
	lastSampleTimepoint = now;
	if (0 == nFocusSelLayer) bIsCurrentNeedFocus = false;

	DBG_MSG("是否聚焦 = %s \n", bIsCurrentNeedFocus != 0 ? "Yes" : "No");
	if (bIsCurrentNeedFocus)
	{
		{
			ALLOW_TEST_CONTEXT ctx = { 0 }; 
			ctx.TaskId = taskID;
			m_BillHelper->AddStatusInformation(ThmeInformationType, TaskFocus, &ctx, sizeof(ALLOW_TEST_CONTEXT));
		}
		if (bopen < 1)
		{
			bopen = 1;
	
			// Modify at 2022.12.23 by sy.hu
			// 聚焦前定位由AI识别改为传统算法:
			//    每采集一张图片就分析图片中是否可能存在疑似小细胞轮廓
			//    1.如果疑似的数量到达阈值（比如5个）就中断定位，在当前疑似位置开始聚焦
			//    2.如果所有位置的疑似疑似小细胞数量都没有达到设定的阈值，则选择疑似小细胞数量最多的位置开始聚焦
			//    3.如果都没有疑似小细胞，则定位失败，不需要聚焦，直接采图

#pragma region 聚焦前定位

			DBG_MSG("开始聚焦前定位。。。 \n");

            ULONGLONG tickFocusBeforeCaptureStart = GetTickCount64();

			int nRowIndex = 0;
			int nBeforeFocusPicCount = 0;
			std::map<int, _Point> mapBFocusPos;
			std::map<int, std::vector<cv::Rect>> mapBFocusResult;
			bool bBFocusPosSuccess = false;
			for (int nRow = 0; nRow < nBeforeFocusPosRow*(nBeforeFocusPosYSpace + 1); nRow += (nBeforeFocusPosYSpace + 1))
			{
				for (int nCol = 0; nCol < nBeforeFocusPosColumn*(nBeforeFocusPosXSpace + 1); nCol += (nBeforeFocusPosXSpace + 1))
				{
					int xBPos = nBeforeFocusPosXOffset + ((nRowIndex % 2 == 0) ? nCol : (nBeforeFocusPosColumn*(nBeforeFocusPosXSpace + 1) - (nCol + nBeforeFocusPosXSpace) - 1));
					int yBPos = nBeforeFocusPosYOffset + nRow;
					mapBFocusPos.emplace(++nBeforeFocusPicCount, _Point{ xBPos, yBPos });
					//printf("(%d,%d) ", xPos, yPos);

					DBG_MSG("聚焦前定位 Move40x(%d, %d)\n", xBPos, yBPos);
					status = Move40x(lc_obj, finish_info->ChannelNum, xBPos, yBPos);
					if (status != STATUS_SUCCESS)
					{
						DBG_MSG("聚焦前定位 Move40x(%d, %d) err= %u\n", xBPos, yBPos, status);
					}
					Sleep(nCapturePicDelayMs);//-----sleep-----------
					CString str;
					FOCUS_PARAMS params = { 0 };
					str.Format(_T("%d.jpg"), nBeforeFocusPicCount);
					sprintf(BeforceFocusPath, FocusPicPath);
					PathAppend(BeforceFocusPath, "loc");
					if (!PathFileExistsA(BeforceFocusPath)) CreateDirectoryA(BeforceFocusPath, NULL);
					strcpy_s(rec_focus_full_record.ReslutRelativePath, BeforceFocusPath);
					PathAppend(BeforceFocusPath, str);
					strcpy_s(rec_focus_full_record.GrapPath, BeforceFocusPath);
					// 聚焦前定位采图
					//CaptureCameraGrap(ca_obj, 0, BeforceFocusPath); 
					//DBG_MSG("fGradient BeforceFocusPath = %s", BeforceFocusPath);
					FRAME_FILE_INFO *ffifo = (FRAME_FILE_INFO*)calloc(1, sizeof(FRAME_FILE_INFO));
					CaptureCameraGrapBuffer(ca_obj, 0, BeforceFocusPath, *ffifo);
					// 图片处理:
					std::vector<cv::Rect> contours;
					LocationCellPos((HBITMAP)ffifo->hRawGrapBuffer, contours);
					mapBFocusResult[nBeforeFocusPicCount] = contours;
					// 异步保存图片文件
					m_SavePicWorker.appendTask([=](void *ctx){
						if (!ctx)
						{
							return;
						}
						FRAME_FILE_INFO *ffi = (FRAME_FILE_INFO *)ctx;
						if (ffi)
						{
							PROGRAM_OBJECT         ca_obj;
							PBILL3_EXTENSION       pExtension;
							FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
							ca_obj = pExtension->o_dev.s_ca_dev;
							SaveCaptureCameraGrapBuffer(ca_obj, *ffi); // 保存图片
							free(ffi);
						}
					}, (void*)ffifo);
					if (contours.size() >= locationSmallCellThreshold) // 5
					{
						goto LOC_END;
					}

					// 高速采图聚焦定位方法
					/*if (g_isBibaiize)
					{
						CString* tLocPicPath = new CString;
						*tLocPicPath = BeforceFocusPath;
						std::shared_ptr<CString> sp(tLocPicPath);
						auto f = std::async(std::launch::async, [&](std::shared_ptr<CString> tSp) {
							BinarizaFindCell(nBeforeFocusPicCount, tSp->GetBuffer());
						}, sp);
						FindCell(1, nBeforeFocusPicCount, BeforceFocusPath, &params);
						f.get();
						DBG_MSG("debug:BinarizaFindCell get");
					}

//#ifdef LOC_GRADIDENT_AND_AI
					if(!g_isHspeedCaptureImage)
					{
						FindCell(1, nBeforeFocusPicCount, BeforceFocusPath, &params);
						rec_focus_full_record.g_rc.rGrapSize = 0; // 聚焦定位标志0
						rec_focus_full_record.g_rc.rIndex = (USHORT)nBeforeFocusPicCount;
						rec_focus_full_record.rTaskID = taskID;
						status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_focus_full_record, sizeof(rec_focus_full_record), NULL, 0, FALSE, FALSE);
						if (status != STATUS_SUCCESS)//识别聚焦图片
						{
							DBG_MSG("[taskId=%d]聚焦前定位图片识别错误 Rec rec_focus_full_record Error = %#x\n", taskID, status);
						}
					}*/
//#endif									
				}
				nRowIndex++;
			}
LOC_END:
            ULONGLONG tickFocusBeforeCaptureEnd = GetTickCount64();
            DBG_MSG("Focus Before Capture Image time consuming: %llu ms (%llu-%llu)", (tickFocusBeforeCaptureEnd - tickFocusBeforeCaptureStart), tickFocusBeforeCaptureEnd, tickFocusBeforeCaptureStart); // 毫秒数

			// 获取有细胞的位置(优先级：1.细胞最多的图片位置。2.清晰度最高的位置)
			int loc = 0;
			PathRemoveFileSpec(BeforceFocusPath);
			if (GetBeforeFocusResult(BeforceFocusPath, loc, mapBFocusPos, mapBFocusResult) && loc > 0)
			{
				bBFocusPosSuccess = true;
				DBG_MSG("聚焦前定位 Postion(%d, %d) maybe cell count = %d\n", mapBFocusPos[loc].x, mapBFocusPos[loc].y, mapBFocusResult[loc].size());
				// 移动到有细胞的位置
				status = Move40x(lc_obj, finish_info->ChannelNum, mapBFocusPos[loc].x, mapBFocusPos[loc].y);
				if (status != STATUS_SUCCESS)
				{
					DBG_MSG("聚焦前定位结束 Move40x(%d, %d) err= %u\n", mapBFocusPos[loc].x, mapBFocusPos[loc].y, status);
				}
			}
			else
			{
				Move40x(lc_obj, finish_info->ChannelNum, 0, 0);
				DBG_MSG("聚焦前定位失败\n");
			}
			
            ULONGLONG tickFocusBeforeCaptureEnd2 = GetTickCount64();
            DBG_MSG("Focus Before Calc Result time consuming: %llu ms (%llu-%llu)", (tickFocusBeforeCaptureEnd2 - tickFocusBeforeCaptureEnd), tickFocusBeforeCaptureEnd2, tickFocusBeforeCaptureEnd); // 毫秒数

#pragma endregion
			Sleep(nBeforeFocusDelayMs); // 聚焦前定位结束后,聚焦前延时
#pragma region 聚焦选层
            ULONGLONG tickFocusCaptureStart = GetTickCount64();

			if (bFocusPostionFailStillFocus) bBFocusPosSuccess = true;
			if (bBFocusPosSuccess) // 只有在聚焦前定位成功的情况下才聚焦，否则直接采图
			{
				DBG_MSG("开始聚焦选层。。。 \n");

				strcpy_s(rec_focus_full_record.ReslutRelativePath, FocusPicPath);
				status = StartFocus(lc_obj);
				if (status != STATUS_SUCCESS)
				{
					DBG_MSG("[taskId=%d]识别聚焦进入调焦模式错误 = %#x\n", taskID, status);
				}
				int nTotalFoucesLayer = nEnd / nStep;
				int nInitMoveLayer = (nTotalFoucesLayer + 1) / 2;
				DBG_MSG("聚焦[task=%d]：总层数[%d]、层距[%d]、初始移动层数[%d]\n", taskID, nTotalFoucesLayer, nStep, nInitMoveLayer);
				MoveZ(lc_obj, Foreward, nInitMoveLayer*nStep); // 移动Z轴到一端
				CDirection direction = Foreward;
				int Zdistance = 0;
				Sleep(200);
				// 先把所有聚焦层图片采集下来，再分析
				std::map<int, FRAME_FILE_INFO *> mapFocusPic;
				vector<FOCUSIMAGE> vecFocusPic;
				
                ULONGLONG tickFocusCaptureBegin = GetTickCount64();
				while (nTotalStep < nEnd) // 默认180/6=30层
				{
					if (nTotalStep > 0)
					{
						// 聚焦采图延时
						Sleep(nCapturePicDelayMs);
					}
					CString num;
					num.Format(_T("%d.jpg"), picNum);
					PathAppend(FocusPicPath, num);
					FOCUSIMAGE fimage = {};
					fimage.path = FocusPicPath;
					//FRAME_FILE_INFO *ffifo = (FRAME_FILE_INFO*)calloc(1, sizeof(FRAME_FILE_INFO));
					FRAME_FILE_INFO ffifo = {};
					DWORD tf1 = GetTickCount();
					CaptureCameraGrapBuffer(ca_obj, 0, FocusPicPath, ffifo);
					//mapFocusPic[picNum] = ffifo; // 缓存聚焦图片信息
					PathRemoveFileSpec(FocusPicPath);
					picNum++;
					nTotalStep += nStep;
					DBG_MSG("聚焦[task=%d]：总层数[%d]、层距[%d]、采集第几层[%d][%d]\n", taskID, nTotalFoucesLayer, nStep, picNum - 1, nTotalStep);
					MoveZ(lc_obj, Backward, nStep);
					FOCUS_PARAMS params = {};
                    fimage.hImage = (HBITMAP)ffifo.hRawGrapBuffer;
                    FindCell(picNum - 1, (HBITMAP)ffifo.hRawGrapBuffer, &params, fimage);
					//DeleteObject((HBITMAP)ffifo.hRawGrapBuffer);
					vecFocusPic.push_back(fimage);
					DBG_MSG("聚焦[task=%d]采图%d：%d ms", taskID, picNum-1, (GetTickCount() - tf1));
				}
                ULONGLONG tickFocusCaptureOver = GetTickCount64();
                DBG_MSG("Focus Capture Image time consuming: %llu ms (%llu-%llu)", (tickFocusCaptureOver - tickFocusCaptureBegin), tickFocusCaptureOver, tickFocusCaptureBegin); // 毫秒数
                
                ULONGLONG tickFocusRecAnalyzeBegin = GetTickCount64();
				{
					// 目前只有单个识别，所以必须控制：聚焦时暂停细胞图片识别，聚焦完成后，继续识别细胞图片
					std::shared_ptr<AutoRallThreadpoolPause> rallTP = (bSnycFocusAndRec ? std::make_shared<AutoRallThreadpoolPause>(m_worker): nullptr);
					// 聚焦图片采集完成，现在分析
					for (int i = 0; i < vecFocusPic.size(); i++)
					{
						auto & pic = vecFocusPic[i];
                        if (pic.hImage != NULL && !pic.path.empty())
                        {
                            // 保存图片
                            //imwrite(pic.path, pic.image);
                            FRAME_FILE_INFO ffi = { 0 };
                            strcpy_s(ffi.FrameFilePath, pic.path.c_str());
                            ffi.hRawGrapBuffer = pic.hImage;
                            ffi.nDHIndex = 0;
                            SaveCaptureCameraGrapBuffer(ca_obj, ffi); // 保存图片
                            pic.hImage = NULL;

							int nFocusPicNum = i+1;
							const char *pSzFocusPicPath = pic.path.c_str();

							rec_focus_full_record.g_rc.rGrapSize = 1; // 聚焦识别标志1
							rec_focus_full_record.g_rc.rIndex = (USHORT)nFocusPicNum;
							rec_focus_full_record.rTaskID = taskID;
							strcpy_s(rec_focus_full_record.GrapPath, pSzFocusPicPath);
							status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_focus_full_record, sizeof(rec_focus_full_record), NULL, 0, FALSE, FALSE);
							if (status != STATUS_SUCCESS)//识别聚焦图片
							{
								DBG_MSG("[taskId=%d]识别聚焦图片错误 Rec rec_focus_full_record Error = %#x\n", taskID, status);
							}
						}
					}
					//for (auto itr = mapFocusPic.begin(); itr != mapFocusPic.end(); itr++)
					//{
					//	if (itr->second) free(itr->second);
					//}
				}

				/*while (nTotalStep < nEnd) // 默认180/6=30层
				{
					FOCUS_PARAMS params;
					CString num;
					num.Format(_T("%d.jpg"), picNum);
					PathAppend(FocusPicPath, num);
					DBG_MSG("聚焦: %s\n", FocusPicPath);

					DWORD time1 = GetTickCount();
					CaptureCameraGrap(ca_obj, 0, FocusPicPath); // 聚焦采图
					DBG_MSG("fGradient FocusPicPath = %s", FocusPicPath);
					FindCell(1, picNum, FocusPicPath, &params);
					DWORD time2 = GetTickCount();
					DBG_MSG("聚焦采图(一张)时间： %d ms\n", (time2 - time1));
					//#ifdef  LOC_GRADIDENT_AND_AI	
					if (!g_isHspeedCaptureImage)
					{
						rec_focus_full_record.g_rc.rGrapSize = 1; // 聚焦识别标志1
						rec_focus_full_record.g_rc.rIndex = (USHORT)picNum;
						rec_focus_full_record.rTaskID = taskID;
						strcpy_s(rec_focus_full_record.GrapPath, FocusPicPath);
						status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_focus_full_record, sizeof(rec_focus_full_record), NULL, 0, FALSE, FALSE);
						if (status != STATUS_SUCCESS)//识别聚焦图片
						{
							DBG_MSG("[taskId=%d]识别聚焦图片错误 Rec rec_focus_full_record Error = %#x\n", taskID, status);
							//continue;
						}
					}
					//#endif //  LOC_GRADIDENT_AND_AI
					PathRemoveFileSpec(FocusPicPath);
					picNum++;
					nTotalStep += nStep;
					DBG_MSG("聚焦[task=%d]：总层数[%d]、层距[%d]、移动到第几层[%d][%d]\n", taskID, nTotalFoucesLayer, nStep, picNum - 1, nTotalStep);
					MoveZ(lc_obj, Backward, nStep);
					//Sleep(50);//-----sleep-----------
				}*/

				BOOL bFocusBySmallCell = FALSE;
				int nlastlayer = GetFocusResult(rec_obj, finish_info->TaskID, vecFocusPic, bFocusBySmallCell);
				DBG_MSG("聚焦[task=%d]：总层数[%d]、层距[%d]、清晰层[%d]、修正层数[%d]\n", taskID, nTotalFoucesLayer, nStep, nlastlayer, AutoFocusResultOpt);

				if (nlastlayer == -1)
				{
					MoveZ(lc_obj, Foreward, nStep*((nTotalFoucesLayer - 1) / 2 + 1));
					//chanelIsFoucusOk[ChannelNum] = false;
				}
				else
				{
					if (nTotalFoucesLayer > (nlastlayer + AutoFocusResultOpt) && (nlastlayer + AutoFocusResultOpt) > 0)//可以进行聚焦清晰层修正
						nlastlayer += AutoFocusResultOpt;

					DBG_MSG("聚焦[task=%d]通道[%d]：总层数[%d]、层距[%d]、清晰层[%d]、反向移动[%d][%d]\n", taskID, ChannelNum, nTotalFoucesLayer, nStep, nlastlayer, (nTotalFoucesLayer - nlastlayer), (nEnd - nStep*nlastlayer));
					MoveZ(lc_obj, Foreward, (nEnd - nStep*nlastlayer));
					chanelIsFoucusOk[ChannelNum] = nlastlayer;
				}
				if (bFocusBySmallCell)
				{
					if (bSaveZposAfterFocus)
					{
						status = SaveZ(lc_obj);
						if (status != STATUS_SUCCESS)
						{
							DBG_MSG("[taskId=%d]识别聚焦保存当前焦距错误 = %#x\n", taskID, status);
						}
					}
				}
				status = StopFocus(lc_obj);
				if (status != STATUS_SUCCESS)
				{
					DBG_MSG("[taskId=%d]识别聚焦退出调焦模式错误 = %#x\n", taskID, status);
				}
				Sleep(nCapturePicDelayMsAutoFocus);
				DBG_MSG("自动聚焦后延时延时：%d ms", nCapturePicDelayMsAutoFocus);

                ULONGLONG tickFocusRecAnalyzeOver = GetTickCount64();
                DBG_MSG("Focus RecAnalyze time consuming: %llu ms (%llu-%llu)", (tickFocusRecAnalyzeOver - tickFocusRecAnalyzeBegin), tickFocusRecAnalyzeOver, tickFocusRecAnalyzeBegin); // 毫秒数
			}
			else
			{
				auto itr = chanelIsFoucusOk.find(ChannelNum);
				if (itr != chanelIsFoucusOk.end())
				{
					MoveZ(lc_obj, Foreward, nEnd-nStep*itr->second);
					DBG_MSG("聚焦前定位失败，移动到通道[%d]上一次聚焦正确的层数：%d", ChannelNum, itr->second);
				}
			}

            ULONGLONG tickFocusCaptureEnd = GetTickCount64();
            DBG_MSG("Focus Capture RecAnalyze time consuming: %llu ms (%llu-%llu)", (tickFocusCaptureEnd - tickFocusCaptureStart), tickFocusCaptureEnd, tickFocusCaptureStart); // 毫秒数
#pragma endregion
		}
		/*DBG_MSG("开始聚焦选层。。。 \n");
		//找这幅图是否有细胞
		if (bopen<5)
		{
		CString csCellfind = _T("");
		csCellfind.Format(_T("findcell_%d.jpg"), bopen);
		PathAppend(FocusPicPath, csCellfind);
		// 聚焦采图
		CaptureCameraGrap(ca_obj, 0, FocusPicPath);

		FOCUS_PARAMS params;
		FindCell(1, picNum, FocusPicPath, &params, FALSE);
		//DeleteFile(FocusPicPath);
		DBG_MSG("聚焦找清晰图片(fSharpness:%.3f): %s\n", params.fSharpness, FocusPicPath);
		PathRemoveFileSpec(FocusPicPath);

		if (params.fSharpness >= fFoucsSharpness)
		{
		status = StartFocus(lc_obj);
		if (status != STATUS_SUCCESS)
		{
		DBG_MSG("[taskId=%d]识别聚焦进入调焦模式错误 = %#x\n", taskID, status);
		}
		int nTotalFoucesLayer = nEnd / nStep;
		int nInitMoveLayer = (nTotalFoucesLayer + 1) / 2;
		DBG_MSG("聚焦[task=%d]：总层数[%d]、层距[%d]、初始移动层数[%d]\n", taskID, nTotalFoucesLayer, nStep, nInitMoveLayer);
		MoveZ(lc_obj, Foreward, nInitMoveLayer*nStep); // 移动Z轴到一端
		Sleep(200);
		while (nTotalStep < nEnd) // 默认180/6=30层
		{
		WORD zStep = 0;
		CString num;
		num.Format(_T("%d.jpg"), picNum);
		PathAppend(FocusPicPath, num);
		DBG_MSG("聚焦: %s\n", FocusPicPath);

		CaptureCameraGrap(ca_obj, 0, FocusPicPath); // 聚焦采图
		FindCell(1, picNum, FocusPicPath, &params);

		rec_focus_full_record.g_rc.rGrapSize = 1; // 聚焦识别标志1
		rec_focus_full_record.g_rc.rIndex = (USHORT)picNum;
		rec_focus_full_record.rTaskID = taskID;
		strcpy_s(rec_focus_full_record.GrapPath, FocusPicPath);
		status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_focus_full_record, sizeof(rec_focus_full_record), NULL, 0, FALSE, FALSE);
		if (status != STATUS_SUCCESS)//识别聚焦图片
		{
		DBG_MSG("[taskId=%d]识别聚焦图片错误 Rec rec_focus_full_record Error = %#x\n", taskID, status);
		//continue;
		}

		PathRemoveFileSpec(FocusPicPath);
		picNum++;
		nTotalStep += nStep;
		DBG_MSG("聚焦[task=%d]：总层数[%d]、层距[%d]、移动到第几层[%d][%d]\n", taskID, nTotalFoucesLayer, nStep, picNum-1, nTotalStep);
		MoveZ(lc_obj, Backward, nStep);
		Sleep(100);
		}

		int nlastlayer = GetFocusResult(rec_obj, finish_info->TaskID);
		DBG_MSG("聚焦[task=%d]：总层数[%d]、层距[%d]、清晰层[%d]\n", taskID, nTotalFoucesLayer, nStep, nlastlayer);
		if (nlastlayer == -1)
		{
		MoveZ(lc_obj, Foreward, nStep*((nTotalFoucesLayer - 1) / 2 + 1));
		}
		else
		{
		DBG_MSG("聚焦[task=%d]：总层数[%d]、层距[%d]、清晰层[%d]、反向移动[%d][%d]\n", taskID, nTotalFoucesLayer, nStep, nlastlayer, (nTotalFoucesLayer - nlastlayer), (nEnd - nStep*nlastlayer));
		MoveZ(lc_obj, Foreward, (nEnd - nStep*nlastlayer));
		}
		Sleep(2000);
		/ *status = SaveZ(lc_obj);
		if (status != STATUS_SUCCESS)
		{
		DBG_MSG("[taskId=%d]识别聚焦保存当前焦距错误 = %#x\n", taskID, status);
		}* /
		status = StopFocus(lc_obj);
		if (status != STATUS_SUCCESS)
		{
		DBG_MSG("[taskId=%d]识别聚焦退出调焦模式错误 = %#x\n", taskID, status);
		}

		bopen = 5;
		}
		else
		{
		++bopen;
		}
		}*/
	}

	// 采图
#pragma region 采图

	int nIndex = 0;
	for (int nX = 0; nX < row; nX++)
	{
		// 高速采图，每一行连续采图
		if (g_isHspeedCaptureImage)
		{
			DBG_MSG("debug:开始移动%d行", nX);
			StartMoveNewRow(lc_obj, finish_info->ChannelNum, nX, nHSpeedValue);
			Sleep(nCapturePicDelayMs);
			for (int nY = 0; nY < column; nY++)
			{
				if (QueryLCPicHaveEnd())
				{
					DBG_MSG("debug:QueryLCPicHaveEnd(),下位机移动已经到行尾,通道号:%d 行号:%d\n", m_PicHaveEndRet.ChannelNumber, m_PicHaveEndRet.nRow);
					ResetLCPicHaveEnd();
					if (m_PicHaveEndRet.ChannelNumber == finish_info->ChannelNum && nX == m_PicHaveEndRet.nRow)
					{
						m_PicHaveEndThenPicNew = true;
						DBG_MSG("debug:QueryLCPicHaveEnd(),下位机移动已经到行尾");
						MessageBox(NULL, "采图列数设置过大", "提示", MB_OK);
						break;
					}
				}
				nIndex++;
				DBG_MSG("Task(%d) nSample40xCounter(%d) index %d\n", finish_info->TaskID, nSample40xCounter, nIndex);
				WaitForSingleObject(m_hPauseEvent, INFINITE);
				{
					std::lock_guard<std::mutex> lck(pArctx->mutex);
					if (m_BillHelper->QueryTaskOverByFail())
					{
						status = STATUS_CANCEL_ACTION;
						m_bCancelTask = TRUE;
						DBG_MSG("Sample40xGrapControl STATUS_CANCEL_ACTION\n");
						goto LOOP_END;
					}
				}

				sprintf_s(temp, "%s\\%.3u_%.4u.jpg", GrapPath, 40, (nIndex - 1));

				// 真正40倍采图
				//status = CaptureCameraGrap(ca_obj, 0, temp);
				FRAME_FILE_INFO *ffi = (FRAME_FILE_INFO*)calloc(1, sizeof(FRAME_FILE_INFO));
				status = CaptureCameraGrapBuffer(ca_obj, 0, temp, *ffi);

				DBG_MSG("debug:当前采集图片(%d,%d)", nX, nY);
				if (nSample40xCounter != 0)
				{
					s_prg.nPos = (float)(nX + 1) / (float)row * 100;
					m_BillHelper->AddStatusInformation(ThmeInformationType, SamplingProgress, &s_prg, sizeof(s_prg));
				}
				if (status != STATUS_SUCCESS)
				{
					DBG_MSG("采图错误:Sample40xGrapControl->CaptureCameraGrap Err = %#x\n", status);
					if (ffi) free(ffi);
					continue;
				}

				rec_full_record.g_rc.rGrapSize = 40;
				rec_full_record.g_rc.rIndex = (USHORT)(nIndex - 1);
				rec_full_record.rTaskID = taskID;
				strcpy_s(rec_full_record.GrapPath, temp);
#ifndef ASYNC_REC
				status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_full_record, sizeof(rec_full_record), NULL, 0, FALSE, FALSE);
				if( status != STATUS_SUCCESS )
				{
					DBG_MSG("Rec Grap Error = %u\n", status);
					continue;
				}

				pos         = (float)(nIndex) / (float)nSample40xCounter;
				s_prg.nPos  = (int)(pos * 100.0f);
				m_BillHelper->AddStatusInformation(ThmeInformationType, RecognitionProgress, &s_prg, sizeof(RECOGNITION_PROGRESS));

				if (nIndex == nSample40xCounter)
				{
					s_info.nTaskID = taskID;
					s_info.nSampleIndex = rec_full_record.g_rc.rIndex;
					s_info.nGrapSize = rec_full_record.g_rc.rGrapSize;
					s_info.Coefficient = 1.0f;
					m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));
				}

#else // 异步识别（一边采图，一边识别）
				PREC_GRAP_CONTEXT_WARP pRgcw = (PREC_GRAP_CONTEXT_WARP)calloc(1, sizeof(REC_GRAP_CONTEXT_WARP));
				if (pRgcw == NULL)
				{
					DBG_MSG("40x Rec Grap alloc memeory Error\n");
					std::lock_guard<std::mutex> lck(pArctx->mutex);
					pArctx->nRecCompleteCount++;
					if (pArctx->nRecCompleteCount == nSample40xCounter)
					{
						s_info.nTaskID = taskID;
						s_info.nSampleIndex = rec_full_record.g_rc.rIndex;
						s_info.nGrapSize = rec_full_record.g_rc.rGrapSize;
						s_info.Coefficient = 1.0f;
						m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));
						m_bCancelTask = TRUE;
					}
					continue;
				}
				char buff[64];
				rec_full_record.Reserved[1] = EnableQCRec;
				memcpy(&pRgcw->rgfr, &rec_full_record, sizeof(REC_GRAP_FULL_RECORD));
				pRgcw->pContext = this;
				pRgcw->pContext2 = rec_obj;
				pRgcw->pContext3 = ffi;
				pRgcw->recCtx = pCtx;

				m_SavePicWorker.appendTask([=](void *ctx){
					if (!ctx)
					{
						return;
					}
					PREC_GRAP_CONTEXT_WARP p = (PREC_GRAP_CONTEXT_WARP)ctx;
					FRAME_FILE_INFO *ffi = (FRAME_FILE_INFO *)p->pContext3;
					if (ffi)
					{
						PROGRAM_OBJECT         ca_obj;
						PBILL3_EXTENSION       pExtension;
						FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
						ca_obj = pExtension->o_dev.s_ca_dev;
						SaveCaptureCameraGrapBuffer(ca_obj, *ffi); // 保存图片
						free(ffi);
						p->pContext3 = NULL;
					}
					DBG_MSG("[%d]%d加入识别队列\n", p->rgfr.rTaskID, p->rgfr.g_rc.rIndex);
					m_worker.appendTask([=](void *context){
						if (!context)
						{
							return;
						}
						PREC_GRAP_CONTEXT_WARP p = (PREC_GRAP_CONTEXT_WARP)context;
						p->rgfr.Reserved[2] = ChannelNum;
						DBG_MSG("[%d]%d开始识别\n", p->rgfr.rTaskID, p->rgfr.g_rc.rIndex);
						KSTATUS status = FMMC_EasySynchronousBuildPRP(p->pContext2, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &p->rgfr, sizeof(REC_GRAP_FULL_RECORD), NULL, 0, FALSE, FALSE);
						if (status != STATUS_SUCCESS)
						{
							DBG_MSG("Rec Grap Error = %u\n", status);
						}
						PRP_REC_COMPLETE_CALLBACK2((CUSArc *)p->pContext, p);
					}, ctx);
				}, (void*)pRgcw);

				/*DBG_MSG("[%d]%d加入识别队列\n", taskID, (nIndex - 1));
				m_worker.appendTask([=](void *ctx){
					if (!ctx)
					{
						return;
					}
					PREC_GRAP_CONTEXT_WARP p = (PREC_GRAP_CONTEXT_WARP)ctx;
					FRAME_FILE_INFO *ffi = (FRAME_FILE_INFO *)p->pContext3;
					if (ffi)
					{
						PROGRAM_OBJECT         ca_obj;
						PBILL3_EXTENSION       pExtension;
						FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
						ca_obj = pExtension->o_dev.s_ca_dev;
						SaveCaptureCameraGrapBuffer(ca_obj, *ffi); // 保存图片
						free(ffi);
					}
					p->rgfr.Reserved[2] = ChannelNum;
					DBG_MSG("[%d]%d开始识别\n", p->rgfr.rTaskID, p->rgfr.g_rc.rIndex);
					KSTATUS status = FMMC_EasySynchronousBuildPRP(p->pContext2, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &p->rgfr, sizeof(REC_GRAP_FULL_RECORD), NULL, 0, FALSE, FALSE);
					if (status != STATUS_SUCCESS)
					{
						DBG_MSG("Rec Grap Error = %u\n", status);
					}
					PRP_REC_COMPLETE_CALLBACK2((CUSArc *)p->pContext, p);
				}, (void*)pRgcw);*/
#endif 
				if (QueryLCPicHaveEnd())
				{
					DBG_MSG("debug:QueryLCPicHaveEnd(),下位机移动已经到行尾,通道号:%d 行号:%d\n", m_PicHaveEndRet.ChannelNumber, m_PicHaveEndRet.nRow);
					ResetLCPicHaveEnd();
					if (m_PicHaveEndRet.ChannelNumber == finish_info->ChannelNum && nX == m_PicHaveEndRet.nRow)
					{
						m_PicHaveEndThenPicNew = true;
						DBG_MSG("debug:QueryLCPicHaveEnd(),下位机移动已经到行尾");
						MessageBox(NULL, "采图列数设置过大", "提示", MB_OK);
						break;
					}
				}
			}

			if (m_PicHaveEndThenPicNew)
			{
				m_PicHaveEndThenPicNew = false;
				continue;
			}
			EndMoveNewRow(lc_obj, finish_info->ChannelNum, nX);
			DBG_MSG("debug:结束移动%d行", nX);
		}
		else  // 正常采图
		{
            ULONGLONG tickCellCaptureStart = GetTickCount64();

			for (int nY = 0; nY < column; nY++)
			{
				int xPos = (nX % 2 == 0) ? nY : (column - nY - 1);
				int yPos = nX;
				nIndex = nX*column + nY + 1;

				DBG_MSG("Task(%d) nSample40xCounter(%d) index %d\n", finish_info->TaskID, nSample40xCounter, nIndex);
				WaitForSingleObject(m_hPauseEvent, INFINITE);
				{
					std::lock_guard<std::mutex> lck(pArctx->mutex);
					if (m_BillHelper->QueryTaskOverByFail())
					{
						status = STATUS_CANCEL_ACTION;
						m_bCancelTask = TRUE;
						DBG_MSG("Sample40xGrapControl STATUS_CANCEL_ACTION\n");
						goto LOOP_END;
					}
				}
				for (USHORT nRetry = 0; nRetry < RETRY_COUNTER; nRetry++)
				{
					status = Move40x(lc_obj, finish_info->ChannelNum, xPos, yPos);
					if (status != STATUS_SUCCESS)
					{
						DBG_MSG("Move40x(%d, %d) err= %u\n", xPos, yPos, status);
						continue;
					}
					DBG_MSG("Move40x retry %d\n", nRetry);
					break;
				}
				// 移动后，添加延时
				Sleep(nCapturePicDelayMs);

				sprintf_s(temp, "%s\\%.3u_%.4u.jpg", GrapPath, 40, (nIndex - 1));

				// 真正40倍采图
				//status = CaptureCameraGrap(ca_obj, 0, temp);
				FRAME_FILE_INFO *ffi = (FRAME_FILE_INFO*)calloc(1, sizeof(FRAME_FILE_INFO));
				status = CaptureCameraGrapBuffer(ca_obj, 0, temp, *ffi);

				if (nSample40xCounter != 0)
				{
					s_prg.nPos = (float)(nIndex) / (float)nSample40xCounter * 100;
					m_BillHelper->AddStatusInformation(ThmeInformationType, SamplingProgress, &s_prg, sizeof(s_prg));
				}
				if (status != STATUS_SUCCESS)
				{
					DBG_MSG("采图错误:Sample40xGrapControl->CaptureCameraGrap Err = %#x\n", status);
					if (ffi) free(ffi);
					continue;
				}

				rec_full_record.g_rc.rGrapSize = 40;
				rec_full_record.g_rc.rIndex = (USHORT)(nIndex - 1);
				rec_full_record.rTaskID = taskID;
				strcpy_s(rec_full_record.GrapPath, temp);
#ifndef ASYNC_REC
				status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_full_record, sizeof(rec_full_record), NULL, 0, FALSE, FALSE);
				if( status != STATUS_SUCCESS )
				{
					DBG_MSG("Rec Grap Error = %u\n", status);
					continue;
				}

				pos = (float)(nIndex) / (float)nSample40xCounter;
				s_prg.nPos = (int)(pos * 100.0f);
				m_BillHelper->AddStatusInformation(ThmeInformationType, RecognitionProgress, &s_prg, sizeof(RECOGNITION_PROGRESS));

				if (nIndex == nSample40xCounter)
				{
					s_info.nTaskID = taskID;
					s_info.nSampleIndex = rec_full_record.g_rc.rIndex;
					s_info.nGrapSize = rec_full_record.g_rc.rGrapSize;
					s_info.Coefficient = 1.0f;
					m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));
				}

#else // 异步识别（一边采图，一边识别）
				PREC_GRAP_CONTEXT_WARP pRgcw = (PREC_GRAP_CONTEXT_WARP)calloc(1, sizeof(REC_GRAP_CONTEXT_WARP));
				if (pRgcw == NULL)
				{
					DBG_MSG("40x Rec Grap alloc memeory Error\n");
					std::lock_guard<std::mutex> lck(pArctx->mutex);
					pArctx->nRecCompleteCount++;
					if (pArctx->nRecCompleteCount == nSample40xCounter)
					{
						s_info.nTaskID = taskID;
						s_info.nSampleIndex = rec_full_record.g_rc.rIndex;
						s_info.nGrapSize = rec_full_record.g_rc.rGrapSize;
						s_info.Coefficient = 1.0f;
						m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));
						m_bCancelTask = TRUE;
					}
					continue;
				}
				char buff[64];
				rec_full_record.Reserved[1] = EnableQCRec;
				memcpy(&pRgcw->rgfr, &rec_full_record, sizeof(REC_GRAP_FULL_RECORD));
				pRgcw->pContext = this;
				pRgcw->pContext2 = rec_obj;
				pRgcw->pContext3 = ffi;
				pRgcw->recCtx = pCtx;
				
				// 先加入异步队列中保存文件，然后再把任务加入到识别队列中
				m_SavePicWorker.appendTask([=](void *ctx){
					if (!ctx)
					{
						return;
					}
					PREC_GRAP_CONTEXT_WARP p = (PREC_GRAP_CONTEXT_WARP)ctx;
					FRAME_FILE_INFO *ffi = (FRAME_FILE_INFO *)p->pContext3;
					if (ffi)
					{
						PROGRAM_OBJECT         ca_obj;
						PBILL3_EXTENSION       pExtension;
						FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
						ca_obj = pExtension->o_dev.s_ca_dev;
						SaveCaptureCameraGrapBuffer(ca_obj, *ffi); // 保存图片
						free(ffi);
						p->pContext3 = NULL;
					}
					DBG_MSG("[%d]%d加入识别队列\n", p->rgfr.rTaskID, p->rgfr.g_rc.rIndex);
					m_worker.appendTask([=](void *context){
						if (!context)
						{
							return;
						}
						PREC_GRAP_CONTEXT_WARP p = (PREC_GRAP_CONTEXT_WARP)context;
						p->rgfr.Reserved[2] = ChannelNum;
						DBG_MSG("[%d]%d开始识别\n", p->rgfr.rTaskID, p->rgfr.g_rc.rIndex);
						KSTATUS status = FMMC_EasySynchronousBuildPRP(p->pContext2, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &p->rgfr, sizeof(REC_GRAP_FULL_RECORD), NULL, 0, FALSE, FALSE);
						if (status != STATUS_SUCCESS)
						{
							DBG_MSG("Rec Grap Error = %u\n", status);
						}
						PRP_REC_COMPLETE_CALLBACK2((CUSArc *)p->pContext, p);
					}, ctx);
				}, (void*)pRgcw);

				/*DBG_MSG("[%d]%d加入识别队列\n", taskID, (nIndex - 1));
				m_worker.appendTask([=](void *ctx){
					if (!ctx)
					{
						return;
					}
					PREC_GRAP_CONTEXT_WARP p = (PREC_GRAP_CONTEXT_WARP)ctx;
					FRAME_FILE_INFO *ffi = (FRAME_FILE_INFO *)p->pContext3;
					if (ffi)
					{
						PROGRAM_OBJECT         ca_obj;
						PBILL3_EXTENSION       pExtension;
						FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
						ca_obj = pExtension->o_dev.s_ca_dev;
						SaveCaptureCameraGrapBuffer(ca_obj, *ffi); // 保存图片
						free(ffi);
					}
					p->rgfr.Reserved[2] = ChannelNum;
					DBG_MSG("[%d]%d开始识别\n", p->rgfr.rTaskID, p->rgfr.g_rc.rIndex);
					KSTATUS status = FMMC_EasySynchronousBuildPRP(p->pContext2, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &p->rgfr, sizeof(REC_GRAP_FULL_RECORD), NULL, 0, FALSE, FALSE);
					if (status != STATUS_SUCCESS)
					{
						DBG_MSG("Rec Grap Error = %u\n", status);
					}
					PRP_REC_COMPLETE_CALLBACK2((CUSArc *)p->pContext, p);
				}, (void*)pRgcw);*/
#endif
			}
            ULONGLONG tickCellCaptureEnd = GetTickCount64();
            DBG_MSG("Capture Cell Image time consuming: %llu ms (%llu-%llu)", (tickCellCaptureEnd - tickCellCaptureStart), tickCellCaptureEnd, tickCellCaptureStart); // 毫秒数
		}	
	}
	if (nIndex != nSample40xCounter)
		m_vecNpicTask.push_back(N_PIC_TARGET_TASK{ taskID, nIndex });

#pragma endregion

LOOP_END:
	if (m_bCancelTask)
	{
		ALLOW_TEST_CONTEXT ctx = {0};
		ctx.TaskId = taskID;
		m_BillHelper->AddStatusInformation(ThmeInformationType, RespondTaskFail, &ctx, sizeof(ALLOW_TEST_CONTEXT));
	}

#ifdef ASYNC_REC
	pArctx.release();
#endif

	return STATUS_SUCCESS; //表示整个流程顺利完成
}

/*
KSTATUS CUSArc::Sample40xGrapControl( PROGRAM_OBJECT lc_obj, PROGRAM_OBJECT ca_obj, PROGRAM_OBJECT rec_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info, HANDLE hA10x )
{
	char                     GrapPath[MAX_PATH];
	float                    pos;
	ULONG                    nSample40xCounter, j;
	KSTATUS                  status;
	PSAMPLE_40X_INFO         s_40_i;
	SAMPLING_PROGRESS        s_prg;
	REC_GRAP_FULL_RECORD     rec_full_record = {0};

	REC_GRAP_FULL_RECORD     rec_focus_full_record = {0};

	char  FocusPicPath[MAX_PATH] = {0};//聚焦图片路径2019
	int nStep  = GetPrivateProfileInt(_T("Focus"),_T("step"),50,focusini);
	int nEnd   = GetPrivateProfileInt(_T("Focus"),_T("end"),500,focusini);
	int nTime  = GetPrivateProfileInt(_T("Focus"),_T("time"),1,focusini);
	int Offset = GetPrivateProfileInt(_T("Focus"),_T("offset"), 1000, focusini);
	int nSwitch = GetPrivateProfileInt(_T("Focus"),_T("switch"), 1, focusini); 
	int nFocusWay = GetPrivateProfileInt(_T("Focus"),_T("FocusWay"), 0, focusini); 
	int nSavePic  = GetPrivateProfileInt(_T("Focus"),_T("SavePic"), 0, focusini); 
	int nFocusSelLayer = GetPrivateProfileInt(_T("Focus"),_T("FoucsSelLayer"), 1, focusini);
	int nTotalStep = 0;
	int  picNum = 1; 
	
	nSample40xCounter = AnalysePic10x_GetSample40xGrapCounter(hA10x);
	if( nSample40xCounter == 0 )
	{
		DBG_MSG("AnalysePic10x_GetSample40xGrapCounter NULL...\n");
		//return STATUS_NOT_FOUND_VALUE;
		return STATUS_SUCCESS;
	}
	
	s_40_i = (PSAMPLE_40X_INFO)malloc(sizeof(SAMPLE_40X_INFO) * nSample40xCounter);
	if( s_40_i == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	ZeroMemory(s_40_i, sizeof(SAMPLE_40X_INFO) * nSample40xCounter);
	if( AnalysePic10x_GetSample40xGrapInfo(hA10x, s_40_i, nSample40xCounter) == FALSE )
	{
		DBG_MSG("AnalysePic10x_GetSample40xGrapInfo fail...\n");
		free(s_40_i);
		return STATUS_NOT_FOUND_VALUE;
		
	}
	
	m_BillHelper->GetRecGrapReslutPath(GrapPath);
	strcpy_s(rec_full_record.ReslutRelativePath, GrapPath);

	strcpy_s(FocusPicPath,GrapPath);//聚焦图片路径2019
 

	char temp[MAX_PATH]={0};
	sprintf_s(temp,"%d\\FocusPic",finish_info->TaskID);

	PathAppend(FocusPicPath,temp);

	strcpy_s(rec_focus_full_record.ReslutRelativePath,FocusPicPath); 

    DBG_MSG("FocusPicPath  %s\n",FocusPicPath);
	if( PathFileExists(FocusPicPath) == FALSE )//创建聚焦图片文件夹2019
	{
		CreateDirectory(FocusPicPath, NULL);
		DWORD cc = GetLastError();
		DBG_MSG("FocusPicPath  %d\n",cc);
	}

	PathAppend(GrapPath, "Result.ini");
	DeleteFile(GrapPath);
	PathRemoveFileSpec(GrapPath);
	PathAppend(GrapPath, "temp_40x.jpg");
	s_prg.nTaskID = finish_info->TaskID;

	int bopen = 1;

	
	for( j = 0 ; j < nSample40xCounter ; j ++ )
	{
		DBG_MSG("nSample40xCounter index  %d\n",j);
		WaitForSingleObject(m_hPauseEvent, INFINITE);
		if( m_BillHelper->QueryTaskOverByFail() )
		{
			status = STATUS_CANCEL_ACTION;
			DBG_MSG("Sample10xGrapControl STATUS_CANCEL_ACTION\n");
			break;
		}

		for( USHORT nRetry = 0 ; nRetry < RETRY_COUNTER ; nRetry ++ )
		{
			status = Move40x(lc_obj, finish_info->ChannelNum, &s_40_i[j]);

			if( status != STATUS_SUCCESS )
			{
				DBG_MSG("Move40x err= %u\n", status);

				if( nRetry + 1 == RETRY_COUNTER )
					return status;
				continue;
			}
			DBG_MSG("Move40x  %d\n",nRetry);
			break;
		}

		DBG_MSG("[%s][%s]FocusSelLayer = %d \n", __FILE__, __FUNCTION__, nFocusSelLayer);
		if (0 != nFocusSelLayer)
		{
			DBG_MSG("开始聚集选层。。。 \n");
			//找这幅图是否有细胞
			if (bopen<5)
			{
				CString csCellfind=_T("");
				csCellfind.Format(_T("findcell.jpg"));
				PathAppend(FocusPicPath,csCellfind);
				DBG_MSG("找细胞  :%s\n",FocusPicPath);
				CaptureCameraGrap(ca_obj, 0, FocusPicPath);

				FOCUS_PARAMS params;
				FindCell(1,picNum,FocusPicPath,&params);
				DeleteFile(FocusPicPath);
				PathRemoveFileSpec(FocusPicPath);

				DBG_MSG("fSharpness  :%f\n",params.fSharpness);

				if (params.fSharpness>4.5)
					//if (j==0)
				{
					MoveZ(lc_obj, Foreward, nStep*(nEnd/nStep-1)/2);
					while( nTotalStep < nEnd )
					{
						WORD zStep = 0;
						CString num=_T("");
						num.Format(_T("\\temp%d.jpg"),picNum);
						PathAppend(FocusPicPath,num);

						DBG_MSG("FindCell  :%s\n",FocusPicPath);

						CaptureCameraGrap(ca_obj, 0, FocusPicPath);
						FindCell(1,picNum,FocusPicPath,&params);

						//if (params.fSharpness<4.5)
						//{
						//}

						DBG_MSG("FocusPicPath2  %s\n",FocusPicPath);

						rec_focus_full_record.g_rc.rGrapSize = 1;
						rec_focus_full_record.g_rc.rIndex    =(USHORT)picNum;
						rec_focus_full_record.rTaskID = finish_info->TaskID;
						strcpy_s(rec_focus_full_record.GrapPath,FocusPicPath);
						status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_focus_full_record, sizeof(rec_focus_full_record), NULL, 0, FALSE, FALSE);
						if( status != STATUS_SUCCESS )//识别聚焦图片
						{
							DBG_MSG("Rec rec_focus_full_record Error = %u\n", status);
							//continue;
						}

						PathRemoveFileSpec(FocusPicPath);
						picNum++;
						nTotalStep += nStep;
						MoveZ(lc_obj, Backward, nStep);
					}

					int nlastlayer= GetFocusResult(rec_obj,finish_info->TaskID);

					if (nlastlayer== -1)
					{
						MoveZ(lc_obj,Foreward,nStep*((nEnd/nStep-1)/2+1));
					}
					else
					{
						MoveZ(lc_obj,Foreward,nStep*(nEnd/nStep-nlastlayer));
					}

					//MoveZBack(lc_obj);

					Sleep(2000);
					//MoveZ(lc_obj,Backward,nStep*nlastlayer);
					//Sleep(2000);
					bopen=5;  

				}
				else
				{
					++bopen;
					continue;
					//MoveZ(lc_obj, Foreward, nStep*(nEnd/nStep-1)/2);
				}

			}
		}
		
		status = CaptureCameraGrap(ca_obj, 0, GrapPath);
		if( status != STATUS_SUCCESS )
		{
			DBG_MSG("Sample40xGrapControl->CaptureCameraGrap Err = %x\n", status);
			continue;
		}
		
		rec_full_record.g_rc.rGrapSize = 40;
		rec_full_record.g_rc.rIndex    = (USHORT)j;
		rec_full_record.rTaskID        = finish_info->TaskID;
		strcpy_s(rec_full_record.GrapPath, GrapPath);
		/ *status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_full_record, sizeof(rec_full_record), NULL, 0, FALSE, FALSE);
		if( status != STATUS_SUCCESS )
		{
			DBG_MSG("Rec Grap Error = %u\n", status);
			continue;
		}* /
		
		pos         = (float)(j+1) / (float)nSample40xCounter;
		s_prg.nPos  = (int)(pos * 50.0f);
		s_prg.nPos += 50;
		//m_BillHelper->AddStatusInformation(ThmeInformationType, SamplingProgress, &s_prg, sizeof(s_prg));


		FMMC_EasyasynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_full_record, sizeof(rec_full_record), &s_prg, sizeof(s_prg), PRP_REC_COMPLETE_CALLBACK, (PVOID)m_BillHelper);
	}
	
	free(s_40_i);

	return STATUS_SUCCESS; //表示整个流程顺利完成
	
}*/

void CUSArc::LCUSSampleFinish( PROGRAM_OBJECT lc_obj, UCHAR  nChannel)
{
	LC_COMMAND                     lc_cmd = {0};
	//PLC_SAMPLE_FINISH_TYPE         s_finish = (PLC_SAMPLE_FINISH_TYPE)&lc_cmd;
	//s_finish->lc_header.flag = LC_FLAG;
	//s_finish->lc_header.nLCCmd = LC_SAMPLE_FINISH;
	//s_finish->FinishSampleInfo.ChannelNum = nChannel;
	
	lc_cmd.lc_header.flag = LC_FLAG;
	lc_cmd.lc_header.nLCCmd = UC_CAPTURE_IMAGE_FINISH;

	FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}

KSTATUS CUSArc::CaptureCameraGrap( PROGRAM_OBJECT CaObj,UCHAR nCaIndex,PCHAR pFilePath )
{
	USHORT                 idx = nCaIndex;
	HANDLE                 hRawBuffer = NULL;
	KSTATUS                status;
	FRAME_FILE_INFO        file_info = {0};
	
	DWORD t1 = GetTickCount();
	status = FMMC_EasySynchronousBuildPRP(CaObj, NULL, PRP_AC_READ, RAW_GRAP_BUFFER, &idx, sizeof(USHORT), &hRawBuffer, sizeof(HANDLE), FALSE, FALSE);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("CaptureCameraGrap->Read Buffer Error = %x...\n",status);
		return status;
	}
	DWORD t2 = GetTickCount();
	DBG_MSG("开始采图 time: %d\n", (t1));
	DBG_MSG("FMMC_EasySynchronousBuildPRP RAW_GRAP_BUFFER time: %d\n", (t2-t1));
	
	file_info.hRawGrapBuffer = hRawBuffer;
	file_info.nDHIndex       = nCaIndex;
	strcpy_s(file_info.FrameFilePath,pFilePath);
	DWORD t3 = GetTickCount();
	status = FMMC_EasySynchronousBuildPRP(CaObj, NULL, PRP_AC_WRITE, FRAME_FILE, (PVOID)&file_info, sizeof(file_info), NULL, 0, FALSE, FALSE);
	DWORD t4 = GetTickCount();
	DBG_MSG("FMMC_EasySynchronousBuildPRP FRAME_FILE time: %d\n", (t4 - t3));
	DBG_MSG("结束采图 time: %d\n", (GetTickCount()));

	return status;	
}

KSTATUS CUSArc::CaptureCameraGrapBuffer(PROGRAM_OBJECT CaObj, UCHAR nCaIndex, PCHAR pFilePath, FRAME_FILE_INFO &ffi)
{
	USHORT                 idx = nCaIndex;
	HANDLE                 hRawBuffer = NULL;
	KSTATUS                status;

	DWORD t1 = GetTickCount();
	DBG_MSG("before RAW_GRAP_BUFFER");
	status = FMMC_EasySynchronousBuildPRP(CaObj, NULL, PRP_AC_READ, RAW_GRAP_BUFFER, &idx, sizeof(USHORT), &hRawBuffer, sizeof(HANDLE), FALSE, FALSE);
	DBG_MSG("after RAW_GRAP_BUFFER");
	if (status != STATUS_SUCCESS)
	{
		DBG_MSG("CaptureCameraGrap->Read Buffer Error = %x...\n", status);
		return status;
	}
	DWORD t2 = GetTickCount();
	DBG_MSG("开始采图 time: %d\n", (t1));
	DBG_MSG("FMMC_EasySynchronousBuildPRP RAW_GRAP_BUFFER time: %d\n", (t2 - t1));

	ffi.hRawGrapBuffer = hRawBuffer;
	ffi.nDHIndex = nCaIndex;
	strcpy_s(ffi.FrameFilePath, pFilePath);

	return status;
}

KSTATUS CUSArc::CaptureCameraGrapBuffer(PROGRAM_OBJECT CaObj, UCHAR nCaIndex, HBITMAP &hBitmap)
{
	FRAME_FILE_INFO ffi = { 0 };
	KSTATUS status = FMMC_EasySynchronousBuildPRP(CaObj, NULL, PRP_AC_WRITE, FRAME_FILE, (PVOID)&ffi, sizeof(ffi), NULL, 0, FALSE, FALSE);
	hBitmap = (HBITMAP)ffi.hRawGrapBuffer;
	return status;
}

KSTATUS CUSArc::SaveCaptureCameraGrapBuffer(PROGRAM_OBJECT CaObj, FRAME_FILE_INFO &ffi)
{
	return FMMC_EasySynchronousBuildPRP(CaObj, NULL, PRP_AC_WRITE, FRAME_FILE, (PVOID)&ffi, sizeof(ffi), NULL, 0, FALSE, FALSE);
}

KSTATUS CUSArc::StartUSEvent( PFINISH_STANDING_INFO pStandInfo ) //静止时间到,发出开始US采集事件
{
	if( Que_InsertData(m_USSampleTaskQue, pStandInfo) == FALSE )
		return STATUS_ALLOC_MEMORY_ERROR;
	
	SetEvent(m_Request_Event);
	return STATUS_SUCCESS;
}

KSTATUS CUSArc::ControlUSSample( BOOL bPause )
{
	if( bPause == TRUE )
		ResetEvent(m_hPauseEvent);
	else
		SetEvent(m_hPauseEvent);

	return STATUS_SUCCESS;
	
}

KSTATUS CUSArc::USStartNewRow(PMOVE_NEW_ROW_REQ tDate)
{
	InterlockedExchange(&m_LCPicHaveEnd,1);
	m_PicHaveEndRet.ChannelNumber = tDate->ChannelNumber;
	m_PicHaveEndRet.nRow = tDate->nRow;
	return STATUS_SUCCESS;
}

KSTATUS CUSArc::StartBarcodeEvent(PBARCODE_REQ_CONTEXT ctx)
{
	if (Que_InsertData(m_BarcodeSampleTaskQue, ctx) == FALSE)
		return STATUS_ALLOC_MEMORY_ERROR;

	SetEvent(m_Barcode_Request_Event);
	return STATUS_SUCCESS;
}

BOOL CUSArc::QueryLCPicHaveEnd()
{
	return (BOOL)(InterlockedExchangeAdd(&m_LCPicHaveEnd, 0) != 0);
}

void CUSArc::ResetLCPicHaveEnd()
{
	InterlockedExchange(&m_LCPicHaveEnd, 0);
}

void CUSArc::StopUSArc()
{
	InterlockedExchange(&m_ExitThread, 1);

	if( m_hPauseEvent )
		SetEvent(m_hPauseEvent);
	if( m_Request_Event )
		SetEvent(m_Request_Event);

	if( m_work_thread )
	{
		WaitForSingleObject(m_work_thread, INFINITE);
		CloseHandle(m_work_thread);
	}

	if (m_Barcode_Request_Event)
		SetEvent(m_Barcode_Request_Event);

	Que_ReleaseList(m_USSampleTaskQue);
	Que_ReleaseList(m_BarcodeSampleTaskQue);
}

std::string ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

std::wstring s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

void CUSArc::FindCell(int type, int num, PCHAR Path, PFOCUS_PARAMS params, BOOL bCache /*= TRUE*/)
{
	int nGradientWay = GetPrivateProfileInt(_T("Focus"), _T("GradientWay"), 0, focusini);

	if(PathFileExists(Path) == TRUE )
	{
		std::wstringstream wss;
		FOCUS_PARAMS  Fparams = {0};
		IMAGEQUALITY  imgquality = {0};

		wss<<Path;

		if (nGradientWay == 0)
		{
			if ( RecFun != NULL ) 
			{ 
				Fparams.fGradient = RecFun(wss.str().c_str());	
				DBG_MSG("RecFun fGradient = %d", imgquality.fGradient);
				Fparams.fSharpness = 0;
			}			
		}
		else
		{
			if (AnalysisFileFun != NULL)
			{
				std::wstring wstr = s2ws(Path);
				if (AnalysisFileFun(wstr.c_str(), imgquality))
				{
					DBG_MSG("AnalysisFileFun fGradient = %d", imgquality.fGradient);
					Fparams.fGradient = imgquality.fGradient;
					Fparams.fSharpness = imgquality.fSharpness;
				}
			}
		}

		if( type == 2 )
			Fparams.nLayer = num;
		else if( type == 1 )
			Fparams.nLayer = num;

		if (bCache)
		{
			m_vecFocus.push_back(Fparams);
		}

		params->fGradient = Fparams.fGradient;
		params->fSharpness = Fparams.fSharpness;

		DBG_MSG("m_vecFocus  Fparams.nLayer:%d\n",Fparams.nLayer);
		//if( bSavePic == FALSE )
		//	DeleteFile(file_info.FrameFilePath);
	}

}

void CUSArc::FindCell(int num, HBITMAP hBitmap, PFOCUS_PARAMS params, FOCUSIMAGE &fi, BOOL bCache /*= TRUE*/)
{
	try
	{
		cv::Mat tmp, src;
		if (HBitmapToMat(hBitmap, tmp))
		{
			fi.image = tmp;
			FOCUS_PARAMS  Fparams = { 0 };
			IMAGEQUALITY  imgquality = { 0 };
			cv::resize(tmp, src, cv::Size(720, 480));
			cv::Mat gray, graySrc, dst;
			cv::cvtColor(src, gray, CV_BGR2GRAY);
			/*graySrc = gray.clone();
			cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);
			cv::Canny(gray, gray, 100, 250);
			cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			for (int i = 0; i < 5; i++) {
				cv::morphologyEx(gray, gray, cv::MORPH_DILATE, element);
			}
			for (int i = 0; i < 0; i++) {
				cv::morphologyEx(gray, gray, cv::MORPH_ERODE, element);
			}
			fillHole(gray, gray);
			double  Otsu = 0;
			Otsu = cv::threshold(gray, dst, Otsu, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
			std::vector<std::vector<cv::Point> > contours;
			std::vector<cv::Vec4i> hierarchy;
			cv::findContours(dst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

			cv::Mat tempImg(graySrc.size(), CV_8UC1, cv::mean(graySrc));
			for (int i = 0; i < contours.size(); i++)
			{
				cv::RotatedRect rr = cv::minAreaRect(cv::Mat(contours[i]));
				//去掉面积大的和面积小的
				auto area = rr.size.area();
				if (area >= 15 * 15 || area <= 70 * 70)
				{
					cv::Rect rt = cv::boundingRect(cv::Mat(contours[i]));
					graySrc(rt).copyTo(tempImg(rt));

				}
			}
			*/
			cv::Mat imageSobel;
			//cv::Sobel(tempImg, imageSobel, CV_16U, 1, 1);
			cv::Sobel(gray, imageSobel, CV_16U, 1, 1);
			double meanValue = mean(imageSobel)[0];

			Fparams.nLayer = num;
			Fparams.fGradient = meanValue;
			Fparams.fSharpness = 0;

			if (bCache)
			{
				m_vecFocus.push_back(Fparams);
			}

			params->fGradient = Fparams.fGradient;
			params->fSharpness = Fparams.fSharpness;

			fi.gradient = Fparams.fGradient;

			DBG_MSG("m_vecFocus num:%d, fGradient:%f\n", num, Fparams.fGradient);
		}
	}
	catch (cv::Exception& e)
	{
		DBG_MSG("%s Catch cv exception: %s\n", __FUNCTION__, e.what());
	}
	catch (std::exception& e)
	{
		DBG_MSG("%s Catch std exception: %s\n", __FUNCTION__, e.what());
	}
	catch (...)
	{
		DBG_MSG("%s Catch unknown exception!\n", __FUNCTION__);
	}
}

int CUSArc::GetFocusResult(PROGRAM_OBJECT rec_obj, ULONG nID, vector<FOCUSIMAGE> &vecFi, BOOL &bFocusBySmallCell)
{
	float nMaxGradient = 0 ,nMaxSharpness = 0;
	CHAR  nLayer = 0;  
	char  temp[MAX_PATH] = {0},FocusIni[MAX_PATH] = {0},Key[MAX_PATH] = {0},ConfIni[MAX_PATH] = {0}, PicPath[MAX_PATH] = {0};

	int   nCount = 0,nLayerCount = 0;
	BOOL  bFind = FALSE;
	FOCUS_PARAMS Layer[64] = {0};

	GetModuleFileName(NULL,FocusIni,MAX_PATH);
	PathRemoveFileSpec(FocusIni);

	strcpy_s(ConfIni,FocusIni);
	PathAppend(FocusIni, REC_GRAP_PATH);
	_snprintf_s(temp, sizeof(temp)-1, "%d\\FocusPic\\%s", nID, RESULT_FILE);
	PathAppend(FocusIni,temp);

	strcpy_s(PicPath,FocusIni);
	PathRemoveFileSpec(PicPath);

	PathAppend(ConfIni,"Conf.ini");

	int nWay = GetPrivateProfileInt("sys", "FocusWay", 0, ConfIni);
	int nDirection = GetPrivateProfileInt("sys", "nDirection", 0, ConfIni);

	nCount = m_vecFocus.size();
	if( nCount == 0 )
		return 0;

	if( nWay == 0 )//取最大值
	{
		DBG_MSG("nWay 000000000"); // 找到梯度值最大的层
		for ( int i = 0; i <  m_vecFocus.size(); i++ )
		{
			if ( nMaxGradient < m_vecFocus[i].fGradient )
			{
				nMaxGradient = m_vecFocus[i].fGradient;
				nLayer = m_vecFocus[i].nLayer;			
			}
			
			_snprintf_s(temp, sizeof(temp)-1,"fGradient:%f,fSharpness:%f",m_vecFocus[i].fGradient,m_vecFocus[i].fSharpness);
			_snprintf_s(Key, sizeof(Key) - 1, "%d", i + 1);
			WritePrivateProfileString("focus",Key,temp,FocusIni);
		}
		_snprintf_s(temp, sizeof(temp) - 1, "%d", nLayer);
		WritePrivateProfileString("sys","Clearlayer",temp,m_FocusIniPath);
       
		//GetFocusResultEX(nID);
	}
	else if( nWay == 1 )
	{
		for ( int i = 0; i <  nCount; i++ )
		{
			if ( nMaxGradient < m_vecFocus[i].fGradient )
			{
				nMaxGradient = m_vecFocus[i].fGradient;
				nLayer = m_vecFocus[i].nLayer;	
			}

			if( i > 0 && i < nCount - 1 )
			{
				if( m_vecFocus[i].fGradient < m_vecFocus[i - 1].fGradient && m_vecFocus[i].fGradient < m_vecFocus[i + 1].fGradient )
				{

					Layer[nLayerCount].nLayer = m_vecFocus[i].nLayer;	
					Layer[nLayerCount].fGradient = m_vecFocus[i].fGradient;	
					Layer[nLayerCount].nIndex = i;	
					Layer[nLayerCount].nPoor = m_vecFocus[i - 1].fGradient + m_vecFocus[i + 1].fGradient - 2 * (m_vecFocus[i].fGradient);


					DBG_MSG("c1 Layer[%d].nLayer:%d,Layer[%d].nPoor:%d,nLayerCount:%d\n",nLayerCount,m_vecFocus[i].nLayer,nLayerCount,Layer[nLayerCount].nPoor,nLayerCount);
					nLayerCount++;				
					bFind = TRUE;
				}
			}

			_snprintf_s(temp, sizeof(temp) - 1, "fGradient:%f,fSharpness:%f", m_vecFocus[i].fGradient, m_vecFocus[i].fSharpness);
			_snprintf_s(Key, sizeof(Key)-1, sizeof(temp) - 1, "%d", i + 1);
			WritePrivateProfileString("focus",Key,temp,FocusIni);
		}

		if( bFind == TRUE )
		{
			//if( nLayerCount > 1 )
			{		
				int nMaxPoor = 0;

				for( int i = 0; i < nLayerCount; i++ )
				{
					if( Layer[i].fGradient < nMaxGradient / 2 )
					{
						continue;
					}
					else if( nMaxPoor < Layer[i].nPoor )
					{
						DBG_MSG("c2\n nMaxPoor:%d, nLayer:%d ;nPoor:%d\n",nMaxPoor,Layer[i].nLayer,Layer[i].nPoor);
						nMaxPoor = Layer[i].nPoor;
						nLayer = Layer[i].nLayer;
					}
					/*else if( nMaxPoor == Layer[i].nPoor )
					{
					}*/
					
				}
			}
			/*else
			{
				DBG_MSG("c3\n");
				nLayer = Layer[0].nLayer;
			}*/
		}		
	}
	else if( nWay == 2 )
	{
		for ( int i = 0; i <  m_vecFocus.size(); i++ )
		{
			_snprintf_s(temp, sizeof(temp)-1,"fGradient:%f,fSharpness:%f ",m_vecFocus[i].fGradient,m_vecFocus[i].fSharpness);
			_snprintf_s(Key, sizeof(Key)-1, "%d", i+1);
			WritePrivateProfileString("focus",Key,temp,FocusIni);
		}

		nLayer = GetClearNum(nCount,m_vecFocus,nDirection);		
	}
	else if (nWay == 3) 
	{
		nLayer = 1;
		double maxGradient = 0.f;
		for (int i = 0; i < vecFi.size(); i++)
		{
			if (vecFi[i].gradient > maxGradient)
			{
				maxGradient = vecFi[i].gradient;
				nLayer = i + 1;
			}
			_snprintf_s(temp, sizeof(temp) - 1, "fGradient:%f", vecFi[i].gradient);
			_snprintf_s(Key, sizeof(Key) - 1, "%d", i + 1);
			WritePrivateProfileString("focus", Key, temp, FocusIni);
		}
	}
	
lc:	
	m_vecFocus.clear();
	DBG_MSG("GetFocusResult 111111111 path:%s   nLayer:%d\n", FocusIni, nLayer);
	if( nLayer == 0 )
		return 0;

	_snprintf_s(temp, sizeof(temp) - 1, "%d", nLayer);
	WritePrivateProfileString("sys","Clearlayer",temp,m_FocusIniPath);	
	WritePrivateProfileString("focus","Clearlayer",temp,FocusIni);

	// 再通过ai识别结果修正找到的清晰层
	nLayer = GetFocusResultByAIRec(rec_obj, nID, PicPath, nLayer, vecFi, bFocusBySmallCell);

	DBG_MSG("GetFocusResult 22222222 path:%s   nLayer:%d\n",FocusIni,nLayer);

	return nLayer;
}

BOOL CUSArc::GetBeforeFocusResult(PCHAR szPicPath, int &nPos)
{
	float nMaxGradient = 0, nMaxSharpness = 0;
	float nMaxBlock = 0;
	CHAR  nLayer = -1;
	CHAR  nLayer_bin = -1;
	char  temp[MAX_PATH] = { 0 }, IniPath[MAX_PATH] = { 0 }, Key[MAX_PATH] = { 0 }, ConfIni[MAX_PATH] = { 0 }, PicPath[MAX_PATH] = { 0 };

	int nRet = TRUE;

	int   nCount = 0, nLayerCount = 0;
	BOOL  bFind = FALSE;
	FOCUS_PARAMS Layer[64] = { 0 };

	_snprintf_s(IniPath, sizeof(IniPath) - 1, "%s\\%s", szPicPath, RESULT_FILE);

	nCount = m_vecFocus.size();
	if (nCount == 0)
		return 0;

	do
	{
		std::vector<int> vecLayers;
//#ifdef LOC_GRADIDENT_AND_AI
		if(!g_isHspeedCaptureImage)
		{
			// 1.找小细胞最多层
			std::map<int, AllCellsInfo> AllPicCells;
			GetAllPicCells(AllPicCells, IniPath);

			int nPicKey = -1, nMaxSamllCellCount = 0;
			for_each(AllPicCells.begin(), AllPicCells.end(), [&](std::map<int, AllCellsInfo>::reference r) {
				int nSmallCellCount =
					r.second.HCount +
					r.second.BCount +
					r.second.YHCount +
					r.second.MJCount +
					r.second.XBTCount +
					r.second.NSCount +
					r.second.LSCount +
					r.second.CSCount +
					r.second.KLCount +
					r.second.TMCount;
				DBG_MSG("定位[%d]层：%d\n", r.first, nSmallCellCount);
				if (nSmallCellCount > 0)
				{
					if (nSmallCellCount > nMaxSamllCellCount)
					{
						nPicKey = r.first;
						vecLayers.clear();
						vecLayers.push_back(nPicKey);
						nMaxSamllCellCount = nSmallCellCount;
					}
					else if (nSmallCellCount == nMaxSamllCellCount)
					{
						nPicKey = r.first;
						vecLayers.push_back(nPicKey);
					}
				}
			});
		}
//#endif // LOC_GRADIDENT_AND_AI


		// 2.如果没找到就就选择清晰度最大的层
		for (int i = 0; i < m_vecFocus.size(); i++)
		{
			if (nMaxGradient < m_vecFocus[i].fGradient)
			{
				nMaxGradient = m_vecFocus[i].fGradient;
				nLayer = m_vecFocus[i].nLayer;
			}
			_snprintf_s(temp, sizeof(temp) - 1, "fGradient:%.2f,fSharpness:%.2f ", m_vecFocus[i].fGradient, m_vecFocus[i].fSharpness);
			_snprintf_s(Key, sizeof(Key) - 1, "%d", i + 1);
			WritePrivateProfileString("locationPos", Key, temp, IniPath);
		}

		if (g_isBibaiize)
		{
			for (int i = 0; i < m_vecBvalue.size(); i++)
			{
				if (nMaxBlock < m_vecBvalue[i].nValue)
				{
					nMaxBlock = m_vecBvalue[i].nValue;
					nLayer_bin = m_vecBvalue[i].num;
				}
				_snprintf_s(temp, sizeof(temp) - 1, "Block:%d ", m_vecBvalue[i].nValue);
				_snprintf_s(Key, sizeof(Key) - 1, "%d", i + 1);
				WritePrivateProfileString("locationPos_bin", Key, temp, IniPath);
			}
		}

		if(!g_isHspeedCaptureImage)
		{
			if (vecLayers.size() > 0)
			{
				nPos = vecLayers[0];
				for (int i = 1; i < vecLayers.size(); i++)
				{
					if (m_vecFocus[vecLayers[i] - 1].fGradient > m_vecFocus[nPos - 1].fGradient)
					{
						nPos = vecLayers[i];
					}
				}
			}
			else if (nLayer != -1)
			{
				nPos = nLayer;
			}
			else
			{
				nRet = FALSE;
			}
		}
		else
		{
			if (g_isBibaiize)
			{
				if (nMaxBlock > 100)
				{
					nPos = nLayer_bin;
				}
				else
				{
					nPos = nLayer;
				}
			}
			else
			{
				if (nLayer != -1)
				{
					nPos = nLayer;
				}
				else
				{
					nRet = FALSE;
				}
			}
		}
	} while (0);

	m_vecFocus.clear();
	m_vecBvalue.clear();
	if (nRet)
	{
		_snprintf_s(temp, sizeof(temp) - 1, "%d", nPos);
		WritePrivateProfileString("locationPos", "ClearPos", temp, IniPath);
	}

	return nRet;
}

BOOL CUSArc::GetBeforeFocusResult(PCHAR szPicPath, int &nPos, std::map<int, _Point> &mapBFocusPos, std::map<int, std::vector<cv::Rect>> &mapBFocusResult)
{
	BOOL ret = FALSE;
	int locationSmallCellThreshold = abs((int)GetPrivateProfileInt("Focus", "locationSmallCellThreshold", 5, focusini));
	char section[32] = { 0 }, Key[32] = { 0 }, temp[256] = { 0 };
	char IniPath[256] = { 0 };
	strcpy_s(IniPath, szPicPath);
	PathAppend(IniPath, RESULT_FILE);

	do 
	{
		int maxSamllCellCount = 0, maxSamllCellPos = 0;
		for (auto itr = mapBFocusResult.begin(); itr != mapBFocusResult.end(); itr++)
		{
			std::vector<cv::Rect> &vec = itr->second;
			_snprintf_s(section, sizeof(section) - 1, "%d", itr->first);
			for (int i = 0; i < vec.size(); i++)
			{
				_snprintf_s(Key, sizeof(Key) - 1, "%d", i);
				_snprintf_s(temp, sizeof(temp) - 1, "%d,%d,%d,%d", vec[i].x, vec[i].y, vec[i].x + vec[i].width, vec[i].y + vec[i].height);
				WritePrivateProfileString(section, Key, temp, IniPath);
			}
			
			if (maxSamllCellCount < itr->second.size())
			{
				maxSamllCellCount = itr->second.size();
				maxSamllCellPos = itr->first;
			}
			if (itr->second.size() >= locationSmallCellThreshold)
			{
				nPos = itr->first;
				ret = TRUE;
				break;
			}
		}
		if (ret == FALSE && maxSamllCellCount > 0)
		{
			nPos = maxSamllCellPos;
			ret = TRUE;
		}
	} while (0);

	if (ret)
	{
		_snprintf_s(temp, sizeof(temp) - 1, "%d-(%d,%d)", nPos, mapBFocusPos[nPos].x, mapBFocusPos[nPos].y);
		WritePrivateProfileString("locationPos", "postion", temp, IniPath);
	}
	else
	{
		WritePrivateProfileString("locationPos", "postion", "not find", IniPath);
	}

	return ret;
}

int CUSArc::GetFocusPostResult( ULONG nID )
{
	DBG_MSG("GetFocusPostResult 222222222");
	float nMaxGradient = 0 ,nMaxSharpness = 0;
	CHAR  nLayer = 0;  
	char  temp[MAX_PATH] = {0}, FocusIni[MAX_PATH] = {0}, Key[MAX_PATH] = {0};
	int   nCount = 0,nLayerCount = 0;
	BOOL  bFind = FALSE;
	FOCUS_PARAMS Layer[64] = {0};

	GetModuleFileName(NULL,FocusIni,MAX_PATH);
	PathRemoveFileSpec(FocusIni);
	PathAppend(FocusIni, REC_GRAP_PATH);
	sprintf_s(temp,"%d\\FocusPostPic\\result.ini",nID);
	PathAppend(FocusIni,temp);

	nCount = m_vecFocus.size();

	if( nCount <= 0 )
		return TRUE;


	for ( int i = 0; i <  nCount; i++ )
	{
		if ( nMaxGradient < m_vecFocus[i].fGradient )
		{
			nMaxGradient = m_vecFocus[i].fGradient;
			nLayer = m_vecFocus[i].nLayer;	
		}		
		
		sprintf_s(temp,"fGradient:%.2f,fSharpness:%.2f ",m_vecFocus[i].fGradient,m_vecFocus[i].fSharpness);
		sprintf_s(Key,"%d",i+1);
		DBG_MSG("%s", temp);
		WritePrivateProfileString("focus",Key,temp,FocusIni);
	}

	m_vecFocus.clear();

	if( nLayer == 0 )
		return TRUE;

	sprintf_s(temp,"%d",nLayer);
	//WritePrivateProfileString("sys","Clealayer",temp,m_FocusIniPath);	

	WritePrivateProfileString("focus","Clealayer",temp,FocusIni);

	DBG_MSG("GetFocusPostResult 111111111 path:%s   nLayer:%d\n",FocusIni,nLayer);


	return nLayer;
}

int CUSArc::GetClearNum(int nLayerCount,std::vector<FOCUS_PARAMS> vecFocus,int nDirection)
{
	double temp;
	int    num = 0;
	float  nMaxGradient = 0;

	//for( int i = 0; i < nLayerCount; i++ )
	//{
	//	if( nMaxGradient < vecFocus[i].fGradient )
	//	{
	//		nMaxGradient = vecFocus[i].fGradient;
	//		num = vecFocus[i].nLayer;
	//	}
	//}

	//if( num == nLayerCount )
	//	return vecFocus[num].nLayer;
DBG_MSG("GetCleanNum  111111");
	if( nLayerCount <= 1 )
		return 0;
DBG_MSG("GetCleanNum 22222");

	// 1.查找梯度最大的聚焦层
	if ( vecFocus[0].fGradient > vecFocus[1].fGradient )
	{
		temp = vecFocus[0].fGradient;
		num = 0;
	}
	else
	{
		temp = vecFocus[1].fGradient;
		num = 1;
	}

	for (int i = 2 ; i < nLayerCount; i++)
	{
		if (temp < vecFocus[i].fGradient)
		{
			temp = vecFocus[i].fGradient;
			num = i;
		}
	}

	if( num < 2 )
		return vecFocus[num].nLayer;

	int a = 0;

	if ( ( int(vecFocus[num].fGradient/100000) < 10 ) && ( int(vecFocus[num].fGradient/100000) >= 1 ) )
	{
		a = 10000;
	}
	else if ( (int(vecFocus[num].fGradient/10000) < 10) && ( int(vecFocus[num].fGradient/10000) >= 1 ) )
	{
		a = 1000;
	}
	
	if( nDirection == 1 )
	{
		DBG_MSG("GetCleanNum 333333  a:%d",a);
		if( num + 1 >=  nLayerCount )
			return vecFocus[num].nLayer;
		else if( num + 2 >=  nLayerCount )
			return vecFocus[num + 1].nLayer;


		if ( (int(vecFocus[num].fGradient/a) == int(vecFocus[num + 1].fGradient/a) ) && ( (int(vecFocus[num].fGradient/a) != int(vecFocus[num + 2].fGradient/a)) ) )
		{
			if ( ( (vecFocus[num + 2]).fGradient / ( vecFocus[num + 1].fGradient) ) > 0.75 )
			{
				return vecFocus[num].nLayer;
			}

			//else
			//{
			//	return vecFocus[num - 1].nLayer
			//}

		}
		else if ( ( (vecFocus[num + 2].fGradient) / (vecFocus[num + 1]).fGradient ) <= 0.71 )
		{
			return vecFocus[num].nLayer;
		}

		return vecFocus[num + 1].nLayer;
	}
	else
	{
		DBG_MSG("GetCleanNum 4444444 a:%d",a);
		if ( (int(vecFocus[num].fGradient/a) == int(vecFocus[num-1].fGradient/a) ) && ( (int(vecFocus[num].fGradient/a) != int(vecFocus[num-2].fGradient/a)) ) )
		{
			if ( ( (vecFocus[num-2]).fGradient / ( vecFocus[num-1].fGradient) ) > 0.75 )
			{
				return vecFocus[num].nLayer;
			}

			//else
			//{
			//	return vecFocus[num - 1].nLayer
			//}

		}
		else if ( ( (vecFocus[num-2].fGradient) / (vecFocus[num-1]).fGradient ) <= 0.71 )
		{
			return vecFocus[num].nLayer;
		}

		if( num < 1 )
			return vecFocus[num].nLayer;
		else
			return vecFocus[num - 1].nLayer;

	}	
}

void CUSArc::GetFocusResultEX(ULONG nID)
{
	char  temp[MAX_PATH] = {0},temp1[MAX_PATH] = {0},FocusPath[MAX_PATH] = {0},PicName[MAX_PATH] = {0};
	int   nRt1 = 0,nRt2 = 0,nRt3 = 0,nRt4 = 0;

	m_nTotalFocusLyar = GetPrivateProfileInt("sys","nFocusLayer", 16, m_FocusIniPath);

	int nRow = GetPrivateProfileInt("sys","nFocusRow", 2, m_FocusIniPath);
	int nCol = GetPrivateProfileInt("sys","nFocusCol", 4, m_FocusIniPath);

	GetModuleFileName(NULL,FocusPath,MAX_PATH);
	PathRemoveFileSpec(FocusPath);
	PathAppend(FocusPath, REC_GRAP_PATH);
	sprintf_s(temp,"%d\\FocusPic",nID);
	PathAppend(FocusPath,temp);

	int nMaxIndex = GetPrivateProfileInt("sys","Clealayer",0,m_FocusIniPath);

	int nBeginIndex = nMaxIndex - 5;
	int nEndIndex = nMaxIndex + 5;

	if( nBeginIndex <= 0 )
		nBeginIndex = 1;
	if( nEndIndex > m_nTotalFocusLyar )
		nEndIndex = m_nTotalFocusLyar;



	DBG_MSG("nWay 000000000-1  %d,%d\n",nBeginIndex,nEndIndex);

	for( int i = nBeginIndex; i <= nEndIndex; i++ )
	{
		sprintf_s(PicName,"%d.jpg",i);
		DBG_MSG(PicName);
		CutPic(FocusPath,PicName,nRow,nCol);
	}


	int nMin = IdentifyEX(FocusPath,nRow,nCol,nMaxIndex);

	/*if( nMin == nMaxIndex )
	{
	GetPrivateProfileString("focus","fMaxGradient",0,temp1,MAX_PATH,FocusPath);	
	if( atof(temp1) > 35000 )
	nMin -= 2;
	}*/

	//nMin -= 1;

	sprintf_s(temp,"%d",nMin);
	WritePrivateProfileString("sys","Clealayer",temp,m_FocusIniPath);
	PathAppend(FocusPath,"result.ini");
	WritePrivateProfileString("focus","Clealayer",temp,FocusPath);
}

BOOL CUSArc::CutPic(PCHAR pPicPath,PCHAR pFileName,int nRow,int nCol)
{
	CxImage     Image1,smallImg1,smallImg2,smallImg3,smallImg4;
	char        PicPath[MAX_PATH] = {0},CutPicPath[MAX_PATH],temp[MAX_PATH] = {0};

	int         nPicIndex = 1;

	strcpy_s(PicPath,pPicPath);
	PathAppend(PicPath,pFileName);
	DBG_MSG("nWay 000000000-1  %s\n",PicPath);
	if( FALSE == PathFileExists( PicPath) ) 
		return FALSE;

	
	if( Image1.Load(PicPath, CXIMAGE_FORMAT_JPG) == FALSE )
		return FALSE;

	DBG_MSG("nWay 000000000-2\n");
	BYTE * buffer = 0;
	INT    size = 0;
	std::string sData;

	int nWidth  = 1440 / nCol;
	int nHight  = 960 / nRow;

	for( int i = 0; i < nRow; i++ )
	{
		for( int j = 0; j < nCol; j++ )
		{
			strcpy_s(CutPicPath,pPicPath);	
			sprintf_s(temp,"Pic%d",nPicIndex++);	
			PathAppend(CutPicPath,temp);
			CreateDirectory(CutPicPath, NULL);
			PathAppend(CutPicPath,pFileName);
			Image1.Crop( nWidth * j,nHight * i ,nWidth * ( j + 1),nHight * (i + 1 ),&smallImg1);
			smallImg1.Encode(buffer, size, CXIMAGE_FORMAT_JPG);
			sData = std::string((char*)buffer, size);	
			smallImg1.Save(CutPicPath,CXIMAGE_FORMAT_JPG); 
			//smallImg1.Save(CutPicPath,CXIMAGE_SUPPORT_JPG); 
		}
	}
	return TRUE;
}


int CUSArc::IdentifyEX(PCHAR pPicPath,int nRow,int nCol,int nMAXLayer)
{
	char PicFullPath[MAX_PATH] = {0};

	m_nTotalFocusLyar = GetPrivateProfileInt("sys","nFocusLayer", 0, m_FocusIniPath);

	if( pPicPath == NULL )
		return 0;

	FOCUS_PARAMS  Identify_Info = {0};
	IMAGEQUALITY  imgquality = {0};

	char          temp[MAX_PATH] = {0},FocusIni[MAX_PATH] = {0};

	BOOL  bFind = FALSE;
	FOCUS_PARAMS  Layer[64] = {0};
	int a[8] = {0};
	int c[8] = {0};
	int d[8] = {0};
	int bb[8] = {0};
	double ssum[64] = {0};
	CHAR  nLayer = 0; 
	int   nCount = 0,nLayerCount = 0;

	int nMin = 100000000;

	for( int nPicIndex = 1; nPicIndex <= nRow * nCol; nPicIndex++ )
	{
		sprintf_s(temp,"pic%d",nPicIndex);

		strcpy_s(PicFullPath,pPicPath);
		strcpy_s(FocusIni,pPicPath);

		PathAppend(PicFullPath,temp);

		PathAppend(FocusIni,temp);
		PathAppend(FocusIni,"result.ini");


		int z = 0;

		for( int i = 1; i <= m_nTotalFocusLyar; i++ )
		{

			size_t len = 0;
			size_t converted = 0;
			ZeroMemory(&Identify_Info,sizeof(Identify_Info));	
			sprintf_s(temp,"%d.jpg",i);	
			PathAppend(PicFullPath,temp);

			if( PathFileExists(PicFullPath) == FALSE )
			{
				PathRemoveFileSpec(PicFullPath);
				continue;
			}

			ATL::CImage Img;

			if (S_OK==Img.Load(PicFullPath))
			{
				int nWidth =0;
				nWidth = Img.GetWidth();//获取图像宽度

				int nHeight = 0;
				nHeight	= Img.GetHeight();//获取图像高度


				int nWidthbytes = (Img.GetBPP() * nWidth+31)/32 * 4;//获取图像每一行的字节数

				LPBYTE lpBits = (LPBYTE)Img.GetPixelAddress(0, nHeight-1);//图片数据首地址
				double a={0};
				byte r[360*480] = {0};
				byte g[360*480] = {0};
				byte b[360*480] = {0};

				int z=0;
				for (int j = 0; j<nHeight;++j)
				{
					for (int ii = 0 ; ii<nWidth; ++ii)
					{
						r[z] = lpBits[(nHeight - j - 1) * nWidthbytes+ii*3+2];
						g[z] = lpBits[(nHeight - j - 1) * nWidthbytes+ii*3+1];
						b[z] = lpBits[(nHeight - j - 1) * nWidthbytes+ii*3];
						z++;
					}
				}

				double sum = 0;
				int m = 0;
				for (int j = 0; j<nHeight;++j)
				{
					for (int iii = 0 ; iii<nWidth; ++iii)
					{

						int temp= ((int)r[m]+(int)g[m]+(int)b[m])/3;
						if (temp!=0)
						{
							a=(pow(((double)r[m]-temp),2)+pow(((double)g[m]-temp),2)+pow(((double)b[m]-temp),2))/3;
							sum+=a;
						}


						m++;

					}
				}

				ssum[i]= sum;

			}
			PathRemoveFileSpec(PicFullPath);
		}

		double MAX = 0;
		int  ntemp = 0;
		for(int x = 0; x < m_nTotalFocusLyar; x++)
		{
			if (ssum[x]==0)
			{
				continue;
			}
			if ( MAX < ssum[x] )
			{
				MAX = ssum[x];
				ntemp = x;
			}
		}


		a[nPicIndex - 1 ] = ntemp;

		sprintf_s(temp,"%d",ntemp);
		WritePrivateProfileString("focus","nMaxGradientIndex",temp,FocusIni);
		PathRemoveFileSpec(PicFullPath);	



		if( ntemp < nMin) 
			nMin = ntemp;
		FOCUS_LAYERS newlayer;
		if (m_vecLayers.size()>0)
		{
			int nflag = 1;
			std::vector<FOCUS_LAYERS>::iterator iter2;
			for (iter2 = m_vecLayers.begin();iter2!=m_vecLayers.end();++iter2)
			{
				if ((*iter2).nValue == nMin)
				{
					(*iter2).num+=1;
					nflag = 0;
					break;
				}
			}
			if (nflag)
			{
				
				newlayer.nValue = nMin;
				newlayer.num = 1;
				m_vecLayers.push_back(newlayer);
			}
		}
		else
		{
			newlayer.nValue = nMin;
			newlayer.num = 1;
			m_vecLayers.push_back(newlayer);
		}
	}





	memcpy(c,a,sizeof(int)*8);
	memcpy(d,a,sizeof(int)*8);

	if ( nMin != nMAXLayer )
	{
		if( abs(nMAXLayer - nMin) > 2  )
		{
			nMin = nMAXLayer;
		}
	}

	return nMin;
}

int CUSArc::GetFocusResultByAIRec(PROGRAM_OBJECT rec_obj, ULONG nID, PCHAR pFocusPath, int nCleanLayer, vector<FOCUSIMAGE> &vecFi, BOOL &bFocusBySmallCell)
{
	char temp[MAX_PATH] = { 0 }, IniPath[MAX_PATH] = { 0 }, FileName[MAX_PATH] = { 0 }, NewPicPath[MAX_PATH] = { 0 }, Key[MAX_PATH] = { 0 }, ConfIni[MAX_PATH] = { 0 };
	int  nCutCount = 1, nCutIndex = 0, nTryCount = 0, nRecIndex = 0, nRecTempIndex = 0;

	int  nMaxSmallCellCount = 0, nMaxSmallCellIndex = 0;
	BOOL bFindSmallMaxCountLayr = FALSE;
	BOOL bFocusEp = FALSE;

	int nMinEpCount = GetPrivateProfileInt("Focus", "MinEpCount", 25, focusini);
	int nMaxCutCount = GetPrivateProfileInt("Focus", "MaxCutCount", 5, focusini);
	int nFocusCount = abs((int)GetPrivateProfileInt("Focus", "FocusCount", 6, focusini));

	GetModuleFileName(NULL, ConfIni, MAX_PATH);
	PathRemoveFileSpec(ConfIni);
	PathAppend(ConfIni, "Conf.ini");
	int nFocusWay = GetPrivateProfileInt("sys", "FocusWay", 0, ConfIni);

	//int Step = GetPrivateProfileInt("Focus", "step", 8, focusini);
	//int End = GetPrivateProfileInt("Focus", "end", 240, focusini);

	m_nTotalFocusLyar = vecFi.size();

	RECT  CutRect[30] = { 0 };
	int   nTempMaxIndex[30] = { 0 };
	int   nMaxIndex = { 0 };

	//CellsIn20x CellImg = { };
	AllCellsInfo Cells = { };
	AllCellsInfo cacheCells = { };
	int nRt = 0;

	int nBegin = 1;
	int nEnd = nFocusCount;
	int haftLayr = 0;

	strcpy_s(NewPicPath, pFocusPath);

	strcpy_s(IniPath, pFocusPath);
	PathAppend(IniPath, RESULT_FILE);

	DBG_MSG("ini path %s\n", IniPath);

	if (nFocusWay == 3)
	{
		// 新的计算清晰层算法，采集所有聚焦层图像
		nFocusCount = m_nTotalFocusLyar;
		// 确定修正层数范围
		if (0 == nFocusCount)
		{
			return nCleanLayer;
		}
		nBegin = 1;
		nEnd = nFocusCount;
	}
	else
	{
		// 确定修正层数范围
		if (0 == nFocusCount)
		{
			return nCleanLayer;
		}
		haftLayr = (nFocusCount + 1) / 2;
		nBegin = nCleanLayer - haftLayr;
		nEnd = nCleanLayer + haftLayr;

		if (nEnd > m_nTotalFocusLyar)
			nEnd = m_nTotalFocusLyar;
		if (nBegin < 1)
			nBegin = 1;
		if (nEnd == -1)
		{
			return nCleanLayer;
		}
	}
	
	nRecTempIndex = nBegin;

Rec_lc:
	if (nRecTempIndex >= nBegin && nRecTempIndex <= nEnd)
	{
		nRecIndex = nRecTempIndex;
		DBG_MSG("nRecIndex %d\n", nRecIndex);
	}
	else
	{
		DBG_MSG("nRecIndex2 %d\n", nRecIndex);
		goto getRecResut_lc;
	}
	_snprintf_s(temp, sizeof(temp)-1, "temp%d.jpg", nRecIndex);
	DBG_MSG("rec_grap 9999999 %d\n", nRecIndex);

getRecResut_lc:
	if (nRecTempIndex > nEnd && nMaxSmallCellIndex > 0)
	{
		nRecIndex = nMaxSmallCellIndex;
		nMaxSmallCellIndex = 0;
	}

	// 获取AI识别结果
	GetAiRecResult(pFocusPath, nRecIndex, Cells, nRt);

	int nSmallCellCount = Cells.HCount + Cells.BCount + Cells.YHCount + Cells.MJCount + Cells.XBTCount + Cells.NSCount + Cells.LSCount + Cells.CSCount + Cells.KLCount + Cells.TMCount;

	if (nSmallCellCount < nMaxCutCount) //小细胞未达到设置个数
	{
		if (nSmallCellCount == 0 && Cells.SPCount >= nMinEpCount) //小细胞为0，上皮达到设置个数，以上皮找清晰层
		{
			nCutIndex = 0;
			goto Sp_lc;
		}

		if (nMaxSmallCellCount < nSmallCellCount) //取小细胞较多的层
		{
			nMaxSmallCellCount = nSmallCellCount;
			nMaxSmallCellIndex = nRecIndex;
			memcpy(&cacheCells, &Cells, sizeof(Cells));
			bFindSmallMaxCountLayr = TRUE;
			DBG_MSG("取小细胞较多的层 %d,%d\n", nRecIndex, nMaxSmallCellCount);
		}

		if (nRecTempIndex <= nEnd) //小细胞未达到设置个数,逐层识别
		{
			nRecTempIndex++;

			DBG_MSG("nRecTempIndex %d\n", nRecTempIndex);
			goto Rec_lc;
		}
	}
	else
	{
		bFindSmallMaxCountLayr = TRUE;
		memcpy(&cacheCells, &Cells, sizeof(Cells));
	}

	if (!bFindSmallMaxCountLayr)
	{
		return nCleanLayer;
	}

	if (cacheCells.HCount > 0)
	{
		DBG_MSG("H......\n");

		if (cacheCells.HCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.HCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.H[k].left;
			CutRect[nCutIndex].right = cacheCells.H[k].right;
			CutRect[nCutIndex].top = cacheCells.H[k].top;
			CutRect[nCutIndex].bottom = cacheCells.H[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.YHCount > 0)
	{
		DBG_MSG("YH......\n");

		if (cacheCells.YHCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.YHCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.YH[k].left;
			CutRect[nCutIndex].right = cacheCells.YH[k].right;
			CutRect[nCutIndex].top = cacheCells.YH[k].top;
			CutRect[nCutIndex].bottom = cacheCells.YH[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.BCount > 0)
	{
		DBG_MSG("B......\n");

		if (cacheCells.BCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.BCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.B[k].left;
			CutRect[nCutIndex].right = cacheCells.B[k].right;
			CutRect[nCutIndex].top = cacheCells.B[k].top;
			CutRect[nCutIndex].bottom = cacheCells.B[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.XBTCount > 0)
	{
		DBG_MSG("XBT......\n");

		if (cacheCells.XBTCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.XBTCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.XBT[k].left;
			CutRect[nCutIndex].right = cacheCells.XBT[k].right;
			CutRect[nCutIndex].top = cacheCells.XBT[k].top;
			CutRect[nCutIndex].bottom = cacheCells.XBT[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.NSCount > 0)
	{
		DBG_MSG("NS......\n");

		if (cacheCells.NSCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.NSCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.NS[k].left;
			CutRect[nCutIndex].right = cacheCells.NS[k].right;
			CutRect[nCutIndex].top = cacheCells.NS[k].top;
			CutRect[nCutIndex].bottom = cacheCells.NS[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.LSCount > 0)
	{
		DBG_MSG("LS......\n");

		if (cacheCells.LSCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.LSCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.LS[k].left;
			CutRect[nCutIndex].right = cacheCells.LS[k].right;
			CutRect[nCutIndex].top = cacheCells.LS[k].top;
			CutRect[nCutIndex].bottom = cacheCells.LS[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.CSCount > 0)
	{
		DBG_MSG("CS......\n");

		if (cacheCells.CSCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.CSCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.CS[k].left;
			CutRect[nCutIndex].right = cacheCells.CS[k].right;
			CutRect[nCutIndex].top = cacheCells.CS[k].top;
			CutRect[nCutIndex].bottom = cacheCells.CS[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.MJCount > 0)
	{
		DBG_MSG("DC......\n");
		if (cacheCells.MJCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.MJCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.MJ[k].left;
			CutRect[nCutIndex].right = cacheCells.MJ[k].right;
			CutRect[nCutIndex].top = cacheCells.MJ[k].top;
			CutRect[nCutIndex].bottom = cacheCells.MJ[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.KLCount > 0)
	{
		DBG_MSG("KL......\n");
		if (cacheCells.KLCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.KLCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.KL[k].left;
			CutRect[nCutIndex].right = cacheCells.KL[k].right;
			CutRect[nCutIndex].top = cacheCells.KL[k].top;
			CutRect[nCutIndex].bottom = cacheCells.KL[k].bottom;
			nCutIndex++;
		}
	}

	if (cacheCells.TMCount > 0)
	{
		DBG_MSG("TM......\n");
		if (cacheCells.TMCount + nCutIndex >= nMaxCutCount)
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = cacheCells.TMCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[nCutIndex].left = cacheCells.TM[k].left;
			CutRect[nCutIndex].right = cacheCells.TM[k].right;
			CutRect[nCutIndex].top = cacheCells.TM[k].top;
			CutRect[nCutIndex].bottom = cacheCells.TM[k].bottom;
			nCutIndex++;
		}
	}

	nCutCount = nCutIndex;

	if (nCutCount > 0)
	{
		goto Cut_lc;
	}
	else
	{
		return nCleanLayer;
	}

Sp_lc:
	if (Cells.SPCount > 0)
	{
		DBG_MSG("SP......\n");

		bFocusEp = TRUE;

		DBG_MSG("Find SP......\n");

		if (Cells.SPCount >= nMaxCutCount)
			nCutCount = nMaxCutCount;
		else
			nCutCount = Cells.SPCount;

		for (int k = 0; k < nCutCount; k++)
		{
			CutRect[k].left = Cells.SP[k].left;
			CutRect[k].right = Cells.SP[k].right;
			CutRect[k].top = Cells.SP[k].top;
			CutRect[k].bottom = Cells.SP[k].bottom;
		}
	}
	else
	{
		return nCleanLayer;
	}

Cut_lc:
	std::vector<LAYER_COUNT> vecLayerCount;
	LAYER_COUNT LayerCount = { 0 };
	int nLayerCount = 0;

	nMaxIndex = nCleanLayer;

	for (int i = 0; i < nCutCount; i++)
	{
		// 切图获取清晰层
		nTempMaxIndex[i] = (nFocusWay == 3)? GetCleanLayer(vecFi, pFocusPath, IniPath, CutRect[i], nBegin, nEnd, nCleanLayer, i, bFocusEp)
			: GetCleanLayer(pFocusPath, IniPath, CutRect[i], nBegin, nEnd, nCleanLayer, i, bFocusEp);

		if (nTempMaxIndex[i] == -1)
			continue;

		//取细胞清晰个数多的层
		LayerCount.nCount = 1;
		LayerCount.nLayer = nTempMaxIndex[i];

		if (i == 0)
		{
			vecLayerCount.push_back(LayerCount);
		}
		else
		{
			BOOL bFind = FALSE;
			for (int k = 0; k < vecLayerCount.size(); k++)
			{
				if (vecLayerCount[k].nLayer == LayerCount.nLayer)
				{
					vecLayerCount[k].nCount++;
					bFind = TRUE;
					break;
				}
			}
			if (bFind == FALSE)
				vecLayerCount.push_back(LayerCount);
		}

	}
	//find max count layer
	// 相同数量的选择距离初始清晰层更近的层作为清晰层
	char szKey[256], szValue[256];
	if (nFocusWay == 3)
	{
		if (vecLayerCount.size() == 0)
		{
			nMaxIndex = nCleanLayer;
		}
		else
		{
			vector<int> vecResult;
			int maxCount = std::max_element(vecLayerCount.cbegin(), vecLayerCount.cend(), [](const LAYER_COUNT &x, const LAYER_COUNT &y) {return x.nCount < y.nCount; })->nCount;
			vector<int> vecMaxLayer;
			for (auto itr = vecLayerCount.cbegin(); itr != vecLayerCount.cend(); itr++)
			{
				if (itr->nCount == maxCount) vecMaxLayer.push_back(itr->nLayer);
				_snprintf_s(szKey, sizeof(szKey) - 1, "----Focus Result[%d]----", itr->nLayer);
				_snprintf_s(szValue, sizeof(szValue) - 1, "layer:%d  count:%d", itr->nLayer, itr->nCount);
				WritePrivateProfileString("focus", szKey, szValue, IniPath);
			}
			if (vecMaxLayer.size() == 1)
			{
				_snprintf_s(szKey, sizeof(szKey) - 1, "----result focus layer----");
				_snprintf_s(szValue, sizeof(szValue) - 1, "%d", vecMaxLayer[0]);
				WritePrivateProfileString("focus", szKey, szValue, IniPath);
			}
			else
			{
				int midLayer = vecFi.size() / 2;
				int L = *vecMaxLayer.rbegin();
				int distance = vecFi.size();
				for (int i = 0; i < vecMaxLayer.size(); i++)
				{
					int D = abs(vecMaxLayer[i] - midLayer);
					if (D < distance)
					{
						distance = D;
						L = vecMaxLayer[i];
					}
				}
				_snprintf_s(szKey, sizeof(szKey) - 1, "----result focus layer----");
				_snprintf_s(szValue, sizeof(szValue) - 1, "%d", L);
				WritePrivateProfileString("focus", szKey, szValue, IniPath);
				nMaxIndex = L;
			}
		}
	}
	else
	{
		set<int> maxCountLayers;
		for (int k = 0; k < vecLayerCount.size(); k++)
		{
			if (nLayerCount < vecLayerCount[k].nCount)
			{
				nLayerCount = vecLayerCount[k].nCount;
			}
		}
		for (int k = 0; k < vecLayerCount.size(); k++)
		{
			if (nLayerCount == vecLayerCount[k].nCount)
			{
				maxCountLayers.insert(vecLayerCount[k].nLayer);
			}
		}
		int maxDistance = 10000;
		for (auto itr = maxCountLayers.begin(); itr != maxCountLayers.end(); itr++)
		{
			int distance = abs(*itr - nCleanLayer);
			if (distance <= maxDistance)
			{
				maxDistance = distance;
				nMaxIndex = *itr;
			}
		}
	}

	_snprintf_s(temp, sizeof(temp)-1, "%d", nMaxIndex);
	WritePrivateProfileString("focus", "nMaxIndex", temp, IniPath);

	//nMaxIndex = IdentifyNew(pFocusPath,nBegin,nEnd,nMaxIndex);

	_snprintf_s(temp, sizeof(temp) - 1, "%d", nMaxIndex);
	WritePrivateProfileString("sys", "Clearlayer", temp, m_FocusIniPath);

	if (!bFocusEp) bFocusBySmallCell = TRUE;

	return nMaxIndex;
}

void CUSArc::GetAiRecResult(PCHAR path,int nCleanLayer,AllCellInfo1 & CellImg,int & ret)
{
	char SessionName[MAX_PATH] = {0},temp[MAX_PATH] = {0}, str[MAX_PATH] = {0},PicPath[MAX_PATH] = {0};

	int nBIndex = 0,nSpIndex = 0, nGJIndex = 0,nMJIndex = 0,nYSBZIndex = 0,nXSIndex = 0,nJSIndex = 0,nDCIndex = 0,nHIndex = 0;

	strcpy_s(PicPath,path);
	sprintf_s(SessionName,"%u", nCleanLayer);

	PathAppend(PicPath, "Result.ini");

	DBG_MSG("AI picPath  %s\n",PicPath);
	int ncount = GetPrivateProfileInt(SessionName,"Count",0,PicPath);

	ret = GetPrivateProfileInt("focus","rec",0,PicPath);

	int left = 0, right = 0, top = 0,bottom = 0, nClass = 0,score = 0;

	DBG_MSG("RecConut %d\n",ncount);

	ZeroMemory(&CellImg,sizeof(AllCellInfo1));

	for( int i = 0 ; i < ncount; i++ )
	{
		_snprintf_s(str, sizeof(str)-1, "%u", i);
		GetPrivateProfileString(SessionName,str,"",temp,MAX_PATH,PicPath);
		sscanf_s(temp,"%d,%d,%d,%d,%d,%d",
			&nClass, 
			&left, 
			&top,
			&right,
			&bottom,
			&score);

		if( nClass == CELL_WHITE_TYPE)
		{
			CellImg.BCount++;
			CellImg.B[nBIndex].left = left;
			CellImg.B[nBIndex].right = right;
			CellImg.B[nBIndex].top = top;
			CellImg.B[nBIndex].bottom = bottom;
			CellImg.B[nBIndex].score = score;
			nBIndex++;
		}
		else if( nClass == CELL_EP_TYPE)
		{
			CellImg.SPCount++;
			CellImg.SP[nSpIndex].left = left;
			CellImg.SP[nSpIndex].right = right;
			CellImg.SP[nSpIndex].top = top;
			CellImg.SP[nSpIndex].bottom = bottom;
			CellImg.SP[nSpIndex].score = score;
			nSpIndex++;
		}
		else if( nClass == CELL_RED_TYPE )
		{
			CellImg.HCount++;
			CellImg.H[nHIndex].left = left;
			CellImg.H[nHIndex].right = right;
			CellImg.H[nHIndex].top = top;
			CellImg.H[nHIndex].bottom = bottom;
			CellImg.H[nHIndex].score = score;
			nHIndex++;
		}
		else if( nClass == CELL_CAOX_TYPE)
		{
			CellImg.CSCount++;
			CellImg.CS[nGJIndex].left = left;
			CellImg.CS[nGJIndex].right = right;
			CellImg.CS[nGJIndex].top = top;
			CellImg.CS[nGJIndex].bottom = bottom;
			CellImg.CS[nGJIndex].score = score;
			nGJIndex++;
		}
		else if( nClass == CELL_MJ_TYPE)
		{
			CellImg.MJCount++;
			CellImg.MJ[nMJIndex].left = left;
			CellImg.MJ[nMJIndex].right = right;
			CellImg.MJ[nMJIndex].top = top;
			CellImg.MJ[nMJIndex].bottom = bottom;
			CellImg.MJ[nMJIndex].score = score;
			nMJIndex++;
		}
		else if( nClass == CELL_CSJ_TYPE)
		{
			CellImg.CSJCount++;
			CellImg.CSJ[nYSBZIndex].left = left;
			CellImg.CSJ[nYSBZIndex].right = right;
			CellImg.CSJ[nYSBZIndex].top = top;
			CellImg.CSJ[nYSBZIndex].bottom = bottom;
			CellImg.CSJ[nYSBZIndex].score = score;
			nYSBZIndex++;
		}
		else if( nClass == CELL_XY_TYPE )
		{
			CellImg.XYCount++;
			CellImg.XY[nXSIndex].left = left;
			CellImg.XY[nXSIndex].right = right;
			CellImg.XY[nXSIndex].top = top;
			CellImg.XY[nXSIndex].bottom = bottom;
			CellImg.XY[nXSIndex].score = score;
			nXSIndex++;
		}
		else if( nClass == CELL_WBCC_TYPE )
		{
			CellImg.XBTCount++;
			CellImg.XBT[nJSIndex].left = left;
			CellImg.XBT[nJSIndex].right = right;
			CellImg.XBT[nJSIndex].top = top;
			CellImg.XBT[nJSIndex].bottom = bottom;
			CellImg.XBT[nJSIndex].score = score;
			nJSIndex++;
		}
		else if( nClass == CELL_DRED_TYPE )
		{
			CellImg.YHCount++;
			CellImg.YH[nDCIndex].left = left;
			CellImg.YH[nDCIndex].right = right;
			CellImg.YH[nDCIndex].top = top;
			CellImg.YH[nDCIndex].bottom = bottom;
			CellImg.YH[nDCIndex].score = score;
			nDCIndex++;
		}
	}
}

void CUSArc::GetAiRecResult(PCHAR path, int nCleanLayer, CellsIn20x & CellImg, int & ret)
{
	char SessionName[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 }, str[MAX_PATH] = { 0 }, PicPath[MAX_PATH] = { 0 };

	int nXBTIndex = 0, nKLIndex = 0, nTSIndex = 0, nTMIndex = 0, nXYIndex = 0;
	int nNYIndex = 0, nNSIndex = 0, nSPIndex = 0, nCSJIndex = 0, nLSIndex = 0;
	int nMJIndex = 0, nCSIndex = 0, nHIndex = 0, nBIndex = 0, nYHIndex = 0;

	strcpy_s(PicPath, path);
	sprintf_s(SessionName, "%d", nCleanLayer);

	PathAppend(PicPath, "Result.ini");

	DBG_MSG("AI iniPath  %s\n", PicPath);
	int ncount = GetPrivateProfileInt(SessionName, "Count", 0, PicPath);

	ret = GetPrivateProfileInt("focus", "rec", 0, PicPath);

	int left = 0, right = 0, top = 0, bottom = 0, nClass = 0, score = 0;

	DBG_MSG("RecConut %d\n", ncount);

	ZeroMemory(&CellImg, sizeof(CellsIn20x));

	for (int i = 0; i < ncount; i++)
	{
		_snprintf_s(str, sizeof(str)-1, "%u", i);
		GetPrivateProfileString(SessionName, str, "", temp, MAX_PATH, PicPath);
		sscanf_s(temp, "%d,%d,%d,%d,%d,%d",
			&nClass,
			&left,
			&top,
			&right,
			&bottom,
			&score);

		if (nClass == CELL_WBCC_TYPE)
		{
			CellImg.XBTCount++;
			CellImg.XBT[nXBTIndex].left = left;
			CellImg.XBT[nXBTIndex].right = right;
			CellImg.XBT[nXBTIndex].top = top;
			CellImg.XBT[nXBTIndex].bottom = bottom;
			CellImg.XBT[nXBTIndex].score = score;
			nXBTIndex++;
		}
		else if (nClass == CELL_GRAN_TYPE)
		{
			CellImg.KLCount++;
			CellImg.KL[nKLIndex].left = left;
			CellImg.KL[nKLIndex].right = right;
			CellImg.KL[nKLIndex].top = top;
			CellImg.KL[nKLIndex].bottom = bottom;
			CellImg.KL[nKLIndex].score = score;
			nKLIndex++;
		}
		else if (nClass == CELL_PHAGOCYTES_TYPE)
		{
			CellImg.TSCount++;
			CellImg.TS[nTSIndex].left = left;
			CellImg.TS[nTSIndex].right = right;
			CellImg.TS[nTSIndex].top = top;
			CellImg.TS[nTSIndex].bottom = bottom;
			CellImg.TS[nTSIndex].score = score;
			nTSIndex++;
		}
		else if (nClass == CELL_HYAL_TYPE)
		{
			CellImg.TMCount++;
			CellImg.TM[nTMIndex].left = left;
			CellImg.TM[nTMIndex].right = right;
			CellImg.TM[nTMIndex].top = top;
			CellImg.TM[nTMIndex].bottom = bottom;
			CellImg.TM[nTMIndex].score = score;
			nTMIndex++;
		}
		else if (nClass == CELL_SREP_TYPE)
		{
			CellImg.XYCount++;
			CellImg.XY[nXYIndex].left = left;
			CellImg.XY[nXYIndex].right = right;
			CellImg.XY[nXYIndex].top = top;
			CellImg.XY[nXYIndex].bottom = bottom;
			CellImg.XY[nXYIndex].score = score;
			nXYIndex++;
		}
		else if (nClass == CELL_MUCS_TYPE)
		{
			CellImg.NYCount++;
			CellImg.NY[nNYIndex].left = left;
			CellImg.NY[nNYIndex].right = right;
			CellImg.NY[nNYIndex].top = top;
			CellImg.NY[nNYIndex].bottom = bottom;
			CellImg.NY[nNYIndex].score = score;
			nNYIndex++;
		}
		else if (nClass == CELL_URIC_TYPE)
		{
			CellImg.NSCount++;
			CellImg.NS[nNSIndex].left = left;
			CellImg.NS[nNSIndex].right = right;
			CellImg.NS[nNSIndex].top = top;
			CellImg.NS[nNSIndex].bottom = bottom;
			CellImg.NS[nNSIndex].score = score;
			nNSIndex++;
		}
		else if (nClass == CELL_SQEP_TYPE)
		{
			CellImg.SPCount++;
			CellImg.SP[nSPIndex].left = left;
			CellImg.SP[nSPIndex].right = right;
			CellImg.SP[nSPIndex].top = top;
			CellImg.SP[nSPIndex].bottom = bottom;
			CellImg.SP[nSPIndex].score = score;
			nSPIndex++;
		}
		else if (nClass == CELL_RED_CAOX_TYPE)
		{
			CellImg.CSJCount++;
			CellImg.CSJ[nCSJIndex].left = left;
			CellImg.CSJ[nCSJIndex].right = right;
			CellImg.CSJ[nCSJIndex].top = top;
			CellImg.CSJ[nCSJIndex].bottom = bottom;
			CellImg.CSJ[nCSJIndex].score = score;
			nCSJIndex++;
		}
		else if (nClass == CELL_CAPH_TYPE)
		{
			CellImg.LSCount++;
			CellImg.LS[nLSIndex].left = left;
			CellImg.LS[nLSIndex].right = right;
			CellImg.LS[nLSIndex].top = top;
			CellImg.LS[nLSIndex].bottom = bottom;
			CellImg.LS[nLSIndex].score = score;
			nLSIndex++;
		}
		else if (nClass == CELL_BACT_TYPE)
		{
			CellImg.MJCount++;
			CellImg.MJ[nMJIndex].left = left;
			CellImg.MJ[nMJIndex].right = right;
			CellImg.MJ[nMJIndex].top = top;
			CellImg.MJ[nMJIndex].bottom = bottom;
			CellImg.MJ[nMJIndex].score = score;
			nMJIndex++;
		}
		else if (nClass == CELL_CAOX_TYPE)
		{
			CellImg.CSCount++;
			CellImg.CS[nCSIndex].left = left;
			CellImg.CS[nCSIndex].right = right;
			CellImg.CS[nCSIndex].top = top;
			CellImg.CS[nCSIndex].bottom = bottom;
			CellImg.CS[nCSIndex].score = score;
			nCSIndex++;
		}
		else if (nClass == CELL_RED_TYPE)
		{
			CellImg.HCount++;
			CellImg.H[nHIndex].left = left;
			CellImg.H[nHIndex].right = right;
			CellImg.H[nHIndex].top = top;
			CellImg.H[nHIndex].bottom = bottom;
			CellImg.H[nHIndex].score = score;
			nHIndex++;
		}
		else if (nClass == CELL_WHITE_TYPE)
		{
			CellImg.BCount++;
			CellImg.B[nBIndex].left = left;
			CellImg.B[nBIndex].right = right;
			CellImg.B[nBIndex].top = top;
			CellImg.B[nBIndex].bottom = bottom;
			CellImg.B[nBIndex].score = score;
			nBIndex++;
		}
		else if (nClass == CELL_DRED_TYPE) //异型红细胞
		{
			CellImg.YHCount++;
			CellImg.YH[nYHIndex].left = left;
			CellImg.YH[nYHIndex].right = right;
			CellImg.YH[nYHIndex].top = top;
			CellImg.YH[nYHIndex].bottom = bottom;
			CellImg.YH[nYHIndex].score = score;
			nYHIndex++;
		}
	}
}

void CUSArc::GetAiRecResult(PCHAR path, int nCleanLayer, AllCellsInfo & Cells, int & ret)
{
	char SessionName[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 }, str[MAX_PATH] = { 0 }, PicPath[MAX_PATH] = { 0 };

	int nXBTIndex = 0, nKLIndex = 0, nTSIndex = 0, nTMIndex = 0, nXYIndex = 0;
	int nNYIndex = 0, nNSIndex = 0, nSPIndex = 0, nCSJIndex = 0, nLSIndex = 0;
	int nMJIndex = 0, nCSIndex = 0, nHIndex = 0, nBIndex = 0, nYHIndex = 0;
	int nJZIndex = 0, nGJIndex = 0, nQIndex = 0;

	strcpy_s(PicPath, path);
	_snprintf_s(SessionName, sizeof(SessionName)-1, "%d", nCleanLayer);

	PathAppend(PicPath, RESULT_FILE);

	DBG_MSG("AI iniPath  %s\n", PicPath);
	int ncount = GetPrivateProfileInt(SessionName, "Count", 0, PicPath);

	ret = GetPrivateProfileInt("focus", "rec", 0, PicPath);

	int left = 0, right = 0, top = 0, bottom = 0, nClass = 0, score = 0;

	DBG_MSG("RecConut %d\n", ncount);

	ZeroMemory(&Cells, sizeof(AllCellsInfo));

	for (int i = 0; i < ncount; i++)
	{
		_snprintf_s(str, sizeof(str) - 1, "%u", i);
		GetPrivateProfileString(SessionName, str, "", temp, MAX_PATH, PicPath);
		sscanf_s(temp, "%d,%d,%d,%d,%d,%d",
			&nClass,
			&left,
			&top,
			&right,
			&bottom,
			&score);

		if (nClass == CELL_QC_TYPE)
		{
			Cells.QCount++;
			Cells.Q[nQIndex].left = left;
			Cells.Q[nQIndex].right = right;
			Cells.Q[nQIndex].top = top;
			Cells.Q[nQIndex].bottom = bottom;
			Cells.Q[nQIndex].score = score;
			nQIndex++;
		}
		else if (nClass == CELL_EP_TYPE)
		{
			Cells.SPCount++;
			Cells.SP[nSPIndex].left = left;
			Cells.SP[nSPIndex].right = right;
			Cells.SP[nSPIndex].top = top;
			Cells.SP[nSPIndex].bottom = bottom;
			Cells.SP[nSPIndex].score = score;
			nSPIndex++;
		}
		else if (nClass == CELL_PHAGOCYTES_TYPE)
		{
			Cells.TSCount++;
			Cells.TS[nTSIndex].left = left;
			Cells.TS[nTSIndex].right = right;
			Cells.TS[nTSIndex].top = top;
			Cells.TS[nTSIndex].bottom = bottom;
			Cells.TS[nTSIndex].score = score;
			nTSIndex++;
		}
		else if (nClass == CELL_WBCC_TYPE)
		{
			Cells.XBTCount++;
			Cells.XBT[nXBTIndex].left = left;
			Cells.XBT[nXBTIndex].right = right;
			Cells.XBT[nXBTIndex].top = top;
			Cells.XBT[nXBTIndex].bottom = bottom;
			Cells.XBT[nXBTIndex].score = score;
			nXBTIndex++;
		}
		else if (nClass == CELL_XY_TYPE)
		{
			Cells.XYCount++;
			Cells.XY[nXYIndex].left = left;
			Cells.XY[nXYIndex].right = right;
			Cells.XY[nXYIndex].top = top;
			Cells.XY[nXYIndex].bottom = bottom;
			Cells.XY[nXYIndex].score = score;
			nXYIndex++;
		}
		else if (nClass == CELL_BACT_TYPE)
		{
			Cells.MJCount++;
			Cells.MJ[nMJIndex].left = left;
			Cells.MJ[nMJIndex].right = right;
			Cells.MJ[nMJIndex].top = top;
			Cells.MJ[nMJIndex].bottom = bottom;
			Cells.MJ[nMJIndex].score = score;
			nMJIndex++;
		}
		else if (nClass == CELL_CAOX_TYPE)
		{
			Cells.CSCount++;
			Cells.CS[nCSIndex].left = left;
			Cells.CS[nCSIndex].right = right;
			Cells.CS[nCSIndex].top = top;
			Cells.CS[nCSIndex].bottom = bottom;
			Cells.CS[nCSIndex].score = score;
			nCSIndex++;
		}
		else if (nClass == CELL_RED_CAOX_TYPE)
		{
			Cells.CSJCount++;
			Cells.CSJ[nCSJIndex].left = left;
			Cells.CSJ[nCSJIndex].right = right;
			Cells.CSJ[nCSJIndex].top = top;
			Cells.CSJ[nCSJIndex].bottom = bottom;
			Cells.CSJ[nCSJIndex].score = score;
			nCSJIndex++;
		}
		else if (nClass == CELL_URIC_TYPE)
		{
			Cells.NSCount++;
			Cells.NS[nNSIndex].left = left;
			Cells.NS[nNSIndex].right = right;
			Cells.NS[nNSIndex].top = top;
			Cells.NS[nNSIndex].bottom = bottom;
			Cells.NS[nNSIndex].score = score;
			nNSIndex++;
		}
		else if (nClass == CELL_LS_TYPE)
		{
			Cells.LSCount++;
			Cells.LS[nLSIndex].left = left;
			Cells.LS[nLSIndex].right = right;
			Cells.LS[nLSIndex].top = top;
			Cells.LS[nLSIndex].bottom = bottom;
			Cells.LS[nLSIndex].score = score;
			nLSIndex++;
		}
		else if (nClass == CELL_GRAN_TYPE)
		{
			Cells.KLCount++;
			Cells.KL[nKLIndex].left = left;
			Cells.KL[nKLIndex].right = right;
			Cells.KL[nKLIndex].top = top;
			Cells.KL[nKLIndex].bottom = bottom;
			Cells.KL[nKLIndex].score = score;
			nKLIndex++;
		}
		else if (nClass == CELL_HYAL_TYPE)
		{
			Cells.TMCount++;
			Cells.TM[nTMIndex].left = left;
			Cells.TM[nTMIndex].right = right;
			Cells.TM[nTMIndex].top = top;
			Cells.TM[nTMIndex].bottom = bottom;
			Cells.TM[nTMIndex].score = score;
			nTMIndex++;
		}
		else if (nClass == CELL_MUCS_TYPE)
		{
			Cells.NYCount++;
			Cells.NY[nNYIndex].left = left;
			Cells.NY[nNYIndex].right = right;
			Cells.NY[nNYIndex].top = top;
			Cells.NY[nNYIndex].bottom = bottom;
			Cells.NY[nNYIndex].score = score;
			nNYIndex++;
		}
		else if (nClass == CELL_BACILLUS_TYPE)
		{
			Cells.GJCount++;
			Cells.GJ[nGJIndex].left = left;
			Cells.GJ[nGJIndex].right = right;
			Cells.GJ[nGJIndex].top = top;
			Cells.GJ[nGJIndex].bottom = bottom;
			Cells.GJ[nGJIndex].score = score;
			nGJIndex++;
		}
		else if (nClass == CELL_SPRM_TYPE)
		{
			Cells.JZCount++;
			Cells.JZ[nJZIndex].left = left;
			Cells.JZ[nJZIndex].right = right;
			Cells.JZ[nJZIndex].top = top;
			Cells.JZ[nJZIndex].bottom = bottom;
			Cells.JZ[nJZIndex].score = score;
			nJZIndex++;
		}
		else if (nClass == CELL_RED_TYPE)
		{
			Cells.HCount++;
			Cells.H[nHIndex].left = left;
			Cells.H[nHIndex].right = right;
			Cells.H[nHIndex].top = top;
			Cells.H[nHIndex].bottom = bottom;
			Cells.H[nHIndex].score = score;
			nHIndex++;
		}
		else if (nClass == CELL_WHITE_TYPE)
		{
			Cells.BCount++;
			Cells.B[nBIndex].left = left;
			Cells.B[nBIndex].right = right;
			Cells.B[nBIndex].top = top;
			Cells.B[nBIndex].bottom = bottom;
			Cells.B[nBIndex].score = score;
			nBIndex++;
		}
		else if (nClass == CELL_DRED_TYPE) //异型红细胞
		{
			Cells.YHCount++;
			Cells.YH[nYHIndex].left = left;
			Cells.YH[nYHIndex].right = right;
			Cells.YH[nYHIndex].top = top;
			Cells.YH[nYHIndex].bottom = bottom;
			Cells.YH[nYHIndex].score = score;
			nYHIndex++;
		}
	}
}

bool CUSArc::GetAiRecResult(PCHAR szPicPath, int nPicCount, AllCellsInfo & Cells)
{
	char SessionName[64] = { 0 }, temp[MAX_PATH] = { 0 }, str[MAX_PATH] = { 0 }, PicPath[MAX_PATH] = { 0 };

	int nXBTIndex = 0, nKLIndex = 0, nTSIndex = 0, nTMIndex = 0, nXYIndex = 0;
	int nNYIndex = 0, nNSIndex = 0, nSPIndex = 0, nCSJIndex = 0, nLSIndex = 0;
	int nMJIndex = 0, nCSIndex = 0, nHIndex = 0, nBIndex = 0, nYHIndex = 0;
	int nJZIndex = 0, nGJIndex = 0, nQIndex = 0;

	strcpy_s(PicPath, szPicPath);
	PathAppend(PicPath, RESULT_FILE);
	DBG_MSG("AI iniPath  %s\n", PicPath);

	ZeroMemory(&Cells, sizeof(AllCellsInfo));
	for (INT i = 0; i < nPicCount; i++)
	{
		_snprintf_s(SessionName, sizeof(SessionName) - 1, "%d", (i+1));
		int ncount = GetPrivateProfileInt(SessionName, "Count", 0, PicPath);

		int left = 0, right = 0, top = 0, bottom = 0, nClass = 0, score = 0;
		for (int i = 0; i < ncount; i++)
		{
			_snprintf_s(str, sizeof(str) - 1, "%u", i);
			GetPrivateProfileString(SessionName, str, "", temp, MAX_PATH, PicPath);
			sscanf_s(temp, "%d,%d,%d,%d,%d,%d",
				&nClass,
				&left,
				&top,
				&right,
				&bottom,
				&score);

			if (nClass == CELL_QC_TYPE)
			{
				Cells.SPCount++;
				Cells.Q[nQIndex].left = left;
				Cells.Q[nQIndex].right = right;
				Cells.Q[nQIndex].top = top;
				Cells.Q[nQIndex].bottom = bottom;
				Cells.Q[nQIndex].score = score;
				nQIndex++;
			}
			else if (nClass == CELL_EP_TYPE)
			{
				Cells.SPCount++;
				Cells.SP[nSPIndex].left = left;
				Cells.SP[nSPIndex].right = right;
				Cells.SP[nSPIndex].top = top;
				Cells.SP[nSPIndex].bottom = bottom;
				Cells.SP[nSPIndex].score = score;
				nSPIndex++;
			}
			else if (nClass == CELL_PHAGOCYTES_TYPE)
			{
				Cells.TSCount++;
				Cells.TS[nTSIndex].left = left;
				Cells.TS[nTSIndex].right = right;
				Cells.TS[nTSIndex].top = top;
				Cells.TS[nTSIndex].bottom = bottom;
				Cells.TS[nTSIndex].score = score;
				nTSIndex++;
			}
			else if (nClass == CELL_WBCC_TYPE)
			{
				Cells.XBTCount++;
				Cells.XBT[nXBTIndex].left = left;
				Cells.XBT[nXBTIndex].right = right;
				Cells.XBT[nXBTIndex].top = top;
				Cells.XBT[nXBTIndex].bottom = bottom;
				Cells.XBT[nXBTIndex].score = score;
				nXBTIndex++;
			}
			else if (nClass == CELL_XY_TYPE)
			{
				Cells.XYCount++;
				Cells.XY[nXYIndex].left = left;
				Cells.XY[nXYIndex].right = right;
				Cells.XY[nXYIndex].top = top;
				Cells.XY[nXYIndex].bottom = bottom;
				Cells.XY[nXYIndex].score = score;
				nXYIndex++;
			}
			else if (nClass == CELL_BACT_TYPE)
			{
				Cells.MJCount++;
				Cells.MJ[nMJIndex].left = left;
				Cells.MJ[nMJIndex].right = right;
				Cells.MJ[nMJIndex].top = top;
				Cells.MJ[nMJIndex].bottom = bottom;
				Cells.MJ[nMJIndex].score = score;
				nMJIndex++;
			}
			else if (nClass == CELL_CAOX_TYPE)
			{
				Cells.CSCount++;
				Cells.CS[nCSIndex].left = left;
				Cells.CS[nCSIndex].right = right;
				Cells.CS[nCSIndex].top = top;
				Cells.CS[nCSIndex].bottom = bottom;
				Cells.CS[nCSIndex].score = score;
				nCSIndex++;
			}
			else if (nClass == CELL_RED_CAOX_TYPE)
			{
				Cells.CSJCount++;
				Cells.CSJ[nCSJIndex].left = left;
				Cells.CSJ[nCSJIndex].right = right;
				Cells.CSJ[nCSJIndex].top = top;
				Cells.CSJ[nCSJIndex].bottom = bottom;
				Cells.CSJ[nCSJIndex].score = score;
				nCSJIndex++;
			}
			else if (nClass == CELL_URIC_TYPE)
			{
				Cells.NSCount++;
				Cells.NS[nNSIndex].left = left;
				Cells.NS[nNSIndex].right = right;
				Cells.NS[nNSIndex].top = top;
				Cells.NS[nNSIndex].bottom = bottom;
				Cells.NS[nNSIndex].score = score;
				nNSIndex++;
			}
			else if (nClass == CELL_LS_TYPE)
			{
				Cells.LSCount++;
				Cells.LS[nLSIndex].left = left;
				Cells.LS[nLSIndex].right = right;
				Cells.LS[nLSIndex].top = top;
				Cells.LS[nLSIndex].bottom = bottom;
				Cells.LS[nLSIndex].score = score;
				nLSIndex++;
			}
			else if (nClass == CELL_GRAN_TYPE)
			{
				Cells.KLCount++;
				Cells.KL[nKLIndex].left = left;
				Cells.KL[nKLIndex].right = right;
				Cells.KL[nKLIndex].top = top;
				Cells.KL[nKLIndex].bottom = bottom;
				Cells.KL[nKLIndex].score = score;
				nKLIndex++;
			}
			else if (nClass == CELL_HYAL_TYPE)
			{
				Cells.TMCount++;
				Cells.TM[nTMIndex].left = left;
				Cells.TM[nTMIndex].right = right;
				Cells.TM[nTMIndex].top = top;
				Cells.TM[nTMIndex].bottom = bottom;
				Cells.TM[nTMIndex].score = score;
				nTMIndex++;
			}
			else if (nClass == CELL_MUCS_TYPE)
			{
				Cells.NYCount++;
				Cells.NY[nNYIndex].left = left;
				Cells.NY[nNYIndex].right = right;
				Cells.NY[nNYIndex].top = top;
				Cells.NY[nNYIndex].bottom = bottom;
				Cells.NY[nNYIndex].score = score;
				nNYIndex++;
			}
			else if (nClass == CELL_BACILLUS_TYPE)
			{
				Cells.GJCount++;
				Cells.GJ[nGJIndex].left = left;
				Cells.GJ[nGJIndex].right = right;
				Cells.GJ[nGJIndex].top = top;
				Cells.GJ[nGJIndex].bottom = bottom;
				Cells.GJ[nGJIndex].score = score;
				nGJIndex++;
			}
			else if (nClass == CELL_SPRM_TYPE)
			{
				Cells.JZCount++;
				Cells.JZ[nJZIndex].left = left;
				Cells.JZ[nJZIndex].right = right;
				Cells.JZ[nJZIndex].top = top;
				Cells.JZ[nJZIndex].bottom = bottom;
				Cells.JZ[nJZIndex].score = score;
				nJZIndex++;
			}
			else if (nClass == CELL_RED_TYPE)
			{
				Cells.HCount++;
				Cells.H[nHIndex].left = left;
				Cells.H[nHIndex].right = right;
				Cells.H[nHIndex].top = top;
				Cells.H[nHIndex].bottom = bottom;
				Cells.H[nHIndex].score = score;
				nHIndex++;
			}
			else if (nClass == CELL_WHITE_TYPE)
			{
				Cells.BCount++;
				Cells.B[nBIndex].left = left;
				Cells.B[nBIndex].right = right;
				Cells.B[nBIndex].top = top;
				Cells.B[nBIndex].bottom = bottom;
				Cells.B[nBIndex].score = score;
				nBIndex++;
			}
			else if (nClass == CELL_DRED_TYPE) //异型红细胞
			{
				Cells.YHCount++;
				Cells.YH[nYHIndex].left = left;
				Cells.YH[nYHIndex].right = right;
				Cells.YH[nYHIndex].top = top;
				Cells.YH[nYHIndex].bottom = bottom;
				Cells.YH[nYHIndex].score = score;
				nYHIndex++;
			}
		}
	}

	return true;
}

int CUSArc::GetCleanLayer(PCHAR pPicPath,PCHAR pIniPath,RECT CutRect,int nBegin,int nEnd,int nCleanLayer,int nCellIndex,BOOL bFocusEP /*= FALSE*/)
{
	char temp[MAX_PATH] = { 0 }, FileName[MAX_PATH] = { 0 }, NewPicPath[MAX_PATH] = { 0 }, Key[MAX_PATH] = { 0 }, CellPicPath[MAX_PATH] = { 0 };

	float      fMax = 0.0;
	int        nMaxIndex = 0;

	DBG_MSG("GetCleanLayer %s......\n",pPicPath);

	CxImage    CutImage,JpgImage; 
	BYTE *     buffer = 0;
	INT        size = 0;
	std::string sData;

	double	nLight[64] = {0.0};
	std::vector<double> vecArticulation(nEnd+1, 0.0f);

	strcpy_s(CellPicPath, pPicPath);
	
	PathAppend(CellPicPath, "cells");
	if (!PathFileExists(CellPicPath)) CreateDirectory(CellPicPath, 0);
	strcpy_s(NewPicPath, CellPicPath);

	int nCatchLayer = GetPrivateProfileInt("Focus","LightCatchLayer",2,m_FocusIniPath);
	int nEpAdjustLayer = GetPrivateProfileInt("Focus","EpAdjustLayer",0,m_FocusIniPath);

	for( int i = nBegin; i <= nEnd; i++ )
	{
		sprintf_s(FileName,"%d.jpg",i);
		PathAppend(pPicPath,FileName);

		sprintf_s(FileName, "%d_%d.jpg", nCellIndex, i);
		PathAppend(NewPicPath,FileName);
		
		cv::Mat src = cv::imread(pPicPath);
		if (!src.empty())
		{
			cv::Mat CutImage = src(cv::Rect(CutRect.left, CutRect.top, CutRect.right - CutRect.left, CutRect.bottom - CutRect.top));
			DBG_MSG("Save cell cut picture: %s\n", NewPicPath);
			cv::imwrite(NewPicPath, CutImage);
			//Tenengrad梯度方法利用Sobel算子分别计算水平和垂直方向的梯度，同一场景下梯度值越高，图像越清晰。以下是具体实现，这里衡量的指标是经过Sobel算子处理后的图像的平均灰度值，值越大，代表图像越清晰。
			cv::Mat imageGrey, imageSobel;
			cv::cvtColor(CutImage, imageGrey, CV_RGB2GRAY);
			cv::Sobel(imageGrey, imageSobel, CV_16U, 1, 1);
			//图像的平均灰度
			double meanValue = 0.0;
			meanValue = mean(imageSobel)[0];
			vecArticulation[i] = meanValue;

			sprintf_s(Key, "articulation_%d_%d", nCellIndex, i);
			sprintf_s(temp, "%f", meanValue);
			WritePrivateProfileString("focus", Key, temp, pIniPath);
		}

		/*if( PathFileExists(pPicPath) == TRUE &&  JpgImage.Load(pPicPath, CXIMAGE_FORMAT_JPG) == TRUE )
		{
			JpgImage.Crop(CutRect.left,CutRect.top, CutRect.right, CutRect.bottom,&CutImage);
			CutImage.Encode(buffer, size, CXIMAGE_FORMAT_JPG);
			//sData = std::string((char*)buffer, size);	
			DBG_MSG("Save cell cut picture: %s\n", NewPicPath);
			CutImage.Save(NewPicPath,CXIMAGE_FORMAT_JPG);

			int nLightCount = 0;
			for( int l = 0; l < CutImage.GetWidth(); l++ )
			{
				for( int j = 0; j < CutImage.GetHeight(); j++ )
				{	
					RGBQUAD color = CutImage.GetPixelColor(l, j, true);		
					if(color.rgbGreen>230||color.rgbRed>210||color.rgbBlue>170)
						//	if( color.rgbGreen < 135 || color.rgbGreen > 230  || color.rgbRed < 135 || color.rgbRed > 210 || color.rgbBlue < 135 || color.rgbBlue > 210  || (color.rgbRed > 180 && color.rgbRed < 200  && color.rgbGreen > 180 && color.rgbGreen < 200 && color.rgbBlue > 180 && color.rgbBlue < 200 && color.rgbBlue >  color.rgbGreen && color.rgbBlue >  color.rgbRed ))
						continue;
					//nLight += CutImage.GetPixelGray(l,j);
					nLight[i] += color.rgbBlue +  color.rgbRed + color.rgbGreen;
					nLightCount++;
				}
			}

			if( nLightCount > 0  )
				nLight[i] /= nLightCount;
			else
				nLight[i] = 0; 


			sprintf_s(Key, "light_%d_%d", nCellIndex, i);
			sprintf_s(temp,"%f",nLight[i]);
			WritePrivateProfileString("focus",Key,temp,pIniPath);

			std::wstringstream wss;

			FOCUS_PARAMS  Fparams = {0};
			IMAGEQUALITY  imgquality = {0};

			wss<<NewPicPath;

			if ( RecFun != NULL ) 
			{ 
				Fparams.fGradient = RecFun(wss.str().c_str());	

				if( fMax < Fparams.fGradient)
				{
					fMax = Fparams.fGradient;
					nMaxIndex = i;
				}
				sprintf_s(Key, "%d_%d_fGradient", nCellIndex, i);
				sprintf_s(temp,"%.2f",Fparams.fGradient);
				WritePrivateProfileString("focus",Key,temp,pIniPath);
			}

			CutImage.Destroy();
			JpgImage.Destroy();
		}*/

		PathRemoveFileSpec(pPicPath);
		PathRemoveFileSpec(NewPicPath);
	}


	fMax = 0.0;
	for(int i = nBegin ; i <= nEnd; i++ )
	{
		if( fMax <= vecArticulation[i] )
		{
			fMax = vecArticulation[i];
			nMaxIndex = i;
		}
	}

	sprintf_s(temp,"%d",nMaxIndex);
	sprintf_s(Key,"%d_nMaxIndex",nCellIndex);
	WritePrivateProfileString("focus",Key,temp,pIniPath);

	if( bFocusEP == TRUE )
		nMaxIndex += nEpAdjustLayer;

	if( nMaxIndex <= 0 )
		nMaxIndex = 1;
	if( nMaxIndex > m_nTotalFocusLyar )
		nMaxIndex = m_nTotalFocusLyar;

	DBG_MSG("nCellIndex: %d,nBegin: %d,nEnd: %d,nMaxIndex %d", nCellIndex, nBegin, nEnd, nMaxIndex);

	return nMaxIndex;
}


int CUSArc::GetCleanLayer(vector<FOCUSIMAGE> &vecFi, PCHAR pPicPath, PCHAR pIniPath, RECT CutRect, int nBegin, int nEnd, int nCleanLayer, int nCellIndex, BOOL bFocusEP /*= FALSE*/)
{
	char temp[MAX_PATH] = { 0 }, FileName[MAX_PATH] = { 0 }, NewPicPath[MAX_PATH] = { 0 }, Key[MAX_PATH] = { 0 }, CellPicPath[MAX_PATH] = { 0 };

	float      fMax = 0.0;
	int        nMaxIndex = 0;

	DBG_MSG("GetCleanLayer %s......\n", pPicPath);

	if (vecFi.size() != nEnd - nBegin + 1)
		return -1;

	struct cellImageInfo
	{
		double grad;
		double light;
		double saturation;
		double areaPre1;
		double areaPre2;
	};
	std::vector<cellImageInfo> vecArticulation(nEnd + 1);

	strcpy_s(CellPicPath, pPicPath);

	PathAppend(CellPicPath, "cells");
	if (!PathFileExists(CellPicPath)) CreateDirectory(CellPicPath, 0);
	strcpy_s(NewPicPath, CellPicPath);

	int nCatchLayer = GetPrivateProfileInt("Focus", "LightCatchLayer", 2, m_FocusIniPath);
	int nEpAdjustLayer = GetPrivateProfileInt("Focus", "EpAdjustLayer", 0, m_FocusIniPath);

	for (int i = nBegin-1; i < nEnd; i++)
	{
		sprintf_s(FileName, "%d.jpg", i);
		PathAppend(pPicPath, FileName);

		sprintf_s(FileName, "%d_%d.jpg", nCellIndex, i);
		PathAppend(NewPicPath, FileName);

		cv::Mat src;
		if (vecFi[i].image.empty())
		{
			src = cv::imread(pPicPath);
		}
		else
		{
			src = vecFi[i].image;
		}
		if (!src.empty())
		{
			cv::Mat CutImage = src(cv::Rect(CutRect.left, CutRect.top, CutRect.right - CutRect.left, CutRect.bottom - CutRect.top));
			DBG_MSG("Save cell cut picture: %s\n", NewPicPath);
			cv::imwrite(NewPicPath, CutImage); // 保存细胞抠图
			//Tenengrad梯度方法利用Sobel算子分别计算水平和垂直方向的梯度，同一场景下梯度值越高，图像越清晰。以下是具体实现，这里衡量的指标是经过Sobel算子处理后的图像的平均灰度值，值越大，代表图像越清晰。
			cv::Mat grayImage, hsvImage;
			cv::cvtColor(CutImage, grayImage, CV_BGR2GRAY);
			cv::cvtColor(CutImage, hsvImage, CV_BGR2HSV); 
			cv::Scalar hsvScalar = cv::mean(hsvImage);

			double gradValue = Tenengrad(grayImage);
			double lightValue = lightGray(grayImage);
			double saturationValue = hsvScalar.val[1];
			Mat mask1, mask2;
			inRange(grayImage, cv::Scalar(0), cv::Scalar(128), mask1);
			inRange(grayImage, cv::Scalar(lightValue), cv::Scalar(255), mask2);
			double grayAreaPreValue = double(cv::countNonZero(mask1)) / mask1.total();
			double lightAreaPreValue = double(cv::countNonZero(mask2)) / mask2.total();

			vecArticulation[i] = { gradValue, lightValue, saturationValue, grayAreaPreValue, lightAreaPreValue };

			sprintf_s(Key, "articulation_%d_%d", nCellIndex, i);
			sprintf_s(temp, "grad:%f,light:%f,saturation:%f,grayAreaPre:%f,lightAreaPre:%f", gradValue, lightValue, saturationValue, grayAreaPreValue, lightAreaPreValue);
			WritePrivateProfileString("focus", Key, temp, pIniPath);
		}

		PathRemoveFileSpec(pPicPath);
		PathRemoveFileSpec(NewPicPath);
	}

	// 计算当前位置细胞最清晰层
	int layer = -1;
	// 计算暗色（灰度图中0~128）最少的层
	double minValue_areaPre1;
	cv::Point  minIdx_areaPre1;
	vector<double> vecGrayAreaPre1;
	for (int i = nBegin; i <= nEnd; i++)
	{
		cellImageInfo &cinfo = vecArticulation[i];
		vecGrayAreaPre1.push_back(cinfo.areaPre1);
	}
	cv::Scalar areaGrayPreMean = cv::mean(vecGrayAreaPre1);
	sprintf_s(temp, "%f", areaGrayPreMean[0]);
	WritePrivateProfileString("focus", "--mean areapre1", temp, pIniPath);

	cv::minMaxLoc(vecGrayAreaPre1, &minValue_areaPre1, 0, &minIdx_areaPre1);
	int areaPre1Layer = minIdx_areaPre1.x + nBegin;
	sprintf_s(temp, "%f", minValue_areaPre1);
	WritePrivateProfileString("focus", "--min areapre1", temp, pIniPath);

	if (areaGrayPreMean[0] < 0.18f) // 阈值，大于则是模糊图
	{
		// 计算暗色（灰度图中0~128）最少的层的前一层和后两层中梯度最大层
		double maxValue_grad;
		cv::Point  maxIdx_grad;
		vector<double> vecGrad;
		int beginIdx = minIdx_areaPre1.x + nBegin;
		int endIdx = 0;
		if (beginIdx < 0)
		{
			beginIdx = 0;
		}
		if (beginIdx + 3>nEnd)
		{
			beginIdx = nEnd - 3;
		}
		endIdx = beginIdx + 3;

		for (int i = beginIdx; i <= endIdx; i++)
		{
			vecGrad.push_back(vecArticulation[i].grad);
		}
		sprintf_s(temp, "[%d, %d, %d, %d]", beginIdx, beginIdx + 1, beginIdx + 2, beginIdx + 3);
		WritePrivateProfileString("focus", "--Layer Range", temp, pIniPath);

		cv::minMaxLoc(vecGrad, 0, &maxValue_grad, 0, &maxIdx_grad);
		int gradLayer = maxIdx_grad.x + beginIdx;
		sprintf_s(temp, "[%d](%f)", gradLayer, maxValue_grad);
		WritePrivateProfileString("focus", "--max grad", temp, pIniPath);

		// 计算暗色（灰度图中0~128）最少的层中灰度图中（均值~255）最多的层
		double maxValue_areaPre2;
		cv::Point  maxIdx_areaPre2;
		vector<double> vecAreaPre2;
		for (int i = beginIdx; i <= endIdx; i++)
		{
			vecAreaPre2.push_back(vecArticulation[i].areaPre2);
		}
		cv::minMaxLoc(vecAreaPre2, 0, &maxValue_areaPre2, 0, &maxIdx_areaPre2);
		int areaPre2Layer = maxIdx_areaPre2.x + beginIdx;
		sprintf_s(temp, "[%d](%f)", areaPre2Layer, maxValue_areaPre2);
		WritePrivateProfileString("focus", "--max areapre2", temp, pIniPath);

		// 计算暗色（灰度图中0~128）最少的层中灰度图中饱和度最大的层
		double maxValue_saturation;
		cv::Point  maxIdx_saturation;
		vector<double> vecSaturation;
		for (int i = beginIdx; i <= endIdx; i++)
		{
			vecSaturation.push_back(vecArticulation[i].saturation);
		}
		cv::minMaxLoc(vecSaturation, 0, &maxValue_saturation, 0, &maxIdx_saturation);
		int saturationLayer = maxIdx_saturation.x + beginIdx;
		sprintf_s(temp, "[%d](%f)", saturationLayer, maxValue_saturation);
		WritePrivateProfileString("focus", "--max saturatio", temp, pIniPath);

		// 计算清晰层
		layer = minIdx_areaPre1.x + nBegin;
		if (gradLayer == areaPre2Layer || gradLayer == saturationLayer)
		{
			layer = gradLayer;
		}
		else if (areaPre2Layer == saturationLayer)
		{
			layer = areaPre2Layer;
		}
		else
		{
			int distance1 = abs(gradLayer - areaPre1Layer);
			int distance2 = abs(areaPre2Layer - areaPre1Layer);
			int distance3 = abs(saturationLayer - areaPre1Layer);
			int minDistance = min3(distance1, distance2, distance3);
			layer = nEnd-nBegin+1;
			if (minDistance == distance1 && gradLayer < layer)
			{
				layer = gradLayer;
			}
			if (minDistance == distance2 && areaPre2Layer < layer)
			{
				layer = areaPre2Layer;
			}
			if (minDistance == distance3 && saturationLayer < layer)
			{
				layer = saturationLayer;
			}
		}
		sprintf_s(Key, "--cell[%d] final layer", nCellIndex);
		sprintf_s(temp, "[%d]", layer);
		WritePrivateProfileString("focus", Key, temp, pIniPath);
	}

	sprintf_s(temp, "%d", nMaxIndex);
	sprintf_s(Key, "%d_nMaxIndex", nCellIndex);
	WritePrivateProfileString("focus", Key, temp, pIniPath);

	if (bFocusEP == TRUE)
		layer += nEpAdjustLayer;

	if (layer <= 0)
		layer = 1;
	if (layer > m_nTotalFocusLyar)
		layer = m_nTotalFocusLyar;

	DBG_MSG("nCellIndex: %d,nBegin: %d,nEnd: %d,layer %d", nCellIndex, nBegin, nEnd, layer);

	return layer;
}

BOOL  CUSArc::LoadInfo()
{
	CHAR DllPath[MAX_PATH] = {0},RecPath[MAX_PATH] = {0};

	GetModuleFileName( NULL, DllPath, MAX_PATH);
	PathRemoveFileSpec(DllPath);

	strcpy_s(RecPath,DllPath);

	PathAppend(DllPath, "shapness.dll");
	PathAppend(RecPath, "Recognition.dll");

	PathAppend(m_FocusIniPath,"Conf.ini");

	//m_nRecSwitch = GetPrivateProfileInt("sys","switch", 0, m_FocusIniPath);

	//m_bOpenRec = GetPrivateProfileInt("sys","OpenRec", 1, m_FocusIniPath);

	//m_nRow = GetPrivateProfileInt("sys","nRow", 1, m_FocusIniPath);

	//m_nCol = GetPrivateProfileInt("sys","nCol", 1, m_FocusIniPath);

//m_nLayer = GetPrivateProfileInt("sys","nLayer",16, m_FocusIniPath);
//m_nLayerStep = GetPrivateProfileInt("sys","nLayerStep", 16, m_FocusIniPath);

	//m_bMorePic = GetPrivateProfileInt("sys","morePic", 0, m_FocusIniPath);

	//m_nSleepTime = GetPrivateProfileInt("sys","SleepTime", 200, m_FocusIniPath); 
	DBG_MSG("m_hDll 1111111 %s", DllPath);

	m_hDll = LoadLibrary(DllPath); 
	if ( m_hDll == NULL )
		return FALSE;
	RecFun = (lpRecFun)GetProcAddress(m_hDll, "ImgSharpness_File");

	DBG_MSG("m_hDll 1111111 %s", RecPath);

	m_hDll = LoadLibrary(RecPath);//AfxFreeLibrary(m_hLangDLL)，动态库应释放，但这并不是我崩溃得原因
	if ( m_hDll == NULL )
		return FALSE;
	AnalysisFileFun = (AnalysisFile)GetProcAddress(m_hDll, "AnalysisFile"); 

	DBG_MSG("m_hDll 222222 ");
	return TRUE;

}

KSTATUS CUSArc::Sample40xGrapControl_noPrecipitate(PROGRAM_OBJECT lc_obj, PROGRAM_OBJECT ca_obj, PROGRAM_OBJECT rec_obj, PBILL3_EXTENSION pExtension, PFINISH_STANDING_INFO finish_info)
{
	char												GrapPath[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 };
	char												BeforceFocusPath[MAX_PATH] = { 0 };
	float												pos = 0.0f;
	ULONG										nSample40xCounter = 0, nTotalPicCounter = 0;
	KSTATUS										status = STATUS_SUCCESS;
	SAMPLING_PROGRESS				s_prg = { 0 };
	SAMPLING_INFORMATION			s_info = { 0 };
	REC_GRAP_FULL_RECORD			rec_full_record = { 0 };
	REC_GRAP_FULL_RECORD			rec_focus_full_record = { 0 };
	BOOL											m_bCancelTask = FALSE;
	DWORD										t1 = 0, t2 = 0, t3 = 0;
	unsigned short							taskID = finish_info->TaskID;
	int												ChannelNum = finish_info->ChannelNum;
	char												buff[64] = { 0 };
	char												confPath[256] = { 0 };

	m_picCountActually = 0;


	GetModuleFileNameA(NULL, confPath, MAX_PATH);
	PathRemoveFileSpecA(confPath);
	PathAppendA(confPath, "\\Conf.ini");
	WritePrivateProfileString(_T("noRecPos"), _T("curcha"), itoa(ChannelNum, buff, 10), confPath);

	char  FocusPicPath[MAX_PATH] = { 0 };//聚焦图片路径
	int nStep = GetPrivateProfileInt(_T("Focus"), _T("step"), 8, focusini);
	int nEnd = GetPrivateProfileInt(_T("Focus"), _T("end"), 160, focusini);
	int nTime = GetPrivateProfileInt(_T("Focus"), _T("time"), 1, focusini);
	int Offset = GetPrivateProfileInt(_T("Focus"), _T("offset"), 1000, focusini);
	int nSwitch = GetPrivateProfileInt(_T("Focus"), _T("switch"), 1, focusini);
	int nFocusWay = GetPrivateProfileInt(_T("Focus"), _T("FocusWay"), 0, focusini);
	int nSavePic = GetPrivateProfileInt(_T("Focus"), _T("SavePic"), 0, focusini);
	int AutoLight = GetPrivateProfileInt(_T("Focus"), _T("AutoLight"), 0, focusini);
	int nBeforeFocusPosXOffset = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosXOffset"), 0, focusini);
	int nBeforeFocusPosYOffset = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosYOffset"), 0, focusini);
	int nBeforeFocusPosXSpace = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosXSpace"), 0, focusini);
	int nBeforeFocusPosYSpace = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosYSpace"), 0, focusini);
	int nBeforeFocusPosRow = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosRow"), 0, focusini);
	int nBeforeFocusPosColumn = GetPrivateProfileInt(_T("Focus"), _T("BeforeFocusPosColumn"), 0, focusini);
	int nHSpeedValue = GetPrivateProfileInt(_T("Focus"), _T("HSpeedValue"), 1500, focusini);

	int nCapturePicDelayMs = GetPrivateProfileInt(_T("Focus"), _T("CapturePicDelayMs"), 10, focusini);
	int nCapturePicDelayMsAutoFocus = GetPrivateProfileInt(_T("Focus"), _T("CapturePicDelayMsAutoFocus"), 0, focusini);
	int nFocusSelLayer = GetPrivateProfileInt(_T("Focus"), _T("FoucsSelLayer"), 1, focusini);

	int EnableQCRec = GetPrivateProfileInt(_T("Focus"), _T("EnableQCRec"), 0, focusini);//质控识别
	int AutoFocusResultOpt = GetPrivateProfileInt(_T("Focus"), _T("AutoFocusResultOpt"), 0, focusini);//自动聚焦修订

	GetPrivateProfileString(_T("Focus"), _T("FoucsSharpness"), "0", temp, sizeof(temp) - 1, focusini);
	float fFoucsSharpness = atof(temp);
	int nTotalStep = 0;
	int picNum = 1;

	int row = GetPrivateProfileInt("Focus", "CaptureImageRow", 4, focusini);
	int column = GetPrivateProfileInt("Focus", "CaptureImageColumn", 8, focusini);
	nTotalPicCounter = nSample40xCounter = row*column * (nEnd/nStep);

	// 设置聚焦图片路径
	m_BillHelper->GetRecGrapReslutPath(GrapPath); // RecGrapReslut
	strcpy_s(FocusPicPath, GrapPath);
	_snprintf_s(temp, sizeof(temp) - 1, "%d\\FocusPic", taskID);  // RecGrapReslut/[id]/FocusPic
	PathAppend(FocusPicPath, temp);

	DBG_MSG("FocusPicPath Path: %s\n", FocusPicPath);
	_snprintf_s(temp, sizeof(temp) - 1, "%s\\%d", GrapPath, taskID);
	if (PathFileExists(temp) == FALSE)//创建聚焦图片文件夹
	{
		CreateDirectory(temp, NULL);
	}
	if (PathFileExists(FocusPicPath) == FALSE)//创建聚焦图片文件夹
	{
		CreateDirectory(FocusPicPath, NULL);
	}

	// 设置细胞图片路径
	_snprintf_s(temp, sizeof(temp) - 1, "%d\\"RESULT_FILE, taskID);
	DBG_MSG("debug: 设置细胞图片路径 = %s", temp);
	PathAppend(GrapPath, temp);
	DeleteFile(GrapPath); // 删除已经存在的识别结果文件
	PathRemoveFileSpec(GrapPath);
	strcpy_s(rec_full_record.ReslutRelativePath, GrapPath);

	s_prg.nTaskID = taskID;

	int bopen = 0;

#ifdef ASYNC_REC
	std::unique_ptr<ASYNC_REC_CONTEXT> pArctx = std::make_unique<ASYNC_REC_CONTEXT>();
	if (pArctx == NULL)
	{
		DBG_MSG("debug:Sample40xGrapControl alloc memeory Failed.\n");
		return STATUS_ALLOC_MEMORY_ERROR;
	}
	PASYNC_REC_CONTEXT pCtx = pArctx.get();
	//memset(pArctx.get(), 0, sizeof(ASYNC_REC_CONTEXT));
	pArctx->nRecCompleteCount = 0;
	pArctx->nSample40xCounter = nTotalPicCounter;
	pArctx->nTotalCounter = nTotalPicCounter;
#endif
	// 采图

	//采图前Z轴先移动到一端
	bool z_Direction_up = Backward;
	MoveZ(lc_obj, z_Direction_up, nEnd / 2);

	int nIndex = 0;
	for (int nX = 0; nX < row; nX++)
	{
		for (int nY = 0; nY < column; nY++)
		{
			z_Direction_up = !z_Direction_up;

			int xPos = (nX % 2 == 0) ? nY : (column - nY - 1);
			int yPos = nX;
			nIndex = (nEnd / nStep)*(nX*column + nY) + 1;
			DBG_MSG("debug: move x nIndex = %d", nIndex);
			//移动x轴
			for (USHORT nRetry = 0; nRetry < RETRY_COUNTER; nRetry++)
			{
				status = Move40x(lc_obj, finish_info->ChannelNum, xPos, yPos);
				if (status != STATUS_SUCCESS)
				{
					DBG_MSG("Move40x(%d, %d) err= %u\n", xPos, yPos, status);
					continue;
				}
				DBG_MSG("Move40x retry %d\n", nRetry);
				break;
			}
			// 移动后，添加延时
			Sleep(nCapturePicDelayMs);
			//移动Z采图
			for (int i = 0; i < nEnd / nStep; i++)
			{
				WaitForSingleObject(m_hPauseEvent, INFINITE);
				{
					std::lock_guard<std::mutex> lck(pArctx->mutex);
					if (m_BillHelper->QueryTaskOverByFail())
					{
						status = STATUS_CANCEL_ACTION;
						m_bCancelTask = TRUE;
						DBG_MSG("debug:Sample40xGrapControl STATUS_CANCEL_ACTION\n");
						goto LOOP_END;
					}
				}
				DBG_MSG("debug: move z nIndex = %d", nIndex);
				MoveZ(lc_obj, z_Direction_up, nStep);
				sprintf_s(temp, "%s\\%.3u_%.4u.jpg", GrapPath, 40, (nIndex - 1));
				// 真正40倍采图
				status = CaptureCameraGrap(ca_obj, 0, temp);
				if (nSample40xCounter != 0)
				{
					s_prg.nPos = (float)(nIndex) / (float)nSample40xCounter * 100;
					m_BillHelper->AddStatusInformation(ThmeInformationType, SamplingProgress, &s_prg, sizeof(s_prg));
				}
				if (status != STATUS_SUCCESS)
				{
					DBG_MSG("debug:采图错误:Sample40xGrapControl->CaptureCameraGrap Err = %#x\n", status);
					continue;
				}
				DBG_MSG("debug:Task(%d) nSample40xCounter(%d) index %d\n", finish_info->TaskID, nSample40xCounter, nIndex);
				rec_full_record.g_rc.rGrapSize = 40;
				rec_full_record.g_rc.rIndex = (USHORT)(nIndex - 1);
				rec_full_record.rTaskID = taskID;
				strcpy_s(rec_full_record.GrapPath, temp);

				//异步识别
				PREC_GRAP_CONTEXT_WARP pRgcw = (PREC_GRAP_CONTEXT_WARP)calloc(1, sizeof(REC_GRAP_CONTEXT_WARP));
				if (pRgcw == NULL)
				{
					DBG_MSG("debug:40x Rec Grap alloc memeory Error\n");
					std::lock_guard<std::mutex> lck(pArctx->mutex);
					pArctx->nRecCompleteCount++;
					if (pArctx->nRecCompleteCount == nSample40xCounter)
					{
						s_info.nTaskID = taskID;
						s_info.nSampleIndex = rec_full_record.g_rc.rIndex;
						s_info.nGrapSize = rec_full_record.g_rc.rGrapSize;
						s_info.Coefficient = 1.0f;
						m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));
						m_bCancelTask = TRUE;
					}
					continue;
				}
				rec_full_record.Reserved[1] = EnableQCRec;
				memcpy(&pRgcw->rgfr, &rec_full_record, sizeof(REC_GRAP_FULL_RECORD));
				pRgcw->pContext = this;
				pRgcw->pContext2 = rec_obj;
				pRgcw->recCtx = pCtx;

				DBG_MSG("debug:[%d]%d加入识别队列\n", taskID, (nIndex - 1));
				m_worker.appendTask([=](void *ctx) {
					if (!ctx)
					{
						return;
					}
					PREC_GRAP_CONTEXT_WARP p = (PREC_GRAP_CONTEXT_WARP)ctx;
					p->rgfr.Reserved[2] = ChannelNum;
					DBG_MSG("[%d]%d开始识别\n", p->rgfr.rTaskID, p->rgfr.g_rc.rIndex);
					KSTATUS status = FMMC_EasySynchronousBuildPRP(p->pContext2, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &p->rgfr, sizeof(REC_GRAP_FULL_RECORD), NULL, 0, FALSE, FALSE);
					if (status != STATUS_SUCCESS)
					{
						DBG_MSG("Rec Grap Error = %u\n", status);
					}
					PRP_REC_COMPLETE_CALLBACK2((CUSArc *)p->pContext, p);
				}, (void*)pRgcw);
				nIndex++;
			}
		}
		DWORD t4 = GetTickCount();
		DBG_MSG("采图耗时：%d ms", (t4 - t3));
	}
LOOP_END:
	if (m_bCancelTask)
	{
		ALLOW_TEST_CONTEXT ctx = { 0 };
		ctx.TaskId = taskID;
		m_BillHelper->AddStatusInformation(ThmeInformationType, RespondTaskFail, &ctx, sizeof(ALLOW_TEST_CONTEXT));
	}

#ifdef ASYNC_REC
	pArctx.release();
#endif

	return STATUS_SUCCESS; //表示整个流程顺利完成
}
