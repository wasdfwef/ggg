#include "StdAfx.h"
#include "DevManager.h"
#include "DbgMsg.h"


static   OPEN_DEV_INFORMATION     oi_dev = {0};
static   PROGRAM_OBJECT           m_THBillPrg = NULL;

KSTATUS  CDevManager::QuerySystemDev(PSYSTEM_DEVICE_SUPPORT  *sys_dev,PULONG                   pCounter,	BOOLEAN                  bQueryStartDev)
{
	ULONG                    nIndex = 0, nVaildCaCounter;
	KSTATUS                  status;
	PROGRAM_OBJECT           t_obj, *ObjArray;
	TDEVICE_DESCRITOR        dev_des;
	PSYSTEM_DEVICE_SUPPORT   t_dev;
	
	if(sys_dev == NULL || pCounter == NULL || IsBadWritePtr(sys_dev, sizeof(ULONG)) == TRUE || IsBadWritePtr(pCounter, sizeof(ULONG)) == TRUE )
		return STATUS_INVALID_PARAMETERS;
	
	if( bQueryStartDev == FALSE )
	{
		status = FMMC_GetPrgObjectByFlag(&t_obj, pCounter, PRG_OBJECT_DEVICE);
		if( status != STATUS_SUCCESS )
			return status;
		
		ObjArray = (PROGRAM_OBJECT*)t_obj;
		t_dev    = (PSYSTEM_DEVICE_SUPPORT)FMMC_CreateBuffer((*pCounter) * sizeof(SYSTEM_DEVICE_SUPPORT));
		if( t_dev )
		{
			ZeroMemory(t_dev, (*pCounter) * sizeof(SYSTEM_DEVICE_SUPPORT));
			for( ULONG j = 0 ; j < (*pCounter) ; j ++ )
			{
				dev_des = DeviceDes2MainTypeDes(ObjArray[j]);
				if( dev_des == T_DEV_UNKNOW_TYPE )
					continue;

				if( dev_des == T_DEV_CAMERA_TYPE )
				{
					status = THInterface_QueryDevCaps(T_DEV_CAMERA_TYPE, UsageDevNumber, NULL, 0, (PCHAR)&nVaildCaCounter, sizeof(ULONG), 0);
					if( status != STATUS_SUCCESS )
						continue;
					t_dev[nIndex].ca_start.nCounter = (UCHAR)nVaildCaCounter;
				}
				else if( dev_des == T_DEV_REC_TYPE )
				{
					status = THInterface_QueryDevCaps(T_DEV_REC_TYPE, UsageDevNumber, NULL, 0, (PCHAR)&nVaildCaCounter, sizeof(ULONG), 0);
					if( status != STATUS_SUCCESS )
						continue;

					t_dev[nIndex].rec_start.nCounter = (UCHAR)nVaildCaCounter;
				}
				
				FMMC_QueryPrgObjectName(ObjArray[j], t_dev[nIndex].DeviceName);
				t_dev[nIndex].dev_des = dev_des;
				nIndex++;
			}
			if( nIndex )
			{
				*pCounter = nIndex;
				*sys_dev  = t_dev;
			}
			else
			{
				*pCounter = 0;
				status = STATUS_NOT_FOUND_VALUE;
				FMMC_FreeBuffer(t_dev);
			}
		}
		else
			status = STATUS_ALLOC_MEMORY_ERROR;
		
		FMMC_FreeBuffer(t_obj);
	}
	else
	{
		ObjArray = (PROGRAM_OBJECT *)&oi_dev;
		
		t_dev = (PSYSTEM_DEVICE_SUPPORT)FMMC_CreateBuffer(6 * sizeof(SYSTEM_DEVICE_SUPPORT));
		if( t_dev == NULL )
			return STATUS_ALLOC_MEMORY_ERROR;
		
		*pCounter = 0;
		ZeroMemory(t_dev, 6 *sizeof(SYSTEM_DEVICE_SUPPORT));
		
		for( ULONG j = 0 ; j < 6 ; j ++ )
		{
			if( ObjArray[j] == NULL )
				continue;
			
			dev_des = DeviceDes2MainTypeDes(ObjArray[j]);
			if( dev_des == T_DEV_UNKNOW_TYPE )
				continue;
			
			t_dev[*pCounter].dev_des = dev_des;
			
			FMMC_QueryPrgObjectName(ObjArray[j], t_dev[*pCounter].DeviceName);
			(*pCounter)++;
		}
		
		if( *pCounter == 0 )
		{
			FMMC_FreeBuffer(t_dev);
			status = STATUS_NOT_FOUND_VALUE;
		}
		else
		{
			*sys_dev = t_dev;
			status = STATUS_SUCCESS;
		}
	}
	return status;
}


