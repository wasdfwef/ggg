#include "StdAfx.h"
#include "RecThread.h"
#include "DbgMsg.h"

#pragma pack(1)

typedef  struct _REC_REQUEST_CONTENT
{
	USHORT   nGrapIndex;
	UCHAR    nGrapSize;
	char     ImageFolder[MAX_PATH];
	ULONG    nProgressInfo;
	
}REC_REQUEST_CONTENT, *PREC_REQUEST_CONTENT;


#pragma pack()


CRecThread::CRecThread()
{
	m_PauseEvent   = NULL;
    m_recThread    = NULL;
	m_RequestEvent = NULL;
	m_RecQue       = NULL;
	m_referEvent   = NULL;
	m_referThread  = NULL;
	m_UseRecEvent  = NULL;
	m_pre_request  = NULL;
	m_PreContext   = NULL;
	m_ExitThread   = 0;
	m_CurTaskID    = 0xffffffff;
}

CRecThread::~CRecThread()
{
	InterlockedExchange((PLONG)&m_ExitThread, 1);

	
	if( m_PauseEvent )
		SetEvent(m_PauseEvent);


	if( m_referThread )
	{
		SetEvent(m_referEvent);
		WaitForSingleObject(m_referThread, INFINITE);
		CloseHandle(m_referThread);
	}


	if( m_recThread )
	{
		SetEvent(m_RequestEvent);
		WaitForSingleObject(m_recThread, INFINITE);
		CloseHandle(m_recThread);
	}

	if( m_RequestEvent )
		CloseHandle(m_RequestEvent);

	if( m_RecQue )
		Que_ReleaseList(m_RecQue);

	if( m_PauseEvent )
		CloseHandle(m_PauseEvent);

	if( m_referEvent )
		CloseHandle(m_referEvent);
	
}


void  CRecThread::RecGrapProgress()
{
	BOOL                   bRet;
	REC_REQUEST_CONTENT    rc;

	while( TRUE )
	{
		WaitForSingleObject(m_RequestEvent, INFINITE);
		if( InterlockedExchangeAdd((PLONG)&m_ExitThread, 0) )
			break;

		while( TRUE )
		{
			WaitForSingleObject(m_PauseEvent, INFINITE);   
			ResetEvent(m_UseRecEvent);
			bRet = Que_GetHeaderData(m_RecQue, &rc);
			
			if( InterlockedExchangeAdd((PLONG)&m_ExitThread, 0) || bRet == FALSE )
			{
				SetEvent(m_UseRecEvent);
				break;
			}
			rec_cb(rc.ImageFolder, rc.nGrapIndex, rc.nGrapSize, m_Context, rc.nProgressInfo);
			SetEvent(m_UseRecEvent);
		}
	}

}

static
void
RecWorkThread(CRecThread *rt)
{
	rt->RecGrapProgress();
}

static
void
ReferWorkThread(
CRecThread  *rt
)
{
	rt->RefereGrapProgress();

}

BOOL CRecThread::StartRecThread(REFER_REQUEST_CALLBACK refer_request_cb,REC_PROGRESS_CALLBACK   RecProCB, PVOID pContext)
{
	DWORD     dwThreadID;


	if( refer_request_cb == NULL || RecProCB == NULL )
		return FALSE;

	rec_cb    = RecProCB;
	refer_cb  = refer_request_cb;
	m_Context = pContext;

	m_RecQue = Que_Initialize(sizeof(REC_REQUEST_CONTENT));
	if( m_RecQue == NULL )
		return FALSE;

	m_UseRecEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if( m_UseRecEvent ==  NULL )
		return FALSE;

	m_RequestEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if( m_RequestEvent == NULL )
		return FALSE;

	m_PauseEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if( m_PauseEvent == NULL )
		return FALSE;

	m_referEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if( m_referEvent == NULL )
		return FALSE;


	m_recThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecWorkThread, this, 0, &dwThreadID);
	if( m_recThread == NULL )
		return FALSE;

	m_referThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReferWorkThread,this, 0, &dwThreadID);
	if( m_referThread == NULL )
		return FALSE;

	return TRUE;
}

void  CRecThread::RefereNewRequest(ULONG  nTaskID,REFER_PRE_REQUEST_CALLBACK  pre_request, PVOID pContext)
{
	ClearRecQue();
	m_pre_request = pre_request;
	m_PreContext  = pContext;
	InterlockedExchange((PLONG)&m_CurTaskID, nTaskID);
	SetEvent(m_referEvent);
}

void   CRecThread::ClearRecQue()
{
	ResetEvent(m_PauseEvent);
	while( Que_GetHeaderData(m_RecQue) );
}

BOOL  CRecThread::InsertRecGrapRequest(ULONG nTaskID, PCHAR  pTaskImagePath, USHORT nGrapIndex, UCHAR nSize,ULONG nProgressInfo)
{
	ULONG                tTaskID;
	REC_REQUEST_CONTENT  rc = {0};


	tTaskID = (ULONG)InterlockedExchangeAdd((PLONG)&m_CurTaskID, 0);
	if( tTaskID != nTaskID )
	{
		return FALSE;
	}

	strcpy(rc.ImageFolder, pTaskImagePath);
	rc.nGrapIndex = nGrapIndex;
	rc.nGrapSize  = nSize;
	rc.nProgressInfo  = nProgressInfo;

	if( Que_InsertData(m_RecQue, &rc) == TRUE )
	{
		SetEvent(m_RequestEvent);
		return TRUE;
	}
	return FALSE;
}

void  CRecThread::RefereGrapProgress()
{
	LONG     nTaskID;
	HANDLE   h_array[] = {m_referEvent, m_UseRecEvent};

	while( TRUE )
	{
		WaitForMultipleObjects(2, h_array, TRUE, INFINITE); //线程有空事件与新请求事件为TRUE时才会被执行.
//		WaitForSingleObject(m_referEvent, INFINITE);
		
		if( m_pre_request )
			m_pre_request(m_PreContext);

		nTaskID = InterlockedExchangeAdd((PLONG)&m_ExitThread, 0); //只负责进行清除
		if( nTaskID == -1 )
			continue;

		if( (ULONG)InterlockedExchangeAdd((PLONG)&m_CurTaskID, 0) )

		WaitForSingleObject(m_UseRecEvent, INFINITE);
		SetEvent(m_PauseEvent);
		refer_cb(m_Context, (ULONG)InterlockedExchangeAdd((PLONG)&m_CurTaskID, 0));

	}
}

ULONG CRecThread::GetCurTaskID()
{
	return  InterlockedExchangeAdd((PLONG)&m_CurTaskID, 0);
}
