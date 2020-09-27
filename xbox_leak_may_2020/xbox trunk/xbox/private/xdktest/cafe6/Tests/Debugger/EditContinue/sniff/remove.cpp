///////////////////////////////////////////////////////////////////////////////
//	Remove.CPP
//											 
//	Created by :			
//		VCBU QA		

#include <process.h>
#include "stdafx.h"
#include "Remove.h"	

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_DYNAMIC(CRemovingCodeTest, CECTestSet)
IMPLEMENT_TEST(CRemovingCodeTestEXE, CRemovingCodeTest, "EC Sniff RemovingCode EXE", -1, CSniffSubSuite)
IMPLEMENT_TEST(CRemovingCodeTestEXT, CRemovingCodeTest, "EC Sniff RemovingCode EXT", -1, CSniffSubSuite)
IMPLEMENT_TEST(CRemovingCodeTestINT, CRemovingCodeTest, "EC Sniff RemovingCode INT", -1, CSniffSubSuite)
												 
CRemovingCodeTest::CRemovingCodeTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CECTestSet(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}
												 
void CRemovingCodeTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CRemovingCodeTest::Run()
{	 
	XSAFETY;
	EXPECT_TRUE( dbg.ToggleEditAndContinue(TOGGLE_ON) );
	
	XSAFETY;
	RemoveFunction();	// 4 cases:
	XSAFETY;
	RemoveMemberFunction();	 // 4 cases: 2 passed, 2 failed :#23545, #23547
	XSAFETY;
	RemoveFunctionCall();  // 4 cases: 1 fails: #13137, 13568, 3 blocked
	XSAFETY;
	RemoveGlobal();		 // 4 cases:
	XSAFETY;
	RemoveDataMember();	  // 2 cases:
	XSAFETY;
	RemoveStaticVar();	// 2 cases:
	XSAFETY;
	RemoveLocalVar();	// 2 cases: 1st passed, 2nd failed - #13137, 13568
	XSAFETY;
	RemoveLocalObject();  // 2 cases: 1st passed, 2nd failed - #13772 crash
	XSAFETY;
	RemoveOperations();	  // 4 cases: 1 fails: #13137, 13568, 3 blocked
	XSAFETY;
	RemoveLoop();	 // 2 cases: 1 fails: #13137, 13568, 1 blocked
	XSAFETY;

	COWorkSpace ws;
	ws.CloseAllWindows();
	XSAFETY;
}

void CRemovingCodeTestEXE::Run()
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
		CRemovingCodeTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
		
}

void CRemovingCodeTestEXT::Run()
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
		CRemovingCodeTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}

void CRemovingCodeTestINT::Run()
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
		CRemovingCodeTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}


