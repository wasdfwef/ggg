#pragma once
#include "resource.h"

// CSoftwareSetUI 对话框

class CSoftwareSetUI : public CPropertyPage
{
	DECLARE_DYNAMIC(CSoftwareSetUI)

public:
	CSoftwareSetUI();   // 标准构造函数
	virtual ~CSoftwareSetUI();

// 对话框数据
	enum { IDD = IDD_DIALOG_UISET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnBnClickedButton1();
};
