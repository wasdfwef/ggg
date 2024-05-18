#pragma once
#include "resource.h"

// CListCtrlSkinDlg 对话框

class CListCtrlSkinDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListCtrlSkinDlg)

public:
	CListCtrlSkinDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CListCtrlSkinDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_LISTSKIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	COLORREF m_OddColor;
	COLORREF m_EvenColor;
	virtual BOOL OnInitDialog();
	CBrush m_brush;
	CRect m_pos;
	void GetLastColor(COLORREF,COLORREF);
	void GetParentPos(CRect);
	afx_msg void OnStnClickedListColorOdd();
	afx_msg void OnStnClickedListskinEven();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
};
