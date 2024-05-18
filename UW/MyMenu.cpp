#include "StdAfx.h"
#include "MyMenu.h"


CMyMenu::CMyMenu(void)
{
}


CMyMenu::~CMyMenu(void)
{
}


void CMyMenu::DrawItem(LPDRAWITEMSTRUCT lpDS)
{
	CDC* pDC = CDC::FromHandle(lpDS->hDC);

	CRect rect = lpDS->rcItem;

	CString strText;

	SItem &sItem = m_map[lpDS->itemID];

	if (lpDS->itemID) strText = sItem.szText;

	//选中状态菜单项:画边线和浅蓝色填充
	if ((lpDS->itemState & ODS_SELECTED) && (lpDS->itemAction & ODA_SELECT))
	{
		pDC->FillSolidRect(&rect, ::GetSysColor(COLOR_MENU));
		rect.DeflateRect(1, 1);
		CPen pen(PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHT));
		CBrush br(RGB(182, 189, 210));
		pDC->SelectObject(&pen);
		pDC->SelectObject(br);
		pDC->Rectangle(rect);
	}
	else//非选中状态:普通背景填充
	{
		pDC->FillSolidRect(&rect, GetSysColor(COLOR_MENU));

		if (rect.Height() == 10)//如果是分隔条菜单:画两条线
		{
			CPen pen1(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
			pDC->SelectObject(&pen1);
			pDC->MoveTo(rect.left + 2, rect.top + 4);
			pDC->LineTo(rect.right - 2, rect.top + 4);

			CPen pen2(PS_SOLID, 1, GetSysColor(COLOR_HOTLIGHT));
			pDC->SelectObject(&pen2);
			pDC->MoveTo(rect.left + 2, rect.top + 5);
			pDC->LineTo(rect.right - 2, rect.top + 5);

			return;
		}
		rect.DeflateRect(1, 1);
	}

	//画图标
	pDC->DrawIcon(rect.left + 1, rect.top + 1, sItem.hIcon);

	pDC->SetBkMode(TRANSPARENT);
	rect.left += 35;
	pDC->DrawText(strText, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

}


void CMyMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMS)
{
	lpMS->itemWidth = 80;//菜单项的宽度

	lpMS->itemHeight = m_map[lpMS->itemID].nHeight;//每个菜单项的高度
}

BOOL CMyMenu::AppendMenu(UINT_PTR nIDNewItem, LPCTSTR lpszNewItem, HICON hIcon, UINT nFlags)
{
	//将菜单的文字和图标信息按照ID进行登记，以便自绘时使用

	SItem sItem = { hIcon };
	

	if (lpszNewItem) 
		memcpy(sItem.szText, lpszNewItem, sizeof(LPCTSTR));

	if (nFlags & MF_SEPARATOR) 
		sItem.nHeight = 10;
	else 
		sItem.nHeight = 35;

	//根据ID添加到map中
	m_map[nIDNewItem] = sItem;

	//每个插入的菜单项都自动添加自绘属性
	return CMenu::AppendMenu(nFlags | MF_OWNERDRAW, nIDNewItem, lpszNewItem);

}