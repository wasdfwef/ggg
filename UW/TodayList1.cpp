// TodayList.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "TodayList.h"
#include "Include/Common/String.h"
#include "MainFrm.h"
#include ".\\inc\\THUIInterface.h"
#include "DbgMsg.h"

#include <Shlwapi.h>
// CTodayList


#define LIS_DATE_MAX_LEN                1024*6 

#define  MODEL_PATH "Model"

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
	ZeroMemory(&m_CurTaskInfo, sizeof(m_CurTaskInfo));
	InitializeCriticalSection(&m_TaskInfo_CS);

}

CTodayList::~CTodayList()
{
}


BEGIN_MESSAGE_MAP(CTodayList, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_TODAY, &CTodayList::OnNMClick)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TODAY, &CTodayList::OnNMRClick)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TODAY, &CTodayList::OnNMDClick)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TODAY, &CTodayList::OnItemChanged)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI_RANGE(ID_OPERATE_REVISE, ID_OPERATE_SEND_TO_LIS, &CTodayList::OnUpdateOperate)
	ON_COMMAND_RANGE(ID_OPERATE_REVISE, ID_OPERATE_SEND_TO_LIS, &CTodayList::OnOperate)
	
END_MESSAGE_MAP()


int CTodayList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	m_Font.CreateFont(-8,4,0, 0, FW_NORMAL,	0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_List.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER
		, CRect(0, 0, 360, 200), this, IDC_LIST_TODAY))
	{
		TRACE0("未能创建当天列表\n");
		return -1;      // 未能创建
	}
	// 设置扩展属性
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    
	// 设置图标
	//m_List.CreateSmallImage();

	// 初始化列表
	InitList();
    FillList();
	SetBillMode(FALSE);
	return 0;
}

void CTodayList::InitList( void )
{

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

	
	m_List.InsertColumn(0,GetIdsString(_T("list_today"),_T("nid")).c_str(),0,0);
	m_List.InsertColumn(1,GetIdsString(_T("list_today"),_T("nstate1")).c_str(),0,66);
	m_List.InsertColumn(2,GetIdsString(_T("list_today"),_T("nstate2")).c_str(),0,66);
	m_List.InsertColumn(3,GetIdsString(_T("list_today"),_T("nsn")).c_str(),0,50);
	m_List.InsertColumn(4,GetIdsString(_T("list_today"),_T("nattr")).c_str(),0,50);
	m_List.InsertColumn(5,GetIdsString(_T("list_today"),_T("nrow")).c_str(),0,40);
	m_List.InsertColumn(6,GetIdsString(_T("list_today"),_T("npos")).c_str(),0,40);
	m_List.InsertColumn(7,GetIdsString(_T("list_today"),_T("sname")).c_str(),0,0);
	m_List.InsertColumn(8,GetIdsString(_T("list_today"),_T("lisstatus")).c_str(),0,70);
	m_List.InsertColumn(9,GetIdsString(_T("list_today"),_T("printstatus")).c_str(),0,70);
	m_List.InsertColumn(10,GetIdsString(_T("list_today"),_T("dtDate")).c_str(),0,105);
	m_List.InsertColumn(11,GetIdsString(_T("list_today"),_T("nalarm")).c_str(),0,105);
	


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
	m_List.ModifyStyle(0,LVS_SHOWSELALWAYS); 

//    m_List.SetItemHeight()


	   
}

void CTodayList::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
	
	m_List.SetFocus();
}

void CTodayList::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_List.SetWindowPos (this, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	m_List.ShowWindow(TRUE);
}

void CTodayList::FillList(void)
{
	TOADY_TASK_INFO1  t_today_info;



	if ( Access_GetTodayTaskInfo(&t_today_info) != STATUS_SUCCESS )
		return;

	m_nlistNum    =  t_today_info.nToadyTaskCounter;
    theApp.m_nSN  =  t_today_info.pFullTaskInfo[m_nlistNum -1].main_info.nSN + 1;
	theApp.m_nRow =  t_today_info.pFullTaskInfo[m_nlistNum -1].main_info.nRow + 1;

	if (theApp.m_nSN > MAX_SN)
	{
		theApp.m_nSN = 1;
	}

	if (theApp.m_nRow > MAX_ROW)
	{
		theApp.m_nRow = 1;
	}

	m_List.DeleteAllItems();
	m_List.SetRedraw(FALSE);

	for (int i = 0; i< m_nlistNum; ++i)
		AddTaskToList(t_today_info.pFullTaskInfo[i]);

	m_List.SetRedraw(TRUE);
	Access_FreeGetTodayTaskInfoBuf(t_today_info);
}

