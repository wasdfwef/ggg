#include "stdafx.h"
#include "ListGroupView.h"
#include "DbgMsg.h"
#include "RecTask.h"
#include <afxcmn.h>
#include "thumbnail.h"
#include <Shlwapi.h>
#include <afxtabview.h>
#include "resource.h"

#pragma comment(lib, "..\\..\\..\\lib\\QueueManager.lib")
#pragma comment(lib, "..\\..\\..\\lib\\thumbnail.lib")
#pragma comment(lib, "shlwapi.lib")


//listctrl分组,非unicode,需要加入以下代码

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_DELETE_CELL  0X9000
#define CUSTOM_CREATE_MSG 0X9120
#define FLASH_PRE       10 

#define  MAKE_PROGRESS_INFO(nGroupCounter, i) ((nGroupCounter << 16) | i)
#define  GET_PROGRESS_INFO(nProgressInfo, nGroupCounter, nCurProgress) {nGroupCounter = nProgressInfo >> 16;nCurProgress = nProgressInfo & 0xffff;}
#define  SET_BUSY(x, busy)  InterlockedExchange(&x, busy)
#define  QUERY_BUSY(x)     (InterlockedExchangeAdd(&x,0) != 0)



IMPLEMENT_DYNCREATE(CListGroupView, CView)

BEGIN_MESSAGE_MAP(CListGroupView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_ICON_LIST, &CListGroupView::OnNMDblclk)
	ON_NOTIFY(NM_CLICK, IDC_ICON_LIST, &CListGroupView::OnNMLclk)
	ON_NOTIFY(NM_RCLICK, IDC_ICON_LIST, &CListGroupView::OnNMRclk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ICON_LIST, &CListGroupView::OnLvnItemchanged)
	ON_MESSAGE(CUSTOM_CREATE_MSG, &CListGroupView::OnCustomCreateMsg)
END_MESSAGE_MAP()

typedef struct _BITMAP_QUE
{
	CBitmap *iBitmap;

}BITMAP_QUE, *PBITMAP_QUE;

VOID 
CALLBACK 
rogress_callback(
PCHAR  pImageFolder,
USHORT nGrapIndex,
UCHAR  nGrapSize,
PVOID  pContext,
ULONG  nProgressInfo
);

VOID 
CALLBACK 
rogress_request(
PVOID  pContext,
ULONG  nTaskID
);

CListGroupView::CListGroupView()
{
	m_Busy       = 0;
	m_BitmapQue  = NULL;
	m_RecThread  = NULL;
	m_ThumbnailStart = FALSE;
	yShowType = -1;
	ZeroMemory(&m_PicSize, sizeof(m_PicSize));
	ZeroMemory(&m_CGSize, sizeof(SIZE));
	ZeroMemory(m_ImageFolder, sizeof(m_ImageFolder));	
}

CListGroupView::~CListGroupView()
{	
}

void CListGroupView::OnDraw(CDC* pDC)
{
}


void CListGroupView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void CListGroupView::OnDestroy()
{
	// TODO: 在此处添加消息处理程序代码

	DeleteAllItem();

	CView::OnDestroy();
}


int CListGroupView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//创建列表框

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_List.Create(WS_CHILD | WS_VISIBLE, rectDummy, this, IDC_ICON_LIST))
	{
		TRACE0("未能创建列表控件\n");
		return -1;      // 未能创建
	}
	LOGFONT lf = { 0 };
	m_List.GetFont()->GetLogFont(&lf);
	lf.lfHeight = -14;
	//DBG_MSG("m_WndPropList default font size: %s %d, %d\n", WstringToString(lf.lfFaceName).c_str(), lf.lfHeight, lf.lfWidth);
	const auto hFont = ::CreateFontIndirectW(&lf);
	m_List.SetFont(CFont::FromHandle(hFont));
		
	return 0;

}

void CListGroupView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	m_List.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

