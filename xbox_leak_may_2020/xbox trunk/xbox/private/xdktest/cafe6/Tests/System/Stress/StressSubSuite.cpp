///////////////////////////////////////////////////////////////////////////////
//	StressSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					1/28/98
//
//	Description :
//		implementation of the CSysStressSubsuite class
//

#include "stdafx.h"
#include "StressSubsuite.h"
#include "afxdllx.h"

#include "StressCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSysStressSubsuite

IMPLEMENT_SUBSUITE(CSysStressSubsuite, CIDESubSuite, "System Stress", "MichMa")

BEGIN_TESTLIST(CSysStressSubsuite)
	TEST(CSysStressCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
