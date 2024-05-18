#include "stdafx.h"
#include "PaintView.h"
#include "include/common/String.h"
#include <gdiplus.h>

using namespace Gdiplus;

using namespace Common;
#define PAINTWIDTH 600
#define PAINTHIGHT 400
#define COFF (32/8)	//32为位置直径，8为细胞直径，COFF为直径换算系数
#define XSTEP 10	
#define YSTEP 3
#define FONTSIZE 30
#define SMOOTH 20 //平滑处理系数
#define MAX_POINT 256
#define WIDTHLEN 107



CString UDCI_QC(_T("UDC质控(阳性)"));
CString UDCII_QC(_T("UDC质控(阴性)"));

CString USI_QC(_T("US质控(水平1)"));
CString USII_QC(_T("US质控(水平2)"));
CString USIII_QC(_T("US质控(水平3)"));


CPaintView::CPaintView()
{
	isPaintUs = false;
	isPreView = false;
}


CPaintView::~CPaintView()
{
}
BEGIN_MESSAGE_MAP(CPaintView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_MESSAGE(WM_PAINT_VIEW,&CPaintView::OnPaintBk)
	ON_MESSAGE(WM_PAINT_VIEW_US,&CPaintView::OnPaintUsBk)
END_MESSAGE_MAP()


char * CPaintView::GetItemStringByIndex(int index)
{
	switch (index)
	{
	case 0:return "LEU";
	case 1:return "NIT";
	case 2:return "URO";
	case 3:return "PRO";
	case 4:return "PH";
	case 5:return "BLD";
	case 6:return "SG";
	case 7:return "KET";
	case 8:return "BIL";
	case 9:return "GLU";
	case 10:return "VC";
	case 11:return "MA";
	case 12:return "CRE";
	case 13:return "CA";
	default:
		return "质控球";
		break;
	}
}

int CPaintView::GetOffsetByLen(int len)
{
	return (int)((WIDTHLEN - len) / 2);
}

bool CPaintView::SaveBMP(const char * fileName)
{	
	HWND hwnd = this->GetSafeHwnd();
	HDC hWndDC = ::GetDC(hwnd);//获取DC   

	RECT rect;
	::GetClientRect(hwnd, &rect);//获取大小   
	HDC hDCMem = ::CreateCompatibleDC(hWndDC);//创建兼容DC   

	HBITMAP hBitMap = ::CreateCompatibleBitmap(hWndDC, rect.right, rect.bottom);//创建兼容位图   
	HBITMAP hOldMap = (HBITMAP)::SelectObject(hDCMem, hBitMap);//将位图选入DC，并保存返回值   

	::BitBlt(hDCMem, 0, 0, rect.right, rect.bottom, hWndDC, 0, 0, SRCCOPY);//将DC的图象复制到内存DC中   

	CImage image;
	image.Attach(hBitMap);
	image.Save(CharToWChar(fileName).c_str());//如果文件后缀为.bmp，则保存为为bmp格式  
	image.Detach();

	::SelectObject(hDCMem, hOldMap);//选入上次的返回值   

	//释放   
	::DeleteObject(hBitMap);
	::DeleteDC(hDCMem);
	::ReleaseDC(hwnd, hWndDC);

	return true;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	WORD wBitCount;//位图中每象素所占字节数
	DWORD dwPaletteSize = 0;//定义调色板大小
	DWORD dwBmBitsSize = 0;//位图中像素字节大小
	DWORD dwDIBSize = 0;//位图文件大小
	DWORD dwWritten = 0;//写入文件字节数

	BITMAP Bitmap;//位图属性结构

	BITMAPFILEHEADER bmfHdr;//位图文件头结构

	BITMAPINFOHEADER bi;//位图信息头结构

	LPBITMAPINFOHEADER lpbi;//指向位图信息头结构
	//定义文件，分配内存句柄，调色板句柄
	HANDLE fh = NULL;
	HANDLE hDib = NULL;
	HANDLE hPal = NULL;
	HANDLE hOldPal = NULL;

	wBitCount = 24;

	GetObject(bitmapTemp, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//为位图内容分配内存
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//处理调色板
	hPal = GetStockObject(DEFAULT_PALETTE);
	HDC hDC;
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	else
	{
		return false;
	}

	// 获取该调色板下新的像素值
	GetDIBits(hDC, bitmapTemp, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//恢复调色板
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件
	fh = CreateFile(Common::CharToWChar(fileName).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return false;

	// 设置位图文件头
	bmfHdr.bfType = 0x4D42; // "BM "
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	// 写入位图文件头
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//清除
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return true;
}


void CPaintView::OnPaint()
{
	CPaintDC dc(this);
	CRect rectClient;
	GetClientRect(&rectClient);//获取窗口信息
	CDC dcMem;
	CBitmap bmp;
	BITMAP bitmap = { 0 };
	if (!bmp.m_hObject)
	{
		if (!bmp.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height()))//创建内存位图
		{
			return;
		}
	}
	else
	{
		bmp.GetBitmap(&bitmap);
		if (bitmap.bmWidth != rectClient.Width() || bitmap.bmHeight != rectClient.Height())
		{
			bmp.DeleteObject();
			if (!bmp.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height()))//创建内存位图
			{
				return;
			}
		}
	}
	if (!dcMem.m_hDC)
	{
		if (!dcMem.CreateCompatibleDC(&dc)) //依附窗口DC创建兼容的DC
		{
			return;
		}
	}
	dcMem.SelectObject(&bmp);//将内存位图选入内存dc
	//填充颜色
	dcMem.FillSolidRect(rectClient, RGB(250, 250, 250));   //填充颜色
	/*
	各种绘图操作在这里进行
	*/
	//设置字体
	static CFont font;
	if (NULL == font.m_hObject)
	{
		font.CreateFont(20,                                          //   nHeight   
			0,                                                   //   nWidth   
			0,                                                   //   nEscapement 
			0,                                                   //   nOrientation   
			FW_NORMAL,                                   //   nWeight   
			FALSE,                                           //   bItalic   
			FALSE,                                           //   bUnderline   
			0,                                                   //   cStrikeOut   
			ANSI_CHARSET,                             //   nCharSet   
			OUT_DEFAULT_PRECIS,                 //   nOutPrecision   
			CLIP_DEFAULT_PRECIS,               //   nClipPrecision   
			DEFAULT_QUALITY,                       //   nQuality   
			DEFAULT_PITCH | FF_SWISS,     //   nPitchAndFamily     
			_T("宋体"));
	}

	dcMem.SelectObject(&font);
	dcMem.SetBkMode(TRANSPARENT);
	dcMem.SetTextColor(RGB(0, 0, 0));

	if (isPaintUs)
	{
        PaintUsBk(dcMem);
    }
    else if (isPreView)
    {
        PaintPreViewBk(dcMem);
    }
    else
    {
        PaintBk(dcMem);
    }

	dc.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcMem, 0, 0, SRCCOPY);//绘制图片到主dc
}

void CPaintView::DrawTextVert(int x, int y, CString str, CDC*pDC)
{
    int id = pDC->SaveDC();

    CFont* pFont = pDC->GetCurrentFont();
    LOGFONT logFont;
    pFont->GetLogFont(&logFont);
    logFont.lfEscapement = -900;//900/10 = 90
    logFont.lfHeight = 14;
    wsprintf(logFont.lfFaceName, L"Arial");
    HFONT hFont = CreateFontIndirect(&logFont);
    pDC->SelectObject(hFont);

    pDC->TextOut(x, y, str);

	/*TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	long lHeigh = tm.tmHeight;
	int nLine = 0;
	for (int i = 0; i < str.GetLength(); i++)
	{
		if ((BYTE)str[i] > 127)
		{
			pDC->TextOut(x, y + nLine*lHeigh, str.Mid(i, 2));
			nLine++;
			i++;
		}
		else
		{
			pDC->TextOut(x, y + nLine*lHeigh, str.Mid(i, 1));
			nLine++;
		}
	}*/
    pDC->RestoreDC(id);
}


void CPaintView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	CRect wndRect;
	GetParent()->GetClientRect(wndRect);
	wndRect.top += 20;
	this->MoveWindow(wndRect);
	// TODO:  在此处添加消息处理程序代码
}

