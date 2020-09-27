///////////////////////////////////////////////////////////////////////////////
//  M11CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix11Cases class
//

#include "stdafx.h"
#include "m11cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix11Cases, CDebugTestSet, "Mixed11", 15, CMixSubSuite)


void CMix11Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix11Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((stlong5 |= (stlong3 >>= stlong4)) << ((stlong4 >> stlong4) >= stlong2))", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong3 *= (stlong1 <= stlong6)) ^ (stlong8 += (stlong1 + stlong1)))", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong3 = (stlong4 % stlong1)) << (stlong2 *= (stlong7 || stlong8)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong2", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((*pd2 = ( -- stlong3)) < (stlong2 |= (stlong6 < stlong5)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong2", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", -1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d2", -1.0000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong3 / ((stlong1)-- )) | (stlong6 <= (stlong3 % stlong2)))", -1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong1", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
