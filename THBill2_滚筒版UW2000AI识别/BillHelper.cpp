#include "StdAfx.h"
#include "BillHelper.h"
#include "QueueManager.h"
#include <SHLWAPI.H>
#include <MALLOC.H>
#include "DbgMsg.h"
#include <math.h>
#include "..\..\inc\THInterface.h"


#define   REC_GRAP_PATH               "RecGrapReslut"
#define   INVAILD_TASK_ID             0XFFFFFFFF

#pragma pack(1)


typedef  struct _PRG_TASK_STATUS
{
	ULONG      nTaskID;
	CheckType  type;
	BOOLEAN    bCommitUsTask;
	BOOLEAN    bCommitUdcTask;

}PRG_TASK_STATUS, *PPRG_TASK_STATUS;

#define  FILL_TASK_ACTION            0X1
#define  REMOVE_TASK_BY_TASKID       0X2
#define  UPDATE_US_STATUS_BY_TASKID  0X3
#define  UPDATE_UDC_STATUS_BY_TASKID 0X4

typedef  struct _PRG_TASK_CONTEXT
{
	ULONG   nAction;
	PVOID   pInputBuffer;
	PVOID   pOutputBuffer;
	

}PRG_TASK_CONTEXT, *PPRG_TASK_CONTEXT;


////////////////////////////////////////////////////////
#define LC_FORCE_CLEAR            0X00000001                //强制清洗
#define LC_NORMAL_CLEAR           0X00000002                //普通清洗
#define LC_NEW_TASK               0X00000003                //新任务
#define LC_FAIL_NEW_TASK          0X00000004                //任务失败
#define LC_SAMPLING_PROGRESS      0X00000005                //采样过程中
#define LC_START_STATUS           0X00000006			    //下位机开机
#define LC_QUERY_CONFIG_INFO      0X00000007                //下位机配置信息
#define LC_SET_CONFIG_INFO        0X00000008	            //设置下位机配置信息
#define LC_RESET_INFO             0X00000009                //下位机复位状态
#define LC_SCAN_MOVE              0X0000000A                //扫描平台点动
#define LC_SAVE_Z_SETP_NUMBER     0X0000000B                //记录Z轴频数
#define LC_SOLENOID_1             0X0000000C	            //电阀#1
#define LC_SOLENOID_2             0X0000000D                //电阀#2
#define LC_PUMP_REVERSION         0X0000000E                    
#define LC_LOCATION               0X0000000F
#define LC_CLEANING_DROP          0X00000020
#define LC_COLLECTION_DROP        0X00000021
#define LC_WASTE_DISCHARGE        0X00000022
#define LC_LIGHT_ON               0X00000023
#define LC_LIGHT_OFF              0X00000024
#define LC_PUMP_COROTATION        0X00000025 
#define LC_MOVE_40_LENSE          0X00000026
#define LC_IMPORT_MODE            0X00000027
#define LC_START_FORCE_CLEAR      0X00000028
#define LC_OVER_FORCE_CLEAR       0X00000029
#define LC_FORCE_CLEAR_TIMEOUT    0X0000002A
#define LC_QUERY_DEV_VERSION      0X0000002B

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

struct TASKID_INFO
{
	UCHAR SuccessFlag;
	WORD TaskID;
};

typedef TASKID_INFO *PTASKID_INFO;

////////////////////////////////////////////////////////

#pragma pack()

CBillHelper::CBillHelper()
{
	m_BillPrgObj = m_THMMgrObj = m_PrgTask = NULL;
	m_hPauseEvent = m_Request_Event = NULL;
	m_FailOverUSTask = m_ExitThread = 0;

}

