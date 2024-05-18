#pragma once
class CMyWaitCursor
{
public:
	CMyWaitCursor(void);
	void Restore();
	void begin();
	~CMyWaitCursor(void);
};