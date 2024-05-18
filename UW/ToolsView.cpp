// ToolsView.cpp : 实现文件
//

#include "stdafx.h"
#include "ToolsView.h"
#include "resource.h"
#include "UW.h"
#include "DbgMsg.h"
#include "MainFrm.h"
#include "..\..\..\inc\CmdHelper.h"
#include <future>
#include <atomic>
#include "SetTestTypeDlg.h"

static std::atomic<bool> isCleaning = false;

// CToolsView

IMPLEMENT_DYNCREATE(CToolsView, CView)

CToolsView::CToolsView()
{
	is2020 = false;
	TCHAR INIPath[256] = { 0 };

	GetModuleFileName(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpec(INIPath);
	PathAppend(INIPath, _T("config\\Lis1.ini"));
	if (2020 == theApp.m_sendLisType)
		is2020 = true;
}

CToolsView::~CToolsView()
{
}

BEGIN_MESSAGE_MAP(CToolsView, CView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CToolsView::OnBnClickedSearch)//工具栏按钮处理函数
	ON_BN_CLICKED(IDC_BTN_USERMGR, &CToolsView::OnBnClickedUsermgr)
	ON_BN_CLICKED(IDC_BTN_HOSPTIAL, &CToolsView::OnBnClickedHospital)
	ON_BN_CLICKED(IDC_BTN_SERIALPORT, &CToolsView::OnBnClickedserialport)
	ON_BN_CLICKED(IDC_BTN_COLOR, &CToolsView::OnBnClickedcolor)
	ON_BN_CLICKED(IDC_BTN_TEMP, &CToolsView::OnBnClickedtemp)
	ON_BN_CLICKED(IDC_BTN_SET, &CToolsView::OnBnClickedset)
	ON_BN_CLICKED(IDC_BUTTONSURE, &CToolsView::OnBnClickedsure)
	ON_BN_CLICKED(IDC_BUTTONSURE + 1, &CToolsView::OnBnClickedSetTestType)
	ON_BN_CLICKED(IDC_BUTTONSURE + 2, &CToolsView::OnBnClickedSetSave)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CToolsView::OnBnClickedClean)
	ON_BN_CLICKED(IDC_BUTTON_STRONGCLEAN, &CToolsView::OnBnClickedStrongClean)
	ON_CBN_SELCHANGE(IDC_COMBOBOX, &CToolsView::OnComBoxChanged)
	ON_CBN_SELCHANGE(IDC_COMBOBOXUDCITEM, &CToolsView::OnComBoxChanged)
	ON_UPDATE_COMMAND_UI(IDC_COMBOBOX, &CToolsView::OnChangeTestType)
	ON_EN_KILLFOCUS(IDC_EDITNUM, &CToolsView::OnEditKillFocus)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CToolsView 绘图

void CToolsView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

void CToolsView::OnBnClickedSearch()
{
	((CMainFrame*)theApp.GetMainWnd())->m_wndSearchRecord.DoModal();
}

void CToolsView::OnBnClickedUsermgr()
{
	((CMainFrame*)theApp.GetMainWnd())->setting.OnBnClickedSetUsermanage();
/*	((CMainFrame*)theApp.GetMainWnd())->setting.SendMessage(WM_COMMAND, MAKEWPARAM(LOWORD(IDC_SET_USERMANAGE), HIWORD(BN_CLICKED)), NULL);*/
}

void CToolsView::OnBnClickedHospital()
{
	((CMainFrame*)theApp.GetMainWnd())->setting.OnBnClickedBtnDoctor();
}

void CToolsView::OnBnClickedserialport()
{
	((CMainFrame*)theApp.GetMainWnd())->setting.OnBnClickedButton10();
}

void CToolsView::OnBnClickedcolor()
{
	((CMainFrame*)theApp.GetMainWnd())->setting.OnBnClickedBtnColor();
}

void CToolsView::OnBnClickedtemp()
{
	((CMainFrame*)theApp.GetMainWnd())->setting.OnBnClickedButton12();
}

void CToolsView::OnBnClickedset()
{
	((CMainFrame*)theApp.GetMainWnd())->setdlg.DoModal();
}

