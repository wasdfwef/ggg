#include "stdafx.h"
#include "MyListCtrl.h"
#include "THDBUW_Access.h"

extern
std::wstring GetIdsString(const std::wstring &sMain, const std::wstring &sId);
typedef struct positiveID
{
	int id;
	int active;

}positiveID;

extern std::vector<positiveID> positiveUDCID;
extern std::vector<positiveID> positiveUSID;

#define	POSITIVETEXT RGB(178,34,34);
#define UNCERTAINTEXT RGB(0,139,69);
#define INACTIVETEXT RGB(180,180,180);
#define SELECTROW RGB(0,0,200);

CMyListCtrl::CMyListCtrl()
{

	ReadInitListColor();
	curSelectID = 0;
	isShowButton = true;
}


CMyListCtrl::~CMyListCtrl()
{

	SaveInitListColor();

}
BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CMyListCtrl::OnNMCustomdraw)
	ON_BN_CLICKED(IDC_BUTTON_LISTSKIN, &CMyListCtrl::OnBnClickedListSkin)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(IDC_BTN_SEARCH,OnUpdateCommandUI)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


void CMyListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*> (pNMHDR);
	*pResult = 0;
	// TODO: Add your control notification handler code here
	//指定列表项绘制前后发送消息
	std::vector<positiveID>::iterator it;
	for (it = positiveUDCID.begin(); it != positiveUDCID.end();it++)
	{
		if ((int)pLVCD->nmcd.lItemlParam == it->id)
		{
			if (it->active == NOTHING || it->active == FAIL || it->active == TESTFAIL)
			{
				pLVCD->clrText = INACTIVETEXT;
				*pResult = CDRF_NEWFONT;
				continue;
			}
			pLVCD->clrText = POSITIVETEXT;
			*pResult = CDRF_NEWFONT;
		}
		else
		{
			*pResult = CDRF_DODEFAULT;
		}
	}



	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}

	//奇偶颜色变化
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
// 		if ((int)pLVCD->nmcd.lItemlParam == curSelectID)
// 		{
// 			pLVCD->clrTextBk = SELECTROW;
// 		}
		//奇数行 
		if (pLVCD->nmcd.dwItemSpec % 2)
		{
			pLVCD->clrTextBk = m_EvenColor;
		}
		//偶数行 
		else
		{
			pLVCD->clrTextBk = m_OddColor;
		}
		//继续 
		*pResult = CDRF_DODEFAULT;
	}

}

void CMyListCtrl::OnBnClickedListSkin()
{
	
	m_ListSkin.GetLastColor(m_OddColor,m_EvenColor);
	CRect buttonPos;
	GetClientRect(buttonPos);
	ClientToScreen(buttonPos);
	m_ListSkin.GetParentPos(buttonPos);
	m_ListSkin.DoModal();
	m_OddColor = m_ListSkin.m_OddColor;
	m_EvenColor = m_ListSkin.m_EvenColor;
/* 	::PostMessage(GetParent()->GetSafeHwnd(), WM_UPDATESKINUI, 0, 0);*/
 	this->PostMessageW(NM_CUSTOMDRAW, 0, 0);
	Invalidate();
	UpdateWindow();
}

void CMyListCtrl::OnUpdateCommandUI(CCmdUI *cmdUI)
{

}

int CMyListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;

	m_tipCtrl.Create(this);
	m_BtnListskin.Create(_T(""), dwStyle, CRect(0, 0, 20, 20), this, IDC_BUTTON_LISTSKIN);
	m_BtnListskin.SetImagePng((UINT)IDB_PNG_LISTSKIN);
	m_BtnListskin.SetTransparentColor(RGB(255, 255, 255));
	m_BtnListskin.SaveBG();
	m_BtnListskin.ShowWindow(SW_HIDE);
	m_tipCtrl.AddTool(GetDlgItem(IDC_BUTTON_LISTSKIN),L"更换列表颜色");
	m_tipCtrl.SetDelayTime(0); //设置延迟，如果为0则不等待，立即显示
	m_tipCtrl.SetTipTextColor(RGB(0, 0, 255)); //设置提示文本的颜色
	m_tipCtrl.SetTipBkColor(RGB(255, 255, 255)); //设置提示框的背景颜色
	m_tipCtrl.SetMaxTipWidth(600);//设置文本框的最大宽度
	m_tipCtrl.Activate(FALSE); //设置是否启用提示


	return 0;
}

void CMyListCtrl::InitRes()
{
	
}



