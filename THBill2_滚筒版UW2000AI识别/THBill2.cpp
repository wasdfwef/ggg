// THBill2.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\..\inc\THFrameDef.h"
#include "interal.h"
#include "DbgMsg.h"
#include "Bill2Arc.h"


#pragma comment(lib, "..\\..\\lib\\fmmc.lib")
#pragma comment(lib, "..\\..\\lib\\QueueManager.lib")
#pragma comment(lib, "..\\..\\lib\\AnalysePic10x.lib")
#pragma comment(lib, "shlwapi.lib")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

static
void
BillSetRelativeOneObject(
PROGRAM_OBJECT  PrgObject,
PCHAR           RelativeName
						 )
{
	PROGRAM_OBJECT   R_Object;
	
	if( FMMC_GetPrgObjectByName(RelativeName, &R_Object) != STATUS_SUCCESS )
		return;
	
	FMMC_SetRelativePrgObject(R_Object, PrgObject);
	FMMC_AvowSupDevPrgObject(PrgObject, R_Object, AvowByDevicePrgObject);
}

static
KSTATUS
BillSetRelative(
PROGRAM_OBJECT PrgObject
)
{
	BillSetRelativeOneObject(PrgObject, UW2000_NEW_NAME);
	BillSetRelativeOneObject(PrgObject, CA_NAME);
	BillSetRelativeOneObject(PrgObject, TH_REC_CELL1_NAME);
	//BillSetRelativeOneObject(PrgObject, TH_UDC_2000_NAME); // 新版UW2000中UDC模块以及集成到US中，不需要单独的UDC处理模块
    BillSetRelativeOneObject(PrgObject, TH_LIS_NAME);
	
	return STATUS_SUCCESS;
}

static
KSTATUS
WINAPI
BillComm_Dispatch_Request(
PROGRAM_OBJECT  PrgObject,
PRP             prp
)
{
	PCHAR               pInputBuffer, pOutputBuffer;
	ULONG               nMajorRequest, nMinorRequest, nInSize, nOutSize;
	KSTATUS             status;
	CBill2Arc          *bill_arc;
	PBILL3_EXTENSION    pExtension;
	
	status = FMMC_GetPrgObjectExtension(PrgObject, (PVOID*)&pExtension);
	
	if( status != STATUS_SUCCESS )
		goto exit_dispatch;
	
	status = FMMC_GetPRPRequest(prp, (PRP_TYPE*)&nMajorRequest, &nMinorRequest);
	
	if( status != STATUS_SUCCESS )
		goto exit_dispatch;
	
	status = FMMC_GetPRPParmeters(prp, (PVOID*)&pInputBuffer, &nInSize, (PVOID*)&pOutputBuffer, &nOutSize);
	
	if( status != STATUS_SUCCESS )
		goto exit_dispatch;
	
	status   = STATUS_INVALID_REQUEST;
	bill_arc = (CBill2Arc*)pExtension->InterFace;
	if( bill_arc == NULL )
		return STATUS_DEV_NOT_START;


	switch(nMajorRequest)
	{
	case PRP_AC_CREATE:
		{
			if( nMinorRequest == START_RELATIVE_PRG )
				status = bill_arc->StartBillReatliveDev(PrgObject);
			break;
		}
	case PRP_AC_DEVIO:
		{
			if( nMinorRequest == CONTROL_CMD_REQUEST && sizeof(LC_COMMAND) == nInSize )
				status = bill_arc->LCProtoDispatch(PrgObject, (PLC_COMMAND)pInputBuffer);

			break;
		}

	case PRP_AC_QUERY:
		{
			//DBG_MSG("PRP_AC_QUERY OPEN_DEV_REQUEST (%d, %d)\n",  sizeof(OPEN_DEV_INFORMATION), nOutSize);
			if( nMinorRequest == OPEN_DEV_REQUEST && sizeof(OPEN_DEV_INFORMATION) == nOutSize )
			{
				POPEN_DEV_INFORMATION   OpenDevInfo = (POPEN_DEV_INFORMATION)pOutputBuffer;
				memcpy(OpenDevInfo, &pExtension->o_dev, sizeof(OPEN_DEV_INFORMATION));
				status = STATUS_SUCCESS;
			}
			
			break;
		}


	case PRP_AC_SET:
		{
			if( nMinorRequest == PRP_MIN_RELATIVE )
				status = BillSetRelative(PrgObject);
			else if( nMinorRequest == SYSTEM_WORK_STATUS_REQUEST && nInSize == sizeof(SYS_WORK_STATUS) )
				status = bill_arc->SetWorkStatus(PrgObject, (PSYS_WORK_STATUS)pInputBuffer);
			
			break;
		}
	case PRP_AC_CLOSE:
		{
			if( nMinorRequest == CLOSE_DEV_REQUEST )
			{
				status = STATUS_SUCCESS;
				bill_arc->CloseDev(PrgObject);
				delete bill_arc;
				ZeroMemory(pExtension, sizeof(BILL3_EXTENSION));
			}
			break;
		}
	default:
		{
			break;
		}
	}

exit_dispatch:
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
		status = FMMC_SetDispatch(ModObject, BillComm_Dispatch_Request, (PRP_TYPE)type);
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
		                          TH_BILL3_NAME,
		                          PRG_OBJECT_NORMAL,
		                          TH_BILL3_DESCRITOR,
		                          sizeof(BILL3_EXTENSION),
		                          &object);
	
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("THBill1->AddProgramObjectDispatch...Fail status = %x\n", status);
	}
	else
	{
		CBill2Arc         *arc;
		PBILL3_EXTENSION   pExtension;
		
		FMMC_GetPrgObjectExtension(object, (PVOID*)&pExtension);
		ZeroMemory(pExtension, sizeof(BILL3_EXTENSION));
		InitializeCriticalSection(&pExtension->cs);

		pExtension->InterFace = new CBill2Arc;
		if( pExtension->InterFace == NULL )
			status =  STATUS_ALLOC_MEMORY_ERROR;

		arc = (CBill2Arc *)pExtension->InterFace;
		status = arc->GetSample10xInfo(pExtension);

	}
	
	return status;
}