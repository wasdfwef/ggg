// SoftwareSetUI.cpp : 实现文件
//

#include "stdafx.h"
#include "SoftwareSetUI.h"
#include "afxdialogex.h"
#include "MainFrm.h"

#define MAX_COLUMN 12

using namespace std;
// CSoftwareSetUI 对话框

IMPLEMENT_DYNAMIC(CSoftwareSetUI, CPropertyPage)

CSoftwareSetUI::CSoftwareSetUI()
: CPropertyPage(CSoftwareSetUI::IDD)
{

}

CSoftwareSetUI::~CSoftwareSetUI()
{
}

void CSoftwareSetUI::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSoftwareSetUI, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CSoftwareSetUI::OnBnClickedButton1)
END_MESSAGE_MAP()


// CSoftwareSetUI 消息处理程序


BOOL CSoftwareSetUI::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char iniUIPath[MAX_PATH] = { 0 };
	char buff[64] = { 0 };
	char isShow[64] = { 0 };
	CRect wndRect;
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->m_wndTodayList.GetWindowRect(wndRect);
	GetModuleFileNameA(NULL, iniUIPath, MAX_PATH);
	PathRemoveFileSpecA(iniUIPath);
	PathAppendA(iniUIPath, "Config\\UI.ini");

	
	for (int i = 0; i <= MAX_COLUMN; i++)
	{
 		GetPrivateProfileStringA("todaylist", itoa(i + 100, buff, 10), "1", isShow, 64, iniUIPath);
		if (atoi(isShow))
			((CButton*)GetDlgItem(IDC_CHECK13+i))->SetCheck(BST_CHECKED);
		else
			((CButton*)GetDlgItem(IDC_CHECK13+i))->SetCheck(BST_UNCHECKED);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CSoftwareSetUI::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类

	CPropertyPage::OnCancel();
}


void CSoftwareSetUI::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	char iniUIPath[MAX_PATH] = { 0 };
	char buff[64] = { 0 };
	char isShow[64] = { 0 };
	CRect wndRect;
	int widthTotal = 0;
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->m_wndTodayList.GetWindowRect(wndRect);
	GetModuleFileNameA(NULL, iniUIPath, MAX_PATH);
	PathRemoveFileSpecA(iniUIPath);
	PathAppendA(iniUIPath, "Config\\UI.ini");
	for (int i = 0; i <= MAX_COLUMN; i++)
	{
		if (((CButton*)GetDlgItem(IDC_CHECK13 + i))->GetCheck())
		{
			int width = GetPrivateProfileIntA("todaylist", itoa(i, buff, 10), 100, iniUIPath);
			pMain->m_wndTodayList.m_List.SetColumnWidth(i, width);
			widthTotal += width;
			if (TRUE != WritePrivateProfileStringA("todaylist", itoa(i + 100, buff, 10), "1", iniUIPath))
				goto WriteFailed;
		}	
		else
		{
			pMain->m_wndTodayList.m_List.SetColumnWidth(i, 0);
			if (TRUE != WritePrivateProfileStringA("todaylist", itoa(i + 100, buff, 10), "0", iniUIPath))
				goto WriteFailed;
		}
	}

	return;
WriteFailed:
	MessageBox(L"参数修改失败", L"提示", MB_OK);
	return;
}
