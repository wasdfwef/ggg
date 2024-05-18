
// UWDoc.cpp : CUWDoc 类的实现
//

#include "stdafx.h"
#include "UW.h"

#include "UWDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUWDoc

IMPLEMENT_DYNCREATE(CUWDoc, CDocument)

BEGIN_MESSAGE_MAP(CUWDoc, CDocument)
END_MESSAGE_MAP()


// CUWDoc 构造/析构

CUWDoc::CUWDoc()
{
	// TODO: 在此添加一次性构造代码

}

CUWDoc::~CUWDoc()
{
}

BOOL CUWDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CUWDoc 序列化

void CUWDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CUWDoc 诊断

#ifdef _DEBUG
void CUWDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUWDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CUWDoc 命令
