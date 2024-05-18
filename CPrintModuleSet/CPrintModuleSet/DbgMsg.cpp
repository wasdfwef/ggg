#include "StdAfx.h"
#include "DbgMsg.h"

extern
VOID
TraceDbgSrcFileInfo(
LPCSTR  format,
PCHAR   pSrcFile,
ULONG   nLines,
...
)
{
	CHAR    DbgMsgText[1024], NewFormat[1024];
	va_list argptr; 
	int cnt; 
	
	_try
	{
		sprintf(NewFormat, "%s Files = %s, Lines = %u\n",format, pSrcFile, nLines);
	}
	_except(EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}
	va_start(argptr, nLines); 
	cnt = vsprintf(DbgMsgText, NewFormat, argptr); 
	va_end(argptr); 
	if( cnt )
		OutputDebugString(DbgMsgText);

}


extern
VOID
TraceDbgNoSrcFileInfo(
LPCSTR  format,
...
)
{
	CHAR    DbgMsgText[1024];
	va_list argptr; 
	int     cnt; 
	

	va_start(argptr, format); 
	cnt = vsprintf(DbgMsgText, format, argptr); 
	va_end(argptr); 
	if( cnt )
		OutputDebugString(DbgMsgText);
	
}