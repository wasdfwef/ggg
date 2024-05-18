#pragma once


#include "afxwin.h"


// CSetDialog 对话框

class CSetDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetDialog)

public:
	CSetDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetDialog();

// 对话框数据
	enum { IDD = IDD_SET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedBtnUdc();
	afx_msg void OnBnClickedBtnDoctor();
	afx_msg void OnBnClickedSetUsermanage();
	afx_msg void OnBnClickedSetModifypassword();
	CButton m_btnUserManage;
	CButton m_btnRemoveall;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnColor();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedS();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedSetModifypassword2();
	afx_msg void OnBnClickedButton43();
};
