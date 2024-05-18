// StdUdcTest.cpp : 实现文件
//

#include "stdafx.h"
#include "StdUdcTest.h"
#include "afxdialogex.h"
#include "Include/Common/String.h"
#include <algorithm>
#include <future>
#include "math.h"
#include "DbgMsg.h"

// CStdUdcTest 对话框

IMPLEMENT_DYNAMIC(CStdUdcTest, CPropertyPage)

CStdUdcTest::CStdUdcTest()
: CPropertyPage(CStdUdcTest::IDD)
{
	listRectTop = 0;
	isCalcd = false;
}

CStdUdcTest::~CStdUdcTest()
{
	
}

void CStdUdcTest::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStdUdcTest, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON14, &CStdUdcTest::OnBnClickedButton14)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON44, &CStdUdcTest::OnBnClickedButton44)
END_MESSAGE_MAP()


// CStdUdcTest 消息处理程序


void CStdUdcTest::OnBnClickedButton14()
{
	// TODO:  在此添加控件通知处理程序代码
/*	((CListCtrl*)GetDlgItem(IDC_LIST2))->DeleteAllItems();*/
	auto f = std::async(std::launch::async, [=](){

		HWND hBtn = GetDlgItem(IDC_BUTTON14)->GetSafeHwnd();
		::EnableWindow(hBtn, FALSE);
		CString str;
		CListCtrl *list = (CListCtrl*)GetDlgItem(IDC_LIST2);

		UDC_STDPAPER_TEST_RESULT_CONTEXT ctx = { 0 };

		if (UdcStdPaperTest(ctx))
		{
			testNum++;
			const char* colorArr[] = { "绿灯", "黄绿灯", "橙灯", "红灯"};
			for (int i = 0; i < sizeof(colorArr) / sizeof(char*); i++)
			{
				UDC_COLORBLOCK_CONTEXT *p = (UDC_COLORBLOCK_CONTEXT*)&ctx + i;
				str = colorArr[i];
				list->InsertItem(0, str);
				str.Format(_T("%d"), p->ColorBlock1_LEU);
				list->SetItemText(0, 1, str);
				str.Format(_T("%d"), p->ColorBlock2_NIT);
				list->SetItemText(0, 2, str);
				str.Format(_T("%d"), p->ColorBlock3_URO);
				list->SetItemText(0, 3, str);
				str.Format(_T("%d"), p->ColorBlock4_PRO);
				list->SetItemText(0, 4, str);
				str.Format(_T("%d"), p->ColorBlock5_PH);
				list->SetItemText(0, 5, str);
				str.Format(_T("%d"), p->ColorBlock6_BLD);
				list->SetItemText(0, 6, str);
				str.Format(_T("%d"), p->ColorBlock7_SG);
				list->SetItemText(0, 7, str);
				str.Format(_T("%d"), p->ColorBlock8_KET);
				list->SetItemText(0, 8, str);
				str.Format(_T("%d"), p->ColorBlock9_BIL);
				list->SetItemText(0, 9, str);
				str.Format(_T("%d"), p->ColorBlock10_GLU);
				list->SetItemText(0, 10, str);
				str.Format(_T("%d"), p->ColorBlock11_VC);
				list->SetItemText(0, 11, str);
				str.Format(_T("%d"), p->ColorBlock12_MALB);
				list->SetItemText(0, 12, str);
				str.Format(_T("%d"), p->ColorBlock13_CRE);
				list->SetItemText(0, 13, str);
				str.Format(_T("%d"), p->ColorBlock14_CA);
				list->SetItemText(0, 14, str);
			}
		}
		else
		{
			MessageBox(_T("标准条测试失败!"), _T("提示"), MB_OK | MB_ICONERROR);
			::EnableWindow(hBtn, TRUE);
		}

		::EnableWindow(hBtn, TRUE);

		int nCount = list->GetItemCount();
		if ((nCount > 0))
			list->EnsureVisible(nCount - 1, FALSE);

	});
}


BOOL CStdUdcTest::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	SetWindowText(L"标准条测试");
	// TODO:  在此添加额外的初始化
	CListCtrl *list2 = (CListCtrl*)GetDlgItem(IDC_LIST2);
	list2->InsertColumn(0, _T("Color"), 0, 100);
	list2->InsertColumn(1, _T("LEU"), 0, 100);
	list2->InsertColumn(2, _T("NIT"), 0, 100);
	list2->InsertColumn(3, _T("URO"), 0, 100);
	list2->InsertColumn(4, _T("PRO"), 0, 100);
	list2->InsertColumn(5, _T("PH"), 0, 100);
	list2->InsertColumn(6, _T("BLD"), 0, 100);
	list2->InsertColumn(7, _T("SG"), 0, 100);
	list2->InsertColumn(8, _T("KET"), 0, 100);
	list2->InsertColumn(9, _T("BIL"), 0, 100);
	list2->InsertColumn(10, _T("GLU"), 0, 100);
	list2->InsertColumn(11, _T("VC"), 0, 100);
	list2->InsertColumn(12, _T("MA"), 0, 100);
	list2->InsertColumn(13, _T("CRE"), 0, 100);
	list2->InsertColumn(14, _T("CA"), 0, 100);


