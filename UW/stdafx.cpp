
// stdafx.cpp : 只包括标准包含文件的源文件
// UW.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include <string>
#include <vector>
#include <locale>
#include <io.h>

// 需包含locale、string头文件、使用setlocale函数。
std::wstring StringToWstring(const std::string str)
{// string转wstring
	unsigned len = str.size() * 2;// 预留字节数
	auto locale = setlocale(LC_CTYPE, "");     //必须调用此函数
	wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
	mbstowcs(p, str.c_str(), len);// 转换
	std::wstring str1(p);
	delete[] p;// 释放申请的内存
	setlocale(LC_CTYPE, locale);
	return str1;
}

std::string WstringToString(const std::wstring str)
{// wstring转string
	unsigned len = str.size() * 4;
	auto locale = setlocale(LC_CTYPE, "");
	char *p = new char[len];
	wcstombs(p, str.c_str(), len);
	std::string str1(p);
	delete[] p;
	setlocale(LC_CTYPE, locale);
	return str1;
}

//使用：Convert(strA_in,strB_out,CP_UTF8,CP_ACP)//UTF8转换ANSI
// Convert(strA_out, strB_in, CP_ACP, CP_UTF8)//ANSI转换UTF8
int Convert(const char* strIn, char** strOut, int sourceCodepage, int targetCodepage)
{
	int len = strlen(strIn);
	int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strIn, -1, NULL, 0);

	wchar_t* pUnicode = NULL;
	pUnicode = new wchar_t[unicodeLen];
	memset(pUnicode, 0, (unicodeLen)*sizeof(wchar_t));
	MultiByteToWideChar(sourceCodepage, 0, strIn, -1, (LPWSTR)pUnicode, unicodeLen);

	char * pTargetData = NULL;
	int targetLen = WideCharToMultiByte(targetCodepage, 0, (LPWSTR)pUnicode, -1, NULL, 0, NULL, NULL);

	pTargetData = new char[targetLen + 1];
	memset(pTargetData, 0, targetLen + 1);
	WideCharToMultiByte(targetCodepage, 0, (LPWSTR)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);

	//strcpy(strOut, (char*)pTargetData);

	delete pUnicode;
	//delete pTargetData;
	*strOut = pTargetData;

	return targetLen;
}

int GBKToUTF8(const char* strIn, char **strOut)
{
	return Convert(strIn, strOut, CP_ACP, CP_UTF8);
}
int UTF8ToGBK(const char* strIn, char **strOut)
{
	return Convert(strIn, strOut, CP_UTF8, CP_ACP);
}


void SplitString(const std::string& s, const std::string& c, std::vector<std::string>& v)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

std::string trim(std::string& s)
{
	const std::string drop = " ";
	// trim right
	s.erase(s.find_last_not_of(drop) + 1);
	// trim left
	return s.erase(0, s.find_first_not_of(drop));
}

std::string ltrim(std::string& s)
{
	const std::string drop = " ";
	// trim left
	return s.erase(0, s.find_first_not_of(drop));
}

std::string rtrim(std::string& s)
{
	const std::string drop = " ";
	// trim right
	return    s.erase(s.find_last_not_of(drop) + 1);

}

// 将pc中的文件夹从一个目录拷贝到另外的一个目录
BOOL CopyDirectory(std::string strSrcPath, std::string strDesPath, BOOL bFailIfExists, BOOL bRecursiveCopy)
{
	if (strSrcPath.empty())
	{
		return FALSE;
	}
	int ret = _access(strSrcPath.c_str(), 0);
	if (ret)
	{
		return FALSE;
	}
	ret = _access(strDesPath.c_str(), 0);
	if (ret )
	{
		if (!CreateDirectoryA(strDesPath.c_str(), 0))
			return FALSE;
	}

	if (strSrcPath.at(strSrcPath.length() - 1) != '\\')
		strSrcPath += '\\';
	if (strDesPath.at(strDesPath.length() - 1) != '\\')
		strDesPath += '\\';

	BOOL bRet = FALSE; // 因为源目录不可能为空，所以该值一定会被修改
	std::string src = strSrcPath;
	src.append("*");
	CFileFind ff;
	BOOL bFound = ff.FindFile(StringToWstring(src).c_str(), 0);
	while (bFound)      // 递归拷贝
	{
		bFound = ff.FindNextFile();
		if (ff.GetFileName() == "." || ff.GetFileName() == "..")
			continue;

		CString strSubSrcPath = ff.GetFilePath();
		CString strSubDespath = strSubSrcPath;
		strSubDespath.Replace(StringToWstring(strSrcPath).c_str(), StringToWstring(strDesPath).c_str());

		if (ff.IsDirectory())
		{
			if (bRecursiveCopy)
			{
				bRet = CopyDirectory(WstringToString(strSubSrcPath.GetBuffer()), WstringToString(strSubDespath.GetBuffer()), bFailIfExists, bRecursiveCopy);     // 递归拷贝文件夹
			}
		}
		else
		{
			bRet = CopyFile(strSubSrcPath, strSubDespath, bFailIfExists);   // 拷贝文件
		}
		if (!bRet)
			break;
	}
	ff.Close();
	return bRet;
}
