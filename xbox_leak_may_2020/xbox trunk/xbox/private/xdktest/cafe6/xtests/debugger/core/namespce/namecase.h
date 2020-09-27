///////////////////////////////////////////////////////////////////////////////
//	Namecase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CNameSpceIDETest Class
//

#ifndef __Namecase_H__
#define __Namecase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "namesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CNameSpceIDETest class

class CNameSpceIDETest : public CDbgTestBase
{
	DECLARE_TEST(CNameSpceIDETest, CNameSpceSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);

	// Test cases
	private:
		void EvaluateNamespaceVarOutsideNamespace(void);
		void EvaluateNamespaceFuncOutsideNamespace(void);
		void EvaluateNestedNamespaceVarOutsideNamespace(void);
		void EvaluateNamespaceMemberInsideNamespace(void);
		void GoToLocationBpOnNamespaceFunc(void);
		void GoToDataBpOnNamespaceVar(void);
		void NavigateMemoryWndToNamespaceVar(void);
		void VerifyReturnValueInfoIncludesNamespace(void);
		void NavigateStackOutOfAndBackIntoNamespaceFunc(void);
		void ImplicitlyReferenceNamespaceMemberAfterUsingDirective(void);
		void ImplicitlyReferenceNamespaceMemberInsideNamespace(void);

	// Data
	private:
		EXPR_INFO expr_info_actual[4], expr_info_expected[4];
	};

#endif //__Namecase_H__
