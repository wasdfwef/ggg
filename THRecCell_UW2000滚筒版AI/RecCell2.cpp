#include "StdAfx.h"
#include "RecCell2.h"
#include <MALLOC.H>
#include <stdio.h>
#include <SHLWAPI.H>
#include "DbgMsg.h"
#include "interal.h"


CRecCell2::CRecCell2()
{
	hMod = NULL;
	Cell2Recognising = NULL;
	m_RecReslut = NULL;
}
CRecCell2::~CRecCell2()
{
	if( hMod )
		FreeLibrary(hMod);

	if( m_RecReslut )
		free(m_RecReslut);
}

KSTATUS  CRecCell2::StartRecCell(PREC_CELL_EXTENSION pExtension)
{
	CHAR  path[MAX_PATH];
	
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAppend(path, RECOGNISE_DLL_2);
	
	hMod = LoadLibrary(path);
	if( hMod == NULL )
	{
		DBG_MSG("CRecCell1::StartRecCell Load Fail...\n");
		return STATUS_MODULE_LOAD_FAIL;
	}
	
	Cell2Recognising = (TC2_RECOGNISING)GetProcAddress(hMod, "Label_Cell");
	
	
	if( Cell2Recognising == NULL )
		return STATUS_NOT_FOUND_VALUE;


	m_RecReslut = (CUS_Result *)malloc(sizeof(CUS_Result) * 1024);
	if( m_RecReslut == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;
	
//	pExtension->InterFace = this;
//	pExtension->rec_type  = REC_CELL_TYPE2;
	
	return STATUS_SUCCESS;
}


void   CRecCell2::SaveRecReslut(PCHAR  pIniPath, PCHAR pSessionName, int nRecCounter)
{
	int    nType;
	char   va[256], key[20];

	sprintf(key, "%u", nRecCounter);
	WritePrivateProfileString(pSessionName, "Count", key, pIniPath);

	for( int j = 0 ; j < nRecCounter ; j ++ )
	{

		nType = m_RecReslut[j].nType;
		if( nType == 2 )
			nType = CELL_RED_TYPE;
		else if( nType == 3 )
			nType = CELL_WHITE_TYPE;
		else if( nType == 1 )
			nType = CELL_EP_TYPE;
		else if( nType == 9 )
			nType = CELL_CRY_TYPE;
		else if( nType == 5 )
			nType = CELL_CAST_TYPE;
		else if( nType == 7 )
			nType = CELL_BYST_TYPE;
		else
			nType = CELL_OTHER_TYPE;

		sprintf(key, "%u", j);
		sprintf(va, 
			    "%d,%d,%d,%d,%d", 
				nType,
				m_RecReslut[j].nLeft,
			    m_RecReslut[j].nTop,
			    m_RecReslut[j].nRight,
			    m_RecReslut[j].nBottom);

		WritePrivateProfileString(pSessionName,key, va, pIniPath);

	}

}

KSTATUS  CRecCell2::BuildRecReslut(PREC_GRAP_FULL_RECORD   grap_record)
{
	char      IniPath[MAX_PATH], SessionName[40];
	int       nRecCount;
	KSTATUS   status;
	
	status = BuildRecPre(grap_record, SessionName, IniPath);
	if( status != STATUS_SUCCESS )
		return status;
	
	try
	{
		nRecCount = 0;

		if( Cell2Recognising(grap_record->GrapPath, m_RecReslut, &nRecCount) == FALSE )
			return STATUS_UNKNOW_ERROR;
	
		if( nRecCount )
		{
			PathAppend(IniPath, RESLUT_INI);
			SaveRecReslut(IniPath, SessionName, nRecCount);
			PathRemoveFileSpec(IniPath);
		}
		
		return STATUS_SUCCESS;
	}
	catch(...)
	{
		DBG_MSG("CRecCell1::BuildRecReslut->Exception...\n");
		return STATUS_EXCEPTION_ERROR;
	}
	
}
