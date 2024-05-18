#ifndef _CCONFIGSET_INCLUDE
#define _CCONFIGSET_INCLUDE
#include "..\..\..\inc\THDBUW_Access.h"

extern "C" KSTATUS WINAPI QueryRecordConfig(PCONFIG_RECORD_INFO ConfigInfo);

extern "C" KSTATUS WINAPI SetRecordConfig(PCONFIG_RECORD_INFO ConfigInfo);

extern "C" VOID WINAPI SetConfigIniPath( PCHAR    pResIniPath );
//必须先调用SetConfigIniPath设置ini 路径，pResIniPath为空时自动查询ini 路径

#endif