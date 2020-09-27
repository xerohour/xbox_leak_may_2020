//
// dbgset.cpp
//
//	Created by		Date
//		TomSe		9/2/94
//
//	Description
//		Definition of base class containing common data and methods for all
//		Debug Test Sets.


#include "stdafx.h"
#include "ecset.h"
#include "guitarg.h"

IMPLEMENT_DYNAMIC( CSubTestException, CException );

//IMPLEMENT_TEST(CECTestSet, CTest, "EC base class", -1, CIDESubSuite)
IMPLEMENT_DYNAMIC(CECTestSet, CTest)

CECTestSet::CECTestSet(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CTest(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}

BOOL CECTestSet::SetProject(LPCSTR projName, PROJECT_TYPE projType /* PROJECT_EXE */, PROJECT_FLAGS flags /* fDefault */, LPCSTR cfgName /*NULL*/)
{
	const char* const THIS_FUNCTION = "SetProject()";
	CString projPathAndName = projName;
	int iName = projPathAndName.ReverseFind('\\');
	if( iName < 0 )
	{
		m_strProjectDir = FullPath("src");
		m_strProjectName = projName;
	}
	else
	{
		m_strProjectDir = (CString)FullPath("src") + "\\" + projPathAndName.Left(iName);
		m_strProjectName = projPathAndName.Mid(iName + 1);
	}

	CIDESubSuite* pSubSuite=(CIDESubSuite*)GetSubSuite();
	ASSERT(pSubSuite->IsKindOf(RUNTIME_CLASS(CIDESubSuite))); //our tests are IDE tests only
	// get the toolset environment settings.
	CSettings *settingsToolsetEnv = pSubSuite->GetToolset()->GetEnvSettings();
	// build the path, lib, and include environment strings.

	CString cstrPathEnv = settingPathEnv + CString("=") + settingsToolsetEnv->GetTextValue(settingPathEnv);
	CString cstrLibEnv = settingLibEnv + CString("=") + settingsToolsetEnv->GetTextValue(settingLibEnv);
 	CString cstrIncludeEnv = settingIncludeEnv + CString("=") + settingsToolsetEnv->GetTextValue(settingIncludeEnv);

	// get windows and system dir and append to path environment string.
	CString cstrWindowsDir, cstrSystemDir;
	// each dir is prepended with a semi-colon as a separator. 
	char chDirBuf[MAX_PATH] = ";";
 	GetWindowsDirectory(&chDirBuf[1], MAX_PATH);	// &chDirBuf[0] = ';'
	cstrWindowsDir = chDirBuf;
	GetSystemDirectory(&chDirBuf[1], MAX_PATH);		// &chDirBuf[0] = ';'
	cstrSystemDir = chDirBuf;
	cstrPathEnv += cstrWindowsDir + cstrSystemDir;

	// nmake's environment block.
	char chNmakeEnv[1024];
	// pointer to navigate through block.
	char *pchNmakeEnv = chNmakeEnv;

	// put the path, lib, and include environment strings into the block.
	// each string is terminated by null.
	strcpy(pchNmakeEnv, cstrPathEnv);
	pchNmakeEnv += strlen(pchNmakeEnv) + 1;
	strcpy(pchNmakeEnv, cstrLibEnv);
	pchNmakeEnv += strlen(pchNmakeEnv) + 1;
	strcpy(pchNmakeEnv, cstrIncludeEnv);
	// block is terminated by additional null.
	pchNmakeEnv[strlen(pchNmakeEnv) + 1] = 0;
 
	// to find nmake.exe in the toolset path environment, 
	// we'll use cstrPathFrag to search	for and extract directories
	// from cstrPathEnv	(minus first 5 characters which are "PATH=").
	CString cstrPathFrag = cstrPathEnv.Mid(5);
	// stores dir extracted from cstrPathFrag.
	CString cstrDir;
	// stores index of next semi-colon (dir separator) in cstrPathEnv.
	int indexSemi;
	// handle to nmake.exe returned by FindFirstFile().
	HANDLE hNmake;
	// FindFirstFile param.
	WIN32_FIND_DATA fdFindData;

	while(1)
		
		{
		// find the next semi-colon which terminates the next dir to search.
		indexSemi = cstrPathFrag.Find(';');

		// if a semi-colon was found, extract the dir it terminates. otherwise
		// we are at the final dir in the path environment. 
		if(indexSemi != -1)
			cstrDir = cstrPathFrag.Left(indexSemi);
		else
			cstrDir = cstrPathFrag;
			
		// only want to search dirs at least 3 chars long. for example:
		// path  = C:\;
		// index = 0123
		if(cstrDir.GetLength() >= 3)
			hNmake = FindFirstFile(cstrDir + "\\nmake.exe", &fdFindData);
  		
		// stop searching if nmake was found in cstrPathFrag.
		if(hNmake != INVALID_HANDLE_VALUE)
			break;

		// if no semi-colon was found or it was the last char in the path, 
		// then we just searched the final dir in the environment string, 
		// but still haven't found nmake. 
	 	if((indexSemi == -1) || (cstrPathFrag.GetLength() == (indexSemi + 1)))
			{
			m_pLog->RecordInfo("ERROR in %s: could not find nmake.exe in path environment "
							   "specified by CAFE.", THIS_FUNCTION);
			return FALSE;
			}
		else
			// advance to the start of the next dir after the semi-colon.
			cstrPathFrag = cstrPathFrag.Mid(indexSemi + 1);
		}


	// determine the configuration switch that we need to pass to nmake.
	// also set platform member value for easy access by tests.
	if(cfgName==NULL)
	switch(GetUserTargetPlatforms())
		{
        case PLATFORM_WIN32_ALPHA:
            cfgName = "alpha";
            m_platform = PLATFORM_WIN32_ALPHA;
            break;
		case PLATFORM_WIN32_X86:
			cfgName = "x86";
			m_platform = PLATFORM_WIN32_X86;
			break;
		}

	// project name and configuration switch we will pass to nmake.
	CString cstrProjAndConfig = "\"" + m_strProjectName + ".mak\" CFG=" + cfgName;
	if( UIWB.GetPcodeSwitch() )
		cstrProjAndConfig += " PCODE=yes";
	// CreateProcess() params.
   	STARTUPINFO siStartInfo;
	PROCESS_INFORMATION piProcInfo;
	// this is the minium initialization of STARTUPINFO required for CreateProcess() to work.
	memset(&siStartInfo, 0, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.wShowWindow = SW_SHOWDEFAULT;
	// fully qualified nmake.exe cmd line (cstrDir is directory we found nmake in earlier).
	char chNmakeCmdLine[MAX_PATH];
	strcpy(chNmakeCmdLine, cstrDir + "\\nmake.exe /a /f " + cstrProjAndConfig);
	
	// before spawning nmake make sure Debugger is not running, so vc50.pdb is released
	if(dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();

	// spawn nmake in the project dir.
	BOOL bRet = CreateProcess(NULL, chNmakeCmdLine, NULL, NULL, TRUE, 0, chNmakeEnv,
							  m_strProjectDir, &siStartInfo, &piProcInfo);
	
	// verify that nmake was successfully spawned.						  
	if(!bRet)
		{
		m_pLog->RecordInfo("ERROR in %s: nmake.exe found but could not be spawned.", THIS_FUNCTION);
		return FALSE;
		}

	// nmake's exit code returned by GetExitCodeProcess().
	DWORD dwNmakeExitCode;

	// wait up to 60 minutes for nmake to terminate.
	for(int i = 0; i < 3600; i++)
		
		{
		Sleep(1000);
		// get nmake's exit code (if it has finished).
		GetExitCodeProcess(piProcInfo.hProcess, &dwNmakeExitCode);
		
		// keep sleeping if nmake is still running.
		if(dwNmakeExitCode != STILL_ACTIVE)
			break;
		}

	// verify that nmake finished within the allotted time.
	if(i == 3600)
		{
		m_pLog->RecordInfo("ERROR in %s: after 60 minutes nmake is still running.", THIS_FUNCTION);
		return FALSE;
		}

	// verify that nmake was successful in building the executable.
	if(dwNmakeExitCode != 0)
		{
		m_pLog->RecordInfo("ERROR in %s: nmake.exe returned error code %d.", THIS_FUNCTION, dwNmakeExitCode);
		return FALSE;
		}

	// wait until nmake's window has gone away.
	if(!MST.WFndWndWait("nmake.exe", FW_PART + FW_NOEXIST, 5))
		{
		m_pLog->RecordInfo("ERROR in %s: after 5 seconds nmake's window still exists.", THIS_FUNCTION);
		return FALSE;
		}

	// stop here if we only want to build.
	if(flags & fBuildOnly)
		return TRUE;

	// project exe has same base as makefile (i.e. xxx.mak/xxx.exe/xxx.dll)
	// we want to strip out any prepended path and just use the file name
	// because we use this name to set the remote path, and the host
	// path will usually not match the remote path.
	CString cstrProjectExe = m_strProjectName.Right(m_strProjectName.GetLength() -
							 m_strProjectName.ReverseFind('\\') - 1) + 
							 ((projType == PROJECT_EXE) ? ".exe" : ".dll");


	// we need to make sure the debugger isn't running on a previous project
	// or COWorkSpace::CloseAllWindows will fail.
	if(dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();

	// we need to close all windows before opening the exe or COProject::Open will fail.
 	COWorkSpace ws;
	ws.CloseAllWindows();

	// open the executable.	
	if(proj.Open(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR in %s: could not open \"%s\".", THIS_FUNCTION, 
							m_strProjectDir + "\\" + cstrProjectExe);
		return FALSE;
		}

	// allows us to access ide's connection settings via pconnec.
	COConnection *pconnec = pSubSuite->GetIDE()->GetConnectionInfo();
//	COConnection *pconnec = GetSubSuite()->GetIDE()->GetConnectionInfo();

	// set the connection as specified by the user.
	if(pconnec->SetAll() != ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR in %s: could not set connection settings.", THIS_FUNCTION);
		return FALSE;
		}
	
	// if indicated, set the remote path and copy the executable to the remote machine.
	if(pconnec->GetUserConnections() != CONNECTION_LOCAL)
		
		{
		if(proj.SetRemotePath(cstrProjectExe) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("ERROR in %s: could not set remote path using \"%s\".", 
							   THIS_FUNCTION, cstrProjectExe);
			return FALSE;
			}
		
		if(proj.UpdateRemoteFile(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("ERROR in %s: could not update remote file using \"%s\".", 
							   THIS_FUNCTION, m_strProjectDir + "\\" + cstrProjectExe);
			return FALSE;
			}
		}
	
	return TRUE;
}


BOOL CECTestSet::PrepareProject(LPCSTR projName)
{
	const char* const THIS_FUNCTION = "PrepareProject()";
	CString projPathAndName = projName;
	int iName = projPathAndName.ReverseFind('\\');
	if( iName < 0 )
	{
		m_strProjectDir = FullPath("apps");
		m_strProjectName = projName;
	}
	else
	{
		m_strProjectDir = (CString)FullPath("apps") + "\\" + projPathAndName.Left(iName);
		m_strProjectName = projPathAndName.Mid(iName + 1);
	}


	// project exe has same base as makefile (i.e. xxx.mak/xxx.exe/xxx.dll)
	// we want to strip out any prepended path and just use the file name
	// because we use this name to set the remote path, and the host
	// path will usually not match the remote path.
/*	CString cstrProjectExe = m_strProjectName.Right(m_strProjectName.GetLength() -
							 m_strProjectName.ReverseFind('\\') - 1) + 
							 ((projType == PROJECT_EXE) ? ".exe" : ".dll");
*/
	CString cstrProjectExe = m_strProjectName;

	// we need to make sure the debugger isn't running on a previous project
	// or COWorkSpace::CloseAllWindows will fail.
	if(dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();

	// we need to close all windows before opening the exe or COProject::Open will fail.
 	COWorkSpace ws;
	ws.CloseAllWindows();

	// open the executable.	
	if(proj.Open(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR in %s: could not open \"%s\".", THIS_FUNCTION, 
							m_strProjectDir + "\\" + cstrProjectExe);
		return FALSE;
		}

	CIDESubSuite* pSubSuite=(CIDESubSuite*)GetSubSuite();
	ASSERT(pSubSuite->IsKindOf(RUNTIME_CLASS(CIDESubSuite))); //our tests are IDE tests only

	// allows us to access ide's connection settings via pconnec.
	COConnection *pconnec = pSubSuite->GetIDE()->GetConnectionInfo();
//	COConnection *pconnec = GetSubSuite()->GetIDE()->GetConnectionInfo();

	// set the connection as specified by the user.
	if(pconnec->SetAll() != ERROR_SUCCESS)
		{
		m_pLog->RecordInfo("ERROR in %s: could not set connection settings.", THIS_FUNCTION);
		return FALSE;
		}
	
	// if indicated, set the remote path and copy the executable to the remote machine.
	if(pconnec->GetUserConnections() != CONNECTION_LOCAL)
		
		{
		if(proj.SetRemotePath(cstrProjectExe) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("ERROR in %s: could not set remote path using \"%s\".", 
							   THIS_FUNCTION, cstrProjectExe);
			return FALSE;
			}
		
		if(proj.UpdateRemoteFile(m_strProjectDir + "\\" + cstrProjectExe) != ERROR_SUCCESS)
			{
			m_pLog->RecordInfo("ERROR in %s: could not update remote file using \"%s\".", 
							   THIS_FUNCTION, m_strProjectDir + "\\" + cstrProjectExe);
			return FALSE;
			}
		}
	
	return TRUE;
}


/*
** start debugging and do some initialization.  
*/

BOOL CECTestSet::StartDebugging(void)
{
	BOOL bSuccess; 
	COSource src;	

	bps.ClearAllBreakpoints();		// Clear All Breakpoints Just in case.
	dbg.StepOver(1);				// Start debugging session with initial trace.
	src.AttachActiveEditor();		// WinslowF
	bSuccess = dbg.AtSymbol("WinMain");

	// attach to the editor window the debugger opened
	bSuccess &= src.AttachActiveEditor();

	UIWB.SetMemoryFormat(MEM_FORMAT_BYTE); 
	dbg.SetSteppingMode(SRC); 		
	return bSuccess;
}


BOOL CECTestSet::StopDbgCloseProject(void)
{
	if (dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();
	UIWB.CloseAllWindows();
	proj.Close();						

	return TRUE;
}


int ReturnCode =0;
bp  * ReturnBP =0;

void CECTestSet::LogResult(int line, int result,LPCSTR szoperation, int code /* 0 */, LPCSTR szComment /* "" */)

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


//
//	Returns the current directory of the first time this function was called.
//
LPCSTR CECTestSet::HomeDir()
{
	static CString szCurDir;

	if( szCurDir.IsEmpty() )
	{
		char* psz = szCurDir.GetBufferSetLength(_MAX_DIR);

		GetCurrentDirectory(_MAX_DIR - 1, psz);
		szCurDir.ReleaseBuffer(-1);
		if( szCurDir.Right(1) != '\\' )
			szCurDir += "\\";
	}
 	return szCurDir;
}

//
//	Returns the fully qualified path with szAddPath appended to the breadth dir.	
//
LPCSTR CECTestSet::FullPath( LPCSTR szAddPath )
{
	static CString szFilespec;

	szFilespec = HomeDir();
	szFilespec += szAddPath;

	return szFilespec;
}


void CECTestSet::FillExprInfo(EXPR_INFO &expr_info, 
								 int state, LPCSTR type, LPCSTR name, LPCSTR value)

	{
	expr_info.state = state;
	expr_info.type = type;
	expr_info.name = name;
	expr_info.value = value;
	}


BOOL CECTestSet::ExprInfoIs(EXPR_INFO * expr_info_actual, EXPR_INFO * expr_info_expected, 
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
						if(expr_info_actual[i].value.Mid(10) ==  (" " + expr_info_expected[i].value.Mid(8)))
							value_tests_passed=TRUE;	
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


BOOL CECTestSet::ValueIsAddress(CString str)
	{
	return (str.Mid(0, 2) == "0x") && 
		   (str.Mid(2).SpanIncluding("0123456789abcdef").GetLength() == 8);
	}


void CECTestSet::LogTestHeader(LPCSTR test_desc_str, TestType testtype /* TT_TEST */)
{
	if( testtype == TT_TEST )
		m_pLog->RecordInfo("");
	m_pLog->RecordInfo("");
	CString strTestType;
    switch (testtype)
    {
		case TT_TEST:
			strTestType = "TEST";
			break;

		case TT_SUBTEST:
			strTestType = "SubTest";
			break;

		case TT_TESTCASE:
			strTestType = "TestCase";
			break;
	}

	m_pLog->RecordInfo("%s:: %s", strTestType, test_desc_str);
}

void CECTestSet::LogTestStartEnd(LPCSTR test_desc_str, StartEndTest startend /* START_TEST */)
{
	CString strTest = "END";

	if( startend == START_TEST)
	{
		strTest = "START";
		m_pLog->RecordInfo("");
	}
 
	strTest += "***TEST";

	m_pLog->RecordInfo("%s: %s", strTest, test_desc_str);
}

void CECTestSet::LogTestBegin(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("BEGIN***TEST: %s", test_desc_str);
}

void CECTestSet::LogTestEnd(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("END***TEST: %s", test_desc_str);
}

void CECTestSet::LogTestEnd(LPCSTR test_desc_str, CTimeSpan elapsed_time, bool timing /* FALSE */)
{
//	GetLog()->RecordInfo("***TEST elapsed time %s", elapsed_time.Format("%H:%M:%S"));
	if( timing )
	{
		m_pLog->RecordInfo("END***TEST: %s  elapsed time %s", test_desc_str, elapsed_time.Format("%H:%M:%S"));
	}
	else
	{
		m_pLog->RecordInfo("END***TEST: %s", test_desc_str);
	}
}

void CECTestSet::LogSubTestHeader(LPCSTR test_desc_str)
{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("SubTest: %s", test_desc_str);
}

void CECTestSet::VerifyLocalsWndString(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber)
{
	uivar.Activate();
	EXPECT_TRUE(uivar.SetPane(PANE_LOCALS) == ERROR_SUCCESS);

	EXPR_INFO expr_info;
	uivar.GetAllFields(&expr_info, lineNumber, 1);
	CString csType = expr_info.type;
	CString csName = expr_info.name;
	CString csValue = expr_info.value;


	WriteLog( (csType == strType && csName == strName && 
		csValue.Find(strValue) >= 0) ? PASSED : FAILED,
		"Current Locals line: \"%s %s %s \" Expectation: \"%s %s %s \"", 
		(LPCSTR)csType, (LPCSTR)csName, (LPCSTR)csValue, 
		(LPCSTR)strType, (LPCSTR)strName, (LPCSTR)strValue );
}

BOOL CECTestSet::LocalsWndStringIs(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber)
{
	uivar.Activate();
	EXPECT_TRUE(uivar.SetPane(PANE_LOCALS) == ERROR_SUCCESS);
	BOOL ret = FALSE;

	EXPR_INFO expr_info;
	if(uivar.GetAllFields(&expr_info, lineNumber, 1)!=ERROR_SUCCESS)
		return FALSE;
	if(strcmp(strType, expr_info.type)==0 &&
	   strcmp(strName, expr_info.name)==0 &&
	   strstr(strValue,expr_info.value)!=NULL)
		ret = TRUE;

	return ret;
}

BOOL CECTestSet::CopyProjectSources( LPCSTR szSRCFileLoc, LPCSTR szProjLoc )
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

void CECTestSet::RemoveTestFiles( CString szProjLoc )
{
	KillAllFiles( szProjLoc, TRUE );
}
	
BOOL CECTestSet::ModifyCodeLine(LPCSTR szFind, LPCSTR szReplace)
{
	if( src.Find(szFind) )
	{
//		MST.DoKeys("{Delete}");
/* src.TypeTextAtCursor(szReplace, FALSE); doesn't work as well as InsertText():
 for some reason it cannot get the editor 
*/
		MST.DoKeys(szReplace, TRUE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CECTestSet::ModifyCode(LPCSTR szFind, LPCSTR szReplace, int nLines /* 1 */)
{
	CString cStr = szReplace;
	CString str1;

	if( src.Find(szFind) )
	{
		MST.DoKeys("{Home}");
		for( int i = 1; i < nLines; i++ )
		{	
			MST.DoKeys("+{down}");
		}
		MST.DoKeys("+{End}");
		int length = cStr.GetLength();
		if( length <= 0)
		{
			MST.DoKeys("{Delete}");
			return TRUE;
		}

		int right;
		while( length )
		{
			right = cStr.Find("{Enter}");
			if( right < 0)
			{
				MST.DoKeys(cStr, TRUE);
				return TRUE;
			}
			else 
			{
				str1 = cStr.Mid(0, right);
				MST.DoKeys(str1, TRUE);
				MST.DoKeys("{Enter}");
				if( length = (length - right - 7) )
				{
					cStr = cStr.Mid(right + 7, length);
				}
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CECTestSet::IsMsgBox(CString strMsg, CString strBtn)
{	
	if( WaitMsgBoxText( strMsg, 10 ) )
	{
		MST.WButtonClick( strBtn );
		return TRUE;
	}
	else
		return FALSE;
}

CString CECTestSet::GetBldLog()
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


BOOL CECTestSet::VerifyBuildOccured( BOOL iBuild )
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

	return TRUE;
}

int CECTestSet::VerifyBuildString( LPCSTR verifyString, BOOL fExist /* TRUE */ )
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

int CECTestSet::VerifyDebugOutputString( LPCSTR verifyString, BOOL fExist /* TRUE */ )
{
	int iFoundString;
	CString csLog;
	CString testString = verifyString;

	// Get the output window contents
	// TODO(michma): mismatch with CODebug here. need to merge?
	dbg.SetDebugOutputPane(IDSS_PANE_DEBUG_OUTPUT);
	MST.DoKeys("^{END}");			// goto end
	MST.DoKeys("+^{HOME}");			// select whole build record
	MST.DoKeys("^c");				// copy it
	GetClipText(csLog);       // get text from clipboard
	MST.DoKeys("{ESC}");            // return focus from output window
	csLog.MakeLower();

	// For some reason the output window text is always lower case when extracted.
	testString.MakeLower();
	iFoundString = csLog.Find(testString);

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

void CECTestSet::VerifyECSuccess()
{
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
//		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
}

CStartEndTest::CStartEndTest(LPCSTR testName, CECTestSet *test, bool timing /*=FALSE*/)
{
	m_testName = testName;
	m_Test = test;
	if( timing )
	{
		m_startTime = CTime::GetCurrentTime();
		m_timing = TRUE;
		m_Test->LogTestBegin( m_testName );
	}
}

CStartEndTest::~CStartEndTest()
{
	if( m_timing )
	{
		m_endTime = CTime::GetCurrentTime();
		CTimeSpan elapsed_time(m_endTime - m_startTime);
		m_Test->LogTestEnd( m_testName, elapsed_time, m_timing );
	}
	else
		m_Test->LogTestEnd( m_testName );
}


char szecEditTypes[6][256] =
{
	"EC_NO_STACK_FRAME",
	"EC_CURRENT_FRAME_AFTER_IP",
	"EC_CURRENT_FRAME_BEFORE_IP",
	"EC_CURRENT_FRAME_AT_IP", 
	"EC_PARENT_FRAME_AFTER_IP", 
	"EC_PARENT_FRAME_BEFORE_IP"
};

BOOL CECTestSet::IsProjectReady(ProjType projType)
{
	switch (projType)
	{
	case PT_EXE:
		m_strProjectName = "ecconsol";
		break;
	case PT_EXT:
		m_strProjectName = "ecwinapp";
		break;
	case PT_INT:
		m_strProjectName = "ecmfcapp";
		break;
	}
	
	m_strSrcDir = "ecapp";	//ToDo: We should get the name from file with all languages strings
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strSrcDir + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;

	if (projType == PT_INT)	  // MFC app
		RemoveTestFiles( m_strProjectLoc + "res\\");
	RemoveTestFiles( m_strProjectLoc );
	CreateDirectory(m_strProjectLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );
	if (projType == PT_INT)	   // MFC app
	{
		CreateDirectory(m_strProjectLoc + "res\\", 0);          
		EXPECT_TRUE( CopyProjectSources( szSRCFileLoc + "res\\", m_strProjectLoc + "res\\" ) );
	}
	if (projType == PT_EXE)	   // EXE consol app
	{
		if( SetProject(szProjPathName) )
			return TRUE;
	}
	else
	{
		int iWaitForBuild = 3;	// Minutes to wait for the build to complete
		if( PrepareProject(szProjPathName) && VERIFY_TEST_SUCCESS( proj.Build(iWaitForBuild)))
		{
			proj.SetExeForDebugSession(m_strProjectName + ".exe");
			return TRUE;
		}
	}
	return FALSE;
}
