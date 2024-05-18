// ChoiceQcPri.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "ChoiceQcPri.h"
#include "afxdialogex.h"


// CChoiceQcPri 对话框

IMPLEMENT_DYNAMIC(CChoiceQcPri, CDialogEx)

CChoiceQcPri::CChoiceQcPri(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChoiceQcPri::IDD, pParent)
{

}

CChoiceQcPri::~CChoiceQcPri()
{
}

void CChoiceQcPri::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChoiceQcPri, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CChoiceQcPri::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CChoiceQcPri::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CChoiceQcPri::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON13, &CChoiceQcPri::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON30, &CChoiceQcPri::OnBnClickedButton30)
END_MESSAGE_MAP()


// CChoiceQcPri 消息处理程序


void CChoiceQcPri::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	m_priType = 1;
	OnCancel();
}


void CChoiceQcPri::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	m_priType = 2;
	OnCancel();
}


void CChoiceQcPri::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	m_priType = 3;
	OnCancel();
}


void CChoiceQcPri::OnBnClickedButton13()
{
	// TODO:  在此添加控件通知处理程序代码
	m_priType = 4;
	OnCancel();
}


void CChoiceQcPri::OnBnClickedButton30()
{
	// TODO:  在此添加控件通知处理程序代码
	m_priType = 5;
	OnCancel();
}


BOOL CChoiceQcPri::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(L"打印类型");
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
