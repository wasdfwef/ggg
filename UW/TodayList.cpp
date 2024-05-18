// TodayList.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "TodayList.h"
#include "Include/Common/String.h"
#include "MainFrm.h"
#include "..\..\..\inc\\THUIInterface.h"
#include "DbgMsg.h"
#include <string>
#include <future>
#include "ForceCanvas.h"
#include "DRBCImageView.h"
#include <Shlwapi.h>
#include "RecTask.h"
// CTodayList
#include "OpenCV2_1/include/cxcore.h"
#include "OpenCV2_1/include/highgui.h"
#include "OpenCV2_1/include/cv.h"

#pragma comment(lib, "OpenCV2_1/lib/highgui210.lib")
#pragma comment(lib, "OpenCV2_1/lib/cxcore210.lib")
#pragma comment(lib, "OpenCV2_1/lib/cv210.lib")

#define pi 3.1415926

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
std::vector<positiveID> positiveUDCID;
std::vector<positiveID> positiveUSID;

#define LIS_DATE_MAX_LEN                1024*8 
#define  MODEL_PATH "Model"
#define WM_NEXTBARCODE  WM_USER+0x1A

extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

IMPLEMENT_DYNAMIC(CTodayList, CDockablePane)

using namespace Common;



CTodayList::CTodayList()
{

	m_BillMode = FALSE;
	m_AutoSwitchTab = TRUE;
	memset(m_bAbnormity, 0, sizeof(m_bAbnormity));
	for(int i = 0; i < UDC_MAXITEM; i++)
	{
		m_strTestResult[i] = _T("");
		m_strUnit[i] = _T("");
	}
	m_CurSelIndex = -1;
	m_SaveCurRecord = 0;
	totalId = 0;
	todayFirstID = 0;
	
	char ConfigPath[MAX_PATH] = { 0 }; 
	GetModuleFileNameA(NULL, ConfigPath, MAX_PATH);
	PathRemoveFileSpecA(ConfigPath);
	PathAppendA(ConfigPath, "config//setting.ini");
	m_autoChangePrit =  GetPrivateProfileIntA("autochangeprit", "on", 0, ConfigPath);

	ZeroMemory(&m_CurTaskInfo, sizeof(m_CurTaskInfo));
	InitializeCriticalSection(&m_TaskInfo_CS);
	m_bEscEdit = FALSE;
}

CTodayList::~CTodayList()
{
}


BEGIN_MESSAGE_MAP(CTodayList, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_TODAY, &CTodayList::OnNMClick)
	ON_EN_KILLFOCUS(IDC_EDIT_BARCODE, &CTodayList::OnEditKillFocus)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TODAY, &CTodayList::OnNMRClick)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TODAY, &CTodayList::OnNMDClick)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_TODAY, &CTodayList::OnCustomList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TODAY, &CTodayList::OnItemChanged)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI_RANGE(ID_OPERATE_DELALL, ID_OPERATE_SEND_TO_LIS, &CTodayList::OnUpdateOperate)
	ON_COMMAND_RANGE(ID_OPERATE_DELALL, ID_OPERATE_SEND_TO_LIS, &CTodayList::OnOperate)
	ON_UPDATE_COMMAND_UI(ID_OPERATE_NOT_REVISE, &CTodayList::OnUpdateOperate)
	
	ON_MESSAGE(WM_NEXTBARCODE, OnNextBarcode)
	
	ON_COMMAND(ID_OPERATE_GETINFO, &CTodayList::OnOperateGetinfoFromLis)
	ON_COMMAND(ID_OPERATE_NOT_REVISE, &CTodayList::OnOperate)
END_MESSAGE_MAP()


int CTodayList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	m_Font.CreateFont(-8,4,0, 0, FW_NORMAL,	0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_List.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER| LVS_OWNERDRAWFIXED
		, CRect(0, 0, 360, 200), this, IDC_LIST_TODAY))
	{
		TRACE0("未能创建当天列表\n");
		return -1;      // 未能创建
	}

	if (!m_BarcodeEdit.Create(WS_CHILD | WS_VISIBLE
		, CRect(0, 0, 0, 0), this, IDC_EDIT_BARCODE))
	{
		TRACE0("未能创建条码编辑框\n");
		return -1;      // 未能创建
	}
	// 设置扩展属性
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

	// 设置图标
	//m_List.CreateSmallImage();

	// 初始化列表
	InitList();
    FillList();
	SetBillMode(FALSE);
    /*std::thread([&]{
        Sleep(10*1000);
        UpdateUSReslut(8918, 1.0f, true);
    }).detach();*/
    
/*	TestRecCircle();*/
	return 0;
}

void CTodayList::InitList( void )
{
	// 设置字体
	NONCLIENTMETRICS ncm{ 0 };
	ncm.cbSize = sizeof(ncm);
	::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	DBG_MSG("SystemParametersInfoW default font size: %s %d, %d\n", WstringToString(ncm.lfMessageFont.lfFaceName).c_str(), ncm.lfMessageFont.lfHeight, ncm.lfMessageFont.lfWidth);
	ncm.lfMessageFont.lfHeight = -16;
	wcscpy(ncm.lfMessageFont.lfFaceName, L"Arial");
	const auto hFont = ::CreateFontIndirectW(&ncm.lfMessageFont);
	m_List.SetFont(CFont::FromHandle(hFont));
	//SetStatusToFailed();

	// 初始化列表
	//if (m_pConfig)
	//{
		//m_List.InitList(m_pConfig, _T("list_today"));

		/*m_Rs.Open(m_szCurrentSql,
			theApp.m_pConn->GetInterfacePtr(),
			ADODB::adOpenKeyset,
			ADODB::adLockOptimistic,
			ADODB::adCmdText);*/
		//m_List.FillData(&m_Rs);
	//}

	m_List.SetListColor(RGB(223, 223, 223), RGB(245, 245, 245));
	m_List.InsertColumn(0,GetIdsString(_T("list_today"),_T("nid")).c_str(),0,0);
	m_List.InsertColumn(1,GetIdsString(_T("list_today"),_T("nstate1")).c_str(),0,106);
	m_List.InsertColumn(2,GetIdsString(_T("list_today"),_T("nstate2")).c_str(),0,66);
	m_List.InsertColumn(3,GetIdsString(_T("list_today"),_T("nsn")).c_str(),0,50);
	m_List.InsertColumn(4,GetIdsString(_T("list_today"),_T("nattr")).c_str(),0,50);
	m_List.InsertColumn(5,GetIdsString(_T("list_today"),_T("nrow")).c_str(),0,40);
	m_List.InsertColumn(6,GetIdsString(_T("list_today"),_T("npos")).c_str(),0,40);
	m_List.InsertColumn(7, GetIdsString(_T("list_today"), _T("barcode")).c_str(), 0, 100);//条码
/*	m_List.InsertColumn(7,GetIdsString(_T("list_today"),_T("sname")).c_str(),0,0);//姓名*/
	m_List.InsertColumn(8,GetIdsString(_T("list_today"),_T("lisstatus")).c_str(),0,70);
	m_List.InsertColumn(9,GetIdsString(_T("list_today"),_T("printstatus")).c_str(),0,70);
	m_List.InsertColumn(10,GetIdsString(_T("list_today"),_T("dtDate")).c_str(),0,105);
	m_List.InsertColumn(11,GetIdsString(_T("list_today"),_T("nalarm")).c_str(),0,0);
	m_List.InsertColumn(12, _T("阳性"), 0,0);
	
/*	m_List.SetColomnLockMask(7, FALSE);*/
	m_List.SetColomnLockMask(12, FALSE);

	//m_static_list_info_id.SetWindowText(GetIdsString(_T("list_info","normal_col1").c_str());
	//m_static_list_info_time.SetWindowText(GetIdsString(_T("list_info","normal_col7").c_str());
	//m_static_list_info_status.SetWindowText(GetIdsString(_T("list_info","normal_col2").c_str());
	//m_static_list_comment.SetWindowText(GetIdsString(_T("list_info","comment").c_str());

	//m_static_list_qc_type.SetWindowText(GetIdsString(_T("list_info","qc_col1").c_str());
	////		m_static_list_qc_max.SetWindowText(GetIdsString(_T("list_info","qc_max").c_str());
	////		m_static_list_qc_min.SetWindowText(GetIdsString(_T("list_info","qc_min").c_str());
	////		m_static_list_qc_targe.SetWindowText(GetIdsString(_T("list_info","qc_targe").c_str());
	//m_static_list_udc_res_info.SetWindowText(GetIdsString(_T("list_info","udc_info").c_str());
	//m_static_list_us_res_info.SetWindowText(GetIdsString(_T("list_info","us_info").c_str());
//	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
//	m_List.SetExtendedStyle(m_List.GetExtendedStyle()| LVS_EX_GRIDLINES);
	//m_List.ModifyStyle(0,LVS_SHOWSELALWAYS); 

//    m_List.SetItemHeight()


	m_BarcodeEdit.ShowWindow(SW_HIDE);
	   
}
void CTodayList::SimulationCell(void)
{
	std::map<int, int> RBCSize;
	for (int i = 0; i < 10000; i++) //模拟计算细胞大小
	{
		SYSTEMTIME k;
		GetSystemTime(&k);
		srand(k.wMilliseconds);
		rand();
		int a = rand() % 20 + 30;
		int b = rand() % 45 + 31;
		int c = rand() % 44 + 139;
		int d = rand() % 55 + 136;
		CRect rbcSize = { a, b, c, d };
		CountRBCSize(rbcSize, RBCSize);
	}
}

void CTodayList::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
// 	m_List.SetItemState(m_nSelItem, FALSE, LVIF_STATE);
// 	m_List.SetFocus();
}

void CTodayList::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_List.SetWindowPos (this, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	m_List.ShowWindow(TRUE);
}

LRESULT CTodayList::OnNextBarcode(WPARAM w, LPARAM l)
{
	DoSubItemEdit(w, l);

	return 0;
}

void CTodayList::FillList(void)
 {
	TOADY_TASK_INFO1  t_today_info;



	if ( Access_GetTodayTaskInfo(&t_today_info) != STATUS_SUCCESS )
		return;

	m_nlistNum    =  t_today_info.nToadyTaskCounter;


	for (int i = 0; i < m_nlistNum; ++i)
	{
		JudgeUSResult(t_today_info.pFullTaskInfo[i]);
		if (t_today_info.pFullTaskInfo[i].main_info.nAttribute == TestNormal || t_today_info.pFullTaskInfo[i].main_info.nAttribute == TestEP)
		{
			theApp.m_nSN = t_today_info.pFullTaskInfo[i].main_info.nSN + 1;
			theApp.m_nTubeShelfNum = t_today_info.pFullTaskInfo[i].main_info.nRow;
		}
	}
	
	if (theApp.m_nSN > MAX_SN)
	{
		theApp.m_nSN = 1;
	}

	if (theApp.m_nTubeShelfNum > MAX_ROW)
	{
		theApp.m_nTubeShelfNum = 0;
	}

	m_List.DeleteAllItems();
	m_List.SetRedraw(FALSE);

	for (int i = 0; i < m_nlistNum; ++i)
	{
		if (t_today_info.pFullTaskInfo[i].main_info.nAttribute != TestNormal && t_today_info.pFullTaskInfo[i].main_info.nAttribute != TestEP)
			continue;
		AddTaskToList(t_today_info.pFullTaskInfo[i]);
	}

	m_List.SetRedraw(TRUE);
	

	Access_FreeGetTodayTaskInfoBuf(t_today_info);
}

void CTodayList::OnNMRClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	

	CPoint point;
	GetCursorPos(&point);
	CMenu menu;
	menu.LoadMenuW(IDR_POPUP_OPERATE);
	CMenu * pMenu = menu.GetSubMenu(0);
	if (!theApp.m_bIsLiushuixain)
	{
		pMenu->DeleteMenu(ID_OPERATE_GETINFO, MF_BYCOMMAND);
	}
	TCHAR confPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, confPath, MAX_PATH);
	PathRemoveFileSpec(confPath);
	PathAppend(confPath, _T("conf.ini"));
	BOOL showdeleteopt = (BOOL)GetPrivateProfileInt(_T("menu"), _T("showdeleteopt"), 1, confPath);
	if (!showdeleteopt)
	{
		pMenu->DeleteMenu(ID_OPERATE_DELETE, MF_BYCOMMAND);
		pMenu->DeleteMenu(ID_OPERATE_DELALL, MF_BYCOMMAND);
	}
	theApp.GetContextMenuManager()->ShowPopupMenu(pMenu->GetSafeHmenu(), point.x, point.y, this, TRUE);
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_OPERATE, point.x, point.y, this, TRUE);
	if( QueryBillMode() == TRUE )
		AutoUpdateCheckStatus();

	*pResult = 0;
}


bool CTodayList::IsSelectTaskNotTodayChecked()
{
	int SelectIndex = GetOnlyOneSelected();
	if (-1 == SelectIndex)
	{
		return false;
	}

	int Year = 0;
	int Month = 0;
	int Day = 0;
	CString Value = m_List.GetItemText( SelectIndex, 10);
	_snwscanf(Value.GetBuffer(), Value.GetLength(), _T("%d-%d-%d"), &Year, &Month, &Day);
	

	SYSTEMTIME CurrentTime = {0};
	GetSystemTime(&CurrentTime);
	return (CurrentTime.wYear == Year && CurrentTime.wMonth == Month && CurrentTime.wDay == Day);
}

void CTodayList::OnUpdateOperate( CCmdUI *pCmdUI )
{

	BOOL bEnable = FALSE;
	int iSelCount = m_List.GetSelectedCount();

	bEnable = (iSelCount != 0);// && m_bCanLookInfo;

	if( bEnable == FALSE )
	{
		pCmdUI->Enable(bEnable);
		return;
	}

	switch (pCmdUI->m_nID)
	{
	case ID_OPERATE_REVISE:
	case ID_OPERATE_NOT_REVISE:
		break;
	
	case ID_OPERATE_US_RETEST:
	case ID_OPERATE_UDC_RETEST:
	case ID_OPERATE_RETEST:
		{
			bEnable = FALSE;
			if( iSelCount > 1 )
				break;

			bEnable = theApp.IsIdleStatus();
			if( bEnable == FALSE )
				break;

			bEnable = IsSelectTaskNotTodayChecked();
			if( bEnable == FALSE )
				break;

			break;
		}

	case ID_OPERATE_DELETE:
		{
			bEnable = !IsSelectedCurTask();
			break;
		}
	case ID_OPERATE_DELALL:
	default:
		break;
	
	}
	pCmdUI->Enable(bEnable);
}

void CTodayList::OnOperate( UINT nID )
{
	if( nID == ID_OPERATE_REVISE )
		AutoUpdateCheckStatus(FALSE);
	else if( nID == ID_OPERATE_DELETE )
		DeleteRecord();
	else if (nID == ID_OPERATE_DELALL)
		DeleteAllRecord();
	else if( nID == ID_OPERATE_PREVIEW )
		PrintReport(FALSE);
	else if( nID == ID_OPERATE_PRINT )
		PrintReport(TRUE);
	else if (nID == ID_OPERATE_US_RETEST)
		CmdResetType(US_TEST_TYPE);
	else if (nID == ID_OPERATE_UDC_RETEST)
		CmdResetType(UDC_TEST_TYPE);
	else if( nID == ID_OPERATE_RETEST )
		CmdResetType(ALL_TEST_TYPE);
	else if (nID == ID_OPERATE_SEND_TO_LIS)
		SendLis();
		
}

void CTodayList::Delete( void )
{
	int nItem;

	unsigned int nID = 0;
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	int nIDarry[]={0};
	int j = 0;
	while (pos)
	{
		nItem = m_List.GetNextSelectedItem(pos);
		nID = ::_ttol(m_List.GetItemText(nItem, ID_INDEX));	
		nIDarry[j] = nID;
		++j;
	}

	if (j!=0)
	{
		DEL_TASK *del_task = new DEL_TASK;
		if (del_task !=NULL)
		{
			delete del_task;
		}
		memcpy(del_task->pDelIDArray,nIDarry,sizeof(nIDarry));
		del_task->nCounter = j;
		del_task->bIsQC = FALSE;
		
		if (Access_DelTask(del_task) != STATUS_SUCCESS)
		{
			AfxMessageBox(GetIdsString(_T("list_today"),_T("state8")).c_str());
		}
	}
	// 排序
	//std::sort(viItem.begin(), viItem.end(), std::greater<int>());
	//// 删除
	//try
	//{
	//	for (Itr = viItem.begin(); Itr != viItem.end(); Itr++)
	//	{
	//		// 移动指针
	//		if (!m_Rs.IsBOF())
	//		{
	//			m_Rs.MoveFirst();
	//			m_Rs.Move(*Itr);
	//			m_Rs.Delete();
	//			m_Rs.Update();
	//		}
	//	}
	//}
	//catch (_com_error e)
	//{
	//	m_Error = _T("CTodayList::Delete()");
	//	m_Error += _T("\r\n");
	//	m_Error += (const TCHAR*) e.ErrorMessage();
	//	m_Error += _T("\r\n");
	//	m_Error += (const TCHAR*) e.Description();
	//	AfxMessageBox(m_Error.m_str.c_str());
	//}
	theApp.GetMainWnd()->PostMessage(WM_TODAYLIST_DELINFO, 0, 0);

}

int CTodayList::Test( int nPos, int nCha, int nTestType, BOOL bTaskResult,TASK_INFO &task_info)
{
	int nID = 0;
	int nTubeShelfNum = 0;
	StatusEnum   nUDCStatus, nUSStatus;
	COleDateTime dtDateTime;

	DBG_MSG("CTodayList::Test nTestType %d\n", nTestType);
	if((1 != nTestType) && (2 != nTestType) && (3 != nTestType))
	{
		nTestType = 3;
	}
	// 新建记录
	TASK_ADD taskNew = {0};
	ZeroMemory(&task_info, sizeof(task_info));

	theApp.GetStatusAtNew(nUSStatus, nUDCStatus, nTestType, bTaskResult);
	DBG_MSG("debug:新增时当前架号为%d", theApp.m_nTubeShelfNum);
	nTubeShelfNum = theApp.m_nTubeShelfNum;

	taskNew.nPos = nPos;
	taskNew.nCha = nCha;
	taskNew.nTestType = (TestTypeEnum)nTestType;
	taskNew.nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
	//taskNew.bIsQC = false;
	CMainFrame* pMain = (CMainFrame*)theApp.m_pMainWnd;
	if (theApp.nAttribute == TestNormal || theApp.nAttribute == TestEP)
	{
		taskNew.nNo = theApp.m_nSN++;
	}
	else
	{
		pMain->qcDlg->GetDlgItem(IDC_EDIT1)->UpdateData();
		taskNew.nNo = pMain->qcDlg->m_editSn;
	}

	
	CString strText;
	strText.Format(_T("%d"), theApp.m_nSN);
	pMain->m_wndBtnTools.myView->m_editNum.SetWindowTextW(strText);//序号更新到工具栏

	if (theApp.m_nSN > MAX_SN)
	{
		theApp.m_nSN = 1;
	}
	taskNew.nRow = nTubeShelfNum;
	taskNew.nTestDes = TestNormal; 
	if (theApp.nAttribute != TestNormal && theApp.nAttribute != TestEP)   //质控
	{
		//taskNew.nTestDes = theApp.nAttribute
        taskNew.nTestDes = theApp.GetNextQcType();
		theApp.isQC = theApp.nAttribute;
	}
	else
	{
		if (taskNew.nPos == 11)										//急诊
		{
			taskNew.nTestDes = TestEP;			
		}
	}
	taskNew.bLimitTaskTotalCount = (theApp.ReadSaveNums() > 0);
	taskNew.nLimitTaskTotalCount = theApp.ReadSaveNums();

	//if (taskNew.bIsQC)
	//{
	//	if (UDC_QC_TYPE == theApp.m_nTestQcType)
	//	{
	//		taskNew.nQcUdcType = type1;
	//	}
	//	else
	//	{
	//		taskNew.nQcUsType = type_n;
	//	}
	//}
	//else
// 		if( taskNew.nPos == 11 )  //急诊孔位号
// 			taskNew.nTestDes = TestEP;

	if (Access_NewTask(&taskNew) != STATUS_SUCCESS) //质控则测试类型为0，为啥我也不知道，还是别改了
		return -1;
	//Access_UpdateTaskStatus(taskNew.nID,taskNew.nStatus);
	TASK_INFO info = { 0 };
	if(STATUS_SUCCESS == Access_GetTaskInfo(taskNew.nID, USER_TYPE_INFO, &info))
	{
		TCHAR confPath[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, confPath, MAX_PATH);
		PathRemoveFileSpec(confPath);
		PathAppend(confPath, _T("conf.ini"));
		TCHAR chRetBuf[MAX_PATH] = { 0 }, defval[MAX_PATH] = { 0 };
		GetPrivateProfileString(_T("color"), _T("defval"), _T(""), defval, MAX_PATH, confPath);
		DWORD dwRet = GetPrivateProfileString(_T("color"), _T("name0"), _T(""), chRetBuf, MAX_PATH, confPath);
		std::wstring color(L"淡黄色");
		if (std::wstring(defval) == _T("null")) color.clear();
		else if (!std::wstring(defval).empty()) color = defval;
		if (dwRet > 0) color = chRetBuf;
		GetPrivateProfileString(_T("clear"), _T("defval"), _T(""), defval, MAX_PATH, confPath);
		dwRet = GetPrivateProfileString(_T("clear"), _T("name0"), _T(""), chRetBuf, MAX_PATH, confPath);
		std::wstring trans(L"透明");
		if (std::wstring(defval) == _T("null")) trans.clear();
		else if (!std::wstring(defval).empty()) trans = defval;
		if (dwRet > 0) trans = chRetBuf;
		
		if (!color.empty() || !trans.empty())
		{
			sprintf_s(info.main_info.sColor, sizeof(info.main_info.sColor), WstringToString(color).c_str());
			sprintf_s(info.main_info.sTransparency, sizeof(info.main_info.sColor), WstringToString(trans).c_str());
			Access_UpdateTaskInfo(USER_TYPE_INFO, info.main_info.nID, &info);
		}
	}
	AddTaskToList(taskNew.nID, task_info);
	if (totalId == 0)
	{
		todayFirstID = taskNew.nID;
	}
	totalId = taskNew.nID;//最大id.
	GetParent()->PostMessage(WM_TODAYLIST_UPDATELISTPOS, 1);

	
	return taskNew.nID;
}

