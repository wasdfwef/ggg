#ifndef   _TASK_STATUS_INCLUDE
#define   _TASK_STATUS_INCLUDE

#define  NO_TASK_STATUS   -1
#include <vector>

class CTaskSTatus
{

#pragma pack(1)

typedef struct _TEMP_TASK_STATUS
{
	ULONG   nID;
	ULONG   nStatus;

}TEMP_TASK_STATUS, *PTEMP_TASK_STATUS;


#pragma pack()

	static LONG m_IsPipeIn;
    CRITICAL_SECTION  m_cs;

	typedef std::vector<TEMP_TASK_STATUS> CTaskStatusList;
	CTaskStatusList m_task_status;
	CTaskStatusList m_TaskStatusList;


public:
	CTaskSTatus();

	void  SetTaskStatus(LONG  nTaskID, ULONG nTestType);
	void  ClearTaskStatus();
	void  UpdateTaskStatus(ULONG nTaskID, ULONG nTestType);
	BOOL  IsIdleStatus();
	BOOL  IsCurTaskStatus(ULONG nTaskID);
	bool  IsTaskFinished(ULONG nTaskID);
	BOOL  IsNoTask();

	static void PipeIn();
	static void PipeOut();

};



#endif