#ifndef   _REC_HELPER_INCLUDE
#define   _REC_HELPER_INCLUDE

#include "..\..\inc\THFrameDef.h"
#include "interal.h"
#include "RecMrg.h"

#define CELL_COUNT      "Count"
#define RESLUT_INI      "Result.ini"
#define STAND_FILE_LEN   12


#define   GET_OK    0
#define   NO_INFO   1
#define   IVA_INFO  2


class   CRecHelper: public CRecMgr
{
	
	void             BuildIdentify(PCELL_DETAIL_INFO  CellDetailInfo);
	int              VerifyGrapInfo(PCHAR &pSessionName);
	KSTATUS          DelRecTaskInfo(PDEL_REC_EX_INFO   del_rec);
	KSTATUS          DelRecResult(PDEL_REC_EX_INFO  grap_rc);

public:

	KSTATUS  QueryTaskInfoPre(PCHAR  pIniPath, ULONG  &nGrapSize);
	KSTATUS  QueryTaskInfo(PREC_GRAP_FULL_RECORD   grap_rc, PTASK_REC_INFO *pTaskInfo);
	KSTATUS  QueryCellInfo(PREC_GRAP_FULL_RECORD grap_rc, PCELL_FULL_INFO *pCellInfo);
	KSTATUS  ModifyCellInfoEntry(PCHANGE_MODIFY_INFO   cModifyInfo);
	KSTATUS  GetGrapFileName(PREC_GRAP_FULL_RECORD  grap_rc, PCHAR  pGrapFileName);
	KSTATUS  DelRecGrap(PDEL_REC_EX_INFO   del_rec);
	KSTATUS  AddRecCellGrap(PCHANGE_MODIFY_INFO  pChangeModifyInfo);
	KSTATUS  ModifyCellInfo(PCHANGE_MODIFY_INFO   cModifyInfo, ULONG nModifyCellItem);
	KSTATUS  QueryQCInfo(char*, PCELL_FULL_INFO);

	CRecHelper();
   ~CRecHelper();

	
	

};



#endif