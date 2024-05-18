// SetDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "SetDialog.h"
#include "UdcCriterionDlg.h"
#include "UserManageDlg.h"
#include "MainFrm.h"
#include "UserPasswordDlg.h"
//#include "AdjustImg.h"
#include "MoveVisioDlg.h"
#include "..\..\..\inc\CStartComSet.h"
#include "..\..\..\inc\CStartCalibrate.h"


#pragma comment(lib, "..\\..\\..\\lib\\CStartComSet.lib")
#pragma comment(lib, "..\\..\\..\\lib\\CStartCalibrate.lib")

// CSetDialog 对话框

IMPLEMENT_DYNAMIC(CSetDialog, CDialog)
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

CSetDialog::CSetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDialog::IDD, pParent)
{

}

CSetDialog::~CSetDialog()
{	
}

void CSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SET_USERMANAGE, m_btnUserManage);
	DDX_Control(pDX, IDC_BUTTON_REMOVEALL, m_btnRemoveall);
}


BEGIN_MESSAGE_MAP(CSetDialog, CDialog)
	//	ON_BN_CLICKED(IDC_BUTTON6, &CSetDialog::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BTN_UDC, &CSetDialog::OnBnClickedBtnUdc)
	ON_BN_CLICKED(IDC_BTN_DOCTOR, &CSetDialog::OnBnClickedBtnDoctor)
	ON_BN_CLICKED(IDC_SET_USERMANAGE, &CSetDialog::OnBnClickedSetUsermanage)
	ON_BN_CLICKED(IDC_SET_MODIFYPASSWORD, &CSetDialog::OnBnClickedSetModifypassword)
	ON_BN_CLICKED(IDC_BTN_COLOR, &CSetDialog::OnBnClickedBtnColor)
	ON_BN_CLICKED(IDC_BUTTON8, &CSetDialog::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON7, &CSetDialog::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_S, &CSetDialog::OnBnClickedS)
	ON_BN_CLICKED(IDC_BUTTON11, &CSetDialog::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON9, &CSetDialog::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CSetDialog::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON12, &CSetDialog::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_SET_MODIFYPASSWORD2, &CSetDialog::OnBnClickedSetModifypassword2)
ON_BN_CLICKED(IDC_BUTTON43, &CSetDialog::OnBnClickedButton43)
END_MESSAGE_MAP()


// CSetDialog 消息处理程序

//void CSetDialog::OnBnClickedButton6()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}

// 干化标定
void CSetDialog::OnBnClickedBtnUdc()
{
	// TODO: 在此添加控件通知处理程序代码
	CUdcCriterionDlg dlg;
	dlg.DoModal();
}

// 科室医生
void CSetDialog::OnBnClickedBtnDoctor()
{
	// TODO: 在此添加控件通知处理程序代码
	
	/*LPCOLOR_INFO colorinfo[MAX_PATH];
	for (int i=0;i<nCount;++i)
	{
		GetColorInfo(sizeof(colors),nCount);
	}*/
	ShowInfoDlg();

}

// 用户管理
void CSetDialog::OnBnClickedSetUsermanage()
{
	// TODO: 在此添加控件通知处理程序代码
	CUserManageDlg udlg;
	udlg.DoModal();
}

// 修改密码
void CSetDialog::OnBnClickedSetModifypassword()
{
	// TODO: 在此添加控件通知处理程序代码
	CUserPasswordDlg pdlg;
	pdlg.DoModal();
}

