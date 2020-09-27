///////////////////////////////////////////////////////////////////////////////
//  COCP.CPP
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Implementation of the COCoProcessor class
//

#include "stdafx.h"
#include "cocp.h"
#include "testxcpt.h"
#include "..\sym\qcqp.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "uidebug.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: CString COCoProcessor::GetCPRegister(int reg)
// Description: Get the value of one of the coprocessor's registers.
// Return: A CString that contains the value of the specified coprocessor's register.
// Param: reg An integer that specifies the 1-based index into the set of coprocessor registers.
// END_HELP_COMMENT
CString  COCoProcessor::GetCPRegister(int reg)
	{
	EXPECT(uicp = UIDebug::ShowDockWindow(IDW_CPU_WIN));
	CString temp  = uicp.GetCPRegister(reg);
	EXPECT(uicp.Close());
	return  temp;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL COCoProcessor::CPRegisterValueIs(int reg, LPCSTR value)
// Description: Determine whether the given coprocessor register matches the given value.
// Return: A Boolean value that indicates whether the coprocessor's register matches the given value (TRUE) or not.
// Param: reg An integer that specifies the 1-based index into the set of coprocessor registers.
// Param: value A pointer to a string that contains the expected value of the specified register.
// END_HELP_COMMENT
BOOL  COCoProcessor::CPRegisterValueIs(int reg, LPCSTR value)
	{
	EXPECT(uicp = UIDebug::ShowDockWindow(IDW_CPU_WIN));
	CString temp  = uicp.GetCPRegister(reg);
	if (temp!=value)
	{
		// log warning
		LOG->RecordInfo( (LPCSTR) "Register Values is %d, expected %d",temp, value);
	}
	EXPECT(uicp.Close());
	return  temp==value;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL COCoProcessor::SetCPRegister(int reg, LPCSTR value)
// Description: Set the value of one of the coprocessor's registers.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: reg An integer that specifies the 1-based index into the set of coprocessor registers.
// Param: value A pointer to a string that contains the value to set the specified register to.
// END_HELP_COMMENT
BOOL COCoProcessor::SetCPRegister(int reg, LPCSTR value)
	{
	EXPECT(uicp = UIDebug::ShowDockWindow(IDW_CPU_WIN));
	EXPECT(uicp.SetCPRegister(reg, value ));
	return uicp.Close();
	};
