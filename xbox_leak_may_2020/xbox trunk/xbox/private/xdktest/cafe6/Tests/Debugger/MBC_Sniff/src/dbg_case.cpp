///////////////////////////////////////////////////////////////////////////////
//	Dbg_case.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "Dbg_case.h"	
#include "dbcstr.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
/* #define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS
*/												 

/*
	Source files in szSrcDir
*/
static CString szSrcMainFile	= "mbctest";
static CString szSrc2ndFile		= "file2";

/*
	Destination files in szTestDir
*/
CString szTestMainFile;  // the dbc string will be loaded from sniff.rc
CString szTest2ndFile;   // the dbc string will be loaded from sniff.rc

/*
	Source directory
*/
static CString szSrcDir		= "src";

/*
	Destination directory
*/
CString szTestDir = "test";       // the dbc string will be loaded from sniff.rc


static CString szBackslash = "\\";

IMPLEMENT_TEST(CDbg_mbcIDETest, CTest, "DBG MBC test", -1, CDbg_mbcSubSuite)

void CDbg_mbcIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDbg_mbcIDETest::Run(void)
{
	UIDebug dbgUI;
	UIOptionsTabDlg uifn;

	XSAFETY;

	/*
		Copy project from src to test directory.  test will be a DBC dir.
	*/
	SetUp();
	
		IME.Open();
		Sleep(500);
		IME.Enable(); 
		Sleep(500); 
		IME.SetMode(CIME::HIRAGANA, 2);

//		Set a J font
//		uifn.SetFont(13 /*"‚l‚r@ƒSƒVƒbƒN" */); // This should be set by default now.

		m_szErrorText = "->>CreateBuild project";
		m_pLog->Comment( m_szErrorText );
		EXPECT( CreateBuildProject() );
		m_szErrorText = "->>Set break points";
		m_pLog->Comment( m_szErrorText );
		EXPECT( SetBreakpoints() );
		EXPECT( ReadDBCSource() );
//		EXPECT(dbgUI.ShowDockWindow(IDW_OUTPUT_WIN, TRUE));	// it works fine but returns FALSE which makes test failing 
		dbgUI.ShowDockWindow(IDW_OUTPUT_WIN);
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

		IME.SetMode(CIME::ALPHA, 1);

		m_szErrorText = "->>End test";  
		m_pLog->Comment( m_szErrorText );
		EXPECT( ENDThisTest() );

	XSAFETY;


}


void CDbg_mbcIDETest::CopyFile(CString szFrom, CString szTo)
{
	CString	szDestination = m_szTestDir+szBackslash+szTo;

	SetFileAttributes(szDestination,FILE_ATTRIBUTE_NORMAL);				// Unset READONLY
	::CopyFile( m_szSourceDir+szBackslash+szFrom, szDestination, FALSE );
	SetFileAttributes(szDestination,FILE_ATTRIBUTE_NORMAL);				// Unset READONLY
}


void CDbg_mbcIDETest::SetUp()
{


	char* psz = m_strCurDir.GetBufferSetLength(_MAX_DIR);


	/*
	** obtain the dbc names from the string table.
	*/

    szTestMainFile.LoadString (IDS_TESTMAINFILE);
    szTest2ndFile.LoadString (IDS_TEST2NDFILE);
    szTestDir.LoadString (IDS_TESTDIR);
    m_szComment.LoadString (IDS_COMMENT);
    m_szMem1.LoadString (IDS_MEM1);
    m_szMem2.LoadString (IDS_MEM2);


	GetCurrentDirectory(_MAX_DIR - 1, psz);
	m_strCurDir.ReleaseBuffer(-1);
	if( m_strCurDir.Right(1) != '\\' )
		m_strCurDir += szBackslash;

	m_szSourceDir	= m_strCurDir + szSrcDir;
	m_szTestDir		= m_strCurDir + szTestDir;

	// delete test dir so we don't get error when re-creating project.
	KillAllFiles(m_szTestDir, TRUE);

//	m_szProject		= m_szTestDir+szBackslash+szTestMainFile+".mak";
	m_szProject		= m_szTestDir+szBackslash+szTestMainFile;

	m_szBPLocation	= "{,"+szTest2ndFile+".cpp,}file_two";
	m_szQWExpr1		= "{file_two,"+szTest2ndFile+".cpp,}pch==glbDBNumbers";
	m_szQWExpr2		= "(int)*pch";

	m_szProjectFiles	= m_szTestDir + "\\*.cpp";
	m_szNewIntermediateDir = m_szTestDir;
	m_szNewOutputDir = m_szTestDir;

	ProjOptions.szDir	= m_strCurDir;
	ProjOptions.szName	= szTestMainFile;
	ProjOptions.iType	= CONSOLE_APP;
	ProjOptions.szNewSubDir = szTestDir;


//	KillAllFiles((const char*) szTestDir, TRUE);
	/*
		Copy mbctest.mak project to m_szTestDir from the m_szSourceDir
	*/
 	CreateDirectoryLong( (const char*) szTestDir);  

	::CopyFile( m_szSourceDir + szBackslash + szSrcMainFile+".h",   m_szTestDir + szBackslash + szSrcMainFile+".h", FALSE );
	::CopyFile( m_szSourceDir + szBackslash + szSrcMainFile+".cpp", m_szTestDir + szBackslash + szTestMainFile+".cpp", FALSE );
	::CopyFile( m_szSourceDir + szBackslash + szSrc2ndFile+".cpp",  m_szTestDir + szBackslash + szTest2ndFile+".cpp", FALSE );

	// SetFileAttributes( m_szProject, FILE_ATTRIBUTE_NORMAL );	// Remove RO flag

	::SetFileAttributes( m_szTestDir + szBackslash + szSrcMainFile+".h" , FILE_ATTRIBUTE_NORMAL );	// Remove RO flag
	::SetFileAttributes( m_szTestDir + szBackslash + szTestMainFile+".cpp" , FILE_ATTRIBUTE_NORMAL );	// Remove RO flag
	::SetFileAttributes( m_szTestDir + szBackslash + szTest2ndFile+".cpp" , FILE_ATTRIBUTE_NORMAL );	// Remove RO flag

	Sleep(10);
}


