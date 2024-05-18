// DbgHardwareparam.cpp : 实现文件
//

#include "stdafx.h"
#include "DbgHardwareparam.h"
#include "afxdialogex.h"




// CDbgHardwareparam 对话框

IMPLEMENT_DYNAMIC(CDbgHardwareparam, CPropertyPage)

CDbgHardwareparam::CDbgHardwareparam()
	: CPropertyPage(CDbgHardwareparam::IDD)
{

}

CDbgHardwareparam::~CDbgHardwareparam()
{
}

void CDbgHardwareparam::initParamCtrl()
{
	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(0);

	SetDlgItemText(IDC_EDIT5, _T(""));
	SetDlgItemText(IDC_EDIT6, _T(""));
	SetDlgItemText(IDC_EDIT7, _T(""));
	SetDlgItemText(IDC_EDIT8, _T(""));
	SetDlgItemText(IDC_EDIT9, _T(""));
	SetDlgItemText(IDC_EDIT10, _T(""));
	SetDlgItemText(IDC_EDIT11, _T(""));
	SetDlgItemText(IDC_EDIT12, _T(""));
	SetDlgItemText(IDC_EDIT13, _T(""));
	SetDlgItemText(IDC_EDIT14, _T(""));
	SetDlgItemText(IDC_EDIT15, _T(""));
	SetDlgItemText(IDC_EDIT16, _T(""));
	SetDlgItemText(IDC_EDIT17, _T(""));
	SetDlgItemText(IDC_EDIT18, _T(""));
	SetDlgItemText(IDC_EDIT19, _T(""));
	SetDlgItemText(IDC_EDIT20, _T(""));
	SetDlgItemText(IDC_EDIT21, _T(""));
	SetDlgItemText(IDC_EDIT22, _T(""));
	SetDlgItemText(IDC_EDIT23, _T(""));
	SetDlgItemText(IDC_EDIT24, _T(""));
	SetDlgItemText(IDC_EDIT25, _T(""));
	SetDlgItemText(IDC_EDIT26, _T(""));
	SetDlgItemText(IDC_EDIT27, _T(""));
	SetDlgItemText(IDC_EDIT28, _T(""));
	SetDlgItemText(IDC_EDIT29, _T(""));
	SetDlgItemText(IDC_EDIT30, _T(""));
	SetDlgItemText(IDC_EDIT31, _T(""));
	SetDlgItemText(IDC_EDIT32, _T(""));
	SetDlgItemText(IDC_EDIT33, _T(""));
	SetDlgItemText(IDC_EDIT34, _T(""));
	SetDlgItemText(IDC_EDIT35, _T(""));
	SetDlgItemText(IDC_EDIT36, _T(""));
	SetDlgItemText(IDC_EDIT37, _T(""));
	SetDlgItemText(IDC_EDIT38, _T(""));
	SetDlgItemText(IDC_EDIT39, _T(""));
	SetDlgItemText(IDC_EDIT40, _T(""));
	SetDlgItemText(IDC_EDIT41, _T(""));
}

void CDbgHardwareparam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_readParamBtn);
	DDX_Control(pDX, IDC_BUTTON14, m_writeParamBtn);
	DDX_Control(pDX, IDC_BUTTON13, m_resetParamBtn);
	DDX_Control(pDX, IDC_BUTTON15, m_saveDefaultBtn);
	DDX_Control(pDX, IDC_BUTTON31, m_readDefaultBtn);
}


BEGIN_MESSAGE_MAP(CDbgHardwareparam, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON13, &CDbgHardwareparam::OnBnClickedResetHardwareParam)
	ON_BN_CLICKED(IDC_BUTTON1, &CDbgHardwareparam::OnBnClickedReadHardwareParam)
	ON_BN_CLICKED(IDC_BUTTON14, &CDbgHardwareparam::OnBnClickedWriteHardwareParam)
	ON_BN_CLICKED(IDC_BUTTON15, &CDbgHardwareparam::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON31, &CDbgHardwareparam::OnBnClickedButton31)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON2, &CDbgHardwareparam::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDbgHardwareparam 消息处理程序


BOOL CDbgHardwareparam::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	initParamCtrl();

	GetHardwareVersion();

#if (_LC_V <= 2150)
	GetDlgItem(IDC_EDIT49)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_49)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT100)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_100)->ShowWindow(SW_HIDE);
#elif (_LC_V == 2180)
	GetDlgItem(IDC_EDIT49)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_49)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_EDIT100)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_100)->ShowWindow(SW_HIDE);
#elif (_LC_V >= 2190 && _LC_V < 2230)
	GetDlgItem(IDC_EDIT40)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_40)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT100)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_100)->ShowWindow(SW_HIDE);
	
#elif (_LC_V >= 2230)
	GetDlgItem(IDC_EDIT40)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_40)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDC_STATIC_100, _T("排管推进步数"));
	SetDlgItemText(IDC_STATIC_99, _T("推进到位确认步数"));

#elif (_LC_V >= 2260)
	GetDlgItem(IDC_EDIT40)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_40)->ShowWindow(SW_HIDE);

#else
	GetDlgItem(IDC_EDIT49)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_49)->ShowWindow(SW_HIDE);

#endif

#if (_LC_V >= 2230)

	GetDlgItem(IDC_STATIC_MIRCVERSION)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_PIPEVERSION)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_MIRCVERSION)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_PIPEVERSION)->ShowWindow(SW_SHOW);
#else

	GetDlgItem(IDC_STATIC_MIRCVERSION)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_PIPEVERSION)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_MIRCVERSION)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PIPEVERSION)->ShowWindow(SW_HIDE);
