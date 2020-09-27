///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the CDisAsmSubSuite class
//

#include "stdafx.h"
#include "prj1sub.h"
#include "afxdllx.h"
//#include "..\cleanup.h"

#include "prjcases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CProj1SubSuite

IMPLEMENT_SUBSUITE(CProj1SubSuite, CIDESubSuite, "Proj1", "VCQA ProjBuild")

BEGIN_TESTLIST(CProj1SubSuite)
	TEST(CPrjAppTest, RUN)
END_TESTLIST()
/*
void CProj1SubSuite::CleanUp(void)
	{
//	::CleanUp();
	}
*/
///////////////////////////////////////////////////////////////////////////////
//	SubSuite initialization
