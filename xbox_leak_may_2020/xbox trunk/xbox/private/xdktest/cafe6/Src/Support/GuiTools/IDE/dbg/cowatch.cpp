///////////////////////////////////////////////////////////////////////////////
//  COWATCH.CPP
//
//  Created by:            Date:
//      MichMa                  6/11/98
//
//  Description:
//      Implementation of the COWatch class
//

#include "stdafx.h"
#include "cowatch.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#pragma warning (disable : 4069)


// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::VerifyWatchInfo(CWatch Watch)
// Description: Verify that a watch of the specified name, value, type, and state exists in the debugger.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: Watch A CWatch object containing the name, value, type, and state of the watch to verify.
// END_HELP_COMMENT
BOOL COWatch::VerifyWatchInfo(CWatch Watch)

{
	const char* const THIS_FUNCTION = "COWatch::VerifyWatchInfo()";

	// activate the watch window.
	// TODO(michma): UIWatch::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uiwatch.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate watch window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uiwatch.Activate();

	// select the watch.
	if(!uiwatch.SelectWatch(Watch.m_strName))
	{
		LOG->RecordInfo("ERROR in %s: can't select watch \"%s\".", THIS_FUNCTION, Watch.m_strName);
		return FALSE;
	}
	
	// stores the watch's actual and expected information.
	EXPR_INFO ActualWatchInfo, ExpectedWatchInfo;
	coee.FillExprInfo(ExpectedWatchInfo, Watch.m_ExprState, Watch.m_strType, Watch.m_strName, Watch.m_strValue);

	// get the fields of the watch from the watch window.
	if(uiwatch.GetAllFields(&ActualWatchInfo, ROW_CURRENT) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: couldn't get fields of watch \"%s\".", THIS_FUNCTION, Watch.m_strName);
		return FALSE;
	}

	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	// compare the actual watch info with the expected watch info.
	return coee.CompareExprInfo(&ActualWatchInfo, &ExpectedWatchInfo);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::VerifyWatchInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState)
