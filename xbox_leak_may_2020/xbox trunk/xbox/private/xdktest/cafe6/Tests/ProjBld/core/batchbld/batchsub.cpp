///////////////////////////////////////////////////////////////////////////////
//	batchsub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CBatchSubSuite class
//

#include "stdafx.h"
#include "batchsub.h"
#include "afxdllx.h"

#include "batch1.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CBatchsubSuite

IMPLEMENT_SUBSUITE(CBatchSubSuite, CIDESubSuite, "Batch Build", "VCQA ProjBuild")

BEGIN_TESTLIST(CBatchSubSuite)
	TEST(CBatchBuildTest1, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
