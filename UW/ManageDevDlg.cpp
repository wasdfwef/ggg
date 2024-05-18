
#include "stdafx.h"
#include "UW.h"
#include "ManageDevDlg.h"
#include "CmdHelper.h"
#include <algorithm>
#include "DbgMsg.h"


// CManageDevDlg 对话框

IMPLEMENT_DYNAMIC(CManageDevDlg, CDialog)
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

#pragma comment(lib, "version.lib")

CManageDevDlg::CManageDevDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManageDevDlg::IDD, pParent)
	,nCurPage(0)
	,m_Sheet(_T("SHEET"))
{
	m_IsResetLC = false;
	
}

CManageDevDlg::~CManageDevDlg()
{
	
}

void CManageDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManageDevDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CManageDevDlg 消息处理程序

void CManageDevDlg::InitChildWnd(CWnd* childWnd)
{
/*	std::wstring win = GetIdsString(L"win10", L"iswin10");*/
	if (childWnd == NULL)
		return;

	if (IsFileVersionWindows10OrGreater())
	{
		CRect rect;
		CRect childSaved;
		float x = 0.9;
		float y = 0.85;
		childWnd->GetWindowRect(rect);
		int wndWidth = GetSystemMetrics(SM_CXSCREEN);  //屏幕宽度    
		int wndHeight = GetSystemMetrics(SM_CYSCREEN);

		CWnd* pWnd = childWnd->GetWindow(GW_CHILD);//获取子窗体  

		while (pWnd)
		{

			pWnd->GetWindowRect(childSaved);
			// 			LPWSTR szClassName;
			// 			GetClassName(pWnd->GetSafeHwnd(), szClassName, MAX_PATH);
			// 			if (wcsicmp(szClassName, L"Button") != 0)
			// 			{
			// 
			// 				continue;
			// 			}
			childSaved.left -= 20;
			childSaved.right -= 20;
			childSaved.top -= 30;
			childSaved.bottom -= 30;

			childSaved.top *= y;
			childSaved.bottom *= y;
			childSaved.left *= x;//根据比例调整控件上下左右距离对话框的距离 
			childSaved.right *= x;
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_MOVEMIC)
			{
				childSaved.bottom += 55;
				childSaved.top -= 5;
			}

			ScreenToClient(childSaved);
			pWnd->MoveWindow(childSaved);
			pWnd = pWnd->GetNextWindow();//取下一个子窗体  

		}

		childWnd->GetWindowRect(rect);
		/*	ScreenToClient(rect);*/
		/*	childWnd->MoveWindow(rect.top, rect.left, rect.Width()*x, rect.Height()*y);*/
	}
	
}

BOOL CManageDevDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_mode == SOFTWARE)
	{
		SetWindowText(GetIdsString(_T("debug"), _T("title3")).c_str());
		m_Sheet.AddPage(&m_dbgPicImagePage);
		m_Sheet.AddPage(&m_dbgSetUIPage);

		m_Sheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
		m_Sheet.ModifyStyle(0, WS_TABSTOP | TCS_BOTTOM);
		m_Sheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);

		CRect rcSheet;
		GetClientRect(&rcSheet);
		m_Sheet.SetWindowPos(NULL, rcSheet.left, rcSheet.top, rcSheet.Width(), rcSheet.Height(),
			SWP_NOZORDER | SWP_NOACTIVATE);
		m_Sheet.SetActivePage(0);
		m_Sheet.SetActivePage(1);
		m_Sheet.SetActivePage(nCurPage);

		DBG_MSG("m_dbgPicImagePage: %#x\n", m_dbgPicImagePage.GetSafeHwnd());
		DBG_MSG("m_dbgSetUIPage: %#x\n", m_dbgSetUIPage.GetSafeHwnd());
		HWND hwnd = m_dbgPicImagePage.GetSafeHwnd();
		CWnd* m_wnd = FromHandle(hwnd);
		InitChildWnd(m_wnd);

		hwnd = m_dbgSetUIPage.GetSafeHwnd();
		m_wnd = FromHandle(hwnd);
		InitChildWnd(m_wnd);

		return TRUE;
	}
	SetWindowText(GetIdsString(_T("debug"),_T("title2")).c_str());

	// TODO:  在此添加额外的初始化
	//m_Sheet.AddPage(&m_page1);
	//m_Sheet.AddPage(&m_page2);
	

	m_Sheet.AddPage(&m_dbgActionPage);
	m_Sheet.AddPage(&m_dbgMotorPage);
	m_Sheet.AddPage(&m_dbgSensorPage);
	m_Sheet.AddPage(&m_dbgUdcPage);
	m_Sheet.AddPage(&m_dbgStdUdc);
	m_Sheet.AddPage(&m_dbgMVD);


	//m_Sheet.AddPage(&m_dbgMicroscopePage);
	//m_Sheet.AddPage(&m_dbgCharBarcodePage);
	m_Sheet.AddPage(&m_dbgHardwareParamPage);

