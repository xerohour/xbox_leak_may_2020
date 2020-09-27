///////////////////////////////////////////////////////////////////////////////
//	DebugATLCOMServerSubsuite.CPP
//
//	Created by :			Date :			
//		MichMa					5/21/97
//
//	Description :
//		implementation of the CDebugATLCOMServerSubsuite class
//

#include "stdafx.h"
#include "DebugATLCOMServerSubsuite.h"
#include "afxdllx.h"

#include "DebugATLCOMServerCase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSystemSubSuite

IMPLEMENT_SUBSUITE(CDebugATLCOMServerSubsuite, CIDESubSuite, "Debug ATL COM Server", "VCQA")

BEGIN_TESTLIST(CDebugATLCOMServerSubsuite)
	TEST(CDebugATLCOMServerCase, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
