// UW2000Dev.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "THFrameDef.h"
#include "ThLcProto_UW2000.h"
#include "interal.h"
#include "RequestHandler.h"

#pragma comment(lib, "fmmc.lib")

typedef KSTATUS (*RequestHandler)(PROGRAM_OBJECT  PrgObject, PRP prp);

struct RequestInfo
{
	PRP_TYPE MajorFunction;
	DWORD MinorFunction;
	RequestHandler Func;
};


static RequestInfo RequestList[] = 
{
	PRP_AC_CREATE,	START_LC_REQUEST,		StartLC,
	PRP_AC_QUERY,	SERIAL_PORT_CFG,		QueryDevInfo,
	PRP_AC_SET,		PRP_MIN_RELATIVE,		SetRelation, 
	PRP_AC_SET,		QUERY_DEVICE_CAP,		SetDevInfo,
	PRP_AC_DEVIO,	CONTROL_CMD_REQUEST,	DispatchCmd,
	PRP_AC_DEVIO,	RCV_SERIAL_PORT,		RecvFromSport,
	PRP_AC_CLOSE,	STOP_LC_REQUEST,		StopLC,
	PRP_AC_QUERY,	QUERY_DEVICE_CAP,		QueryDeviceCap,
};


PROGRAM_OBJECT g_UW2000Object = NULL;


BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
    return TRUE;
}


extern "C" KSTATUS WINAPI Dev_Request_Dispatch(PROGRAM_OBJECT ProgramObject, PRP prp)
{
	DWORD	  RequestIndex = 0;
	ULONG     nMajorFunction, nMinorFunction;
	KSTATUS   Status = STATUS_SUCCESS;
	
	
	Status = FMMC_GetPRPRequest(prp, (PRP_TYPE*)&nMajorFunction, &nMinorFunction);	
	CheckResultCode("Dev_Request_Dispatch", "FMMC_GetPRPRequest", Status);
	
	Status = STATUS_INVALID_REQUEST;
	for (; RequestIndex < GetItemCount(RequestList); ++RequestIndex)
	{
		if ( (nMajorFunction == RequestList[RequestIndex].MajorFunction) && (nMinorFunction == RequestList[RequestIndex].MinorFunction) )
		{
			Status = RequestList[RequestIndex].Func(ProgramObject, prp);
			goto end;
		}
	}
	
	DbgPrint( "cmd(nMajorFunction: %d, nMinorFunction:%d) is not handled!!!!!", nMajorFunction, nMinorFunction );
	
end:
	FMMC_RequestComplete(prp, Status);
	return Status;
}


extern "C" KSTATUS WINAPI ModuleEntry(MOD_OBJECT ModObject)
{
	KSTATUS   Status = STATUS_SUCCESS;
	
	for( ULONG type = PRP_AC_CREATE ; type < PRP_MAX_NUM ; type ++ )
	{
		Status = FMMC_SetDispatch(ModObject, Dev_Request_Dispatch, (PRP_TYPE)type);
		CheckResultCode("ModuleEntry", "FMMC_SetDispatch", Status);
	}
	
end:
	return Status;
}

extern "C" KSTATUS WINAPI AddProgramObjectDispatch(MOD_OBJECT ModObject)
{
	KSTATUS              Status;
	PDEV_EXTENSION   pExtension;
	
	Status = FMMC_CreatePrgObject(ModObject, UW2000_NEW_NAME, PRG_OBJECT_DEVICE, UW2000_NEW_DESCRITOR, sizeof(DEV_EXTENSION), &g_UW2000Object);
	CheckResultCode("AddProgramObjectDispatch", "FMMC_CreatePrgObject", Status);
	
	FMMC_GetPrgObjectExtension(g_UW2000Object, (PVOID*)&pExtension);
	ZeroMemory(pExtension, sizeof(DEV_EXTENSION));
	
end:
	return Status;
}
