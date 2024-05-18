/*****************************************************************************
Copyright (C), 1995-2010, Chongqing Tianhai Medical Equipment Co., Ltd.
文件名:UdcView.cpp
功能:  干化学检测并显示结果
时间:  2010-06
*******************************************************************************/
#include "stdafx.h"
#include "UW.h"
#include "MainFrm.h"
#include "UdcView.h"
#include "Include/Common/String.h"
#include "DbgMsg.h"

using namespace Common;


extern std::vector<positiveID> positiveUDCID;
extern std::vector<positiveID> positiveUSID;

// CUdcView

IMPLEMENT_DYNCREATE(CUdcView, CView)

CUdcView::CUdcView()
{
	GetUDCSettingIniPath();
}

CUdcView::~CUdcView()
{
}

BEGIN_MESSAGE_MAP(CUdcView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_UDC, &CUdcView::OnCustomList)
	ON_MESSAGE(WM_UDCSETPAPERTYPE, &CUdcView::OnSetPaperType)
	ON_MESSAGE(WM_UPDATESKINUI, &CUdcView::OnSetPaperType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_UDC, &CUdcView::OnItemChanged)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CUdcView 绘图

void CUdcView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CUdcView 诊断

#ifdef _DEBUG
void CUdcView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CUdcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CUdcView 消息处理程序
int CUdcView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//创建列表框
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_List.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER
		, rectDummy, this, IDC_LIST_UDC))
	{
		OutputDebugString(_T("未能创建UDC列表"));
		return -1;     
	}

	// 设置扩展属性
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_List.ModifyStyle(LVS_SHOWSELALWAYS,0);
	//m_List.CreateSmallImage();

	//初始化
	InitList();
	//RefurbishData();
	FillList();
	return 0;
}

void CUdcView::OnSize(UINT iType, int cx, int cy)
{
	CView::OnSize(iType, cx, cy);

	//
	m_List.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CUdcView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CUdcView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void CUdcView::OnCustomList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// Take the default processing unless we 
	// set this to something else below.
	*pResult = CDRF_DODEFAULT;
	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		COLORREF crText, crBk;
		// 判断测试结果是否大于一个+
		if (IsResultGreateOnePlus(pLVCD->nmcd.dwItemSpec))
		{
			crText = RGB(255, 0, 0);
			pLVCD->clrText = crText;
			*pResult = CDRF_NEWFONT;
		}
		else
		{
			*pResult = CDRF_DODEFAULT;
		}
		//奇偶判断
		/*if ((pLVCD->nmcd.dwItemSpec % 2) == 0)
		{
		crText = RGB(0, 0, 0);//RGB(32,32,255);
		crBk = RGB(229, 232, 239);
		}
		else if ((pLVCD->nmcd.dwItemSpec % 2) == 1)
		{
		crText = RGB(0, 0, 0);
		crBk = RGB(240, 247, 249);
		}
		else {
		crText = RGB(0, 0, 0);
		crBk = RGB(0, 0, 126);
		}*/
		// Store the color back in the NMLVCUSTOMDRAW struct.
		//pLVCD->clrText = crText;
		//pLVCD->clrTextBk = crBk;

		//设置选择项的颜色
		/*if (this->m_List.GetItemState(pLVCD->nmcd.dwItemSpec, CDIS_SELECTED)) {
		crBk = RGB(75, 149, 229);//itunes//RGB(10, 36, 106);//RGB(0, 0, 64);
		crText = RGB(255, 0, 0);
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBk;
		*pResult = CDRF_NEWFONT;
		}
		if (LVIS_SELECTED == m_List.GetItemState(pLVCD->nmcd.dwItemSpec, LVIS_SELECTED))
		{
		//清除选择状态，如果不清除的话，还是会显示出蓝色的高亮条
		BOOL b = m_List.SetItemState(pLVCD->nmcd.dwItemSpec, 0, LVIS_SELECTED);
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBk;

		*pResult = CDRF_NEWFONT;
		return;
		}*/
		//*pResult = CDRF_NEWFONT;
		//*pResult = CDRF_DODEFAULT;
	}
}

