///////////////////////////////////////////////////////////////////////////////
//	InsertActiveXControlIntoHTMLPageSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CInsertActiveXControlIntoHTMLPageSubsuite class
//

#include "stdafx.h"
#include "InsertActiveXControlIntoHTMLPageSubsuite.h"
#include "afxdllx.h"

#include "InsertActiveXControlIntoHTMLPageCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CInsertActiveXControlIntoHTMLPageSubsuite, CIDESubSuite, "Insert ActiveX Control Into HTML Page", "VCQA")

BEGIN_TESTLIST(CInsertActiveXControlIntoHTMLPageSubsuite)
	TEST(CInsertActiveXControlIntoHTMLPageCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
