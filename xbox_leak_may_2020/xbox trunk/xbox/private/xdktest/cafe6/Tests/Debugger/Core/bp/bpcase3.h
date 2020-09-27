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

#include "..\dbgset.h"
#include "bpsub.h"


///////////////////////////////////////////////////////////////////////////////
//	CBp3Cases class

class CBp3Cases : public CDebugTestSet

{
	DECLARE_TEST(CBp3Cases, CbpSubSuite)

// Data
private:
	// the base name of the localized directories and files we will use.
	CString m_strProjBase;
	// the location of the unlocalized sources, under which the projects we will use will be located.
	CString m_strSrcDir;
	// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
	CString m_strDllDir;
	CString m_strExeDir;

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
	void HitBpInDllSetBeforeStartDebugging(void);
	void HitBpInDllSetAfterStartDebugging(void);
	void HitLocationBpInExeSetWhileDebuggeeRunning(void);
	void HitLocationBpInDllSetWhileDebuggeeRunning(void);
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
};

#endif //__BPCASE3_H__
