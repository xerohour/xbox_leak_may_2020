///////////////////////////////////////////////////////////////////////////////
//	testcase.CPP
//
//	Created by :			
//		VCBU QA		
//
//	Description :
//		implementation of the TestDriver class
//

#include "stdafx.h"
#include "testtype.h"

#include "testcase.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// The one and only TestDriver object

TestDriver NEAR theApp;
CWBDebugThread NEAR theDebugThread;

/////////////////////////////////////////////////////////////////////////////
// TestDriver

BEGIN_TESTSET_MAP(TestDriver)
	TESTSET(TestTestSet, TRUE)
END_TESTSET_MAP()

TestDriver::TestDriver()
{
}

BOOL TestDriver::InitializeTest( void )
{
	if( !CWBDriver::InitializeTest() )
		return FALSE;
	//TODO: Replace XXX with name of sniff test (e.g. VPROJ, VRES, etc.)
	//TODO: Replace OWNER with email name of owner
	return m_Log.BeginLog( "TestWizard Generated script", "VCBU QA", "BVT", m_bDebug, m_bPost );
}

BOOL TestDriver::RunTests()
{
	return RunTestsInOrder() == 0;
}

BOOL TestDriver::FinalizeTest( void )
{
	m_Log.EndLog();
	return CWBDriver::FinalizeTest();
}

BEGIN_MESSAGE_MAP(TestDriver, CWBDriver)
	//{{AFX_MSG_MAP(TestDriver)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()
