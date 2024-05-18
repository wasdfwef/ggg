#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <string>
#include <Shlwapi.h>


#define MAX_TEMP_SETTING_COUNTS 1024

typedef struct _TEMP_SETTING_NODE
{
	std::string spath;
	std::string scomment;


}TEMP_SETTING_NODE,*PTEMP_SETTING_NODE;

typedef struct _TEMP_SETTING_INFO
{
	int   counts;
	TEMP_SETTING_NODE node[MAX_TEMP_SETTING_COUNTS];

}TEMP_SETTING_INFO,*PTEMP_SETTING_INFO;

// CPrintModuleSetDlg 对话框

class CPrintModuleSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CPrintModuleSetDlg)

public:
	CPrintModuleSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPrintModuleSetDlg();

// 对话框数据
	enum { IDD = IDD_PRINTMODULESETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	void InitializeRes(void);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_edit_temp_modify_info;
	CListCtrl m_list_temp_setting;
	CButton m_btn_temp_setting_add;
	CButton m_btn_temp_setting_del;
	CButton m_btn_temp_setting_modify;
	CButton m_btn_temp_setting_set_task;
	CButton m_btn_temp_setting_set_qc;
	CButton m_btn_ok;
	CButton m_btn_cancel;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonSettask();
	afx_msg void OnBnClickedButtonSetqc();
	CStatic m_static_temp_modify_name;


	std::string GetIdsString(std::string sMain,std::string sId);
	std::string	GetIniFilePath(std::string  str);	
	void SetTemp(void);
	void GetTemp(void);
	void SetControlsStatus(BOOL bEnable);

	std::string MakeTempPath(void);

	std::string m_sIniFilePath;

	// 写打印模板
	void SetPrintTemp(std::string sTempPath);

	// 读打印模板
	std::string GetPrintTemp(void);

	// 写QC打印模板
	void SetQCPrintTemp(std::string sQCTempPath);

	// 读QC打印模板
	std::string GetQCPrintTemp(void);


	TEMP_SETTING_INFO m_temp_setting_info;
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEdit1();
	
};
