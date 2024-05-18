/*****************************************************************************
Copyright (C), 1995-2010, Chongqing Tianhai Medical Equipment Co., Ltd.
文件名:		PropertiesWndPat.cpp
功能:		病人信息显示
时间:		2010-05
*******************************************************************************/

#include "stdafx.h"
#include "UW.h"
//#include "ComValue.h"
#include "PropertiesWndPat.h"
#include "Include/Common/String.h"
#include "MainFrm.h"

// CPropertiesWndPat
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

static ULONG n_CruID = 0;
BOOL CheckNum(CMFCPropertyGridProperty *pSN, int &nNum,int nDefaultNum)
{
	COleVariant  d_val((long)nDefaultNum, VT_UINT);
	int nSN = (int)(_variant_t(pSN->GetValue()));

	if( nSN < 1 || nSN > MAX_SN )
	{
		pSN->SetOriginalValue(d_val);
		pSN->SetValue(d_val);
		return FALSE;
	}

	nNum  = nSN;
	return TRUE;
}

IMPLEMENT_DYNAMIC(CPropertiesWndPat, CDockablePane)

using namespace Common;
CPropertiesWndPat::CPropertiesWndPat()
{

}

CPropertiesWndPat::~CPropertiesWndPat()
{
}


BEGIN_MESSAGE_MAP(CPropertiesWndPat, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_CONTEXTMENU()
//	ON_WM_KEYUP()
//ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CPropertiesWndPat 消息处理程序

int CPropertiesWndPat::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_WndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_GRID_PROPERTY1))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}
	AdjustLayout();
	

	LOGFONT lf = { 0 };
	m_WndPropList.GetFont()->GetLogFont(&lf);
	lf.lfHeight = -14;
	//DBG_MSG("m_WndPropList default font size: %s %d, %d\n", WstringToString(lf.lfFaceName).c_str(), lf.lfHeight, lf.lfWidth);
	const auto hFont = ::CreateFontIndirectW(&lf);
	m_WndPropList.SetFont(CFont::FromHandle(hFont));

	//InitPropList();

	return 0;
}

void CPropertiesWndPat::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
	//m_WndPropList.SetWindowPos (this, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	//m_WndPropList.ShowWindow(TRUE);
}

void CPropertiesWndPat::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_WndPropList.SetFocus();
}

/**************************************************
函数名:  AdjustLayout
函数描述:调整显示
**************************************************/
void CPropertiesWndPat::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);
	m_WndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

/**************************************************
函数名:  InitPropList
函数描述:初始化属性列表
**************************************************/
void CPropertiesWndPat::InitPropList()
{
	n_CruID;
	m_WndPropList.InitList();
	//m_WndPropList.GetProperty(2)->GetSubItem(2)->ExpandDeep();

	LOGFONT lf = { 0 };
	m_WndPropList.GetFont()->GetLogFont(&lf);
	lf.lfHeight = -14;
	const auto hFont = ::CreateFontIndirectW(&lf);
	m_WndPropList.SetFont(CFont::FromHandle(hFont));
}



/*********************************************************
函数名:  ResetOtherOption
函数描述:重新设置属性列表
*********************************************************/
void CPropertiesWndPat::ResetOtherOption()
{
	m_WndPropList.ResetOtherOption();
}

/*********************************************************
函数名:  ResetUserOption
函数描述:重新设置用户属性列表的选项
*********************************************************/
//void CPropertiesWndPat::ResetUserOption()
//{
//	m_WndPropList.ResetUserOption();
//}
/********************************************************
函数名:  ShowData
函数描述:显示数据
输入参数:iID-----显示数据库中ID值为iID的记录
********************************************************/

LRESULT CPropertiesWndPat::OnPropertyChanged( WPARAM wParam, LPARAM lParam)
{
	/*if (theApp.m_bSaveDate== false)
	  return 0;*/

	int                       nCurIndex, nGroupIndex, nNum;
	BOOL                      bResetValue = FALSE;
	CString                   str;
	TASK_INFO                 task_info = {0};
	CMainFrame              *pMainFrame = (CMainFrame*)theApp.GetMainWnd();
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lParam;

	//pPr

	nCurIndex = pMainFrame->m_wndTodayList.GetCurSelIndex();

	if( nCurIndex == -1 )
	{
err_loc:
		pProp->SetValue(pProp->GetOriginalValue());
		return 0;
	}

	nGroupIndex = pProp->GetData();
	str = pProp->GetValue();

	if( nGroupIndex != 16 && str.GetLength() >= 32 )
	{
		TCHAR  temp[32] = L"";


		wcsncpy(temp, str, 31);
		str = temp;
		bResetValue = TRUE;
	}

	if( bResetValue == FALSE )
		pProp->SetOriginalValue(pProp->GetValue());
	else
	{
		pProp->SetOriginalValue(str);
		pProp->SetValue(pProp->GetOriginalValue());
	}

	ResetOtherOption();



	//pMainFrame->m_wndTodayList.GetCurTaskInfo(task_info);

	task_info.main_info.nID = n_CruID;
	KSTATUS r = Access_GetTaskInfo(n_CruID,USER_TYPE_INFO, &task_info);

	if( nGroupIndex == 0 )   //SN
	{
		 if( CheckNum(pProp, nNum, task_info.main_info.nSN) == FALSE )
			 goto err_loc;

		
		 task_info.main_info.nSN = nNum;
		 pMainFrame->m_wndTodayList.m_List.SetItemText(nCurIndex, 3, str);
		 pMainFrame->m_wndResult.m_WndPropList.ShowData(task_info, SHOW_USER_TYPE);

	}
	else if (nGroupIndex == 1)
	{
		CopyStr(pProp, task_info.main_info.sCode);
		pMainFrame->m_wndTodayList.m_List.SetItemText(nCurIndex, 7, str);
	}
		
	else if( nGroupIndex == 2 )
	{
		CopyStr(pProp, task_info.pat_info.sName);		

	}

	else if( nGroupIndex == 3 )
	{
		task_info.pat_info.nSex = GetSex(pProp);
		//pMainFrame->m_wndTodayList.m_List.SetItemText(nCurIndex, 8, str);
	}
	else if( nGroupIndex == 4 )
	{
		if( CheckNum(pProp, nNum, 0) == FALSE )
			goto err_loc;
		task_info.pat_info.nAge = nNum;
		//pMainFrame->m_wndTodayList.m_List.SetItemText(nCurIndex, 9, str);
	}
	else if( nGroupIndex == 5 )
		task_info.pat_info.nAgeUnit = AgeUnit(pProp);
	else if( nGroupIndex == 6 )
		CopyStr(pProp, task_info.pat_info.sHospital);
	else if( nGroupIndex == 7 )
		CopyStr(pProp, task_info.pat_info.sSickBed);
	else if( nGroupIndex == 8 )
	{	
		CopyStr(pProp, task_info.main_info.sCompany);
		clearOption();
	}
	else if( nGroupIndex == 9 )
	{
		CopyStr(pProp, task_info.main_info.sDepartment);
		clearDoctorOption();
	}
	else if( nGroupIndex == 10 )
		CopyStr(pProp, task_info.main_info.sDoctor);
	else if( nGroupIndex == 11 )
		CopyStr(pProp, task_info.main_info.sDocimaster);
	else if( nGroupIndex == 12 )
		CopyStr(pProp, task_info.main_info.sAuditor);
	else if( nGroupIndex == 13 )
		CopyStr(pProp, task_info.main_info.sReporter);
	else if( nGroupIndex == 14 )
		CopyStr(pProp, task_info.main_info.sColor);
	else if( nGroupIndex == 15 )
		CopyStr(pProp, task_info.main_info.sTransparency);
	else 
		CopyStr(pProp, task_info.main_info.sDiagnosis);


	pMainFrame->m_wndTodayList.UpdateCurTaskInfo(task_info, USER_TYPE_INFO);
	//pMainFrame->m_wndTodayList.SetSaveCurRecord(TRUE);
	r = Access_UpdateTaskInfo(USER_TYPE_INFO, task_info.main_info.nID, &task_info);
	

	return 0;
}





