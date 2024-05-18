#include "StdAfx.h"
#include "DbThAccess.h"



TH_DB_Access g_db;

DbThAccess::DbThAccess(void)
{
}

DbThAccess::~DbThAccess(void)
{
}


KSTATUS DbThAccess::TH_DB_Open_Connect(OPEN_ACCESS_FILE_PAR open_access_file_par,BOOL bNeedInitialize)
{
	g_db.SetInitializestatus(bNeedInitialize);
	return g_db.Open_Connect(open_access_file_par);
}

void DbThAccess::TH_DB_Close_Connect(void)
{
	g_db.Close_Connect();
}


KSTATUS DbThAccess::TH_DB_Excute(EXECUTE_PAR excute_par)
{
	return g_db.Excute(excute_par);
}


KSTATUS DbThAccess::TH_DB_Open_Rs(OPEN_RS_PAR*  open_rs_par)
{
	return g_db.Open_Rs(open_rs_par);
}



void DbThAccess::TH_DB_Close_Rs(ADODB::_RecordsetPtr Rs)
{
	return g_db.Close_Rs(Rs);
}



KSTATUS  DbThAccess::TH_DB_Login(PUSER_LOGIN login_par,int * UserType )
{
	return g_db.Login(login_par, UserType );
}


KSTATUS DbThAccess::TH_DB_CheckAccount(CHECK_ACCOUNT check_account)
{
	return g_db.CheckAccount(check_account);
}

KSTATUS  DbThAccess::TH_DB_GetUserInfo(USER_INFO* pUserInfo,unsigned int* pnSize)
{
	return g_db.GetUserInfo(pUserInfo,pnSize);
}


KSTATUS  DbThAccess::TH_DB_ModifyPwd(const char *UserName, const char *OldPwd, const char *NewPwd)
{
	return g_db.ModifyPwd(UserName, OldPwd, NewPwd);
}

KSTATUS DbThAccess::TH_DB_ModifyUserInfo( PUSER_INFO pUser_Info )
{
	return g_db.ModifyUserInfo(pUser_Info);
}

KSTATUS  DbThAccess::TH_DB_AddUser(USER_ADD user_add)
{
	return g_db.AddUser(user_add);
}


KSTATUS  DbThAccess::TH_DB_DelUser(USER_DEL user_del)
{
	return g_db.DelUser(user_del);
}


KSTATUS DbThAccess::TH_DB_AddTask(TASK_ADD* ptask_add)
{
	return g_db.AddTask(ptask_add);
}


KSTATUS DbThAccess::TH_DB_AddTaskMust( TASK_ADD* ptask_add)
{
	return g_db.AddTaskMust(ptask_add);

}
KSTATUS DbThAccess::TH_DB_DelTask(int* pnDelID,int nCounts,bool bIsqc )
{
	return g_db.DelTask(pnDelID,nCounts,bIsqc);
}


KSTATUS DbThAccess::TH_DB_DelAllTask(bool isAll)
{
	return g_db.DelAllTask(isAll);
}


KSTATUS DbThAccess::TH_DB_GetTaskInfo(bool bIsSelectById,int nID,TASK_INFO* pTaskInfo,unsigned int* pnSize)
{
	return g_db.GetTaskInfo(bIsSelectById,nID,pTaskInfo,pnSize);
}


KSTATUS DbThAccess::TH_DB_ReSetTask(int nID,TestTypeEnum nTestType,ULONG nStatus)
{
	return g_db.ReSetTask(nID, nTestType,nStatus);
}



KSTATUS DbThAccess::TH_DB_GetInfo(TASK_INFO* pTaskInfo,ULONG nID,TYPE_INFO type)
{
	return g_db.GetInfo(pTaskInfo,nID,type);
}



KSTATUS DbThAccess::TH_DB_SetInfo(TASK_INFO* pTaskInfo,ULONG nID,TYPE_INFO type)
{
	return g_db.SetInfo(pTaskInfo, nID,type);
}


KSTATUS DbThAccess::TH_DB_GetUsInfo(GET_US_INFO* pGetUsInfo)
{
	return g_db.GetUsInfo(pGetUsInfo);
}


