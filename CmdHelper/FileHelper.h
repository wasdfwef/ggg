#ifndef FILE_HELPER_ADFASDFADFA
#define FILE_HELPER_ADFASDFADFA

#include <Windows.h>
#include <vector>
#include <string>

using namespace std;


typedef vector<string> CFilePathList;

void GetConfigFilePath(char *ConfigFilePath, int ConfigFilePathLength, const char *SubPath);

void GetAllFilePath(const char *DirPath, CFilePathList &FilePathList);

void DeleteAllFile(const char *DirPath);



#endif