#include "StdAfx.h"
#include "RecMrg.h"
#include "DbgMsg.h"
#include "RecHelper.h"
#include <SHLWAPI.H>
#include <MALLOC.H>


#pragma comment(lib, "shlwapi.lib")

#define  CA_MGR_CFG  "config\\DrvCfg.ini"
#define  DRV_SESSION "RecModulePath"
#define  DRV_COUNTER_KEY  "DrvPathCounter"
#define  ADDITION_SECTION "AdditionInfo"
#define  END_INDEX_KEY    "EndIndex"
#define  GRAP_SIZE_KEY    "GrapSize"


#define  DEV_START(x) (InterlockedExchangeAdd(x, 0) != 0)

const char RecApiName[][30] = { { "RecDrvQueryType" }, { "RecDrvStart" }, { "RecDrvBuildResult" }};//, {"RecDrvPicDistinct"}20190531老版UW2000使用程序少一个函数接口否则自检通不过
#define  API_COUNTER  3

CRecMgr::CRecMgr()
{
	ZeroMemory(&m_rec_info, sizeof(m_rec_info));
	LoadAllCaDrv();
	
}

CRecMgr::~CRecMgr()
{
	for( ULONG j = 0 ; j < m_rec_info.nRecDrvCount ;j  ++ )
		FreeRecDrv(&m_rec_info.pRecDriver[j]);

	if( m_rec_info.pRecDriver )
		free(m_rec_info.pRecDriver);
}

