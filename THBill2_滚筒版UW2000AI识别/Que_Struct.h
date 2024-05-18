#pragma pack(1)

typedef struct _LIST_STRUCT
{
	struct _LIST_STRUCT *pNextList;
	ULONG  nUserDataSize;
	PVOID  pUserNode;

}LIST_STRUCT, *PLIST_STRUCT;

typedef struct _QUE_STRUCT
{
	ULONG           nCounter;
	ULONG           nBlockSize;
	HANDLE          hSynEvent;
	PLIST_STRUCT    pHeaderList;
	PLIST_STRUCT    pLastList;

}QUE_STRUCT, *PQUE_STRUCT;

typedef struct _FIND_NODE_CTRL
{
	PLIST_STRUCT   pCurNode;
	PLIST_STRUCT   pPrevNode;
	
}FIND_NODE_CTRL, *PFIND_NODE_CTRL;


#pragma pack()