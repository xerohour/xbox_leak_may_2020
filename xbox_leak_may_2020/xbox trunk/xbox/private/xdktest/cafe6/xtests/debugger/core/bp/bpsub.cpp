///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CbpSubSuite class
//

#include "stdafx.h"
#include "bpsub.h"
#include "afxdllx.h"

#include "bpcase.h"
#include "bpcase2.h"
#include "bpcase3.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CbpSubSuite

IMPLEMENT_SUBSUITE(CbpSubSuite, CIDESubSuite, "Breakpoint", "VCQA Debugger")

BEGIN_TESTLIST(CbpSubSuite)
	TEST(CbpIDETest, RUN)
	//xbox (bps in dlls) TEST(Cbp2IDETest, RUN)
	TEST(CBp3Cases, RUN)
END_TESTLIST()

void CbpSubSuite::CleanUp(void)
	{
	::CleanUp("testbp01");
	::CleanUp("testgo01");
	::CleanUp("dbg");
	::CleanUp("dbga");
	::CleanUp("bp3");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization

