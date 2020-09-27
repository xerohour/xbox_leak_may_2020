///////////////////////////////////////////////////////////////////////////////
//	Mix.CPP
//											 
//	Created by :			
//		VCBU QA		

#include <process.h>
#include "stdafx.h"
#include "mix.h"	

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
//IMPLEMENT_TEST(CMixTest, CECTestSet, "EC Sniff Mix", -1, CSniffSubSuite)
IMPLEMENT_DYNAMIC(CMixTest, CECTestSet)
IMPLEMENT_TEST(CMixTestEXE, CMixTest, "EC Sniff Mix EXE", -1, CSniffSubSuite)
IMPLEMENT_TEST(CMixTestEXT, CMixTest, "EC Sniff Mix EXT", -1, CSniffSubSuite)
IMPLEMENT_TEST(CMixTestINT, CMixTest, "EC Sniff Mix INT", -1, CSniffSubSuite)
												 
CMixTest::CMixTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CECTestSet(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}
												 
void CMixTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CMixTest::Run()
{	 
	XSAFETY;
	EXPECT_TRUE( dbg.ToggleEditAndContinue(TOGGLE_ON) );
	
	XSAFETY;
	EditHeaderExe();   // 2 cases:
	XSAFETY;
	EditIncludeCpp();  // 2 cases:
	XSAFETY;
	ECStopDebuggingLink();	// 2 cases: #8951 - fixed
	XSAFETY;
	SyntaxError();		// 2 cases:
	XSAFETY;
		
	COWorkSpace ws;
	ws.CloseAllWindows();
	XSAFETY;
}

