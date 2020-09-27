///////////////////////////////////////////////////////////////////////////////
//	BldWin32.CPP
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Implementation of the CBuildMFCTest class

#include "stdafx.h"
#include "BldMFC.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBuildMFCTest, CProjectTestSet, "Building MFC Projects", -1, CBuildSubSuite)

void CBuildMFCTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CBuildMFCTest

void CBuildMFCTest::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( BuildMFCAppWizardExe() );
	XSAFETY;
	EXPECT_TRUE( BuildMFCAppWizardDll() );
	XSAFETY;
	EXPECT_TRUE( BuildMFCActiveXControlWizard() );
	XSAFETY;
	EXPECT_TRUE( BuildMFCAppWizardExeCSHelp());
	XSAFETY;
	EXPECT_TRUE( BuildMFCAppWizardExeStaticMFC());
	XSAFETY;

}


BOOL CBuildMFCTest::BuildMFCAppWizardExe( void )
{
	LogTestHeader("BuildMFCAppWizardExe");

	m_strProjName = "mfcawexe";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szProjResLoc	= m_strProjLoc + "res\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szResFileLoc	= szSRCFileLoc + "res\\";
	CString szNewResFileLoc	= szNewSRCFileLoc + "res\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;
	CString szAppFile = szBasePathName + ".cpp";
	CString szNewFile = szNewBasePathName + ".cpp";

	RemoveTestFiles( szProjResLoc );
	RemoveTestFiles( m_strProjLoc );

	CreateDirectory(m_strProjLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );
	CreateDirectory(szProjResLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szResFileLoc, szProjResLoc ) );

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
	EXPECT_TRUE( CopyProjectSources( szNewResFileLoc, szProjResLoc ) );
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));

	LogSubTestHeader("Build Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".h", szNewBasePathName + ".h", m_strProjName + ".cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Doc.cpp", szNewBasePathName + "Doc.cpp", m_strProjName + "Doc.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "View.cpp", szNewBasePathName + "View.cpp", m_strProjName + "View.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Doc.h", szNewBasePathName + "Doc.h", m_strProjName + "Doc.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "View.h", szNewBasePathName + "View.h", m_strProjName + "View.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "ChildFrm.h", szNewSRCFileLoc + "ChildFrm.h", "ChildFrm.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "ChildFrm.cpp", szNewSRCFileLoc + "ChildFrm.cpp", "ChildFrm.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "MainFrm.h", szNewSRCFileLoc + "MainFrm.h", "MainFrm.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "MainFrm.cpp", szNewSRCFileLoc + "MainFrm.cpp", "MainFrm.cpp"));

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

BOOL CBuildMFCTest::BuildMFCAppWizardDll( void )
{
	LogTestHeader("BuildMFCAppWizardDll");

	m_strProjName = "mfcawdll";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szProjResLoc	= m_strProjLoc + "res\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szResFileLoc	= szSRCFileLoc + "res\\";
	CString szNewResFileLoc	= szNewSRCFileLoc + "res\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;
	CString szAppFile = szBasePathName + ".cpp";
	CString szNewFile = szNewBasePathName + ".cpp";

	RemoveTestFiles( szProjResLoc );
	RemoveTestFiles( m_strProjLoc );

	CreateDirectory(m_strProjLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );
	CreateDirectory(szProjResLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szResFileLoc, szProjResLoc ) );

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

	LogSubTestHeader("Switch to Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change def file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".def", szNewBasePathName + ".def", GetLocString(IDSS_OUTPUTWIN_LINKING)));
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_COMPILING), FALSE ) < 0 );

	LogSubTestHeader("Change resources and build the project");
	EXPECT_TRUE( CopyProjectSources( szNewResFileLoc, szProjResLoc ) );
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".h", szNewBasePathName + ".h", m_strProjName + ".cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".cpp"));

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

