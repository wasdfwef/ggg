// SearchRecordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SearchRecordDlg.h"
#include "afxdialogex.h"
#include "UW.h"
#include "Include/Common/String.h"
#include "MainFrm.h"
#include "DbgMsg.h"
#include "FindRecord.h"

extern
std::wstring
GetIdsString(const std::wstring &sMain, const std::wstring &sId);
// SearchRecordDlg 对话框

#define xPos 100
#define yPos 23
#define OffsetPos 30

IMPLEMENT_DYNAMIC(SearchRecordDlg, CDialogEx)

SearchRecordDlg::SearchRecordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SearchRecordDlg::IDD, pParent)
{

}

SearchRecordDlg::~SearchRecordDlg()
{

}

void SearchRecordDlg::InitRes()
{
	int offsetPos = 0;
	SetWindowTextW(GetIdsString(_T("findrecord"), _T("title")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_ID)->SetWindowTextW(GetIdsString(_T("findrecord"),_T("sn")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_START_DATE)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("date")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_START_TIME)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("time")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_END_TIME)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("time1")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_END_DATE)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("date1")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_BARCODE)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("barcode")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_NAME)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("name")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_HOSPITAL)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("hospital")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_ROOM)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("department")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_DOCTOR)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("doctor")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_NUMBER)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("hospitalsn")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_CHECKER)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("checker")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_AUDIT)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("rechecker")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_REPORTER)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("reporter")).c_str());
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_QC)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("bQc")).c_str());

	CRect clientRect;
	GetClientRect(clientRect);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_ID)->MoveWindow(clientRect.Width() *5/12  - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_START_DATE)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_START_TIME)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_END_DATE)->MoveWindow(clientRect.Width() * 5 / 12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_END_TIME)->MoveWindow(clientRect.Width() * 5 / 12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_BARCODE)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_NAME)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_HOSPITAL)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_ROOM)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_DOCTOR)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_NUMBER)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_CHECKER)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_AUDIT)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_REPORTER)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_STATIC_SEARCH_RECORD_QC)->MoveWindow(clientRect.Width() *5/12 - xPos, offsetPos, xPos, yPos);
	offsetPos = -3;
	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_ID)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;

	GetDlgItem(IDC_BUTTON_SEARCH_RECORD_DATE)->MoveWindow(clientRect.Width() *5/12 + xPos - yPos + 2, offsetPos + 1, yPos - 2, yPos - 2);//按钮嵌入编辑框
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_START_DATE)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	((CEdit*)GetDlgItem(IDC_EDIT_SEARCH_RECORD_START_DATE))->SetMargins(0, xPos - yPos);

	GetDlgItem(IDC_EDIT_SEARCH_RECORD_START_DATE)->ModifyStyle(0, WS_CLIPSIBLINGS);



	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_START_TIME)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_START_END_DATE)->MoveWindow(clientRect.Width() * 5 / 12, offsetPos, xPos, yPos);

	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_END_TIME)->MoveWindow(clientRect.Width() * 5 / 12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_BARCODE)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_NAME)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_COMBO_SEARCH_RECORD_HOSPITAL)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_ROOM)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_DOCTOR)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_NUMBER)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_COMBO_SEARCH_RECORD_CHECKER)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_COMBO_SEARCH_RECORD_AUDIT)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_COMBO_SEARCH_RECORD_REPORTER)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;
	GetDlgItem(IDC_COMBO_SEARCH_RECORD_QC)->MoveWindow(clientRect.Width() *5/12, offsetPos, xPos, yPos);
	offsetPos += OffsetPos;

	GetDlgItem(IDC_BUTTON_SEARCH_RECORD_DATE)->SetWindowTextW(GetIdsString(_T("findrecord"), _T("setdate")).c_str());

	GetDlgItem(IDC_EDIT_SEARCH_RECORD_START_TIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SEARCH_RECORD_END_TIME)->EnableWindow(FALSE);
	ConfigPropList();
}

void SearchRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SEARCH_RECORD_CHECKER, m_checker);
	DDX_Control(pDX, IDC_COMBO_SEARCH_RECORD_AUDIT, m_auditer);
	DDX_Control(pDX, IDC_COMBO_SEARCH_RECORD_REPORTER, m_reporter);
	DDX_Control(pDX, IDC_COMBO_SEARCH_RECORD_QC, m_qc);
	DDX_Control(pDX, IDC_COMBO_SEARCH_RECORD_HOSPITAL, m_hospital);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_ROOM, m_room);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_DOCTOR, m_doctor);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_START_TIME, m_start_time);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_START_END_DATE, m_end_date);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_END_TIME, m_end_time);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_START_DATE, m_start_date);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_ID, m_sn);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_BARCODE, m_barcode);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_SEARCH_RECORD_NUMBER, m_number);
}

