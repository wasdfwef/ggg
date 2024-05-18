// DebugSheet.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "DebugSheet.h"


// CDebugSheet

IMPLEMENT_DYNAMIC(CDebugSheet, CPropertySheet)

CDebugSheet::CDebugSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CDebugSheet::CDebugSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

CDebugSheet::~CDebugSheet()
{
}


BEGIN_MESSAGE_MAP(CDebugSheet, CPropertySheet)
END_MESSAGE_MAP()


// CDebugSheet 消息处理程序
