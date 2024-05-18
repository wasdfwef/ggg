#pragma once
#include "resource.h"
#include "CmdHelper.h"
#include "UW.h"
#include "afxwin.h"

// CDbgHardwareparam 对话框

class CDbgHardwareparam : public CPropertyPage
{
	DECLARE_DYNAMIC(CDbgHardwareparam)

public:
	CDbgHardwareparam();
	virtual ~CDbgHardwareparam();

// 对话框数据
	enum { IDD = IDD_DLG_DBG_HARDWARE_PARAM };

private:
	void initParamCtrl();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void GetHardwareVersion();

	afx_msg void OnBnClickedResetHardwareParam();
	afx_msg void OnBnClickedReadHardwareParam();
	afx_msg void OnBnClickedWriteHardwareParam();
	afx_msg void OnBnClickedButton15();

	bool SaveDefaultParam(HARDWARE_PARAM_CONTEXT); 
	bool ReadDefaultParam(HARDWARE_PARAM_CONTEXT*);
	afx_msg void OnBnClickedButton31();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CButton m_readParamBtn;
	CButton m_writeParamBtn;
	CButton m_resetParamBtn;
	CButton m_saveDefaultBtn;
	CButton m_readDefaultBtn;
	afx_msg void OnBnClickedButton2();
};
