#pragma once
#include "IniParser.h"
#include <Shlwapi.h>
class CQueryAndSetConfig
{
public:
	CQueryAndSetConfig(void);
	~CQueryAndSetConfig(void);

public:
	KSTATUS  TH_DB_GetUsInfo(GET_US_INFO* pGetUsInfo);
	KSTATUS  TH_DB_SetUsInfo(SET_US_INFO set_us_info);
	KSTATUS  TH_DB_GetUdcInfo(GET_UDC_INFO* pGetUdcInfo);
	KSTATUS  TH_DB_SetUdcInfo(SET_UDC_INFO set_udc_info);
	BOOL     GetConfigIniFilePath();
	void   SetResIniPath(PCHAR  path);

	char   m_nConfig_IniFile[MAX_PATH];
};
