///////////////////////////////////////////////////////////////////////////////
//  M3CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix3Cases class
//

#include "stdafx.h"
#include "m3cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix3Cases, CDebugTestSet, "Mixed3", 9, CMixSubSuite)


void CMix3Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix3Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((stlong1 &= (stlong4 & stlong2)) <= (stlong8 |= (stlong4 == stlong4)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong1", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((( ! stlong6) | stlong2) >> (stlong4 < (stlong6 - stlong1)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong4 == (stlong2 / stlong7)) & (stlong2 >= ( - stlong3)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong7 != ((stlong2)-- )) ^ (stlong6 + (stlong3 / stlong3)))", 6) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong2", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong8 = (stlong7 >= stlong5)) << ((stlong1 << stlong1) <= stlong2))", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
