// UserPasswordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "UserPasswordDlg.h"
#include "Include/Common/String.h"

// CUserPasswordDlg 对话框
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);
IMPLEMENT_DYNAMIC(CUserPasswordDlg, CDialog)

CUserPasswordDlg::CUserPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserPasswordDlg::IDD, pParent)
	, m_csName(_T(""))
	, m_csAccount(_T(""))
	, m_csPwd(_T(""))
	, m_csNewPwd(_T(""))
	, m_csRePwd(_T(""))
	, m_sName(_T(""))
	, m_sAccount(_T(""))
	, m_sOldPwd(_T(""))
	, m_sNewPwd(_T(""))
	, m_sRePwd(_T(""))
{

}

CUserPasswordDlg::~CUserPasswordDlg()
{
}

void CUserPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERPWD_CSNAME, m_csName);
	DDX_Text(pDX, IDC_USERPWD_CSACCOUNT, m_csAccount);
	DDX_Text(pDX, IDC_USERPWD_CSPWD, m_csPwd);
	DDX_Text(pDX, IDC_USERPWD_CSNEWPWD, m_csNewPwd);
	DDX_Text(pDX, IDC_USERPWD_CSREPWD, m_csRePwd);
	DDX_Text(pDX, IDC_USERPWD_EDIT_NAME, m_sName);
	DDX_Text(pDX, IDC_USERPWD_EDIT_ACCOUNT, m_sAccount);
	DDX_Text(pDX, IDC_USERPWD_EDIT_OLDPWD, m_sOldPwd);
	DDX_Text(pDX, IDC_USERPWD_EDIT_NEWPWD, m_sNewPwd);
	DDX_Text(pDX, IDC_USERPWD_EDIT_REPWD, m_sRePwd);
	DDX_Control(pDX, IDC_USERPWD_EDIT_OLDPWD, m_editOldPwd);
}


BEGIN_MESSAGE_MAP(CUserPasswordDlg, CDialog)
	ON_BN_CLICKED(IDC_USERPWD_BTN_MODIFY, &CUserPasswordDlg::OnBnClickedUserpwdBtnModify)
END_MESSAGE_MAP()


// CUserPasswordDlg 消息处理程序

BOOL CUserPasswordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
      SetWindowText(GetIdsString(_T("userpassword"),_T("title")).c_str());
	  m_csName = GetIdsString(_T("userpassword"),_T("name")).c_str();
	  m_csAccount = GetIdsString(_T("userpassword"),_T("account")).c_str();
	  m_csPwd = GetIdsString(_T("userpassword"),_T("pwd")).c_str();
	  m_csNewPwd = GetIdsString(_T("userpassword"),_T("newpwd")).c_str();
	  m_csRePwd= GetIdsString(_T("userpassword"),_T("repwd")).c_str();
	  if (m_sName.IsEmpty()) m_sName = theApp.loginUser.sName;
	  if (m_sAccount.IsEmpty()) m_sAccount = theApp.loginUser.sAccount;
	  UpdateData(FALSE);

	  if (theApp.loginGroup == 111 || theApp.loginGroup == 4)
	  {
		  m_editOldPwd.EnableWindow(FALSE);
	  }

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CUserPasswordDlg::OnBnClickedUserpwdBtnModify()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_sOldPwd;
	m_sNewPwd;
	m_sRePwd;
	bool hasAdminRight = (theApp.loginGroup == 111 || theApp.loginGroup == 4);
	UpdateData(FALSE);

	if (hasAdminRight)
	{
		if (m_sNewPwd.GetLength() == 0 || m_sRePwd.GetLength() == 0)
		{
			AfxMessageBox(GetIdsString(_T("userpassword"), _T("message")).c_str());
			return;
		}
	}
	else
	{
		if (m_sOldPwd.GetLength() == 0 || m_sNewPwd.GetLength() == 0 || m_sRePwd.GetLength() == 0)
		{
			AfxMessageBox(GetIdsString(_T("userpassword"), _T("message")).c_str());
			return;
		}
	}

	if (m_sRePwd.Compare(m_sNewPwd))
	{
		AfxMessageBox(GetIdsString(_T("userpassword"),_T("message1")).c_str());
		return;
	}
	
	USER_UPC upc;
    strcpy(upc.UserName,Common::WCharToChar(m_sAccount.GetBuffer()).c_str());
	strcpy(upc.NewPsd,Common::WCharToChar(m_sNewPwd.GetBuffer()).c_str());
	if (hasAdminRight)
	{
		for (int i = 0; i < theApp.m_nUserNum; ++i)
		{
			if (m_sAccount == Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sAccount).c_str())
			{
				strcpy(upc.OldPsd, theApp.m_userlist.pUserInfo[i].sPassword);
				break;
			}
		}
	}
	else
	{
		strcpy(upc.OldPsd, Common::WCharToChar(m_sOldPwd.GetBuffer()).c_str());
	}

	switch (Access_ModifyPwd(&upc))
	{
	case STATUS_PWD_ERROR:
		AfxMessageBox(GetIdsString(_T("userpassword"),_T("message2")).c_str());
		m_sOldPwd =_T("");
		break;
	case STATUS_SUCCESS:
		AfxMessageBox(GetIdsString(_T("userpassword"),_T("message3")).c_str());
		OnOK();
		break;
	case STATUS_MODIFY_PWD_FAILED:
		AfxMessageBox(GetIdsString(_T("userpassword"),_T("message4")).c_str());
		break;
	}
	

	//return Access_ModifyUserInfo(&userinfo);
}