CListGroupView* CListGroupView::CreateView(PLIST_GROUP_INFO  pListGInfo)
{
	BOOL            bRet = FALSE;
	CListGroupView *tView = NULL;

//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( pListGInfo->nCreateType == TYPE_NORMAL_LIST_VIEW )
	{
		tView = new CListGroupView;
		if( tView )
		{
			bRet = tView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, pListGInfo->ListGroupRC, CWnd::FromHandle(pListGInfo->hParentWnd), 9123);
			if( bRet == FALSE )
			{
				delete tView;
				tView = NULL;
			}
		}
	}
	else if( pListGInfo->nCreateType == TYPE_MFC_TAB_VIEW )
	{

		int                  nIndex;
		CString              tn;
		PTAB_CTRL_CONTEXT    context = (PTAB_CTRL_CONTEXT)pListGInfo->pOtherCreateContext;
		CTabView *pTabView = (CTabView*)context->pTabCtrl;

	//	HINSTANCE   hInstOld = NULL;   
	//	hInstOld = AfxGetResourceHandle();

	//	HINSTANCE h = (HINSTANCE)GetModuleHandle("THUIInterface.dll");

	//	AfxSetResourceHandle(h); 
		tn = context->TabName;
		nIndex = pTabView->AddView(RUNTIME_CLASS(CListGroupView), tn, context->nIndex);
		tView = (CListGroupView*)pTabView->GetTabControl().GetTabWnd(nIndex);

	//	AfxSetResourceHandle(hInstOld); 

	}

	return tView;
}



int  CListGroupView::InsertGroup(int nGroupIndex, int nGroupID, PCHAR  pGroupName)
{
	PWCHAR  pBuf;
	LVGROUP lg = {0};
	lg.cbSize = sizeof(LVGROUP);
	lg.state = LVGS_NORMAL;
	lg.stateMask = LVGS_NORMAL;
	lg.mask = LVGF_GROUPID | LVGF_HEADER | LVGF_STATE | LVGF_ALIGN;
	lg.uAlign = LVGA_HEADER_LEFT;
	lg.iGroupId = nGroupID;
	

	pBuf = ANSIToUnicode(std::string(pGroupName));
	if( pBuf == NULL )
		return -1;

	lg.pszHeader = pBuf;
	lg.cchHeader = strlen(pGroupName) + 1;
	
	int nRet =  m_List.InsertGroup(nGroupIndex,&lg);

	delete [] pBuf;
	return nRet;
}

int  CListGroupView::InsertIconItem(int nItemIndex, int nGroupId, int nGrapIndex, ITEM_CELL_INFO* pCellInfo)
{
	LV_ITEM lvItem = {0};

	lvItem.iImage = nGrapIndex;
	lvItem.mask = LVIF_IMAGE | LVIF_GROUPID;
	lvItem.iItem = nItemIndex;
	lvItem.iGroupId = nGroupId;
	int nIndex =  m_List.InsertItem(&lvItem);
	if(nIndex != -1)
	{
		m_List.SetItemData(nIndex,(DWORD)pCellInfo);
	}

	return nIndex;
}


int  CListGroupView::AddHBitmapToImageList(HBITMAP hBitmap,ULONG nGrapIndex,bool isMore)
{
/*	BITMAP_QUE   b_que, old_b;

	b_que.iBitmap = new CBitmap;
	b_que.iBitmap->Attach(hBitmap);
	if( nGrapIndex >= Que_GetCounter(m_BitmapQue) )
	{
		Que_InsertData(m_BitmapQue, &b_que);
		m_ImageList.Add(b_que.iBitmap, RGB(0,0,0));
	}
	else
	{
		m_ImageList.Replace(nGrapIndex, b_que.iBitmap,RGB(0,0,0));
		if( Que_ReplaceUserData(m_BitmapQue, nGrapIndex, &b_que, &old_b) == TRUE )
		{
			DeleteObject(old_b.iBitmap->Detach());
			delete old_b.iBitmap;
		}
	}*/	
	if (isMore)//BITMAP 不显示，只能用add ico的方法
	{
		m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_MORE));
		return Que_GetCounter(m_BitmapQue) - 1;
	}
	BITMAP_QUE   b_que, old_b;
	b_que.iBitmap = new CBitmap;
	b_que.iBitmap->Attach(hBitmap);
	Que_InsertData(m_BitmapQue, &b_que);
	m_ImageList.Add(b_que.iBitmap, RGB(0, 0, 0));	
	return Que_GetCounter(m_BitmapQue) - 1;

}

