#pragma once
#include "afxcmn.h"
class CMyToolTipCtrl :
	public CToolTipCtrl
{
public:
	CMyToolTipCtrl();
	~CMyToolTipCtrl();
	int a;
	CMyToolTipCtrl & CMyToolTipCtrl::operator = (const CMyToolTipCtrl & RightSides)//添加运算符=
	{
		a = RightSides.a;
		return *this;
	}
	CMyToolTipCtrl(const CMyToolTipCtrl & C)//添加拷贝构造函数
	{

		a = C.a;
	}

};

