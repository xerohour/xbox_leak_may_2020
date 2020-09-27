///////////////////////////////////////////////////////////////////////////////
//  CODAM.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the CODAM class
//

#ifndef __CODAM_H__
#define __CODAM_H__

#include "dbgxprt.h"
#include "uidam.h"


///////////////////////////////////////////////////////////////////////////////
//  CODAM class

// BEGIN_CLASS_HELP
// ClassName: CODAM
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS CODAM

	{

	// Data
	private:
		UIDAM uidam;

	// Utilities
	public:

		BOOL GotoSymbol(LPCSTR symbol, BOOL bCloseAfter = TRUE);
		BOOL GotoAddress(int address, BOOL bCloseAfter = TRUE);
		CString GetInstruction(void);
		BOOL InstructionIs(LPCSTR inst);
		BOOL Enable(void);
		BOOL Disable(void);
		BOOL ToggleSourceDisplay(BOOL bToggle);
		CString GetLines(int startline = 0, int count = 1);
		BOOL InstructionContains(int offset = 1, CString ExpectedInst = "",BOOL bLocateCaretAtLeftTop = FALSE);
		BOOL SwitchBetweenASMAndSRC(BOOL bUsingContextMenu = FALSE);
		BOOL IsActive(void);
		BOOL VerifyCurrentInstruction(LPCSTR szInstruction);
	};

#endif // __CODAM_H__
