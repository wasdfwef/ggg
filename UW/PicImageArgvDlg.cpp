// PicImageArgvDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PicImageArgvDlg.h"
#include "afxdialogex.h"
#include "UW.h"
#include "..\..\..\inc\CConfigSet.h"

// CPicImageArgvDlg 对话框

IMPLEMENT_DYNAMIC(CPicImageArgvDlg, CPropertyPage)

CPicImageArgvDlg::CPicImageArgvDlg()
	: CPropertyPage(CPicImageArgvDlg::IDD)
	, m_picImageDelay(0)
	, m_picImageDelayAutoFocus(0)
	, m_FocusStep(0)
	, m_FocusEnd(0)
	, m_cleanH(0)
	, m_cleanM(0)
	, m_cleanNum(0)
	, m_AutoFocusResultOpt(0)
	, m_shutdownClean(FALSE)
	, m_noRecCha(0)
	, m_noRecImage(0)
	, m_noRecL(0)
	, m_noRecT(0)
	, m_noRecR(0)
	, m_noRecB(0)
	, m_noRecOn(FALSE)
	, m_picRow(0)
	, m_picColumn(0)
	, m_ontimeClean(FALSE)
	, m_autoChangePrit(FALSE)
	, m_nHSpeed(0)
{

}

CPicImageArgvDlg::~CPicImageArgvDlg()
{
}

void CPicImageArgvDlg::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PIC_IMAGE_DELAY, m_picImageDelay);
    DDX_Text(pDX, IDC_EDIT_PIC_IMAGE_DELAY2, m_picImageDelayAutoFocus);
    DDX_Text(pDX, IDC_EDIT4, m_FocusStep);
    DDX_Text(pDX, IDC_EDIT5, m_FocusEnd);
    DDX_Text(pDX, IDC_EDIT1, m_cleanH);
    DDX_Text(pDX, IDC_EDIT2, m_cleanM);
    DDX_Text(pDX, IDC_EDIT3, m_cleanNum);
    DDX_Check(pDX, IDC_CHANNELD_BTN, m_shutdownClean);
    DDX_CBIndex(pDX, IDC_COMBO1, m_noRecCha);
    DDX_Text(pDX, IDC_EDIT6, m_noRecImage);
    DDX_Text(pDX, IDC_EDIT7, m_noRecL);
    DDX_Text(pDX, IDC_EDIT42, m_noRecT);
    DDX_Text(pDX, IDC_EDIT8, m_noRecR);
    DDX_Text(pDX, IDC_EDIT43, m_noRecB);
    DDX_Check(pDX, IDC_CHECK1, m_noRecOn);
    DDX_Text(pDX, IDC_EDIT36, m_AutoFocusResultOpt);
    DDX_Text(pDX, IDC_EDIT37, m_picRow);
    DDV_MinMaxInt(pDX, m_picRow, 0, 6);
    DDX_Text(pDX, IDC_EDIT38, m_picColumn);
    DDV_MinMaxInt(pDX, m_picColumn, 0, 26);
    DDX_Check(pDX, IDC_CHANNELD_BTN2, m_ontimeClean);
    DDX_Check(pDX, IDC_CHECK2, m_autoChangePrit);
    DDX_Control(pDX, IDC_CHECK3, m_checkAutoLight);
    DDX_Control(pDX, IDC_CHK_HSPEED, m_chkHSpeed);
    DDX_Text(pDX, IDC_EDIT_HSPEED, m_nHSpeed);
    DDV_MinMaxInt(pDX, m_nHSpeed, 0, 100000);

    DDX_Text(pDX, IDC_EDIT56, m_clean2Internel);
    DDX_Text(pDX, IDC_EDIT9, m_clean2StrongCleanTimes);
    DDX_Text(pDX, IDC_EDIT11, m_clean2DailyCleanTimes);
    DDX_Check(pDX, IDC_CHANNELD_BTN3, m_ontimeClean2);
    DDX_Control(pDX, IDC_RADIO1, m_radio1);
    DDX_Control(pDX, IDC_RADIO2, m_radio2);
    DDX_Control(pDX, IDC_CHECK_CLEAN1, m_clean2Strong);
    DDX_Control(pDX, IDC_CHECK_CLEAN2, m_clean2Daily);
    DDX_Control(pDX, IDC_LIST1, m_itemList);
    DDX_Control(pDX, IDC_EDIT_RElIABLILITY, m_editReliability);
}


