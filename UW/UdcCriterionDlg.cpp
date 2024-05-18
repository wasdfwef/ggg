// UdcCriterionDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "UdcCriterionDlg.h"


void GetUDCIni(TCHAR *UDCIni)
{
	GetModuleFileName(NULL, UDCIni, MAX_PATH);
	PathRemoveFileSpec(UDCIni);
	PathAppend(UDCIni, _T("config\\Udc.ini"));

}


void InitUdcini()
{
	TCHAR s_inifile[MAX_PATH];
	GetUDCIni(s_inifile);
	
	std::string str;
	TCHAR szLine[MAX_PATH*8] = {0};
	
	if( 0 !=GetPrivateProfileString(_T("udc"),
		_T("item"),
		_T("0"),
		szLine,
		MAX_PATH*8,
		s_inifile))
	{
		theApp.m_nUdcItem = _ttoi(szLine);
	}
	else
	{
		theApp.m_nUdcItem = 0;
		//AfxMessageBox(_T("udc load fail!"));
		str = "0";
	}

    int   iIndex = 0;
	int   nItemIndex=0;
	int   nGradeEnable=0;
	TCHAR chIndex[INDEX_BUF_LEN] = {0};
	TCHAR ItemCode[INDEX_BUF_LEN]={0};
	TCHAR GradeName[INDEX_BUF_LEN]={0};
	int   nGradeThreahold=0;
	TCHAR GradeValue[INDEX_BUF_LEN]={0};
	TCHAR GradeUnit[INDEX_BUF_LEN]={0};
    
	TCHAR chRetBuf[MAX_PATH] = {0};
	
	CString code,name;
	

   for (int i=0; i<theApp.m_nUdcItem;++i)
   {
	 code.Format(_T("code%d"),i);
	 name.Format(_T("name%d"),i);
     DWORD dwRet = GetPrivateProfileString(_T("udc"), code, NULL, chRetBuf, MAX_PATH, s_inifile);
	 if (dwRet > 0)
	 {
	   _tcscpy_s(theApp.m_UdcitemInfo[i].m_ctsItemCode,chRetBuf);
	 }
     
	 dwRet = GetPrivateProfileString(_T("udc"), name, NULL, chRetBuf, MAX_PATH, s_inifile);
	 if (dwRet > 0)
	 {
		 _tcscpy_s(theApp.m_UdcitemInfo[i].m_ctsItemName,chRetBuf);
	 }

		for (int j=0;j < UDC_MAXGRADE; ++j)
		{
			dwRet = 0;
			_stprintf_s(chIndex, INDEX_BUF_LEN, _T("%d"), iIndex);
			dwRet = GetPrivateProfileString(_T("udc"), chIndex, NULL, chRetBuf, MAX_PATH, s_inifile);
			if (dwRet > 0)
			{
				_stscanf(chRetBuf, _T("%d,%[^,],%d,%[^,],%[^,]"), &nGradeEnable, GradeName,&nGradeThreahold,GradeUnit,GradeValue);
				theApp.m_UdcitemInfo[i].m_bGradeEnable[j]=nGradeEnable;
				if (nGradeEnable)
				{
					_tcscpy_s(theApp.m_UdcitemInfo[i].m_ctsGradeName[j],GradeName);
					theApp.m_UdcitemInfo[i].m_nGradeThreshold[j] = nGradeThreahold;
					_tcscpy_s(theApp.m_UdcitemInfo[i].m_ctsGradeUnit[j],GradeUnit);
					_tcscpy_s(theApp.m_UdcitemInfo[i].m_ctsGradeValue[j],GradeValue);
				}
				
			
			}
			/*else
			{
				break;
			}*/
			iIndex++;
		/*	if (iIndex % 8 == 0)
			{
				nItemIndex++;
			}*/
		}
   }
   //theApp.m_nNums = iIndex;

}

extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);
// CUdcCriterionDlg 对话框


 

IMPLEMENT_DYNAMIC(CUdcCriterionDlg, CDialog)

CUdcCriterionDlg::CUdcCriterionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUdcCriterionDlg::IDD, pParent)
	
{
   memset(s_inifile,0,MAX_PATH);
}

