#include "StdAfx.h"
#include "USArc.h"
#include "DbgMsg.h"
#include <shlwapi.h>
#include <MATH.H>
#include <malloc.h>

#include <sstream>
#include "ximage.h"
#pragma comment(lib, "..\\lib\\cximage.lib")
#pragma comment(lib, "..\\lib\\png.lib")
#pragma comment(lib, "..\\lib\\Jpeg.lib")
#pragma comment(lib, "..\\lib\\zlib.lib")
#include <atlimage.h>

#define   REC_GRAP_PATH               "RecGrapReslut"
#define   ADDITION_INFO_FILE          "Addition.ini"
#define   SECTION_10X                 "10xInfo"
#define   SECTION_40X                 "40xInfo"
#define   COUNTER_KEY                 "Count"
#define   CELL_COUNT_KEY              "CellCount"

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

CUSArc::CUSArc()
{
	m_BillPrgObj = NULL;
	m_hPauseEvent = m_Request_Event = NULL;
	m_BillHelper = NULL;
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


	FMMC_GetPrgObjectExtension(BillPrgObj, (PVOID*)&pExtension);
	GetModuleFileName(NULL,pExtension->prg_status.GrapPath,MAX_PATH);
	PathRemoveFileSpec(pExtension->prg_status.GrapPath);


	strcpy(focusini,pExtension->prg_status.GrapPath);
	PathAppend(focusini,"Config\\Focus.ini");//聚焦配置文件2019


	PathAppend(pExtension->prg_status.GrapPath, REC_GRAP_PATH);




	m_BillPrgObj = BillPrgObj;
	m_BillHelper = pBillHelper;
	return STATUS_SUCCESS;
}



void CUSArc::USWorkThread( PVOID pContext )
{
	CUSArc *us_arc = (CUSArc*)pContext;
	
	us_arc->USSampleControl();
}


void CUSArc::USSampleControl()         //US采集控制过程
{
	FINISH_STANDING_INFO   finish_stnd_info;
	
	
	while( TRUE )
	{
		WaitForSingleObject(m_Request_Event, INFINITE);
		
		if( m_BillHelper->QueryThreadExit() )
			break;
		
		while( Que_GetHeaderData(m_USSampleTaskQue, &finish_stnd_info) )
			USSampleOne(&finish_stnd_info);
	}
	
}


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
	
/*	status = m_BillHelper->UpdateUSStatus(pStandInfo->TaskID, &bFullTaskFinish);
	if( status != STATUS_SUCCESS )
	{
		//由于缓冲队列中没有该记录,则不需要发送消息到UI
		DBG_MSG("StartSampleUSTask->UpdateUSStatus Not Found nTaskID = %u\n", pStandInfo->TaskID);
		LCUSSampleFinish(lc_obj, pStandInfo->ChannelNum);
		return;
	}*/

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
	LCUSSampleFinish(lc_obj, pStandInfo->ChannelNum);
	
	s_info.nTaskID = pStandInfo->TaskID;

	DBG_MSG("尿沉渣采图任务完成...TaskID = %u\n", s_info.nTaskID);
	
	if( status != STATUS_SUCCESS )
	{
		m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFailByLC, &s_info, sizeof(s_info));
//		m_BillHelper->RemoveTaskID(s_info.nTaskID);
	}
	else
	{
		if( n40xCounter == 0 )
			s_info.Coefficient = 1.0f;
		else
			s_info.Coefficient = (32.0f / (float)pExtension->s10x_range.nSample10xCount) * ((float)nTotalCounter / (float)n40xCounter);
		m_BillHelper->AddStatusInformation(ThmeInformationType, SampleTaskFinish, &s_info, sizeof(s_info));
	}


	m_BillHelper->SetFailOverTask(FALSE);
	
	//sprintf(temp_name, "temp_%u.jpg", )
	
	
}


//10倍镜移动
KSTATUS CUSArc::Move10x( PROGRAM_OBJECT  lc_obj, UCHAR n10xIndex, PFINISH_STANDING_INFO finish_info )
{
	LC_COMMAND                             lc_cmd;
	PLC_LOCATE_10_MICROSCOPE_INFO          l10 = (PLC_LOCATE_10_MICROSCOPE_INFO) &lc_cmd;
	
	l10->lc_header.flag = LC_FLAG;
	l10->lc_header.nLCCmd = LC_LOCATE_10_MICROSCOPE;
	l10->LocateInfo.ChannelNum = finish_info->ChannelNum;
	l10->LocateInfo.IsReturnTakePicCommand = RETURN_SAMPLE_CMD;
	l10->LocateInfo.YPicNum = n10xIndex >> 3;
	l10->LocateInfo.XPicNum = n10xIndex & 0x7;
	
	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
	
}

//40倍镜移动
KSTATUS CUSArc::Move40x( PROGRAM_OBJECT lc_obj, UCHAR  nChannel, PSAMPLE_40X_INFO s_40 )
{
	LC_COMMAND                             lc_cmd = {0};
	PLC_LOCATE_40_MICROSCOPE_INFO          l40 = (PLC_LOCATE_40_MICROSCOPE_INFO) &lc_cmd;
	
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
	
	l40->LocateInfo.IsReturnTakePicCommand = RETURN_SAMPLE_CMD;
	return FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
}





