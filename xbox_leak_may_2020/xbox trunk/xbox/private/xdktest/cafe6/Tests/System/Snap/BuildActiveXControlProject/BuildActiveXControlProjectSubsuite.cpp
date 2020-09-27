///////////////////////////////////////////////////////////////////////////////
//	BuildActiveXControlProjectSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CBuildActiveXControlProjectSubsuite class
//

#include "stdafx.h"
#include "BuildActiveXControlProjectSubsuite.h"
#include "afxdllx.h"

#include "BuildActiveXControlProjectCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CBuildActiveXControlProjectSubsuite, CIDESubSuite, "Build ActiveX Control Project", "VCQA")

BEGIN_TESTLIST(CBuildActiveXControlProjectSubsuite)
	TEST(CBuildActiveXControlProjectCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
