/*****************************************************************************
Copyright (C), 1995-2010, Chongqing Tianhai Medical Equipment Co., Ltd.
文件名:		PropertiesWndRet.cpp
功能:		病人检测结果显示
时间:		2010-05
*******************************************************************************/

#include "stdafx.h"
#include "UW.h"
#include "PropertiesWndRet.h"
#include "Include/Common/String.h"
#include "MainFrm.h"
#include "DbgMsg.h"
#include "THUIInterface.h"
#include "..\..\..\inc\THDBUW_Access.h"
#include "DRBCImage.h"
#define UDC_MAXITEM          16
#define UDC_MAXGRADE         7
#define CELLNUMBERONE		 115
// CPropertiesWndRet

extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);


extern std::vector<positiveID> positiveUDCID;
extern std::vector<positiveID> positiveUSID;

IMPLEMENT_DYNAMIC(CPropertiesWndRet, CDockablePane)
using namespace Common;

CPropertiesWndRet::CPropertiesWndRet()
{
}
CPropertiesWndRet::~CPropertiesWndRet()
{
}


BEGIN_MESSAGE_MAP(CPropertiesWndRet, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CPropertiesWndRet 消息处理程序
static ULONG n_CruID = 0;

int CPropertiesWndRet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_WndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_GRID_PROPERTY2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}
	AdjustLayout();

	//InitPropList();

	return 0;
}

void CPropertiesWndRet::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWndRet::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_WndPropList.SetFocus();
}

/**************************************************
函数名:  AdjustLayout
函数描述:调整显示
**************************************************/
void CPropertiesWndRet::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);
	m_WndPropList.SetWindowPos(NULL,
		rectClient.left,
		rectClient.top,
		rectClient.Width(),
		rectClient.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER);
}

/**************************************************
函数名:  InitPropList
函数描述:初始化属性列表
**************************************************/
void CPropertiesWndRet::InitPropList()
{
	m_WndPropList.InitList();

	LOGFONT lf = { 0 };
	m_WndPropList.GetFont()->GetLogFont(&lf);
	lf.lfHeight = -14;
	//DBG_MSG("m_WndPropList default font size: %s %d, %d\n", WstringToString(lf.lfFaceName).c_str(), lf.lfHeight, lf.lfWidth);
	const auto hFont = ::CreateFontIndirectW(&lf);
	m_WndPropList.SetFont(CFont::FromHandle(hFont));

	/*NONCLIENTMETRICS ncm{ 0 };
	ncm.cbSize = sizeof(ncm);
	::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	ncm.lfMessageFont.lfHeight = -12;
	//wcscpy(ncm.lfMessageFont.lfFaceName, L"Arial");
	const auto hFont = ::CreateFontIndirectW(&ncm.lfMessageFont);
	m_WndPropList.SetFont(CFont::FromHandle(hFont));*/
}


void CPropertiesWndRet::ShowRatioRed(CString &str)
{
	float                     f;
	TASK_INFO                 task_info = {0};
	CMainFrame               *pFrame;

	for( int i = 0 ; i < str.GetLength() ; i ++ )
	{
		if( str.GetAt(i) < '0' || str.GetAt(i) > '9' )
			return;
	}

	f      = _wtof(str);
	pFrame = (CMainFrame*)theApp.GetMainWnd();
	pFrame->m_wndTodayList.GetCurTaskInfo(task_info);

	if( task_info.us_info.us_node[0].us_res != 0.0f )
	{
		str.Format(_T("%.2f"), f / task_info.us_info.us_node[0].us_res);
		m_WndPropList.GetProperty(3)->GetSubItem(1)->SetValue(str);

	}

}

extern BOOL CheckNum(CMFCPropertyGridProperty *pSN, int &nNum,int nDefaultNum);

