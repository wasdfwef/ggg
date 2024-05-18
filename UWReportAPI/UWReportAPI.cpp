// UWReportAPI.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "afxwin.h"
#include <xstring>
#include <string.h>
#include <Winspool.h>
#pragma comment(lib, "Winspool")


#include <Shlwapi.h>
#include "resource.h"
#include "PrintViewDlg.h"

#include "DbgMsg.h"
#include "..\..\inc\f_err.h"

#include "..\..\inc\UW2000DB.h"
#pragma comment(lib, "..\\..\\lib\\UW2000DB.lib")


#include "..\..\inc\THInterface.h"
#pragma comment(lib, "..\\..\\lib\\THInterface.lib")



#include "afxcmn.h"
using namespace std;
#include "UWReportAPI.h"
#include "UWVAr.h"
#include "UWQCVar.h"
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

// CUWReportAPIApp

BEGIN_MESSAGE_MAP(CUWReportAPIApp, CWinApp)
END_MESSAGE_MAP()


// CUWReportAPIApp 构造

CUWReportAPIApp::CUWReportAPIApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CUWReportAPIApp 对象

CUWReportAPIApp theApp;


// CUWReportAPIApp 初始化

BOOL CUWReportAPIApp::InitInstance()
{

	
	CWinApp::InitInstance();
	return TRUE;
}



static
HANDLE
UWReport_Pre(
ULONG   nTaskID,
PCHAR   pModulePath
)
{
	HANDLE            hPreHandle = NULL;
	KSTATUS           status;
	CUWVAr            UW;
	ULONG             var_counter;
	PSHELL_VAR_INFO   var_info;
	TASK_INFO         Task_info = {0};

	if( pModulePath == NULL || pModulePath[0] == 0 || PathFileExists(pModulePath) == FALSE )
	{
		DBG_MSG("UWReportPre->par Error...");
		return hPreHandle;
	}
	//Access_OpenAccess(TRUE);
	DBG_MSG("nTaskID = %d\n",nTaskID);	
	status = Access_GetTaskInfo(nTaskID,ALL_TYPE_INFO,&Task_info); 
	DBG_MSG("Task_info.main_info.nID = %d\n",Task_info.main_info.nID);
	//Access_CloseAccess();
	if( status != STATUS_SUCCESS )
	{

		DBG_MSG("BuildShellVarInfoByGroupPrint->ReadRecord Error...%x", status);
		return hPreHandle;
	}
	

	DBG_MSG("Task_info.us_info.us_node[0].us_res = %f\n",Task_info.us_info.us_node[0].us_res);
	status = UW.BuildVarArray(&Task_info, var_info, var_counter);

	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("??????\n");
		return hPreHandle;
	}

	for (int i = 0; i < var_counter; i++)
		DBG_MSG("var_info[%d]: %s\n", i, var_info[i].VarValue);
	hPreHandle = THReportAPI_PreStep(pModulePath, var_info, var_counter);
	UW.FreeVarArray(var_info);
	DBG_MSG("THReportAPI_UWReportPre->hPreHandle = %x\n", hPreHandle);
	return hPreHandle;

}


#define CONFIG_DIR       "config"
#define INI_FILE_NAME    "setting.ini"
#define SECTION_PRINTER "printer"
#define KEY_DEFAULT_PRINTER "default"
#define DEFAULT_PRINTER ""


void GetIniFilePath(char *IniFilePath, DWORD IniFilePathLength)
{
	GetModuleFileNameA(NULL, IniFilePath, IniFilePathLength);
	PathRemoveFileSpecA(IniFilePath);
	PathAppendA(IniFilePath, CONFIG_DIR);
	PathAppendA(IniFilePath, INI_FILE_NAME);
}


#define PRN_FILE_NAME        "prn_dm.dat"

void GetDevModeInfoPath(char *IniFilePath, DWORD IniFilePathLength)
{
	GetModuleFileNameA(NULL, IniFilePath, IniFilePathLength);
	PathRemoveFileSpecA(IniFilePath);
	PathAppendA(IniFilePath, CONFIG_DIR);
	PathAppend(IniFilePath, PRN_FILE_NAME);
}

