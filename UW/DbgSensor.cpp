// DbgSensor.cpp : 实现文件
//

#include "stdafx.h"
#include "DbgSensor.h"
#include "afxdialogex.h"
#include "CmdHelper.h"
#include "UW.h"
#include "DbgMsg.h"
#include <algorithm>

#define WM_SENSORTEST WM_USER+0xC1

// CDbgSensor 对话框

IMPLEMENT_DYNAMIC(CDbgSensor, CPropertyPage)

CDbgSensor::CDbgSensor()
	: CPropertyPage(CDbgSensor::IDD)
{

}

CDbgSensor::~CDbgSensor()
{
}

void CDbgSensor::OnRecvSensorTestData(void *arg, void *cbarg)
{
	SendMessage(WM_SENSORTEST, (WPARAM)arg);
}

LRESULT CDbgSensor::RecvSensorTestDataHandler(WPARAM w, LPARAM l)
{
#if (_LC_V <= 2150)
	if (w)
	{
		PSENSOR_TEST_DATA_CONTEXT ctx = (PSENSOR_TEST_DATA_CONTEXT)w;

		DBG_MSG("下位机发送传感器消息：0x%x,0x%x,0x%x,0x%x", ctx->CleanLiquidSensor, ctx->TubeExistSensor, ctx->TubeShelfExistSensor, ctx->WastePaperBoxSensor);
		CString str;
		str.Format(_T("%d"), ctx->CleanLiquidSensor);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), ctx->TubeExistSensor);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), ctx->TubeShelfExistSensor);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), ctx->WastePaperBoxSensor);
		SetDlgItemText(IDC_EDIT13, str);
		str = ctx->SamplingTranslationReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT14, str);
		str = ctx->SamplingUpdownReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT15, str);
		str = ctx->SyringepumpReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT16, str);
		str = ctx->XAxisRest ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT17, str);
		str = ctx->YAxisABChannelReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT18, str);
		// 		str = ctx->YAxisCDChannelReset ? _T("已复位") : _T("未复位");
		// 		SetDlgItemText(IDC_EDIT19, str);
		str = ctx->ZAxisReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT20, str);
		// 		str = ctx->M10XLensReset ? _T("已到位") : _T("未到位");
		// 		SetDlgItemText(IDC_EDIT21, str);
		// 		str = ctx->M40XLensReset ? _T("已到位") : _T("未到位");
		// 		SetDlgItemText(IDC_EDIT22, str);
		str = ctx->PipeInReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT23, str);
		str = ctx->PipeOutReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT24, str);
		str = ctx->PipeStepReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT25, str);
		str = ctx->StepLeftReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT26, str);
		str = ctx->StepRightReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT27, str);
		str = ctx->TubeRotationReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT28, str);
		str = ctx->PipeInMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT29, str);
		str = ctx->PipeOUtMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT30, str);
		str = ctx->EmergencyMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT31, str);
		str = ctx->TubeExistDetect ? _T("有试管") : _T("无试管");
		SetDlgItemText(IDC_EDIT32, str);
		str = ctx->TubeShelfExistDetect ? _T("有试管架") : _T("无试管架");
		SetDlgItemText(IDC_EDIT33, str);
		str = ctx->CleanLiquidAlarm ? _T("液体满") : _T("液体空");
		SetDlgItemText(IDC_EDIT34, str);
		str = ctx->WastePaperBoxAlarm ? _T("废纸盒满") : _T("废纸盒空");
		SetDlgItemText(IDC_EDIT35, str);
	}
	return 0;

