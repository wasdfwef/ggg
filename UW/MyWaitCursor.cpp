#include "StdAfx.h"
#include "MyWaitCursor.h"


CMyWaitCursor::CMyWaitCursor(void)
{
	
}

void CMyWaitCursor::Restore()//如果你执行了可能会使光标改变的操作，比如显示消息框或对话框，则应调用Restore成员函数以恢复光标。即使当前正在显示等待光标也可以调用Restore函数
{
	AfxGetApp()->RestoreWaitCursor();
}

void CMyWaitCursor::begin()
{
	AfxGetApp()->BeginWaitCursor();
}

CMyWaitCursor::~CMyWaitCursor(void)
{
	//消耗掉消息队列中的所有消息以达到在鼠标忙碌状态下点击不响应的效果
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		//WM_PAINT不能被REMOVE，需要Dispatch
		if (msg.message = WM_PAINT)
		{
			::DispatchMessage(&msg);
		}
	}
	AFX_BEGIN_DESTRUCTOR

		AfxGetApp()->EndWaitCursor();

	AFX_END_DESTRUCTOR
}