void  CListGroupView::DeleteAllItem()
{
	int nCounter = 0;

	m_List.SetRedraw(FALSE);

	nCounter = m_List.GetItemCount();
	for(int i = 0 ; i < nCounter ; i ++)
	{
		ITEM_CELL_INFO* point = (ITEM_CELL_INFO*)m_List.GetItemData(i);
		if(point != NULL)
		{
			delete point;
			point = NULL;
		}
	}

	m_List.DeleteAllItems();
	nCounter = m_ImageList.GetImageCount();
	for( nCounter -- ; nCounter >= 0 ; nCounter -- )
		m_ImageList.Remove(nCounter);

	BITMAP_QUE   b_que;

	while( Que_GetHeaderData(m_BitmapQue, &b_que) )
	{
		DeleteObject(b_que.iBitmap->Detach());
		delete b_que.iBitmap;
	}
	DBG_MSG("继续运行？？2\n");
	m_List.SetRedraw(TRUE);

}

void CListGroupView::EmptyGroupName()
{
	CString csTemp;
	int nCount = m_List.GetGroupCount();
	for (int i = 0; i < nCount; i++)
	{
		LVGROUP gInfo = { 0 };
		gInfo.cbSize = sizeof(LVGROUP);
		wchar_t wstrHeadGet[45] = { 0 };
		gInfo.cchHeader = 45;
		gInfo.pszHeader = wstrHeadGet;
		gInfo.mask = (LVGF_ALIGN | LVGF_STATE | LVGF_HEADER | LVGF_GROUPID);
		gInfo.state = LVGS_NORMAL;
		gInfo.uAlign = LVGA_HEADER_LEFT;
		m_List.GetGroupInfoByIndex(i, &gInfo);


		gInfo.mask = (LVGF_ALIGN | LVGF_STATE | LVGF_HEADER);
		AfxExtractSubString(csTemp, (LPCTSTR)gInfo.pszHeader, 0, '(');

		wsprintfW(gInfo.pszHeader, L"%s(%d)", csTemp, 0);
		gInfo.cchHeader = sizeof(gInfo.pszHeader);
		m_List.SetGroupInfo(gInfo.iGroupId, &gInfo);
	}
}

void CListGroupView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	ULONG            nTaskID;
	ITEM_CELL_INFO * ItemCellInfo;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    SIZE  picSize;
	picSize.cx =1008;
	picSize.cy =672;

	nTaskID = m_RecThread->GetCurTaskID();
	if( nTaskID == 0xffffffff )
		goto d_exit_loc;

	if( QUERY_BUSY(m_Busy) == TRUE || m_ThumbnailStart == FALSE )
		goto d_exit_loc;


	if (pNMItemActivate->iItem != -1)
	{
		ItemCellInfo = (ITEM_CELL_INFO*)m_List.GetItemData(pNMItemActivate->iItem);
		if( ItemCellInfo == NULL )
			goto d_exit_loc;
		if (strcmp(ItemCellInfo->sRecRelativePath,"more") == 0)//此item为more图标
		{
			goto d_exit_loc;
		}
	   //Thumbnail_ShowPic(nTaskID, ItemCellInfo, &m_PicSize);//20180730
	   Thumbnail_ShowPic(nTaskID, ItemCellInfo, &picSize);
	}
	else
	{
		if( m_List.GetItemCount() == 0 )
			Thumbnail_ShowPic(nTaskID, NULL, &picSize);//Thumbnail_ShowPic(nTaskID, NULL, &m_PicSize);
	}


d_exit_loc:
	*pResult = 0;

}

void CListGroupView::OnNMRclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	ITEM_CELL_INFO* pCellInfo;
	if (pNMItemActivate->iItem != -1)
	{
		CPoint point;
		pCellInfo = (ITEM_CELL_INFO*)m_List.GetItemData(pNMItemActivate->iItem);
		if (strcmp(pCellInfo->sRecRelativePath,"more") == 0)
		{
			return;
		}
		if( QUERY_BUSY(m_Busy) == FALSE )
		{
			GetCursorPos(&point);
			TrackPopupMenu(m_MenuDisp_Des[0].hMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON,point.x, point.y,0,GetSafeHwnd(),NULL);
		}
	}


	*pResult = 0;

}

void CListGroupView::OnNMLclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	ITEM_CELL_INFO* pCellInfo;
	if (pNMItemActivate->iItem != -1)
	{
		CPoint point;
		pCellInfo = (ITEM_CELL_INFO*)m_List.GetItemData(pNMItemActivate->iItem);
		if (strcmp(pCellInfo->sRecRelativePath,"more") == 0)
		{
			ExpandCell.push_back(pCellInfo->cType);
			ShowTaskIcon(GetCurTaskID(), true);
		}
	}
}

void CListGroupView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;
}

