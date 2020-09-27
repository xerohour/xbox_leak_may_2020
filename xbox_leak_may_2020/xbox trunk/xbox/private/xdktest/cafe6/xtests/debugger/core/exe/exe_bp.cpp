///////////////////////////////////////////////////////////////////////////////
//	exe_bp.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include <process.h>
#include "stdafx.h"
#include "exe_bp.h"	

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(Cexe_bpIDETest, CDbgTestBase, "Execution and Breakpoints", -1, CexeSubSuite)
												 
void Cexe_bpIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void Cexe_bpIDETest::Run()
{	 
	XSAFETY;
	
	if( SetProject("testgo01\\testgo01", PROJECT_XBE))
	{	  
		EXPECT_TRUE(bps.ClearAllBreakpoints());
		XSAFETY;
		StepOverFuncToBreakAtLocBP();
		XSAFETY;
		StepOverFuncToBreakOnExprTrueBP();
		XSAFETY;
		StepOverFuncToBreakOnExprChangesBP();
		XSAFETY;
		StepBPLineSRC();
		XSAFETY;
		StepBPLineDAM();
		XSAFETY;
	}
		
	/* For the dbg project:
	** ChainMain is expected to be compiled with full debug information
	** chain_1 is expected to be compiled with full debug information
	** chain_2 is expected to be compiled with partial debug information
	** chain_3 is expected to be compiled without debug information
	** chain_4 is expected to be compiled with full debug information
	*/
	if (0) //xbox TODO SetProject("dbg\\dbg") )
	{
		EXPECT_TRUE(bps.ClearAllBreakpoints());
		XSAFETY;
		EXPECT(BreakStepTrace());
		XSAFETY;
	}
	StopDbgCloseProject();
	Sleep(1000);
	
}


///////////////////////////////////////////////////////////////////////////////
//	Test Cases

/*
** A number of tests lumped together.  Some involve different level of debug
** information or without debug information.  Also a test or two
** involving step/stepout and breakpoints which are important enough
** to have in the stepping model tests.
*/

BOOL Cexe_bpIDETest::BreakStepTrace(   )
{ 	
 	LogTestHeader( "BreakStepTrace" );

 	/*
	** test Step_Out of func which fires a bp in a nested func in secondary module.
	** test to Step_Out of func with debug info into func w/out debug info	
	** also test to Step_Out of func w/out debug info into func w/ partial info
	** also test to Step_Out of func w/partial debug info into func w/full info
	** also test to Step_Out of func with debug info into func w/debug info
	*/

	bp *bp;
	BOOL fSuccess;
	
	dbg.StopDebugging();
	bp=bps.SetBreakpoint("WndProc");
	dbg.Go();  								// execute to function WndProc 
	EXPECT_TRUE ( dbg.VerifyAtSymbol("WndProc") );
	// Step past prolog
	dbg.StepOver(); 						
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// set breakpoint on label
	bp = bps.SetBreakpoint("TagCallChainMain"); 	
	// execute to bp at label
	dbg.Go();  								
	EXPECT_TRUE ( dbg.AtSymbol("{WndProc}TagCallChainMain") );
	EXPECT_TRUE ( dbg.StepInto(1,"ChainMain") );

	/* 
	** test tracing out of static function.
	** test Step_Out of func which fires a bp in a nested func in 
	** secondary module.
	*/

	EXPECT_TRUE ( bps.SetBreakpoint("chain_4") != NULL );
	EXPECT_TRUE ( bps.SetBreakpoint("chain_5") != NULL );
	dbg.Go();
	EXPECT_TRUE ( dbg.AtSymbol("chain_4") );

	// attempt to step out but fire on breakpoint in nested function call
	// proper location is chain_5
	fSuccess = dbg.StepOut(1, NULL, NULL, "chain_5"); 
	WriteLog( fSuccess ? PASSED : FAILED, "StepOut and fire on breakpoint in nested function");
	if (!fSuccess)
		return fSuccess;			
	
	dbg.StepInto();
	EXPECT_TRUE ( fSuccess &= dbg.StepOut(1, NULL, NULL, "chain_4") );

	EXPECT_TRUE ( fSuccess &= dbg.StepOut(1, NULL, NULL, "chain_3") );
	WriteLog( fSuccess ? PASSED : FAILED, "StepOut from full to none cvinfo");

	fSuccess = dbg.StepOut(1, NULL, NULL, "chain_2");  
	WriteLog( fSuccess ? PASSED : FAILED, "StepOut from none to partial cvinfo");

	if (!fSuccess)
		return fSuccess;	
	fSuccess = dbg.StepOut(1, NULL, NULL, "chain_1");  
	WriteLog( fSuccess ? PASSED : FAILED, "StepOut from partial cvinfo to full cvinfo");

	if (!fSuccess)
		return fSuccess;	
	fSuccess = dbg.StepOut(1, NULL, NULL, "ChainMain"); 
	WriteLog( fSuccess ? PASSED : FAILED, "StepOut from full cvinfo to full cvinfo");
	if (!fSuccess)
		return fSuccess;	
/*	
	// attempt to step over function but which generates an exception.
	// ToDo: Yefims: this should go to NLG test
	bps.SetBreakpoint("{OtherMain}TagStepEH");
	dbg.Go();
	dbg.StepOver();
#if defined( _MIPS_ ) || defined(_ALPHA_)
	EXPECT_TRUE(stk.CurrentFunctionIs("NTDLL!"));
#else
	EXPECT_TRUE(stk.CurrentFunctionIs("KERNEL32!"));
#endif
	dbg.StepOut();
	dbg.StepOut();
	//TODO: where should we be?
 */
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	return TRUE;	
}									


