#include "StdAfx.h"
#include "LCCmdHandler.h"
#include "TranslateCmd.h"
#include "WaitQueue.h"
#include "ProtMapping.h"
#include "THSPDef.h"
#include "Global.h"
#include "ThLcProto_UW2000.h"
#include <algorithm>
#include "utils//ByteUtil.h"


#define POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, T1, T2) \
	{ \
		LC_COMMAND Cmd = { 0 }; \
		T1* t = (T1*)&Cmd; \
		t->lc_header.flag = LC_FLAG; \
		t->lc_header.nLCCmd = (OriginalCmd == INVALID_CMD ? CmdData->nCmd : OriginalCmd); \
		T2* ctx = (T2*)CmdData->data; \
		memset((void*)&(t->CmdCtx), 0, sizeof(t->CmdCtx)); \
		memcpy(&(t->CmdCtx), ctx, DataLength); \
		 \
		return PostCmdToUC(PrgObject, &Cmd); \
	}

WaitQueue g_WaitQueue;
//TaskInfoList g_TaskInfoList;
CRITICAL_SECTION g_CS;

typedef KSTATUS (*LCCmdHandler)(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd);

struct ResponseCmdPair
{
	UCHAR LCCmd;
	UCHAR UCOriginCmd;
	LCCmdHandler LCCmdFunc;
};

static KSTATUS ResponseUCCmd(UCHAR OriginalCmd)
{
	if( g_WaitQueue.SetWaitSuccess(OriginalCmd) )
		return STATUS_SUCCESS;
	
	return STATUS_UNKNOW_ERROR;
}

static KSTATUS JustResponseUCCmd(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	return ResponseUCCmd(OriginalCmd == INVALID_CMD ? CmdData->nCmd : OriginalCmd);
}

static KSTATUS JustPostCmdToUC(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	LC_COMMAND Cmd = { 0 };
	Cmd.lc_header.flag = LC_FLAG;
	Cmd.lc_header.nLCCmd = (OriginalCmd == INVALID_CMD ? CmdData->nCmd : OriginalCmd);

	return PostCmdToUC(PrgObject, &Cmd);
}

static KSTATUS ReturnDataToUC(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	CHAR *OutBuffer = NULL;
	int OutBufferLength = 0;

	if (!g_WaitQueue.GetCmdBuffer(OriginalCmd, &OutBuffer, OutBufferLength))
	{
		DbgPrint("retrieve buffer from queue failed!");
		return STATUS_UNKNOW_ERROR;
	}

	ZeroMemory(OutBuffer, OutBufferLength);
	memcpy(OutBuffer, CmdData->data, DataLength);

	return ResponseUCCmd(OriginalCmd);
}

static KSTATUS LCRestartEnd(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	ClearTask();
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, LC_RESET_END_TYPE, LC_RESET_END_CONTEXT)
}

static KSTATUS NewTestTask(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, NEW_TASK_TYPE, NEW_TASK_CONTEXT)
}

static KSTATUS PicRowHaveEnd(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, MOVE_NEW_ROW_REQ_EX, MOVE_NEW_ROW_REQ)
}

static KSTATUS CountBoardCaptureImage(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, CAPTURE_IMAGE_REQ_TYPE, CAPTURE_IMAGE_REQ_CONTEXT)
}

static KSTATUS BarcodeReq(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	PBARCODE_REQ_CONTEXT ctx = (PBARCODE_REQ_CONTEXT)CmdData->data;
	ctx->TaskId = SwitchByte(ctx->TaskId);
	char buf[100] = { 0 }, temp[8];
	strcat_s(buf, sizeof(buf), "下位机发送条码信息:"); 
	for (int i = 0; i < ctx->BarcodeLen + 1; i++)
	{
		_snprintf_s(temp, sizeof(temp), "%02x ", (unsigned char)ctx->Barcode[i]);
		strcat_s(buf, sizeof(buf), temp);
	}
	DBG_MSG("%s\n", buf);
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, BARCODE_REQ_TYPE, BARCODE_REQ_CONTEXT)
}