void CTodayList::OnNMRClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	CPoint point;
	GetCursorPos(&point);
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_OPERATE, point.x, point.y, this, TRUE);
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
	int nRow;
	StatusEnum   nUDCStatus, nUSStatus;
	COleDateTime dtDateTime;
	if((1 != nTestType) && (2 != nTestType) && (3 != nTestType))
	{
		AfxMessageBox(_T("Test Type Error."));
		return -1;
	}
	// 新建记录
	TASK_ADD taskNew = {0};


	ZeroMemory(&task_info, sizeof(task_info));

	theApp.GetStatusAtNew(nUSStatus, nUDCStatus, bTaskResult);

	//nRow = InterlockedExchangeAdd(&theApp.m_nRow, 0);
	nRow = theApp.m_nRow;

	taskNew.nPos = nPos;
	taskNew.nCha = nCha;
	taskNew.nTestType = (TestTypeEnum)nTestType;
	taskNew.nStatus =  theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);
	//taskNew.bIsQC = false;
	
	taskNew.nNo  = theApp.m_nSN++;
	if (theApp.m_nSN > MAX_SN)
	{
		theApp.m_nSN = 1;
	}

	taskNew.nRow = nRow;
	taskNew.nTestDes = TestNormal; 
	if (theApp.nAttribute != TestNormal /*|| taskNew.nPos != 11*/)   //质控
	{
		taskNew.nTestDes = theApp.nAttribute;
		theApp.nAttribute = TestNormal;
	}
	else
	{
		if (taskNew.nPos == 11)										//急诊
		{
			taskNew.nTestDes = TestEP;			
		}
	}

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

	if (Access_NewTask(&taskNew) != STATUS_SUCCESS)
		return -1;
DBG_MSG("taskNew.nTestType = %d\n",taskNew.nTestType);
	//Access_UpdateTaskStatus(taskNew.nID,taskNew.nStatus);
	AddTaskToList(taskNew.nID, task_info);

	// 状态、孔位和通道

	//FillList();
	//theApp.GetMainWnd()->PostMessage(WM_TODAYLIST_ADDINFO, nID, 0);


	//theApp.m_vecProtectedID.push_back(nID);

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
	m_List.InsertItem(nIndex, _T(""));
	strValue.Format(_T("%u"),task_info.main_info.nID);
	m_List.SetItemText(nIndex, 0, strValue);
	ShowStatus(nIndex, 1, nUSStatus);
	ShowStatus(nIndex, 2, nUDCStatus);
	strValue.Format(_T("%d"),task_info.main_info.nSN);
	m_List.SetItemText(nIndex,3,strValue);
	ShowAttribute(nIndex, task_info.main_info.nAttribute);
	strValue.Format(_T("%d"),task_info.main_info.nRow);
	m_List.SetItemText(nIndex,5,strValue);
	strValue.Format(_T("%d"),task_info.main_info.nPos);
	m_List.SetItemText(nIndex,6,strValue);
	m_List.SetItemText(nIndex,7,Common::CharToWChar(task_info.pat_info.sName).c_str());
	ShowPrintStatus(nIndex, task_info.main_info.dtPrintStatus);
	ShowLisStatus(nIndex, task_info.main_info.dtLisStatus);
	m_List.SetItemText(nIndex,10,Common::CharToWChar(task_info.main_info.dtDate).c_str());
	strValue.Format(_T("%d"),task_info.main_info.nAlarm);
	m_List.SetItemText(nIndex,11,strValue);
	ResetItemMaskCode(nIndex, task_info.main_info.nID, task_info.main_info.nState);
	return nIndex;
}