BOOL CBuildMFCTest::BuildMFCActiveXControlWizard( void )
{
	LogTestHeader("BuildMFCActiveXControlWizard");
	m_strProjName = "mfcaxcw";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;
	CString szAppFile = szBasePathName + ".cpp";
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
	EXPECT_TRUE( VerifyBuildString( "Registering") > 0 );

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

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Ctl.h", szNewBasePathName + "Ctl.h", m_strProjName + "Ctl.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Ppg.h", szNewBasePathName + "Ppg.h", m_strProjName + "Ppg.cpp"));

	LogSubTestHeader("Change resources and build the project");
	szNewFile = szNewBasePathName + ".ico";
	CString szCurrentFile = szBasePathName + ".ico";
	CopyFile(szNewFile, szCurrentFile, FALSE);
	SetFileAttributes(szCurrentFile, FILE_ATTRIBUTE_NORMAL ); 
	TouchFile( szCurrentFile );
	szNewFile = szNewBasePathName + "Ctl.bmp";
	szCurrentFile = szBasePathName + "Ctl.bmp";
	CopyFile(szNewFile, szCurrentFile, FALSE);
	SetFileAttributes(szCurrentFile, FILE_ATTRIBUTE_NORMAL ); 
	TouchFile( szCurrentFile );
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));
	EXPECT_TRUE( VerifyBuildString( "Registering") > 0 );

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".h", szNewBasePathName + ".h", m_strProjName + ".cpp"));

	LogSubTestHeader("Change odl file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".odl", szNewBasePathName + ".odl", m_strProjName + ".odl"));
	EXPECT_TRUE( VerifyBuildString( "Registering") > 0 );

	LogSubTestHeader("Change def file and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".def", szNewBasePathName + ".def", GetLocString(IDSS_OUTPUTWIN_LINKING)));
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_COMPILING), FALSE ) < 0 );
	EXPECT_TRUE( VerifyBuildString( "Registering") > 0 );

	LogSubTestHeader("Switch to Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );
	EXPECT_TRUE( VerifyBuildString( "Registering") > 0 );

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Ctl.cpp", szNewBasePathName + "Ctl.cpp", m_strProjName + "Ctl.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Ppg.cpp", szNewBasePathName + "Ppg.cpp", m_strProjName + "Ppg.cpp"));

	szNewFile = szNewBasePathName + ".cpp";
	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".cpp"));

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

BOOL CBuildMFCTest::BuildMFCAppWizardExeCSHelp( void )
{
	LogTestHeader("BuildMFCAppWizardExeCSHelp");

	m_strProjName = "mfcawex2";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szProjResLoc	= m_strProjLoc + "res\\";
	CString szProjHelpLoc	= m_strProjLoc + "hlp\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szResFileLoc	= szSRCFileLoc + "res\\";
	CString szNewResFileLoc	= szNewSRCFileLoc + "res\\";
	CString szHelpFileLoc	= szSRCFileLoc + "hlp\\";
	CString szNewHelpFileLoc	= szNewSRCFileLoc + "hlp\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;
	CString szAppFile = szBasePathName + ".cpp";
	CString szNewFile = szNewBasePathName + ".cpp";

	RemoveTestFiles( szProjResLoc );
	RemoveTestFiles( szProjHelpLoc );
	RemoveTestFiles( m_strProjLoc );

	CreateDirectory(m_strProjLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );
	CreateDirectory(szProjResLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szResFileLoc, szProjResLoc ) );
	CreateDirectory(szProjHelpLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szHelpFileLoc, szProjHelpLoc ) );

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
	EXPECT_TRUE( VerifyBuildString( "Making help file") > 0 );

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
	EXPECT_TRUE( CopyProjectSources( szNewResFileLoc, szProjResLoc ) );
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));

	LogSubTestHeader("Build Release target");
	prj.SetTarget(m_strProjName + RELEASETARGET);
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );
	EXPECT_TRUE( VerifyBuildString( "Making help file") > 0 );

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );
	EXPECT_TRUE( VerifyBuildString( "Making help file") > 0 );

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".h", szNewBasePathName + ".h", m_strProjName + ".cpp"));
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "CntrItem.cpp", szNewSRCFileLoc + "CntrItem.cpp", "CntrItem.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "SrvrItem.h", szNewSRCFileLoc + "SrvrItem.h", "SrvrItem.cpp"));

	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "IpFrame.cpp", szNewSRCFileLoc + "IpFrame.cpp", "IpFrame.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "CntrItem.h", szNewSRCFileLoc + "CntrItem.h", "CntrItem.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Doc.h", szNewBasePathName + "Doc.h", m_strProjName + "Doc.cpp"));

	LogSubTestHeader("Change help files and build the project");
	EXPECT_TRUE( CopyProjectSources( szNewHelpFileLoc, szProjHelpLoc ) );
	CString szCurrentFile = szProjHelpLoc + m_strProjName + ".hpj";
	SetFileAttributes(szCurrentFile, FILE_ATTRIBUTE_NORMAL ); 
	TouchFile( szCurrentFile );
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildString( "Making help file") > 0 );

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "ChildFrm.cpp", szNewSRCFileLoc + "ChildFrm.cpp", "ChildFrm.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "SrvrItem.cpp", szNewSRCFileLoc + "SrvrItem.cpp", "SrvrItem.cpp"));

	EXPECT_TRUE( UpdateBuildVerify(m_strProjLoc + "IpFrame.h", szNewSRCFileLoc + "IpFrame.h", "IpFrame.cpp"));

	LogSubTestHeader("Change odl file and build the project");
	szNewFile = szNewBasePathName + ".odl";
	szCurrentFile = szBasePathName + ".odl";
	CopyFile(szNewFile, szCurrentFile, FALSE);
	SetFileAttributes(szCurrentFile, FILE_ATTRIBUTE_NORMAL ); 
	TouchFile( szCurrentFile );
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildString( GetLocString(IDSS_OUTPUTWIN_CREATING_LIB)) > 0 );

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}

