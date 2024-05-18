#include "stdafx.h"
#include "ForceCanvas.h"
#include "Include/Common/String.h"
#include "MainFrm.h"
#include "DbgMsg.h"
#include "Spline.h"

using namespace SplineSpace;
using namespace Common;
using namespace Gdiplus;


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}


ForceCanvas::ForceCanvas(int canvasWithd, int canvasHeight)
	:myCanvasWithd(canvasWithd),
	myCanvasHeight(canvasHeight)
{
	char Inipath[256] = { 0 };
	GetModuleFileNameA(NULL, Inipath, MAX_PATH);
	PathRemoveFileSpecA(Inipath);
	PathAppendA(Inipath, "config\\setting.ini");
	coefficient = GetPrivateProfileIntA("DRBCImage", "coefficient", 4, Inipath);

	memDc = ::CreateCompatibleDC(NULL); //创建一个兼容dc
	dc = ::GetDC(NULL);
	hBitmap = ::CreateCompatibleBitmap(dc, canvasWithd, canvasHeight);
	hOldBm = ::SelectObject(memDc, hBitmap); //选入设备 保存老的设备以便恢复
}

ForceCanvas::~ForceCanvas()
{
	DeleteObject(hBitmap);
	DeleteDC(memDc);
	::ReleaseDC(NULL, dc);
}

void ForceCanvas::DrawLine(int x1, int y1, int x2, int y2, COLORREF penColor, int penWidth)
{
	HGDIOBJ hPen = ::CreatePen(PS_SOLID, penWidth, penColor);
	::SelectObject(memDc, hPen);
	::MoveToEx(memDc, x1, y1, NULL);
	::LineTo(memDc, x2, y2);
	::DeleteObject(hPen);
	//MoveToEx(memDcEx最后一个参数的意思
	//Pointer to a POINT structure that receives the previous current position.
	//If this parameter is a NULL pointer, the previous position is not returned.
}

