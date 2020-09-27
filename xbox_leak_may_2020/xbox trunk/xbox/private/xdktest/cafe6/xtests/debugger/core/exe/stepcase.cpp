///////////////////////////////////////////////////////////////////////////////
//	STEPCASE.CPP
//
//	Created by :			Date :
//		YefimS			01/06/94
//
//	Description :
//		Implementation of the CST1TestCases class
//

#include <process.h>
#include "stdafx.h"
#include "execase.h"
#include "execase2.h"

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CexeIDETest::StepOutSource(   )
{
 	LogTestHeader( "StepOutSource" );

	COSource		src;	// WinslowF - added to call GoToLine. It's not in UIWB any more.

	bps.ClearAllBreakpoints();
    dbg.Restart();
	if( UIWB.GetPcodeSwitch() )
	{
		EXPECT_TRUE( dbg.CurrentLineIs( "int iBP = TestBP();" ) );
	}
	else
		EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );

	CString strSource = "bp__ulul.c";

	// Open source
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource);

	EXPECT_TRUE( src.Find("ul = (ulong)counter;") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "ul = (ulong)counter;") );

	EXPECT_TRUE( dbg.StepOut(1, NULL, "ul = CFncULUL(ul);", "TestBP()") );

	return (TRUE);
}


BOOL CexeIDETest::StepOverSource(   )
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


BOOL CexeIDETest::StepIntoSource(   )
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

BOOL CexeIDETest::StepToCursorSource(   )
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

BOOL CexeIDETest::StepOverFuncToBreakAtLocBP()
{
 	LogTestHeader( "StepOverFuncToBreakAtLocBP" );
	bps.ClearAllBreakpoints();

	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");

	EXPECT_TRUE( src.Find("Int=foo(Local);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int=foo(Local);") );
	EXPECT_TRUE( src.Find("return Parameter;") );
	EXPECT_TRUE( bps.SetBreakpoint() != NULL );
	EXPECT_TRUE( dbg.StepOver(1, NULL, "return Parameter;") );

	EXPECT_TRUE( src.Find("nest_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "nest_func();") );
	EXPECT_TRUE( bps.SetBreakpoint("func") != NULL );
//	EXPECT_TRUE( dbg.StepOver(1, NULL, "int func() {") );
	EXPECT_TRUE( dbg.StepOver(1, NULL, NULL, "func") );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CexeIDETest::StepOverFuncToBreakOnExprTrueBP()
{
 	LogTestHeader( "StepOverFuncToBreakOnExprTrueBP" );
	bps.ClearAllBreakpoints();

	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");

	EXPECT_TRUE( src.Find("Int = foo_Include(Int);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int = foo_Include(Int);") );
	EXPECT_TRUE( bps.SetBreakOnExpr("{foo_Include}number==34", COBP_TYPE_IF_EXP_TRUE) != NULL );
	dbg.StepOver(1, NULL, NULL, NULL, 0);
	if( WaitMsgBoxText("Break when '{foo_Include}number==34'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{foo_Include}number==34'\" was found as expected" );
		MST.WButtonClick( "OK" ); 
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
		MST.WButtonClick( "OK" ); 
		EXPECT_TRUE_COMMENT( TRUE, "BreakOnExprTrue" );	 	
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{recur_func}x==2'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "if (x<9) recur_func();" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("x==2",1) );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CexeIDETest::StepOverFuncToBreakOnExprChangesBP()
{
 	LogTestHeader( "StepOverFuncToBreakOnExprChangesBP" );
	bps.ClearAllBreakpoints();

	UIWB.OpenFile( m_strProjectDir + "\\" + "testgo.c");
	EXPECT_TRUE( src.Find("Int=recur_func();") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "Int=recur_func();") );
	EXPECT_TRUE( bps.SetBreakOnExpr("{recur_func}x", COBP_TYPE_IF_EXP_CHANGED) != NULL );
	dbg.StepOver(1, NULL, NULL, NULL, 0);
	if( WaitMsgBoxText("Break when '{recur_func}x'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{recur_func}x'\" was found as expected" );
		MST.WButtonClick( "OK" ); 
		EXPECT_TRUE_COMMENT( TRUE, "BreakOnExprChanged" );	 	
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{recur_func}x'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "if (x<9) recur_func();" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("x==2",1) );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CexeIDETest::TraceIntoWndProc()
{
 	LogTestHeader( "TraceIntoWndProc" );
	bps.ClearAllBreakpoints();

	UIWB.OpenFile( m_strProjectDir + "\\" + "main2.c");
	EXPECT_TRUE( src.Find("SendMessage(") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "SendMessage(") );
//	EXPECT_TRUE( dbg.StepInto(1, NULL, "{", "WndProc") );
	EXPECT_TRUE( dbg.StepInto(1, NULL, "{") );
	EXPECT_TRUE( stk.CurrentFunctionIs("WndProc")	);
	EXPECT_TRUE( dbg.StepInto(1, NULL, "SAVELABEL(TagCallOtherMain);", "WndProc") );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL Cexe2IDETest::NoSystemCodeStepping()
{
 	LogTestHeader( "NoSystemCodeStepping" );
	bps.ClearAllBreakpoints();
	BOOL ret = TRUE;

	UIWB.OpenFile( m_strProjectDir + "\\" + "main2.c");
	EXPECT_TRUE( src.Find("SAVELABEL(TagCreateCase);") );
	EXPECT_TRUE( dbg.StepToCursor(0, NULL, "SAVELABEL(TagCreateCase);", "WndProc") );
	EXPECT_TRUE( dbg.StepOut(1, NULL, NULL, NULL, 0) );

//	if( WaitMsgBoxText("Cannot step through system code", 280) )
	if( WaitMsgBoxText("Can't trace into system DLLs", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Can't trace into system DLLs\" was found as expected" );
		MST.WButtonClick( "OK" ); 
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Can't trace into system DLLs\" was not found" );
		ret = FALSE;	 	
	}
	dbg.StopDebugging();
	return ret;
}
