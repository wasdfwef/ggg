#ifndef  _MSGMANGER_INCLUDE
#define  _MSGMANGER_INCLUDE

#include "..\inc\THInterface.h"

class  CMsgManager
{
public:
	static 	KSTATUS SetUserLevelInforamtion(PVOID  pContext,PCHAR  pBuffer,ULONG  nBufferLength);
};

#endif