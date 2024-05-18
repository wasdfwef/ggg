#pragma once
#include "afxwin.h"


// CUserDlg 对话框

class CUserDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserDlg)

public:
	CUserDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserDlg();

// 对话框数据
	enum { IDD = IDD_USER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	// MD5 加密
	static CString MD5(CString &sSrc);
public:
	CString m_strName;
	CString m_strPassword;
public:
	CString                  m_sName;     // 用户名
	CString                  m_sAccount;  // 账号
	CString                  m_sPassword; // 密码
	//int                      m_nGroup;    // 组
	USER_LOGIN               m_user;
	
	int                      m_group;
public:
	BOOL                     m_bLogout;
	CButton m_btnLogin;
	CString m_csAccount;
	afx_msg void OnBnClickedUserLogin();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnKillfocusUserAccount();
	afx_msg void OnBnClickedCancel();
};
