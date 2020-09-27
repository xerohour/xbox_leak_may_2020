///////////////////////////////////////////////////////////////////////////////
//	buildsub.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CBuildSubSuite class
//

#include "stdafx.h"
#include "buildsub.h"
#include "afxdllx.h"
//#include "..\cleanup.h"

#include "bldwin32.h"
#include "bldmfc.h"
#include "bldatl.h"
#include "bldmisc.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CBuildSubSuite

IMPLEMENT_SUBSUITE(CBuildSubSuite, CIDESubSuite, "Build", "VCQA ProjBuild")

BEGIN_TESTLIST(CBuildSubSuite)
	TEST(CBuildWin32Test, RUN)
	TEST(CBuildMFCTest, RUN)
	TEST(CBuildATLTest, RUN)
	TEST(CBuildMiscTest, RUN)
END_TESTLIST()
/*
void CProj1SubSuite::CleanUp(void)
	{
//	::CleanUp();
	}
*/
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