static KSTATUS UDCResult(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, UDC_RESULT_TYPE, UDC_RESULT_CONTEXT)
}

static KSTATUS SensorTestData(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
#if (_LC_V <= 2150)
	PSENSOR_TEST_DATA_CONTEXT ctx = (PSENSOR_TEST_DATA_CONTEXT)CmdData->data;
	ctx->CleanLiquidSensor = SwitchByte(ctx->CleanLiquidSensor);
	ctx->TubeExistSensor = SwitchByte(ctx->TubeExistSensor);
	ctx->TubeShelfExistSensor = SwitchByte(ctx->TubeShelfExistSensor);
	ctx->WastePaperBoxSensor = SwitchByte(ctx->WastePaperBoxSensor);
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, SENSOR_TEST_RESULT_TYPE, SENSOR_TEST_DATA_CONTEXT)
#elif (_LC_V >= 2260)
	PSENSOR_TEST_DATA_CONTEXT ctx = (PSENSOR_TEST_DATA_CONTEXT)CmdData->data;
	ctx->CleanLiquidSensor = SwitchByte(ctx->CleanLiquidSensor);
	ctx->WastePaperBoxSensor = SwitchByte(ctx->WastePaperBoxSensor);
	ctx->PaperFilpSensor = SwitchByte(ctx->PaperFilpSensor);
	ctx->TubeShelfExistSensor = SwitchByte(ctx->TubeShelfExistSensor);
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, SENSOR_TEST_RESULT_TYPE, SENSOR_TEST_DATA_CONTEXT)
#elif (_LC_V >= 2180)
	PSENSOR_TEST_DATA_CONTEXT ctx = (PSENSOR_TEST_DATA_CONTEXT)CmdData->data;
	ctx->CleanLiquidSensor = SwitchByte(ctx->CleanLiquidSensor);
	ctx->TubeExistSensor = SwitchByte(ctx->TubeExistSensor);
	ctx->TubeShelfExistSensor = SwitchByte(ctx->TubeShelfExistSensor);
	ctx->WastePaperBoxSensor = SwitchByte(ctx->WastePaperBoxSensor);
	ctx->PaperFilpSensor = SwitchByte(ctx->PaperFilpSensor);
	ctx->SuctionSampleBoxSensor = SwitchByte(ctx->SuctionSampleBoxSensor);
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, SENSOR_TEST_RESULT_TYPE, SENSOR_TEST_DATA_CONTEXT)
#else
	PSENSOR_TEST_DATA_CONTEXT ctx = (PSENSOR_TEST_DATA_CONTEXT)CmdData->data;
	ctx->CleanLiquidSensor = SwitchByte(ctx->CleanLiquidSensor);
	ctx->TubeExistSensor = SwitchByte(ctx->TubeExistSensor);
	ctx->TubeShelfExistSensor = SwitchByte(ctx->TubeShelfExistSensor);
	ctx->WastePaperBoxSensor = SwitchByte(ctx->WastePaperBoxSensor);
	ctx->PaperFilpSensor = SwitchByte(ctx->PaperFilpSensor);
	ctx->SuctionSampleBoxSensor = SwitchByte(ctx->SuctionSampleBoxSensor);
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, SENSOR_TEST_RESULT_TYPE, SENSOR_TEST_DATA_CONTEXT)
#endif // LC_V1_5
}

static KSTATUS UDCFeedPaperTestData(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	PUDC_SEL_AND_FEED_PAPER_DATA_CONTEXT ctx = (PUDC_SEL_AND_FEED_PAPER_DATA_CONTEXT)CmdData->data;
	ctx->CurTestCount = SwitchByte(ctx->CurTestCount);
	ctx->StuckPaperCount = SwitchByte(ctx->StuckPaperCount);
	ctx->DropPaperCount = SwitchByte(ctx->DropPaperCount);
	ctx->FlipPaperCount = SwitchByte(ctx->FlipPaperCount);
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, UDC_SEL_AND_FEED_PAPER_DATA_TYPE, UDC_SEL_AND_FEED_PAPER_DATA_CONTEXT)
}

