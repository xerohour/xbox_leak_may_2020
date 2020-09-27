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

#include "prtcases.h"
#include "wincases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

IMPLEMENT_SUBSUITE(CSniffDriver, CIDESubSuite, "Debugger Sniff Test", "MichMa")

BEGIN_TESTLIST(CSniffDriver)
	TEST(CPortableDbgCases, RUN)
	TEST(CWin32DbgCases, RUN)
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

	KillFile("debugger\\prtdbgee", "*.mdp");
	KillFile("debugger\\prtdbgee", "*.opt");
	KillFile("debugger\\prtdbgee", "*.ncb");
	KillFile("debugger\\dlldbgee", "*.mdp");
	KillFile("debugger\\dlldbgee", "*.opt");
	KillFile("debugger\\dlldbgee", "*.ncb");
}
