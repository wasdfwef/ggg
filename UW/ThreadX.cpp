#include "StdAfx.h"
#include "ThreadX.h"
#include <process.h>

CThreadX::CThreadX(void)
:m_hThread(NULL)
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &CThreadX::ThreadStaticEntryPoint, this, CREATE_SUSPENDED, NULL);
}

CThreadX::~CThreadX(void)
{
	if (m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void CThreadX::ThreadEntryPoint()
{
	
}

unsigned __stdcall CThreadX::ThreadStaticEntryPoint(void *arg)
{
	if (arg == NULL)
	{
		return 1;
	}
	CThreadX *_this = (CThreadX*)arg;
	_this->ThreadEntryPoint();

	return 0;
}

void CThreadX::Start()
{
	ResumeThread(m_hThread);
}

int CThreadX::Pause()
{
	return SuspendThread(m_hThread);
}

int CThreadX::Resume()
{
	return ResumeThread(m_hThread);
}

void CThreadX::Stop()
{
	_endthreadex(0);
}
