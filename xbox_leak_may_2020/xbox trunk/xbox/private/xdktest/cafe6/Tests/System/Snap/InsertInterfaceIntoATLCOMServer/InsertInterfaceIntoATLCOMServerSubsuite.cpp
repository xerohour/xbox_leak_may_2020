///////////////////////////////////////////////////////////////////////////////
//	InsertInterfaceIntoATLCOMServerSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CInsertInterfaceIntoATLCOMServerSubsuite class
//

#include "stdafx.h"
#include "InsertInterfaceIntoATLCOMServerSubsuite.h"
#include "afxdllx.h"

#include "InsertInterfaceIntoATLCOMServerCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CInsertInterfaceIntoATLCOMServerSubsuite, CIDESubSuite, "Insert Interface Into ATL COM Server", "VCQA")

BEGIN_TESTLIST(CInsertInterfaceIntoATLCOMServerSubsuite)
	TEST(CInsertInterfaceIntoATLCOMServerCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
