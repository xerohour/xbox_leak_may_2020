///////////////////////////////////////////////////////////////////////////////
//	BldATL.CPP
//
//	Created by :			Date :
//		YefimS					5/14/97
//
//	Description :
//		Implementation of the CBuildATLTest class

#include "stdafx.h"
#include "BldATL.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBuildATLTest, CProjectTestSet, "Building ATL Projects", -1, CBuildSubSuite)

void CBuildATLTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CBuildATLTest

void CBuildATLTest::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( BuildATLCOMAppWizardDll() );
	XSAFETY;
	EXPECT_TRUE( BuildATLCOMAppWizardExe() );
	XSAFETY;
	EXPECT_TRUE( BuildATLCOMAppWizardService() );
	XSAFETY;

}


BOOL CBuildATLTest::BuildATLCOMAppWizardExe( void )
{
	LogTestHeader("BuildATLCOMAppWizardExe");

	m_strProjName = "atlcomex";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;

	RemoveTestFiles( m_strProjLoc );

	CreateDirectory(m_strProjLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );

	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}

	LogSubTestHeader("BuildProjectAndVerify");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

 	LogSubTestHeader("SaveProject");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Save()));

	LogSubTestHeader("Close Project");
	BOOL bSaveProjBeforeClose = FALSE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}

	LogSubTestHeader("BuildProjectAgainAndVerify");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( FALSE ) );

	LogSubTestHeader("Change resources and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".h", szNewBasePathName + ".h", m_strProjName + ".cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".cpp", szNewBasePathName + ".cpp", m_strProjName + ".cpp"));

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Build Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change idl file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".idl", szNewBasePathName + ".idl", m_strProjName + ".idl"));

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

BOOL CBuildATLTest::BuildATLCOMAppWizardDll( void )
{
	LogTestHeader("BuildATLCOMAppWizardDll");

	m_strProjName = "atlcomaw";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;

	RemoveTestFiles( m_strProjLoc );

	CreateDirectory(m_strProjLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );

	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}

	LogSubTestHeader("BuildProjectAndVerify");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

 	LogSubTestHeader("SaveProject");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Save()));

	LogSubTestHeader("Close Project");
	BOOL bSaveProjBeforeClose = FALSE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}

	LogSubTestHeader("BuildProjectAgainAndVerify");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( FALSE ) );

	LogSubTestHeader("Change def file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".def", szNewBasePathName + ".def", GetLocString(IDSS_OUTPUTWIN_LINKING)));
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_COMPILING), FALSE ) < 0 );

	LogSubTestHeader("Build Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change resources and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".h", szNewBasePathName + ".h", m_strProjName + ".cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".cpp", szNewBasePathName + ".cpp", m_strProjName + ".cpp"));

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change idl file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".idl", szNewBasePathName + ".idl", m_strProjName + ".idl"));
	EXPECT_TRUE( VerifyBuildString( "Registering") > 0 );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

BOOL CBuildATLTest::BuildATLCOMAppWizardService( void )
{
	LogTestHeader("BuildATLCOMAppWizardService");

	m_strProjName = "atlservi";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;

	RemoveTestFiles( m_strProjLoc );

	CreateDirectory(m_strProjLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );

	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}

	LogSubTestHeader("BuildProjectAndVerify");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

 	LogSubTestHeader("SaveProject");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Save()));

	LogSubTestHeader("Close Project");
	BOOL bSaveProjBeforeClose = FALSE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	LogSubTestHeader("Open Project");
	if (!VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)))
	{
		m_pLog->RecordFailure("Could not open the project");
		return FALSE;
	}

	LogSubTestHeader("BuildProjectAgainAndVerify");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( FALSE ) );

	LogSubTestHeader("Build Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change resources and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));

	LogSubTestHeader("Change source file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".cpp", szNewBasePathName + ".cpp", m_strProjName + ".cpp"));

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change idl file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".idl", szNewBasePathName + ".idl", m_strProjName + ".idl"));

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

