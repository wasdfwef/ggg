#pragma once
#include "resource.h"
#include <map>
#include "afxwin.h"

// CDbgAction 对话框

class CDbgAction : public CPropertyPage
{
	DECLARE_DYNAMIC(CDbgAction)

public:
	CDbgAction();
	virtual ~CDbgAction();

// 对话框数据
	enum { IDD = IDD_DLG_DBG_ACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	CRect oldRect;
	DECLARE_MESSAGE_MAP()

private:
	std::map<int, void*> m_mapEventDelegate;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSysReset();
	afx_msg void OnBnClickedAspirationReset();
	afx_msg void OnBnClickedSamplingTranslationReset();
	afx_msg void OnBnClickedSamplingUpdownReset();
	afx_msg void OnBnClickedToCleanPostion();
	afx_msg void OnBnClickedToAsoirationPostion();
	afx_msg void OnBnClickedToEmergencyPostion();
	afx_msg void OnBnClickedAspiration();
	afx_msg void OnBnClickedSampling();
	afx_msg void OnBnClickedMicroscopeReset();
	afx_msg void OnBnClickedXAxisReset();
	afx_msg void OnBnClickedYAxisABChanReset();
	afx_msg void OnBnClickedYAxisCDChanReset();
	afx_msg void OnBnClickedZAxisReset();
	afx_msg void OnBnClickedToM10Lens();
	afx_msg void OnBnClickedToM40Lens();
	afx_msg void OnBnClickedPipeReset();
	afx_msg void OnBnClickedPipeInReset();
	afx_msg void OnBnClickedPipeOutReset();
	afx_msg void OnBnClickedStepLeftReset();
	afx_msg void OnBnClickedStepRightReset();
	afx_msg void OnBnClickedPipeIn();
	afx_msg void OnBnClickedPipeOut();
	afx_msg void OnBnClickedStep();
	afx_msg void OnBnClickedSelectPaperReset();
	afx_msg void OnBnClickedFeedPaperReset();
	afx_msg void OnBnClickedSelectPaper();
	afx_msg void OnBnClickedFeedPaperToSamplePos();
	afx_msg void OnBnClickedFeedPaperStepOne();
	afx_msg void OnBnClickedButtonClean();
	afx_msg void OnBnClickedStrengClean();
	afx_msg void OnBnClickedBaoyangClean();
	afx_msg void OnBnClickedToPointSampleInitPos();
	afx_msg void OnBnClickedTubeRotateReset();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ReChildWndsize();
	afx_msg void OnBnClickedButton36();
	afx_msg void OnBnClickedButton37();
	afx_msg void OnBnClickedButton38();
	afx_msg void OnBnClickedButton39();
	afx_msg void OnBnClickedButton40();
	afx_msg void OnBnClickedButton41();
	afx_msg void OnBnClickedButtonGetZhuiguang();
	afx_msg void OnBnClickedButtonGetZhuiguang2Clear();

	void OnRecvPhysicsSgSensorResult(void *arg, void *cbarg);
	afx_msg LRESULT  RecvPhysicsSgSensorResultHandler(WPARAM w, LPARAM l);
	afx_msg void OnDestroy();
	CStatic m_static_sg_result;

	void bEnableSGBtn(bool bEnableBtn = true);
	afx_msg void OnBnClickedButton34();
	afx_msg void OnBnClickedButton42();
	afx_msg void OnBnClickedButton43();
	afx_msg void OnBnClickedButton44();
	afx_msg void OnBnClickedButton45();
};
