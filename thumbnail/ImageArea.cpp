// ImageArea.cpp : 实现文件
//

#include "stdafx.h"
#include "thumbnail.h"
#include "ImageArea.h"
#include "Thumbnail1.h"

// CImageArea

IMPLEMENT_DYNAMIC(CImageArea, CStatic)

CImageArea::CImageArea()
{

}

CImageArea::~CImageArea()
{
}



BEGIN_MESSAGE_MAP(CImageArea, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CImageArea 消息处理程序

void CImageArea::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	CThumbnail1*  pWnd = static_cast<CThumbnail1*>(GetParent());
	if(pWnd->m_ListThumbnail.GetItemCount() != 0)
		pWnd->DrawSelectedImage();

}