#endif

	// 权限控制，普通用户只允许读取下位机参数
	if (theApp.loginGroup != 111 && theApp.loginGroup != 4)
	{
		m_writeParamBtn.EnableWindow(FALSE);
		m_resetParamBtn.EnableWindow(FALSE);
		m_saveDefaultBtn.EnableWindow(FALSE);
		m_readDefaultBtn.EnableWindow(FALSE);
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDbgHardwareparam::GetHardwareVersion()
{
	HARDWARE_READ_VERSION_CONTEXT ctx = { 0 };
	if (ReadHardwareVersionInfo(ctx))
	{
		CString str;
		char *p = (char*)ctx.content;
		str = p;
		SetDlgItemText(IDC_EDIT1, str);
		int count = 0;
#if (_LC_V >= 2230)
		int nTotalVersion = 7;
#else
		int nTotalVersion = 4;
#endif
		bool flag = false;
		for (int i = 0; i < sizeof(ctx.content) - 1; i++)
		{
			if (count > nTotalVersion - 1)
			{
				break;
			}
			if (flag)
			{
				str = p + i;
				if (count == 1)
				{
					SetDlgItemText(IDC_EDIT2, str);
				}
				else if (count == 2)
				{
					SetDlgItemText(IDC_EDIT3, str);
				}
				else if (count == 3)
				{
					SetDlgItemText(IDC_EDIT4, str);
				}
				else if (count == 4)
				{
					SetDlgItemText(IDC_EDIT_MIRCVERSION, str);
				}
				else if (count == 5)
				{
					SetDlgItemText(IDC_EDIT_PIPEVERSION, str);
				}
				else if (count == 6)
				{
					SetDlgItemText(IDC_EDIT_PHYSICSVERSION, str);
				}
				else
				{
					break;
				}
				flag = false;
			}
			if (p[i] == 0)
			{
				flag = true;
				count++;
			}
		}
	}
	else
	{
		MessageBox(_T("获取版本信息失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
}

void CDbgHardwareparam::OnBnClickedResetHardwareParam()
{
	if (MessageBox(_T("恢复硬件初始化参数？"), _T("提示"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		bool ret = ResetHardwareParam();
		MessageBox((ret ? _T("恢复硬件初始化参数成功!") : _T("恢复硬件初始化参数失败!")), _T("提示"), MB_OK | (ret ? MB_ICONINFORMATION : MB_ICONWARNING));
		if (ret)
		{
			OnBnClickedReadHardwareParam();
		}
	}
}


void CDbgHardwareparam::OnBnClickedReadHardwareParam()
{
#if (_LC_V <= 2150)
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	if (!ReadHardwareParam(cfg))
	{
		MessageBox(_T("读取硬件参数失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		CString str;
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(cfg.TestType-1);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.UdcCheckItemCount-10);
		str.Format(_T("%d"), cfg.FeedPaperStepToSamplingPostion);
		SetDlgItemText(IDC_EDIT5, str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		SetDlgItemText(IDC_EDIT6, str);
		str.Format(_T("%d"), cfg.MoveToCleanPostionStep);
		SetDlgItemText(IDC_EDIT7, str);
		str.Format(_T("%d"), cfg.MoteToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT8, str);
		str.Format(_T("%d"), cfg.MoveToEmergencyOPostionSte);
		SetDlgItemText(IDC_EDIT9, str);
		str.Format(_T("%d"), cfg.DownToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), cfg.DownToSamplingPostionStep);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), cfg.XAxisCorrectStep);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), cfg.YAxisCorrectStep);
		SetDlgItemText(IDC_EDIT13, str);
		str.Format(_T("%d"), cfg.AspriationVolume);
		SetDlgItemText(IDC_EDIT14, str);
		str.Format(_T("%d"), cfg.CleanCountPoolVolume);
		SetDlgItemText(IDC_EDIT15, str);
		str.Format(_T("%d"), cfg.CleanPipeVolume);
		SetDlgItemText(IDC_EDIT16, str);
		str.Format(_T("%d"), cfg.ColorBlock1SamplingVolume);
		SetDlgItemText(IDC_EDIT17, str);
		str.Format(_T("%d"), cfg.ColorBlock2SamplingVolume);
		SetDlgItemText(IDC_EDIT18, str);
		str.Format(_T("%d"), cfg.ColorBlock3SamplingVolume);
		SetDlgItemText(IDC_EDIT19, str);
		str.Format(_T("%d"), cfg.ColorBlock4SamplingVolume);
		SetDlgItemText(IDC_EDIT20, str);
		str.Format(_T("%d"), cfg.ColorBlock5SamplingVolume);
		SetDlgItemText(IDC_EDIT21, str);
		str.Format(_T("%d"), cfg.ColorBlock6SamplingVolume);
		SetDlgItemText(IDC_EDIT22, str);
		str.Format(_T("%d"), cfg.ColorBlock7SamplingVolume);
		SetDlgItemText(IDC_EDIT23, str);
		str.Format(_T("%d"), cfg.ColorBlock8SamplingVolume);
		SetDlgItemText(IDC_EDIT24, str);
		str.Format(_T("%d"), cfg.ColorBlock9SamplingVolume);
		SetDlgItemText(IDC_EDIT25, str);
		str.Format(_T("%d"), cfg.ColorBlock10SamplingVolume);
		SetDlgItemText(IDC_EDIT26, str);
		str.Format(_T("%d"), cfg.ColorBlock11SamplingVolume);
		SetDlgItemText(IDC_EDIT27, str);
		str.Format(_T("%d"), cfg.ColorBlock12SamplingVolume);
		SetDlgItemText(IDC_EDIT28, str);
		str.Format(_T("%d"), cfg.ColorBlock13SamplingVolume);
		SetDlgItemText(IDC_EDIT29, str);
		str.Format(_T("%d"), cfg.ColorBlock14SamplingVolume);
		SetDlgItemText(IDC_EDIT30, str);
		str.Format(_T("%d"), cfg.MixWellCount);
		SetDlgItemText(IDC_EDIT31, str);
		str.Format(_T("%d"), cfg.CountPoolStandStillTime);
		SetDlgItemText(IDC_EDIT32, str);
		str.Format(_T("%d"), cfg.M40XLensXAxisViewStep);
		SetDlgItemText(IDC_EDIT33, str);
		str.Format(_T("%d"), cfg.M40XLensYAxisViewStep);
		SetDlgItemText(IDC_EDIT34, str);
		str.Format(_T("%d"), cfg.XAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT35, str);
		str.Format(_T("%d"), cfg.YAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT36, str);
		str.Format(_T("%d"), cfg.CountPoolChannelSpaceStep);
		SetDlgItemText(IDC_EDIT37, str);
		str.Format(_T("%d"), cfg.PaperColorBlockSpaceStep);
		SetDlgItemText(IDC_EDIT38, str);
		str.Format(_T("%d"), cfg.PipeInStep);
		SetDlgItemText(IDC_EDIT39, str);
		str.Format(_T("%d"), cfg.PipeOutStep);
		SetDlgItemText(IDC_EDIT40, str);
		str.Format(_T("%d"), cfg.To1HoleStep);
		SetDlgItemText(IDC_EDIT41, str);
	}
#elif (_LC_V == 2180)
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	if (!ReadHardwareParam(cfg))
	{
		MessageBox(_T("读取硬件参数失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		CString str;
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(cfg.TestType - 1);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.UdcCheckItemCount - 10);
		str.Format(_T("%d"), cfg.FeedPaperStepToSamplingPostion);
		SetDlgItemText(IDC_EDIT5, str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		SetDlgItemText(IDC_EDIT6, str);
		str.Format(_T("%d"), cfg.MoveToCleanPostionStep);
		SetDlgItemText(IDC_EDIT7, str);
		str.Format(_T("%d"), cfg.MoteToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT8, str);
		str.Format(_T("%d"), cfg.MoveToEmergencyOPostionSte);
		SetDlgItemText(IDC_EDIT9, str);
		str.Format(_T("%d"), cfg.DownToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), cfg.DownToSamplingPostionStep);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), cfg.XAxisCorrectStep);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), cfg.YAxisCorrectStep);
		SetDlgItemText(IDC_EDIT13, str);
		str.Format(_T("%d"), cfg.AspriationVolume);
		SetDlgItemText(IDC_EDIT14, str);
		str.Format(_T("%d"), cfg.CleanCountPoolVolume);
		SetDlgItemText(IDC_EDIT15, str);
		str.Format(_T("%d"), cfg.CleanPipeVolume);
		SetDlgItemText(IDC_EDIT16, str);
		str.Format(_T("%d"), cfg.ColorBlock1SamplingVolume);
		SetDlgItemText(IDC_EDIT17, str);
		str.Format(_T("%d"), cfg.ColorBlock2SamplingVolume);
		SetDlgItemText(IDC_EDIT18, str);
		str.Format(_T("%d"), cfg.ColorBlock3SamplingVolume);
		SetDlgItemText(IDC_EDIT19, str);
		str.Format(_T("%d"), cfg.ColorBlock4SamplingVolume);
		SetDlgItemText(IDC_EDIT20, str);
		str.Format(_T("%d"), cfg.ColorBlock5SamplingVolume);
		SetDlgItemText(IDC_EDIT21, str);
		str.Format(_T("%d"), cfg.ColorBlock6SamplingVolume);
		SetDlgItemText(IDC_EDIT22, str);
		str.Format(_T("%d"), cfg.ColorBlock7SamplingVolume);
		SetDlgItemText(IDC_EDIT23, str);
		str.Format(_T("%d"), cfg.ColorBlock8SamplingVolume);
		SetDlgItemText(IDC_EDIT24, str);
		str.Format(_T("%d"), cfg.ColorBlock9SamplingVolume);
		SetDlgItemText(IDC_EDIT25, str);
		str.Format(_T("%d"), cfg.ColorBlock10SamplingVolume);
		SetDlgItemText(IDC_EDIT26, str);
		str.Format(_T("%d"), cfg.ColorBlock11SamplingVolume);
		SetDlgItemText(IDC_EDIT27, str);
		str.Format(_T("%d"), cfg.ColorBlock12SamplingVolume);
		SetDlgItemText(IDC_EDIT28, str);
		str.Format(_T("%d"), cfg.ColorBlock13SamplingVolume);
		SetDlgItemText(IDC_EDIT29, str);
		str.Format(_T("%d"), cfg.ColorBlock14SamplingVolume);
		SetDlgItemText(IDC_EDIT30, str);
		str.Format(_T("%d"), cfg.MixWellCount);
		SetDlgItemText(IDC_EDIT31, str);
		str.Format(_T("%d"), cfg.CountPoolStandStillTime);
		SetDlgItemText(IDC_EDIT32, str);
		str.Format(_T("%d"), cfg.M40XLensXAxisViewStep);
		SetDlgItemText(IDC_EDIT33, str);
		str.Format(_T("%d"), cfg.M40XLensYAxisViewStep);
		SetDlgItemText(IDC_EDIT34, str);
		str.Format(_T("%d"), cfg.XAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT35, str);
		str.Format(_T("%d"), cfg.YAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT36, str);
		str.Format(_T("%d"), cfg.CountPoolChannelSpaceStep);
		SetDlgItemText(IDC_EDIT37, str);
		str.Format(_T("%d"), cfg.PaperColorBlockSpaceStep);
		SetDlgItemText(IDC_EDIT38, str);
		str.Format(_T("%d"), cfg.PipeInStep);
		SetDlgItemText(IDC_EDIT39, str);
		str.Format(_T("%d"), cfg.PipeOutStep);
		SetDlgItemText(IDC_EDIT40, str);
		str.Format(_T("%d"), cfg.To1HoleStep);
		SetDlgItemText(IDC_EDIT41, str);
	}
#elif ( _LC_V >= 2190 && _LC_V < 2230)
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	if (!ReadHardwareParam(cfg))
	{
		MessageBox(_T("读取硬件参数失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		CString str;
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(cfg.TestType - 1);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.UdcCheckItemCount - 10);
		str.Format(_T("%d"), cfg.FeedPaperStepToSamplingPostion);
		SetDlgItemText(IDC_EDIT5, str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		SetDlgItemText(IDC_EDIT6, str);
		str.Format(_T("%d"), cfg.MoveToCleanPostionStep);
		SetDlgItemText(IDC_EDIT7, str);
		str.Format(_T("%d"), cfg.MoteToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT8, str);
		str.Format(_T("%d"), cfg.MoveToEmergencyOPostionSte);
		SetDlgItemText(IDC_EDIT9, str);
		str.Format(_T("%d"), cfg.DownToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), cfg.DownToSamplingPostionStep);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), cfg.XAxisCorrectStep);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), cfg.YAxisCorrectStep);
		SetDlgItemText(IDC_EDIT13, str);
		str.Format(_T("%d"), cfg.AspriationVolume);
		SetDlgItemText(IDC_EDIT14, str);
		str.Format(_T("%d"), cfg.CleanCountPoolVolume);
		SetDlgItemText(IDC_EDIT15, str);
		str.Format(_T("%d"), cfg.CleanPipeVolume);
		SetDlgItemText(IDC_EDIT16, str);
		str.Format(_T("%d"), cfg.ColorBlock1SamplingVolume);
		SetDlgItemText(IDC_EDIT17, str);
		str.Format(_T("%d"), cfg.ColorBlock2SamplingVolume);
		SetDlgItemText(IDC_EDIT18, str);
		str.Format(_T("%d"), cfg.ColorBlock3SamplingVolume);
		SetDlgItemText(IDC_EDIT19, str);
		str.Format(_T("%d"), cfg.ColorBlock4SamplingVolume);
		SetDlgItemText(IDC_EDIT20, str);
		str.Format(_T("%d"), cfg.ColorBlock5SamplingVolume);
		SetDlgItemText(IDC_EDIT21, str);
		str.Format(_T("%d"), cfg.ColorBlock6SamplingVolume);
		SetDlgItemText(IDC_EDIT22, str);
		str.Format(_T("%d"), cfg.ColorBlock7SamplingVolume);
		SetDlgItemText(IDC_EDIT23, str);
		str.Format(_T("%d"), cfg.ColorBlock8SamplingVolume);
		SetDlgItemText(IDC_EDIT24, str);
		str.Format(_T("%d"), cfg.ColorBlock9SamplingVolume);
		SetDlgItemText(IDC_EDIT25, str);
		str.Format(_T("%d"), cfg.ColorBlock10SamplingVolume);
		SetDlgItemText(IDC_EDIT26, str);
		str.Format(_T("%d"), cfg.ColorBlock11SamplingVolume);
		SetDlgItemText(IDC_EDIT27, str);
		str.Format(_T("%d"), cfg.ColorBlock12SamplingVolume);
		SetDlgItemText(IDC_EDIT28, str);
		str.Format(_T("%d"), cfg.ColorBlock13SamplingVolume);
		SetDlgItemText(IDC_EDIT29, str);
		str.Format(_T("%d"), cfg.ColorBlock14SamplingVolume);
		SetDlgItemText(IDC_EDIT30, str);
		str.Format(_T("%d"), cfg.MixWellCount);
		SetDlgItemText(IDC_EDIT31, str);
		str.Format(_T("%d"), cfg.CountPoolStandStillTime);
		SetDlgItemText(IDC_EDIT32, str);
		str.Format(_T("%d"), cfg.M40XLensXAxisViewStep);
		SetDlgItemText(IDC_EDIT33, str);
		str.Format(_T("%d"), cfg.M40XLensYAxisViewStep);
		SetDlgItemText(IDC_EDIT34, str);
		str.Format(_T("%d"), cfg.XAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT35, str);
		str.Format(_T("%d"), cfg.YAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT36, str);
		str.Format(_T("%d"), cfg.CountPoolChannelSpaceStep);
		SetDlgItemText(IDC_EDIT37, str);
		str.Format(_T("%d"), cfg.PaperColorBlockSpaceStep);
		SetDlgItemText(IDC_EDIT38, str);
		str.Format(_T("%d"), cfg.PipeInStep);
		SetDlgItemText(IDC_EDIT39, str);
		str.Format(_T("%d"), cfg.To1HoleStep);
		SetDlgItemText(IDC_EDIT41, str);
		str.Format(_T("%d"), cfg.AspriationPoolVolume);
		SetDlgItemText(IDC_EDIT49, str);
	}