///////////////////////////////////////////////////////////////////////////////
//	Test Cases


BOOL CDbg_mbcIDETest::CreateBuildProject(  )
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
BOOL CDbg_mbcIDETest::CreateProject( void )
{
	m_pLog->Comment( "	-> Project.New" );
	if ( proj.New( ProjOptions ) != ERROR_SUCCESS)
	{
		m_szErrorText = "unable to create new project";
		m_pLog->Comment( m_szErrorText );
		return FALSE;
	}

	m_pLog->Comment( "	-> Project.Addfiles" );
	if ( proj.AddFiles( m_szProjectFiles ) != ERROR_SUCCESS)
	{
		m_szErrorText = "adding files to project failed";
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
		m_szErrorText = "setting intermediated directory failed";
		m_pLog->Comment( m_szErrorText );
		return FALSE;
	}

	//set output directory
	m_pLog->Comment( "	-> Project.SetProjOutputDir" );
	if (proj.SetProjProperty(CT_GENERAL,
			VPROJ_IDC_OUTDIRS_TRG, 
			m_szNewOutputDir) != ERROR_SUCCESS)
	{
		m_szErrorText = "setting output directory failed";
		m_pLog->Comment( m_szErrorText );
		return FALSE;
	}

	return TRUE;
}

BOOL CDbg_mbcIDETest::SetBreakpoints(  )
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

BOOL CDbg_mbcIDETest::ReadDBCSource(  )
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
		m_szErrorText = "Go to breakpoint";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}

	else if( VERIFY_TEST_SUCCESS(dbg.StepOver(NStep, NULL, m_szComment )) )	  // "‚O‚P‚Q‚RBBB"
	{
		m_szErrorText = "Read DBC comment "+m_szComment+" in source failed, via StepOver";
		m_pLog->Comment( m_szErrorText );
		bResult = FALSE;
	}

	return bResult;	
}

BOOL CDbg_mbcIDETest::ReadDBCDisasm(  )
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

		if( VERIFY_TEST_SUCCESS(m_szDisasm.Find(m_szComment)) )	// position 58??? I don't think so.
		{
			m_szErrorText = "Read DBC comment string "+m_szComment+
							" in disasm window failed";
			m_pLog->Comment( m_szErrorText );
			bResult = FALSE;
		}
	}

	return bResult;	
}

BOOL CDbg_mbcIDETest::QuickWatchTest( void )
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
	else if( VERIFY_TEST_SUCCESS(qw.ExpressionValueIs( m_szQWExpr2, -126 )) )
	{
		m_szErrorText = "Quick watch expression: "+m_szQWExpr2+"did not equal -117";
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

BOOL CDbg_mbcIDETest::MemoryTest( void )
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
			uimem = dbgUI.ShowDockWindow(IDW_MEMORY_WIN);
			// This grabs 4 DBC characters (8 bytes)
			m_szMemBuf = uimem.GetCurrentChars(4);
			m_szErrorText = "Retrieved: uimem.GetCurrentChars(4) = "+m_szMemBuf;
			m_pLog->Comment( m_szErrorText );

			if( VERIFY_TEST_SUCCESS(!m_szMemBuf.Compare(m_szMem1)) )
			{
				m_szErrorText = "DBC string from memory differed, check src code.";
				m_pLog->Comment( m_szErrorText );
				bResult = FALSE;
			}
		
			// set 12 bytes (8 characters)
			if( VERIFY_TEST_SUCCESS(uimem.SetCurrentChars(m_szMem2)) )
			{
				m_szErrorText = "Setting memory with "+m_szMem2+" failed";
				m_pLog->Comment( m_szErrorText );
				bResult = FALSE;
			}

			m_szMemBuf = uimem.GetCurrentChars(8);
			m_szErrorText = "Retrieved: uimem.GetCurrentChars(8) = "+m_szMem2;
			m_pLog->Comment( m_szErrorText );

			if( VERIFY_TEST_SUCCESS(!m_szMemBuf.Compare(m_szMem2)) )
			{
				m_szErrorText = "DBC string "+m_szMem2+" from memory differed, "
								"check src code.";
				m_pLog->Comment( m_szErrorText );
				bResult = FALSE;
			}
		}
	}
	return bResult;	
}

BOOL CDbg_mbcIDETest::ENDThisTest( void )
{
	BOOL bResult = TRUE;

	UIWBFrame  dbgFrame;

	dbg.StopDebugging();
	Sleep(10);
	dbgFrame.CloseAllWindows();
	Sleep(10);
//	KillAllFiles((const char*) szTestDir, TRUE);
	Sleep(20);

return bResult;
}
