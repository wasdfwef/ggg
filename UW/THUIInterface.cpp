#include "stdafx.h"
#include "..\..\..\inc\THUIInterface.h"
#include "ListGroupView.h"
#include "thumbnail.h"
#include <Shlwapi.h>

#pragma comment(lib, "..\\..\\..\\lib\\THInterface.lib")


static  CListGroupView   *g_list_view = NULL;

static
BOOL
THUIInterface_VerifyListGroupPar(
PLIST_GROUP_INFO    pListGInfo
)
{
	if( IsBadReadPtr(pListGInfo, sizeof(LIST_GROUP_INFO)) == TRUE )
		return FALSE;

	if( pListGInfo->nCreateType < TYPE_NORMAL_LIST_VIEW || pListGInfo->nCreateType > TYPE_MFC_TAB_VIEW )
		return FALSE;

	if( pListGInfo->nCreateType == TYPE_NORMAL_LIST_VIEW )
	{
		if( pListGInfo->hParentWnd == NULL || IsWindow(pListGInfo->hParentWnd) == FALSE )
			return FALSE;

		if( IsRectEmpty(&pListGInfo->ListGroupRC) == TRUE )
			return FALSE;
	}
	else if( pListGInfo->nCreateType == TYPE_MFC_TAB_VIEW )
	{
		if( pListGInfo->pOtherCreateContext == NULL )
			return FALSE;
	}


	if( pListGInfo->gCounter == 0 )
		return FALSE;

	if( pListGInfo->CellGrapSZ.cx == 0 || pListGInfo->CellGrapSZ.cy == 0 )
		return FALSE;

	for( ULONG j = 0; j < pListGInfo->gCounter ; j ++ )
	{
		if( pListGInfo->gn[j].GroupName[0] == 0 )
			return FALSE;
	}

	

	return TRUE;

}

static
BOOL
THUIInterface_VerifDispDesPar(
PMENU_DISP_DESCRIPTION pMenuDispDes,
USHORT                 nMenuDispDesCounter
)
{
	if( IsBadReadPtr(pMenuDispDes, sizeof(MENU_DISP_DESCRIPTION) * nMenuDispDesCounter) == TRUE )
		return FALSE;

	for( USHORT  j = 0 ; j < nMenuDispDesCounter ; j ++ )
	{
		if( pMenuDispDes[j].hMenu == NULL || pMenuDispDes[j].nMenuMask == 0 )
			return FALSE;

		if( !(pMenuDispDes[j].nMenuMask & (MOUSE_R_CLICK_MASK|MOUSE_L_CLICK_MASK|MOUSE_RD_CLICK_MASK|MOUSE_LD_CLICK_MASK|GRAP_SELECTED_STATUS)) )
			return FALSE;
	}
	
	return TRUE;
}

static
BOOL
THUIInterface_VerifyCmdDesPar(
PMENU_COMMAND_DESCRIPTION   pMenuCmdDes,
USHORT                      nMenuCmdDesCounter
)
{
	if( IsBadReadPtr(pMenuCmdDes,sizeof(MENU_COMMAND_DESCRIPTION) * nMenuCmdDesCounter) == TRUE )
		return FALSE;

	for( USHORT j = 0 ; j < nMenuCmdDesCounter ; j ++ )
	{
		if( pMenuCmdDes[j].nMCommandID == 0 )
			return FALSE;

		if( !(pMenuCmdDes[j].nMAction & (CHANGED_CELL_TYPE_ACTION|DELETE_CELL_TYPE_ACTION)) )
			return FALSE;

	}

	return TRUE;
}

static
CListGroupView *
THUIInterface_CreateListGroupView(
PLIST_GROUP_INFO            pListGInfo
)
{
	CListGroupView  *pView = CListGroupView::CreateView(pListGInfo);

	if( pView == NULL )
		return pView;

	pView->InitializeView(pListGInfo);

	for( ULONG j = 0 ; j < pListGInfo->gCounter ; j ++ )
		pView->InsertGroup(j, pListGInfo->gn[j].nCellType, pListGInfo->gn[j].GroupName);

	pView->CloneCellType(pListGInfo->gn, pListGInfo->gCounter);


	return pView;
}


static
BOOL
THUIInterface_CheckParVerify(
HWND                        hRcvWnd,
PCHAR                       pImageFolder,
PLIST_GROUP_INFO            pListGInfo,
PMENU_DISP_DESCRIPTION      pMenuDispDes,
USHORT                      nMenuDispDesCounter,
PMENU_COMMAND_DESCRIPTION   pMenuCmdDes,
USHORT                      nMenuCmdDesCounter
)
{
	if( hRcvWnd && IsWindow(hRcvWnd) == FALSE )
		return FALSE;

	if( pImageFolder == NULL || PathFileExistsA(pImageFolder) == FALSE || pListGInfo == NULL || pMenuCmdDes == NULL || pMenuCmdDes == NULL || nMenuDispDesCounter == 0 || nMenuCmdDesCounter == 0 )
		return FALSE;


	if( THUIInterface_VerifyListGroupPar(pListGInfo) == FALSE )
		return FALSE;

	if( THUIInterface_VerifDispDesPar(pMenuDispDes, nMenuDispDesCounter) == FALSE )
		return FALSE;

	if( THUIInterface_VerifyCmdDesPar(pMenuCmdDes, nMenuCmdDesCounter) == FALSE )
		return FALSE;

	return TRUE;

}