BEGIN_MESSAGE_MAP(CPicImageArgvDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CPicImageArgvDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON13, &CPicImageArgvDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CPicImageArgvDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON16, &CPicImageArgvDlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_CHECK1, &CPicImageArgvDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON17, &CPicImageArgvDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON2, &CPicImageArgvDlg::OnBnClickedButton2)
    ON_NOTIFY(NM_CLICK, IDC_LIST1, &CPicImageArgvDlg::OnNMDblclkItemList)
    ON_EN_KILLFOCUS(IDC_EDIT_RElIABLILITY, &CPicImageArgvDlg::OnEnKillfocusEdit1Reliability)
    ON_NOTIFY(LVN_BEGINSCROLL, IDC_LIST1, &CPicImageArgvDlg::OnLvnBeginScrollList)
END_MESSAGE_MAP()


// CPicImageArgvDlg 消息处理程序


BOOL CPicImageArgvDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char focusPath[MAX_PATH] = {0};
	char confPath[MAX_PATH] = { 0 };
	char settingPath[MAX_PATH] = { 0 };
	char buffer[MAX_PATH] = { 0 };

	GetFocusPath(focusPath);
	GetConfPath(confPath);
	GetConfSettingPath(settingPath);

	

	SetWindowText(_T("上位机参数设置"));
	

	GetPrivateProfileStringA(("Focus"), ("CapturePicDelayMsAutoFocus"), "404", buffer, sizeof(buffer), focusPath);
	m_picImageDelayAutoFocus = atoi(buffer);

	GetPrivateProfileStringA(("Focus"), ("CapturePicDelayMs"), "404", buffer, sizeof(buffer), focusPath);
	m_picImageDelay = atoi(buffer);

	GetPrivateProfileStringA(("Focus"), ("CaptureImageRow"), "404", buffer, sizeof(buffer), focusPath);
	m_picRow = atoi(buffer);
	GetPrivateProfileStringA(("Focus"), ("CaptureImageColumn"), "404", buffer, sizeof(buffer), focusPath);
	m_picColumn = atoi(buffer);

	GetPrivateProfileStringA(("Focus"), ("step"), "404", buffer, sizeof(buffer), focusPath);
	m_FocusStep = atoi(buffer);

	GetPrivateProfileStringA(("Focus"), ("end"), "404", buffer, sizeof(buffer), focusPath);
	m_FocusEnd = atoi(buffer);

	GetPrivateProfileStringA(("Focus"), ("AutoFocusResultOpt"), "404", buffer, sizeof(buffer), focusPath);//自动聚焦修订
	m_AutoFocusResultOpt = atoi(buffer);

#if (_LC_V > 2190)
	BOOL isHSpeed = (BOOL)GetPrivateProfileIntA(("Focus"), ("HSpeed"), 0, focusPath);//高速采图
	int  nHSpeedValue = GetPrivateProfileIntA(("Focus"), ("HSpeedValue"), 1300, focusPath);//高速采图速度
	m_nHSpeed = nHSpeedValue;
	m_chkHSpeed.SetCheck(isHSpeed);
	m_chkHSpeed.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_LAB_HSPEED)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_HSPEED)->ShowWindow(SW_SHOW);
