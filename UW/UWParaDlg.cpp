// UWParaDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "UWParaDlg.h"
#include "TaskMgr.h"


// CUWParaDlg 对话框

IMPLEMENT_DYNAMIC(CUWParaDlg, CPropertyPage)
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);


CUWParaDlg::CUWParaDlg()
	: CPropertyPage(CUWParaDlg::IDD)
{
   memset(m_nNum,0,sizeof(m_nNum));
   memset(s_inifile,0,sizeof(s_inifile));
}

CUWParaDlg::~CUWParaDlg()
{
}

void CUWParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	for (size_t i = 0; i < 44;++i)
	{
		DDX_Control(pDX, 35138+i, m_Value[i]);	
	}

	for (size_t i = 0; i < 44;++i)
	{
		DDX_Control(pDX, 35220+i, m_View[i]);	
	}


}


BEGIN_MESSAGE_MAP(CUWParaDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_BTN_GET_INITDATA, &CUWParaDlg::OnBnClickedBtnGetInitdata)
	ON_BN_CLICKED(IDC_BTN_SET_INITDATA, &CUWParaDlg::OnBnClickedBtnSetInitdata)
	ON_BN_CLICKED(IDC_BTN_GET_INI_PARA, &CUWParaDlg::OnBnClickedBtnGetIniPara)
	ON_BN_CLICKED(IDC_BTN_SET_INI_PARA, &CUWParaDlg::OnBnClickedBtnSetIniPara)
END_MESSAGE_MAP()


// CUWParaDlg 消息处理程序

BOOL CUWParaDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetWindowText(GetIdsString(_T("paparms"),_T("title")).c_str());

	// TODO:  在此添加额外的初始化
	TCHAR sModuleFile[MAX_PATH]={0};
	TCHAR sDrive[MAX_PATH]={0};
	TCHAR sDir[MAX_PATH]={0};

	GetModuleFileName(NULL, sModuleFile, MAX_PATH);

	_wsplitpath(sModuleFile, sDrive, sDir, NULL,NULL);

	//TCHAR s_inifile[MAX_PATH]={0};

	wsprintf(s_inifile,_T("%s%s%s.ini"),sDrive,sDir,_T("Conf"));
	//wmemcpy(lpszIniFileName.GetBuffer(),s_inifile,sizeof(s_inifile));
    //lpszIniFileName = s_inifile;

	

	if (!GetLCInfo(Lcinfo))
	{
		AfxMessageBox(GetIdsString(_T("paparms"),_T("err0")).c_str());
		
	}
	else
	{
	    GetData();
	}
    

	/*m_nNum[0]=Lcinfo.Version;
	m_nNum[1]=Lcinfo.ABDistance;
	m_nNum[2]=Lcinfo
	m_nNum[3]=Lcinfo
	m_nNum[4]=Lcinfo
	m_nNum[5]=Lcinfo
	m_nNum[6]=Lcinfo
	m_nNum[7]=Lcinfo
	m_nNum[8]=Lcinfo
	m_nNum[9]=Lcinfo*/


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//获取下位机参数
void CUWParaDlg::OnBnClickedBtnGetInitdata()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!GetLCInfo(Lcinfo))
	{
		AfxMessageBox(GetIdsString(_T("paparms"),_T("err0")).c_str());
		return;
	}
	GetData();

}


//显示参数
void CUWParaDlg::GetData()
{
	
	
	m_nNum[0]=Lcinfo.Version;
	m_nNum[1]=Lcinfo.UpDownHeight;
	m_nNum[2]=Lcinfo.RollerSteps;
	m_nNum[3]=Lcinfo.RasterStep;
	m_nNum[4]=Lcinfo.TurnSteps;
	m_nNum[5]=Lcinfo.CleanSteps;
	m_nNum[6]=Lcinfo.QCSteps;
	m_nNum[7]=Lcinfo.SuckSteps;
	m_nNum[8]=Lcinfo.DropSpaceBetween;
	m_nNum[9]=Lcinfo.ABDistance;
	m_nNum[10]=Lcinfo.X40PerPicDistance;
	m_nNum[11]=Lcinfo.Y40PerPicDistance;
	m_nNum[12]=Lcinfo.Z40AFocusSteps;
	m_nNum[13]=Lcinfo.Z10AFocusSteps;
	m_nNum[14]=Lcinfo.Z40BFocusSteps;
	m_nNum[15]=Lcinfo.Z10BFocusSteps;
	m_nNum[16]=Lcinfo.X40AdjustSteps;
	m_nNum[17]=Lcinfo.Y40AdjustSteps;
	m_nNum[18]=Lcinfo.XBackSteps;
	m_nNum[19]=Lcinfo.YBackSteps;
	m_nNum[20]=Lcinfo.CollectDly;
	m_nNum[21]=Lcinfo.SuckQuantity;
	m_nNum[22]=Lcinfo.TestStyle;
	m_nNum[23]=Lcinfo.ChannalAllow;
	m_nNum[24]=Lcinfo.UseLiquidSensor;
	m_nNum[25]=Lcinfo.MixCounts;
	m_nNum[26]=Lcinfo.ItemCount;
	m_nNum[27]=Lcinfo.DropDelay;
	m_nNum[28]=Lcinfo.SedimentTime;
	m_nNum[29]=Lcinfo.SupCleanSuckTime;
    
	for (int i = 0; i < 14; ++i)
	{
		m_nNum[30+i]=Lcinfo.DropQuantity[i];
	}

	for (int i =0 ; i < 44; ++i)
	{
		CString strvalue;
		strvalue.Format(_T("%d"),m_nNum[i]);
		m_Value[i].SetWindowText(strvalue);
	}

	
}

