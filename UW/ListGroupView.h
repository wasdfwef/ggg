#include "..\..\..\inc\QueueManager.h"
#include <string>
#include "..\..\..\inc\THUIInterface.h"

#include "RecThread.h"
#include <map>
#ifndef    _GROUP_VIEW_INCLUDE
#define    _GROUP_VIEW_INCLUDE

#include "ListHelper.h"

#define   IDC_ICON_LIST  0X9300
#define   OTHER_GROUP_ID 0XFF00
#define   INI_SHOW_CELL_NUM 9999
typedef struct _MODIFY_CELL_INFO
{
	ULONG nID;
	int   nCellType;
}MODIFY_CELL_INFO,*PMODIFY_CELL_INFO;

class CListGroupView : public CListHelper,public CView
{
	DECLARE_DYNCREATE(CListGroupView)
	
	BOOL           m_ThumbnailStart;
	SIZE           m_CGSize;
	
	HMENU          m_hMenu;
	CRecThread    *m_RecThread;
	QUEUE_HANDLE   m_BitmapQue;

	wchar_t* ANSIToUnicode( const std::string& str );

	
public:
	CListGroupView();
	~CListGroupView();

	char          m_ImageFolder[MAX_PATH];
	char          m_TaskPath[MAX_PATH];
	HWND          m_ParentWnd;
	LONG          m_Busy;
	SIZE          m_PicSize;

	int           InsertGroup(int nGroupIndex, int nGroupID, PCHAR  pGroupName);
	int           InsertIconItem(int nItemIndex, int nGroupId, int nGrapIndex, ITEM_CELL_INFO* pCellInfo);
	int           AddHBitmapToImageList(HBITMAP hBitmap, ULONG nGrapIndex, bool isMore = false);
	ULONG         GetCurTaskID();
	void          DeleteAllItem();
	void          EmptyGroupName();
	void          InitializeView(PLIST_GROUP_INFO  pListGInfo);
	void          MenuCommand(DWORD nMenuCmdID);
	bool		  CountCellTypeNum(int);
	void          UpdateTaskPath(DWORD  nTaskID);
	void          SetStartThumbnailResult(BOOL bResult);
	void          CreateList();
	int			  ShowCellNumByTotal(int);
	BOOL          ModifyCellType(int ncType, CELL_IDENTIFY *cIdentify = NULL);
	BOOL          GetSelectdInfo(PULONG &pSelArray, int &nSelCounter);
	BOOL          DeleteCell(void);
	void		  ScrollGroup(int nGroupId);
	BOOL          SelectCellItem(PCELL_DETAIL_INFO cell_info, ULONG nGrapIndex, UCHAR nGrapSize);
	BOOL          ShowTaskIcon(ULONG    nTaskID, BOOL bForceFlush);
	BOOL          AddCell(PITEM_CELL_INFO item_cell, HBITMAP hBitmap);
	void		  InsertMOREImageList(int, PVOID);
	static CListGroupView   *CreateView(PLIST_GROUP_INFO  pListGInfo);
	std::map<int, int> CellTypeNum;
	std::vector<int> ExpandCell;
	int			yShowType;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT  OnCustomCreateMsg(WPARAM wparam, LPARAM lparam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMLclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();

public:
	CListCtrl  m_List;
	CImageList m_ImageList;


};

#endif