// 	CRect listRect_wnd;
// 	GetClientRect(listRect_wnd);
// 	list2->SetWindowPos(NULL, listRect_wnd.left, listRect_wnd.top, listRect_wnd.Width(),listRect_wnd.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CStdUdcTest::OnShowWindow(BOOL bShow, UINT nStatus)
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

void CStdUdcTest::CalcCvByBlock()
{
	CListCtrl *list2 = (CListCtrl*)GetDlgItem(IDC_LIST2);
	int index = 1;
	int totalr[256] = { 0 }, totalo[256] = { 0 }, totalyg[256] = { 0 }, totalg[256] = { 0 };
	
	CString tValue; 
	for (int j = 1; j < 15;j++)
	{
		int rIndex = 0, gIndex = 0, oIndex = 0, ygIndex = 0;
		int index = 0;
		for (int i = 0; i < list2->GetItemCount() - 4; i++)
		{		
			index = i / 4;
			tValue = list2->GetItemText(i, j);//白细胞
			if (i == (index*4 + 3))//绿色
			{
				totalg[gIndex++] = _ttoi(tValue);
			}
			if (i == (index * 4 + 2))//黄绿
			{
				totalyg[ygIndex++] = _ttoi(tValue);
			}
			if (i == (index * 4 + 1))//橙色
			{
				totalo[oIndex++] = _ttoi(tValue);
			}
			if (i == (index * 4 + 0))//红
			{
				totalr[rIndex++] = _ttoi(tValue);
			}
			
		}		
		CString tmp;
		tmp.Format(L"%.6g%%", CalcCvByBlock(totalr, ++index) * 100);
		list2->SetItemText(index*4, j, tmp);

		tmp.Format(L"%.6g%%", CalcCvByBlock(totalo, index) * 100);
		list2->SetItemText(index * 4 + 1, j, tmp);

		tmp.Format(L"%.6g%%", CalcCvByBlock(totalyg, index) * 100);
		list2->SetItemText(index * 4 + 2, j, tmp);

		tmp.Format(L"%.6g%%", CalcCvByBlock(totalg, index) * 100);
		list2->SetItemText(index * 4 + 3, j, tmp);
	} 
}

double CStdUdcTest::CalcCvByBlock(int *num, int index)
{
	int total = 0;
	double powValue = 0;
	double sqrtValue = 0;
	for (int i = 0; i < index;i++)
	{
		DBG_MSG("debug:计算cv值：num[%d] = %d\n",i,num[i]);
		total += num[i];
	}
	double avg = total / (double)index;
	if (!avg)
	{
		return 0;
	}
	for (int i = 0; i < index; i++)
	{
		double k = double(num[i] - avg);
		powValue += pow(double(num[i] - avg),2);
	}
	if (index < 1)
	{
		return (sqrt((double)((powValue) / index)))/avg;
	}
	return sqrtValue = (sqrt((double)((powValue) / (index - 1))))/avg;
}

void CStdUdcTest::WriteToTXT()
{
	LogFile tLog;
	CString tmp;
	CListCtrl *list = (CListCtrl*)GetDlgItem(IDC_LIST2);
	for (int i = 1; i < 15; i++)
	{
		for (int j = 0; j < list->GetItemCount(); j++)
		{
			tmp.Append(list->GetItemText(i, j));
			tmp.Append(L" ");
			
			
		}
		tmp.Append(L"\n");
	}
	tLog.Log((char*)Common::WCharToChar(tmp.GetBuffer()).c_str(),"udc_std.txt");
}


void CStdUdcTest::OnBnClickedButton44()
{
	// TODO:  在此添加控件通知处理程序代码
	CString str;
	CListCtrl *list = (CListCtrl*)GetDlgItem(IDC_LIST2);
	const char* colorArr[] = { "绿灯", "黄绿灯", "橙灯", "红灯" };
	if (!isCalcd)
	{
		for (int i = 0; i < 4; i++)
		{
			str = colorArr[3 - i];
			str.Append(L"cv值");
			list->InsertItem(list->GetItemCount(), str);
		}
	}
	CalcCvByBlock();
	WriteToTXT();
	isCalcd = true;
}

