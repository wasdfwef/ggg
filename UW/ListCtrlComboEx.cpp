// ListCtrlComboEx.cpp : implementation file
//

#include "stdafx.h"
#include "UW.h"
#include "ListCtrlComboEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlComboEx

CListCtrlComboEx::CListCtrlComboEx()
{
    m_EditAbleArray.clear();
	m_ComboAbleArray.clear();
	m_iRow = -1;
	m_iCol = -1;
	m_iHeight = 0;
	m_pEdit = NULL;
}

CListCtrlComboEx::~CListCtrlComboEx()
{
}


void CListCtrlComboEx::ClearAllAbles()
{
	m_EditAbleArray.clear();
	m_ComboAbleArray.clear();
	m_iRow = -1;
	m_iCol = -1;
	m_pEdit = NULL;
}

IMPLEMENT_DYNAMIC(CListCtrlComboEx, CListCtrl)

BEGIN_MESSAGE_MAP(CListCtrlComboEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlComboEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
		ON_NOTIFY_EX(HDN_ENDTRACKA, 0, OnHeaderEndResize)
		ON_NOTIFY_EX(HDN_ENDTRACKW, 0, OnHeaderEndResize)
		ON_WM_MEASUREITEM_REFLECT()
		ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlComboEx message handlers

void CListCtrlComboEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	int nRow = (int)pLVCD->nmcd.dwItemSpec;
	
	*pResult = CDRF_DODEFAULT;
	
/*	// Allow column-traits to perform their custom drawing
	if (pLVCD->nmcd.dwDrawStage & CDDS_SUBITEM)
	{
		
		CComboBox* pCombo = GetCellColumnTrait(nRow, pLVCD->iSubItem);
		if (pCombo != NULL)
			return;	// Everything is handled by the column-trait
	}
	
	// Always perform drawing of cell-focus rectangle
	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult |= CDRF_NOTIFYITEMDRAW;
		break;
		
		// Before painting a row
	case CDDS_ITEMPREPAINT:
		{
			*pResult |= CDRF_NOTIFYPOSTPAINT;	// Ensure row-traits gets called
			*pResult |= CDRF_NOTIFYSUBITEMDRAW;	// Ensure column-traits gets called
		} break;
		
		// After painting the entire row
	case CDDS_ITEMPOSTPAINT:
		{
			;
		} break;
	}*/
}

BOOL CListCtrlComboEx::OnHeaderEndResize(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	int size = m_ComboAbleArray.size();
	int i;
	for(i=0;i<size;i++)
	{
		int nRow = m_ComboAbleArray[i].nRow;
		int nCol = m_ComboAbleArray[i].nCol;
		cgCComboBox *pCombo = m_ComboAbleArray[i].pCombo;
		CRect rect;
		GetCellRect(nRow,nCol,rect);
		rect.bottom -=  2*::GetSystemMetrics(SM_CXEDGE);
		pCombo->MoveWindow(rect);
		pCombo->SetWindowPos(NULL,		// not relative to any other windows
			0, 0,		// TopLeft corner doesn't change
			rect.Width(), (pCombo->GetCount()+1)*rect.Height(),   // existing width, new height
			SWP_NOMOVE | SWP_NOZORDER	// don't move box or change z-ordering.
			);
		pCombo->ShowWindow(TRUE);
	}
	Invalidate(FALSE);
	return FALSE;
}

void CListCtrlComboEx::SetItemHeight(int nHeight)
{
	m_iHeight = nHeight;
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED,0,(LPARAM)&wp);
}


BOOL CListCtrlComboEx::GetCellRect(int nRow, int nCol,CRect& rect)
{
	if (GetSubItemRect(nRow, nCol, LVIR_BOUNDS, rect)==FALSE)
		return FALSE;
	
	CRect colRect;
	if (GetHeaderCtrl()->GetItemRect(nCol, colRect)==FALSE)
		return FALSE;
	
	if (nCol==0)
	{
		// Fix bug where LVIR_BOUNDS gives the entire row for nCol==0
		CRect labelRect;
		if (GetSubItemRect(nRow, nCol, LVIR_LABEL, labelRect)==FALSE)
			return FALSE;
		
		rect.right = labelRect.right; 
		rect.left  = labelRect.right - colRect.Width();
	}
	else
	{
		// Fix bug when width is smaller than subitem image width
		rect.right = rect.left + colRect.Width();
	}
	
	return TRUE;
}

