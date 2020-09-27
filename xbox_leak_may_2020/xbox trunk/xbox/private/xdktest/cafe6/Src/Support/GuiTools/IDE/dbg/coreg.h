///////////////////////////////////////////////////////////////////////////////
//  COREG.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the CORegisters class
//

#ifndef __COREG_H__
#define __COREG_H__

#include "uireg.h"

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  CORegisters class

// BEGIN_CLASS_HELP
// ClassName: CORegisters
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS CORegisters

	{

	// Data
	private:
		UIRegisters uireg;

	// Utilities
	public:
		int  GetRegister(int reg);
		BOOL SetRegister(int reg, LPCSTR value);
		BOOL RegisterValueIs(int reg, int value);
		BOOL Enable(void);
		BOOL Disable(void);

		CString GetRegister(CString csRegister, int *pnValue);
		BOOL SetRegister(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString = TRUE);
		BOOL RegisterValueIs(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString = TRUE);

		//	added by dverma 3/21/2000
		BOOL RegisterDisplayed(CString csRegister);
	};

#endif // __COREG_H__