void CTodayList::SetStatusToFailed(int nID,StatusEnum nState)
{
	Access_UpdateTaskStatus(nID,nState);
	//FillList();
}

int CTodayList::AddTaskToList(TASK_INFO &task_info)
{
	int           nIndex   = m_List.GetItemCount();
	CString       strValue =_T("");
	StatusEnum    nUSStatus,nUDCStatus;

	theApp.ParseStatusMaskCode(task_info.main_info.nState, nUSStatus, nUDCStatus);

	JudgeUSResult(task_info);
	m_List.InsertItem(nIndex, _T(""));
	strValue.Format(_T("%u"),task_info.main_info.nID);
	m_List.SetItemText(nIndex, 0, strValue);
// 	m_List.SetItemData(nIndex,task_info.main_info.nID);
	ShowStatus(nIndex, 1, nUSStatus);
	ShowStatus(nIndex, 2, nUDCStatus);
	strValue.Format(_T("%d"),task_info.main_info.nSN);
	m_List.SetItemText(nIndex,3,strValue);
	ShowAttribute(nIndex, task_info.main_info.nAttribute);
	strValue.Format(_T("%d"),task_info.main_info.nRow);
	m_List.SetItemText(nIndex,5,strValue);
	strValue.Format(_T("%d"),task_info.main_info.nPos);
	m_List.SetItemText(nIndex,6,strValue);
	//strValue.Format(_T("%s"), );
	m_List.SetItemText(nIndex, 7, (LPCTSTR)_bstr_t(task_info.main_info.sCode));
/*	m_List.SetItemText(nIndex,7,Common::CharToWChar(task_info.pat_info.sName).c_str());*/
	ShowPrintStatus(nIndex, task_info.main_info.dtPrintStatus);
	ShowLisStatus(nIndex, task_info.main_info.dtLisStatus);
	m_List.SetItemText(nIndex,10,Common::CharToWChar(task_info.main_info.dtDate).c_str());
	strValue.Format(_T("%d"),task_info.main_info.nAlarm);
	m_List.SetItemText(nIndex,11,strValue);
	if (IsPositiveResultTask(task_info))
	{
		m_List.SetItemText(nIndex, 12, _T("1"));
	}
	ResetItemMaskCode(nIndex, task_info.main_info.nID, task_info.main_info.nState);

	//int count = m_List.GetItemCount();
	//if (count > 0)
	//{
	//	m_List.EnsureVisible(count-1, TRUE);
	//}
	//GetParent()->PostMessage(WM_TODAYLIST_UPDATELISTPOS, 1);
	m_List.PostMessage(WM_VSCROLL, SB_BOTTOM);

	

	return nIndex;
}

int CTodayList::AddTaskToList( int nID, TASK_INFO &task_info)
{
	CMainFrame *p_Main = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (!p_Main)
	{
		return 0;
	}
	ZeroMemory(&task_info, sizeof(task_info));

	if( Access_GetTaskInfo(nID, ALL_TYPE_INFO, &task_info) != STATUS_SUCCESS ) //这里出错说明数据库存在bug
		return -1;
	//DBG_MSG("task_info.main_info.nTestType = %d\n",task_info.main_info.nTestType),

	
	if (theApp.nAttribute != TestNormal && theApp.nAttribute != TestEP)
	{
		HWND qcHwnd = p_Main->qcDlg->GetSafeHwnd();
		::PostMessage(qcHwnd, WM_USER_ADDTASKTOQC, (WPARAM)0, (LPARAM)&task_info);
	}
	else
	{
		AddTaskToList(task_info);
	}
	return 0;
}




#define NOT_SEND_LIS   0 
#define SEND_LIS	   1 
#define SECTION_SEND_LIS_STATUS     _T("listatus")
#define KEY_SEND					_T("send")
#define KEY_NOT_SEND				_T("notsend")

void  CTodayList::ShowLisStatus(int Index, int Status)
{
	CString strValue=_T("");

	if (Status == NOT_SEND_LIS)
	{
		strValue = GetIdsString(SECTION_SEND_LIS_STATUS, KEY_NOT_SEND).c_str();
	}
	else
	{
		strValue = GetIdsString(SECTION_SEND_LIS_STATUS, KEY_SEND).c_str();
	}

	m_List.SetItemText(Index, 8, strValue);
}


bool CTodayList::SignalUDCisPostive(char* res)
{
	CString resStr;
	resStr.Format(L"%s", res);
	int pos = resStr.Find(_T("(+"));
	if (pos >= 0)
	{
		CString src = resStr.Mid(pos + 2, 1);
		if (src == src.SpanIncluding(_T("0123456789")))
		{
			return true;
		}
	}
	return false;

}

#define NOT_PRINT   0 
#define PRINT  	    1
#define SECTION_PRINT_STATUS   _T("printstatus")
#define KEY_PRINTED            _T("printed")
#define KEY_NOT_PRINTED        _T("notprint")

void  CTodayList::ShowPrintStatus(int Index, int Status)
{
	CString strValue=_T("");

	if (Status == NOT_PRINT)
	{
		strValue=GetIdsString(SECTION_PRINT_STATUS, KEY_NOT_PRINTED).c_str();
	}
	else
	{
		strValue=GetIdsString(SECTION_PRINT_STATUS, KEY_PRINTED).c_str();
	}
	
	m_List.SetItemText(Index, 9, strValue);
}


void CTodayList::ShowStatus( int nIndex, int nSubItem, StatusEnum nStatus )
{
	TCHAR   keyname[50];
	CString strValue=_T("");

	wsprintf(keyname, _T("state%d"), nStatus);
	
	strValue =GetIdsString(_T("list_today"),keyname).c_str();
	m_List.SetItemText(nIndex, nSubItem, strValue);
}


CString GetAttributeDescription(int nAttribute)
{
	TCHAR   keyname[50];
	wsprintf(keyname, _T("Attr%d"), nAttribute);

	return GetIdsString(_T("list_today"),keyname).c_str();
}

void CTodayList::ShowAttribute( int nIndex, int nAttribute )
{
	CString strValue = GetAttributeDescription(nAttribute);
	m_List.SetItemText(nIndex, 4, strValue);

}

void CTodayList::ShowSex( int nIndex, int nSex )
{
	TCHAR   keyname[50];
	CString strValue=_T("");

	if( nSex != 0 && nSex != 1 && nSex != 2 )
		nSex = 1;

	wsprintf(keyname, _T("Sex%d"), nSex);

	strValue=GetIdsString(_T("list_today"),keyname).c_str();
	m_List.SetItemText(nIndex, 8, strValue);

}

void CTodayList::ResetItemMaskCode(int nIndex, int nID, int nStatus )
{
	m_List.SetItemData(nIndex, nID | (nStatus << 24));																																				  

}
int CTodayList::GetIndexByTaskID( int nID )
{
	int  nIndex = m_List.GetItemCount() - 1;
	

	for( ; nIndex >= 0 ; nIndex -- )
	{
		if( GetItemTaskID(nIndex) == nID )
			return nIndex;
	}

	return -1;
}

int CTodayList::GetItemTaskID( int nIndex )
{
	int nID = m_List.GetItemData(nIndex);

	nID <<= 8;
	nID >>= 8;

	return nID;
}

void CTodayList::ShowUSSampleProgress( int nID, int nPos )
{
	TCHAR       format_str[120];
	CString     vStr = _T("");
	StatusEnum  nUSStatus, nUDCStatus;
	curUSTaskId = nID;
	int nIndex = GetIndexByTaskID(nID), nStatus;

	if( nIndex == -1 )
		return;

	nStatus = GetItemStatus(nIndex);
	theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
	if( nUSStatus != TEST )
	{
		nUSStatus = TEST;
		nStatus   = theApp.MakeStatusMaskCode(TEST, nUDCStatus);
		Access_UpdateTaskStatus(nID, nUSStatus);
		ResetItemMaskCode(nIndex, nID, nStatus);
	}

	wcscpy_s(format_str, GetIdsString(_T("list_today"),_T("state3")).c_str());
	vStr.Format(format_str, nPos);
	m_List.SetItemText(nIndex, 1, vStr);
}

void CTodayList::ShowUSRecognitionProgress(int nID, int nPos)
{
	TCHAR       format_str[120];
	CString     vStr = _T("");
	StatusEnum  nUSStatus, nUDCStatus;

	int nIndex = GetIndexByTaskID(nID), nStatus;

	if (nIndex == -1)
		return;

	nStatus = GetItemStatus(nIndex);
	theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
	if (nUSStatus != TEST)
	{
		nUSStatus = TEST;
		nStatus = theApp.MakeStatusMaskCode(TEST, nUDCStatus);
		Access_UpdateTaskStatus(nID, nUSStatus);
		ResetItemMaskCode(nIndex, nID, nStatus);
	}

	wcscpy_s(format_str, GetIdsString(_T("list_today"), _T("state31")).c_str());
	vStr.Format(format_str, nPos);
	m_List.SetItemText(nIndex, 1, vStr);
}

int CTodayList::GetItemStatus( int nIndex )
{
	return  m_List.GetItemData(nIndex) >> 24;
}

void CTodayList::UpdateFinishStatus(int nID, BOOL bUSTestType, BOOL bSuccess, PVOID pContext)
{
	CriticalSection section(m_TaskInfo_CS);
	if (theApp.nAttribute != TestNormal && theApp.nAttribute != TestEP)
	{
		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;

		if (bUSTestType)
		{
			theApp.UpdateTaskStatus(nID, US_TEST_TYPE);

			float *cf = (float*)pContext;
			UpdateUSReslut(nID, *cf, bSuccess);

			pMain->qcDlg->ShowTaskStatus( nID, false);
		}
		else
		{
			curUDCTaskId = nID;
			theApp.UpdateTaskStatus(nID, UDC_TEST_TYPE);

			UpdateUDCResult((PUDC_RESULT_CONTEXT)pContext);

			pMain->qcDlg->ShowTaskStatus(nID);
		}

		Access_UpdateTaskStatus(nID, theApp.MakeStatusMaskCode(FINISH, FINISH));

		return;
	}
	do
	{
		StatusEnum   nUSStatus, nUDCStatus;
		int          nIndex = GetIndexByTaskID(nID), nStatus;
		if (nIndex == -1)
			break;

		nStatus = GetItemStatus(nIndex);
		theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);

		if (bUSTestType)
		{
			theApp.UpdateTaskStatus(nID, US_TEST_TYPE);

			if (bSuccess == FALSE)
			{
				nUSStatus = TESTFAIL;
			}
			else
			{
				float *cf = (float*)pContext; 
				UpdateUSReslut(nID, *cf, bSuccess);

				if (GetCurSelIndex() == nIndex && QueryBillMode() == TRUE) //完成的任务是当前选择的任务,并且处于审核状态下
					nUSStatus = CHECKED;
				else
					nUSStatus = FINISH;

				char ini_path[MAX_PATH];
				strcpy_s(ini_path, theApp.m_szExeFolderA);
				PathAppendA(ini_path, "conf.ini");
				BOOL autoSetDefaultPrintPic = (BOOL)GetPrivateProfileIntA("print", "autoSetDefaultPrintPic", 0, ini_path);
				int autoSetDefaultPrintPicNum = (BOOL)GetPrivateProfileIntA("print", "autoSetDefaultPrintPicNum", 2, ini_path);
				if (autoSetDefaultPrintPicNum < 0 || autoSetDefaultPrintPicNum >8) autoSetDefaultPrintPicNum = 2;
				if (autoSetDefaultPrintPic && autoSetDefaultPrintPicNum > 0)
				{
					//设置默认打印图片，前2张
					TASK_INFO ti = { 0 };
					if (STATUS_SUCCESS == Access_GetTaskInfo(nID, USER_TYPE_INFO, &ti) && strlen(ti.main_info.sImage) == 0)	// 如果打印图片是空的就设置默认打印图片
					{
						CRecTask rt;
						PTASK_REC_INFO tri = NULL;
						if (STATUS_SUCCESS == rt.GetRecGrapTaskInfoWhenFinish((PCHAR)ti.main_info.sFolder, tri) && tri)
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
							if (strlen(tmp))
							{
								tmp[strlen(tmp) - 1] = 0;
							}
							UPDATE_PRINT_PATH1 path;
							path.nTaskID = nID;
							sprintf_s(path.PrintPath, MAX_PRINTIMAGE_LENS, tmp);
							Access_SetPrintPath(&path);
							THInterface_FreeSysBuffer(tri);
						}
					}
				}
			}
			ShowStatus(nIndex, 1, nUSStatus);
		}
		else
		{
			curUDCTaskId = nID;
			theApp.UpdateTaskStatus(nID, UDC_TEST_TYPE);
			if (bSuccess == FALSE)
				nUDCStatus = TESTFAIL;
			else
			{
				UpdateUDCResult((PUDC_RESULT_CONTEXT)pContext);
				nUDCStatus = FINISH;
			}
			ShowStatus(nIndex, 2, nUDCStatus);
		}

		nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
		Access_UpdateTaskStatus(nID, nStatus);

		
		ResetItemMaskCode(nIndex, nID, nStatus);
		if (GetCurSelIndex() == nIndex) //如果缓存中所存放的数据就是当前更新的数据,需要同时更新缓存数据
			UpdateStatusToCache(nStatus);

		m_List.RedrawItems(nIndex, nIndex);
	} while (0);
}

void CTodayList::UpdateFocusStatus(int nID, PVOID pContext)
{
	CriticalSection section(m_TaskInfo_CS);
	int nIndex = GetIndexByTaskID(nID);

	do
	{
		if (nIndex == -1)
			break;
		m_List.SetItemText(nIndex, 1, GetIdsString(_T("list_today"), _T("state32")).c_str());
	} while (0); 
}

void CTodayList::UpdateUSReslut( int nID, float cf ,bool bSuccess)
{
	int              c;
	ULONG            j;
	KSTATUS          status;
	TASK_INFO        task_info = {0};
	PTASK_REC_INFO   rec_info = NULL;
	PCELL_FULL_INFO  cell_info = NULL;
	vector<int>      vectPositiveGrap; // 阳性图片序号集合
	map<int, USHORT> mapCellGrap;      // 图片对应的细胞数量
	TCHAR INIPath[256] = { 0 };
	std::map<int, int> RBCSize;
	map<int, map<int, int>> mapGrapidxCells;

	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("config\\Lis1.ini"));

	char ReslutRelativePath[64] = {0};
	char IniPath[MAX_PATH], Jpg_Path[MAX_PATH], SessionName[128];
	
	int num = MultiByteToWideChar(0, 0, ReslutRelativePath, -1, NULL, 0);
	wchar_t str[128];
	MultiByteToWideChar(0, 0, ReslutRelativePath, -1, str, num);


	char buff[42] = {0};
	_itoa(nID, buff, 10);
	status = THInterface_QueryRecGrapInfo((PVOID)buff, &rec_info);
	if (status != STATUS_SUCCESS)
	{
		for (int i = 0; i < 10; i++)
		{
			vectPositiveGrap.push_back(i);
		}
		SetDefaultPrintGrap(nID, vectPositiveGrap, mapCellGrap);
		return;
	}
	
	for (j = 0; j < rec_info->tGrapCounter; j++)
	{
		status = THInterface_QueryRecCellInfo((PVOID)buff, rec_info->gInfo[j].rIndex, rec_info->gInfo[j].rGrapSize, &cell_info);
		if (status != STATUS_SUCCESS)
			continue;
		
		AnalyzePostivePic(j, cell_info, mapGrapidxCells);
		CountCellResult(cell_info, task_info, RBCSize);
        THInterface_FreeSysBuffer(cell_info);
        cell_info = NULL;
	}
/*	SimulationCell();*/
	task_info.main_info.nID = nID;
	ThreadArgv* m_Argv = new ThreadArgv;
	m_Argv->id = nID;
	m_Argv->RBCSize = std::move(RBCSize);
	m_Argv->info = task_info;
	HANDLE paintTheread;
	m_Argv->pParam = this;
	m_Argv->pMain = AfxGetMainWnd();
// 	paintTheread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PaintDRBCImageThread, (LPVOID)&m_Argv, 0, NULL);
// 	if (!paintTheread)
// 	{
// 		return;
// 	}
	auto f = std::async(std::launch::async, [=]{
		PaintDRBCImageThread(m_Argv);
	});
	DBG_MSG("debug:已经开启异步绘制红细胞位相图");
	c = (int)(theApp.m_b * cf * 100.0f);
	task_info.us_info.nVariableRatio = (float)c / 100.0f;


	CHAR  ConfPath[MAX_PATH] = { 0 };

	GetModuleFileNameA(NULL, ConfPath, MAX_PATH);
	PathRemoveFileSpecA(ConfPath);
	PathAppendA(ConfPath, ("conf.ini"));


	for( j = 0 ; j < MAX_US_COUNTS ; j ++ )
	{
		if( task_info.us_info.us_node[j].us_res == 0.0f )
			continue;


		DBG_MSG("nVariableRatio = %f, task_info.us_info.us_node[%d].us_res = %f", 
			task_info.us_info.nVariableRatio,			
			j,
			task_info.us_info.us_node[j].us_res
			);

		if (theApp.m_isVariableRatioSpecialDeal)
		{
			if (j == CELL_HYAL_TYPE - 1)//透明管型不加系数
			{
				continue;
			}
			if (task_info.us_info.us_node[j].us_res < GetPrivateProfileIntA("calibrate", "MaxValue", 10, ConfPath)) //结果小于10的不乘以系数？
			{
				continue;
			}
		}
		task_info.us_info.us_node[j].us_res *= task_info.us_info.nVariableRatio;
		task_info.us_info.us_node[j].us_res = (int)(task_info.us_info.us_node[j].us_res + 0.5);	 // 四舍五入，取整数
	}

	if (((CMainFrame*)(AfxGetApp()->m_pMainWnd))->qcDlg && (theApp.nAttribute != TestNormal && theApp.nAttribute != TestEP))//窗口有效且为质控模式
	{
		DBG_MSG("debug:发送结果到质控界面,质控球数目为%f", task_info.us_info.us_node[CELL_QC_TYPE - 1].us_res);
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->qcDlg->SendMessage(WM_USER_ADDTASKTOQC, (WPARAM)2, (LPARAM)&task_info);
	}

	SetDefaultPrintGrap(nID, vectPositiveGrap, mapCellGrap);
	THInterface_FreeSysBuffer(cell_info);


	

//theApp.SaveUSLog(nID,task_info);
	vector<int>::iterator it;
	if (2020 == theApp.m_sendLisType && !theApp.m_bIsLiushuixain)//东南医院
	{
		for (it = PROId.begin(); it!= PROId.end();it++)//如果蛋白质为阴性，则忽略沉渣管型
		{
			if (nID == *it)
			{
				task_info.us_info.us_node[CELL_HYAL_TYPE - 1].us_res = 0;
			}
		}
	}
	status = Access_UpdateTaskInfo(US_TYPE_INFO, nID, &task_info);
	task_info.main_info.nID = nID;
	//更新taskinfo状态信息，用于镜检阳性判断
	StatusEnum   nUSStatus, nUDCStatus;
	theApp.ParseStatusMaskCode(task_info.main_info.nState, nUSStatus, nUDCStatus);

	if (bSuccess == FALSE)
	{
		nUSStatus = TESTFAIL;
	}
	else
	{
		 nUSStatus = FINISH;
	}
	task_info.main_info.nState = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
	JudgeUSResult(task_info);
	///////////////////////////////////////////////////
	// 通过判断标本阴阳性来设置打印图片
	if (theApp.m_bIsLiushuixain)
	{
		string strPrintImage;
		vector<int> positiveCellTypes;
		GetPositiveCellTypes(task_info, positiveCellTypes);
		int picIndex = 0;
		if (positiveCellTypes.size() > 0)
		{
			for (auto itr = mapGrapidxCells.cbegin(); itr != mapGrapidxCells.cend(); itr++)
			{
				const map<int, int> & cells = itr->second;
				for (int i = 0; i < positiveCellTypes.size(); i++)
				{
					if (cells.find(positiveCellTypes[i]) != cells.end())
					{
						picIndex = itr->first;
						goto LOOP_END;
					}
				}
			}
		}
		else
		{
			std::srand(std::time(nullptr));
			picIndex = rand() % rec_info->tGrapCounter;
		}
	LOOP_END:
		char msg[256];
		sprintf_s(msg, 256, "040_%04d.jpg|", picIndex);
		if (strlen(msg))
		{
			msg[strlen(msg) - 1] = 0;
		}
		strPrintImage = msg;

		UPDATE_PRINT_PATH1 path;
		path.nTaskID = nID;
		sprintf_s(path.PrintPath, MAX_PRINTIMAGE_LENS, strPrintImage.c_str());
		Access_SetPrintPath(&path);
	}
	///////////////////////////////////////////////////
//theApp.SaveUSLog(nID,task_info,TRUE);
	if( status == STATUS_SUCCESS ) //更新到界面
	{
		if( IsCurSelSameAppointID(nID) == FALSE ) //如果当前用户选择的任务与更新任务ID不一致则不进行更新
			return;

		UpdateCurTaskInfo(task_info, US_TYPE_INFO);
		GetCurTaskInfo(task_info);
		int nIndex = GetIndexByTaskID(nID);
		if (nIndex != -1 && IsPositiveResultTask(task_info))
		{
			m_List.SetItemText(nIndex, 12, _T("1"));
		}
		theApp.GetMainWnd()->SendMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)&task_info, (LPARAM)SHOW_US_TYPE);
		if( QueryBillMode() == TRUE )
			THUIInterface_ShowTaskIcon(nID, TRUE);
	}
}

