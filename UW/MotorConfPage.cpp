

// MotorConfPage.cpp : 实现文件

#include "stdafx.h"
#include "MotorConfPage.h"
#include "afxdialogex.h"
#include "CmdHelper.h"
#include "UW.h"


// CMotorConfPage 对话框

IMPLEMENT_DYNAMIC(CMotorConfPage, CPropertyPage)

CMotorConfPage::CMotorConfPage()
	: CPropertyPage(CMotorConfPage::IDD)
	, m_run1(0)
	, m_run2(0)
	, m_run3(0)
	, m_run4(0)
	, m_run5(0)
	, m_run6(0)
	, m_run7(0)
	, m_run8(0)
	, m_run9(0)
	, m_run10(0)
	, m_run11(0)
	, m_run12(0)
	, m_standby1(0)
	, m_standby2(0)
	, m_standby3(0)
	, m_standby4(0)
	, m_standby5(0)
	, m_standby6(0)
	, m_standby7(0)
	, m_standby8(0)
	, m_standby9(0)
	, m_standby10(0)
	, m_standby11(0)
	, m_standby12(0)
{

}

CMotorConfPage::~CMotorConfPage()
{
}

void CMotorConfPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_com1);
	DDX_Control(pDX, IDC_COMBO12, m_com2);
	DDX_Control(pDX, IDC_COMBO13, m_com3);
	DDX_Control(pDX, IDC_COMBO14, m_com4);
	DDX_Control(pDX, IDC_COMBO15, m_com5);
	DDX_Control(pDX, IDC_COMBO16, m_com6);
	DDX_Control(pDX, IDC_COMBO17, m_com7);
	DDX_Control(pDX, IDC_COMBO18, m_com8);
	DDX_Control(pDX, IDC_COMBO19, m_com9);
	DDX_Control(pDX, IDC_COMBO20, m_com10);
	DDX_Control(pDX, IDC_COMBO21, m_com11);
	DDX_Control(pDX, IDC_COMBO22, m_com12);
	DDX_Text(pDX, IDC_EDIT1, m_run1);
	DDX_Text(pDX, IDC_EDIT56, m_run2);
	DDX_Text(pDX, IDC_EDIT59, m_run3);
	DDX_Text(pDX, IDC_EDIT61, m_run4);
	DDX_Text(pDX, IDC_EDIT63, m_run5);
	DDX_Text(pDX, IDC_EDIT65, m_run6);
	DDX_Text(pDX, IDC_EDIT67, m_run7);
	DDX_Text(pDX, IDC_EDIT69, m_run8);
	DDX_Text(pDX, IDC_EDIT71, m_run9);
	DDX_Text(pDX, IDC_EDIT73, m_run10);
	DDX_Text(pDX, IDC_EDIT75, m_run11);
	DDX_Text(pDX, IDC_EDIT77, m_run12);
	DDX_Text(pDX, IDC_EDIT55, m_standby1);
	DDX_Text(pDX, IDC_EDIT57, m_standby2);
	DDX_Text(pDX, IDC_EDIT60, m_standby3);
	DDX_Text(pDX, IDC_EDIT62, m_standby4);
	DDX_Text(pDX, IDC_EDIT64, m_standby5);
	DDX_Text(pDX, IDC_EDIT66, m_standby6);
	DDX_Text(pDX, IDC_EDIT68, m_standby7);
	DDX_Text(pDX, IDC_EDIT70, m_standby8);
	DDX_Text(pDX, IDC_EDIT72, m_standby9);
	DDX_Text(pDX, IDC_EDIT74, m_standby10);
	DDX_Text(pDX, IDC_EDIT76, m_standby11);
	DDX_Text(pDX, IDC_EDIT78, m_standby12);
	DDX_Control(pDX, IDC_COMBO2, m_cbxDirect1);
	DDX_Control(pDX, IDC_COMBO4, m_cbxDirect2);
	DDX_Control(pDX, IDC_COMBO23, m_cbxDirect3);
	DDX_Control(pDX, IDC_COMBO24, m_cbxDirect4);
	DDX_Control(pDX, IDC_COMBO25, m_cbxDirect5);
	DDX_Control(pDX, IDC_COMBO26, m_cbxDirect6);
	DDX_Control(pDX, IDC_COMBO27, m_cbxDirect7);
	DDX_Control(pDX, IDC_COMBO28, m_cbxDirect8);
	DDX_Control(pDX, IDC_COMBO29, m_cbxDirect9);
	DDX_Control(pDX, IDC_COMBO30, m_cbxDirect10);
	DDX_Control(pDX, IDC_COMBO31, m_cbxDirect11);
	DDX_Control(pDX, IDC_COMBO32, m_cbxDirect12);
}


