#include "stdafx.h"
#include "TestType.h"
#include "TaskMgr.h"
#include "..\..\..\inc\CmdHelper.h"
#include "DbgMsg.h"
#include "UW.h"
#include "MainFrm.h"


CTestType::CTestType()
{
	m_cType = 3;                  //ALL_TEST_TYPE
}

BOOL CTestType::UpdateCheckType()
 {
	DBG_MSG("3.debug:theApp.setTestType = %d\n", theApp.GetSetTestType());
	DBG_MSG("4.debug:theApp.setTestType = %d,& = %p\n ", theApp.setTestType, &theApp.setTestType);
	GETTESTINFO_CONTEXT ctx = { 0 };
	if (theApp.m_bLCResetSuccess || theApp.setTestType)
	{
		if (GetDevTestInfo(ctx))
		{
			UpdateCheckType(ctx.nTestType);
			theApp.m_nUdcItemCount = ctx.nUdcTestItemCount;
			DBG_MSG("测试类型 = %d, UDC检查项 = %d\n", ctx.nTestType, ctx.nUdcTestItemCount);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CTestType::UpdateCheckType( LONG nCType )
{
	InterlockedExchange(&m_cType, nCType);
	return TRUE;
}


LONG CTestType::GetCheckType()
{
	return InterlockedExchangeAdd(&m_cType, 0);
}

BOOL CTestType::UpdateCheckTypeEx(char* changeTestType)
{
	GETTESTINFO_CONTEXT ctx = { 0 };


	if (GetDevTestInfo(ctx))
	{
		UpdateCheckType(ctx.nTestType);
		theApp.m_nUdcItemCount = ctx.nUdcTestItemCount;
		DBG_MSG("测试类型 = %d, UDC检查项 = %d\n", ctx.nTestType, ctx.nUdcTestItemCount);
		return TRUE;
	}


	return FALSE;
}