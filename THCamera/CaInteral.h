#ifndef   _CAINTERAL_INCLUDE
#define   _CAINTERAL_INCLUDE

#define   MAX_DEV_INFO_COUNTER 16

#include "CameraMgr.h"

#pragma pack(1)

typedef  struct _CA_EXTENSION
{
	CCameraMgr   *cCaMgr;

}CA_EXTENSION, *PCA_EXTENSION;

#pragma pack()


#endif