// CPropertyGridCtrlPat

IMPLEMENT_DYNAMIC(CPropertyGridCtrlPat, CMFCPropertyGridCtrl)
using namespace Common;

CPropertyGridCtrlPat::CPropertyGridCtrlPat()
:m_nMaxData(16)
{
}

CPropertyGridCtrlPat::~CPropertyGridCtrlPat()
{
}


HFONT GetSelfDefinedFont(CDC* pDC)
{
	static HFONT hFont = NULL;

	if (NULL == hFont)
	{
		LOGFONT LogFont = {0};
		TEXTMETRIC TextMetric = {0};

		//GetTextMetrics(pDC->GetSafeHdc(), &TextMetric);
		pDC->GetCurrentFont()->GetLogFont(&LogFont);

		//LogFont.lfHeight = TextMetric.tmHeight * 1.5;
		//LogFont.lfWidth = TextMetric.tmAveCharWidth * 1.2;
		LogFont.lfWeight = FW_MEDIUM;
		wcscpy( LogFont.lfFaceName, L"宋体" ); 
		hFont = CreateFontIndirect(&LogFont);
	}

	return hFont;
}


int CPropertyGridCtrlPat::OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const
{
	pDC->SelectObject( GetSelfDefinedFont(pDC) );
	return CMFCPropertyGridCtrl::OnDrawProperty(pDC, pProp);
}

BEGIN_MESSAGE_MAP(CPropertyGridCtrlPat, CMFCPropertyGridCtrl)
	ON_WM_KEYUP()
   //ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CPropertyGridCtrlPat 消息处理程序



/***************************************************
函数名:  InitList
函数描述:初始化属性列表
***************************************************/
void CPropertyGridCtrlPat::InitList()
{


	EnableHeaderCtrl(FALSE);
	EnableDescriptionArea(FALSE);
	SetVSDotNetLook(TRUE);
	MarkModifiedProperties(FALSE);
	_variant_t varStrDefault(_bstr_t(_T("")));
	_variant_t varIntDefault(unsigned int(0));

	// 基本信息
	CMFCPropertyGridProperty *pProGroup1 = new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_BASIC_INFO")).c_str(), 0, FALSE);
	pProGroup1->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_ID")).c_str(), varIntDefault, NULL, 0));
	pProGroup1->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_SN")).c_str(), varIntDefault, NULL, 0));
	pProGroup1->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_DATE")).c_str(), varStrDefault, NULL, 0));
	pProGroup1->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_TIME")).c_str(), varStrDefault, NULL, 0));
	AddProperty(pProGroup1);

	pProGroup1->GetSubItem(0)->AllowEdit(FALSE);
	pProGroup1->GetSubItem(1)->SetData(0);
	pProGroup1->GetSubItem(2)->AllowEdit(FALSE);
	pProGroup1->GetSubItem(3)->AllowEdit(FALSE);

	// 患者资料
	CMFCPropertyGridProperty *pProGroup2 = new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_PAT_INFO")).c_str(), 0, FALSE);
	pProGroup2->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_BARCODE")).c_str(), varStrDefault, NULL, 0));
	pProGroup2->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_PAT_NAME")).c_str(), varStrDefault, NULL, 0));
	pProGroup2->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_SEX")).c_str(), varStrDefault, NULL, 0));
	pProGroup2->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_AGE")).c_str(), varIntDefault, NULL, 0));
	pProGroup2->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_AGE_UNIT")).c_str(), varStrDefault, NULL, 0));
	pProGroup2->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_HOSPITAL_SN")).c_str(), varStrDefault, NULL, 0));
	pProGroup2->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_BED_SN")).c_str(), varStrDefault, NULL, 0));
	AddProperty(pProGroup2);
	
	pProGroup2->GetSubItem(2)->AllowEdit(TRUE);
	pProGroup2->GetSubItem(4)->AllowEdit(TRUE);

	// 送检信息
	CMFCPropertyGridProperty *pProGroup3 = new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_SEND_INFO")).c_str(), 0, FALSE);
	pProGroup3->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_SEND_HOSPTIAL")).c_str(), varStrDefault, NULL, 0));
	pProGroup3->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_SEND_DEPARTMENT")).c_str(), varStrDefault, NULL, 0));
	pProGroup3->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_SEND_DOCTOR")).c_str(), varStrDefault, NULL, 0));
	AddProperty(pProGroup3);

	// 检验信息
	CMFCPropertyGridProperty *pProGroup4 = new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_SEND_INFO")).c_str(), 0, FALSE);
	pProGroup4->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_CHECKER")).c_str(), varStrDefault, NULL, 0));
	pProGroup4->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_RECHECKER")).c_str(), varStrDefault, NULL, 0));
	pProGroup4->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_REPORTER")).c_str(), varStrDefault, NULL, 0));
	AddProperty(pProGroup4);

	// 诊断信息
	CMFCPropertyGridProperty *pProGroup5 = new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_RESULT_INFO")).c_str(), 0, FALSE);
	pProGroup5->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_RESULT_COLOR")).c_str(), varStrDefault, NULL, 0));
	pProGroup5->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"),_T("IDS_RESULT_TRANSPARENT")).c_str(), varStrDefault, NULL, 0));
	pProGroup5->AddSubItem(new CMFCPropertyGridProperty(_T("临床诊断"), varStrDefault, NULL, 0));
	AddProperty(pProGroup5);

	// 配置属性列表
	ConfigList();
	
	
	pProGroup5->GetSubItem(0)->Show(FALSE);
	pProGroup5->GetSubItem(1)->Show(FALSE);
}

