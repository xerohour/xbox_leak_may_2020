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
#include "dbgset.h"
#include "guitarg.h"

IMPLEMENT_DYNAMIC( CSubTestException, CException );

IMPLEMENT_DYNAMIC(CDebugTestSet, CTest)

CDebugTestSet::CDebugTestSet(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CTest(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}

BOOL CDebugTestSet::SetProject(LPCSTR projName, PROJECT_TYPE projType /* PROJECT_EXE */, PROJECT_FLAGS flags /* fDefault */, LPCSTR cfgName /*NULL*/)

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
	// we enclose the project name in quotes to support names with spaces.
	CString cstrProjAndConfig = (CString)"\"" + m_strProjectName + ".mak\" CFG=" + cfgName;
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


	// Allocate console to be able to see how the project is built.  If we don't allocate it the window close
	// immediately after build process has finished. *dklem
	AllocConsole();


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

	// Is 5 seconds seconds enough to inspect the output ? *dklem
	Sleep(5000);
	// Say good bye to the console
	FreeConsole();

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

BOOL CDebugTestSet::StartDebugging(void)
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


BOOL CDebugTestSet::StopDbgCloseProject(void)
{
	if (dbg.GetDebugState() != NotDebugging)
		dbg.StopDebugging();
	UIWB.CloseAllWindows();
	proj.Close();						

	return TRUE;
}


BOOL CDebugTestSet::VerifyProjectRan(int exitcode /* = 0 */)
{

	// REVIEW: use CODebug::VerifyTermination when it is available.
	// OpenOutputWindow
	UIOutput uow = UIDebug::ShowDockWindow(IDW_OUTPUT_WIN, TRUE);
//	EXPECT( uow.IsActive() );
	// Select the last line, write it to clipboard and look for "exit code 0"
	MST.DoKeys( "^{end}" );
	MST.DoKeys( "{up}" );

	char buffer[10];
	CString	szexcode = "exited with code ";			
	szexcode += itoa(exitcode, buffer, 10 ); // WinslowF - for OEM compatibility now we can specify the exit code to be verified

	if ( UIWB.VerifySubstringAtLine( szexcode ))
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}		
	
	return TRUE;
}


