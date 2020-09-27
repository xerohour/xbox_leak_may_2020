///////////////////////////////////////////////////////////////////////////////
//	CreateHTMLPageSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CCreateHTMLPageSubsuite class
//

#include "stdafx.h"
#include "CreateHTMLPageSubsuite.h"
#include "afxdllx.h"

#include "CreateHTMLPageCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CCreateHTMLPageSubsuite, CIDESubSuite, "Create HTML Page", "VCQA")

BEGIN_TESTLIST(CCreateHTMLPageSubsuite)
	TEST(CCreateHTMLPageCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
