#pragma once
#include "resource.h"
#include "afxwin.h"

// CMotorConfPage 对话框

class CMotorConfPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMotorConfPage)

public:
	CMotorConfPage();
	virtual ~CMotorConfPage();

// 对话框数据
	enum { IDD = IDD_DIALOG_MOTOR_CONF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_com1;
	CComboBox m_com2;
	CComboBox m_com3;
	CComboBox m_com4;
	CComboBox m_com5;
	CComboBox m_com6;
	CComboBox m_com7;
	CComboBox m_com8;
	CComboBox m_com9;
	CComboBox m_com10;
	CComboBox m_com11;
	CComboBox m_com12;
	int m_run1;
	int m_run2;
	int m_run3;
	int m_run4;
	int m_run5;
	int m_run6;
	int m_run7;
	int m_run8;
	int m_run9;
	int m_run10;
	int m_run11;
	int m_run12;
	int m_standby1;
	int m_standby2;
	int m_standby3;
	int m_standby4;
	int m_standby5;
	int m_standby6;
	int m_standby7;
	int m_standby8;
	int m_standby9;
	int m_standby10;
	int m_standby11;
	int m_standby12;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton30();
	afx_msg void OnBnClickedButton42();
	void AddStringToComBo(CComboBox& com);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CComboBox m_cbxDirect1;
	CComboBox m_cbxDirect2;
	CComboBox m_cbxDirect3;
	CComboBox m_cbxDirect4;
	CComboBox m_cbxDirect5;
	CComboBox m_cbxDirect6;
	CComboBox m_cbxDirect7;
	CComboBox m_cbxDirect8;
	CComboBox m_cbxDirect9;
	CComboBox m_cbxDirect10;
	CComboBox m_cbxDirect11;
	CComboBox m_cbxDirect12;
	void InitDirectComboxCtrl();
};
