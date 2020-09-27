///////////////////////////////////////////////////////////////////////////////
//	WINCASES.CPP
//
//	Created by :			Date :
//		MichMa					10/5/94
//
//	Description :
//		Win32-specific debugger sniff tests

#include "stdafx.h"
#include "wincases.h"
#include "debugger.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CWin32DbgCases, CTest, "Win32-specific DLL Tests", -1, CSniffDriver)


void CWin32DbgCases::Run(void)

{
	UIDAM uidam;
	CODebug dbg;
	COBreakpoints bp;
	COStack stk;
	COExpEval cxx;
	COProject prj;
	int before, after;

	COConnection *pconnec = GetSubSuite()->GetIDE()->GetConnectionInfo();

	#if defined(_MIPS_)	
		prj.Open(FullPath("dlldbgee\\mipsmain.exe"));					// open the debuggee.
    #elif defined(_ALPHA_)
        prj.Open(FullPath("dlldbgee\\alphamain.exe"));
	#else
		prj.Open(FullPath("dlldbgee\\x86main.exe"));
		pconnec->SetAll();
	#endif

	if(pconnec->GetUserConnections() != CONNECTION_LOCAL)
	{
	#if defined(_MIPS_)
		prj.SetRemotePath("mipsmain.exe");
		prj.UpdateRemoteFile("dlldbgee\\mipsmain.exe");
		prj.UpdateRemoteFile("dlldbgee\\mipsdll.dll");
	#elif defined(_ALPHA_)
		prj.SetRemotePath("alphamain.exe");
		prj.UpdateRemoteFile("dlldbgee\\alphamain.exe");
		prj.UpdateRemoteFile("dlldbgee\\alphadll.dll");
	#else
		prj.SetRemotePath("x86main.exe");
		prj.UpdateRemoteFile("dlldbgee\\x86main.exe");
		prj.UpdateRemoteFile("dlldbgee\\x86dll.dll");
	#endif
	}

	bp.ClearAllBreakpoints();									// Clear All Breakpoints Just in case
	EXPECT_VALIDBP(bp.SetBreakpoint("WndProc"));				// Set simple Breakpint on this function
	EXPECT_TRUE(dbg.Go("WndProc"));								// Go and verify we hit BP

	uidam.Activate();
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));						// Prepare for source mode stepping
	EXPECT_TRUE(UIDebug::ShowDockWindow(IDW_MEMORY_WIN, TRUE));
	EXPECT_TRUE(UIDebug::ShowDockWindow(IDW_CPU_WIN, TRUE));
	EXPECT_TRUE(UIDebug::ShowDockWindow(IDW_CALLS_WIN, TRUE));
	
	EXPECT_TRUE(bp.ClearAllBreakpoints());						// Clear All Breakpoints
	EXPECT_VALIDBP(bp.SetBreakpoint("{,dll.c,}foo"));		// Breakpoint on DLL func w/context operator
	EXPECT_TRUE(dbg.Go(NULL,NULL,"foo"));						// Go and verify we hit BP
	EXPECT_TRUE(dbg.StepOut(1,NULL,"local= foo(1 , 2);","WndProc"));// Execute To Previous frame, out of DLL

	EXPECT_TRUE(dbg.StepInto(2,NULL,NULL,"recursive_dll"));		// Trace from exe Into DLL fcn
		
	EXPECT_VALIDBP(bp.SetBreakpoint("{,,x86main.exe}mycallbackfcn"));// Breakpoint on callback function (in exe)
	EXPECT_TRUE(dbg.Go("mycallbackfcn"));						// Go and verify we hit BP
	EXPECT_TRUE(dbg.StepOut(1,NULL,"(*lpfnCallBack)()"));		// Execute To Previous frame,(out of exe, back to DLL)
	EXPECT_TRUE(bp.ClearAllBreakpoints());						// Clear All Breakpoints
#if !defined( _MIPS_ )	
	EXPECT_TRUE(dbg.StepInto(5,"mycallbackfcn"));				// Step  Into callback function
#else
	//
	//	MIPS handles loop differently.
	//	Takes one less step to get back to function call.
	//
	EXPECT_TRUE(dbg.StepInto(4,"mycallbackfcn"));				// Step  Into callback function
