// TipDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "TipDlg.h"


// CTipDlg 对话框

IMPLEMENT_DYNAMIC(CTipDlg, CDialog)

CTipDlg::CTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTipDlg::IDD, pParent)
{

}

CTipDlg::~CTipDlg()
{
}

void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIP_VLAUE, m_csTip);
}


BEGIN_MESSAGE_MAP(CTipDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTipDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTipDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CTipDlg 消息处理程序

void CTipDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CTipDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

BOOL CTipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Font.CreatePointFont(300,_T("ariial"));
	m_csTip.SetFont(&m_Font);

	m_csTip.SetWindowText(_T("耗材用完，请更换新的耗材"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
