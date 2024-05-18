// CmdHelper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DbgMsg.h"
#include "FileHelper.h"
#include "..\..\inc\CmdHelper.h"
#include "..\UW2000Dev\ProtMapping.h"
#include "..\..\inc\ThLcProto_UW2000.h"
#include "..\..\inc\THInterface.h"
#include "..\UW2000UDCDev\ProtMapping.h"
#include <shlwapi.h>
#include <strsafe.h>
#include "utils/ByteUtil.h"

#pragma comment(lib, "..\\..\\lib\\THInterface.lib")

 
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

//如果IsUDCCmd为true，则Cmd, pOutBuffer和nOutLength无意义

static bool SendCmdToLC(DWORD Cmd, PCHAR pInBuffer = NULL, ULONG nInLength = 0, PCHAR pOutBuffer = NULL, ULONG nOutLength = 0, bool IsUDCCmd = false)
{
	KSTATUS Result = STATUS_SUCCESS;
	TDEVICE_DESCRITOR DeviceDescritor = T_DEV_LC_TYPE;
	
	DBG_MSG("enter SendCmdToLC with cmd: 0x%x, datalength:%d\n", Cmd, nInLength);
	
	if (IsUDCCmd)
	{
		DeviceDescritor = T_DEV_UDC_TYPE;
	}

	Result = THInterface_SendlCCmdActionEx(Cmd, pInBuffer, nInLength, pOutBuffer, nOutLength, DeviceDescritor);
	if(STATUS_SUCCESS != Result)
	{
		DBG_MSG("send cmd %#x to lc failed with error = 0x%x\n", Cmd, Result);
		return false;
	}
	
	return true;
}

extern "C" bool WINAPI PipeBeginAutoRun()
{
	return SendCmdToLC(UC_PIPE_AUTORUN_REQ);
}

extern "C" bool WINAPI PipeStopAutoRun()
{
	return SendCmdToLC(UC_PIPE_STOPRUN_REQ);
}

extern "C" bool WINAPI HandshakeReq(HANDSHAKE_SYS_STATUS &status)
{
	return SendCmdToLC(UC_PIPE_STOPRUN_REQ, NULL, 0, (PCHAR)&status, sizeof(HANDSHAKE_SYS_STATUS));
}

extern "C" bool WINAPI UCRestart()
{
	return SendCmdToLC(UC_UC_RESET);
}

extern "C" bool WINAPI Maintain()
{
	return SendCmdToLC(UC_MAINTAIN_REQ);
}

extern "C" bool WINAPI AllowTest(int16_t TaskId, uint16_t TestType/* = 0*/)
{
	ALLOW_TEST_CONTEXT ctx = { 0 };
	ctx.TaskId = SwitchByte(TaskId);
	ctx.TestType = SwitchByte(TestType);

	return SendCmdToLC(UC_ALLOW_TEST, (PCHAR)&ctx, sizeof(ALLOW_TEST_CONTEXT));
}

extern "C" bool WINAPI AllowEmergrncyTest(int16_t TaskId, uint16_t TestType /*= 0*/)
{
	ALLOW_TEST_CONTEXT ctx = { 0 };
	ctx.TaskId = SwitchByte(TaskId);
	ctx.TestType = SwitchByte(TestType);

	return SendCmdToLC(UC_ALLOW_EMERGENCYTEST, (PCHAR)&ctx, sizeof(ALLOW_TEST_CONTEXT));
}

extern "C" bool WINAPI MoveMicroCaptureImageArea(const COUNT_POOL_CHANNEL &Chan, const MICROSCOPE_LENS &Lens, const CAPTURE_IMAGE_AREA_CONTEXT& area)
{
	MOVE_NEXT_AREA_REQ_CONTEXT ctx = { 0 };
	ctx.ChannelNumber = Chan;
	ctx.MicroLens = Lens;
	if (Lens == MLENS_10X)
	{
		ctx.M10LensX = area.AreaX;
		ctx.M10LensY = area.AreaY;
	}
	else
	{
		ctx.M40LensX = area.AreaX;
		ctx.M40LensY = area.AreaY;
	}

	return SendCmdToLC(UC_MOVE_NEXT_AREA_REQ, (PCHAR)&ctx, sizeof(MOVE_NEXT_AREA_REQ_CONTEXT));
}

extern "C" bool WINAPI MicroCaptureImageComplete()
{
	return SendCmdToLC(UC_CAPTURE_IMAGE_FINISH);
}

extern "C" bool WINAPI FocusBeginAdjust()
{
	return SendCmdToLC(UC_FOCUSING_REQ);
}

