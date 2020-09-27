///////////////////////////////////////////////////////////////////////////////
//	cust1.CPP
//
//	Created by :			Date :
//		Brickb 				3/12/98
//
//	Description :
//		Implementation of the CCustomBuildTest1 class

#include "stdafx.h"
#include "cust1.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
int iWaitForBuild = 3;	// Minutes to wait for the build to complete

IMPLEMENT_TEST(CCustomBuildTest1, CProjectTestSet, "Custom Build Basics", -1, CCustomBuildSubSuite)

void CCustomBuildTest1::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

////////////////////////////////////////////////////////////////////
//	CCustomBuildTest1

void CCustomBuildTest1::Run(void)
{	 
	XSAFETY;
	EXPECT_TRUE( BasicTest() );
	XSAFETY;

}


BOOL CCustomBuildTest1::BasicTest( void )
{
	LogTestHeader("Basic Custom Build Rules");
	// slap together a basic project for this test
	EXPECT_TRUE(GetProjReady());

	//lets put a very simple custom build things in the Prj Settings Dialog
	EXPECT_TRUE(SetCustomBuild());
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	// should not see a build for the above changes
	EXPECT_TRUE(VerifyBuildOccured(FALSE));
	// Check to see of the text made it into the Output window
	EXPECT_TRUE(VerifyBuildString(GetLocString(IDSS_PB_CUSTBLD_DESC_1)));

	// Now lets try all the Macro's at once to see what they get us
	EXPECT_TRUE(SetAllMacros());
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	// should not see a build for the above changes
	EXPECT_TRUE(VerifyBuildOccured(FALSE));

	// close the workspace 
	prj.Close(TRUE);

	return TRUE;
}

BOOL CCustomBuildTest1::SetCustomBuild( void )
{
	CString desc = GetLocString(IDSS_PB_CUSTBLD_DESC_1) + GetLocString(IDSS_PB_CUSTBLD_DESC_2); 
	CStringArray bld_cmds;
	bld_cmds.Add("echo \"ProjDir=$(ProjDir)\" > .\\$(OutDir)\\echo1.out");
	bld_cmds.Add("echo \"TargetName=$(TargetName)\" > .\\$(OutDir)\\echo2.out");
	CStringArray output_files;
	output_files.Add("$(OutDir)\\echo1.out");
	output_files.Add("$(OutDir)\\echo2.out");
	ConfigStrArray configs;
	configs.Add(m_strProjName, PLATFORM_WIN32_X86, BUILD_RELEASE );

	if ( !VERIFY_TEST_SUCCESS(prj.SetCustomBuildOptions(desc, bld_cmds, output_files, configs )))
	{
		m_pLog->RecordFailure("Change custom build options.");
		return FALSE;
	}
	return TRUE;
}

BOOL CCustomBuildTest1::SetAllMacros( void )
{
	CString desc = GetLocString(IDSS_PB_CUSTBLD_DESC_1) + GetLocString(IDSS_PB_CUSTBLD_DESC_2); 
	CStringArray bld_cmds;
	bld_cmds.Add("echo ProjDir=$(ProjDir) >$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo TargetName=$(TargetName) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo IntermediateDir=$(IntDir) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo OutputDir=$(OutDir) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo TargetDir=$(TargetDir) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo ProjectDir=$(ProjDir) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo WorkspaceDir=$(WkspDir) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo MSDEVDir=$(MSDEVDIR) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo RemoteDir=$(RemoteDir) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo TargetPath=$(TargetPath) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo TargetName=$(TargetName) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo InputPath=$(InputPath) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo InputName=$(InputName) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo WorkspaceName=$(WkspName) >>$(ProjDir)\\echo3.out");
	bld_cmds.Add("echo RemoteTarget=$(REMOTETARGETPATH) >>$(ProjDir)\\echo3.out");

	CStringArray output_files;
	output_files.Add("$(ProjDir)\\echo3.out");

	ConfigStrArray configs;
	configs.Add(m_strProjName, PLATFORM_WIN32_X86, BUILD_RELEASE );

	if ( !VERIFY_TEST_SUCCESS(prj.SetCustomBuildOptions(desc, bld_cmds, output_files, configs )))
	{
		m_pLog->RecordFailure("Change custom build options.");
		return FALSE;
	}
	return TRUE;
}


BOOL CCustomBuildTest1::GetProjReady( void )
{

		// Create a new Project
	m_strProjName = GetLocString(IDSS_FUNNY_PROJ_NAME);
	m_strProjLoc  = m_strCWD + _T("apps\\") + m_strProjName + _T("\\");
	CString szSRCFileLoc	= m_strCWD + _T("src\\") + _T("customb1") + _T("\\");
	CString szHFiles		= szSRCFileLoc + _T("*.h");
	CString szCPPFiles		= szSRCFileLoc + _T("*.cpp");
	CString szRCFiles		= szSRCFileLoc + _T("*.rc");
	CString szICOFiles		= szSRCFileLoc + _T("*.ico");

	// Create a clean copy of the test workspace tree
	KillAllFiles(m_strProjLoc + _T("Debug\\"), TRUE);
	KillAllFiles(m_strProjLoc + _T("Release\\"), TRUE);
	KillAllFiles(m_strProjLoc, TRUE);

	if (!VERIFY_TEST_SUCCESS( prj.NewProject( UIAW_PT_APP, m_strProjName, m_strProjLoc, PLATFORMS)))
	{
 		m_pLog->RecordFailure("Could not create an Application project");
		return FALSE;
	}

	// Suck in all the project files we have created for this test.
	EXPECT_TRUE(CopyProjectSources(szSRCFileLoc, m_strProjLoc));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szCPPFiles, m_strProjName )));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szRCFiles, m_strProjName )));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szHFiles, m_strProjName )));
	EXPECT_TRUE(VERIFY_TEST_SUCCESS( prj.AddFiles( szICOFiles, m_strProjName )));

	LogSubTestHeader("BuildProjectCustomb1");
	EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
	return VerifyBuildOccured( TRUE );
}