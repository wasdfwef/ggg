#ifndef GLOBAL_VARIABLE_H_DEFINE
#define GLOBAL_VARIABLE_H_DEFINE

#include "WaitQueue.h"

extern long g_LCTimeoutFlag;
extern WaitQueue g_WaitQueue;

extern HANDLE g_LCAlivetEvent;
extern HANDLE g_SystemExitEvent;
extern HANDLE g_MoniterThread;
extern CRITICAL_SECTION g_CS;


#endif  //GLOBAL_VARIABLE_H_DEFINE