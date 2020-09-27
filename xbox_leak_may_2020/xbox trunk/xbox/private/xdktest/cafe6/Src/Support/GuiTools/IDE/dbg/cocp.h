///////////////////////////////////////////////////////////////////////////////
//  COCP.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the COCoProcessor class
//

#ifndef __COCP_H__
#define __COCP_H__

#include "uicp.h"

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  COCoProcessor class

// BEGIN_CLASS_HELP
// ClassName: COCoProcessor
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COCoProcessor

	{

	// Data
	private:
		UICoProcessor uicp;

	// Utilities
	public:
		CString GetCPRegister(int reg);
		BOOL SetCPRegister(int reg, LPCSTR value);
		BOOL CPRegisterValueIs(int reg, LPCSTR value);
	};


#endif // __COCP_H__
