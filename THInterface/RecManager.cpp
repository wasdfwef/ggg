#include "StdAfx.h"
#include "DevManager.h"
#include "RecManager.h"
#include <stdio.h>
#include <shlwapi.h>
#include "DbgMsg.h"
#include <CString>


using namespace std;

KSTATUS	CRecManager::QueryRecCellInfo(PVOID  pCodePath, USHORT             nGrapIndex,UCHAR              nGrapSize,PCELL_FULL_INFO   *pRecCellInfo)
{
	KSTATUS                status;
	REC_GRAP_FULL_RECORD   rgr = {0};

	if( CDevManager::GetOIInfo().s_rec_dev == NULL || pRecCellInfo == NULL  )
		return STATUS_INVALID_PARAMETERS;

// 	int nPos;
// 	CString* strFilePath = (CString*)pCodePath;
// 	CString strFileName;
// 	nPos = ((CString*)pCodePath)->ReverseFind('\\');
// 	if (nPos < 0)
// 		strFileName = *strFilePath; 
// 	else
// 		strFileName = strFilePath->Mid(nPos + 1);

	status = GetRecGrapPath(pCodePath, rgr.ReslutRelativePath);
	if( status != STATUS_SUCCESS )
		return status;

	rgr.g_rc.rIndex = nGrapIndex;
	rgr.g_rc.rGrapSize = nGrapSize;

	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_rec_dev, NULL, PRP_AC_QUERY, REC_CELL_INFO, &rgr, sizeof(rgr), pRecCellInfo, sizeof(PCELL_FULL_INFO), FALSE, FALSE);
}


KSTATUS CRecManager::ModifyRecCellInfo(PVOID  pCodePath,USHORT   nGrapIndex,UCHAR   nGrapSize,PCELL_DETAIL_INFO  pCellDetailInfo)
{
	KSTATUS              status;
	CHANGE_MODIFY_INFO   ModifyInfo = {0};
	
	if( CDevManager::GetOIInfo().s_rec_dev == NULL || pCellDetailInfo == NULL ||  IsBadWritePtr(pCellDetailInfo, sizeof(CELL_DETAIL_INFO)) == TRUE )
		return STATUS_INVALID_PARAMETERS;

	status = GetRecGrapPath(pCodePath, ModifyInfo.rc_info.ReslutRelativePath);
	if( status != STATUS_SUCCESS )
		return status;
	
	ModifyInfo.rc_info.g_rc.rGrapSize = nGrapSize;
	ModifyInfo.rc_info.g_rc.rIndex    = nGrapIndex;
	memcpy(&ModifyInfo.cInfo, pCellDetailInfo, sizeof(CELL_DETAIL_INFO));
	
	status = FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_rec_dev, NULL, PRP_AC_SET, MODIFY_CELL_INFO, &ModifyInfo, sizeof(ModifyInfo), NULL, 0, FALSE, FALSE);
	if( status == STATUS_SUCCESS )
		memcpy(pCellDetailInfo, &ModifyInfo.cInfo, sizeof(CELL_DETAIL_INFO));
	
	return status;
}

BOOL isPATH(PVOID pCodePath)
{
	char str[128] = { 0 };
	strcpy(str, (char*)pCodePath);
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == '\\')
		{
			return TRUE;
		}
	}
	return FALSE;
}

KSTATUS CRecManager::AddRecCellInfo(PVOID  pCodePath,USHORT   nGrapIndex,UCHAR    nGrapSize,PCELL_DETAIL_INFO  pCellDetailInfo)
{
	KSTATUS             status;
	CHANGE_MODIFY_INFO  ModifyInfo = {0};
	
	if( CDevManager::GetOIInfo().s_rec_dev == NULL || pCellDetailInfo == NULL || IsBadReadPtr(pCellDetailInfo, sizeof(CELL_DETAIL_INFO)) == TRUE )
		return STATUS_INVALID_PARAMETERS;

	status = GetRecGrapPath(pCodePath, ModifyInfo.rc_info.ReslutRelativePath);
	if( status != STATUS_SUCCESS )
		return status;
	
	ModifyInfo.rc_info.g_rc.rGrapSize = nGrapSize;
	ModifyInfo.rc_info.g_rc.rIndex    = nGrapIndex;
	memcpy(&ModifyInfo.cInfo, pCellDetailInfo, sizeof(CELL_DETAIL_INFO));
	
	status = FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_rec_dev, NULL, PRP_AC_DEVIO, ADD_REC_GRAP_REQUEST, &ModifyInfo, sizeof(ModifyInfo), NULL, 0, FALSE, FALSE);
	if( status == STATUS_SUCCESS )
		memcpy(&pCellDetailInfo->identify, &ModifyInfo.cInfo.identify, sizeof(CELL_IDENTIFY));
	
	return status;
}