#elif (_LC_V >= 2230)
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	if (!ReadHardwareParam(cfg))
	{
		MessageBox(_T("读取硬件参数失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		CString str;
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(cfg.TestType - 1);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.UdcCheckItemCount - 10);
		str.Format(_T("%d"), cfg.FeedPaperStepToSamplingPostion);
		SetDlgItemText(IDC_EDIT5, str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		SetDlgItemText(IDC_EDIT6, str);
		str.Format(_T("%d"), cfg.MoveToCleanPostionStep);
		SetDlgItemText(IDC_EDIT7, str);
		str.Format(_T("%d"), cfg.MoteToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT8, str);
		str.Format(_T("%d"), cfg.MoveToEmergencyOPostionSte);
		SetDlgItemText(IDC_EDIT9, str);
		str.Format(_T("%d"), cfg.DownToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), cfg.DownToSamplingPostionStep);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), cfg.XAxisCorrectStep);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), cfg.YAxisCorrectStep);
		SetDlgItemText(IDC_EDIT13, str);
		str.Format(_T("%d"), cfg.AspriationVolume);
		SetDlgItemText(IDC_EDIT14, str);
		str.Format(_T("%d"), cfg.CleanCountPoolVolume);
		SetDlgItemText(IDC_EDIT15, str);
		str.Format(_T("%d"), cfg.CleanPipeVolume);
		SetDlgItemText(IDC_EDIT16, str);
		str.Format(_T("%d"), cfg.ColorBlock1SamplingVolume);
		SetDlgItemText(IDC_EDIT17, str);
		str.Format(_T("%d"), cfg.ColorBlock2SamplingVolume);
		SetDlgItemText(IDC_EDIT18, str);
		str.Format(_T("%d"), cfg.ColorBlock3SamplingVolume);
		SetDlgItemText(IDC_EDIT19, str);
		str.Format(_T("%d"), cfg.ColorBlock4SamplingVolume);
		SetDlgItemText(IDC_EDIT20, str);
		str.Format(_T("%d"), cfg.ColorBlock5SamplingVolume);
		SetDlgItemText(IDC_EDIT21, str);
		str.Format(_T("%d"), cfg.ColorBlock6SamplingVolume);
		SetDlgItemText(IDC_EDIT22, str);
		str.Format(_T("%d"), cfg.ColorBlock7SamplingVolume);
		SetDlgItemText(IDC_EDIT23, str);
		str.Format(_T("%d"), cfg.ColorBlock8SamplingVolume);
		SetDlgItemText(IDC_EDIT24, str);
		str.Format(_T("%d"), cfg.ColorBlock9SamplingVolume);
		SetDlgItemText(IDC_EDIT25, str);
		str.Format(_T("%d"), cfg.ColorBlock10SamplingVolume);
		SetDlgItemText(IDC_EDIT26, str);
		str.Format(_T("%d"), cfg.ColorBlock11SamplingVolume);
		SetDlgItemText(IDC_EDIT27, str);
		str.Format(_T("%d"), cfg.ColorBlock12SamplingVolume);
		SetDlgItemText(IDC_EDIT28, str);
		str.Format(_T("%d"), cfg.ColorBlock13SamplingVolume);
		SetDlgItemText(IDC_EDIT29, str);
		str.Format(_T("%d"), cfg.ColorBlock14SamplingVolume);
		SetDlgItemText(IDC_EDIT30, str);
		str.Format(_T("%d"), cfg.MixWellCount);
		SetDlgItemText(IDC_EDIT31, str);
		str.Format(_T("%d"), cfg.CountPoolStandStillTime);
		SetDlgItemText(IDC_EDIT32, str);
		str.Format(_T("%d"), cfg.M40XLensXAxisViewStep);
		SetDlgItemText(IDC_EDIT33, str);
		str.Format(_T("%d"), cfg.M40XLensYAxisViewStep);
		SetDlgItemText(IDC_EDIT34, str);
		str.Format(_T("%d"), cfg.XAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT35, str);
		str.Format(_T("%d"), cfg.YAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT36, str);
		str.Format(_T("%d"), cfg.CountPoolChannelSpaceStep);
		SetDlgItemText(IDC_EDIT37, str);
		str.Format(_T("%d"), cfg.PaperColorBlockSpaceStep);
		SetDlgItemText(IDC_EDIT38, str);
		str.Format(_T("%d"), cfg.PipeInStep);
		SetDlgItemText(IDC_EDIT39, str);
		str.Format(_T("%d"), cfg.To1HoleStep);
		SetDlgItemText(IDC_EDIT41, str);
		str.Format(_T("%d"), cfg.AspriationPoolVolume);
		SetDlgItemText(IDC_EDIT49, str);
		str.Format(_T("%d"), cfg.CountPoolBetweenStep);
		SetDlgItemText(IDC_EDIT100, str);
	}