KSTATUS  CBillHelper::StartCameraDevice(PROGRAM_OBJECT  caObj, PBILL3_EXTENSION pExtension)
{
	ULONG                     nCaNumber = 0;
	KSTATUS                   status = STATUS_SUCCESS, status2 = STATUS_SUCCESS;
	DEVICE_CAP_DES            dev_cap;
	INIT_CAMERA_PARAMETERS    init = {0};

	if( pExtension->o_dev.s_ca_dev )
		return STATUS_DEV_START_ALREADY;

	ZeroMemory(&dev_cap, sizeof(dev_cap));

	dev_cap.CapDesType    = UsageDevNumber;
	dev_cap.pOutputBuffer = (PCHAR)&nCaNumber;
	dev_cap.nOutputBufferLength = sizeof(nCaNumber);


	status =  FMMC_EasySynchronousBuildPRP(caObj, NULL, PRP_AC_QUERY, QUERY_DEVICE_CAP, &dev_cap, sizeof(DEVICE_CAP_DES), NULL, 0, FALSE, FALSE);
	if( status != STATUS_SUCCESS )
		return status;

	if( nCaNumber < 1 )
		return STATUS_NOT_FOUND_OBJECT;

	//新uw2000有两个摄像头（沉渣和性状）
	//修改新uw2000只有一个摄像头（沉渣） 2020.11.25
// 	int ret = MessageBox(NULL, "提示", "选择第一个摄像头？", MB_YESNOCANCEL);
// 	if (IDYES == ret)
// 	{
		init.nCaDevIndex = 0;
		status = FMMC_EasySynchronousBuildPRP(caObj, NULL, PRP_AC_CREATE, OPEN_CAMERA, (PVOID)&init, sizeof(init), NULL, 0, FALSE, FALSE);
		init.nCaDevIndex = 1;
		//status2 = FMMC_EasySynchronousBuildPRP(caObj, NULL, PRP_AC_CREATE, OPEN_CAMERA, (PVOID)&init, sizeof(init), NULL, 0, FALSE, FALSE);
		if (status == STATUS_SUCCESS && status2 == STATUS_SUCCESS)
			pExtension->o_dev.s_ca_dev = caObj;
/*	}*/

// 	else if (ret == IDNO)
// 	{
// 		init.nCaDevIndex = 1;
// 		status = FMMC_EasySynchronousBuildPRP(caObj, NULL, PRP_AC_CREATE, OPEN_CAMERA, (PVOID)&init, sizeof(init), NULL, 0, FALSE, FALSE);
// 		init.nCaDevIndex = 1;
// 		//status2 = FMMC_EasySynchronousBuildPRP(caObj, NULL, PRP_AC_CREATE, OPEN_CAMERA, (PVOID)&init, sizeof(init), NULL, 0, FALSE, FALSE);
// 		if (status == STATUS_SUCCESS && status2 == STATUS_SUCCESS)
// 			pExtension->o_dev.s_ca_dev = caObj;
// 	}
// 	else
// 	{
// 		exit(0);
// 	}
	return status;
}

KSTATUS  CBillHelper::StartLCDevice(PROGRAM_OBJECT BillObj, PROGRAM_OBJECT  lcObj, PBILL3_EXTENSION pExtension)
{
	KSTATUS         status;
	START_LC_PAR    StartLC = {0};


	StartLC.PrgObject = BillObj;

	if( pExtension->o_dev.s_lc_dev )
		return STATUS_DEV_START_ALREADY;
	
	status = FMMC_EasySynchronousBuildPRP(lcObj, NULL, PRP_AC_CREATE, START_LC_REQUEST, (PVOID)&StartLC, sizeof(StartLC), NULL, 0, FALSE, FALSE);
	if( status == STATUS_SUCCESS )
		pExtension->o_dev.s_lc_dev = lcObj;
	
	return status;
}

