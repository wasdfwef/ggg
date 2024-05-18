#include "StdAfx.h"
#include "UWVAr.h"

#include <MALLOC.H>
#include <stdio.h>
#include <SHLWAPI.H>
#include <STDLIB.H>
#include "DbgMsg.h"
#include "..\..\inc\UW2000DB.h"


#define  BASE_INFO_COUNTER  35


#define  UL_STR      "个/μl"

#define  HP_STR      "HP"

CUWVAr::CUWVAr(void)
{

}

CUWVAr::~CUWVAr(void)
{
}

KSTATUS CUWVAr::BuildVarArray(PTASK_INFO task_info, PSHELL_VAR_INFO &var_info, ULONG &nVarCounter, BOOL bUseOrgFormate /*= FALSE*/)
{
	int            j;
	ULONG          k;
	nVarCounter = 0;
	GET_US_INFO        us_cfg = { 0 };
	GET_UDC_INFO       udc_cfg = { 0 };
	GetConfigInfo(&us_cfg, &udc_cfg);

	for (j = 0; j < MAX_US_COUNTS; j++)
	{
		if (us_cfg.par[j].bIsDelete == FALSE)
			nVarCounter++;
		if (j < MAX_UDC_COUNTS && udc_cfg.par[j].bIsDelete == FALSE)
			nVarCounter++;
	}

	nVarCounter <<= 2;
	nVarCounter += BASE_INFO_COUNTER + 10;

	if (task_info->main_info.sImage[0])
		nVarCounter += 20;

	var_info = (PSHELL_VAR_INFO)malloc(sizeof(SHELL_VAR_INFO) * nVarCounter);
	if (var_info == NULL)
		return STATUS_ALLOC_MEMORY_ERROR;

	//nVarCounter = 0;
	ZeroMemory(var_info, sizeof(SHELL_VAR_INFO) * nVarCounter);
	nVarCounter = 0;
	BuildBaseInfoVarInfo(us_cfg, task_info, var_info);
	nVarCounter += BASE_INFO_COUNTER;
	BuildUsInfoVarInfo(us_cfg, task_info->us_info, &var_info[BASE_INFO_COUNTER], k, task_info->pat_info.nSex == 2, bUseOrgFormate);
	nVarCounter += k;
	BuildUdcInfoVarInfo(udc_cfg, task_info->udc_info, &var_info[BASE_INFO_COUNTER + k], k, bUseOrgFormate);
	nVarCounter += k;
	ULONG nImageCounter;
	if (task_info->main_info.nAttribute != TestNormal && task_info->main_info.nAttribute != TestEP)//如果质控模式下打印
	{
		QCImagePath(&var_info[nVarCounter], nImageCounter);
		nVarCounter += nImageCounter;
		return STATUS_SUCCESS;
	}
	ErrRBCImagePath(task_info->main_info.nID, &var_info[nVarCounter], nImageCounter);
	nVarCounter += nImageCounter;
	if (task_info->main_info.sImage[0])
	{
		DBG_MSG("task_info->sImage=%s\n", task_info->main_info.sImage);

		ParseImagePath(task_info->main_info.sImage, task_info->main_info.nID, &var_info[nVarCounter], nImageCounter);
		nVarCounter += nImageCounter;
	}
	else
	{
		char sImage[] = { "040_0000.jpg|040_0001.jpg|040_0002.jpg|" };
		ParseImagePath(sImage, task_info->main_info.nID, &var_info[nVarCounter], nImageCounter);
		nVarCounter += nImageCounter;
	}


	return STATUS_SUCCESS;
}

void CUWVAr::FreeVarArray(PSHELL_VAR_INFO var_info)
{
	if (var_info)
	{
		free(var_info);
	}
}

const char OldUnit[][6] = { { "year" }, { "mon" }, { "day" } };
const char sex[][10] = { { "male" }, { "female" } };


