   #include "StdAfx.h"
#include "CameraMgr.h"
#include "..\..\CommonAPI\inc\THInterface.h"
#include "..\..\CommonAPI\inc\DrvAPIDef.h"
#include "DbgMsg.h"
#include <SHLWAPI.H>
#include <malloc.h>
#include <stdio.h>

#define  CA_MGR_CFG  "config\\DrvCfg.ini"
#define  DRV_SESSION "CaModulePath"
#define  DRV_COUNTER_KEY  "DrvPathCounter"

#define  DEV_START(x) (InterlockedExchangeAdd(x, 0) != 0)


const char CaApiName[][30] = {{"CaDrvQueryDevCounter"},{"CaDrvStart"}, {"CaDrvQueryDrvSN"}, {"CaDrvSetPrview"}, {"CaDrvQueryGainRange"}, {"CaDrvQueryETRange"}, 
{"CaDrvQueryConfig"}, {"CaDrvSetConfig"},{"CaDrvReadRawBuffer"},{"CaDrvCatchGrap"}, {"CaDrvSetOutputCB"},{"CaDrvCloseAll"},{"CaDrvSetWBalance"}};
#define  API_COUNTER  13

CCameraMgr::CCameraMgr()
{
	ZeroMemory(&m_CaInfo, sizeof(m_CaInfo));
	LoadAllCaDrv();
}

CCameraMgr::~CCameraMgr()
{
	if( m_CaInfo.nDevCounter && m_CaInfo.pCaDevice )
		free(m_CaInfo.pCaDevice);

	if( m_CaInfo.nDrvCounter && m_CaInfo.pCaDriver )
		FreeCaDriverInfo(m_CaInfo.pCaDriver, m_CaInfo.nDrvCounter);

}

