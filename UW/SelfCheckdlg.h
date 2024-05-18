#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSelfCheckdlg 对话框

class CSelfCheckdlg : public CDialog
{
	DECLARE_DYNAMIC(CSelfCheckdlg)

public:
	CSelfCheckdlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelfCheckdlg();

// 对话框数据
	enum { IDD = IDD_SELFCHECK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG*   pMsg);  

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_csInfo;
	CString str;
	int m_nMax,m_nStep;
	void ShowMessage(CString str);
	virtual BOOL OnInitDialog();
	CProgressCtrl m_Progress;
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void ShowModal(const CString &str);
	void HideModal();

};
