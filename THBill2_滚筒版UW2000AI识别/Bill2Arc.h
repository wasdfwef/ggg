#ifndef  _BILL2ARC_INCLUDE
#define  _BILL2ARC_INCLUDE

#include "..\..\inc\THInterface.h"
#include "..\..\inc\THFrameDef.h"
#include "interal.h"
#include "BillHelper.h"
#include "USArc.h"
#include "UDCArc.h"

////////////////////////////
#pragma pack(1)
typedef struct _LC_REPORT_INFORMATION
{
	LC_HEADER   lc_header;
	USHORT      nInfomationType;
	char        Reserved[32];

}LC_REPORT_INFORMATION, *PLC_REPORT_INFORMATION;

#pragma pack()
//////////////////////////////

class  CBill2Arc : public CBillHelper, public CUSArc, public CUDCArc
{
private:

	KSTATUS   FindAppointDev(PROGRAM_OBJECT   BillPrgObj, PCHAR  pDevName, TDEVICE_DESCRITOR   DevDes);
	KSTATUS   ReportStatus(PLC_REPORT_INFORMATION   report_info);
	KSTATUS   LCStartStatus();
	VOID      StopRelativeDevice(PROGRAM_OBJECT PrgObj);
	

public:
	
	void      CloseDev(PROGRAM_OBJECT BillPrgObj);
	KSTATUS   StartBillReatliveDev(PROGRAM_OBJECT   BillPrgObj);
	KSTATUS   LCProtoDispatch(PROGRAM_OBJECT  BillPrgObj, PLC_COMMAND lc_cmd);
	KSTATUS   SetWorkStatus(PROGRAM_OBJECT BillPrgObj, PSYS_WORK_STATUS  sys_work_status);
	


};



#endif