wchar_t* CListGroupView::ANSIToUnicode( const std::string& str )
{
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0 );  

	wchar_t *  pUnicode = NULL;  
	pUnicode = new  wchar_t[unicodeLen+1];  
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  
	::MultiByteToWideChar(CP_ACP,
		                  0,
		                  str.c_str(),
		                  -1,
		                  (LPWSTR)pUnicode,
		                  unicodeLen ); 


	return  pUnicode;  
}


// 修改细胞种类
BOOL CListGroupView::ModifyCellType(int ncType, CELL_IDENTIFY *cIdentify)
 {
	int              iItem = -1;
	BOOL             bFlash = FALSE;
	USHORT           nOldCType;
	LV_ITEM          lvItem;
	KSTATUS          status = STATUS_SUCCESS;
	POSITION         pos;
	ITEM_CELL_INFO  *pCellInfo = NULL;
	CELL_DETAIL_INFO Cell_datail_info;
	MODIFY_CELL_INFO modify_cell_info = {0};

	ULONG           nTaskID = m_RecThread->GetCurTaskID();
	DBG_MSG("NOW TaskID: %d\n",nTaskID);

	if( nTaskID == 0xffffffff )
		return bFlash;

	modify_cell_info.nID = nTaskID;

	pos = m_List.GetFirstSelectedItemPosition();

	while (NULL != pos)
	{
		iItem = m_List.GetNextSelectedItem(pos);
		if(iItem != -1)
		{
			pCellInfo = (ITEM_CELL_INFO*) m_List.GetItemData(iItem);
			if( pCellInfo == NULL )
				continue;

			memset(&Cell_datail_info,0,sizeof(CELL_DETAIL_INFO));
			nOldCType = pCellInfo->cType;
			Cell_datail_info.cType = ncType;
			memcpy(&Cell_datail_info.identify,&pCellInfo->identify, sizeof(CELL_IDENTIFY));
			CopyRect(&Cell_datail_info.rc,&pCellInfo->rc);

			if( nOldCType == ncType )
				continue;

			status = THInterface_ModifyRecCellInfo(pCellInfo->sRecRelativePath,
				                                   pCellInfo->nGrapIndex,
				                                   pCellInfo->nGrapSize,
				                                   &Cell_datail_info);

			if( status != STATUS_SUCCESS )
				continue;

			ZeroMemory(&lvItem, sizeof(lvItem));
			memcpy(&pCellInfo->identify, &Cell_datail_info.identify, sizeof(CELL_IDENTIFY));
			lvItem.iItem    = iItem;
			lvItem.mask     = LVIF_GROUPID;
			lvItem.iGroupId = ncType;
			m_List.SetItem(&lvItem);
			bFlash = TRUE;
			pCellInfo->cType = ncType;
			if( cIdentify )
				memcpy(cIdentify, &Cell_datail_info.identify, sizeof(CELL_IDENTIFY));

			if( m_ParentWnd )
			{
// 				::SendMessage(m_ParentWnd, CHANGED_CELL_MSG, (WPARAM)nOldCType, FALSE);
// 				::SendMessage(m_ParentWnd, CHANGED_CELL_MSG, (WPARAM)ncType, TRUE);
				modify_cell_info.nCellType = nOldCType;
				::SendMessage(m_ParentWnd, CHANGED_CELL_MSG, (WPARAM)&modify_cell_info, FALSE);
				
				modify_cell_info.nCellType = ncType;
				::SendMessage(m_ParentWnd, CHANGED_CELL_MSG, (WPARAM)&modify_cell_info, TRUE);

			}
		}
	}

	if( bFlash && m_ParentWnd )
		::SendMessage(m_ParentWnd, SAVE_TIME_MSG, (WPARAM)0, (LPARAM)0);

	return bFlash;
}

BOOL  CListGroupView::GetSelectdInfo(PULONG &pSelArray, int &nSelCounter)
{
	int        j, nItem;
	POSITION   pos;

	nSelCounter = m_List.GetSelectedCount();
	if( nSelCounter == 0 )
		return FALSE;

	pSelArray = (PULONG)new ULONG[nSelCounter];
	if( pSelArray == NULL )
		return FALSE;
	
	pos = m_List.GetFirstSelectedItemPosition();

	for( j = 0 ; j < nSelCounter ; j ++ )
	{
		nItem = m_List.GetNextSelectedItem(pos);
		if( nItem == -1 )
		{
			delete [] pSelArray;
			return FALSE;
		}
		pSelArray[j] = nItem;
	}
	
	return TRUE;
}

