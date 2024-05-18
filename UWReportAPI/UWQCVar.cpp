#include "StdAfx.h"
#include "UWQCVar.h"
#include <malloc.h>
#include <stdio.h>
#include "BuildHelper.h"
#include <SHLWAPI.H>
#include "DbgMsg.h"
#include "..\..\inc\UW2000DB.h"

CUWQCVar::CUWQCVar(void)
{
}

CUWQCVar::~CUWQCVar(void)
{
}

KSTATUS CUWQCVar::CreateQCDataFile( PULONG  pTaskIDArray,ULONG nTaskCounter, int nLGType, PCHAR pQCDataFile,USHORT nCellType )
{
	ULONG              nQCCounter;
	KSTATUS            status ;
	QC_OUTLINE         out_line;
	PQC_SAMPLE_DATA    qc_data;


	status = LoadTaskInfo( pTaskIDArray, nTaskCounter, nLGType, out_line, qc_data, nQCCounter,nCellType);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("CreateQCDataFile->>>LoadTaskInfo Fail...\n");
		return status;
	}

	status = BuildQCDataFile(out_line, qc_data, nQCCounter, pQCDataFile);
	free(qc_data);
	return status;


}

KSTATUS CUWQCVar::LoadTaskInfo(PULONG  pTaskIDArray,ULONG nTaskCounter, int nLGType,QC_OUTLINE &out_line, PQC_SAMPLE_DATA &pQCSampleData, ULONG &nSampleCounter, USHORT nCellType )
{
	int                   iYear,iMonth,iDay;
	KSTATUS               status;
	PTASK_INFO             pTask_info; 

	pTask_info      = (PTASK_INFO)malloc(sizeof(TASK_INFO) * nTaskCounter);
	if( pTask_info == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;
	

	pQCSampleData = (PQC_SAMPLE_DATA)malloc(sizeof(QC_SAMPLE_DATA) * nTaskCounter);
	if( pQCSampleData == NULL )
	{		
		return STATUS_ALLOC_MEMORY_ERROR;
	}	
	ZeroMemory(pTask_info, sizeof(TASK_INFO) * nTaskCounter);
	ZeroMemory(pQCSampleData, sizeof(QC_SAMPLE_DATA) * nTaskCounter);
	ZeroMemory(&out_line, sizeof(out_line));
	nSampleCounter = 0;
	DBG_MSG("LoadTaskInfo->nTaskCounter = %x\n",nTaskCounter);

	GetQCOutLine(nLGType, nCellType, out_line);

	for( ULONG j = 0 ; j < nTaskCounter ; j ++ )
	{		

		pTask_info[j].main_info.nID = pTaskIDArray[j];
		status = Access_GetTaskInfo(pTask_info[j].main_info.nID,ALL_TYPE_INFO,&pTask_info[j]); 
		if( status != STATUS_SUCCESS )
		{
			DBG_MSG("BuildShellVarInfoByGroupPrint->ReadRecord Error...%x", status);
			return status;
		}

		if( pTask_info[j].main_info.nAttribute == 0 )
		{
			DBG_MSG("LoadTaskInfo->bIsQC.== FALSE.err\n");
			status = STATUS_INVAILD_DATA;
			break;
		}

		if( nLGType != pTask_info[j].main_info.nQcUsType - 1)
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
			pQCSampleData[nSampleCounter].nTestValue = (int)pTask_info[j].us_info.us_node[0].us_res;
		else if( nCellType == CELL_WHITE_TYPE )
			pQCSampleData[nSampleCounter].nTestValue = (int)pTask_info[j].us_info.us_node[2].us_res;
		try
		{
			sscanf(pTask_info[j].main_info.dtDate,("%d-%d-%d"),&iYear,&iMonth,&iDay);
			DBG_MSG("LoadTaskInfo iYear = %u, iMonth = %u, iDay = %u, SrcDate = %s", iYear, iMonth, iDay, pTask_info[j].main_info.dtDate);
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
	
	if( status != STATUS_SUCCESS )
		free(pQCSampleData);
	else
		out_line.nSampleCounter = nSampleCounter;


	DBG_MSG("LoadTaskInfo->status.== %x", status);

	return status;

}

KSTATUS CUWQCVar::BuildQCDataFile( QC_OUTLINE &out_line, PQC_SAMPLE_DATA pQCSampleData, ULONG nSampleCounter, PCHAR pQCDataFilePath )
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

KSTATUS CUWQCVar::GetQCOutLine( int nLGType, USHORT nCellType, QC_OUTLINE &out_line )
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
	else if( nLGType == 1 )
	{
		out_line.nMinValue = GetPrivateProfileInt(SessionName, "p_min", 0, IniPath);
		out_line.nMaxValue = GetPrivateProfileInt(SessionName, "p_max", 0, IniPath);
		out_line.nGerValue = GetPrivateProfileInt(SessionName, "p_value", 0, IniPath);
	}

	return STATUS_SUCCESS;

}