KSTATUS CUSArc::MoveZ(PROGRAM_OBJECT lc_obj,UCHAR Direction,WORD StepCount)
{
	LC_COMMAND                             lc_cmd = {0};
	PLC_MOVE_PLATFORM_INFO               lp =(PLC_MOVE_PLATFORM_INFO)&lc_cmd;

	lp->lc_header.flag = LC_FLAG;
	lp->lc_header.nLCCmd = LC_MOVE_SCAN_PLATFORM;
	lp->MoveInfo.MotorID= (UCHAR)ZMotorID;
	lp->MoveInfo.Direction =(UCHAR)Direction;
	lp->MoveInfo.StepCountHigh = UCHAR(StepCount >> 8);
	lp->MoveInfo.StepCountLow= UCHAR(StepCount & 0XFF);

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

	sprintf(key_name, "%u", finish_info->TaskID);
	PathAppend(task_path, key_name);
	CreateDirectory(task_path, NULL);
	
	hA10x = AnalysePic10x_Create(pExtension->prg_status.nTotalSamplingNum);
	if( hA10x == NULL )
	{
		DBG_MSG("AnalysePic10x_Create err...\n");
		return  STATUS_ALLOC_MEMORY_ERROR;
	}
	
	strcpy(ini_path, task_path);
	PathAppend(ini_path, ADDITION_INFO_FILE);
	DeleteFile(ini_path);
	sprintf(v, "%u", pExtension->s10x_range.nSample10xCount);
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
		
		sprintf(key_name,"%.3u_%.4u.jpg",10,j);
		PathAppend(task_path, key_name);
		status = CaptureCameraGrap(ca_obj, 0, task_path);
		sprintf(key_name, "%u", j);
		if( status != STATUS_SUCCESS )
			strcpy(v, "cap error");
		else
			sprintf(v, "%u", pExtension->s10x_range.pSample10xIndexArray[j]);
		
		WritePrivateProfileString(SECTION_10X, key_name, v, ini_path);
		if( status != STATUS_SUCCESS )
			DBG_MSG("10x Capture Grap Error = %s\n", task_path);
		else
		{
			if( AnalysePic10x_Entry(hA10x, task_path, (UCHAR)pExtension->s10x_range.pSample10xIndexArray[j]) == FALSE )
				DBG_MSG("AnalysePic10x_Entry = %s err\n ",task_path);
			
		}
		
		PathRemoveFileSpec(task_path);
		nPos  = (float)(j + 1) / (float)pExtension->s10x_range.nSample10xCount;
		nPos *= 50.0f;
		s_prg.nPos = (int)nPos;
		m_BillHelper->AddStatusInformation(ThmeInformationType, SamplingProgress, &s_prg, sizeof(s_prg));
		
	}

	AnalysePic10x_Finish(hA10x);

	nRecNumber = 0;
	AnalysePic10x_RecCellNumber(hA10x, &nRecNumber);
	sprintf(v, "%u", nRecNumber);
	WritePrivateProfileString(SECTION_10X, CELL_COUNT_KEY, v, ini_path);

	
	return STATUS_SUCCESS;
	
}


//40倍镜采图
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
	strcpy(rec_full_record.ReslutRelativePath, GrapPath);

	strcpy(FocusPicPath,GrapPath);//聚焦图片路径2019
 

	char temp[MAX_PATH]={0};
	sprintf_s(temp,"%d\\FocusPic",finish_info->TaskID);

	PathAppend(FocusPicPath,temp);

	strcpy(rec_focus_full_record.ReslutRelativePath,FocusPicPath); 

    DBG_MSG("FocusPicPath  %s\n",FocusPicPath);
	if( PathFileExists(FocusPicPath) == FALSE )//创建聚焦图片文件夹2019
	{
		CreateDirectory(FocusPicPath, NULL);
		DWORD cc = GetLastError();
		DBG_MSG("FocusPicPath  %d\n",cc);
	}




	PathAppend(GrapPath, "Reslut.ini");
	DeleteFile(GrapPath);
	PathRemoveFileSpec(GrapPath);
	PathAppend(GrapPath, "temp_40x.jpg");
	s_prg.nTaskID = finish_info->TaskID;

	
	
	for( j = 0 ; j < nSample40xCounter ; j ++ )
	{
		DBG_MSG("nSample40xCounter  %d\n",j);
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

       
		if (j==0)
		{

			MoveZ(lc_obj, Foreward, nStep*(nEnd/nStep-1)/2);
			while( nTotalStep < nEnd )
			{
				WORD zStep = 0;
				CString num=_T("");
				num.Format(_T("\\temp%d.jpg"),picNum);
				PathAppend(FocusPicPath,num);

				CaptureCameraGrap(ca_obj, 0, FocusPicPath);

				FindCell(1,picNum,FocusPicPath);

				DBG_MSG("FocusPicPath2  %s\n",FocusPicPath);

				rec_focus_full_record.g_rc.rGrapSize = 1;
				rec_focus_full_record.g_rc.rIndex    =(USHORT)picNum;
				rec_focus_full_record.rTaskID = finish_info->TaskID;
				strcpy(rec_focus_full_record.GrapPath,FocusPicPath);
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
		strcpy(rec_full_record.GrapPath, GrapPath);
		status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_full_record, sizeof(rec_full_record), NULL, 0, FALSE, FALSE);
		if( status != STATUS_SUCCESS )
		{
			DBG_MSG("Rec Grap Error = %u\n", status);
			continue;
		}
		
		pos         = (float)(j+1) / (float)nSample40xCounter;
		s_prg.nPos  = (int)(pos * 50.0f);
		s_prg.nPos += 50;
		m_BillHelper->AddStatusInformation(ThmeInformationType, SamplingProgress, &s_prg, sizeof(s_prg));
	}
	
	free(s_40_i);

	return STATUS_SUCCESS; //表示整个流程顺利完成
	
}

