#pragma once
#include "afxcmn.h"
#include "UW.h"
#include "afxwin.h"

// CUdcCriterionDlg 对话框

#ifndef CPP_EXAMPLE_H
#define CPP_EXAMPLE_H
extern "C" void InitUdcini();
#endif

class CUdcCriterionDlg : public CDialog
{
	DECLARE_DYNAMIC(CUdcCriterionDlg)

public:
	CUdcCriterionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUdcCriterionDlg();

// 对话框数据
	enum { IDD = IDD_UDCITEM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CStatic m_ctsItem;
	CStatic m_ctsName;
	CButton m_ctsGrade;
	CStatic m_csReflect;
	CStatic m_csValue;
	CStatic m_csUnit;
	CListCtrl m_udcList;
	CListCtrl m_itemList;

	int m_nItemIndex;
	int m_nGradeIndex;

	CString m_strItem,m_strName;
	//int m_nNum;
	BOOL m_bItemEnable;
	void InitUdcList();
	// void InitUdcini();
	void InsertUdcItem();
	void UpdateItemList(void);
    void UpdateGradeList(int nIndex);
	void SaveIni();
	afx_msg void OnNMClickUdcList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnItemchangedUdcList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnItemchangingUdcList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnItemchangedUdcList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnItemclickUdcList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnLvnItemchangedUdcList(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnLvnItemchangingUdcList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnUse();
	CString m_ceItem;
	CString m_ceName;
	CString m_ceGrade;
	CString m_ceReflect;
	CString m_ceValue;
	CString m_ceUnit;
	TCHAR s_inifile[MAX_PATH];
	afx_msg void OnNMClickItemList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnUdcSave();
	afx_msg void OnBnClickedBtnUdcNew();
	afx_msg void OnBnClickedBtnUdcDel();
};
