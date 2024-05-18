#ifndef   _DBGMSG_INCLUDE
#define   _DBGMSG_INCLUDE
#include <stdio.h>
#include <windows.h>

#define _SUPPORT_DBG_MSG_

#define SRC_FILE_INFO    __FILE__,__LINE__

#ifdef _SUPPORT_DBG_MSG_
#define DBG_MSG              TraceDbgNoSrcFileInfo
#else
#define DBG_MSG
#endif



extern void TraceDbgNoSrcFileInfo(
LPCSTR  format,
...
);

#endif