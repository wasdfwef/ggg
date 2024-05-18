#pragma once


class CDogInfo
{
public:
	CDogInfo(void);
	~CDogInfo(void);


public:
	BOOL  ChechDog();
	BOOL  ChechDate();
	void  CodeMC(PUCHAR pMcCode);
	BOOL  CheckType(PCHAR pMcCode,ULONG nDogCurNo);
	ULONG GetDogCurNo();
	BOOL  CheckMcSn(PCHAR pMcSn,ULONG nDogCurNo);
};
