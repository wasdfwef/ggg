// THLis.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\..\inc\THFrameDef.h"
#include "interal.h"
#include <shlwapi.h>
#include "DbgMsg.h"

#pragma comment(lib, "..\\..\\lib\\fmmc.lib")
#pragma comment(lib, "shlwapi.lib")

#define  RELATIVE_PATH       "config"
#define  CONFIG_FILE_NAME    "Lis1.ini"


static
KSTATUS
Lic_OpenDev(
PROGRAM_OBJECT      PrgObj,
PLIC_EXTENSION      pExtension,
PSTART_LC_PAR       pStartLC
)
{
	KSTATUS              status;
	OPEN_PAR             OpenPar = {0}; 
	
	status = FMMC_GetPrgObjectByName(SPORT_NAME, &pExtension->ThCommonSPortPrgObject);
	if( status != STATUS_SUCCESS )
		return status;
	
	OpenPar.PrgObject    = PrgObj;
	if( pStartLC->PortCfgPath[0] )
		strcpy(OpenPar.SConfigPath, pStartLC->PortCfgPath);
	else
	{
		GetModuleFileName(NULL, OpenPar.SConfigPath, MAX_PATH);
		PathRemoveFileSpec(OpenPar.SConfigPath);
		PathAppend(OpenPar.SConfigPath, RELATIVE_PATH);
		CreateDirectory(OpenPar.SConfigPath, NULL);
		PathAppend(OpenPar.SConfigPath, CONFIG_FILE_NAME);
	}
	
	OpenPar.DataParseWay = DATA_PARSE_NO_SUP;
	
	status = FMMC_EasySynchronousBuildPRP(pExtension->ThCommonSPortPrgObject, NULL, PRP_AC_CREATE, OPEN_SERIAL_PORT, &OpenPar, sizeof(OpenPar), &pExtension->s_port_handle, sizeof(HANDLE), FALSE, FALSE);
	
	if( status != STATUS_SUCCESS )
		ZeroMemory(pExtension, sizeof(LIC_EXTENSION));

	DBG_MSG("Lic_OpenDev->status=%x\n", status); 
	
	return status;
}

static
KSTATUS  
Lic_QueryConfigDevInfo(
PLIC_EXTENSION pExtension,
PSERIAL_PORT_CONFIG cfg
)
{
	
	KSTATUS                 status;
	PROGRAM_OBJECT          prg_obj;
	SERIAL_PORT_CONFIG      tg = {0};
	
	memcpy(&tg, cfg, sizeof(SERIAL_PORT_CONFIG));
	if( tg.ConfigPath[0] == 0 )
	{
		GetModuleFileName(NULL, tg.ConfigPath, MAX_PATH);
		PathRemoveFileSpec(tg.ConfigPath);
		PathAppend(tg.ConfigPath, RELATIVE_PATH);
		PathAppend(tg.ConfigPath, CONFIG_FILE_NAME);
	}

	if( pExtension->ThCommonSPortPrgObject == NULL )
	{
		status = FMMC_GetPrgObjectByName(SPORT_NAME, &prg_obj);
		if( status != STATUS_SUCCESS )
			return status;
	}

	else
		prg_obj = pExtension->ThCommonSPortPrgObject;
	
	status = FMMC_EasySynchronousBuildPRP(prg_obj, NULL, PRP_AC_QUERY, SERIAL_PORT_CFG, NULL, 0, &tg, sizeof(tg),FALSE, FALSE);
	if( status == STATUS_SUCCESS )
		memcpy(cfg, &tg, sizeof(tg));
	
	return status;
}

