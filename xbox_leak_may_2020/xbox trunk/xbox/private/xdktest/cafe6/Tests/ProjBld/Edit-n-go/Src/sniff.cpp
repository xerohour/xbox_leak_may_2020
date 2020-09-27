///////////////////////////////////////////////////////////////////////////////
//	SNIFF.CPP
//
//	Created by :			Date :
//		RickKr					8/30/93
//
//	Description :
//		implementation of the CSniffDriver class
//

#include "stdafx.h"
#include "afxdllx.h"
#include "sniff.h"
#include "percases.h"
#include "wizcases.h"
#include "spycases.h"
#include "mpdcases.h"
#include "difcases.h"

//extern BOOL GetBooleanSwitch(LPCSTR);

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////		(CMDLINE->GetTextValue.Find)!=x))
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "Edit 'n Go Sniff Test", "ThuyT X32270")

BEGIN_TESTLIST(CSniffDriver)
	TEST(CDefAppWizCases,((CMDLINE->GetTextValue("Test","")=="") || (CMDLINE->GetTextValue("Test","").Find("APPWIZ")!=(-1))) || (CMDLINE->GetBooleanValue("runall"))? RUN : DONTRUN)
#if !defined(LEGO) // Lego meeds appwiz case only 
	TEST(CSpyCases,     ((((CMDLINE->GetTextValue("Test")).Find("SPY"))!=(-1))      || CMDLINE->GetBooleanValue("runall")) && (GetUserTargetPlatforms() != PLATFORM_MAC_68K && GetUserTargetPlatforms() != PLATFORM_MAC_PPC)? RUN : DONTRUN)
	TEST(CMultipadCases,((((CMDLINE->GetTextValue("Test")).Find("MULTIPAD"))!=(-1)) || CMDLINE->GetBooleanValue("runall")) && (GetUserTargetPlatforms() != PLATFORM_MAC_68K && GetUserTargetPlatforms() != PLATFORM_MAC_PPC)? RUN : DONTRUN)
	TEST(CWinDiffCases, ((((CMDLINE->GetTextValue("Test")).Find("WINDIFF"))!=(-1))  || CMDLINE->GetBooleanValue("runall")) && (GetUserTargetPlatforms() != PLATFORM_MAC_68K && GetUserTargetPlatforms() != PLATFORM_MAC_PPC)? RUN : DONTRUN)
#endif
END_TESTLIST()

void CSniffDriver::SetUp(BOOL bCleanUp)
{
	// call the base class
	CIDESubSuite::SetUp(bCleanUp);
	ExpectedMemLeaks(0);
}

