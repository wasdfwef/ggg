// UdcQcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UdcQcDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "UW.h"
#include "DbgMsg.h"
#include "include/common/String.h"

#define VIEW_WIDTH 300
#define VIEW_HEIGHT 200

using namespace Common;

static bool compare_id(const TASK_INFO *t1, const TASK_INFO *t2){
    return t1->main_info.nID < t2->main_info.nID;
}

// UdcQcDlg 对话框

IMPLEMENT_DYNAMIC(UdcQcDlg, CDialogEx)

UdcQcDlg::UdcQcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(UdcQcDlg::IDD, pParent)
	, m_conf(FALSE)
{
	m_Sn = 1;
	isValid = true;
	days = 30;

	m_paintView = NULL;
    m_preView = NULL;
}

UdcQcDlg::~UdcQcDlg()
{
}

void UdcQcDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list);
    DDX_Check(pDX, IDC_CHECK15, m_conf);
    DDX_Control(pDX, IDC_CHECK15, m_confCon);
    DDX_Control(pDX, IDC_TAB1, m_tab);
    DDX_Control(pDX, IDC_STATIC_PREVIEW, m_prelabel);
    DDX_Control(pDX, IDC_DATETIMEPICKER1, m_dtFrom);
    DDX_Control(pDX, IDC_DATETIMEPICKER2, m_dtTo);
    DDX_Control(pDX, IDC_RADIO_P, m_radioP);
    DDX_Control(pDX, IDC_RADIO_N, m_radioN);
}


BEGIN_MESSAGE_MAP(UdcQcDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &UdcQcDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &UdcQcDlg::OnBnClickedButtonSend)
	ON_EN_KILLFOCUS(IDC_QC_LIST_EDIT, &UdcQcDlg::OnEditKillFocus)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &UdcQcDlg::OnNMDClick)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &UdcQcDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_CHECK15, &UdcQcDlg::OnBnClickedCheck15)
	ON_BN_CLICKED(IDC_BUTTON_CONF_OK, &UdcQcDlg::OnBnClickedButtonConfOk)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &UdcQcDlg::OnTcnSelchangeTab1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, &UdcQcDlg::OnBnClickedButton2)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &UdcQcDlg::OnNMClickList1)
    ON_BN_CLICKED(IDC_RADIO_P, &UdcQcDlg::OnBnClickedRadio)
    ON_BN_CLICKED(IDC_RADIO_N, &UdcQcDlg::OnBnClickedRadio)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &UdcQcDlg::OnNMCustomdrawList)
END_MESSAGE_MAP()


// UdcQcDlg 消息处理程序

void UdcQcDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	ReControlSize();
	// TODO:  在此处添加消息处理程序代码
}

