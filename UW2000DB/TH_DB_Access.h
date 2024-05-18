#pragma once




#include "..\..\..\inc\THDBUW_Access.h"

#include "DbgMsg.h"


#include <string>
using namespace std;

#define OPEN_ACCESS2000_CONNECTING  "Provider=Microsoft.Jet.OLEDB.4.0;Data Source="
#define OPEN_ACCESS2007_CONNECTING  "Provider=Microsoft.ACE.OLEDB.12.0;Data Source="

#define OPEN_ACCESS_BY_PASSWORD     ";Jet OLEDB:Database Password="
#define OPEN_ACCESS_BY_SECURITY     ";Persist Security Info=False;"




class TH_DB_Access
{
public:
	TH_DB_Access(void);
	virtual ~TH_DB_Access(void);


public:

	ADODB::_ConnectionPtr	m_Connection;
	char   m_nConfig_IniFile[MAX_PATH];
	BOOL  bInitialize;

	void SetInitializestatus(BOOL bNeedInitialize);

public:
	// 打开ACCESS文件 OPEN_ACCESS_FILE
	KSTATUS  Open_Connect(OPEN_ACCESS_FILE_PAR open_access_file_par);
	// 关闭链接
	void Close_Connect(void);

	BOOL GetConfigIniFilePath();
	// 执行
	KSTATUS Excute(EXECUTE_PAR excute_par);

	KSTATUS CheckTaskStates(BOOL BIsTaday);




	// 打开表
	KSTATUS Open_Rs(OPEN_RS_PAR*  open_rs_par);

	// 关闭表
	void Close_Rs(ADODB::_RecordsetPtr Rs);


	// 登录
	KSTATUS Login(PUSER_LOGIN login_par,int * UserType );

	// 检测账号
	KSTATUS CheckAccount(CHECK_ACCOUNT check_account);

	// 显示用户
	KSTATUS  GetUserInfo(USER_INFO* pUserInfo,unsigned int* pnSize);

	KSTATUS  GetDeptInfo(Dept_Pos_Info* pUserInfo,unsigned int* pnSize);

	KSTATUS  GetPosInfo(Dept_Pos_Info* pUserInfo,unsigned int* pnSize);

	KSTATUS  GetAllTaskInfo(PULONG pAllTaskID,unsigned int* pnSize );

	KSTATUS GetPwd(const string &UserName, string &Pwd);

	KSTATUS ModifyPwdImp(const string &UserName, const string &NewPwd);

	KSTATUS  ModifyPwd(const string &UserName, const string &OldPwd, const string &NewPwd);

	KSTATUS  ModifyUserInfo(PUSER_INFO pUser_Info);


	// 新建用户
	KSTATUS  AddUser(USER_ADD user_add);

	// 删除用户
	KSTATUS  DelUser(USER_DEL user_del);


	// 获得LIS状态
	KSTATUS GetLisStatus(int nID,unsigned int* pStatus);

	// 设置LIS状态
	KSTATUS SetLisStatus(int nID,unsigned int nStatus);


	// 获得PRINT状态
	KSTATUS GetPrintStatus(int nID,unsigned int* pStatus);

	// 设置PRINT状态
	KSTATUS SetPrintStatus(int nID,unsigned int nStatus);

	// 增加检测任务

	KSTATUS CheckNewStatus(unsigned int* pID,bool bIsQC,ULONG nStatus);

	KSTATUS AddTask(TASK_ADD* ptask_add);

	// 强制新增加任务
	KSTATUS AddTaskMust(TASK_ADD* ptask_add);

	// 删除检测任务
	KSTATUS DelTask(int* pnDelID,int nCounts,bool bIsqc );

	// 删除全部
	KSTATUS DelAllTask(bool isAll = false);

	// 显示任务
	KSTATUS GetTaskInfo(bool bIsSelectById,int nID,TASK_INFO* pTaskInfo,unsigned int* pnSize);

	//
	
	//
	KSTATUS GetQcTaskInfo(QCTypeEnum QcType,QC_INFO*  qc_task_info,unsigned int* pnSize);

	//
	KSTATUS GetUsQcTaskInfo(UsQcEnum  UsQcType,QC_INFO*  qc_task_info,unsigned int* pnSize);

	//
	KSTATUS GetUdcQcTaskInfo(UsQcEnum  UsQcType,QC_INFO*  qc_task_info,unsigned int* pnSize);

	// 重置任务
	KSTATUS ReSetTask(int nID,TestTypeEnum nTestType,ULONG nStatus);

	// 设置保存图片路径
	KSTATUS SetPicPath(SET_PIC_PATH set_pic_path);

	// 获得指定类型信息
	KSTATUS GetInfo(TASK_INFO* pTaskInfo,ULONG nID,TYPE_INFO type);

	// 设置指定类型信息
	KSTATUS SetInfo(TASK_INFO* pTaskInfo,ULONG nID,TYPE_INFO type);


	// 获得US信息
	KSTATUS GetUsInfo(GET_US_INFO* pGetUsInfo);

	// 写入US信息
	KSTATUS SetUsInfo(SET_US_INFO set_us_info);


	// 获得UDC信息
	KSTATUS GetUdcInfo(GET_UDC_INFO* pGetUdcInfo);

	// 写入UDC信息
	KSTATUS  SetUdcInfo(SET_UDC_INFO set_udc_info);



	// 任务搜索
	KSTATUS  SearchTask(SEARCH_TASK* pSearch_normal_task );

	KSTATUS  UpdateTaskStatus(ULONG TaskID,KSTATUS state);

	KSTATUS  UpdateAlarm(ULONG TaskID,int nAlarm);

	// 设置打印图片路径
	KSTATUS  SetPrintPicPath(int nID,char* pPrintPicPath);
	// 获取打印图片路径
	KSTATUS  GetPrintPicPath(int nID,char* pPrintPicPath);
	// 获得LIS信息
	KSTATUS GetLISList(SENDTO_LIS_ID* pSendto_lis_id);	

	// 设置记录条码
	KSTATUS  SetBarcode(int nID, char* pBarcode);
	// 获取记录条码
	KSTATUS  GetBarcode(int nID, char* pBarcode);

    // 获取最后一条记录ID
    KSTATUS GetLastTaskId(ULONG *nID);

	int GetImportType(void);

	std::string GetFilePath(void);

	/*KSTATUS AutoImport(UDC_IMPORT_INFO* pUdcImportInfo,bool* pbIsNew,unsigned int* pID);

	KSTATUS AutoImportUdcData(UDC_IMPORT_INFO* pAutoImportUdcInfo,bool* pbIsNew,unsigned int* pID);*/

	_variant_t GetInterfacePtrEx(void);

	_variant_t GetLoginPassword(ADODB::_RecordsetPtr Rs);

	_variant_t GetLoginGroup(ADODB::_RecordsetPtr Rs);

	bool IsEOF(ADODB::_RecordsetPtr Rs);

	bool IsBOF(ADODB::_RecordsetPtr Rs);


};