/******************************************************
函数名:  ConfigList
函数描述:配置属性列表
******************************************************/
void CPropertyGridCtrlPat::ConfigList()
{
	// 基本信息禁止编辑
	GetProperty(0)->GetSubItem(0)->AllowEdit(FALSE);
	//GetProperty(0)->GetSubItem(1)->AllowEdit(FALSE);
	GetProperty(0)->GetSubItem(2)->AllowEdit(FALSE);
	GetProperty(0)->GetSubItem(3)->AllowEdit(FALSE);

	// 设置患者信息
	_variant_t varStrYear(GetIdsString(_T("pat"),_T("IDS_YEAR")).c_str());
	GetProperty(1)->GetSubItem(2)->AddOption(GetIdsString(_T("pat"),_T("IDS_XX")).c_str(), FALSE);
	GetProperty(1)->GetSubItem(2)->AddOption(GetIdsString(_T("pat"),_T("IDS_BOY")).c_str(), FALSE);
	GetProperty(1)->GetSubItem(2)->AddOption(GetIdsString(_T("pat"),_T("IDS_GIRL")).c_str(), FALSE);
	GetProperty(1)->GetSubItem(4)->AddOption(GetIdsString(_T("pat"),_T("IDS_YEAR")).c_str(), FALSE);
	GetProperty(1)->GetSubItem(4)->AddOption(GetIdsString(_T("pat"),_T("IDS_MONTH")).c_str(), FALSE);
	GetProperty(1)->GetSubItem(4)->AddOption(GetIdsString(_T("pat"),_T("IDS_DAY")).c_str(), FALSE);
	GetProperty(1)->GetSubItem(4)->SetValue(varStrYear);
	

	SetHospitalOption();

	ResetColorOption();
	ResetClearOption();
	ResetDiagnosOption();

	// 设置检验信息
	ResetUserOption();

	// 设置送检信息,诊断信息
	ResetOtherOption();

	// 设置DATA
//	GetProperty(0)->GetSubItem(1)->SetData(0);
	GetProperty(1)->GetSubItem(0)->SetData(1);
	GetProperty(1)->GetSubItem(1)->SetData(2);
	GetProperty(1)->GetSubItem(2)->SetData(3);
	GetProperty(1)->GetSubItem(3)->SetData(4);
	GetProperty(1)->GetSubItem(4)->SetData(5);
	GetProperty(1)->GetSubItem(5)->SetData(6);
	GetProperty(1)->GetSubItem(6)->SetData(7);
	GetProperty(2)->GetSubItem(0)->SetData(8);
	GetProperty(2)->GetSubItem(1)->SetData(9);
	GetProperty(2)->GetSubItem(2)->SetData(10);
	GetProperty(3)->GetSubItem(0)->SetData(11);
	GetProperty(3)->GetSubItem(1)->SetData(12);
	GetProperty(3)->GetSubItem(2)->SetData(13);
	GetProperty(4)->GetSubItem(0)->SetData(14);
	GetProperty(4)->GetSubItem(1)->SetData(15);
	GetProperty(4)->GetSubItem(2)->SetData(16);
}

/*******************************************************
函数名:  ShowData
函数描述:显示数据
输入参数:iID----------显示数据的ID
*******************************************************/
void CPropertyGridCtrlPat::ShowData(TASK_INFO  &task_info)
{
	n_CruID = task_info.main_info.nID;
	unsigned int nSex, nAgeUnit;
	CString strRet, strDate, strTime, strTemp;
	// 基本信息
	GetProperty(0)->GetSubItem(0)->SetValue(_variant_t(unsigned int(task_info.main_info.nID)));
	GetProperty(0)->GetSubItem(1)->SetValue(_variant_t(unsigned int(task_info.main_info.nSN)));
	strRet = (LPCTSTR)_bstr_t(task_info.main_info.dtDate);
	strDate = strRet.Left(strRet.Find(' '));
	strTime = strRet.Right(strRet.GetLength() - strRet.Find(' ') - 1);
	GetProperty(0)->GetSubItem(2)->SetValue(strDate);
	GetProperty(0)->GetSubItem(3)->SetValue(strTime);

	// 患者资料
	GetProperty(1)->GetSubItem(0)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sCode));
	GetProperty(1)->GetSubItem(1)->SetValue((LPCTSTR)_bstr_t(task_info.pat_info.sName));
	nSex = task_info.pat_info.nSex;
	if (1 == nSex){strTemp = GetIdsString(_T("pat"),_T("IDS_BOY")).c_str();}
	else if (2 == nSex){strTemp = GetIdsString(_T("pat"),_T("IDS_GIRL")).c_str();}
	else {strTemp = GetIdsString(_T("pat"),_T("IDS_XX")).c_str();}
	GetProperty(1)->GetSubItem(2)->SetValue(strTemp);
	GetProperty(1)->GetSubItem(3)->SetValue(_variant_t(unsigned int(task_info.pat_info.nAge)));
	nAgeUnit = task_info.pat_info.nAgeUnit;
	if (1 == nAgeUnit){strTemp = GetIdsString(_T("pat"),_T("IDS_MONTH")).c_str();}
	else if (2 == nAgeUnit){strTemp = GetIdsString(_T("pat"),_T("IDS_DAY")).c_str();}
	else {strTemp = GetIdsString(_T("pat"),_T("IDS_YEAR")).c_str();}
	GetProperty(1)->GetSubItem(4)->SetValue(strTemp);
	GetProperty(1)->GetSubItem(5)->SetValue((LPCTSTR)_bstr_t(task_info.pat_info.sHospital));
	GetProperty(1)->GetSubItem(6)->SetValue((LPCTSTR)_bstr_t(task_info.pat_info.sSickBed));

	// 送检信息
	GetProperty(2)->GetSubItem(0)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sCompany));
	//	ResetDoctorOption();
	GetProperty(2)->GetSubItem(1)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sDepartment));
	GetProperty(2)->GetSubItem(2)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sDoctor));

	// 检验信息
	GetProperty(3)->GetSubItem(0)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sDocimaster));
	GetProperty(3)->GetSubItem(1)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sAuditor));
	GetProperty(3)->GetSubItem(2)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sReporter));

	/*GetProperty(3)->GetSubItem(2)->RemoveAllOptions();
	GetProperty(3)->GetSubItem(2)->AddOption(theApp.loginUser.sName);
	GetProperty(3)->GetSubItem(2)->SetValue(theApp.loginUser.sName);*/


	// 诊断信息
	GetProperty(4)->GetSubItem(0)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sColor));
	GetProperty(4)->GetSubItem(1)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sTransparency));
	GetProperty(4)->GetSubItem(2)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sDiagnosis));
	
}

/****************************************************
函数名:  ResetValueByHospitalNum
函数描述:根据住院号设置属性列表的值
****************************************************/
//void CPropertyGridCtrlPat::ResetValueByHospitalNum()
//{
//	CString strHosptial;
//	strHosptial = (LPCTSTR)_bstr_t(GetProperty(1)->GetSubItem(5)->GetValue());
//	strHosptial.TrimLeft();
//	strHosptial.TrimRight();
//	if(strHosptial.IsEmpty())
//		return;

	//unsigned int nSex, nAgeUnit;
	//CString strTemp;
	//TCHAR chSql[MAX_PATH] = _T("");
	//_stprintf_s(chSql, MAX_PATH, _T("SELECT * FROM tMain left outer join tPat on tMain.nID = tPat.nID\
	//								where sHospital = \'%s\'"), strHosptial);
	//m_Rs.Open(chSql, theApp.m_pConn->GetInterfacePtr(), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
	//if(!m_Rs.IsBOF())
	//{
	//	m_Rs.MoveLast();

	//	// 患者资料
	//	GetProperty(1)->GetSubItem(0)->SetValue(m_Rs.GetCollect(_T("sCode")));
	//	GetProperty(1)->GetSubItem(1)->SetValue(m_Rs.GetCollect(_T("sName")));
	//	nSex = (unsigned int)m_Rs.GetCollect(_T("nSex"));
	//	if (1 == nSex){strTemp = GetIdsString(_T("pat","IDS_BOY").c_str();}
	//	else if (2 == nSex){strTemp = GetIdsString(_T("pat","IDS_GIRL").c_str();}
	//	else {strTemp = _T(" ");}
	//	GetProperty(1)->GetSubItem(2)->SetValue(strTemp);
	//	GetProperty(1)->GetSubItem(3)->SetValue(m_Rs.GetCollect(_T("nAge")));
	//	nAgeUnit = (unsigned int)m_Rs.GetCollect(_T("nAgeUnit"));
	//	if (1 == nAgeUnit){strTemp = GetIdsString(_T("pat","IDS_MONTH").c_str();}
	//	else if (2 == nAgeUnit){strTemp = GetIdsString(_T("pat","IDS_DAY").c_str();}
	//	else {strTemp = GetIdsString(_T("pat","IDS_YEAR").c_str();}
	//	GetProperty(1)->GetSubItem(4)->SetValue(strTemp);
	//	GetProperty(1)->GetSubItem(6)->SetValue(m_Rs.GetCollect(_T("sSickBed")));
	//}
	//m_Rs.Close();
