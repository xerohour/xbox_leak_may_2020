///////////////////////////////////////////////////////////////////////////////
//	DebugActiveXControlInHTMLPageSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CDebugActiveXControlInHTMLPageSubsuite class
//

#include "stdafx.h"
#include "DebugActiveXControlInHTMLPageSubsuite.h"
#include "afxdllx.h"

#include "DebugActiveXControlInHTMLPageCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CDebugActiveXControlInHTMLPageSubsuite, CIDESubSuite, "Debug ActiveX Control In HTML Page", "VCQA")

BEGIN_TESTLIST(CDebugActiveXControlInHTMLPageSubsuite)
	TEST(CDebugActiveXControlInHTMLPageCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
