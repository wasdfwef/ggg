#ifndef   _DBGMSG_INCLUDE
#define   _DBGMSG_INCLUDE
#include <stdio.h>

#define _SUPPORT_DBG_MSG_

#define SRC_FILE_INFO    __FILE__,__LINE__

#ifdef _SUPPORT_DBG_MSG_
#define DBG_MSG              TraceDbgNoSrcFileInfo
#define DBG_MSG_EX           TraceDbgSrcFileInfo
#else
#define DBG_MSG
#define DBG_MSG_EX
#endif

extern
VOID
TraceDbgSrcFileInfo(
LPCSTR  format,
PCHAR   pSrcFile,
ULONG   nLines,
...
);


extern
VOID
TraceDbgNoSrcFileInfo(
LPCSTR  format,
...
);

#endif