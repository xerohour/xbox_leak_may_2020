///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CNameSpceSubSuite class
//

#include "stdafx.h"
#include "Namesub.h"
#include "afxdllx.h"

#include "Namecase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CNameSpceSubSuite

IMPLEMENT_SUBSUITE(CNameSpceSubSuite, CIDESubSuite, "Namespace", "VCQA Debugger")

BEGIN_TESTLIST(CNameSpceSubSuite)
	TEST(CNameSpceIDETest, RUN)
END_TESTLIST()

void CNameSpceSubSuite::CleanUp(void)
	{
	::CleanUp("namspace");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