KSTATUS  CDevManager::FindBillPrgObject(PROGRAM_OBJECT   &tBillPrgObj,ULONG &nBillCounter)
{
	ULONG                    nCounter, nDes;
	KSTATUS                  status;
	PROGRAM_OBJECT           t_obj, *ObjArray, *pBillObjArray;

	status = FMMC_GetPrgObjectByFlag(&t_obj, &nCounter, PRG_OBJECT_NORMAL);
	if( status != STATUS_SUCCESS )
		return status;

	pBillObjArray = (PROGRAM_OBJECT *)FMMC_CreateBuffer(sizeof(PROGRAM_OBJECT) * nCounter);
	if( pBillObjArray == NULL )
	{
		FMMC_FreeBuffer(t_obj);
		return STATUS_ALLOC_MEMORY_ERROR;
	}

	ObjArray = (PROGRAM_OBJECT*)t_obj;
	nBillCounter = 0;

	for( ULONG j = 0 ; j < nCounter ; j ++ )
	{
		status = FMMC_GetPrgObjDescritor(ObjArray[j], &nDes);

		if( status != STATUS_SUCCESS )
			continue;

		if( ((nDes >> 24) == TH_MAIN_CP_TYPE) && (((nDes >> 8) & 0xff) == OBJECT_LEVEL_LOGIC) )
			pBillObjArray[nBillCounter++] = ObjArray[j];
	}

	FMMC_FreeBuffer(t_obj);

	if( nBillCounter == 0 )
	{
		FMMC_FreeBuffer(pBillObjArray);
		return STATUS_NOT_FOUND_OBJECT;
	}

	tBillPrgObj = (PROGRAM_OBJECT)pBillObjArray;
	return STATUS_SUCCESS;

}


KSTATUS  CDevManager::StartCameraDevice(PROGRAM_OBJECT  caObj, PDECIDE_START_DEVICE  pCADev)
{
	INIT_CAMERA_PARAMETERS    init = {0};
	
	if( pCADev->dev_sup.ca_start.nCounter == 0 )
		return STATUS_INVALID_PARAMETERS;

	
	for( UCHAR j = 0 ; j < pCADev->dev_sup.ca_start.nCounter ; j ++ )
	{
		init.nCaDevIndex = pCADev->dev_sup.ca_start.nCaDevIndex[j];
		FMMC_EasySynchronousBuildPRP(caObj, NULL, PRP_AC_CREATE, OPEN_CAMERA, (PVOID)&init, sizeof(init), NULL, 0, FALSE, FALSE); //无需关心结果
		
	}
	return STATUS_SUCCESS;
}

KSTATUS CDevManager::StartRECDevice(PROGRAM_OBJECT recObj, PDECIDE_START_DEVICE  pCADev)
{
	REC_CELL_DESCRITOR     rd = {0};

	if( pCADev->dev_sup.rec_start.nCounter == 0 )
		return STATUS_INVALID_PARAMETERS;


	for( UCHAR j = 0 ; j < pCADev->dev_sup.rec_start.nCounter ; j ++ )
	{
		rd.nRecDevIndex = j;
		FMMC_EasySynchronousBuildPRP(recObj, NULL, PRP_AC_CREATE, START_REC_CELL, (PVOID)&rd, sizeof(rd), NULL, 0, FALSE, FALSE);
	}
	
	return STATUS_SUCCESS;
}