void CUSArc::LCUSSampleFinish( PROGRAM_OBJECT lc_obj, UCHAR  nChannel)
{
	LC_COMMAND                     lc_cmd = {0};
	PLC_SAMPLE_FINISH_TYPE         s_finish = (PLC_SAMPLE_FINISH_TYPE)&lc_cmd;
	
	
	s_finish->lc_header.flag = LC_FLAG;
	s_finish->lc_header.nLCCmd = LC_SAMPLE_FINISH;
	s_finish->FinishSampleInfo.ChannelNum = nChannel;
	FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
	
}


KSTATUS CUSArc::CaptureCameraGrap( PROGRAM_OBJECT CaObj,UCHAR nCaIndex,PCHAR pFilePath )
{
	
	USHORT                 idx = nCaIndex;
	HANDLE                 hRawBuffer = NULL;
	KSTATUS                status;
	FRAME_FILE_INFO        file_info = {0};
	
	
	status = FMMC_EasySynchronousBuildPRP(CaObj, NULL, PRP_AC_READ, RAW_GRAP_BUFFER, &idx, sizeof(USHORT), &hRawBuffer, sizeof(HANDLE), FALSE, FALSE);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("CaptureCameraGrap->Read Buffer Error = %x...\n",status);
		return status;
	}
	
	file_info.hRawGrapBuffer = hRawBuffer;
	file_info.nDHIndex       = nCaIndex;
	strcpy(file_info.FrameFilePath,pFilePath);
	status = FMMC_EasySynchronousBuildPRP(CaObj, NULL, PRP_AC_WRITE, FRAME_FILE, (PVOID)&file_info, sizeof(file_info), NULL, 0, FALSE, FALSE);
	return status;
	
}


KSTATUS CUSArc::StartUSEvent( PFINISH_STANDING_INFO pStandInfo ) //静止时间到,发出开始us采集事件
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

	Que_ReleaseList(m_USSampleTaskQue);
}



void CUSArc::FindCell(int type,int num,PCHAR Path)
{
	if(PathFileExists(Path) == TRUE )
		{
			std::wstringstream wss;
			FOCUS_PARAMS  Fparams = {0};
			IMAGEQUALITY  imgquality = {0};

			wss<<Path;

			//if ( m_nRecSwitch == 0 )
			//{
			//	if ( RecFun != NULL ) 
			//	{ 
			//		Fparams.fGradient = RecFun(wss.str().c_str());	
			//		Fparams.fSharpness = 0;
			//	}			
			//}
			//else
			//{
				if( AnalysisFileFun != NULL )
				{
					if (AnalysisFileFun(wss.str().c_str(),imgquality))
					{
						Fparams.fGradient = imgquality.fGradient;
						Fparams.fSharpness = imgquality.fSharpness;
					}	

				}
				/*if (AnalysisFile(wss.str().c_str(),imgquality))
				{
					Fparams.fGradient = imgquality.fGradient;
					Fparams.fSharpness = imgquality.fSharpness;
				}	*/	
			//}	

			if( type == 2 )
				Fparams.nLayer = num;
			else if( type == 1 )
				Fparams.nLayer = num;

			m_vecFocus.push_back(Fparams);

			DBG_MSG("m_vecFocus  Fparams.nLayer:%d\n",Fparams.nLayer);
			//if( bSavePic == FALSE )
			//	DeleteFile(file_info.FrameFilePath);
		}

}

