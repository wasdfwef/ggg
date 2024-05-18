#pragma once
#include "afxwin.h"
#include "PngButton.h"
#include "afxcmn.h"
#include "UdcQcDlg.h"
#include "UsQcDlg.h"
#include "InsertQcTask.h"

//在质控记录的数据库中，诊断结果表示批号，建议表示有效期
#define WM_USER_ADDTASKTOQC WM_USER+505
// CQcDlg 对话框

class CQcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CQcDlg)

public:
	CQcDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CQcDlg();

// 对话框数据
	enum { IDD = IDD_QC_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	int nFlag;
	
	void AddTaskToQcList(CListCtrl &list, PTASK_INFO pInfo, int nItem);
	void AddTaskToQcListInit(CListCtrl &list, int nID, PTASK_INFO info);
	void UpDateTaskToQcUdcList(PTASK_INFO tInfo);
	void UpDateTaskToQcUsList(PTASK_INFO tInfo);
	void Init();
	void DeleTodayReportLast(PTASK_INFO pInfo);

private:
	TestDes m_TestType;
	bool isInit;

public:
	BOOL QCopend;
	int m_TestTypeOpendQC;

	virtual BOOL OnInitDialog();
	void OnSize(UINT nType, int cx, int cy);
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedQC();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonNew();
	afx_msg LRESULT AddTasktoQC(WPARAM wParam, LPARAM lParam);
	void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	void ShowTaskStatus(int nID, bool isUDC = true);
	int GetIndexByTaskID(CListCtrl & list, int nID);
	void FillList(void);
    bool WsQcCtrl(bool qc, int mode, int item);

	CButton m_cancel;
	CTabCtrl m_tab;
	CPngButton* m_enableButton;
	CPngButton* m_disableButton;
	UdcQcDlg m_udcDlg;
	UsQcDlg m_usDlg;
	int m_editSn;
    CButton m_radioUDC;
    CButton m_radioUS;
    afx_msg void OnBnClickedQcType();
    BOOL m_qcChecked[6];

};
