#ifndef    _INTERAL_INCLUDE
#define    _INTERAL_INCLUDE


#pragma pack(1)

typedef  struct DEV_EXTENSION
{
	//BOOL             bActiveStatus;
	//ULONG            Version;
	//LONG             ForceClearStatus;
	//ULONG            TimerID;
	HANDLE           hSport;
	PROGRAM_OBJECT   BillPrgObject;
	PROGRAM_OBJECT   SPortPrgObject;
	
}*PDEV_EXTENSION;

#pragma pack()


#endif