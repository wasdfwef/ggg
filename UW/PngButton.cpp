//PngButton.cpp : implementation file
//

#include "stdafx.h"
#include "PngButton.h"
// CPngButton

IMPLEMENT_DYNAMIC(CPngButton, CButton)

CPngButton::CPngButton()
: m_bFlagIsColorSet(FALSE)
, m_bFlagIsTextSet(FALSE)
, m_bFlagIsImageSet(FALSE)
, m_bFlagIsFirst(TRUE)
, m_bMouseOnButton(FALSE)
, m_bIsPressed(FALSE)
, m_bIsFocused(FALSE)
//, m_iTitleSize(90)
, m_iOnTransparent(0)
, m_iDownTransparent(0)
, m_iState(Disable)
, m_bFlagContinueShow(FALSE)
{
	//DEL initGDI();
	m_BaseColor = RGB(0, 0, 0);
	m_FontColor = RGB(0, 0, 0);
}

CPngButton::~CPngButton()
{
}


BEGIN_MESSAGE_MAP(CPngButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()



// CPngButton message handlers

void CPngButton::LoadImage(CImage &image, UINT nID)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nID), _T("PNG"));
	if (hRsrc == NULL)
		return;

	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (lpRsrc == NULL)
		return;

	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem, lpRsrc, len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem, FALSE, &pstm);

	image.Destroy();
	image.Load(pstm);

	GlobalUnlock(m_hMem);
	GlobalFree(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);

	if (image.GetBPP() == 32)
	{
		for (int i = 0; i < image.GetWidth(); i++)
		{
			for (int j = 0; j < image.GetHeight(); j++)
			{
				unsigned char* pucColor = reinterpret_cast<unsigned char *>(image.GetPixelAddress(i, j));
				pucColor[0] = pucColor[0] * pucColor[3] / 255;
				pucColor[1] = pucColor[1] * pucColor[3] / 255;
				pucColor[2] = pucColor[2] * pucColor[3] / 255;
			}
		}
	}
}

BOOL CPngButton::LoadImage(CImage &image, const CString& Path)
{
	CDC *pDC = GetDC();
	image.Destroy();
	image.Load(Path);
	if (image.IsNull())
	{
		return FALSE;
	}
	if (image.GetBPP() == 32) //确认该图像包含Alpha通道
	{
		int i;
		int j;
		for (i = 0; i < image.GetWidth(); i++)
		{
			for (j = 0; j < image.GetHeight(); j++)
			{
				byte *pByte = (byte *)image.GetPixelAddress(i, j);
				pByte[0] = pByte[0] * pByte[3] / 255;
				pByte[1] = pByte[1] * pByte[3] / 255;
				pByte[2] = pByte[2] * pByte[3] / 255;
			}
		}
	}
	ReleaseDC(pDC);
}
/*檢測和判斷按鈕狀態的消息功能們*/

void CPngButton::CancelHover(void)
{
	//功能:取消鼠標懸浮標誌位
	//參數:無
	//返回值:無
	if (m_bMouseOnButton)
	{
		m_bMouseOnButton = FALSE;
		Invalidate();
	} // if
} // End of CancelHover

void CPngButton::OnMouseMove(UINT nFlags, CPoint point)
{
	//功能:鼠標移動消息
	//參數:nFlag為指示各种虚拟按键是否按下,point為鼠標當前位置
	//返回值:無
	CWnd* wndUnderMouse = NULL;
	CWnd* wndActive = this;
	TRACKMOUSEEVENT csTME;

	CButton::OnMouseMove(nFlags, point);

	ClientToScreen(&point);
	wndUnderMouse = WindowFromPoint(point);

	// If the mouse enter the button with the left button pressed then do nothing
	if (nFlags & MK_LBUTTON && m_bMouseOnButton == FALSE)//鼠標左鍵按下闖過著跳過處理
		return;

	if (wndUnderMouse && wndUnderMouse->m_hWnd == m_hWnd && wndActive)//判斷并更新按鈕當前狀態
	{
		if (!m_bMouseOnButton)
		{
			m_bMouseOnButton = TRUE;

			Invalidate();

			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = m_hWnd;
			::_TrackMouseEvent(&csTME);
		} // if
	}
	else CancelHover();
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CPngButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//功能:鼠標離開按鈕消息
	//參數:wParam,lParam鼠標位置參數
	//返回值:LRESULT
	CancelHover();//取消懸浮狀態
	return 0;
} // End of OnMouseLeave