static
KSTATUS  
Lic_SetDevInfo(
PLIC_EXTENSION pExtension,
PSERIAL_PORT_CONFIG cfg
)
{
	KSTATUS                 status;
	PROGRAM_OBJECT          prg_obj;
	SERIAL_PORT_CONFIG      tg = {0};
	
	memcpy(&tg, cfg, sizeof(SERIAL_PORT_CONFIG));
	if( tg.ConfigPath[0] == 0 )
	{
		GetModuleFileName(NULL, tg.ConfigPath, MAX_PATH);
		PathRemoveFileSpec(tg.ConfigPath);
		PathAppend(tg.ConfigPath, RELATIVE_PATH);
		PathAppend(tg.ConfigPath, CONFIG_FILE_NAME);
	}

	if( pExtension->ThCommonSPortPrgObject == NULL )
	{
		status = FMMC_GetPrgObjectByName(SPORT_NAME, &prg_obj);
		if( status != STATUS_SUCCESS )
			return status;
	}
	else
		prg_obj = pExtension->ThCommonSPortPrgObject;
	
	return FMMC_EasySynchronousBuildPRP(prg_obj, NULL, PRP_AC_SET, SERIAL_PORT_CFG, &tg, sizeof(tg), NULL, 0, FALSE, FALSE);
}

static
KSTATUS  
Lic_StopLC(
PLIC_EXTENSION  pExtension
)
{
	
	if( pExtension->s_port_handle )
		FMMC_EasySynchronousBuildPRP(pExtension->ThCommonSPortPrgObject, NULL, PRP_AC_CLOSE, CLOSE_SERIAL_PORT, pExtension->s_port_handle, sizeof(HANDLE), NULL, 0, FALSE ,FALSE);
	
	ZeroMemory(pExtension, sizeof(LIC_EXTENSION));
	return STATUS_SUCCESS;
	
}

static
KSTATUS
Lic_WriteBufferToSP(
PLIC_EXTENSION  pExtension,
PBUFFER_INFO    buffer_info
)
{
	WRITE_BUFFER_PROC   proc = {0};

	proc.s_handle   = pExtension->s_port_handle;
	proc.nWriteSize = buffer_info->nLength;
	proc.pWriteBuffer = buffer_info->pBuffer;

	return FMMC_EasySynchronousBuildPRP(pExtension->ThCommonSPortPrgObject, NULL, PRP_AC_WRITE, WRITE_SERIAL_PORT, &proc, sizeof(proc), NULL, 0, FALSE, FALSE);

}

static
KSTATUS
LisQueryDeviceCap(
PLIC_EXTENSION  pExtension,
PDEVICE_CAP_DES pDevCap
)
{
	KSTATUS   status = STATUS_INVALID_REQUEST;

	switch( pDevCap->CapDesType )
	{
	case DevCconfig:
		{
			if( pDevCap->nOutputBufferLength != sizeof(SERIAL_PORT_CONFIG) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}

			status = Lic_QueryConfigDevInfo(pExtension, (PSERIAL_PORT_CONFIG)pDevCap->pOutputBuffer);
			break;
		}
	case UsageDevNumber:
		{
			if( pDevCap->nOutputBufferLength != sizeof(ULONG) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}

			if( pExtension->s_port_handle )
				*((PULONG)pDevCap->pOutputBuffer) = 0;
			else
				*((PULONG)pDevCap->pOutputBuffer) = 1;

			status = STATUS_SUCCESS;
			break;
		}

	case StartDevNumber:
		{
			
			if( pDevCap->nOutputBufferLength != sizeof(ULONG) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}

			if( pExtension->s_port_handle )
				*((PULONG)pDevCap->pOutputBuffer) = 1;
			else
				*((PULONG)pDevCap->pOutputBuffer) = 0;
			
			status = STATUS_SUCCESS;
			break;
		}
	}


	return status;
}

static
KSTATUS
LisSetDeviceCap(
PLIC_EXTENSION  pExtension,
PDEVICE_CAP_DES pDevCap
)
{
	KSTATUS   status = STATUS_INVALID_REQUEST;
	
	if( pDevCap->CapDesType == DevCconfig )
	{
		if( pDevCap->nInBufferLength != sizeof(SERIAL_PORT_CONFIG) )
			return STATUS_INVALID_PARAMETERS;
		
		status = Lic_SetDevInfo(pExtension, (PSERIAL_PORT_CONFIG)pDevCap->pInputBuffer);
		
	}
	
	return status;
}

