// PrintModuleSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CPrintModuleSet.h"
#include "PrintModuleSetDlg.h"
#include "DbgMsg.h"
#include "..\..\..\inc\THReportAPI.h"
#pragma comment(lib, "..\\..\\..\\lib\\THReportAPI.lib")

// CPrintModuleSetDlg 对话框

IMPLEMENT_DYNAMIC(CPrintModuleSetDlg, CDialog)

CPrintModuleSetDlg::CPrintModuleSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintModuleSetDlg::IDD, pParent)
{
	m_sIniFilePath = "";
	m_temp_setting_info.counts = 0;
	for(int i = 0 ; i < MAX_TEMP_SETTING_COUNTS ; i ++ )
	{
		m_temp_setting_info.node[i].spath = "";
		m_temp_setting_info.node[i].scomment="";
	}

}

CPrintModuleSetDlg::~CPrintModuleSetDlg()
{
}

void CPrintModuleSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit_temp_modify_info);
	DDX_Control(pDX, IDC_LIST1, m_list_temp_setting);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btn_temp_setting_add);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btn_temp_setting_del);
	DDX_Control(pDX, IDC_BUTTON_MODIFY, m_btn_temp_setting_modify);
	DDX_Control(pDX, IDC_BUTTON_SETTASK, m_btn_temp_setting_set_task);
	DDX_Control(pDX, IDC_BUTTON_SETQC, m_btn_temp_setting_set_qc);
	DDX_Control(pDX, IDOK, m_btn_ok);
	DDX_Control(pDX, IDCANCEL, m_btn_cancel);
	DDX_Control(pDX, IDC_STATIC_NAME, m_static_temp_modify_name);
}


BEGIN_MESSAGE_MAP(CPrintModuleSetDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPrintModuleSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPrintModuleSetDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CPrintModuleSetDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CPrintModuleSetDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CPrintModuleSetDlg::OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_SETTASK, &CPrintModuleSetDlg::OnBnClickedButtonSettask)
	ON_BN_CLICKED(IDC_BUTTON_SETQC, &CPrintModuleSetDlg::OnBnClickedButtonSetqc)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CPrintModuleSetDlg::OnLvnItemchangedList1)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CPrintModuleSetDlg::OnEnKillfocusEdit1)
END_MESSAGE_MAP()


// CPrintModuleSetDlg 消息处理程序

BOOL CPrintModuleSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitializeRes();
	m_sIniFilePath = GetIniFilePath("Conf.ini");

	m_list_temp_setting.InsertColumn(0,GetIdsString("temp_set","name").c_str(),0,300);
	m_list_temp_setting.InsertColumn(1,GetIdsString("temp_set","comment").c_str(),0,250);
	m_list_temp_setting.ModifyStyle(0,LVS_SHOWSELALWAYS);
	m_list_temp_setting.SetExtendedStyle(m_list_temp_setting.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	GetTemp();

	for(int i = 0 ; i < m_temp_setting_info.counts ; i ++)
	{
		m_list_temp_setting.InsertItem(i,m_temp_setting_info.node[i].spath.c_str());
		m_list_temp_setting.SetItemText(i,1,m_temp_setting_info.node[i].scomment.c_str());
	}

	SetControlsStatus(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPrintModuleSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_temp_setting_info.counts = 0;
	for(int i = 0 ; i < m_list_temp_setting.GetItemCount(); i ++)
	{
		m_temp_setting_info.counts ++;
		m_temp_setting_info.node[i].spath = m_list_temp_setting.GetItemText(i,0);
		m_temp_setting_info.node[i].scomment = m_list_temp_setting.GetItemText(i,1);
	}

	SetTemp();

	OnOK();
}

void CPrintModuleSetDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CPrintModuleSetDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string sPath = MakeTempPath();

	BOOL bRet = THReportAPI_NewReportModule((PCHAR)sPath.c_str());

	if(bRet == FALSE)
	{
		AfxMessageBox(GetIdsString("temp_set","temp_err").c_str());
	}
	else
	{

		if(GetFileAttributes(sPath.c_str()) == INVALID_FILE_ATTRIBUTES)
		{
			AfxMessageBox(GetIdsString("temp_set","temp_err").c_str());
		}
		else
		{
			m_temp_setting_info.node[m_temp_setting_info.counts].spath = sPath;
			m_temp_setting_info.node[m_temp_setting_info.counts].scomment = "";
			m_temp_setting_info.counts++;
			SetTemp();
			m_list_temp_setting.DeleteAllItems();

			GetTemp();

			for(int i = 0 ; i < m_temp_setting_info.counts ; i ++)
			{
				m_list_temp_setting.InsertItem(i,m_temp_setting_info.node[i].spath.c_str());
				m_list_temp_setting.SetItemText(i,1,m_temp_setting_info.node[i].scomment.c_str());
			}


		}


	}
}

