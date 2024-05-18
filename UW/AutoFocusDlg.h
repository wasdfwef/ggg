#pragma once
#include "afxcmn.h"
#include "resource.h" 

// CAutoFocusDlg 对话框

class CAutoFocusDlg : public CDialog
{
	DECLARE_DYNAMIC(CAutoFocusDlg)

public:
	CAutoFocusDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAutoFocusDlg();

// 对话框数据
	enum { IDD = IDD_AUTO_FOCUS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CProgressCtrl m_ProgressCtrl;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCancelButton();

private:
	HANDLE m_hThread;
public:
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