BOOL CSetDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(GetIdsString(_T("setdlg"),_T("title")).c_str());
	GetDlgItem(IDC_SET_USERMANAGE)->SetWindowText(GetIdsString(_T("setdlg"),_T("1")).c_str());
	GetDlgItem(IDC_BTN_DOCTOR)->SetWindowText(GetIdsString(_T("setdlg"),_T("2")).c_str());
	GetDlgItem(IDC_BUTTON10)->SetWindowText(GetIdsString(_T("setdlg"),_T("3")).c_str());
	GetDlgItem(IDC_BTN_COLOR)->SetWindowText(GetIdsString(_T("setdlg"),_T("4")).c_str());
	GetDlgItem(IDC_SET_MODIFYPASSWORD)->SetWindowText(GetIdsString(_T("setdlg"),_T("5")).c_str());
	GetDlgItem(IDC_BUTTON12)->SetWindowText(GetIdsString(_T("setdlg"),_T("6")).c_str());
	GetDlgItem(IDC_BUTTON11)->SetWindowText(GetIdsString(_T("setdlg"),_T("7")).c_str());
	GetDlgItem(IDC_BTN_UDC)->SetWindowText(GetIdsString(_T("setdlg"),_T("8")).c_str());
	GetDlgItem(IDC_S)->SetWindowText(GetIdsString(_T("setdlg"),_T("9")).c_str());
	GetDlgItem(IDC_BUTTON9)->SetWindowText(GetIdsString(_T("setdlg"),_T("10")).c_str());
	GetDlgItem(IDC_BUTTON7)->SetWindowText(GetIdsString(_T("setdlg"),_T("11")).c_str());
	GetDlgItem(IDC_BUTTON8)->SetWindowText(GetIdsString(_T("setdlg"),_T("12")).c_str());


	// 权限控制，普通用户不允许删除所有记录
	if (theApp.loginGroup == 111|| theApp.loginGroup == 4)
	{
		//m_btnUserManage.EnableWindow(TRUE);
		m_btnRemoveall.EnableWindow(TRUE);
	}
	else
	{
		//m_btnUserManage.EnableWindow(FALSE);
		m_btnRemoveall.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 颜色
void CSetDialog::OnBnClickedBtnColor()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowSetDlg();

	//LPCOLOR_INFO colors;
	//int nCount;
	//GetColorInfo(colors,nCount);
}

// 沉渣项目
void CSetDialog::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码

	//TCHAR sModuleFile[MAX_PATH]={0};
	//TCHAR sDrive[MAX_PATH]={0};
	//TCHAR sDir[MAX_PATH]={0};

	//GetModuleFileName(NULL, sModuleFile, MAX_PATH);

	//_wsplitpath(sModuleFile, sDrive, sDir, NULL,NULL);

	////TCHAR s_inifile[MAX_PATH]={0};
	//CString s_inifile;

	//wsprintf(s_inifile.GetBuffer(),_T("%s%s%s.ini"),sDrive,sDir,_T("Conf"));

	StartUSCFGSetDlg(NULL);
}

// 沉渣标定
void CSetDialog::OnBnClickedButton7()
{
	StartCalibrate(theApp.m_b);
}

// 干化项目
void CSetDialog::OnBnClickedS()
{
	// TODO: 在此添加控件通知处理程序代码
	StartUDCCFGSetDlg(NULL);
}

// 视野
void CSetDialog::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码
	SetCollectRange();
}

// 调整图像位置
void CSetDialog::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	/*CAdjustImg adjdlg;
	adjdlg.DoModal();*/
	CMoveVisioDlg mdlg;
	mdlg.DoModal();
}

// 报警
void CSetDialog::OnBnClickedButton10()
{
	StartComSetDlg();
	
	
}

// 打印模板
void CSetDialog::OnBnClickedButton12()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPrintModuleSetDlg( );
}


void CSetDialog::OnBnClickedSetModifypassword2()
{
	// TODO:  在此添加控件通知处理程序代码
	INT_PTR ret = ((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.DoModal();
	OnOK();

}


void CSetDialog::OnBnClickedButton43()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox(L"确定删除数据库所有历史记录？", L"提示", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
	{
		BeginWaitCursor();
		ULONG       nTaskID;
		DEL_TASK    del_task;
		CUdcView   *udc_view;
		CMainFrame *pMainFrame;


		del_task.bIsQC = FALSE; //不考虑是否为质控
		del_task.pDelIDArray = NULL;
		del_task.nCounter = DELETE_ALL_TASK_ITEM;
		if (Access_DelTask(&del_task) != STATUS_SUCCESS)
			return;

		THUIInterface_ShowTaskIcon(0xffffffff, FALSE); //强制刷新目的,清除列表

		pMainFrame = (CMainFrame*)theApp.GetMainWnd();
		udc_view = (CUdcView*)pMainFrame->GetUWView()->GetTabWnd(THUM_UDC_TAB);
		udc_view->FillList();
		pMainFrame->m_wndTodayList.m_List.DeleteAllItems();
		EndWaitCursor();
	}
	else
	{

	}
}