KSTATUS DbThAccess::TH_DB_SetUsInfo(SET_US_INFO set_us_info)
{
	return g_db.SetUsInfo(set_us_info);
}



KSTATUS DbThAccess::TH_DB_GetUdcInfo(GET_UDC_INFO* pGetUdcInfo)
{
	return g_db.GetUdcInfo(pGetUdcInfo);
}


KSTATUS  DbThAccess::TH_DB_SetUdcInfo(SET_UDC_INFO set_udc_info)
{
	return g_db.SetUdcInfo(set_udc_info);
}


KSTATUS  DbThAccess::TH_DB_SearchTask(SEARCH_TASK* pSearch_task )
{
	return g_db.SearchTask(pSearch_task);
}


KSTATUS  DbThAccess::TH_DB_SetPrintPicPath(int nID,char* pPrintPicPath)
{
	return g_db.SetPrintPicPath(nID,pPrintPicPath);
}

KSTATUS DbThAccess::TH_DB_GetPrintPicPath( int nID,char* pPrintPicPath )
{
	return g_db.GetPrintPicPath(nID,pPrintPicPath);
}

KSTATUS DbThAccess::TH_DB_SetBarcode(int nID, char* pBarcode)
{
	return g_db.SetBarcode(nID, pBarcode);
}

KSTATUS DbThAccess::TH_DB_GetBarcode(int nID, char* pBarcode)
{
	return g_db.GetBarcode(nID, pBarcode);
}

//KSTATUS DbThAccess::TH_DB_AutoImportUdcData(UDC_IMPORT_INFO* pAutoImportUdcInfo,bool* pbIsNew,unsigned int* pID)
//{
//	return g_db.AutoImportUdcData(pAutoImportUdcInfo,pbIsNew,pID);
//}



KSTATUS DbThAccess::TH_DB_SetPicPath(SET_PIC_PATH set_pic_path)
{
	return g_db.SetPicPath(set_pic_path);
}



KSTATUS DbThAccess::TH_DB_GetLisStatus(int nID,unsigned int* pStatus)
{
	return g_db.GetLisStatus(nID,pStatus);
}

KSTATUS DbThAccess::TH_DB_SetLisStatus(int nID,unsigned int nStatus)
{
	return g_db.SetLisStatus(nID,nStatus);
}

KSTATUS DbThAccess::TH_DB_GetPrintStatus(int nID,unsigned int* pStatus)
{
	return g_db.GetPrintStatus(nID,pStatus);
}

KSTATUS DbThAccess::TH_DB_SetPrintStatus(int nID,unsigned int nStatus)
{
	return g_db.SetPrintStatus(nID,nStatus);
}

KSTATUS DbThAccess::TH_DB_GetLastTaskId(ULONG *nID)
{
    return g_db.GetLastTaskId(nID);
}

KSTATUS DbThAccess::TH_DB_GetLISList( SENDTO_LIS_ID* pSendto_lis_id )
{
	return g_db.GetLISList(pSendto_lis_id);
}

KSTATUS DbThAccess::TH_DB_UpdateTaskStatus( ULONG TaskId,KSTATUS status )
{
	return g_db.UpdateTaskStatus(TaskId,status);

}

KSTATUS DbThAccess::TH_DB_CheckTaskStates( BOOL IsTaday )
{
	return g_db.CheckTaskStates(IsTaday);

}

KSTATUS DbThAccess::TH_DB_GetQcTaskInfo( QCTypeEnum QcType,QC_INFO* qc_task_info,unsigned int* pnSize )
{
	return g_db.GetQcTaskInfo( QcType,qc_task_info,pnSize);
}

KSTATUS DbThAccess::TH_DB_GetDeptInfo( Dept_Pos_Info* pUserInfo,unsigned int* pnSize )
{
	return g_db.GetDeptInfo(pUserInfo,pnSize);

}

KSTATUS DbThAccess::TH_DB_GetPosInfo( Dept_Pos_Info* pUserInfo,unsigned int* pnSize )
{
	return g_db.GetPosInfo(pUserInfo,pnSize);
}

KSTATUS DbThAccess::TH_DB_GetAllTaskInfo( PULONG pAllTaskID,unsigned int* pnSize )
{
	return g_db.GetAllTaskInfo( pAllTaskID,pnSize );
}