LRESULT CUdcView::OnSetPaperType(WPARAM wParam, LPARAM lParam)
{
	int nIndexMA = 0;
	CHeaderCtrl* pHeaderCtrl = m_List.GetHeaderCtrl();
	if (pHeaderCtrl != NULL)
	{
		int  nColumnCount = pHeaderCtrl->GetItemCount();
		for (int i = 0; i < nColumnCount; i++)
		{
			HDITEM pHeadItem;
			pHeadItem.mask = HDI_TEXT;
			const int MAX_HEADER_LEN = 256;
			TCHAR  lpBuffer[MAX_HEADER_LEN];
			pHeadItem.pszText = lpBuffer;
			pHeadItem.cchTextMax = MAX_HEADER_LEN;
			pHeaderCtrl->GetItem(i, &pHeadItem);
			if (StrCmp(pHeadItem.pszText, _T("MA")) == 0)
			{
				nIndexMA = i;
				break;
			}
		}
	}

	int nType = (int)wParam;
	switch (nType)
	{
	case 11:
		if (nIndexMA > 0)
		{
			m_List.SetColumnWidth(nIndexMA, 0);
		}
		break;
	case 12:
		if (nIndexMA > 0)
		{
			m_List.SetColumnWidth(nIndexMA, 65);
		}
		break;
	default:
		break;
	}

	return S_OK;
}

LRESULT CUdcView::OnUpdateSkinUI(WPARAM wParam, LPARAM lParam)//重绘
{
	return 0;
}

void CUdcView::GetUDCSettingIniPath()
{
	GetModuleFileNameA(NULL, m_UDCIni, sizeof(m_UDCIni));
	PathRemoveFileSpecA(m_UDCIni);
	PathAppendA(m_UDCIni, "Config");
	PathAppendA(m_UDCIni, UDC_SETTING_INI);
}

int CUdcView::PushBacktoUDCPositive(positiveID m_udc,int nItem)
{
	if (m_udc.active == NEW)
	{
		return 0;
	}
	std::vector<positiveID>::iterator it;
	if (m_udc.active == FAIL || m_udc.active == TESTFAIL || m_udc.active == NOTHING)
	{
		if (positiveUDCID.size() == 0)
		{
			positiveUDCID.push_back(m_udc);
			return 0;
		}
		for (it = positiveUDCID.begin(); it != positiveUDCID.end(); it++)
		{
			if (it->id == m_udc.id)
			{
				it->active = m_udc.active;
				return 0;
			}
		}
		positiveUDCID.push_back(m_udc);
	}
	else if (IsResultGreateOnePlus(nItem))
	{
		if (positiveUDCID.size() == 0)
		{
			positiveUDCID.push_back(m_udc);
			return 0;
		}
		for (it = positiveUDCID.begin(); it != positiveUDCID.end(); it++)
		{
			if (it->id == m_udc.id)
			{

				it->active = m_udc.active;
				return 0;
			}
			else
			{
				positiveUDCID.push_back(m_udc);
				return 0;
			}
		}
		positiveUDCID.push_back(m_udc);
	}

	return 0;
}

/*************************************************************************
函数名:  InitList
函数描述:初始化列表框
**************************************************************************/
void CUdcView::InitList()
{
	UINT nTestPaperType = GetPrivateProfileIntA(UDC_CHECK_SETTING, UDC_CHECK_ITEM_COUNT, 14, m_UDCIni);
	char confPath[MAX_PATH] = { 0 };
	char udcOrder[64] = { 0 };

	GetModuleFileNameA(NULL, confPath, MAX_PATH);
	PathRemoveFileSpecA(confPath);
	PathAppendA(confPath, "conf.ini");

	LOGFONT lf = { 0 };
	m_List.GetFont()->GetLogFont(&lf);
	lf.lfHeight = -14;
	//DBG_MSG("m_WndPropList default font size: %s %d, %d\n", WstringToString(lf.lfFaceName).c_str(), lf.lfHeight, lf.lfWidth);
	const auto hFont = ::CreateFontIndirectW(&lf);
	m_List.SetFont(CFont::FromHandle(hFont));

	int i = 0;
	m_List.InsertColumn(i++, _T("属性"), LVCFMT_CENTER,80);
	m_List.InsertColumn(i++, _T("序号"), LVCFMT_CENTER, 40);
	int index = 0;
 	for(int j = 0; j < UDC_MAXITEM; j++)
 	{
 		if(theApp.udc_cfg_info.par[j].bIsDelete == TRUE)
 		    return;
		int nDefaultWidth = 65;
		if (nTestPaperType == 11 && strcmp(theApp.udc_cfg_info.par[j].sShortName, "MA") == 0)//如果为11项不要MA
		{
			nDefaultWidth = 0;
		}

		if (strcmp(theApp.udcOrder, "youyuan") == 0)
		{
			index = UdcOrderByYouyuan(i-2);
		}
		else
		{
			index = j;
		}
		m_List.InsertColumn(i++, Common::CharToWChar(theApp.udc_cfg_info.par[index].sShortName).c_str(), LVCFMT_CENTER, nDefaultWidth);
 	}
}

