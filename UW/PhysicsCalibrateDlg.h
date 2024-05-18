#pragma once
#include "afxwin.h"


// CPhysicsCalibrateDlg 对话框

class CPhysicsCalibrateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPhysicsCalibrateDlg)

public:
	CPhysicsCalibrateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPhysicsCalibrateDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PHYSICS_CALIBRATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton m_btn_sg_l_calibrate;
	CButton m_btn_sg_h_calibrate;
	CButton m_btn_turbidity_l_calibrate;
	CButton m_btn_turbidity_h_calibrate;
	CButton m_btn_color_calibrate;
	CButton m_btn_zuiguang;
	CButton m_btn_test;

	void bEnableBtn(bool bEnableBtn = true);

	afx_msg void OnBnClickedButtonSgL();
	afx_msg void OnBnClickedButtonSgH();
	afx_msg void OnBnClickedButtonTurbicityL();
	afx_msg void OnBnClickedButtonTurbicityH();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonZuiguang();
	afx_msg void OnBnClickedButtonTest();
	CStatic m_static_sg;
	CStatic m_static_turbidity;
	CStatic m_static_color;
	CStatic m_static_test_value;
};