void CPrintModuleSetDlg::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
	int iItem = -1;
	POSITION pos;
	int k = m_list_temp_setting.GetItemCount() - 1;




	pos = m_list_temp_setting.GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		iItem = m_list_temp_setting.GetNextSelectedItem(pos);
	}

	if(iItem != -1)
	{
		char p1[MAX_PATH] = "";

		m_list_temp_setting.GetItemText(iItem, 0, p1, MAX_PATH);

		if( strcmp(GetPrintTemp().c_str(), p1) == 0 )
			WritePrivateProfileString("temp", "print_temp", NULL, m_sIniFilePath.c_str());

		sprintf(p1,"commt%d",k);
		WritePrivateProfileString("temp", p1, NULL, m_sIniFilePath.c_str());
		sprintf(p1,"item%d",k);
		WritePrivateProfileString("temp", p1, NULL, m_sIniFilePath.c_str());
		m_list_temp_setting.DeleteItem(iItem);
	}

	m_temp_setting_info.counts = 0;
	for(int i = 0 ; i < m_list_temp_setting.GetItemCount(); i ++)
	{
		m_temp_setting_info.counts ++;
		m_temp_setting_info.node[i].spath = m_list_temp_setting.GetItemText(i,0);
		m_temp_setting_info.node[i].scomment = m_list_temp_setting.GetItemText(i,1);
	}

	SetTemp();
}

void CPrintModuleSetDlg::OnBnClickedButtonModify()
{
	// TODO: 在此添加控件通知处理程序代码
	int iItem = -1;
	POSITION pos;


	pos = m_list_temp_setting.GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		iItem = m_list_temp_setting.GetNextSelectedItem(pos);
	}

	if(iItem != -1)
	{

		BOOL bRet =  THReportAPI_ModifyReportModule((PCHAR)m_list_temp_setting.GetItemText(iItem,0).GetBuffer());


	}
}

void CPrintModuleSetDlg::OnBnClickedButtonSettask()
{
	// TODO: 在此添加控件通知处理程序代码
	int iItem = -1;
	POSITION pos;

	char sContext[MAX_PATH] = {0};
	char szBuf[MAX_PATH] = {0};

	pos = m_list_temp_setting.GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		iItem = m_list_temp_setting.GetNextSelectedItem(pos);
	}

	if(iItem != -1)
	{
		std::string str = m_list_temp_setting.GetItemText(iItem,0);
		SetPrintTemp(str);

		for(int i = 0 ; i < m_list_temp_setting.GetItemCount() ; i ++)
		{
			char szBuf[MAX_PATH]={0};
			m_list_temp_setting.GetItemText(i,1,szBuf,MAX_PATH);

			if(strcmp(szBuf,"Print Default Templete")==0)
			{
				m_list_temp_setting.SetItemText(i,1,"");

				sprintf(szBuf,"commt%d",i);
				WritePrivateProfileString("temp",
					szBuf,
					"",
					m_sIniFilePath.c_str());

			}

		}
		m_list_temp_setting.SetItemText(iItem,1,"Print Default Templete");
		
		sprintf(sContext,"Print Default Templete");
		sprintf(szBuf,"commt%d",iItem);
		WritePrivateProfileString("temp",
			szBuf,
			sContext,
			m_sIniFilePath.c_str());
	}
}

