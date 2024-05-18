#include "stdafx.h"
#include "OutputWnd.h"
#include "UW.h"
#include "MainFrm.h"
#include "UWView.h"

extern
std::wstring GetIdsString(const std::wstring &sMain, const std::wstring &sId);

#define HOFFSET 13
COutputWnd::COutputWnd()
: m_tmNow(COleDateTime::GetCurrentTime())
, m_nNum(0)
{
	memset(m_chIniPath, 0, sizeof(m_chIniPath));
	GetModuleFileName(NULL, m_chIniPath, MAX_PATH);
	TCHAR *pChar = _tcsrchr(m_chIniPath, '\\');
	pChar++;
	*pChar = 0;
}

int hOffset = 0;

COutputWnd::~COutputWnd()
{
	
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_BTN_CLEARINFO,&COutputWnd::OnMyBtnKnow)
	ON_UPDATE_COMMAND_UI(IDC_BTN_CLEARINFO,OnUpdateCommandUI)
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	// 创建窗口:
// 	if (!m_wndEdit.Create(WS_VSCROLL | WS_CHILD | WS_DISABLED | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_READONLY, CRect(0, 0, 0, 0), this, 101))
// 	{
// 		TRACE0("未能创建窗口\n");
// 		return -1;      // 未能创建
// 	}
	if (!m_clearInfo.Create(L"我知道了", WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0, 0, 0, 0), this, IDC_BTN_CLEARINFO))
	{
		TRACE0("未能创建按钮\n");
		return -1;      // 未能创建
	}
		if (!m_wndEditNoDel.Create(WS_VSCROLL | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_READONLY | WS_TABSTOP, CRect(0, 0, 0, 0), this, 102))
	{
		TRACE0("未能创建窗口\n");
		return -1;      // 未能创建
	}

	// 设置字体
/*	m_wndEdit.SetFont(&afxGlobalData.fontRegular);*/
	m_wndEditNoDel.SetFont(&afxGlobalData.fontRegular);
	m_clearInfo.SetFont(&afxGlobalData.fontRegular);
	m_clearInfo.SetImagePng(IDB_PNG_KNOW);
	m_clearInfo.SetTransparentColor(RGB(0, 0,0));
	m_clearInfo.ShowWindow(SW_HIDE);
	Clear();
	AdjustLayout();
	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();

/*	::PostMessage(this->GetSafeHwnd(),WM_PAINT, 0, 0);*/
}

void COutputWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndEditNoDel.SetFocus();
}

void COutputWnd::OnMyBtnKnow()
{
	Clear();
}

void COutputWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);
	m_wndEditNoDel.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	m_clearInfo.MoveWindow(rectClient.Width() - 80,0, 60, 20);
	m_wndEditNoDel.SetMargins(0, rectClient.Width() - 10);
	m_wndEditNoDel.ModifyStyle(0, WS_CLIPSIBLINGS);

 	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
// 
// 
	HWND hWndMyProjectView = pMainFrame->GetSafeHwnd();
	::PostMessage(hWndMyProjectView, WM_SIZE, 0, 0);//原来要给框架发送WM_SIZE而不是WM_PAINT
/* 	::PostMessage(theApp.GetMainWnd()->GetSafeHwnd(), WM_PAINT, 0, 0);*/
	/*m_wndEdit.SetWindowPos(NULL, rectClient.left, rectClient.top + rectClient.Height() / 2, rectClient.Width(), rectClient.Height() / 2, SWP_NOACTIVATE | SWP_NOZORDER);*/

}

// 清除错误信息
void COutputWnd::Clear()
{
	m_wndEditNoDel.SetWindowText(_T(""));
	hOffset = 0;
	CRect rectClientInfo, rectClientPane;
	m_clearInfo.ShowWindow(SW_HIDE);
	GetClientRect(rectClientInfo);
	m_wndEditNoDel.GetWindowRect(rectClientPane);
	SetWindowPos(NULL, rectClientPane.left, rectClientInfo.top, rectClientInfo.Width(), 0, SWP_NOACTIVATE | SWP_NOZORDER);
}

// 显示错误,错误修正时清除
void COutputWnd::ShowInfo(const TCHAR* pContent)
{	
// 	 m_tmNow = COleDateTime::GetCurrentTime();
// 	CString timeNow = m_tmNow.Format(_T("%Y-%m-%d %H:%M:%S :"));
// 	CString strInfo;
// 	m_wndEdit.GetWindowText(strInfo);
// 	m_wndEdit.Clear();
// 	CString strContent = pContent;
// 	strContent.TrimLeft();
// 	strContent.TrimRight();
// 	if (!strContent.IsEmpty())
// 	{
// 		log(timeNow + pContent);
// 		strInfo += _T("\r\n");
// 		strInfo += timeNow;
// 		strInfo += pContent;
// 		m_wndEdit.SetWindowText(strInfo);
// 
// 	}
}