#elif (_LC_V >= 2260)
	if (w)
	{
		PSENSOR_TEST_DATA_CONTEXT ctx = (PSENSOR_TEST_DATA_CONTEXT)w;

		DBG_MSG("下位机发送传感器消息：0x%x,0x%x,0x%x,0x%x", ctx->CleanLiquidSensor, ctx->TubeExistDetect, ctx->TubeShelfExistSensor, ctx->WastePaperBoxSensor);
		CString str;
		str.Format(_T("%d"), ctx->CleanLiquidSensor);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), ctx->WastePaperBoxSensor);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), ctx->PaperFilpSensor);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), ctx->TubeShelfExistSensor);
		SetDlgItemText(IDC_EDIT44, str);

		str = ctx->SamplingTranslationReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT45, str);
		str = ctx->SamplingUpdownReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT13, str);
		str = ctx->SyringepumpReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT14, str);
		str = ctx->XAxisRest ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT15, str);
		str = ctx->YAxisABChannelReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT16, str);
		str = ctx->ZAxisReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT17, str);
		str = ctx->PipeInReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT18, str);
		str = ctx->PipeOutReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT20, str);
		str = ctx->PipeStepReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT23, str);
		str = ctx->StepLeftReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT24, str);
		str = ctx->StepRightReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT25, str);
		str = ctx->PipeInRechPos ? _T("已到位") : _T("未到位");
		SetDlgItemText(IDC_EDIT26, str);
		str = ctx->PipeOutFull ? _T("试管架满") : _T("试管架未满");
		SetDlgItemText(IDC_EDIT27, str);
		str = ctx->EmergencyMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT29, str);
		str = ctx->TubeExistDetect ? _T("有试管") : _T("无试管");
		SetDlgItemText(IDC_EDIT30, str);
		str = ctx->TubeShelfExistDetect ? _T("有试管架") : _T("无试管架");
		SetDlgItemText(IDC_EDIT31, str);
		str = ctx->CleanLiquidAlarm ? _T("液体满") : _T("液体空");
		SetDlgItemText(IDC_EDIT32, str);
		str = ctx->WastePaperBoxAlarm ? _T("废纸盒空") : _T("废纸盒满");
		SetDlgItemText(IDC_EDIT33, str);
		str = ctx->PaperFlipStatus ? _T("纸条正面") : _T("纸条反面");
		SetDlgItemText(IDC_EDIT34, str);
		str = ctx->WastePaperBoxAlarm ? _T("纸条歪斜") : _T("纸条正");
		SetDlgItemText(IDC_EDIT35, str);
	}
#elif (_LC_V >= 2180)
	if (w)
	{
		PSENSOR_TEST_DATA_CONTEXT ctx = (PSENSOR_TEST_DATA_CONTEXT)w;

		DBG_MSG("下位机发送传感器消息：0x%x,0x%x,0x%x,0x%x", ctx->CleanLiquidSensor, ctx->TubeExistSensor, ctx->TubeShelfExistSensor, ctx->WastePaperBoxSensor);
		CString str;
		str.Format(_T("%d"), ctx->CleanLiquidSensor);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), ctx->TubeExistSensor);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), ctx->TubeShelfExistSensor);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), ctx->WastePaperBoxSensor);
		SetDlgItemText(IDC_EDIT13, str);

		str.Format(_T("%d"), ctx->PaperFilpSensor);
		SetDlgItemText(IDC_EDIT44, str);
		str.Format(_T("%d"), ctx->SuctionSampleBoxSensor);
		SetDlgItemText(IDC_EDIT45, str);

		str = ctx->SamplingTranslationReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT14, str);
		str = ctx->SamplingUpdownReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT15, str);
		str = ctx->SyringepumpReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT16, str);
		str = ctx->XAxisRest ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT17, str);
		str = ctx->YAxisABChannelReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT18, str);
		// 		str = ctx->YAxisCDChannelReset ? _T("已复位") : _T("未复位");
		// 		SetDlgItemText(IDC_EDIT19, str);
		str = ctx->ZAxisReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT20, str);
		// 		str = ctx->M10XLensReset ? _T("已到位") : _T("未到位");
		// 		SetDlgItemText(IDC_EDIT21, str);
		// 		str = ctx->M40XLensReset ? _T("已到位") : _T("未到位");
		// 		SetDlgItemText(IDC_EDIT22, str);
		str = ctx->PipeInReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT23, str);
		str = ctx->PipeOutReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT24, str);
		str = ctx->PipeStepReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT25, str);
		str = ctx->StepLeftReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT26, str);
		str = ctx->StepRightReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT27, str);
		// 		str = ctx->TubeRotationReset ? _T("已复位") : _T("未复位");
		// 		SetDlgItemText(IDC_EDIT28, str);
		str = ctx->PipeInMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT29, str);
		str = ctx->PipeOUtMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT30, str);
		str = ctx->EmergencyMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT31, str);
		str = ctx->TubeExistDetect ? _T("有试管") : _T("无试管");
		SetDlgItemText(IDC_EDIT32, str);
		str = ctx->TubeShelfExistDetect ? _T("有试管架") : _T("无试管架");
		SetDlgItemText(IDC_EDIT33, str);
		str = ctx->CleanLiquidAlarm ? _T("液体满") : _T("液体空");
		SetDlgItemText(IDC_EDIT34, str);
		str = ctx->WastePaperBoxAlarm ? _T("废纸盒空") : _T("废纸盒满");
		SetDlgItemText(IDC_EDIT35, str);
		str = ctx->PaperFlipStatus ? _T("纸条正面") : _T("纸条反面");
		SetDlgItemText(IDC_EDIT47, str);
		str = ctx->SuctionSampleStatus ? _T("无液体") : _T("有液体");
		SetDlgItemText(IDC_EDIT48, str);
}

	return 0;
