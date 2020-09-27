///////////////////////////////////////////////////////////////////////////////
//  M5CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix5Cases class
//

#include "stdafx.h"
#include "m5cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix5Cases, CDebugTestSet, "Mixed5", 11, CMixSubSuite)


void CMix5Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix5Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((d4 += (stlong7 - stlong2)) || ((stlong2 << stlong4) || stlong6))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d4", 9.4000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((stlong7 * stlong2) | stlong5) > (f1 || (f3 <= stlong5)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong5 -= (*pd1 <= d2)) - (stlong6 == ( ++ f1)))", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("f1", 2.10000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong5", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong7 = (stlong8 <<= stlong1)) * (( - f2) / f3))", -10.6667) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 16) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong7", 16) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("((stlong8 == (*pd1 + *pf1)) - (stlong6 |= (stlong1 - stlong7)))", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);  
 	WriteLog(cxx.ExpressionValueIs("stlong6", -9) ? PASSED : FAILED, "line = %d\n", __LINE__);  

	cxx.Disable();
	}
