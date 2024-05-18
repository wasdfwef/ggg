// DbgPassword.cpp : 实现文件
//

#include "stdafx.h"
#include "DbgPassword.h"
#include "afxdialogex.h"


// DbgPassword 对话框

IMPLEMENT_DYNAMIC(DbgPassword, CDialogEx)

DbgPassword::DbgPassword(CWnd* pParent /*=NULL*/)
	: CDialogEx(DbgPassword::IDD, pParent)
	, m_password(0)
{

}

DbgPassword::~DbgPassword()
{
}

void DbgPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_password);
}


BEGIN_MESSAGE_MAP(DbgPassword, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &DbgPassword::OnBnClickedButton1)
END_MESSAGE_MAP()


// DbgPassword 消息处理程序


void DbgPassword::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	OnOK();
}
