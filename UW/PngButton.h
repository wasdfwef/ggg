
#pragma once
#include "afxwin.h"

// CPngButton


class CPngButton : public CButton
{
	//功能:加載透明PNG設置半透明的按鈕類
	//注:如出現貼膜顏色為黑色情況請檢查是否有SetColor因為m_BaseColor初始化為RGB(0,0,0)
public:
	CPngButton & CPngButton::operator = (const CPngButton & RightSides)//添加运算符=
	{
		m_BaseColor = RightSides.m_BaseColor;
		m_FontColor = RightSides.m_FontColor;
		m_bFlagIsFirst = RightSides.m_bFlagIsFirst;
		m_bFlagIsColorSet = RightSides.m_bFlagIsColorSet;
		m_iOnTransparent = RightSides.m_iOnTransparent;
		m_iDownTransparent = RightSides.m_iDownTransparent;
		m_bFlagIsTextSet = RightSides.m_iDownTransparent;
		m_bFlagIsImageSet = RightSides.m_iDownTransparent;
		m_iState = RightSides.m_iState;
		m_bMouseOnButton = RightSides.m_bMouseOnButton;
		m_bIsPressed = RightSides.m_bIsPressed;
		m_bIsFocused = RightSides.m_bIsFocused;
		m_bFlagContinueShow = RightSides.m_bFlagContinueShow;
		return *this;
	}
	CPngButton(const CPngButton &rhs)
	{
		m_BaseColor = rhs.m_BaseColor;
		m_FontColor = rhs.m_FontColor;
		m_bFlagIsFirst = rhs.m_bFlagIsFirst;
		m_bFlagIsColorSet = rhs.m_bFlagIsColorSet;
		m_iOnTransparent = rhs.m_iOnTransparent;
		m_iDownTransparent = rhs.m_iDownTransparent;
		m_bFlagIsTextSet = rhs.m_iDownTransparent;
		m_bFlagIsImageSet = rhs.m_iDownTransparent;
		m_iState = rhs.m_iState;
		m_bMouseOnButton = rhs.m_bMouseOnButton;
		m_bIsPressed = rhs.m_bIsPressed;
		m_bIsFocused = rhs.m_bIsFocused;
		m_bFlagContinueShow = rhs.m_bFlagContinueShow;
	}
	//-------------------------------------------------------------------
	//函數名SetColor
	//功能:設置觸摸按鍵的半透明顏色
	//參數:COLORREF color - 按鈕觸碰后的背景色
	// UINT OnTransparent = 0x0F - 鼠標懸浮時膜的透明度
	// UINT DownTransparent = 0x3F - 按鈕按下時膜的透明度
	//返回值:判斷輸入值是否合理
	//注:貼膜專用,OnTransparent和DownTransparent正常範圍0 - FF,值越小越透明
	//-------------------------------------------------------------------
	BOOL SetTransparentColor(COLORREF color, UINT OnTransparent = 0x0F, UINT DownTransparent = 0x3F);
	//設置字體顏色
	void SetFontColor(COLORREF color);
	//-------------------------------------------------------------------
	//函數名:SetTitle
	//功能:設置按鍵的標題
	//參數:CString csTitle - 按鍵顯示的圖片信息
	//返回值:判斷是否設置成功
	//-------------------------------------------------------------------
	BOOL SetTitle(const CString &csTitle);
	//-------------------------------------------------------------------
	//函數名SetImagePng
	//功能:設置插入PNG背景資源
	//參數:UINT nIDNormal - 一般情況背景圖片資源的ID
	// :UINT nIDOn - 鼠標懸浮情況的圖片資源的ID
	// :UINT nIDDown - 按鈕按下情況的圖片資源的ID
	// :UINT nIDDisable =0 - 按鈕失能情況的圖片資源的ID
	//返回值:無
	//注:SetImagePng(UINT nIDNormal,UINT nIDDisable =0);自動設置為貼膜模式
	// SetImagePng(UINT nIDNormal,UINT nIDOn,UINT nIDDown,UINT nIDDisable =0);自動設置為貼圖模式
	//-------------------------------------------------------------------
	void SetImagePng(UINT nIDNormal, UINT nIDDisable = 0);
	void SetImagePng(const CString& PathNormal, const CString& PathDisable = _T(""));
	void SetImagePng(UINT nIDNormal, UINT nIDOn, UINT nIDDown, UINT nIDDisable = 0);
	void SetImagePng(const CString& PathNormal, const CString& PathOn, const CString& PathDown, const CString& PathDisable = _T(""));
	//-------------------------------------------------------------------
	//函數名SaveBG
	//功能:更新控件擋住是背景,改變父窗口背景后需調用一次
	//參數:無
	//返回值:無
	//-------------------------------------------------------------------
	void SaveBG(void);
	//-------------------------------------------------------------------
	//函數名SizeToContent
	//功能:使按鈕大小和PNG資源大小一致
	//參數:無
	//返回值:無
	//-------------------------------------------------------------------
	void SizeToContent(void);
	//-------------------------------------------------------------------
	//函數名SetSize
	//功能:自定義設置控件大小
	//參數:int iWidth - 設置控件寬度
	// :int iHeight - 設置控件高度
	//返回值:無
	//-------------------------------------------------------------------
	void SetSize(int iWidth, int iHeight);
	//-------------------------------------------------------------------
	//函數名SetShowState
	//功能:設置按鈕風格
	//參數:BOOL NeedColorSet - 設置是否顯示貼膜
	// :BOOL NeedTextSet - 設置是否打印文字
	// :BOOL NeedImageSet - 設置是否打印PNG圖片
	//返回值:無
	//-------------------------------------------------------------------
	BOOL SetShowState(BOOL NeedColorSet, BOOL NeedTextSet, BOOL NeedImageSet);
	//-------------------------------------------------------------------
	//函數名SetContinueShow
	//功能:設置按鈕是否需要一直顯示為按下狀態
	//參數:BOOL ContinueShow - 是否顯示為按下狀態
	//返回值:無
	//-------------------------------------------------------------------
	void SetContinueShow(BOOL ContinueShow);
	//********************************************************************
	DECLARE_DYNAMIC(CPngButton)
protected:
	enum ButtonState
	{
		Disable = 0, Normal, On, Down
	};

