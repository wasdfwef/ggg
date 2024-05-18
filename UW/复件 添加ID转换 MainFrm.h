
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "TodayList.h"
#include "RecordList.h"
#include "PropertiesWndPat.h"
#include "PropertiesWndRet.h"
#include "FindRecord.h"
#include "OutputWnd.h"
#include "UWView.h"
#include "UWDoc.h"
#include "SelfCheckdlg.h"

#include <vector>


class CMainFrame : public CFrameWndEx
{

	

	
public: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
protected:
	CSplitterWnd m_wndSplitter;

private:

	BOOL  NormalTestType(int nPos,int nCha,int nTestType, BOOL bTaskResult);
	BOOL  ResetTestType(int nResetTestType, BOOL bTaskResult, LONG nID, int nItemIndex);


public:

// 操作
public:

	HMENU menu_cell;
	CMenu top;
	CMenu menu_pop;
	CMenu menu_record;
    CMenu menu_print;
	CMenu menu_usmic;
	std::vector <ULONG>  m_MapID;

	CTodayList m_wndTodayList;
	CRecordList m_wndRecordList;
	CPropertiesWndPat m_wndPatient;   // 患者资料
	CPropertiesWndRet m_wndResult;    // 检查结果
	COutputWnd        m_wndOutPut;    // 信息输出
	CFindRecord       m_wndFindRecord;// 查询记录

	HANDLE  m_hForceCleanThread;
	HANDLE  hExitEvent ;      
	HANDLE  hReceiveEvent;
	DWORD   m_dwThreadId;
	LONG    m_ExitThread;
    CSelfCheckdlg selfdlg;

	HANDLE   m_hThread;       // 辅助线程
	DWORD    m_dwThreadId1;    // 线程ID
	BOOL     m_bClose;

	BOOL    m_bPicPause;                  //暂停采图
	void  InitShowCellView();

	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* pWnd);
	virtual BOOL OnCloseDockingPane(CDockablePane* pWnd);


// 重写
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	


// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	//CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	//afx_msg void OnDeviceDbg(UINT nID);
	//afx_msg void OnUpdateDeviceDbg(CCmdUI *pCmdUI);
	afx_msg LRESULT OnFindRecordLookInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangedCell(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnInsertUDCData(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnSetPrintPathMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnPressCloseBtn(WPARAM wp,LPARAM lp);
	afx_msg void OnMenuMange(CCmdUI *pCmdUI);
	afx_msg void OnMenuDbg(UINT nID);
	afx_msg void OnDevMange(CCmdUI *pCmdUI);
    afx_msg void OnDevDbg(UINT nID);
	afx_msg void OnCaiTuConfig(UINT nID);
	afx_msg void OnOperateMange(CCmdUI *pCmdUI);
	afx_msg void OnOperateDbg(UINT nID);
	afx_msg LRESULT OnAutoFocus(WPARAM wParam, LPARAM lParam);
	
	static DWORD WINAPI CloseThread(LPVOID pParam);
	void SetClose();
	

	DECLARE_MESSAGE_MAP()

public:
	void menuLoad();
	// 获得view指针
	CUWView* GetUWView();
	BOOL CreateDockingWindows(void);

	BOOL      IsExitThread();
	void      CloseThreadAndEvent(void);
	HANDLE    GetExitEvent();
	BOOL      NewTask(int nPos,int nCha,int nTestType, BOOL bTaskResult);
	void      UpdataTaskState(int nID,StatusEnum nState);
	void      USResult(BOOL bSuccess);
	void      UDCResult(BOOL bSuccess);
	void      OtherInfo(TCHAR *Msg);
	void      ManageWarnning(CString Warning,CTroubleType nType);
	void      SetTabView(PVOID pTabView);
	void      ActiveTab(int nIndex);
	void      UpdateUDCView(PUDC_IMPORT_INFO_EX   udc_import);
	bool      OpenAutoFocus();
	ULONG     GetAutoFocusInterval();
	ULONG     nAutoFocusInterval,nBeginTime,nNowTime;

	ULONG     GetTaskIDFromMap(ULONG nMapIndex);


	
	/*COleDateTime     BeginTime;
	COleDateTime     NowTime;*/
    //afx_msg void OnMenuSet();
	//afx_msg void OnUpdateMenuSet(CCmdUI *pCmdUI);

	//afx_msg void OnMenuSet();
	//afx_msg void OnMenuMem();
	afx_msg void OnClose();
	afx_msg void OnOperateNew();
	afx_msg void OnAutoFocus();
	afx_msg void OnOperateDelete();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void DoForceClean();
};


