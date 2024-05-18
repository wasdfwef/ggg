#include "StdAfx.h"
#include "US12s_DB.h"
#include "DevManager.h"
#include "\CommonAPI\inc\db_interface.h"
#include "\CommonAPI\inc\lc_interface.h"
#include <SHLWAPI.H>

extern "C"
KSTATUS
WINAPI
THInterface_DBUpdateRecordInfo(
ULONG  db_info_type,
PCHAR  pInfoBuffer,
ULONG  nInfoLength
)
{
	if( CDevManager::GetOIInfo().s_db_dev == NULL || db_info_type < USER_INFO_TYPE || nInfoLength == 0 ||
		pInfoBuffer == NULL )
		return STATUS_INVALID_PARAMETERS;
	
	
	if( db_info_type < PRN_PATH_INFO_TYPE )
		return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_WRITE, UPDATE_RECORD_REQUEST, pInfoBuffer, nInfoLength, NULL, 0, FALSE, FALSE);
	else if( db_info_type == MERGER_UDC_TYPE )
		return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_DEVIO, UDC_DATA_MERGER_REQUEST, pInfoBuffer, nInfoLength, NULL, 0, FALSE, FALSE);
	else if( db_info_type == PRN_PATH_INFO_TYPE )
		return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_WRITE, PRINT_PATH_REQUEST, pInfoBuffer, nInfoLength, NULL, 0, FALSE, FALSE);
	else if( db_info_type == RESET_TASK_TYPE )
	{
		RESET_RECORD_INFO   ResetRecordInfo = {0};
		
		if( nInfoLength != sizeof(ULONG) )
			return STATUS_INVALID_PARAMETERS;
		
		memcpy(&ResetRecordInfo.nTaskID, pInfoBuffer, sizeof(ULONG));
		ResetRecordInfo.rec_obj = CDevManager::GetOIInfo().s_rec_dev;
		return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_SET, RESET_RECORD_REQUEST, &ResetRecordInfo, sizeof(ResetRecordInfo), NULL, 0, FALSE, FALSE);
	}
	else if( db_info_type == USER_PSD_CHANGE )
	{
		if( nInfoLength != sizeof(UPC) )
			return STATUS_INVALID_PARAMETERS;
		
		return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_SET, PSD_CHANGE_REQUEST, pInfoBuffer, nInfoLength, NULL, 0, FALSE, FALSE);
	}
	else if( db_info_type == REPORT_STATUS_TYPE )
		return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_SET, SET_REPORTER_STATUS_REQUEST, pInfoBuffer, nInfoLength, NULL, 0, FALSE, FALSE);
	
	OutputDebugString("THInterface_DBUpdateRecordInfo..STATUS_INVALID_PARAMETERS\n");
	return STATUS_INVALID_PARAMETERS;
	
	
}


extern "C"
KSTATUS
WINAPI
THInterface_DBAddRecordInfo(//增加记录
ULONG  db_info_type,   //USER_INFO_TYPE添加用户,否则为添加任务
PCHAR  pRecordBuffer,  //注意:指定USER_INFO_TYPE中,如果将password置空,则为检测帐号,参见NEW_USER_INFO结构,添加任务参见FULL_TASK_INFO_1
ULONG  nRecordLength, //结构长度
PCHAR  pAddReslutBuffer,
ULONG  nReslutLength
)
{
	ADD_RECORD_INFO   AddRecordInfo = {0};
	
	if( pRecordBuffer == NULL || nRecordLength == 0 || CDevManager::GetOIInfo().s_db_dev == NULL || nRecordLength > (sizeof(ADD_RECORD_INFO) - 2) || IsBadReadPtr(pRecordBuffer, nRecordLength) == TRUE )
		return STATUS_INVALID_PARAMETERS;
	
	if( (pAddReslutBuffer && nReslutLength && IsBadWritePtr(pAddReslutBuffer, nReslutLength) == TRUE) ||
		(pAddReslutBuffer && nReslutLength == 0) || (pAddReslutBuffer == NULL && nReslutLength) )
		return STATUS_INVALID_PARAMETERS;
	
	
	AddRecordInfo.record_type = (USHORT)db_info_type;
	memcpy(AddRecordInfo.Reserved, pRecordBuffer, nRecordLength);
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_CREATE, ADD_RECORD_REQUEST, &AddRecordInfo, sizeof(AddRecordInfo), pAddReslutBuffer, nReslutLength, FALSE, FALSE);
	
	
}


extern "C"
KSTATUS
WINAPI
THInterface_ReadRecordInfo(
ULONG  db_info_type,
PCHAR  pRecordBuffer,
ULONG  nLength
)
{
	ULONG  nType;
	
	if( CDevManager::GetOIInfo().s_db_dev == NULL || nLength == 0 || pRecordBuffer == NULL )
		return STATUS_INVALID_PARAMETERS;
	
	if( db_info_type == REPORT_STATUS_TYPE )
		return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_QUERY, QUERY_REPORT_STATUS_REQUEST, pRecordBuffer,nLength, NULL, 0, FALSE, FALSE);
	
	OutputDebugString("THInterface_ReadRecordInfo...\n");
	
	nType = db_info_type;
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_READ, GET_RECORD_REQUEST, &nType, sizeof(nType), pRecordBuffer, nLength, FALSE, FALSE);
	
}