extern "C"
BOOL
WINAPI
THUIInterface_InitializeListGroup(
HWND                        hRcvWnd,
PCHAR                       pImageFolder,
PLIST_GROUP_INFO            pListGInfo,
PMENU_DISP_DESCRIPTION      pMenuDispDes,
USHORT                      nMenuDispDesCounter,
PMENU_COMMAND_DESCRIPTION   pMenuCmdDes,
USHORT                      nMenuCmdDesCounter
)
{
	CListGroupView  *pView = NULL;

	if( g_list_view  )
		return FALSE;

	if( THUIInterface_CheckParVerify(hRcvWnd, pImageFolder, pListGInfo, pMenuDispDes, nMenuDispDesCounter,pMenuCmdDes, nMenuCmdDesCounter ) == FALSE )
		return FALSE;


	pView = THUIInterface_CreateListGroupView(pListGInfo);
	if( pView != NULL )
	{
		strcpy(pView->m_ImageFolder, pImageFolder);
		pView->HookListGroupWindow(pView->GetSafeHwnd(),pMenuDispDes, nMenuDispDesCounter, pMenuCmdDes, nMenuCmdDesCounter);

	}

	BOOL  bStart = Thumbnail_InitializeListGroup(pView->GetSafeHwnd(), pImageFolder, pListGInfo, pMenuDispDes, nMenuDispDesCounter, pMenuCmdDes, nMenuCmdDesCounter);

	pView->SetStartThumbnailResult(bStart);
	pView->m_ParentWnd = hRcvWnd;
	g_list_view = pView;
	return TRUE;

}


extern "C"
BOOL
WINAPI 
THUIInterface_InitializeListGroupByTabCtrl( 
PLIST_GROUP_INFO pListGInfo 
)
{
	CListGroupView  *pView = NULL;

	if( g_list_view )
		return FALSE;

	pView = CListGroupView::CreateView(pListGInfo);
	if( pView == NULL )
		return FALSE;

	g_list_view = pView;
	return TRUE;
}


extern "C"
BOOL
WINAPI
THUIInterface_FillParByTabCtrl(
HWND                        hRcvWnd,
PCHAR                       pImageFolder,
PLIST_GROUP_INFO            pListGInfo,
PMENU_DISP_DESCRIPTION      pMenuDispDes,
USHORT                      nMenuDispDesCounter,
PMENU_COMMAND_DESCRIPTION   pMenuCmdDes,
USHORT                      nMenuCmdDesCounter
)
{

	if( g_list_view == NULL )
		return FALSE;

	if( THUIInterface_CheckParVerify(hRcvWnd, pImageFolder, pListGInfo, pMenuDispDes, nMenuDispDesCounter,pMenuCmdDes, nMenuCmdDesCounter ) == FALSE )
		return FALSE;

	g_list_view->InitializeView(pListGInfo);

	for( ULONG j = 0 ; j < pListGInfo->gCounter ; j ++ )
		g_list_view->InsertGroup(j, pListGInfo->gn[j].nCellType, pListGInfo->gn[j].GroupName);

	g_list_view->CloneCellType(pListGInfo->gn, pListGInfo->gCounter);


	strcpy(g_list_view->m_ImageFolder, pImageFolder);
	g_list_view->HookListGroupWindow(g_list_view->GetSafeHwnd(),pMenuDispDes, nMenuDispDesCounter, pMenuCmdDes, nMenuCmdDesCounter);

	BOOL  bStart = Thumbnail_InitializeListGroup(g_list_view->GetSafeHwnd(), pImageFolder, pListGInfo, pMenuDispDes, nMenuDispDesCounter, pMenuCmdDes, nMenuCmdDesCounter);

	g_list_view->SetStartThumbnailResult(bStart);
	g_list_view->m_ParentWnd = hRcvWnd;


	return TRUE;

}


extern "C"
BOOL
WINAPI 
THUIInterface_ShowTaskIcon( 
ULONG nTaskID,
BOOL  bForceFlush
)
{
	if( g_list_view == NULL )
		return FALSE;

	try
	{
		g_list_view->ExpandCell.clear();
		return g_list_view->ShowTaskIcon(nTaskID,bForceFlush);
	}
	catch (...)
	{
		return FALSE;
	}

}

extern "C"
ULONG
WINAPI 
THUIInterface_GetTaskID(
)
{
	if( g_list_view == NULL )
		return FALSE;

	try
	{
		return g_list_view->GetCurTaskID();
	}
	catch (...)
	{
		return 0xffffffff;
	}
}

extern "C"
ULONG
WINAPI THUIInterface_SetShowGroup(ULONG nTaskID, ULONG GroupID)
{
	g_list_view->ScrollGroup(GroupID);
	return 0;
}


