// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9AA67A2C_E23F_46A7_A44F_06C26A13EFFE__INCLUDED_)
#define AFX_STDAFX_H__9AA67A2C_E23F_46A7_A44F_06C26A13EFFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#include "DbgMsg.h"

#define DbgPrint TraceDbgNoSrcFileInfo 
#define GetItemCount(List) sizeof(List)/sizeof(List[0])

#define CheckResultCode(FuncName, ChildFuncName, Result)											\
{																									\
	if ( STATUS_SUCCESS != (Result) )																\
	{																								\
	DbgPrint( "%s in %s failed with error = 0x%X", (ChildFuncName), (FuncName), (Result) );		\
	goto end;																					\
	}																								\
}																									


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9AA67A2C_E23F_46A7_A44F_06C26A13EFFE__INCLUDED_)
