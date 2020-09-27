///////////////////////////////////////////////////////////////////////////////
//	PRJCASES.CPP
//
//	Created by :			Date :
//		TomWh					10/18/93
//
//	Description :
//		Implementation of the CPrjAppTest class
//		PaulLu - Took over ownership and converted to use new utility layer
//		DorianG - Seized control and made fixes for V3 UI changes - 10/1/94
//		ThuyT - Splitted test cases into trival subsuites.  2/06/96.

#include "stdafx.h"
#include "prjcases.h"

#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

//#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPrjAppTest, CProjectTestSet, "Creating Application Project", -1, CProj1SubSuite)

// Global variables
#define PROJNAME		"Hello"
#define PROJMAKFILE		"Hello.DSP"
CString cstrRelease  =  GetLocString(IDSS_BUILD_RELEASE);
CString EXERELEASETARGET = "Hello - Win32 " + cstrRelease;
CString DLLRELEASETARGET = "DLL - Win32 " + cstrRelease;

BOOL CPrjAppTest::RunAsDependent(void)
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

void CPrjAppTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
	m_szProjEXELoc		 = m_strCWD + PROJNAME + "\\";

}

////////////////////////////////////////////////////////////////////
//	CPrjAppTest

void CPrjAppTest::Run(void)
{	 
	XSAFETY;

// 	m_pLog->RecordInfo("Creating an Application project...");
	RemoveTestFiles( m_szProjEXELoc );
	EXPECT( CreateNewProject() );
//	m_pLog->RecordInfo("Insert files to the project.");
	EXPECT( AddFilesToEXEProject() );
//	m_pLog->RecordInfo("Save the project.");
	EXPECT( SaveProject() );
	EXPECT( CloseProject() );
	m_pLog->RecordInfo("Successfully creating an Application project.");
}

BOOL CPrjAppTest::CreateNewProject( void )
{
	LogTestHeader("Creating an Application project...");
	if (!VERIFY_TEST_SUCCESS( prj.NewProject( EXE, PROJNAME, m_szProjEXELoc, PLATFORMS))) {
 		m_pLog->RecordFailure("Create an Application project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjAppTest::AddFilesToEXEProject( void )
{
	LogTestHeader("Insert files to the project.");

	CString szEXECPPFiles	= m_szProjEXELoc + "Hello*.cpp";
	CString szEXERCFiles	= m_szProjEXELoc + "*.rc";
	CString szSRCFileLoc	= m_strCWD + "src\\";
	CString szPattern		= szSRCFileLoc + "*.*";
	
	// Copy Hello.cpp Hello1.cpp and Hello.rc files to "Hello" directory
	WIN32_FIND_DATA ffdImgFile;
    HANDLE hFile = FindFirstFile(szPattern, &ffdImgFile);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
			CopyFile( szSRCFileLoc + ffdImgFile.cFileName, m_szProjEXELoc + ffdImgFile.cFileName, FALSE );
			SetFileAttributes(m_szProjEXELoc + ffdImgFile.cFileName, FILE_ATTRIBUTE_NORMAL ); 
		}
        while( FindNextFile(hFile, &ffdImgFile) );
        EXPECT( GetLastError() == ERROR_NO_MORE_FILES );
    }

	// Add Hello.cpp Hello1.cpp and Hello.rc files
	if (!VERIFY_TEST_SUCCESS( prj.AddFiles( szEXECPPFiles, PROJNAME ))) {
		m_pLog->RecordFailure("Couldn't add CPP files to EXE project");
		return FALSE;
	}
	
	if (!VERIFY_TEST_SUCCESS( prj.AddFiles( szEXERCFiles, PROJNAME ))) {
		m_pLog->RecordFailure("Couldn't add RC files to EXE project");
		return FALSE;
	}
	return TRUE;
}	

BOOL CPrjAppTest::SaveProject( void )
{
	LogTestHeader("SaveProject");

	if (!VERIFY_TEST_SUCCESS( prj.Save())) {
		m_pLog->RecordFailure("Save the project");
		return FALSE;
	}
	return TRUE;
}	

BOOL CPrjAppTest::CloseProject( void )
{
	LogTestHeader("Close Project");

	BOOL bSaveProjBeforeClose = FALSE; 
	
	if (!VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose))) {
		m_pLog->RecordFailure("Close the project");
		return FALSE;
	}
	return TRUE;
}	


