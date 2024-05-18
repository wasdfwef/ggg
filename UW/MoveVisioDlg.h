#pragma once
#include "afxwin.h"
#include <string>

// CMoveVisioDlg 对话框

#define  MAX_40X_LINES   (20)
#define  MAX_40X_COLUMN  (6)

#define  MAX_10X_LINES   (8)
#define  MAX_10X_COLUMN  (3)

#define  MAX_VISIO_ITEMCOUNTS (256)

typedef struct UW2000_VISIO_MOVEDLG_RECT
{
	int   m_nSelected[MAX_VISIO_ITEMCOUNTS];
	int   m_nIndex[MAX_VISIO_ITEMCOUNTS]; 
	BOOL  m_bIsDefault[MAX_VISIO_ITEMCOUNTS];
	CRect m_VisioRect[MAX_VISIO_ITEMCOUNTS];
} UW2000_VISIO_MOVEDLG_RECT;

typedef struct UW2000_RECT_INFO
{
    int m_nType; // 0 = 10x 1 = 40x
	int m_nCounts;
	int m_height;
	int m_width;
	int m_Lines;
	int m_Column;
	int m_Max_Lines;
	int m_Max_Column;
	CRect m_window_rect;
	CRect m_client_rect;
	UW2000_VISIO_MOVEDLG_RECT m_visio_info;
	UW2000_VISIO_MOVEDLG_RECT m_window_visio_info;
} UW2000_RECT_INFO;

typedef struct UW2000_SELECTED_POS
{
	int nLines;
	int nColumn;
}UW2000_SELECTED_POS;

class CMoveVisioDlg : public CPropertyPage //public CDialog
{
	DECLARE_DYNAMIC(CMoveVisioDlg)

public:
	CMoveVisioDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMoveVisioDlg();

// 对话框数据
	enum { IDD = IDD_MOVEVISIO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_10x_static;
	CStatic m_40x_static;
	virtual BOOL OnInitDialog();

private:

	int  m_nTotialStep;
	int  m_YTotalStep;
	int  m_XTotalStep;
	int  m_10xIndex;
	UW2000_SELECTED_POS m_selected_10x_pos;
	int  m_40xIndex;
	UW2000_SELECTED_POS m_selected_40x_pos;

	CZPosType    m_ZPosType;	
	
	int  m_nChannelNums;
	UW2000_RECT_INFO  m_10x_rect_info;
	UW2000_RECT_INFO  m_40x_rect_info;
	CToolTipCtrl m_tooltipctrl;

	void InitializeCtrls(void);
	void InitializeRect(UW2000_RECT_INFO* pinfo);
	void Clear();
	void DrawVisioRgn(int nID);
	void DrawIndex(HDC hdc,UW2000_RECT_INFO info);
	void Send10xCommandToMachine();
	void Send40xCommandToMachine();
	void SetBtnsStatus(bool bIsEnable);
	bool LocateCameraTo10X();
	bool LocateCameraTo40X();
	
public:
	afx_msg void OnBnClickedUpBtn();
	afx_msg void OnBnClickedDownBtn();
	afx_msg void OnBnClickedRightBtn();
	afx_msg void OnBnClickedMoreRightBtn();
	afx_msg void OnBnClickedLeftBtn();
	afx_msg void OnBnClickedMoreLeftBtn();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedChannelaBtn();
	afx_msg void OnBnClickedChannelbBtn();
	afx_msg void OnBnClickedChannelcBtn();
	afx_msg void OnBnClickedChanneldBtn();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CStatic m_err_info_static;
	CButton m_more_right_btn;
	CButton m_right_btn;
	CButton m_up_btn;
	CButton m_down_btn;
	CButton m_left_btn;
	CButton m_more_left_btn;
	CEdit m_set_edit;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSaveCameraPosBtn();
	afx_msg void OnBnClickedResetCameraButton();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClicked10xButton();
	afx_msg void OnBnClicked40xButton();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	CStatic m_static_notice;
	long m_nZPos;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedMoveZ();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnMoveto();

	void EnablePostBtns(BOOL bEnable);
	CButton m_btn_post_10X;
	CButton m_btn_post_40X;

private:
	CPoint m_oldPostion;
	std::string m_strIniFocus;
	bool m_bLedLightOn;
	bool m_bInAdjustFocusMode;
	int m_nMicroLens;

public:
	afx_msg void OnBnClickedBtnLedCtrl();
	afx_msg void OnBnClickedBtnAdjustFocus();
	afx_msg void OnBnClickedSaveZPos();
	afx_msg void OnBnClickedButtonAction1();
	afx_msg void OnBnClickedButtonAction2();
	afx_msg void OnBnClickedButtonAction3();
	afx_msg void OnBnClickedButtonAction4();
	afx_msg void OnBnClickedButtonAction5();
	afx_msg void OnBnClickedButtonAction6();
	afx_msg void OnBnClickedButtonAction7();
	afx_msg void OnEnChangeEditStepcount();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