bool CUdcView::IsResultGreateOnePlus(int nIndex)
{
	int nCols = m_List.GetHeaderCtrl()->GetItemCount();
	for (int i = 2; i < nCols; i++)
	{
		CString strItem = m_List.GetItemText(nIndex, i);
		int pos = strItem.Find(_T("(+"));
		if (pos >= 0)
		{
			CString src = strItem.Mid(pos + 2, 1);
			if (src == src.SpanIncluding(_T("0123456789")))
			{
				return true;
			}
		}
	}

	return false;
}

/*************************************************************************
函数名:  RefurbishData
函数描述:刷新数据
**************************************************************************/
//void CUdcView::RefurbishData()
//{
//
//	CString        m_strTestResult[UDC_MAXITEM]={0};                 // 检测结果
//	CString        m_strUnit[UDC_MAXITEM]={0};                       // 物质含量
//	bool           m_bAbnormity[UDC_MAXITEM]={0};                    // 异常标记
//
//	CString strTemp =_T("");
//	m_List.SetRedraw(FALSE);
//
//	//清空
//	m_List.DeleteAllItems();
//
//	
//	TOADY_TASK_INFO1 udcInfo;
//	Access_GetTodayTaskInfo(&udcInfo);
//	for (int i = 0 ; i < udcInfo.nToadyTaskCounter; )
//	{
//		CString strSN, strValue;
//		int nItem = -1;
//
//		//插入一个项
//		nItem = m_List.InsertItem(m_List.GetItemCount(), _T(""));
//
//		//
//		//int i = 0;
//		/*if(1 == udcInfo.pFullTaskInfo[i].main_info.nAttribute)
//			m_List.SetItemText(nItem, i, _T("UDC质控"));*/
//	/*	if (0 == udcInfo.pFullTaskInfo[i].main_info.nAttribute )
//		{
//			m_List.SetItemText(nItem, i, _T("UDC质控"));
//		}
//		if(6 == udcInfo.pFullTaskInfo[i].main_info.nAttribute)
//			m_List.SetItemText(nItem, i, _T("急诊"));*/
//		/*if(3 == udcInfo.pFullTaskInfo[i].main_info.nAttribute)
//			m_List.SetItemText(nItem, i, _T("US质控"));*/
//
//
//		switch (udcInfo.pFullTaskInfo[i].main_info.nAttribute)
//		{
//		case 0:
//			m_List.SetItemText(nItem, i, _T("普通"));
//			break;
//		case 1:
//		case 2:
//			m_List.SetItemText(nItem, i, _T("US质控"));
//			break;
//		case 3:
//		case 4:
//		case 5:
//			m_List.SetItemText(nItem, i, _T("UDC质控"));
//			break;
//		case 6:
//			m_List.SetItemText(nItem, i, _T("急诊"));
//			break;
//		}
//
//		i++;
//		strSN.Format(_T("%d"), udcInfo.pFullTaskInfo[i].main_info.nSN);
//		m_List.SetItemText(nItem, i++, strSN);
//
//		// 取颜色反射量
//		for(int j = 0; j < MAX_UDC_COUNTS; j++)
//		{
//			for (int z = 0; z< UDC_MAXGRADE;z++)
//			{
//				if (theApp.m_UdcitemInfo[i].m_bGradeEnable[j])
//				{
//					if (udcInfo.pFullTaskInfo[i].udc_info.udc_node[j].udc_col>=theApp.m_UdcitemInfo[i].m_nGradeThreshold[z])
//					{
//						m_strTestResult[i]=theApp.m_UdcitemInfo[i].m_ctsGradeName[j];
//						m_strUnit[i] = theApp.m_UdcitemInfo[i].m_ctsGradeValue[j];
//
//
//						strTemp.Format(_T("%d(%s)"),udcInfo.pFullTaskInfo[i].udc_info.udc_node[j].udc_col,m_strTestResult[i]);
//
//						//strTemp = theApp.m_UdcitemInfo[i].m_ctsGradeUnit[j];
//						//m_strUnit[i] +=strTemp;
//						m_List.SetItemText(nItem,i++,strTemp);
//					}
//				}
//			}
//		}
//
//		//// 得到检测结果
//		//theApp.m_pUdcTest->GetResult();
//
//		//// 存储检测结果
//		//for(int j = 0; j < UDC_MAXITEM; j++)
//		//{
//		//	if(theApp.m_UdcGrade.m_bItemEnable[j])
//		//	{
//		//		strValue.Format(_T("%d(%s)")
//		//			, theApp.m_pUdcTest->m_nTestValue[j]
//		//			, theApp.m_pUdcTest->m_strTestResult[j]);
//		//		m_List.SetItemText(nItem, i++, strValue);
//		//	}
//		//}
//
//		//for(int j = 0; j < UDC_MAXITEM; j++)
//		//{
//		//	if(theApp.udc_cfg_info.par[j].bIsDelete == TRUE)
//		//		return;
//		//	
//		//	strValue.Format(_T("%d(%s)")
//		//					, udcInfo.pFullTaskInfo[i].udc_info.udc_node[j].
//		//					, theApp.m_pUdcTest->m_strTestResult[j]);
//		//				m_List.SetItemText(nItem, i++, strValue);
//		//}
//
//		//theApp.m_pUdcTest->SaveRecord(nID);
//	}
//	
//
//	m_List.SetRedraw(TRUE);
//}

