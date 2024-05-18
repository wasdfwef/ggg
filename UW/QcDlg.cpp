// QcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "QcDlg.h"
#include "MainFrm.h"
#include "Include/Common/String.h"
#include "DbgMsg.h"

using namespace Common;

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

// CQcDlg 对话框

IMPLEMENT_DYNAMIC(CQcDlg, CDialogEx)
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

CQcDlg::CQcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQcDlg::IDD, pParent)
	, nFlag(0)
	, m_editSn(1)
{
	QCopend = TRUE;
	m_TestTypeOpendQC = 0;
    m_TestType = TestNormal;
    memset(m_qcChecked, 0, sizeof(m_qcChecked));
}

CQcDlg::~CQcDlg()
{

}

void CQcDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab);
    DDX_Text(pDX, IDC_EDIT1, m_editSn);
    DDX_Control(pDX, IDC_RADIO_UDC, m_radioUDC);
    DDX_Control(pDX, IDC_RADIO_US, m_radioUS);
    DDX_Check(pDX, IDC_CHK_US1, m_qcChecked[UsQCBy1]);
    DDX_Check(pDX, IDC_CHK_US2, m_qcChecked[UsQCBy2]);
    DDX_Check(pDX, IDC_CHK_US3, m_qcChecked[UsQCBy3]);
    DDX_Check(pDX, IDC_CHK_UDC1, m_qcChecked[UdcQCByP]);
    DDX_Check(pDX, IDC_CHK_UDC2, m_qcChecked[UdcQCByN]);
}

BEGIN_MESSAGE_MAP(CQcDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CQcDlg::OnCancel)
	ON_BN_CLICKED(IDC_CHK_UDC1, &CQcDlg::OnBnClickedQC)
    ON_BN_CLICKED(IDC_CHK_UDC2, &CQcDlg::OnBnClickedQC)
    ON_BN_CLICKED(IDC_CHK_US1,  &CQcDlg::OnBnClickedQC)
    ON_BN_CLICKED(IDC_CHK_US2,  &CQcDlg::OnBnClickedQC)
    ON_BN_CLICKED(IDC_CHK_US3,  &CQcDlg::OnBnClickedQC)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_ENABLE, &CQcDlg::OnBnClickedButton1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CQcDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &CQcDlg::OnBnClickedButtonNew)
	ON_MESSAGE(WM_USER_ADDTASKTOQC, &CQcDlg::AddTasktoQC)
    ON_BN_CLICKED(IDC_RADIO_UDC, &CQcDlg::OnBnClickedQcType)
    ON_BN_CLICKED(IDC_RADIO_US, &CQcDlg::OnBnClickedQcType)
END_MESSAGE_MAP()

// CQcDlg 消息处理程序

void CQcDlg::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CQcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    UDCI_QC = GetIdsString(_T("qc"), _T("1")).c_str();
    UDCII_QC = GetIdsString(_T("qc"), _T("2")).c_str();
    USI_QC = GetIdsString(_T("qc"), _T("3")).c_str();
    USII_QC = GetIdsString(_T("qc"), _T("4")).c_str();
    USIII_QC = GetIdsString(_T("qc"), _T("5")).c_str();

	// TODO:  在此添加额外的初始化
	SetWindowText(GetIdsString(_T("qc"),_T("title")).c_str());
    SetDlgItemText(IDC_CHK_UDC1, UDCI_QC);
    SetDlgItemText(IDC_CHK_UDC2, UDCII_QC);
    SetDlgItemText(IDC_CHK_US1, USI_QC);
    SetDlgItemText(IDC_CHK_US2, USII_QC);
    SetDlgItemText(IDC_CHK_US3, USIII_QC);
	Init();
	FillList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CQcDlg::OnBnClickedQC()
{
	if (QCopend == TRUE)
	{
		return;
	}

	UpdateData(TRUE);
    if (m_radioUDC.GetCheck())
    {
        m_tab.SetCurSel(0);
    }
    else
    {
        m_tab.SetCurSel(1);
    }
    OnTcnSelchangeTab1(NULL, NULL);
    return;
}

