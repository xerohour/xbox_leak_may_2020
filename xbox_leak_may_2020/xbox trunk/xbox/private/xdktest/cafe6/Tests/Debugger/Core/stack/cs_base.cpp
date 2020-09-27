///////////////////////////////////////////////////////////////////////////////
//	cs_base.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "stacsub.h"
#include "cs_base.h"	 

#if defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(Cstack_baseIDETest, CDebugTestSet, "CallStack Base", -1, CstackSubSuite)

void Cstack_baseIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void Cstack_baseIDETest::Run()
{
	XSAFETY;

//	EXPECT_EXEC( dbg.SetOwnedStack( &stk ), "Could not setup owned stack" );
//	XSAFETY;

	/*
	** For the dbg application:
	** ChainMain is expected to be compiled with full debug information
	** chain_1 is expected to be compiled with full debug information
	** chain_2 is expected to be compiled with partial debug information
	** chain_3 is expected to be compiled without debug information
	** chain_4 is expected to be compiled with full debug information
	** chain_5 is expected to be compiled with full debug information
	*/
	EXPECT_EXEC( SetProject( "dbg\\dbg" ), "Project could not be setup" );
	XSAFETY;
	GeneralNavigation();
	XSAFETY;
	SteppingAndCallStackUpdating();
	XSAFETY;
	ChainDisplay();
	XSAFETY;

	if( SetProject("stack01\\stack01") )
	{	  
		XSAFETY;
		CheckLocals();
		XSAFETY;
	}

	if( SetProject("testgo01\\testgo01") )
	{	  
		XSAFETY;
		NavigationAndModifiedLocalGlobal();
		XSAFETY;
	}

}

