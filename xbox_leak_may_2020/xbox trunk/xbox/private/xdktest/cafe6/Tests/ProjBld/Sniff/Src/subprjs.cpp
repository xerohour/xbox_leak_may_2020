///////////////////////////////////////////////////////////////////////////////
//	SUBPRJS.CPP
//
//	Created by :			Date :
//		ThuyT				02/06/96
//
//	Description :
//		Implementation of the CPrjSubPrjTest class
#include "stdafx.h"
#include "prjcases.h"
#include "subprjs.h"
#include <io.h>
 

#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPrjSubPrjTest, CTest, "Verify Subproject", -1, CSniffDriver)
					   
////////////////////////////////////////////////////////////////////
//	CPrjSubPrjTest


void CPrjSubPrjTest::Run(void)
{
	CPrjUtil	PrjUtil;
	
	szProjEXELoc		 = m_strCWD + PROJNAME + "\\";
	szDLLProj			 = "DLL";
	szProjDLLLoc		 = szProjEXELoc + szDLLProj + "\\";
	szDLLCPPFiles		 = szProjDLLLoc + "*.cpp";

	m_pLog->RecordInfo("Verifying building sub-projects...");
	m_pLog->RecordInfo("Open an existing project.");
	EXPECT( OpenProject() );

	// Check to see if DLL subproject is already created yet.  ( REVIEW thuyt)
	if ( prj.SetTarget( DLLRELEASETARGET) == ERROR_ERROR ) {
		m_pLog->RecordInfo("Insert a DLL as a sub-project.");
		EXPECT( InsertProject() );
		m_pLog->RecordInfo("Add files to the sub-project.");
		EXPECT( AddFilesToDLLProject() );
		prj.SetProjProperty(CT_GENERAL,VPROJ_IDC_OUTDIRS_TRG, "Release");
		prj.SetProjProperty(CT_GENERAL,VPROJ_IDC_OUTDIRS_INT, "Release");
		}
	else {
			PrjUtil.TouchFile( szProjEXELoc + "Hello1.h" );
	}
	m_pLog->RecordInfo("EXERELEASETARGET is %s", EXERELEASETARGET);
	m_pLog->RecordInfo("DLLRELEASETARGET is %s", DLLRELEASETARGET);

	prj.SetTarget(EXERELEASETARGET);
	PrjUtil.RemoveToolsDir(m_strCWD + TOOLSDIR);
	m_pLog->RecordInfo("Build the project...");
	BuildProject();
	VerifySubproject();
	UIWB.SaveAll();
	//prj.ExportMakefile();
	EXPECT( CloseProject() );
	VerifyBLDs();
	m_pLog->RecordInfo("Successfully build sub-projects in the expected order.");

}

BOOL CPrjSubPrjTest::InsertProject(void)
{
	if (!VERIFY_TEST_SUCCESS( prj.InsertProject( DLL, szDLLProj, PLATFORMS, SUB_PROJ, PROJNAME))) {
		m_pLog->RecordFailure("Insert a DLL as a subproject");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjSubPrjTest::AddFilesToDLLProject(void)
{
	CString szSRCFileLoc = m_strCWD + "src\\";
	CString szSharedFiles = szProjEXELoc + "Hello1.cpp";

	CopyFile(szSRCFileLoc + "DLL.cpp", szProjDLLLoc + "DLL.cpp", FALSE);
	CopyFile(szSRCFileLoc + "DLL.h", szProjDLLLoc + "DLL.h", FALSE);
	
	if (!VERIFY_TEST_SUCCESS( prj.AddFiles( szDLLCPPFiles , szDLLProj ))) {
		m_pLog->RecordFailure("Couldn't add CPP files to DLL project");
		return FALSE;
	}

	if (!VERIFY_TEST_SUCCESS( prj.AddFiles( szSharedFiles, szDLLProj ))) {
		m_pLog->RecordFailure("Couldn't add shared CPP files DLL to project");
		return FALSE;
	}
	return TRUE;
}	

BOOL CPrjSubPrjTest::VerifySubproject(void)
{
	CPrjUtil	PrjUtil;

	// Get the output window contents
	CString szBldLog = PrjUtil.GetBldLog();
	CString exe =EXERELEASETARGET ;
	CString dll = DLLRELEASETARGET ;
	dll.MakeLower();
	exe.MakeLower();

	// For some reason the output window text is always lower case when extracted.
	int iFoundDLLLibrary = szBldLog.Find(dll);
	int iFoundEXEBinary = szBldLog.Find(exe);

	if ( iFoundDLLLibrary < 0 ) {
		m_pLog->RecordFailure("DLL subproject didn't get built. Expected : %s", DLLRELEASETARGET);
		return FALSE;
	}
	if ( iFoundEXEBinary < 0 ) {
		m_pLog->RecordFailure("Hello EXE didn't get built. Expected : %s", EXERELEASETARGET);
		return FALSE;
	}

	if ( iFoundDLLLibrary > iFoundEXEBinary ) {
		m_pLog->RecordFailure("DLL subproject should be built first.");
		return FALSE;
	}

	// Make sure DLL.idb file is generated for dep. purposes.
	CFileStatus status;
	CString szDLLIDB = szProjDLLLoc + GetLocString(IDSS_BUILD_RELEASE) +"\\vc60.idb";
	if (!CFile::GetStatus(szDLLIDB, status))  {
		m_pLog->RecordFailure("SubProject failed: %s file not found", szDLLIDB);
		m_pLog->RecordFailure("SubProject failed: build system doesn't pass the /FD switch to the compiler.");
		return FALSE;
	}

	return TRUE;
}
BOOL CPrjSubPrjTest::VerifyBLDs( void )
{
// Make BLD files are generate for each project
	CFileStatus status;
	CString szEXEBLD = szProjEXELoc + PROJNAME + PRJEXTENSION;
	CString szDLLBLD = szProjDLLLoc + "DLL" + PRJEXTENSION;
	if (!CFile::GetStatus(szEXEBLD, status))  {
		m_pLog->RecordFailure("Subproject failed: %s file not found", szEXEBLD);
		return FALSE;
	}

	if (!CFile::GetStatus(szDLLBLD, status))  {
		m_pLog->RecordFailure("Subproject failed: %s file not found", szDLLBLD);
		return FALSE;
	}
// Make sure DSW file contains all the projects
	int iFoundEXEProject = FindStrInFile(szProjEXELoc + PROJNAME + WSPEXTENSION, PROJNAME);
	int iFoundDLLProject = FindStrInFile(szProjEXELoc + PROJNAME + WSPEXTENSION, "DLL");

	if (( iFoundEXEProject != 0 ) && ( iFoundDLLProject != 0 ))
		return TRUE;
	else {
		m_pLog->RecordFailure("The DSW file doesn't include all projects.");
		return FALSE;
	}

	return TRUE;

}


BOOL CPrjSubPrjTest::OpenProject( void  )
{
	if (!VERIFY_TEST_SUCCESS( prj.Open(szProjEXELoc + PROJNAME))) {
		m_pLog->RecordFailure("Open the project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjSubPrjTest::BuildProject( void )
{
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	
	if (!VERIFY_TEST_SUCCESS( prj.RebuildAll(iWaitForBuild))) {
		m_pLog->RecordFailure("Build Project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjSubPrjTest::CloseProject( void )
{
	BOOL bSaveProjBeforeClose = TRUE; 
	
	if (!VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose))) {
		m_pLog->RecordFailure("Close the project");
		return FALSE;
	}
//	m_pLog->RecordInfo("Closing the project successfully");
	return TRUE;
}	

