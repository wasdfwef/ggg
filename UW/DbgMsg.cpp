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
	CHAR    DbgMsgText[8192], NewFormat[8192];
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
		OutputDebugStringA(DbgMsgText);

}


extern
VOID
TraceDbgNoSrcFileInfo(
LPCSTR  format,
...
)
{
	CHAR    DbgMsgText[8192];
	va_list argptr; 
	int     cnt; 
	
	LPCSTR MsgHeader = "UW2000: ";
	strcpy(DbgMsgText, MsgHeader);

	va_start(argptr, format);
	cnt = vsprintf(DbgMsgText + strlen(MsgHeader), format, argptr);
	va_end(argptr);
	if (cnt)
		OutputDebugStringA(DbgMsgText);
}