#endif

	GetPrivateProfileStringA(("ForceCleanTime"), ("on"), "404", buffer, sizeof(buffer), settingPath);
	m_ontimeClean = atoi(buffer);
	GetPrivateProfileStringA(("ForceCleanTime"), ("Hour"), "404", buffer, sizeof(buffer), settingPath);
	m_cleanH = atoi(buffer);
	GetPrivateProfileStringA(("ForceCleanTime"), ("Minute"), "404", buffer, sizeof(buffer), settingPath);
	m_cleanM = atoi(buffer);
	UINT nCleanType1 = GetPrivateProfileIntA(("ForceCleanTime"), ("cleanType"), 0, settingPath);
	if (nCleanType1 == 0)
	{
		m_radio1.SetCheck(1);
	}
	else
	{
		m_radio2.SetCheck(1);
	}

    // 定时清洗2
    {
        GetPrivateProfileStringA(("ForceCleanTime"), ("clean2_on"), "0", buffer, sizeof(buffer), settingPath);
        m_ontimeClean2 = atoi(buffer);
        GetPrivateProfileStringA(("ForceCleanTime"), ("clean2_internel"), "60", buffer, sizeof(buffer), settingPath);
        m_clean2Internel = atoi(buffer);
        GetPrivateProfileStringA(("ForceCleanTime"), ("clean2_strongCleanTimes"), "1", buffer, sizeof(buffer), settingPath);
        m_clean2StrongCleanTimes = atoi(buffer);
        GetPrivateProfileStringA(("ForceCleanTime"), ("clean2_dailyCleanTimes"), "1", buffer, sizeof(buffer), settingPath);
        m_clean2DailyCleanTimes = atoi(buffer);
        UINT nCleanType2 = GetPrivateProfileIntA(("ForceCleanTime"), ("clean2_cleanType"), 0, settingPath);
        m_clean2Strong.SetCheck(nCleanType2 == 1 || nCleanType2 == 3);
        m_clean2Daily.SetCheck(nCleanType2 == 2 || nCleanType2 == 3);
    }

	GetPrivateProfileStringA(("ForceCleanCount"), ("OneCleanTaskNum"), "404", buffer, sizeof(buffer), settingPath);
	m_cleanNum = atoi(buffer);

	GetPrivateProfileStringA(("autochangeprit"), ("on"), "404", buffer, sizeof(buffer), settingPath);
	m_autoChangePrit = atoi(buffer);

	GetPrivateProfileStringA(("UCShutdown"), ("Clean"), "404", buffer, sizeof(buffer), confPath);
	m_shutdownClean = atoi(buffer);	

	/*GetPrivateProfileStringA("Score", "HScore", "404", buffer, sizeof(buffer), confPath);
	m_h   = atoi(buffer);
	GetPrivateProfileStringA("Score", "BScore", "404", buffer, sizeof(buffer), confPath);
	m_b   = atoi(buffer);
	GetPrivateProfileStringA("Score", "SPScore", "404", buffer, sizeof(buffer), confPath);
	m_sp  = atoi(buffer);
	GetPrivateProfileStringA("Score", "XBTScore", "404", buffer, sizeof(buffer), confPath);
	m_xbt = atoi(buffer);
	GetPrivateProfileStringA("Score", "YHScore", "404", buffer, sizeof(buffer), confPath);
	m_yh  = atoi(buffer);
	GetPrivateProfileStringA("Score", "KLScore", "404", buffer, sizeof(buffer), confPath);
	m_kl  = atoi(buffer);
	GetPrivateProfileStringA("Score", "TMScore", "404", buffer, sizeof(buffer), confPath);
	m_tm  = atoi(buffer);
	GetPrivateProfileStringA("Score", "TSScore", "404", buffer, sizeof(buffer), confPath);
	m_ts  = atoi(buffer);
	GetPrivateProfileStringA("Score", "XYScore", "404", buffer, sizeof(buffer), confPath);
	m_xy  = atoi(buffer);
	GetPrivateProfileStringA("Score", "NSScore", "404", buffer, sizeof(buffer), confPath);
	m_ns  = atoi(buffer);
	GetPrivateProfileStringA("Score", "LSScore", "404", buffer, sizeof(buffer), confPath);
	m_ls  = atoi(buffer);
	GetPrivateProfileStringA("Score", "CSScore", "404", buffer, sizeof(buffer), confPath);
	m_cs  = atoi(buffer);
	GetPrivateProfileStringA("Score", "MJScore", "404", buffer, sizeof(buffer), confPath);
	m_mj  = atoi(buffer);
	GetPrivateProfileStringA("Score", "CSJScore", "404", buffer, sizeof(buffer), confPath);
	m_csj = atoi(buffer);
	GetPrivateProfileStringA("Score", "JZScore", "404", buffer, sizeof(buffer), confPath);
	m_jz = atoi(buffer);
	GetPrivateProfileStringA("Score", "NYScore", "404", buffer, sizeof(buffer), confPath);
	m_nys= atoi(buffer);
	GetPrivateProfileStringA("Score", "GJScore", "404", buffer, sizeof(buffer), confPath);
	m_gj = atoi(buffer);*/

    DWORD dwStyle = m_itemList.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT; //选中某行使整行高亮（只适用与report风格的listctrl）
    dwStyle |= LVS_EX_GRIDLINES; //网格线（只适用与report风格的listctrl）
    dwStyle |= LVS_EX_ONECLICKACTIVATE;
    dwStyle |= LVS_EX_SUBITEMIMAGES;
    m_itemList.SetExtendedStyle(dwStyle);
    static CFont listFont;
    listFont.CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("MS Shell Dlg"));
    m_editReliability.SetFont(&listFont);
    m_itemList.SetFont(&listFont);

    m_itemList.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 50);
    m_itemList.InsertColumn(1, _T("细胞名称"), LVCFMT_LEFT, 158);
    m_itemList.InsertColumn(2, _T("可信度"), LVCFMT_LEFT, 158);
    USES_CONVERSION;
    for (int i = 0; i < theApp.m_usInfo.nUsCounts; i++)
    {
        CString strTmp;
        strTmp.Format(_T("%d"), i+1);
        m_itemList.InsertItem(i, strTmp);
        m_itemList.SetItemText(i, 1, A2T(theApp.m_usInfo.par[i].sLongName));
        strTmp.Format(_T("%d"), theApp.m_usInfo.par[i].m_reliability);
        m_itemList.SetItemText(i, 2, strTmp);
    }

	
	((CButton*)(GetDlgItem(IDC_CHK_MERGEOHR)))->SetCheck(GetPrivateProfileIntA("th_us", "lowerThanSocreMergeToOtherClass", 0, confPath));
	((CButton*)(GetDlgItem(IDC_CHK_MERGERBC)))->SetCheck(GetPrivateProfileIntA("th_us", "mergeRbcDrbc", 0, confPath));

	GetPrivateProfileStringA("noRecPos", "on", "404", buffer, sizeof(buffer), confPath);
	m_noRecOn = atoi(buffer);
	if (m_noRecOn != TRUE)
	{
		(CEdit*)GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT42)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT43)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT8)->EnableWindow(FALSE);
	}
	else
	{
		(CEdit*)GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT42)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT43)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT6)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(L"A");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(L"B");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(L"C");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(L"D");


	GetPrivateProfileStringA("noRecPos", "Cha", "404", buffer, sizeof(buffer), confPath);
	m_noRecCha = atoi(buffer);
	GetPrivateProfileStringA("noRecPos", "image", "404", buffer, sizeof(buffer), confPath);
	m_noRecImage= atoi(buffer);	
	GetPrivateProfileStringA("noRecPos", "left", "404", buffer, sizeof(buffer), confPath);
	m_noRecL = atoi(buffer);
	GetPrivateProfileStringA("noRecPos", "top", "404", buffer, sizeof(buffer), confPath);
	m_noRecT = atoi(buffer);
	GetPrivateProfileStringA("noRecPos", "right", "404", buffer, sizeof(buffer), confPath);
	m_noRecR = atoi(buffer);
	GetPrivateProfileStringA("noRecPos", "bottom", "404", buffer, sizeof(buffer), confPath);
	m_noRecB = atoi(buffer);



