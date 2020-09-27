///////////////////////////////////////////////////////////////////////////////
//  I2CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/06/93
//
//  Description :
//      Implementation of the CInt2Cases class
//

#include "stdafx.h"
#include "i2cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt2Cases, CDebugTestSet, "Integer2", 11, CIntSubSuite)

												 
void CInt2Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt2Cases::Run(void)

	{
	if(!fProjectReady)
		{
		InitProject("int");
		bps.SetBreakpoint("break_here");
		fProjectReady = TRUE;
		}
	else
		dbg.Restart();

	dbg.Go();
	cxx.Enable();
		
	WriteLog(cxx.ExpressionValueIs("((rint4 -= (rint5 > l)) + (rint1 |= (rint2 ^ *pc)))", 27) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint1", 23) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint4", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("(((**pps == ui) & c) <= (rint5 &= ( - *pi)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("rint5", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((rint4 != ( - **ppuc)) || ((s / rint2) / ui))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((ui += (*pi < i)) < (ul || (rint1 - i)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ui", 100) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppui |= ( ++ *pi)) + ((uc / rint1) * l))", 596) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("i", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ui", 111) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
