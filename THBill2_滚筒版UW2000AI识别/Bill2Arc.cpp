#include "StdAfx.h"
#include "Bill2Arc.h"
#include "DbgMsg.h"
#include "ThLcProto_UW2000.h"
#include "THBarcodeRecDLL.h"

#pragma comment(lib, "THBarcodeRecDLL.lib")

////////////////////////////////////////////////////
#define  REPORT_PIPE_IN_INFO           0X1   //检测到排管推进
#define  REPORT_PIPE_OUT_INFO          0X2   //检测到排管推出
#define  REPORT_PIPE_IN_FAIL_INFO      0X3   //排管推进失败
#define  RETRY_PIPE_IN_FAIL            0X4   //排管推进重试
#define  REPORT_TEST_CARD_EMPTY        0X5   //无测试卡
#define  REPORT_TEST_CARD_DIRECTION    0X6   //测试卡方向错误
#define  RETRY_TEST_CARD_EMPTY         0X7   //重试无测试卡
#define  RETRY_TEST_CARD_DIRECTION     0X8   //测试卡方向错误重试
#define  REPORT_BD_FINISH              0X9   //标定完成
#define  REPORT_LC_OFFLINE             0XA   //下位机离线
#define  REPORT_PIPE_FULL              0XB   //排管满
#define  REPORT_CANCEL_PIPE_FULL       0XC   //解除排管满
//#define  REPORT_CLEANER_EMPTY	       0XD   //清洗液空
#define  REPORT_CANCEL_CLEANER_EMPTY   0XF   //解除清洗液空
#define  REPORT_DILUENT_EMPTY	       0X11  //稀释液空
#define  REPORT_CANCEL_DILUENT_EMPTY   0X12  //解除稀释液空
#define  REPORT_JS_EMPTY 		       0X13   //无计数版
#define  REPORT_JS_DIRECTION    	   0X14   //计数版方向错误
#define  RETRY_JS_EMPTY        		   0X15   //重试无计数版
#define  RETRY_JS_DIRECTION     	   0X16   //计数版方向错误重试
#define  RESPOND_CANCEL_CLEANER_EMPTY  0x17   //应答解除清洗液空
#define  RESPOND_DILUENT_EMPTY		   0x18   //应答稀释液空
#define  REPORT_SENSE_STATUS           0X19   //传感器状态
#define  REPORT_TESTCARD_BOX_ERROR     0X20   //测试卡盛放盒安装错误
#define  REPORT_COUNTPLATE_BOX_ERROR   0X21   //计数板盛放盒安装错误
#define  REPORT_LEANER_OUT_OF_DATE     0X22   //清洗液过期

#define  REPORT_CPU_CARD_TYPE_ERROR    0X23   //CPU卡片类型错误
#define  REPORT_BLANK_CARD			   0X24   //卡片是空白卡
#define  REPORT_CPU_CARD_PSW_ERROR     0X25   //卡片密码验证失败
#define  REPORT_CPU_CARD_READ_WRITE_ERROR     0X26   //读写卡片错误
#define  REPORT_LEANER_TYPE_ERROR      0X27   //清洗液类型错误
#define  REPORT_CPU_CARD_DATA_ERROR    0X28   //卡片数据错误
#define  REPORT_CPU_CARD_READ_SUCCESS  0X29   //读取成功正常

#define  REPORT_DAILY_LEANER_COUNT		0x30   //日常清洗液次数
#define  REPORT_STRENGTHEN_LEANER_COUNT 0x31   //强化清洗液次数
#define  REPORT_KEEP_LEANER_COUNT		0x32   //保养清洗液次数

#define RETRY_TESTCARD_BOX_ERROR		0x33   //重试测试卡盛放盒安装错误
#define RETRY_COUNTPLATE_BOX_ERROR		0x34   //重试计数板盛放盒安装错误
#define RETRY_CLEANER_EMPTY				0x35   //重试清洗液空 (舍弃)
#define RETRY_DILUENT_EMPTY				0x36   //重试稀释剂空

#define REPORT_DAILY_CLEANER_EMPTY	    0X37   //日常清洗液空
#define REPORT_STRENGTHEN_CLEANER_EMPTY	0X38   //强化清洗液空
#define REPORT_KEEP_CLEANER_EMPTY	    0X39   //维护清洗液空

#define RETRY_DAILY_CLEANER_EMPTY		0x3A   //重试日常清洗液空
#define RETRY_STRENGTHEN_CLEANER_EMPTY	0x3B   //重试强化清洗液空
#define RETRY_KEEP_CLEANER_EMPTY		0x3C   //重试维护清洗液空