int CTodayList::AddTaskToList( int nID, TASK_INFO &task_info)
{
	
	ZeroMemory(&task_info, sizeof(task_info));

	if( Access_GetTaskInfo(nID, ALL_TYPE_INFO, &task_info) != STATUS_SUCCESS ) //这里出错说明数据库存在bug
		return -1;
	DBG_MSG("task_info.main_info.nTestType = %d\n",task_info.main_info.nTestType),
	AddTaskToList(task_info);
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
	
	strValue=GetIdsString(_T("list_today"),keyname).c_str();
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

int CTodayList::GetItemStatus( int nIndex )
{
	return  m_List.GetItemData(nIndex) >> 24;
}

void CTodayList::UpdateFinishStatus(int nID, BOOL bUSTestType, BOOL bSuccess, PVOID pContext)
{
	int          nIndex = GetIndexByTaskID(nID), nStatus;
	StatusEnum   nUSStatus, nUDCStatus;

	if( nIndex == -1 )
		return;

	nStatus = GetItemStatus(nIndex);
	theApp.ParseStatusMaskCode(nStatus, nUSStatus, nUDCStatus);

	if( bUSTestType )
	{
		theApp.UpdateTaskStatus(nID, US_TEST_TYPE);

		if( bSuccess == FALSE )
			nUSStatus  = TESTFAIL;
		else
		{
			float *cf = (float*)pContext;

			UpdateUSReslut(nID,*cf);
			if( GetCurSelIndex() == nIndex && QueryBillMode() == TRUE ) //完成的任务是当前选择的任务,并且处于审核状态下
				nUSStatus = CHECKED;
			else
				nUSStatus = FINISH;
		}

		ShowStatus(nIndex, 1, nUSStatus);
	}
	else
	{
		theApp.UpdateTaskStatus(nID, UDC_TEST_TYPE);
		if( bSuccess == FALSE )
			nUDCStatus = TESTFAIL;
		else
		{
			UpdateUDCResult((PUDC_IMPORT_INFO_EX)pContext); //此函数中需要杨振进行完善
			nUDCStatus = FINISH;
		}
		
		ShowStatus(nIndex, 2, nUDCStatus);
	}

	nStatus = theApp.MakeStatusMaskCode(nUSStatus, nUDCStatus);

	Access_UpdateTaskStatus(nID, nStatus);
	ResetItemMaskCode(nIndex, nID, nStatus);
	if( GetCurSelIndex() == nIndex ) //如果缓存中所存放的数据就是当前更新的数据,需要同时更新缓存数据
		UpdateStatusToCache(nStatus);
	
}

void CTodayList::UpdateUSReslut( int nID, float cf )
{
	int              c;
	ULONG            j;
	KSTATUS          status;
	TASK_INFO        task_info = {0};
	PTASK_REC_INFO   rec_info;
	PCELL_FULL_INFO  cell_info;

	status = THInterface_QueryRecGrapInfo((PVOID)nID, &rec_info);
	if( status != STATUS_SUCCESS )
		return;

	for( j = 0 ; j < rec_info->tGrapCounter ; j ++ )
	{
		status = THInterface_QueryRecCellInfo((PVOID)nID, rec_info->gInfo[j].rIndex,rec_info->gInfo[j].rGrapSize, &cell_info);
		if( status != STATUS_SUCCESS )
			continue;

		CountCellResult(cell_info, task_info);
		THInterface_FreeSysBuffer(cell_info);
	}

	THInterface_FreeSysBuffer(rec_info);

	c = (int)(theApp.m_b * cf * 100.0f);
	task_info.us_info.nVariableRatio = (float)c / 100.0f;


	for( j = 0 ; j < MAX_US_COUNTS ; j ++ )
	{
		if( task_info.us_info.us_node[j].us_res == 0.0f )
			continue;


		DBG_MSG("nVariableRatio = %f, task_info.us_info.us_node[%d].us_res = %f", 
			task_info.us_info.nVariableRatio,			
			j,
			task_info.us_info.us_node[j].us_res
			);


		

		task_info.us_info.us_node[j].us_res *= task_info.us_info.nVariableRatio;
		
	}
//theApp.SaveUSLog(nID,task_info);
	status = Access_UpdateTaskInfo(US_TYPE_INFO, nID, &task_info);

//theApp.SaveUSLog(nID,task_info,TRUE);
	if( status == STATUS_SUCCESS ) //更新到界面
	{
		if( IsCurSelSameAppointID(nID) == FALSE ) //如果当前用户选择的任务与更新任务ID不一致则不进行更新
			return;

		UpdateCurTaskInfo(task_info, US_TYPE_INFO);
		GetCurTaskInfo(task_info);
		theApp.GetMainWnd()->SendMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)&task_info, (LPARAM)SHOW_US_TYPE);
		if( QueryBillMode() == TRUE )
			THUIInterface_ShowTaskIcon(nID, TRUE);
	}

}

