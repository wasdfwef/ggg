/*****************************************************************************
Copyright (C), 1995-2010, Chongqing Tianhai Medical Equipment Co., Ltd.
文件名:		FindRecord.cpp
功能:		查找记录
时间:		2010-05
*******************************************************************************/

#include "stdafx.h"
#include "UW.h"
#include "FindRecord.h"
#include "Include/Common/String.h"
//#include "ComValue.h"
#include "MainFrm.h"
#include "DbgMsg.h"

// CFindRecord
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

IMPLEMENT_DYNAMIC(CFindRecord, CDockablePane)

CFindRecord::CFindRecord()
{
}

CFindRecord::~CFindRecord()
{
}


BEGIN_MESSAGE_MAP(CFindRecord, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_BN_CLICKED(IDC_FINDRECORD_SEARCH, &CFindRecord::OnBnClickedSearch)
	ON_UPDATE_COMMAND_UI(IDC_FINDRECORD_SEARCH, &CFindRecord::OnUpdateSearch)
END_MESSAGE_MAP()

// CFindRecord 消息处理程序

/**********************************************
函数名:  InitFindList
函数描述:初始化查询条件属性列表
**********************************************/
void CFindRecord::InitFindList()
{
	m_FindPropList.EnableHeaderCtrl(FALSE);
	m_FindPropList.EnableDescriptionArea(FALSE);
	m_FindPropList.SetVSDotNetLook(TRUE);
	m_FindPropList.MarkModifiedProperties(FALSE);
	_variant_t varStrDefault(_bstr_t(_T("")));
	_variant_t varIntDefault(unsigned int(0));

	//查询条件
	CMFCPropertyGridProperty *pProGroup = new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("title")).c_str(), 0, FALSE);
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("sn")).c_str(), varIntDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridDateTimeProperty(GetIdsString(_T("findrecord"),_T("date")).c_str(), varStrDefault, NULL, 0));

	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("time")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("date1")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("time1")).c_str(), varStrDefault, NULL, 0));



	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("barcode")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("name")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("hospitalsn")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("hospital")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("department")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("doctor")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("checker")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("rechecker")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("reporter")).c_str(), varStrDefault, NULL, 0));
	pProGroup->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("findrecord"),_T("bQc")).c_str(), varStrDefault, NULL, 0));

	m_FindPropList.AddProperty(pProGroup);

	pProGroup->GetSubItem(1)->AllowEdit(FALSE);
	pProGroup->GetSubItem(3)->AllowEdit(FALSE);
	pProGroup->GetSubItem(2)->AllowEdit(FALSE);
	pProGroup->GetSubItem(4)->AllowEdit(FALSE);
	pProGroup->GetSubItem(2)->Enable(FALSE);
	pProGroup->GetSubItem(4)->Enable(FALSE);

	//配置属性列表
	ConfigPropList();
}

/*************************************************
函数名:  ConfigPropList
函数描述:配置属性列表
*************************************************/
void CFindRecord::ConfigPropList()
{
	//设置检验信息
	ResetUserOption();

	//设置科室医院选项
	ResetOtherOption();
}


/*************************************************
函数名:  ResetUserOption
函数描述:重新设置用户属性列表的选项
*************************************************/
void CFindRecord::ResetUserOption()
{
	m_FindPropList.GetProperty(0)->GetSubItem(11)->RemoveAllOptions();
	m_FindPropList.GetProperty(0)->GetSubItem(12)->RemoveAllOptions();
	m_FindPropList.GetProperty(0)->GetSubItem(13)->RemoveAllOptions();

	for (int i = 0; i <  theApp.m_userlist.nUserCounter; ++i )
	{
		CString strTemp;

		strTemp = Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sName).c_str();

		m_FindPropList.GetProperty(0)->GetSubItem(11)->AddOption(strTemp);
		m_FindPropList.GetProperty(0)->GetSubItem(12)->AddOption(strTemp);
		m_FindPropList.GetProperty(0)->GetSubItem(13)->AddOption(strTemp);
		
	}
	

		CString strTemp;
		strTemp = GetIdsString(_T("findrecord"),_T("1")).c_str();
		m_FindPropList.GetProperty(0)->GetSubItem(14)->AddOption(strTemp);
		strTemp = GetIdsString(_T("findrecord"),_T("2")).c_str();
		m_FindPropList.GetProperty(0)->GetSubItem(14)->AddOption(strTemp);
		//m_FindPropList.GetProperty(0)->GetSubItem(14)->SetValue()
	
}

