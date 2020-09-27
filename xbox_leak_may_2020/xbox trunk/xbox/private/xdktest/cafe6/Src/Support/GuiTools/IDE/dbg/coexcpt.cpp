///////////////////////////////////////////////////////////////////////////////
//  COEXCPT.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the COExceptions class
//

#include "stdafx.h"
#include "coexcpt.h"
#include "testxcpt.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL COExceptions::VerifyExceptionHit(void)
// Description: Determine whether an exception was hit. (See VerifyExceptionHit(int) to verify a specific exception.)
// Return: A Boolean value that indicates whether an exception was hit (TRUE) or not.
// END_HELP_COMMENT
BOOL COExceptions::VerifyExceptionHit(void)
	{
	// todo check to see if any exception was hit (message box).
	if (FALSE)
	{
		LOG->RecordInfo( (LPCSTR) "No Exceptions trapped");
	}
	return TRUE;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL COExceptions::VerifyExceptionHit(int code)
// Description: Determine whether a specific exception was hit. (See VerifyException(void) to verify any exception.)
// Return: A Boolean value that indicates whether the specified exception was hit (TRUE) or not.
// Param: code An integer that contains the exception code expected.
// END_HELP_COMMENT
BOOL COExceptions::VerifyExceptionHit(int code)
	{
	// todo check to see if the specified exception was hit (message box).
	BOOL temp = FALSE;  //call white box verify here?
	if (FALSE)
	{
		LOG->RecordInfo( (LPCSTR) "Exception %d was not trapped", code );
	}
	return TRUE;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL COExceptions::SetException(int code, int option)
// Description: Set or change an exception.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: code An integer that contains the exception code.
// Param: option A value that specifies the type of action to take when the exception is caught: STOP_ALWAYS, IF_NOT_HANDLED.
// END_HELP_COMMENT
BOOL COExceptions::SetException(int code, int option)
	{
	EXPECT(uiexcpt.Activate());
	EXPECT(uiexcpt.SetException(code, option));
	EXPECT(uiexcpt.Close());
	// verify really set, log warning return FALSE if not
	return TRUE;
	};

// BEGIN_HELP_COMMENT
// Function: CString  COExceptions::GetException(int code)
// Description: Get a string that describes the exception.
// Return: A CString that contains a string describing the exception specified.
// Param: code An integer that contains the exception code.
// END_HELP_COMMENT
CString  COExceptions::GetException(int code)
	{
	EXPECT(uiexcpt.Activate());
	CString temp = uiexcpt.GetException(code);
	EXPECT(uiexcpt.Close());
	return  temp;
	};

// BEGIN_HELP_COMMENT
// Function: BOOL COExceptions::RestoreDefualts(void)
// Description: Restore all default exceptions.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COExceptions::RestoreDefualts(void)
	{
	EXPECT(uiexcpt.Activate());
	EXPECT(uiexcpt.RestoreDefualts());
	EXPECT(uiexcpt.Close());
	// verify really done, log warning return FALSE if not
	return TRUE;
	};