#else
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	if (!ReadHardwareParam(cfg))
	{
		MessageBox(_T("读取硬件参数失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		CString str;
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(cfg.TestType - 1);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.UdcCheckItemCount - 10);
		str.Format(_T("%d"), cfg.FeedPaperStepToSamplingPostion);
		SetDlgItemText(IDC_EDIT5, str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		SetDlgItemText(IDC_EDIT6, str);
		str.Format(_T("%d"), cfg.MoveToCleanPostionStep);
		SetDlgItemText(IDC_EDIT7, str);
		str.Format(_T("%d"), cfg.MoteToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT8, str);
		str.Format(_T("%d"), cfg.MoveToEmergencyOPostionSte);
		SetDlgItemText(IDC_EDIT9, str);
		str.Format(_T("%d"), cfg.DownToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), cfg.DownToSamplingPostionStep);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), cfg.XAxisCorrectStep);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), cfg.YAxisCorrectStep);
		SetDlgItemText(IDC_EDIT13, str);
		str.Format(_T("%d"), cfg.AspriationVolume);
		SetDlgItemText(IDC_EDIT14, str);
		str.Format(_T("%d"), cfg.CleanCountPoolVolume);
		SetDlgItemText(IDC_EDIT15, str);
		str.Format(_T("%d"), cfg.CleanPipeVolume);
		SetDlgItemText(IDC_EDIT16, str);
		str.Format(_T("%d"), cfg.ColorBlock1SamplingVolume);
		SetDlgItemText(IDC_EDIT17, str);
		str.Format(_T("%d"), cfg.ColorBlock2SamplingVolume);
		SetDlgItemText(IDC_EDIT18, str);
		str.Format(_T("%d"), cfg.ColorBlock3SamplingVolume);
		SetDlgItemText(IDC_EDIT19, str);
		str.Format(_T("%d"), cfg.ColorBlock4SamplingVolume);
		SetDlgItemText(IDC_EDIT20, str);
		str.Format(_T("%d"), cfg.ColorBlock5SamplingVolume);
		SetDlgItemText(IDC_EDIT21, str);
		str.Format(_T("%d"), cfg.ColorBlock6SamplingVolume);
		SetDlgItemText(IDC_EDIT22, str);
		str.Format(_T("%d"), cfg.ColorBlock7SamplingVolume);
		SetDlgItemText(IDC_EDIT23, str);
		str.Format(_T("%d"), cfg.ColorBlock8SamplingVolume);
		SetDlgItemText(IDC_EDIT24, str);
		str.Format(_T("%d"), cfg.ColorBlock9SamplingVolume);
		SetDlgItemText(IDC_EDIT25, str);
		str.Format(_T("%d"), cfg.ColorBlock10SamplingVolume);
		SetDlgItemText(IDC_EDIT26, str);
		str.Format(_T("%d"), cfg.ColorBlock11SamplingVolume);
		SetDlgItemText(IDC_EDIT27, str);
		str.Format(_T("%d"), cfg.ColorBlock12SamplingVolume);
		SetDlgItemText(IDC_EDIT28, str);
		str.Format(_T("%d"), cfg.ColorBlock13SamplingVolume);
		SetDlgItemText(IDC_EDIT29, str);
		str.Format(_T("%d"), cfg.ColorBlock14SamplingVolume);
		SetDlgItemText(IDC_EDIT30, str);
		str.Format(_T("%d"), cfg.MixWellCount);
		SetDlgItemText(IDC_EDIT31, str);
		str.Format(_T("%d"), cfg.CountPoolStandStillTime);
		SetDlgItemText(IDC_EDIT32, str);
		str.Format(_T("%d"), cfg.M40XLensXAxisViewStep);
		SetDlgItemText(IDC_EDIT33, str);
		str.Format(_T("%d"), cfg.M40XLensYAxisViewStep);
		SetDlgItemText(IDC_EDIT34, str);
		str.Format(_T("%d"), cfg.XAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT35, str);
		str.Format(_T("%d"), cfg.YAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT36, str);
		str.Format(_T("%d"), cfg.CountPoolChannelSpaceStep);
		SetDlgItemText(IDC_EDIT37, str);
		str.Format(_T("%d"), cfg.PaperColorBlockSpaceStep);
		SetDlgItemText(IDC_EDIT38, str);
		str.Format(_T("%d"), cfg.PipeInStep);
		SetDlgItemText(IDC_EDIT39, str);
		str.Format(_T("%d"), cfg.PipeOutStep);
		SetDlgItemText(IDC_EDIT40, str);
		str.Format(_T("%d"), cfg.To1HoleStep);
		SetDlgItemText(IDC_EDIT41, str);
	}
#endif // _LC_V
}


void CDbgHardwareparam::OnBnClickedWriteHardwareParam()
{
#if  (_LC_V <= 2150)
	if (IDOK == MessageBox(_T("是否写入参数?"),_T("提示"),  MB_OKCANCEL | MB_ICONWARNING))
	{
		bool bSuccess = false;
		HARDWARE_PARAM_CONTEXT cfg = { 0 };

		CString str;
		cfg.TestType = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel() + 1;
		GetDlgItemText(IDC_COMBO1, str);
		cfg.UdcCheckItemCount = _ttoi(str);
		GetDlgItemText(IDC_COMBO2, str);
		cfg.UdcCheckItemCount = _ttoi(str);
		GetDlgItemText(IDC_EDIT5, str);
		cfg.FeedPaperStepToSamplingPostion = _ttoi(str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		GetDlgItemText(IDC_EDIT6, str);
		cfg.UdcCrevice = _ttoi(str);
		GetDlgItemText(IDC_EDIT7, str);
		cfg.MoveToCleanPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT8, str);
		cfg.MoteToAspriationPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT9, str);
		cfg.MoveToEmergencyOPostionSte = _ttoi(str);
		GetDlgItemText(IDC_EDIT10, str);
		cfg.DownToAspriationPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT11, str);
		cfg.DownToSamplingPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT12, str);
		cfg.XAxisCorrectStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT13, str);
		cfg.YAxisCorrectStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT14, str);
		cfg.AspriationVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT15, str);
		cfg.CleanCountPoolVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT16, str);
		cfg.CleanPipeVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT17, str);
		cfg.ColorBlock1SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT18, str);
		cfg.ColorBlock2SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT19, str);
		cfg.ColorBlock3SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT20, str);
		cfg.ColorBlock4SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT21, str);
		cfg.ColorBlock5SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT22, str);
		cfg.ColorBlock6SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT23, str);
		cfg.ColorBlock7SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT24, str);
		cfg.ColorBlock8SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT25, str);
		cfg.ColorBlock9SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT26, str);
		cfg.ColorBlock10SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT27, str);
		cfg.ColorBlock11SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT28, str);
		cfg.ColorBlock12SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT29, str);
		cfg.ColorBlock13SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT30, str);
		cfg.ColorBlock14SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT31, str);
		cfg.MixWellCount = _ttoi(str);
		GetDlgItemText(IDC_EDIT32, str);
		cfg.CountPoolStandStillTime = _ttoi(str);
		GetDlgItemText(IDC_EDIT33, str);
		cfg.M40XLensXAxisViewStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT34, str);
		cfg.M40XLensYAxisViewStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT35, str);
		cfg.XAxisReturnClearanceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT36, str);
		cfg.YAxisReturnClearanceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT37, str);
		cfg.CountPoolChannelSpaceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT38, str);
		cfg.PaperColorBlockSpaceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT39, str);
		cfg.PipeInStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT40, str);
		cfg.PipeOutStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT41, str);
		cfg.To1HoleStep = _ttoi(str);

		WirteHardwareParam(cfg, bSuccess);
		if (bSuccess)
		{
			theApp.m_nUdcItemCount = cfg.UdcCheckItemCount;
		}
		MessageBox((bSuccess ? _T("写入硬件参数成功!") : _T("写入硬件参数失败!")), _T("提示"), MB_OK | (bSuccess ? MB_ICONINFORMATION : MB_ICONWARNING));
	}
	else
	{

	}
