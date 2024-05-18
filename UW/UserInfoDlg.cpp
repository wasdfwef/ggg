// UserInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "UserInfoDlg.h"
#include "Include/Common/String.h"

// CUserInfoDlg 对话框
extern 
std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId);

IMPLEMENT_DYNAMIC(CUserInfoDlg, CDialog)

CUserInfoDlg::CUserInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserInfoDlg::IDD, pParent)
	, m_csDetail(_T(""))
	, m_csDep(_T(""))
	, m_strName(_T(""))
	, m_strAccount(_T(""))
	, m_sDep(_T(""))
	, m_NewPwd(_T(""))
{

}

CUserInfoDlg::~CUserInfoDlg()
{
}

void CUserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERINFO_DETAIL, m_csDetail);
	DDX_Text(pDX, IDC_USERINFO_DEP, m_csDep);
	DDX_Text(pDX, IDC_USERINFO_NAME, m_csName);
	DDX_Text(pDX, IDC_USERINFO_POS, m_csPos);
	DDX_Text(pDX, IDC_USERINFO_ACCOUNT, m_csAccount);
	DDX_Text(pDX, IDC_USERINFO_PASSWORD, m_csPwd);
	DDX_Text(pDX, IDC_USERINFO_GROUP, m_csGroup);
	DDX_Control(pDX, IDC_USERINOF_CBO_DEP, m_cboDep);
	DDX_CBString(pDX, IDC_USERINOF_CBO_DEP, m_sDep);
	DDV_MaxChars(pDX, m_sDep, 64);
	DDX_Text(pDX, IDC_USERINOF_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 32);
	DDX_Control(pDX, IDC_USERINOF_CBO_POS, m_cboPosition);
	DDX_CBString(pDX, IDC_USERINOF_CBO_POS, m_sPosition);
	DDV_MaxChars(pDX, m_sPosition, 64);
	DDX_Control(pDX, IDC_USERINOF_CBO_GROUP, m_cboGroup);
	DDX_Text(pDX, IDC_USERINOF_EDIT_ACCOUNT, m_strAccount);
	DDV_MaxChars(pDX, m_strAccount, 32);
	DDX_Control(pDX, IDC_USERINOF_CHECK_PWD, m_btnPwd);

	DDX_Text(pDX, IDC_USERINOF_EDIT_PASSWORD, m_NewPwd);
}


BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialog)
	ON_BN_CLICKED(IDC_USERINOF_BTN_OK, &CUserInfoDlg::OnBnClickedUserinofBtnOk)
	ON_BN_CLICKED(IDCANCEL, &CUserInfoDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_USERINOF_CBO_DEP, &CUserInfoDlg::OnCbnSelchangeUserinofCboDep)
	ON_BN_CLICKED(IDC_USERINOF_CHECK_PWD, &CUserInfoDlg::OnBnClickedUserinofCheckPwd)
END_MESSAGE_MAP()


// CUserInfoDlg 消息处理程序

BOOL CUserInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(GetIdsString(_T("userinfo"),_T("title")).c_str());
	UpdateData(TRUE);
    m_csDetail = GetIdsString(_T("userinfo"),_T("title1")).c_str();
	m_csDep = GetIdsString(_T("userinfo"),_T("dep")).c_str();
	m_csName = GetIdsString(_T("userinfo"),_T("name")).c_str();
	m_csPos = GetIdsString(_T("userinfo"),_T("position")).c_str();
	m_csPwd = GetIdsString(_T("userinfo"),_T("password")).c_str();
	m_csGroup=GetIdsString(_T("userinfo"),_T("group")).c_str();
	m_csAccount =GetIdsString(_T("userinfo"),_T("account")).c_str();
    m_btnPwd.SetWindowText(GetIdsString(_T("userinfo"),_T("psd")).c_str());
	GetDlgItem(IDC_USERINOF_EDIT_PASSWORD)->EnableWindow(TRUE);
	Dept_Info  dep;
	Pos_Info   pos;
	Access_GetDeptAndPosInfo(&dep,&pos);

    for (int i = 0; i< dep.nCount;++i)
    {
         m_cboDep.InsertString(i,Common::CharToWChar(dep.pDept[i].name).c_str());
    }

	for (int j= 0; j< pos.nCount;++j)
	{
		m_cboPosition.InsertString(j,Common::CharToWChar(pos.pPos[j].name).c_str());
	}

	m_cboDep.SetCurSel(0);
	m_cboGroup.InsertString(0,_T("用户组"));
	if (theApp.loginGroup == 111 || theApp.loginGroup == 4)
	{
		m_cboGroup.InsertString(1, _T("管理员"));
	}

    if (m_bNew)
    {
		m_cboGroup.SetCurSel(0);
    }
	else
	{
		for (int i = 0; i<theApp.m_nUserNum;++i)
		{
			if (strAccount == Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sAccount).c_str())
			{
				
				m_sDep = Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sDept).c_str();
				m_strName = Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sName).c_str();
				m_strAccount = Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sAccount).c_str();
				m_sPosition = Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sPosition).c_str();
				GetDlgItem(IDC_USERINOF_EDIT_PASSWORD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_USERINOF_CHECK_PWD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_USERINFO_PASSWORD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_USERINOF_EDIT_ACCOUNT)->EnableWindow(FALSE);
				if (theApp.m_userlist.pUserInfo[i].Group == 4)
				{
					m_cboGroup.SetCurSel(1);
				}
				else
				{
					m_cboGroup.SetCurSel(0);
				}
				

				
				break;
			}
		}
	}
    UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//增加用户
