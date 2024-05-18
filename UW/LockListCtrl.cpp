// LockListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "LockListCtrl.h"

typedef struct positiveID
{
	int id;
	int active;

}positiveID;

#define	POSITIVETEXT RGB(178,34,34)
#define UNCERTAINTEXT RGB(0,139,69)
#define INACTIVETEXT RGB(180,180,180)

extern std::vector<positiveID> positiveUDCID;
extern std::vector<positiveID> positiveUSID;

#define POSITIVESTATUS 1
#define NEGATIVESTATUS 2
#define INACTIVESTATUS 3
// CLockListCtrl

IMPLEMENT_DYNAMIC(CLockListCtrl, CListCtrl)

CLockListCtrl::CLockListCtrl()
{
	m_ColomnLockMask = 0xFFFFFFFF;
	m_nRowHeight = 30;
}

CLockListCtrl::~CLockListCtrl()
{
}

BEGIN_MESSAGE_MAP(CLockListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CLockListCtrl::OnNMCustomdraw)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()



// CLockListCtrl 消息处理程序


BOOL CLockListCtrl::SetColomnLockMask(unsigned int nIndex, BOOL bMask)
{
	int nMaxPos = sizeof(m_ColomnLockMask) * 8 - 1;
	if (nIndex <= nMaxPos)
	{
		if (bMask)
		{
			m_ColomnLockMask = m_ColomnLockMask | (0x1 << nIndex);
		}
		else
		{
			m_ColomnLockMask = m_ColomnLockMask & ~(0x1 << nIndex);
		}
		return TRUE;
	}
	return FALSE;
}

void CLockListCtrl::SetListColor(COLORREF odd, COLORREF even)
{
	m_OddColor = odd;
	m_EvenColor = even;
}

BOOL CLockListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO:  在此添加专用代码和/或调用基类
	HD_NOTIFY   *pHDNotify = (HD_NOTIFY*)lParam;
	int nMinSize = 0;
	int nMaxPos = sizeof(m_ColomnLockMask) * 8 - 1;
	if (pHDNotify->iItem < nMaxPos)
	{
		if (!((m_ColomnLockMask >> (pHDNotify->iItem)) & 0x1))
		//if (pHDNotify->iItem == 12)
		{
			switch (pHDNotify->hdr.code)
			{
				//   How   to   force   a   minimum   column   width   Cees   Mechielsen.   
				//   http://www.codeguru.com/mfc/comments/866.shtml   
			case   HDN_ITEMCHANGINGA:
			case   HDN_ITEMCHANGINGW:
			case   HDN_ENDTRACK:
			{
				if (pHDNotify->pitem->mask   &   HDI_WIDTH   &&
					pHDNotify->pitem->cxy < nMinSize   &&
					pHDNotify->pitem->cxy >= 0)
				{
					pHDNotify->pitem->cxy = nMinSize; //   Set   the   column   width   
				}
			}
			break;

			//   Prevent   CListCtrl   column   resizing   
			case   HDN_DIVIDERDBLCLICKA:
			case   HDN_DIVIDERDBLCLICKW:
			case   HDN_BEGINTRACKW:
			case   HDN_BEGINTRACKA:
			{
				*pResult = TRUE;  //   disable   tracking   
				return   TRUE;

			}
			}
		}
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}


void CLockListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*> (pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;

	if (IsPositiveID((int)pLVCD->nmcd.lItemlParam, true) == POSITIVESTATUS && IsPositiveID((int)pLVCD->nmcd.lItemlParam, false) == POSITIVESTATUS)//US、UDC均为阳性
	{
		pLVCD->clrText = POSITIVETEXT;
		*pResult = CDRF_NEWFONT;
	}
	else if (IsPositiveID((int)pLVCD->nmcd.lItemlParam, true) == NEGATIVESTATUS && IsPositiveID((int)pLVCD->nmcd.lItemlParam, false) == POSITIVESTATUS
		|| IsPositiveID((int)pLVCD->nmcd.lItemlParam, true) == POSITIVESTATUS && IsPositiveID((int)pLVCD->nmcd.lItemlParam, false) == NEGATIVESTATUS)//US和UDC结果不匹配
	{
		pLVCD->clrText = UNCERTAINTEXT;
/*		pLVCD->clrText = POSITIVETEXT;*/
		*pResult = CDRF_NEWFONT;
	}
	else if (IsPositiveID((int)pLVCD->nmcd.lItemlParam, true) == POSITIVESTATUS && IsPositiveID((int)pLVCD->nmcd.lItemlParam, false) == INACTIVESTATUS
		|| IsPositiveID((int)pLVCD->nmcd.lItemlParam, false) == POSITIVESTATUS && IsPositiveID((int)pLVCD->nmcd.lItemlParam, true) == INACTIVESTATUS)//US或UDC只开了一种，则以开启的项目结果为准
	{
		pLVCD->clrText = POSITIVETEXT;
		*pResult = CDRF_NEWFONT;
	}
	else if (IsPositiveID((int)pLVCD->nmcd.lItemlParam, true) == INACTIVESTATUS && IsPositiveID((int)pLVCD->nmcd.lItemlParam, false) == INACTIVESTATUS)
	{
		pLVCD->clrText = INACTIVETEXT;
		*pResult = CDRF_NEWFONT;
	}
	else//均为阴性
	{
		*pResult = CDRF_DODEFAULT;
	}

	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}

	//改变背景颜色
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
// 	CString str;
// 	str = GetItemText(pLVCD->nmcd.dwItemSpec, 4);
// 	if (StrNCmp(str, L"U", 1) == 0)
// 	{
// 		pLVCD->nmcd.rc.bottom = pLVCD->nmcd.rc.top;
// 	}
// 	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
// 	{
// 		//奇数行 
// 		if (pLVCD->nmcd.dwItemSpec % 2)
// 			pLVCD->clrTextBk = m_EvenColor;
// 		//偶数行 
// 		else
// 			pLVCD->clrTextBk = m_OddColor;
// 		//继续 
// 		*pResult = CDRF_DODEFAULT;
// 	}
}

