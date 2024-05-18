#pragma once
#include "resource.h"

// DbgPassword 对话框

class DbgPassword : public CDialogEx
{
	DECLARE_DYNAMIC(DbgPassword)

public:
	DbgPassword(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DbgPassword();

// 对话框数据
	enum { IDD = IDD_DIALOG_DBGPASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_password;
	afx_msg void OnBnClickedButton1();
};
