#include "stdafx.h"
#include "CriticalSection.h"


CriticalSection::CriticalSection(CRITICAL_SECTION& lpCriticalSection)
{
	this->lpCriticalSection = lpCriticalSection;
	EnterCriticalSection(&this->lpCriticalSection);
}


CriticalSection::~CriticalSection()
{
	LeaveCriticalSection(&lpCriticalSection);
}
