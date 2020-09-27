///////////////////////////////////////////////////////////////////////////////
//	BP1CASES.CPP
//
//	Created by :			Date :
//		YefimS			11/21/93
//
//	Description :
//		Implementation of the BP1Cases
//

#include <process.h>
#include "stdafx.h"
#include "bpcase.h"	

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CbpIDETest::BreakAtLine(  )
{
 	LogTestHeader( "BreakAtLine" );
	UIBreakpoints bpd;

	CString strSource = "bp__cc.c";
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource );

	CString strLine = "105";
	int nLine = 105;
	CString strLocation = (CString)"{," + strSource + ",}." + strLine;
	EXPECT( src.GoToLine(nLine) );
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	bpd.Activate();
	EXPECT_TRUE_COMMENT( bpd.BreakpointIsSet(strLine, strSource), "F9 * Source Inactive * at line 105" );
	EXPECT( bpd.Close() == NULL );
	EXPECT_TRUE_COMMENT( dbg.Go(NULL, "us = (ushort)counter;"), "F9 * Source Inactive * Break at BP" );

	strLine = "109";
	nLine = 109;
	strLocation = (CString)"{," + strSource + ",}." + strLine;
	EXPECT( src.GoToLine(nLine) );
	DoKeys( KEY_TOGGLE_BREAKPOINT );
	bpd.Activate();
	EXPECT_TRUE_COMMENT( bpd.BreakpointIsSet(strLine, strSource), "F9 * Source Active * at line 109" );
	EXPECT( bpd.Close() == NULL );
	EXPECT_TRUE_COMMENT( dbg.Go(NULL, "d  = (double)counter;"), "F9 * Source Active * Break at BP" );
	
	bps.ClearAllBreakpoints();
	strSource = "bp__ll.c";
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource );
	strLine = "108";
	nLine = 108;
	strLocation = (CString)"{," + strSource + ",}@" + strLine;
	bps.SetBreakpoint(strLocation);
	bpd.Activate();
	EXPECT_TRUE_COMMENT( bpd.BreakpointIsSet(strLine, strSource), "BPDlg * Source Inactive * at line 108" );
	EXPECT( bpd.Close() == NULL );
	EXPECT_TRUE_COMMENT( dbg.Go(NULL, "ul = (ulong)counter;"), "BPDlg * Source Inactive * Break at BP" );
	
	strLine = "125";
	nLine = 125;
	strLocation = (CString)"{," + strSource + ",}@" + strLine;
	bps.SetBreakpoint(strLocation);
	bpd.Activate();
	EXPECT_TRUE_COMMENT( bpd.BreakpointIsSet(strLine, strSource), "BPDlg * Source Active * at line 125" );
	EXPECT( bpd.Close() == NULL );
	EXPECT_TRUE_COMMENT( dbg.Go(NULL, "pun->ul = (ulong)counter;"), "BPDlg * Source Active * Break at BP" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CbpIDETest::Disable_Enable()
{
 	LogTestHeader( "Disable_Enable" );
	bps.ClearAllBreakpoints();
	CString strSource = "bp__ll.c";
	UIWB.OpenFile( m_strProjectDir + "\\" + strSource );

	CString strLine = "106";
	CString strLocation = (CString)"{," + strSource + ",}@" + strLine;
	bp *pbp = bps.SetBreakpoint(strLocation);

	UIBreakpoints bpd;
	bpd.Activate();
	EXPECT_TRUE( bpd.BreakpointIsSet(strLine, strSource) );

	bpd.Disable();
	EXPECT_TRUE_COMMENT ( !bpd.BreakpointIsEnabled(strLine, strSource), "Disable BP : at line 106 in bp__ll.c" );

	bpd.Enable();
	EXPECT_TRUE_COMMENT ( bpd.BreakpointIsEnabled(strLine, strSource), "Enable BP : at line 106 in bp__ll.c" );

	EXPECT( bpd.Close() == NULL );
	EXPECT_TRUE_COMMENT( dbg.Go(NULL, "us = (ushort)counter;"), "Break at enabled BP" );

	bpd.Activate();
	bpd.Disable();
	EXPECT_TRUE_COMMENT ( !bpd.BreakpointIsEnabled(strLine, strSource), "Disable BP again: 106" );
	EXPECT( bpd.Close() == NULL );

	bps.ClearAllBreakpoints();
	strLine = "108";
	strLocation = (CString)"{," + strSource + ",}@" + strLine;
	pbp = bps.SetBreakpoint(strLocation);

	bpd.Activate();
	bpd.Disable();
	EXPECT_TRUE_COMMENT ( !bpd.BreakpointIsEnabled(strLine, strSource), "Disable BP : at line 108 in bp__ll.c" );

	EXPECT( bpd.Close() == NULL );
	//xbox EXPECT_TRUE_COMMENT ( dbg.Go(NULL,NULL,NULL,WAIT_FOR_TERMINATION) , "The Project testbp01 was run" );
	dbg.StopDebugging();

	bpd.Activate();
	bpd.Enable();
	EXPECT_TRUE_COMMENT ( bpd.BreakpointIsEnabled(strLine, strSource), "Enable BP : at line 108 in bp__ll.c" );
	EXPECT( bpd.Close() == NULL );
	EXPECT_TRUE_COMMENT( dbg.Go(NULL, "ul = (ulong)counter;"), "Break at enabled BP: 108" );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CbpIDETest::BreakOnExpressionStaticVars()
{
 	LogTestHeader( "BreakOnExpressionStaticVars" );
	bps.ClearAllBreakpoints();
	dbg.Restart();

	EXPECT_TRUE( bps.SetBreakOnExpr("{recur_func}x==7", COBP_TYPE_IF_EXP_TRUE) != NULL );
	dbg.Go(NULL,NULL,NULL,0);
	if( WaitMsgBoxText("Break when '{recur_func}x==7'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{recur_func}x==7'\" was found as expected" );
		// REVIEW(michma - 6/3/99): there is this weird problem on w2k where the data breakpoint hit msg box
		// isn't active right away. doesn't repro manually, only when Go is performed through cafe.
		// so we need to reactivate the IDE for the OK button to become active so we can dismiss the dlg.
		UIWB.Activate();
		MST.WButtonClick( "OK" );
		while(MST.WButtonExists("OK"));
		EXPECT_TRUE_COMMENT( TRUE, "BreakOnExprTrue" );	 	
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{recur_func}x==7'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "if (x<9) recur_func();" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("x==7",1) );

	EXPECT_TRUE( bps.SetBreakOnExpr("{recur_func}x", COBP_TYPE_IF_EXP_CHANGED) != NULL );
	dbg.Go(NULL,NULL,NULL,0);

	if( WaitMsgBoxText("Break when '{recur_func}x'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{recur_func}x'\" was found as expected" );
		// REVIEW(michma - 6/3/99): there is this weird problem on w2k where the data breakpoint hit msg box
		// isn't active right away. doesn't repro manually, only when Go is performed through cafe.
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
	EXPECT_TRUE( cxx.ExpressionValueIs("x==8",1) );

	dbg.Go(NULL,NULL,NULL,0);
	if( WaitMsgBoxText("Break when '{recur_func}x'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{recur_func}x'\" was found as expected" );
		// REVIEW(michma - 6/3/99): there is this weird problem on w2k where the data breakpoint hit msg box
		// isn't active right away. doesn't repro manually, only when Go is performed through cafe.
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
	EXPECT_TRUE( cxx.ExpressionValueIs("x==9",1) );

	//xbox EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION) );
	EXPECT_TRUE( dbg.Restart() );
	
	return (TRUE);
}

BOOL CbpIDETest::BreakOnCompoundExpression()
{
 	LogTestHeader( "BreakOnCompoundExpression" );
	bps.ClearAllBreakpoints();
 	dbg.Restart();

	EXPECT_TRUE( bps.SetBreakOnExpr("{TestG}(Global*Local+Int/3)==1", COBP_TYPE_IF_EXP_TRUE) != NULL );
	dbg.Go(NULL,NULL,NULL,0);
	WaitStepInstructions("Waiting for Break dialog");

	if( WaitMsgBoxText("Break when '{TestG}(Global*Local+Int/3)==1'", 280) )
	{
		WriteLog( PASSED, "The MsgBox \"Break when '{TestG}(Global*Local+Int/3)==1'\" was found as expected" );
		// REVIEW(michma - 6/3/99): there is this weird problem on w2k where the data breakpoint hit msg box
		// isn't active right away. doesn't repro manually, only when Go is performed through cafe.
		// so we need to reactivate the IDE for the OK button to become active so we can dismiss the dlg.
		UIWB.Activate();
		MST.WButtonClick( "OK" );
		while(MST.WButtonExists("OK"));
		EXPECT_TRUE_COMMENT( TRUE, "BreakOnExprTrue" );	 	
	}
	else
	{
		WriteLog( FAILED, "The MsgBox \"Break when '{TestG}(Global*Local+Int/3)==1'\" was not found" );
		EXPECT_TRUE( FALSE );	 	
	}
	EXPECT_TRUE( dbg.CurrentLineIs( "for (Int=0; Int < ArrayLength; Int++)" ) );
	EXPECT_TRUE( cxx.ExpressionValueIs("(Global*Local+Int/3)==1",1) );

	dbg.StopDebugging();
	return (TRUE);
}

BOOL CbpIDETest::BreakAtVirtualFunc()
{
 	LogTestHeader( "BreakAtVirtualFunc" );
	bps.ClearAllBreakpoints();

	bps.SetBreakpoint("fooclass::vf1");
	bps.SetBreakpoint("fooclass::vf2");
	bps.SetBreakpoint("base::vf2");
	bps.SetBreakpoint("base::vf3");

	EXPECT_TRUE( dbg.Go(NULL, "void vf1() { int i = 7; };", "fooclass::vf1") );
	EXPECT_TRUE( dbg.Go(NULL, "virtual int  vf2() { return 10; };", "base::vf2") );
	EXPECT_TRUE( dbg.Go(NULL, "int  vf2(int j) { return j; };", "fooclass::vf2") );
	EXPECT_TRUE( dbg.Go(NULL, "virtual int  vf3(int ind) { return ind*2; };", "base::vf3") );

	dbg.StopDebugging();
	return (TRUE);
}
