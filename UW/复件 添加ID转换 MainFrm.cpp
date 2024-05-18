
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "UW.h"
#include "ManageDevDlg.h"
#include "MainFrm.h"
#include "SetDialog.h"
#include "Setting.h"
#include "TaskMgr.h"
#include "Include/Common/String.h"
#include ".\inc\THUIInterface.h"
#include "TaskMgr.h"
#include "QcDlg.h"
#include "UdcView.h"
#include "AutoFocusMgr.h"
#include ".\inc\THDBUW_Access.h"
#include "DbgMsg.h"
#include "DogInfo.h"
//#include "UWEquipmentSheet.h"



#include "UWView.h"
#include "Camera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_INSERT_UDC_DATA_MSG  0X8123

static  CTabView *m_TabView = NULL;

extern 
std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId);
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)	
	ON_REGISTERED_MESSAGE(AFX_WM_ON_PRESS_CLOSE_BUTTON, &CMainFrame::OnPressCloseBtn)

	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
    ON_WM_CLOSE()
	ON_MESSAGE(WM_AUTO_FOCUS, &CMainFrame::OnAutoFocus)
    ON_MESSAGE(WM_FINDRECORD_LOOKINFO, &CMainFrame::OnFindRecordLookInfo)
	ON_MESSAGE(WM_INSERT_UDC_DATA_MSG, &CMainFrame::OnInsertUDCData)
	ON_MESSAGE(CHANGED_CELL_MSG, &CMainFrame::OnChangedCell)
	ON_MESSAGE(SET_PRINT_PATH_MSG, &CMainFrame::OnSetPrintPathMsg)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_SET,ID_MENU_MEM,&CMainFrame::OnMenuMange)
	ON_COMMAND_RANGE(ID_MENU_SET,ID_MENU_MEM,&CMainFrame::OnMenuDbg)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EQUIPMENT1_DBG,ID_EQUIPMENT1_PARA,&CMainFrame::OnDevMange)
	ON_COMMAND_RANGE(ID_EQUIPMENT1_DBG,ID_EQUIPMENT1_PARA,&CMainFrame::OnDevDbg)

	ON_UPDATE_COMMAND_UI_RANGE(ID_US_QC1,ID_DEVICE_CLEAR_EX1,&CMainFrame::OnOperateMange)
	ON_COMMAND_RANGE(ID_US_QC1,ID_DEVICE_CLEAR_EX1,&CMainFrame::OnOperateDbg)


	ON_COMMAND_RANGE(ID_US1_MIC_CONFIG, ID_US1_MIC, &CMainFrame::OnCaiTuConfig)


	//ON_UPDATE_COMMAND_UI_RANGE(ID_EQUIPMENT1_DBG, ID_EQUIPMENT1_PARA, &CMainFrame::OnUpdateDeviceDbg)
	//ON_COMMAND_RANGE(ID_EQUIPMENT1_DBG, ID_EQUIPMENT1_PARA, &CMainFrame::OnDeviceDbg)

	ON_COMMAND(ID_AUTO_FOCUS, &CMainFrame::OnAutoFocus)
    ON_COMMAND(ID_OPERATE_NEW, &CMainFrame::OnOperateNew)
//    ON_COMMAND(ID_OPERATE_DELETE, &CMainFrame::OnOperateDelete)
	//ON_COMMAND(ID_MENU_SET, &CMainFrame::OnMenuSet)
	//ON_COMMAND(ID_MENU_MEM, &CMainFrame::OnMenuMem)
	//ON_WM_INITMENUPOPUP()

	ON_WM_TIMER()
END_MESSAGE_MAP()


void AutoSendToLis(CMainFrame *pTestUW, int TaskID)
{
	if ( !theApp.IsTaskFinished(TaskID) )
		return;

	if ( 0 == theApp.ReadSendLisSign() )
		return;

	pTestUW->m_wndTodayList.SendLis(TaskID);
}


