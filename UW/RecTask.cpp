#include "StdAfx.h"
#include "RecTask.h"
#include <shlwapi.h>
#include <malloc.h>
//#include "ximage.h"
#include "..\..\..\inc\CxImage\ximage.h"
#include "DbgMsg.h"


#pragma comment(lib, "shlwapi.lib")

#pragma comment(lib, "..\\..\\..\\lib\\Thinterface.lib")

/*
#pragma comment(lib, ".\\lib\\png.lib")
#pragma comment(lib, ".\\lib\\Jpeg.lib")
#pragma comment(lib, ".\\lib\\cximage.lib")
#pragma comment(lib, ".\\lib\\zlib.lib")
*/

#ifdef _DEBUG
#ifdef _UNICODE
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\tiff.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\mng.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\jasper.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\libpsd.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\libdcr.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\zlib.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\png.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\Jpeg.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\jbig.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug_Unicode\\cximage.lib")
#else
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\tiff.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\mng.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\jasper.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\libpsd.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\libdcr.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\zlib.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\png.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\Jpeg.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\jbig.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Debug\\cximage.lib")
#endif // _UNICODE
#else
#ifdef _UNICODE
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\tiff.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\mng.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\jasper.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\libpsd.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\libdcr.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\zlib.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\png.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\Jpeg.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\jbig.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release_Unicode\\cximage.lib")
#else
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\tiff.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\mng.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\jasper.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\libpsd.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\libdcr.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\zlib.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\png.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\Jpeg.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\jbig.lib")
#pragma comment(lib, "..\\..\\..\\lib\\ximage\\vc12\\x86\\Release\\cximage.lib")
#endif // _UNICODE
#endif // _DEBUG


//#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")


#define  RECT_WIDTH(x) (x.right - x.left)
#define  RECT_HEIGHT(x) (x.bottom - x.top)

CRecTask::CRecTask()
{
}

KSTATUS CRecTask::GetRecGrapTaskInfoWhenFinish(PCHAR pTaskImagePath, PTASK_REC_INFO &task_rec_info)
{
// 	if( PathFileExistsA(pTaskImagePath) == FALSE )
// 		return STATUS_NOT_FOUND_FILE;
	
	return THInterface_QueryRecGrapInfo(pTaskImagePath, &task_rec_info);
	
}

void CRecTask::ScalCellRect(RECT &rt)
{
	const int iSquareLen = 48;
	const int iWidth = rt.right - rt.left;
	const int iHeight = rt.bottom - rt.top;
	
	//细胞图像很小时居中显示
    if ((iWidth < iSquareLen) && (iHeight < iSquareLen))
	{
		rt.left = (iSquareLen - iWidth)/2;
		rt.right = rt.left + iWidth;
		rt.top = (iSquareLen - iHeight)/2;
		rt.bottom = rt.top + iHeight;
		return;
	}
	
	if (iWidth > iHeight)//宽度大于高度
	{
		rt.left = 0;
		rt.right = iSquareLen;
		rt.top = (iSquareLen - iHeight * iSquareLen / iWidth) / 2;
		rt.bottom = rt.top + iHeight * iSquareLen / iWidth;
	}
	else if (iWidth < iHeight)//宽度小于高度
	{
		rt.top = 0;
		rt.bottom = iSquareLen;
		rt.left = (iSquareLen - iWidth * iSquareLen / iHeight) / 2;
		rt.right = rt.left + iWidth * iSquareLen / iHeight;
	}
	else if (iWidth == iHeight)//宽度等于高度
	{
		rt.left = 0;
		rt.right = iSquareLen;
		rt.top = 0;
		rt.bottom = iSquareLen;
	}
	else
	{}
}