BOOL UdcQcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!m_Edit.Create(WS_CHILD | WS_VISIBLE
		, CRect(0, 0, 0, 0), this, IDC_QC_LIST_EDIT))
	{
		TRACE0("未能创建编辑框\n");
		return -1;      // 未能创建
	}
    static CFont editFont;
    editFont.CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("MS Shell Dlg"));
    m_Edit.SetFont(&editFont);
    m_list.SetFont(&editFont);

	m_paintView = new CPaintView;
	if (m_paintView)
	{
		CRect rect;
		m_tab.GetWindowRect(rect);
		m_paintView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE,  CRect(), &m_tab, AFX_IDW_PANE_LAST);
	}

	m_preView = new CPaintView;

	if (m_preView)
	{
		m_preView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,300,200), &m_prelabel, NULL);
	}
	m_prelabel.ShowWindow(SW_HIDE);

	CImageList imgList;  //为ClistCtrl设置一个图像列表，以设置行高
	imgList.Create(IDB_BITMAP3, 1, 1, RGB(255, 255, 255)); // IDB_BITMAP4 是 2*16的 所以行高16像素

	m_list.SetImageList(&imgList, LVSIL_SMALL);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_list.ModifyStyle(LVS_SHOWSELALWAYS, 0);

	int i = 0;
	m_list.InsertColumn(i++, L"序号", LVCFMT_CENTER, 60);
	m_list.InsertColumn(i++, L"状态", LVCFMT_CENTER, 60);
	m_list.InsertColumn(i++, L"质控类型", LVCFMT_CENTER, 120);
	
	int index = i;
	for (int j = 0; j < theApp.m_nUdcItemCount; j++)
	{
		if (theApp.udc_cfg_info.par[j].bIsDelete == TRUE)
			continue;
		int nDefaultWidth = 65;
		m_tab.InsertItem(index, Common::CharToWChar(theApp.udc_cfg_info.par[j].sShortName).c_str());
		m_list.InsertColumn(index++, Common::CharToWChar(theApp.udc_cfg_info.par[j].sShortName).c_str(), LVCFMT_CENTER, nDefaultWidth);
		index++;
	}
	m_list.InsertColumn(index++, L"测试时间", LVCFMT_CENTER, 150);
	m_list.InsertColumn(index++, L"批号", LVCFMT_CENTER, 120);
	m_list.InsertColumn(index++, L"有效期", LVCFMT_CENTER, 120);

	m_confCon.SetCheck(false);
	ShowConfigControl(false);
	ReadRangeFormIni();
	UpdateStringToUI();
	AddStringToComBox();
	if (!SetComboxByValueString())
	{
		MessageBox(L"靶值选择无效，请重新确认", L"提示", MB_OK);
		isValid = false;
	}
	else
	{
		isValid = true;
	}

	CTime theTime = CTime::GetCurrentTime();
	this->m_dtFrom.SetTime(&theTime);
	this->m_dtTo.SetTime(&theTime);


    m_radioP.SetCheck(1);
    GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);

    m_radioP.SetWindowText(UDCI_QC);
    m_radioN.SetWindowText(UDCII_QC);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void UdcQcDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	char lastTime[MAX_DATA_LENS] = { 0 };
    SEARCH_TASK tInfo1 = {}, tInfo2 = {};
	CString strValue;
    tInfo1.bIsSearch_QC_Task = true;
    tInfo1.conditions.nTestDes = UdcQCByP;

	COleDateTime dt;
	m_dtFrom.GetTime(dt);
	strcpy(tInfo1.conditions.sDate_From, WCharToChar(dt.Format(_T("%Y-%m-%d")).GetBuffer()).c_str());
	m_dtTo.GetTime(dt);
	strcpy(tInfo1.conditions.sDate_To, WCharToChar(dt.Format(_T("%Y-%m-%d")).GetBuffer()).c_str());

	days = calcTimeDiff(tInfo1.conditions.sDate_From, tInfo1.conditions.sDate_To) + 1;

	strcpy(tInfo1.conditions.sTime_From,"00:00:00");
	strcpy(tInfo1.conditions.sTime_To, "23:59:59");

    tInfo2 = tInfo1;
    tInfo2.conditions.nTestDes = UdcQCByN;

    m_list.DeleteAllItems();

    KSTATUS rcode1 = Access_SearchRecord(&tInfo1);
    KSTATUS rcode2 = Access_SearchRecord(&tInfo2);
    if (STATUS_SUCCESS != rcode1 || STATUS_SUCCESS != rcode2)
	{
        Access_FreeSearchRecordBuf(tInfo1);
        Access_FreeSearchRecordBuf(tInfo2);
        SendMessgeToPaint(m_tab.GetCurSel());
		return;
	}
    std::list<TASK_INFO*> tasks;
    for (int i = 0; i < tInfo1.pSearchInfoCounts; i++)
    {
        tasks.push_back(tInfo1.pSearchInfo + i);
    }
    for (int i = 0; i < tInfo2.pSearchInfoCounts; i++)
    {
        tasks.push_back(tInfo2.pSearchInfo + i);
    }
    tasks.sort(compare_id);

	int index = 0;
    for (auto itr = tasks.begin(); itr != tasks.end(); itr++)
	{
        TASK_INFO *task = *itr;
        if (task->main_info.nAttribute != UdcQCByP && task->main_info.nAttribute != UdcQCByN)
		{
			continue;
		}
        StatusEnum   nUSStatus, nUDCStatus;
        theApp.ParseStatusMaskCode(task->main_info.nState, nUSStatus, nUDCStatus);
        CString stateTxt;
        switch (nUDCStatus)
        {
        case NEW:     //新建
            stateTxt = _T("新建");
            break;
        case TEST:	   //检测
        case PRO:     //识别
            stateTxt = _T("测试中");
            break;
        case TESTFAIL:     //测试失败
        case FAIL:
            stateTxt = _T("测试失败");
            break;
        case FINISH:    //完成
        case CHECKED:   //已审核	
            stateTxt = _T("完成");
            break;
        default:
            break;
        }
		m_list.InsertItem(index,L"");
        strValue.Format(L"%d", task->main_info.nSN);
		m_list.SetItemText(index, 0, strValue);
        m_list.SetItemData(index, task->main_info.nID);
        m_list.SetItemText(index, 1, stateTxt); 
        switch (task->main_info.nAttribute)
		{
		case UdcQCByP:strValue.Format(_T("%s"), UDCI_QC); break;
		case UdcQCByN:strValue.Format(_T("%s"), UDCII_QC); break;
		default:
			strValue.Format(_T("%s"), L"Normal");
			break;
		}
		m_list.SetItemText(index, 2, strValue);

		for (int j = 0; j < theApp.m_nUdcItemCount; j++)
		{
            strValue.Format(L"%s", CharToWChar(task->udc_info.udc_node[j].udc_res).c_str());
			m_list.SetItemText(index, j + 3, strValue);
		}		
		
        strValue.Format(L"%s", CharToWChar(task->main_info.dtDate).c_str());
		m_list.SetItemText(index, theApp.m_nUdcItemCount + 3, strValue);
        strValue.Format(L"%s", CharToWChar(task->main_info.sDiagnosis).c_str());
		m_list.SetItemText(index, theApp.m_nUdcItemCount + 4, strValue);
        strValue.Format(L"%s", CharToWChar(task->main_info.sSuggestion).c_str());
		m_list.SetItemText(index, theApp.m_nUdcItemCount + 5, strValue);
		index++;
	}

	Access_FreeSearchRecordBuf(tInfo1);
    Access_FreeSearchRecordBuf(tInfo2);
	SendMessgeToPaint(m_tab.GetCurSel());
}