//}

/****************************************************
函数名:  SaveData
函数描述:保存数据
****************************************************/
    void CPropertyGridCtrlPat::SaveData()
 {
	m_bStringLenLegit = TRUE;
	CheckStringLen(GetProperty(1)->GetSubItem(0));
	CheckStringLen(GetProperty(1)->GetSubItem(1));
	CheckStringLen(GetProperty(1)->GetSubItem(5));
	CheckStringLen(GetProperty(1)->GetSubItem(6));

	CheckStringLen(GetProperty(2)->GetSubItem(0));
	CheckStringLen(GetProperty(2)->GetSubItem(1));
	CheckStringLen(GetProperty(2)->GetSubItem(2));

	CheckStringLen(GetProperty(3)->GetSubItem(0));
	CheckStringLen(GetProperty(3)->GetSubItem(1));
	CheckStringLen(GetProperty(3)->GetSubItem(2));

	CheckStringLen(GetProperty(4)->GetSubItem(0));
	CheckStringLen(GetProperty(4)->GetSubItem(1));

	if(!m_bStringLenLegit)
	{
		//AfxMessageBox(IDS_INPUT_TOO_LONG);
	}

	CString strSex, strAgeUnit,sCode,sName ,sHospital,sSickBed,sDepartment,sDoctor,sCompany,
		sDocimaster,sAuditor,sReporter,sColor,sTransparency,sDiagnosis;
	unsigned int nTemp = 0;
//	TCHAR chSql[MAX_PATH] = _T("");
//	_stprintf_s(chSql, MAX_PATH, _T("SELECT * FROM tMain left outer join tPat on tMain.nID=tPat.nID where tMain.nID=%d"), theApp.m_nID);
//	m_Rs.Open(chSql, theApp.m_pConn->GetInterfacePtr(), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
//	if(!m_Rs.IsBOF())
//	{
//		m_Rs.MoveFirst();
	TASK_INFO modifyInfo={0};
        //Access_UpdateTaskInfo(ALL_TYPE_INFO,theApp.m_nID,)
		// 基本资料
		unsigned int nSN = (unsigned int)(_variant_t(GetProperty(0)->GetSubItem(1)->GetValue()));
		if((nSN < 1) || (nSN > 10000))
			nSN = 1;

		GetProperty(0)->GetSubItem(1)->SetValue(_variant_t(nSN));
		modifyInfo.main_info.nSN = nSN;
		// 患者资料
		sCode=GetProperty(1)->GetSubItem(0)->GetValue();
		sName=GetProperty(1)->GetSubItem(1)->GetValue();
		strcpy(modifyInfo.main_info.sCode,Common::WCharToChar(sCode.GetBuffer()).c_str());
		strcpy(modifyInfo.pat_info.sName,Common::WCharToChar(sName.GetBuffer()).c_str());
		strSex = (LPCTSTR)_bstr_t(GetProperty(1)->GetSubItem(2)->GetValue());
		if (0 == _tcscmp(GetIdsString(_T("pat"),_T("IDS_BOY")).c_str(), strSex)){nTemp = 1;}
		else if (0 == _tcscmp(GetIdsString(_T("pat"),_T("IDS_GIRL")).c_str(), strSex)){nTemp = 2;}
		else {nTemp = 0;}
		modifyInfo.pat_info.nSex = nTemp;
        modifyInfo.pat_info.nAge = (unsigned int)(_variant_t(GetProperty(1)->GetSubItem(3)->GetValue()));

		strAgeUnit = (LPCTSTR)_bstr_t(GetProperty(1)->GetSubItem(4)->GetValue());
		if (0 == _tcscmp(GetIdsString(_T("pat"),_T("IDS_MONTH")).c_str(), strAgeUnit)){nTemp = 1;}
		else if (0 == _tcscmp(GetIdsString(_T("pat"),_T("IDS_DAY")).c_str(), strAgeUnit)){nTemp = 2;}
		else {nTemp = 0;}
		modifyInfo.pat_info.nAgeUnit = nTemp;


		sHospital=GetProperty(1)->GetSubItem(5)->GetValue();
		sSickBed=GetProperty(1)->GetSubItem(6)->GetValue();
		strcpy(modifyInfo.pat_info.sHospital,Common::WCharToChar(sHospital.GetBuffer()).c_str());
		strcpy(modifyInfo.pat_info.sSickBed,Common::WCharToChar(sSickBed.GetBuffer()).c_str());

		//		// 送检信息
		sDepartment=GetProperty(2)->GetSubItem(0)->GetValue();
		sDoctor=GetProperty(2)->GetSubItem(1)->GetValue();
		sCompany= GetProperty(2)->GetSubItem(2)->GetValue();
		strcpy(modifyInfo.main_info.sDepartment,Common::WCharToChar(sDepartment.GetBuffer()).c_str());
		strcpy(modifyInfo.main_info.sDoctor,Common::WCharToChar(sDoctor.GetBuffer()).c_str());
		strcpy(modifyInfo.main_info.sCompany,Common::WCharToChar(sCompany.GetBuffer()).c_str());

		//		// 检验信息
		sDocimaster=GetProperty(3)->GetSubItem(0)->GetValue();
		sAuditor=GetProperty(3)->GetSubItem(1)->GetValue();
		sReporter=GetProperty(3)->GetSubItem(2)->GetValue();
		strcpy(modifyInfo.main_info.sDocimaster,Common::WCharToChar(sDocimaster.GetBuffer()).c_str());
		strcpy(modifyInfo.main_info.sAuditor,Common::WCharToChar(sAuditor.GetBuffer()).c_str());
		strcpy(modifyInfo.main_info.sReporter,Common::WCharToChar(sReporter.GetBuffer()).c_str());

		//		// 诊断信息
		sColor=GetProperty(4)->GetSubItem(0)->GetValue();
		sTransparency=GetProperty(4)->GetSubItem(1)->GetValue();
		sDiagnosis=GetProperty(4)->GetSubItem(2)->GetValue();

		strcpy(modifyInfo.main_info.sColor,Common::WCharToChar(sColor.GetBuffer()).c_str());
		strcpy(modifyInfo.main_info.sTransparency,Common::WCharToChar(sTransparency.GetBuffer()).c_str());
		strcpy(modifyInfo.main_info.sDiagnosis,Common::WCharToChar(sDiagnosis.GetBuffer()).c_str());


		ULONG states = Access_UpdateTaskInfo(ALL_TYPE_INFO,theApp.m_nID,&modifyInfo);
  
 
   
	
	//if (states != STATUS_SUCCESS)
	//{
		
//	}


  }

