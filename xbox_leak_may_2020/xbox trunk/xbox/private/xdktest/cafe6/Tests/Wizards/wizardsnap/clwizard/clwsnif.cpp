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
#include "clwsnif.h"

#include "clwcases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// The one and only CSniffDriver object

CSniffDriver NEAR theApp;
CWBDebugThread NEAR theDebugThread;

/////////////////////////////////////////////////////////////////////////////
// CSniffDriver

BEGIN_TESTSET_MAP(CSniffDriver)
	TESTSET(ClassWizardTestCases, TRUE)
END_TESTSET_MAP()

CSniffDriver::CSniffDriver()
{
}

BOOL CSniffDriver::InitializeTest( void )
{
	if( !CWBDriver::InitializeTest() )
		return FALSE;
	//TODO: Replace XXX with name of sniff test (e.g. VPROJ, VRES, etc.)
	//TODO: Replace OWNER with email name of owner
	return m_Log.BeginLog( "ClassWizard BVT", "Ivanl", "BVT", m_bDebug, m_bPost );
}

BOOL CSniffDriver::RunTests()
{
	return RunTestsInOrder() == 0;
}

BOOL CSniffDriver::FinalizeTest( void )
{
	m_Log.EndLog();
	return CWBDriver::FinalizeTest();
}

BEGIN_MESSAGE_MAP(CSniffDriver, CWBDriver)
	//{{AFX_MSG_MAP(CSniffDriver)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()