extern "C" bool WINAPI FocusAdjust(const FOCUS_DIRECTION& Direction, int16_t StepCount)
{
	FOCUS_STEP_REQ_CONTEXT ctx = { 0 };
	ctx.Direction = Direction;
	ctx.Step = StepCount;

	return SendCmdToLC(UC_FOCUS_STEP_REQ, (PCHAR)&ctx, sizeof(FOCUS_STEP_REQ_CONTEXT));
}

extern "C" bool WINAPI FocusSave()
{
	return SendCmdToLC(UC_SAVE_FOCALLENGTH_REQ);
}


extern "C" bool WINAPI FocusEndAdjust()
{
	return SendCmdToLC(UC_EXITFOCUSING);
}

extern "C" bool WINAPI MoveCharacterNextAngle()
{
	return SendCmdToLC(UC_ROTATE_NEXT_ANGLE_REQ);
}

extern "C" bool WINAPI CharacterCaptureImageComplete()
{
	return SendCmdToLC(UC_END_BARCODE_TURBIDITY);
}

extern "C" bool WINAPI AlarmProcessed(const ALARM_TYPE& AlarmNum)
{
	HARDWARE_ALARM_CONTEXT ctx = { 0 };
	ctx.AlarmNumber = AlarmNum;

	return SendCmdToLC(UC_ALARM_PROCESSED, (PCHAR)&ctx, sizeof(HARDWARE_ALARM_CONTEXT));
}

extern "C" bool WINAPI GetDevTestInfo(GETTESTINFO_CONTEXT& ctx)
{
	bool ret = SendCmdToLC(UC_GETTESTINFO_REQ, NULL, 0, (PCHAR)&ctx, sizeof(GETTESTINFO_CONTEXT));
	if (ret)
	{
		ctx.nTestType = SwitchByte(ctx.nTestType);
		ctx.nUdcTestItemCount = SwitchByte(ctx.nUdcTestItemCount);
	}
	return ret;
}

extern "C" bool WINAPI ResetZAdjust(int num)
{
	return SendCmdToLC(UC_FOCUS_RESET_Z_ADJUST,(PCHAR)&num,sizeof(int));
}

extern "C" bool WINAPI MotorTest(const MOTOR_ID &MotorId, const MOTOR_ROTATION_DIRECTION &Direction, const MOTOR_SPEED &Speed, uint16_t StepCount)
{
	MOTORTEST_REQ_CONTEXT ctx = { 0 };
	ctx.MotorId = MotorId;
	ctx.MotorDirection = Direction;
	ctx.MotorSpeed = Speed;
	ctx.MotorStep = SwitchByte(StepCount);

	return SendCmdToLC(UC_DBG_MOTORTEST_REQ, (PCHAR)&ctx, sizeof(MOTORTEST_REQ_CONTEXT));
}

extern "C" bool WINAPI SolenoidTest(const SOLENOID_ID &SolenoidId)
{
	SOLENOID_VALVE_TEST_REQ_CONTEXT ctx = { 0 };
	ctx.SolenoidId = SolenoidId;

	return SendCmdToLC(UC_DBG_SOLENOID_VALVE_TEST_REQ, (PCHAR)&ctx, sizeof(SOLENOID_VALVE_TEST_REQ_CONTEXT));
}

extern "C" bool WINAPI MicrosopeLedSwitch(bool bTurnOn /*= true*/)
{
	DWORD cmd = 0;
	if (bTurnOn)
	{
		cmd = UC_DBG_MIROSCOPE_LED_ON_REQ;
	}
	else
	{
		cmd = UC_DBG_MIROSCOPE_LED_OFF_REQ;
	}
	return SendCmdToLC(cmd);
}

extern "C" bool WINAPI BarcodeLedSwitch(bool bTurnOn /*= true*/)
{
	DWORD cmd = 0;
	if (bTurnOn)
	{
		cmd = UC_DBG_BARCODE_LED_ON_REQ;
	}
	else
	{
		cmd = UC_DBG_BARCODE_LED_OFF_REQ;
	}
	return SendCmdToLC(cmd);
}

extern "C" bool WINAPI SensorTest(bool bStart /*= true*/)
{
	DWORD cmd = 0;
	if (bStart)
	{
		cmd = UC_DBG_SENSOR_TEST_REQ;
	}
	else
	{
		cmd = UC_DBG_SENSOR_TEST_END;
	}
	return SendCmdToLC(cmd);
}

