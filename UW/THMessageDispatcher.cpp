#include "StdAfx.h"
#include "THMessageDispatcher.h"
#include "DbgMsg.h"
#include "UW.h"
#include <map>

using std::map;

ClassPair_t g_eventArray[] = {
	{ LcReStarting,               CMyEvent() },
	{ LcReStartEnd,               CMyEvent() },
	{ DetectedPipeAndIn,          CMyEvent() },
	{ PipeInSuccess,              CMyEvent() },
	{ LcNewTask,				  CMyEvent() }, 
	{ RespondTaskFail,            CMyEvent() },
	{ TaskActionFinish,			  CMyEvent() },
	{ PipeOutShelf,               CMyEvent() },
	{ LcNewEmergencyTask,         CMyEvent() },
	{ CaptureImageRequest,        CMyEvent() },
	{ BarcodeRequest,			  CMyEvent() },
	{ UdcResult,                  CMyEvent() },
	{ SensorTestData,             CMyEvent() },
	{ UdcSelAndFeedPaperTestData, CMyEvent() },
	{ UdcRepeatTestData,          CMyEvent() },
	{ LcAlarm,                    CMyEvent() },
	{ SamplingProgress,			  CMyEvent() },
	{ RecognitionProgress,        CMyEvent() },
	{ SampleTaskFinish,           CMyEvent() },
	{ TaskFocus,				  CMyEvent() },
	{ PhysicsResult,              CMyEvent() },
	{ PhysicsSgSensor,            CMyEvent() },
};

void trimString(std::string & str, std::string & result)
{
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ") + 1);

	result = str;
}

CTHMessageDispatcher::CTHMessageDispatcher(void)
{
	m_ExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CTHMessageDispatcher::~CTHMessageDispatcher(void)
{
	Stop();
	CloseHandle(m_ExitEvent);
}

void CTHMessageDispatcher::Stop()
{
	SetEvent(m_ExitEvent);
	InterlockedExchange(&m_ExitCode, 1);
}

bool CTHMessageDispatcher::AddEventHandler(int cmd, CMyDelegate *d)
{

	CMyEvent *event = GetCmdEvent(cmd);
	if (event) 
	{
		event->AddHandler(d);
	}

	return (event!=NULL);
}

bool CTHMessageDispatcher::RemoveEventHandler(int cmd, CMyDelegate *d)
{
	CMyEvent *event = GetCmdEvent(cmd);
	if (event)
	{
		event->RemoveHandler(d);
	}

	return (event != NULL);
}

CMyEvent * CTHMessageDispatcher::GetCmdEvent(int cmd) const
{
	int nSize = sizeof(g_eventArray) / sizeof(ClassPair_t);
	for (int i = 0; i < nSize; i++)
	{
		if (g_eventArray[i].cmd == cmd)
		{
			return &g_eventArray[i].event;
		}
	}
	return nullptr;
}

void CTHMessageDispatcher::ThreadEntryPoint()
{
	KSTATUS                   status;
	TH_STATUS_INFORMATION     s_info;

	while( TRUE )
	{
		ZeroMemory(&s_info, sizeof(s_info));

		status = THInterface_GetSystemInformation(&s_info, m_ExitEvent);

		if( InterlockedExchangeAdd(&m_ExitCode, 0) != 0 )
			break;

		if( status != STATUS_SUCCESS )
			continue;

		CMyEvent *event = GetCmdEvent(s_info.i_class);
		if (event != nullptr)
		{
			event->OnEvent(s_info.content.buffer);
		}
		
		if (s_info.content.bSysBuffer)
		{
			THInterface_FreeSysBuffer(s_info.content.buffer);
		}
	}
}