#define REPORT_LC_RESET_FAILED			0x3D   //下位机复位失败
#define REPORT_TESTCARD_STUCK			0x3E   //测试卡卡住
#define RETRY_TESTCARD_STUCK			0x3F   //重试测试卡卡住
#define REPORT_JS_STUCK					0x40   //计数板卡住
#define RETRY_JS_STUCK					0x41   //重试计数板卡住
#define REPORT_WASTE_PAPER_BOX_FULL		0x42
#define RETRY_WASTE_PAPER_BOX_FULL		0x43
#define REPORT_NEW_PIPE					0X44
//#define REPORT_NEW_TASk				0X45
#define REPORT_FINISH_TEST_SELECT_CARD  0x46
#define REPORT_SELECT_CARD_RESULT		0x47
#define REPORT_SELECT_BOARD_RESULT		0x48
#define REPORT_FINISH_TEST_SELECT_BOARD 0x49
#define REPORT_FOCAL_LENGTH				0x50

#define REPORT_LC_FAILURE				0x51
#define REPORT_PAPER_STUCK				0x52
#define REPORT_NO_PAPER					0x53
#define REPORT_FINISH_SELF_CHECK		0x54

#define RETURN_BARCODE                  0x55


#define LC_FAILURE_INFO_COUNT 20
struct LC_FAILURE_INFO
{
	UCHAR FailureInfoList[LC_FAILURE_INFO_COUNT];
};
typedef LC_FAILURE_INFO *PLC_FAILURE_INFO;

//////////////////////////////////////////////////


KSTATUS CBill2Arc::StartBillReatliveDev( PROGRAM_OBJECT BillPrgObj )
{
	KSTATUS                   status;
	PBILL3_EXTENSION          pExtension;

	FMMC_GetPrgObjectExtension(BillPrgObj, (PVOID*)&pExtension);

	//启动帮助类
	status = InitializeStatusInformation(BillPrgObj);
	if (status != STATUS_SUCCESS)
	{
		DBG_MSG("Bill 启动MMgr帮助类 失败\n");
		return status;
	}
	DBG_MSG("Bill 启动MMgr帮助类 成功\n");
	
	//启动US处理类
	status = InitializeUSArc(BillPrgObj, (CBillHelper*)this);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("Bill 启动US处理类 失败\n");
		CloseAllDevice(pExtension);
		return status;
	}
	DBG_MSG("Bill 启动US处理类 成功\n");

	//启动UDC处理类
	//InitializeUDCArc(BillPrgObj, (CBillHelper*)this);

	//新uw2000下面驱动设备,必须成功启动us,否则新uw2000底层设备集将停止工作
	status = FindAppointDev(BillPrgObj, UW2000_NEW_NAME, T_DEV_LC_TYPE);   //下位机必须成功启动,否则uw2000将不能成功启动...
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("Bill UW2000下位机未启动\n");
		CloseAllDevice(pExtension);
		return status;
	}
	DBG_MSG("Bill UW2000下位机 启动成功\n");
// 	status = FindAppointDev(BillPrgObj, TH_UDC_2000_NAME, T_DEV_UDC_TYPE);   //UW2000尿干化设备必须成功启动,否则uw2000将不能成功启动
// 	if( status != STATUS_SUCCESS )
// 	{
// 		CloseAllDevice(pExtension);
// 		return status;
// 	}

	//status = FindAppointDev(BillPrgObj, CA_NAME, T_DEV_CAMERA_TYPE);
	if (FindAppointDev(BillPrgObj, CA_NAME, T_DEV_CAMERA_TYPE) != STATUS_SUCCESS)
	{
		DBG_MSG("Bill 相机设备未启动\n");
	}
	else
	{
		DBG_MSG("Bill 相机设备 启动成功\n");
	}
	status = FindAppointDev(BillPrgObj, TH_REC_CELL1_NAME, T_DEV_REC_TYPE);
	if (status != STATUS_SUCCESS)
	{
		DBG_MSG("Bill 细胞识别设备未启动\n");
	}
	else
	{
		DBG_MSG("Bill 细胞识别设备 启动成功\n");
	}
	//status = FindAppointDev(BillPrgObj, TH_LIS_NAME, T_DEV_LIS_TYPE);
	if (FindAppointDev(BillPrgObj, TH_LIS_NAME, T_DEV_LIS_TYPE) != STATUS_SUCCESS)
	{
		DBG_MSG("Bill LIS设备未启动\n");
	}
	else
	{
		DBG_MSG("Bill LIS设备 启动成功\n");
	}
	
	// 初始化条码识别
	//BarcodeRec_Initialize();

	return status;
}

