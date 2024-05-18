#include "StdAfx.h"
#include "BuildHelper.h"
#include <stdio.h>

extern
void
BuildHelper_BaseInfoFormat(
PCHAR             pValue,
int               nVarIndex,
SHELL_VAR_INFO   &var_info,
UCHAR             nVarType
)
{
	int nLen;

	var_info.nUnitType = TEXT_UNIT_TYPE;
	var_info.nVarType  = nVarType;
	if( nVarType == BASE_INFO_TEXT )
		var_info.nVarIndex = (UCHAR)nVarIndex;
	else
		var_info.nVarIndex = 0;

	nLen = strlen(pValue);
	if( nLen < MAX_PATH )
		strcpy(var_info.VarValue, pValue);
	else
	{
		memcpy(var_info.VarValue, pValue, MAX_PATH - 2);
		var_info.VarValue[MAX_PATH-1] = 0;
	}
}