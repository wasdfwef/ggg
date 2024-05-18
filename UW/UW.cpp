
// UW.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "UW.h"
#include "MainFrm.h"
#include "DbgMsg.h"
#include "UWDoc.h"
#include "UWView.h"
#include "AutoFocusMgr.h"
#include "..\..\..\inc\CConfigSet.h"
#include "..\..\..\inc\AutoFocus.h"
#include <Shlwapi.h>
//#include "DogInfo.h"
#include "TipDlg.h"
#include "ProcessManager.h"
#include "minidump.h" 
#include <future>
#include <chrono>
#include "Include/Common/String.h"
#include <sapi.h>

#include "..\..\..\inc\Recognition.h"
#include "..\..\..\inc\SuppliesModule.h"

using namespace ADODB;
#import "ado\msado15.dll" rename("EOF","EndOfFile")
#import "ado\MSJRO.DLL" no_namespace rename("ReplicaTypeEnum", "_ReplicaTypeEnum")

#include "websocket/websoket-client.h"

#pragma comment(lib, "..\\..\\..\\lib\\SuppliesModule.lib")
#pragma comment(lib, "..\\..\\..\\lib\\Recognition.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "..\\..\\..\\lib\\CConfigSet.lib")
#pragma comment(lib, "..\\..\\..\\lib\\AutoFocus.lib")

#include "..\..\inc\shareData.h"
#pragma comment(lib, "..\\lib\\ShareData.lib")

using namespace Common;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#ifdef UNICODE
//#define GetIdsString  GetIdsStringW
//#else
//#define GetIdsString GetIdsStringA
//#endif 

// CUWApp

extern "C"
{
#include "UdcCriterionDlg.h"
}

// 添加MessageBoxTimeout支持
extern "C"
{
	int WINAPI MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
	int WINAPI MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
};

#ifdef UNICODE
#define MessageBoxTimeout MessageBoxTimeoutW
#else
#define MessageBoxTimeout MessageBoxTimeoutA
#endif

// 清空文件夹
BOOL ClearPath(TCHAR *dir);
// 压缩mdb数据库
HRESULT CompactDatabase(const char * dbPath);

void RecursiveDeleteDir(CString szPath, BOOL isDelSelf = TRUE)
{
	CFileFind filefind;
	CString path = szPath;
	if (path.Right(1) != "\\")//目录的最右边需要“\”字符
		path += "\\";
	path += "*.*";
	BOOL res = filefind.FindFile(path);
	while (res)
	{
		res = filefind.FindNextFile();
		//是文件时直接删除
		if (!filefind.IsDots() && !filefind.IsDirectory())
			DeleteFile(filefind.GetFilePath());//删除文件
		else if (filefind.IsDots())
			continue;
		else if (filefind.IsDirectory())//为目录
		{
			path = filefind.GetFilePath();
			//是目录时继续递归，删除该目录下的文件
			RecursiveDeleteDir(path);
			//目录为空后删除目录
			RemoveDirectory((LPCTSTR)path);
		}
	}
	if (isDelSelf)
	{
		RemoveDirectory(szPath);
	}
}

BEGIN_MESSAGE_MAP(CUWApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CUWApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
//	ON_COMMAND(ID_MENU_SET, &CUWApp::OnMenuSet)
END_MESSAGE_MAP()


// CUWApp 构造

#ifdef UNICODE
std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId)
{
	TCHAR sModuleFile[MAX_PATH]={0};
	TCHAR sDrive[MAX_PATH]={0};
	TCHAR sDir[MAX_PATH]={0};

	GetModuleFileName(NULL, sModuleFile, MAX_PATH);

	_tsplitpath(sModuleFile, sDrive, sDir, NULL,NULL);

	TCHAR s_inifile[MAX_PATH]={0};

    wsprintf(s_inifile, _T("%s%s%d.ini"), sDrive, sDir, 2052);
	
	std::wstring str;
	TCHAR szLine[MAX_PATH*8] = {0};

	if( 0 !=GetPrivateProfileString(sMain.c_str(),
		sId.c_str(),
		_T("0"),
		szLine,
		MAX_PATH*8,
		s_inifile))
	{
		str = szLine;
	}
	else
	{
		str =_T("");
	}

	return str;
}
bool SetIdsString(const std::wstring &sMain, const std::wstring &sId, const std::wstring &sVal)
{
	TCHAR sModuleFile[MAX_PATH]={0};
	TCHAR sDrive[MAX_PATH]={0};
	TCHAR sDir[MAX_PATH]={0};

	GetModuleFileName(NULL, sModuleFile, MAX_PATH);
	_wsplitpath(sModuleFile, sDrive, sDir, NULL,NULL);

	TCHAR s_inifile[MAX_PATH]={0};
	wsprintf(s_inifile,_T("%s%s%d.ini"),sDrive,sDir,2052);

	if( TRUE !=WritePrivateProfileString(sMain.c_str(),
		sId.c_str(),
		sVal.c_str(),
		s_inifile))
	{
		return false;
	}
	else
	{
		return true;
	}
}
#else
std::string GetIdsString(const std::string &sMain,const std::string &sId)
{
	char sModuleFile[MAX_PATH]={0};
	char sDrive[MAX_PATH]={0};
	char sDir[MAX_PATH]={0};

	GetModuleFileName(NULL, sModuleFile, MAX_PATH);

	_splitpath(sModuleFile, sDrive, sDir, NULL,NULL);

	char s_inifile[MAX_PATH]={0};

	sprintf_s(s_inifile,"%s%s%d.ini",sDrive,sDir,2052);


	std::string str;
	TCHAR szLine[MAX_PATH*8] = {0};

	if( 0 !=GetPrivateProfileString(sMain.c_str(),
		sId.c_str(),
		_T("0"),
		szLine,
		MAX_PATH*8,
		s_inifile))
	{
		str = szLine;
	}
	else
	{
		str = "";
	}

	return str;
}
#endif 



CUWApp::CUWApp()
	:m_lsx(*CPipelineManager::GetInstance())
{
	m_bHiColorIcons = TRUE;
	m_nUdcItem = 0;
	m_startDev = true;
	m_nSN = 1;
	m_nTubeShelfNum = 0;
	m_SaveCount = ReadSaveNums();
	m_IsAutoModifyLEU = false;
	m_IsFirst = true;
	m_bNeesAutoFocus = false;
	m_bCanAutoFocus  = false;
	m_bAutoCheckFail = true;
	m_bInDebugMode   = false;
	m_bIsInAutoFocus = false;	
	m_bSaveDate =false;
	m_LastClearTimes = 0;
	theApp.nAttribute = TestNormal;

	memset(m_chClassIniPath,0,MAX_PATH);
	memset(m_szExeFolder,0,MAX_PATH);
	memset(m_szExeFolderA,0,MAX_PATH);
	memset(udcOrder, 0, sizeof(udcOrder));
	GetLocalTime(&m_CurrentDate);

	m_bLCResetSuccess = false;
	m_nUdcItemCount = 12;
	//m_nUdcItemCount = 14; //临时修改20240223  原始值m_nUdcItemCount = 12;
	setTestType = false;
	continueResetZ = false;
	m_bIsLiushuixain = false;

	m_isVariableRatioSpecialDeal = TRUE;

	m_bIsDoubleRec = FALSE;

	g_vecHoleForSet.resize(11);

    memset(&m_qcTypeList, 0, sizeof(m_qcTypeList));
}

// 唯一的一个 CUWApp 对象

CUWApp theApp;


// CUWApp 初始化