#endif
	EXPECT_VALIDBP(bp.SetBreakpoint("{,main.c,}@92"));			// Breakpoint on callback function (in exe)
	EXPECT_TRUE(dbg.Go(NULL,"@92","WndProc"));					// Execute To Previous frame,out of DLL

	EXPECT_TRUE(bp.ClearAllBreakpoints());						// Clear All Breakpoints
	EXPECT_VALIDBP(bp.SetBreakpoint("global",COBP_TYPE_IF_EXP_CHANGED,1)); //Breakpoint if expr changed
	EXPECT_TRUE(cxx.GetExpressionValue("global", &before));		// Get expression's value before execution
#if !defined( _MIPS_ )	
	EXPECT_TRUE(dbg.Go(NULL,"global++"));						// Go and verify we hit BP
#else
	//
	//	MIPS specific behavior.
	//
	EXPECT_TRUE(dbg.Go(NULL,"localx=local+global"));			// Go and verify we hit BP
#endif
	EXPECT_TRUE(cxx.GetExpressionValue("global", &after));		// Get expression's value after execution
	EXPECT_TRUE((before != after));								// Verify expression changed

	EXPECT_TRUE(bp.ClearAllBreakpoints());						// Clear All Breakpoints
	EXPECT_VALIDBP(bp.SetBreakpoint("(global+local==14)",COBP_TYPE_IF_EXP_TRUE,1)); //Breakpoint if expr true
	// REVIEW (michma): timing problem in ide (not cafe), won't fix.
	Sleep(1000);
	EXPECT_TRUE(dbg.Go());										// Go and verify we hit BP
	EXPECT_TRUE(cxx.ExpressionValueIs("(global+local)",14));	// Verify expression true

	/* exception handling
	EXPECT_TRUE(bp.ClearAllBreakpoints());						// Clear All Breakpoints
	EXPECT_TRUE(dbg.Go());										// Go and verify we hit BP
	// handle exception dialog
	*/

	EXPECT_TRUE(dbg.SetNextStatement(101));						// Move Cursor, Set Next statement to be executed
	
	// ***break on message stuff goes here***
/*
	EXPECT_VALIDBP(bp.SetBreakOnMsg("{,,x86main.exe}WndProc","WM_PAINT",BK_MESSAGE)); // Set Breakpoint on message
	EXPECT_TRUE(dbg.Go("WndProc"));								// Go and verify we hit BP
	EXPECT_TRUE(dbg.StepInto());								// Step  Into WndProc
	EXPECT_TRUE(cxx.ExpressionValueIs("message",15));			// Verify message=16 (WM_PAINT)

	EXPECT_TRUE(bp.ClearAllBreakpoints());						// Clear All Breakpoints
	EXPECT_VALIDBP(bp.SetBreakpoint("@109"));					// Set simple Breakpint on this line
	EXPECT_TRUE(dbg.Go(NULL,"@109"));							// Go and verify we hit BP
	EXPECT_TRUE(dbg.SetNextStatement("MyPostMessage(hwnd)"));	// Set execution to MyPostMessage()
	EXPECT_TRUE(dbg.SetNextStatement(125));						// Set execution to MyPostMessage()
	EXPECT_TRUE(bp.ClearAllBreakpoints());						// Clear All Breakpoints
	EXPECT_VALIDBP(bp.SetBreakOnMsg("{,,x86main.exe}WndProc","Window",BK_CLASS));// Set Breakpoint on message class
	EXPECT_TRUE(dbg.Go("WndProc"));								// Go and verify we hit BP
	EXPECT_TRUE(dbg.StepInto());								// Step  Into WndProc
	EXPECT_TRUE(cxx.ExpressionValueIs("message",15));			// Verify message=16 (WM_PAINT)

	
	EXPECT_TRUE(bp.ClearAllBreakpoints());						// Clear All Breakpoints
	EXPECT_TRUE(dbg.Go(NULL,NULL,NULL,0))  						// Execute debuggee NO Waiting
	EXPECT_TRUE(dbg.Break())									// Break debuggee execution
	EXPECT_TRUE(dbg.GetDebugState()==Debug_Break);				//  	Verify we broke?

    */
	return;
}

