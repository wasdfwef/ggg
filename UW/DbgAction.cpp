// DbgAction.cpp : 实现文件
//

#include "stdafx.h"
#include "DbgAction.h"
#include "afxdialogex.h"
#include "CmdHelper.h"
#include <algorithm>
#include "UW.h"

#define WM_PHYSICSSGSENSOR WM_USER+0xD1
// CDbgAction 对话框

IMPLEMENT_DYNAMIC(CDbgAction, CPropertyPage)

CDbgAction::CDbgAction()
	: CPropertyPage(CDbgAction::IDD)
{

}

CDbgAction::~CDbgAction()
{
}

void CDbgAction::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SG_RESULT, m_static_sg_result);
}


BEGIN_MESSAGE_MAP(CDbgAction, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CDbgAction::OnBnClickedSysReset)
	ON_BN_CLICKED(IDC_BUTTON2, &CDbgAction::OnBnClickedAspirationReset)
	ON_BN_CLICKED(IDC_BUTTON3, &CDbgAction::OnBnClickedSamplingTranslationReset)
	ON_BN_CLICKED(IDC_BUTTON4, &CDbgAction::OnBnClickedSamplingUpdownReset)
	ON_BN_CLICKED(IDC_BUTTON5, &CDbgAction::OnBnClickedToCleanPostion)
	ON_BN_CLICKED(IDC_BUTTON6, &CDbgAction::OnBnClickedToAsoirationPostion)
	ON_BN_CLICKED(IDC_BUTTON7, &CDbgAction::OnBnClickedToEmergencyPostion)
	ON_BN_CLICKED(IDC_BUTTON8, &CDbgAction::OnBnClickedAspiration)
	ON_BN_CLICKED(IDC_BUTTON9, &CDbgAction::OnBnClickedSampling)
	ON_BN_CLICKED(IDC_BUTTON10, &CDbgAction::OnBnClickedMicroscopeReset)
	ON_BN_CLICKED(IDC_BUTTON11, &CDbgAction::OnBnClickedXAxisReset)
	ON_BN_CLICKED(IDC_BUTTON12, &CDbgAction::OnBnClickedYAxisABChanReset)
	ON_BN_CLICKED(IDC_BUTTON13, &CDbgAction::OnBnClickedYAxisCDChanReset)
	ON_BN_CLICKED(IDC_BUTTON14, &CDbgAction::OnBnClickedZAxisReset)
	ON_BN_CLICKED(IDC_BUTTON15, &CDbgAction::OnBnClickedToM10Lens)
	ON_BN_CLICKED(IDC_BUTTON16, &CDbgAction::OnBnClickedToM40Lens)
	ON_BN_CLICKED(IDC_BUTTON17, &CDbgAction::OnBnClickedPipeReset)
	ON_BN_CLICKED(IDC_BUTTON18, &CDbgAction::OnBnClickedPipeInReset)
	ON_BN_CLICKED(IDC_BUTTON19, &CDbgAction::OnBnClickedPipeOutReset)
	ON_BN_CLICKED(IDC_BUTTON20, &CDbgAction::OnBnClickedStepLeftReset)
	ON_BN_CLICKED(IDC_BUTTON21, &CDbgAction::OnBnClickedStepRightReset)
	ON_BN_CLICKED(IDC_BUTTON22, &CDbgAction::OnBnClickedPipeIn)
	ON_BN_CLICKED(IDC_BUTTON23, &CDbgAction::OnBnClickedPipeOut)
	ON_BN_CLICKED(IDC_BUTTON24, &CDbgAction::OnBnClickedStep)
	ON_BN_CLICKED(IDC_BUTTON25, &CDbgAction::OnBnClickedSelectPaperReset)
	ON_BN_CLICKED(IDC_BUTTON26, &CDbgAction::OnBnClickedFeedPaperReset)
	ON_BN_CLICKED(IDC_BUTTON27, &CDbgAction::OnBnClickedSelectPaper)
	ON_BN_CLICKED(IDC_BUTTON28, &CDbgAction::OnBnClickedFeedPaperToSamplePos)
	ON_BN_CLICKED(IDC_BUTTON29, &CDbgAction::OnBnClickedFeedPaperStepOne)
	ON_BN_CLICKED(IDC_BUTTON30, &CDbgAction::OnBnClickedButtonClean)
	ON_BN_CLICKED(IDC_BUTTON31, &CDbgAction::OnBnClickedStrengClean)
	ON_BN_CLICKED(IDC_BUTTON32, &CDbgAction::OnBnClickedBaoyangClean)
	ON_BN_CLICKED(IDC_BUTTON33, &CDbgAction::OnBnClickedToPointSampleInitPos)
	ON_BN_CLICKED(IDC_BUTTON35, &CDbgAction::OnBnClickedTubeRotateReset)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON36, &CDbgAction::OnBnClickedButton36)
	ON_BN_CLICKED(IDC_BUTTON37, &CDbgAction::OnBnClickedButton37)
	ON_BN_CLICKED(IDC_BUTTON38, &CDbgAction::OnBnClickedButton38)
	ON_BN_CLICKED(IDC_BUTTON39, &CDbgAction::OnBnClickedButton39)
	ON_BN_CLICKED(IDC_BUTTON40, &CDbgAction::OnBnClickedButton40)
	ON_BN_CLICKED(IDC_BUTTON41, &CDbgAction::OnBnClickedButton41)
	ON_BN_CLICKED(IDC_BUTTON_GET_ZHUIGUANG, &CDbgAction::OnBnClickedButtonGetZhuiguang)
	ON_BN_CLICKED(IDC_BUTTON_GET_ZHUIGUANG2_CLEAR, &CDbgAction::OnBnClickedButtonGetZhuiguang2Clear)

	ON_MESSAGE(WM_PHYSICSSGSENSOR, &CDbgAction::RecvPhysicsSgSensorResultHandler)

	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON34, &CDbgAction::OnBnClickedButton34)
	ON_BN_CLICKED(IDC_BUTTON42, &CDbgAction::OnBnClickedButton42)
	ON_BN_CLICKED(IDC_BUTTON43, &CDbgAction::OnBnClickedButton43)
	ON_BN_CLICKED(IDC_BUTTON44, &CDbgAction::OnBnClickedButton44)
	ON_BN_CLICKED(IDC_BUTTON45, &CDbgAction::OnBnClickedButton45)
