///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			Date :
//		DavidGa					10/27/93
//
//	Description :
//		implementation of the Wizards' CSniffDriver class
//

#include "stdafx.h"
#include "sniff.h"

#include "appsnap.h"
#include "clwsnap.h"
#include "ctrsnap.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "Wizards Snap test", "AnitaG")

BEGIN_TESTLIST(CSniffDriver)
	TEST(CTestAppWizardCases, DONTRUN)	//appsnap.cpp
	TEST(ClassWizardTestCases, RUN)	//clwsnap.cpp
	TEST(CCtrlWizardCases, RUN)	//ctrsnap.cpp
END_TESTLIST()


// flag to indicate to class wizard whether app wizard was successful or not
BOOL gbAppWiz;

void CSniffDriver::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
 
	gbAppWiz = FALSE;
}
