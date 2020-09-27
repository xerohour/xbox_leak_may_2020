///////////////////////////////////////////////////////////////////////////////
//  COEE.H
//
//  Created by :            Date :
//      MichMa                  1/13/94
//
//  Description :
//      Declaration of the COExpEval class
//

#ifndef __COEE_H__
#define __COEE_H__

#include "uiqw.h"
#include "uiwatch.h"
#include "uieewnd.h"

#include "dbgxprt.h"

#pragma warning (disable : 4069)


typedef enum{EE_VALUES_ONLY, EE_ALL_INFO} EE_TYPE;


///////////////////////////////////////////////////////////////////////////////
//  COExpEval class

// BEGIN_CLASS_HELP
// ClassName: COExpEval
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COExpEval

	{
	// Data
	private:
		UIQuickWatch m_uiqw;
		UIWatch	m_uiwatch;
		EE_TYPE m_eetype;
		BOOL m_bEEIsEnabled;

	// Utilities
	public:

		BOOL Enable(EE_TYPE eetype = EE_VALUES_ONLY);
		BOOL Disable();

		BOOL GetExpressionValue(LPCSTR expression, char *value);
		BOOL GetExpressionValue(LPCSTR expression, unsigned char *value);
		BOOL GetExpressionValue(LPCSTR expression, short int *value);
		BOOL GetExpressionValue(LPCSTR expression, unsigned short int *value);
		BOOL GetExpressionValue(LPCSTR expression, int *value);
		BOOL GetExpressionValue(LPCSTR expression, unsigned int *value);
		BOOL GetExpressionValue(LPCSTR expression, long int *value);
		BOOL GetExpressionValue(LPCSTR expression, unsigned long int *value);
		BOOL GetExpressionValue(LPCSTR expression, float *value);
		BOOL GetExpressionValue(LPCSTR expression, double *value);
		BOOL GetExpressionValue(LPCSTR expression, long double *value);
		BOOL GetExpressionValue(LPCSTR expression, CString &value);

		BOOL SetExpressionValue(LPCSTR expression, char value);
		BOOL SetExpressionValue(LPCSTR expression, unsigned char value);
		BOOL SetExpressionValue(LPCSTR expression, short int value);
		BOOL SetExpressionValue(LPCSTR expression, unsigned short int value);
		BOOL SetExpressionValue(LPCSTR expression, int value);
		BOOL SetExpressionValue(LPCSTR expression, unsigned int value);
		BOOL SetExpressionValue(LPCSTR expression, long int value);
		BOOL SetExpressionValue(LPCSTR expression, unsigned long int value);
		BOOL SetExpressionValue(LPCSTR expression, float value);
		BOOL SetExpressionValue(LPCSTR expression, double value);
		BOOL SetExpressionValue(LPCSTR expression, long double value);
		BOOL SetExpressionValue(LPCSTR expression, __int64 value);
		BOOL SetExpressionValue(LPCSTR expression, unsigned __int64 value);
		BOOL SetExpressionValue(LPCSTR expression, LPCSTR value);
		
		BOOL ExpressionValueIs(LPCSTR expression, char value);
		BOOL ExpressionValueIs(LPCSTR expression, unsigned char value);
		BOOL ExpressionValueIs(LPCSTR expression, short int value);
		BOOL ExpressionValueIs(LPCSTR expression, unsigned short int value);
		BOOL ExpressionValueIs(LPCSTR expression, int value);
		BOOL ExpressionValueIs(LPCSTR expression, unsigned int value);
		BOOL ExpressionValueIs(LPCSTR expression, long int value);
		BOOL ExpressionValueIs(LPCSTR expression, unsigned long int value);
		BOOL ExpressionValueIs(LPCSTR expression, float value);
		BOOL ExpressionValueIs(LPCSTR expression, double value);
		BOOL ExpressionValueIs(LPCSTR expression, long double value);
		BOOL ExpressionValueIs(LPCSTR expression, LPCSTR value);
		BOOL ExpressionValueSubstringIs(LPCSTR expression, LPCSTR value);

		BOOL ExpressionValueIsTrue(LPCSTR expression);
		BOOL ExpressionValueIsFalse(LPCSTR expression);
		BOOL ExpressionErrorIs(LPCSTR expression, LPCSTR value);

		BOOL ValueIsAddress(LPCSTR szValue);
		void FillExprInfo(EXPR_INFO &ExprInfo, EXPR_STATE ExprState, LPCSTR szType, LPCSTR szName, LPCSTR szValue);
		BOOL CompareExprInfo(EXPR_INFO *ExprInfo1, EXPR_INFO *ExprInfo2, int iCount = 1);
		BOOL VerifyExprInfo(EXPR_INFO expr_info);

	private:
		CString ExtractExpressionValue(LPCSTR expression,BOOL bErrorExpected=FALSE);
	};

#endif // __COEE_H__
