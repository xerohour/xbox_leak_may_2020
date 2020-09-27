///////////////////////////////////////////////////////////////////////////////
//	ATLWizSubsuite.CPP
//
//	Created by :			Date :			
//		ChrisKoz					1/28/98
//
//	Description :
//		implementation of the CSysATLWizSubsuite class
//

#include "stdafx.h"
#include "ATLWizSuite.h"
#include "afxdllx.h"

#include "ATLWizCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSysATLWizSubsuite

IMPLEMENT_SUBSUITE(CSysATLWizSubsuite, CIDESubSuite, "System ATLWiz", "MichMa")

BEGIN_TESTLIST(CSysATLWizSubsuite)
	TEST(CSysATLWizCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
