// DRBCImage.cpp : 实现文件
//

#include "stdafx.h"
#include "DRBCImage.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "Include/Common/String.h"

using namespace Common;

// CDRBCImage 对话框

IMPLEMENT_DYNAMIC(CDRBCImage, CDialogEx)

CDRBCImage::CDRBCImage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDRBCImage::IDD, pParent)
{
	pView = new CDRBCImageView;
	showImage = new CStatic;
}

CDRBCImage::~CDRBCImage()
{
	delete pView;
	delete showImage;
}

void CDRBCImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDRBCImage, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDRBCImage 消息处理程序




void CDRBCImage::GetTaskID(int id)
{
	pView->nID = id;
}

void CDRBCImage::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect wndRect;
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->GetClientRect(wndRect);
	this->MoveWindow(wndRect.right -  360,wndRect.bottom - 240 , 360, 240);
	this->GetClientRect(wndRect);
	showImage->MoveWindow(0, 0, wndRect.Width(), wndRect.Height());
// 	pView->MoveWindow(wndRect.left, wndRect.top, cx, cy);
	// TODO:  在此处添加消息处理程序代码
}

void CDRBCImage::DrawPictureFromBmpFile(CDialog * dlg, int pictureCtrlID, const char * fileName)
{

	CStatic *pWnd = (CStatic *)dlg->GetDlgItem(pictureCtrlID);

	if (pWnd == 0)
		return;

	CRect wndRect;
	this->GetClientRect(wndRect);
	ATL::CImage image;
	if (SUCCEEDED(image.Load(Common::CharToWChar(fileName).c_str())))
	{
		CDC * pDC = pWnd->GetDC();
		if (pDC)
		{
			pDC->SetStretchBltMode(HALFTONE);
			image.Draw(pDC->GetSafeHdc(), wndRect);
			ReleaseDC(pDC);
		}
	}
}
/*			bmp.Detach();*/
// 			BITMAPINFO *pBmpInfo;       //记录图像细节  
// 			BYTE *pBmpData;             //图像数据  
// 			BITMAPFILEHEADER bmpHeader; //文件头  
// 			BITMAPINFOHEADER bmpInfo;   //信息头  
// 			CFile bmpFile;              //记录打开文件  
// 
// 			//以只读的方式打开文件 读取bmp图片各部分 bmp文件头 信息 数据  
// 			if (!bmpFile.Open(Common::CharToWChar(fileName).c_str(), CFile::modeRead | CFile::typeBinary ))
// 				return;
// 			if (bmpFile.Read(&bmpHeader, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER))
// 				return;
// 			if (bmpFile.Read(&bmpInfo, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER))
// 				return;
// 			pBmpInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER)];
// 			//为图像数据申请空间  
// 			memcpy(pBmpInfo, &bmpInfo, sizeof(BITMAPINFOHEADER));
// 			DWORD dataBytes = bmpHeader.bfSize - bmpHeader.bfOffBits;
// 			pBmpData = (BYTE *)new char[dataBytes];
// 			bmpFile.SeekToBegin();
// /*			bmpFile.Seek(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), CFile::begin);*/
// 			bmpFile.Read(pBmpData, dataBytes);
// 			bmpFile.Close();

			//显示图像  
// 			CWnd *pWnd = GetDlgItem(pictureCtrlID); //获得pictrue控件窗口的句柄  
// 			CRect rect;
// 			pWnd->GetClientRect(&rect); //获得pictrue控件所在的矩形区域  
// 			CDC *pDC = pWnd->GetDC(); //获得pictrue控件的DC  
// 			pDC->SetStretchBltMode(COLORONCOLOR);
// 			StretchDIBits(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0,
// 				bmpInfo.biWidth, bmpInfo.biHeight, pBmpData, pBmpInfo, DIB_RGB_COLORS, SRCCOPY);


int CDRBCImage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetWindowText(L"红细胞位相图");
	showImage->Create(L"", WS_CHILD | WS_VISIBLE, CRect(0, 0, 300,200), this, IDC_STATIC_SHOWIMAGE);
// 	if (pView)
// 	{
// 		pView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 250, 220), this, AFX_IDW_PANE_LAST);
// 	}
	showImage->ModifyStyle(0, BS_OWNERDRAW);
	return 0;
}


BOOL CDRBCImage::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDRBCImage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	CRect wndRect;
	this->GetClientRect(wndRect);
	ATL::CImage image;
	if (SUCCEEDED(image.Load(Common::CharToWChar(dRBCImagePath).c_str())))
	{
		dc.SetStretchBltMode(HALFTONE);
		image.Draw(dc.GetSafeHdc(), wndRect);
	}
	return;

	
}
