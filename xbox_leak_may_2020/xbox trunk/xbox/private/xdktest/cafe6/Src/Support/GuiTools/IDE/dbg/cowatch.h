///////////////////////////////////////////////////////////////////////////////
//  COWATCH.H
//
//  Created by:            Date:
//      MichMa                  6/11/98
//
//  Description:
//      Declaration of the COWatch class
//

#ifndef __COWATCH_H__
#define __COWATCH_H__

#include "dbgxprt.h"
#include "uiwatch.h"
#include "coee.h"

#pragma warning (disable : 4069)


///////////////////////////////////////////////////////////////////////////////
//  CWatch class

// BEGIN_CLASS_HELP
// ClassName: CWatch
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS CWatch

{
public:

	CString m_strType;
	CString m_strName;
	CString m_strValue;
	EXPR_STATE m_ExprState;
	CWatch(void){}

	const CWatch& operator=(const CWatch &WatchSrc)
	{
		m_strType = WatchSrc.m_strType;
		m_strName = WatchSrc.m_strName;
		m_strValue = WatchSrc.m_strValue;
		m_ExprState = WatchSrc.m_ExprState;
		return *this;	
	}

	CWatch(CWatch &WatchSrc)
	{
		m_strType = WatchSrc.m_strType;
		m_strName = WatchSrc.m_strName;
		m_strValue = WatchSrc.m_strValue;
		m_ExprState = WatchSrc.m_ExprState;
	}
};


///////////////////////////////////////////////////////////////////////////////
//  COWatch class

// BEGIN_CLASS_HELP
// ClassName: COWatch
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COWatch

{
	// Data
	private:
		UIWatch uiwatch;
		COExpEval coee;

	// Utilities
	public:
		BOOL AddWatch(LPCSTR szWatch);
		BOOL RemoveWatch(LPCSTR szWatch);
		BOOL ExpandWatch(LPCSTR szWatch);
		BOOL CollapseWatch(LPCSTR szWatch);
		BOOL VerifyWatchInfo(CWatch Watch);
		BOOL VerifyWatchInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState);
		BOOL WatchDoesNotExist(LPCSTR szWatch);
		BOOL WatchCountIs(int intWatchCount);
		BOOL SetWatchValue(LPCSTR szWatch, LPCSTR szValue);
};

#endif // __COWATCH_H__
