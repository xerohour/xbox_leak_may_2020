///////////////////////////////////////////////////////////////////////////////
//  UIEXCPT.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the UIExceptions class
//

#include "stdafx.h"
#include "uiexcpt.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\sym\vcpp32.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::Activate(void)
// Description: Open the Exceptions dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIExceptions::Activate(void)
{
	UIWB.DoCommand(IDM_RUN_EXCEPTIONS, DC_MNEMONIC);
	if (WaitAttachActive(5000)) {
		return TRUE;
	}
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: CString UIExceptions::ExcptToString(int excpt)
// Description: Convert an exception ID value to a string. For example, the exception value 0xc0000005 is converted to the string c0000005.
// Return: A CString that contains the exception value converted to a string.
// Param: excpt An integer that contains the exception value.
// END_HELP_COMMENT
CString UIExceptions::ExcptToString(int excpt)
{
	CString sExcpt;
	sprintf(sExcpt.GetBuffer(255), "%.8x", excpt);
	return sExcpt;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::SetNumber(int code)
// Description: Set the exception number in the Exceptions dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: code An integer that contains the exception ID to set.
// END_HELP_COMMENT
BOOL UIExceptions::SetNumber(int code)
{
	EXPECT(IsActive() && MST.WEditEnabled(GetLabel(VCPP32_DLG_EXCEP_NUMBER)));
	MST.WEditSetText(GetLabel(VCPP32_DLG_EXCEP_NUMBER), ExcptToString(code));
	return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::SetName(LPCSTR name)
// Description: Set the exception name in the Exceptions dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: name A pointer to a string that contains the name of the exception.
// END_HELP_COMMENT
BOOL UIExceptions::SetName(LPCSTR name)
{
	EXPECT(IsActive() && MST.WEditEnabled(GetLabel(VCPP32_DLG_EXCEP_NAME)));
	MST.WEditSetText(GetLabel(VCPP32_DLG_EXCEP_NAME), name);
	return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::SetOption(int option)
// Description: Set the exception action in the Exceptions dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: option A value that specifies the action to take when the exception is detected: STOP_ALWAYS, IF_NOT_HANDLED.
// END_HELP_COMMENT
BOOL UIExceptions::SetOption(int option)
{
	if (option==STOP_ALWAYS)
	{
		MST.WOptionSelect(GetLabel(VCPP32_DLG_EXCEP_1STCHANCE));
	};
	if (option==IF_NOT_HANDLED )
	{
		MST.WOptionSelect(GetLabel(VCPP32_DLG_EXCEP_2NDCHANCE));
	};
	return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::AddException(void)
// Description: Click the Add button in the Exceptions dialog to add a new exception.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIExceptions::AddException(void)
{
	if (IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_DLG_EXCEP_ADD)))
	{
		MST.WButtonClick(GetLabel(VCPP32_DLG_EXCEP_ADD));
		return TRUE;
	}
	else
	{
		return FALSE;
	};
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::ChangeException(void)
// Description: Click the Change button in the Exceptions dialog to change the exception information.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIExceptions::ChangeException(void)
{
	if (IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_DLG_EXCEP_CHANGE)))
	{
		MST.WButtonClick(GetLabel(VCPP32_DLG_EXCEP_CHANGE));
		return TRUE;
	}
	else
	{
		return FALSE;
	};
};


// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::RestoreDefualts(void)
// Description: Click the Reset button in the Exceptions dialog to restore the default exceptions.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIExceptions::RestoreDefualts(void)
{
	EXPECT(IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_DLG_EXCEP_DEFAULT)));
	MST.WButtonClick(GetLabel(VCPP32_DLG_EXCEP_DEFAULT));
	return TRUE;
};


// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::SetException(int code, int option)
// Description: Add or change an exception in the Exceptions dialog. The exception is added, if it doesn't exist; changed if it does exist.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: code An integer that specifies the exception ID to add or change.
// Param: option A value that specifies the action to take when the exception is detected: STOP_ALWAYS, IF_NOT_HANDLED.
// END_HELP_COMMENT
BOOL UIExceptions::SetException(int code, int option)
{
	EXPECT(SetNumber(code));
	EXPECT(SetName(GetLocString(IDSS_EXCPT_EXCEPT_NAME)));	// "testing change"
	EXPECT(SetOption(option));
	if (!AddException()) ChangeException();
	return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIExceptions::DeleteException(int code)
// Description: Delete an exception from the Exceptions dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: code An integer that specifies the exception ID to delete.
// END_HELP_COMMENT
BOOL UIExceptions::DeleteException(int code)
{
	CString sListItem;
	CString sExcpt = ExcptToString(code);
	EXPECT(IsActive() && MST.WListEnabled(GetLabel(VCPP32_DLG_EXCEP_LIST)));
	int limit = MST.WListCount(GetLabel(VCPP32_DLG_EXCEP_LIST));
	for (int x=1; x< limit; x++)
	{
		MST.WListItemText(GetLabel(VCPP32_DLG_EXCEP_LIST), x, sListItem);
		if (sListItem.Find(sExcpt) != -1 ) { break; };   // is excpt in list item?
	};
	if ( x > limit )  return FALSE;      //excpt not found
	MST.WListItemClk(GetLabel(VCPP32_DLG_EXCEP_LIST), x);
	EXPECT(IsActive() && MST.WButtonEnabled(GetLabel(VCPP32_DLG_EXCEP_DELETE)));
	MST.WButtonClick(GetLabel(VCPP32_DLG_EXCEP_DELETE));
	return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: CString UIExceptions::GetException(int code)
// Description: Get the description for the specified exception from the Exceptions dialog.
// Return: A CString that contains the description of the specified exception.
// Param: code An integer that specifies the exception ID.
// END_HELP_COMMENT
CString  UIExceptions::GetException(int code)
{
	CString sListItem;
	CString sExcpt = ExcptToString(code);
	EXPECT(IsActive() && MST.WListEnabled(GetLabel(VCPP32_DLG_EXCEP_LIST)));
	int limit = MST.WListCount(GetLabel(VCPP32_DLG_EXCEP_LIST));
	for (int x=1; x< limit; x++)
	{
		MST.WListItemText(GetLabel(VCPP32_DLG_EXCEP_LIST), x, sListItem);
		if (sListItem.Find(sExcpt) != -1 ) { break; };   // is excpt in list item?
	};
	return sListItem;
};

