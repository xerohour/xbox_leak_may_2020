///////////////////////////////////////////////////////////////////////////////
//  COEXCPT.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the COExceptions class
//

#ifndef __COEXCPT_H__
#define __COEXCPT_H__

#include "uiexcpt.h"

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  COExceptions class

// BEGIN_CLASS_HELP
// ClassName: COExceptions
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COExceptions

	{

	// Data
	private:
		UIExceptions uiexcpt;

	// Utilities
	public:
		BOOL VerifyExceptionHit(void);
		BOOL VerifyExceptionHit(int code);
		BOOL SetException(int code, int option);
		CString  GetException(int code);
		BOOL RestoreDefualts(void);

	};

#endif // __COEXCPT_H__
