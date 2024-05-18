#include "PngButton.h"
#pragma once

class COutputWnd : public CDockablePane
{
// 构造
public:
	COutputWnd();

// 属性
protected:
	CEdit m_wndEdit;
	CEdit m_wndEditNoDel;
	CPngButton m_clearInfo;
// 实现
public:
	virtual ~COutputWnd();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMyBtnKnow();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	virtual void OnPressCloseButton();
	void OnUpdateCommandUI(CCmdUI *cmdUI);
	DECLARE_MESSAGE_MAP()

public:
	// 清除错误信息
	void Clear();
	// 显示错误,错误修正时清除
	void ShowInfo(const TCHAR* pContent);
	// 显示错误
	void ShowInfoNoDel(const TCHAR* pContent);

private:
	// 写日志
	void log(CString logStr);    
	// 调整显示
	void AdjustLayout();

private:
	int m_nNum;
	COleDateTime m_tmNow;
	TCHAR m_chIniPath[MAX_PATH];
};