BOOL CUWApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	 


	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	InitMinDump();


	CString ErrorMsg = GetIdsString( _T("warning"), _T("appisrunning") ).c_str();
	// 只允许一个进程运行
	if( CreateMutexA(NULL, FALSE, UW2000_ISRUNNING_MUTEX) == NULL || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		AfxMessageBox(ErrorMsg);
		return FALSE;
	}

	auto f = std::async(std::launch::async, [&]{
		this_thread::sleep_for(chrono::seconds(8));
		// 启动识别软件
		LoadRecProcess();
	});
	
	
	char FlagFilePath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, FlagFilePath, MAX_PATH);
	PathRemoveFileSpecA(FlagFilePath);
	PathAppendA(FlagFilePath, "leu_flag.dat");
	m_IsAutoModifyLEU = ( TRUE == PathFileExistsA(FlagFilePath) );
	// 获取ini路径
	GetModuleFileName(NULL, m_szExeFolder, MAX_PATH);
	PathRemoveFileSpec(m_szExeFolder);
	WideCharToMultiByte (CP_ACP, NULL, (LPCWSTR)m_szExeFolder, -1, m_szExeFolderA, MAX_PATH, NULL, FALSE);
	_tcscpy(m_chClassIniPath, m_szExeFolder);
	PathAppend(m_chClassIniPath, CLASS_INI_NAME);

    // 获取UDC标定
	GetUDCSettingIniPath();
	InitUdcini();

	GetAllInfo(hospitalInfo);
	GetColorInfo(colorInfo, colorNum);
	GetClearInfo(clearInfo, clearNum);
	GetDiagnosInfo(diagnosInfo, diagnosNum);

	// 获取每一孔位设置测试类型
	GetTestTypeHoleInfo();

	SetConfigIniPath(NULL);
	if( GetCellConfig(TRUE) == FALSE )
		return FALSE;
	if( GetCellConfig(FALSE) == FALSE )
		return FALSE;

	// 初始化耗材信息
	GetConsumableControl();
	if (m_bIsControlConsumable)
	{
		if (Interface_IcCardInit() == false)//耗材卡初始化
		{
			AfxMessageBox(_T("耗材卡初始化读取失败！"));
			return FALSE;
		}
	}

	GetBD();
	GetLisType();
	GetUDCOrder();
	GetUsConInfo(&m_usInfo);
	GetUdcConInfo(&m_udcInfo);

    // 检查是否需要备份和更新数据库
    bool dbOpted = DoDbbackupAndcleanup();
	// 每次启动程序时，修复和压缩数据库
    if (!dbOpted) CompactDatabase("db\\uw.mdb");
	//数据库损坏就把损坏数据库备份，再拷贝上次开机备份的数据库过来，如果仍然有错就从backup拷贝过来
	// 打开数据库
	KSTATUS status = Access_OpenAccess(FALSE);
	if (status != STATUS_SUCCESS)
	{
		/*CopyDBToBackup(true);
		CopyBackupToDB();
		status = Access_OpenAccess(FALSE);
		if (status != STATUS_SUCCESS)
		{
			CopyBackupToDB(true);
			status = Access_OpenAccess(FALSE);
			if (status != STATUS_SUCCESS)
			{
				AfxMessageBox(_T("数据库初始化失败！"));
				return FALSE;
			}
		}*/
        CopyBackupToDB(false);
        status = Access_OpenAccess(FALSE);
        if (status != STATUS_SUCCESS)
        {
            CopyBackupToDB(true);
            status = Access_OpenAccess(FALSE);
            if (status != STATUS_SUCCESS)
            {
                AfxMessageBox(_T("数据库初始化失败！"));
                return FALSE;
            }
        }
	}	

    if (!dbOpted) CheckDbbackupAndcleanup();

	Access_GetUserList(&m_userlist);
	m_nUserNum = m_userlist.nUserCounter;

	// 流水线配置
	GetLiushuixianCfg();
	if (!m_bIsLiushuixain)
	{
		if (userdlg.DoModal() != IDOK)
		{
			return FALSE;
		}
		else
		{
			if (userdlg.m_group == 111)
			{
				loginGroup = 111;
			}
			else
			{
				//memcpy(&loginUser,&(userdlg.m_user),sizeof(loginUser));
				loginGroup = userdlg.m_group;
			}
		}
	}
	else
	{
		loginGroup = 111;
	}


    
    

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(GetIdsString(_T("app"), _T("name")).c_str());
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CUWDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CUWView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	CleanState(_T("WorkSpace"));

	wstring Version = GetIdsString(_T("app"), _T("version"));;
#if defined(OEM_NY)
	void(0);
#elif   (_LC_V == 2150)
	Version = _T("V2.1.5");
#elif (_LC_V == 2180)
	Version = _T("V2.1.8");
#elif (_LC_V == 2190)
	Version = _T("V2.1.9");
#elif (_LC_V == 2200)
	Version = _T("V2.2.0");
#elif (_LC_V == 2230)
	Version = _T("V2.2.3");
#elif (_LC_V == 2240)
	Version = _T("V2.2.4");
#elif (_LC_V == 2250)
	Version = _T("V2.2.5");
#elif (_LC_V == 2260)
	Version = _T("V2.2.6");
#elif (_LC_V == 2270)
	Version = _T("V2.2.7");
#elif (_LC_V == 2280)
	Version = _T("V2.2.8");
#elif (_LC_V == 2290)
	Version = _T("V2.2.9");
#elif (_LC_V == 2300)
	Version = _T("V2.3.0");
#elif (_LC_V == 2310)
	Version = _T("V2.3.1");
#elif (_LC_V == 2320)
	Version = _T("V2.3.2");
#endif

	AfxGetMainWnd()->SetWindowText(GetIdsString(_T("app"), _T("name")).append(_T(" ")).append(Version).c_str());

	//::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TODAYLIST_UPDATELISTPOS, (WPARAM)2, (LPARAM)0);

	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	return TRUE;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateMenuSet(CCmdUI *pCmdUI);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)

	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CUWApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CUWApp 自定义加载/保存方法

void CUWApp::PreLoadState()
{
	GetContextMenuManager()->AddMenu(GetIdsString(_T("menu_popup"),_T("title")).c_str(), IDR_POPUP_OPERATE);
	GetContextMenuManager()->AddMenu(GetIdsString(_T("record"),_T("title")).c_str(), IDR_FIND_RECORD);
	//GetContextMenuManager()->AddMenu(GetIdsString(_T("menu_edit"),_T("title")).c_str(), IDR_POPUP_CLASS);
	GetContextMenuManager()->AddMenu(GetIdsString(_T("pic"),_T("title")).c_str(), IDR_PRINT_PIC);
	GetContextMenuManager()->AddMenu(GetIdsString(_T("us_mic_config"),_T("title")).c_str(), IDR_MENU_US_MIC_CONFIG);
}

void CUWApp::LoadCustomState()
{
	//设置为TRUE，则自动加载界面记忆
	this->m_bSaveState = FALSE;
}

void CUWApp::SaveCustomState()
{
	//设置为TRUE，则自动加载界面记忆
	this->m_bSaveState = FALSE;
}

// CUWApp 消息处理程序

int CUWApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	Access_FreeGetUserListBuf(m_userlist);
	Access_CloseAccess();

	return CWinAppEx::ExitInstance();
}


#define  CONFIG_NAME			"Conf.ini"
#define  SECTION_SHOW_OTHER		"th_us"
#define  KEY_SHOW_OTHER			"bIsShowOther"
#define  SHOW_OTHER				"1"

bool CUWApp::ShowOtherType()
{
	char Value[20] = {0};
	char ConfigPath[MAX_PATH] = {0};

	GetModuleFileNameA( NULL, ConfigPath, sizeof(ConfigPath) );
	PathRemoveFileSpecA(ConfigPath);
	PathAppendA(ConfigPath, CONFIG_NAME);

	GetPrivateProfileStringA(SECTION_SHOW_OTHER, KEY_SHOW_OTHER, SHOW_OTHER, Value, sizeof(Value), ConfigPath);
	if ( 0 == stricmp(Value, SHOW_OTHER) )
		return true;

	return false;
}

//#include "AutoFocusMgr.h"

