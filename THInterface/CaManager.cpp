#include "StdAfx.h"
#include "CaManager.h"
#include "DevManager.h"
#include "DbgMsg.h"


KSTATUS CCaManager::SetCameraPrviewWindow(PCAMERA_PRVIEW_CONFIG   PrvCfg)
{
	if( PrvCfg == NULL || CDevManager::GetOIInfo().s_ca_dev == NULL )
		return STATUS_INVALID_PARAMETERS;
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_ca_dev, NULL, PRP_AC_SET, SET_PRVIEW_CAMERA, PrvCfg, sizeof(CAMERA_PRVIEW_CONFIG), NULL, 0, FALSE, FALSE);

}


KSTATUS CCaManager::CaptureCameraGrap(UCHAR  nCaIndex,PCHAR  pFilePath)
{
	USHORT                 idx = nCaIndex;
	HANDLE                 hRawBuffer = NULL;
	KSTATUS                status;
	FRAME_FILE_INFO        file_info = {0};
	
	
	if( pFilePath == NULL )
	{
		DBG_MSG("THInterface_CaptureCameraGrap->Invaild Par...\n");
		return STATUS_INVALID_PARAMETERS;
	}
	
	if( CDevManager::GetOIInfo().s_ca_dev == NULL )
		return STATUS_DEV_NOT_START;
	
	status = FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_ca_dev, NULL, PRP_AC_READ, RAW_GRAP_BUFFER, &idx, sizeof(USHORT), &hRawBuffer, sizeof(HANDLE), FALSE, FALSE);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("THInterface_CaptureCameraGrap->Read Buffer Error = %x...\n",status);
		return status;
	}
	
	file_info.hRawGrapBuffer = hRawBuffer;
	file_info.nDHIndex       = nCaIndex;
	strcpy(file_info.FrameFilePath,pFilePath);
	status = FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_ca_dev, NULL, PRP_AC_WRITE, FRAME_FILE, (PVOID)&file_info, sizeof(file_info), NULL, 0, FALSE, FALSE);
	return status;

}