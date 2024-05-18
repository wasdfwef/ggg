#ifndef _COMSETSTRUCT_INCLUDE
#define _COMSETSTRUCT_INCLUDE

#include "..\..\inc\CommDef.h"


#pragma pack(1)


typedef  struct _DEV_TYPE_INFO
{
	TDEVICE_DESCRITOR DevType;

}DEV_TYPE_INFO,*PDEV_TYPE_INFO;

typedef  struct _PORT_DEV_INFO
{
	USHORT   nCounts;
	PDEV_TYPE_INFO  pDevTypeInfo;

}PORT_DEV_INFO, *PPORT_DEV_INFO;

typedef  struct _SYSTEM_SERIAL_INFO
{
	USHORT   nIndex;
	BOOLEAN  bUse;

}SYSTEM_SERIAL_INFO, *PSYSTEM_SERIAL_INFO;

#endif