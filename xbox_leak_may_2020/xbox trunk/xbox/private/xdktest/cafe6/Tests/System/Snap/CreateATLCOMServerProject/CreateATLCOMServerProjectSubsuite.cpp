///////////////////////////////////////////////////////////////////////////////
//	CreateATLCOMServerProjectSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CCreateATLCOMServerProjectSubsuite class
//

#include "stdafx.h"
#include "CreateATLCOMServerProjectSubsuite.h"
#include "afxdllx.h"

#include "CreateATLCOMServerProjectCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CCreateATLCOMServerProjectSubsuite, CIDESubSuite, "Create ATL COM Server Project", "VCQA")

BEGIN_TESTLIST(CCreateATLCOMServerProjectSubsuite)
	TEST(CCreateATLCOMServerProjectCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
