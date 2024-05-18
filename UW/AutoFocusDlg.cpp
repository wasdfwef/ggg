// AutoFocusDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "AutoFocusDlg.h"
#include "AutoFocusMgr.h"
#include "..\..\..\inc\AutoFocus.h"


// CAutoFocusDlg 对话框

IMPLEMENT_DYNAMIC(CAutoFocusDlg, CDialog)

CAutoFocusDlg::CAutoFocusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoFocusDlg::IDD, pParent)
{
	m_hThread = NULL;
}

CAutoFocusDlg::~CAutoFocusDlg()
{
}

void CAutoFocusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CAutoFocusDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, &CAutoFocusDlg::OnBnClickedCancelButton)
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


extern std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId);


// CAutoFocusDlg 消息处理程序

BOOL CAutoFocusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	::SetWindowTextW( ::GetDlgItem(this->m_hWnd, IDC_STATIC), GetIdsString(_T("autofocus"),_T("focusing")).c_str() );

	m_ProgressCtrl.SetRange(0,200);
	SetTimer(1,1000,NULL);

	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CAutoFocusMgr::AutoFocus, this->m_hWnd, 0, NULL);

	//::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CAutoFocusDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (1 == nIDEvent)
	{
		m_ProgressCtrl.StepIt();
	}

	CDialog::OnTimer(nIDEvent);
}
BOOL CAutoFocusDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message   ==   WM_KEYDOWN)   
	{   
		switch(pMsg->wParam)   
		{   
		case   VK_RETURN://屏蔽回车   
			return   TRUE;   

		case   VK_ESCAPE://屏蔽Esc   
			return   TRUE;   
		}   
	}   

	return CDialog::PreTranslateMessage(pMsg);
}


void CAutoFocusDlg::OnBnClickedCancelButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CAutoFocusMgr::CancelAutoFocus();
	::SetWindowTextW( ::GetDlgItem(this->m_hWnd, IDC_STATIC), GetIdsString(_T("autofocus"),_T("cancel")).c_str() );
	::EnableWindow( ::GetDlgItem(this->m_hWnd, IDC_CANCEL_BUTTON), FALSE);	
}

void CAutoFocusDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DWORD Result = 0;

	WaitForSingleObject(m_hThread, INFINITE);
	GetExitCodeThread(m_hThread, &Result);
	CloseHandle(m_hThread);

	if (Result != 0)
		AfxMessageBox( GetIdsString(_T("autofocus"),_T("fail")).c_str() );
	
	CDialog::OnClose();
}

void CAutoFocusDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ReleaseCapture();
	::SendMessage(this->m_hWnd, WM_SYSCOMMAND, 0xF012, 0);

	CDialog::OnLButtonDown(nFlags, point);
}