/*****************************************************
函数名:  OnPropertyChanged
函数描述:属性列表的值改变时此函数被系统调用
*****************************************************/
LRESULT CPropertiesWndRet::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	char                      aBuf[MAX_PATH];
	LONG                      n, v = 0, high_bit;
	BOOL                      bShowXW = FALSE;
	CString                   str;
	TASK_INFO                 task_info = {0};
	TYPE_INFO                 type_info = USER_TYPE_INFO;
	CMainFrame               *pFrame;
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*) lParam;

	n = pProp->GetData();
	pFrame = (CMainFrame*)theApp.GetMainWnd();

	int nCurIndex = pFrame->m_wndTodayList.GetCurSelIndex();

	if( nCurIndex == -1 )
		goto err_loc;

	str = pProp->GetValue();
	//pFrame->m_wndTodayList.GetCurTaskInfo(task_info);

	task_info.main_info.nID = n_CruID;
	KSTATUS r = Access_GetTaskInfo(n_CruID,ALL_TYPE_INFO, &task_info);

	if ( pProp ==  m_WndPropList.GetProperty(0)->GetSubItem(0) )
	{
		int nNum = 0;
		if( CheckNum(pProp, nNum, task_info.main_info.nSN) == FALSE )
			goto err_loc;


		task_info.main_info.nSN = nNum;
		pFrame->m_wndTodayList.m_List.SetItemText(nCurIndex, 3, str);
		pFrame->m_wndPatient.m_WndPropList.ShowData(task_info);
	}
	else if (pProp == m_WndPropList.GetProperty(5)->GetSubItem(0))
	{
		char        temp[MAX_PATH];
		if (str == "")
		{
			task_info.main_info.sColor[0] = 0;
		}
		else
		{
			WideCharToMultiByte(CP_ACP, NULL, (LPCWSTR)str, -1, temp, MAX_PATH, NULL, FALSE);
			strcpy(task_info.main_info.sColor, temp);
		}
		pFrame->m_wndPatient.m_WndPropList.ShowData(task_info);
	}
	else if (pProp == m_WndPropList.GetProperty(5)->GetSubItem(1))
	{
		char        temp[MAX_PATH];
		if (str == "")
		{
			task_info.main_info.sTransparency[0] = 0;
		}
		else
		{
			WideCharToMultiByte(CP_ACP, NULL, (LPCWSTR)str, -1, temp, MAX_PATH, NULL, FALSE);
			strcpy(task_info.main_info.sTransparency, temp);
		}
		pFrame->m_wndPatient.m_WndPropList.ShowData(task_info);
	}
	else
	{
		high_bit = n >> 16;
		if( high_bit == 1 )
			type_info = UDC_TYPE_INFO;
		else if( high_bit == 0 )
			type_info = US_TYPE_INFO;
		else //解决未处理bug
		{
			//		if( (n & 0xffff) == 0 )
			//			ShowRatioRed(str);

			return 0;
		}

		n &= 0xffff;


		if( type_info == US_TYPE_INFO )
		{

			if( str == _T("") )
			{
err_loc:
				pProp->SetValue(pProp->GetOriginalValue());
				return 0;
			}

			for( int i = 0 ; i < str.GetLength() ; i ++ )
			{
				if( i == 0 )
				{
					if( str.GetAt(i) == '-')
						goto err_loc;
					else if( str.GetAt(i)== '.' )
						goto err_loc;
				}

				if( str[i] == '.' )
				{
					v ++;
					if( v > 1)
						goto err_loc;

					continue;
				}
				else if( str.GetAt(i) < '0' || str.GetAt(i) > '9' )
					goto err_loc;

			}

			if( str.GetLength() > 9 )
			{
				str.SetAt(9, 0);
				pProp->SetValue(str);
			}

		}


		WideCharToMultiByte (CP_ACP, NULL, (LPCWSTR)str, -1, aBuf, MAX_PATH, NULL, FALSE);

		if( type_info == US_TYPE_INFO )
		{
			task_info.us_info.us_node[n].us_res = atof(aBuf);
			if (n == (CELL_RED_TYPE - 1) || n == (CELL_DRED_TYPE - 1))
				bShowXW = TRUE;
		}
		else
			strcpy_s(task_info.udc_info.udc_node[n].udc_res, aBuf);

	}

	pProp->SetOriginalValue(str);

	pFrame->m_wndTodayList.UpdateCurTaskInfo(task_info, type_info);
	pFrame->m_wndTodayList.SetSaveCurRecord(TRUE);

	r = Access_UpdateTaskInfo(ALL_TYPE_INFO, task_info.main_info.nID, &task_info);

	//pFrame->m_wndTodayList.AutoSaveCurRecord();
	if( bShowXW == TRUE )
		m_WndPropList.ShowRedXW(task_info);

	//m_WndPropList.SaveData();
	return 0;
}


void CPropertiesWndRet::OnContextMenu( CWnd* /*pWnd*/, CPoint point )
{

}



/********************************************************
函数名:  ShowData
函数描述:显示数据
输入参数:iID-----显示数据库中ID值为iID的记录
********************************************************/
/*void CPropertiesWndRet::ShowData(int iID)
{
	m_WndPropList.ShowData(iID);
}*/

// CPropertyGridCtrlRet

IMPLEMENT_DYNAMIC(CPropertyGridCtrlRet, CMFCPropertyGridCtrl)

CPropertyGridCtrlRet::CPropertyGridCtrlRet()
:m_nMaxData(2)
{
	curTaskID = 0;
}

CPropertyGridCtrlRet::~CPropertyGridCtrlRet()
{
}


extern HFONT GetSelfDefinedFont(CDC* pDC);

int CPropertyGridCtrlRet::OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const
{
	pDC->SelectObject( GetSelfDefinedFont(pDC) );
	return CMFCPropertyGridCtrl::OnDrawProperty(pDC, pProp);
}



BEGIN_MESSAGE_MAP(CPropertyGridCtrlRet, CMFCPropertyGridCtrl)
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


#define XW_TYPE 0XFF

// CPropertyGridCtrlRet 消息处理程序

int CPropertyGridCtrlRet::UdcOrderByYouyuan(int index)
{
	switch (index)
	{
	case 0:	index = 0; break;
	case 1:	index = 1; break;
	case 2:	index = 3; break;
	case 3:	index = 9; break;
	case 4:	index = 7; break;
	case 5:	index = 2; break;
	case 6:	index = 8; break;
	case 7:	index = 4; break;
	case 8:	index = 6; break;
	case 9:	index = 5; break;
	case 10:index = 10; break;
	case 11:index = 12; break;
	case 12:index = 11; break;
	case 13:index = 13; break;
	case 14:index = 14; break;
	case 15:index = 15; break;
	default:
		break;
	}
	return index;
}

