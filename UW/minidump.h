#ifndef _MINIDUMP_H_

  
#include <windows.h>
#include <DbgHelp.h>
#include <stdlib.h>
#include <tchar.h>
#pragma comment(lib, "dbghelp.lib")   

#ifndef _M_IX86   
#error "The following code only works for x86!"   
#endif   

inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)  
{  
	if(pModuleName == 0)  
	{  
		return FALSE;  
	}  

	WCHAR szFileName[_MAX_FNAME] = L"";  
	_wsplitpath_s(pModuleName, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, NULL, 0);  

	if(_wcsicmp(szFileName, L"ntdll") == 0)  
		return TRUE;  

	return FALSE;  
}  

inline BOOL CALLBACK MiniDumpCallback(PVOID                            pParam,  
									  const PMINIDUMP_CALLBACK_INPUT   pInput,  
									  PMINIDUMP_CALLBACK_OUTPUT        pOutput)  
{  
	if(pInput == 0 || pOutput == 0)  
		return FALSE;  

	switch(pInput->CallbackType)  
	{  
	case ModuleCallback:  
		if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg)  
			if(!IsDataSectionNeeded(pInput->Module.FullPath))  
				pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);  
	case IncludeModuleCallback:  
	case IncludeThreadCallback:  
	case ThreadCallback:  
	case ThreadExCallback:  
		return TRUE;  
	default:;  
	}  

	return FALSE;  
}  

inline LONG CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)  
{  
    LONG lRetValue = EXCEPTION_CONTINUE_SEARCH;
	HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE,  
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	if(hFile != INVALID_HANDLE_VALUE)  
	{  
        MINIDUMP_EXCEPTION_INFORMATION mdei = { 0 };
		mdei.ThreadId           = GetCurrentThreadId();  
		mdei.ExceptionPointers  = pep;  
		mdei.ClientPointers     = NULL;  

        MINIDUMP_CALLBACK_INFORMATION mci = { 0 };
		mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;  
		mci.CallbackParam       = 0;  

        BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &mdei, NULL, &mci);
        if (bOK)
        {
            lRetValue = EXCEPTION_EXECUTE_HANDLER;

            TCHAR exePath[MAX_PATH] = { 0 };
            TCHAR path[MAX_PATH] = { 0 };
            GetModuleFileName(NULL, path, MAX_PATH);
            TCHAR szDeriveName[MAX_PATH] = { 0 }, szDirName[MAX_PATH] = { 0 },
                szFileName[MAX_PATH] = { 0 }, fileExtent[MAX_PATH] = { 0 };
            _tsplitpath_s(path, szDeriveName, MAX_PATH, szDirName, MAX_PATH, szFileName, MAX_PATH, fileExtent, MAX_PATH);
            _sntprintf(exePath, MAX_PATH-1, _T("%s%sCrashReporter.exe"), szDeriveName, szDirName);

            TCHAR par[MAX_PATH] = { 0 };
            _sntprintf(par, MAX_PATH-1, _T("%s %s%s %s"), szFileName, szFileName, fileExtent, strFileName);
            OutputDebugString(exePath);
            OutputDebugString(par);
            HINSTANCE hInstCrashReporter = ShellExecute(NULL, _T("open"), exePath, par, NULL, SW_SHOW);
            if (hInstCrashReporter <= (HINSTANCE)32)
            {
                lRetValue = EXCEPTION_CONTINUE_SEARCH;
                OutputDebugString(_T("ShellExecute open CrashReporter failed"));
            }
        }
        lRetValue = EXCEPTION_EXECUTE_HANDLER;
		CloseHandle(hFile);  
	}

    return lRetValue;
}  

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)  
{  
	SYSTEMTIME time;
	GetLocalTime(&time);
    TCHAR strDmpName[MAX_PATH] = { 0 };
    _sntprintf(strDmpName, MAX_PATH-1, _T("%04d%02d%02d_%02d%02d%02d.dmp"), time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
    TCHAR path[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, path, MAX_PATH);
    TCHAR szDeriveName[MAX_PATH] = { 0 }, szDirName[MAX_PATH] = { 0 },
        szFileName[MAX_PATH] = { 0 }, fileExtent[MAX_PATH] = { 0 };
    _tsplitpath_s(path, szDeriveName, MAX_PATH, szDirName, MAX_PATH, szFileName, MAX_PATH, fileExtent, MAX_PATH);

    TCHAR strPath[MAX_PATH] = { 0 };
    _sntprintf(strPath, MAX_PATH-1, _T("%s%sDump\\"), szDeriveName, szDirName);
    CreateDirectory(strPath, 0);
    _tcscat(strPath, strDmpName);

    return CreateMiniDump(pExceptionInfo, strPath);
}  

// 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效   
void DisableSetUnhandledExceptionFilter()  
{  
	void* addr = (void*)GetProcAddress(LoadLibraryA("kernel32.dll"),  
		"SetUnhandledExceptionFilter");  

	if (addr)  
	{  
		unsigned char code[16];  
		int size = 0;  

		code[size++] = 0x33;   
		code[size++] = 0xC0;  
		code[size++] = 0xC2;  
		code[size++] = 0x04;  
		code[size++] = 0x00;  

		DWORD dwOldFlag, dwTempFlag;  
		if (VirtualProtectEx(GetCurrentProcess(), addr, size, PAGE_EXECUTE_READWRITE, &dwOldFlag) == 0)
		{
            OutputDebugString(_T("DisableSetUnhandledExceptionFilter VirtualProtectEx failed\n"));
			return;
		}
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		VirtualProtectEx(GetCurrentProcess(), addr, size, dwOldFlag, &dwTempFlag);

	}  
}  

void InitMinDump()  
{  
	//注册异常处理函数   
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);  

	//使SetUnhandledExceptionFilter   
	DisableSetUnhandledExceptionFilter();  
}  

#endif //_MINIDUMP_H_
