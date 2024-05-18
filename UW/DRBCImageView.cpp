#include "stdafx.h"
#include "DRBCImageView.h"
#include <map>
#include "MainFrm.h"
CDRBCImageView::CDRBCImageView()
{
}


CDRBCImageView::~CDRBCImageView()
{
}


void CDRBCImageView::OnDraw(CDC* pDC)
{
	// TODO:  在此添加专用代码和/或调用基类
// 	CMFC_CDCDoc* pDoc = GetDocument();
// 	ASSERT_VALID(pDoc);
// 	if (!pDoc)
// 		return;
/*	PaintDRBCImage();*/

}


void CDRBCImageView::PaintDRBCImage(ThreadArgv* argv)
{
// 	TASK_INFO* taskInfo = new TASK_INFO;
// 	ZeroMemory(taskInfo,sizeof(TASK_INFO));
// 	memcpy(taskInfo, &argv->info,sizeof(TASK_INFO));
// 
// 	CDC *pDC = GetDC();
// 	CMainFrame* pMain = (CMainFrame*)argv->pMain;
// 	char recImagePath[256] = { 0 };
// 	GetModuleFileNameA(NULL, recImagePath, MAX_PATH);
// 	PathRemoveFileSpecA(recImagePath);
// 
// 	PathAppendA(recImagePath, "\\RecGrapReslut");
// 	int sizeRangeNum = pMain->m_wndTodayList.RBCSize.size();
// 
// 	sprintf(recImagePath, "%s\\%d", recImagePath,taskInfo->main_info.nID);
// 	CFont font;
// 	font.CreateFont(
// 		14,//   nHeight  
// 		0,//   nWidth  
// 		0,//   nEscapement  
// 		0,//   nOrientation  
// 		FW_NORMAL,//   nWeight  
// 		FALSE,//   bItalic  
// 		FALSE,//   bUnderline  
// 		0,//   cStrikeOut  
// 		ANSI_CHARSET,//   nCharSet  
// 		OUT_DEFAULT_PRECIS,//   nOutPrecision  
// 		CLIP_DEFAULT_PRECIS,//   nClipPrecision  
// 		DEFAULT_QUALITY,//   nQuality  
// 		DEFAULT_PITCH | FF_SWISS,//   nPitchAndFamily  
// 		L"微软雅黑");
// 	CFont *OldFont = pDC->SelectObject(&font);
// 	CRect rectClient(0,0,250,220);
// 	CPen pen_text, pen_line, pen_rbc;
// 
// 
// 	pen_line.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
// 	pen_rbc.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
// 	pen_text.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
// 
// 	pDC->SelectObject(&pen_line);
// 
// 
// 	pDC->SetMapMode(MM_ANISOTROPIC);
// 
// 	pDC->SetWindowExt(250, 220);
// 
// 	pDC->SetViewportExt(rectClient.right, -rectClient.bottom);
// 
// 	pDC->SetViewportOrg(rectClient.left + 10, rectClient.bottom - 30);
// 
// 	pDC->SetTextAlign(TA_RIGHT);
// 
// 	pDC->TextOut(60, 190, L"异常比例(%):");
// 	pDC->TextOut(120, 190, L"平均直径(um):");
// 
// 	pDC->MoveTo(0, 0);
// 
// 	pDC->LineTo(220, 0);//绘制X轴
// 
// 	pDC->LineTo(210, 5);
// 
// 	pDC->MoveTo(220, 0);
// 
// 	pDC->LineTo(210, -5);//绘制x轴箭头
// 	int abscissa = 200 / sizeRangeNum;
// 	for (int i = 0; i <= 10; i++)
// 
// 	{
// 		
// 		pDC->MoveTo(20 * i, 0);
// 
// 		pDC->LineTo(20 * i, 5);
// 
// 		CString Str;
// 
// 		Str.Format(_T("%d"), i + 3);
// 
// 		if (i != 0)
// 
// 		{
// 
// 			pDC->TextOut(20 * i - 2, -2, Str);
// 
// 		}
// 
// 	}//绘制X轴坐标尺
// 
// 	pDC->MoveTo(0, 0);
// 
// 	pDC->LineTo(0, 180);//绘制Y轴
// 
// 	pDC->LineTo(-5, 170);
// 
// 	pDC->MoveTo(0, 180);
// 
// 	pDC->LineTo(5, 170);//绘制y轴箭头
// 
// 	// 	for (int i = -5; i <= 5; i++)
// 	// 
// 	// 	{
// 	// 
// 	// 		pDC->MoveTo(-5, 10 * i);
// 	// 
// 	// 		pDC->LineTo(5, 10 * i);
// 	// 
// 	// 		CString Str;
// 	// 
// 	// 		Str.Format(_T("%d"), i);
// 	// 
// 	// 		pDC->TextOut(-2, 10 * i - 5, Str);
// 	// 
// 	// 	}//绘制Y轴坐标尺
// 
// 	pDC->TextOut(220, -5, _T("x"));
// 
// 	pDC->TextOut(-10, 170, _T("y"));
// 
// 
// 	pDC->SelectObject(&pen_rbc);
// 
// 
// 	//弧度=X坐标/曲线宽度*角系数*π
// 
// 	//Y坐标=振幅*曲线宽度*sin(弧度)
// 
// 	//map默认排序
// 	//线程中遍历取得键值和value
// 	//画图
// 	int x = 0;
// 	int y = 0;
// 	int totalDiameter = 0;
// 	int cellnum = 0;
// 	pDC->MoveTo((int)x, (int)y);
// 	std::map<int, int>::iterator it;
// 	for (it = pMain->m_wndTodayList.RBCSize.begin();
// 		it != pMain->m_wndTodayList.RBCSize.end();
// 		++it)
// 	{
// 		x = it->first;
// 		y = it->second;
// 		pDC->LineTo((int)x * 30, (int)y * 5);
// 		pDC->MoveTo((int)x * 30, (int)y * 5);
// 		totalDiameter += x*y;
// 		cellnum += y;
// 	}
// 	CString averDiameter, DRBCratio;
// 	averDiameter.Format(L"%.2f", (float)totalDiameter/cellnum);
// 	DRBCratio.Format(L"%.2f", taskInfo->us_info.us_node[0].us_res / (taskInfo->us_info.us_node[0].us_res + taskInfo->us_info.us_node[1].us_res));
// 	pDC->TextOut(90, 190,DRBCratio);
// 	pDC->TextOut(150, 190, averDiameter);
// 
// 	pen_text.DeleteObject();
// 
// 	pen_line.DeleteObject();
// 
// 	int charLen = strlen(recImagePath);
// 	CString strPath;
// 	int len = MultiByteToWideChar(CP_ACP, 0, recImagePath, charLen, NULL, 0);
// 	TCHAR *buf = new TCHAR[len + 1];
// 	//计算多字节字符的大小，按字符计算。
// 	//多字节编码转换成宽字节编码
// 	MultiByteToWideChar(CP_ACP, 0, recImagePath, charLen, buf, len + 1);
// 	buf[len] = '\0';
// 	strPath.Format(_T("%s"), buf);
// 	SaveDRBCImage(strPath);
// 	delete taskInfo;
}