ULONG CUserInfoDlg::AddUser()
{
	USER_ADD user;
	m_cboDep.GetWindowText(m_sDep);
	m_cboPosition.GetWindowText(m_sPosition);
	strcpy(user.sDept,Common::WCharToChar(m_sDep.GetBuffer()).c_str());
    strcpy(user.sAccount,Common::WCharToChar(m_strAccount.GetBuffer()).c_str());
	strcpy(user.sPosition,Common::WCharToChar(m_sPosition.GetBuffer()).c_str());
	strcpy(user.sName,Common::WCharToChar(m_strName.GetBuffer()).c_str());
	if (m_btnPwd.GetCheck())
	{
		strcpy(user.sPassword, Common::WCharToChar(L"123456").c_str());
	}
	else
	{
		strcpy(user.sPassword, Common::WCharToChar(m_NewPwd.GetBuffer()).c_str());
	}
	//CString strGroup;
	//m_cboGroup.GetWindowText(strGroup);
    int nGroup = m_cboGroup.GetCurSel();
	if (nGroup == 0)
	{ 
		user.nGroupe = 0;
	}
	else if (nGroup == 1)
	{
		user.nGroupe = 4;
	}
	
   

	ULONG valueAdd = Access_UserAdd(&user);

	return valueAdd;
}


// 修改用户
ULONG CUserInfoDlg::EditUser()
{
	USER_INFO userinfo;
	m_cboDep.GetWindowText(m_sDep);
	m_cboPosition.GetWindowText(m_sPosition);
	UpdateData(TRUE);
	strcpy(userinfo.sAccount,Common::WCharToChar(m_strAccount.GetBuffer()).c_str());
	strcpy(userinfo.sDept,Common::WCharToChar(m_sDep.GetBuffer()).c_str());
	strcpy(userinfo.sPosition,Common::WCharToChar(m_sPosition.GetBuffer()).c_str());
    strcpy(userinfo.sName,Common::WCharToChar(m_strName.GetBuffer()).c_str());
	//是否修改密码
	if (m_btnPwd.GetCheck())
	{
		strcpy(userinfo.sPassword,"123456");
	}
	else
	{
		char pwd[64] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, m_NewPwd,m_NewPwd.GetLength(),pwd, 64, NULL, NULL);
		strcpy(userinfo.sPassword,pwd);
	}
	int nGroup = m_cboGroup.GetCurSel();
	if (nGroup == 0)
	{ 
		userinfo.Group = 0;
	}
	else if (nGroup == 1)
	{
		userinfo.Group = 4;
	}

    return Access_ModifyUserInfo(&userinfo);
	
}


bool CUserInfoDlg::CheckeDataValid()
{
	USER_INFO userinfo;


    m_cboDep.GetWindowText(m_sDep);

	if( m_sDep == _T("") )
	{
		AfxMessageBox(GetIdsString(_T("warning"),_T("invailddep")).c_str());
		return false;
	}

	if ( strlen(Common::WCharToChar(m_strAccount.GetBuffer()).c_str()) + 1 > sizeof(userinfo.sAccount) )
	{
		AfxMessageBox(GetIdsString(_T("warning"),_T("invaildaccountlen")).c_str());
		return false;
	}

	if ( strlen(Common::WCharToChar(m_sDep.GetBuffer()).c_str()) + 1 > sizeof(userinfo.sDept) )
	{
		AfxMessageBox(GetIdsString(_T("warning"),_T("invailddeptlen")).c_str());
		return false;
	}

	if ( strlen(Common::WCharToChar(m_sPosition.GetBuffer()).c_str()) + 1 > sizeof(userinfo.sPosition) )
	{
		AfxMessageBox(GetIdsString(_T("warning"),_T("invaildposlen")).c_str());
		return false;
	}

	if ( strlen(Common::WCharToChar(m_strName.GetBuffer()).c_str()) + 1 > sizeof(userinfo.sName) )
	{
		AfxMessageBox(GetIdsString(_T("warning"),_T("invaildnamelen")).c_str());
		return false;
	}

	return true;
}


void CUserInfoDlg::OnBnClickedUserinofBtnOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	

	if (m_bNew)
	{
		if( !CheckeDataValid() )
			return;

		switch(AddUser())
		{
		case STATUS_SUCCESS:
			AfxMessageBox(GetIdsString(_T("userinfo"),_T("message4")).c_str());
			OnOK();
			break;
		case STATUS_LOGIN_USER_EXIST:
			AfxMessageBox(GetIdsString(_T("userinfo"),_T("message5")).c_str());
			break;
		}
	}
	else
	{
		switch (EditUser())
		{
		case STATUS_SUCCESS:
			AfxMessageBox(GetIdsString(_T("usermanage"),_T("message2")).c_str());
			OnOK();
			break;
		case STATUS_INVALID_PARAMETERS:
			AfxMessageBox(GetIdsString(_T("usermanage"),_T("message3")).c_str());
			break;
		}
	}
}

void CUserInfoDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CUserInfoDlg::OnCbnSelchangeUserinofCboDep()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CUserInfoDlg::OnBnClickedUserinofCheckPwd()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_btnPwd.GetCheck())
	{
		GetDlgItem(IDC_USERINOF_EDIT_PASSWORD)->SetWindowText(L"");
		GetDlgItem(IDC_USERINOF_EDIT_PASSWORD)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_USERINOF_EDIT_PASSWORD)->EnableWindow(TRUE);
	}
}