int CFindRecord::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//创建
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_FindPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_GRID_PROPERTY3))
	{
		OutputDebugString(_T("未能创建查询条件属性列表"));
		return -1;
	}

	m_btnSearch.Create(GetIdsString(_T("findrecord"),_T("title")).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, CRect(10,10,20,20), this, IDC_FINDRECORD_SEARCH);

	//调整窗口
	AdjustLayout();

	return 0;
}

void CFindRecord::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFindRecord::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
}

/**************************************
函数名:  AdjustLayout
函数描述:调整窗口
**************************************/
void CFindRecord::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	//
	CRect rectClient;
	GetClientRect(rectClient);
	m_FindPropList.SetWindowPos(NULL
		, rectClient.left, rectClient.top, rectClient.Width() * 3 / 4, rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	//
	if(0 != m_FindPropList.GetPropertyCount())
		m_FindPropList.GetProperty(0)->GetSubItem(1)->AdjustButtonRect();

	m_btnSearch.SetWindowPos(NULL
		, rectClient.left + rectClient.Width() * 3 / 4
		, rectClient.top
		, rectClient.Width() * 1 / 4
		, rectClient.Height()
		, SWP_NOACTIVATE | SWP_NOZORDER);
}

LRESULT CFindRecord::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	//if (this->m_FindPropList.GetProperty(0)->GetSubItem(8)->IsValueChanged())
	//{
	//	_variant_t varStrDefault(_bstr_t(_T("")));
	//	m_FindPropList.GetProperty(0)->GetSubItem(9)->SetValue(varStrDefault);
	//	m_FindPropList.GetProperty(0)->GetSubItem(10)->SetValue(varStrDefault);
	//}
	

	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lParam;
	int i= (int)pProp->GetData();
	//pProp->GetSubItem(m_nIndex);
	CString s = pProp->GetName();
    _variant_t varStrDefault(_bstr_t(_T("")));
	CString strName = m_FindPropList.GetProperty(0)->GetSubItem(8)->GetName();
	if (s == strName)
	{
		COleVariant t = pProp->GetValue();
		CString subValue = t.bstrVal;
		t = pProp->GetOriginalValue();
		CString strNow;
		strNow = t.bstrVal;
		if (strNow != subValue)
		{
			m_FindPropList.GetProperty(0)->GetSubItem(9)->RemoveAllOptions();
			m_FindPropList.GetProperty(0)->GetSubItem(10)->RemoveAllOptions();
			m_FindPropList.GetProperty(0)->GetSubItem(9)->SetValue(varStrDefault);
			m_FindPropList.GetProperty(0)->GetSubItem(10)->SetValue(varStrDefault);
			ResetDepartmentOption();
		}
	}
	//if (s == )
	//{
	//}

	if (s == m_FindPropList.GetProperty(0)->GetSubItem(9)->GetName())
	{
		COleVariant t1 = pProp->GetValue();
		CString subValue1 = t1.bstrVal;
		t1 = pProp->GetOriginalValue();
		CString strNow1;
		strNow1= t1.bstrVal;
		if (strNow1 != subValue1)
		{
			m_FindPropList.GetProperty(0)->GetSubItem(10)->RemoveAllOptions();
			m_FindPropList.GetProperty(0)->GetSubItem(10)->SetValue(varStrDefault);
			ResetDoctorOption();
		}
	}
	
	
	

	
	return 0;
}

void CFindRecord::OnBnClickedSearch()
{
	BeginWaitCursor();
	Search();
	EndWaitCursor();
}

