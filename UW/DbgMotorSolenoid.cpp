// DbgMotorSolenoid.cpp : 实现文件
//

#include "stdafx.h"
#include "DbgMotorSolenoid.h"
#include "afxdialogex.h"
#include "CmdHelper.h"


// CDbgMotorSolenoid 对话框

IMPLEMENT_DYNAMIC(CDbgMotorSolenoid, CPropertyPage)

CDbgMotorSolenoid::CDbgMotorSolenoid()
	: CPropertyPage(CDbgMotorSolenoid::IDD)
{
	m_nMotorId = SAMPLING_TRANSLATION_MOTOR;
	m_nMotorDirection = MOTOR_DIRECTION_REVERSE;
	m_nMotorSpeed = MOTOR_SPEED_LOW;
}

CDbgMotorSolenoid::~CDbgMotorSolenoid()
{
}

void CDbgMotorSolenoid::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO3, m_cbxMotorStepCount);
}


BEGIN_MESSAGE_MAP(CDbgMotorSolenoid, CPropertyPage)
	ON_BN_CLICKED(IDC_RADIO1, &CDbgMotorSolenoid::OnBnClickedMotor0)
	ON_BN_CLICKED(IDC_RADIO2, &CDbgMotorSolenoid::OnBnClickedMotor1)
	ON_BN_CLICKED(IDC_RADIO3, &CDbgMotorSolenoid::OnBnClickedMotor2)
	ON_BN_CLICKED(IDC_RADIO4, &CDbgMotorSolenoid::OnBnClickedMotor3)
	ON_BN_CLICKED(IDC_RADIO5, &CDbgMotorSolenoid::OnBnClickedMotor4)
	ON_BN_CLICKED(IDC_RADIO6, &CDbgMotorSolenoid::OnBnClickedMotor5)
	ON_BN_CLICKED(IDC_RADIO7, &CDbgMotorSolenoid::OnBnClickedMotor6)
	ON_BN_CLICKED(IDC_RADIO8, &CDbgMotorSolenoid::OnBnClickedMotor7)
	ON_BN_CLICKED(IDC_RADIO9, &CDbgMotorSolenoid::OnBnClickedMotor8)
	ON_BN_CLICKED(IDC_RADIO10, &CDbgMotorSolenoid::OnBnClickedMotor9)
/*	ON_BN_CLICKED(IDC_RADIO11, &CDbgMotorSolenoid::OnBnClickedMotor10)*/
	ON_BN_CLICKED(IDC_RADIO12, &CDbgMotorSolenoid::OnBnClickedMotor11)
	ON_BN_CLICKED(IDC_RADIO13, &CDbgMotorSolenoid::OnBnClickedMotor12)
	ON_BN_CLICKED(IDC_RADIO14, &CDbgMotorSolenoid::OnBnClickedMotor13)
	ON_BN_CLICKED(IDC_RADIO20, &CDbgMotorSolenoid::OnBnClickedMotorDirection1)
	ON_BN_CLICKED(IDC_RADIO21, &CDbgMotorSolenoid::OnBnClickedMotorDirection2)
	ON_BN_CLICKED(IDC_RADIO22, &CDbgMotorSolenoid::OnBnClickedMotorSpeedLow)
	ON_BN_CLICKED(IDC_RADIO23, &CDbgMotorSolenoid::OnBnClickedMotorSpeedMid)
	ON_BN_CLICKED(IDC_RADIO24, &CDbgMotorSolenoid::OnBnClickedMotorSpeedHight)
	ON_BN_CLICKED(IDC_BUTTON2, &CDbgMotorSolenoid::OnBnClickedSolenoid1)
	ON_BN_CLICKED(IDC_BUTTON3, &CDbgMotorSolenoid::OnBnClickedSolenoid2)
	ON_BN_CLICKED(IDC_BUTTON4, &CDbgMotorSolenoid::OnBnClickedSolenoid3)
	ON_BN_CLICKED(IDC_BUTTON5, &CDbgMotorSolenoid::OnBnClickedSolenoid4)
	ON_BN_CLICKED(IDC_BUTTON6, &CDbgMotorSolenoid::OnBnClickedSolenoid5)
	ON_BN_CLICKED(IDC_BUTTON7, &CDbgMotorSolenoid::OnBnClickedSolenoid6)
	ON_BN_CLICKED(IDC_BUTTON8, &CDbgMotorSolenoid::OnBnClickedSolenoid7)
	ON_BN_CLICKED(IDC_BUTTON9, &CDbgMotorSolenoid::OnBnClickedSolenoid8)
	ON_BN_CLICKED(IDC_BUTTON10, &CDbgMotorSolenoid::OnBnClickedSolenoid9)
	ON_BN_CLICKED(IDC_BUTTON11, &CDbgMotorSolenoid::OnBnClickedSolenoid10)
	ON_BN_CLICKED(IDC_BUTTON12, &CDbgMotorSolenoid::OnBnClickedSolenoid11)
	ON_BN_CLICKED(IDC_BUTTON1, &CDbgMotorSolenoid::OnBnClickedMotorTest)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDbgMotorSolenoid 消息处理程序


BOOL CDbgMotorSolenoid::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

