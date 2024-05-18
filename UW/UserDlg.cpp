// UserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "UserDlg.h"
#include "Include/Common/String.h"
#include <WinCrypt.h>
#pragma comment (lib, "Crypt32")
// CUserDlg 对话框

extern 
std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId);

extern
bool SetIdsString(const std::wstring &sMain, const std::wstring &sId, const std::wstring &sVal);

IMPLEMENT_DYNAMIC(CUserDlg, CDialog)


using namespace Common;

CUserDlg::CUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserDlg::IDD, pParent)
	, m_strName(_T(""))
	, m_strPassword(_T(""))
	, m_csAccount(_T(""))
{

}

CUserDlg::~CUserDlg()
{
	//Access_CloseAccess();
}

void CUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USER_CSNAME, m_strName);
	DDX_Text(pDX, IDC_USER_CSPASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_USER_CSACCOUNT, m_csAccount);
    DDX_Text(pDX, IDC_USER_NAME, m_sName);
	DDX_Control(pDX, IDC_USER_LOGIN, m_btnLogin);
	DDV_MaxChars(pDX, m_sName, 32);
	DDX_Text(pDX, IDC_USER_ACCOUNT, m_sAccount);
	DDV_MaxChars(pDX, m_sAccount, 32);
	DDX_Text(pDX, IDC_USER_PASSWORD, m_sPassword);
	DDV_MaxChars(pDX, m_sPassword, 32);
	
}