void CFindRecord::OnUpdateSearch(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

/*******************************************************
函数名:  ResetOtherOption
函数描述:重新设置属性列表 
*******************************************************/
void CFindRecord::ResetOtherOption()
{
	//清除旧的选项
	m_FindPropList.GetProperty(0)->GetSubItem(8)->RemoveAllOptions();
	//m_FindPropList.GetProperty(0)->GetSubItem(10)->RemoveAllOptions();

	

	           
	for (int i = 0 ; i < theApp.hospitalInfo.HospitalCount; ++i )
	{
		m_FindPropList.GetProperty(0)->GetSubItem(8)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].name).c_str(), FALSE);
	
	}
	



	//设置送检信息
	//送检科室
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
	//		m_FindPropList.GetProperty(0)->GetSubItem(5)->AddOption(chRetBuf, FALSE);
	//	}
	//	else
	//	{
	//		break;
	//	}
	//	iIndex++;
	//}
	//送检医院
	/*iIndex = 0;
	while (1)
	{
		_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndex);
		DWORD dwRet = GetPrivateProfileString(HOSPITAL_SECTION_NAME, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
		if (dwRet > 0)
		{
			pChar = _tcschr(chRetBuf, ',');
			if(NULL != pChar)
			{
				*pChar = 0;
			}
			m_FindPropList.GetProperty(0)->GetSubItem(7)->AddOption(chRetBuf, FALSE);
		}
		else
		{
			break;
		}
		iIndex++;
	}*/
}

void CFindRecord::ResetDepartmentOption()
{
	CString strHospital;
	strHospital = (LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(8)->GetValue());
	for (int i = 0 ; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		if (!strcmp(theApp.hospitalInfo.HospitalInfo[i].name,Common::WCharToChar(strHospital.GetBuffer()).c_str()))
		{
			for (int j = 0; j < theApp.hospitalInfo.HospitalInfo[i].DepartmentCount ; ++j)
			{
				m_FindPropList.GetProperty(0)->GetSubItem(9)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name).c_str(), FALSE);
			}
		}
	}
}

/*********************************************************
函数名:  ResetDoctorOption
函数描述:重新设置医生属性列表的选项
*********************************************************/
void CFindRecord::ResetDoctorOption()
{
	


	CString strHospital;
	strHospital = (LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(8)->GetValue());
	CString strDepartment;
	strDepartment = (LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(9)->GetValue());

	//清除旧的设置
	//m_FindPropList.GetProperty(0)->GetSubItem(9)->RemoveAllOptions();
	//m_FindPropList.GetProperty(0)->GetSubItem(10)->RemoveAllOptions();
	


  
	for (int i = 0 ; i < theApp.hospitalInfo.HospitalCount; ++i)
	{
		if (!strcmp(theApp.hospitalInfo.HospitalInfo[i].name,Common::WCharToChar(strHospital.GetBuffer()).c_str()))
		{
			for (int j = 0; j < theApp.hospitalInfo.HospitalInfo[i].DepartmentCount ; ++j)
			{
				//m_FindPropList.GetProperty(0)->GetSubItem(9)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name).c_str(), FALSE);

				if (!strcmp(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].name,Common::WCharToChar(strDepartment.GetBuffer()).c_str()))
				{
					for (int z = 0 ; z < theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorCount; ++z)
					{
						m_FindPropList.GetProperty(0)->GetSubItem(10)->AddOption(Common::CharToWChar(theApp.hospitalInfo.HospitalInfo[i].DepartmentInfo[j].DoctorInfo[z].name).c_str(), FALSE);
					}
					break;
				}
			}
		}


	}




	//TCHAR chDepartment[NAME_BUF_LEN] = {0};
	//TCHAR chSection[SECTION_BUF_LEN] = {0};
	//TCHAR chName[NAME_BUF_LEN] = {0};
	////从ini文件取信息
	//while (1)
	//{
	//	_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndexDepartment);
	//	DWORD dwRet = GetPrivateProfileString(DEPARTMENT_SECTION_NAME, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
	//	if (dwRet > 0)
	//	{
	//		//取科室和它对应的节名
	//		memset(chDepartment, 0, sizeof(chDepartment));
	//		memset(chSection, 0, sizeof(chSection));
	//		_stscanf(chRetBuf, _T("%[^,],%*[^,],%[^,]"), chDepartment, chSection);
	//		//找到传入的科室
	//		if(0 == _tcscmp(strDepartment, chDepartment))
	//		{
	//			//取属于科室的人名
	//			iIndexDoctor = 0;
	//			while (1)
	//			{
	//				_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndexDoctor);
	//				DWORD dwRet = GetPrivateProfileString(chSection, chIndex, NULL, chRetBuf, MAX_PATH, theApp.m_chConfigDataIniPath);
	//				if (dwRet > 0)
	//				{
	//					pChar = _tcschr(chRetBuf, ',');
	//					if(NULL != pChar)
	//					{
	//						*pChar = 0;
	//					}
	//					_tcscpy_s(chName, NAME_BUF_LEN, chRetBuf);
	//					m_FindPropList.GetProperty(0)->GetSubItem(6)->AddOption(chName, FALSE);
	//				}
	//				else
	//				{
	//					break;
	//				}
	//				iIndexDoctor++;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		break;
	//	}
	//	iIndexDepartment++;
	//}
}