void CPaintView::PaintBk(CDC &memDc)
{
	CRect rc;
	GetClientRect(rc);
	rc.DeflateRect(50, 50);
	char buff[64] = { 0 };
	POINT ptRect[4] = { {-5,-5},{-5,5},{5,5},{5,-5} };
	POINT ptDraw[4];
	POINT pt[256];
    Graphics graphics(memDc.m_hDC);
    graphics.SetSmoothingMode(SmoothingModeHighQuality);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	int gridXnums = m_paint.x_days;	
	int gridYnums = vRange.size();
	if (!gridYnums || !gridXnums)
	{
		return;
	}
	int dx = rc.Width() / (gridXnums + 1);
	int dy = rc.Height() / (gridYnums + 1);

	CRect gridRect(rc.left, rc.top, rc.left + dx * (gridXnums + 1), rc.top + dy * (gridYnums+1));
	CPen gridPen(PS_DOT, 1, RGB(0, 0, 0));
	CPen *oldPen = memDc.SelectObject(&gridPen);
	CBrush MyBrush, *OldBrush;

	for (int i = 0; i <= gridXnums + 1; i++)
	{
		memDc.MoveTo(gridRect.left + i * dx, gridRect.bottom);
		memDc.LineTo(gridRect.left + i * dx, gridRect.top);
        if (i <= gridXnums && i > 0)
            /* 			::TextOut(memDc,gridRect.left + i * dx + 10, gridRect.bottom,CharToWChar(itoa(i,buff,10)).c_str(),wcslen(CharToWChar(itoa(i, buff, 10)).c_str()));*/
        {
            COleDateTime dt = m_paint.dtStart + COleDateTimeSpan(i-1, 0, 0, 0);
            DrawTextVert(gridRect.left + i * dx + 8, gridRect.bottom +3, dt.Format(_T("%m-%d")), &memDc);
        }
	}


	for (int j = 0; j <= gridYnums + 1; j++)
	{
		memDc.MoveTo(gridRect.left, gridRect.top + j * dy);
		memDc.LineTo(gridRect.right, gridRect.top + j * dy);
		if (j < vRange.size())
			::TextOut(memDc, gridRect.left - 40, gridRect.top + (j+1) * dy - 10, CharToWChar(vRange.at(vRange.size()-1 - j)).c_str(),wcslen(CharToWChar(vRange.at(vRange.size() - 1 - j)).c_str()));
	}

	memDc.SelectObject(oldPen);
	gridPen.Detach();

	CBrush gridBrush(RGB(255, 0, 0));
	CBrush *oldBrush = memDc.SelectObject(&gridBrush);

	for (int i = 0;i <vRange.size();i++)//画靶值线
	{
		if (m_paint.y_target == vRange.at(i))
		{
			CPen gridPen(PS_SOLID, 3, RGB(0, 0, 0));
			oldPen = memDc.SelectObject(&gridPen); 
            if (!MyBrush.m_hObject) MyBrush.DeleteObject();
			MyBrush.CreateSolidBrush(RGB(0, 0, 0));//创建画刷
			OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备山下文

			memDc.MoveTo(gridRect.left, gridRect.top + (vRange.size() - i) * dy);
			memDc.LineTo(gridRect.right, gridRect.top + (vRange.size() - i) * dy);
/*			DrawGon(memDc, 4, L"靶值");*/
		}
	}

    {
        CPen gridPen(PS_SOLID, 3, RGB(255, 0, 0));
        oldPen = memDc.SelectObject(&gridPen);
        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
        MyBrush.CreateSolidBrush(RGB(255, 0, 0));//创建红色画刷
        OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备山下文
        DrawGon(memDc, 1, L"质控结果");
        CPen gridPen2(PS_SOLID, 3, RGB(0, 0, 0));
        oldPen = memDc.SelectObject(&gridPen2);
        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
        MyBrush.CreateSolidBrush(RGB(0, 0, 0));//创建红色画刷
        OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备山下文
        DrawGon(memDc, 2, L"靶值");
    }
	int rangeHeight;	
	if (vValue1.size() == m_paint.x_days)
	{
		CPen gridPen(PS_SOLID, 3, RGB(255, 0, 0));
		oldPen = memDc.SelectObject(&gridPen);
        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
		MyBrush.CreateSolidBrush(RGB(255, 0, 0));//创建红色画刷
		OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备山下文
        std::map<int,std::vector<Point>> mapLines;
        int index = 0;
        Point ptGdipDraw;
		for (int i = 0; i < vValue1.size(); i++)
		{
			int h = GetHeightByRange(vValue1, i);
			pt[i].x = gridRect.left + (i+1) * dx;
			pt[i].y = gridRect.bottom - (int)(h * dy);
            ptGdipDraw.X = gridRect.left + (i + 1) * dx;
            ptGdipDraw.Y = gridRect.bottom - (int)(h * dy);
			for (int j = 0; j < 4; j++)
			{
				ptDraw[j].x = ptRect[j].x + pt[i].x;
				ptDraw[j].y = ptRect[j].y + pt[i].y;
			}
            if (!vValue1[i].empty() && vValue1[i] != "*")
            {
                memDc.Polygon(ptDraw, 4);
                mapLines[index].push_back(ptGdipDraw);
            }
            if (mapLines.size() > 0 && mapLines.find(index+1)==mapLines.end() && (vValue1[i].empty() || vValue1[i] == "*"))
            {
                index++;
            }
		}
		//DrawGon(memDc, 1, L"阳性");
		//memDc.Polyline(pt, m_paint.x_days);
        for (std::map<int, std::vector<Point>>::const_iterator itr = mapLines.cbegin(); itr != mapLines.cend(); itr++)
        {
            if (itr->second.size() > 1)
            {
                graphics.DrawLines(&Pen(Color(255, 0, 0), 2.0f), &(itr->second)[0], itr->second.size());
                }
        }
	}
	
	if (vValue2.size() == m_paint.x_days)
	{
		CPen gridPen(PS_SOLID, 3, RGB(0, 255, 0));
		oldPen = memDc.SelectObject(&gridPen);
        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
		MyBrush.CreateSolidBrush(RGB(0, 255, 0));//
		OldBrush = memDc.SelectObject(&MyBrush);
        std::map<int, std::vector<Point>> mapLines;
        int index = 0;
        Point ptGdipDraw;
		for (int i = 0; i < vValue2.size(); i++)
		{
			int h = GetHeightByRange(vValue2, i);
			pt[i].x = gridRect.left + (i+1) * dx;
			pt[i].y = gridRect.bottom - (int)(h * dy);
            ptGdipDraw.X = gridRect.left + (i + 1) * dx;
            ptGdipDraw.Y = gridRect.bottom - (int)(h * dy);
			for (int j = 0; j < 4; j++)
			{
				ptDraw[j].x = ptRect[j].x + pt[i].x;
				ptDraw[j].y = ptRect[j].y + pt[i].y;
			}
            if (!vValue2[i].empty() && vValue2[i] != "*")
            {
                memDc.Polygon(ptDraw, 4);
                mapLines[index].push_back(ptGdipDraw);
            }
            if (mapLines.size() > 0 && mapLines.find(index + 1) == mapLines.end() && (vValue2[i].empty() || vValue2[i] == "*"))
            {
                index++;
            }
		}
		//DrawGon(memDc, 2, L"阴性");
		//memDc.Polyline(pt, m_paint.x_days);
        for (std::map<int, std::vector<Point>>::const_iterator itr = mapLines.cbegin(); itr != mapLines.cend(); itr++)
        {
            if (itr->second.size() > 1)
            {
                graphics.DrawLines(&Pen(Color(0, 255, 0), 2.0f), &(itr->second)[0], itr->second.size());
            }
        }
	}

	/*if (vValue3.size() == m_paint.x_days)
	{
		CPen gridPen(PS_SOLID, 3, RGB(0, 0, 255));
		oldPen = memDc.SelectObject(&gridPen);
        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
		MyBrush.CreateSolidBrush(RGB(0, 0, 255));//
		OldBrush = memDc.SelectObject(&MyBrush);

		for (int i = 0; i < vValue3.size(); i++)
		{
			int h = GetHeightByRange(vValue3, i);
			pt[i].x = gridRect.left + (i+1) * dx;
			pt[i].y = gridRect.bottom - (int)(h * dy);
			for (int j = 0; j < 4; j++)
			{
				ptDraw[j].x = ptRect[j].x + pt[i].x;
				ptDraw[j].y = ptRect[j].y + pt[i].y;
			}
			memDc.Polygon(ptDraw, 4);
		}
		DrawGon(memDc,3,L"III");
		memDc.Polyline(pt, m_paint.x_days);
	}*/


	
	memDc.SelectObject(oldPen);
	memDc.SelectObject(oldBrush);
}

