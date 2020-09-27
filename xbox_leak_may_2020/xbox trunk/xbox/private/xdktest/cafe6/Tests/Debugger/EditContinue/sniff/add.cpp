///////////////////////////////////////////////////////////////////////////
//	Add.CPP
//											 
//	Created by :			
//		VCBU QA		

#include <process.h>
#include "stdafx.h"
#include "Add.h"	

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
//IMPLEMENT_TEST(CAddingCodeTest, CECTestSet, "EC Sniff AddingCode", -1, CSniffSubSuite)
IMPLEMENT_DYNAMIC(CAddingCodeTest, CECTestSet)
IMPLEMENT_TEST(CAddingCodeTestEXE, CAddingCodeTest, "EC Sniff AddingCode EXE", -1, CSniffSubSuite)
IMPLEMENT_TEST(CAddingCodeTestEXT, CAddingCodeTest, "EC Sniff AddingCode EXT", -1, CSniffSubSuite)
IMPLEMENT_TEST(CAddingCodeTestINT, CAddingCodeTest, "EC Sniff AddingCode INT", -1, CSniffSubSuite)
												 
CAddingCodeTest::CAddingCodeTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CECTestSet(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}
												 
void CAddingCodeTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void CAddingCodeTest::Run()
{	 
	XSAFETY;
	EXPECT_TRUE( dbg.ToggleEditAndContinue(TOGGLE_ON) );
	
	XSAFETY;
	AddFunction();	 // 6 cases: 1 fails - #12795, 12799, 5 blocked.
	XSAFETY;
	AddMemberFunction(); // 2 cases:
	XSAFETY;
	AddFunctionCall(); // 8 cases: 1 fails - #12924, 12932, 7 blocked
	XSAFETY;
	AddGlobal();	 // 2 cases:
	XSAFETY;
	AddDataMember();   // 2 cases:
	XSAFETY;
	AddStaticVar();	   // 2 cases:
	XSAFETY;
	AddLocalVar();	   // 2 cases:
	XSAFETY;
	AddLocalObject();   // 2 cases: 1 fails - #13772 - crash
	XSAFETY;
	AddOperations();	// 4 cases: 1 fails - #13956, 3 blocked
	XSAFETY;
	AddLoop();	   // 2 cases: 1 fails - #13956, 1 blocked
	XSAFETY;

	COWorkSpace ws;
	ws.CloseAllWindows();
	XSAFETY;
}

void CAddingCodeTestEXE::Run()
{	 
	XSAFETY;

/*	m_strProjectName = "ecconsol";	//ToDo: We should get the name from file with all languages strings
	m_strSrcDir = "ecapp";	//ToDo: We should get the name from file with all languages strings
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strSrcDir + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;

	RemoveTestFiles( m_strProjectLoc );
	CreateDirectory(m_strProjectLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );

	if( SetProject(szProjPathName) )
	{
		CAddingCodeTest::Run();
	}
*/
	m_projType = PT_EXE;
	if (IsProjectReady(m_projType))
		CAddingCodeTest::Run();

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
		
}

void CAddingCodeTestEXT::Run()
{	 
	XSAFETY;
	
/*	m_strProjectName = "ecwinapp";	//ToDo: We should get the name from file with all languages strings
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
		CAddingCodeTest::Run();
	}
*/
	m_projType = PT_EXT;
	if (IsProjectReady(m_projType))
		CAddingCodeTest::Run();

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}

void CAddingCodeTestINT::Run()
{	 
	XSAFETY;

/*	m_strProjectName = "ecmfcapp";	//ToDo: We should get the name from file with all languages strings
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
		CAddingCodeTest::Run();
	}
*/
	m_projType = PT_INT;
	if (IsProjectReady(m_projType))
		CAddingCodeTest::Run();

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}


