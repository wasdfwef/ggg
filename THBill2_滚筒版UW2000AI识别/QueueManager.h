#ifndef    _QUEUE_MANAGER_INCLUDE
#define    _QUEUE_MANAGER_INCLUDE

#define QUEUE_HANDLE      HANDLE
#define QUEUE_FIND_CTRL   HANDLE


extern "C"
QUEUE_HANDLE
Que_Initialize(
ULONG   nDataSize = 0
);

extern "C"
BOOL
Que_InsertData(
QUEUE_HANDLE  q_handle,
PVOID         pUserData,
ULONG         nUserDataSize = 0,
BOOLEAN       bAddTail = TRUE
);

extern "C"
BOOL
Que_GetHeaderData(
QUEUE_HANDLE q_handle,
PVOID        pUserData = NULL
);

extern "C"
ULONG
Que_GetCounter(
QUEUE_HANDLE q_handle
);

extern "C"
BOOL
Que_GetCurUserDataSize(
QUEUE_HANDLE q_handle,
PULONG       pLen
);

extern "C"
QUEUE_FIND_CTRL
Que_CreateFindCtrl(
QUEUE_HANDLE q_handle
);

extern "C"
BOOL
Que_GetUserDataByFinding(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl,
PVOID            pUserData
);

extern "C"
VOID
Que_DeleteNodeByFinding(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl
);

extern "C"
VOID
Que_UpdateUserNodeByFinding(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl,
PVOID            pUserData
);

extern "C"
VOID
Que_UpdateUserLastNode(
QUEUE_HANDLE     q_handle,
PVOID            pUserData
);

extern "C"
BOOL
Que_SetNextNodeByFinding(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl
);

extern "C"
BOOL
Que_GetCurDataSizeByFind(
QUEUE_FIND_CTRL  q_find_ctrl,
PULONG           pUserDataSize
);

#define  QUE_FIND_NEXT     0
#define  QUE_FIND_OVER     1
#define  QUE_FIND_UPDATE   2
#define  QUE_FIND_DELETE   4
#define  QUE_FIND_OK       0X1000 //表示找到所需要的项目

typedef
ULONG
(CALLBACK*FINDCALLBACK)(
PVOID              pUserContext,
PVOID              pUserData
);

extern "C"
BOOL
Que_FindByCallBack(
QUEUE_HANDLE que_handle,
FINDCALLBACK find_callback,
PVOID        pUserContext
);


extern "C"
VOID
Que_CloseFind(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl
);

extern "C"
VOID
Que_ReleaseList(
QUEUE_HANDLE q_handle
);

#endif