extern "C" bool WINAPI ReadSensorConfig(SENSOR_CFG_CONTEXT &cfg)
{
#if (_LC_V <= 2150)
	bool ret = SendCmdToLC(UC_DBG_READ_SENSOR_CFG_REQ, NULL, 0, (PCHAR)&cfg, sizeof(SENSOR_CFG_CONTEXT));
	if (ret)
	{
		cfg.CleanLiquidThreshold = SwitchByte(cfg.CleanLiquidThreshold);
		cfg.TubeExisthreshold = SwitchByte(cfg.TubeExisthreshold);
		cfg.TubeShelfExisthreshold = SwitchByte(cfg.TubeShelfExisthreshold);
		cfg.WastePaperBoxhreshold = SwitchByte(cfg.WastePaperBoxhreshold);
	}
	return ret;
#elif (_LC_V >= 2260)
	bool ret = SendCmdToLC(UC_DBG_READ_SENSOR_CFG_REQ, NULL, 0, (PCHAR)&cfg, sizeof(SENSOR_CFG_CONTEXT));
	if (ret)
	{
		cfg.CleanLiquidThreshold = SwitchByte(cfg.CleanLiquidThreshold);
		cfg.WastePaperBoxhreshold = SwitchByte(cfg.WastePaperBoxhreshold);
		cfg.PaperFliphreshold = SwitchByte(cfg.PaperFliphreshold);
		cfg.PaperAskewthreshold = SwitchByte(cfg.PaperAskewthreshold);
		cfg.TubeShelfExisthreshold = SwitchByte(cfg.TubeShelfExisthreshold);
	}
	return ret;
#elif (_LC_V >= 2180)
	bool ret = SendCmdToLC(UC_DBG_READ_SENSOR_CFG_REQ, NULL, 0, (PCHAR)&cfg, sizeof(SENSOR_CFG_CONTEXT));
	if (ret)
	{
		cfg.CleanLiquidThreshold = SwitchByte(cfg.CleanLiquidThreshold);
		cfg.TubeExisthreshold = SwitchByte(cfg.TubeExisthreshold);
		cfg.TubeShelfExisthreshold = SwitchByte(cfg.TubeShelfExisthreshold);
		cfg.WastePaperBoxhreshold = SwitchByte(cfg.WastePaperBoxhreshold);
		cfg.PaperFliphreshold = SwitchByte(cfg.PaperFliphreshold);
		cfg.SuctionSamplehreshold = SwitchByte(cfg.SuctionSamplehreshold);
		// 		cfg.SuctionSampleMin = SwitchByte(cfg.SuctionSampleMin);
		// 		cfg.SuctionSampleMax = SwitchByte(cfg.SuctionSampleMax);
	}
	return ret;
#endif
}

extern "C" bool WINAPI WirteSensorConfig(const SENSOR_CFG_CONTEXT &cfg, bool &bSuccess)
{
#if (_LC_V <= 2150)
	SENSOR_CFG_CONTEXT *p = (SENSOR_CFG_CONTEXT *)&cfg;
	p->CleanLiquidThreshold = SwitchByte(p->CleanLiquidThreshold);
	p->TubeExisthreshold = SwitchByte(p->TubeExisthreshold);
	p->TubeShelfExisthreshold = SwitchByte(p->TubeShelfExisthreshold);
	p->WastePaperBoxhreshold = SwitchByte(p->WastePaperBoxhreshold);
	return SendCmdToLC(UC_DBG_WRITE_SENSOR_CFG_REQ, (PCHAR)&cfg, sizeof(SENSOR_CFG_CONTEXT), (PCHAR)&bSuccess, sizeof(bool));
#elif (_LC_V >= 2260)
	SENSOR_CFG_CONTEXT *p = (SENSOR_CFG_CONTEXT *)&cfg;
	p->CleanLiquidThreshold = SwitchByte(p->CleanLiquidThreshold);
	p->WastePaperBoxhreshold = SwitchByte(p->WastePaperBoxhreshold);
	p->PaperFliphreshold = SwitchByte(p->PaperFliphreshold);
	p->PaperAskewthreshold = SwitchByte(p->PaperAskewthreshold);
	p->TubeShelfExisthreshold = SwitchByte(p->TubeShelfExisthreshold);
	return SendCmdToLC(UC_DBG_WRITE_SENSOR_CFG_REQ, (PCHAR)&cfg, sizeof(SENSOR_CFG_CONTEXT), (PCHAR)&bSuccess, sizeof(bool));
#elif (_LC_V >= 2180)
	SENSOR_CFG_CONTEXT *p = (SENSOR_CFG_CONTEXT *)&cfg;
	p->CleanLiquidThreshold = SwitchByte(p->CleanLiquidThreshold);
	p->TubeExisthreshold = SwitchByte(p->TubeExisthreshold);
	p->TubeShelfExisthreshold = SwitchByte(p->TubeShelfExisthreshold);
	p->WastePaperBoxhreshold = SwitchByte(p->WastePaperBoxhreshold);

	p->PaperFliphreshold = SwitchByte(p->PaperFliphreshold);
	p->SuctionSamplehreshold = SwitchByte(cfg.SuctionSamplehreshold);
	// 	p->SuctionSampleMin = SwitchByte(p->SuctionSampleMin);
	// 	p->SuctionSampleMax = SwitchByte(p->SuctionSampleMax);
	return SendCmdToLC(UC_DBG_WRITE_SENSOR_CFG_REQ, (PCHAR)&cfg, sizeof(SENSOR_CFG_CONTEXT), (PCHAR)&bSuccess, sizeof(bool));
#else
	SENSOR_CFG_CONTEXT *p = (SENSOR_CFG_CONTEXT *)&cfg;
	p->CleanLiquidThreshold = SwitchByte(p->CleanLiquidThreshold);
	p->TubeExisthreshold = SwitchByte(p->TubeExisthreshold);
	p->TubeShelfExisthreshold = SwitchByte(p->TubeShelfExisthreshold);
	p->WastePaperBoxhreshold = SwitchByte(p->WastePaperBoxhreshold);

	p->PaperFliphreshold = SwitchByte(p->PaperFliphreshold);
	p->SuctionSamplehreshold = SwitchByte(cfg.SuctionSamplehreshold);
	// 	p->SuctionSampleMin = SwitchByte(p->SuctionSampleMin);
	// 	p->SuctionSampleMax = SwitchByte(p->SuctionSampleMax);
	return SendCmdToLC(UC_DBG_WRITE_SENSOR_CFG_REQ, (PCHAR)&cfg, sizeof(SENSOR_CFG_CONTEXT), (PCHAR)&bSuccess, sizeof(bool));
#endif

}

