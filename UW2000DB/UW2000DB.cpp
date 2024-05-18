// UW2000DB.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "UW2000DB.h"
#include "DbThAccess.h"
#include <shlwapi.h>
#include "DbgMsg.h"


#define  ACCESS_RELATIVE_PATH   "DB\\uw.mdb"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CUW2000DBApp

BEGIN_MESSAGE_MAP(CUW2000DBApp, CWinApp)
END_MESSAGE_MAP()




DbThAccess db;

static bool DeleteDir(const char *DirName)
{
	bool Result = false;
	char PathName[MAX_PATH] = {0};
	char PicFullPath[MAX_PATH] = {0};
	WIN32_FIND_DATA FindFileData = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;

	strcpy(PathName,DirName);
	PathAppend(PathName, "*.*");
	hFind = FindFirstFile(PathName, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		goto end;

	while(TRUE)
	{
		/*DBG_MSG("dwFileAttributes=%x",FindFileData.dwFileAttributes);
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			DeleteDir(FindFileData.cFileName);
		else*/
		{

			if( stricmp(FindFileData.cFileName, ".") != 0 && stricmp(FindFileData.cFileName, "..") != 0)
			{
				PathCombine(PicFullPath, DirName, FindFileData.cFileName);
				DeleteFile(PicFullPath);
			}
		}

		if( FindNextFile(hFind, &FindFileData) == FALSE )
			break;
	}
end:
	if(hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	if( RemoveDirectory(DirName) )
		Result = true;

	//DBG_MSG("%d",Result);

	return Result;
}


#define GRAPHIC_SUBDIR "RecGrapReslut"

static void DeleteAllTaskPics()
{
	char path[MAX_PATH]={0};
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAppend(path,GRAPHIC_SUBDIR);

	DeleteDir(path);
}
static void DeleteTaskPics(ULONG pnDelID)
{
	char path[MAX_PATH]={0};
	char SubDir[20] = {0};
	sprintf(SubDir,"%d",pnDelID);

	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAppend(path,GRAPHIC_SUBDIR);

	PathAppend(path,SubDir);

	DeleteDir(path);

}
// CUW2000DBApp 构造

CUW2000DBApp::CUW2000DBApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CUW2000DBApp 对象

CUW2000DBApp theApp;


// CUW2000DBApp 初始化

BOOL CUW2000DBApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}



extern "C"
KSTATUS
WINAPI
Access_OpenAccess(
BOOL bNeedInitialize
					 )
{
	OPEN_ACCESS_FILE_PAR    open_access;	

	ZeroMemory(&open_access, sizeof(open_access));
	open_access.nOpen_Type = OpenAccess2000;

	GetModuleFileName(NULL, open_access.csAccess_file_full_path, MAX_PATH);
	PathRemoveFileSpec(open_access.csAccess_file_full_path);
	PathAppend(open_access.csAccess_file_full_path, ACCESS_RELATIVE_PATH);
	KSTATUS status = db.TH_DB_Open_Connect(open_access,bNeedInitialize);

	//DBG_MSG("%x\n",status);
	return status;
}


extern "C"
VOID
WINAPI
Access_CloseAccess()
{
	db.TH_DB_Close_Connect();
}


extern "C"
KSTATUS
WINAPI
Access_CheckTaskStates(BOOL IsToday )
{
	return db.TH_DB_CheckTaskStates(IsToday);

}

extern "C"
KSTATUS
WINAPI
Access_NewTask(
TASK_ADD*   task_add
				  )
{
	KSTATUS    status;
	//if(task_add->bForceAdd == FALSE)
		status = db.TH_DB_AddTask(task_add);
	/*else
		status = db.TH_DB_AddTaskMust(task_add);*/
	return status;
}

extern "C"
KSTATUS
WINAPI
Access_UpdateTaskStatus
(ULONG   TaskID,
 ULONG   state
 )
{
	return db.TH_DB_UpdateTaskStatus(TaskID,state);

}



extern "C"
KSTATUS
WINAPI
Access_ResetTask(
RESET_RECORD_INFO*  ResetRecInfo
					)
{
	
	KSTATUS                 status;
	status = db.TH_DB_ReSetTask(ResetRecInfo->nTaskID,ResetRecInfo->nTestType,ResetRecInfo->nStatus);
	/*if(status != STATUS_SUCCESS)
		return status;*/
	//DeleteTaskPics(ResetRecInfo->nTaskID);
	return status;
}

extern "C"
KSTATUS
WINAPI
Access_DelTask(
DEL_TASK*   del_info
					  )
{
	bool                   bIsQC;
	KSTATUS                status = STATUS_SUCCESS;
	if( del_info->nCounter == 0)
		return STATUS_INVALID_PARAMETERS;

	if( del_info->bIsQC )
		bIsQC = TRUE;
	else
		bIsQC = FALSE;

	if( del_info->pDelIDArray && del_info->nCounter != DELETE_ALL_TASK_ITEM && del_info->nCounter > 0)
	{
		status = db.TH_DB_DelTask((int*)del_info->pDelIDArray, del_info->nCounter, bIsQC);
		if(status != STATUS_SUCCESS)
			return status;
		for(int i = 0;i < del_info->nCounter ;i++)
		{
			DeleteTaskPics(del_info->pDelIDArray[i]);
		}
	}
		
	else if( del_info->pDelIDArray == NULL && del_info->nCounter != DELETE_ALL_TASK_ITEM )
	{

		//DBG_MSG("CCC");
		status = db.TH_DB_DelAllTask();

		//DBG_MSG("status=%x\n",status);
		if(status != STATUS_SUCCESS)
			return status;
		//DeleteAllTaskPics();
	}
	else if(del_info->pDelIDArray == NULL && del_info->nCounter == DELETE_ALL_TASK_ITEM)
	{
		status = db.TH_DB_DelAllTask(true);
		if (status != STATUS_SUCCESS)
			return status;
	}
	else
	{

	}
			
	return status;
}



extern "C" 
KSTATUS
WINAPI
Access_FreeBuf(PCHAR pBuffer)
{
	if(pBuffer != NULL)
		delete[] pBuffer;
	pBuffer = NULL;
	return STATUS_SUCCESS;
}


extern "C"
KSTATUS
WINAPI
Access_FreeGetQcTaskInfoBuf(QC_TASK_INFO &qc_task_info)
{
	if(qc_task_info.pQcTaskInfo != NULL)
		delete[] qc_task_info.pQcTaskInfo;
	qc_task_info.pQcTaskInfo = NULL;
	return STATUS_SUCCESS;
}


extern "C"
KSTATUS
WINAPI
Access_GetQcTaskInfo(
QCTypeEnum         QcType,
QC_TASK_INFO*    qc_task_info
					   )
{
	qc_task_info->nQcTaskCounter = 0;
	qc_task_info->pQcTaskInfo =NULL;

	KSTATUS  status = 0;
	status = db.TH_DB_GetQcTaskInfo(QcType,NULL,(unsigned int *)&qc_task_info->nQcTaskCounter);

	DBG_MSG("qc_task_info->nQcTaskCounter=%d\n",qc_task_info->nQcTaskCounter);

	if(qc_task_info->nQcTaskCounter == 0)
	{
		return STATUS_NOT_FOUND_VALUE;
	}	
	qc_task_info->pQcTaskInfo = new QC_INFO[qc_task_info->nQcTaskCounter];
	if(qc_task_info->pQcTaskInfo ==NULL)
	{
		return STATUS_ALLOC_MEMORY_ERROR;
	}

	ZeroMemory(qc_task_info->pQcTaskInfo,sizeof(QC_INFO)*qc_task_info->nQcTaskCounter);

	status = db.TH_DB_GetQcTaskInfo(QcType,(QC_INFO*)qc_task_info->pQcTaskInfo,(unsigned int *)&qc_task_info->nQcTaskCounter);
	if(status != STATUS_SUCCESS)
	{
		if(qc_task_info->pQcTaskInfo !=NULL)
			delete[] qc_task_info->pQcTaskInfo;
		qc_task_info->nQcTaskCounter = 0;
	}

	return   status;
}

//extern "C"
//KSTATUS
//WINAPI
//Access_GetUsQcTaskInfo(
//UsQcEnum         UsQcType,
//PQC_TASK_INFO    qc_task_info
//)
//{
//	KSTATUS  status = 0;
//	/*status = db.TH_DB_GetQcTaskInfo()*/
//
//	return   status;
//}
//
//extern "C"
//KSTATUS
//WINAPI
//Access_GetUdcQcTaskInfo(
//UdsQcEnum        UdcQcType,
//PQC_TASK_INFO    qc_task_info
//					   )
//{
//	KSTATUS  status = 0;
//
//	return   status;
//}


extern "C"
KSTATUS
WINAPI
Access_FreeGetTodayTaskInfoBuf(TOADY_TASK_INFO1 &today_task_info)
{
	if(today_task_info.pFullTaskInfo != NULL)
		delete[] today_task_info.pFullTaskInfo;
	today_task_info.pFullTaskInfo = NULL;
	return STATUS_SUCCESS;
}


extern "C"
KSTATUS
WINAPI
Access_FreeGetAllTaskInfoBuf(ALL_TASK_INFO &all_task_info)
{
	if(all_task_info.pAllTaskID != NULL)
		delete[] all_task_info.pAllTaskID ;
	all_task_info.pAllTaskID = NULL;
	return STATUS_SUCCESS;

}
extern "C"
KSTATUS
WINAPI
Access_GetAllTaskInfo(
ALL_TASK_INFO*  all_task_info
)
{
	KSTATUS                  status;	

	all_task_info->nAllTaskCounter = 0;
	all_task_info->pAllTaskID = NULL;

	status = db.TH_DB_GetAllTaskInfo(NULL, (unsigned int*)&all_task_info->nAllTaskCounter);
	DBG_MSG("nAllTaskCounter = %d\n",all_task_info->nAllTaskCounter);
	if( all_task_info->nAllTaskCounter == 0 )
		return STATUS_NOT_FOUND_VALUE;
	all_task_info->pAllTaskID =new ULONG[all_task_info->nAllTaskCounter];	
	if( all_task_info->pAllTaskID == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;
	ZeroMemory(all_task_info->pAllTaskID, all_task_info->nAllTaskCounter * sizeof(ULONG));


	status = db.TH_DB_GetAllTaskInfo(all_task_info->pAllTaskID, (unsigned int*)&all_task_info->nAllTaskCounter);
	//DBG_MSG("status = %u\n",status);
	if( status != STATUS_SUCCESS )
	{
		if(all_task_info->pAllTaskID != NULL)
			delete[]all_task_info->pAllTaskID;
		all_task_info->nAllTaskCounter= 0;
	}
	return status;	
}

extern "C"
KSTATUS
WINAPI
Access_GetTodayTaskInfo(
TOADY_TASK_INFO1*  today_task_info
					  )
{
	
	ULONG                    nSize = 0;
	KSTATUS                  status;	

	today_task_info->nToadyTaskCounter = 0;
	today_task_info->pFullTaskInfo = NULL;

	status = db.TH_DB_GetTaskInfo(FALSE, 0, NULL, (unsigned int*)&today_task_info->nToadyTaskCounter);
DBG_MSG("nToadyTaskCounter = %d\n",today_task_info->nToadyTaskCounter);
	if( today_task_info->nToadyTaskCounter == 0 )
		return STATUS_NOT_FOUND_VALUE;
	today_task_info->pFullTaskInfo =new TASK_INFO[today_task_info->nToadyTaskCounter];	
	if( today_task_info->pFullTaskInfo == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	//为什么这里不进行初始化,以前我培训的时候就讲过该问题. cracker
	ZeroMemory(today_task_info->pFullTaskInfo, today_task_info->nToadyTaskCounter * sizeof(TASK_INFO));


	status = db.TH_DB_GetTaskInfo(FALSE, 0, (TASK_INFO*)today_task_info->pFullTaskInfo, (unsigned int*)&today_task_info->nToadyTaskCounter);
//DBG_MSG("status = %u\n",status);
	if( status != STATUS_SUCCESS )
	{
		if(today_task_info->pFullTaskInfo != NULL)
			delete[]today_task_info->pFullTaskInfo;
		today_task_info->nToadyTaskCounter = 0;
	}
	return status;	
}

extern "C"
KSTATUS
WINAPI
Access_GetTaskInfo(
ULONG           nID,
TYPE_INFO       TypeInfo,
TASK_INFO*      tk_1
					 )
{
	KSTATUS   status = STATUS_INVALID_PARAMETERS;	

	ZeroMemory(tk_1,sizeof(TASK_INFO));
	if( TypeInfo < USER_TYPE_INFO || TypeInfo > ALL_TYPE_INFO )
		return STATUS_INVALID_PARAMETERS;

 	if( TypeInfo == USER_TYPE_INFO || TypeInfo == ALL_TYPE_INFO )
	{
		status = db.TH_DB_GetInfo(tk_1,nID, USER_TYPE_INFO);
		if( status != STATUS_SUCCESS )
			return status;
	}

	if( TypeInfo == US_TYPE_INFO || TypeInfo == ALL_TYPE_INFO )
	{
		status = db.TH_DB_GetInfo(tk_1,nID, US_TYPE_INFO);
		if( status != STATUS_SUCCESS )
			return status;
	}

	//DBG_MSG("xxxxxxxxxxxxxxx: %d", sizeof(TASK_INFO));

	if( TypeInfo == UDC_TYPE_INFO || TypeInfo == ALL_TYPE_INFO )
	{
		status = db.TH_DB_GetInfo(tk_1,nID, UDC_TYPE_INFO);
	}
		
	return status;

}

extern "C"
KSTATUS
WINAPI
Access_UpdateTaskInfo(
TYPE_INFO     TypeInfo,
ULONG         nID,
TASK_INFO*    ptask_info
						 )
{
	KSTATUS     status = STATUS_INVALID_PARAMETERS;

	/*if (!ptask_info || nID != ptask_info->main_info.nID || nID != ptask_info->pat_info.nID || nID != ptask_info->us_info.nID || nID != ptask_info->udc_info.nID)
	{
		DBG_MSG("Access_UpdateTaskInfo param error\n");
		return STATUS_INVALID_PARAMETERS;
	}*/

	if( TypeInfo < USER_TYPE_INFO || TypeInfo > ALL_TYPE_INFO )
		return STATUS_INVALID_PARAMETERS;

	if( TypeInfo == USER_TYPE_INFO || TypeInfo == ALL_TYPE_INFO )
	{
		status = db.TH_DB_SetInfo(ptask_info,nID, USER_TYPE_INFO);
		if (status != STATUS_SUCCESS)
		{
			DBG_MSG("TH_DB_SetInfo() == %d\n",status);
			return status;
		}
	}
		
	if( TypeInfo == US_TYPE_INFO || TypeInfo == ALL_TYPE_INFO )	
	{
		status = db.TH_DB_SetInfo(ptask_info, nID,US_TYPE_INFO);
		if( status != STATUS_SUCCESS )
			return status;
	}
		

	if( TypeInfo == UDC_TYPE_INFO || TypeInfo == ALL_TYPE_INFO )
		status = db.TH_DB_SetInfo(ptask_info,nID, UDC_TYPE_INFO);

	return status;

}

extern "C"
KSTATUS
WINAPI
Access_QueryReportStatus(
REPORT_STATUS1*  pReportStatus
					  )
{
	KSTATUS  status1;
	unsigned int status = 0;

	pReportStatus->bLis = pReportStatus->bPrint = FALSE;

	if( pReportStatus->report_mask & REPORT_LIS_MASK )
	{
		status1 = db.TH_DB_GetLisStatus(pReportStatus->nTaskID,&status);
		if( status1 != STATUS_SUCCESS )
			pReportStatus->bLis = FALSE;
		else
			pReportStatus->bLis = (BOOLEAN)status;
	}
	if( pReportStatus->report_mask & REPORT_PRINT_MASK )
	{
		status1 = db.TH_DB_GetPrintStatus(pReportStatus->nTaskID,&status);
		if( status1 != STATUS_SUCCESS )
			pReportStatus->bPrint = FALSE;
		else
			pReportStatus->bPrint = (BOOLEAN)status;
		
	}

	return STATUS_SUCCESS;
}



extern "C"
KSTATUS
WINAPI
Access_SetReportStatus(
REPORT_STATUS1*   pReportStatus
					   )
{
	if( pReportStatus->report_mask & REPORT_LIS_MASK )
		db.TH_DB_SetLisStatus(pReportStatus->nTaskID,pReportStatus->bLis ? 1 : 0);
	if( pReportStatus->report_mask & REPORT_PRINT_MASK )
		db.TH_DB_SetPrintStatus(pReportStatus->nTaskID, pReportStatus->bPrint ? 1 : 0);

	return STATUS_SUCCESS;
}

extern "C"
KSTATUS
WINAPI
Access_QueryRecordConfig(
CONFIG_RECORD_INFO* ConfigInfo
							)
{
	KSTATUS   status = STATUS_INVALID_PARAMETERS;

	if( ConfigInfo->nConfigLength == 0 || ConfigInfo->pConfigConent == NULL )
	{
		DBG_MSG("fail");
		return status;
	}
	if( ConfigInfo->nRecordType == US_TYPE_INFO && ConfigInfo->nConfigLength == sizeof(GET_US_INFO) )
	{
		DBG_MSG("Access_QueryRecordConfig\n");
		status = db.TH_DB_GetUsInfo((GET_US_INFO*)ConfigInfo->pConfigConent);
	}
		
	else if( ConfigInfo->nRecordType == UDC_TYPE_INFO && ConfigInfo->nConfigLength == sizeof(GET_UDC_INFO) )
		status = db.TH_DB_GetUdcInfo((GET_UDC_INFO*)ConfigInfo->pConfigConent);

	return status;
}

extern "C"
KSTATUS
WINAPI
Access_SetRecordConfig(
CONFIG_RECORD_INFO* ConfigInfo
						  )
{
	KSTATUS   status = STATUS_INVALID_PARAMETERS;

	if( ConfigInfo->nConfigLength == 0 || ConfigInfo->pConfigConent == NULL )
		return status;

	if( ConfigInfo->nRecordType == US_TYPE_INFO && ConfigInfo->nConfigLength == sizeof(SET_US_INFO) )
	{
		DBG_MSG("Access_SetRecordConfig");
		SET_US_INFO* s_info = (SET_US_INFO*)ConfigInfo->pConfigConent;
		status = db.TH_DB_SetUsInfo(*s_info);

	}
	else if( ConfigInfo->nRecordType == UDC_TYPE_INFO && ConfigInfo->nConfigLength == sizeof(SET_UDC_INFO) )
	{
		SET_UDC_INFO* c_info = (SET_UDC_INFO*)ConfigInfo->pConfigConent;
		status = db.TH_DB_SetUdcInfo(*c_info);
	}

	return status;
}

extern "C"
KSTATUS
WINAPI
Access_SetGrapPath(
SAMPLING_GRAP_PATH*   s_path
					  )
{
	SET_PIC_PATH   pic_path = {0};

	strcpy(pic_path.sImageFolder, s_path->RelativePath);
	PathRemoveFileSpec(pic_path.sImageFolder);

	if( s_path->RelativePath[0] == 0 )
		return STATUS_INVALID_PARAMETERS;

	pic_path.nID = s_path->nTaskID;
	strcpy(pic_path.sImageFolder, s_path->RelativePath);

	return db.TH_DB_SetPicPath(pic_path);
}


extern "C"
KSTATUS
WINAPI
Access_SetPrintPath(
UPDATE_PRINT_PATH1*    update_path
					   )
{
	if (update_path->PrintPath[0] == 0)
		;//return STATUS_INVALID_PARAMETERS;
	return db.TH_DB_SetPrintPicPath(update_path->nTaskID, update_path->PrintPath);
}

extern "C"
KSTATUS
WINAPI
Access_GetPrintPath(
UPDATE_PRINT_PATH1*    update_path
					)
{	
	return db.TH_DB_GetPrintPicPath(update_path->nTaskID, update_path->PrintPath);
}

extern "C"
KSTATUS
WINAPI
Access_SetTaskBarcode(UPDATE_BARCODE* update_barcode)
{
	return db.TH_DB_SetBarcode(update_barcode->nTaskID, update_barcode->sBarcode);
}

extern "C"
KSTATUS
WINAPI
Access_GetTaskBarcode(UPDATE_BARCODE* update_barcode)
{
	return db.TH_DB_GetBarcode(update_barcode->nTaskID, update_barcode->sBarcode);
}

extern "C"
KSTATUS
WINAPI
Access_FreeSearchRecordBuf(SEARCH_TASK &s_task1)
{
	if(s_task1.pSearchInfo != NULL)
		delete[] s_task1.pSearchInfo;
	s_task1.pSearchInfo = NULL;
	return STATUS_SUCCESS;
}


extern "C"
KSTATUS
WINAPI
Access_SearchRecord(
SEARCH_TASK*   s_task1
					   )
{
	KSTATUS       status;
	s_task1->pSearchInfoCounts = 0;
	s_task1->pSearchInfo       = NULL;
	

	status = db.TH_DB_SearchTask((SEARCH_TASK*)s_task1);
DBG_MSG("Counts = %d\n",s_task1->pSearchInfoCounts);
	if( s_task1->pSearchInfoCounts == 0 )
		return STATUS_NOT_FOUND_VALUE;
	

	s_task1->pSearchInfo = new TASK_INFO[s_task1->pSearchInfoCounts];
	if( s_task1->pSearchInfo == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;
	DBG_MSG("SearchInfoCounts:%d\n",s_task1->pSearchInfoCounts);
	ZeroMemory(s_task1->pSearchInfo,s_task1->pSearchInfoCounts * sizeof(TASK_INFO));

	status = db.TH_DB_SearchTask((SEARCH_TASK*)s_task1);

	if(status != STATUS_SUCCESS)
	{
		if(s_task1->pSearchInfo != NULL)
			delete[] s_task1->pSearchInfo;
		s_task1->pSearchInfoCounts = 0;

	}

	DBG_MSG("s_task1->pSearchInfoCounts:%d\n",s_task1->pSearchInfoCounts);
	return status;
}


extern "C"
KSTATUS
WINAPI
Access_FreeGetUserListBuf(USER_LIST_INFO1  &list_info)
{
	if(list_info.pUserInfo != NULL)
		delete[] list_info.pUserInfo;
	list_info.pUserInfo = NULL;
	return STATUS_SUCCESS;

}

extern "C"
KSTATUS
WINAPI
Access_GetUserList(
USER_LIST_INFO1*  list_info
					  )
{
	KSTATUS   status;

	list_info->nUserCounter = 0;
	 list_info->pUserInfo = NULL;

	db.TH_DB_GetUserInfo(NULL, (unsigned int*)&list_info->nUserCounter);
	if( list_info->nUserCounter == 0 )
		return STATUS_NOT_FOUND_VALUE;
	list_info->pUserInfo = new USER_INFO[list_info->nUserCounter];
	if( list_info->pUserInfo == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	ZeroMemory(list_info->pUserInfo,list_info->nUserCounter * sizeof(USER_INFO));

	status = db.TH_DB_GetUserInfo(list_info->pUserInfo, (unsigned int*)&list_info->nUserCounter);	
	if( status != STATUS_SUCCESS )
	{
		if(NULL != list_info->pUserInfo)
			delete[] list_info->pUserInfo;
	}
	return status;

}


extern "C"
KSTATUS
WINAPI
Access_FreeGetDeptAndPosInfoBuf(
Dept_Info &pDeptInfo,
Pos_Info &pPosInfo
								)
{
	if(pDeptInfo.pDept != NULL)
		delete[] pDeptInfo.pDept;
	pDeptInfo.pDept = NULL;
	if(pPosInfo.pPos != NULL)
		delete[] pPosInfo.pPos;
	pPosInfo.pPos = NULL;
	return STATUS_SUCCESS;

}

extern "C"
KSTATUS
WINAPI
Access_GetDeptAndPosInfo(
Dept_Info*    pDeptInfo,
Pos_Info*     pPosInfo
					)
{

	KSTATUS   status;

	pDeptInfo->nCount = 0;
	pDeptInfo->pDept = NULL ;
	pPosInfo->nCount = 0 ;
	pPosInfo->pPos = NULL;

	db.TH_DB_GetDeptInfo(NULL, (unsigned int*)&pDeptInfo->nCount);
	if( pDeptInfo->nCount == 0 )
		return STATUS_NOT_FOUND_VALUE;
	pDeptInfo->pDept = new Dept_Pos_Info[pDeptInfo->nCount];
	if( pDeptInfo->pDept == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	ZeroMemory(pDeptInfo->pDept,sizeof(Dept_Pos_Info) * pDeptInfo->nCount);

	status = db.TH_DB_GetDeptInfo(pDeptInfo->pDept, (unsigned int*)&pDeptInfo->nCount);

	if(status != STATUS_SUCCESS)
		return status;

	db.TH_DB_GetPosInfo(NULL, (unsigned int*)&pPosInfo->nCount);
	if( pPosInfo->nCount == 0 )
		return STATUS_NOT_FOUND_VALUE;
	pPosInfo->pPos = new Dept_Pos_Info[pPosInfo->nCount];
	if( pPosInfo->pPos == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	ZeroMemory(pPosInfo->pPos,pPosInfo->nCount * sizeof(Dept_Pos_Info));

	status = db.TH_DB_GetPosInfo(pPosInfo->pPos, (unsigned int*)&pPosInfo->nCount);
	return status;
}

extern "C"
KSTATUS
WINAPI
Access_UserAdd(
USER_ADD*         pUserAdd
					  )
{
	KSTATUS   status;
	PCHECK_ACCOUNT    check_account = (PCHECK_ACCOUNT)pUserAdd;

	status = db.TH_DB_CheckAccount(*check_account);
	if(status == STATUS_LOGIN_USER_EXIST)
	{
		//AfxMessageBox("用户已存在");
		//DBG_MSG("status = %x\n",status);
		return status;
	}
		
	status = db.TH_DB_AddUser(*pUserAdd);

	return status;

}

extern "C"
KSTATUS
WINAPI
Access_DelUser(
USER_DEL*   user_del
				  )
{
	return db.TH_DB_DelUser(*user_del);
}

extern "C"
KSTATUS
WINAPI
Access_ModifyPwd(
USER_UPC* upc
			   )
{
	return db.TH_DB_ModifyPwd(upc->UserName, upc->OldPsd, upc->NewPsd);
}

extern "C"
KSTATUS
WINAPI
Access_UserLogin(
USER_LOGIN*         plogin_par,
int *                UserType
			   )
{		
	return db.TH_DB_Login(plogin_par,UserType);	
}

extern "C"
KSTATUS
WINAPI
Access_ModifyUserInfo(
USER_INFO*         pUser_Info
				 )
{		
	return db.TH_DB_ModifyUserInfo(pUser_Info);	
}


extern "C"
KSTATUS
WINAPI
Access_GetLisInfo(
LIS_SET_INFO* LisInfo
					 )
{
	KSTATUS         status;
	SENDTO_LIS_ID   sLisID = {0};

	sLisID.SendtoLisID = LisInfo->nTaskID;
	status = db.TH_DB_GetLISList((SENDTO_LIS_ID*)&sLisID);
	if( status == STATUS_SUCCESS )
		memcpy(&LisInfo->TaskInfo.main_info, &sLisID.LisInfo, sizeof(TASK_INFO));

	return status;

}

extern "C"
KSTATUS
WINAPI
Access_GetLastTaskId(ULONG *nID)
{
    KSTATUS   status = STATUS_INVALID_PARAMETERS;
    if (nID == NULL)
    {
        return status;
    }
    status = db.TH_DB_GetLastTaskId(nID);

    return status;
}