int  CTodayList::CellMapUsIndex(USHORT nCellType)
{
	if( nCellType >= CELL_RED_TYPE && nCellType <= (CELL_RED_TYPE + 44) )
		return nCellType - 1;

	return -1;
}

void CTodayList::CountCellResult( PCELL_FULL_INFO cell_info, TASK_INFO & task_info)
{
	int              nIndex;

	for( ULONG j = 0 ; j < cell_info->cCellCounter ; j ++ )
	{
		nIndex = CellMapUsIndex(cell_info->cInfo[j].cType);
		if( nIndex == -1 )
			continue;

		task_info.us_info.us_node[nIndex].us_res ++;
	}
}



void CTodayList::OnItemChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;  

	if( pNMListView->uChanged == LVIF_STATE && (pNMListView->uNewState & LVIS_SELECTED) )  
	{  

		KSTATUS    status;
		TASK_INFO  task_info = {0};

		if( GetCurSelIndex() == pNMListView->iItem )
			return;

		AutoSaveCurRecord();

		status = Access_GetTaskInfo(GetItemTaskID(pNMListView->iItem), ALL_TYPE_INFO, &task_info);
		if( status != STATUS_SUCCESS )
			return;

		InterlockedExchange(&m_CurSelIndex, pNMListView->iItem);
		UpdateCurTaskInfo(task_info, ALL_TYPE_INFO);

		if( QueryBillMode() == TRUE )
			AutoUpdateCheckStatus();
		theApp.GetMainWnd()->SendMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)&task_info, (LPARAM)(SHOW_US_TYPE | SHOW_UDC_TYPE | SHOW_USER_TYPE));
		
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

void CTodayList::UpdateCurTaskInfo( TASK_INFO &task_info, TYPE_INFO type_info )
{
	EnterCriticalSection(&m_TaskInfo_CS);

	if( type_info == USER_TYPE_INFO )
	{
		memcpy(&m_CurTaskInfo.main_info, &task_info.main_info, sizeof(task_info.main_info));
		memcpy(&m_CurTaskInfo.pat_info, &task_info.pat_info, sizeof(task_info.pat_info));
	}
	else if( type_info == US_TYPE_INFO )
		memcpy(&m_CurTaskInfo.us_info, &task_info.us_info, sizeof(task_info.us_info));
	else if( type_info == UDC_TYPE_INFO )
		memcpy(&m_CurTaskInfo.udc_info, &task_info.udc_info, sizeof(task_info.udc_info));
	else
		memcpy(&m_CurTaskInfo, &task_info, sizeof(task_info));

	LeaveCriticalSection(&m_TaskInfo_CS);
}

void CTodayList::GetCurTaskInfo( TASK_INFO &task_info )
{
	EnterCriticalSection(&m_TaskInfo_CS);
	memcpy(&task_info, &m_CurTaskInfo, sizeof(m_CurTaskInfo));
	LeaveCriticalSection(&m_TaskInfo_CS);

}

void CTodayList::SetSaveCurRecord( BOOL bSave )
{
	InterlockedExchange(&m_SaveCurRecord, bSave);

}