BOOL CListGroupView::DeleteCell(void)
{
	int              nSelCounter;
	BOOL             bFlash = FALSE;
	PULONG           pSelArray;
	KSTATUS          status = STATUS_UNKNOW_ERROR;
	ITEM_CELL_INFO  *pCellInfo = NULL;
	MODIFY_CELL_INFO mci;
	REC_GRAP_RECORD  grap;

	if( GetSelectdInfo(pSelArray, nSelCounter) == FALSE )
		return FALSE;

	m_List.SetRedraw(FALSE);

	mci.nID = m_RecThread->GetCurTaskID();

	for( nSelCounter-- ; nSelCounter >=0 ; nSelCounter-- )
	{
		pCellInfo = (ITEM_CELL_INFO*) m_List.GetItemData(pSelArray[nSelCounter]);
		if(pCellInfo)
		{
			memset(&grap,0,sizeof(REC_GRAP_RECORD));
			grap.rGrapSize = pCellInfo->nGrapSize;
			grap.rIndex    = pCellInfo->nGrapIndex;
			status = THInterface_DeleteRecCellInfo(pCellInfo->sRecRelativePath,
				                                  &grap,
				                                  &pCellInfo->identify,
				                                  FALSE);

			if(status == STATUS_SUCCESS)
			{
				if( m_ParentWnd )
				{
					mci.nCellType = pCellInfo->cType;
					::SendMessage(m_ParentWnd, CHANGED_CELL_MSG, (WPARAM)&mci, FALSE); //删除
				}

				delete pCellInfo;
				m_List.DeleteItem(pSelArray[nSelCounter]);
				bFlash = TRUE;
				
			}
		}
	}

	m_List.SetRedraw(TRUE);
	delete [] pSelArray;


	if( bFlash == TRUE )
	{
		if( m_ParentWnd )
			::SendMessage(m_ParentWnd, SAVE_TIME_MSG, NULL, NULL);
	}

	return (status == STATUS_SUCCESS);

}




void CListGroupView::ScrollGroup(int nGroupId)
{
	RECT rect;
	int  y = 0;

	if (nGroupId <= 0)
	{
		m_List.GetGroupRect(1, &rect, LVGGR_GROUP);
		y = rect.top;
	}
	else
	{
		m_List.GetGroupRect(nGroupId, &rect, LVGGR_GROUP);
		y = rect.top;
	}

	CSize sz(0, y);
	m_List.Scroll(sz);

}

BOOL  CListGroupView::SelectCellItem(PCELL_DETAIL_INFO cell_info, ULONG nGrapIndex, UCHAR nGrapSize)
{
	int             nCounter = m_List.GetItemCount(), n1, k;
	PULONG          pArray;
	ITEM_CELL_INFO *ItemInfo;


	if( GetSelectdInfo(pArray, n1) == TRUE ) //用户只关心当前所选,取消以前的选择
	{
		for( k = 0 ; k < n1 ; k ++ )
			m_List.SetItemState(pArray[k],0,LVIS_SELECTED|LVIS_FOCUSED);

		delete [] pArray;
	}

	if( cell_info == NULL )
		return TRUE;

	for( int j = 0 ; j < nCounter ; j ++ )
	{
		ItemInfo = (ITEM_CELL_INFO *)m_List.GetItemData(j);
		if( ItemInfo == NULL )
			continue;

		if( ItemInfo->cType == cell_info->cType && memcmp(&ItemInfo->identify, &cell_info->identify,sizeof(CELL_IDENTIFY)) == 0 &&
			ItemInfo->nGrapIndex == nGrapIndex && ItemInfo->nGrapSize == nGrapSize )
		{
			m_List.SetItemState(j,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED); 
			m_List.SetSelectionMark(j);
			return TRUE;
		}
	}
	return FALSE;
}


