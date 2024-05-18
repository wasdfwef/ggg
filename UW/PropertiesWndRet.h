/*****************************************************************************
Copyright (C), 1995-2010, Chongqing Tianhai Medical Equipment Co., Ltd.
文件名:		PropertiesWndRet.h
功能:		病人检测结果显示
时间:		2010-05
*******************************************************************************/
#pragma once
//#include "PropertiesWndRetRes.h"
// CPropertyGridCtrlRet
//#include "Include\\Common\\Tree.h"

//using namespace Common;
#include "ListGroupView.h"


typedef struct _PCR_ACR
{
	char pro[64];
	char cre[64];
	char mca[64];
}PCR_ACR;

#define  SHOW_US_TYPE  0x1
#define  SHOW_UDC_TYPE 0X2
#define  SHOW_USER_TYPE 0X4

#define PRO_G	(3)
#define CRE_G	(12)
#define MCA_G	(11)

class CPropertyGridCtrlRet : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyGridCtrlRet)

	void   ShowUSData(TASK_INFO &task_info);
	void   ShowUDCData(TASK_INFO &task_info);

public:
	CPropertyGridCtrlRet();
	virtual ~CPropertyGridCtrlRet();
	virtual int OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const;

protected:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()

private:
	//Common::Recordset m_Rs; //数据集
	unsigned int m_nMaxData;//
	BOOL m_bStringLenLegit; //

public:
	int UdcOrderByYouyuan(int);
	void InitList();       //初始化属性列表
	void ShowData(TASK_INFO  & task_info, int nShowType);//显示数据
	void ShowRedXW(TASK_INFO & task_info);
	void ShowAcrPcr(PCR_ACR);
	char* GetIntByUDCGrade(int, char*);
	void TcharToChar(const TCHAR * tchar, char * _char);
	void SetDefaultValue(int nTestType = 3);
	void ChangeUSCell(PMODIFY_CELL_INFO  pModifyCellInfo, BOOL bAdd);
	int curTaskID;
	//int Warming(void);
    //void SetResultToDb(int iID,int nRet);

private:
	void ConfigList();     //配置属性列表
	//限制字符串长度
	void CheckStringLen(CMFCPropertyGridProperty *pProp, unsigned int nMaxLen = 32);
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

// CPropertiesWndRet

class CPropertiesWndRet : public CDockablePane
{
	DECLARE_DYNAMIC(CPropertiesWndRet)

public:
	CPropertiesWndRet();
	virtual ~CPropertiesWndRet();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);
	void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	CPropertyGridCtrlRet m_WndPropList;//属性列表

public:
	//初始化属性列表
	void InitPropList();
	//显示数据
//	void ShowData(int iID); 



private:
	//调整显示
	void AdjustLayout();
	void ShowRatioRed(CString &str);
	

};