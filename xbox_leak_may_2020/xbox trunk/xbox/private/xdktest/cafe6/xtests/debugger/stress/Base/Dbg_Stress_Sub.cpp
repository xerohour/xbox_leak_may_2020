///////////////////////////////////////////////////////////////////////////////
//	Dbg_Stress_Sub.CPP
//
//	Created by :			
//		Xbox XDK Test		
//
//	Description :
//		implementation of the CDbgStressSubSuite class
//

#include "stdafx.h"
#include "Dbg_Stress_Sub.h"
#include "afxdllx.h"

#include "Dbg_stress_base.h"


#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CDbgStressSubSuite

IMPLEMENT_SUBSUITE(CDbgStressSubSuite, CIDESubSuite, "Debugger Stress", "Xbox Debugger")

BEGIN_TESTLIST(CDbgStressSubSuite)
	TEST(CDbgStressStopAndGoTest, RUN)
	TEST(CDbgStressRestartAndGoTest, RUN)
	TEST(CDbgStressBreakAndGoTest, RUN)
	TEST(CDbgStressConditionalBPNonCountingTest, RUN)
	TEST(CDbgStressConditionalBPCountingTest, RUN)
	TEST(CDbgStressDataBPTest, RUN)
END_TESTLIST()

void CDbgStressSubSuite::CleanUp(void)
	{
	::CleanUp("stress01");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