VOID
SystemMsgThread(
PVOID  pContext)
{
	KSTATUS                      status;
	CMainFrame                  *pTestUW = (CMainFrame *)pContext;
	PSAMPLE_TASK_INFO            pSampleTaskInfo;
	TH_STATUS_INFORMATION        status_info;
	bool bsuccess = false;

	while( TRUE )
	{
		status = THInterface_GetSystemInformation(&status_info, pTestUW->GetExitEvent());
		if( pTestUW->IsExitThread() )
			break;

		if( status != STATUS_SUCCESS )
			continue;
        int nID = 0;
		CString warning;
		CString failInfo=_T("");
		
		DBG_MSG("ThmeInfo:%d\n",status_info.i_class);
		
		switch(status_info.i_class)
		{
		case NewSampleTask:
			{
				//pTestUW->NewTask();
				pSampleTaskInfo = (PSAMPLE_TASK_INFO)status_info.content.buffer;
				 
				DBG_MSG("CheckType = %d\n",theApp.GetCheckType());

				pTestUW->NewTask(pSampleTaskInfo->nHN,pSampleTaskInfo->nCN,theApp.GetCheckType(), pSampleTaskInfo->bTaskResult);
				theApp.UpdateCheckType();                 //质控之后恢复测试类型

				break;
			}
		case SamplingProgress:
			{
				ULONG               nID;
				PSAMPLING_PROGRESS  sprg = (PSAMPLING_PROGRESS)status_info.content.buffer;

				nID = pTestUW->GetTaskIDFromMap(sprg->nTaskID);
				if(  nID > 0 )
				{
					pTestUW->m_wndTodayList.ShowUSSampleProgress(nID, sprg->nPos);
				}
				
				break;
			}
		case SampleTaskFinish:
			{
				ULONG                   nID;
				PSAMPLING_INFORMATION   s_info = (PSAMPLING_INFORMATION)status_info.content.buffer;

				nID = pTestUW->GetTaskIDFromMap(s_info->nTaskID);
				if( nID > 0 )
				{
					pTestUW->m_wndTodayList.UpdateFinishStatus(nID, TRUE, TRUE, (PVOID)&s_info->Coefficient);
					AutoSendToLis(pTestUW, nID);
				}				
	
				break;
			}
		case SampleTaskFailByLC:
			{
				PSAMPLING_INFORMATION   s_info = (PSAMPLING_INFORMATION)status_info.content.buffer;
				ULONG                   nID;

				nID = pTestUW->GetTaskIDFromMap(s_info->nTaskID);
				if( nID > 0 )
				{
					pTestUW->m_wndTodayList.UpdateFinishStatus(nID, TRUE, FALSE, NULL);
					AutoSendToLis(pTestUW, nID);
					//pTestUW->GetUWView()->m_pUdcView->FillList();	
				}						
	
				break;
			}
		case UdcImportFail:
			{
				PUDC_IMPORT_FAIL   udc_fail = (PUDC_IMPORT_FAIL)status_info.content.buffer;
				ULONG                   nID;

				nID = pTestUW->GetTaskIDFromMap(udc_fail->nTaskID);
				if( nID > 0 )
				{
					//udc_fail->nTaskID 处理它就是了...nErrorCode暂时忽略
					pTestUW->m_wndTodayList.UpdateFinishStatus(nID, FALSE, FALSE, NULL);
					AutoSendToLis(pTestUW, nID);

					//pTestUW->GetUWView()->m_pUdcView->FillList();
				}

				
				break;
			}
		case UdcImportEventEx:
			{
				PUDC_IMPORT_INFO_EX   udc_import = (PUDC_IMPORT_INFO_EX)status_info.content.buffer;
				//udc数据
				ULONG                   nID;

				nID = pTestUW->GetTaskIDFromMap(udc_import->ID);
				if( nID > 0 )
				{
					udc_import->ID = nID;
					pTestUW->m_wndTodayList.UpdateFinishStatus(nID, FALSE, TRUE, udc_import);
					pTestUW->UpdateUDCView(udc_import);
					AutoSendToLis(pTestUW, nID);
					//pTestUW->GetUWView()->m_pUdcView->FillList();
				}
				
				
				break;
			}
		case BarcodeEvent:
			{
				PBARCODE_INFORMATION BarCode_info = (PBARCODE_INFORMATION)status_info.content.buffer;
				ULONG                   nID;

				nID = pTestUW->GetTaskIDFromMap(BarCode_info->nTaskID);
				if( nID > 0 )
				{
					BarCode_info->nTaskID = nID;
					pTestUW->m_wndTodayList.UpdateBarCode((PBARCODE_INFORMATION)status_info.content.buffer);
				}
				
				break;
			}
		case PipeInEvent:
			{
				//InterlockedExchangeAdd(&theApp.m_nRow, 1);
				
				theApp.PipeIn();
				break;
			
			}
		case PipeFull:
			{

				pTestUW->OtherInfo((TCHAR *)GetIdsString(_T("mainfrm"),_T("warn1")).c_str());
				break;
			}
		case StrengthenCleanerEmpty://强化清洗液用完
			{
				DBG_MSG("强化清洗液空");
				SendMessage(pTestUW->selfdlg.m_hWnd, WM_CLOSE, 0, 0);
				pTestUW->CloseThreadAndEvent();
				warning = GetIdsString(_T("mainfrm"),_T("warn2")).c_str();
				//pTestUW->OtherInfo((TCHAR *)warning.GetBuffer());
				pTestUW->ManageWarnning(warning,ForceCleanerRunOut);
				break;
			}
		case DailyCleanerEmpty://清洗液用完
			{
				DBG_MSG("清洗液空");
				warning = GetIdsString(_T("mainfrm"),_T("warn3")).c_str();
				pTestUW->OtherInfo((TCHAR *)warning.GetBuffer());
				pTestUW->ManageWarnning(warning,CleanerRunOut);

				break;
			}
		case KeepCleanerEmpty: //保养液空
			{
				DBG_MSG("保养液空");
				warning = GetIdsString(_T("mainfrm"),_T("warn6")).c_str();
				pTestUW->OtherInfo((TCHAR *)warning.GetBuffer());
				pTestUW->ManageWarnning(warning,KeepCleanerRunOut);
				break;
			}
		case LCStartProgress:
			{
				theApp.m_bAutoCheckFail = true;
				if( theApp.m_bIsInAutoFocus == true )
				{
					CAutoFocusMgr::CancelAutoFocus();
					theApp.m_bIsInAutoFocus = false;									
					/*AfxMessageBox(_T("由于下位机复位，自动聚焦失败，请重启软件后再做自动聚焦"));
					SendMessage(pTestUW->m_hWnd, WM_CLOSE, 0, 0);
					break;*/
				}
				//pTestUW->OtherInfo("下位机启动");
		
				//SendMessage(pTestUW->selfdlg.m_hWnd, WM_CLOSE, 0, 0);
				//pTestUW->selfdlg.CDialog::OnOK();
				
				switch( GetPrivateProfileIntA(UDC_CHECK_SETTING, UDC_CHECK_ITEM_COUNT, 11, theApp.m_UDCIni) )
				{
				case 11:
					UDCSpecifyPaperType(PAPER_TYPE_11,bsuccess);
					break;
				case 12:
					UDCSpecifyPaperType(PAPER_TYPE_12,bsuccess);
					break;
				case 13:
					break;
				case 14:
					break;
				}
				theApp.ClearTaskStatus();				
				if (BeginSelfCheck())
				{					
					SetTimer(pTestUW->m_hWnd,2,500,NULL);
				}
				if( theApp.m_bNeesAutoFocus == true )
					DisablePipeIn();
				break;
			}
		case PaperStuck://卡纸
			{
				warning = GetIdsString(_T("mainfrm"),_T("warn4")).c_str();
				pTestUW->OtherInfo((TCHAR *)warning.GetBuffer());
				pTestUW->ManageWarnning(warning,LCPaperStuck);
				break;
			}
		case NoPaper:
			{
				pTestUW->OtherInfo((TCHAR *)GetIdsString(_T("mainfrm"),_T("warn5")).c_str());
				break;
			}
		case LCFailure://自检消息
			{
				 int nFail = 0;

				PLC_FAILURE_INFORMATION    devfail= (PLC_FAILURE_INFORMATION)status_info.content.buffer;
				
				for (int i =0; i < FAILURE_INFO_COUNT; ++i)
				{
					DBG_MSG("devfail->FailureInfoList[i]: %d\n",devfail->FailureInfoList[i]);
					if (devfail->FailureInfoList[i])
					{
						CString strn;
						strn.Format(_T("%d"),i);
						DBG_MSG("nFailIndex:%d\n",i);
						nFail++;
						failInfo += GetIdsString(_T("lcfail"),strn.GetBuffer()).c_str();
						failInfo +=_T("\r\n");
					}
					
				}
				if (nFail > 0)
				{
					theApp.m_bAutoCheckFail = true;
					SendMessage(pTestUW->selfdlg.m_hWnd, WM_CLOSE, 0, 0);
					AfxMessageBox(failInfo);
					DBG_MSG("LC_FAILURE");
				}
				else
				{
					// 自检成功
					bool IsEnableSuccess = false;

					SendMessage(pTestUW->selfdlg.m_hWnd, WM_CLOSE, 0, 0);

					EnableAutoCheck(IsEnableSuccess);				
					if(!IsEnableSuccess)
					{
						AfxMessageBox(GetIdsString(_T("lcfail"),_T("19")).c_str());
						DBG_MSG("EnableAutoCheck_FAILURE");
					}	

					theApp.m_bAutoCheckFail = false;
					if ( theApp.m_IsFirst && theApp.m_bNeesAutoFocus == false )
					{						
						//theApp.m_IsFirst = false;						
						PostMessage(theApp.m_pMainWnd->m_hWnd, WM_AUTO_FOCUS, 0, 0);						
					}

					theApp.UpdateCheckType();				

				}
				break;
			}
		case PipeOutEvent:
			{
				theApp.m_nRow++;
				if (theApp.m_nRow > MAX_ROW)
					theApp.m_nRow = 1;

				theApp.PipeOut();

				/*if( theApp.m_bNeesAutoFocus == true )
					theApp.m_bCanAutoFocus = true;*/

				break;
			}
		default:
			{
				break;
			}
		}

		if( status_info.content.bSysBuffer == TRUE )
			THInterface_FreeSysBuffer(status_info.content.buffer);

	}

}

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
: m_bPicPause(FALSE)
{
	// TODO: 在此添加成员初始化代码
	m_ExitThread = 0;
	m_TabView = NULL;
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
	m_hForceCleanThread = NULL;
	m_MapID.clear();
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	//if (!m_wndMenuBar.Create(this))
	//{
	//	TRACE0("未能创建菜单栏\n");
	//	return -1;      // 未能创建
	//}

	//m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

// 	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
// 		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
// 	{
// 		TRACE0("未能创建工具栏\n");
// 		return -1;      // 未能创建
// 	}
	//menuLoad();



// 	CString strToolBarName;
// 	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
// 	ASSERT(bNameValid);
// 	m_wndToolBar.SetWindowText(strToolBarName);

// 	CString strCustomize;
// 	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
// 	ASSERT(bNameValid);
// 	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作:
//	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));


	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	//m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndMenuBar);
//	DockPane(&m_wndToolBar);


	//EnableDocking(CBRS_ALIGN_ANY);
	//m_wndTodayList.EnableDocking(CBRS_ALIGN_ANY);
	
	DockPane(&m_wndTodayList);

	m_wndTodayList.ShowPane(TRUE,0, TRUE);

	
	

	//m_wndRecordList.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndRecordList.AttachToTabWnd(&m_wndTodayList, DM_SHOW, FALSE, NULL);

	//m_wndPatient.EnableDocking(CBRS_ALIGN_ANY);

	DockPane(&m_wndPatient);

	//m_wndResult.EnableDocking(CBRS_ALIGN_ANY);
	m_wndResult.AttachToTabWnd(&m_wndPatient, DM_SHOW, FALSE, NULL);

	//m_wndOutPut.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutPut);

	//m_wndFindRecord.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFindRecord.AttachToTabWnd(&m_wndOutPut, DM_SHOW, FALSE, NULL);

	m_wndPatient.InitPropList();
	m_wndFindRecord.InitFindList();
	m_wndResult.InitPropList();

		// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	CDockingManager::EnableDockSiteMenu(FALSE);
	//CMFCPopupMenu::EnableMenuSound(FALSE);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	//EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 启用工具栏和停靠窗口菜单替换
//	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
//	CMFCToolBar::EnableQuickCustomization();

// 	if (CMFCToolBar::GetUserImages() == NULL)
// 	{
// 		// 加载用户定义的工具栏图像
// 		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
// 		{
// 			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
// 			CMFCToolBar::SetUserImages(&m_UserImages);
// 		}
// 	}

	 //启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	//lstBasicCommands.AddTail(ID_MENU_SET);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
//	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);

//	CMFCToolBar::SetBasicCommands(lstBasicCommands);
	
	menuLoad();
	InitShowCellView();


	//m_wndTodayList.ShowPane(FALSE,FALSE,TRUE);
   
	hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	hReceiveEvent = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SystemMsgThread,this,0,&m_dwThreadId);

	//SetTimer(1,500,NULL);
		
	SetTimer(5,300,NULL);	
	SetTimer(6,10000,NULL);
	nBeginTime = GetTickCount();
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,               // TODO: 调整行数和列数
		CSize(10, 10),      // TODO: 调整最小窗格大小
		pContext);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void CMainFrame::menuLoad()
{
	SetMenu(NULL);	

	top.LoadMenu(IDR_MAINFRAME);
	SetMenu(&top);

	top.ModifyMenu(0,MF_BYPOSITION, 0, GetIdsString(_T("menu_user"),_T("title")).c_str());
	
	CMenu * pMenu = top.GetSubMenu(0);
    
	if(pMenu)
	{
		pMenu->ModifyMenu(ID_USER_LOGIN, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_user"),_T("gl")).c_str());
		pMenu->ModifyMenu(ID_USER_PASSWORD, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_user"),_T("sz")).c_str());

	}
	

	//top.ModifyMenu(1,MF_BYPOSITION, 0, GetIdsString(_T("menu_edit","title").c_str());
	//CMenu * pMenu1 = top.GetSubMenu(1);

	//if(pMenu1)
	//{
	//	pMenu1->ModifyMenu(ID_CLASS_RBC, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell1").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_WBC, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell2").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_EP, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell3").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell4").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell5").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell6").c_str());
	//	pMenu1->ModifyMenu(7,MF_BYPOSITION, 0,GetIdsString(_T("menu_edit","title1").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_EP1,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell7").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_EP2,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell8").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_EP3,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell9").c_str());
	//	pMenu1->ModifyMenu(8,MF_BYPOSITION, 0,GetIdsString(_T("menu_edit","title2").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST1,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell10").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST2,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell11").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST3,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell12").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST4,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell13").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST5,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell14").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST6,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell15").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST7,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell16").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST8,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell17").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST9,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell18").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST10,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell19").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CAST11,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell20").c_str());
	//	pMenu1->ModifyMenu(9,MF_BYPOSITION, 0,GetIdsString(_T("menu_edit","title3").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY1,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell21").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY2,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell22").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY3,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell23").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY4,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell24").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY5,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell25").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY6,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell26").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY7,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell27").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY8,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell28").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY9,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell29").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY10,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell30").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY11,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell31").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY12,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell32").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY13,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell33").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY14,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell34").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_CRY15,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell35").c_str());
	//	pMenu1->ModifyMenu(10,MF_BYPOSITION, 0,GetIdsString(_T("menu_edit","title4").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER1,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell36").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER2,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell37").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER3,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell38").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER4,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell39").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER5,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell40").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER6,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell41").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER7,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell42").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER8,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell43").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER9,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell44").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER10,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell45").c_str());
	//	pMenu1->ModifyMenu(ID_CLASS_OTHER11,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell46").c_str());
	//	pMenu1->ModifyMenu(12,MF_BYPOSITION, 0,GetIdsString(_T("menu_edit","cell47").c_str());

	//	//pMenu1->ModifyMenu(ID_CLASS_CAST3,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_edit","cell12").c_str());
	//}

	top.ModifyMenu(1,MF_BYPOSITION, 0, GetIdsString(_T("menu_debug"),_T("title")).c_str());
	CMenu * pMenu2 = top.GetSubMenu(2);

	if(pMenu2)
	{
		pMenu2->ModifyMenu(ID_EQUIPMENT_DBG,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("dbg")).c_str());
		pMenu2->ModifyMenu(ID_EQUIPMENT_PARA,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("para")).c_str());
		pMenu2->ModifyMenu(ID_US_QC,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("usqc")).c_str());
		pMenu2->ModifyMenu(ID_UDC_QC,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("udcqc")).c_str());
		pMenu2->ModifyMenu(ID_DEVICE_CLEAR,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("clear")).c_str());
		pMenu2->ModifyMenu(ID_DEVICE_CLEAR_EX,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("clearex")).c_str());
		pMenu2->ModifyMenu(ID_US_MIC,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("mic")).c_str());
		pMenu2->ModifyMenu(ID_US_MIC_CONFIG,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("micconfig")).c_str());
		pMenu2->ModifyMenu(ID_END_TEST,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("end")).c_str());
		pMenu2->ModifyMenu(ID_START_TEST,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("start")).c_str());
		pMenu2->ModifyMenu(ID_AUTO_FOCUS,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("autofocus")).c_str());
		//pMenu2->ModifyMenu(ID_USER_MANAGEMENT, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_user","manage").c_str());
	}

	//top.ModifyMenu(3,MF_BYPOSITION, 0, GetIdsString(_T("menu_config","title").c_str());
	//CMenu * pMenu3 = top.GetSubMenu(2);

	//if(pMenu3)
	//{
	//	pMenu3->ModifyMenu(ID_MENU_DEPARTMENT_DOCTOR,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","doctor").c_str());
	//	pMenu3->ModifyMenu(ID_MENU_HOSPITAL,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","hospital").c_str());
	//	pMenu3->ModifyMenu(ID_MENU_COLOR,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","color").c_str());
	//	pMenu3->ModifyMenu(ID_MENU_TRANSPARENT,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","trans").c_str());
	//	pMenu3->ModifyMenu(ID_MENU_CRITERION_DLG,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","udc").c_str());
	//	pMenu3->ModifyMenu(ID_MENU_US_CRITERION,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","criterion").c_str());
	//	pMenu3->ModifyMenu(ID_MENU_US_ITEM,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","item").c_str());
	//	pMenu3->ModifyMenu(ID_IMG_LOC,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","loc").c_str());
	//	pMenu3->ModifyMenu(ID_WARMING,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","warn").c_str());
	//	pMenu3->ModifyMenu(ID_VISIO_VIEDO,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","viedo").c_str());
	//	pMenu3->ModifyMenu(ID_TUBE_ROW,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_config","row").c_str());
	//}