void CDRBCImageView::SaveDRBCImage(CString strPath)
{

	CClientDC dc(this);
	CRect rect;  

	GetClientRect(&rect);                  //获取画布大小    
	HBITMAP hbitmap = CreateCompatibleBitmap(dc, rect.right - rect.left, rect.bottom - rect.top);       //创建兼容位图  

	HDC hdc = CreateCompatibleDC(dc);      //创建兼容DC，以便将图像保存为不同的格式    
	HBITMAP hOldMap = (HBITMAP)SelectObject(hdc, hbitmap);  //将位图选入DC，并保存返回值   

	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, dc, 0, 0, SRCCOPY);        //将屏幕DC的图像复制到内存DC中    

	CImage image;
	image.Attach(hbitmap);                //将位图转化为一般图像     

	CString strFileName;          //如果用户没有指定文件扩展名，则为其添加一个  

	strFileName.Format(_T("%s\\%s"), strPath, L"DRBC.jpg");
	
	if (PathFileExists(strFileName))
	{
		DeleteFile(strFileName);
	}
	//AfxMessageBox(saveFilePath);               //显示图像保存的全路径(包含文件名)    
	HRESULT hResult = image.Save(strFileName);     //保存图像    
	image.Detach();
	SelectObject(hdc, hOldMap);
}

BOOL CDRBCImageView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	return CView::PreTranslateMessage(pMsg);
}
BEGIN_MESSAGE_MAP(CDRBCImageView, CView)
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


int CDRBCImageView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
