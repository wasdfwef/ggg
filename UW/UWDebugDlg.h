#pragma once
#include "afxwin.h"


// CUWDebugDlg 对话框

class CUWDebugDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CUWDebugDlg)

public:
	CUWDebugDlg();
	virtual ~CUWDebugDlg();

// 对话框数据
	enum { IDD = IDD_DLG_UW_DEBUG };

private:
	char m_UDCIni[MAX_PATH];
	void GetUDCSettingIniPath();
	void GetLogFileName(char *LogFilePath);
	void LogToFile(const CUDCTestPaperResult &paperResut);
	void GetUDCLogData(const CUDCTestPaperResult &paperResut, CStringA &LogData);
	BOOL GetTransUDCReslut( int nItemIndex, int nMirror, CStringA &strReslut );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnPushin();
	afx_msg void OnBnClickedBtnPushstep();
	afx_msg void OnBnClickedBtnPushstepReset();
	afx_msg void OnBnClickedBtnPushout();
	afx_msg void OnBnClickedBtnBarcode();
	afx_msg void OnBnClickedBtnNeedleDown();
	afx_msg void OnBnClickedBtnNeedleUp();
	afx_msg void OnBnClickedBtnNeedleReset();
	afx_msg void OnBnClickedBtnNeedleSample();
	afx_msg void OnBnClickedBtnNeedleClean();
	afx_msg void OnBnClickedBtnNeedleQc();
	afx_msg void OnBnClickedBtnRollerReset();
	afx_msg void OnBnClickedBtnRollerDebug();
	afx_msg void OnBnClickedBtnSelect1paper();
	afx_msg void OnBnClickedBtnSelect100paper();
	afx_msg void OnBnClickedBtnTurnFore();
	afx_msg void OnBnClickedBtnTurnBack();
	afx_msg void OnBnClickedBtnPaperfeed();
	afx_msg void OnBnClickedBtnXReset();
	afx_msg void OnBnClickedBtnYReset();
	afx_msg void OnBnClickedBtnZReset();
	afx_msg void OnBnClickedBtnZPos();
	afx_msg void OnBnClickedBtnGoto10lens();
	afx_msg void OnBnClickedBtnGoto40lens();
	afx_msg void OnBnClickedBtnMicLightDark();
	afx_msg void OnBnClickedBtnDroppumpTest();
	afx_msg void OnBnClickedBtnCleanpumpTest();
	afx_msg void OnBnClickedBtnUdcTest();
	afx_msg void OnBnClickedBtnPaperTest();
	afx_msg void OnBnClickedBtnRollerLightDark();
	afx_msg void OnBnClickedBtnUdcLight();

public:
	CButton m_btnPushIn;
	CButton m_btnPushStep;
	CButton m_btnPushStep_Reset;
	CButton m_btnPushOut;
	CButton m_btnBarcode;
	CButton m_btnNeedleDown;
	CButton m_btnNeedleUp;
	CButton m_btnNeedleReset;
	CButton m_btnNeedleSample;
	CButton m_btnNeedleClean;
	CButton m_btnNeedleQc;
	CButton m_btnRollerReset;
	CButton m_btnRollerDebug;
	CButton m_btnSelect1Paper;
	CButton m_btnSelect100Paper;
	CButton m_btnTurnFore;
	CButton m_btnTurnBack;
	CButton m_btnPaperFeed;
	CButton m_btn_X_Reset;
	CButton m_btn_Y_Reset;
	CButton m_btn_Z_Reset;
	CButton m_btn_Z_Pos;
	CButton m_btnGoTo10Lens;
	CButton m_btnGoTo40Lens;
	CButton m_btnMicLight;
	CButton m_btnDropPump;
	CButton m_btnCleanPump;
	CButton m_btnUdcTest;
	CButton m_btnPaperTest;
	CButton m_btnRollerLight;

	int m_nLight;
	int m_nMicLight;
	int m_nUdcLight;
	
	
	CButton m_btnUdcLight;
	CStatic m_strInfo;
	CComboBox m_cboPaper;
	CButton m_btnSetPaperType;
	afx_msg void OnBnClickedBtnSetpapertype();
	afx_msg void OnBnClickedBtnGreyPaperTest();
	CButton m_btnGreyPaperTest;
	afx_msg void OnBnClickedButtonSolenoid1();
	afx_msg void OnBnClickedButtonSolenoid2();
	afx_msg void OnBnClickedButtonSolenoid3();
	afx_msg void OnBnClickedButtonSolenoid4();
	afx_msg void OnBnClickedButtonSolenoid5();
	afx_msg void OnBnClickedButtonSolenoid6();
	afx_msg void OnBnClickedButtonSolenoid7();
	afx_msg void OnBnClickedButtonSolenoid8();
};