void ForceCanvas::DrawPictureFromBmpFile(CDialog * dlg, int pictureCtrlID, const char * fileName)
{
	CStatic *pWnd = (CStatic *)dlg->GetDlgItem(pictureCtrlID);
	if (pWnd == 0)
		return;

	HANDLE filehandle = ::LoadImage(NULL, Common::CharToWChar(fileName).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if (filehandle != NULL)
	{
		CBitmap bmp;
		if (bmp.Attach(filehandle))
		{
			CDC * pDC = pWnd->GetDC();
			BITMAP bmpInfo;
			bmp.GetBitmap(&bmpInfo);
			CDC dcMemory;
			dcMemory.CreateCompatibleDC(pDC);
			dcMemory.SelectObject(&bmp);
			pDC->SetStretchBltMode(HALFTONE);
			//获取Static控件的大小范围
			CRect rect;
			pWnd->GetClientRect(&rect);
			pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMemory, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
			bmp.Detach();
		}
	}
}

bool cmp(const pair<int, int>& a, const pair<int, int>& b) {
	return a.second > b.second;
}

void ForceCanvas::DrawImage(ThreadArgv* argv)
{
	TASK_INFO & taskInfo = argv->info;
	
	CMainFrame* pMain = (CMainFrame*)argv->pMain;
/*	CTodayList *argv->pParam = new CTodayList;*/
/*	argv->pParam = argv->pParam;*/
	int sizeRangeNum = argv->RBCSize.size();
	HFONT hFont, hFont2;
	HPEN  pen_line, pen_line2, pen_rbc;

	pen_line = ::CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	pen_line2 = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	pen_rbc = ::CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

	HDC hDC;
	hFont = CreateFont(FONTSIZE,//   nHeight  
		0,//   nWidth  
		0,//   nEscapement  
		0,//   nOrientation  
		FW_NORMAL,//   nWeight  
		FALSE,//   bItalic  
		FALSE,//   bUnderline  
		0,//   cStrikeOut  
		ANSI_CHARSET,//   nCharSet  
		OUT_DEFAULT_PRECIS,//   nOutPrecision  
		CLIP_DEFAULT_PRECIS,//   nClipPrecision  
		DEFAULT_QUALITY,//   nQuality  
		DEFAULT_PITCH | FF_SWISS,//   nPitchAndFamily  
		L"微软雅黑");

	hFont2 = CreateFont(20,//   nHeight  
		0,//   nWidth  
		0,//   nEscapement  
		0,//   nOrientation  
		FW_NORMAL,//   nWeight  
		FALSE,//   bItalic  
		FALSE,//   bUnderline  
		0,//   cStrikeOut  
		ANSI_CHARSET,//   nCharSet  
		OUT_DEFAULT_PRECIS,//   nOutPrecision  
		CLIP_DEFAULT_PRECIS,//   nClipPrecision  
		DEFAULT_QUALITY,//   nQuality  
		DEFAULT_PITCH | FF_SWISS,//   nPitchAndFamily  
		L"微软雅黑");


	::SelectObject(memDc, hFont2);
	::SelectObject(memDc, pen_line);

	CRect rectClient(0, 0, PAINTWIDTH, PAINTHIGHT);


/*	::SelectObject(memDc,&pen_line);*/

 	::SetMapMode(memDc,MM_ANISOTROPIC);
// 
 	::SetWindowExtEx(memDc,PAINTWIDTH, PAINTHIGHT,NULL);
// 
// 	::SetViewportExt(rectClient.right, -rectClient.bottom);
// 
// 	::SetViewportOrg(rectClient.left + 10, rectClient.bottom - 30);
	::SetViewportExtEx(memDc,rectClient.right, -rectClient.bottom,NULL);
	::SetViewportOrgEx(memDc, rectClient.left + (FONTSIZE+10), rectClient.bottom - (FONTSIZE+10), NULL);
 	::SetTextAlign(memDc,TA_LEFT);



	::MoveToEx(memDc,0, 0,NULL);

	::LineTo(memDc,PAINTWIDTH - 40, 0);//绘制X轴

	::LineTo(memDc, PAINTWIDTH - 60, 10);

	::MoveToEx(memDc, PAINTWIDTH - 40, 0, NULL);

	::LineTo(memDc, PAINTWIDTH - 60, -10);//绘制x轴箭头
/*	int abscissa = PAINTHIGHT / sizeRangeNum;*/

	::SelectObject(memDc, pen_line2);
	for (int i = 0; i < 8; i++)

	{

		::MoveToEx(memDc, XSTEP * i, 0, NULL);

		::LineTo(memDc, XSTEP * i, 5);

		CString Str;

		Str.Format(_T("%d"), i + 4);

		if (i != 0)

		{

			::TextOut(memDc, XSTEP * i - 7, -2, Str, Str.GetLength());

		}

	}//绘制X轴坐标尺
	::SelectObject(memDc, pen_line);

	::MoveToEx(memDc,0, 0,NULL);

	::LineTo(memDc,0, PAINTHIGHT - 40);//绘制Y轴

	::LineTo(memDc, -10, PAINTHIGHT - 60);

	::MoveToEx(memDc, 0, PAINTHIGHT - 40, NULL);

	::LineTo(memDc, 10, PAINTHIGHT - 60);//绘制y轴箭头

	// 	for (int i = -5; i <= 5; i++)
	// 
	// 	{
	// 
	// 		::MoveToEx(memDc(-5, 10 * i);
	// 
	// 		::LineTo(memDc,5, 10 * i);
	// 
	// 		CString Str;
	// 
	// 		Str.Format(_T("%d"), i);
	// 
	// 		::TextOut(-2, 10 * i - 5, Str);
	// 
	// 	}//绘制Y轴坐标尺
	::SetBkMode(memDc, TRANSPARENT);
	::TextOut(memDc, PAINTWIDTH - 90, -5, _T("直径"), _tcslen(_T("直径")));
	::TextOut(memDc, -FONTSIZE - 5, PAINTHIGHT - 40, _T("数"), _tcslen(_T("数")));
	::TextOut(memDc, -FONTSIZE - 5, PAINTHIGHT - 55, _T("量"), _tcslen(_T("量")));

	// 改变绘图起点位置
	::SetViewportOrgEx(memDc, rectClient.left + (FONTSIZE + 10 + 6), rectClient.bottom - (FONTSIZE + 10 + 4), NULL);


	::SelectObject(memDc, hFont);
	::TextOut(memDc, PAINTWIDTH - 250, PAINTHIGHT - 40, L"平均直径(um):", wcslen(L"平均直径(um):"));
	::TextOut(memDc, PAINTWIDTH - 250, PAINTHIGHT - (40 + FONTSIZE), L"异常比例(%):", wcslen(L"异常比例(%):"));

	//弧度=X坐标/曲线宽度*角系数*π

	//Y坐标=振幅*曲线宽度*sin(弧度)

	//map默认排序
	//线程中遍历取得键值和value
	//画图
	if (!argv->RBCSize.size())
	{
		::DeleteObject(pen_rbc);
		::DeleteObject(hFont);
		::DeleteObject(pen_line);
		::DeleteObject(pen_line2);
		return;
	}
//旧yf 对Z轴高度进行处理
// 	vector<pair<int, int>> vec(argv->pParam->RBCSize.begin(), argv->pParam->RBCSize.end());
// 	//对线性的vector进行排序
//  	sort(vec.begin(), vec.end(),cmp);
// 	int max_y = vec.at(0).second;//排序得到最大VALUE
// 	float yf = (float)((float)(PAINTHIGHT - 100) / (float)max_y);

	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	int totalDiameter = 0;
	int cellnum = 0;

	std::map<int, int>::iterator it;
	
	float xf = (float)XSTEP / (float)coefficient;
	int x[MAX_POINT] = { 0 };
	int y[MAX_POINT] = { 0 };
	::SelectObject(memDc, pen_rbc);

	it = argv->RBCSize.begin();//第一个起始点
	
	x1 = (it->first -1 - 4 * coefficient) *xf;
	y1 = 0;
	::MoveToEx(memDc, x1, (int)y1, NULL);
	x1 = it->first - 1;
// 	for (int k = 4 * coefficient; k < 10 * coefficient; k++)//当k==32时，x轴在32既是8um //折线图画法
// 	{
// 		for (it = argv->pParam->RBCSize.begin();
// 			it != argv->pParam->RBCSize.end();
// 			++it)
// 		{
// 			x = it->first;
// 			y = it->second;
// 			if (k == x)
// 			{		
// 				totalDiameter += x*y;
// 				cellnum += y;
// 				break;
// 			}
// 			x = k;
// 			y = 0;
// 		}
// 		::LineTo(memDc, (int)(x - 4 * coefficient) *xf, (float)y * yf);
// 		::MoveToEx(memDc, (int)(x - 4 * coefficient)*xf, (float)y * yf, NULL);
// 	}
	x[0] = it->first - 1;
	y[0] = 0;
	int i = 1;
	
	for (it = argv->RBCSize.begin();
		it != argv->RBCSize.end();
		++it)
	{
		x2 = it->first;
		y2 = it->second;
		totalDiameter += x2*y2;
		cellnum += y2;

		x[i] = x2;
		y[i] = y2;
/*			PaintCurve((x1 - 4 * coefficient) *xf, (float)y1 * yf, (x2 - 4 * coefficient) *xf, (float)y2 * yf);*///折线图
		i++;
	}
	x[i] = x2 + 1;
	y[i] = 0;
	CountSplinePoint(x, y, xf,argv->RBCSize.size()+2);
	
	//::TextOut(memDc, PAINTWIDTH - 250, PAINTHIGHT - (40+FONTSIZE), L"异常比例(%):", wcslen(L"异常比例(%):"));
	CString averDiameter, DRBCratio;	
	if ((taskInfo.us_info.us_node[0].us_res + taskInfo.us_info.us_node[1].us_res) > 0 && (taskInfo.us_info.us_node[0].us_res + taskInfo.us_info.us_node[1].us_res) < 9999)
	{
		double r = taskInfo.us_info.us_node[1].us_res * 100 / (taskInfo.us_info.us_node[0].us_res + taskInfo.us_info.us_node[1].us_res);
		if (taskInfo.us_info.us_node[0].us_res < 0.000000001f && taskInfo.us_info.us_node[1].us_res > 0.000000001f)
		{
			DRBCratio.Format(L"%.0f", r);
		}
		else if (r >= 10.f)
		{
			DRBCratio.Format(L"%.1f", r);
		}
		else
		{
			DRBCratio.Format(L"%.2f", r);
		}
	}
	else
	{
		DRBCratio.Format(L"%s", L"0");
	}
	
	::TextOut(memDc, PAINTWIDTH - 100, PAINTHIGHT - (FONTSIZE+40), DRBCratio, DRBCratio.GetLength());
	if (!cellnum)
		cellnum = 1;

	averDiameter.Format(L"%.2f", (float)(((float)totalDiameter / (float)cellnum) / (float)coefficient));
	DBG_MSG("debug:totalDiameter = %d,cellnum = %d,coefficient = %d,averDiameter = %s", totalDiameter, cellnum, coefficient, averDiameter);
	//::TextOut(memDc, PAINTWIDTH - 250, PAINTHIGHT - 40, L"平均直径(um):", wcslen(L"平均直径(um):"));
	::TextOut(memDc, PAINTWIDTH - 100, PAINTHIGHT - 40, averDiameter, averDiameter.GetLength());

	::DeleteObject(pen_rbc);
	::DeleteObject(hFont);
	::DeleteObject(hFont2);
	::DeleteObject(pen_line);
	::DeleteObject(pen_line2);
}
	

bool ForceCanvas::SaveBMP(const char * fileName)
{
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

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
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
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
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

bool ForceCanvas::SaveJpg(const char * fileName)
{
	Gdiplus::Bitmap *bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);
	if (bitmap)
	{
		CLSID Clsid;
		GetEncoderClsid(L"image/jpeg", &Clsid);
		return bitmap->Save(StringToWstring(fileName).c_str(), &Clsid) == Gdiplus::Status::Ok;
	}

	return false;
}

void ForceCanvas::PaintCurve(int x1, int y1, int x2, int y2)
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

void ForceCanvas::PaintCurve(double* x0, double* y0,int num)
{
	/*double x[SMOOTH*MAX_POINT];	//插值点
	double y[SMOOTH*MAX_POINT];

	SplineInterface* sp = new Spline(x0, y0, num);	//使用接口，且使用默认边界条件
	sp->AutoInterp(SMOOTH*MAX_POINT, x, y);

	double max = y[0];
	double paintCoff = 1;

	for (int j = 0; j < SMOOTH*MAX_POINT; j++)
	{
		if (y[j] > max)
			max = y[j];
	}*/

	double max = 0.f;
	for (int j = 0; j < num; j++)
	{
		if (y0[j] > max)
			max = y0[j];
	}
	double paintCoff = (double)((PAINTHIGHT - 100) / (double)max);//根据三次插样算法调整曲线Z轴高度
	std::vector<PointF> points;
	for (int j = 0; j < num; j++)
	{
		points.emplace_back(x0[j], y0[j] * paintCoff);
	}
	Graphics graphics(memDc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Pen pen(Color::Red, 2.0f);
	graphics.DrawCurve(&pen, &points[0], points.size());
    
	return;


	/*for (int i = 0; i < SMOOTH*MAX_POINT; i++)
	{
		double dX = x[i];
		if (dX <= 0)
		{
			continue;
		}
		double dY = y[i];
		if (dY <= 0)
		{
			dY = 0;
		}
		int x = dX;
		int y = dY*paintCoff;
 		::LineTo(memDc, x, y);
	}*/
}

void ForceCanvas::CountSplinePoint(int* x1, int* y1, float xf, int num)
{
/*	double a = 0,b = 0;*/
	double x0[MAX_POINT] = { 0 }, y0[MAX_POINT] = {0};
	for (int i = 0; i < num;i++)
	{
		x0[i] = (double)((x1[i] - 4 * coefficient) *xf);
		y0[i] = (double)(y1[i]);
// 		a += x0[i] * y0[i];
// 		b += y0[i];
	}
/*	double c = a / (double)b;*/
	PaintCurve(x0, y0,num);//三次样条插值算法
}

void ForceCanvas::SetBackgroundColor(COLORREF backgroundColor)
{
	HBRUSH brush = CreateSolidBrush(backgroundColor);
	RECT rect = { 0, 0, myCanvasWithd, myCanvasHeight };
	HGDIOBJ oldBrush = SelectObject(memDc, brush);
	FillRect(memDc, &rect, brush);
	SelectObject(memDc, oldBrush);
	DeleteObject(brush);
}
