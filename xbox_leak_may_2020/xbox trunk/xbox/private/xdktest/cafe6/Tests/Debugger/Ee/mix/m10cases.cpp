///////////////////////////////////////////////////////////////////////////////
//  M10CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix10Cases class
//

#include "stdafx.h"
#include "m10cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix10Cases, CDebugTestSet, "Mixed10", 12, CMixSubSuite)


void CMix10Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix10Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((stlong2 &= (stlong1 & stlong8)) > ((stlong3 ^ stlong3) ^ stlong7))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong2", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((((*pd1)++ ) + f2) + (stlong8 += (d4 != f4 + .1)))", 12.300000047684) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d1", 2.1000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong5 |= (stlong3 += stlong6)) << (stlong4 -= (stlong1 / stlong6)))", 208) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((( ! stlong3) != *pf3) >= (stlong5 &= (stlong4 - stlong1)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((stlong5 + stlong1) != stlong3) & ((stlong3 || stlong8) || stlong7))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