END_MESSAGE_MAP()


// CDbgAction 消息处理程序


BOOL CDbgAction::OnInitDialog()
{
 	CPropertyPage::OnInitDialog();
/*	ReChildWndsize();*/

	CMyDelegate1<CDbgAction*, void (CDbgAction::*)(void*, void*), void*> *f = new CMyDelegate1<CDbgAction*, void (CDbgAction::*)(void*, void*), void*>;
	f->RegisterEventHandler(this, &CDbgAction::OnRecvPhysicsSgSensorResult, 0);
	theApp.m_clsMsgDipatcher.AddEventHandler(PhysicsSgSensor, f);
	m_mapEventDelegate[PhysicsSgSensor] = f;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDbgAction::OnBnClickedSysReset()
{
	PerformCommonAction(UC_DBG_SYS_RESET_START);
}


void CDbgAction::OnBnClickedAspirationReset()
{
	PerformCommonAction(UC_DBG_ASPIRATION_DEVICE_RESET_START);
}


void CDbgAction::OnBnClickedSamplingTranslationReset()
{
	PerformCommonAction(UC_DBG_SAMPLE_TRASLATION_RESET_START);
}


void CDbgAction::OnBnClickedSamplingUpdownReset()
{
	PerformCommonAction(UC_DBG_SAMPLE_UPDOWN_RESET_START);
}


void CDbgAction::OnBnClickedToCleanPostion()
{
	PerformCommonAction(UC_DBG_GOTO_CLEANPOSITION_START);
}


void CDbgAction::OnBnClickedToAsoirationPostion()
{
	PerformCommonAction(UC_DBG_GOTO_ASPIRATIONPOSITION_START);
}


void CDbgAction::OnBnClickedToEmergencyPostion()
{
	PerformCommonAction(UC_DBG_GOTO_EMERGENCYPOSITION_START);
}


void CDbgAction::OnBnClickedAspiration()
{
	PerformCommonAction(UC_DBG_ASPIRATION_START);
}


void CDbgAction::OnBnClickedSampling()
{
	PerformCommonAction(UC_DBG_POINTSAMPLE_START);
}


void CDbgAction::OnBnClickedMicroscopeReset()
{
	PerformCommonAction(UC_DBG_MIROSCOPE_RESET_START);
}


void CDbgAction::OnBnClickedXAxisReset()
{
	PerformCommonAction(UC_DBG_X_AXIS_RESET_START); 
}

void CDbgAction::OnBnClickedYAxisABChanReset()
{
	PerformCommonAction(UC_DBG_Y_AXIS_AB_CHANNEL_RESET_START);
}


void CDbgAction::OnBnClickedYAxisCDChanReset()
{
	PerformCommonAction(UC_DBG_Y_AXIS_CD_CHANNEL_RESET_START);
}


void CDbgAction::OnBnClickedZAxisReset()
{
	PerformCommonAction(UC_DBG_Z_AXIS_RESET_START);
}


void CDbgAction::OnBnClickedToM10Lens()
{
	PerformCommonAction(UC_DBG_GOTO_10X_LENS_START);
}


void CDbgAction::OnBnClickedToM40Lens()
{
/*	PerformCommonAction(UC_DBG_GOTO_40X_LENS_START);*/
}


void CDbgAction::OnBnClickedPipeReset()
{
	PerformCommonAction(UC_DBG_PIPE_RESET_START);
}


void CDbgAction::OnBnClickedPipeInReset()
{
	PerformCommonAction(UC_DBG_PIPE_IN_RESET_START);
}


void CDbgAction::OnBnClickedPipeOutReset()
{
	PerformCommonAction(UC_DBG_PIPE_OUT_RESET_START);
}


void CDbgAction::OnBnClickedStepLeftReset()
{
	PerformCommonAction(UC_DBG_STEP_LEFT_RESET_START);
}


void CDbgAction::OnBnClickedStepRightReset()
{
	PerformCommonAction(UC_DBG_STEP_RIGHT_RESET_START);
}


void CDbgAction::OnBnClickedPipeIn()
{
	PerformCommonAction(UC_DBG_PIPE_IN_START);
}


void CDbgAction::OnBnClickedPipeOut()
{
	PerformCommonAction(UC_DBG_PIPE_OUT_START);
}


void CDbgAction::OnBnClickedStep()
{
	PerformCommonAction(UC_DBG_ONE_STEP_START);
}


void CDbgAction::OnBnClickedSelectPaperReset()
{
	PerformCommonAction(UC_DBG_SELECT_PAPER_RESET_START);
}


void CDbgAction::OnBnClickedFeedPaperReset()
{
	PerformCommonAction(UC_DBG_FEED_PAPER_RESET_START);
}


void CDbgAction::OnBnClickedSelectPaper()
{
	PerformCommonAction(UC_DBG_SELECT_PAPER_START);
}


void CDbgAction::OnBnClickedFeedPaperToSamplePos()
{
	PerformCommonAction(UC_DBG_FEED_PAPER_TOPOINT_START);
}


void CDbgAction::OnBnClickedFeedPaperStepOne()
{
	PerformCommonAction(UC_DBG_FEED_PAPER_ONESTEP_START);
}


void CDbgAction::OnBnClickedButtonClean()
{
	PerformCommonAction(UC_DBG_CLEAN);
}


void CDbgAction::OnBnClickedStrengClean()
{
	PerformCommonAction(UC_DBG_STRENGCLEAN);
}


void CDbgAction::OnBnClickedBaoyangClean()
{
	PerformCommonAction(UC_DBG_BAOYANGCLEAN);
}


void CDbgAction::OnBnClickedToPointSampleInitPos()
{
	PerformCommonAction(UC_DBG_TOPOINTSAMPLE_INITPOS);
}


void CDbgAction::OnBnClickedTubeRotateReset()
{
	PerformCommonAction(UC_DBG_TUBEROTATE_RESET);
}


void CDbgAction::OnShowWindow(BOOL bShow, UINT nStatus)
{

	CPropertyPage::OnShowWindow(bShow, nStatus);

// 	CRect rect;
// 	GetParent()->GetWindowRect(&rect);
// 	int wndWidth = GetSystemMetrics(SM_CXSCREEN);  //屏幕宽度    
// 	int wndHeight = GetSystemMetrics(SM_CYSCREEN);
// 
// 	GetParent()->GetWindowRect(&rect);
// 	int nWidth = rect.right - rect.left;
// 	int nHeight = rect.bottom - rect.top;
// 	if (bShow)
// 	{
// 		GetParent()->GetParent()->SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
// 	}
	// TODO:  在此处添加消息处理程序代码
}



void CDbgAction::ReChildWndsize()
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


void CDbgAction::OnBnClickedButton36()
{
	// TODO:  在此添加控件通知处理程序代码
	if (::MessageBox(NULL, L"请放入背面朝上的新纸条到选纸机构，完成后点击确定", L"提示", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
	{
		PerformCommonAction(UC_DBG_SELECT_PAPER_BALANCE_START);
	}
	else
	{

	}

}


void CDbgAction::OnBnClickedButton37()
{
	// TODO:  在此添加控件通知处理程序代码
	PerformCommonAction(UC_DBG_PUT_PAPER_BALANCE_START);
}


void CDbgAction::OnBnClickedButton38()
{
	// TODO:  在此添加控件通知处理程序代码
	PerformCommonAction(UC_DBG_EMPTY_PIPE_START);
}


void CDbgAction::OnBnClickedButton39()
{
	GetDlgItem(IDC_EDIT1)->SetWindowText(L"");
	// TODO:  在此添加控件通知处理程序代码
	BARCODE_REQ_CONTEXT_DBG ctx = { 0 };
	ReadBarCode(ctx);

	int num = MultiByteToWideChar(0, 0, ctx.Barcode, -1, NULL, 0);
	wchar_t *wide = new wchar_t[num];
	MultiByteToWideChar(0, 0, ctx.Barcode, -1, wide, num);

	GetDlgItem(IDC_EDIT1)->SetWindowText(wide);
}

void CDbgAction::OnBnClickedButton40()
{
	// TODO:  在此添加控件通知处理程序代码
	PerformCommonAction(UC_DBG_FEED_PAPER_FIRST_HALF_STEP_START);
}


void CDbgAction::OnBnClickedButton41()
{
	// TODO:  在此添加控件通知处理程序代码
	PerformCommonAction(UC_DBG_FEED_PAPER_SECOND_HALF_STEP_START);
}


void CDbgAction::OnBnClickedButtonGetZhuiguang()
{
	unsigned char  hasGain;             // 有无增益
	unsigned short nHasZhuiGuangVal;    // 有无追光值
	unsigned char  blackGain;           // 黑块增益
	unsigned short nBlackZhuiGuangVal;  // 黑块追光值
	unsigned short nHasCurrentVal;      // 有无当前值
	unsigned short nBlackCurrentVal;    // 黑块当前值
	unsigned char  nZhuiGuangStep;
	DBG_GET_ZHUIGUANG_VAL_CONTEXT ctx = { 0 };
	if (GetZhuiGuangVal(ctx))
	{
		CString str;
		str.Format(_T("%d"), ctx.hasGain);            SetDlgItemText(IDC_EDIT_ZHUIGUANG_1, str);
		str.Format(_T("%d"), ctx.nHasZhuiGuangVal);   SetDlgItemText(IDC_EDIT_ZHUIGUANG_2, str);
		str.Format(_T("%d"), ctx.blackGain);          SetDlgItemText(IDC_EDIT_ZHUIGUANG_3, str);
		str.Format(_T("%d"), ctx.nBlackZhuiGuangVal); SetDlgItemText(IDC_EDIT_ZHUIGUANG_4, str);
		str.Format(_T("%d"), ctx.nHasCurrentVal);     SetDlgItemText(IDC_EDIT_ZHUIGUANG_5, str);
		str.Format(_T("%d"), ctx.nBlackCurrentVal);   SetDlgItemText(IDC_EDIT_ZHUIGUANG_6, str);
		str.Format(_T("%d"), ctx.nZhuiGuangStep);     SetDlgItemText(IDC_EDIT_ZHUIGUANG_7, str);
	}
	else
	{
		AfxMessageBox(_T("获取失败"), MB_OK|MB_ICONWARNING);
	}
}


void CDbgAction::OnBnClickedButtonGetZhuiguang2Clear()
{
	SetDlgItemText(IDC_EDIT_ZHUIGUANG_1, _T(""));
	SetDlgItemText(IDC_EDIT_ZHUIGUANG_2, _T(""));
	SetDlgItemText(IDC_EDIT_ZHUIGUANG_3, _T(""));
	SetDlgItemText(IDC_EDIT_ZHUIGUANG_4, _T(""));
	SetDlgItemText(IDC_EDIT_ZHUIGUANG_5, _T(""));
	SetDlgItemText(IDC_EDIT_ZHUIGUANG_6, _T(""));
	SetDlgItemText(IDC_EDIT_ZHUIGUANG_7, _T(""));
}

void CDbgAction::OnDestroy()
{
	CPropertyPage::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
	std::for_each(m_mapEventDelegate.begin(), m_mapEventDelegate.end(), [](std::map<int, void*>::reference r){
		theApp.m_clsMsgDipatcher.RemoveEventHandler(r.first, (CMyDelegate*)r.second);
	});
}

void CDbgAction::bEnableSGBtn(bool bEnableBtn)
{
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON34), bEnableBtn);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON42), bEnableBtn);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON43), bEnableBtn);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUTTON45), bEnableBtn);
}

