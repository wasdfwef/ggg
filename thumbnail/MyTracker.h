#pragma once
#include "afxext.h"

class CMyTracker : public CRectTracker
{
private:

	RECT  m_RangeRC;
	HDC   m_hDC;
	HWND  m_hWnd;
	SIZE  m_MaxSize;
	BOOL  m_DrawStart;

	BOOL  OverMaxRCRange(PRECT rc);
	BOOL  OverTrackerRange(PRECT rc);

public:
	CMyTracker(void);
	CMyTracker(LPCRECT lpSrcRect, UINT nStyle);
	~CMyTracker(void);

	void  Draw(CDC *pDC,CPen *pPen)const;
	void  SetRectTrackerRange(RECT &rc, HWND  hWnd);
	void  SetMaxSize(SIZE &max_sz);

	virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo,
		CDC* pDC, CWnd* pWnd);

	virtual void OnChangedRect(const CRect& rectOld);
};
