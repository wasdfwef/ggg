#pragma once

#include <windows.h>
#include <atltime.h>
#include <sys/timeb.h>
#include <iostream>
using namespace std;

class LogFile
{
public:
	LogFile(void);
	~LogFile(void);

public:
	//日志文件
	FILE* m_pfLogFile;
	char m_cInfo[10240];

	int  SetLogFile(FILE *pfLogFile);
	int  WriteLogInfo(const char *pInfo);
	void Log(char *,char* );
	char* GetCurrentTimeEx(bool isYear = true);
};