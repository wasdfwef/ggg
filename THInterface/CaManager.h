#ifndef  _CAMANAGER_INCLUDE
#define  _CAMANAGER_INCLUDE

#include "..\inc\THInterface.h"

class   CCaManager
{
public:

	static KSTATUS SetCameraPrviewWindow(PCAMERA_PRVIEW_CONFIG   PrvCfg);
	static KSTATUS CaptureCameraGrap(UCHAR  nCaIndex,PCHAR  pFilePath);
};


#endif