void CListGroupView::InitializeView( PLIST_GROUP_INFO pListGInfo )
{

	m_BitmapQue = Que_Initialize(sizeof(BITMAP_QUE));
	m_ImageList.Create(pListGInfo->CellGrapSZ.cx,  pListGInfo->CellGrapSZ.cy, ILC_COLOR24, 0, 0);
	m_List.SetImageList(&m_ImageList,LVSIL_NORMAL);

	//设置风格
	m_List.ModifyStyle(LVS_TYPEMASK, LVS_ICON | LVS_SHOWSELALWAYS);
	m_List.SetExtendedStyle(LVS_EX_BORDERSELECT);
	m_List.SetIconSpacing(pListGInfo->CellGrapSZ.cx + 8 ,pListGInfo->CellGrapSZ.cy + 8);
	
	m_List.RemoveAllGroups();
	m_List.EnableGroupView(TRUE);
	memcpy(&m_CGSize, &pListGInfo->CellGrapSZ, sizeof(SIZE));
	m_RecThread = new CRecThread;
	if( m_RecThread == NULL )
		return;

	m_RecThread->StartRecThread(rogress_request,rogress_callback,this);
}

void CListGroupView::MenuCommand( DWORD nMenuCmdID )
{
	MENU_COMMAND_DESCRIPTION   MenuCmdDes;

	if( SearchMenuCmd(nMenuCmdID, MenuCmdDes) == FALSE )
		return;

	if( MenuCmdDes.nMAction == CHANGED_CELL_TYPE_ACTION )
		ModifyCellType(MenuCmdDes.nAdditionData);
	else if( MenuCmdDes.nMAction == DELETE_CELL_TYPE_ACTION )
		DeleteCell();
}

static
VOID
CALLBACK
ReferePreRequestCB( 
CListGroupView  *lv
)
{
	lv->CellTypeNum.clear();
	lv->EmptyGroupName();
	lv->DeleteAllItem();
}


VOID 
CALLBACK 
rogress_request(
PVOID  pContext,
ULONG  nTaskID
)
{
	KSTATUS           status;
	CRecTask          rec;
	CListGroupView   *list_view = (CListGroupView*)pContext;
	PTASK_REC_INFO    task_rec_info;

	list_view->UpdateTaskPath(nTaskID);
	status = rec.GetRecGrapTaskInfoWhenFinish(list_view->m_TaskPath,task_rec_info);

	if( status != STATUS_SUCCESS )
		return;

	SET_BUSY(list_view->m_Busy,1);

	for( unsigned int i = 0 ; i < task_rec_info->tGrapCounter ; i ++ )
	{
		list_view->m_RecThread->InsertRecGrapRequest(nTaskID,
			                                         list_view->m_TaskPath,
			                                         task_rec_info->gInfo[i].rIndex,
			                                         task_rec_info->gInfo[i].rGrapSize,
			                                         MAKE_PROGRESS_INFO(task_rec_info->tGrapCounter,i));
	} 

	THInterface_FreeSysBuffer(task_rec_info);
}



static   int   nFlashPre = 0;

