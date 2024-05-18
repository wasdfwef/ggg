#pragma once
#include "resource.h"
#include "DRBCImageView.h"
// CDRBCImage 对话框

class CDRBCImage : public CDialogEx
{
	DECLARE_DYNAMIC(CDRBCImage)

public:
	CDRBCImage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDRBCImage();

// 对话框数据
	enum { IDD = IDD_DIALOG_DRBCIMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CDRBCImageView *pView;
	DECLARE_MESSAGE_MAP()
	
public:
	void GetTaskID(int);
	CStatic *showImage;
	char dRBCImagePath[256];
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void DrawPictureFromBmpFile(CDialog * dlg, int pictureCtrlID, const char * fileName);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
};