void CTodayList::AutoSaveCurRecord()
{
	TASK_INFO  task_info = {0};

	if( GetCurSelIndex() == -1 || IsSaveCurRecord() == FALSE )
		return;

	GetCurTaskInfo(task_info);
	KSTATUS r = Access_UpdateTaskInfo(ALL_TYPE_INFO, task_info.main_info.nID, &task_info);
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


bool CTodayList::IsWeakPositive(int Index, int Value)
{
	return IsTheGrade( Index, Value, _T("+-") );
}


bool CTodayList::IsNegative(int Index, int Value)
{
	return IsTheGrade( Index, Value, _T("-") );
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
#define 		MA_INDEX		11



void CTodayList::ModifyUDCValue(LONG *UDCValueList)
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


void CTodayList::GetUDCLogData(PUDC_IMPORT_INFO_EX udc_import, CStringA &LogData)
{
	CStringA Data;

	LogData.AppendFormat("%d\r\n", udc_import->ID);

	LogData.AppendFormat("%s: %d\r\n", "BIL", udc_import->BIL);
	LogData.AppendFormat("%s: %d\r\n", "BLD", udc_import->BLD);
	LogData.AppendFormat("%s: %d\r\n", "GLU", udc_import->GLU);
	LogData.AppendFormat("%s: %d\r\n", "KET", udc_import->KET);
	LogData.AppendFormat("%s: %d\r\n", "LEU", udc_import->LEU);
	LogData.AppendFormat("%s: %d\r\n", "MA", udc_import->MA);
	LogData.AppendFormat("%s: %d\r\n", "NIT", udc_import->NIT);
	LogData.AppendFormat("%s: %d\r\n", "PH", udc_import->PH);
	LogData.AppendFormat("%s: %d\r\n", "PRO", udc_import->PRO);
	LogData.AppendFormat("%s: %d\r\n", "SG", udc_import->SG);
	LogData.AppendFormat("%s: %d\r\n", "URO", udc_import->URO);
	LogData.AppendFormat("%s: %d\r\n\r\n\r\n", "VC", udc_import->VC);

	return;
}


void CTodayList::LogUDCResultToFile(PUDC_IMPORT_INFO_EX udc_import)
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

	GetUDCLogData(udc_import, LogData);


	SetFilePointer(LogFileHandle, 0, 0, FILE_END);
	WriteFile(LogFileHandle, LogData.GetBuffer(), LogData.GetLength(), &NumberOfBytesWritten, NULL);

end:
	if (LogFileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(LogFileHandle);
	}
}


void CTodayList::UpdateUDCResult( PUDC_IMPORT_INFO_EX udc_import )
{
	PLONG  Value = &udc_import->LEU;
	TASK_INFO taskInfo = {0};

	taskInfo.udc_info.nID = udc_import->ID;
	taskInfo.udc_info.bUdc=true;

	LogUDCResultToFile(udc_import);

	for (int i = 0;  i < MAX_UDC_COUNTS; i++)
	{
		taskInfo.udc_info.udc_node[i].udc_col = Value[i];
		if ( theApp.m_IsAutoModifyLEU && IsNeedModifyTask(udc_import->ID) )
		{
			taskInfo.udc_info.udc_node[i].udc_col = GetModifiedUDCValue(i);
			Value[i] = taskInfo.udc_info.udc_node[i].udc_col;
		}

		GetGradeResult(i, Value[i], taskInfo);
	}

	if ( IsNormalTask(udc_import->ID) )
	{
		ModifyUDCValue(Value);

		for (int i = 0;  i < MAX_UDC_COUNTS; i++)
		{
			taskInfo.udc_info.udc_node[i].udc_col = Value[i];
			GetGradeResult(i, Value[i], taskInfo);
		}
	}
/*theApp.SaveUDCLog(taskInfo.udc_info.nID,taskInfo); */   
	if( Access_UpdateTaskInfo(UDC_TYPE_INFO,udc_import->ID,&taskInfo) != STATUS_SUCCESS )
		return;
//theApp.SaveUDCLog(taskInfo.udc_info.nID,taskInfo,TRUE); 
	if( IsCurSelSameAppointID(udc_import->ID) == FALSE )
		return;
	
	UpdateCurTaskInfo(taskInfo, UDC_TYPE_INFO);
	GetCurTaskInfo(taskInfo);
	theApp.GetMainWnd()->SendMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)&taskInfo, (LPARAM)SHOW_UDC_TYPE);

}

void CTodayList::OnNMDClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	AutoUpdateCheckStatus();
}

void CTodayList::OnNMClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	if( QueryBillMode() == TRUE )
		AutoUpdateCheckStatus();

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
	if( bCheckMode == TRUE )
		m_List.ModifyStyle(0, LVS_SINGLESEL);
	else
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
	if( nUSStatus == TEST || nUSStatus == PRO || nUSStatus == NEW )
	{
		if( THUIInterface_GetTaskID() != 0xffffffff ) //如果细胞图表中有细胞,则必须清除
			THUIInterface_ShowTaskIcon(0xffffffff);
		return;
	}

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

void CTodayList::UpdateUSValueOfCurTaskInfo(USHORT nCellType,float u_res)
{
	EnterCriticalSection(&m_TaskInfo_CS);
	m_CurTaskInfo.us_info.us_node[nCellType-1].us_res = u_res;
	LeaveCriticalSection(&m_TaskInfo_CS);

}



