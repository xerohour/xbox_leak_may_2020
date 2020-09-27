///////////////////////////////////////////////////////////////////////////////
//	exe_dlls.CPP
//											 
//	Created by :			
//		VCBU QA		

#include "stdafx.h"
#include "exe_dlls.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(Cexe_dllsIDETest, CDebugTestSet, "Execution DLLs", -1, CexeSubSuite)
												 
void Cexe_dllsIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}

void Cexe_dllsIDETest::Run()
{	 
	XSAFETY;
	
	if (SetProject("dllapp\\incdll", PROJECT_DLL) && SetProject("dllapp\\powdll", PROJECT_DLL)
		&& SetProject("dllapp\\dec2", PROJECT_DLL) && SetProject("dllapp\\dllappx"))
	{
		XSAFETY;
//		StartDebugging();		// WinslowF
		bps.ClearAllBreakpoints();
			
		EXPECT_TRUE(DllStepTrace());
		XSAFETY;
		EXPECT_TRUE(DllStepOut());
		XSAFETY;
		EXPECT_TRUE(DllTraceOut());
		XSAFETY;
		EXPECT_TRUE(StepOverFuncInUnloadedDll());
		XSAFETY;
		EXPECT_TRUE(RestartFromWithinDll());
		XSAFETY;
		EXPECT_TRUE(GoFromWithinDll());
		XSAFETY;
	}
	//	StopDbgCloseProject();
	
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

/*
** Stepping model test(s) involving DLL's.
** Step Loadlibrary and then trace into the virtual explicit dll.
**
*/ 

BOOL Cexe_dllsIDETest::DllStepTrace(   )
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
	EXPECT_TRUE( fSuccess &= (dbg.VerifyNotification("Loaded symbols", 3, 20, TRUE) && dbg.VerifyNotification("powdll.dll", 1, 20, TRUE)) );

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
	EXPECT_TRUE( fSuccess &= (dbg.VerifyNotification("Loaded symbols", 3, 20, TRUE) && dbg.VerifyNotification("powdll.dll", 1, 20, TRUE)) );

	dbg.StopDebugging();
	EXPECT_TRUE(fSuccess &= (bps.SetBreakpoint("{Dec,,dec2.dll}TagCallPow") != NULL) );
	dbg.Go();
	EXPECT_TRUE ( fSuccess &= dbg.Go("{Dec,,dec2.dll}TagCallPow") );
	EXPECT_TRUE(fSuccess &= (bps.SetBreakpoint("dPow") != NULL));
	EXPECT_TRUE ( fSuccess &= dbg.StepOver(1, "dPow") );
	return fSuccess;	
}

BOOL Cexe_dllsIDETest::DllStepOut(   )
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

BOOL Cexe_dllsIDETest::DllTraceOut(   )
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

BOOL Cexe_dllsIDETest::GoFromWithinDll(   )
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

BOOL Cexe_dllsIDETest::RestartFromWithinDll(   )
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

BOOL Cexe_dllsIDETest::StepOverFuncInUnloadedDll(   )
{
 	LogTestHeader( "StepOverFuncInUnloadedDll" );
	BOOL fSuccess = TRUE;
	dbg.Restart();
	bps.ClearAllBreakpoints();

	UIWB.OpenFile( m_strProjectDir + "\\" + "dllappx.c");
	EXPECT_TRUE( fSuccess &= src.Find("Dec( &wValue );") );
	EXPECT_TRUE( fSuccess &= dbg.StepToCursor(0, NULL, "Dec( &wValue );", "MainWndProc") );
	EXPECT_TRUE(fSuccess &= dbg.StepOver(1, NULL, NULL, "MainWndProc"));
	EXPECT_TRUE( fSuccess &= (dbg.VerifyNotification("Loaded symbols", 3, 20, TRUE) && dbg.VerifyNotification("powdll.dll", 1, 20, TRUE)) );

	dbg.StopDebugging();
	return fSuccess;	
}