VOID CALLBACK 
rogress_callback(
PCHAR  pImageFolder,
USHORT nGrapIndex,
UCHAR  nGrapSize,
PVOID  pContext,
ULONG  nProgressInfo
)
{ 

	int                 nIndex, nGroupId, nBitmapIndex;
	PSIZE               pGPX = NULL;
	USHORT              nGroupCounter, nCurProgress;
	CRecTask            RecTask;
	ITEM_CELL_INFO*     pCellInfo;
	CListGroupView*     list_view = (CListGroupView*)pContext;
	PCELL_FULL_INFO_EX cell_full_info;

	memset(&cell_full_info,0,sizeof(cell_full_info));

	if( list_view->m_PicSize.cx == 0 || list_view->m_PicSize.cy == 0 )
	{
		pGPX = &list_view->m_PicSize;
		
	}

	KSTATUS status = RecTask.GetPickupRecCellGrap((PCHAR)pImageFolder,
		                                           nGrapIndex,
		                                           nGrapSize,
		                                           cell_full_info,
												   list_view,
												   TRUE,
												   pGPX);
	if( pGPX )
	{
		DBG_MSG("********************SET m_picSize******************** = %u\n", pGPX->cx);
	}


	if(status != STATUS_SUCCESS)
	{
		DBG_MSG("rogress_callback->GetPickupRecCellGrap Error...\n");
		goto pos_loc;
	}

	list_view->m_List.SetRedraw(FALSE);
	int i = 0;
	if(status == STATUS_SUCCESS)
	{
		nFlashPre ++;

		int showCellNum = list_view->ShowCellNumByTotal(cell_full_info->cCellCounter);
		for (i = 0; i < showCellNum; i++)
		{
			if( cell_full_info->cInfo[i].hBitmap == NULL )
				continue;
			
			pCellInfo = new ITEM_CELL_INFO;
			if( pCellInfo == NULL )
				continue;

			if (false == list_view->CountCellTypeNum(cell_full_info->cInfo[i].cType))
			{
				continue;
			}
			nGroupId = cell_full_info->cInfo[i].cType;
			nIndex = list_view->m_List.GetItemCount();
			nBitmapIndex = list_view->AddHBitmapToImageList(cell_full_info->cInfo[i].hBitmap, nIndex);
			pCellInfo->cType = cell_full_info->cInfo[i].cType;
			pCellInfo->nGrapIndex = nGrapIndex;
			pCellInfo->nGrapSize = nGrapSize;
			memcpy(&pCellInfo->identify,&cell_full_info->cInfo[i].identify,sizeof(CELL_IDENTIFY));
			strcpy(pCellInfo->sRecRelativePath,pImageFolder);
			CopyRect(&pCellInfo->rc,&cell_full_info->cInfo[i].rc);
			list_view->InsertIconItem(nIndex,nGroupId,nBitmapIndex,pCellInfo);
		} // end for
		RecTask.FreeCellFullInfo(cell_full_info);
	}
pos_loc:
	if( list_view->m_ParentWnd )
	{
		int   pos;
		float f;

		GET_PROGRESS_INFO(nProgressInfo, nGroupCounter, nCurProgress);

		if( nGroupCounter )
		{
			f   = (float)(nCurProgress + 1) / (float)nGroupCounter;
			pos = (int)(f * 100.0f);

			if( list_view->m_ParentWnd )
				SendMessage(list_view->m_ParentWnd, SHOW_CELL_PROGRESS_MSG, (WPARAM)pos, NULL);
		}
		
	}

	BOOL bFinally = (nGroupCounter && ((nCurProgress + 1) >= nGroupCounter));

	if( bFinally || nFlashPre >= FLASH_PRE )
	{
		nFlashPre = 0;
		list_view->m_List.SetRedraw(TRUE);

		if( bFinally )
		{
			list_view->InsertMOREImageList(list_view->m_List.GetItemCount(), cell_full_info);//全部图片显示完毕
			SET_BUSY(list_view->m_Busy, 0);
			::SendMessage(list_view->m_ParentWnd, END_SHOW_CELL_MSG, NULL, NULL);
		}	
	}
}
bool CListGroupView::CountCellTypeNum(int type)
{
	int count = 1, isExist = 0;
	std::vector<int>::iterator it;
	isExist = CellTypeNum.count(type);
	if (isExist == 0)
	{
		CellTypeNum.insert(std::pair<int, int>(type, count));
	}
	else
	{
		count = CellTypeNum.at(type);
		CellTypeNum.at(type) = ++count;
	}
	for (it = ExpandCell.begin(); it != ExpandCell.end();it++)//如果这个细胞类型被展开了，则不做限制显示
	{
		if (*it == type)
		{
			yShowType = type;
			return true;
		}
	}
	if (count > INI_SHOW_CELL_NUM)
	{
		return false;
	}
	return true;
}
BOOL CListGroupView::ShowTaskIcon(ULONG nTaskID, BOOL bForceFlush)
{
	if( m_RecThread == NULL )
		return FALSE;

	if( bForceFlush == FALSE )
	{
	    if( GetCurTaskID() == nTaskID )
			return TRUE;
	}
	
	if( m_ParentWnd )
		::SendMessage(m_ParentWnd, BEGIN_SHOW_CELL_MSG, NULL, NULL);
	DBG_MSG("继续运行？？\n");
	m_RecThread->RefereNewRequest(nTaskID, (REFER_PRE_REQUEST_CALLBACK)ReferePreRequestCB, this);
	return TRUE;
}

void CListGroupView::UpdateTaskPath( DWORD nTaskID )
{
	char  Temp[40];

	sprintf(Temp, "%u", nTaskID);
	strcpy(m_TaskPath, m_ImageFolder);
	PathAppendA(m_TaskPath, Temp);

}

