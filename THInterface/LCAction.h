#ifndef  _LCACTION_INCLUDE
#define  _LCACTION_INCLUDE

#include "..\inc\THInterface.h"
#include "..\inc\THFrameDef.h"
#include "..\inc\lc_interface.h"
#include "..\inc\CommDef.h"


class  CLCAction
{


public:

	static KSTATUS AutoDetectLCConfig(USHORT      nPortIndex, TDEVICE_DESCRITOR   dev_type);
	static KSTATUS SendlCCmdActionEx(ULONG       ActionCmd,PCHAR       pInBuffer,ULONG       nInLength,PCHAR       pOutBuffer,ULONG       nOutLength, TDEVICE_DESCRITOR   dev_type);

	
};


#endif