void CQcDlg::AddTaskToQcList(CListCtrl &listCtrl,PTASK_INFO pInfo,int nItem)
{
	
	CString       strValue;
	GetDlgItem(IDC_EDIT1)->UpdateData();
	int nIndex = listCtrl.GetItemCount();
	strValue.Format(_T("%d"), m_editSn++);
// 		m_list.DeleteAllItems();
	listCtrl.InsertItem(nIndex, _T(""));
	listCtrl.SetItemData(nIndex, pInfo->main_info.nID);
	listCtrl.SetItemText(nIndex, 0, strValue);
    strValue.Format(_T("%s"), _T("新建"));
	listCtrl.SetItemText(nIndex, 1, strValue);
    switch (pInfo->main_info.nAttribute)
	{
	case UsQCBy1:strValue.Format(_T("%s"),USI_QC); break;
	case UsQCBy2:strValue.Format(_T("%s"),USII_QC); break;
    case UsQCBy3:strValue.Format(_T("%s"), USIII_QC); break;
	case UdcQCByP:strValue.Format(_T("%s"), UDCI_QC); break;
	case UdcQCByN:strValue.Format(_T("%s"), UDCII_QC); break;
	default:
		strValue.Format(_T("%s"), _T("Normal"));
		break;
	}
	listCtrl.SetItemText(nIndex, 2, strValue);
	for (int i = 0; i < nItem; i++)
	{
		strValue.Format(_T("%s"), _T("待测试"));
		listCtrl.SetItemText(nIndex, i + 3, strValue);
	}
	listCtrl.SetItemText(nIndex, nItem + 3, CharToWChar(pInfo->main_info.dtDate).c_str());
	//	DeleTodayReportLast(pInfo);//新建之后删除那一天的其他相同质控类型得记录

    listCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM);
}

void CQcDlg::AddTaskToQcListInit(CListCtrl &listCtrl, int nID, PTASK_INFO info)
{
	int           nIndex;
	CString       strValue;

	GetDlgItem(IDC_EDIT1)->UpdateData();
	strValue.Format(L"%d", info->main_info.nSN);
	nIndex = listCtrl.GetItemCount();
	listCtrl.InsertItem(nIndex, _T(""));
	listCtrl.SetItemData(nIndex, nID);
	listCtrl.SetItemText(nIndex, 0, strValue);
    strValue.Format(_T("%s"), _T("新建"));
	m_usDlg.m_list.SetItemText(nIndex, 1, strValue);
	switch (info->main_info.nAttribute)
	{
	case UsQCBy1:strValue.Format(_T("%s"), USI_QC); break;
	case UsQCBy2:strValue.Format(_T("%s"), USII_QC); break;
    case UsQCBy3:strValue.Format(_T("%s"), USIII_QC); break;
	case UdcQCByP:strValue.Format(_T("%s"), UDCI_QC); break;
	case UdcQCByN:strValue.Format(_T("%s"), UDCII_QC); break;
	default:
        strValue.Format(_T("%s"), _T("Normal"));
		break;
	}
	listCtrl.SetItemText(nIndex, 2, strValue);

    listCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM);
}

void CQcDlg::UpDateTaskToQcUdcList(PTASK_INFO tInfo)
{
	CString  strValue;
	int index = 0;
	PTASK_INFO taskInfo = new TASK_INFO;
	memcpy(taskInfo, (PTASK_INFO)tInfo, sizeof(TASK_INFO));

	index = GetIndexByTaskID(m_udcDlg.m_list, taskInfo->main_info.nID);

    m_udcDlg.m_list.SetItemText(index, 1, _T("完成"));
	for (int i = 0; i < theApp.m_nUdcItemCount; i++)
	{
        strValue.Format(_T("%s"), (Common::CharToWChar(taskInfo->udc_info.udc_node[i].udc_res).c_str()));
		m_udcDlg.m_list.SetItemText(index, i + 3, strValue);
	}
	if (strlen(taskInfo->main_info.dtDate))
	{
		m_udcDlg.m_list.SetItemText(index, theApp.m_nUdcItemCount + 3, CharToWChar(taskInfo->main_info.dtDate).c_str());
		m_udcDlg.m_list.SetItemText(index, theApp.m_nUdcItemCount + 4, CharToWChar(taskInfo->main_info.sDiagnosis).c_str());
		m_udcDlg.m_list.SetItemText(index, theApp.m_nUdcItemCount + 5, CharToWChar(taskInfo->main_info.sSuggestion).c_str());
	}

	delete taskInfo;
}