/*	ReChildWndsize();*/
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO20))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO22))->SetCheck(TRUE);
	((CComboBox *)GetDlgItem(IDC_COMBO3))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CDbgMotorSolenoid::OnBnClickedMotor0()
{
	m_nMotorId = SAMPLING_TRANSLATION_MOTOR;
}

void CDbgMotorSolenoid::OnBnClickedMotor1()
{
	m_nMotorId = SAMPLING_UPDOWN_MOTOR;
}

void CDbgMotorSolenoid::OnBnClickedMotor2()
{
	m_nMotorId = SYRINGEPUMP_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor3()
{
	m_nMotorId = XAXIS_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor4()
{
	m_nMotorId = YAXIS_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor5()
{
	m_nMotorId = ZAXIS_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor6()
{
	m_nMotorId = CHANGE_MLENS_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor7()
{
	m_nMotorId = PIPE_IN_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor8()
{
	m_nMotorId = PIPE_OUT_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor9()
{
	m_nMotorId = PIPE_STEP_MOTOR;
}


// void CDbgMotorSolenoid::OnBnClickedMotor10()
// {
// 	m_nMotorId = TUBE_ROTATION_MOTOR;
// }


void CDbgMotorSolenoid::OnBnClickedMotor11()
{
	m_nMotorId = CLEANPUMP1_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor12()
{
	m_nMotorId = CLEANPUPM2_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotor13()
{
	m_nMotorId = WASTE_LIQUID_PUPM_MOTOR;
}


void CDbgMotorSolenoid::OnBnClickedMotorDirection1()
{
	m_nMotorDirection = MOTOR_DIRECTION_REVERSE;
}


void CDbgMotorSolenoid::OnBnClickedMotorDirection2()
{
	m_nMotorDirection = MOTOR_DIRECTION_FORWARD;
}


void CDbgMotorSolenoid::OnBnClickedMotorSpeedLow()
{
	m_nMotorSpeed = MOTOR_SPEED_LOW;
}


void CDbgMotorSolenoid::OnBnClickedMotorSpeedMid()
{
	m_nMotorSpeed = MOTOR_SPEED_MID;
}


void CDbgMotorSolenoid::OnBnClickedMotorSpeedHight()
{
	m_nMotorSpeed = MOTOR_SPEED_HEIGHT;
}


void CDbgMotorSolenoid::OnBnClickedSolenoid1()
{
	SolenoidTest(SOLENOID_ID_1);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid2()
{
	SolenoidTest(SOLENOID_ID_2);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid3()
{
	SolenoidTest(SOLENOID_ID_3);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid4()
{
	SolenoidTest(SOLENOID_ID_4);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid5()
{
	SolenoidTest(SOLENOID_ID_5);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid6()
{
	SolenoidTest(SOLENOID_ID_6);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid7()
{
	SolenoidTest(SOLENOID_ID_7);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid8()
{
	SolenoidTest(SOLENOID_ID_8);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid9()
{
	SolenoidTest(SOLENOID_ID_9);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid10()
{
	SolenoidTest(SOLENOID_ID_10);
}


void CDbgMotorSolenoid::OnBnClickedSolenoid11()
{
	SolenoidTest(SOLENOID_ID_11);
}


void CDbgMotorSolenoid::OnBnClickedMotorTest()
{
	unsigned short m_nMotorStepCount = 0; // 电机步数
	CString str;
	m_cbxMotorStepCount.GetWindowText(str);
	m_nMotorStepCount = _ttoi(str);
	MotorTest(m_nMotorId, m_nMotorDirection, m_nMotorSpeed, m_nMotorStepCount);
}


void CDbgMotorSolenoid::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
// 	RECT rect;
// 	GetParent()->GetWindowRect(&rect);
// 	int nWidth = rect.right - rect.left;
// 	int nHeight = rect.bottom - rect.top;
// 	if (bShow)
// 	{
// 		GetParent()->GetParent()->SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
// 	}
	// TODO:  在此处添加消息处理程序代码
}

void CDbgMotorSolenoid::ReChildWndsize()
{
	CRect rect;
	CRect childSaved;
	float x = 0.8;
	float y = 0.8;
	GetWindowRect(rect);
	int wndWidth = GetSystemMetrics(SM_CXSCREEN);  //屏幕宽度    
	int wndHeight = GetSystemMetrics(SM_CYSCREEN);

	CWnd* pWnd = GetWindow(GW_CHILD);//获取子窗体  

	while (pWnd)
	{

		pWnd->GetWindowRect(childSaved);
		if (rect.Width() > wndWidth * 2 / 3 || rect.Height() > wndHeight * 2 / 3);
		{
			childSaved.left *= x;//根据比例调整控件上下左右距离对话框的距离  
			childSaved.right *= x;
			childSaved.top *= y;
			childSaved.bottom *= y;
			ScreenToClient(childSaved);
			pWnd->MoveWindow(childSaved);
			pWnd = pWnd->GetNextWindow();//取下一个子窗体  
		}
	}
	GetWindowRect(rect);
	ScreenToClient(rect);
	MoveWindow(rect.top, rect.left, rect.Width()*x, rect.Height()*y);
}
