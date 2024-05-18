#pragma once
#include "resource.h"
#include "vector"
#include "CmdHelper.h"
#include "UW.h"
#include "LogFile.h"
// CStdUdcTest 对话框

class CStdUdcTest : public CPropertyPage
{
	DECLARE_DYNAMIC(CStdUdcTest)

public:
	CStdUdcTest();   // 标准构造函数
	virtual ~CStdUdcTest();

// 对话框数据
	enum { IDD = IDD_DIALOG_STDUDC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton14();
	virtual BOOL OnInitDialog();
	int testNum ;
	int listRectTop;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void CalcCvByBlock();
	double CalcCvByBlock(int *,int);
	void WriteToTXT();
	afx_msg void OnBnClickedButton44();
	bool isCalcd;
};
