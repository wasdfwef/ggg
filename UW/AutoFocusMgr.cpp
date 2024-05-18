#include "StdAfx.h"
#include "AutoFocusMgr.h"
#include "AutoFocusDlg.h"
#include "..\..\..\inc\CmdHelper.h"
#include "..\..\..\inc\AutoFocus.h"

#pragma comment(lib, "..\\..\\..\\lib\\AutoFocus.lib")
#pragma comment(lib, "..\\..\\..\\lib\\CmdHelper.lib")
#include "DbgMsg.h"

#define SECTON_A_10  "A10"
#define SECTON_A_40  "A40"
#define SECTON_B_10  "B10"
#define SECTON_B_40  "B40"

#define KEY_X_PIC "xpic"
#define KEY_Y_PIC "ypic"
#define KEY_40_PIC "40pic"
#define KEY_X_STEP "x_step"
#define KEY_Y_STEP "y_step"
#define KEY_Z_POS  "z_pos"

#define INVALID_VALUE "OXFFFFFF"

LONG CAutoFocusMgr::m_IsCancelFocus = 0;

CAutoFocusMgr::CAutoFocusMgr(void)
{
}

CAutoFocusMgr::~CAutoFocusMgr(void)
{
}


void CAutoFocusMgr::GetSettingFile(char *FilePah)
{
	GetModuleFileNameA(NULL, FilePah, MAX_PATH);
	PathRemoveFileSpecA(FilePah);
	PathAppendA(FilePah, "config\\setting.ini");
}


void CAutoFocusMgr::WritePrivateProfileInt(char *SecName, char *key, int Value, char *ConfigFile)
{
	char StrValue[30] = {0};
	itoa(Value, StrValue, 10);

	WritePrivateProfileStringA(SecName, key, StrValue, ConfigFile);
}

bool CAutoFocusMgr::GetPrivateProfileInt(char *SecName, char *key, char *ConfigFile, int &Value)
{
	char StrValue[30] = {0};

	GetPrivateProfileStringA(SecName, key, INVALID_VALUE, StrValue, sizeof(StrValue), ConfigFile);
	if ( 0 != stricmp(INVALID_VALUE, StrValue) )
	{
		Value = atoi(StrValue);
		return true;
	}

	return false;
}


void CAutoFocusMgr::SaveCameraConfigInfo(CZPosType PosType, int XStep, int YStep, int ZPos)
{
	char *SectionName = NULL;
	char *KeyName = NULL;

	char ConfigFile[MAX_PATH] = {0};
	GetSettingFile(ConfigFile);

	if( PosType == AChannel10 )
	{
		SectionName = SECTON_A_10;
	}
	else if( PosType == BChannel10 )
	{
		SectionName = SECTON_B_10;
	}
	else if( PosType == AChannel40 )
	{
		SectionName = SECTON_A_40;
	}
	else
	{
		SectionName = SECTON_B_40;
	}

	if( ZPos > 3800 || ZPos < 300 )
		ZPos = 2000;

	WritePrivateProfileInt(SectionName, KEY_X_STEP, XStep, ConfigFile);
	WritePrivateProfileInt(SectionName, KEY_Y_STEP, YStep, ConfigFile);
	WritePrivateProfileInt(SectionName, KEY_Z_POS, ZPos, ConfigFile);		
}


bool CAutoFocusMgr::GetCameraConfigInfo(CZPosType PosType, int &XStep, int &YStep, int &ZPos)
{
	char *SectionName = NULL;
	char *KeyName = NULL;

	char ConfigFile[MAX_PATH] = {0};
	GetSettingFile(ConfigFile);

	if( PosType == AChannel10 )
	{
		SectionName = SECTON_A_10;
	}
	else if( PosType == BChannel10 )
	{
		SectionName = SECTON_B_10;
	}
	else if( PosType == AChannel40 )
	{
		SectionName = SECTON_A_40;
	}
	else
	{
		SectionName = SECTON_B_40;
	}

	if ( !GetPrivateProfileInt(SectionName, KEY_X_STEP, ConfigFile, XStep) )
		return false;

	if( !GetPrivateProfileInt(SectionName, KEY_Y_STEP, ConfigFile, YStep) )
		return false;
	if( !GetPrivateProfileInt(SectionName, KEY_Z_POS, ConfigFile, ZPos) )
		return false;	
	return true;
}


bool CAutoFocusMgr::BeginAutoFocus()
{

	if ( !HasConfigCamera() )
		return true;
DBG_MSG("ShowAutoFocusDlg");
	CAutoFocusDlg AutoFocusDlg;
	AutoFocusDlg.DoModal();
	return true;
}