BOOL CUWApp::StartUW2KFramework()
{
	DBG_MSG("启动框架\n");
	if (THInterface_Initialize() != STATUS_SUCCESS)
	{
		MessageBoxTimeout(AfxGetMainWnd()->GetSafeHwnd(), _T("框架启动错误"), _T("错误"), MB_ICONERROR|MB_OK, 0, 3*1000);
		return FALSE;
	}
	DBG_MSG("启动框架完成\n");
	if(THInterface_StartDev()!=STATUS_SUCCESS)
	{
        MessageBoxTimeout(AfxGetMainWnd()->GetSafeHwnd(), _T("框架设备启动错误"), _T("错误"), MB_ICONERROR | MB_OK, 0, 3 * 1000);
		return TRUE;
	}

	ULONG              nStartCounter;
	KSTATUS            status;
	TDEVICE_DESCRITOR  dev[] = {
		T_DEV_LC_TYPE,
		T_DEV_REC_TYPE,
		T_DEV_CAMERA_TYPE,	
		//T_DEV_UDC_TYPE, //新UW2000不需要单独的UDC软件模块
		T_DEV_LIS_TYPE
	};


	for( int j = 0 ; j < (sizeof(dev) / sizeof(TDEVICE_DESCRITOR)) ; j++ )
	{
		nStartCounter = 0;
		status = THInterface_QueryDevCaps(dev[j], StartDevNumber, NULL, 0, (PCHAR)&nStartCounter, sizeof(nStartCounter), 0);
		if( nStartCounter == 0 )
		{
			switch(dev[j])
			{
			case T_DEV_LC_TYPE:
				{
					m_bAutoCheckFail = true;
                    MessageBoxTimeout(0, _T("下位机启动失败"), _T("消息"), MB_OK, 0, 5 * 1000);
					break;
				}
			case T_DEV_REC_TYPE:
				{
					m_bAutoCheckFail = true;
                    MessageBoxTimeout(0, _T("识别设备启动失败"), _T("消息"), MB_OK, 0, 5 * 1000);
					break;
				}
			case T_DEV_CAMERA_TYPE:
				{
					m_bAutoCheckFail = true;
                    MessageBoxTimeout(0, _T("摄像头设备启动失败"), _T("消息"), MB_OK, 0, 5 * 1000);
					break;
				}
			case T_DEV_UDC_TYPE:
				{
                    MessageBoxTimeout(0, _T("UDC设备启动失败"), _T("消息"), MB_OK, 0, 5 * 1000);
					break;
				}
			case T_DEV_LIS_TYPE:
				{
                    if (!m_bIsLiushuixain)
                        MessageBoxTimeout(0, _T("Lis设备启动失败"), _T("消息"), MB_OK, 0, 5 * 1000);
					break;
				}

			default:
				{
					break;
				}
			}

			//return FALSE;
		}
	}
    theApp.UpdateCheckType();
	theApp.g_nSettingType1_type = theApp.GetCheckType();

	// 启动下位机消息分发器
	m_clsMsgDipatcher.Start();

	return TRUE;
}

ULONG CUWApp::MakeStatusMaskCode(StatusEnum  nUSStatus, StatusEnum  nUDCStatus)
{
	return  (nUSStatus | (nUDCStatus << 4));
}

bool CUWApp::GetSetTestType()
{
	return setTestType;
}


void CUWApp::GetStatusAtNew( StatusEnum & nUSStatus, StatusEnum & nUDCStatus, int nTestType, BOOL bTaskResult)
{
	DBG_MSG(" CUWApp::nTestType = %d\n", nTestType);
	
	if( nTestType == UDC_TEST_TYPE )
	{
		nUSStatus  = NOTHING;
		nUDCStatus = NEW;
	}
	else if( nTestType == US_TEST_TYPE )
	{
		nUSStatus  = NEW;
		nUDCStatus = NOTHING;
	}
	else
	{
		nUDCStatus = nUSStatus = NEW;
	}

	if( bTaskResult == TRUE )
		return;

	if( nUSStatus != NOTHING )
		nUSStatus = FAIL;
	if( nUDCStatus != NOTHING )
		nUSStatus = FAIL;

}

void CUWApp::ParseStatusMaskCode( ULONG nStatusMaskCode, StatusEnum & nUSStatus, StatusEnum & nUDCStatus )
{
	nUSStatus = (StatusEnum)(nStatusMaskCode & 0xf);
	nUDCStatus = (StatusEnum)((nStatusMaskCode >> 4) & 0xf);
}

BOOL CUWApp::GetCellConfig( BOOL bUS )
{
	CONFIG_RECORD_INFO   cfg_rec = {0};

	if( bUS )
	{
		cfg_rec.nRecordType = US_TYPE_INFO;
		cfg_rec.pConfigConent = &us_cfg_info;
		cfg_rec.nConfigLength = sizeof(us_cfg_info);
	}
	else
	{
		cfg_rec.nRecordType = UDC_TYPE_INFO;
		cfg_rec.pConfigConent = &udc_cfg_info;
		cfg_rec.nConfigLength = sizeof(udc_cfg_info);
	}

	KSTATUS  status = QueryRecordConfig(&cfg_rec);

	if( status != STATUS_SUCCESS )
		return FALSE;

	return TRUE;
}

void CUWApp::GetBD()
{
	CHAR  Path[MAX_PATH], Value[90];

	strcpy_s(Path,m_szExeFolderA);
	PathAppendA(Path, ("conf.ini"));

	GetPrivateProfileStringA("calibrate", "value", "1.00", Value,90, Path);
	m_b = (float)atof(Value);
	m_isVariableRatioSpecialDeal = (BOOL)GetPrivateProfileIntA("calibrate", "specialDeal", 1, Path);
}

void CUWApp::GetConsumableControl()
{
	CHAR  Path[MAX_PATH];
	UINT flag = 0;
#if CONTRALCLEAN == 1 
	/*strcpy_s(Path, m_szExeFolderA);
	PathAppendA(Path, "config\\setting.ini");
	flag = GetPrivateProfileIntA("consumables", "control", 1, Path);
	m_bIsControlConsumable = (flag != 0);*/
	m_bIsControlConsumable = true;
#else
	m_bIsControlConsumable = false;
#endif

	if (PathFileExistsA(".\\Config\\debug.ok"))
	{
		m_bIsControlConsumable = false;
	}
}

void CUWApp::GetLiushuixianCfg()
{
    DBG_MSG("Enter %s", __FUNCTION__);
	char cfgFilePath[MAX_PATH] = { 0 }, netioFilePath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, cfgFilePath, MAX_PATH);
	PathRemoveFileSpecA(cfgFilePath);
	strcpy_s(netioFilePath, cfgFilePath);
	PathAppendA(cfgFilePath, "conf.ini");
	PathAppendA(netioFilePath, "config\\netio.ini");

	
	UINT nLiushuixian = GetPrivateProfileIntA("Liushuixian", "isLiushuixian", 0, cfgFilePath);
	if (nLiushuixian != 0)
	{
		m_bIsLiushuixain = true;

		m_nDeviceNo = GetPrivateProfileIntA("machine", "deviceNo", 1, netioFilePath);
		//获取流水线网络配置
		char host[64] = { 0 };
		memset(&m_stDevPipelineInfo, 0, sizeof(m_stDevPipelineInfo));
		GetPrivateProfileStringA("local", "host", "0.0.0.0", host, HOST_LEN, netioFilePath);
		m_stDevPipelineInfo.local.port = GetPrivateProfileIntA("local", "port", 30000, netioFilePath);
		strcpy_s(m_stDevPipelineInfo.local.host, HOST_LEN, host);
		GetPrivateProfileStringA("remote", "host", "127.0.0.1", host, HOST_LEN, netioFilePath);
		m_stDevPipelineInfo.remote.port = GetPrivateProfileIntA("remote", "port", 60000, netioFilePath);
		m_stDevPipelineInfo.remote.port2 = GetPrivateProfileIntA("remote", "port2", 13600, netioFilePath);
		strcpy_s(m_stDevPipelineInfo.remote.host, HOST_LEN, host);
		GetPrivateProfileStringA("remote", "imagePath", "", m_stDevPipelineInfo.remote.imagePath, MAX_PATH, netioFilePath);

        auto f = std::async(std::launch::async, [&](const string sharePath, int devNo){
            if (PathFileExistsA(sharePath.c_str()))
            {
                char path[MAX_PATH] = { 0 };
                _snprintf_s(path, sizeof(path) - 1, "%s\\NY", sharePath.c_str());
                if (!PathFileExistsA(path))
                {
                    CreateDirectoryA(path, NULL);
                }
                _snprintf_s(path, sizeof(path) - 1, "%s\\NY\\%d", sharePath.c_str(), devNo);
                if (!PathFileExistsA(path))
                {
                    CreateDirectoryA(path, NULL);
                }
            }
        }, m_stDevPipelineInfo.remote.imagePath, m_nDeviceNo);

        DBG_MSG("%s  CreateTcpServer-%d", __FUNCTION__, m_stDevPipelineInfo.local.port);
		// 创建服务端
		m_lsx.CreateTcpServer(m_stDevPipelineInfo.local.host, m_stDevPipelineInfo.local.port);

		// 创建websoket客户端
		CreateWebSocketCli(m_stDevPipelineInfo.remote.host, m_stDevPipelineInfo.remote.port2, "thme", (FlowlineClientInterface*)this);

        DBG_MSG("Leave %s", __FUNCTION__);
	}
}

void CUWApp::WriteSNNums(int nNums)
{
	CString sData;
	sData.Format(_T("%d"),nNums);
	WritePrivateProfileString(_T("sn"),_T("num"),sData,m_chClassIniPath);
}