// 	top.ModifyMenu(2,MF_BYPOSITION, 0, GetIdsString(_T("menu_debug"),_T("title")).c_str());
// 	CMenu * pMenu4 = top.GetSubMenu(2);
// 
// 	if(pMenu4)
// 	{
// 		
// 		
// 	}

/*    top.Detach();
	menu_pop.LoadMenu(IDR_POPUP_OPERATE);
	SetMenu(&menu_pop);
	CMenu *popmenu =menu_pop.GetSubMenu(0);
	if (popmenu)
	{*/
	//	popmenu->ModifyMenu(ID_OPERATE_NEW, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("main_menu","new").c_str());
	//	popmenu->ModifyMenu(ID_OPERATE_DELETE, MF_STRING |MF_BYCOMMAND, ID_OPERATE_DELETE,GetIdsString(_T("main_menu","del").c_str());
	//	popmenu->ModifyMenu(ID_OPERATE_REVISE, MF_STRING |MF_BYCOMMAND, ID_OPERATE_REVISE,GetIdsString(_T("main_menu"),_T("res")).c_str());
	//	popmenu->ModifyMenu(ID_OPERATE_PREVIEW, MF_STRING |MF_BYCOMMAND, ID_OPERATE_PREVIEW,GetIdsString(_T("main_menu","prv").c_str());
	//	popmenu->ModifyMenu(ID_OPERATE_PRINT, MF_STRING |MF_BYCOMMAND, ID_OPERATE_PRINT,GetIdsString(_T("main_menu","pri").c_str());
	//	popmenu->ModifyMenu(ID_OPERATE_RETEST, MF_STRING |MF_BYCOMMAND, ID_OPERATE_RETEST,GetIdsString(_T("main_menu","rtest").c_str());
	//	popmenu->ModifyMenu(ID_OPERATE_US_RETEST, MF_STRING |MF_BYCOMMAND, ID_OPERATE_US_RETEST,GetIdsString(_T("main_menu","usrtest").c_str());
	//	popmenu->ModifyMenu(ID_OPERATE_UDC_RETEST, MF_STRING |MF_BYCOMMAND, ID_OPERATE_UDC_RETEST,GetIdsString(_T("main_menu","udcrtest").c_str());
	//	popmenu->ModifyMenu(ID_OPERATE_SEND_TO_LIS,MF_STRING |MF_BYCOMMAND, ID_OPERATE_SEND_TO_LIS,GetIdsString(_T("main_menu","lis").c_str());
