 
 /*
** STEP.CPP
**
**	Created by :			Date :
**		waltcr				8/14/94
**
**	Description :
**		Step/trace/go tests
*/


#include "stdafx.h"
#include "execase.h"
#include "execase2.h"

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

/*
** A number of tests lumped together.  Some involve different level of debug
** information or without debug information.  Also a test or two
** involving step/stepout and breakpoints which are important enough
** to have in the stepping model tests.
** 
*/

BOOL CexeIDETest::BreakStepTrace(   )
{ 	
 	LogTestHeader( "BreakStepTrace" );

 	/*
	** test Step_Out of func which fires a bp in a nested func in secondary module.
	** test to Step_Out of func with debug info into func w/out debug info	
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

	bp *bp;
	BOOL fSuccess;
	
	bps.ClearAllBreakpoints();
	dbg.StopDebugging();
	bp=bps.SetBreakpoint("WndProc");
	dbg.Go();  								// execute to function WndProc 
	EXPECT_TRUE ( dbg.VerifyAtSymbol("WndProc") );
	dbg.SetSteppingMode(SRC);
	// Step past prolog
	dbg.StepOver(); 						
	bps.ClearAllBreakpoints();
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

	// window already open, but it would be interesting to close and confirm 
	// dam is opened and gets focus, but that would be using whitebox testing 
	// which is considered voodoo.
	// should go back to chain_3, but wait till after next test to verify
	EXPECT_TRUE ( fSuccess &= dbg.StepOut(1, NULL, NULL, "chain_3") );

	// Proper location is w/in func chain_2
	fSuccess = dbg.StepOut(1, NULL, NULL, "chain_2");  
	WriteLog( fSuccess ? PASSED : FAILED, "StepOut from full to none and then none to partial cvinfo");
	if (!fSuccess)
		return fSuccess;	
	// proper location is w/in func chain_1
	fSuccess = dbg.StepOut(1, NULL, NULL, "chain_1");  
	WriteLog( fSuccess ? PASSED : FAILED, "StepOut from partial cvinfo to full cvinfo");
	if (!fSuccess)
		return fSuccess;	
	// proper location is chainmain
	fSuccess = dbg.StepOut(1, NULL, NULL, "ChainMain"); 
	WriteLog( fSuccess ? PASSED : FAILED, "StepOut from full cvinfo to full cvinfo");
	if (!fSuccess)
		return fSuccess;	
	
	// attempt to step over function but which generates an exception.
	
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

	return TRUE;	
}									


/*
** A number of tests lumped together.  Some involve step/trace/stepout
** with different level of debug information or without debug 
** information.  
** 
*/

BOOL CexeIDETest::StepModel(   )
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
	// CAFE REVIEW: the stepinto and check of wndproc is not returning 
	// focus to src window.  should it?  For now next line sets it back.
	dbg.SetSteppingMode(SRC);
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


/*
** Stepping model test(s) involving DLL's.
** Step Loadlibrary and then trace into the virtual explicit dll.
**
*/ 

BOOL Cexe2IDETest::DllStepTrace(   )
{
 	LogTestHeader( "DllStepTrace" );

	BOOL fSuccess;
	int count;

	/* 
	** test step/trace of LoadLibrary
	** (for portability reasons this needs to be isolated)
	*/
	dbg.Restart();
	bps.ClearAllBreakpoints();
	EXPECT_TRUE ( fSuccess = (bps.SetBreakpoint("{Dec,,dec2.dll}TagLoadLib") != NULL) );
	EXPECT_TRUE ( fSuccess &= dbg.Go("{Dec,,dec2.dll}TagLoadLib") );
	dbg.StepInto();  // step load library call which does a symbol load
	// REVIEW:  is there anything interesting to verify?
	EXPECT_TRUE( fSuccess &= (dbg.VerifyNotification("Loaded symbols", 3, 20, TRUE) && dbg.VerifyNotification("powdll.dll", 3, 20, TRUE)) );

	/* 
	** test tracing into function contained in a virtual dll. (for 
	** portability reasons this needs to be isolated)
	*/
	count=0;
	while ( (count<10) && (!dbg.AtSymbol("{Dec,,dec2.dll}TagCallPow")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE(fSuccess &= (dbg.AtSymbol("TagCallPow") && stk.CurrentFunctionIs("Dec")) );
	EXPECT_TRUE(fSuccess &= dbg.StepInto(1, NULL, NULL, "dPow"));

	dbg.StopDebugging();
	EXPECT_TRUE ( fSuccess &= dbg.Go("{Dec,,dec2.dll}TagLoadLib") );
	dbg.StepOver();  // step over load library call which does a symbol load
	EXPECT_TRUE( fSuccess &= (dbg.VerifyNotification("Loaded symbols", 3, 20, TRUE) && dbg.VerifyNotification("powdll.dll", 3, 20, TRUE)) );

	dbg.StopDebugging();
	EXPECT_TRUE(fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}TagCallPow") != NULL) );
	dbg.Go();
	EXPECT_TRUE ( fSuccess &= dbg.Go("{Dec,,dec2.dll}TagCallPow") );
	EXPECT_TRUE(fSuccess &= (bps.SetBreakpoint("dPow") != NULL));
	EXPECT_TRUE ( fSuccess &= dbg.StepOver(1, "dPow") );
	return fSuccess;	
}

BOOL Cexe2IDETest::DllStepOut(   )
{
 	LogTestHeader( "DllStepOut" );
	BOOL fSuccess = TRUE;
	dbg.Restart();
	bps.ClearAllBreakpoints();
	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}Dec") != NULL ));
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, NULL, "Dec") );
	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}@54") != NULL ));
	
	// StepOut of implibed DLL into DLL
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, "Inc(wBar);") );
	EXPECT_TRUE ( fSuccess &= dbg.StepInto(1, NULL, NULL, "Inc") ); 
	EXPECT_TRUE_COMMENT ( fSuccess &= dbg.StepOut(1, NULL, NULL, "Dec"), "StepOut of implibed DLL into DLL (source mode)" ); 

	// StepOut of not implibed DLL into DLL
	int count=0;
	while ( (count<25) && (!dbg.AtSymbol("{Dec,,dec2.dll}TagCallPow")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE(fSuccess &= (dbg.AtSymbol("TagCallPow") && stk.CurrentFunctionIs("Dec")));
	EXPECT_TRUE(fSuccess &= dbg.StepInto(1, NULL, NULL, "dPow"));
	EXPECT_TRUE_COMMENT ( fSuccess &= dbg.StepOut(1, NULL, NULL, "Dec"), "StepOut of not implibed DLL into DLL (source mode)" ); 

	// StepOut of implibed DLL into EXE
	EXPECT_TRUE_COMMENT ( fSuccess &= dbg.StepOut(1, NULL, NULL, "MainWndProc"), "StepOut of implibed DLL into EXE (source mode)" ); 

	// ASM mode
	dbg.Restart();
	dbg.SetSteppingMode(ASM);
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, NULL, "Dec") );

	// StepOut of implibed DLL into DLL
	EXPECT_TRUE_COMMENT ( fSuccess &= dbg.Go(NULL, NULL, "Dec"), "At line 54" );
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("Inc")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( fSuccess &= stk.CurrentFunctionIs("Inc") );
	EXPECT_TRUE ( fSuccess &= dbg.StepInto(3, NULL, NULL, "Inc") ); 
	EXPECT_TRUE_COMMENT ( fSuccess &= dbg.StepOut(1, NULL, NULL, "Dec"), "StepOut of implibed DLL into DLL (mixed mode)" ); 

	// StepOut of not implibed DLL into DLL
	EXPECT_TRUE(fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}TagCallPow") != NULL));
	dbg.Go();
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("dPow")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE ( fSuccess &= stk.CurrentFunctionIs("dPow") );
	EXPECT_TRUE ( fSuccess &= dbg.StepInto(2, NULL, NULL, "dPow") ); 
	EXPECT_TRUE_COMMENT ( fSuccess &= dbg.StepOut(1, NULL, NULL, "Dec"), "StepOut of not implibed DLL into DLL (mixed mode)" ); 

	// StepOut of implibed DLL into EXE
	EXPECT_TRUE_COMMENT ( fSuccess &= dbg.StepOut(1, NULL, NULL, "MainWndProc"), "StepOut of implibed DLL into EXE (mixed mode)" ); 

	dbg.StopDebugging();
	return fSuccess;	
}

