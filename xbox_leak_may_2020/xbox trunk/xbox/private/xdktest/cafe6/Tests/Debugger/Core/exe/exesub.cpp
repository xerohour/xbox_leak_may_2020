///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CexeSubSuite class
//

#include "stdafx.h"
#include "exesub.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "exe_base.h"
#include "exe_dlls.h"
#include "exe_bp.h"
#include "exe_spec.h"
#include "execase3.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CexeSubSuite

IMPLEMENT_SUBSUITE(CexeSubSuite, CIDESubSuite, "Execution", "VCQA Debugger")

BEGIN_TESTLIST(CexeSubSuite)
	TEST(Cexe_baseIDETest, RUN)
	TEST(Cexe_dllsIDETest, RUN)
	TEST(Cexe_bpIDETest, RUN)
	TEST(Cexe_specialIDETest, RUN)
	TEST(CExe3Cases, RUN)
END_TESTLIST()

void CexeSubSuite::CleanUp(void)
	{
	::CleanUp("testbp01");
	::CleanUp("testgo01");
	::CleanUp("dbg");
	::CleanUp("dbga");
	::CleanUp("dllapp");
	}

///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
