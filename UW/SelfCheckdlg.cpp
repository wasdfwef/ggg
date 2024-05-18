// SelfCheckdlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "SelfCheckdlg.h"


// CSelfCheckdlg 对话框

IMPLEMENT_DYNAMIC(CSelfCheckdlg, CDialog)

CSelfCheckdlg::CSelfCheckdlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelfCheckdlg::IDD, pParent)
{

}

CSelfCheckdlg::~CSelfCheckdlg()
{
}

void CSelfCheckdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_csInfo);
	DDX_Control(pDX, IDC_PROGRESS2, m_Progress);
}


BEGIN_MESSAGE_MAP(CSelfCheckdlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CSelfCheckdlg::ShowMessage( CString str )
{
	m_csInfo.SetWindowText(str);
}


// CSelfCheckdlg 消息处理程序

BOOL CSelfCheckdlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ShowMessage(str);
	m_Progress.SetRange(0,200);
	m_nMax=200;
	m_nStep=10;
	SetTimer(4,1000,NULL);

	ModifyStyleEx(WS_EX_APPWINDOW, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
} 

void CSelfCheckdlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (4 == nIDEvent)
	{
		 int nPrePos=m_Progress.StepIt();
	}
	CDialog::OnTimer(nIDEvent);
}

void CSelfCheckdlg::ShowModal(const CString &str)
{
	this->str = str;
	if (IsWindow(GetSafeHwnd()) && IsWindowVisible())
	{
		m_csInfo.SetWindowText(str);
	}
	else
	{
		DoModal();
	}
}

void CSelfCheckdlg::HideModal()
{
	if (IsWindow(GetSafeHwnd()))
	{
		EndDialog(0);
	}
}

BOOL   CSelfCheckdlg::PreTranslateMessage(MSG*   pMsg)     
{   
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
	return   CDialog::PreTranslateMessage(pMsg);   
}
