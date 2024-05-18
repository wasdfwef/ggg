// QueueManager.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "QueueManager.h"
#include "Que_Struct.h"
#include <malloc.h>


extern "C"
QUEUE_HANDLE
Que_Initialize(
ULONG   nDataSize
)
{
	PQUE_STRUCT   QueStruct;

	QueStruct = (PQUE_STRUCT)malloc(sizeof(QUE_STRUCT));

	if( QueStruct == NULL )
		return NULL;

	ZeroMemory(QueStruct, sizeof(QUE_STRUCT));
	QueStruct->hSynEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if( QueStruct->hSynEvent == NULL )
	{
		free(QueStruct);
		return NULL;
	}

	QueStruct->nBlockSize = nDataSize;

	return QueStruct;
}

extern "C"
BOOL
Que_InsertData(
QUEUE_HANDLE  q_handle,
PVOID         pUserData,
ULONG         nUserDataSize,
BOOLEAN       bAddTail
)
{
	PVOID          p;
	ULONG          nBlockSize;
	PQUE_STRUCT    q_struct = (PQUE_STRUCT)q_handle;
	PLIST_STRUCT   pList;

	if( q_struct == NULL || pUserData == NULL )
	{
		return FALSE;
	}

	WaitForSingleObject(q_struct->hSynEvent, INFINITE);

	if( q_struct->nBlockSize == 0 && nUserDataSize == 0)
	{
		SetEvent(q_struct->hSynEvent);
		return FALSE;
	}

	pList = (PLIST_STRUCT)malloc(sizeof(LIST_STRUCT));
	if( pList )
	{
		nBlockSize = q_struct->nBlockSize;
		if( nBlockSize == 0 )
			nBlockSize = nUserDataSize;

		p = malloc(nBlockSize);
		if( p == NULL )
		{
			free(pList);
			pList = NULL;
			goto MEM_FAIL;
		}

		ZeroMemory(pList, sizeof(LIST_STRUCT));

		if( q_struct->nBlockSize )
		{
			memcpy(p, pUserData, q_struct->nBlockSize);
			pList->nUserDataSize = q_struct->nBlockSize;
		}
		else
		{
			memcpy(p, pUserData, nUserDataSize);
			pList->nUserDataSize = nUserDataSize;
		}
		
		if( q_struct->pHeaderList == NULL )
		{
			q_struct->pHeaderList = pList;
			q_struct->pLastList   = pList;
		}
		else
		{
			if( bAddTail == TRUE )
			{
				q_struct->pLastList->pNextList = pList;
				q_struct->pLastList = pList;
			}
			else
			{
				pList->pNextList      = q_struct->pHeaderList;
				q_struct->pHeaderList = pList;
			}
		}

		pList->pUserNode = p;
		q_struct->nCounter++;
	}

MEM_FAIL:
	SetEvent(q_struct->hSynEvent);
	if( pList )
		return TRUE;

	return FALSE;

}

extern "C"
BOOL
Que_GetHeaderData(
QUEUE_HANDLE q_handle,
PVOID        pUserData
)
{
	BOOL           bRet     = TRUE;
	PQUE_STRUCT    q_struct = (PQUE_STRUCT)q_handle;
	PLIST_STRUCT   pList;

	if( q_struct == NULL )
		return FALSE;

	WaitForSingleObject(q_struct->hSynEvent, INFINITE);
	if( q_struct->pHeaderList )
	{
		pList = q_struct->pHeaderList;

		q_struct->pHeaderList = pList->pNextList;
		if( q_struct->pHeaderList == NULL )
			q_struct->pLastList = NULL;

		if( pUserData )
			memcpy(pUserData, pList->pUserNode, pList->nUserDataSize);

		free(pList->pUserNode);
		free(pList);
		q_struct->nCounter--;
	}
	else
	{
		bRet = FALSE;
	}

	SetEvent(q_struct->hSynEvent);
	return bRet;
}

extern "C"
VOID
Que_ReleaseList(
QUEUE_HANDLE q_handle
)
{
	PQUE_STRUCT    q_struct = (PQUE_STRUCT)q_handle;
	PLIST_STRUCT   pListStruct;

	if( q_struct == NULL )
		return;

	WaitForSingleObject(q_struct->hSynEvent, INFINITE);

	while( q_struct->pHeaderList )
	{
		pListStruct = q_struct->pHeaderList;
		q_struct->pHeaderList = pListStruct->pNextList;
		free(pListStruct->pUserNode);
		free(pListStruct);
	}

	SetEvent(q_struct->hSynEvent);
	CloseHandle(q_struct->hSynEvent);
	free(q_struct);

}


