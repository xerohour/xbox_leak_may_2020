///////////////////////////////////////////////////////////////////////////////
//  I1CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/10/93
//
//  Description :
//      Implementation of the CInt1Cases class
//

#include "stdafx.h"
#include "i1cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt1Cases, CDebugTestSet, "Integer1", 14, CIntSubSuite)

												 
void CInt1Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt1Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((((*pui)++ ) && *puc) << (rint1 < (i / rint3)))", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ui", 101) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((rint4 % rint1) | uc) / (((c)-- ) || *pl))", 128) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 19) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppl *= (*pc != *pul)) | ((c ^ **pps) ^ *pus))", 33015) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("l", 97) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((*pc &= ((**ppus)-- )) && (ul &= (rint2 *= **ppi)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint2", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ul", 65000) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("us", 32999) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((us <= (s % *ps)) ^ (*pc *= (rint2 &= rint5)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint2", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("c", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	
	cxx.Disable();
	}
