///////////////////////////////////////////////////////////////////////////////
//	TMPLSUB.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CTemplatesSubSuite class
//

#include "stdafx.h"
#include "tmplsub.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "tmplcase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CTemplatesSubSuite

IMPLEMENT_SUBSUITE(CTemplatesSubSuite, CIDESubSuite, "Templates", "VCQA Debugger")

BEGIN_TESTLIST(CTemplatesSubSuite)
	TEST(CTemplatesIDETest, RUN)
END_TESTLIST()

void CTemplatesSubSuite::CleanUp(void)
	{
	::CleanUp("ee");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

