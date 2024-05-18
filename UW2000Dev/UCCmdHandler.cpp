#include "StdAfx.h"
#include "UCCmdHandler.h"
#include "TranslateCmd.h"
#include "WaitQueue.h"
#include "ProtMapping.h"
#include "interal.h"
#include "Global.h"
#include "lc_interface.h"
#include "THSPDef.h"
#include "ThLcProto_UW2000.h"


#define TIME_INTERVAL_ZERO				0
#define TIME_INTERVAL_1S				1*1000
#define TIME_INTERVAL_2S				2*1000
#define TIME_INTERVAL_3S				3*1000
#define TIME_INTERVAL_4S				4*1000
#define TIME_INTERVAL_5S				5*1000
#define TIME_INTERVAL_6S				3*1000
#define TIME_INTERVAL_7S				3*1000
#define TIME_INTERVAL_8S				3*1000
#define TIME_INTERVAL_9S				3*1000
#define TIME_INTERVAL_10S				10*1000
#define TIME_INTERVAL_20S				20*1000
#define TIME_INTERVAL_30S				30*1000
#define TIME_INTERVAL_60S				60*1000
#define TIME_INTERVAL_100S				100*1000
#define TIME_INTERVAL_240S				240*1000
#define TIME_INTERVAL_510S				510*1000
#define TIME_INTERVAL_1000S				1000*1000
#define INVALID_TIME_INTERVAL			0XFFFFFFFF


#define POSTCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, T) \
	{ \
		UCHAR CmdDataBuffer[1024] = { 0 }; \
		PCOMM_CMD_DATA CmdData = (PCOMM_CMD_DATA)CmdDataBuffer; \
		T* ctx = (T*)Command->Reserved; \
		 \
		CmdData->nCmd = (ProtocolCmd == INVALID_CMD ? Command->lc_header.nLCCmd : ProtocolCmd); \
		memcpy(CmdData->data, ctx, sizeof(T)); \
		 \
		return PostCmdToLC(ProgramObject, CmdData, COMM_CMD_LENGTH + sizeof(T)); \
	}

#define SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, T1, T2, bHaveCmdData, bWaitReturnData) \
	{ \
		UCHAR CmdDataBuffer[1024] = { 0 }; \
		PCOMM_CMD_DATA CmdData = (PCOMM_CMD_DATA)CmdDataBuffer; \
		CmdData->nCmd = (ProtocolCmd == INVALID_CMD ? Command->lc_header.nLCCmd : ProtocolCmd); \
		if(bHaveCmdData) \
		{ \
			T1* ctx = (T1*)Command->Reserved; \
			memcpy(CmdData->data, ctx, sizeof(T1)); \
		} \
		 \
		if(bWaitReturnData) \
		{ \
		  T2* pOutBuffer = (T2*)Command->Reserved;  \
		  return SendCmdToLC(ProgramObject, CmdData, COMM_CMD_LENGTH + (bHaveCmdData?sizeof(T1):0), (PCHAR)pOutBuffer, sizeof(T2), TimeoutInterval); \
		} \
		 \
		return SendCmdToLC(ProgramObject, CmdData, COMM_CMD_LENGTH + (bHaveCmdData?sizeof(T1):0), NULL, 0, TimeoutInterval); \
	}


typedef KSTATUS (*UCCmdHandler)(PROGRAM_OBJECT ProgramObject,  PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval);


struct UCProtocolCmdInfo
{
	UCHAR UCCmd;
	UCHAR UCProtocolCmd;
	UCCmdHandler UCCmdFunc; 
	DWORD TimeoutInterval;
};


void ClearTask()
{
	g_WaitQueue.Clear();
}

static KSTATUS JustPostCmdToLC(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	KSTATUS Status = STATUS_SUCCESS;
	COMM_CMD_DATA CmdData = { 0 };

	CmdData.nCmd = (ProtocolCmd == INVALID_CMD ? Command->lc_header.nLCCmd : ProtocolCmd);

	Status = PostCmdToLC(ProgramObject, &CmdData);
	CheckResultCode("PostCmdOnly", "JustPostCmdToLC", Status);

end:
	return Status;
}

