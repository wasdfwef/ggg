#include "StdAfx.h"
#include "RequestHandler.h"
#include "UCCmdHandler.h"
#include "ProtMapping.h"
#include "shlwapi.h"
#include "THSPDef.h"
#include "interal.h"
#include "THFrameDef.h"
#include "LCCmdHandler.h"
#include <strsafe.h>
#include "global.h"
#include "TranslateCmd.h"
#include "ThLcProto_UW2000.h"

#pragma comment(lib, "shlwapi")

HANDLE g_LCAlivetEvent = NULL;
HANDLE g_SystemExitEvent = NULL;
HANDLE g_MoniterThread = NULL;

#define  RELATIVE_PATH       "config"
#define  CONFIG_FILE_NAME    "UW2000.ini"


// static   QUEUE_HANDLE              m_WaitQue = NULL;
// static   PU12SDEV_EXTENSION        m_Extension = NULL;
void ReleaseHandle(HANDLE Handle)
{
	if (NULL != Handle)
	{
		CloseHandle(Handle);
		Handle = NULL;
	}
}


void GetConfigPath(CHAR *ConfigPath)
{
	GetModuleFileName(NULL, ConfigPath, MAX_PATH);
	PathRemoveFileSpec(ConfigPath);
	PathAppend(ConfigPath, RELATIVE_PATH);
	PathAppend(ConfigPath, CONFIG_FILE_NAME);
}


KSTATUS StartSPort(PROGRAM_OBJECT  PrgObject, PSTART_LC_PAR pStartLC)
{
	KSTATUS                Status;
	OPEN_PAR               OpenPar = {0};
	PDEV_EXTENSION		   pExtension;
	
	DbgPrint("enter StartSPort\n");

	Status = FMMC_GetPrgObjectExtension(PrgObject, (PVOID*)&pExtension);
	CheckResultCode("StartSPort", "FMMC_GetPrgObjectExtension", Status);
	
	if( NULL != pExtension->hSport )
	{
		DbgPrint("pExtension->hSport is nonnull and is invalid");
		Status = STATUS_DEV_START_ALREADY;
		goto end;
	}
	
	
	if( pStartLC->PortCfgPath[0] && !PathFileExists(pStartLC->PortCfgPath) )
	{
		DbgPrint("%s is not exist", pStartLC->PortCfgPath);
		Status = STATUS_INVALID_PARAMETERS;
		goto end;
	}
	
	Status = FMMC_GetPrgObjectByName(SPORT_NAME, &pExtension->SPortPrgObject);
	CheckResultCode("StartSPort", "FMMC_GetPrgObjectByName", Status);
	
	pExtension->BillPrgObject = pStartLC->PrgObject;
	
	if( pStartLC->PortCfgPath[0] )
	{
		StringCbCopy(OpenPar.SConfigPath, MAX_PATH, pStartLC->PortCfgPath);
	}
	else
	{
		GetConfigPath(OpenPar.SConfigPath);
	}
	OpenPar.PrgObject = PrgObject;
	OpenPar.DataParseWay = DATA_PRASE_TYPE3;
	
	DbgPrint("pExtension->SPortPrgObject: 0x%x\n", pExtension->SPortPrgObject);
	Status = FMMC_EasySynchronousBuildPRP(pExtension->SPortPrgObject, NULL, PRP_AC_CREATE, OPEN_SERIAL_PORT, &OpenPar, sizeof(OpenPar), &pExtension->hSport, sizeof(HANDLE), FALSE, FALSE);
	CheckResultCode("StartSPort", "FMMC_EasySynchronousBuildPRP", Status);

end:
	DbgPrint("leave StartSPort\n");
	return Status;
}


static KSTATUS InitGlobalData(PROGRAM_OBJECT  PrgObject)
{
	KSTATUS Status = STATUS_SUCCESS;

	InitializeCriticalSection(&g_CS);
	g_LCAlivetEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_SystemExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (NULL == g_LCAlivetEvent || NULL == g_SystemExitEvent)
	{
		Status = STATUS_INVALID_OBJECT;
		goto end;
	}
	
	
// 	g_MoniterThread = CreateThread(NULL, 0, MonitorLCStatus, (PVOID)PrgObject, 0, NULL);
// 	if (NULL == g_MoniterThread )
// 	{
// 		DbgPrint( "CreateThread failed with error = %d", GetLastError() );
// 		Status = STATUS_START_THREAD_FAIL;
// 		goto end;
// 	}

end:
	return Status;
}


static void ReleaseGlobalData()
{
	ReleaseHandle(g_LCAlivetEvent);
	ReleaseHandle(g_SystemExitEvent);
	ReleaseHandle(g_MoniterThread);
	DeleteCriticalSection(&g_CS);
}