void UdcQcDlg::OnEditKillFocus()
{
	m_Edit.ShowWindow(SW_HIDE);//隐藏编辑框

	CString str;
	m_Edit.GetWindowText(str);//取得编辑框的内容
    if (str.IsEmpty())
    {
        return;
    }
	CString strOldText = m_list.GetItemText(m_row, m_column);

	//将该内容更新到CListCtrl中
	m_list.SetItemText(m_row, m_column, str);
	if (str == strOldText)
	{
		return;
	}
	//更新数据到数据库
	TASK_INFO task_info = { 0 };
	ULONG taskId = m_list.GetItemData(m_row);

	if (Access_GetTaskInfo(taskId, USER_TYPE_INFO, &task_info) != STATUS_SUCCESS)
		return;
	if (0 == m_column)//序号
	{
		task_info.main_info.nSN = atoi(WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else if (m_column >= 3 && m_column < theApp.m_nUdcItemCount + 3)
	{
		strcpy(task_info.udc_info.udc_node[m_column - 3].udc_res,WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(UDC_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else if (m_column == theApp.m_nUdcItemCount + 3)
	{
		strcpy(task_info.main_info.dtDate, WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else if (m_column == theApp.m_nUdcItemCount + 4)
	{
		strcpy(task_info.main_info.sDiagnosis,WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else if (m_column == theApp.m_nUdcItemCount + 5)
	{
		strcpy(task_info.main_info.sSuggestion,WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else  
	{

		return;
	}
	SendMessgeToPaint(m_tab.GetCurSel());
}

void UdcQcDlg::DeleteRecord(int nItem)
{
	ULONG       nTaskID;
	DEL_TASK    del_task;
	CUdcView   *udc_view;
	CMainFrame *pMainFrame;

	if (nItem == -1)
		return;

	nTaskID = m_list.GetItemData(nItem);


	del_task.bIsQC = FALSE; //不考虑是否为质控
	del_task.pDelIDArray = &nTaskID;
	del_task.nCounter = 1;
	if (Access_DelTask(&del_task) != STATUS_SUCCESS)
		return;

	m_list.DeleteItem(nItem);

	pMainFrame = (CMainFrame*)theApp.GetMainWnd();
	udc_view = (CUdcView*)pMainFrame->GetUWView()->GetTabWnd(THUM_UDC_TAB);
	udc_view->DeleteItem(nTaskID);
}

void UdcQcDlg::OnNMDClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CRect rc;
	if (pNMListView->iItem != -1)
	{
		m_row = pNMListView->iItem;//m_row为被选中行的行序号（int类型成员变量）
		m_column = pNMListView->iSubItem;//m_column为被选中行的列序号（int类型成员变量）
		if (m_column != 0 && m_column <3)
		{
			return;
		}
		DoSubItemEdit(m_row, m_column);
	}
}

void UdcQcDlg::DoSubItemEdit(int item, int subitem)
{
	CRect rc;
	m_list.GetSubItemRect(item, subitem, LVIR_BOUNDS, rc);//取得子项的矩形
    if (subitem == 0)
    {
        CRect firstColumnRect;
        m_list.GetHeaderCtrl()->GetItemRect(0, &firstColumnRect);
        rc.right = rc.left + firstColumnRect.Width();
    }
    rc.DeflateRect(4, 1, -1, 0);

	CString str;
	str = m_list.GetItemText(item, subitem);//取得子项的内容
    m_Edit.SetParent(&m_list);
	m_Edit.SetWindowText(str);//将子项的内容显示到编辑框中
	m_Edit.ShowWindow(SW_SHOW);//显示编辑框
	m_Edit.MoveWindow(&rc);//将编辑框移动到子项上面，覆盖在子项上
	m_Edit.SetFocus();//使编辑框取得焦点
	m_Edit.CreateSolidCaret(1, rc.Height() - 2);//创建一个光标
	m_Edit.ShowCaret();//显示光标
	m_Edit.SetSel(-1);//使光标移到最后面
}

void UdcQcDlg::ReControlSize()
{
	if (!m_list)
	{
		return;
	}
	CRect wndRect;

	this->GetClientRect(wndRect);
	wndRect.bottom /= 3;
	wndRect.right -= 10;
	m_list.MoveWindow(wndRect);

	CRect control(wndRect);
	control.top = wndRect.top + wndRect.Height();
	int x = 0;
	int y = 120;

    GetDlgItem(IDC_RADIO_P)->SetWindowPos(NULL, control.left + x + 20, control.top + 10, 60, 20, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_RADIO_N)->SetWindowPos(NULL, control.left + x + 100, control.top + 10, 60, 20, SWP_NOSIZE | SWP_NOACTIVATE);

    control.top += 20;

	GetDlgItem(IDC_DATETIMEPICKER1)->SetWindowPos(NULL, control.left + x ,		control.top + 10, 80, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC)->SetWindowPos(NULL, control.left + x + 160,			control.top + 12, 80, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_DATETIMEPICKER2)->SetWindowPos(NULL, control.left + x + 180, control.top + 10, 80, 20, SWP_NOSIZE | SWP_NOACTIVATE);

	GetDlgItem(IDC_BUTTON1)->SetWindowPos(NULL, control.left + (x += y) + 260,			control.top + 10, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON2)->SetWindowPos(NULL, control.left + (x += y) + 260,			control.top + 10, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_BUTTON_SEND)->SetWindowPos(NULL, control.left + (x += y) + 260, control.top + 10, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON_DEL)->SetWindowPos(NULL, control.left + (x += y) + 260,		control.top + 10, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_CHECK15)->SetWindowPos(NULL, control.left + (x += y) + 260,			control.top + 10 + 2, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	
	y = 60;
	int offset = 30;
	x -= y;
	y = 60;
	GetDlgItem(IDC_STATIC11)->SetWindowPos(NULL, control.left + x + 260 + 60,			control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT11)->SetWindowPos(NULL, control.left +	x + 260 + 90,			control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO51)->SetWindowPos(NULL, control.left + x + 260 + 340,			control.top + 10 + offset , 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON_CONF_OK)->SetWindowPos(NULL, control.left + x + 260 + 340,	control.top + 10, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);

	offset += 30;
	GetDlgItem(IDC_STATIC12)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT12)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO52)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC13)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT13)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO53)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC14)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT14)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO54)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC15)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT15)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO55)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC16)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT16)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO56)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC17)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT17)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO57)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC18)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT18)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO58)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC19)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT19)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO59)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC20)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT20)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO60)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC21)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT21)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO61)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC22)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT22)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO62)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC23)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT23)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO63)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);

	offset += 30;
	GetDlgItem(IDC_STATIC24)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + TOP_OFSET, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT24)->SetWindowPos(NULL, control.left + x + 260 + 90, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_COMBO64)->SetWindowPos(NULL, control.left + x + 260 + 340, control.top + 10 + offset, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC_TIP)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 10 + offset + 2, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);


