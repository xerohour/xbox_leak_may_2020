///////////////////////////////////////////////////////////////////////////////
//	CrashDumpSubsuite.cpp
//
//	Created by: MichMa		Date: 9/30/97
//
//	Description :
//		Implementation of the CCrashDumpSubsuite class
//

#include "stdafx.h"
#include "CrashDumpSubsuite.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "CrashDumpCases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CCrashDumpSubsuite

IMPLEMENT_SUBSUITE(CCrashDumpSubsuite, CIDESubSuite, "Crash Dump", "VCQA Debugger")

BEGIN_TESTLIST(CCrashDumpSubsuite)
	TEST(CCrashDumpCases, RUN)
END_TESTLIST()

void CCrashDumpSubsuite::CleanUp(void)
	{
	}

