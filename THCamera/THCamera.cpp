// THCamera.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "CADetialRequest.h"
#include "DbgMsg.h"
#include "..\..\inc\THCameraDef.h"

//#pragma comment(lib, "..\\..\\CommonAPI\\lib\\fmmc.lib")
#pragma comment(lib, "..\\..\\lib\\fmmc.lib")
#pragma comment(lib, "shlwapi.lib")

static
KSTATUS
WINAPI
CaRequest_Dispatch(
PROGRAM_OBJECT   ProgramObject,
PRP              prp
)
{
	ULONG            nMajor, nMinor, nInSize, nOutSize;
	PCHAR            pInputBuffer, pOutputBuffer;
	KSTATUS          status;
	PCA_EXTENSION    pExtension;

	status = FMMC_GetPrgObjectExtension(ProgramObject, (PVOID*)&pExtension);
	if( status != STATUS_SUCCESS )
		goto exit_request;
    
	if( pExtension == NULL )
	{
		DBG_MSG("FMMC_GetPrgObjectExtension  STATUS_INVALID_REQUEST .....\n");
		status = STATUS_INVALID_REQUEST;
		goto exit_request;
	}

	status = FMMC_GetPRPRequest(prp, (PRP_TYPE*)&nMajor, &nMinor);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("FCaRequest_Dispatch MMC_GetPRPRequest  STATUS %#x\n", status);
		goto exit_request;
	}

	status = FMMC_GetPRPParmeters(prp, (PVOID*)&pInputBuffer, &nInSize, (PVOID*)&pOutputBuffer, &nOutSize);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("FCaRequest_Dispatch FMMC_GetPRPParmeters  STATUS %#x\n", status);
		goto exit_request;
	}

	status = STATUS_INVALID_REQUEST;

	switch(nMajor)
	{
	case PRP_AC_CREATE:
		{
			if( nMinor == OPEN_CAMERA )
			{
				if( nInSize == sizeof(INIT_CAMERA_PARAMETERS) )
					status = CaCreateRequest(ProgramObject, pExtension, (PINIT_CAMERA_PARAMETERS)pInputBuffer);
			}
			break;
		}
	case PRP_AC_SET:
		{
			if( nMinor == SET_PRVIEW_CAMERA )
			{
				if( nInSize == sizeof(CAMERA_PRVIEW_CONFIG) )
					status = CaSetPrviewRequest(pExtension, (PCAMERA_PRVIEW_CONFIG)pInputBuffer);
			}
			else if( nMinor == QUERY_DEVICE_CAP && nInSize == sizeof(DEVICE_CAP_DES) )
				status = CaSetCameraDevCapRequest(pExtension, (PDEVICE_CAP_DES)pInputBuffer);
			else if( nMinor == CAMERA_CONFIG && nInSize == CONFIG_CONTENT_LENGTH )      //支持老版本
				status = CaSetCameraConfigRequest(pExtension, pInputBuffer, nInSize);

			else if( nMinor == PRP_MIN_RELATIVE )
				status = FMMC_SetPassivityRelation(ProgramObject);
			break;
		}
	case PRP_AC_QUERY:
		{
			if( nMinor == QUERY_DEVICE_CAP && nInSize == sizeof(DEVICE_CAP_DES) ) //公有的命令,摄像头必须支持
				status = CaQueryCameraDevCapRequest(pExtension, (PDEVICE_CAP_DES)pInputBuffer);

			else if( nMinor == CAMERA_CONFIG && nOutSize == CONFIG_CONTENT_LENGTH ) //支持老版本
				status = CaQueryCameraConfigRequest(pExtension, pOutputBuffer, nOutSize);

			else if( nMinor == CAMERA_DEV_ABLITY && nInSize == sizeof(QUERY_CADEV_ABLITY) ) //支持老版本
				status = CaQueryCameraDevAblityRequest(pExtension, (PQUERY_CADEV_ABLITY)pInputBuffer, pOutputBuffer, nOutSize);

			else if( nMinor == CAMERA_DEV_SN_REQUEST && nInSize == sizeof(PUCHAR) && nOutSize ) //支持老版本
			{
				PUCHAR pDHIndex = (PUCHAR)pInputBuffer;
				status = CaQueryCameraSNInfoOld(pExtension, *pDHIndex, pOutputBuffer, nOutSize);
			}



			break;
		}
	case PRP_AC_READ:
		{
			if( nMinor == RAW_GRAP_BUFFER )
			{
				if( nOutSize == sizeof(HANDLE) && nInSize == sizeof(USHORT) )
					status = CaReadCameraGrapRawBuffer(pExtension,(PUSHORT)pInputBuffer, (HANDLE*)pOutputBuffer);
			}

			break;
		}
	case PRP_AC_WRITE:
		{
			if( nMinor == FRAME_FILE )
			{
				if( nInSize == sizeof(FRAME_FILE_INFO) )
					status = CaSaveFrameFile(pExtension, (PFRAME_FILE_INFO)pInputBuffer);
			}
			else if (nMinor == FRAME_BITMAP)
			{
				if( nInSize == sizeof(FRAME_FILE_INFO) )
					status = CaSaveFrameBitmap(pExtension, (PFRAME_FILE_INFO)pInputBuffer, (HANDLE*)pOutputBuffer);
			}

			break;
		}
	case PRP_AC_DEVIO:
		{
			if( nMinor == FREE_RAWBUF_REQUEST && nInSize == sizeof(HANDLE) )
			{
				if( pInputBuffer )
				{
					VirtualFree(pInputBuffer, 0, MEM_RELEASE);
					status = STATUS_SUCCESS;
				}
			}
			break;
		}
	case PRP_AC_CLOSE:
		{
			if( nMinor == CLOSE_CAMERA_REQUEST )
				status = CaCloseCameraRequest(pExtension);

			break;
		}
	default:
		{
			break;
		}
	}

exit_request:

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
		status = FMMC_SetDispatch(ModObject, CaRequest_Dispatch, (PRP_TYPE)type);
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
	KSTATUS           status;
	PCA_EXTENSION     ca_ext;
	PROGRAM_OBJECT    object;

	

	status = FMMC_CreatePrgObject(ModObject,
		                          CA_NAME,
		                          PRG_OBJECT_DEVICE,
		                          CA_DESCRITOR,
		                          sizeof(CA_EXTENSION),
		                          &object);

	if( status != STATUS_SUCCESS )
		return status;

	
	status = FMMC_GetPrgObjectExtension(object, (PVOID*)&ca_ext);
	if( status != STATUS_SUCCESS )
		return status;

	ca_ext->cCaMgr = new CCameraMgr;
	if( ca_ext->cCaMgr == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;
	
	return status;
}


BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
    return TRUE;
}