KSTATUS CBillHelper::StartRECDevice(PROGRAM_OBJECT recObj, PBILL3_EXTENSION pExtension)
{
	ULONG                  nRecDevNumber = 0;
	KSTATUS                status;
	DEVICE_CAP_DES         dev_cap;
	REC_CELL_DESCRITOR     rd = {0};

	if( pExtension->o_dev.s_rec_dev )
		return STATUS_DEV_START_ALREADY;

	ZeroMemory(&dev_cap, sizeof(dev_cap));
	dev_cap.CapDesType    = UsageDevNumber;
	dev_cap.pOutputBuffer = (PCHAR)&nRecDevNumber;
	dev_cap.nOutputBufferLength = sizeof(nRecDevNumber);
	
	status =  FMMC_EasySynchronousBuildPRP(recObj, NULL, PRP_AC_QUERY, QUERY_DEVICE_CAP, &dev_cap, sizeof(DEVICE_CAP_DES), NULL, 0, FALSE, FALSE);

	DBG_MSG("StartRECDevice...%d\n",status);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( nRecDevNumber < 1 )
		return STATUS_NOT_FOUND_OBJECT;

	DBG_MSG("nRecDevNumber...%d\n",nRecDevNumber);
	//uw2000使用的重大的识别算法,由外部配置来决定,这里只负责启动识别算法.
	
	status = FMMC_EasySynchronousBuildPRP(recObj, NULL, PRP_AC_CREATE, START_REC_CELL, (PVOID)&rd, sizeof(rd), NULL, 0, FALSE, FALSE);

	DBG_MSG("START_REC_CELL...%x\n",status);

	if( status == STATUS_SUCCESS )
		pExtension->o_dev.s_rec_dev = recObj;

	return STATUS_SUCCESS;
}

KSTATUS  CBillHelper::StartUDCDevice(PROGRAM_OBJECT BillObj, PROGRAM_OBJECT PrgObj, PBILL3_EXTENSION pExtension)
{
	KSTATUS         status;
	START_LC_PAR    StartLC = {0};

	DBG_MSG("pExtension->o_dev.StartUDCDevice...\n");

	StartLC.PrgObject = BillObj;
	
	if( pExtension->o_dev.s_udc_dev )
		return STATUS_DEV_START_ALREADY;

	status = FMMC_EasySynchronousBuildPRP(PrgObj, NULL, PRP_AC_CREATE, START_LC_REQUEST, (PVOID)&StartLC, sizeof(StartLC), NULL, 0, FALSE, FALSE);
	if( status == STATUS_SUCCESS )
	{
		pExtension->o_dev.s_udc_dev = PrgObj;
		DBG_MSG("pExtension->o_dev.s_udc_dev = %x\n", PrgObj);
	}
	else
		DBG_MSG("pExtension->o_dev.s_udc_dev Run Fail status = %x\n", status);

	return status;
}

KSTATUS  CBillHelper::StartLisDevice(PROGRAM_OBJECT PrgObj, PBILL3_EXTENSION pExtension)
{
	KSTATUS         status;
	START_LC_PAR    StartLC = {0};
	
	if( pExtension->o_dev.s_lis_dev )
		return STATUS_DEV_START_ALREADY;
	
	status = FMMC_EasySynchronousBuildPRP(PrgObj, NULL, PRP_AC_CREATE, START_LC_REQUEST, (PVOID)&StartLC, sizeof(StartLC), NULL, 0, FALSE, FALSE);
	if( status == STATUS_SUCCESS )
		pExtension->o_dev.s_lis_dev = PrgObj;
	
	return status;
}

KSTATUS  CBillHelper::StartHelperDevice(PROGRAM_OBJECT StartPrgObj, TDEVICE_DESCRITOR  nDevDes)
{
	KSTATUS          status;
	START_LC_PAR     StartLC = {0};
	PBILL3_EXTENSION pExtension;

	FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
	
	if( nDevDes == T_DEV_CAMERA_TYPE )
		status = StartCameraDevice(StartPrgObj, pExtension);
	else if( nDevDes == T_DEV_LC_TYPE )
		status = StartLCDevice(m_BillPrgObj, StartPrgObj, pExtension);
	else if( nDevDes == T_DEV_REC_TYPE )
		status = StartRECDevice(StartPrgObj, pExtension);
	else if( nDevDes == T_DEV_UDC_TYPE )
		status = StartUDCDevice(m_BillPrgObj,StartPrgObj, pExtension);
	else if( nDevDes == T_DEV_LIS_TYPE )
		status = StartLisDevice(StartPrgObj, pExtension);
	else
		status = STATUS_INVALID_REQUEST;

	return status;
}