void CUWVAr::BuildBaseInfoVarInfo(PTASK_INFO task_info, PSHELL_VAR_INFO var_info)
{
	char  temp[128], temp1[128], m_sIniFilePath[MAX_PATH], m_LanFilePath[MAX_PATH];
	int   bPrintComment = 0;

	GetModuleFileName(NULL, m_sIniFilePath, MAX_PATH);
	PathRemoveFileSpec(m_sIniFilePath);
	strcpy(m_LanFilePath, m_sIniFilePath);
	PathAppend(m_sIniFilePath, "conf.ini");
	PathAppend(m_LanFilePath, "2052.ini");

	/*bPrintComment = GetPrivateProfileInt("comment","print",0,m_sIniFilePath);

	if( bPrintComment == 0 )
	base_info.sComment[0] = 0;*/

	BuildHelper_BaseInfoFormat(task_info->pat_info.sName, 1, var_info[0]);
	BuildHelper_BaseInfoFormat(task_info->pat_info.dtBirthDate, 2, var_info[1]);

	if (task_info->pat_info.nSex == 2)
		GetPrivateProfileString("string", "female", "", temp1, 128, m_LanFilePath);
	else if (task_info->pat_info.nSex == 1)
		GetPrivateProfileString("string", "male", "", temp1, 128, m_LanFilePath);
	else
		GetPrivateProfileString("string", "unknown", "", temp1, 128, m_LanFilePath);


	BuildHelper_BaseInfoFormat(temp1, 3, var_info[2]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sDoctor, 27, var_info[3]);
	//BuildHelper_BaseInfoFormat(task_info->main_info.sCompany, 5, var_info[4]);
	BuildHelper_BaseInfoFormat(task_info->pat_info.sHospital, 5, var_info[4]);
	DBG_MSG("sDepartment = %s\n", task_info->main_info.sDepartment);
	BuildHelper_BaseInfoFormat(task_info->main_info.sDepartment, 6, var_info[5]);

	//BuildHelper_BaseInfoFormat(task_info->main_info.sLaboratorian, 7, var_info[6]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sReporter, 8, var_info[7]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sAuditor, 9, var_info[8]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sDiagnosis, 10, var_info[9]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sSuggestion, 11, var_info[10]);

	//GetPrivateProfileString("string", OldUnit[task_info->pat_info.nAgeUnit], "", temp1, 128, m_LanFilePath);
	if (task_info->pat_info.nAgeUnit == 1)
		GetPrivateProfileString("string", "mon", "", temp1, 128, m_LanFilePath);
	else if (task_info->pat_info.nAgeUnit == 2)
		GetPrivateProfileString("string", "day", "", temp1, 128, m_LanFilePath);
	else
		GetPrivateProfileString("string", "year", "", temp1, 128, m_LanFilePath);

	DBG_MSG("temp1 = %s,%d,%s\n", temp1, task_info->pat_info.nAgeUnit, m_LanFilePath);

	BuildHelper_BaseInfoFormat(temp1, 12, var_info[11]);
	BuildHelper_BaseInfoFormat(task_info->pat_info.sSickBed, 13, var_info[12]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sColor, 14, var_info[13]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sTransparency, 15, var_info[14]);
	SYSTEMTIME sys = {};
	GetLocalTime(&sys);
	sprintf_s(temp, "%4d-%02d-%02d %02d:%02d:%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	BuildHelper_BaseInfoFormat(/*task_info->main_info.dtDate*/temp, 16, var_info[15]);
	BuildHelper_BaseInfoFormat(task_info->main_info.dtDate, 17, var_info[16]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sDocimaster, 27, var_info[17]);

	//GetPrivateProfileString("string", OldUnit[task_info->pat_info.nAgeUnit], "", temp1, 128, m_LanFilePath);		

	if (task_info->pat_info.nAgeUnit == 1)
		GetPrivateProfileString("string", "mon", "", temp1, 128, m_LanFilePath);
	else if (task_info->pat_info.nAgeUnit == 2)
		GetPrivateProfileString("string", "day", "", temp1, 128, m_LanFilePath);
	else
		GetPrivateProfileString("string", "year", "", temp1, 128, m_LanFilePath);
	if (task_info->pat_info.nAge == 0)
		sprintf(temp, "  %s", temp1);
	else
		sprintf(temp, "%u %s", task_info->pat_info.nAge, temp1);
	BuildHelper_BaseInfoFormat(temp, 18, var_info[18]);

	sprintf(temp, "%u", task_info->main_info.nSN);
	BuildHelper_BaseInfoFormat(temp, 19, var_info[19]);

	BuildHelper_BaseInfoFormat(task_info->main_info.sCode, 20, var_info[20]);



	if (task_info->us_info.us_node[0].us_res != 0.0f && task_info->us_info.us_node[1].us_res != 0.0f)
	{
		CString  str = _T("");

		str.Format(_T("%.2f"), task_info->us_info.us_node[1].us_res / (task_info->us_info.us_node[0].us_res + task_info->us_info.us_node[1].us_res));

		//	sprintf(temp,"%.2f",str);
		BuildHelper_BaseInfoFormat(str.GetBuffer(), 25, var_info[21]);


		str = _T("");

		str.Format(_T("%.2f"), task_info->us_info.us_node[0].us_res / (task_info->us_info.us_node[0].us_res + task_info->us_info.us_node[1].us_res));


		BuildHelper_BaseInfoFormat(str.GetBuffer(), 26, var_info[22]);

	}

	GetPrivateProfileString("red", "ErrRbcValue", "", temp1, 128, m_sIniFilePath);
	BuildHelper_BaseInfoFormat(temp1, 28, var_info[23]);

	GetPrivateProfileString("red", "ErrRbcdiameter", "", temp1, 128, m_sIniFilePath);
	BuildHelper_BaseInfoFormat(temp1, 29, var_info[24]);
}

void CUWVAr::BuildBaseInfoVarInfo(GET_US_INFO &us_cfg, PTASK_INFO task_info, PSHELL_VAR_INFO var_info)
{
	char  temp[128], temp1[128], m_sIniFilePath[MAX_PATH], m_LanFilePath[MAX_PATH];
	int   bPrintComment = 0;

	GetModuleFileName(NULL, m_sIniFilePath, MAX_PATH);
	PathRemoveFileSpec(m_sIniFilePath);
	strcpy(m_LanFilePath, m_sIniFilePath);
	PathAppend(m_sIniFilePath, "conf.ini");
	PathAppend(m_LanFilePath, "2052.ini");

	/*bPrintComment = GetPrivateProfileInt("comment","print",0,m_sIniFilePath);

	if( bPrintComment == 0 )
	base_info.sComment[0] = 0;*/

	BuildHelper_BaseInfoFormat(task_info->pat_info.sName, 1, var_info[0]);
	BuildHelper_BaseInfoFormat(task_info->pat_info.dtBirthDate, 2, var_info[1]);

	if (task_info->pat_info.nSex == 2)
		GetPrivateProfileString("string", "female", "", temp1, 128, m_LanFilePath);
	else if (task_info->pat_info.nSex == 1)
		GetPrivateProfileString("string", "male", "", temp1, 128, m_LanFilePath);
	else
		GetPrivateProfileString("string", "unknown", "", temp1, 128, m_LanFilePath);


	BuildHelper_BaseInfoFormat(temp1, 3, var_info[2]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sDoctor, 27, var_info[3]);
	//BuildHelper_BaseInfoFormat(task_info->main_info.sCompany, 5, var_info[4]);
	BuildHelper_BaseInfoFormat(task_info->pat_info.sHospital, 5, var_info[4]);
	DBG_MSG("sDepartment = %s\n", task_info->main_info.sDepartment);
	BuildHelper_BaseInfoFormat(task_info->main_info.sDepartment, 6, var_info[5]);

	//BuildHelper_BaseInfoFormat(task_info->main_info.sLaboratorian, 7, var_info[6]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sReporter, 8, var_info[7]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sAuditor, 9, var_info[8]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sDiagnosis, 10, var_info[9]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sSuggestion, 11, var_info[10]);

	//GetPrivateProfileString("string", OldUnit[task_info->pat_info.nAgeUnit], "", temp1, 128, m_LanFilePath);
	if (task_info->pat_info.nAgeUnit == 1)
		GetPrivateProfileString("string", "mon", "", temp1, 128, m_LanFilePath);
	else if (task_info->pat_info.nAgeUnit == 2)
		GetPrivateProfileString("string", "day", "", temp1, 128, m_LanFilePath);
	else
		GetPrivateProfileString("string", "year", "", temp1, 128, m_LanFilePath);

	DBG_MSG("temp1 = %s,%d,%s\n", temp1, task_info->pat_info.nAgeUnit, m_LanFilePath);

	BuildHelper_BaseInfoFormat(temp1, 12, var_info[11]);
	BuildHelper_BaseInfoFormat(task_info->pat_info.sSickBed, 13, var_info[12]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sColor, 14, var_info[13]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sTransparency, 15, var_info[14]);
	SYSTEMTIME sys = {};
	GetLocalTime(&sys);
	sprintf_s(temp, "%4d-%02d-%02d %02d:%02d:%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	BuildHelper_BaseInfoFormat(/*task_info->main_info.dtDate*/temp, 16, var_info[15]);
	BuildHelper_BaseInfoFormat(task_info->main_info.dtDate, 17, var_info[16]);
	BuildHelper_BaseInfoFormat(task_info->main_info.sDocimaster, 27, var_info[17]);

	//GetPrivateProfileString("string", OldUnit[task_info->pat_info.nAgeUnit], "", temp1, 128, m_LanFilePath);		

	if (task_info->pat_info.nAgeUnit == 1)
		GetPrivateProfileString("string", "mon", "", temp1, 128, m_LanFilePath);
	else if (task_info->pat_info.nAgeUnit == 2)
		GetPrivateProfileString("string", "day", "", temp1, 128, m_LanFilePath);
	else
		GetPrivateProfileString("string", "year", "", temp1, 128, m_LanFilePath);
	if (task_info->pat_info.nAge == 0)
		sprintf(temp, "  %s", temp1);
	else
		sprintf(temp, "%u %s", task_info->pat_info.nAge, temp1);
	BuildHelper_BaseInfoFormat(temp, 18, var_info[18]);

	sprintf(temp, "%u", task_info->main_info.nSN);
	BuildHelper_BaseInfoFormat(temp, 19, var_info[19]);

	BuildHelper_BaseInfoFormat(task_info->main_info.sCode, 20, var_info[20]);

	//南京优源
	char acr[64], pcr[64];
	BuildHelper_BaseInfoFormat_ACR_PCR(task_info, acr, pcr);
	BuildHelper_BaseInfoFormat(acr, 40, var_info[27]);
	BuildHelper_BaseInfoFormat(pcr, 41, var_info[28]);

	if (task_info->us_info.us_node[0].us_res != 0.0f && task_info->us_info.us_node[1].us_res != 0.0f)
	{
		CString  str = _T("");

		str.Format(_T("%.2f"), task_info->us_info.us_node[1].us_res / (task_info->us_info.us_node[0].us_res + task_info->us_info.us_node[1].us_res));

		//	sprintf(temp,"%.2f",str);
		BuildHelper_BaseInfoFormat(str.GetBuffer(), 25, var_info[21]);


		str = _T("");

		str.Format(_T("%.2f"), task_info->us_info.us_node[0].us_res / (task_info->us_info.us_node[0].us_res + task_info->us_info.us_node[1].us_res));


		BuildHelper_BaseInfoFormat(str.GetBuffer(), 26, var_info[22]);

	}

	GetPrivateProfileString("red", "ErrRbcValue", "", temp1, 128, m_sIniFilePath);
	BuildHelper_BaseInfoFormat(temp1, 28, var_info[23]);

	GetPrivateProfileString("red", "ErrRbcdiameter", "", temp1, 128, m_sIniFilePath);
	BuildHelper_BaseInfoFormat(temp1, 29, var_info[24]);


	float rbc = 0.0f, drbc = 0.0f;
	for (int j = 0; j < MAX_US_COUNTS; j++)
	{
		if (strcmp(us_cfg.par[j].sShortName, "RBC") == 0)
		{
			rbc = task_info->us_info.us_node[j].us_res;
		}
		else if (strcmp(us_cfg.par[j].sShortName, "DRBC") == 0)
		{
			drbc = task_info->us_info.us_node[j].us_res;
		}
	}
	if (rbc == 0.0f && drbc == 0.0f)
	{
		sprintf_s(temp, "0");
		sprintf_s(temp1, "0");
	}
	else if (rbc != 0.0f && drbc == 0.0f)
	{
		sprintf_s(temp, "100");
		sprintf_s(temp1, "0");
	}
	else if (rbc == 0.0f && drbc != 0.0f)
	{
		sprintf_s(temp, "0");
		sprintf_s(temp1, "100");
	}
	else
	{
		sprintf_s(temp, "%.0f", rbc * 100 / (rbc + drbc));
		sprintf_s(temp1, "%.0f", drbc * 100 / (rbc + drbc));
	}
	BuildHelper_BaseInfoFormat(temp, 35, var_info[25]);
	BuildHelper_BaseInfoFormat(temp1, 36, var_info[26]);
}


#define  HPF  2

void CUWVAr::BuildUsInfoVarInfo(GET_US_INFO &us_cfg, US_INFO &us_info, PSHELL_VAR_INFO var_info, ULONG &nCounter, BOOL bMale, BOOL bUseOrgFormate /*= FALSE*/)
{
	int            j;
	char           temp1[1024], temp[1024], str[40], Range[100];
	float          Reslut;

	nCounter = 0;

	char ModPath[MAX_PATH] = { 0 }, szUnit[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, ModPath, MAX_PATH);
	PathRemoveFileSpec(ModPath);
	PathAppend(ModPath, "Config/Report.ini");
	GetPrivateProfileString("US", "unit", UL_STR, szUnit, MAX_PATH, ModPath);
	if (strlen(szUnit) == 0) strcpy(szUnit, UL_STR);

	float rbc = 0.0f, drbc = 0.0f;
	for (j = 0; j < MAX_US_COUNTS; j++)
	{
		if (us_cfg.par[j].bIsDelete == FALSE)
		{
			temp[0] = 0;
			//DBG_MSG("us_info.us_node[%d].us_res = %f\n",j,us_info.us_node[j].us_res);
			Reslut = (float)us_info.us_node[j].us_res;

			if (strcmp(us_cfg.par[j].sShortName, "RBC") == 0)
			{
				rbc = us_info.us_node[j].us_res;
			}
			else if (strcmp(us_cfg.par[j].sShortName, "DRBC") == 0)
			{
				drbc = us_info.us_node[j].us_res;
			}

			strcpy(str, szUnit);

			if (bMale)
				sprintf(Range, "%d-%d", us_cfg.par[j].nMin_normal_female, us_cfg.par[j].nMax_normal_female);
			else
				sprintf(Range, "%d-%d", us_cfg.par[j].nMin_normal_male, us_cfg.par[j].nMax_normal_male);
			//sprintf(temp, "     %s                      %.2f  %s  %s", US_NAME_DEF[j],Reslut, str,Range);

			BuildHelper_BaseInfoFormat(us_cfg.par[j].sLongName, 0, var_info[nCounter++], US_NAME_TEXT);


			DBG_MSG("%d %f\n", bMale, Reslut);
			if (bMale)
			{
				if (Reslut < us_cfg.par[j].nMin_normal_female && (Reslut < -0.0000001 || Reslut > 0.0000001))
					sprintf_s(temp1, "↓");
				else if (Reslut > us_cfg.par[j].nMax_normal_female && (Reslut < -0.0000001 || Reslut > 0.0000001))
					sprintf_s(temp1, "↑");
				else
					sprintf_s(temp1, "");

			}
			else
			{
				if (Reslut < us_cfg.par[j].nMin_normal_male && (Reslut < -0.0000001 || Reslut > 0.0000001))
					sprintf_s(temp1, "↓");
				else if (Reslut > us_cfg.par[j].nMax_normal_male && (Reslut < -0.0000001 || Reslut > 0.0000001))
					sprintf_s(temp1, "↑");
				else
					sprintf_s(temp1, "");
			}

			sprintf(temp, "%.2f%s", Reslut, temp1);

			BuildHelper_BaseInfoFormat(temp, 0, var_info[nCounter++], US_VALUE_TEXT);
			BuildHelper_BaseInfoFormat(Range, 0, var_info[nCounter++], SRANGE_VALUE_TEXT);
			BuildHelper_BaseInfoFormat(str, 0, var_info[nCounter++], SPARTICLE_UNIT_TEXT);

		}
	}

	for (j = 0; j < MAX_US_COUNTS; j++)
	{
		if (strcmp(us_cfg.par[j].sShortName, "UNCL1") == 0)
		{
			temp[0] = 0;
			//Reslut = (float)us_info.us_node[j].us_res;
			Reslut = (rbc + drbc);
			strcpy(str, szUnit);

			if (bMale)
				sprintf(Range, "%d-%d", us_cfg.par[j].nMin_normal_female, us_cfg.par[j].nMax_normal_female);
			else
				sprintf(Range, "%d-%d", us_cfg.par[j].nMin_normal_male, us_cfg.par[j].nMax_normal_male);
			//sprintf(temp, "     %s                      %.2f  %s  %s", US_NAME_DEF[j],Reslut, str,Range);

			BuildHelper_BaseInfoFormat(us_cfg.par[j].sLongName, 0, var_info[nCounter++], US_NAME_TEXT);


			DBG_MSG("%d %f\n", bMale, Reslut);
			if (bMale)
			{
				if (Reslut < us_cfg.par[j].nMin_normal_female && (Reslut < -0.0000001 || Reslut > 0.0000001))
					sprintf_s(temp1, "↓");
				else if (Reslut > us_cfg.par[j].nMax_normal_female && (Reslut < -0.0000001 || Reslut > 0.0000001))
					sprintf_s(temp1, "↑");
				else
					sprintf_s(temp1, "");

			}
			else
			{
				if (Reslut < us_cfg.par[j].nMin_normal_male && (Reslut < -0.0000001 || Reslut > 0.0000001))
					sprintf_s(temp1, "↓");
				else if (Reslut > us_cfg.par[j].nMax_normal_male && (Reslut < -0.0000001 || Reslut > 0.0000001))
					sprintf_s(temp1, "↑");
				else
					sprintf_s(temp1, "");
			}

			sprintf(temp, "%.2f%s", Reslut, temp1);

			BuildHelper_BaseInfoFormat(temp, 0, var_info[nCounter++], US_VALUE_TEXT);
			BuildHelper_BaseInfoFormat(Range, 0, var_info[nCounter++], SRANGE_VALUE_TEXT);
			BuildHelper_BaseInfoFormat(str, 0, var_info[nCounter++], SPARTICLE_UNIT_TEXT);

			break;
		}
	}
}

void CUWVAr::GetRange(PCHAR pRange, PCHAR pName)
{
	char ModPath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, ModPath, MAX_PATH);
	PathRemoveFileSpec(ModPath);
	PathAppend(ModPath, "conf.ini");
	GetPrivateProfileString(pName, "Range", "-", pRange, MAX_PATH, ModPath);
}


void CUWVAr::BuildUdcInfoVarInfo(GET_UDC_INFO &udc_cfg, UDC_INFO &udc_info, PSHELL_VAR_INFO var_info, ULONG &nCounter, BOOL bUseOrgFormate /*= FALSE*/)
{
	int             j;
	char            temp[1024] = { 0 }, temp1[MAX_PATH] = { 0 };

	nCounter = 0;

	for (j = 0; j < MAX_UDC_COUNTS; j++)
	{
		if (udc_cfg.par[j].bIsDelete == FALSE)
		{
			temp[0] = 0;

			BuildHelper_BaseInfoFormat(udc_cfg.par[j].sLongName, 0, var_info[nCounter++], UDC_NAME_TEXT);
			if (udc_info.udc_node[j].udc_res[0] == 0)
				strcpy(temp, "-");
			else
				strcpy(temp, udc_info.udc_node[j].udc_res);

			BuildHelper_BaseInfoFormat(temp, 0, var_info[nCounter++], UDC_VALUE_TEXT);

			/*	if( j == 4 )
			strcpy(temp, "6-7");
			else if( j == 2 )
			strcpy(temp, "+-");
			else if( j == 6 )
			strcpy(temp, "1.010-1.025");
			else
			strcpy(temp, "-");*/

			sprintf_s(temp1, "udc_particle%d", j + 1);
			GetRange(temp, temp1);

			if (strcmp(temp, "-") == 0)
			{
				if (j == 4)
					strcpy(temp, "6.0-7.0");
				else if (j == 6)
					strcpy(temp, "1.010-1.030");
			}


			BuildHelper_BaseInfoFormat(temp, 0, var_info[nCounter++], DRANGE_VALUE_TEXT);

			strcpy(temp, udc_info.udc_node[j].udc_value);

			BuildHelper_BaseInfoFormat(temp, 0, var_info[nCounter++], DPARTICLE_UNIT_TEXT);



		}
	}

}

void CUWVAr::ParseImagePath(PCHAR pImagePath, ULONG nTaskID, PSHELL_VAR_INFO var_info, ULONG &nImageCounter)
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
		for (j = 0; j < MAX_PATH; j++)
		{
			if (p[j] == 0)
				break;

			if (p[j] == '|')
			{
				p[j] = 0;

				var_info[nImageCounter].nUnitType = PIC_UNIT_TYPE;
				sprintf(var_info[nImageCounter].VarValue, "%s\\%s", FullGrapPath, p);
				DBG_MSG("ParseImagePath->Path = %s\n", var_info[nImageCounter].VarValue);
				p = &p[j + 1];
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


void CUWVAr::ErrRBCImagePath(ULONG nTaskID, PSHELL_VAR_INFO var_info, ULONG &nImageCounter)
{
	char   FullGrapPath[MAX_PATH], TaskDir[40];

	ULONG  nIndex = 0;

	nImageCounter = 0;


	sprintf(TaskDir, "%u\\DRBC.jpg", nTaskID);
	GetModuleFileName(NULL, FullGrapPath, MAX_PATH);
	PathRemoveFileSpec(FullGrapPath);
	PathAppend(FullGrapPath, "RecGrapReslut");
	PathAppend(FullGrapPath, TaskDir);

	try
	{

		var_info[nImageCounter].nUnitType = PIC_UNIT_TYPE;
		sprintf(var_info[nImageCounter].VarValue, "%s", FullGrapPath);
		DBG_MSG("ParseImagePath->Path = %s\n", var_info[nImageCounter].VarValue);

		nImageCounter++;


	}
	catch (...)
	{
		nImageCounter = 0;
	}

}




void CUWVAr::QCImagePath(PSHELL_VAR_INFO var_info, ULONG &nImageCounter)
{
	char   FullGrapPath[MAX_PATH], TaskDir[40];

	ULONG  nIndex = 0;

	nImageCounter = 0;

	try
	{
		GetModuleFileNameA(NULL, FullGrapPath, MAX_PATH);
		PathRemoveFileSpecA(FullGrapPath);
		sprintf(FullGrapPath, "%s\\QcImage\\list.bmp", FullGrapPath);

		var_info[nImageCounter].nUnitType = PIC_UNIT_TYPE;
		sprintf(var_info[nImageCounter].VarValue, "%s", FullGrapPath);
		DBG_MSG("ParseImagePath->Path = %s\n", var_info[nImageCounter].VarValue);

		nImageCounter++;

		
		PathRemoveFileSpecA(FullGrapPath);
		sprintf(FullGrapPath, "%s\\pic.bmp", FullGrapPath);



		var_info[nImageCounter].nUnitType = PIC_UNIT_TYPE;
		sprintf(var_info[nImageCounter].VarValue, "%s", FullGrapPath);
		DBG_MSG("ParseImagePath->Path = %s\n", var_info[nImageCounter].VarValue);

		nImageCounter++;





	}
	catch (...)
	{
		nImageCounter = 0;
	}
}

void CUWVAr::InitUdcini()
{
	int m_nUdcItem = 0;
	TCHAR s_inifile[MAX_PATH];


	GetModuleFileName(NULL, s_inifile, MAX_PATH);
	PathRemoveFileSpec(s_inifile);
	PathAppend(s_inifile, _T("config\\Udc.ini"));

	std::string str;
	TCHAR szLine[MAX_PATH * 8] = { 0 };

	if (0 != GetPrivateProfileString(_T("udc"),
		_T("item"),
		_T("0"),
		szLine,
		MAX_PATH * 8,
		s_inifile))
	{
		m_nUdcItem = _ttoi(szLine);
	}
	else
	{
		m_nUdcItem = 0;
		//AfxMessageBox(_T("udc load fail!"));
		str = "0";
	}

	int   iIndex = 0;
	int   nItemIndex = 0;
	int   nGradeEnable = 0;
	TCHAR chIndex[INDEX_BUF_LEN] = { 0 };
	TCHAR ItemCode[INDEX_BUF_LEN] = { 0 };
	TCHAR GradeName[INDEX_BUF_LEN] = { 0 };
	int   nGradeThreahold = 0;
	TCHAR GradeValue[INDEX_BUF_LEN] = { 0 };
	TCHAR GradeUnit[INDEX_BUF_LEN] = { 0 };

	TCHAR chRetBuf[MAX_PATH] = { 0 };

	CString code, name;


	for (int i = 0; i<m_nUdcItem; ++i)
	{
		code.Format(_T("code%d"), i);
		name.Format(_T("name%d"), i);
		DWORD dwRet = GetPrivateProfileString(_T("udc"), code, NULL, chRetBuf, MAX_PATH, s_inifile);
		if (dwRet > 0)
		{
			_tcscpy(m_UdcitemInfo[i].m_ctsItemCode, chRetBuf);
		}

		dwRet = GetPrivateProfileString(_T("udc"), name, NULL, chRetBuf, MAX_PATH, s_inifile);
		if (dwRet > 0)
		{
			_tcscpy_s(m_UdcitemInfo[i].m_ctsItemName, chRetBuf);
		}

		for (int j = 0; j < UDC_MAXGRADE; ++j)
		{
			dwRet = 0;
			_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndex);
			dwRet = GetPrivateProfileString(_T("udc"), chIndex, NULL, chRetBuf, MAX_PATH, s_inifile);
			if (dwRet > 0)
			{
				_stscanf(chRetBuf, _T("%d,%[^,],%d,%[^,],%[^,]"), &nGradeEnable, GradeName, &nGradeThreahold, GradeUnit, GradeValue);
				m_UdcitemInfo[i].m_bGradeEnable[j] = nGradeEnable;
				if (nGradeEnable)
				{
					_tcscpy(m_UdcitemInfo[i].m_ctsGradeName[j], GradeName);
					m_UdcitemInfo[i].m_nGradeThreshold[j] = nGradeThreahold;
					_tcscpy(m_UdcitemInfo[i].m_ctsGradeUnit[j], GradeUnit);
					_tcscpy(m_UdcitemInfo[i].m_ctsGradeValue[j], GradeValue);
				}


			}
			iIndex++;
		}
	}
	//m_nNums = iIndex;

}



void CUWVAr::BuildHelper_BaseInfoFormat_ACR_PCR(PTASK_INFO task_info, char* acr, char* pcr)
{
	InitUdcini();
	PCR_ACR cr = { 0 };

	char tmp[64] = { 0 };

	for (int i = 0; i < UDC_MAXGRADE; i++)
	{
		if (strcmp(m_UdcitemInfo[11].m_ctsGradeName[i], task_info->udc_info.udc_node[11].udc_res) == 0)
		{
			strcpy(cr.mca, m_UdcitemInfo[11].m_ctsGradeValue[i]);
		}
		if (strcmp(m_UdcitemInfo[12].m_ctsGradeName[i], task_info->udc_info.udc_node[12].udc_res) == 0)
		{
			strcpy(cr.cre, m_UdcitemInfo[12].m_ctsGradeValue[i]);
		}
		if (strcmp(m_UdcitemInfo[3].m_ctsGradeName[i], task_info->udc_info.udc_node[3].udc_res) == 0)
		{
			strcpy(cr.pro, m_UdcitemInfo[3].m_ctsGradeValue[i]);
		}
	}

	char buff[64] = { 0 };
	if (!strcmp(cr.cre, "0") == 0)
	{
		CString  str = _T("");
		float acrf = (float)(atof(cr.mca) / atof(cr.cre));
		sprintf(acr, "%.2f", acrf);

		float pcrf = (float)(atof(cr.pro) / atof(cr.cre));
		sprintf(pcr, "%.2f", pcrf);

	}
}


void CUWVAr::GetConfigInfo(PGET_US_INFO  pus_cfg, PGET_UDC_INFO pudc_cfg)
{
	SetConfigIniPath(NULL);
	CONFIG_RECORD_INFO ConfigInfo = { 0 };
	ConfigInfo.nRecordType = US_TYPE_INFO;
	ConfigInfo.nConfigLength = sizeof(GET_US_INFO);
	ConfigInfo.pConfigConent = new GET_US_INFO;
	if (ConfigInfo.pConfigConent == NULL)
		return;
	KSTATUS status;
	status = QueryRecordConfig(&ConfigInfo);
	if (status == STATUS_SUCCESS)
	{
		memcpy(pus_cfg, ConfigInfo.pConfigConent, sizeof(GET_US_INFO));
		DBG_MSG("pus_cfg->par[0].bIsDelete = %d\n", pus_cfg->par[0].bIsDelete);
	}


	if (ConfigInfo.pConfigConent != NULL)
		delete[] ConfigInfo.pConfigConent;
	ConfigInfo.pConfigConent = NULL;

	ConfigInfo.nRecordType = UDC_TYPE_INFO;
	ConfigInfo.nConfigLength = sizeof(GET_UDC_INFO);
	ConfigInfo.pConfigConent = new GET_UDC_INFO;

	if (ConfigInfo.pConfigConent == NULL)
		return;

	status = QueryRecordConfig(&ConfigInfo);
	DBG_MSG("status = %x\n", status);
	if (status == STATUS_SUCCESS)
	{
		memcpy(pudc_cfg, ConfigInfo.pConfigConent, sizeof(GET_UDC_INFO));
		DBG_MSG("pudc_cfg->par[0].bIsDelete = %d\n", pudc_cfg->par[0].bIsDelete);
	}

	if (ConfigInfo.pConfigConent != NULL)
		delete[] ConfigInfo.pConfigConent;
	ConfigInfo.pConfigConent = NULL;

}

BOOL CUWVAr::BuildVarArrayforGroupPrint(PULONG   pTaskIDArray, ULONG nTaskCounter, PSHELL_VAR_INFO &var_info, ULONG &nVarCounter, ULONG &nUnitCounterPerGroup)
{

	GET_US_INFO        us_cfg = { 0 };
	GET_UDC_INFO       udc_cfg = { 0 };
	GetConfigInfo(&us_cfg, &udc_cfg);
	int              j;
	BOOL             bRet;
	ULONG            nShellCounter;
	PSHELL_VAR_INFO  pShellVarInfo;

	nVarCounter = 0;

	nUnitCounterPerGroup = GetVarCounterByGroupPrint(&us_cfg, &udc_cfg);
	if (nUnitCounterPerGroup == 0)
		return FALSE;

	nShellCounter = nUnitCounterPerGroup * nTaskCounter;
	pShellVarInfo = (PSHELL_VAR_INFO)malloc(sizeof(SHELL_VAR_INFO) *nShellCounter);

	if (pShellVarInfo == NULL)
		return FALSE;

	for (j = 0; j < nTaskCounter; j++)
	{
		bRet = BuildShellVarInfoByGroupPrint(&pShellVarInfo[j * nUnitCounterPerGroup], pTaskIDArray[j], us_cfg, udc_cfg);
		if (bRet == FALSE)
			break;
	}

	if (bRet == FALSE)
		free(pShellVarInfo);
	else
	{
		nVarCounter = nShellCounter;
		var_info = pShellVarInfo;
	}

	return bRet;
}


#define   G_BASE_COUNTER  5 

ULONG CUWVAr::GetVarCounterByGroupPrint(PGET_US_INFO pUsCfgInfo, PGET_UDC_INFO pUdcCfgInfo)
{
	ULONG   nCounter = 0;
	ULONG j = 0;
	for (; j < pUsCfgInfo->nUsCounts; j++)
	{
		if (pUsCfgInfo->par[j].bIsDelete)
			continue;

		nCounter += 2;
	}

	for (j = 0; j < pUdcCfgInfo->nUdcCounts; j++)
	{
		if (pUdcCfgInfo->par[j].bIsDelete)
			continue;

		nCounter += 2;
	}


	return  nCounter + G_BASE_COUNTER;

}

BOOL CUWVAr::BuildShellVarInfoByGroupPrint(PSHELL_VAR_INFO var_info, ULONG  nTaskID, GET_US_INFO &us_cfg, GET_UDC_INFO &udc_cfg)
{
	char              temp[MAX_PATH] = { 0 }, temp1[MAX_PATH] = { 0 };
	ULONG             nIndex = 0;
	TASK_INFO         task_info = { 0 };
	KSTATUS           status;

	//Access_OpenAccess(TRUE);
	status = Access_GetTaskInfo(nTaskID, ALL_TYPE_INFO, &task_info);
	//Access_CloseAccess();
	if (status != STATUS_SUCCESS)
	{
		DBG_MSG("BuildShellVarInfoByGroupPrint->ReadRecord Error...%x", status);
		return FALSE;
	}

	//构建基本信息
	sprintf(temp, "%u", task_info.main_info.nSN);
	BuildHelper_BaseInfoFormat(temp, 19, var_info[nIndex++]);
	BuildHelper_BaseInfoFormat(task_info.main_info.dtDate, 17, var_info[nIndex++]);

	//构建us信息

	for (ULONG j = 0; j < us_cfg.nUsCounts; j++)
	{
		if (us_cfg.par[j].bIsDelete)
			continue;

		BuildHelper_BaseInfoFormat(us_cfg.par[j].sLongName, 0, var_info[nIndex++], US_NAME_TEXT);
		DBG_MSG("%d %f\n", task_info.pat_info.nSex, task_info.us_info.us_node[j].us_res);
		if (task_info.pat_info.nSex == 2)
		{
			if (task_info.us_info.us_node[j].us_res < task_info.us_info.us_node[j].nMin_normal_female)
				sprintf_s(temp1, "↓");
			else if (task_info.us_info.us_node[j].us_res > task_info.us_info.us_node[j].nMax_normal_female)
				sprintf_s(temp1, "↑");
			else
				sprintf_s(temp1, "");
		}
		else
		{
			if (task_info.us_info.us_node[j].us_res < task_info.us_info.us_node[j].nMin_normal_male)
				sprintf_s(temp1, "↓");
			else if (task_info.us_info.us_node[j].us_res > task_info.us_info.us_node[j].nMax_normal_male)
				sprintf_s(temp1, "↑");
			else
				sprintf_s(temp1, "");

		}

		sprintf(temp, "%.2f%s", task_info.us_info.us_node[j].us_res, temp1);
		BuildHelper_BaseInfoFormat(temp, 0, var_info[nIndex++], US_VALUE_TEXT);
	}

	//构建udc信息

	for (ULONG i = 0; i < udc_cfg.nUdcCounts; i++)
	{
		if (udc_cfg.par[i].bIsDelete)
			continue;

		BuildHelper_BaseInfoFormat(udc_cfg.par[i].sLongName, 0, var_info[nIndex++], UDC_NAME_TEXT);

		if (task_info.udc_info.udc_node[i].udc_res[0] == 0)
			strcpy(temp, "-");
		else
			strcpy(temp, task_info.udc_info.udc_node[i].udc_res);

		BuildHelper_BaseInfoFormat(temp, 0, var_info[nIndex++], UDC_VALUE_TEXT);
	}

	return TRUE;

}