	enum ButtonMode
	{
		MaskMode = 0, PictureMode
	};
public:
	CPngButton();
	virtual ~CPngButton();
protected:
	void LoadImage(CImage &image, UINT nID);
	BOOL LoadImage(CImage &image, const CString& Path);
	CImage m_image[4];
	void Draw(UINT nID, CDC* pDC, CRect rect);//畫PNG貼圖
protected:
	DECLARE_MESSAGE_MAP()
	void CancelHover(void);//修改鼠標不在懸浮于控件上的標誌
protected:
	CDC m_MemDCBG;//保持的父窗口擋住的背景
	COLORREF m_BaseColor;//半透明貼膜的顏色
	COLORREF m_FontColor;//字體的顏色

	BOOL m_bFlagIsFirst;//是否為第一次取得父窗口背景的標誌位

	BOOL m_bFlagIsColorSet;//是否需要貼膜
	int m_iOnTransparent;//鼠標懸浮在按鈕上時半透明貼膜的透明度
	int m_iDownTransparent;//鼠標按下是半透明貼膜的透明度

	BOOL m_bFlagIsTextSet;//是否需要打印出文字
	BOOL m_bFlagIsImageSet;//是否需要打印圖片
	//CFont m_font;
	//LOGFONT m_lf;
	//int m_iTitleSize;//按鈕文字的大小

	int m_iState;//按鈕的風格(0為採用半透明貼膜效果,1為使用3張以上PNG資源來區分狀態)
protected:
	BOOL m_bMouseOnButton;//鼠標是否在按鈕上
	BOOL m_bIsPressed;//按鍵是否按下
	BOOL m_bIsFocused;//按鍵是否得到焦點
	BOOL m_bFlagContinueShow;//這個為TRUE時按鈕的繪圖會一直保持在按下狀態
protected:
	virtual void PreSubclassWindow();//設置按鈕分割為重繪

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);//重繪按鈕圖案

	//一些判斷按鈕狀態的系統函數
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);//防止擦灰背景
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);//鼠標移動到上面時判斷狀態
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);//鼠標離開時消息判斷狀態
	afx_msg void OnKillFocus(CWnd* pNewWnd);//失去焦點時判斷狀態
};