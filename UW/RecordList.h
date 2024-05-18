#pragma once


// CRecordList

class CRecordList : public CDockablePane
{
	DECLARE_DYNAMIC(CRecordList)

public:
	CRecordList();
	virtual ~CRecordList();
public:
	CListCtrl          m_recordList;

protected:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDBLClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateOperate(CCmdUI *pCmdUI);
	afx_msg void OnOperate(UINT nID);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	// 初始化列表
	void InitList(void);
	void LookRecordInfo();
	void UpdateRecordList(void);
	void Delete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