/**************************************************
函数名:  CheckDateLegit
函数描述:日期值是否合法
*************************************************/
BOOL CFindRecord::CheckDateLegit(CString strDate)
{
	BOOL bLegit = TRUE;
	strDate.TrimLeft();
	strDate.TrimRight();
	if (!strDate.IsEmpty()) 
	{
		COleDateTime datetime;
		if(!datetime.ParseDateTime(strDate, VAR_DATEVALUEONLY))
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

/**************************************************
函数名:  Search
函数描述:查找记录
*************************************************/
void CFindRecord::Search()
{
	//检查日期是否合法
	CString             strDate;
	CMainFrame         *pMainFrame = (CMainFrame*)theApp.GetMainWnd();

	//禁止在做任务的状态进行查询
	if( theApp.IsIdleStatus() == FALSE )
		return;

	pMainFrame->m_wndTodayList.AutoSaveCurRecord();
	strDate = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(1)->GetValue());
	if (!CheckDateLegit(strDate))
	{
		return;
	}
	strDate.TrimLeft();
	strDate.TrimRight();



	/*unsigned int nSN;
	CString strCode, strDepartment, strDoctor, strCompany, strDocimaster, strAuditor, strReporter;
	CString strName, strHospital;
	nSN = (unsigned int)_variant_t(m_FindPropList.GetProperty(0)->GetSubItem(0)->GetValue());
	strCode = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(5)->GetValue());
	strName = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(6)->GetValue());
	strHospital = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(7)->GetValue());
	strDepartment = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(8)->GetValue());
	strDoctor = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(9)->GetValue());
	strCompany = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(10)->GetValue());
	strDocimaster = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(11)->GetValue());
	strAuditor = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(12)->GetValue());
	strReporter = (LPCSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(13)->GetValue());
	if(0 != nSN)
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and nSN=%d"), nSN);
		_tcscat(chSql, chTemp);
	}
	if(!strDate.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and DateDiff('D', dtDate, '%s') = 0"), strDate);
		_tcscat(chSql, chTemp);
	}
	if(!strCode.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sCode=\'%s\'"), strCode);
		_tcscat(chSql, chTemp);
	}
	if(!strName.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sName LIKE \'%%%s%%\'"), strName);
		_tcscat(chSql, chTemp);
	}
	if(!strHospital.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sHospital=\'%s\'"), strHospital);
		_tcscat(chSql, chTemp);
	}
	if(!strDepartment.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sDepartment=\'%s\'"), strDepartment);
		_tcscat(chSql, chTemp);
	}
	if(!strDoctor.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sDoctor=\'%s\'"), strDoctor);
		_tcscat(chSql, chTemp);
	}
	if(!strCompany.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sCompany=\'%s\'"), strCompany);
		_tcscat(chSql, chTemp);
	}
	if(!strDocimaster.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sDocimaster=\'%s\'"), strDocimaster);
		_tcscat(chSql, chTemp);
	}
	if(!strAuditor.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sAuditor=\'%s\'"), strAuditor);
		_tcscat(chSql, chTemp);
	}
	if(!strReporter.IsEmpty())
	{
		memset(chTemp, 0, sizeof(chTemp));
		_stprintf_s(chTemp, MAX_PATH, _T(" and sReporter=\'%s\'"), strReporter);
		_tcscat(chSql, chTemp);
	}*/

	ZeroMemory(&theApp.searchtask,sizeof(SEARCH_TASK));

    if ( m_FindPropList.GetProperty(0)->GetSubItem(14)->GetValue().bstrVal == GetIdsString(_T("findrecord"),_T("2")) )
    {
		theApp.searchtask.bIsSearch_QC_Task = true;
    }
	else
	{
		theApp.searchtask.bIsSearch_QC_Task = false;
	}
	
	
		
	strcpy(theApp.searchtask.conditions.sDate_From,Common::WCharToChar(strDate.GetBuffer()).c_str());
	strcpy(theApp.searchtask.conditions.sTime_From,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(2)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sDate_To,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(3)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sTime_To,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(4)->GetValue())).c_str());

	theApp.searchtask.conditions.nSN = (unsigned int)_variant_t(m_FindPropList.GetProperty(0)->GetSubItem(0)->GetValue());
	strcpy(theApp.searchtask.conditions.sBarcode,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(5)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sName,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(6)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sHospital,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(7)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sCompany,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(8)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sDepartment,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(9)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sDoctor,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(10)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sDocimaster,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(11)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sAuditor,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(12)->GetValue())).c_str());
	strcpy(theApp.searchtask.conditions.sReporter,Common::WCharToChar((LPCTSTR)_bstr_t(m_FindPropList.GetProperty(0)->GetSubItem(13)->GetValue())).c_str());
	
    
	ULONG findstate = Access_SearchRecord(&theApp.searchtask);
	DBG_MSG("theApp.searchtask.pSearchInfoCounts :%d\n",theApp.searchtask.pSearchInfoCounts);
	//生成查询语句
	//TCHAR chSql[4 * MAX_PATH] = _T("SELECT * FROM tMain left outer join tPat on tMain.nID=tPat.nID where 1=1");
	//TCHAR chTemp[MAX_PATH] = {0};
	
	//_tcscat(chSql, _T(" ORDER BY tMain.nID"));

	//CString sdbg;
	//sdbg.Format(_T("CFindRecord::Search chSql:%s\n"),chSql);
	//OutputDebugString(sdbg);



	////限制sql语句长度
	//if(_tcslen(chSql) > (4 * MAX_PATH - 10))
	//	return;

	//memset(((CMainFrame*)theApp.GetMainWnd())->m_wndRecordList.m_szCurrentSql, 0, sizeof(((CMainFrame*)theApp.GetMainWnd())->m_wndRecordList.m_szCurrentSql));
	//_tcscpy(((CMainFrame*)theApp.GetMainWnd())->m_wndRecordList.m_szCurrentSql, chSql);
	((CMainFrame*)theApp.GetMainWnd())->m_wndTodayList.UpdateRecordList();//m_wndRecordList.UpdateRecordList();
	((CUdcView*)(((CMainFrame*)theApp.GetMainWnd())->GetUWView()->GetTabWnd(THUM_UDC_TAB)))->UpdateSearchRecordResult();
}

