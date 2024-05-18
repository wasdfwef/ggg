#include "stdafx.h"
#include "WndBtnTools.h"
#include "resource.h"
#include "DbgMsg.h"


#define ShowColor RGB(255,255,255)

WndBtnTools::WndBtnTools()
{
}


WndBtnTools::~WndBtnTools()
{

}
BEGIN_MESSAGE_MAP(WndBtnTools, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
/*	ON_UPDATE_COMMAND_UI(IDC_BTN_SEARCH,OnUpdateCommandUI)*/

END_MESSAGE_MAP()


int WndBtnTools::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  在此添加您专用的创建代码
	myView = new CToolsView;
	CRect   rect;
	GetClientRect(&rect);
	if (myView)
	{
		myView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, AFX_IDW_PANE_LAST);
	}

	return 0;
}


void WndBtnTools::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	myView->MoveWindow(0,0,cx,cy);
	// TODO:  在此处添加消息处理程序代码
}



void WndBtnTools::OnUpdateCommandUI(CCmdUI *cmdUI)
{

}

void WndBtnTools::InitRes(void)
{
	TCHAR INIPath[256] = { 0 };

	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("config\\Lis1.ini"));
	//if (!myView->is2020)
		myView->InsertPngBtn();
}





BOOL WndBtnTools::DestroyWindow()
{
	// TODO:  在此添加专用代码和/或调用基类
	if (myView)
	{
		myView->DestroyWindow();
	}
	return CDockablePane::DestroyWindow();
}

int WndBtnTools::InsertBtn(CPngButton& newBtn, int newBtnPng,int newBtnId, CString newTipStr,CRect newRect)
{
	myView->btnVector.push_back(newBtn);
	myView->btnPngVector.push_back(newBtnPng);
	myView->btnIdVector.push_back(newBtnId);
	myView->tipStrVector.push_back(newTipStr);
	myView->btnRectVector.push_back(newRect);
	return 0;
}