///////////////////////////////////////////////////////////////////////////////
//  M8CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix1Cases class
//

#include "stdafx.h"
#include "m8cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix8Cases, CDebugTestSet, "Mixed8", 17, CMixSubSuite)


void CMix8Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix8Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((stlong2 /= (stlong1 + stlong4)) % (stlong8 /= (stlong3 &= stlong6)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong2", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong7 %= (stlong2 + stlong3)) < (stlong4 >>= (stlong8 /= stlong1)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong7", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((d1 * d1) + stlong2) - (*pd4 += (*pf2 /= stlong6)))", -3.5566666746140) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("f2", 0.366667) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d4", 4.7666666746140) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong7 | ( - stlong1)) * (stlong8 <<= (stlong6 &= stlong3)))", -16) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong6", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 16) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong4 >>= (stlong5 >>= stlong6)) + (d1 || ( ! f3)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