/*************************************************************************
函数名:  GetColorAndResult
函数描述:从数据库中获得颜色,检测
输入参数:nID----------添加项对应数据库中的ID
**************************************************************************/
//void CUdcView::GetColorAndResult(unsigned int nID)
//{
//	
//}

extern CString GetAttributeDescription(int nAttribute);

int CUdcView::AddTaskToList(TASK_INFO &task_info, BOOL bInsert)
{

	CString        m_strTestResult[UDC_MAXITEM]={0};                 // 检测结果
	CString        m_strUnit[UDC_MAXITEM]={0};                       // 物质含量
	bool           m_bAbnormity[UDC_MAXITEM]={0};                    // 异常标记

	CString strTemp =_T("");
	int      nItem   = m_List.GetItemCount(),i = 0;
	char confPath[MAX_PATH] = { 0 };

	//插入一个项

	if( bInsert == TRUE )
	{
		m_List.InsertItem(m_List.GetItemCount(), _T(""));
		m_List.SetItemData(nItem, task_info.main_info.nID);

		CString strValue = GetAttributeDescription(task_info.main_info.nAttribute);
		m_List.SetItemText(nItem, i, strValue);

		//if (theApp.m_pMainWnd)
		//{
		//	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TODAYLIST_UPDATELISTPOS, (WPARAM)2, (LPARAM)0);
		//}
		m_List.PostMessage(WM_VSCROLL, SB_BOTTOM);
		

// 		switch (task_info.main_info.nAttribute)
// 		{
// 		case 0:
// 			m_List.SetItemText(nItem, i, _T("普通"));
// 			break;
// 		case 1:
// 		case 2:
// 			m_List.SetItemText(nItem, i, _T("US（）"));
// 			break;
// 		case 3:
// 		case 4:
// 		case 5:
// 			m_List.SetItemText(nItem, i, _T("UDC质控"));
// 			break;
// 		case 6:
// 			m_List.SetItemText(nItem, i, _T("急诊"));
// 			break;
// 		}
	}
	else
	{
		for( nItem -- ; nItem >= 0 ; nItem -- )
		{
			if( m_List.GetItemData(nItem) == task_info.main_info.nID )
				break;
		}

		if( nItem < 0 )
			return -1;
	}

	i++;
	CString strSN;

	if( bInsert == TRUE )
	{
		strSN.Format(_T("%d"), task_info.main_info.nSN);
		m_List.SetItemText(nItem, i++, strSN);
	}
	else
	{
		i++;
	}

	int index = 0;
		
	BOOL bFindGrade = FALSE;
	// 取颜色反射量
	for(int j = 0; j < MAX_UDC_COUNTS; j++)
	{
		if (!theApp.udc_cfg_info.par[j].bIsDelete)
		{
			CString strTemp;
			char ConfigPath[MAX_PATH] = { 0 };

			GetModuleFileNameA(NULL, ConfigPath, MAX_PATH);
			PathRemoveFileSpecA(ConfigPath);
			PathAppendA(ConfigPath, "config//setting.ini");

			int nSGTestWay = GetPrivateProfileIntA("SGTestWay", "Way", 0, ConfigPath);

			if (nSGTestWay == 1 && strcmp(Common::WCharToChar(theApp.m_UdcitemInfo[j].m_ctsItemCode).c_str(), "SG") == 0)
			{
				CHAR ResultPath[MAX_PATH] = { 0 }, Value[32] = { 0 };
				GetModuleFileNameA(NULL, ResultPath, MAX_PATH);
				PathRemoveFileSpecA(ResultPath);
				PathAppendA(ResultPath, "RecGrapReslut");
				sprintf_s(Value, "%d", task_info.udc_info.nID);
				PathAppendA(ResultPath, Value);
				PathAppendA(ResultPath, "Physics.ini");
				strTemp.Format(_T("%d(%.3f)"), 1000 + GetPrivateProfileIntA("Physics", "sg_test", 0, ResultPath), float(1000.0 + GetPrivateProfileIntA("Physics", "sg_test", 0, ResultPath)) / 1000.0);
			}
			else
			{
				bFindGrade = FALSE;

				for (int z = 0; z < UDC_MAXGRADE; z++)
				{
					if (theApp.m_UdcitemInfo[j].m_bGradeEnable[z])
					{
						if (task_info.udc_info.udc_node[j].udc_col >= theApp.m_UdcitemInfo[j].m_nGradeThreshold[z])
						{
							bFindGrade = TRUE;
							m_strTestResult[j] = theApp.m_UdcitemInfo[j].m_ctsGradeName[z];
							m_strUnit[j] = theApp.m_UdcitemInfo[j].m_ctsGradeValue[z];
							strTemp.Format(_T("%d(%s)"), task_info.udc_info.udc_node[j].udc_col, m_strTestResult[j]);
							strcpy(task_info.udc_info.udc_node[j].udc_res,Common::WCharToChar(m_strTestResult[j].GetBuffer()).c_str());
							break;
						}
					}
				}

				if (bFindGrade == FALSE)
					strTemp.Format(_T("%d( )"), task_info.udc_info.udc_node[j].udc_col);
			}
			
		}
		else
		{
			strTemp.Empty();
		}

		m_List.SetItemText(nItem, i++, strTemp);			
	}
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (!bInsert && theApp.nAttribute != TestNormal && theApp.nAttribute != TestEP && pMain->qcDlg)
	{		
		pMain->qcDlg->SendMessage(WM_USER_ADDTASKTOQC, (WPARAM)1, (LPARAM)&task_info);
	}
	if (strncmp(theApp.udcOrder, "youyuan",7) == 0)
	{
		i = 2;
		for (int j = 0; j < MAX_UDC_COUNTS; j++)
		{
			index = UdcOrderByYouyuan(j);//UDC重新排序
			strTemp.Format(_T("%d(%s)"), task_info.udc_info.udc_node[index].udc_col, m_strTestResult[index]);
			m_List.SetItemText(nItem, i++, strTemp);
		}
	}
	//int nCount = m_List.GetItemCount();
	//if (nCount > 0)
	//{
	//	m_List.EnsureVisible(nCount - 1, FALSE);
	//}

	StatusEnum   nUSStatus, nUDCStatus;
	theApp.ParseStatusMaskCode(task_info.main_info.nState, nUSStatus, nUDCStatus);
	positiveID m_udc;
	m_udc.active = nUDCStatus;
	m_udc.id = task_info.main_info.nID;
	PushBacktoUDCPositive(m_udc,nItem);

/*	m_List.UpdateWindow();*/
	m_List.SetScrollPos(SB_VERT,nItem);
	return nItem;
}

