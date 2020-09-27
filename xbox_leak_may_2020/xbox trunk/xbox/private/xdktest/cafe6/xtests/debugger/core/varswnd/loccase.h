///////////////////////////////////////////////////////////////////////////////
//	LOCCASE.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CLocalsCases Class
//

#ifndef __LOCCASE_H__
#define __LOCCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "varssub.h"

///////////////////////////////////////////////////////////////////////////////
//	CLocalsCases class

class CLocalsCases : public CDbgTestBase
{
	DECLARE_TEST(CLocalsCases, CVarsWndSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);

	// Test cases
	public:
		void VerifyLocPaneWhenVariablesChangeViaExec(void);
		void VerifyLocPaneWhenVariablesChangeViaMemoryWindow(void);
		void VerifyLocPaneWhenVariablesChangeViaWatchWindow(void);
		void VerifyLocPaneWhenVariablesChangeViaLocalsWindow(void);
		void VerifyLocPaneWhenVariablesChangeViaQuickWatchWindow(void);
		void VerifyThatFunctionArgumentsAppearWhenPrologReached(void);
		void VerifyThatLocalsAppearAfterExecutingProlog(void);
		void NavigateStackToParentFunctionAndBack(void);
		void CheckLocalsWhenSwitchToThisAutoAndBack(void);
		void VerifyThatLocalsUpdatesViaRegisterWindow(void);
		void StepThroughFunctionWithNoArguments(void);
		void StepThroughPrologWithThisPaneActive(void);

	// Data
	private:
		EXPR_INFO expr_info_actual[8], expr_info_expected[8];
		int m_TestNumber;
		int m_TotalNumberOfTests;
		CString m_strLogInfo;
	};

#endif //__LOCCASE_H__
