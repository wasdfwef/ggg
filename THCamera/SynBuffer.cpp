#include "StdAfx.h"
#include "SynBuffer.h"


CSynBuffer::CSynBuffer()
{
	m_TransBuffer = NULL;
	m_CopyOverEvent = NULL;

}

CSynBuffer::~CSynBuffer()
{
	if( m_CopyOverEvent )
		CloseHandle(m_CopyOverEvent);

}

BOOL  CSynBuffer::Create()
{
	m_CopyOverEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if( m_CopyOverEvent == NULL )
		return FALSE;

	return TRUE;
}

BOOL  CSynBuffer::CopySynBuffer(PCHAR  pBuffer, ULONG  nBufferSize)
{
	PCHAR    pRawBuffer = (PCHAR)InterlockedExchangeAdd((PLONG)&m_TransBuffer, 0);

	if( pRawBuffer == NULL )
		return FALSE;
	
	InterlockedExchange((PLONG)&m_TransBuffer, 0);
	memcpy(pRawBuffer, pBuffer, nBufferSize);
	SetEvent(m_CopyOverEvent);

	return TRUE;
	
}

#define WAIT_TIME 3 * 1000

BOOL  CSynBuffer::SetSynBufferAndWait(PCHAR  pSynBuffer)
{
	ResetEvent(m_CopyOverEvent);

	InterlockedExchange((PLONG)&m_TransBuffer, (LONG)pSynBuffer);
	if( WaitForSingleObject(m_CopyOverEvent, WAIT_TIME) == WAIT_TIMEOUT )
		return FALSE;


	return TRUE;

}