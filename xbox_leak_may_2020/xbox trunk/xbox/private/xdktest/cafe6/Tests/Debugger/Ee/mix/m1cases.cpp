///////////////////////////////////////////////////////////////////////////////
//  M1CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix1Cases class
//

#include "stdafx.h"
#include "m1cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix1Cases, CDebugTestSet, "Mixed1", 12, CMixSubSuite)


void CMix1Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix1Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((((stlong4)++ ) && stlong7) << (stlong2 < (stlong5 / stlong3)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong8 - ( ++ f4)) != (stlong6 >>= (stlong4 >>= stlong3)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong6", 6) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("f4", 5.40000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong3 |= (stlong4 + stlong6)) >> (stlong1 * ( -- stlong5)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((stlong1 % stlong5) + f3) / ((stlong8 / stlong3) | stlong1))", 4.30000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong1 *= (stlong2 != stlong3)) | ((stlong2 ^ stlong7) ^ stlong2))", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong1", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