/*	m_picImageDelay = atoi(picImageDelayChar);*/
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CPicImageArgvDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	char focusPath[MAX_PATH] = { 0 };
	GetFocusPath(focusPath);
	CString strValue;

	strValue.Format(_T("%d"), m_picImageDelayAutoFocus);	
	if (TRUE != WritePrivateProfileString(_T("Focus"), _T("CapturePicDelayMsAutoFocus"), strValue, StringToWstring(focusPath).c_str()))
	{
		goto WriteFailed;
	}

	strValue.Format(_T("%d"), m_picImageDelay);
	if(TRUE != WritePrivateProfileString(_T("Focus"), _T("CapturePicDelayMs"), strValue, StringToWstring(focusPath).c_str()))
		goto WriteFailed;

	strValue.Format(_T("%d"), m_FocusStep);
	if (TRUE != WritePrivateProfileString(_T("Focus"), _T("step"), strValue, StringToWstring(focusPath).c_str()))
		goto WriteFailed;

	strValue.Format(_T("%d"), m_FocusEnd);
	if (TRUE != WritePrivateProfileString(_T("Focus"), _T("end"), strValue, StringToWstring(focusPath).c_str()))
		goto WriteFailed;

	strValue.Format(_T("%d"), m_AutoFocusResultOpt);
	if (TRUE != WritePrivateProfileString(_T("Focus"), _T("AutoFocusResultOpt"), strValue, StringToWstring(focusPath).c_str()))
		goto WriteFailed;

	strValue.Format(_T("%d"), m_picRow);
	if (TRUE != WritePrivateProfileString(_T("Focus"), _T("CaptureImageRow"), strValue, StringToWstring(focusPath).c_str()))
		goto WriteFailed;

	strValue.Format(_T("%d"), m_picColumn);
	if (TRUE != WritePrivateProfileString(_T("Focus"), _T("CaptureImageColumn"), strValue, StringToWstring(focusPath).c_str()))
		goto WriteFailed;