void   CRecTask::DrawBackGround(HDC &hMemDC)
{

	HPEN    hPen = CreatePen(PS_SOLID, 1, 0xCCCCCC);
	HBRUSH  hBrush = CreateSolidBrush(0xEEEEEE);
	HGDIOBJ hOldPen = SelectObject(hMemDC, hPen);
	HGDIOBJ hOldBrush = SelectObject(hMemDC, hBrush);

	Rectangle(hMemDC, 0, 0, REC_GRAP_CX, REC_GRAP_CY);
	SelectObject(hMemDC, hOldPen);
	SelectObject(hMemDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	
}

BOOL CRecTask::ScalSpecialCellRect(RECT&SrcRC, RECT &ScalRC)
{
	int   nSrcWidth = RECT_WIDTH(SrcRC), nSrcHeight = RECT_HEIGHT(SrcRC);

	if( nSrcWidth <= REC_GRAP_CX && nSrcHeight <= REC_GRAP_CY )
		return FALSE;

	ZeroMemory(&ScalRC, sizeof(RECT));
	ScalRC.right = SrcRC.right;
	ScalRC.bottom = SrcRC.bottom;
	
	if( nSrcWidth > REC_GRAP_CX )
	{
		ScalRC.left  = (nSrcWidth - REC_GRAP_CX) >> 1;
		ScalRC.left += SrcRC.left;
		ScalRC.right = ScalRC.left + REC_GRAP_CX;
	}
	
	if( nSrcHeight > REC_GRAP_CY )
	{
		ScalRC.top    = (nSrcHeight - REC_GRAP_CY) >> 1;
		ScalRC.top   += SrcRC.top; 
		ScalRC.bottom = ScalRC.top + REC_GRAP_CY;
	}


	return TRUE;

}

KSTATUS CRecTask::GetPickupRecCellGrap(PCHAR  pImageFolder, USHORT  nGrapIndex, UCHAR  nGrapSize, PCELL_FULL_INFO_EX &cell_info,CListGroupView *pListGroupView,BOOL  bBuildMap, PSIZE pGPX)
{
	int                  cx,cy, sx,sy;
	HDC                  hDC, hMemDC, hMemDC1;
	BOOL                 bSkipNormalScal, bOnlyGetSize = FALSE;
	char                 GrapPath[MAX_PATH], FileName[MAX_PATH];
	ULONG                j, mask;
	KSTATUS              status;
	HBITMAP              hRecBitmap = NULL, hOldBitmap, hOldBitmap1;
	CxImage              jpg_image;
	PCELL_FULL_INFO      t_cell_info;


	mask = MAKE_REC_FILE_NAME(nGrapIndex, nGrapSize);

	status = THInterface_QueryDevCaps(T_DEV_REC_TYPE, RecGrapFileName, (PCHAR)&mask, sizeof(ULONG), FileName, MAX_PATH, 0);
	if( status != STATUS_SUCCESS )
		return status;
	status = THInterface_QueryRecCellInfo(pImageFolder, nGrapIndex, nGrapSize, &t_cell_info);
	if( status != STATUS_SUCCESS )
	{
		if( pGPX == NULL )
			return status;

		bOnlyGetSize = TRUE;
		goto load_jpg_loc;
	}

	if( t_cell_info->cCellCounter == 0 )
	{
		THInterface_FreeSysBuffer(t_cell_info);
		status = STATUS_NOT_FOUND_VALUE;
		if( pGPX == NULL )
			return status;

		bOnlyGetSize = TRUE;

		goto load_jpg_loc;
	}

	cell_info = (PCELL_FULL_INFO_EX)malloc(sizeof(CELL_DETAIL_INFO_EX) * t_cell_info->cCellCounter + sizeof(CELL_FULL_INFO_EX));
	if( cell_info == NULL )
	{
		THInterface_FreeSysBuffer(t_cell_info);
		return STATUS_ALLOC_MEMORY_ERROR;
	}
	

	ZeroMemory(cell_info, sizeof(CELL_DETAIL_INFO_EX) * t_cell_info->cCellCounter + sizeof(CELL_FULL_INFO_EX));
	for( j = 0 ; j < t_cell_info->cCellCounter ; j ++ )
	{
		if( pListGroupView->VaildCellType(t_cell_info->cInfo[j].cType) == FALSE )
			continue;
		memcpy(&cell_info->cInfo[cell_info->cCellCounter], &t_cell_info->cInfo[j], sizeof(CELL_DETAIL_INFO)-8);
		cell_info->cInfo[cell_info->cCellCounter].nBlongIndex = nGrapIndex;
		cell_info->cCellCounter++;
	}

	THInterface_FreeSysBuffer(t_cell_info);

	if( bBuildMap == FALSE )
		return STATUS_SUCCESS;
	
load_jpg_loc:	
	
	strcpy(GrapPath, pImageFolder);
	PathAppendA(GrapPath, FileName);

#ifdef UNICODE
	TCHAR tGrapPath[1024];
	MultiByteToWideChar(CP_ACP, 0, GrapPath, -1, tGrapPath, 1024);
	if (jpg_image.Load(tGrapPath, CXIMAGE_FORMAT_JPG) == FALSE)
		return STATUS_LOAD_FILE_FAIL;
#else
	if( jpg_image.Load(GrapPath, CXIMAGE_FORMAT_JPG) == FALSE )
		return STATUS_LOAD_FILE_FAIL;
#endif

	if( pGPX )
	{
		pGPX->cy = jpg_image.GetHeight();
		pGPX->cx = jpg_image.GetWidth();
	}

	if( bOnlyGetSize )
		return status;

	hDC       = GetDC(NULL);
	hRecBitmap = jpg_image.MakeBitmap(hDC);
	if( hRecBitmap == NULL )
	{
		ReleaseDC(NULL, hDC);
		return STATUS_ALLOC_MEMORY_ERROR;
	}



	hMemDC    = CreateCompatibleDC(hDC);
	hMemDC1   = CreateCompatibleDC(hDC);
	hOldBitmap1 = (HBITMAP)SelectObject(hMemDC1, hRecBitmap);

	for( j = 0 ; j < cell_info->cCellCounter ; j ++ )
	{
	
		bSkipNormalScal = FALSE;
		//补偿算法
		if( cell_info->cInfo[j].cType == CELL_EP_TYPE )
			bSkipNormalScal = ScalSpecialCellRect(cell_info->cInfo[j].rc, cell_info->cInfo[j].MinRC);


		if( bSkipNormalScal == FALSE )
		{
			CopyRect(&cell_info->cInfo[j].MinRC, &cell_info->cInfo[j].rc);
			ScalCellRect(cell_info->cInfo[j].MinRC);
		}


		cell_info->cInfo[j].hBitmap = CreateCompatibleBitmap(hDC, REC_GRAP_CX, REC_GRAP_CY);
		if( cell_info->cInfo[j].hBitmap == NULL )
			continue;

		hOldBitmap = (HBITMAP)SelectObject(hMemDC, cell_info->cInfo[j].hBitmap);

		DrawBackGround(hMemDC);

		cx = RECT_WIDTH(cell_info->cInfo[j].MinRC);
		cy = RECT_HEIGHT(cell_info->cInfo[j].MinRC);
		sx = (REC_GRAP_CX - cx)>> 1;
		sy = (REC_GRAP_CY - cy)>> 1;

	/*	if( bSkipNormalScal == FALSE )
			BitBlt(hMemDC, sx, sy, cx, cy, hMemDC1, cell_info->cInfo[j].rc.left, cell_info->cInfo[j].rc.top, SRCCOPY);
		else
			BitBlt(hMemDC, sx, sy, cx, cy, hMemDC1, cell_info->cInfo[j].MinRC.left, cell_info->cInfo[j].MinRC.top, SRCCOPY);20181210屏蔽20x细胞截图显示*/

		SetStretchBltMode(hMemDC,HALFTONE);
		::SetBrushOrgEx(hMemDC,0,0,NULL);
		StretchBlt(hMemDC,sx, sy, cx, cy, hMemDC1, cell_info->cInfo[j].rc.left, cell_info->cInfo[j].rc.top,cell_info->cInfo[j].rc.right-cell_info->cInfo[j].rc.left,cell_info->cInfo[j].rc.bottom-cell_info->cInfo[j].rc.top,SRCCOPY);//20181210 40x细胞图像修改

		cell_info->cInfo[j].MinRC.right -= cell_info->cInfo[j].MinRC.left;
		cell_info->cInfo[j].MinRC.bottom -= cell_info->cInfo[j].MinRC.top;
		cell_info->cInfo[j].MinRC.left = cell_info->cInfo[j].MinRC.top = 0;
		SelectObject(hMemDC, hOldBitmap);
	}

	
	if( hRecBitmap )
	{
		SelectObject(hMemDC1, hOldBitmap1);
		DeleteObject(hRecBitmap);
	}

	DeleteDC(hMemDC1);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hDC);
	return STATUS_SUCCESS;
}