/***************************************************
函数名:  InitList
函数描述:初始化属性列表
***************************************************/
void CPropertyGridCtrlRet::InitList()
{
	curTaskID = 0;
	EnableHeaderCtrl(FALSE);
	EnableDescriptionArea(FALSE);
	SetVSDotNetLook(TRUE);
	MarkModifiedProperties(FALSE);
	_variant_t varStrDefault(_bstr_t(_T("*")));
	_variant_t varIntDefault(unsigned int(0));


	// 基本信息
	CMFCPropertyGridProperty *pProGroup1 = new CMFCPropertyGridProperty(GetIdsString(_T("wndret"),_T("inf")).c_str(), 0, FALSE);
//	pProGroup1->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("wndret"),_T("id")).c_str(), varIntDefault, NULL, 0));
	pProGroup1->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("wndret"),_T("sn")).c_str(), varStrDefault, NULL, 0));
	pProGroup1->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("wndret"),_T("date")).c_str(), varStrDefault, NULL, 0));
	pProGroup1->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("wndret"),_T("time")).c_str(), varStrDefault, NULL, 0));
	AddProperty(pProGroup1);

	pProGroup1->GetSubItem(0)->AllowEdit(TRUE);
    pProGroup1->GetSubItem(1)->SetData(0);
	pProGroup1->GetSubItem(1)->AllowEdit(FALSE);
	pProGroup1->GetSubItem(2)->AllowEdit(FALSE);

	// 尿干化结果
	CMFCPropertyGridProperty *pProGroup2 = new CMFCPropertyGridProperty(GetIdsString(_T("wndret"),_T("udc")).c_str(), 0, FALSE);
	CMFCPropertyGridProperty *pSubItem;
	int index = 0;
	CString strTemp=_T("");
    for(int i = 0; i < MAX_UDC_COUNTS; i++)
	{
// 		if( theApp.udc_cfg_info.par[i].bIsDelete == TRUE )//激活、禁用干化项目
// 			continue;

		if (strcmp(theApp.udcOrder, "youyuan") == 0)
		{
			index = UdcOrderByYouyuan(i);
		}
		else
		{
			index = i;
		}

		strTemp = theApp.udc_cfg_info.par[index].sLongName;
		pSubItem = new CMFCPropertyGridProperty(strTemp,varStrDefault,NULL,0);
		pSubItem->SetData(index | (1 << 16));
		pProGroup2->AddSubItem(pSubItem);
	}
	AddProperty(pProGroup2);

	// 尿沉渣结果

	CMFCPropertyGridProperty *pProGroup3 = new CMFCPropertyGridProperty(GetIdsString(_T("wndret"),_T("us")).c_str(), 0, FALSE);
	for(int i = 0; i < MAX_US_COUNTS; i++)
	{
		if( theApp.us_cfg_info.par[i].bIsDelete == TRUE )
			continue;

		strTemp = theApp.us_cfg_info.par[i].sLongName;

		pSubItem = new CMFCPropertyGridProperty(strTemp, varStrDefault, NULL, 0);

		pSubItem->SetData(i);
		pProGroup3->AddSubItem(pSubItem);
		if (i == MAX_US_COUNTS - 1)
		{
			pSubItem->Show(FALSE);
		}
	}

	AddProperty(pProGroup3);

	// 变相红细胞
	CMFCPropertyGridProperty *pProGroup4 = new CMFCPropertyGridProperty(GetIdsString(_T("redcell"),_T("title")).c_str(), 0, FALSE);
//	pProGroup4->ADDSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("redcell"),_T("name")).c_str(), varStrDefault, NULL, 1));
	pProGroup4->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("redcell"),_T("value")).c_str(), varStrDefault, NULL, 1));
	pProGroup4->GetSubItem(0)->AllowEdit(FALSE);
	pProGroup4->GetSubItem(0)->SetData(XW_TYPE << 16);
//	pProGroup4->GetSubItem(1)->SetData((XW_TYPE << 16) | 1);
	AddProperty(pProGroup4);
	//ACR、PCR

	CMFCPropertyGridProperty *pProGroup5 = new CMFCPropertyGridProperty(GetIdsString(_T("UDCyouyuan"), _T("title")).c_str(), 0, FALSE);
	pProGroup5->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("UDCyouyuan"), _T("ACR")).c_str(), varStrDefault, NULL, 0));
	pProGroup5->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("UDCyouyuan"), _T("PCR")).c_str(), varStrDefault, NULL, 0));
	pProGroup5->GetSubItem(0)->AllowEdit(FALSE);

	pProGroup5->GetSubItem(1)->AllowEdit(FALSE);
	//	pProGroup4->GetSubItem(1)->SetData((XW_TYPE << 16) | 1);
	AddProperty(pProGroup5);
	if (strcmp(theApp.udcOrder, "youyuan") != 0)
	{
		pProGroup5->Show(FALSE);
	}

	CMFCPropertyGridProperty *pProGroup6 = new CMFCPropertyGridProperty(_T("性状"), 0, FALSE);
	pProGroup6->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"), _T("IDS_RESULT_COLOR")).c_str(), varStrDefault, NULL, 0));
	pProGroup6->AddSubItem(new CMFCPropertyGridProperty(GetIdsString(_T("pat"), _T("IDS_RESULT_TRANSPARENT")).c_str(), varStrDefault, NULL, 0));
	pProGroup6->GetSubItem(0)->SetData(0xFFF1);
	pProGroup6->GetSubItem(1)->SetData(0xFFF2);
	AddProperty(pProGroup6);

	//// 配置属性列表
	ConfigList();

}


#define MAX_GRID_COUNT 100
static int CurrentMaxNum = 0;
static CMFCPropertyGridProperty* MFCPropertyGridPropertyList[MAX_GRID_COUNT] = {0};

/******************************************************
函数名:  ConfigList
函数描述:配置属性列表
******************************************************/
void CPropertyGridCtrlRet::ConfigList()
{ 
	/*// 基本信息禁止编辑*/
// 	for(int i = 0; i < 3; i++)
// 	{
// 		CMFCPropertyGridProperty* pSubWnd = NULL;
// 		pSubWnd = GetProperty(0)->GetSubItem(i);
// 
// 		if(pSubWnd != NULL)
// 		{
// 			pSubWnd->AllowEdit(FALSE);
// 		}
// 	}

	// 设置DATA
	//int nData = 1;
	for(int i = 0; i <theApp.m_udcInfo.nUdcCounts; i++)
	{


			CMFCPropertyGridProperty* pSubWnd = NULL;
			pSubWnd = GetProperty(1)->GetSubItem(i);
			
			if(pSubWnd != NULL)
			{
				MFCPropertyGridPropertyList[CurrentMaxNum++] = pSubWnd;
				//pSubWnd->SetData(nData++);
			}
	
	}

	for(int i = 0; i < theApp.m_usInfo.nUsCounts; i++)
	{
			CMFCPropertyGridProperty* pSubWnd = NULL;
			pSubWnd = GetProperty(2)->GetSubItem(i);
			
			if(pSubWnd != NULL)
			{
				MFCPropertyGridPropertyList[CurrentMaxNum++] = pSubWnd;
				//pSubWnd->SetData(nData++);
			}
	}

	for(int i = 0 ; i < 1; i++)
	{
		CMFCPropertyGridProperty* pSubWnd = NULL;
		pSubWnd = GetProperty(3)->GetSubItem(i);

		if(pSubWnd != NULL)
		{
			MFCPropertyGridPropertyList[CurrentMaxNum++] = pSubWnd;
			//pSubWnd->SetData(nData++);
		}
	}

	m_nMaxData = CurrentMaxNum - 1;

	GetProperty(5)->GetSubItem(0)->RemoveAllOptions();
	for (int i = 0; i < theApp.colorNum; ++i)
	{
		GetProperty(5)->GetSubItem(0)->AddOption(Common::CharToWChar(theApp.colorInfo[i].name).c_str(), FALSE);
	}
	GetProperty(5)->GetSubItem(1)->RemoveAllOptions();
	for (int i = 0; i < theApp.clearNum; ++i)
	{
		GetProperty(5)->GetSubItem(1)->AddOption(Common::CharToWChar(theApp.clearInfo[i].name).c_str(), FALSE);
	}
	GetProperty(5)->GetSubItem(0)->SetValue(_T(""));
	GetProperty(5)->GetSubItem(1)->SetValue(_T(""));
	MFCPropertyGridPropertyList[CurrentMaxNum++] = GetProperty(5)->GetSubItem(0);
	MFCPropertyGridPropertyList[CurrentMaxNum++] = GetProperty(5)->GetSubItem(1);
}

