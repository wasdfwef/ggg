/*****************************************************************************
Copyright (C), 1995-2010, Chongqing Tianhai Medical Equipment Co., Ltd.
文件名:		PropertiesWndPat.h
功能:		病人信息显示
时间:		2010-05
*******************************************************************************/
#pragma once
//#include "PropertiesWndPatRes.h"
#include "..\..\..\inc\THDBUW_Access.h"
// CPropertyGridCtrlPat

class CPropertyGridCtrlPat : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyGridCtrlPat)

public:
	CPropertyGridCtrlPat();
	virtual ~CPropertyGridCtrlPat();
	virtual int OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const;
	

protected:
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()


	

private:
//	Common::Recordset m_Rs;      //数据集
	     //属性列表项附带数据的最大值
	BOOL m_bStringLenLegit;      //字符串长度是否超过限制
	

public:
	unsigned int m_nMaxData;

	
	
	//初始化属性列表
	void InitList();   
	//显示数据
	void ShowData(TASK_INFO  &task_info);        
	//保存数据
	void SaveData();                 
	//根据快捷键设置属性列表的值
	void ResetValueByKey(LPCTSTR pPropName, LPCTSTR pPropValue);
	//根据快捷键设置除医生以外属性列表的值
	//void ResetOtherValueByKey(LPCTSTR pPropName, LPCTSTR pPropValue);
	//根据快捷键设置医生属性列表的值
	void ResetDepartmentValueByKey(LPCTSTR pPropValue);
	void ResetHospitalValueByKey(LPCTSTR pPropValue);
	void ResetDoctorValueByKey(LPCTSTR pPropValue);


	void SetHospitalOption();
	//重新设置除医生以外属性列表的选项
	void ResetOtherOption();
	

	void clearOption();
	void clearDoctorOption();
	//重新设置医生属性列表的选项
	//void ResetDoctorOption();    
	// 重新设置用户属性列表的选项
	void ResetUserOption();
	void SetDefaultValue();

	void ResetColorOption();
	void ResetClearOption();
	void ResetDiagnosOption();

private:
	//配置属性列表
	void ConfigList();       
	//根据住院号设置属性列表的值
	//void ResetValueByHospitalNum();
	//限制字符串长度
	void CheckStringLen(CMFCPropertyGridProperty *pProp, unsigned int nMaxLen = 32);

	
};

// CPropertiesWndPat

class CPropertiesWndPat : public CDockablePane
{
	DECLARE_DYNAMIC(CPropertiesWndPat)

public:
	CPropertiesWndPat();
	virtual ~CPropertiesWndPat();

	
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);

	DECLARE_MESSAGE_MAP()

public:
	CPropertyGridCtrlPat m_WndPropList;//属性列表

public:
	//初始化属性列表
	void InitPropList();
	//重新设置属性列表
	void ResetOtherOption();
	// 重新设置用户属性列表的选项
	void ResetUserOption();
	void clearOption();
	void clearDoctorOption();
	int  GetSex(CMFCPropertyGridProperty *pSex);
	int  AgeUnit(CMFCPropertyGridProperty *pAgeUnit);
//	BOOL CheckNum(CMFCPropertyGridProperty *pSN, int &nNum, int nDefaultNum);
	void CopyStr(CMFCPropertyGridProperty *pSrc, PCHAR  pDst);



	//bool Warming(void);

private:
	//调整显示
	void AdjustLayout();

public:
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
};