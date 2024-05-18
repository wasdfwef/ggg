#pragma once
#include "afxwin.h"


// CSetting 对话框

class CSetting : public CDialog
{
	DECLARE_DYNAMIC(CSetting)

public:
	CSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetting();

// 对话框数据
	enum { IDD = IDD_SYS_ROW_NUMS_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void GetLisConfigPath(char* path);
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_row_set_edit;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CEdit m_sn_nums_set_edit;
	CEdit m_Save_Nums_Set_Edit;
	CButton m_auto_sendto_lis_check;
	afx_msg void OnBnClickedAutoSendToLisCheck();
	int m_lisTimeInterval;
	BOOL m_defaulTimeInterval;
	afx_msg void OnBnClickedCheck2();
};