//	}
	//menu_record.LoadMenu(IDR_FIND_RECORD);
	//SetMenu(&menu_record);
	//CMenu *menurecord = menu_record.GetSubMenu(0);
	//if (menurecord)
	//{
	//	menurecord->ModifyMenu(ID_FIND_RECORD_REVISE, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("record","revise").c_str());
	//	menurecord->ModifyMenu(ID_FIND_RECORD_DELETE, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("record","delete").c_str());
	//	menurecord->ModifyMenu(ID_FIND_RECORD_PRINT, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("record","print").c_str());
	//	menurecord->ModifyMenu(ID_FIND_RECORD_PREVIEW, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("record","preview").c_str());

	//}
	//menu_usmic.LoadMenu(IDR_MENU_US_MIC_CONFIG);
	//SetMenu(&menu_usmic);
	//CMenu *usmic = menu_usmic.GetSubMenu(0);
 //   if (usmic)
 //   {
	//	usmic->ModifyMenu(ID_XYADJUST1, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("us_mic_config"),_T("XYADJUST")).c_str());
	//	usmic->ModifyMenu(ID_ZADJUST1, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("us_mic_config"),_T("ZADJUST")).c_str());
	//	usmic->ModifyMenu(ID_X_RESET1, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("us_mic_config"),_T("X_RESET")).c_str());
	//	usmic->ModifyMenu(ID_Y_RESET1, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("us_mic_config"),_T("Y_RESET")).c_str());
	//	usmic->ModifyMenu(ID_Z_RESET1, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("us_mic_config"),_T("Z_RESET")).c_str());
	//	usmic->ModifyMenu(ID_US_MIC_EXIT1, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("us_mic_config"),_T("exit")).c_str());
 //   }

	//menu_print.LoadMenu(IDR_PRINT_PIC);
	//SetMenu(&menu_print);
	//CMenu *m_print = menu_print.GetSubMenu(0);
	//if (m_print)
	//{
 //         m_print ->ModifyMenu(ID_PRINT_PIC,MF_STRING |MF_BYCOMMAND, ID_OPERATE_PRINT,GetIdsString(_T("pic","print").c_str());
	//}*/

}