int CUdcView::AddTaskToList( int nID )
{
	TASK_INFO  task_info = {0};

	if( Access_GetTaskInfo(nID, ALL_TYPE_INFO, &task_info) != STATUS_SUCCESS ) //这里出错说明数据库存在bug
		return -1;

	AddTaskToList(task_info);
	return 0;

}




void CUdcView::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;  

	if (pNMListView->uChanged == LVIF_STATE && (pNMListView->uNewState & LVIS_SELECTED))
	{

		KSTATUS    status;
		TASK_INFO  task_info1 = { 0 };

		int id = m_List.GetItemData(pNMListView->iItem);
		theApp.GetMainWnd()->SendMessage(WM_SELECTRECORDBYUDC,id,0);
	}
}

//
void CUdcView::FillList(void)
{
	TOADY_TASK_INFO1  t_today_info;


	if ( Access_GetTodayTaskInfo(&t_today_info) != STATUS_SUCCESS )
		return;

	m_nlistNum    =  t_today_info.nToadyTaskCounter;
	//theApp.m_nSN  =  t_today_info.pFullTaskInfo[m_nlistNum -1].main_info.nSN;
	//theApp.m_nRow =  t_today_info.pFullTaskInfo[m_nlistNum -1].main_info.nRow;

	m_List.DeleteAllItems();
	m_List.SetRedraw(FALSE);

	for (int i = 0; i< m_nlistNum; ++i)
		AddTaskToList(t_today_info.pFullTaskInfo[i]);

	m_List.SetRedraw(TRUE);
	Access_FreeGetTodayTaskInfoBuf(t_today_info);
}