extern "C" bool WINAPI PerformCommonAction(const uint8_t cmd)
{
	return SendCmdToLC(cmd);
}

extern "C" bool WINAPI ChangeChannel(const COUNT_POOL_CHANNEL &DestChan, COUNT_POOL_CHANNEL &CurChan)
{
	CHANNEL_CONTEXT ctx = { 0 }, CurCtx = { 0 };
	ctx.Channel = DestChan;

	bool ret = SendCmdToLC(UC_DBG_CHANGE_CHANNEL_START, (PCHAR)&ctx, sizeof(CHANNEL_CONTEXT), (PCHAR)&CurCtx, sizeof(CHANNEL_CONTEXT));
	if (ret)
	{
		CurChan = (COUNT_POOL_CHANNEL)CurCtx.Channel;
	}
	return ret;
}

extern "C" bool WINAPI ChangeM10LensArea(const CAPTURE_IMAGE_AREA_CONTEXT &DestArea, CAPTURE_IMAGE_AREA_CONTEXT &CurArea)
{
	return SendCmdToLC(UC_DBG_CHANGE_10X_LENS_AREA_START, (PCHAR)&DestArea, sizeof(CAPTURE_IMAGE_AREA_CONTEXT), (PCHAR)&CurArea, sizeof(CAPTURE_IMAGE_AREA_CONTEXT));
}

extern "C" bool WINAPI ChangeM40LensArea(const CAPTURE_IMAGE_AREA_CONTEXT &DestArea, CAPTURE_IMAGE_AREA_CONTEXT &CurArea)
{
	return SendCmdToLC(UC_DBG_CHANGE_40X_LENS_AREA_START, (PCHAR)&DestArea, sizeof(CAPTURE_IMAGE_AREA_CONTEXT), (PCHAR)&CurArea, sizeof(CAPTURE_IMAGE_AREA_CONTEXT));
}

extern "C" bool WINAPI UdcSelAndFeedPaperTest(bool bStart /*= true*/)
{
	DWORD cmd = 0;
	if (bStart)
	{
		cmd = UC_DBG_UDC_SEL_AND_FEED_PAPER_START;
	}
	else
	{
		cmd = UC_DBG_UDC_SEL_AND_FEED_PAPER_END_REQ;
	}
	return SendCmdToLC(cmd);
}

extern "C" bool WINAPI UdcRepeatTest(bool bStart /*= true*/)
{
	DWORD cmd = 0;
	if (bStart)
	{
		cmd = UC_DBG_UDC_REPEATTEST_START;
	}
	else
	{
		cmd = UC_DBG_UDC_REPEATTEST_END_REQ;
	}
	return SendCmdToLC(cmd);
}

