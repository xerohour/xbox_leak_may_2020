///////////////////////////////////////////////////////////////////////////////
//	exe_base.CPP
//											 
//	Created by :			
//		VCBU QA		

#include <process.h>
#include "stdafx.h"
#include "exe_base.h"	

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(Cexe_baseIDETest, CDebugTestSet, "Execution Base", -1, CexeSubSuite)
												 
void Cexe_baseIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void Cexe_baseIDETest::Run()
{	 
	XSAFETY;
	
	if( SetProject("testgo01\\testgo01"))
	{	  
		bps.ClearAllBreakpoints();
		XSAFETY;
		GoAndVerify();
		XSAFETY;
		RestartAndGo();
		XSAFETY;
		RestartStopDebugging();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"testgo01\"." );
		
	if( SetProject("testbp01\\testbp01") )
	{	  
		bps.ClearAllBreakpoints();
		XSAFETY;
		StepOutSource();
		XSAFETY;
		StepOverSource();
		XSAFETY;
		StepIntoSource();
		XSAFETY;
		StepToCursorSource();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"testbp01\"." );

		
	if ( SetProject("dbg\\dbg") )
	{
		bps.ClearAllBreakpoints();
		XSAFETY;
		EXPECT(StepModel());
		XSAFETY;
		StopDbgCloseProject();
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"dbg\"." );

		
// TODO(michma - 3/26/98): apparently this test wasn't finished.
/*		
	if ( SetProject("spcfc01\\spcfc01") )
	{
		XSAFETY;
		StaticFuncStepTrace();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"spcfc01\"." );
*/
}


///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL Cexe_baseIDETest::GoAndVerify( )
{
 	LogTestHeader( "GoAndVerify" );
	m_pLog->RecordInfo( " Run an app and verify it ran " );

	EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION) );
	return (TRUE);
}

BOOL Cexe_baseIDETest::RestartAndGo( )
{
 	LogTestHeader( "RestartAndGo" );
	m_pLog->RecordInfo( " Restart, verify stop and Go " );

	EXPECT_TRUE( dbg.Restart() );
	if( UIWB.GetPcodeSwitch() )
	{
		EXPECT_TRUE( dbg.CurrentLineIs( "int iGo = TestG();" ) );
	}
	else
	{
		EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );
	}

	EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION) );

	return (TRUE);
}


BOOL Cexe_baseIDETest::RestartStopDebugging( )
{
 	LogTestHeader( "RestartStopDebugging" );
	m_pLog->RecordInfo( "Restart, StopDebugging and verify" );

	EXPECT_TRUE( dbg.StepInto(1, NULL, "{") );
	EXPECT_TRUE( dbg.StepOver(1) );
    EXPECT_TRUE( dbg.Restart() );
	EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );
	EXPECT_TRUE( dbg.StopDebugging() );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_TERMINATION) );

/*	#if defined( _M_IX86 ) || defined( _MAC )  // WinslowF - for OEM compatibility
		EXPECT_TRUE( VerifyProjectRan(0) );
	#else
		EXPECT_TRUE( VerifyProjectRan(1) );
	#endif
*/
	EXPECT_TRUE( dbg.StepOver(1) );
	EXPECT_TRUE( dbg.StopDebugging() );
	EXPECT_TRUE( dbg.Wait(WAIT_FOR_TERMINATION) );

	WriteLog(PASSED, "Restart-StopDebugging-StartDebugging was handled as expected" );
	return (TRUE);
}


