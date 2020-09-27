///////////////////////////////////////////////////////////////////////////////
//  I3CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/10/93
//
//  Description :
//      Implementation of the CInt3Cases class
//

#include "stdafx.h"
#include "i3cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt3Cases, CDebugTestSet, "Integer3", 12, CIntSubSuite)

												 
void CInt3Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt3Cases::Run(void)

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

	WriteLog(cxx.ExpressionValueIs("((*ps &= (uc & ul)) <= (rint5 |= (*pi == uc)))", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint5", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("s", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((( ! ul) | *puc) >> (*pul < (**ppus - uc)))", 128) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppus != ((rint3)-- )) ^ (rint1 + (**ppui / **ppul)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint3", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppui ^ (*pl - ui)) == (rint1 ^= (i *= rint3)))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("i", 20) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint1", 21) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("((ul %= ( -- *ps)) * (( ! *pl) - **ppc))", 4293667296) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("s", -1) ? PASSED : FAILED, "line = %d\n", __LINE__);
 	WriteLog(cxx.ExpressionValueIs("ul", 65000) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();	
	}
