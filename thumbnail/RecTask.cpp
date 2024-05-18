#include "StdAfx.h"
#include "RecTask.h"

#include <malloc.h>
#include "..\..\..\inc\CxImage\ximage.h"
#include "DbgMsg.h"
 
#pragma comment(lib, "shlwapi.lib")

#pragma comment(lib, "..\\..\\..\\lib\\Thinterface.lib")

#pragma comment(lib, "..\\..\\..\\lib\\tiff.lib")
#pragma comment(lib, "..\\..\\..\\lib\\mng.lib")
#pragma comment(lib, "..\\..\\..\\lib\\jasper.lib")
#pragma comment(lib, "..\\..\\..\\lib\\libpsd.lib")
#pragma comment(lib, "..\\..\\..\\lib\\libdcr.lib")
#pragma comment(lib, "..\\..\\..\\lib\\zlib.lib")
#pragma comment(lib, "..\\..\\..\\lib\\png.lib")
#pragma comment(lib, "..\\..\\..\\lib\\Jpeg.lib")
#pragma comment(lib, "..\\..\\..\\lib\\cximage.lib")


//#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")


#define  RECT_WIDTH(x) (x.right - x.left)
#define  RECT_HEIGHT(x) (x.bottom - x.top)

CRecTask::CRecTask()
{
	
}

KSTATUS CRecTask::GetRecGrapTaskInfoWhenFinish( PCHAR pTaskImagePath,PTASK_REC_INFO &task_rec_info)
{
	KSTATUS                status;
		
	if( PathFileExists(pTaskImagePath) == FALSE )
		return STATUS_NOT_FOUND_FILE;	
	status = THInterface_QueryRecGrapInfo(pTaskImagePath, &task_rec_info);
    
	DBG_MSG("status = %x,pTaskImagePath = %s\n",status,pTaskImagePath);



	return status;
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



KSTATUS CRecTask::GetPickupRecCellGrap(PCHAR  pImageFolder, USHORT  nGrapIndex, UCHAR  nGrapSize, PCELL_FULL_INFO_EX &cell_info,ULONG  m_CellTypeCounter,PULONG  m_CellTypeArray,BOOL  bBuildMap)
{
	int                  cx,cy, sx,sy;
	HDC                  hDC, hMemDC, hMemDC1;
	BOOL                 bSkipNormalScal;
	char                 GrapPath[MAX_PATH], FileName[MAX_PATH];
	ULONG                j,mask;
	KSTATUS              status;
	HBITMAP              hRecBitmap = NULL, hOldBitmap, hOldBitmap1;
	CxImage              jpg_image;
	PCELL_FULL_INFO      t_cell_info;


	/*status = THInterface_GetRecFileName(nGrapIndex, nGrapSize, FileName);*/
	mask = MAKE_REC_FILE_NAME(nGrapIndex, nGrapSize);

	status = THInterface_QueryDevCaps(T_DEV_REC_TYPE, RecGrapFileName, (PCHAR)&mask, sizeof(ULONG), FileName, MAX_PATH, 0);
	if( status != STATUS_SUCCESS )
		return status;


	status = THInterface_QueryRecCellInfo(pImageFolder, nGrapIndex, nGrapSize, &t_cell_info);
	
	if( status != STATUS_SUCCESS )
	{
		if( status == STATUS_NOT_FOUND_VALUE )
		{
			cell_info = (PCELL_FULL_INFO_EX)malloc(sizeof(CELL_FULL_INFO_EX));
			if( cell_info == NULL )
				return STATUS_ALLOC_MEMORY_ERROR;

			status = STATUS_SUCCESS;
			ZeroMemory(cell_info, sizeof(CELL_FULL_INFO_EX));

		}
		
		return status;
	}
		

	cell_info = (PCELL_FULL_INFO_EX)malloc(sizeof(CELL_DETAIL_INFO_EX) * t_cell_info->cCellCounter + sizeof(CELL_FULL_INFO_EX));
	if( cell_info == NULL )
	{
		THInterface_FreeSysBuffer(t_cell_info);
		return STATUS_ALLOC_MEMORY_ERROR;
	}
	

	ZeroMemory(cell_info, sizeof(CELL_DETAIL_INFO_EX) * t_cell_info->cCellCounter + sizeof(CELL_FULL_INFO_EX));
	//cell_info->cCellCounter = t_cell_info->cCellCounter;

	for( j = 0 ; j < t_cell_info->cCellCounter ; j ++ )
	{
		if( VaildCellType(m_CellTypeArray, m_CellTypeCounter, t_cell_info->cInfo[j].cType) == FALSE )
			continue;
		
		memcpy(&cell_info->cInfo[cell_info->cCellCounter], &t_cell_info->cInfo[j], sizeof(CELL_DETAIL_INFO)-8);
		cell_info->cInfo[cell_info->cCellCounter].nBlongIndex = nGrapIndex;
		cell_info->cCellCounter ++;
		
	}

	THInterface_FreeSysBuffer(t_cell_info);

	if( bBuildMap == FALSE )
		return STATUS_SUCCESS;
	
	hDC       = GetDC(NULL);
	
	strcpy(GrapPath, pImageFolder);
	PathAppend(GrapPath, FileName);
	if( jpg_image.Load(GrapPath, CXIMAGE_FORMAT_JPG) == FALSE )
	{
		ReleaseDC(NULL, hDC);
		return STATUS_LOAD_FILE_FAIL;
	}

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

		if( bSkipNormalScal == FALSE )
			BitBlt(hMemDC, sx, sy, cx, cy, hMemDC1, cell_info->cInfo[j].rc.left, cell_info->cInfo[j].rc.top, SRCCOPY);
		else
			BitBlt(hMemDC, sx, sy, cx, cy, hMemDC1, cell_info->cInfo[j].MinRC.left, cell_info->cInfo[j].MinRC.top, SRCCOPY);

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
	
	if( pJpgPath == NULL || PathFileExists(pJpgPath) == FALSE  )
		return STATUS_INVALID_PARAMETERS;

	if( JpgImage.Load(pJpgPath, CXIMAGE_FORMAT_JPG) == FALSE )
		return STATUS_LOAD_FILE_FAIL;

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
	{
		
		return;
	}
		
	DBG_MSG("right %d,bottom %d",rc->right,rc->bottom);

	hDC = GetDC(hWnd);
	SetStretchBltMode(hDC,HALFTONE);
	if( rc )
		StretchBlt(hDC, 
		rc->left,
		rc->top,
		RECT_WIDTH((*rc)),
		RECT_HEIGHT((*rc)),
		AssoicateDC.hMemDC,
		0,
		0,
		AssoicateDC.rc.right,
		AssoicateDC.rc.bottom,
		SRCCOPY);
	else
		StretchBlt(hDC, 
		AssoicateDC.rc.left,
		AssoicateDC.rc.top,
		RECT_WIDTH(AssoicateDC.rc),
		RECT_HEIGHT(AssoicateDC.rc),
		AssoicateDC.hMemDC,
		0,
		0,
		AssoicateDC.rc.right,
		AssoicateDC.rc.bottom,
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

BOOL CRecTask::VaildCellType( PULONG pActiveCell, ULONG nActiveCounter, ULONG nTestCellType )
{
	for( ULONG i  = 0 ; i < nActiveCounter ; i ++ )
	{

		if( pActiveCell[i] == nTestCellType )
			return TRUE;
	}

	return FALSE;

}
