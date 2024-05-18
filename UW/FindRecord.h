/*****************************************************************************
Copyright (C), 1995-2010, Chongqing Tianhai Medical Equipment Co., Ltd.
文件名:		FindRecord.h
功能:		查找记录
时间:		2010-05
*******************************************************************************/
#pragma once
#include "afxdtctl.h"

//#include "Resource/FindRecordRes.h"
// CFindRecord

class CFindRecord : public CDockablePane
{
	DECLARE_DYNAMIC(CFindRecord)

public:
	CFindRecord();
	virtual ~CFindRecord();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);
	afx_msg void OnBnClickedSearch();
	afx_msg void OnUpdateSearch(CCmdUI *pCmdUI);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	CMFCPropertyGridCtrl m_FindPropList; // 查询条件属性列表
	//Common::Recordset m_Rs;              // 数据集
	CButton m_btnSearch;                 // 搜索按钮

public:
	// 初始化查询条件属性列表 
	void InitFindList();
	// 重新设置除医生以外属性列表的选项
	void ResetOtherOption();
	// 重新设置用户属性列表的选项
	void ResetUserOption();
	// 查找记录
	void Search();




private:
	// 配置属性列表
	void ConfigPropList();
	// 重新设置医生属性列表的选项
	void ResetDoctorOption();
	void ResetDepartmentOption();
	// 调整显示
	void AdjustLayout();

	// 日期值是否合法
	BOOL CheckDateLegit(CString strDate);
};

// CMFCPropertyGridDateTimeProperty

class CMFCPropertyGridDateTimeProperty : public CMFCPropertyGridProperty
{
public:
	CMFCPropertyGridDateTimeProperty(const CString& strName
		, const _variant_t& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CMFCPropertyGridDateTimeProperty();

public:
	virtual void OnClickButton(CPoint point);
};

// CFindRecordSetDate 对话框

class CFindRecordSetDate : public CDialog
{
	DECLARE_DYNAMIC(CFindRecordSetDate)

public:
	CFindRecordSetDate(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFindRecordSetDate();

	// 对话框数据
	enum { IDD = IDD_DIALOG_SET_DATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

public:
	CDateTimeCtrl m_DateTimeCtrl;// 时间控件
	CString m_strDate,m_strDate2,m_strTime,m_strTime2;           // 格式化后的时间字符串
	CDateTimeCtrl m_TimeCtrl;
	CDateTimeCtrl m_DateCtrl2;
	CDateTimeCtrl m_TimeCtrl2;
	COleDateTime time1,time2;
	virtual BOOL OnInitDialog();
	afx_msg void OnDtnDatetimechangeDatetimepicker4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult);
};
