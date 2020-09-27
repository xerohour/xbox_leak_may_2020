///////////////////////////////////////////////////////////////////////////////
//  I9CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Implementation of the CInt9Cases class
//

#include "stdafx.h"
#include "i9cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt9Cases, CDebugTestSet, "Integer9", 13, CIntSubSuite)

												 
void CInt9Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt9Cases::Run(void)

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

 	WriteLog(cxx.ExpressionValueIs("((*pl > (i / **pps)) & (**ppul ^= (**ppi | s)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ul", 64999) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((*pui += (i < i)) == (*pc %= (*pul += **ppl)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ul", 65096) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 20) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ui", 100) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((*pc % s) % *pui) >= (rint3 >>= (**ppuc & rint4)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint3", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppul -= (rint4 | *pus)) * ((l <= *ps) != rint5))", 32092) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ul", 32092) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((*pus -= (rint3 < **ppui)) && (*pc >>= (s > *puc)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 20) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("us", 32999) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
