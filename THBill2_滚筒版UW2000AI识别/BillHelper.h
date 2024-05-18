#ifndef     _BILLHELPER_INCLUDE
#define     _BILLHELPER_INCLUDE

#include "..\..\inc\THFrameDef.h"
#include "..\..\inc\THFrameDef.h"
#include "interal.h"
#include "QueueManager.h"
#include "..\..\inc\AnalysePic10x.h"


#define   MARK_BILL_PROGRESSING(x)   (x = x | BILL_SAMPLING_PROGRESSING)
#define   CLEAR_BILL_PROGRESSING(x)  (x &= (~BILL_SAMPLING_PROGRESSING))
#define   MARK_CANCEL_FLAG(x)        (x = x | BILL_CANCEL_PROGRESSING)
#define   CLEAR_CANCEL_FALG(x)       (x &= (~BILL_CANCEL_PROGRESSING))
#define   SET_TASK_QC(x)             (x = x | TASK_STATUS_QC)
#define   CANCEL_TASK_QC(x)          (x &= (~TASK_STATUS_QC))
#define   SET_TASK_TESTTING(x)       (x = x | TASK_STATUS_TESTING)
#define   CANCEL_TASK_TESTTING(x)    (x &= (~TASK_STATUS_TESTING))
#define   SET_TASK_FAIL(x)           (x = x | TASK_STATUS_FAIL)
#define   CANCEL_TASK_FAIL(x)        (x &= (~TASK_STATUS_FAIL))
#define   IS_TASK_TESTING(x)         ((BOOL)(x & TASK_STATUS_TESTING))
#define   IS_TASK_QC(x)              ((BOOL)(x & TASK_STATUS_QC))
#define   IS_TASK_FAIL(x)            ((BOOL)(x & TASK_STATUS_FAIL))

#define   IS_CANCEL_PROGRESSING(x)   ((BOOL)(x & BILL_CANCEL_PROGRESSING))
#define   IS_BILL_PROGRESSING(x)     ((BOOL)(x & BILL_SAMPLING_PROGRESSING))



class   CBillHelper
{


	void     Finish12sSampleStep(PROGRAM_OBJECT lc_prg_obj, ULONG nTotalSampleNumber, ULONG nCurrentSampleIndex);
	void     Stop12sSampleAction(PROGRAM_OBJECT lc_prg_obj);
	void     Reloc10xPos(PBILL3_EXTENSION pExtension);
	
	KSTATUS  StartCameraDevice(PROGRAM_OBJECT  caObj, PBILL3_EXTENSION pExtension);
	KSTATUS  StartUDCDevice(PROGRAM_OBJECT BillObj, PROGRAM_OBJECT PrgObj, PBILL3_EXTENSION pExtension);
	KSTATUS  StartLisDevice(PROGRAM_OBJECT PrgObj, PBILL3_EXTENSION pExtension);
	KSTATUS  StartLCDevice(PROGRAM_OBJECT BillObj, PROGRAM_OBJECT  lcObj, PBILL3_EXTENSION pExtension);
	KSTATUS  StartRECDevice(PROGRAM_OBJECT recObj, PBILL3_EXTENSION pExtension);
	KSTATUS  StartMMgrDevice();
	
	KSTATUS  SetDefaultSample10xRange(PBILL3_EXTENSION pExtension);


	LONG             m_ExitThread, m_FailOverUSTask;
	HANDLE           m_hPauseEvent;
	HANDLE           m_Request_Event;
	HANDLE           m_work_thread;
	QUEUE_HANDLE     m_PrgTask, m_USSampleTaskQue;
	PROGRAM_OBJECT   m_THMMgrObj, m_BillPrgObj;

public:

	CBillHelper();

	void     ClearTaskStatus(PBILL3_EXTENSION   pExtension);
	void     CloseAllDevice(PBILL3_EXTENSION  pExtension);
	void     GetRecGrapReslutPath(PCHAR   pPath);
	void     FinishSampleStep(PROGRAM_OBJECT  lc_prg_obj, ULONG nTotalSampleNumber, ULONG nCurrentSampleIndex);
	void     StopLCSampleAction(PROGRAM_OBJECT  lc_prg_obj);
	void     LCNormalClear(PROGRAM_OBJECT  lc_prg_obj);

	//系统机制

	void     SetFailOverTask(BOOL bFail);
	BOOL     QueryTaskOverByFail();
	BOOL     QueryThreadExit();
	KSTATUS  StartHelperDevice(PROGRAM_OBJECT StartPrgObj, TDEVICE_DESCRITOR  nDevDes);
	KSTATUS  InitializeStatusInformation(PROGRAM_OBJECT  BillPrgObj);
	KSTATUS  AddStatusInformation(InformationType i_type, ThmeInfoClass i_class, PVOID  pContent, ULONG nLength, BOOL bSysBuffer = TRUE, PVOID pContext = NULL);

	//配置读取
	KSTATUS  GetSample10xInfo(PBILL3_EXTENSION  pExtension);


	//任务相关

	void     RemoveTaskID(ULONG  nTaskID);
	KSTATUS  InsertNewTask(UCHAR  nHN, UCHAR nCN, BOOLEAN bTaskResult);
	KSTATUS  FillTaskID(PRESPOND_SAMPLE   pResSample);
	KSTATUS  UpdateUSStatus(ULONG nTaskID, PBOOL  pFullTaskFinish);
	KSTATUS  UpdateUDCStatus(ULONG  nTaskID, PBOOL pFullTaskFinish);
	
	

	//与下位机通信
	KSTATUS  SendTaskIDToLC(PRESPOND_SAMPLE  res_sample);


};



#endif