BOOL CBuildMFCTest::BuildMFCAppWizardExeStaticMFC( void )
{
	LogTestHeader("BuildMFCAppWizardExeStaticMFC");

	m_strProjName = "mfcawex3";	//ToDo: We should get the name from file with all languages strings
	m_strProjLoc  = m_strCWD + "apps\\" + m_strProjName + "\\";
	CString szProjResLoc	= m_strProjLoc + "res\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjName + "\\";
	CString szNewSRCFileLoc	= szSRCFileLoc + "new\\";
	CString szResFileLoc	= szSRCFileLoc + "res\\";
	CString szNewResFileLoc	= szNewSRCFileLoc + "res\\";
	CString szBasePathName = m_strProjLoc + m_strProjName;
	CString szNewBasePathName = szNewSRCFileLoc + m_strProjName;
	CString szAppFile = szBasePathName + ".cpp";
	CString szNewFile = szNewBasePathName + ".cpp";

	RemoveTestFiles( szProjResLoc );
	RemoveTestFiles( m_strProjLoc );

	CreateDirectory(m_strProjLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );
	CreateDirectory(szProjResLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szResFileLoc, szProjResLoc ) );

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

	LogSubTestHeader("ReBuildProject");
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild)));
	EXPECT_TRUE( VerifyBuildOccured( TRUE ) );

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".h", szNewBasePathName + ".h", m_strProjName + ".cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Dlg.cpp", szNewBasePathName + "Dlg.cpp", m_strProjName + "Dlg.cpp"));

	LogSubTestHeader("Change resources and build the project");
	EXPECT_TRUE( CopyProjectSources( szNewResFileLoc, szProjResLoc ) );
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + ".rc", szNewBasePathName + ".rc", GetLocString(IDSS_OUTPUTWIN_RC_COMPILING)));

	LogSubTestHeader("Change source files and build the project");
	EXPECT_TRUE( UpdateBuildVerify(szBasePathName + "Dlg.h", szNewBasePathName + "Dlg.h", m_strProjName + "Dlg.cpp"));
	EXPECT_TRUE( UpdateBuildVerify(szAppFile, szNewFile, m_strProjName + ".cpp"));

	LogSubTestHeader("Close And Save");
	bSaveProjBeforeClose = TRUE; 
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose)));

	RemoveTestFiles( m_strProjLoc );
	return TRUE;
}



