// UserInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "thumbnail.h"
#include "DbgMsg.h"
#include "UserInfoDlg.h"


// CUserInfoDlg 对话框

IMPLEMENT_DYNAMIC(CUserInfoDlg, CDialog)

CUserInfoDlg::CUserInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserInfoDlg::IDD, pParent)
{
	m_sInfo = "";

}

CUserInfoDlg::~CUserInfoDlg()
{
	m_sInfo = "";

}

void CUserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INFO, m_edit_info);
}


BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CUserInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUserInfoDlg 消息处理程序


BOOL CUserInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_edit_info.SetWindowText(m_sInfo);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CUserInfoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CUserInfoDlg::SetInfo( CString sInfo )
{
	m_sInfo = sInfo;
DBG_MSG("m_sInfo=%s\n",m_sInfo);
	/*m_edit_info.UpdateData(FALSE);*/
}

void CUserInfoDlg::ReDrawInfo( void )
{
	
	
	m_edit_info.SetWindowText(m_sInfo);

}