KSTATUS  CBillHelper::InitializeStatusInformation(PROGRAM_OBJECT BillPrgObj)
{
	KSTATUS                status;
	PBILL3_EXTENSION       pExtension;

	status = StartMMgrDevice();
	if( status != STATUS_SUCCESS )
		return status;

	m_PrgTask = Que_Initialize(sizeof(PRG_TASK_STATUS));
	if( m_PrgTask == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	m_BillPrgObj = BillPrgObj;

	FMMC_GetPrgObjectExtension(BillPrgObj, (PVOID*)&pExtension);
	pExtension->o_dev.s_thmgr_dev = m_THMMgrObj;


	return status;
}

KSTATUS CBillHelper::AddStatusInformation(InformationType i_type, ThmeInfoClass i_class, PVOID  pContent, ULONG nLength, BOOL bSysBuffer, PVOID pContext)
{
	TH_STATUS_INFORMATION  info;
	
	if( m_THMMgrObj == NULL )
		return STATUS_DEV_NOT_START;
	
	
	ZeroMemory(&info, sizeof(info));
	
	info.i_type    = i_type;
	info.i_class   = i_class;
	info.pContext  = pContext;
	info.content.bSysBuffer = bSysBuffer;
	info.content.length     = nLength;
	info.content.buffer = (PCHAR)pContent;
	if (info.content.buffer != NULL && nLength < sizeof(ULONG))
		info.content.length = sizeof(ULONG);
	return FMMC_EasySynchronousBuildPRP(m_THMMgrObj, NULL, PRP_AC_WRITE, MMGR_MSG_REQUEST, &info, sizeof(info), NULL, 0, FALSE, FALSE);
}

void   CBillHelper::ClearTaskStatus(PBILL3_EXTENSION   pExtension)
{
	EnterCriticalSection(&pExtension->cs);
	CLEAR_CANCEL_FALG(pExtension->prg_status.b_BillStatus);
	CLEAR_BILL_PROGRESSING(pExtension->prg_status.b_BillStatus);
	CANCEL_TASK_FAIL(pExtension->prg_status.b_TaskStatus);
	pExtension->prg_status.nCurrentSamplingNum = 0;
	pExtension->prg_status.nFailSamplingNum = 0;
	pExtension->prg_status.nTaskID = 0;
	pExtension->prg_status.GrapPath[0] = 0;
	LeaveCriticalSection(&pExtension->cs);
}

void  CBillHelper::GetRecGrapReslutPath(PCHAR   pPath)
{
	GetModuleFileName(NULL, pPath, MAX_PATH);
	PathRemoveFileSpec(pPath);
	PathAppend(pPath, REC_GRAP_PATH);
}

void  CBillHelper::CloseAllDevice(PBILL3_EXTENSION  pExtension)
{
	EnterCriticalSection(&pExtension->cs);
	
	if( pExtension->o_dev.s_lc_dev )
		FMMC_EasySynchronousBuildPRP(pExtension->o_dev.s_lc_dev, NULL,PRP_AC_CLOSE, STOP_LC_REQUEST,NULL,0,NULL,0,FALSE,FALSE);
	if( pExtension->o_dev.s_udc_dev )
		FMMC_EasySynchronousBuildPRP(pExtension->o_dev.s_udc_dev, NULL,PRP_AC_CLOSE, STOP_LC_REQUEST,NULL,0,NULL,0,FALSE,FALSE);
	if( pExtension->o_dev.s_lis_dev )
		FMMC_EasySynchronousBuildPRP(pExtension->o_dev.s_lis_dev, NULL,PRP_AC_CLOSE, STOP_LC_REQUEST,NULL,0,NULL,0,FALSE,FALSE);
	if( pExtension->o_dev.s_ca_dev )
		FMMC_EasySynchronousBuildPRP(pExtension->o_dev.s_ca_dev, NULL,PRP_AC_CLOSE, CLOSE_CAMERA_REQUEST,NULL,0,NULL,0,FALSE,FALSE);
	if( pExtension->o_dev.s_rec_dev )
		FMMC_EasySynchronousBuildPRP(pExtension->o_dev.s_rec_dev, NULL,PRP_AC_CLOSE, CLOSE_CELL_REQUEST,NULL,0,NULL,0,FALSE,FALSE);

	ZeroMemory(&pExtension->o_dev, sizeof(pExtension->o_dev));

	LeaveCriticalSection(&pExtension->cs);
	
}

void  CBillHelper::Finish12sSampleStep(PROGRAM_OBJECT lc_prg_obj, ULONG nTotalSampleNumber, ULONG nCurrentSampleIndex)
{
	if( nTotalSampleNumber == 160 ) //12s采160幅图片为默认不处理结束相关工作
		return;

	if( nCurrentSampleIndex + 1 == nTotalSampleNumber ) //已经处于设定采图数量,需要进行相关的处理工作
		Stop12sSampleAction(lc_prg_obj);
}

void  CBillHelper::FinishSampleStep(PROGRAM_OBJECT  lc_prg_obj, ULONG nTotalSampleNumber, ULONG nCurrentSampleIndex)
{
	CHAR     DevName[128];
	KSTATUS  status;


	status = FMMC_QueryPrgObjectName(lc_prg_obj, DevName);
	if( status != STATUS_SUCCESS )
		return;

	if( strcmp(US12S_NAME, DevName) == 0 )
		Finish12sSampleStep(lc_prg_obj, nTotalSampleNumber, nCurrentSampleIndex);


}

void CBillHelper::Stop12sSampleAction(PROGRAM_OBJECT lc_prg_obj)
{
	LC_COMMAND    lc_cmd = {0};

	lc_cmd.lc_header.flag   = LC_FLAG;
	lc_cmd.lc_header.nLCCmd = LC_NORMAL_CLEAR;

	FMMC_EasySynchronousBuildPRP(lc_prg_obj, NULL, PRP_AC_DEVIO,CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);

}

void  CBillHelper::StopLCSampleAction(PROGRAM_OBJECT  lc_prg_obj)
{
	CHAR     DevName[128];
	KSTATUS  status;
	
	
	status = FMMC_QueryPrgObjectName(lc_prg_obj, DevName);
	if( status != STATUS_SUCCESS )
		return;
	
	if( strcmp(US12S_NAME, DevName) == 0 )
		Stop12sSampleAction(lc_prg_obj);

}


void  CBillHelper::LCNormalClear(PROGRAM_OBJECT  lc_prg_obj)
{
	Stop12sSampleAction(lc_prg_obj);

}

KSTATUS CBillHelper::StartMMgrDevice()
{
	KSTATUS   status;
	
	status = FMMC_GetPrgObjectByName(TH_MMGR_NAME, &m_THMMgrObj);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("CBillHelper::InitializeStatusInformation fail1 status = %x\n", status);
		return status;
	}
	
	status = FMMC_EasySynchronousBuildPRP(m_THMMgrObj, NULL, PRP_AC_CREATE, MMGR_CREATE_REQUEST, NULL, 0, NULL, 0, FALSE, FALSE);
	if( status != STATUS_SUCCESS )
		m_THMMgrObj = NULL;
	
	DBG_MSG("CBillHelper::InitializeStatusInformation ok...\n");
	
	return status;
	
}

