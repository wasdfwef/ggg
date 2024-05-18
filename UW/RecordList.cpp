// RecordList.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "RecordList.h"
#include "Include/Common/String.h"

// CRecordList
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

IMPLEMENT_DYNAMIC(CRecordList, CDockablePane)

CRecordList::CRecordList()
{

}

CRecordList::~CRecordList()
{
}


BEGIN_MESSAGE_MAP(CRecordList, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_RECORD, &CRecordList::OnNMRClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FIND_RECORD_REVISE, ID_FIND_RECORD_PREVIEW, &CRecordList::OnUpdateOperate)
	ON_COMMAND_RANGE(ID_FIND_RECORD_REVISE, ID_FIND_RECORD_PREVIEW, &CRecordList::OnOperate)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RECORD, &CRecordList::OnNMDBLClick)
END_MESSAGE_MAP()



// CRecordList 消息处理程序



int CRecordList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//创建
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_recordList.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER
		, rectDummy, this, IDC_LIST_RECORD))
	{
		OutputDebugString(_T("未能创建查询结果列表"));
		return -1;     
	}

	m_recordList.SetExtendedStyle(m_recordList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	InitList();
	return 0;
}

void CRecordList::InitList( void )
{
	m_recordList.InsertColumn(0,GetIdsString(_T("list_record"),_T("nid")).c_str(),0,0);
	m_recordList.InsertColumn(1,GetIdsString(_T("list_record"),_T("nattr")).c_str(),0,50);
	m_recordList.InsertColumn(2,GetIdsString(_T("list_record"),_T("nsn")).c_str(),0,50);
	m_recordList.InsertColumn(3,GetIdsString(_T("list_record"),_T("barcode")).c_str(),0,50);
	m_recordList.InsertColumn(4,GetIdsString(_T("list_record"),_T("sname")).c_str(),0,40);
	m_recordList.InsertColumn(5,GetIdsString(_T("list_record"),_T("nsex")).c_str(),0,40);
	m_recordList.InsertColumn(6,GetIdsString(_T("list_record"),_T("hospital")).c_str(),0,60);
	m_recordList.InsertColumn(7,GetIdsString(_T("list_record"),_T("date")).c_str(),0,40);

	m_recordList.SetExtendedStyle(m_recordList.GetExtendedStyle() | LVS_EX_FULLROWSELECT );
	m_recordList.SetExtendedStyle(m_recordList.GetExtendedStyle()| LVS_EX_GRIDLINES);
	m_recordList.ModifyStyle(0,LVS_SHOWSELALWAYS); 
}

void CRecordList::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_recordList.SetWindowPos (this, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	m_recordList.ShowWindow(TRUE);
}

void CRecordList::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
	m_recordList.SetFocus();
}

