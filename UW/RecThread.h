#ifndef  _RECTHREAD_INCLUDE
#define  _RECTHREAD_INCLUDE

#include "..\..\..\inc\QueueManager.h"


typedef
VOID
(CALLBACK*REC_PROGRESS_CALLBACK)(
PCHAR     pImageFolder,
USHORT    nGrapIndex,
UCHAR     nGrapSize,
PVOID     pContext,
ULONG     nProgressInfo
);

typedef
VOID
(CALLBACK*REFER_REQUEST_CALLBACK)(
PVOID  pContext,
ULONG  nTaskID
); 

typedef
VOID
(CALLBACK*REFER_PRE_REQUEST_CALLBACK)(
PVOID  pContext
);

class  CRecThread
{
	LONG                      m_ExitThread;
	ULONG                     m_CurTaskID;
	PVOID                     m_Context, m_PreContext;
	HANDLE                    m_recThread, m_referThread;
	HANDLE                    m_RequestEvent, m_PauseEvent, m_referEvent, m_UseRecEvent;
	QUEUE_HANDLE              m_RecQue;

	
	void                         ClearRecQue();
	REC_PROGRESS_CALLBACK        rec_cb;
	REFER_REQUEST_CALLBACK       refer_cb;
	REFER_PRE_REQUEST_CALLBACK   m_pre_request;  //显示细胞前期准备
	
public:
	 CRecThread();
	~CRecThread();

	//第一步：调用该API初始化
	BOOL               StartRecThread(REFER_REQUEST_CALLBACK refer_request_cb,REC_PROGRESS_CALLBACK   RecProCB, PVOID pContext); //调用者传递回调, pContext为用户上下文(popo传类指针)

	//第二步：当用户双击时,调用该方法.这个时候界面配合将光标设置成忙状态.在一定时间内,会调用refer_request_cb,被调用者在该回调中可以安全的进行第三步
	void               RefereNewRequest(ULONG  nTaskID, REFER_PRE_REQUEST_CALLBACK  pre_request = NULL, PVOID pContext = NULL);
	//第三步:该方法必须在refer_request_cb中进行调用,此时调用者可以将光标恢复成正常状态
	BOOL               InsertRecGrapRequest(ULONG nTaskID, PCHAR  pTaskImagePath, USHORT nGrapIndex, UCHAR nSize, ULONG nProgressInfo); //增加一个识别图片

	ULONG              GetCurTaskID();


	void               RecGrapProgress();  //系统使用
	void               RefereGrapProgress();

};


#endif