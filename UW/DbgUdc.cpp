// DbgUdc.cpp : 实现文件
//

#include "stdafx.h"
#include "DbgUdc.h"
#include "afxdialogex.h"
#include "CmdHelper.h"
#include "UW.h"
#include <algorithm>
#include <future>
#include "DbgMsg.h"

#define UDC_COLORBLOCK_COUNT (14)

#define WM_SELFEEDPAPERTEST WM_USER+0xA1 
#define WM_REPEATTEST       WM_USER+0xA2

// CDbgUdc 对话框

IMPLEMENT_DYNAMIC(CDbgUdc, CPropertyPage)

CDbgUdc::CDbgUdc()
	: CPropertyPage(CDbgUdc::IDD)
{
	isCalced = false;
}

CDbgUdc::~CDbgUdc()
{
}

void CDbgUdc::OnRecvSelAndFeedPaperTestData(void *arg, void *cbarg)
{
	SendMessage(WM_SELFEEDPAPERTEST, (WPARAM)arg);
}

void CDbgUdc::OnRecvRepeatTestData(void *arg, void *cbarg)
{
	SendMessage(WM_REPEATTEST, (WPARAM)arg);
}

LRESULT CDbgUdc::RecvSelFeedPaperTestDataHandler(WPARAM w, LPARAM l)
{
	if (w)
	{
		PUDC_SEL_AND_FEED_PAPER_DATA_CONTEXT ctx = (PUDC_SEL_AND_FEED_PAPER_DATA_CONTEXT)w;
		CString str;
		str.Format(_T("%d"), ctx->CurTestCount);
		SetDlgItemText(IDC_EDIT1, str);
		str.Format(_T("%d"), ctx->StuckPaperCount);
		SetDlgItemText(IDC_EDIT2, str);
		str.Format(_T("%d"), ctx->DropPaperCount);
		SetDlgItemText(IDC_EDIT3, str);
		str.Format(_T("%d"), ctx->FlipPaperCount);
		SetDlgItemText(IDC_EDIT4, str);
	}

	return 0;
}

LRESULT CDbgUdc::RecvRepeatTestDataHandler(WPARAM w, LPARAM l)
{
	if (w)	
	{
		PUDC_REPEATTEST_DATA_CONTEXT ctx = (PUDC_REPEATTEST_DATA_CONTEXT)w;
		CListCtrl *list1 = (CListCtrl*)GetDlgItem(IDC_LIST1);
		int nCount = list1->GetItemCount();
		CString str;
		switch (ctx->PaperStatus)
		{
		case PAPER_NORMAL:
			str = _T("正常");
			break;
		case PAPER_ERROR:
			str = _T("错误");
			break;
		case PAPER_DROP:
			str = _T("丢失");
			break;
		default:
			str = _T("异常");//str.Format(_T("%d"), ctx->PaperStatus);
			break;
		}
		list1->InsertItem(nCount,str);
		uint16_t *p = (uint16_t*)&ctx->UdcColorBlockCtx;
		for (int i = 0; i < UDC_COLORBLOCK_COUNT; i++)
		{
			str.Format(_T("%d"), p[i]);
			list1->SetItemText(nCount, i + 1, str);
		}
	}

	return 0;
}

void CDbgUdc::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON3, m_btnCV);
}


BEGIN_MESSAGE_MAP(CDbgUdc, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CDbgUdc::OnBnClickedFeedPaperStartTest)
	ON_BN_CLICKED(IDC_BUTTON2, &CDbgUdc::OnBnClickedFeedPaperStopTest)
	ON_BN_CLICKED(IDC_BUTTON13, &CDbgUdc::OnBnClickedUdcStartRepeatTest)
	ON_BN_CLICKED(IDC_BUTTON30, &CDbgUdc::OnBnClickedUdcStopRepeatTest)
	ON_MESSAGE(WM_SELFEEDPAPERTEST, &CDbgUdc::RecvSelFeedPaperTestDataHandler)
	ON_MESSAGE(WM_REPEATTEST, &CDbgUdc::RecvRepeatTestDataHandler)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON3, &CDbgUdc::OnBnClickedButton3)
END_MESSAGE_MAP()


// CDbgUdc 消息处理程序