CUdcCriterionDlg::~CUdcCriterionDlg()
{
}

void CUdcCriterionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UDC_LIST, m_udcList);
	DDX_Control(pDX, IDC_ITEM_LIST, m_itemList);
	DDX_Control(pDX, IDC_UDC_ITEM, m_ctsItem);
	DDX_Control(pDX, IDC_UDC_NAME, m_ctsName);
	DDX_Control(pDX, IDC_UDC_GRADE, m_ctsGrade);
	DDX_Control(pDX, IDC_UDC_REFLECT, m_csReflect);
	DDX_Control(pDX, IDC_UDC_VALUE, m_csValue);
	DDX_Control(pDX, IDC_UDC_UNIT, m_csUnit);
	DDX_Text(pDX, IDC_EDIT_UDC_ITEM, m_ceItem);
	DDX_Text(pDX, IDC_EDIT_UDC_NAME, m_ceName);
	DDX_Text(pDX, IDC_EDIT_UDC_GRADE, m_ceGrade);
	DDX_Text(pDX, IDC_EDIT_UDC_REFLECT, m_ceReflect);
	DDX_Text(pDX, IDC_EDIT_UDC_VALUE, m_ceValue);
	DDX_Text(pDX, IDC_EDIT_UDC_UNIT, m_ceUnit);
}


BEGIN_MESSAGE_MAP(CUdcCriterionDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_UDC_LIST, &CUdcCriterionDlg::OnNMClickUdcList)
//	ON_NOTIFY(HDN_ITEMCHANGED, 0, &CUdcCriterionDlg::OnHdnItemchangedUdcList)
//ON_NOTIFY(HDN_ITEMCHANGING, 0, &CUdcCriterionDlg::OnHdnItemchangingUdcList)
//ON_NOTIFY(HDN_ITEMCHANGED, 0, &CUdcCriterionDlg::OnHdnItemchangedUdcList)
//ON_NOTIFY(HDN_ITEMCLICK, 0, &CUdcCriterionDlg::OnHdnItemclickUdcList)
//ON_NOTIFY(LVN_ITEMCHANGED, IDC_UDC_LIST, &CUdcCriterionDlg::OnLvnItemchangedUdcList)
//ON_NOTIFY(LVN_ITEMCHANGING, IDC_UDC_LIST, &CUdcCriterionDlg::OnLvnItemchangingUdcList)
ON_BN_CLICKED(IDC_BTN_UDC_USE, &CUdcCriterionDlg::OnBnClickedBtnUse)
ON_NOTIFY(NM_CLICK, IDC_ITEM_LIST, &CUdcCriterionDlg::OnNMClickItemList)
ON_BN_CLICKED(IDC_BTN_UDC_SAVE, &CUdcCriterionDlg::OnBnClickedBtnUdcSave)
ON_BN_CLICKED(IDC_BTN_UDC_NEW, &CUdcCriterionDlg::OnBnClickedBtnUdcNew)
ON_BN_CLICKED(IDC_BTN_UDC_DEL, &CUdcCriterionDlg::OnBnClickedBtnUdcDel)
END_MESSAGE_MAP()


// CUdcCriterionDlg 消息处理程序

BOOL CUdcCriterionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	SetWindowText(GetIdsString(_T("udcset"),_T("title2")).c_str());

	// TODO:  在此添加额外的初始化
	m_ctsItem.SetWindowText(GetIdsString(_T("udcset"),_T("title")).c_str());
	m_ctsName.SetWindowText(GetIdsString(_T("udcset"),_T("name")).c_str());
	m_ctsGrade.SetWindowText(GetIdsString(_T("udcset"),_T("grade")).c_str());
	m_csReflect.SetWindowText(GetIdsString(_T("udcset"),_T("reflect")).c_str());
	m_csValue.SetWindowText(GetIdsString(_T("udcset"),_T("value")).c_str());
	m_csUnit.SetWindowText(GetIdsString(_T("udcset"),_T("unit")).c_str());

	GetUDCIni(s_inifile);
	m_nItemIndex = -1;
	m_nGradeIndex = -1;

	InitUdcList();
