///////////////////////////////////////////////////////////////////////////////
//	bp.CPP
//											 
//	Created by :			
//		VCBU QA		

#include <process.h>
#include "stdafx.h"
#include "bp.h"	

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
//IMPLEMENT_TEST(CBreakpointsTest, CECTestSet, "EC Sniff Breakpoints", -1, CSniffSubSuite)
IMPLEMENT_DYNAMIC(CBreakpointsTest, CECTestSet)
IMPLEMENT_TEST(CBreakpointsTestEXE, CBreakpointsTest, "EC Sniff Breakpoints EXE", -1, CSniffSubSuite)
IMPLEMENT_TEST(CBreakpointsTestEXT, CBreakpointsTest, "EC Sniff Breakpoints EXT", -1, CSniffSubSuite)
IMPLEMENT_TEST(CBreakpointsTestINT, CBreakpointsTest, "EC Sniff Breakpoints INT", -1, CSniffSubSuite)
												 
CBreakpointsTest::CBreakpointsTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CECTestSet(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}
												 
void CBreakpointsTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CBreakpointsTest::Run()
{	 
	XSAFETY;
	EXPECT_TRUE( dbg.ToggleEditAndContinue(TOGGLE_ON) );

	XSAFETY;
	BPAtNewCodeLine();	// 2 cases: 1 failed: # 13137/13568, 1 blocked
	XSAFETY;
	BPOnNewLocal();	  // 2 cases: 1 failed: # 14133, 1 blocked
	XSAFETY;
	BPOnNewLocalWithCodition();	  // 2 cases: 1 failed: # 14133, 1 blocked
	XSAFETY;

	COWorkSpace ws;
	ws.CloseAllWindows();
	XSAFETY;
}

void CBreakpointsTestEXE::Run()
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
		CBreakpointsTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
		
}

void CBreakpointsTestEXT::Run()
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
		CBreakpointsTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}

void CBreakpointsTestINT::Run()
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
		CBreakpointsTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CBreakpointsTest::BPAtNewCodeLine( )
{
// 	LogTestHeader( "BPAtNewCodeLine" );
	CStartEndTest se_test("BPAtNewCodeLine", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Add a new statement to inactive frame, set a breakpoint and GO" );

//	LogTestHeader( "Add operations to inactive frame: add a new statement", TT_TESTCASE );
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( ModifyCode("int j_1 = 5;",
		"int j_1 = 5;{Enter}   j_1 *= (i_1 + 2);", 1) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
	EXPECT_TRUE( dbg.CurrentLineIs( "Based();" ) );
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("j_1 *= (i_1 + 2);") );
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	//VerifyECSuccess();
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
	if( WaitMsgBoxText("One or more breakpoints are not positioned on valid lines", 280) )
	{
		MST.WButtonClick( "OK" ); 
	}
*/
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("func1") );
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "j_1 *= (i_1 + 2);" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "for (int i =0; i < 10; i++)	// func1") );
	EXPECT_TRUE( cxx.ExpressionValueIs("j_1", 35) );
	EXPECT_TRUE( dbg.StepOut() );
	LogTestEnd( "Add a new statement to inactive frame, set a breakpoint and GO" );

	// backup changes and continue debugging
	LogTestBegin( "Add a new statement to inactive frame, set a breakpoint and GO: Backup case" );
	EXPECT_TRUE( ModifyCode("int j_1 = 5;",
		"int j_1 = 5;", 2) );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("One or more breakpoints are not positioned on valid lines", 280) )
	{
		MST.WButtonClick( "OK" ); 
	}
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("func1") );
//	EXPECT_TRUE( dbg.CurrentLineIs( "for (int i =0; i < 10; i++)	// func1" ) );
	EXPECT_TRUE( dbg.CurrentLineIs( "int j_1 = 5;" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "for (int i =0; i < 10; i++)	// func1") );
	EXPECT_TRUE( cxx.ExpressionValueIs("j_1", 5) );
	LogTestEnd( "Add a new statement to inactive frame, set a breakpoint and GO: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CBreakpointsTest::BPOnNewLocal( )
{
// 	LogTestHeader( "BPOnNewLocal" );
	CStartEndTest se_test("BPOnNewLocal", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Add a local var to inactive frame and set a breakpoint" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int Local;",
		"int Local;{Enter}   double LocalVar = 9.9;", 1) );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	dbg.StepOver();
	EXPECT_TRUE( bps.SetBreakOnExpr("{TestG}LocalVar", COBP_TYPE_IF_EXP_CHANGED) != NULL );
	dbg.Go(NULL,NULL,NULL,0);
	if( WaitMsgBoxText("Break when '{TestG}LocalVar'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{TestG}LocalVar'\" was found as expected" );
		MST.WButtonClick( "OK" ); 
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{TestG}LocalVar'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "double LocalVar = 9.9;" ) );	
	EXPECT_TRUE( dbg.StepOver(1, NULL, "char Char = 'f';") );
	EXPECT_TRUE( cxx.ExpressionValueIs("LocalVar", 9.9) );
	bps.ClearAllBreakpoints();
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main_Calls") );
	LogTestEnd( "Add a local var to inactive frame and set a breakpoint" );

	// backup changes and continue debugging
	LogTestBegin( "Add a local var to inactive frame and set a breakpoint: backup case" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int Local;",
		"int Local;", 2) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	LogTestEnd( "Add a local var to inactive frame and set a breakpoint: backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CBreakpointsTest::BPOnNewLocalWithCodition( )
{
// 	LogTestHeader( "BPOnNewLocalWithCodition" );
	CStartEndTest se_test("BPOnNewLocalWithCodition", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Add a local var to inactive frame and set a breakpoint" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int Local;",
		"int Local;{Enter}   double LocalVar = 9.9;", 1) );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( bps.SetBreakOnExpr("{TestG}LocalVar", COBP_TYPE_IF_EXP_CHANGED, 3) != NULL );
//	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main_Calls") );
	dbg.Go(NULL,NULL,NULL,0);
	if( WaitMsgBoxText("Break when '{TestG}LocalVar'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{TestG}LocalVar'\" was found as expected" );
		MST.WButtonClick( "OK" ); 
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{TestG}LocalVar'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	bps.ClearAllBreakpoints();
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main_Calls") );
	LogTestEnd( "Add a local var to inactive frame and set a breakpoint" );

	// backup changes and continue debugging
	LogTestBegin( "Add a local var to inactive frame and set a breakpoint: Backup case" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int Local;",
		"int Local;", 2) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	LogTestEnd( "Add a local var to inactive frame and set a breakpoint: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}


