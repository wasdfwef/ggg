// ListCtrlSkinDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "ListCtrlSkinDlg.h"
#include "afxdialogex.h"

using namespace std;
// CListCtrlSkinDlg 对话框

IMPLEMENT_DYNAMIC(CListCtrlSkinDlg, CDialogEx)

CListCtrlSkinDlg::CListCtrlSkinDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CListCtrlSkinDlg::IDD, pParent)
{

}

CListCtrlSkinDlg::~CListCtrlSkinDlg()
{
}

void CListCtrlSkinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CListCtrlSkinDlg, CDialogEx)
	ON_STN_CLICKED(IDC_LIST_COLOR_ODD, &CListCtrlSkinDlg::OnStnClickedListColorOdd)
	ON_STN_CLICKED(IDC_LISTSKIN_EVEN, &CListCtrlSkinDlg::OnStnClickedListskinEven)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON2, &CListCtrlSkinDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CListCtrlSkinDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CListCtrlSkinDlg 消息处理程序


BOOL CListCtrlSkinDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowTextW(L"列表自定义颜色");
	MoveWindow(m_pos.right - 250,m_pos.top, 250, 200);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CListCtrlSkinDlg::GetLastColor(COLORREF lastOdd, COLORREF lastEven)
{
	m_OddColor = lastOdd;
	m_EvenColor = lastEven;
	
// 	GetDlgItem(IDC_LISTSKIN_EVEN)->Invalidate();
// 	GetDlgItem(IDC_LISTSKIN_EVEN)->UpdateWindow();
// 	GetDlgItem(IDC_LIST_COLOR_ODD)->Invalidate();
// 	GetDlgItem(IDC_LIST_COLOR_ODD)->UpdateWindow();
	
}

void CListCtrlSkinDlg::GetParentPos(CRect buttonPos)
{
	this->m_pos = buttonPos;
}

void CListCtrlSkinDlg::OnStnClickedListColorOdd()
{
	// TODO:  在此添加控件通知处理程序代码
	CColorDialog m_setClrDlg;
	m_setClrDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;   // CC_RGBINIT可以让上次选择的颜色作为初始颜色显示出来
	m_setClrDlg.m_cc.rgbResult = m_OddColor;        //记录上次选择的颜色
	if (IDOK == m_setClrDlg.DoModal())
	{
		m_OddColor = m_setClrDlg.m_cc.rgbResult;            // 保存用户选择的颜色

	}
	GetDlgItem(IDC_LIST_COLOR_ODD)->Invalidate();
	GetDlgItem(IDC_LIST_COLOR_ODD)->UpdateWindow();
}


void CListCtrlSkinDlg::OnStnClickedListskinEven()
{
	// TODO:  在此添加控件通知处理程序代码
	CColorDialog m_setClrDlg;
	m_setClrDlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;   // CC_RGBINIT可以让上次选择的颜色作为初始颜色显示出来
	m_setClrDlg.m_cc.rgbResult = m_EvenColor;        //记录上次选择的颜色
	if (IDOK == m_setClrDlg.DoModal())
	{
		m_EvenColor = m_setClrDlg.m_cc.rgbResult;            // 保存用户选择的颜色
	}
	GetDlgItem(IDC_LISTSKIN_EVEN)->Invalidate();
	GetDlgItem(IDC_LISTSKIN_EVEN)->UpdateWindow();
}


HBRUSH CListCtrlSkinDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_LIST_COLOR_ODD)
	{
		pDC->SetBkColor(m_OddColor);
		pDC->SetBkMode(TRANSPARENT);
		HBRUSH br = CreateSolidBrush(m_OddColor);
		return br;
	}
	if (pWnd->GetDlgCtrlID() == IDC_LISTSKIN_EVEN)
	{
		pDC->SetBkColor(m_EvenColor);
		pDC->SetBkMode(TRANSPARENT);
		HBRUSH br = CreateSolidBrush(m_EvenColor);
		return br;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CListCtrlSkinDlg::OnBnClickedButton2()
{
	OnOK();
	// TODO:  在此添加控件通知处理程序代码
}


void CListCtrlSkinDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	m_OddColor = RGB(223,223,223);
	m_EvenColor = RGB(245,245,245);

	OnOK();
}
