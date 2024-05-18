#pragma once
#include "afxdockablepane.h"
#include "PngButton.h"
#include "ToolsView.h"


class WndBtnTools :
	public CDockablePane
{
public:
	void InitRes(void);
	WndBtnTools();
	~WndBtnTools();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void OnUpdateCommandUI(CCmdUI *cmdUI);
private:

	

public:
	CToolsView* myView;
	int InsertBtn(CPngButton&, int, int,CString,CRect rect = NULL);
	virtual BOOL DestroyWindow();

};

