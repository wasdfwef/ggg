#pragma once



// CDebugSheet

class CDebugSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDebugSheet)

public:
	CDebugSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDebugSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CDebugSheet();

protected:
	DECLARE_MESSAGE_MAP()
};


