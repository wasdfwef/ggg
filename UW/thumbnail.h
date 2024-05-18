#ifndef  _THUMBNAIL_INCLUDE
#define  _THUMBNAIL_INCLUDE

#include "..\..\..\inc\THUIStruct.h"
//typedef  struct  _ITEM_CELL_INFO_
//{
//
//	unsigned short   nGrapIndex; // 图片序号
//	unsigned char    nGrapSize;  // 图片SIZE
//	unsigned short   cType;      // 类型
//	CELL_IDENTIFY    identify;
//
//	char     sRecRelativePath[MAX_PATH];
//	RECT     rc;      
//
//}ITEM_CELL_INFO;

extern "C"
BOOL
WINAPI
Thumbnail_ShowPic(
unsigned int nID,
ITEM_CELL_INFO* pSelectPicInfo,
PSIZE PicSize
);

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
);

#endif