static KSTATUS PhysicsResult(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, PHYSICS_RESULT_TYPE, PHYSICS_RESULT_CONTEXT)
}

static KSTATUS SgSensorResult(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, SG_SENSOR_TYPE, SG_SENSOR_CONTEXT)
}

static KSTATUS UDCRepeatTestData(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	PUDC_REPEATTEST_DATA_CONTEXT ctx = (PUDC_REPEATTEST_DATA_CONTEXT)CmdData->data;
	ctx->UdcColorBlockCtx.ColorBlock1_LEU = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock1_LEU);
	ctx->UdcColorBlockCtx.ColorBlock2_NIT = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock2_NIT);
	ctx->UdcColorBlockCtx.ColorBlock3_URO = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock3_URO);
	ctx->UdcColorBlockCtx.ColorBlock4_PRO = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock4_PRO);
	ctx->UdcColorBlockCtx.ColorBlock5_PH = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock5_PH);
	ctx->UdcColorBlockCtx.ColorBlock6_BLD = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock6_BLD);
	ctx->UdcColorBlockCtx.ColorBlock7_SG = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock7_SG);
	ctx->UdcColorBlockCtx.ColorBlock8_KET = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock8_KET);
	ctx->UdcColorBlockCtx.ColorBlock9_BIL = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock9_BIL);
	ctx->UdcColorBlockCtx.ColorBlock10_GLU = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock10_GLU);
	ctx->UdcColorBlockCtx.ColorBlock11_VC = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock11_VC);
	ctx->UdcColorBlockCtx.ColorBlock12_MALB = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock12_MALB);
	ctx->UdcColorBlockCtx.ColorBlock13_CRE = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock13_CRE);
	ctx->UdcColorBlockCtx.ColorBlock14_CA = SwitchByte(ctx->UdcColorBlockCtx.ColorBlock14_CA);
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, UDC_REPEATTEST_DATA_TYPE, UDC_REPEATTEST_DATA_CONTEXT)
}

static KSTATUS LCAlarm(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, HARDWARE_ALARM_TYPE, HARDWARE_ALARM_CONTEXT)
}



static KSTATUS PhysicsTurbidityCalibrate(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, SG_CALIBRATE_TYPE, SG_CALIBRATE_CONTEXT)
}
static KSTATUS PhysicsColorCalibrate(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, COLOR_CALIBRATE_TYPE, COLOR_CALIBRATE_CONTEXT)
}
static KSTATUS PhysicsColorWb(PROGRAM_OBJECT  PrgObject, PCOMM_CMD_DATA CmdData, DWORD DataLength, UCHAR OriginalCmd)
{
	POSTCMDTOUC(PrgObject, CmdData, DataLength, OriginalCmd, COLOR_ZUIGUANG_TYPE, COLOR_ZUIGUANG_CONTEXT)
}