void   CRecTask::FreeCellFullInfo(PCELL_FULL_INFO_EX cell_info)
{
	for( ULONG j = 0 ; j < cell_info->cCellCounter ; j ++ )
	{
		if( cell_info->cInfo[j].hBitmap )
			DeleteObject(cell_info->cInfo[j].hBitmap);
	}

	free(cell_info);
}


KSTATUS CRecTask::ModifyRecCellInfo(PCHAR  pRelativePath, REC_GRAP_RECORD &rgr, CELL_DETAIL_INFO_EX &cell_info_ex)
{
	CELL_DETAIL_INFO     cell_info = {0};

	if( cell_info_ex.nBlongIndex != rgr.rIndex )
		return STATUS_INVALID_PARAMETERS;

	cell_info.cType = cell_info_ex.cType;
	memcpy(&cell_info.identify,&cell_info_ex.identify, sizeof(CELL_IDENTIFY));
	CopyRect(&cell_info.rc, &cell_info_ex.rc);

	return THInterface_ModifyRecCellInfo(pRelativePath, rgr.rIndex, rgr.rGrapSize, &cell_info);
}

KSTATUS  CRecTask::AddRecCellInfo(PCHAR pRelativePath, REC_GRAP_RECORD &rgr,CELL_DETAIL_INFO_EX &cell_info_ex)
{
	CELL_DETAIL_INFO     t_cell_info = {0};
	
	
	t_cell_info.cType = cell_info_ex.cType;
	CopyRect(&t_cell_info.rc, &cell_info_ex.rc);

	return THInterface_AddRecCellInfo(pRelativePath, rgr.rIndex, rgr.rGrapSize, &t_cell_info);
}

