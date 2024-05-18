#ifndef  _UWVIEW_INCLUDE
#define  _UWVIEW_INCLUDE
// UWView.h : CUWView 类的接口
//
#include "Camera.h"
#include "UdcView.h"
#include "UWDoc.h"

class CUWView : public CTabView
{
protected: // 仅从序列化创建
	CUWView();
	DECLARE_DYNCREATE(CUWView)

	

// 属性
public:
	CUWDoc* GetDocument() const;

// 操作
public:
	void    InitShowCellView();

   CMicroscopeCameraView     *m_pMicroscopeCamera; // 显微镜相机
   CCharactorCameraView      *m_pCharacterCamera;  // 性状相机
   CUdcView    *m_pUdcView;
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(CView* pActivateView);

// 实现
public:
	virtual ~CUWView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	int           m_iUpwardTab; // 指向上一个视图
public:
	// 设置激活的视图
	void SetActiveTab(int nTab);
	// 得到激活的视图
	int GetActiveTab();
	// 切换到上一个视图
	void UpwardTab(void);

	CWnd *GetTabWnd(int nTab);

public:
//	afx_msg void OnMenuSet();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // UWView.cpp 中的调试版本
inline CUWDoc* CUWView::GetDocument() const
   { return reinterpret_cast<CUWDoc*>(m_pDocument); }
#endif

#endif