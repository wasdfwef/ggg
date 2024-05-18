
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "UW.h"
#include "ManageDevDlg.h"
#include "MainFrm.h"
#include "TaskMgr.h"
#include "Include/Common/String.h"
#include "..\..\..\inc\THUIInterface.h"
#include "TaskMgr.h"

#include "UdcView.h"
#include "AutoFocusMgr.h"
#include "..\..\..\inc\THDBUW_Access.h"
#include "DbgMsg.h"
#include "DogInfo.h"C
#include "DbgPassword.h"
#include "UdcCriterionDlg.h"
#include "PicImageArgvDlg.h"
//#include "UWEquipmentSheet.h"
#include <thread>
#include <future>
#include "ProcessManager.h"
#include "..\..\..\inc\CStartComSet.h"
#include "..\..\..\inc\CStartCalibrate.h"
#include "UWView.h"
#include "Camera.h"
#include "RecTask.h"

#include <sapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 添加MessageBoxTimeout支持
extern "C"
{
	int WINAPI MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
	int WINAPI MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
};
#ifdef UNICODE
#define MessageBoxTimeout MessageBoxTimeoutW
#else
#define MessageBoxTimeout MessageBoxTimeoutA
#endif

#define WM_INSERT_UDC_DATA_MSG  0X8123
#define WM_CTRL_TIP_DLG_MSG  0X1123

#define WM_LCRSTARTING       WM_USER+0xB1
#define WM_LCRSTARTEND       WM_USER+0xB2
#define WM_DETECTEDPIPEANDIN WM_USER+0xB3
#define WM_PIPEINSUCCESS     WM_USER+0xB4
#define WM_LCNEWTASK         WM_USER+0xB5
#define WM_LCNEWEMERGENCYTASK WM_USER+0xB6
#define WM_RESPONDTASKFAILED WM_USER+0xB7
#define WM_TASKACTIONFINISH  WM_USER+0xB8
#define WM_PIPEOUTSHELF      WM_USER+0xB9
#define WM_CAPTUREIMAGEREQ   WM_USER+0xBA
#define WM_BARTURBREQUEST    WM_USER+0xBB
#define WM_UDCRESULT         WM_USER+0xBC
#define WM_LCALARM           WM_USER+0xBD
#define WM_SAMPLINGPROGRESS  WM_USER+0xBE
#define WM_RECOPROGRESS      WM_USER+0xBF
#define WM_SAMPLETASKFINISH  WM_USER+0xC1
#define WM_TASKFOCUS		 WM_USER+0xC2
#define WM_UCSHUTDOWN		 WM_USER+0xC3

#if (_LC_V <= 2150)
#define RESET_FLAG_COUNT (15)
#elif (_LC_V >= 2180)
#define RESET_FLAG_COUNT (14)
#else
#define RESET_FLAG_COUNT (15)
#endif

#define CLEAN_TIMER_1 (6)
#define CLEAN_TIMER_2 (7)

#define RESULT_DOCK_LEFT 0

static  CTabView *m_TabView = NULL;
extern std::vector<positiveID> positiveUDCID;
extern std::vector<positiveID> positiveUSID;

std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId);
// CMainFrame

#define MAX_COLUMN 12

#define STRONG_CLEAN_TIMER (0xAB)

typedef void (CMainFrame::*ObjFun)(void*, void*);
struct MyEventHanlder 
{
	int eventType;
	ObjFun fun;
	void *arg;
};
static MyEventHanlder m_lcEventHandler[] = {
	{ LcReStarting,				&CMainFrame::OnRecvLcRestarting, NULL },
	{ LcReStartEnd,				&CMainFrame::OnRecvLcRestartEnd, NULL },
	{ DetectedPipeAndIn,		&CMainFrame::OnRecvDetectedPipeAndIn, NULL },
	{ PipeInSuccess,			&CMainFrame::OnRecvPipeInSuccess, NULL },
	{ LcNewTask,				&CMainFrame::OnRecvLcNewTask, NULL },
	{ LcNewEmergencyTask,		&CMainFrame::OnRecvLcNewEmergencyTask, NULL },
	{ RespondTaskFail,			&CMainFrame::OnRecvRespondTaskFail, NULL },
	{ TaskActionFinish,			&CMainFrame::OnRecvTaskActionFinish, NULL },
	{ PipeOutShelf,				&CMainFrame::OnRecvPipeOutShelft, NULL },
	{ CaptureImageRequest,		&CMainFrame::OnRecvCaptureImageReqest, NULL },
	{ BarcodeRequest,			&CMainFrame::OnRecvBarcodeInfoRequest, NULL },
	{ UdcResult,				&CMainFrame::OnRecvUdcTestResult, NULL },
	{ LcAlarm,					&CMainFrame::OnRecvLcAlarm, NULL },
	{ SamplingProgress,			&CMainFrame::OnRecvSamplingProgress, NULL },
	{ RecognitionProgress,		&CMainFrame::OnRecvRecognitionProgress, NULL },
	{ SampleTaskFinish,			&CMainFrame::OnRecvSampleTaskFinish, NULL },
	{ TaskFocus,				&CMainFrame::OnRecvTaskFocus, NULL },
	{ PhysicsResult,            &CMainFrame::OnRecvPhysicsResult, NULL },
};

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_WM_MENUSELECT()

    ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
    ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
    ON_REGISTERED_MESSAGE(AFX_WM_ON_PRESS_CLOSE_BUTTON, &CMainFrame::OnPressCloseBtn)

    ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)

    ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_SET, IDC_MENU_UDCSET, &CMainFrame::OnMenuMange)
    ON_COMMAND_RANGE(ID_MENU_SET, IDC_MENU_UDCSET, &CMainFrame::OnMenuDbg) // 管理、设置 

    ON_UPDATE_COMMAND_UI_RANGE(ID_SOFTWARE_PARA, ID_EQUIPMENT1_PARA, &CMainFrame::OnDevMange)
    ON_COMMAND_RANGE(ID_SOFTWARE_PARA, ID_EQUIPMENT1_PARA, &CMainFrame::OnDevDbg) // 设备调试、设备参数

    ON_UPDATE_COMMAND_UI_RANGE(ID_US_QC1, ID_DEVICE_CLEAR_EX1, &CMainFrame::OnOperateMange)
    ON_COMMAND_RANGE(ID_US_QC1, ID_DEVICE_CLEAR_EX1, &CMainFrame::OnOperateDbg)  // 尿沉渣质控、尿干化质控、清洗、强化清洗

    ON_UPDATE_COMMAND_UI_RANGE(ID_US1_MIC_CONFIG, ID_US1_MIC, &CMainFrame::OnCaiTuUIMange)
    ON_COMMAND_RANGE(ID_US1_MIC_CONFIG, ID_US1_MIC, &CMainFrame::OnCaiTuConfig) // 尿沉渣显微镜设置、暂停测试、继续测试

    ON_COMMAND(ID_AUTO_FOCUS, &CMainFrame::OnAutoFocus)
    ON_COMMAND(ID_OPERATE_NEW, &CMainFrame::OnOperateNew)

    ON_MESSAGE(WM_AUTO_FOCUS, &CMainFrame::OnAutoFocus)
    ON_MESSAGE(WM_FINDRECORD_LOOKINFO, &CMainFrame::OnFindRecordLookInfo)
    ON_MESSAGE(WM_INSERT_UDC_DATA_MSG, &CMainFrame::OnInsertUDCData)
    ON_MESSAGE(CHANGED_CELL_MSG, &CMainFrame::OnChangedCell)
    ON_MESSAGE(SET_PRINT_PATH_MSG, &CMainFrame::OnSetPrintPathMsg)
    ON_MESSAGE(WM_TODAYLIST_UPDATELISTPOS, &CMainFrame::OnUpdateListPos)
    ON_MESSAGE(WM_HAOCAI, &CMainFrame::OnSetConsumables)
    ON_MESSAGE(GET_PRINT_PATH_MSG, &CMainFrame::OnGetPrintPathMsg)

    ON_MESSAGE(WM_CTRL_TIP_DLG_MSG, &ShowSelfDlgHandler)

    ON_MESSAGE(WM_LCRSTARTING, &RecvLcRestartingHandler)
    ON_MESSAGE(WM_UCSHUTDOWN, &SetClose)
    ON_MESSAGE(WM_LCRSTARTEND, &RecvLcRestartEndHandler)
    ON_MESSAGE(WM_DETECTEDPIPEANDIN, &RecvDetectedPipeAndInHandler)
    ON_MESSAGE(WM_PIPEINSUCCESS, &RecvPipeInSuccessHandler)
    ON_MESSAGE(WM_LCNEWTASK, &RecvLcNewTaskHandler)
    ON_MESSAGE(WM_LCNEWEMERGENCYTASK, &RecvLcNewEmergencyTaskHandler)
    ON_MESSAGE(WM_TASKACTIONFINISH, &RecvTaskActionFinishHandler)
    ON_MESSAGE(WM_PIPEOUTSHELF, &RecvPipeOutShelftHandler)
    ON_MESSAGE(WM_CAPTUREIMAGEREQ, &RecvCaptureImageReqestHandler)
    ON_MESSAGE(WM_BARTURBREQUEST, &RecvBarcodeTurbidityRequestHandler)
    ON_MESSAGE(WM_UDCRESULT, &RecvUdcTestResultHandler)
    ON_MESSAGE(WM_LCALARM, &RecvLcAlarmHandler)
    ON_MESSAGE(WM_SAMPLINGPROGRESS, &RecvSamplingProgressHandler)
    ON_MESSAGE(WM_RECOPROGRESS, &RecvRecognitionProgressHandler)
    ON_MESSAGE(WM_SAMPLETASKFINISH, &RecvSampleTaskFinishHandler)
    ON_MESSAGE(WM_TASKFOCUS, &RecvTaskFocusHandler)
    ON_MESSAGE(WM_SELECTRECORDBYUDC, &RecvTaskIDFromUDC)

    ON_COMMAND(ID_SOFTWARE_PARA, &CMainFrame::OnSoftwarePara)

    ON_COMMAND_RANGE(ID_INDICATOR_EXT, ID_INDICATOR_USER, NULL)

    ON_MESSAGE(WM_SETCLEANTIMER2, &CMainFrame::OnSetCleanTimer)
    ON_MESSAGE(WM_UPDATE_TASKUI, &CMainFrame::OnUpdateTaskUI)

    ON_MESSAGE(WM_WS_QC_CTRL, &CMainFrame::OnWsQcCtrl)
END_MESSAGE_MAP()


void AutoSendToLis(CMainFrame *pTestUW, int TaskID)
{
	DBG_MSG("debug:send lis task id = %d",TaskID);
	if ( !theApp.IsTaskFinished(TaskID) )
		return;

	if ( 0 == theApp.ReadSendLisSign() )
		return;

	TCHAR INIPath[256] = { 0 };

	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("config\\Lis1.ini"));
	if (2020 == theApp.m_sendLisType && !theApp.m_bIsLiushuixain)
	{
		pTestUW->m_wndTodayList.SendLis(TaskID, 2020);
	}
	else if (theApp.m_bIsLiushuixain)
	{
		pTestUW->m_wndTodayList.SendLisLisShuiXian(TaskID, 2020);
	}
	else
	{
		pTestUW->m_wndTodayList.SendLis(TaskID);
	}
}

void SendToLisQueue(CMainFrame *pTestUW, int TaskID)
{
    DBG_MSG("debug:send lis task id = %d", TaskID);
    if (!theApp.IsTaskFinished(TaskID))
        return;

    CriticalSection m_cs(pTestUW->m_sendLisCS);
    pTestUW->m_SendLisQueue.push(TaskID);
}