KSTATUS CBillHelper::InsertNewTask(UCHAR  nHN, UCHAR nCN, BOOLEAN bTaskResult)
{
	
	PRG_TASK_STATUS    task_status = {0};
	SAMPLE_TASK_INFO   s_task_info;


	if( bTaskResult == FALSE ) //如果吸样失败,则直接告诉界面
		goto send_msg;

	task_status.nTaskID = INVAILD_TASK_ID;
//	if( Que_InsertData(m_PrgTask, &task_status) == FALSE )
//		return STATUS_ALLOC_MEMORY_ERROR;

send_msg:
	SetFailOverTask(FALSE);
	ZeroMemory(&s_task_info, sizeof(s_task_info));
	s_task_info.nHN = nHN;
	s_task_info.nCN = nCN;
	s_task_info.bTaskResult = bTaskResult;
	AddStatusInformation(ThmeInformationType, NewSampleTask, &s_task_info, sizeof(SAMPLE_TASK_INFO));
	DBG_MSG("Find New Task Created!nHN = %u, nCN = %u, bTaskResult = %u!but no task id...\n", nHN, nCN, bTaskResult);
	return STATUS_SUCCESS;
	
}


static
ULONG
CALLBACK
PrgTaskCB(
PPRG_TASK_CONTEXT        pTaskContext,
PPRG_TASK_STATUS         pTaskStatus
)
{
	ULONG               nTaskID;
	PRESPOND_SAMPLE     res_sample;

	if( pTaskContext->nAction == FILL_TASK_ACTION )
	{
		if( pTaskStatus->nTaskID == INVAILD_TASK_ID )
		{
			res_sample           = (PRESPOND_SAMPLE)pTaskContext->pInputBuffer;
			pTaskStatus->nTaskID = res_sample->nTaskID;
			pTaskStatus->type    = res_sample->type;
			return QUE_FIND_OK | QUE_FIND_OVER | QUE_FIND_UPDATE;
		}

	}
	else if( pTaskContext->nAction == REMOVE_TASK_BY_TASKID )
	{
		nTaskID = (ULONG)pTaskContext->pInputBuffer;

		if( pTaskStatus->nTaskID != INVAILD_TASK_ID && ((pTaskStatus->nTaskID & 0xffff) && (nTaskID & 0xffff)) )
			return QUE_FIND_OK | QUE_FIND_OVER | QUE_FIND_DELETE;
	}
	else if( pTaskContext->nAction == UPDATE_US_STATUS_BY_TASKID || pTaskContext->nAction == UPDATE_UDC_STATUS_BY_TASKID )
	{
		nTaskID = (ULONG)pTaskContext->pInputBuffer;

		if( pTaskStatus->nTaskID != INVAILD_TASK_ID && ((pTaskStatus->nTaskID & 0xffff) && (nTaskID & 0xffff)) )
		{
			if( pTaskContext->nAction == UPDATE_US_STATUS_BY_TASKID )
				pTaskStatus->bCommitUsTask  = TRUE;
			else
				pTaskStatus->bCommitUdcTask = TRUE;

			if( pTaskStatus->type == CheckAllType )
			{
				if( pTaskStatus->bCommitUdcTask && pTaskStatus->bCommitUsTask )
				{
					pTaskContext->pOutputBuffer = (PVOID)TRUE;
					return QUE_FIND_OK | QUE_FIND_OVER | QUE_FIND_DELETE;
				}
			}
			else if( pTaskStatus->type == CheckUsType )
			{
				if( pTaskStatus->bCommitUsTask  )
				{
					pTaskContext->pOutputBuffer = (PVOID)TRUE;
					return QUE_FIND_OK | QUE_FIND_OVER | QUE_FIND_DELETE;
				}
			}
			else if( pTaskStatus->type == CheckUdcType )
			{
				if( pTaskStatus->bCommitUdcTask  )
				{
					pTaskContext->pOutputBuffer = (PVOID)TRUE;
					return QUE_FIND_OK | QUE_FIND_OVER | QUE_FIND_DELETE;
				}

			}

			pTaskContext->pOutputBuffer = (PVOID)FALSE;
			return QUE_FIND_OK | QUE_FIND_OVER | QUE_FIND_UPDATE;
		}
	}

	return QUE_FIND_NEXT;

}


