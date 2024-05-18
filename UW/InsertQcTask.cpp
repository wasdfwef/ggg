// InsertQcTask.cpp : 实现文件
//

#include "stdafx.h"
#include "InsertQcTask.h"
#include "UW.h"
#include "afxdialogex.h"
#include "locale.h"
#include "UdcQcDlg.h"


// CInsertQcTask 对话框

IMPLEMENT_DYNAMIC(CInsertQcTask, CDialogEx)

CInsertQcTask::CInsertQcTask(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInsertQcTask::IDD, pParent)
	, m_InsertSn(1)
{

}

CInsertQcTask::~CInsertQcTask()
{
}

void CInsertQcTask::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_qcType);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_date);
	DDX_Control(pDX, IDC_DATETIMEPICKER5, m_time);
	DDX_Text(pDX, IDC_EDIT1, m_InsertSn);
}


BEGIN_MESSAGE_MAP(CInsertQcTask, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CInsertQcTask::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON13, &CInsertQcTask::OnBnClickedButton13)
END_MESSAGE_MAP()


// CInsertQcTask 消息处理程序


void CInsertQcTask::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	m_curSel = m_qcType.GetCurSel();
	CTime dt;
	m_date.GetTime(dt);
	m_insertDate = dt.Format("%Y-%m-%d");
	m_date.GetTime(dt);
	m_InsertTime = dt.Format("%H:%M:%S");

	OnOK();
}


void CInsertQcTask::OnBnClickedButton13()
{
	// TODO:  在此添加控件通知处理程序代码
	OnCancel();
}


BOOL CInsertQcTask::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	m_qcType.InsertString(0, USI_QC);
	m_qcType.InsertString(1, USII_QC);
    m_qcType.InsertString(2, USIII_QC);
	m_qcType.InsertString(3, UDCI_QC);
	m_qcType.InsertString(4, UDCII_QC);

	m_qcType.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
