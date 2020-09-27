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

// Data
private:
	// the base name of the localized directories and files we will use.
	CString m_strProjBase;
	// the location of the unlocalized sources, under which the projects we will use will be located.
	CString m_strSrcDir;
	// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
	CString m_strExeDir;


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