int CUWApp::ReadSNNums()
{
	int nNums = 0;

	TCHAR szLine[MAX_PATH] = {0};

	if( 0 !=GetPrivateProfileString(_T("sn"),_T("num"),_T("0"),szLine,MAX_PATH,m_chClassIniPath))
	{
		nNums = _tstoi(szLine);
	}
	else
	{
		nNums = 1;                                                                                                                    
	}

	return nNums;
}

void CUWApp::WriteTubeShelfNums(int nNums)
{
	CString sData;
	sData.Format(_T("%d"),nNums);
	WritePrivateProfileString(_T("row"),_T("num"),sData,m_chClassIniPath);
}

int CUWApp::ReadTubeShelfNums()
{
	int nNums = 0;

	TCHAR szLine[MAX_PATH] = {0};

	if( 0 !=GetPrivateProfileString(_T("row"),_T("num"),_T("0"),szLine,MAX_PATH,m_chClassIniPath))
	{
		nNums = _tstoi(szLine);
	}
	else
	{
		nNums = 1;                                                                                                                    
	}

	return nNums;
}

void CUWApp::WriteSaveNums(int nNums)
{
	CString sData;
	sData.Format(_T("%d"),nNums);
	WritePrivateProfileString(_T("save"),_T("num"),sData,m_chClassIniPath);
}


void CUWApp::WriteSendLisSign(int IsAutoSendToLis)
{
	CString sData;
	sData.Format(_T("%d"),IsAutoSendToLis);
	WritePrivateProfileString(_T("sendtolis"),_T("auto"),sData,m_chClassIniPath);
}


int CUWApp::ReadSendLisSign()
{
	int AutoSendToLis = 0;
	TCHAR szLine[MAX_PATH] = {0};

	if( 0 !=GetPrivateProfileString(_T("sendtolis"),_T("auto"),_T("0"),szLine,MAX_PATH,m_chClassIniPath))
	{
		AutoSendToLis = _tstoi(szLine);
	}
	else
	{
		AutoSendToLis = 0;                                                                                                                    
	}

	return AutoSendToLis;
}

int CUWApp::ReadSaveNums()
{
	int nNums = 0;

	TCHAR szLine[MAX_PATH] = {0};

	if( 0 !=GetPrivateProfileString(_T("save"),_T("num"),_T("0"),szLine,MAX_PATH,m_chClassIniPath))
	{
		nNums = _tstoi(szLine);
	}
	else
	{
		nNums = 0;                                                                                                                    
	}

	return nNums;
}

void CUWApp::GetConfigVisioLineAndColumn(UW2000_VISIO_LINE_COLUMN* pUw2000visiolinecolumn)
{
	CString sDbgString;
	CString sApp;
	sApp.Format(_T("visio%d"),pUw2000visiolinecolumn->nChannel);
	TCHAR szLine[MAX_PATH] = {0};

	if( 0 !=GetPrivateProfileString(sApp,_T("lines"),_T("0"),szLine,MAX_PATH,m_chClassIniPath))
	{
		pUw2000visiolinecolumn->nLines = _tstoi(szLine);
	}
	else
	{
		pUw2000visiolinecolumn->nLines = 0;                                                                                                                    
	}

	memset(szLine,0,sizeof(TCHAR)*MAX_PATH);

	if( 0 !=GetPrivateProfileString(sApp,_T("column"),_T("0"),szLine,MAX_PATH,m_chClassIniPath))
	{
		pUw2000visiolinecolumn->nColumn = _tstoi(szLine);
	}
	else
	{
		pUw2000visiolinecolumn->nColumn = 0;
	}
}

void CUWApp::WriteConfigVisioVaule(UW2000_VISIO_INFO info)
{
	CString sDbgString;
	CString sData;
	CString sName;
	CString sApp;

	sData.Format(_T("%d"),info.nSelect);
	sName.Format(_T("item%d"),info.nIndex);
	sApp.Format(_T("visio%d"),info.nChannel);

	if(WritePrivateProfileString(sApp,sName,sData,m_chClassIniPath) == FALSE)
	{
		sDbgString.Format(_T("WriteInfoToFile:Err 0x%x\n"),GetLastError());
		OutputDebugString(sDbgString);
	}
}

void CUWApp::GetUDCOrder()
{
	char confPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, confPath, MAX_PATH);
	PathRemoveFileSpecA(confPath);
	PathAppendA(confPath, "conf.ini");
	GetPrivateProfileStringA("th_udc", "order", 0, udcOrder, sizeof(udcOrder), confPath);//南京优源
}

void CUWApp::GetLisType()
{
	TCHAR INIPath[256] = { 0 };

	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("config\\Lis1.ini"));

	m_sendLisType = GetPrivateProfileInt(L"config", L"SendLISType", 2000, INIPath);
	m_sendNum =		GetPrivateProfileInt(L"config", L"SendNum", 1, INIPath);
	m_sendLisTimeInterval = GetPrivateProfileInt(L"config", L"TimeInterval ", 1000, INIPath);
}

void CUWApp::GetConfigVisioVaule(UW2000_VISIO_INFO* pUw2000visioinfo)
{
	CString sDbgString;
	CString sApp;
	CString sName;
	sName.Format(_T("item%d"),pUw2000visioinfo->nIndex);
	sApp.Format(_T("visio%d"),pUw2000visioinfo->nChannel);

	TCHAR szLine[MAX_PATH] = {0};

	if( 0 !=GetPrivateProfileString(sApp,sName,_T("0"),szLine,MAX_PATH,m_chClassIniPath))
	{
		pUw2000visioinfo->nSelect = _tstoi(szLine);
	}
	else
	{
		pUw2000visioinfo->nSelect = 0;
	}
}

void CUWApp::GetUDCSettingIniPath()
{
	GetModuleFileNameA( NULL, m_UDCIni, sizeof(m_UDCIni) );
	PathRemoveFileSpecA(m_UDCIni);
	PathAppendA(m_UDCIni, "Config");
	PathAppendA(m_UDCIni, UDC_SETTING_INI);
}

UINT CUWApp::GetFocusStep()
{
	char ConfigFile[MAX_PATH] = {0};

	CAutoFocusMgr::GetSettingFile(ConfigFile);
	return GetPrivateProfileIntA("focus", "step", 50, ConfigFile);
}

#define START_SN 1

void CUWApp::UpdateSNByDate()
{
	SYSTEMTIME Today = {0};
	GetLocalTime(&Today);

	if (Today.wDay != m_CurrentDate.wDay || Today.wMonth != m_CurrentDate.wMonth || Today.wYear != m_CurrentDate.wYear)
	{
		m_nSN = START_SN;
		m_CurrentDate = Today;
	}
}

void CUWApp::WriteConfigVisioLineAndColumn(UW2000_VISIO_LINE_COLUMN info)
{
	CString sDbgString;
	CString sData;
	CString sApp;

	sData.Format(_T("%d"),info.nLines);
	sApp.Format(_T("visio%d"),info.nChannel);

	if(WritePrivateProfileString(sApp,_T("lines"),sData,m_chClassIniPath) == FALSE)
	{
		sDbgString.Format(_T("WriteInfoToFile:Err 0x%x\n"),GetLastError());
		OutputDebugString(sDbgString);
	}

	sData.Format(_T("%d"),info.nColumn);

	if(WritePrivateProfileString(sApp,_T("column"),sData,m_chClassIniPath) == FALSE)
	{
		sDbgString.Format(_T("WriteInfoToFile:Err 0x%x\n"),GetLastError());
		OutputDebugString(sDbgString);
	}
}

BOOL CAboutDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	wstring Version = GetIdsString(_T("app"), _T("version"));;
#if defined(OEM_NY)
	void(0);
#elif   (_LC_V == 2150)
	Version = _T("V2.1.5");
#elif (_LC_V == 2180)
	Version = _T("V2.1.8");
#elif (_LC_V == 2190)
	Version = _T("V2.1.9");
#elif (_LC_V == 2200)
	Version = _T("V2.2.0");
#elif (_LC_V == 2230)
	Version = _T("V2.2.3");
#elif (_LC_V == 2240)
	Version = _T("V2.2.4");
#elif (_LC_V == 2250)
	Version = _T("V2.2.5");
#elif (_LC_V == 2260)
	Version = _T("V2.2.6");
#elif (_LC_V == 2270)
	Version = _T("V2.2.7");
