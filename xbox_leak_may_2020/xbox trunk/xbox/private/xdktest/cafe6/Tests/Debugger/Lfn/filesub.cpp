///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		WinslowF		
//
//	Description :
//		implementation of the CFILENAMESubSuite class
//

#include "stdafx.h"
#include "FILEsub.h"

#include "FILEcase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CFILENAMESubSuite

IMPLEMENT_SUBSUITE(CFILENAMESubSuite, CIDESubSuite, "DBG filename test", "WinslowF")

BEGIN_TESTLIST(CFILENAMESubSuite)
	TEST(CFILENAMEIDETest, RUN)
END_TESTLIST()
