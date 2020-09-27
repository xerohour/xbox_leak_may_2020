///////////////////////////////////////////////////////////////////////////////
//	SYSSET.CPP
//
//	Created by :			Date :
//		YefimS					5/19/97
//
//	Description :
//		Definition of base class containing common data and methods for all
//		System Test Sets.

#include "stdafx.h"
#include "SYSSET.h"
#include "guitarg.h"
//#include <io.h>
#include <sys/utime.h>
#include <sys/stat.h> 

IMPLEMENT_DYNAMIC(CSystemTestSet, CTest)

int ReturnCode =0;

CSystemTestSet::CSystemTestSet(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CTest(pSubSuite, szName, nExpectedCompares, szListFilename)
{
	if(GetSystem() & SYSTEM_DBCS)
	{//REVIEW(chriskoz): note doubled character '\' - compiler treats it as ESC sequence
		// TODO(michma - 12/9/97): all disabled mbcs names are due to bug 2116.
		m_strWorkspaceName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü AppWiz App";
		m_strAppProjName = m_strWorkspaceName;
		m_strAppClassName = "CMyAppWizAppApp";	 // not sure it is correct
		m_strMenuName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü Test";
		m_strMenuItemName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü TestDialog";
		m_strAppResourceID = "IDR_APPWIZTYPE";
		m_strHTMLPageName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü HTML Page.htm";
		// TODO(michma): what is this for japanese?
		//m_strActiveXControlProjName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü ActiveX Control";
		m_strActiveXControlProjName = "ActiveX Control";
		m_strActiveXControlName = "ActiveXControl";
		// TODO(michma): what is this for japanese?
		m_strActiveXControlViewClass = "";
		//m_strATLCOMServerProjName = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü ATL COM Server";
		m_strATLCOMServerProjName = "ATL COM Server";
		// are any of these allowed to contain mbcs characters?
		m_strATLCOMServerInterface = "ATLCOMServerInterface";
		m_strATLCOMServerClsID = "CLSID_ATLCOMServerInterface";
		m_strATLCOMServerIID = "IID_IATLCOMServerInterface";
	}
	else
	{ //REVIEW(chriskoz) do we want to have some upper ASCII on german system
		m_strWorkspaceName = "AppWiz App";
		m_strAppProjName = m_strWorkspaceName;
		m_strAppClassName = "CAppWizAppApp";
		m_strMenuName = "Test";
		m_strMenuItemName = "TestDialog";
		m_strAppResourceID = "IDR_APPWIZTYPE";
		m_strHTMLPageName = "HTML Page.htm";
		m_strActiveXControlProjName = "ActiveX Control";
		m_strActiveXControlName = "ActiveXControl";
		m_strActiveXControlViewClass = "CActiveXControlCtrl";
		m_strATLCOMServerProjName = "ATL COM Server";
		m_strATLCOMServerInterface = "ATLCOMServerInterface";
		m_strATLCOMServerClsID = "CLSID_ATLCOMServerInterface";
		m_strATLCOMServerIID = "IID_IATLCOMServerInterface";
	}

	// REIVEW(michma): do we want a localized name for "apps" too?
	m_strAppsRoot = "apps";
	m_strWorkspaceLoc = m_strAppsRoot + "\\" + m_strWorkspaceName + "\\";
	m_strHTMLPageRelPath = m_strWorkspaceLoc + m_strActiveXControlProjName + "\\" + m_strHTMLPageName;
}

void CSystemTestSet::RemoveTestFiles( CString szProjLoc )
{
	KillAllFiles( szProjLoc );
}
	
BOOL CSystemTestSet::TouchFile( CString szFileName )
{
	if ( _utime( szFileName, NULL ) == -1 ) {
		return FALSE;
	}
	return TRUE;
}

time_t CSystemTestSet::GetFileTime( CString szFileName )
{
	struct _stat buf;
	int result = _stat( szFileName, &buf );
	return( buf.st_mtime );
}

BOOL CSystemTestSet::SetToolsDir(CString szToolsLoc)
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

BOOL CSystemTestSet::RemoveToolsDir(CString szToolsLoc)
{
	COEnvironment OptionsDirectories;
	
	if ( !VERIFY_TEST_SUCCESS( OptionsDirectories.RemoveDirectory(szToolsLoc, DIR_TYPE_EXE, PLATFORM_WIN32_X86)) )
	{
		return FALSE;
	}

	return TRUE;
}

CString CSystemTestSet::GetBldLog()
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

void CSystemTestSet::LogTestHeader(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST: %s", test_desc_str);
}

void CSystemTestSet::LogSubTestHeader(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("SubTest: %s", test_desc_str);
}


void CSystemTestSet::LogResult(int line, int result,LPCSTR szoperation, int code /* 0 */, LPCSTR szComment /* "" */)
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

BOOL CSystemTestSet::VerifyBuildOccured( BOOL iBuild )
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

	// TODO(michma - 12/8/97): the "compiling" and "linking" need to be localized for this 
	// to pass on japanese and german.
/*
	int iFoundCompiling = szBldLog.Find ("compiling");	// International ?
	int iFoundLinking = szBldLog.Find ("linking");		// International ?

	if( iBuild )
	{
		if( (iFoundCompiling < 0) && (iFoundLinking < 0) )
		{
			m_pLog->RecordFailure("Incorrect build: can't find 'Compiling' or 'Linking");
			return FALSE;
		}
	}
	else
	{
		if ( (iFoundCompiling > 0) || (iFoundLinking > 0) )
		{
			m_pLog->RecordFailure("Nothing change, the project shouldn't be rebuilt.");
			return FALSE;
		}
	}
*/
	return TRUE;
}