DWORD CTodayList::PaintDRBCImageThread(ThreadArgv* pArgv)
{
	if (pArgv)
	{
		DBG_MSG("debug:开始绘图");
		ForceCanvas canvas(PAINTWIDTH, PAINTHIGHT);
		DBG_MSG("debug:绘制背景");
		canvas.SetBackgroundColor(RGB(255, 255, 255));
		/*	canvas.DrawLine(100, 100, 500, 100, RGB(0, 255, 0), 3);*/
		DBG_MSG("debug:绘制图像");
		canvas.DrawImage(pArgv);
		DBG_MSG("debug:拼接保存路径");
		char recImagePath[1024] = { 0 };
		GetModuleFileNameA(NULL, recImagePath, MAX_PATH);
		PathRemoveFileSpecA(recImagePath);

		PathAppendA(recImagePath, "\\RecGrapReslut");

		sprintf(recImagePath, "%s\\%d\\%s", recImagePath, pArgv->id, "DRBC.jpg");
		DBG_MSG("debug:DRBCrecImagePath=%s", recImagePath);
		canvas.SaveJpg(recImagePath);

		delete pArgv;
	}
	return 0;
}

BOOL isPATH(PVOID pCodePath)
{
	char str[128] = { 0 };
	strcpy(str, (char*)pCodePath);
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == '\\')
		{
			return TRUE;
		}
	}
	return FALSE;
}
KSTATUS CTodayList::GetRecGrapPath(PVOID pCodePath, LPWSTR pRecGrapPath)
{
	int                    nLen;
	char                   dir[40], TestDir[MAX_PATH];


	GetModuleFileName(NULL, pRecGrapPath, MAX_PATH);
	PathRemoveFileSpec(pRecGrapPath);
	PathAppend(pRecGrapPath, L"RecGrapReslut");
	sprintf(dir, "%u", atoi((char*)pCodePath));

	int num = MultiByteToWideChar(0, 0, dir, -1, NULL, 0);
	wchar_t wide[128];
	MultiByteToWideChar(0, 0, dir, -1, wide, num);

	PathAppend(pRecGrapPath, wide);

	return STATUS_SUCCESS;

}
int  CTodayList::CellMapUsIndex(USHORT nCellType)
{
	if( nCellType >= CELL_RED_TYPE && nCellType <= (CELL_RED_TYPE + 44) )
		return nCellType - 1;

	return -1;
}

void CTodayList::CountCellResult(PCELL_FULL_INFO cell_info, TASK_INFO & task_info, std::map<int, int> & RBCSize)
{
	int              nIndex;

	for( ULONG j = 0 ; j < cell_info->cCellCounter ; j ++ )
	{
		nIndex = CellMapUsIndex(cell_info->cInfo[j].cType);
		if( nIndex == -1 )
			continue;

		//if (cell_info->cInfo[j].cType == 0 || cell_info->cInfo[j].cType == 1)
		if (nIndex == 0 || nIndex == 1)
		{
			DBG_MSG("debug:判断红细胞直径,rect =(%d,%d,%d,%d)", cell_info->cInfo[j].rc.left, 
				cell_info->cInfo[j].rc.top, cell_info->cInfo[j].rc.right, cell_info->cInfo[j].rc.bottom);
			CountRBCSize(cell_info->cInfo[j].rc, RBCSize);
		}
		task_info.us_info.us_node[nIndex].us_res ++;
	}
}

void CTodayList::CountCellResult(PCELL_FULL_INFO  cell_info, TASK_INFO & task_info, int nGrapIndex, vector<int> &v, map<int, USHORT> mapCellGrap)
{
	int nIndex;

	if (cell_info->cCellCounter <= 0)
	{
		return;
	}
	mapCellGrap.insert(make_pair(nGrapIndex, cell_info->cCellCounter)); // 加入游戏包的图片
	nIndex = CellMapUsIndex(cell_info->cInfo[0].cType);
	
	//性别(0未知1男性2女性)
	int nSex = task_info.pat_info.nSex;
	if (theApp.us_cfg_info.par[nIndex].bIsDelete == TRUE)
	{
		return;
	}
	float us_res = task_info.us_info.us_node[nIndex].us_res;
	float nMax_normal_male = theApp.us_cfg_info.par[nIndex].nMax_normal_male;
	float nMin_normal_male = theApp.us_cfg_info.par[nIndex].nMin_normal_male;
	float nMax_normal_female = theApp.us_cfg_info.par[nIndex].nMax_normal_female;
	float nMin_normal_female = theApp.us_cfg_info.par[nIndex].nMin_normal_female;
	if (nSex < 2)
	{
		if (us_res > nMax_normal_male || us_res < nMin_normal_male)
		{
			//return true;
			v.push_back(nGrapIndex);
		}
	}
	else
	{
		if (us_res > nMax_normal_female || us_res < nMin_normal_female)
		{
			//return true;
			v.push_back(nGrapIndex);
		}
	}
}

void CTodayList::OnItemChanged( NMHDR* pNMHDR, LRESULT* pResult )
{

	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;  

	if( pNMListView->uChanged == LVIF_STATE && (pNMListView->uNewState & LVIS_SELECTED) )  
	{  

		KSTATUS    status;
		TASK_INFO  task_info1 = {0};

		CriticalSection section(m_TaskInfo_CS);

		if( GetCurSelIndex() == pNMListView->iItem )
			return;

		AutoSaveCurRecord();

		status = Access_GetTaskInfo(GetItemTaskID(pNMListView->iItem), ALL_TYPE_INFO, &task_info1);
		if( status != STATUS_SUCCESS )
			return;


		InterlockedExchange(&m_CurSelIndex, pNMListView->iItem);
		DBG_MSG(("m_CurSelIndex=%d/n"), m_CurSelIndex);
		DBG_MSG(("EE:::ID:%d , SN:%d/n"),task_info1.main_info.nID,task_info1.main_info.nSN);
		UpdateCurTaskInfo(task_info1, ALL_TYPE_INFO);

		if( QueryBillMode() == TRUE )
			AutoUpdateCheckStatus();

		theApp.GetMainWnd()->SendMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)&task_info1, (LPARAM)(SHOW_US_TYPE | SHOW_UDC_TYPE | SHOW_USER_TYPE));

	}



}

void CTodayList::OnCustomList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// Take the default processing unless we 
	// set this to something else below.
	*pResult = CDRF_DODEFAULT;
	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		COLORREF crText, crBk;
		// 判断测试结果是否为阳性
		if (IsPositiveResultTask(pLVCD->nmcd.dwItemSpec))
		{
			crText = RGB(255, 0, 0);
			pLVCD->clrText = crText;
			*pResult = CDRF_NEWFONT;
		}
		else
		{
			*pResult = CDRF_DODEFAULT;
		}
		//奇偶判断
		/*if ((pLVCD->nmcd.dwItemSpec % 2) == 0) 
		{
			crText = RGB(0, 0, 0);//RGB(32,32,255);
			crBk = RGB(229, 232, 239);
		}
		else if ((pLVCD->nmcd.dwItemSpec % 2) == 1) 
		{
			crText = RGB(0, 0, 0);
			crBk = RGB(240, 247, 249);
		}
		else {
			crText = RGB(0, 0, 0);
			crBk = RGB(0, 0, 126);
		}*/
		// Store the color back in the NMLVCUSTOMDRAW struct.
		//pLVCD->clrText = crText;
		//pLVCD->clrTextBk = crBk;

		//设置选择项的颜色
		/*if (this->m_List.GetItemState(pLVCD->nmcd.dwItemSpec, CDIS_SELECTED)) {
			crBk = RGB(75, 149, 229);//itunes//RGB(10, 36, 106);//RGB(0, 0, 64);
			crText = RGB(255, 0, 0);
			pLVCD->clrText = crText;
			pLVCD->clrTextBk = crBk;
			*pResult = CDRF_NEWFONT;
		}
		if (LVIS_SELECTED == m_List.GetItemState(pLVCD->nmcd.dwItemSpec, LVIS_SELECTED))
		{
			//清除选择状态，如果不清除的话，还是会显示出蓝色的高亮条
			BOOL b = m_List.SetItemState(pLVCD->nmcd.dwItemSpec, 0, LVIS_SELECTED);
			pLVCD->clrText = crText;
			pLVCD->clrTextBk = crBk;

			*pResult = CDRF_NEWFONT;
			return;
		}*/
		//*pResult = CDRF_NEWFONT;
		//*pResult = CDRF_DODEFAULT;
	}
}

LONG CTodayList::GetCurSelIndex() //其实表示记录显示的nIndex
{
	return InterlockedExchangeAdd(&m_CurSelIndex, 0);
}

BOOL CTodayList::IsCurSelSameAppointID( LONG nID )
{
	LONG  nCurIndex = GetCurSelIndex();

	if( nCurIndex == -1 )
		return FALSE;

	if( GetItemTaskID(nCurIndex) == nID )
		return TRUE;

	return FALSE;

}

void CTodayList::UpdateBaseInfo(int taskId, std::map<string, string>& baseInfo)
{
	CriticalSection section(m_TaskInfo_CS);

	TASK_INFO  task_info = { 0 };
	if (IsCurSelSameAppointID(taskId))
	{
		GetCurTaskInfo(task_info);
	}
	else
	{
		if (Access_GetTaskInfo(taskId, USER_TYPE_INFO, &task_info) != STATUS_SUCCESS)
			return;
	}

	//更新病人信息
	map<string, string>::const_iterator iter;
	iter = baseInfo.find("姓名");
	if (iter != baseInfo.end())
	{
		strcpy_s(task_info.pat_info.sName, 32, iter->second.c_str());
	}
	iter = baseInfo.find("性别");
	if (iter != baseInfo.end())
	{
		if (iter->second == string("男"))
		{
			task_info.pat_info.nSex = 1;
		}
		else if (iter->second == string("女"))
		{
			task_info.pat_info.nSex = 2;
		}
		else
		{
			task_info.pat_info.nSex = 0;
		}
	}
	iter = baseInfo.find("条码");
	if (iter != baseInfo.end())
	{
		strcpy_s(task_info.main_info.sCode, 32, iter->second.c_str());
	}
	iter = baseInfo.find("年龄");
	if (iter != baseInfo.end())
	{
		task_info.pat_info.nAge = atoi(iter->second.c_str());
	}
	iter = baseInfo.find("顺序号");
	if (iter != baseInfo.end() && !iter->second.empty())
	{
        string strSeqNo = iter->second;
        for (int j = strSeqNo.length() - 1; j >= 0; j--)
        {
            if (!isdigit(strSeqNo.at(j)))
            {
                strSeqNo.erase(strSeqNo.begin(), strSeqNo.begin() + j + 1);
                break;
            }
        }
        task_info.main_info.nSN = atoi(strSeqNo.c_str());
        task_info.pat_info.nNo = atoi(strSeqNo.c_str());
	}
	iter = baseInfo.find("科室");
	if (iter != baseInfo.end())
	{
		strcpy_s(task_info.main_info.sDepartment, 32, iter->second.c_str());
	}
	iter = baseInfo.find("床号");
	if (iter != baseInfo.end())
	{
		strcpy_s(task_info.pat_info.sSickBed, 64, iter->second.c_str());
	}
	iter = baseInfo.find("临床诊断");
	if (iter != baseInfo.end())
	{
		strcpy_s(task_info.main_info.sDiagnosis, 255, iter->second.c_str());
	}
	iter = baseInfo.find("送检医生");
	if (iter != baseInfo.end())
	{
		strcpy_s(task_info.main_info.sDoctor, 32, iter->second.c_str());
	}
	if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
		return;
	UpdateNewTaskInfoUIByBarcode(taskId, task_info);
	if (IsCurSelSameAppointID(taskId) == FALSE)
		return;

	UpdateCurTaskInfo(task_info, USER_TYPE_INFO);
	GetCurTaskInfo(task_info);
	theApp.GetMainWnd()->SendMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)&task_info, (LPARAM)SHOW_USER_TYPE);
}

void CTodayList::UpdateCurTaskInfo( TASK_INFO &task_info, TYPE_INFO type_info )
{
	CriticalSection section(m_TaskInfo_CS);
	DBG_MSG("debug:锁上");
	do{
		if (type_info == USER_TYPE_INFO)
		{
			DBG_MSG(("3.收到的条码为%s"), task_info.main_info.sCode);
			memcpy(&m_CurTaskInfo.main_info, &task_info.main_info, sizeof(task_info.main_info));
			memcpy(&m_CurTaskInfo.pat_info, &task_info.pat_info, sizeof(task_info.pat_info));
			DBG_MSG(("AA:::ID:%d , SN:%d/n"), task_info.main_info.nID, task_info.main_info.nSN);
		}
		else if (type_info == US_TYPE_INFO)
		{
			memcpy(&m_CurTaskInfo.us_info, &task_info.us_info, sizeof(task_info.us_info));
			DBG_MSG(("BB:::ID:%d /n"), task_info.us_info.nID);
		}
		else if (type_info == UDC_TYPE_INFO)
		{
			memcpy(&m_CurTaskInfo.udc_info, &task_info.udc_info, sizeof(task_info.udc_info));
			DBG_MSG(("CC:::ID:%d /n"), task_info.udc_info.nID);
		}
		else
		{
			DBG_MSG("debug:拷贝");
			memcpy(&m_CurTaskInfo, &task_info, sizeof(task_info));
			DBG_MSG(("DD:::ID:%d , SN:%d/n"), task_info.main_info.nID, task_info.main_info.nSN);
		}
	} while (0);
	DBG_MSG("debug:解锁");
	
}

void CTodayList::GetCurTaskInfo( TASK_INFO &task_info )
{
	CriticalSection section(m_TaskInfo_CS);
	memcpy(&task_info, &m_CurTaskInfo, sizeof(m_CurTaskInfo));
}

void CTodayList::SetSaveCurRecord( BOOL bSave )
{
	InterlockedExchange(&m_SaveCurRecord, bSave);
}

void CTodayList::AnalyzePostivePic(int nGrapIndex, PCELL_FULL_INFO cell_info, map<int, map<int, int>> &mapCells)
{
	if (cell_info == NULL)
	{
		return;
	}
	map<int, int> cells;
	for (ULONG j = 0; j < cell_info->cCellCounter; j++)
	{	
		if (cells.find(cell_info->cInfo[j].cType) != cells.end())
		{
			cells[cell_info->cInfo[j].cType]++;
		}
		else
		{
			cells[cell_info->cInfo[j].cType] = 1;
		}
	}
	if (!cells.empty())
	{
		mapCells[nGrapIndex] = cells;
	}
}

void CTodayList::AutoSaveCurRecord()
{
	TASK_INFO  task_info = {0};

	//if (theApp.m_bSaveDate == false)
	//	return;

	if( GetCurSelIndex() == -1 || IsSaveCurRecord() == FALSE )
		return;

	CriticalSection section(m_TaskInfo_CS);

	GetCurTaskInfo(task_info);
	//KSTATUS r = Access_UpdateTaskInfo(ALL_TYPE_INFO, task_info.main_info.nID, &task_info);
	KSTATUS r = Access_UpdateTaskInfo(US_TYPE_INFO, task_info.main_info.nID, &task_info);
	r = Access_UpdateTaskInfo(UDC_TYPE_INFO, task_info.main_info.nID, &task_info);
	SetSaveCurRecord(FALSE);
}

BOOL CTodayList::IsSaveCurRecord()
{
	return (BOOL)InterlockedExchangeAdd(&m_SaveCurRecord, 0);
}


void CTodayList::GetLevel(TCHAR *LevelDes, int UDCItemIndex, int &Level)
{
	for ( int j = 0; j< UDC_MAXGRADE; j++ )
	{
		if( 0 == wcsicmp(theApp.m_UdcitemInfo[UDCItemIndex].m_ctsGradeName[j], LevelDes) )
		{
			Level = j;
			return;
		}
	}
}


int CTodayList::MakeRandomUDCItemValue(int UDCItemIndex, int Level)
{
	int MinValue = 0;
	int MaxValue = 0;
	int DValue = 0;

	
	MaxValue = theApp.m_UdcitemInfo[UDCItemIndex].m_nGradeThreshold[Level - 1];
	MinValue = theApp.m_UdcitemInfo[UDCItemIndex].m_nGradeThreshold[Level];

	DValue = MaxValue - MinValue;

	srand( GetTickCount() );
	return MinValue + rand() % DValue;
}


int CTodayList::GetRandomLEUValue(int UDCItemIndex)
{
	int Level = 0;
	TCHAR LevelDes[20] = {0};
	srand( GetTickCount() );
	switch ( rand() % 10 )
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		wcscpy( LevelDes, _T("1+") );
		break;
	case 5:
	case 6:
	case 7:
	case 8:
		wcscpy( LevelDes, _T("2+") );
		break;
	case 9:
		wcscpy( LevelDes, _T("3+") );
		break;
	}

	GetLevel( LevelDes, UDCItemIndex, Level );
	return MakeRandomUDCItemValue(UDCItemIndex, Level);
		
}

#define INVALID_UDC_VALUE  -1

int CTodayList::GetRandomUDCItemValue(int UDCItemIndex, TCHAR *LevelDes)
{
	int Level = 0;

	GetLevel( LevelDes, UDCItemIndex, Level );
	return MakeRandomUDCItemValue(UDCItemIndex, Level);
}



int CTodayList::GetModifiedUDCValue(int UDCItemIndex)
{
	switch (UDCItemIndex)
	{
	case 0:
		return GetRandomUDCItemValue(0, _T("2+"));
		break;
	case 1:
		return GetRandomUDCItemValue(1, _T("Pos"));
		break;
	case 2:
		return GetRandomUDCItemValue(2, _T("Normal"));
		break;
	case 3:
		return GetRandomUDCItemValue(3, _T("3+"));
		break;
	case 4:
		return GetRandomUDCItemValue(4, _T("6.0"));
		break;
	case 5:
		return GetRandomUDCItemValue(5, _T("3+"));
		break;
	case 6:
		return GetRandomUDCItemValue(6, _T("1.030"));
		break;
	case 7:
		return GetRandomUDCItemValue(7, _T("2+"));
		break;
	case 8:
		return GetRandomUDCItemValue(8, _T("3+"));
		break;
	case 9:
		return GetRandomUDCItemValue(9, _T("3+"));
		break;
	case 10:
		return GetRandomUDCItemValue(10, _T("+-"));
		break;
	}
	
	return INVALID_UDC_VALUE;
}


void CTodayList::GetGradeResult(int Index, int Value, TASK_INFO &taskInfo)
{
	CString strTemp;

	char ConfigPath[MAX_PATH] = { 0 };

	GetModuleFileNameA(NULL, ConfigPath, MAX_PATH);
	PathRemoveFileSpecA(ConfigPath);
	PathAppendA(ConfigPath, "config//setting.ini");

	int nSGTestWay = GetPrivateProfileIntA("SGTestWay", "Way", 0, ConfigPath);

	if (nSGTestWay == 1 && strcmp(Common::WCharToChar(theApp.m_UdcitemInfo[Index].m_ctsItemCode).c_str(), "SG") == 0)
	{
		CHAR ResultPath[MAX_PATH] = { 0 }, Value[32] = { 0 };
		GetModuleFileNameA(NULL, ResultPath, MAX_PATH);
		PathRemoveFileSpecA(ResultPath);
		PathAppendA(ResultPath, "RecGrapReslut");
		sprintf_s(Value, "%d", taskInfo.udc_info.nID);
		PathAppendA(ResultPath, Value);
		PathAppendA(ResultPath, "Physics.ini");
		DBG_MSG("获取物理比重 nSGTestWay %d,%s", nSGTestWay, ResultPath);
		
		sprintf_s(taskInfo.udc_info.udc_node[Index].udc_res, "%.3f", float(1000.0 + GetPrivateProfileIntA("Physics", "sg_test", 0, ResultPath)) / 1000.0);

		return;
	}

	for (int j = 0; j< UDC_MAXGRADE;j++)
	{
		if (theApp.m_UdcitemInfo[Index].m_bGradeEnable[j])
		{
			if (Value >= theApp.m_UdcitemInfo[Index].m_nGradeThreshold[j])
			{
				m_strTestResult[Index]=theApp.m_UdcitemInfo[Index].m_ctsGradeName[j];
				m_strUnit[Index] = theApp.m_UdcitemInfo[Index].m_ctsGradeValue[j];

				strTemp = theApp.m_UdcitemInfo[Index].m_ctsGradeUnit[j];
				m_strUnit[Index] +=strTemp;

				strcpy(taskInfo.udc_info.udc_node[Index].udc_res,Common::WCharToChar(m_strTestResult[Index].GetBuffer()).c_str());

				strcpy(taskInfo.udc_info.udc_node[Index].udc_value,Common::WCharToChar(m_strUnit[Index].GetBuffer()).c_str());
				break;

			}
		}
	}
}

#define MAX_UDC_VALUE   0XFFFFFFF

bool CTodayList::GetGradeValue(int Index, TCHAR *Grade, int &MinValue, int &MaxValue)
{
	for (int j = 0; j< UDC_MAXGRADE;j++)
	{
		if ( !theApp.m_UdcitemInfo[Index].m_bGradeEnable[j] )
			break;

		if( _tcsicmp(theApp.m_UdcitemInfo[Index].m_ctsGradeName[j], Grade) != 0 )
			continue;

		MinValue = theApp.m_UdcitemInfo[Index].m_nGradeThreshold[j];

		if( j - 1 < 0)
			MaxValue = MAX_UDC_VALUE;
		else
			MaxValue = theApp.m_UdcitemInfo[Index].m_nGradeThreshold[j - 1];

		return true;
	}

	return false;
}