void CPaintView::PaintUsBk(CDC &memDc)
{
	CRect rc;
	GetClientRect(rc);
	rc.DeflateRect(50, 50);
	char buff[64] = { 0 };
	POINT ptRect[4] = { {-4,-4},{-4,4},{4,4},{4,-4} };
	POINT pt[256];
	POINT tempPos[2];
	int gridXnums = m_paint.x_days;
    Point ptGdipDraw[4];
    Graphics graphics(memDc.m_hDC);
    graphics.SetSmoothingMode(SmoothingModeHighQuality);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	m_max = m_paint.max;//两个for循环找出最大值建立坐标
	for (int i = 0;i < vValue1.size();i++)
	{
		if (atof(vValue1.at(i).c_str()) > m_max)
		{
			m_max = atof(vValue1.at(i).c_str());
		}
	}
	for (int i = 0; i < vValue2.size(); i++)
	{
		if (atof(vValue2.at(i).c_str()) > m_max)
		{
			m_max = atof(vValue2.at(i).c_str());
		}
	}
    for (int i = 0; i < vValue3.size(); i++)
    {
        if (atof(vValue3.at(i).c_str()) > m_max)
        {
            m_max = atof(vValue3.at(i).c_str());
        }
    }
	int gridYnums = m_max;
	if (!gridYnums || !gridXnums)
	{
		return;
	}

	double dx = rc.Width() / (double)(gridXnums + 1);
	double dy = rc.Height() / (double)(gridYnums);

	CRect gridRect(rc.left, rc.top, rc.left + dx * (gridXnums + 1), rc.top + dy * (gridYnums));

	CPen gridPen(PS_DOT, 1, RGB(0, 0, 0));
	CPen *oldPen = memDc.SelectObject(&gridPen);
	CBrush MyBrush, *OldBrush;




	for (int i = 0; i <= gridXnums + 1; i++)//x轴
	{
		memDc.MoveTo(gridRect.left + i * dx, gridRect.bottom);
		memDc.LineTo(gridRect.left + i * dx, gridRect.top);
        if (i <= gridXnums && i > 0)
        {
            COleDateTime dt = m_paint.dtStart + COleDateTimeSpan(i-1, 0, 0, 0);
            DrawTextVert(gridRect.left + i * dx + 8, gridRect.bottom + 3, dt.Format(_T("%m-%d")), &memDc);
            //DrawTextVert(gridRect.left + i * dx - 5, gridRect.bottom, CharToWChar(itoa(i, buff, 10)).c_str(), &memDc);
        }
/*			memDc.TextOut( gridRect.left + i * dx, gridRect.bottom, CharToWChar(itoa(i, buff, 10)).c_str(), wcslen(CharToWChar(itoa(i, buff, 10)).c_str()));*/
	}
	//绘制图像最大值及最小值 
	memDc.MoveTo(gridRect.left, gridRect.top);
	memDc.LineTo(gridRect.right, gridRect.top);
	
	memDc.MoveTo(gridRect.left, gridRect.top + (int)((gridYnums) * dy));
	memDc.LineTo(gridRect.right, gridRect.top + (int)((gridYnums)* dy));
	memDc.TextOut(gridRect.left - 40, gridRect.top - 10, CharToWChar(itoa(gridYnums, buff, 10)).c_str(),wcslen(CharToWChar(itoa(gridYnums, buff, 10)).c_str()));

	//靶值线	
	gridPen.DeleteObject();
	gridPen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	oldPen = memDc.SelectObject(&gridPen);
    if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
	MyBrush.CreateSolidBrush(RGB(0, 0, 0));//创建画刷
	OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备上下文

	tempPos[0].x = rc.left; tempPos[0].y = gridRect.top + (int)((gridYnums - m_paint.min) * dy);
	tempPos[1].x = rc.right; tempPos[1].y = gridRect.top + (int)((gridYnums - m_paint.min) * dy);
	memDc.MoveTo(tempPos[0].x = rc.left, tempPos[0].y);
	memDc.LineTo(tempPos[1].x, tempPos[1].y);
	if (m_paint.min != 0)
		memDc.TextOut(tempPos[0].x - 40, tempPos[0].y - 10 , CharToWChar(itoa(m_paint.min, buff, 10)).c_str(), wcslen(CharToWChar(itoa(m_paint.min, buff, 10)).c_str()));
	
	tempPos[0].x = rc.left; tempPos[0].y = gridRect.top + (int)((gridYnums - m_paint.max) * dy);
	tempPos[1].x = rc.right; tempPos[1].y = gridRect.top + (int)((gridYnums - m_paint.max) * dy);
	memDc.MoveTo(tempPos[0].x = rc.left, tempPos[0].y);
	memDc.LineTo(tempPos[1].x, tempPos[1].y);

	if (m_paint.max != gridYnums)
		memDc.TextOut(tempPos[0].x - 40, tempPos[0].y - 10, CharToWChar(itoa(m_paint.max, buff, 10)).c_str(), wcslen(CharToWChar(itoa(m_paint.max, buff, 10)).c_str()));
/*	DrawGon(memDc, 3, L"靶值");*/




	memDc.SelectObject(oldPen);
	gridPen.Detach();

	CBrush gridBrush(RGB(255, 0, 0));
	CBrush *oldBrush = memDc.SelectObject(&gridBrush);

    {
        CPen gridPen(PS_SOLID, 3, RGB(255, 0, 0));
        oldPen = memDc.SelectObject(&gridPen);
        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
        MyBrush.CreateSolidBrush(RGB(255, 0, 0));//创建红色画刷
        OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备山下文
        DrawGon(memDc, 1, L"质控结果");
        CPen gridPen2(PS_SOLID, 3, RGB(0, 0, 0));
        oldPen = memDc.SelectObject(&gridPen2);
        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
        MyBrush.CreateSolidBrush(RGB(0, 0, 0));//创建红色画刷
        OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备山下文
        DrawGon(memDc, 2, L"靶值");
    }

	int rangeHeight;
	if (vValue1.size() == m_paint.x_days)
	{
		CPen gridPen(PS_SOLID, 3, RGB(255, 0, 0));
		oldPen = memDc.SelectObject(&gridPen);

        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
		MyBrush.CreateSolidBrush(RGB(255, 0, 0));//创建红色画刷
		OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备上下文
        std::map<int, std::vector<Point>> mapLines;
        int index = 0;
        Point ptGdipDraw;
		for (int i = 0; i < vValue1.size(); i++)
		{
			pt[i].x = gridRect.left + (int)((i+1) * dx);
			pt[i].y = gridRect.top + (int)((gridYnums - atof(vValue1.at(i).c_str())) * dy);
            ptGdipDraw.X = gridRect.left + (int)((i + 1) * dx);
            ptGdipDraw.Y = gridRect.top + (int)((gridYnums - atof(vValue1.at(i).c_str())) * dy);
            POINT ptDraw[4];
            for (int j = 0; j < 4; j++)
            {
                ptDraw[j].x = ptRect[j].x + pt[i].x;
                ptDraw[j].y = ptRect[j].y + pt[i].y;
            }
            if (!vValue1[i].empty() && vValue1[i] != "*")
            {
                memDc.Polygon(ptDraw, 4);
                mapLines[index].push_back(ptGdipDraw);
            }
            if (mapLines.size() > 0 && mapLines.find(index + 1) == mapLines.end() && (vValue1[i].empty() || vValue1[i] == "*"))
            {
                index++;
            }
		}
		for (int i = 0; i < vValue1.size(); i++) 
        {
			//坐标线
			CPen gridPen(PS_DOT, 1, RGB(0, 0, 0));
			oldPen = memDc.SelectObject(&gridPen);
			memDc.MoveTo(gridRect.left, pt[i].y);
			memDc.LineTo(gridRect.right, pt[i].y);
		}
		gridPen.DeleteObject();
		gridPen.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		oldPen = memDc.SelectObject(&gridPen);
		//DrawGon(memDc, 1, L"P");
		//memDc.Polyline(pt, m_paint.x_days);
        for (std::map<int, std::vector<Point>>::const_iterator itr = mapLines.cbegin(); itr != mapLines.cend(); itr++)
        {
            if (itr->second.size() > 1)
            {
                graphics.DrawLines(&Pen(Color(255, 0, 0), 2.0f), &(itr->second)[0], itr->second.size());
            }
        }

	}


	if (vValue2.size() == m_paint.x_days)
	{
		CPen gridPen(PS_SOLID, 3, RGB(0, 255, 0));
		oldPen = memDc.SelectObject(&gridPen);

        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
		MyBrush.CreateSolidBrush(RGB(0, 255, 0));//创建画刷
		OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备上下文
        std::map<int, std::vector<Point>> mapLines;
        int index = 0;
        Point ptGdipDraw;
		for (int i = 0; i < vValue2.size(); i++)//先获取各个点
		{
			pt[i].x = gridRect.left + (int)((i + 1) * dx);
			pt[i].y = gridRect.top + (int)((gridYnums - atof(vValue2.at(i).c_str())) * dy);	
            ptGdipDraw.X = gridRect.left + (int)((i + 1) * dx);
            ptGdipDraw.Y = gridRect.top + (int)((gridYnums - atof(vValue2.at(i).c_str())) * dy);
            POINT ptDraw[4];
            for (int j = 0; j < 4; j++)
            {
                ptDraw[j].x = ptRect[j].x + pt[i].x;
                ptDraw[j].y = ptRect[j].y + pt[i].y;
            }
            if (!vValue2[i].empty() && vValue2[i] != "*")
            {
                memDc.Polygon(ptDraw, 4);
                mapLines[index].push_back(ptGdipDraw);
            }
            if (mapLines.size() > 0 && mapLines.find(index + 1) == mapLines.end() && (vValue2[i].empty() || vValue2[i] == "*"))
            {
                index++;
            }
		}
		
		for (int i = 0; i < vValue2.size(); i++)//绘制横坐标
		{
			CPen gridPen(PS_DOT, 1, RGB(0, 0, 0));
			oldPen = memDc.SelectObject(&gridPen);

			memDc.MoveTo(gridRect.left, pt[i].y);
			memDc.LineTo(gridRect.right, pt[i].y);
		}
		gridPen.DeleteObject();
		gridPen.CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
		oldPen = memDc.SelectObject(&gridPen);

		//memDc.Polyline(pt, m_paint.x_days);
		//DrawGon(memDc, 2,L"N");
        for (std::map<int, std::vector<Point>>::const_iterator itr = mapLines.cbegin(); itr != mapLines.cend(); itr++)
        {
            if (itr->second.size() > 1)
            {
                graphics.DrawLines(&Pen(Color(0, 255, 0), 2.0f), &(itr->second)[0], itr->second.size());
            }
        }
	}

    if (vValue3.size() == m_paint.x_days)
    {
        CPen gridPen(PS_SOLID, 3, RGB(0, 0, 255));
        oldPen = memDc.SelectObject(&gridPen);

        if (NULL != MyBrush.m_hObject) MyBrush.DeleteObject();
        MyBrush.CreateSolidBrush(RGB(0, 0, 255));//创建画刷
        OldBrush = memDc.SelectObject(&MyBrush);//选中画刷到设备上下文
        std::map<int, std::vector<Point>> mapLines;
        int index = 0;
        Point ptGdipDraw;
        for (int i = 0; i < vValue3.size(); i++)//先获取各个点
        {
            pt[i].x = gridRect.left + (int)((i + 1) * dx);
            pt[i].y = gridRect.top + (int)((gridYnums - atof(vValue3.at(i).c_str())) * dy);
            ptGdipDraw.X = gridRect.left + (int)((i + 1) * dx);
            ptGdipDraw.Y = gridRect.top + (int)((gridYnums - atof(vValue3.at(i).c_str())) * dy);
            POINT ptDraw[4];
            for (int j = 0; j < 4; j++)
            {
                ptDraw[j].x = ptRect[j].x + pt[i].x;
                ptDraw[j].y = ptRect[j].y + pt[i].y;
            }
            if (!vValue3[i].empty() && vValue3[i] != "*")
            {
                memDc.Polygon(ptDraw, 4);
                mapLines[index].push_back(ptGdipDraw);
            }
            if (mapLines.size() > 0 && mapLines.find(index + 1) == mapLines.end() && (vValue3[i].empty() || vValue3[i] == "*"))
            {
                index++;
            }
        }

        for (int i = 0; i < vValue3.size(); i++)//绘制横坐标
        {
            CPen gridPen(PS_DOT, 1, RGB(0, 0, 0));
            oldPen = memDc.SelectObject(&gridPen);

            memDc.MoveTo(gridRect.left, pt[i].y);
            memDc.LineTo(gridRect.right, pt[i].y);
        }
        gridPen.DeleteObject();
        gridPen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
        oldPen = memDc.SelectObject(&gridPen);

        //memDc.Polyline(pt, m_paint.x_days);
        //DrawGon(memDc, 2, L"N");
        for (std::map<int, std::vector<Point>>::const_iterator itr = mapLines.cbegin(); itr != mapLines.cend(); itr++)
        {
            if (itr->second.size() > 1)
            {
                graphics.DrawLines(&Pen(Color(0, 0, 255), 2.0f), &(itr->second)[0], itr->second.size());
            }
        }
    }
	memDc.SelectObject(oldPen);
	memDc.SelectObject(oldBrush);
}