BEGIN_MESSAGE_MAP(CMotorConfPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON2, &CMotorConfPage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON30, &CMotorConfPage::OnBnClickedButton30)
	ON_BN_CLICKED(IDC_BUTTON42, &CMotorConfPage::OnBnClickedButton42)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CMotorConfPage 消息处理程序


BOOL CMotorConfPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(L"电机驱动配置");
	AddStringToComBo(m_com1);
	AddStringToComBo(m_com2);
	AddStringToComBo(m_com3);
	AddStringToComBo(m_com4);
	AddStringToComBo(m_com5);
	AddStringToComBo(m_com6); 
	AddStringToComBo(m_com7);
	AddStringToComBo(m_com8);
	AddStringToComBo(m_com9);
	AddStringToComBo(m_com10);
	AddStringToComBo(m_com11);
	AddStringToComBo(m_com12);

	InitDirectComboxCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CMotorConfPage::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	MOTOR_CONF_CONTEXT cfg = { 0 };

	if (!ReadMotorConf(cfg))
	{
		MessageBox(_T("读取电机驱动配置失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		CString str;	
		m_com1.SetCurSel(cfg.item[0].motor_combo);
		m_com2.SetCurSel(cfg.item[1].motor_combo);
		m_com3.SetCurSel(cfg.item[2].motor_combo);
		m_com4.SetCurSel(cfg.item[3].motor_combo);
		m_com5.SetCurSel(cfg.item[4].motor_combo);
		m_com6.SetCurSel(cfg.item[5].motor_combo);
		m_com7.SetCurSel(cfg.item[6].motor_combo);
		m_com8.SetCurSel(cfg.item[7].motor_combo);
		m_com9.SetCurSel(cfg.item[8].motor_combo);
		m_com10.SetCurSel(cfg.item[9].motor_combo);
		m_com11.SetCurSel(cfg.item[10].motor_combo);
		m_com12.SetCurSel(cfg.item[11].motor_combo);
		m_run1 = cfg.item[0].motor_run;
		m_run2 = cfg.item[1].motor_run;
		m_run3 = cfg.item[2].motor_run;
		m_run4 = cfg.item[3].motor_run;
		m_run5 = cfg.item[4].motor_run;
		m_run6 = cfg.item[5].motor_run;
		m_run7 = cfg.item[6].motor_run;
		m_run8 = cfg.item[7].motor_run;
		m_run9 = cfg.item[8].motor_run;
		m_run10 = cfg.item[9].motor_run;
		m_run11 = cfg.item[10].motor_run;
		m_run12 = cfg.item[11].motor_run;

		m_standby1 = cfg.item[0].motor_standby;
		m_standby2 = cfg.item[1].motor_standby;
		m_standby3 = cfg.item[2].motor_standby;
		m_standby4 = cfg.item[3].motor_standby;
		m_standby5 = cfg.item[4].motor_standby;
		m_standby6 = cfg.item[5].motor_standby;
		m_standby7 = cfg.item[6].motor_standby;
		m_standby8 = cfg.item[7].motor_standby;
		m_standby9 = cfg.item[8].motor_standby;
		m_standby10 = cfg.item[9].motor_standby;
		m_standby11 = cfg.item[10].motor_standby;
		m_standby12 = cfg.item[11].motor_standby;

#if (_LC_V >=2240)
		m_cbxDirect1.SetCurSel(cfg.item[0].motor_direct);
		m_cbxDirect2.SetCurSel(cfg.item[1].motor_direct);
		m_cbxDirect3.SetCurSel(cfg.item[2].motor_direct);
		m_cbxDirect4.SetCurSel(cfg.item[3].motor_direct);
		m_cbxDirect5.SetCurSel(cfg.item[4].motor_direct);
		m_cbxDirect6.SetCurSel(cfg.item[5].motor_direct);
		m_cbxDirect7.SetCurSel(cfg.item[6].motor_direct);
		m_cbxDirect8.SetCurSel(cfg.item[7].motor_direct);
		m_cbxDirect9.SetCurSel(cfg.item[8].motor_direct);
		m_cbxDirect10.SetCurSel(cfg.item[9].motor_direct);
		m_cbxDirect11.SetCurSel(cfg.item[10].motor_direct);
		m_cbxDirect12.SetCurSel(cfg.item[11].motor_direct);
#endif

		UpdateData(FALSE);

	}
}


void CMotorConfPage::OnBnClickedButton30()
{
	// TODO:  在此添加控件通知处理程序代码
	if (IDOK == MessageBox(_T("是否写入电机驱动配置?"), _T("提示"), MB_OKCANCEL | MB_ICONWARNING))
	{
		bool bSuccess = false;
		MOTOR_CONF_CONTEXT cfg = { 0 };

		UpdateData();
		cfg.item[0].motor_combo = m_com1.GetCurSel();
		cfg.item[1].motor_combo = m_com2.GetCurSel();
		cfg.item[2].motor_combo = m_com3.GetCurSel();
		cfg.item[3].motor_combo = m_com4.GetCurSel();
		cfg.item[4].motor_combo = m_com5.GetCurSel();
		cfg.item[5].motor_combo = m_com6.GetCurSel();
		cfg.item[6].motor_combo = m_com7.GetCurSel();
		cfg.item[7].motor_combo = m_com8.GetCurSel();
		cfg.item[8].motor_combo = m_com9.GetCurSel();
		cfg.item[9].motor_combo = m_com10.GetCurSel();
		cfg.item[10].motor_combo = m_com11.GetCurSel();
		cfg.item[11].motor_combo = m_com12.GetCurSel();

		cfg.item[0].motor_run = m_run1;
		cfg.item[1].motor_run = m_run2;
		cfg.item[2].motor_run = m_run3;
		cfg.item[3].motor_run = m_run4;
		cfg.item[4].motor_run = m_run5;
		cfg.item[5].motor_run = m_run6;
		cfg.item[6].motor_run = m_run7;
		cfg.item[7].motor_run = m_run8;
		cfg.item[8].motor_run = m_run9;
		cfg.item[9].motor_run = m_run10;
		cfg.item[10].motor_run = m_run11;
		cfg.item[11].motor_run = m_run12;

		cfg.item[0].motor_standby = m_standby1;
		cfg.item[1].motor_standby = m_standby2;
		cfg.item[2].motor_standby = m_standby3;
		cfg.item[3].motor_standby = m_standby4;
		cfg.item[4].motor_standby = m_standby5;
		cfg.item[5].motor_standby = m_standby6;
		cfg.item[6].motor_standby = m_standby7;
		cfg.item[7].motor_standby = m_standby8;
		cfg.item[8].motor_standby = m_standby9;
		cfg.item[9].motor_standby = m_standby10;
		cfg.item[10].motor_standby = m_standby11;
		cfg.item[11].motor_standby = m_standby12;

#if (_LC_V >=2240)
		cfg.item[0].motor_direct = m_cbxDirect1.GetCurSel();
		cfg.item[1].motor_direct = m_cbxDirect2.GetCurSel();
		cfg.item[2].motor_direct = m_cbxDirect3.GetCurSel();
		cfg.item[3].motor_direct = m_cbxDirect4.GetCurSel();
		cfg.item[4].motor_direct = m_cbxDirect5.GetCurSel();
		cfg.item[5].motor_direct = m_cbxDirect6.GetCurSel();
		cfg.item[6].motor_direct = m_cbxDirect7.GetCurSel();
		cfg.item[7].motor_direct = m_cbxDirect8.GetCurSel();
		cfg.item[8].motor_direct = m_cbxDirect9.GetCurSel();
		cfg.item[9].motor_direct = m_cbxDirect10.GetCurSel();
		cfg.item[10].motor_direct = m_cbxDirect11.GetCurSel();
		cfg.item[11].motor_direct = m_cbxDirect12.GetCurSel();
#endif

		WriteMotorConf(cfg, bSuccess);
		MessageBox((bSuccess ? _T("写入电机驱动配置成功!") : _T("写入电机驱动配置失败!")), _T("提示"), MB_OK | (bSuccess ? MB_ICONINFORMATION : MB_ICONWARNING));
	}
	else
	{

	}
}


void CMotorConfPage::OnBnClickedButton42()
{
	// TODO:  在此添加控件通知处理程序代码
	if (MessageBox(_T("恢复电机配置初始值？"), _T("提示"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		bool ret = ResetMotorConf();
		MessageBox((ret ? _T("恢复电机配置初始值成功!") : _T("恢复电机配置初始值失败!")), _T("提示"), MB_OK | (ret ? MB_ICONINFORMATION : MB_ICONWARNING));
		if (ret)
		{
			OnBnClickedButton2();
		}
	}
}

void CMotorConfPage::AddStringToComBo(CComboBox& com)
{
	com.InsertString(0, L"256");
	com.InsertString(1, L"128");
	com.InsertString(2, L"64");
	com.InsertString(3, L"32");
	com.InsertString(4, L"16");
	com.InsertString(5, L"8");
	com.InsertString(6, L"4");
	com.InsertString(7, L"2");
	com.InsertString(8, L"1");

}

void CMotorConfPage::InitDirectComboxCtrl()
{
	m_cbxDirect1.InsertString(0, L"正向");
	m_cbxDirect1.InsertString(1, L"反向");
	m_cbxDirect2.InsertString(0, L"正向");
	m_cbxDirect2.InsertString(1, L"反向");
	m_cbxDirect3.InsertString(0, L"正向");
	m_cbxDirect3.InsertString(1, L"反向");
	m_cbxDirect4.InsertString(0, L"正向");
	m_cbxDirect4.InsertString(1, L"反向");
	m_cbxDirect5.InsertString(0, L"正向");
	m_cbxDirect5.InsertString(1, L"反向");
	m_cbxDirect6.InsertString(0, L"正向");
	m_cbxDirect6.InsertString(1, L"反向");
	m_cbxDirect7.InsertString(0, L"正向");
	m_cbxDirect7.InsertString(1, L"反向");
	m_cbxDirect8.InsertString(0, L"正向");
	m_cbxDirect8.InsertString(1, L"反向");
	m_cbxDirect9.InsertString(0, L"正向");
	m_cbxDirect9.InsertString(1, L"反向");
	m_cbxDirect10.InsertString(0, L"正向");
	m_cbxDirect10.InsertString(1, L"反向");
	m_cbxDirect11.InsertString(0, L"正向");
	m_cbxDirect11.InsertString(1, L"反向");
	m_cbxDirect12.InsertString(0, L"正向");
	m_cbxDirect12.InsertString(1, L"反向");
#if (_LC_V < 2240)
	m_cbxDirect1.ShowWindow(SW_HIDE);
	m_cbxDirect2.ShowWindow(SW_HIDE);
	m_cbxDirect3.ShowWindow(SW_HIDE);
	m_cbxDirect4.ShowWindow(SW_HIDE);
	m_cbxDirect5.ShowWindow(SW_HIDE);
	m_cbxDirect6.ShowWindow(SW_HIDE);
	m_cbxDirect7.ShowWindow(SW_HIDE);
	m_cbxDirect8.ShowWindow(SW_HIDE);
	m_cbxDirect9.ShowWindow(SW_HIDE);
	m_cbxDirect10.ShowWindow(SW_HIDE);
	m_cbxDirect11.ShowWindow(SW_HIDE);
	m_cbxDirect12.ShowWindow(SW_HIDE);

	CWnd * pWnd = NULL;
	pWnd = GetDlgItem(IDC_STATIC49); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC50); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC51); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC52); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC53); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC54); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC55); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC56); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC57); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC58); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC59); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_STATIC60); if (pWnd) pWnd->ShowWindow(SW_HIDE);
	/*GetDlgItem(IDC_STATIC49)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC50)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC51)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC52)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC53)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC54)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC55)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC56)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC57)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC58)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC59)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC60)->ShowWindow(SW_HIDE);*/
#endif
		
}

void CMotorConfPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
#ifdef DEBUG
	return;
#endif // DEBUG
	if (bShow)
		OnBnClickedButton2();
	// TODO:  在此处添加消息处理程序代码
}