extern "C"
ULONG
Que_GetCounter(
QUEUE_HANDLE q_handle
)
{
	ULONG          nCounter;
	PQUE_STRUCT    q_struct = (PQUE_STRUCT)q_handle;

	if( q_struct == NULL )
		return 0;

	WaitForSingleObject(q_struct->hSynEvent, INFINITE);
	nCounter = q_struct->nCounter;
	SetEvent(q_struct->hSynEvent);
	return nCounter;

}

extern "C"
QUEUE_FIND_CTRL
Que_CreateFindCtrl(
QUEUE_HANDLE q_handle
)
{
	PQUE_STRUCT       q_struct = (PQUE_STRUCT)q_handle;
	PFIND_NODE_CTRL   find_ctrl;

	if( q_handle == NULL )
		return NULL;

	find_ctrl = (PFIND_NODE_CTRL)malloc(sizeof(FIND_NODE_CTRL));

	if( find_ctrl == NULL )
		return NULL;

	WaitForSingleObject(q_struct->hSynEvent, INFINITE);
	ZeroMemory(find_ctrl, sizeof(FIND_NODE_CTRL));
	if( q_struct->pHeaderList == NULL )
	{
		free(find_ctrl);
		SetEvent(q_struct->hSynEvent);
		return NULL;
	}
	find_ctrl->pCurNode = q_struct->pHeaderList;
	find_ctrl->pPrevNode = NULL;
	return find_ctrl;
}

extern "C"
VOID
Que_CloseFind(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl
)
{
	PQUE_STRUCT       q_struct = (PQUE_STRUCT)q_handle;

	if( q_find_ctrl == NULL || q_handle == NULL )
		return;

	free(q_find_ctrl);
	SetEvent(q_struct->hSynEvent);
}

extern "C"
BOOL
Que_GetCurDataSizeByFind(
QUEUE_FIND_CTRL  q_find_ctrl,
PULONG   pUserDataSize
)
{
	PFIND_NODE_CTRL   find_node = (PFIND_NODE_CTRL)q_find_ctrl;

	if( q_find_ctrl == NULL || pUserDataSize == NULL )
	{
		return FALSE;
	}

	if(	find_node->pCurNode == NULL )
	{
		return FALSE;
	}

	*pUserDataSize = find_node->pCurNode->nUserDataSize;
	return TRUE;
}

extern "C"
BOOL
Que_GetUserDataByFinding(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl,
PVOID            pUserData
)
{
	
	PQUE_STRUCT       q_struct = (PQUE_STRUCT)q_handle;
	PLIST_STRUCT      pListStruct;
	PFIND_NODE_CTRL   find_ctrl = (PFIND_NODE_CTRL)q_find_ctrl;

	if( q_handle == NULL || q_find_ctrl == NULL || pUserData == NULL )
	{
		return FALSE;
	}
	
	if( find_ctrl->pCurNode == NULL )
	{
		return FALSE;
	}

	pListStruct = (PLIST_STRUCT)find_ctrl->pCurNode;
	memcpy(pUserData, pListStruct->pUserNode, pListStruct->nUserDataSize);
	return TRUE;
}

extern "C"
BOOL
Que_SetNextNodeByFinding(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl
)
{
	BOOL              bRet;
	PQUE_STRUCT       q_struct = (PQUE_STRUCT)q_handle;
	PLIST_STRUCT      pListStruct;
	PFIND_NODE_CTRL   find_ctrl = (PFIND_NODE_CTRL)q_find_ctrl;


	if( q_handle == NULL || q_find_ctrl == NULL )
	{
		return FALSE;
	}

	if( find_ctrl->pCurNode == NULL )
	{
		return FALSE;
	}
	
	pListStruct = (PLIST_STRUCT)find_ctrl->pCurNode;
	find_ctrl->pPrevNode = find_ctrl->pCurNode;
	find_ctrl->pCurNode = pListStruct->pNextList;

	if( find_ctrl->pCurNode == NULL )
	{
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}

	return bRet;
}

extern "C"
VOID
Que_UpdateUserNodeByFinding(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl,
PVOID            pUserData
)
{

	PQUE_STRUCT       q_struct = (PQUE_STRUCT)q_handle;
	PFIND_NODE_CTRL   find_ctrl = (PFIND_NODE_CTRL)q_find_ctrl;

	if( q_find_ctrl == NULL || q_handle == NULL || pUserData == NULL )
		return;
	
	if( find_ctrl->pCurNode )
		CopyMemory(find_ctrl->pCurNode->pUserNode, pUserData, find_ctrl->pCurNode->nUserDataSize);
}

