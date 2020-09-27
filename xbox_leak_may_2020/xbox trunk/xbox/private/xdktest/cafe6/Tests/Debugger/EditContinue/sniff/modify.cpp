////////////////////////////////////////////////////////////////////////////
//	Modify.CPP
//											 
//	Created by :			
//		VCBU QA		

#include <process.h>
#include "stdafx.h"
#include "Modify.h"	

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
//IMPLEMENT_TEST(CModifyingCodeTest, CECTestSet, "EC Sniff ModifyingCode", -1, CSniffSubSuite)
IMPLEMENT_DYNAMIC(CModifyingCodeTest, CECTestSet)
IMPLEMENT_TEST(CModifyingCodeTestEXE, CModifyingCodeTest, "EC Sniff ModifyingCode EXE", -1, CSniffSubSuite)
IMPLEMENT_TEST(CModifyingCodeTestEXT, CModifyingCodeTest, "EC Sniff ModifyingCode EXT", -1, CSniffSubSuite)
IMPLEMENT_TEST(CModifyingCodeTestINT, CModifyingCodeTest, "EC Sniff ModifyingCode INT", -1, CSniffSubSuite)

CModifyingCodeTest::CModifyingCodeTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: CECTestSet(pSubSuite, szName, nExpectedCompares, szListFilename)
{
}
												 
void CModifyingCodeTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CModifyingCodeTest::Run()
{	 
	XSAFETY;
	EXPECT_TRUE( dbg.ToggleEditAndContinue(TOGGLE_ON) );
/*	XSAFETY;
	BOOL fSetProject;

	m_strProjectName = "ecconsol";	//ToDo: We should get the name from file with all languages strings
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjectName + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;

	if( !(GetSubSuite()->fProjBuilt) )
	{
		RemoveTestFiles( m_strProjectLoc );
		CreateDirectory(m_strProjectLoc, 0);          
		EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );

		fSetProject = SetProject(szProjPathName);
//		fSetProject = SetProject("ecconsol\\ecconsol");
		if( fSetProject )
			GetSubSuite()->fProjBuilt = TRUE;
	}
	else
	{
//		fSetProject = PrepareProject("ecconsol\\ecconsol");
		fSetProject = PrepareProject(szProjPathName);
	}
//	if( SetProject("ecconsol\\ecconsol"))
	if( fSetProject )
	{	  
*/		XSAFETY;
		ModifyFunctionDeclaration();	 // 5 cases
		XSAFETY;
		ModifyFunctionName();			 //  2 cases
		XSAFETY;
		ModifyFunctionDefinition();	  //2 cases: #13137 - fixed
		XSAFETY;
		ModifyMemberFunctionDeclaration();	// 6 cases: #13174 - fixed
		XSAFETY;

		ReplaceFunctionCall();	//6 cases: #9485, #13187, #17148 - fixed.	 
		XSAFETY;
		ReplaceMemberFunctionCall();   //4 cases: #17162 - fixed
		XSAFETY;
		ModifyFunctionCall();		   //8 cases: #17148, #13187, #13568 - fixed
		XSAFETY;
		ModifyMemberFunctionCall();	  //6 cases: #9241, #13137 - fixed
		XSAFETY;
		ModifyGlobal();	 //14 cases #9485 - fixed
		XSAFETY;
		ModifyDataMember();	 //12 cases
		XSAFETY;
		ModifyStaticVar();	// 18 cases: //# 13530, #9744 - fixed
		XSAFETY;
		ModifyLocalVar(); //7 cases: #17148, #13137, #13652, #9851 - fixed
		XSAFETY;
		ModifyLocalFunctionPointer();	//6 cases: //#13568 - fixed
		XSAFETY;
		ModifyLocalPointer();	//4 cases: #13752 -fixed
		XSAFETY;
		ModifyLocalObject();  // 7 cases: //#13772 - fixed
		XSAFETY;
		ModifyOperations();	  //  6 cases: #13137, 13568 - fixed
		XSAFETY;
		ModifyLoop();	  // 6 cases: #13808 - fixed
		XSAFETY;

//	}
		
	COWorkSpace ws;
	ws.CloseAllWindows();
	XSAFETY;
}

void CModifyingCodeTestEXE::Run()
{	 
	XSAFETY;

	m_strProjectName = "ecconsol";	//ToDo: We should get the name from file with all languages strings
	m_strSrcDir = "ecapp";	//ToDo: We should get the name from file with all languages strings
//	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
//	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strProjectName + "\\";
//	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strSrcDir + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;

	RemoveTestFiles( m_strProjectLoc );
	CreateDirectory(m_strProjectLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );

//	if( SetProject("ecconsol\\ecconsol"))
	if( SetProject(szProjPathName) )
	{
		CModifyingCodeTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
		
}

void CModifyingCodeTestEXT::Run()
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
//		EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
		proj.SetExeForDebugSession(m_strProjectName + ".exe");
		CModifyingCodeTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}

void CModifyingCodeTestINT::Run()
{	 
	XSAFETY;

	m_strProjectName = "ecmfcapp";	//ToDo: We should get the name from file with all languages strings
	m_strSrcDir = "ecapp";	//ToDo: We should get the name from file with all languages strings
	m_strProjectLoc  = m_strCWD + "apps\\" + m_strProjectName + "\\";
	CString szSRCFileLoc	= m_strCWD + "src\\" + m_strSrcDir + "\\";
	CString szProjPathName	= (CString)("..\\apps\\") + m_strProjectName + "\\" + m_strProjectName;
/*
	RemoveTestFiles( szProjResLoc );
	RemoveTestFiles( m_strProjLoc );

	CreateDirectory(m_strProjLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjLoc ) );
	CreateDirectory(szProjResLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szResFileLoc, szProjResLoc ) );
*/
	RemoveTestFiles( m_strProjectLoc + "res\\");
	RemoveTestFiles( m_strProjectLoc );
	CreateDirectory(m_strProjectLoc, 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc, m_strProjectLoc ) );
	CreateDirectory(m_strProjectLoc + "res\\", 0);          
	EXPECT_TRUE( CopyProjectSources( szSRCFileLoc + "res\\", m_strProjectLoc + "res\\" ) );

	int iWaitForBuild = 3;	// Minutes to wait for the build to complete
	if( PrepareProject(szProjPathName) && VERIFY_TEST_SUCCESS( proj.Build(iWaitForBuild)))
	{
//		EXPECT_TRUE( VERIFY_TEST_SUCCESS( prj.Build(iWaitForBuild)));
		proj.SetExeForDebugSession(m_strProjectName + ".exe");
		CModifyingCodeTest::Run();
	}

	dbg.StopDebugging();
	proj.Close();
//	RemoveTestFiles( m_strProjectLoc );
}


