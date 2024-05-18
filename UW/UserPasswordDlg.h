#pragma once
#include "afxwin.h"


// CUserPasswordDlg 对话框

class CUserPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserPasswordDlg)

public:
	CUserPasswordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserPasswordDlg();

// 对话框数据
	enum { IDD = IDD_USERPWD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_csName;
	CString m_csAccount;
	CString m_csPwd;
	CString m_csNewPwd;
	CString m_csRePwd;
	CString m_sName;
	CString m_sAccount;
	CString m_sOldPwd;
	CString m_sNewPwd;
	CString m_sRePwd;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedUserpwdBtnModify();
	CEdit m_editOldPwd;
};