extern "C" bool WINAPI UdcStdPaperTest(UDC_STDPAPER_TEST_RESULT_CONTEXT &ctx)
{
	bool ret = SendCmdToLC(UC_DBG_UDC_STDPAPER_TEST_REQ, NULL, 0, (PCHAR)&ctx, sizeof(UDC_STDPAPER_TEST_RESULT_CONTEXT));
	if (ret)
	{
		int n = 56;
		uint16_t *ptr = (uint16_t*)&ctx;
		for (int i = 0; i < n; i++)
		{
			ptr[i] = SwitchByte(ptr[i]);
		}
	}
	return ret;
}

extern "C" bool WINAPI ReadHardwareParam(HARDWARE_PARAM_CONTEXT &ctx)
{
	bool ret = SendCmdToLC(UC_HARDWARE_READ_PARAM_REQ, NULL, 0, (PCHAR)&ctx, sizeof(HARDWARE_PARAM_CONTEXT));
	if (ret)
	{
		int n = 43; // 43个参数
		uint16_t *ptr = (uint16_t*)&ctx;
		for (int i = 0; i < 43; i++) // 改变字节顺序
		{
			ptr[i] = SwitchByte(ptr[i]);
		}
	}
	return ret;
}




extern "C" bool WINAPI ReadBarCode(BARCODE_REQ_CONTEXT_DBG &ctx)
{
	bool ret = SendCmdToLC(UC_DBG_SCANNING_BARCODE_START, NULL, 0, (PCHAR)&ctx, sizeof(BARCODE_REQ_CONTEXT_DBG));
// 	if (ret)
// 	{
// 		int n = 43; // 43个参数
// 		uint16_t *ptr = (uint16_t*)&ctx;
// 		for (int i = 0; i < 43; i++) // 改变字节顺序
// 		{
// 			ptr[i] = SwitchByte(ptr[i]);
// 		}
// 	} 
	return ret;
}

extern "C" bool WINAPI WirteHardwareParam(const HARDWARE_PARAM_CONTEXT &ctx, bool &bSuccess)
{
	int count = sizeof(HARDWARE_PARAM_CONTEXT) / sizeof(uint16_t); //参数个数
	uint16_t *ptr = (uint16_t*)&ctx;
	for (int i = 0; i < count; i++) // 改变字节顺序
	{
		ptr[i] = SwitchByte(ptr[i]);
	}
	return SendCmdToLC(UC_HARDWARE_WRITE_PARAM_REQ, (PCHAR)&ctx, sizeof(HARDWARE_PARAM_CONTEXT), (PCHAR)&bSuccess, sizeof(bool));
}

extern "C" bool WINAPI ResetHardwareParam()
{
	return SendCmdToLC(UC_HARDWARE_RESET_PARAM_REQ);
}

extern "C" bool WINAPI ReadHardwareVersionInfo(HARDWARE_READ_VERSION_CONTEXT &ctx)
{
	return SendCmdToLC(UC_HARDWARE_READ_VERSION_REQ, NULL, 0, (PCHAR)&ctx, sizeof(HARDWARE_READ_VERSION_CONTEXT));
}

extern "C" bool WINAPI ReadMotorConf(MOTOR_CONF_CONTEXT &ctx)
{
	return SendCmdToLC(UC_MOTOR_CONF_READ_REQ, NULL, 0, (PCHAR)&ctx, sizeof(MOTOR_CONF_CONTEXT));	
}

extern "C" bool WINAPI WriteMotorConf(const MOTOR_CONF_CONTEXT &ctx, bool &bSuccess)
{
	return SendCmdToLC(UC_MOTOR_CONF_WRITE_REQ, (PCHAR)&ctx, sizeof(MOTOR_CONF_CONTEXT), (PCHAR)&bSuccess, sizeof(bool));
}

extern "C" bool WINAPI ResetMotorConf()
{
	return SendCmdToLC(UC_MOTOR_CONF_RESET_REQ);
}

extern "C" bool WINAPI WirteHardwareParamTestType( const GETTESTINFO_CONTEXT &ctx, bool &bSuccess )
{
	int n = 2; // 2个参数
	uint16_t *ptr = (uint16_t*)&ctx;
	for (int i = 0; i < 2; i++) // 改变字节顺序
	{
		ptr[i] = SwitchByte(ptr[i]);
	}
	return SendCmdToLC(UC_SETTESTINFO_REQ, (PCHAR)&ctx, sizeof(GETTESTINFO_CONTEXT), (PCHAR)&bSuccess, sizeof(bool));
}