int CSystemTestSet::VerifyBuildString( LPCSTR verifyString, BOOL fExist /* TRUE */ )
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
	}
	
	if ( (iFoundString >= 0) && !fExist )
	{
		m_pLog->RecordFailure("Found %s : it should not be there", verifyString);
	}

	return iFoundString;
}

BOOL CSystemTestSet::CopyProjectSources( LPCSTR szSRCFileLoc, LPCSTR szProjLoc )
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

BOOL CSystemTestSet::UpdateBuildVerify( LPCSTR szCurrentFile, LPCSTR szNewFile, LPCSTR szFileString )
{
	int iWaitForBuild = 3;	// Minutes to wait for the build to complete

	CopyFile(szNewFile, szCurrentFile, FALSE);
	SetFileAttributes(szCurrentFile, FILE_ATTRIBUTE_NORMAL ); 
	TouchFile( szCurrentFile );
	if (!VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)))
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

void CSystemTestSet::FillExprInfo(EXPR_INFO &expr_info, 
								 int state, LPCSTR type, LPCSTR name, LPCSTR value)

	{
	expr_info.state = state;
	expr_info.type = type;
	expr_info.name = name;
	expr_info.value = value;
	}


BOOL CSystemTestSet::ExprInfoIs(EXPR_INFO * expr_info_actual, EXPR_INFO * expr_info_expected, 
							   int total_rows /* 1 */)
	
	{
	for(int i = 0; i < total_rows; i++)

		{
		if(expr_info_actual[i].state != expr_info_expected[i].state)
			
			{
			m_pLog->RecordFailure("ERROR in ExprInfoIs(): state is %d instead of %d", 
								  expr_info_actual[i].state, expr_info_expected[i].state);

			return FALSE;
			}

		if(expr_info_actual[i].type != expr_info_expected[i].type)
			
			{
			m_pLog->RecordFailure("ERROR in ExprInfoIs(): type is %s instead of %s", 
								  expr_info_actual[i].type, expr_info_expected[i].type);

			return FALSE;
			}

		if(expr_info_actual[i].name != expr_info_expected[i].name)
			
			{
			m_pLog->RecordFailure("ERROR in ExprInfoIs(): name is %s instead of %s", 
								  expr_info_actual[i].name, expr_info_expected[i].name);

			return FALSE;
			}

		if(expr_info_expected[i].value.Mid(0, 7) != "UNKNOWN") 

			{
			BOOL value_tests_passed;

			if(expr_info_expected[i].value.Mid(0, 7) == "ADDRESS")
				
				{
				value_tests_passed = ValueIsAddress(expr_info_actual[i].value);

				// is extra data besides address expected? (ex. peek at string)
				if(expr_info_expected[i].value != "ADDRESS")
				
					{
					// user must supply extra data to avoid CString assert.
					EXPECT(expr_info_expected[i].value.GetLength() > 8);

					// make sure actual string is long enough so we avoid CString assert.
					if(expr_info_actual[i].value.GetLength() > 10)
					{
						// verify extra data is correct.
						if(expr_info_actual[i].value.Mid(10) == (" " + expr_info_expected[i].value.Mid(8)))
							value_tests_passed = TRUE;
					}
					else
						// actual string wasn't long enough to contain extra data.
						value_tests_passed = FALSE;
					}
				}	 
	
			else	
				value_tests_passed = expr_info_actual[i].value == expr_info_expected[i].value;

			if(!value_tests_passed)
				
				{
				m_pLog->RecordFailure("ERROR in ExprInfoIs(): value is %s instead of %s", 
									  expr_info_actual[i].value, expr_info_expected[i].value);

				return FALSE;
				}
			}
		}

	return TRUE;
	}


BOOL CSystemTestSet::ValueIsAddress(CString str)
	{
	return (str.Mid(0, 2) == "0x") && 
		   (str.Mid(2).SpanIncluding("0123456789abcdef").GetLength() == 8);
	}

