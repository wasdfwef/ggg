#include "StdAfx.h"
#include "us12sQCVar.h"
#include <malloc.h>
#include <stdio.h>
#include "BuildHelper.h"
#include <SHLWAPI.H>
#include "DbgMsg.h"

#pragma comment(lib, "shlwapi.lib")


KSTATUS  CUS12sQCVar::GetQCOutLine(int nLGType, USHORT nCellType, QC_OUTLINE &out_line)
{
	char  SessionName[50], IniPath[MAX_PATH];

	GetModuleFileName(NULL, IniPath, MAX_PATH);
	PathRemoveFileSpec(IniPath);
	PathAppend(IniPath, "conf.ini");
	if( nCellType == CELL_RED_TYPE )
		strcpy(SessionName, "rbc_qc");
	else
		strcpy(SessionName, "wbc_qc");

	out_line.nLJType = nLGType;

	if( nLGType == 0 ) //n
	{
		out_line.nMinValue = GetPrivateProfileInt(SessionName, "n_min", 0, IniPath);
		out_line.nMaxValue = GetPrivateProfileInt(SessionName, "n_max", 0, IniPath);
		out_line.nGerValue = GetPrivateProfileInt(SessionName, "n_value", 0, IniPath);
	}
	else
	{
		out_line.nMinValue = GetPrivateProfileInt(SessionName, "p_min", 0, IniPath);
		out_line.nMaxValue = GetPrivateProfileInt(SessionName, "p_max", 0, IniPath);
		out_line.nGerValue = GetPrivateProfileInt(SessionName, "p_value", 0, IniPath);
	}

	return STATUS_SUCCESS;


}