/*******************************************************
函数名:  ShowData
函数描述:显示数据
输入参数:iID----------显示数据的ID
*******************************************************/



void CPropertyGridCtrlRet::ShowUSData( TASK_INFO &task_info )
{
	float coefficient = 0;
	int     nIndex = 0;
	CString strInfo;
	char Inipath[128] = {0};
	n_CruID = task_info.main_info.nID;
	int tmp = 0;


// 	GetModuleFileNameA(NULL, Inipath, MAX_PATH);
// 	PathRemoveFileSpecA(Inipath);
// 	PathAppendA(Inipath, "config\\Focus.ini");
// 	int row = GetPrivateProfileIntA("Focus", "CaptureImageRow", 8, Inipath);
// 	int column = GetPrivateProfileIntA("Focus", "CaptureImageColumn", 8, Inipath);
// 
// 	coefficient = (float)CELLNUMBERONE / (float)row / (float)column;
	GetModuleFileNameA(NULL, Inipath, MAX_PATH);
	PathRemoveFileSpecA(Inipath);
	PathAppendA(Inipath, "conf.ini");
	coefficient = GetPrivateProfileIntA("calibrate", "value", 1, Inipath);

	for( int i = 0 ; i < MAX_US_COUNTS ; i ++ )
	{
		if( theApp.us_cfg_info.par[i].bIsDelete == TRUE )
			continue;
/*		strInfo.Format(_T("%.f"),round(task_info.us_info.us_node[i].us_res*coefficient));//此系数已经在UpdateUSResult用过了*/
		strInfo = (LPCTSTR)_bstr_t(task_info.us_info.us_node[i].us_res);
		
		GetProperty(2)->GetSubItem(nIndex)->SetValue(strInfo);
		nIndex++;

	}
    if (task_info.main_info.nAttribute == UsQCBy1 || task_info.main_info.nAttribute == UsQCBy2 || task_info.main_info.nAttribute == UsQCBy3)//如果为沉渣质控测试，则质控球项目显示。注意质控球项目始终激活
	{
		GetProperty(2)->GetSubItem(nIndex - 1)->Show(TRUE);
	}

// 	if (task_info.main_info.nAttribute == 1 || task_info.main_info.nAttribute == 2)
// 	{
// 		if (GetPropertyCount() == 4)
// 		{
// 			CMFCPropertyGridProperty *pSubItem;
// 			CString strTemp = _T("");
// 			CMFCPropertyGridProperty *pProGroup5 = new CMFCPropertyGridProperty(L"尿沉渣质控", 0, FALSE);
// 			_variant_t varStrDefault(_bstr_t(_T("*")));
// 
// 			strTemp = "质控球";
// 
// 			pSubItem = new CMFCPropertyGridProperty(strTemp, varStrDefault, NULL, 0);
// 			pSubItem->SetData(0);
// 			pProGroup5->AddSubItem(pSubItem);
// 			AddProperty(pProGroup5);
// 		}
// 		strInfo.Format(_T("%.f"), round(task_info.us_info.us_node[44].us_res*coefficient));
// 		GetProperty(4)->GetSubItem(0)->SetValue(strInfo);
// 	}
// 	else
// 	{
// 		if (GetPropertyCount() == 5)
// 		{
// 			CMFCPropertyGridProperty *pProGroup5 = new CMFCPropertyGridProperty(L"尿沉渣质控", 0, FALSE);
// 			pProGroup5 = GetProperty(4);
// 			DeleteProperty(pProGroup5);
// 			delete pProGroup5;
// 		}
// 	}

	ShowRedXW(task_info);

	// 性状
	GetProperty(5)->GetSubItem(0)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sColor));
	GetProperty(5)->GetSubItem(1)->SetValue((LPCTSTR)_bstr_t(task_info.main_info.sTransparency));

}