#else
	if (w)
	{
		PSENSOR_TEST_DATA_CONTEXT ctx = (PSENSOR_TEST_DATA_CONTEXT)w;

		DBG_MSG("下位机发送传感器消息：0x%x,0x%x,0x%x,0x%x", ctx->CleanLiquidSensor, ctx->TubeExistSensor, ctx->TubeShelfExistSensor, ctx->WastePaperBoxSensor);
		CString str;
		str.Format(_T("%d"), ctx->CleanLiquidSensor);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), ctx->TubeExistSensor);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), ctx->TubeShelfExistSensor);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), ctx->WastePaperBoxSensor);
		SetDlgItemText(IDC_EDIT13, str);

		str.Format(_T("%d"), ctx->PaperFilpSensor);
		SetDlgItemText(IDC_EDIT44, str);
		str.Format(_T("%d"), ctx->SuctionSampleBoxSensor);
		SetDlgItemText(IDC_EDIT45, str);
		str = ctx->SamplingTranslationReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT14, str);
		str = ctx->SamplingUpdownReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT15, str);
		str = ctx->SyringepumpReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT16, str);
		str = ctx->XAxisRest ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT17, str);
		str = ctx->YAxisABChannelReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT18, str);
		// 		str = ctx->YAxisCDChannelReset ? _T("已复位") : _T("未复位");
		// 		SetDlgItemText(IDC_EDIT19, str);
		str = ctx->ZAxisReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT20, str);
		// 		str = ctx->M10XLensReset ? _T("已到位") : _T("未到位");
		// 		SetDlgItemText(IDC_EDIT21, str);
		// 		str = ctx->M40XLensReset ? _T("已到位") : _T("未到位");
		// 		SetDlgItemText(IDC_EDIT22, str);
		str = ctx->PipeInReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT23, str);
		str = ctx->PipeOutReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT24, str);
		str = ctx->PipeStepReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT25, str);
		str = ctx->StepLeftReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT26, str);
		str = ctx->StepRightReset ? _T("已复位") : _T("未复位");
		SetDlgItemText(IDC_EDIT27, str);
		// 		str = ctx->TubeRotationReset ? _T("已复位") : _T("未复位");
		// 		SetDlgItemText(IDC_EDIT28, str);
		str = ctx->PipeInMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT29, str);
		str = ctx->PipeOUtMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT30, str);
		str = ctx->EmergencyMicroSwitch ? _T("已按下") : _T("未按下");
		SetDlgItemText(IDC_EDIT31, str);
		str = ctx->TubeExistDetect ? _T("有试管") : _T("无试管");
		SetDlgItemText(IDC_EDIT32, str);
		str = ctx->TubeShelfExistDetect ? _T("有试管架") : _T("无试管架");
		SetDlgItemText(IDC_EDIT33, str);
		str = ctx->CleanLiquidAlarm ? _T("液体满") : _T("液体空");
		SetDlgItemText(IDC_EDIT34, str);
		str = ctx->WastePaperBoxAlarm ? _T("废纸盒空") : _T("废纸盒满");
		SetDlgItemText(IDC_EDIT35, str);
		str = ctx->PaperFlipStatus ? _T("纸条正面") : _T("纸条反面");
		SetDlgItemText(IDC_EDIT47, str);
		str = ctx->SuctionSampleStatus ? _T("无液体") : _T("有液体");
		SetDlgItemText(IDC_EDIT48, str);
	}

	
#endif // LC_V1.8

	return 0;
}