KSTATUS StartLC(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	ULONG                  nInSize;
	PSTART_LC_PAR          pStartLC;
	KSTATUS Status = STATUS_SUCCESS;

	Status = FMMC_GetPRPParmeters(prp, (PVOID*)&pStartLC, &nInSize, NULL, NULL);
	CheckResultCode("StartLC", "FMMC_GetPRPParmeters", Status);
	
	if( nInSize != sizeof(START_LC_PAR) || pStartLC->PrgObject == NULL )
	{
		DbgPrint(" nInSize != sizeof(START_LC_PAR) || pStartLC->PrgObject == NULL ");
		Status = STATUS_INVALID_PARAMETERS;
		goto end;
	}

	Status = StartSPort(PrgObject, pStartLC);
	CheckResultCode("StartLC", "StartSPort", Status);

// 	Status = ResetLC(PrgObject);
// 	CheckResultCode("StartLC", "ResetLC", Status);

	Status = InitGlobalData(PrgObject);
	CheckResultCode("StartLC", "InitGlobalData", Status);	
	
end:
	return Status;
}


KSTATUS QueryDevInfo(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	ULONG                   nOutSize;
	KSTATUS                 Status;
	PROGRAM_OBJECT          prg_obj;
	PDEV_EXTENSION			pExtension;
	SERIAL_PORT_CONFIG      tg = {0};
	PSERIAL_PORT_CONFIG     cfg;
	
	Status = FMMC_GetPRPParmeters(prp, NULL, NULL,(PVOID*)&cfg, &nOutSize);
	CheckResultCode("QueryDevInfo", "FMMC_GetPRPParmeters", Status);
	
	if( nOutSize != sizeof(SERIAL_PORT_CONFIG) )
	{
		DbgPrint( "nOutSize != sizeof(SERIAL_PORT_CONFIG)" );
		Status = STATUS_INVALID_PARAMETERS;
		goto end;
	}
	
	Status = FMMC_GetPrgObjectExtension(PrgObject, (PVOID*)&pExtension);
	CheckResultCode("QueryDevInfo", "FMMC_GetPrgObjectExtension", Status);

	memcpy(&tg, cfg, sizeof(SERIAL_PORT_CONFIG));
	if( tg.ConfigPath[0] == 0 )
	{
		GetConfigPath(tg.ConfigPath);
	}

	if( pExtension->SPortPrgObject == NULL )
	{
		Status = FMMC_GetPrgObjectByName(SPORT_NAME, &prg_obj);
		CheckResultCode("QueryDevInfo", "FMMC_GetPrgObjectByName", Status);
	}
	else
	{
		prg_obj = pExtension->SPortPrgObject;
	}
	
	Status = FMMC_EasySynchronousBuildPRP(prg_obj, NULL, PRP_AC_QUERY, SERIAL_PORT_CFG, NULL, 0, &tg, sizeof(tg),FALSE, FALSE);
	CheckResultCode("QueryDevInfo", "FMMC_EasySynchronousBuildPRP", Status);

	memcpy(cfg, &tg, sizeof(tg));
end:
	return Status;
}

extern PROGRAM_OBJECT g_UW2000Object;

KSTATUS QueryDeviceCap(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	KSTATUS Result = STATUS_SUCCESS;
	PDEVICE_CAP_DES pDevCapDes = NULL;
	PDEV_EXTENSION pExtension = NULL;
	ULONG nInSize = 0;

	Result = FMMC_GetPrgObjectExtension(PrgObject, (PVOID*)&pExtension);
	CheckResultCode("SetDevInfo", "FMMC_GetPrgObjectExtension", Result);

	Result = FMMC_GetPRPParmeters(prp, (PVOID*)&pDevCapDes, &nInSize, NULL, NULL);
	CheckResultCode("SetDevInfo", "FMMC_GetPRPParmeters", Result);

	if( nInSize != sizeof(DEVICE_CAP_DES) )
	{
		Result =  STATUS_INVAILD_DATA;
		goto end;
	}

	switch(pDevCapDes->CapDesType)
	{
	case SupDevNumber:
		{
			if( pDevCapDes->nOutputBufferLength != sizeof(ULONG) )
			{
				Result = STATUS_INVALID_PARAMETERS;
				break;
			}
			
			*((PULONG)pDevCapDes->pOutputBuffer) = 1;
			Result = STATUS_SUCCESS;
			break;
		}
	case UsageDevNumber:
		{
			if( pDevCapDes->nOutputBufferLength != sizeof(ULONG) )
			{
				Result = STATUS_INVALID_PARAMETERS;
				break;
			}
			
			if( pExtension->hSport )
				*((PULONG)pDevCapDes->pOutputBuffer) = 0;
			else
				*((PULONG)pDevCapDes->pOutputBuffer) = 1;
			
			Result = STATUS_SUCCESS;
			break;
		}
	case StartDevNumber:
		{
			if( pDevCapDes->nOutputBufferLength != sizeof(ULONG) )
			{
				Result = STATUS_INVALID_PARAMETERS;
				break;
			}
			
			if( pExtension->hSport )	
				*((PULONG)pDevCapDes->pOutputBuffer) = 1;
			else
				*((PULONG)pDevCapDes->pOutputBuffer) = 0;
			
			Result = STATUS_SUCCESS;
			break;
		}
	case DevCconfig:
		{
			if( pDevCapDes->nOutputBufferLength != sizeof(SERIAL_PORT_CONFIG) )
			{
				Result = STATUS_INVALID_PARAMETERS;
				break;
			}

			if (g_UW2000Object == NULL) 
			{
				Result = STATUS_UNINIT_STATUS;
				break;
			}

			Result = FMMC_EasySynchronousBuildPRP(g_UW2000Object, NULL, PRP_AC_QUERY, SERIAL_PORT_CFG, NULL, 0, pDevCapDes->pOutputBuffer, pDevCapDes->nOutputBufferLength, FALSE, FALSE);
			CheckResultCode("QueryDeviceCap", "FMMC_EasySynchronousBuildPRP", Result);
			break;
		}
		
	default:
		{
			break;
		}
	}

end:
	return Result;
}


