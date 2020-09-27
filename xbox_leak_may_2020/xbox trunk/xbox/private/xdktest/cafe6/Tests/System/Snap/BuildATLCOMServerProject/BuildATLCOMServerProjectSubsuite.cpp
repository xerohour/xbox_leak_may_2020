///////////////////////////////////////////////////////////////////////////////
//	BuildATLCOMServerProjectSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CBuildATLCOMServerProjectSubsuite class
//

#include "stdafx.h"
#include "BuildATLCOMServerProjectSubsuite.h"
#include "afxdllx.h"

#include "BuildATLCOMServerProjectCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CBuildATLCOMServerProjectSubsuite, CIDESubSuite, "Build ATL COM Server Project", "VCQA")

BEGIN_TESTLIST(CBuildATLCOMServerProjectSubsuite)
	TEST(CBuildATLCOMServerProjectCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