void CPrintModuleSetDlg::OnBnClickedButtonSetqc()
{
	// TODO: 在此添加控件通知处理程序代码
	int iItem = -1;
	POSITION pos;

	pos = m_list_temp_setting.GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		iItem = m_list_temp_setting.GetNextSelectedItem(pos);
	}

	if(iItem != -1)
	{
		std::string str = m_list_temp_setting.GetItemText(iItem,0);
		SetQCPrintTemp(str);

		for(int i = 0 ; i < m_list_temp_setting.GetItemCount() ; i ++)
		{
			char szBuf[MAX_PATH]={0};
			m_list_temp_setting.GetItemText(i,1,szBuf,MAX_PATH);

			if(strcmp(szBuf,"QC Print Default Templete")==0)
			{
				m_list_temp_setting.SetItemText(i,1,"");
			}
		}
		m_list_temp_setting.SetItemText(iItem,1,"QC Print Default Templete");
	}

}

void CPrintModuleSetDlg::SetTemp( void )
{
	char sContext[MAX_PATH]={0};
	sprintf(sContext,"%d",m_temp_setting_info.counts);

	WritePrivateProfileString("temp",
		"counts",
		sContext,
		m_sIniFilePath.c_str());

	for(int i = 0 ; i < m_temp_setting_info.counts ; i++)
	{
		char szBuf[MAX_PATH]={0};
		char sContext[MAX_PATH]={0};
		sprintf(sContext,"%s",m_temp_setting_info.node[i].spath.c_str());
		sprintf(szBuf,"item%d",i);
		WritePrivateProfileString("temp",
			szBuf,
			sContext,
			m_sIniFilePath.c_str());

		sprintf(sContext,"%s",m_temp_setting_info.node[i].scomment.c_str());
		sprintf(szBuf,"commt%d",i);

		WritePrivateProfileString("temp",
			szBuf,
			sContext,
			m_sIniFilePath.c_str());
	}

}

void CPrintModuleSetDlg::GetTemp( void )
{
	char szLine[MAX_PATH*8] = {0};

	GetPrivateProfileString("temp",
		"counts",
		_T("0"),
		szLine,
		MAX_PATH*8,
		m_sIniFilePath.c_str());

	m_temp_setting_info.counts = atoi(szLine);


	for(int i = 0 ; i < m_temp_setting_info.counts ; i++)
	{
		char szBuf[MAX_PATH]={0};
		sprintf(szBuf,"item%d",i);

		GetPrivateProfileString("temp",
			szBuf,
			_T("0"),
			szLine,
			MAX_PATH*8,
			m_sIniFilePath.c_str());

		m_temp_setting_info.node[i].spath = szLine;


		sprintf(szBuf,"commt%d",i);

		GetPrivateProfileString("temp",
			szBuf,
			_T("0"),
			szLine,
			MAX_PATH*8,
			m_sIniFilePath.c_str());

		m_temp_setting_info.node[i].scomment = szLine;

	}


}

void CPrintModuleSetDlg::SetControlsStatus( BOOL bEnable )
{
	m_btn_temp_setting_del.EnableWindow(bEnable);
	m_btn_temp_setting_modify.EnableWindow(bEnable);
	m_btn_temp_setting_set_task.EnableWindow(bEnable);
	m_btn_temp_setting_set_qc.EnableWindow(bEnable);

}

void CPrintModuleSetDlg::SetPrintTemp( std::string sTempPath )
{
	WritePrivateProfileString("temp",
		"print_temp",
		sTempPath.c_str(),
		m_sIniFilePath.c_str());

}

std::string CPrintModuleSetDlg::GetPrintTemp( void )
{
	char szLine[MAX_PATH*8] = {0};

	GetPrivateProfileString("temp",
		"print_temp",
		"0",
		szLine,
		MAX_PATH*8,
		m_sIniFilePath.c_str());

	return std::string(szLine);

}

void CPrintModuleSetDlg::SetQCPrintTemp( std::string sQCTempPath )
{
	WritePrivateProfileString("temp",
		"qc_temp",
		sQCTempPath.c_str(),
		m_sIniFilePath.c_str());

}