BOOL Cexe2IDETest::DllTraceOut(   )
{
 	LogTestHeader( "DllTraceOut" );
	BOOL fSuccess = TRUE;
	dbg.Restart();
	bps.ClearAllBreakpoints();
	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}Dec") != NULL ));
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, NULL, "Dec") );
	
	// TraceOut of implibed DLL into DLL
	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}@54") != NULL ));
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, "Inc(wBar);") );
	EXPECT_TRUE ( fSuccess &= dbg.StepInto(1, NULL, NULL, "Inc") ); 
	int count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("Dec")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE_COMMENT ( fSuccess &= stk.CurrentFunctionIs("Dec"), "TraceOut of implibed DLL into DLL" );

	// TraceOut of not implibed DLL into DLL
	count=0;
	while ( (count<25) && (!dbg.AtSymbol("{Dec,,dec2.dll}TagCallPow")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE(fSuccess &= (dbg.AtSymbol("TagCallPow") && stk.CurrentFunctionIs("Dec")));
	EXPECT_TRUE(fSuccess &= dbg.StepInto(1, NULL, NULL, "dPow"));
	count=0;
	while ( (count<25) && (!stk.CurrentFunctionIs("Dec")))
	{
		count++;
		dbg.StepInto();
		Sleep(100);
	} 
	EXPECT_TRUE_COMMENT ( fSuccess &= stk.CurrentFunctionIs("Dec"), "TraceOut of not implibed DLL into DLL" );

	dbg.StopDebugging();
	return fSuccess;	
}

BOOL Cexe2IDETest::GoFromWithinDll(   )
{
 	LogTestHeader( "GoFromWithinDll" );
	BOOL fSuccess = TRUE;
	dbg.Restart();
	bps.ClearAllBreakpoints();

	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}Dec") != NULL ));
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, NULL, "Dec") );
	UIWB.OpenFile( m_strProjectDir + "\\" + "dllappx.c");
	EXPECT_TRUE( fSuccess &= src.Find("BPWithLengthTest();") );
	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint() != NULL ));
	EXPECT_TRUE( dbg.Go(NULL, "BPWithLengthTest();", "MainWndProc") );
	bps.ClearAllBreakpoints();
	dbg.Restart();

	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint("{Inc,,incdll.dll}Inc") != NULL ));
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, NULL, "Inc") );
	UIWB.OpenFile( m_strProjectDir + "\\" + "dec2.c");
	EXPECT_TRUE( fSuccess &= src.Find("dValue = dpfnPow( 10, 3 );") );
	EXPECT_TRUE( fSuccess &= dbg.StepToCursor(0, NULL, "dValue = dpfnPow( 10, 3 );") );
	
	EXPECT_TRUE(fSuccess &= (dbg.AtSymbol("TagCallPow") && stk.CurrentFunctionIs("Dec")));
	EXPECT_TRUE(fSuccess &= dbg.StepInto(2, NULL, NULL, "dPow"));
	UIWB.OpenFile( m_strProjectDir + "\\" + "dllappx.c");
	EXPECT_TRUE( fSuccess &= src.Find("BPWithLengthTest();") );
	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint() != NULL ));
	EXPECT_TRUE( fSuccess &= dbg.Go(NULL, "BPWithLengthTest();", "MainWndProc") );

	dbg.StopDebugging();
	return fSuccess;	
}

