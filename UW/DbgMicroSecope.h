#pragma once
#include "resource.h"
#include "CmdHelper.h"
#include <string>

// CDbgMicroSecope 对话框

class CDbgMicroSecope : public CPropertyPage
{
	DECLARE_DYNAMIC(CDbgMicroSecope)

public:
	CDbgMicroSecope();
	virtual ~CDbgMicroSecope();

// 对话框数据
	enum { IDD = IDD_DLG_DBG_MICROSCOPE };




private:
	COUNT_POOL_CHANNEL m_channel;
	MICROSCOPE_LENS m_mlens;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	bool SetCameraPreview(int nCameraIndex);

	std::string m_strIniFocus;
	bool m_bInAdjustFocusMode;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLedTurnOn();
	afx_msg void OnBnClickedLedTurnOff();
	afx_msg void OnBnClickedChangeChannel();
	afx_msg void OnBnClickedChangeCaptureImageArea();
	afx_msg void OnBnClickedChannelA();
	afx_msg void OnBnClickedChannelB();
	afx_msg void OnBnClickedOnBnClickedChannelC();
	afx_msg void OnBnClickedChannelD();
	afx_msg void OnBnClickedM10Lens();
	afx_msg void OnBnClickedM40Lens();
	afx_msg void OnBnClickedFocusOption();
	afx_msg void OnBnClickedSaveZAxisPos();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
