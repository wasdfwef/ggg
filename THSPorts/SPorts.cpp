#include "StdAfx.h"
#include "SPorts.h"
#include "..\..\inc\THFrameDef.h"
#include "interal.h"
#include <stdio.h>
#include <malloc.h>
#include "DbgMsg.h"
#include <shlwapi.h>
#include "THSerialPort.h"

#pragma comment(lib, "shlwapi.lib")


static
KSTATUS
Sport_OpenSPort(
PROGRAM_OBJECT   ProgramObject,
PRP              prp
)
{
	ULONG              nInSize, nOutSize;
	KSTATUS            status;
	HANDLE             hBackHandle = NULL;
	POPEN_PAR          pPar;
	PSPORTS_PROC_INFO  s_info = NULL;


	status = FMMC_GetPRPParmeters(prp, (PVOID *)&pPar, &nInSize,  &hBackHandle, &nOutSize);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("Sport_OpenSPort err\n");
		return status;
	}

	if( nInSize != sizeof(OPEN_PAR) || nOutSize != sizeof(HANDLE))
	{
		DBG_MSG("Sport_OpenSPort...nInSize nOutSize invaild...\n");
		return STATUS_INVALID_PARAMETERS;
	}

	s_info = (PSPORTS_PROC_INFO)malloc(sizeof(SPORTS_PROC_INFO));
	if( s_info == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	ZeroMemory(s_info, sizeof(SPORTS_PROC_INFO));
	s_info->serial_port = new CTHSerialPort;
	status = s_info->serial_port->CreateSerialPort(pPar);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("Sport_OpenSPort CreateSerialPortErr\n");
		delete s_info->serial_port;
		free(s_info);
	}
	else
	{
		HANDLE    *hTHandle;
		s_info->InteralFlag = SPORTS_FLAG;
		hTHandle = (HANDLE *)hBackHandle;
		*hTHandle = s_info;
	}

	return status;
}


static
KSTATUS
Sports_CreateRequest_Dispatch(
PROGRAM_OBJECT   ProgramObject,
PRP              prp,
ULONG            nMinorRequest
)
{
	KSTATUS   status = STATUS_INVALID_REQUEST;


	if( nMinorRequest == OPEN_SERIAL_PORT )
		status = Sport_OpenSPort(ProgramObject, prp);

	return status;

}


static
KSTATUS
Sports_WriteBufferToPorts(
PROGRAM_OBJECT   ProgramObject,
PRP              prp
)
{
	ULONG                  nSize;
	PWRITE_BUFFER_PROC     w_proc;
	PSPORTS_PROC_INFO      s_proc_info;
	KSTATUS                status;

    status = FMMC_GetPRPParmeters(prp, (PVOID *)&w_proc, &nSize,  NULL, NULL);
	if( status != STATUS_SUCCESS )
		return status;

	if( nSize != sizeof(WRITE_BUFFER_PROC) || w_proc->s_handle == NULL || w_proc->nWriteSize == 0 || w_proc->pWriteBuffer == NULL )
		return STATUS_INVALID_PARAMETERS;

	try
	{
		s_proc_info = (PSPORTS_PROC_INFO)w_proc->s_handle;
		if( IsBadReadPtr(s_proc_info, sizeof(SPORTS_PROC_INFO)) == TRUE ||
			s_proc_info->InteralFlag != SPORTS_FLAG )
		{
			DBG_MSG("Sports_WriteBufferToPorts Invaild s_proc_info...\n");
			return STATUS_INVALID_PARAMETERS;
		}
		status = s_proc_info->serial_port->WriteBufferToSerialPort(w_proc->pWriteBuffer, w_proc->nWriteSize);
	}
	catch(...)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("Sports_WriteBufferToPorts Exception...\n");
	}

	return status;
}

static
KSTATUS
Sports_WriteRequest_Dispatch(
PROGRAM_OBJECT   ProgramObject,
PRP              prp,
ULONG            nMinorRequest
)
{
	KSTATUS  status = STATUS_INVALID_REQUEST;

	if( nMinorRequest == WRITE_SERIAL_PORT )
		status = Sports_WriteBufferToPorts(ProgramObject, prp);

	return status;
}


