#pragma once
#include "TH_DB_Access.h"

class DbThAccess
{
public:
	DbThAccess(void);
	~DbThAccess(void);




public:
	KSTATUS  TH_DB_Open_Connect(OPEN_ACCESS_FILE_PAR open_access_file_par,BOOL bNeedInitialize);
	void     TH_DB_Close_Connect(void);
	KSTATUS  TH_DB_CheckTaskStates(BOOL IsTaday);
	KSTATUS  TH_DB_Excute(EXECUTE_PAR excute_par);
	KSTATUS  TH_DB_Open_Rs(OPEN_RS_PAR*  open_rs_par);
	void     TH_DB_Close_Rs(ADODB::_RecordsetPtr Rs);
	KSTATUS  TH_DB_Login(PUSER_LOGIN login_par,int *UserType );
	KSTATUS  TH_DB_ModifyUserInfo(PUSER_INFO pUser_Info);	
	KSTATUS  TH_DB_CheckAccount(CHECK_ACCOUNT check_account);
	KSTATUS  TH_DB_GetUserInfo(USER_INFO* pUserInfo,unsigned int* pnSize);

	KSTATUS  TH_DB_GetDeptInfo(Dept_Pos_Info* pUserInfo,unsigned int* pnSize);
	KSTATUS  TH_DB_GetPosInfo(Dept_Pos_Info* pUserInfo,unsigned int* pnSize);



	KSTATUS  TH_DB_ModifyPwd(const char *UserName, const char *OldPwd, const char *NewPwd);
	KSTATUS  TH_DB_AddUser(USER_ADD user_add);
	KSTATUS  TH_DB_DelUser(USER_DEL user_del);

	KSTATUS  TH_DB_AddTask(TASK_ADD* ptask_add);

	KSTATUS  TH_DB_AddTaskMust(TASK_ADD* ptask_add);

	KSTATUS  TH_DB_DelTask(int* pnDelID,int nCounts,bool bIsqc );
	KSTATUS  TH_DB_DelAllTask(bool isAll = false);
	KSTATUS  TH_DB_GetTaskInfo(bool bIsSelectById,int nID,TASK_INFO* pTaskInfo,unsigned int* pnSize);

	KSTATUS  TH_DB_GetAllTaskInfo(PULONG pAllTaskID,unsigned int* pnSize);
	
	KSTATUS  TH_DB_GetQcTaskInfo(QCTypeEnum QcType,QC_INFO*  qc_task_info,unsigned int* pnSize);
	KSTATUS  TH_DB_ReSetTask(int nID,TestTypeEnum   nTestType,ULONG nStatus);
	KSTATUS  TH_DB_UpdateTaskStatus(ULONG TaskId,KSTATUS status);
	KSTATUS  TH_DB_GetInfo(TASK_INFO* pTaskInfo,ULONG nID,TYPE_INFO type);
	KSTATUS  TH_DB_SetInfo(TASK_INFO* pTaskInfo,ULONG nID,TYPE_INFO type);
	KSTATUS  TH_DB_GetUsInfo(GET_US_INFO* pGetUsInfo);
	KSTATUS  TH_DB_SetUsInfo(SET_US_INFO set_us_info);
	KSTATUS  TH_DB_GetUdcInfo(GET_UDC_INFO* pGetUdcInfo);
	KSTATUS  TH_DB_SetUdcInfo(SET_UDC_INFO set_udc_info);
	KSTATUS  TH_DB_SearchTask(SEARCH_TASK* pSearch_task );
	KSTATUS  TH_DB_SetPrintPicPath(int nID,char* pPrintPicPath);
	KSTATUS  TH_DB_GetPrintPicPath(int nID,char* pPrintPicPath);
	KSTATUS  TH_DB_SetBarcode(int nID, char* pBarcode);
	KSTATUS  TH_DB_GetBarcode(int nID, char* pBarcode);

	KSTATUS  TH_DB_GetLISList(SENDTO_LIS_ID* pSendto_lis_id);
	/*KSTATUS  TH_DB_AutoImportUdcData(UDC_IMPORT_INFO* pAutoImportUdcInfo,bool* pbIsNew,unsigned int* pID);*/
	KSTATUS  TH_DB_SetPicPath(SET_PIC_PATH set_pic_path);
	
	KSTATUS  TH_DB_GetLisStatus(int nID,unsigned int* pStatus);
	KSTATUS  TH_DB_SetLisStatus(int nID,unsigned int nStatus);
	KSTATUS  TH_DB_GetPrintStatus(int nID,unsigned int* pStatus);
	KSTATUS  TH_DB_SetPrintStatus(int nID,unsigned int nStatus);

    KSTATUS  TH_DB_GetLastTaskId(ULONG *nID);
};