KSTATUS SetRelation(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	return FMMC_SetPassivityRelation(PrgObject);
}


KSTATUS SetDevInfo(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	ULONG                   nInSize;
	KSTATUS                 Status = STATUS_INVALID_REQUEST;
	PROGRAM_OBJECT          prg_obj;
	PDEV_EXTENSION			pExtension;
	SERIAL_PORT_CONFIG      tg = {0};
	PSERIAL_PORT_CONFIG		cfg;
	PDEVICE_CAP_DES		    pDevCap;
	
	Status = FMMC_GetPRPParmeters(prp, (PVOID*)&pDevCap, &nInSize, NULL, NULL);
	CheckResultCode("SetDevInfo", "FMMC_GetPRPParmeters", Status);

	if( nInSize != sizeof(DEVICE_CAP_DES) )
	{
		DbgPrint( "nInSize != sizeof(DEVICE_CAP_DES)" );
		Status = STATUS_INVALID_PARAMETERS;
		goto end;
	}
	
	if( pDevCap->CapDesType == DevCconfig )
	{
		if( pDevCap->nInBufferLength != sizeof(SERIAL_PORT_CONFIG) )
			return STATUS_INVALID_PARAMETERS;

		Status = FMMC_GetPrgObjectExtension(PrgObject, (PVOID*)&pExtension);
		CheckResultCode("SetDevInfo", "FMMC_GetPrgObjectExtension", Status);
		
		cfg = (PSERIAL_PORT_CONFIG)pDevCap->pInputBuffer;
		memcpy(&tg, cfg, sizeof(SERIAL_PORT_CONFIG));
		if( tg.ConfigPath[0] == 0 )
		{
			GetConfigPath(tg.ConfigPath);
		}
		
		if( pExtension->SPortPrgObject == NULL )
		{
			Status = FMMC_GetPrgObjectByName(SPORT_NAME, &prg_obj);
			CheckResultCode("SetDevInfo", "FMMC_GetPrgObjectByName", Status);
		}
		else
		{
			prg_obj = pExtension->SPortPrgObject;
		}
		
		Status = FMMC_EasySynchronousBuildPRP(prg_obj, NULL, PRP_AC_SET, SERIAL_PORT_CFG, &tg, sizeof(tg), NULL, 0, FALSE, FALSE);
		CheckResultCode("SetDevInfo", "FMMC_EasySynchronousBuildPRP", Status);
	}
	
end:
	return Status;
}

KSTATUS DispatchCmd(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	return HandleUCCmd(PrgObject, prp);
}

KSTATUS RecvFromSport(PROGRAM_OBJECT  PrgObject, PRP prp)
{	
	return HandleLCCmd(PrgObject, prp);
}

KSTATUS StopSPort(PROGRAM_OBJECT  PrgObject)
{
	PDEV_EXTENSION   pExtension;

	DbgPrint("enter StopSPort\n");
	
	FMMC_GetPrgObjectExtension(PrgObject, (PVOID*)&pExtension);
	
	if( pExtension->hSport )
		FMMC_EasySynchronousBuildPRP(pExtension->SPortPrgObject, NULL, PRP_AC_CLOSE, CLOSE_SERIAL_PORT, pExtension->hSport, sizeof(HANDLE), NULL, 0, FALSE ,FALSE);
	
	ZeroMemory(pExtension, sizeof(DEV_EXTENSION));

	DbgPrint("leave StopSPort\n");
	return STATUS_SUCCESS;
}


KSTATUS StopLC(PROGRAM_OBJECT  PrgObject, PRP prp)
{
	StopSPort(PrgObject);

	if ( NULL != g_SystemExitEvent)
	{
		SetEvent(g_SystemExitEvent);
	}
	
	if (NULL != g_MoniterThread)
	{
		WaitForSingleObject(g_MoniterThread, INFINITE);
	}
	

	ReleaseGlobalData();
	DbgPrint("leave StopLC\n");

	return STATUS_SUCCESS;
}
