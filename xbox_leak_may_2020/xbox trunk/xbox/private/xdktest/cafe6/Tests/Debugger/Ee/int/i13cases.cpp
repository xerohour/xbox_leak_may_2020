///////////////////////////////////////////////////////////////////////////////
//  I13CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Implementation of the CInt13Cases class
//

#include "stdafx.h"
#include "i13cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt13Cases, CDebugTestSet, "Integer13", 11, CIntSubSuite)

												 
void CInt13Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt13Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((l && ((i)-- )) == (us ^= ( ! rint5)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("us", 33000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("i", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppus | (**ppui & l)) * ((*pui * rint2) >= *pul))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppc /= (rint2 * **ppui)) <= (s += (rint3 < rint5)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("s", 14) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((( ! i) & **ppui) >> ((uc > *puc) && *pui))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppc *= (**ppui |= rint5)) >= ((*puc % s) == rint2))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ui", 101) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	
	cxx.Disable();
	}
