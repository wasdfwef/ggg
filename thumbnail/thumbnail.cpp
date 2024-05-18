// thumbnail.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "thumbnail.h"
#include "Thumbnail1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CthumbnailApp

BEGIN_MESSAGE_MAP(CthumbnailApp, CWinApp)
END_MESSAGE_MAP()


// CthumbnailApp 构造

CthumbnailApp::CthumbnailApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CthumbnailApp 对象

CthumbnailApp theApp;

static  PMENU_DISP_DESCRIPTION     g_MenuDisp = NULL;
static  PMENU_COMMAND_DESCRIPTION  g_MenuCmdDes = NULL;
static  USHORT                     g_nMenuDispDesCounter = 0, g_MenuCmdDesCounter = 0;
static  CHAR                       g_ImageFolder[MAX_PATH];
static  HWND                       g_RcvWnd = NULL;
static  LIST_GROUP_INFO            g_list_info = {0};

// CthumbnailApp 初始化

BOOL CthumbnailApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

int CthumbnailApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	if(g_MenuDisp != NULL)
		delete[] g_MenuDisp;
	g_MenuDisp = NULL;

	if(g_MenuCmdDes != NULL)
		delete[] g_MenuCmdDes;
	g_MenuCmdDes = NULL;
	
	return CWinApp::ExitInstance();
}








extern "C"
BOOL
WINAPI
Thumbnail_InitializeListGroup(
HWND                        hRcvWnd,
PCHAR                       pImageFolder,
PLIST_GROUP_INFO            pListGInfo,
PMENU_DISP_DESCRIPTION      pMenuDispDes,
USHORT                      nMenuDispDesCounter,
PMENU_COMMAND_DESCRIPTION   pMenuCmdDes,
USHORT                      nMenuCmdDesCounter							  
							  )
{
	
		
	memcpy(&g_list_info,pListGInfo,sizeof(LIST_GROUP_INFO));
	strcpy(g_ImageFolder, pImageFolder);
	

	g_MenuDisp  = new MENU_DISP_DESCRIPTION[nMenuDispDesCounter];

	if( g_MenuDisp == NULL )
		return FALSE;

	g_MenuCmdDes  = new MENU_COMMAND_DESCRIPTION[nMenuCmdDesCounter];
	if( g_MenuCmdDes == NULL )
	{
		delete [] g_MenuCmdDes;
		g_MenuCmdDes = NULL;
		return FALSE;
	}

	memcpy(g_MenuDisp, pMenuDispDes, sizeof(MENU_DISP_DESCRIPTION) * nMenuDispDesCounter);
	memcpy(g_MenuCmdDes, pMenuCmdDes, sizeof(MENU_COMMAND_DESCRIPTION) * nMenuCmdDesCounter);
	g_nMenuDispDesCounter  = nMenuDispDesCounter;
	g_MenuCmdDesCounter    = nMenuCmdDesCounter;
	g_RcvWnd = hRcvWnd;	

	return TRUE;

}

extern "C"
BOOL
WINAPI
Thumbnail_ShowPic(
unsigned int nID,
ITEM_CELL_INFO* pSelectPicInfo,
PSIZE PicSize
)
{
	DBG_MSG("Thumbnail_ShowPic nID=%d,PicSize->cx=%d,PicSize->cy=%d,\n",nID,PicSize->cx,PicSize->cy);

	if( g_RcvWnd == NULL )
		return FALSE;

	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CThumbnail1 View;

	HINSTANCE   hInstOld; 		

	memcpy(&View.m_ListGInfo, &g_list_info,sizeof(LIST_GROUP_INFO));
	strcpy(View.m_ImageFolder, g_ImageFolder);

	View.HookListGroupWindow(NULL,g_MenuDisp, g_nMenuDispDesCounter, g_MenuCmdDes, g_MenuCmdDesCounter);
	View.m_ParentWnd = g_RcvWnd;

	if( !View.SetImageSize(nID,PicSize))
		return FALSE;

	if(pSelectPicInfo)
	{
		if(!View.SetSelectInfo(pSelectPicInfo)) 
			return FALSE;
	}
		

	hInstOld = AfxGetResourceHandle();

	AfxSetResourceHandle(theApp.m_hInstance); 
	// AfxSetResourceHandle(AfxGetInstanceHandle()); 
	View.DoModal() ;		

	AfxSetResourceHandle(hInstOld); 
	return TRUE;
	
}

