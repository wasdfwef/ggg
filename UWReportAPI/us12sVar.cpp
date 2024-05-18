#include "StdAfx.h"
#include "us12sVar.h"
#include "BuildHelper.h"
#include <MALLOC.H>
#include <stdio.h>
#include <SHLWAPI.H>
#include <STDLIB.H>
#include "DbgMsg.h"
#define  BASE_INFO_COUNTER  21


#define  UL_STR      "/uL"
#define  HPF_STR     "HPF"


KSTATUS  CUS12sVar::BuildVarArray(FULL_TASK_INFO_1  &task_info, PSHELL_VAR_INFO &var_info, ULONG &nVarCounter, PUS_CFG_INFO1  pUsCfgInfo,PUDC_CFG_INFO1 pUdcCfgInfo,BOOL bUseOrgFormate)
{
	int            j;
	ULONG          k;
	US_CFG_INFO1   us_cfg = {0};
	UDC_CFG_INFO1  udc_cfg = {0};


	nVarCounter = 0;

	if( pUsCfgInfo == NULL )
		THInterface_QueryDBConfigInfo(US_INFO_TYPE, (PCHAR)&us_cfg, sizeof(us_cfg));
	else
		memcpy(&us_cfg, pUsCfgInfo, sizeof(us_cfg));

	if( pUdcCfgInfo == NULL )
		THInterface_QueryDBConfigInfo(UDC_INFO_TYPE,(PCHAR)&udc_cfg, sizeof(udc_cfg));
	else
		memcpy(&udc_cfg, pUdcCfgInfo, sizeof(udc_cfg));


	for( j = 0 ; j < 45 ; j ++ )
	{
		if( us_cfg.par[j].bIsDelete == FALSE )
			nVarCounter ++;
		if( j < 16 && udc_cfg.par[j].bIsDelete == FALSE )
			nVarCounter ++;
		
	}

	nVarCounter <<= 2;
	nVarCounter += BASE_INFO_COUNTER + 4;

	if( task_info.MainInfo1.sPrintImage[0] )
		nVarCounter += 3;

	var_info = (PSHELL_VAR_INFO)malloc(sizeof(SHELL_VAR_INFO) * nVarCounter);
	if( var_info == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	nVarCounter = 0;

	ZeroMemory(var_info, sizeof(SHELL_VAR_INFO) * nVarCounter);
	BuildBaseInfoVarInfo(task_info.MainInfo1, var_info, task_info);
	nVarCounter += BASE_INFO_COUNTER;
	BuildUsInfoVarInfo(us_cfg,task_info.us1, &var_info[BASE_INFO_COUNTER], k, task_info.MainInfo1.nSex == 2,bUseOrgFormate);
	nVarCounter += k;
	BuildUdcInfoVarInfo(udc_cfg,task_info.udc1, &var_info[BASE_INFO_COUNTER+k], k,bUseOrgFormate);
	nVarCounter += k;
	if( task_info.MainInfo1.sPrintImage[0] )
	{
		ULONG nImageCounter;

		ParseImagePath(task_info.MainInfo1.sPrintImage, task_info.MainInfo1.nID,&var_info[nVarCounter], nImageCounter);
		nVarCounter += nImageCounter;
	}

	return STATUS_SUCCESS;
}

void  CUS12sVar::ParseImagePath(PCHAR  pImagePath, ULONG nTaskID,PSHELL_VAR_INFO var_info, ULONG &nImageCounter)
{
	char   FullGrapPath[MAX_PATH], TaskDir[40];
	ULONG  j;
	PCHAR  p;
	ULONG  nIndex = 0;
	
	nImageCounter = 0;
	p = pImagePath;

	sprintf(TaskDir, "%u", nTaskID);
	GetModuleFileName(NULL, FullGrapPath, MAX_PATH);
	PathRemoveFileSpec(FullGrapPath);
	PathAppend(FullGrapPath, "RecGrapReslut");
	PathAppend(FullGrapPath, TaskDir);

	try
	{
		for( j = 0 ; j < MAX_PATH ; j ++ )
		{
			if( p[j] == 0 )
				break;

			if( p[j] == '|')
			{
				p[j] = 0;
				
				var_info[nImageCounter].nUnitType = PIC_UNIT_TYPE;
				sprintf(var_info[nImageCounter].VarValue, "%s\\%s", FullGrapPath, p);
				DBG_MSG("ParseImagePath->Path = %s\n",var_info[nImageCounter].VarValue);
				p    = &p[j+1];
				nImageCounter++;
				j = 0;
			}

		}
		
	}
	catch (...)
	{
		nImageCounter = 0;
	}
}


const char OldUnit[][6] = {{"year"}, {"mon"}, {"day"}};

void  CUS12sVar::BuildBaseInfoVarInfo(MAIN_INFO1 &base_info, PSHELL_VAR_INFO var_info, FULL_TASK_INFO_1 &full_task_info)
{
	char  temp[128], temp1[128], m_sIniFilePath[MAX_PATH], m_LanFilePath[MAX_PATH];
	int   bPrintComment = 0;

    GetModuleFileName(NULL,m_sIniFilePath,MAX_PATH);
	PathRemoveFileSpec(m_sIniFilePath);
	strcpy(m_LanFilePath, m_sIniFilePath);
	PathAppend(m_sIniFilePath, "conf.ini");
	PathAppend(m_LanFilePath, "2052.ini");
	
	
	bPrintComment = GetPrivateProfileInt("comment","print",0,m_sIniFilePath);

	if( bPrintComment == 0 )
		base_info.sComment[0] = 0;


	BuildHelper_BaseInfoFormat(base_info.sName, 1, var_info[0]);
	BuildHelper_BaseInfoFormat(base_info.sBirthday, 2, var_info[1]);
	BuildHelper_BaseInfoFormat(base_info.sSex, 3, var_info[2]);
//	BuildHelper_BaseInfoFormat(base_info.sDoctor, 4, var_info[3]);
	BuildHelper_BaseInfoFormat(base_info.sCompany, 5, var_info[4]);
	BuildHelper_BaseInfoFormat(base_info.sDepartment, 6, var_info[5]);
	BuildHelper_BaseInfoFormat(base_info.sLaboratorian, 7, var_info[6]);
	BuildHelper_BaseInfoFormat(base_info.sReporter, 8, var_info[7]);
	BuildHelper_BaseInfoFormat(base_info.sDoctor, 9, var_info[8]);
	BuildHelper_BaseInfoFormat(base_info.sComment, 10, var_info[9]);
	//BuildHelper_BaseInfoFormat(base_info.sComment, 11, var_info[10]);
		
	BuildHelper_BaseInfoFormat(base_info.sAgeunit, 12, var_info[11]);
	BuildHelper_BaseInfoFormat(base_info.sSickBed, 13, var_info[12]);
	BuildHelper_BaseInfoFormat(base_info.sColor, 14, var_info[13]);
	BuildHelper_BaseInfoFormat(base_info.sTransparency, 15, var_info[14]);
	BuildHelper_BaseInfoFormat(base_info.sSendData, 16, var_info[15]);
	BuildHelper_BaseInfoFormat(base_info.sCheckdata, 17, var_info[16]);
	BuildHelper_BaseInfoFormat(base_info.sSubmitDoctor, 27, var_info[17]);

	for( int j = 0 ; j < 3 ; j++ )
	{
		GetPrivateProfileString("string", OldUnit[j], "", temp1, 128, m_LanFilePath);
		if( base_info.sAgeunit[0] == 0 )
			break;
		if( strcmp(base_info.sAgeunit, temp1) == 0 )
			break;

	}

	sprintf(temp, "%u %s", base_info.nAge, temp1);

	
	BuildHelper_BaseInfoFormat(temp, 18, var_info[18]);
	sprintf(temp, "%u", base_info.nSN);
	BuildHelper_BaseInfoFormat(temp, 19, var_info[19]);
	BuildHelper_BaseInfoFormat(full_task_info.udc1.sudc_SN, 20, var_info[20]);
}

#define  HPF  2

void  CUS12sVar::BuildUsInfoVarInfo(US_CFG_INFO1 &us_cfg,US_INFO_1 &us_info, PSHELL_VAR_INFO var_info, ULONG &nCounter, BOOL	 bMale,BOOL bUseOrgFormate)
{
	int            j;
	char           temp[1024], str[40], Range[100];
	float          Reslut;


	nCounter = 0;

	for( j = 0 ; j < 45 ; j ++ )
	{
		if( us_cfg.par[j].bIsDelete == FALSE )
		{
			temp[0] = 0;

			Reslut = (float)us_info.us_d1[j].us_res;


			if( us_cfg.par[j].nUnit == HPF )
			{
				if(us_cfg.par[j].fArgument != 0.0f )
					Reslut /= us_cfg.par[j].fArgument;
				else
					Reslut = us_cfg.par[j].fArgument;

				strcpy(str, HPF_STR);
			}
			else
				strcpy(str,UL_STR);

			if( bMale )
				sprintf(Range, "%d-%d", us_cfg.par[j].nMin_normal_male, us_cfg.par[j].nMax_normal_male);
			else
				sprintf(Range, "%d-%d", us_cfg.par[j].nMin_normal_female, us_cfg.par[j].nMax_normal_female);

			//sprintf(temp, "     %s                      %.2f  %s  %s", US_NAME_DEF[j],Reslut, str,Range);

			BuildHelper_BaseInfoFormat(us_cfg.par[j].sLongName, 0, var_info[nCounter++],US_NAME_TEXT);
			sprintf(temp, "%.2f", Reslut);
			BuildHelper_BaseInfoFormat(temp, 0, var_info[nCounter++],US_VALUE_TEXT);
			BuildHelper_BaseInfoFormat(Range, 0, var_info[nCounter++],SRANGE_VALUE_TEXT);
			BuildHelper_BaseInfoFormat(str, 0, var_info[nCounter++],SPARTICLE_UNIT_TEXT);
			
		}
	}
}

void  CUS12sVar::BuildUdcInfoVarInfo(UDC_CFG_INFO1 &udc_cfg, UDC_INFO_1 &udc_info, PSHELL_VAR_INFO var_info, ULONG &nCounter, BOOL bUseOrgFormate)
{
	int             j;
	char            temp[1024];

	nCounter = 0;

	for( j = 0 ; j < (int)udc_cfg.nUsCounts ; j ++ )
	{
		if( udc_cfg.par[j].bIsDelete == FALSE )
		{
			temp[0] = 0;

			BuildHelper_BaseInfoFormat(udc_cfg.par[j].sLongName, 0, var_info[nCounter++], UDC_NAME_TEXT);
			if( udc_info.udc_d1[j].udc_res[0] == 0 )
				strcpy(temp,"-");
			else
				strcpy(temp, udc_info.udc_d1[j].udc_res);

			BuildHelper_BaseInfoFormat(temp, 0, var_info[nCounter++], UDC_VALUE_TEXT);

			if( j == 4 )
				strcpy(temp, "6-7");
			else if( j == 5 )
				strcpy(temp, "1.010-1.025");
			else
				strcpy(temp, "-");

			BuildHelper_BaseInfoFormat(temp, 0, var_info[nCounter++], DRANGE_VALUE_TEXT);
			
		}
	}

}



void CUS12sVar::RemoveSpace(PCHAR  pValue)
{
	int          nLen, nIndex = 0;
	char         t[MAX_PATH] = "";
	BOOLEAN      bRemoved = FALSE;

	strcpy(t, pValue);
	nLen = strlen(t);
	ZeroMemory(pValue, nLen);
	pValue[0] = 32;

	for( int j = 0 ; j < nLen ; j ++ )
	{
		if( t[j] != 32 )
			goto vaild_str;

		if( j + 1 >= nLen )
			break;

		if( pValue[nIndex] != 32 )
		{
			if( t[j+1] != 32 )
				goto vaild_str;
		}
		continue;

vaild_str:
		pValue[nIndex] = t[j];
		nIndex ++;
	}

}


void  CUS12sVar::FreeVarArray(PSHELL_VAR_INFO  var_info)
{
	free(var_info);
}



BOOL CUS12sVar::BuildVarArrayforGroupPrint(PULONG  pTaskIDArray, ULONG  nTaskCounter,PSHELL_VAR_INFO &var_info,ULONG &nVarCounter, ULONG &nUnitCounterPerGroup, PUS_CFG_INFO1  pUsCfgInfo,PUDC_CFG_INFO1 pUdcCfgInfo)
{
	int              j;
	BOOL             bRet;
	ULONG            nShellCounter;
	US_CFG_INFO1     us_cfg = {0};
	UDC_CFG_INFO1    udc_cfg = {0};
	PSHELL_VAR_INFO  pShellVarInfo;
	
	
	nVarCounter = 0;
	
	if( pUsCfgInfo == NULL )
		THInterface_QueryDBConfigInfo(US_INFO_TYPE, (PCHAR)&us_cfg, sizeof(us_cfg));
	else
		memcpy(&us_cfg, pUsCfgInfo, sizeof(us_cfg));
	
	if( pUdcCfgInfo == NULL )
		THInterface_QueryDBConfigInfo(UDC_INFO_TYPE,(PCHAR)&udc_cfg, sizeof(udc_cfg));
	else
		memcpy(&udc_cfg, pUdcCfgInfo, sizeof(udc_cfg));

	nUnitCounterPerGroup =  GetVarCounterByGroupPrint(&us_cfg, &udc_cfg);
	if( nUnitCounterPerGroup == 0 )
		return FALSE;

	nShellCounter = nUnitCounterPerGroup * nTaskCounter;
	pShellVarInfo = (PSHELL_VAR_INFO)malloc(sizeof(SHELL_VAR_INFO) *nShellCounter);

	if( pShellVarInfo == NULL )
		return FALSE;

	for( j = 0 ; j < nTaskCounter ; j ++ )
	{
		bRet = BuildShellVarInfoByGroupPrint(&pShellVarInfo[j * nUnitCounterPerGroup], pTaskIDArray[j], us_cfg, udc_cfg);
		if( bRet == FALSE )
			break;
	}

	if( bRet == FALSE )
		free(pShellVarInfo);
	else
	{
		nVarCounter = nShellCounter;
		var_info    = pShellVarInfo;
	}
	
	return bRet;
}

#define   G_BASE_COUNTER  5 

ULONG CUS12sVar::GetVarCounterByGroupPrint(PUS_CFG_INFO1  pUsCfgInfo,PUDC_CFG_INFO1 pUdcCfgInfo)
{
	ULONG   nCounter = 0;

	for( ULONG j = 0 ; j < pUsCfgInfo->nUsCounts ; j ++ )
	{
		if( pUsCfgInfo->par[j].bIsDelete )
			continue;

		nCounter += 2;
	}

	for( j = 0 ; j < pUdcCfgInfo->nUsCounts ; j ++ )
	{
		if( pUdcCfgInfo->par[j].bIsDelete )
			continue;

		nCounter += 2;
	}
	

	return  nCounter + G_BASE_COUNTER;

}


BOOL  CUS12sVar::BuildShellVarInfoByGroupPrint(PSHELL_VAR_INFO var_info, ULONG  nTaskID, US_CFG_INFO1 &us_cfg, UDC_CFG_INFO1 &udc_cfg)
{
	char              temp[MAX_PATH];
	ULONG             nIndex = 0;
	KSTATUS           status;
	FULL_TASK_INFO_1  task_info = {0};


	task_info.MainInfo1.nID = nTaskID;
	task_info.TypeInfo      = ALL_INFO_TYPE;
	
	status = THInterface_ReadRecordInfo(task_info.TypeInfo, (PCHAR)&task_info, sizeof(task_info));
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("BuildShellVarInfoByGroupPrint->ReadRecord Error...%x", status);
		return FALSE;
	}

	//构建基本信息
	sprintf(temp, "%u", task_info.MainInfo1.nSN);
	BuildHelper_BaseInfoFormat(temp, 19, var_info[nIndex++]);
	BuildHelper_BaseInfoFormat(task_info.MainInfo1.sCheckdata, 17, var_info[nIndex++]);

	//构建us信息

	for( ULONG j = 0 ; j < us_cfg.nUsCounts ; j ++ )
	{
		if( us_cfg.par[j].bIsDelete )
			continue;

		BuildHelper_BaseInfoFormat(us_cfg.par[j].sLongName, 0, var_info[nIndex++],US_NAME_TEXT);
		sprintf(temp, "%.2f", task_info.us1.us_d1[j].us_res);
		BuildHelper_BaseInfoFormat(temp, 0, var_info[nIndex++],US_VALUE_TEXT);
	}

	//构建udc信息

	for( j = 0 ; j < udc_cfg.nUsCounts ; j ++ )
	{
		if( udc_cfg.par[j].bIsDelete )
			continue;

		BuildHelper_BaseInfoFormat(udc_cfg.par[j].sLongName, 0, var_info[nIndex++], UDC_NAME_TEXT);

		if( task_info.udc1.udc_d1[j].udc_res[0] == 0 )
			strcpy(temp,"-");
		else
			strcpy(temp, task_info.udc1.udc_d1[j].udc_res);

		BuildHelper_BaseInfoFormat(temp, 0, var_info[nIndex++], UDC_VALUE_TEXT);
	}

	return TRUE;
}