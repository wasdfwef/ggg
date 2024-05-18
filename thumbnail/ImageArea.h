#pragma once


// CImageArea

class CImageArea : public CStatic
{
	DECLARE_DYNAMIC(CImageArea)

public:
	CImageArea();
	virtual ~CImageArea();

protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};