KSTATUS  CDevManager::StartDevForDebug(PDECIDE_START_DEVICE   pStartDev,ULONG     nCounter)
{
	KSTATUS                    status;
	PROGRAM_OBJECT             dev_obj;
	INIT_CAMERA_PARAMETERS     init_ca = {0};

	for( ULONG j = 0 ; j < nCounter ; j ++ )
	{
		if( pStartDev[j].dev_sup.dev_des == T_DEV_CAMERA_TYPE || pStartDev[j].dev_sup.dev_des == T_DEV_REC_TYPE )
		{
			status = FMMC_GetPrgObjectByName(pStartDev[j].dev_sup.DeviceName, &dev_obj);
			if( status != STATUS_SUCCESS )
				continue;

			if( pStartDev[j].dev_sup.dev_des == T_DEV_CAMERA_TYPE )
			{
				if( StartCameraDevice(dev_obj, &pStartDev[j]) == STATUS_SUCCESS )
					oi_dev.s_ca_dev = dev_obj;
			}
			else if( pStartDev[j].dev_sup.dev_des == T_DEV_REC_TYPE )
			{
				if( StartRECDevice(dev_obj, &pStartDev[j]) == STATUS_SUCCESS )
					oi_dev.s_rec_dev = dev_obj;
			}
		}

	}

	return STATUS_SUCCESS;
}

KSTATUS  CDevManager::StartDev()
{
	ULONG                nBillCounter;
	KSTATUS              status;
	PROGRAM_OBJECT       tBillPrg, *tBillArrayObj;
	
	if( m_THBillPrg )
		return STATUS_INVALID_PARAMETERS;

/*	status = FMMC_GetPrgObjectByName(pStartDev[lc_index].dev_sup.DeviceName, &lc_dev_obj);
	if( status != STATUS_SUCCESS )
		return status;
	
	status = FMMC_GetPrevLevelPrgObject(lc_dev_obj, NULL, &m_THBillPrg);
	if( status != STATUS_SUCCESS )
	{
		m_THBillPrg = NULL;
		return status;
	}*/

	status = FindBillPrgObject(tBillPrg, nBillCounter);
	if( status != STATUS_SUCCESS )
		return status;

	tBillArrayObj = (PROGRAM_OBJECT *)tBillPrg;

	for( ULONG j = 0 ; j < nBillCounter ; j  ++ )
	{
		m_THBillPrg = tBillArrayObj[j];
		status = FMMC_EasySynchronousBuildPRP(m_THBillPrg, NULL, PRP_AC_CREATE, START_RELATIVE_PRG, NULL, 0, NULL, 0, FALSE, FALSE);
		if( status == STATUS_SUCCESS )
		{
			status = FMMC_EasySynchronousBuildPRP(m_THBillPrg, NULL, PRP_AC_QUERY, OPEN_DEV_REQUEST, NULL, 0, &oi_dev, sizeof(OPEN_DEV_INFORMATION),FALSE, FALSE );
			DBG_MSG("THInterface_StartDev oi_dev.LC  = %#x status = %x\n", oi_dev.s_lc_dev, status);
			DBG_MSG("THInterface_StartDev oi_dev.LIS = %#x status = %x\n", oi_dev.s_lis_dev, status);
			DBG_MSG("THInterface_StartDev oi_dev.Cam = %#x status = %x\n", oi_dev.s_ca_dev, status);
			DBG_MSG("THInterface_StartDev oi_dev.Rec = %#x status = %x\n", oi_dev.s_rec_dev, status);
			DBG_MSG("THInterface_StartDev oi_dev.UDC = %#x status = %x\n", oi_dev.s_udc_dev, status);
			DBG_MSG("THInterface_StartDev oi_dev.Msg = %#x status = %x\n", oi_dev.s_thmgr_dev, status);
			DBG_MSG("THInterface_StartDev oi_dev.Bar = %#x status = %x\n", oi_dev.s_barcode_dev, status);
			break;
		}
		else
			m_THBillPrg = NULL;
	}
	
	return status;
}

