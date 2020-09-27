///////////////////////////////////////////////////////////////////////////////
//	PRJSET.CPP
//
//	Created by :			Date :
//		YefimS					4/28/97
//
//	Description :
//		Definition of base class containing common data and methods for all
//		Project Test Sets.

#include "stdafx.h"
#include "prjset.h"
#include "guitarg.h"
//#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

#define WAIT_FOR_BUILD 8
// I added this because it seems to be missing from this module (t-seanc)
int ReturnCode = 0;

IMPLEMENT_DYNAMIC(CProjectTestSet, CTest)

CProjectTestSet::CProjectTestSet(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CTest(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}

void CProjectTestSet::RemoveTestFiles( CString szProjLoc )
{
	KillAllFiles( szProjLoc );
}
	
BOOL CProjectTestSet::TouchFile( CString szFileName )
{
	m_pLog->RecordInfo("Touching time stamp on %s", szFileName);
	// A FMN to give the OS a chance to commit the File Change
	// It's a Win98 and Win95 feature
	Sleep(3000);
	// make the change
	if ( _utime( szFileName, NULL ) == -1 ) {
		return FALSE;
	}
	Sleep(3000);

	return TRUE;
}

time_t CProjectTestSet::GetFileTime( CString szFileName )
{
	struct _stat buf;
	int result = _stat( szFileName, &buf );
	return( buf.st_mtime );
}

BOOL CProjectTestSet::SetToolsDir(CString szToolsLoc)
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

BOOL CProjectTestSet::RemoveToolsDir(CString szToolsLoc)
{
	COEnvironment OptionsDirectories;
	
	if ( !VERIFY_TEST_SUCCESS( OptionsDirectories.RemoveDirectory(szToolsLoc, DIR_TYPE_EXE, PLATFORM_WIN32_X86)) )
	{
		return FALSE;
	}

	return TRUE;
}

CString CProjectTestSet::GetBldLog()
{
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

void CProjectTestSet::LogTestHeader(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST: %s", test_desc_str);
}

void CProjectTestSet::LogSubTestHeader(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("SubTest: %s", test_desc_str);
}


void CProjectTestSet::LogResult(int line, int result,LPCSTR szoperation, int code /* 0 */, LPCSTR szComment /* "" */)
{
	CString szOpCom = (CString)szoperation + "  " + szComment;
	if(result == PASSED)
		m_pLog->RecordSuccess("%s", szOpCom);
	else
	{
		CString szextra;
		char chbuf[24];
		szextra = szextra + "Error Code = " + itoa(code, chbuf, 10);
		m_pLog->RecordFailure("LINE %d %s", line, szextra + szOpCom);
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Function: TouchAndVerifyRebuild

	This function sets the last modified time stamp on the file specifed
	to the current time and sends a Build command to DevStudio.  It then
	checks to make sure that the build actually occured.
  
	Parameter: CString szFileToTouch
	
	This should be the path that specifies the file to be touched before
	the rebuild occurs.

	Parameter: BOOL bShouldRebuild

	This parameter specifies whether or not a rebuild is expected to happen.
	If it is TRUE and the build does not happen, that will generate an error.
	If it is FALSE, and the build happens, that will generate an error.  The
	default value for this parameter is TRUE.
	  
	Return Value: BOOL

	This function returns TRUE if the build occurs and FALSE if the build
	does not occur.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
BOOL CProjectTestSet::TouchAndVerifyRebuild
	( 
	CString szFileToTouch,
	BOOL	bShouldRebuild  /* = TRUE */
	)

	{
	// Make sure the string is non-null
//	_ASSERTE(szFileToTouch != NULL);	// bogus assert

	// Make sure the file exists
	OFSTRUCT ofs;
	ofs.cBytes = sizeof(OFSTRUCT);
	if(OpenFile(szFileToTouch, &ofs, OF_EXIST) == HFILE_ERROR)
		{
		m_pLog->RecordFailure(
			_T("%s"), 
			szFileToTouch + _T(": file does not exist.")
			);
		}
	
	// Change the last modified time to the current time
	TouchFile(szFileToTouch);
	
	// Rebuild the project and return whether or not the build happens
	prj.Build(WAIT_FOR_BUILD);
	return VerifyBuildOccured(bShouldRebuild);
	}

BOOL CProjectTestSet::VerifyBuildOccured( BOOL iBuild )
{
	// iBuild == TRUE  -  project should be rebuilt
	// iBuild == FALSE  -  project should not be rebuilt

	// Get the output window contents
	CString szBldLog = GetBldLog();
	if (szBldLog == "")
	{
		m_pLog->RecordFailure("Incorrect build: Output window is empty");
		return FALSE;
	}
	
	int iFoundCompiling = szBldLog.Find (GetLocString(IDSS_OUTPUTWIN_COMPILING));
	int iFoundLinking = szBldLog.Find (GetLocString(IDSS_OUTPUTWIN_LINKING));

	if( iBuild )
	{
		if( (iFoundCompiling < 0) && (iFoundLinking < 0) )
		{
			m_pLog->RecordFailure("Incorrect build: can't find %s or %s",GetLocString(IDSS_OUTPUTWIN_COMPILING), GetLocString(IDSS_OUTPUTWIN_LINKING) );
			m_pLog->RecordInfo(szBldLog);
			return FALSE;
		}
	}
	else
	{
		if ( (iFoundCompiling > 0) || (iFoundLinking > 0) )
		{
			m_pLog->RecordFailure("Nothing changed, the project shouldn't be rebuilt.");
			m_pLog->RecordInfo(szBldLog);
			return FALSE;
		}
	}

	return TRUE;
}

int CProjectTestSet::VerifyBuildString( LPCSTR verifyString, BOOL fExist /* TRUE */ )
{
	int iFoundString;

	// Get the output window contents
	CString szBldLog = GetBldLog();
	CString testString = verifyString;

	// For some reason the output window text is always lower case when extracted.

	testString.MakeLower();
	iFoundString = szBldLog.Find(testString);

	if ( (iFoundString < 0) && fExist )
	{
		m_pLog->RecordFailure("Can't find %s", verifyString);
		m_pLog->RecordInfo(szBldLog);
	}
	
	if ( (iFoundString >= 0) && !fExist )
	{
		m_pLog->RecordFailure("Found %s : it should not be there", verifyString);
		m_pLog->RecordInfo(szBldLog);
	}

	return iFoundString;
}

BOOL CProjectTestSet::CopyProjectSources( LPCSTR szSRCFileLoc, LPCSTR szProjLoc )
{
	CString szPattern		= (CString)szSRCFileLoc + "*.*";

	// Copy sources to the project directory
	WIN32_FIND_DATA ffdImgFile;
    HANDLE hFile = FindFirstFile(szPattern, &ffdImgFile);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
			CopyFile( (CString)szSRCFileLoc + ffdImgFile.cFileName, (CString)szProjLoc + ffdImgFile.cFileName, FALSE );
			SetFileAttributes((CString)szProjLoc + ffdImgFile.cFileName, FILE_ATTRIBUTE_NORMAL );
		}
        while( FindNextFile(hFile, &ffdImgFile) );
        EXPECT( GetLastError() == ERROR_NO_MORE_FILES );
		return TRUE;
    }
	else
	{
		m_pLog->RecordFailure("Could not copy sources from %s to %s", szSRCFileLoc, szProjLoc);
		return FALSE;
	}
}

BOOL CProjectTestSet::UpdateBuildVerify( LPCSTR szCurrentFile, LPCSTR szNewFile, LPCSTR szFileString )
{

	if(!CopyFile(szNewFile, szCurrentFile, FALSE))
	{
		_TCHAR szMsg[1024];
		wsprintf(szMsg, "Could not copy \"%s\" to \"%s\".", szNewFile, szCurrentFile);
		m_pLog->RecordFailure(szMsg);
		return FALSE;
	}
	if(!SetFileAttributes(szCurrentFile, FILE_ATTRIBUTE_NORMAL ))
	{
		_TCHAR szMsg[1024];
		wsprintf(szMsg, "Could not set attributes on file \"%s\".", szCurrentFile);
		m_pLog->RecordFailure(szMsg);
		return FALSE;
	}
	TouchFile( szCurrentFile );

	Sleep(500);
	MST.DoKeys("{ENTER}");        // "rc was modified outside of IDE. reload? 

	if (!VERIFY_TEST_SUCCESS( prj.Build(WAIT_FOR_BUILD)))
	{
		m_pLog->RecordFailure("Could not build the Project");
		return FALSE;
	}

	VerifyBuildOccured( TRUE );
	if( VerifyBuildString( szFileString ) <= 0 )
	{
		m_pLog->RecordFailure("Could not find string in the build output");
		return FALSE;
	}

	return TRUE;
}