int CUSArc::GetFocusResult(PROGRAM_OBJECT rec_obj,ULONG nID)
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
	sprintf_s(temp,"%d\\FocusPic\\result.ini",nID);
	PathAppend(FocusIni,temp);

	strcpy_s(PicPath,FocusIni);
	PathRemoveFileSpec(PicPath);


	PathAppend(ConfIni,"Conf.ini");

	int nWay = GetPrivateProfileInt("sys","FocusWay",2,ConfIni);
	
	int nDirection = GetPrivateProfileInt("sys","nDirection",0,ConfIni);

	nCount = m_vecFocus.size();
	if( nCount == 0 )
		return 0;

	if( nWay == 0 )//取最大值
	{
		DBG_MSG("nWay 000000000");
		for ( int i = 0; i <  m_vecFocus.size(); i++ )
		{
			if ( nMaxGradient < m_vecFocus[i].fGradient )
			{
				nMaxGradient = m_vecFocus[i].fGradient;
				nLayer = m_vecFocus[i].nLayer;			
			}

			sprintf_s(temp,"fGradient:%.2f,fSharpness:%.2f ",m_vecFocus[i].fGradient,m_vecFocus[i].fSharpness);
			sprintf_s(Key,"%d",i+1);
			WritePrivateProfileString("focus",Key,temp,FocusIni);
		}
		sprintf_s(temp,"%d",nLayer);
		WritePrivateProfileString("sys","Clealayer",temp,m_FocusIniPath);
       
		GetFocusResultEX(nID);
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

			sprintf_s(temp,"fGradient:%.2f,fSharpness:%.2f ",m_vecFocus[i].fGradient,m_vecFocus[i].fSharpness);
			sprintf_s(Key,"%d",i+1);
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
			sprintf_s(temp,"fGradient:%.2f,fSharpness:%.2f ",m_vecFocus[i].fGradient,m_vecFocus[i].fSharpness);
			sprintf_s(Key,"%d",i+1);
			WritePrivateProfileString("focus",Key,temp,FocusIni);
		}

		nLayer = GetCleanNum(nCount,m_vecFocus,nDirection);		
	}
	
lc:	
	m_vecFocus.clear();
	DBG_MSG("GetFocusResult 111111111 path:%s   nLayer:%d\n",FocusIni,nLayer);
	if( nLayer == 0 )
		return 0;

	sprintf_s(temp,"%d",nLayer);
	WritePrivateProfileString("sys","Clealayer",temp,m_FocusIniPath);	
	WritePrivateProfileString("focus","Clealayer",temp,FocusIni);


	nLayer = GetFocusResultByAIRec(rec_obj,nID,PicPath ,nLayer);

	DBG_MSG("GetFocusResult 111111111 path:%s   nLayer:%d\n",FocusIni,nLayer);

	return nLayer;
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


int CUSArc::GetCleanNum(int nLayerCount,std::vector<FOCUS_PARAMS> vecFocus,int nDirection)
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

			CImage Img;

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

