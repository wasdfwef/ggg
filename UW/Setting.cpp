// Setting.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "Setting.h"


// CSetting 对话框

IMPLEMENT_DYNAMIC(CSetting, CDialog)
extern 
std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId);

CSetting::CSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CSetting::IDD, pParent)
	, m_lisTimeInterval(0)
	, m_defaulTimeInterval(FALSE)
{

}

CSetting::~CSetting()
{
}

void CSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROW_NUMS_SET_EDIT, m_row_set_edit);
	DDX_Control(pDX, IDC_SN_NUMS_SET_EDIT, m_sn_nums_set_edit);
	DDX_Control(pDX, IDC_SAVE_NUMS_SET_EDIT, m_Save_Nums_Set_Edit);
	DDX_Control(pDX, IDC_AUTO_SEND_TO_LIS_CHECK, m_auto_sendto_lis_check);
	DDX_Text(pDX, IDC_EDIT_SENDTIME, m_lisTimeInterval);
	DDX_Check(pDX, IDC_CHECK2, m_defaulTimeInterval);
}


void CSetting::GetLisConfigPath(char* Lispath)
{
	char confPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, Lispath, MAX_PATH);
	PathRemoveFileSpecA(Lispath);
	PathAppendA(Lispath, "config/Lis1.ini");
}

BEGIN_MESSAGE_MAP(CSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDC_AUTO_SEND_TO_LIS_CHECK, &CSetting::OnBnClickedAutoSendToLisCheck)
	ON_BN_CLICKED(IDC_CHECK2, &CSetting::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CSetting 消息处理程序

BOOL CSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化


	// 架号
	CString strText;
	strText.Format(_T("%d"),theApp.m_nTubeShelfNum+1);
	m_row_set_edit.SetWindowText(strText);
	// 顺序号
	strText.Format(_T("%d"),theApp.m_nSN);
	m_sn_nums_set_edit.SetWindowText(strText);

	strText.Format(_T("%d"),theApp.ReadSaveNums());
	m_Save_Nums_Set_Edit.SetWindowText(strText);

	// 设置自动发送到LIS
	m_auto_sendto_lis_check.SetCheck( theApp.ReadSendLisSign() );

	SetWindowText(GetIdsString(_T("setting"),_T("title")).c_str());


	m_lisTimeInterval = theApp.m_sendLisTimeInterval;
	if (m_lisTimeInterval == 1000)
	{
		m_defaulTimeInterval = TRUE;
		GetDlgItem(IDC_EDIT_SENDTIME)->EnableWindow(FALSE);
	}
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSetting::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strRowText;
	CString strSNText;
	CString strSaveTaskCount;
	CString Warning;
	m_row_set_edit.GetWindowText(strRowText);
	m_sn_nums_set_edit.GetWindowText(strSNText);
	m_Save_Nums_Set_Edit.GetWindowText(strSaveTaskCount);


	for (int i = 0; i < strRowText.GetLength(); ++i)
	{
		if (0 == iswdigit(strRowText[i]))
		{
			Warning = GetIdsString(_T("warning"),_T("rowerror")).c_str();
			MessageBox(Warning, _T("UW"), MB_OK);
			return;
		}
	}

	for (int i = 0; i < strSNText.GetLength(); ++i)
	{
		if (0 == iswdigit(strSNText[i]))
		{
			Warning = GetIdsString(_T("warning"),_T("snerror")).c_str();
			MessageBox(Warning, _T("UW"), MB_OK);
			return;
		}
	}

	for (int i = 0; i < strSaveTaskCount.GetLength(); ++i)
	{
		if (0 == iswdigit(strSaveTaskCount[i]))
		{
			Warning = GetIdsString(_T("warning"),_T("taskcounterror")).c_str();
			MessageBox(Warning, _T("UW"), MB_OK);
			return;
		}
	}

	if (_tstoi(strRowText) <= 0 || _tstoi(strRowText) > MAX_ROW)
	{
		Warning = GetIdsString(_T("warning"),_T("rowerror")).c_str();
		MessageBox(Warning, _T("UW"), MB_OK);
		return;
	}

	if (_tstoi(strSNText) <= 0 || _tstoi(strSNText) > MAX_SN)
	{
		Warning = GetIdsString(_T("warning"),_T("snerror")).c_str();
		MessageBox(Warning, _T("UW"), MB_OK);
		return;
	}

// 	if (_tstoi(strSaveTaskCount) <= 0 || _tstoi(strSaveTaskCount) > 10000)
// 	{
// 		Warning = GetIdsString(_T("warning"),_T("taskcounterror")).c_str();
// 		MessageBox(Warning, _T("UW"), MB_OK);
// 		return;
// 	}

// 	theApp.WriteRowNums(_tstoi(strRowText));
 	theApp.m_nTubeShelfNum = _tstoi(strRowText);
	theApp.m_nTubeShelfNum--;
// 
// 	theApp.WriteSNNums(_tstoi(strSNText));
 	theApp.m_nSN = _tstoi(strSNText);

	theApp.WriteSaveNums(_tstoi(strSaveTaskCount));
	theApp.m_SaveCount = theApp.ReadSaveNums();

	theApp.m_sendLisTimeInterval = m_lisTimeInterval;

	char lisPath[MAX_PATH] = { 0 };
	char buffer[256] = { 0 };
	GetLisConfigPath(lisPath);
	WritePrivateProfileStringA("config", "timeinterval", itoa(m_lisTimeInterval,buffer,10), lisPath);
	theApp.WriteSendLisSign(m_auto_sendto_lis_check.GetCheck());
	OnOK();
}

void CSetting::OnBnClickedAutoSendToLisCheck()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CSetting::OnBnClickedCheck2()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_defaulTimeInterval)
	{
		(CEdit*)GetDlgItem(IDC_EDIT_SENDTIME)->EnableWindow(false);
		m_lisTimeInterval = 1000;
	}
	else
	{
		(CEdit*)GetDlgItem(IDC_EDIT_SENDTIME)->EnableWindow(true);
	}
	UpdateData(FALSE);
}