// 显示错误
void COutputWnd::ShowInfoNoDel(const TCHAR* pContent)
{	
	m_wndEditNoDel.SetFocus();
	CRect rectClientInfo, rectCombo, rectClientPane;
	int rectHeight = 0;

	m_tmNow = COleDateTime::GetCurrentTime();
	CString timeNow = m_tmNow.Format(_T("%Y-%m-%d %H:%M:%S :"));

	CString strInfo;
	m_wndEditNoDel.GetWindowText(strInfo);
	
	CString strContent = pContent;
	strContent.TrimLeft();
	strContent.TrimRight();
	if (!strContent.IsEmpty())
	{
		log(timeNow + pContent);
		if (!strInfo.IsEmpty())
		{
			strInfo += _T("\r\n");
		}
		strInfo += timeNow;
		strInfo += pContent;
		m_wndEditNoDel.SetWindowText(strInfo);

		hOffset += HOFFSET;
		CString row = GetIdsString(_T("outputinfo"), _T("row")).c_str();
		if (hOffset >= atoi((const char*)row.GetBuffer()) *HOFFSET)
		{
			/*m_clearInfo.ShowWindow(SW_SHOW);*/
			m_wndEditNoDel.ModifyStyle(0, ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOVSCROLL);
			m_wndEditNoDel.LineScroll(m_wndEditNoDel.GetLineCount());
		}
		else  if (hOffset > HOFFSET)
		{
			GetClientRect(rectClientInfo);
			m_wndEditNoDel.GetWindowRect(rectClientPane);
			SetWindowPos(NULL, rectClientPane.left, rectClientInfo.top, rectClientInfo.Width(), rectClientInfo.Height() + HOFFSET, SWP_NOACTIVATE | SWP_NOZORDER);
		}
		else
		{
//为啥不显示第一次错误信息
// 			m_wndEditNoDel.SetWindowText(_T(""));
// 			CRect rectClientInfo, rectClientPane;
// 			m_clearInfo.ShowWindow(SW_HIDE);
// 			GetClientRect(rectClientInfo);
// 			m_wndEditNoDel.GetWindowRect(rectClientPane);
// 			SetWindowPos(NULL, rectClientPane.left, rectClientInfo.top, rectClientInfo.Width(), 15, SWP_NOACTIVATE | SWP_NOZORDER);
// 			
// 			::PostMessage(GetSafeHwnd(), WM_SIZE, 0, 0);
// 			m_wndEditNoDel.SetWindowText(strInfo);
			m_clearInfo.ShowWindow(SW_SHOW);
			GetClientRect(rectClientInfo);
			m_wndEditNoDel.GetWindowRect(rectClientPane);
			SetWindowPos(NULL, rectClientPane.left, rectClientInfo.top, rectClientInfo.Width(), 16, SWP_NOACTIVATE | SWP_NOZORDER);//必须通知框架大小改变了才能显示，并且大小一定要有一点改变
		}
	}
	UpdateWindow();
}

// 写日志
void COutputWnd::log(CString logStr)
{
	CFileFind findFile;
	CString filePath;
	filePath = m_chIniPath ;
	filePath +=_T("Log\\");
	if (!findFile.FindFile(filePath))
	{
		::CreateDirectory(filePath,NULL);
	}
	CString fileName = filePath + m_tmNow.Format(_T("%Y-%m-%d"))+_T(".ini");
	if (m_nNum == 0)
	{
		m_nNum = GetPrivateProfileInt(_T("Log"),_T("Count"),0,fileName);
	}
	m_nNum++;
	CString numStr ;
	numStr.Format(_T("%d"),m_nNum);
	WritePrivateProfileString(_T("Log"),_T("Count"),numStr,fileName);
	WritePrivateProfileString(_T("Log"),numStr,logStr,fileName);
}

HBRUSH COutputWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	pDC->SetTextColor(RGB(255, 0, 0));
	return hbr;
}



void COutputWnd::OnContextMenu( CWnd* /*pWnd*/, CPoint point )
{

}

void COutputWnd::OnPressCloseButton()
{

}

void COutputWnd::OnUpdateCommandUI(CCmdUI *cmdUI)
{

}
