// THMMgr.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\..\inc\THMMgr.h"
#include "DbgMsg.h"
#include "interal.h"
#include "..\..\inc\THInterface.h"

#pragma comment(lib, "..\\..\\lib\\fmmc.lib")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )										 
{
    return TRUE;
}

																																	   
KSTATUS  MMgr_ReadStatusInfomation(HANDLE  hExitEvent, PTH_STATUS_INFORMATION  pStatusInfo, PMMGR_EXTENSION  pExtension)
{
	DWORD     dwRet;
	ULONG     nCounter = 1;
	HANDLE    hEventArray[2] = {NULL};
																								   
retry_loc:
	if( Que_GetHeaderData(pExtension->que_handle, pStatusInfo) == TRUE )
		return STATUS_SUCCESS;
	
	hEventArray[0] = pExtension->hSystemReqeustEvent;
	if( hExitEvent )
	{
		hEventArray[1] = hExitEvent;
		nCounter = 2;
	}
	else
		nCounter = 1;
	
	dwRet = WaitForMultipleObjects(nCounter, hEventArray, FALSE, INFINITE);
	if( dwRet == WAIT_FAILED )
		return STATUS_INVALID_PARAMETERS;
	
	if( dwRet == WAIT_OBJECT_0 )
		goto retry_loc;
	
	return STATUS_CANCEL_ACTION;
}

KSTATUS MMgr_WriteStatusInfo(PMMGR_EXTENSION  pExtension, PTH_STATUS_INFORMATION pStatusInfo)
{
	TH_STATUS_INFORMATION t;

	if( pStatusInfo->i_type < ThmeInformationType || pStatusInfo->i_type > UserInformationType )
	{
		return STATUS_INVALID_PARAMETERS;
	}


	if( pStatusInfo->content.bSysBuffer == TRUE )
	{
		if( pStatusInfo->content.length == 0 )
			return STATUS_INVALID_PARAMETERS;

		if( IsBadWritePtr(pStatusInfo->content.buffer, pStatusInfo->content.length) == TRUE )
			return STATUS_INVAILD_DATA;
	}
	else
	{
		if( pStatusInfo->i_type == ThmeInformationType && pStatusInfo->content.length > sizeof(ULONG) ) //ThmeInformationType不允许非系统缓存的字节长度超过ULONG
		{
			return STATUS_INVALID_PARAMETERS;
		}
	}

	memcpy(&t, pStatusInfo, sizeof(t));

	if( pStatusInfo->i_type == ThmeInformationType )
	{
		if( pStatusInfo->content.bSysBuffer == TRUE )
		{
			t.content.buffer = (PCHAR)FMMC_CreateBuffer(pStatusInfo->content.length);
			if( t.content.buffer == NULL )
				return STATUS_ALLOC_MEMORY_ERROR;
			
			memcpy(t.content.buffer, pStatusInfo->content.buffer, pStatusInfo->content.length);
		}
	}
	else
	{
		if( t.content.bSysBuffer == TRUE )
			return STATUS_INVALID_PARAMETERS;
	}
		

	if( Que_InsertData(pExtension->que_handle, &t) == FALSE )
	{
		if( t.content.bSysBuffer && t.content.buffer )
			FMMC_FreeBuffer(t.content.buffer);

		return STATUS_INSERT_QUE_FAIL;
	}

	SetEvent(pExtension->hSystemReqeustEvent);

	return STATUS_SUCCESS;
}

KSTATUS MMgr_Destory(PMMGR_EXTENSION  pExtension)
{
	TH_STATUS_INFORMATION   status_info;

	ZeroMemory(&status_info, sizeof(status_info));
	status_info.i_type  = ThmeInformationType;
	status_info.i_class = SystemExitClass;
	MMgr_WriteStatusInfo(pExtension, &status_info);
	Sleep(200);
	while( Que_GetHeaderData(pExtension->que_handle, &status_info) )
	{
		if( status_info.content.bSysBuffer == TRUE )
			FMMC_FreeBuffer(status_info.content.buffer);
	}

	Que_ReleaseList(pExtension->que_handle);
	pExtension->que_handle = NULL;
	return STATUS_SUCCESS;
}