///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CRemovingCodeTest::RemoveFunction( )
{
// 	LogTestHeader( "RemoveFunction" );
	CStartEndTest se_test("RemoveFunction", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

 	LogTestBegin( "Remove a fuction (definition, declaration and calls) from inactive frame" );
	// Remove nest_func (definition, declaration and call) in testg.cpp
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	//EXPECT_TRUE( ModifyCodeLine("void nest_func();", "//void nest_func();") );
	EXPECT_TRUE( ModifyCode("void nest_func() {",
		"/*void nest_func() {{Enter} if (func()){Enter}	func_1param (func());{Enter}};*/", 4) );
	//EXPECT_TRUE( ModifyCodeLine("nest_func(); // nest_func call line", "//nest_func(); // nest_func call line") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
 	LogTestEnd( "Remove a fuction (definition, declaration and calls) from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a fuction (definition, declaration and calls) from inactive frame: Backup" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	//EXPECT_TRUE( ModifyCodeLine("//void nest_func();", "void nest_func();") );
	EXPECT_TRUE( ModifyCode("/*void nest_func() {",
		"void nest_func() {{Enter} if (func()){Enter}	func_1param (func());{Enter}};", 4) );
	//EXPECT_TRUE( ModifyCodeLine("//nest_func(); // nest_func call line", "nest_func(); // nest_func call line") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
 	LogTestEnd( "Remove a fuction (definition, declaration and calls) from inactive frame: Backup" );

 	LogTestBegin( "Remove a fuction definition from inactive frame" );
	dbg.Restart();
	// Remove only definition of the nest_func in testg.cpp
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("void nest_func() {",
		"/*void nest_func() {{Enter} if (func()){Enter}	func_1param (func());{Enter}};*/", 4) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
 	LogTestEnd( "Remove a fuction definition from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a fuction definition from inactive frame: Backup" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("/*void nest_func() {",
		"void nest_func() {{Enter} if (func()){Enter}	func_1param (func());{Enter}};", 4) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
 	LogTestEnd( "Remove a fuction definition from inactive frame: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveMemberFunction( )
{
// 	LogTestHeader( "RemoveMemberFunction" );
	CStartEndTest se_test("RemoveMemberFunction", this, TRUE);

	bps.ClearAllBreakpoints();

  	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("fooclass foo1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "fooclass foo1;") );

 	LogTestBegin( "Remove a member fuction (definition, declaration and calls) from inactive frame" );
	// Remove base::virtual_func3 (definition, declaration and call) in cxx.cxx
	EXPECT_TRUE( ModifyCodeLine("virtual int  virtual_func3(int ind) { return ind*2; };",
		"//virtual int  virtual_func3(int ind) { return ind*2; };") );
	EXPECT_TRUE( ModifyCodeLine("result = drv1.virtual_func3(9);",
		"//result = drv1.virtual_func3(9);") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests") );
 	LogTestEnd( "Remove a member fuction (definition, declaration and calls) from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a member fuction (definition, declaration and calls) from inactive frame: Backup" );
	EXPECT_TRUE( ModifyCodeLine("//virtual int  virtual_func3(int ind) { return ind*2; };",
		"virtual int  virtual_func3(int ind) { return ind*2; };") );
	EXPECT_TRUE( ModifyCodeLine("//result = drv1.virtual_func3(9);",
		"result = drv1.virtual_func3(9);") );
	EXPECT_TRUE( src.Find("int test_include = Test_func();") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "Test_func") );
 	LogTestBegin( "Remove a member fuction (definition, declaration and calls) from inactive frame: Backup" );

 	LogTestBegin( "Remove a member fuction definition from inactive frame" );
	dbg.Restart();
	// Remove only definition of inline_func in cxx.cxx
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("int test_include = Test_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int test_include = Test_func();") );
	EXPECT_TRUE( ModifyCodeLine("inline void inline_func() { private_value++; };",
		"inline void inline_func();") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
 	LogTestEnd( "Remove a member fuction definition from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a member fuction definition from inactive frame: Backup" );
	EXPECT_TRUE( ModifyCodeLine("inline void inline_func();",
		"inline void inline_func() { private_value++; };") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
 	LogTestEnd( "Remove a member fuction definition from inactive frame: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveFunctionCall( )
{
// 	LogTestHeader( "RemoveFunctionCall" );
	CStartEndTest se_test("RemoveFunctionCall", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	dbg.StepInto(1);

 	LogTestBegin( "Remove a fuction call from inactive frame" );
	// Remove a call to CFncInt64 in typetest.cpp
	UIWB.OpenFile( m_strProjectLoc + "typetest.cpp");
	EXPECT_TRUE( ModifyCodeLine("i64 = CFncInt64(i64);", "//i64 = CFncInt64(i64);") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "typetest.cpp" ) >= 0);  
 	LogTestEnd( "Remove a fuction call from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a fuction call from inactive frame: Backup" );
	UIWB.OpenFile( m_strProjectLoc + "typetest.cpp");
	EXPECT_TRUE( ModifyCodeLine("//i64 = CFncInt64(i64);", "i64 = CFncInt64(i64);") );
	EXPECT_TRUE( src.Find("i64 = CFncInt64(i64);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("Types_Tests") );
	EXPECT_TRUE( dbg.CurrentLineIs( "i64 = CFncInt64(i64);" ) );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "CFncInt64") );
 	LogTestEnd( "Remove a fuction call from inactive frame: Backup" );

 	LogTestBegin( "Remove a member fuction call from inactive frame" );
	dbg.Restart();
	// Remove a call to  base::virtual_func in cxx.cxx
	UIWB.OpenFile( m_strProjectLoc + "dbg_main.c");
	EXPECT_TRUE( src.Find("TagCallOtherMain:") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "OtherMain();") );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("pb->overloaded_func('a');",
		"//pb->overloaded_func('a');") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("OtherMain") );
 	LogTestEnd( "Remove a member fuction call from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a member fuction call from inactive frame: Backup" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("//pb->overloaded_func('a');",
		"pb->overloaded_func('a');") );
	EXPECT_TRUE( src.Find("pb->overloaded_func('a');") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "base::overloaded_func") );
 	LogTestEnd( "Remove a member fuction call from inactive frame: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveGlobal( )
{
 //	LogTestHeader( "RemoveGlobal" );
	CStartEndTest se_test("RemoveGlobal", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Remove a Global" );
 	LogTestHeader( "Remove Global: declaration and references", TT_TESTCASE );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("int GlobalG;",	"//int GlobalG;") );
	EXPECT_TRUE( ModifyCodeLine("GlobalG=77;",	"//GlobalG=77;") );
	EXPECT_TRUE( ModifyCodeLine("if (1 < Local) do { GlobalG++; } while (Local == 0);",	
		"//if (1 < Local) do { GlobalG++; } while (Local == 0);") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	LogTestEnd( "Remove a Global" );

	// backup changes and continue debugging
	LogTestBegin( "Remove a Global: backup" );
	EXPECT_TRUE( ModifyCodeLine("//int GlobalG;",	"int GlobalG;") );
	EXPECT_TRUE( ModifyCodeLine("//GlobalG=77;",	"GlobalG=77;") );
	EXPECT_TRUE( ModifyCodeLine("//if (1 < Local) do { GlobalG++; } while (Local == 0);",	
		"if (1 < Local) do { GlobalG++; } while (Local == 0);") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	LogTestEnd( "Remove a Global: backup" );

 	LogTestBegin( "Remove Global declaration" );
	dbg.Restart();
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("int GlobalG;",	"//int GlobalG;") );
//	dbg.StepInto();
	EXPECT_TRUE( src.Find("int TestG()") );
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
 	LogTestEnd( "Remove Global declaration" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove Global declaration: Backup" );
	EXPECT_TRUE( ModifyCodeLine("//int GlobalG;",	"int GlobalG;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
 	LogTestEnd( "Remove Global declaration: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveDataMember( )
{
// 	LogTestHeader( "RemoveDataMember" );
	CStartEndTest se_test("RemoveDataMember", this, TRUE);

	bps.ClearAllBreakpoints();

  	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "dbg_main.c");
	EXPECT_TRUE( src.Find("TagCallOtherMain:") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "OtherMain();") );

 	LogTestBegin( "Remove a data member from inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	// Remove base::m_protected in cxx.cxx
	EXPECT_TRUE( ModifyCodeLine("char *m_pchar;",	"//char *m_pchar;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("dbg_main") );
 	LogTestEnd( "Remove a data member from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a data member from inactive frame: Backup" );
	EXPECT_TRUE( ModifyCodeLine("//char *m_pchar;",	"char *m_pchar;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("OtherMain") );
 	LogTestEnd( "Remove a data member from inactive frame: Backup" );
 
	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveStaticVar( )
{
// 	LogTestHeader( "RemoveStaticVar" );
	CStartEndTest se_test("RemoveStaticVar", this, TRUE);

	bps.ClearAllBreakpoints();

  	// Start debugging
	EXPECT_TRUE( dbg.StepInto() );

 	LogTestBegin( "Remove a non-referenced static variable from inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "dbg_main.c");
	// Remove static double z from dbg_main.c
	EXPECT_TRUE( ModifyCodeLine("static double z;",	"//static double z;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "dbg_main.c" ) >= 0);  
 	LogTestEnd( "Remove a non-referenced static variable from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a non-referenced static variable from inactive frame: Backup" );
	UIWB.OpenFile( m_strProjectLoc + "dbg_main.c");
	EXPECT_TRUE( ModifyCodeLine("//static double z;",	"static double z;") );
	EXPECT_TRUE( src.Find("TagCallChainMain:") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("dbg_main") );
	EXPECT_TRUE( dbg.CurrentLineIs( "ChainMain();" ) );
//	EXPECT_TRUE( VerifyBuildString( "dbg_main.c" ) >= 0);  
 	LogTestEnd( "Remove a non-referenced static variable from inactive frame: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveLocalVar( )
{
// 	LogTestHeader( "RemoveLocalVar" );
	CStartEndTest se_test("RemoveLocalVar", this, TRUE);

	bps.ClearAllBreakpoints();

  	// Start debugging
	EXPECT_TRUE( dbg.StepInto() );

 	LogTestBegin( "Remove a local variable from inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "dbg_main.c");
	// Remove x variable from dbg_main.c
	EXPECT_TRUE( ModifyCodeLine("int x;",	"//int x;") );
	EXPECT_TRUE( ModifyCodeLine("x = y;",	"//x = y;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "dbg_main.c" ) >= 0);  
 	LogTestEnd( "Remove a local variable from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a local variable from inactive frame: Backup" );
	UIWB.OpenFile( m_strProjectLoc + "dbg_main.c");
	EXPECT_TRUE( ModifyCodeLine("//int x;",	"int x;") );
	EXPECT_TRUE( ModifyCodeLine("//x = y;",	"x = y;") );
	EXPECT_TRUE( src.Find("y = chain_x1(99);") );
//	dbg.StepToCursor(0);
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "dbg_main.c" ) >= 0);  
	EXPECT_TRUE( dbg.StepOver(1, NULL, "x = y;", "dbg_main"));
 	LogTestEnd( "Remove a local variable from inactive frame: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveLocalObject( )
{
 //	LogTestHeader( "RemoveLocalObject" );
	CStartEndTest se_test("RemoveLocalObject", this, TRUE);

	bps.ClearAllBreakpoints();

  	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "dbg_main.c");
	EXPECT_TRUE( src.Find("TagCallOtherMain:") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "OtherMain();") );

 	LogTestBegin( "Remove a local object from inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	// Remove base* object in cxx.cxx
	EXPECT_TRUE( ModifyCodeLine("base* object = object1;",	"//base* object = object1;") );
	EXPECT_TRUE( ModifyCodeLine("object = object2; // assignment",	"//object = object2; // assignment") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("OtherMain") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
 	LogTestEnd( "Remove a local object from inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Remove a local object from inactive frame: Backup" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("//base* object = object1;",	"base* object = object1;") );
	EXPECT_TRUE( ModifyCodeLine("//object = object2; // assignment",	"object = object2; // assignment") );
	EXPECT_TRUE( src.Find("object = object2; // assignment") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
	EXPECT_TRUE( dbg.CurrentLineIs( "object = object2; // assignment" ) );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
 	LogTestEnd( "Remove a local object from inactive frame: Backup" );
 
	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveOperations( )
{
// 	LogTestHeader( "RemoveOperations" );
	CStartEndTest se_test("RemoveOperations", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

//	LogSubTestHeader( "Remove operations from inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");

	LogTestBegin( "Remove operations from inactive frame: modify a statement" );
	EXPECT_TRUE( ModifyCodeLine("f_3p = l_3p - i_3p;", "f_3p = l_3p;") );
	EXPECT_TRUE( src.Find("f_3p = l_3p;") );
//	dbg.StepToCursor();
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	bps.ClearAllBreakpoints();
	EXPECT_TRUE( stk.CurrentFunctionIs("func_3param") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "f_3p = l_3p;" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "l_3p++;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("f_3p", 1033) );
//	float d_value;
//	EXPECT_TRUE( cxx.GetExpressionValue("f_3p", d_value) );
//	EXPECT_TRUE( d_value > 1032.0 && d_value <= 1033.0);
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "TestG") );
//	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main_Calls") );
//	bps.ClearAllBreakpoints();
	LogTestEnd( "Remove operations from inactive frame: modify a statement" );

	// backup changes and continue debugging
	LogTestBegin( "Remove operations from inactive frame: modify a statement: Backup" );
//	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("f_3p = l_3p;", "f_3p = l_3p - i_3p;") );
	EXPECT_TRUE( src.Find("f_3p = l_3p - i_3p;") );
//	dbg.StepToCursor();
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("func_3param") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "f_3p = l_3p - i_3p;" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "l_3p++;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("f_3p", 999) );
//	EXPECT_TRUE( cxx.GetExpressionValue("f_3p", d_value) );
//	EXPECT_TRUE( d_value > 998.0 && d_value <= 999.0);
	bps.ClearAllBreakpoints();
	LogTestEnd( "Remove operations from inactive frame: modify a statement: Backup" );

	LogTestBegin( "Remove operations from inactive frame: remove a statement" );
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( ModifyCodeLine("i_1 = 5;", "//i_1 = 5;") );
	EXPECT_TRUE( src.Find("//i_1 = 5;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("func1") );
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "int j_1 = 5;" ) );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "Based") );
	LogTestEnd( "Remove operations from inactive frame: remove a statement" );

	// backup changes and continue debugging
	LogTestBegin( "Remove operations from inactive frame: remove a statement: Backup" );
	EXPECT_TRUE( ModifyCodeLine("//i_1 = 5;", "i_1 = 5;") );
	EXPECT_TRUE( src.Find("i_1 = 5;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.StepOver(1, NULL, "int j_1 = 5;", "func1") );
	EXPECT_TRUE( cxx.ExpressionValueIs("i_1", 5) );
	LogTestEnd( "Remove operations from inactive frame: remove a statement: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CRemovingCodeTest::RemoveLoop( )
{
//	LogTestHeader( "RemoveLoop" );
	CStartEndTest se_test("RemoveLoop", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Remove a do_while_loop from inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("do { //func_3param",
		"/*do { //func_3param{Enter}  l_3p++;{Enter}}while(l_3p <1050);*/",
		3) );
	EXPECT_TRUE( src.Find("return d_3p;") );
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("func_3param") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "return d_3p;" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("l_3p", 1033) );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "TestG") );
	LogTestEnd( "Remove a do_while_loop from inactive frame" );

	// backup changes and continue debugging
	LogTestBegin( "Remove a do_while_loop from inactive frame: Backup" );
//	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("/*do { //func_3param",
		"do { //func_3param{Enter}  l_3p++;{Enter}}while(l_3p <1050);",
		3) );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("func_3param") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "return d_3p;" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("l_3p", 1050) );
	bps.ClearAllBreakpoints();
	LogTestEnd( "Remove a do_while_loop from inactive frame: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}


