#pragma once

// CAdjustImg 对话框

class CAdjustImg : public CDialog
{
	DECLARE_DYNAMIC(CAdjustImg)

public:
	CAdjustImg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdjustImg();

// 对话框数据
	enum { IDD = IDD_ADJUSTIMG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	unsigned int m_nCha;
	unsigned int m_n10xIndex;
	unsigned int m_n40xIndex;
	int m_nNum;
	// 10x采图命令
	void Pic10x();
	// 40x采图命令
	void Pic40x();
public:
	//CDevice *m_pDevice;
	afx_msg void OnBnClickedAdjust10x1();
	afx_msg void OnBnClickedAdjust10x2();
	afx_msg void OnBnClickedAdjust10x3();
	afx_msg void OnBnClickedAdjust10x4();
	afx_msg void OnBnClickedAdjust10x5();
	afx_msg void OnBnClickedAdjust10x6();
	afx_msg void OnBnClickedAdjust10x7();
	afx_msg void OnBnClickedAdjust10x8();
	afx_msg void OnBnClickedAdjust10x9();
	afx_msg void OnBnClickedAdjust10x10();
	afx_msg void OnBnClickedAdjust40x1();
	afx_msg void OnBnClickedAdjust40x2();
	afx_msg void OnBnClickedAdjust40x3();
	afx_msg void OnBnClickedAdjust40x4();
	afx_msg void OnBnClickedAdjust40x5();
	afx_msg void OnBnClickedAdjust40x6();
	afx_msg void OnBnClickedAdjust40x7();
	afx_msg void OnBnClickedAdjust40x8();
	afx_msg void OnBnClickedAdjust40x9();
	afx_msg void OnBnClickedAdjust40x10();
	afx_msg void OnBnClickedAdjust40x11();
	afx_msg void OnBnClickedAdjust40x12();
	afx_msg void OnBnClickedAdjust40x13();
	afx_msg void OnBnClickedAdjust40x14();
	afx_msg void OnBnClickedAdjust40x15();
	afx_msg void OnBnClickedAdjust40x16();
	afx_msg void OnBnClickedMoveLeft();
	afx_msg void OnBnClickedMoveRight();
	afx_msg void OnBnClickedMoveFront();
	afx_msg void OnBnClickedMoveBack();
	afx_msg void OnBnClickedAjustimgA();
	afx_msg void OnBnClickedAjustimgB();
	afx_msg void OnBnClickedOk();
};