extern "C" bool WINAPI GetZhuiGuangVal(DBG_GET_ZHUIGUANG_VAL_CONTEXT &ctx)
{
	bool ret = SendCmdToLC(UC_DBG_GET_ZHUIGUANG_VAL_START, NULL, 0, (PCHAR)&ctx, sizeof(DBG_GET_ZHUIGUANG_VAL_CONTEXT));
	if (ret)
	{
		int n = 43; // 43个参数
		uint16_t *ptr = (uint16_t*)&ctx;
		for (int i = 0; i < 43; i++) // 改变字节顺序
		{
			ptr[i] = SwitchByte(ptr[i]);
		}
		ctx.nBlackCurrentVal = SwitchByte(ctx.nBlackCurrentVal);
		ctx.nBlackZhuiGuangVal = SwitchByte(ctx.nBlackZhuiGuangVal);
		ctx.nHasCurrentVal = SwitchByte(ctx.nHasCurrentVal);
		ctx.nHasZhuiGuangVal = SwitchByte(ctx.nHasZhuiGuangVal);
	} 
	return ret;
}

/////////////////////////////////////////////////////////////////old///////////////////////////////////////////////
extern "C" bool WINAPI BeginSelfCheck()
{
	// not implement
	return true;
}

extern "C" bool WINAPI EnableAutoCheck(bool &IsEnableSuccess)
{
	// not implement
	return true;
}

extern "C" bool WINAPI DisableAutoCheck()
{
	PipeStopAutoRun();
	return true;
}

static void GetConfigFile(char *ConfigFile)
{
	GetModuleFileName(NULL, ConfigFile, MAX_PATH);
	PathRemoveFileSpec(ConfigFile);
	PathAppend(ConfigFile, "config\\setting.ini");
}

static UINT GetMovePlatformSleepTime()
{
	// not implement
	return true;
}

extern "C" bool WINAPI MoveScanPlatform(const CMotorType &MotorID, const CDirection &Direction, const WORD &StepCount)
{
	// not implement
	return true;
}

extern "C" bool WINAPI TurnOnLight()
{
	// not implement
	return true;
}

extern "C" bool WINAPI TurnOffLight()
{
	// not implement
	return true;
}

static UINT GetChannelTime()
{
	char ConfigFile[MAX_PATH] = {0};
	GetConfigFile(ConfigFile);
	
	return GetPrivateProfileInt("lc", "channel_time", 3000, ConfigFile);
}

extern "C" bool WINAPI ConvertToAChannel()
{
	// not implement
	return true;
}

extern "C" bool WINAPI ConvertToBChannel()
{
	// not implement
	return true;
}

static UINT GetSwitchTime()
{
	char ConfigFile[MAX_PATH] = {0};
	GetConfigFile(ConfigFile);
	
	return GetPrivateProfileInt("lc", "switch_time", 3000, ConfigFile);
}

extern "C" bool WINAPI ConvertTo10Microscope()
{
	// not implement
	return true;
}

extern "C" bool WINAPI ConvertTo40Microscope()
{
	// not implement
	return true;
}

static UINT GetXYResetSleepTime()
{
	char ConfigFile[MAX_PATH] = {0};
	GetConfigFile(ConfigFile);

	return GetPrivateProfileInt("lc", "reset_time", 3000, ConfigFile);
}

extern "C" bool WINAPI XAxisReset()
{
	// not implement
	return true;
}

extern "C" bool WINAPI YAxisReset()
{
	// not implement
	return true;
}

extern "C" bool WINAPI ZAxisReset()
{
	// not implement
	return true;
}

#define SECTION_Z_AXIS   "zaxis"
#define KEY_Z_A_40       "a40"
#define KEY_Z_B_40       "b40"
#define INVALID_Z        0 

#define CONFIG_FILE_PATH  "config\\Conf.ini"

void SetZAxisInfo(CZPosType ZPosType, int ZAxisInfo)
{
	char Value[20] = {0};
	char IniFile[MAX_PATH] = {0};

	GetConfigFilePath( IniFile, sizeof(IniFile), CONFIG_FILE_PATH );
	StringCbPrintf( Value, sizeof(Value), "%d",  ZAxisInfo);

	if(ZPosType == AChannel40)
	{
		WritePrivateProfileString(SECTION_Z_AXIS, KEY_Z_A_40, Value, IniFile);
	}
	else if(ZPosType == BChannel40)
	{
		WritePrivateProfileString(SECTION_Z_AXIS, KEY_Z_B_40, Value, IniFile);
	}
	else
	{
		
	}
}

extern "C" bool WINAPI ResetZAxisInfo()
{
	WORD StepCount = 0;
	CZPosType ZPos = AChannel40;
	
	if( !QueryZPos(ZPos, StepCount) )
		return false;
	
	if (ZPos != AChannel40 && ZPos != BChannel40)
		return true;

	SetZAxisInfo(ZPos, INVALID_Z);
	return true;
}