void CRecordList::UpdateRecordList(void)
{
	m_recordList.DeleteAllItems();
	
	CString strTemp=_T("");

	int nItem = -1;
	for (int i=0; i<theApp.searchtask.pSearchInfoCounts; ++i)
	{
		m_recordList.InsertItem(i,_T(""));
		m_recordList.SetItemData(i,theApp.searchtask.pSearchInfo[i].main_info.nID);

		strTemp.Format(_T("%d"),theApp.searchtask.pSearchInfo[i].main_info.nSN);
		m_recordList.SetItemText(i,1,strTemp);
		m_recordList.SetItemText(i,2,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].main_info.sFolder).c_str());
		m_recordList.SetItemText(i,3,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].main_info.sCode).c_str());
		m_recordList.SetItemText(i,4,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].pat_info.sName).c_str());


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
		m_recordList.SetItemText(i,5,strTemp);
		m_recordList.SetItemText(i,6,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].pat_info.sHospital).c_str());
		m_recordList.SetItemText(i,7,Common::CharToWChar(theApp.searchtask.pSearchInfo[i].main_info.dtDate).c_str());

	}
  
	//theApp.searchtask.pSearchInfo[i].main_info.nID

	//nMainID = (unsigned int)m_Rs.GetCollect(_T("tMain.nID"));
	//nItem = m_RecordList.InsertItem(m_RecordList.GetItemCount(), _T(""));
	//m_RecordList.SetItemData(nItem, nMainID);

		//tMain
	//nTemp = (unsigned int)m_Rs.GetCollect(_T("nSN"));

	//strTemp.Format(_T("%d"), nTemp);
	//m_recordList.SetItemText(nItem, 1, strTemp);


	//strTemp = (LPCSTR)_bstr_t (m_Rs.GetCollect(_T("sFolder")));
	//m_RecordList.SetItemText(nItem, 2, strTemp);


	//strRet = (LPCTSTR)_bstr_t(m_Rs.GetCollect(_T("sCode")));
	//strTemp.Format(_T("%s"), strRet);

	//if(strTemp.GetLength() <=0)
	//{
	//	strTemp.Format(_T(""));
	//}
	//m_RecordList.SetItemText(nItem, 3, strTemp);

	////tPat
	//strTemp = (LPCSTR)_bstr_t(m_Rs.GetCollect(_T("sName")));

	//if(strTemp.GetLength() <=0)
	//{
	//	strTemp.Format(_T(""));
	//}

	//m_RecordList.SetItemText(nItem, 4, strTemp);



	//nSex = (unsigned int)m_Rs.GetCollect(_T("nSex"));
	//if (1 == nSex){strTemp = LOAD_STRING(IDS_FINDRECORD_BOY);}
	//else if (2 == nSex){strTemp = LOAD_STRING(IDS_FINDRECORD_GIRL);}
	//else {strTemp = _T("");}
	//m_RecordList.SetItemText(nItem, 5, strTemp);


	//strTemp = (LPCSTR)_bstr_t(m_Rs.GetCollect(_T("sHospital")));
	//if(strTemp.GetLength() <=0)
	//{
	//	strTemp.Format(_T(""));
	//}
	//m_RecordList.SetItemText(nItem, 6, strTemp);


	//strTemp = (LPCSTR)_bstr_t(m_Rs.GetCollect(_T("dtDate")));
	//if(strTemp.GetLength() <=0)
	//{
	//	strTemp.Format(_T(""));
	//}
	//m_RecordList.SetItemText(nItem, 7, strTemp);

}

void CRecordList::OnNMDBLClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//查看选中项的详细信息
	LookRecordInfo();
	*pResult = 0;
}

void CRecordList::LookRecordInfo()
{
	POSITION pos = m_recordList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nItem = m_recordList.GetNextSelectedItem(pos);
		DWORD dwData = m_recordList.GetItemData(nItem);
		theApp.GetMainWnd()->PostMessage(WM_FINDRECORD_LOOKINFO, (WPARAM)dwData, 0);
	}
}

void CRecordList::Delete()
{
	// 得到需要删除的列表索引和数据库ID

	int nItem = -1;
	unsigned int nID = 0;
	int nIDarry[]={0};
	int nItemArry[]={0};
	int j = 0;
	POSITION pos = m_recordList.GetFirstSelectedItemPosition();
	while (pos)
	{
		nItem = m_recordList.GetNextSelectedItem(pos);
		nID = m_recordList.GetItemData(nItem);
	    nIDarry[j] = nID;
		nItemArry[j] = nItem;
		++j;
	}

	// 删除列表项
	for (unsigned int nIndex = j; nIndex>0; nIndex--)
	{
		m_recordList.DeleteItem(nItemArry[nIndex-1]);
	}

	DEL_TASK *del_task = new DEL_TASK;
	if (del_task !=NULL)
	{
		delete del_task;
	}

	if (j!=0)
	{
		memcpy(del_task->pDelIDArray,nIDarry,sizeof(nIDarry));
		del_task->nCounter = j;
		del_task->bIsQC = FALSE;
		if (Access_DelTask(del_task) != STATUS_SUCCESS)
		{
			AfxMessageBox(GetIdsString(_T("list_today"),_T("state8")).c_str());
		}
	}

	//// 删除数据库项
	//TCHAR chSql[MAX_PATH] = _T("");
	//for (unsigned int nIndex = 0; nIndex < vecDeleteID.size(); nIndex++)
	//{
	//	memset(chSql, 0, sizeof(chSql));
	//	_stprintf_s(chSql, MAX_PATH, _T("SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID)\
	//									LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID)\
	//									LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID) where tMain.nID=%d"), vecDeleteID[nIndex]);
	//	m_Rs.Open(chSql, theApp.m_pConn->GetInterfacePtr(), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
	//	if (!m_Rs.IsBOF())
	//	{
	//		m_Rs.Delete();
	//		m_Rs.Update();
	//	}
	//	m_Rs.Close();
	//}

	theApp.GetMainWnd()->PostMessage(WM_FINDRECORD_DELINFO, 0, 0);

}

