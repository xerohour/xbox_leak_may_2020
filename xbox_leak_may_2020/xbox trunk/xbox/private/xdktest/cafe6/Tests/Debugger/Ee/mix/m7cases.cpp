///////////////////////////////////////////////////////////////////////////////
//  M7CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix7Cases class
//

#include "stdafx.h"
#include "m7cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix7Cases, CDebugTestSet, "Mixed7", 11, CMixSubSuite)


void CMix7Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix7Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((stlong1 != (d1 < *pd3)) || (stlong7 ^= (stlong2 /= stlong6)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong2", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong7", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong3 << ( ! stlong8)) - (stlong4 |= (stlong6 && stlong3)))", -2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong2 & (stlong7 * stlong6)) < (( - stlong4) % stlong3))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((stlong1 ^ stlong4) && f4) - (d1 = ((stlong5)-- )))", -4.0000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d1", 5.0000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((f2 /= (stlong2 - *pd1)) + (stlong8 && (d2 + *pf1)))", 0.560000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("f2", -0.440000) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