KSTATUS CBillHelper::FillTaskID(PRESPOND_SAMPLE   pResSample)
{
	PRG_TASK_CONTEXT  ctx = {0};

	ctx.nAction = FILL_TASK_ACTION;
	ctx.pInputBuffer = (PVOID)pResSample;

	if( Que_FindByCallBack(m_PrgTask, (FINDCALLBACK)PrgTaskCB, &ctx) == FALSE )
		return STATUS_NOT_FOUND_VALUE;

	return STATUS_SUCCESS;
	
}

KSTATUS CBillHelper::SendTaskIDToLC( PRESPOND_SAMPLE res_sample )  //通知下位机ID号
{
	USHORT             nRetry = 0;
	KSTATUS            status;
	LC_COMMAND         lc_cmd = {0};
	//PTASKID_INFO       t_info;
	PALLOW_TEST_CONTEXT ctx = NULL;
	PROGRAM_OBJECT     lc_obj;
	PBILL3_EXTENSION   pExtension;


	FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);
	
	EnterCriticalSection(&pExtension->cs);
	lc_obj = pExtension->o_dev.s_lc_dev;
	LeaveCriticalSection(&pExtension->cs);

	lc_cmd.lc_header.flag = LC_FLAG; 
	lc_cmd.lc_header.nLCCmd = (res_sample->nHN == 11 ? UC_ALLOW_EMERGENCYTEST : UC_ALLOW_TEST);
	ctx = (PALLOW_TEST_CONTEXT)lc_cmd.Reserved;
	ctx->TaskId = (uint16_t)res_sample->nTaskID;
	ctx->TestType = (uint16_t)res_sample->type;
	//lc_cmd.lc_header.nLCCmd = LC_SEND_TASKID;
	//t_info              = (PTASKID_INFO)lc_cmd.Reserved;
	//t_info->TaskID      = (WORD)res_sample->nTaskID;
	//t_info->SuccessFlag = (res_sample->nHN != 0xff);
	//if( t_info->SuccessFlag == FALSE )
	//	RemoveTaskID(INVAILD_TASK_ID);