/**************************************************************
函数名:  ResetValueByKey
函数描述:根据快捷键设置属性列表的值
输入参数:pPropName-----属性列表项名
pPropValue----属性列表项值
**************************************************************/
void CPropertyGridCtrlPat::ResetValueByKey(LPCTSTR pPropName, LPCTSTR pPropValue)
{
	//GetIdsString(_T("pat","IDS_SEND_DOCTOR").c_str()）;
	//CString strHospital = GetIdsString_T("pat","IDS_HOSPITAL_SN").c_str()）;

	//// 医生快捷键
	//if(0 == strDoctorPropName.Compare(pPropName))
	//{
	//	ResetDoctorValueByKey(pPropValue);
	//}
	//// 找相同住院号,填充旧的信息
	//else if(0 == strHospital.Compare(pPropName))
	//{
	//	ResetValueByHospitalNum();
	//}
	//else 
	//{
	//	// 其它快捷键
	//	ResetOtherValueByKey(pPropName, pPropValue);
	//}
	CString strHospital = GetProperty(2)->GetSubItem(0)->GetName();
	CString strDepartment = GetProperty(2)->GetSubItem(1)->GetName();
	CString strDoctorPropName = GetProperty(2)->GetSubItem(2)->GetName();
	if (0 == strHospital.Compare(pPropName))
	{
		ResetHospitalValueByKey(pPropValue);
	}
	else if (0 ==strDepartment.Compare(pPropName))
	{
		ResetDepartmentValueByKey(pPropValue);
	}
	else if (0 == strDoctorPropName.Compare(pPropName))
	{
		ResetDoctorValueByKey(pPropValue);
	}


	
}

void CPropertyGridCtrlPat::ResetDoctorValueByKey( LPCTSTR pPropValue )
{

	CString  strh = GetProperty(2)->GetSubItem(0)->GetValue();
	CString  strd = GetProperty(2)->GetSubItem(1)->GetValue();
	for (int i = 0 ; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		if (Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].name).c_str() == strh)
		{
			for (int j= 0; j < theApp.hospitalInfo.HospitalInfo[i].DepartmentCount; ++j)
			{
				if (Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name).c_str() == strd)
				{
					for (int z = 0; z < theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorCount ; ++z)
					{
						if (Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorInfo[z].key).c_str() == pPropValue)
						{
							GetProperty(2)->GetSubItem(2)->SetValue(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorInfo[z].name).c_str());
							break;
						}
					}
				}
			}

		}
	}
}


// 科室属性设置
void CPropertyGridCtrlPat::ResetDepartmentValueByKey( LPCTSTR pPropValue )
{
	CString  strh = GetProperty(2)->GetSubItem(0)->GetValue();
	for (int i = 0 ; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		if (Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].name).c_str() == strh)
		{
			for (int j= 0; j < theApp.hospitalInfo.HospitalInfo[i].DepartmentCount; ++j)
			{
				if (Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].key).c_str() == pPropValue)
				{
					GetProperty(2)->GetSubItem(1)->SetValue(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name).c_str());
					break;
				}
			}
			
		}
	}

}

/********************************************************************
函数名:  ResetDoctorValueByKey
函数描述:根据快捷键设置医生属性列表的值
输入参数:pPropValue----------属性列表项值
********************************************************************/
void CPropertyGridCtrlPat::ResetHospitalValueByKey(LPCTSTR pPropValue)
{

	GetProperty(2)->GetSubItem(0)->RemoveAllOptions();
	for (int i = 0 ; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		if ( 0 == _tcscmp(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].key).c_str(),pPropValue))
		{
			//GetProperty(2)->GetSubItem(0)->SetValue(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].name).c_str());
			GetProperty(2)->GetSubItem(0)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].name).c_str(),FALSE);
		}
	}


}

/**************************************************************************
函数名:  ResetOtherValueByKey
函数描述:根据快捷键设置除医生以外属性列表的值
输入参数:pPropName-------属性列表项名
pPropValue------属性列表项值
**************************************************************************/
//void CPropertyGridCtrlPat::ResetOtherValueByKey(LPCTSTR pPropName, LPCTSTR pPropValue)
//{
//	// 属性列表名和ini中键名的对应表
//	TCHAR chData[4][2][32] = {
//		_T("department"), DEPARTMENT_SECTION_NAME,
//		_T("hospital"), HOSPITAL_SECTION_NAME,
//		_T("color"), COLOR_SECTION_NAME,
//		_T("transparent"), TRANSPARENCY_SECTION_NAME
//	};
//	_tcscpy(chData[0][0], GetIdsString(_T("pat","IDS_SEND_DEPARTMENT").c_str());
//	_tcscpy(chData[1][0], GetIdsString(_T("pat","IDS_SEND_HOSPTIAL").c_str());
//	_tcscpy(chData[2][0], GetIdsString(_T("pat","IDS_RESULT_COLOR").c_str());
//	_tcscpy(chData[3][0], GetIdsString(_T("pat","IDS_RESULT_TRANSPARENT").c_str());
//
//	// 找操作的属性列表
//	int iValueType = 0;
//	int i;
//	for(i = 0; i < 4; i++)
//	{
//		if(0 == _tcscmp(chData[i][0], pPropName))
//		{
//			iValueType = i;
//			break;
//		}
//	}
//	// 如果属性列表快捷键不存储在ini文件中,则用属性列表可选项索引当做快捷键
//	if(4 == i)
//	{
//		int nKey = _ttoi(pPropValue);
//		int nOptionCount = 0;
//		int nPropertyIndex = -1;
//		int nSubItemIndex = -1;
//		CString strSex = GetIdsString(_T("pat","IDS_SEX").c_str();
//		CString strAgeUnit = GetIdsString(_T("pat","IDS_AGE_UNIT").c_str();
//		CString strChecker = GetIdsString(_T("pat","IDS_CHECKER").c_str();
//		CString strReChecker = GetIdsString(_T("pat","IDS_RECHECKER").c_str();
//		CString strReporter = GetIdsString(_T("pat","IDS_REPORTER").c_str();
//		if(0 == strSex.Compare(pPropName))
//		{
//			nPropertyIndex = 1;
//			nSubItemIndex = 2;
//		}
//		else if(0 == strAgeUnit.Compare(pPropName))
//		{
//			nPropertyIndex = 1;
//			nSubItemIndex = 4;
//		}
//		else if(0 == strChecker.Compare(pPropName))
//		{
//			nPropertyIndex = 3;
//			nSubItemIndex = 0;
//		}
//		else if(0 == strReChecker.Compare(pPropName))
//		{
//			nPropertyIndex = 3;
//			nSubItemIndex = 1;
//		}
//		else if(0 == strReporter.Compare(pPropName))
//		{
//			nPropertyIndex = 3;
//			nSubItemIndex = 2;
//		}
//		else
//		{
//			return;
//		}
//		nOptionCount = GetProperty(nPropertyIndex)->GetSubItem(nSubItemIndex)->GetOptionCount();
//		if((nKey > 0) && (nKey <= nOptionCount))
//		{
//			GetProperty(nPropertyIndex)->GetSubItem(nSubItemIndex)->SetValue(
//				GetProperty(nPropertyIndex)->GetSubItem(nSubItemIndex)->GetOption(nKey - 1));
//		}
//		return;
//	}
//
//	int iIndex = 0;
//	TCHAR chIndex[INDEX_BUF_LEN] = {0};
//	TCHAR chRetBuf[MAX_PATH] = {0};
//	TCHAR chName[NAME_BUF_LEN] = {0};
//	TCHAR chKey[KEY_BUF_LEN] = {0};
//	// 循环判断
//	while (1)
//	{
//		_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndex);
//		DWORD dwRet = GetPrivateProfileString(chData[iValueType][1], chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
//		if (dwRet > 0)
//		{
//			// 取项值和对应的快捷键
//			memset(chName, 0, sizeof(chName));
//			memset(chKey, 0, sizeof(chKey));
//			_stscanf(chRetBuf, _T("%[^,],%[^,]"), chName, chKey);
//			// 找到和输入key对应的项
//			CString strValueTemp = pPropValue;
//			strValueTemp = strValueTemp.TrimLeft();
//			strValueTemp = strValueTemp.TrimRight();
//			if(0 == _tcscmp(strValueTemp, chKey))
//			{
//				switch (iValueType)
//				{
//				case 0:
//					{
//						GetProperty(2)->GetSubItem(0)->SetValue(chName);
//						break;
//					}
//				case 1:
//					{
//						GetProperty(2)->GetSubItem(2)->SetValue(chName);
//						break;
//					}
//				case 2:
//					{
//						GetProperty(4)->GetSubItem(0)->SetValue(chName);
//						break;
//					}
//				case 3:
//					{
//						GetProperty(4)->GetSubItem(1)->SetValue(chName);
//						break;
//					}
//				default:
//					break;
//				}
//				break;
//			}
//		}
//		else
//		{
//			break;
//		}
//		iIndex++;
//	}
//}



