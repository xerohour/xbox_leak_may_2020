///////////////////////////////////////////////////////////////////////////////
//  M4CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix4Cases class
//

#include "stdafx.h"
#include "m4cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix4Cases, CDebugTestSet, "Mixed4", 12, CMixSubSuite)


void CMix4Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix4Cases::Run(void)

	{
	if(!fProjectReady)
		{
		InitProject("mix");
		bps.SetBreakpoint("break_here");
		fProjectReady = TRUE;
		}
	else
		dbg.Restart();

	dbg.Go();
	cxx.Enable();

 	WriteLog(cxx.ExpressionValueIs("((stlong7 & (stlong2 - stlong8)) <= (stlong6 != ( ++ *pd1)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d1", 2.1000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((( -- *pd3) == f1) - (stlong1 /= (stlong7 && stlong8)))", -1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong1", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d3", 2.3000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong5 - ( - stlong8)) ^ (stlong7 &= (stlong4 %= stlong1)))", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong7", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong6 |= (stlong3 ^ stlong3)) - ((*pf1 / f2) > d1))", 6) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong6", 6) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((f4 != ((*pd1)-- )) + (*pd3 && (*pf2 + f3)))", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d1", 1.10000) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