BOOL CDebugTestSet::StepInUntilFrameAdded( CString addframe )
{
	BOOL bResult = TRUE;
	COStack stk;
	CODebug dbg;
	FrameList framelist0;

	// Establish base line
	framelist0.CreateFromText( stk.GetAllFunctions() );
	Frame topframe( framelist0[0] );

	while( bResult ) {
		dbg.StepInto();
		FrameList current;
		current.CreateFromText( stk.GetAllFunctions() );

		if( current.GetSize() == framelist0.GetSize() ) {
			// Only check function name or image name for top frame.
			CString str0;
			if( topframe.GetFunctionName( str0 ) ) {
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else if( topframe.GetImageName( str0 ) ){
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else {
				// Bad format
				m_pLog->RecordInfo( "StepInUntilFrameAdded - Bad format for frame '%s'",
					(LPCTSTR)topframe );
				bResult = FALSE;
			}

			// The remaining frames should be identical
			for( int iFrame = 1; bResult && iFrame < framelist0.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame] == current[iFrame];
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch in frame %d, '%s' != '%s'",
						(LPCTSTR)current[0], (LPCTSTR)framelist0[0] );
				}
			}
		}
		else if( current.GetSize() == framelist0.GetSize() + 1 ) {
			// Test new top frame
			bResult = Frame( current[0] ).Test( addframe );

			// Only check function name or image name for 2nd frame since the line number
			// may change.
			CString str0;
			if( topframe.GetFunctionName( str0 ) ) {
				bResult = Frame( current[1] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch 2nd frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[1] );
				}
			}
			else if( topframe.GetImageName( str0 ) ) {
				bResult = Frame( current[1] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch 2nd frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[1] );
				}
			}
			else {
				// Bad format
				m_pLog->RecordInfo( "StepInUntilFrameAdded - Bad format for frame '%s'",
					(LPCTSTR)topframe );
				bResult = FALSE;
			}

			// The remaining frames should be identical
			for( int iFrame = 1; bResult && iFrame < framelist0.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame] == current[iFrame+1];
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameAdded - Mismatch in frame %d, '%s' != '%s'",
						(LPCTSTR)current[iFrame+1], (LPCTSTR)framelist0[iFrame] );
				}
			}
			break;	// Get out of loop

		}
		else {
			// Unexpected number of frames
			m_pLog->RecordInfo( "StepInUntilFrameAdded - Expected %d frames, got %d frames",
				framelist0.GetSize() + 1, current.GetSize() );
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL CDebugTestSet::StepInUntilFrameRemoved() {
	BOOL bResult = TRUE;
	COStack stk;
	CODebug dbg;
	FrameList framelist0;

	// Establish base line

	framelist0.CreateFromText( stk.GetAllFunctions() );
	Frame topframe( framelist0[0] );

	while( bResult ) {
		dbg.StepInto();
		FrameList current;
		current.CreateFromText( stk.GetAllFunctions() );

		//
		// REVIEW: This can be shared with StepInUntilAdded
		//
		if( current.GetSize() == framelist0.GetSize() ) {
			// Only check function name or image name for top frame.
			CString str0;
			if( topframe.GetFunctionName( str0 ) ) {
				CString str1;
				if( topframe.GetParameters( str1 ) ) {
					str0 += "(" + str1 + ")";
				}
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameRemoved - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else if( topframe.GetImageName( str0 ) ) {
				bResult = Frame( current[0] ).Test( str0 );
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameRemoved - Mismatch top frame '%s' not in '%s'",
						(LPCTSTR)str0, (LPCTSTR)current[0] );
				}
			}
			else {
				// Bad format
				m_pLog->RecordInfo( "StepInUntilFrameRemoved - Bad format for frame '%s'",
					(LPCTSTR)topframe );
				bResult = FALSE;
			}

			// The remaining frames should be identical
			for( int iFrame = 1; bResult && iFrame < framelist0.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame] == current[iFrame];
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameRemoved - Mismatch in frame %d, '%s' != '%s'",
						iFrame, (LPCTSTR)current[0], (LPCTSTR)framelist0[0] );
				}
			}
		}
		else if( current.GetSize() == framelist0.GetSize() - 1 ) {
			// The remaining frames should be identical
			for( int iFrame = 0; bResult && iFrame < current.GetSize(); iFrame++ ) {
				bResult = framelist0[iFrame+1] == current[iFrame];
				if( !bResult ) {
					m_pLog->RecordInfo( "StepInUntilFrameRemoved - Mismatch in frame %d, '%s' != '%s'",
						iFrame, (LPCTSTR)current[iFrame], (LPCTSTR)framelist0[iFrame+1] );
				}
			}
			break;	// Get out of loop
		}
		else {
			// Unexpected number of frames
			m_pLog->RecordInfo( "StepInUntilFrameRemoved - Expected %d frames, got %d frames",
				framelist0.GetSize() + 1, current.GetSize() );
			bResult = FALSE;
		}
	}

	return bResult;
}


int ReturnCode =0;
bp  * ReturnBP =0;

void CDebugTestSet::LogResult(int line, int result,LPCSTR szoperation, int code /* 0 */, LPCSTR szComment /* "" */)

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
LPCSTR CDebugTestSet::HomeDir()
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
LPCSTR CDebugTestSet::FullPath( LPCSTR szAddPath )
{
	static CString szFilespec;

	szFilespec = HomeDir();
	szFilespec += szAddPath;

	return szFilespec;
}


void CDebugTestSet::FillExprInfo(EXPR_INFO &expr_info, 
								 int state, LPCSTR type, LPCSTR name, LPCSTR value)

	{
	expr_info.state = state;
	expr_info.type = type;
	expr_info.name = name;
	expr_info.value = value;
	}


BOOL CDebugTestSet::ExprInfoIs(EXPR_INFO * expr_info_actual, EXPR_INFO * expr_info_expected, 
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


BOOL CDebugTestSet::ValueIsAddress(CString str)
	{
	return (str.Mid(0, 2) == "0x") && 
		   (str.Mid(2).SpanIncluding("0123456789abcdef").GetLength() == 8);
	}


void CDebugTestSet::LogTestHeader(LPCSTR test_desc_str)
	{
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST: %s", test_desc_str);
	}

void CDebugTestSet::VerifyLocalsWndString(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber)
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

	// return focus from locals window.
	MST.DoKeys("{ESC}");
}

BOOL CDebugTestSet::LocalsWndStringIs(LPCSTR strType, LPCSTR strName,  LPCSTR strValue, int lineNumber)
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