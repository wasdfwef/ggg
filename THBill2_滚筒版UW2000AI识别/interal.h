#ifndef   _INTERAL_INCLUDE
#define   _INTERAL_INCLUDE

#define  BILL_SAMPLING_PROGRESSING     0X1
#define  BILL_CANCEL_PROGRESSING       0X2

#define  TASK_STATUS_QC                0X1
#define  TASK_STATUS_TESTING           0X2
#define  TASK_STATUS_FAIL              0X4
#define  EXIT_THREAD(x)              (InterlockedExchangeAdd(&x, 0) != 0)

#pragma pack(1)

typedef  struct   _SAMPLING_PROGRESSING_STATUS
{
	char              GrapPath[MAX_PATH];
	ULONG             b_TaskStatus;
	ULONG             b_BillStatus;
	ULONG             nTaskID;
	HANDLE            m_TimeOutThread;
	int               nQCType;
	USHORT            nTotalSamplingNum;
	USHORT            nFailSamplingNum;
	USHORT            nCurrentSamplingNum;

}SAMPLING_PROGRESSING_STATUS, *PSAMPLING_PROGRESSING_STATUS;

typedef struct  _SAMPLE_10X_RANGE
{
	USHORT        nSample10xCount;
	PUCHAR        pSample10xIndexArray;

}SAMPLE_10X_RANGE, *PSAMPLE_10X_RANGE;


typedef  struct   _BILL3_EXTENSION
{
	PVOID                            InterFace;
	OPEN_DEV_INFORMATION             o_dev;
	SAMPLING_PROGRESSING_STATUS      prg_status;
	SAMPLE_10X_RANGE                 s10x_range;
	CRITICAL_SECTION                 cs;


}BILL3_EXTENSION, *PBILL3_EXTENSION;


#pragma pack()



#endif