void CPaintView::PaintPreViewBk(CDC &memDc)
{
	static CFont font;
    if (NULL == font.m_hObject)
    {
        font.CreateFont(14,                                          //   nHeight   
            0,                                                   //   nWidth   
            0,                                                   //   nEscapement 
            0,                                                   //   nOrientation   
            FW_NORMAL,                                   //   nWeight   
            FALSE,                                           //   bItalic   
            FALSE,                                           //   bUnderline   
            0,                                                   //   cStrikeOut   
            ANSI_CHARSET,                             //   nCharSet   
            OUT_DEFAULT_PRECIS,                 //   nOutPrecision   
            CLIP_DEFAULT_PRECIS,               //   nClipPrecision   
            DEFAULT_QUALITY,                       //   nQuality   
            DEFAULT_PITCH | FF_SWISS,     //   nPitchAndFamily     
            _T("宋体"));
    }

	memDc.SelectObject(&font);

	CRect rc;
	GetClientRect(rc);
	rc.DeflateRect(1, 0);
	char buff[64] = { 0 };
	POINT pt[256][256];
	POINT tempPos[2];
	

	m_max = m_paint.max;//两个for循环找出最大值建立坐标
	for (int i = 0; i < vValue1.size(); i++)
	{
		if (atof(vValue1.at(i).c_str()) > m_max)
		{
			m_max = atof(vValue1.at(i).c_str());
		}
	}
	int gridXnums = 5;
	int gridYnums = m_paint.x_days;
	if (!gridYnums || !gridXnums)
	{
		return;
	}

	double dx = WIDTHLEN;
	double dy = 16;

	CRect gridRect(rc.left, rc.top, rc.left + dx * (gridXnums), rc.top + dy * (gridYnums + 1));

	CPen gridPen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen *oldPen = memDc.SelectObject(&gridPen);
	CBrush MyBrush, *OldBrush;

	for (int i = 0; i <= gridXnums; i++)//x轴
	{
		memDc.MoveTo(gridRect.left + i * dx, gridRect.bottom);
		memDc.LineTo(gridRect.left + i * dx, gridRect.top);
		
	}
	memDc.TextOutW(gridRect.left + 0 * dx + XSTEP, gridRect.top + YSTEP, L"项目");
	memDc.TextOutW(gridRect.left + 1 * dx + XSTEP, gridRect.top + YSTEP, L"日期");
	memDc.TextOutW(gridRect.left + 2 * dx + XSTEP, gridRect.top + YSTEP, L"结果");
	memDc.TextOutW(gridRect.left + 3 * dx + XSTEP, gridRect.top + YSTEP, L"批次");
	memDc.TextOutW(gridRect.left + 4 * dx + XSTEP, gridRect.top + YSTEP, L"有效期");

	for (int j = 0; j <= gridYnums + 1; j++)
	{
		memDc.MoveTo(gridRect.left, gridRect.top + j * dy);
		memDc.LineTo(gridRect.right, gridRect.top + j * dy);
	}


	memDc.SelectObject(oldPen);
	gridPen.Detach();

	CBrush gridBrush(RGB(255, 0, 0));
	CBrush *oldBrush = memDc.SelectObject(&gridBrush);


	int rangeHeight;
	if (vValue1.size() == m_paint.x_days && (m_paint.priType == 1 || m_paint.priType == 4))
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < m_paint.x_days; j++)
			{
				pt[i][j].x = gridRect.left + (int)(i * dx);
				pt[i][j].y = gridRect.top + (int)((j + 1) * dy);
				if (i == 0)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(GetItemStringByIndex(m_paint.itmeIndex)).c_str());
				}
				if (i == 1)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValueDate.at(j)).c_str());
				}
				if (i == 2)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValue1.at(j)).c_str());
				}
				if (i == 3)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValueBatch.at(j)).c_str());
				}
				if (i == 4)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValueValidDate.at(j)).c_str());
				}
			}
		}
	}
	if (vValue2.size() == m_paint.x_days && (m_paint.priType == 2 || m_paint.priType == 5))
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < m_paint.x_days; j++)
			{
				pt[i][j].x = gridRect.left + (int)(i * dx);
				pt[i][j].y = gridRect.top + (int)((j + 1) * dy);
				if (i == 0)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(GetItemStringByIndex(m_paint.itmeIndex)).c_str());
				}
				if (i == 1)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValueDate.at(j)).c_str());
				}
				if (i == 2)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValue2.at(j)).c_str());
				}
				if (i == 3)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValueBatch.at(j)).c_str());
				}
				if (i == 4)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValueValidDate.at(j)).c_str());
				}
			}
		}
	}
	if (vValue3.size() == m_paint.x_days && m_paint.priType == 3)
	{
		for (int i = 0;i < 5;i++)
		{
			for (int j = 0;j < m_paint.x_days;j++)
			{
				pt[i][j].x = gridRect.left + (int)(i * dx);
				pt[i][j].y = gridRect.top + (int)((j + 1) * dy);
				if (i == 0)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(GetItemStringByIndex(m_paint.itmeIndex)).c_str());
				}
				if (i == 1)
				{
					memDc.TextOut(pt[i][j].x + XSTEP,pt[i][j].y + 2,CharToWChar(vValueDate.at(j)).c_str());
				}
				if (i == 2)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValue3.at(j)).c_str());
				}
				if (i == 3)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValueBatch.at(j)).c_str());
				}
				if (i == 4)
				{
					memDc.TextOut(pt[i][j].x + XSTEP, pt[i][j].y + 2, CharToWChar(vValueValidDate.at(j)).c_str());
				}
			}
		}
	}
	memDc.SelectObject(oldPen);
	memDc.SelectObject(oldBrush);
}