#elif (_LC_V == 2280)
    Version = _T("V2.2.8");
#elif (_LC_V == 2290)
    Version = _T("V2.2.9");
#endif

    ::SetDlgItemText(this->m_hWnd, IDC_SNAME, GetIdsString(_T("app"), _T("name")).c_str());
	::SetWindowText( ::GetDlgItem(this->m_hWnd, IDC_Version), Version.c_str() );
    if (Version.rfind(_T(".")) != string::npos)
    {
        ::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_Version2), Version.substr(0, Version.rfind(_T("."))).c_str());
    }
    else
    {
        ::SetWindowText(::GetDlgItem(this->m_hWnd, IDC_Version2), Version.c_str());
    }

	TCHAR path[MAX_PATH] = { 0 }, ai_version[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAppend(path, _T("AI_test\\config.ini"));
	GetPrivateProfileString(_T("VERSION"), _T("version_number"), _T(""), ai_version, MAX_PATH, path);
	SetDlgItemText(IDC_MODULE_AI, ai_version);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CUWApp::GetRecInitResult()
{
	char ConIniPath[MAX_PATH] = {0},temp[MAX_PATH] = {0};

	GetModuleFileNameA(NULL,ConIniPath,MAX_PATH);
	PathRemoveFileSpecA(ConIniPath);
	PathAppendA(ConIniPath,"config\\ImgRec.ini");
	
	GetPrivateProfileStringA("USRec","Load","False",temp,MAX_PATH,ConIniPath);
	return  ( strcmp(temp,"True") == 0 );
}

int CUWApp::GetModeIndex()
{
	CHAR SettingIniPath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL,SettingIniPath,MAX_PATH);
	PathRemoveFileSpecA(SettingIniPath);
	PathAppendA(SettingIniPath,"Config//setting.ini");

	return GetPrivateProfileIntA("RecCfg","Mode",0,SettingIniPath);
}

BOOL CUWApp::LoadRecProcess()
{
	DBG_MSG("%s  begin", __FUNCTION__);
	TCHAR        BasePath[MAX_PATH], ConfPath[MAX_PATH], TipExePath[MAX_PATH], RecPicPath[MAX_PATH], RecPicPath2[MAX_PATH];
	BOOL         bRet;

	GetModuleFileName(NULL, BasePath, MAX_PATH);
	PathRemoveFileSpec(BasePath);

	_tcscpy_s(ConfPath, BasePath);
	PathAppend(ConfPath, _T("Conf.ini"));
	m_bIsDoubleRec = (BOOL)GetPrivateProfileInt(_T("sys"), _T("isDoubleRec"), 0, ConfPath);

	_tcscpy_s(RecPicPath, BasePath);
	PathAppend(RecPicPath, _T(REC_PATH));
	_tcscpy_s(RecPicPath2, BasePath);
	PathAppend(RecPicPath2, _T(REC_PATH2));

	TCHAR DrvCfgPath[MAX_PATH], ExePath[MAX_PATH];
	_tcscpy_s(DrvCfgPath, BasePath);
	PathAppend(DrvCfgPath, _T("Config\\DrvCfg.ini"));
	GetPrivateProfileString(_T("RecExePath"), _T("0"), _T("NY_DetectService.exe"), ExePath, MAX_PATH, DrvCfgPath);
	if (_tcslen(ExePath) == 0)
	{
		_tcscpy_s(ExePath, _T("NY_DetectService.exe"));
	}
	else
	{
		char s1[256] = { 0 };
		strcpy(s1, PathFindFileNameA(WstringToString(ExePath).c_str()));
		PathRemoveExtensionA(s1);
		// 启动前设置共享内存信息
		SYSTEMTIME systime = {};
		GetLocalTime(&systime);
		char txt[32] = { 0 }, key[32] = { 0 };
		sprintf_s(txt, "THME_NY_%04d-%02d-%02d", systime.wYear, systime.wMonth, systime.wDay);
		sprintf_s(key, "THME%02d%02d", systime.wMonth, systime.wDay);
		string strMd5 = GetMd5(txt);
		//DBG_MSG("%s\n", txt);
		//DBG_MSG("%s\n", key);
		//DBG_MSG("%s\n", strMd5.c_str());
		int len = strMd5.size();
		char temp[8] = { 0 };
		std::string encrypt;
		for (int i = 0; i < len; i += 8)
		{
			memcpy(temp, strMd5.data() + i, 8);
			encrypt.append(text_encrypt(temp, key, 1));
		}
		int mod = len % 8;
		if (mod != 0)
		{
			memset(temp, 0, 8);
			memcpy(temp, strMd5.data() + (len - mod), mod);
			encrypt.append(text_encrypt(temp, key, 1));
		}
		//DBG_MSG("%s\n", encrypt.c_str());
		InitSharedMemery(encrypt.data());
	}

	_tcscpy_s(TipExePath, BasePath);
	PathAppend(TipExePath, ExePath);

	int pos = CString(ExePath).ReverseFind('\\');
	if (pos == -1) pos = CString(ExePath).ReverseFind('/');
	CString ProcessName = CString(ExePath).Right(CString(ExePath).GetLength() - pos - 1);

	// 清空识别临时目录
	ClearPath(RecPicPath);
	ClearPath(RecPicPath2);
	// 终止以前启动的进程
	CProcessManager::CloseProcess(ProcessName);
	// 启动新的进程

	//STARTUPINFO si = { 0 };
	//PROCESS_INFORMATION  pi = { 0 };
	//si.cb = sizeof(si);
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_HIDE;
	TCHAR CmdLine[1024];

	DBG_MSG("%s  before CreateProcess", __FUNCTION__);

	if (ProcessName == _T("NY_DetectService.exe")) _sntprintf_s(CmdLine, 1024, _T("\"%s\" \"%s\""), TipExePath, RecPicPath);
	else _sntprintf_s(CmdLine, 1024, _T("\"%s\""), TipExePath);
	//CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	CProcessManager::CreateProcess(TipExePath, RecPicPath, NULL, false);

	DBG_MSG("%s\n", WstringToString(CmdLine).c_str());

	if (m_bIsDoubleRec)
	{
		if (ProcessName == _T("NY_DetectService.exe")) _sntprintf_s(CmdLine, 1024, _T("\"%s\" \"%s\""), TipExePath, RecPicPath2);
		else _sntprintf_s(CmdLine, 1024, _T("\"%s\""), TipExePath);
		//CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		CProcessManager::CreateProcess(TipExePath, RecPicPath2, NULL, false);
	}
	DBG_MSG("%s  end", __FUNCTION__);
	return TRUE;
	//return CProcessManager::CreateProcess(TipExePath, RecPicPath, BasePath);
}

void CUWApp::CleanTestImages()
{
	TCHAR path[MAX_PATH], RecPicPath[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);

	_tcscpy_s(RecPicPath, path);
	PathAppend(path, _T("config\\debug.ini"));
	PathAppend(RecPicPath, _T("RecGrapReslut"));

	UINT num = GetPrivateProfileInt(_T("save"), _T("num"), 7, path);
	if (num == 0)
	{
		// 没有限制保存数量，不需要清理图片
		return;
	}

	std::map<int, CString> mapDirs;
	CString strPath = RecPicPath;
	CFileFind filefind;
	if (strPath.Right(1) != "\\")//目录的最右边需要“\”字符
		strPath += "\\";
	strPath += "*.*";
	BOOL res = filefind.FindFile(strPath);
	int count = 0;
	while (res)
	{
		res = filefind.FindNextFile();
		if (filefind.IsDirectory() )//为目录
		{
			strPath = filefind.GetFilePath();
			CString fn = filefind.GetFileName();
			if (fn.SpanIncluding(_T("0123456789")) == fn)
			{
				mapDirs[_tcstol(fn,NULL, 10)] = strPath;
				++count;
			}
		}
	}
	if (count > num)
	{
		std::map<int, CString>::iterator itr = mapDirs.begin();
		std::map<int, CString>::iterator itrNext = itr;
		for (int i = 0; i < count - num; ++i)
		{
			itrNext++;
			RecursiveDeleteDir(itr->second);//是目录时继续递归，删除该目录下的文件
			mapDirs.erase(itr);
			itr = itrNext;
		}
	}
}