static KSTATUS JustSendCmdToLC(PROGRAM_OBJECT PrgObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	COMM_CMD_DATA CmdData = { 0 };

	CmdData.nCmd = (ProtocolCmd == INVALID_CMD ? Command->lc_header.nLCCmd : ProtocolCmd);
	return SendCmdToLC(PrgObject, &CmdData, COMM_CMD_LENGTH, NULL, 0, TimeoutInterval);
}

static KSTATUS JustSendResetZToLC(PROGRAM_OBJECT PrgObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	UCHAR CmdDataBuffer[1024] = { 0 };
	PCOMM_CMD_DATA CmdData = (PCOMM_CMD_DATA)CmdDataBuffer;


	CmdData->nCmd = (ProtocolCmd == INVALID_CMD ? Command->lc_header.nLCCmd : ProtocolCmd);
	int* ctx = (int*)Command->Reserved;
	memcpy(CmdData->data, ctx, sizeof(int));
	return SendCmdToLCEx(PrgObject, CmdData, COMM_CMD_LENGTH, NULL, 0, TimeoutInterval);
}

static KSTATUS LCPowerfulClean(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{

	UCHAR CmdDataBuffer[1024] = { 0 }; 
	PCOMM_CMD_DATA CmdData = (PCOMM_CMD_DATA)CmdDataBuffer; 

	LONG* ctx = (LONG*)Command->Reserved;
	memcpy(CmdData->data,ctx,sizeof(LONG));
	TimeoutInterval *= (LONG)(*CmdData->data);
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, LONG, RESULT_0_OR_1_CONTEXT, true, false)

}

static KSTATUS HandshakeReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{

	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, HANDSHAKE_ACK_CONTEXT, false, true)
}

static KSTATUS ReadHardwareParamReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, HARDWARE_PARAM_CONTEXT, HARDWARE_PARAM_CONTEXT, true, true)
}

static KSTATUS ReadMotorConfReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, MOTOR_CONF_CONTEXT, MOTOR_CONF_CONTEXT, true, true)
}


static KSTATUS ReadBarCodeReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{ 
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, BARCODE_REQ_CONTEXT_DBG, BARCODE_REQ_CONTEXT_DBG, true, true)
}

static KSTATUS WriteHardwareParamReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, HARDWARE_PARAM_CONTEXT, RESULT_0_OR_1_CONTEXT, true, true)
}

static KSTATUS WriteMotorConfReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, MOTOR_CONF_CONTEXT, RESULT_0_OR_1_CONTEXT, true, true)
}

static KSTATUS WriteHardwareParamTestTypeReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, GETTESTINFO_CONTEXT, RESULT_0_OR_1_CONTEXT, true, true)
}

static KSTATUS WriteQCTestTypeReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, QCType, QCType, true, true)
}


static KSTATUS AllowTestAck(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	POSTCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, ALLOW_TEST_CONTEXT)
}

static KSTATUS MoveNextAreaReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, MOVE_NEXT_AREA_REQ_CONTEXT, char, true, false)
}

static KSTATUS MoveNewRowReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, MOVE_NEW_ROW_CONTEXT, MOVE_NEW_ROW_REQ, true, true)
}

static KSTATUS MoveNewRowEndReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, MOVE_NEW_ROW_REQ, MOVE_NEW_ROW_REQ, true, true)
}


static KSTATUS FocusStepReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, FOCUS_STEP_REQ_CONTEXT, char, true, false)
}

static KSTATUS MotorTestReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, MOTORTEST_REQ_CONTEXT, char, true, false)
}

static KSTATUS SolenoidTestReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, SOLENOID_VALVE_TEST_REQ_CONTEXT, char, true, false)
}

static KSTATUS ReadSensorCfgReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, SENSOR_CFG_CONTEXT, false, true)
}

static KSTATUS WriteSensorCfgReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, SENSOR_CFG_CONTEXT, RESULT_0_OR_1_CONTEXT, true, true)
}

static KSTATUS ChangeChannelReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, CHANNEL_CONTEXT, CHANNEL_CONTEXT, true, true)
}

static KSTATUS ChangeMicroscopeAreaReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, CAPTURE_IMAGE_AREA_CONTEXT, CAPTURE_IMAGE_AREA_CONTEXT, true, true)
}

