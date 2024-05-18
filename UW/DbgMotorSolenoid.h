#pragma once
#include "resource.h"
#include "afxwin.h"
#include "CmdHelper.h"

// CDbgMotorSolenoid 对话框

class CDbgMotorSolenoid : public CPropertyPage
{
	DECLARE_DYNAMIC(CDbgMotorSolenoid)

public:
	CDbgMotorSolenoid();
	virtual ~CDbgMotorSolenoid();

// 对话框数据
	enum { IDD = IDD_DLG_DBG_MOTOR_AND_SOLENOID_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


private:
	MOTOR_ID m_nMotorId; // 电机ID
	MOTOR_ROTATION_DIRECTION m_nMotorDirection; // 电机方向
	MOTOR_SPEED m_nMotorSpeed; // 电机速度
	


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMotor0();
	afx_msg void OnBnClickedMotor1();
	afx_msg void OnBnClickedMotor2();
	afx_msg void OnBnClickedMotor3();
	afx_msg void OnBnClickedMotor4();
	afx_msg void OnBnClickedMotor5();
	afx_msg void OnBnClickedMotor6();
	afx_msg void OnBnClickedMotor7();
	afx_msg void OnBnClickedMotor8();
	afx_msg void OnBnClickedMotor9();
/*	afx_msg void OnBnClickedMotor10();*/
	afx_msg void OnBnClickedMotor11();
	afx_msg void OnBnClickedMotor12();
	afx_msg void OnBnClickedMotor13();
	afx_msg void OnBnClickedMotorDirection1();
	afx_msg void OnBnClickedMotorDirection2();
	afx_msg void OnBnClickedMotorSpeedLow();
	afx_msg void OnBnClickedMotorSpeedMid();
	afx_msg void OnBnClickedMotorSpeedHight();
	CComboBox m_cbxMotorStepCount;
	afx_msg void OnBnClickedSolenoid1();
	afx_msg void OnBnClickedSolenoid2();
	afx_msg void OnBnClickedSolenoid3();
	afx_msg void OnBnClickedSolenoid4();
	afx_msg void OnBnClickedSolenoid5();
	afx_msg void OnBnClickedSolenoid6();
	afx_msg void OnBnClickedSolenoid7();
	afx_msg void OnBnClickedSolenoid8();
	afx_msg void OnBnClickedSolenoid9();
	afx_msg void OnBnClickedSolenoid10();
	afx_msg void OnBnClickedSolenoid11();
	afx_msg void OnBnClickedMotorTest();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ReChildWndsize();
};
