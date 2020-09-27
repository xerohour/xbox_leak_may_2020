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

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPrjAppTest, CTest, "Creating Application Project", -1, CSniffDriver)

// Global variables
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
	szProjEXELoc		 = m_strCWD + PROJNAME + "\\";

}

////////////////////////////////////////////////////////////////////
//	CPrjAppTest

void CPrjAppTest::Run(void)
{	 
	XSAFETY;

	CPrjUtil PrjUtil;


 	m_pLog->RecordInfo("Creating an Application project...");
	PrjUtil.RemoveTestFiles( szProjEXELoc );
	EXPECT( CreateNewProject() );
	m_pLog->RecordInfo("Insert files to the project.");
	EXPECT( AddFilesToEXEProject() );
	m_pLog->RecordInfo("Save the project.");
	EXPECT( SaveProject() );
	EXPECT( CloseProject() );
	m_pLog->RecordInfo("Successfully creating an Application project.");
}

BOOL CPrjAppTest::CreateNewProject( void )
{
	if (!VERIFY_TEST_SUCCESS( prj.NewProject( EXE, PROJNAME, szProjEXELoc, PLATFORMS))) {
 		m_pLog->RecordFailure("Create an Application project");
		return FALSE;
	}
	return TRUE;
}

BOOL CPrjAppTest::AddFilesToEXEProject( void )
{
	CString szEXECPPFiles	= szProjEXELoc + "Hello*.cpp";
	CString szEXECPPFile_1	= szProjEXELoc + "Hello1.cpp";
	CString szEXECPPFile_2	= szProjEXELoc + "Hello.cpp";
	CString szEXERCFiles	= szProjEXELoc + "Hello.rc";
	CString szSRCFileLoc	= m_strCWD + "src\\";
	CString szPattern		= szSRCFileLoc + "*.*";
	
	// Copy Hello.cpp Hello1.cpp and Hello.rc files to "Hello" directory
	WIN32_FIND_DATA ffdImgFile;
    HANDLE hFile = FindFirstFile(szPattern, &ffdImgFile);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
			CopyFile( szSRCFileLoc + ffdImgFile.cFileName, szProjEXELoc + ffdImgFile.cFileName, FALSE );
			SetFileAttributes(szProjEXELoc + ffdImgFile.cFileName, FILE_ATTRIBUTE_NORMAL ); 
		}
        while( FindNextFile(hFile, &ffdImgFile) );
        EXPECT( GetLastError() == ERROR_NO_MORE_FILES );
    }

	// Add Hello.cpp Hello1.cpp and Hello.rc files
	// can't do a multi add file on NT5. It's causing an Exception in the Dialog when Cafe
	// is hooked msdev as the debugger. This is a hack since this problem never exists
	// when doing manual a manaul test pass.
	if(GetSystem() & SYSTEM_NT_5)
	{
		if (!VERIFY_TEST_SUCCESS( prj.AddFiles( szEXECPPFile_1, PROJNAME )))
		{
			m_pLog->RecordFailure("Couldn't add CPP files to EXE project");
			return FALSE;
		}
		if (!VERIFY_TEST_SUCCESS( prj.AddFiles( szEXECPPFile_2, PROJNAME )))
		{
			m_pLog->RecordFailure("Couldn't add CPP files to EXE project");
			return FALSE;
		}
	}
	else   // do a multi add for all else
	{
		if (!VERIFY_TEST_SUCCESS( prj.AddFiles( szEXECPPFiles, PROJNAME )))
		{
			m_pLog->RecordFailure("Couldn't add CPP files to EXE project");
			return FALSE;
		}
	}
	
	if (!VERIFY_TEST_SUCCESS( prj.AddFiles( szEXERCFiles, PROJNAME ))) {
		m_pLog->RecordFailure("Couldn't add RC files to EXE project");
		return FALSE;
	}
	return TRUE;
}	

BOOL CPrjAppTest::SaveProject( void )
{
	if (!VERIFY_TEST_SUCCESS( prj.Save())) {
		m_pLog->RecordFailure("Save the project");
		return FALSE;
	}
	return TRUE;
}	

BOOL CPrjAppTest::CloseProject( void )
{
	BOOL bSaveProjBeforeClose = FALSE; 
	
	if (!VERIFY_TEST_SUCCESS( prj.Close(bSaveProjBeforeClose))) {
		m_pLog->RecordFailure("Close the project");
		return FALSE;
	}
	return TRUE;
}	


///////////////////////////////////////////////////////////
// CPrjUtil
///////////////////////////////////////////////////////////
void CPrjUtil::RemoveTestFiles( CString szProjLoc )
{
	KillAllFiles( szProjLoc );
}
	
BOOL CPrjUtil::TouchFile( CString szFileName )
{
	if (GetSystem() & SYSTEM_WIN)
		Sleep(3000);

	if ( _utime( szFileName, NULL ) == -1 ) {
		return FALSE;
	}
	// Because Win95 has some silly file commit thing that will cause this touch
	// not to commit before another process reads the file
	// we will let this process take a nap for a moment.
	if (GetSystem() & SYSTEM_WIN)
		Sleep(3000);

	return TRUE;
}

time_t CPrjUtil::GetFileTime( CString szFileName )
{
	struct _stat buf;
	int result = _stat( szFileName, &buf );
	return( buf.st_mtime );
}

BOOL CPrjUtil::SetToolsDir(CString szToolsLoc)
{
//	Set build tools directory to point to dummy build tools (which
//	must be provided by us)

	COEnvironment OptionsDirectories;
	
	if ( !VERIFY_TEST_SUCCESS( OptionsDirectories.PrependDirectory(szToolsLoc, DIR_TYPE_EXE, PLATFORM_WIN32_X86)) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CPrjUtil::RemoveToolsDir(CString szToolsLoc)
{
	COEnvironment OptionsDirectories;
	
	if ( !VERIFY_TEST_SUCCESS( OptionsDirectories.RemoveDirectory(szToolsLoc, DIR_TYPE_EXE, PLATFORM_WIN32_X86)) )
	{
		return FALSE;
	}

	return TRUE;
}

CString   CPrjUtil::GetBldLog() {


	CString BuildRecord;
	CString str;
	
	// Get build data
	UIWB.DoCommand(IDM_WINDOW_ERRORS,DC_ACCEL);  // goto output window
	MST.DoKeys("^{END}");			// goto end
	MST.DoKeys("+^{HOME}");			// select whole build record
	MST.DoKeys("^c");				// copy it
	GetClipText(BuildRecord);       // get text from clipboard
	MST.DoKeys("{ESC}");            // return focus from output window
	BuildRecord.MakeLower();
	return (BuildRecord);
	
};


