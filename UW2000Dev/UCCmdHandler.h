#ifndef HANDLE_UC_CMD_H
#define HANDLE_UC_CMD_H

#include "f_err.h"
#include "fmmc.h"

#pragma pack(1)

typedef  struct _COMM_CMD_DATA
{
	UCHAR     nCmd;
	CHAR      data[1];
	
}COMM_CMD_DATA, *PCOMM_CMD_DATA;

#pragma pack()

#define   DATA_LENGTH_IN_CMD(CMDDATATYPE) ( sizeof(CMDDATATYPE) - sizeof(LC_HEADER) )

DWORD WINAPI MonitorLCStatus(LPVOID lpParameter);

KSTATUS HandleUCCmd(PROGRAM_OBJECT  PrgObject, PRP prp);

void ClearTask();
 
// KSTATUS LCForceClear(PROGRAM_OBJECT ProgramObject, DWORD ProtocolCmd);
// 
// KSTATUS LCReset(PROGRAM_OBJECT ProgramObject, DWORD ProtocolCmd);
// 
// KSTATUS LCStartAutoDetect(PROGRAM_OBJECT ProgramObject, DWORD ProtocolCmd);
// 
// KSTATUS LCStopAutoDetect(PROGRAM_OBJECT ProgramObject, DWORD ProtocolCmd);
// 
// KSTATUS xxxxx(PROGRAM_OBJECT ProgramObject, DWORD ProtocolCmd);


#endif	//HANDLE_UC_CMD_H
