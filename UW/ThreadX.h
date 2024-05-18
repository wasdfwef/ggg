#pragma once

#include <Windows.h>


class CThreadX
{
public:
	CThreadX(void);
	virtual ~CThreadX(void);

	virtual void ThreadEntryPoint();

	void		 Start();
	int			 Pause();
	int			 Resume();
	virtual void Stop();


private:
	static unsigned __stdcall ThreadStaticEntryPoint(void *arg);

private:
	HANDLE m_hThread;
};