BOOL CMainFrame::CreateDockingWindows( void )
{

	if (!m_wndTodayList.Create((const TCHAR*)GetIdsString(_T("list_today"),_T("title")).c_str(), this, CRect(0, 0, 360, 200), TRUE, ID_VIEW_TODAY,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建当天列表窗口\n"));
		return FALSE; // 未能创建
	}

	//创建历史记录窗口
/*	if (!m_wndRecordList.Create((const TCHAR*)GetIdsString(_T("list_record"),_T("title")).c_str(), this, CRect(0, 0, 360, 200), TRUE, ID_VIEW_RECORDLIST,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建历史记录列表窗口\n"));
		return FALSE; // 未能创建
	}*/


	// 创建患者窗口
	if (!m_wndPatient.Create((const TCHAR*)GetIdsString(_T("pat"),_T("IDS_PAT_INFO")).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PATIENT, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建患者资料窗口\n"));
		return FALSE; // 未能创建
	}

	// 创建结果窗口
	if (!m_wndResult.Create((const TCHAR*)GetIdsString(_T("wndret"),_T("title")).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_RESULT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建检测结果窗口\n"));
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	if (!m_wndOutPut.Create(_T("输出窗口"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_ALIGN_BOTTOM | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建信息输出窗口\n"));
		return FALSE; // 未能创建
	}

	if (!m_wndFindRecord.Create(GetIdsString(_T("findrecord"),_T("title")).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FINDRECORD,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_ALIGN_BOTTOM | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建查询记录窗口\n"));
		return FALSE; // 未能创建
	}


	return TRUE;

}

//void CMainFrame::OnUpdateMenuSet(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	switch (pCmdUI->m_nID)
//	{
//	case ID_MENU_SET:
//	AfxMessageBox(_T("123"));
//	break;
//	}
//}

//void CMainFrame::OnMenuSet()
//{
//	// TODO: 在此添加命令处理程序代码
//	
//}



//void CMainFrame::OnUpdateMenuSet(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->Enable(TRUE);
//}

//void CMainFrame::OnMenuSet()
//{
	// TODO: 在此添加命令处理程序代码
//	MessageBox(_T("123"));
//}

//void CMainFrame::OnMenuMem()
//{
//	// TODO: 在此添加命令处理程序代码
//	//MessageBox(_T("321"));
//	CSetDialog setting;
//	setting.DoModal();
//}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	if( theApp.IsIdleStatus() == FALSE )  //当前正在做任务...
	{
		MessageBox(GetIdsString(_T("close"),_T("2")).c_str(),GetIdsString(_T("close"),_T("3")).c_str(),MB_OK);
		return;
	}

	m_bClose = TRUE;
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) CloseThread, this, 0, &m_dwThreadId1);
	DisablePipeIn();
	theApp.m_bInDebugMode = true;

	//CTaskMgr::DeleteExtraTasks();
	m_wndTodayList.AutoSaveCurRecord();

	Access_CloseAccess();
	//DisableAutoCheck();


	bool bClean = false, bForceClean = false;

	Clean(bClean);

	ForceClean(bForceClean);
	if( bForceClean == false )
	{
		/*goto CloseClean_lc;*/
	}

	
	
CloseClean_lc:
	::SendMessage(selfdlg.m_hWnd,WM_CLOSE, 0, 0);
	//selfdlg.OnOK();
	//SendMessage()
	DisableAutoCheck();
/*	if ( !bClean || !bForceClean )
	{
		AfxMessageBox(GetIdsString(_T("close"),_T("1")).c_str());
	}*/
	
	CloseThreadAndEvent();
    
		
		
	CFrameWndEx::OnClose();
	
}

LRESULT CMainFrame::OnAutoFocus(WPARAM wParam, LPARAM lParam)
{
	char sFocusPath[MAX_PATH] = {0},temp[MAX_PATH] = {0};
	int  nDoFristFocus;
	 
	GetModuleFileNameA(NULL, sFocusPath, MAX_PATH);
	PathRemoveFileSpecA(sFocusPath);
	PathAppendA(sFocusPath, "config\\Focus.ini");

	GetPrivateProfileStringA("Focus", "DoFristFocus", 0, temp, sizeof(temp), sFocusPath);
	nDoFristFocus = atoi(temp);

	if ( !CAutoFocusMgr::HasConfigCamera() || nDoFristFocus == 0 )
		return 0;

	if ( IDYES == ::MessageBoxW(theApp.m_pMainWnd->m_hWnd, GetIdsString(_T("mainfrm"),_T("warn6")).c_str(), L"", MB_YESNO ) )
	{
		OnAutoFocus();
		nBeginTime = GetTickCount();
	}
	

	return 0;
}

LRESULT CMainFrame::OnFindRecordLookInfo( WPARAM wParam, LPARAM lParam )
{
	theApp.m_nID = wParam;

	//ShowSample(theApp.m_nID);

	//double dMultiple = 3.3;
	//TCHAR chBuf[MAX_PATH] = {0};
	//GetPrivateProfileString(_T("UsCriterion"), _T("LOW"), NULL, chBuf, MAX_PATH, theApp.m_chUsCriterionIni);
	//dMultiple = _tstof(chBuf);
	//SaveResult(theApp.m_nID, dMultiple);

	

	PTASK_INFO task_info = (PTASK_INFO)wParam;

	if( lParam & SHOW_USER_TYPE )
	{
		m_wndPatient.m_WndPropList.ShowData(*task_info);
	}
	if( lParam & (SHOW_UDC_TYPE | SHOW_US_TYPE) )
	{
		m_wndResult.m_WndPropList.ShowData(*task_info, lParam);

		/*if( lParam & SHOW_UDC_TYPE )
			theApp.SaveUDCLog(task_info->main_info.nID,*task_info,TRUE);
		if( lParam & SHOW_US_TYPE ) 
			theApp.SaveUSLog(task_info->main_info.nID,*task_info,TRUE);*/
	}

	return 0;
}

void CMainFrame::OnAutoFocus()
{
	if( theApp.IsIdleStatus() == FALSE || theApp.m_bAutoCheckFail == true || theApp.m_bInDebugMode == true || theApp.m_bIsInAutoFocus == true)	
	{
		EnablePipeIn();
		return;
	}


		
	DisablePipeIn();
	theApp.m_bIsInAutoFocus = true;
	if( theApp.m_IsFirst )
		theApp.m_IsFirst = false;
	CAutoFocusMgr::BeginAutoFocus();	
	theApp.m_bIsInAutoFocus = false;
	//发送开启排管推进命令
	EnablePipeIn();
	DBG_MSG("结束聚焦");
}

void CMainFrame::OnOperateNew()
{

}

void CMainFrame::OnOperateDelete()
{

}

//void CMainFrame::OnDeviceDbg(UINT nID)
//{
//	// 硬件调试和硬件参数对话框
//	CManageDevDlg dlg;
//
//	switch(nID)
//	{
//	case ID_EQUIPMENT1_DBG:
//		{
//			dlg.nSheet = 0;
//			dlg.DoModal();
//		}
//		break;
//	case ID_EQUIPMENT1_PARA:
//		{
//			dlg.nSheet = 1;
//			dlg.DoModal();
//		}
//		break;
//
//	}
//	
//}

//void CMainFrame::OnUpdateDeviceDbg(CCmdUI *pCmdUI)
//{
	//BOOL bEnable = FALSE;
	//if((!theApp.m_bRackIn)
	//	&& (theApp.m_pDevice->m_bInitSuccess == TRUE)
	//	&& (m_Udc_qc_sign == false)
	//	&& (m_Us_qc_sign == false)
	//	&& (!theApp.m_bInReTest)
	//	&& (!theApp.m_bInUdcReTest)
	//	&& (!theApp.m_bInUsReTest)
	//	&& (!theApp.m_bInUdcQc)
	//	&& (!theApp.m_bInUsQc)
	//	&& (!theApp.m_bInClean)
	//	&& (AllTestEnd(m_DeviceData)))
  // if (theApp.loginUser.Group > 0)
 //  {
//	   bEnable = TRUE;
  // }
		

	//pCmdUI->Enable(bEnable);
		
/*
	if(g_isfirst != false)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		g_isfirst = false;
		if(m_bPicPause != FALSE)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
*/

	
//}


HANDLE CMainFrame::GetExitEvent()
{
	return  hExitEvent;

}

BOOL CMainFrame::IsExitThread()
{
	return   (BOOL)(InterlockedExchangeAdd(&m_ExitThread, 0) != 0);

}

//新建任务
//BOOL CMainFrame::NewTask(int nPos,int nCha,int nTestType,BOOL bTaskResult)
//{
//	SYS_WORK_STATUS              sys_work = {0};
//	PRESPOND_SAMPLE              res_sample;
//
//	int nID = m_wndTodayList.Test(nPos,nCha,nTestType,bTaskResult);
//
//	sys_work.nType = RespondSample;
//	res_sample     = (PRESPOND_SAMPLE)sys_work.Reserved;
//
//	res_sample->nTaskID = nID;
//	if (bTaskResult)
//	{
//		if( nID != -1 )
//		{
//			if( nTestType != 1 )  //对于只做尿干化,界面将不会关心
//				theApp.SetTaskStatus(nID);
//		}
//		else
//			res_sample->nHN = 0xff; //表示失败
//
//		THInterface_SetSystemWorkStatusEx(&sys_work);
//	}
//	else
//	{
//		UpdataTaskState(nID,TESTFAIL);
//		return FALSE;
//	}
//
//	return TRUE;
//}
BOOL CMainFrame::NewTask(int nPos,int nCha,int nTestType,BOOL bTaskResult)
{
	int    nResetTestType, nItemIndex;
	LONG   nID;

	//跨天后修改顺序号
	theApp.UpdateSNByDate();

	nID = m_wndTodayList.IsNoramlTestType(nResetTestType, nItemIndex);
	if( nID == -1 ) //正常测试
	{
		DBG_MSG("正常测试 nTestType = %d\n",nTestType);
		return NormalTestType(nPos, nCha, nTestType, bTaskResult);
	}
		

	DBG_MSG("复测 nResetTestType = %d\n",nResetTestType);
	return ResetTestType(nResetTestType, bTaskResult, nID, nItemIndex);

}



void CMainFrame::UpdataTaskState(int nID,StatusEnum nState)
{
    m_wndTodayList.SetStatusToFailed(nID,nState);
}



void CMainFrame::OtherInfo(TCHAR *pMsg)
{
	m_wndOutPut.ShowInfoNoDel(pMsg);
}

void CMainFrame::OnMenuMange(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.IsIdleStatus());

}

void CMainFrame::OnMenuDbg( UINT nID )
{
	CSetDialog setting;
	CSetting  setdlg;
	theApp.m_bInDebugMode = true;

	switch (nID)
	{
	case ID_MENU_SET:
		setdlg.DoModal();
		break;
	case ID_MENU_MEM:
		setting.DoModal();
		break;
	}
	theApp.m_bInDebugMode = false;
}

void CMainFrame::OnDevMange( CCmdUI *pCmdUI )
{
	BOOL bEnable = theApp.IsIdleStatus();
    pCmdUI->Enable(bEnable);
}

void CMainFrame::OnDevDbg( UINT nID )
{
		CManageDevDlg dlg;
		EnterHardwareDebug();	
		theApp.m_bInDebugMode = true;
		switch(nID)
		{
		case ID_EQUIPMENT1_DBG:
			{
				dlg.nSheet = 0;
				dlg.DoModal();
			}
			break;
		case ID_EQUIPMENT1_PARA:
			{
				dlg.nSheet = 1;
				dlg.DoModal();
			}
			break;
	
		}
		ExitHardwareDebug();
		theApp.m_bInDebugMode = false;

		if ( !dlg.m_IsResetLC )
		{
			bool IsEnableSuccess;
			EnableAutoCheck(IsEnableSuccess);
			if (!IsEnableSuccess)
			{
				MessageBox( GetIdsString(_T("warning"), _T("enableautocheckfailed")).c_str() );
			}
		}
}

#define  IDR_MENU_RED_CELL   0X9500
#define  IDR_MENU_DEL_CELL   0X95F0


void CMainFrame::InitShowCellView()
{
	char                        t_path[MAX_PATH];
	USHORT                      j;
	LIST_GROUP_INFO             list_group_info = {0};
	TAB_CTRL_CONTEXT            cxt = {0};
	MENU_DISP_DESCRIPTION       menu_des = {0};
	MENU_COMMAND_DESCRIPTION    cmd_des[MAX_GROUP_COUNTER] = {0};


	menu_cell = CreatePopupMenu();

	
	list_group_info.CellGrapSZ.cx = 48;
	list_group_info.CellGrapSZ.cy = 48;
	list_group_info.nCreateType = TYPE_MFC_TAB_VIEW;
	list_group_info.pOtherCreateContext = &cxt;
	

	for( j = 0 ; j < MAX_US_COUNTS ; j ++ )
	{
		if( theApp.us_cfg_info.par[j].bIsDelete == TRUE )
			continue;

		list_group_info.gn[list_group_info.gCounter].nCellType = CELL_RED_TYPE + j;
		AppendMenuA(menu_cell, MF_BYCOMMAND|MF_STRING, IDR_MENU_RED_CELL + list_group_info.gCounter, theApp.us_cfg_info.par[j].sLongName);
		strcpy_s(list_group_info.gn[list_group_info.gCounter].GroupName, theApp.us_cfg_info.par[j].sLongName);
		cmd_des[list_group_info.gCounter].nMCommandID = IDR_MENU_RED_CELL + list_group_info.gCounter;
		cmd_des[list_group_info.gCounter].nMAction    = CHANGED_CELL_TYPE_ACTION;
		cmd_des[list_group_info.gCounter].nAdditionData = list_group_info.gn[list_group_info.gCounter].nCellType;
		list_group_info.gCounter++;

	}

	if ( theApp.ShowOtherType() )
	{
		AppendMenuA(menu_cell, MF_SEPARATOR, 0, NULL);

		list_group_info.gn[list_group_info.gCounter].nCellType = CELL_OTHER_TYPE;
		strcpy_s(list_group_info.gn[list_group_info.gCounter].GroupName, "其它");
		AppendMenuA(menu_cell, MF_BYCOMMAND|MF_STRING, IDR_MENU_RED_CELL + CELL_OTHER_TYPE, list_group_info.gn[list_group_info.gCounter].GroupName);
		cmd_des[list_group_info.gCounter].nMCommandID = IDR_MENU_RED_CELL + CELL_OTHER_TYPE;
		cmd_des[list_group_info.gCounter].nMAction    = CHANGED_CELL_TYPE_ACTION;
		cmd_des[list_group_info.gCounter].nAdditionData = list_group_info.gn[list_group_info.gCounter].nCellType;
		list_group_info.gCounter++;
	}

	AppendMenuA(menu_cell, MF_SEPARATOR, 0, NULL);
	AppendMenuA(menu_cell, MF_BYCOMMAND|MF_STRING, IDR_MENU_DEL_CELL, "删除");
	cmd_des[list_group_info.gCounter].nMCommandID = IDR_MENU_DEL_CELL;
	cmd_des[list_group_info.gCounter].nMAction    = DELETE_CELL_TYPE_ACTION;
	menu_des.hMenu     = menu_cell;
	menu_des.nMenuMask = MOUSE_L_CLICK_MASK | GRAP_SELECTED_STATUS;
	strcpy_s(t_path, theApp.m_szExeFolderA);
	PathAppendA(t_path, "RecGrapReslut");
	CreateDirectoryA(t_path, NULL);

	THUIInterface_FillParByTabCtrl(theApp.GetMainWnd()->GetSafeHwnd(), t_path, &list_group_info, &menu_des, 1, cmd_des, (USHORT)list_group_info.gCounter + 1);


}

void CMainFrame::SetTabView( PVOID pTabView )
{
	m_TabView = (CTabView*)pTabView;
}

void CMainFrame::ActiveTab( int nIndex )
{
	m_TabView->SetActiveView(nIndex);

}

LRESULT CMainFrame::OnChangedCell( WPARAM wparam, LPARAM lparam )
{
	if( (USHORT)wparam != CELL_OTHER_TYPE )
		m_wndResult.m_WndPropList.ChangeUSCell((PMODIFY_CELL_INFO)wparam, (BOOL)lparam);

	return 0;

}

DWORD WINAPI ForceCleanThread( LPVOID pParam )
{
	CMainFrame *pdlg = (CMainFrame*)pParam;
	CString str= GetIdsString(_T("timer"),_T("4")).c_str();
	pdlg->selfdlg.str = str;	
	pdlg->selfdlg.DoModal();	
	return 0;
}

void CMainFrame::ManageWarnning( CString Warning,CTroubleType nType )
{
	if (IDOK == AfxMessageBox(Warning,MB_OK))
	{	
		if( nType == ForceCleanerRunOut )
		{
			m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ForceCleanThread, this, 0, &m_dwThreadId1);
		}
		TroubleShooting(nType);
		//::SendMessage(selfdlg.m_hWnd, WM_CLOSE, 0, 0);
	}
}

/************************************************
函数名:  GetUWView
函数描述:获得view指针
************************************************/
CUWView* CMainFrame::GetUWView()
{
	CUWView *pView = NULL;
	CUWDoc *pDoc = (CUWDoc*)GetActiveDocument();
	if(NULL == pDoc)
	{
		return NULL;
	}

	POSITION pos = pDoc->GetFirstViewPosition();
	if(NULL != pos)
	{
		pView = (CUWView*)(pDoc->GetNextView(pos));
		return pView;
	}
	else
	{
		return NULL;
	}
}

void CMainFrame::OnCaiTuConfig( UINT nID )
{
	CCamera *pCamera = (CCamera *)GetUWView()->GetTabWnd(VIDEO_CY_VIEW_TAB);


	switch(nID)
	{
	case ID_US1_MIC_CONFIG:
		{
			pCamera->StartUsMicConfig();
		}
		break;
	case ID_END1_TEST:
		{
			// 暂停采图
			pCamera->ControlUSSample(TRUE);
			m_bPicPause = TRUE;
		}
		break;
	case ID_START1_TEST:
		{
			// 重新开始采图
			if(m_bPicPause)
			{
				m_bPicPause = FALSE;
				pCamera->ControlUSSample(FALSE);
			}
			//pCamera->EndUsMicConfig();
		}
		break;
	case ID_US1_MIC: //US采图测试
		{
		
		}
		break;
	}
}






void CMainFrame::DoForceClean()
{
	bool bForceClean = false;
	
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ForceCleanThread, this, 0, &m_dwThreadId1);

	ForceClean(bForceClean);
	::SendMessage(selfdlg.m_hWnd,WM_CLOSE, 0, 0);
	CloseThreadAndEvent();
}


static BOOL bDisablePipe = FALSE;
static bool bNeedForceClean = false;
static int  nForceCleanCount = 0;
static SYSTEMTIME ForceCleantime = {0};


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (1 == nIDEvent)
	//{
	//	KillTimer(1);
	//	if (!theApp.m_startDev)
	//	{
	//		CString str= GetIdsString(_T("timer"),_T("1")).c_str();
	//	   // selfdlg.str = str;
	//		//selfdlg.DoModal();
	//		AfxMessageBox(str);
	//	}
	//}

	SYSTEMTIME systemtime = {0};
	int Hour = 0, Minute = 0;

	if ( 6 == nIDEvent)
	{

		char ConfigFile[MAX_PATH] = {0},temp[MAX_PATH] = {0};
		GetModuleFileNameA(NULL,ConfigFile,MAX_PATH);
		PathRemoveFileSpecA(ConfigFile);
		PathAppendA(ConfigFile,"Config\\setting.ini");

		Hour = GetPrivateProfileIntA("ForceCleanTime","Hour",23,ConfigFile);
		Minute = GetPrivateProfileIntA("ForceCleanTime","Minute",58,ConfigFile);
		
		DBG_MSG("ForceCleanTimeSet Hour:%d,Minute:%d\n",Hour,Minute);

		GetLocalTime(&systemtime);
		DBG_MSG("NOWTime Hour:%d,Minute:%d\n",systemtime.wHour,systemtime.wMinute);
		//if( ( systemtime.wHour == 23 && systemtime.wMinute >= 58 ) || bNeedForceClean == true )

		if( (ForceCleantime.wYear != systemtime.wYear || ForceCleantime.wMonth != systemtime.wMonth || ForceCleantime.wDay != systemtime.wDay ) && nForceCleanCount > 0 )
			nForceCleanCount = 0;
		if( ( systemtime.wHour == Hour && systemtime.wMinute >= Minute ) || bNeedForceClean == true ) 
		{	
			if( nForceCleanCount == 0 && bNeedForceClean == false )
			{
				GetLocalTime(&ForceCleantime);
				DBG_MSG("NeedDoForceCleanTime Hour:%d,Minute:%d\n",ForceCleantime.wHour,ForceCleantime.wMinute);
			}
			if( ForceCleantime.wYear == systemtime.wYear && ForceCleantime.wMonth == systemtime.wMonth && ForceCleantime.wDay == systemtime.wDay && nForceCleanCount > 0 ) 
				return;			

			if( theApp.IsIdleStatus() == false || theApp.m_bAutoCheckFail == true || theApp.m_bInDebugMode == true || theApp.m_bIsInAutoFocus == true )
			{
				bNeedForceClean = true;
				return;
			}

			DoForceClean();
			nForceCleanCount++;
			bNeedForceClean = false;
			
		}
	}


	if ( 2 == nIDEvent)
	{
		KillTimer(2);
		CString str = GetIdsString(_T("timer"),_T("2")).c_str();
		selfdlg.str = str;
		selfdlg.DoModal();
	}

	if ( 3 == nIDEvent)
	{
		KillTimer(3);
		CString str= GetIdsString(_T("timer"),_T("4")).c_str();
		selfdlg.str = str;
		selfdlg.DoModal();
	}
	
	ULONG timespan;
	if ( 5 == nIDEvent)
	{		
		nNowTime = GetTickCount();		
		nAutoFocusInterval = GetAutoFocusInterval();	
		timespan = nNowTime - nBeginTime;
		//DBG_MSG("timespan: %d,nAutoFocusInterval: %d",timespan,nAutoFocusInterval);
		if( timespan > nAutoFocusInterval )
			timespan = nAutoFocusInterval;
		if( timespan < nAutoFocusInterval ||  timespan <= 0 )
			return;		

		
		if( OpenAutoFocus() == false || theApp.m_bAutoCheckFail == true || theApp.m_bInDebugMode == true || theApp.m_bIsInAutoFocus == true )
		{
			return;
		}		

		theApp.m_bNeesAutoFocus = true;
		if( bDisablePipe == FALSE )
		{
			DBG_MSG("发送停止排管推进命令");
			DisablePipeIn();		
			bDisablePipe = TRUE;
		}
		
		if( theApp.IsIdleStatus() == TRUE )
		{
			DBG_MSG("开始聚焦");
			OnAutoFocus();
			bDisablePipe = FALSE;
			theApp.m_bNeesAutoFocus = false;
			nBeginTime = GetTickCount();	
			timespan  = 0;
		}
	}
	CFrameWndEx::OnTimer(nIDEvent);
}