void CDbgSensor::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDbgSensor, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CDbgSensor::OnBnClickedSensorStartTest)
	ON_BN_CLICKED(IDC_BUTTON13, &CDbgSensor::OnBnClickedSensorStopTest)
	ON_BN_CLICKED(IDC_BUTTON14, &CDbgSensor::OnBnClickedReadSensorConfig)
	ON_BN_CLICKED(IDC_BUTTON15, &CDbgSensor::OnBnClickedWirteSensorConfig)
	ON_MESSAGE(WM_SENSORTEST, &CDbgSensor::RecvSensorTestDataHandler)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT10, &CDbgSensor::OnEnChangeEdit10)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDbgSensor 消息处理程序


BOOL CDbgSensor::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO5))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO6))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO7))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO8))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO9))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO10))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO11))->SetCurSel(0);

	GetDlgItem(IDC_EDIT6)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT7)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT8)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT9)->SetWindowText(_T("0"));

	CMyDelegate1<CDbgSensor*, void (CDbgSensor::*)(void*, void*), void*> *e = new CMyDelegate1<CDbgSensor*, void (CDbgSensor::*)(void*, void*), void*>;
	e->RegisterEventHandler(this, &CDbgSensor::OnRecvSensorTestData, 0);
	theApp.m_clsMsgDipatcher.AddEventHandler(SensorTestData, e);

	m_mapEventDelegate[SensorTestData] = e;

	Init();

