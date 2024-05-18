#include "StdAfx.h"
#include "DogInfo.h"
#include "..\..\..\inc\Gsmh.h"
#pragma comment(lib,"..\\..\\..\\lib\\Win32dll.lib")
#include <Shlwapi.h>

CDogInfo::CDogInfo(void)
{	

}

CDogInfo::~CDogInfo(void)
{
}

BOOL CDogInfo::ChechDog()
{
	
	MH_DLL_PARA para = {0};

	para.Cascade = 0;
	para.DogPassword = 0;
	para.Command = 1;

	if( GS_MHDog(&para) != 0  || ChechDate() == FALSE )
	{		
		MessageBox(NULL,_T("无软件狗或软件狗信息错误"),_T("错误"),0);
		return FALSE;
	}
		
	return TRUE;
}

#define SUBKEY _T("DOGINFO")
#define REG_INFO_VALUE    _T("Reg")

#define CODELEN 64
void CDogInfo::CodeMC( PUCHAR pMcCode )
{
	USHORT r = 13;
	PUCHAR  p = (PUCHAR)&r;

	for( int j = 0 ; j < CODELEN ; j ++ )
		pMcCode[j] ^= p[j & 0x1];
}

ULONG CDogInfo::GetDogCurNo()
{
	ULONG nDogCurNo = 0;
	MH_DLL_PARA para = {0};

	para.Cascade = 0;
	para.DogPassword = 0;
	para.Command = 5;

	if( GS_MHDog(&para) != 0 )
	{	
		return 0;
	}
	else
	{	
		nDogCurNo = *(PULONG)(para.DogData);
		return nDogCurNo;
	}
}

BOOL CDogInfo::CheckMcSn( PCHAR pMcSn,ULONG nDogCurNo )
{
	CHAR ModePath[MAX_PATH] = {0},McSn[MAX_PATH] = {0};

	GetModuleFileNameA(NULL,ModePath,MAX_PATH);
	PathRemoveFileSpecA(ModePath);
	PathAppendA(ModePath,"Conf.ini");
	
	if( GetPrivateProfileStringA("McSN","sn","",McSn,MAX_PATH,ModePath) != 0 )
	{
		if( strcmp(McSn,pMcSn) != 0 )	
			return FALSE;
	}
	else
	{
		WritePrivateProfileStringA("McSN","sn",pMcSn,ModePath);		
	}

	if( GetPrivateProfileStringA("McSN","DogCurNo","",McSn,MAX_PATH,ModePath) != 0 )
	{
		if( atoi(McSn) == nDogCurNo )
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		sprintf(McSn,"%d",nDogCurNo);
		WritePrivateProfileStringA("McSN","DogCurNo",McSn,ModePath);
		return TRUE;
	}
}

BOOL CDogInfo::CheckType( PCHAR pMcCode,ULONG nDogCurNo )
{
	/*CHAR DogCurNo[21] = {0};

	for (int i = 0; i < CODELEN; i++ )
	{
		if( pMcCode[i] == '_')
		{
			memcpy(DogCurNo,pMcCode,i);
			break;
		}
	}
	if( atoi(DogCurNo) ==  nDogCurNo )
		return TRUE;
	else
		return FALSE;*/

	int   index = 0;
	CHAR  DogCurNo[21] = {0}, McType[21] = {0}, McSn[21] = {0};	
	char *p = NULL;

	p = strtok(pMcCode,"_");	
	strncpy(DogCurNo,p,sizeof(DogCurNo) - 1);
	while(p)
	{
		index++;
		p = strtok(NULL,"_");
		if( index == 1  && p )
			strncpy(McType,p,sizeof(McType) - 1);
		else if( index == 2  && p )
			strncpy(McSn,p,sizeof(McSn) - 1);
	}

	if( ( atoi(DogCurNo) ==  nDogCurNo ) && ( strcmp( McType, "UW2000") == 0 ) )
	{
		if( CheckMcSn(McSn,nDogCurNo) == TRUE )
			return TRUE;
		else
			return FALSE;
	}	
	else
		return FALSE;
}

BOOL CDogInfo::ChechDate()
{
	HKEY        m_hKey;
	MH_DLL_PARA para = {0};
	char        temp[200] = {0};
	DWORD       r,type, nLength = sizeof(temp);
	ULONG       nDogCurNo = GetDogCurNo();

	para.Cascade = 0;
	para.DogPassword = 0;
	para.Command = 2;
	para.DogAddr = 0;
	para.DogBytes = CODELEN;

	if( GS_MHDog(&para) != 0 )
	{		
		return FALSE;
	}
	else
	{
		CodeMC((PUCHAR)(para.DogData));
		return CheckType((PCHAR)(para.DogData),nDogCurNo);

		/*if( RegCreateKeyEx(HKEY_CLASSES_ROOT,SUBKEY,0,NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, &r) != NO_ERROR )
			return FALSE;
		if( m_hKey == NULL )
			return FALSE;
		if( RegQueryValueEx(m_hKey, REG_INFO_VALUE, NULL, &type, (PUCHAR)temp, &nLength) == NO_ERROR )
		{			
			if( strcmp( (char *)&para.DogData,temp) == 0 )
			{
				return TRUE;
			}				
			else
				return FALSE;
		}	
		else
			return FALSE;*/
	}
}