KSTATUS CRecManager::DeleteRecCellInfo(PVOID  pCodePath, PREC_GRAP_RECORD   pDelRecGrapRecord, PCELL_IDENTIFY     pCellIdentify,BOOL               bDelAllRecInfo)
{
	KSTATUS           status;
	DEL_REC_EX_INFO   del_rec = {0};
	
	if( CDevManager::GetOIInfo().s_rec_dev == NULL )
		return STATUS_INVALID_PARAMETERS;

	if( bDelAllRecInfo == FALSE )
	{
		if( pDelRecGrapRecord == NULL || pCellIdentify == NULL )
			return STATUS_INVALID_PARAMETERS;
	}

	if( pCellIdentify && IsBadReadPtr(pCellIdentify, sizeof(CELL_IDENTIFY)) == TRUE )
		return STATUS_INVALID_PARAMETERS;

	if( pDelRecGrapRecord && IsBadReadPtr(pDelRecGrapRecord, sizeof(REC_GRAP_RECORD)) == TRUE )
		return STATUS_INVALID_PARAMETERS;

	status = GetRecGrapPath(pCodePath, del_rec.RelativePath);
	if( status != STATUS_SUCCESS )
		return status;
	
	del_rec.bDelAll = bDelAllRecInfo;
	if( pCellIdentify )
		memcpy(&del_rec.cdi.identify, pCellIdentify, sizeof(CELL_IDENTIFY));

	if( pDelRecGrapRecord )
		memcpy(&del_rec.rgr, pDelRecGrapRecord, sizeof(REC_GRAP_RECORD));
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_rec_dev, NULL, PRP_AC_DEVIO, DEL_REC_EX_REQUEST, &del_rec, sizeof(del_rec), NULL, 0, FALSE, FALSE);
}

KSTATUS CRecManager::QueryRecGrapInfo(PVOID  pCodePath,PTASK_REC_INFO *pRecInfo)
{
	KSTATUS  status;
	REC_GRAP_FULL_RECORD   rgr = {0};
	
	if( CDevManager::GetOIInfo().s_rec_dev == NULL || pRecInfo == NULL )
		return STATUS_INVALID_PARAMETERS;
	
	status = GetRecGrapPath(pCodePath, rgr.ReslutRelativePath);
	if( status != STATUS_SUCCESS )
		return status;

	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_rec_dev, NULL, PRP_AC_QUERY, REC_GRAP_INFO, &rgr, sizeof(rgr), pRecInfo, sizeof(PTASK_REC_INFO), FALSE, FALSE);
}

KSTATUS CRecManager::GetRecGrapPath( PVOID pCodePath, PCHAR pRecGrapPath )
{
	int                    nLen;
	char                   dir[40], TestDir[MAX_PATH];



	if (IsBadReadPtr(pCodePath, 5) == 0 && isPATH(pCodePath) == FALSE)
	{
code_path_loc:
		GetModuleFileName(NULL, pRecGrapPath, MAX_PATH);
		PathRemoveFileSpec(pRecGrapPath);
		PathAppend(pRecGrapPath, "RecGrapReslut");
		sprintf(dir, "%u", atoi((char*)pCodePath));
		PathAppend(pRecGrapPath, dir);
	}
	else
	{
		_try
		{
			strcpy(pRecGrapPath, (PCHAR)pCodePath);
			nLen = strlen(pRecGrapPath);
			if( nLen < 5 )
				goto code_path_loc;

			strcpy(TestDir, pRecGrapPath);
			PathRemoveFileSpec(TestDir);
			if( PathFileExists(TestDir) == FALSE )
			{
				goto code_path_loc;
			}
			
		}
		_except(EXCEPTION_EXECUTE_HANDLER)
		{
			goto code_path_loc;

		}
	}
	
	
	if( PathFileExists(pRecGrapPath) == FALSE )
	{
		return STATUS_INVALID_PARAMETERS;
	}
	
	return STATUS_SUCCESS;

}