#elif (_LC_V == 2180)
	if (IDOK == MessageBox(_T("是否写入参数?"), _T("提示"), MB_OKCANCEL | MB_ICONWARNING))
	{
		bool bSuccess = false;
		HARDWARE_PARAM_CONTEXT cfg = { 0 };

		CString str;
		cfg.TestType = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel() + 1;
		GetDlgItemText(IDC_COMBO1, str);
		cfg.UdcCheckItemCount = _ttoi(str);
		GetDlgItemText(IDC_COMBO2, str);
		cfg.UdcCheckItemCount = _ttoi(str);
		GetDlgItemText(IDC_EDIT5, str);
		cfg.FeedPaperStepToSamplingPostion = _ttoi(str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		GetDlgItemText(IDC_EDIT6, str);
		cfg.UdcCrevice = _ttoi(str);
		GetDlgItemText(IDC_EDIT7, str);
		cfg.MoveToCleanPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT8, str);
		cfg.MoteToAspriationPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT9, str);
		cfg.MoveToEmergencyOPostionSte = _ttoi(str);
		GetDlgItemText(IDC_EDIT10, str);
		cfg.DownToAspriationPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT11, str);
		cfg.DownToSamplingPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT12, str);
		cfg.XAxisCorrectStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT13, str);
		cfg.YAxisCorrectStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT14, str);
		cfg.AspriationVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT15, str);
		cfg.CleanCountPoolVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT16, str);
		cfg.CleanPipeVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT17, str);
		cfg.ColorBlock1SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT18, str);
		cfg.ColorBlock2SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT19, str);
		cfg.ColorBlock3SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT20, str);
		cfg.ColorBlock4SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT21, str);
		cfg.ColorBlock5SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT22, str);
		cfg.ColorBlock6SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT23, str);
		cfg.ColorBlock7SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT24, str);
		cfg.ColorBlock8SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT25, str);
		cfg.ColorBlock9SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT26, str);
		cfg.ColorBlock10SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT27, str);
		cfg.ColorBlock11SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT28, str);
		cfg.ColorBlock12SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT29, str);
		cfg.ColorBlock13SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT30, str);
		cfg.ColorBlock14SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT31, str);
		cfg.MixWellCount = _ttoi(str);
		GetDlgItemText(IDC_EDIT32, str);
		cfg.CountPoolStandStillTime = _ttoi(str);
		GetDlgItemText(IDC_EDIT33, str);
		cfg.M40XLensXAxisViewStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT34, str);
		cfg.M40XLensYAxisViewStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT35, str);
		cfg.XAxisReturnClearanceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT36, str);
		cfg.YAxisReturnClearanceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT37, str);
		cfg.CountPoolChannelSpaceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT38, str);
		cfg.PaperColorBlockSpaceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT39, str);
		cfg.PipeInStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT40, str);
		cfg.PipeOutStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT41, str);
		cfg.To1HoleStep = _ttoi(str);

		WirteHardwareParam(cfg, bSuccess);
		if (bSuccess)
		{
			theApp.m_nUdcItemCount = cfg.UdcCheckItemCount;
		}
		MessageBox((bSuccess ? _T("写入硬件参数成功!") : _T("写入硬件参数失败!")), _T("提示"), MB_OK | (bSuccess ? MB_ICONINFORMATION : MB_ICONWARNING));
	}
	else
	{

	}
#elif (_LC_V >= 2190 && _LC_V < 2230)
if (IDOK == MessageBox(_T("是否写入参数?"), _T("提示"), MB_OKCANCEL | MB_ICONWARNING))
{
	bool bSuccess = false;
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	CString str;
	cfg.TestType = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel() + 1;
	GetDlgItemText(IDC_COMBO1, str);
	cfg.UdcCheckItemCount = _ttoi(str);
	GetDlgItemText(IDC_COMBO2, str);
	cfg.UdcCheckItemCount = _ttoi(str);
	GetDlgItemText(IDC_EDIT5, str);
	cfg.FeedPaperStepToSamplingPostion = _ttoi(str);
	str.Format(_T("%d"), cfg.UdcCrevice);
	GetDlgItemText(IDC_EDIT6, str);
	cfg.UdcCrevice = _ttoi(str);
	GetDlgItemText(IDC_EDIT7, str);
	cfg.MoveToCleanPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT8, str);
	cfg.MoteToAspriationPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT9, str);
	cfg.MoveToEmergencyOPostionSte = _ttoi(str);
	GetDlgItemText(IDC_EDIT10, str);
	cfg.DownToAspriationPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT11, str);
	cfg.DownToSamplingPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT12, str);
	cfg.XAxisCorrectStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT13, str);
	cfg.YAxisCorrectStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT14, str);
	cfg.AspriationVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT15, str);
	cfg.CleanCountPoolVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT16, str);
	cfg.CleanPipeVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT17, str);
	cfg.ColorBlock1SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT18, str);
	cfg.ColorBlock2SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT19, str);
	cfg.ColorBlock3SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT20, str);
	cfg.ColorBlock4SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT21, str);
	cfg.ColorBlock5SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT22, str);
	cfg.ColorBlock6SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT23, str);
	cfg.ColorBlock7SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT24, str);
	cfg.ColorBlock8SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT25, str);
	cfg.ColorBlock9SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT26, str);
	cfg.ColorBlock10SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT27, str);
	cfg.ColorBlock11SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT28, str);
	cfg.ColorBlock12SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT29, str);
	cfg.ColorBlock13SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT30, str);
	cfg.ColorBlock14SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT31, str);
	cfg.MixWellCount = _ttoi(str);
	GetDlgItemText(IDC_EDIT32, str);
	cfg.CountPoolStandStillTime = _ttoi(str);
	GetDlgItemText(IDC_EDIT33, str);
	cfg.M40XLensXAxisViewStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT34, str);
	cfg.M40XLensYAxisViewStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT35, str);
	cfg.XAxisReturnClearanceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT36, str);
	cfg.YAxisReturnClearanceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT37, str);
	cfg.CountPoolChannelSpaceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT38, str);
	cfg.PaperColorBlockSpaceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT39, str);
	cfg.PipeInStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT41, str);
	cfg.To1HoleStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT49, str);
	cfg.AspriationPoolVolume = _ttoi(str);
	WirteHardwareParam(cfg, bSuccess);
	if (bSuccess)
	{
		theApp.m_nUdcItemCount = cfg.UdcCheckItemCount;
	}
	MessageBox((bSuccess ? _T("写入硬件参数成功!") : _T("写入硬件参数失败!")), _T("提示"), MB_OK | (bSuccess ? MB_ICONINFORMATION : MB_ICONWARNING));
}
else
{

}
#elif (_LC_V >= 2230)
if (IDOK == MessageBox(_T("是否写入参数?"), _T("提示"), MB_OKCANCEL | MB_ICONWARNING))
{
	bool bSuccess = false;
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	CString str;
	cfg.TestType = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel() + 1;
	GetDlgItemText(IDC_COMBO1, str);
	cfg.UdcCheckItemCount = _ttoi(str);
	GetDlgItemText(IDC_COMBO2, str);
	cfg.UdcCheckItemCount = _ttoi(str);
	GetDlgItemText(IDC_EDIT5, str);
	cfg.FeedPaperStepToSamplingPostion = _ttoi(str);
	str.Format(_T("%d"), cfg.UdcCrevice);
	GetDlgItemText(IDC_EDIT6, str);
	cfg.UdcCrevice = _ttoi(str);
	GetDlgItemText(IDC_EDIT7, str);
	cfg.MoveToCleanPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT8, str);
	cfg.MoteToAspriationPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT9, str);
	cfg.MoveToEmergencyOPostionSte = _ttoi(str);
	GetDlgItemText(IDC_EDIT10, str);
	cfg.DownToAspriationPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT11, str);
	cfg.DownToSamplingPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT12, str);
	cfg.XAxisCorrectStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT13, str);
	cfg.YAxisCorrectStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT14, str);
	cfg.AspriationVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT15, str);
	cfg.CleanCountPoolVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT16, str);
	cfg.CleanPipeVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT17, str);
	cfg.ColorBlock1SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT18, str);
	cfg.ColorBlock2SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT19, str);
	cfg.ColorBlock3SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT20, str);
	cfg.ColorBlock4SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT21, str);
	cfg.ColorBlock5SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT22, str);
	cfg.ColorBlock6SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT23, str);
	cfg.ColorBlock7SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT24, str);
	cfg.ColorBlock8SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT25, str);
	cfg.ColorBlock9SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT26, str);
	cfg.ColorBlock10SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT27, str);
	cfg.ColorBlock11SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT28, str);
	cfg.ColorBlock12SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT29, str);
	cfg.ColorBlock13SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT30, str);
	cfg.ColorBlock14SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT31, str);
	cfg.MixWellCount = _ttoi(str);
	GetDlgItemText(IDC_EDIT32, str);
	cfg.CountPoolStandStillTime = _ttoi(str);
	GetDlgItemText(IDC_EDIT33, str);
	cfg.M40XLensXAxisViewStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT34, str);
	cfg.M40XLensYAxisViewStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT35, str);
	cfg.XAxisReturnClearanceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT36, str);
	cfg.YAxisReturnClearanceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT37, str);
	cfg.CountPoolChannelSpaceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT38, str);
	cfg.PaperColorBlockSpaceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT39, str);
	cfg.PipeInStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT41, str);
	cfg.To1HoleStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT49, str);
	cfg.AspriationPoolVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT100, str);
	cfg.CountPoolBetweenStep = _ttoi(str);
	WirteHardwareParam(cfg, bSuccess);
	if (bSuccess)
	{
		theApp.m_nUdcItemCount = cfg.UdcCheckItemCount;
	}
	MessageBox((bSuccess ? _T("写入硬件参数成功!") : _T("写入硬件参数失败!")), _T("提示"), MB_OK | (bSuccess ? MB_ICONINFORMATION : MB_ICONWARNING));
}
else
{

}
#else
if (IDOK == MessageBox(_T("是否写入参数?"), _T("提示"), MB_OKCANCEL | MB_ICONWARNING))
{
	bool bSuccess = false;
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	CString str;
	cfg.TestType = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel() + 1;
	GetDlgItemText(IDC_COMBO1, str);
	cfg.UdcCheckItemCount = _ttoi(str);
	GetDlgItemText(IDC_COMBO2, str);
	cfg.UdcCheckItemCount = _ttoi(str);
	GetDlgItemText(IDC_EDIT5, str);
	cfg.FeedPaperStepToSamplingPostion = _ttoi(str);
	str.Format(_T("%d"), cfg.UdcCrevice);
	GetDlgItemText(IDC_EDIT6, str);
	cfg.UdcCrevice = _ttoi(str);
	GetDlgItemText(IDC_EDIT7, str);
	cfg.MoveToCleanPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT8, str);
	cfg.MoteToAspriationPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT9, str);
	cfg.MoveToEmergencyOPostionSte = _ttoi(str);
	GetDlgItemText(IDC_EDIT10, str);
	cfg.DownToAspriationPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT11, str);
	cfg.DownToSamplingPostionStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT12, str);
	cfg.XAxisCorrectStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT13, str);
	cfg.YAxisCorrectStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT14, str);
	cfg.AspriationVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT15, str);
	cfg.CleanCountPoolVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT16, str);
	cfg.CleanPipeVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT17, str);
	cfg.ColorBlock1SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT18, str);
	cfg.ColorBlock2SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT19, str);
	cfg.ColorBlock3SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT20, str);
	cfg.ColorBlock4SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT21, str);
	cfg.ColorBlock5SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT22, str);
	cfg.ColorBlock6SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT23, str);
	cfg.ColorBlock7SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT24, str);
	cfg.ColorBlock8SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT25, str);
	cfg.ColorBlock9SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT26, str);
	cfg.ColorBlock10SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT27, str);
	cfg.ColorBlock11SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT28, str);
	cfg.ColorBlock12SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT29, str);
	cfg.ColorBlock13SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT30, str);
	cfg.ColorBlock14SamplingVolume = _ttoi(str);
	GetDlgItemText(IDC_EDIT31, str);
	cfg.MixWellCount = _ttoi(str);
	GetDlgItemText(IDC_EDIT32, str);
	cfg.CountPoolStandStillTime = _ttoi(str);
	GetDlgItemText(IDC_EDIT33, str);
	cfg.M40XLensXAxisViewStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT34, str);
	cfg.M40XLensYAxisViewStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT35, str);
	cfg.XAxisReturnClearanceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT36, str);
	cfg.YAxisReturnClearanceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT37, str);
	cfg.CountPoolChannelSpaceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT38, str);
	cfg.PaperColorBlockSpaceStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT39, str);
	cfg.PipeInStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT40, str);
	cfg.PipeOutStep = _ttoi(str);
	GetDlgItemText(IDC_EDIT41, str);
	cfg.To1HoleStep = _ttoi(str);

	WirteHardwareParam(cfg, bSuccess);
	if (bSuccess)
	{
		theApp.m_nUdcItemCount = cfg.UdcCheckItemCount;
	}
	MessageBox((bSuccess ? _T("写入硬件参数成功!") : _T("写入硬件参数失败!")), _T("提示"), MB_OK | (bSuccess ? MB_ICONINFORMATION : MB_ICONWARNING));
}
else
{

}
#endif //  LC_V1_5



}


