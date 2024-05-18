#ifndef _UWVAR_INCLUDE
#define _UWVAR_INCLUDE

#pragma once
#include "BuildHelper.h"
#include "CConfigSet.h"
#pragma comment(lib,"..\\..\\lib\\CConfigSet.lib")

#define INDEX_BUF_LEN              16
#define UDC_MAXGRADE         7

typedef struct udcinfo
{
	TCHAR m_ctsItemCode[INDEX_BUF_LEN];
	TCHAR m_ctsItemName[INDEX_BUF_LEN];
	BOOL  m_bGradeEnable[UDC_MAXGRADE];
	TCHAR m_ctsGradeName[UDC_MAXGRADE][INDEX_BUF_LEN];
	int   m_nGradeThreshold[UDC_MAXGRADE];
	TCHAR m_ctsGradeValue[UDC_MAXGRADE][INDEX_BUF_LEN];
	TCHAR m_ctsGradeUnit[UDC_MAXGRADE][INDEX_BUF_LEN];
}UW2000_UDC_INFO;

typedef struct _PCR_ACR
{
	char pro[64];
	char cre[64];
	char mca[64];
}PCR_ACR;


class CUWVAr
{
public:
	CUWVAr(void);
	virtual ~CUWVAr(void);
public:
	
	void     GetConfigInfo(PGET_US_INFO  pus_cfg,PGET_UDC_INFO pudc_cfg);
	void     BuildBaseInfoVarInfo(PTASK_INFO task_info, PSHELL_VAR_INFO var_info);
	void     BuildBaseInfoVarInfo(GET_US_INFO &us_cfg, PTASK_INFO task_info, PSHELL_VAR_INFO var_info);
	void     BuildUsInfoVarInfo(GET_US_INFO &us_cfg, US_INFO &us_info, PSHELL_VAR_INFO var_info, ULONG &nCounter, BOOL	 bMale, BOOL bUseOrgFormate = FALSE);
	void     BuildUdcInfoVarInfo(GET_UDC_INFO &udc_cfg,UDC_INFO &udc_info, PSHELL_VAR_INFO var_info, ULONG &nCounter,BOOL bUseOrgFormate = FALSE);
	void     ParseImagePath(PCHAR  pImagePath, ULONG nTaskID,PSHELL_VAR_INFO var_info, ULONG &nImageCounter);
	BOOL     BuildVarArrayforGroupPrint(PULONG  pTaskIDArray,ULONG  nTaskCounter,PSHELL_VAR_INFO &var_info,ULONG &nVarCounter, ULONG &nUnitCounterPerGroup);
	ULONG    GetVarCounterByGroupPrint(PGET_US_INFO  pUsCfgInfo,PGET_UDC_INFO pUdcCfgInfo);
	BOOL     BuildShellVarInfoByGroupPrint(PSHELL_VAR_INFO var_info,ULONG  nTaskID, GET_US_INFO &us_cfg, GET_UDC_INFO &udc_cfg);
	void     ErrRBCImagePath(ULONG nTaskID,PSHELL_VAR_INFO var_info, ULONG &nImageCounter);
	void     QCImagePath(PSHELL_VAR_INFO var_info, ULONG &nImageCounter);
	void	 InitUdcini();
	char*	 GetIntByUDCGrade(int type, char* grade);
	void	 BuildHelper_BaseInfoFormat_ACR_PCR(PTASK_INFO task_info, char* acr,char* pcr);
public:
	KSTATUS  BuildVarArray(PTASK_INFO  task_info, PSHELL_VAR_INFO &var_info,ULONG &nVarCounter,BOOL bUseOrgFormate = FALSE);
	void     FreeVarArray(PSHELL_VAR_INFO  var_info);
	void     GetRange(PCHAR pRange, PCHAR pName);
	UW2000_UDC_INFO m_UdcitemInfo[15];

};

#endif