DWORD WINAPI CMainFrame::CloseThread( LPVOID pParam )
{
	CMainFrame *pdlg = (CMainFrame*)pParam;
	pdlg->SetClose();

	return 0;
}

void CMainFrame::SetClose()
{
	while(TRUE)
	{
		if (m_bClose)
		{
			CString str= GetIdsString(_T("timer"),_T("3")).c_str();
			selfdlg.str = str;
			selfdlg.DoModal();
			m_bClose = FALSE;
		}
		
	}

}

#include "DbgMsg.h"
void CMainFrame::OnOperateMange( CCmdUI *pCmdUI )
{
/*	BOOL bEnable = FALSE;

	if (theApp.loginUser.Group > 0)
	{
		DBG_MSG("theApp.loginUser.Group > 0");
	}
	else
	{
		DBG_MSG("theApp.loginUser.Group <= 0");
	}

	if (theApp.IsIdleStatus() == TRUE)
	{
		DBG_MSG("theApp.IsIdleStatus() == TRUE");
	}
	else
	{
		DBG_MSG("theApp.IsIdleStatus() == false");
	}
	if (theApp.loginUser.Group > 0 && theApp.IsIdleStatus() == TRUE) 
	{
		bEnable = TRUE;
	}*/
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnOperateDbg( UINT nID )
{
	bool bflag;
	CQcDlg qcdlg;


	if( theApp.IsIdleStatus() == FALSE )
		return;

	theApp.m_bInDebugMode = true;
	switch(nID)
	{
	case ID_US_QC1:
		qcdlg.nFlag = 2;
		qcdlg.DoModal();
		//SpecialTest(USQC);
		break;
	case ID_UDC_QC1:
		qcdlg.nFlag = 1;
		qcdlg.DoModal();
		//SpecialTest(UDCQC);
		break;
	case ID_DEVICE_CLEAR1:
		Clean(bflag);
		break;
	case ID_DEVICE_CLEAR_EX1:
        //ForceClean(bflag);
		DoForceClean();
		break;
	}
	theApp.m_bInDebugMode = false;
}

void CMainFrame::CloseThreadAndEvent( void )
{
	if (m_hThread)
	{
		// 等待线程结束
		if (WaitForSingleObject(m_hThread, 500) == WAIT_OBJECT_0)
		{
			// 关闭线程句柄
			CloseHandle(m_hThread);
		}
		else
		{
			// 强制结束线程
			TerminateThread(m_hThread, 0);
		}
		m_hThread = NULL;
	}
}

LRESULT CMainFrame::OnInsertUDCData( WPARAM wparam, LPARAM lparam )
{
	CUdcView    *udc_view;
	PTASK_INFO  task_info = (PTASK_INFO)wparam;

	udc_view = (CUdcView*)GetUWView()->GetTabWnd(THUM_UDC_TAB);

	udc_view->AddTaskToList(*task_info, (BOOL)lparam);


	return 0;

}

void CMainFrame::UpdateUDCView( PUDC_IMPORT_INFO_EX udc_import )
{
	PLONG       pValue    =  &udc_import->LEU;
	TASK_INFO   task_info = {0};

	task_info.main_info.nID = udc_import->ID;

	for( int i = 0 ; i < MAX_UDC_COUNTS ; i ++ )
	{
		task_info.udc_info.udc_node[i].udc_col = pValue[i];
		DBG_MSG("UDCResult%d:%d\n",i,task_info.udc_info.udc_node[i].udc_col);
	}


	::SendMessage(this->GetSafeHwnd(), WM_INSERT_UDC_DATA_MSG, (WPARAM)&task_info, (LPARAM)FALSE);
	
}

BOOL CMainFrame::NormalTestType( int nPos,int nCha,int nTestType, BOOL bTaskResult )
{
	BOOL                         bRet;
	TASK_INFO                    task_info;
	SYS_WORK_STATUS              sys_work = {0};
	PRESPOND_SAMPLE              res_sample;

	int nID = m_wndTodayList.Test(nPos,nCha,nTestType,bTaskResult,task_info);

	sys_work.nType = RespondSample;
	res_sample     = (PRESPOND_SAMPLE)sys_work.Reserved;
	
	if (bTaskResult)
	{
		bRet = TRUE;
		if( nID != -1 )
			theApp.SetTaskStatus(nID,nTestType);
		else
			res_sample->nHN = 0xff; //表示失败
	}
	else
	{
		UpdataTaskState(nID,TESTFAIL);
		bRet = FALSE;
	}

	res_sample->nTaskID = m_MapID.size();
	m_MapID.push_back(nID);

	THInterface_SetSystemWorkStatusEx(&sys_work);
	::SendMessage(this->GetSafeHwnd(), WM_INSERT_UDC_DATA_MSG, (WPARAM)&task_info, (LPARAM)TRUE);


	return bRet;

}

BOOL CMainFrame::ResetTestType( int nResetTestType, BOOL bTaskResult, LONG nID, int nItemIndex )
{
	ULONG                        nStatus;
	StatusEnum                   nUSStatus, nUDCStatus;
	SYS_WORK_STATUS              sys_work = {0};
	PRESPOND_SAMPLE              res_sample;

	sys_work.nType = RespondSample;
	res_sample     = (PRESPOND_SAMPLE)sys_work.Reserved;

	res_sample->nTaskID = m_MapID.size();
	m_MapID.push_back(nID);

	if( bTaskResult == FALSE )
	{
		if( nResetTestType & UDC_TEST_TYPE )
			m_wndTodayList.UpdateFinishStatus(nID,FALSE, FALSE,NULL);
		if( nResetTestType & US_TEST_TYPE )
			m_wndTodayList.UpdateFinishStatus(nID,TRUE, FALSE,NULL);
	}
	else
	{
		nStatus = m_wndTodayList.GetItemStatus(nItemIndex);
		theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
		if( nResetTestType & UDC_TEST_TYPE )
			nUDCStatus = NEW;
		if( nResetTestType & US_TEST_TYPE )
			nUSStatus = NEW;

		nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
		m_wndTodayList.ResetItemMaskCode(nItemIndex, nID, nStatus);
	}

	THInterface_SetSystemWorkStatusEx(&sys_work);

	return TRUE;
}

LRESULT CMainFrame::OnSetPrintPathMsg( WPARAM wparam, LPARAM lparam )
{
	return (Access_SetPrintPath((PUPDATE_PRINT_PATH1)wparam) == STATUS_SUCCESS);
}

BOOL CMainFrame::OnCloseMiniFrame( CPaneFrameWnd* pWnd )
{
	return FALSE;

}

BOOL CMainFrame::OnCloseDockingPane( CDockablePane* pWnd )
{
	return FALSE;

}

LRESULT CMainFrame::OnPressCloseBtn( WPARAM wp,LPARAM lp )
{
	return TRUE; 

}

bool CMainFrame::OpenAutoFocus()
{
	char ConfigFile[MAX_PATH] = {0},temp[MAX_PATH] = {0};
	GetModuleFileNameA(NULL,ConfigFile,MAX_PATH);
	PathRemoveFileSpecA(ConfigFile);
	PathAppendA(ConfigFile,"Config\\Focus.ini");
	
	GetPrivateProfileStringA("Focus","AutoFocus",0,temp,MAX_PATH,ConfigFile);

	if( atoi(temp) == 1 )
		return true;
	else
		return false;
}

ULONG CMainFrame::GetAutoFocusInterval()
{
	char ConfigFile[MAX_PATH] = {0},temp[MAX_PATH] = {0};
	GetModuleFileNameA(NULL,ConfigFile,MAX_PATH);
	PathRemoveFileSpecA(ConfigFile);
	PathAppendA(ConfigFile,"Config\\Focus.ini");

	GetPrivateProfileStringA("Focus","AutoFocusInterval","30",temp,MAX_PATH,ConfigFile);

	return atoi(temp) * 60000;
}

ULONG CMainFrame::GetTaskIDFromMap( ULONG nMapIndex )
{
	if( nMapIndex < m_MapID.size() )
	{
		return m_MapID[nMapIndex];
	}
	else
	{
		return 0;
	}
}