extern "C"
KSTATUS
WINAPI
THInterface_DelRecordInfo(
ULONG  db_info_type,  //USER_INFO_TYPE删除用户,否则为删除任务
PCHAR  pDelRecordInfo,
ULONG  nLength
)
{
	DEL_RECORD_INFO    del_record_info = {0};
	
	if( CDevManager::GetOIInfo().s_db_dev == NULL || pDelRecordInfo == NULL || nLength == 0 || IsBadReadPtr(pDelRecordInfo, nLength) == TRUE || nLength > (sizeof(DEL_RECORD_INFO) - 2))
		return STATUS_INVALID_PARAMETERS;
	
	
	del_record_info.nRecordType = (USHORT)db_info_type;
	memcpy(del_record_info.Reserved, pDelRecordInfo, nLength);
	GetModuleFileName(NULL,del_record_info.RelativePath, MAX_PATH);
	PathRemoveFileSpec(del_record_info.RelativePath);
	if( db_info_type != USER_INFO_TYPE )
		del_record_info.s_rec_obj = CDevManager::GetOIInfo().s_rec_dev;
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_SET, DEL_RECORD_REQUEST, &del_record_info, sizeof(del_record_info), NULL, 0, FALSE, FALSE);
}


extern "C"
KSTATUS
WINAPI
THInterface_ImportDataToDB(
PCHAR  pImportData,     
ULONG  nImportLength,
BOOL   bAutoImport
)
{
	DATA_IMPORT_INFO    i_info = {0};
	
	if( CDevManager::GetOIInfo().s_db_dev == NULL || pImportData == NULL || nImportLength == 0 || nImportLength > sizeof(i_info.Reserved) )
		return STATUS_INVALID_PARAMETERS;
	
	if( bAutoImport )
		i_info.ImportType = UDC_AUTO_IMPORT_TYPE;
	else
		i_info.ImportType = UDC_MAN_IMPORT_TYPE;
	
	memcpy(i_info.Reserved, pImportData, nImportLength);
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_DEVIO,IMPORT_DATA_REQUEST, &i_info, sizeof(i_info), NULL, 0, FALSE, FALSE);
	
}



extern "C"
KSTATUS
WINAPI
THInterface_QueryDBConfigInfo(
ULONG  db_info_type,    //目前只支持udc与us的信息
PCHAR  pConfigInfo,    //目前使用db_interface.h中US_CFG_INFO1与UDC_CFG_INFO1结构
ULONG  nLength         //对应结构长度
)
{
	CONFIG_RECORD_INFO  r_config_info = {0};
	
	if( CDevManager::GetOIInfo().s_db_dev == NULL || db_info_type < USER_INFO_TYPE || pConfigInfo == NULL || nLength == 0 || IsBadWritePtr(pConfigInfo, nLength) == TRUE )
		return STATUS_INVALID_PARAMETERS;
	
	r_config_info.nRecordType = (USHORT)db_info_type;
	r_config_info.pConfigConent = pConfigInfo;
	r_config_info.nConfigLength = nLength;
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev,NULL, PRP_AC_QUERY, CONFIG_RECORD_REQUEST, NULL, 0, &r_config_info,sizeof(r_config_info), FALSE, FALSE);
}



extern "C"
KSTATUS
WINAPI
THInterface_SetDBConfigInfo(
ULONG  db_info_type,    //目前只支持udc与us的信息
PCHAR  pConfigInfo,    //目前使用db_interface.h中US_CFG_INFO1与UDC_CFG_INFO1结构
ULONG  nLength         //对应结构长度
)
{
	CONFIG_RECORD_INFO  r_config_info = {0};
	
	if( CDevManager::GetOIInfo().s_db_dev == NULL || db_info_type < USER_INFO_TYPE || pConfigInfo == NULL || nLength == 0 || IsBadReadPtr(pConfigInfo, nLength) == TRUE )
		return STATUS_INVALID_PARAMETERS;
	
	r_config_info.nRecordType = (USHORT)db_info_type;
	r_config_info.pConfigConent = pConfigInfo;
	r_config_info.nConfigLength = nLength;
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev,NULL, PRP_AC_SET, CONFIG_RECORD_REQUEST, &r_config_info, sizeof(r_config_info), NULL, 0, FALSE, FALSE);
	
}


extern "C"
KSTATUS
THInterface_SearchRecordInfo(
PCHAR   pSearchInfo,      //目前只支持使用SEARCH_TASK1,如果执行成功,由系统负责分配相关的buffer,则使用完后调用THInterface_FreeSysBuffer
ULONG   nInfoLength
)
{
	if( CDevManager::GetOIInfo().s_db_dev == NULL || pSearchInfo == NULL || nInfoLength == 0 )
		return STATUS_INVALID_PARAMETERS;
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_DEVIO, SEARCH_RECORD_REQUEST, pSearchInfo, nInfoLength, NULL, 0, FALSE, FALSE);
}


extern "C"
KSTATUS
WINAPI
THInterface_UserLogin(
PLOGIN_USER_INFO   pLoginInfo,
PULONG             pUserToken,
PVOID              pReserved
)
{
	if( CDevManager::GetOIInfo().s_db_dev == NULL )
	{
		OutputDebugString("THInterface_UserLogin->db_dev not start...\n");
		return STATUS_DEV_NOT_START;
		
	}
	
	if( pLoginInfo == NULL || pUserToken == NULL  )
	{
		OutputDebugString("THInterface_UserLogin->STATUS_INVALID_PARAMETERS...\n");
		return STATUS_INVALID_PARAMETERS;
	}
	
	return FMMC_EasySynchronousBuildPRP(CDevManager::GetOIInfo().s_db_dev, NULL, PRP_AC_DEVIO, USER_LOGIN_REQUEST, pLoginInfo, sizeof(LOGIN_USER_INFO), pUserToken, sizeof(ULONG), FALSE, FALSE);
	
}