// 	GetDlgItem(IDC_CHECK1)->SetWindowPos(NULL, control.left + x , control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK2)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK3)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK4)->SetWindowPos(NULL, control.left + ( x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK5)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK6)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK7)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK8)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK9)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK10)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK11)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK12)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK13)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 	GetDlgItem(IDC_CHECK14)->SetWindowPos(NULL, control.left + (x += y), control.top + 40, 40, 20, SWP_NOSIZE | SWP_NOACTIVATE);
// 
// 
	GetDlgItem(IDC_STATIC11)->SetWindowText(L"LEU");
	GetDlgItem(IDC_STATIC12)->SetWindowText(L"NIT");
	GetDlgItem(IDC_STATIC13)->SetWindowText(L"URO");
	GetDlgItem(IDC_STATIC14)->SetWindowText(L"PRO");
	GetDlgItem(IDC_STATIC15)->SetWindowText(L"PH");
	GetDlgItem(IDC_STATIC16)->SetWindowText(L"BLD");
	GetDlgItem(IDC_STATIC17)->SetWindowText(L"SG");
	GetDlgItem(IDC_STATIC18)->SetWindowText(L"KET");
	GetDlgItem(IDC_STATIC19)->SetWindowText(L"BIL");
	GetDlgItem(IDC_STATIC20)->SetWindowText(L"GLU");
	GetDlgItem(IDC_STATIC21)->SetWindowText(L"VC");
	GetDlgItem(IDC_STATIC22)->SetWindowText(L"MA");
	GetDlgItem(IDC_STATIC23)->SetWindowText(L"CRE");
	GetDlgItem(IDC_STATIC24)->SetWindowText(L"CA");
	GetWindowRect(wndRect);
	m_tab.SetWindowPos(NULL, control.left, control.top + 40, x + 260 + 40, wndRect.Height() - control.bottom - 60, SWP_NOACTIVATE);

	m_tab.GetClientRect(wndRect);
	
    if (m_paintView && m_paintView->m_hWnd )
	{
        m_paintView->MoveWindow(wndRect);
	}
}


void UdcQcDlg::AddStringToComBox(CComboBox* tComb, string tStr)
{
	vector<string> v;
	SplitString(tStr, v, ";");//可按多个字符来分隔;
	for (vector<string>::size_type i = 0; i != v.size(); ++i)
		tComb->AddString(CharToWChar(v[i]).c_str());
	v.clear();
}

void UdcQcDlg::AddStringToComBox()
{
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO51), rangeStr.at(0));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO52), rangeStr.at(1));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO53), rangeStr.at(2));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO54), rangeStr.at(3));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO55), rangeStr.at(4));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO56), rangeStr.at(5));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO57), rangeStr.at(6));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO58), rangeStr.at(7));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO59), rangeStr.at(8));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO60), rangeStr.at(9));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO61), rangeStr.at(10));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO62), rangeStr.at(11));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO63), rangeStr.at(12));
	AddStringToComBox((CComboBox*)GetDlgItem(IDC_COMBO64), rangeStr.at(13));
}

void UdcQcDlg::OnBnClickedButtonDel()
{
	// TODO:  在此添加控件通知处理程序代码
	int        nCounter, j;
	PLONG      pSelItem;
	POSITION   pos;

	nCounter = m_list.GetSelectedCount();

	if (nCounter == 0)
		return;

	pSelItem = new LONG[nCounter];
	if (pSelItem == NULL)
		return;

	pos = m_list.GetFirstSelectedItemPosition();

	for (j = 0; j < nCounter; j++)
		pSelItem[j] = m_list.GetNextSelectedItem(pos);

	m_list.SetRedraw(FALSE);

	for (j = nCounter - 1; j >= 0; j--)
		DeleteRecord(pSelItem[j]);

	m_list.SetRedraw(TRUE);

	delete[] pSelItem;
}

void UdcQcDlg::OnBnClickedCheck15()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_confCon.GetCheck())
		ShowConfigControl();
	else
		ShowConfigControl(false); 
}

void UdcQcDlg::ShowConfigControl(bool isShow /*= true*/)
{
	int flag;
	if (isShow)
		flag = SW_SHOW;
	else
		flag= SW_HIDE;
	GetDlgItem(IDC_STATIC11)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT11)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO51)->ShowWindow(flag);
	GetDlgItem(IDC_BUTTON_CONF_OK)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC12)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT12)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO52)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC13)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT13)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO53)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC14)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT14)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO54)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC15)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT15)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO55)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC16)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT16)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO56)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC17)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT17)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO57)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC18)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT18)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO58)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC19)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT19)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO59)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC20)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT20)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO60)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC21)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT21)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO61)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC22)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT22)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO62)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC23)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT23)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO63)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC24)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT24)->ShowWindow(flag);
	GetDlgItem(IDC_COMBO64)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC_TIP)->ShowWindow(flag);
}


void UdcQcDlg::OnBnClickedButtonConfOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateUIToString();
	WriteRangeToIni();
	if (!SetComboxByValueString())
	{ 
		MessageBox(L"靶值选择无效，请重新确认", L"提示", MB_OK);
		isValid = false;
	}
	else
	{
		isValid = true;
	}
	SendMessgeToPaint(m_tab.GetCurSel());
}

