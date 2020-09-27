///////////////////////////////////////////////////////////////////////////////
//	InsertCallToATLCOMServerInAppSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CInsertCallToATLCOMServerInAppSubsuite class
//

#include "stdafx.h"
#include "InsertCallToATLCOMServerInAppSubsuite.h"
#include "afxdllx.h"

#include "InsertCallToATLCOMServerInAppCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CInsertCallToATLCOMServerInAppSubsuite, CIDESubSuite, "Insert Call To ATL COM Server In App", "VCQA")

BEGIN_TESTLIST(CInsertCallToATLCOMServerInAppSubsuite)
	TEST(CInsertCallToATLCOMServerInAppCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