static KSTATUS UdcStdPaperTestReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, UDC_STDPAPER_TEST_RESULT_CONTEXT, false, true)
}

static KSTATUS ReadHardwareVersionReq(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, HARDWARE_READ_VERSION_CONTEXT, false, true)
}

static KSTATUS AlarmProcessed(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	POSTCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, HARDWARE_ALARM_CONTEXT)
}
static KSTATUS DoGetTestInfo(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, GETTESTINFO_CONTEXT, false, true)
}

static KSTATUS DoGetGetZhuiGuangVal(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, DBG_GET_ZHUIGUANG_VAL_CONTEXT, false, true)
}


static KSTATUS SgCalibrate(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, PHYSICS_CALIBRATE_CONTEXT, SG_CALIBRATE_CONTEXT, true, true)
}
static KSTATUS TurbidityCalibrate(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, PHYSICS_CALIBRATE_CONTEXT, SG_CALIBRATE_CONTEXT, true, true)
}

static KSTATUS ColorCalibrate(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, COLOR_CALIBRATE_CONTEXT, false, true)
}

static KSTATUS ColorZuiGuang(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, COLOR_ZUIGUANG_CONTEXT, false, true)
}

static KSTATUS PhysicsTest(PROGRAM_OBJECT ProgramObject, PLC_COMMAND Command, UCHAR ProtocolCmd, DWORD TimeoutInterval)
{
	SENDCMDTOLC(ProgramObject, Command, ProtocolCmd, TimeoutInterval, char, PHYSICS_RESULT_CONTEXT, false, true)
}

