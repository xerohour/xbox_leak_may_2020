///////////////////////////////////////////////////////////////////////////////
//	CUSTBLD.CPP
//
//	Created by :			Date :
//		ThuyT				02/06/96
//
//	Description :
//		Implementation of the CPrjCustomBuildTest class

#include "stdafx.h"
#include "prjcases.h"
#include "custbld.h"
 
#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPrjCustomBuildTest, CTest, "Verify Custom Build Rules", -1, CSniffDriver)
////////////////////////////////////////////////////////////////////
//	CPrjCustomBuildTest

void CPrjCustomBuildTest::Run(void)
{
	szProjEXELoc = m_strCWD + PROJNAME + "\\";
	m_pLog->RecordInfo("Verify custom build rules...");
	EXPECT( OpenProject() );
	prj.SetTarget(EXERELEASETARGET);
	prj.SetProjProperty(CT_GENERAL,VPROJ_IDC_OUTDIRS_TRG, NEWEXEOUTDIR);
	m_pLog->RecordInfo("Set custom build rules.");
	SetCustomBuild();
	m_pLog->RecordInfo("Build the project....");
	BuildProject();
	m_pLog->RecordInfo("Verify the resutls...");
	VerifyCustomBuild();
	EXPECT( CloseProject() );
	// Reload the project. Make sure Multi custom build rules are read back properly.
 	m_pLog->RecordInfo("Reload the project to make sure multi custom build rules are read back properly....");
	KillFile(szProjEXELoc + NEWEXEOUTDIR,"echo1.out");
	KillFile(szProjEXELoc + NEWEXEOUTDIR,"echo2.out");
	Sleep(250);
	EXPECT( OpenProject() );    
	m_pLog->RecordInfo("Build the project....");
	BuildProject();
	m_pLog->RecordInfo("Verify the resutls...");
	VerifyCustomBuild();
	EXPECT( CloseProject() );

	m_pLog->RecordInfo("Successfully set and run custom build rules.");
}

BOOL CPrjCustomBuildTest::SetCustomBuild( void )
{
	CString desc = "Generate echo1.out and echo2.out files...";
	CStringArray bld_cmds;
	bld_cmds.Add("echo \"ProjDir=$(ProjDir)\" > .\\$(OutDir)\\echo1.out");
	bld_cmds.Add("echo \"TargetName=$(TargetName)\" > .\\$(OutDir)\\echo2.out");
	CStringArray output_files;
	output_files.Add("$(OutDir)\\echo1.out");
	output_files.Add("$(OutDir)\\echo2.out");
	ConfigStrArray configs;
	configs.Add(PROJNAME, PLATFORM_WIN32_X86, BUILD_RELEASE );

	if ( !VERIFY_TEST_SUCCESS(prj.SetCustomBuildOptions(desc, bld_cmds, output_files, configs )))
	{
		m_pLog->RecordFailure("Change custom build options.");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjCustomBuildTest::VerifyCustomBuild( void )
{
// Make sure the outfile is generated.
	CFileStatus status;
	CString szOutName1 = szProjEXELoc + NEWEXEOUTDIR + "\\echo1.out";
	CString szOutName2 = szProjEXELoc + NEWEXEOUTDIR + "\\echo2.out";
	if (!CFile::GetStatus(szOutName1, status))  {
		m_pLog->RecordFailure("Custom build failed: %s file not found", szOutName1);
		return FALSE;
	}

	if (!CFile::GetStatus(szOutName2, status))  {
		m_pLog->RecordFailure("Custom build failed: %s file not found", szOutName2);
		return FALSE;
	}
return TRUE;

}

BOOL CPrjCustomBuildTest::OpenProject( void  )
{
	if (!VERIFY_TEST_SUCCESS( prj.Open(szProjEXELoc + PROJNAME))) {
		m_pLog->RecordFailure("Open the project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjCustomBuildTest::BuildProject( void )
{
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	
	if (!VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild))) {
		m_pLog->RecordFailure("Build Project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjCustomBuildTest::CloseProject( void )
{
	BOOL bSaveProjBeforeClose = TRUE; 
	
	if (!VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose))) {
		m_pLog->RecordFailure("Close the project");
		return FALSE;
	}
//	m_pLog->RecordInfo("Closing the project successfully");
	return TRUE;
}	