KSTATUS CBill2Arc::FindAppointDev( PROGRAM_OBJECT BillPrgObj, PCHAR  pDevName, TDEVICE_DESCRITOR DevDes )
{
	KSTATUS          status;
	PROGRAM_OBJECT   StartPrgObj;
	
	status = FMMC_GetPrgObjectByName(pDevName, &StartPrgObj);
	if( status != STATUS_SUCCESS )
		return status;

	return StartHelperDevice(StartPrgObj, DevDes);
}

KSTATUS CBill2Arc::LCProtoDispatch( PROGRAM_OBJECT BillPrgObj, PLC_COMMAND lc_cmd )
{
	KSTATUS    status; 

	if( lc_cmd->lc_header.flag != LC_FLAG )
		return STATUS_INVAILD_DATA;

	switch( lc_cmd->lc_header.nLCCmd )
	{
	case LC_LC_RESET_STARTING:
	{
		SetFailOverTask(TRUE);
		status = AddStatusInformation(ThmeInformationType, LcReStarting, nullptr, 0, FALSE);
		break;
	}
	case LC_LC_RESET_END:
	{
		status = AddStatusInformation(ThmeInformationType, LcReStartEnd, lc_cmd->Reserved, sizeof(LC_RESET_END_CONTEXT));
		break;
	}
	case LC_DETECTED_PIPE_AND_IN:
	{
		status = AddStatusInformation(ThmeInformationType, DetectedPipeAndIn, nullptr, 0, FALSE);
		break;
	}
	case LC_PIPE_IN_SUCCESS:
	{
		status = AddStatusInformation(ThmeInformationType, PipeInSuccess, nullptr, 0, FALSE);
		break;
	}
	case LC_NEW_TASK:
	{
		SetFailOverTask(FALSE);
		status = AddStatusInformation(ThmeInformationType, LcNewTask, lc_cmd->Reserved, sizeof(NEW_TASK_CONTEXT));
		break;
	}
	case LC_TASK_ACTION_FINISH:
	{
		status = AddStatusInformation(ThmeInformationType, TaskActionFinish, nullptr, 0, FALSE);
		break;
	}
	case LC_PIPE_OUT_SHELF:
	{
		status = AddStatusInformation(ThmeInformationType, PipeOutShelf, nullptr, 0, FALSE);
		break;
	}
	case LC_EMERGENCY_TEST:
	{
		SetFailOverTask(FALSE);
		status = AddStatusInformation(ThmeInformationType, LcNewEmergencyTask, nullptr, 0, FALSE);
		break;
	}
	case LC_CAPTURE_IMAGE_REQ:
	{
		DBG_MSG("下位机通知计数池采图...\n");
		PCAPTURE_IMAGE_REQ_CONTEXT ctx = (PCAPTURE_IMAGE_REQ_CONTEXT)lc_cmd->Reserved;
		FINISH_STANDING_INFO info = { 0 };
		info.TaskID = ctx->TaskId;
		info.HoleNum = ctx->HoleNumber;
		info.ChannelNum = ctx->ChannelNumber;

		status = StartUSEvent(&info);

		// status = AddStatusInformation(ThmeInformationType, CaptureImageRequest, lc_cmd->Reserved, sizeof(CAPTURE_IMAGE_REQ_CONTEXT));
		break;
		//如果为调试模式则发送条码信息给调试界面
		if (FALSE)
		{
		}
	}
	case LC_BARCODE_REQ:
	{
		PBARCODE_REQ_CONTEXT ctx = (PBARCODE_REQ_CONTEXT)lc_cmd->Reserved;
		if (ctx->BarcodeLen < sizeof(ctx->Barcode))
		{
			ctx->Barcode[ctx->BarcodeLen] = 0;
		}
		DBG_MSG("下位机发送条码信息...[%d][%d][%s]\n", ctx->TaskId, ctx->BarcodeLen, ctx->Barcode);
		char buf[100] = { 0 }, temp[8];
		for (int i = 0; i < ctx->BarcodeLen; i++)
		{
			_snprintf_s(temp, sizeof(temp), "%02x ", (unsigned char)ctx->Barcode[i]);
			strcat_s(buf, sizeof(buf), temp);
		}
		DBG_MSG("%s\n", buf);
		//status = StartBarcodeEvent(ctx);
		status = AddStatusInformation(ThmeInformationType, BarcodeRequest, lc_cmd->Reserved, sizeof(BARCODE_REQ_CONTEXT));
		DBG_MSG("4.BILL2ARC:下位机发送条码信息:%s...\n", lc_cmd->Reserved);
		break;
	}
	case LC_UDC_RESULT:
	{
		status = AddStatusInformation(ThmeInformationType, UdcResult, lc_cmd->Reserved, sizeof(UDC_RESULT_CONTEXT));
		break;
	}
	case LC_PHYSICS_RESULT:
	{
		DBG_MSG("下位机发送物理三项结果");
		status = AddStatusInformation(ThmeInformationType, PhysicsResult, lc_cmd->Reserved, sizeof(PHYSICS_RESULT_TYPE));
		break;
	}
	case LC_DBG_TEST_SG_SENSOR_RETURN:
	{
		DBG_MSG("下位机发送物理比重传感器测试结果");
		status = AddStatusInformation(ThmeInformationType, PhysicsSgSensor, lc_cmd->Reserved, sizeof(PHYSICS_RESULT_TYPE));
		break;
	}
	case LC_DBG_SENSOR_TEST_DATA:
	{	
		status = AddStatusInformation(ThmeInformationType, SensorTestData, lc_cmd->Reserved, sizeof(SENSOR_TEST_DATA_CONTEXT));	
		DBG_MSG("5.BILL2ARC:下位机发送传感器信息:头0x%x，尾0x%x\n", lc_cmd->lc_header.nLCCmd, lc_cmd->Reserved);
		break;
	}
	case LC_DBG_UDC_SEL_AND_FEED_PAPER_DATA:
	{
		status = AddStatusInformation(ThmeInformationType, UdcSelAndFeedPaperTestData, lc_cmd->Reserved, sizeof(UDC_SEL_AND_FEED_PAPER_DATA_CONTEXT));
		break;
	}
	case LC_DBG_UDC_REPEATTEST_DATA:
	{
		status = AddStatusInformation(ThmeInformationType, UdcRepeatTestData, lc_cmd->Reserved, sizeof(UDC_REPEATTEST_DATA_CONTEXT));
		break;
	}
	case LC_ALARM:
	{
		status = AddStatusInformation(ThmeInformationType, LcAlarm, lc_cmd->Reserved, sizeof(HARDWARE_ALARM_CONTEXT));
		break;
	}
	case LC_PIC_NEW_ROW_HAVE_END:
	{
		//自己处理
		USStartNewRow((PMOVE_NEW_ROW_REQ)lc_cmd->Reserved);
		break;
	}
	/*case LC_NEW_TASK:   //吸样成功
		{
			PNEW_TASK   new_task = (PNEW_TASK)lc_cmd;

			DBG_MSG("检测到一个标本...\n");

			status = InsertNewTask(new_task->HoleNum, new_task->ChannelNum, new_task->IsSuccess);
			break;
		}
	case LC_START_SAMPLING:  //开始us流程
		{
			DBG_MSG("下位机通知尿沉渣采图...\n");

			status = StartUSEvent((PFINISH_STANDING_INFO)lc_cmd->Reserved); 
			break;
		}
	case LC_START_UDC_CHECK:  
		{
			PLC_START_UDC_CHECK_INFO    udc_info = (PLC_START_UDC_CHECK_INFO)lc_cmd;

			status = StartUDCEvent(udc_info->UDCCheckInfo.TaskID);
			break;
		}
	case LC_BARCODE:
		{
			PLC_BARCODE_INFO     lc_bc    = (PLC_BARCODE_INFO)lc_cmd;
			BARCODE_INFORMATION  bc_info = {0};

			strcpy(bc_info.BarCode, (PCHAR)lc_bc->BarcodeInfo.Barcode);
			bc_info.nTaskID = lc_bc->BarcodeInfo.TaskID;
			status = AddStatusInformation(ThmeInformationType, BarcodeEvent, &bc_info, sizeof(bc_info));
			break;
		}
	case LC_REPORT_ACTION:
		{
			status = ReportStatus((PLC_REPORT_INFORMATION)lc_cmd);
			break;
		}
	case LC_START_STATUS:
		{
			status = LCStartStatus();
			break;
		}
	case LC_CONSUMABLE_SURPLUS:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, QueryConsumables, NULL, 0);
			break;
		}*/
		
	default:
		{
			status = STATUS_INVALID_REQUEST;
		}
	}

	return status;
}

