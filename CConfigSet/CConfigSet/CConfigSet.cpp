// CConfigSet.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "CConfigSet.h"

#include "QueryAndSetConfig.h"
#include "DbgMsg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CCConfigSetApp

BEGIN_MESSAGE_MAP(CCConfigSetApp, CWinApp)
END_MESSAGE_MAP()


// CCConfigSetApp 构造

CCConfigSetApp::CCConfigSetApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CCConfigSetApp 对象

CCConfigSetApp theApp;


// CCConfigSetApp 初始化

BOOL CCConfigSetApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


CQueryAndSetConfig db;
extern "C"
VOID
WINAPI
SetConfigIniPath(
PCHAR    pResIniPath
			  )
{
	if( pResIniPath == NULL || pResIniPath[0] == 0 || PathFileExists(pResIniPath) == FALSE )
		db.SetResIniPath(NULL);
	else
		db.SetResIniPath(pResIniPath);

}


extern "C"
KSTATUS
WINAPI
QueryRecordConfig(
PCONFIG_RECORD_INFO ConfigInfo
							)
{
	KSTATUS   status = STATUS_INVALID_PARAMETERS;

	if( ConfigInfo->nConfigLength == 0 || ConfigInfo->pConfigConent == NULL )
	{
		DBG_MSG("fail");
		return status;
	}

	DBG_MSG("GET_US_INFO = %u\n",sizeof(GET_US_INFO));

	if( ConfigInfo->nRecordType == US_TYPE_INFO && ConfigInfo->nConfigLength == sizeof(GET_US_INFO) )
	{
		DBG_MSG("Access_QueryRecordConfig\n");
		status = db.TH_DB_GetUsInfo((GET_US_INFO*)ConfigInfo->pConfigConent);
	}

	else if( ConfigInfo->nRecordType == UDC_TYPE_INFO && ConfigInfo->nConfigLength == sizeof(GET_UDC_INFO) )
		status = db.TH_DB_GetUdcInfo((GET_UDC_INFO*)ConfigInfo->pConfigConent);

	return status;
}

extern "C"
KSTATUS
WINAPI
SetRecordConfig(
PCONFIG_RECORD_INFO ConfigInfo
						  )
{
	KSTATUS   status = STATUS_INVALID_PARAMETERS;

	if( ConfigInfo->nConfigLength == 0 || ConfigInfo->pConfigConent == NULL )
		return status;

	if( ConfigInfo->nRecordType == US_TYPE_INFO && ConfigInfo->nConfigLength == sizeof(SET_US_INFO) )
	{
		DBG_MSG("Access_SetRecordConfig");
		SET_US_INFO* s_info = (SET_US_INFO*)ConfigInfo->pConfigConent;
		status = db.TH_DB_SetUsInfo(*s_info);

	}
	else if( ConfigInfo->nRecordType == UDC_TYPE_INFO && ConfigInfo->nConfigLength == sizeof(SET_UDC_INFO) )
	{
		SET_UDC_INFO* c_info = (SET_UDC_INFO*)ConfigInfo->pConfigConent;
		status = db.TH_DB_SetUdcInfo(*c_info);
	}

	return status;
}