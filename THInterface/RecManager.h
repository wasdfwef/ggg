#ifndef    _RECMANAGER_INCLUDE
#define   _RECMANAGER_INCLUDE

#include "..\inc\THInterface.h"
#include <atlstr.h>

class  CRecManager
{
	static  KSTATUS GetRecGrapPath(PVOID  pCodePath, PCHAR  pRecGrapPath);

public:

	static  KSTATUS AddRecCellInfo(PVOID  pCodePath,USHORT   nGrapIndex,UCHAR    nGrapSize,PCELL_DETAIL_INFO  pCellDetailInfo);
	static  KSTATUS DeleteRecCellInfo(PVOID  pCodePath, PREC_GRAP_RECORD   pDelRecGrapRecord, PCELL_IDENTIFY     pCellIdentify,BOOL               bDelAllRecInfo);
	static  KSTATUS	QueryRecCellInfo(PVOID  pCodePath,USHORT   nGrapIndex,UCHAR  nGrapSize,PCELL_FULL_INFO   *pRecCellInfo);
	static  KSTATUS QueryRecGrapInfo(PVOID  pCodePath,PTASK_REC_INFO *pRecInfo);
	static  KSTATUS ModifyRecCellInfo(PVOID  pCodePath,USHORT   nGrapIndex,	UCHAR   nGrapSize,PCELL_DETAIL_INFO  pCellDetailInfo);
	
};

BOOL isPATH(PVOID);
#endif