int CUSArc::GetFocusResultByAIRec(PROGRAM_OBJECT rec_obj,ULONG nID,PCHAR pFocusPath,int nCleanLayer)
{
	char temp[MAX_PATH] = {0},IniPath[MAX_PATH] = {0},FileName[MAX_PATH] = {0},NewPicPath[MAX_PATH] = {0},Key[MAX_PATH] = {0};
	int  nCutCount = 1,nCutIndex = 0, nTryCount = 0,nRecIndex = 0 ,nRecTempIndex = 0;

	int  nMaxSmallCellCount = 0,nMaxSmallCellIndex = 0;

	BOOL bFocusEp = FALSE;

	//nCleanLayer = GetPrivateProfileInt("sys","Clealayer",0,m_FocusIniPath);
	
	int nMinEpCount  = GetPrivateProfileInt("focus","MinEpCount",8,m_FocusIniPath);
	int nMaxCutCount = GetPrivateProfileInt("focus","MaxCutCount",10,m_FocusIniPath);
	int nFocusCount = GetPrivateProfileInt("focus","FocusCount",11,m_FocusIniPath);
	m_nTotalFocusLyar = GetPrivateProfileInt("sys","nFocusLayer",30,m_FocusIniPath);

	int nPicWidth  = GetPrivateProfileInt("focus","PicWidth",1440,m_FocusIniPath);
	int nPicHeight = GetPrivateProfileInt("focus","PicWidth",1080,m_FocusIniPath);
	int nMaxDeviation = GetPrivateProfileInt("focus","MaxDeviation",2,m_FocusIniPath);

	//添加聚焦AI识别
	REC_GRAP_FULL_RECORD    rec_grap = {0};
	rec_grap.rTaskID    = nID;	
	rec_grap.g_rc.rGrapSize  = 1;

	RECT CutRect[30] = {0};

	AllCellInfo1 CellImg = {0};


	int nRt = 0;

	int   nMaxIndex = {0};
	int   nTempMaxIndex[30] = {0};

	int nBegin = 0;
	int nEnd   = nFocusCount;

	strcpy_s(NewPicPath,pFocusPath);

	strcpy_s(IniPath,pFocusPath);
	PathAppend(IniPath,"Reslut.ini");

	DBG_MSG("ini path %s\n",IniPath);

	
	
	for (int j = 1; j<=nEnd;++j)
	{
		char tempnum[MAX_PATH]={0};
		sprintf_s(tempnum,"%d",j);
		int nncount = GetPrivateProfileInt(tempnum,"Count",0,IniPath);

		DBG_MSG("ini nncount1 %d\n",nncount);
		if (nncount > 0)
		{
			nBegin = j;
			break;
		}

	}
	
	////for(int f =nBegin+1;f<=nEnd;++f)
	////{
	////	char tempnum[MAX_PATH]={0};
	////	sprintf_s(tempnum,"%d",f);
	////	int nncount = GetPrivateProfileInt(tempnum,"Count",0,IniPath);
	////	DBG_MSG("ini nncount2 %d\n",nncount);
	////	if (nncount == 0)
	////	{
	////		nEnd = f-1;
	////		break;
	////	}	
	////}

	//if (nBegin+1>nEnd)
	//{
	//	nEnd = nBegin;
	//}

    if (nEnd == -1)
    {
		return nEnd;
    }













	









	//if( nFocusCount < 0 )
	//{
	//	nBegin = 1 ;
	//	nEnd  = nFocusCount;
	//}
	//else if( nFocusCount > 0 )
	//{
	//	nBegin = 1;
	//	nEnd   = nFocusCount;
	//}
	//else
	//	return nCleanLayer;


	//if( nBegin < 1 )
	//	nBegin = 1;
	//if( nEnd > 11 )
	//	nEnd = 11;

	nRecTempIndex = 1;	



Rec_lc:
	strcpy_s(rec_grap.GrapPath,pFocusPath);

	if( nRecTempIndex == nBegin)
		nRecIndex = nCleanLayer;
	else 
		if( nRecTempIndex > nBegin && nRecTempIndex <= nEnd )
		{
			nRecIndex = nRecTempIndex;
			DBG_MSG("nRecIndex %d\n", nRecIndex);
		}
	else
	{
		DBG_MSG("nRecIndex2 %d\n", nRecIndex);
		goto getRecResut_lc;	
	}
	sprintf_s(temp,"temp%d.jpg",nRecIndex);
    //sprintf_s(temp,"%d\\%.3u_%.4u.jpg",nID,1,nRecIndex);
	PathAppend(rec_grap.GrapPath,temp);

	DBG_MSG("rec_grap 9999999 %s\n", rec_grap.GrapPath);

	rec_grap.g_rc.rGrapSize = 1;
	rec_grap.g_rc.rIndex = nRecIndex;

	PathRemoveFileSpec(rec_grap.GrapPath);
	strcpy(rec_grap.ReslutRelativePath,rec_grap.GrapPath);
	//rec_grap.nLayer = nRecIndex;

	//if( Que_InsertData(m_RecQue, &rec_grap) == TRUE ) 
		////SetEvent(m_hRequestEvent);

	//KSTATUS status = FMMC_EasySynchronousBuildPRP(rec_obj, NULL, PRP_AC_CREATE, BUILD_REC_RESLUT, &rec_grap, sizeof(rec_grap), NULL, 0, FALSE, FALSE);
	//if( status != STATUS_SUCCESS )
	//{
	//	DBG_MSG("Rec Grap Error = %u\n", status);
	//	//continue;
	//}

	//while( GetPrivateProfileInt("focus","rec",-1,IniPath) != 0 )
	//{
	//	if( nTryCount > 25 )
	//	{
	//		nTryCount = 0;
	//		break;
	//	}
	//	Sleep(200);
	//	nTryCount++;
	//}	

	//sprintf_s(temp,"%d",-1);
	//WritePrivateProfileString("focus","rec",temp,IniPath);

getRecResut_lc:	
/*	if( nRecTempIndex > nEnd && nMaxSmallCellIndex == 0 )
	{
		nRecIndex = nCleanLayer;
	}
	else*/
	if( nRecTempIndex > nEnd && nMaxSmallCellIndex > 0 )
	{
		nRecIndex = nMaxSmallCellIndex;
		nMaxSmallCellIndex = 0;
	}


	GetAiRecResult(pFocusPath,nRecIndex,CellImg,nRt);
	

	int nSmallCellCount = CellImg.MJCount + CellImg.YHCount + CellImg.HCount + CellImg.BCount + CellImg.CSJCount + CellImg.CSCount;

	if(  nSmallCellCount <  nMaxCutCount ) //小细胞未达到设置个数
	{	
		if(nSmallCellCount == 0 && CellImg.SPCount >= nMinEpCount ) //小细胞为0，上皮达到设置个数，以上皮找清晰层
		{
			nCutIndex = 0;
			goto Sp_lc;
		}

		if( nMaxSmallCellCount < nSmallCellCount ) //取小细胞较多的层
		{
			nMaxSmallCellCount = nSmallCellCount;
			nMaxSmallCellIndex = nRecIndex;
			DBG_MSG("取小细胞较多的层 %d,%d\n", nRecIndex,nMaxSmallCellCount);
		}

		if( nRecTempIndex <= nEnd ) //小细胞未达到设置个数,逐层识别
		{
			nRecTempIndex++;

			DBG_MSG("nRecTempIndex %d\n", nRecTempIndex);
			goto Rec_lc;
		}
	}

	if( CellImg.MJCount > 0 )
	{
		DBG_MSG("BZ......\n");

		m_bFindBAST = TRUE;
		m_nTempLayer = nRecIndex;
		m_nFocusID = nID;

		if( CellImg.MJCount >= nMaxCutCount )
			nCutCount = nMaxCutCount;
		else
			nCutCount = CellImg.MJCount;
		DBG_MSG("CellImg.MJCount:%d,nMaxCutCount:%d,nCutCount:%d\n",CellImg.MJCount,nMaxCutCount,nCutCount);

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[nCutIndex].left = CellImg.MJ[k].left; 
			CutRect[nCutIndex].right = CellImg.MJ[k].right; 
			CutRect[nCutIndex].top = CellImg.MJ[k].top; 
			CutRect[nCutIndex].bottom = CellImg.MJ[k].bottom; 
			nCutIndex++;
		}

		
	}

	if( CellImg.YHCount > 0 )
	{
		DBG_MSG("YSBZ......\n");

		if( CellImg.YHCount >= nMaxCutCount )
			nCutCount = nMaxCutCount;
		else
			nCutCount = CellImg.YHCount;
		DBG_MSG("CellImg.MJCount:%d,nMaxCutCount:%d,nCutCount:%d\n",CellImg.YHCount,nMaxCutCount,nCutCount);

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[nCutIndex].left = CellImg.YH[k].left; 
			CutRect[nCutIndex].right = CellImg.YH[k].right; 
			CutRect[nCutIndex].top = CellImg.YH[k].top; 
			CutRect[nCutIndex].bottom = CellImg.YH[k].bottom; 
			nCutIndex++;
		}


	}

	if( CellImg.BCount > 0 )
	{
		DBG_MSG("B......\n");

		if( CellImg.BCount + nCutIndex >= nMaxCutCount )
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = CellImg.BCount;

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[nCutIndex].left = CellImg.B[k].left; 
			CutRect[nCutIndex].right = CellImg.B[k].right; 
			CutRect[nCutIndex].top = CellImg.B[k].top; 
			CutRect[nCutIndex].bottom = CellImg.B[k].bottom; 
			nCutIndex++;
		}		
	}

	if( CellImg.HCount > 0 )
	{
		DBG_MSG("H......\n");

		if( CellImg.HCount + nCutIndex >= nMaxCutCount )
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = CellImg.HCount;

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[nCutIndex].left = CellImg.H[k].left; 
			CutRect[nCutIndex].right = CellImg.H[k].right; 
			CutRect[nCutIndex].top = CellImg.H[k].top; 
			CutRect[nCutIndex].bottom = CellImg.H[k].bottom; 
			nCutIndex++;
		}
	}

	if( CellImg.CSJCount > 0 )
	{
		DBG_MSG("JS......\n");
		if( CellImg.XYCount + nCutIndex >= nMaxCutCount )
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = CellImg.CSJCount;

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[nCutIndex].left = CellImg.CSJ[k].left; 
			CutRect[nCutIndex].right = CellImg.CSJ[k].right; 
			CutRect[nCutIndex].top = CellImg.CSJ[k].top; 
			CutRect[nCutIndex].bottom = CellImg.CSJ[k].bottom; 
			nCutIndex++;
		}
	}
	
	if( CellImg.CSCount > 0 )
	{
		DBG_MSG("DC......\n");
		if( CellImg.CSCount + nCutIndex >= nMaxCutCount )
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = CellImg.CSCount;

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[nCutIndex].left = CellImg.CS[k].left; 
			CutRect[nCutIndex].right = CellImg.CS[k].right; 
			CutRect[nCutIndex].top = CellImg.CS[k].top; 
			CutRect[nCutIndex].bottom = CellImg.CS[k].bottom; 
			nCutIndex++;
		}
	}	

	/*if( CellImg.GJCount > 0 )
	{
		DBG_MSG("GJ......\n");
		if( CellImg.GJCount  + nCutIndex >= nMaxCutCount )
			nCutCount = nMaxCutCount - nCutIndex;
		else
			nCutCount = CellImg.GJCount;

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[nCutIndex].left = CellImg.GJ[k].left; 
			CutRect[nCutIndex].right = CellImg.GJ[k].right; 
			CutRect[nCutIndex].top = CellImg.GJ[k].top; 
			CutRect[nCutIndex].bottom = CellImg.GJ[k].bottom; 
			nCutIndex++;
		}
	}*/

	nCutCount = nCutIndex;

	if( nCutCount >  0 ) 
	{
		goto Cut_lc;
	}


	if( CellImg.SPCount > 0 )
	{
Sp_lc:
		DBG_MSG("SP......\n");
		
		bFocusEp = TRUE;

		DBG_MSG("Find SP......\n");

		if( CellImg.SPCount >= nMaxCutCount )
			nCutCount = nMaxCutCount;
		else
			nCutCount = CellImg.SPCount;

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[k].left = CellImg.SP[k].left; 
			CutRect[k].right = CellImg.SP[k].right; 
			CutRect[k].top = CellImg.SP[k].top; 
			CutRect[k].bottom = CellImg.SP[k].bottom; 
		}
	}
	else if( CellImg.XYCount > 0 )
	{
		DBG_MSG("XS......\n");

		if( CellImg.XYCount >= nMaxCutCount )
			nCutCount = nMaxCutCount;
		else
			nCutCount = CellImg.XYCount;

		for( int k = 0; k < nCutCount; k++ )
		{
			CutRect[k].left = CellImg.XY[k].left; 
			CutRect[k].right = CellImg.XY[k].right; 
			CutRect[k].top = CellImg.XY[k].top; 
			CutRect[k].bottom = CellImg.XY[k].bottom; 
		}
	}	 
	else
	{
// 		if( m_nExperienceLayer > 0 )
// 			return m_nExperienceLayer;
// 		return 0;
		return nCleanLayer;
	}