BOOL Cexe2IDETest::RestartFromWithinDll(   )
{
 	LogTestHeader( "RestartFromWithinDll" );
	BOOL fSuccess = TRUE;
	dbg.Restart();
	bps.ClearAllBreakpoints();

	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}Dec") != NULL ));
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, NULL, "Dec") );
	EXPECT_TRUE( fSuccess &= dbg.Restart() );
	bps.ClearAllBreakpoints();

	EXPECT_TRUE ( fSuccess &= (bps.SetBreakpoint("{Inc,,incdll.dll}Inc") != NULL ));
	EXPECT_TRUE ( fSuccess &= dbg.Go(NULL, NULL, "Inc") );
	EXPECT_TRUE( fSuccess &= dbg.Restart() );
	bps.ClearAllBreakpoints();
	
	UIWB.OpenFile( m_strProjectDir + "\\" + "dec2.c");
	EXPECT_TRUE( fSuccess &= src.Find("dValue = dpfnPow( 10, 3 );") );
	EXPECT_TRUE( fSuccess &= dbg.StepToCursor(0, NULL, "dValue = dpfnPow( 10, 3 );") );
	EXPECT_TRUE(fSuccess &= (dbg.AtSymbol("TagCallPow") && stk.CurrentFunctionIs("Dec")));
	EXPECT_TRUE(fSuccess &= dbg.StepInto(2, NULL, NULL, "dPow"));
	EXPECT_TRUE( fSuccess &= dbg.Restart() );

	dbg.StopDebugging();
	return fSuccess;	
}

BOOL Cexe2IDETest::StepOverFuncInUnloadedDll(   )
{
 	LogTestHeader( "StepOverFuncInUnloadedDll" );
	BOOL fSuccess = TRUE;
	dbg.Restart();
	bps.ClearAllBreakpoints();

	UIWB.OpenFile( m_strProjectDir + "\\" + "dllappx.c");
	EXPECT_TRUE( fSuccess &= src.Find("Dec( &wValue );") );
	EXPECT_TRUE( fSuccess &= dbg.StepToCursor(0, NULL, "Dec( &wValue );", "MainWndProc") );
	EXPECT_TRUE(fSuccess &= dbg.StepOver(1, NULL, NULL, "MainWndProc"));
	EXPECT_TRUE( fSuccess &= (dbg.VerifyNotification("Loaded symbols", 3, 20, TRUE) && dbg.VerifyNotification("powdll.dll", 3, 20, TRUE)) );

	dbg.StopDebugging();
	return fSuccess;	
}

// TODO ?

/* 
** attempt to step over function but which fires on a conditional bp.
*/


/* 
** test tracing constructor.
*/


/* 
** test tracing into function contained in a secondary module.
*/


/* 
** test tracing into function contained in a virtual c++ function.
*/


/* 
** test tracing into function through indirect call.
*/


/* 
** perform test Step_Out of main/winmain.  (is this really a core test?
*/


/* 
** test to Step_Out of func w/partial info into func w/out debug info
*/
