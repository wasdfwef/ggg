#ifndef  _UDCARC_INCLUDE
#define  _UDCARC_INCLUDE

#include "..\inc\THFrameDef.h"
#include "interal.h"
#include "QueueManager.h"
#include "BillHelper.h"

class  CUDCArc
{

	
	PROGRAM_OBJECT  m_BillPrgObj;

public:

	CUDCArc();
	KSTATUS  InitializeUDCArc(PROGRAM_OBJECT  BillPrgObj, CBillHelper *pBillHelper);
	KSTATUS  StartUDCEvent(ULONG  nTaskID);
	CBillHelper    *m_BillHelpler;
};



#endif