KSTATUS CBill2Arc::SetWorkStatus( PROGRAM_OBJECT BillPrgObj, PSYS_WORK_STATUS sys_work_status )
{
	KSTATUS                status = STATUS_INVALID_PARAMETERS;
	PBILL3_EXTENSION       pExtension = NULL;
	


	FMMC_GetPrgObjectExtension(BillPrgObj, (PVOID*)&pExtension);
	
	if( sys_work_status->nType == RespondSample )
	{
		PRESPOND_SAMPLE  res_sample = (PRESPOND_SAMPLE)sys_work_status->Reserved;

		DBG_MSG("Set New Task ID = %u, nHN = %u, type = %d\n", res_sample->nTaskID, res_sample->nHN, res_sample->type);
//		status = FillTaskID(res_sample);
//		if( status == STATUS_SUCCESS )
		status = SendTaskIDToLC(res_sample);
	}

	else if( sys_work_status->nType == USSampleCtrl )
	{
		PBOOL  pPause = (PBOOL)sys_work_status->Reserved;

		status = ControlUSSample(*pPause);
	}

	return status;
}

KSTATUS CBill2Arc::ReportStatus( PLC_REPORT_INFORMATION report_info )
{
	KSTATUS   status = STATUS_INVALID_REQUEST;

	switch(report_info->nInfomationType)
	{
	case REPORT_PAPER_STUCK:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, PaperStuck, NULL, 0);
			break;
		}
	case REPORT_PIPE_FULL:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, PipeFull, NULL, 0);
			break;
		}
	case REPORT_NO_PAPER:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, NoPaper, NULL, 0);
			break;
		}
	case REPORT_STRENGTHEN_CLEANER_EMPTY:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, StrengthenCleanerEmpty, NULL, 0);
			break;
		}
	case REPORT_DAILY_CLEANER_EMPTY:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, DailyCleanerEmpty, NULL, 0);
			break;
		}
	case REPORT_LC_FAILURE:
		{
			PLC_FAILURE_INFO   lc_fail = (PLC_FAILURE_INFO)report_info->Reserved;

			LC_FAILURE_INFORMATION LCFailureInfo = {0};
			memcpy( LCFailureInfo.FailureInfoList, lc_fail->FailureInfoList, sizeof(LCFailureInfo.FailureInfoList) );

			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, LCFailure, LCFailureInfo.FailureInfoList, sizeof(LCFailureInfo.FailureInfoList));
			break;
		}
	case REPORT_KEEP_CLEANER_EMPTY:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, KeepLeanerCount, NULL, 0);
			break;
	
		}

	case REPORT_PIPE_IN_INFO:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, PipeInEvent, NULL, 0);
			break;

		}
	case REPORT_PIPE_OUT_INFO:
		{
			status = STATUS_SUCCESS;
			AddStatusInformation(ThmeInformationType, PipeOutEvent, NULL, 0);
			break;

		}
		
	default:
		{
			break;
		}


	}

	return status;
}