/*	ReChildWndsize();*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CDbgSensor::OnDestroy()
{
	CPropertyPage::OnDestroy();

	std::for_each(m_mapEventDelegate.begin(), m_mapEventDelegate.end(), [](std::map<int, void*>::reference r){
		theApp.m_clsMsgDipatcher.RemoveEventHandler(r.first, (CMyDelegate*)r.second);
	});
}

void CDbgSensor::OnBnClickedSensorStartTest()
{
	SensorTest();
}


void CDbgSensor::OnBnClickedSensorStopTest()
{
	SensorTest(false);
}


void CDbgSensor::OnBnClickedReadSensorConfig()
{
	SENSOR_CFG_CONTEXT cfg = { 0 };
	if (ReadSensorConfig(cfg))
	{
		CString str;

#if   (_LC_V <= 2150)
		str.Format(_T("%d"), cfg.CleanLiquidThreshold);
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);
		str.Format(_T("%d"), cfg.TubeExisthreshold);
		GetDlgItem(IDC_EDIT2)->SetWindowText(str);
		str.Format(_T("%d"), cfg.TubeShelfExisthreshold);
		GetDlgItem(IDC_EDIT3)->SetWindowText(str);
		str.Format(_T("%d"), cfg.WastePaperBoxhreshold);
		GetDlgItem(IDC_EDIT4)->SetWindowText(str);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.BuzzerAlarmEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(cfg.PipeInMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(cfg.TakeOutTubeWhenPipeInEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO5))->SetCurSel(cfg.PipeOutMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO6))->SetCurSel(cfg.EmergencyMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO7))->SetCurSel(cfg.ScanBarcodeTubeRationEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO8))->SetCurSel(cfg.ChannelAEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO9))->SetCurSel(cfg.ChannelBEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO10))->SetCurSel(cfg.ChannelCEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO11))->SetCurSel(cfg.ChannelDEnabled == 0 ? 0 : 1);

#elif (_LC_V >= 2260)
		str.Format(_T("%d"), cfg.CleanLiquidThreshold);
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);
		str.Format(_T("%d"), cfg.WastePaperBoxhreshold);
		GetDlgItem(IDC_EDIT2)->SetWindowText(str);
		str.Format(_T("%d"), cfg.PaperFliphreshold);
		GetDlgItem(IDC_EDIT3)->SetWindowText(str);
		str.Format(_T("%d"), cfg.PaperAskewthreshold);
		GetDlgItem(IDC_EDIT4)->SetWindowText(str);
		str.Format(_T("%d"), cfg.TubeShelfExisthreshold);
		GetDlgItem(IDC_EDIT50)->SetWindowText(str);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.PipeInMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(cfg.TakeOutTubeWhenPipeInEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(cfg.PipeOutMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO5))->SetCurSel(cfg.EmergencyMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO6))->SetCurSel(cfg.ScanBarcodeTubeRationEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO7))->SetCurSel(cfg.BuzzerAlarmEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO8))->SetCurSel(cfg.ChannelAEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO9))->SetCurSel(cfg.ChannelBEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO10))->SetCurSel(cfg.ChannelCEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO11))->SetCurSel(cfg.ChannelDEnabled == 0 ? 0 : 1);

#else
		str.Format(_T("%d"), cfg.CleanLiquidThreshold);
		GetDlgItem(IDC_EDIT1)->SetWindowText(str);
		str.Format(_T("%d"), cfg.WastePaperBoxhreshold);
		GetDlgItem(IDC_EDIT2)->SetWindowText(str);
		str.Format(_T("%d"), cfg.PaperFliphreshold);
		GetDlgItem(IDC_EDIT3)->SetWindowText(str);
		str.Format(_T("%d"), cfg.SuctionSamplehreshold);
		GetDlgItem(IDC_EDIT4)->SetWindowText(str);
		str.Format(_T("%d"), cfg.TubeExisthreshold);
		GetDlgItem(IDC_EDIT50)->SetWindowText(str);
		str.Format(_T("%d"), cfg.TubeShelfExisthreshold);
		GetDlgItem(IDC_EDIT53)->SetWindowText(str);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.PipeInMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(cfg.TakeOutTubeWhenPipeInEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(cfg.PipeOutMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO5))->SetCurSel(cfg.EmergencyMicroSwitchEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO6))->SetCurSel(cfg.ScanBarcodeTubeRationEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO7))->SetCurSel(cfg.BuzzerAlarmEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO8))->SetCurSel(cfg.ChannelAEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO9))->SetCurSel(cfg.ChannelBEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO10))->SetCurSel(cfg.ChannelCEnabled == 0 ? 0 : 1);
		((CComboBox*)GetDlgItem(IDC_COMBO11))->SetCurSel(cfg.ChannelDEnabled == 0 ? 0 : 1);

#endif // LC_V1.5
	}
	else
	{
		MessageBox(_T("读取传感器配置失败!"), _T("提示"), MB_OK | MB_ICONERROR);
	}
}


void CDbgSensor::OnBnClickedWirteSensorConfig()
{

#if (_LC_V <= 2150)
	bool bSuccess = false;
	CString str;
	SENSOR_CFG_CONTEXT cfg = { 0 };
	GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	cfg.CleanLiquidThreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT2)->GetWindowText(str);
	cfg.TubeExisthreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT3)->GetWindowText(str);
	cfg.TubeShelfExisthreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT4)->GetWindowText(str);
	cfg.WastePaperBoxhreshold = _ttoi(str);
	cfg.BuzzerAlarmEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();
	cfg.PipeInMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCurSel();
	cfg.TakeOutTubeWhenPipeInEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO4))->GetCurSel();
	cfg.PipeOutMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO5))->GetCurSel();
	cfg.EmergencyMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO6))->GetCurSel();
	cfg.ScanBarcodeTubeRationEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO7))->GetCurSel();
	cfg.ChannelAEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO8))->GetCurSel();
	cfg.ChannelBEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO9))->GetCurSel();
	cfg.ChannelCEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO10))->GetCurSel();
	cfg.ChannelDEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO11))->GetCurSel();
#elif (_LC_V >= 2260)
	bool bSuccess = false;
	CString str;
	SENSOR_CFG_CONTEXT cfg = { 0 };
	GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	cfg.CleanLiquidThreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT2)->GetWindowText(str);
	cfg.WastePaperBoxhreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT3)->GetWindowText(str);
	cfg.PaperFliphreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT4)->GetWindowText(str);
	cfg.PaperAskewthreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT50)->GetWindowText(str);
	cfg.TubeShelfExisthreshold = _ttoi(str);
	cfg.PipeInMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();
	cfg.TakeOutTubeWhenPipeInEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCurSel();
	cfg.PipeOutMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO4))->GetCurSel();
	cfg.EmergencyMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO5))->GetCurSel();
	cfg.ScanBarcodeTubeRationEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO6))->GetCurSel();
	cfg.BuzzerAlarmEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO7))->GetCurSel();
	cfg.ChannelAEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO8))->GetCurSel();
	cfg.ChannelBEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO9))->GetCurSel();
	cfg.ChannelCEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO10))->GetCurSel();
	cfg.ChannelDEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO11))->GetCurSel();
#else
	bool bSuccess = false;
	CString str;
	SENSOR_CFG_CONTEXT cfg = { 0 };
	GetDlgItem(IDC_EDIT1)->GetWindowText(str);
	cfg.CleanLiquidThreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT2)->GetWindowText(str);
	cfg.WastePaperBoxhreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT3)->GetWindowText(str);
	cfg.PaperFliphreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT4)->GetWindowText(str);
	cfg.SuctionSamplehreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT50)->GetWindowText(str);
	cfg.TubeExisthreshold = _ttoi(str);
	GetDlgItem(IDC_EDIT53)->GetWindowText(str);
	cfg.TubeShelfExisthreshold = _ttoi(str);
	cfg.PipeInMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO2))->GetCurSel();
	cfg.TakeOutTubeWhenPipeInEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCurSel();
	cfg.PipeOutMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO4))->GetCurSel();
	cfg.EmergencyMicroSwitchEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO5))->GetCurSel();
	cfg.ScanBarcodeTubeRationEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO6))->GetCurSel();
	cfg.BuzzerAlarmEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO7))->GetCurSel();
	cfg.ChannelAEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO8))->GetCurSel();
	cfg.ChannelBEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO9))->GetCurSel();
	cfg.ChannelCEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO10))->GetCurSel();
	cfg.ChannelDEnabled = ((CComboBox*)GetDlgItem(IDC_COMBO11))->GetCurSel();
	DBG_MSG("cfg: A:%d, B:%d, C:%d, D:%d\n", cfg.ChannelAEnabled, cfg.ChannelBEnabled, cfg.ChannelCEnabled, cfg.ChannelDEnabled);

#endif // LC_V1.5	
	WirteSensorConfig(cfg, bSuccess);
	MessageBox((bSuccess?_T("写入传感器配置成功!"):_T("写入传感器配置失败!")), _T("提示"), MB_OK | (bSuccess?MB_ICONINFORMATION:MB_ICONWARNING));
}





void CDbgSensor::OnEnChangeEdit10()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CDbgSensor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
#ifdef DEBUG
	return;
#endif // DEBUG
	if (bShow)
		OnBnClickedReadSensorConfig();

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

void CDbgSensor::ReChildWndsize()
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

#define MVOE_CTRL(ctrl_id) \
	GetDlgItem(ctrl_id)->GetWindowRect(rt); \
	ScreenToClient(rt); \
	GetDlgItem(ctrl_id)->SetWindowPos(0, rt.left, rt.top - topOffset, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

void CDbgSensor::Init()
{
	CRect rt1, rt2, rt;
	int topOffset = 0;
	
#if (_LC_V <= 2150)
	// config
	SetDlgItemText(IDC_STATIC_C_1, _T("清洗液门限:"));
	SetDlgItemText(IDC_STATIC_C_2, _T("试管有无门限:"));
	SetDlgItemText(IDC_STATIC_C_3, _T("试管架有无门限:"));
	SetDlgItemText(IDC_STATIC_C_4, _T("废纸盒门限:"));
	SetDlgItemText(IDC_STATIC_C_7, _T("蜂鸣器报警:"));
	SetDlgItemText(IDC_STATIC_C_8, _T("推进微动开关:"));
	SetDlgItemText(IDC_STATIC_C_9, _T("推进时试管架取出:"));
	SetDlgItemText(IDC_STATIC_C_10, _T("推出微动开关:"));
	SetDlgItemText(IDC_STATIC_C_11, _T("急诊微动开关:"));
	SetDlgItemText(IDC_STATIC_C_12, _T("扫描条码试管旋转:"));
	SetDlgItemText(IDC_STATIC_C_13, _T("计数池通道A:"));
	SetDlgItemText(IDC_STATIC_C_14, _T("计数池通道B:"));
	SetDlgItemText(IDC_STATIC_C_15, _T("计数池通道C:"));
	SetDlgItemText(IDC_STATIC_C_16, _T("计数池通道D:"));
	GetDlgItem(IDC_STATIC_C_5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT50)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_C_5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT53)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_C_5)->GetWindowRect(rt1);
	GetDlgItem(IDC_EDIT50)->GetWindowRect(rt2);
	ScreenToClient(rt1);
	ScreenToClient(rt2);
	GetDlgItem(IDC_STATIC_C_8)->GetWindowRect(rt);
	ScreenToClient(rt);
	topOffset = rt.top - rt1.top;
	GetDlgItem(IDC_STATIC_C_8)->SetWindowPos(0, rt.left, rt.top - topOffset, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	MVOE_CTRL(IDC_STATIC_C_9);
	MVOE_CTRL(IDC_STATIC_C_10);
	MVOE_CTRL(IDC_STATIC_C_11);
	MVOE_CTRL(IDC_STATIC_C_12);
	MVOE_CTRL(IDC_STATIC_C_13);
	MVOE_CTRL(IDC_STATIC_C_14);
	MVOE_CTRL(IDC_STATIC_C_15);
	MVOE_CTRL(IDC_STATIC_C_16);
	GetDlgItem(IDC_COMBO2)->GetWindowRect(rt);
	ScreenToClient(rt);
	topOffset = rt.top - rt2.top;
	GetDlgItem(IDC_COMBO2)->SetWindowPos(0, rt.left, rt.top - topOffset, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	MVOE_CTRL(IDC_COMBO3);
	MVOE_CTRL(IDC_COMBO4);
	MVOE_CTRL(IDC_COMBO5);
	MVOE_CTRL(IDC_COMBO6);
	MVOE_CTRL(IDC_COMBO7);
	MVOE_CTRL(IDC_COMBO8);
	MVOE_CTRL(IDC_COMBO9);
	MVOE_CTRL(IDC_COMBO10);
	MVOE_CTRL(IDC_COMBO11);

#elif (_LC_V >= 2260)
	// test result
	SetDlgItemText(IDC_STATIC_T_1, _T("清洗液传感器:"));
	SetDlgItemText(IDC_STATIC_T_2, _T("废纸盒传感器:"));
	SetDlgItemText(IDC_STATIC_T_3, _T("纸条翻转传感器:"));
	SetDlgItemText(IDC_STATIC_T_4, _T("试管架有无传感器:"));
	SetDlgItemText(IDC_STATIC_T_5, _T("采样平移复位:"));
	SetDlgItemText(IDC_STATIC_T_6, _T("采样升降复位:"));
	SetDlgItemText(IDC_STATIC_T_7, _T("注射泵复位:"));
	SetDlgItemText(IDC_STATIC_T_8, _T("X轴复位:"));
	SetDlgItemText(IDC_STATIC_T_9, _T("Y轴复位:"));
	SetDlgItemText(IDC_STATIC_T_10, _T("Z轴复位:"));
	SetDlgItemText(IDC_STATIC_T_11, _T("排管推进复位:"));
	SetDlgItemText(IDC_STATIC_T_12, _T("排管推出复位:"));
	SetDlgItemText(IDC_STATIC_T_14, _T("排管单步复位:"));
	SetDlgItemText(IDC_STATIC_T_15, _T("单步左复位:"));
	SetDlgItemText(IDC_STATIC_T_16, _T("单步右复位:"));
	SetDlgItemText(IDC_STATIC_T_17, _T("推进到位传感器:"));
	SetDlgItemText(IDC_STATIC_T_18, _T("推出满传感器:"));
	SetDlgItemText(IDC_STATIC_T_19, _T("急诊微动开关:"));
	SetDlgItemText(IDC_STATIC_T_20, _T("试管有无检测:"));
	SetDlgItemText(IDC_STATIC_T_21, _T("试管架有无检测:"));
	SetDlgItemText(IDC_STATIC_T_22, _T("清洗液报警:"));
	SetDlgItemText(IDC_STATIC_T_23, _T("废纸盒报警:"));
	SetDlgItemText(IDC_STATIC_T_24, _T("纸条翻转状态:"));
	SetDlgItemText(IDC_STATIC_T_25, _T("纸条歪斜状态:"));
	GetDlgItem(IDC_STATIC_T_26)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_T_27)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT47)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT48)->ShowWindow(SW_HIDE);

	// config
	SetDlgItemText(IDC_STATIC_C_1, _T("清洗液门限:"));
	SetDlgItemText(IDC_STATIC_C_2, _T("废纸盒门限:"));
	SetDlgItemText(IDC_STATIC_C_3, _T("纸条翻转门限:"));
	SetDlgItemText(IDC_STATIC_C_4, _T("纸条歪斜门限:"));
	SetDlgItemText(IDC_STATIC_C_5, _T("试管架有无门限:"));
	SetDlgItemText(IDC_STATIC_C_7, _T("推进微动开关:"));
	SetDlgItemText(IDC_STATIC_C_8, _T("推进时试管架取出:"));
	SetDlgItemText(IDC_STATIC_C_9, _T("推出微动开关:"));
	SetDlgItemText(IDC_STATIC_C_10, _T("急诊微动开关:"));
	SetDlgItemText(IDC_STATIC_C_11, _T("老化模拟测试:"));
	SetDlgItemText(IDC_STATIC_C_12, _T("蜂鸣器报警:"));
	SetDlgItemText(IDC_STATIC_C_13, _T("计数池通道A:"));
	SetDlgItemText(IDC_STATIC_C_14, _T("计数池通道B:"));
	SetDlgItemText(IDC_STATIC_C_15, _T("计数池通道C:"));
	SetDlgItemText(IDC_STATIC_C_16, _T("计数池通道D:"));
	GetDlgItem(IDC_STATIC_C_6)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT53)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_C_6)->GetWindowRect(rt1);
	GetDlgItem(IDC_EDIT53)->GetWindowRect(rt2);
	ScreenToClient(rt1);
	ScreenToClient(rt2);
	GetDlgItem(IDC_STATIC_C_7)->GetWindowRect(rt);
	ScreenToClient(rt);
	topOffset = rt.top - rt1.top;
	GetDlgItem(IDC_STATIC_C_7)->SetWindowPos(0, rt.left, rt.top - topOffset, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	MVOE_CTRL(IDC_STATIC_C_8);
	MVOE_CTRL(IDC_STATIC_C_9);
	MVOE_CTRL(IDC_STATIC_C_10);
	MVOE_CTRL(IDC_STATIC_C_11);
	MVOE_CTRL(IDC_STATIC_C_12);
	MVOE_CTRL(IDC_STATIC_C_13);
	MVOE_CTRL(IDC_STATIC_C_14);
	MVOE_CTRL(IDC_STATIC_C_15);
	MVOE_CTRL(IDC_STATIC_C_16);
	GetDlgItem(IDC_COMBO2)->GetWindowRect(rt);
	ScreenToClient(rt);
	topOffset = rt.top - rt2.top;
	GetDlgItem(IDC_COMBO2)->SetWindowPos(0, rt.left, rt.top - topOffset, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	MVOE_CTRL(IDC_COMBO3);
	MVOE_CTRL(IDC_COMBO4);
	MVOE_CTRL(IDC_COMBO5);
	MVOE_CTRL(IDC_COMBO6);
	MVOE_CTRL(IDC_COMBO7);
	MVOE_CTRL(IDC_COMBO8);
	MVOE_CTRL(IDC_COMBO9);
	MVOE_CTRL(IDC_COMBO10);
	MVOE_CTRL(IDC_COMBO11);

#else
	// config
	SetDlgItemText(IDC_STATIC_C_1, _T("清洗液门限:"));
	SetDlgItemText(IDC_STATIC_C_2, _T("废纸盒门限:"));
	SetDlgItemText(IDC_STATIC_C_3, _T("纸条翻转门限:"));
	SetDlgItemText(IDC_STATIC_C_4, _T("吸样管路空门限:"));
	SetDlgItemText(IDC_STATIC_C_5, _T("试管有无门限:"));
	SetDlgItemText(IDC_STATIC_C_6, _T("试管架有无门限:"));
	SetDlgItemText(IDC_STATIC_C_7, _T("推进微动开关:"));
	SetDlgItemText(IDC_STATIC_C_8, _T("推进时试管架取出:"));
	SetDlgItemText(IDC_STATIC_C_9, _T("推出微动开关:"));
	SetDlgItemText(IDC_STATIC_C_10, _T("急诊微动开关:"));
	SetDlgItemText(IDC_STATIC_C_11, _T("老化模拟测试:"));
	SetDlgItemText(IDC_STATIC_C_12, _T("蜂鸣器报警:"));
	SetDlgItemText(IDC_STATIC_C_13, _T("计数池通道A:"));
	SetDlgItemText(IDC_STATIC_C_14, _T("计数池通道B:"));
	SetDlgItemText(IDC_STATIC_C_15, _T("计数池通道C:"));
	SetDlgItemText(IDC_STATIC_C_16, _T("计数池通道D:"));

#endif
}