#if (_LC_V >= 2200)
	m_Sheet.AddPage(&m_dbgMotorConfPage);
#endif
	m_Sheet.Create(this, WS_CHILD | WS_VISIBLE,0);
	m_Sheet.ModifyStyle(0, WS_TABSTOP | TCS_BOTTOM);
	m_Sheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	/*CTabCtrl* pTab = m_Sheet->GetTabControl();
	TCITEM tcitem;
	for( int i = 0; i < 2; i++)
	{
		pTab->SetItem(i, &tcitem);
	}*/

	/*int nExtraHeight = 0;
	CRect rectwindow;
	GetWindowRect(rectwindow);
	SetWindowPos(NULL, 0, 0, rectwindow.Width(), rectwindow.Height() , SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	CWnd *pPropSheet = (CWnd*)m_Sheet;
	CRect rectTab;
	pPropSheet->GetWindowRect(rectTab);
	pPropSheet->SetWindowPos(NULL, 0, 0, rectwindow.Width(), rectwindow.Height() , SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);*/

	CRect rcSheet;
	GetClientRect(&rcSheet);
	m_Sheet.SetWindowPos(NULL, rcSheet.left, rcSheet.top, rcSheet.Width() ,rcSheet.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);

	m_Sheet.SetActivePage(0);
	m_Sheet.SetActivePage(1);
	m_Sheet.SetActivePage(2);
	m_Sheet.SetActivePage(3);
	m_Sheet.SetActivePage(4);
	m_Sheet.SetActivePage(5); 
	m_Sheet.SetActivePage(6);
#if (_LC_V >= 2200)
	m_Sheet.SetActivePage(7);
#endif
	m_Sheet.SetActivePage(nCurPage);

	HWND hwnd = m_dbgActionPage.GetSafeHwnd();
	CWnd* m_wnd = FromHandle(hwnd);
	InitChildWnd(m_wnd);

	hwnd = m_dbgHardwareParamPage.GetSafeHwnd();
	m_wnd = FromHandle(hwnd);
	InitChildWnd(m_wnd);

	hwnd = m_dbgMotorPage.GetSafeHwnd();
	m_wnd = FromHandle(hwnd);
	InitChildWnd(m_wnd);
	
	hwnd = m_dbgSensorPage.GetSafeHwnd();
	m_wnd = FromHandle(hwnd);
	InitChildWnd(m_wnd);

	hwnd = m_dbgUdcPage.GetSafeHwnd();
	m_wnd = FromHandle(hwnd);
	InitChildWnd(m_wnd);

	
	hwnd = m_dbgStdUdc.GetSafeHwnd();
	m_wnd = FromHandle(hwnd);
	InitChildWnd(m_wnd);

	
	hwnd = m_dbgMVD.GetSafeHwnd();
	m_wnd = FromHandle(hwnd);
	InitChildWnd(m_wnd);

#if (_LC_V >= 2200)
	hwnd = m_dbgMotorConfPage.GetSafeHwnd();
	m_wnd = FromHandle(hwnd);
	InitChildWnd(m_wnd);
#endif

