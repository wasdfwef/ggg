#pragma once
#include "resource.h"
#include "FindRecord.h"
#include "afxwin.h"
// SearchRecordDlg 对话框

class SearchRecordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SearchRecordDlg)

public:
	SearchRecordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SearchRecordDlg();

// 对话框数据
	void InitRes();

	enum { IDD = IDD_DIALOG_SEARCH_RECORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void ConfigPropList();
	// 重新设置除医生以外属性列表的选项
	void ResetOtherOption();
	// 重新设置用户属性列表的选项
	void ResetUserOption();

	void ResetDepartmentOption();
	// 重新设置医生属性列表的选项
	void ResetDoctorOption();
	// 查找记录
	void Search();

	BOOL CheckDateLegit(CString strDate);

public:

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRecordIdcancel();
	CComboBox m_checker;
	CComboBox m_auditer;
	CComboBox m_reporter;
	CComboBox m_qc;
	CComboBox m_hospital;
	afx_msg void OnCbnKillfocusComboSearchRecordHospital();
	afx_msg void OnEnKillfocusEditSearchRecordRoom();
	CEdit m_room;
	CEdit m_doctor;
	afx_msg void OnBnClickedRecordIdok();
	CEdit m_start_time;
	CEdit m_end_date;
	CEdit m_end_time;
	CEdit m_start_date;
	CEdit m_sn;
	CEdit m_barcode;
	CEdit m_name;
	CEdit m_number;
	afx_msg void OnBnClickedButtonSearchRecordDate();
};