retry_send_loc1:

	status = FMMC_EasySynchronousBuildPRP(lc_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(lc_cmd), NULL, 0, FALSE, FALSE);

	if( status == STATUS_SUCCESS )
		return status;

	if( nRetry >= 3 )
	{
		Sleep(200);
		nRetry++;
		goto retry_send_loc1;
	}

	//错误处理
	RemoveTaskID(res_sample->nTaskID);
	SAMPLING_INFORMATION   s_info = {0};
	s_info.nTaskID = res_sample->nTaskID;
	AddStatusInformation(ThmeInformationType, RespondTaskFail, ctx, sizeof(ALLOW_TEST_CONTEXT));

	return status;
}


void CBillHelper::RemoveTaskID( ULONG nTaskID )
{
	PRG_TASK_CONTEXT  ctx = {0};
	
	ctx.nAction = REMOVE_TASK_BY_TASKID;
	ctx.pInputBuffer = (PVOID)nTaskID;
	Que_FindByCallBack(m_PrgTask, (FINDCALLBACK)PrgTaskCB, &ctx);
	
}

KSTATUS CBillHelper::UpdateUSStatus( ULONG nTaskID, PBOOL pFullTaskFinish )
{
	PRG_TASK_CONTEXT  ctx = {0};
	
	ctx.nAction      = UPDATE_US_STATUS_BY_TASKID;
	ctx.pInputBuffer = (PVOID)nTaskID;
	if( Que_FindByCallBack(m_PrgTask, (FINDCALLBACK)PrgTaskCB, &ctx) == FALSE )
		return STATUS_NOT_FOUND_VALUE;

	*pFullTaskFinish = (BOOL)ctx.pOutputBuffer;

	return STATUS_SUCCESS;

}


KSTATUS CBillHelper::UpdateUDCStatus( ULONG nTaskID, PBOOL pFullTaskFinish )
{
	PRG_TASK_CONTEXT  ctx = {0};
	
	ctx.nAction      = UPDATE_UDC_STATUS_BY_TASKID;
	ctx.pInputBuffer = (PVOID)nTaskID;
	if( Que_FindByCallBack(m_PrgTask, (FINDCALLBACK)PrgTaskCB, &ctx) == FALSE )
		return STATUS_NOT_FOUND_VALUE;
	
	*pFullTaskFinish = (BOOL)ctx.pOutputBuffer;
	
	return STATUS_SUCCESS;
}

#define  SAMPLE_10X_SESSION   "setpicrange"
#define  SAMPLE_10X_COUNT_KEY "NumberOfViews"
#define  SAMPLE_10X_INDEX_KEY "Viewindexs%u"
#define  SAMPLE_40X_NUMBER_KEY "40xPicCount"

#define  FULL_SAMPLE_10X_RANGE 24
#define  DEFAULT_SAMPLE_40X_RANGE 50