void CPropertyGridCtrlRet::ShowUDCData( TASK_INFO &task_info )
{
	int     nIndex = 0;
	CString strInfo;
	PCR_ACR cr;
	ZeroMemory((void*)&cr, sizeof(PCR_ACR));
	int index = 0;
	StatusEnum   nUSStatus, nUDCStatus;
	theApp.ParseStatusMaskCode(task_info.main_info.nState, nUSStatus, nUDCStatus);
	for(int i = 0; i < MAX_UDC_COUNTS; i++)
	{
		if(theApp.udc_cfg_info.par[i].bIsDelete == TRUE )
			continue;


		if (strcmp(theApp.udcOrder, "youyuan") == 0)
		{
			if (nUDCStatus == FINISH || nUDCStatus == CHECKED)
			{
				switch (i)
				{
				case PRO_G:strcpy(cr.pro, GetIntByUDCGrade(i, task_info.udc_info.udc_node[i].udc_res)); break;
				case CRE_G:strcpy(cr.cre, GetIntByUDCGrade(i, task_info.udc_info.udc_node[i].udc_res)); break;
				case MCA_G:strcpy(cr.mca, GetIntByUDCGrade(i, task_info.udc_info.udc_node[i].udc_res)); break;
				default:
					break;
				}
			}
			else
			{
				sprintf_s(cr.cre, "*");
			}
			index = UdcOrderByYouyuan(i);			
		}
		else
		{
			index = i;
		}

		strInfo = Common::CharToWChar(task_info.udc_info.udc_node[index].udc_res).c_str();
		GetProperty(1)->GetSubItem(nIndex)->SetValue(strInfo);
		nIndex++;
	}
	if (strcmp(theApp.udcOrder, "youyuan") == 0)
		ShowAcrPcr(cr);

}

void CPropertyGridCtrlRet::SetDefaultValue(int nTestType)
{
	int        j;
	_variant_t varStrDefault(_bstr_t(_T("*")));
	_variant_t varIntDefault(unsigned int(0));

	if( nTestType == ALL_TEST_TYPE )
	{
		GetProperty(0)->GetSubItem(0)->SetValue(varStrDefault);
		GetProperty(0)->GetSubItem(1)->SetValue(varStrDefault);
		GetProperty(0)->GetSubItem(2)->SetValue(varStrDefault);
	}

	if( nTestType & UDC_TEST_TYPE )
	{
		for( j = 0 ; j < GetProperty(1)->GetSubItemsCount() ; j ++ )
			GetProperty(1)->GetSubItem(j)->SetValue(varStrDefault);
	}

	if( nTestType & US_TEST_TYPE )
	{
		for( j = 0 ; j < GetProperty(2)->GetSubItemsCount() ; j ++ )
			GetProperty(2)->GetSubItem(j)->SetValue(varStrDefault);

		GetProperty(3)->GetSubItem(0)->SetValue(varStrDefault);
	}
}


void CPropertyGridCtrlRet::ShowData(TASK_INFO  & task_info, int nShowType)
{

	curTaskID = task_info.main_info.nID;
	CString strRet, strDate, strTime;
	

	// 基本信息
	//GetProperty(0)->GetSubItem(0)->SetValue(_variant_t(unsigned int(task_info.main_info.nID)));
	strRet = (LPCTSTR)_bstr_t(task_info.main_info.nSN);
	GetProperty(0)->GetSubItem(0)->SetValue( strRet );

	strRet = (LPCTSTR)_bstr_t(task_info.main_info.dtDate);
	strDate = strRet.Left(strRet.Find(' '));
	strTime = strRet.Right(strRet.GetLength() - strRet.Find(' ') - 1);
	GetProperty(0)->GetSubItem(1)->SetValue(strDate);
	GetProperty(0)->GetSubItem(2)->SetValue(strTime);

	//以下关于task_info.main_info.nAttribute的判断有点多余，但有比没有好
	if (task_info.main_info.nTestType == 3)
	{
		GetProperty(1)->Show(TRUE);
		GetProperty(2)->Show(TRUE);
		GetProperty(3)->Show(TRUE);
		if (strcmp(theApp.udcOrder, "youyuan") == 0) GetProperty(4)->Show(TRUE);
		GetProperty(5)->Show(TRUE);
	}
	else if (task_info.main_info.nTestType == 1
		|| task_info.main_info.nAttribute == UdcQCByP
		|| task_info.main_info.nAttribute == UdcQCByN)
	{
		GetProperty(2)->Show(FALSE);
		GetProperty(3)->Show(FALSE);
		GetProperty(5)->Show(FALSE);
		GetProperty(1)->Show(TRUE);
		if (strcmp(theApp.udcOrder, "youyuan") == 0) GetProperty(4)->Show(TRUE);
	}
	else if (task_info.main_info.nTestType == 2
		|| task_info.main_info.nAttribute == UsQCBy1
		|| task_info.main_info.nAttribute == UsQCBy2
        || task_info.main_info.nAttribute == UsQCBy3)
	{
		GetProperty(1)->Show(FALSE);
		if (strcmp(theApp.udcOrder, "youyuan") == 0) GetProperty(4)->Show(FALSE);
		GetProperty(2)->Show(TRUE);
		GetProperty(3)->Show(TRUE);
		GetProperty(5)->Show(TRUE);
	}
	else //task_info.main_info.nTestType == 0,即是质控
	{
		GetProperty(1)->Show(TRUE);
		GetProperty(2)->Show(TRUE);
		GetProperty(3)->Show(TRUE);
		GetProperty(5)->Show(TRUE);
		if (strcmp(theApp.udcOrder, "youyuan") == 0) GetProperty(4)->Show(TRUE);
	}
	
	if (GetProperty(1)->IsVisible())
	{
		for (int j = theApp.m_nUdcItemCount; j < theApp.udc_cfg_info.nUdcCounts; j++)
		{
			GetProperty(1)->GetSubItem(j)->Show(FALSE);
		}
	}
	if( nShowType & SHOW_US_TYPE )
		ShowUSData(task_info);

	if( nShowType & SHOW_UDC_TYPE )
		ShowUDCData(task_info);

	TCHAR INIPath[256] = { 0 };
	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("config\\Lis1.ini"));
	if (2020 == theApp.m_sendLisType && !theApp.m_bIsLiushuixain)
	{
		GetProperty(3)->Show(FALSE);
	}


	//CMainFrame* pMainFrame = (CMainFrame*)theApp.GetMainWnd();

	//int nret ;//= Warming()

	//if(nret == 2)
	//{
	//	CString str;
	//	str.Format(_T("结果报警:开  任务%d检测结果冲突"),iID);

	//	if(pMainFrame)
	//	{
	//		pMainFrame->m_wndStatusBar.SetPaneText(1,str);
	//	}

	//}
	//else
	//{
	//	pMainFrame->m_wndStatusBar.SetPaneText(1,_T("结果报警:未知"));
	//}

	//SetResultToDb(iID,nret);
}

