#pragma once
#include "resource.h"
#include <map>

// CDbgSensor 对话框

class CDbgSensor : public CPropertyPage
{
	DECLARE_DYNAMIC(CDbgSensor)

private:
	std::map<int, void*> m_mapEventDelegate;

public:
	CDbgSensor();
	virtual ~CDbgSensor();

// 对话框数据
	enum { IDD = IDD_DLG_DBG_SENSOR_TEST };

public:
	void OnRecvSensorTestData(void *arg, void *cbarg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSensorStartTest();
	afx_msg void OnBnClickedSensorStopTest();
	afx_msg void OnBnClickedReadSensorConfig();
	afx_msg void OnBnClickedWirteSensorConfig();
	afx_msg LRESULT RecvSensorTestDataHandler(WPARAM w, LPARAM l);
	afx_msg void OnDestroy();
	afx_msg void OnEnChangeEdit10();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ReChildWndsize();

private:
	void Init();
};
