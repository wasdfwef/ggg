// THSPorts.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SPorts.h"
#include "..\..\inc\THFrameDef.h"
#include "DbgMsg.h"

#pragma comment(lib, "..\\..\\lib\\fmmc.lib")

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
		status = FMMC_SetDispatch(ModObject, Sports_Request_Dispatch, (PRP_TYPE)type);
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
		                          SPORT_NAME,
								  PRG_OBJECT_ONESELF,
								  SPORT_DESCRITOR,
								  0,
								  &object);

	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("THSPorts->AddProgramObjectDispatch...Fail status = %x\n", status);
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