void CRecordList::OnOperate( UINT nID )
{
	switch (nID)
	{
	case ID_FIND_RECORD_REVISE:// 结果审核
		{
			LookRecordInfo();
			break;
		}
	case ID_FIND_RECORD_DELETE:// 删除
		{
			Delete();
			break;
		}
	case ID_FIND_RECORD_PRINT:// 打印
		{
			/*InitPrintID();
			if (theApp.m_vecPrintID.size() > 250)
			{
				AfxMessageBox(IDS_PRINT_TOO_MUCH);
				break;
			}
			((CMainFrame*)theApp.GetMainWnd())->GetUWView()->SendMessage(WM_COMMAND, ID_FILE_PRINT, NULL);*/
			break;
		}
	case ID_FIND_RECORD_PREVIEW:// 打印预览
		{
			/*InitPrintID();
			if (theApp.m_vecPrintID.size() > 250)
			{
				AfxMessageBox(IDS_PRINT_TOO_MUCH);
				break;
			}
			((CMainFrame*)theApp.GetMainWnd())->GetUWView()->SendMessage(WM_COMMAND, ID_FILE_PRINT_PREVIEW, NULL);*/
			break;
		}
	}
}

void CRecordList::OnUpdateOperate( CCmdUI *pCmdUI )
{
	CString sdbg;
	sdbg.Format(_T("CRecordList::OnUpdateOperate CP1\n"));
	OutputDebugString(sdbg);


	// 选则列数目
	int nSelCount = m_recordList.GetSelectedCount();

	sdbg.Format(_T("CRecordList::OnUpdateOperate nSelCount:%d\n"),nSelCount);
	OutputDebugString(sdbg);


	// 如果正在处理,不能删除
	int nSel = -1;
	BOOL bProtected = FALSE;
	unsigned int nID = 0;
	POSITION pos = m_recordList.GetFirstSelectedItemPosition();
	while (pos)
	{
		nSel = m_recordList.GetNextSelectedItem(pos);
		nID = m_recordList.GetItemData(nSel);
		/*for (unsigned int i = 0; i < theApp.m_vecProtectedID.size(); i++)
		{
			if (nID == theApp.m_vecProtectedID[i])
			{
				bProtected = TRUE;
			}
		}*/
	}

	CToolBar *pToolBar=(CToolBar *)GetParentFrame()->GetControlBar(IDR_MAINFRAME_256);
	CToolBarCtrl *pToolBarCtrl = NULL;
	if(pToolBar)
	{
		pToolBarCtrl=&(pToolBar->GetToolBarCtrl());
	}

	// 更新
	BOOL bEnable = FALSE;
	switch (pCmdUI->m_nID)
	{
	case ID_FIND_RECORD_REVISE:
		{
			bEnable = (theApp.loginUser.Group > 0) && (nSelCount == 1);
			break;
		}
	case ID_FIND_RECORD_DELETE:
		{
			bEnable = (theApp.loginUser.Group > 0) && (nSelCount > 0); //&& !bProtected;

			/*if(pToolBarCtrl)
			{
				pToolBarCtrl->EnableButton(ID_OPERATE_DELETE,FALSE);
			}*/
			break;
		}
	case ID_FIND_RECORD_PRINT:
		{
			bEnable = (theApp.loginUser.Group> 0) && (nSelCount > 0);
			break;
		}
	case ID_FIND_RECORD_PREVIEW:
		{
			bEnable = (theApp.loginUser.Group > 0) && (nSelCount > 0);
			break;
		}
	}

	sdbg.Format(_T("CRecordList::OnUpdateOperate bEnable:%d\n"),bEnable);
	OutputDebugString(sdbg);


	//  菜单状态设置
	pCmdUI->Enable(bEnable);
}

void CRecordList::OnNMRClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	CPoint point;
	GetCursorPos(&point);
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_FIND_RECORD, point.x, point.y, this, TRUE);
	*pResult = 0;
}