BOOL Cexe_bpIDETest::StepOverFuncToBreakAtLocBP()
{
 	LogTestHeader( "StepOverFuncToBreakAtLocBP" );

	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");

	EXPECT_TRUE( src.Find("Int=foo(Local);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int=foo(Local);") );
	EXPECT_TRUE( src.Find("return Parameter;") );
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "return Parameter;") );

	EXPECT_TRUE( src.Find("nest_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "nest_func();") );
	EXPECT_TRUE( bps.SetBreakpoint("func") != NULL );
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, "func") );

	EXPECT_TRUE(bps.ClearAllBreakpoints());
	dbg.StopDebugging();
	return (TRUE);
}

BOOL Cexe_bpIDETest::StepOverFuncToBreakOnExprTrueBP()
{
 	LogTestHeader( "StepOverFuncToBreakOnExprTrueBP" );

	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");

	EXPECT_TRUE( src.Find("Int = foo_Include(Int);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int = foo_Include(Int);") );
	EXPECT_TRUE( bps.SetBreakOnExpr("{foo_Include}number==34", COBP_TYPE_IF_EXP_TRUE) != NULL );
	dbg.StepOver(1, NULL, NULL, NULL, 0);
	if( WaitMsgBoxText("Break when '{foo_Include}number==34'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{foo_Include}number==34'\" was found as expected" );
		// REVIEW(michma - 6/3/99): there is this weird problem on w2k where the data breakpoint hit msg box
		// isn't active right away. doesn't repro manually, only when performed through cafe.
		// so we need to reactivate the IDE for the OK button to become active so we can dismiss the dlg.
		UIWB.Activate();
		MST.WButtonClick( "OK" );
		while(MST.WButtonExists("OK"));
		EXPECT_TRUE_COMMENT( TRUE, "BreakOnExprTrue" );	 	
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{foo_Include}number==34'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "if(number == 3)" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("number == 34",1) );

	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");
	EXPECT_TRUE( src.Find("Int=recur_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int=recur_func();") );
	EXPECT_TRUE( bps.SetBreakOnExpr("{recur_func}x==2", COBP_TYPE_IF_EXP_TRUE) != NULL );
	dbg.StepOver(1, NULL, NULL, NULL, 0);
	if( WaitMsgBoxText("Break when '{recur_func}x==2'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{recur_func}x==2'\" was found as expected" );
		// REVIEW(michma - 6/3/99): there is this weird problem on w2k where the data breakpoint hit msg box
		// isn't active right away. doesn't repro manually, only when performed through cafe.
		// so we need to reactivate the IDE for the OK button to become active so we can dismiss the dlg.
		UIWB.Activate();
		MST.WButtonClick( "OK" );
		while(MST.WButtonExists("OK"));
		EXPECT_TRUE_COMMENT( TRUE, "BreakOnExprTrue" );	 	
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{recur_func}x==2'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "if (x<9) recur_func();" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("x==2",1) );
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// TODO(michma): we need to do this extra step because of bug 38172.
	EXPECT_TRUE(dbg.StepInto());

	dbg.StopDebugging();
	return (TRUE);
}

BOOL Cexe_bpIDETest::StepOverFuncToBreakOnExprChangesBP()
{
 	LogTestHeader( "StepOverFuncToBreakOnExprChangesBP" );

	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");
	EXPECT_TRUE( src.Find("Int=recur_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int=recur_func();") );
	EXPECT_TRUE( bps.SetBreakOnExpr("{recur_func}x", COBP_TYPE_IF_EXP_CHANGED) != NULL );
	dbg.StepOver(1, NULL, NULL, NULL, 0);
	if( WaitMsgBoxText("Break when '{recur_func}x'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{recur_func}x'\" was found as expected" );
		// REVIEW(michma - 6/3/99): there is this weird problem on w2k where the data breakpoint hit msg box
		// isn't active right away. doesn't repro manually, only when performed through cafe.
		// so we need to reactivate the IDE for the OK button to become active so we can dismiss the dlg.
		UIWB.Activate();
		MST.WButtonClick( "OK" );
		while(MST.WButtonExists("OK"));
		EXPECT_TRUE_COMMENT( TRUE, "BreakOnExprChanged" );	 	
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{recur_func}x'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "if (x<9) recur_func();" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("x==2",1) );
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// TODO(michma): we need to do this extra step because of bug 38172.
	EXPECT_TRUE(dbg.StepInto());

	dbg.StopDebugging();
	return (TRUE);
}

BOOL Cexe_bpIDETest::StepBPLineSRC()
{
 	LogTestHeader( "StepBPLineSRC" );

 	LogTestHeader( "StepOver to BPLine: foo_Include" );
	UIWB.OpenFile( m_strProjectDir + "\\" + "testg.h");
	EXPECT_TRUE( src.Find("if(number == 3)") );
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	UIBreakpoints bpd;
	bpd.Activate();
	EXPECT_TRUE_COMMENT ( bpd.BreakpointIsSet("26", "testg.h"), "BP is set at line 26 in testg.h" );
	bpd.Close();
	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");
	EXPECT_TRUE( src.Find("foo_Include(Int);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "foo_Include(Int);") );
	EXPECT_TRUE( dbg.StepInto(2, NULL, "number*=2;") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "if(number == 3)") );
	bpd.Activate();
	EXPECT_TRUE_COMMENT ( bpd.BreakpointIsSet("26", "testg.h"), "BP is set at line 26 in testg.h" );
	bpd.Close();
	EXPECT_TRUE(bps.ClearAllBreakpoints());

 	LogTestHeader( "StepOut to BPLine" );
	CString strLine = "146";
	CString strLocation = (CString)"{,testgo.c,}@" + strLine;
	EXPECT_TRUE( bps.SetBreakpoint(strLocation) != NULL );
	bpd.Activate();
	EXPECT_TRUE( bpd.BreakpointIsSet(strLine, "testgo.c") );
	bpd.Close();
	EXPECT_TRUE( dbg.StepOut(1, NULL, "foo_Include(Int);") );
	bpd.Activate();
	EXPECT_TRUE( bpd.BreakpointIsSet(strLine, "testgo.c") );
	bpd.Close();
	EXPECT_TRUE(bps.ClearAllBreakpoints());

 	LogTestHeader( "StepInto to BPLine: recur_func" );
	EXPECT_TRUE( src.Find("if (x<9) recur_func();") );
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	bpd.Activate();
	EXPECT_TRUE_COMMENT ( bpd.BreakpointIsSet("56", "testgo.c"), "BP is set at line 56 in testgo.c" );
	bpd.Close();
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Int=recur_func();") );
	EXPECT_TRUE( dbg.StepInto(3, NULL, "if (x<9) recur_func();") );
	strLine = "56";
	bpd.Activate();
	EXPECT_TRUE_COMMENT ( bpd.BreakpointIsSet(strLine, "testgo.c"), "BP is set at line 56 in testgo.c" );
	bpd.Close();
	EXPECT_TRUE(bps.ClearAllBreakpoints());

 	LogTestHeader( "StepToCursor to BPLine: TestG" );
	EXPECT_TRUE( src.Find("nest_func();") );
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	bpd.Activate();
	EXPECT_TRUE_COMMENT ( bpd.BreakpointIsSet("150", "testgo.c"), "BP is set at line 150 in testgo.c" );
	bpd.Close();
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "nest_func();") );
	strLine = "150";
	bpd.Activate();
	EXPECT_TRUE ( bpd.BreakpointIsSet(strLine, "testgo.c") );
	bpd.Close();
	EXPECT_TRUE(bps.ClearAllBreakpoints());

	dbg.StopDebugging();
	return (TRUE);
}

BOOL Cexe_bpIDETest::StepBPLineDAM()
{
 	LogTestHeader( "StepBPLineDAM" );
	UIBreakpoints bpd;

 	LogTestHeader( "StepOver to BPLine" );
	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");
	EXPECT_TRUE( src.Find("foo_Include(Int);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "foo_Include(Int);") );
	EXPECT_TRUE( dbg.StepInto(2, NULL, "number*=2;") );
	dbg.SetSteppingMode(ASM);
	MST.DoKeys("{down 2}");
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	bpd.Activate();
	EXPECT_TRUE ( bpd.BreakpointIsSet("0x") );
	bpd.Close();
	EXPECT_TRUE( dbg.StepOver(2) );
	bpd.Activate();
	EXPECT_TRUE ( bpd.BreakpointIsSet("0x") );
	bpd.Close();
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_TRUE( dbg.StepOut(1) );

 	LogTestHeader( "StepInto to BPLine" );
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE( src.Find("if (x<9) recur_func();") );
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	bpd.Activate();
	EXPECT_TRUE_COMMENT ( bpd.BreakpointIsSet("56", "testgo.c"), "BP is set at line 56 in testgo.c" );
	bpd.Close();
	EXPECT_TRUE( dbg.StepOver(1, NULL, "Int=recur_func();") );
	EXPECT_TRUE( dbg.StepInto(2, NULL, "x++;") );
	dbg.SetSteppingMode(ASM);
	EXPECT_TRUE( dbg.StepInto(3) );
	CString strLine = "56";
	bpd.Activate();
	EXPECT_TRUE ( bpd.BreakpointIsSet(strLine, "testgo.c") );
	bpd.Close();
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	MST.DoKeys("{down}");
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	bpd.Activate();
	EXPECT_TRUE ( bpd.BreakpointIsSet("0x") );
	bpd.Close();
	EXPECT_TRUE( dbg.StepInto(1) );
	bpd.Activate();
	EXPECT_TRUE ( bpd.BreakpointIsSet("0x") );
	bpd.Close();
	EXPECT_TRUE(bps.ClearAllBreakpoints());

 	LogTestHeader( "StepToCursor to BPLine" );
	dbg.SetSteppingMode(SRC);
	EXPECT_TRUE( src.Find("nest_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "nest_func();") );
	dbg.SetSteppingMode(ASM);
	MST.DoKeys("{down 3}");
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	bpd.Activate();
	EXPECT_TRUE ( bpd.BreakpointIsSet("0x") );
	bpd.Close();
	EXPECT_TRUE( dbg.StepToCursor(0) );
	bpd.Activate();
	EXPECT_TRUE ( bpd.BreakpointIsSet("0x") );
	bpd.Close();
	EXPECT_TRUE(bps.ClearAllBreakpoints());

	dbg.StopDebugging();
	return (TRUE);
}