//	InitUdcini();
	UpdateItemList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CUdcCriterionDlg::InitUdcList()
{
	//m_udcList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER|LVS_NOSCROLL);
	m_udcList.SetExtendedStyle(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER|LVS_EX_FULLROWSELECT);
	//AfxMessageBox(GetIdsString(_T("udcset"),_T(,"title")).c_str());
	m_udcList.InsertColumn(0,GetIdsString(_T("udcset"),_T("title")).c_str(),0,50);
	m_udcList.InsertColumn(1,GetIdsString(_T("udcset"),_T("1")).c_str(),0,50);
	m_udcList.InsertColumn(2,GetIdsString(_T("udcset"),_T("2")).c_str(),0,50);
	m_udcList.InsertColumn(3,GetIdsString(_T("udcset"),_T("3")).c_str(),0,50);
	m_udcList.InsertColumn(4,GetIdsString(_T("udcset"),_T("4")).c_str(),0,50);
	m_udcList.InsertColumn(5,GetIdsString(_T("udcset"),_T("5")).c_str(),0,50);
	m_udcList.InsertColumn(6,GetIdsString(_T("udcset"),_T("6")).c_str(),0,50);
	m_udcList.InsertColumn(7,GetIdsString(_T("udcset"),_T("7")).c_str(),0,50);
	//m_udcList.ModifyStyle(0,LVS_SHOWSELALWAYS); 

	m_itemList.SetExtendedStyle(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER|LVS_EX_FULLROWSELECT);

	m_itemList.InsertColumn(0,GetIdsString(_T("udcset"),_T("grade")).c_str(),0,50);
	m_itemList.InsertColumn(1,GetIdsString(_T("udcset"),_T("reflect")).c_str(),0,80);
	m_itemList.InsertColumn(2,GetIdsString(_T("udcset"),_T("value")).c_str(),0,50);
	m_itemList.InsertColumn(3,GetIdsString(_T("udcset"),_T("unit")).c_str(),0,50);
	//m_itemList.ModifyStyle(0,LVS_SHOWSELALWAYS); 





}



void CUdcCriterionDlg::InsertUdcItem()
{

}

void CUdcCriterionDlg::UpdateItemList( void )
{
		// 项目列表数据刷新
	CString strColor;
	m_udcList.SetRedraw(FALSE);
	m_udcList.DeleteAllItems();
	for(int i = 0; i < theApp.m_nUdcItem; i++)
	{
		m_udcList.InsertItem(i,_T(""));
		m_udcList.SetItemText(i, 0, (LPCTSTR)theApp.m_UdcitemInfo[i].m_ctsItemCode);
		for(int j = 0; j < UDC_MAXGRADE; j++)
		{
			strColor.Format(_T("%d"), theApp.m_UdcitemInfo[i].m_nGradeThreshold[j]);
			if(lstrlen(theApp.m_UdcitemInfo[i].m_ctsGradeName[j]) == 0)
				strColor = _T("");
			m_udcList.SetItemText(i, j + 1, strColor);
		}
	}
	m_udcList.SetRedraw(TRUE);
}



void CUdcCriterionDlg::OnNMClickUdcList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	//POSITION pos = m_udcList.GetFirstSelectedItemPosition();
	//m_nItemIndex =  m_udcList.GetNextSelectedItem(pos); 

	if(-1 != pNMItemActivate->iItem)
	{
		m_nItemIndex = pNMItemActivate->iItem;
		m_nGradeIndex = -1;
	}
	else
	{
		// 无选
		m_nItemIndex = -1;
		m_nGradeIndex = -1;
	}

	// 刷新界面
	UpdateGradeList(m_nItemIndex);
	if(-1 == m_nItemIndex)
	{
		m_strItem = _T("");
		//m_strName = _T("");
		//m_bItemEnable = FALSE;
	}
	else
	{
		m_ceItem = (CString)theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemCode;
		m_ceName = (CString)theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemName;
		//m_strName = theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemName[m_nItemIndex].m_String.data();
		//m_bItemEnable = theApp.m_UdcitemInfo[m_nItemIndex].m_bItemEnable[m_nItemIndex];
	}
	if(-1 == m_nGradeIndex)
	{
		m_ceGrade=_T("");
		//m_nColor = 0;
		//m_ceItem=_T("");
		m_ceValue=_T("");
		m_ceUnit= _T("");
		m_ceReflect= _T("");
		m_ctsGrade.SetCheck(0);
		//m_bGradeEnable = FALSE;
	}
	UpdateData(FALSE);

	*pResult = 0;
}