// 设置系统参数
void CUWParaDlg::OnBnClickedBtnSetInitdata()
{
	// TODO: 在此添加控件通知处理程序代码

	SetData();


	Lcinfo.Version=m_nNum[0];
	Lcinfo.UpDownHeight=m_nNum[1];
	Lcinfo.RollerSteps=m_nNum[2];
	Lcinfo.RasterStep=m_nNum[3];
	Lcinfo.TurnSteps=m_nNum[4];
	Lcinfo.CleanSteps=m_nNum[5];
	Lcinfo.QCSteps=m_nNum[6];
	Lcinfo.SuckSteps=m_nNum[7];
	Lcinfo.DropSpaceBetween=m_nNum[8];
	Lcinfo.ABDistance=m_nNum[9];
	Lcinfo.X40PerPicDistance=m_nNum[10];
	Lcinfo.Y40PerPicDistance=m_nNum[11];
	Lcinfo.Z40AFocusSteps=m_nNum[12];
	Lcinfo.Z10AFocusSteps=m_nNum[13];
	Lcinfo.Z40BFocusSteps=m_nNum[14];
	Lcinfo.Z10BFocusSteps=m_nNum[15];
	Lcinfo.X40AdjustSteps=m_nNum[16];
	Lcinfo.Y40AdjustSteps=m_nNum[17];
	Lcinfo.XBackSteps=m_nNum[18];
	Lcinfo.YBackSteps=m_nNum[19];
	Lcinfo.CollectDly=m_nNum[20];
	Lcinfo.SuckQuantity=m_nNum[21];
	Lcinfo.TestStyle=m_nNum[22];
	Lcinfo.ChannalAllow=m_nNum[23];
	Lcinfo.UseLiquidSensor=m_nNum[24];
	Lcinfo.MixCounts=m_nNum[25];
	Lcinfo.ItemCount=m_nNum[26];
	Lcinfo.DropDelay=m_nNum[27];
	Lcinfo.SedimentTime=m_nNum[28];
	Lcinfo.SupCleanSuckTime=m_nNum[29];
	for (int i = 0 ; i< 14; ++i)
	{
		Lcinfo.DropQuantity[i]=m_nNum[30+i];
	}
	


	if (SetLCInfo(Lcinfo))
	{
		theApp.UpdateCheckType();
		AfxMessageBox(GetIdsString(_T("paparms"),_T("sucess")).c_str());

	}
	else
	{
		AfxMessageBox(GetIdsString(_T("paparms"),_T("err2")).c_str());
	}
	

}

//加载默认参数
void CUWParaDlg::OnBnClickedBtnGetIniPara()
{
	// TODO: 在此添加控件通知处理程序代码

	CString szKeyName=_T("");
	for (int i = 0; i < nParams; i++)
	{
		szKeyName.Format( _T("%d"), i + 1);
		m_nNum[i] = GetPrivateProfileInt(_T("Paprams"), szKeyName, 0,s_inifile);
	}
	for (int i =0 ; i < 44; ++i)
	{
		CString strvalue;
		strvalue.Format(_T("%d"),m_nNum[i]);
		m_Value[i].SetWindowText(strvalue);
	}
	AfxMessageBox(GetIdsString(_T("paparms"),_T("sucess1")).c_str());
}

//设置默认参数
void CUWParaDlg::OnBnClickedBtnSetIniPara()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CString szKeyName=_T("");
	CString szValue=_T("");

    SetData();

	for (int i = 0; i < nParams; i++)
	{

		
		//_stprintf_s(szValue, 32, _T("%u"), &m_nNum[i]);
		szValue.Format(_T("%d"),m_nNum[i]);


		szKeyName.Format( _T("%d"), i + 1);
		WritePrivateProfileString(_T("Paprams"), szKeyName, szValue, s_inifile);
	}
	AfxMessageBox(GetIdsString(_T("paparms"),_T("sucess2")).c_str());
}

// 设置参数检查
void CUWParaDlg::SetData()
{
	UpdateData(TRUE);
	for (int i = 0 ; i < 44 ;++ i)
	{
		CString strvalue,strNum;
		m_Value[i].GetWindowText(strvalue);
		m_nNum[i]=_wtoi(strvalue);
		if (m_nNum[i] < 0 && m_nNum[i]>50000)
		{
			strNum.Format(GetIdsString(_T("paparms"),_T("err1")).c_str(),i);
			AfxMessageBox(strNum);
			return;
		}
	}
}

void CUWParaDlg::InitUI()
{
   for (int i=0; i< 44 ; ++i)
   {
	   CString strNum;
	   strNum.Format(_T("%d"),i);
	   m_View[i].SetWindowText(GetIdsString(_T("paparms"),strNum.GetBuffer()).c_str());
   }
}
