#include "stdafx.h"
#include <windows.h>
#include "ProcessManager.h"
#include <Psapi.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <tchar.h>


#pragma comment(lib, "Psapi.lib")


CProcessManager::CProcessManager()

{

}
CProcessManager::~CProcessManager()

{

}
std::vector<int> CProcessManager::GetProcessID(const TCHAR *cPorcessName)
{
	std::vector<int> ids;
	HANDLE hProcess = NULL;
	PROCESSENTRY32 prry;
	prry.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet;
	TCHAR pmbbuf[MAX_PATH];
	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	bRet = Process32First(hProcess, &prry);
	while (bRet)
	{
		memset(pmbbuf, 0, MAX_PATH);
		_sntprintf_s(pmbbuf, MAX_PATH, _T("%s"), prry.szExeFile);
        CString str1(pmbbuf), str2(cPorcessName);
        if (0 == str1.CompareNoCase(str2))
		{
			ids.push_back(prry.th32ProcessID);
		}
		else
		{
			//查找下一个进程
			bRet = Process32Next(hProcess, &prry);
		}
	};

	return ids;
}
bool CProcessManager::CloseProcess(unsigned int unProcessID)
{
	HANDLE bExitCode = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE
		| PROCESS_ALL_ACCESS, FALSE, unProcessID);
	//WaitForSingleObject(bExitCode, INFINITE);
	if (NULL != bExitCode)
	{
		BOOL bFlag = TerminateProcess(bExitCode, 0);
		CloseHandle(bExitCode);
		return true;
	}
	return false;
}

bool CProcessManager::CloseProcess(const TCHAR *cPorcessName)
{
	std::vector<int> nPids = GetProcessID(cPorcessName);
	bool ret = true;
	for (int i = 0; i < nPids.size(); i++)
	{
		if (!CloseProcess(nPids[i]))
		{
			ret = false;
		}
	}
	
	return ret;
}

BOOL CProcessManager::CreateProcess(TCHAR *pAppName, TCHAR *pParam, TCHAR *pWorkDir, bool bShowWnd)
{
	PROCESS_INFORMATION pi = {0}; //进程信息  
	STARTUPINFO si = {0}; //启动信息  
	si.dwFlags = STARTF_USESHOWWINDOW;
	
	DWORD dwCreationFlags = 0;
	if (!bShowWnd)
	{
		//dwCreationFlags = CREATE_NO_WINDOW;
		//si.wShowWindow = SW_HIDE;
		si.wShowWindow = SW_MINIMIZE;
	}
	else
	{
		si.wShowWindow = SW_SHOWNORMAL;
	}
	TCHAR param[MAX_PATH] = { 0 };
	_sntprintf_s(param, MAX_PATH, _T(" %s"), pParam);
	TCHAR CmdLine[1024];
	_sntprintf_s(CmdLine, 1024, _T("\"%s\" %s"), pAppName, pParam);
	TCHAR dir[1024];
	_sntprintf_s(dir, 1024, _T("\"%s\""), pWorkDir);

	return ::CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, dwCreationFlags, NULL, dir, &si, &pi);
}