void CUWApp::GetTestTypeHoleInfo()
{
	// 读取配置文件
	char CfgFilePath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, CfgFilePath, MAX_PATH);
	PathRemoveFileSpecA(CfgFilePath);
	PathAppendA(CfgFilePath, "\\Config\\testmode.ini");

	theApp.g_nSettingType = GetPrivateProfileIntA("testmode", "mode", 1, CfgFilePath);
	//theApp.g_nSettingType1_type = GetPrivateProfileIntA("testmode", "mode1_type", 3, CfgFilePath);

	g_isForceUseSoftwareSettingTesttype = (BOOL)GetPrivateProfileIntA("testmode", "liushuixianForceUseSoftwareSettingTesttype", 0, CfgFilePath);

	char key[32] = { 0 };
	for (int i = 0; i < theApp.g_vecHoleForSet.size(); i++)
	{
		sprintf_s(key, "hole_%d", (i + 1));
		theApp.g_vecHoleForSet[i] = GetPrivateProfileIntA("testmode", key, 3, CfgFilePath);
		theApp.g_mapHoleSet[i] = std::map<int, int>();
	}
	theApp.g_mapHoleSet[0].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H1_1));
	theApp.g_mapHoleSet[0].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H1_2));
	theApp.g_mapHoleSet[0].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H1_3));
	theApp.g_mapHoleSet[1].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H2_1));
	theApp.g_mapHoleSet[1].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H2_2));
	theApp.g_mapHoleSet[1].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H2_3));
	theApp.g_mapHoleSet[2].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H3_1));
	theApp.g_mapHoleSet[2].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H3_2));
	theApp.g_mapHoleSet[2].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H3_3));
	theApp.g_mapHoleSet[3].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H4_1));
	theApp.g_mapHoleSet[3].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H4_2));
	theApp.g_mapHoleSet[3].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H4_3));
	theApp.g_mapHoleSet[4].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H5_1));
	theApp.g_mapHoleSet[4].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H5_2));
	theApp.g_mapHoleSet[4].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H5_3));
	theApp.g_mapHoleSet[5].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H6_1));
	theApp.g_mapHoleSet[5].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H6_2));
	theApp.g_mapHoleSet[5].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H6_3));
	theApp.g_mapHoleSet[6].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H7_1));
	theApp.g_mapHoleSet[6].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H7_2));
	theApp.g_mapHoleSet[6].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H7_3));
	theApp.g_mapHoleSet[7].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H8_1));
	theApp.g_mapHoleSet[7].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H8_2));
	theApp.g_mapHoleSet[7].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H8_3));
	theApp.g_mapHoleSet[8].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H9_1));
	theApp.g_mapHoleSet[8].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H9_2));
	theApp.g_mapHoleSet[8].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H9_3));
	theApp.g_mapHoleSet[9].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H10_1));
	theApp.g_mapHoleSet[9].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H10_2));
	theApp.g_mapHoleSet[9].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H10_3));
	theApp.g_mapHoleSet[10].insert(std::pair<int, int>(1, IDC_RADIO_TYPE2_H11_1));
	theApp.g_mapHoleSet[10].insert(std::pair<int, int>(2, IDC_RADIO_TYPE2_H11_2));
	theApp.g_mapHoleSet[10].insert(std::pair<int, int>(3, IDC_RADIO_TYPE2_H11_3));
}

void CUWApp::Poweroff(int sec)
{
	char cmd[32];
	sprintf_s(cmd, "-s -f -t %d", sec);
	ShellExecuteA(NULL, NULL, "shutdown", cmd, NULL, SW_HIDE);
	//system(cmd);
}

void CUWApp::EnterDebugMode(bool on)
{
    m_bInDebugMode = on;
    if (m_bIsLiushuixain) OnWsTestStateRequest();
}

bool CUWApp::IsInDebugMode() const
{
    return m_bInDebugMode;
}

void CUWApp::SetQcType(BOOL arr[6])
{
    if (m_qcTypeList.headerNode)
    {
        QC_TYPE_NODE *h = m_qcTypeList.headerNode;
        std::list<QC_TYPE_NODE*> lst;
        QC_TYPE_NODE *node = h;
        while (node)
        {
            lst.push_back(node);
            if (h == node->nextTypeNode)
            {
                break;
            }
            node = node->nextTypeNode;
        }
        for (auto itr = lst.cbegin(); itr != lst.cend(); itr++)
        {
            delete *itr;
        }
        lst.clear();
        memset(&m_qcTypeList, 0, sizeof(m_qcTypeList));
    }
    
    for (int i = UsQCBy1; i <= UdcQCByN; i++)
    {
        if (arr[i])
        {
            QC_TYPE_NODE *node = new QC_TYPE_NODE;
            node->td = (TestDes)i;
            node->nextTypeNode = NULL;
            if (m_qcTypeList.headerNode == NULL)
            {
                m_qcTypeList.headerNode = node;
                node->nextTypeNode = node;
            }
            else
            {
                QC_TYPE_NODE *h = m_qcTypeList.headerNode;
                QC_TYPE_NODE *n = h;
                while (n)
                {
                    if (h == n->nextTypeNode)
                    {
                        n->nextTypeNode = node;
                        node->nextTypeNode = h;
                        break;
                    }
                    else
                    {
                        n = n->nextTypeNode;
                    }
                }
            }
        }
    }
    m_qcTypeList.curNode = theApp.m_qcTypeList.headerNode;
}

TestDes CUWApp::GetNextQcType()
{
    TestDes td = UsQCBy1;

    if (m_qcTypeList.headerNode && m_qcTypeList.curNode)
    {
        td = m_qcTypeList.curNode->td;
        m_qcTypeList.curNode = m_qcTypeList.curNode->nextTypeNode;
    }

    return td;
}

void CUWApp::OnWsConnectEstablished(CWebSocket *session)
{
	FlowlineMessge fmsg;
	RegisterRequest regReq;
	regReq.set_name("Test Device");
	regReq.set_devtype(DeviceType::DEV_NY);
	regReq.set_devno(theApp.m_nDeviceNo);
	fmsg.set_name(regReq.GetTypeName());
	fmsg.set_message(regReq.SerializeAsString());
	std::string str;
	fmsg.SerializeToString(&str);
	if (session)
	{
		session->sendTextMessage((char*)str.c_str(), str.length());
	}
}

void CUWApp::OnWsPoweroffRequest()
{
	if (IDOK != MessageBoxTimeoutA(*AfxGetApp()->GetMainWnd(), "关闭计算机?", "关机提醒", MB_OKCANCEL | MB_ICONQUESTION, 0, 5000))
	{
        return;
	}
    else
    {
        Poweroff(3);
        AfxGetApp()->PostThreadMessageW(WM_QUIT, 0, 0);
    }
}

void CUWApp::OnWsConsumablesRequest()
{
    if (g_WSClient)
    {
        int count = 0;
        bool isExpired = false;
        theApp.QueryConsumables(0, count, isExpired, AfxGetApp()->GetMainWnd() ? AfxGetApp()->GetMainWnd()->GetSafeHwnd() : 0);

        /*FlowlineMessge fmsg;
        ConsumablesNotify msg;
        msg.set_devtype(DEV_NY);
        msg.set_devno(m_nDeviceNo);
        msg.set_number(count);
        fmsg.set_name(msg.GetTypeName());
        fmsg.set_message(msg.SerializeAsString());
        std::string message = fmsg.SerializeAsString();
        g_WSClient->sendTextMessage((char*)message.c_str(), message.size());*/
    }
}

void CUWApp::OnWsTestStateRequest()
{
    if (g_WSClient)
    {
        string txt = "空闲";
        if (m_bInDebugMode)
        {
            txt = "维护";
        }
        else if (!theApp.IsIdleStatus())
        {
            txt = "测试-标本";
            if (nAttribute > TestNormal && nAttribute < TestEP)
            {
                txt = "测试-质控";
            }
        }
        else if (theApp.IsIdleStatus())
        {
            txt = "空闲-标本";
            if (nAttribute > TestNormal && nAttribute < TestEP)
            {
                txt = "空闲-质控";
            }
        }
        
        FlowlineMessge fmsg;
        TestStateNotify msg;
        msg.set_devtype(DEV_NY);
        msg.set_devno(m_nDeviceNo);
        msg.set_teststate(txt);
        fmsg.set_name(msg.GetTypeName());
        fmsg.set_message(msg.SerializeAsString());
        std::string message = fmsg.SerializeAsString();
        g_WSClient->sendTextMessage((char*)message.c_str(), message.size());

    }
}