///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CAddingCodeTest::AddFunction( )
{
// 	LogTestHeader( "AddFunction" );
	CStartEndTest se_test("AddFunction", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

// 	LogSubTestHeader( "Add a fuction using known return type" );
//	LogTestHeader( "Add a fuction using known return type", TT_TESTCASE );
 	LogTestBegin( "Add a fuction using known return type" );
	// Add func2 in testg.cpp
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int func();",
		"int func();{Enter}   int func2();", 1) );
	EXPECT_TRUE( ModifyCode("typedef int My_typedef;",
		"typedef int My_typedef;{Enter}   int func2(){Enter} { return 55; };", 1) );
	EXPECT_TRUE( ModifyCode("Enum_local = one;",
		"Enum_local = one;{Enter}   Int = func2();", 1) );
	EXPECT_TRUE( src.Find("{ return 55; };") );
	dbg.StepToCursor(0);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
	{
		WriteLog(FAILED, "Msg box should not appear" );
//	m_pLog->RecordInfo( " Run an app and verify it ran " );
		MST.WButtonClick( "No" );
	}
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.CurrentLineIs( "{ return 55; };" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Int = func2();"));
	EXPECT_TRUE( dbg.StepOver(1, NULL, "if (Flag)"));
	EXPECT_TRUE( cxx.ExpressionValueIs("Int", 55) );
	dbg.StopDebugging();
 	LogTestEnd( "Add a fuction using known return type" );

	LogTestBegin( "Add a fuction using typedef return type" );
	// Start debugging again
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	// Add func3 in testg.cpp
//	EXPECT_TRUE( ModifyCode("int func();",
//		"int func();{Enter}   My_typedef func3();", 1) );
	EXPECT_TRUE( ModifyCode("typedef int My_typedef;",
		"typedef int My_typedef;{Enter}   My_typedef func3(){Enter} { return 99; };", 1) );
	EXPECT_TRUE( ModifyCode("Enum_local = one;",
		"Enum_local = one;{Enter}   Mine = func3();", 1) );
	EXPECT_TRUE( src.Find("{ return 99; };") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "{ return 99; };" ) );
	EXPECT_TRUE( stk.CurrentFunctionIs("func3") );
	EXPECT_TRUE( dbg.StepOut(1, NULL, "Mine = func3();", "TestG"));
	EXPECT_TRUE( dbg.StepOver(1));
	EXPECT_TRUE( cxx.ExpressionValueIs("Mine", 99) );
	LogTestEnd( "Add a fuction using typedef return type" );

	// Need a new application and use double  : will move to a different test
	LogTestBegin( "Add a fuction using new return type" );
	// Add long_double_func to based.cpp
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( ModifyCode("int (*pfn222)();",
		"int (*pfn222)();{Enter}   long double * long_double_func(int addr);{Enter}   { return (long double *)addr; }", 1) );
	EXPECT_TRUE( ModifyCode("int *p_global_int2;",
		"int *p_global_int2;{Enter}   p_global_double = (double *)long_double_func((int)rg_global_int);", 1) );
	EXPECT_TRUE( src.Find("p_global_double = (double *)long_double_func((int)rg_global_int);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") ); // Check where should we be: main_Calls or TestG ?
	LogTestEnd( "Add a fuction using new return type" );

	// Backup changes and continue
	LogTestBegin( "Backup case: Remove 2 functions" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int func();",
		"int func();", 2) );
	EXPECT_TRUE( ModifyCode("typedef int My_typedef;",
		"typedef int My_typedef;", 5) );
	EXPECT_TRUE( ModifyCode("Enum_local = one;",
		"Enum_local = one;", 3) );
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( ModifyCode("int (*pfn222)();",
		"int (*pfn222)();", 3) );
	EXPECT_TRUE( ModifyCode("int *p_global_int2;",
		"int *p_global_int2;", 2) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
//	VerifyECSuccess();
// TODO: Check where we should be at the time
	// Verify EC fails gracefully after removing the new function
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") ); 
	LogTestEnd( "Backup case: Remove 2 functions" );

	dbg.StopDebugging();
	if (IsProjectReady(m_projType))
		return (TRUE);
	else 
		return FALSE;
}

