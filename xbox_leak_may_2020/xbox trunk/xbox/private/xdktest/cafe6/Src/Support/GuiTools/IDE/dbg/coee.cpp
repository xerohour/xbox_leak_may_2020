///////////////////////////////////////////////////////////////////////////////
//  COEE.CPP
//
//  Created by :            Date :
//      MichMa                  1/13/94
//
//  Description :
//      Implementation of the COExpEval class
//

#include "stdafx.h"
#include "coee.h"
#include "guiv1.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#pragma warning (disable : 4069)


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::Enable(EE_TYPE eetype /* EE_VALUES_ONLY */)
// Description: Enable the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COExpEval::Enable(EE_TYPE eetype /* EE_VALUES_ONLY */)

{
	// if we're only checking values, then we can use the quickwatch dlg. otherwise we need the watch window.
	if(eetype == EE_VALUES_ONLY)
	{
		if (!m_uiqw.Activate())
			return FALSE;
	}
	else
		// TODO(michma - 2/7/98): need to check return value when we get Activate working for watch window.
		m_uiwatch.Activate();

	// store the current EE_TYPE for later.
	m_eetype = eetype;
	m_bEEIsEnabled = TRUE;
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::Disable()
// Description: Disable the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COExpEval::Disable()

{
	// if we're using the quickwatch dlg to evaluate expressions, then we want to close it when finished.
	// if we're using the watch window, we want to leave it open.
	if(m_eetype == EE_VALUES_ONLY)
	{
		if(m_uiqw.Close() != NULL) 
			return FALSE;
	}
	else
		MST.DoKeys("{ESC}");

	m_bEEIsEnabled = FALSE;
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, char *value)
// Description: Get the result (one charachter) of the given expression of type char from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a char that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, char *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);

	if(cstr == "")
		return FALSE;

	*value = (char)strtol(cstr, NULL, 0);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, unsigned char *value)
