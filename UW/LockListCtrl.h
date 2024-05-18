#pragma once


// CLockListCtrl




class CLockListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CLockListCtrl)

public:
	CLockListCtrl();
	virtual ~CLockListCtrl();

public:
	BOOL SetColomnLockMask(unsigned int nIndex, BOOL bMask);
	void SetListColor(COLORREF, COLORREF);
private:
	long m_ColomnLockMask;
	COLORREF m_OddColor;
	COLORREF m_EvenColor;

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

	int IsPositiveID(int,bool);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void SetRowHeigt(int nHeight);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	int m_nRowHeight;
};