void AutoSendToLisQueue(CMainFrame *pTestUW, int TaskID)
{
	DBG_MSG("debug:send lis task id = %d", TaskID);
	if (!theApp.IsTaskFinished(TaskID))
		return;

	if (0 == theApp.ReadSendLisSign())
		return;

	CriticalSection m_cs(pTestUW->m_sendLisCS);
	pTestUW->m_SendLisQueue.push(TaskID);
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
	//int nClear;
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
		
		int cleartimes = 0;
		switch(status_info.i_class)
		{
		case NewSampleTask:
			{
				/*int remain = 0;
				bool bIsExpired = false;
				theApp.QueryConsumables(1, remain, bIsExpired);*/

				pSampleTaskInfo = (PSAMPLE_TASK_INFO)status_info.content.buffer;
				int newTaskId = 0;
				pTestUW->NewTask(pSampleTaskInfo->nHN, pSampleTaskInfo->nCN, theApp.GetCheckType(), pSampleTaskInfo->bTaskResult, newTaskId);
				DBG_MSG("CheckType = %d\n",theApp.GetCheckType());
				theApp.UpdateCheckType();                 //质控之后恢复测试类型

				break;
			}
		case SamplingProgress:
			{
				theApp.m_bSaveDate = false;
				PSAMPLING_PROGRESS  sprg = (PSAMPLING_PROGRESS)status_info.content.buffer;

				pTestUW->m_wndTodayList.ShowUSSampleProgress(sprg->nTaskID, sprg->nPos);
				break;
			}
		case SampleTaskFinish:
			{
				
				PSAMPLING_INFORMATION   s_info = (PSAMPLING_INFORMATION)status_info.content.buffer;

				pTestUW->m_wndTodayList.UpdateFinishStatus(s_info->nTaskID, TRUE, TRUE, (PVOID)&s_info->Coefficient);
				DBG_MSG("debug:SampleTaskFinish send lis task id = %d", s_info->nTaskID);
				AutoSendToLisQueue(pTestUW, s_info->nTaskID);
				DBG_MSG("TaskFinish TaskID:%d\n",s_info->nTaskID);
				theApp.m_bSaveDate = true;
				break;
			}
		case SampleTaskFailByLC:
			{
				PSAMPLING_INFORMATION   s_info = (PSAMPLING_INFORMATION)status_info.content.buffer;
				
				pTestUW->m_wndTodayList.UpdateFinishStatus(s_info->nTaskID, TRUE, FALSE, NULL);
				DBG_MSG("debug:SampleTaskFailByLC send lis task id = %d", s_info->nTaskID);
				AutoSendToLisQueue(pTestUW, s_info->nTaskID);
				//pTestUW->GetUWView()->m_pUdcView->FillList();			
	
				break;
			}
		case UdcImportFail:
			{
				PUDC_IMPORT_FAIL   udc_fail = (PUDC_IMPORT_FAIL)status_info.content.buffer;
				DBG_MSG("debug:UdcImportFail send lis task id = %d", udc_fail->nTaskID);
				//udc_fail->nTaskID 处理它就是了...nErrorCode暂时忽略
				pTestUW->m_wndTodayList.UpdateFinishStatus(udc_fail->nTaskID, FALSE, FALSE, NULL);

				AutoSendToLisQueue(pTestUW, udc_fail->nTaskID);

				//pTestUW->GetUWView()->m_pUdcView->FillList();
				break;
			}
		case UdcImportEventEx:
			{
				PUDC_IMPORT_INFO_EX   udc_import = (PUDC_IMPORT_INFO_EX)status_info.content.buffer;
				//udc数据
				
				pTestUW->m_wndTodayList.UpdateFinishStatus(udc_import->ID, FALSE, TRUE, udc_import);
				//pTestUW->UpdateUDCView(udc_import);
				DBG_MSG("debug:UdcImportEventEx send lis task id = %d", udc_import->ID);
				AutoSendToLisQueue(pTestUW, udc_import->ID);
			    //pTestUW->GetUWView()->m_pUdcView->FillList();
				break;
			}
		case BarcodeEvent:
			{
				//pTestUW->m_wndTodayList.UpdateBarCode((PBARCODE_INFORMATION)status_info.content.buffer);
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

				pTestUW->OutputMessage((TCHAR *)GetIdsString(_T("mainfrm"),_T("warn1")).c_str());
				break;
			}
		case StrengthenCleanerEmpty://强化清洗液用完
			{
				DBG_MSG("强化清洗液空");			
				warning = GetIdsString(_T("mainfrm"),_T("warn2")).c_str();
				if( !pTestUW->m_bClose )
					pTestUW->OutputMessage((TCHAR *)warning.GetBuffer());
				//pTestUW->ManageWarnning(warning,ForceCleanerRunOut);
				break;
			}
		case DailyCleanerEmpty://清洗液用完
			{
				DBG_MSG("清洗液空");
				warning = GetIdsString(_T("mainfrm"),_T("warn3")).c_str();
				if( !pTestUW->m_bClose )
					pTestUW->OutputMessage((TCHAR *)warning.GetBuffer());
				//pTestUW->ManageWarnning(warning,CleanerRunOut);

				break;
			}
		case KeepLeanerCount: //保养液空
		case KeepCleanerEmpty: //保养液空
			{
				DBG_MSG("保养液空");			
				warning = GetIdsString(_T("mainfrm"),_T("warn7")).c_str();
				if( !pTestUW->m_bClose )
					pTestUW->OutputMessage((TCHAR *)warning.GetBuffer());
				//pTestUW->ManageWarnning(warning,KeepCleanerRunOut);
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

				//if( bsuccess == false )
				//{
				//	pTestUW->OtherInfo(_T("UDC模块通信失败"));
				//	//AfxMessageBox(_T("UDC模块通信失败"));
				//}
				theApp.ClearTaskStatus();				
				if (BeginSelfCheck())//下位机自检
				{					
					SetTimer(pTestUW->m_hWnd, 2, 500, NULL);
				}
				break;
			}
		case PaperStuck://卡纸
			{
				warning = GetIdsString(_T("mainfrm"),_T("warn4")).c_str();
				pTestUW->OutputMessage((TCHAR *)warning.GetBuffer());
				//pTestUW->ManageWarnning(warning,LCPaperStuck);
				break;
			}
		case NoPaper:
			{
				pTestUW->OutputMessage((TCHAR *)GetIdsString(_T("mainfrm"),_T("warn5")).c_str());
				break;
			}
		case LCFailure://自检消息
			{
				 int nFail = 0;

				PLC_FAILURE_INFORMATION    devfail= (PLC_FAILURE_INFORMATION)status_info.content.buffer;
				
				for (int i =0; i < FAILURE_INFO_COUNT; ++i)
				{
					DBG_MSG("devfail->FailureInfoList[%d]: %d\n", i, devfail->FailureInfoList[i]);
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
					PostMessage(pTestUW->selfdlg.m_hWnd, WM_CLOSE, 0, 0);
					AfxMessageBox(failInfo);
					DBG_MSG("LC_FAILURE");
				}
				else
				{
					// 自检成功
					bool IsEnableSuccess = false;
					DBG_MSG("自检成功111\n");
					PostMessage(pTestUW->selfdlg.m_hWnd, WM_CLOSE, 0, 0);

					//theApp.LoadRecMode();
					// 开始自动检测
					EnableAutoCheck(IsEnableSuccess);				
					if(!IsEnableSuccess)
					{
						AfxMessageBox(GetIdsString(_T("lcfail"),_T("19")).c_str());
						DBG_MSG("EnableAutoCheck_FAILURE");
					}	
					DBG_MSG("自检成功222 EnableAutoCheck\n");

					//theApp.m_bAutoCheckFail = false;
					//if ( theApp.m_IsFirst && theApp.m_bNeesAutoFocus == false )
					//{						
					//	//theApp.m_IsFirst = false;						
					//	PostMessage(theApp.m_pMainWnd->m_hWnd, WM_AUTO_FOCUS, 0, 0);						
					//}
					//theApp.LoadRecMode2();
					int nRecMode = theApp.GetModeIndex();


					if( nRecMode == 0 )
					{
						//if( InitRecInfo() == FALSE )	
						//theApp.InitRecInfo();
						if( theApp.GetRecInitResult() == FALSE )
						{	
							AfxMessageBox(_T("识别模块初始化错误"));
							//return FALSE;
						}
					}
					

					theApp.UpdateCheckType();				

				}
				break;
			}
		case PipeOutEvent:
			{
/*				theApp.m_nTubeShelfNum++;*/
				DBG_MSG("debug:下一架号为%d",theApp.m_nTubeShelfNum);
				if (theApp.m_nTubeShelfNum > MAX_ROW)
					theApp.m_nTubeShelfNum = 1;

				theApp.PipeOut();

				/*if( theApp.m_bNeesAutoFocus == true )
					theApp.m_bCanAutoFocus = true;*/

				break;
			}
		case QueryConsumables:
		{
			if (pTestUW->m_bCheckCunsumablesSuccess)
			{
				pTestUW->m_bCheckCunsumablesSuccess = false;
				SetEvent(pTestUW->m_hCheckCardEvent);
			}
			else
			{
				NotifyConsumables(0);
			}
			break;
		}
		default:
			{
				break;
			}
		}

		if (status_info.content.bSysBuffer == TRUE)
		{
			THInterface_FreeSysBuffer(status_info.content.buffer);
		}
	}
}


static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	//ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_NUM,
	ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
: m_bPicPause(FALSE)
{
	// TODO: 在此添加成员初始化代码
	m_bClose = FALSE;
	m_ExitThread = 0;
	m_TabView = NULL;
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
	m_hForceCleanThread = NULL;	
	lastNormalTestTime = { 0 };
	m_bCheckCunsumablesSuccess = true;
	m_hCheckCardEvent = NULL;
	m_hExitCheckCardEvent = NULL;
	isLCReset = false;
	m_HaveOntimeClean = false;
	qcDlg = NULL;
/*	qcDlg = new CQcDlg;*/
	InitializeCriticalSection(&m_sendLisCS);
}

CMainFrame::~CMainFrame()
{
	CloseHandle(lisHandle);
	DeleteCriticalSection(&m_sendLisCS);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
//	CFrameWndEx::ModifyStyle(0,WS_VSCROLL | WS_HSCROLL,0);



	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);
	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);
	
	/*if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
		{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
		}*/
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
	CString str;
	m_wndStatusBar.SetPaneWidth(1, 250);
	m_wndStatusBar.SetPaneText(1, _T(""));
	str.Format(_T("用户：%s(%s)"), StringToWstring(theApp.loginUser.sAccount).c_str(), StringToWstring(theApp.loginUser.sName).c_str());
	m_wndStatusBar.SetPaneWidth(2, theApp.m_bIsLiushuixain ? 0 : 150);
	m_wndStatusBar.SetPaneText(2, str); 
	m_wndStatusBar.SetPaneTextColor(2, RGB(0,102,204));
	m_wndStatusBar.SetPaneWidth(3, 150); 
	m_wndStatusBar.SetPaneText(3, _T(""));
	//m_wndStatusBar.SetPaneText(2, _T("当前耗材剩余量: 10000"));
	if (theApp.m_bIsControlConsumable)
	{
		auto f = std::async(std::launch::async, [=](){
			Sleep(1000);
			int count;
			bool isExpired;
			CWnd * wnd = theApp.m_pMainWnd;
 			theApp.QueryConsumables(0, count, isExpired, m_hWnd);
		});
	}

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	//m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndMenuBar);
	//DockPane(&m_wndToolBar);


	//EnableDocking(CBRS_ALIGN_ANY);
	//m_wndTodayList.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndBtnTools);
	m_wndBtnTools.ShowPane(TRUE, 0, TRUE);
	
	DockPane(&m_wndTodayList);

	m_wndTodayList.ShowPane(TRUE,0, TRUE);

	//m_wndRecordList.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndRecordList.AttachToTabWnd(&m_wndTodayList, DM_SHOW, FALSE, NULL);

	//m_wndPatient.EnableDocking(CBRS_ALIGN_ANY);

/*	m_wndResult.DockToWindow(&m_wndTodayList, CBRS_ALIGN_RIGHT);*/

	DockPane(&m_wndResult);
	//m_wndResult.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPatient.AttachToTabWnd(&m_wndResult, DM_SHOW, FALSE, NULL);

	//m_wndOutPut.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutPut);

	//m_wndFindRecord.EnableDocking(CBRS_ALIGN_ANY);
/*	m_wndFindRecord.AttachToTabWnd(&m_wndOutPut, DM_SHOW, FALSE, NULL);*/

	m_wndPatient.InitPropList();
/*	m_wndFindRecord.InitFindList();*/

	CPngButton setBtn;//创建按钮
	m_wndBtnTools.InsertBtn(setBtn, IDB_PNG_SET, IDC_BTN_SET, GetIdsString(L"wndtooltipstr", L"set").c_str());
	CPngButton searchBtn;//创建按钮
	m_wndBtnTools.InsertBtn(searchBtn, IDB_PNG_SEARCH, IDC_BTN_SEARCH, GetIdsString(L"wndtooltipstr", L"search").c_str());
	CPngButton usermgrBtn;//创建按钮
	m_wndBtnTools.InsertBtn(usermgrBtn, IDB_PNG_USERMGR, IDC_BTN_USERMGR, GetIdsString(L"wndtooltipstr", L"usermgr").c_str());
	CPngButton hospitalBtn;//创建按钮
	m_wndBtnTools.InsertBtn(hospitalBtn, IDB_PNG_HOSPTIAL, IDC_BTN_HOSPTIAL, GetIdsString(L"wndtooltipstr", L"hospital").c_str());
	CPngButton serialPortBtn;//创建按钮
	m_wndBtnTools.InsertBtn(serialPortBtn, IDB_PNG_SERIALPORT, IDC_BTN_SERIALPORT, GetIdsString(L"wndtooltipstr", L"serialport").c_str());
	CPngButton colorBtn;//创建按钮
	m_wndBtnTools.InsertBtn(colorBtn, IDB_PNG_COLOR, IDC_BTN_COLOR, GetIdsString(L"wndtooltipstr", L"color").c_str());
	CPngButton tempBtn;//创建按钮
	m_wndBtnTools.InsertBtn(tempBtn, IDB_PNG_TEMP, IDC_BTN_TEMP, GetIdsString(L"wndtooltipstr", L"temp").c_str());
	CPngButton cleanBtn;//创建按钮
	auto strClean = GetIdsString(L"wndtooltipstr", L"clean");
	m_wndBtnTools.InsertBtn(cleanBtn, IDB_PNG_CLEAN, IDC_BUTTON_CLEAN, strClean==_T("0")? L"日常清洗" : strClean.c_str());
	CPngButton strongCleanBtn;//创建按钮
	auto strStrongClean = GetIdsString(L"wndtooltipstr", L"strongclean");
	m_wndBtnTools.InsertBtn(strongCleanBtn, IDB_PNG_STRONGCLEAN, IDC_BUTTON_STRONGCLEAN, strStrongClean== _T("0") ? L"强化清洗" : strStrongClean.c_str());
	m_wndBtnTools.InitRes();


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
	/*CList<UINT, UINT> lstBasicCommands;
	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_MENU_SET);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	CMFCToolBar::SetBasicCommands(lstBasicCommands);*/

	// 加载菜单
	menuLoad();
	// 出啊石化细胞视图
	InitShowCellView();
	SetUIPosBeforeShutdown();
	CMyDelegate1<CMainFrame*, void (CMainFrame::*)(void*, void*), void*> *pDelegate = nullptr;
	for (int i = 0; i < sizeof(m_lcEventHandler) / sizeof(MyEventHanlder); i++)
	{
		pDelegate = new CMyDelegate1<CMainFrame*, void (CMainFrame::*)(void*, void*), void*>;
		pDelegate->RegisterEventHandler(this, m_lcEventHandler[i].fun, m_lcEventHandler[i].arg);
		theApp.m_clsMsgDipatcher.AddEventHandler(m_lcEventHandler[i].eventType, pDelegate);
	}

	hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	//hReceiveEvent = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SystemMsgThread,this,0,&m_dwThreadId);
		
	// 定时检查软件所在磁盘空间
	SetTimer(0xFF, 500, NULL);
	// 定时清理图片
	SetTimer(0xFE, 1000, NULL);

	//SetTimer(5,300,NULL);	
    SetTimer(CLEAN_TIMER_1, 30000, NULL); // 定时清洗1
    // 定时清洗2
    {
        char confPath[256] = { 0 };
        GetModuleFileNameA(NULL, confPath, MAX_PATH);
        PathRemoveFileSpecA(confPath);
        PathAppendA(confPath, "Config\\setting.ini");

        BOOL isOn = (BOOL)GetPrivateProfileIntA(("ForceCleanTime"), ("clean2_on"), 0, confPath);
        if (isOn)
        {
            UINT internelTime = GetPrivateProfileIntA(("ForceCleanTime"), ("clean2_internel"), 60, confPath);
            if (internelTime > 0)
            {
                // 单位分钟
                SetTimer(CLEAN_TIMER_2, internelTime * 60 * 1000, NULL);
            }
        }
    }

	nBeginTime = GetTickCount();
    DBG_MSG("before StartUW2KFramework\n");
	// 启动天海核心软件框架
	if (theApp.StartUW2KFramework() == FALSE)
	{
		ExitProcess(0);
	}
    DBG_MSG("after StartUW2KFramework\n");
	// 重启下位机
	//m_startDev = RestartLC();
	theApp.m_startDev = UCRestart();
    DBG_MSG("after UCRestart\n");
	if (!theApp.m_startDev)
	{
		AfxMessageBox(GetIdsString(_T("timer"), _T("1")).c_str(), MB_OK|MB_ICONWARNING);
	}

	m_OnTimeSendLisEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	lisHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendLisInQueue, this, 0, NULL);
	if (lisHandle == NULL)
	{
		DBG_MSG("创建发送LIS线程失败");
		return -1;
	}
	SetTimer(10,theApp.m_sendLisTimeInterval, NULL);
	DBG_MSG("debug:SetTimer(10,%d)", theApp.m_sendLisTimeInterval);

	// ---------------------
	m_hCheckCardEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hExitCheckCardEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	auto f = std::async(std::launch::async, [=](){
		bool isExpired = false;
		int remainTimes;
		HANDLE handle[] = { this->m_hCheckCardEvent, this->m_hExitCheckCardEvent };
		while (1)
		{
			DWORD dwEvent = WaitForMultipleObjects(2, handle, FALSE, INFINITE);
			if (dwEvent == WAIT_OBJECT_0 + 0)
			{
				theApp.QueryConsumables(1, remainTimes, isExpired, m_hWnd);
				NotifyConsumables(1);
				this->m_bCheckCunsumablesSuccess = true;
			}
			else if (dwEvent == WAIT_OBJECT_0 + 1)
			{
				// 退出循环,退出线程
				break;
			}
		}
	});

    SetTimer(STRONG_CLEAN_TIMER, 5 * 1000, NULL);


   /* auto f2 = std::async(std::launch::async, [=](){
        Sleep(10000);
        char buf[7] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36 };
        NEW_TASK_CONTEXT t;
        t.HoleNumber = 1;
        t.BarcodeLen = 7;
        memcpy(t.Barcode, buf, 7);
        OnRecvLcNewTask(&t, 0);
    });*/

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
		| WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU ;

