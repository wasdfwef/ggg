#pragma once
#include "afxwin.h"
class CMyMenu :public CMenu
{
public:
	CMyMenu(void);
	~CMyMenu(void);
private:
	struct SItem{
		HICON hIcon;
		char szText[32];
		int nHeight;
	};
	CMap<WORD, WORD, SItem, SItem> m_map;//菜单Id与对应的节点信息


public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMS);
	BOOL AppendMenu(UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL, HICON hIcon = NULL, UINT nFlags = 0);
};