bool CMyListCtrl::IsButtonArea(LONG x, LONG y)
{
	CRect buttonRect;
	m_BtnListskin.GetWindowRect(buttonRect);
	if (x >= buttonRect.left && x <= buttonRect.right && y >= buttonRect.top && y <= buttonRect.bottom)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CMyListCtrl::ReadInitListColor()
{
	int i = 0;
	int colorMapint[3];

	CString oddColor = GetIdsString(L"listctrlskin", L"odd").c_str();
	CString* colorMapStr = SplitString(oddColor, ',', i);
	for (int j = 0; j < i; j++)
	{
		colorMapint[j] = _ttoi(colorMapStr[j]);
	}
	m_OddColor = RGB(colorMapint[0], colorMapint[1], colorMapint[2]);

	CString evenColor = GetIdsString(L"listctrlskin", L"even").c_str();
	colorMapStr = SplitString(evenColor, ',', i);
	for (int j = 0; j < i; j++)
	{
		colorMapint[j] = _ttoi(colorMapStr[j]);
	}
	m_EvenColor = RGB(colorMapint[0], colorMapint[1], colorMapint[2]);
}

void CMyListCtrl::SaveInitListColor()
{
	char buffer[64] = { 0 };
	TCHAR sModuleFile[MAX_PATH] = { 0 };
	TCHAR sDrive[MAX_PATH] = { 0 };
	TCHAR sDir[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, sModuleFile, MAX_PATH);

	_wsplitpath(sModuleFile, sDrive, sDir, NULL, NULL);

	TCHAR s_inifile[MAX_PATH] = { 0 };

	wsprintf(s_inifile, _T("%s%s%d.ini"), sDrive, sDir, 2052);
	int rgbValue[3] = { 0 };
	CString rgbValueStr;

	rgbValue[0] = GetRValue(m_OddColor);
	rgbValue[1] = GetGValue(m_OddColor);
	rgbValue[2] = GetBValue(m_OddColor);
	rgbValueStr = (CString)itoa(rgbValue[0], buffer, 10) + "," + (CString)itoa(rgbValue[1], buffer, 10) + "," + (CString)itoa(rgbValue[2], buffer, 10);
	WritePrivateProfileString(L"listctrlskin", L"odd", rgbValueStr, s_inifile);

	rgbValue[0] = GetRValue(m_EvenColor);
	rgbValue[1] = GetGValue(m_EvenColor);
	rgbValue[2] = GetBValue(m_EvenColor);
	rgbValueStr = (CString)itoa(rgbValue[0], buffer, 10) + "," + (CString)itoa(rgbValue[1], buffer, 10) + "," + (CString)itoa(rgbValue[2], buffer, 10);
	WritePrivateProfileString(L"listctrlskin", L"even", rgbValueStr, s_inifile);
}

void CMyListCtrl::SetIsChangeListColor(bool isShow,COLORREF odd, COLORREF even)
{
	isShowButton = isShow;
	if (isShowButton == FALSE)
	{
		m_OddColor = odd;
		m_EvenColor = even;
		m_BtnListskin.ShowWindow(SW_HIDE);
	}
}

void CMyListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);
	CRect wndRect;
	GetClientRect(wndRect);
	m_BtnListskin.MoveWindow(wndRect.right - 20 ,0, 20, 20);

	// TODO:  在此处添加消息处理程序代码
}


BOOL CMyListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类

	int ret;
	CWnd *pWnd = FromHandle(pMsg->hwnd);
	if (IsButtonArea(pMsg->pt.x, pMsg->pt.y) && pWnd->GetDlgCtrlID() == 0)
	{
		m_BtnListskin.PostMessageW(pMsg->message, 0, 0);
		if (pMsg->message == WM_MOUSEMOVE)
			m_tipCtrl.RelayEvent(pMsg);
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}


CString * SplitString(CString str, char split, int& iSubStrs)
{
	int iPos = 0; //分割符位置
	int iNums = 0; //分割符的总数
	CString strTemp = str;
	CString strRight;
	//先计算子字符串的数量
	while (iPos != -1)
	{
		iPos = strTemp.Find(split);
		if (iPos == -1)
		{
			break;
		}
		strRight = strTemp.Mid(iPos + 1, str.GetLength());
		strTemp = strRight;
		iNums++;
	}
	if (iNums == 0) //没有找到分割符
	{
		//子字符串数就是字符串本身
		iSubStrs = 1;
		return NULL;
	}
	//子字符串数组
	iSubStrs = iNums + 1; //子串的数量 = 分割符数量 + 1
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//左子串
		strLeft = strTemp.Left(iPos);
		//右子串
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}

BOOL CMyListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	CRect wndRect;
	
	// TODO:  在此添加专用代码和/或调用基类
// 	HD_NOTIFY  *pHDN = (HD_NOTIFY*)lParam;
// 
// 	if (pHDN->hdr.code == HDN_ITEMCHANGINGW || pHDN->hdr.code == HDN_ITEMCHANGINGA)
// 	{
// 		GetClientRect(wndRect);
// 		PostMessageW(WM_SIZE, SIZE_RESTORED, MAKELPARAM(wndRect.Width()-20, wndRect.Height()));
// 	}
	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

bool CMyListCtrl::IsResultGreateOnePlus(int nIndex)
{
// 	int nCols = m_List.GetHeaderCtrl()->GetItemCount();
// 	for (int i = 2; i < nCols; i++)
// 	{
// 		CString strItem = m_List.GetItemText(nIndex, i);
// 		int pos = strItem.Find(_T("(+"));
// 		if (pos >= 0)
// 		{
// 			CString src = strItem.Mid(pos + 2, 1);
// 			if (src == src.SpanIncluding(_T("0123456789")))
// 			{
// 				return true;
// 			}
// 		}
// 	}
// 
	return false;
}

void CMyListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);
	GetParent()->SendMessage(WM_SETFOCUS, (WPARAM)pOldWnd, 0);
/*	GetParent()->SetFocus();*/
	// TODO:  在此处添加消息处理程序代码
}
