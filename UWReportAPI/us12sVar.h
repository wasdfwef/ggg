#ifndef   _US12S_VAR_INCLUDE
#define   _US12S_VAR_INCLUDE

#include "..\..\inc\THFrameDef.h"
#include "..\..\inc\THReportAPI.h"

class  CUS12sVar
{
	void     BuildUdcInfoVarInfo(UDC_CFG_INFO1 &udc_cfg,UDC_INFO_1 &udc_info, PSHELL_VAR_INFO var_info, ULONG &nCounter,BOOL bUseOrgFormate = FALSE);
	void     BuildUsInfoVarInfo(US_CFG_INFO1 &us_cfg, US_INFO_1 &us_info, PSHELL_VAR_INFO var_info, ULONG &nCounter, BOOL	 bMale, BOOL bUseOrgFormate = FALSE);
	void     BuildBaseInfoVarInfo(MAIN_INFO1 &base_info, PSHELL_VAR_INFO var_info, FULL_TASK_INFO_1 &full_task_info);
	void     BuildUsLineInfo(PCHAR  pOutputBuffer,ULONG nIndex, PCHAR pCellName, PCHAR  pUnitName, float Reslut, PCHAR pRange);
	void     ParseImagePath(PCHAR  pImagePath, ULONG nTaskID,PSHELL_VAR_INFO var_info, ULONG &nImageCounter);
	void     RemoveSpace(PCHAR  pValue);
	BOOL     BuildShellVarInfoByGroupPrint(PSHELL_VAR_INFO var_info,ULONG  nTaskID, US_CFG_INFO1 &us_cfg, UDC_CFG_INFO1 &udc_cfg);
	ULONG    GetVarCounterByGroupPrint(PUS_CFG_INFO1  pUsCfgInfo,PUDC_CFG_INFO1 pUdcCfgInfo);
	
public:
	KSTATUS  BuildVarArray(FULL_TASK_INFO_1  &task_info, PSHELL_VAR_INFO &var_info,ULONG &nVarCounter, PUS_CFG_INFO1  pUsCfgInfo,PUDC_CFG_INFO1 pUdcCfgInfo, BOOL bUseOrgFormate = FALSE);
	BOOL     BuildVarArrayforGroupPrint(PULONG  pTaskIDArray, ULONG  nTaskCounter,PSHELL_VAR_INFO &var_info,ULONG &nVarCounter, ULONG &nUnitCounterPerGroup,PUS_CFG_INFO1  pUsCfgInfo,PUDC_CFG_INFO1 pUdcCfgInfo);
	void     FreeVarArray(PSHELL_VAR_INFO  var_info);

};



#endif