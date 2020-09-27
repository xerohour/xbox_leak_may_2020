///////////////////////////////////////////////////////////////////////////////
//  M13CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/15/93
//
//  Description :
//      Implementation of the CMix13Cases class
//

#include "stdafx.h"
#include "m13cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CMix13Cases, CDebugTestSet, "Mixed13", 14, CMixSubSuite)


void CMix13Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CMix13Cases::Run(void)

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
	
	WriteLog(cxx.ExpressionValueIs("((stlong3 || (stlong4 | stlong3)) * (( - *pf4) != *pf2))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong4 >= ((*pd4)-- )) && (d1 = (stlong7 ^= stlong8)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong7", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d1", 1.1000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("d4", 3.4000000000000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((d2 && ((stlong1)-- )) == (stlong7 ^= ( ! stlong8)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong7", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong1", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((stlong8 >>= ( -- stlong3)) & (stlong6 & (stlong4 != stlong2)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong3", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong8", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((f4 *= (*pf2 >= *pf2)) == (stlong6 /= (stlong7 >> stlong1)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("stlong6", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("f4", 4.40000) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