void CDbgAction::OnBnClickedButton34()
{
	// TODO:  在此添加控件通知处理程序代码
	if (AfxMessageBox(_T("请放入低比重液进行校准")) == IDOK)
	{
		CString str;
		int nSgType = 0;
		SG_CALIBRATE_CONTEXT SgCalibrateResult = { 0 };

		bEnableSGBtn(false);
		PhysicsSgCalibrate(nSgType, SgCalibrateResult);
		str.Format(_T("低比重液校准结果：%d\r\nA端AD值%d\r\nB端AD值%d"), SgCalibrateResult.nValue, SgCalibrateResult.nAD_a, SgCalibrateResult.nAD_b);
		m_static_sg_result.SetWindowText(str);
		bEnableSGBtn(true);
	}
}


void CDbgAction::OnBnClickedButton42()
{
	// TODO:  在此添加控件通知处理程序代码
	if (AfxMessageBox(_T("请放入高比重液进行校准")) == IDOK)
	{
		CString str;
		int nSgType = 1;
		SG_CALIBRATE_CONTEXT SgCalibrateResult = { 0 };

		bEnableSGBtn(false);
		PhysicsSgCalibrate(nSgType, SgCalibrateResult);
		str.Format(_T("高比重液校准结果：%d\r\nA端AD值%d\r\nB端AD值%d"), SgCalibrateResult.nValue, SgCalibrateResult.nAD_a, SgCalibrateResult.nAD_b);
		m_static_sg_result.SetWindowText(str);
		bEnableSGBtn(true);
	}
}


