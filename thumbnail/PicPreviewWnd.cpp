// PicPreviewWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "thumbnail.h"
#include "PicPreviewWnd.h"
#include "afxdialogex.h"
#include <gdiplus.h>
#include "DbgMsg.h"

// CPicPreviewWnd 对话框

IMPLEMENT_DYNAMIC(CPicPreviewWnd, CDialogEx)

CPicPreviewWnd::CPicPreviewWnd(vector<CString>& vecImages, CString &path, int nCurIndex, CWnd* pParent /*=NULL*/)
	: CDialogEx(CPicPreviewWnd::IDD, pParent)
	, m_vecImages(vecImages)
	, m_path(path)
	, m_nCurIndex(nCurIndex)
{

}

CPicPreviewWnd::~CPicPreviewWnd()
{
}

void CPicPreviewWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPicPreviewWnd, CDialogEx)
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CPicPreviewWnd 消息处理程序


BOOL CPicPreviewWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 全屏、置顶
	int cx, cy;
	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);
#ifdef DEBUG
	SetWindowPos(0, 0, 0, cx, cy, SWP_SHOWWINDOW);
#else
	SetWindowPos(&wndTopMost, 0, 0, cx, cy, SWP_SHOWWINDOW);
#endif // DEBUG

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CPicPreviewWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	return DrawPic(dc);

}

void CPicPreviewWnd::DrawPic(HDC hDc)
{
	USES_CONVERSION;
	CRect rt;
	GetClientRect(rt);
	if (m_vecImages.size() > m_nCurIndex)
	{
		WCHAR wszFilePath[MAX_PATH];
		wcscpy(wszFilePath, CT2CW(m_path + _T("\\") + m_vecImages.at(m_nCurIndex)));

		if (!PathFileExistsW(wszFilePath))
		{
			return;
		}

		Gdiplus::Image image(wszFilePath);
		UINT width = image.GetWidth();
		UINT height = image.GetHeight();
		float ratio = float(width) / height;
		float ratio2 = float(rt.Width()) / rt.Height();
		int scaledWidth = 0;
		int scaledHeight = 0;
		if (ratio - ratio2 > 0.000001f)
		{
			scaledWidth = rt.Width();
			scaledHeight = scaledWidth / ratio;
		}
		else
		{
			scaledHeight = rt.Height();
			scaledWidth = scaledHeight * ratio;
			
		}
		DBG_MSG("PicPreviewWnd: %d,%d, scaled: %d,%d\n", rt.Width(), rt.Height(), scaledWidth, scaledHeight);

		Gdiplus::Bitmap bmp(scaledWidth, scaledHeight);
		Gdiplus::Graphics g(&bmp);
		g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		g.DrawImage(&image, 0, 0, scaledWidth, scaledHeight);

		Gdiplus::Graphics graphics(hDc);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed);

		Gdiplus::FontFamily fontFamily(L"Arial");
		Gdiplus::Font font(&fontFamily, 30, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		Gdiplus::StringFormat stringformat;
		//stringformat.SetAlignment(StringAlignmentCenter);
		//stringformat.SetLineAlignment(StringAlignmentCenter); 
		stringformat.SetAlignment(Gdiplus::StringAlignmentFar);
		stringformat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		Gdiplus::SolidBrush brush(Gdiplus::Color(180, 255, 0, 0));

		CString str;
		str.Format(_T("(%d/%d)"), (m_nCurIndex + 1), m_vecImages.size());

		graphics.DrawImage(&bmp, (rt.Width() - scaledWidth) / 2, (rt.Height() - scaledHeight) / 2, scaledWidth, scaledHeight);
		graphics.DrawString(CT2CW(str), -1, &font, Gdiplus::RectF((rt.Width() - scaledWidth) / 2, (rt.Height() - scaledHeight) / 2, scaledWidth, scaledHeight), &stringformat, &brush);
	}
}

BOOL CPicPreviewWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	if (zDelta < 0)
	{
		if (m_vecImages.size() > 0)
		{
			if (++m_nCurIndex >= m_vecImages.size())
			{
				m_nCurIndex = 0;
			}
			CClientDC dc(this);
			DrawPic(dc);
		}
	}
	else if (zDelta > 0)
	{
		if (m_vecImages.size() > 0)
		{
			if (--m_nCurIndex < 0)
			{
				m_nCurIndex = m_vecImages.size() - 1;
			}
			CClientDC dc(this);
			DrawPic(dc);
		}
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CPicPreviewWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_LEFT:
		case VK_UP:
		{
			if (m_vecImages.size() > 0)
			{
				if (--m_nCurIndex < 0)
				{
					m_nCurIndex = m_vecImages.size() - 1;
				}
				CClientDC dc(this);
				DrawPic(dc);
			}
		}
		break;
		case VK_RIGHT:
		case VK_DOWN:
		{
			if (m_vecImages.size() > 0)
			{
				if (++m_nCurIndex >= m_vecImages.size())
				{
					m_nCurIndex = 0;
				}
				CClientDC dc(this);
				DrawPic(dc);
			}
		}
		break;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
