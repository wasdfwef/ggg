#pragma once

#include <vector>
using namespace std;

// CPicPreviewWnd 对话框

class CPicPreviewWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CPicPreviewWnd)

private:
	vector<CString>& m_vecImages;
	CString m_path;
	int m_nCurIndex;

public:
	CPicPreviewWnd(vector<CString>& vecImages, CString &path, int nCurIndex, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPicPreviewWnd();

// 对话框数据
	enum { IDD = IDD_DIALOG_PREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	void DrawPic(HDC hDc);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
