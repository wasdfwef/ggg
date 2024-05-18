// THInterface.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\inc\THFrameDef.h"
#include "RecManager.h"
#include "DevManager.h"
#include "CaManager.h"
#include "LCAction.h"
#include "StatusManager.h"
#include "MsgManager.h"
#include <SHLWAPI.H>
#include "DbgMsg.h"

#pragma comment(lib, "..\\lib\\fmmc.lib")
#pragma comment(lib, "shlwapi.lib")

extern "C"
KSTATUS
WINAPI
THInterface_Initialize(
)
{
	return FMMC_Initialize();
}

extern "C"
KSTATUS
WINAPI
THInterface_QuerySystemDev(
PSYSTEM_DEVICE_SUPPORT  *sys_dev,
PULONG                   pCounter,
BOOLEAN                  bQueryStartDev
)
{
	return CDevManager::QuerySystemDev(sys_dev, pCounter, bQueryStartDev);
}

extern "C"
void
WINAPI
THInterface_FreeSysBuffer(
PVOID  pSysBuffer
)
{
	if( pSysBuffer )
		FMMC_FreeBuffer(pSysBuffer);
}

#define DES_MASK_CODE 0XF

extern "C"
KSTATUS
WINAPI
THInterface_StartDev(
)
{

	return CDevManager::StartDev();
	
}

extern "C"
KSTATUS
WINAPI
THInterface_StartDevForDebug(
PDECIDE_START_DEVICE   pStartDev,
ULONG                  nCounter
)
{
	return CDevManager::StartDevForDebug(pStartDev, nCounter);
}

extern "C"
KSTATUS
WINAPI
THInterface_SetCameraPrviewWindow(
PCAMERA_PRVIEW_CONFIG   PrvCfg
)
{
	return CCaManager::SetCameraPrviewWindow(PrvCfg);
}

extern "C"
KSTATUS
WINAPI
THInterface_CaptureCameraGrap(
UCHAR  nCaIndex,
PCHAR  pFilePath
)
{
	return CCaManager::CaptureCameraGrap(nCaIndex, pFilePath);
}


extern "C"
KSTATUS
WINAPI
THInterface_QueryRecGrapInfo(
PVOID           pCodePath,
PTASK_REC_INFO *pRecInfo   //由系统自动分配buffer, 使用完后调用系统THInterface_FreeSysBuffer
)
{
	return CRecManager::QueryRecGrapInfo(pCodePath, pRecInfo);

}

extern "C"
KSTATUS
WINAPI
THInterface_QueryRecCellInfo(
PVOID              pCodePath,
USHORT             nGrapIndex,
UCHAR             nGrapSize,
PCELL_FULL_INFO   *pRecCellInfo      //由系统自动分配buffer, 使用完后调用系统THInterface_FreeSysBuffer
)
{
	return CRecManager::QueryRecCellInfo(pCodePath, nGrapIndex, nGrapSize, pRecCellInfo);

}

extern "C"
KSTATUS
WINAPI
THInterface_ModifyRecCellInfo(
PVOID              pCodePath,
USHORT             nGrapIndex,
UCHAR              nGrapSize,
PCELL_DETAIL_INFO  pCellDetailInfo   
)
{
	return CRecManager::ModifyRecCellInfo(pCodePath, nGrapIndex, nGrapSize, pCellDetailInfo);
}



extern "C"
KSTATUS
WINAPI
THInterface_QuerySystemWorkStatus(
PQC_SETTING   pTaskQC,         //可选项,如果设置为NULL,则表示不关心如一状态
PBOOL      pProgressing
)
{
	return CStatusManager::QuerySystemWorkStatus(pTaskQC, pProgressing);

}

extern "C"
KSTATUS
WINAPI
THInterface_SetSystemWorkStatus(
PQC_SETTING   pTaskQC,
PBOOL      pTaskTest,   //上面两项同时只能使用一项,不能同时存在
ULONG      nSamplingNumber, //只要该值不为0,则会设置采图数量
ULONG      nCancelFlag  //只要该值不为0,则会设置取消状态
)
{
	return  CStatusManager::SetSystemWorkStatus(pTaskQC, pTaskTest, nSamplingNumber, nCancelFlag);
}