#if (_LC_V > 2190)
	strValue.Format(_T("%d"), m_chkHSpeed.GetCheck());
	if (TRUE != WritePrivateProfileString(_T("Focus"), _T("HSpeed"), strValue, StringToWstring(focusPath).c_str()))
		goto WriteFailed;
	strValue.Format(_T("%d"), m_nHSpeed);
	if (TRUE != WritePrivateProfileString(_T("Focus"), _T("HSpeedValue"), strValue, StringToWstring(focusPath).c_str()))
		goto WriteFailed;
#endif

	MessageBox(L"参数修改成功",L"提示",MB_OK);
	return;

WriteFailed:
	MessageBox(L"参数修改失败", L"提示", MB_OK);
	return;
}

void CPicImageArgvDlg::GetFocusPath(char* focusPath)
{
	GetModuleFileNameA(NULL, focusPath, MAX_PATH);
	PathRemoveFileSpecA(focusPath);

	PathAppendA(focusPath, "Config\\Focus.ini");//聚焦配置文件2019
}

void CPicImageArgvDlg::GetConfPath(char* confPath)
{
	GetModuleFileNameA(NULL, confPath, MAX_PATH);
	PathRemoveFileSpecA(confPath);

	PathAppendA(confPath, "\\Conf.ini");//配置文件2019
}


void CPicImageArgvDlg::GetConfSettingPath(char* confPath)
{
	GetModuleFileNameA(NULL, confPath, MAX_PATH);
	PathRemoveFileSpecA(confPath);

	PathAppendA(confPath, "Config\\setting.ini");
}

void CPicImageArgvDlg::OnBnClickedButton13()
{
	char confPath[MAX_PATH] = { 0 };
	char buffer[64] = { 0 };
	LONG success = 0;
	GetConfPath(confPath);
	
    for (int i = 0; i < m_itemList.GetItemCount(); i++)
    {
        CString itemName = m_itemList.GetItemText(i, 1);
        CString txt = m_itemList.GetItemText(i, 2);
        int reliability = StrToInt(txt);
        theApp.m_usInfo.par[i].m_reliability = reliability;
        for (int i = 0; i < theApp.us_cfg_info.nUsCounts; i++)
        {
			if (!theApp.us_cfg_info.par[i].bIsDelete && itemName == theApp.us_cfg_info.par[i].sLongName)
            {
                theApp.us_cfg_info.par[i].m_reliability = reliability;
                break;
            }
        }
    }

    KSTATUS status = STATUS_SUCCESS;
    CONFIG_RECORD_INFO ConfigInfo = { 0 };
    ConfigInfo.nConfigLength = sizeof(SET_US_INFO);
    ConfigInfo.nRecordType = US_TYPE_INFO;
    ConfigInfo.pConfigConent = &theApp.us_cfg_info;

    status = SetRecordConfig(&ConfigInfo);
    if (status == STATUS_SUCCESS)
	{
		MessageBox(L"保存成功", L"提示", MB_OK);
	}
	else
	{
        MessageBox(L"保存失败", L"提示", MB_OK);
    }
	// TODO:  在此添加控件通知处理程序代码
	WritePrivateProfileStringA("th_us", "lowerThanSocreMergeToOtherClass", std::to_string(((CButton*)(GetDlgItem(IDC_CHK_MERGEOHR)))->GetCheck()).c_str(), confPath);
	WritePrivateProfileStringA("th_us", "bIsShowOther", std::to_string(((CButton*)(GetDlgItem(IDC_CHK_MERGEOHR)))->GetCheck()).c_str(), confPath);
}