bool CTodayList::GetGradeValue(int projIndex, int grade, int &MinValue, int &MaxValue)
{
	bool ret = false;

	do
	{
		if (grade < 1 || grade > UDC_MAXGRADE)
		{
			break;
		}
		if (!theApp.m_UdcitemInfo[projIndex].m_bGradeEnable[grade - 1])
		{
			break;
		}
		MinValue = theApp.m_UdcitemInfo[projIndex].m_nGradeThreshold[grade - 1];
		if (grade == 1)
			MaxValue = MAX_UDC_VALUE;
		else
			MaxValue = theApp.m_UdcitemInfo[projIndex].m_nGradeThreshold[grade - 2];

		ret = true;
	} while (0);

	return ret;
}

bool CTodayList::IsGreatGrade(int projIndex, int Value, int grade)
{
	bool ret = false;
	int MinValue = 0;
	int MaxValue = 0;

	do 
	{
		if (!GetGradeValue(projIndex, grade, MinValue, MaxValue))
		{
			break;
		}

		ret = (Value < MinValue);
	} while (0);

	return ret;
}

bool CTodayList::IsTheGrade(int Index, int Value, TCHAR *GradeDes)
{
	int MinValue = 0;
	int MaxValue = 0;

	if( !GetGradeValue(Index, GradeDes, MinValue, MaxValue) )
		return false;

	return Value >= MinValue && Value < MaxValue;
}


bool CTodayList::IsPositive1(int Index, int Value)
{
	return IsTheGrade( Index, Value, _T("+1") );;
}


bool CTodayList::IsPositive2(int Index, int Value)
{
	return IsTheGrade( Index, Value, _T("+2") );
}

bool CTodayList::IsPositive3(int Index, int Value)
{
	return IsTheGrade( Index, Value, _T("+3") );
}

bool CTodayList::IsPositive4(int Index, int Value)
{
	return IsTheGrade(Index, Value, _T("+4"));
}

bool CTodayList::IsWeakPositive(int Index, int Value)
{
	return IsTheGrade( Index, Value, _T("+-") );
}


bool CTodayList::IsNegative(int Index, int Value)
{
	return IsTheGrade( Index, Value, _T("-") );
}


