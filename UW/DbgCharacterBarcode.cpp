// DbgCharacterBarcode.cpp : 实现文件
//

#include "stdafx.h"
#include "DbgCharacterBarcode.h"
#include "afxdialogex.h"
#include "CmdHelper.h"


// CDbgCharacterBarcode 对话框

IMPLEMENT_DYNAMIC(CDbgCharacterBarcode, CPropertyPage)

CDbgCharacterBarcode::CDbgCharacterBarcode()
	: CPropertyPage(CDbgCharacterBarcode::IDD)
{

}

CDbgCharacterBarcode::~CDbgCharacterBarcode()
{
}

void CDbgCharacterBarcode::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDbgCharacterBarcode, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CDbgCharacterBarcode::OnBnClickedLedTurnOn)
	ON_BN_CLICKED(IDC_BUTTON13, &CDbgCharacterBarcode::OnBnClickedLedTurnOff)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDbgCharacterBarcode 消息处理程序


BOOL CDbgCharacterBarcode::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDbgCharacterBarcode::OnBnClickedLedTurnOn()
{
	BarcodeLedSwitch();
}


void CDbgCharacterBarcode::OnBnClickedLedTurnOff()
{
	BarcodeLedSwitch(false);
}


void CDbgCharacterBarcode::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
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