void SearchRecordDlg::ConfigPropList()
{
	//设置检验信息
	ResetUserOption();

	//设置科室医院选项
	ResetOtherOption();
}

void SearchRecordDlg::ResetUserOption()
{
	m_checker.Clear();
	m_auditer.Clear();
	m_reporter.Clear();

	for (int i = 0; i < theApp.m_userlist.nUserCounter; ++i)
	{
		CString strTemp;

		strTemp = Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sName).c_str();
		m_checker.AddString(strTemp);
		m_auditer.AddString(strTemp);
		m_reporter.AddString(strTemp);

	}


	CString strTemp;
	strTemp = GetIdsString(_T("findrecord"), _T("1")).c_str();
	m_qc.AddString(strTemp);
	strTemp = GetIdsString(_T("findrecord"), _T("2")).c_str();
	m_qc.AddString(strTemp);
}

void SearchRecordDlg::ResetDepartmentOption()
{
	CString strHospital; 
	m_hospital.GetWindowTextW(strHospital);
	for (int i = 0; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		if (!strcmp(theApp.hospitalInfo.HospitalInfo[i].name, Common::WCharToChar(strHospital.GetBuffer()).c_str()))
		{
			for (int j = 0; j < theApp.hospitalInfo.HospitalInfo[i].DepartmentCount; ++j)
			{
				m_room.SetWindowTextW(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name).c_str());
			}
		}
	}
}

void SearchRecordDlg::ResetDoctorOption()
{
	CString strHospital;
	m_hospital.GetWindowTextW(strHospital);
	CString strDepartment;
	m_room.GetWindowTextW(strDepartment);

	//清除旧的设置
	//m_FindPropList.GetProperty(0)->GetSubItem(9)->RemoveAllOptions();
	//m_FindPropList.GetProperty(0)->GetSubItem(10)->RemoveAllOptions();
	for (int i = 0; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		if (!strcmp(theApp.hospitalInfo.HospitalInfo[i].name, Common::WCharToChar(strHospital.GetBuffer()).c_str()))
		{
			for (int j = 0; j < theApp.hospitalInfo.HospitalInfo[i].DepartmentCount; ++j)
			{
				//m_FindPropList.GetProperty(0)->GetSubItem(9)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name).c_str(), FALSE);

				if (!strcmp(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name, Common::WCharToChar(strDepartment.GetBuffer()).c_str()))
				{
					for (int z = 0; z < theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorCount; ++z)
					{
						m_doctor.SetWindowTextW(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorInfo[z].name).c_str());
					}
					break;
				}
			}
		}


	}
}

void SearchRecordDlg::Search()
{
	//检查日期是否合法
	CString             strDate;
	CMainFrame         *pMainFrame = (CMainFrame*)theApp.GetMainWnd();

	//禁止在做任务的状态进行查询
	if (theApp.IsIdleStatus() == FALSE)
	{
		::MessageBox(NULL,L"请等待任务完成后再执行搜索操作", L"提示", MB_OK);
		return;
	}


	pMainFrame->m_wndTodayList.AutoSaveCurRecord();
	m_start_date.GetWindowTextW(strDate);
	if (!CheckDateLegit(strDate))
	{
		return;
	}
	strDate.TrimLeft();
	strDate.TrimRight();



	ZeroMemory(&theApp.searchtask, sizeof(SEARCH_TASK));
	CString qcstr;
	m_qc.GetWindowTextW(qcstr);
	CString tmp("是");
	if (qcstr == tmp)
	{
		theApp.searchtask.bIsSearch_QC_Task = true;
	}
	else
	{
		theApp.searchtask.bIsSearch_QC_Task = false;
	}



	strcpy(theApp.searchtask.conditions.sDate_From, Common::WCharToChar(strDate.GetBuffer()).c_str());
	CString tr;
	m_start_time.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sTime_From, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_end_date.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sDate_To, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_end_time.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sTime_To, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_sn.GetWindowText(tr);
	theApp.searchtask.conditions.nSN = atoi((Common::WCharToChar(tr.GetBuffer()).c_str()));
	m_barcode.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sBarcode, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_name.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sName, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_number.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sHospital, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_hospital.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sCompany, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_room.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sDepartment, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_doctor.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sDoctor, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_checker.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sDocimaster, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_auditer.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sAuditor, Common::WCharToChar(tr.GetBuffer()).c_str());
	m_reporter.GetWindowTextW(tr);
	strcpy(theApp.searchtask.conditions.sReporter, Common::WCharToChar(tr.GetBuffer()).c_str());


	ULONG findstate = Access_SearchRecord(&theApp.searchtask);
	DBG_MSG("theApp.searchtask.pSearchInfoCounts :%d\n", theApp.searchtask.pSearchInfoCounts);
	//生成查询语句
	((CMainFrame*)theApp.GetMainWnd())->m_wndTodayList.UpdateRecordList();
	((CUdcView*)(((CMainFrame*)theApp.GetMainWnd())->GetUWView()->GetTabWnd(THUM_UDC_TAB)))->UpdateSearchRecordResult();
}

