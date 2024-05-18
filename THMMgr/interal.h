#ifndef   _INTERAL_INCLUDE
#define   _INTERAL_INCLUDE

#include "QueueManager.h"

#pragma pack(1)

typedef  struct _MMGR_EXTENSION
{
	QUEUE_HANDLE    que_handle;
	HANDLE          hSystemReqeustEvent;

}MMGR_EXTENSION, *PMMGR_EXTENSION;



#pragma pack()


#endif
