#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CUserManageDlg 对话框

class CUserManageDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserManageDlg)

public:
	CUserManageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserManageDlg();

// 对话框数据
	enum { IDD = IDD_USER_MANAGE_DIALOG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_nameList;
	CListCtrl m_depList;
	Dept_Info  dep;
	Pos_Info   pos;
	void initDepList();
	void initNameList();
	void FillUser(TCHAR *dep);
	void FillDept(int nIndex);
	virtual BOOL OnInitDialog();
public:
	

	afx_msg void OnLvnItemchangedUserdepList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedUsernameList(NMHDR *pNMHDR, LRESULT *pResult);

public:
	int   m_nDept;
	int   m_nUser;
	afx_msg void OnBnClickedUsermanageNew();
	afx_msg void OnBnClickedUsermanageDel();
	afx_msg void OnBnClickedUsermanageModify();
	afx_msg void OnBnClickedUsermanageFinish();
	afx_msg void OnBnClickedButtonPwd();
	CButton m_addBtn;
	CButton m_delBtn;
	CButton m_editBtn;
	CButton m_modifyPwdBtn;
};
