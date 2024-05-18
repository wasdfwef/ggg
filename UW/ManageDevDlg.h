#pragma once
#include "UWDebugDlg.h"
#include "UWParaDlg.h"
#include "DebugSheet.h"

#include "PicImageArgvDlg.h"
#include "DbgAction.h"
#include "DbgUdc.h"
#include "DbgSensor.h"
#include "DbgMicroSecope.h"
#include "DbgCharacterBarcode.h"
#include "DbgMotorSolenoid.h"
#include "DbgHardwareparam.h"
#include "MoveVisioDlg.h"
#include "StdUdcTest.h"
#include "SoftwareSetUI.h"
#include "MotorConfPage.h"
#include <map>

#define DEV			1
#define SOFTWARE	2
// CManageDevDlg 对话框

class CManageDevDlg : public CDialog
{
	DECLARE_DYNAMIC(CManageDevDlg)

private:
	std::map<int, void*> m_mapEventDelegate;

public:
	CManageDevDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManageDevDlg();

// 对话框数据
	enum { IDD = IDD_DLG_DEV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	CDebugSheet  m_Sheet;
	CUWDebugDlg  m_page1;
	CUWParaDlg   m_page2;

	CDbgAction			 m_dbgActionPage;
	CDbgMotorSolenoid    m_dbgMotorPage;
	CDbgSensor			 m_dbgSensorPage;
	CDbgUdc			     m_dbgUdcPage;
	CStdUdcTest			 m_dbgStdUdc;
	CMoveVisioDlg        m_dbgMVD;
	CDbgMicroSecope      m_dbgMicroscopePage;
	CDbgCharacterBarcode m_dbgCharBarcodePage;
	CDbgHardwareparam    m_dbgHardwareParamPage;
	CPicImageArgvDlg	 m_dbgPicImagePage;
	CSoftwareSetUI		 m_dbgSetUIPage;
	CMotorConfPage		 m_dbgMotorConfPage;
	int m_mode;
	int   nCurPage;       
	bool  m_IsResetLC;
	CList<CRect, CRect> m_listRect;
	void InitChildWnd(CWnd*);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	CRect GetPageMaxSize(int ,int);
	void OnRecvLCReseting(void *arg, void *cbarg);
	int GetSystem();
	BOOL IsFileVersionWindows10OrGreater();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void SetDlgMode(int);
};