void CUWApp::OnWsChangeQcModelRequest(bool qc, int model, int item)
{
    DBG_MSG("---%s--(%s, %d, %d)-", __FUNCTION__, qc?"true":"false", model, item); 
    LRESULT r = m_pMainWnd->SendMessage(WM_WS_QC_CTRL, qc, (model<<16) | item);
    char buf[256] = { 0 };
    sprintf_s(buf, "%s%s", qc ? "开启" : "关闭", r == 0 ? "成功" : "失败");

    FlowlineMessge fmsg;
    ChangeQcModelRespond msg;
    msg.set_devtype(DEV_NY);
    msg.set_devno(m_nDeviceNo);
    msg.set_qc(qc);
    msg.set_mode(model);
    msg.set_item(item);
    msg.set_resultmsg(buf);
    fmsg.set_name(msg.GetTypeName());
    fmsg.set_message(msg.SerializeAsString());
    std::string message = fmsg.SerializeAsString();
    g_WSClient->sendTextMessage((char*)message.c_str(), message.size());

    //OnWsTestStateRequest();
}

void CUWApp::CheckDbbackupAndcleanup()
{
    ULONG nID = 0;
    KSTATUS status = Access_GetLastTaskId(&nID);
    DBG_MSG("Last task id is %u\n", nID);
    if (status != STATUS_SUCCESS)
    {
        DBG_MSG("Access_GetLastTaskId failed");
        return;
    }
    
    const int tipDbTaskId = 60000;
    const int maxDbTaskId = 65000;
    char ini[256] = { 0 };
    sprintf(ini, theApp.m_szExeFolderA);
    PathAppendA(ini, "Conf.ini");

    if (nID > maxDbTaskId)
    {
        WritePrivateProfileStringA("db_opt", "backup_and_cleanup", "1", ini);
    }
    else if (nID > tipDbTaskId)
    {
        if (IDOK == MessageBox(NULL, _T("数据库记录过大，是否备份和清空数据库?\n注:备份、清空数据库操作将在下次启动软件时完成"), _T("提示"), MB_ICONQUESTION | MB_OKCANCEL))
        {
            WritePrivateProfileStringA("db_opt", "backup_and_cleanup", "1", ini);
        }
    }
}

bool CUWApp::DoDbbackupAndcleanup()
{
    bool ret = false;
    char ini[256] = { 0 };
    sprintf(ini, theApp.m_szExeFolderA);
    PathAppendA(ini, "Conf.ini");

    BOOL IsOpt = (BOOL)GetPrivateProfileIntA("db_opt", "backup_and_cleanup", 0, ini);
    if (IsOpt)
    {
        SYSTEMTIME st = {};
        GetLocalTime(&st);
        char db[256] = { 0 }, dbBackupTarget[256] = { 0 }, dbBackup[256] = { 0 }, tmp[256] = { 0 };
        sprintf(db, theApp.m_szExeFolderA);
        sprintf(dbBackupTarget, theApp.m_szExeFolderA);
        sprintf(dbBackup, theApp.m_szExeFolderA);
        PathAppendA(db, "db\\uw.mdb");
        sprintf_s(tmp, "db\\uw_backup_%04d%02d%02d.mdb", st.wYear, st.wMonth, st.wDay);
        PathAppendA(dbBackupTarget, tmp);
        PathAppendA(dbBackup, "db\\uw_backup.mdb");
        if (PathFileExistsA(db) && PathFileExistsA(dbBackup))
        {
            if (CopyFileA(db, dbBackupTarget, FALSE))
            {
                if (CopyFileA(dbBackup, db, FALSE))
                {
                    if (WritePrivateProfileStringA("db_opt", "backup_and_cleanup", "0", ini))
                    {
                        ret = true;
                    }
                    else
                    {
                        CopyFileA(dbBackupTarget, db, FALSE);
                    }
                }
            }
        }
    }

    return ret;
}

void CUWApp::SubCleanCount( USHORT nCleanCount,int &RemainTimes,BOOL bShowTip )
{
	//TCHAR   buf[200];

	if (m_bIsControlConsumable)
	{

		BOOL  bRet = FALSE;

		//*pRemainCount = 0;


		StructClearTimer clearinfo;
		while(Interface_ReadCardTimes(&clearinfo)!=0)
		{
			//UnAllowTest();
		}

		//CMMgr_QueryCleanCount(CLEAN_DAILY_TYPE,pRemainCount);
		m_LastClearTimes = clearinfo.Timer;
		while( clearinfo.Timer <=0)
		{
			//UnAllowTest();
			//MessageBox(NULL,_T("耗材用完，请更换新的耗材"),_T("提示"),MB_OK);
			//CTipDlg dlg;
			//if (dlg.DoModal() == IDOK)
			{
				//Interface_ReadCardTimes(&clearinfo);
				COleDateTime datetimeNow,readtime;
				datetimeNow.GetCurrentTime();
				CString str = (CString)clearinfo.strDate;
				CString year,month,day,strdates;
				year = str.Mid(0,4);
				month = str.Mid(4,2);
				day = str.Mid(6,2);
				readtime.SetDate(_ttoi(year),_ttoi(month),_ttoi(day));

				CTime tmS(readtime);
				CTime tmE(datetimeNow);

				CTimeSpan span = tmE-tmS;

				if (span.GetDays()>=2*356)
				{
					clearinfo.Timer = 0;
					MessageBox(NULL,_T("耗材有效期已过，请更换新的耗材"),_T("提示"),MB_OK);
				}
			}
		}

		DBG_MSG("SubCleanCount %d",clearinfo.Timer);

		RemainTimes = clearinfo.Timer-nCleanCount;
		while((Interface_WriteCardTimes((nCleanCount)))!= true)
		{
			//UnAllowTest();
		}
		DBG_MSG("SubCleanCount %d",clearinfo.Timer-nCleanCount);
		//AllowTest();
	}
}

bool CUWApp::SubCleanCount(USHORT nCleanCount)
{
	if (m_bIsControlConsumable)
	{
		if (Interface_WriteCardTimes((nCleanCount)) != true)
		{
			DBG_MSG("写耗材卡失败");
			return false;
		}
		return true;
	}
	return true;
}

static void WarnningMessageBox(CString Warning)
{
    static std::atomic<bool> exitSpeak = false;
    exitSpeak = false;

    if (!Warning.IsEmpty() && theApp.m_bIsLiushuixain)
    {
        auto f = std::async(std::launch::async, [Warning]{
            if (FAILED(::CoInitializeEx(NULL, COINIT_MULTITHREADED)))
            {
                DBG_MSG("CoInitializeEx error");
                return;
            }
            CComPtr<ISpVoice>              pVoice;
            HRESULT hr = pVoice.CoCreateInstance(CLSID_SpVoice);
            if (FAILED(hr))
            {
                DBG_MSG("CoCreateInstance failed");
                return;
            }
            pVoice->SetVolume(100);//设置音量，范围是 0 -100
            pVoice->SetRate(-1);//设置速度，范围是 -10 - 10
            pVoice->SetSyncSpeakTimeout(300);
            while (1)
            {
                if (exitSpeak)
                {
                    break;
                }
                hr = pVoice->Speak(Warning, 0, NULL);
                if (FAILED(hr))
                {
                    DBG_MSG("Speak failed");
                }
            }
            ::CoUninitialize();
        });
    }

    HWND hWNd = AfxGetApp()->GetMainWnd() ? AfxGetApp()->GetMainWnd()->GetSafeHwnd() : NULL;
    ::MessageBox(hWNd, Warning, L"UW2000提示", MB_OK | MB_ICONWARNING);
    exitSpeak = true;
}