void UdcQcDlg::ReadRangeFormIni()
{
	char ConfigFile[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, ConfigFile, MAX_PATH);
	PathRemoveFileSpecA(ConfigFile);
	PathAppendA(ConfigFile, "Config\\qc.ini");
	GetPrivateProfileStringA("LEU", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("NIT", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("URO", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("PRO", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("PH", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("BLD", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("SG", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("KET", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("BIL", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("GLU", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("VC", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("MA", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("CRE", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);
	GetPrivateProfileStringA("CA", "range","", temp, MAX_PATH, ConfigFile);
	rangeStr.push_back(temp);

	GetPrivateProfileStringA("LEU", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("NIT", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("URO", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("PRO", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("PH", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("BLD", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("SG", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("KET", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("BIL", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("GLU", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("VC", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("MA", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("CRE", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
	GetPrivateProfileStringA("CA", "value", "", temp, MAX_PATH, ConfigFile);
	valueStr.push_back(temp);
}

void UdcQcDlg::WriteRangeToIni()
{
	char ConfigFile[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, ConfigFile, MAX_PATH);
	PathRemoveFileSpecA(ConfigFile);
	PathAppendA(ConfigFile, "Config\\qc.ini");
	
	WritePrivateProfileStringA("LEU", "range", rangeStr.at(0).c_str(), ConfigFile);
	WritePrivateProfileStringA("NIT", "range", rangeStr.at(1).c_str(), ConfigFile);
	WritePrivateProfileStringA("URO", "range", rangeStr.at(2).c_str(), ConfigFile);
	WritePrivateProfileStringA("PRO", "range", rangeStr.at(3).c_str(), ConfigFile);
	WritePrivateProfileStringA("PH", "range", rangeStr.at(4).c_str(), ConfigFile);
	WritePrivateProfileStringA("BLD", "range", rangeStr.at(5).c_str(), ConfigFile);
	WritePrivateProfileStringA("SG", "range", rangeStr.at(6).c_str(), ConfigFile);
	WritePrivateProfileStringA("KET", "range", rangeStr.at(7).c_str(), ConfigFile);
	WritePrivateProfileStringA("BIL", "range", rangeStr.at(8).c_str(), ConfigFile);
	WritePrivateProfileStringA("GLU", "range", rangeStr.at(9).c_str(), ConfigFile);
	WritePrivateProfileStringA("VC", "range", rangeStr.at(10).c_str(), ConfigFile);
	WritePrivateProfileStringA("MA", "range", rangeStr.at(11).c_str(), ConfigFile);
	WritePrivateProfileStringA("CRE", "range", rangeStr.at(12).c_str(), ConfigFile);
	WritePrivateProfileStringA("CA", "range", rangeStr.at(13).c_str(), ConfigFile);

	WritePrivateProfileStringA("LEU", "value", valueStr.at(0).c_str(), ConfigFile);
	WritePrivateProfileStringA("NIT", "value", valueStr.at(1).c_str(), ConfigFile);
	WritePrivateProfileStringA("URO", "value", valueStr.at(2).c_str(), ConfigFile);
	WritePrivateProfileStringA("PRO", "value", valueStr.at(3).c_str(), ConfigFile);
	WritePrivateProfileStringA("PH", "value", valueStr.at(4).c_str(), ConfigFile);
	WritePrivateProfileStringA("BLD", "value", valueStr.at(5).c_str(), ConfigFile);
	WritePrivateProfileStringA("SG", "value", valueStr.at(6).c_str(), ConfigFile);
	WritePrivateProfileStringA("KET", "value", valueStr.at(7).c_str(), ConfigFile);
	WritePrivateProfileStringA("BIL", "value", valueStr.at(8).c_str(), ConfigFile);
	WritePrivateProfileStringA("GLU", "value", valueStr.at(9).c_str(), ConfigFile);
	WritePrivateProfileStringA("VC", "value", valueStr.at(10).c_str(), ConfigFile);
	WritePrivateProfileStringA("MA", "value", valueStr.at(11).c_str(), ConfigFile);
	WritePrivateProfileStringA("CRE", "value", valueStr.at(12).c_str(), ConfigFile);
	WritePrivateProfileStringA("CA", "value", valueStr.at(13).c_str(), ConfigFile);
}

void UdcQcDlg::UpdateStringToUI()
{
	GetDlgItem(IDC_EDIT11)->SetWindowText(Common::CharToWChar(rangeStr.at(0)).c_str());
	GetDlgItem(IDC_EDIT12)->SetWindowText(Common::CharToWChar(rangeStr.at(1)).c_str());
	GetDlgItem(IDC_EDIT13)->SetWindowText(Common::CharToWChar(rangeStr.at(2)).c_str());
	GetDlgItem(IDC_EDIT14)->SetWindowText(Common::CharToWChar(rangeStr.at(3)).c_str());
	GetDlgItem(IDC_EDIT15)->SetWindowText(Common::CharToWChar(rangeStr.at(4)).c_str());
	GetDlgItem(IDC_EDIT16)->SetWindowText(Common::CharToWChar(rangeStr.at(5)).c_str());
	GetDlgItem(IDC_EDIT17)->SetWindowText(Common::CharToWChar(rangeStr.at(6)).c_str());
	GetDlgItem(IDC_EDIT18)->SetWindowText(Common::CharToWChar(rangeStr.at(7)).c_str());
	GetDlgItem(IDC_EDIT19)->SetWindowText(Common::CharToWChar(rangeStr.at(8)).c_str());
	GetDlgItem(IDC_EDIT20)->SetWindowText(Common::CharToWChar(rangeStr.at(9)).c_str());
	GetDlgItem(IDC_EDIT21)->SetWindowText(Common::CharToWChar(rangeStr.at(10)).c_str());
	GetDlgItem(IDC_EDIT22)->SetWindowText(Common::CharToWChar(rangeStr.at(11)).c_str());
	GetDlgItem(IDC_EDIT23)->SetWindowText(Common::CharToWChar(rangeStr.at(12)).c_str());
	GetDlgItem(IDC_EDIT24)->SetWindowText(Common::CharToWChar(rangeStr.at(13)).c_str());

	GetDlgItem(IDC_COMBO51)->SetWindowText(Common::CharToWChar(valueStr.at(0)).c_str());
	GetDlgItem(IDC_COMBO52)->SetWindowText(Common::CharToWChar(valueStr.at(1)).c_str());
	GetDlgItem(IDC_COMBO53)->SetWindowText(Common::CharToWChar(valueStr.at(2)).c_str());
	GetDlgItem(IDC_COMBO54)->SetWindowText(Common::CharToWChar(valueStr.at(3)).c_str());
	GetDlgItem(IDC_COMBO55)->SetWindowText(Common::CharToWChar(valueStr.at(4)).c_str());
	GetDlgItem(IDC_COMBO56)->SetWindowText(Common::CharToWChar(valueStr.at(5)).c_str());
	GetDlgItem(IDC_COMBO57)->SetWindowText(Common::CharToWChar(valueStr.at(6)).c_str());
	GetDlgItem(IDC_COMBO58)->SetWindowText(Common::CharToWChar(valueStr.at(7)).c_str());
	GetDlgItem(IDC_COMBO59)->SetWindowText(Common::CharToWChar(valueStr.at(8)).c_str());
	GetDlgItem(IDC_COMBO60)->SetWindowText(Common::CharToWChar(valueStr.at(9)).c_str());
	GetDlgItem(IDC_COMBO61)->SetWindowText(Common::CharToWChar(valueStr.at(10)).c_str());
	GetDlgItem(IDC_COMBO62)->SetWindowText(Common::CharToWChar(valueStr.at(11)).c_str());
	GetDlgItem(IDC_COMBO63)->SetWindowText(Common::CharToWChar(valueStr.at(12)).c_str());
	GetDlgItem(IDC_COMBO64)->SetWindowText(Common::CharToWChar(valueStr.at(13)).c_str());
}

void UdcQcDlg::SendMessgeToPaint(int iColumn)
{
    COleDateTime dt1, dt2;
    m_dtFrom.GetTime(dt1);
    m_dtTo.GetTime(dt2);
    days = calcTimeDiff(WCharToChar(dt1.Format(_T("%Y-%m-%d")).GetBuffer()).c_str(), WCharToChar(dt2.Format(_T("%Y-%m-%d")).GetBuffer()).c_str()) + 1;

    std::shared_ptr<PaintView> sPaint = std::make_shared<PaintView>();
    *(sPaint.get()) = {};
	int i1 =  0,i2 = 0;
	CString value1,value2;
	sPaint->y_range = rangeStr.at(iColumn);
	sPaint->y_target = valueStr.at(iColumn);
	sPaint->x_days = days; 
    std::map<CString, CString> mapDateRange1 = GetCurDateRange();
    std::map<CString, CString> mapDateRange2 = mapDateRange1;
    int nDateColIndex = m_list.GetHeaderCtrl()->GetItemCount()-3;
	for (int iRow = 0; iRow < m_list.GetItemCount(); iRow++)
	{
        CString dt = m_list.GetItemText(iRow, nDateColIndex).GetLength() >= 10 ? m_list.GetItemText(iRow, nDateColIndex).Left(10) : CString();
        CString strType = m_list.GetItemText(iRow, 2);
        CString strVal = m_list.GetItemText(iRow, 3);
        std::map<CString, CString>::iterator item1, item2;
        if (strType == UDCI_QC)
        {
            item1 = mapDateRange1.find(dt);
            if (item1 == mapDateRange1.end() || item1->second.IsEmpty())
            {
                mapDateRange1[dt] = strVal;
            }
		}
        else if (strType == UDCII_QC)
		{
            item2 = mapDateRange2.find(dt);
            if (item2 == mapDateRange2.end() || item2->second.IsEmpty())
            {
                mapDateRange2[dt] = strVal;
            }
		}
		else
		{
			MessageBox(L"未知的干化质控类型", L"提示", MB_OK);
		}
	}
    if (m_radioP.GetCheck()) mapDateRange2.clear();
    else mapDateRange1.clear();
    for (std::map<CString, CString>::const_iterator itr = mapDateRange1.cbegin(); itr != mapDateRange1.cend(); itr++)
    {
        value1.Append(itr->second);
        value1.Append(L";");
    }
    for (std::map<CString, CString>::const_iterator itr = mapDateRange2.cbegin(); itr != mapDateRange2.cend(); itr++)
    {
        value2.Append(itr->second);
        value2.Append(L";");
    }
	sPaint->value1 = WCharToChar(value1.GetBuffer());
	sPaint->value2= WCharToChar(value2.GetBuffer());
    COleDateTime dtFrom;
    m_dtFrom.GetTime(dtFrom);
    sPaint->dtStart = dtFrom;
	/*if (((i1 && (i1 != days)) || (i2 && (i2 != days))))
	{	
		if (IDNO == MessageBox(L"所选日期与已有质控记录不匹配,是否继续？", L"提示", MB_YESNO))
			return;
	}
    if (!(i1 || i2))
    {
        return;
    }*/
	m_paintView->SendMessage(WM_PAINT_VIEW, (WPARAM)iColumn, (LPARAM)sPaint.get());
}

void UdcQcDlg::SendMessgeToPreview(int iColumn)
{
    std::shared_ptr<PaintView> sPaint = std::make_shared<PaintView>();
    *(sPaint.get()) = {};
	int i1 = 0, i2 = 0;
	CString value1, value2,valueDate,valueValidDate,valueBatch;
	sPaint->y_target = valueStr.at(iColumn);
	sPaint->x_days = days;
	sPaint->itmeIndex = iColumn;
	sPaint->priType = m_priType;
	for (int iRow = 0; iRow < m_list.GetItemCount(); iRow++)
	{
		if (m_list.GetItemText(iRow, 2) == UDCI_QC)
		{
			value1.Append(m_list.GetItemText(iRow, iColumn + 3));
			value1.Append(L";");

			if (m_priType == 1)
			{
				valueDate.Append(m_list.GetItemText(iRow, theApp.m_nUdcItemCount + 3));
				valueDate.Append(L";");

				valueBatch.Append(m_list.GetItemText(iRow, theApp.m_nUdcItemCount + 4));
				valueBatch.Append(L";");

				valueValidDate.Append(m_list.GetItemText(iRow, theApp.m_nUdcItemCount + 5));
				valueValidDate.Append(L";");
			}
			i1++;
		}
		else if (m_list.GetItemText(iRow, 2) == UDCII_QC)
		{
			value2.Append(m_list.GetItemText(iRow, iColumn + 3));
			value2.Append(L";");
			if (m_priType == 2)
			{
				valueDate.Append(m_list.GetItemText(iRow, theApp.m_nUdcItemCount + 3));
				valueDate.Append(L";");

				valueBatch.Append(m_list.GetItemText(iRow, theApp.m_nUdcItemCount + 4));
				valueBatch.Append(L";");

				valueValidDate.Append(m_list.GetItemText(iRow, theApp.m_nUdcItemCount + 5));
				valueValidDate.Append(L";");
			}

			i2++;
		}
		else
		{
			MessageBox(L"未知的干化质控类型", L"提示", MB_OK);
		}
	}

	sPaint->value1 = WCharToChar(value1.GetBuffer());
	sPaint->value2 = WCharToChar(value2.GetBuffer());
	sPaint->valueBatch = WCharToChar(valueBatch.GetBuffer());
	sPaint->valueDate = WCharToChar(valueDate.GetBuffer());
	sPaint->valueValidDate = WCharToChar(valueValidDate.GetBuffer());
	/*if (((i1 && (i1 != days)) || (i2 && (i2 != days))))
	{
		if (IDNO == MessageBox(L"所选日期与已有质控记录不匹配,是否继续？", L"提示", MB_YESNO))
			return;
	}
	if (!(i1 || i2))
	{
		return;
	}*/
	m_preView->SendMessage(WM_PAINT_VIEW_US, (WPARAM)2, (LPARAM)sPaint.get());
}

void UdcQcDlg::UpdateUIToString()
{
	CString strValue;
	((CEdit*)GetDlgItem(IDC_EDIT11))->GetWindowText(strValue);
	rangeStr[0] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT12)->GetWindowText(strValue);
	rangeStr[1] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT13)->GetWindowText(strValue);
	rangeStr[2] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT14)->GetWindowText(strValue);
	rangeStr[3] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT15)->GetWindowText(strValue);
	rangeStr[4] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT16)->GetWindowText(strValue);
	rangeStr[5] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT17)->GetWindowText(strValue);
	rangeStr[6] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT18)->GetWindowText(strValue);
	rangeStr[7] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT19)->GetWindowText(strValue);
	rangeStr[8] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT20)->GetWindowText(strValue);
	rangeStr[9] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT21)->GetWindowText(strValue);
	rangeStr[10] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT22)->GetWindowText(strValue);
	rangeStr[11] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT23)->GetWindowText(strValue);
	rangeStr[12] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_EDIT24)->GetWindowText(strValue);
	rangeStr[13] = WCharToChar(strValue.GetBuffer());

	((CComboBox*)GetDlgItem(IDC_COMBO51))->GetWindowText(strValue);
	valueStr[0] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO52)->GetWindowText(strValue);
	valueStr[1] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO53)->GetWindowText(strValue);
	valueStr[2] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO54)->GetWindowText(strValue);
	valueStr[3] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO55)->GetWindowText(strValue);
	valueStr[4] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO56)->GetWindowText(strValue);
	valueStr[5] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO57)->GetWindowText(strValue);
	valueStr[6] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO58)->GetWindowText(strValue);
	valueStr[7] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO59)->GetWindowText(strValue);
	valueStr[8] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO60)->GetWindowText(strValue);
	valueStr[9] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO61)->GetWindowText(strValue);
	valueStr[10] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO62)->GetWindowText(strValue);
	valueStr[11] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO63)->GetWindowText(strValue);
	valueStr[12] = WCharToChar(strValue.GetBuffer());
	GetDlgItem(IDC_COMBO64)->GetWindowText(strValue);
	valueStr[13] = WCharToChar(strValue.GetBuffer());
}