KSTATUS  CUS12sQCVar::LoadTaskInfo(PULONG   pTaskIDArray, ULONG nTaskCounter, int nLGType,QC_OUTLINE &out_line, PQC_SAMPLE_DATA &pQCSampleData, ULONG &nSampleCounter, USHORT nCellType)
{
	int                   iYear,iMonth,iDay;
	KSTATUS               status;
	PTASK_INFO    TaskInfo;

	TaskInfo      = (PTASK_INFO)malloc(sizeof(PTASK_INFO) * nTaskCounter);
	if( TaskInfo == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	pQCSampleData = (PQC_SAMPLE_DATA)malloc(sizeof(QC_SAMPLE_DATA) * nTaskCounter);
	if( pQCSampleData == NULL )
	{
		free(TaskInfo);
		return STATUS_ALLOC_MEMORY_ERROR;
	}

	ZeroMemory(TaskInfo, sizeof(PTASK_INFO) * nTaskCounter);
	ZeroMemory(pQCSampleData, sizeof(QC_SAMPLE_DATA) * nTaskCounter);
	ZeroMemory(&out_line, sizeof(out_line));
	nSampleCounter = 0;
	DBG_MSG("LoadTaskInfo->nTaskCounter = %x\n",nTaskCounter);

	GetQCOutLine(nLGType, nCellType, out_line);

	for( ULONG j = 0 ; j < nTaskCounter ; j ++ )
	{
		TaskInfo[j].MainInfo1.nID = pTaskIDArray[j];
		TaskInfo[j].TypeInfo      = ALL_INFO_TYPE;
		status = THInterface_ReadRecordInfo(ALL_INFO_TYPE, (PCHAR)&TaskInfo[j], sizeof(FULL_TASK_INFO_1));
		if( status != STATUS_SUCCESS )
		{
			DBG_MSG("LoadTaskInfo->THInterface_ReadRecordInfo..err\n");
			break;
		}

		if( TaskInfo[j].MainInfo1.bIsQC == FALSE )
		{
			DBG_MSG("LoadTaskInfo->bIsQC.== FALSE.err\n");
			status = STATUS_INVAILD_DATA;
			break;
		}

		if( nLGType != TaskInfo[j].MainInfo1.nQCType )
			continue;

/*		if( TaskInfo[j].MainInfo1.nMaxvalue != out_line.nMaxValue ||
			TaskInfo[j].MainInfo1.nMinvalue != out_line.nMinValue ||
			TaskInfo[j].MainInfo1.nGerValue != out_line.nGerValue )
			{
				DBG_MSG("LoadTaskInfo->STATUS_DATA_MATCH_ERROR.== %u<->%u, %u<->%u, %u<->%u, nLGType = %x, nQCType = %x\n", TaskInfo[j].MainInfo1.nMaxvalue, out_line.nMaxValue, TaskInfo[j].MainInfo1.nMinvalue, out_line.nMinValue, TaskInfo[j].MainInfo1.nGerValue, out_line.nGerValue, nLGType, TaskInfo[j].MainInfo1.nQCType);
				status = STATUS_DATA_MATCH_ERROR;
				break;
			}*/

		if( nCellType == CELL_RED_TYPE )
			pQCSampleData[nSampleCounter].nTestValue = (int)TaskInfo[j].us1.us_d1[0].us_res;
		else if( nCellType == CELL_WHITE_TYPE )
			 pQCSampleData[nSampleCounter].nTestValue = (int)TaskInfo[j].us1.us_d1[2].us_res;
		try
		{
			 sscanf(TaskInfo[j].MainInfo1.sdtValidityday,("%d-%d-%d"),&iYear,&iMonth,&iDay);
			 DBG_MSG("LoadTaskInfo iYear = %u, iMonth = %u, iDay = %u, SrcDate = %s", iYear, iMonth, iDay, TaskInfo[j].MainInfo1.sdtValidityday);
			 pQCSampleData[nSampleCounter].nTestDate = MAKE_DATE(iYear, iMonth, iDay);
		}
		catch (...)
		{
			pQCSampleData[nSampleCounter].nTestDate = 0;
		}

		nSampleCounter ++;
	}

	if( status == STATUS_SUCCESS && nSampleCounter == 0 )
		status = STATUS_NO_UNIT_DATA;

	free(TaskInfo);
	if( status != STATUS_SUCCESS )
		free(pQCSampleData);
	else
		out_line.nSampleCounter = nSampleCounter;


	DBG_MSG("LoadTaskInfo->status.== %x", status);
	
	return status;

}

KSTATUS  CUS12sQCVar::BuildQCDataFile(QC_OUTLINE &out_line, PQC_SAMPLE_DATA pQCSampleData, ULONG nSampleCounter, PCHAR pQCDataFilePath)
{
	char               str[80];
	ULONG              r;
	HANDLE             hFile;
	LARGE_INTEGER      t;

	GetModuleFileName(NULL, pQCDataFilePath, MAX_PATH);
	PathRemoveFileSpec(pQCDataFilePath);
	PathAppend(pQCDataFilePath, "Temp");
	if( PathFileExists(pQCDataFilePath) == FALSE )
		CreateDirectory(pQCDataFilePath, NULL);
	QueryPerformanceCounter(&t);
	sprintf(str, "%u_%u.qcd", t.u.HighPart,t.u.LowPart);
	PathAppend(pQCDataFilePath,str);
	

	hFile = CreateFile(pQCDataFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return STATUS_WRITE_FAIL;

	WriteFile(hFile, &out_line, sizeof(out_line), &r, NULL);
	WriteFile(hFile, pQCSampleData, sizeof(QC_SAMPLE_DATA) * nSampleCounter, &r, NULL);
	CloseHandle(hFile);
	return STATUS_SUCCESS;
}

KSTATUS CUS12sQCVar::CreateQCDataFile(PULONG  pTaskIDArray, ULONG nTaskCounter, int nLGType, PCHAR  pQCDataFile, USHORT nCellType)
{
	ULONG              nQCCounter;
	KSTATUS            status;
	QC_OUTLINE         out_line;
	PQC_SAMPLE_DATA    qc_data;

	
	status = LoadTaskInfo(pTaskIDArray, nTaskCounter, nLGType, out_line, qc_data, nQCCounter,nCellType);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("CreateQCDataFile->>>LoadTaskInfo Fail...\n");
		return status;
	}

	status = BuildQCDataFile(out_line, qc_data, nQCCounter, pQCDataFile);
	free(qc_data);
	return status;

}