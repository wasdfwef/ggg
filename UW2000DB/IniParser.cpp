#include "StdAfx.h"
#include "IniParser.h"
#include "DbgMsg.h"

CIniParser::CIniParser(void)
{

	memset(m_szFileName,0,sizeof(m_szFileName));
}

CIniParser::~CIniParser(void)
{
}


BOOL CIniParser:: Initialize(char* pConfig_Ini_FileName)
{
	BOOL bRet = FALSE;

	if(strlen(pConfig_Ini_FileName)<=0)
	{
		bRet = FALSE;
		return bRet;
	}

	strcpy_s(m_szFileName,pConfig_Ini_FileName);

	bRet = TRUE;

	return bRet;
}

void CIniParser:: Uninitialize(void)
{
	memset(m_szFileName,0,sizeof(m_szFileName));
}



KSTATUS CIniParser::Read(READ_PAR*  pread_par)
{
	KSTATUS status = STATUS_SUCCESS;

	if(pread_par == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	int nLens = GetPrivateProfileString(pread_par->sAppName,
		pread_par->sKeyName,
		"",
		pread_par->sString,
		MAX_PATH,
		m_szFileName);

	if(nLens > 0)
	{
		if(false == pread_par->bIsString)
		{
			pread_par->nValue = atoi(pread_par->sString);
		}
		if( pread_par->bIsfloat)
		{
			pread_par->fValue = (float)atof(pread_par->sString);
		}


		status = STATUS_SUCCESS;
	}
	else
	{
		status = STATUS_INVALID_PARAMETERS;
	}
	
	return status;
}


KSTATUS CIniParser::Write(WRITE_PAR write_par)
{
	KSTATUS status = STATUS_SUCCESS;

	if(WritePrivateProfileString(write_par.sAppName,
		write_par.sKeyName,
		write_par.sString,
		m_szFileName))
	{
		status = STATUS_SUCCESS;
	}
	else
	{
		status = STATUS_INVALID_PARAMETERS;
	}

	return status;
}


void CIniParser::EmptyByAppName(char* sAppName)
{
	if(sAppName)
	{
		WritePrivateProfileString(sAppName,
			NULL,
			NULL,
			m_szFileName);
	}
}