/*******************************************************
函数名:  ResetUserOption
函数描述:重新设置用户属性列表的选项
*******************************************************/
void CPropertyGridCtrlPat::ResetUserOption()
{
	GetProperty(3)->GetSubItem(0)->RemoveAllOptions();
	GetProperty(3)->GetSubItem(1)->RemoveAllOptions();
	GetProperty(3)->GetSubItem(2)->RemoveAllOptions();
	GetProperty(3)->GetSubItem(0)->AllowEdit(TRUE);
	GetProperty(3)->GetSubItem(1)->AllowEdit(TRUE);
	GetProperty(3)->GetSubItem(2)->AllowEdit(TRUE);
	for (int i = 0; i <  theApp.m_userlist.nUserCounter; ++i )
	{
		CString strTemp;

		strTemp = Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sName).c_str();

		GetProperty(3)->GetSubItem(0)->AddOption(strTemp);
		GetProperty(3)->GetSubItem(1)->AddOption(strTemp);
		GetProperty(3)->GetSubItem(2)->AddOption(strTemp);

	}

	GetProperty(3)->GetSubItem(2)->AddOption(Common::CharToWChar(theApp.loginUser.sName).c_str());


}

/*******************************************************
函数名:  ResetOtherOption
函数描述:重新设置属性列表 
*******************************************************/
void CPropertyGridCtrlPat::ResetOtherOption()
{
	// 清除旧的选项
	//GetProperty(2)->GetSubItem(0)->RemoveAllOptions();
	//GetProperty(2)->GetSubItem(1)->RemoveAllOptions();
	//GetProperty(2)->GetSubItem(2)->RemoveAllOptions();
    _variant_t varStrDefault(_bstr_t(_T("")));
	CString strHospital;
	strHospital = (LPCTSTR)_bstr_t(GetProperty(2)->GetSubItem(0)->GetValue());
	CString strDepartment;
	strDepartment = (LPCTSTR)_bstr_t(GetProperty(2)->GetSubItem(1)->GetValue());
	
	GetProperty(2)->GetSubItem(1)->RemoveAllOptions();
	GetProperty(2)->GetSubItem(2)->RemoveAllOptions();
//	GetProperty(2)->GetSubItem(0)->AllowEdit(FALSE);
//	GetProperty(2)->GetSubItem(1)->AllowEdit(FALSE);
//	GetProperty(2)->GetSubItem(2)->AllowEdit(FALSE);
	for (int i = 0 ; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		if (!strcmp(theApp.hospitalInfo.HospitalInfo[i].name,Common::WCharToChar(strHospital.GetBuffer()).c_str()))
		{
			
			for (int j = 0; j < theApp.hospitalInfo.HospitalInfo[i].DepartmentCount ; ++j)
			{
				GetProperty(2)->GetSubItem(1)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name).c_str(), FALSE);

				if (!strcmp(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name,Common::WCharToChar(strDepartment.GetBuffer()).c_str()))
				{
					
					for (int z = 0 ; z < theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorCount; ++z)
					{
						GetProperty(2)->GetSubItem(2)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorInfo[z].name).c_str(), FALSE);
					}
					
					
				}


			}

			break;
		}
	
	}




	
	




	//GetProperty(4)->GetSubItem(0)->RemoveAllOptions();
	//GetProperty(4)->GetSubItem(1)->RemoveAllOptions();

	//TCHAR *pChar = NULL;
	//int iIndex = 0;
	//TCHAR chIndex[INDEX_BUF_LEN] = {0};
	//TCHAR chRetBuf[MAX_PATH] = {0};
	//// 设置送检信息
	//// 送检科室
	//while (1)
	//{
	//	_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndex);
	//	DWORD dwRet = GetPrivateProfileString(DEPARTMENT_SECTION_NAME, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
	//	if (dwRet > 0)
	//	{
	//		pChar = _tcschr(chRetBuf, ',');
	//		if(NULL != pChar)
	//		{
	//			*pChar = 0;
	//		}
	//		GetProperty(2)->GetSubItem(0)->AddOption(chRetBuf, FALSE);
	//	}
	//	else
	//	{
	//		break;
	//	}
	//	iIndex++;
	//}
	//ResetDoctorOption();
	//// 送检医院
	//iIndex = 0;
	//while (1)
	//{
	//	_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndex);
	//	DWORD dwRet = GetPrivateProfileString(HOSPITAL_SECTION_NAME, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
	//	if (dwRet > 0)
	//	{
	//		pChar = _tcschr(chRetBuf, ',');
	//		if(NULL != pChar)
	//		{
	//			*pChar = 0;
	//		}
	//		GetProperty(2)->GetSubItem(2)->AddOption(chRetBuf, FALSE);
	//	}
	//	else
	//	{
	//		break;
	//	}
	//	iIndex++;
	//}

	//// 设置诊断信息
	//// 颜色
	//iIndex = 0;
	//while (1)
	//{
	//	_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndex);
	//	DWORD dwRet = GetPrivateProfileString(COLOR_SECTION_NAME, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
	//	if (dwRet > 0)
	//	{
	//		pChar = _tcschr(chRetBuf, ',');
	//		if(NULL != pChar)
	//		{
	//			*pChar = 0;
	//		}
	//		GetProperty(4)->GetSubItem(0)->AddOption(chRetBuf, FALSE);
	//	}
	//	else
	//	{
	//		break;
	//	}
	//	iIndex++;
	//}
	//// 透明度
	//iIndex = 0;
	//while (1)
	//{
	//	_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndex);
	//	DWORD dwRet = GetPrivateProfileString(TRANSPARENCY_SECTION_NAME, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
	//	if (dwRet > 0)
	//	{
	//		pChar = _tcschr(chRetBuf, ',');
	//		if(NULL != pChar)
	//		{
	//			*pChar = 0;
	//		}
	//		GetProperty(4)->GetSubItem(1)->AddOption(chRetBuf, FALSE);

	//	}
	//	else
	//	{
	//		break;
	//	}
	//	iIndex++;
	//}
}