void CToolsView::OnBnClickedsure()
{
	if (theApp.IsIdleStatus() == FALSE)
	{
		MessageBox(L"请等到检测完成后再变更测试类型",MB_OK);
		m_combox.SetWindowTextW(L"");
		return;
	}
	GETTESTINFO_CONTEXT ctx = { 0 };
	bool bSuccess;
	if (theApp.m_bLCResetSuccess)
	{
		BeginWaitCursor();
		WirteHardwareParamTestType(ctx, bSuccess);
		
		if (bSuccess)
		{
			MessageBox((bSuccess ? _T("更改测试类型成功!") : _T("更改测试类型失败!")), _T("提示"), MB_OK | (bSuccess ? MB_ICONINFORMATION : MB_ICONWARNING));
		}
		
	}
	theApp.UpdateCheckType();
	EndWaitCursor();
}

void CToolsView::OnBnClickedSetTestType()
{
	CSetTestTypeDlg dlg;
	dlg.DoModal();
}

void CToolsView::OnBnClickedSetSave()
{
	OnEditKillFocus();
	OnComBoxChanged();
}

void CToolsView::OnBnClickedClean()
{
	if (theApp.IsIdleStatus() == FALSE)
	{
		::MessageBox(GetSafeHwnd(), _T("所有标本检测完成后才可以进行日常清洗"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	
	if (isCleaning)
	{
		::MessageBox(GetSafeHwnd(), _T("正在清洗中,请稍后再试."), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	auto f1 = std::async(std::launch::async, [&]{
		//GetDlgItem(IDC_BUTTON_CLEAN)->EnableWindow(FALSE);
		//GetDlgItem(IDC_BUTTON_STRONGCLEAN)->EnableWindow(FALSE);
		isCleaning.exchange(true);
		Clean(1);
		isCleaning.exchange(false);
		//GetDlgItem(IDC_BUTTON_CLEAN)->EnableWindow(TRUE);
		//GetDlgItem(IDC_BUTTON_STRONGCLEAN)->EnableWindow(TRUE);
	});
}

void CToolsView::OnBnClickedStrongClean()
{
	if (theApp.IsIdleStatus() == FALSE)
	{
		::MessageBox(GetSafeHwnd(), _T("所有标本检测完成后才可以进行强化清洗"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	if (isCleaning)
	{
		::MessageBox(GetSafeHwnd(), _T("正在清洗中,请稍后再试."), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	auto f1 = std::async(std::launch::async, [&]{
		//GetDlgItem(IDC_BUTTON_CLEAN)->EnableWindow(FALSE);
		//GetDlgItem(IDC_BUTTON_STRONGCLEAN)->EnableWindow(FALSE);
		isCleaning.exchange(true);
		ForceClean(1);
		isCleaning.exchange(false);
		//GetDlgItem(IDC_BUTTON_CLEAN)->EnableWindow(TRUE);
		//GetDlgItem(IDC_BUTTON_STRONGCLEAN)->EnableWindow(TRUE);
	});
}

void CToolsView::OnComBoxChanged()
{
	if (theApp.IsIdleStatus() == FALSE)
	{
		MessageBox(L"请等到检测完成后再变更测试类型", MB_OK);
		goto backTestType;
	}
	DBG_MSG("正在变更测试类型");
	GETTESTINFO_CONTEXT ctx = { 0 };
	bool bSuccess = false;
/*	ctx.nUdcTestItemCount =  theApp.m_nUdcItemCount;*/
	switch (m_comboxUdcItem.GetCurSel())
	{
	case 0:ctx.nUdcTestItemCount = 10; break;
	case 1:ctx.nUdcTestItemCount = 11; break;
	case 2:ctx.nUdcTestItemCount = 12; break;
	case 3:ctx.nUdcTestItemCount = 13; break;
	case 4:ctx.nUdcTestItemCount = 14; break;
	default:
	{
		MessageBox(L"干化项目数选择无效,请重新选择", MB_OK);
		goto backTestType;
		break;
	}
	}
	switch (m_combox.GetCurSel())
	{
	case 0:ctx.nTestType = 3; break;
	case 1:ctx.nTestType = 1; break;
	case 2:ctx.nTestType = 2; break;
	default:
	{
		MessageBox(L"测试类型选择无效,请重新选择", MB_OK);
		goto backTestType;
		break;
	}
	}
	theApp.setTestType = true;
	DBG_MSG("1.debug:theApp.setTestType = %d,& = %p\n", theApp.setTestType, &theApp.setTestType);
	BeginWaitCursor();
	if (!WirteHardwareParamTestType(ctx, bSuccess) || !bSuccess)
	{
		EndWaitCursor();
		MessageBox(L"设置测试类型和干化项目数失败!", MB_OK);
		goto backTestType;
	}
	DBG_MSG("2.debug:theApp.setTestType = %d,& = %p\n", theApp.setTestType, &theApp.setTestType);
	theApp.UpdateCheckType();
	
	EndWaitCursor();
	DBG_MSG("5.debug:theApp.setTestType = %d,& = %p\n", theApp.setTestType, &theApp.setTestType);
	if (ctx.nUdcTestItemCount == theApp.m_nUdcItemCount && ctx.nTestType == theApp.GetCheckType())
	{
		//MessageBox(L"修改测试类型成功", MB_OK);
		ChangeUdcItem(theApp.m_nUdcItemCount);
		theApp.setTestType = false;
		return;
	}

	MessageBox(L"修改测试类型失败", MB_OK);

backTestType:
	switch (theApp.GetCheckType())
	{
	case 1:m_combox.SetCurSel(1); break;
	case 2:m_combox.SetCurSel(2); break;
	case 3:m_combox.SetCurSel(0); break;
	default:
		MessageBox(L"获取到未知的测试类型", MB_OK);
		break;
	}
		
	return;
}

void CToolsView::OnEditKillFocus()
{
	CString num;
	m_editNum.GetWindowTextW(num);
	if (_tstoi(num) == 0)
	{
		MessageBox(L"序号设置异常", MB_OK);
		num.Format(L"%d", theApp.m_nSN);
		m_editNum.SetWindowTextW(num);
	}
	else
	{
		//MessageBox(L"序号设置成功", MB_OK);
		theApp.m_nSN = _tstoi(num);
	}
}

void CToolsView::ChangeUdcItem(int udcItem)
{
	//改变UDC测试项目数后的连带操作
	CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
	CUdcView* udcView = (CUdcView*)pMain->GetUWView()->GetTabWnd(THUM_UDC_TAB);
	for (int i = 0; i < udcItem; i++)
	{
		pMain->m_wndResult.m_WndPropList.GetProperty(1)->GetSubItem(i)->Show(TRUE);	
		udcView->m_List.SetColumnWidth(i+2, 65);//干化列表列宽度
	}

	for (int j = udcItem; j < theApp.udc_cfg_info.nUdcCounts; j++)
	{
		pMain->m_wndResult.m_WndPropList.GetProperty(1)->GetSubItem(j)->Show(FALSE);
		udcView->m_List.SetColumnWidth(j + 2,0);//干化列表列宽度
	}

	if (udcItem < 13)//如果没有肌酐、微量白蛋白测试项，则隐藏PCR ACR
	{
		pMain->m_wndResult.m_WndPropList.GetProperty(4)->Show(FALSE);
	}

	else
	{
		pMain->m_wndResult.m_WndPropList.GetProperty(4)->Show(TRUE);
	}
}



int CToolsView::InsertPngBtn()
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
	vector<CPngButton>::iterator it;
	CRect wndRect;
	CRect staticRect = {0};
	GetWindowRect(wndRect);
	int i = 0;
	int xPos = 0;
	int staticWidth = 60;
	for (it = btnVector.begin(); it != btnVector.end(); it++)
	{
// 		CStatic* staticText;
// 		staticText = new CStatic;

		it->Create(_T(""), dwStyle, CRect(0, 0, 16, 16), this, btnIdVector.at(i));
		it->MoveWindow(xPos, 0, wndRect.Height(), wndRect.Height());	
/*		staticText->Create(tipStrVector.at(i), dwStyle, CRect(xPos + 16, 0, xPos + 16 + staticWidth, 16), this, btnIdVector.at(i) + 100);*/

		it->SetImagePng((UINT)btnPngVector.at(i));
		it->SetTransparentColor(RGB(255, 255, 255));
		it->SaveBG();
/*		it->ShowWindow(SW_HIDE);*/
		m_tipCtrl.AddTool(GetDlgItem(btnIdVector.at(i)), tipStrVector.at(i));
		m_tipCtrl.SetDelayTime(0); //设置延迟，如果为0则不等待，立即显示
		m_tipCtrl.SetTipTextColor(RGB(0, 0, 255)); //设置提示文本的颜色
		m_tipCtrl.SetTipBkColor(RGB(255, 255, 255)); //设置提示框的背景颜色
		m_tipCtrl.SetMaxTipWidth(600);//设置文本框的最大宽度
		m_tipCtrl.Activate(TRUE); //设置是否启用提示
/*		it->SizeToContent();*/
		i++;
		xPos += 25;
	}
	return 0;
}

// CToolsView 诊断

#ifdef _DEBUG
void CToolsView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CToolsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CToolsView 消息处理程序


int CToolsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	// 	mySearchBtn.SubclassDlgItem(IDC_BTN_SEARCH, this);
	//	mySearchBtn.SetSize(20, 20);//設置按鈕大小
	CRect windowRect;
	GetWindowRect(windowRect);


	textQCTip.Create(L"质控模式", WS_CHILD | WS_VISIBLE,
		CRect(windowRect.Width()/2, 0, windowRect.Width()/2 + 80, 14), this, IDC_QCSTATIC);

	m_tipCtrl.Create(this);

#if (_LC_V < 2250)
	m_btnSet.Create(L"保存设置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(windowRect.Width() - 260, 0, windowRect.Width() - 210, 12), this, IDC_BUTTONSURE + 2);
	HFONT  hFont = CreateFont(12, 0, 0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		L"宋体");
	m_btnSet.SendMessage(WM_SETFONT, (WPARAM)hFont, TRUE);
#endif

	m_numStatic.Create(L"起始序号：", WS_CHILD | WS_VISIBLE,
		CRect(windowRect.Width() - 200, 0, windowRect.Width() - 100, 12), this, IDC_NUMSTATIC);
	m_editNum.Create(WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_WANTRETURN,
		CRect(windowRect.Width() - 100, 0, windowRect.Width(), 12), this, IDC_EDITNUM);

	m_comboxStatic.Create(L"测试类型：", WS_CHILD | WS_VISIBLE,
		CRect(windowRect.Width() - 200, 0, windowRect.Width() - 100, 12), this, IDC_COMBOXSTATIC);
	m_combox.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
		CRect(windowRect.Width() - 100, 0, windowRect.Width(), 12), this, IDC_COMBOBOX);
#if (_LC_V >= 2250)
	m_btnSetTestType.Create(L"设置测试类型", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(420, 0, 550, 30), this, IDC_BUTTONSURE+1);
	m_comboxStatic.ShowWindow(SW_HIDE);
	m_combox.ShowWindow(SW_HIDE);
#endif

	m_comboxStaticUdcItem.Create(L"尿干化项目数：", WS_CHILD | WS_VISIBLE,
		CRect(windowRect.Width() - 200, 0, windowRect.Width() - 100, 12), this, IDC_COMBOXSTATICUDCITEM);
	m_comboxUdcItem.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
		CRect(windowRect.Width() - 100, 0, windowRect.Width(), 12), this, IDC_COMBOBOXUDCITEM);


	textQCTip.SetImagePng(IDB_PNG_qcmain);
	textQCTip.SetTransparentColor(RGB(255, 255, 255));
	textQCTip.SaveBG();
	textQCTip.SizeToContent();
	textQCTip.ShowWindow(SW_HIDE);
	set_font(12, &my_font, L"宋体"); //字体大小、指针、名称
	set_font(14, &my_font_QC, L"宋体"); //字体大小、指针、名称
	m_combox.SetFont(&my_font);//改变字体
	m_comboxStatic.SetFont(&my_font);
	m_comboxUdcItem.SetFont(&my_font);//改变字体
	m_comboxStaticUdcItem.SetFont(&my_font);
	if (m_btnSetTestType.m_hWnd) m_btnSetTestType.SetFont(&my_font);
	m_editNum.SetFont(&my_font);
	m_numStatic.SetFont(&my_font);
// 	m_btnSure.Create(L"确定", WS_CHILD | WS_DISABLED,
// 		CRect(420, 0, 500, 20), this, IDC_BUTTONSURE);

	m_combox.InsertString(0,L"尿干化&尿沉渣");
	m_combox.InsertString(1,L"尿干化");
	m_combox.InsertString(2,L"尿沉渣");
	m_combox.SetCurSel(0);

	m_comboxUdcItem.InsertString(0, L"10");
	m_comboxUdcItem.InsertString(1, L"11");
	m_comboxUdcItem.InsertString(2, L"12");
	m_comboxUdcItem.InsertString(3, L"13");
	m_comboxUdcItem.InsertString(4, L"14");
	m_comboxUdcItem.SetCurSel(2);
	//m_comboxUdcItem.SetCurSel(4);		 //临时修改20240223	 原始值m_comboxUdcItem.SetCurSel(2); 

	CString strText;
	strText.Format(_T("%d"), theApp.m_nSN);
	m_editNum.SetWindowText(strText);

	char INIPath[256] = { 0 };
	GetModuleFileNameA(NULL, INIPath, MAX_PATH);
	PathRemoveFileSpecA(INIPath);
	PathAppendA(INIPath, "config\\setting.ini");
	BOOL testtype_permission = (BOOL)GetPrivateProfileIntA("permission", "test_type", 0, INIPath);
	BOOL testudcitemcount_permission = (BOOL)GetPrivateProfileIntA("permission", "test_udc_itemcount", 0, INIPath);
	bool isAdmin = (theApp.loginUser.Group == 4);
	if (testtype_permission && !isAdmin)
	{
		m_combox.EnableWindow(FALSE);
	}
	if (testudcitemcount_permission && !isAdmin)
	{
		m_comboxUdcItem.EnableWindow(FALSE);
	}

	return 0;
}

void CToolsView::set_font(int height, CFont *font, LPCWSTR name)
{
	// font=new CFont();//不在此，要在外部初始化，否则找不到指针
	LOGFONT lf;
	lf.lfHeight = 20; lf.lfWidth = 0;
	lf.lfEscapement = 0; lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL; lf.lfItalic = 0;
	lf.lfUnderline = 0; lf.lfStrikeOut = 0;
	lf.lfCharSet = 134; lf.lfOutPrecision = 3;
	lf.lfClipPrecision = 2; lf.lfQuality = 1;
	lf.lfPitchAndFamily = 2; lstrcpy(lf.lfFaceName, L"宋体");
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;//OUT_TT_PRECIS;

	lf.lfHeight = height; //字体大小
	lstrcpy(lf.lfFaceName, name);//名称 
	if (font != NULL)
	{
		font->DeleteObject();
		font->CreateFontIndirect(&lf);
	}
}

BOOL CToolsView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	m_tipCtrl.RelayEvent(pMsg);

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (GetFocus() == GetDlgItem(IDC_EDITNUM))
		{
			this->SetFocus();
		}
	}

	return CView::PreTranslateMessage(pMsg);
}


void CToolsView::OnChangeTestType(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(theApp.IsIdleStatus());
}

void CToolsView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

#if (_LC_V < 2250)
	m_btnSet.MoveWindow(cx - 560, 0, 70, 18);
#endif

	/*if (is2020)
	{
		m_numStatic.MoveWindow(0, 3, 60, 12);
		m_editNum.MoveWindow(60, 3, 60, 12);
	}
	else*/
	{
		m_numStatic.MoveWindow(cx - 480, 3, 60, 12);
		m_editNum.MoveWindow(cx - 420, 3, 60, 12);
	}


	m_comboxStatic.MoveWindow(cx - 360, 3, 60, 12);
	m_combox.MoveWindow(cx - 300, 0, 100, 12);

	m_comboxStaticUdcItem.MoveWindow(cx - 180, 3, 90, 12);
	m_comboxUdcItem.MoveWindow(cx - 90, 0, 90, 12);

#if (_LC_V >= 2250)
	m_btnSetTestType.MoveWindow(cx - 360, 0, 150, 25);
#endif

	textQCTip.MoveWindow(cx/2, 3, 80, 29);
	// TODO:  在此处添加消息处理程序代码
}
