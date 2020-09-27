///////////////////////////////////////////////////////////////////////////////
//	FILEcase.CPP
//											 
//	Created by :			
//		WinslowF		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "FILEcase.h"	
#include <string.h>

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 

/*
	Source files in szSrcDir
*/
static CString szSrcMainFile	= "mainfile";
static CString szSrc2ndFile		= "file2";
static CString szTestMainFile	= "testfile";
static CString szTest2ndFile	= "xx";
static CString szTestDir		= "test";
static CString szOpenFile		= "Names.TXT";
static CString szErrFile		= "FilErr.Log";
static CString szRootDir		= "testroot";


/*
	Source directory
*/
static CString szSrcDir		= "src";

static CString szBackslash = "\\";

IMPLEMENT_TEST(CFILENAMEIDETest, CTest, "DBG filename test", -1, CFILENAMESubSuite)

void CFILENAMEIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CFILENAMEIDETest::CopyFile(CString szFrom, CString szTo)
{
	CString	szDestination = m_szTestDir+szBackslash+szTo;

	SetFileAttributes(szDestination,FILE_ATTRIBUTE_NORMAL);				// Unset READONLY
	::CopyFile( m_szSourceDir+szBackslash+szFrom, szDestination, FALSE );
	SetFileAttributes(szDestination,FILE_ATTRIBUTE_NORMAL);				// Unset READONLY
}

void CFILENAMEIDETest::Run(void)
{
	XSAFETY;

	UIDebug dbgUI;
	CStringList szFileList;
	POSITION pos;

	/*
		Copy project from src to test directory.
	*/
	if( VERIFY_TEST_SUCCESS( CreateFileNameList(&szFileList) ) )
	{
		m_szErrorText = "Can't create filename list";
		m_pLog->Comment( m_szErrorText );
	}
	

	for( pos = szFileList.GetHeadPosition(); pos != NULL; )
	{
		szTest2ndFile = szFileList.GetNext( pos );
	
		Sleep(10);
		SetUp();
		
		m_szErrorText = "<<======== Filename test for directory " + m_szTestDir + " and filename " + szTest2ndFile + "========>>";
		m_pLog->Comment( m_szErrorText );

		m_szErrorText = "->>CreateBuild project";
		m_pLog->Comment( m_szErrorText );
		EXPECT( CreateBuildProject() );
		m_szErrorText = "->>Set break points";
		m_pLog->Comment( m_szErrorText );
		EXPECT( SetBreakpoints() );
		EXPECT( ReadDBCSource() );
		EXPECT(dbgUI.ShowDockWindow(IDW_OUTPUT_WIN, TRUE));
		m_szErrorText = "->>Show registers";
		m_pLog->Comment( m_szErrorText );
		EXPECT(dbgUI.ShowDockWindow(IDW_CPU_WIN, TRUE));
		m_szErrorText = "->>Show callstack";
		m_pLog->Comment( m_szErrorText );
		EXPECT(dbgUI.ShowDockWindow(IDW_CALLS_WIN, TRUE));
		EXPECT(dbg.SetSteppingMode(SRC));	
		EXPECT( ReadDBCDisasm() );
		m_szErrorText = "->>QuickWatch test";
		m_pLog->Comment( m_szErrorText );
		EXPECT( QuickWatchTest() );
		m_szErrorText = "->>Memory test";
		m_pLog->Comment( m_szErrorText );
		EXPECT( MemoryTest() );
		m_szErrorText = "->>End test";  
		m_pLog->Comment( m_szErrorText );
		EXPECT( ENDThisTest() );
	}

	XSAFETY;
}


BOOL CFILENAMEIDETest::CreateFileNameList(CStringList *szFileList)
{
	char* psz = m_strCurDir.GetBufferSetLength(_MAX_DIR);
	CStdioFile f1;


	GetCurrentDirectory(_MAX_DIR - 1, psz);
	m_strCurDir.ReleaseBuffer(-1);
	if( m_strCurDir.Right(1) != '\\' )
		m_strCurDir += szBackslash;

	m_szSourceDir	= m_strCurDir + szSrcDir;

	if( !f1.Open( m_szSourceDir + szBackslash + szOpenFile,
		CFile::modeRead | CFile::typeText ) )
	{
		m_szErrorText = "Can't open datafile " + szOpenFile;
		m_pLog->Comment( m_szErrorText );
		f1.Close();
		return FALSE;
	}
	char buf[256];

	while(f1.ReadString( buf, 256 ) != NULL)
	{
		if (buf[0] != '#')
		{
			if (buf[strlen(buf) - 1] == '\n')
				buf[strlen(buf) - 1] = '\0';
			szFileList->AddTail(buf);
		}
	}
	f1.Close();
	return TRUE;
}