std::string CPrintModuleSetDlg::GetQCPrintTemp( void )
{
	char szLine[MAX_PATH*8] = {0};

	GetPrivateProfileString("temp",
		"qc_temp",
		"0",
		szLine,
		MAX_PATH*8,
		m_sIniFilePath.c_str());

	return std::string(szLine);

}
void CPrintModuleSetDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int iItem = -1;
	POSITION pos;


	pos = m_list_temp_setting.GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		iItem = m_list_temp_setting.GetNextSelectedItem(pos);
	}

	if(iItem != -1)
	{
		SetControlsStatus(TRUE);
		// 设置显示内容

		CString str = m_list_temp_setting.GetItemText(iItem,0);
		m_static_temp_modify_name.SetWindowText(str);

		str = m_list_temp_setting.GetItemText(iItem,1);
		m_edit_temp_modify_info.SetWindowText(str);


	}
	else
	{
		m_static_temp_modify_name.SetWindowText(GetIdsString("temp_set","comment_modify").c_str());
		m_edit_temp_modify_info.SetWindowText("");


		SetControlsStatus(FALSE);
	}


	*pResult = 0;
}

void CPrintModuleSetDlg::OnEnKillfocusEdit1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_edit_temp_modify_info.GetWindowText(str);

	CString sName;
	m_static_temp_modify_name.GetWindowText(sName);

	for(int i = 0 ; i < m_list_temp_setting.GetItemCount() ; i ++)
	{
		CString sItem = m_list_temp_setting.GetItemText(i,0);

		if(strcmp(sItem,sName)==0)
		{
			m_list_temp_setting.SetItemText(i,1,str);
			break;
		}


	}
}

void CPrintModuleSetDlg::InitializeRes( void )
{
	std::string str = GetIdsString("temp_set","title");
	SetWindowText(str.c_str());


	str = GetIdsString("temp_set","add");
	m_btn_temp_setting_add.SetWindowText(str.c_str());

	str = GetIdsString("temp_set","del");
	m_btn_temp_setting_del.SetWindowText(str.c_str());

	str = GetIdsString("temp_set","modify");
	m_btn_temp_setting_modify.SetWindowText(str.c_str());

	str = GetIdsString("temp_set","settask");
	m_btn_temp_setting_set_task.SetWindowText(str.c_str());

	str = GetIdsString("temp_set","setqc");
	m_btn_temp_setting_set_qc.SetWindowText(str.c_str());

	str = GetIdsString("temp_set","ok");
	m_btn_ok.SetWindowText(str.c_str());

	str = GetIdsString("temp_set","cancel");
	m_btn_cancel.SetWindowText(str.c_str());

}

std::string CPrintModuleSetDlg::GetIdsString( std::string sMain,std::string sId )
{
	char s_inifile[MAX_PATH]={0};

	sprintf(s_inifile,"%s",GetIniFilePath("2052.ini").c_str());

	DBG_MSG("s_inifile=%s\n",s_inifile);

	TCHAR szLine[MAX_PATH*8] = {0};
	std::string str;

	if( 0 !=GetPrivateProfileString(sMain.c_str(),
		sId.c_str(),
		_T("0"),
		szLine,
		MAX_PATH*8,
		s_inifile))
	{
		str = szLine;
	}
	else
	{
		str = "";
	}

	return str;

}

std::string CPrintModuleSetDlg::GetIniFilePath( std::string str )
{
	char path[MAX_PATH]={0};
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);	
	PathAppend(path,str.c_str());
	return std::string(path);
}

std::string CPrintModuleSetDlg::MakeTempPath( void )
{
	char sModuleFile[MAX_PATH]={0};
	char sDrive[MAX_PATH]={0};
	char sDir[MAX_PATH]={0};


	GetModuleFileName(NULL, sModuleFile, MAX_PATH);

	_splitpath(sModuleFile, sDrive, sDir, NULL,NULL);


	CTime tmSCan = CTime::GetCurrentTime();
	CString szTime = tmSCan.Format("%Y%m%d_%H%M%S"); 

	char szBuf[MAX_PATH]={0};
	sprintf(szBuf,"%s%s%s%s.%s",sDrive,sDir,"Model\\",szTime,"uit");

	return std::string(szBuf);

}