// THRecCell.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\..\inc\THFrameDef.h"
#include "..\..\inc\THInterface.h"
#include "interal.h"
#include "DbgMsg.h"
#include <shlwapi.h>

#pragma comment(lib, "..\\..\\lib\\fmmc.lib")
#pragma comment(lib, "Shlwapi.lib")


static
KSTATUS
THRecCell_QueryRecDevCap(
PREC_CELL_EXTENSION  pExtension,
PDEVICE_CAP_DES      pCapDes
)
{
	KSTATUS    status = STATUS_INVALID_REQUEST;
	CRecHelper *rec_helper = (CRecHelper *)pExtension->pInterface;

	switch(pCapDes->CapDesType)
	{

	case SupDevNumber:
		{
			if( pCapDes->nOutputBufferLength != sizeof(ULONG) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}
			*((PULONG)pCapDes->pOutputBuffer) = SUPPORT_MAX_RECDEV_COUNTER;
			status = STATUS_SUCCESS;
			break;
		}
	case UsageDevNumber:
		{
			if( pCapDes->nOutputBufferLength != sizeof(ULONG) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}
			*((PULONG)pCapDes->pOutputBuffer) = rec_helper->GetRecCount();
			status = STATUS_SUCCESS;
			break;
		}
	case StartDevNumber:
		{

			if( pCapDes->nOutputBufferLength != sizeof(ULONG) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}

			*((PULONG)pCapDes->pOutputBuffer) = rec_helper->GetStartRecCount();
			status = STATUS_SUCCESS;
			break;
		}
	case DevTypeInfoDes:
		{
			if( pCapDes->nOutputBufferLength != sizeof(ULONG) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}

			*((PULONG)pCapDes->pOutputBuffer) = rec_helper->RecDevQueryType((UCHAR)pCapDes->nIndex);
			status = STATUS_SUCCESS;
			break;

		}
	case RecGrapFileName:
		{
			REC_GRAP_FULL_RECORD   rg = {0};

			if( pCapDes->nInBufferLength != sizeof(ULONG) || pCapDes->nOutputBufferLength == 0 )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}

			rg.g_rc.rIndex      = (USHORT)((*((PULONG)pCapDes->pInputBuffer)) >> 16);
			rg.g_rc.rGrapSize   = (UCHAR)((*((PULONG)pCapDes->pInputBuffer)) & 0xffff);

			status = rec_helper->GetGrapFileName(&rg, (PCHAR)pCapDes->pOutputBuffer);
			break;
		}
	case PicDistinct:
		{
			PPIC_DISTINCT_INFO   pInfo;

			if( pCapDes->nInBufferLength != sizeof(PIC_DISTINCT_INFO) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}

			pInfo = (PPIC_DISTINCT_INFO)pCapDes->pInputBuffer;
			status = rec_helper->QueryPicDis((UCHAR)pCapDes->nIndex, pInfo->PicPath, &pInfo->nDistinct);
			break;
		}
	default:
		{
			break;
		}
	}

	return status;
}