bool GetZAxisStartPointFromIni(CZPosType ZPosType, WORD &StartPoint)
{
	char IniFile[MAX_PATH] = {0};

	GetConfigFilePath( IniFile, sizeof(IniFile), CONFIG_FILE_PATH );

	if (ZPosType == AChannel40)
	{
		StartPoint = GetPrivateProfileInt(SECTION_Z_AXIS, KEY_Z_A_40, INVALID_Z, IniFile);
	}
	else if (ZPosType == BChannel40)
	{
		StartPoint = GetPrivateProfileInt(SECTION_Z_AXIS, KEY_Z_B_40, INVALID_Z, IniFile);
	}
	else
	{
		return false;
	}

	if (StartPoint == INVALID_Z)
		return false;

	return true;
}

extern "C" bool WINAPI MoveZAxis(const WORD &StepCount)
{
	// not implement
	return true;
}

extern "C" bool WINAPI MoveZAxisToStartPoint()
{
	WORD StartPoint = 0;
	WORD StepCount = 0;
	CZPosType ZPos = AChannel40;

	if( !QueryZPos(ZPos, StepCount) )
		return false;

	if (ZPos != AChannel40 && ZPos != BChannel40)
		return false;

	if ( !GetZAxisStartPointFromIni(ZPos, StartPoint) )
	{
		SetZAxisInfo(ZPos, StepCount);
		return true;
	}

	return MoveZAxis(StartPoint);
}

extern "C" bool WINAPI Save40XYSetting()
{
	bool Result = true;
	
	WORD XOffset = 0;
	WORD YOffset = 0;
	
	CLCInfo LCInfo = {0};
	
	Result = QueryXY40Offset(XOffset, YOffset);
	if (!Result)
	{
		goto end;
	}
	
	Result = GetLCInfo(LCInfo);
	if (!Result)
	{
		goto end;
	}
	
	LCInfo.X40AdjustSteps = XOffset;
	LCInfo.Y40AdjustSteps = YOffset;
	
	Result = SetLCInfo(LCInfo);
	
end:
	return Result;
}

extern "C" bool WINAPI SaveZSetting()
{
	bool Result = true;
	
	CZPosType ZPos = AChannel40;
	WORD StepCount = 0;
	
	CLCInfo LCInfo = {0};
	
	Result = QueryZPos(ZPos, StepCount);
	if (!Result)
	{
		goto end;
	}
	
	Result = GetLCInfo(LCInfo);
	if (!Result)
	{
		goto end;
	}
	
	if (ZPos == AChannel10)
	{
		LCInfo.Z10AFocusSteps = StepCount;
	}
	else if (ZPos == AChannel40)
	{
		LCInfo.Z40AFocusSteps = StepCount;
	}
	else if (ZPos == BChannel10)
	{
		LCInfo.Z10BFocusSteps = StepCount;
	}
	else
	{
		LCInfo.Z40BFocusSteps = StepCount;
	}
	
	Result = SetLCInfo(LCInfo);
	
end:
	return Result;
}

extern "C" bool WINAPI TestHardware(const CHardwareType &Hardware, const CHardwareAction &Action)
{
	// not implement
	return true;
}

extern "C" bool WINAPI TroubleShooting(const CTroubleType &TroubleType)
{
	// not implement
	return true;
}

extern "C" bool WINAPI QueryZPos(CZPosType &ZPos, WORD &StepCount)
{
	// not implement
	return true;
}

extern "C" bool WINAPI QueryXY40Offset(WORD &XOffset, WORD &YOffset)
{
	// not implement
	return true;
}

extern "C" bool WINAPI Clean(unsigned short cleanCount)
{
	// 测试界面日常清洗
	int n = 2; // 2个参数
	uint16_t *ptr = (uint16_t*)&cleanCount;

	ptr[0] = SwitchByte(ptr[0]);
	SendCmdToLC(UC_AUTO_CLEAN, (PCHAR)&cleanCount, sizeof(unsigned short));
	return true;
	return true;
}

extern "C" bool WINAPI ForceClean(unsigned short cleanCount)
{
	// 测试界面强化清洗
	int n = 2; // 2个参数
	uint16_t *ptr = (uint16_t*)&cleanCount;

	ptr[0] = SwitchByte(ptr[0]);
	SendCmdToLC(UC_AUTO_POWERFULCLEAN, (PCHAR)&cleanCount, sizeof(unsigned short));
	return true;
}

