///////////////////////////////////////////////////////////////////////////////
//	depsub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CDependenciesSubSuite class
//

#include "stdafx.h"
#include "depsub.h"
#include "afxdllx.h"

#include "filedep.h"
#include "projdep.h"
#include "gprojdep.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CDependenciesSubSuite

IMPLEMENT_SUBSUITE(CDependenciesSubSuite, CIDESubSuite, "Project Dependencies", "VCQA ProjBuild")

BEGIN_TESTLIST(CDependenciesSubSuite)
	TEST(CFileDependencies, RUN)
	TEST(CProjectDependencies, RUN)
	TEST(CGenProjectDependencies, RUN)
END_TESTLIST()
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
