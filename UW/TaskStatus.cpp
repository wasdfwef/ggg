#include "stdafx.h"
#include "TaskStatus.h"
#include "DbgMsg.h"

#define PIPE_OUT  0
#define PIPE_IN  1
LONG CTaskSTatus::m_IsPipeIn = PIPE_OUT;

CTaskSTatus::CTaskSTatus()
{
	m_task_status.clear();
	m_TaskStatusList.clear();
	InitializeCriticalSection(&m_cs);
}



void CTaskSTatus::SetTaskStatus(LONG  nTaskID, ULONG nTestType)
{
	TEMP_TASK_STATUS  temp_status;

	temp_status.nID     = nTaskID;
	temp_status.nStatus = nTestType;

	EnterCriticalSection(&m_cs);
	m_task_status.push_back(temp_status);

	CTaskStatusList::iterator FirstTask = m_TaskStatusList.begin();
	CTaskStatusList::iterator LastTask = m_TaskStatusList.end();

	for( ; FirstTask != LastTask; FirstTask++ )
	{
		if(FirstTask->nID == nTaskID )
		{
			m_TaskStatusList.erase(FirstTask);
			break;
		}
	}
	m_TaskStatusList.push_back(temp_status);
	LeaveCriticalSection(&m_cs);

}

void CTaskSTatus::ClearTaskStatus()
{
	EnterCriticalSection(&m_cs);


	m_task_status.clear();
	m_TaskStatusList.clear();
	PipeOut();
	LeaveCriticalSection(&m_cs);

}

BOOL CTaskSTatus::IsIdleStatus()
{
	BOOL   bRet;

	//EnterCriticalSection(&m_cs);
	bRet = ( PIPE_OUT == InterlockedExchangeAdd(&m_IsPipeIn, 0) ) && (m_task_status.size() == 0);
	//LeaveCriticalSection(&m_cs);
	//DBG_MSG("m_task_status.size() = %d\n", m_task_status.size());
	return bRet;
}

BOOL CTaskSTatus::IsCurTaskStatus( ULONG nTaskID )
{
	BOOL                bRet = FALSE;
	ULONG               j;

	EnterCriticalSection(&m_cs);

	for( j = 0 ; j < m_task_status.size() ; j ++ )
	{
		if( m_task_status[j].nID == nTaskID )
		{
			bRet = TRUE;
			break;
		}
	}
	LeaveCriticalSection(&m_cs);

	return bRet;

}


void CTaskSTatus::UpdateTaskStatus( ULONG nTaskID, ULONG nTestType )
{
	BOOL                bUpdated = FALSE;

	EnterCriticalSection(&m_cs);

	CTaskStatusList::iterator FirstTask = m_TaskStatusList.begin();
	CTaskStatusList::iterator LastTask = m_TaskStatusList.end();


	for( ; FirstTask != LastTask; FirstTask++ )
	{
		if(FirstTask->nID == nTaskID )
		{
			FirstTask->nStatus &= (~nTestType);
			break;
		}
	}

	FirstTask = m_task_status.begin();
	LastTask = m_task_status.end();

	for( ; FirstTask != LastTask; FirstTask++ )
	{
		if( FirstTask->nID == nTaskID )
		{
			bUpdated = TRUE;
			FirstTask->nStatus &= (~nTestType);
			break;
		}
	}

	if( bUpdated == TRUE && FirstTask->nStatus == 0 ) //没有任何状态
		m_task_status.erase(FirstTask);

	LeaveCriticalSection(&m_cs);

}


void CTaskSTatus::PipeIn()
{
	InterlockedExchange(&m_IsPipeIn, PIPE_IN);
}


void CTaskSTatus::PipeOut()
{
	InterlockedExchange(&m_IsPipeIn, PIPE_OUT);
}


bool  CTaskSTatus::IsTaskFinished(ULONG nTaskID)
{
	bool Result = true;
	EnterCriticalSection(&m_cs);

	for( int j = 0 ; j < m_TaskStatusList.size() ; j ++ )
	{
		if( m_TaskStatusList[j].nID != nTaskID )
			continue;

		Result = (m_TaskStatusList[j].nStatus == 0);
	}

	LeaveCriticalSection(&m_cs);
	return Result;
}

BOOL CTaskSTatus::IsNoTask()
{
	BOOL Result;

	EnterCriticalSection(&m_cs);
	Result =  ( m_task_status.size() == 0 );
	LeaveCriticalSection(&m_cs);

	return Result;
}