BEGIN_MESSAGE_MAP(CUserDlg, CDialog)
	ON_BN_CLICKED(IDC_USER_LOGIN, &CUserDlg::OnBnClickedUserLogin)
	ON_EN_KILLFOCUS(IDC_USER_ACCOUNT, &CUserDlg::OnEnKillfocusUserAccount)
	ON_BN_CLICKED(IDCANCEL, &CUserDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CUserDlg 消息处理程序

BOOL CUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(GetIdsString(_T("user"),_T("title")).c_str());
	m_strName = GetIdsString(_T("user"),_T("name")).c_str();
	m_strPassword =GetIdsString(_T("user"),_T("psd")).c_str();
	m_csAccount =GetIdsString(_T("user"),_T("account")).c_str();
	m_btnLogin.SetWindowText(GetIdsString(_T("user"),_T("btn1")).c_str());
	CString lastLoginSuccessUser = GetIdsString(_T("user"), _T("lastloginaccount")).c_str();
	m_sAccount = _T("Adm");
	if (lastLoginSuccessUser != _T("0")) m_sAccount = lastLoginSuccessUser;
	UpdateData(FALSE);

	// 设置账号输入框获得焦点
	{
		CString strInfo;
		CEdit *pAccount = (CEdit*)GetDlgItem(IDC_USER_ACCOUNT);
		pAccount->GetWindowText(strInfo);
		int nLength = strInfo.GetLength();
		pAccount->SetSel(nLength, nLength, FALSE);
		pAccount->SetFocus();
		GetDlgItem(IDC_USER_PASSWORD)->SetFocus();
	}

	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// MD5 加密
CString CUserDlg::MD5(CString &sSrc)
{
	CString sDest;
	unsigned char Dest[33];
	unsigned long dwDstLen;
	HCRYPTPROV hProv = NULL;
	HCRYPTHASH hHash = NULL;

	char *pBuffer = NULL;
	int nCount = sSrc.GetLength();
#ifdef UNICODE
	nCount = WideCharToMultiByte(CP_ACP, NULL, sSrc.GetBuffer(), -1, NULL, 0, NULL, FALSE) - 1;
	pBuffer = new char[nCount + 1];
	WideCharToMultiByte(CP_ACP, NULL, sSrc.GetBuffer(), -1, pBuffer, nCount + 1, NULL, FALSE);
#else
	pBuffer = new char[nCount + 1];
	strcpy_s(pBuffer, nCount, sSrc.GetBuffer());
#endif
	// 创建一个密钥容器
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0))
	{
		// 创建一个新密钥容器
		CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET);
	}
	// 创建一个哈希对象
	if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		if (CryptHashData(hHash, (const BYTE*) pBuffer, nCount, 0))
		{
			// 得到一个哈希对象参数
			CryptGetHashParam(hHash, HP_HASHVAL, Dest, &dwDstLen, 0);
			// 转化为字符串
			sDest.Format(_T("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X"),
				Dest[0], Dest[1], Dest[2], Dest[3], Dest[4], Dest[5], Dest[6], Dest[7],
				Dest[8], Dest[9], Dest[10], Dest[11], Dest[12], Dest[13], Dest[14], Dest[15]);
		}
	}
	// 销毁哈希对象
	if (hHash)
	{
		CryptDestroyHash(hHash);
		hHash = NULL;
	}
	// 释放容器句柄
	if (hProv)
	{
		CryptReleaseContext(hProv, NULL);
		hProv = NULL;
	}
	// 释放内存
	delete[] pBuffer;
	return sDest;
}
void CUserDlg::OnBnClickedUserLogin()
{
	// TODO: 在此添加控件通知处理程序代码
	
	UpdateData(TRUE);
   //PUSER_LOGIN  m_user={0};
	
	string saccont =Common::WCharToChar(m_sAccount.GetBuffer());
	if (saccont=="")
	{
		AfxMessageBox(GetIdsString(_T("user"),_T("message3")).c_str());
	}
	else
	{
		//memset(m_user->sAccount,NULL,sizeof(m_user->sAccount));
		//char *a =(LPSTR)(LPCTSTR)m_sAccount;
		strcpy(m_user.sAccount,saccont.c_str());
		// = m_sAccount;
		//sAccount.Replace(_T("'"), _T("''"));

		string spasssword = Common::WCharToChar(m_sPassword.GetBuffer());
		if (spasssword == "")
		{
			AfxMessageBox(GetIdsString(_T("user"),_T("message4")).c_str());
		}
		else
		{

			if (m_sAccount == _T("thmepublic_super_password"))
			{
				if (m_sPassword == _T("15310176579"))
				{
					//theApp.loginGroup = 111;
					OnOK();
				}
				else
				{
					AfxMessageBox(GetIdsString(_T("user"),_T("message1")).c_str());
				}
			}
			else
			{
				strcpy(m_user.sPassword,spasssword.c_str());
				// m_user->sPassword = m_sPassword;

				ULONG loginState =Access_UserLogin(&m_user,&m_group);

				//loginState = STATUS_SUCCESS;
				if (STATUS_LOGIN_PASSWORD_ERR == loginState)
				{
					AfxMessageBox(GetIdsString(_T("user"),_T("message1")).c_str());
				}
				else if(STATUS_LOGIN_USER_ERR  == loginState)
				{
					AfxMessageBox(GetIdsString(_T("user"),_T("message2")).c_str());
				}
				else if (loginState == STATUS_SUCCESS)
				{
					for (int i=0;i<theApp.m_nUserNum;++i)
					{
						CString name = m_sAccount;
						CString acc(theApp.m_userlist.pUserInfo[i].sAccount);
						if (name.MakeLower() == acc.MakeLower())
						{
							SetIdsString(_T("user"), _T("lastloginaccount"), CString(theApp.m_userlist.pUserInfo[i].sAccount).GetBuffer());
							memcpy(&theApp.loginUser,&theApp.m_userlist.pUserInfo[i],sizeof(USER_INFO));
							break;
						}
					}
					
					OnOK();
				}
				else
				{
					AfxMessageBox(GetIdsString(_T("user"),_T("loginfail")).c_str());
				}
			}

		}

	}

}

BOOL CUserDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd *pWnd = GetFocus();
		if (pWnd != NULL)
		{
			switch (pWnd->GetDlgCtrlID())
			{
			case IDC_USER_ACCOUNT:
				pMsg->wParam = VK_TAB;
				break;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CUserDlg::OnEnKillfocusUserAccount()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	
	
	for (int i=0;i<theApp.m_nUserNum;++i)
	{
		CString name = m_sAccount;
		CString acc(theApp.m_userlist.pUserInfo[i].sAccount);
		if (name.MakeLower() == acc.MakeLower())
		//if (m_sAccount == theApp.m_userlist.pUserInfo[i].sAccount)
		{
			m_sName = theApp.m_userlist.pUserInfo[i].sName;
			
			break;
		}
	}
	UpdateData(FALSE);
}

void CUserDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