static ResponseCmdPair g_ResponseCmdList[] =
{
	LC_PIPE_AUTORUN_ACK,						UC_PIPE_AUTORUN_REQ,					JustResponseUCCmd,
	LC_PIPE_STOPRUN_ACK,						UC_PIPE_STOPRUN_REQ,					JustResponseUCCmd,
	LC_HANDSHAKE_ACK,							UC_HANDSHAKE_REQ,						ReturnDataToUC,
	LC_LC_RESET_STARTING,						INVALID_CMD,							JustPostCmdToUC,
	LC_LC_RESET_END,							INVALID_CMD,							LCRestartEnd,
	LC_MAINTAIN_ACK,							UC_MAINTAIN_REQ,						JustResponseUCCmd,
	LC_MOTOR_CONF_READ_ACK,						UC_MOTOR_CONF_READ_REQ,					ReturnDataToUC,
	LC_HARDWARE_READ_PARAM_ACK,					UC_HARDWARE_READ_PARAM_REQ,				ReturnDataToUC,
	LC_HARDWARE_WRITE_PARAM_ACK,				UC_HARDWARE_WRITE_PARAM_REQ,			ReturnDataToUC,
	LC_MOTOR_CONF_WRITE_ACK,					UC_MOTOR_CONF_WRITE_REQ,				ReturnDataToUC,
	LC_SETTESTINFO_ACK,							UC_SETTESTINFO_REQ,						JustResponseUCCmd,
	LC_SETQC_ACK,								UC_SETQC_REQ,							ReturnDataToUC,
	LC_HARDWARE_RESET_PARAM_ACK,				UC_HARDWARE_RESET_PARAM_REQ,			JustResponseUCCmd,
	LC_MOTOR_CONF_RESET_ACK,					UC_MOTOR_CONF_RESET_REQ,				JustResponseUCCmd,
	LC_DETECTED_PIPE_AND_IN,					INVALID_CMD,							JustPostCmdToUC,
	LC_PIPE_IN_SUCCESS,							INVALID_CMD,							JustPostCmdToUC,
	LC_NEW_TASK,								INVALID_CMD,							NewTestTask,
	LC_TASK_ACTION_FINISH,						INVALID_CMD,							JustPostCmdToUC,
	LC_PIPE_OUT_SHELF,							INVALID_CMD,							JustPostCmdToUC,
	LC_EMERGENCY_TEST,							INVALID_CMD,							JustPostCmdToUC,
	LC_PIC_NEW_ROW_HAVE_END,					INVALID_CMD,							PicRowHaveEnd,
	LC_CAPTURE_IMAGE_REQ,						INVALID_CMD,							CountBoardCaptureImage,
	LC_MOVE_AREA_ACK,							UC_MOVE_NEXT_AREA_REQ,					JustResponseUCCmd,
	LC_START_PIC_NEW_ROW,						UC_START_PIC_NEW_ROW,					ReturnDataToUC,
	LC_END_PIC_NEW_ROW,							UC_END_PIC_NEW_ROW,						ReturnDataToUC,
	LC_FOCUSING_ACK,							UC_FOCUSING_REQ,						JustResponseUCCmd,
	LC_FOCUS_STEP_ACK,							UC_FOCUS_STEP_REQ,						JustResponseUCCmd,
	LC_SAVE_FOCALLENGTH_ACK,					UC_SAVE_FOCALLENGTH_REQ,				JustResponseUCCmd,
	LC_BARCODE_REQ,								INVALID_CMD,							BarcodeReq,
	LC_ROTATE_NEXT_ANGLE_ACK,					UC_ROTATE_NEXT_ANGLE_REQ,				JustResponseUCCmd,
	LC_UDC_RESULT,								INVALID_CMD,							UDCResult,
	LC_DBG_MOTORTEST_ACK,						UC_DBG_MOTORTEST_REQ,					JustResponseUCCmd,
	LC_DBG_SOLENOID_VALVE_TEST_ACK,				UC_DBG_SOLENOID_VALVE_TEST_REQ,			JustResponseUCCmd,
	LC_DBG_SENSOR_TEST_DATA,					INVALID_CMD,							SensorTestData,
	LC_DBG_READ_SENSOR_CFG_ACK,					UC_DBG_READ_SENSOR_CFG_REQ,				ReturnDataToUC,
	LC_DBG_WRITE_SENSOR_CFG_ACK,				UC_DBG_WRITE_SENSOR_CFG_REQ,			ReturnDataToUC,
	LC_DBG_SYS_RESET_END,						UC_DBG_SYS_RESET_START,					JustResponseUCCmd,
	LC_DBG_ASPIRATION_DEVICE_RESET_END,			UC_DBG_ASPIRATION_DEVICE_RESET_START,	JustResponseUCCmd,
	LC_DBG_SAMPLE_TRASLATION_RESET_END,			UC_DBG_SAMPLE_TRASLATION_RESET_START,	JustResponseUCCmd,
	LC_DBG_SAMPLE_UPDOWN_RESET_END,				UC_DBG_SAMPLE_UPDOWN_RESET_START,		JustResponseUCCmd,
	LC_DBG_GOTO_CLEANPOSITION_END,				UC_DBG_GOTO_CLEANPOSITION_START,		JustResponseUCCmd,
	LC_DBG_GOTO_ASPIRATIONPOSITION_END,			UC_DBG_GOTO_ASPIRATIONPOSITION_START,	JustResponseUCCmd,
	LC_DBG_GOTO_EMERGENCYPOSITION_END,			UC_DBG_GOTO_EMERGENCYPOSITION_START,	JustResponseUCCmd,
	LC_DBG_ASPIRATION_END,						UC_DBG_ASPIRATION_START,				JustResponseUCCmd,
	LC_DBG_POINTSAMPLE_END,						UC_DBG_POINTSAMPLE_START,				JustResponseUCCmd,
	LC_DBG_MIROSCOPE_RESET_END,					UC_DBG_MIROSCOPE_RESET_START,			JustResponseUCCmd,
	LC_DBG_X_AXIS_RESET_END,					UC_DBG_X_AXIS_RESET_START,				JustResponseUCCmd,
	LC_DBG_Y_AXIS_AB_CHANNEL_RESET_END,			UC_DBG_Y_AXIS_AB_CHANNEL_RESET_START,	JustResponseUCCmd,
	LC_DBG_Y_AXIS_CD_CHANNEL_RESET_END,			UC_DBG_Y_AXIS_CD_CHANNEL_RESET_START,	JustResponseUCCmd,
	LC_DBG_Z_AXIS_RESET_END,					UC_DBG_Z_AXIS_RESET_START,				JustResponseUCCmd,
	LC_DBG_GOTO_10X_LENS_END,					UC_DBG_GOTO_10X_LENS_START,				JustResponseUCCmd,
/*	LC_DBG_GOTO_40X_LENS_END,					UC_DBG_GOTO_40X_LENS_START,				JustResponseUCCmd,*/
	LC_DBG_PIPE_RESET_END,						UC_DBG_PIPE_RESET_START,				JustResponseUCCmd,
	LC_DBG_PIPE_IN_RESET_END,					UC_DBG_PIPE_IN_RESET_START,				JustResponseUCCmd,
	LC_DBG_PIPE_OUT_RESET_END,					UC_DBG_PIPE_OUT_RESET_START,			JustResponseUCCmd,
	LC_DBG_STEP_LEFT_RESET_END,					UC_DBG_STEP_LEFT_RESET_START,			JustResponseUCCmd,
	LC_DBG_STEP_RIGHT_RESET_END,				UC_DBG_STEP_RIGHT_RESET_START,			JustResponseUCCmd,
	LC_DBG_PIPE_IN_END,							UC_DBG_PIPE_IN_START,					JustResponseUCCmd,
	LC_DBG_PIPE_OUT_END,						UC_DBG_PIPE_OUT_START,					JustResponseUCCmd,
	LC_DBG_ONE_STEP_END,						UC_DBG_ONE_STEP_START,					JustResponseUCCmd,
	LC_DBG_CLEAN_END,							UC_DBG_CLEAN,							JustResponseUCCmd,
    LC_AUTO_CLEAN_END       ,					UC_AUTO_CLEAN,					        JustResponseUCCmd,
	LC_AUTO_POWERFULCLEAN_END,					UC_AUTO_POWERFULCLEAN,					JustResponseUCCmd,
	LC_FOCUS_RESET_Z_ADJUST_END,				UC_FOCUS_RESET_Z_ADJUST,				JustResponseUCCmd,
	LC_DBG_STRENGCLEAN_END,						UC_DBG_STRENGCLEAN,						JustResponseUCCmd,
	LC_DBG_BAOYANGCLEAN_END,					UC_DBG_BAOYANGCLEAN,					JustResponseUCCmd,
	LC_DBG_TUBEROTATE_RESET_END,				UC_DBG_TUBEROTATE_RESET,				JustResponseUCCmd,
	LC_DBG_TOPOINTSAMPLE_INITPOS_END,			UC_DBG_TOPOINTSAMPLE_INITPOS,			JustResponseUCCmd,
	LC_DBG_SELECT_PAPER_RESET_END,				UC_DBG_SELECT_PAPER_RESET_START,		JustResponseUCCmd,
	LC_DBG_FEED_PAPER_RESET_END,				UC_DBG_FEED_PAPER_RESET_START,			JustResponseUCCmd,
	LC_DBG_SELECT_PAPER_END,					UC_DBG_SELECT_PAPER_START,				JustResponseUCCmd,
	LC_DBG_FEED_PAPER_TOPOINT_END,				UC_DBG_FEED_PAPER_TOPOINT_START,		JustResponseUCCmd,
	LC_DBG_FEED_PAPER_ONESTEP_END,				UC_DBG_FEED_PAPER_ONESTEP_START,		JustResponseUCCmd,
	LC_DBG_FEED_PAPER_FIRST_HALF_STEP_END,		UC_DBG_FEED_PAPER_FIRST_HALF_STEP_START,JustResponseUCCmd,
	LC_DBG_FEED_PAPER_SECOND_HALF_STEP_END,		UC_DBG_FEED_PAPER_SECOND_HALF_STEP_START,JustResponseUCCmd,
	LC_DBG_SELECT_PAPER_BALANCE_END,			UC_DBG_SELECT_PAPER_BALANCE_START,		JustResponseUCCmd,
	LC_DBG_PUT_PAPER_BALANCE_END,				UC_DBG_PUT_PAPER_BALANCE_START,			JustResponseUCCmd,
	LC_DBG_EMPTY_PIPE_END,						UC_DBG_EMPTY_PIPE_START,				JustResponseUCCmd,
	LC_DBG_SCANNING_BARCODE_END,				UC_DBG_SCANNING_BARCODE_START,			ReturnDataToUC,
	LC_DBG_CHANGE_CHANNEL_END,					UC_DBG_CHANGE_CHANNEL_START,			ReturnDataToUC,
	LC_DBG_CHANGE_10X_LENS_AREA_END,			UC_DBG_CHANGE_10X_LENS_AREA_START,		ReturnDataToUC,
	LC_DBG_CHANGE_40X_LENS_AREA_END,			UC_DBG_CHANGE_40X_LENS_AREA_START,		ReturnDataToUC,
	LC_DBG_UDC_SEL_AND_FEED_PAPER_DATA,			INVALID_CMD,							UDCFeedPaperTestData,
	LC_DBG_UDC_SEL_AND_FEED_PAPER_END_ACK,		UC_DBG_UDC_SEL_AND_FEED_PAPER_END_REQ,	JustResponseUCCmd,
	LC_DBG_UDC_REPEATTEST_DATA,					INVALID_CMD,							UDCRepeatTestData,
	LC_DBG_UDC_REPEATTEST_END_ACK,				UC_DBG_UDC_REPEATTEST_END_REQ,			JustResponseUCCmd,//尿干化重复性测试结果反馈
	LC_DBG_UDC_STDPAPER_TEST_ACK,				UC_DBG_UDC_STDPAPER_TEST_REQ,			ReturnDataToUC,
	LC_HARDWARE_READ_VERSION_ACK,				UC_HARDWARE_READ_VERSION_REQ,			ReturnDataToUC,
	LC_ALARM,									INVALID_CMD,							LCAlarm,
	LC_GETTESTINFO_ACK,							UC_GETTESTINFO_REQ,						ReturnDataToUC,
	LC_DBG_GET_ZHUIGUANG_VAL_ACK,				UC_DBG_GET_ZHUIGUANG_VAL_START,			ReturnDataToUC,
	LC_PHYSICS_RESULT,                          INVALID_CMD,                            PhysicsResult,
	LC_SG_CALIBRATE_RETURN,                     UC_SG_CALIBRATE,                        ReturnDataToUC,
	/*LC_TURBIDITY_CALIBRATE_RETURN,              UC_TURBIDITY_CALIBRATE,                 PhysicsTurbidityCalibrate,
	LC_COLOR_CALIBRATE_RETURN,                  UC_COLOR_CALIBRATE,                     PhysicsColorCalibrate,
	LC_COLOR_WB_RETURN,                         UC_COLOR_WB,                            PhysicsColorWb,*/
	LC_DBG_TEST_SG_SENSOR_RETURN,               INVALID_CMD,                            SgSensorResult,

	LC_DBG_SG_ONE_TEST_RETURN,                  UC_DBG_BEGIN_SG_ONE_TEST,               ReturnDataToUC,
	LC_DBG_OVER_SG_ONE_TEST,                    INVALID_CMD,                            JustPostCmdToUC,
};