BOOL CAddingCodeTest::AddMemberFunction( )
{
// 	LogTestHeader( "AddMemberFunction" );
	CStartEndTest se_test("AddMemberFunction", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("fooclass foo1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "fooclass foo1;") );

 	LogTestBegin( "Add a member fuction" );
	// Add derID to derived class in cxx.cxx
	EXPECT_TRUE( ModifyCode("derived(); // c-tor",
		"derived(); // c-tor{Enter}   int derID() {return id;};", 1) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
 	LogTestEnd( "Add a member fuction" );

	// backup changes and continue
 	LogTestBegin( "Add a member fuction: Backup case" );
	EXPECT_TRUE( ModifyCode("derived(); // c-tor",
		"derived();", 2) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
 	LogTestEnd( "Add a member fuction: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CAddingCodeTest::AddFunctionCall( )
{
// 	LogTestHeader( "AddFunctionCall" );
	CStartEndTest se_test("AddFunctionCall", this, TRUE);

	bps.ClearAllBreakpoints();

 	LogTestBegin( "Add a member fuction call to inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("Result=foo1.getCount();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Result=foo1.getCount();") );
	// Add a call to overloaded_func inside Test_func in cxx.cxx
	EXPECT_TRUE( ModifyCode("result = drv1.getCount();",
		"result = drv1.getCount();{Enter}   result = b.overloaded_func(1002);", 1) );
	EXPECT_TRUE( src.Find("result = b.overloaded_func(1002);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "result = b.overloaded_func(1002);" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "result = drv1;"));
	EXPECT_TRUE( cxx.ExpressionValueIs("result", 1002) );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
 	LogTestEnd( "Add a member fuction call to inactive frame" );

 	LogTestBegin( "Add a member fuction call to active frame before IP: Invalid call" );
	EXPECT_TRUE( src.Find("Result=foo1.getCount();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Result=foo1.getCount();") );
	// Add a call to overloaded_func inside Test_func in cxx.cxx
	EXPECT_TRUE( ModifyCode("result = drv1.getCount();",
		"result = drv1.getCount();{Enter}   result = derived::getID(); // new call", 1) );
//ToDo: this generate compiler error: need to replace derived::getID() by something else
	EXPECT_TRUE( src.Find("result = derived::getID(); // new call") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully : this generates compiler error
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
 	LogTestEnd( "Add a member fuction call to active frame before IP: Invalid call" );

	// Backup the last change and add a valid call
 	LogTestBegin( "Add a member fuction call to active frame before IP: Valid call" );
	EXPECT_TRUE( ModifyCode("result = drv1.getCount();",
		"result = drv1.getCount();{Enter}   result = derived::static_func(); // new call", 2) );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "result = derived::static_func(); // new call" ) );
//	EXPECT_TRUE( dbg.StepOver(1, NULL, "result = b.overloaded_func(1002);"));
	EXPECT_TRUE( dbg.StepOver(1));
	EXPECT_TRUE( cxx.ExpressionValueIs("result", 9) );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
 	LogTestEnd( "Add a member fuction call to active frame before IP: Valid call" );

 	LogTestBegin( "Add a member fuction call to non-top active frame after IP" );
	// Add a call to fooclass::chain2_func inside Cxx_Tests in cxx.cxx
	EXPECT_TRUE( ModifyCode("int test_include = Test_func();",
		"int test_include = Test_func();{Enter}   Result = foo1.chain2_func();", 1) );
	EXPECT_TRUE( src.Find("Result = foo1.chain2_func();") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
//	VerifyECSuccess();
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests")	);
	dbg.Restart();
	dbg.SetSteppingMode(SRC);
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("Result = foo1.chain2_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Result = foo1.chain2_func();", "Cxx_Tests" ) );
//	EXPECT_TRUE( dbg.CurrentLineIs( "Result = foo1.chain2_func();" ) );
//	EXPECT_TRUE( dbg.StepOver(1, NULL, "}"));
	EXPECT_TRUE( dbg.StepOver(1));
	EXPECT_TRUE( cxx.ExpressionValueIs("Result", 3) );
//	EXPECT_TRUE( VerifyBuildString( "foo.c" ) >= 0);  
 	LogTestEnd( "Add a member fuction call to non-top active frame after IP" );

 	LogTestBegin( "Add a fuction call to inactive frame" );
	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );
	// Add a call to func_1param in testg.cpp
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("Enum_local = one;",
		"Enum_local = one;{Enter}   Int = func_1param(1001);", 1) );
	EXPECT_TRUE( src.Find("Int = func_1param(1001);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "Int = func_1param(1001);" ) );
//	EXPECT_TRUE( dbg.StepOver(1, NULL, "if (Flag)"));
	EXPECT_TRUE( dbg.StepOver(1));
	EXPECT_TRUE( cxx.ExpressionValueIs("Int", 1001) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
 	LogTestEnd( "Add a fuction call to inactive frame" );
	
 	LogTestBegin( "Add a fuction call to active frame At IP" );
	// Add a call to func_2param in testg.cpp
	EXPECT_TRUE( ModifyCodeLine("Int = func_1param(1001);", "Long = func_2param(44, 55); Int = func_1param(1001);") );
	EXPECT_TRUE( src.Find("Long = func_2param(44, 55); Int = func_1param(1001);") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( dbg.CurrentLineIs( "if (Flag)" ) );
//	EXPECT_TRUE( cxx.ExpressionValueIs("Long", 99) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
 	LogTestEnd( "Add a fuction call to active frame At IP" );
	
 	LogTestBegin( "Add a fuction call to active frame After IP" );
	// Add a call to foo_Include in testg.cpp
	EXPECT_TRUE( ModifyCode("Int = foo_Include(i);",
		"Int = foo_Include(i);{Enter}   Int = 3+foo_Include(i);", 1) );
	EXPECT_TRUE( src.Find("Int = 3+foo_Include(i);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "Int = 3+foo_Include(i);" ) );
//	EXPECT_TRUE( dbg.StepOver(1, NULL, "Int = recur_func();"));
	EXPECT_TRUE( dbg.StepOver(1));
	EXPECT_TRUE( cxx.ExpressionValueIs("Int", 3) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
 	LogTestEnd( "Add a fuction call to active frame After IP" );
	
 	LogTestBegin( "Add a fuction call to active non-top frame Before IP" );
	// Add a call to Based in calls.cpp while IP is in TestG
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( ModifyCode("int iCalls = Calls();",
		"int iCalls = Calls();{Enter}   Based(); // new call", 1) );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
//	VerifyECSuccess();
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
//	EXPECT_TRUE( VerifyBuildString( "calls.cpp" ) >= 0);  
 	LogTestEnd( "Add a fuction call to active non-top frame Before IP" );
	
	// Backup changes == remove all new function calls
 	LogTestBegin( "Backup case: Backup all changes and continue" );
	EXPECT_TRUE( ModifyCode("int iCalls = Calls();",
		"int iCalls = Calls();", 2) );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCode("result = drv1.getCount();",
		"result = drv1.getCount();", 3) );
	EXPECT_TRUE( ModifyCode("int test_include = Test_func();",
		"int test_include = Test_func();", 2) );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("Enum_local = one;",
		"Enum_local = one;", 2) );
	EXPECT_TRUE( ModifyCode("Int = foo_Include(i);",
		"Int = foo_Include(i);", 2) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
//	EXPECT_TRUE( VerifyBuildString( "calls.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "foo.c" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
 	LogTestEnd( "Backup case: Backup all changes and continue" );
	
	dbg.StopDebugging();
	return (TRUE);
}

BOOL CAddingCodeTest::AddGlobal( )
{
// 	LogTestHeader( "AddGlobal" );
	CStartEndTest se_test("AddGlobal", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Add a Global" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int GlobalG;",
		"int GlobalG;{Enter}   __int64 Global64;", 1) );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	LogTestEnd( "Add a Global" );

	// backup changes and continue debugging
	LogTestBegin( "Add a Global: backup case" );
	EXPECT_TRUE( ModifyCode("int GlobalG;",
		"int GlobalG;", 2) );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	LogTestEnd( "Add a Global: backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CAddingCodeTest::AddDataMember( )
{
// 	LogTestHeader( "AddDataMember" );
	CStartEndTest se_test("AddDataMember", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("fooclass foo1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "fooclass foo1;") );

 	LogTestBegin( "Add a data member" );
	// Add __int64* pi64 to derived class in cxx.cxx
	EXPECT_TRUE( ModifyCode("int id;",
		"int id;{Enter}   __int64* pi64;", 1) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);
 	LogTestEnd( "Add a data member" );
	
	// backup changes and continue debugging
 	LogTestBegin( "Add a data member: backup case" );
	EXPECT_TRUE( ModifyCode("int id;",
		"int id;", 2) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "Result=foo1.getID(); // getID" ) );
 	LogTestEnd( "Add a data member: backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CAddingCodeTest::AddStaticVar( )
{
// 	LogTestHeader( "AddStaticVar" );
	CStartEndTest se_test("AddStaticVar", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	bps.SetBreakpoint("TestG");
	EXPECT_TRUE( dbg.Go(NULL, NULL, "TestG") );

	LogTestBegin( "Add a Static var" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int func() {",
		"int func() {{Enter}   static __int64 statVar;", 1) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	LogTestEnd( "Add a Static var" );

	// backup changes and continue debugging
	LogTestBegin( "Add a Static var: Backup case" );
	EXPECT_TRUE( ModifyCode("int func() {",
		"int func() {", 2) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	EXPECT_TRUE( dbg.CurrentLineIs( "char Char = 'f';" ) );
	LogTestEnd( "Add a Static var: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CAddingCodeTest::AddLocalVar( )
{
// 	LogTestHeader( "AddLocalVar" );
	CStartEndTest se_test("AddLocalVar", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Add a local var to inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCode("int Local;",
		"int Local;{Enter}   double LocalVar;", 1) );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	LogTestEnd( "Add a local var to inactive frame" );

	// backup changes and continue debugging
	LogTestBegin( "Add a local var to inactive frame: Backup case" );
	EXPECT_TRUE( ModifyCode("int Local;",
		"int Local;", 2) );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	EXPECT_TRUE( dbg.CurrentLineIs( "char Char = 'f';" ) );	 // Can't be verified till active frame E&C is implemented
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	LogTestEnd( "Add a local var to inactive frame: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CAddingCodeTest::AddLocalObject( )
{
// 	LogTestHeader( "AddLocalObject" );
	CStartEndTest se_test("AddLocalObject", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("fooclass foo1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "fooclass foo1;") );

 	LogTestBegin( "Add a local object to inactive frame" );
	// Add __int64* pi64 to derived class in cxx.cxx
	EXPECT_TRUE( ModifyCode("derivedm drvm;",
		"derivedm drvm;{Enter}   derivedm drvm2;", 1) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Cannot update current stack top frame", "OK") )
	{
//		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Cannot update current stack top frame'");
	}
	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
//		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
 	LogTestEnd( "Add a local object to inactive frame" );

	// backup changes and continue debugging
 	LogTestBegin( "Add a local object to inactive frame: Backup case" );
	EXPECT_TRUE( ModifyCode("derivedm drvm;",
		"derivedm drvm;", 2) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
//	VerifyECSuccess();
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Cannot update current stack top frame", "OK") )
	{
//		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Cannot update current stack top frame'");
	}
	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
//		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
 	LogTestEnd( "Add a local object to inactive frame: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CAddingCodeTest::AddOperations( )
{
// 	LogTestHeader( "AddOperations" );
	CStartEndTest se_test("AddOperations", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Add operations to inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");

	LogTestHeader( "Add operations to inactive frame: modify a statement", TT_TESTCASE );
	EXPECT_TRUE( ModifyCodeLine("Union_local.Long = 999;", "Union_local.Long = 999 - Union_local.Int;") );
	EXPECT_TRUE( src.Find("Union_local.Long = 999 - Union_local.Int;") );
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "Union_local.Long = 999 - Union_local.Int;" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Enum_local = one;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("Union_local.Long", 994) );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main_Calls") );
	bps.ClearAllBreakpoints();
	LogTestEnd( "Add operations to inactive frame" );

	// backup changes and continue debugging
	LogTestBegin( "Add operations to inactive frame: Backup case" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("Union_local.Long = 999 - Union_local.Int;", "Union_local.Long = 999;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	LogTestEnd( "Add operations to inactive frame: Backup case" );

	LogTestBegin( "Add operations to inactive frame: add a new statement" );
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( ModifyCode("int j_1 = 5;",
		"int j_1 = 5;{Enter}   j_1 *= i_1;", 1) );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("func1") );
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "j_1 *= i_1;" ) );
//	EXPECT_TRUE( dbg.StepOver(1, NULL, "Enum_local = one;") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "for (int i =0; i < 10; i++)	// func1") );
	EXPECT_TRUE( cxx.ExpressionValueIs("j_1", 25) );
	EXPECT_TRUE( dbg.StepOut() );
	LogTestEnd( "Add operations to inactive frame: add a new statement" );

	// backup changes and continue debugging
	LogTestBegin( "Add operations to inactive frame: add a new statement: Backup case" );
	EXPECT_TRUE( ModifyCode("int j_1 = 5;",
		"int j_1 = 5;", 2) );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	LogTestEnd( "Add operations to inactive frame: add a new statement: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CAddingCodeTest::AddLoop( )
{
// 	LogTestHeader( "AddLoop" );
	CStartEndTest se_test("AddLoop", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Add a for_loop to inactive frame" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");

//	EXPECT_TRUE( ModifyCodeLine("Enum_local = one;",
//		"Enum_local = one;  for(Int=0; Int<5; Int++) { Union_local.Long += Int; };") );
	EXPECT_TRUE( ModifyCode("Enum_local = one;",
		"Enum_local = one;{Enter}  for(Int=0; Int<5; Int++){Enter} { Union_local.Int += Int; };",
		1) );
	EXPECT_TRUE( src.Find("if (Flag)") );
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	EXPECT_TRUE( dbg.CurrentLineIs( "if (Flag)" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("Union_local.Long", 1009) );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main_Calls") );
	bps.ClearAllBreakpoints();
	LogTestEnd( "Add a for_loop to inactive frame" );

	// backup changes and continue debugging
	LogTestBegin( "Add a for_loop to inactive frame: Backup case" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
//	EXPECT_TRUE( ModifyCodeLine("Enum_local = one;  for(Int=0; Int<5; Int++) { Union_local.Long += Int; };",
//		"Enum_local = one;") );
	EXPECT_TRUE( ModifyCode("Enum_local = one;",
		"Enum_local = one;", 3) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
	LogTestEnd( "Add a for_loop to inactive frame: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}


