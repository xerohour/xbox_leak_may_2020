///////////////////////////////////////////////////////////////////////////////
//	exe_bp.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CexeIDETest Class
//

#ifndef __exe_bp_H__
#define __exe_bp_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "exesub.h"

///////////////////////////////////////////////////////////////////////////////
//	Cexe_bpIDETest class

class Cexe_bpIDETest : public CDbgTestBase
{
	DECLARE_TEST(Cexe_bpIDETest, CexeSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:

	BOOL StepOverFuncToBreakAtLocBP();
	BOOL StepOverFuncToBreakOnExprTrueBP();
	BOOL StepOverFuncToBreakOnExprChangesBP();
	BOOL BreakStepTrace();
	BOOL StepBPLineSRC();
	BOOL StepBPLineDAM();
};

#endif //__exe_bp_H__