void CMixTestEXE::Run()
{	 
	XSAFETY;

	m_strProjectName = "ecconsol";	//ToDo: We should get the name from file with all languages strings
	m_strSrcDir = "ecapp";	//ToDo: We should get the name from file with all languages strings
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strSrcDir + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;

	RemoveTestFiles( m_strProjectLoc );
	CreateDirectory(m_strProjectLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );

	if( SetProject(szProjPathName) )
	{
		CMixTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
		
}

void CMixTestEXT::Run()
{	 
	XSAFETY;

	m_strProjectName = "ecwinapp";	//ToDo: We should get the name from file with all languages strings
	m_strSrcDir = "ecapp";	//ToDo: We should get the name from file with all languages strings
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strSrcDir + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;

	RemoveTestFiles( m_strProjectLoc );
	CreateDirectory(m_strProjectLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );

	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	if( PrepareProject(szProjPathName) && VERIFY_TEST_SUCCESS( proj.Build(iWaitForBuild)))
	{
		proj.SetExeForDebugSession(m_strProjectName + ".exe");
		CMixTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}

void CMixTestINT::Run()
{	 
	XSAFETY;

	m_strProjectName = "ecmfcapp";	//ToDo: We should get the name from file with all languages strings
	m_strSrcDir = "ecapp";	//ToDo: We should get the name from file with all languages strings
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strSrcDir + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;

	RemoveTestFiles( m_strProjectLoc + "res\\");
	RemoveTestFiles( m_strProjectLoc );
	CreateDirectory(m_strProjectLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );
	CreateDirectory(m_strProjectLoc + "res\\", 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc + "res\\", m_strProjectLoc + "res\\" ) );

	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	if( PrepareProject(szProjPathName) && VERIFY_TEST_SUCCESS( proj.Build(iWaitForBuild)))
	{
		proj.SetExeForDebugSession(m_strProjectName + ".exe");
		CMixTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}


///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CMixTest::EditHeaderExe( )
{
//	LogTestBegin( "EditHeaderExe" );
	CStartEndTest se_test("EditHeaderExe", this, TRUE);
	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Edit a header file for exe: Case 1");
	UIWB.OpenFile( m_strProjectLoc + "testg.h");

	EXPECT_TRUE( ModifyCodeLine("number*=2;", "number*=4;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC ignores the change
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	if( IsMsgBox("Cannot update current stack top frame", "OK") )
	{
		m_pLog->RecordInfo("OK on message box 'Cannot update current stack top frame'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue", FALSE ) < 0);  
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp", FALSE ) < 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	LogTestEnd( "Edit a header file for exe: Case 1");

	// backup changes and continue debugging
	LogTestBegin( "Edit a header file for exe: Backup Case 1");
	UIWB.OpenFile( m_strProjectLoc + "testg.h");
	EXPECT_TRUE( ModifyCodeLine("number*=4;", "number*=2;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC ignores the change
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	if( IsMsgBox("Cannot update current stack top frame", "OK") )
	{
		m_pLog->RecordInfo("OK on message box 'Cannot update current stack top frame'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue", FALSE ) < 0);  
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp", FALSE ) < 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	LogTestEnd( "Edit a header file for exe: Backup Case 1");

	dbg.StopDebugging();
//	LogTestEnd( "EditHeaderExe");
	return (TRUE);
}

BOOL CMixTest::EditIncludeCpp( )
{
// 	LogTestBegin( "EditIncludeCpp" );
	CStartEndTest se_test("EditIncludeCpp", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("fooclass foo1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "fooclass foo1;") );

	LogTestBegin( "Edit a include cpp file for exe: Case1" );
	UIWB.OpenFile( m_strProjectLoc + "include.cpp");

	EXPECT_TRUE( ModifyCodeLine("iGlobal2 = 0;", "iGlobal2 = 9;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC ignores the change
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	if( IsMsgBox("Cannot update current stack top frame", "OK") )
	{
		m_pLog->RecordInfo("OK on message box 'Cannot update current stack top frame'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue", FALSE ) < 0);  
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx", FALSE ) < 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests") );
	LogTestEnd( "Edit a include cpp file for exe: Case1" );

	// backup changes and continue debugging
	LogTestBegin( "Edit a include cpp file for exe: Backup Case1" );
	UIWB.OpenFile( m_strProjectLoc + "include.cpp");
	EXPECT_TRUE( ModifyCodeLine("iGlobal2 = 9;", "iGlobal2 = 0;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC ignores the change
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	if( IsMsgBox("Cannot update current stack top frame", "OK") )
	{
		m_pLog->RecordInfo("OK on message box 'Cannot update current stack top frame'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue", FALSE ) < 0);  
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx", FALSE ) < 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests") );
	LogTestEnd( "Edit a include cpp file for exe: Backup Case1" );

	dbg.StopDebugging();
// 	LogTestEnd( "EditIncludeCpp" );
	return (TRUE);
}

BOOL CMixTest::ECStopDebuggingLink( )
{
	CStartEndTest se_test("ECStopDebuggingLink", this, TRUE);
// 	LogTestBegin( "ECStopDebuggingLink" );
//	CTime timeEnd, timeStart = CTime::GetCurrentTime();

 	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( src.Find("Double = func_3param(Int, Long, Real);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Double = func_3param(Int, Long, Real);") );

 	LogTestBegin( "EditContinue in inactive frame StopDebugging VerifyLink" );
	EXPECT_TRUE( ModifyCodeLine("f_3p = l_3p - i_3p;", "f_3p = l_3p - i_3p + 2;") );
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.StepOver(1, NULL, "l_3p++;", "func_3param"));
	EXPECT_TRUE( cxx.ExpressionValueIs("f_3p", 1001) );
 	bps.ClearAllBreakpoints();
	dbg.StopDebugging();
	// Verify link line
	// Verify changes are stiil in the obj
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( src.Find("f_3p = l_3p - i_3p + 2;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "f_3p = l_3p - i_3p + 2;") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "l_3p++;", "func_3param"));
	EXPECT_TRUE( cxx.ExpressionValueIs("f_3p", 1001) );
 	LogTestEnd( "EditContinue in inactive frame StopDebugging VerifyLink" );

	// backup changes and continue debugging
 	LogTestBegin( "Backup case: EditContinue in inactive frame StopDebugging VerifyLink" );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "TestG"));
	EXPECT_TRUE( ModifyCodeLine("f_3p = l_3p - i_3p + 2;", "f_3p = l_3p - i_3p;") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
 	LogTestEnd( "Backup case: EditContinue in inactive frame StopDebugging VerifyLink" );

	dbg.StopDebugging();
/*	timeEnd = CTime::GetCurrentTime();
	CTimeSpan elapsed_time(timeEnd - timeStart);
	GetLog()->RecordInfo("***TEST elapsed time %s", elapsed_time.Format("%H:%M:%S"));
 	LogTestEnd( "ECStopDebuggingLink" );
*/	return (TRUE);
}

BOOL CMixTest::SyntaxError( )
{
// 	LogTestBegin( "SyntaxError" );
	CStartEndTest se_test("SyntaxError", this, TRUE);

 	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( src.Find("Double = func_3param(Int, Long, Real);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Double = func_3param(Int, Long, Real);") );

 	LogTestBegin( "Syntax Error in inactive frame" );
	EXPECT_TRUE( ModifyCodeLine("i_3p++;", "i_3p  intParam;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "syntax error" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
 	LogTestEnd( "Syntax Error in inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Backup case: Syntax Error in inactive frame" );
	EXPECT_TRUE( ModifyCodeLine("i_3p  intParam;", "i_3p++;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("func_3param") );
	EXPECT_TRUE( src.Find("i_3p++;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "i_3p++;") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "l_3p += i_3p;", "func_3param"));
	EXPECT_TRUE( cxx.ExpressionValueIs("i_3p", 34) );
 	LogTestEnd( "Backup case: Syntax Error in inactive frame" );

	dbg.StopDebugging();
// 	LogTestEnd( "SyntaxError" );
	return (TRUE);
}