static
KSTATUS
WINAPI
USImport_Dispatch_Request(
						  PROGRAM_OBJECT  PrgObj,
						  PRP             prp
						  )
{
	PCHAR                  pInputBuffer, pOutBuffer;
	ULONG                  nMajorFunction, nMinorFunction, nInSize, nOutSize;
	KSTATUS                status = STATUS_INVALID_REQUEST;
	PLIC_EXTENSION         pExtension;
	
	
	status = FMMC_GetPRPRequest(prp, (PRP_TYPE*)&nMajorFunction, &nMinorFunction);
	if( status != STATUS_SUCCESS )
		goto r_exit;
	
	status = FMMC_GetPRPParmeters(prp, (PVOID*)&pInputBuffer, &nInSize, (PVOID*)&pOutBuffer, &nOutSize);
	if( status != STATUS_SUCCESS )
		goto r_exit;
	
	status = FMMC_GetPrgObjectExtension(PrgObj, (PVOID*)&pExtension);
	if( status != STATUS_SUCCESS )
		goto r_exit;
	
	
	status = STATUS_INVALID_REQUEST;
	
	switch(nMajorFunction)
	{
	case PRP_AC_CREATE:
		{
			if( nMinorFunction == START_LC_REQUEST && nInSize == sizeof(START_LC_PAR) )
				status = Lic_OpenDev(PrgObj, pExtension, (PSTART_LC_PAR)pInputBuffer);
			
			break;
		}
	case PRP_AC_QUERY:
		{
			if( nMinorFunction == QUERY_DEVICE_CAP && sizeof(DEVICE_CAP_DES) == nInSize )
				status = LisQueryDeviceCap(pExtension, (PDEVICE_CAP_DES)pInputBuffer);
			
			break;
		}
	case PRP_AC_SET:
		{
			if( nMinorFunction == PRP_MIN_RELATIVE )
				status = STATUS_SUCCESS;
	
			else if( nMinorFunction == QUERY_DEVICE_CAP && sizeof(DEVICE_CAP_DES) == nInSize )
				status = LisSetDeviceCap(pExtension, (PDEVICE_CAP_DES)pInputBuffer);
			
			break;
		}
	case PRP_AC_DEVIO:
		{
			if( nMinorFunction == RCV_SERIAL_PORT && nInSize == sizeof(RCV_BUFFER_PROC) )
				status = STATUS_SUCCESS;
			
			break;
		}
	case PRP_AC_WRITE:
		{
			if( nMinorFunction == WRITE_BUFFER_REQUEST && nInSize == sizeof(BUFFER_INFO) )
				status = Lic_WriteBufferToSP(pExtension, (PBUFFER_INFO)pInputBuffer);
			break;
		}
	case PRP_AC_CLOSE:
		{
			if( nMinorFunction == STOP_LC_REQUEST )
				status = Lic_StopLC(pExtension);
			break;
		}
	default:
		{
			break;
		}
	}
r_exit:
	
	FMMC_RequestComplete(prp, status);
	return status;
	
}

extern "C"
KSTATUS
WINAPI
ModuleEntry(
MOD_OBJECT  ModObject
)
{
	KSTATUS   status;
	
	for( ULONG type = PRP_AC_CREATE ; type < PRP_MAX_NUM ; type ++ )
	{
		status = FMMC_SetDispatch(ModObject, USImport_Dispatch_Request, (PRP_TYPE)type);
		if( status != STATUS_SUCCESS )
			break;
	}
	
	return status;
}

extern "C"
KSTATUS
WINAPI
AddProgramObjectDispatch(
						 MOD_OBJECT  ModObject
						 )
{
	KSTATUS              status;
	PROGRAM_OBJECT       object;
	
	status = FMMC_CreatePrgObject(ModObject,
		TH_LIS_NAME,
		PRG_OBJECT_DEVICE,
		TH_LIS_DESCRITOR,
		sizeof(LIC_EXTENSION),
		&object);
	
	if( status == STATUS_SUCCESS )
	{
		PLIC_EXTENSION   pExtension;
		
		FMMC_GetPrgObjectExtension(object, (PVOID*)&pExtension);
		ZeroMemory(pExtension, sizeof(LIC_EXTENSION));
		
	}
	
	return status;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

