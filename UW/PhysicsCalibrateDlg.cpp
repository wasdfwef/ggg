// PhysicsCalibrateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "PhysicsCalibrateDlg.h"
#include "afxdialogex.h"


// CPhysicsCalibrateDlg 对话框

IMPLEMENT_DYNAMIC(CPhysicsCalibrateDlg, CDialogEx)

CPhysicsCalibrateDlg::CPhysicsCalibrateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPhysicsCalibrateDlg::IDD, pParent)
{

}

CPhysicsCalibrateDlg::~CPhysicsCalibrateDlg()
{
}

void CPhysicsCalibrateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SG_L, m_btn_sg_l_calibrate);
	DDX_Control(pDX, IDC_BUTTON_SG_H, m_btn_sg_h_calibrate);
	DDX_Control(pDX, IDC_BUTTON_TURBICITY_L, m_btn_turbidity_l_calibrate);
	DDX_Control(pDX, IDC_BUTTON_TURBICITY_H, m_btn_turbidity_h_calibrate);
	DDX_Control(pDX, IDC_BUTTON_COLOR, m_btn_color_calibrate);
	DDX_Control(pDX, IDC_BUTTON_ZUIGUANG, m_btn_zuiguang);
	DDX_Control(pDX, IDC_BUTTON_TEST, m_btn_test);
	DDX_Control(pDX, IDC_STATIC_SG, m_static_sg);
	DDX_Control(pDX, IDC_STATIC_TURBICITY, m_static_turbidity);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_static_color);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_static_test_value);
}


BEGIN_MESSAGE_MAP(CPhysicsCalibrateDlg, CDialogEx)	
	ON_BN_CLICKED(IDC_BUTTON_SG_L, &CPhysicsCalibrateDlg::OnBnClickedButtonSgL)
	ON_BN_CLICKED(IDC_BUTTON_SG_H, &CPhysicsCalibrateDlg::OnBnClickedButtonSgH)
	ON_BN_CLICKED(IDC_BUTTON_TURBICITY_L, &CPhysicsCalibrateDlg::OnBnClickedButtonTurbicityL)
	ON_BN_CLICKED(IDC_BUTTON_TURBICITY_H, &CPhysicsCalibrateDlg::OnBnClickedButtonTurbicityH)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, &CPhysicsCalibrateDlg::OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_ZUIGUANG, &CPhysicsCalibrateDlg::OnBnClickedButtonZuiguang)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CPhysicsCalibrateDlg::OnBnClickedButtonTest)
END_MESSAGE_MAP()


// CPhysicsCalibrateDlg 消息处理程序

void CPhysicsCalibrateDlg::bEnableBtn(bool bEnableBtn)
{
	m_btn_sg_l_calibrate.EnableWindow(bEnableBtn);
	m_btn_sg_h_calibrate.EnableWindow(bEnableBtn);
	m_btn_turbidity_l_calibrate.EnableWindow(bEnableBtn);
	m_btn_turbidity_h_calibrate.EnableWindow(bEnableBtn);
	m_btn_color_calibrate.EnableWindow(bEnableBtn);
	m_btn_zuiguang.EnableWindow(bEnableBtn);
	m_btn_test.EnableWindow(bEnableBtn);
}



void CPhysicsCalibrateDlg::OnBnClickedButtonSgL()
{
	// TODO:  在此添加控件通知处理程序代码
	//if (AfxMessageBox(_T("请放入低比重液进行校准")) == IDOK)
	//{
	//	CString str;
	//	int nSgType = 0;
	//	SG_CALIBRATE_CONTEXT SgCalibrateResult = { 0 };

	//	bEnableBtn(false);
	//	PhysicsSgCalibrate(nSgType, SgCalibrateResult);
	//	str.Format(_T("校准结果：%d"), SgCalibrateResult.nValue);	
	//	m_static_sg.SetWindowText(str);
	//	bEnableBtn(true);
	//}
}


void CPhysicsCalibrateDlg::OnBnClickedButtonSgH()
{
	// TODO:  在此添加控件通知处理程序代码
	//if (AfxMessageBox(_T("请放入高比重液进行校准")) == IDOK)
	//{
	//	CString str;
	//	int nSgType = 1;
	//	SG_CALIBRATE_CONTEXT SgCalibrateResult = { 0 };

	//	bEnableBtn(false);
	//	PhysicsSgCalibrate(nSgType, SgCalibrateResult);
	//	str.Format(_T("校准结果：%d"), SgCalibrateResult.nValue);
	//	m_static_sg.SetWindowText(str);
	//	bEnableBtn(true);
	//}
}


