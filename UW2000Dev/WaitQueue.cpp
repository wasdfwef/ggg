#include "StdAfx.h"
#include "WaitQueue.h"


WaitQueue::WaitQueue()
{
	InitializeCriticalSection(&m_Cs);
}

WaitQueue::~WaitQueue()
{
	DeleteCriticalSection(&m_Cs);
}

bool WaitQueue::Add(UCHAR Cmd, HANDLE hEvent, CHAR *OutputBuffer, int OutputLength)
{
	bool Result = false;
	EnterCriticalSection(&m_Cs);

	WaitContex NewWait(Cmd, hEvent, OutputBuffer, OutputLength);
	m_Queue.insert( WaitContextQueue :: value_type (Cmd, NewWait) );
	Result = true;

end:
	LeaveCriticalSection(&m_Cs);
	return Result;
}

bool WaitQueue::Remove(UCHAR Cmd)
{
	bool Result = false;
	EnterCriticalSection(&m_Cs);

	if ( !IsItemExist(Cmd) )
	{
		DbgPrint("Remove Command %#x is not exist in cmdlist!!!!!", Cmd);
		Result = false;
		goto end;
	}

	m_Queue.erase(Cmd);
	Result = true;

end:
	LeaveCriticalSection(&m_Cs);
	return Result;
}

bool WaitQueue::SetWaitSuccess(UCHAR Cmd)
{
	return SetItemStatusAndRemove(Cmd);
}

bool WaitQueue::SetWaitFailed(UCHAR Cmd)
{
	return SetItemStatusAndRemove(Cmd);
}

void WaitQueue::Clear()
{
	EnterCriticalSection(&m_Cs);
	
	WaitContextQueue::iterator FirstItem = m_Queue.begin();
	WaitContextQueue::iterator LastItem = m_Queue.end();

	for (; FirstItem != LastItem; ++FirstItem)
	{
		SetEvent(FirstItem->second.m_hEvent);
	}

	m_Queue.clear();
	LeaveCriticalSection(&m_Cs);
}

bool WaitQueue::GetCmdBuffer(UCHAR Cmd, CHAR **OutputBuffer, int &OutputBufferLength)
{
	bool Result = false;
	WaitContextQueue::iterator CmdIte;

	EnterCriticalSection(&m_Cs);

	if ( !IsItemExist(Cmd) )
	{
		DbgPrint("GetCmdBuffer command %#x is not exist in cmdlist!!!!!", Cmd);
		Result = false;
		goto end;
	}

	CmdIte = m_Queue.find(Cmd);
	*OutputBuffer = CmdIte->second.m_OutputBuffer;
	OutputBufferLength = CmdIte->second.m_OutputLength;
	Result = true;
end:
	LeaveCriticalSection(&m_Cs);
	return Result;
}

bool WaitQueue::IsItemExist(UCHAR Cmd)
{
	bool Result = false;

	EnterCriticalSection(&m_Cs);
	Result = (m_Queue.end() != m_Queue.find(Cmd));
	LeaveCriticalSection(&m_Cs);
	return Result;
}

bool WaitQueue::SetItemStatusAndRemove(UCHAR Cmd)
{
	bool Result = false;
	WaitContextQueue::iterator CmdIte;

	EnterCriticalSection(&m_Cs);
	
	if ( !IsItemExist(Cmd) )
	{
		DbgPrint("SetItemStatusAndRemove Command %#x is not exist in cmdlist!!!!!", Cmd);
		Result = false;
		goto end;
	}

	CmdIte = m_Queue.find(Cmd);
	SetEvent(CmdIte->second.m_hEvent);
	
	//SetEvent(m_Queue[Cmd].m_hEvent);	
	Result = Remove(Cmd);
	if ( !Result )
	{
		goto end;
	}

	Result = true;

end:
	LeaveCriticalSection(&m_Cs);
	return Result;
}