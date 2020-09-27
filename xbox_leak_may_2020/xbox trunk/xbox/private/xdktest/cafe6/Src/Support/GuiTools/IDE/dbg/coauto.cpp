///////////////////////////////////////////////////////////////////////////////
//  COAUTO.CPP
//
//  Created by:            Date:
//      MichMa                  9/16/98
//
//  Description:
//      Implementation of the COAuto class
//

#include "stdafx.h"
#include "coauto.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#pragma warning (disable : 4069)


// BEGIN_HELP_COMMENT
// Function: BOOL COAuto::VerifyAutoInfo(CAuto Auto)
// Description: Verify that an expression of the specified name, value, type, and state exists in the debugger's auto list.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: Auto A CAuto object containing the name, value, type, and state of the expression to verify.
// END_HELP_COMMENT
BOOL COAuto::VerifyAutoInfo(CAuto Auto)

{
	const char* const THIS_FUNCTION = "COAuto::VerifyAutoInfo";

	// activate the variables window.
	// TODO(michma): UIVariables::Activate isn't returning TRUE even though the window gets activated.
	/*if(!m_uivar.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate variables window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	m_uivar.Activate();

	// select the auto.
	if(!m_uivar.SelectAuto(Auto.m_strName))
	{
		LOG->RecordInfo("ERROR in %s: can't select auto \"%s\".", THIS_FUNCTION, Auto.m_strName);
		return FALSE;
	}
	
	// stores the auto's actual and expected information.
	EXPR_INFO ActualAutoInfo, ExpectedAutoInfo;
	m_coee.FillExprInfo(ExpectedAutoInfo, Auto.m_ExprState, Auto.m_strType, Auto.m_strName, Auto.m_strValue);

	// get the fields of the auto from the variables window.
	if(m_uivar.GetAllFields(&ActualAutoInfo, ROW_CURRENT) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: couldn't get fields of auto \"%s\".", THIS_FUNCTION, Auto.m_strName);
		return FALSE;
	}

	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	// compare the actual auto info with the expected auto info.
	return m_coee.CompareExprInfo(&ActualAutoInfo, &ExpectedAutoInfo);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COAuto::VerifyAutoInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState)
// Description: Verify that an expression of the specified name, value, type, and state exists debugger's auto list.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szName - A string specifying the name of the expression.
// Param: szValue - A string specifying the value of the expression.
// Param: szType - A string specifying the type of the expression.
// Param: ExprState - An EXPR_STATE value specifying the expansion state of the expression (NOT_EXPANABLE, COLLAPSED, EXPANDED).
// END_HELP_COMMENT
BOOL COAuto::VerifyAutoInfo(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState)
{
	CAuto Auto;
	Auto.m_ExprState = ExprState;
	Auto.m_strType = szType;
	Auto.m_strName = szName;
	Auto.m_strValue = szValue;
	return VerifyAutoInfo(Auto);
}


// BEGIN_HELP_COMMENT
// Function: BOOL COAuto::AutoDoesNotExist(LPCSTR szName)
// Description: Verify that an expression of the specified name does not exist in the debugger's auto list.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szName A pointer to a string that contains the name of the expression to check for.
// END_HELP_COMMENT
BOOL COAuto::AutoDoesNotExist(LPCSTR szName)

{
	const char* const THIS_FUNCTION = "COAuto::AutoDoesNotExist";

	// activate the variables window.
	// TODO(michma): UIVariables::Activate isn't returning TRUE even though the window gets activated.
	/*if(!m_uivar.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate variables window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	m_uivar.Activate();
	
	BOOL bResult = m_uivar.SelectAuto(szName);
	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return !bResult;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COAuto::AutoCountIs(int intAutoCount)
// Description: Verify that the debugger has an accurate count of the expressions in its auto list.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: intAutoCount An integer specifying the expected count of items in the auto list.
// END_HELP_COMMENT
BOOL COAuto::AutoCountIs(int intAutoCount)

{
	const char* const THIS_FUNCTION = "COAuto::AutoCountIs";

	// activate the variables window.
	// TODO(michma): UIVariables::Activate isn't returning TRUE even though the window gets activated.
	/*if(!m_uivar.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate variables window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	m_uivar.Activate();

	int intActualAutoCount = m_uivar.GetAutoCount();
	LOG->RecordInfo("%s: actual auto count is %d.", THIS_FUNCTION, intActualAutoCount);
	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return intActualAutoCount == intAutoCount;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COAuto::SetAutoValue(LPCSTR szName, LPCSTR szValue)
// Description: Sets a value for the specified variable in the debugger's auto list.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szName A string specifying the name of the auto variable for which to assign a value.
// Param: szValue A string specifying the value to be assigned to the auto variable.
// END_HELP_COMMENT
BOOL COAuto::SetAutoValue(LPCSTR szName, LPCSTR szValue)

{
	const char* const THIS_FUNCTION = "COAuto::SetAutoValue";

	// activate the variables window.
	// TODO(michma): UIVariables::Activate isn't returning TRUE even though the window gets activated.
	/*if(!m_uivar.Activate())
	{
		LOG->RecordInfo("ERROR in %s: can't activate variables window.", THIS_FUNCTION);
		return FALSE;
	}
	*/
	m_uivar.Activate();

	// select the auto to change.
	if(!m_uivar.SelectAuto(szName))
	{
		LOG->RecordInfo("ERROR in %s: can't select auto \"%s\".", THIS_FUNCTION, szName);
		return FALSE;
	}

	// Change the auto value
	if(ERROR_SUCCESS != m_uivar.SetValue(szValue, ROW_CURRENT))
	{
		LOG->RecordInfo("ERROR in %s: can't set the value of auto \"%s\".", THIS_FUNCTION, szName);
		return FALSE;
	}
	
	// return focus back to source (if it's there).
	MST.DoKeys("{ESC}");
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COAuto::VerifyReturnValue(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState)
// Description: Verify that a return value of the specified function name, value, type, and state exists debugger's auto list.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: szName - A string specifying the name of the function that returned.
// Param: szValue - A string specifying the value of the return.
// Param: szType - A string specifying the type of the return value.
// Param: ExprState - An EXPR_STATE value specifying the expansion state of the return value (NOT_EXPANABLE, COLLAPSED, EXPANDED).
// END_HELP_COMMENT
BOOL COAuto::VerifyReturnValue(LPCSTR szName, LPCSTR szValue,  LPCSTR szType, EXPR_STATE ExprState)
{
	CAuto Auto;
	Auto.m_ExprState = ExprState;
	Auto.m_strType = szType;
	Auto.m_strName = (CString)szName + " " + GetLocString(IDSS_RETURNED);
	Auto.m_strValue = szValue;
	return VerifyAutoInfo(Auto);
}

#pragma warning (default : 4069)

