#include "StdAfx.h"
#include "CADetialRequest.h"
#include "DbgMsg.h"


//**********************************************************************老版本接口定义*********************************************************

#pragma pack(1)

typedef struct _DH_CAMERA_COMMOM_HEADER
{
	USHORT   nDHIndex;
	CHAR     Reserved[32];
	
}DH_CAMERA_COMMOM_HEADER, *PDH_CAMERA_COMMOM_HEADER;

typedef struct _DH_CAMERA_CONFIG_INFO      //大恒配置参数
{
	DH_CAMERA_COMMOM_HEADER  cHeader;
	
	int    dGrapWidth;                     //采图大小,该值不建议提供出来让用户设置,它将会直接影响识别效果
	int    dGrapHeight;
	int    dGain;
	int    dTime;
	int    dGamma;
	
	
}DH_CAMERA_CONFIG_INFO, *PDH_CAMERA_CONFIG_INFO;

typedef   struct  _CA_EXPOSE_TIME_RANGE_OLD
{
	BOOLEAN   bSupportExposeTime;
	int       nMinExposeTime;
	int       nMaxExposeTime;
	
}CA_EXPOSE_TIME_RANGE_OLD, *PCA_EXPOSE_TIME_RANGE_OLD;


typedef  struct _CA_GAIN_RANGE_OLD
{
	int  nMinGain;
	int  nMaxGain;
	
}CA_GAIN_RANGE_OLD, *PCA_GAIN_RANGE_OLD;




#pragma pack()

#define  CONFIG_CONTENT_LENGTH  512
//**********************************************************************************************************************************************


extern
KSTATUS
CaCreateRequest(
PROGRAM_OBJECT                PrgObj,
PCA_EXTENSION                 pExtension,
PINIT_CAMERA_PARAMETERS       pPar
)
{
	return pExtension->cCaMgr->StartCaDev(pPar->nCaDevIndex);
}

extern
KSTATUS
CaSetPrviewRequest(
PCA_EXTENSION                 pExtension,
PCAMERA_PRVIEW_CONFIG         PrviewConfig
)
{
	return pExtension->cCaMgr->SetPrviewWindow((UCHAR)PrviewConfig->nCaIndex, PrviewConfig->cPrviewWnd, PrviewConfig->cPrviewRC);
}


extern
KSTATUS
CaSaveFrameFile(
PCA_EXTENSION                 pExtension, 
PFRAME_FILE_INFO              pFrameFileInfo
)
{
	DBG_MSG("CaSaveFrameFile.....\n");

	if( pFrameFileInfo->FrameFilePath[0] == 0 || pFrameFileInfo->hRawGrapBuffer == NULL  )
		return STATUS_INVALID_PARAMETERS;
	
	return pExtension->cCaMgr->CatchGrap(pFrameFileInfo->nDHIndex, pFrameFileInfo->FrameFilePath, pFrameFileInfo->hRawGrapBuffer);
}

extern
KSTATUS CaSaveFrameBitmap(PCA_EXTENSION pExtension, PFRAME_FILE_INFO pFrameFileInfo, HANDLE *pHbitmap)
{
	DBG_MSG("CaSaveFrameBitmap.....\n");

	if( pFrameFileInfo->hRawGrapBuffer == NULL || pHbitmap == NULL  )
		return STATUS_INVALID_PARAMETERS;

	return pExtension->cCaMgr->CatchBitmap(pFrameFileInfo->nDHIndex, pHbitmap, pFrameFileInfo->hRawGrapBuffer);
}


extern
KSTATUS
CaReadCameraGrapRawBuffer(
PCA_EXTENSION      pExtension, 
PUSHORT            pDHIndex,
HANDLE            *handle
)
{
	
	return  pExtension->cCaMgr->ReadRawGrapBuffer(*pDHIndex,  handle);
}

extern
KSTATUS
CaCloseCameraRequest(
PCA_EXTENSION      pExtension
)
{
	return pExtension->cCaMgr->CloseAllDevice();
}



extern
KSTATUS
CaQueryCameraSNInfo(
PCA_EXTENSION  pExtension,
UCHAR          nCaDevIndex,
PCHAR          pSnBuffer,
ULONG          nSnBufferLength
)
{
	return pExtension->cCaMgr->QueryCaSN(nCaDevIndex, pSnBuffer, nSnBufferLength);
}


