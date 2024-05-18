#include "StdAfx.h"
#include "TaskMgr.h"
#include <set>
#include <strsafe.h>
#include <windows.h>
#include "Include/Common/String.h"
#include "UW.h"

using namespace Common;

CTaskMgr::CTaskMgr(void)
{
}


CTaskMgr::~CTaskMgr(void)
{
}


static bool DeleteDir(const char *DirName)
{
	bool Result = false;
	char PathName[MAX_PATH] = {0};
	char PicFullPath[MAX_PATH] = {0};
	WIN32_FIND_DATAA FindFileData = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;

	StringCbCopyA(PathName, MAX_PATH, DirName);
	::PathAppendA(PathName, "*.*");

	hFind = FindFirstFileA(PathName, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		goto end;

	if( stricmp(FindFileData.cFileName, ".") != 0 && stricmp(FindFileData.cFileName, "..") != 0)
	{
		::PathCombineA(PicFullPath, DirName, FindFileData.cFileName);
		::DeleteFileA(PicFullPath);
	}

	while( FindNextFileA(hFind, &FindFileData) )
	{
		if( stricmp(FindFileData.cFileName, ".") != 0 && stricmp(FindFileData.cFileName, "..") != 0)
		{
			::PathCombineA(PicFullPath, DirName, FindFileData.cFileName);
			::DeleteFileA(PicFullPath);
		}
	}

end:
	if(hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	if( ::RemoveDirectoryA(DirName) )
		Result = true;

	return Result;
}

static int myDeleteDirectory(const char * directory_path) //删除一个文件夹下的所有内容
{
	CFileFind finder;
	CString path;
	path.Format(L"%s\\*.*", CharToWChar(directory_path));
	BOOL bWorking = finder.FindFile(path);
	while (bWorking){
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory() && !finder.IsDots()){//处理文件夹
			myDeleteDirectory(Common::WCharToChar(finder.GetFilePath().GetBuffer()).c_str()); //递归删除文件夹
			return RemoveDirectory(finder.GetFilePath());
		}
		else{//处理文件
			DeleteFile(finder.GetFilePath());
		}
	}
	RemoveDirectory(CharToWChar(directory_path).c_str());
}

#define COUNTLESS_TASK 0

bool CTaskMgr::DeleteExtraTasks()
{
	bool Result = true;
	DWORD TaskCount = 0;
	TDirList DirList;
	DEL_TASK del_info = {0};

	TDirList::iterator FirstDir;
	TDirList::iterator LastDir;

	GetTaskPicDirList(DirList);
	TaskCount = theApp.ReadSaveNums();

	if (COUNTLESS_TASK == TaskCount)
		goto end;

	FirstDir = DirList.begin();
	LastDir = DirList.end();

	if(DirList.size() <= TaskCount)
		goto end;

	for(int Index = 0; Index < TaskCount; ++Index)
		--LastDir;

	for( ; FirstDir != LastDir; ++FirstDir )
	{
		Result = myDeleteDirectory(FirstDir->c_str());
	}

end:
	return Result;
}


typedef set<int> TDirNamelist;

void CTaskMgr::GetTaskPicDirList(TDirList &DirList)
{
	char DirName[MAX_PATH] = {0};
	char PathName[MAX_PATH] = {0};
	char DirFullPath[MAX_PATH] = {0};
	char DirNameBuffer[10] = {0};
	WIN32_FIND_DATAA FindFileData = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;

	TDirNamelist DirNameList;
	TDirNamelist::iterator FirstDirName;
	TDirNamelist::iterator LastDirName;

	GetGraphicSubDirPath(PathName, MAX_PATH);
	StringCbCopyA(DirName, MAX_PATH, PathName);
	::PathAppendA(PathName, "*.*");

	hFind = FindFirstFileA(PathName, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		goto end;

	if( stricmp(FindFileData.cFileName, ".") != 0 && stricmp(FindFileData.cFileName, "..") != 0 && FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
	{
		DirNameList.insert( atoi(FindFileData.cFileName) );
	}

	while( FindNextFileA(hFind, &FindFileData) )
	{
		if( stricmp(FindFileData.cFileName, ".") != 0 && stricmp(FindFileData.cFileName, "..") != 0 && FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
		{
			DirNameList.insert( atoi(FindFileData.cFileName) );
		}
	}

	FirstDirName = DirNameList.begin();
	LastDirName = DirNameList.end();
	for( ; FirstDirName != LastDirName; ++FirstDirName )
	{
		itoa(*FirstDirName, DirNameBuffer, 10);
		::PathCombineA(DirFullPath, DirName, DirNameBuffer);
		DirList.push_back(DirFullPath);
	}

end:
	if(hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);
}


#define GRAPHIC_SUBDIR "RecGrapReslut"
void CTaskMgr::GetGraphicSubDirPath(char *PicPath, DWORD PicPathLength)
{
	::GetModuleFileNameA(NULL, PicPath, PicPathLength);
	::PathRemoveFileSpecA(PicPath);
	::PathAppendA(PicPath, GRAPHIC_SUBDIR);
}

bool CTaskMgr::m_CheckUDC = false;
bool CTaskMgr::m_CheckUS = false;
CTaskMgr::CTaskStatusList CTaskMgr::m_TaskStatusList;


CTaskMgr::CTaskStatusList::iterator CTaskMgr::FindTask(int TaskID)
{
	CTaskStatusList::iterator FirstTask = m_TaskStatusList.begin();
	CTaskStatusList::iterator LastTask = m_TaskStatusList.end();

	for (; FirstTask != LastTask; ++FirstTask)
	{
		if (FirstTask->TaskID != TaskID)
		{
			continue;
		}

		return FirstTask;
	}

	return LastTask;
}


void CTaskMgr::AddNewTaskImp(int TaskID, bool UDCFinish, bool USFinish)
{
	CTaskStatus NewTaskStatus;

	NewTaskStatus.TaskID = TaskID;
	NewTaskStatus.UDCFinish = UDCFinish;
	NewTaskStatus.USFinish = USFinish;
	NewTaskStatus.TaskFinish = false;

	//复测时，删除原来的信息
	CTaskStatusList::iterator TaskFind = FindTask(TaskID);
	if ( TaskFind != m_TaskStatusList.end() )
	{
		m_TaskStatusList.erase(TaskFind);
	}

	m_TaskStatusList.push_back(NewTaskStatus);
}


void CTaskMgr::AddNewTask(int TaskID)
{
	AddNewTaskImp(TaskID, false, false);
}


void CTaskMgr::CheckUDCOnly()
{
	m_CheckUDC = true;
	m_CheckUS = false;
}


void CTaskMgr::CheckUSOnly()
{
	m_CheckUS = true;
	m_CheckUDC = false;
}


void CTaskMgr::CheckBoth()
{
	m_CheckUS = true;
	m_CheckUDC = true;
}


bool CTaskMgr::FinishUDCCheck(int TaskID)
{
	CTaskStatusList::iterator TaskFind = FindTask(TaskID);
	if ( TaskFind == m_TaskStatusList.end() )
		return false;
	
	TaskFind->UDCFinish = true;

	if (!m_CheckUS)
	{
		TaskFind->TaskFinish = true;
	}
	else
	{
		if (TaskFind->USFinish)
		{
			TaskFind->TaskFinish = true;
		}
	}

	return true;
}



bool CTaskMgr::FinishUSCheck(int TaskID)
{
	CTaskStatusList::iterator TaskFind = FindTask(TaskID);
	if ( TaskFind == m_TaskStatusList.end() )
		return false;

	TaskFind->USFinish = true;

	if (!m_CheckUDC)
	{
		TaskFind->TaskFinish = true;
	}
	else
	{
		if (TaskFind->UDCFinish)
		{
			TaskFind->TaskFinish = true;
		}
	}

	return true;
}



bool CTaskMgr::IsTaskFinished(int TaskID)
{
	CTaskStatusList::iterator TaskFind = FindTask(TaskID);
	if ( TaskFind == m_TaskStatusList.end() )
		return false;

	return TaskFind->TaskFinish;
}
