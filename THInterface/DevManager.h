#ifndef  _DEVMANAGER_INCLUDE
#define  _DEVMANAGER_INCLUDE

#include "THInterface.h"
#include "THFrameDef.h"

class  CDevManager
{

#define DES_MASK_CODE 0XF

	static TDEVICE_DESCRITOR   DeviceDes2MainTypeDes(PROGRAM_OBJECT  PrgObj);
    static KSTATUS             FindBillPrgObject(PROGRAM_OBJECT   &tBillPrgObj, ULONG &nBillCounter);
	static KSTATUS             StartCameraDevice(PROGRAM_OBJECT  caObj, PDECIDE_START_DEVICE  pCADev);
	static KSTATUS             StartRECDevice(PROGRAM_OBJECT recObj, PDECIDE_START_DEVICE  pCADev);
	
public:
	static OPEN_DEV_INFORMATION& GetOIInfo();
	static PROGRAM_OBJECT      GetBillPrgObject();
	static KSTATUS             GetSpecialDevTypeObject(TDEVICE_DESCRITOR  dev_type, PROGRAM_OBJECT  &PrgObj);
	static KSTATUS             QuerySystemDev(PSYSTEM_DEVICE_SUPPORT  *sys_dev,PULONG       pCounter,BOOLEAN        bQueryStartDev);
	static KSTATUS             StartDev();
	static KSTATUS             StartDevForDebug(PDECIDE_START_DEVICE   pStartDev,ULONG                  nCounter);
	static KSTATUS             QueryDevCap(TDEVICE_DESCRITOR       dev_type,DevCapsInfoType info_type,PCHAR           pInputBuffer,ULONG           nInBufLength,PCHAR           pOutputBuffer,ULONG           nOutBufLength, USHORT nDevIndex);
	static KSTATUS             SetDevCap(TDEVICE_DESCRITOR       dev_type,DevCapsInfoType info_type,PCHAR           pInputBuffer,ULONG           nInBufLength, USHORT nDevIndex);
	static KSTATUS             CloseDev();


};


#endif