void CQcDlg::UpDateTaskToQcUsList(PTASK_INFO tInfo)
{
	CString strValue;
	int index = 0;
	PTASK_INFO taskInfo = new TASK_INFO;
	memcpy(taskInfo, (PTASK_INFO)tInfo, sizeof(TASK_INFO));

	index = GetIndexByTaskID(m_usDlg.m_list, taskInfo->main_info.nID);

    m_usDlg.m_list.SetItemText(index, 1, _T("完成"));
	DBG_MSG("debug:质控更新质控球结果ID:%d,结果为%f", taskInfo->main_info.nID,taskInfo->us_info.us_node[CELL_QC_TYPE - 1].us_res);
    strValue.Format(taskInfo->us_info.us_node[CELL_QC_TYPE - 1].us_res>100.f ? _T("%.1f"):_T("%.2f"), taskInfo->us_info.us_node[CELL_QC_TYPE - 1].us_res);
    if (strValue.Right(2) == _T(".0")) strValue.Delete(strValue.GetLength() - 2, 2);
    else if (strValue.Right(3) == _T(".00")) strValue.Delete(strValue.GetLength() - 3, 3);
	m_usDlg.m_list.SetItemText(index, 3, strValue);
	if (strlen(taskInfo->main_info.dtDate))
		m_usDlg.m_list.SetItemText(index, 4, CharToWChar(taskInfo->main_info.dtDate).c_str());
	delete taskInfo;
}

void CQcDlg::Init()
{
	switch (theApp.nAttribute)
	{
    case TestNormal:QCopend = FALSE; break;
    case UsQCBy1:m_qcChecked[UsQCBy1] = TRUE; m_TestTypeOpendQC = 1; break;
    case UsQCBy2:m_qcChecked[UsQCBy2] = TRUE; m_TestTypeOpendQC = 1; break;
    case UsQCBy3:m_qcChecked[UsQCBy3] = TRUE; m_TestTypeOpendQC = 1; break;
    case UdcQCByP:m_qcChecked[UdcQCByP] = TRUE; m_TestTypeOpendQC = 0; break;
    case UdcQCByN:m_qcChecked[UdcQCByN] = TRUE; m_TestTypeOpendQC = 0; break;
	default:
		break;
	}

	m_tab.InsertItem(0, _T("尿干化质控"));
	m_tab.InsertItem(1, _T("尿沉渣质控"));
	//创建两个对话框  
	m_udcDlg.Create(IDD_QC_UDC, &m_tab);
	m_usDlg.Create(IDD_QC_US, &m_tab);

	//显示初始页面  
	m_udcDlg.ShowWindow(SW_SHOW);
	m_usDlg.ShowWindow(SW_HIDE);
	UpdateData(TRUE);

    m_radioUDC.SetCheck(1);
    //PostMessage(WM_COMMAND, MAKEWPARAM(IDC_RADIO_UDC, BN_CLICKED), (LPARAM)&m_radioUDC);
	OnBnClickedQcType();
	UpdateData(FALSE);
	OnBnClickedQC();
	
	CRect rect;
	GetClientRect(rect);

	theApp.m_pMainWnd->GetWindowRect(rect);
	this->MoveWindow(rect);
}

