#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include "ChoiceQcPri.h"
#include "PaintView.h"
#include "afxdtctl.h"
#include <map>

#define TOP_OFSET 3



#define WM_PAINT_VIEW WM_USER + 526
#define WM_PAINT_VIEW_US WM_USER + 527

struct SortData
{
     CListCtrl *listctrl;                 //CListCtrl控件指针
     int isub;        //列号
     int isAsc;        //1为升序，0为降序
};

// UdcQcDlg 对话框
using namespace std;
class UdcQcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UdcQcDlg)

public:
	UdcQcDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~UdcQcDlg();

// 对话框数据
	enum { IDD = IDD_QC_UDC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit;
	CListCtrl m_list;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEditKillFocus();
	void DeleteRecord(int nItem);
	afx_msg void OnNMDClick(NMHDR *pNMHDR, LRESULT *pResult);
	void DoSubItemEdit(int item, int subitem);
	void ReControlSize();
	void AddStringToComBox(CComboBox*  tComb, string tStr);
	void AddStringToComBox();
	int m_Sn;
	int m_row;
	int m_column;
	int days;
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedCheck15();
	void ShowConfigControl(bool isShow = true);
	void OnBnClickedButtonConfOk();
	void ReadRangeFormIni();
	void WriteRangeToIni();
	void UpdateStringToUI();
	void SendMessgeToPaint(int i);
	void SendMessgeToPreview(int i);
	void UpdateUIToString();
	void SaveQCImage();
	void SaveListImage();
	void GetUitPath(char*);
	int SetComboxByValueString(CComboBox* combo,char* strValue);
	bool SetComboxByValueString();
	BOOL m_conf;
	bool isValid;
	CButton m_confCon;
	CTabCtrl m_tab;
	CBitmap bitmapTemp, *pOldBitmap;
	std::vector<std::string> rangeStr;
	std::vector<std::string> valueStr;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	CPaintView *m_paintView;
    int calcTimeDiff(const char*, const char*);
	char curItemName[64];
	CPaintView *m_preView;
	afx_msg void OnBnClickedButton2();
	CStatic m_prelabel;
	int m_priType;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	CDateTimeCtrl m_dtFrom;
	CDateTimeCtrl m_dtTo;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    CButton m_radioP;
    CButton m_radioN;
    afx_msg void OnBnClickedRadio();
    afx_msg void OnBnClickedButtonSend();

private:
    std::map<CString, CString> GetCurDateRange() const;
public:
    afx_msg void OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);
};

int CALLBACK listCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);