static
KSTATUS
WINAPI
MMgr_Dispatch_Request(
PROGRAM_OBJECT  PrgObject,
PRP             prp
)
{
	PCHAR              pInputBuffer, pOutputBuffer;
	ULONG              nMajorRequest, nMinorRequest, nInSize, nOutSize;
	KSTATUS            status;
	PMMGR_EXTENSION    pExtension;

	status = FMMC_GetPRPRequest(prp, (PRP_TYPE*)&nMajorRequest, &nMinorRequest);
	if( status != STATUS_SUCCESS )
		goto err_exit;

	status = FMMC_GetPRPParmeters(prp, (PVOID*)&pInputBuffer, &nInSize, (PVOID*)&pOutputBuffer, &nOutSize);
	if( status != STATUS_SUCCESS )
		goto err_exit;

	status = FMMC_GetPrgObjectExtension(PrgObject, (PVOID*)&pExtension);
	if( status != STATUS_SUCCESS )
		goto err_exit;

	status = STATUS_INVALID_REQUEST;

	switch(nMajorRequest)
	{
	case PRP_AC_CREATE:
		{
			if( nMinorRequest == MMGR_CREATE_REQUEST && pExtension->que_handle == NULL )
			{
				pExtension->que_handle = Que_Initialize(sizeof(TH_STATUS_INFORMATION));
				if( pExtension->que_handle == NULL )
					status = STATUS_INITIALZIE_QUE_FAIL;
				else
				{
					status = STATUS_SUCCESS;
					ResetEvent(pExtension->hSystemReqeustEvent);
				}
			}
			break;
		}
	case PRP_AC_READ:
		{
			if( nMinorRequest == MMGR_MSG_REQUEST )
			{
				HANDLE   hUserExitEvent = NULL;
				if( nOutSize != sizeof(TH_STATUS_INFORMATION) || pExtension->que_handle == NULL )
				{
					status = STATUS_INVALID_PARAMETERS;
					break;
				}

				if( nInSize == sizeof(HANDLE) )
					hUserExitEvent = *((HANDLE*)pInputBuffer);

				status = MMgr_ReadStatusInfomation(hUserExitEvent, (PTH_STATUS_INFORMATION)pOutputBuffer, pExtension);
			}
			break;
		}
	case PRP_AC_WRITE:
		{
			if( nMinorRequest == MMGR_MSG_REQUEST )
			{
				if( nInSize != sizeof(TH_STATUS_INFORMATION) || pExtension->que_handle == NULL )
				{
					status = STATUS_INVALID_PARAMETERS;
					break;
				}

				status = MMgr_WriteStatusInfo(pExtension, (PTH_STATUS_INFORMATION)pInputBuffer);
			}
			break;
		}
	case PRP_AC_CLOSE:
		{
			if( nMinorRequest == MMGR_DESTORY_REQUEST )
			{
				if( pExtension->que_handle == NULL )
					status = STATUS_INVALID_PARAMETERS;

				status = MMgr_Destory(pExtension);
			}
			break;

		}
	default:
		{
			status = STATUS_INVALID_REQUEST;
			break;
		}
	}

err_exit:

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
		status = FMMC_SetDispatch(ModObject, MMgr_Dispatch_Request, (PRP_TYPE)type);
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
		                          TH_MMGR_NAME,
		                          PRG_OBJECT_ONESELF,
		                          TH_MMGR_DESCRITOR,
		                          sizeof(MMGR_EXTENSION),
		                          &object);
	
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("THMMgr->AddProgramObjectDispatch...Fail status = %x\n", status);
	}
	else
	{
		PMMGR_EXTENSION   pExtension;
		
		FMMC_GetPrgObjectExtension(object, (PVOID*)&pExtension);
		ZeroMemory(pExtension, sizeof(MMGR_EXTENSION));
		pExtension->hSystemReqeustEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if( pExtension->hSystemReqeustEvent == NULL )
		     return STATUS_CREATE_EVENT_FAIL;
									
	}
	
	return status;
}