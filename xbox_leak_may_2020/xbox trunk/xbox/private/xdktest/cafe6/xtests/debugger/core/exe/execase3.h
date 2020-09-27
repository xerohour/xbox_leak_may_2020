///////////////////////////////////////////////////////////////////////////////
//	EXECASE3.H
//
//	Created by:			Date:
//		MichMa				2/2/98
//
//	Description :
//		Declaration of the CExe3Cases Class
//

#ifndef __EXECASE3_H__
#define __EXECASE3_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "exesub.h"


///////////////////////////////////////////////////////////////////////////////
//	CExe3Cases class

class CExe3Cases : public CDbgTestBase

{
	DECLARE_TEST(CExe3Cases, CexeSubSuite)

public:

	// Operations
	virtual void PreRun(void);
	virtual void Run(void);

	// Utils
	void InitTestState(void);

	// Test Cases
	void StartDebuggingBreakRestartAndStep(void);
	void StepIntoFuncThroughIndirectCall(void);
	void StepIntoWhenInstructionPtrAdvancedButNotSourcePtr(void);
	void StepIntoWhenInstructionAndSourcePtrsAdvanceWithFocusInAsm(void);
	void StepIntoWhenInstructionAndSourcePtrsAdvanceWithFocusInSource(void);
	void StepOverToLineWithLocationBP(void);
	void StepOverFuncInMixedMode(void);
	void StepOverFuncWhichWillHitMessageBP(void);
	void StepOutOfFuncWhichWillHitExpressionTrueBP(void);
	void StepOutOfFuncWhichWillHitExpressionChangedBP(void);
	void StepOutOfFuncWhichWillHitMessageBP(void);
	void GoUntilExceptionIsRaised(void);
	void StopDebuggingWhileDebuggeeIsRunning(void);
	void StepToCursorInAsm(void);
	void StepToCursorAtLineWithLocationBP(void);
	void SetNextStatementInSource(void);
	void SetNextStatementInAsm(void);
	void ShowNextStatementWhenCursorNotOnStatementAndSourceInForeground(void);
	void ShowNextStatementWhenScrollingRequiredAndSourceInForeground(void);
	void ShowNextStatementInAsm(void);
	void StepIntoSpecificNonNested(void);
	void StepIntoSpecificNested(void);
	void StepIntoSpecificWhenBPSetInInnerFunc(void);
	void LoadCOFFExports(void);
};

#endif //__EXECASE3_H__
