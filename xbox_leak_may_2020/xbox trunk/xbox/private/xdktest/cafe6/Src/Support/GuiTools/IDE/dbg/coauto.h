///////////////////////////////////////////////////////////////////////////////
//  COAUTO.H
//
//  Created by:            Date:
//      MichMa                  9/16/98
//
//  Description:
//      Declaration of the COAuto class
//

#ifndef __COAUTO_H__
#define __COAUTO_H__

#include "dbgxprt.h"
#include "uivar.h"
#include "coee.h"

#pragma warning (disable : 4069)


///////////////////////////////////////////////////////////////////////////////
//  CAuto class

// BEGIN_CLASS_HELP
// ClassName: CAuto
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS CAuto

{
public:

	CString m_strType;
	CString m_strName;
	CString m_strValue;
	EXPR_STATE m_ExprState;
	CAuto(void){}

	const CAuto& operator=(const CAuto &AutoSrc)
	{
		m_strType = AutoSrc.m_strType;
		m_strName = AutoSrc.m_strName;
		m_strValue = AutoSrc.m_strValue;
		m_ExprState = AutoSrc.m_ExprState;
		return *this;	
	}

	CAuto(CAuto &AutoSrc)
	{
		m_strType = AutoSrc.m_strType;
		m_strName = AutoSrc.m_strName;
		m_strValue = AutoSrc.m_strValue;
		m_ExprState = AutoSrc.m_ExprState;
	}
};


///////////////////////////////////////////////////////////////////////////////
//  COAuto class

// BEGIN_CLASS_HELP
// ClassName: COAuto
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COAuto

{
	// Data
	private:
		UIVariables m_uivar;
		COExpEval m_coee;

	// Utilities
	public:
		BOOL VerifyAutoInfo(CAuto Auto);
		BOOL VerifyAutoInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState);
		BOOL AutoDoesNotExist(LPCSTR szName);
		BOOL AutoCountIs(int intAutoCount);
		BOOL SetAutoValue(LPCSTR szName, LPCSTR szValue);
		BOOL VerifyReturnValue(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState);
};

#endif // __COAUTO_H__