BOOL CDbgUdc::OnInitDialog()
{

	CPropertyPage::OnInitDialog();

	CListCtrl *list1 = (CListCtrl*)GetDlgItem(IDC_LIST1);

	list1->InsertColumn(0,_T("纸条状态"), 0, 60);
	list1->InsertColumn(1,_T("LEU"), 0, 100);
	list1->InsertColumn(2,_T("NIT"), 0, 100);
	list1->InsertColumn(3,_T("URO"), 0, 100);
	list1->InsertColumn(4,_T("PRO"), 0, 100);
	list1->InsertColumn(5,_T("PH"), 0, 100);
	list1->InsertColumn(6,_T("BLD"), 0, 100);
	list1->InsertColumn(7,_T("SG"), 0, 100);
	list1->InsertColumn(8,_T("KET"), 0, 100);
	list1->InsertColumn(9,_T("BIL"), 0, 100);
	list1->InsertColumn(10,_T("GLU"), 0, 100);
	list1->InsertColumn(11, _T("VC"), 0, 100);
	list1->InsertColumn(12, _T("MA"), 0, 100);
	list1->InsertColumn(13, _T("CRE"), 0, 100);
	list1->InsertColumn(14, _T("CA"), 0, 100);

	m_btnCV.EnableWindow(FALSE);

	CMyDelegate1<CDbgUdc*, void (CDbgUdc::*)(void*, void*), void*> *e1 = new CMyDelegate1<CDbgUdc*, void (CDbgUdc::*)(void*, void*), void*>;
	e1->RegisterEventHandler(this, &CDbgUdc::OnRecvSelAndFeedPaperTestData, 0);
	theApp.m_clsMsgDipatcher.AddEventHandler(UdcSelAndFeedPaperTestData, e1);

	CMyDelegate1<CDbgUdc*, void (CDbgUdc::*)(void*, void*), void*> *e2 = new CMyDelegate1<CDbgUdc*, void (CDbgUdc::*)(void*, void*), void*>;
	e2->RegisterEventHandler(this, &CDbgUdc::OnRecvRepeatTestData, 0);
	theApp.m_clsMsgDipatcher.AddEventHandler(UdcRepeatTestData, e2);

	m_mapEventDelegate[UdcSelAndFeedPaperTestData] = e1;
	m_mapEventDelegate[UdcRepeatTestData] = e2;

/*	ReChildWndsize();*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CDbgUdc::OnDestroy()
{
	CPropertyPage::OnDestroy();

	std::for_each(m_mapEventDelegate.begin(), m_mapEventDelegate.end(), [](std::map<int,void*>::reference r){
		theApp.m_clsMsgDipatcher.RemoveEventHandler(r.first, (CMyDelegate*)r.second);
	});
}

void CDbgUdc::OnBnClickedFeedPaperStartTest()
{
	GetDlgItem(IDC_EDIT1)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT2)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT3)->SetWindowText(_T(""));
	UdcSelAndFeedPaperTest();
}


void CDbgUdc::OnBnClickedFeedPaperStopTest()
{
	UdcSelAndFeedPaperTest(false);
}


void CDbgUdc::OnBnClickedUdcStartRepeatTest()
{
	m_btnCV.EnableWindow(FALSE);
	((CListCtrl*)GetDlgItem(IDC_LIST1))->DeleteAllItems();
	UdcRepeatTest();
}


void CDbgUdc::OnBnClickedUdcStopRepeatTest()
{
	m_btnCV.EnableWindow(TRUE);
	UdcRepeatTest(false);
}








void CDbgUdc::OnShowWindow(BOOL bShow, UINT nStatus)
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

void CDbgUdc::ReChildWndsize()
{
	CRect rect;
	CRect childSaved;
	float x = 0.8;
	float y = 0.8;
	GetWindowRect(rect);
	int wndWidth = GetSystemMetrics(SM_CXSCREEN);  //屏幕宽度    
	int wndHeight = GetSystemMetrics(SM_CYSCREEN);

	CWnd* pWnd = GetWindow(GW_CHILD);//获取子窗体  

	while (pWnd)
	{

		pWnd->GetWindowRect(childSaved);
		if (rect.Width() > wndWidth * 2 / 3 || rect.Height() > wndHeight * 2 / 3);
		{
			childSaved.left *= x;//根据比例调整控件上下左右距离对话框的距离  
			childSaved.right *= x;
			childSaved.top *= y;
			childSaved.bottom *= y;
			ScreenToClient(childSaved);
			pWnd->MoveWindow(childSaved);
			pWnd = pWnd->GetNextWindow();//取下一个子窗体  
		}
	}
	GetWindowRect(rect);
	ScreenToClient(rect);
	MoveWindow(rect.top, rect.left, rect.Width()*x, rect.Height()*y);
}


void CDbgUdc::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	CListCtrl *list1 = (CListCtrl*)GetDlgItem(IDC_LIST1);
	int nCount = list1->GetItemCount();
	list1->InsertItem(nCount, L"cv值");
	CalcCvByBlock();
	isCalced = true;
	m_btnCV.EnableWindow(FALSE);
	WriteToTXT();
}

void CDbgUdc::CalcCvByBlock()
{
	CListCtrl *list2 = (CListCtrl*)GetDlgItem(IDC_LIST1);
	int index = 1;
	int total[256] = { 0 };
	CString tValue;
	for (int j = 1; j < 15; j++)
	{
		index = 1;
		for (int i = 0; i < list2->GetItemCount() -1; i++)
		{
			tValue = list2->GetItemText(i, j);//白细胞
			total[index - 1] = _ttoi(tValue);
			index++;
		}
		index--;
		CString tmp;
		tmp.Format(L"%.6g%%", CalcCvByBlock(total, index) * 100);
		list2->SetItemText(index, j, tmp);
	}
}

double CDbgUdc::CalcCvByBlock(int *num, int index)
{
	int total = 0;
	double powValue = 0;
	double sqrtValue = 0;
	for (int i = 0; i < index; i++)
	{
		DBG_MSG("debug:计算cv值：num[%d] = %d\n", i, num[i]);
		total += num[i];
	}
	double avg = total / (double)index;
	if (avg <= 0)
		return 0;
	for (int i = 0; i < index; i++)
	{
		double k = double(num[i] - avg);
		powValue += pow(double(num[i] - avg), 2);
	}
	if (index < 1)
	{
		return (sqrt((double)((powValue) / index))) / avg;
	}
	return sqrtValue = (sqrt((double)((powValue) / (index - 1))))/avg;

}

void CDbgUdc::WriteToTXT()
{
	LogFile tLog;
	CString tmp;
	CListCtrl *list = (CListCtrl*)GetDlgItem(IDC_LIST1);
	for (int i = 1; i < 15; i++)
	{
		for (int j = 0; j < list->GetItemCount(); j++)
		{
			tmp.Append(list->GetItemText(i, j));
			tmp.Append(L" ");


		}
		tmp.Append(L"\n");
	}
	tLog.Log((char*)Common::WCharToChar(tmp.GetBuffer()).c_str(),"udc_repeat.txt");
}