bool CAutoFocusMgr::HasConfigCamera()
{
	char Value[30] = {0};
	char ConfigFile[MAX_PATH] = {0};

	GetSettingFile(ConfigFile);	
	GetPrivateProfileStringA(SECTON_A_10, KEY_X_STEP, INVALID_VALUE, Value, sizeof(Value), ConfigFile);
	return ( 0 != stricmp(Value, INVALID_VALUE) );
}


bool CAutoFocusMgr::MoveScanPlatform(CMotorType MotorType, int Step)
{
	CDirection Direction = Foreward;

	if(Step < 0)
	{
		Step = -Step;
		Direction = Backward;
	}
	return ::MoveScanPlatform(MotorType, Direction, Step);
}


bool CAutoFocusMgr::MoveCameraByConfigInfo(CZPosType ZPosType, bool bNeed10XConfigInfo /*= false*/ )
{
	int XStep = 0,XTemp = 0;
	int YStep = 0,YTemp = 0;
	int ZPos = 0,ZTemp = 0;

	if( !GetCameraConfigInfo( ZPosType, XStep, YStep, ZPos ) )
		return true;

	if( bNeed10XConfigInfo )
	{
		
		if( ZPosType == AChannel40 )
		{
			GetCameraConfigInfo( AChannel10, XTemp, YTemp, ZTemp );
		}
		else if( ZPosType == BChannel40 )
		{
			GetCameraConfigInfo( BChannel10, XTemp, YTemp, ZTemp );
		}

		XStep += XTemp;
		YStep += YTemp;
		
	}
	DBG_MSG("XStep:%d,YStep:%d,ZPos:%d\n",XStep,YStep,ZPos);
	//MessageBoxW(NULL,_T("开始移动x轴"),NULL,0);
	if( ! MoveScanPlatform(XMotorID, XStep) ) 
	{
		DBG_MSG("移动x轴失败\n");		
		return false; 
	}
		
	//MessageBoxW(NULL,_T("开始移动Y轴"),NULL,0);
	if( !MoveScanPlatform(YMotorID, YStep) )
	{
		DBG_MSG("移动y轴失败\n");
		//MessageBoxW(NULL,_T("移动y轴失败"),NULL,0);
		return false; 
	}
	//MessageBoxW(NULL,_T("开始移动z轴"),NULL,0);

	if( ZPos > 3800 || ZPos < 300 )
		ZPos = 2000;

	if( !MoveZAxis(ZPos) )
	{
		DBG_MSG("移动z轴失败\n");
		//MessageBoxW(NULL,_T("移动z轴失败"),NULL,0);
		//return false; 
	}

	return true;
}



#define CANCEL_FOCUS  2

DWORD CAutoFocusMgr::FocusOneCameraLen(CZPosType ZPosType)
{
	DBG_MSG("call FocusOneCameraLen\n");
	if ( InterlockedExchange(&m_IsCancelFocus, 0) != 0)
		return CANCEL_FOCUS;

	if ( ZPosType == AChannel10 || ZPosType == BChannel10 )
	{
		DBG_MSG("切换到10X镜\n");
		if( !ConvertTo10Microscope() )
		{
			DBG_MSG("切换到10X镜失败\n");			
			return 1;
		}			
	}
	else	
	{			
		DBG_MSG("切换到40X镜\n");
		if( !ConvertTo40Microscope() )
		{
			DBG_MSG("切换到40X镜失败\n");				
			return 1;
		}
	}

// 	ULONG  nStartTime;
// 	BOOL   b = FALSE;

	if(ZPosType == AChannel10)
	{
		if( !MoveToStartPos(AChannel) )
		{
			DBG_MSG("切换到A10X镜初始位置失败\n");			
			return 1;
		}
			
	}
	else if(ZPosType == BChannel10)
	{
		if( !MoveToStartPos(BChannel) )
		{
			DBG_MSG("切换到B10X镜初始位置失败\n");				
			return 1;
		}
	}	
	else if( ZPosType == AChannel40 )
	{
// 		b = TRUE;
// 		nStartTime = GetTickCount();

		if( !MoveTo40XStartPos(AChannel) )
		{
			DBG_MSG("切换到A40X镜初始位置失败\n");		
			return 1;
		}
			

		//DBG_MSG("A channel Reset Time = %u ms\n", GetTickCount() - nStartTime);
	}
	else if( ZPosType == BChannel40 )
	{
// 		b = TRUE;
// 		nStartTime = GetTickCount();
		if( !MoveTo40XStartPos(BChannel) ) 
		{
			DBG_MSG("切换到B40X镜初始位置失败\n");			
			return 1;
		}
			
		/*DBG_MSG("B channel Reset Time = %u ms\n", GetTickCount() - nStartTime);*/
	}

	//nStartTime = GetTickCount();
	if( !MoveCameraByConfigInfo( ZPosType) )
		return 1;

	//DBG_MSG("Move Scan PosType Time = %u ms\n", GetTickCount() - nStartTime);

	if( FALSE == FindFocus(ZPosType,&m_IsCancelFocus) )
		return 1;

	return 0;
}