void CFindRecord::OnContextMenu( CWnd* /*pWnd*/, CPoint point )
{

}





// CMFCPropertyGridDateTimeProperty

#define AFX_PROP_HAS_LIST 0x0001
#define AFX_PROP_HAS_BUTTON 0x0002
#define AFX_PROP_HAS_SPIN 0x0004
CMFCPropertyGridDateTimeProperty::CMFCPropertyGridDateTimeProperty(const CString& strName
	, const _variant_t& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData) 
	:CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData)
{
	m_dwFlags = AFX_PROP_HAS_BUTTON;
}

CMFCPropertyGridDateTimeProperty::~CMFCPropertyGridDateTimeProperty()
{
}

// CMFCPropertyGridDateTimeProperty 成员函数

void CMFCPropertyGridDateTimeProperty::OnClickButton(CPoint point)
{
	CFindRecordSetDate dlg;
	if(dlg.DoModal() == IDOK)
	{
		SetValue(dlg.m_strDate);
		((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.m_start_time.SetWindowTextW(dlg.m_strTime);
		((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.m_end_date.SetWindowTextW(dlg.m_strDate2);
		((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.m_end_time.SetWindowTextW(dlg.m_strTime2);
	}
}

// CFindRecordSetDate 对话框

IMPLEMENT_DYNAMIC(CFindRecordSetDate, CDialog)

CFindRecordSetDate::CFindRecordSetDate(CWnd* pParent /*=NULL*/)
: CDialog(CFindRecordSetDate::IDD, pParent)
, m_strDate(_T(""))
{

}

CFindRecordSetDate::~CFindRecordSetDate()
{
}

void CFindRecordSetDate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_DateTimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_TimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER3, m_DateCtrl2);
	DDX_Control(pDX, IDC_DATETIMEPICKER4, m_TimeCtrl2);
}


BEGIN_MESSAGE_MAP(CFindRecordSetDate, CDialog)
	ON_BN_CLICKED(IDOK, &CFindRecordSetDate::OnBnClickedOk)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER4, &CFindRecordSetDate::OnDtnDatetimechangeDatetimepicker4)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, &CFindRecordSetDate::OnDtnDatetimechangeDatetimepicker2)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CFindRecordSetDate::OnDtnDatetimechangeDatetimepicker1)
END_MESSAGE_MAP()

// CFindRecordSetDate 消息处理程序

void CFindRecordSetDate::OnBnClickedOk()
{
	COleDateTime datetime;
	m_DateTimeCtrl.GetTime(datetime);
	
	COleDateTime  date2;
    m_DateCtrl2.GetTime(date2);
	
	m_strDate.Format(_T("%04d-%02d-%02d"), datetime.GetYear(), datetime.GetMonth(), datetime.GetDay());
	m_strTime.Format(_T("%02d:%02d:%02d"),time1.GetHour(),time1.GetMinute(),time1.GetSecond());
	m_strDate2.Format(_T("%04d-%02d-%02d"),date2.GetYear(), date2.GetMonth(), date2.GetDay());
	m_strTime2.Format(_T("%02d:%02d:%02d"),time2.GetHour(),time2.GetMinute(),time2.GetSecond());

	CString warning = GetIdsString(_T("warning"),_T("dateerror")).c_str();

	if(m_strDate > m_strDate2)
	{
		MessageBox(warning, _T("UW"), MB_OK);
	}
	else if (m_strDate == m_strDate2 && m_strTime > m_strTime2)
	{
		MessageBox(warning, _T("UW"), MB_OK);
	}
	else
	{
		OnOK();
	}
}

BOOL CFindRecordSetDate::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	time1.SetTime(0,0,0);
	time2.SetTime(23,59,59);
	m_TimeCtrl.SetTime(time1);
	m_TimeCtrl2.SetTime(time2);
	COleDateTime  data1,date2;
	SYSTEMTIME systime1;
	::GetSystemTime(&systime1);
	data1.SetDate(systime1.wYear,systime1.wMonth,systime1.wDay);
    date2.SetDate(2000,1,1);
	
	
    m_DateTimeCtrl.SetRange(&date2,&data1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFindRecordSetDate::OnDtnDatetimechangeDatetimepicker4(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_TimeCtrl2.GetTime(time2);
	*pResult = 0;
}

void CFindRecordSetDate::OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_TimeCtrl.GetTime(time1);
	*pResult = 0;
}

void CFindRecordSetDate::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	COleDateTime datetime,datetime2;
	m_DateTimeCtrl.GetTime(datetime);
	SYSTEMTIME systime1;
	::GetSystemTime(&systime1);
	datetime2.SetDate(systime1.wYear,systime1.wMonth,systime1.wDay);
	m_DateCtrl2.SetRange(&datetime,&datetime2);
	*pResult = 0;
}
