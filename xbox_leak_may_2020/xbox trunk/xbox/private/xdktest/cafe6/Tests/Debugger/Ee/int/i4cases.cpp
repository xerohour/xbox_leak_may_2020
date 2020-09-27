///////////////////////////////////////////////////////////////////////////////
//  I4CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/10/93
//
//  Description :
//      Implementation of the CInt4Cases class
//

#include "stdafx.h"
#include "i4cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt4Cases, CDebugTestSet, "Integer4", 12, CIntSubSuite)

												 
void CInt4Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt4Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((*pi & (*pc - l)) <= (*puc != ( ++ ul)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ul", 65001) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((( -- rint4) == *pi) - (**pps /= (uc && *pi)))", -13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("s", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint4", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((rint4 - ( - rint2)) ^ (*pl &= (**ppus %= **ppuc)))", 101) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("us", 104) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("l", 96) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((rint5 |= (uc ^ s)) - ((rint3 / ui) > rint1))", 141) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint5", 141) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((*pl != ((**ppui)-- )) + (i && (**ppl + **ppc)))", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ui", 99) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