//
//void CPropertyGridCtrlRet::SetResultToDb(int iID,int nRet)
//{
//	TCHAR chSql[MAX_PATH] = _T("");
//	_stprintf_s(chSql, MAX_PATH, _T("SELECT * FROM ((tMain left outer join tUdc on tMain.nID=tUdc.nID)\
//									left outer join tUs on tMain.nID=tUs.nID) where tMain.nID=%d"), iID);
//	Common::Recordset Rs;
//	Rs.Open(chSql, theApp.m_pConn->GetInterfacePtr(), ADODB::adOpenKeyset, ADODB::adLockOptimistic, ADODB::adCmdText);
//
//	if(!Rs.IsBOF())
//	{
//		Rs.MoveFirst();
//
//		Rs.PutCollect(_T("nAlarm"),nRet);
//		Rs.Update();
//	}
//
//	Rs.Close();
//
//	CMainFrame* pMainFrame = (CMainFrame*)theApp.GetMainWnd();
//	pMainFrame->m_wndTodayList.Refurbish();
//}
//
//
//int  CPropertyGridCtrlRet::Warming(void)
//{
//
//	CString sdbg;
//
//	int nret = 0;
//	UW2000_WARMING_INFO warming;
//	warming.m_bIsCheck = 0;
//	warming.m_nCounts = 0;
//
//	theApp.GetConfigWarmingValue(&warming);
//
//	sdbg.Format(_T("CPropertyGridCtrlRet::Warming warming.check:%d,warming.counts:%d\n"),warming.m_bIsCheck,warming.m_nCounts);
//	OutputDebugString(sdbg);
//
//	for (int i = 0 ; i < warming.m_nCounts ; i ++)
//	{
//		sdbg.Format(_T("CPropertyGridCtrlRet::Warming warming.item[%d]:%s\n"),i,warming.m_sWarmingItem[i]);
//		OutputDebugString(sdbg);
//	}
//
//
//	if(warming.m_bIsCheck == 0)
//	{
//		return 0;
//	}
//
//	if(warming.m_nCounts <= 0)
//	{
//		return 0;
//	}
//
//
//	// 获得性别
//	int nSex = 0;
//	try
//	{
//		nSex = (unsigned int)m_Rs.GetCollect(_T("nSex"));
//	}
//	catch (_com_error e)
//	{
//	}
//
//	// 获得US值
//	int nRbc = 4;
//	int nWbc = 5;
//	int nEp = 3;
//
//	if(nSex == 1)
//	{
//		// 男性 就是默认值
//	}
//	else if(nSex == 2)
//	{
//		nRbc = theApp.m_Max_r_w;
//		nWbc = theApp.m_Max_w_w;
//		nEp = theApp.m_Max_ep_w;
//	}
//	else
//	{
//	}
//
//	sdbg.Format(_T("CPropertyGridCtrlRet::Warming nRbc:%d,nWbc:%d,nEp:%d,nSex:%d\n"),nRbc,nWbc,nEp,nSex);
//	OutputDebugString(sdbg);
//
//	for ( int i = 0 ; i < warming.m_nCounts ; i ++)
//	{
//
//		int nIndex = warming.m_sWarmingItem[i].Find(_T(","));
//
//		
//		CString sUs = warming.m_sWarmingItem[i].Left(nIndex);
//		CString sUdc = warming.m_sWarmingItem[i].Right(nIndex-1);
//
//		sdbg.Format(_T("CPropertyGridCtrlRet::Warming item[%d] sUs:%s,sUdc:%s\n"),i,sUs,sUdc);
//		OutputDebugString(sdbg);
//
//
//		int nUs = -1;
//		int nUdc = -1;
//
//
//		for (int j = 0 ; j < UDC_MAXITEM ; j ++)
//		{
//			sdbg.Format(_T("CPropertyGridCtrlRet::Warming sUdc:%s,theApp.m_UdcGrade.m_ctsItemCode[j].m_String:%s\n"),
//				sUdc,theApp.m_UdcGrade.m_ctsItemCode[j].m_String.c_str());
//			OutputDebugString(sdbg);
//
//			if(theApp.m_UdcGrade.m_ctsItemCode[j].m_String.compare(sUdc) == 0)
//			{
//
//				CString strInfo;
//
//				try
//				{
//					strInfo = (LPCTSTR)_bstr_t(m_Rs.GetCollect(theApp.m_strRes[j].GetBuffer()));
//				}
//				catch (_com_error e)
//				{
//				}
//
//				sdbg.Format(_T("CPropertyGridCtrlRet::Warming Udc:%s\n"),
//				strInfo);
//				OutputDebugString(sdbg);
//
//
//				if(strInfo.Compare(_T("-")) == 0)
//				{
//					OutputDebugString(_T("CPropertyGridCtrlRet::Warming CP3\n"));
//					nUdc = 0;
//				}
//				else if(strInfo.Compare(_T("+-")) == 0)
//				{
//					OutputDebugString(_T("CPropertyGridCtrlRet::Warming CP3.1\n"));
//					nUdc = 0;
//				}
//				else if(strInfo.Compare(_T("*")) == 0)
//				{
//					OutputDebugString(_T("CPropertyGridCtrlRet::Warming CP3.2\n"));
//					nUdc = 0;
//				}
//				else
//				{
//					OutputDebugString(_T("CPropertyGridCtrlRet::Warming CP3.3\n"));
//					nUdc = 1;
//				}
//
//				break;
//			}
//			else
//			{
//				OutputDebugString(_T("CPropertyGridCtrlRet::Warming CP5\n"));
//				nUdc = -1;
//			}
//
//
//		  OutputDebugString(_T("CPropertyGridCtrlRet::Warming CP6\n"));
//
//
//		} // end for
//
//		sdbg.Format(_T("CPropertyGridCtrlRet::Warming item[%d] nUdc:%d\n"),i,nUdc);
//		OutputDebugString(sdbg);
//
//
//		if(nUdc == -1)
//		{
//			continue;
//		}
//
//
//		int nData = -1;
//
//		for (int x = 0 ; x < UDC_MAXITEM ; x ++)
//		{
//
//			sdbg.Format(_T("CPropertyGridCtrlRet::Warming sUs:%s,theApp.m_strCellType[x]:%s\n"),
//				sUs,theApp.m_strCellType[x]);
//			OutputDebugString(sdbg);
//
//
//			if(theApp.m_strCellType[x].Compare(sUs) == 0)
//			{
//				CString strInfo;
//
//				try
//				{
//					strInfo = (LPCTSTR)_bstr_t(m_Rs.GetCollect(theApp.m_strCellType[x].GetBuffer()));
//				}
//				catch (_com_error e)
//				{
//				}
//			
//				OutputDebugString(strInfo);
//				nData = _wtoi(strInfo);
//			}
//		}
//
//
//
//		if(nData == -1)
//		{
//			continue;
//		}
//
//		if(sUs.Compare(_T("sRBC"))== 0)
//		{
//			if(nRbc < nData)
//			{
//				nUs = 1;
//			}
//			else
//			{
//				nUs = 0;
//			}
//		}
//
//		if(sUs.Compare(_T("sWBC"))== 0)
//		{
//			if(nWbc < nData)
//			{
//				nUs = 1;
//			}
//			else
//			{
//				nUs = 0;
//			}
//		}
//
//
//		if(sUs.Compare(_T("sEP"))== 0)
//		{
//			if(nEp < nData)
//			{
//				nUs = 1;
//			}
//			else
//			{
//				nUs = 0;
//			}
//		}
//
//
//		sdbg.Format(_T("CPropertyGridCtrlRet::Warming item[%d] nUs:%d\n"),i,nUs);
//		OutputDebugString(sdbg);
//
//		if(nUs != nUdc)
//		{
//			nret = 2;
//			break;
//		}
//		else
//		{
//			nret = 1;
//		}
//	}
//
//
//	return nret;
//}
//


