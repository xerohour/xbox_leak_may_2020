///////////////////////////////////////////////////////////////////////////////
//	ThreadsSubsuite.cpp
//
//	Created by: MichMa		Date: 2/5/98
//
//	Description :
//		Implementation of the CThreadsSubsuite class
//

#include "stdafx.h"
#include "ThreadsSubsuite.h"
#include "afxdllx.h"
#include "..\cleanup.h"

#include "ThreadsCases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CThreadsSubsuite

IMPLEMENT_SUBSUITE(CThreadsSubsuite, CIDESubSuite, "Threads", "VCQA Debugger")

BEGIN_TESTLIST(CThreadsSubsuite)
	TEST(CThreadsCases, RUN)
END_TESTLIST()