void CPngButton::OnKillFocus(CWnd* pNewWnd)
{
	//功能:失去焦點時消息處理
	//參數:pNewWnd指定指向接收输入焦点的窗口
	//返回值:無
	CButton::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	CancelHover();
}

BOOL CPngButton::OnEraseBkgnd(CDC* pDC)
{
	//功能:擦除背景消息處理
	//參數:pDC為此按鈕的DC
	//返回值:無
	return TRUE;// CButton::OnEraseBkgnd(pDC);
}

/*設置相關參數*/
void CPngButton::PreSubclassWindow()
{
	//功能:按鈕設置自繪模式
	//參數:無
	//返回值:無
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}

void CPngButton::SetSize(int iWidth, int iHeight)
{
	//功能:設置按鈕大小
	//參數:iWidth為按鈕寬度,iHeight為按鈕高度
	//返回值:無
	SetWindowPos(NULL, -1, -1, iWidth, iHeight,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
}

BOOL CPngButton::SetTransparentColor(COLORREF color, UINT OnTransparent, UINT DownTransparent)
{
	//功能:設置半透明貼膜參數
	//參數:color貼膜的顏色,OnTransparent是鼠標懸浮在按鈕時其透明度,DownTransparent是按鈕按下時其透明度
	//返回值:BOOL是否設置成功
	m_BaseColor = color;//載入貼膜顏色
	//DEL int ibrightness = 0.299*GetRValue(color) + 0.587*GetGValue(color) + 0.114*GetBValue(color);//亮度
	//DEL if (ibrightness > 225 || ibrightness < 15)
	//DEL {
	//DEL m_bFlagIsColorSet = FALSE;
	//DEL return FALSE;
	//DEL }
	//DEL else
	//DEL {
	//DEL m_bFlagIsColorSet = TRUE;
	//DEL return TRUE;
	//DEL }
	if (OnTransparent < 0xFF && DownTransparent < 0xFF)//半透明參數需要小於255,否則不設置貼膜
	{
		//設置貼膜有效，并載入半透明度
		m_bFlagIsColorSet = TRUE;
		m_iOnTransparent = OnTransparent;
		m_iDownTransparent = DownTransparent;
	}
	else{
		m_bFlagIsColorSet = FALSE;//設置貼膜無效
	}
	return m_bFlagIsColorSet;//返回貼膜設置狀態
}

void CPngButton::SetFontColor(COLORREF color)
{
	m_FontColor = color;
}

BOOL CPngButton::SetTitle(const CString &csTitle)
{
	//功能:設置標題
	//參數:csTitle為標題
	//返回值:BOOL標題是否顯示
	this->SetWindowTextW(csTitle);
	m_bFlagIsTextSet = TRUE;
	//m_font.DeleteObject();
	//m_font.CreatePointFont(m_iTitleSize, _T("微軟雅黑"));
	return m_bFlagIsTextSet;
}

void CPngButton::SetImagePng(UINT nIDNormal, UINT nIDDisable)
{
	//功能:設置按鈕對應PNG圖片
	//參數:nID為按鈕普通狀態的PNG貼圖資源,nIDDisable為按鈕失能狀態下的PNG貼圖資源
	//返回值:無
	m_bFlagIsImageSet = TRUE;;
	if (m_bFlagIsImageSet)
	{
		LoadImage(m_image[Disable], nIDDisable);
		LoadImage(m_image[Normal], nIDNormal);
		m_iState = MaskMode;
	}
	SizeToContent();
}

void CPngButton::SetImagePng(const CString& PathNormal, const CString& PathDisable)
{
	m_bFlagIsImageSet = TRUE;;
	if (m_bFlagIsImageSet)
	{
		LoadImage(m_image[Disable], PathDisable);
		LoadImage(m_image[Normal], PathNormal);
		m_iState = MaskMode;
	}
	SizeToContent();
}

void CPngButton::SetImagePng(UINT nIDNormal, UINT nIDOn, UINT nIDDown, UINT nIDDisable)
{
	//功能:設置按鈕對應PNG圖片
	//參數:nID為按鈕普通狀態的PNG貼圖資源,nIDOn為鼠標懸浮在按鈕上狀態的PNG貼圖資源,nIDDown為按鈕按下狀態的PNG貼圖資源,nIDDisable為按鈕失能狀態下的PNG貼圖資源
	//返回值:無
	m_bFlagIsImageSet = TRUE;
	if (m_bFlagIsImageSet)
	{
		if (m_image[Disable].IsNull() == FALSE)
			m_image[Disable].Destroy();
		LoadImage(m_image[Disable], nIDDisable);

		if (m_image[Normal].IsNull() == FALSE)
			m_image[Normal].Destroy();
		LoadImage(m_image[Normal], nIDNormal);

		if (m_image[On].IsNull() == FALSE)
			m_image[On].Destroy();
		LoadImage(m_image[On], nIDOn);

		if (m_image[Down].IsNull() == FALSE)
			m_image[Down].Destroy();
		LoadImage(m_image[Down], nIDDown);
		m_iState = PictureMode;
	}
	SizeToContent();
}

void CPngButton::SetImagePng(const CString& PathNormal, const CString& PathOn, const CString& PathDown, const CString& PathDisable)
{
	m_bFlagIsImageSet = TRUE;;
	if (m_bFlagIsImageSet)
	{
		if (m_image[Disable].IsNull() == FALSE)
			m_image[Disable].Destroy();
		LoadImage(m_image[Disable], PathDisable);

		if (m_image[Normal].IsNull() == FALSE)
			m_image[Normal].Destroy();
		LoadImage(m_image[Normal], PathNormal);

		if (m_image[On].IsNull() == FALSE)
			m_image[On].Destroy();
		LoadImage(m_image[On], PathOn);

		if (m_image[Down].IsNull() == FALSE)
			m_image[Down].Destroy();
		LoadImage(m_image[Down], PathDown);
		m_iState = PictureMode;
	}
	SizeToContent();
}

void CPngButton::SetContinueShow(BOOL ContinueShow)
{
	//功能:設置是否一直顯示為按下狀態
	//參數:ContinueShow為是否要一直顯示為按下狀態
	//返回值:無
	m_bFlagContinueShow = ContinueShow;
}

void CPngButton::SizeToContent(void)
{
	//功能:使按鈕大小和載入的圖片大小一致
	//參數:無
	//返回值:無
	if (m_image[1] != NULL)
	{
		SetWindowPos(NULL, -1, -1, m_image[1].GetWidth(), m_image[1].GetHeight(),
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
	} // if
} // End of SizeToContent

void CPngButton::SaveBG(void)
{
	//功能:保存父窗口擋住的背景
	//參數:無
	//返回值:無
	CRect rc;
	this->GetWindowRect(&rc);
	CDC* pDC = GetDC();
	CBitmap memBitmap;

	//得到父窗口句柄以及控件在父窗口的位置
	CDC* pParentDC = GetParent()->GetDC();
	CPoint pt(0, 0);
	MapWindowPoints(GetParent(), &pt, 1);

	if (m_MemDCBG == NULL)//如果已經m_MemDCBG已經創建了就不用再創建了,否則下次調用時會錯誤
		m_MemDCBG.CreateCompatibleDC(pDC);

	//將擋住的父窗口背景部分保存入m_MemDCBG中
	memBitmap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	m_MemDCBG.SelectObject(&memBitmap);
	m_MemDCBG.BitBlt(0, 0, rc.Width(), rc.Height(), pParentDC, pt.x, pt.y, SRCCOPY);

	::ReleaseDC(GetParent()->m_hWnd, *pParentDC);
	::ReleaseDC(this->m_hWnd, *pDC);
}

BOOL CPngButton::SetShowState(BOOL NeedColorSet, BOOL NeedTextSet, BOOL NeedImageSet)
{
	//功能:設置按鈕風格
	//參數:NeedColorSet為是否顯示貼膜,NeedTextSet為是否打印文字,NeedImageSet為是否打印PNG圖片
	//返回值:無
	BOOL bFlagSuccessSet = TRUE;
	//設置是否顯示貼膜
	if (NeedColorSet)
	{
		if (m_BaseColor == NULL)
		{
			bFlagSuccessSet = FALSE;
		}
		else
			m_bFlagIsColorSet = NeedColorSet;
	}
	else{
		m_bFlagIsColorSet = NeedColorSet;
	}
	//設置是否顯示文字
	m_bFlagIsTextSet = NeedTextSet;
	//設置是否顯示png資源
	if (m_image[Disable] == NULL)
		;
	else
		m_bFlagIsImageSet = NeedImageSet;
	return bFlagSuccessSet;
}

/*繪製按鈕*/
void CPngButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//功能:繪製按鈕，參考BitButtonNL類
	//參數:lpDrawItemStruct按鈕參數
	//返回值:無
	m_bIsFocused = (lpDrawItemStruct->itemState & ODS_FOCUS);//按鈕是否有焦點
	m_bIsPressed = (lpDrawItemStruct->itemState & ODS_SELECTED);//按鈕是否按下
	if (m_bFlagIsFirst)//第一次調用保存父窗口擋住的背景
	{
		SaveBG();
		m_bFlagIsFirst = FALSE;
		DrawItem(lpDrawItemStruct);
	}
	else
	{
		CRect rc;
		this->GetWindowRect(&rc);
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		pDC->SetBkMode(TRANSPARENT);

		CDC MemDC;
		CBitmap bmp;

		//創建雙緩存用的MemDC
		MemDC.CreateCompatibleDC(pDC);
		bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
		MemDC.SelectObject(&bmp);

		MemDC.BitBlt(0, 0, rc.Width(), rc.Height(), &m_MemDCBG, 0, 0, SRCCOPY); //畫父窗口擋住的背景
		if (IsWindowEnabled())
		{
			switch (m_iState)
			{
			case MaskMode:
				if ((m_bFlagIsColorSet && m_bMouseOnButton) || m_bFlagContinueShow)
				{
					//畫半透明貼膜
					CDC MemDCBK;
					CDC* pDCDrawBG;
					pDCDrawBG = GetDC();
					CBitmap bmpBK;

					//創建半透明貼膜
					MemDCBK.CreateCompatibleDC(pDCDrawBG);
					bmpBK.CreateCompatibleBitmap(pDCDrawBG, rc.Width(), rc.Height());
					MemDCBK.SelectObject(&bmpBK);
					MemDCBK.FillSolidRect(0, 0, rc.Width(), rc.Height(), m_BaseColor);

					//半透明參數設置
					BLENDFUNCTION bf;
					bf.BlendOp = AC_SRC_OVER;
					bf.BlendFlags = 0;
					if (m_bIsPressed || m_bFlagContinueShow)
						bf.SourceConstantAlpha = m_iDownTransparent;
					else
						bf.SourceConstantAlpha = m_iOnTransparent;
					bf.AlphaFormat = 0;

					MemDC.AlphaBlend(0, 0, rc.Width(), rc.Height(), &MemDCBK, 0, 0, rc.Width(), rc.Height(), bf);//貼膜

					::ReleaseDC(this->m_hWnd, *pDCDrawBG);
				}//end if ((m_bFlagIsColorSet && m_bMouseOnButton ) || m_bFlagContinueShow)
				if (m_bFlagIsImageSet)
				{
					m_image[Normal].Draw(MemDC.m_hDC, 0, 0);
				}//end if (m_bFlagIsImageSet)
				break;
			case PictureMode:
				if (m_bMouseOnButton || m_bFlagContinueShow)
				{
					if (m_bIsPressed || m_bFlagContinueShow)
						m_image[Down].Draw((&MemDC)->m_hDC, 0, 0);
					else
						m_image[On].Draw((&MemDC)->m_hDC, 0, 0);
				}
				else{//end if (m_bMouseOnButton || m_bFlagContinueShow)
					m_image[Normal].Draw((&MemDC)->m_hDC, 0, 0);
				}//end if (m_bMouseOnButton || m_bFlagContinueShow)...else
				break;
			}//end switch(m_iState)
		}
		else{//endif(IsWindowEnabled())
			if (m_image[Disable] != NULL)
				m_image[Disable].Draw((&MemDC)->m_hDC, 0, 0);
			else
				m_image[Normal].Draw((&MemDC)->m_hDC, 0, 0);
		}
		if (m_bFlagIsTextSet)
		{
			//畫文字
			CString sTitle;
			GetWindowText(sTitle);

			//設置文字風格
			SetBkMode(MemDC, TRANSPARENT);
			SetTextColor(MemDC, m_FontColor);
			//設置畫在控件中心
			CRect captionRect = lpDrawItemStruct->rcItem;
			captionRect.top--;

			MemDC.SelectObject(GetFont());
			MemDC.DrawText((LPCTSTR)sTitle, &captionRect, DT_SINGLELINE | DT_VCENTER | DT_WORDBREAK | DT_CENTER);
		}//end if (m_bFlagIsTextSet)
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &MemDC, 0, 0, SRCCOPY);
	}//end if (m_bFlagIsFirst)..else
}