void CQcDlg::DeleTodayReportLast(PTASK_INFO pInfo)
{
	SEARCH_TASK tInfo;
	DEL_TASK    del_task;
	ZeroMemory(&tInfo, sizeof(SEARCH_TASK));
	CString strValue;
	tInfo.bIsSearch_QC_Task = true;
	char * p = strtok(pInfo->main_info.dtDate, " ");
	strcpy(tInfo.conditions.sDate_From, p);
	strcpy(tInfo.conditions.sDate_To,p);
	strcpy(tInfo.conditions.sTime_From, "00:00:00");
	strcpy(tInfo.conditions.sTime_To, "23:59:59");

	Access_SearchRecord(&tInfo);
	if (!tInfo.pSearchInfo)
	{
		return;
	}
	int index = 0;
	for (int i = 0; i < tInfo.pSearchInfoCounts; i++)
	{
		if (tInfo.pSearchInfo[i].main_info.nAttribute == pInfo->main_info.nAttribute)
		{
			if (tInfo.pSearchInfo[i].main_info.nID == pInfo->main_info.nID)
			{
				continue;
			}
			if (tInfo.pSearchInfo[i].main_info.nAttribute == UdcQCByP
				|| tInfo.pSearchInfo[i].main_info.nAttribute == UdcQCByN)
			{
				del_task.bIsQC = FALSE; //不考虑是否为质控
				del_task.pDelIDArray = (PULONG)&tInfo.pSearchInfo[i].main_info.nID;
				del_task.nCounter = 1;
				if (Access_DelTask(&del_task) != STATUS_SUCCESS)
					return;
			}
			else if (tInfo.pSearchInfo[i].main_info.nAttribute == UsQCBy1
				|| tInfo.pSearchInfo[i].main_info.nAttribute == UsQCBy2
                || tInfo.pSearchInfo[i].main_info.nAttribute == UsQCBy3)
			{
				del_task.bIsQC = FALSE; //不考虑是否为质控
				del_task.pDelIDArray = (PULONG)&tInfo.pSearchInfo[i].main_info.nID;
				del_task.nCounter = 1;
				if (Access_DelTask(&del_task) != STATUS_SUCCESS)
					return;
			}
			else
			{

			}
		}
	}
}

