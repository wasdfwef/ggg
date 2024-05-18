#pragma once
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"

// CPicImageArgvDlg 对话框

class CPicImageArgvDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CPicImageArgvDlg)

public:
	CPicImageArgvDlg();
	virtual ~CPicImageArgvDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PICIMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	void GetFocusPath(char*);
	void GetConfPath(char*);
	void GetConfSettingPath(char*);
	int m_picImageDelay;
	afx_msg void OnBnClickedButton13();
	int m_picImageDelayAutoFocus;
	int m_FocusStep;
	int m_FocusEnd;
	afx_msg void OnBnClickedButton14();
	int m_cleanH;
	int m_cleanM;
	int m_cleanNum;
	afx_msg void OnBnClickedButton15();
	BOOL m_shutdownClean;
	int m_noRecCha;
	int m_noRecImage;
	int m_noRecL;
	int m_noRecT;
	int m_noRecR;
	int m_noRecB;
	afx_msg void OnBnClickedButton16();
	BOOL m_noRecOn;
	afx_msg void OnBnClickedCheck1();
	int m_AutoFocusResultOpt;
	int m_picRow;
	int m_picColumn;
	BOOL m_ontimeClean;
	BOOL m_autoChangePrit;
	afx_msg void OnBnClickedButton17();
	CButton m_checkAutoLight;
	afx_msg void OnBnClickedButton2();
	CButton m_chkHSpeed;
	int m_nHSpeed;
	int m_clean2Internel;
    int m_clean2StrongCleanTimes;
    int m_clean2DailyCleanTimes;
	BOOL m_ontimeClean2;
	CButton m_radio1;
	CButton m_radio2;
	
	CButton m_clean2Strong;
	CButton m_clean2Daily;
    CListCtrl m_itemList;
    afx_msg void OnNMDblclkItemList(NMHDR *pNMHDR, LRESULT *pResult);
    CEdit m_editReliability;
    afx_msg void OnEnKillfocusEdit1Reliability();
    afx_msg void OnLvnBeginScrollList(NMHDR *pNMHDR, LRESULT *pResult);
};