static
KSTATUS
CaRequest_QueryDevNumberInfo(
PCA_EXTENSION     pExtension,
PDEVICE_CAP_DES   pCapDes
)
{
	KSTATUS    status = STATUS_INVALID_PARAMETERS;
	
	switch( pCapDes->CapDesType )
	{
	case SupDevNumber:
		{
			if( pCapDes->nOutputBufferLength != sizeof(ULONG) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}
			*((PULONG)pCapDes->pOutputBuffer) = MAX_DEV_INFO_COUNTER;
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

			*((PULONG)pCapDes->pOutputBuffer) = pExtension->cCaMgr->QueryUseageDevCounter();
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

			*((PULONG)pCapDes->pOutputBuffer) = pExtension->cCaMgr->QueryStartDevCounter();
			status = STATUS_SUCCESS;
			break;
		}
	default:
		{
			break;
		}
	}
	
	return status;
}


extern
KSTATUS
CaSetCameraDevCapRequest(
PCA_EXTENSION                 pExtension,
PDEVICE_CAP_DES               pDevCapDes
)
{
	KSTATUS        status = STATUS_INVAILD_DATA;

	if( pDevCapDes->CapDesType == DevCconfig && pDevCapDes->nInBufferLength == sizeof(CAMERA_CONFIG_INFO) )
		status = pExtension->cCaMgr->SetConfig((PCAMERA_CONFIG_INFO)pDevCapDes->pInputBuffer);
	else if( pDevCapDes->CapDesType == DevCB && pDevCapDes->nInBufferLength == sizeof(PRVIEW_OUTPUT_CB) )
		status = pExtension->cCaMgr->SetPrivewCB((PPRVIEW_OUTPUT_CB)pDevCapDes->pInputBuffer);
	else if( pDevCapDes->CapDesType == WBalance )
		status = pExtension->cCaMgr->SetWBalance(pDevCapDes->nIndex);


	return status;

}


extern
KSTATUS
CaQueryCameraDevCapRequest(
PCA_EXTENSION                 pExtension,
PDEVICE_CAP_DES               pDevCapDes
)
{
	KSTATUS        status = STATUS_INVAILD_DATA;

	switch(pDevCapDes->CapDesType)
	{
	case SupDevNumber:     //公有命令
	case UsageDevNumber:
	case StartDevNumber:
		{
			status = CaRequest_QueryDevNumberInfo(pExtension, pDevCapDes);
			break;
		}
	case DevSN:
		{
			status = CaQueryCameraSNInfo(pExtension, (UCHAR)pDevCapDes->nIndex, pDevCapDes->pOutputBuffer, pDevCapDes->nOutputBufferLength);
			break;
		}
	case DevCconfig:
		{
			int a = sizeof(CAMERA_CONFIG_INFO);
			if( pDevCapDes->nOutputBufferLength != sizeof(CAMERA_CONFIG_INFO) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}
			status = pExtension->cCaMgr->QueryConfig((PCAMERA_CONFIG_INFO)pDevCapDes->pOutputBuffer);
			break;
		}
	case CaExposeTimeRange:
		{
			if( pDevCapDes->nOutputBufferLength != sizeof(CA_EXPOSE_TIME_RANGE) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;
			}
			status = pExtension->cCaMgr->QueryETRange((PCA_EXPOSE_TIME_RANGE)pDevCapDes->pOutputBuffer);
			break;
		}
	case CaGainRange:
		{
			if( pDevCapDes->nOutputBufferLength != sizeof(CA_GAIN_RANGE) )
			{
				status = STATUS_INVALID_PARAMETERS;
				break;

			}
			status = pExtension->cCaMgr->QueryGainRange((PCA_GAIN_RANGE)pDevCapDes->pOutputBuffer);
			break;
		}
	default:
		{
			break;
		}

	}


	return status;

}

extern
KSTATUS 
CaSetCameraConfigRequest( 
PCA_EXTENSION pExtension, 
PCHAR         pCfgContent, 
ULONG         nCfgLength 
)
{
	CAMERA_CONFIG_INFO         ca_cfg  = {0};
	PDH_CAMERA_CONFIG_INFO     pDHCaCfg = (PDH_CAMERA_CONFIG_INFO)pCfgContent;
	PDH_CAMERA_COMMOM_HEADER   pHeader  = (PDH_CAMERA_COMMOM_HEADER)pCfgContent;
	
	ca_cfg.nGlobalIndex = pHeader->nDHIndex - 1;
	ca_cfg.dPrevHeight  = ca_cfg.dCatchHeight = pDHCaCfg->dGrapHeight;
	ca_cfg.dPrevWidth   = ca_cfg.dCatchWidth  = pDHCaCfg->dGrapWidth;
	ca_cfg.dGain        = pDHCaCfg->dGain;	
	ca_cfg.dGamma       = pDHCaCfg->dGamma;
	ca_cfg.dTime        = pDHCaCfg->dTime;
	
	
	return  pExtension->cCaMgr->SetConfig(&ca_cfg);
}

