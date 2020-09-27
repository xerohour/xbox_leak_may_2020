///////////////////////////////////////////////////////////////////////////////
//  M2CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix2Cases class
//

#include "stdafx.h"
#include "m2cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix2Cases, CDebugTestSet, "Mixed2", 11, CMixSubSuite)


void CMix2Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix2Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((stlong2 -= (*pf2 > f3)) + (stlong5 |= (stlong4 ^ stlong7)))", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong2", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("(((stlong2 == stlong5) & stlong3) <= (stlong7 &= ( - stlong5)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("stlong7", 1) ? PASSED : FAILED, "line = %d\n", __LINE__); 
	WriteLog(cxx.ExpressionValueIs("((stlong2 != ( - *pf3)) || ((stlong4 / stlong2) / f1))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong3 += (*pf4 < stlong4)) < (stlong4 || (*pf2 - d4)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong8 |= ( ++ stlong1)) + ((f2 / stlong3) * f3))", 12.4200) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong1", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
