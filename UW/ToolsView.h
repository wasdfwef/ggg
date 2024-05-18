#pragma once
#include "PngButton.h"
#include <vector>
// CToolsView 视图

#define IDC_COMBOBOX 0x1101
#define IDC_BUTTONSURE 0x1102
#define IDC_COMBOXSTATIC 0x1103
#define IDC_EDITNUM 0x1104
#define IDC_NUMSTATIC 0x1105
#define IDC_QCSTATIC 0x1106
#define IDC_COMBOBOXUDCITEM 0x1107
#define IDC_COMBOXSTATICUDCITEM 0x1109
#define IDC_BUTTON_CLEAN		0x110A
#define IDC_BUTTON_STRONGCLEAN	0x110B

class CToolsView : public CView
{
	DECLARE_DYNCREATE(CToolsView)

public:
	CToolsView();           // 动态创建所使用的受保护的构造函数
	virtual ~CToolsView();
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedUsermgr();
	afx_msg void OnBnClickedHospital();
	afx_msg void OnBnClickedserialport();
	afx_msg void OnBnClickedDisplay();
	afx_msg void OnBnClickedcolor();
	afx_msg void OnBnClickedtemp();
	afx_msg void OnBnClickedset();
	afx_msg void OnBnClickedsure();
	afx_msg void OnBnClickedSetTestType();
	afx_msg void OnBnClickedSetSave();
	afx_msg void OnBnClickedClean();
	afx_msg void OnBnClickedStrongClean();
	afx_msg void OnComBoxChanged();
	afx_msg void OnEditKillFocus();
	afx_msg void OnChangeTestType(CCmdUI *pCmdUI);
	void set_font(int height, CFont *font, LPCWSTR name);
	void ChangeUdcItem(int);
	void ChangePriTemp(int);
	void reControlSize(void);
	int  InsertPngBtn();
	std::vector<CPngButton> btnVector;
	std::vector<int> btnPngVector;
	std::vector<int> btnIdVector;
	std::vector<CString> tipStrVector;
	std::vector<CRect> btnRectVector;
	CComboBox m_comboxUdcItem;
	CStatic m_comboxStaticUdcItem;
	CComboBox m_combox;
	CStatic m_comboxStatic;
	CButton m_btnSetTestType;
	CButton m_btnSet;
	CStatic m_numStatic;
	CEdit m_editNum;
	CPngButton btn;
	CFont my_font;
	CFont my_font_QC;
	CPngButton textQCTip;
	bool isToolView;
	bool is2020;
	CButton m_btnSure;
	CToolTipCtrl m_tipCtrl;
	int btnPng;
};


