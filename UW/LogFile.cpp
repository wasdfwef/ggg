#include "stdafx.h"
#include "LogFile.h"

LogFile::LogFile(void)
{
	m_pfLogFile = NULL;
	memset(m_cInfo, NULL, sizeof(m_cInfo));
}

LogFile::~LogFile(void)
{
	if (NULL != m_pfLogFile)
	{
		fclose(m_pfLogFile);
		m_pfLogFile = NULL;
	}
}

int LogFile::SetLogFile(FILE *pfLogFile)
{
	m_pfLogFile = pfLogFile;
	return 0;
}

int LogFile::WriteLogInfo(const char *pInfo)
{
	if (NULL != m_pfLogFile)
	{
		fprintf(m_pfLogFile, "%s", pInfo);
		fflush(m_pfLogFile);
		return 0;
	}
	return 1;


}

void LogFile::Log(char * buffer,char* fileName)
{
	char cFileName[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, cFileName, MAX_PATH);
	PathRemoveFileSpecA(cFileName);
	PathAppendA(cFileName, "Log");

	sprintf(cFileName, "%s\\%s_%s", cFileName, GetCurrentTimeEx(), fileName);

	FILE *m_pfLogFile = NULL;
	if (NULL != m_pfLogFile)
	{
		fclose(m_pfLogFile);
	}
	m_pfLogFile = fopen(cFileName, "a+");
	if (NULL == m_pfLogFile)
	{
		return;
	}
	LogFile rl;
	rl.SetLogFile(m_pfLogFile);


	sprintf(rl.m_cInfo," %s",buffer);
	rl.WriteLogInfo(rl.m_cInfo);
}


char* LogFile::GetCurrentTimeEx(bool isYear)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);

	char temp[512] = { 0 };

	if (isYear)
		sprintf(temp, "%04d-%02d-%2d", sys.wYear, sys.wMonth, sys.wDay);
	else
		sprintf(temp, "%02d:%02d:%02d", sys.wHour,sys.wMinute,sys.wSecond);

	char * pTemp = temp;

	return pTemp;
}