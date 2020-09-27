///////////////////////////////////////////////////////////////////////////////
//	bpcase3.h
//
//	Created by:			Date:
//		MichMa				10/10/97
//
//	Description :
//		Declaration of the CBp3Cases Class
//

#ifndef __BPCASE3_H__
#define __BPCASE3_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "bpsub.h"


///////////////////////////////////////////////////////////////////////////////
//	CBp3Cases class

class CBp3Cases : public CDbgTestBase

{
	DECLARE_TEST(CBp3Cases, CbpSubSuite)

public:

	// Operations
	virtual void PreRun(void);
	virtual void Run();

	// Test Cases
	void BreakOnGlobalConstructorAndRestart(void);
	void BreakOnFunctionContainingScopeOperator(void);
	void BreakOnOverloadedSymbolSetBeforeStartDebugging(void);
	void BreakOnOverloadedSymbolSetAfterStartDebugging(void);
	void StepOverFunctionThatHitsBp(void);
	void BreakWhenAggregateExpressionChanges(void);
	void BreakOnMultipleBreakpoints(void);
	void BreakOnDllFuncThatWasUnloadedAfterSettingBp(void);
	void CompareGlobalAndLocalDataBpPerformance(void);
	void StepOutOfFunctionThatHitsBp(void);
	void StepToCursorThatHitsBp(void);
	//xbox void HitBpInDllSetBeforeStartDebugging(void);
	//xbox void HitBpInDllSetAfterStartDebugging(void);
	void HitLocationBpInExeSetWhileDebuggeeRunning(void);
	//xbox void HitLocationBpInDllSetWhileDebuggeeRunning(void);
	void HitDataBpInExeSetWhileDebuggeeRunning(void);
	void EditBpCodeWhenSourceIsInForeground(void);
	void EditBpCodeWhenSourceIsOpenButNotInForeground(void);
	void EditBpCodeWhenSourceIsNotOpen(void);
	void EditBpCodeWhenInputIsRequiredToFindSource(void);
	void HitBpWithPassCount(void);
	void HitBpAfterInterruptingPassCountAndRestarting(void);
	void HitBpSetInStack(void);
	void RemoveBpSetInStack(void);
	void HitBpWhenInputRequiredToFindSource(void);
	void InitTestState(void);
};

#endif //__BPCASE3_H__