bool CUWApp::QueryConsumables(USHORT nCleanCount, int &count, bool &isExpired, HWND hWnd)
{
	LARGE_INTEGER nFreq;
	LARGE_INTEGER t1;
	LARGE_INTEGER t2;
	LARGE_INTEGER t3;
	LARGE_INTEGER t4;
	LARGE_INTEGER t5;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);
	if (m_bIsControlConsumable)
	{
		StructClearTimer clearinfo;
		while (Interface_ReadCardTimes(&clearinfo) != 0)
		{
			//MessageBox(hWnd, _T("读耗材卡失败"), _T("提示"), MB_OK);
            CString str = GetIdsString(_T("haocai"), _T("0")).c_str();
            if (str.IsEmpty() || str == _T("0")) str = _T("读耗材卡失败");
            WarnningMessageBox(str);
		}
		m_LastClearTimes = clearinfo.Timer;
		QueryPerformanceCounter(&t2);
		while (clearinfo.Timer <= 0)
		{
			::PostMessage(hWnd, WM_HAOCAI, 0, 0);
			//int ret = MessageBox(hWnd, _T("耗材用完，请更换新的耗材"), _T("提示"), MB_OK);
            CString str = GetIdsString(_T("haocai"), _T("1")).c_str();
            if (str.IsEmpty() || str == _T("0")) str = _T("耗材用完，请更换新的耗材");
            WarnningMessageBox(str);
			//CTipDlg dlg;
			//if (ret == IDOK)
			{
				Interface_ReadCardTimes(&clearinfo);
				COleDateTime datetimeNow, readtime;
				datetimeNow.GetCurrentTime();
				CString str = (CString)clearinfo.strDate;
				CString year, month, day, strdates;
				year = str.Mid(0, 4);
				month = str.Mid(4, 2);
				day = str.Mid(6, 2);
				readtime.SetDate(_ttoi(year), _ttoi(month), _ttoi(day));

				CTime tmS(readtime);
				CTime tmE(datetimeNow);

				CTimeSpan span = tmE - tmS;

				if (span.GetDays() >= 356)
				{
					clearinfo.Timer = 0;
                    str = GetIdsString(_T("haocai"), _T("2")).c_str();
                    if (str.IsEmpty() || str == _T("0")) str = _T("耗材有效期已过，请更换新的耗材");
                    WarnningMessageBox(str);
					//MessageBox(hWnd, _T("耗材有效期已过，请更换新的耗材"), _T("提示"), MB_OK);
				}
			}
		}
		QueryPerformanceCounter(&t3);
		DBG_MSG("Before SubCleanCount %d", clearinfo.Timer);

		count = clearinfo.Timer - nCleanCount;
		if (nCleanCount > 0)
		{
			while ((Interface_WriteCardTimes((nCleanCount))) != true)
			{
				//MessageBox(hWnd, _T("写耗材卡失败"), _T("提示"), MB_OK);
                CString str = GetIdsString(_T("haocai"), _T("3")).c_str();
                if (str.IsEmpty() || str == _T("0")) str = _T("写耗材卡失败");
                WarnningMessageBox(str);
			}
		}
		DBG_MSG("After SubCleanCount %d", count);
		QueryPerformanceCounter(&t4);
		while (Interface_ReadCardTimes(&clearinfo) != 0)
		{
			//MessageBox(hWnd, _T("读耗材卡失败"), _T("提示"), MB_OK);
            CString str = GetIdsString(_T("haocai"), _T("0")).c_str();
            if (str.IsEmpty() || str == _T("0")) str = _T("读耗材卡失败");
            WarnningMessageBox(str);
		}
		count = clearinfo.Timer;
		::PostMessage(hWnd, WM_HAOCAI, (WPARAM)count, 0);
		QueryPerformanceCounter(&t5);

		double d1 = (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000;
		double d2 = (t3.QuadPart - t2.QuadPart) / (double)nFreq.QuadPart * 1000;
		double d3 = (t4.QuadPart - t3.QuadPart) / (double)nFreq.QuadPart * 1000;
		double d4 = (t5.QuadPart - t4.QuadPart) / (double)nFreq.QuadPart * 1000;
		DBG_MSG("%.2f, %.2f, %.2f, %.2f\n", d1,d2,d3,d4);

        if (m_bIsLiushuixain && g_WSClient)
        {
            FlowlineMessge fmsg;
            ConsumablesNotify msg;
            msg.set_devtype(DEV_NY);
            msg.set_devno(m_nDeviceNo);
            msg.set_number(count);
            fmsg.set_name(msg.GetTypeName());
            fmsg.set_message(msg.SerializeAsString());
            std::string message = fmsg.SerializeAsString();
            g_WSClient->sendTextMessage((char*)message.c_str(), message.size());
        }

		return true;
	}
	return true;
}

bool CUWApp::CopyDBToBackup(bool isError)
{
	char target[MAX_PATH],source[MAX_PATH];
	GetModuleFileNameA(NULL, target,MAX_PATH);
	PathRemoveFileSpecA(target);
	memcpy(source, target, sizeof(target));

	if (isError)
		PathAppendA(target, "\\db\\uw_error.mdb");
	else
		PathAppendA(target, "\\db\\uw_backup.mdb");

	CFileFind finder;
	PathAppendA(source, "\\db\\uw.mdb");										
	bool bWorking = finder.FindFile(Common::CharToWChar(source).c_str());  //是否找到了需要拷贝的文件的路径
	if(bWorking) 
			return CopyFileA(source, target, FALSE);
}

bool CUWApp::CopyBackupToDB(bool isReset)
{
	char target[MAX_PATH], source[MAX_PATH];
	GetModuleFileNameA(NULL, target, MAX_PATH);
	PathRemoveFileSpecA(target);
	memcpy(source, target, sizeof(target));

	if (isReset)
		PathAppendA(source, "\\Backup\\uw.mdb");
	else
		PathAppendA(source, "\\db\\uw_backup.mdb");

	CFileFind finder;
	PathAppendA(target, "\\db\\uw.mdb");
	bool bWorking = finder.FindFile(Common::CharToWChar(source).c_str());  //是否找到了需要拷贝的文件的路径
	if (bWorking)
		return CopyFileA(source, target, FALSE);	
}


BOOL ClearPath(TCHAR *dir)
{
	BOOL bRet = TRUE;
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR path[MAX_PATH] = { 0 }, file[MAX_PATH] = { 0 };

	_sntprintf(path, MAX_PATH, _T("%s"), dir);
	PathAppend(path, _T("\\*"));
	hFind = FindFirstFile(path, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return FALSE;
	}
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		_sntprintf(file, MAX_PATH, _T("%s\\%s"), dir, ffd.cFileName);
		bRet = DeleteFile(file);
		if (!bRet)
		{
			break;
		}
	} while (FindNextFile(hFind, &ffd));


	return bRet;
}



void CAboutDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialog::OnOK();
}

HRESULT CompactDatabase(const char * dbPath)
{
	HRESULT hr = E_FAIL;
	if (dbPath == NULL)
	{
		return hr;
	}

	char path[MAX_PATH] = { 0 };
	char fileSrc[MAX_PATH] = { 0 };
	char fileDst[MAX_PATH] = { 0 };
	char fileBack[MAX_PATH] = { 0 };
	char temp[MAX_PATH] = { 0 };

	GetModuleFileNameA(NULL, fileSrc, MAX_PATH);
	PathRemoveFileSpecA(fileSrc);
	strcpy(fileDst, fileSrc);
	time_t now = time(NULL);
	sprintf_s(temp, "%llu", now);
	PathAppendA(fileDst, temp);

	PathAppendA(fileSrc, dbPath);
	sprintf_s(temp, "%s.bak", fileSrc);

	if (!PathFileExistsA(dbPath))
	{
		return hr;
	}

	if (!CopyFileA(fileSrc, temp, FALSE))
	{
		::MessageBoxA(NULL, "CompactDatabase 复制文件失败!", "", MB_OK);
		return hr;
	}
	if (!MoveFileA(fileSrc, fileDst))
	{
		::MessageBoxA(NULL, "CompactDatabase 移动文件失败 1!", "", MB_OK);
		DeleteFileA(temp);
		return hr;
	}

	char strSrc[MAX_PATH];
	char strDst[MAX_PATH];
	sprintf_s(strSrc, "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s", fileDst);
	sprintf_s(strDst, "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Engine Type=5", fileSrc);
	try
	{
		IJetEnginePtr jet(__uuidof(JetEngine));
		hr = jet->CompactDatabase(strSrc, strDst);
	}
	catch (_com_error &e)
	{
		::MessageBoxA(NULL, (LPCSTR)e.Description(), "", MB_OK);
		DeleteFileA(fileDst);
		DeleteFileA(fileSrc);
		if (!MoveFileA(temp, fileSrc))
		{
			::MessageBoxA(NULL, "CompactDatabase 移动文件失败 2!", "", MB_OK);
			DeleteFileA(temp);
			return hr;
		}
	}
	if (SUCCEEDED(hr))
	{
		if (!DeleteFileA(fileDst))
		{
			::MessageBoxA(NULL, "CompactDatabase 删除文件失败 1!", "", MB_OK);
			return E_FAIL;
		}
		if (!DeleteFileA(temp))
		{
			::MessageBoxA(NULL, "CompactDatabase 删除文件失败 2!", "", MB_OK);
			return E_FAIL;
		}
	}

	return hr;
}