int CLockListCtrl::IsPositiveID(int id, bool isUDC)
{
	id <<= 8;
	id >>= 8;
	std::vector<positiveID>::iterator it;
	if (isUDC)
	{
		for (it = positiveUDCID.begin(); it != positiveUDCID.end(); it++)
		{

			if (id == it->id)
			{
				if (it->active == NOTHING || it->active == FAIL || it->active == TESTFAIL)
				{
					return INACTIVESTATUS;
				}
				return POSITIVESTATUS;
			}
		}
		return NEGATIVESTATUS;
	}
	else
	{

		for (it = positiveUSID.begin(); it != positiveUSID.end(); it++)
		{
			if (id == it->id)
			{
				if (it->active == NOTHING || it->active == FAIL || it->active == TESTFAIL)
				{
					return INACTIVESTATUS;
				}
				return POSITIVESTATUS;
			}
		}
		return NEGATIVESTATUS;
	}
}


void CLockListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CListCtrl::OnLButtonDblClk(nFlags, point);
}


void CLockListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_nRowHeight;
}

void CLockListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString str;
	str = GetItemText(lpMeasureItemStruct->itemID, 4);
	if (StrNCmp(str, L"U", 1) == 0)
	{
		lpMeasureItemStruct->itemHeight = 0;
	}
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CLockListCtrl::SetRowHeigt(int nHeight) //高置行高(别忘了在.h里添加void SetRowHeigt(int nHeight))
{
	m_nRowHeight = nHeight;

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}
void CLockListCtrl::DrawItem(LPDRAWITEMSTRUCT lpMeasureItemStruct)
{	
	CDC* pDC = CDC::FromHandle(lpMeasureItemStruct->hDC);
	LVITEM lvi = { 0 };
	lvi.mask = LVIF_STATE;//|LVIF_IMAGE;
	lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	lvi.iItem = lpMeasureItemStruct->itemID;
	BOOL bGet = GetItem(&lvi);

	//高亮显示

	BOOL bHighlight = ((lvi.state & LVIS_DROPHILITED) || ((lvi.state & LVIS_SELECTED) &&
		((GetFocus() == this) || (GetStyle() & LVS_SHOWSELALWAYS))));

	// 画文本背景
	CRect rcBack = lpMeasureItemStruct->rcItem;
	pDC->SetBkMode(TRANSPARENT);
	if (bHighlight) //如果被选中
	{
		pDC->SetTextColor(RGB(255, 255, 255)); //文本为白色
		pDC->FillRect(rcBack, &CBrush(RGB(90, 162, 255)));
	}
	else
	{
		pDC->SetTextColor(RGB(0, 0, 0));       //文本为黑色
		pDC->FillRect(rcBack, &CBrush(RGB(255, 255, 255)));
	}
	if (IsPositiveID((int)lpMeasureItemStruct->itemData, true) == POSITIVESTATUS && IsPositiveID((int)lpMeasureItemStruct->itemData, false) == POSITIVESTATUS)//US、UDC均为阳性
	{
		pDC->SetTextColor(POSITIVETEXT);
	}
	else if (IsPositiveID((int)lpMeasureItemStruct->itemData, true) == NEGATIVESTATUS && IsPositiveID((int)lpMeasureItemStruct->itemData, false) == POSITIVESTATUS
		|| IsPositiveID((int)lpMeasureItemStruct->itemData, true) == POSITIVESTATUS && IsPositiveID((int)lpMeasureItemStruct->itemData, false) == NEGATIVESTATUS)//US和UDC结果不匹配
	{
		pDC->SetTextColor(UNCERTAINTEXT);
	}
	else if (IsPositiveID((int)lpMeasureItemStruct->itemData, true) == POSITIVESTATUS && IsPositiveID((int)lpMeasureItemStruct->itemData, false) == INACTIVESTATUS
		|| IsPositiveID((int)lpMeasureItemStruct->itemData, false) == POSITIVESTATUS && IsPositiveID((int)lpMeasureItemStruct->itemData, true) == INACTIVESTATUS)//US或UDC只开了一种，则以开启的项目结果为准
	{
		pDC->SetTextColor(POSITIVETEXT);
	}
	else if (IsPositiveID((int)lpMeasureItemStruct->itemData, true) == INACTIVESTATUS && IsPositiveID((int)lpMeasureItemStruct->itemData, false) == INACTIVESTATUS)
	{
		pDC->SetTextColor(INACTIVETEXT);
	}
	else//均为阴性
	{
		pDC->SetTextColor(CDRF_DODEFAULT);
	}

	if (lpMeasureItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		//写文本
		CString szText;
		int nCollumn = GetHeaderCtrl()->GetItemCount();//列数
		for (int i = 0; i < GetHeaderCtrl()->GetItemCount(); i++)
		{ //循环得到文本
			CRect rcItem;
			if (!GetSubItemRect(lpMeasureItemStruct->itemID, i, LVIR_LABEL, rcItem))
				continue;
			szText = GetItemText(lpMeasureItemStruct->itemID, i);
			rcItem.left += 5; rcItem.right -= 1;
			pDC->DrawText(szText, lstrlen(szText), &rcItem, DT_LEFT
				| DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
		}
	}
}