KSTATUS
WINAPI
THRecCell_Dispatch_Request(
PROGRAM_OBJECT   ProgramObject,
PRP              prp
)
{
	ULONG                    nMajorRequest, nMinorRequest, nInputSize, nOutputSize;
	PCHAR                    pInputBuffer, pOutputBuffer;
	KSTATUS                  status;
	CRecHelper              *rec_helper;
	PREC_CELL_EXTENSION      pExtension;


	status = FMMC_GetPRPRequest(prp, (PRP_TYPE*)&nMajorRequest, &nMinorRequest);
	if( status != STATUS_SUCCESS )
		goto r_exit;

	status = FMMC_GetPRPParmeters(prp, (PVOID*)&pInputBuffer, &nInputSize, (PVOID*)&pOutputBuffer, &nOutputSize);
	if( status != STATUS_SUCCESS )
		goto r_exit;

	FMMC_GetPrgObjectExtension(ProgramObject, (PVOID*)&pExtension);

	rec_helper = (CRecHelper *)pExtension->pInterface;

	status = STATUS_INVALID_REQUEST;

	switch(nMajorRequest)
	{
	case PRP_AC_CREATE:
		{
			if( nMinorRequest == START_REC_CELL && sizeof(REC_CELL_DESCRITOR) == nInputSize )
			{
				PREC_CELL_DESCRITOR    rd = (PREC_CELL_DESCRITOR)pInputBuffer;
				status = rec_helper->StartRecDev((UCHAR)rd->nRecDevIndex, rec_helper);

			}
			else if( nMinorRequest == BUILD_REC_RESLUT && sizeof(REC_GRAP_FULL_RECORD) == nInputSize )
			{
				PREC_GRAP_FULL_RECORD  full_rec = (PREC_GRAP_FULL_RECORD)pInputBuffer;

				status = rec_helper->RecDevBuildResult(full_rec);
			}

			break;
		}
	case PRP_AC_QUERY:
		{
			if( nMinorRequest == REC_GRAP_INFO && sizeof(PTASK_REC_INFO*) == nOutputSize && sizeof(REC_GRAP_FULL_RECORD) == nInputSize )
			{
				PREC_GRAP_FULL_RECORD  full_rec = (PREC_GRAP_FULL_RECORD)pInputBuffer;
				
				status = rec_helper->QueryTaskInfo((PREC_GRAP_FULL_RECORD)pInputBuffer, (PTASK_REC_INFO*)pOutputBuffer);

			}
			else if( nMinorRequest == REC_CELL_INFO && sizeof(REC_GRAP_FULL_RECORD) == nInputSize && sizeof(PCELL_FULL_INFO*) == nOutputSize )
			{
				PREC_GRAP_FULL_RECORD  full_rec = (PREC_GRAP_FULL_RECORD)pInputBuffer;
				
				status = rec_helper->QueryCellInfo((PREC_GRAP_FULL_RECORD)pInputBuffer, (PCELL_FULL_INFO*)pOutputBuffer);
			}

			else if( nMinorRequest == QUERY_DEVICE_CAP && sizeof(DEVICE_CAP_DES) == nInputSize )
				status = THRecCell_QueryRecDevCap(pExtension, (PDEVICE_CAP_DES)pInputBuffer);

			break;

		}
	case PRP_AC_SET:
		{
			if( nMinorRequest == PRP_MIN_RELATIVE )
				status = FMMC_SetPassivityRelation(ProgramObject);
			else if( nMinorRequest == MODIFY_CELL_INFO && sizeof(CHANGE_MODIFY_INFO) == nInputSize )
			{
				PCHANGE_MODIFY_INFO  modify_info = (PCHANGE_MODIFY_INFO)pInputBuffer;

				status = rec_helper->ModifyCellInfoEntry((PCHANGE_MODIFY_INFO)pInputBuffer);
			}
			break;
		}
	case PRP_AC_DEVIO:
		{
			if( nMinorRequest == DEL_REC_EX_REQUEST && nInputSize == sizeof(DEL_REC_EX_INFO) )
			{
				PDEL_REC_EX_INFO  del_rec = (PDEL_REC_EX_INFO)pInputBuffer;

				status = rec_helper->DelRecGrap((PDEL_REC_EX_INFO)pInputBuffer);

			}
			else if( nMinorRequest == ADD_REC_GRAP_REQUEST && nInputSize == sizeof(CHANGE_MODIFY_INFO) )
			{
				PCHANGE_MODIFY_INFO  modify_info = (PCHANGE_MODIFY_INFO)pInputBuffer;
				
				status = rec_helper->AddRecCellGrap((PCHANGE_MODIFY_INFO)pInputBuffer);
				
			}
			break;
		}
	case PRP_AC_CLOSE:
		{
			if( nMinorRequest == CLOSE_CELL_REQUEST )
			{
				delete rec_helper;
				ZeroMemory(pExtension,sizeof(REC_CELL_EXTENSION));
			}
				
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
		status = FMMC_SetDispatch(ModObject, THRecCell_Dispatch_Request, (PRP_TYPE)type);
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
		                          TH_REC_CELL1_NAME,
		                          PRG_OBJECT_DEVICE,
		                          TH_REC_CELL1_DESCRITOR,
		                          sizeof(REC_CELL_EXTENSION),
		                          &object);
	
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("THRECCELL->AddProgramObjectDispatch...Fail status = %x\n", status);
	}
	else
	{
		PREC_CELL_EXTENSION  pExtension;

		FMMC_GetPrgObjectExtension(object, (PVOID*)&pExtension);
		ZeroMemory(pExtension, sizeof(REC_CELL_EXTENSION));

		pExtension->pInterface = new CRecHelper;
		if( pExtension->pInterface == NULL )
		{
			status = STATUS_ALLOC_MEMORY_ERROR;
			DBG_MSG("new  rec_helper err..\n");
		}
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