void CUdcCriterionDlg::UpdateGradeList( int nIndex )
{
	// 项目列表无选择
	if(-1 == nIndex)
	{
		for(int i = 0; i < UDC_MAXGRADE; i++)
		{
			m_itemList.SetItemText(i, 0, _T(""));
			m_itemList.SetItemText(i, 1, _T(""));
			m_itemList.SetItemText(i, 2, _T(""));
			m_itemList.SetItemText(i, 3, _T(""));
		}
		return;
	}

	// 项目列表选择索引为nIndex
	CString strColor;
	m_itemList.SetRedraw(FALSE);
	m_itemList.DeleteAllItems();
	for(int i = 0; i < UDC_MAXGRADE; i++)
	{
		m_itemList.InsertItem(i,_T(""));
		m_itemList.SetItemText(i, 0, (LPCTSTR)theApp.m_UdcitemInfo[nIndex].m_ctsGradeName[i]);
		if(lstrlen(theApp.m_UdcitemInfo[nIndex].m_ctsGradeName[i])==0)
		{
			m_itemList.SetItemText(i, 1, _T(""));
			m_itemList.SetItemText(i, 2, _T(""));
			m_itemList.SetItemText(i, 3, _T(""));
		}
		else
		{
			strColor.Format(_T("%d"), theApp.m_UdcitemInfo[nIndex].m_nGradeThreshold[i]);
			m_itemList.SetItemText(i, 1, strColor);
			m_itemList.SetItemText(i, 2, (LPCTSTR)theApp.m_UdcitemInfo[nIndex].m_ctsGradeValue[i]);
			m_itemList.SetItemText(i, 3, (LPCTSTR)theApp.m_UdcitemInfo[nIndex].m_ctsGradeUnit[i]);
		}
	}
	m_itemList.SetRedraw(TRUE);
}


//void CUdcCriterionDlg::OnLvnItemchangingUdcList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	if (pNMLV->uChanged & LVIF_STATE)
//	{
//		if (pNMLV->uNewState & LVIS_SELECTED)
//		{
//			m_nItemIndex = pNMLV->iItem;
//			// 刷新界面
//			UpdateGradeList(m_nItemIndex);
//			if(-1 == m_nItemIndex)
//			{
//				m_strItem = _T("");
//				//m_strName = _T("");
//				//m_bItemEnable = FALSE;
//			}
//			else
//			{
//				m_strItem = (CString)theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemCode;
//				//m_strName = theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemName[m_nItemIndex].m_String.data();
//				//m_bItemEnable = theApp.m_UdcitemInfo[m_nItemIndex].m_bItemEnable[m_nItemIndex];
//			}
//			if(-1 == m_nGradeIndex)
//			{
//				//m_strGrade = _T("");
//				//m_nColor = 0;
//				//m_strValue = _T("");
//				//m_strUnit = _T("");
//				//m_bGradeEnable = FALSE;
//			}
//			UpdateData(FALSE);
//		}
//	}
//	*pResult = 0;
//}