/*******************************************************
函数名:  ResetOtherOption
函数描述:重新设置属性列表
*******************************************************/
void SearchRecordDlg::ResetOtherOption()
{
	//清除旧的选项
	m_hospital.Clear();
	//m_FindPropList.GetProperty(0)->GetSubItem(10)->RemoveAllOptions();
	for (int i = 0; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		m_hospital.AddString(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].name).c_str());

	}
}
BEGIN_MESSAGE_MAP(SearchRecordDlg, CDialogEx)
	ON_BN_CLICKED(SEARCH_RECORD_IDCANCEL, &SearchRecordDlg::OnBnClickedRecordIdcancel)
	ON_CBN_KILLFOCUS(IDC_COMBO_SEARCH_RECORD_HOSPITAL, &SearchRecordDlg::OnCbnKillfocusComboSearchRecordHospital)
	ON_EN_KILLFOCUS(IDC_EDIT_SEARCH_RECORD_ROOM, &SearchRecordDlg::OnEnKillfocusEditSearchRecordRoom)
	ON_BN_CLICKED(SEARCH_RECORD_IDOK, &SearchRecordDlg::OnBnClickedRecordIdok)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_RECORD_DATE, &SearchRecordDlg::OnBnClickedButtonSearchRecordDate)
END_MESSAGE_MAP()


// SearchRecordDlg 消息处理程序




BOOL SearchRecordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitRes();
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void SearchRecordDlg::OnBnClickedRecordIdcancel()
{
	// TODO:  在此添加控件通知处理程序代码
	OnCancel();
}


void SearchRecordDlg::OnCbnKillfocusComboSearchRecordHospital()
{
	// TODO:  在此添加控件通知处理程序代码
	_variant_t varStrDefault(_bstr_t(_T("")));
	CString strNow("");

	m_hospital.Clear();
	m_room.Clear();
	m_hospital.AddString(strNow);
	m_room.SetWindowTextW(strNow);
	ResetDepartmentOption();
}


void SearchRecordDlg::OnEnKillfocusEditSearchRecordRoom()
{
	// TODO:  在此添加控件通知处理程序代码
	_variant_t varStrDefault(_bstr_t(_T("")));
	CString strNow("");
	m_doctor.Clear();
	m_doctor.SetWindowTextW(strNow);
	ResetDoctorOption();

}


void SearchRecordDlg::OnBnClickedRecordIdok()
{
	// TODO:  在此添加控件通知处理程序代码
	BeginWaitCursor();
	Search();
	EndWaitCursor();
	OnOK();
}

BOOL SearchRecordDlg::CheckDateLegit(CString strDate)
{
	BOOL bLegit = TRUE;
	strDate.TrimLeft();
	strDate.TrimRight();
	if (!strDate.IsEmpty())
	{
		COleDateTime datetime;
		if (!datetime.ParseDateTime(strDate, VAR_DATEVALUEONLY))
		{
			bLegit = FALSE;
		}
	}
	if (!bLegit)
	{
		//AfxMessageBox(IDS_VALUE_NOT_LEGIT);
	}
	return bLegit;
}

void SearchRecordDlg::OnBnClickedButtonSearchRecordDate()
{
	// TODO:  在此添加控件通知处理程序代码
	CFindRecordSetDate dlg;
	if (dlg.DoModal() == IDOK)
	{
/*		SetValue(dlg.m_strDate);*/
		((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.m_start_date.SetWindowTextW(dlg.m_strDate);
		((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.m_start_time.SetWindowTextW(dlg.m_strTime);
		((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.m_end_date.SetWindowTextW(dlg.m_strDate2);
		((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.m_end_time.SetWindowTextW(dlg.m_strTime2);
	}
}
