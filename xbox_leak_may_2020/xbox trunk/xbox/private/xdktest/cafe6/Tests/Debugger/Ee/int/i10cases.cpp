///////////////////////////////////////////////////////////////////////////////
//  I10CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Implementation of the CInt10Cases class
//

#include "stdafx.h"
#include "i10cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt10Cases, CDebugTestSet, "Integer10", 10, CIntSubSuite)

												 
void CInt10Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt10Cases::Run(void)

	{
	if(!fProjectReady)
		{
		InitProject("int");
		bps.SetBreakpoint("break_here");
		fProjectReady = TRUE;
		}
	else
		dbg.Restart();

	dbg.Go();
	cxx.Enable();

	WriteLog(cxx.ExpressionValueIs("((**ppus &= (s & s)) > ((rint5 ^ *pc) ^ **ppc))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("us", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((((**pps)++ ) + *puc) + (**ppc += (**ppuc != **ppuc)))", 161) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 20) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("s", 14) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((( ! **ppul) != ul) >= (*puc &= (**ppui - rint5)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("uc", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((us + *pl) != *pc) & ((rint5 || *pui) || ui))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((l += (i / rint4)) * (rint1 ^ (uc > **ppuc)))", 99) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("l", 99) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
