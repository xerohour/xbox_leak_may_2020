///////////////////////////////////////////////////////////////////////////////
//  M9CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix9Cases class
//

#include "stdafx.h"
#include "m9cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix9Cases, CDebugTestSet, "Mixed9", 11, CMixSubSuite)


void CMix9Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix9Cases::Run(void)

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

 	WriteLog(cxx.ExpressionValueIs("(((*pd4 - d4) == d3) != (((stlong5)++ ) >> stlong4))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 6) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong6 & (stlong7 >> stlong1)) > (stlong4 &= (stlong2 ^ stlong8)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((( - f4) && *pf3) || ((d3 != *pd1) && *pd3))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((*pd1 += (*pd2 < d4)) == (stlong2 %= (stlong7 += stlong6)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong7", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong2", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d1", 2.1000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((stlong3 % stlong6) % stlong7) >= (stlong4 >>= (stlong3 & stlong8)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong4", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