void CPaintView::PaintCurve(CDC &memDc,int x1, int y1, int x2, int y2)
{
	int i = 0;
	int offset = (y2 - y1) / (x2 - x1);
	while (x1 < x2)
	{
		x1 = x1 + 1;
		y1 = y1 + offset;
		::LineTo(memDc, x1, y1);
		::MoveToEx(memDc, x1, y1, NULL);
	}
}


LRESULT CPaintView::OnPaintBk(WPARAM i, LPARAM arg)
{
	vRange.clear();
	vValue1.clear();
	vValue2.clear();
	vValue3.clear();
	memcpy(&m_paint, (PPaintView)arg,sizeof(PaintView));
	SplitString(m_paint.y_range, vRange, ";");//可按多个字符来分隔;
	SplitString(m_paint.value1, vValue1, ";");//可按多个字符来分隔;
	SplitString(m_paint.value2, vValue2, ";");//可按多个字符来分隔;
	SplitString(m_paint.value3, vValue3, ";");//可按多个字符来分隔;
	Invalidate();
	return i;
}

LRESULT CPaintView::OnPaintUsBk(WPARAM i, LPARAM arg)
{
	if (i == 1)
	{
		isPaintUs = true;
		vRange.clear();
		vValue1.clear();
		vValue2.clear();
        vValue3.clear();
		memcpy(&m_paint, (PPaintView)arg, sizeof(PaintView));
		SplitString(m_paint.value1, vValue1, ";");//可按多个字符来分隔;
		SplitString(m_paint.value2, vValue2, ";");//可按多个字符来分隔;
        SplitString(m_paint.value3, vValue3, ";");//可按多个字符来分隔;
		Invalidate();
		return i;
	}
	else
	{
		isPreView = true;
		vRange.clear();
		vValue1.clear();
		vValue2.clear();
		vValue3.clear();
		vValueDate.clear();
		vValueValidDate.clear();
		vValueBatch.clear();

		memcpy(&m_paint, (PPaintView)arg, sizeof(PaintView));
		SplitString(m_paint.value1, vValue1, ";");//可按多个字符来分隔;
		SplitString(m_paint.value2, vValue2, ";");//可按多个字符来分隔;
		SplitString(m_paint.value3, vValue3, ";");//可按多个字符来分隔;
		SplitString(m_paint.valueDate, vValueDate, ";");//可按多个字符来分隔;
		SplitString(m_paint.valueValidDate, vValueValidDate, ";");//可按多个字符来分隔;
		SplitString(m_paint.valueBatch, vValueBatch, ";");//可按多个字符来分隔;

		vector<std::string>::iterator it;
		for (it = vValueDate.begin(); it != vValueDate.end();it++)
		{
			*it = it->substr(0, it->find(" "));
		}

		Invalidate();
		return i;
	}
		
}