BOOL Cexe_baseIDETest::StepOutSource(   )
{
 	LogTestHeader( "StepOutSource" );

	COSource		src;	// WinslowF - added to call GoToLine. It's not in UIWB any more.

	bps.ClearAllBreakpoints();
    dbg.Restart();
	EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );

	CString strSource = "bp__ulul.c";

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);

	EXPECT_TRUE( src.Find("ul = (ulong)counter;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "ul = (ulong)counter;") );

	EXPECT_TRUE( dbg.StepOut(1, NULL, "ul = CFncULUL(ul);", "TestBP()") );

	return (TRUE);
}


BOOL Cexe_baseIDETest::StepOverSource(   )
{
 	LogTestHeader( "StepOverSource" );

    EXPECT_TRUE( dbg.Go(NULL,NULL,NULL,WAIT_FOR_TERMINATION) );

	CString strSource = "testbp.c";
	bps.ClearAllBreakpoints();

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);

	EXPECT_TRUE( src.Find("ul = CFncULUL(ul);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "ul = CFncULUL(ul);") );

	EXPECT_TRUE( dbg.StepOver(1, NULL, "f = CFncFF(f);") );

	return (TRUE);
}


BOOL Cexe_baseIDETest::StepIntoSource(   )
{
 	LogTestHeader( "StepIntoSource" );

	bps.ClearAllBreakpoints();
    dbg.Restart();

	if( !UIWB.GetPcodeSwitch() )
	{
	    EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );
		EXPECT_TRUE( dbg.StepInto() );
	}

    EXPECT_TRUE( dbg.CurrentLineIs( "int iBP = TestBP();" ) );

	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "TestBP()") );

	MST.DoKeys("{up}");
    EXPECT_TRUE( dbg.CurrentLineIs( "int TestBP()" ) );

	EXPECT_TRUE( src.Find("s = CFncSSRecurse(10);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "s = CFncSSRecurse(10);") );

	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "CFncSSRecurse") );

	MST.DoKeys("{up}");
    EXPECT_TRUE( dbg.CurrentLineIs( "CFncSSRecurse(short NTimes)" ) );

	EXPECT_TRUE( src.Find("NTimes = (short)CFncSSRecurse((short)(NTimes-1));") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "NTimes = (short)CFncSSRecurse((short)(NTimes-1));") );

	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "CFncSSRecurse") );
	EXPECT_TRUE( stk.FunctionIs("CFncSSRecurse",1) );

	MST.DoKeys("{up}");
    EXPECT_TRUE( dbg.CurrentLineIs( "CFncSSRecurse(short NTimes)" ) );

	dbg.StopDebugging();

	return (TRUE);
}