void CPicImageArgvDlg::OnBnClickedButton14()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	char settingPath[MAX_PATH] = { 0 };
	char buffer[64] = { 0 };
	char confPath[MAX_PATH] = { 0 };
	GetConfSettingPath(settingPath);
	GetConfPath(confPath);

	if (WritePrivateProfileStringA(("ForceCleanTime"), ("on"), _itoa(m_ontimeClean, buffer, 10), settingPath) != TRUE)
		goto WriteFailed;
	if (WritePrivateProfileStringA(("ForceCleanTime"), ("Hour"), _itoa(m_cleanH, buffer, 10), settingPath) != TRUE)
		goto WriteFailed;
	if (WritePrivateProfileStringA(("ForceCleanTime"), ("Minute"), _itoa(m_cleanM, buffer, 10), settingPath) != TRUE)
		goto WriteFailed;
	if (WritePrivateProfileStringA(("ForceCleanCount"), ("OneCleanTaskNum"), _itoa((m_cleanNum <= 0 ? 1 : m_cleanNum), buffer, 10), settingPath) != TRUE)
		goto WriteFailed;
	if (WritePrivateProfileStringA(("ForceCleanTime"), ("cleanType"), _itoa(m_radio2.GetCheck(), buffer, 10), settingPath) != TRUE)
		goto WriteFailed;
    // 定时清洗2
    {
        if (WritePrivateProfileStringA(("ForceCleanTime"), ("clean2_on"), _itoa(m_ontimeClean2, buffer, 10), settingPath) != TRUE)
            goto WriteFailed;
        if (WritePrivateProfileStringA(("ForceCleanTime"), ("clean2_internel"), _itoa(m_clean2Internel, buffer, 10), settingPath) != TRUE)
            goto WriteFailed;
        if (WritePrivateProfileStringA(("ForceCleanTime"), ("clean2_strongCleanTimes"), _itoa(m_clean2StrongCleanTimes, buffer, 10), settingPath) != TRUE)
            goto WriteFailed;
        if (WritePrivateProfileStringA(("ForceCleanTime"), ("clean2_dailyCleanTimes"), _itoa(m_clean2DailyCleanTimes, buffer, 10), settingPath) != TRUE)
            goto WriteFailed;
        int cleanType2 = 0;
        if (m_clean2Strong.GetCheck() && m_clean2Daily.GetCheck()) cleanType2 = 3;
        else if (m_clean2Strong.GetCheck()) cleanType2 = 1;
        else if (m_clean2Daily.GetCheck()) cleanType2 = 2;
        if (WritePrivateProfileStringA(("ForceCleanTime"), ("clean2_cleanType"), _itoa(cleanType2, buffer, 10), settingPath) != TRUE)
            goto WriteFailed;

        AfxGetMainWnd()->PostMessage(WM_SETCLEANTIMER2, (WPARAM)m_ontimeClean2, (LPARAM)m_clean2Internel);
    }


	if (TRUE != WritePrivateProfileStringA(("UCShutdown"), ("Clean"), _itoa(m_shutdownClean, buffer, 10), confPath))
		goto WriteFailed;


	MessageBox(L"参数修改成功", L"提示", MB_OK);
	return;

WriteFailed:
	MessageBox(L"参数修改失败", L"提示", MB_OK);
	return;
}




