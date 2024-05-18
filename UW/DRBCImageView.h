#pragma once
#include "afxwin.h"
#include "THDBUW_Access.h"
#include "TodayList.h"
class CDRBCImageView :
	public CView
{
public:
	CDRBCImageView();
	~CDRBCImageView();
	virtual void OnDraw(CDC* /*pDC*/);
	void PaintDRBCImage(ThreadArgv * taskInfo = NULL);
	int nID;
	void SaveDRBCImage(CString strPath = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};

