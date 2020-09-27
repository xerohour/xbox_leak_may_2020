///////////////////////////////////////////////////////////////////////////////
//	CreateActiveXControlProjectSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CCreateActiveXControlProjectSubsuite class
//

#include "stdafx.h"
#include "CreateActiveXControlProjectSubsuite.h"
#include "afxdllx.h"

#include "CreateActiveXControlProjectCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CCreateActiveXControlProjectSubsuite, CIDESubSuite, "Create ActiveX Control Project", "VCQA")

BEGIN_TESTLIST(CCreateActiveXControlProjectSubsuite)
	TEST(CCreateActiveXControlProjectCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
