#pragma once


// CChoiceQcPri 对话框

class CChoiceQcPri : public CDialogEx
{
	DECLARE_DYNAMIC(CChoiceQcPri)

public:
	CChoiceQcPri(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChoiceQcPri();
	int m_priType;
// 对话框数据
	enum { IDD = IDD_DIALOG_CHOICE_QC_PRI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton30();
	virtual BOOL OnInitDialog();
};