static UCProtocolCmdInfo g_UCHandleCmdInfoList[] = 
{
	UC_PIPE_AUTORUN_REQ,					INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_2S,
	UC_PIPE_STOPRUN_REQ,					INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_2S,
	UC_HANDSHAKE_REQ,						INVALID_CMD,						HandshakeReq,					TIME_INTERVAL_5S,
	UC_UC_RESET,							INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_MAINTAIN_REQ,						INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_1000S,
	UC_MOTOR_CONF_READ_REQ,					INVALID_CMD,						ReadMotorConfReq,				TIME_INTERVAL_5S,
	UC_MOTOR_CONF_WRITE_REQ,				INVALID_CMD,						WriteMotorConfReq,				TIME_INTERVAL_5S,
	UC_HARDWARE_READ_PARAM_REQ,				INVALID_CMD,						ReadHardwareParamReq,			TIME_INTERVAL_5S,
	UC_HARDWARE_WRITE_PARAM_REQ,			INVALID_CMD,						WriteHardwareParamReq,			TIME_INTERVAL_5S,
	UC_SETTESTINFO_REQ,						INVALID_CMD,						WriteHardwareParamTestTypeReq,	TIME_INTERVAL_5S,
	UC_SETQC_REQ,							INVALID_CMD,						WriteQCTestTypeReq,				TIME_INTERVAL_5S,
	UC_HARDWARE_RESET_PARAM_REQ,			INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_MOTOR_CONF_RESET_REQ,				INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_ALLOW_TEST,							INVALID_CMD,						AllowTestAck,					TIME_INTERVAL_ZERO,
	UC_ALLOW_EMERGENCYTEST,					INVALID_CMD,						AllowTestAck,					TIME_INTERVAL_ZERO,
	UC_MOVE_NEXT_AREA_REQ,					INVALID_CMD,						MoveNextAreaReq,				TIME_INTERVAL_5S,
	UC_START_PIC_NEW_ROW,					INVALID_CMD,						MoveNewRowReq,					TIME_INTERVAL_5S,
	UC_END_PIC_NEW_ROW,						INVALID_CMD,						MoveNewRowEndReq,				TIME_INTERVAL_5S,
	UC_CAPTURE_IMAGE_FINISH,				INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_FOCUSING_REQ,						INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_FOCUS_STEP_REQ,						INVALID_CMD,						FocusStepReq,					TIME_INTERVAL_5S,
	UC_SAVE_FOCALLENGTH_REQ,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_2S,
	UC_EXITFOCUSING,						INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_ROTATE_NEXT_ANGLE_REQ,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_END_BARCODE_TURBIDITY,				INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_MOTORTEST_REQ,					INVALID_CMD,						MotorTestReq,					TIME_INTERVAL_20S,
	UC_DBG_SOLENOID_VALVE_TEST_REQ,			INVALID_CMD,						SolenoidTestReq,				TIME_INTERVAL_20S,
	UC_DBG_MIROSCOPE_LED_ON_REQ,			INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_MIROSCOPE_LED_OFF_REQ,			INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_BARCODE_LED_ON_REQ,				INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_BARCODE_LED_OFF_REQ,				INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_SENSOR_TEST_REQ,					INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_SENSOR_TEST_END,					INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_READ_SENSOR_CFG_REQ,				INVALID_CMD,						ReadSensorCfgReq,				TIME_INTERVAL_5S,
	UC_DBG_WRITE_SENSOR_CFG_REQ,			INVALID_CMD,						WriteSensorCfgReq,				TIME_INTERVAL_5S,
	UC_DBG_SYS_RESET_START,					INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_30S,
	UC_DBG_ASPIRATION_DEVICE_RESET_START,	INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_SAMPLE_TRASLATION_RESET_START,	INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_SAMPLE_UPDOWN_RESET_START,		INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_GOTO_CLEANPOSITION_START,		INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_GOTO_ASPIRATIONPOSITION_START,	INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_GOTO_EMERGENCYPOSITION_START,	INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_ASPIRATION_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_POINTSAMPLE_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_MIROSCOPE_RESET_START,			INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_X_AXIS_RESET_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_Y_AXIS_AB_CHANNEL_RESET_START,	INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_Y_AXIS_CD_CHANNEL_RESET_START,	INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_Z_AXIS_RESET_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_GOTO_10X_LENS_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
/*	UC_DBG_GOTO_40X_LENS_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,*/
	UC_DBG_PIPE_RESET_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_10S,
	UC_DBG_PIPE_IN_RESET_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_PIPE_OUT_RESET_START,			INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_STEP_LEFT_RESET_START,			INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_STEP_RIGHT_RESET_START,			INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_PIPE_IN_START,					INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_PIPE_OUT_START,					INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_ONE_STEP_START,					INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_CLEAN,							INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_AUTO_CLEAN,							INVALID_CMD,						LCPowerfulClean,				TIME_INTERVAL_510S,//做几次等待几分钟
	UC_AUTO_POWERFULCLEAN,					INVALID_CMD,						LCPowerfulClean,				TIME_INTERVAL_510S,//做几次等待几分钟
	UC_FOCUS_RESET_Z_ADJUST,				INVALID_CMD,						JustSendResetZToLC,				TIME_INTERVAL_10S,
	UC_DBG_STRENGCLEAN,						INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_BAOYANGCLEAN,					INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_TUBEROTATE_RESET,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_TOPOINTSAMPLE_INITPOS,			INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_SELECT_PAPER_RESET_START,		INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_FEED_PAPER_RESET_START,			INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_SELECT_PAPER_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_10S,
	UC_DBG_FEED_PAPER_TOPOINT_START,		INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_20S,
	UC_DBG_FEED_PAPER_ONESTEP_START,		INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_FEED_PAPER_FIRST_HALF_STEP_START,INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_FEED_PAPER_SECOND_HALF_STEP_START, INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_SELECT_PAPER_BALANCE_START,		INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_PUT_PAPER_BALANCE_START,			INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_SCANNING_BARCODE_START,			INVALID_CMD,						ReadBarCodeReq,					TIME_INTERVAL_5S,
	UC_DBG_EMPTY_PIPE_START,				INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_100S,
	UC_DBG_CHANGE_CHANNEL_START,			INVALID_CMD,						ChangeChannelReq,				TIME_INTERVAL_10S,
	UC_DBG_CHANGE_10X_LENS_AREA_START,		INVALID_CMD,						ChangeMicroscopeAreaReq,		TIME_INTERVAL_5S,
	UC_DBG_CHANGE_40X_LENS_AREA_START,		INVALID_CMD,						ChangeMicroscopeAreaReq,		TIME_INTERVAL_5S,
	UC_DBG_UDC_SEL_AND_FEED_PAPER_START,	INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_UDC_SEL_AND_FEED_PAPER_END_REQ,	INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_UDC_REPEATTEST_START,			INVALID_CMD,						JustPostCmdToLC,				TIME_INTERVAL_ZERO,
	UC_DBG_UDC_REPEATTEST_END_REQ,			INVALID_CMD,						JustSendCmdToLC,				TIME_INTERVAL_5S,
	UC_DBG_UDC_STDPAPER_TEST_REQ,			INVALID_CMD,						UdcStdPaperTestReq,				TIME_INTERVAL_60S*5,
	UC_HARDWARE_READ_VERSION_REQ,			INVALID_CMD,						ReadHardwareVersionReq,			TIME_INTERVAL_5S,
	UC_ALARM_PROCESSED,						INVALID_CMD,						AlarmProcessed,					TIME_INTERVAL_ZERO,
	UC_GETTESTINFO_REQ,						INVALID_CMD,						DoGetTestInfo,					TIME_INTERVAL_3S,
	UC_DBG_GET_ZHUIGUANG_VAL_START,			INVALID_CMD,						DoGetGetZhuiGuangVal,			TIME_INTERVAL_30S,

	UC_SG_CALIBRATE,                        INVALID_CMD,                        SgCalibrate,                    TIME_INTERVAL_60S,
	/*UC_TURBIDITY_CALIBRATE,               INVALID_CMD,                        TurbidityCalibrate,             TIME_INTERVAL_10S,
	UC_COLOR_CALIBRATE,                     INVALID_CMD,                        ColorCalibrate,                 TIME_INTERVAL_10S,
	UC_COLOR_WB,                            INVALID_CMD,                        ColorZuiGuang,                  TIME_INTERVAL_10S,
	UC_DBG_TEST_PHYSICS,                    INVALID_CMD,                        PhysicsTest,                    TIME_INTERVAL_10S,*/
	UC_DBG_BEGIN_SG_SENSOR_TEST,            INVALID_CMD,                        JustSendCmdToLC,                TIME_INTERVAL_1S,
	UC_DBG_OVER_SG_SENSOR_TEST,             INVALID_CMD,                        JustSendCmdToLC,                TIME_INTERVAL_1S,
	UC_DBG_BEGIN_SG_ONE_TEST,               INVALID_CMD,                        PhysicsTest,                    TIME_INTERVAL_60S,
};