KSTATUS CBillHelper::GetSample10xInfo( PBILL3_EXTENSION pExtension )
{
	CHAR   cfg_path[MAX_PATH], KeyName[50];
	int    nIndex;
	USHORT nVaildCount = 0;

	GetModuleFileName(NULL, cfg_path, MAX_PATH);
	PathRemoveFileSpec(cfg_path);
//	PathAppend(cfg_path, "config");
	PathAppend(cfg_path, "conf.ini");

	pExtension->s10x_range.nSample10xCount = GetPrivateProfileInt(SAMPLE_10X_SESSION, SAMPLE_10X_COUNT_KEY, FULL_SAMPLE_10X_RANGE, cfg_path);
	pExtension->s10x_range.pSample10xIndexArray = (PUCHAR)malloc(sizeof(UCHAR) * pExtension->s10x_range.nSample10xCount);
	if( pExtension->s10x_range.pSample10xIndexArray == NULL	)
		return STATUS_ALLOC_MEMORY_ERROR;

	for( USHORT j = 0 ; j < pExtension->s10x_range.nSample10xCount ; j ++ )
	{
		sprintf_s(KeyName, SAMPLE_10X_INDEX_KEY, j);
		nIndex = GetPrivateProfileInt(SAMPLE_10X_SESSION, KeyName, -1, cfg_path);
		if( nIndex == -1 || nIndex >= FULL_SAMPLE_10X_RANGE )
			break;

		pExtension->s10x_range.pSample10xIndexArray[nVaildCount++] = (UCHAR)nIndex;
	}

	pExtension->prg_status.nTotalSamplingNum = GetPrivateProfileInt(SAMPLE_10X_SESSION, SAMPLE_40X_NUMBER_KEY, DEFAULT_SAMPLE_40X_RANGE, cfg_path);
	if( nVaildCount == 0 )
	{
		free(pExtension->s10x_range.pSample10xIndexArray);
		SetDefaultSample10xRange(pExtension);
	}
	else
		pExtension->s10x_range.nSample10xCount   = nVaildCount;

	Reloc10xPos(pExtension);

	return STATUS_SUCCESS;

}


void CBillHelper::Reloc10xPos(PBILL3_EXTENSION pExtension)
{
	UCHAR   t_idx[FULL_SAMPLE_10X_RANGE], i;
	UCHAR   nCol, nLine, nArrayIdx;


	memset(t_idx, 0xff, FULL_SAMPLE_10X_RANGE);

	
	for( i = 0; i < pExtension->s10x_range.nSample10xCount ; i ++ )
	{
		nArrayIdx = pExtension->s10x_range.pSample10xIndexArray[i];
		nCol      = nArrayIdx >> 3;
		if( nCol & 0x1 ) //奇数排(从序号0开始)
		{
			nLine = nArrayIdx & 0x7;
			nLine = 7 - nLine;
			nArrayIdx = (nCol << 3) +  nLine;
		}
		
		t_idx[nArrayIdx] = pExtension->s10x_range.pSample10xIndexArray[i];
	}

	pExtension->s10x_range.nSample10xCount = 0;

	for( i = 0 ; i < FULL_SAMPLE_10X_RANGE ; i ++ )
	{
		if( t_idx[i] == 0xff )
			continue;

		pExtension->s10x_range.pSample10xIndexArray[pExtension->s10x_range.nSample10xCount] = t_idx[i];
		pExtension->s10x_range.nSample10xCount ++;
	}
	
}


KSTATUS CBillHelper::SetDefaultSample10xRange( PBILL3_EXTENSION pExtension )
{
	pExtension->s10x_range.nSample10xCount = FULL_SAMPLE_10X_RANGE;

	pExtension->s10x_range.pSample10xIndexArray = (PUCHAR)malloc(sizeof(UCHAR) * pExtension->s10x_range.nSample10xCount);
	if( pExtension->s10x_range.pSample10xIndexArray == NULL	)
		return STATUS_ALLOC_MEMORY_ERROR;

	for( UCHAR j = 0 ; j < pExtension->s10x_range.nSample10xCount ; j ++ )
		pExtension->s10x_range.pSample10xIndexArray[j] = j;

	return STATUS_SUCCESS;
}



void CBillHelper::SetFailOverTask( BOOL bFail )
{
    InterlockedExchange(&m_FailOverUSTask, bFail);
}

BOOL CBillHelper::QueryTaskOverByFail()
{
	return   (BOOL)InterlockedExchangeAdd(&m_FailOverUSTask, 0);
}

BOOL CBillHelper::QueryThreadExit()
{
	return (BOOL)(InterlockedExchangeAdd(&m_ExitThread, 0) != 0);
}





