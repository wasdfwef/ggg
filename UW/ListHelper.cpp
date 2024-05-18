#include "stdafx.h"
#include "ListHelper.h"
#include "ListGroupView.h"
#include "inter_msg.h"

LRESULT 
CALLBACK 
ListGroupWndProc(
HWND hwnd, 
UINT uMsg, 
WPARAM wParam, 
LPARAM lParam 
)
{
	CListGroupView  *pListGroupView = (CListGroupView*)CListGroupView::FromHandle(hwnd);

	if( uMsg == WM_COMMAND )
		pListGroupView->MenuCommand(LOWORD(wParam));
	else if( uMsg == WM_ADD_CELL )
		return pListGroupView->AddCell((PITEM_CELL_INFO)wParam, (HBITMAP)lParam);
	else if( uMsg == WM_SELECT_CELL )
	{
		PCELL_DETAIL_INFO   cell_detail_info = (PCELL_DETAIL_INFO)wParam;
		PREC_GRAP_RECORD    rc               = (PREC_GRAP_RECORD)lParam;

		if( cell_detail_info && IsBadReadPtr(cell_detail_info, sizeof(CELL_DETAIL_INFO)) == TRUE  && 
			rc               && IsBadReadPtr(rc, sizeof(REC_GRAP_RECORD)) == TRUE )
			return 0;

		pListGroupView->SelectCellItem(cell_detail_info, rc->rIndex, rc->rGrapSize);
		return 0;
	}
	else if( uMsg == WM_SET_PRINT_PATH )
	{
		PCHAR  pFileName = (PCHAR)wParam;

		if( pFileName == NULL || IsBadReadPtr(pFileName, 4) == TRUE )
			return 0;

		return SendMessage(pListGroupView->m_ParentWnd, SET_PRINT_PATH_MSG, wParam, lParam);
	}
	else if (uMsg == WM_GET_PRINT_PATH)
	{
		PCHAR pPrintImages = (PCHAR)wParam;
		ULONG id = (ULONG)lParam;
		return SendMessage(pListGroupView->m_ParentWnd, GET_PRINT_PATH_MSG, wParam, lParam);
	}

	return CallWindowProc(pListGroupView->GetOldListGroupWndProc(), hwnd, uMsg, wParam, lParam);
}


WNDPROC CListHelper::GetOldListGroupWndProc()
{
	return  m_OldWndProc;
}

void CListHelper::HookListGroupWindow(HWND  HookWnd,PMENU_DISP_DESCRIPTION pMenuDispDes,USHORT nMenuDispDesCounter,PMENU_COMMAND_DESCRIPTION pMenuCmdDes, USHORT nMenuCmdDesCounter )
{
	m_MenuDisp_Des  = NULL;
	m_MenuCmd_Des   = NULL;
	
	m_MenuDisp_Des  = new MENU_DISP_DESCRIPTION[nMenuDispDesCounter];
	
	if( m_MenuDisp_Des == NULL )
		return;

	m_MenuCmd_Des  = new MENU_COMMAND_DESCRIPTION[nMenuCmdDesCounter];
	if( m_MenuCmd_Des == NULL )
	{
		delete [] m_MenuCmd_Des;
		m_MenuCmd_Des = NULL;
	}

	memcpy(m_MenuDisp_Des, pMenuDispDes, sizeof(MENU_DISP_DESCRIPTION) * nMenuDispDesCounter);
	memcpy(m_MenuCmd_Des, pMenuCmdDes, sizeof(MENU_COMMAND_DESCRIPTION) * nMenuCmdDesCounter);
	m_MenuDisp_Counter  = nMenuDispDesCounter;
	m_MenuCmdDesCounter = nMenuCmdDesCounter;

	m_OldWndProc = (WNDPROC)SetWindowLong(HookWnd, GWL_WNDPROC, (LONG)ListGroupWndProc);

}

BOOL CListHelper::SearchMenuCmd( DWORD nMenuCmdID, MENU_COMMAND_DESCRIPTION & MenuCmdDes )
{
	for( ULONG j = 0 ; j < m_MenuCmdDesCounter ; j ++ )
	{
		if( (m_MenuCmd_Des[j].nMCommandID & 0xffff) == nMenuCmdID )
		{
			memcpy(&MenuCmdDes, &m_MenuCmd_Des[j], sizeof(MENU_COMMAND_DESCRIPTION));
			return TRUE;
		}
	}

	return FALSE;

}

void CListHelper::CloneCellType( PGROUP_NAME pGroupName, ULONG nCounter )
{
	m_CellTypeArray = (PUSHORT)malloc(sizeof(USHORT) * nCounter);
	if( m_CellTypeArray == NULL )
		return;

	for( m_CellTypeCounter = 0 ; m_CellTypeCounter < nCounter; m_CellTypeCounter ++ )
		m_CellTypeArray[m_CellTypeCounter] = (USHORT)pGroupName[m_CellTypeCounter].nCellType;

}

CListHelper::CListHelper()
{
	m_CellTypeArray   = NULL;
	m_CellTypeCounter = 0;

}

CListHelper::~CListHelper()
{
	if( m_CellTypeCounter && m_CellTypeArray )
		free(m_CellTypeArray);

}

BOOL CListHelper::VaildCellType( USHORT nCellType )
{
	for( ULONG j  = 0 ; j < m_CellTypeCounter ; j ++ )
	{
		if( m_CellTypeArray[j] == nCellType )
			return TRUE;
	}

	return FALSE;

}