void CCameraMgr::LoadAllCaDrv()
{
	int                      nCounter, nVaildCounter = 0;
	char                     base_path[MAX_PATH], cfg_ini_path[MAX_PATH], ModPath[MAX_PATH], KeyName[40], Temp[MAX_PATH];
	USHORT                   nDevCounter;
	KSTATUS                  status;
	FULL_CA_INFO             drv_info = {0};


	GetModuleFileName(NULL, cfg_ini_path, MAX_PATH);
	PathRemoveFileSpec(cfg_ini_path);
	strcpy(base_path, cfg_ini_path);
	PathAppend(cfg_ini_path, CA_MGR_CFG);

	nCounter = GetPrivateProfileInt(DRV_SESSION, DRV_COUNTER_KEY, 0, cfg_ini_path);
	if( nCounter == 0 )
		return;

	drv_info.pCaDriver = (PCA_DRIVER)malloc(sizeof(CA_DRIVER) * nCounter);
	if( drv_info.pCaDriver == NULL )
		return;

	ZeroMemory(drv_info.pCaDriver, sizeof(CA_DRIVER) * nCounter);

	int j = 0;
	//加载dll
	for( j = 0 ; j < nCounter ; j ++ )
	{
		Temp[0] = 0;
		sprintf(KeyName, "%u", j);
		GetPrivateProfileString(DRV_SESSION, KeyName, "",  Temp, MAX_PATH, cfg_ini_path);
		if( Temp[0] == 0 )
			continue;

		strcpy(ModPath, base_path);
		PathAppend(ModPath, Temp);
		drv_info.pCaDriver[nVaildCounter].hMod = LoadLibraryEx(ModPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if( drv_info.pCaDriver[nVaildCounter].hMod == NULL )
			continue;

		nVaildCounter++;
	}

    nCounter      = nVaildCounter;
	nVaildCounter = 0;

	for( j = 0 ; j < nCounter ; j ++ )   //加载api
	{
		if( GetAPIInfo(&drv_info.pCaDriver[j]) == FALSE )
		{
ivd_dll:
		    FreeCaDrv(&drv_info.pCaDriver[j]);
			continue;
		}
		try
		{
			status = drv_info.pCaDriver[j].nDrvAPI.DrvQueryDevCounter(&nDevCounter);
			if( status != STATUS_SUCCESS || nDevCounter == 0 )
				goto ivd_dll;

			drv_info.nDevCounter               += nDevCounter;
			drv_info.pCaDriver[j].nCameraNumber = nDevCounter;
			nVaildCounter ++;
			nDevCounter = 0;

		}
		catch (...)
		{
			DBG_MSG("CCameraMgr::LoadAllCaDrv->DrvQueryDevCounter Exception...\n");
			goto ivd_dll;
		}
	}

	if( nVaildCounter == 0 )  //没有摄像头
		goto fail_loc;
	
	m_CaInfo.pCaDriver   = (PCA_DRIVER)malloc(sizeof(CA_DRIVER) * nVaildCounter);
	if( m_CaInfo.pCaDriver == NULL )
	{
fail_loc:
		drv_info.nDevCounter = 0;
		FreeCaDriverInfo(drv_info.pCaDriver, nCounter);
		return;
	}

	m_CaInfo.nDrvCounter = nVaildCounter;
	nVaildCounter = 0;
	for( j = 0 ; j < nCounter ; j ++ )
	{
		if( drv_info.pCaDriver[j].hMod == NULL )
			continue;

		memcpy(&m_CaInfo.pCaDriver[nVaildCounter ++], &drv_info.pCaDriver[j], sizeof(CA_DRIVER));
	}

	free(drv_info.pCaDriver);

	m_CaInfo.nDevCounter = drv_info.nDevCounter;
	QueryCaDevInfo();

}

BOOL CCameraMgr::GetAPIInfo( PCA_DRIVER pCaDriver )
{
	PULONG   pAddress;

	pAddress = (PULONG)&pCaDriver->nDrvAPI;

	for( int n = 0 ; n < API_COUNTER ; n ++ )
	{
		pAddress[n] = (ULONG)GetProcAddress(pCaDriver->hMod, CaApiName[n]);
		if( pAddress[n] == 0 ) 
			return FALSE;
	}

	return TRUE;
}


void CCameraMgr::FreeCaDrv( PCA_DRIVER pCaDriver )
{
	if( pCaDriver->hMod )
	{
		FreeLibrary(pCaDriver->hMod);
		ZeroMemory(pCaDriver, sizeof(CA_DRIVER));
	}
	
}

void CCameraMgr::FreeCaDriverInfo(PCA_DRIVER pCaDriver, USHORT nDrvCount)
{
	for( USHORT j = 0 ; j < nDrvCount ; j ++ )
		FreeCaDrv(&pCaDriver[j]);
	
}

void CCameraMgr::QueryCaDevInfo()
{
	USHORT     j, k, nDevCounter = 0;

	if( m_CaInfo.nDevCounter == 0 || m_CaInfo.nDrvCounter == 0 )
		return;

	m_CaInfo.pCaDevice = (PCA_DEVICE)malloc(sizeof(CA_DEVICE) * m_CaInfo.nDevCounter);
	if( m_CaInfo.pCaDevice == NULL )
	{
fail_loc1:
		FreeCaDriverInfo(m_CaInfo.pCaDriver, m_CaInfo.nDrvCounter);
		ZeroMemory(&m_CaInfo, sizeof(m_CaInfo));
		return;
	}

	ZeroMemory(m_CaInfo.pCaDevice, sizeof(CA_DEVICE) * m_CaInfo.nDevCounter);

	for( j = 0 ; j < m_CaInfo.nDrvCounter ; j ++ )
	{
		for( k = 0 ; k < m_CaInfo.pCaDriver[j].nCameraNumber ; k ++ )
		{

			m_CaInfo.pCaDevice[nDevCounter].nRelativeCaIndex = k;
			m_CaInfo.pCaDevice[nDevCounter].pRelativeDriver  = &m_CaInfo.pCaDriver[j];
			nDevCounter++;
		}
	}

	if( nDevCounter == 0 )
	{
		free(m_CaInfo.pCaDevice);
		goto fail_loc1;
	}

	m_CaInfo.nDevCounter = nDevCounter;

}


KSTATUS CCameraMgr::StartCaDev( UCHAR nGlobalCaIndex )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;

	

	status = GetRDriverObjectInfo(nGlobalCaIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;

	if( DEV_START(&ca_dev->bStart) )
		return STATUS_DEV_START_ALREADY;

	try
	{
		status = ca_dev->pRelativeDriver->nDrvAPI.DrvStart(nRelativeCaIndex);
		if( status == STATUS_SUCCESS )
		{
			InterlockedExchange(&ca_dev->bStart, 1);
		}

		DBG_MSG("CCameraMgr::StartCaDev = %u,Reslut = %x\n", nGlobalCaIndex, status);

	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::StartCaDev->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}

	return status;

}

KSTATUS CCameraMgr::GetRDriverObjectInfo( UCHAR nGlobalCaIndex, PUSHORT pRelativeCaIndex /*= NULL*/, PCA_DEVICE *pCaDevice /*= NULL*/ )
{
	if( nGlobalCaIndex >= m_CaInfo.nDevCounter )
		return STATUS_INVALID_PARAMETERS;

	if( pRelativeCaIndex )
		*pRelativeCaIndex = m_CaInfo.pCaDevice[nGlobalCaIndex].nRelativeCaIndex;

	if( pCaDevice )
		*pCaDevice = &m_CaInfo.pCaDevice[nGlobalCaIndex];

	return STATUS_SUCCESS;
	
}

KSTATUS CCameraMgr::QueryCaSN( UCHAR nGlobalCaIndex, PCHAR pDevSN, int nDevSNLength )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo(nGlobalCaIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE)
		return STATUS_DEV_NOT_START;

	try
	{
		status = ca_dev->pRelativeDriver->nDrvAPI.DrvQuerySN(nRelativeCaIndex, pDevSN, nDevSNLength);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::QueryCaSN->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}

	return status;
}


KSTATUS CCameraMgr::SetPrviewWindow( UCHAR nGlobalCaIndex, HWND hPrvWnd, RECT nPrvRC )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;

	status = GetRDriverObjectInfo(nGlobalCaIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE)
		return STATUS_DEV_NOT_START;
	
	try
	{
		status = ca_dev->pRelativeDriver->nDrvAPI.DrvSetPrview(nRelativeCaIndex, hPrvWnd,  nPrvRC);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::SetPrviewWindow->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
	
	return status;
	
}