bool CTodayList::IsPositiveResultTask(TASK_INFO &TaskInfo)
{
	//性别(0未知1男性2女性)
	int nSex = TaskInfo.pat_info.nSex;
	for (int i = 0; i < MAX_US_COUNTS; i++)
	{
		if (theApp.us_cfg_info.par[i].bIsDelete == TRUE)
		{
			continue;
		}
		float us_res = TaskInfo.us_info.us_node[i].us_res;
		float nMax_normal_male = theApp.us_cfg_info.par[i].nMax_normal_male;
		float nMin_normal_male = theApp.us_cfg_info.par[i].nMin_normal_male;
		float nMax_normal_female = theApp.us_cfg_info.par[i].nMax_normal_female;
		float nMin_normal_female = theApp.us_cfg_info.par[i].nMin_normal_female;
		if (nSex < 2)
		{
			if (us_res > nMax_normal_male || us_res < nMin_normal_male)
			{
				return true;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (us_res > nMax_normal_female || us_res < nMin_normal_female)
			{
				return true;
			}
			else
			{
				continue;
			}
		}
	}
	return false;
}

bool CTodayList::IsPositiveResultTask(int nIndex)
{
	CString str = m_List.GetItemText(nIndex, 12);
	if (str.IsEmpty())
	{
		return false;
	}
	return true;
}

void CTodayList::GetPositiveCellTypes(const TASK_INFO &TaskInfo, vector<int> &vec)
{
	//性别(0未知1男性2女性)
	int nSex = TaskInfo.pat_info.nSex;
	for (int i = 0; i < MAX_US_COUNTS; i++)
	{
		if (theApp.us_cfg_info.par[i].bIsDelete == TRUE)
		{
			continue;
		}
		float us_res = TaskInfo.us_info.us_node[i].us_res;
		float nMax_normal_male = theApp.us_cfg_info.par[i].nMax_normal_male;
		float nMin_normal_male = theApp.us_cfg_info.par[i].nMin_normal_male;
		float nMax_normal_female = theApp.us_cfg_info.par[i].nMax_normal_female;
		float nMin_normal_female = theApp.us_cfg_info.par[i].nMin_normal_female;
		if (nSex < 2)
		{
			if (us_res > nMax_normal_male || us_res < nMin_normal_male)
			{
				return vec.push_back(i+1);
			}
		}
		else
		{
			if (us_res > nMax_normal_female || us_res < nMin_normal_female)
			{
				return vec.push_back(i+1);
			}
		}
	}
}

void CTodayList::JudgeUSResult(TASK_INFO taskInfo)
{
	StatusEnum   nUSStatus, nUDCStatus;
	theApp.ParseStatusMaskCode(taskInfo.main_info.nState, nUSStatus, nUDCStatus);
	positiveID m_us;
	m_us.id = taskInfo.main_info.nID;
	m_us.active = nUSStatus;
	if (m_us.active == NEW)
	{
		return;
	}
	std::vector<positiveID>::iterator it;
	if (m_us.active == FAIL || m_us.active == TESTFAIL || m_us.active == NOTHING)
	{
		if (positiveUSID.size() == 0)
		{
			positiveUSID.push_back(m_us);
			return;
		}
		for (it = positiveUSID.begin(); it != positiveUSID.end();it++)
		{
			if (it->id == m_us.id)
			{
				it->active = m_us.active;
				return;
			}
		}
		positiveUSID.push_back(m_us);
	}

	else if (IsPositiveResultTask(taskInfo) == true)
	{
		if (positiveUSID.size() == 0)
		{
			positiveUSID.push_back(m_us);
			return;
		}
		for (it = positiveUSID.begin(); it != positiveUSID.end(); it++)
		{
			if (it->id == m_us.id)
			{
				it->active = m_us.active;
				return;
			}
		}
		positiveUSID.push_back(m_us);
	}
	return;
// 	if (taskInfo.us_info.us_node[0].us_res > 3)		//红细胞
// // 		taskInfo.us_info.us_node[0].us_res >		//异常红细胞
// // 		taskInfo.us_info.us_node[0].us_res			//白细胞团
// // 		taskInfo.us_info.us_node[0].us_res			//白细胞
// // 		taskInfo.us_info.us_node[0].us_res			//上皮细胞
// // 		taskInfo.us_info.us_node[0].us_res			//霉菌
// // 		taskInfo.us_info.us_node[0].us_res			//芽殖酵母
// // 		taskInfo.us_info.us_node[0].us_res			//吞噬细胞
// // 		taskInfo.us_info.us_node[0].us_res			//小圆上皮
// // 		taskInfo.us_info.us_node[0].us_res			//透明管型
// // 		taskInfo.us_info.us_node[0].us_res			//粒壮管型
// // 		taskInfo.us_info.us_node[0].us_res			//
// // 		taskInfo.us_info.us_node[0].us_res)
// 	for (int i = 0; i < MAX_US_LENS;i++)
// 	{
// 		if (taskInfo.us_info.us_node[i].us_res > taskInfo.us_info.us_node[i].nMax_normal_male 
//  			|| taskInfo.us_info.us_node[i].us_res < taskInfo.us_info.us_node[i].nMin_normal_male)
// 		{
// 			positiveUSID.push_back(m_us);
// 			return;
// 		}
// 	}	
}

#define			LEU_INDEX		0
#define 		NIT_INDEX		1
#define 		URO_INDEX		2
#define 		PRO_INDEX		3
#define 		PH_INDEX		4
#define 		BLD_INDEX		5
#define 		SG_INDEX		6
#define 		KET_INDEX		7
#define 		BIL_INDEX		8
#define 		GLU_INDEX		9	
#define 		VC_INDEX		10	
#define 		MALB_INDEX		11
#define 		CRE_INDEX		12
#define 		CA_INDEX		13



void CTodayList::ModifyUDCValue(USHORT *UDCValueList)
{
	if( IsPositive1(BIL_INDEX, UDCValueList[BIL_INDEX]) || IsPositive2(BIL_INDEX, UDCValueList[BIL_INDEX]) || IsPositive3(BIL_INDEX, UDCValueList[BIL_INDEX]) )
	{
		if ( PathFileExistsA("C:\\uw_udc1.flag") )
		{
			if( IsPositive1(BLD_INDEX, UDCValueList[BLD_INDEX]) || IsWeakPositive(BLD_INDEX, UDCValueList[BLD_INDEX]) ) 
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("-") );
			else if( IsPositive2(BLD_INDEX, UDCValueList[BLD_INDEX]) || IsPositive3(BLD_INDEX, UDCValueList[BLD_INDEX]) ) 
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("+-") );
			else
				NULL;

			if( IsPositive1(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsWeakPositive(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("-") );
			else if( IsPositive2(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsPositive3(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("+-") );
			else
				NULL;
		}
		else if ( PathFileExistsA("C:\\uw_udc2.flag") )
		{
			if( IsPositive1(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsWeakPositive(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("-") );
			else if( IsPositive2(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsPositive3(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("+-") );
			else
				NULL;
		}
		else if ( PathFileExistsA("C:\\uw_udc3.flag") )
		{
			if( IsPositive1(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("+-") );
			else if( IsWeakPositive(LEU_INDEX, UDCValueList[LEU_INDEX]) )
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("-") );
			else if( IsPositive2(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsPositive3(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("+1") );
			else
				NULL;
		}
		else if ( PathFileExistsA("C:\\uw_udc4.flag") )
		{
			if( IsWeakPositive(BLD_INDEX, UDCValueList[BLD_INDEX]) )
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("-") );
			else if( IsPositive1(BLD_INDEX, UDCValueList[BLD_INDEX]) )
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("+-") );
			else if( IsPositive2(BLD_INDEX, UDCValueList[BLD_INDEX]) )
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("+1") );
			else if( IsPositive3(BLD_INDEX, UDCValueList[BLD_INDEX]) )
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("+2") );
			else
				NULL;

			if( IsPositive1(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsWeakPositive(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("-") );
			else if( IsPositive2(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsPositive3(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("+-") );
			else
				NULL;
		}
		else if ( PathFileExistsA("C:\\uw_udc5.flag") )
		{
			if( IsWeakPositive(BLD_INDEX, UDCValueList[BLD_INDEX]) )
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("-") );
			else if( IsPositive1(BLD_INDEX, UDCValueList[BLD_INDEX]) )
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("+-") );
			else if( IsPositive2(BLD_INDEX, UDCValueList[BLD_INDEX]) )
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("+1") );
			else if( IsPositive3(BLD_INDEX, UDCValueList[BLD_INDEX]) )
				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("+2") );
			else
				NULL;

			if( IsPositive1(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("+-") );
			else if( IsWeakPositive(LEU_INDEX, UDCValueList[LEU_INDEX]) )
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("-") );
			else if( IsPositive2(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsPositive3(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("+1") );
			else
				NULL;
		}
		else
		{
			NULL;
		}

		
// 		else
// 		{
// 			if( IsPositive1(BLD_INDEX, UDCValueList[BLD_INDEX]) || IsWeakPositive(BLD_INDEX, UDCValueList[BLD_INDEX]) || IsPositive2(BLD_INDEX, UDCValueList[BLD_INDEX]) ) 
// 				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("-") );
// 
// 			if( IsPositive3(BLD_INDEX, UDCValueList[BLD_INDEX]) ) 
// 				UDCValueList[BLD_INDEX] = GetRandomUDCItemValue( BLD_INDEX, _T("+-") );
// 
// 			if( IsPositive2(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsPositive1(LEU_INDEX, UDCValueList[LEU_INDEX]) || IsWeakPositive(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
// 				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("-") );
// 
// 			if( IsPositive3(LEU_INDEX, UDCValueList[LEU_INDEX]) ) 
// 				UDCValueList[LEU_INDEX] = GetRandomUDCItemValue( LEU_INDEX, _T("+-") );
// 		}

	}

	// 2022.06.16修改：如果蛋白质阳性，微量白蛋白改成阳性
	char ini_path[MAX_PATH];
	strcpy_s(ini_path, theApp.m_szExeFolderA);
	PathAppendA(ini_path, "conf.ini");
	BOOL bProMaRelatedPositive = (BOOL)GetPrivateProfileIntA("udc", "pro_ma_related_positive", 0, ini_path);
	UINT nProGreatGrade = GetPrivateProfileIntA("udc", "pro_great_grade", 1, ini_path);
	UINT nMaDestGrade = GetPrivateProfileIntA("udc", "ma_dest_grade", 1, ini_path);
	if (bProMaRelatedPositive)
	{
		if (IsGreatGrade(PRO_INDEX, UDCValueList[PRO_INDEX], nProGreatGrade))
		{
			if (!IsGreatGrade(MALB_INDEX, UDCValueList[MALB_INDEX], nMaDestGrade)) 
			{
				UDCValueList[MALB_INDEX] = MakeRandomUDCItemValue(MALB_INDEX, nMaDestGrade);
			}
		}
        /*if (IsNegative(PRO_INDEX, UDCValueList[PRO_INDEX]))
        {
            UDCValueList[MALB_INDEX] = GetRandomUDCItemValue(MALB_INDEX, _T("-"));
        }*/
	}

    // 如果蛋白质异常（不是阴性），则为微量白带白随之异常（阳性）
    BOOL bProMaRelatedException = (BOOL)GetPrivateProfileIntA("udc", "pro_ma_related_exception", 0, ini_path);
    if (bProMaRelatedException)
    {
        if(!IsNegative(PRO_INDEX, UDCValueList[PRO_INDEX]))
        {
            UDCValueList[MALB_INDEX] = MakeRandomUDCItemValue(MALB_INDEX, 1);
        }
    }
}


void CTodayList::GetLogFileName(char *LogFilePath)
{
	SYSTEMTIME CurrentTime = {0};
	CStringA LogFileName;

	GetLocalTime(&CurrentTime);

	GetModuleFileNameA(NULL, LogFilePath, MAX_PATH);
	PathRemoveFileSpecA(LogFilePath);
	PathAppendA(LogFilePath, "Log");
	LogFileName.Format("%d_%d_%dudc_debug.txt", CurrentTime.wYear, CurrentTime.wMonth, CurrentTime.wDay);
	PathAppendA( LogFilePath, LogFileName.GetBuffer() );
}


void CTodayList::GetUDCLogData(PUDC_RESULT_CONTEXT udc_ctx, CStringA &LogData)
{
	LogData.AppendFormat("%d\r\n", udc_ctx->TaskId);
	LogData.AppendFormat("%s: %d\r\n", "status", udc_ctx->PaperStatus);
	LogData.AppendFormat("%s: %d\r\n", "LEU", udc_ctx->UdcColorBlockCtx.ColorBlock1_LEU);
	LogData.AppendFormat("%s: %d\r\n", "NIT", udc_ctx->UdcColorBlockCtx.ColorBlock2_NIT);
	LogData.AppendFormat("%s: %d\r\n", "URO", udc_ctx->UdcColorBlockCtx.ColorBlock3_URO);
	LogData.AppendFormat("%s: %d\r\n", "PRO", udc_ctx->UdcColorBlockCtx.ColorBlock4_PRO);
	LogData.AppendFormat("%s: %d\r\n", "PH", udc_ctx->UdcColorBlockCtx.ColorBlock5_PH);
	LogData.AppendFormat("%s: %d\r\n", "BLD", udc_ctx->UdcColorBlockCtx.ColorBlock6_BLD);
	LogData.AppendFormat("%s: %d\r\n", "SG", udc_ctx->UdcColorBlockCtx.ColorBlock7_SG);
	LogData.AppendFormat("%s: %d\r\n", "KET", udc_ctx->UdcColorBlockCtx.ColorBlock8_KET);
	LogData.AppendFormat("%s: %d\r\n", "BIL", udc_ctx->UdcColorBlockCtx.ColorBlock9_BIL);
	LogData.AppendFormat("%s: %d\r\n", "GLU", udc_ctx->UdcColorBlockCtx.ColorBlock10_GLU);
	LogData.AppendFormat("%s: %d\r\n", "VC", udc_ctx->UdcColorBlockCtx.ColorBlock11_VC);
	LogData.AppendFormat("%s: %d\r\n", "MALB", udc_ctx->UdcColorBlockCtx.ColorBlock12_MALB);
	LogData.AppendFormat("%s: %d\r\n", "CRE", udc_ctx->UdcColorBlockCtx.ColorBlock13_CRE);
	LogData.AppendFormat("%s: %d\r\n", "CA", udc_ctx->UdcColorBlockCtx.ColorBlock14_CA);

	return;
}


void CTodayList::LogUDCResultToFile(PUDC_RESULT_CONTEXT udc_ctx)
{
	HANDLE LogFileHandle = INVALID_HANDLE_VALUE ;

	CStringA LogData;
	char LogFilePath[MAX_PATH] = {0};

	DWORD NumberOfBytesWritten = 0;

	GetLogFileName(LogFilePath);
	LogFileHandle = CreateFileA(LogFilePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( LogFileHandle == INVALID_HANDLE_VALUE )
	{
		goto end;
	}

	GetUDCLogData(udc_ctx, LogData);


	SetFilePointer(LogFileHandle, 0, 0, FILE_END);
	WriteFile(LogFileHandle, LogData.GetBuffer(), LogData.GetLength(), &NumberOfBytesWritten, NULL);

end:
	if (LogFileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(LogFileHandle);
	}
}


void CTodayList::UpdateNewTaskInfoUIByBarcode(int TaskID,TASK_INFO taskInfo)
{
	TCHAR Value[20] = { 0 };
	swprintf_s(Value, sizeof(Value) / sizeof(TCHAR), _T("%d"), TaskID);

	for (int Index = 0; Index < m_List.GetItemCount(); ++Index)
	{
		if (0 != m_List.GetItemText(Index, 0).Compare(Value))
			continue;

		m_List.SetItemText(Index, 7, (LPCTSTR)_bstr_t(taskInfo.main_info.sCode));
		return;
	}
}

void CTodayList::SetDefaultPrintGrap(ULONG nID, vector<int> &v, map<int, USHORT> mapCellGrap, int nGrapSize /*= 40*/)
{
	std::string s;
	char msg[256] = { 0 };
	int nSize1 = v.size();
	if (nSize1 >= 6)
	{
		if (nGrapSize == 10)
		{
			for (int i = 0; i < 3; i++)
			{
				sprintf_s(msg, 256, "%03d_%04d.jpg|", nGrapSize, v[i]);
				s.append(msg);
			}
		}
		else
		{
			for (int i = 0; i < 6; i++)
			{
				sprintf_s(msg, 256, "%03d_%04d.jpg|", nGrapSize, v[i]);
				s.append(msg);
			}
		}
	}
	else
	{
		for (int i = 0; i < nSize1; i++)
		{
			sprintf_s(msg, 256, "%03d_%04d.jpg|", nGrapSize, v[i]);
			s.append(msg);
		}
		struct _info{
			int index;
			USHORT count;
		};
		vector<_info> vectGrap;
		map<int, USHORT>::iterator itr;
		for (itr = mapCellGrap.begin(); itr != mapCellGrap.end();)
		{
			_info i;
			i.index = itr->first;
			i.count = itr->second;
			vectGrap.push_back(i);
		}
		sort(vectGrap.begin(), vectGrap.end(), [](_info& d1, _info& d2){
			return d1.count > d2.count;
		});
		int nCount = vectGrap.size() <= (6 - nSize1) ? vectGrap.size() : (6 - nSize1);
		for (int i = 0; i < nCount; i++)
		{
			sprintf_s(msg, 256, "%03d_%04d.jpg|", nGrapSize, vectGrap[i].index);
			s.append(msg);
		}
		if (nCount <= 0)
		{
			int n = min(v.size(), 3);
			for (int i = 0; i < n; i++)
			{
				sprintf_s(msg, 256, "%03d_%04d.jpg|", nGrapSize, v[i]);
				s.append(msg);
			}
		}
	}
	UPDATE_PRINT_PATH1 path;
	path.nTaskID = nID;
	sprintf_s(path.PrintPath, MAX_PRINTIMAGE_LENS, s.c_str());
	Access_SetPrintPath(&path);
}

void CTodayList::DoSubItemEdit(int item, int subitem)
{
	CRect rc;
	m_List.GetSubItemRect(item, subitem, LVIR_BOUNDS, rc);//取得子项的矩形
	rc.left += 2;
	rc.top += 2;
	rc.right -= 4;
	rc.bottom -= 4;

	CString str;
	str = m_List.GetItemText(item, subitem);//取得子项的内容
	m_BarcodeEdit.SetWindowText(str);//将子项的内容显示到编辑框中
	m_BarcodeEdit.ShowWindow(SW_SHOW);//显示编辑框
	m_BarcodeEdit.MoveWindow(&rc);//将编辑框移动到子项上面，覆盖在子项上
	m_BarcodeEdit.SetFocus();//使编辑框取得焦点
	m_BarcodeEdit.CreateSolidCaret(1, rc.Height() - 2);//创建一个光标
	m_BarcodeEdit.ShowCaret();//显示光标
	m_BarcodeEdit.SetSel(-1);//使光标移到最后面
}


void CTodayList::UpdateUDCResult(PUDC_RESULT_CONTEXT udc_ctx)
{

	TCHAR INIPath[256] = { 0 };

	char udcOrder[64] = { 0 };
	char confPath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("config\\Lis1.ini"));

	uint16_t* Value = (uint16_t*)&udc_ctx->UdcColorBlockCtx;
	TASK_INFO taskInfo = {0};

	taskInfo.udc_info.nID = udc_ctx->TaskId;
	taskInfo.udc_info.bUdc = true;

	LogUDCResultToFile(udc_ctx); // UDC结果记录到日志文件

	for (int i = 0;  i < MAX_UDC_COUNTS; i++)
	{
		taskInfo.udc_info.udc_node[i].udc_col = Value[i];
		if (theApp.m_IsAutoModifyLEU && IsNeedModifyTask(udc_ctx->TaskId))
		{
			taskInfo.udc_info.udc_node[i].udc_col = GetModifiedUDCValue(i);
			Value[i] = taskInfo.udc_info.udc_node[i].udc_col;
		}

		GetGradeResult(i, Value[i], taskInfo);
	}

	if (IsNormalTask(udc_ctx->TaskId))
	{
		ModifyUDCValue(Value);

		for (int i = 0;  i < MAX_UDC_COUNTS; i++)
		{
			taskInfo.udc_info.udc_node[i].udc_col = Value[i];
			GetGradeResult(i, Value[i], taskInfo);
		}
		if (2020 == theApp.m_sendLisType && !theApp.m_bIsLiushuixain)//东南医院
		{
			if (!SignalUDCisPostive(taskInfo.udc_info.udc_node[3].udc_res))//如果蛋白质为阴性，则管型为0
			{
				PROId.push_back(udc_ctx->TaskId);
			}
		}
	}

/*theApp.SaveUDCLog(taskInfo.udc_info.nID,taskInfo); */   
	if (Access_UpdateTaskInfo(UDC_TYPE_INFO, udc_ctx->TaskId, &taskInfo) != STATUS_SUCCESS)
		return;
//theApp.SaveUDCLog(taskInfo.udc_info.nID,taskInfo,TRUE); 
	if (IsCurSelSameAppointID(udc_ctx->TaskId) == FALSE)
		return;

	UpdateCurTaskInfo(taskInfo, UDC_TYPE_INFO);
	GetCurTaskInfo(taskInfo);
	theApp.GetMainWnd()->SendMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)&taskInfo, (LPARAM)SHOW_UDC_TYPE);

}

void CTodayList::OnNMDClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	//if( QueryBillMode() == TRUE )
// 	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
// 	AutoSendToLisQueue(pMain,790);
	AutoUpdateCheckStatus();
}

void CTodayList::OnNMClick( NMHDR *pNMHDR, LRESULT *pResult )
{
// 	TASK_INFO task_info;
// 	ZeroMemory(&task_info, sizeof(task_info));
// 	task_info.main_info.nID = 790;
// 	ThreadArgv *m_Argv = new ThreadArgv;
// 	m_Argv->id = 790;
// 	m_Argv->pParam = this;
// 	m_Argv->info = task_info;
// 	m_Argv->pMain = AfxGetMainWnd();
// 	auto f = std::async(std::launch::async, [=]{
// 		PaintDRBCImageThread(m_Argv);
// 	});
/*	UpdateUSReslut(1146, 1, TRUE);*/
	//if( QueryBillMode() == TRUE )
	//	AutoUpdateCheckStatus();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	DBG_MSG("收到NM_CLICK事件，(%d, %d)\n", pNMListView->iItem, pNMListView->iSubItem);
	CRect rc;
	if (pNMListView->iItem != -1)
	{
		m_row = pNMListView->iItem;//m_row为被选中行的行序号（int类型成员变量）
		m_column = pNMListView->iSubItem;//m_column为被选中行的列序号（int类型成员变量）
		if (m_column != 7)//选中列为条码
			return;
		DoSubItemEdit(m_row, m_column);
	}

	*pResult = 0;
}

LONG CTodayList::GetOnlyOneSelected()
{
	POSITION  pos = m_List.GetFirstSelectedItemPosition();

	if( pos == NULL || m_List.GetSelectedCount() > 1 )
		return -1;

	return m_List.GetNextSelectedItem(pos);

}

void CTodayList::SetBillMode( BOOL bCheckMode )
{
// 	if( bCheckMode == TRUE )
// 		m_List.ModifyStyle(0, LVS_SINGLESEL);
// 	else
		m_List.ModifyStyle(LVS_SINGLESEL, 0);

	InterlockedExchange((PLONG)&m_BillMode,bCheckMode);

}

//自动更新审核状态
void CTodayList::AutoUpdateCheckStatus(BOOL bAuto)
{
	int        nOneIndex = GetOnlyOneSelected(), nStatus;
	ULONG      nCurTaskID;
	StatusEnum nUSStatus, nUDCStatus;
	CMainFrame *pFrame;

	if( nOneIndex == -1 )
		return;

	nCurTaskID = GetItemTaskID(nOneIndex);
	nStatus = GetItemStatus(nOneIndex);
	theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus); 

	//对于审核尿沉渣为新建,识别,检测状态,都不能进行审核和显示图片
	DBG_MSG("当前镜检状态为%d,若为1,2,3则返回\n", nUSStatus);
	if (nUSStatus == TEST || nUSStatus == PRO || nUSStatus == NEW)
	{
		if( THUIInterface_GetTaskID() != 0xffffffff ) //如果细胞图表中有细胞,则必须清除
			THUIInterface_ShowTaskIcon(0xffffffff);
		return;
	}
	DBG_MSG("继续运行？\n");
	if( bAuto == TRUE )
	{
		//自动审核将进入审核模式
		pFrame = (CMainFrame *)theApp.GetMainWnd();
		m_AutoSwitchTab = FALSE;
		pFrame->ActiveTab(THUM_VIEW_TAB);
		m_AutoSwitchTab = TRUE;
		SetBillMode(TRUE);

		//显示细胞列表
		THUIInterface_ShowTaskIcon(nCurTaskID);

		//自动审核如果所选择的任务US状态不为成功完成都不能更新为审核状态
		if( nUSStatus != FINISH )
			return;
	}
	else
	{
		//手动审核不再动已经审核的任务再重复审核
		if( nUSStatus == CHECKED )
			return;
	}
	//更新为审核状态
	nUSStatus = CHECKED;
	nStatus  = theApp.MakeStatusMaskCode(nUSStatus,nUDCStatus);
	if( Access_UpdateTaskStatus(nCurTaskID, nStatus) == STATUS_SUCCESS )
	{
		ResetItemMaskCode(nOneIndex, nCurTaskID, nStatus);
		ShowStatus(nOneIndex, 1, nUSStatus);
		if( GetCurSelIndex() == nOneIndex )
		   UpdateStatusToCache(nStatus);
	}


}

void CTodayList::UpdateCellList()
{
	int        nOneIndex = GetOnlyOneSelected();
	ULONG      nTaskID;

	if( nOneIndex == -1 || m_AutoSwitchTab == FALSE ) //如果是程序切换TAB view则不进行更新
		return;

	nTaskID = GetItemTaskID(nOneIndex);
	if( nTaskID == THUIInterface_GetTaskID() ) //相同不进行更新
		return;

	THUIInterface_ShowTaskIcon(nTaskID, FALSE);
}


void CTodayList::DeleteRecord( int nItem )
{
	ULONG       nTaskID;
	DEL_TASK    del_task;
	CUdcView   *udc_view;
	CMainFrame *pMainFrame;

	if( nItem == -1 )
		return;

	nTaskID = GetItemTaskID(nItem);


	del_task.bIsQC = FALSE; //不考虑是否为质控
	del_task.pDelIDArray = &nTaskID;
	del_task.nCounter = 1;
	if( Access_DelTask(&del_task) != STATUS_SUCCESS )
		return;

	//如果被删除的记录是当前显示的记录,则需要进行恢复默认值
	if( GetCurSelIndex() == nItem )
	{
		CMainFrame  *pFrame = (CMainFrame *)theApp.GetMainWnd();

		InterlockedExchange(&m_CurSelIndex,-1);
		pFrame->m_wndResult.m_WndPropList.SetDefaultValue();
		pFrame->m_wndPatient.m_WndPropList.SetDefaultValue();
		if( QueryBillMode() == TRUE && THUIInterface_GetTaskID() == nTaskID )
			THUIInterface_ShowTaskIcon(0xffffffff, FALSE); //强制刷新目的,清除列表
	}

	m_List.DeleteItem(nItem);

	pMainFrame = (CMainFrame*)theApp.GetMainWnd();
	udc_view   = (CUdcView*)pMainFrame->GetUWView()->GetTabWnd(THUM_UDC_TAB);
	udc_view->DeleteItem(nTaskID);
	DelRecGrap(nTaskID);

	//theApp.GetMainWnd()->SendMessage(WM_INSERT_UDC_DATA_MSG, (WPARAM)&task_info, (LPARAM)TRUE);
}

void CTodayList::DeleteRecord()
{
	int        nCounter, j;
	PLONG      pSelItem;
	POSITION   pos;

	nCounter = m_List.GetSelectedCount();

	if( nCounter == 0 )
		return;

	pSelItem = new LONG[nCounter];
	if( pSelItem == NULL )
		return;

	pos = m_List.GetFirstSelectedItemPosition();

	for( j = 0 ; j < nCounter ; j ++ )
		pSelItem[j] = m_List.GetNextSelectedItem(pos);

	m_List.SetRedraw(FALSE);

	for( j = nCounter - 1 ; j >= 0 ; j -- )
		DeleteRecord(pSelItem[j]);

	m_List.SetRedraw(TRUE);

	delete [] pSelItem;
}

void CTodayList::TestRecCircle()
{
	int              c;
	ULONG            j;
	KSTATUS          status;
	TASK_INFO        task_info = { 0 };
	PTASK_REC_INFO   rec_info = NULL;
	vector<int>      vectPositiveGrap; // 阳性图片序号集合
	map<int, USHORT> mapCellGrap;      // 图片对应的细胞数量
	char ReslutRelativePath[64] = { 0 };
	char IniPath[MAX_PATH], Jpg_Path[MAX_PATH], SessionName[128];

	int num = MultiByteToWideChar(0, 0, ReslutRelativePath, -1, NULL, 0);
	wchar_t *str = new wchar_t[num];
	MultiByteToWideChar(0, 0, ReslutRelativePath, -1, str, num);
	PCELL_FULL_INFO cell_info;
	cell_info = new CELL_FULL_INFO;



	strcpy(Jpg_Path, "D:\\THME_code\\THProduct_UW2000_20x\\bin\\Debug\\RecGrapReslut\\5048");

	sprintf(SessionName, "\\%s_%s", "040", "0024");
	strcat(Jpg_Path, SessionName);
	strcat(Jpg_Path, ".jpg");
	QueryQCInfo(Jpg_Path, cell_info);
}

void CTodayList::DeleteAllRecord()
{
	if (MessageBox(L"确定删除当天所有记录？", L"提示", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
	{
		ULONG       nTaskID;
		DEL_TASK    del_task;
		CUdcView   *udc_view;
		CMainFrame *pMainFrame;


		del_task.bIsQC = FALSE; //不考虑是否为质控
		del_task.pDelIDArray = NULL;
		del_task.nCounter = 1;
		if (Access_DelTask(&del_task) != STATUS_SUCCESS)
			return;

		THUIInterface_ShowTaskIcon(0xffffffff, FALSE); //强制刷新目的,清除列表

		pMainFrame = (CMainFrame*)theApp.GetMainWnd();
		udc_view = (CUdcView*)pMainFrame->GetUWView()->GetTabWnd(THUM_UDC_TAB);
		udc_view->FillList();
		m_List.DeleteAllItems();
	}
	else
	{

	}
}

void CTodayList::UpdateUSValueOfCurTaskInfo(USHORT nCellType,float u_res)
{

	CriticalSection section(m_TaskInfo_CS);
	m_CurTaskInfo.us_info.us_node[nCellType-1].us_res = u_res;
	 

}



void CTodayList::PrintReport( BOOL bPrint )
{
	TASK_INFO TaskInfo = {0};
	int          nIndex = GetOnlyOneSelected();
	char         mod_path[MAX_PATH] = "", ini_path[MAX_PATH];



	//if( nIndex == -1 )
	//	return;


	strcpy_s(ini_path, theApp.m_szExeFolderA);
	PathAppendA(ini_path, "conf.ini");

	GetPrivateProfileStringA("temp", "print_temp", "", mod_path, MAX_PATH, ini_path);
	if (mod_path[0] == 0)
		return;
	AutoSaveCurRecord();

//	int TaskID = GetItemTaskID(nIndex);
//	if( bPrint == FALSE )
//	{
//		if( TRUE != UWReport_PrintView(TaskID, mod_path) )               //返回true表示打印成功
//			goto end;
//	}
//	else
//	{
//		if( TRUE != UWReport_Print(TaskID, mod_path) )
//			goto end;
//	}
//
//	GetCurTaskInfo(TaskInfo);
//
//	if (TaskInfo.main_info.nID != TaskID)
//	{
//		if ( Access_GetTaskInfo(TaskID, ALL_TYPE_INFO, &TaskInfo) != STATUS_SUCCESS )
//			return;
//	}
//
//	TaskInfo.main_info.dtPrintStatus = PRINT;
//	Access_UpdateTaskInfo(USER_TYPE_INFO, TaskInfo.main_info.nID, &TaskInfo);
//	UpdatePrintStatusOnUI(TaskInfo.main_info.nID);
//	UpdateCurTaskInfo(TaskInfo, USER_TYPE_INFO);
//
//end:
//	NULL;

	TASK_INFO info = { 0 };
	int        nCounter, j;
	PLONG      pSelItem;
	POSITION   pos;

	nCounter = m_List.GetSelectedCount();

	if( nCounter == 0 )
		return;

	pSelItem = new LONG[nCounter];
	if( pSelItem == NULL )
		return;

	pos = m_List.GetFirstSelectedItemPosition();

	for( j = 0 ; j < nCounter ; j ++ )
		pSelItem[j] = _wtoi(m_List.GetItemText(m_List.GetNextSelectedItem(pos),0));

	

	//for( j = nCounter - 1 ; j >= 0 ; j -- )
	for( j = 0; j < nCounter; j ++ )
	{
		int TaskID = pSelItem[j];

		GetCurTaskInfo(TaskInfo);

		if (TaskInfo.main_info.nID != TaskID)
		{
			if (Access_GetTaskInfo(TaskID, ALL_TYPE_INFO, &TaskInfo) != STATUS_SUCCESS)
				return;
		}

		if (m_autoChangePrit)
		{
			ChangePriTemp(TaskInfo.main_info.nTestType, mod_path);//选择默认打印模板
			if (mod_path[0] == 0)
				return;
		}

		if( bPrint == FALSE )
			{
				if( TRUE != UWReport_PrintView(TaskID, mod_path) )               //返回true表示打印成功
					goto end;
			}
			else
			{
				if( TRUE != UWReport_Print(TaskID, mod_path) )
					goto end;
			}

			TaskInfo.main_info.dtPrintStatus = PRINT;
			Access_UpdateTaskInfo(USER_TYPE_INFO, TaskInfo.main_info.nID, &TaskInfo);
			UpdatePrintStatusOnUI(TaskInfo.main_info.nID);
			UpdateCurTaskInfo(TaskInfo, USER_TYPE_INFO);

		end:
			NULL;
	}

	delete [] pSelItem;
}
void CTodayList::ChangePriTemp(int curSel,char* modPath)//1 udc 2 US 3 udc+us
{
	char ConfigFile[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 }, modalPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, ConfigFile, MAX_PATH);
	PathRemoveFileSpecA(ConfigFile);
	PathAppendA(ConfigFile, "conf.ini");

	GetModuleFileNameA(NULL, modalPath, MAX_PATH);
	PathRemoveFileSpecA(modalPath);
	PathAppendA(modalPath, "Model\\");
	char tempCounts[128] = { 0 }, tempCommt[128] = { 0 };
	GetPrivateProfileStringA("temp", "counts", "3", tempCounts, MAX_PATH, ConfigFile);

	for (int i = 0; i < atoi(tempCounts); i++)
	{
		sprintf_s(tempCommt, "commt%d", i);
		WritePrivateProfileStringA("temp", tempCommt, "", ConfigFile);
	}
	switch (curSel)
	{
	case 1:
	{
		PathRemoveFileSpecA(modalPath);
		PathAppendA(modalPath, "UDC.uit");
		memcpy(modPath, modalPath,sizeof(modalPath));
		break;
	}
	case 2:
	{
		PathRemoveFileSpecA(modalPath);
		PathAppendA(modalPath, "US.uit");
		memcpy(modPath, modalPath, sizeof(modalPath));
		break;
	}
	case 3:
	{
		PathRemoveFileSpecA(modalPath);
		PathAppendA(modalPath, "UDC-US.uit");
		memcpy(modPath, modalPath, sizeof(modalPath));
		break;
	}
	default:
		break;
	}
}
void CTodayList::UpdateStatusToCache( ULONG nStatus )
{
	CriticalSection section(m_TaskInfo_CS);
	m_CurTaskInfo.main_info.nState = nStatus;
	 

}

void CTodayList::UpdateBarCode( PBARCODE_REQ_CONTEXT barcode )
{
	CriticalSection section(m_TaskInfo_CS);

	TASK_INFO  task_info = {0};
	if (IsCurSelSameAppointID(barcode->TaskId))
	{
		GetCurTaskInfo(task_info);
	}
	else
	{
		if( Access_GetTaskInfo(barcode->TaskId, USER_TYPE_INFO, &task_info) != STATUS_SUCCESS )
			return;
	}
	
	int len = min(MAX_BARCODE_LENS - 1, barcode->BarcodeLen);
	memcpy(task_info.main_info.sCode, barcode->Barcode, len);
	DBG_MSG("2.下位机发送条码：%s,条码长度为%d",task_info.main_info.sCode, len);
	if( Access_UpdateTaskInfo(USER_TYPE_INFO, barcode->TaskId, &task_info) != STATUS_SUCCESS )
		return;
	UpdateNewTaskInfoUIByBarcode(barcode->TaskId,task_info);
	if( IsCurSelSameAppointID(barcode->TaskId) == FALSE )
		return;

	UpdateCurTaskInfo(task_info, USER_TYPE_INFO);
	GetCurTaskInfo(task_info);
	theApp.GetMainWnd()->SendMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)&task_info, (LPARAM)SHOW_USER_TYPE);


}

BOOL CTodayList::QueryBillMode()
{
	return (BOOL)InterlockedExchangeAdd((PLONG)&m_BillMode, 0);
}

void CTodayList::SendLis()
{
	int        nCounter, j;
	std::vector<LONG> vecSelItems;
	POSITION   pos;
	TCHAR INIPath[256] = {0};

	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("config\\Lis1.ini"));

	nCounter = m_List.GetSelectedCount();

	if( nCounter == 0 )
		return;

	pos = m_List.GetFirstSelectedItemPosition();

	for (j = 0; j < nCounter; j++)
	{
		vecSelItems.push_back(_wtoi(m_List.GetItemText(m_List.GetNextSelectedItem(pos), 0)));
	}

	m_List.SetRedraw(FALSE);

	if (2020 == theApp.m_sendLisType && !theApp.m_bIsLiushuixain)
	{
		for (j = nCounter - 1; j >= 0; j--)
			SendLis(vecSelItems[j], 2020);
	}
	else if (theApp.m_bIsLiushuixain && 2020 == theApp.m_sendLisType)
	{
		for (j = nCounter - 1; j >= 0; j--)
			SendLisLisShuiXian(vecSelItems[j], 2020);
	}
	else
	{
		for (j = nCounter - 1; j >= 0; j--)
			SendLis(vecSelItems[j]);
	}

	m_List.SetRedraw(TRUE);
}

enum CCheckItemName
{
	RBC,
	WBC, 
	EP,
	CAST,
};
 

struct CLevelInfo
{
	int Level;
	char LevelDes[20];
};


CLevelInfo LevelInfoList[] = 
{
	0, "-",
	1, "+",
	2, "++",
	3, "+++",
};


float MenLevelTable[4][4] = {0};
float WomenLevelTable[4][4] = {0};


float GetPrivateProfileFloatA(char *Section, char *key, char *Path)
{
	char Value[30] = {0};
	GetPrivateProfileStringA(Section, key, "", Value, sizeof(Value), Path);
	return atof(Value);
}


void MakeLevelTable()
{
	char IniPath[MAX_PATH] = {0};

	GetModuleFileNameA( NULL, IniPath, sizeof(IniPath) );
	PathRemoveFileSpecA(IniPath);
	PathAppendA(IniPath, "config");
	PathAppendA(IniPath, "Report.ini");

	MenLevelTable[RBC][0] =  GetPrivateProfileFloatA( "MEN", "RBC1", IniPath);
	MenLevelTable[RBC][1] = GetPrivateProfileFloatA( "MEN", "RBC2", IniPath);
	MenLevelTable[RBC][2] = GetPrivateProfileFloatA( "ALL", "A2", IniPath);
	MenLevelTable[RBC][3] = GetPrivateProfileFloatA( "ALL", "A3", IniPath);

	MenLevelTable[WBC][0] = GetPrivateProfileFloatA( "MEN", "WBC1", IniPath);
	MenLevelTable[WBC][1] = GetPrivateProfileFloatA( "MEN", "WBC2", IniPath);
	MenLevelTable[WBC][2] = GetPrivateProfileFloatA( "ALL", "A2", IniPath);
	MenLevelTable[WBC][3] = GetPrivateProfileFloatA( "ALL", "A3", IniPath);

	MenLevelTable[EP][0] = 0;
	MenLevelTable[EP][1] = GetPrivateProfileFloatA( "ALL", "A1", IniPath);
	MenLevelTable[EP][2] = GetPrivateProfileFloatA( "ALL", "A2", IniPath);
	MenLevelTable[EP][3] = GetPrivateProfileFloatA( "ALL", "A3", IniPath);

	MenLevelTable[CAST][0] = 1;
	MenLevelTable[CAST][1] = GetPrivateProfileFloatA( "ALL", "A1", IniPath);
	MenLevelTable[CAST][2] = GetPrivateProfileFloatA( "ALL", "A2", IniPath);
	MenLevelTable[CAST][3] = GetPrivateProfileFloatA( "ALL", "A3", IniPath);


	WomenLevelTable[RBC][0] = GetPrivateProfileFloatA( "WOMEN", "RBC1", IniPath);
	WomenLevelTable[RBC][1] = GetPrivateProfileFloatA( "WOMEN", "RBC2", IniPath);
	WomenLevelTable[RBC][2] = GetPrivateProfileFloatA( "ALL", "A2", IniPath);
	WomenLevelTable[RBC][3] = GetPrivateProfileFloatA( "ALL", "A3", IniPath);

	WomenLevelTable[WBC][0] = GetPrivateProfileFloatA( "WOMEN", "WBC1", IniPath);
	WomenLevelTable[WBC][1] = GetPrivateProfileFloatA( "WOMEN", "WBC2", IniPath);
	WomenLevelTable[WBC][2] = GetPrivateProfileFloatA( "ALL", "A2", IniPath);
	WomenLevelTable[WBC][3] = GetPrivateProfileFloatA( "ALL", "A3", IniPath);

	WomenLevelTable[EP][0] = 0;
	WomenLevelTable[EP][1] = GetPrivateProfileFloatA( "ALL", "A1", IniPath);
	WomenLevelTable[EP][2] = GetPrivateProfileFloatA( "ALL", "A2", IniPath);
	WomenLevelTable[EP][3] = GetPrivateProfileFloatA( "ALL", "A3", IniPath);

	WomenLevelTable[CAST][0] = 1;
	WomenLevelTable[CAST][1] = GetPrivateProfileFloatA( "ALL", "A1", IniPath);
	WomenLevelTable[CAST][2] = GetPrivateProfileFloatA( "ALL", "A2", IniPath);
	WomenLevelTable[CAST][3] = GetPrivateProfileFloatA( "ALL", "A3", IniPath);
}


void GetCheckItemResultLevel(int Sex, const CCheckItemName &CheckItem, float CheckItemResult, std::string &Level)
{
	static bool IsFirstCall = true;
	float (*LevelTable)[4];

	Level = "";

	if (IsFirstCall)
	{
		MakeLevelTable();
		IsFirstCall = false;
	}

	LevelTable = Sex == 1?MenLevelTable:WomenLevelTable;

	for(int Index = 0; Index < sizeof(LevelInfoList) / sizeof(CLevelInfo); ++Index)
	{
		if (CheckItemResult > LevelTable[CheckItem][Index])
			continue;

		Level = LevelInfoList[Index].LevelDes;
		return;
	}

	//大于第三等级的为"+++"
	Level = "+++";
	return;
}


struct CCheckResultInfo
{
	char CheckItemShortName[30];
	CCheckItemName CheckItem;
};


CCheckResultInfo CheckResultInfoList[] = 
{
	"RBC", RBC,
	"WBC", WBC, 
	"EP", EP, 
	"CAST", CAST,
};


void CTodayList::SendLis( int nItem )
{

	// 从数据库读数据并格式化数据
	unsigned int nSex = 0;
	CString strSex;
	unsigned int nAge = 0;
	unsigned int nAgeUnit = 0;
	CString strAgeUnit;
	TASK_INFO TaskInfo = {0};
	CHAR szSendData[LIS_DATE_MAX_LEN] = "";
	char confPath[MAX_PATH] = { 0 };
	PCR_ACR cr;
	ZeroMemory(&cr, sizeof(cr));

	
	
	TOADY_TASK_INFO1  c_today_info;

     char           sztemp[512] = "";

	 AutoSaveCurRecord();
	 //GetCurTaskInfo(TaskInfo);

	 if (TaskInfo.main_info.nID != nItem)
	 {
		 if ( Access_GetTaskInfo(nItem, ALL_TYPE_INFO, &TaskInfo) != STATUS_SUCCESS )
			 return;
	 }

	 StatusEnum   nUSStatus, nUDCStatus;
	 theApp.ParseStatusMaskCode(TaskInfo.main_info.nState, nUSStatus, nUDCStatus);
	 BOOL isUSFillEmpty = nUSStatus<FINISH;
	 BOOL isUDCFillEmpty = nUDCStatus<FINISH;

// 	   int i;
//        for (i = 0; i < c_today_info.nToadyTaskCounter; ++i)
//        {
// 		   if (c_today_info.pFullTaskInfo[i].main_info.nID == nItem)
// 		   {
// 			   break;
// 		   }
//        }
// 
// 	   if (i == c_today_info.nToadyTaskCounter)
// 	   {
// 		   DBG_MSG("not get task %d from Access_GetTodayTaskInfo", nItem);
// 		   return;
// 	   }


		//cell
		nSex = TaskInfo.pat_info.nSex;
		//m_Rs.GetCollect(_T("nSex"));
		nAge = TaskInfo.pat_info.nAge;//m_Rs.GetCollect(_T("nAge"));
		nAgeUnit = TaskInfo.pat_info.nAgeUnit;//m_Rs.GetCollect(_T("nAgeUnit"));

		if (1 == nSex){strSex = _T("男");}
		else if (2 == nSex){strSex = _T("女");}
		else {strSex = _T(" ");}
		if (1 == nAgeUnit){strAgeUnit = _T("月");}
		else if (2 == nAgeUnit){strAgeUnit = _T("日");}
		else {strAgeUnit = _T("岁");}


		// 基本信息

		sprintf(sztemp,"%c\r\n",0x02);
		strcat(szSendData, sztemp);

		sprintf(sztemp, "ID                       %16d\r\n", TaskInfo.main_info.nID);
		strcat(szSendData, sztemp);
		sprintf(sztemp,"Date                      %16s\r\n",TaskInfo.main_info.dtDate);
		strcat(szSendData, sztemp);
		sprintf(sztemp,"NO.                       %16d\r\n",TaskInfo.main_info.nSN);
		strcat(szSendData, sztemp);
		sprintf(sztemp,"Barcode                   %16s\r\n",TaskInfo.main_info.sCode);
		strcat(szSendData, sztemp);
		sprintf(sztemp,"Name                      %16s\r\n",TaskInfo.pat_info.sName);
		strcat(szSendData, sztemp);

		sprintf(sztemp,"Sex                       %16s\r\n",Common::WCharToChar(strSex.GetBuffer()).c_str());
		strcat(szSendData, sztemp);
		sprintf(sztemp,"Age                       %13d%3s\r\n",TaskInfo.pat_info.nAge,
			Common::WCharToChar(strAgeUnit.GetBuffer()).c_str());
		strcat(szSendData, sztemp);
		char AttrDes[30] = {0};
		WideCharToMultiByte(CP_ACP, 0, GetAttributeDescription(TaskInfo.main_info.nAttribute).GetBuffer(), -1, AttrDes, sizeof(AttrDes), "", NULL );  
		sprintf( sztemp,"Attrib                       %16s\r\n", AttrDes);
		strcat(szSendData, sztemp);

		
		//DBG_MSG("FormatLIS:%s",sztemp);

		if (TaskInfo.main_info.nTestType == 2 || TaskInfo.main_info.nTestType == 3 
            || TaskInfo.main_info.nAttribute == UsQCBy1 || TaskInfo.main_info.nAttribute == UsQCBy2 || TaskInfo.main_info.nAttribute == UsQCBy3)
		{
			sprintf(sztemp, "PrintImagePath                      %16s\r\n", TaskInfo.main_info.sFolder);
			strcat(szSendData, sztemp);
			sprintf(sztemp, "PrintImages                      %16s\r\n", TaskInfo.main_info.sImage);
			strcat(szSendData, sztemp);

			for (int j = 0; j < theApp.us_cfg_info.nUsCounts; j++)
			{

				//if(false != lis_info.TaskInfo.us1.us_d1[i].bIsSendtoLIS)

				if (theApp.us_cfg_info.par[j].bIsDelete == TRUE)
					continue;
				//					sprintf(sztemp,"%s          %10.2f (/uL)\r\n",theApp.us_cfg_info.par[j].sShortName,TaskInfo.us_info.us_node[j].us_res);
				//					strcat(szSendData, sztemp);


				bool IsNeedSendLevel = false;
				for (int Index = 0; Index < sizeof(CheckResultInfoList) / sizeof(CCheckResultInfo); ++Index)
				{
					if (0 != stricmp(CheckResultInfoList[Index].CheckItemShortName, theApp.us_cfg_info.par[j].sShortName))
						continue;

					// 					std::string Level;
					// 					GetCheckItemResultLevel(nSex, CheckResultInfoList[Index].CheckItem, TaskInfo.us_info.us_node[j].us_res, Level);
					// 					sprintf(sztemp,"%s          %25d(%s) (/uL)\r\n",theApp.us_cfg_info.par[j].sShortName,(int)TaskInfo.us_info.us_node[j].us_res, Level.c_str());
					if (isUSFillEmpty)
						sprintf(sztemp, "%s          \r\n", theApp.us_cfg_info.par[j].sShortName);
					else
						sprintf(sztemp, "%s          %25d (/uL)\r\n", theApp.us_cfg_info.par[j].sShortName, (int)TaskInfo.us_info.us_node[j].us_res);
					IsNeedSendLevel = true;
					break;
				}

				if (!IsNeedSendLevel)
				{
					if (isUSFillEmpty)
						sprintf(sztemp, "%s          \r\n", theApp.us_cfg_info.par[j].sShortName);
					else
						sprintf(sztemp, "%s          %25d (/uL)\r\n", theApp.us_cfg_info.par[j].sShortName, (int)TaskInfo.us_info.us_node[j].us_res);
				}

				strcat(szSendData, sztemp);
			}

				//else
				//{
				//	if(lis_info.TaskInfo.MainInfo1.nSex == 2) //男
				//	{
				//		if(EXCEPTION_VALUE_M(lis_info.TaskInfo.us1.us_d1[i],lis_info.TaskInfo.us1.us_d1[i].us_res))			
				//		{
				//			sprintf(sztemp,"%s                       %10.2f (/uL)\r\n",m_us_info.par[i].sLongName,lis_info.TaskInfo.us1.us_d1[i].us_res);
				//			strcat(szSendData, sztemp);
				//		}
				//	}
				//	else if(lis_info.TaskInfo .MainInfo1.nSex == 3)//女
				//	{
				//		if(EXCEPTION_VALUE_F(lis_info.TaskInfo.us1.us_d1[i],lis_info.TaskInfo.us1.us_d1[i].us_res))			
				//		{
				//			sprintf(sztemp,"%s                       %10.2f (/uL)\r\n",m_us_info.par[i].sLongName,lis_info.TaskInfo.us1.us_d1[i].us_res);
				//			strcat(szSendData, sztemp);

				//		}
				//	}
				//}
			// 红细胞位相
			sprintf(sztemp, "RedCellPhase                       %.2f\r\n", 
				TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res<0.00000001f ? 
				0 : 
				(TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res) / (TaskInfo.us_info.us_node[CELL_RED_TYPE - 1].us_res + TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res));
			strcat(szSendData, sztemp);

			sprintf(sztemp, "RedCellPhasePic                      %16s\r\n", "DRBC.jpg");
			strcat(szSendData, sztemp);

			sprintf(sztemp, "Hemosiderin               %16s\r\n", isUSFillEmpty ? "" : "0");
			strcat(szSendData, sztemp);

			sprintf(sztemp, "Faeces                     %16s\r\n", isUSFillEmpty ? "" : "0");
			strcat(szSendData, sztemp);

			sprintf(sztemp, "Starch                     %16s\r\n", isUSFillEmpty ? "" : "0");
			strcat(szSendData, sztemp);

			sprintf(sztemp, "Fiber                     %16s\r\n", isUSFillEmpty ? "" : "0");
			strcat(szSendData, sztemp);

			sprintf(sztemp, "Parasite                  %16s\r\n", isUSFillEmpty ? "" : "0");
			strcat(szSendData, sztemp);

			// 颜色
			sprintf(sztemp, "Color                     %16s\r\n", TaskInfo.main_info.sColor);
			strcat(szSendData, sztemp);

			// 透明度
			sprintf(sztemp, "Transparency              %16s\r\n", TaskInfo.main_info.sTransparency);
			strcat(szSendData, sztemp);

		}

		//// modify 修改113错误 POPO
		//char szLine[MAX_PATH]={0};

		//GetPrivateProfileString("comment",
		//	"lis",
		//	_T("0"),
		//	szLine,
		//	MAX_PATH*8,
		//	szIniFile);

		//int nTolis = atoi(szLine);

		//DBG_MSG("szLine = %s\nszLine,szIniFile = %s\n",szLine, szIniFile);


		//if(nTolis == 1)
		//{
		//	// 诊断
		//	sprintf(sztemp,"Diagnosis                 %16s\r\n",lis_info.TaskInfo.MainInfo1.sComment);
		//	strcat(szSendData, sztemp);
		//}



		if (TaskInfo.main_info.nTestType == 1 || TaskInfo.main_info.nTestType == 3
			|| TaskInfo.main_info.nAttribute == UdcQCByP || TaskInfo.main_info.nAttribute == UdcQCByN)
		{
			for (int j = 0; j < theApp.udc_cfg_info.nUdcCounts; j++)
			{
				if (theApp.udc_cfg_info.par[j].bIsDelete == TRUE)
					continue;

				if (isUDCFillEmpty)
					sprintf(sztemp, "%s  %37s\r\n", theApp.udc_cfg_info.par[j].sShortName, "");
				else
					sprintf(sztemp, "%s  %37s\r\n", theApp.udc_cfg_info.par[j].sShortName, TaskInfo.udc_info.udc_node[j].udc_res);
				strcat(szSendData, sztemp);

				if (strcmp(theApp.udcOrder, "youyuan") == 0)
				{
					CMainFrame* pMain = (CMainFrame*)theApp.m_pMainWnd;
					switch (j)
					{
					case PRO_G:strcpy(cr.pro, pMain->m_wndResult.m_WndPropList.GetIntByUDCGrade(j, TaskInfo.udc_info.udc_node[j].udc_res)); break;
					case CRE_G:strcpy(cr.cre, pMain->m_wndResult.m_WndPropList.GetIntByUDCGrade(j, TaskInfo.udc_info.udc_node[j].udc_res)); break;
					case MCA_G:strcpy(cr.mca, pMain->m_wndResult.m_WndPropList.GetIntByUDCGrade(j, TaskInfo.udc_info.udc_node[j].udc_res)); break;
					default:
						break;
					}
				}
			}
			if (strcmp(theApp.udcOrder, "youyuan") == 0)
			{
				if (!strcmp(cr.cre,"0") == 0)
				{
					char buff[64] = { 0 };
					CString  str = _T("");
					float acr = (float)(atof(cr.mca) / atof(cr.cre));
					sprintf(buff, "%.2f", acr);
					sprintf(sztemp, "ACR  %37s\r\n", isUDCFillEmpty ? "" : buff);
					strcat(szSendData, sztemp);

					float pcr = (float)(atof(cr.pro) / atof(cr.cre));
					sprintf(buff, "%.2f", pcr);

					sprintf(sztemp, "PCR  %37s\r\n", isUDCFillEmpty?"":buff);
					strcat(szSendData, sztemp);
				}
				else
				{
					sprintf(sztemp, "ACR  %37s\r\n", "");
					strcat(szSendData, sztemp);
					sprintf(sztemp, "PCR  %37s\r\n", "");
					strcat(szSendData, sztemp);
				}
			}
		}
	sprintf(sztemp, "%c\r\n", 0x03);
	strcat(szSendData, sztemp);


	DBG_MSG("%s\n", szSendData);
	// 发送
	KSTATUS Result = THInterface_SendlCCmdActionEx(0, szSendData, strlen(szSendData), NULL, 0,T_DEV_LIS_TYPE);
	DBG_MSG("THInterface_SendlCCmdActionEx in SendLis Return: %d", Result);
	if (Result == STATUS_SUCCESS)
	{
		TaskInfo.main_info.dtLisStatus = SEND_LIS;
		Result = Access_UpdateTaskInfo(USER_TYPE_INFO, TaskInfo.main_info.nID, &TaskInfo);
		UpdateLisStatusOnUI(TaskInfo.main_info.nID);
		if (IsCurSelSameAppointID(TaskInfo.main_info.nID)) UpdateCurTaskInfo(TaskInfo, USER_TYPE_INFO);
	}
}


void CTodayList::SendLis(int nItem, int sendType)
{
	// 从数据库读数据并格式化数据
	unsigned int nSex = 0;
	CString strSex;
	unsigned int nAge = 0;
	unsigned int nAgeUnit = 0;
	CString strAgeUnit;
	TASK_INFO TaskInfo = { 0 };
	CHAR szSendData[LIS_DATE_MAX_LEN] = {0};
	TOADY_TASK_INFO1  c_today_info;
	char           sztemp[512] = { 0 };

	char iniPath[512] = { 0 };
	GetModuleFileNameA(NULL, iniPath, MAX_PATH);
	PathRemoveFileSpecA(iniPath);
	PathAppendA(iniPath, "Config\\Lis1.ini");
	theApp.m_sendNum = GetPrivateProfileIntA("config","SendNum",1,iniPath);

	AutoSaveCurRecord();
	//GetCurTaskInfo(TaskInfo);

	if (TaskInfo.main_info.nID != nItem)
	{
		if (Access_GetTaskInfo(nItem, ALL_TYPE_INFO, &TaskInfo) != STATUS_SUCCESS)
			return;
	}

	//cell
	nSex = TaskInfo.pat_info.nSex;
	//m_Rs.GetCollect(_T("nSex"));
	nAge = TaskInfo.pat_info.nAge;//m_Rs.GetCollect(_T("nAge"));
	nAgeUnit = TaskInfo.pat_info.nAgeUnit;//m_Rs.GetCollect(_T("nAgeUnit"));

	if (1 == nSex){ strSex = _T("男"); }
	else if (2 == nSex){ strSex = _T("女"); }
	else { strSex = _T(" "); }
	if (1 == nAgeUnit){ strAgeUnit = _T("月"); }
	else if (2 == nAgeUnit){ strAgeUnit = _T("日"); }
	else { strAgeUnit = _T("岁"); }


	// 基本信息

// 	sprintf(sztemp, "%c", 0x03);
// 	strcat(szSendData, sztemp);

	sprintf(sztemp, "标本号:%d\r\n", TaskInfo.main_info.nID);
	strcat(szSendData, sztemp);
	sprintf(sztemp, "测试模式:%d\r\n", TaskInfo.main_info.nTestType);
	strcat(szSendData, sztemp);
	if (TaskInfo.main_info.nState == 8)//复测
	{
		sprintf(sztemp, "覆测的标本号:%d\r\n", TaskInfo.main_info.nID);
	}
	else
	{
		sprintf(sztemp, "覆测的标本号:%d\r\n", 0);
	}
	strcat(szSendData, sztemp);
	sprintf(sztemp, "条           码:%s\r\n", TaskInfo.main_info.sCode);
	strcat(szSendData, sztemp);
	sprintf(sztemp, "住   院   号:%s\r\n", TaskInfo.pat_info.sHospital);
	strcat(szSendData, sztemp);	
	sprintf(sztemp, "姓           名:%s\r\n", TaskInfo.pat_info.sName);
	strcat(szSendData, sztemp);	
	sprintf(sztemp, "编号:%d\r\n", TaskInfo.main_info.nSN);
	strcat(szSendData, sztemp);	
	sprintf(sztemp, "时间:%s\r\n", TaskInfo.main_info.dtDate);
	strcat(szSendData, sztemp);
	sprintf(sztemp, "颜           色:%s\r\n", TaskInfo.main_info.sColor);
	strcat(szSendData, sztemp);
	sprintf(sztemp, "透   明   度:%s\r\n", TaskInfo.main_info.sTransparency);
	strcat(szSendData, sztemp);

	//DBG_MSG("FormatLIS:%s",sztemp);

	sprintf(sztemp, "[尿沉渣]\r\n");
	strcat(szSendData, sztemp);
	if (TaskInfo.main_info.nTestType == 2 || TaskInfo.main_info.nTestType == 3)
	{
		// 打印图片路径
		sprintf(sztemp, "PrintImagePath                      %16s\r\n", TaskInfo.main_info.sFolder);
		strcat(szSendData, sztemp);
		sprintf(sztemp, "PrintImages                      %16s\r\n", TaskInfo.main_info.sImage);
		strcat(szSendData, sztemp);
		// 红细胞位相
		sprintf(sztemp, "RedCellPhase                       %.2f\r\n",
			TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res < 0.00000001f ?
			0 :
			(TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res) / (TaskInfo.us_info.us_node[CELL_RED_TYPE - 1].us_res + TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res));
		strcat(szSendData, sztemp);

		sprintf(sztemp, "RedCellPhasePic                      %16s\r\n", "DRBC.jpg");
		strcat(szSendData, sztemp);

		for (int j = 0; j < theApp.us_cfg_info.nUsCounts; j++)
		{
			if (theApp.us_cfg_info.par[j].bIsDelete == TRUE)
				continue;

			bool IsNeedSendLevel = false;

			for (int Index = 0; Index < sizeof(CheckResultInfoList) / sizeof(CCheckResultInfo); ++Index)
			{
				if (0 != stricmp(CheckResultInfoList[Index].CheckItemShortName, theApp.us_cfg_info.par[j].sShortName))
					continue;
				if (j == 0)//红细胞
				{
					sprintf(sztemp, "C%s:%d\r\n", theApp.us_cfg_info.par[0].sLongName, (int)TaskInfo.us_info.us_node[0].us_res + (int)TaskInfo.us_info.us_node[1].us_res);
				}
				else
				{
					sprintf(sztemp, "C%s:%d\r\n", theApp.us_cfg_info.par[j].sLongName, (int)TaskInfo.us_info.us_node[j].us_res);
				}			
				IsNeedSendLevel = true;
				break;
			}

			if (!IsNeedSendLevel)
			{
				sprintf(sztemp, "C%s:%d\r\n", theApp.us_cfg_info.par[j].sLongName, (int)TaskInfo.us_info.us_node[j].us_res);
			}

			strcat(szSendData, sztemp);

		}
	}


	sprintf(sztemp, "[尿干化]\r\n");
	strcat(szSendData, sztemp);

	if (TaskInfo.main_info.nTestType == 1 || TaskInfo.main_info.nTestType == 3)
	{
		for (int j = 0; j < theApp.udc_cfg_info.nUdcCounts; j++)
		{
			if (theApp.udc_cfg_info.par[j].bIsDelete == TRUE)
				continue;
			sprintf(sztemp, "G%s:%s\r\n", theApp.udc_cfg_info.par[j].sLongName, TaskInfo.udc_info.udc_node[j].udc_res);
			strcat(szSendData, sztemp);

		}
	}

splitResult:

// 	sprintf(sztemp, "%c", 0x02);
// 	strcat(szSendData, sztemp);

	//DBG_MSG("%s\n", szSendData);
	KSTATUS Result = STATUS_SUCCESS;

	// 发送
	if (theApp.m_bIsLiushuixain)
	{
		// 拷贝图片文件到共享目录
		char PicPath[MAX_PATH] = { 0 }, DstPath[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, PicPath, MAX_PATH);
		PathRemoveFileSpecA(PicPath);
		PathAppendA(PicPath, "RecGrapReslut\\");
		sprintf(sztemp, "%d\\", TaskInfo.main_info.nID);
		PathAppendA(PicPath, sztemp);
		sprintf(DstPath, theApp.m_stDevPipelineInfo.remote.imagePath);
		sprintf(sztemp, "NY\\%d\\%d\\", theApp.m_nDeviceNo, TaskInfo.main_info.nID);
		PathAppendA(DstPath, sztemp);
		if (!CopyDirectory(PicPath, DstPath, FALSE, FALSE))
		{
			DBG_MSG("拷贝图片到审核端失败![%s]->[%s]\n", PicPath, DstPath);
		}
		sprintf_s(sztemp, "图片路径       %s\r\n", DstPath);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "打印图片       %s\r\n", TaskInfo.main_info.sImage);
		strcat(szSendData, sztemp);
		// 发送结果到审核平台
		MSG_PACKAGE_INFO info = { 0 };
		char *pBuf = NULL;
		int nUtf8Size = GBKToUTF8(szSendData, &pBuf);
		CreateTestResultRequest(pBuf, nUtf8Size - 1, &info, theApp.m_nDeviceNo);
		char *pkg = new char[info.nPackageSize];
		if (pkg)
		{
			PackageToData(&info, pkg, info.nPackageSize);
			if (0 != theApp.m_lsx.CreateAsyncTcpClient(
				theApp.m_stDevPipelineInfo.remote.host,
				theApp.m_stDevPipelineInfo.remote.port,
				this, 
				TaskInfo.main_info.nID, pkg, info.nPackageSize, this))
			{
				DBG_MSG("发送结果到审核平台，创建TCP客端失败![%s:%d]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port);
			}
		}
		SAFE_RELEASE_ARRAY(pBuf);
	}
	else
	{
		sprintf(szSendData, "%s%c", szSendData,0x02); // 加尾
		memmove(szSendData + 1, szSendData, strlen(szSendData));
		szSendData[0] = 0x03;
		int i = theApp.m_sendNum;
		while (i--)
		{
			DBG_MSG("THInterface_SendlCCmdActionEx sendNum = %d", i);
			Result = THInterface_SendlCCmdActionEx(0, szSendData, strlen(szSendData), NULL, 0, T_DEV_LIS_TYPE);
			DBG_MSG("THInterface_SendlCCmdActionEx in SendLis Return: %d", Result);
		}
		if (Result == STATUS_SUCCESS)
		{
			REPORT_STATUS1   ReportStatus = { 0 };
			ReportStatus.nTaskID = TaskInfo.main_info.nID;
			ReportStatus.bLis = 1;
			ReportStatus.report_mask = REPORT_LIS_MASK;
			Access_SetReportStatus(&ReportStatus);
			UpdateLisStatusOnUI(TaskInfo.main_info.nID);
			if (IsCurSelSameAppointID(TaskInfo.main_info.nID)) UpdateCurTaskInfo(TaskInfo, USER_TYPE_INFO);
		}
	}
}

void CTodayList::SendLisLisShuiXian(int nItem, int sendType)
{
	// 从数据库读数据并格式化数据
	unsigned int nSex = 0;
	CString strSex;
	int sendNum = 1;
	unsigned int nAge = 0;
	unsigned int nAgeUnit = 0;
	CString strAgeUnit;
	TASK_INFO TaskInfo = { 0 };
	CHAR szSendData[LIS_DATE_MAX_LEN] = { 0 };
	TOADY_TASK_INFO1  c_today_info;
	char           sztemp[512] = { 0 };

	char iniPath[512] = { 0 };
	GetModuleFileNameA(NULL, iniPath, MAX_PATH);
	PathRemoveFileSpecA(iniPath);
	PathAppendA(iniPath, "Config\\Lis1.ini");
	sendNum = GetPrivateProfileIntA("SerialPort", "SendNum", 1, iniPath);

	AutoSaveCurRecord();
	//GetCurTaskInfo(TaskInfo);

	if (TaskInfo.main_info.nID != nItem)
	{
		if (Access_GetTaskInfo(nItem, ALL_TYPE_INFO, &TaskInfo) != STATUS_SUCCESS)
			return;
	}

	//cell
	nSex = TaskInfo.pat_info.nSex;
	//m_Rs.GetCollect(_T("nSex"));
	nAge = TaskInfo.pat_info.nAge;//m_Rs.GetCollect(_T("nAge"));
	nAgeUnit = TaskInfo.pat_info.nAgeUnit;//m_Rs.GetCollect(_T("nAgeUnit"));

	if (1 == nSex){ strSex = _T("男"); }
	else if (2 == nSex){ strSex = _T("女"); }
	else { strSex = _T(" "); }
	if (1 == nAgeUnit){ strAgeUnit = _T("月"); }
	else if (2 == nAgeUnit){ strAgeUnit = _T("日"); }
	else { strAgeUnit = _T("岁"); }


	// 基本信息

	// 	sprintf_s(sztemp, "%c", 0x03);
	// 	strcat(szSendData, sztemp);
	string splitString = ":";
	if (theApp.m_bIsLiushuixain)
	{
		splitString = "       ";
		int nTestItem = TaskInfo.main_info.nTestType;
		if (nTestItem == 2) nTestItem = 1;
		else if (nTestItem == 1) nTestItem = 2;

		int nTestType = TaskInfo.main_info.nAttribute;
        if (nTestType == TestNormal || nTestType == TestEP)
		{
			nTestType = 1; // 正常测试
		}
		else 
		{
			nTestType = 2; // 质控
            nTestItem = TaskInfo.main_info.nAttribute;
		}
		std::string strTime, strDate;
		if (strlen(TaskInfo.main_info.dtDate) > 0)
		{
			vector<string> strMid;
			SplitString(TaskInfo.main_info.dtDate, " ", strMid);
			if (strMid.size() >= 2)
			{
				strDate = strMid[0];
				strTime = strMid[1];
			}
		}


		sprintf_s(sztemp, "ID       %d\r\n", TaskInfo.main_info.nID);
		strcat(szSendData, sztemp);
		if (TaskInfo.main_info.nState == 8)//复测
		{
			sprintf_s(sztemp, "覆测的标本号       %d\r\n", TaskInfo.main_info.nID);
		}
		else
		{
			sprintf_s(sztemp, "覆测的标本号       %d\r\n", 0);
		}
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "条码       %s\r\n", TaskInfo.main_info.sCode);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "住院号       %s\r\n", TaskInfo.pat_info.sHospital);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "姓名       %s\r\n", TaskInfo.pat_info.sName);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "性别       %s\r\n", WstringToString(strSex.GetBuffer()).c_str());
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "年龄       %d%s\r\n", TaskInfo.pat_info.nAge, WstringToString(strAgeUnit.GetBuffer()).c_str());
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "顺序号       %d\r\n", TaskInfo.main_info.nSN);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "时间       %s\r\n", strTime.c_str());
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "日期       %s\r\n", strDate.c_str());
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "颜色       %s\r\n", TaskInfo.main_info.sColor);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "透明度       %s\r\n", TaskInfo.main_info.sTransparency);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "架号       %d\r\n", TaskInfo.main_info.nRow);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "孔号       %d\r\n", TaskInfo.main_info.nPos);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "测试类型       %d\r\n", nTestType);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "测试项目       %d\r\n", nTestItem);
		strcat(szSendData, sztemp);
        sprintf_s(sztemp, "类型       %d\r\n", 3);// 结果类型:1镜检,2干化，3全部(镜检+干化)
		strcat(szSendData, sztemp);
	}
	else
	{
		sprintf_s(sztemp, "标本号:%d\r\n", TaskInfo.main_info.nID);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "测试模式:%d\r\n", TaskInfo.main_info.nTestType);
		strcat(szSendData, sztemp);
		if (TaskInfo.main_info.nState == 8)//复测
		{
			sprintf_s(sztemp, "覆测的标本号:%d\r\n", TaskInfo.main_info.nID);
		}
		else
		{
			sprintf_s(sztemp, "覆测的标本号:%d\r\n", 0);
		}
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "条           码:%s\r\n", TaskInfo.main_info.sCode);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "住   院   号:%s\r\n", TaskInfo.pat_info.sHospital);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "姓           名:%s\r\n", TaskInfo.pat_info.sName);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "编号:%d\r\n", TaskInfo.main_info.nSN);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "时间:%s\r\n", TaskInfo.main_info.dtDate);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "颜           色:%s\r\n", TaskInfo.main_info.sColor);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "透   明   度:%s\r\n", TaskInfo.main_info.sTransparency);
		strcat(szSendData, sztemp);
	}

	//DBG_MSG("FormatLIS:%s",sztemp);

	//sprintf_s(sztemp, "[尿沉渣]\r\n");
	//strcat(szSendData, sztemp);
	if (TaskInfo.main_info.nTestType == US_TEST_TYPE || TaskInfo.main_info.nTestType == ALL_TEST_TYPE)
	{
		// 打印图片路径
		sprintf(sztemp, "PrintImagePath                      %16s\r\n", TaskInfo.main_info.sFolder);
		strcat(szSendData, sztemp);
		sprintf(sztemp, "PrintImages                      %16s\r\n", TaskInfo.main_info.sImage);
		strcat(szSendData, sztemp);
		// 红细胞位相
		sprintf(sztemp, "RedCellPhase                       %.2f\r\n",
			TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res < 0.00000001f ?
			0 :
			(TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res) / (TaskInfo.us_info.us_node[CELL_RED_TYPE - 1].us_res + TaskInfo.us_info.us_node[CELL_DRED_TYPE - 1].us_res));
		strcat(szSendData, sztemp);

		sprintf(sztemp, "RedCellPhasePic                      %16s\r\n", "DRBC.jpg");
		strcat(szSendData, sztemp);

        bool isQC = (TaskInfo.main_info.nAttribute >= UsQCBy1 && TaskInfo.main_info.nAttribute <= UdcQCByN);
		for (int j = 0; j < theApp.us_cfg_info.nUsCounts; j++)
		{
			if (theApp.us_cfg_info.par[j].bIsDelete == TRUE)
				continue;

			bool IsNeedSendLevel = false;

            if (!isQC)
            {
                for (int Index = 0; Index < sizeof(CheckResultInfoList) / sizeof(CCheckResultInfo); ++Index)
                {
                    if (0 != stricmp(CheckResultInfoList[Index].CheckItemShortName, theApp.us_cfg_info.par[j].sShortName))
                        continue;
                    /*if (j == 0)//红细胞
                    {
                        sprintf_s(sztemp, "C%s%s%d\r\n", theApp.us_cfg_info.par[0].sLongName, splitString.c_str(), (int)TaskInfo.us_info.us_node[0].us_res + (int)TaskInfo.us_info.us_node[1].us_res);
                    }
                    else*/
                    {
                        sprintf_s(sztemp, "C%s%s%d\r\n", theApp.us_cfg_info.par[j].sLongName, splitString.c_str(), (int)TaskInfo.us_info.us_node[j].us_res);
                    }
                    IsNeedSendLevel = true;
                    break;
                }

                if (!IsNeedSendLevel)
                {
                    sprintf_s(sztemp, "C%s%s%d\r\n", theApp.us_cfg_info.par[j].sLongName, splitString.c_str(), (int)TaskInfo.us_info.us_node[j].us_res);
                }
                strcat(szSendData, sztemp);
            }
            else
            {
                if (CString("QC") == theApp.us_cfg_info.par[j].sShortName)
                {
                    sprintf_s(sztemp, "C%s%s%d\r\n", theApp.us_cfg_info.par[j].sLongName, splitString.c_str(), (int)TaskInfo.us_info.us_node[j].us_res);
                    strcat(szSendData, sztemp);
                    break;
                }
            }
		}
	}


	//sprintf_s(sztemp, "[尿干化]\r\n");
	//strcat(szSendData, sztemp);

	if (TaskInfo.main_info.nTestType == UDC_TEST_TYPE || TaskInfo.main_info.nTestType == ALL_TEST_TYPE)
	{
		for (int j = 0; j < theApp.udc_cfg_info.nUdcCounts; j++)
		{
			if (theApp.udc_cfg_info.par[j].bIsDelete == TRUE)
				continue;
			sprintf_s(sztemp, "G%s%s%s\r\n", theApp.udc_cfg_info.par[j].sLongName, splitString.c_str(), TaskInfo.udc_info.udc_node[j].udc_res);
			strcat(szSendData, sztemp);

		}
	}

