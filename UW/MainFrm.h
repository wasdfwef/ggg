
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "TodayList.h"
#include "RecordList.h"
#include "PropertiesWndPat.h"
#include "PropertiesWndRet.h"
#include "FindRecord.h"
#include "OutputWnd.h"
#include "MyMenu.h"
#include "UWView.h"
#include "UWDoc.h"
#include "SelfCheckdlg.h"
#include "WndBtnTools.h"
#include "SearchRecordDlg.h"
#include "SetDialog.h"
#include "Setting.h"
#include "pipeline_manager.h"
#include <queue>
#include "QcDlg.h"

using namespace std;

class CMainFrame : public CFrameWndEx, public IConnectListener
{

	
public: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
protected:
	CSplitterWnd m_wndSplitter;

private:

	BOOL  NormalTestType(int nPos,int nCha,int nTestType, BOOL bTaskResult, int &TaskId); // 正常检测
	BOOL  ResetTestType(int nResetTestType, BOOL bTaskResult, LONG nID, int nItemIndex); // 复测


	int CalculateCleanCount(int taskNumNeedClean, int* todayRest);
public:

// 操作
public:

	HMENU menu_cell;
	CMenu top;
	CMenu menu_pop;
	CMenu menu_record;
    CMenu menu_print;
	CMenu menu_usmic;
	bool isLCReset;
	CQcDlg* qcDlg;
	CTodayList m_wndTodayList;        // 当天记录
	//CRecordList m_wndRecordList;      // @unused
	CPropertiesWndPat m_wndPatient;   // 患者资料
	CPropertiesWndRet m_wndResult;    // 检查结果
	COutputWnd        m_wndOutPut;    // 信息输出
/*	CFindRecord       m_wndFindRecord;*/// 查询记录
	WndBtnTools		  m_wndBtnTools;	//窗口工具栏
	WndBtnTools		  m_wndBtnRight;	//按钮栏
	SearchRecordDlg   m_wndSearchRecord;// 查询记录
	CSetDialog setting;//管理窗口
	CSetting  setdlg;//设置窗口
	SYSTEMTIME lastNormalTestTime;//系统时间
	HANDLE  m_hForceCleanThread; // 强化清洗线程
	HANDLE  hExitEvent ;		 // 线程退出事件
	HANDLE  hReceiveEvent;       // 系统消息线程（下位机消息）
	LONG    m_ExitThread;
    CSelfCheckdlg selfdlg;    // 自检窗口
	std::queue<int> m_SendLisQueue;
	HANDLE   m_hThread;       // 辅助线程
	BOOL     m_bClose;
	bool	m_HaveOntimeClean;
	BOOL    m_bPicPause;      // 暂停采图
	CRITICAL_SECTION    m_sendLisCS;
	void    InitShowCellView();
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
/*	CMFCToolBar       m_wndToolBar;*/
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

// 生成的消息映射函数
public:
	afx_msg void OnCaiTuUIMange(CCmdUI *pCmdUI);
	afx_msg void OnCaiTuConfig(UINT nID);
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
	afx_msg LRESULT OnGetPrintPathMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnUpdateListPos(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnPressCloseBtn(WPARAM wp,LPARAM lp);
	afx_msg void OnMenuMange(CCmdUI *pCmdUI);
	afx_msg void OnMenuDbg(UINT nID);
	afx_msg void OnDevMange(CCmdUI *pCmdUI);
    afx_msg void OnDevDbg(UINT nID);
	afx_msg void OnOperateMange(CCmdUI *pCmdUI);
	afx_msg void OnOperateDbg(UINT nID);
	afx_msg LRESULT OnAutoFocus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ShowSelfDlgHandler(WPARAM w, LPARAM l);
	
	static DWORD WINAPI CloseThread(LPVOID pParam);
	static DWORD WINAPI SendLisInQueue(LPVOID pParam);
	afx_msg LRESULT SetClose(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()

public:
	void menuLoad();
	// 获得view指针
	CUWView* GetUWView();
	BOOL CreateDockingWindows(void);
	void GetUIPosBeforeShutdown();
	void SetUIPosBeforeShutdown();
	void SendMessageToQcDlg();
	BOOL      IsExitThread();
	void      CloseThreadAndEvent(void);
	HANDLE    GetExitEvent();
	HANDLE	lisHandle;
	BOOL      NewTask(int nPos,int nCha,int nTestType, BOOL bTaskResult, int &newTaskId);
	void      UpdataTaskState(int nID,StatusEnum nState);
	void      USResult(BOOL bSuccess); //@unused
	void      UDCResult(BOOL bSuccess);//@unused
	void      OutputMessage(TCHAR *Msg);
	void      ManageWarnning(CString Warning,ALARM_TYPE nType);
	void      SetTabView(PVOID pTabView);
	void      ActiveTab(int nIndex);
	void      UpdateUDCView(PUDC_RESULT_CONTEXT udc_ctx);
	bool      OpenAutoFocus();
	ULONG     GetAutoFocusInterval();
	ULONG     nAutoFocusInterval,nBeginTime,nNowTime;
	void      SetConsumables(int count);
	void	  HandleTaskUnfinished();
	bool	  m_bCheckCunsumablesSuccess;
	HANDLE    m_hCheckCardEvent;
	HANDLE    m_hExitCheckCardEvent;
	HANDLE    m_OnTimeSendLisEvent;
	void      MicSet(bool bIsInSetting);
	int       GetTestType(int nHole, string barcode, std::map<string, string>& baseInfo);
	int       GetTestTypeByLiusxianFromLis(string barcode, std::map<string, string>& baseInfo);
	
	/*COleDateTime     BeginTime;
	COleDateTime     NowTime;*/
    //afx_msg void OnMenuSet();
	//afx_msg void OnUpdateMenuSet(CCmdUI *pCmdUI);

	//afx_msg void OnMenuSet();
	//afx_msg void OnMenuMem();
	afx_msg void OnClose();
	static DWORD RecvUCShutDownHandler(LPVOID pParam);
	static DWORD RecvUCShutDownHandlerUI(LPVOID pParam);
	afx_msg void OnOperateNew();
	afx_msg void OnAutoFocus();
	afx_msg void OnOperateDelete();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void KillProcess(const TCHAR *pszProcessName);
	void DoClean(bool bStrengthen = false,int cleanNum = 0);
	void TestWndDoClean(bool bStrengthen = false, int cleanNum = 0);
	void TestWndDoClean(bool bStrengthen, bool bDayltiClean, int cleanNum);
    void TestWndDoClean(bool bStrengthen, bool bDayltiClean, int strongCleanNum, int dailyCleanNum);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg LRESULT OnSetConsumables(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetCleanTimer(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnUpdateTaskUI(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnWsQcCtrl(WPARAM wp, LPARAM lp);

	bool LCIsShutdown;
	// -------------add by syhu at 2020.09.15-------------------------
	void OnRecvLcRestarting(void *arg, void *cbarg);
	void OnRecvLcRestartEnd(void *arg, void *cbarg);
	void OnRecvDetectedPipeAndIn(void *arg, void *cbarg);
	void OnRecvPipeInSuccess(void *arg, void *cbarg);
	void OnRecvLcNewTask(void *arg, void *cbarg);
	void OnRecvLcNewEmergencyTask(void *arg, void *cbarg);
	void OnRecvRespondTaskFail(void *arg, void *cbarg);
	void OnRecvTaskActionFinish(void *arg, void *cbarg);
	void OnRecvPipeOutShelft(void *arg, void *cbarg);
	void OnRecvCaptureImageReqest(void *arg, void *cbarg);
	void OnRecvBarcodeInfoRequest(void *arg, void *cbarg);
	void OnRecvUdcTestResult(void *arg, void *cbarg);
	void OnRecvLcAlarm(void *arg, void *cbarg);
	void OnRecvSamplingProgress(void *arg, void *cbarg);
	void OnRecvRecognitionProgress(void *arg, void *cbarg);
	void OnRecvSampleTaskFinish(void *arg, void *cbarg);
	void OnRecvTaskFocus(void *arg, void *cbarg);
	void OnRecvPhysicsResult(void *arg, void *cbarg);
	afx_msg LRESULT RecvLcRestartingHandler(WPARAM w, LPARAM l);

	afx_msg LRESULT RecvLcRestartEndHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvDetectedPipeAndInHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvPipeInSuccessHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvLcNewTaskHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvLcNewEmergencyTaskHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvRespondTaskFailHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvTaskActionFinishHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvPipeOutShelftHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvCaptureImageReqestHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvBarcodeTurbidityRequestHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvUdcTestResultHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvLcAlarmHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvSamplingProgressHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvRecognitionProgressHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvSampleTaskFinishHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvTaskFocusHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvTaskIDFromUDC(WPARAM w, LPARAM l);

	virtual void OnSuccess(unsigned int s, unsigned long taskID, char *data, unsigned short len, void *userdata);
	virtual void OnFail(unsigned long taskID, char *data, unsigned short len, int nOperationType, void *userdata);
	afx_msg void OnSoftwarePara();
};
void SendToLisQueue(CMainFrame *pTestUW, int TaskID);
void AutoSendToLisQueue(CMainFrame *pTestUW, int TaskID);
void CharToTchar(const char * _char, TCHAR * tchar);

