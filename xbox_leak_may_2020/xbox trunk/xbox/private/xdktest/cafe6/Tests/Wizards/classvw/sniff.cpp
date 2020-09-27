///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			Date :
//		JimGries			4/3/95
//
//	Description :
//		implementation of the CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"

#include "ClassVw.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CClassViewSubSuite, CIDESubSuite, "ClassView Subsuite", "JimGries")

BEGIN_TESTLIST(CClassViewSubSuite)
	TEST(CComprehensiveTest, RUN)
END_TESTLIST()

