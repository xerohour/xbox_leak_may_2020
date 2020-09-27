///////////////////////////////////////////////////////////////////////////////
//      edsntype.CPP
//
//      Created by :                    
//              GeorgeCh                
//
//      Description :
//              implementation of the Edsnif01Driver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "edsntype.h"
#include "edsncase.h"
#include "emulcase.h"
#include "macrcase.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_SUBSUITE(EditorSniff, CIDESubSuite, "Source Edit Sniff", "MHopkins x60946")

// Fill out the test list
BEGIN_TESTLIST(EditorSniff)
	TEST(SmartIndent, RUN)
	TEST(EdsnifTabIndent, RUN)
	TEST(EdsnifOvrInsert, RUN)
	TEST(EdsnifToolsMenu, DONTRUN)
	TEST(cVC3Position, RUN)
	TEST(cVC2Position, RUN)
	TEST(cBriefPosition, DONTRUN)
	TEST(cEpsilonPosition, DONTRUN)
  TEST(CMacroTestCases, DONTRUN)
END_TESTLIST()

void EditorSniff::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
}