splitResult:

	// 	sprintf_s(sztemp, "%c", 0x02);
	// 	strcat(szSendData, sztemp);

	//DBG_MSG("%s\n", szSendData);
	KSTATUS Result = STATUS_SUCCESS;

	// 发送
	if (theApp.m_bIsLiushuixain)
	{
		// 拷贝图片文件到共享目录
		char PicPath[MAX_PATH] = { 0 }, DstPath[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, PicPath, MAX_PATH);
		PathRemoveFileSpecA(PicPath);
		PathAppendA(PicPath, "RecGrapReslut\\");
		sprintf_s(sztemp, "%d\\", TaskInfo.main_info.nID);
		PathAppendA(PicPath, sztemp);
		sprintf_s(DstPath, theApp.m_stDevPipelineInfo.remote.imagePath);
		sprintf_s(sztemp, "NY\\%d\\%d\\", theApp.m_nDeviceNo, TaskInfo.main_info.nID);
		PathAppendA(DstPath, sztemp);
		string strSrcIniFile = PicPath;
		strSrcIniFile.append("Result.ini");
		string strDstIniFile = PicPath;
		strDstIniFile.append("Reslut.ini");
		CopyFileA(strSrcIniFile.c_str(), strDstIniFile.c_str(), FALSE);
		if (!CopyDirectory(PicPath, DstPath, FALSE, FALSE))
		{
			DBG_MSG("拷贝图片到审核端失败![%s]->[%s]\n", PicPath, DstPath);
		}
		sprintf_s(sztemp, "图片路径       %s\r\n", DstPath);
		strcat(szSendData, sztemp);
		sprintf_s(sztemp, "打印图片       %s\r\n", TaskInfo.main_info.sImage);
		strcat(szSendData, sztemp);
		// 发送结果到审核平台
		MSG_PACKAGE_INFO info = { 0 };
		char *pBuf = NULL;
		int nUtf8Size = GBKToUTF8(szSendData, &pBuf);
		CreateTestResultRequest(pBuf, nUtf8Size - 1, &info, theApp.m_nDeviceNo);
		char *pkg = new char[info.nPackageSize];
		if (pkg)
		{
			PackageToData(&info, pkg, info.nPackageSize);
			if (0 != theApp.m_lsx.CreateAsyncTcpClient(
				theApp.m_stDevPipelineInfo.remote.host,
				theApp.m_stDevPipelineInfo.remote.port,
				this,
				TaskInfo.main_info.nID, pkg, info.nPackageSize, this))
			{
				DBG_MSG("发送结果到审核平台，创建TCP客端失败![%s:%d]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port);
			}
		}
		SAFE_RELEASE_ARRAY(pBuf);
	}
	else
	{
		sprintf_s(szSendData, "%s%c", szSendData, 0x02); // 加尾
		memmove(szSendData + 1, szSendData, strlen(szSendData));
		szSendData[0] = 0x03;
		while (sendNum--)
		{
			DBG_MSG("THInterface_SendlCCmdActionEx sendNum = %d", sendNum);
			Result = THInterface_SendlCCmdActionEx(0, szSendData, strlen(szSendData), NULL, 0, T_DEV_LIS_TYPE);
			DBG_MSG("THInterface_SendlCCmdActionEx in SendLis Return: %d", Result);
		}
		if (Result == STATUS_SUCCESS)
		{
			REPORT_STATUS1   ReportStatus = { 0 };
			ReportStatus.nTaskID = TaskInfo.main_info.nID;
			ReportStatus.bLis = 1;
			ReportStatus.report_mask = REPORT_LIS_MASK;
			Access_SetReportStatus(&ReportStatus);
			UpdateLisStatusOnUI(TaskInfo.main_info.nID);
			if (IsCurSelSameAppointID(TaskInfo.main_info.nID)) UpdateCurTaskInfo(TaskInfo, USER_TYPE_INFO);
		}
	}
}