KSTATUS  CDevManager::CloseDev()
{
	KSTATUS status = STATUS_INVALID_REQUEST;
	
	if( m_THBillPrg )
	{
		FMMC_EasySynchronousBuildPRP(m_THBillPrg, NULL, PRP_AC_CLOSE, CLOSE_DEV_REQUEST, NULL, 0, NULL, 0, FALSE, FALSE);
		m_THBillPrg = NULL;
		ZeroMemory(&oi_dev,sizeof(oi_dev));
	}
	
	return status;

}


TDEVICE_DESCRITOR   CDevManager::DeviceDes2MainTypeDes(PROGRAM_OBJECT  PrgObj)
{
	ULONG    Descritor;
	
	if( FMMC_GetPrgObjDescritor(PrgObj, &Descritor) != STATUS_SUCCESS )
		return T_DEV_UNKNOW_TYPE;
	
	if( (Descritor >> 24) == TH_MAIN_CA_TYPE )
		return T_DEV_CAMERA_TYPE;
	else if( (Descritor >> 24) == TH_MAIN_BDB_TYPE )
		return T_DEV_DB_TYPE;
	else if( (Descritor >> 24) == TH_MAIN_LC_TYPE )
		return T_DEV_LC_TYPE;
	else if( (Descritor >> 24) == TH_MAIN_REC_TYPE )
		return T_DEV_REC_TYPE;
	else if( (Descritor >> 24) == TH_MAIN_UDC_I_TYPE )
		return T_DEV_UDC_TYPE;
	else if( (Descritor >> 24) == TH_MAIN_LIS_TYPE )
		return T_DEV_LIS_TYPE;
	else if( (Descritor >> 24) == TH_MAIN_MSG_TYPE )
		return T_DEV_MSG_TYPE;
	else if( (Descritor >> 24) == TH_MAIN_BARCODE_TYPE )
		return T_DEV_BARCODE_TYPE;
	
	return T_DEV_UNKNOW_TYPE;
}

PROGRAM_OBJECT CDevManager::GetBillPrgObject()
{
	return m_THBillPrg;
}


OPEN_DEV_INFORMATION& CDevManager::GetOIInfo()
{
	return oi_dev;
}


KSTATUS CDevManager::QueryDevCap(TDEVICE_DESCRITOR       dev_type, DevCapsInfoType info_type, PCHAR           pInputBuffer, ULONG           nInBufLength, PCHAR           pOutputBuffer, ULONG           nOutBufLength, USHORT nDevIndex)
{
	KSTATUS            status;
	PROGRAM_OBJECT     PrgObj;
	DEVICE_CAP_DES     dev_cap;

	if( dev_type < T_DEV_LC_TYPE  )
		return STATUS_INVALID_PARAMETERS;

	if( (pInputBuffer && nInBufLength == 0) || (pInputBuffer == NULL && nInBufLength) )
		return STATUS_INVALID_PARAMETERS;

	if( (pOutputBuffer && nOutBufLength == 0) || (pOutputBuffer == NULL && nOutBufLength) )
		return STATUS_INVALID_PARAMETERS;

	if( pInputBuffer && IsBadReadPtr(pInputBuffer, nInBufLength) == TRUE )
		return STATUS_INVALID_PARAMETERS;

	if( pOutputBuffer && IsBadWritePtr(pOutputBuffer, nOutBufLength) == TRUE )
		return STATUS_INVALID_PARAMETERS;

	status = GetSpecialDevTypeObject(dev_type, PrgObj);
	if( status != STATUS_SUCCESS )
		return status;

	ZeroMemory(&dev_cap, sizeof(dev_cap));

	dev_cap.CapDesType          = info_type;
	dev_cap.nInBufferLength     = nInBufLength;
	dev_cap.pInputBuffer        = pInputBuffer;
	dev_cap.nIndex              = nDevIndex;
	dev_cap.pOutputBuffer       = pOutputBuffer;
	dev_cap.nOutputBufferLength = nOutBufLength;

	return FMMC_EasySynchronousBuildPRP(PrgObj, NULL, PRP_AC_QUERY, QUERY_DEVICE_CAP, &dev_cap, sizeof(DEVICE_CAP_DES), NULL, 0, FALSE, FALSE);
}