Cut_lc:
	for( int l = 0 ; l < nCutCount; l++ )
	{
		if( CutRect[l].left < 0 )
			CutRect[l].left = 0;
		if( CutRect[l].right > nPicWidth  )
			CutRect[l].left = nPicWidth;
		if( CutRect[l].top < 0 )
			CutRect[l].top = 0;
		if( CutRect[l].bottom > nPicHeight )
			CutRect[l].bottom = nPicHeight;
	}

		
	
	std::vector<LAYER_COUNT> vecLayerCount;
	LAYER_COUNT LayerCount= {0};
	int nLayerCount = 0;

	nMaxIndex = nCleanLayer;

	for( int i = 0; i < nCutCount; i++ )
	{
		nTempMaxIndex[i] = GetCleanLayer(pFocusPath,IniPath,CutRect[i],nBegin,nEnd,nCleanLayer,i,bFocusEp);

		//if( nTempMaxIndex[i] == nCleanLayer )
		//	continue;

		//if( nTempMaxIndex[i] - nCleanLayer  > nMaxDeviation )
		//	nTempMaxIndex[i] = nCleanLayer + nMaxDeviation;
		//else if( nCleanLayer - nTempMaxIndex[i] > nMaxDeviation )
		//	nTempMaxIndex[i] = nCleanLayer - nMaxDeviation;

		//if( nTempMaxIndex[i] <= 0 )
		//	nTempMaxIndex[i] = 1;
		//else if( nTempMaxIndex[i] > m_nTotalFocusLyar )
		//	nTempMaxIndex[i] = m_nTotalFocusLyar;

		//nMaxIndex += nTempMaxIndex[i];

		//取重复个数多的层
		LayerCount.nCount = 1;
		LayerCount.nLayer = nTempMaxIndex[i];

		if( i == 0 )
		{			
			vecLayerCount.push_back(LayerCount);
		}
		else
		{
			BOOL bFind = FALSE;
			for( int k = 0; k < vecLayerCount.size(); k++ )
			{
				if( vecLayerCount[k].nLayer == LayerCount.nLayer )
				{
					vecLayerCount[k].nCount++;
					bFind = TRUE;
					break;
				}
			}
			if( bFind == FALSE )
				vecLayerCount.push_back(LayerCount);
		}

	}


	for( int k = 0; k < vecLayerCount.size(); k++ )
	{
		if( nLayerCount < vecLayerCount[k].nCount )
		{			
			nLayerCount = vecLayerCount[k].nCount;	
			nMaxIndex = vecLayerCount[k].nLayer;
		}
		else if( nLayerCount == vecLayerCount[k].nCount )
		{
			if( nMaxIndex > vecLayerCount[k].nLayer )
			{
				nLayerCount = vecLayerCount[k].nCount;	
				nMaxIndex = vecLayerCount[k].nLayer;
			}
		}
	}
	
	//nMaxIndex = (int) ( (float)nMaxIndex / (float)nCutCount + 0.5 );
	
	

	sprintf_s(temp,"%.d",nMaxIndex);
	WritePrivateProfileString("focus","nMaxIndex",temp,IniPath);
	
	//nMaxIndex = IdentifyNew(pFocusPath,nBegin,nEnd,nMaxIndex);

	sprintf_s(temp,"%d",nMaxIndex);
	WritePrivateProfileString("sys","Clealayer",temp,m_FocusIniPath);	

	return nMaxIndex;
}

