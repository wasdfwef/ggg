#ifndef _UWQCVAR_INCLUDE
#define _UWQCVAR_INCLUDE
#pragma once

#include "..\..\inc\THInterface.h"
#include "..\..\inc\THReportAPI.h"
#include "..\..\inc\THDBUW_Access.h"

class CUWQCVar
{
public:
	CUWQCVar(void);
	virtual ~CUWQCVar(void);

public:
	KSTATUS  CreateQCDataFile(PULONG  pTaskIDArray,ULONG nTaskCounter, int nLGType, PCHAR  pQCDataFile,USHORT nCellType);
	KSTATUS  LoadTaskInfo(PULONG  pTaskIDArray,ULONG nTaskCounter, int nLGType,QC_OUTLINE &out_line, PQC_SAMPLE_DATA &pQCSampleData, ULONG &nSampleCounter, USHORT nCellType);
	KSTATUS  BuildQCDataFile(QC_OUTLINE &out_line, PQC_SAMPLE_DATA pQCSampleData, ULONG nSampleCounter, PCHAR pQCDataFilePath);
	KSTATUS  GetQCOutLine(int nLGType, USHORT nCellType, QC_OUTLINE &out_line);
};


#endif