BOOL CListCtrlComboEx::IsEditAble(int nRow,int nCol)
{
	int i;
	int size = m_EditAbleArray.size();
	if(size > 0)
	{
		for(i=0;i<size;i++)
		{
			if(m_EditAbleArray[i].nRow == nRow && m_EditAbleArray[i].nCol == nCol)
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CListCtrlComboEx::IsComboAble(int nRow,int nCol)
{
	int i;
	int size = m_ComboAbleArray.size();
	if(size > 0)
	{
		for(i=0;i<size;i++)
		{
			if(m_ComboAbleArray[i].nRow == nRow && m_ComboAbleArray[i].nCol == nCol)
				return TRUE;
		}
	}
	return FALSE;
}

void CListCtrlComboEx::SetEditAble(int nRow,int nCol)
{
	stEditAble sam;
	sam.nRow = nRow;
	sam.nCol = nCol;
	m_EditAbleArray.push_back(sam);
}


void CListCtrlComboEx::SetComboAble(int nRow,int nCol,cgCComboBox *pCombo)
{
	stComboAble sam;
	sam.nRow = nRow;
	sam.nCol = nCol;
	sam.pCombo = pCombo;
	m_ComboAbleArray.push_back(sam);

	CRect rect;
	GetCellRect(nRow,nCol,rect);
	rect.bottom -=  2*::GetSystemMetrics(SM_CXEDGE);
	pCombo->SetWindowPos(NULL,		// not relative to any other windows
		0, 0,		// TopLeft corner doesn't change
		rect.Width(), (pCombo->GetCount()+1)*rect.Height(),   // existing width, new height
		SWP_NOMOVE | SWP_NOZORDER	// don't move box or change z-ordering.
		);
	pCombo->ShowWindow(TRUE);
}


void CListCtrlComboEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nRow, nCol;
	if(m_pEdit != NULL)
	{
		CString str;
		m_pEdit->GetWindowText(str);
		SetItemText(m_iRow,m_iCol,str);
		m_iRow = -1;
		m_iCol = -1;
		delete m_pEdit;
	//	m_pEdit->PostMessage(WM_CLOSE);
		m_pEdit = NULL;
	}
	LVHITTESTINFO lvhti = {0};
	lvhti.pt = point;
	nRow = ListView_SubItemHitTest(m_hWnd, &lvhti);	// SubItemHitTest is non-const
	nCol = lvhti.iSubItem;
	if (!(lvhti.flags & LVHT_ONITEM))
		nRow = -1;
	if(nRow == -1 || nCol == -1)
	{
		CListCtrl::OnLButtonDown(nFlags, point);
		return;
	}
	
	if(IsEditAble(nRow,nCol))
	{
		m_iRow = nRow;
		m_iCol = nCol;
		CRect rect;
		GetCellRect(nRow,nCol,rect);
		m_pEdit = new CEdit();
		m_pEdit->Create(WS_CHILD|ES_LEFT|ES_AUTOHSCROLL,rect,this,0);
		CString str;
		str = GetItemText(nRow,nCol);
		m_pEdit->SetWindowText(str);
		m_pEdit->ShowWindow(SW_SHOW);
		m_pEdit->SetFocus();
		return;
	}


	CListCtrl::OnLButtonDown(nFlags,point);

}


void CListCtrlComboEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if(m_iHeight>0)
	{
		lpMeasureItemStruct->itemHeight = m_iHeight;
	}


}

void CListCtrlComboEx::PreSubclassWindow()
{
	ModifyStyle(0,LVS_OWNERDRAWFIXED);
	CListCtrl::PreSubclassWindow();

}

void CListCtrlComboEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int nItem = lpDrawItemStruct->itemID;
	if(nItem == -1)
		return;
	CRect rcCol = lpDrawItemStruct->rcItem;
	CString sText;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int nOldDCMode = pDC->SaveDC();
	LVITEM item;
	item.iItem = nItem;
	item.iSubItem = 0;
	item.mask = LVIF_IMAGE|LVIF_STATE;
	item.stateMask = 0xFFFF;
	GetItem(&item);
	BOOL bSelected = item.state & LVIS_SELECTED;
	COLORREF color = ::GetSysColor(COLOR_WINDOW);
	if(bSelected)
	{
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		color = ::GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		pDC->SetBkColor(color);
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	}
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT|LVCF_WIDTH;
	rcCol.right = rcCol.left;
	for(int nCol=0;GetColumn(nCol,&lvc);nCol++)
	{
		rcCol.left = rcCol.right;
		rcCol.right = rcCol.left + GetColumnWidth(nCol);
		HPEN hOldPen = (HPEN)::SelectObject(lpDrawItemStruct->hDC,::CreatePen(PS_SOLID,1,color));
		HBRUSH hOldBrush = (HBRUSH)::SelectObject(lpDrawItemStruct->hDC,::CreateSolidBrush(color));
		::Rectangle(lpDrawItemStruct->hDC,rcCol.left-1,rcCol.top-1,rcCol.right,rcCol.bottom);
		::DeleteObject(SelectObject(lpDrawItemStruct->hDC,hOldBrush));
		::DeleteObject(SelectObject(lpDrawItemStruct->hDC,hOldPen));
		sText = MakeShortString(pDC,GetItemText(nItem,nCol),rcCol.Width(),3);
		pDC->DrawText(sText,CRect::CRect(rcCol.left+3,rcCol.top,rcCol.right,rcCol.bottom),DT_LEFT|DT_VCENTER|DT_SINGLELINE );

	}
	pDC->RestoreDC(nOldDCMode);


}

LPCTSTR CListCtrlComboEx::MakeShortString(CDC *pDC,LPCTSTR lpszLong,int nColumnLen,int nOffset)
{
	static const _TCHAR szThreeDots[] = _T("...");
	int nStringLen = lstrlen(lpszLong);
	if(nStringLen == 0 ||
		(pDC->GetTextExtent(lpszLong,nStringLen).cx+nOffset) <= nColumnLen)
	{
		return(lpszLong);
	}
	static _TCHAR szShort[MAX_PATH];
	lstrcpy(szShort,lpszLong);
	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i=nStringLen-1; i>0; i--)
	{
		szShort[i] = 0;
		if((pDC->GetTextExtent(szShort,i).cx+nAddLen+nOffset) <= nColumnLen)
		{
			break;
		}
	}
	lstrcat(szShort,szThreeDots);
	return(szShort);
}