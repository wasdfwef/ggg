
// UWView.cpp : CUWView 类的实现
//

#include "stdafx.h"
#include "UW.h"
#include "Camera.h"
#include "UWDoc.h"
#include "UWView.h"
#include "..\..\..\inc\THUIInterface.h"
#include "Include/Common/String.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUWView

extern 
std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId);

IMPLEMENT_DYNCREATE(CUWView, CTabView)

BEGIN_MESSAGE_MAP(CUWView, CTabView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CUWView::OnFilePrintPreview)
//	ON_COMMAND(ID_MENU_SET, &CUWView::OnMenuSet)
ON_WM_CREATE()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CUWView 构造/析构

CUWView::CUWView()
:m_iUpwardTab(1)
{
	m_pMicroscopeCamera = NULL;
	m_pCharacterCamera = NULL;
	m_pUdcView = NULL;
}

CUWView::~CUWView()
{
}

BOOL CUWView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
// 	cs.style &= ~WS_VSCROLL;//去掉垂直滚动条
// 	cs.style &= ~WS_HSCROLL;//去掉水平滚动条

// 	cs.cx = GetSystemMetrics(SM_CXSCREEN);
// 	cs.cy = GetSystemMetrics(SM_CYSCREEN);
// 
// 	cs.style |= WS_HSCROLL | WS_VSCROLL;


	return CView::PreCreateWindow(cs);
}

// CUWView 绘制

void CUWView::OnDraw(CDC* /*pDC*/)
{
	CUWDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CUWView 打印


void CUWView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CUWView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CUWView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CUWView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CUWView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CUWView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CUWView 诊断

#ifdef _DEBUG
void CUWView::AssertValid() const
{
	CView::AssertValid();
}

void CUWView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUWDoc* CUWView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUWDoc)));
	return (CUWDoc*)m_pDocument;
}
#endif //_DEBUG


// CUWView 消息处理程序

//void CUWView::OnMenuSet()
//{
//	// TODO: 在此添加命令处理程序代码
//	AfxMessageBox(_T("321"));
//}

static BOOL g_Init = FALSE;


int CUWView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	CMainFrame        *pMainFrame;
	LIST_GROUP_INFO   list_group_info = {0};
	TAB_CTRL_CONTEXT  ctx = {0};

	if( g_Init == TRUE )
		return -1;


	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	this->GetTabControl().ModifyTabStyle(CMFCTabCtrl::STYLE_FLAT_SHARED_HORZ_SCROLL);
	AddView(RUNTIME_CLASS(CMicroscopeCameraView), GetIdsString(_T("uwview"),_T("name1")).c_str(), VIDEO_CY_VIEW_TAB);
	//AddView(RUNTIME_CLASS(CCharactorCameraView), GetIdsString(_T("uwview"), _T("name2")).c_str(), VIDEO_CY_VIEW2_TAB);
	// TODO:  在此添加您专用的创建代码
	CMFCTabCtrl &TabCtrl = GetTabControl();

	ctx.pTabCtrl = this;
	ctx.nIndex  = THUM_VIEW_TAB;
	strcpy(ctx.TabName, Common::WCharToChar(GetIdsString(_T("uwview"),_T("name3")).c_str()).c_str());
	list_group_info.nCreateType = TYPE_MFC_TAB_VIEW;
	list_group_info.pOtherCreateContext = &ctx;

	pMainFrame = (CMainFrame*)theApp.GetMainWnd();

	pMainFrame->SetTabView(this);
	THUIInterface_InitializeListGroupByTabCtrl(&list_group_info);
	
	// 添加视图
	//AddView(); 两个TAB  VIDEO_CY_VIEW_TAB   THUM_VIEW_TAB

	//AddView(RUNTIME_CLASS(CThumView), _T("TTT"), THUM_VIEW_TAB);
	//AddView(RUNTIME_CLASS(CListGroupView), _T("TTT"), THUM_VIEW_TAB);

	AddView(RUNTIME_CLASS(CUdcView),GetIdsString(_T("uwview"),_T("name4")).c_str(), THUM_UDC_TAB);

	m_pUdcView = (CUdcView*)TabCtrl.GetTabWnd(THUM_UDC_TAB); 
	TabCtrl.EnableTabSwap(FALSE);


	g_Init = TRUE;

	return 0;
}

//设置激活的视图
void CUWView::SetActiveTab( int nTab )
{
	CMFCTabCtrl &TabCtrl = GetTabControl();
	TabCtrl.SetActiveTab(nTab);
}

//得到激活的视图
int CUWView::GetActiveTab()
{
	CMFCTabCtrl &TabCtrl = GetTabControl();
	return TabCtrl.GetActiveTab();
}


void CUWView::OnActivateView( CView* pActivateView )
{
	
	CMainFrame  *pFrame  = (CMainFrame*)theApp.GetMainWnd();
	CMFCTabCtrl &TabCtrl = GetTabControl();
	int nTab = TabCtrl.GetActiveTab();

	m_pMicroscopeCamera = (CMicroscopeCameraView*)TabCtrl.GetTabWnd(VIDEO_CY_VIEW_TAB);
	//m_pCharacterCamera = (CCharactorCameraView *)TabCtrl.GetTabWnd(VIDEO_CY_VIEW2_TAB);
    
	//if (nTab == VIDEO_CY_VIEW_TAB || nTab == VIDEO_CY_VIEW2_TAB || nTab == THUM_VIEW_TAB || nTab == THUM_UDC_TAB)
	if (nTab == VIDEO_CY_VIEW_TAB || nTab == THUM_VIEW_TAB || nTab == THUM_UDC_TAB)
	{
		if( nTab == VIDEO_CY_VIEW_TAB )
		{
			pFrame->m_wndTodayList.SetBillMode(FALSE);
			m_pMicroscopeCamera->SetVideoPrview(TRUE);
		}
		/*if (nTab == VIDEO_CY_VIEW2_TAB)
		{
			m_pCharacterCamera->SetVideoPrview(TRUE);
		}*/
		else if(nTab == THUM_VIEW_TAB)
		{
			//pCamera->SetVideoPrview(FALSE);
			pFrame->m_wndTodayList.UpdateCellList();
			pFrame->m_wndTodayList.SetBillMode(TRUE);
		}
		else
		{
			//pCamera->SetVideoPrview(FALSE);
			pFrame->m_wndTodayList.SetBillMode(FALSE);
		}
		m_iUpwardTab = nTab;
	}
	//GetDocument()->SetDocTitle(nTab);
	CTabView::OnActivateView(pActivateView);
}

//切换到上一个视图
void CUWView::UpwardTab( void )
{
	CMFCTabCtrl &TabCtrl = GetTabControl();
	TabCtrl.SetActiveTab(m_iUpwardTab);
}

BOOL CUWView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
}

CWnd * CUWView::GetTabWnd( int nTab )
{
	CMFCTabCtrl &TabCtrl = GetTabControl();

	return  TabCtrl.GetTabWnd(nTab);

}


BOOL CUWView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
// 	if (pMsg->message == WM_LBUTTONDOWN)
// 	{
// 		CWnd *pWnd = FromHandle(pMsg->hwnd);
// 		if (pWnd->GetDlgCtrlID() == IDC_BUTTON_LISTSKIN)
// 		{
// 			MessageBox(_T("Yes"));
// 		}
// 	}
	return CTabView::PreTranslateMessage(pMsg);
}