///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CModifyingCodeTest::ModifyFunctionDeclaration( )
{
// 	LogTestHeader( "ModifyFunctionDeclaration" );
	CStartEndTest se_test("ModifyFunctionDeclaration", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

 	LogTestBegin( "Modify function's return type" );
	// Modify return type of "func_1param" declaration in testg.cpp
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("int func_1param(int);", "long func_1param(int);") );
	EXPECT_TRUE( src.Find("int func_1param(int Parameter)") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
//	if( WaitMsgBoxText("Do you want to save", 10) )
//		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "Cancel") );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// CString str = UIWB.GetStatusText();	// doesn't work now
	// Output window: overloaded function differs only by return type
	EXPECT_TRUE( VerifyBuildString( "overloaded function differs only by return type" ) >= 0);

	// Modify the definition to get in ssync with declaration
	EXPECT_TRUE( ModifyCodeLine("int func_1param(int Parameter)", "long func_1param(int Parameter)") );
	EXPECT_TRUE( src.Find("long func_1param(int Parameter)") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	// We should handle a dialog "One or more files are out of date..." and click "Cancel".
//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue; build required" ) >= 0);
 	LogTestEnd( "Modify function's return type" );

	// Backup previuos changes
	EXPECT_TRUE( ModifyCodeLine("long func_1param(int);", "int func_1param(int);") );
	EXPECT_TRUE( ModifyCodeLine("long func_1param(int Parameter)", "int func_1param(int Parameter)") );

 	LogTestBegin( "Modify function's parameter type" );
//	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("long func_2param(int, long);", "long func_2param(long, long);") );
	EXPECT_TRUE( ModifyCodeLine("long func_2param(int intParam, long longParam)", "long func_2param(long intParam, long longParam)") );
	EXPECT_TRUE( src.Find("long func_2param(long intParam, long longParam)") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue; build required" ) >= 0);
 	LogTestEnd( "Modify function's parameter type" );

	// Backup changes
	EXPECT_TRUE( ModifyCodeLine("long func_2param(long, long);", "long func_2param(int, long);") );
	EXPECT_TRUE( ModifyCodeLine("long func_2param(long intParam, long longParam)", "long func_2param(int intParam, long longParam)") );

	LogTestBegin( "Add function's parameter" );
//	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("void nest_func();", "void nest_func(int);") );
	EXPECT_TRUE( ModifyCodeLine("void nest_func() {", "void nest_func(int par) {") );
	EXPECT_TRUE( ModifyCodeLine("nest_func(); // nest_func call line", "nest_func(5); // nest_func call line") );
	EXPECT_TRUE( src.Find("void nest_func(int par) {") );
//	dbg.StepToCursor(0);
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  // Better output should come, then we'll change it
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: void __cdecl nest_func(int)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl TestG(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image: void __cdecl nest_func(int)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing: nest_func" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	bps.ClearAllBreakpoints();
	LogTestEnd( "Add function's parameter" );

	// Backup changes and continue
	LogTestBegin( "Backup all changes and continue" );
	EXPECT_TRUE( ModifyCodeLine("void nest_func(int);", "void nest_func();") );
	EXPECT_TRUE( ModifyCodeLine("void nest_func(int par) {", "void nest_func() {") );
	EXPECT_TRUE( ModifyCodeLine("nest_func(5); // nest_func call line", "nest_func(); // nest_func call line") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
/*	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
*/	// Verify EC success (was: fails gracefully ) (uses a known return type)
//	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( !IsMsgBox("One or more errors occurred", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
	LogTestEnd( "Backup all changes and continue" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyFunctionName( )
{
// 	LogTestHeader( "ModifyFunctionName" );
	CStartEndTest se_test("ModifyFunctionName", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

 	LogTestBegin( "Modify function's name" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("int recur_func();", "int recur_func2();") );
	EXPECT_TRUE( ModifyCodeLine("int recur_func() {", "int recur_func2() {") );
	EXPECT_TRUE( ModifyCodeLine("recur_func(); // call line", "recur_func2(); // call line") );
	EXPECT_TRUE( ModifyCodeLine("Int = recur_func();", "Int = recur_func2();") );
	EXPECT_TRUE( src.Find("int recur_func2() {") );
//	dbg.StepToCursor(0);
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC fails gracefully 
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
//	EXPECT_TRUE( dbg.CurrentLineIs( "int recur_func2() {" ) );
	bps.ClearAllBreakpoints();
//	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "TestG") );
 	LogTestEnd( "Modify function's name" );

	// Backup changes and continue
 	LogTestBegin( "Modify function's name: Backup case" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("int recur_func2();", "int recur_func();") );
	EXPECT_TRUE( ModifyCodeLine("recur_func2(); // call line", "recur_func(); // call line") );
	EXPECT_TRUE( ModifyCodeLine("Int = recur_func2();", "Int = recur_func();") );
	EXPECT_TRUE( ModifyCodeLine("int recur_func2() {", "int recur_func() {") );
//	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success (was: fails gracefully ) (uses a known return type)
//	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( !IsMsgBox("One or more errors occurred", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
	else
	{
		m_pLog->RecordInfo("Message box 'Could not perform a complete stack walk' didn't appear");
	}
*/	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
 	LogTestEnd( "Modify function's name: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyFunctionDefinition( )
{
// 	LogTestHeader( "ModifyFunctionDefinition" );
	CStartEndTest se_test("ModifyFunctionDefinition", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

	LogTestBegin( "Modify function's parameter name" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("double func_3param(int intParam, long longParam, float floatParam)", "double func_3param(int intParam, long longParam, float realParam)") );
	EXPECT_TRUE( ModifyCodeLine("double d_3p = intParam + longParam + floatParam;", "double d_3p = intParam + longParam + realParam;") );
//	EXPECT_TRUE( src.Find("double func_3param(int intParam, long longParam, float realParam)") );
	EXPECT_TRUE( src.Find("double d_3p = intParam + longParam + realParam;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
//	if( WaitMsgBoxText("Do you want to save", 10) )
//		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
	else
	{
		m_pLog->RecordInfo("Message box 'Could not perform a complete stack walk' didn't appear");
	}
*/	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Verify EC success (inactive frame)
	EXPECT_TRUE( dbg.CurrentLineIs( "double d_3p = intParam + longParam + realParam;" ) );
	LogTestEnd( "Modify function's parameter name" );

	// Backup changes and continue
	LogTestBegin( "Modify function's parameter name: Backup case" );
	EXPECT_TRUE( ModifyCodeLine("double func_3param(int intParam, long longParam, float realParam)", "double func_3param(int intParam, long longParam, float floatParam)") );
	EXPECT_TRUE( ModifyCodeLine("double d_3p = intParam + longParam + realParam;", "double d_3p = intParam + longParam + floatParam;") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Verify EC success (active frame)
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	LogTestEnd( "Modify function's parameter name: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}


BOOL CModifyingCodeTest::ModifyMemberFunctionDeclaration( )
{
 	//LogTestHeader( "ModifyMemberFunctionDeclaration" );
	CStartEndTest se_test("ModifyMemberFunctionDeclaration", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("fooclass foo1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "fooclass foo1;") );

 	LogTestBegin( "Modify static member function return type" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.h");
	EXPECT_TRUE( ModifyCodeLine("static int static_func() { return 9; };", "static __int64 static_func() { return 9; };") );
	EXPECT_TRUE( src.Find("static __int64 static_func() { return 9; };") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
//	if( WaitMsgBoxText("Do you want to save", 10) )
//		MST.WButtonClick( "No" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "Cancel") );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx", FALSE ) < 0);  
 	LogTestEnd( "Modify static member function return type" );

	// Backup changes
	EXPECT_TRUE( ModifyCodeLine("static __int64 static_func() { return 9; };", "static int static_func() { return 9; };") );

 	LogTestBegin( "Modify inline member function name" );
//	UIWB.OpenFile( m_strProjectLoc + "cxx.h");
	EXPECT_TRUE( ModifyCodeLine("inline void inline_func()", "inline void inline_func2()") );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("foo1.inline_func();", "foo1.inline_func2();") );
	UIWB.OpenFile( m_strProjectLoc + "cxx.h");
	EXPECT_TRUE( src.Find("inline void inline_func2()") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
 	LogTestEnd( "Modify inline member function name" );

	// Backup changes
	EXPECT_TRUE( ModifyCodeLine("inline void inline_func2()", "inline void inline_func()") );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("foo1.inline_func2();", "foo1.inline_func();") );

 	LogTestBegin( "Modify overloaded member function" );
//	UIWB.OpenFile( m_strProjectLoc + "include.cpp");
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("int overloaded_func(int i){return i;}", "int overloaded_func(int i, long l){return i+l;}") );
	EXPECT_TRUE( ModifyCodeLine("pb->overloaded_func(1);", "pb->overloaded_func(1,22);") );
	EXPECT_TRUE( src.Find("int overloaded_func(int i, long l){return i+l;}") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0); 
 	LogTestEnd( "Modify overloaded member function" );

	// Backup changes
	EXPECT_TRUE( ModifyCodeLine("int overloaded_func(int i, long l){return i+l;}", "int overloaded_func(int i){return i;}") );
	EXPECT_TRUE( ModifyCodeLine("pb->overloaded_func(1,22);", "pb->overloaded_func(1);") );

 	LogTestBegin( "Modify virtual member function's parameter" );
//	UIWB.OpenFile( m_strProjectLoc + "include.h");
	UIWB.OpenFile( m_strProjectLoc + "cxx.h");
	EXPECT_TRUE( ModifyCodeLine("virtual int base1_virtual_func(int i); // base1", "virtual int base1_virtual_func(long i); // base1") );
	EXPECT_TRUE( ModifyCodeLine("virtual int base1_virtual_func(int i); // derivedm", "virtual int base1_virtual_func(long i); // derivedm") );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("int base1::base1_virtual_func(int i)", "int base1::base1_virtual_func(long i)") );
	EXPECT_TRUE( ModifyCodeLine("int derivedm::base1_virtual_func(int i)", "int derivedm::base1_virtual_func(long i)") );
	EXPECT_TRUE( src.Find("int derivedm::base1_virtual_func(long i)") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0); 
	EXPECT_TRUE( VerifyBuildString( "not supported by Edit and Continue" ) >= 0); 
	// Backup changes
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("int base1::base1_virtual_func(long i)", "int base1::base1_virtual_func(int i)") );
	EXPECT_TRUE( ModifyCodeLine("int derivedm::base1_virtual_func(long i)", "int derivedm::base1_virtual_func(int i)") );
	UIWB.OpenFile( m_strProjectLoc + "cxx.h");
	EXPECT_TRUE( ModifyCodeLine("virtual int base1_virtual_func(long i); // base1", "virtual int base1_virtual_func(int i); // base1") );
	EXPECT_TRUE( ModifyCodeLine("virtual int base1_virtual_func(long i); // derivedm", "virtual int base1_virtual_func(int i); // derivedm") );
 	LogTestEnd( "Modify virtual member function's parameter" );

 	LogTestBegin( "Modify overridden member function" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.h");
	EXPECT_TRUE( ModifyCodeLine("int base1_overridden_func(int i); // base1", "long base1_overridden_func(long l); // base1") );
	EXPECT_TRUE( ModifyCodeLine("int base1_overridden_func(int i); // derivedm", "long base1_overridden_func(long l); // derivedm") );
//	UIWB.OpenFile( m_strProjectLoc + "include.cpp");
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("int base1::base1_overridden_func(int i)", "long base1::base1_overridden_func(long l)") );
	EXPECT_TRUE( ModifyCodeLine("int derivedm::base1_overridden_func(int i)", "long derivedm::base1_overridden_func(long l)") );
	EXPECT_TRUE( src.Find("long derivedm::base1_overridden_func(long l)") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
//	EXPECT_TRUE( IsMsgBox("One or more files are out of date", "Cancel") );
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "not supported by Edit and Continue" ) >= 0);  
 	LogTestEnd( "Modify overridden member function" );

	// Backup changes and continue
 	LogTestBegin( "Backup changes and continue" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("long base1::base1_overridden_func(long l)", "int base1::base1_overridden_func(int i)") );
	EXPECT_TRUE( ModifyCodeLine("long derivedm::base1_overridden_func(long l)", "int derivedm::base1_overridden_func(int i)") );
	UIWB.OpenFile( m_strProjectLoc + "cxx.h");
	EXPECT_TRUE( ModifyCodeLine("long base1_overridden_func(long l); // base1", "int base1_overridden_func(int i); // base1") );
	EXPECT_TRUE( ModifyCodeLine("long base1_overridden_func(long l); // derivedm", "int base1_overridden_func(int i); // derivedm") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
 	LogTestEnd( "Backup changes and continue" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ReplaceFunctionCall( )
{
// 	LogTestHeader( "ReplaceFunctionCall" );
	CStartEndTest se_test("ReplaceFunctionCall", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

 	LogTestBegin( "Replace a function call in non active frame" );
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("Int = func_1param(Local);", "Int = foo_Include(Local);") );
	EXPECT_TRUE( src.Find("Int = foo_Include(Local);") );
//	dbg.StepToCursor(0);
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
//	if( WaitMsgBoxText("Do you want to save", 10) )
//		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "Int = foo_Include(Local);" ) );
	bps.ClearAllBreakpoints();
 	LogTestEnd( "Replace a function call in non active frame" );

 	LogTestBegin( "Replace a function call in active frame After IP" );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG")	);
	EXPECT_TRUE( ModifyCodeLine("Int = foo_Include(i);", "Int = recur_func(); // replacement") );
	EXPECT_TRUE( src.Find("Double = func_3param(Int, Long, Real);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "Double = func_3param(Int, Long, Real);" ) );
 	LogTestEnd( "Replace a function call in active frame After IP" );

 	LogTestBegin( "Replace a function call in active frame Before IP" );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG")	);
	EXPECT_TRUE( ModifyCodeLine("Long = func_2param(Int, Long);", "Long = func_1param(Local);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);  //It should execute a cycle in the loop back to the same line
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "Long = func_1param(Local);" ) );
 	LogTestEnd( "Replace a function call in active frame Before IP" );

 	LogTestBegin( "Replace a function call in active frame aT IP" );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG")	);
	EXPECT_TRUE( ModifyCodeLine("Double = func_3param(Int, Long, Real);", "Double = func_2param(Int, Long);") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);  
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("func_1param") );
 	LogTestEnd( "Replace a function call in active frame aT IP" );

	// #17148   // ToDo: do one more stepout to get rid of DAM: change the verification part
 	LogTestBegin( "Replace a function call in non-top active frame After IP" );
	EXPECT_TRUE( stk.FunctionIs("TestG",1)	);
	EXPECT_TRUE( ModifyCodeLine("Int = recur_func(); // initial call", "Int = check_func(Int,20);") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);  
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
//8044	EXPECT_TRUE( IsMsgBox("Edit and Continue has applied", "OK") );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "OK") );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "Yes") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG")	);
//	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);  
//	if( WaitMsgBoxText("The function on top of the call stack has been updated", 10) )
//		MST.WButtonClick( "OK" );
//	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	dbg.StopDebugging();
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( src.Find("Int = check_func(Int,20);") );
	dbg.StepToCursor(0);
	dbg.StepInto();  
	EXPECT_TRUE( stk.CurrentFunctionIs("check_func") );
 	LogTestEnd( "Replace a function call in non-top active frame After IP" );

	// Backup changes in testg.cpp
 	LogTestBegin( "Backup changes in testg.cpp" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("Int = foo_Include(Local);", "Int = func_1param(Local);") );
	EXPECT_TRUE( ModifyCodeLine("Int = recur_func(); // replacement", "Int = foo_Include(i);") );
	EXPECT_TRUE( ModifyCodeLine("Long = func_1param(Local);", "Long = func_2param(Int, Long);") );
	EXPECT_TRUE( ModifyCodeLine("Double = func_2param(Int, Long);", "Double = func_3param(Int, Long, Real);") );
	EXPECT_TRUE( ModifyCodeLine("Int = check_func(Int,20);", "Int = recur_func(); // initial call") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);  
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success  ????
	EXPECT_TRUE( IsMsgBox("Edit and Continue has applied", "OK") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG")	);
 	LogTestEnd( "Backup changes in testg.cpp" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyFunctionCall( )
{
// 	LogTestHeader( "ModifyFunctionCall" );
	CStartEndTest se_test("ModifyFunctionCall", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "calls.cpp");
	EXPECT_TRUE( src.Find("iCalls = TestG();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "iCalls = TestG();") );

 	LogTestBegin( "Modify function call's parameters (var to value) in non active frame" );
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("Int = func_1param(Local);", "Int = func_1param(5);") );
	EXPECT_TRUE( src.Find("Int = func_1param(5);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "Int = func_1param(5);" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Long = func_2param(Int, Long);") );
 	LogTestEnd( "Modify function call's parameters (var to value) in non active frame" );
	
 	LogTestBegin( "Modify function call's parameters in active frame aT IP" );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Double = func_3param(Int, Long, Real);") );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	EXPECT_TRUE( ModifyCodeLine("Double = func_3param(Int, Long, Real);", "Double = func_3param(5,999,1.5);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.CurrentLineIs( "Double = func_3param(5,999,1.5);" ) );
 	LogTestEnd( "Modify function call's parameters in active frame aT IP" );

	// change some params back to vars
 	LogTestBegin( "change some params back to vars" );
	EXPECT_TRUE( ModifyCodeLine("Double = func_3param(5,999,1.5);", "Double = func_3param(Int,555,Real);") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "Ldouble = func_3param(func_1param(Local), func_2param(Int, Long), Real);" ) );
 	LogTestEnd( "change some params back to vars" );

 	LogTestBegin( "Modify function call's parameters in active frame Before IP" );
	EXPECT_TRUE( dbg.StepOver(2, NULL, "Int = recur_func();") );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	EXPECT_TRUE( ModifyCodeLine("Int = foo_Include(i);", "Int = foo_Include(7);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.CurrentLineIs( "Int = foo_Include(7);" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Int = recur_func();") );
 	LogTestEnd( "Modify function call's parameters in active frame Before IP" );
	
	// change param back to var
 	LogTestBegin( "change param back to var" );
	EXPECT_TRUE( ModifyCodeLine("Int = foo_Include(7);", "Int = foo_Include(Int);") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "nest_func(); // nest_func call line" ) );
	dbg.StepToCursor(0);  // execute the changed call through loop cycle
	EXPECT_TRUE( dbg.CurrentLineIs( "nest_func(); // nest_func call line" ) );
 	LogTestEnd( "change param back to var" );

 	LogTestBegin( "Modify function call's parameters in non-top active frame Before IP" );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "nest_func") );
	EXPECT_TRUE( stk.FunctionIs("TestG",1)	);
	EXPECT_TRUE( ModifyCodeLine("Int = func_1param(5);", "Int = func_1param(recur_func());") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);  
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
//8044	EXPECT_TRUE( IsMsgBox("Edit and Continue has applied", "OK") );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "Yes") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG")	);
	dbg.StopDebugging();
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
/*	EXPECT_TRUE( src.Find("Int = func_1param(recur_func());") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("The function on top of the call stack has been updated", 10) )
		MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
*/	EXPECT_TRUE( src.Find("Int = func_1param(recur_func());") );
	dbg.StepToCursor(0);
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Long = func_2param(Int, Long);") );
	EXPECT_TRUE( cxx.ExpressionValueIs("Int", 9) );
 	LogTestEnd( "Modify function call's parameters in non-top active frame Before IP" );

 	LogTestBegin( "Modify function call's parameters in active frame After IP" );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	EXPECT_TRUE( ModifyCodeLine("Int=0;  Int=check_func(Int,90);", "Int=0;  Int=check_func(90,Int);") );
	EXPECT_TRUE( src.Find("Int=0;  Int=check_func(90,Int);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "Int=check_func(90,Int);" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Void = (void*)Int;") );
 	LogTestEnd( "Modify function call's parameters in active frame After IP" );

	// Backup changes in testg.cpp
 	LogTestBegin( "Backup changes in testg.cpp" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("Double = func_3param(Int,555,Real);", "Double = func_3param(Int, Long, Real);") );
	EXPECT_TRUE( ModifyCodeLine("Int = foo_Include(Int);", "Int = foo_Include(i);") );
	EXPECT_TRUE( ModifyCodeLine("Int = func_1param(recur_func());", "Int = func_1param(Local);") );
	EXPECT_TRUE( ModifyCodeLine("Int=0;  Int=check_func(90,Int);", "Int=0;  Int=check_func(Int,90);") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( stk.CurrentFunctionIs("main_Calls") );
 	LogTestEnd( "Backup changes in testg.cpp" );

	dbg.StopDebugging();
	
	return (TRUE);
}

BOOL CModifyingCodeTest::ReplaceMemberFunctionCall( )
{
// 	LogTestHeader( "ReplaceMemberFunctionCall" );
	CStartEndTest se_test("ReplaceMemberFunctionCall", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "foo.c");
	EXPECT_TRUE( src.Find("for(i=0; i<9; i++)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "for(i=0; i<9; i++)") );
	EXPECT_TRUE( stk.CurrentFunctionIs("OtherMain") );

 	LogTestBegin( "Replace a member function call in non active frame" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("Result=fooclass::static_func();", "Result=foo1.chain2_func();") );
	EXPECT_TRUE( src.Find("Result=foo1.chain2_func();") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
	EXPECT_TRUE( dbg.CurrentLineIs( "Result=foo1.chain2_func();" ) );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "fooclass::chain2_func") );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "Cxx_Tests") );
 	LogTestEnd( "Replace a member function call in non active frame" );

 	LogTestBegin( "Replace a member function call in active frame After IP" );
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests")	);
	EXPECT_TRUE( ModifyCodeLine("foo1.inline_func();", "foo1.chain1_func(); // replacement") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
/*	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
*/	// Verify EC success
//	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "Cancel") );
	if( WaitMsgBoxText("Point of execution moved", 10) )
		MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests")	);
	dbg.StepOver(3);  // Do extra step since the previous one was ignored by E&C
//8044	EXPECT_TRUE( dbg.CurrentLineIs( "Result=foo1;" ) );
	dbg.StepOver();
	EXPECT_TRUE( cxx.ExpressionValueIs("Result", 0) );
 	LogTestEnd( "Replace a member function call in active frame After IP" );

 	LogTestBegin( "Replace a member function call in active frame Before IP" );
	EXPECT_TRUE( ModifyCodeLine("Result=foo1.getID(); // getID", "Result=foo1.getCount(); // getID") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);  //It should execute a cycle in the loop back to the same line  
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.StepOver(2, NULL, "Result=foo1;") );
 	LogTestEnd( "Replace a member function call in active frame Before IP" );

	// Backup all changes in cxx.cxx  and continue
 	LogTestBegin( "Backup all changes in cxx.cxx  and continue" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("Result=foo1.chain2_func();", "Result=fooclass::static_func();") );
	EXPECT_TRUE( ModifyCodeLine("foo1.chain1_func();", "foo1.inline_func();") );
	EXPECT_TRUE( ModifyCodeLine("Result=foo1.getCount(); // getID", "Result=foo1.getID(); // getID") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);  //It should execute a cycle in the loop back to the same line  
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
 	LogTestEnd( "Backup all changes in cxx.cxx  and continue" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyMemberFunctionCall( )
{
//	LogTestHeader( "ModifyMemberFunctionCall" );
	CStartEndTest se_test("ModifyMemberFunctionCall", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("int test_include = Test_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int test_include = Test_func();") );
	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests") );

 	LogTestBegin( "Modify member function call's parameters in non active frame" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("b2.base2_virtual_func(1);", "b2.base2_virtual_func(ib1);") );
	EXPECT_TRUE( src.Find("b2.base2_virtual_func(ib1);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);    
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
/*	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "Cancel") );
	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "b2.base2_virtual_func(ib1);" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "int iderived = drvm.derivedm_public_func(1+2);") );
 	LogTestEnd( "Modify member function call's parameters in non active frame" );

	LogTestBegin( "Modify member function call's parameters in active frame After IP" );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func2") );
	EXPECT_TRUE( ModifyCodeLine("iderived = drvm.base1_overloaded_func('C');", "iderived = drvm.base1_overloaded_func((char)ib2);") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);    
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("derivedm::derivedm_public_func") );
	LogTestEnd( "Modify member function call's parameters in active frame After IP" );
	
 	LogTestBegin( "Modify member function call's parameters in non-top active frame Before IP" );
//	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "nest_func") );
	EXPECT_TRUE( stk.FunctionIs("Test_func2",1)	);
	EXPECT_TRUE( ModifyCodeLine("ib1 = b1.base1_overridden_func(1);", "ib1 = b1.base1_overridden_func(8);") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);    
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "OK") );
//	EXPECT_TRUE( IsMsgBox("Edit and Continue", "OK") );
	if( WaitMsgBoxText("Edit and Continue", 10) )
		MST.WButtonClick( "OK" );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func2")	);
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("ib1 = b1.base1_overridden_func(8);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("The function on top of the call stack has been updated", 10) )
		MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( src.Find("ib1 = b1.base1_overridden_func(8);") );
	dbg.StepToCursor(0);
	EXPECT_TRUE( dbg.StepOver(1, NULL, "ib1 = b1.base1_virtual_func(1);") );
 	LogTestEnd( "Modify member function call's parameters in non-top active frame Before IP" );

 	LogTestBegin( "Modify member function call's parameters in active frame Before IP" );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "Test_func") );
	EXPECT_TRUE( ModifyCodeLine("b.virtual_func(0);", "b.virtual_func(::global);") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);    
/*	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "Cancel") );
	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( src.Find("b.virtual_func(::global);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "b.virtual_func(::global);") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "base *pb1 = new base;") );
 	LogTestEnd( "Modify member function call's parameters in active frame Before IP" );
	
 	LogTestBegin( "Modify member function call's parameters in active frame aT IP" );
	EXPECT_TRUE( src.Find("b.virtual_func(1);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "b.virtual_func(1);") );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
	EXPECT_TRUE( ModifyCodeLine("b.virtual_func(1);", "b.virtual_func(b.m_public);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);    
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "Cancel") );
	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.CurrentLineIs( "b.virtual_func(b.m_public);" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "::global = 1;") );
 	LogTestEnd( "Modify member function call's parameters in active frame aT IP" );

	// Backup changes in cxx.cxx
 	LogTestBegin( "Backup changes in cxx.cxx" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("b2.base2_virtual_func(ib1);", "b2.base2_virtual_func(1);") );
	EXPECT_TRUE( ModifyCodeLine("iderived = drvm.base1_overloaded_func((char)ib2);", "iderived = drvm.base1_overloaded_func('C');") );
	EXPECT_TRUE( ModifyCodeLine("ib1 = b1.base1_overridden_func(8);", "ib1 = b1.base1_overridden_func(1);") );
	EXPECT_TRUE( ModifyCodeLine("b.virtual_func(::global);", "b.virtual_func(0);") );
	EXPECT_TRUE( ModifyCodeLine("b.virtual_func(b.m_public);", "b.virtual_func(1);") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);    
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	// Verify EC success
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
 	LogTestEnd( "Backup changes in cxx.cxx" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyGlobal( )
{
// 	LogTestHeader( "ModifyGlobal" );
	CStartEndTest se_test("ModifyGlobal", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	dbg.StepInto();
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");

	LogTestBegin( "Modify Global's type" );
	EXPECT_TRUE( ModifyCodeLine("int global = 0;", "long global = 0;") );
	EXPECT_TRUE( src.Find("base b = func_class();") );
//	dbg.StepToCursor(0);
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	dbg.Go(NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
	bps.ClearAllBreakpoints();
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Global's type" );

	// Backup the change
	LogTestBegin( "Modify Global's type: Backup case" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");  // extra
	EXPECT_TRUE( ModifyCodeLine("long global = 0;", "int global = 0;") );
// it hangs under CAFE	 #9485
	EXPECT_TRUE( src.Find("base b = func_class();") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
	LogTestEnd( "Modify Global's type: Backup case" );

	LogTestBegin( "Modify Global's type to pointer" );
	EXPECT_TRUE( ModifyCodeLine("int global = 0;", "int * global = (int *)0;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "cannot convert" ) >= 0);  
	LogTestEnd( "Modify Global's type to pointer" );

	// Backup the change
	LogTestBegin( "Modify Global's type to pointer: Backup case" );
	EXPECT_TRUE( ModifyCodeLine("int * global = (int *)0;", "int global = 0;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
	LogTestEnd( "Modify Global's type to pointer: Backup case" );

	LogTestBegin( "Modify Global's initialization value" );
	EXPECT_TRUE( ModifyCodeLine("int result = 0;", "int result = 99;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Global's initialization value" );

	// Backup the change
	LogTestBegin( "Modify Global's initialization value: Backup case" );
	EXPECT_TRUE( ModifyCodeLine("int result = 99;", "int result = 0;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
	LogTestEnd( "Modify Global's initialization value: Backup case" );

	LogTestBegin( "Modify Global's initialization value: Nothing to Var" );
	EXPECT_TRUE( ModifyCodeLine("int iGlobal;", "int iGlobal = result;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Global's initialization value: Nothing to Var" );

	// Backup the change
	LogTestBegin( "Modify Global's initialization value: Var to Nothing: Backup case" );
	EXPECT_TRUE( ModifyCodeLine("int iGlobal = result;", "int iGlobal;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
	LogTestEnd( "Modify Global's initialization value: Var to Nothing: Backup case" );

	LogTestBegin( "Modify Global's initialization value: Nothing to Number" );
	EXPECT_TRUE( ModifyCodeLine("int iGlobal;", "int iGlobal = 5;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// Status bar: Could not complete "Edit & Continue"
	// Output window: Edit not supported by Edit and Continue; build required
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Global's initialization value: Nothing to Number" );

	// Backup the change
	LogTestBegin( "Modify Global's initialization value: Number to Nothing: Backup case" );
	EXPECT_TRUE( ModifyCodeLine("int iGlobal = 5;", "int iGlobal;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
	LogTestEnd( "Modify Global's initialization value: Number to Nothing: Backup case" );

	LogTestBegin( "Modify Global pointer's type and value" );
	EXPECT_TRUE( ModifyCodeLine("int *piGlobal = &iGlobal;", "long *piGlobal = (long *)&result;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
//	EXPECT_TRUE( VerifyBuildString( "cannot convert" ) >= 0);  
	LogTestEnd( "Modify Global pointer's type and value" );

	// Backup the change
	LogTestBegin( "Modify Global pointer's type and value: Backup case" );
	EXPECT_TRUE( ModifyCodeLine("long *piGlobal = (long *)&result;", "int *piGlobal = &iGlobal;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
	LogTestEnd( "Modify Global pointer's type and value: Backup case" );

	LogTestBegin( "Modify Global's name" );
	EXPECT_TRUE( ModifyCodeLine("int global = 0;", "int Xglobal = 0;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "undeclared identifier" ) >= 0);  
	LogTestEnd( "Modify Global's name" );

	LogTestBegin( "Modify Global's name: Backup case" );
	EXPECT_TRUE( ModifyCodeLine("int Xglobal = 0;", "int global = 0;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
//	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "stack walk" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl Test_func(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Global's name: Backup case" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyDataMember( )
{
// 	LogTestHeader( "ModifyDataMember" );
	CStartEndTest se_test("ModifyDataMember", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("base b = func_class();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "base b = func_class();") );

	LogTestBegin( "Modify Data Member's type: int to long" );
	// Case 1 
	EXPECT_TRUE( ModifyCodeLine("int m_protected;", "long m_protected;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
	LogTestEnd( "Modify Data Member's type: int to long" );

	// Backup the change
	LogTestBegin( "Modify Data Member's type: long to int : Backup case" );
	EXPECT_TRUE( ModifyCodeLine("long m_protected;", "int m_protected;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("func_class") );
	LogTestEnd( "Modify Data Member's type: long to int : Backup case" );

	// Case 2
	LogTestBegin( "Modify Data Member's type: float to double" );
	EXPECT_TRUE( ModifyCodeLine("float f;", "double f;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("func_class") );
	LogTestEnd( "Modify Data Member's type: float to double" );

	// Backup the change
	LogTestBegin( "Modify Data Member's type: Backup case: double to float" );
	EXPECT_TRUE( ModifyCodeLine("double f;", "float f;") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
	LogTestEnd( "Modify Data Member's type: Backup case: double to float" );

	// Case 3
	LogTestBegin( "Modify Data Member's type: char* to char" );
	EXPECT_TRUE( ModifyCodeLine("char *m_pchar;", "char m_pchar;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "not supported by Edit and Continue" ) >= 0);  
	LogTestEnd( "Modify Data Member's type: char* to char" );

	// Backup the change
	LogTestBegin( "Modify Data Member's type: char to char* : Backup" );
	EXPECT_TRUE( ModifyCodeLine("char m_pchar;", "char *m_pchar;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
	LogTestEnd( "Modify Data Member's type: char to char* : Backup" );

	// Case 4
	LogTestBegin( "Modify Data Member's type: static int to long" );
	EXPECT_TRUE( ModifyCodeLine("static int ref_cnt;", "long ref_cnt;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "not supported by Edit and Continue" ) >= 0);  
	LogTestEnd( "Modify Data Member's type: static int to long" );

	// Backup the change
	LogTestBegin( "Modify Data Member's type: long to static int : Backup" );
	EXPECT_TRUE( ModifyCodeLine("long ref_cnt;", "static int ref_cnt;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Test_func") );
	LogTestEnd( "Modify Data Member's type: long to static int : Backup" );

	LogTestBegin( "Modify Data Member's name" );
	// Case 1
	EXPECT_TRUE( ModifyCodeLine("int m_protected;", "int m_prot;") );
	EXPECT_TRUE( src.Find("drv1.virtual_func1();") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "undeclared identifier" ) >= 0);  
	LogTestEnd( "Modify Data Member's name" );

	// Backup the change
	LogTestBegin( "Modify Data Member's name: Backup" );
	EXPECT_TRUE( ModifyCodeLine("int m_prot;", "int m_protected;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Edit and Continue has applied", "OK") );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("derived::derived") );
	LogTestEnd( "Modify Data Member's name: Backup" );

	// Case 2
	LogTestBegin( "Modify Data Member's name: Case 2" );
	EXPECT_TRUE( ModifyCodeLine("int i;", "int i_A;") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "cxx.cxx" ) >= 0);  
//	EXPECT_TRUE( VerifyBuildString( "Edit not supported by Edit and Continue" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "not supported by Edit and Continue" ) >= 0);  
	LogTestEnd( "Modify Data Member's name: Case 2" );

	// Backup the change
	LogTestBegin( "Modify Data Member's name: Case 2: Backup" );
	EXPECT_TRUE( ModifyCodeLine("int i_A;", "int i;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("derived::derived") );
	LogTestEnd( "Modify Data Member's name: Case 2: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyStaticVar( )
{
// 	LogTestHeader( "ModifyStaticVar" );
	CStartEndTest se_test("ModifyStaticVar", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	bps.SetBreakpoint("TestG");
	EXPECT_TRUE( dbg.Go(NULL, NULL, "TestG") );

	LogTestBegin( "Modify Static Var's type" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("static x=1;", "static unsigned long x=1;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl TestG(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Static Var's type" );

	// Backup the change
	LogTestBegin( "Modify Static Var's type: Backup" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("static unsigned long x=1;", "static x=1;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	LogTestEnd( "Modify Static Var's type: Backup" );

	LogTestBegin( "Modify Static Var's Initialization value" );
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( ModifyCodeLine("static x=1;", "static x=6;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl TestG(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Static Var's Initialization value" );

	// Backup the change
	LogTestBegin( "Modify Static Var's Initialization value: Backup" );
	EXPECT_TRUE( ModifyCodeLine("static x=6;", "static x=1;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	LogTestEnd( "Modify Static Var's Initialization value: Backup" );

	LogTestBegin( "Modify Static Var's name" );
	EXPECT_TRUE( ModifyCode("static x=1;",
		"static y=1;{Enter}   y++;{Enter}   if (y<9){Enter}	   recur_func(); // call line{Enter}   return y;",
		5) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: int __cdecl TestG(void)" ) >= 0);
//	EXPECT_TRUE( VerifyBuildString( "testg.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Static Var's name" );

	// Backup the change
	LogTestBegin( "Modify Static Var's name: Backup" );
	EXPECT_TRUE( ModifyCode("static y=1;",
		"static x=1;{Enter}   x++;{Enter}   if (x<9){Enter}	   recur_func(); // call line{Enter}   return x;",
		5) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("TestG") );
	LogTestEnd( "Modify Static Var's name: Backup" );

	dbg.StopDebugging();
	bps.ClearAllBreakpoints();

	// Start debugging
	bps.SetBreakpoint("Based");
	EXPECT_TRUE( dbg.Go(NULL, NULL, "Based") );

	LogTestBegin( "Modify Static Array's type" );
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( ModifyCodeLine("static int rg_static_int[26];", "static char rg_static_int[26];") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "cannot convert" ) >= 0);  
	LogTestEnd( "Modify Static Array's type" );

	// Backup the change
	LogTestBegin( "Modify Static Array's type: Backup" );
	EXPECT_TRUE( ModifyCodeLine("static char rg_static_int[26];", "static int rg_static_int[26];") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Based") );
	LogTestEnd( "Modify Static Array's type: Backup" );

	LogTestBegin( "Modify Static Array's dimension" );
	EXPECT_TRUE( ModifyCodeLine("static int rg_static_int[26];", "static int rg_static_int[10];") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: void __cdecl Based(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Static Array's dimension" );

	// Backup the change: false backup
	LogTestBegin( "Modify Static Array's dimension: Backup" );
	EXPECT_TRUE( ModifyCodeLine("static int rg_static_int[10];", "static int rg_static_int[26];") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Based") );
	LogTestEnd( "Modify Static Array's dimension: Backup" );

	// #13530
	LogTestBegin( "Modify Static Array's name" );
	EXPECT_TRUE( ModifyCode("static int rg_static_int[26];",
		"static int rg_static_intX[26];{Enter}	int *p_static = &rg_static_intX[0];{Enter}	rg_static_intX[2] = 32;{Enter}	return rg_static_intX[2];{Enter}",
		4) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "undeclared identifier" ) >= 0);  
	LogTestEnd( "Modify Static Array's name" );

	// Special case: "Backup" the change
	LogTestBegin( "Special case: False (corrupted) Backup the change" );
	EXPECT_TRUE( ModifyCode("static int rg_static_intX[26];",
		"static int rg_static_int[26];{Enter}	int *p_static = &rg_static_int[0];{Enter}	rg_static_int[2] = 32;{Enter}	return rg_static_int[2];{Enter}",
		4) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Based") );
	LogTestEnd( "Special case: False (corrupted) Backup the change" );

	// Backup the change (real)
	LogTestBegin( "Modify Static Array's name: Real Backup the change" );
	EXPECT_TRUE( ModifyCode("static int rg_static_int[26];",
		"static int rg_static_int[26];{Enter}int *p_2 = &rg_static_int[1];{Enter}int *p_static = &rg_static_int[0];{Enter}rg_static_int[2] = 32;",
		4) );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Based") );
	LogTestEnd( "Modify Static Array's name: Real Backup the change" );

	LogTestBegin( "Modify Static Function Pointer's type" );
//	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( ModifyCodeLine("static int (*pfn100)() = func1;", "static __int64 (*pfn100)() = func1;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "cannot convert" ) >= 0);  
	LogTestEnd( "Modify Static Function Pointer's type" );

	// Backup the change
	LogTestBegin( "Modify Static Function Pointer's type: Backup" );
	EXPECT_TRUE( ModifyCodeLine("static __int64 (*pfn100)() = func1;", "static int (*pfn100)() = func1;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Based") );
	LogTestEnd( "Modify Static Function Pointer's type: Backup" );

	LogTestBegin( "Modify Static Function Pointer's Initialization Value" );
	EXPECT_TRUE( ModifyCodeLine("static int (*pfn100)() = func1;", "static int (*pfn100)() = func3;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( VerifyBuildString( "cannot convert" ) >= 0);  
	LogTestEnd( "Modify Static Function Pointer's Initialization Value" );

	// Backup the change
	LogTestBegin( "Modify Static Function Pointer's Initialization Value: Backup" );
	EXPECT_TRUE( ModifyCodeLine("static int (*pfn100)() = func3;", "static int (*pfn100)() = func1;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
/*	if( IsMsgBox("Could not perform a complete stack walk", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Could not perform a complete stack walk'");
	}
*/	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Based") );
	LogTestEnd( "Modify Static Function Pointer's Initialization Value: Backup" );

	LogTestBegin( "Modify Static Function Pointer's name" );
	EXPECT_TRUE( ModifyCodeLine("static int (*pfn100)() = func1;", "static int (*pfn100X)() = func1;") );
	EXPECT_TRUE( ModifyCodeLine("int i1 = pfn100() + 1;", "int i1 = pfn100X() + 1;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
//	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Edited section exports: void __cdecl Based(void)" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Adding new function to image:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "Symbol changed or missing:" ) >= 0);
//	EXPECT_TRUE( VerifyDebugOutputString( "ENC: Undoing last edit-n-continue" ) >= 0);
	LogTestEnd( "Modify Static Function Pointer's name" );

	// Backup the change
	LogTestBegin( "Modify Static Function Pointer's name: Backup" );
	EXPECT_TRUE( ModifyCodeLine("static int (*pfn100X)() = func1;", "static int (*pfn100)() = func1;") );
	EXPECT_TRUE( ModifyCodeLine("int i1 = pfn100X() + 1;", "int i1 = pfn100() + 1;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	if( IsMsgBox("Point of execution moved", "OK") )
	{
		MST.WButtonClick( "OK" );
		m_pLog->RecordInfo("OK on message box 'Point of execution moved'");
	}
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Based") );
	dbg.StepOut();
	dbg.SetSteppingMode(SRC);
	LogTestEnd( "Modify Static Function Pointer's name: Backup" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyLocalFunctionPointer( )
{
 //	LogTestHeader( "ModifyLocalFunctionPointer" );
	CStartEndTest se_test("ModifyLocalFunctionPointer", this, TRUE);
	bps.ClearAllBreakpoints();
	CString d_value;

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("p_global_char = &rg_global_char[1];") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "p_global_char = &rg_global_char[1];") );

	// #9851  Case sensitive name
 	LogTestBegin( "1 Modify Local function pointer's Initialization value in non active frame" );
	EXPECT_TRUE( ModifyCodeLine("int (*pfn101)() = func1;", "int (*pfn101)() = func3;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
 	LogTestEnd( "1 Modify Local function pointer's Initialization value in non active frame" );

// #13568
 	LogTestBegin( "2 Modify Local function pointer's Initialization value in non active frame" );
	EXPECT_TRUE( ModifyCodeLine("int (*pfn101)() = func3;", "int (*pfn101)() = (int(*)())func3;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.StepOver(1, NULL, "int i2 = pfn101() + 2;"));
	EXPECT_TRUE( cxx.GetExpressionValue("pfn101", d_value) );
	EXPECT_TRUE( d_value.Find("func3") >= 0 );
 	LogTestEnd( "2 Modify Local function pointer's Initialization value in non active frame" );

 	LogTestBegin( "1 Modify Local function pointer's type in active frame After IP" );
	EXPECT_TRUE( src.Find("int i1 = pfn100() + 1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int i1 = pfn100() + 1;") );
	EXPECT_TRUE( stk.CurrentFunctionIs("func2") );
	EXPECT_TRUE( ModifyCodeLine("int (*pfn101)() = (int(*)())func3;", "__int64 (*pfn101)() = (int(*)())func3;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails gracefully 
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
 	LogTestEnd( "1 Modify Local function pointer's type in active frame After IP" );

	LogTestBegin( "2 Modify Local function pointer's type in active frame After IP" );
//	EXPECT_TRUE( src.Find("int i1 = pfn100() + 1;") );
//	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int i1 = pfn100() + 1;") );
	EXPECT_TRUE( stk.CurrentFunctionIs("func2") );
	EXPECT_TRUE( ModifyCodeLine("__int64 (*pfn101)() = (int(*)())func3;", "__int64 (*pfn101)() = (__int64(*)())func3;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	EXPECT_TRUE( !IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("func2") );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "Based") );
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE( src.Find("int i1 = pfn100() + 1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int i1 = pfn100() + 1;") );
	EXPECT_TRUE( stk.CurrentFunctionIs("func2") );
	EXPECT_TRUE( dbg.StepOver(3, NULL, "int *p_2 = &rg_static_int[1];"));
	EXPECT_TRUE( cxx.GetExpressionValue("pfn101", d_value) );
	EXPECT_TRUE( d_value.Find("func3") >= 0 );
	LogTestEnd( "2 Modify Local function pointer's type in active frame After IP" );

 	LogTestBegin( "Modify Local function pointer's name in active frame Before IP" );
	EXPECT_TRUE( ModifyCodeLine("__int64 (*pfn101)() = (__int64(*)())func3;", "__int64 (*pfn101X)() = (__int64(*)())func3;") );
	EXPECT_TRUE( ModifyCodeLine("int i2 = pfn101() + 2;", "int i2 = pfn101X() + 2;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT); // it executes next call to func2, so the new code is executed
	VerifyECSuccess();
	EXPECT_TRUE( dbg.StepOver(1, NULL, "int *p_2 = &rg_static_int[1];"));
 	LogTestEnd( "Modify Local function pointer's name in active frame Before IP" );

	// Backup the changes
 	LogTestBegin( "Backup the changes" );
	EXPECT_TRUE( ModifyCodeLine("__int64 (*pfn101X)() = (__int64(*)())func3;", "int (*pfn101)() = func1;") );
	EXPECT_TRUE( ModifyCodeLine("int i2 = pfn101X() + 2;", "int i2 = pfn101() + 2;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
 	LogTestEnd( "Backup the changes" );

	dbg.StopDebugging();
	return (TRUE);
}


BOOL CModifyingCodeTest::ModifyLocalPointer( )
{
 //	LogTestHeader( "ModifyLocalPointer" );
	CStartEndTest se_test("ModifyLocalPointer", this, TRUE);
	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");

 	LogTestBegin( "Modify Local pointer's Initialization value in non-top active frame After IP" );
	EXPECT_TRUE( src.Find("int i1 = pfn100() + 1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int i1 = pfn100() + 1;","func2" ) );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "func1") );
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( ModifyCodeLine("int *p_static = &rg_static_int[0];", "int *p_static = p_2;") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("func2") );
	EXPECT_TRUE( src.Find("int *p_static = p_2;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int *p_static = p_2;" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "rg_static_int[2] = 32;" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("p_static == p_2", 1) );
 	LogTestEnd( "Modify Local pointer's Initialization value in non-top active frame After IP" );

	//	 #13752
 	LogTestBegin( "Modify Local pointer's type in non-active frame" );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "Based" ) );
	EXPECT_TRUE( ModifyCodeLine("int *p_static = p_2;", "long *p_static = (long *)p_2;") );
	EXPECT_TRUE( src.Find("long *p_static = (long *)p_2;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
//	EXPECT_TRUE( VerifyBuildString( "based.cpp" ) >= 0);  
	EXPECT_TRUE( stk.CurrentFunctionIs("func2") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "rg_static_int[2] = 32;" ) );
 	LogTestEnd( "Modify Local pointer's type in non-active frame" );

 	LogTestBegin( "Modify Local pointer's name in non-top active frame Before IP" );
	dbg.Restart();
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("i1 = func1();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "i1 = func1(); // func2","func2" ) );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "func1") );
	EXPECT_TRUE( ModifyCodeLine("long *p_static = (long *)p_2;", "long *p_staticX = (long *)p_2;") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	VerifyECSuccess();
//8044	EXPECT_TRUE( IsMsgBox("Edit and Continue has applied", "OK") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	dbg.Restart();
//	EXPECT_TRUE( stk.CurrentFunctionIs("func2") );
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("long *p_staticX = (long *)p_2;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "long *p_staticX = (long *)p_2;" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "rg_static_int[2] = 32;" ) );
 	LogTestEnd( "Modify Local pointer's name in non-top active frame Before IP" );

	// Backup the changes
 	LogTestBegin( "Backup the changes" );
	EXPECT_TRUE( ModifyCodeLine("long *p_staticX = (long *)p_2;", "int *p_static = &rg_static_int[0];") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
 	LogTestEnd( "Backup the changes" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyLocalVar( )
{
// 	LogTestHeader( "ModifyLocalVar" );
	CStartEndTest se_test("ModifyLocalVar", this, TRUE);
	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("i1 = func1(); // func2") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "i1 = func1(); // func2") );

 	LogTestBegin( "Modify Local's name in non-active frame" );
	EXPECT_TRUE( ModifyCodeLine("int j_1 = 5;", "int k_1 = 5;") );
	EXPECT_TRUE( ModifyCodeLine("j_1 += i;", "k_1 += i;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.StepOver(1, NULL, "}", "func2" ) );
 	LogTestEnd( "Modify Local's name in non-active frame" );

	// # 13137
 	LogTestBegin( "Modify Local's Initialization value in active frame After IP" );
	EXPECT_TRUE( src.Find("i_1 = 5;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "i_1 = 5;", "func1" ) );
	EXPECT_TRUE( ModifyCodeLine("int k_1 = 5;", "int k_1 = 90;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.StepOver(1, NULL, "for (int i =0; i < 10; i++)", "func1" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("k_1", 90) );
 	LogTestEnd( "Modify Local's Initialization value in active frame After IP" );

 	LogTestBegin( "Modify Local's type in active frame aT IP" );
	EXPECT_TRUE( src.Find("int k_1 = 90;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int k_1 = 90;", "func1" ) );
	EXPECT_TRUE( ModifyCodeLine("int k_1 = 90;", "short k_1 = 90;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
 	LogTestEnd( "Modify Local's type in active frame aT IP" );

 	LogTestBegin( "Modify Local's type in non-top active frame before IP" );
	dbg.Restart();
	dbg.SetSteppingMode(SRC);
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("ifn2 = pfn222();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "ifn2 = pfn222();", "Based" ) );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "func2") );
	EXPECT_TRUE( ModifyCodeLine("__int64 i64 = func3();", "long i64 = func3();") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//8044	EXPECT_TRUE( IsMsgBox("Edit and Continue has applied", "OK") );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "OK") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("Based")	);
	dbg.Restart();
	dbg.SetSteppingMode(SRC);
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("long i64 = func3();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "long i64 = func3();", "Based" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "for(i=0; i<26; i++) rg_global_int2[i] = 66;" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("i64", 3333) );
 	LogTestEnd( "Modify Local's type in non-top active frame before IP" );

 	LogTestBegin( "Modify Local's Initialization value in active frame before IP" );
	EXPECT_TRUE( ModifyCodeLine("long i64 = func3();", "long i64 = 35;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	dbg.Restart();
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("long i64 = 35;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "long i64 = 35;", "Based" ) );
	dbg.StepOver(1);
	EXPECT_TRUE( cxx.ExpressionValueIs("i64", 35) );
 	LogTestEnd( "Modify Local's Initialization value in active frame before IP" );

 	LogTestBegin( "Modify Local's name in active frame aT IP" );
	dbg.Restart();
	UIWB.OpenFile( m_strProjectLoc + "based.cpp");
	EXPECT_TRUE( src.Find("long i64 = 35;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "long i64 = 35;", "Based" ) );
	EXPECT_TRUE( ModifyCodeLine("long i64 = 35;", "long i64X = 35;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( cxx.ExpressionValueIs("i64X", 35) );
 	LogTestEnd( "Modify Local's name in active frame aT IP" );

	// Backup the changes
 	LogTestBegin( "Backup the changes" );
	EXPECT_TRUE( ModifyCodeLine("short k_1 = 90;", "int j_1 = 5;") );
	EXPECT_TRUE( ModifyCodeLine("k_1 += i;", "j_1 += i;") );
	EXPECT_TRUE( ModifyCodeLine("long i64X = 35;", "__int64 i64 = func3();") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
 	LogTestEnd( "Backup the changes" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyLocalObject( )
{
// 	LogTestHeader( "ModifyLocalObject" );
	CStartEndTest se_test("ModifyLocalObject", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( src.Find("int test_include = Test_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "int test_include = Test_func();") );

 	LogTestBegin( "Modify Local object's Initialization value in active frame aT IP" );
	EXPECT_TRUE( src.Find("base* object = object1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "base* object = object1;") );
	EXPECT_TRUE( ModifyCodeLine("base* object = object1;", "base* object = object2;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "object = object2; // assignment" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("object == object2", 1) );
 	LogTestEnd( "Modify Local object's Initialization value in active frame aT IP" );

	// 13772 
 	LogTestBegin( "Modify Local object's name in non-top active frame Before IP" );
	EXPECT_TRUE( src.Find("obj.func('a');") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "obj.func('a');") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "A_CLASS::func"));
	EXPECT_TRUE( ModifyCodeLine("base* object = object2;", "base* objectX = object2;") );
	EXPECT_TRUE( ModifyCodeLine("object = object2; // assignment", "objectX = object2; // assignment") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//8044	EXPECT_TRUE( IsMsgBox("Edit and Continue has applied", "OK") );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "OK") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// It goes to DAM
//	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);  
	if( WaitMsgBoxText("The function on top of the call stack has been updated", 10) )
		MST.WButtonClick( "OK" );
//	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
//	EXPECT_TRUE( stk.CurrentFunctionIs("Cxx_Tests")	);
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "Cxx_Tests"));
	dbg.Restart();
	dbg.SetSteppingMode(SRC);
 	LogTestEnd( "Modify Local object's name in non-top active frame Before IP" );

 	LogTestBegin( "Modify Local object's type in non-active frame" );
	UIWB.OpenFile( m_strProjectLoc + "cxx.cxx");
	EXPECT_TRUE( ModifyCodeLine("base* objectX = object2;", "derived1* objectX = object2;") );
	EXPECT_TRUE( src.Find("derived1* objectX = object2;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC fails : unrelated types
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "No") );
	EXPECT_TRUE( IsMsgBox("One or more errors occurred", "No") );
	EXPECT_TRUE( VerifyBuildString( "cannot convert" ) >= 0);  
	// backup changes
	EXPECT_TRUE( ModifyCodeLine("derived1* objectX = object2;", "base* objectX = object2;") );
 	LogTestEnd( "Modify Local object's type in non-active frame" );

 	LogTestBegin( "Modify Local object's Initialization value in non-active frame" );
	EXPECT_TRUE( ModifyCodeLine("base *pd = new derived;", "base *pd = pb;") );
	EXPECT_TRUE( src.Find("base *pd = pb;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "base *pd = pb;" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "pd->m_public = 1;", "Test_func"));
 	LogTestEnd( "Modify Local object's Initialization value in non-active frame" );
 
 	LogTestBegin( "Modify Local object's type in active frame Before IP" );
	EXPECT_TRUE( src.Find("int ib2 = b2.base2_virtual_func(1);") );
	dbg.StepToCursor(0, NULL, "int ib2 = b2.base2_virtual_func(1);");
	EXPECT_TRUE( ModifyCodeLine("base2 b2;", "base1 b2;") );
	EXPECT_TRUE( ModifyCodeLine("int ib2 = b2.base2_virtual_func(1);", "int ib2 = b2.base1_virtual_func(1);") );
	EXPECT_TRUE( src.Find("base1 b2;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
//	VerifyECSuccess();
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot apply code changes", "Yes") );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "Yes") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
//			MST.WButtonClick( "OK" );
			MST.WButtonClick( "Yes" );
//	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	// It goes to DAM
//	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);  
	if( WaitMsgBoxText("The function on top of the call stack has been updated", 10) )
		MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "derivedm drvm;"));
 	LogTestEnd( "Modify Local object's type in active frame Before IP" );

 	LogTestBegin( "Modify Local object's name in active frame After IP" );
	EXPECT_TRUE( src.Find("base1 b1;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "base1 b1;") );
	EXPECT_TRUE( ModifyCodeLine("base1 b2;", "base1 b2X;") );
//	EXPECT_TRUE( ModifyCodeLine("derivedm drvm = b2;", "derivedm drvm = b2X;") );
	EXPECT_TRUE( ModifyCodeLine("int ib2 = b2.base1_virtual_func(1);", "int ib2 = b2X.base1_virtual_func(1);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
	if( WaitMsgBoxText("Cannot complete Edit and Continue", 10) )
		MST.WButtonClick( "OK" );
	if( WaitMsgBoxText("The function on top of the call stack has been updated", 10) )
		MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "int iderived = drvm.derivedm_public_func(1+2);"));
 	LogTestEnd( "Modify Local object's name in active frame After IP" );

	// Backup the changes
 	LogTestBegin( "Backup all the changes" );
	EXPECT_TRUE( ModifyCodeLine("base* objectX = object2;", "base* object = object1;") );
	EXPECT_TRUE( ModifyCodeLine("objectX = object2; // assignment", "object = object2; // assignment") );
//	EXPECT_TRUE( ModifyCodeLine("derivedm drvm = b2X;", "derivedm drvm;") );
	EXPECT_TRUE( ModifyCodeLine("base1 b2X;", "base2 b2;") );
	EXPECT_TRUE( ModifyCodeLine("int ib2 = b2X.base1_virtual_func(1);", "int ib2 = b2.base2_virtual_func(1);") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
//	VerifyECSuccess();
	if( WaitMsgBoxText("Do you want to save", 10) )
		MST.WButtonClick( "No" );
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "OK") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
//			MST.WButtonClick( "OK" );
			MST.WButtonClick( "Yes" );
	if( WaitMsgBoxText("The function on top of the call stack has been updated", 10) )
		MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
 	LogTestEnd( "Backup all the changes" );

	dbg.StopDebugging();

	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyOperations( )
{
// 	LogTestHeader( "ModifyOperations" );
	CStartEndTest se_test("ModifyOperations", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( src.Find("Double = func_3param(Int, Long, Real);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Double = func_3param(Int, Long, Real);") );

 	LogTestBegin( "Modify Operation in non-active frame" );
	EXPECT_TRUE( ModifyCodeLine("i_3p++;", "i_3p += intParam;") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( src.Find("i_3p += intParam;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "i_3p += intParam;") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "l_3p += i_3p;", "func_3param"));
	EXPECT_TRUE( cxx.ExpressionValueIs("i_3p", 50) );
 	LogTestEnd( "Modify Operation in non-active frame" );
 
 	LogTestBegin( "Modify Operation in active frame aT IP" );
	EXPECT_TRUE( src.Find("i_3p += intParam;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "i_3p += intParam;") );
	EXPECT_TRUE( ModifyCodeLine("i_3p += intParam;", "i_3p--;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "l_3p += i_3p;" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("i_3p", 32) );
 	LogTestEnd( "Modify Operation in active frame aT IP" );
 
 	LogTestBegin( "Modify Operation in non-top active frame" );
	EXPECT_TRUE( src.Find("i_3p = func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "i_3p = func();") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "func") );
	EXPECT_TRUE( ModifyCodeLine("l_3p += i_3p;", "l_3p -= i_3p;") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( src.Find("f_3p = l_3p - i_3p;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "f_3p = l_3p - i_3p;", "func_3param") );
	EXPECT_TRUE( cxx.ExpressionValueIs("l_3p", 967) );
 	LogTestEnd( "Modify Operation in non-top active frame" );
 
 	LogTestBegin( "Modify Operation in active frame Before IP" );
	EXPECT_TRUE( ModifyCodeLine("double d_3p = intParam + longParam + floatParam;", "double d_3p = (intParam + longParam) * floatParam;") );
	EXPECT_TRUE( src.Find("i_3p = func();") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "i_3p = func();" ) );
	double d_value;
	EXPECT_TRUE( cxx.GetExpressionValue("d_3p", &d_value) );
	EXPECT_TRUE( d_value > 1333. );
 	LogTestEnd( "Modify Operation in active frame Before IP" );
 
 	LogTestBegin( "Modify Operation in active frame After IP" );
	EXPECT_TRUE( ModifyCodeLine("f_3p = l_3p - i_3p;", "f_3p += (l_3p / i_3p);") );
	EXPECT_TRUE( src.Find("l_3p++;") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "l_3p++;" ) );
	float f_value;
	EXPECT_TRUE( cxx.GetExpressionValue("f_3p", &f_value) );
	EXPECT_TRUE( f_value < 50. );
 	LogTestEnd( "Modify Operation in active frame After IP" );
 
	// Backup the changes
 	LogTestBegin( "Backup the changes" );
	EXPECT_TRUE( ModifyCodeLine("i_3p--;", "i_3p++;") );
	EXPECT_TRUE( ModifyCodeLine("l_3p -= i_3p;", "l_3p += i_3p;") );
	EXPECT_TRUE( ModifyCodeLine("double d_3p = (intParam + longParam) * floatParam;", "double d_3p = intParam + longParam + floatParam;") );
	EXPECT_TRUE( ModifyCodeLine("f_3p += (l_3p / i_3p);", "f_3p = l_3p - i_3p;") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
 	LogTestEnd( "Backup the changes" );

	dbg.StopDebugging();

	return (TRUE);
}

BOOL CModifyingCodeTest::ModifyLoop( )
{
// 	LogTestHeader( "ModifyLoop" );
	CStartEndTest se_test("ModifyLoop", this, TRUE);

	bps.ClearAllBreakpoints();

	// Start debugging
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( src.Find("if (Flag)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "if (Flag)") );

 	LogTestBegin( "Modify if statement condition in active frame aT IP" );
	EXPECT_TRUE( ModifyCodeLine("if (Flag)", "if (Flag - 1)") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "for (Int=0; Int<3; Int++)" ) );
 	LogTestEnd( "Modify if statement condition in active frame aT IP" );
 
 	LogTestBegin( "Modify While loop condition in non-top active frame" );
	EXPECT_TRUE( src.Find("Mine=func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Mine=func();") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "func") );
	EXPECT_TRUE( ModifyCodeLine("while (Local == 17)", "while (Local < 17)") );
	dbg.StepOut(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( src.Find("while (Local < 17)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "while (Local < 17)") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "else"));
 	LogTestEnd( "Modify While loop condition in non-top active frame" );
 
 	LogTestBegin( "Modify combination of if-do-while conditions in active frame After IP" );
	EXPECT_TRUE( ModifyCodeLine("if (1 < Local) do { GlobalG++; } while (Local == 0);", "if (1 < Local) do { GlobalG++; } while (GlobalG < 80);") );
	EXPECT_TRUE( src.Find("if (1 < Local) do { GlobalG++; } while (GlobalG < 80);") );
	dbg.StepToCursor(0, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( dbg.CurrentLineIs( "if (1 < Local) do { GlobalG++; } while (GlobalG < 80);" ) );
	dbg.StepOver();
	EXPECT_TRUE( cxx.ExpressionValueIs("GlobalG", 80) );
 	LogTestEnd( "Modify combination of if-do-while conditions in active frame After IP" );

 	LogTestBegin( "Modify for loop conditions in active frame Before IP" );
	EXPECT_TRUE( ModifyCodeLine("for (Int=0; Int<3; Int++)", "for (Int=2; Int<3; Int++)") );
//	dbg.Restart();
	UIWB.Restart(NOWAIT);
	VerifyECSuccess();
	UIWB.OpenFile( m_strProjectLoc + "testg.cpp");
	EXPECT_TRUE( src.Find("Local=17;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Local=17;") );
	EXPECT_TRUE( !cxx.ExpressionValueIs("Array1d[1]", 1) );
 	LogTestEnd( "Modify for loop conditions in active frame Before IP" );
 
 	LogTestBegin( "Modify While loop in non-active frame" );
	EXPECT_TRUE( src.Find("Double = func_3param(Int, Long, Real);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Double = func_3param(Int, Long, Real);", "TestG") );
	EXPECT_TRUE( ModifyCodeLine("}while(l_3p <1050);", "}while(l_3p <1060);") );
	dbg.StepInto(1, NULL, NULL, NULL, NOWAIT);
	VerifyECSuccess();
	EXPECT_TRUE( stk.CurrentFunctionIs("func_3param") );
	EXPECT_TRUE( src.Find("return d_3p;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "return d_3p;") );
	EXPECT_TRUE( cxx.ExpressionValueIs("l_3p", 1060) );
 	LogTestEnd( "Modify While loop in non-active frame" );
 
	// Backup the changes
 	LogTestBegin( "Backup the changes" );
	EXPECT_TRUE( ModifyCodeLine("if (Flag - 1)", "if (Flag)") );
	EXPECT_TRUE( ModifyCodeLine("while (Local < 17)", "while (Local == 17)") );	  
	EXPECT_TRUE( ModifyCodeLine("if (1 < Local) do { GlobalG++; } while (GlobalG < 80);", "if (1 < Local) do { GlobalG++; } while (Local == 0);") );
	EXPECT_TRUE( ModifyCodeLine("for (Int=2; Int<3; Int++)", "for (Int=0; Int<3; Int++)") );
	EXPECT_TRUE( ModifyCodeLine("}while(l_3p <1060);", "}while(l_3p <1050);") );
	dbg.StepOver(1, NULL, NULL, NULL, NOWAIT);
	// Verify EC success
//	EXPECT_TRUE( IsMsgBox("Cannot complete Edit and Continue", "OK") );
	if (MST.WFndWndWait("Edit and Continue", FW_NOCASE | FW_FOCUS | FW_PART, 1))
			MST.WButtonClick( "OK" );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_BREAK) );
	EXPECT_TRUE( stk.CurrentFunctionIs("func_3param") );
 	LogTestEnd( "Backup the changes" );

	dbg.StopDebugging();

	return (TRUE);
}


