///////////////////////////////////////////////////////////////////////////////
//	exe_base.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CexeIDETest Class
//

#ifndef __exe_base_H__
#define __exe_base_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "exesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CexeIDETest class

class Cexe_baseIDETest : public CDbgTestBase
{
	DECLARE_TEST(Cexe_baseIDETest, CexeSubSuite)

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
	BOOL StepModel();
	BOOL StaticFuncStepTrace();

};

#endif //__exe_base_H__
