//#include "StdAfx.h"
#include "DbgMsg.h"

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

	LPCSTR MsgHeader = "CmdHelper: ";
	strcpy(DbgMsgText, MsgHeader);

	va_start(argptr, format);
	cnt = vsprintf(DbgMsgText + strlen(MsgHeader), format, argptr);
	va_end(argptr); 
	if( cnt )
		OutputDebugString(DbgMsgText);
	
}