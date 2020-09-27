///////////////////////////////////////////////////////////////////////////////
//  COLOCALS.H
//
//  Created by:            Date:
//      MichMa                  10/26/97
//
//  Description:
//      Declaration of the COLocals class
//

#ifndef __COLOCALS_H__
#define __COLOCALS_H__

#include "uivar.h"
#include "coee.h"
#include "dbgxprt.h"

#pragma warning (disable : 4069)


///////////////////////////////////////////////////////////////////////////////
//  CLocal class

// BEGIN_CLASS_HELP
// ClassName: CLocal
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS CLocal

{
public:

	CString m_strType;
	CString m_strName;
	CString m_strValue;
	EXPR_STATE m_ExprState;
	CLocal(void){}

	const CLocal& operator=(const CLocal &LocalSrc)
	{
		m_strType = LocalSrc.m_strType;
		m_strName = LocalSrc.m_strName;
		m_strValue = LocalSrc.m_strValue;
		m_ExprState = LocalSrc.m_ExprState;
		return *this;	
	}

	CLocal(CLocal &LocalSrc)
	{
		m_strType = LocalSrc.m_strType;
		m_strName = LocalSrc.m_strName;
		m_strValue = LocalSrc.m_strValue;
		m_ExprState = LocalSrc.m_ExprState;
	}
};


///////////////////////////////////////////////////////////////////////////////
//  COLocals class

// BEGIN_CLASS_HELP
// ClassName: COLocals
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COLocals

{
	// Data
	private:
		UIVariables uivar;
		COExpEval coee;

	// Utilities
	public:
		BOOL VerifyLocalInfo(CLocal Local);
		BOOL VerifyLocalInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState);
		BOOL LocalDoesNotExist(LPCSTR szLocal);
		BOOL LocalsCountIs(int intLocalCount);
		BOOL SetLocalValue(LPCSTR szLocal, LPCSTR szValue);
};

#endif // __COLOCALS_H__
