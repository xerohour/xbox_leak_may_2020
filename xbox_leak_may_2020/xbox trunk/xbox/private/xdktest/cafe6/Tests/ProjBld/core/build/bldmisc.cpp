///////////////////////////////////////////////////////////////////////////////
//	BldWin32.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CBuildMiscTest class

#include "stdafx.h"
#include "BldMisc.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBuildMiscTest, CProjectTestSet, "Building Miscellaneous Projects", -1, CBuildSubSuite)

void CBuildMiscTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CBuildMiscTest

void CBuildMiscTest::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( CreateAndBuildMakefile() );
	XSAFETY;
	EXPECT_TRUE( BuildISAPIExtensionWizard() );
	XSAFETY;

}


BOOL CBuildMiscTest::CreateAndBuildMakefile( void )
{
	LogTestHeader("CreateAndBuildMakefile");

	m_strProjName = "mkfile01";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";

	RemoveTestFiles( m_strProjLoc );

	LogSubTestHeader("Creating a Makefile project");
	if (!(VERIFY_TEST_SUCCESS( prj.NewProject( UIAW_PT_MAKEFILE, m_strProjName, m_strProjLoc, PLATFORMS)) 
		  && VERIFY_TEST_SUCCESS( prj.SetBuildCommand("echo Build makefile project") )))
	{
 		m_pLog->RecordFailure("Could not create project");
		return FALSE;
	}

	LogSubTestHeader("BuildProject");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildString( "Build makefile project") > 0 );

 	LogSubTestHeader("SaveProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Save()));

	LogSubTestHeader("Close Project");
	BOOL bSaveProjBeforeClose = FALSE; 
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	LogSubTestHeader("Open Project");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)));

	LogSubTestHeader("BuildProjectAgain");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildString( "Build makefile project") > 0 );

	LogSubTestHeader("Change the command line and build the project");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.SetBuildCommand("echo Build makefile project second time") ));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildString( "Build makefile project second time") > 0 );

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildString( "Build makefile project") > 0 );

	LogSubTestHeader("Build Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.SetBuildCommand("echo Build makefile project release") ));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildString( "Build makefile project release") > 0 );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}


BOOL CBuildMiscTest::BuildISAPIExtensionWizard( void )
{
	LogTestHeader("BuildISAPIExtensionWizard");

	m_strProjName = "isapiew";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;
	CString szNewFile = szNewBasePathName + ".cpp";

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

	LogSubTestHeader("Change header and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".h", szNewBasePathName + ".h", m_strProjName + ".cpp"));

	LogSubTestHeader("Change resources and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));

	LogSubTestHeader("Change source file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".cpp", szNewBasePathName + ".cpp", m_strProjName + ".cpp"));

	LogSubTestHeader("Change def file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".def", szNewBasePathName + ".def", GetLocString(IDSS_OUTPUTWIN_LINKING)));
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_COMPILING), FALSE ) < 0 );

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Build Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

