///////////////////////////////////////////////////////////////////////////////
//	SCANDEP.CPP
//
//	Created by :			Date :
//              ThuyT                   02/09/96
//
//	Description :
//              Implementation of the CPrjScanDepTest class
#include "stdafx.h"
#include "prjcases.h"
#include "scandep.h"
#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h>
#include <errno.h>
 

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPrjScanDepTest, CTest, "Verify Scan Dependencies", -1, CSniffDriver)
////////////////////////////////////////////////////////////////////
//	CPrjScanDepTest

void CPrjScanDepTest::Run(void)
{
	CPrjUtil	PrjUtil;
	szProjEXELoc = m_strCWD + PROJNAME + "\\";
	
	// Open the project
	m_pLog->RecordInfo("Verify file dependencies...");
	EXPECT( OpenProject() );

	// Initial the test case
	prj.SetTarget(EXERELEASETARGET);	
	prj.SetProjProperty(CT_GENERAL,VPROJ_IDC_OUTDIRS_INT, NEWEXEINTDIR);
	PrjUtil.RemoveToolsDir(m_strCWD + TOOLSDIR);

	// Build the project.  Expect Hello.cpp to compile.
	m_pLog->RecordInfo("Touch hello1.h");
	PrjUtil.TouchFile( szProjEXELoc + "Hello1.h" );
	m_pLog->RecordInfo("Build the project...");
	BuildProject();
	m_pLog->RecordInfo("Verify results...");
	VerifyResults( "hello1.cpp" );

	// Add Foo.cpp file to project
	m_pLog->RecordInfo("Add foo.cpp to the project.");
	prj.AddFiles( "foo.cpp", PROJNAME );

	// Build the project
	m_pLog->RecordInfo("Build the project...");
	BuildProject();

	// Build the project.  Expect Foo.cpp to compile.
	Sleep(1001); // Pause to let the time increase enough to be noticed
	m_pLog->RecordInfo("Touch foo.h");
	PrjUtil.TouchFile( szProjEXELoc + "foo.h" );
	m_pLog->RecordInfo("Build the project...");
	Sleep(1001); // Pause before build to let the touch take
	BuildProject();
	m_pLog->RecordInfo("Verify results...");
	VerifyResults( "foo.cpp" );

}


BOOL CPrjScanDepTest::VerifyResults( CString szSourceName )
{
	CPrjUtil	PrjUtil;

	// Get the output window contents
	CString szBldLog = PrjUtil.GetBldLog();
	if (szBldLog.Find(szSourceName) < 0 ) {
		m_pLog->RecordFailure("File %s didn't get compiled", szSourceName);
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjScanDepTest::OpenProject( void  )
{
	if (!VERIFY_TEST_SUCCESS( prj.Open(szProjEXELoc + PROJNAME))) {
		m_pLog->RecordFailure("Open the project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjScanDepTest::BuildProject( void )
{
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	
	if (!VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild))) {
		m_pLog->RecordFailure("Build Project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjScanDepTest::CloseProject( void )
{
	BOOL bSaveProjBeforeClose = TRUE; 
	
	if (!VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose))) {
		m_pLog->RecordFailure("Close the project");
		return FALSE;
	}
	return TRUE;
}	

