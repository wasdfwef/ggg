#if !defined(AFX_LISTCTRLCOMBOEX_H__CF78F101_D071_46A3_BCA8_CB30861448F1__INCLUDED_)
#define AFX_LISTCTRLCOMBOEX_H__CF78F101_D071_46A3_BCA8_CB30861448F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlComboEx.h : header file
//
#define cgCComboBox CComboBox  //you can use yourself defined combobox
/////////////////////////////////////////////////////////////////////////////
// CListCtrlComboEx window
#include <vector>
using namespace std;
struct stEditAble
{
	int nRow;
	int nCol;
};

struct stComboAble
{
	int nRow;
	int nCol;
	cgCComboBox *pCombo;
};


class CListCtrlComboEx : public CListCtrl
{
// Construction
public:
	CListCtrlComboEx();
protected:
	vector<stEditAble> m_EditAbleArray;
	vector<stComboAble> m_ComboAbleArray;
	int m_iRow;
	int m_iCol;
	CEdit *m_pEdit;
	int m_iHeight;


// Attributes
public:
	void SetEditAble(int nRow,int nCol);
	void SetComboAble(int nRow,int nCol,cgCComboBox *pCombo);
	void SetItemHeight(int nHeight);
	void ClearAllAbles();
	BOOL GetCellRect(int nRow, int nCol, CRect& rect);


// Operations
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlComboEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlComboEx();

	// Generated message map functions
protected:
	BOOL IsEditAble(int nRow,int nCol);
	BOOL IsComboAble(int nRow,int nCol);
	virtual void PreSubclassWindow();
	LPCTSTR MakeShortString(CDC *pDC,LPCTSTR lpszLong,int nColumnLen,int nOffset);
	
	
	
protected:
	//{{AFX_MSG(CListCtrlComboEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	virtual afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg BOOL OnHeaderEndResize(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	
	//}}AFX_MSG

	DECLARE_DYNAMIC(CListCtrlComboEx)
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLCOMBOEX_H__CF78F101_D071_46A3_BCA8_CB30861448F1__INCLUDED_)
