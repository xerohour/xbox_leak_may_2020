///////////////////////////////////////////////////////////////////////////////
//	AUTOCASE.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CAutoPaneIDETest Class
//

#ifndef __AUTOCASE_H__
#define __AUTOCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#include "..\dbgset.h"
#include "..\..\DbgTestBase.h"
#include "varssub.h"

///////////////////////////////////////////////////////////////////////////////
//	CNameSpceIDETest class

class CAutoPaneIDETest : public CDbgTestBase
{
	DECLARE_TEST(CAutoPaneIDETest, CVarsWndSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);

	// Test cases
	private:
		void VerifyUpdateAfterGo(void);
		void VerifyUpdateAfterStepInto(void);
		void VerifyUpdateAfterStepOut(void);
		void VerifyUpdateAfterStepOver(void);
		void VerifyUpdateAfterRestart(void);
		void VerifyUpdateAfterBreak(void);
		void VerifyUpdateAfterRunToCursor(void);
		void VerifyUpdateAfterSetNextStatement(void);
		void ChangeValueViaExecution(void);
		void ChangeValueViaMemWnd(void);
		void ChangeValueViaWatchWnd(void);
		void ChangeValueViaQuickWatchDlg(void);
		void ChangeValueViaAutoPane(void);
		void VerifyFuncArgsAppearWhenPrologReached(void);
		void NavigateStackToParentFuncAndBack(void);
		void ChangeToThisPaneAndBack(void);
		void ChangeToLocalsPaneAndBack(void);
		void VerifyOnlyCurrentAndPrevStatementsAreScanned(void);
		void VerifyMultilineStatementsGetScannedBackTenLines(void);
		void ScanArraySubscript(void);
		void ScanCastsAndOpsInsideArraySubscript(void);
		void ScanSelfSubscriptedArray(void);
		void ScanIndirectAccessToClassVariableAndFunc(void);
		void ScanDirectAccessToClassVariableAndFunc(void);
		void ScanScopeOp(void);
		void ScanAddressOfOp(void);
		void ScanPointerToMember(void);
		void VerifyRegistersNotAddedInMixedMode(void);
		void VerifyNamesCantBeManuallyDeletedAddedOrEdited(void);
		void VerifyConstantExpressionsNotAdded(void);
		void VerifyFuncCallsNotAdded(void);
		void VerifyUnsupportedOpsNotAdded(void);
		void StepIntoContextOfNoExpressions(void);
		void VerifyExpressionsInCommentsNotAdded(void);
		void VerifyDupExpressionsNotAdded(void);
		void VerifyExpressionsWithErrorValsNotAdded(void);

	// Data
	private:
		EXPR_INFO expr_info_actual[8], expr_info_expected[8];
	};

#endif //__AUTOCASE_H__