// Description: Get the result (one character) of the given expression of type unsigned char from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to an unsigned char that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, unsigned char *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);

	if(cstr == "")
		return FALSE;

	*value = (unsigned char)strtoul(cstr, NULL, 0);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, short int *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a short integer that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, short int *value)

	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = (short int)strtol(cstr, NULL, 0);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, unsigned short int *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to an unsigned short integer that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, unsigned short int *value)

	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = (unsigned short int)strtoul(cstr, NULL, 0);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, int *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to an integer that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, int *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = (int)strtol(cstr, NULL, 0);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, unsigned int *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to an unsigned integer that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, unsigned int *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = (unsigned int)strtoul(cstr, NULL, 0);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, long int *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a long that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, long int *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = strtol(cstr, NULL, 0);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, unsigned long int *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to an unsigned long that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, unsigned long int *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = strtoul(cstr, NULL, 0);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, float *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a float that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, float *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = (float)atof(cstr);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, double *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a double that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, double *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = (double)atof(cstr);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, long double *value)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a long double that will contain the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, long double *value)
	
	{
	CString cstr = ExtractExpressionValue(expression);
	
	if(cstr == "")
		return FALSE;

	*value = atof(cstr);
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::GetExpressionValue(LPCSTR expression, short int *value)
// Description: Get the result of the string expression from the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A string that will contain the result of the expression evaluation. It will contain the exprassion as it's displayed in the debugger including the quotes and/or braces (ex. {"string"})
// END_HELP_COMMENT
BOOL COExpEval::GetExpressionValue(LPCSTR expression, CString &value)

	{
	CString cstr = ExtractExpressionValue(expression);
	
	value = cstr;

	if(cstr == "")
		return FALSE;

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: CString COExpEval::ExtractExpressionValue(LPCSTR expression, BOOL bErrorExpected /* FALSE */)
// Description: Get the result of the given expression from the expression evaluator.
// Return: A CString that contains the result of the expression evaluation.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: bErrorExpected A Boolean value that indicates whether to expect an error (TRUE) during evaluation or not.
// END_HELP_COMMENT
CString COExpEval::ExtractExpressionValue(LPCSTR expression, BOOL bErrorExpected /* FALSE */)
	
	{
	int EEisActive = m_uiqw.IsActive();

	if(!EEisActive)
		if(!Enable()) return CString("");

	if(!m_uiqw.SetExpression(expression)) return CString("");
	if(!m_uiqw.Recalc()) return CString("");
	CString value = m_uiqw.GetCurrentValue();
	if(value == "") return value;

	if(!EEisActive)
		if(!Disable()) return CString("");

	if(value.Find("Error") == -1 || (bErrorExpected))
		{
		LOG->RecordInfo("EE - extracted value = %s", value);
		return value;
		}

	
	LOG->RecordInfo("EE ERROR - %s\n", value);
	return CString("");

	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, char value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value A char that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, char value)
	{
	CString cstr = value;
	return SetExpressionValue(expression, cstr);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned char value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value An unsigned char that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned char value)
	{
	CString cstr = value;
	return SetExpressionValue(expression, cstr);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, short int value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value A short that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, short int value)
	{
	char string[256];
	return SetExpressionValue(expression, _itoa(value, string, 10));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned short int value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value An unsigned short that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned short int value)
	{
	char string[256];
	return SetExpressionValue(expression, _itoa((signed int)value, string, 10));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, int value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value An integer that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, int value)
	{
	char string[256];
	return SetExpressionValue(expression, _itoa(value, string, 10));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned int value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value An unsigned integer that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned int value)
	{
	char string[256];
	return SetExpressionValue(expression, _itoa((signed)value, string, 10));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, long int value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value A long that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, long int value)
	{
	char string[256];
	return SetExpressionValue(expression, _ltoa(value, string, 10));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned long int value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value An unsigned long that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned long int value)
	{
	char string[256];
	return SetExpressionValue(expression, _ltoa((signed)value, string, 10));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, float value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value A float that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, float value)
	{
	char string[256];
	return SetExpressionValue(expression, _gcvt(value, 6, string));
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, double value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value A double that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, double value)
	{
	char string[256];
	return SetExpressionValue(expression, _gcvt(value, 14, string));	
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, long double value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value A long double that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, long double value)
	{
	char string[256];
	return SetExpressionValue(expression, _gcvt(value, 14 , string));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, __int64 value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value An __int64 that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, __int64 value)
	{
	char string[256];
	return SetExpressionValue(expression, _i64toa(value, string, 10));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned __int64 value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value An __int64 that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, unsigned __int64 value)
	{
	char string[256];
	return SetExpressionValue(expression, _i64toa(value, string, 10));
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::SetExpressionValue(LPCSTR expression, LPCSTR value)
// Description: Set an expression's value in the expression evaluator.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: expression A pointer to a string that contains the expression to set.
// Param: value A pointer to a string that contains the value to set the expression to.
// END_HELP_COMMENT
BOOL COExpEval::SetExpressionValue(LPCSTR expression, LPCSTR value)
	
	{
	int EEisActive = m_uiqw.IsActive();

	if(!EEisActive)
		if(!Enable()) return FALSE;

	if(!m_uiqw.SetExpression(expression)) return FALSE;
	if(!m_uiqw.Recalc()) return FALSE;
	if(!m_uiqw.SetNewValue(value)) return FALSE;

	if(!EEisActive)
		if(!Disable()) return FALSE;
	
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, char value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A char that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, char value)
	{
	char eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, unsigned char value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value An unsigned char that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, unsigned char value)
	{
	unsigned char eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, short int value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A short that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, short int value)
	{
	short int eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, unsigned short int value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value An unsigned short that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, unsigned short int value)
	{
	unsigned short int eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, int value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value An integer that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, int value)
	{
	int eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, unsigned int value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value An unsigned integer that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, unsigned int value)
	{
	unsigned int eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, long int value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A long that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, long int value)
	{
	long int eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, unsigned long int value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value An unsigned long that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, unsigned long int value)
	{
	unsigned long int eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, float value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A float that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, float value)
	{
	float eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;

	char str1[128], str2[128];

	// Since comparing floats doesn't give correct results if one of them is a result of atof
	// we compare strings

	memset(str1, 0, sizeof(str1));
	memset(str2, 0, sizeof(str2));

	sprintf(str1, "%.5e", eevalue);
	sprintf(str2, "%.5e", value);
	return !strcmp(str1,str2);

//	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, double value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A double that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, double value)
	{
	double eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, long double value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A long double that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, long double value)
	{
	long double eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return value == eevalue;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIs(LPCSTR expression, LPCSTR value)
// Description: Determine whether the expression evaluates to the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a string that contains the expected result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIs(LPCSTR expression, LPCSTR value)

	{
	// evaluate the expression.
	CString eevalue = ExtractExpressionValue(expression);
	CString expected_value = value;

	// is an address (unknown value) expected at the beginning of the value?
	if(expected_value.Mid(0, 7) == "ADDRESS")
		
		{
		// verify that the first 10 characters of the extracted value are a non-zero address.
		if (!((eevalue.Mid(0, 2) == "0x") && 
		      (eevalue.Mid(2).SpanIncluding("0123456789abcdef").GetLength() == 8) &&
		      (eevalue.Mid(0, 10) != "0x00000000")))
			return FALSE;

		// is extra data besides the address expected?
		if(expected_value.Mid(0, 8) == "ADDRESS ")
		
			{
			// make sure actual string is long enough so we avoid CString assert.
			if(eevalue.GetLength() > 10)
				// verify extra data is correct.
				return eevalue.Mid(10) == expected_value.Mid(7);
			else
				// actual string wasn't long enough to contain extra data.
				return FALSE;
			}

		return TRUE;
		}
	
	return eevalue == expected_value;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueSubstringIs(LPCSTR expression, LPCSTR value)
// Description: Determine whether the expression evaluates to the string which contains the given value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the string which contains the specified value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a string that contains the expected substring of the result of the expression evaluation.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueSubstringIs(LPCSTR expression, LPCSTR value)

	{
	// evaluate the expression.
	CString eevalue = ExtractExpressionValue(expression);
	CString expected_value = value;

	// is an address (unknown value) expected at the beginning of the value?
	if(expected_value.Mid(0, 7) == "ADDRESS")
		
		{
		// verify that the first 10 characters of the extracted value are a non-zero address.
		if (!((eevalue.Mid(0, 2) == "0x") && 
		      (eevalue.Mid(2).SpanIncluding("0123456789abcdef").GetLength() == 8) &&
		      (eevalue.Mid(0, 10) != "0x00000000")))
			return FALSE;

		// is extra data besides the address expected?
		if(expected_value.Mid(0, 8) == "ADDRESS ")
		
			{
			// make sure actual string is long enough so we avoid CString assert.
			if(eevalue.GetLength() > 10)
				// verify extra data is correct.
				return (eevalue.Mid(10).Find(expected_value.Mid(7)) != -1);
			else
				// actual string wasn't long enough to contain extra data.
				return FALSE;
			}

		return TRUE;
		}
	
	return (eevalue.Find(expected_value) != -1);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIsTrue(LPCSTR expression)
// Description: Determine whether the given expression evaluates to TRUE in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to TRUE (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIsTrue(LPCSTR expression)
	{
	char eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return eevalue == 1;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionValueIsFalse(LPCSTR expression)
// Description: Determine whether the given expression evaluates to FALSE in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to FALSE (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionValueIsFalse(LPCSTR expression)
	{
	char eevalue;
	if (!GetExpressionValue(expression, &eevalue)) return FALSE;
	return eevalue == 0;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ExpressionErrorIs(LPCSTR expression, LPCSTR value)
// Description: Determine whether the given expression evaluates to the specified error value in the expression evaluator.
// Return: A Boolean value that indicates whether the given expression evaluates to the given error value (TRUE) or not.
// Param: expression A pointer to a string that contains the expression to evaluate.
// Param: value A pointer to a string that contains the error expected.
// END_HELP_COMMENT
BOOL COExpEval::ExpressionErrorIs(LPCSTR expression, LPCSTR value)
	{
	CString eevalue = ExtractExpressionValue(expression,TRUE);
	return eevalue == value;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::ValueIsAddress(LPCSTR szValue)
// Description: Reports whether or not a specified value represents an address.
// Return: TRUE if the value is an address, FALSE if not.
// Param: szValue A pointer to a string that contains the value to check.
// END_HELP_COMMENT
BOOL COExpEval::ValueIsAddress(LPCSTR szValue)
{
	CString strValue = szValue;
	return (strValue.Mid(0, 2) == "0x") && 
		   (strValue.Mid(2).SpanIncluding("0123456789abcdef").GetLength() == 8);
}


// BEGIN_HELP_COMMENT
// Function: void COExpEval::FillExprInfo(EXPR_INFO &ExprInfo, EXPR_STATE ExprState, LPCSTR szType, LPCSTR szName, LPCSTR szValue)
// Description: Fills a given EXPR_INFO structure with the supplied state, type, name, and value of an expression.
// Return: none.
// Param: ExprInfo A reference to an EXPR_INFO object that will be filled with the other parameters.
// Param: ExprState An EXPR_STATE enum value specifying the state of the expression (NOT_EXPANDABLE, EXPANDED, COLLAPSED).
// Param: szType A pointer to a string that contains the type of the expression.
// Param: szName A pointer to a string that contains the name of the expression.
// Param: szValue A pointer to a string that contains the value of the expression.
// END_HELP_COMMENT
void COExpEval::FillExprInfo(EXPR_INFO &ExprInfo, EXPR_STATE ExprState, LPCSTR szType, LPCSTR szName, LPCSTR szValue)
{
	ExprInfo.state = ExprState;
	ExprInfo.type = szType;
	ExprInfo.name = szName;
	ExprInfo.value = szValue;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::CompareExprInfo(EXPR_INFO *ExprInfo1, EXPR_INFO *ExprInfo2, int iCount /* 1 */)
// Description: Compares two arrays of EXPR_INFO structures.
// Return: A boolean value indicating whether or not the EXPR_INFO arrays match, element for element.
// Param: ExprInfo1 An EXPR_STATE array that is expected to match ExprInfo2 element for element.
// Param: ExprInfo2 An EXPR_STATE array that is expected to match ExprInfo1 element for element.
// Param: iCount An integer specifying how many elements to compare.
// END_HELP_COMMENT
BOOL COExpEval::CompareExprInfo(EXPR_INFO *ExprInfo1, EXPR_INFO *ExprInfo2, int iCount /* 1 */)
	
{
	// loop through all the elements to compare.
	for(int i = 0; i < iCount; i++)

	{
		// compare the states.
		if(ExprInfo1[i].state != ExprInfo2[i].state)
		{
			LOG->RecordInfo("ERROR in COExpEval::ExprInfoIs(): state is %d instead of %d", 
							ExprInfo1[i].state, ExprInfo2[i].state);
			return FALSE;
		}

		// compare the types.
		if(ExprInfo1[i].type != ExprInfo2[i].type)
		{
			LOG->RecordInfo("ERROR in COExpEval::ExprInfoIs(): type is \"%s\" instead of \"%s\"", 
							ExprInfo1[i].type, ExprInfo2[i].type);
			return FALSE;
		}

		// compare the names.
		if(ExprInfo1[i].name != ExprInfo2[i].name)
		{
			LOG->RecordInfo("ERROR in COExpEval::ExprInfoIs(): name is \"%s\" instead of \"%s\"", 
							ExprInfo1[i].name, ExprInfo2[i].name);
			return FALSE;
		}

		// make sure an unknown value isn't being compared.
		// "UNKNOWN" values must always be passed in the ExprInfo2 array.
		if(ExprInfo2[i].value.Mid(0, 7) != "UNKNOWN") 

		{
			BOOL bValueComparePassed;

			// check if an address is being copmared.
			// "ADDRESS" values must always be passed in the ExprInfo2 array.
			if(ExprInfo2[i].value.Mid(0, 7) == "ADDRESS")
				
			{
				// verify that the value is an address.
				bValueComparePassed = ValueIsAddress(ExprInfo1[i].value);

				// is extra data besides address expected? (ex. peek at string)
				if(ExprInfo2[i].value != "ADDRESS")
				
				{
					// user must supply extra data to avoid CString assert.
					EXPECT(ExprInfo2[i].value.GetLength() > 8);

					// make sure actual string is long enough so we avoid CString assert.
					if(ExprInfo1[i].value.GetLength() > 10)
					{
						// verify extra data is correct.
						if(ExprInfo1[i].value.Mid(10) ==  (" " + ExprInfo2[i].value.Mid(8)))
							bValueComparePassed = TRUE;	
					}
					else
						// actual string wasn't long enough to contain extra data.
						bValueComparePassed = FALSE;
				}	// extra data besides address expected?
			}	// address being compared?

			// straight value comparison (no address).
			else	
				bValueComparePassed = ExprInfo1[i].value == ExprInfo2[i].value;

			// check the value comparison.
			if(!bValueComparePassed)
			{
				LOG->RecordInfo("ERROR in COExpEval::ExprInfoIs(): value is \"%s\" instead of \"%s\"", 
								ExprInfo1[i].value, ExprInfo2[i].value);
				return FALSE;
			}
		
		}	// unknown value being compared?
	
	}	// loop through all the elements to compare.

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COExpEval::VerifyExprInfo(EXPR_INFO expr_info)
// Description: Verify the name, value, type, and state of an expression.
// Return: A Boolean value that specifies whether the verification was sucessful (TRUE) or not (FALSE).
// Param: expr_info A EXPR_INFO object containing the name, value, type, and state of the expression to verify.
// END_HELP_COMMENT
BOOL COExpEval::VerifyExprInfo(EXPR_INFO expr_info)

{
	char szThisFunc[] = "COExpEval::VerifyExprInfo";

	// enable the watch window if it hasn't been already.
	if(!m_bEEIsEnabled)
		// TODO(michma - 2/7/98): check for a return value when we get Activate working for the watch window.
		m_uiwatch.Activate();

	// add a watch on the expression.
	if(m_uiwatch.SetName(expr_info.name, ROW_LAST) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: can't add watch on expression \"%s\".", szThisFunc, expr_info.name);
		return FALSE;
	}
	
	// stores the expression's actual information.
	EXPR_INFO expr_info_actual;

	// get the fields of the expression from the watch window.
	if(m_uiwatch.GetAllFields(&expr_info_actual, ROW_PREVIOUS) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: couldn't get fields of expression \"%s\".", szThisFunc, expr_info.name);
		return FALSE;
	}

	// delete the watch so as not to clutter up the watch window.
	if(m_uiwatch.Delete(ROW_PREVIOUS) != ERROR_SUCCESS)
	{
		LOG->RecordInfo("ERROR in %s: couldn't delete expression \"%s\" from watch window.", szThisFunc, expr_info.name);
		return FALSE;
	}

	// compare the actual expression info with the expected expression info.
	return CompareExprInfo(&expr_info_actual, &expr_info);
}


#pragma warning (default : 4069)
