#pragma once
#include "resource.h"

// CInsertQcTask 对话框

class CInsertQcTask : public CDialogEx
{
	DECLARE_DYNAMIC(CInsertQcTask)

public:
	CInsertQcTask(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInsertQcTask();

// 对话框数据
	enum { IDD = IDD_DIALOG_INSERT_QC_TASK  };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CComboBox m_qcType;
	int m_curSel;
	int m_InsertSn;
	afx_msg void OnBnClickedButton13();
	virtual BOOL OnInitDialog();
	CString m_insertDate;
	CString m_InsertTime;
	CDateTimeCtrl m_date;
	CDateTimeCtrl m_time;
};
