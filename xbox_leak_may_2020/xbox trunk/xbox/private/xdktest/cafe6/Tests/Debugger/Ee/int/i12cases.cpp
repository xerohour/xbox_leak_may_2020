///////////////////////////////////////////////////////////////////////////////
//  I12CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Implementation of the CInt12Cases class
//

#include "stdafx.h"
#include "i12cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt12Cases, CDebugTestSet, "Integer12", 14, CIntSubSuite)

												 
void CInt12Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt12Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("(((**ppl < **ppui) ^ *pul) != (c / ( - c)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((c /= (**ppi + *pl)) % (*pus = (**ppuc += **ppi)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("uc", 138) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("us", 138) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppuc || (**ppuc == ul)) | (c = ((l)-- )))", 97) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("l", 96) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 97) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((rint1 -= (rint3 %= us)) * (s += (rint4 %= uc)))", -34) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint4", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("s", 17) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint3", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint1", -2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((uc < (*pi % rint4)) <= (rint4 < (**pps % ul)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	
	cxx.Disable();
	}
