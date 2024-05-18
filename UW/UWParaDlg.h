#pragma once
#define  nParams  44

// CUWParaDlg 对话框

class CUWParaDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CUWParaDlg)

public:
	CUWParaDlg();
	virtual ~CUWParaDlg();

// 对话框数据
	enum { IDD = IDD_DLG_UW_PARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    CLCInfo Lcinfo;
	CStatic       m_Value[44];	   // 静态文本控件数组
	int           m_nNum[44];	   // 指数组
	void GetData();
	void SetData();
	void InitUI();
	afx_msg void OnBnClickedBtnGetInitdata();
	afx_msg void OnBnClickedBtnSetInitdata();
	afx_msg void OnBnClickedBtnGetIniPara();
	afx_msg void OnBnClickedBtnSetIniPara();
	CString  lpszIniFileName;

	CStatic      m_View[44];
	TCHAR s_inifile[MAX_PATH];
};
