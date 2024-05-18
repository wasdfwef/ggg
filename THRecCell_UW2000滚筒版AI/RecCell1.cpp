#include "StdAfx.h"
#include "RecCell1.h"
#include  <SHLWAPI.H>
#include "DbgMsg.h"
#include <stdio.h>
#include "RecHelper.h"
#include <stdlib.h>
#include "interal.h"


CRecCell1::CRecCell1()
{
	LoadCellImage   = NULL;
	CellRecognising = NULL;
	SaveRecReslut = NULL;
}

CRecCell1::~CRecCell1()
{
	if( hMod )
		FreeLibrary(hMod);

}

KSTATUS  CRecCell1::StartRecCell(PREC_CELL_EXTENSION pExtension)
{
	CHAR  path[MAX_PATH];

	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAppend(path, RECOGNISE_DLL_1);

	hMod = LoadLibrary(path);
	if( hMod == NULL )
	{
		DBG_MSG("CRecCell1::StartRecCell Load Fail...\n");
		return STATUS_MODULE_LOAD_FAIL;
	}

	LoadCellImage   = (TC1_LOAD_CELL_IMAGE)GetProcAddress(hMod, "Load");
	CellRecognising = (TC1_RECOGNISING)GetProcAddress(hMod, "Recognise");
	SaveRecReslut   = (TC1_SAVE_REC_RESLUT)GetProcAddress(hMod, "Save");

	if( LoadCellImage == NULL || CellRecognising == NULL || SaveRecReslut == NULL )
		return STATUS_NOT_FOUND_VALUE;

//	pExtension->InterFace = this;
//	pExtension->rec_type  = REC_CELL_TYPE1;

	return STATUS_SUCCESS;
}


KSTATUS  CRecCell1::BuildRecReslut(PREC_GRAP_FULL_RECORD   rec_record)
{
	char      IniPath[MAX_PATH], SessionName[40];
	WCHAR     IniPathW[MAX_PATH] = {0}, SessionNameW[40] = {0};
	ULONG     nRecCount;
	KSTATUS   status;

	status = BuildRecPre(rec_record, SessionName, IniPath);
	if( status != STATUS_SUCCESS )
		return status;

	try
	{
		EntryCS();

		DBG_MSG("BuildRecReslut->rec_record->GrapPath = %s\n", rec_record->GrapPath);
		if( LoadCellImage(rec_record->GrapPath) == FALSE )
		{
			LeaveCS();
			return STATUS_LOAD_FILE_FAIL;
		}

		nRecCount = CellRecognising();
		
		if( nRecCount )
		{
			PathAppend(IniPath, RESLUT_INI);

			MultiByteToWideChar(CP_ACP, 0, IniPath, strlen(IniPath)+1,IniPathW, strlen(IniPath)+1);
			MultiByteToWideChar(CP_ACP, 0, SessionName, strlen(SessionName)+1,SessionNameW, strlen(SessionName)+1);
			SaveRecReslut(IniPathW, SessionNameW);
			TransStandFormat(rec_record);
		}

		LeaveCS();
		return STATUS_SUCCESS;
	}
	catch(...)
	{
		DBG_MSG("CRecCell1::BuildRecReslut->Exception...\n");
		LeaveCS();
		return STATUS_EXCEPTION_ERROR;
	}
	
}


KSTATUS  CRecCell1::TransStandFormat(PREC_GRAP_FULL_RECORD   rec_record)
{
	KSTATUS                 status;
	PCELL_FULL_INFO         pCellFullInfo = NULL;
	CHANGE_MODIFY_INFO      modify_cell = {0};
	REC_GRAP_FULL_RECORD    rc;

	memcpy(&rc, rec_record, sizeof(rc));

	try
	{
		strcpy(rc.ReslutRelativePath, rc.GrapPath);
		PathRemoveFileSpec(rc.ReslutRelativePath);
		status = QueryCellInfo(&rc, &pCellFullInfo);
		if( status != STATUS_SUCCESS )
			return status;
		
		memcpy(&modify_cell.rc_info, &rc, sizeof(modify_cell.rc_info));


		for( USHORT j = 0 ; j < pCellFullInfo->cCellCounter ; j ++ )
		{
			if( pCellFullInfo->cInfo[j].cType > 100 )
			{
				if( pCellFullInfo->cInfo[j].cType == 601 )
				{
					pCellFullInfo->cInfo[j].cType = CELL_MUCS_TYPE;
					goto md_loc;
				}
				
				else if( pCellFullInfo->cInfo[j].cType == 602 )
				{
					pCellFullInfo->cInfo[j].cType = CELL_BACT_TYPE;
					goto md_loc;
				}
				else if( pCellFullInfo->cInfo[j].cType == 603 )
				{
					pCellFullInfo->cInfo[j].cType = CELL_BYST_TYPE;
					goto md_loc;
				}
				
				pCellFullInfo->cInfo[j].cType /= 100;
			}
			
		
			switch(pCellFullInfo->cInfo[j].cType)
			{
			case 1:
				{
					pCellFullInfo->cInfo[j].cType = CELL_RED_TYPE;
					break;
				}
			case 2: //wbc
				{
					pCellFullInfo->cInfo[j].cType = CELL_RED_TYPE + 2;
					break;
				}
			case 3:  //ep
				{
					pCellFullInfo->cInfo[j].cType = CELL_RED_TYPE + 4;
					break;
				}
			case 4: //cast
				{
					pCellFullInfo->cInfo[j].cType = CELL_RED_TYPE + 5;
					break;
				}
			case 5: //cry
				{
					pCellFullInfo->cInfo[j].cType = CELL_RED_TYPE + 10;
					break;
				}
			default:
				{
					pCellFullInfo->cInfo[j].cType = CELL_OTHER_TYPE;
					break;
				}
			}
md_loc:
			memcpy(&modify_cell.cInfo, &pCellFullInfo->cInfo[j], sizeof(modify_cell.cInfo));
			status = ModifyCellInfo(&modify_cell, j);
			
		}

		if( pCellFullInfo )
			FMMC_FreeBuffer(pCellFullInfo);

		return status;
	}

	catch(...)
	{
		DBG_MSG("CRecCell1::TransStandFormat Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}


}

