#ifndef  _TESTTYPE_INCLUDE
#define  _TESTTYPE_INCLUDE

#include "..\..\..\inc\THInterface.h"

class  CTestType
{
	LONG  m_cType;

public:
	CTestType();

	BOOL          UpdateCheckType();
	BOOL          UpdateCheckType(LONG  nCType);
	BOOL			UpdateCheckTypeEx(char* changeTestType = NULL);
	LONG          GetCheckType();
};

#endif