KSTATUS CDevManager::SetDevCap( TDEVICE_DESCRITOR dev_type,DevCapsInfoType info_type,PCHAR pInputBuffer,ULONG nInBufLength, USHORT nDevIndex )
{
	KSTATUS            status;
	PROGRAM_OBJECT     PrgObj;
	DEVICE_CAP_DES     dev_cap;
	
	if( dev_type < T_DEV_LC_TYPE  )
		return STATUS_INVALID_PARAMETERS;
	
	if( (pInputBuffer && nInBufLength == 0) || (pInputBuffer == NULL && nInBufLength) )
		return STATUS_INVALID_PARAMETERS;
	
	if( pInputBuffer && IsBadReadPtr(pInputBuffer, nInBufLength) == TRUE )
		return STATUS_INVALID_PARAMETERS;
	
	
	status = GetSpecialDevTypeObject(dev_type, PrgObj);
	if( status != STATUS_SUCCESS )
		return status;
	
	ZeroMemory(&dev_cap, sizeof(dev_cap));
	
	dev_cap.CapDesType          = info_type;
	dev_cap.nInBufferLength     = nInBufLength;
	dev_cap.pInputBuffer        = pInputBuffer;
	dev_cap.nIndex              = nDevIndex;
	
	return FMMC_EasySynchronousBuildPRP(PrgObj, NULL, PRP_AC_SET, QUERY_DEVICE_CAP, &dev_cap, sizeof(DEVICE_CAP_DES), NULL, 0, FALSE, FALSE);
}



KSTATUS  CDevManager::GetSpecialDevTypeObject(TDEVICE_DESCRITOR  dev_type, PROGRAM_OBJECT  &PrgObj)
{
	ULONG                    nDevCounter;
	KSTATUS                  status;
	PROGRAM_OBJECT           t_obj, *ObjArray;

	if( dev_type == T_DEV_CAMERA_TYPE && oi_dev.s_ca_dev )
	{
		PrgObj = oi_dev.s_ca_dev;
		return STATUS_SUCCESS;
	}

	else if( dev_type == T_DEV_LC_TYPE && oi_dev.s_lc_dev )
	{
		PrgObj = oi_dev.s_lc_dev;
		return STATUS_SUCCESS;
	}

	else if( dev_type == T_DEV_DB_TYPE )
		return STATUS_INVALID_OBJECT;

	else if( dev_type == T_DEV_REC_TYPE && oi_dev.s_rec_dev )
	{
		PrgObj = oi_dev.s_rec_dev;
		return STATUS_SUCCESS;
	}

	else if( dev_type == T_DEV_LIS_TYPE && oi_dev.s_lis_dev )
	{
		PrgObj = oi_dev.s_lis_dev;
		return STATUS_SUCCESS;
	}

	else if( dev_type == T_DEV_UDC_TYPE && oi_dev.s_udc_dev )
	{
		PrgObj = oi_dev.s_udc_dev;
		return STATUS_SUCCESS;
	}

	else if( dev_type == T_DEV_BARCODE_TYPE && oi_dev.s_barcode_dev )
	{
		PrgObj = oi_dev.s_barcode_dev;
		return STATUS_SUCCESS;
	}

	status = FMMC_GetPrgObjectByFlag(&t_obj, &nDevCounter, PRG_OBJECT_DEVICE);
	if( status != STATUS_SUCCESS )
		return status;
	
	ObjArray = (PROGRAM_OBJECT*)t_obj;
	status   = STATUS_NOT_FOUND_OBJECT;

	for( ULONG j = 0 ; j < nDevCounter ; j ++ )
	{
		if( DeviceDes2MainTypeDes(ObjArray[j]) == dev_type )
		{
			status = STATUS_SUCCESS;
			PrgObj = ObjArray[j];
			break;
		}
	}

	FMMC_FreeBuffer(t_obj);
	return status;

}

