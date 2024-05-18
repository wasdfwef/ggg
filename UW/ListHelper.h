#ifndef  _LISTHELPER_INCLUDE
#define  _LISTHELPER_INCLUDE

#include "..\..\..\inc\THUIInterface.h"

class   CListHelper
{
	PUSHORT                        m_CellTypeArray;
	ULONG                          m_CellTypeCounter;
	WNDPROC                        m_OldWndProc;
	
	PMENU_COMMAND_DESCRIPTION      m_MenuCmd_Des;
	USHORT                         m_MenuDisp_Counter, m_MenuCmdDesCounter;

public:

	CListHelper();
	~CListHelper();

	void     HookListGroupWindow(HWND  HookWnd,PMENU_DISP_DESCRIPTION  pMenuDispDes,USHORT   nMenuDispDesCounter,PMENU_COMMAND_DESCRIPTION   pMenuCmdDes,	USHORT   nMenuCmdDesCounter);
	void     CloneCellType(PGROUP_NAME  pGroupName, ULONG nCounter);
	BOOL     SearchMenuCmd(DWORD  nMenuCmdID, MENU_COMMAND_DESCRIPTION & MenuCmdDes);
	BOOL     VaildCellType(USHORT  nCellType);
	WNDPROC  GetOldListGroupWndProc();
	PMENU_DISP_DESCRIPTION         m_MenuDisp_Des;

};




#endif