void CUdcCriterionDlg::OnBnClickedBtnUse()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	
		
	// 消除空格
	/*m_strItem.Trim();
	m_strName.Trim();
	m_strGrade.Trim();
	m_strValue.Trim();
	m_strUnit.Trim();*/

	// 更新数据
	if(-1 != m_nItemIndex)
	{
		_tcscpy_s(theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemName,m_ceName);
		_tcscpy_s(theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemCode, m_ceItem);
		 //=.GetBuffer();
		//theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemName = m_ceName;
		//theApp.m_UdcitemInfo[m_nItemIndex].m_bItemEnable[m_nItemIndex] = m_bItemEnable;
	}
	if((-1 != m_nItemIndex) && (-1 != m_nGradeIndex))
	{
		theApp.m_UdcitemInfo[m_nItemIndex].m_bGradeEnable[m_nGradeIndex] = m_ctsGrade.GetCheck();
		_tcscpy_s(theApp.m_UdcitemInfo[m_nItemIndex].m_ctsGradeName[m_nGradeIndex],m_ceGrade);
		theApp.m_UdcitemInfo[m_nItemIndex].m_nGradeThreshold[m_nGradeIndex] =_ttoi(m_ceReflect);
		_tcscpy_s(theApp.m_UdcitemInfo[m_nItemIndex].m_ctsGradeValue[m_nGradeIndex] , m_ceValue);
		_tcscpy_s(theApp.m_UdcitemInfo[m_nItemIndex].m_ctsGradeUnit[m_nGradeIndex] , m_ceUnit);
		//theApp.m_UdcitemInfo[m_nItemIndex].m_bGradeEnable[m_nItemIndex][m_nGradeIndex] = m_bGradeEnable;
	}

	SaveIni();
	// 刷新界面
	UpdateItemList();
	UpdateGradeList(m_nItemIndex);

	// 更新今天的结果
	//::PostMessage(HWND_BROADCAST, WM_UDC_CHANGE_APPLY, 0, 0);
}

void CUdcCriterionDlg::OnNMClickItemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if(-1 != pNMItemActivate->iItem)
	{
		m_nGradeIndex = pNMItemActivate->iItem;
	}
	else
	{
		// 无选
		m_nGradeIndex = -1;
	}

     CString rvalue;
	// 刷新界面
	if(-1 == m_nGradeIndex)
	{
		
		m_ceGrade=_T("");
		//m_nColor = 0;
		m_ceItem=_T("");
		m_ceValue=_T("");
		m_ceUnit= _T("");
		m_ceReflect= _T("");
		//m_bGradeEnable = FALSE;
	}
	else
	{
		if(-1 != m_nItemIndex)
		{
			rvalue.Format(_T("%d"),theApp.m_UdcitemInfo[m_nItemIndex].m_nGradeThreshold[m_nGradeIndex]);
			m_ceItem=theApp.m_UdcitemInfo[m_nItemIndex].m_ctsItemCode;
			m_ceGrade=(theApp.m_UdcitemInfo[m_nItemIndex].m_ctsGradeName[m_nGradeIndex]);
			m_ceReflect=(rvalue);
			m_ceUnit=(theApp.m_UdcitemInfo[m_nItemIndex].m_ctsGradeUnit[m_nGradeIndex]);
			m_ceValue=(theApp.m_UdcitemInfo[m_nItemIndex].m_ctsGradeValue[m_nGradeIndex]);
			m_ctsGrade.SetCheck(theApp.m_UdcitemInfo[m_nItemIndex].m_bGradeEnable[m_nGradeIndex]);
			
			//m_bGradeEnable = theApp.m_UdcGrade.m_bGradeEnable[m_nItemIndex][m_nGradeIndex];
		}
	}
	UpdateData(FALSE);
	*pResult = 0;
}

void CUdcCriterionDlg::OnBnClickedBtnUdcSave()
{
	// TODO: 在此添加控件通知处理程序代码

	SaveIni();
}

