#include "StdAfx.h"
#include "MsgManager.h"
#include "DevManager.h"

KSTATUS CMsgManager::SetUserLevelInforamtion(PVOID  pContext,PCHAR  pBuffer,ULONG  nBufferLength)
{
	TH_STATUS_INFORMATION  t_info;

	if( CDevManager::GetOIInfo().s_thmgr_dev == NULL )
		return STATUS_DEV_NOT_START;

	ZeroMemory(&t_info, sizeof(t_info));
	
	t_info.i_type = UserInformationType;
	t_info.pContext = pContext;
	t_info.content.buffer = pBuffer;
	t_info.content.length = nBufferLength;
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_thmgr_dev, NULL, PRP_AC_WRITE, MMGR_MSG_REQUEST, &t_info, sizeof(t_info), NULL, 0, FALSE, FALSE);
}