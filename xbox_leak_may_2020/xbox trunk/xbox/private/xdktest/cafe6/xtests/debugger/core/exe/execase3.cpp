///////////////////////////////////////////////////////////////////////////////
//	EXECASE3.CPP
//											 
//	Created by:			Date:			
//		MichMa				2/2/98	
//
//	Description:								 
//		Implementation of CExe3Cases		 

#include "stdafx.h"
#include "execase3.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
											   
IMPLEMENT_TEST(CExe3Cases, CDbgTestBase, "Execution Pri1", -1, CexeSubSuite)


// each function that runs a test has this prototype.
typedef void (CExe3Cases::*PTR_TEST_FUNC)(void); 

// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.
PTR_TEST_FUNC tests[] = 
{
	&CExe3Cases::StartDebuggingBreakRestartAndStep,
	&CExe3Cases::StepIntoFuncThroughIndirectCall,
	&CExe3Cases::StepIntoWhenInstructionPtrAdvancedButNotSourcePtr,
	&CExe3Cases::StepIntoWhenInstructionAndSourcePtrsAdvanceWithFocusInAsm,
	&CExe3Cases::StepIntoWhenInstructionAndSourcePtrsAdvanceWithFocusInSource,
	&CExe3Cases::StepOverToLineWithLocationBP,
	&CExe3Cases::StepOverFuncInMixedMode,
	&CExe3Cases::StepOverFuncWhichWillHitMessageBP,
	&CExe3Cases::StepOutOfFuncWhichWillHitExpressionTrueBP,
	&CExe3Cases::StepOutOfFuncWhichWillHitExpressionChangedBP,
	&CExe3Cases::StepOutOfFuncWhichWillHitMessageBP,
	&CExe3Cases::GoUntilExceptionIsRaised,
	&CExe3Cases::StopDebuggingWhileDebuggeeIsRunning,
	&CExe3Cases::StepToCursorInAsm,
	&CExe3Cases::StepToCursorAtLineWithLocationBP,
	&CExe3Cases::SetNextStatementInSource,
	&CExe3Cases::SetNextStatementInAsm,
	&CExe3Cases::ShowNextStatementWhenCursorNotOnStatementAndSourceInForeground,
	&CExe3Cases::ShowNextStatementWhenScrollingRequiredAndSourceInForeground,
	&CExe3Cases::ShowNextStatementInAsm,
	&CExe3Cases::StepIntoSpecificNonNested,
	&CExe3Cases::StepIntoSpecificNested,
	&CExe3Cases::StepIntoSpecificWhenBPSetInInnerFunc,
	//xbox - we dont do this, but check with jlange anyway
	//&CExe3Cases::LoadCOFFExports
};


void CExe3Cases::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


void CExe3Cases::Run(void)
{
	if (!CreateXboxProjectFromSource("execution pri1", "src\\execution pri1",
				"execution pri1 exe.cpp"))
			return;

	// all tests expect the ide to be in this initial state.
	InitTestState();

	// we only want to randomly run each test once.

	RandomNumberCheckList rncl(sizeof(tests) / sizeof(PTR_TEST_FUNC));
	// indexes into the test function array.
	int iTest;
	
	if(CMDLINE->GetBooleanValue("random", TRUE))
	{
		// randomly run each test once.
		while((iTest = rncl.GetNext()) != -1)
			(this->*(tests[iTest]))();
	}
	else
	{
		// run each test in sequence.
		for(iTest = 0; iTest < sizeof(tests) / sizeof(PTR_TEST_FUNC); iTest++)
			(this->*(tests[iTest]))();
	}
}