void CDbgHardwareparam::OnBnClickedButton15()
{
	// TODO:  在此添加控件通知处理程序代码
	if (IDOK == MessageBox(_T("是否将当前参数设置为默认参数?"), _T("提示"), MB_OKCANCEL | MB_ICONWARNING))
	{
		bool bSuccess = false;
		HARDWARE_PARAM_CONTEXT cfg = { 0 };
		CString str;
		cfg.TestType = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel() + 1;
		GetDlgItemText(IDC_COMBO1, str);
		cfg.UdcCheckItemCount = _ttoi(str);
		GetDlgItemText(IDC_COMBO2, str);
		cfg.UdcCheckItemCount = _ttoi(str);
		GetDlgItemText(IDC_EDIT5, str);
		cfg.FeedPaperStepToSamplingPostion = _ttoi(str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		GetDlgItemText(IDC_EDIT6, str);
		cfg.UdcCrevice = _ttoi(str);
		GetDlgItemText(IDC_EDIT7, str);
		cfg.MoveToCleanPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT8, str);
		cfg.MoteToAspriationPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT9, str);
		cfg.MoveToEmergencyOPostionSte = _ttoi(str);
		GetDlgItemText(IDC_EDIT10, str);
		cfg.DownToAspriationPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT11, str);
		cfg.DownToSamplingPostionStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT12, str);
		cfg.XAxisCorrectStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT13, str);
		cfg.YAxisCorrectStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT14, str);
		cfg.AspriationVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT15, str);
		cfg.CleanCountPoolVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT16, str);
		cfg.CleanPipeVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT17, str);
		cfg.ColorBlock1SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT18, str);
		cfg.ColorBlock2SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT19, str);
		cfg.ColorBlock3SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT20, str);
		cfg.ColorBlock4SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT21, str);
		cfg.ColorBlock5SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT22, str);
		cfg.ColorBlock6SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT23, str);
		cfg.ColorBlock7SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT24, str);
		cfg.ColorBlock8SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT25, str);
		cfg.ColorBlock9SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT26, str);
		cfg.ColorBlock10SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT27, str);
		cfg.ColorBlock11SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT28, str);
		cfg.ColorBlock12SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT29, str);
		cfg.ColorBlock13SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT30, str);
		cfg.ColorBlock14SamplingVolume = _ttoi(str);
		GetDlgItemText(IDC_EDIT31, str);
		cfg.MixWellCount = _ttoi(str);
		GetDlgItemText(IDC_EDIT32, str);
		cfg.CountPoolStandStillTime = _ttoi(str);
		GetDlgItemText(IDC_EDIT33, str);
		cfg.M40XLensXAxisViewStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT34, str);
		cfg.M40XLensYAxisViewStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT35, str);
		cfg.XAxisReturnClearanceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT36, str);
		cfg.YAxisReturnClearanceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT37, str);
		cfg.CountPoolChannelSpaceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT38, str);
		cfg.PaperColorBlockSpaceStep = _ttoi(str);
		GetDlgItemText(IDC_EDIT39, str);
		cfg.PipeInStep = _ttoi(str);
#if   (_LC_V <= 2150)
		GetDlgItemText(IDC_EDIT40, str);
		cfg.PipeOutStep = _ttoi(str);
#elif (_LC_V == 2180)
		GetDlgItemText(IDC_EDIT40, str);
		cfg.PipeOutStep = _ttoi(str);
#elif (_LC_V>=2190 && _LC_V<2230 )
		GetDlgItemText(IDC_EDIT49, str);
		cfg.AspriationPoolVolume = _ttoi(str);
#elif (_LC_V>=2230)
		GetDlgItemText(IDC_EDIT49, str);
		cfg.AspriationPoolVolume = _ttoi(str);

		GetDlgItemText(IDC_EDIT100, str);
		cfg.CountPoolBetweenStep= _ttoi(str);
#else
		GetDlgItemText(IDC_EDIT40, str);
		cfg.PipeOutStep = _ttoi(str);
#endif
		GetDlgItemText(IDC_EDIT41, str);
		cfg.To1HoleStep = _ttoi(str);

		bSuccess = SaveDefaultParam(cfg);
		MessageBox((bSuccess ? _T("写入默认参数成功!") : _T("写入默认参数失败!")), _T("提示"), MB_OK | (bSuccess ? MB_ICONINFORMATION : MB_ICONWARNING));
	}
	else
	{

	}

}