HBRUSH CQcDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性


	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CQcDlg::OnBnClickedButton1()
{
    UpdateData(TRUE);

    DBG_MSG("m_TestTypeOpendQC:%d, m_TestType: [%d][%d][%d][%d][%d], QCopend: %d\n", 
        m_TestTypeOpendQC, 
        m_qcChecked[UsQCBy1],
        m_qcChecked[UsQCBy2],
        m_qcChecked[UsQCBy3],
        m_qcChecked[UdcQCByP],
        m_qcChecked[UdcQCByN],
        QCopend);
	
	if (theApp.IsIdleStatus() == FALSE)
	{
        MessageBoxTimeoutA(this->GetSafeHwnd(), "请等待当前任务完成后再操作", "提示", MB_ICONWARNING | MB_OK, 0, 3 * 1000);
		return;
	}
	
	char focusIni[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, focusIni, MAX_PATH);
	PathRemoveFileSpecA(focusIni);
	PathAppendA(focusIni, "Config\\Focus.ini");
	// TODO:  在此添加控件通知处理程序代码
	//theApp.nAttribute = TestNormal;
	QCType qcType = { 0 };
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;

    if(QCopend) //已开启质控需要关闭
    {
        qcType.on_off = 0;
        qcType.QCtype = m_TestTypeOpendQC;//需要关闭的质控类型
        if (SpecialTest(qcType))//关闭质控，回溯之前的测试类型
        {
            theApp.UpdateCheckType(theApp.testTypeBeforeQC);//1干化2镜检
            theApp.nAttribute = theApp.nAttributeBeforeQC;
            WritePrivateProfileStringA("Focus", "EnableQCRec", "0", focusIni);

            GetDlgItem(IDC_BUTTON_ENABLE)->SetWindowTextW(_T("开启质控"));
            QCopend = false;

            GetDlgItem(IDC_RADIO_UDC)->EnableWindow(TRUE);
            GetDlgItem(IDC_RADIO_US)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHK_UDC1)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHK_UDC2)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHK_US1)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHK_US2)->EnableWindow(TRUE);
            GetDlgItem(IDC_CHK_US3)->EnableWindow(TRUE);
            if (m_radioUDC.GetCheck())
            {
                PostMessage(WM_COMMAND, MAKEWPARAM(IDC_RADIO_UDC, BN_CLICKED), (LPARAM)&m_radioUDC);
            }
            else
            {
                PostMessage(WM_COMMAND, MAKEWPARAM(IDC_RADIO_US, BN_CLICKED), (LPARAM)&m_radioUS);
            }
        }
        else
        {
            MessageBoxTimeout(*this, _T("质控关闭失败"), _T("提示"), MB_OK, 0, 3 * 1000);
            return;
        }
        if (1)//仅关闭质控不开启新的质控
        {
            pMain->m_wndBtnTools.myView->m_combox.EnableWindow(TRUE);
            pMain->m_wndBtnTools.myView->m_comboxUdcItem.EnableWindow(TRUE);
            pMain->m_wndBtnTools.myView->textQCTip.ShowWindow(SW_HIDE);
            pMain->m_wndBtnTools.myView->m_comboxUdcItem.EnableWindow(TRUE);
            pMain->m_wndBtnTools.myView->m_combox.EnableWindow(TRUE);
            return;
        }
    }
    else//未开启质控
    {
        bool cmpret = (
            (m_qcChecked[0] == TRUE) || (m_qcChecked[1] == TRUE)
                        || (m_qcChecked[2] == TRUE) || (m_qcChecked[3] == TRUE) || (m_qcChecked[4] == TRUE))
            ;
        if (m_TestType == TestNormal || !cmpret)
        {
            MessageBoxTimeout(*this, _T("请先选择质控类型!"), _T("提示"), MB_OK | MB_ICONINFORMATION, 0, 3 * 1000);
            return;
        }

        GetDlgItem(IDC_BUTTON_ENABLE)->SetWindowTextW(_T("关闭质控"));
        qcType.on_off = 1;
        qcType.QCtype = m_TestTypeOpendQC;
        //theApp.SetQcType(m_qcChecked);
        if (SpecialTest(qcType))
        {
            theApp.nAttributeBeforeQC = theApp.nAttribute;
            theApp.testTypeBeforeQC = theApp.GetCheckType();
            QCopend = true;
            theApp.UpdateCheckType(qcType.QCtype + 1);//1干化2镜检
            theApp.nAttribute = m_TestType;
            theApp.SetQcType(m_qcChecked);
            pMain->m_wndBtnTools.myView->m_combox.EnableWindow(FALSE);
            pMain->m_wndBtnTools.myView->m_comboxUdcItem.EnableWindow(FALSE);
            pMain->m_wndBtnTools.myView->textQCTip.ShowWindow(SW_SHOW);
            WritePrivateProfileStringA("Focus", "EnableQCRec", "1", focusIni);
            pMain->m_wndBtnTools.myView->m_combox.EnableWindow(FALSE);
            pMain->m_wndBtnTools.myView->m_comboxUdcItem.EnableWindow(FALSE);

            GetDlgItem(IDC_RADIO_UDC)->EnableWindow(FALSE);
            GetDlgItem(IDC_RADIO_US)->EnableWindow(FALSE);
            GetDlgItem(IDC_CHK_UDC1)->EnableWindow(FALSE);
            GetDlgItem(IDC_CHK_UDC2)->EnableWindow(FALSE);
            GetDlgItem(IDC_CHK_US1)->EnableWindow(FALSE);
            GetDlgItem(IDC_CHK_US2)->EnableWindow(FALSE);
            GetDlgItem(IDC_CHK_US3)->EnableWindow(FALSE);
        }
        else
        {
            MessageBoxTimeout(*this, _T("质控模式开启失败"), _T("提示"), MB_OK, 0, 3 * 1000);
            QCopend = false;
            GetDlgItem(IDC_BUTTON_ENABLE)->SetWindowTextW(_T("开启质控"));
        }

        return;
    }
}

void CQcDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect wndRect;
	if (m_tab)
	{
		this->GetClientRect(wndRect);
		wndRect.left += 180;
		wndRect.right -= 10;
		m_tab.MoveWindow(wndRect);

		m_tab.GetClientRect(wndRect);
		wndRect.top += 20;
		wndRect.bottom -= 10;
		m_udcDlg.MoveWindow(wndRect);
		m_usDlg.MoveWindow(wndRect);
	}
}

void CQcDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	switch (m_tab.GetCurSel())
	{
	case 0:
		m_udcDlg.ShowWindow(SW_SHOW);
		m_usDlg.ShowWindow(SW_HIDE);
		m_udcDlg.SetFocus();
		m_editSn = m_udcDlg.m_Sn;
		break;
	case 1:
		m_udcDlg.ShowWindow(SW_HIDE);
		m_usDlg.ShowWindow(SW_SHOW);
		m_usDlg.SetFocus();
		m_editSn = m_usDlg.m_Sn;
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}

void CQcDlg::OnBnClickedButtonNew()
{
	CInsertQcTask dlg;
	if (IDOK != dlg.DoModal())
		return;

	// 新建记录
	TASK_ADD taskNew = { 0 };

	taskNew.nPos = 0;
	taskNew.nCha = 0;
    taskNew.nTestType = ((dlg.m_curSel + 1) == UdcQCByP || (dlg.m_curSel + 1) == UdcQCByN) ? UDC_TEST_TYPE : US_TEST_TYPE;
	taskNew.nStatus = theApp.MakeStatusMaskCode(NEW, NEW);
	taskNew.nTestDes = (TestDes)(dlg.m_curSel + 1);	
	taskNew.nNo = dlg.m_InsertSn;
	taskNew.nRow = 0;
	taskNew.bLimitTaskTotalCount = (theApp.ReadSaveNums()>0);
	taskNew.nLimitTaskTotalCount = theApp.ReadSaveNums();
	
	if (Access_NewTask(&taskNew) != STATUS_SUCCESS) 
		return;
    TASK_INFO taskInfo = {};
	Access_GetTaskInfo(taskNew.nID, USER_TYPE_INFO, &taskInfo);
	strcpy(taskInfo.main_info.dtDate,WCharToChar((dlg.m_insertDate+" "+dlg.m_InsertTime).GetBuffer()).c_str());

	Access_UpdateTaskInfo(USER_TYPE_INFO, taskNew.nID, &taskInfo);

//	DeleTodayReportLast(&taskInfo);//新建之后删除那一天的其他相同质控类型得记录
//	只需要在数据库中新建，新建之后调用一次filllist就行了
// 	if (taskNew.nTestDes == UdcQCBy1 || taskNew.nTestDes == UdcQCBy2 || taskNew.nTestDes == UdcQCBy3)
// 	{
// 		AddTaskToQcList(m_udcDlg.m_list, taskNew.nID, theApp.m_nUdcItemCount);
// 		m_udcDlg.m_Sn = m_editSn++;
// 		m_udcDlg.m_list.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
// 	}
// 	else
// 	{
// 		AddTaskToQcList(m_usDlg.m_list, taskNew.nID, 1);
// 		m_usDlg.m_Sn = m_editSn++;
// 		m_usDlg.m_list.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);	
// 	}
	FillList();

    m_tab.SetCurSel(dlg.m_curSel > 2 ? 0 : 1);
    OnTcnSelchangeTab1(NULL, NULL);

    SendMessage(WM_USER_ADDTASKTOQC, 0, (LPARAM)&taskInfo);
}

afx_msg LRESULT CQcDlg::AddTasktoQC(WPARAM wParam, LPARAM lParam)
{
	DBG_MSG("debug:质控界面收到任务完成信息");
	if (wParam == 0)//新建
	{
		DBG_MSG("debug:质控界面收到新建任务信息");
		PTASK_INFO taskInfo = new TASK_INFO;
		memcpy(taskInfo, (PTASK_INFO)lParam, sizeof(TASK_INFO));
        if (taskInfo->main_info.nAttribute == UdcQCByP || taskInfo->main_info.nAttribute == UdcQCByN)
		{
            //if ((taskInfo->main_info.nAttribute == UdcQCByP && m_udcDlg.m_radioP.GetCheck())
            //    || (taskInfo->main_info.nAttribute == UdcQCByN && m_udcDlg.m_radioN.GetCheck()))
            {
                AddTaskToQcList(m_udcDlg.m_list, taskInfo, theApp.m_nUdcItemCount);
                m_udcDlg.m_list.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
            }
			m_udcDlg.m_Sn = m_editSn;
			UpdateData(FALSE);
		}	
        else if (taskInfo->main_info.nAttribute == UsQCBy1 || taskInfo->main_info.nAttribute == UsQCBy2 || taskInfo->main_info.nAttribute == UsQCBy3)
		{
            //if ((taskInfo->main_info.nAttribute == UsQCBy1 && m_usDlg.m_radio1.GetCheck())
             //   || (taskInfo->main_info.nAttribute == UsQCBy2 && m_usDlg.m_radio2.GetCheck())
             //   || (taskInfo->main_info.nAttribute == UsQCBy3 && m_usDlg.m_radio3.GetCheck()))
            {
                m_usDlg.GetDlgItem(IDC_EDIT_SHOW_CV_VALUE)->ShowWindow(SW_HIDE);
                AddTaskToQcList(m_usDlg.m_list, taskInfo, 1);
                m_usDlg.m_Sn = m_editSn;
                m_usDlg.m_list.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
            }
			m_usDlg.m_Sn = m_editSn;
			UpdateData(FALSE);
		}
			
		delete taskInfo;
		return wParam;
	}
	else if (wParam == 1)//UDC更新
	{
		DBG_MSG("debug:质控界面收到任务完成信息 UDC");
		UpDateTaskToQcUdcList((PTASK_INFO)lParam);
	}
	else if (wParam == 2)//US更新
	{
		DBG_MSG("debug:质控界面收到任务完成信息 US");
		UpDateTaskToQcUsList((PTASK_INFO)lParam);
	}
	else
	{
		
	}
}

void CQcDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialogEx::PostNcDestroy();

	delete this;
}

void CQcDlg::ShowTaskStatus(int nID, bool isUDC)
{
	CString       strValue;
	int index = 0;
	if (isUDC)
	{	
		index = GetIndexByTaskID(m_udcDlg.m_list, nID);
		strValue.Format(L"%s", _T("完成"));
		m_udcDlg.m_list.SetItemText(index, 1, strValue);
	}
	else
	{
		index = GetIndexByTaskID(m_usDlg.m_list, nID);
		strValue.Format(L"%s", _T("完成"));
		m_usDlg.m_list.SetItemText(index, 1, strValue);
	}
}

int CQcDlg::GetIndexByTaskID(CListCtrl &m_list,int nID)
{
	int  nIndex = m_list.GetItemCount() - 1;


	for (; nIndex >= 0; nIndex--)
	{
		if (m_list.GetItemData(nIndex) == nID)
			return nIndex;
	}
	return -1;
}

void CQcDlg::FillList(void)
{
	/*TOADY_TASK_INFO1  t_today_info;
	m_udcDlg.m_list.DeleteAllItems();
	m_udcDlg.m_list.SetRedraw(FALSE);

	m_usDlg.m_list.DeleteAllItems();
	m_usDlg.m_list.SetRedraw(FALSE);

	if (Access_GetTodayTaskInfo(&t_today_info) != STATUS_SUCCESS)
	{
		m_udcDlg.m_list.SetRedraw(TRUE);
		m_usDlg.m_list.SetRedraw(TRUE);
		return;
	}
		
	int m_nlistNum = t_today_info.nToadyTaskCounter;

	for (int i = 0; i < m_nlistNum; ++i)
	{
		if (t_today_info.pFullTaskInfo[i].main_info.nAttribute == UdcQCByP
			|| t_today_info.pFullTaskInfo[i].main_info.nAttribute == UdcQCByN)
		{
			AddTaskToQcListInit(m_udcDlg.m_list, t_today_info.pFullTaskInfo[i].main_info.nID, &t_today_info.pFullTaskInfo[i]);
			m_udcDlg.m_Sn = t_today_info.pFullTaskInfo[i].main_info.nSN + 1;
			AddTasktoQC((WPARAM)1, (LPARAM)&t_today_info.pFullTaskInfo[i]);
		}	
		else if (t_today_info.pFullTaskInfo[i].main_info.nAttribute == UsQCBy1
			|| t_today_info.pFullTaskInfo[i].main_info.nAttribute == UsQCBy2
            || t_today_info.pFullTaskInfo[i].main_info.nAttribute == UsQCBy3)
		{
			AddTaskToQcListInit(m_usDlg.m_list, t_today_info.pFullTaskInfo[i].main_info.nID, &t_today_info.pFullTaskInfo[i]);
			m_usDlg.m_Sn = t_today_info.pFullTaskInfo[i].main_info.nSN + 1;
			AddTasktoQC((WPARAM)2, (LPARAM)&t_today_info.pFullTaskInfo[i]);
		}
		else
		{

		}
	}
	m_udcDlg.m_list.SetRedraw(TRUE);
	m_usDlg.m_list.SetRedraw(TRUE);
	m_usDlg.m_list.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
	m_udcDlg.m_list.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
	Access_FreeGetTodayTaskInfoBuf(t_today_info);*/

    if (!m_tab.GetCurSel())//UDC
    {
        m_editSn = m_udcDlg.m_Sn;
    }
    else
    {
        m_editSn = m_usDlg.m_Sn;
    }

    UpdateData(FALSE);
}