void CTodayList::PrintReport( BOOL bPrint )
{
	TASK_INFO TaskInfo = {0};
	int          nIndex = GetOnlyOneSelected();
	char         mod_path[MAX_PATH] = "", ini_path[MAX_PATH];

	if( nIndex == -1 )
		return;

	strcpy_s(ini_path, theApp.m_szExeFolderA);
	PathAppendA(ini_path, "conf.ini");
	GetPrivateProfileStringA("temp", "print_temp", "", mod_path, MAX_PATH, ini_path);
	if( mod_path[0] == 0 )
		return;

	AutoSaveCurRecord();

	int TaskID = GetItemTaskID(nIndex);
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

	GetCurTaskInfo(TaskInfo);

	if (TaskInfo.main_info.nID != TaskID)
	{
		if ( Access_GetTaskInfo(TaskID, ALL_TYPE_INFO, &TaskInfo) != STATUS_SUCCESS )
			return;
	}

	TaskInfo.main_info.dtPrintStatus = PRINT;
	Access_UpdateTaskInfo(USER_TYPE_INFO, TaskInfo.main_info.nID, &TaskInfo);
	UpdatePrintStatusOnUI(TaskInfo.main_info.nID);
	UpdateCurTaskInfo(TaskInfo, USER_TYPE_INFO);

end:
	NULL;

}

void CTodayList::UpdateStatusToCache( ULONG nStatus )
{
	EnterCriticalSection(&m_TaskInfo_CS);
	m_CurTaskInfo.main_info.nState = nStatus;
	LeaveCriticalSection(&m_TaskInfo_CS);

}

