///////////////////////////////////////////////////////////////////////////////
//	BldWin32.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CBuildWin32Test class

#include "stdafx.h"
#include "BldWin32.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBuildWin32Test, CProjectTestSet, "Building Win32 Projects", -1, CBuildSubSuite)

// Global variables
static CString cstrRelease  =  GetLocString(IDSS_BUILD_RELEASE);
static CString cstrDebug  =  GetLocString(IDSS_BUILD_DEBUG);
CString RELEASETARGET = " - Win32 " + cstrRelease;
CString DEBUGTARGET = " - Win32 " + cstrDebug;
/*
BOOL CBuildWin32Test::RunAsDependent(void)
{
	// name of the project file
	CString strProjFile = m_strCWD + PROJNAME + "\\" + PROJMAKFILE;

	// if the project file exists, there's no need to run this test as a dependency
	if (_access(strProjFile, 0) != -1) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}
*/
void CBuildWin32Test::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CBuildWin32Test

void CBuildWin32Test::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( CreateAndBuildWin32App() );
	XSAFETY;
	EXPECT_TRUE( CreateAndBuildConsoleApp() );
	XSAFETY;
	EXPECT_TRUE( CreateAndBuildWin32Dll() );
	XSAFETY;
	EXPECT_TRUE( CreateAndBuildWin32StaticLib() );
	XSAFETY;

}