KSTATUS CBill2Arc::LCStartStatus()
{
	SetFailOverTask(TRUE);
	AddStatusInformation(ThmeInformationType, LCStartProgress, NULL, 0);
	return STATUS_SUCCESS;
}

void CBill2Arc::CloseDev( PROGRAM_OBJECT BillPrgObj )
{
	PBILL3_EXTENSION    pExtension;

	FMMC_GetPrgObjectExtension(BillPrgObj, (PVOID*)&pExtension);

	//先停止业务层...不再让其与UI发生交互
	StopUSArc();

	StopRelativeDevice(pExtension->o_dev.s_lc_dev);
	StopRelativeDevice(pExtension->o_dev.s_udc_dev);
	StopRelativeDevice(pExtension->o_dev.s_ca_dev);
	StopRelativeDevice(pExtension->o_dev.s_lis_dev);
	StopRelativeDevice(pExtension->o_dev.s_rec_dev);
	StopRelativeDevice(pExtension->o_dev.s_thmgr_dev);	
}

VOID CBill2Arc::StopRelativeDevice( PROGRAM_OBJECT PrgObj )
{
	if( PrgObj == NULL )
		return;

	
	FMMC_EasySynchronousBuildPRP(PrgObj, NULL, PRP_AC_CLOSE, CLOSE_DEV_REQUEST, NULL, 0, NULL, 0, FALSE, FALSE);

	
}