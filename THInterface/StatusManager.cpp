#include "StdAfx.h"
#include "StatusManager.h"
#include "DevManager.h"

KSTATUS CStatusManager::QuerySystemWorkStatus(PQC_SETTING   pTaskQC,PBOOL      pProgressing)
{
	if( CDevManager::GetBillPrgObject() == NULL || (pTaskQC == NULL && pProgressing == NULL) )
		return STATUS_INVALID_PARAMETERS;
	
	if( pTaskQC )
	{
		if( IsBadWritePtr(pTaskQC, sizeof(QC_SETTING)) == TRUE )
			return STATUS_INVALID_PARAMETERS;
		
		FMMC_EasySynchronousBuildPRP(CDevManager::GetBillPrgObject(), NULL, PRP_AC_QUERY, TASK_QC_REQUEST, NULL, 0, pTaskQC, sizeof(QC_SETTING), FALSE, FALSE);
	}
	
	if( pProgressing )
	{
		if( IsBadWritePtr(pProgressing, sizeof(ULONG)) == TRUE )
			return STATUS_INVALID_PARAMETERS;
		
		FMMC_EasySynchronousBuildPRP(CDevManager::GetBillPrgObject(), NULL, PRP_AC_QUERY, STATUS_PROGRESSING_REQUEST, NULL, 0, pProgressing, sizeof(ULONG), FALSE, FALSE);
	}
	
	return STATUS_SUCCESS;


}


KSTATUS CStatusManager::SetSystemWorkStatus(PQC_SETTING  pTaskQC,PBOOL     pTaskTest,ULONG     nSamplingNumber,ULONG nCancelFlag)
{
	KSTATUS status;
	
	if( CDevManager::GetBillPrgObject() == NULL || (pTaskQC && pTaskTest) || (pTaskTest == NULL && pTaskQC == NULL && nSamplingNumber == 0 && nCancelFlag == 0) )
		return STATUS_INVALID_PARAMETERS;
	
	if( nCancelFlag )
		status = FMMC_EasySynchronousBuildPRP(CDevManager::GetBillPrgObject(), NULL, PRP_AC_SET, SET_CANCEL_REQUEST, NULL, 0, NULL, 0, FALSE, FALSE);
	
	if( pTaskQC )
	{
		if( IsBadReadPtr(pTaskQC, sizeof(QC_SETTING)) == TRUE )
			return STATUS_INVALID_PARAMETERS;
		
		status = FMMC_EasySynchronousBuildPRP(CDevManager::GetBillPrgObject(), NULL, PRP_AC_SET, TASK_QC_REQUEST, pTaskQC, sizeof(QC_SETTING), NULL, 0, FALSE, FALSE);
		if( status != STATUS_SUCCESS )
			return status;
	}
	
	else if( pTaskTest )
	{
		if( IsBadReadPtr(pTaskTest, sizeof(ULONG)) == TRUE )
			return STATUS_INVALID_PARAMETERS;
		
		
		status = FMMC_EasySynchronousBuildPRP(CDevManager::GetBillPrgObject(), NULL, PRP_AC_SET, TASK_TESTTING_REQUEST, pTaskTest, sizeof(ULONG), NULL, 0, FALSE, FALSE);
		if( status != STATUS_SUCCESS )
			return status;
		
	}
	
	if( nSamplingNumber )
		status = FMMC_EasySynchronousBuildPRP(CDevManager::GetBillPrgObject(), NULL, PRP_AC_SET, SAMPELING_NUMBER_REQUEST, &nSamplingNumber, sizeof(ULONG), NULL, 0, FALSE, FALSE);
	
	return status;

}


KSTATUS CStatusManager::SetSystemWorkStatusEx(PSYS_WORK_STATUS   pSysWorkStatus)
{
	if( pSysWorkStatus == NULL || IsBadReadPtr(pSysWorkStatus, sizeof(SYS_WORK_STATUS)) == TRUE )
		return STATUS_INVALID_PARAMETERS;
	
	
	if( CDevManager::GetBillPrgObject() == NULL )
		return STATUS_DEV_NOT_START;
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetBillPrgObject(), NULL, PRP_AC_SET, SYSTEM_WORK_STATUS_REQUEST, pSysWorkStatus, sizeof(SYS_WORK_STATUS), NULL, 0, FALSE, FALSE);
}

KSTATUS CStatusManager::GetSystemInformation(PTH_STATUS_INFORMATION  status_info,HANDLE   hExitEvent)
{
	ULONG   nInSize      = 0;
	PCHAR   pInputBuffer = NULL;
	
	if( CDevManager::GetOIInfo().s_thmgr_dev == NULL || status_info == NULL )
		return STATUS_INVALID_PARAMETERS;
	
	if( hExitEvent )
	{
		nInSize       = sizeof(HANDLE);
		pInputBuffer  = (PCHAR)hExitEvent;  //API事件
		
	}
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_thmgr_dev, NULL, PRP_AC_READ, MMGR_MSG_REQUEST, &pInputBuffer, nInSize, status_info, sizeof(TH_STATUS_INFORMATION), FALSE, FALSE);
}