bool CDbgHardwareparam::SaveDefaultParam(HARDWARE_PARAM_CONTEXT cfg)
{

	TCHAR sModuleFile[MAX_PATH] = { 0 };
	TCHAR sDrive[MAX_PATH] = { 0 };
	TCHAR sDir[MAX_PATH] = { 0 };
	char buffer[64] = { 0 };

	GetModuleFileName(NULL, sModuleFile, MAX_PATH);
	_wsplitpath(sModuleFile, sDrive, sDir, NULL, NULL);
	TCHAR inifile[MAX_PATH] = { 0 };
	wsprintf(inifile, _T("%s%s%s.ini"), sDrive, sDir, _T("defaultparam"));

	WritePrivateProfileString(_T("defaultparam"),_T("TestType"),							(LPCTSTR)_bstr_t(_itoa(cfg.TestType,buffer,10))							,inifile);
	WritePrivateProfileString(_T("defaultparam"),_T("UdcCheckItemCount"),					(LPCTSTR)_bstr_t(_itoa(cfg.UdcCheckItemCount,buffer,10))				,inifile);
	WritePrivateProfileString(_T("defaultparam"),_T("FeedPaperStepToSamplingPostion"),		(LPCTSTR)_bstr_t(_itoa(cfg.FeedPaperStepToSamplingPostion,buffer,10))	,inifile);
	WritePrivateProfileString(_T("defaultparam"),_T("UdcCrevice"),							(LPCTSTR)_bstr_t(_itoa(cfg.UdcCrevice,buffer,10))						,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("MoveToCleanPostionStep"), 			(LPCTSTR)_bstr_t(_itoa(cfg.MoveToCleanPostionStep, buffer, 10))			,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("MoteToAspriationPostionStep"), 		(LPCTSTR)_bstr_t(_itoa(cfg.MoteToAspriationPostionStep, buffer, 10))	,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("MoveToEmergencyOPostionStep"), 		(LPCTSTR)_bstr_t(_itoa(cfg.MoveToEmergencyOPostionSte, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("DownToAspriationPostionStep"), 		(LPCTSTR)_bstr_t(_itoa(cfg.DownToAspriationPostionStep, buffer, 10))	,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("DownToSamplingPostionStep"), 			(LPCTSTR)_bstr_t(_itoa(cfg.DownToSamplingPostionStep, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("XAxisCorrectStep"),					(LPCTSTR)_bstr_t(_itoa(cfg.XAxisCorrectStep, buffer, 10))				,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("YAxisCorrectStep"), 					(LPCTSTR)_bstr_t(_itoa(cfg.YAxisCorrectStep, buffer, 10))				,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("AspriationVolume"), 					(LPCTSTR)_bstr_t(_itoa(cfg.AspriationVolume, buffer, 10))				,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("CleanCountPoolVolume"), 				(LPCTSTR)_bstr_t(_itoa(cfg.CleanCountPoolVolume, buffer, 10))				,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("CleanPipeVolume"), 					(LPCTSTR)_bstr_t(_itoa(cfg.CleanPipeVolume, buffer, 10))				,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock1SamplingVolume"), 			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock1SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock2SamplingVolume"), 			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock2SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock3SamplingVolume"), 			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock3SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock4SamplingVolume"), 			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock4SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock5SamplingVolume"),			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock5SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock6SamplingVolume"), 			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock6SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock7SamplingVolume"), 			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock7SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock8SamplingVolume"), 			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock8SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock9SamplingVolume"), 			(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock9SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock10SamplingVolume"), 		(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock10SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock11SamplingVolume"), 		(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock11SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock12SamplingVolume"), 		(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock12SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock13SamplingVolume"), 		(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock13SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("ColorBlock14SamplingVolume"), 		(LPCTSTR)_bstr_t(_itoa(cfg.ColorBlock14SamplingVolume, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("MixWellCount"), 						(LPCTSTR)_bstr_t(_itoa(cfg.MixWellCount, buffer, 10))					,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("CountPoolStandStillTime"), 			(LPCTSTR)_bstr_t(_itoa(cfg.CountPoolStandStillTime, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("M40XLensXAxisViewStep"), 				(LPCTSTR)_bstr_t(_itoa(cfg.M40XLensXAxisViewStep, buffer, 10))			,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("M40XLensYAxisViewStep"), 				(LPCTSTR)_bstr_t(_itoa(cfg.M40XLensYAxisViewStep, buffer, 10))			,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("XAxisReturnClearanceStep"), 			(LPCTSTR)_bstr_t(_itoa(cfg.XAxisReturnClearanceStep, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("YAxisReturnClearanceStep"), 			(LPCTSTR)_bstr_t(_itoa(cfg.YAxisReturnClearanceStep, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("CountPoolChannelSpaceStep"), 			(LPCTSTR)_bstr_t(_itoa(cfg.CountPoolChannelSpaceStep, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("PaperColorBlockSpaceStep"), 			(LPCTSTR)_bstr_t(_itoa(cfg.PaperColorBlockSpaceStep, buffer, 10))		,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("PipeInStep"), 						(LPCTSTR)_bstr_t(_itoa(cfg.PipeInStep, buffer, 10))						,inifile);

#if (_LC_V <= 2150)
	WritePrivateProfileString(_T("defaultparam"), _T("PipeOutStep"), 						(LPCTSTR)_bstr_t(_itoa(cfg.PipeOutStep, buffer, 10))					,inifile);
#elif (_LC_V == 2180)
	WritePrivateProfileString(_T("defaultparam"), _T("PipeOutStep"), 						(LPCTSTR)_bstr_t(_itoa(cfg.PipeOutStep, buffer, 10))					,inifile);
#elif (_LC_V >= 2190 && _LC_V < 2230)
	WritePrivateProfileString(_T("defaultparam"), _T("AspriationPoolVolume"), 				(LPCTSTR)_bstr_t(_itoa(cfg.AspriationPoolVolume, buffer, 10))			,inifile);
#elif (_LC_V >= 2230)
	WritePrivateProfileString(_T("defaultparam"), _T("AspriationPoolVolume"), 				(LPCTSTR)_bstr_t(_itoa(cfg.AspriationPoolVolume, buffer, 10))			,inifile);
	WritePrivateProfileString(_T("defaultparam"), _T("CountPoolBetweenStep"), 				(LPCTSTR)_bstr_t(_itoa(cfg.CountPoolBetweenStep, buffer, 10))			,inifile);
#else
	WritePrivateProfileString(_T("defaultparam"), _T("PipeOutStep"),						(LPCTSTR)_bstr_t(_itoa(cfg.PipeOutStep, buffer, 10))					, inifile);
#endif
	WritePrivateProfileString(_T("defaultparam"), _T("To1HoleStep"), 						(LPCTSTR)_bstr_t(_itoa(cfg.To1HoleStep, buffer, 10))					,inifile);
	return true;
}


bool CDbgHardwareparam::ReadDefaultParam(HARDWARE_PARAM_CONTEXT* cfg)
{
	TCHAR sModuleFile[MAX_PATH] = { 0 };
	TCHAR sDrive[MAX_PATH] = { 0 };
	TCHAR sDir[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, sModuleFile, MAX_PATH);
	_wsplitpath(sModuleFile, sDrive, sDir, NULL, NULL);
	TCHAR inifile[MAX_PATH] = { 0 };
	wsprintf(inifile, _T("%s%s%s.ini"), sDrive, sDir, _T("defaultparam"));

	cfg->TestType 							= GetPrivateProfileInt(_T("defaultparam"), _T("TestType"),							0, inifile);
	cfg->UdcCheckItemCount 					= GetPrivateProfileInt(_T("defaultparam"), _T("UdcCheckItemCount"), 				0, inifile);
	cfg->FeedPaperStepToSamplingPostion 	= GetPrivateProfileInt(_T("defaultparam"), _T("FeedPaperStepToSamplingPostion"),	0, inifile);
	cfg->UdcCrevice 						= GetPrivateProfileInt(_T("defaultparam"), _T("UdcCrevice"), 						0, inifile);
	cfg->MoveToCleanPostionStep 			= GetPrivateProfileInt(_T("defaultparam"), _T("MoveToCleanPostionStep"), 			0, inifile);
	cfg->MoteToAspriationPostionStep 		= GetPrivateProfileInt(_T("defaultparam"), _T("MoteToAspriationPostionStep"), 		0, inifile);
	cfg->MoveToEmergencyOPostionSte 		= GetPrivateProfileInt(_T("defaultparam"), _T("MoveToEmergencyOPostionStep"),		0, inifile);
	cfg->DownToAspriationPostionStep 		= GetPrivateProfileInt(_T("defaultparam"), _T("DownToAspriationPostionStep"),		0, inifile);
	cfg->DownToSamplingPostionStep 			= GetPrivateProfileInt(_T("defaultparam"), _T("DownToSamplingPostionStep"),			0, inifile);
	cfg->XAxisCorrectStep 					= GetPrivateProfileInt(_T("defaultparam"), _T("XAxisCorrectStep"), 					0, inifile);
	cfg->YAxisCorrectStep 					= GetPrivateProfileInt(_T("defaultparam"), _T("YAxisCorrectStep"), 					0, inifile);
	cfg->AspriationVolume 					= GetPrivateProfileInt(_T("defaultparam"), _T("AspriationVolume"), 					0, inifile);
	cfg->CleanCountPoolVolume				= GetPrivateProfileInt(_T("defaultparam"), _T("CleanCountPoolVolume"),				0, inifile);
	cfg->CleanPipeVolume 					= GetPrivateProfileInt(_T("defaultparam"), _T("CleanPipeVolume"),  					0, inifile);
	cfg->ColorBlock1SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock1SamplingVolume"), 		0, inifile);
	cfg->ColorBlock2SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock2SamplingVolume"), 		0, inifile);
	cfg->ColorBlock3SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock3SamplingVolume"), 		0, inifile);
	cfg->ColorBlock4SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock4SamplingVolume"), 		0, inifile);
	cfg->ColorBlock5SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock5SamplingVolume"), 		0, inifile);
	cfg->ColorBlock6SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock6SamplingVolume"), 		0, inifile);
	cfg->ColorBlock7SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock7SamplingVolume"), 		0, inifile);
	cfg->ColorBlock8SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock8SamplingVolume"), 		0, inifile);
	cfg->ColorBlock9SamplingVolume 			= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock9SamplingVolume"), 		0, inifile);
	cfg->ColorBlock10SamplingVolume 		= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock10SamplingVolume"),		0, inifile);
	cfg->ColorBlock11SamplingVolume 		= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock11SamplingVolume"),		0, inifile);
	cfg->ColorBlock12SamplingVolume 		= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock12SamplingVolume"),		0, inifile);
	cfg->ColorBlock13SamplingVolume 		= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock13SamplingVolume"),		0, inifile);
	cfg->ColorBlock14SamplingVolume 		= GetPrivateProfileInt(_T("defaultparam"), _T("ColorBlock14SamplingVolume"),		0, inifile);
	cfg->MixWellCount 						= GetPrivateProfileInt(_T("defaultparam"), _T("MixWellCount"),   					0, inifile);
	cfg->CountPoolStandStillTime 			= GetPrivateProfileInt(_T("defaultparam"), _T("CountPoolStandStillTime"),  			0, inifile);
	cfg->M40XLensXAxisViewStep 				= GetPrivateProfileInt(_T("defaultparam"), _T("M40XLensXAxisViewStep"), 			0, inifile);
	cfg->M40XLensYAxisViewStep 				= GetPrivateProfileInt(_T("defaultparam"), _T("M40XLensYAxisViewStep"), 			0, inifile);
	cfg->XAxisReturnClearanceStep 			= GetPrivateProfileInt(_T("defaultparam"), _T("XAxisReturnClearanceStep"), 			0, inifile);
	cfg->YAxisReturnClearanceStep 			= GetPrivateProfileInt(_T("defaultparam"), _T("YAxisReturnClearanceStep"), 			0, inifile);
	cfg->CountPoolChannelSpaceStep 			= GetPrivateProfileInt(_T("defaultparam"), _T("CountPoolChannelSpaceStep"),			0, inifile);
	cfg->PaperColorBlockSpaceStep 			= GetPrivateProfileInt(_T("defaultparam"), _T("PaperColorBlockSpaceStep"),  		0, inifile);
	cfg->PipeInStep							= GetPrivateProfileInt(_T("defaultparam"), _T("PipeInStep"),  						0, inifile);
	
#if (_LC_V <=2150)
	cfg->PipeOutStep 						= GetPrivateProfileInt(_T("defaultparam"), _T("PipeOutStep"), 						0, inifile);
#elif (_LC_V ==2180)
	cfg->PipeOutStep 						= GetPrivateProfileInt(_T("defaultparam"), _T("PipeOutStep"), 						0, inifile);
#elif (_LC_V >=2190 && _LC_V < 2230)
	cfg->AspriationPoolVolume				= GetPrivateProfileInt(_T("defaultparam"), _T("AspriationPoolVolume"),				0, inifile);
#elif (_LC_V >=2230)
	cfg->AspriationPoolVolume				= GetPrivateProfileInt(_T("defaultparam"), _T("AspriationPoolVolume"),				0, inifile);
	cfg->CountPoolBetweenStep			= GetPrivateProfileInt(_T("defaultparam"), _T("CountPoolBetweenStep"),				0, inifile);
#else
	cfg->PipeOutStep 						= GetPrivateProfileInt(_T("defaultparam"), _T("PipeOutStep"), 						0, inifile);
#endif
	cfg->To1HoleStep 						= GetPrivateProfileInt(_T("defaultparam"), _T("To1HoleStep"), 						0, inifile);
	return true;
}


void CDbgHardwareparam::OnBnClickedButton31()
{
	// TODO:  在此添加控件通知处理程序代码
	HARDWARE_PARAM_CONTEXT cfg = { 0 };

	if (!ReadDefaultParam(&cfg))
	{
		MessageBox(_T("读取默认参数失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		CString str;
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(cfg.TestType - 1);
		((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(cfg.UdcCheckItemCount - 10);
		str.Format(_T("%d"), cfg.FeedPaperStepToSamplingPostion);
		SetDlgItemText(IDC_EDIT5, str);
		str.Format(_T("%d"), cfg.UdcCrevice);
		SetDlgItemText(IDC_EDIT6, str);
		str.Format(_T("%d"), cfg.MoveToCleanPostionStep);
		SetDlgItemText(IDC_EDIT7, str);
		str.Format(_T("%d"), cfg.MoteToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT8, str);
		str.Format(_T("%d"), cfg.MoveToEmergencyOPostionSte);
		SetDlgItemText(IDC_EDIT9, str);
		str.Format(_T("%d"), cfg.DownToAspriationPostionStep);
		SetDlgItemText(IDC_EDIT10, str);
		str.Format(_T("%d"), cfg.DownToSamplingPostionStep);
		SetDlgItemText(IDC_EDIT11, str);
		str.Format(_T("%d"), cfg.XAxisCorrectStep);
		SetDlgItemText(IDC_EDIT12, str);
		str.Format(_T("%d"), cfg.YAxisCorrectStep);
		SetDlgItemText(IDC_EDIT13, str);
		str.Format(_T("%d"), cfg.AspriationVolume);
		SetDlgItemText(IDC_EDIT14, str);
		str.Format(_T("%d"), cfg.CleanCountPoolVolume);
		SetDlgItemText(IDC_EDIT15, str);
		str.Format(_T("%d"), cfg.CleanPipeVolume);
		SetDlgItemText(IDC_EDIT16, str);
		str.Format(_T("%d"), cfg.ColorBlock1SamplingVolume);
		SetDlgItemText(IDC_EDIT17, str);
		str.Format(_T("%d"), cfg.ColorBlock2SamplingVolume);
		SetDlgItemText(IDC_EDIT18, str);
		str.Format(_T("%d"), cfg.ColorBlock3SamplingVolume);
		SetDlgItemText(IDC_EDIT19, str);
		str.Format(_T("%d"), cfg.ColorBlock4SamplingVolume);
		SetDlgItemText(IDC_EDIT20, str);
		str.Format(_T("%d"), cfg.ColorBlock5SamplingVolume);
		SetDlgItemText(IDC_EDIT21, str);
		str.Format(_T("%d"), cfg.ColorBlock6SamplingVolume);
		SetDlgItemText(IDC_EDIT22, str);
		str.Format(_T("%d"), cfg.ColorBlock7SamplingVolume);
		SetDlgItemText(IDC_EDIT23, str);
		str.Format(_T("%d"), cfg.ColorBlock8SamplingVolume);
		SetDlgItemText(IDC_EDIT24, str);
		str.Format(_T("%d"), cfg.ColorBlock9SamplingVolume);
		SetDlgItemText(IDC_EDIT25, str);
		str.Format(_T("%d"), cfg.ColorBlock10SamplingVolume);
		SetDlgItemText(IDC_EDIT26, str);
		str.Format(_T("%d"), cfg.ColorBlock11SamplingVolume);
		SetDlgItemText(IDC_EDIT27, str);
		str.Format(_T("%d"), cfg.ColorBlock12SamplingVolume);
		SetDlgItemText(IDC_EDIT28, str);
		str.Format(_T("%d"), cfg.ColorBlock13SamplingVolume);
		SetDlgItemText(IDC_EDIT29, str);
		str.Format(_T("%d"), cfg.ColorBlock14SamplingVolume);
		SetDlgItemText(IDC_EDIT30, str);
		str.Format(_T("%d"), cfg.MixWellCount);
		SetDlgItemText(IDC_EDIT31, str);
		str.Format(_T("%d"), cfg.CountPoolStandStillTime);
		SetDlgItemText(IDC_EDIT32, str);
		str.Format(_T("%d"), cfg.M40XLensXAxisViewStep);
		SetDlgItemText(IDC_EDIT33, str);
		str.Format(_T("%d"), cfg.M40XLensYAxisViewStep);
		SetDlgItemText(IDC_EDIT34, str);
		str.Format(_T("%d"), cfg.XAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT35, str);
		str.Format(_T("%d"), cfg.YAxisReturnClearanceStep);
		SetDlgItemText(IDC_EDIT36, str);
		str.Format(_T("%d"), cfg.CountPoolChannelSpaceStep);
		SetDlgItemText(IDC_EDIT37, str);
		str.Format(_T("%d"), cfg.PaperColorBlockSpaceStep);
		SetDlgItemText(IDC_EDIT38, str);
		str.Format(_T("%d"), cfg.PipeInStep);
		SetDlgItemText(IDC_EDIT39, str);

#if (_LC_V <= 2150)
		str.Format(_T("%d"), cfg.PipeOutStep);
		SetDlgItemText(IDC_EDIT40, str);
#elif  (_LC_V == 2180)
		str.Format(_T("%d"), cfg.PipeOutStep);
		SetDlgItemText(IDC_EDIT40, str);
#elif  (_LC_V >= 2190 && _LC_V < 2230)
		str.Format(_T("%d"), cfg.AspriationPoolVolume);
		SetDlgItemText(IDC_EDIT49, str);
#elif  (_LC_V >= 2230)
		str.Format(_T("%d"), cfg.AspriationPoolVolume);
		SetDlgItemText(IDC_EDIT49, str);

		str.Format(_T("%d"), cfg.CountPoolBetweenStep);
		SetDlgItemText(IDC_EDIT100, str);
#else
		str.Format(_T("%d"), cfg.PipeOutStep);
		SetDlgItemText(IDC_EDIT40, str);
#endif
		str.Format(_T("%d"), cfg.To1HoleStep);
		SetDlgItemText(IDC_EDIT41, str);
	}
}


void CDbgHardwareparam::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
#ifdef DEBUG
	return;
#endif // DEBUG

	if (bShow)
		OnBnClickedReadHardwareParam();
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


void CDbgHardwareparam::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	GetHardwareVersion();
}
