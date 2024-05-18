#pragma once

#include	"ImageArea.h"
#include	<vector>
#include    "RecTask.h"
#include    "MyTracker.h"
#include    "DbgMsg.h"
#include    "..\..\..\inc\thuistruct.h"

#include "CConfigSet.h"
#pragma comment(lib,"..\\..\\..\\lib\\CConfigSet.lib")

#define WM_ADD_CELL        WM_USER+101
#define WM_SET_PRINT_PATH  WM_USER+102
#define WM_SELECT_CELL     WM_USER+103
#define WM_GET_PRINT_PATH  WM_USER+104



#define RES_STRING_NAME   "string"

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


 //CThumbnail1 对话框

class CThumbnail1 :  public CDialog
{
	DECLARE_DYNAMIC(CThumbnail1)

public:
	CThumbnail1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CThumbnail1();

// 对话框数据
	enum { IDD = IDD_DLG_THUMBNAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	void     ClearSelectItemStatus(int nSel);
	void     RestoreNormalMode();
	void     CellDothing(int cCellType, BOOL bDrawImage = TRUE);
	BOOL     DeleteOthCell();
	int      GetCurSelCell(POINT &pt);
	BOOL     SetSelectInfo(ITEM_CELL_INFO* pSelectPicInfo);
	void     DrawSelectedImage();	// draw the selected thumbnail image in full size
	void     DrawCellText();
	void     DrawSelectRC();
	void     GetTextShowLoc(HDC hDC, RECT &rc, PCHAR pStr, SIZE &off_loc);
	void     ModifyCell(USHORT CellType,BOOL bDrawImage = TRUE);
	void     CreateNewMyTracker();
	void     AddCell(USHORT CellType);
	BOOL     HitTest(POINT pt);
	KSTATUS  LoadGrapAndCellInfo();
	BOOL     SetImageSize(unsigned int nID,PSIZE PicSize);
	BOOL     GetImageFileNames();	// gather the image file names
	void     DrawThumbnails();
	BOOL     CellDisableDisaply(ULONG  cType);
	int		 CellMapUsIndex(USHORT nCellType);	
	
	void	 ResizeWindow(PSIZE PicSize);
	void	 RMenuStatus(CMenu  *SubMenu);
	

public:
	
	HICON					m_hIcon;
	ULONG                   m_GrapIndex;
	int                     m_CurSelCellIndex;
	BOOL			        m_AddMode;
	RECT			        m_DrawRect;
	CPen			        pen;

	CMyTracker		       *m_RectTracker;
	ULONG_PTR		        m_gdiplusToken;
	
	CELL_IDENTIFY           m_CellIdentify;
	ASSOCIATE_DC            m_AssoicateDC;
	PCELL_FULL_INFO_EX      m_cell_full_info;
	CRecTask                m_rec_task;	
	PTASK_REC_INFO          m_ptask_rec_info;
	LIST_GROUP_INFO         m_ListGInfo;
	GET_US_INFO             get_us_info;

	char                    m_TaskPath[MAX_PATH];
	char                    m_ImageFolder[MAX_PATH];	

	HWND                    m_ParentWnd;	
	unsigned int            m_nID;
	/*HWND					m_hwnd;*/
	SIZE                    m_picsize;	
	CImageArea              m_ImageRect;
	CImageList				m_ImageListThumb;		// image list holding the thumbnails
	std::vector<CString>	m_VectorImageNames;		// vector holding the image names
	int						m_nSelectedItem;
	CListCtrl               m_ListThumbnail;
	std::vector<CString>	m_VectorPrintImages;   // vector holding the print image names

	int                     m_nIsShowOther;
    float                   m_fx,m_fy;

	DECLARE_MESSAGE_MAP()
	
	
public:
	afx_msg void OnBtnLoad();
	afx_msg void OnCellNew();

	afx_msg void OnPopRbc();
	afx_msg void OnPopWbc();
	afx_msg void OnPopEpithelialcell();
	afx_msg void OnPopCasts();
	afx_msg void OnPopC();
	afx_msg void OnPopMucus();
	afx_msg void OnPopBactreia();
	afx_msg void OnPopByst();
	afx_msg void OnPopDelete();
	afx_msg void OnACCell(UINT nID);
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);	
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLvnItemchangedListThumb(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownListThumb(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListThumb(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListThumb(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnStnClickedImageRect();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	CString m_strImageDir;
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void     HookListGroupWindow(HWND  HookWnd,PMENU_DISP_DESCRIPTION  pMenuDispDes,USHORT   nMenuDispDesCounter,PMENU_COMMAND_DESCRIPTION   pMenuCmdDes,	USHORT   nMenuCmdDesCounter);
	BOOL     SearchMenuCmd(DWORD  nMenuCmdID, MENU_COMMAND_DESCRIPTION & MenuCmdDes);
	WNDPROC  GetOldListGroupWndProc();
	void     CloneCellType(PGROUP_NAME  pGroupName, ULONG nCounter);
	BOOL     VaildCellType(USHORT  nCellType);

	void     GetWhetherShowOther();

	PMENU_DISP_DESCRIPTION         m_MenuDisp_Des;
	PMENU_COMMAND_DESCRIPTION      m_MenuCmd_Des;
	USHORT                         m_MenuDisp_Counter, m_MenuCmdDesCounter;
	WNDPROC                        m_OldWndProc;
	PULONG                         m_CellTypeArray;
	ULONG                          m_CellTypeCounter;

	void          UpdateTaskPath(DWORD  nTaskID);

	void          MenuCommand(DWORD nMenuCmdID);
	afx_msg void OnAddPrintpic();
	afx_msg void OnDelPrintpic();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