void CExe3Cases::StartDebuggingBreakRestartAndStep(void)
{	
	LogTestHeader("StartDebuggingBreakRestartAndStep");
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	Sleep(3000); // emmang@xbox - give the app some time
	EXPECT_TRUE(dbg.Break());
	EXPECT_TRUE(dbg.CurrentLineIs("loop line"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.CurrentLineIs("second line of main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	InitTestState();
}


void CExe3Cases::StepIntoFuncThroughIndirectCall(void)
{	
	LogTestHeader("StepIntoFuncThroughIndirectCall");
	EXPECT_TRUE(src.Find("pFuncCalledIndirectly();"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of FuncCalledIndirectly()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("FuncCalledIndirectly"));
	InitTestState();
}


void CExe3Cases::StepIntoWhenInstructionPtrAdvancedButNotSourcePtr(void)
{
	LogTestHeader("StepIntoWhenInstructionPtrAdvancedButNotSourcePtr");
	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dam.InstructionIs("mov"));
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	InitTestState();
}


void CExe3Cases::StepIntoWhenInstructionAndSourcePtrsAdvanceWithFocusInAsm(void)
{
	LogTestHeader("StepIntoWhenInstructionAndSourcePtrsAdvanceWithFocusInAsm");
	EXPECT_TRUE(src.Find("_asm"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dam.InstructionIs("pop"));
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	EXPECT_TRUE(dbg.CurrentLineIs("pop eax"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	InitTestState();
}


void CExe3Cases::StepIntoWhenInstructionAndSourcePtrsAdvanceWithFocusInSource(void)
{
	LogTestHeader("StepIntoWhenInstructionAndSourcePtrsAdvanceWithFocusInSource");
	EXPECT_TRUE(src.Find("_asm"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.CurrentLineIs("pop eax"));
	EXPECT_TRUE(dbg.SetSteppingMode(ASM));
	EXPECT_TRUE(dam.InstructionIs("pop"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	InitTestState();
}


void CExe3Cases::StepOverToLineWithLocationBP(void)
{	
	LogTestHeader("StepOverToLineWithLocationBP");
	// note that initial state has current line at first line of main().
	EXPECT_TRUE(src.Find("second line of main()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.StepOver());
	// make sure the bp is still there.
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("second line of main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CExe3Cases::StepOverFuncInMixedMode(void)
{
	LogTestHeader("StepOverFuncInMixedMode");
	EXPECT_TRUE(src.Find("Func();"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(dbg.StepOver());
	// we want to check the source line immediately above the current instruction. 
	CString strDAMLine = dam.GetLines(-1, 1);
	EXPECT_TRUE(strDAMLine.Find("line after call to Func()") != -1);
	// now verify the source window.
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	EXPECT_TRUE(dbg.CurrentLineIs("line after call to Func()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	InitTestState();
}


void CExe3Cases::StepOutOfFuncWhichWillHitExpressionTrueBP(void)
{
	LogTestHeader("StepOutOfFuncWhichWillHitExpressionTrueBP");
	EXPECT_TRUE(src.Find("first line of Func()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_VALIDBP(bps.SetBreakOnExpr("gintVar == 1", COBP_TYPE_IF_EXP_TRUE));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("line after expression bp hit"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// TODO(michma): we need to do this extra step because of bug 38172.
	EXPECT_TRUE(dbg.StepInto());
	InitTestState();
}


void CExe3Cases::StepOutOfFuncWhichWillHitExpressionChangedBP(void)
{
	LogTestHeader("StepOutOfFuncWhichWillHitExpressionChangedBP");
	EXPECT_TRUE(src.Find("first line of Func()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_VALIDBP(bps.SetBreakOnExpr("gintVar", COBP_TYPE_IF_EXP_CHANGED));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("line after expression bp hit"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// TODO(michma): we need to do this extra step because of bug 38172.
	EXPECT_TRUE(dbg.StepInto());
	InitTestState();
}


void CExe3Cases::GoUntilExceptionIsRaised(void)
{
	LogTestHeader("GoUntilExceptionIsRaised");
	// this will cause a divide by 0 error.
	EXPECT_TRUE(cxx.SetExpressionValue("gintDenominator", 0));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	EXPECT_TRUE(dbg.HandleMsg(MSG_EXCEPTION_DIVIDE_BY_ZERO));
	EXPECT_TRUE(dbg.CurrentLineIs("int i = 1 / gintDenominator;"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	InitTestState();
}


void CExe3Cases::StopDebuggingWhileDebuggeeIsRunning(void)
{
	LogTestHeader("StopDebuggingWhileDebuggeeIsRunning");
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_RUN));
	EXPECT_TRUE(dbg.StopDebugging());
	InitTestState();
}


void CExe3Cases::StepToCursorInAsm(void)
{
	LogTestHeader("StepToCursorInAsm");
	// debug to a place where we know for sure what the asm is.
	EXPECT_TRUE(src.Find("_asm"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dam.Enable());
	// attach the COSource object to the asm view so we can navigate it.
	src.AttachActive();
	// the instruction we want to step to is 2 lines down in the listing.
	src.TypeTextAtCursor("{DOWN 2}");
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dam.InstructionIs("pop"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	InitTestState();
}


void CExe3Cases::StepToCursorAtLineWithLocationBP(void)
{	
	LogTestHeader("StepToCursorAtLineWithLocationBP");
	// note that initial state has current line at first line of main().
	EXPECT_TRUE(src.Find("second line of main()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.StepToCursor());
	// make sure the bp is still there.
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("second line of main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CExe3Cases::ShowNextStatementWhenCursorNotOnStatementAndSourceInForeground(void)
{
	LogTestHeader("ShowNextStatementWhenCursorNotOnStatementAndSourceInForeground");
	EXPECT_TRUE(src.Find("first line of source file"));
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of main()"));
	InitTestState();
}


void CExe3Cases::ShowNextStatementWhenScrollingRequiredAndSourceInForeground(void)
{
	LogTestHeader("ShowNextStatementWhenScrollingRequiredAndSourceInForeground");
	EXPECT_TRUE(src.Find("END: for tests that require scrolling code out of view."));
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of main()"));
	InitTestState();
}


void CExe3Cases::ShowNextStatementInAsm(void)
{
	LogTestHeader("ShowNextStatementInAsm");
	// debug to a place where we know for sure what the asm is.
	EXPECT_TRUE(src.Find("_asm"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dam.Enable());
	// attach the COSource object to the asm view so we can navigate it.
	src.AttachActive();
	// move the cursor down a page.
	src.TypeTextAtCursor("{PGDN}");
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(dam.InstructionIs("push"));
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	InitTestState();
}


void CExe3Cases::StepOverFuncWhichWillHitMessageBP(void)
{
	LogTestHeader("StepOverFuncWhichWillHitMessageBP");
	EXPECT_TRUE(src.Find("SendMessageFunc();"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_VALIDBP(bps.SetBreakOnMsg("WndProc", "WM_NULL"));
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of WndProc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("WndProc"));
	// value of WM_NULL message is 0.
	EXPECT_TRUE(cxx.ExpressionValueIs("msg", 0));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CExe3Cases::StepOutOfFuncWhichWillHitMessageBP(void)

{
		LogTestHeader("StepOutOfFuncWhichWillHitMessageBP");
		EXPECT_TRUE(src.Find("first line of SendMessageFunc()"));
		EXPECT_TRUE(dbg.StepToCursor());
		EXPECT_VALIDBP(bps.SetBreakOnMsg("WndProc", "WM_NULL"));
		EXPECT_TRUE(dbg.StepOut());
		EXPECT_TRUE(dbg.CurrentLineIs("first line of WndProc()"));
		EXPECT_TRUE(stk.CurrentFunctionIs("WndProc"));
	
		// michma - 3/1/99: due to bug vc98:60437.
		if(GetSystem() | SYSTEM_NT_5)
			{ EXPECT_TRUE(dbg.StepOver()); }

		// value of WM_NULL message is 0.
		EXPECT_TRUE(cxx.ExpressionValueIs("msg", 0));
		EXPECT_TRUE(bps.ClearAllBreakpoints());
		InitTestState();
}


void CExe3Cases::SetNextStatementInSource(void)
{
	LogTestHeader("SetNextStatementInSource");
	// step to a point where the next statement is an infinite loop.
	EXPECT_TRUE(src.Find("loop line"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(src.Find("returned from main()"));
	EXPECT_TRUE(bps.SetBreakpoint());
	// set the next statement to one after the infinite loop.
	EXPECT_TRUE(src.Find("return from main()"));
	EXPECT_TRUE(dbg.SetNextStatement());
	// now instead of hitting the infinite loop the app should terminate.
	// EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("returned from main()"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CExe3Cases::SetNextStatementInAsm(void)
{
	LogTestHeader("SetNextStatementInAsm");
	// step to a point where the next statement is an infinite loop.
	EXPECT_TRUE(src.Find("loop line"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(src.Find("returned from main()"));
	EXPECT_TRUE(bps.SetBreakpoint());
	EXPECT_TRUE(dam.Enable());
	// attach the COSource object to the asm view so we can navigate it.
	src.AttachActive();
	// set the next instruction to one after the infinite loop (in the main return).
	src.TypeTextAtCursor("{DOWN 6}");
	EXPECT_TRUE(dbg.SetNextStatement());
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	// now instead of hitting the infinite loop the app should terminate.
	// EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("returned from main()"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CExe3Cases::StepIntoSpecificNonNested(void)
{
	LogTestHeader("StepIntoSpecificNonNested");
	EXPECT_TRUE(src.Find("Func();"));
	EXPECT_TRUE(dbg.StepToCursor());
	// put cursor on function name.
	EXPECT_TRUE(src.Find("Func();"));
	src.TypeTextAtCursor("{LEFT}{RIGHT}");
	EXPECT_TRUE(dbg.StepIntoSpecific());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	InitTestState();
}


void CExe3Cases::StepIntoSpecificNested(void)
{
	LogTestHeader("StepIntoSpecificNested");
	EXPECT_TRUE(src.Find("OuterFunc(InnerFunc());"));
	EXPECT_TRUE(dbg.StepToCursor());
	// put cursor on inner function name.
	EXPECT_TRUE(src.Find("InnerFunc()"));
	src.TypeTextAtCursor("{LEFT}{RIGHT}");
	EXPECT_TRUE(dbg.StepIntoSpecific());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of InnerFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("InnerFunc"));
	EXPECT_TRUE(dbg.StepOut());
	// put cursor on outer function name.
	EXPECT_TRUE(src.Find("OuterFunc("));
	src.TypeTextAtCursor("{LEFT}{RIGHT}");
	EXPECT_TRUE(dbg.StepIntoSpecific());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of OuterFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("OuterFunc"));
	InitTestState();
}


void CExe3Cases::StepIntoSpecificWhenBPSetInInnerFunc(void)
{
	LogTestHeader("StepIntoSpecificWhenBPSetInInnerFunc");
	EXPECT_TRUE(src.Find("OuterFunc(InnerFunc());"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_VALIDBP(bps.SetBreakpoint("InnerFunc"));
	// put cursor on outer function name.
	EXPECT_TRUE(src.Find("OuterFunc("));
	src.TypeTextAtCursor("{LEFT}{RIGHT}");
	EXPECT_TRUE(dbg.StepIntoSpecific());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of InnerFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("InnerFunc"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CExe3Cases::LoadCOFFExports(void)

{
	// this feature isn't supported on win9x.
	if(!((GetSystem() & SYSTEM_WIN_95) || (GetSystem() & SYSTEM_WIN_98) || (GetSystem() & SYSTEM_WIN_MILL)))
	
	{
		LogTestHeader("LoadCOFFExports");
		// Load COFF/Exports setting can only be toggled between sessions.
		EXPECT_TRUE(dbg.StopDebugging());

		// michma - 9/1/99: there should be a CODebug::ToggleLoadCOFFExports function for this, but since this is a one-time test addition for 
		// an old product, i don't want to take the time.
		
		// turn the option on.
		UIOptionsTabDlg uiopt;
		uiopt.Display();
		uiopt.ShowPage(TAB_DEBUG, 6);
		MST.WCheckCheck("&Load COFF && Exports");
		uiopt.OK();

		EXPECT_TRUE(dbg.StepInto());
		// instead of just KERNEL32 plus some address, we should now see a function name in the 3rd frame.
		if(GetSystem() & SYSTEM_NT_5)
			{EXPECT_TRUE(stk.FunctionIs("SetUnhandledExceptionFilter", 2));}
		else
			{EXPECT_TRUE(stk.FunctionIs("GetProcessPriorityBoost", 2));}
		
		// turn the option off.
		EXPECT_TRUE(dbg.StopDebugging());
		uiopt.Display();
		uiopt.ShowPage(TAB_DEBUG, 6);
 		MST.WCheckUnCheck("&Load COFF && Exports");
		uiopt.OK();

		EXPECT_TRUE(dbg.StepInto());
		// the function name should be gone now.
		if(GetSystem() & SYSTEM_NT_5)
			{EXPECT_TRUE(!stk.FunctionIs("SetUnhandledExceptionFilter", 2));}
		else
			{EXPECT_TRUE(!stk.FunctionIs("GetProcessPriorityBoost", 2));}
	}
}


void CExe3Cases::InitTestState(void)
{
	EXPECT_TRUE(dbg.Restart());
	// attach the COSource object to the source file that the debugger opened.
	src.AttachActive();
} 

