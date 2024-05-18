#ifndef  _BUILD_HELPER_INCLUDE
#define  _BUILD_HELPER_INCLUDE

#include "..\..\inc\BuildShell.h"
#include "..\..\inc\THReportAPI.h"

extern
void
BuildHelper_BaseInfoFormat(
PCHAR             pValue,
int               nVarIndex,
SHELL_VAR_INFO   &var_info,
UCHAR             nVarType = BASE_INFO_TEXT
);


#endif