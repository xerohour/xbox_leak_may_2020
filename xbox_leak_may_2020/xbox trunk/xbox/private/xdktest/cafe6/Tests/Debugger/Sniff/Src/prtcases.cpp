///////////////////////////////////////////////////////////////////////////////
//      PRTCASES.CPP
//
//      Created by :                    Date :
//              MichMa                                  10/5/94
//
//      Description :
//              Portable debugger sniff tests

#include "stdafx.h"
#include "prtcases.h"
#include "debugger.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CPortableDbgCases, CTest, "Portable Tests", -1, CSniffDriver)


void CPortableDbgCases::Run(void)

{
	UIDAM uidam;
	COMemory mem;
	COProject prj;
	CODebug dbg;
	COBreakpoints bp;
	COStack stk;
	COExpEval cxx;
	COConnection *pconnec = GetSubSuite()->GetIDE()->GetConnectionInfo();

	#if defined(_MIPS_)                                             
		prj.Open(FullPath("prtdbgee\\win32mips.exe")); 
    #elif defined(_ALPHA_)
        prj.Open(FullPath("prtdbgee\\win32alpha.exe"));
	#else
		prj.Open(FullPath("prtdbgee\\win32x86.exe"));
	#endif
	
	pconnec->SetAll();

	if(pconnec->GetUserConnections() != CONNECTION_LOCAL)
	{
	#if defined(_MIPS_)                                             
 		prj.SetRemotePath("win32mips.exe");
		prj.UpdateRemoteFile("prtdbgee\\win32mips.exe");
	#elif defined(_ALPHA_)
		prj.SetRemotePath("win32alpha.exe");
		prj.UpdateRemoteFile("prtdbgee\\win32alpha.exe");
	#else
		prj.SetRemotePath("win32x86.exe");
		prj.UpdateRemoteFile("prtdbgee\\win32x86.exe");
	#endif
	}

	bp.ClearAllBreakpoints();                                                                       // Clear All Breakpoints Just in case
 
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));                                          // Prepare for source mode stepping
	EXPECT_TRUE(dbg.StepInto(1,NULL,"WinMain("));                           // Trace to start debugging

	uidam.Activate();
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));  
	EXPECT_TRUE(UIDebug::ShowDockWindow(IDW_MEMORY_WIN, TRUE));
	EXPECT_TRUE(UIDebug::ShowDockWindow(IDW_CPU_WIN, TRUE));
	EXPECT_TRUE(UIDebug::ShowDockWindow(IDW_CALLS_WIN, TRUE));

	EXPECT_TRUE(dbg.StopDebugging(ASSUME_NORMAL_TERMINATION));      // Stop debugging
	EXPECT_TRUE(dbg.GetDebugState()==NotDebugging);                         // Verify stopped
	
	EXPECT_TRUE(dbg.StepOver(2,NULL,"obj_a    obj_a1;"));           // Step to start debugging
	EXPECT_TRUE(dbg.StepOver(3,NULL,"obj_ab   obj_ab1;"));          // Step  Over fcns (constructor)
	EXPECT_TRUE(dbg.StepInto(1,NULL,NULL,"obj_ab::obj_ab"));        // Trace Into fcn  (constructor)
	EXPECT_TRUE(dbg.StepInto(1,NULL,NULL,"obj_a::obj_a"));          // Trace Into fcn  (constructor)
	EXPECT_VALIDBP(bp.SetBreakpoint());                                                     // Set simple Breakpint on this line
	EXPECT_TRUE(dbg.StepOut(1,NULL,NULL,"obj_ab::obj_ab"));         // Execute To Previous frame,(StepOut)
	EXPECT_TRUE(dbg.StepInto(1,NULL,NULL,"obj_b::obj_b"));          // Trace Into fcn  (constructor)
	EXPECT_TRUE(dbg.StepOut(1,NULL,NULL,"obj_ab::obj_ab"));         // Execute To Previous frame,(StepOut)
	EXPECT_TRUE(dbg.StepInto(4,NULL,"obj_v_ab obj_v_ab1;","WinMain("));//Trace thru and out of fnc

	EXPECT_TRUE(dbg.StepOver(1,NULL,"obj_a()","obj_a::obj_a")); // Step function with bp, hit bp
	EXPECT_TRUE(dbg.StepOut(1,NULL,NULL,"obj_v_ab::obj_v_ab"));     // Execute To Previous frame,(StepOut)
	EXPECT_TRUE(dbg.StepOut(1,NULL,NULL,"WinMain("));                               // Execute To Previous frame,(StepOut)
	
	EXPECT_TRUE(dbg.StepToCursor(258,NULL,"@258"));                         // Move Cursor to line, Execute to cursor (StepToCursor)
	
	EXPECT_TRUE(dbg.StepInto(2,NULL,NULL,"fooclass::print("));

	EXPECT_TRUE(stk.NavigateStack(1)>0);                                                      // Naviage callstack back to parent fcn
	EXPECT_TRUE(dbg.VerifyCurrentLine("@259"));                                     // Verify cursor is at correct line
	EXPECT_TRUE(cxx.ExpressionValueIs("f.id",2));
	// Verify locals of prev frame accessable

	EXPECT_TRUE(cxx.SetExpressionValue("f.id",7));                          // Verify locals of prev frame writable
	EXPECT_TRUE(dbg.StepOut(1,NULL,"@259"));                                        // Execute To Previous frame,(StepOut)

	EXPECT_TRUE(cxx.ExpressionValueIs("f.id",7));                           // Verify locals of prev frame accessable

	EXPECT_VALIDBP(bp.SetBreakpoint("DURING","(Int==2)",COBP_TYPE_IF_EXP_TRUE,0)); //Breakpoint at loc if expr true
	EXPECT_TRUE(dbg.Go("DURING"));                                                          // Go and verify we hit BP  ******  needs to handle BP hit dialog
	EXPECT_TRUE(cxx.ExpressionValueIs("Int",2));                            // Verify expression is TRUE
	EXPECT_VALIDBP(bp.SetBreakOnExpr("(Int==4)",COBP_TYPE_IF_EXP_TRUE,0)); //Breakpoint if expr true
	EXPECT_TRUE(dbg.Go());                                                                          // Go and verify we hit BP  ******  needs to handle BP hit dialog               
	EXPECT_TRUE(cxx.ExpressionValueIs("Int",4));                            // Verify expression is TRUE
	EXPECT_TRUE(bp.ClearAllBreakpoints());                                  // Clear All Breakpoints

	EXPECT_VALIDBP(bp.SetBreakpoint("fooclass::print(char *)"));// Breakpoint on overloaded function
	EXPECT_TRUE(dbg.Go(NULL,NULL,"fooclass::print("));                      // Go and verify we hit BP
	EXPECT_TRUE(dbg.StepOut());                                                                     // Execute To Previous frame,(StepOut) 

	EXPECT_TRUE(cxx.ExpressionValueIs("((*(obj_a*)(&obj_ab1))).common",1)); // some quick EE testing

	#if !defined( _MIPS_ )   // temporarily exclude function evaluation on mips
		EXPECT_TRUE(cxx.ExpressionValueIs("buddy()",-2));       
	#endif

	EXPECT_TRUE(mem.MemoryDataIs("((*(obj_a*)(&obj_ab1))).common", 1));

	EXPECT_TRUE(bp.ClearAllBreakpoints());                                                  // Clear All Breakpoints
	
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.Restart());                                                                             // Restart debugger
	EXPECT_TRUE(dbg.VerifyCurrentLine("WinMain("));                 // Verify at correct line (first)
	EXPECT_TRUE(dbg.Go(NULL,NULL,NULL,ASSUME_NORMAL_TERMINATION));  // Execute to debuggee termination
	EXPECT_TRUE(dbg.GetDebugState()==NotDebugging);                                 // Verify we terminated

	UIWB.CloseAllWindows();                                                                                 // Clean up
	prj.Close();
	return;
}
