#pragma once
#include "afxwin.h"

#include <xstring>
using namespace std;
#include <algorithm>
#include <Shlwapi.h>
#include "DbgMsg.h"
#include "..\..\inc\THReportAPI.h"
#pragma comment(lib, "..\\..\\lib\\THReportAPI.lib")

// CPrintViewDlg 对话框

class CPrintViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CPrintViewDlg)

public:
	CPrintViewDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPrintViewDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PRINT_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CButton m_view_print_button;
	CButton m_CancelPrintbutton;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();


	void SetPrinterHandle(HANDLE handle,BOOL bDisplayPrnBtn = FALSE);

	HANDLE m_handle;
	BOOL   m_DislpayPrnBtn;

	std::string	  GetIniFilePath(std::string  str);
	std::string   GetIdsString(std::string sMain,std::string sId);
	std::string	  m_sIniFilePath;
};