void CRecMgr::LoadAllCaDrv()
{
	int                      nCounter, nVaildCounter = 0;
	char                     base_path[MAX_PATH], cfg_ini_path[MAX_PATH], ModPath[MAX_PATH], KeyName[40], Temp[MAX_PATH];
	FULL_REC_INFO            drv_info = {0};
	
	
	GetModuleFileName(NULL, cfg_ini_path, MAX_PATH);
	PathRemoveFileSpec(cfg_ini_path);
	strcpy(base_path, cfg_ini_path);
	PathAppend(cfg_ini_path, CA_MGR_CFG);
	
 	nCounter = GetPrivateProfileInt(DRV_SESSION, DRV_COUNTER_KEY, 0, cfg_ini_path);
	if( nCounter == 0 )
		return;
	
	drv_info.pRecDriver = (PREC_DRIVER)malloc(sizeof(REC_DRIVER) * nCounter);
	if( drv_info.pRecDriver == NULL )
		return;
	
	ZeroMemory(drv_info.pRecDriver, sizeof(REC_DRIVER) * nCounter);
	
	//加载dll
	for( int j = 0 ; j < nCounter ; j ++ )
	{
		Temp[0] = 0;
		sprintf(KeyName, "%u", j);
		GetPrivateProfileString(DRV_SESSION, KeyName, "",  Temp, MAX_PATH, cfg_ini_path);
		if( Temp[0] == 0 )
			continue;
		
		strcpy(ModPath, base_path);
		PathAppend(ModPath, Temp);
		drv_info.pRecDriver[nVaildCounter].hMod = LoadLibraryEx(ModPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if( drv_info.pRecDriver[nVaildCounter].hMod == NULL )
			continue;

		if( GetAPIInfo(&drv_info.pRecDriver[j]) == FALSE )
		{
			FreeRecDrv(&drv_info.pRecDriver[j]);
			continue;
		}
		
		nVaildCounter++;
	}

	if( nVaildCounter == 0 )
	{
err_loc:
		for(int j = 0 ; j < nCounter ; j ++ )
			FreeRecDrv(&drv_info.pRecDriver[j]);

		if( drv_info.pRecDriver )
			free(drv_info.pRecDriver);

	}
	else if( nVaildCounter == nCounter )
	{
		m_rec_info.nRecDrvCount = nVaildCounter;
		m_rec_info.pRecDriver   = drv_info.pRecDriver;
	}
	else
	{
		m_rec_info.pRecDriver = (PREC_DRIVER)malloc(sizeof(REC_DRIVER) * nVaildCounter);
		if( m_rec_info.pRecDriver == NULL )
			goto err_loc;

		m_rec_info.nRecDrvCount = nVaildCounter;	
		nVaildCounter = 0;
		for(int j = 0 ; j < nCounter ; j ++ )
		{
			if( drv_info.pRecDriver[j].hMod == NULL )
				continue;
			
			memcpy(&m_rec_info.pRecDriver[nVaildCounter ++], &drv_info.pRecDriver[j], sizeof(REC_DRIVER));
		}

		free(drv_info.pRecDriver);
	}
}

BOOL CRecMgr::GetAPIInfo( PREC_DRIVER pRecDriver )
{
	PULONG   pAddress;
	
	pAddress = (PULONG)&pRecDriver->nDrvAPI;
	
	for( int n = 0 ; n < API_COUNTER ; n ++ )
	{
		pAddress[n] = (ULONG)GetProcAddress(pRecDriver->hMod, RecApiName[n]);
		if( pAddress[n] == 0 ) 
			return FALSE;
	}
	
	return TRUE;
}


void CRecMgr::FreeRecDrv( PREC_DRIVER pRecDriver )
{
	if( pRecDriver->hMod )
	{
		FreeLibrary(pRecDriver->hMod);
		ZeroMemory(pRecDriver, sizeof(REC_DRIVER));
	}
	
}

KSTATUS CRecMgr::GetRDriverObjectInfo( UCHAR nGlobalRecIndex, PREC_DRIVER *pRecDriver /*= NULL*/ )
{
	if( nGlobalRecIndex >= m_rec_info.nRecDrvCount )
		return STATUS_INVALID_PARAMETERS;
	
	
	if( pRecDriver )
		*pRecDriver = &m_rec_info.pRecDriver[nGlobalRecIndex];
	
	return STATUS_SUCCESS;
}


KSTATUS  
CALLBACK
ModifyCellInfoCB(
PCHANGE_MODIFY_INFO   cModifyInfo,
ULONG                 nModifyCellItem,
CRecHelper           *rec_helper
)
{
	if( cModifyInfo == NULL || rec_helper == NULL )
		return STATUS_INVALID_PARAMETERS;
	
	return rec_helper->ModifyCellInfo(cModifyInfo, nModifyCellItem);
		
}


KSTATUS
CALLBACK  
QueryCellInfoCB(
PREC_GRAP_FULL_RECORD   grap_rc, 
PCELL_FULL_INFO        *pCellInfo,
CRecHelper             *rec_helper
)
{
	if( rec_helper == NULL || grap_rc == NULL || pCellInfo == NULL )
		return STATUS_INVALID_PARAMETERS;

	return rec_helper->QueryCellInfo(grap_rc, pCellInfo);
}

KSTATUS CRecMgr::StartRecDev( UCHAR nGlobalRecIndex, PVOID pContext )
{
	KSTATUS       status;
	PREC_DRIVER   rec_dev;
	
	status = GetRDriverObjectInfo(nGlobalRecIndex, &rec_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&rec_dev->bStart) )
		return STATUS_DEV_START_ALREADY;

	try
	{
	    status = rec_dev->nDrvAPI.DrvRecDevStart((REC_QUERY_CELL_INFO_CB)QueryCellInfoCB, (REC_MODIFY_CELL_INFO_CB)ModifyCellInfoCB, pContext);
		if( status == STATUS_SUCCESS )
			InterlockedExchange(&rec_dev->bStart, 1);

		return status;
	}
	catch (...)
	{
		DBG_MSG("CRecMgr::StartRecDev Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
}

RecDevTypeDes CRecMgr::RecDevQueryType( UCHAR nGlobalRecIndex )
{
	KSTATUS       status;
	PREC_DRIVER   rec_dev;
	
	status = GetRDriverObjectInfo(nGlobalRecIndex, &rec_dev);
	if( status != STATUS_SUCCESS )
		return IvdRecDev;

	try
	{
		return rec_dev->nDrvAPI.DrvRecDevQueryType();
	}
	catch (...)
	{
		DBG_MSG("CRecMgr::RecDevQueryType...Exception!\n");
		return IvdRecDev;
	}
}

KSTATUS  CRecMgr::BuildRecPre(PREC_GRAP_FULL_RECORD   grap_rc, PCHAR SessionName, PCHAR  pIniPath)
{
	char  str[40];

	strcpy(pIniPath, grap_rc->GrapPath);
	PathRemoveFileSpec(pIniPath);
	if ((int)grap_rc->g_rc.rGrapSize == 1 || (int)grap_rc->g_rc.rGrapSize == 0)//20190530判断是聚焦则不修改路径直接返回
	{
		sprintf(SessionName, "%d", grap_rc->g_rc.rIndex);
		return STATUS_SUCCESS;
	}
	else
	{
		sprintf(SessionName, "%.3u_%.4u", grap_rc->g_rc.rGrapSize, grap_rc->g_rc.rIndex);
	}
	
	/*if( grap_rc->GrapPath[0] == 0 || grap_rc->ReslutRelativePath[0] == 0 )
		return STATUS_INVALID_PARAMETERS;
	
	if( PathFileExists(grap_rc->GrapPath) == FALSE || PathFileExists(grap_rc->ReslutRelativePath) == FALSE )
		return STATUS_NOT_FOUND_FILE;
	
	strcpy(pIniPath, grap_rc->ReslutRelativePath);
	sprintf(str, "%u",grap_rc->rTaskID);
	PathAppend(pIniPath, str);
	//建立任务目录
	if( PathFileExists(pIniPath) == FALSE )
	{
		if( CreateDirectory(pIniPath, NULL) == FALSE )
			return STATUS_MKDIR_FAIL;
	}
	
	sprintf(str, "%.3u_%.4u.jpg", grap_rc->g_rc.rGrapSize, grap_rc->g_rc.rIndex);
	sprintf(SessionName, "%.3u_%.4u", grap_rc->g_rc.rGrapSize, grap_rc->g_rc.rIndex);
	PathAppend(pIniPath, str);
	
	if( MoveFileEx(grap_rc->GrapPath, pIniPath, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED) == FALSE )
		return STATUS_RENAME_FAIL;
	
	//更新路径
	strcpy(grap_rc->GrapPath, pIniPath);
	PathRemoveFileSpec(pIniPath);*/
	return STATUS_SUCCESS;
}

KSTATUS CRecMgr::RecDevBuildResult( PREC_GRAP_FULL_RECORD rec_reslut )
{
	char          IniPath[MAX_PATH], SessionName[40], temp[40];
	KSTATUS       status;
	PREC_DRIVER   rec_dev;
	
	status = GetRDriverObjectInfo(rec_reslut->nRecDevIndex, &rec_dev);
	if( status != STATUS_SUCCESS )
		return status;

	if( !DEV_START(&rec_dev->bStart) )
		return STATUS_DEV_NOT_START;

	status = BuildRecPre(rec_reslut, SessionName, IniPath);
	if( status != STATUS_SUCCESS )
		return status;
	
	try
	{
		PathAppend(IniPath, RESLUT_INI);
		int ei = GetPrivateProfileInt(ADDITION_SECTION, END_INDEX_KEY, 0xFFFF, IniPath);
		if (ei == 0xFFFF)
		{
			sprintf_s(temp, "%u", rec_reslut->g_rc.rIndex);
			WritePrivateProfileString(ADDITION_SECTION, END_INDEX_KEY, temp, IniPath);
		}
		else
		{
			if (ei < rec_reslut->g_rc.rIndex)
			{
				sprintf_s(temp, "%u", rec_reslut->g_rc.rIndex);
				WritePrivateProfileString(ADDITION_SECTION, END_INDEX_KEY, temp, IniPath);
			}
		}
		sprintf_s(temp, "%u", rec_reslut->g_rc.rGrapSize);
		WritePrivateProfileString(ADDITION_SECTION, GRAP_SIZE_KEY,temp, IniPath);
		//PathRemoveFileSpec(IniPath);
		return rec_dev->nDrvAPI.DrvRecBuildResult(rec_reslut, SessionName, IniPath);
		
	}
	catch (...)
	{
		DBG_MSG("CRecMgr::RecDevQueryType...Exception!\n");
		return STATUS_EXCEPTION_ERROR;
	}
	
}

ULONG CRecMgr::GetRecCount()
{
	return m_rec_info.nRecDrvCount;
}

ULONG CRecMgr::GetStartRecCount()
{
	ULONG  n = 0;

	for( ULONG j = 0 ; j < m_rec_info.nRecDrvCount ; j ++ )
	{
		if( DEV_START(&m_rec_info.pRecDriver[j].bStart) )
			n++;
	}

	return n;
}


KSTATUS CRecMgr::QueryPicDis( UCHAR nRecDevIndex, PCHAR pPicPath, float* pDist )
{
	char          IniPath[MAX_PATH], SessionName[40], temp[40];
	KSTATUS       status;
	PREC_DRIVER   rec_dev;
	
	status = GetRDriverObjectInfo(nRecDevIndex, &rec_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( !DEV_START(&rec_dev->bStart) )
		return STATUS_DEV_NOT_START;

	try
	{
		return rec_dev->nDrvAPI.DrvRecPicDistinct(pPicPath, pDist);

	}
	catch (...)
	{
		DBG_MSG("CRecMgr::QueryPicDis...Exception!\n");
		return STATUS_EXCEPTION_ERROR;
	}
	
}