KSTATUS  CRecTask::CreateAssociateDC(PCHAR pJpgPath, ASSOCIATE_DC &AssoicateDC)
{
	HDC        hDC;
	BITMAP     bm;      //位图属性结构
	CxImage    JpgImage;
	
	ZeroMemory(&AssoicateDC,sizeof(AssoicateDC));
	
	if( pJpgPath == NULL || PathFileExistsA(pJpgPath) == FALSE  )
		return STATUS_INVALID_PARAMETERS;

#ifdef UNICODE
	TCHAR tGrapPath[1024];
	MultiByteToWideChar(CP_ACP, 0, pJpgPath, -1, tGrapPath, 1024);
	if (JpgImage.Load(tGrapPath, CXIMAGE_FORMAT_JPG) == FALSE)
		return STATUS_LOAD_FILE_FAIL;
#else
	if( JpgImage.Load(pJpgPath, CXIMAGE_FORMAT_JPG) == FALSE )
		return STATUS_LOAD_FILE_FAIL;
#endif

	hDC = GetDC(NULL);
	AssoicateDC.hBitmap = JpgImage.MakeBitmap(hDC);
	if( AssoicateDC.hBitmap == NULL )
	{
		ReleaseDC(NULL, hDC);
		return STATUS_ALLOC_MEMORY_ERROR;
	}

	GetObject(AssoicateDC.hBitmap, sizeof(bm), &bm);
	AssoicateDC.hMemDC     = CreateCompatibleDC(hDC);
	AssoicateDC.hOldBitmap = (HBITMAP)SelectObject(AssoicateDC.hMemDC, AssoicateDC.hBitmap);
	ReleaseDC(NULL, hDC);
	AssoicateDC.rc.right  = bm.bmWidth;
	AssoicateDC.rc.bottom = bm.bmHeight;
	
	return STATUS_SUCCESS;

}


void  CRecTask::DestoryAssoicateDC(ASSOCIATE_DC &AssoicateDC)
{
	SelectObject(AssoicateDC.hMemDC, AssoicateDC.hOldBitmap);
	DeleteObject(AssoicateDC.hBitmap);
	DeleteDC(AssoicateDC.hMemDC);
}

void  CRecTask::DrawPickupRecDC(HWND  hWnd, ASSOCIATE_DC &AssoicateDC, RECT *rc)
{
	HDC   hDC;

	if( hWnd == NULL || IsWindow(hWnd) == FALSE || AssoicateDC.hMemDC == NULL )
		return;

	hDC = GetDC(hWnd);
	if( rc )
		BitBlt(hDC, 
		rc->left,
		rc->top,
		RECT_WIDTH((*rc)),
		RECT_HEIGHT((*rc)),
		AssoicateDC.hMemDC,
		0,
		0,
		SRCCOPY);
	else
		BitBlt(hDC, 
		AssoicateDC.rc.left,
		AssoicateDC.rc.top,
		RECT_WIDTH(AssoicateDC.rc),
		RECT_HEIGHT(AssoicateDC.rc),
		AssoicateDC.hMemDC,
		0,
		0, 
		SRCCOPY);




	ReleaseDC(hWnd, hDC);

}

KSTATUS  CRecTask::GetPickupRecCellGrap(ASSOCIATE_DC &AssoicateDC, HBITMAP &hCellBitmap, RECT &CellRect)
{
	int       cx,cy, sx,sy;
	HDC       hMemDC;
	RECT      MinRC;
	HBITMAP   hOldBitmap;


	hMemDC      = CreateCompatibleDC(AssoicateDC.hMemDC);
	hCellBitmap = CreateCompatibleBitmap(AssoicateDC.hMemDC, REC_GRAP_CX, REC_GRAP_CY);
	hOldBitmap  = (HBITMAP)SelectObject(hMemDC, hCellBitmap);
	CopyRect(&MinRC, &CellRect);
	ScalCellRect(MinRC);

	cx = RECT_WIDTH(MinRC);
	cy = RECT_HEIGHT(MinRC);
	sx = (REC_GRAP_CX - cx)>> 1;
	sy = (REC_GRAP_CY - cy)>> 1;

	DrawBackGround(hMemDC);
	BitBlt(hMemDC, sx, sy, cx, cy, AssoicateDC.hMemDC, CellRect.left, CellRect.top, SRCCOPY);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);

	return STATUS_SUCCESS;
}