void CPropertyGridCtrlRet::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMFCPropertyGridCtrl::OnKeyUp(nChar, nRepCnt, nFlags);

	//unsigned int Index = 0;


	switch (nChar)
	{
	case VK_TAB:
	case VK_RETURN:
		// Shift 上移
		int Index = 0;
		bool IsFind = false;

		CMFCPropertyGridProperty* CurrentSelect = GetCurSel();
		if(CurrentSelect == NULL)
			break;

		for (; Index < CurrentMaxNum; ++Index)
		{
			if (MFCPropertyGridPropertyList[Index] == CurrentSelect)
			{
				IsFind = true;
				break;
			}
		}

		if (!IsFind)
		{
			break;
		}

		if (GetKeyState(VK_SHIFT) & 0xFF00)
		{
			if (Index == 0)
			{
				Index = CurrentMaxNum - 1;
			}
			else
			{
				Index--;		
			}

			//nData = GetCurSel()->GetData() - 1;
		}
		else
		{
			if (Index == CurrentMaxNum - 1)
			{
				Index = 0;
			}
			else
			{
				Index++;
			}
			//nData = GetCurSel()->GetData() + 1;
		}
		// 范围限制
// 		if (nData <= 0)
// 		{
// 			nData = m_nMaxData;
// 		}
// 		else if (nData > m_nMaxData)
// 		{
// 			nData = 1;
// 		}
		// 设置焦点
		
		//SetCurSel(MFCPropertyGridPropertyList[Index]);
		EditItem(MFCPropertyGridPropertyList[Index]);
		//SetCurSel(FindItemByData(nData));
		break;
	}
}

//限制字符串长度
void CPropertyGridCtrlRet::CheckStringLen(CMFCPropertyGridProperty *pProp, unsigned int nMaxLen)
{
	if(_tcslen((LPCTSTR)(_bstr_t)pProp->GetValue()) > nMaxLen)
	{
		pProp->SetValue(_T(""));
		m_bStringLenLegit = FALSE;
	}
}


void CPropertyGridCtrlRet::ChangeUSCell( PMODIFY_CELL_INFO  pModifyCellInfo, BOOL bAdd )
{
	int         j;
	float       res;
	BOOL        bFind = FALSE, bShowXW = FALSE;
	CString     str;
	CMainFrame *pFrame;
	TASK_INFO   task_info = {0};

	pFrame = (CMainFrame *)theApp.GetMainWnd();

	pFrame->m_wndTodayList.GetCurTaskInfo(task_info);

	if( task_info.main_info.nID != pModifyCellInfo->nID )  //由于delay会造成数据错乱
	{
		int nCellTypeIndex;
		ZeroMemory(&task_info, sizeof(task_info));
		if( Access_GetTaskInfo(pModifyCellInfo->nID, US_TYPE_INFO, &task_info) != STATUS_SUCCESS )
			return;

		if( pModifyCellInfo->nCellType == CELL_OTHER_TYPE )
			return;


		nCellTypeIndex = pModifyCellInfo->nCellType - 1;

		if( bAdd == TRUE )
		{
			task_info.us_info.us_node[nCellTypeIndex].us_res += task_info.us_info.nVariableRatio;
		}
		else
		{
			if( task_info.us_info.us_node[nCellTypeIndex].us_res < task_info.us_info.nVariableRatio )
				task_info.us_info.us_node[nCellTypeIndex].us_res = 0.0f;
			else
				task_info.us_info.us_node[nCellTypeIndex].us_res -= task_info.us_info.nVariableRatio;
		}

		Access_UpdateTaskInfo(US_TYPE_INFO, pModifyCellInfo->nID, &task_info);
		return;
	}

	for( j = 0 ; j < GetProperty(2)->GetSubItemsCount() ; j ++ )
	{
		if( pModifyCellInfo->nCellType == GetProperty(2)->GetSubItem(j)->GetData()+1)
		{
			bFind = TRUE;
			if (pModifyCellInfo->nCellType == CELL_RED_TYPE || pModifyCellInfo->nCellType == CELL_DRED_TYPE)
				bShowXW = TRUE;
			break;
		}
	}

	if( bFind == FALSE )
		return;

	str = GetProperty(2)->GetSubItem(j)->GetValue();
	res = _wtof(str);

	if( bAdd )
		res += task_info.us_info.nVariableRatio;
	else
	{
		if( res <= task_info.us_info.nVariableRatio )
			res = 0.0f;
		else
			res -= task_info.us_info.nVariableRatio;
	}
	str.Format(_T("%.2f"), res);
	GetProperty(2)->GetSubItem(j)->SetValue(str);
	
	pFrame->m_wndTodayList.UpdateUSValueOfCurTaskInfo(pModifyCellInfo->nCellType, res);
	pFrame->m_wndTodayList.SetSaveCurRecord(TRUE);
	if( bShowXW == TRUE )
	{
		TASK_INFO  task_info = {0};

		pFrame->m_wndTodayList.GetCurTaskInfo(task_info);
		ShowRedXW(task_info);
	}
}