BOOL CFILENAMEIDETest::CreateBuildProject( void )
{

	BOOL bResult = TRUE;
	UIOptionsTabDlg	uiopt;


// the IDE's UI has changed regarding setting fonts and this no longer works.
// Temporarily will not be used until have time to change cafe.
#if 0  
	if( VERIFY_TEST_SUCCESS( !uiopt.SetOption(TAB_FONTS, VCPP32_IDC_FONTLIST, "Fixedsys" )) )
	{
		m_szErrorText = "Default font set to Fixedsys failed";
		m_pLog->Comment( m_szErrorText );
		/*
			Clean up??? How to recover from this?
		*/
	}
	else
	{
//		MST.WButtonClick(GetLabel(VCPP32_IDC_USEASDEFAULT));
		MST.WButtonClick(GetLabel(IDOK));				//	uiopt.Close(); is broken
	}
#endif

	if( VERIFY_TEST_SUCCESS( CreateProject() ) )
	{
		m_szErrorText = "Internal makefile "+m_szProject+" could not be created";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else if( VERIFY_TEST_SUCCESS(!proj.RebuildAll(6)) )
	{
		m_szErrorText = "Build of "+m_szProject+" failed";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}

	return bResult;
}

/*
** create the project.  Instead of using a static .mak file which 
** becomes obsolete with changes to the project facility, create
** the project each time.  The project source files are static.
** We also get some freebies regarding setting target, because
** since we generate the makefile on the desired platform it will
** be the default target.
*/
BOOL CFILENAMEIDETest::CreateProject( void )
{
	m_pLog->Comment( "	-> Project.New" );
	if ( proj.NewProject( CONSOLE_APP, ProjOptions.szName, GetUserTargetPlatforms(), ProjOptions.szDir) != ERROR_SUCCESS)
	{
		m_szErrorText = "	unable to create new project";
		m_pLog->Comment( m_szErrorText );
		return FALSE;
	}

	CopySourceFiles();

	m_pLog->Comment( "	-> Project.Addfiles" );
	if ( proj.AddFiles( m_szProjectFiles ) != ERROR_SUCCESS)
	{
		m_szErrorText = "	adding files to project failed";
		m_pLog->Comment( m_szErrorText );
		return FALSE;
	}

	// workaround to the new project window (it can be hiden at this time)
	proj.ActivateProjWnd();

	//set intermediate directory
	m_pLog->Comment( "	-> Project.SetProjProperty" );
	if ( proj.SetProjProperty(CT_GENERAL,
			VPROJ_IDC_OUTDIRS_INT, 
			m_szNewIntermediateDir) != ERROR_SUCCESS)
	{
		m_szErrorText = "	setting intermediated directory failed";
		m_pLog->Comment( m_szErrorText );
		return FALSE;
	}

	//set output directory
	m_pLog->Comment( "	-> Project.SetProjOutputDir" );
	if (proj.SetProjProperty(CT_GENERAL,
			VPROJ_IDC_OUTDIRS_TRG, 
			m_szNewOutputDir) != ERROR_SUCCESS)
	{
		m_szErrorText = "	setting output directory failed";
		m_pLog->Comment( m_szErrorText );
		return FALSE;
	}

	return TRUE;
}

void CreateDirectoryLong(const char *path)
{
	char CurrentDir[_MAX_DIR], StartDir[_MAX_DIR], buf[_MAX_DIR], szPath[_MAX_DIR];
	char *chPtr;

	GetCurrentDirectory(_MAX_DIR - 1, CurrentDir);
	strcpy(StartDir, CurrentDir);
	strcpy (szPath, path);
	chPtr = strchr(szPath, '\\');
	while(chPtr)
	{
		*chPtr = '\0';
		strcpy(buf, szPath);
		CreateDirectory(buf, NULL);
		GetCurrentDirectory(_MAX_DIR - 1, CurrentDir);
		SetCurrentDirectory(CurrentDir + szBackslash + buf);
		strcpy(szPath, ++chPtr);
		chPtr = strchr(szPath, '\\');
	}

	strcpy(buf, szPath);
	CreateDirectory(buf, NULL);
	SetCurrentDirectory(StartDir);
	
}


void CFILENAMEIDETest::SetUp()
{

	char* psz = m_strCurDir.GetBufferSetLength(_MAX_DIR);

	m_szComment		= "This is a comment";
	m_szMem1		= "0123";
	m_szMem2		= "4567";

	GetCurrentDirectory(_MAX_DIR - 1, psz);
	m_strCurDir.ReleaseBuffer(-1);
	if( m_strCurDir.Right(1) != '\\' )
		m_strCurDir += szBackslash;

	char bufDir[_MAX_DIR];
	char *chLS;

	strcpy(bufDir, szTest2ndFile);	// Get complete path filename.
	chLS = strrchr(bufDir, '\\');	// Check for directory path existence
	if(chLS)
	{
		*chLS = '\0';
		szTest2ndFile = ++chLS;		// separate filename from path
		szTestDir = "test\\";
		szTestDir += bufDir;		// get directory path only

		// check for " in the filename.
		chLS = strrchr(szTest2ndFile, '"');
		if(chLS)
		{
			strcpy(bufDir, "\"");
			strcat(bufDir, szTest2ndFile);
			chLS = strrchr(bufDir, '"');
			*chLS = '\0';
			strcpy(bufDir, ".cpp\"");
			szTest2ndFile = bufDir;
		}
		else
			szTest2ndFile += ".cpp";	


	// check for " in the directory name.
		strcpy(bufDir, szTestDir);
		chLS = strchr(bufDir, '\"');
		if(chLS)
		{
			strcat(bufDir, "\"");
			szTestDir = bufDir;
		}
	}
	else
	{
		szTestDir = "test";

	// check for " in the filename.
		strcpy(bufDir, szTest2ndFile);
		chLS = strrchr(bufDir, '\"');
		if(chLS)
		{
			*chLS = '\0';
			strcat(bufDir, ".cpp\"");
			szTest2ndFile = bufDir;
		}
		else
			szTest2ndFile += ".cpp";
	}

// now szTest2ndFile has the correct filename
// and sztestDir has the corect directory

	szRootDir = m_strCurDir + "test";

	m_szSourceDir	= m_strCurDir + szSrcDir;

	chLS = strchr(szTestDir, '\"');
	if(chLS)
	{
		chLS++;
		m_szTestDir		= "\"" + m_strCurDir + chLS;
	}
	else
		m_szTestDir		= m_strCurDir + szTestDir;

	m_szProject		= m_szTestDir + szBackslash + szSrcMainFile;

	m_szBPLocation	= "{," + szTest2ndFile + ",}file_two";
	m_szQWExpr1		= "{file_two," + szTest2ndFile + ",}pch==glbDBNumbers";
	m_szQWExpr2		= "(int)*pch";

	m_szProjectFiles	= m_szTestDir + "\\*.cpp";
	m_szNewIntermediateDir = m_szTestDir + szBackslash + szTestMainFile;
	m_szNewOutputDir = m_szTestDir + szBackslash + szTestMainFile;

	ProjOptions.szDir	= m_szTestDir;
	ProjOptions.szName	= szTestMainFile;
	ProjOptions.iType	= CONSOLE_APP;
	ProjOptions.szNewSubDir = "";		// WinslowF for M3 work around  -> should be - m_szTestDir;


//	KillAllFiles((const char*) szTestDir+szBackslash+szTestMainFile, TRUE);
	KillAllFiles((const char*) szTestDir, TRUE);
	/*
		Copy mbctest.mak project to m_szTestDir from the m_szSourceDir
	*/
//	CreateDirectoryLong( (const char*) szTestDir);  

}

void CFILENAMEIDETest::CopySourceFiles()
{
	// create the new filename to copy
	CString NewTest2File;
	char bufDir[_MAX_DIR];
	char *chLS;
	char *chLS2;
	
	//m_szTestDir		= m_szTestDir + szBackslash + szTestMainFile;	// WinslowF Proj.New() change

	strcpy(bufDir, m_szTestDir);
	chLS = strrchr(bufDir, '\"');
	chLS2 = strchr(szTest2ndFile, '\"');
	if(chLS || chLS2)
	{
		if(chLS && chLS2)
		{
			chLS = '\0';
			NewTest2File	= bufDir + szBackslash +  ++chLS2;
		}
		else if(chLS)
		{
			chLS = '\0';
			NewTest2File	= bufDir + szBackslash + szTest2ndFile;
		}
		else if(chLS2)
		{
		//	NewTest2File	= "\"";
			NewTest2File += bufDir + szBackslash +  ++chLS2;
			chLS2 = strrchr(NewTest2File, '\"');
			*chLS2 = '\0';
		}

	}
	else
		NewTest2File	= m_szTestDir + szBackslash + szTest2ndFile;

	
	CStdioFile filerr;
	char buffer[20];
	int openerr = FALSE;
	int err;

	if( !filerr.Open( m_szSourceDir + szBackslash + szErrFile,
		CFile::modeCreate | CFile::modeWrite | CFile::typeText ) )
	{
		m_szErrorText = "Can't open error file" + szOpenFile;
		m_pLog->Comment( m_szErrorText );
		filerr.Close();
		openerr = TRUE;
	}

	::CopyFile( m_szSourceDir + szBackslash + szSrcMainFile+".cpp", m_szTestDir + szBackslash  + szTestMainFile +".cpp", FALSE );
	if(!openerr)
	{
		err = GetLastError();
		itoa(err, buffer, 10);
		filerr.WriteString( buffer );
		filerr.WriteString( "\n" );
	}
	::CopyFile( m_szSourceDir + szBackslash + szSrcMainFile+".h",   m_szTestDir + szBackslash  + szSrcMainFile+".h", FALSE );
	if(!openerr)
	{
		err = GetLastError();
		itoa(err, buffer, 10);
		filerr.WriteString( buffer );
		filerr.WriteString( "\n" );
	}
	::CopyFile( m_szSourceDir + szBackslash + szSrc2ndFile+".cpp",  NewTest2File, FALSE );
	if(!openerr)
	{
		err = GetLastError();
		itoa(err, buffer, 10);
		filerr.WriteString( buffer );
		filerr.WriteString( "\n" );
		filerr.Close();
	}

	::SetFileAttributes( m_szTestDir + szBackslash + szTestMainFile+".cpp" , FILE_ATTRIBUTE_NORMAL );	// Remove RO flag
	::SetFileAttributes( m_szTestDir + szBackslash + szSrcMainFile+".h" , FILE_ATTRIBUTE_NORMAL );	// Remove RO flag
	::SetFileAttributes( NewTest2File, FILE_ATTRIBUTE_NORMAL );	// Remove RO flag


	Sleep(10);
}


BOOL CFILENAMEIDETest::SetBreakpoints( void )
{

	BOOL bResult = TRUE;
	
	bp.ClearAllBreakpoints();

	if( VERIFY_BP_SUCCESS(!bp.SetBreakpoint(m_szBPLocation)) )
	{
		m_szErrorText = "Could not set breakpoint on "+m_szBPLocation;
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}

	return bResult;
}

BOOL CFILENAMEIDETest::ReadDBCSource( void )
{

	BOOL bResult = TRUE;
	int NStep;


#if defined( _MIPS_ )
	NStep = 2;
#else
	NStep = 3;
#endif



	if( VERIFY_TEST_SUCCESS(dbg.Go()) )
	{
		m_szErrorText = "This is a comment";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}

	else
	{
		MST.WFndWnd(szTest2ndFile, FW_FOCUS);
	
		 if( VERIFY_TEST_SUCCESS(dbg.StepOver(NStep, NULL, m_szComment )) )	  // "This is a comment"
		{
			m_szErrorText = "Read comment "+m_szComment+" in source failed, via StepOver";
			m_pLog->Comment( m_szErrorText );
			bResult = FALSE;
		}

	}

	return bResult;
}

BOOL CFILENAMEIDETest::ReadDBCDisasm( void )
{

	BOOL bResult = TRUE;
	UIDebug	dbgUI;
	UIDAM uidam = dbgUI.ShowDockWindow(IDW_DISASSY_WIN);

	if( VERIFY_TEST_SUCCESS(uidam.IsActive()) )
	{
		m_szErrorText = "Disassembly window is not active";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else
	{
		m_szDisasm = uidam.GetInstruction(-1,1);

		if( VERIFY_TEST_SUCCESS(m_szDisasm.Find(m_szComment) ) )	// position 58
		{
			m_szErrorText = "Read comment string "+m_szComment+
							" in disasm window failed";
			m_pLog->Comment( m_szErrorText );
			bResult = FALSE;
		}
	}

	dbgUI.ShowDockWindow(IDW_DISASSY_WIN, FALSE);
	return bResult;
}

BOOL CFILENAMEIDETest::QuickWatchTest( void )
{

	BOOL bResult = TRUE;

	if( VERIFY_TEST_SUCCESS(dbg.SetSteppingMode(SRC)) )
	{
		m_szErrorText = "Set debug stepping mode to SRC failed";
		m_pLog->Comment( m_szErrorText );
	}
	else if( VERIFY_TEST_SUCCESS(dbg.StepOver(2)) )
	{
		m_szErrorText = "Stepping over 2 lines failed";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else if( VERIFY_TEST_SUCCESS(qw.Enable()) )
	{
		m_szErrorText = "Quick watch dialog could not be enabled (1st time)";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else if( VERIFY_TEST_SUCCESS(qw.ExpressionValueIs( m_szQWExpr1, 1 )) )
	{
		m_szErrorText = "Quick watch expression: "+m_szQWExpr1+"did not equal 1";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else if( VERIFY_TEST_SUCCESS(qw.Disable()) )
	{
		m_szErrorText = "Quick watch dialog could not be disabled (1st time)";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else if( VERIFY_TEST_SUCCESS(dbg.StepOver(1)) )
	{
		m_szErrorText = "Stepping over 1 line failed";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else if( VERIFY_TEST_SUCCESS(qw.Enable()) )
	{
		m_szErrorText = "Quick watch dialog could not be enabled (2nd time)";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else if( VERIFY_TEST_SUCCESS(qw.ExpressionValueIs( m_szQWExpr2, 48 )) )
	{
		m_szErrorText = "Quick watch expression: "+m_szQWExpr2+"did not equal 48";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else if( VERIFY_TEST_SUCCESS(qw.Disable()) )
	{
		m_szErrorText = "Quick watch dialog could not be disabled (2nd time)";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}

	return bResult;
}

BOOL CFILENAMEIDETest::MemoryTest( void )
{

	BOOL bResult = TRUE;
	UIOptionsTabDlg	uiopt;
	UIMemory		uimem;
	COSource		src;	// WinslowF - added to call GoToLine. It's not in UIWB any more.
	UIDebug			dbgUI;

	UIWB.SetMemoryFormat(MEM_FORMAT_ASCII);			// Re-enable after fixed
	if( VERIFY_TEST_SUCCESS(!uiopt.SetOption(TAB_DEBUG, VCPP32_IDC_DEBUG_FORMAT, MEM_WIN_ASCII )) )
	{
		m_szErrorText = "Memory window to ASCII mode failed";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	else 
	{
		MST.WButtonClick(GetLabel(IDOK));				//	uiopt.Close(); is broken
		uimem = dbgUI.ShowDockWindow(IDW_MEMORY_WIN);
		if( VERIFY_TEST_SUCCESS(uimem.IsActive()) )
		{
			m_szErrorText = "Memory window was not active";
			m_pLog->Comment( m_szErrorText );
			bResult = FALSE;
		}
		else if( VERIFY_TEST_SUCCESS(src.GoToLine("*pch")) )
		{
			m_szErrorText = "Could not goto address of *pch";
			m_pLog->Comment( m_szErrorText );
			bResult = FALSE;
		}
		else
		{
			// This grabs 4 characters (4 bytes)
			m_szMemBuf = uimem.GetCurrentChars(4);
			m_szErrorText = "Retrieved: uimem.GetCurrentChars(4) = "+m_szMemBuf;
			m_pLog->Comment( m_szErrorText );

			if( VERIFY_TEST_SUCCESS(!m_szMemBuf.Compare(m_szMem1)) )
			{
				m_szErrorText = "string from memory differed, check src code.";
				m_pLog->Comment( m_szErrorText );
				bResult = FALSE;
			}
		
			// set 8 bytes (8 characters)
			if( VERIFY_TEST_SUCCESS(uimem.SetCurrentChars(m_szMem2)) )
			{
				m_szErrorText = "Setting memory with "+m_szMem2+" failed";
				m_pLog->Comment( m_szErrorText );
				bResult = FALSE;
			}
			m_szMemBuf = uimem.GetCurrentChars(8);
			m_szErrorText = "Retrieved: uimem.GetCurrentChars(8) = "+ m_szMem2 + m_szMem2;
			m_pLog->Comment( m_szErrorText );

			if( VERIFY_TEST_SUCCESS(!m_szMemBuf.Compare(m_szMem2 + m_szMem2)) )
			{
				m_szErrorText = "string "+m_szMem2+" from memory differed, "
								"check src code.";
				m_pLog->Comment( m_szErrorText );
				bResult = FALSE;
			}
		}
		dbgUI.ShowDockWindow(IDW_MEMORY_WIN, FALSE);
	}

	return bResult;
}

BOOL CFILENAMEIDETest::ENDThisTest( void )
{
	BOOL bResult = TRUE;

	/* if( VERIFY_TEST_SUCCESS ( dbg.StopDebugging(6) ) );
	{
		m_szErrorText = "can't stop debugging";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}

	if( VERIFY_TEST_SUCCESS ( proj.Close() ) );
	{
		m_szErrorText = "can't close the project";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}
	*/
	UIWBFrame  dbgFrame;

	dbg.StopDebugging();
	Sleep(10);
	dbgFrame.CloseAllWindows();
//	dbgFrame.CloseProject();
	Sleep(10);
	KillAllFiles((const char*) szRootDir, TRUE);
	Sleep(20);

return bResult;
}


		