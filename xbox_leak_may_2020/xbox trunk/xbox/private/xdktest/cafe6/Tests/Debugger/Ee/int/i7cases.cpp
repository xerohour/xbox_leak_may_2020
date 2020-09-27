///////////////////////////////////////////////////////////////////////////////
//  I7CASES.CPP
//
//  Created by :            Date :
//      MichMa             		12/29/93
//
//  Description :
//      Implementation of the CInt7Cases class
//

#include "stdafx.h"
#include "i7cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt7Cases, CDebugTestSet, "Integer7", 13, CIntSubSuite)

												 
void CInt7Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt7Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((rint1 != (*pui < rint3)) || (**ppul ^= (*ps /= l)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("s", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ul", 65000) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((*ps << ( ! *pui)) - (i |= (c && rint1)))", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("i", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((rint5 ^ rint2) && **ppc) - (*ps = ((**ppuc)-- )))", -127) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("uc", 127) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("s", 128) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**pps /= (**ppi - rint2)) + (**ppc && (*pl + **ppc)))", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("s", 14) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((uc += (**ppl < rint3)) != (**ppc ^ ( -- rint1)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint1", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("uc", 127) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
