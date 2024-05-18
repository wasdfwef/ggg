#ifndef TRANSLATE_MSG_H
#define TRANSLATE_MSG_H

#include "fmmc.h"
#include "UCCmdHandler.h"
#include "ThLcProto_UW2000.h"

#define COMM_CMD_LENGTH (sizeof(COMM_CMD_DATA) - 1)

KSTATUS PostCmdToLC(PROGRAM_OBJECT PrgObject, PCOMM_CMD_DATA CmdData, DWORD CmdLength = COMM_CMD_LENGTH);

KSTATUS SendCmdToLC(PROGRAM_OBJECT PrgObject, PCOMM_CMD_DATA CmdData, int CmdDataLen = COMM_CMD_LENGTH, PCHAR OutputBuffer = NULL, int OutputLen = 0, DWORD TimeoutInterval = INFINITE);
KSTATUS SendCmdToLCEx(PROGRAM_OBJECT PrgObject, PCOMM_CMD_DATA CmdData, int CmdDataLen = COMM_CMD_LENGTH, PCHAR OutputBuffer = NULL, int OutputLen = 0, DWORD TimeoutInterval = INFINITE);

KSTATUS PostCmdToUC(PROGRAM_OBJECT PrgObject, PLC_COMMAND CmdData);


#endif  //TRANSLATE_MSG_H