BOOL CListGroupView::AddCell( PITEM_CELL_INFO item_cell, HBITMAP hBitmap )
{
	int                nIndex, nGroupId, nBitmapIndex;
	KSTATUS            status;
	ITEM_CELL_INFO    *ic;
	CELL_DETAIL_INFO   cdi = {0};
	MODIFY_CELL_INFO   mci;

	if(  VaildCellType(item_cell->cType) == FALSE )
		return FALSE;

	mci.nID = m_RecThread->GetCurTaskID();

	cdi.cType = item_cell->cType;
	CopyRect(&cdi.rc, &item_cell->rc);
	status = THInterface_AddRecCellInfo(item_cell->sRecRelativePath, item_cell->nGrapIndex, item_cell->nGrapSize, &cdi);
	if( status != STATUS_SUCCESS )
		return FALSE;

	ic = new ITEM_CELL_INFO;
	if( ic == NULL )
		return FALSE;

	nGroupId = item_cell->cType;

	memcpy(&item_cell->identify, &cdi.identify, sizeof(CELL_IDENTIFY));
	nIndex = m_List.GetItemCount();
	nBitmapIndex  = AddHBitmapToImageList(hBitmap,nIndex);
	memcpy(ic, item_cell, sizeof(ITEM_CELL_INFO));
	InsertIconItem(nIndex,nGroupId,nBitmapIndex,ic);
	if( m_ParentWnd )
	{
		mci.nCellType = item_cell->cType;

		::SendMessage(m_ParentWnd, CHANGED_CELL_MSG, (WPARAM)&mci, (LPARAM)TRUE);
		::SendMessage(m_ParentWnd, SAVE_TIME_MSG, NULL, NULL);
	}
	
	return TRUE;

}

void CListGroupView::InsertMOREImageList(int index, PVOID cell_info)
{
	if (!CellTypeNum.size())
	{
		return;
	}
	int nCount = m_List.GetGroupCount();

	std::map<int,int>::iterator it;
	for (it = CellTypeNum.begin(); it != CellTypeNum.end(); it++)
	{
		for (int i = 0; i < nCount; i++)
		{
			LVGROUP gInfo = { 0 };
			gInfo.cbSize = sizeof(LVGROUP);
			wchar_t wstrHeadGet[45] = { 0 };
			gInfo.cchHeader = 45;
			gInfo.pszHeader = wstrHeadGet;
			gInfo.mask = (LVGF_ALIGN | LVGF_STATE | LVGF_HEADER | LVGF_GROUPID);
			gInfo.state = LVGS_NORMAL;
			gInfo.uAlign = LVGA_HEADER_LEFT;
			m_List.GetGroupInfoByIndex(i, &gInfo);
			if (gInfo.iGroupId == it->first)
			{
				CString csTemp;
				gInfo.mask = (LVGF_ALIGN | LVGF_STATE | LVGF_HEADER);
				AfxExtractSubString(csTemp, (LPCTSTR)gInfo.pszHeader, 0, '(');

				wsprintfW(gInfo.pszHeader, L"%s(%d)", csTemp, it->second);
				gInfo.cchHeader = sizeof(gInfo.pszHeader);
				m_List.SetGroupInfo(it->first, &gInfo);
			}
		}
		if (it->second <= INI_SHOW_CELL_NUM || yShowType == it->first)//当此细胞类型不足INI_SHOW_CELL_NUM或者刚刚被展开，则不添加more图标
		{
			continue;
		}
		ITEM_CELL_INFO* pCellInfo;
		pCellInfo = new ITEM_CELL_INFO;	
		int nBitmapIndex = AddHBitmapToImageList(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_MORE)), index++,true);
		pCellInfo->cType = it->first;
		pCellInfo->nGrapIndex = nBitmapIndex++;
		strcpy(pCellInfo->sRecRelativePath, "more");
		InsertIconItem(index, it->first, nBitmapIndex, pCellInfo);
	}
	if (yShowType != -1)
	{
		CRect rect; int y = 0;
		m_List.GetGroupRect(yShowType, &rect, LVGGR_GROUP);
		y = rect.top;
		CSize sz(0, y);//滚动到展开位置
		m_List.Scroll(sz);
		yShowType = -1;
	}
}

void CListGroupView::SetStartThumbnailResult( BOOL bResult )
{
	m_ThumbnailStart = bResult;
}

void CListGroupView::CreateList()
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_List.Create(WS_CHILD | WS_VISIBLE, rectDummy, this, IDC_ICON_LIST);


}

int CListGroupView::ShowCellNumByTotal(int totalNum)
{
	return totalNum;
}

LRESULT CListGroupView::OnCustomCreateMsg( WPARAM wparam, LPARAM lparam )
{
	CreateList();
	return 0;
}

ULONG CListGroupView::GetCurTaskID()
{
	return m_RecThread->GetCurTaskID();
}