/*********************************************************
函数名:  ResetDoctorOption
函数描述:重新设置医生属性列表的选项
*********************************************************/
//void CPropertyGridCtrlPat::ResetDoctorOption()
//{
//	CString strDepartment;
//	strDepartment = (LPCTSTR)_bstr_t(GetProperty(2)->GetSubItem(0)->GetValue());
//
//	// 清除旧的设置
//	GetProperty(2)->GetSubItem(1)->RemoveAllOptions();
//
//	int iIndexDepartment = 0;
//	int iIndexDoctor = 0;
//	TCHAR *pChar = NULL;
//	TCHAR chIndex[INDEX_BUF_LEN] = {0};
//	TCHAR chRetBuf[MAX_PATH] = {0};
//	TCHAR chDepartment[NAME_BUF_LEN] = {0};
//	TCHAR chSection[SECTION_BUF_LEN] = {0};
//	TCHAR chName[NAME_BUF_LEN] = {0};
//	// 从ini文件取信息
//	while (1)
//	{
//		_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndexDepartment);
//		DWORD dwRet = GetPrivateProfileString(DEPARTMENT_SECTION_NAME, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
//		if (dwRet > 0)
//		{
//			// 取科室和它对应的节名
//			memset(chDepartment, 0, sizeof(chDepartment));
//			memset(chSection, 0, sizeof(chSection));
//			_stscanf(chRetBuf, _T("%[^,],%*[^,],%[^,]"), chDepartment, chSection);
//			// 找到传入的科室
//			if(0 == _tcscmp(strDepartment, chDepartment))
//			{
//				// 取属于科室的人名
//				iIndexDoctor = 0;
//				while (1)
//				{
//					_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndexDoctor);
//					DWORD dwRet = GetPrivateProfileString(chSection, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
//					if (dwRet > 0)
//					{
//						pChar = _tcschr(chRetBuf, ',');
//						if(NULL != pChar)
//						{
//							*pChar = 0;
//						}
//						_tcscpy_s(chName, NAME_BUF_LEN, chRetBuf);
//						GetProperty(2)->GetSubItem(1)->AddOption(chName, FALSE);
//					}
//					else
//					{
//						break;
//					}
//					iIndexDoctor++;
//				}
//			}
//		}
//		else
//		{
//			break;
//		}
//		iIndexDepartment++;
//	}
//}

/**************************************************************************
函数名:  OnKeyUp
函数描述:key up 此函数执行
*************************************************************************/
void CPropertyGridCtrlPat::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMFCPropertyGridCtrl::OnKeyUp(nChar, nRepCnt, nFlags);

	unsigned int nData = 0;
	switch (nChar)
	{
	case VK_TAB:
	case VK_RETURN:
		// Shift 上移
		if(NULL == GetCurSel())
			break;
		if (GetKeyState(VK_SHIFT) & 0xFF00)
		{
			nData = GetCurSel()->GetData() - 1;
		}
		else
		{
			nData = GetCurSel()->GetData() + 1;
		}
		// 范围限制
		if (nData <= 0)
		{
			nData = m_nMaxData;
		}
		else if (nData > m_nMaxData)
		{
			nData = 1;
		}
		// 设置焦点
		//SetCurSel(FindItemByData(nData)); 
		EditItem(FindItemByData(nData));
		break;

	}
}

//void CPropertyGridCtrlPat::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	CMFCPropertyGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
//	char lsChar;
//	lsChar=char(nChar);
//
//	if(lsChar=='A')
//	{
//		AfxMessageBox(_T(""));
//	}
//
//}

//限制字符串长度
void CPropertyGridCtrlPat::CheckStringLen(CMFCPropertyGridProperty *pProp, unsigned int nMaxLen)
{
	if(_tcslen((LPCTSTR)(_bstr_t)pProp->GetValue()) > nMaxLen)
	{
		pProp->SetValue(_T(""));
		m_bStringLenLegit = FALSE;
	}
}




//设置医院列表选项
void CPropertyGridCtrlPat::SetHospitalOption()
{
	GetProperty(2)->GetSubItem(0)->RemoveAllOptions();
	//GetProperty(2)->GetSubItem(0)->AllowEdit(TRUE);
	//GetProperty(2)->GetSubItem(0)->IsExpanded();
	//m_FindPropList.GetProperty(0)->GetSubItem(10)->RemoveAllOptions();
	for (int i = 0 ; i < theApp.hospitalInfo.HospitalCount; ++i )
	{
		GetProperty(2)->GetSubItem(0)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].name).c_str(), FALSE);
	}
	//GetProperty(2)->GetSubItem(0)->Expand(TRUE);
}

void CPropertyGridCtrlPat::ResetColorOption()
{
	GetProperty(4)->GetSubItem(0)->RemoveAllOptions();
	for (int i = 0 ; i < theApp.colorNum; ++i )
	{
		GetProperty(4)->GetSubItem(0)->AddOption(Common::CharToWChar(theApp.colorInfo[i].name).c_str(), FALSE);
	}
}

void CPropertyGridCtrlPat::ResetClearOption()
{
	GetProperty(4)->GetSubItem(1)->RemoveAllOptions();
	for (int i = 0 ; i < theApp.clearNum; ++i )
	{
		GetProperty(4)->GetSubItem(1)->AddOption(Common::CharToWChar(theApp.clearInfo[i].name).c_str(), FALSE);
	}
}

void CPropertyGridCtrlPat::ResetDiagnosOption()
{
	GetProperty(4)->GetSubItem(2)->RemoveAllOptions();
	for (int i = 0 ; i < theApp.diagnosNum; ++i )
	{
		GetProperty(4)->GetSubItem(2)->AddOption(Common::CharToWChar(theApp.diagnosInfo[i].name).c_str(), FALSE);
	}
}

void CPropertyGridCtrlPat::SetDefaultValue()
{
	_variant_t varStrDefault(_bstr_t(_T("")));
	_variant_t varIntDefault(unsigned int(0));

	GetProperty(0)->GetSubItem(0)->SetValue(varIntDefault);
	GetProperty(0)->GetSubItem(1)->SetValue(varIntDefault);
	GetProperty(0)->GetSubItem(2)->SetValue(varStrDefault);
	GetProperty(0)->GetSubItem(3)->SetValue(varStrDefault);
	GetProperty(1)->GetSubItem(0)->SetValue(varStrDefault);
	GetProperty(1)->GetSubItem(1)->SetValue(varStrDefault);
	GetProperty(1)->GetSubItem(2)->SetValue(varStrDefault);
	GetProperty(1)->GetSubItem(3)->SetValue(varIntDefault);
	GetProperty(1)->GetSubItem(5)->SetValue(varStrDefault);
	GetProperty(1)->GetSubItem(6)->SetValue(varStrDefault);
	GetProperty(2)->GetSubItem(0)->SetValue(varStrDefault);
	GetProperty(2)->GetSubItem(1)->SetValue(varStrDefault);
	GetProperty(2)->GetSubItem(2)->SetValue(varStrDefault);
	GetProperty(3)->GetSubItem(0)->SetValue(varStrDefault);
	GetProperty(3)->GetSubItem(1)->SetValue(varStrDefault);
	GetProperty(3)->GetSubItem(2)->SetValue(varStrDefault);
	GetProperty(4)->GetSubItem(0)->SetValue(varStrDefault);
	GetProperty(4)->GetSubItem(1)->SetValue(varStrDefault);
	GetProperty(4)->GetSubItem(2)->SetValue(varStrDefault);
	
}