extern "C"
KSTATUS
WINAPI
THInterface_SetSystemWorkStatusEx(
PSYS_WORK_STATUS   pSysWorkStatus
)
{
	return CStatusManager::SetSystemWorkStatusEx(pSysWorkStatus);
}

extern "C"
KSTATUS
WINAPI
THInterface_GetSystemInformation(
PTH_STATUS_INFORMATION      status_info,
HANDLE                      hExitEvent
)
{
	return CStatusManager::GetSystemInformation(status_info, hExitEvent);
}


extern "C"
KSTATUS
WINAPI
THInterface_DeleteRecCellInfo(
PVOID              pCodePath,
PREC_GRAP_RECORD   pDelRecGrapRecord, //指定需要删除图片的索引
PCELL_IDENTIFY     pCellIdentify,     //指定需要删除图片中的某个被识别的细胞.该值的有效性受下面参数的影响,如果为FALSE才有效
BOOL               bDelAllRecInfo    //是否删除指定图片,TRUE=删除图片,FALSE=删除图片中的某个细胞
)
{
	return CRecManager::DeleteRecCellInfo(pCodePath, pDelRecGrapRecord, pCellIdentify, bDelAllRecInfo);
}

extern "C"
KSTATUS
WINAPI
THInterface_AddRecCellInfo(
PVOID    pCodePath,
USHORT   nGrapIndex,
UCHAR    nGrapSize,
PCELL_DETAIL_INFO  pCellDetailInfo
)
{
	return CRecManager::AddRecCellInfo(pCodePath, nGrapIndex,nGrapSize, pCellDetailInfo);

}



extern "C"
KSTATUS
WINAPI
THInterface_CloseDev(
)
{
	return CDevManager::CloseDev();
}


extern "C"
KSTATUS
WINAPI
THInterface_AutoDetectLCConfig(
USHORT              nPortIndex,
TDEVICE_DESCRITOR   dev_type
)
{
	return CLCAction::AutoDetectLCConfig(nPortIndex, dev_type);

}

extern "C"
KSTATUS
WINAPI
THInterface_SendlCCmdActionEx(
ULONG       ActionCmd,
PCHAR       pInBuffer,
ULONG       nInLength,
PCHAR       pOutBuffer,
ULONG       nOutLength,
TDEVICE_DESCRITOR   dev_type
)
{
	return CLCAction::SendlCCmdActionEx(ActionCmd, pInBuffer, nInLength, pOutBuffer, nOutLength, dev_type);
}




extern "C"
KSTATUS
WINAPI
THInterface_SetUserLevelInforamtion(
PVOID  pContext,
PCHAR  pBuffer,
ULONG  nBufferLength
)
{
	return  CMsgManager::SetUserLevelInforamtion(pContext, pBuffer,nBufferLength);

}


extern "C"
KSTATUS
WINAPI
THInterface_QueryDevCaps(
TDEVICE_DESCRITOR    dev_type,
DevCapsInfoType      info_type,
PCHAR                pInputBuffer,
ULONG                nInBufLength,
PCHAR                pOutputBuffer,
ULONG                nOutBufLength,
USHORT               nDevIndex
)
{
	return CDevManager::QueryDevCap(dev_type, info_type, pInputBuffer, nInBufLength, pOutputBuffer, nOutBufLength, nDevIndex);
}


extern "C"
KSTATUS
WINAPI
THInterface_SetDevCaps(
TDEVICE_DESCRITOR    dev_type,
DevCapsInfoType      info_type,
PCHAR                pInputBuffer,
ULONG                nInBufLength,
USHORT               nDevIndex
)
{
	return CDevManager::SetDevCap(dev_type, info_type, pInputBuffer, nInBufLength, nDevIndex);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
)
{
    return TRUE;
}

