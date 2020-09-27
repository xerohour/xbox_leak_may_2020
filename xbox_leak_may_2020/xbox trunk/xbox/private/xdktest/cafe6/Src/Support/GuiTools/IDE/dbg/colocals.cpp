///////////////////////////////////////////////////////////////////////////////
//  COLOCALS.CPP
//
//  Created by:            Date:
//      MichMa                  10/26/97
//
//  Description:
//      Implementation of the COLocals class
//

#include "stdafx.h"
#include "colocals.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#pragma warning (disable : 4069)


// BEGIN_HELP_COMMENT
// Function: BOOL COLocals::VerifyLocalInfo(CLocal Local)
// Description: Verify that a local of the specified name, value, type, and state exists in the current frame of the app being debugged.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: Local A CLocal object containing the name, value, type, and state of the local to verify.
// END_HELP_COMMENT
BOOL COLocals::VerifyLocalInfo(CLocal Local)

{
	// activate the variables window.
	// TODO(michma): UIVariables::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uivar.Activate())
	{
		LOG->RecordInfo("ERROR in COLocals::VerifyLocalInfo(): can't activate variables window.", Local.m_strName);
		return FALSE;
	}
	*/
	uivar.Activate();

	// select the local.
	if(!uivar.SelectLocal(Local.m_strName))
	{
		LOG->RecordInfo("ERROR in COLocals::VerifyLocalInfo(): can't select local \"%s\".", Local.m_strName);
		return FALSE;
	}
	
	// stores the local's actual and expected information.
	EXPR_INFO ActualLocalInfo, ExpectedLocalInfo;
	coee.FillExprInfo(ExpectedLocalInfo, Local.m_ExprState, Local.m_strType, Local.m_strName, Local.m_strValue);

	// get the fields of the local from the variables window.
	if(uivar.GetAllFields(&ActualLocalInfo, ROW_CURRENT) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in COLocals::VerifyLocalInfo(): couldn't get fields of local.");
		return FALSE;
	}

	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	// compare the actual local info with the expected local info.
	return coee.CompareExprInfo(&ActualLocalInfo, &ExpectedLocalInfo);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COLocals::VerifyLocalInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState)
// Description: Verify that a local of the specified name, value, type, and state exists in the current frame of the app being debugged.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szName - A string specifying the name of the local.
// Param: szValue - A string specifying the value of the local.
// Param: szType - A string specifying the type of the local.
// Param: ExprState - An EXPR_STATE value specifying the expansion state of the local (NOT_EXPANABLE, COLLAPSED, EXPANDED).
// END_HELP_COMMENT
BOOL COLocals::VerifyLocalInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState)
{
	CLocal Local;
	Local.m_ExprState = ExprState;
	Local.m_strType = szType;
	Local.m_strName = szName;
	Local.m_strValue = szValue;
	return VerifyLocalInfo(Local);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COLocals::LocalDoesNotExist(LPCSTR szLocal)
// Description: Verify that a local of the specified name does not exist.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szLocal A pointer to a string that contains the local to check for.
// END_HELP_COMMENT
BOOL COLocals::LocalDoesNotExist(LPCSTR szLocal)

{
	// activate the variables window.
	// TODO(michma): UIVariables::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uivar.Activate())
	{
		LOG->RecordInfo("ERROR in COLocals::VerifyLocalInfo(): can't activate variables window.", Local.m_strName);
		return FALSE;
	}
	*/
	uivar.Activate();
	
	BOOL bResult = uivar.SelectLocal(szLocal);
	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return !bResult;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COLocals::LocalsCountIs(int intLocalsCount)
// Description: Verify that the debugger has an accurate count of the locals in the current function.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: intLocalCount An integer specifying the expected count of locals.
// END_HELP_COMMENT
BOOL COLocals::LocalsCountIs(int intLocalsCount)

{
	// activate the variables window.
	// TODO(michma): UIVariables::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uivar.Activate())
	{
		LOG->RecordInfo("ERROR in COLocals::VerifyLocalInfo(): can't activate variables window.", Local.m_strName);
		return FALSE;
	}
	*/
	uivar.Activate();

	int intActualLocalsCount = uivar.GetLocalsCount();
	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return intActualLocalsCount == intLocalsCount;
}

// BEGIN_HELP_COMMENT
// Function: BOOL COLocals::SetLocalValue(LPCSTR szLocal, LPCSTR szValue)
// Description: Sets a value for the local variable.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szLocal A string specifying the name of the local value expression for which to assign a value
// Param: szValue A string specifying the value to be assigned to the local variable
// END_HELP_COMMENT
BOOL COLocals::SetLocalValue(LPCSTR szLocal, LPCSTR szValue)

{
	const char* const THIS_FUNCTION = "COLocals::SetLocalValue";

	// activate the variables window.
	// TODO(michma): UIVariables::Activate isn't returning TRUE even though the window gets activated.
	/*if(!uivar.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate variables window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	uivar.Activate();

	// select the local to change.
	if(!uivar.SelectLocal(szLocal))
	{
		LOG->RecordInfo("ERROR in %s: can't select local \"%s\".", THIS_FUNCTION, szLocal);
		return FALSE;
	}

	// Change the local value
	if(ERROR_SUCCESS != uivar.SetValue(szValue, ROW_CURRENT))
	{
		LOG->RecordInfo("ERROR in %s: can't set the value of local \"%s\".", THIS_FUNCTION, szLocal);
		return FALSE;
	}
	
	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return TRUE;
}

#pragma warning (default : 4069)