int CUdcView::GetIndexByID(int id)
{
	int nCount = m_List.GetItemCount();
	for (int i = 0; i < nCount;i++)
	{
		int a = (int)m_List.GetItemData(i);
		if ((int)m_List.GetItemData(i) == id)
		{
			return i;
		}
	}
	return -1;
}

void CUdcView::UpdateListView()
{
	int count = m_List.GetItemCount();
	if (count > 0)
	{
		m_List.EnsureVisible(count - 1, FALSE);
	}
}

int CUdcView::UdcOrderByYouyuan(int index)
{
	switch (index)
	{
	case 0:	index = 0; break;
	case 1:	index = 1; break;
	case 2:	index = 3; break;
	case 3:	index = 9; break;
	case 4:	index = 7; break;
	case 5:	index = 2; break;
	case 6:	index = 8; break;
	case 7:	index = 4; break;
	case 8:	index = 6; break;
	case 9:	index = 5; break;
	case 10:index = 10; break;
	case 11:index = 12; break;
	case 12:index = 11; break;
	case 13:index = 13; break;
	case 14:index = 14; break;
	case 15:index = 15; break;
	default:
		break;
	}
	return index;
// 	switch (i)
// 	{
// 	case 2:m_List.InsertColumn(2,	 L"LEU", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 3:m_List.InsertColumn(3,	 L"NIT", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 4:m_List.InsertColumn(7,	 L"PRO", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 5:m_List.InsertColumn(4,	 L"GLU", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 6:m_List.InsertColumn(9,	 L"KET", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 7:m_List.InsertColumn(11,	 L"URO", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 8:m_List.InsertColumn(10,	 L"BIL", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 9:m_List.InsertColumn(6,	 L"PH", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 10:m_List.InsertColumn(8,	 L"SG", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 11:m_List.InsertColumn(5,	 L"BLD", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 12:m_List.InsertColumn(12,	 L"VC", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 13:m_List.InsertColumn(14,	 L"CRE", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 14:m_List.InsertColumn(13,	 L"MCA", LVCFMT_CENTER, nDefaultWidth); break;
// 	case 15:m_List.InsertColumn(15,	 L"Ca", LVCFMT_CENTER, nDefaultWidth); break;
// 	default:
// 	}
}

void CUdcView::UpdateSearchRecordResult()
{
	m_List.DeleteAllItems();
	for (int i = 0; i < theApp.searchtask.pSearchInfoCounts; i++)
	{
		AddTaskToList(theApp.searchtask.pSearchInfo[i]);
	}
}

void CUdcView::DeleteItem(ULONG nTaskID)
{
	for( ULONG j = 0 ; j < m_List.GetItemCount() ; j ++ )
	{
		if( m_List.GetItemData(j) == nTaskID )
		{
			m_List.DeleteItem(j);
			return;
		}
	}

}




void CUdcView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
	m_nSelItem = GetIndexByID(m_List.curSelectID);
	m_List.SetItemState(m_nSelItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
	// TODO:  在此处添加消息处理程序代码
}


void CUdcView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
/*	m_List.SetRedraw(FALSE);*/
	m_List.SetItemState(m_nSelItem, FALSE, LVIF_STATE);
/*	m_List.SetRedraw(TRUE);*/
	// TODO:  在此处添加消息处理程序代码
}