#define CHECK_RESULT(Result)                  \
{												\
	if (Result != 0)							\
	{											\
		if ( CANCEL_FOCUS == Result)			\
			Result = 0;						\
												\
		goto end;									\
	}												\
}


bool CAutoFocusMgr::MoveToStartPos(CChannelType ChannelType)
{
	CLocate10MicroscopeInfo LocateInfo = {0};

	LocateInfo.ChannelNum = ChannelType;
	LocateInfo.IsReturnTakePicCommand = 1;
	LocateInfo.XPicNum = 0;
	LocateInfo.YPicNum = 0;

	//MessageBox(NULL,L"定位到A通道0，0位置",NULL,0);
	return Locate10Microscope(LocateInfo);
}


bool CAutoFocusMgr::MoveToAChannelStartPos()
{
	CZPosType ZPos = AChannel10;
	WORD StepCount = 0;

	//MessageBoxW(NULL,_T("查询AChannel 10X Z步数"),NULL,0);
	
	if( !QueryZPos(ZPos, StepCount) )
		return false;

	if ( ZPos != AChannel10 || ZPos != AChannel40)
	{
		//MessageBoxW(NULL,_T("切换到A通道"),NULL,0);
		
		if ( !ConvertToAChannel() )
			return false;
	}

	//return MoveToStartPos(AChannel);
	return true;
}


DWORD CAutoFocusMgr::AutoFocus(HWND hWnd)
{
	int XStep = 0;
	int YStep = 0;
	int ZPos = 0;
	int Result = 0;

	//ULONG nStartTime = GetTickCount();

/*	if( !MoveToAChannelStartPos() )
	{
		Result = 1;
		goto end;
	}*/

	if( !ConvertToAChannel() )
	{
		DBG_MSG("切换到A通道失败");
		Result = 1;
		goto end;
	}
	//theApp.m_IsFirst = FALSE; //debug code

	/*if( theApp.m_IsFirst == true )*/
	{
		Result = FocusOneCameraLen(AChannel10);
		CHECK_RESULT(Result);
		DBG_MSG("AChannel10聚焦完成");
	}		
	
	Result = FocusOneCameraLen(AChannel40);
	CHECK_RESULT(Result);
	DBG_MSG("AChannel40聚焦完成");
	
	if( !ConvertToBChannel() )
	{
		DBG_MSG("切换到B通道失败");
		Result = 1;
		goto end;
	}
//DBG_MSG("换通道");

	/*if( theApp.m_IsFirst == true )*/
	{
		Result = FocusOneCameraLen(BChannel10);
		CHECK_RESULT(Result);
		DBG_MSG("BChannel10聚焦完成");
	}	

	Result = FocusOneCameraLen(BChannel40);
	CHECK_RESULT(Result);
	DBG_MSG("BChannel40聚焦完成");

	Result = 0;

end:
	//DBG_MSG("整个AutoFocus花费时间 = %u s\n", ((GetTickCount()- nStartTime) / 1000));

	PostMessage(hWnd, WM_CLOSE, 0, 0);	
	return Result;
}


void CAutoFocusMgr::CancelAutoFocus()
{
	InterlockedExchange(&m_IsCancelFocus, 1);
}

bool CAutoFocusMgr::MoveTo40XStartPos( CChannelType ChannelType )
{
	CLocate40MicroscopeInfo LocateInfo = {0};

	LocateInfo.ChannelNum = ChannelType;
	LocateInfo.IsReturnTakePicCommand = 1;
	LocateInfo.XPicNumFor10 = 0;
	LocateInfo.YPicNumFor10 = 0;
	LocateInfo.XOffset      = 0;
	LocateInfo.YOffset      = 0;
	LocateInfo.PicNumFor40  = 0;
	return Locate40Microscope(LocateInfo);
}