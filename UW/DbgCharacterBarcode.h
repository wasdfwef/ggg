#pragma once
#include "resource.h"

// CDbgCharacterBarcode 对话框

class CDbgCharacterBarcode : public CPropertyPage
{
	DECLARE_DYNAMIC(CDbgCharacterBarcode)

public:
	CDbgCharacterBarcode();
	virtual ~CDbgCharacterBarcode();

// 对话框数据
	enum { IDD = IDD_DLG_DBG_CHARACTER_BARCODE_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLedTurnOn();
	afx_msg void OnBnClickedLedTurnOff();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
