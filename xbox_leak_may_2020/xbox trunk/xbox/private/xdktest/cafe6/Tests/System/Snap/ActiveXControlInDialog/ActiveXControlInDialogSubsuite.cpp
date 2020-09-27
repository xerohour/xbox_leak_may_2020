///////////////////////////////////////////////////////////////////////////////
//	ActiveXControlInDialogSubSuitee.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CActiveXControlInDialogSubSuitee class
//

#include "stdafx.h"
#include "ActiveXControlInDialogSubSuite.h"
#include "afxdllx.h"

#include "ActiveXControlInDialogCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CActiveXControlInDialogSubsuite, CIDESubSuite, "ActiveX Control In Dialog", "VCQA")

BEGIN_TESTLIST(CActiveXControlInDialogSubsuite)
	TEST(CActiveXControlInDialogCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
