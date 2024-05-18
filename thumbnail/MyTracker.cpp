#include "StdAfx.h"
#include ".\mytracker.h"

#define  VAILD_RECT(rc) (rc.top < rc.bottom && rc.left < rc.right)
#define  VAILD_SIZE(sz) (sz.cx != 0 && sz.cy != 0)
#define  RECT_WIDTH(x) (x.right - x.left)
#define  RECT_HEIGHT(x) (x.bottom - x.top)


CMyTracker::CMyTracker(void)
{
	m_hDC = NULL;
	m_hWnd = NULL;
	ZeroMemory(&m_RangeRC, sizeof(m_RangeRC));
	ZeroMemory(&m_MaxSize, sizeof(m_MaxSize));
	m_DrawStart = FALSE;
}

CMyTracker::CMyTracker(LPCRECT lpSrcRect, UINT nStyle)
{
	CRectTracker::CRectTracker(lpSrcRect, nStyle);
}

CMyTracker::~CMyTracker(void)
{
}

void CMyTracker::Draw(CDC *pDC,CPen *pen)const   
  {   
	  if((m_nStyle & dottedLine) != 0)   
	  {
		  VERIFY(pDC->SaveDC() != 0);   
		  pDC->SetMapMode(MM_TEXT);   
		  pDC->SetViewportOrg(0,0);   
		  pDC->SetWindowOrg(0,0);   
 
		  CRect rect = m_rect;   
		  rect.NormalizeRect();   
			
		  CPen *pOldPen = NULL;   
		  CBrush *pOldBrush = NULL;     
		  int nOldROP;   
			
		  pOldPen = (CPen*)pDC->SelectObject(pen);   
		  pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);   
		  nOldROP = pDC->SetROP2(R2_COPYPEN);   
				
		  rect.InflateRect(+1,+1);  
		  pDC->Rectangle(rect.left,rect.top,rect.right,rect.bottom);
		  pDC->SetROP2(nOldROP); 

		  if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
		  {
			  UINT mask = GetHandleMask();
			  for (int i = 0; i < 8; ++i)
				  if(mask & (1<<i))
				  {
					  GetHandleRect((TrackerHit)i,&rect);
					  LOGPEN logpen;
					  pen->GetLogPen(&logpen);
					  pDC->FillSolidRect(rect,logpen.lopnColor);
				  }
		  }
	  } 
	  else 
		  CRectTracker::Draw(pDC);   
  }   

void CMyTracker::SetRectTrackerRange(RECT &rc, HWND hWnd)
{
	CopyRect(&m_RangeRC, &rc);
	m_hDC = GetDC(hWnd);
	m_hWnd = hWnd;
}

void CMyTracker::SetMaxSize(SIZE &max_sz)
{
	memcpy(&m_MaxSize, &max_sz, sizeof(SIZE));
}


void CMyTracker::DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd)
{
	if( VAILD_SIZE(m_MaxSize) )
		OverMaxRCRange((PRECT)lpRect);

	if( VAILD_RECT(m_RangeRC) )
		OverTrackerRange((PRECT)lpRect);

	CRectTracker::DrawTrackerRect(lpRect, pWndClipTo, pDC, pWnd);
}

BOOL CMyTracker::OverMaxRCRange(PRECT rc)
{
	BOOL bOver = FALSE;

	if( RECT_WIDTH((*rc)) > m_MaxSize.cx )
	{
		bOver = TRUE;
		rc->right = rc->left + m_MaxSize.cx;
	}
	if( RECT_HEIGHT((*rc)) > m_MaxSize.cy )
	{
	    bOver = TRUE;
		rc->bottom = rc->top + m_MaxSize.cy;
	}

	return bOver;
}



BOOL CMyTracker::OverTrackerRange(PRECT rc)
{
	int sub, sub1;

#define DEFAULT_SUB 2
	if( rc->left <= m_RangeRC.left - DEFAULT_SUB)
		rc->left = m_RangeRC.left + DEFAULT_SUB;
	if( rc->top <= m_RangeRC.top - DEFAULT_SUB )
		rc->top = m_RangeRC.top + DEFAULT_SUB;
	if( rc->right >= m_RangeRC.right + DEFAULT_SUB)
		rc->right = m_RangeRC.right - DEFAULT_SUB;
	if( rc->bottom >= m_RangeRC.bottom + DEFAULT_SUB )
		rc->bottom = m_RangeRC.bottom - DEFAULT_SUB;

	if( rc->left + m_sizeMin.cx >= rc->right )
	{
		sub = m_RangeRC.right - rc->right;
		sub1 = rc->left - m_RangeRC.left;
		if( sub < sub1 )
			rc->left = rc->right - m_sizeMin.cx;
		else
			rc->right = rc->left + m_sizeMin.cx;
	}
	if( rc->top + m_sizeMin.cy >= rc->bottom )
	{
		sub = m_RangeRC.bottom - rc->bottom;
		sub1 = rc->top - m_RangeRC.top;
		if( sub < sub1 )
			rc->top = rc->bottom - m_sizeMin.cy;
		else
			rc->bottom = rc->top + m_sizeMin.cy;
	}


	return FALSE;
}

void CMyTracker::OnChangedRect(const CRect& rectOld)
{
		
	if( VAILD_SIZE(m_MaxSize) )
	{
		OverMaxRCRange(&m_rect);
		OverMaxRCRange((PRECT)&rectOld);
	}

	if( VAILD_RECT(m_RangeRC) )
	{
		OverTrackerRange(&m_rect);
		OverTrackerRange((PRECT)&rectOld);
	}

	CRectTracker::OnChangedRect(rectOld);
}