#ifndef  _STATUS_MANAGER_INCLUDE
#define  _STATUS_MANAGER_INCLUDE

#include "..\inc\THInterface.h"

class  CStatusManager
{
public:

	static  KSTATUS GetSystemInformation(PTH_STATUS_INFORMATION  status_info,HANDLE   hExitEvent);
	static 	KSTATUS QuerySystemWorkStatus(PQC_SETTING   pTaskQC,PBOOL      pProgressing);
	static  KSTATUS SetSystemWorkStatus(PQC_SETTING  pTaskQC,PBOOL     pTaskTest,ULONG     nSamplingNumber,ULONG nCancelFlag);
	static  KSTATUS SetSystemWorkStatusEx(PSYS_WORK_STATUS   pSysWorkStatus);
};


#endif