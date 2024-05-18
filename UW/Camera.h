#ifndef  _CAMERA_INCLUDE
#define  _CAMERA_INCLUDE
#include "MyWaitCursor.h"
#include <string>

class CMicroscopeCameraView : public CView
{
	DECLARE_DYNCREATE(CMicroscopeCameraView)

private:
	BOOL   m_bPrv;
	
	CFont   m_font;
	CButton	m_btnFocus;

	BOOL m_bInMicConfig;
	BOOL m_IsMicSetting;

	CMenu  menu_usmic;

	std::string m_strIniFocus;
	bool m_bInAdjustFocusMode;


	BOOL   InteralVideoPrview(BOOL bPrv);
	CMyWaitCursor wait;
public:
	bool noResetZMode;
	CMicroscopeCameraView();
	BOOL   SetVideoPrview(BOOL bPrv);
	VOID   ControlUSSample(BOOL bPause);
protected:

	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFoucs();

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateMicConfig(CCmdUI *pCmdUI);
	afx_msg void OnMicConfig(UINT nID);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

public:
	// 开始显微镜设置
	bool StartUsMicConfig();
	void EndUsMicConfig();
	void SaveZPosToIni(CZPosType zPostype,int nPos);

	afx_msg void OnDestroy();
	afx_msg void OnFocusBegin();
	afx_msg void OnFocusEnd();
	afx_msg void OnFocusSaveZ();
	static DWORD ResetZAdjustFun(LPVOID param);
	afx_msg LRESULT OnFocusBegin(WPARAM, LPARAM);
	afx_msg LRESULT OnFocusEnd(WPARAM,LPARAM);
	afx_msg LRESULT OnFocusSaveZ(WPARAM,LPARAM);
	afx_msg void OnFocusResetZ();
	afx_msg LRESULT OnReceiveLight(WPARAM, LPARAM);
	afx_msg LRESULT OnShowLight(WPARAM, LPARAM);
	LRESULT OnFocusResetZ(WPARAM, LPARAM);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int todayListWidth;
};



class CCharactorCameraView : public CView
{
	DECLARE_DYNCREATE(CCharactorCameraView)

private:
	BOOL   m_bPrv;

	CFont   m_font;
	CButton	m_btnFocus;

	BOOL m_bInMicConfig;
	BOOL m_IsMicSetting;

	CMenu  menu_usmic;

	BOOL   InteralVideoPrview(BOOL bPrv);

public:
	CCharactorCameraView();
	BOOL   SetVideoPrview(BOOL bPrv);

protected:

	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFoucs();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateMicConfig(CCmdUI *pCmdUI);
	afx_msg void OnMicConfig(UINT nID);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

public:
	// 开始显微镜设置
	void StartUsMicConfig();
	void EndUsMicConfig();
	void SaveZPosToIni(CZPosType zPostype, int nPos);

};


#endif