static bool IsUCDirectHandleCmd(UCHAR UCCmd, DWORD &CmdIndex)
{
	for (CmdIndex = 0; CmdIndex < GetItemCount(g_UCHandleCmdInfoList); ++CmdIndex)
	{
		if (UCCmd == g_UCHandleCmdInfoList[CmdIndex].UCCmd)
		{
			return true;
		}
	}

	return false;
}

KSTATUS HandleUCCmd(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	DWORD CmdIndex = 0;
	ULONG nInSize = 0;
	KSTATUS Status = STATUS_SUCCESS;
	PLC_COMMAND lp_cmd;

	Status = FMMC_GetPRPParmeters(prp, (PVOID *)&lp_cmd, &nInSize, NULL, NULL);
	CheckResultCode("DispatchCmd", "FMMC_GetPRPParmeters", Status);
	
	DbgPrint("enter HandleUCCmd %#x\n", lp_cmd->lc_header.nLCCmd);

	if( nInSize != sizeof(LC_COMMAND) || lp_cmd->lc_header.flag != LC_FLAG )
	{
		DbgPrint( "if( nInSize != sizeof(LC_COMMAND) || lp_cmd->lc_header.flag != LC_FLAG )" );
		Status = STATUS_INVALID_PARAMETERS;
		goto end;
	}
	
	if ( IsUCDirectHandleCmd(lp_cmd->lc_header.nLCCmd, CmdIndex) )
	{
		Status = g_UCHandleCmdInfoList[CmdIndex].UCCmdFunc(PrgObject, lp_cmd, g_UCHandleCmdInfoList[CmdIndex].UCProtocolCmd,  g_UCHandleCmdInfoList[CmdIndex].TimeoutInterval);
	}
	else
	{
		DbgPrint("UC Cmd %#x is not handled!!!!!", lp_cmd->lc_header.nLCCmd);
		Status = STATUS_UNKNOW_ERROR;
	}
	
end:
	return Status;
}