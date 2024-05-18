#ifndef    _RECTASK_INCLUDE
#define    _RECTASK_INCLUDE

#include "..\..\..\inc\THInterface.h"
#include "ListGroupView.h"

#pragma pack(1)


typedef  struct  _CELL_DETAIL_INFO_EX
{
	USHORT         cType;
	RECT           rc;
	CELL_IDENTIFY  identify;
	USHORT         nBlongIndex; //所属图片ID
	HBITMAP        hBitmap;
	RECT           MinRC;

}CELL_DETAIL_INFO_EX, *PCELL_DETAIL_INFO_EX;

typedef  struct  _CELL_FULL_INFO_EX
{
	USHORT                  cCellCounter;
	CELL_DETAIL_INFO_EX     cInfo[1];
	
}CELL_FULL_INFO_EX, *PCELL_FULL_INFO_EX;

//以上结构,将其中保留位重新定义给该类专门使用

typedef struct _REC_TASK_MANAGER
{
	PTASK_REC_INFO       task_rec_info;
//	FULL_TASK_INFO_1     task_db_info;
	PCELL_FULL_INFO_EX   cell_info_ex;

}REC_TASK_MANAGER, *PREC_TASK_MANAGER;

typedef struct _ASSOCIATE_DC
{
	HDC       hMemDC;
	HBITMAP   hBitmap;
	HBITMAP   hOldBitmap;
	RECT      rc;

}ASSOCIATE_DC, *PASSOCIATE_DC;

#pragma pack()

#define  REC_GRAP_CX 48
#define  REC_GRAP_CY 48


class  CRecTask
{
	void ScalCellRect(RECT &rt);
	void DrawBackGround(HDC &hMemDC);
	BOOL ScalSpecialCellRect(RECT&SrcRC, RECT &ScalRC); //补偿算法

public:
	CRecTask();

	KSTATUS   GetRecGrapTaskInfoWhenFinish(PCHAR pTaskImagePath, PTASK_REC_INFO &task_rec_info); //注意:task_rec_info使用完后，需要用THInterface_FreeSysBuffer
	KSTATUS   GetPickupRecCellGrap(PCHAR  pImageFolder, USHORT  nGrapIndex, UCHAR  nGrapSize, PCELL_FULL_INFO_EX &cell_info_ex, CListGroupView *pListGroupView,BOOL  bBuildMap = TRUE, PSIZE pGPX = NULL); //注意:cell_info使用完后,需要调用FreeCellFullInfo进行free
	KSTATUS   GetPickupRecCellGrap(ASSOCIATE_DC &AssoicateDC, HBITMAP &hCellBitmap, RECT &CellRect);
	KSTATUS   CreateAssociateDC(PCHAR pJpgPath,	ASSOCIATE_DC &AssoicateDC);
	
	
	KSTATUS   ModifyRecCellInfo(PCHAR  pRelativePath, REC_GRAP_RECORD &rgr,CELL_DETAIL_INFO_EX &cell_info);
	KSTATUS   AddRecCellInfo(PCHAR pRelativePath, REC_GRAP_RECORD &rgr,CELL_DETAIL_INFO_EX &cell_info);
	void      FreeCellFullInfo(PCELL_FULL_INFO_EX cell_info);


	void      DrawPickupRecDC(HWND  hWnd, ASSOCIATE_DC &AssoicateDC, RECT *rc = NULL);
	void      DestoryAssoicateDC(ASSOCIATE_DC &AssoicateDC);


	
};



#endif