void UdcQcDlg::SaveQCImage()
{
	char qcIni[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, qcIni, MAX_PATH);
	PathRemoveFileSpecA(qcIni);
	sprintf(qcIni, "%s\\QcImage\\pic.bmp", qcIni);
	m_paintView->SaveBMP(qcIni);
}


void UdcQcDlg::SaveListImage()
{
	char qcIni[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, qcIni, MAX_PATH);
	PathRemoveFileSpecA(qcIni);
	sprintf(qcIni, "%s\\QcImage\\list.bmp", qcIni);
	m_preView->SaveBMP(qcIni);
}

void UdcQcDlg::GetUitPath(char* qcIni)
{
	GetModuleFileNameA(NULL, qcIni, MAX_PATH);
	PathRemoveFileSpecA(qcIni);
	sprintf(qcIni, "%s\\QcImage\\qc.uit", qcIni);
}

int UdcQcDlg::SetComboxByValueString(CComboBox* combo,char* strValue)
{
	CString tStrValue;
	for (int i = 0; i < combo->GetCount();i++)
	{
		combo->SetCurSel(i);
		combo->GetWindowTextW(tStrValue);
		if (strcmp(WCharToChar(tStrValue.GetBuffer()).c_str(),strValue) == 0)
			return i;
	}
	return -1;
}