void CTodayList::UpdatePrintStatusOnUI(int TaskID)
{
	TCHAR Value[20] = {0};	
	swprintf_s(Value, sizeof(Value)/sizeof(TCHAR), _T("%d"), TaskID);

	for (int Index = 0; Index < m_List.GetItemCount(); ++Index)
	{
		if ( 0 != m_List.GetItemText(Index, 0).Compare(Value) )
			continue;

		ShowPrintStatus(Index, PRINT);
		return;
	}
}


void CTodayList::UpdateLisStatusOnUI(int TaskID)
{
	TCHAR Value[20] = {0};
	swprintf_s(Value, sizeof(Value)/sizeof(TCHAR), _T("%d"), TaskID);	

	int nCount = m_List.GetItemCount();
	for (int Index = 0; Index < nCount; ++Index)
	{
		if ( 0 != m_List.GetItemText(Index, 0).Compare(Value) )
			continue;

		ShowLisStatus(Index, SEND_LIS);
		return;
	}
}

void  CTodayList::UpdatePhysicsResult(PPHYSICS_RESULT_CONTEXT ctx)
{
	CHAR ResultPath[MAX_PATH] = { 0 }, Value[32] = { 0 };
	GetModuleFileNameA(NULL, ResultPath, MAX_PATH);
	PathRemoveFileSpecA(ResultPath);
	PathAppendA(ResultPath, "RecGrapReslut");
	sprintf_s(Value, "%d", ctx->TaskId);
	PathAppendA(ResultPath, Value);
	DBG_MSG("保存物理三项结果 %s", ResultPath);
	if (PathFileExistsA(ResultPath) == FALSE)
	{
		CreateDirectoryA(ResultPath, NULL);
	}

	PathAppendA(ResultPath, "Physics.ini");
	sprintf_s(Value, "%d", ctx->sg_test);
	WritePrivateProfileStringA("Physics", "sg_test", Value, ResultPath);

	sprintf_s(Value, "%d", ctx->sg_original_a);
	WritePrivateProfileStringA("Physics", "sg_original_a", Value, ResultPath);

	sprintf_s(Value, "%d", ctx->sg_original_b);
	WritePrivateProfileStringA("Physics", "sg_original_b", Value, ResultPath);
	
}

void CTodayList::UpdateCurTaskPrintImages(ULONG nTaskId, char* PrintPath)
{
	if (m_CurTaskInfo.main_info.nID == nTaskId)
	{
		sprintf_s(m_CurTaskInfo.main_info.sImage, sizeof(m_CurTaskInfo.main_info.sImage), PrintPath);
	}
}