BOOL Cexe_baseIDETest::StepToCursorSource(   )
{
 	LogTestHeader( "StepToCursorSource" );

	CString strSource = "bp__ldld.c";
	bps.ClearAllBreakpoints();

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);

	EXPECT_TRUE( src.Find("ul = (ulong)counter;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "ul = (ulong)counter;") );

	EXPECT_TRUE( src.Find("d  = (double)counter;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "d  = (double)counter;") );
	dbg.StopDebugging();
	return (TRUE);
}


/*
** A number of tests lumped together.  Some involve step/trace/stepout
** with different level of debug information or without debug 
** information.  
** 
*/

BOOL Cexe_baseIDETest::StepModel(   )
{
 	LogTestHeader( "StepModel" );

	int count;
	BOOL fOkay;

	/*
	** test Step_Out of func with debug info into func w/out debug info	
	** also test to Step_Out of func w/out debug info into func w/ partial info
	** also test to Step_Out of func w/partial debug info into func w/full info
	** also test to Step_Out of func with debug info into func w/debug info
	**
	** ChainMain is expected to be compiled with full debug information
	** chain_1 is expected to be compiled with full debug information
	** chain_2 is expected to be compiled with partial debug information
	** chain_3 is expected to be compiled without debug information
	** chain_4 is expected to be compiled with full debug information
	*/

	dbg.Restart();

	/* 
	** test tracing into function with partial debug information.
	*/
 
	bps.SetBreakpoint("{chain_1}START");
	dbg.Go();
	dbg.AtSymbol("START");
	fOkay = dbg.StepInto(1,NULL, NULL, "chain_2");
	WriteLog( fOkay ? PASSED : FAILED, "Trace from full cvinfo into module w/only partial cvinfo");
	if (!fOkay)
		return fOkay;	
 
	/* 
	** test tracing out of function w/out cvinfo into func w/ partial.
	** ; test instruction level tracing
	*/

	EXPECT_TRUE ( bps.SetBreakpoint(_CHAIN_3) != NULL );
	EXPECT_TRUE ( dbg.Go(NULL, NULL, "chain_3") );
	EXPECT_TRUE ( dbg.StepInto(2, NULL, NULL, "chain_3") );
	EXPECT_TRUE ( bps.DisableAllBreakpoints() );

	count=0;
	while ( (count<25) && (stk.GetFunction().Find("chain_3") != -1))
	{
		count++;
		dbg.StepOver();   // step over chain_4 ...
	} 
	fOkay = stk.CurrentFunctionIs("chain_2");
	WriteLog( fOkay ? PASSED : FAILED, "instruction level tracing from no cvinfo to partial cvinfo");
	if (!fOkay)
		return fOkay;	

	dbg.SetSteppingMode(SRC);

 	/* 
	** test tracing into function w/out cvinfo
	*/

	bps.ClearAllBreakpoints();
	EXPECT_TRUE ( bps.SetBreakpoint("{WndProc}TagCallNoCVInfo") != NULL );

	dbg.Go();   // will hit the throw
	dbg.Go(); 	// go again to get to the breakpoint
	dbg.AtSymbol("TagCallNoCVInfo");
	dbg.SetSteppingMode(SRC);

	// attempt to trace into func, but func doesn't have cvinfo so 
	// trace should actually stepover
	fOkay = dbg.StepInto(1, NULL, NULL, "WndProc");
	WriteLog(fOkay ? PASSED : FAILED, "Source level trace of func call w/out cvinfo");
	if (!fOkay)
		return FALSE;	

	src.Find("TagCallNoCVInfo:", TRUE);
	if (dbg.SetNextStatement(NULL) == FALSE)
	{
		m_pLog->RecordFailure("Unable to Set_Next_Statement");
		return FALSE;	
	}
	if (dbg.SetSteppingMode(ASM) == FALSE)
	{
		m_pLog->RecordFailure("Unable to set stepping mode to ASM");
		return FALSE;	// ( FALSE, GetTestDesc() );
	}

	// don't know exactly how many instructions so allow a few traces 
	count=0;
	do 
	{
		count++;
		dbg.StepInto();
	} while( (count<17) && (!dbg.AtSymbol(_CHAIN_3)) );
	EXPECT_TRUE ( dbg.AtSymbol(_CHAIN_3) );
	// now only have dissassembly.  keep tracing until get back to mixed.
	do 
	{
		dbg.StepInto();
	} while (!dbg.AtSymbol("chain_4"));
	fOkay = dbg.AtSymbol("chain_4");
	WriteLog(fOkay ? PASSED : FAILED, "Instruction level trace from no cvinfo to full cvinfo");
	
	dbg.SetSteppingMode(SRC);
	bps.ClearAllBreakpoints();
	return TRUE;	
}

BOOL Cexe_baseIDETest::StaticFuncStepTrace( )
{
 	LogTestHeader( "StaticFuncStepTrace" );

	bps.ClearAllBreakpoints();

    dbg.Restart();
    EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );
	
 	LogTestHeader( "SRC: Trace into Static member function and trace out" );

	EXPECT_TRUE( src.Find("try_array(iA);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "try_array(iA);") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "try_array(Array<int>") );
	EXPECT_TRUE( dbg.StepOver(4, NULL, "iA.static_func(mid);") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "Array<int>::static_func") );
	EXPECT_TRUE( dbg.StepInto(3, NULL, NULL, "try_array(Array<int>") );

	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main") );

	// repeat for double type of the template function
	EXPECT_TRUE( src.Find("try_array(dA);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "try_array(dA);") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "try_array(Array<double>") );
	EXPECT_TRUE( dbg.StepOver(4, NULL, "iA.static_func(mid);") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "Array<double>::static_func") );
	EXPECT_TRUE( dbg.StepInto(3, NULL, NULL, "try_array(Array<double>") );

	// Once more with a nested call
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "iA.static_func(iA.getSize());") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "Array<int>::getSize") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "main") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "Array<int>::static_func") );
	EXPECT_TRUE( dbg.StepInto(3, NULL, NULL, "main") );

	// Step over static member function
	EXPECT_TRUE( dbg.StepOver(1, NULL, "dA.static_func(iA.getSize());") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "int iGo = TestG();", "main") );

	// foo calls foo_Include
	EXPECT_TRUE( src.Find("Int=foo(Local);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int=foo(Local);", "TestG") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "foo") );
	EXPECT_TRUE( dbg.StepInto(2, NULL, NULL, "foo_Include") );
	EXPECT_TRUE( dbg.StepInto(5, NULL, NULL, "foo") );
	EXPECT_TRUE( dbg.StepInto(3, NULL, NULL, "TestG") );
	EXPECT_TRUE( dbg.StepInto(2, NULL, NULL, "foo_Include") );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "TestG") );

	// Step over static function
	EXPECT_TRUE( dbg.StepOver(3, NULL, "nest_func(foo_Include(foo(Global)));") );

	// trace again through nested calls of static functions
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "foo") );
	EXPECT_TRUE( dbg.StepInto(2, NULL, NULL, "foo_Include") );
	EXPECT_TRUE( dbg.StepInto(5, NULL, NULL, "foo") );
	EXPECT_TRUE( dbg.StepInto(3, NULL, NULL, "TestG") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "foo_Include") );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "TestG") );

	// Test a couple situations in Disassmebly
    dbg.Restart();
    EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );
	
 	LogTestHeader( "DAM: Trace into Static member function and trace out" );

	EXPECT_TRUE( src.Find("try_array(iA);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "try_array(iA);") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, NULL, "try_array(Array<int>") );
	EXPECT_TRUE( dbg.StepOver(4, NULL, "iA.static_func(mid);") );
	dbg.SetSteppingMode(ASM);

	//trace in
	int count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("Array<int>::static_func")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( stk.CurrentFunctionIs("Array<int>::static_func") );

	//trace out
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("try_array(Array<int>")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( stk.CurrentFunctionIs("try_array(Array<int>") );

	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, "main") );
	dbg.SetSteppingMode(SRC);

	// Step over static member function in DAM
 	LogTestHeader( "DAM: Step Over Static member function" );
	EXPECT_TRUE( src.Find("dA.static_func(iA.getSize());") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "dA.static_func(iA.getSize());") );
	dbg.SetSteppingMode(ASM);
	count=0;
	while ( (count<25) && (!dbg.CurrentLineIs( "static_func" )) )
	{
		count++;
		dbg.StepOver();
		Sleep(100);
	} 
	EXPECT_TRUE ( dbg.CurrentLineIs( "static_func" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, "main") );

	dbg.SetSteppingMode(SRC);

 	LogTestHeader( "DAM: Step Over simple Static function" );
	EXPECT_TRUE( src.Find("Int=foo(Local);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int=foo(Local);") );
	dbg.SetSteppingMode(ASM);
	count=0;
	while ( (count<25) && (!dbg.CurrentLineIs( "foo" )) )
	{
		count++;
		dbg.StepOver();
		Sleep(100);
	} 
	EXPECT_TRUE ( dbg.CurrentLineIs( "foo" ) );
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, "TestG") );

	dbg.SetSteppingMode(SRC);

 	LogTestHeader( "DAM: Trace into simple Static function and trace out" );

	EXPECT_TRUE( src.Find("nest_func(foo_Include(foo(Global)));") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "nest_func(foo_Include(foo(Global)));") );
	dbg.SetSteppingMode(ASM);

	//trace from TestG into foo
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("foo")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( stk.CurrentFunctionIs("foo") );

	//trace from foo into foo_Include
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("foo_Include")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( stk.CurrentFunctionIs("foo_Include") );

	//trace out	from foo_Include back to foo
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("foo")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( stk.CurrentFunctionIs("foo") );

	//trace out	from foo back to TestG
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("TestG")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( stk.CurrentFunctionIs("TestG") );

	//trace from TestG into foo_Include
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("foo_Include")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( stk.CurrentFunctionIs("foo_Include") );

	//trace out	from foo_Include back to TestG
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("TestG")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( stk.CurrentFunctionIs("TestG") );

	dbg.StopDebugging();

	return TRUE;	
}