void CPropertyGridCtrlRet::ShowRedXW(TASK_INFO &task_info)
{
	_variant_t varStrDefault(_bstr_t(_T("*")));

	//if (task_info.us_info.us_node[CELL_RED_TYPE - 1].us_res != 0.0f && task_info.us_info.us_node[CELL_DRED_TYPE - 1].us_res != 0.0f)
	if (task_info.us_info.us_node[CELL_RED_TYPE - 1].us_res + task_info.us_info.us_node[CELL_DRED_TYPE - 1].us_res != 0.0f)
	{
		CString  str = _T("");

		str.Format(_T("%.2f"), task_info.us_info.us_node[CELL_DRED_TYPE - 1].us_res / (task_info.us_info.us_node[CELL_RED_TYPE - 1].us_res + task_info.us_info.us_node[CELL_DRED_TYPE - 1].us_res));
		GetProperty(3)->GetSubItem(0)->SetValue(str);
		return;
	}

	GetProperty(3)->GetSubItem(0)->SetValue(varStrDefault);

}


void CPropertyGridCtrlRet::ShowAcrPcr(PCR_ACR cr)
{	
	float acr = 0.0f;
	float pcr = 0.0f;
	if (strcmp(cr.cre, "*") == 0)
	{
		char start[] = "*";
		GetProperty(4)->GetSubItem(0)->SetValue((_variant_t)start);
		GetProperty(4)->GetSubItem(1)->SetValue((_variant_t)start);
		return;
	}
	else if (strcmp(cr.cre,"0") != 0)
	{
		acr = (float)(atof(cr.mca) / atof(cr.cre));
		pcr = (float)(atof(cr.pro) / atof(cr.cre));
	}
	char buff[64] = { 0 };
	CString  str = _T("");
	sprintf(buff, "%.2f", acr);
	GetProperty(4)->GetSubItem(0)->SetValue((_variant_t)buff);
	sprintf(buff, "%.2f", pcr);
	GetProperty(4)->GetSubItem(1)->SetValue((_variant_t)buff);
	return;
}

char* CPropertyGridCtrlRet::GetIntByUDCGrade(int type, char* grade)
{
	char tmp[64] = { 0 };
	
	for (int i = 0; i < UDC_MAXGRADE; i++)
	{
		TcharToChar(theApp.m_UdcitemInfo[type].m_ctsGradeName[i], tmp);
		if (strcmp(tmp, grade) == 0)
		{
			TcharToChar(theApp.m_UdcitemInfo[type].m_ctsGradeValue[i], tmp);
			return tmp;
		}
	}
	return "0";
}

void CPropertyGridCtrlRet::TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//将tchar值赋给_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CPropertyGridCtrlRet::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CMFCPropertyGridProperty::ClickArea clickArea;
	CMFCPropertyGridProperty* pHit = HitTest(point, &clickArea);
	if (!pHit)
	{
		return;
	}
	if (GetProperty(4) != pHit->GetParent())
	{
		if (clickArea == CMFCPropertyGridProperty::ClickName)
		{
			if (pHit->GetData() == (XW_TYPE << 16))
			{
				if (!curTaskID)
				{
					return;
				}
				CDRBCImage* m_DRecImage = new CDRBCImage;
				char recImagePath[1024] = { 0 };
				GetModuleFileNameA(NULL, recImagePath, MAX_PATH);
				PathRemoveFileSpecA(recImagePath);

				PathAppendA(recImagePath, "\\RecGrapReslut");

				sprintf(recImagePath, "%s\\%d\\%s", recImagePath, curTaskID, "DRBC.jpg");
				m_DRecImage->Create(IDD_DIALOG_DRBCIMAGE, this);	
				strcpy(m_DRecImage->dRBCImagePath, recImagePath);
/*				m_DRecImage->DrawPictureFromBmpFile(m_DRecImage, IDC_STATIC_SHOWIMAGE, recImagePath);*/
				m_DRecImage->ShowWindow(SW_SHOW);
			}
		}
	}
	if (GetProperty(2) != pHit->GetParent())
	{
		return;
	}
	switch (clickArea)
	{
	case CMFCPropertyGridProperty::ClickExpandBox:
		break;

	case CMFCPropertyGridProperty::ClickName:
	{	
		int cellType = pHit->GetData() + 1;
		CMainFrame* pMain = (CMainFrame*)theApp.m_pMainWnd;
		CUWView* pUWView = (CUWView*)pMain->GetUWView()->GetTabWnd(THUM_VIEW_TAB);
		pMain->ActiveTab(THUM_VIEW_TAB);
		THUIInterface_SetShowGroup(0, cellType);	
	}
	break;

	case CMFCPropertyGridProperty::ClickValue:
		break;
	default:
		break;
	}
	CMFCPropertyGridCtrl::OnLButtonDblClk(nFlags, point);
}