// 	m_Sheet.PostMessage(TCN_SELCHANGE, 0, 0);
	CMyDelegate1<CManageDevDlg*, void (CManageDevDlg::*)(void*, void*), void*> *e = new CMyDelegate1<CManageDevDlg*, void (CManageDevDlg::*)(void*, void*), void*>;
	e->RegisterEventHandler(this, &CManageDevDlg::OnRecvLCReseting, 0);
	theApp.m_clsMsgDipatcher.AddEventHandler(LcReStarting, e);

	m_mapEventDelegate[LcReStarting] = e;

	// 权限控制，普通用户不允许调试
	if (theApp.loginGroup != 111 && theApp.loginGroup != 4)
	{
		m_Sheet.RemovePage(&m_dbgActionPage);
		m_Sheet.RemovePage(&m_dbgMotorPage);
		m_Sheet.RemovePage(&m_dbgSensorPage);
		m_Sheet.RemovePage(&m_dbgUdcPage);
		m_Sheet.RemovePage(&m_dbgStdUdc);
		m_Sheet.RemovePage(&m_dbgMVD);
#if (_LC_V >= 2200)
		m_Sheet.RemovePage(&m_dbgMotorConfPage);
#endif
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CManageDevDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_mode == SOFTWARE)
	{
		CDialog::OnClose();
		return;
	}
	if ( IDYES == MessageBox( GetIdsString(_T("debug"), _T("lcreset")).c_str(),NULL, MB_YESNO ) )
	{
		m_IsResetLC = true;
		theApp.m_bAutoCheckFail = true;
		theApp.m_startDev = UCRestart();
		DBG_MSG("调试后下位机复位");
	}
	else
	{
		m_IsResetLC = false;
		theApp.UpdateCheckTypeEx();
	}

	CDialog::OnClose();
}

CRect CManageDevDlg::GetPageMaxSize(int, int)
{
	CRect maxSize;
	for (int i = 0; i < m_Sheet.GetPageCount();i++)
	{
		
	}
	return maxSize;
}

void CManageDevDlg::OnDestroy()
{
	CDialog::OnDestroy();

	std::for_each(m_mapEventDelegate.begin(), m_mapEventDelegate.end(), [](std::map<int, void*>::reference r){
		theApp.m_clsMsgDipatcher.RemoveEventHandler(r.first, (CMyDelegate*)r.second);
	});
}

void CManageDevDlg::SetDlgMode(int mode)
{
	this->m_mode = mode;
}

void CManageDevDlg::OnRecvLCReseting(void *arg, void *cbarg)
{
	EndDialog(0);
}


int CManageDevDlg::GetSystem()
{
	int a = 0, b = 0, i = 0, j = 0;
	_asm
	{
		pushad
			mov ebx, fs:[0x18]; get self pointer from TEB
			mov eax, fs:[0x30]; get pointer to PEB / database
			mov ebx, [eax + 0A8h]; get OSMinorVersion
			mov eax, [eax + 0A4h]; get OSMajorVersion
			mov j, ebx
			mov i, eax
			popad
	}

	if ((i == 5) && (j == 0))
	{
		AfxMessageBox(_T("系统版本为 Windows 2000"), MB_OK);
	}
	else if ((i == 5) && (j == 1))
	{
		AfxMessageBox(_T("系统版本为 Windows XP"), MB_OK);
	}
	else if ((i == 5) && (j == 2))

	{
		AfxMessageBox(_T("系统版本为 Windows 2003"), MB_OK);
	}
	else if ((i == 6) && (j == 0))
	{
		AfxMessageBox(_T("系统版本为 Windows Vista"), MB_OK);
	}
	else if ((i == 6) && (j == 1))
	{
		return 7;
	}
	else
	{
		return 0;
	}
}


BOOL CManageDevDlg::IsFileVersionWindows10OrGreater()
{
	DWORD dwVerInfoSize = GetFileVersionInfoSizeW(L"kernel32.dll", NULL);
	if (0 != dwVerInfoSize)
	{
		LPWSTR pszBuffer = (LPWSTR)_alloca(dwVerInfoSize*sizeof(WCHAR));
		if (TRUE == GetFileVersionInfoW(L"kernel32.dll", 0, dwVerInfoSize, pszBuffer))
		{
			VS_FIXEDFILEINFO* pVsInfo = NULL;
			UINT nFileInfoSize = sizeof(VS_FIXEDFILEINFO);
			if (TRUE == VerQueryValueW(pszBuffer, L"\\", (void**)&pVsInfo, &nFileInfoSize))
			{
				printf("%d.%d.%d.%d\n", HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS), HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
				DBG_MSG("当前操作系统为%d\n", HIWORD(pVsInfo->dwFileVersionMS));
				if (HIWORD(pVsInfo->dwFileVersionMS) >= 10)
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;

}