void CDbgAction::OnBnClickedButton43()
{
	// TODO:  在此添加控件通知处理程序代码
	bEnableSGBtn(false);
	PhysicsSgSensorTest(1);
}


void CDbgAction::OnBnClickedButton44()
{
	// TODO:  在此添加控件通知处理程序代码
	PhysicsSgSensorTest(0);
	bEnableSGBtn(true);
}


void CDbgAction::OnRecvPhysicsSgSensorResult(void *arg, void *cbarg)
{
	SendMessage(WM_PHYSICSSGSENSOR, (WPARAM)arg);
}

LRESULT  CDbgAction::RecvPhysicsSgSensorResultHandler(WPARAM w, LPARAM l)
{
	CString str;

	if (w)
	{
		PSG_SENSOR_CONTEXT ctx = (PSG_SENSOR_CONTEXT)w;
		str.Format(_T("测试结果：比重 %d\r\n  比重原始值a %d，比重原始值b %d\r\n  比重差值b %d"), ctx->sg_test, ctx->sg_original_a, ctx->sg_original_b, ctx->nDvalue);
		m_static_sg_result.SetWindowText(str);
	}

	return 0;
}


void CDbgAction::OnBnClickedButton45()
{
	// TODO:  在此添加控件通知处理程序代码
	CString str;
	PHYSICS_SG_DBG_RESULT_CONTEXT SgTestResult = { 0 };

	bEnableSGBtn(false);
	PhysicsSgTest(SgTestResult);
	str.Format(_T("比重测试结果：%d\r\nA端AD值：%d\r\nB端AD值：%d"), SgTestResult.sg_test, SgTestResult.sg_original_a, SgTestResult.sg_original_b);
	m_static_sg_result.SetWindowText(str);
	bEnableSGBtn(true);
}