static
KSTATUS
Sports_CloseSerialPort(
PROGRAM_OBJECT  ProgramObject,
PRP             prp
)
{
	ULONG                 nSize;
	KSTATUS               status;
	PSPORTS_PROC_INFO     s_info;

	status = FMMC_GetPRPParmeters(prp, (PVOID*)&s_info, &nSize, NULL, 0);

	if( status != STATUS_SUCCESS )
		return status;

	if( nSize != sizeof(HANDLE) || IsBadReadPtr(s_info, sizeof(SPORTS_PROC_INFO)) == TRUE || s_info->InteralFlag != SPORTS_FLAG )
		return STATUS_INVALID_PARAMETERS;

	try
	{
		delete s_info->serial_port;

		ZeroMemory(s_info, sizeof(SPORTS_PROC_INFO));
		free(s_info);
	}
	catch(...)
	{
		DBG_MSG("Sports_CloseSerialPort->Excetpion...\n");

	}

	return status;
}

static
KSTATUS
Sports_CloseRequest_Dispatch(
PROGRAM_OBJECT  ProgramObject,
PRP             prp,
ULONG           nMinorRequest
)
{
	KSTATUS    status = STATUS_INVALID_REQUEST;

	if( nMinorRequest == CLOSE_SERIAL_PORT )
		status = Sports_CloseSerialPort(ProgramObject, prp);

	return status;
}

static
KSTATUS
Sports_QueryRequest_Dispatch(
PROGRAM_OBJECT  ProgramObject,
PRP             prp,
ULONG           nMinorRequest
)
{
	KSTATUS    status = STATUS_INVALID_REQUEST;
	
	
	
	if( nMinorRequest == SERIAL_PORT_CFG )
	{
		ULONG                 nOutSize;
		PSERIAL_PORT_CONFIG   port_config = NULL;

		status = FMMC_GetPRPParmeters(prp, NULL, NULL, (PVOID*)&port_config, &nOutSize);
		if( status != STATUS_SUCCESS )
			return status;

		if( nOutSize != sizeof(SERIAL_PORT_CONFIG) )
			return STATUS_INVALID_PARAMETERS;

		CTHSerialPort::GetConfigInfoFromFile(port_config->ConfigPath, port_config->dcb, port_config->m_CommTimeouts,port_config->InBufferMaxSize, port_config->OutBufferMaxSize, port_config->sPortNumber);

	}
	
	return status;
}

static
KSTATUS
Sport_SetRequest_Dispatch(
PROGRAM_OBJECT  ProgramObject,
PRP             prp,
ULONG           nMinorRequest
)
{
	KSTATUS    status = STATUS_INVALID_REQUEST;

	//独立编程对象是可以不需要建立关联
	if( nMinorRequest == PRP_MIN_RELATIVE )
		status = STATUS_SUCCESS; 
	
	else if( nMinorRequest == SERIAL_PORT_CFG )
	{
		ULONG                 nInSize;
		PSERIAL_PORT_CONFIG   port_config = NULL;
		
		status = FMMC_GetPRPParmeters(prp, (PVOID*)&port_config, &nInSize, NULL, 0);
		if( status != STATUS_SUCCESS )
			return status;
		
		if( nInSize != sizeof(SERIAL_PORT_CONFIG) || port_config->ConfigPath[0] == 0 )
			return STATUS_INVALID_PARAMETERS;
		
		CTHSerialPort::SaveConfigInformationToFile(port_config->ConfigPath, port_config->dcb, port_config->m_CommTimeouts,port_config->InBufferMaxSize, port_config->OutBufferMaxSize, port_config->sPortNumber);
	}
	
	return status;

}


extern "C"
KSTATUS
WINAPI
Sports_Request_Dispatch(
PROGRAM_OBJECT   ProgramObject,
PRP              prp
)
{
	ULONG       nMajorRequest, nMinorRequest;
	KSTATUS     status;

	
	status = FMMC_GetPRPRequest(prp, (PRP_TYPE*)&nMajorRequest, &nMinorRequest);

	if( status != STATUS_SUCCESS )
		goto r_exit;

	switch(nMajorRequest)
	{
	case PRP_AC_CREATE:
		{
			status = Sports_CreateRequest_Dispatch(ProgramObject, prp, nMinorRequest);
			break;
		}
	case PRP_AC_WRITE:
		{
			status = Sports_WriteRequest_Dispatch(ProgramObject, prp, nMinorRequest);
			break;
		}
	case PRP_AC_QUERY:
		{
			status = Sports_QueryRequest_Dispatch(ProgramObject, prp, nMinorRequest);
			break;
		}
	case PRP_AC_SET:
		{
			status = Sport_SetRequest_Dispatch(ProgramObject, prp, nMinorRequest);
			break;
		}
	case PRP_AC_CLOSE:
		{
			status = Sports_CloseRequest_Dispatch(ProgramObject, prp, nMinorRequest);
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