BOOL CBuildWin32Test::CreateAndBuildWin32App( void )
{
	LogTestHeader("CreateAndBuildWin32App");

	m_strProjName = "Hello";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szCPPFiles	= m_strProjLoc + "*.cpp";
	CString szRCFiles	= m_strProjLoc + "*.rc";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szAppFile = m_strProjLoc + m_strProjName + ".cpp";
	CString szNewFile = szNewSRCFileLoc + m_strProjName + ".cpp";

	RemoveTestFiles( m_strProjLoc );

	LogSubTestHeader("Creating a Win32 Application project");
	if (!VERIFY_TEST_SUCCESS( prj.NewProject( EXE, m_strProjName, m_strProjLoc, PLATFORMS)))
	{
 		m_pLog->RecordFailure("Could not create an Application project");
		return FALSE;
	}

	LogSubTestHeader("Insert files into the project");
	
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );
	// Add source files files
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.AddFiles( szCPPFiles, m_strProjName )));
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.AddFiles( szRCFiles, m_strProjName )));

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

	LogSubTestHeader("ChangeProjectFiles");
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".cpp"));

	LogSubTestHeader("Switch to Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

BOOL CBuildWin32Test::CreateAndBuildConsoleApp( void )
{
	LogTestHeader("CreateAndBuildConsoleApp using .c files and nested headers");

	m_strProjName = "consol";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szCFiles	= m_strProjLoc + "*.c";
	CString szHeaderFiles	= m_strProjLoc + "*.h";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szAppFile = m_strProjLoc + m_strProjName + ".c";
	CString szNewFile = szNewSRCFileLoc + m_strProjName + ".c";
	CString szHeader = m_strProjLoc + m_strProjName + ".h";
	CString szNewHeader = szNewSRCFileLoc + m_strProjName + ".h";
	CString szHeader2 = m_strProjLoc + m_strProjName + "2.h";
	CString szNewHeader2 = szNewSRCFileLoc + m_strProjName + "2.h";

	RemoveTestFiles( m_strProjLoc );

	LogSubTestHeader("Creating a Win32 Console Application project");
	if (!VERIFY_TEST_SUCCESS( prj.NewProject( CONSOLE_APP, m_strProjName, m_strProjLoc, PLATFORMS)))
	{
 		m_pLog->RecordFailure("Could not create an Application project");
		return FALSE;
	}

	LogSubTestHeader("Insert files into the project");
	
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );

	// Add source files	and headers
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.AddFiles( szCFiles, m_strProjName )));
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.AddFiles( szHeaderFiles, m_strProjName )));

	LogSubTestHeader("BuildProjectAndVerify");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Switch to Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
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

	LogSubTestHeader("Switch to Debug target");
	prj.SetTarget(m_strProjName + DEBUGTARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( FALSE ) );

	LogSubTestHeader("Change header and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szHeader, szNewHeader, m_strProjName + ".c"));

	LogSubTestHeader("Change source: .c and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".c"));

	LogSubTestHeader("Change nested header and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szHeader2, szNewHeader2, m_strProjName + ".c"));

	LogSubTestHeader("Switch to Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}


BOOL CBuildWin32Test::CreateAndBuildWin32Dll( void )
{
	LogTestHeader("CreateAndBuildWin32Dll");

	m_strProjName = "a_dll";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szCPPFiles	= m_strProjLoc + "*.cpp";
	CString szHeaderFiles	= m_strProjLoc + "*.h";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szAppFile = m_strProjLoc + m_strProjName + ".cpp";
	CString szNewFile = szNewSRCFileLoc + m_strProjName + ".cpp";
	CString szHeader = m_strProjLoc + m_strProjName + ".h";
	CString szNewHeader = szNewSRCFileLoc + m_strProjName + ".h";

	RemoveTestFiles( m_strProjLoc );

	LogSubTestHeader("Creating a Win32 Dll project");
	if (!VERIFY_TEST_SUCCESS( prj.NewProject( DLL, m_strProjName, m_strProjLoc, PLATFORMS)))
	{
 		m_pLog->RecordFailure("Could not create an Application project");
		return FALSE;
	}

	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );

	LogSubTestHeader("Insert files into the project");
	// Add source files and headers
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szCPPFiles, m_strProjName )));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szHeaderFiles, m_strProjName )));

	LogSubTestHeader("BuildProject");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

 	LogSubTestHeader("SaveProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Save()));

	LogSubTestHeader("Close Project");
	BOOL bSaveProjBeforeClose = FALSE; 
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	LogSubTestHeader("Open Project");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)));

	LogSubTestHeader("BuildProjectAgain");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( FALSE ) );

	LogSubTestHeader("Change source: .cpp and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".cpp"));

	LogSubTestHeader("Switch to Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change header and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szHeader, szNewHeader, m_strProjName + ".cpp"));

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));


	// Dll with def file and DllMain
	m_strProjName = "b_dll";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szCFiles	= m_strProjLoc + "*.c";
	szHeaderFiles	= m_strProjLoc + "*.h";
	CString szDefFiles	= m_strProjLoc + "*.def";
	szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	szAppFile = m_strProjLoc + m_strProjName + ".c";
	szNewFile = szNewSRCFileLoc + m_strProjName + ".c";
	szHeader = m_strProjLoc + m_strProjName + ".h";
	szNewHeader = szNewSRCFileLoc + m_strProjName + ".h";
	CString szDefFile = m_strProjLoc + m_strProjName + ".def";
	CString szNewDefFile = szNewSRCFileLoc + m_strProjName + ".def";

	RemoveTestFiles( m_strProjLoc );

	LogSubTestHeader("Creating a Win32 Dll project with def file");
	if (!VERIFY_TEST_SUCCESS( prj.NewProject( DLL, m_strProjName, m_strProjLoc, PLATFORMS)))
	{
 		m_pLog->RecordFailure("Could not create an Application project");
		return FALSE;
	}

	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );

	LogSubTestHeader("Insert files into the project");
	// Add source files, def files and headers
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szCFiles, m_strProjName )));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szHeaderFiles, m_strProjName )));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szDefFiles, m_strProjName )));

 	LogSubTestHeader("SaveProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Save()));

	LogSubTestHeader("BuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	LogSubTestHeader("Open Project");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)));

	LogSubTestHeader("BuildProjectAgain");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( FALSE ) );

	LogSubTestHeader("Switch to Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change source: .c and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".c"));

	LogSubTestHeader("Change header and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szHeader, szNewHeader, m_strProjName + ".c"));

	LogSubTestHeader("Change def file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szDefFile, szNewDefFile, GetLocString(IDSS_OUTPUTWIN_LINKING)));
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_COMPILING), FALSE ) < 0 );

 	LogSubTestHeader("SaveProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Save()));

	LogSubTestHeader("Close Project");
	bSaveProjBeforeClose = FALSE; 
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

BOOL CBuildWin32Test::CreateAndBuildWin32StaticLib( void )
{
	LogTestHeader("CreateAndBuildWin32StaticLib");

	m_strProjName = "stat_lib";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szCPPFiles	= m_strProjLoc + "*.cpp";
	CString szHeaderFiles	= m_strProjLoc + "*.h";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szAppFile = m_strProjLoc + m_strProjName + ".cpp";
	CString szNewFile = szNewSRCFileLoc + m_strProjName + ".cpp";
	CString szHeader = m_strProjLoc + m_strProjName + ".h";
	CString szNewHeader = szNewSRCFileLoc + m_strProjName + ".h";

	RemoveTestFiles( m_strProjLoc );

	LogSubTestHeader("Creating a Win32 Static Library project");
	if (!VERIFY_TEST_SUCCESS( prj.NewProject( STATIC_LIBRARY, m_strProjName, m_strProjLoc, PLATFORMS)))
	{
 		m_pLog->RecordFailure("Could not create project");
		return FALSE;
	}

	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );

	LogSubTestHeader("Insert files into the project");
	// Add source files and headers
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szCPPFiles, m_strProjName )));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szHeaderFiles, m_strProjName )));

	LogSubTestHeader("BuildProject");
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_LINKING), FALSE ) < 0 );

 	LogSubTestHeader("SaveProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Save()));

	LogSubTestHeader("Close Project");
	BOOL bSaveProjBeforeClose = FALSE; 
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	LogSubTestHeader("Open Project");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Open(m_strProjLoc + m_strProjName)));

	LogSubTestHeader("BuildProjectAgain");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( FALSE ) );

	LogSubTestHeader("Change header and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szHeader, szNewHeader, m_strProjName + ".cpp"));
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_LINKING), FALSE ) < 0 );

	LogSubTestHeader("Change source: .cpp and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".cpp"));
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_LINKING), FALSE ) < 0 );

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_LINKING), FALSE ) < 0 );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}



