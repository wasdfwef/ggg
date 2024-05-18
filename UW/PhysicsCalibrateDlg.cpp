// PhysicsCalibrateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UW.h"
#include "PhysicsCalibrateDlg.h"
#include "afxdialogex.h"


// CPhysicsCalibrateDlg �Ի���

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


// CPhysicsCalibrateDlg ��Ϣ�������

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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//if (AfxMessageBox(_T("�����ͱ���Һ����У׼")) == IDOK)
	//{
	//	CString str;
	//	int nSgType = 0;
	//	SG_CALIBRATE_CONTEXT SgCalibrateResult = { 0 };

	//	bEnableBtn(false);
	//	PhysicsSgCalibrate(nSgType, SgCalibrateResult);
	//	str.Format(_T("У׼�����%d"), SgCalibrateResult.nValue);	
	//	m_static_sg.SetWindowText(str);
	//	bEnableBtn(true);
	//}
}


void CPhysicsCalibrateDlg::OnBnClickedButtonSgH()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//if (AfxMessageBox(_T("�����߱���Һ����У׼")) == IDOK)
	//{
	//	CString str;
	//	int nSgType = 1;
	//	SG_CALIBRATE_CONTEXT SgCalibrateResult = { 0 };

	//	bEnableBtn(false);
	//	PhysicsSgCalibrate(nSgType, SgCalibrateResult);
	//	str.Format(_T("У׼�����%d"), SgCalibrateResult.nValue);
	//	m_static_sg.SetWindowText(str);
	//	bEnableBtn(true);
	//}
}


void CPhysicsCalibrateDlg::OnBnClickedButtonTurbicityL()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	/*if (AfxMessageBox(_T("�������Ƕ�Һ����У׼")) == IDOK)
	{
		CString str;
		int nTurbidityType = 0;
		SG_CALIBRATE_CONTEXT TurbidityCalibrateResult = { 0 };

		bEnableBtn(false);
		PhysicsTurbidityCalibrate(nTurbidityType, TurbidityCalibrateResult);
		str.Format(_T("У׼�����%d"), TurbidityCalibrateResult.nValue);
		m_static_turbidity.SetWindowText(str);
		bEnableBtn(true);
	}*/
}


void CPhysicsCalibrateDlg::OnBnClickedButtonTurbicityH()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//if (AfxMessageBox(_T("�������Ƕ�Һ����У׼")) == IDOK)
	//{
	//	CString str;
	//	int nTurbidityType = 0;
	//	SG_CALIBRATE_CONTEXT TurbidityCalibrateResult = { 0 };

	//	bEnableBtn(false);
	//	PhysicsTurbidityCalibrate(nTurbidityType, TurbidityCalibrateResult);
	//	str.Format(_T("У׼�����%d"), TurbidityCalibrateResult.nValue);
	//	m_static_turbidity.SetWindowText(str);
	//	bEnableBtn(true);
	//}
}


void CPhysicsCalibrateDlg::OnBnClickedButtonColor()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	/*if (AfxMessageBox(_T("�Ƿ��������������ɫУ׼")) == IDOK)
	{
		CString str;
		COLOR_CALIBRATE_CONTEXT ColorCalibrateResult = { 0 };

		bEnableBtn(false);
		PhysicsColorCalibrate(ColorCalibrateResult);

		str.Format(_T("У׼��������� %d,׷�� %d/r/n�׹ⰵ�� %d,�׹����� %d/r/n��ⰵ�� %d,������� %d/r/n�̹ⰵ�� %d,�̹����� %d/r/n���ⰵ�� %d,�������� %d"), ColorCalibrateResult.nGain, ColorCalibrateResult.nZuiguang,
			ColorCalibrateResult.nWhite_d, ColorCalibrateResult.nWhite_l, ColorCalibrateResult.nRed_d, ColorCalibrateResult.nRed_l,
			ColorCalibrateResult.nGreen_d, ColorCalibrateResult.nGreen_l, ColorCalibrateResult.nBlue_d, ColorCalibrateResult.nBlue_l );
		m_static_color.SetWindowText(str);

		bEnableBtn(true);
	}*/
}


void CPhysicsCalibrateDlg::OnBnClickedButtonZuiguang()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//if (AfxMessageBox(_T("�Ƿ��������������ɫ׷���ƽ��")) == IDOK)
	//{
	//	CString str;
	//	COLOR_ZUIGUANG_CONTEXT ColorZuiGuangResult = { 0 };

	//	bEnableBtn(false);
	//	PhysicsColorZuiGuang(ColorZuiGuangResult);

	//	str.Format(_T("׷���������� %d/r/n׷�� %d"), ColorZuiGuangResult.nGain, ColorZuiGuangResult.nZuiguang);
	//	m_static_color.SetWindowText(str);

	//	bEnableBtn(true);
	//}
}


void CPhysicsCalibrateDlg::OnBnClickedButtonTest()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//PHYSICS_RESULT_CONTEXT PhysicsTestResult = {0};
	//CString str;

	//bEnableBtn(false);
	//PhysicsTest(PhysicsTestResult);
	//str.Format(_T("���Խ�������� %d,����ԭʼֵa %d/r/n����ԭʼֵb %d"), PhysicsTestResult.sg_test, PhysicsTestResult.sg_original_a, PhysicsTestResult.sg_original_b);
	//m_static_color.SetWindowText(str);
	//bEnableBtn(true);
}
