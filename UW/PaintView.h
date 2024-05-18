#pragma once

#include "afxwin.h"
#include <vector>

extern CString UDCI_QC;
extern CString UDCII_QC;

extern CString USI_QC;
extern CString USII_QC;
extern CString USIII_QC;

using namespace std;

typedef struct _PaintView
{
	int x_days;
	int itmeIndex;
	std::string y_range;
	std::string y_target;
	std::string value1;
	std::string value2;
	std::string value3;
	std::string valueDate;
	std::string valueValidDate;
	std::string valueBatch;
	int priType;
	int max;
	int min;
    COleDateTime dtStart;
}PaintView, *PPaintView;

#define WM_PAINT_VIEW WM_USER + 526
#define WM_PAINT_VIEW_US WM_USER + 527

class CPaintView :
	public CWnd
{
public:
	CPaintView();
	~CPaintView();
	DECLARE_MESSAGE_MAP()
	
    virtual void PostNcDestroy();
    afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnPaintBk(WPARAM, LPARAM);
	afx_msg LRESULT OnPaintUsBk(WPARAM, LPARAM);

    void DrawTextVert(int x, int y, CString str, CDC*pDC);
    void PaintBk(CDC &memDC);
    void PaintUsBk(CDC &memDC);
    void PaintPreViewBk(CDC &memDC);
    void PaintCurve(CDC& memDc, int x1, int y1, int x2, int y2);

	PaintView m_paint;
	vector<std::string> vRange;
	vector<std::string> vValue1;
	vector<std::string> vValue2;
	vector<std::string> vValue3;
	vector<std::string> vValueDate;
	vector<std::string> vValueValidDate;
	vector<std::string> vValueBatch;
	int itemIndex;
	int m_max;
    bool isPaintUs;
    bool isPreView;

	void PaintLine();
	int GetHeightByRange(vector<string>& v,int i);
	void DrawGon(CDC &memDc,int flag, LPWSTR nType);
	char * GetItemStringByIndex(int index);
	int GetOffsetByLen(int);

public:
	CBitmap bitmapTemp, *pOldBitmap;
	bool SaveBMP(const char * fileName);
	
};
void SplitString(const string& s, vector<string>& v, const string& c);