KSTATUS CCameraMgr::QueryGainRange(PCA_GAIN_RANGE pGainRange)
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo((UCHAR)pGainRange->nGlobalIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE)
		return STATUS_DEV_NOT_START;
	
	try
	{
		status = ca_dev->pRelativeDriver->nDrvAPI.DrvQueryGainRange(nRelativeCaIndex, &pGainRange->nMaxGain, &pGainRange->nMinGain);

		DBG_MSG("CCameraMgr::QueryGainRange nMaxGain = %u\n",pGainRange->nMaxGain);

	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::QueryGainRange->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
	
	return status;
	
}

KSTATUS CCameraMgr::QueryETRange( PCA_EXPOSE_TIME_RANGE pETRange )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo((UCHAR)pETRange->nGlobalIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE)
		return STATUS_DEV_NOT_START;
	
	try
	{
		return ca_dev->pRelativeDriver->nDrvAPI.DrvQueryETRange(nRelativeCaIndex, &pETRange->nMaxExposeTime, &pETRange->nMinExposeTime);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::QueryGainRange->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
	
}

KSTATUS CCameraMgr::QueryConfig( PCAMERA_CONFIG_INFO pCaCfg )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo((UCHAR)pCaCfg->nGlobalIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE)
		return STATUS_DEV_NOT_START;
	
	try
	{
		return ca_dev->pRelativeDriver->nDrvAPI.DrvQueryConfig(nRelativeCaIndex, pCaCfg);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::QueryConfig->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
}

KSTATUS CCameraMgr::SetConfig( PCAMERA_CONFIG_INFO pCaCfg )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo((UCHAR)pCaCfg->nGlobalIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE)
		return STATUS_DEV_NOT_START;
	
	try
	{
		return ca_dev->pRelativeDriver->nDrvAPI.DrvSetConfig(nRelativeCaIndex, pCaCfg);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::SetConfig->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
}

KSTATUS CCameraMgr::ReadRawGrapBuffer( USHORT nGlobalCaIndex, HANDLE *pHandle )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo((UCHAR)nGlobalCaIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE )
		return STATUS_DEV_NOT_START;
	
	try
	{
		return ca_dev->pRelativeDriver->nDrvAPI.DrvReadRawGrapBuffer(nRelativeCaIndex, pHandle);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::ReadRawGrapBuffer->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
}

KSTATUS CCameraMgr::CatchGrap( USHORT nCaIndex,PCHAR pGrapPath,HANDLE hRawGrapBuffer )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo((UCHAR)nCaIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE)
		return STATUS_DEV_NOT_START;
	
	try
	{
		return ca_dev->pRelativeDriver->nDrvAPI.DrvCatchGrap(nRelativeCaIndex, pGrapPath, hRawGrapBuffer);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::CatchGrap->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
}

ULONG CCameraMgr::QueryUseageDevCounter()
{
	return m_CaInfo.nDevCounter;
	
}

ULONG CCameraMgr::QueryStartDevCounter()
{
	ULONG   n = 0;

	for( ULONG j = 0 ; j < m_CaInfo.nDevCounter ; j ++ )
	{
		if( DEV_START(&m_CaInfo.pCaDevice[j].bStart) )
			n ++;
	}

	return n;
	
}

KSTATUS CCameraMgr::CloseAllDevice()
{

	for( ULONG j = 0 ; j < m_CaInfo.nDevCounter ; j ++ )
	{
		if( DEV_START(&m_CaInfo.pCaDevice[j].bStart) )
		{
			try
			{
				DBG_MSG("enter Close %u CaDevice\n",j);
				m_CaInfo.pCaDevice[j].pRelativeDriver->nDrvAPI.DrvCloseAll();
				DBG_MSG("leave CloseCaDevice\n");
			}
			catch (...)
			{
				
			}
		}
	}


	return STATUS_SUCCESS;
}

KSTATUS CCameraMgr::SetPrivewCB( PPRVIEW_OUTPUT_CB prv_out_cb )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo((UCHAR)prv_out_cb->nCaIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE )
		return STATUS_DEV_NOT_START;
	
	try
	{
		return ca_dev->pRelativeDriver->nDrvAPI.DrvOutputCB(nRelativeCaIndex,prv_out_cb);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::SetConfig->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
	
}

KSTATUS CCameraMgr::SetWBalance( USHORT nCaIndex )
{
	USHORT        nRelativeCaIndex;
	KSTATUS       status;
	PCA_DEVICE    ca_dev;
	
	status = GetRDriverObjectInfo((UCHAR)nCaIndex, &nRelativeCaIndex, &ca_dev);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( DEV_START(&ca_dev->bStart) == FALSE )
		return STATUS_DEV_NOT_START;

	try
	{
		return ca_dev->pRelativeDriver->nDrvAPI.DrvSetBalance(nRelativeCaIndex, NULL);
	}
	catch (...)
	{
		DBG_MSG("CCameraMgr::SetConfig->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
	
}

