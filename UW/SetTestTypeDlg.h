#pragma once


// CSetTestTypeDlg 对话框

class CSetTestTypeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetTestTypeDlg)

public:
	CSetTestTypeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetTestTypeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SETTESTTYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	void initUI();

public:
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioType();
	afx_msg void OnBnClickedRadioType1();
	afx_msg void OnBnClickedRadioType2H1();
	afx_msg void OnBnClickedRadioType2H2();
	afx_msg void OnBnClickedRadioType2H3();
	afx_msg void OnBnClickedRadioType2H4();
	afx_msg void OnBnClickedRadioType2H5();
	afx_msg void OnBnClickedRadioType2H6();
	afx_msg void OnBnClickedRadioType2H7();
	afx_msg void OnBnClickedRadioType2H8();
	afx_msg void OnBnClickedRadioType2H9();
	afx_msg void OnBnClickedRadioType2H10();
	afx_msg void OnBnClickedRadioType2H11();
};
