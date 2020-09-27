///////////////////////////////////////////////////////////////////////////////
//	execase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CexeIDETest Class
//

#ifndef __execase_H__
#define __execase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "exesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CexeIDETest class

class CexeIDETest : public CDbgTestBase
{
	DECLARE_TEST(CexeIDETest, CexeSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:

	// Go cases
	BOOL GoAndVerify();
	BOOL RestartAndGo();
	BOOL RestartStopDebugging();

	// Step cases
	BOOL StepIntoSource();
	BOOL StepOverSource();
	BOOL StepOutSource();
	BOOL StepToCursorSource();
	BOOL StepOverFuncToBreakAtLocBP();
	BOOL StepOverFuncToBreakOnExprTrueBP();
	BOOL StepOverFuncToBreakOnExprChangesBP();
	BOOL TraceIntoWndProc();

	BOOL StepModel();
	BOOL BreakStepTrace();

};

#endif //__execase_H__
