#pragma once
#include "afxcmn.h"
#include "PngButton.h"
#include "ListCtrlSkinDlg.h"
#include "resource.h"
#include <vector>



#define WM_UPDATESKINUI WM_USER + 556

class CMyListCtrl :
	public CListCtrl
{
public:
	CPngButton m_BtnListskin;
	CToolTipCtrl m_tipCtrl;
	COLORREF m_OddColor;
	COLORREF m_EvenColor;
	CMyListCtrl();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void InitRes();

	~CMyListCtrl();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedListSkin();
	void OnUpdateCommandUI(CCmdUI *cmdUI);
	void SetCellColor(int iRow, int iCol, COLORREF color);
	void SetFontColor(int iRow, int iCol, COLORREF color);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CListCtrlSkinDlg m_ListSkin;
	bool IsButtonArea(LONG,LONG);
	bool isShowButton;
	void ReadInitListColor();
	void SaveInitListColor();
	void SetIsChangeListColor(bool,COLORREF,COLORREF);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	bool IsResultGreateOnePlus(int nIndex);
public:
	int curSelectID;
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
CString * SplitString(CString str, char split, int& iSubStrs);