BOOL Cstack_baseIDETest::GeneralNavigation()
{
	CString fnName;	
	CString szTitle;
	UIEditor editor;
 	LogTestHeader( "GeneralNavigation Test" );
 	LogTestHeader( "Initial callstack after first stepin" );
	EXPECT_SUBTEST( dbg.StepInto( 1, "WinMain" ) );
	EXPECT_SUBTEST( Frame( stk.GetFunction( 0 ) ).GetFunctionName( fnName ) );
	EXPECT_SUBTEST( fnName == "WinMain" );

 	LogTestHeader( "General navigation with full debug info" );
	EXPECT_SUBTEST( NULL != bps.SetBreakpoint("chain_5") );
	EXPECT_SUBTEST( dbg.Go() );
	EXPECT_SUBTEST( dbg.AtSymbol("chain_5"));
//	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);	

 	LogTestHeader( "Navigate to 'chain_5' : full debug info : current function" );
	EXPECT_TRUE( stk.NavigateStack( "chain_5" )>0 );
	MST.WFndWndWait("chain1.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "chain1.c" );
	if( GetUserTargetPlatforms() == PLATFORM_WIN32_ALPHA ) {
		EXPECT_TRUE( src.GetCurrentLine() == 34 );
	}
	else {
		EXPECT_TRUE( src.GetCurrentLine() == 32 );
	}
	// bad programing practice - string constants?? WinslowF
	MST.WFndWndWait("chain1.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( ERROR_SUCCESS == src.Close(FALSE) );

 	LogTestHeader( "Navigate to 'chain_4' : full debug info" );
	EXPECT_TRUE( stk.NavigateStack( "chain_4" )>0 );
	MST.WFndWndWait("chain4.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "chain4.c" );
	EXPECT_TRUE( src.GetCurrentLine() == 99 );
	EXPECT_TRUE( editor.Close() );
	VerifyLocalsWndString("int", "x", "3");
	VerifyLocalsWndString("int", "Sum_4", "4", 2);

 	LogTestHeader( "Navigate to 'chain_2' : partial debug info" );
	EXPECT_TRUE( stk.NavigateStack( "DBG! chain_2" )>0 );
	MST.WFndWndWait("chain2.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "chain2.c" );
	EXPECT_TRUE( src.GetCurrentLine() == 26 );
//	EXPECT_TRUE( editor.Close() ); --leave the "chain2.c" file opened

 	LogTestHeader( "Navigate to 'chain_1' : full debug info" );
	EXPECT_TRUE( stk.NavigateStack( "chain_1" )>0 );
	MST.WFndWndWait("chain1.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "chain1.c" );
	EXPECT_TRUE( src.GetCurrentLine() == 24 );
	EXPECT_TRUE( editor.Close() );

 	LogTestHeader( "Navigate to 'ChainMain' : full debug info" );
	EXPECT_TRUE( stk.NavigateStack( "ChainMain" )>0 );
	MST.WFndWndWait("chain0.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "chain0.c" );
	EXPECT_TRUE( src.GetCurrentLine() == 28 );
	EXPECT_TRUE( editor.Close() );

 	LogTestHeader( "Navigate to 'WndProc' : full debug info" );
	EXPECT_TRUE( stk.NavigateStack( "WndProc" )>0 );
	MST.WFndWndWait("main.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "main.c" );
	EXPECT_TRUE( src.GetCurrentLine() == 117 );
	EXPECT_TRUE( editor.Close() );

 	LogTestHeader( "Navigate to 'chain_3' (DAM: no debug info)" );
	EXPECT_TRUE( stk.NavigateStack( "chain_3" )==-1 );
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	EXPECT_TRUE( editor.GetTitle() == "Disassembly" );
	// bad programing practice - string constants?? WinslowF
	MST.WFndWndWait("Disassembly", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( ERROR_SUCCESS == src.Close(FALSE) );

 	LogTestHeader( "Once more to 'chain_5'" );
	EXPECT_TRUE( stk.NavigateStack( "chain_5" )>0 );
	MST.WFndWndWait("chain1.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "chain1.c" );
	if( GetUserTargetPlatforms() == PLATFORM_WIN32_ALPHA) 
	{
		EXPECT_TRUE( src.GetCurrentLine() == 34 );
	}
	else
	{
		EXPECT_TRUE( src.GetCurrentLine() == 32 );
	}

	LogTestHeader( "Once more to 'chain_2' : source wnd opened" );
	EXPECT_TRUE( stk.NavigateStack( "DBG! chain_2" )>0 );
	MST.WFndWndWait("chain2.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "chain2.c" );
	EXPECT_TRUE( src.GetCurrentLine() == 26 );
//	EXPECT_TRUE( editor.Close() );

	LogTestHeader( "Navigation within the C++ member functions" );
	UIWB.DoCommand(ID_WINDOW_CLOSE_ALL, DC_MNEMONIC);	
	EXPECT_SUBTEST( NULL != bps.SetBreakpoint("fooclass::chain2_func") );
	EXPECT_SUBTEST( dbg.Go() );
	EXPECT_SUBTEST( dbg.AtSymbol("fooclass::chain2_func"));

 	LogTestHeader( "Navigate to 'fooclass::chain1_func', no src window" );
	EXPECT_TRUE( stk.NavigateStack( "fooclass::chain1_func" )>0 );
	MST.WFndWndWait("cxx.cxx", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	EXPECT_TRUE( szTitle == "cxx.cxx" );
	EXPECT_TRUE( src.GetCurrentLine() == 52 );
//	EXPECT_TRUE( editor.Close() );
	VerifyLocalsWndString("unsigned char", "local_cpp_func", "65", 2);

 	LogTestHeader( "Navigate to 'fooclass::chain2_func', src wnd opened" );
	EXPECT_TRUE( stk.NavigateStack( "fooclass::chain2_func" )>0 );
	MST.WFndWndWait("cxx.h", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	szTitle = editor.GetTitle();
	EXPECT_TRUE( szTitle == "cxx.h" );
	EXPECT_TRUE( src.GetCurrentLine() == 50 );
//	EXPECT_TRUE( editor.Close() );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL Cstack_baseIDETest::ChainDisplay()
{
	CString fnName;
	UIEditor editor;
	int index;
	const char* frame1;
	const char* frame6;

 	LogTestHeader( "Verify Call Stack chain display" );

	bps.ClearAllBreakpoints();

	// Get into nested function to prepare for the test
	EXPECT_TRUE( bps.SetBreakpoint("chain_5") );
	EXPECT_TRUE( dbg.Go(NULL, NULL, "chain_5") );
//	EXPECT_TRUE( dbg.Go() );
//	EXPECT_TRUE( dbg.AtSymbol("chain_5"));

 	LogTestHeader( "Navigate to 'WndProc'" );
	EXPECT_TRUE( stk.NavigateStack( "WndProc" )>0 );
	MST.WFndWndWait("main.c", FW_FOCUS | FW_PART, 2);
	EXPECT_TRUE( src.AttachActiveEditor() );
	EXPECT_TRUE( editor.AttachActive() );
	CString szTitle = editor.GetTitle();
	szTitle.MakeLower();
	EXPECT_TRUE( szTitle == "main.c" );
	EXPECT_TRUE( src.GetCurrentLine() == 117 );

	if( GetUserTargetPlatforms() == PLATFORM_WIN32_ALPHA )
	{
		frame1 = "chain_5( int 3 ) line 34";
	}
	else 
	{
		frame1 = "chain_5( int 3 ) line 32";
	}

	frame6 = "WndProc( HWND__ * %, unsigned int %, unsigned int %, long %) line 117";

	char const * const stk0[] =
	{
		{ frame1 },
		{ "chain_4( int 3 ) line 99" },
		{ "DBG! chain_3" },
		{ "DBG! chain_2 line 26" },
		{ "chain_1( int % ) line 24" },
		{ "ChainMain() line 28" },
		{ frame6 },
//		{ "WndProc( HWND__ * %, unsigned int %, unsigned int %, long %) line 117" },
		NULL
	};

	// Verify information on stack above callback
	EXPECT_SUBTEST( (index = stk.Index( "WndProc" ) ) != -1 );
	EXPECT_SUBTEST( stk.Compare( stk0, FrameList::NoFilter, 0, index ) );

	dbg.StopDebugging();

	RebuildProject("dbg\\dbg","1"); //configuration 1
	EXPECT_TRUE( dbg.Go(NULL, NULL, "chain_5") );
//	EXPECT_TRUE( dbg.Go() );
//	EXPECT_TRUE( dbg.AtSymbol("chain_5"));
	char const * const stk1[] =
	{
		{ frame1 },
		{ "chain_4( int 3 ) line 99" },
		{ "chain_3( int 3 ) line 27" },
		{ "DBG! chain_2 line 26" },
		{ "chain_1( int % ) line 24" },
		{ "ChainMain() line 28" },
		{ frame6 },
//		{ "WndProc( HWND__ * %, unsigned int %, unsigned int %, long %) line 117" },
		NULL
	};
	// Verify information on stack above callback
	EXPECT_SUBTEST( (index = stk.Index( "WndProc" ) ) != -1 );
	EXPECT_SUBTEST( stk.Compare( stk1, FrameList::NoFilter, 0, index ) );

	dbg.StopDebugging();

	XSAFETY;

	RebuildProject("dbg\\dbg","2"); //configuration 2
	EXPECT_TRUE( dbg.Go(NULL, NULL, "chain_5") );
//	EXPECT_TRUE( dbg.Go() );
//	EXPECT_TRUE( dbg.AtSymbol("chain_5"));
	char const * const stk2[] =
	{
		{ frame1 },
		{ "chain_4( int 3 ) line 99" },
		{ "DBG! chain_3" },
		{ "chain_2( int 3 ) line 26" },
		{ "chain_1( int % ) line 24" },
		{ "ChainMain() line 28" },
		{ frame6 },
//		{ "WndProc( HWND__ * %, unsigned int %, unsigned int %, long %) line 117" },
		NULL
	};
	// Verify information on stack above callback
	EXPECT_SUBTEST( (index = stk.Index( "WndProc" ) ) != -1 );
	EXPECT_SUBTEST( stk.Compare( stk2, FrameList::NoFilter, 0, index ) );

	dbg.StopDebugging();
	XSAFETY;

	return (TRUE);
}

void Cstack_baseIDETest::RebuildProject(LPCSTR projName,LPCSTR pConfig)
{
	GetSubSuite()->CleanUp();

	EXPECT_EXEC( SetProject(projName, PROJECT_EXE,fBuildOnly,pConfig), "Project could not be rebuilt" );
}

BOOL Cstack_baseIDETest::SteppingAndCallStackUpdating()
{
	CString fnName;
	UIEditor editor;

 	LogTestHeader( "SteppingAndCallStackUpdating" );

	bps.ClearAllBreakpoints();

 	LogTestHeader( "Updating of callstack during stepping" );

	UIWB.OpenFile( m_strProjectDir + "\\" + "main.c");   //Review yefims
	
	EXPECT_SUBTEST( dbg.StepToCursor(117) );

		FrameList list0;
		list0.CreateFromText( stk.GetAllFunctions() );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "OtherMain" ) );

		EXPECT_SUBTEST( StepInUntilFrameAdded( "foo" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		
		//	TODO: this is due to bug #65634
		//	remove the if line below when fixed - test fails on NT4
		if (GetSystem() != SYSTEM_NT_4)
		EXPECT_TRUE( cxx.ExpressionValueIs("foo(5, 6)", "11") ); //evaluate the function we've stepped out
		EXPECT_SUBTEST( StepInUntilFrameAdded( "BPWithLengthTest" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );

		EXPECT_SUBTEST( StepInUntilFrameAdded( "Cxx_Tests" ) );
		long an_exception;
		EXPECT_TRUE( cxx.GetExpressionValue("OtherMain()", &an_exception)); //evaluate the function we're at which throws an exception (discard result)
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::fooclass" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::getID" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::getCount" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::operator int()" ) );
		EXPECT_TRUE( cxx.ExpressionValueIs("Cxx_Tests()", "<void>") ); //evaluate the function we're at
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::static_func" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::clear" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::inline_func" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::operator int()" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );
		EXPECT_SUBTEST( dbg.StepOverUntilLine( "};") );
		EXPECT_SUBTEST( StepInUntilFrameAdded( "fooclass::~fooclass" ) );
	
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );

		EXPECT_SUBTEST( StepInUntilFrameRemoved() );	// Cxx_Tests

		//	TODO: this is due to bug #65634
		//	remove the if line below when fixed - test fails on NT4
		if (GetSystem() != SYSTEM_NT_4)
		EXPECT_TRUE( cxx.ExpressionValueIs("foo2(6, 5)", "11") ); //evaluate the function we're about to step into
		EXPECT_SUBTEST( StepInUntilFrameAdded( "foo2" ) );
		EXPECT_SUBTEST( StepInUntilFrameRemoved() );

//	StopDbgCloseProject();
//	WriteLog(PASSED,"Testing of dbg.mak completed");
//	Sleep (200);

	dbg.StopDebugging();
	return (TRUE);
}

BOOL Cstack_baseIDETest::CheckLocals(   )
{
 	LogTestHeader( "CheckLocals" );

	CString strSource = "cstack.c";
	dbg.StepInto();

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);   //Review yefims
//	file.Open( m_strProjectDir + "\\" + strSource);   
	MST.WFndWnd(strSource, FW_FOCUS);

	EXPECT_TRUE( src.Find("VoidProcNoParam();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "VoidProcNoParam();") );

	MST.WFndWnd(strSource, FW_FOCUS);
	EXPECT_TRUE( dbg.StepInto() );

	CString strFunc = "VoidProcNoParam()";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);

	dbg.SetSteppingMode(SRC);
	MST.WFndWnd(strSource, FW_FOCUS);
	EXPECT_TRUE( src.Find("CharProc1Param(SCHAR_MIN)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "CharProc1Param(SCHAR_MIN)") );
	EXPECT_TRUE( dbg.StepInto(2) );
	strFunc = "CharProc1Param(char -128)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	VerifyLocalsWndString("char", "c", "-128");

	dbg.SetSteppingMode(SRC);
	MST.WFndWnd(strSource, FW_FOCUS);
	EXPECT_TRUE( src.Find("UCharProc1Param(UCHAR_MAX)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "UCharProc1Param(UCHAR_MAX)") );
	EXPECT_TRUE( dbg.StepInto(2) );
	strFunc = "UCharProc1Param(unsigned char 255)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	VerifyLocalsWndString("unsigned char", "uc", "255");

	dbg.SetSteppingMode(SRC);
	MST.WFndWnd(strSource, FW_FOCUS);
	EXPECT_TRUE( src.Find("(IntProc1Param(INT_MIN)") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "(IntProc1Param(INT_MIN)") );
	EXPECT_TRUE( dbg.StepInto(2) );
	strFunc = "IntProc1Param(int -2147483648)";
	WriteLog( stk.CurrentFunctionIs(strFunc) ? PASSED : FAILED, "Current function expected value: %s ", strFunc);
	VerifyLocalsWndString("int", "i", "-2147483648");
	
	return (TRUE);
}

BOOL Cstack_baseIDETest::NavigationAndModifiedLocalGlobal()
{
 	LogTestHeader( "NavigationAndModifiedLocalGlobal" );
	bps.ClearAllBreakpoints();

	bps.SetBreakpoint("foo");
	EXPECT_TRUE( dbg.Go(NULL, "int foo(int Parameter) {", "foo") );
	EXPECT_TRUE( cxx.ExpressionValueIs("Global", 77) );
	CString errorString = "CXX0017: Error: symbol \"Local\" not found";
	EXPECT_TRUE( cxx.ExpressionErrorIs("Local", errorString) );

	EXPECT_TRUE( stk.NavigateStack("TestG")>0 );
	EXPECT_TRUE( cxx.ExpressionValueIs("Global", 77) );
	EXPECT_TRUE( cxx.ExpressionValueIs("Local", 17) );
	EXPECT_TRUE( cxx.SetExpressionValue("Global", 60) );
	EXPECT_TRUE( cxx.SetExpressionValue("Local", 10) );

	EXPECT_TRUE( stk.NavigateStack("foo")>0 );
	EXPECT_TRUE( cxx.ExpressionValueIs("Global", 60) );
	EXPECT_TRUE( cxx.ExpressionErrorIs("Local", errorString) );

	EXPECT_TRUE( stk.NavigateStack("WinMain")>0 );
	EXPECT_TRUE( cxx.ExpressionValueIs("Global", 60) );
	EXPECT_TRUE( cxx.ExpressionErrorIs("Local", errorString) );

	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "TestG") );
	EXPECT_TRUE( cxx.ExpressionValueIs("Global", 60) );
	EXPECT_TRUE( cxx.ExpressionValueIs("Local", 10) );
	
	bps.ClearAllBreakpoints();
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "WinMain") );
	EXPECT_TRUE( cxx.ExpressionValueIs("Global", 60) );
	EXPECT_TRUE( cxx.ExpressionErrorIs("Local", errorString) );
	
	dbg.StopDebugging();
	return (TRUE);
}