void CTodayList::UpdateBarCode( PBARCODE_INFORMATION barcode )
{
	TASK_INFO  task_info = {0};

	if( IsCurSelSameAppointID(barcode->nTaskID) )
		GetCurTaskInfo(task_info);
	else
	{
		if( Access_GetTaskInfo(barcode->nTaskID, USER_TYPE_INFO, &task_info) != STATUS_SUCCESS )
			return;
	}

	memcpy(task_info.main_info.sCode, barcode->BarCode, MAX_BARCODE_LENS);
	if( Access_UpdateTaskInfo(USER_TYPE_INFO, barcode->nTaskID, &task_info) != STATUS_SUCCESS )
		return;

	if( IsCurSelSameAppointID(barcode->nTaskID) == FALSE )
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

	m_List.SetRedraw(FALSE);

	for( j = nCounter - 1 ; j >= 0 ; j -- )
		SendLis(pSelItem[j]);

	m_List.SetRedraw(TRUE);

	delete [] pSelItem;
	
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
	
	
	TOADY_TASK_INFO1  c_today_info;

     char           sztemp[LIS_DATE_MAX_LEN] = "";

	 AutoSaveCurRecord();
	 GetCurTaskInfo(TaskInfo);

	 if (TaskInfo.main_info.nID != nItem)
	 {
		 if ( Access_GetTaskInfo(nItem, ALL_TYPE_INFO, &TaskInfo) != STATUS_SUCCESS )
			 return;
	 }

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


		for( int j = 0; j< theApp.us_cfg_info.nUsCounts;j ++)
		{			            
		
				//if(false != lis_info.TaskInfo.us1.us_d1[i].bIsSendtoLIS)

				if(theApp.us_cfg_info.par[j].bIsDelete == TRUE)
					continue;
//					sprintf(sztemp,"%s          %10.2f (/uL)\r\n",theApp.us_cfg_info.par[j].sShortName,TaskInfo.us_info.us_node[j].us_res);
//					strcat(szSendData, sztemp);


				bool IsNeedSendLevel = false;
				for(int Index = 0; Index < sizeof(CheckResultInfoList) / sizeof(CCheckResultInfo); ++Index)
				{
					if ( 0 != stricmp(CheckResultInfoList[Index].CheckItemShortName, theApp.us_cfg_info.par[j].sShortName) )
						continue;
					
// 					std::string Level;
// 					GetCheckItemResultLevel(nSex, CheckResultInfoList[Index].CheckItem, TaskInfo.us_info.us_node[j].us_res, Level);
// 					sprintf(sztemp,"%s          %25d(%s) (/uL)\r\n",theApp.us_cfg_info.par[j].sShortName,(int)TaskInfo.us_info.us_node[j].us_res, Level.c_str());
					sprintf(sztemp,"%s          %25d (/uL)\r\n",theApp.us_cfg_info.par[j].sShortName,(int)TaskInfo.us_info.us_node[j].us_res);
					IsNeedSendLevel = true;
					break;
				}

				if (!IsNeedSendLevel)
				{
					sprintf(sztemp,"%s          %25d (/uL)\r\n",theApp.us_cfg_info.par[j].sShortName,(int)TaskInfo.us_info.us_node[j].us_res);
				}
					
				strcat(szSendData, sztemp);

				

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

		}

		
		sprintf(sztemp,"Hemosiderin               %16s\r\n","0");
		strcat(szSendData, sztemp);
	
		sprintf(sztemp,"Faeces                     %16s\r\n","0");
		strcat(szSendData, sztemp);
		
		sprintf(sztemp,"Starch                     %16s\r\n","0");
		strcat(szSendData, sztemp);
	
		sprintf(sztemp,"Fiber                     %16s\r\n","0");
		strcat(szSendData, sztemp);
	
		sprintf(sztemp,"Parasite                  %16s\r\n","0");
		strcat(szSendData, sztemp);

		// 颜色
		sprintf(sztemp,"Color                     %16s\r\n",TaskInfo.main_info.sColor);
		strcat(szSendData, sztemp);

		// 透明度
		sprintf(sztemp,"Transparency              %16s\r\n",TaskInfo.main_info.sTransparency);
		strcat(szSendData, sztemp);



	


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




		for( int j = 0; j< theApp.udc_cfg_info.nUdcCounts;j ++)
		{			
			if( theApp.udc_cfg_info.par[j].bIsDelete == TRUE )
				continue;
			sprintf(sztemp,"%s  %37s\r\n",theApp.udc_cfg_info.par[j].sShortName,TaskInfo.udc_info.udc_node[j].udc_res);
			strcat(szSendData, sztemp);
	
		}

		sprintf(sztemp, "%c\r\n", 0x03);
		strcat(szSendData, sztemp);


		DBG_MSG("%s\n", szSendData);
	// 发送
	KSTATUS Result = THInterface_SendlCCmdActionEx(0, szSendData, strlen(szSendData), NULL, 0,T_DEV_LIC_TYPE);
	DBG_MSG("THInterface_SendlCCmdActionEx in SendLis Return: %d", Result);
	if (Result == STATUS_SUCCESS)
	{
		TaskInfo.main_info.dtLisStatus = SEND_LIS;
		Result = Access_UpdateTaskInfo(USER_TYPE_INFO, TaskInfo.main_info.nID, &TaskInfo);
		UpdateLisStatusOnUI(TaskInfo.main_info.nID);
		UpdateCurTaskInfo(TaskInfo, USER_TYPE_INFO);
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

	for (int Index = 0; Index < m_List.GetItemCount(); ++Index)
	{
		if ( 0 != m_List.GetItemText(Index, 0).Compare(Value) )
			continue;

		ShowLisStatus(Index, SEND_LIS);
		return;
	}
}


void CTodayList::UpdateRecordList(void)
{
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
		m_List.InsertItem(i,_T(""));
		//m_List.SetItemData(i,theApp.searchtask.pSearchInfo[i].main_info.nID);

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
		m_List.SetItemText(i,7,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].pat_info.sName).c_str());
		ShowPrintStatus(i, theApp.searchtask.pSearchInfo[i].main_info.dtPrintStatus);
		ShowLisStatus(i, theApp.searchtask.pSearchInfo[i].main_info.dtLisStatus);
		m_List.SetItemText(i,10,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].main_info.dtDate).c_str());
		strTemp.Format(_T("%d"),theApp.searchtask.pSearchInfo[i].main_info.nAlarm);
		m_List.SetItemText(i,11,strTemp);

	}
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

	if( SpecialTest((CSpecialTestType)nResetType) == false )
	{
		DBG_MSG("SpecialTest fail...\n");
		return;
	}

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
	UpdateCurTaskInfo(task_info, ALL_TYPE_INFO);
	theApp.SetTaskStatus(nID, nResetType);
}

void CTodayList::DelRecGrap( ULONG nTaskID )
{
	REC_GRAP_RECORD   rc = {0};

	THInterface_DeleteRecCellInfo((PVOID)nTaskID, &rc, NULL, TRUE);
}

BOOL CTodayList::IsAutoHideMode()
{
	return TRUE;

}

void CTodayList::OnContextMenu( CWnd* /*pWnd*/, CPoint point )
{

}





