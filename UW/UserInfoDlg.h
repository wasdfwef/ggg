#pragma once
#include "afxwin.h"


// CUserInfoDlg 对话框

class CUserInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserInfoDlg)

public:
	CUserInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserInfoDlg();

// 对话框数据
	enum { IDD = IDD_USERINFO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString                  m_sDep;
	CString                  m_sName;
	CString                  m_sPosition;
	CString                  m_sAccount;
	BOOL                     m_bResetPassword;
	int                      m_nGroup;
	CString                  strAccount;
	CString m_csDetail;
	CString m_csDep;
	CString m_csName;
	CString m_csPos;
	CString m_csAccount;
	CString m_csPwd;
	CString m_csGroup;
	virtual BOOL OnInitDialog();
	CComboBox m_cboDep;
	CString m_strName;
	CComboBox m_cboPosition;
	CComboBox m_cboGroup;
	CString m_strAccount;
	CButton m_btnPwd;
	BOOL m_bNew;
	ULONG AddUser();
	ULONG EditUser();
	bool CheckeDataValid();
	
	afx_msg void OnBnClickedUserinofBtnOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeUserinofCboDep();
	CString m_NewPwd;
	afx_msg void OnBnClickedUserinofCheckPwd();
};
