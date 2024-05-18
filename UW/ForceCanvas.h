#pragma once
#include "TodayList.h"


#define PAINTWIDTH 600
#define PAINTHIGHT 400
#define COFF (32/8)	//32为位置直径，8为细胞直径，COFF为直径换算系数
#define XSTEP (PAINTWIDTH/9)	//X轴每格长度
#define FONTSIZE 26
#define SMOOTH 20 //平滑处理系数
#define MAX_POINT 256

class ForceCanvas
{
public:
	ForceCanvas(int canvasWithd, int canvasHeight);
	~ForceCanvas();
	void DrawLine(int x1, int y1, int x2, int y2, COLORREF penColor, int penWidth);
	void DrawPictureFromBmpFile(CDialog * dlg, int pictureCtrlID, const char * fileName);
	void DrawImage(ThreadArgv* argv);
	void SetBackgroundColor(COLORREF backgroundColor);
	bool SaveBMP(const char * fileName);
	bool SaveJpg(const char * fileName);
	void PaintCurve(int ,int ,int,int);
	void PaintCurve(double*,double*,int);
	void CountSplinePoint(int*,int*, float,int);
protected:
	HDC memDc;
	HDC dc;
	HBITMAP hBitmap;
	HGDIOBJ hOldBm;
	int myCanvasWithd;
	int myCanvasHeight;
	int coefficient;
};
bool cmp(const pair<int, int>& a, const pair<int, int>& b);