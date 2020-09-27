///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CDbg_mbcSubSuite class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "Dbg_sub.h"

#include "Dbg_case.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CDbg_mbcSubSuite

IMPLEMENT_SUBSUITE(CDbg_mbcSubSuite, CIDESubSuite, "DBG MBC Sniff Tests", "MichMa X64633")

BEGIN_TESTLIST(CDbg_mbcSubSuite)
	TEST(CDbg_mbcIDETest, RUN)
END_TESTLIST()