bool CQcDlg::WsQcCtrl(bool qc, int mode, int item)
{
    bool ret = false;

    if (QCopend == true)
    {
        OnBnClickedButton1();
    }
    if (!qc)
    {
        this->ShowWindow(SW_HIDE);
        ret = (QCopend==false);
    }
    else
    {
        if (mode == 0) // 干化
        {
            m_radioUDC.SetCheck(1);
            m_radioUS.SetCheck(0);
            OnBnClickedQcType();
            m_qcChecked[UdcQCByP] = (item & 0x1) ? TRUE : FALSE;
            m_qcChecked[UdcQCByN] = (item & 0x2) ? TRUE : FALSE;
        }
        else  // 沉渣
        {
            m_radioUS.SetCheck(1);
            m_radioUDC.SetCheck(0);
            OnBnClickedQcType();
            m_qcChecked[UsQCBy1] = (item & 0x1) ? TRUE : FALSE;
            m_qcChecked[UsQCBy2] = (item & 0x2) ? TRUE : FALSE;
            m_qcChecked[UsQCBy3] = (item & 0x4) ? TRUE : FALSE;
        }
        UpdateData(FALSE);

        OnBnClickedButton1();
        ret = (QCopend == true);
    }

    return ret;
}

void CQcDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (theApp.IsIdleStatus() == FALSE || (theApp.nAttribute != TestNormal && theApp.nAttribute != TestEP))
	{
		MessageBoxTimeout(*this, _T("请在任务完成后关闭质控模式再退出"), _T("提示"), MB_ICONWARNING | MB_OK, 0, 3*1000);
		return;
	}
	this->ShowWindow(SW_HIDE);
}



void CQcDlg::OnBnClickedQcType()
{
    if (m_radioUDC.GetCheck())
    {
        m_TestType = UdcQCByP;
        m_TestTypeOpendQC = 0;
        m_tab.SetCurSel(0);
        GetDlgItem(IDC_CHK_UDC1)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHK_UDC2)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHK_US1)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_US2)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_US3)->EnableWindow(FALSE);
        m_qcChecked[UsQCBy1] = FALSE;
        m_qcChecked[UsQCBy2] = FALSE;
        m_qcChecked[UsQCBy3] = FALSE;
    }
    else if (m_radioUS.GetCheck())
    {
        m_TestType = UsQCBy1;
        m_TestTypeOpendQC = 1;
        m_tab.SetCurSel(1);
        GetDlgItem(IDC_CHK_UDC1)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_UDC2)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHK_US1)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHK_US2)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHK_US3)->EnableWindow(TRUE);
        m_qcChecked[UdcQCByP] = FALSE;
        m_qcChecked[UdcQCByN] = FALSE;
    }
    else
    {
        m_TestType = TestNormal;
    }
    OnTcnSelchangeTab1(NULL, NULL);
    UpdateData(FALSE);
}
