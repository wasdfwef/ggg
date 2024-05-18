#pragma once
#include "resource.h"
#include <map>
#include "afxwin.h"
#include "LogFile.h"
#include "Include/Common/String.h"

// CDbgUdc 对话框

class CDbgUdc : public CPropertyPage
{
	DECLARE_DYNAMIC(CDbgUdc)

private:
	std::map<int, void*> m_mapEventDelegate;

public:
	CDbgUdc();
	virtual ~CDbgUdc();

// 对话框数据
	enum { IDD = IDD_DLG_DBG_UDC_TEST };

public:
	void OnRecvSelAndFeedPaperTestData(void *arg, void *cbarg);
	void OnRecvRepeatTestData(void *arg, void *cbarg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedFeedPaperStartTest();
	afx_msg void OnBnClickedFeedPaperStopTest();
	afx_msg void OnBnClickedUdcStartRepeatTest();
	afx_msg void OnBnClickedUdcStopRepeatTest();
	afx_msg LRESULT RecvSelFeedPaperTestDataHandler(WPARAM w, LPARAM l);
	afx_msg LRESULT RecvRepeatTestDataHandler(WPARAM w, LPARAM l);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ReChildWndsize();
	afx_msg void OnBnClickedButton3();
	void CalcCvByBlock();
	double CalcCvByBlock(int *num, int index);
	void WriteToTXT();
	bool isCalced;
	CButton m_btnCV;
};