void CPropertiesWndPat::CopyStr( CMFCPropertyGridProperty *pSrc, PCHAR pDst )
{
	char        temp[MAX_PATH];
	CString     str = pSrc->GetValue();

	if( str == "" )
	{
		pDst[0] = 0;
		return;
	}

	WideCharToMultiByte (CP_ACP, NULL, (LPCWSTR)str, -1, temp, MAX_PATH, NULL, FALSE);
	strcpy(pDst, temp);
}

int CPropertiesWndPat::GetSex( CMFCPropertyGridProperty *pSex )
{
	CString strSex = pSex->GetValue();

	if ( _tcscmp(GetIdsString(_T("pat"),_T("IDS_BOY")).c_str(), strSex) == 0 )
		return 1;
	
	else if ( _tcscmp(GetIdsString(_T("pat"),_T("IDS_GIRL")).c_str(), strSex) == 0 )
		return 2;

	
	return 0;
}

int CPropertiesWndPat::AgeUnit( CMFCPropertyGridProperty *pAgeUnit )
{
	CString strAgeUnit = pAgeUnit->GetValue();

	if (_tcscmp(GetIdsString(_T("pat"),_T("IDS_MONTH")).c_str(), strAgeUnit) == 0 )
		return 1;

	else if ( _tcscmp(GetIdsString(_T("pat"),_T("IDS_DAY")).c_str(), strAgeUnit) == 0 )
		return 2;

	return 0;
}

void CPropertiesWndPat::clearOption()
{
   m_WndPropList.clearOption();
}

void CPropertiesWndPat::clearDoctorOption()
{
	m_WndPropList.clearDoctorOption();
}

void CPropertiesWndPat::OnContextMenu( CWnd* /*pWnd*/, CPoint point )
{

}

void CPropertyGridCtrlPat::clearOption()
{
	_variant_t varStrDefault(_bstr_t(_T("")));
	GetProperty(2)->GetSubItem(1)->SetValue(varStrDefault);
	GetProperty(2)->GetSubItem(2)->SetValue(varStrDefault);
}

void CPropertyGridCtrlPat::clearDoctorOption()
{
	_variant_t varStrDefault(_bstr_t(_T("")));
	GetProperty(2)->GetSubItem(2)->SetValue(varStrDefault);
}



//void CPropertiesWndPat::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	unsigned int nData = 0;
//	switch (nChar)
//	{
//	case VK_TAB:
//	case VK_RETURN:
//		// Shift 上移
//		//if(NULL == GetCurSel())
//		//	break;
//		//if (GetKeyState(VK_SHIFT) & 0xFF00)
//		//{
//		//	nData = GetCurSel()->GetData() - 1;
//		//}
//		//else
//		//{
//		//	nData = GetCurSel()->GetData() + 1;
//		//}
//		//// 范围限制
//		//if (nData <= 0)
//		//{
//		//	nData = m_nMaxData;
//		//}
//		//else if (nData > m_nMaxData)
//		//{
//		//	nData = 1;
//		//}
//		//// 设置焦点
//		//SetCurSel(FindItemByData(nData));
//		break;
//	}
//
//	CDockablePane::OnKeyUp(nChar, nRepCnt, nFlags);
//}

//BOOL CPropertyGridCtrlPat::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//
//
//	return CMFCPropertyGridCtrl::PreTranslateMessage(pMsg);
//}







//BOOL CPropertiesWndPat::PreTranslateMessage(MSG* pMsg)
//{
	// TODO: 在此添加专用代码和/或调用基类
//	if (pMsg->message == WM_KEYDOWN)
//	{
	//	CString strInput=_T("");
		//if((pMsg->wParam>='A')&&(pMsg->wParam<='Z'))//直接用上面的虚码代替就可以响应所指键
		//{
		//	for (int i = 0 ; i < 3 ; ++i)
		//	{
		//		if (m_WndPropList.GetProperty(2)->GetSubItem(i)->IsSelected())
		//		{
		//			switch (pMsg->wParam)
		//			{
		//			case 'A':
		//				strInput = _T("A");
		//				break;
		//			case 'B':
		//				strInput = _T("B");
		//				break;
		//			case 'C':
		//				strInput = _T("C");
		//				break;
		//			case 'D':
		//				strInput = _T("D");
		//				break;
		//			case 'E':
		//				strInput = _T("E");
		//				break;
		//			case 'F':
		//				strInput = _T("F");
		//				break;
		//			case 'G':
		//				strInput = _T("G");
		//				break;
		//			case 'H':
		//				strInput = _T("H");
		//				break;
		//			case 'I':
		//				strInput = _T("T");
		//				break;
		//			case 'J':
		//				strInput = _T("J");
		//				break;
		//			case 'K':
		//				strInput = _T("K");
		//				break;
		//			case 'L':
		//				strInput = _T("L");
		//				break;
		//			case 'M':
		//				strInput = _T("M");
		//				break;
		//			case 'N':
		//				strInput = _T("N");
		//				break;
		//			case 'O':
		//				strInput = _T("O");
		//				break;
		//			case 'P':
		//				strInput = _T("P");
		//				break;
		//			case 'Q':
		//				strInput = _T("Q");
		//				break;
		//			case 'R':
		//				strInput = _T("R");
		//				break;
		//			case 'S':
		//				strInput = _T("S");
		//				break;
		//			case 'T':
		//				strInput = _T("T");
		//				break;
		//			case 'U':
		//				strInput = _T("U");
		//				break;
		//			case 'V':
		//				strInput = _T("V");
		//				break;
		//			case 'W':
		//				strInput = _T("W");
		//				break;
		//			case 'X':
		//				strInput = _T("X");
		//				break;
		//			case 'Y':
		//				strInput = _T("Y");
		//				break;
		//			case 'Z':
		//				strInput = _T("Z");
		//				break;


		//			}
		//		}
		//		if ( strInput.GetLength() > 0)
		//		{
		//			
		//			
  //              //this->UpdateData(FALSE);
		//		   // m_WndPropList.GetProperty(2)->GetSubItem(i)->RemoveAllOptions();	
		//			//m_WndPropList.ResetValueByKey((LPCTSTR)_bstr_t(m_WndPropList.GetProperty(2)->GetSubItem(i)->GetName()), strInput);
		//			//m_WndPropList.EditItem(m_WndPropList.GetProperty(2)->GetSubItem(0));
		//			//::SendMessage(this->m_hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM( CursorPos.x, CursorPos.y ) );
		//								// m_WndPropList.GetProperty(2)->Show();
		//			//m_WndPropList.GetProperty(2)->Redraw();
		//			
		//		this->UpdateData(TRUE);
		//		CPoint CursorPos;
		//		m_WndPropList.GetProperty(2)->GetSubItem(0)->OnClickButton(CursorPos);
		//			
		//			break;
		//		}
		//		

		//		
		//	}


		//}
		//else
		//{
  //        switch (pMsg->wParam)
  //        {
		//  case VK_BACK:
		//  case VK_DELETE:
		//  case VK_CLEAR:
		//	  if (strInput.GetLength() == 0)
		//	  {
		//		  
		//		  m_WndPropList.SetHospitalOption();
		//		  
		//	  }
		//	  break;
  //        }
		//  CPoint CursorPos;
		//  m_WndPropList.GetProperty(2)->GetSubItem(0)->OnClickButton(CursorPos);
		//}

		//m_WndPropList.Invalidate();

	//}
	

	//return 0; //CDockablePane::PreTranslateMessage(pMsg);
//}