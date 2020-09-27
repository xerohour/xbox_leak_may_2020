///////////////////////////////////////////////////////////////////////////////
//  I8CASES.CPP
//
//  Created by :            Date :
//      MichMa              	12/29/93
//
//  Description :
//      Implementation of the CInt8Cases class
//

#include "stdafx.h"
#include "i8cases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CInt8Cases, CDebugTestSet, "Integer8", 15, CIntSubSuite)

												 
void CInt8Cases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CInt8Cases::Run(void)

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
						  
 	WriteLog(cxx.ExpressionValueIs("(((rint1 % s) > rint1) & (**ppl %= ((rint2)-- )))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint2", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("l", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("(((*pl * ui) + **ppui) - (**ppi += (**ppuc /= **ppc)))", 184) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("uc", 6) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("i", 16) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((*pl >>= (rint1 >>= rint2)) + (**ppc || ( ! ui)))", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint1", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("l", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((**ppui = ((rint3)-- )) | (rint4 ^ ( ++ ul)))", 65007) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ul", 65001) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("rint3", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("ui", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("((rint3 == ( -- **ppuc)) - ((l * c) < **ppl))", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("uc", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
	}