void CTodayList::UpdateRecordList(void)
{
	m_List.SetRedraw(FALSE);
	CMainFrame  *pMainFrm = (CMainFrame*)theApp.GetMainWnd();
	m_List.DeleteAllItems();

	InterlockedExchange(&m_CurSelIndex, -1);
	InterlockedExchange(&m_SaveCurRecord, 0);
	THUIInterface_ShowTaskIcon(0xffffffff, FALSE); //制刷新目的,清除列表
	pMainFrm->m_wndPatient.m_WndPropList.SetDefaultValue();
	pMainFrm->m_wndResult.m_WndPropList.SetDefaultValue();

	CString strTemp=_T("");
	
	int nItem = -1;
	for (int i=0; i<theApp.searchtask.pSearchInfoCounts; ++i)
	{
		JudgeUSResult(theApp.searchtask.pSearchInfo[i]);
		m_List.InsertItem(i,_T(""));
/*		m_List.SetItemData(i,theApp.searchtask.pSearchInfo[i].main_info.nID);*/

	/*	strTemp.Format(_T("%d"),theApp.searchtask.pSearchInfo[i].main_info.nSN);
		m_List.SetItemText(i,1,strTemp);
		m_List.SetItemText(i,2,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].main_info.sFolder).c_str());
		m_List.SetItemText(i,3,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].main_info.sCode).c_str());
		m_List.SetItemText(i,4,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].pat_info.sName).c_str());


		switch(theApp.searchtask.pSearchInfo[i].pat_info.nSex)
		{
		case 0:
			strTemp=_T("未知");
			break;
		case 1:
			strTemp=_T("女");
			break;
		case 2:
			strTemp=_T("男");
			break;
		}
		m_List.SetItemText(i,5,strTemp);
		m_List.SetItemText(i,6,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].pat_info.sHospital).c_str());
		m_List.SetItemText(i,7,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].main_info.dtDate).c_str());*/



		//m_List.InsertItem(i, _T(""));
		//strValue.Format(_T("%u"),task_info.main_info.nID);
		//m_List.SetItemText(nIndex, 0, strValue);
		//ShowStatus(nIndex, 1, nUSStatus);
		//ShowStatus(nIndex, 2, nUDCStatus);
		//strValue.Format(_T("%d"),task_info.main_info.nSN);
		strTemp.Format(_T("%d"),theApp.searchtask.pSearchInfo[i].main_info.nID);
		m_List.SetItemText(i,0,strTemp);
		
		StatusEnum    nUSStatus,nUDCStatus;
		theApp.ParseStatusMaskCode(theApp.searchtask.pSearchInfo[i].main_info.nState, nUSStatus, nUDCStatus);
// 		if (nUSStatus != 5 && nUSStatus != 8 && nUSStatus != 6)//只要不是5,6,8则为测试失败
// 		{
// 			nUSStatus = (StatusEnum)4;
// 		}
// 		if (nUDCStatus != 5 && nUDCStatus != 8 && nUDCStatus != 6)
// 		{
// 			nUDCStatus = (StatusEnum)4;
// 		}
 		ShowStatus(i, 1, nUSStatus);
		ShowStatus(i, 2, nUDCStatus);
		ResetItemMaskCode(i, theApp.searchtask.pSearchInfo[i].main_info.nID,theApp.searchtask.pSearchInfo[i].main_info.nState );
		strTemp.Format(_T("%d"),theApp.searchtask.pSearchInfo[i].main_info.nSN);
		m_List.SetItemText(i,3,strTemp);
		ShowAttribute(i, theApp.searchtask.pSearchInfo[i].main_info.nAttribute);
		strTemp.Format(_T("%d"),theApp.searchtask.pSearchInfo[i].main_info.nRow);
		m_List.SetItemText(i,5,strTemp);
		strTemp.Format(_T("%d"),theApp.searchtask.pSearchInfo[i].main_info.nPos);
		m_List.SetItemText(i,6,strTemp);
/*		strTemp.Format(_T("%s"), (LPCTSTR)_bstr_t(theApp.searchtask.pSearchInfo[i].main_info.sCode));*/
		m_List.SetItemText(i, 7, (LPCTSTR)_bstr_t(theApp.searchtask.pSearchInfo[i].main_info.sCode));
/*		m_List.SetItemText(i,7,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].pat_info.sName).c_str());*/
		ShowPrintStatus(i, theApp.searchtask.pSearchInfo[i].main_info.dtPrintStatus);
		ShowLisStatus(i, theApp.searchtask.pSearchInfo[i].main_info.dtLisStatus);
		m_List.SetItemText(i,10,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].main_info.dtDate).c_str());
		strTemp.Format(_T("%d"),theApp.searchtask.pSearchInfo[i].main_info.nAlarm);
		m_List.SetItemText(i,11,strTemp);

	}
	m_List.SetRedraw(TRUE);
}

KSTATUS CTodayList::QueryQCInfo(char* pPath, PCELL_FULL_INFO pCellInfo)
{
	// 	if (theApp.nAttribute == UsQCByN || theApp.nAttribute == UsQCByP)
	// 	{
	IplImage* srcImg, *RoiImg;

	int num = MultiByteToWideChar(0, 0, pPath, -1, NULL, 0);
	wchar_t *str = new wchar_t[num];
	MultiByteToWideChar(0, 0, pPath, -1, str, num);
	if (!PathFileExists(str))
	{
		DBG_MSG("图片路径不存在");
		return 1;
	}
	srcImg = cvLoadImage(pPath, 0);
	DBG_MSG("pPath %s\n", pPath);
	cvSmooth(srcImg, srcImg, CV_GAUSSIAN, 7, srcImg->nChannels);
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* circles = 0;
	//第四个参数，double类型的dp，用来检测圆心的累加器图像的分辨率于输入图像之比的倒数，且此参数允许创建一个比输入图像分辨率低的累加器。上述文字不好理解的话，来看例子吧。例如，如果dp= 1时，累加器和输入图像具有相同的分辨率。如果dp=2，累加器便有输入图像一半那么大的宽度和高度。
	//第五个参数，double类型的minDist，为霍夫变换检测到的圆的圆心之间的最小距离，即让我们的算法能明显区分的两个不同圆之间的最小距离。这个参数如果太小的话，多个相邻的圆可能被错误地检测成了一个重合的圆。反之，这个参数设置太大的话，某些圆就不能被检测出来了。
	//第六个参数，double类型的param1，有默认值100。它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法CV_HOUGH_GRADIENT，它表示传递给canny边缘检测算子的高阈值，而低阈值为高阈值的一半。
	//第七个参数，double类型的param2，也有默认值100。它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法CV_HOUGH_GRADIENT，它表示在检测阶段圆心的累加器阈值。它越小的话，就可以检测到更多根本不存在的圆，而它越大的话，能通过检测的圆就更加接近完美的圆形了。
	//第八个参数，int类型的minRadius,有默认值0，表示圆半径的最小值。
	//第九个参数，int类型的maxRadius,也有默认值0，表示圆半径的最大值。

	circles = cvHoughCircles(srcImg, storage, CV_HOUGH_GRADIENT, 1, 18, 100, 25, 5, 65);

	pCellInfo->cCellCounter = circles->total;

	float* p = NULL;
	for (size_t i = 0; i < pCellInfo->cCellCounter; i++)
	{
		p = (float*)cvGetSeqElem(circles, i);
		if (p != NULL)
		{
			pCellInfo->cInfo->rc.left = (LONG)p[0] - (LONG)p[2];
			pCellInfo->cInfo->rc.right = (LONG)p[0] + (LONG)p[2];
			pCellInfo->cInfo->rc.top = (LONG)p[1] - (LONG)p[2];
			pCellInfo->cInfo->rc.bottom = (LONG)p[1] + (LONG)p[2];

			CvPoint pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
			cvCircle(srcImg, pt, cvRound(p[2]), CV_RGB(255, 255, 255), 3);
		}
	}
	pCellInfo->cInfo->cType = 45;//质控球
	cvSaveImage(pPath, srcImg);
	cvReleaseMemStorage(&storage);

	cvReleaseImage(&srcImg);
	/*	}*/
	return STATUS_SUCCESS;
}

BOOL CTodayList::IsSelectedCurTask()
{
	int        nCounter, j, nItem;
	POSITION   pos;

	if( theApp.IsIdleStatus() == TRUE )
		return FALSE;

	nCounter = m_List.GetSelectedCount();

	if( nCounter == 0 )
		return FALSE;

	pos = m_List.GetFirstSelectedItemPosition();

	for( j = 0 ; j < nCounter ; j ++ )
	{

		nItem = m_List.GetNextSelectedItem(pos);

		if( theApp.IsCurTaskStatus(GetItemTaskID(nItem)) == TRUE )
			return TRUE;
	}

	return FALSE;


}


void CTodayList::UpdateTaskToList(int nID)
{
    if (nID <= 0)
    {
        return;
    }
    TASK_INFO  task_info = { 0 };
    KSTATUS state = Access_GetTaskInfo(nID, USER_TYPE_INFO, &task_info);
    if (state != STATUS_SUCCESS)
    {
        return;
    }

    for (int i = 0; i < m_List.GetItemCount(); i++)
    {
        if (GetItemTaskID(i) != nID)
        {
            continue;
        }
        CString       strValue = _T("");
        int nIndex = i;

        //JudgeUSResult(task_info);
        strValue.Format(_T("%u"), task_info.main_info.nID);
        m_List.SetItemText(nIndex, 0, strValue);
        strValue.Format(_T("%d"), task_info.main_info.nSN);
        m_List.SetItemText(nIndex, 3, strValue);
        ShowAttribute(nIndex, task_info.main_info.nAttribute);
        strValue.Format(_T("%d"), task_info.main_info.nRow);
        m_List.SetItemText(nIndex, 5, strValue);
        strValue.Format(_T("%d"), task_info.main_info.nPos);
        m_List.SetItemText(nIndex, 6, strValue);
        m_List.SetItemText(nIndex, 7, (LPCTSTR)_bstr_t(task_info.main_info.sCode));
        ShowPrintStatus(nIndex, task_info.main_info.dtPrintStatus);
        ShowLisStatus(nIndex, task_info.main_info.dtLisStatus);
        m_List.SetItemText(nIndex, 10, Common::CharToWChar(task_info.main_info.dtDate).c_str());
        strValue.Format(_T("%d"), task_info.main_info.nAlarm);
        m_List.SetItemText(nIndex, 11, strValue);
        /*if (IsPositiveResultTask(task_info))
        {
            m_List.SetItemText(nIndex, 12, _T("1"));
        }*/
        break;
    }
}

bool CTodayList::IsNormalTask(int TaskID)
{
	TASK_INFO TaskInfo = {0};

	if( STATUS_SUCCESS != Access_GetTaskInfo(TaskID, ALL_TYPE_INFO, &TaskInfo) )
		return false;

	return TaskInfo.main_info.nAttribute == 0;
}


bool CTodayList::IsNeedModifyTask(int TaskID)
{
	TASK_INFO TaskInfo = {0};

	if( STATUS_SUCCESS != Access_GetTaskInfo(TaskID, ALL_TYPE_INFO, &TaskInfo) )
		return false;

	return TaskInfo.main_info.nAttribute == 3;
}


LONG CTodayList::IsNoramlTestType(int &nTestType, int &nItemIndex)
{
	BOOL          bFound = FALSE;
	ULONG         nStatus;
	StatusEnum    nUDCStatus, nUSStatus;

	nTestType = 0;

    for( ULONG j = 0 ; j < m_List.GetItemCount() ; j ++ )
	{
		nStatus = GetItemStatus(j);
		theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);

		if( nUSStatus ==  RESETTEST )
		{
			nTestType = US_TEST_TYPE;
			bFound = TRUE;
		}

		if( nUDCStatus == RESETTEST )
		{
			nTestType |= UDC_TEST_TYPE;
			bFound = TRUE;
		}

		if( bFound == TRUE )
		{
			nItemIndex = (int)j;
			return GetItemTaskID(j);
		}
		
	}

	return -1;

}

void CTodayList::CmdResetType( int nResetType )
{
	int         nCurItem;
	ULONG       nStatus, nID;
	TASK_INFO   task_info = {0};
	CMainFrame  *pFrame = (CMainFrame*)theApp.GetMainWnd();
	StatusEnum  nUDCStatus, nUSStatus;


	nCurItem = GetOnlyOneSelected();
	if( GetCurSelIndex() != nCurItem ) //如果全局缓存的信息并不是当前的数据则返回
		return;

// 	if( SpecialTest((CSpecialTestType)nResetType) == false )
// 	{
// 		DBG_MSG("SpecialTest fail...\n");
// 		return;
// 	}

	nStatus = GetItemStatus(nCurItem);
	nID     = GetItemTaskID(nCurItem);
	theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);
	GetCurTaskInfo(task_info);

	if( nResetType & US_TEST_TYPE )
	{
		nUSStatus = RESETTEST;
		ZeroMemory(&task_info.us_info, sizeof(task_info.us_info));
		Access_UpdateTaskInfo(US_TYPE_INFO, nID, &task_info);
		ShowStatus(nCurItem, 1, nUSStatus);
		DelRecGrap(nID);
		THUIInterface_ShowTaskIcon(0xffffffff, FALSE); //强制刷新目的,清除列表
		
	}

	if( nResetType & UDC_TEST_TYPE )
	{
		nUDCStatus = RESETTEST;
		ZeroMemory(&task_info.udc_info, sizeof(task_info.udc_info));
		Access_UpdateTaskInfo(UDC_TYPE_INFO, nID, &task_info);
		ShowStatus(nCurItem, 2, nUDCStatus);
	}

	pFrame->m_wndResult.m_WndPropList.SetDefaultValue(nResetType);
	nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
	ResetItemMaskCode(nCurItem, (int)nID, nStatus); //只将复测做为临时状态,不更新到数据库中去

	if( nResetType & US_TEST_TYPE )
		nUSStatus = NEW;
	if( nResetType & UDC_TEST_TYPE )
		nUDCStatus = NEW;

	nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
	task_info.main_info.nState = nStatus;
	Access_UpdateTaskStatus(nID, nStatus);
	UpdateStatusToCache(nStatus);
	DBG_MSG(("FF:::ID:%d , SN:%d/n"),task_info.main_info.nID,task_info.main_info.nSN);
	UpdateCurTaskInfo(task_info, ALL_TYPE_INFO);
	theApp.SetTaskStatus(nID, nResetType);
}

void CTodayList::DelRecGrap( ULONG nTaskID )
{
	REC_GRAP_RECORD   rc = {0};
	char buff[42] = { 0 };
	_itoa(nTaskID, buff, 10);
	THInterface_DeleteRecCellInfo((PVOID)buff, &rc, NULL, TRUE);

	TCHAR RecPicPath[MAX_PATH];
	GetModuleFileName(NULL, RecPicPath, MAX_PATH);
	PathRemoveFileSpec(RecPicPath);
	PathAppend(RecPicPath, _T("RecGrapReslut"));
	
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION  pinfo = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	TCHAR CmdLine[1024];
	_sntprintf_s(CmdLine, 1024, _T("c:\\windows\\system32\\cmd.exe /c rd /S /Q \"%s\\%d\""), RecPicPath, nTaskID);
	DBG_MSG("%s", WstringToString(CmdLine).c_str());

	CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pinfo);
	WaitForSingleObject(pinfo.hProcess, INFINITE);
	CloseHandle(pinfo.hProcess);
	CloseHandle(pinfo.hThread);
}

BOOL CTodayList::IsAutoHideMode()
{
	return TRUE;

}

void CTodayList::OnContextMenu( CWnd* /*pWnd*/, CPoint point )
{

}

// void CTodayList::OnKillFocus(CWnd* pNewWnd)
// {
// 	CDockablePane::OnKillFocus(pNewWnd);
// // 	m_nSelItem = GetIndexByTaskID(curSelectID);
// // 	m_List.SetItemState(m_nSelItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
// 
// 	// TODO:  在此处添加消息处理程序代码
// }

void CTodayList::OnEditKillFocus()
{
	m_BarcodeEdit.ShowWindow(SW_HIDE);//隐藏编辑框

	if (m_bEscEdit)
	{
		m_bEscEdit = FALSE;
		return;
	}

	CString str;
	m_BarcodeEdit.GetWindowText(str);//取得编辑框的内容
	CString strOldText = m_List.GetItemText(m_row, m_column);
	if (strOldText == str)//编辑框内容变化，编辑框失去焦点时自动换行，没有变化就不换行
	{
		return;
	}
	//将该内容更新到CListCtrl中
	m_List.SetItemText(m_row, m_column, str);
	
	//更新控件条码到数据库
	TASK_INFO task_info = {0};
	ULONG taskId = GetItemTaskID(m_row);

	if (Access_GetTaskInfo(taskId, USER_TYPE_INFO, &task_info) != STATUS_SUCCESS)
		return;
	strncpy_s(task_info.main_info.sCode, sizeof(task_info.main_info.sCode) - 1, WstringToString(str.GetBuffer()).c_str(), sizeof(task_info.main_info.sCode) - 1);
	if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
		return;

	int nCount = m_List.GetItemCount();
	int nTopIndex = m_List.GetTopIndex();
	int nCurPageLastIndex = 0;
	CRect rt, rtList;
	m_List.GetClientRect(rtList);
	m_List.GetItemRect(m_row, rt, LVIR_BOUNDS);
	nCurPageLastIndex = nTopIndex + rtList.Height() / rt.Height();
	DBG_MSG("当前行:%d,当前最后一个元素的序号:%d\n", m_row, nCurPageLastIndex);
	while (m_row < nCount - 1 && m_row < nCurPageLastIndex)
	{
		m_row += 1;
		CString nextItemStr = m_List.GetItemText(m_row, m_column);
		if (nextItemStr.IsEmpty())
		{
			// 发送列表点击事件
			// win10下收不到此消息,所以采用发送自定义消息的方式
			/*NMLISTVIEW nmListView;
			nmListView.iItem = m_row; //告诉响应函数选中的是第几行，如果为-1则为不选中
			nmListView.iSubItem = m_column;
			nmListView.hdr.code = NM_CLICK;
			nmListView.hdr.idFrom = m_List.GetDlgCtrlID();
			nmListView.hdr.hwndFrom = m_List.GetSafeHwnd();
			::PostMessage(this->GetSafeHwnd(), WM_NOTIFY, (WPARAM)m_List.GetDlgCtrlID(), (LPARAM)&nmListView);*/
			PostMessage(WM_NEXTBARCODE, (WPARAM)m_row, (LPARAM)m_column);
			break;
		}
	}
}

void CTodayList::CountRBCSize(CRect rbcSize, std::map<int, int> & RBCSize)
{
	int count = 1,isExist = 0;
	int width = rbcSize.Width();
	int height = rbcSize.Height();
	int diameter = (width < height ? width : height);

	isExist = RBCSize.count(diameter);
	if (isExist == 0)
	{
		RBCSize.insert(std::pair<int, int>(diameter, count));
		DBG_MSG("debug:insert diameter=%d,count = %d", diameter, count);
	}
	else
	{
		count = RBCSize.at(diameter);
		RBCSize.at(diameter) = ++count;
		DBG_MSG("debug: diameter[%d].count = %d", diameter, count);
	}
}

BOOL CTodayList::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
		{
			if (GetFocus() == GetDlgItem(IDC_EDIT_BARCODE))
			{
				this->SetFocus();
				return true;
			}
		}
		else if (VK_ESCAPE== pMsg->wParam)
		{
			if (GetFocus() == GetDlgItem(IDC_EDIT_BARCODE))
			{
				m_bEscEdit = TRUE;
				this->SetFocus();
				return true;
			}
		}
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}

void CTodayList::OnSuccess(unsigned int s, unsigned long taskID, char *data, unsigned short len, void *userdata)
{
	DBG_MSG("OnSuccess[ID:%lu]\n", taskID);
	if (0 == theApp.m_lsx.PostData(s, data, len))
	{
		DBG_MSG("发送数据错误[ID:%lu]\n", taskID);
	}
	else
	{
		REPORT_STATUS1   ReportStatus = { 0 };
		ReportStatus.nTaskID = taskID;
		ReportStatus.bLis = 1;
		ReportStatus.report_mask = REPORT_LIS_MASK;
		Access_SetReportStatus(&ReportStatus);
		UpdateLisStatusOnUI(taskID);
	}
	SAFE_RELEASE_ARRAY(data);
}

void CTodayList::OnFail(unsigned long taskID, char *data, unsigned short len, int nOperationType, void *userdata)
{
	if (nOperationType == NIO_ERR_C2S_CONNECT)
	{
		DBG_MSG("连接服务器错误[%s:%u taskID:%lu]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port, taskID);
	}
	else if (nOperationType == NIO_ERR_C2S_SEND)
	{
		DBG_MSG("发送数据到服务器错误[%s:%u taskID:%lu]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port, taskID);
	}
}


void CTodayList::OnOperateGetinfoFromLis()
{
	int        nCounter, j;
	POSITION   pos;

	nCounter = m_List.GetSelectedCount();

	if (nCounter == 0)
		return;

	pos = m_List.GetFirstSelectedItemPosition();
	int nItem = m_List.GetNextSelectedItem(pos);
	CString* strBarcode = new CString;
	std::shared_ptr<CString> p(strBarcode);
	*p = m_List.GetItemText(nItem, 7);
	int nTaskId = _ttoi(m_List.GetItemText(nItem, 0));

	if (theApp.m_bIsLiushuixain && !(*p).IsEmpty())
	{
		
		auto f = std::async(std::launch::async, [=](std::shared_ptr<CString> tSp){
			std::string barcode = WstringToString(tSp->GetBuffer());
			// 异步向LIS服务器请求条码信息
			// 1.根据条码从审核平台获取病人信息
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
				if (0 != theApp.m_lsx.CreateAsyncTcpClient(
					theApp.m_stDevPipelineInfo.remote.host,
					theApp.m_stDevPipelineInfo.remote.port,
					(IConnectListener*)(CMainFrame*)(AfxGetApp()->GetMainWnd()), 
					nTaskId, pkg, info.nPackageSize, this))
				{
					DBG_MSG("获取病人信息，创建TCP客端失败![%s:%d]\n", theApp.m_stDevPipelineInfo.remote.host, theApp.m_stDevPipelineInfo.remote.port);
				}
				DBG_MSG("获取病人信息2  :%s\n", barcode.c_str());
			}
			//SAFE_RELEASE_ARRAY(pBuf);
		},p);
	}
}


void CTodayList::OnOperate()
{
	int        nOneIndex = GetOnlyOneSelected(), nStatus;
	ULONG      nCurTaskID;
	StatusEnum nUSStatus, nUDCStatus;
	CMainFrame *pFrame;

	if (nOneIndex == -1)
		return;

	nCurTaskID = GetItemTaskID(nOneIndex);
	nStatus = GetItemStatus(nOneIndex);
	theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);

	//对于审核尿沉渣为新建,识别,检测状态,都不能进行审核和显示图片
	DBG_MSG("当前镜检状态为%d,若为1,2,3则返回\n", nUSStatus);
	if (nUSStatus == TEST || nUSStatus == PRO || nUSStatus == NEW)
	{
		if (THUIInterface_GetTaskID() != 0xffffffff) //如果细胞图表中有细胞,则必须清除
			THUIInterface_ShowTaskIcon(0xffffffff);
		return;
	}

	if (nUSStatus != CHECKED)
		return;

	//更新为未审核状态
	nUSStatus = FINISH;
	nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
	if (Access_UpdateTaskStatus(nCurTaskID, nStatus) == STATUS_SUCCESS)
	{
		ResetItemMaskCode(nOneIndex, nCurTaskID, nStatus);
		ShowStatus(nOneIndex, 1, nUSStatus);
		if (GetCurSelIndex() == nOneIndex)
			UpdateStatusToCache(nStatus);
	}
}
