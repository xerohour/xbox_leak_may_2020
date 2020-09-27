///////////////////////////////////////////////////////////////////////////////
//  SNIFF.CPP
//
//  Created by :                    Date :
//		MarcI & DougMan					1/1/97
//
//  Description :
//      Implementation of the CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"
#include "f1lookup.h"
#include "dlgwalk.h"
#include "FTS.h"

#include "Generic.h"


#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "InfoViewer Sniff Test", "DougMan x39440, MarcI x36051")

BEGIN_TESTLIST(CSniffDriver)
//	TEST(CF1Lookup,((CMDLINE->GetTextValue("Test","")=="") || (CMDLINE->GetTextValue("Test","").Find("APPWIZ")!=(-1))) || (CMDLINE->GetBooleanValue("runall"))? RUN : DONTRUN)
	TEST(CDlgWalk, RUN)
	TEST(CFTS, RUN)
	TEST(CF1Lookup, RUN)

//	TEST(CGeneric, RUN)
END_TESTLIST()

void CSniffDriver::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
}

void CSniffDriver::CleanUp(void)
{
	// close all windows
	UIWB.CloseAllWindows();

}