extern "C" bool WINAPI GetLCInfo(CLCInfo &LCInfo)
{
	// not implement
	return true;
}

extern "C" bool WINAPI SetLCInfo(const CLCInfo &LCInfo)
{
	// not implement
	return true;
}

extern "C" bool WINAPI EnterHardwareDebug()
{
	// not implement
	return true;
}

extern "C" bool WINAPI ExitHardwareDebug()
{
	// not implement
	return true;
}

extern "C" bool WINAPI RestartLC()
{
	// not implement
	return true;
}

extern "C" bool WINAPI Locate10Microscope(const CLocate10MicroscopeInfo &LocateInfo)
{
	// not implement
	return true;
}

extern "C" bool WINAPI Locate40Microscope(const CLocate40MicroscopeInfo &LocateInfo)
{
	// not implement
	return true;
}

extern "C" bool WINAPI SpecialTest(QCType testType)
{
	QCType checkTestType = {0};

	int ret = SendCmdToLC(UC_SETQC_REQ, (PCHAR)&testType, sizeof(QCType), (PCHAR)&checkTestType, sizeof(QCType));
	if (ret)
	{
		if (memcmp(&testType, &checkTestType,sizeof(QCType)) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return ret;
}


extern "C" bool WINAPI EnablePipeIn()
{
	// not implement
	return true;
}

extern "C" bool WINAPI DisablePipeIn()
{
	// not implement
	return true;
}

extern "C" bool WINAPI NotifyConsumables(UCHAR flag)
{
	// not implement
	return true;
}

extern "C" bool WINAPI UDCCorrectModule(bool &IsSuccess)
{
	// not implement
	return false;
}

extern "C" bool WINAPI UDCCorrectModuleEx(bool &IsSuccess)
{
	// not implement
	return true;
}

extern "C" bool WINAPI UDCSpecifyPaperType(CPaperType PaperType, bool &IsSuccess)
{
	// not implement
	return true;
}

extern "C" bool WINAPI UDCTestPaper(CUCDTestErrorCode &ErrorCode, CUDCTestPaperResult &TestResult)
{
	// not implement
	return true;
}

extern "C" bool WINAPI UDCTurnOnLight()
{
	// not implement
	return true;
}

extern "C" bool WINAPI UDCTurnOffLight()
{
	// not implement
	return true;
}


extern "C" bool WINAPI PhysicsSgSensorTest(int nType)
{
	if (nType == 1)
		return SendCmdToLC(UC_DBG_BEGIN_SG_SENSOR_TEST, NULL, 0, NULL, 0);
	else
		return SendCmdToLC(UC_DBG_OVER_SG_SENSOR_TEST, NULL, 0, NULL, 0);
}

extern "C" bool WINAPI PhysicsSgCalibrate(int nSgType, SG_CALIBRATE_CONTEXT & SgCalibrateResult)
{
	return SendCmdToLC(UC_SG_CALIBRATE, (PCHAR)&nSgType, sizeof(nSgType), (PCHAR)&SgCalibrateResult, sizeof(SG_CALIBRATE_CONTEXT));
}

extern "C" bool WINAPI PhysicsSgTest(PHYSICS_SG_DBG_RESULT_CONTEXT & SgTestResult)
{
	return SendCmdToLC(UC_DBG_BEGIN_SG_ONE_TEST, NULL, 0, (PCHAR)&SgTestResult, sizeof(PHYSICS_SG_DBG_RESULT_CONTEXT));
}

//extern "C" bool WINAPI PhysicsTurbidityCalibrate(int nTurbidityType, SG_CALIBRATE_CONTEXT & TurbidityCalibrateResult)
//{
//	return SendCmdToLC(UC_TURBIDITY_CALIBRATE, (PCHAR)&nTurbidityType, sizeof(nTurbidityType), (PCHAR)&TurbidityCalibrateResult, sizeof(SG_CALIBRATE_CONTEXT));
//}
//extern "C" bool WINAPI PhysicsColorCalibrate(COLOR_CALIBRATE_CONTEXT & ColorCalibrateResult)
//{
//	return SendCmdToLC(UC_COLOR_CALIBRATE, NULL, 0, (PCHAR)&ColorCalibrateResult, sizeof(COLOR_CALIBRATE_CONTEXT));
//}
//extern "C" bool WINAPI PhysicsColorZuiGuang(COLOR_ZUIGUANG_CONTEXT & ColorZuiGuangResult)
//{
//	return SendCmdToLC(UC_COLOR_WB, NULL, 0, (PCHAR)&ColorZuiGuangResult, sizeof(COLOR_ZUIGUANG_CONTEXT));
//}