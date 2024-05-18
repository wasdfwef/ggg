// FileHelper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "FileHelper.h"
#include <shlwapi.h>
#include <strsafe.h>


#pragma comment(lib, "shlwapi.lib")


void GetConfigFilePath(char *ConfigFilePath, int ConfigFilePathLength, const char *SubPath)
{
	GetModuleFileNameA(NULL, ConfigFilePath, ConfigFilePathLength);
	PathRemoveFileSpecA(ConfigFilePath);
	PathAppendA(ConfigFilePath, SubPath);
}


static void AddOneFilePath(const char *DirPath, const WIN32_FIND_DATA &FindFileData, CFilePathList &FilePathList)
{
	char FileFullPath[MAX_PATH] = {0};

	if( stricmp(FindFileData.cFileName, ".") != 0 && stricmp(FindFileData.cFileName, "..") != 0 )
	{
		::PathCombineA(FileFullPath, DirPath, FindFileData.cFileName);
		FilePathList.push_back(FileFullPath);
	}
}


void GetAllFilePath(const char *DirPath, const char *FileType, CFilePathList &FilePathList)
{
	char PathName[MAX_PATH] = {0};
	
	WIN32_FIND_DATA FindFileData = {0};
	HANDLE hFind = INVALID_HANDLE_VALUE;
	
	StringCbCopyA( PathName, MAX_PATH, DirPath);
	::PathAppend(PathName, FileType);
	
	hFind = FindFirstFile(PathName, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		goto end;
	
	AddOneFilePath(DirPath, FindFileData, FilePathList);
	
	while( FindNextFile(hFind, &FindFileData) )
	{
		AddOneFilePath(DirPath, FindFileData, FilePathList);
	}
	
end:
	if(hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);
}


void DeleteAllFile(const char *DirPath)
{
	CFilePathList FilePathList;

	GetAllFilePath(DirPath, "*.*", FilePathList);

	CFilePathList::iterator FirstFile = FilePathList.begin();
	CFilePathList::iterator LastFile = FilePathList.end();
	for ( ; FirstFile != LastFile; ++FirstFile )
	{
		DeleteFile( FirstFile->c_str() );
	}
}

