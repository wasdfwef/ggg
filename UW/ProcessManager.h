#ifndef _PROCESS_MANAGER_H_
#define _PROCESS_MANAGER_H_

class CProcessManager
{
public:
	CProcessManager();
	~CProcessManager();
public:
	static std::vector<int> GetProcessID(const TCHAR *cPorcessName);
	static bool CloseProcess(unsigned int unProcessID);
	static bool CloseProcess(const TCHAR *cPorcessName);
	static BOOL CreateProcess(TCHAR *pAppName, TCHAR *pParam, TCHAR *pWorkDir, bool bShowWnd = false);
};

#endif // _PROCESS_MANAGER_H_