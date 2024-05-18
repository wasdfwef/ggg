#ifndef   _US12SQCVAR_INCLUDE
#define   _US12SQCVAR_INCLUDE

#include "..\..\inc\THInterface.h"
#include "..\..\inc\THReportAPI.h"

class  CUS12sQCVar
{
	KSTATUS  LoadTaskInfo(PULONG   pTaskIDArray, ULONG nTaskCounter, int nLGType,QC_OUTLINE &out_line, PQC_SAMPLE_DATA &pQCSampleData, ULONG &nSampleCounter, USHORT nCellType);
	KSTATUS  BuildQCDataFile(QC_OUTLINE &out_line, PQC_SAMPLE_DATA pQCSampleData, ULONG nSampleCounter, PCHAR pQCDataFilePath);
	KSTATUS  GetQCOutLine(int nLGType, USHORT nCellType, QC_OUTLINE &out_line);

public:

	KSTATUS  CreateQCDataFile(PULONG  pTaskIDArray, ULONG nTaskCounter, int nLGType, PCHAR  pQCDataFile,USHORT nCellType);
	
};



#endif