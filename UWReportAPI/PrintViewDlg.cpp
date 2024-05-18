// PrintViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UWReportAPI.h"
#include "PrintViewDlg.h"


// CPrintViewDlg 对话框

IMPLEMENT_DYNAMIC(CPrintViewDlg, CDialog)

CPrintViewDlg::CPrintViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintViewDlg::IDD, pParent)
{

}

CPrintViewDlg::~CPrintViewDlg()
{
}

void CPrintViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_view_print_button);
	DDX_Control(pDX, IDC_BUTTON2, m_CancelPrintbutton);
}


BEGIN_MESSAGE_MAP(CPrintViewDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPrintViewDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPrintViewDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CPrintViewDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPrintViewDlg::OnBnClickedButton2)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPrintViewDlg 消息处理程序


BOOL CPrintViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	this->ShowWindow(SW_MAXIMIZE);
	GetIniFilePath(m_sIniFilePath);
	m_view_print_button.SetWindowText(GetIdsString("view","print").c_str());
	m_CancelPrintbutton.SetWindowText(GetIdsString("view","cancel").c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

HBRUSH CPrintViewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔

	if( nCtlColor==CTLCOLOR_DLG)
	{
		pDC->SetBkMode(TRANSPARENT); 

		static CBrush brush(RGB(0,0,0));
		hbr = brush;

	}

	return hbr;
}

void CPrintViewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	THReport_PrintPrview(m_handle,m_hWnd);
}
void CPrintViewDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CPrintViewDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CPrintViewDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CPrintViewDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}

void CPrintViewDlg::SetPrinterHandle( HANDLE handle,BOOL bDisplayPrnBtn /*= FALSE*/ )
{
	m_handle = handle;
	m_DislpayPrnBtn = bDisplayPrnBtn;

}

std::string CPrintViewDlg::GetIniFilePath( std::string str )
{
	char path[MAX_PATH]={0};

	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);	
	PathAppend(path,str.c_str());
	return std::string(path);

}

std::string CPrintViewDlg::GetIdsString( std::string sMain,std::string sId )
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