BOOL SavePrinterDM( PDEVMODE dev_mode )
{
	char      prn_path[MAX_PATH];
	ULONG     r;
	HANDLE    hFile;

	GetDevModeInfoPath(prn_path,sizeof(prn_path));

	hFile = CreateFile(prn_path, GENERIC_WRITE, NULL, 0, CREATE_ALWAYS, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	WriteFile(hFile, dev_mode, dev_mode->dmSize + dev_mode->dmDriverExtra, &r, NULL);
	CloseHandle(hFile);
	return TRUE;
}

BOOL LoadPrnDM(PDEVMODE & dev_mode)
{
	char      prn_path[MAX_PATH];
	ULONG     r;
	HANDLE    hFile;
	PDEVMODE  pDev_mode = NULL;

	GetDevModeInfoPath(prn_path,sizeof(prn_path));

	hFile = CreateFile(prn_path, GENERIC_READ, NULL, 0, OPEN_EXISTING, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	r = GetFileSize(hFile, NULL);
	if( r == 0 )
	{
		CloseHandle(hFile);	
		return FALSE;
	}


	dev_mode = (PDEVMODE)new CHAR[r];
	ReadFile(hFile, dev_mode, r, &r, NULL);
	CloseHandle(hFile);	

	return TRUE;
}

bool OpenDefaultPrinter(PCHAR pPrinterName, PDEVMODE &pDevMode)
{
	bool      Result = false;
	char      IniFile[MAX_PATH] = {0};
	char      PrinterName[MAX_PATH] = {0};


	GetIniFilePath( IniFile, sizeof(IniFile) );
	GetPrivateProfileString(SECTION_PRINTER, KEY_DEFAULT_PRINTER, DEFAULT_PRINTER, PrinterName, sizeof(PrinterName), IniFile);

prn_err_loc:

	if ( PrinterName[0] == 0 )
	{
		CPrintDialog dlg(FALSE);

		if(dlg.DoModal() != IDOK)
			goto end;

		strncpy_s( PrinterName, dlg.GetDeviceName(), sizeof(PrinterName) );
		WritePrivateProfileString(SECTION_PRINTER, KEY_DEFAULT_PRINTER, PrinterName, IniFile);
		pDevMode = (PDEVMODE)new CHAR[dlg.GetDevMode()->dmSize + dlg.GetDevMode()->dmDriverExtra];
		memcpy(pDevMode, dlg.GetDevMode(),dlg.GetDevMode()->dmSize + dlg.GetDevMode()->dmDriverExtra);
		SavePrinterDM(pDevMode);
	}
	else
	{
		if( LoadPrnDM(pDevMode) == FALSE )
		{
			PrinterName[0] = 0;
			goto prn_err_loc;
		}
	}

	Result = true;
	strcpy(pPrinterName, PrinterName);

end:
	if( Result == FALSE && pDevMode )
		delete [] (PCHAR)pDevMode;
	return Result;
}


void CloseDefaultPrinter(HANDLE &hPrinter)
{
	if(NULL != hPrinter)
		ClosePrinter(hPrinter);
}

extern "C"
BOOL
WINAPI
UWReport_Print(
ULONG   nTaskID,
PCHAR   pModulePath
 )
{
	char        PrinterName[MAX_PATH] = {0};
	PDEVMODE    pDevMode = NULL;

	HANDLE hPrintHandle = UWReport_Pre(nTaskID,pModulePath);
	if(hPrintHandle == NULL)
		return FALSE;

	//DEVMODE DevMode = {0};
	//
	//GetPrinterDevMode(DevMode);
	if( OpenDefaultPrinter(PrinterName, pDevMode) == FALSE )
		return FALSE;

	//BOOL bRet = THReportAPI_PrintByDevMode(hPrintHandle, pDevMode,PrinterName);		
	BOOL bRet = THReportAPI_PrintByDevMode(hPrintHandle, pDevMode,PrinterName);	
	THReportAPI_ClosePreHandle(hPrintHandle);
	delete [] (PCHAR)pDevMode;
	//delete pDevMode;

	return bRet;
}

 extern "C"
 BOOL
 WINAPI
 UWReport_PrintView(
 ULONG   nTaskID,
 PCHAR   pModulePath
 )
 {
	 char        PrinterName[MAX_PATH] = {0};
	 PDEVMODE    pDevMode = NULL;

	 HANDLE hPrintHandle = UWReport_Pre(nTaskID,pModulePath);

	 if(hPrintHandle == NULL)
		 return FALSE;

	 AFX_MANAGE_STATE(AfxGetStaticModuleState());
	 HINSTANCE   hInstOld; 	

	 CPrintViewDlg dlg;

	 if(hPrintHandle != NULL)
		 dlg.SetPrinterHandle(hPrintHandle);
	 else
		 return FALSE;

	 hInstOld = AfxGetResourceHandle();

	 AfxSetResourceHandle(theApp.m_hInstance); 

	 int nRet =dlg.DoModal();
	 BOOL bRet = FALSE;

	 if(nRet == IDOK)
	 {
		 //bRet = TRUE;
		/* DEVMODE DevMode = {0};

		 GetPrinterDevMode(DevMode);*/
		 if( OpenDefaultPrinter(PrinterName, pDevMode) == TRUE )
		 {
			  bRet = THReportAPI_PrintByDevMode(hPrintHandle, pDevMode,PrinterName);	
			  delete [] (PCHAR)pDevMode;
		 }
		 else
		 {
			 bRet = FALSE;
		 }				 	
	 }
	 THReportAPI_ClosePreHandle(hPrintHandle);
	 AfxSetResourceHandle(hInstOld); 
	 return bRet;
 }





extern "C"
BOOL
WINAPI
UWReport_GroupPrint(
PULONG      pTaskIDArray,
ULONG       nTaskCounter,
PCHAR       pModulePath,
PDEVMODE    pDevMode						
)
{
	BOOL                 bRet = TRUE;
	ULONG                nShellCounter, nUnitCounterPreGroup, nPageCounter;
	HANDLE               hPre, hPrintHandle;
	CUWVAr               UW;
	PSHELL_VAR_INFO      pShellVarInfo = NULL;
	if( pTaskIDArray == NULL || nTaskCounter == 0 || pModulePath == NULL )
	{
		DBG_MSG("UWReport_GroupPrintPre->Invaild Par...\n");
		return FALSE;
	}

	if( UW.BuildVarArrayforGroupPrint(pTaskIDArray, nTaskCounter, pShellVarInfo, nShellCounter, nUnitCounterPreGroup) == FALSE )
		return NULL;

	hPre = THReportAPI_PreStepByGroup(pModulePath, nTaskCounter, nUnitCounterPreGroup, pShellVarInfo, nShellCounter, FALSE);
	UW.FreeVarArray(pShellVarInfo);
	if( hPre == NULL )
		return FALSE; 

	nPageCounter = THReportAPI_GetPrintPageCountByGroup(hPre);
	if( nPageCounter == 0 )
		return FALSE;

	for( ULONG j  = 0 ; j < nPageCounter ; j ++ )
	{
		hPrintHandle = THReportAPI_GetPrintHandleByGroup(hPre, j);
		if( hPrintHandle == NULL )
		{
			bRet = FALSE;
			break;
		}

		bRet = THReportAPI_PrintByDevMode(hPrintHandle, pDevMode);
		if( bRet == FALSE )
			break;
	}

	THReportAPI_CloseHandleByGroup(hPre);
	return bRet;

}


static
HANDLE
UWReport_StartQCProcess(
PCHAR  pResIniPath,
PCHAR pQCPath
							 )
{
	char                 cmd_line[1024], ini_path[MAX_PATH], LGProcPath[MAX_PATH];
	PCHAR                pEmfPath;
	BOOL                 bRet = FALSE;
	STARTUPINFOA         si = {0};
	PROCESS_INFORMATION  pi = {0};


	strcpy(ini_path, pQCPath);
	PathRemoveExtension(ini_path);
	PathAddExtension(ini_path, ".ini");
	WritePrivateProfileString(LG_SECTION_NAME, QC_DATA_KEY, pQCPath, ini_path);
	WritePrivateProfileString(LG_SECTION_NAME, LG_RES_INI_PATH_KEY, pResIniPath, ini_path);

	GetModuleFileName(NULL, LGProcPath, MAX_PATH);
	PathRemoveFileSpec(LGProcPath);
	PathAppend(LGProcPath, "THLG.exe");

	si.cb = sizeof(si);
	sprintf(cmd_line, "%s %s", LGProcPath, ini_path);
	bRet  = CreateProcess(NULL, cmd_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if( bRet == TRUE )
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		bRet = FALSE;
		GetExitCodeProcess(pi.hProcess, (PULONG)&bRet);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		GetPrivateProfileString(LG_SECTION_NAME,NEW_EMF_PATH_KEY,NULL,LGProcPath, MAX_PATH, ini_path);
		if( bRet == FALSE )
			DeleteFile(LGProcPath);
		else
		{
			if( bRet == FALSE )
				DeleteFile(LGProcPath);
		}
	}

	//DeleteFile(ini_path);
	//	DeleteFile(pQCPath);

	if( bRet == FALSE )
		return NULL;

	pEmfPath = (PCHAR)malloc(strlen(LGProcPath) + 1);
	if( pEmfPath == NULL )
		DeleteFile(LGProcPath);
	else
		strcpy(pEmfPath, LGProcPath);

	return pEmfPath;
}

extern "C"
HANDLE
WINAPI
UWReport_LGReportPre(
PULONG      pTaskIDArray,
ULONG       nTaskCounter,
PCHAR       pIniPath,
int         nLGType,
USHORT      nCellType
)
{
	char                  QCPath[MAX_PATH] = "";
	HANDLE                hPreHandle = NULL;
	KSTATUS               status;
	CUWQCVar              qcUW;

	if( pTaskIDArray== NULL || nTaskCounter == 0 || pIniPath == NULL || PathFileExists(pIniPath) == FALSE || (nCellType != CELL_RED_TYPE && nCellType != CELL_WHITE_TYPE) )
	{
		DBG_MSG("THReportAPI_UWLGReportPre...Invaild Par\n");
		DBG_MSG(" nTaskCounter = %x, pIniPath = %x->%s,nCellType = %x\n", nTaskCounter, pIniPath, pIniPath, nCellType);
		return NULL;
	}

	DBG_MSG("UWReport_LGReportPre->nLGType = %u\n", nLGType);


	status = qcUW.CreateQCDataFile(pTaskIDArray,nTaskCounter, nLGType, QCPath,nCellType);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("CreateQCDataFile Fail...\n");
		return NULL;
	}

	hPreHandle = UWReport_StartQCProcess(pIniPath, QCPath);
	if( hPreHandle == NULL )
		DBG_MSG("hPreHandle Fail...\n");

	return hPreHandle;


}