bool UdcQcDlg::SetComboxByValueString()
{
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO51),(char*)valueStr.at(0).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO52), (char*)valueStr.at(1).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO53), (char*)valueStr.at(2).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO54), (char*)valueStr.at(3).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO55), (char*)valueStr.at(4).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO56), (char*)valueStr.at(5).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO57), (char*)valueStr.at(6).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO58), (char*)valueStr.at(7).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO59), (char*)valueStr.at(8).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO60), (char*)valueStr.at(9).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO61), (char*)valueStr.at(10).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO62), (char*)valueStr.at(11).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO63), (char*)valueStr.at(12).c_str()))
		return false;
	if( -1 == SetComboxByValueString((CComboBox*)GetDlgItem(IDC_COMBO64), (char*)valueStr.at(13).c_str()))
		return false;
	return true;
}




void UdcQcDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	if (!isValid)
	{
		return;
	}
	
	switch (m_tab.GetCurSel())
	{
	case 0:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 1:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 2:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 3:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 4:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 5:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 6:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 7:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 8:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 9:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 10:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 11:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 12:SendMessgeToPaint(m_tab.GetCurSel()); break;
	case 13:SendMessgeToPaint(m_tab.GetCurSel()); break;
	default:
		break;
	}	
	*pResult = 0;
}



