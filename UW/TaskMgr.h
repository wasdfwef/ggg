#pragma once

#include <string>
#include <vector>

using namespace std;

class CTaskMgr
{
public:
	CTaskMgr(void);
	~CTaskMgr(void);

	static bool DeleteExtraTasks();

private:
	static void AddNewTask(int TaskID);

	static void CheckUDCOnly();
	static void CheckUSOnly();
	static void CheckBoth();

	static bool FinishUDCCheck(int TaskID);
	static bool FinishUSCheck(int TaskID);
	static bool IsTaskFinished(int TaskID);


	typedef vector<string> TDirList;

	struct CTaskStatus
	{
		int TaskID;
		bool UDCFinish;
		bool USFinish;
		bool TaskFinish;
	};

	typedef vector<CTaskStatus> CTaskStatusList;

private:
	

	static bool m_CheckUDC;
	static bool m_CheckUS;

	static CTaskStatusList m_TaskStatusList;
	static CTaskStatusList::iterator FindTask(int TaskID);
	static void AddNewTaskImp(int TaskID, bool UDCFinish, bool USFinish);


	static void GetTaskPicDirList(TDirList &DirList);
	static void GetGraphicSubDirPath(char *PicPath, DWORD PicPathLength);
};
