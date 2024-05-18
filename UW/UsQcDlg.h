#pragma once
#include "resource.h"
#include "afxcmn.h"
#include <vector>
#include "ChoiceQcPri.h"
#include "PaintView.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include <map>


// UsQcDlg 对话框



class UsQcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UsQcDlg)

public:
	UsQcDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~UsQcDlg();

// 对话框数据
	enum { IDD = IDD_QC_US };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	int m_Sn;
	CEdit m_Edit;
	int m_row;
	int m_column;
	int days;
	BOOL m_conf;
	bool isValid;
	CButton m_confCon;
	CTabCtrl m_tab;
	CPaintView *m_paintView;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ReControlSize();
	virtual BOOL OnInitDialog();
	void ShowConfigControl(bool isShow = true);
	afx_msg void OnBnClickedCheck15();
	afx_msg void OnBnClickedButton1();
	void OnEditKillFocus();
	void DeleteRecord(int nItem);
	void OnNMDClick(NMHDR *pNMHDR, LRESULT *pResult);
	void DoSubItemEdit(int item, int subitem);
	void SaveQCImage();
	afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButtonSend();
	void GetUitPath(char* qcIni);
	void SaveListImage();
	void SendMessgeToPreview(int iColumn);
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonConfOk();
	std::vector<std::string> rangeStr;
	std::vector<std::string> valueStr;
	void ReadRangeFormIni();
	void WriteRangeToIni();
	void UpdateStringToUI();
	void UpdateUIToString();
	void SendMessgeToPaint(int iColumn);
	void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	int m_priType;
    int calcTimeDiff(const char* from, const char* to);
    int m_max, m_min;
    int m_max2, m_min2;
    int m_max3, m_min3;
	CPaintView *m_preView;
	CStatic m_prelabel;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCalcCv();
	void CalcCvByQC();
	double CalcCvByBlock(int *num, int index);
	CDateTimeCtrl m_dtFrom;
	CDateTimeCtrl m_dtTo;
	afx_msg void OnBnClickedButtonSelpic();
	afx_msg void OnBnClickedButtonReccircle();
	afx_msg void OnBnClickedCheckQctest();
	afx_msg void OnBnClickedButtonSetArg();
	afx_msg void OnBnClickedButtonResetArg();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    CButton m_radio1;
    CButton m_radio2;
    CButton m_radio3;
    afx_msg void OnBnClickedRadio();

private:
    std::map<CString, CString> GetCurDateRange() const;
public:
    afx_msg void OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);
};