template<class T>
bool IsLCCmdInQueue(T *Queue, DWORD ItemCount, DWORD Cmd, DWORD &CmdIndex)
{
	for ( CmdIndex = 0; CmdIndex < ItemCount; ++CmdIndex)
	{
		if (Queue[CmdIndex].LCCmd == Cmd)
		{
			return true;
		}
	}
	
	return false;
}

static bool IsResponseCmd(DWORD Cmd, DWORD &CmdIndex)
{
	return IsLCCmdInQueue(g_ResponseCmdList, GetItemCount(g_ResponseCmdList), Cmd, CmdIndex);
}

KSTATUS HandleLCCmd(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	KSTATUS              Status = STATUS_SUCCESS;
	PCOMM_CMD_DATA		 CmdData = NULL;
	PRCV_BUFFER_PROC     rcv_buff;
	ULONG                nInSize;
	DWORD				 CmdIndex = 0;
	DWORD				 DataLengthInCmd = 0;
	
	DbgPrint("Enter HandleLCCmd\n");
	
	Status = FMMC_GetPRPParmeters(prp, (PVOID*)&rcv_buff, &nInSize, NULL, NULL);
	CheckResultCode("HandleLCCmd", "FMMC_GetPRPParmeters", Status);
	
	if( rcv_buff->bParsePro == FALSE || rcv_buff->nRcvSize < COMM_CMD_LENGTH )
	{
		DbgPrint( "rcv_buff->bParsePro == FALSE || rcv_buff->nRcvSize < DBFJ_CMD_LENGTH" );
		Status = STATUS_INVALID_PARAMETERS;
		goto end;
	}
	
	DataLengthInCmd = rcv_buff->nRcvSize - 1;
	CmdData = (PCOMM_CMD_DATA)rcv_buff->pRcvBuffer;
	DbgPrint("receive lc cmd: 0x%X\n", CmdData->nCmd);
	
	if ( IsResponseCmd(CmdData->nCmd, CmdIndex) )
	{
		Status = g_ResponseCmdList[CmdIndex].LCCmdFunc(PrgObject, CmdData, DataLengthInCmd, g_ResponseCmdList[CmdIndex].UCOriginCmd);
	}
	else
	{
		DbgPrint("unhandled cmd:0x%X!!!!!", CmdData->nCmd);
		Status = STATUS_UNKNOW_ERROR;
	}

end:
	DbgPrint("leave HandleLCCmd\n");
	return Status;
}