void CPaintView::PaintLine()
{

}

void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

int CPaintView::GetHeightByRange(vector<string>& v, int i)
{
	for (int j = 0; j < vRange.size(); j++)
	{
		if (v.at(i) == "*")
		{
			v[i] = m_paint.y_target;
		}
		if (v.at(i) == vRange.at(j))
		{
			return j+1;
		}
	}
}

void CPaintView::DrawGon(CDC &memDc,int flag,const LPWSTR nType)
{
	POINT ptRect[4] = {0};
	int x = 120*flag;
	int y = 20;
	int side = 10;
	switch (flag)
	{
	case 0:
		ptRect[0].x = x; ptRect[0].y = y;
		ptRect[1].x = x + side; ptRect[1].y = y;
		ptRect[3].x = x; ptRect[2].y = y + side;
		ptRect[2].x = x + side; ptRect[3].y = y + side;
		memDc.TextOutW(ptRect[1].x + 5, ptRect[1].y - 4, nType);
		break;
	case 1:	
		ptRect[0].x = x;ptRect[0].y = y;
		ptRect[1].x = x+side; ptRect[1].y= y;
		ptRect[3].x = x;ptRect[2].y = y+side;
		ptRect[2].x = x+side;ptRect[3].y = y+side;
		memDc.TextOutW(ptRect[1].x + 5, ptRect[1].y - 4, nType);
		break;
	case 2 :
		ptRect[0].x = x; ptRect[0].y = y;
		ptRect[1].x = x + side; ptRect[1].y = y;
		ptRect[3].x = x; ptRect[2].y = y + side;
		ptRect[2].x = x + side; ptRect[3].y = y + side;
		memDc.TextOutW(ptRect[1].x + 5, ptRect[1].y - 4, nType);
		break;
	case 3 :
		ptRect[0].x = x; ptRect[0].y = y;
		ptRect[1].x = x + side; ptRect[1].y = y;
		ptRect[3].x = x; ptRect[2].y = y + side;
		ptRect[2].x = x + side; ptRect[3].y = y + side;
		memDc.TextOutW(ptRect[1].x + 5 , ptRect[1].y - 4, nType);
		break;
	case 4: 
		ptRect[0].x = x; ptRect[0].y = y;
		ptRect[1].x = x + side; ptRect[1].y = y;
		ptRect[3].x = x; ptRect[2].y = y + side;
		ptRect[2].x = x + side; ptRect[3].y = y + side;
		memDc.TextOutW(ptRect[1].x + 5, ptRect[1].y - 4, nType);
		break;
	default:
		break;
	}
	memDc.Polygon(ptRect, 4);
}

void CPaintView::PostNcDestroy()
{
	// TODO:  在此添加专用代码和/或调用基类
	if (!this)
	{
		delete this;
	}
}