void CPicImageArgvDlg::OnBnClickedButton16()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	char buffer[64] = { 0 };
	char confPath[MAX_PATH] = { 0 };

	GetConfPath(confPath);

	if (!WritePrivateProfileStringA("noRecPos", "on", _itoa(m_noRecOn, buffer, 10), confPath))
		goto WriteFailed;
	if (!WritePrivateProfileStringA("noRecPos", "Cha", _itoa(m_noRecCha, buffer, 10), confPath))
		goto WriteFailed;
	if (!WritePrivateProfileStringA("noRecPos", "image", _itoa(m_noRecImage, buffer, 10), confPath))
		goto WriteFailed;
	if (!WritePrivateProfileStringA("noRecPos", "left", _itoa(m_noRecL, buffer, 10), confPath))
		goto WriteFailed;
	if (!WritePrivateProfileStringA("noRecPos", "top", _itoa(m_noRecT, buffer, 10), confPath))
		goto WriteFailed;
	if (!WritePrivateProfileStringA("noRecPos", "right",  _itoa(m_noRecR, buffer, 10), confPath))
		goto WriteFailed;
	if (!WritePrivateProfileStringA("noRecPos", "bottom", _itoa(m_noRecB ,buffer, 10), confPath))
		goto WriteFailed;
	if (!WritePrivateProfileStringA("th_us", "mergeRbcDrbc", std::to_string(((CButton*)(GetDlgItem(IDC_CHK_MERGERBC)))->GetCheck()).c_str(), confPath))
		goto WriteFailed;

	MessageBox(L"参数修改成功", L"提示", MB_OK);
	return;

WriteFailed:
	MessageBox(L"参数修改失败", L"提示", MB_OK);
	return;
}


void CPicImageArgvDlg::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_noRecOn)
	{
		(CEdit*)GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT42)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT43)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT6)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
		(CEdit*)GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);
	}
	else
	{
		(CEdit*)GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT42)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT43)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
		(CEdit*)GetDlgItem(IDC_EDIT8)->EnableWindow(FALSE);
	}
}


void CPicImageArgvDlg::OnBnClickedButton17()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	char buffer[64] = { 0 };
	char settingPath[MAX_PATH] = { 0 };

	GetConfSettingPath(settingPath);

	if (!WritePrivateProfileStringA("autochangeprit", "on", _itoa(m_autoChangePrit, buffer, 10), settingPath))
		goto WriteFailed;

	MessageBox(L"参数修改成功", L"提示", MB_OK);
	return;

WriteFailed:
	MessageBox(L"参数修改失败", L"提示", MB_OK);
	return;
}

//自动亮度保存
void CPicImageArgvDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
}

static int m_row = 0, m_col = 0;
void CPicImageArgvDlg::OnNMDblclkItemList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
    *pResult = 0;

    CRect rc;
    if (pNMListView->iItem != -1 && pNMListView->iSubItem == 2)
    {
        m_row = pNMListView->iItem;
        m_col = pNMListView->iSubItem;
        m_itemList.GetSubItemRect(m_row, m_col, LVIR_LABEL, rc);
        rc.left += 1;
        rc.top += 1;
        rc.right -= 1;
        rc.bottom -= 1;

        CString txt = m_itemList.GetItemText(m_row, m_col);
        m_editReliability.SetParent(&m_itemList);
        m_editReliability.SetWindowText(txt);
        m_editReliability.ShowWindow(SW_SHOW);
        m_editReliability.MoveWindow(&rc);
        m_editReliability.SetFocus();
        m_editReliability.CreateSolidCaret(1, rc.Height()-5);
        m_editReliability.ShowCaret();
        m_editReliability.SetSel(-1);
    }
}

void CPicImageArgvDlg::OnEnKillfocusEdit1Reliability()
{
    CString str;
    m_editReliability.GetWindowText(str);
    m_editReliability.ShowWindow(SW_HIDE);
    if (!str.IsEmpty())
    {
        int reliability = StrToInt(str);
        if (reliability < 0)
        {
            str = _T("0");
        }
        else if (reliability > 100)
        {
            str = _T("100");
        }
        m_itemList.SetItemText(m_row, m_col, str);
    }
}


void CPicImageArgvDlg::OnLvnBeginScrollList(NMHDR *pNMHDR, LRESULT *pResult)
{
    // 此功能要求 Internet Explorer 5.5 或更高版本。
    // 符号 _WIN32_IE 必须是 >= 0x0560。
    LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
    // TODO:  在此添加控件通知处理程序代码
    *pResult = 0;

    if (m_editReliability.IsWindowVisible())
    {
        m_itemList.SetFocus();
        m_itemList.SetItemState(m_row, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
    }
}
