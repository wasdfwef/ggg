#pragma once

#include "..\..\..\inc\THDBUW_Access.h"



typedef struct _READ_PAR
{
	bool  bIsString;
	bool  bIsfloat;
	int   nValue;
	float fValue;
	char  sAppName[MAX_PATH];
	char  sKeyName[MAX_PATH];
	char  sString[MAX_PATH];

}READ_PAR,*PREAD_PAR;


typedef struct _WRITE_PAR
{
	char  sAppName[MAX_PATH];
	char  sKeyName[MAX_PATH];
	char  sString[MAX_PATH];

}WRITE_PAR,*PWRITE_PAR;



class CIniParser
{
public:
	CIniParser(void);
	~CIniParser(void);



public:


	BOOL  Initialize(char* pConfig_Ini_FileName);
	void  Uninitialize(void);


	KSTATUS Read(READ_PAR*  pread_par);
	KSTATUS Write(WRITE_PAR write_par);


	void EmptyByAppName(char* sAppName);


private:

	char m_szFileName[MAX_PATH];

};
