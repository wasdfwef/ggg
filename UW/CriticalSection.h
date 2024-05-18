#pragma once
class CriticalSection
{
public:
	CriticalSection(CRITICAL_SECTION& lpCriticalSection);
	~CriticalSection();

	CRITICAL_SECTION lpCriticalSection;
};