// Description: Verify that a watch of the specified name, value, type, and state exists in the debugger.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szName - A string specifying the name of the watch.
// Param: szValue - A string specifying the value of the watch.
// Param: szType - A string specifying the type of the watch.
// Param: ExprState - An EXPR_STATE value specifying the expansion state of the watch (NOT_EXPANABLE, COLLAPSED, EXPANDED).
// END_HELP_COMMENT
BOOL COWatch::VerifyWatchInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState)
{
	CWatch Watch;
	Watch.m_ExprState = ExprState;
	Watch.m_strType = szType;
	Watch.m_strName = szName;
	Watch.m_strValue = szValue;
	return VerifyWatchInfo(Watch);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::WatchDoesNotExist(LPCSTR szWatch)
// Description: Verify that a watch of the specified name does not exist.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szWatch A pointer to a string that contains the watch to check for.
// END_HELP_COMMENT
BOOL COWatch::WatchDoesNotExist(LPCSTR szWatch)

{
	const char* const THIS_FUNCTION = "COWatch::WatchDoesNotExist";

	// activate the watch window.
	// TODO(michma): UIWatch::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uiwatch.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate watch window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uiwatch.Activate();
	
	BOOL bResult = uiwatch.SelectWatch(szWatch);
	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return !bResult;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::WatchCountIs(int intWatchCount)
// Description: Verify that the debugger has an accurate count of the watches.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: intWatchCount An integer specifying the expected count of watches.
// END_HELP_COMMENT
BOOL COWatch::WatchCountIs(int intWatchCount)

{
	const char* const THIS_FUNCTION = "COWatch::WatchCountIs()";

	// activate the watch window.
	// TODO(michma): UIWatch::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uiwatch.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate watch window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uiwatch.Activate();

	int intActualWatchCount = uiwatch.GetWatchCount();
	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return intActualWatchCount == intWatchCount;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::AddWatch(LPCSTR szWatch)
// Description: Adds a watch on an expression to the debugger.
// Return: A Boolean value that specifies whether the operation was sucessful (TRUE) or not (FALSE).
// Param: szWatch A string specifying the name of the expression for which to add a watch.
// END_HELP_COMMENT
BOOL COWatch::AddWatch(LPCSTR szWatch)

{
	const char* const THIS_FUNCTION = "COWatch::AddWatch()";

	// activate the watch window.
	// TODO(michma): UIWatch::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uiwatch.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate watch window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uiwatch.Activate();

	// add the watch by setting the name of the last (empty) row in the watch window.
	if(uiwatch.SetName(szWatch, ROW_LAST) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: can't set name \"%s\" in watch window.", THIS_FUNCTION);
		return FALSE;
	}

	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::RemoveWatch(LPCSTR szWatch)
// Description: Removes a watch on an expression from the debugger.
// Return: A Boolean value that specifies whether the operation was sucessful (TRUE) or not (FALSE).
// Param: szWatch A string specifying the name of the expression for which to remove a watch.
// END_HELP_COMMENT
BOOL COWatch::RemoveWatch(LPCSTR szWatch)
{
	const char* const THIS_FUNCTION = "COWatch::RemoveWatch()";

	// activate the watch window.
	// TODO(michma): UIWatch::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uiwatch.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate watch window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uiwatch.Activate();

	// select the watch to delete.
	if(!uiwatch.SelectWatch(szWatch))
	{
		LOG->RecordInfo("ERROR in %s: can't select watch \"%s\" in watch window.", THIS_FUNCTION, szWatch);
		return FALSE;
	}

	// delete the watch.
	if(uiwatch.Delete() != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: can't delete watch \"%s\" in watch window.", THIS_FUNCTION, szWatch);
		return FALSE;
	}

	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::SetWatchValue(LPCSTR szWatch, LPCSTR szValue)
// Description: Sets the value of a watch expression.
// Return: A Boolean value that specifies whether the operation was sucessful (TRUE) or not (FALSE).
// Param: szWatch A string specifying the name of the watch expression for which to assign a value.
// Param: szValue A string specifying the value to be assigned to the watch expression.
// END_HELP_COMMENT
BOOL COWatch::SetWatchValue(LPCSTR szWatch, LPCSTR szValue)
{
	const char* const THIS_FUNCTION = "COWatch::SetWatchValue()";

	// activate the watch window.
	// TODO(michma): UIWatch::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uiwatch.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate watch window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uiwatch.Activate();

	// select the watch to change.
	if(!uiwatch.SelectWatch(szWatch))
	{
		LOG->RecordInfo("ERROR in %s: can't select watch \"%s\" in watch window.", THIS_FUNCTION, szWatch);
		return FALSE;
	}

	// change the watch value.
	if(uiwatch.SetValue(szValue, ROW_CURRENT, NAME_IS_EDITABLE) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: can't change value in watch window \"%s\" in watch window.", THIS_FUNCTION, szWatch);
		return FALSE;
	}

	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::ExpandWatch(LPCSTR szWatch)
// Description: Expands a watch on an expression in the debugger.
// Return: A Boolean value that specifies whether the operation was sucessful (TRUE) or not (FALSE).
// Param: szWatch A string specifying the name of the expression for which to expand a watch.
// END_HELP_COMMENT
BOOL COWatch::ExpandWatch(LPCSTR szWatch)

{
	const char* const THIS_FUNCTION = "COWatch::ExpandWatch()";

	// activate the watch window.
	// TODO(michma): UIWatch::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uiwatch.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate watch window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uiwatch.Activate();

	// select the watch to expand.
	if(!uiwatch.SelectWatch(szWatch))
	{
		LOG->RecordInfo("ERROR in %s: can't select watch \"%s\" in watch window.", THIS_FUNCTION, szWatch);
		return FALSE;
	}

	// expand the watch.
	if(uiwatch.Expand() != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: can't expand watch \"%s\" in watch window.", THIS_FUNCTION, szWatch);
		return FALSE;
	}

	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COWatch::CollapseWatch(LPCSTR szWatch)
// Description: Collapses a watch on an expression in the debugger.
// Return: A Boolean value that specifies whether the operation was sucessful (TRUE) or not (FALSE).
// Param: szWatch A string specifying the name of the expression for which to collapse a watch.
// END_HELP_COMMENT
BOOL COWatch::CollapseWatch(LPCSTR szWatch)

{
	const char* const THIS_FUNCTION = "COWatch::CollapseWatch()";

	// activate the watch window.
	// TODO(michma): UIWatch::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uiwatch.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate watch window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uiwatch.Activate();

	// select the watch to expand.
	if(!uiwatch.SelectWatch(szWatch))
	{
		LOG->RecordInfo("ERROR in %s: can't select watch \"%s\" in watch window.", THIS_FUNCTION, szWatch);
		return FALSE;
	}

	// collapse the watch.
	if(uiwatch.Collapse() != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: can't expand watch \"%s\" in watch window.", THIS_FUNCTION, szWatch);
		return FALSE;
	}

	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return TRUE;
}


#pragma warning (default : 4069)