// 	cs.cx = GetSystemMetrics(SM_CXSCREEN);
// 	cs.cy = GetSystemMetrics(SM_CYSCREEN);

	cs.style &= ~WS_VSCROLL;//去掉垂直滚动条
	cs.style &= ~WS_HSCROLL;//去掉水平滚动条
/*	cs.style |= WS_HSCROLL | WS_VSCROLL;*/
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

	delete pDlgCust;

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

	return 0;
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
    
	// 管理、设置。（菜单项）
	if(pMenu)
	{
		pMenu->ModifyMenu(ID_USER_LOGIN, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_user"),_T("gl")).c_str());
		pMenu->ModifyMenu(ID_USER_PASSWORD, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_user"),_T("sz")).c_str());

// 		pMenu->DeleteMenu(ID_MENU_MEM, MF_BYCOMMAND);
// 
//  		pMenu->DeleteMenu(ID_MENU_SET, MF_BYCOMMAND);

// 		pMenu->EnableMenuItem(IDC_MENU_UDCITEM, MF_BYCOMMAND | MF_ENABLED);
// 		pMenu->EnableMenuItem(IDC_MENU_UDCSET, MF_BYCOMMAND | MF_ENABLED);
// 		pMenu->EnableMenuItem(IDC_MENU_USITEM, MF_BYCOMMAND | MF_ENABLED);
// 		pMenu->EnableMenuItem(IDC_MENU_USSET, MF_BYCOMMAND | MF_ENABLED);
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
	CMenu * pMenu2 = top.GetSubMenu(1);

	// 设备调试、设备参数、显微镜设置、暂停测试、继续测试。（菜单项）
	if(pMenu2)
	{
		//pMenu2->ModifyMenu(ID_EQUIPMENT_DBG,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("dbg")).c_str());
		//pMenu2->ModifyMenu(ID_EQUIPMENT_PARA,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("para")).c_str());
		//pMenu2->ModifyMenu(ID_US_QC,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("usqc")).c_str());
		//pMenu2->ModifyMenu(ID_UDC_QC,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("udcqc")).c_str());
		//pMenu2->ModifyMenu(ID_DEVICE_CLEAR,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("clear")).c_str());
		//pMenu2->ModifyMenu(ID_DEVICE_CLEAR_EX,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("clearex")).c_str());
		//pMenu2->ModifyMenu(ID_US_MIC,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("mic")).c_str());
		//pMenu2->ModifyMenu(ID_US_MIC_CONFIG,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("micconfig")).c_str());
		//pMenu2->ModifyMenu(ID_END_TEST,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("end")).c_str());
		//pMenu2->ModifyMenu(ID_START_TEST,MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_debug"),_T("start")).c_str());
		pMenu2->DeleteMenu(ID_AUTO_FOCUS, MF_BYCOMMAND); // 删除自动聚焦菜单项
		//pMenu2->ModifyMenu(ID_AUTO_FOCUS, MF_STRING |MF_BYCOMMAND, ID_AUTO_FOCUS, GetIdsString(_T("menu_debug"), _T("autofocus")).c_str());
		//pMenu2->ModifyMenu(ID_USER_MANAGEMENT, MF_STRING |MF_BYCOMMAND, NULL,GetIdsString(_T("menu_user","manage").c_str());

		pMenu2->EnableMenuItem(ID_US1_MIC_CONFIG, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pMenu2->EnableMenuItem(ID_END1_TEST, MF_BYCOMMAND | MF_ENABLED);
		pMenu2->EnableMenuItem(ID_START1_TEST, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

		// 权限控制，普通用户不允许调试
		if (theApp.loginGroup != 111 && theApp.loginGroup != 4)
		{
			pMenu2->DeleteMenu(ID_EQUIPMENT1_DBG, MF_BYCOMMAND);
		}

		pMenu2->DeleteMenu(ID_US1_MIC_CONFIG, MF_BYCOMMAND);
		pMenu2->DeleteMenu(ID_END1_TEST, MF_BYCOMMAND);
		pMenu2->DeleteMenu(ID_START1_TEST, MF_BYCOMMAND);
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

	if (!m_wndTodayList.Create((const TCHAR*)GetIdsString(_T("list_today"),_T("title")).c_str(), this, CRect(0, 0, 450, 200), TRUE, ID_VIEW_TODAY,
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
#if RESULT_DOCK_LEFT
	if (!m_wndResult.Create((const TCHAR*)GetIdsString(_T("wndret"), _T("title")).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_RESULT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建检测结果窗口\n"));
		return FALSE; // 未能创建
	}
#else
	if (!m_wndResult.Create((const TCHAR*)GetIdsString(_T("wndret"), _T("title")).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_RESULT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建检测结果窗口\n"));
		return FALSE; // 未能创建
	}
#endif
	
	// 创建输出窗口
	if (!m_wndOutPut.Create(_T("输出窗口"), this, CRect(0, 0, 200, 15), FALSE, ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_ALIGN_BOTTOM | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建信息输出窗口\n"));
		return FALSE; // 未能创建
	}
	// 创建查找窗口
// 	if (!m_wndFindRecord.Create(GetIdsString(_T("findrecord"),_T("title")).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FINDRECORD,
// 		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_ALIGN_BOTTOM | CBRS_FLOAT_MULTI))
// 	{
// 		OutputDebugString(_T("未能创建查询记录窗口\n"));
// 		return FALSE; // 未能创建
// 	}

	/*LOGFONT lf = { 0 };
	m_wndResult.GetFont()->GetLogFont(&lf);
	lf.lfHeight = -16;
	const auto hFont = ::CreateFontIndirect(&lf);
	m_wndResult.SetFont(CFont::FromHandle(hFont));*/
	NONCLIENTMETRICS ncm{ 0 };
	ncm.cbSize = sizeof(ncm);
	::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	DBG_MSG("SystemParametersInfoW default font size: %s %d, %d\n", WstringToString(ncm.lfMessageFont.lfFaceName).c_str(), ncm.lfMessageFont.lfHeight, ncm.lfMessageFont.lfWidth);
	ncm.lfMessageFont.lfHeight = -16;
	//wcscpy(ncm.lfMessageFont.lfFaceName, L"Arial");
	const auto hFont = ::CreateFontIndirectW(&ncm.lfMessageFont);
	m_wndResult.SetFont(CFont::FromHandle(hFont));
	m_wndPatient.SetFont(CFont::FromHandle(hFont));


	CRect wndrect;
	GetWindowRect(wndrect);
	if (!m_wndBtnTools.Create(NULL,
		this,
		CRect(0, 0, wndrect.Width(), 20), 
		FALSE,
		ID_VIEW_TOOL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_ALIGN_TOP | CBRS_FLOAT_MULTI))
	{
		OutputDebugString(_T("未能创建工具栏窗口\n"));
		return FALSE; // 未能创建
	}
	
	return TRUE;

}

void CMainFrame::GetUIPosBeforeShutdown()
{
	char iniUIPath[MAX_PATH] = { 0 };
	char buff[64] = { 0 };
	char buffer[64] = { 0 };
	CRect wndRect;

	GetModuleFileNameA(NULL, iniUIPath, MAX_PATH);
	PathRemoveFileSpecA(iniUIPath);
	PathAppendA(iniUIPath, "Config\\UI.ini");
	m_wndTodayList.GetWindowRect(wndRect);

// 	for (int i = 0; i <= MAX_COLUMN;i++)
// 	{	
// 		WritePrivateProfileStringA("todaylist", itoa(i, buff, 10), itoa(m_wndTodayList.m_List.GetColumnWidth(i), buffer, 10), iniUIPath);
// 	}
	
	CMFCTabCtrl &TabCtrl = GetUWView()->GetTabControl();
	int nTab = TabCtrl.GetActiveTab();

	WritePrivateProfileStringA("main","tab",itoa(nTab,buff,10),iniUIPath);
	WritePrivateProfileStringA("main", "applook", itoa(theApp.m_nAppLook, buff, 10), iniUIPath);
	WritePrivateProfileStringA("todaylist", "width", itoa(wndRect.Width(), buff, 10), iniUIPath);

	m_wndResult.GetWindowRect(wndRect);
	WritePrivateProfileStringA("TestResult", "width", itoa(wndRect.Width(), buff, 10), iniUIPath);

}

void CMainFrame::SetUIPosBeforeShutdown()
{
	char iniUIPath[MAX_PATH] = { 0 };
	char buff[64] = { 0 };
	CRect wndRect;
	m_wndTodayList.GetWindowRect(wndRect);
	GetModuleFileNameA(NULL, iniUIPath, MAX_PATH);
	PathRemoveFileSpecA(iniUIPath);
	PathAppendA(iniUIPath, "Config\\UI.ini");
	char columnWidth[128] = {0};
	int widthTotal = 0;
	char isShow[64] = {0};
	CMFCTabCtrl &TabCtrl = GetUWView()->GetTabControl();
	int nTab = GetPrivateProfileIntA("main", "tab", 0, iniUIPath);
// 	TabCtrl.SetActiveTab(0);
// 	TabCtrl.SetActiveTab(nTab);

	for (int i = 0; i <= MAX_COLUMN; i++)
	{
		GetPrivateProfileStringA("todaylist", itoa(i + 100, buff, 10), "1", isShow, 64, iniUIPath);
		if (atoi(isShow))
		{
			GetPrivateProfileStringA("todaylist", itoa(i, buff, 10), "100", columnWidth, 128, iniUIPath);
			m_wndTodayList.m_List.SetColumnWidth(i, atoi(columnWidth));
			widthTotal += atoi(columnWidth);
		}
		else
		{
			m_wndTodayList.m_List.SetColumnWidth(i, 0);
		}
	}
	int todayWidthIni = GetPrivateProfileIntA("todaylist", "width", 450, iniUIPath);
	if (widthTotal < todayWidthIni)
	{
		todayWidthIni = widthTotal;
	}	
	::MoveWindow(m_wndTodayList.GetSafeHwnd(), wndRect.left, wndRect.top, todayWidthIni, wndRect.Height(),NULL);

	int resultWidthIni = GetPrivateProfileIntA("TestResult", "width", 450, iniUIPath);
	m_wndResult.GetWindowRect(wndRect);
	::MoveWindow(m_wndResult.GetSafeHwnd(), wndRect.left, wndRect.top, resultWidthIni, wndRect.Height(), NULL);

	int applook = GetPrivateProfileIntA("main", "applook", ID_VIEW_APPLOOK_WIN_2000, iniUIPath);
	OnApplicationLook(applook);
}

void CMainFrame::SendMessageToQcDlg()
{
	HWND qcHwnd = qcDlg->GetSafeHwnd();
	
	::PostMessage(qcHwnd,WM_USER_ADDTASKTOQC,(LPARAM)0, (WPARAM)0);
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

void CMainFrame::KillProcess(const TCHAR *pszProcessName)
{
	CProcessManager::CloseProcess(pszProcessName);
}

void CMainFrame::SetConsumables(int count)
{
	CString str;
	str.Format(_T("当前耗材剩余量: %d"), count);
	if (count < 50)
	{
		m_wndStatusBar.SetPaneTextColor(3, RGB(255, 0, 0));
	}
	else
	{
		m_wndStatusBar.SetPaneTextColor(3, RGB(0, 0, 0));
	}
	m_wndStatusBar.SetPaneText(3, str);
}

void CMainFrame::HandleTaskUnfinished()
{
	StatusEnum nUSStatus, nUDCStatus;
	int nIndexTotal = m_wndTodayList.GetIndexByTaskID(m_wndTodayList.totalId);
	int nIndexFirst = m_wndTodayList.GetIndexByTaskID(m_wndTodayList.todayFirstID);
	ULONG nStatus;
	DBG_MSG("复位前第一个标本：%d，共有%d个标本", nIndexFirst,nIndexTotal);
	if (nIndexTotal == -1 || nIndexFirst == -1 )
	{
		return;
	}
	if (m_wndTodayList.totalId)
	{
/*		m_wndTodayList.AutoSaveCurRecord();//自动保存当前记录*/
		for (int i = nIndexFirst; i <= nIndexTotal; i++)
		{
			nStatus = m_wndTodayList.GetItemStatus(i);
			theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
			DBG_MSG("复位前第%d个标本的测试状态为镜检：%d，干化：%d", i, nUSStatus, nUDCStatus);
			if (nUSStatus != FINISH && nUSStatus != CHECKED && nUSStatus != NOTHING && nUSStatus != RESETTEST)
			{
				nUSStatus = TESTFAIL;
			}
			if (nUDCStatus != FINISH && nUDCStatus != CHECKED && nUDCStatus != NOTHING && nUDCStatus != RESETTEST)
			{
				nUDCStatus = TESTFAIL;
			}
			nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
			int nID = m_wndTodayList.GetItemTaskID(i);
			Access_UpdateTaskStatus(nID, nStatus);
		}
	}

// 
// 	DBG_MSG("共有%d个标本测试，镜检测试进行到%d时退出id:%d", nIndexTotal, nCurUSItem, m_wndTodayList.curUSTaskId);
// 	if (m_wndTodayList.totalId != 0 && m_wndTodayList.curUSTaskId != 0 && m_wndTodayList.curUDCTaskId != 0)//将开始采图的标本号 到新建标本号全部判断一次再重启
// 	{
// 		m_wndTodayList.AutoSaveCurRecord();//自动保存当前记录
// 		int nCurItem = min(nCurUSItem, nCurUDCItem);
// 		for (int i = nCurItem; i <= nIndexTotal; i++)
// 		{
// 			nStatus = m_wndTodayList.GetItemStatus(i);
// 			theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
// 			DBG_MSG("第%d个标本的测试状态为镜检：%d，干化：%d", i, nUSStatus, nUDCStatus);
// 			if (nUSStatus != FINISH && nUSStatus != CHECKED && nUSStatus != NOTHING)
// 			{
// 				nUSStatus = TESTFAIL;
// 			}
// 			if (nUDCStatus != FINISH && nUDCStatus != CHECKED && nUDCStatus != NOTHING)
// 			{
// 				nUDCStatus = TESTFAIL;
// 			}
// 			nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
// 			int nID = m_wndTodayList.GetItemTaskID(i);
// 			Access_UpdateTaskStatus(nID, nStatus);
// 		}
// 	}
// 	else if (m_wndTodayList.totalId != 0 && m_wndTodayList.curUSTaskId == 0 && m_wndTodayList.curUDCTaskId != 0)//只做干化
// 	{
// 		m_wndTodayList.AutoSaveCurRecord();//自动保存当前记录
// 		for (int i = nCurUDCItem; i <= nIndexTotal; i++)
// 		{
// 			nStatus = m_wndTodayList.GetItemStatus(i);
// 			theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
// 			DBG_MSG("第%d个标本的测试状态为镜检：%d，干化：%d", i, nUSStatus, nUDCStatus);
// 			if (nUSStatus != FINISH && nUSStatus != CHECKED && nUSStatus != NOTHING)
// 			{
// 				nUSStatus = TESTFAIL;
// 			}
// 			if (nUDCStatus != FINISH && nUDCStatus != CHECKED && nUDCStatus != NOTHING)
// 			{
// 				nUDCStatus = TESTFAIL;
// 			}
// 			nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
// 			int nID = m_wndTodayList.GetItemTaskID(i);
// 			Access_UpdateTaskStatus(nID, nStatus);
// 		}
// 	}
// 	else if (m_wndTodayList.totalId != 0 && m_wndTodayList.curUDCTaskId == 0 && m_wndTodayList.curUSTaskId != 0)//只做镜检
// 	{
// 		m_wndTodayList.AutoSaveCurRecord();//自动保存当前记录
// 		for (int i = nCurUSItem; i <= nIndexTotal; i++)
// 		{
// 			nStatus = m_wndTodayList.GetItemStatus(i);
// 			theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
// 			DBG_MSG("第%d个标本的测试状态为镜检：%d，干化：%d", i, nUSStatus, nUDCStatus);
// 			if (nUSStatus != FINISH && nUSStatus != CHECKED && nUSStatus != NOTHING)
// 			{
// 				nUSStatus = TESTFAIL;
// 			}
// 			if (nUDCStatus != FINISH && nUDCStatus != CHECKED && nUDCStatus != NOTHING)
// 			{
// 				nUDCStatus = TESTFAIL;
// 			}
// 			nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
// 			int nID = m_wndTodayList.GetItemTaskID(i);
// 			Access_UpdateTaskStatus(nID, nStatus);
// 		}
// 	}
// 	if (m_wndTodayList.totalId != 0 && m_wndTodayList.curUSTaskId == 0 && m_wndTodayList.curUDCTaskId == 0)//当第一个标本就未完成时复位，m_wndTodayList.curUSTaskId == 0
// 	{
// 		for (int i = 0; i <= nIndexTotal; i++)
// 		{
// 			nStatus = m_wndTodayList.GetItemStatus(i);
// 			theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
// 			if (nUSStatus != FINISH && nUSStatus != CHECKED && nUSStatus != NOTHING)
// 			{
// 				nUSStatus = TESTFAIL;
// 			}
// 			if (nUDCStatus != FINISH && nUDCStatus != CHECKED && nUDCStatus != NOTHING)
// 			{
// 				nUDCStatus = TESTFAIL;
// 			}
// 			nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
// 			int nID = m_wndTodayList.GetItemTaskID(i);
// 			Access_UpdateTaskStatus(nID, nStatus);
// 		}
// 	 
// 	 }
// 	else//未开始新建
// 	{
// 	}
}

void CMainFrame::MicSet(bool bIsInSetting)
{
	if (bIsInSetting)
	{
		OnCaiTuConfig(ID_START1_TEST);
		OnCaiTuConfig(ID_US1_MIC_CONFIG);
	}
	else
	{
		OnCaiTuConfig(ID_US1_MIC_CONFIG);
		OnCaiTuConfig(ID_END1_TEST);
	}
}

int CMainFrame::GetTestType(int nHole, string barcode, std::map<string, string>& baseInfo)
{
	int tt = 0;
	//theApp.g_nSettingType 1按统一设置的类型测试，2按每一孔位独立设置的测试类型进行测试
	bool bTestTypebySet = true;

#if (_LC_V < 2250)
	tt = theApp.GetCheckType();
#else
	if (theApp.m_bIsLiushuixain)
	{
		if (!theApp.g_isForceUseSoftwareSettingTesttype)
		{
			if (!barcode.empty())
			{
				bTestTypebySet = false;
				// 流水线下，先向lis请求测试类型
				tt = GetTestTypeByLiusxianFromLis(barcode, baseInfo);
				if (tt <= 0 || tt >= 3)
				{
					bTestTypebySet = true;
				}
			}
            else
            {
                bTestTypebySet = true;
            }
		}
	}
	if (bTestTypebySet)
	{
		if (theApp.g_nSettingType == 1)
		{
			// 统一设置
			tt = theApp.GetCheckType();
		}
		else
		{
			// 按照每一孔位设置
			if (nHole < 1 || nHole > 11) nHole = 1;
			tt = theApp.g_vecHoleForSet[nHole - 1];
			DBG_MSG("Current Hole(%d),Hole TestType: %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d\n", nHole,
				theApp.g_vecHoleForSet[0], 
				theApp.g_vecHoleForSet[1], 
				theApp.g_vecHoleForSet[2], 
				theApp.g_vecHoleForSet[3], 
				theApp.g_vecHoleForSet[4], 
				theApp.g_vecHoleForSet[5], 
				theApp.g_vecHoleForSet[6], 
				theApp.g_vecHoleForSet[7], 
				theApp.g_vecHoleForSet[8], 
				theApp.g_vecHoleForSet[9], 
				theApp.g_vecHoleForSet[10]);
		}
	}
#endif
	

	return tt;
}

int CMainFrame::GetTestTypeByLiusxianFromLis(string barcode, std::map<string, string>& baseInfo)
{
	int ret = 0;
	MSG_PACKAGE_INFO info;
	char *pBuf = NULL;
	int nUtf8Size = GBKToUTF8(barcode.c_str(), &pBuf);
	std::shared_ptr<char> sp(pBuf, [&](char* p){SAFE_RELEASE_ARRAY(pBuf); });
	CreateBarcodeRequest(pBuf, nUtf8Size - 1, &info, theApp.m_nDeviceNo);
	char *pkg = new char[info.nPackageSize];
	if (pkg)
	{
		DBG_MSG("获取病人信息1  :%s\n", barcode.c_str());
		PackageToData(&info, pkg, info.nPackageSize);
		unsigned int sockClient = 0;
		if (!theApp.m_lsx.CreateTcpClient(theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port, sockClient))
		{
			DBG_MSG("获取病人信息，创建TCP客端失败![%s:%d]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port);
		}
		else
		{ 
			if (0 == theApp.m_lsx.SendData(sockClient, pkg, info.nPackageSize))
			{
				DBG_MSG("获取病人信息，发送数据失败![%s:%d]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port);
			}
			else
			{
				int code = theApp.m_lsx.Wait(sockClient, 5000); //5秒超时
				if (code == 0)
				{
					DBG_MSG("获取病人信息超时\n");
				}
				else if (code < 0)
				{
					DBG_MSG("获取病人信息Socket错误\n");
				}
				else
				{
					char recvBuf[2048] = { 0 };
					int len = recv(sockClient, recvBuf, 2048, 0);
					ret = theApp.m_lsx.ParseMsg(recvBuf, len, baseInfo);
				}
			}
		}
		if (sockClient!=0) closesocket(sockClient);

		delete[] pkg;
	}

		return ret;
}

void CMainFrame::OnClose()
{
	CString str = L"正在关机清洗";
	bool bClean = true, bForceClean = false;
	char focusIni[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, focusIni, MAX_PATH);
	PathRemoveFileSpecA(focusIni);
	PathAppendA(focusIni, "Config\\Focus.ini");

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	HandleTaskUnfinished();
	WritePrivateProfileStringA("Focus", "EnableQCRec", "0", focusIni);
	if (!theApp.m_startDev) // 如果下位机没有启动，则直不需要清洗
	{
		if (MessageBox(_T("是否退出程序？"), GetIdsString(_T("close"), _T("3")).c_str(), MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
		{
			return;
		}
		else
		{
			PostQuitMessage(0);
		}
	}
	DBG_MSG("debug:正在关机");

	
	if( theApp.IsIdleStatus() == FALSE )  //当前正在做任务...
	{
		MessageBox(GetIdsString(_T("close"),_T("2")).c_str(),GetIdsString(_T("close"),_T("3")).c_str(),MB_OK|MB_ICONWARNING);
		return;
	}
	
	if (MessageBox(_T("是否退出程序？"), GetIdsString(_T("close"), _T("3")).c_str(), MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
	{
		return;
	}
	CTaskMgr::DeleteExtraTasks();
	GetUIPosBeforeShutdown();
	HANDLE hTestThread;
	hTestThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvUCShutDownHandler, this, 0, NULL);
	if (hTestThread == NULL)
		return;

	m_bClose = TRUE;
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CloseThread, this, 0, NULL);//MFC UI是线程相关的.不能在线程调用其他线程得UI,会assert


// 	this->selfdlg.ShowModal(L"关机清洗");
// 	m_bClose = FALSE;

}

DWORD CMainFrame::RecvUCShutDownHandler(LPVOID pParam)
{
	TCHAR INIPath[256] = { 0 };

	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("conf.ini"));

	CMainFrame *pMain = (CMainFrame*)pParam;
	bool bClean = true, bForceClean = false;



	DisableAutoCheck();//关机时，必须先发此命令，再发清洗，才吸保养液

	if (true == GetPrivateProfileInt(L"UCShutdown", L"Clean", 0, INIPath))
	{
		Maintain();  //关机时清洗实际是吸保养液，放在强化清洗后
	}
	CloseHandle(pMain->m_hCheckCardEvent);
	if (pMain->m_hExitCheckCardEvent)
	{
		SetEvent(pMain->m_hExitCheckCardEvent);
		CloseHandle(pMain->m_hExitCheckCardEvent);
	}
	pMain->m_hCheckCardEvent = NULL;
	pMain->m_hExitCheckCardEvent = NULL;
	
    theApp.EnterDebugMode(true);

	
	pMain->m_wndTodayList.AutoSaveCurRecord();
	Access_CloseAccess();
	//DisableAutoCheck();




// 	CloseHandle(hTestThread);
// 	hTestThread = NULL;

	if( bForceClean == false )
	{
		DBG_MSG("强化清洗失败");
		//goto CloseClean_lc;
	}


/*	ForceClean();*/

	::SendMessage(pMain->selfdlg.m_hWnd, WM_CLOSE, 0, 0);
	pMain->KillProcess(_T("NY_DetectService.exe"));
	//selfdlg.OnOK();
	//SendMessage()

/*	if ( !bClean || !bForceClean )
	{
		AfxMessageBox(GetIdsString(_T("close"),_T("1")).c_str());
	}*/



		
	exit(0);
	system("TASKKILL / F / T / IM UW2000.exe");
	return 0;
}
	

DWORD CMainFrame::RecvUCShutDownHandlerUI(LPVOID pParam)
{
	CMainFrame *pMain = (CMainFrame*)pParam;
	pMain->selfdlg.ShowModal(L"关机清洗中");
	return 0;
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

LRESULT CMainFrame::ShowSelfDlgHandler(WPARAM w, LPARAM l)
{
	int t = (int)w;
	if (t == 0)
	{
		selfdlg.HideModal();
	}
	else
	{
		CString *str = (CString *)l;
		if (str)
		{
			selfdlg.ShowModal(*str);
			delete str;
		}
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
	CUdcView* udcView = (CUdcView*)GetUWView()->GetTabWnd(THUM_UDC_TAB);
	int index = udcView->GetIndexByID(task_info->main_info.nID);
	if (index != -1)
	{
		udcView->m_List.curSelectID = task_info->main_info.nID;
		udcView->m_List.EnsureVisible(index, FALSE);
		udcView->m_List.SetItemState(index, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
		udcView->SetFocus();
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

BOOL CMainFrame::NewTask(int nPos, int nCha, int nTestType, BOOL bTaskResult, int &newTaskId)
{
	int    nResetTestType = 0, nItemIndex = 0;
	LONG   nID = 0;

	//跨天后修改顺序号
	theApp.UpdateSNByDate();

	nID = m_wndTodayList.IsNoramlTestType(nResetTestType, nItemIndex);
	if( nID == -1 ) //正常测试
	{
		DBG_MSG("正常测试 nTestType = %d\n",nTestType);
		return NormalTestType(nPos, nCha, nTestType, bTaskResult, newTaskId);
	}
	else
	{
		newTaskId = nID;
	}
		
	DBG_MSG("复测 nResetTestType = %d\n", nResetTestType);
	return ResetTestType(nResetTestType, bTaskResult, nID, nItemIndex);
}

void CMainFrame::UpdataTaskState(int nID,StatusEnum nState)
{
    m_wndTodayList.SetStatusToFailed(nID,nState);
}

void CMainFrame::OutputMessage(TCHAR *pMsg)
{
	m_wndOutPut.ShowInfoNoDel(pMsg);
}

void CMainFrame::OnMenuMange(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.IsIdleStatus());
} 

void CMainFrame::OnMenuDbg( UINT nID )
{
    theApp.EnterDebugMode(true);
	char confPath[MAX_PATH] = { 0 };
	char udcOrder[64] = { 0 };
	switch (nID)
	{
	case ID_MENU_SET:
		setdlg.DoModal();
		break;
	case ID_MENU_MEM:
		setting.DoModal();
		break;
	case IDC_MENU_USITEM:
		StartUSCFGSetDlg(NULL);
		break;
	case IDC_MENU_USSET:
		StartCalibrate(theApp.m_b);
		break;
	case IDC_MENU_UDCITEM:
	{
		if (strcmp(theApp.udcOrder, "youyuan") == 0)
		{
			break;
		}
		StartUDCCFGSetDlg(NULL);
		break;
	}
	case IDC_MENU_UDCSET:
		CUdcCriterionDlg dlg;
		dlg.DoModal();
		break;
	}
    theApp.EnterDebugMode(false);
}

void CMainFrame::OnDevMange(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.IsIdleStatus());
}

void CMainFrame::OnDevDbg( UINT nID )
{
	DbgPassword dbgDlg;
	while (true)
	{
		if (IDOK == dbgDlg.DoModal())
		{
			if (dbgDlg.m_password != 123456 && dbgDlg.m_password != 2)
			{
				MessageBox(L"密码错误！", MB_OK);
			}
			else
				break;
		}
		else
		{
			return;
		}
	}

	
		CManageDevDlg dlg;
        theApp.EnterDebugMode(true);
		PipeStopAutoRun();
		
		//((CMicroscopeCameraView*)(GetUWView()->GetTabWnd(VIDEO_CY_VIEW_TAB)))->SetVideoPrview(FALSE);
		
		switch(nID)
		{
		case ID_SOFTWARE_PARA:
		{
			dlg.SetDlgMode(SOFTWARE);
			dlg.nCurPage = 0;
			dlg.DoModal();
		}
			break;
		case ID_EQUIPMENT1_DBG:
			{
				dlg.nCurPage = 0;
				dlg.DoModal();
			}
			break;
		case ID_EQUIPMENT1_PARA:
			{
				dlg.nCurPage = 6;
				dlg.DoModal();
			}
			break;
	
		}
		PipeBeginAutoRun();
        theApp.EnterDebugMode(false);

		//((CMicroscopeCameraView*)(GetUWView()->GetTabWnd(VIDEO_CY_VIEW_TAB)))->SetVideoPrview(TRUE);
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
	pdlg->selfdlg.ShowModal(str);	
	return 0;
}

void CMainFrame::ManageWarnning( CString Warning,ALARM_TYPE nType )
{
    static std::atomic<bool> exitSpeak = false;
    exitSpeak = false;

    if (theApp.m_bIsLiushuixain)
    {
        auto f = std::async(std::launch::async, [Warning]{
            if (FAILED(::CoInitializeEx(NULL, COINIT_MULTITHREADED)))
            {
                DBG_MSG("CoInitializeEx error");
                return;
            }
            CComPtr<ISpVoice>              pVoice;
            HRESULT hr = pVoice.CoCreateInstance(CLSID_SpVoice);
            if (FAILED(hr))
            {
                DBG_MSG("CoCreateInstance failed");
                return;
            }
            pVoice->SetVolume(100);//设置音量，范围是 0 -100
            pVoice->SetRate(-1);//设置速度，范围是 -10 - 10
            pVoice->SetSyncSpeakTimeout(300);
            while (1)
            {
                if (exitSpeak)
                {
                    break;
                }
                hr = pVoice->Speak(Warning, 0, NULL);
                if (FAILED(hr))
                {
                    DBG_MSG("Speak failed");
                }
            }
            ::CoUninitialize();
        });
    }

	if (IDOK == MessageBox(Warning,L"UW2000提示", MB_OK | MB_ICONWARNING))
	{		
		//TroubleShooting(nType);
        exitSpeak = true;
		if (isLCReset)
		{
			return;
		}
		AlarmProcessed(nType);
	}
    exitSpeak = true;
}

/************************************************
函数名:  GetUWView
函数描述:获得view指针
************************************************/
CUWView* CMainFrame::GetUWView()
{
	/*CUWView *pView = NULL;
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
	}*/
	return (CUWView*)m_TabView;
}

void CMainFrame::OnCaiTuUIMange(CCmdUI *pCmdUI)
{
	BOOL bEnable = TRUE;
	switch (pCmdUI->m_nID)
	{
	case ID_US1_MIC_CONFIG:
		break;
	case ID_END1_TEST:
		if (m_bPicPause)
		{
			bEnable = FALSE;
		}
		else
		{
			bEnable = TRUE;
		}
		break;
	case ID_START1_TEST:
		if (m_bPicPause)
		{
			bEnable = TRUE;
		}
		else
		{
			bEnable = FALSE;
		}
		break;
	default:
		break;
	}

	pCmdUI->Enable(bEnable);
}

void CMainFrame::OnCaiTuConfig( UINT nID )
{
	CMicroscopeCameraView *pCamera = (CMicroscopeCameraView *)GetUWView()->GetTabWnd(VIDEO_CY_VIEW_TAB);
	
	CMenu *pMenu = GetMenu();
	CMenu *pMenuSub = pMenu->GetSubMenu(1);

	switch(nID)
	{
	case ID_US1_MIC_CONFIG:
		{
			if (pCamera->StartUsMicConfig())
			{
				if (pMenuSub)
				{
					UINT state = pMenu->GetMenuState(ID_US1_MIC_CONFIG, MF_BYCOMMAND);
					pMenuSub->CheckMenuItem(ID_US1_MIC_CONFIG, ((state & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
				}
			}
		}
		break;
	case ID_END1_TEST:
		{
			DBG_MSG("暂停采图");
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
				DBG_MSG("重新开始采图");
				m_bPicPause = FALSE;
				pCamera->ControlUSSample(FALSE);
			}
		}
		break;
	case ID_US1_MIC: //US采图测试
		{
		
		}
		break;
	}
}

// 清洗or强制清洗
void CMainFrame::DoClean(bool bStrengthen,int cleanNum)
{
	bool bCleanSuccess = true;
	
	CString *str = nullptr;
	if (bStrengthen)
	{
		str = new CString(GetIdsString(_T("timer"), _T("4")).c_str());
	}
	else
	{
		str = new CString(GetIdsString(_T("timer"), _T("5")).c_str());
	}

	auto f = std::async(std::launch::async, [&, str, bStrengthen, cleanNum](CString *s, bool bStrong, int num){
		m_bClose = TRUE;
		PostMessage(WM_CTRL_TIP_DLG_MSG, 1, (LPARAM)s);
		DisableAutoCheck();
		bStrong ? ForceClean(num) : Clean(num);
		PipeBeginAutoRun();
		PostMessage(WM_CTRL_TIP_DLG_MSG, 0, 0);
		m_bClose = FALSE;
	}, str, bStrengthen, cleanNum);


	/*m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ForceCleanThread, this, 0, NULL);

	m_bClose = TRUE;
	ForceClean(bForceClean);
	::SendMessage(selfdlg.m_hWnd,WM_CLOSE, 0, 0);
	m_bClose = FALSE;
	CloseThreadAndEvent();*/
}
// 测试界面的 清洗or强制清洗 (注意：只能在空闲状态下执行，且排管是开启的)
void CMainFrame::TestWndDoClean(bool bStrengthen, int cleanNum)
{
	bool bCleanSuccess = true;

	CString *str = nullptr;
	if (bStrengthen)
	{
		str = new CString(GetIdsString(_T("timer"), _T("4")).c_str());
	}
	else
	{
		str = new CString(GetIdsString(_T("timer"), _T("5")).c_str());
	}

	if (theApp.IsIdleStatus())
	{
		auto f = std::async(std::launch::async, [&, str, bStrengthen, cleanNum](CString *s, bool bStrong, int num){
			m_bClose = TRUE;
			PostMessage(WM_CTRL_TIP_DLG_MSG, 1, (LPARAM)s);
			bStrong ? ForceClean(num) : Clean(num);
			PostMessage(WM_CTRL_TIP_DLG_MSG, 0, 0);
			m_bClose = FALSE;
		}, str, bStrengthen, cleanNum);
	}
	else
	{
		MessageBoxTimeout(GetSafeHwnd(), _T("只能在空闲状态下才能执行清洗操作!!!"), _T("消息"), MB_OK, 0, 3 * 1000);
	}
}

void CMainFrame::TestWndDoClean(bool bStrengthen, bool bDayltiClean, int strongCleanNum, int dailyCleanNum)
{
    bool bCleanSuccess = true;

    if (theApp.IsIdleStatus())
    {
        auto f = std::async(std::launch::async, [=](bool strong, bool daily, int num1, int num2){
            m_bClose = TRUE;
            CString *str = nullptr;
            if (strong && num1 > 0)
            {
                str = new CString(GetIdsString(_T("timer"), _T("4")).c_str());
                PostMessage(WM_CTRL_TIP_DLG_MSG, 1, (LPARAM)str);
                ForceClean(num1);
                PostMessage(WM_CTRL_TIP_DLG_MSG, 0, 0);
            }
            if (daily && num2 > 0)
            {
                str = new CString(GetIdsString(_T("timer"), _T("5")).c_str());
                PostMessage(WM_CTRL_TIP_DLG_MSG, 1, (LPARAM)str);
                Clean(num2);
                PostMessage(WM_CTRL_TIP_DLG_MSG, 0, 0);
            }
            
            m_bClose = FALSE;
        }, bStrengthen, bDayltiClean, strongCleanNum, dailyCleanNum);
    }
    else
    {
        MessageBoxTimeout(GetSafeHwnd(), _T("只能在空闲状态下才能执行清洗操作!!!"), _T("消息"), MB_OK, 0, 3 * 1000);
    }
}


void CMainFrame::TestWndDoClean(bool bStrengthen, bool bDayltiClean, int cleanNum)
{
	bool bCleanSuccess = true;

	CString *str1 = nullptr, *str2 = nullptr;
	if (bStrengthen)
	{
		str1 = new CString(GetIdsString(_T("timer"), _T("4")).c_str());
	}
	if (bDayltiClean)
	{
		str2 = new CString(GetIdsString(_T("timer"), _T("5")).c_str());
	}

	if (theApp.IsIdleStatus())
	{
		auto f = std::async(std::launch::async, [&, str1, str2, bStrengthen, bDayltiClean, cleanNum](CString *s1, CString *s2, bool bStrong, bool daylti, int num){
			m_bClose = TRUE;
			if (bStrong)
			{
				PostMessage(WM_CTRL_TIP_DLG_MSG, 1, (LPARAM)s1);
				ForceClean(num);
				PostMessage(WM_CTRL_TIP_DLG_MSG, 0, 0);
			}
			if (daylti)
			{
				if (bStrong) Sleep(2000);
				PostMessage(WM_CTRL_TIP_DLG_MSG, 1, (LPARAM)s2);
				Clean(num);
				PostMessage(WM_CTRL_TIP_DLG_MSG, 0, 0);
			}
			m_bClose = FALSE;
		}, str1, str2, bStrengthen, bDayltiClean, cleanNum);
	}
	else
	{
		MessageBoxTimeout(GetSafeHwnd(), _T("只能在空闲状态下才能执行清洗操作!!!"), _T("消息"), MB_OK, 0, 3 * 1000);
	}
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
	char buffer[64] = { 0 };
	SYSTEMTIME systemtime = {0};
	int Hour = 0, Minute = 0 ;
	int taskNumNeedClean ,todayRest = 0;
	if (CLEAN_TIMER_1 == nIDEvent)
	{
		char ConfigFile[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, ConfigFile, MAX_PATH);
		PathRemoveFileSpecA(ConfigFile);
		PathAppendA(ConfigFile, "Config\\setting.ini");

		GetLocalTime(&systemtime);
		if (systemtime.wHour == 0 && systemtime.wMinute >= 0 && systemtime.wMinute <= 1)
		{
			if (theApp.IsIdleStatus() == true)
			{
				m_wndTodayList.FillList();
			}
			m_HaveOntimeClean = false;//新的一天开始，还未进行定时清洗
		}

		if (GetPrivateProfileIntA("ForceCleanTime", "on", 1, ConfigFile))
		{
			Hour = GetPrivateProfileIntA("ForceCleanTime", "Hour", 23, ConfigFile);
			Minute = GetPrivateProfileIntA("ForceCleanTime", "Minute", 58, ConfigFile);
			taskNumNeedClean = GetPrivateProfileIntA("ForceCleanCount", "OneCleanTaskNum", 25, ConfigFile);
			todayRest = GetPrivateProfileIntA("ForceCleanCount", "TodayRest", 0, ConfigFile);
			int fixCleanCount = GetPrivateProfileIntA("ForceCleanCount", "fixCleanCount", 0, ConfigFile);
			UINT nCleanType = GetPrivateProfileIntA(("ForceCleanTime"), ("cleanType"), 0, ConfigFile);
			//DBG_MSG("ForceCleanTimeSet Hour:%d,Minute:%d\n",Hour,Minute);
			
			//DBG_MSG("NOWTime Hour:%d,Minute:%d\n",systemtime.wHour,systemtime.wMinute);
			//if( ( systemtime.wHour == 23 && systemtime.wMinute >= 58 ) || bNeedForceClean == true )
			if (!m_HaveOntimeClean)//还未进行定时清洗
			{
				if (((systemtime.wHour == Hour && systemtime.wMinute >= Minute) ||
					(systemtime.wHour > Hour)) && //超过当天预定时间
					(lastNormalTestTime.wHour != 0 || lastNormalTestTime.wMinute != 0) &&//并且本次开机做过标本
					((systemtime.wHour - lastNormalTestTime.wHour) >= 1 ||
					(systemtime.wMinute - lastNormalTestTime.wMinute) >= 30) &&
					theApp.IsIdleStatus() == true)//到了预定时间（23：50）,并且距离上一次正常测试已经超过半个小时，并且不处于测试状态
				{
					int cleanCount = CalculateCleanCount(taskNumNeedClean, &todayRest);
					if (cleanCount <= 0) cleanCount = 1;
					if (fixCleanCount > 0 && fixCleanCount < 100) cleanCount = fixCleanCount;
					WritePrivateProfileStringA("ForceCleanCount", "TodayRest", _itoa(todayRest, buffer, 10), ConfigFile);
                    DBG_MSG("OnTime1 Clean Num = %d", cleanCount);
					if (cleanCount > 0)
					{
						TestWndDoClean((nCleanType==0), cleanCount);//强化/日常清洗
						m_HaveOntimeClean = true;//当天已经定时清洗
					}
				}
			}
		}
	}
    else if (CLEAN_TIMER_2 == nIDEvent)
    {
        // 定时清洗2
        char confPath[256] = { 0 };
        GetModuleFileNameA(NULL, confPath, MAX_PATH);
        PathRemoveFileSpecA(confPath);
        PathAppendA(confPath, "Config\\setting.ini");

        UINT clean2_strongCleanTimes = GetPrivateProfileIntA(("ForceCleanTime"), ("clean2_strongCleanTimes"), 0, confPath);
        UINT clean2_dailyCleanTimes = GetPrivateProfileIntA(("ForceCleanTime"), ("clean2_dailyCleanTimes"), 0, confPath);
        UINT nCleanType = GetPrivateProfileIntA(("ForceCleanTime"), ("clean2_cleanType"), 3, confPath);
        if (theApp.IsIdleStatus())
        {
            TestWndDoClean(nCleanType == 1 || nCleanType == 3, nCleanType == 2 || nCleanType == 3, clean2_strongCleanTimes, clean2_dailyCleanTimes);
            DBG_MSG("OnTimer Clean2  %d times, %d times", clean2_strongCleanTimes, clean2_dailyCleanTimes);
        }
    }
    else if (nIDEvent == STRONG_CLEAN_TIMER)
    {
        // 为解决清洗液匹配问题
        // 空闲时间达到60秒，则进入判断强化清洗流程
        // 1.判断空闲时间是否达到60秒
        std::unique_lock<std::mutex> lock(theApp.g_schrduleStrongClean.lock);
        if (theApp.IsIdleStatus() == TRUE)
        {
            theApp.g_schrduleStrongClean.idleTimeSecond += 5;
            char confPath[256] = { 0 };
            GetModuleFileNameA(NULL, confPath, MAX_PATH);
            PathRemoveFileSpecA(confPath);
            PathAppendA(confPath, "Config\\setting.ini");
            int afterIdleTime = (int)GetPrivateProfileIntA("ForceCleanTime", "clean3_afterIdleTime", 60, confPath);
            if (theApp.g_schrduleStrongClean.idleTimeSecond >= afterIdleTime)
            {
                DBG_MSG("g_schrduleStrongClean: %d\n", time(0));
                KillTimer(nIDEvent);
                theApp.g_schrduleStrongClean.idleTimeSecond = 0;
                // 2.读取配置：每做多少个标本，强化清洗多少次？
                int strongCleanPerSamples = (int)GetPrivateProfileIntA("ForceCleanTime", "clean3_strongCleanPerSamples", 10, confPath);
                int strongCleanCount = (int)GetPrivateProfileIntA("ForceCleanTime", "clean3_strongCleanCount", 0, confPath);
                if (strongCleanPerSamples == 0) strongCleanPerSamples = 10;
                if (theApp.g_schrduleStrongClean.countSamples >= strongCleanPerSamples)
                {
                    int n = theApp.g_schrduleStrongClean.countSamples / strongCleanPerSamples;
                    theApp.g_schrduleStrongClean.countSamples -= (n*strongCleanPerSamples);
                    TestWndDoClean(true, n*strongCleanCount, 0);
                }
            }
        }
        else
        {
            theApp.g_schrduleStrongClean.idleTimeSecond = 0;
        }
    }
// 		if( (ForceCleantime.wYear != systemtime.wYear || ForceCleantime.wMonth != systemtime.wMonth || ForceCleantime.wDay != systemtime.wDay ) && nForceCleanCount > 0 )
// 			nForceCleanCount = 0;
// 		if( ( systemtime.wHour == Hour && systemtime.wMinute >= Minute ) || bNeedForceClean == true ) 
// 		{	
// 			if( nForceCleanCount == 0 && bNeedForceClean == false )
// 			{
// 				GetLocalTime(&ForceCleantime);
// 				//DBG_MSG("NeedDoForceCleanTime Hour:%d,Minute:%d\n",ForceCleantime.wHour,ForceCleantime.wMinute);
// 			}
// 			if( ForceCleantime.wYear == systemtime.wYear && ForceCleantime.wMonth == systemtime.wMonth && ForceCleantime.wDay == systemtime.wDay && nForceCleanCount > 0 ) 
// 				return;			
// 
// 			if( theApp.IsIdleStatus() == false || theApp.m_bAutoCheckFail == true || theApp.m_bInDebugMode == true || theApp.m_bIsInAutoFocus == true )
// 			{
// 				bNeedForceClean = true;
// 				return;
// 			}
// 
// 			DoClean();
// 			nForceCleanCount++;
// 			bNeedForceClean = false;
// 			
// 		}


	else if ( 2 == nIDEvent)
	{
		KillTimer(2);
		CString str = GetIdsString(_T("timer"),_T("2")).c_str();
		selfdlg.str = str;
		selfdlg.DoModal();
	}
	else if ( 3 == nIDEvent)
	{
		KillTimer(3);
		CString str= GetIdsString(_T("timer"),_T("4")).c_str();
		selfdlg.str = str;
		selfdlg.DoModal();
	}
	else if ( 5 == nIDEvent)
	{	
		ULONG timespan;
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
	else if (10 == nIDEvent)
	{
		if (!m_SendLisQueue.empty())
		{
			DBG_MSG("debug:即将发送发送%d",m_SendLisQueue.front());
			SetEvent(m_OnTimeSendLisEvent);
		}
	}
	else if (0xFF == nIDEvent)
	{
		static bool isSet = false;
		if (!isSet)
		{
			isSet = true;
			KillTimer(0xFF);
			SetTimer(0xFF, 30 * 1000, NULL);
		}
		TCHAR        BasePath[MAX_PATH];
		GetModuleFileName(NULL, BasePath, MAX_PATH);
		CString root = CString(BasePath).Left(2);
		CString unit1,unit2;
		unsigned _int64 i64FreeBytesToCaller;
		unsigned _int64 i64TotalBytes;
		unsigned _int64 i64FreeBytes;
		GetDiskFreeSpaceEx(
			root,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,  //磁盘总容量
			(PULARGE_INTEGER)&i64FreeBytes);   //磁盘剩余空间
		float pre = float(i64FreeBytesToCaller) / i64TotalBytes;
		double TotalBytes, FreeBytes;
		if (i64TotalBytes >= 2 ^ 30)
		{
			TotalBytes = (float)i64TotalBytes / 1024 / 1024 / 1024;
			unit1 = "GB";
		}
		else if (i64TotalBytes >= 2 ^ 20)
		{
			TotalBytes = (float)i64TotalBytes / 1024 / 1024;
			unit1 = "MB";
		}
		else if (i64TotalBytes >= 2 ^ 10)
		{
			TotalBytes = (float)i64TotalBytes / 1024;
			unit1 = "kB";
		}
		else
		{
			TotalBytes = i64TotalBytes;
			unit1 = "byte";
		}
		if (i64FreeBytesToCaller >= 2 ^ 30)
		{
			FreeBytes = (float)i64FreeBytesToCaller / 1024 / 1024 / 1024;
			unit2 = "GB";
		}
		else if (i64FreeBytesToCaller >= 2 ^ 20)
		{
			FreeBytes = (float)i64FreeBytesToCaller / 1024 / 1024;
			unit2 = "MB";
		}
		else if (i64FreeBytesToCaller >= 2 ^ 10)
		{
			FreeBytes = (float)i64FreeBytesToCaller / 1024;
			unit2 = "kB";
		}
		else
		{
			FreeBytes = i64FreeBytesToCaller;
			unit2 = "byte";
		}
		CString txt;
		txt.Format(_T("磁盘(%s) %0.1f%s 可用, 共%0.1f%s)"), root, FreeBytes, unit2, TotalBytes, unit1);
		m_wndStatusBar.SetPaneText(1, txt);
		auto textColor = RGB(255, 255, 255);
		auto bgColor = RGB(0, 0, 0);
		if (pre - 0.1f > 0.000001f)
		{
			
		}
		else if (pre - 0.05f > 0.000001f)
		{
			textColor = RGB(0, 0, 0);
			bgColor = RGB(255, 255, 0);
		}
		else
		{
			textColor = RGB(255, 255, 255);
			bgColor = RGB(255, 0, 0);
		}
		m_wndStatusBar.SetPaneTextColor(1, textColor);
		m_wndStatusBar.SetPaneBackgroundColor(1, bgColor);
	}
	else if (0xFE == nIDEvent)
	{
		static bool isSet = false;
		if (!isSet)
		{
			isSet = true;
			KillTimer(0xFE);
			SetTimer(0xFE, 60 * 60 * 1000, NULL);
		}
		auto f = std::async(std::launch::async, [&]{
			theApp.CleanTestImages();
		});
	}

	CFrameWndEx::OnTimer(nIDEvent);
}

DWORD WINAPI CMainFrame::CloseThread( LPVOID pParam )
{
	CMainFrame *pdlg = (CMainFrame*)pParam;
	pdlg->SendMessage(WM_UCSHUTDOWN, 0, 0);
	pdlg->CloseThreadAndEvent();
	return 0;
}


DWORD WINAPI CMainFrame::SendLisInQueue(LPVOID pParam)
{
	CMainFrame* pMain = (CMainFrame*)pParam;
	int TaskID = 0;
	while (true)
	{
		WaitForSingleObject(pMain->m_OnTimeSendLisEvent,INFINITE);
		EnterCriticalSection(&pMain->m_sendLisCS);
		TaskID = pMain->m_SendLisQueue.front();
		pMain->m_SendLisQueue.pop();
		LeaveCriticalSection(&pMain->m_sendLisCS);

		if (2020 == theApp.m_sendLisType && !theApp.m_bIsLiushuixain)
		{
			pMain->m_wndTodayList.SendLis(TaskID, 2020);
		}
		else if (2020 == theApp.m_sendLisType && theApp.m_bIsLiushuixain)
		{
			pMain->m_wndTodayList.SendLisLisShuiXian(TaskID, 2020);
		}
		else
		{
			pMain->m_wndTodayList.SendLis(TaskID);
		}
		ResetEvent(pMain->m_OnTimeSendLisEvent);

	}
	
}

LRESULT CMainFrame::SetClose(WPARAM wparam, LPARAM lparam)
{
	while(TRUE)
	{
		if (m_bClose)
		{
			CString str= GetIdsString(_T("timer"),_T("3")).c_str();
			selfdlg.str = str;
			selfdlg.ShowModal(str);
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
	pCmdUI->Enable(theApp.IsIdleStatus());
}

void CMainFrame::OnOperateDbg( UINT nID )
{
	char focusIni[MAX_PATH] = {0};


    int nClear = 0;
	if( theApp.IsIdleStatus() == FALSE )
		return;

    theApp.EnterDebugMode(true);
	switch(nID)
	{
	case ID_US_QC1:
	{
		if (!qcDlg)
		{
			qcDlg = new CQcDlg;
			qcDlg->Create(IDD_QC_DIALOG, this);
			qcDlg->ShowWindow(SW_NORMAL);

		}
		else if (qcDlg->IsWindowVisible())
		{
			//MessageBoxA(NULL, "存在的质控窗口尚未关闭", "提示", MB_OK);
			//return;
			qcDlg->ShowWindow(SW_MAXIMIZE);
		}
		else
		{
			qcDlg->ShowWindow(SW_NORMAL);
		}

		break;
	}
	case ID_DEVICE_CLEAR1:
		DoClean();
		SetEvent(m_hCheckCardEvent);
		break;
	case ID_DEVICE_CLEAR_EX1:
        //ForceClean(bflag);
		DoClean(true);
		break;
	}
    theApp.EnterDebugMode(false);
}

void CMainFrame::CloseThreadAndEvent( void )
{
	if (m_hThread)
	{
		// 等待线程结束
		if (WaitForSingleObject(m_hThread, 2000) == WAIT_OBJECT_0)
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

void CMainFrame::UpdateUDCView( PUDC_RESULT_CONTEXT udc_ctx )
{
	USHORT* pValue = (USHORT*)&udc_ctx->UdcColorBlockCtx;
	TASK_INFO   task_info = {0};

	task_info.main_info.nID = udc_ctx->TaskId;
	StatusEnum   nUSStatus, nUDCStatus;
	theApp.ParseStatusMaskCode(task_info.main_info.nState, nUSStatus, nUDCStatus);
	nUDCStatus = FINISH;
	task_info.main_info.nState = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);

	
	for( int i = 0 ; i < MAX_UDC_COUNTS ; i ++ )
	{
		task_info.udc_info.udc_node[i].udc_col = pValue[i];
		DBG_MSG("UDCResult%d: %d\n",i,task_info.udc_info.udc_node[i].udc_col);
	}

	::SendMessage(this->GetSafeHwnd(), WM_INSERT_UDC_DATA_MSG, (WPARAM)&task_info, (LPARAM)FALSE);
}

BOOL CMainFrame::NormalTestType(int nPos, int nCha, int nTestType, BOOL bTaskResult, int &TaskId)
{
	BOOL                         bRet;
	TASK_INFO                    task_info = {0};
	SYS_WORK_STATUS              sys_work = {0};
	PRESPOND_SAMPLE              res_sample = NULL;

	int nID = m_wndTodayList.Test(nPos, nCha, nTestType, bTaskResult, task_info);
	TaskId = nID;

	sys_work.nType = RespondSample;
	res_sample     = (PRESPOND_SAMPLE)sys_work.Reserved;

	res_sample->nTaskID = nID;
	res_sample->type = (CheckType)nTestType;
	if (bTaskResult)
	{
		bRet = TRUE;
		if (nID != -1)
		{
			theApp.SetTaskStatus(nID, nTestType);
			res_sample->nHN = nPos;
		}
		else
		{
			res_sample->nHN = 0xff; //表示失败
		}
	}
	else
	{
		UpdataTaskState(nID, TESTFAIL);
		bRet = FALSE;
	}

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
	res_sample->nTaskID = nID;

	if( bTaskResult == FALSE )
	{
		if( nResetTestType & UDC_TEST_TYPE )
			m_wndTodayList.UpdateFinishStatus(nID, FALSE, FALSE, NULL);
		if( nResetTestType & US_TEST_TYPE )
			m_wndTodayList.UpdateFinishStatus(nID, TRUE, FALSE, NULL);
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

int CMainFrame::CalculateCleanCount(int taskNumNeedClean, int* todayRest)
{
	div_t m_div = div(m_wndTodayList.m_List.GetItemCount() + *todayRest, taskNumNeedClean);
	*todayRest = m_div.rem;
	return m_div.quot;
}

LRESULT CMainFrame::OnSetPrintPathMsg( WPARAM wparam, LPARAM lparam )
{
	PUPDATE_PRINT_PATH1 ppp = (PUPDATE_PRINT_PATH1)wparam;
	auto ret = Access_SetPrintPath(ppp);
	if (ret == STATUS_SUCCESS)
	{
		m_wndTodayList.UpdateCurTaskPrintImages(ppp->nTaskID, ppp->PrintPath);
	}
	return (ret == STATUS_SUCCESS);
}

LRESULT CMainFrame::OnGetPrintPathMsg(WPARAM wparam, LPARAM lparam)
{
	PCHAR pPrintImages = (PCHAR)wparam;
	UINT id = (ULONG)lparam;
	TASK_INFO taskInfo;
	ULONG ret = Access_GetTaskInfo(id, USER_TYPE_INFO, &taskInfo);
	if (STATUS_SUCCESS == ret)
	{
		if (strlen(taskInfo.main_info.sImage) > 0)
		{
			sprintf_s(pPrintImages, 255, taskInfo.main_info.sImage);
		}
		else
		{
			char ini_path[MAX_PATH];
			strcpy_s(ini_path, theApp.m_szExeFolderA);
			PathAppendA(ini_path, "conf.ini");
			BOOL autoSetDefaultPrintPic = (BOOL)GetPrivateProfileIntA("print", "autoSetDefaultPrintPic", 0, ini_path);
			int autoSetDefaultPrintPicNum = (BOOL)GetPrivateProfileIntA("print", "autoSetDefaultPrintPicNum", 2, ini_path);
			if (autoSetDefaultPrintPicNum < 0 || autoSetDefaultPrintPicNum >8) autoSetDefaultPrintPicNum = 2;
			if (autoSetDefaultPrintPic && autoSetDefaultPrintPicNum > 0)
			{
				CRecTask rt;
				PTASK_REC_INFO tri = NULL;
				if (STATUS_SUCCESS == rt.GetRecGrapTaskInfoWhenFinish((PCHAR)taskInfo.main_info.sFolder, tri) && tri)
				{
					autoSetDefaultPrintPicNum = min(autoSetDefaultPrintPicNum, tri->tGrapCounter);
					char tmp[256] = { 0 };
					for (int i = 0; i < autoSetDefaultPrintPicNum; i++)
					{
						char buf[32] = { 0 };
						sprintf_s(buf, "%03d_%04d.jpg|",
							tri->gInfo[i].rGrapSize, tri->gInfo[i].rIndex);
						strcat_s(tmp, buf);
					}
					sprintf_s(taskInfo.main_info.sImage, sizeof(taskInfo.main_info.sImage), tmp);
					if (STATUS_SUCCESS == Access_UpdateTaskInfo(USER_TYPE_INFO, id, &taskInfo))
					{
						sprintf_s(pPrintImages, 255, taskInfo.main_info.sImage);
					}
					THInterface_FreeSysBuffer(tri);
				}
			}
		}
	}

	return (STATUS_SUCCESS == ret);
}

LRESULT CMainFrame::OnUpdateListPos(WPARAM wparam, LPARAM lparam)
{
	if (wparam == 1)
	{
		int count = m_wndTodayList.m_List.GetItemCount();
		if (count > 0)
		{
			m_wndTodayList.m_List.EnsureVisible(count - 1, FALSE);
		}
	}
	else if(wparam == 2)
	{
		((CUdcView*)(((CUWView*)m_TabView)->GetTabWnd(THUM_UDC_TAB)))->UpdateListView();
	}
	return 0;
}

BOOL CMainFrame::OnCloseMiniFrame(CPaneFrameWnd* pWnd)
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


void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	if (nItemID >= ID_Menu && nItemID <= ID_US_MIC_EXIT)
	{
		TCHAR str[MAX_PATH];
		GetMenuString(hSysMenu, nItemID, str, MAX_PATH, nFlags);
		TCHAR *p = _tcsstr(str, _T("("));
		if (p)
		{
			*p = 0;
		}
		m_wndStatusBar.SetPaneText(0, str);
		return;
	}
	CFrameWndEx::OnMenuSelect(nItemID, nFlags, hSysMenu);

	// TODO:  在此处添加消息处理程序代码
	}

LRESULT CMainFrame::OnSetConsumables(WPARAM wParam, LPARAM lParam)
{
	DBG_MSG("耗材剩余量:%d\n", (int)wParam);
	SetConsumables((int)wParam);
	return 0;
}

LRESULT CMainFrame::OnSetCleanTimer(WPARAM wp, LPARAM lp)
{
    KillTimer(CLEAN_TIMER_2);
    DBG_MSG("%s on:%d, t:%d", __FUNCTION__, wp, lp);
    BOOL timerOn = (BOOL)wp;
    if (timerOn && lp > 0)
    {
        SetTimer(CLEAN_TIMER_2, lp * 60 * 1000, NULL);
    }
    
    return 0;
}

LRESULT CMainFrame::OnUpdateTaskUI(WPARAM wp, LPARAM lp)
{
    m_wndTodayList.UpdateTaskToList(wp);
    if (lp)
    {
        TASK_INFO *task_info = (TASK_INFO*)lp;
        m_wndPatient.m_WndPropList.ShowData(*task_info);
        delete task_info;
    }
    return 0;
}

LRESULT CMainFrame::OnWsQcCtrl(WPARAM wp, LPARAM lp)
{
    LRESULT r = 0;
    bool qc = wp;
    int mode = (lp>>16);
    int item = (lp & 0xFF);
    
    if (theApp.IsIdleStatus() == FALSE)
        return 1;

    if (!qcDlg)
    {
        qcDlg = new CQcDlg;
        qcDlg->Create(IDD_QC_DIALOG, this);
        qcDlg->ShowWindow(SW_NORMAL);

    }
    else if (qcDlg->IsWindowVisible())
    {
        qcDlg->ShowWindow(SW_MAXIMIZE);
    }
    else
    {
        qcDlg->ShowWindow(SW_NORMAL);
    }

    if (!qcDlg->WsQcCtrl(qc, mode, item))
    {
        r = 2;
    }

    return r;
}

void CMainFrame::OnRecvLcRestarting(void *arg, void *cbarg)
{
	DBG_MSG("下位机开始重启");

	theApp.m_bAutoCheckFail = true;
	if (theApp.m_bIsInAutoFocus == true)
	{
		CAutoFocusMgr::CancelAutoFocus();
		theApp.m_bIsInAutoFocus = false;
	}
	theApp.ClearTaskStatus();
	/*	CloseThreadAndEvent();*/
	//下位机重启关闭报警弹窗
	HWND pWnd = NULL;
	do
	{
		isLCReset = true;
		pWnd = ::FindWindowEx(NULL,NULL, NULL, L"UW2000提示");
		if (pWnd)
		{
			::SendMessage((HWND)pWnd, WM_CLOSE, NULL, NULL);
			DBG_MSG("FindWindowEx(NULL, NULL, NULL, UW2000提示)");
		}
		/*		pWnd = pWnd->GetNextWindow();*/
	} while (pWnd);
/*	m_wndOutPut.Clear();*/
	isLCReset = false;
	//////////////////
	CString str = GetIdsString(_T("timer"), _T("2")).c_str();
	PostMessage(WM_LCRSTARTING, 0, (LPARAM)str.AllocSysString());
}

void CMainFrame::OnRecvLcRestartEnd(void *arg, void *cbarg)
{
	DBG_MSG("下位机重启完成");
	int hasError = 0;
	CString str;
	if (arg)
	{
		PLC_RESET_END_CONTEXT ctx = (PLC_RESET_END_CONTEXT)arg;
		bool *p = (bool*)ctx;
		int nResetFlagCount = RESET_FLAG_COUNT;
		for (int i = 0; i < nResetFlagCount; i++)
		{
			if (!p[i])
			{
				hasError = 1;
				CString index;
				index.Format(_T("%d"), i);
#if (_LC_V <= 2150)
				str = GetIdsString(_T("lc_reset_v1.5"), index.GetBuffer()).c_str();
#elif (_LC_V >= 2180)
				str = GetIdsString(_T("lc_reset_v1.8"), index.GetBuffer()).c_str();
#else
				str = GetIdsString(_T("lc_reset_v1.5"), index.GetBuffer()).c_str();
#endif
				
				str.Append(_T(": "));
				if (i == nResetFlagCount - 1)
				{
					str.Append(_T("清洗液空"));
				}
				else
				{
					str.Append(_T("未复位"));
				}
				//AfxMessageBox(str, MB_OK | MB_ICONERROR);
				OutputMessage(str.GetBuffer());
			}
		}
	}
	if (0 != hasError )
	{
		PipeStopAutoRun();
		::MessageBox(GetSafeHwnd(),_T("下位机初始化状态异常，已关闭排管自动推进:\n")+str,_T("提示"),MB_ICONWARNING|MB_OK);
	}
	else
	{
		PipeBeginAutoRun();
	}

/*	theApp.m_clsMsgDipatcher.Start();*/
	PostMessage(WM_LCRSTARTEND, (WPARAM)3); 
}

void CMainFrame::OnRecvDetectedPipeAndIn(void *arg, void *cbarg)
{
	PostMessage(WM_DETECTEDPIPEANDIN);
}

void CMainFrame::OnRecvPipeInSuccess(void *arg, void *cbarg)
{
	if (theApp.nAttribute == TestNormal || theApp.nAttribute == TestEP)
	{
		theApp.m_nTubeShelfNum++;
	}

	DBG_MSG("debug:下一架号为%d", theApp.m_nTubeShelfNum);
	if (theApp.m_nTubeShelfNum > MAX_ROW)
	{
		theApp.m_nTubeShelfNum = 0;
	}
	theApp.PipeIn();

	this->m_wndBtnTools.myView->m_combox.EnableWindow(FALSE);
	this->m_wndBtnTools.myView->m_comboxUdcItem.EnableWindow(FALSE);
	PostMessage(WM_PIPEINSUCCESS);
}

void CMainFrame::OnRecvPipeOutShelft(void *arg, void *cbarg)
{
	theApp.PipeOut();
	DBG_MSG("OnRecvPipeOutShelft 排管推出，修改测试类型不使能，等待检测完成");
	this->m_wndBtnTools.myView->m_combox.EnableWindow(TRUE);
	this->m_wndBtnTools.myView->m_comboxUdcItem.EnableWindow(TRUE);
	//记录最后一次正常测试的时间
	GetLocalTime(&lastNormalTestTime);
	PostMessage(WM_PIPEOUTSHELF);
}

void CMainFrame::OnRecvLcNewTask(void *arg, void *cbarg)
{
	// 新检测任务
    if (arg)
    {
        PNEW_TASK_CONTEXT ctx = (PNEW_TASK_CONTEXT)arg;
        std::map<string, string> baseInfo;
        int TestType = this->GetTestType(ctx->HoleNumber, (char*)ctx->Barcode, baseInfo);
        int newTaskId = 0;
        BOOL ret = NewTask(ctx->HoleNumber, 0, TestType, TRUE, newTaskId);
        if (!ret)
        {
            return;
        }
        if (theApp.m_bIsLiushuixain)
        {
            m_wndTodayList.UpdateBaseInfo(newTaskId, baseInfo);
            PostMessage(WM_UPDATE_TASKUI, newTaskId, 0);
        }
        if (ctx->BarcodeLen > 0 && strlen((const char*)ctx->Barcode) > 0)
        {
            BARCODE_REQ_CONTEXT bcCtx = { 0 };
            bcCtx.TaskId = newTaskId;
            bcCtx.BarcodeLen = ctx->BarcodeLen;
            strcpy_s(bcCtx.Barcode, (const char*)ctx->Barcode);
            m_wndTodayList.UpdateBarCode(&bcCtx);
        }
        int remaindtimers = 0;
        bool isExpired = false;
        if (!theApp.QueryConsumables(1, remaindtimers, isExpired, m_hWnd))
        {
            return;
        }
        //SetEvent(m_hCheckCardEvent); // 减少耗材次数
        DBG_MSG("New Task CheckType = %d\n", TestType);
        theApp.UpdateCheckType(); // 质控之后恢复测试类型
        //SendMessage(WM_LCNEWTASK, 0, (LPARAM)arg);

        std::unique_lock<std::mutex> lock(theApp.g_schrduleStrongClean.lock);
        theApp.g_schrduleStrongClean.countSamples++;
    }
}

void CMainFrame::OnRecvLcNewEmergencyTask(void *arg, void *cbarg)
{
	//if (arg)
	{
		//PNEW_TASK_CONTEXT ctx = (PNEW_TASK_CONTEXT)arg;
		std::map<string, string> baseInfo;
		int TestType = this->GetTestType(11, string(), baseInfo);
		int newTaskId = 0;
		BOOL ret = NewTask(11, 0, TestType, TRUE, newTaskId);
		if (!ret)
		{
			return;
		}
		/*if (theApp.m_bIsLiushuixain)
		{
			m_wndTodayList.UpdateBaseInfo(newTaskId, baseInfo);
		}
		if (ctx->BarcodeLen > 0 && strlen((const char*)ctx->Barcode) > 0)
		{
			BARCODE_REQ_CONTEXT bcCtx = { 0 };
			bcCtx.TaskId = newTaskId;
			bcCtx.BarcodeLen = ctx->BarcodeLen;
			strcpy_s(bcCtx.Barcode, (const char*)ctx->Barcode);
			m_wndTodayList.UpdateBarCode(&bcCtx);
		}*/
		int remaindtimers = 0;
		bool isExpired = false;
		if (!theApp.QueryConsumables(1, remaindtimers, isExpired, m_hWnd))
		{
			return;
		}
		//SetEvent(m_hCheckCardEvent); // 减少耗材次数
		DBG_MSG("New Emergency Task CheckType = %d\n", TestType);
		theApp.UpdateCheckType(); // 质控之后恢复测试类型
		//PostMessage(WM_LCNEWEMERGENCYTASK);
	}
}

void CMainFrame::OnRecvRespondTaskFail(void *arg, void *cbarg)
{
	SendMessage(WM_RESPONDTASKFAILED, 0, (LPARAM)arg);
}

void CMainFrame::OnRecvTaskActionFinish(void *arg, void *cbarg)
{
	PostMessage(WM_TASKACTIONFINISH);
}

void CMainFrame::OnRecvCaptureImageReqest(void *arg, void *cbarg)
{
	SendMessage(WM_CAPTUREIMAGEREQ, 0, (LPARAM)arg);
}

void CMainFrame::OnRecvBarcodeInfoRequest(void *arg, void *cbarg)
{
	if (arg)
	{
		//PBARCODE_INFORMATION ctx = (PBARCODE_INFORMATION)arg;
		PBARCODE_REQ_CONTEXT ptr = new BARCODE_REQ_CONTEXT;
		std::shared_ptr<BARCODE_REQ_CONTEXT> ctx(ptr);
		memcpy(ptr, arg,sizeof(BARCODE_REQ_CONTEXT));
		if (ctx->TaskId > 0 && ctx->BarcodeLen > 0 )
		{
			DBG_MSG("1.下位机发送条码的任务ID为%d,条码长度为%d,条码内容为%s", ctx->TaskId, ctx->BarcodeLen, ctx->Barcode);
			m_wndTodayList.UpdateBarCode(ptr);
			if (theApp.m_bIsLiushuixain)
			{
				auto f = std::async(std::launch::async, [=](std::shared_ptr<BARCODE_REQ_CONTEXT> brctx){
					// 异步向LIS服务器请求条码信息
					// 1.根据条码从审核平台获取病人信息
					MSG_PACKAGE_INFO info;
					char *pBuf = NULL;
					int nUtf8Size = GBKToUTF8(brctx->Barcode, &pBuf);
					std::shared_ptr<char> sp(pBuf, [&](char* p){SAFE_RELEASE_ARRAY(pBuf); });
					CreateBarcodeRequest(pBuf, nUtf8Size - 1, &info, theApp.m_nDeviceNo);
					char *pkg = new char[info.nPackageSize];
					if (pkg)
					{
						DBG_MSG("获取病人信息1  :%s\n", brctx->Barcode);
						PackageToData(&info, pkg, info.nPackageSize);
						if (0 != theApp.m_lsx.CreateAsyncTcpClient(
							theApp.m_stDevPipelineInfo.remote.host,
							theApp.m_stDevPipelineInfo.remote.port,
							this, brctx->TaskId, pkg, info.nPackageSize, this))
						{
							DBG_MSG("获取病人信息，创建TCP客端失败![%s:%d]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port);
						}
						DBG_MSG("获取病人信息2  :%s\n", brctx->Barcode);
					}
					//SAFE_RELEASE_ARRAY(pBuf);
				}, ctx);
			}
		}
	}
}

void CMainFrame::OnRecvUdcTestResult(void *arg, void *cbarg)
{
	char iniPath[256] = { 0 };
	GetModuleFileNameA(NULL, iniPath, MAX_PATH);
	PathRemoveFileSpecA(iniPath);
	PathAppendA(iniPath, "config\\setting.ini");
	int paperTip = GetPrivateProfileIntA("tip", "paper", 1, iniPath);
	if (arg)
	{
		PUDC_RESULT_CONTEXT ctx = (PUDC_RESULT_CONTEXT)arg;		
        bool hasExcption = (ctx->PaperStatus != PAPER_NORMAL); 
        if (hasExcption && paperTip)
		{
			char buff[64] = { 0 };
			char mutilByte[64] = { 0 };
			TCHAR tBuff[64] = { 0 };
			int nIndex = m_wndTodayList.GetIndexByTaskID(ctx->TaskId);
			CString nSn =  m_wndTodayList.m_List.GetItemText(nIndex,3);
			WideCharToMultiByte(CP_ACP, NULL, (LPCWSTR)nSn, -1, mutilByte, MAX_PATH, NULL, FALSE);
			
			DBG_MSG("debug:标本[%s]纸条状态为%d",mutilByte, ctx->PaperStatus);
			sprintf(buff,"标本[ %s ]进行尿干化测试时纸条状态异常！",mutilByte);
			CharToTchar(buff, tBuff);
			OutputMessage(tBuff);
			MessageBoxA(this->GetSafeHwnd(),buff, "警告", MB_OK | MB_ICONWARNING);

		}
        m_wndTodayList.UpdateFinishStatus(ctx->TaskId, FALSE, hasExcption ? FALSE:TRUE, ctx);
        if (hasExcption)
        {
            memset(&ctx->UdcColorBlockCtx, 0, sizeof(ctx->UdcColorBlockCtx));
        }
        UpdateUDCView(ctx);
        DBG_MSG("debug:OnRecvUdcTestResult send lis task id = %d", ctx->TaskId);
        StatusEnum   nUSStatus, nUDCStatus;
        int nIndex = m_wndTodayList.GetIndexByTaskID(ctx->TaskId), nStatus;
        if (nIndex != -1)
        {
            nStatus = m_wndTodayList.GetItemStatus(nIndex);
            theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
            if (nUSStatus != TESTFAIL && nUDCStatus != TESTFAIL)
            {
                AutoSendToLisQueue(this, ctx->TaskId);
            }
        }
        else
        {
            AutoSendToLisQueue(this, ctx->TaskId);
        }
	}
}

void CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

void CMainFrame::OnRecvSampleTaskFinish(void *arg, void *cbarg)
{
	if (arg)
	{
		PSAMPLING_INFORMATION   s_info = (PSAMPLING_INFORMATION)arg;
		m_wndTodayList.UpdateFinishStatus(s_info->nTaskID, TRUE, TRUE, (PVOID)&s_info->Coefficient);
		DBG_MSG("debug:OnRecvSampleTaskFinish send lis task id = %d", s_info->nTaskID);
        StatusEnum   nUSStatus, nUDCStatus;
        int nIndex = m_wndTodayList.GetIndexByTaskID(s_info->nTaskID), nStatus;
        if (nIndex != -1)
        {
            nStatus = m_wndTodayList.GetItemStatus(nIndex);
            theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
            if (nUSStatus != TESTFAIL && nUDCStatus != TESTFAIL)
            {
                AutoSendToLisQueue(this, s_info->nTaskID);
            }
        }
        else
        {
            AutoSendToLisQueue(this, s_info->nTaskID);
        }
		DBG_MSG("TaskFinish TaskID:%d\n", s_info->nTaskID);
	}
}

void CMainFrame::OnRecvLcAlarm(void *arg, void *cbarg)
{
	if (arg)
	{
		PHARDWARE_ALARM_CONTEXT ctx = (PHARDWARE_ALARM_CONTEXT)arg;
		PostMessage(WM_LCALARM, 0, (LPARAM)ctx->AlarmNumber);
	}
}

void CMainFrame::OnRecvSamplingProgress(void *arg, void *cbarg)
{
	if (arg)
	{
		PSAMPLING_PROGRESS  sprg = (PSAMPLING_PROGRESS)arg;
		m_wndTodayList.ShowUSSampleProgress(sprg->nTaskID, sprg->nPos);
	}
}

void CMainFrame::OnRecvRecognitionProgress(void *arg, void *cbarg)
{
	if (arg)
	{
		PRECOGNITION_PROGRESS  rprg = (PRECOGNITION_PROGRESS)arg;
		m_wndTodayList.ShowUSRecognitionProgress(rprg->nTaskID, rprg->nPos);
	}
}

void CMainFrame::OnRecvTaskFocus(void *arg, void *cbarg)
{
	DBG_MSG("聚焦中。。。");
	SendMessage(WM_TASKFOCUS, 0, (LPARAM)arg);
}


void CMainFrame::OnRecvPhysicsResult(void *arg, void *cbarg)
{
	DBG_MSG("物理三项结果。。。");
	if (arg)
	{
		PPHYSICS_RESULT_CONTEXT ctx = (PPHYSICS_RESULT_CONTEXT)arg;
		DBG_MSG("物理三项结果。。。ID=%d, sg_test=%d", ctx->TaskId, ctx->sg_test);

		m_wndTodayList.UpdatePhysicsResult(ctx);
	}
}

LRESULT CMainFrame::RecvLcRestartingHandler(WPARAM w, LPARAM l)
{
	// 启动进度条UI显示

	DBG_MSG("UI进度条显示");
	BSTR b = (BSTR)l;
	CString s(b);
	SysFreeString(b);

	selfdlg.ShowModal(s);

	return 0;
}


LRESULT CMainFrame::RecvLcRestartEndHandler(WPARAM w, LPARAM l)
{
	m_wndOutPut.Clear();
	selfdlg.HideModal();
	
// 	if (w == 3)//仅需要关闭进度条
// 	{
// 		return 0;
// 	}



	// 根据复位结果，设置UI
	HandleTaskUnfinished();
	m_wndTodayList.FillList();
	if (w == 0)
	{
		// 没有错误
		theApp.m_bLCResetSuccess = true;
		theApp.UpdateCheckType();
	}
	else
	{
		// 有错误
		theApp.m_bLCResetSuccess = false;
	}
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	GETTESTINFO_CONTEXT ctx = { 0 };
	DBG_MSG("下位机开始启动，获取测试类型");

	if (GetDevTestInfo(ctx))
	{
		theApp.UpdateCheckType(ctx.nTestType);
		theApp.m_nUdcItemCount = ctx.nUdcTestItemCount;
		DBG_MSG("当前测试类型为 %d，纸条项目为%d",ctx.nTestType,ctx.nUdcTestItemCount);
		switch (ctx.nTestType)
		{
		case 1:pMain->m_wndBtnTools.myView->m_combox.SetCurSel(1);  break;
		case 2:pMain->m_wndBtnTools.myView->m_combox.SetCurSel(2); break;
		case 3:pMain->m_wndBtnTools.myView->m_combox.SetCurSel(0); break;
		default:
			break;
		}

		switch (ctx.nUdcTestItemCount)
		{
		case 10:pMain->m_wndBtnTools.myView->m_comboxUdcItem.SetCurSel(0); break;
		case 11:pMain->m_wndBtnTools.myView->m_comboxUdcItem.SetCurSel(1); break;
		case 12:pMain->m_wndBtnTools.myView->m_comboxUdcItem.SetCurSel(2); break;
		case 13:pMain->m_wndBtnTools.myView->m_comboxUdcItem.SetCurSel(3); break;
		case 14:pMain->m_wndBtnTools.myView->m_comboxUdcItem.SetCurSel(4); break;
		default:
			break;
		}
	}
	theApp.nAttribute = TestNormal;
	
	char focusIni[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, focusIni, MAX_PATH);
	PathRemoveFileSpecA(focusIni);
	PathAppendA(focusIni, "Config\\Focus.ini");
	WritePrivateProfileStringA("Focus", "EnableQCRec", "0", focusIni);

	this->m_wndBtnTools.myView->m_combox.EnableWindow(TRUE);
	this->m_wndBtnTools.myView->m_comboxUdcItem.EnableWindow(TRUE);
	this->m_wndBtnTools.myView->textQCTip.ShowWindow(FALSE);
	this->m_wndBtnTools.myView->ChangeUdcItem(theApp.m_nUdcItemCount);

	if (qcDlg && qcDlg->IsWindowVisible())
	{
		qcDlg->ShowWindow(SW_HIDE);
	}

	return 0;
}

LRESULT CMainFrame::RecvDetectedPipeAndInHandler(WPARAM w, LPARAM l)
{
	// 检测到排管，UI按钮变为不可用（菜单按钮）
	// not implement

    KillTimer(STRONG_CLEAN_TIMER);
    std::unique_lock<std::mutex> lock(theApp.g_schrduleStrongClean.lock);
    theApp.g_schrduleStrongClean.idleTimeSecond = 0;

	return 0;
}

LRESULT CMainFrame::RecvPipeInSuccessHandler(WPARAM w, LPARAM l)
{
	// 排管推进成功

	return 0;
}

LRESULT CMainFrame::RecvPipeOutShelftHandler(WPARAM w, LPARAM l)
{
	// 排管推出，UI按钮变为可用（菜单按钮）

    std::unique_lock<std::mutex> lock(theApp.g_schrduleStrongClean.lock);
    theApp.g_schrduleStrongClean.idleTimeSecond = 0;
    SetTimer(STRONG_CLEAN_TIMER, 5 * 1000, NULL);

	return 0;
}

LRESULT CMainFrame::RecvLcNewTaskHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvLcNewEmergencyTaskHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvRespondTaskFailHandler(WPARAM w, LPARAM l)
{
	PALLOW_TEST_CONTEXT ctx = (PALLOW_TEST_CONTEXT)l;
	m_wndTodayList.UpdateFinishStatus(ctx->TaskId, TRUE, FALSE, NULL);
	m_wndTodayList.UpdateFinishStatus(ctx->TaskId, FALSE, FALSE, NULL);

	return 0;
}

LRESULT CMainFrame::RecvTaskActionFinishHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvCaptureImageReqestHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvBarcodeTurbidityRequestHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvUdcTestResultHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvLcAlarmHandler(WPARAM w, LPARAM l)
{
	uint8_t nAlarmType = (uint8_t)l;
	CString index;
	index.Format(_T("%d"), nAlarmType);
#if (_LC_V <= 2150)
	CString str = GetIdsString(_T("lc_alarm_v1.5"), index.GetBuffer()).c_str();
	if (nAlarmType == 19)//显微镜Z轴复位失败
	{
		CMicroscopeCameraView* pAtiveView = ((CMainFrame*)theApp.GetMainWnd())->GetUWView()->m_pMicroscopeCamera;
		if (pAtiveView->noResetZMode == FALSE)
		{
			theApp.continueResetZ = true;
			ResetZAdjust(30);//删除队列中等待的 z轴复位，并重新添加队列
		}
	}
#elif(_LC_V >= 2260)
	CString str = GetIdsString(_T("lc_alarm_v2.6"), index.GetBuffer()).c_str();
	if (nAlarmType == 17)//显微镜Z轴复位失败
	{
		CMicroscopeCameraView* pAtiveView = ((CMainFrame*)theApp.GetMainWnd())->GetUWView
		()->m_pMicroscopeCamera;
		if (pAtiveView->noResetZMode == FALSE)
		{
			theApp.continueResetZ = true;
			ResetZAdjust(30);//删除队列中等待的 z轴复位，并重新添加队列
		}
	}
#else
	CString str = GetIdsString(_T("lc_alarm_v1.8"), index.GetBuffer()).c_str();
	if (nAlarmType == 17)//显微镜Z轴复位失败
	{
		CMicroscopeCameraView* pAtiveView = ((CMainFrame*)theApp.GetMainWnd())->GetUWView()->m_pMicroscopeCamera;
		if (pAtiveView->noResetZMode == FALSE)
		{
			theApp.continueResetZ = true;
			ResetZAdjust(30);//删除队列中等待的 z轴复位，并重新添加队列
		}
	}
#endif

    if (str == _T("0"))
    {
        str.Format(_T("未知错误(错误码：%d)"), nAlarmType);
    }
	OutputMessage(str.GetBuffer());
	ManageWarnning(str, (ALARM_TYPE)nAlarmType);

	return 0;
}

LRESULT CMainFrame::RecvSamplingProgressHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvRecognitionProgressHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvSampleTaskFinishHandler(WPARAM w, LPARAM l)
{
	return 0;
}

LRESULT CMainFrame::RecvTaskFocusHandler(WPARAM w, LPARAM l)
{
	PALLOW_TEST_CONTEXT ctx = (PALLOW_TEST_CONTEXT)l;
	m_wndTodayList.UpdateFocusStatus(ctx->TaskId, NULL);

	return 0;
}

LRESULT CMainFrame::RecvTaskIDFromUDC(WPARAM w, LPARAM l)
{
// 	int id = (int)w;
// 
// 	int index = m_wndTodayList.GetIndexByTaskID(id);
// 	if (index != -1)
// 	{
// 		m_wndTodayList.curSelectID = id;
// 		m_wndTodayList.m_List.EnsureVisible(index, FALSE);
// 		m_wndTodayList.m_List.SetItemState(index, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
// 		m_wndTodayList.SetFocus();
// 	}
	return 0;
}

void CMainFrame::OnSuccess(unsigned int s, unsigned long taskID, char *data, unsigned short len, void *userdata)
{
	DBG_MSG("OnSuccess[ID:%lu]\n", taskID);
	if (0 == theApp.m_lsx.PostData(s, data, len))
	{
		DBG_MSG("发送数据错误[ID:%lu]\n", taskID);
	}
	SAFE_RELEASE_ARRAY(data);
}

void CMainFrame::OnFail(unsigned long taskID, char *data, unsigned short len, int nOperationType, void *userdata)
{
	if (nOperationType == NIO_ERR_C2S_CONNECT)
	{
		DBG_MSG("连接服务器错误[%s:%u taskID:%lu]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port, taskID);
	}
}



void CMainFrame::OnSoftwarePara()
{
	// TODO:  在此添加命令处理程序代码
	
}
