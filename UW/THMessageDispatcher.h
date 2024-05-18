#pragma once

#include "ThreadX.h"
#include "MyEvent.h"
#include "../../../inc/ThLcProto_UW2000.h"

#pragma pack(1)
typedef struct CmdPar
{
	int8_t   cmd;
	CMyEvent event;
}ClassPair_t;
#pragma pack()

extern ClassPair_t g_eventArray[];



class CTHMessageDispatcher
	: public CThreadX
{
public:
	CTHMessageDispatcher(void);
	~CTHMessageDispatcher(void);

	void Stop();

	bool AddEventHandler(int cmd, CMyDelegate *d);
	bool RemoveEventHandler(int cmd, CMyDelegate *d);
	

private:
	void ThreadEntryPoint();

private:
	HANDLE	 m_ExitEvent;
	LONG     m_ExitCode;

	CMyEvent *GetCmdEvent(int cmd) const;
};