extern
KSTATUS 
CaQueryCameraConfigRequest( 
PCA_EXTENSION pExtension, 
PCHAR pCfgContent, 
ULONG nCfgLength 
)
{
	KSTATUS                    status;
	CAMERA_CONFIG_INFO         ca_cfg  = {0};
	PDH_CAMERA_CONFIG_INFO     pDHCaCfg = (PDH_CAMERA_CONFIG_INFO)pCfgContent;

	ca_cfg.nGlobalIndex = pDHCaCfg->cHeader.nDHIndex - 1;

	status = pExtension->cCaMgr->QueryConfig(&ca_cfg);

	if( status == STATUS_SUCCESS )
	{
		pDHCaCfg->dGain = ca_cfg.dGain;
		pDHCaCfg->dTime = ca_cfg.dTime;
		pDHCaCfg->dGamma = ca_cfg.dGamma;
		pDHCaCfg->dGrapHeight = ca_cfg.dCatchHeight;
		pDHCaCfg->dGrapWidth  = ca_cfg.dCatchWidth;
	}

	return status;

}

extern
KSTATUS 
CaQueryCameraDevAblityRequest( 
PCA_EXTENSION       pExtension, 
PQUERY_CADEV_ABLITY pCablity, 
PCHAR               pOutputBuffer, 
ULONG               nLength 
)
{
	KSTATUS                      status;
	CA_GAIN_RANGE                gain_range = {0};
	DEVICE_CAP_DES               dev_cap_des;
	CA_EXPOSE_TIME_RANGE         et_range = {0};

	ZeroMemory(&dev_cap_des, sizeof(dev_cap_des));

	dev_cap_des.nIndex = pCablity->nCaIndex - 1;
	
	if( pCablity->nQueryType == QUERY_EXPOSE_TIME )
	{
		dev_cap_des.CapDesType          = CaExposeTimeRange;
		et_range.nGlobalIndex           = dev_cap_des.nIndex;
		dev_cap_des.nOutputBufferLength = sizeof(CA_EXPOSE_TIME_RANGE);
		dev_cap_des.pOutputBuffer       = (PCHAR)&et_range;

	}
	else if( pCablity->nQueryType == QUERY_GAIN )
	{
		dev_cap_des.CapDesType          = CaGainRange;
		gain_range.nGlobalIndex         = dev_cap_des.nIndex;
		dev_cap_des.nOutputBufferLength = sizeof(CA_GAIN_RANGE);
		dev_cap_des.pOutputBuffer       = (PCHAR)&gain_range;
	}
	else
	{
		return STATUS_INVALID_PARAMETERS;
	}

	//return CaQueryCameraDevCapRequest(pExtension, &dev_cap_des);

	status = CaQueryCameraDevCapRequest(pExtension, &dev_cap_des);

	if( status == STATUS_SUCCESS )
	{
		if( dev_cap_des.CapDesType == CaExposeTimeRange )
		{
			PCA_EXPOSE_TIME_RANGE_OLD pETOld = (PCA_EXPOSE_TIME_RANGE_OLD)pOutputBuffer;

			pETOld->bSupportExposeTime = TRUE;
			pETOld->nMaxExposeTime     = et_range.nMaxExposeTime;
			pETOld->nMinExposeTime     = et_range.nMinExposeTime;

		}
		else
		{
			PCA_GAIN_RANGE_OLD  pGainOld = (PCA_GAIN_RANGE_OLD)pOutputBuffer;

			pGainOld->nMaxGain = gain_range.nMaxGain;
			pGainOld->nMinGain = gain_range.nMinGain;

		}
	}


	return status;
	
}

extern
KSTATUS 
CaQueryCameraSNInfoOld( 
PCA_EXTENSION pExtension, 
UCHAR         nCaDevIndex, 
PCHAR         pSnBuffer, 
ULONG         nSnBufferLength 
)
{
	return CaQueryCameraSNInfo(pExtension, (UCHAR)nCaDevIndex - 1, pSnBuffer, nSnBufferLength);
}
