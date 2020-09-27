///////////////////////////////////////////////////////////////////////////////
//  COREG.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the CORegisters class
//

#include "stdafx.h"
#include "coreg.h"
#include "testxcpt.h"
#include "..\sym\qcqp.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "uidebug.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: int CORegisters::GetRegister(int reg)
// Description: Get the value of one of the registers.
// Return: An integer that contains the value of the specified register.
// Param: reg An integer that specifies the 1-based index into the set of registers.
// END_HELP_COMMENT
int  CORegisters::GetRegister(int reg)
	{
	EXPECT(uireg = UIDebug::ShowDockWindow(IDW_CPU_WIN));
	int temp  = uireg.GetRegister(reg);
	EXPECT(uireg.Close());
	return  temp;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL CORegisters::RegisterValueIs(int reg, int value)
// Description: Determine whether the given register's value matches the given value.
// Return: A Boolean that indicates whether the given register's value matches the given value (TRUE) or not.
// Param: reg An integer that specifies the 1-based index into the set of registers.
// Param: value An integer that contains the expected register value.
// END_HELP_COMMENT
BOOL  CORegisters::RegisterValueIs(int reg, int value)
	{
	EXPECT(uireg = UIDebug::ShowDockWindow(IDW_CPU_WIN));
	int temp  = uireg.GetRegister(reg);
	if (temp!=value)
	{
		// log warning
		LOG->RecordInfo( (LPCSTR) "Register Values is %d, expected %d",temp, value);
	}
	EXPECT(uireg.Close());
	return  temp==value;
	};


// BEGIN_HELP_COMMENT
// Function: BOOL CORegisters::SetRegister(int reg, LPCSTR value)
// Description: Set the value of the specified register.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: reg An integer that specifies the 1-based index into the set of registers.
// Param: value A pointer to a string that contains the value to set the specified register to.
// END_HELP_COMMENT
BOOL CORegisters::SetRegister(int reg, LPCSTR value)
	{
	EXPECT(uireg = UIDebug::ShowDockWindow(IDW_CPU_WIN));
	EXPECT(uireg.SetRegister(reg, value ));
	return uireg.Close();
	};


// BEGIN_HELP_COMMENT
// Function: BOOL CORegisters::Enable()
// Description: Enable access to registers in the debugger.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL CORegisters::Enable()
{
	return uireg.Activate();
}


// BEGIN_HELP_COMMENT
// Function: BOOL CORegisters::Disable()
// Description: Disable access to registers in the debugger.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL CORegisters::Disable()
{
	// TODO(michma)
	// return FALSE;
	return uireg.Close();
}

// BEGIN_HELP_COMMENT
// Function: CString CORegisters::GetRegister(CString csRegister, int *pnValue)
// Description: Get the value of one of the registers.
// Return: A CString that contains the value of the specified register.
// Param : CString csRegister a string that specifies the register
// Param int* pnValue A pointer to integer that contains the value of the specified register.
// END_HELP_COMMENT
CString CORegisters::GetRegister(CString csRegister, int *pnValue)
{
	return uireg.GetRegister(csRegister, pnValue);
}

// BEGIN_HELP_COMMENT
// Function: BOOL CORegisters::SetRegister(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString = TRUE)
// Description: Set the value of the specified register.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param : CString csRegister CString that specifies the register to be set
// Param : CString szValue A CString that contains the value to set the specified register to
// Param : int nValue An int that contains the value to set the specified register to
// Param : bValueIsCString A BOOL that indicats if value is represented in the form of CString (TRUE) or int
// END_HELP_COMMENT
BOOL CORegisters::SetRegister(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString /* TRUE */)
{
	return uireg.SetRegister(csRegister, szValue, nValue, bValueIsCString);
}

// BEGIN_HELP_COMMENT
// Function: BOOL CORegisters::RegisterValueIs(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString = TRUE)
// Description: Determine whether the given register's value matches the given value.
// Return: A Boolean that indicates whether the given register's value matches the given value (TRUE) or not.
// Param : CString csRegister CString that specifies the register to be set
// Param : CString szValue A CString that contains the expected register value
// Param : int nValue An int that contains the expected register value
// Param : bValueIsCString A BOOL that indicats if value is represented in the form of CString (TRUE) or int
// END_HELP_COMMENT
BOOL CORegisters::RegisterValueIs(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString /* TRUE */)
{
	CString csActualValue;
	int nActualValue;
	BOOL bRetValue;

	UIDebug::ShowDockWindow(IDW_CPU_WIN);
	csActualValue = uireg.GetRegister(csRegister, &nActualValue);

	if(csActualValue.IsEmpty())
		return FALSE;


	if(bValueIsCString)
		bRetValue = (szValue == csActualValue);

	else
		bRetValue = (nValue == nActualValue);

	if(!bRetValue)
	{
		if(bValueIsCString)
			LOG->RecordInfo((LPCSTR) "Actual Register (%s) value (%s) doesn't match the given value (%s)", 
						LPCTSTR(csRegister), LPCTSTR(csActualValue), LPCTSTR(szValue ));
		else
			LOG->RecordInfo((LPCSTR) "Actual Register (%s) value doesn't match the given value ", LPCTSTR(csRegister));
	}

	return bRetValue;
}


// BEGIN_HELP_COMMENT
//	added by dverma 3/21/2000
// Function: BOOL CORegisters::RegisterDisplayed(CString csRegister)
// Description: Checks whether the given register is displayed in the registers window
// Return: A Boolean that indicates whether the given register was displayed or not
// Param : CString that specifies the register to checked
// END_HELP_COMMENT
BOOL CORegisters::RegisterDisplayed(CString csRegister)
{
	return uireg.RegisterDisplayed(csRegister);
}