void CUSArc::GetAiRecResult(PCHAR path,int nCleanLayer,AllCellInfo1 & CellImg,int & ret)
{
	char SessionName[MAX_PATH] = {0},temp[MAX_PATH] = {0}, str[MAX_PATH] = {0},PicPath[MAX_PATH] = {0};

	int nBIndex = 0,nSpIndex = 0, nGJIndex = 0,nMJIndex = 0,nYSBZIndex = 0,nXSIndex = 0,nJSIndex = 0,nDCIndex = 0,nHIndex = 0;

	strcpy_s(PicPath,path);
	sprintf_s(SessionName,"%d",nCleanLayer);

	PathAppend(PicPath, "Reslut.ini");

	DBG_MSG("AI picPath  %s\n",PicPath);
	int ncount = GetPrivateProfileInt(SessionName,"Count",0,PicPath);

	ret = GetPrivateProfileInt("focus","rec",0,PicPath);

	int left = 0, right = 0, top = 0,bottom = 0, nClass = 0,score = 0;

	DBG_MSG("RecConut %d\n",ncount);

	ZeroMemory(&CellImg,sizeof(AllCellInfo1));

	for( int i = 0 ; i < ncount; i++ )
	{
		sprintf(str, "%u", i);
		GetPrivateProfileString(SessionName,str,"",temp,MAX_PATH,PicPath);
		sscanf(temp,"%d,%d,%d,%d,%d,%d",
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

int CUSArc::GetCleanLayer(PCHAR pPicPath,PCHAR pIniPath,RECT CutRect,int nBegin,int nEnd,int nCleanLayer,int nCellIndex,BOOL bFocusEP /*= FALSE*/)
{
	char temp[MAX_PATH] = {0},FileName[MAX_PATH] = {0},NewPicPath[MAX_PATH] = {0},Key[MAX_PATH] = {0};

	float      fMax = 0.0;
	int        nMaxIndex = {0};

	DBG_MSG("GetCleanLayer %s......\n",pPicPath);

	CxImage    CutImage,JpgImage; 
	BYTE *     buffer = 0;
	INT        size = 0;
	std::string sData;

	double	nLight[64] = {0.0};

	strcpy_s(NewPicPath,pPicPath);

	int nCatchLayer = GetPrivateProfileInt("Focus","LightCatchLayer",2,m_FocusIniPath);
	int nEpAdjustLayer = GetPrivateProfileInt("Focus","EpAdjustLayer",0,m_FocusIniPath);

	for( int i = nBegin; i <= nEnd; i++ )
	{
		sprintf_s(FileName,"temp%d.jpg",i);
		PathAppend(pPicPath,FileName);

		sprintf_s(FileName,"%d_%d.jpg",nCellIndex,i);
		PathAppend(NewPicPath,FileName);

		if( PathFileExists(pPicPath) == TRUE &&  JpgImage.Load(pPicPath, CXIMAGE_FORMAT_JPG) == TRUE )
		{
			JpgImage.Crop(CutRect.left,CutRect.top, CutRect.right, CutRect.bottom,&CutImage);
			CutImage.Encode(buffer, size, CXIMAGE_FORMAT_JPG);
			sData = std::string((char*)buffer, size);	
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


			sprintf_s(Key,"light_%d_%d",nCellIndex,i);
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
				sprintf_s(Key,"%d_%d_fGradient",nCellIndex,i);
				sprintf_s(temp,"%.2f",Fparams.fGradient);
				WritePrivateProfileString("focus",Key,temp,pIniPath);
			}

			CutImage.Destroy();
			JpgImage.Destroy();
		}

		PathRemoveFileSpec(pPicPath);
		PathRemoveFileSpec(NewPicPath);
	}


	if (nBegin == m_nTotalFocusLyar)
	{

	}

	nBegin = nCleanLayer - nCatchLayer;
	nEnd   = nCleanLayer + nCatchLayer;

	//nBegin = nMaxIndex - nCatchLayer;
	//nEnd   = nMaxIndex + nCatchLayer;

	if( nBegin < 1 )
		nBegin = 1;
	if( nEnd > m_nTotalFocusLyar )
		nEnd = m_nTotalFocusLyar;

	fMax = 0.0;
	DBG_MSG("nCleanLayer %d,nCatchLayer %d,nBegin %d,nEnd %d,nMaxIndex %d",nCleanLayer,nCatchLayer,nBegin,nEnd,nMaxIndex);
	for(int i = nBegin ; i <= nEnd; i++ )
	{
		if( fMax <= nLight[i] )
		{
			fMax = nLight[i];
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

	return nMaxIndex;
}


BOOL  CUSArc::LoadInfo()
{
	CHAR DllPath[MAX_PATH] = {0},RecPath[MAX_PATH] = {0};

	GetModuleFileName( NULL, DllPath, MAX_PATH);
	PathRemoveFileSpec(DllPath);

//strcpy_s(m_FocusIniPath,DllPath);
	strcpy_s(RecPath,DllPath);

//PathAppend(DllPath,"shapness.dll");
	PathAppend(RecPath,"Recognition.dll");

	PathAppend(m_FocusIniPath,"Conf.ini");

	//m_nRecSwitch = GetPrivateProfileInt("sys","switch", 0, m_FocusIniPath);

	//m_bOpenRec = GetPrivateProfileInt("sys","OpenRec", 1, m_FocusIniPath);

	//m_nRow = GetPrivateProfileInt("sys","nRow", 1, m_FocusIniPath);

	//m_nCol = GetPrivateProfileInt("sys","nCol", 1, m_FocusIniPath);

//m_nLayer = GetPrivateProfileInt("sys","nLayer",16, m_FocusIniPath);
//m_nLayerStep = GetPrivateProfileInt("sys","nLayerStep", 16, m_FocusIniPath);

	//m_bMorePic = GetPrivateProfileInt("sys","morePic", 0, m_FocusIniPath);

	//m_nSleepTime = GetPrivateProfileInt("sys","SleepTime", 200, m_FocusIniPath); 

//m_hDll = LoadLibrary(DllPath); 
//if ( m_hDll == NULL )
//	return FALSE;

//RecFun = (lpRecFun)GetProcAddress(m_hDll, "ImgSharpness_File");

	DBG_MSG("m_hDll 1111111 %s",RecPath);

	m_hDll = LoadLibrary(RecPath);
	if ( m_hDll == NULL )
		return FALSE;
	AnalysisFileFun = (AnalysisFile)GetProcAddress(m_hDll, "AnalysisFile"); 

	DBG_MSG("m_hDll 222222 ");
	return TRUE;

}