void CUdcCriterionDlg::OnBnClickedBtnUdcNew()
{
	// TODO: 在此添加控件通知处理程序代码
	theApp.m_nUdcItem++;
	if (theApp.m_nUdcItem > 16)
	{
		theApp.m_nUdcItem = 16;
		AfxMessageBox(GetIdsString(_T("udcset"),_T("warning1")).c_str());
		return;
	}
	m_udcList.InsertItem(theApp.m_nUdcItem - 1,_T(""));
	m_udcList.SetItemText(theApp.m_nUdcItem - 1, 0, _T("New"));
    m_udcList.SetItemState(theApp.m_nUdcItem - 1,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_udcList.EnsureVisible(1, false); 
	m_udcList.SetFocus();
	m_nItemIndex = theApp.m_nUdcItem - 1;
	m_nGradeIndex = 0;
    UpdateGradeList(theApp.m_nUdcItem - 1);
	wcscpy(theApp.m_UdcitemInfo[theApp.m_nUdcItem - 1].m_ctsItemCode,_T("NEW"));
	//theApp.m_UdcitemInfo[theApp.m_nUdcItem - 1].m_ctsGradeName=_T("NEW");
	SaveIni();
}


//删除项目
void CUdcCriterionDlg::OnBnClickedBtnUdcDel()
{
	// TODO: 在此添加控件通知处理程序代码
	int iRet;

	iRet=MessageBox(GetIdsString(_T("udcset"),_T("message")).c_str(),GetIdsString(_T("udcset"),_T("title1")).c_str(),MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON1|MB_SYSTEMMODAL);
	if (iRet==IDYES)
	{
		if (theApp.m_nUdcItem < 2)
		{
			AfxMessageBox(GetIdsString(_T("udcset"),_T("warning2")).c_str());
			return;
		}
		for (int i= 1;i<=7 ;++i)
		{
			CString strNum;
			strNum.Format(_T("%d"),(theApp.m_nUdcItem*7-i));
			WritePrivateProfileString(_T("udc"),strNum,NULL,s_inifile);
		}
		--theApp.m_nUdcItem;
		m_udcList.DeleteItem(theApp.m_nUdcItem);
		memset(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsItemCode,0,sizeof(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsItemCode));
		memset(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsItemName,0,sizeof(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsItemName));
		memset(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_bGradeEnable,0,sizeof(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_bGradeEnable));

		memset(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsGradeName,0,sizeof(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsGradeName));
		memset(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsGradeUnit,0,sizeof(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsGradeUnit));
		memset(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsGradeValue,0,sizeof(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_ctsGradeValue));
		memset(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_nGradeThreshold,0,sizeof(theApp.m_UdcitemInfo[theApp.m_nUdcItem].m_nGradeThreshold));
		//UpdateItemList();
		CString code,name;
		code.Format(_T("code%d"),theApp.m_nUdcItem);
		name.Format(_T("name%d"),theApp.m_nUdcItem);
		WritePrivateProfileString(_T("udc"),code,NULL,s_inifile);
		WritePrivateProfileString(_T("udc"),name,NULL,s_inifile);
		CString strinfo;
		strinfo.Format(_T("%d"),theApp.m_nUdcItem);
		WritePrivateProfileString(_T("udc"),_T("item"),strinfo,s_inifile);
		
	}
	
}

//保存ini
void CUdcCriterionDlg::SaveIni()
{
	CString strinfo;
	strinfo.Format(_T("%d"),theApp.m_nUdcItem);
	WritePrivateProfileString(_T("udc"),_T("item"),strinfo,s_inifile);
	CString code,name;
	TCHAR strValue[MAX_PATH];
	int num = 0;
	CString strnum;
	for (int i = 0;i < theApp.m_nUdcItem;++i)
	{
		code.Format(_T("code%d"),i);
		name.Format(_T("name%d"),i);
		WritePrivateProfileString(_T("udc"),code,theApp.m_UdcitemInfo[i].m_ctsItemCode,s_inifile);
		WritePrivateProfileString(_T("udc"),name,theApp.m_UdcitemInfo[i].m_ctsItemName,s_inifile);
		for (int j = 0;j < UDC_MAXGRADE;++j)
		{
			::_stprintf_s(strValue, MAX_PATH, TEXT("%d,%s,%d,%s,%s"), 
				theApp.m_UdcitemInfo[i].m_bGradeEnable[j],
				theApp.m_UdcitemInfo[i].m_ctsGradeName[j],
				theApp.m_UdcitemInfo[i].m_nGradeThreshold[j]>0 ? theApp.m_UdcitemInfo[i].m_nGradeThreshold[j]:0,
				theApp.m_UdcitemInfo[i].m_ctsGradeUnit[j],
				theApp.m_UdcitemInfo[i].m_ctsGradeValue[j]);
			strnum.Format(_T("%d"),num);
			WritePrivateProfileString(_T("udc"),strnum,strValue,s_inifile);
			num++;
		}
	}
	 //theApp.m_nNums = num;
}
