/*****************************************************************************
Copyright (C), 1995-2010, Chongqing Tianhai Medical Equipment Co., Ltd.
文件名:UdcView.h 
功能:  干化学检测并显示结果
时间:  2010-06
*******************************************************************************/
#pragma once
#include "MyListCtrl.h"

typedef struct positiveID
{
	int id;
	int active;

}positiveID;
// CUdcView 视图

class CUdcView : public CView
{
	DECLARE_DYNCREATE(CUdcView)

protected:
	CUdcView();           // 动态创建所使用的受保护的构造函数
	virtual ~CUdcView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnInitialUpdate();
	afx_msg void OnCustomList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnSetPaperType(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateSkinUI(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:

/*	CListCtrl m_List;//列表*/
	//Common::Recordset  m_Rs;//数据库集
	char m_UDCIni[MAX_PATH];
	

public:
	//CUdc *m_pUdc;           //干化学操作
	int m_nlistNum;
	COLORREF m_color;

public:
	//刷新数据
	//void RefurbishData();
	CPngButton m_BtnListskin;
	CToolTipCtrl m_tipCtrl;
	CMyListCtrl m_List;//列表
	int AddTaskToList(TASK_INFO &task_info, BOOL bInsert = TRUE);
	int AddTaskToList( int nID );
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	void DeleteItem(ULONG nTaskID);
	void FillList();
	int GetIndexByID(int);
	void UpdateListView();
	int m_nSelItem;
	int UdcOrderByYouyuan(int);
	void UpdateSearchRecordResult();

private:
	//初始化列表框
	void InitList();
	//从数据库中获得颜色,检测
	//void GetColorAndResult(unsigned int nID);

	afx_msg void OnBnClickedListSkin();

	void GetUDCSettingIniPath();
	int PushBacktoUDCPositive(positiveID ,int);
	
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//判断结果是否大于一个+
	bool IsResultGreateOnePlus(int nIndex);
};


