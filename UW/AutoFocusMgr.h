#pragma once

#include "..\..\..\inc\CmdHelper.h"
#include "UW.h"
class CAutoFocusMgr
{
public:
	CAutoFocusMgr(void);
	~CAutoFocusMgr(void);

	static bool BeginAutoFocus();
	static bool HasConfigCamera();
	static DWORD AutoFocus(HWND hWnd);
	static void CancelAutoFocus();
	static void GetSettingFile(char *FilePah);

	static bool MoveToStartPos(CChannelType ChannelType);
	static bool MoveTo40XStartPos(CChannelType ChannelType);
	static bool MoveCameraByConfigInfo(CZPosType ZPosType, bool bNeed10XConfigInfo = false);
	static void SaveCameraConfigInfo(CZPosType PosType, int XStep, int YStep, int ZPos);
	static bool GetCameraConfigInfo(CZPosType PosType, int &XStep, int &YStep, int &ZPos);
private:
	static LONG m_IsCancelFocus;
	static void WritePrivateProfileInt(char *SecName, char *key, int Value, char *ConfigFile);
	static bool GetPrivateProfileInt(char *SecName, char *key, char *ConfigFile, int &Value);
	static bool MoveScanPlatform(CMotorType MotorType, int Step);
	static DWORD FocusOneCameraLen(CZPosType ZPosType);
	static bool MoveToAChannelStartPos();
};