extern "C"
VOID
Que_DeleteNodeByFinding(
QUEUE_HANDLE     q_handle,
QUEUE_FIND_CTRL  q_find_ctrl
)
{
	PQUE_STRUCT       q_struct = (PQUE_STRUCT)q_handle;
	PLIST_STRUCT      pCurList, pPrevList, pDelList;
	PFIND_NODE_CTRL   find_ctrl = (PFIND_NODE_CTRL)q_find_ctrl;
	
	
	if( q_handle == NULL || q_find_ctrl == NULL )
		return;


	if( find_ctrl->pCurNode == NULL )
		return;

	pCurList  = (PLIST_STRUCT)find_ctrl->pCurNode;
	pPrevList = (PLIST_STRUCT)find_ctrl->pPrevNode;

	//处理结点在头部的情况
	if( pPrevList == NULL ) //更新队列头
		q_struct->pHeaderList = pCurList->pNextList;
	else
		pPrevList->pNextList  = pCurList->pNextList; //否则只更新FIND结构头的前置结点所指向的下个数据结点

	pDelList = pCurList;                            //由于当前结点数据将会被删除,因此FIND结构中当前结点中的值自然会移动到下个结点去
	pCurList = pCurList->pNextList;
	find_ctrl->pCurNode = pCurList;

 	q_struct->nCounter --;
	free(pDelList->pUserNode);
	free(pDelList);

	if( pCurList == NULL )   //判断移动后的结点是否已经结束
	{
		q_struct->pLastList  = pPrevList;   //更新队列中末结点变量所指向的结点
		if( pPrevList )                     //如果队列不为空,那么pPrevList即为最后一个结点,因此其pNextList将被置为NULL
			pPrevList->pNextList = NULL;
	}

	if( q_struct->nCounter == 1 )  //重新调整结构指针
		find_ctrl->pPrevNode = NULL;

}

extern "C"
BOOL
Que_GetCurUserDataSize(
QUEUE_HANDLE q_handle,
PULONG       pLen
)
{
	PQUE_STRUCT  q_struct;

	if( q_handle == NULL || pLen == NULL )
	{
		return FALSE;
	}

	q_struct = (PQUE_STRUCT)q_handle;

	WaitForSingleObject(q_struct->hSynEvent, INFINITE);
	*pLen = q_struct->nBlockSize;
	SetEvent(q_struct->hSynEvent);
	return TRUE;
}

extern "C"
VOID
Que_UpdateUserLastNode(
QUEUE_HANDLE     q_handle,
PVOID            pUserData
)
{
	PQUE_STRUCT       q_struct = (PQUE_STRUCT)q_handle;
	
	if( q_handle == NULL || pUserData == NULL )
		return;

	WaitForSingleObject(q_struct->hSynEvent, INFINITE);
	memcpy(q_struct->pLastList->pUserNode, pUserData, q_struct->nBlockSize);
	SetEvent(q_struct->hSynEvent);

}


extern "C"
BOOL
Que_FindByCallBack(
QUEUE_HANDLE que_handle,
FINDCALLBACK find_callback,
PVOID        pUserContext
)
{
	BOOL               bFindOK = FALSE;
	PCHAR              pUserData;
	ULONG              iRet, nUserDataSize;
	QUEUE_FIND_CTRL    hFind;

	if( que_handle == NULL || find_callback == NULL )
		return FALSE;

	if( Que_GetCurUserDataSize(que_handle, &nUserDataSize) == FALSE )
		return FALSE;

	hFind = Que_CreateFindCtrl(que_handle);
	if( hFind == NULL )
		return FALSE;

	pUserData = (PCHAR)malloc(nUserDataSize);
	if( pUserData == NULL )
	{
		Que_CloseFind(que_handle,hFind);
		return FALSE;
	}

	while( TRUE )
	{
		ZeroMemory(pUserData, nUserDataSize);
	    if( Que_GetUserDataByFinding(que_handle, hFind, pUserData) == FALSE )
		   break;

		iRet = find_callback(pUserContext,pUserData);

		if( iRet & QUE_FIND_UPDATE )
			Que_UpdateUserNodeByFinding(que_handle, hFind, pUserData);
		else
		{
			if( iRet & QUE_FIND_DELETE )
				Que_DeleteNodeByFinding(que_handle, hFind);
		}

		if( iRet & QUE_FIND_OK )
			bFindOK = TRUE;

		if( iRet & QUE_FIND_OVER )
			break;

		Que_SetNextNodeByFinding(que_handle, hFind);
	}

	free(pUserData);
	Que_CloseFind(que_handle, hFind);
	return bFindOK;
}