void CPhysicsCalibrateDlg::OnBnClickedButtonTurbicityL()
{
	// TODO:  在此添加控件通知处理程序代码
	/*if (AfxMessageBox(_T("请放入低浊度液进行校准")) == IDOK)
	{
		CString str;
		int nTurbidityType = 0;
		SG_CALIBRATE_CONTEXT TurbidityCalibrateResult = { 0 };

		bEnableBtn(false);
		PhysicsTurbidityCalibrate(nTurbidityType, TurbidityCalibrateResult);
		str.Format(_T("校准结果：%d"), TurbidityCalibrateResult.nValue);
		m_static_turbidity.SetWindowText(str);
		bEnableBtn(true);
	}*/
}


void CPhysicsCalibrateDlg::OnBnClickedButtonTurbicityH()
{
	// TODO:  在此添加控件通知处理程序代码
	//if (AfxMessageBox(_T("请放入高浊度液进行校准")) == IDOK)
	//{
	//	CString str;
	//	int nTurbidityType = 0;
	//	SG_CALIBRATE_CONTEXT TurbidityCalibrateResult = { 0 };

	//	bEnableBtn(false);
	//	PhysicsTurbidityCalibrate(nTurbidityType, TurbidityCalibrateResult);
	//	str.Format(_T("校准结果：%d"), TurbidityCalibrateResult.nValue);
	//	m_static_turbidity.SetWindowText(str);
	//	bEnableBtn(true);
	//}
}


void CPhysicsCalibrateDlg::OnBnClickedButtonColor()
{
	// TODO:  在此添加控件通知处理程序代码
	/*if (AfxMessageBox(_T("是否进行物理三项颜色校准")) == IDOK)
	{
		CString str;
		COLOR_CALIBRATE_CONTEXT ColorCalibrateResult = { 0 };

		bEnableBtn(false);
		PhysicsColorCalibrate(ColorCalibrateResult);

		str.Format(_T("校准结果：增益 %d,追光 %d/r/n白光暗点 %d,白光亮点 %d/r/n红光暗点 %d,红光亮点 %d/r/n绿光暗点 %d,绿光亮点 %d/r/n蓝光暗点 %d,蓝光亮点 %d"), ColorCalibrateResult.nGain, ColorCalibrateResult.nZuiguang,
			ColorCalibrateResult.nWhite_d, ColorCalibrateResult.nWhite_l, ColorCalibrateResult.nRed_d, ColorCalibrateResult.nRed_l,
			ColorCalibrateResult.nGreen_d, ColorCalibrateResult.nGreen_l, ColorCalibrateResult.nBlue_d, ColorCalibrateResult.nBlue_l );
		m_static_color.SetWindowText(str);

		bEnableBtn(true);
	}*/
}


void CPhysicsCalibrateDlg::OnBnClickedButtonZuiguang()
{
	// TODO:  在此添加控件通知处理程序代码
	//if (AfxMessageBox(_T("是否进行物理三项颜色追光白平衡")) == IDOK)
	//{
	//	CString str;
	//	COLOR_ZUIGUANG_CONTEXT ColorZuiGuangResult = { 0 };

	//	bEnableBtn(false);
	//	PhysicsColorZuiGuang(ColorZuiGuangResult);

	//	str.Format(_T("追光结果：增益 %d/r/n追光 %d"), ColorZuiGuangResult.nGain, ColorZuiGuangResult.nZuiguang);
	//	m_static_color.SetWindowText(str);

	//	bEnableBtn(true);
	//}
}


void CPhysicsCalibrateDlg::OnBnClickedButtonTest()
{
	// TODO:  在此添加控件通知处理程序代码
	//PHYSICS_RESULT_CONTEXT PhysicsTestResult = {0};
	//CString str;

	//bEnableBtn(false);
	//PhysicsTest(PhysicsTestResult);
	//str.Format(_T("测试结果：比重 %d,比重原始值a %d/r/n比重原始值b %d"), PhysicsTestResult.sg_test, PhysicsTestResult.sg_original_a, PhysicsTestResult.sg_original_b);
	//m_static_color.SetWindowText(str);
	//bEnableBtn(true);
}