int UdcQcDlg::calcTimeDiff(const char* from, const char* to)
{
	int nYear, nMonth, nDate;
	sscanf(from, "%d-%d-%d", &nYear, &nMonth, &nDate);
	CTime t1(nYear, nMonth, nDate, 0, 0, 0); 

	sscanf(to, "%d-%d-%d", &nYear, &nMonth, &nDate);
	CTime t2(nYear, nMonth, nDate, 0, 0, 0);

	CTimeSpan ts = t2 - t1;            
	//ATLASSERT(ts.GetTotalSeconds() == 86400L);
	ATLASSERT((t1 + ts) == t2);      
	ATLASSERT((t2 - ts) == t1);   
	return ts.GetDays();
}

int CALLBACK listCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SortData *paramSort = (SortData *)lParamSort;
	CListCtrl* list = paramSort->listctrl;
	int isub = paramSort->isub;

	LVFINDINFO findInfo;
	findInfo.flags = LVFI_PARAM;
	findInfo.lParam = lParam1;
	int iItem1 = list->FindItem(&findInfo, -1);
	findInfo.lParam = lParam2;
	int iItem2 = list->FindItem(&findInfo, -1);

	CString strItem1 = list->GetItemText(iItem1, isub);
	CString strItem2 = list->GetItemText(iItem2, isub);

	return _tcscmp(strItem1.GetBuffer(MAX_PATH), strItem2.GetBuffer(MAX_PATH));
}

void UdcQcDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	SaveQCImage();
	CChoiceQcPri dlg;
	dlg.DoModal();
	m_priType = dlg.m_priType;
 	m_prelabel.ShowWindow(SW_SHOW);
	CRect wndRect(0, 0, 5 * 108, 16 * 31);
	m_prelabel.MoveWindow(wndRect);
	m_preView->MoveWindow(wndRect);
	SendMessgeToPreview(m_tab.GetCurSel());
	m_preView->UpdateWindow();
	SaveListImage();
	m_prelabel.ShowWindow(SW_HIDE);

	char pPath[MAX_PATH] = { 0 };
	GetUitPath(pPath);
	if (TRUE != UWReport_PrintView(m_list.GetItemData(0),pPath))
		return;
}


void UdcQcDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CRect rc;
	if (pNMListView->iItem != -1)
	{
		m_row = pNMListView->iItem;//m_row为被选中行的行序号（int类型成员变量）
		m_column = pNMListView->iSubItem;//m_column为被选中行的列序号（int类型成员变量）
		if (m_column != 0 && m_column < 3)
		{
			return;
		}
		DoSubItemEdit(m_row, m_column);
	}
	*pResult = 0;
}



BOOL UdcQcDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO:  在此添加专用代码和/或调用基类
    if (WM_KEYDOWN == pMsg->message)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            return TRUE;
        }
        else if (pMsg->wParam == VK_RETURN)
        {
            this->SetFocus();
            return TRUE;
        }
    }
    else if (WM_SYSKEYDOWN == pMsg->message)
    {
        if (VK_F4 == pMsg->wParam)
        {
            return TRUE;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void UdcQcDlg::OnBnClickedRadio()
{
    // TODO:  在此添加控件通知处理程序代码
    //OnBnClickedButton1();
    SendMessgeToPaint(m_tab.GetCurSel());
}

void UdcQcDlg::OnBnClickedButtonSend()
{
    int nCount = m_list.GetSelectedCount();
    if (nCount == 0)
    {
        AfxMessageBox(_T("请先选择需要发送的记录!"), MB_OK | MB_ICONINFORMATION);
        return;
    }
    POSITION pos = m_list.GetFirstSelectedItemPosition();
    for (int j = 0; j < nCount; j++)
    {
        int nItem = m_list.GetNextSelectedItem(pos);
        ULONG taskId = m_list.GetItemData(nItem);
        if (taskId > 0)
        {
            SendToLisQueue((CMainFrame*)AfxGetMainWnd(), taskId);
        }
    }
}


std::map<CString, CString> UdcQcDlg::GetCurDateRange() const
{
    std::map<CString, CString> ret;
    COleDateTime dtFrom, dtTo, dtTemp;
    m_dtFrom.GetTime(dtFrom);
    m_dtTo.GetTime(dtTo);

    if (dtFrom > dtTo)
    {
        dtTemp = dtFrom;
        dtFrom = dtTo;
        dtTo = dtTemp;
    }

    for (COleDateTime dt = dtFrom; dt <= dtTo; dt += COleDateTimeSpan(1, 0, 0, 0))
    {
        ret[dt.Format(_T("%Y-%m-%d"))] = _T("");
    }

    return ret;
}


void UdcQcDlg::OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    // TODO:  在此添加控件通知处理程序代码
    *pResult = CDRF_DODEFAULT;

    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
    }
    else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
    {

        COLORREF clrNewTextColor, clrNewBkColor;

        int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

        POSITION pos = m_list.GetFirstSelectedItemPosition();
        int index = m_list.GetNextSelectedItem(pos);

        if (index == nItem)//如果要刷新的项为当前选择的项，则将文字设为白色，背景色设为蓝色
        {
            clrNewTextColor = RGB(255, 255, 255);        //Set the text to white
            clrNewBkColor = RGB(51, 153, 255);        //Set the background color to blue
        }
        else
        {
            clrNewTextColor = RGB(0, 0, 0);        //set the text black
            clrNewBkColor = RGB(255, 255, 255);    //leave the background color white
        }

        pLVCD->clrText = clrNewTextColor;
        pLVCD->clrTextBk = clrNewBkColor;

        *pResult = CDRF_DODEFAULT;
    }
}
