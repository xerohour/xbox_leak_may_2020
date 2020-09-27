///////////////////////////////////////////////////////////////////////////////
//	execase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CexeIDETest Class
//

#ifndef __execase2_H__
#define __execase2_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "exesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CexeIDETest class

class Cexe2IDETest : public CDbgTestBase
{
	DECLARE_TEST(Cexe2IDETest, CexeSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:

	// Go cases
	BOOL GoExeDoesNotExist();
	BOOL ExitWhileDebugging();

	// Step cases
	BOOL StepIntoMixed();
	BOOL StepOverMixed();
	BOOL StepOutMixed();
	BOOL StepToCursorMixed();

	BOOL DllStepTrace();
	BOOL DllStepOut();
	BOOL DllTraceOut();
	BOOL CannotFindDll();
	BOOL StepOverFuncInUnloadedDll();
	BOOL RestartFromWithinDll();
	BOOL GoFromWithinDll();
	BOOL StdInOutRedirection();
	BOOL NoSystemCodeStepping();

// Utilities
protected:
	BOOL CheckGeneralNetworkFailure();

};

#endif //__execase2_H__
