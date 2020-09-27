///////////////////////////////////////////////////////////////////////////////
//	DisAcase.CPP
//											 
//	Created by :			
//		dklem
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "DisAcase.h"	
#include "process.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
											   
IMPLEMENT_TEST(/*999,*/ CDisAsmIDETest, CDbgTestBase, "General", -1, CDisAsmSubSuite)

// each function that runs a test has this prototype.
typedef void (CDisAsmIDETest::*PTR_TEST_FUNC)(void); 

// Information about a test to be put in the test header
#define LOGTESTHEADER(strTestName) 	m_strLogInfo.Format("# - %d.  %s. Total # - %d, Passed - %d %%", ++m_TestNumber, strTestName, m_TotalNumberOfTests, (m_TestNumber*100)/m_TotalNumberOfTests);  \
									LogTestHeader(m_strLogInfo);

//#define XBOXBUG_INCRLINK

// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.

PTR_TEST_FUNC tests[] = {
	&CDisAsmIDETest::HitBreakPoint,
	&CDisAsmIDETest::SourceAnnotation,
	&CDisAsmIDETest::GoToSymbol,
	&CDisAsmIDETest::SwitchDAMtoSRCandBack,
	&CDisAsmIDETest::TraceIntoCall,
	&CDisAsmIDETest::SwitchSRCtoDAMMultiInstance,
	&CDisAsmIDETest::GoToMaxMinAddress,
	&CDisAsmIDETest::GoToInvalidAddress,
	// TODO: (dklem 09/10/98) Uncomment the following 1 line when #7924 get fixed. Fixed!
	// TODO: (dklem 12/03/98) Uncomment the following 1 line and write a new code for this test when #11382 fixed.
	//&CDisAsmIDETest::OpenDAMWhileChildIsRunning,
	&CDisAsmIDETest::CodeBytes,
	&CDisAsmIDETest::SetFocusToThread,
	&CDisAsmIDETest::GoToOverloadedSymbol,
	&CDisAsmIDETest::GoAfterScrollDisassemblyWndModified,
	&CDisAsmIDETest::CheckAsmSourceAndDAMConsistency,
	&CDisAsmIDETest::ToolbarToDAMExpression,
	&CDisAsmIDETest::StepIntoStepOver,
	&CDisAsmIDETest::VerifyDockingView,
};


PTR_TEST_FUNC tests_woutcvinfo[] = {
	
	// TODO: (dklem 11/06/98) Uncomment the following line when #9202 is fixed.  Fixed in 8313.4
	&CDisAsmIDETest::HitBreakPointNoCVINFO,
	&CDisAsmIDETest::NavigateStackNoCVINFO,
	&CDisAsmIDETest::PgUpDownCtrlHomeEndNoCVINFO,
	// TODO: (dklem 12/16/98) Uncomment the following 1 line when #12874 is fixed
	&CDisAsmIDETest::SetNextStatementViaCtrlShiftF7NoCVINFO,
	&CDisAsmIDETest::RedisplayingAfterEditingRegistersAndMemoryWindowsNoCVINFO
	
	
};


// holds the asm we expect to see.
CStringArray csDamWindowTextAtTemplateFooLine;

												 
void CDisAsmIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDisAsmIDETest::Run()
{	 
	/******************
	 * INITIALIZATION *
	 ******************/
	PopulateDamWindowText();

	if (!CreateXboxProjectFromSource("disasm", "src\\disasm",
				"disasm.cpp", "damthread.c"))
			return;
	
	// indexes into the test function array.
	int iTest;
	
	// all tests expect the ide to be in this initial state.
	EXPECT_TRUE(dbg.StepOver());
	
	// In the tests it is assumed that the initial DAM window has Docking View turned on
	EXPECT_TRUE(uidam.EnableDockingView(FALSE));
	EXPECT_TRUE(dbg.ToggleSourceAnnotation(TRUE));

	m_TestNumber = 0;
	m_TotalNumberOfTests = sizeof(tests) / sizeof(PTR_TEST_FUNC) + sizeof(tests_woutcvinfo) / sizeof(PTR_TEST_FUNC) + 2;
	// + 2 = 1 (DebugJIT) + 1 (GoAfterScrollDisassemblyWndModified() contains GoToMaxMinAddress() )

	// we only want to randomly run each test once.
	RandomNumberCheckList rncl(sizeof(tests) / sizeof(PTR_TEST_FUNC));

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

	// randomly run each test once.
	while((iTest = rncl.GetNext()) != -1)
	if (0)
		(this->*(tests[iTest]))();

	dbg.StopDebugging();

#pragma message("***Without CVINFO, its all screwy, revisit!")
	return;

	// for xbox
	WaitStepInstructions("End of CV tests, sleeping for 20 seconds.");
	Sleep(20000);

	// Rebuild without cvinfo
	prj.SetTarget((CString)m_strProjectName + " - Xbox " +
						GetLocString(IDSS_BUILD_RELEASE));

	// set remote path before building, so autocopy works
	EXPECT_SUCCESS(prj.SetRemotePath(REMOTE_XBOX_FILE));

	// now build
	EXPECT_SUCCESS(prj.Build());

	prj.SetLocateOtherDLLs(TOGGLE_OFF);

	// all tests expect the ide to be in this initial state.
	//EXPECT_TRUE(dbg.StepOver(1, NULL, NULL, NULL, NOWAIT));
	EXPECT_TRUE(dbg.StepOver(1, NULL, NULL, NULL, NOWAIT));

	// The tests that checks if the dialog "exe does not contain debugging information ...Show this message [ ]" comes up
	COApplication appRel;
	if(!appRel.Attach("Microsoft Developer Studio", 5))
	{
	//	TODO: (dklem 09/22/98) Uncomment the following 1 line when #6954 gets fixed.  Fixed in 8310.3
		LOG->RecordFailure("The dialog \"exe does not contain debugging information ...Show this message [ ]\" did not come up");
	}
	else
	{
		MST.WCheckCheck("@1");
		// Click OK on the dialog "exe does not contain debugging information ...Show this message [ ]" 
		appRel.SendKeys("{ENTER}");	
	}

	// we only want to randomly run each test once.
	RandomNumberCheckList rncl1(sizeof(tests_woutcvinfo) / sizeof(PTR_TEST_FUNC));

	if(CMDLINE->GetBooleanValue("random", TRUE))
	{
		// randomly run each test once.
		while((iTest = rncl1.GetNext()) != -1)
			(this->*(tests_woutcvinfo[iTest]))();
	}
	else
	{
		// run each test in sequence.
		for(iTest = 0; iTest < sizeof(tests_woutcvinfo) / sizeof(PTR_TEST_FUNC); iTest++)
			(this->*(tests_woutcvinfo[iTest]))();
	}

	dbg.StopDebugging(NOWAIT) ;
	prj.Close();

	//Debug retail exe.  JIT
	// TODO(michma - 10/22/98): re-enable when mc bug 8150 is fixed. manually verify it before re-enabling this test, since
	// bugs like this tend to not be fixed correctly the first time, and this bug hangs up the whole snap run.
	// remember to make the fixed ide your jit debugger when testing.
	//DebugJIT();

}	

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

void CDisAsmIDETest::SourceAnnotation(void)
{

	LOGTESTHEADER("SourceAnnotation");

	bool bInitiallyMaximized;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	int nLinesInDamCode = sizeof(csDamWindowTextAtTemplateFooLine)/sizeof(csDamWindowTextAtTemplateFooLine[0]);

	uidam.Activate();

	// We need to maximize the DAM window to be able to read all csDamWindowTextAtTemplateFooLine[] instructions
	HWND hwnd = uidam.HWnd();

	if(IsZoomed(hwnd))
		bInitiallyMaximized = TRUE;
	else 
	{
		bInitiallyMaximized = FALSE;
		ShowWindow(hwnd, SW_MAXIMIZE);
	}

	// TODO: (dklem 02/04/99) MSTEST doesn't work correctly on W2K.  If we do {DOWN 50} and then {UP 50 }
	// the caret sometimes doesn't return to the same position 
	// so we check just 10 lines
	nLinesInDamCode = 10;

	// Read instrunctions and compare them with csDamWindowTextAtTemplateFooLine[]
	for(int ii = 0; ii < nLinesInDamCode; ii++) 
		EXPECT_TRUE(dam.InstructionContains(ii - 2, csDamWindowTextAtTemplateFooLine[ii], FALSE));

	// Restore the DAM window before restarting
	if(!bInitiallyMaximized)
		ShowWindow(hwnd, SW_RESTORE);

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

	// Check that Source annotation off works
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	// Activate and maximize for the next test
	uidam.Activate();
	hwnd = uidam.HWnd();

	if(IsZoomed(hwnd))
		bInitiallyMaximized = TRUE;
	else 
	{
		bInitiallyMaximized = FALSE;
		ShowWindow(hwnd, SW_MAXIMIZE);
	}


	// Turn Source annotation off
	// TODO: (dklem 09/17/98) Uncomment the following 1 line when #6811 gets fixed
	EXPECT_TRUE(uidam.ChooseContextMenuItem(SOURCE_ANNOTATION));

	// Check that there are no Soure code lines.  We assume that each line in csDamWindowTextAtTemplateFooLine[]
	// which begins with space  or \t is a Source code line
	// Read instrunctions and compare them with csDamWindowTextAtTemplateFooLine[]
	for(int jj = ii = 0; jj < nLinesInDamCode; ii++, jj++) 
	{
		// check if this is a Source code line
		while(csDamWindowTextAtTemplateFooLine[jj].GetAt(0) == (char) ' ' || csDamWindowTextAtTemplateFooLine[jj].GetAt(0) ==  '\t') 
			if(++jj >= nLinesInDamCode - 1)
				break;

	// TODO: (dklem 09/17/98) Uncomment the following 2 lines when #6774 gets fixed.  Fixed in 8337.0
			if(jj < nLinesInDamCode)
				EXPECT_TRUE(dam.InstructionContains(ii - 1, csDamWindowTextAtTemplateFooLine[jj], FALSE));

	}

	// Turn Source annotation on
	// TODO: (dklem 09/17/98) Uncomment the following 1 line when #6811 gets fixed
	EXPECT_TRUE(uidam.ChooseContextMenuItem(SOURCE_ANNOTATION));
	EXPECT_TRUE(dam.GotoSymbol("FuncWithArg"));
	EXPECT_TRUE(dam.InstructionContains(-1, "int FuncWithArg(int nArg)", FALSE));

	// Restore the DAM window
	if(!bInitiallyMaximized)
		ShowWindow(hwnd, SW_RESTORE);

	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

}

void CDisAsmIDETest::GoToSymbol(void)
{
	LOGTESTHEADER("GoToSymbol");

	CString csAddress, InstructionSaved;
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();
	// We need this Step Over to locate caret at the current addres
	EXPECT_TRUE(dbg.StepOver());
	InstructionSaved = dam.GetInstruction();
	csAddress = InstructionSaved.Left(8);

	EXPECT_TRUE(dam.GotoSymbol("FuncWithArg"));
	EXPECT_TRUE(dam.InstructionContains(-1, "int FuncWithArg(int nArg)", FALSE));
	EXPECT_TRUE(dam.InstructionContains(1, "push        ebp", FALSE));
	
	EXPECT_TRUE(dam.GotoSymbol(csAddress));

	//* We need this Step Over to locate caret at the current addres
	//* EXPECT_TRUE(dbg.StepOver());
	// TODO: (dklem 09/09/98) Uncomment the following 1 line when #6403 gets fixed. Fixed in Build 8296.1
	EXPECT_TRUE(dam.InstructionContains(0, InstructionSaved, FALSE));

	//Change address to hex
	csAddress = "0x" + csAddress;
	EXPECT_TRUE(dam.GotoSymbol("FuncWithArg"));
	EXPECT_TRUE(dam.GotoSymbol(csAddress));

	//* EXPECT_TRUE(dbg.StepOver());
	// TODO: (dklem 09/09/98) Uncomment the following 1 line when #6403 gets fixed. Fixed in Build 8296.1
	EXPECT_TRUE(dam.InstructionContains(0, InstructionSaved, FALSE));

	// Toggle Hex mode and do the previous tests to verify that hex mode doesn't affect them
	EXPECT_TRUE(dbg.ToggleHexDisplay(TRUE));
	
	EXPECT_TRUE(dam.GotoSymbol("FuncWithArg"));
	EXPECT_TRUE(dam.GotoSymbol(csAddress));
	//* EXPECT_TRUE(dbg.StepOver());
	// TODO: (dklem 09/09/98) Uncomment the following 1 line when #6403 gets fixed. Fixed in Build 8296.1
	EXPECT_TRUE(dam.InstructionContains(0, InstructionSaved, FALSE));
	
	//Change address to dec
	csAddress = csAddress.Right(8);
	EXPECT_TRUE(dam.GotoSymbol("FuncWithArg"));
	EXPECT_TRUE(dam.GotoSymbol(csAddress));

	//* EXPECT_TRUE(dbg.StepOver());
	// TODO: (dklem 09/09/98) Uncomment the following 1 line when #6403 gets fixed. Fixed in Build 8296.1
	EXPECT_TRUE(dam.InstructionContains(0, InstructionSaved, FALSE));

	// Toggle Hex mode back
	EXPECT_TRUE(dbg.ToggleHexDisplay(FALSE));

	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
}

void CDisAsmIDETest::HitBreakPoint(void)
{
	LOGTESTHEADER("HitBreakPoint");

	CString csAddress, InstructionSaved;
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	// In this part we set a BP at FuncWithArg using F9

	EXPECT_TRUE(dam.GotoSymbol("FuncWithArg"));
	EXPECT_TRUE(dam.InstructionContains(-1, "int FuncWithArg(int nArg)", FALSE));
	EXPECT_TRUE(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK));
	CheckThatIPisAtFuncWithArg();

	// In this part we Restart and Go and see in the a BP at FuncWithArg is hit
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK));
	CheckThatIPisAtFuncWithArg();

	// In this part we Stop and Go and see in the a BP at FuncWithArg is hit. We also toggle the BP off
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK));
	// Toggle breakpoint off
	EXPECT_TRUE(uidam.ChooseContextMenuItem(TOGGLE_BREAKPOINT));
	CheckThatIPisAtFuncWithArg();

	// In this part we set a BP at Func using Context Menu and Go.
	// We are trying to kill two birds here:
	// 1. Check if BP at FuncWithArg was actully toggled off, so we pass it
	// 2. BP at Func was set and hit
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dam.GotoSymbol("Func"));
	EXPECT_TRUE(dam.InstructionContains(-1, "int Func(double dArg)", FALSE));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(INSERT_REMOVE_BREAK_POINT));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK));
	// We need to make step over in the DAM window, so we activate
	dam.Enable();
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.InstructionContains(-3, "int Func(double dArg)", FALSE));

	// In this part we toggle BP at FuncWithArg on.  Check this and remove it.
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dam.GotoSymbol("FuncWithArg"));
	EXPECT_TRUE(dam.InstructionContains(-1, "int FuncWithArg(int nArg)", FALSE));
	// Toggle breakpoint on
	EXPECT_TRUE(uidam.ChooseContextMenuItem(TOGGLE_BREAKPOINT));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK));
	// Remove breakpoint
	EXPECT_TRUE(uidam.ChooseContextMenuItem(INSERT_REMOVE_BREAK_POINT));
	CheckThatIPisAtFuncWithArg();

	// In this part we check that BP at FuncWithArg was removed.
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK));
	// We need to make step over in the DAM window, so we activate
	dam.Enable();
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.InstructionContains(-3, "int Func(double dArg)", FALSE));

	//Remove all BPs
	EXPECT_TRUE(bps.ClearAllBreakpoints());

	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
}

void CDisAsmIDETest::CheckThatIPisAtFuncWithArg(void)
{

	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(dam.InstructionContains(-2, "int FuncWithArg(int nArg)", FALSE));

	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC());
	EXPECT_TRUE(!dam.IsActive());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.VerifyCurrentLine("return nArg + 2;"));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC());
	EXPECT_TRUE(dam.IsActive());
	EXPECT_TRUE(dbg.StepOver());
	// TODO (dklem 02/01/99)  ERROR.  Looks like a bug in 60.
	//'mov         al,byte ptr [chJustChar]' looks as 'mov         al,byte ptr [ebp - whatever]'
	// The following 1 line commented because this bug
	//	EXPECT_TRUE(dam.InstructionContains(-1, "mov         eax,dword ptr [nArg]", FALSE));
	EXPECT_TRUE(dam.InstructionContains(-2, "return nArg + 2;", FALSE));

}

void CDisAsmIDETest::SwitchDAMtoSRCandBack(void)
{
	LOGTESTHEADER("SwitchDAMtoSRCandBack");
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	
	dam.Enable();

	for(BOOL bUsingContextMenu = FALSE; bUsingContextMenu <= TRUE; bUsingContextMenu++)
		for(int ii = 0; ii < 4; ii++)
		{
			EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(bUsingContextMenu));
			EXPECT_TRUE(!dam.IsActive());
			EXPECT_TRUE(dbg.VerifyCurrentLine("TemplateFoo <char>(chJustChar);		/* First line for tests */"));
			EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(bUsingContextMenu));
			EXPECT_TRUE(dam.IsActive());
			//Sleep(500);
			EXPECT_TRUE(dam.InstructionContains(-1, "TemplateFoo <char>(chJustChar);     /* First line for tests */", FALSE));
												
			// TODO (dklem 02/01/99)  ERROR.  Looks like a bug in 60.
			//'mov         al,byte ptr [chJustChar]' looks as 'mov         al,byte ptr [ebp - whatever]'
			// The following 1 line commented because this bug
			// EXPECT_TRUE(dam.InstructionContains(0, "mov         al,byte ptr [chJustChar]", FALSE));

		}

	MinimalTestResizeMoveWindow();

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

}

void CDisAsmIDETest::SwitchSRCtoDAMMultiInstance(void)
{
	// Note that the Resolve Ambiguity verification implemented differently
	// in the GoToOverloadedSymbol() and in SwitchSRCtoDAMMultiInstance()
	// In SwitchSRCtoDAMMultiInstance() we find first instructions in each of TemplateFoo instances
	// by stepping in them and then verify that Resolve Ambiguity dialog brings us to the correct instructions.
	// In GoToOverloadedSymbol() we go to call TemplateFoo and check the address og the first instruction
	// and then verify that Resolve Ambiguity dialog brings us to the correct instructions.

	LOGTESTHEADER("SwitchSRCtoDAMMultiInstance");

	CString csResolveAmbiguityFunction[2];
	CString csInstructionInTemplateFoo[2];
	BOOL bFunctionFound;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	// First of all we want to find first instruction in TemplateFoo <char>(chJustChar);		
	EXPECT_TRUE(dbg.StepInto());
	csInstructionInTemplateFoo[0] = dam.GetInstruction();

	// Then we want to find first instruction in TemplateFoo <int>(nJustInt);	
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(FALSE));
	EXPECT_TRUE(dbg.StepOut());
	EXPECT_TRUE(dbg.StepInto(2));
	csInstructionInTemplateFoo[1] = dam.GetInstruction();

	// Now check that Resolve Ambiguity dialog brings us to the correct instructions.
	csResolveAmbiguityFunction[0] = "TemplateFoo(char)";
	csResolveAmbiguityFunction[1] = "TemplateFoo(int)";

	for(int jj = 0; jj < 2; jj++)
	{
		dam.Disable();
		EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(FALSE));
		EXPECT_TRUE(dbg.ResolveSourceLineAmbiguity(csResolveAmbiguityFunction[jj]));

		//EXPECT_TRUE(dam.InstructionContains(1, csInstructionInTemplateFoo[jj], FALSE));

		bFunctionFound = FALSE;
		for(int ii = 0; ii < 10; ii++)
			if(	TRUE == (bFunctionFound = dam.InstructionContains(ii, csInstructionInTemplateFoo[jj], FALSE)))
				break;
		if(!bFunctionFound)
			LOG->RecordFailure("Address %s for %s was not found in %d lines", csInstructionInTemplateFoo[jj], csResolveAmbiguityFunction[jj], ii);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ delete until this line ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}

	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
}


void CDisAsmIDETest::TraceIntoCall(void)
{
	LOGTESTHEADER("TraceIntoCall");

	CString csAddress, csAddress1;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* Second line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	// Trace into call
	EXPECT_TRUE(dam.InstructionContains(-1, "FuncWithArg(1);", FALSE));
	EXPECT_TRUE(dam.InstructionContains(0, "push        1", FALSE));
	EXPECT_TRUE(dbg.StepInto());
	// Get address
	if(csAddress = dam.GetInstruction())
	{
		csAddress.MakeUpper();
		csAddress = csAddress.Right(csAddress.GetLength() - csAddress.Find("FUNCWITHARG"));
		csAddress = csAddress.Right(csAddress.GetLength() - csAddress.Find("(") - 1);
		csAddress = csAddress.Left(csAddress.FindOneOf("h)"));
	}

	// Check SHOW_NEX_STATEMENT
	EXPECT_TRUE(uidam.ChooseContextMenuItem(SHOW_NEX_STATEMENT));
	EXPECT_TRUE(dam.InstructionContains(0, "call", FALSE));
#ifdef XBOXBUG_INCRLINK
	EXPECT_TRUE("Test is running modified for Xbox incremental link bug");
	EXPECT_TRUE(dam.InstructionContains(0, "FuncWithArg (", FALSE));
#else
	EXPECT_TRUE(dam.InstructionContains(0, "(FuncWithArg) (", FALSE));
#endif
	

#ifdef XBOXBUG_INCRLINK
#else
	EXPECT_TRUE(dbg.StepInto());
	// Get address
	if(csAddress1 = dam.GetInstruction())
	{
		csAddress1.MakeUpper();
		// Check that the address is correct
		EXPECT_TRUE( -1 != (csAddress1.Left(8)).Find(csAddress));
		csAddress1 = csAddress1.Right(csAddress1.GetLength() - csAddress1.Find("FUNCWITHARG"));
		csAddress1 = csAddress1.Right(csAddress1.GetLength() - csAddress1.Find("(") - 1);
		csAddress1 = csAddress1.Left(csAddress1.FindOneOf("h)"));
	}

	EXPECT_TRUE(dam.InstructionContains(0, "jmp         FuncWithArg (", FALSE));
#endif

	EXPECT_TRUE(dbg.StepInto());
	
	// Check that the address is correct
	if(csAddress = dam.GetInstruction())
	{
		csAddress1.MakeUpper();	
		EXPECT_TRUE( -1 != (csAddress.Left(8)).Find(csAddress1));
	}

	EXPECT_TRUE(dam.InstructionContains(-2, "int FuncWithArg(int nArg)", FALSE));
	EXPECT_TRUE(dbg.StepOut());
	EXPECT_TRUE(dam.InstructionContains(1, "Func(dJustDouble);", FALSE));
	
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
}


void CDisAsmIDETest::GoToMaxMinAddress(void)
{

	LOGTESTHEADER("GoToMaxMinAddress");

	CString csAddress, csInstruction;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	EXPECT_TRUE(dam.GotoSymbol("00000000"));
	// TODO: (dklem 09/09/98) Change uidam.GetInstruction() to dam.GetInstruction() when #6013 gets fixed
	// #6013 was split into severeal bugs.  New bug for this case is #7887
	//csInstruction = uidam.GetInstruction(0, 1, TRUE);
	csInstruction = dam.GetInstruction();

	EXPECT_TRUE("00000000" == (csAddress = csInstruction.Left(8)));
	LOG->RecordInfo("CurrentAddress is %s", csAddress);

	// TODO: (dklem 10/30/98) uncomment the following line when #8781 gets fixed
	EXPECT_TRUE(dam.GotoSymbol("FFFFFFFF"));

	// TODO: (dklem 09/09/98) Change uidam.GetInstruction() to dam.GetInstruction() when #6013 gets fixed
	// #6013 was split into severeal bugs.  New bug for this case is #7887
	//csInstruction = uidam.GetInstruction(0, 1, TRUE);
	csInstruction = dam.GetInstruction();

	// TODO: (dklem 10/30/98) uncomment the following line when #8781 gets fixed
	EXPECT_TRUE("FFFFFFFF" == (csAddress = csInstruction.Left(8)));
	LOG->RecordInfo("CurrentAddress is %s", csAddress);
	
	// Resize and Move
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
}


void CDisAsmIDETest::GoAfterScrollDisassemblyWndModified(void)
{

	LOGTESTHEADER("GoAfterScrollDisassemblyWndModified");

	for ( int i = 0; i < 200; i++)
	{
		MST.DoKeys ("+{pgdn}",FALSE,2000); //wait for idling
		if (MST.WFndWndWait("Browse for Folder", FW_NOCASE | FW_PART,0)
			|| MST.WFndWndWait("Find Source", FW_NOCASE | FW_PART,0))
			MST.WButtonClick( "Cancel" );
	}
		
	GoToMaxMinAddress();
}

void CDisAsmIDETest::GoToInvalidAddress(void)
{

	LOGTESTHEADER("GoToInvalidAddress");

	CString csAddress, csInstruction;
	HWND resulthWnd;

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	// TODO: (dklem 10/30/98) uncomment the following line when #8781 gets fixed
	EXPECT_TRUE(dam.GotoSymbol("FFFFFFFFF5"));
	// TODO: (dklem 09/09/98) Change uidam.GetInstruction() to dam.GetInstruction() when #6013 gets fixed
	// #6013 was split into severeal bugs.  New bug for this case is #7887
	// csInstruction = uidam.GetInstruction(0, 1, TRUE);
	csInstruction = dam.GetInstruction();

	// TODO: (dklem 10/30/98) uncomment the following line when #8781 gets fixed
	EXPECT_TRUE("FFFFFFF5" == (csAddress = csInstruction.Left(8)));
	LOG->RecordInfo("CurrentAddress is %s", csAddress);

	EXPECT_TRUE(dam.GotoSymbol("PRIVET", FALSE));

	EXPECT_TRUE(NULL != (resulthWnd = MST.WFndWnd("Microsoft Visual C++", FW_NOCASE | FW_EXIST)));
	if(NULL != resulthWnd)
	{
		MST.WButtonClick("OK");
	}
	dam.Enable();
	// TODO: (dklem 09/09/98) Change uidam.GetInstruction() to dam.GetInstruction() when #6013 gets fixed
	// #6013 was split into severeal bugs.  New bug for this case is #7887
	// csInstruction = uidam.GetInstruction(0, 1, TRUE);
	// TODO: (dklem 10/30/98) uncomment the following line when #8781 gets fixedcsInstruction = dam.GetInstruction();
	EXPECT_TRUE("FFFFFFF5" == (csAddress = csInstruction.Left(8)));
	LOG->RecordInfo("CurrentAddress is %s", csAddress);

	// Resize and Move
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
}


void CDisAsmIDETest::OpenDAMWhileChildIsRunning(void)
{

	LOGTESTHEADER("OpenDAMWhileChildIsRunning");

	CString csInstruction;
	
	// Restart and Go.  Close DAM Open DAM
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));

	dam.Enable();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
	dam.Enable();
	csInstruction = dam.GetInstruction();
	// TODO: (dklem 11/03/98) Uncomment the following line when #8896 is fixed
	// EXPECT_TRUE(-1 != csInstruction.Find("(unavailable while child is running)"));

	// Stop and Go with DAM opened
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	dam.Enable();
	csInstruction = dam.GetInstruction();
	EXPECT_TRUE(-1 != csInstruction.Find("(unavailable while child is running)"));

	// Stop, Go and open DAM with Context menu
	// TODO: (dklem 09/10/98) Uncomment the following 5 line when #6530 and #6531 get fixed
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	EXPECT_TRUE(dam.SwitchBetweenASMAndSRC(TRUE));
	csInstruction = dam.GetInstruction();
	EXPECT_TRUE(-1 != csInstruction.Find("(unavailable while child is running)"));

	// Stop, start, Go and open DAM 
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.StepInto());
	dam.Enable();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	dam.Enable();
	csInstruction = dam.GetInstruction();
	// TODO: (dklem 11/03/98) Uncomment the following line when #8896 is fixed
	// EXPECT_TRUE(-1 != csInstruction.Find("(unavailable while child is running)"));

	// Stop, start, open DAM and Go 
// TODO: (dklem 09/10/98) Uncomment the following 5 line when #6534 gets fixed
//	EXPECT_TRUE(dbg.StopDebugging());
//	EXPECT_TRUE(dbg.StepInto());
//	dam.Enable();
//	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
//	csInstruction = dam.GetInstruction();
//	EXPECT_TRUE(-1 != csInstruction.Find("(unavailable while child is running)"));

	// We can not restart neither when child is running nor when debugger is not started, so we stop and start it into 2 steps
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.StepInto());

	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
	
}


void CDisAsmIDETest::CodeBytes(void)
{

	LOGTESTHEADER("CodeBytes");

	CString csAddress, csInstruction, csCodeBytes;
	char csCodeBytesInMemWindow[64];
	
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CODE_BYTES));

	for(int ii = 0; ii < 2; ii++)
	{
		EXPECT_TRUE(dbg.StepInto());
		csInstruction = dam.GetInstruction();
		csAddress = csInstruction.Left(8);
		csCodeBytes = csInstruction.Mid(9, 20);
		csCodeBytes.TrimLeft();
		csCodeBytes.TrimRight();
		mem.GetMemoryData(csAddress, csCodeBytesInMemWindow, MEM_FORMAT_BYTE, csCodeBytes.GetLength());
		csCodeBytesInMemWindow[csCodeBytes.GetLength()]  ='\0';
		LOG->Comment("csInstruction=[%s], csAddress=[%s], csCodeBytes=[%s]", csInstruction, csAddress, csCodeBytes);
		EXPECT_TRUE(0 == csCodeBytes.CompareNoCase(csCodeBytesInMemWindow));
	}
	
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CODE_BYTES));
	EXPECT_TRUE(!dam.InstructionContains(0, csCodeBytes, FALSE));

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CODE_BYTES));
	EXPECT_TRUE(dam.InstructionContains(0, csCodeBytes, FALSE));

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CODE_BYTES));
	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
}



void CDisAsmIDETest::SetFocusToThread(void)
{
	// xbox: changed MessageBeep to SetLastError

	LOGTESTHEADER("SetFocusToThread");

	BOOL bFileSeparatorFound = FALSE; 
			
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(ERROR_SUCCESS == src.Open("damthread.c")); 
	EXPECT_TRUE(src.Find("SetLastError(1);", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.VerifyCurrentLine("SetLastError(1);"));

	dam.Enable();

	EXPECT_TRUE(dam.InstructionContains(-1, "SetLastError(1);", FALSE));
	EXPECT_TRUE(dam.InstructionContains(0, "push        1", FALSE));
	EXPECT_TRUE(dam.InstructionContains(1, "call        SetLastError", FALSE));
	/*
	EXPECT_TRUE(dam.InstructionContains(-1, "MessageBeep(1);", FALSE));
	EXPECT_TRUE(dam.InstructionContains(0, "mov         esi,esp", FALSE));
	EXPECT_TRUE(dam.InstructionContains(1, "push        1", FALSE));
	// One more check for a mangeled name
	EXPECT_TRUE(dam.InstructionContains(2, "call        dword ptr [__imp__MessageBeep@4", FALSE));
	*/


#if 1
#pragma message("Excluding code that checks for file separators, Xbox binaries dont match Win32 ones")
	EXPECT_TRUE("Excluding code that checks for file separators, Xbox binaries dont match Win32 ones");
#else
	// Check File separators
	EXPECT_TRUE(dam.GotoSymbol("ThreadRoutine"));
	// Using the next statement we just scroll 1 line up.  Actually it will be 2 lines scroll, 
	// since file separator is skipped while scrolling
	//dam.InstructionContains(-1, "", FALSE);
	EXPECT_TRUE(dam.InstructionContains(-3, "disasm xbe\\damthread.c  ---", FALSE));

	EXPECT_TRUE(dam.GotoSymbol("WinMain"));
	// Using the next statement we just scroll 8 lines up.  Actually it will be 9 lines scroll, 
	// since file separator is skipped while scrolling
	//dam.InstructionContains(-8, "", FALSE);

	for(int ii = 0; ii > -10; ii--)
		if(	TRUE == (bFileSeparatorFound = dam.InstructionContains(ii, m_strProjectName + ".cpp" + "  ---", FALSE)))
			break;
	if(!bFileSeparatorFound)
			LOG->RecordFailure("File separator for %s was not found in %d lines", m_strProjectName + ".cpp", ii);
#endif

	//Remove all BPs
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_TRUE(ERROR_SUCCESS == src.Open(m_strProjectName + ".cpp"));

	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
}


void CDisAsmIDETest::GoToOverloadedSymbol(void)
{

	// Note that the Resolve Ambiguity verification implemented differently
	// in the GoToOverloadedSymbol() and in SwitchSRCtoDAMMultiInstance()
	// In SwitchSRCtoDAMMultiInstance() we find first instructions in each of TemplateFoo instances
	// by stepping in them and then verify that Resolve Ambiguity dialog brings us to the correct instructions.
	// In GoToOverloadedSymbol() we go to call TemplateFoo and check the address og the first instruction
	// and then verify that Resolve Ambiguity dialog brings us to the correct instructions.


	CString csInstruction, csAddress;
	CString csResolveAmbiguityFunction[2];
	CString csResolveAmbiguityInstruction[2];
	
	LOGTESTHEADER("GoToOverloadedSymbol");

	BOOL bFuncltionFound; 

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	csResolveAmbiguityFunction[0] = "OverloadedFunc(int)";
	csResolveAmbiguityInstruction[0] = "void OverloadedFunc(int nArg)";

	csResolveAmbiguityFunction[1] = "OverloadedFunc(void)";
	csResolveAmbiguityInstruction[1] = "void OverloadedFunc(void)";

	// Test Overloaded Functions
	for(int jj = 0; jj < 2; jj++)
	{
		dam.Enable();
		dam.GotoSymbol("OverloadedFunc", FALSE);
		EXPECT_TRUE(dbg.ResolveAmbiguity(csResolveAmbiguityFunction[jj]));
		dam.Enable();

		// TODO: (dklem 09/16/98) Uncomment the following line when #6013 gets fixed
		// #6013 was split into severeal bugs.  New bug for this case is #7884 (related bug #7887)
		// EXPECT_TRUE(dam.InstructionContains(0, "void OverloadedFunc(int nArg)", FALSE));
		// TODO: (dklem 09/16/98) Remove the following 5 lines when #6013 gets fixed
		// #6013 was split into severeal bugs.  New bug for this case is #7884 (related bug #7887)
		for(int ii = 0, bFuncltionFound = FALSE; ii > -20; ii--)
			if(	TRUE == (bFuncltionFound = dam.InstructionContains(ii, csResolveAmbiguityInstruction[jj], FALSE)))
				break;
		if(!bFuncltionFound)
				LOG->RecordFailure("Function %s was not found in %d lines", csResolveAmbiguityInstruction[jj], ii);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ delete until this line ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

	// The same test for Template Functions
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	
	csResolveAmbiguityFunction[0] = "TemplateFoo(char)";
	csResolveAmbiguityFunction[1] = "TemplateFoo(int)";

	for(jj = 0; jj < 2; jj++)
	{
		dam.Enable();
		EXPECT_TRUE(dbg.StepInto(3));

		if(csAddress = dam.GetInstruction())
		{
			csAddress = csAddress.Right(csAddress.GetLength() - csAddress.Find("TemplateFoo"));
			csAddress = csAddress.Right(csAddress.GetLength() - csAddress.Find("(") - 1);
#ifdef XBOXBUG_INCRLINK
			csAddress = csAddress.Left(csAddress.FindOneOf(")"));
#else
			csAddress = csAddress.Left(csAddress.FindOneOf("h)"));
#endif
			csAddress.MakeUpper();
		}

		dam.GotoSymbol("TemplateFoo", FALSE);
		EXPECT_TRUE(dbg.ResolveAmbiguity(csResolveAmbiguityFunction[jj]));
		dam.Enable();

		// TODO: (dklem 09/16/98) Uncomment the following line when #6013 gets fixed
		// #6013 was split into severeal bugs.  New bug for this case is #7884 (related bug #7887)
		// EXPECT_TRUE(dam.InstructionContains(0, csAddress, FALSE));
		// TODO: (dklem 09/16/98) Remove the following 5 lines when #6013 gets fixed
		// #6013 was split into severeal bugs.  New bug for this case is #7884 (related bug #7887)
		for(int ii = 0; ii < 10 ; ii++)
			if(	TRUE == (bFuncltionFound = dam.InstructionContains(ii, csAddress, FALSE)))
				break;
		if(!bFuncltionFound)
			LOG->RecordFailure("Address %s for %s was not found in %d lines", csAddress, csResolveAmbiguityFunction[jj], ii);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ delete until this line ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		EXPECT_TRUE(dbg.StepOver());
		EXPECT_TRUE(dbg.StepOut());
		EXPECT_TRUE(dbg.StepOver());
	}

	MinimalTestResizeMoveWindow();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

}

void CDisAsmIDETest::CheckAsmSourceAndDAMConsistency(void)
{
	LOGTESTHEADER("CheckAsmSourceAndDAMConsistency");

	BOOL bFuncltionFound = FALSE; 

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("__asm {", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	EXPECT_TRUE(dam.InstructionContains(0, "push        eax", FALSE));
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dam.InstructionContains(0, "pop         eax", FALSE));

	// The following 4 lines are bug 3879 related
	// TODO (dklem 02/01/99)  ERROR.  Looks like a bug in 60.
	//'mov         al,byte ptr [chJustChar]' looks as 'mov         al,byte ptr [ebp - whatever]'
	// The following 4 lines used to skip such lines
/*
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dam.InstructionContains(0, "lea         eax,[nJustInt]", FALSE));
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dam.InstructionContains(0, "mov         dword ptr [p],eax ", FALSE));
*/
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

}


void CDisAsmIDETest::ToolbarToDAMExpression(void)
{
	LOGTESTHEADER("ToolbarToDAMExpression");

	BOOL bFuncltionFound = FALSE; 

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("FuncWithArg", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	app.SendKeys("^F");
	app.SendKeys("^{F7}");	

	EXPECT_TRUE(dam.InstructionContains(-2, "FuncWithArg", FALSE));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
	
}


void CDisAsmIDETest::StepIntoStepOver(void)
{

	LOGTESTHEADER("StepIntoStepOver");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests ", FALSE, FALSE, FALSE, 0 )); // last argument 0 = FD_DOWN
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	// Read instrunctions and compare them with csDamWindowTextAtTemplateFooLine[]
	for(int ii = 2; ii < 4; ii++) {
		EXPECT_TRUE(dam.InstructionContains(0, csDamWindowTextAtTemplateFooLine[ii], FALSE));
		EXPECT_TRUE(dbg.StepOver());
	}

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests ", FALSE, FALSE, FALSE, 0 )); // last argument 0 = FD_DOWN
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	// Read instrunctions and compare them with csDamWindowTextAtTemplateFooLine[]
	for(ii = 2; ii < 4; ii++) {
		EXPECT_TRUE(dam.InstructionContains(0, csDamWindowTextAtTemplateFooLine[ii], FALSE));
		EXPECT_TRUE(dbg.StepInto());
	}

	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 )); // last argument 0 = FD_DOWN
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	// Read instrunctions and compare them with csDamWindowTextAtTemplateFooLine[]
	for(ii = 2; ii < 4; ii++) {
		// TODO: (dklem 10/8/98) Uncomment the following line when #4321 gets fixed
		EXPECT_TRUE(dam.InstructionContains(0, csDamWindowTextAtTemplateFooLine[ii], FALSE));
		EXPECT_TRUE(dbg.StepOver());
	}

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	dam.Enable();

	// Read instrunctions and compare them with csDamWindowTextAtTemplateFooLine[]
	for(ii = 2; ii < 4; ii++) {
		// TODO: (dklem 10/8/98) Uncomment the following line when #4321 gets fixed
		EXPECT_TRUE(dam.InstructionContains(0, csDamWindowTextAtTemplateFooLine[ii], FALSE));
		EXPECT_TRUE(dbg.StepInto());
	}


	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

}

void CDisAsmIDETest::RedisplayingAfterEditingRegistersAndMemoryWindowsNoCVINFO(void)
{
	LOGTESTHEADER("RedisplayingAfterEditingRegistersAndMemoryWindowsNoCVINFO");

	CString csAddress, InstructionSaved;
	char csCodeBytesInMemWindow[64];

	// Restart 
	EXPECT_TRUE(dbg.Restart());

	dam.Enable();

	// We need this Step Over to locate caret at the current address
	EXPECT_TRUE(dbg.StepOver());
	InstructionSaved = dam.GetLines(10, 1);
	csAddress = InstructionSaved.Left(8);

	// Change register
	EXPECT_TRUE(regs.SetRegister(EIP, csAddress)); 

	dam.Enable();

	// TODO: (dklem 09/22/98) Uncomment the following line when #6923 and #6924 get fixed. #6923 is fixed in 8308.1. #6924 is fixed in 8323.0
	EXPECT_TRUE(dam.InstructionContains(0, csAddress, FALSE));
	EXPECT_TRUE(dbg.StepOver());
	// TODO: (dklem 09/22/98) Uncomment the following line when #6923 gets fixed. Fixed in 8308.1
	EXPECT_TRUE(dam.InstructionContains(-1, csAddress, FALSE));

	// The test plan says that combination with DAM scrolling is interesting
	dam.Enable();

	PgUpDownCtrlHomeEnd();
	
	mem.GetMemoryData(csAddress, csCodeBytesInMemWindow, MEM_FORMAT_BYTE,1);
	csCodeBytesInMemWindow[2] = '\0';
	if(strcmp(csCodeBytesInMemWindow,"AA") == 0)
		strcpy(csCodeBytesInMemWindow, "FF");
	else
		strcpy(csCodeBytesInMemWindow, "AA");

	// TODO: (dklem 09/22/98) Uncomment the following line when #6923 gets fixed. Fixed in 8308.1
	EXPECT_TRUE(mem.SetMemoryData(csAddress, csCodeBytesInMemWindow, MEM_FORMAT_BYTE));
	
	dam.Enable();
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CODE_BYTES));
	EXPECT_TRUE(dam.GotoSymbol(csAddress));

	// TODO: (dklem 09/22/98) Change uidam.GetInstruction() to dam.GetInstruction() when #6013 gets fixed
	// #6013 was split into severeal bugs.  New bug for this case is #7887
	// InstructionSaved = uidam.GetInstruction(0, 1, TRUE);
	InstructionSaved = dam.GetInstruction();

	// Get two first code bytes
	InstructionSaved = InstructionSaved.Mid(9, 20);
	InstructionSaved.TrimLeft(" \t");
	InstructionSaved = InstructionSaved.Left(2);

	// TODO: (dklem 09/22/98) Uncomment the following line when #6923 and #6013 get fixed. #6923 is fixed in 8308.1
	EXPECT_TRUE(InstructionSaved == csCodeBytesInMemWindow);
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CODE_BYTES));

	// The test plan says that combination with DAM scrolling is interesting
	dam.Enable();
	PgUpDownCtrlHomeEnd();

}

void CDisAsmIDETest::HitBreakPointNoCVINFO(void)
{
	LOGTESTHEADER("HitBreakPointNoCVINFO");

	CString csAddress, InstructionSaved;
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());

	// We need this Step Over to locate caret at the current addres
	EXPECT_TRUE(dbg.StepOver());
	InstructionSaved = dam.GetLines(10, 1);
	csAddress = InstructionSaved.Left(8);

	EXPECT_TRUE(dam.GotoSymbol(csAddress));

	// TODO: (dklem 09/16/98) Remove the following line when #6013 gets fixed
	// #6013 was split into severeal bugs.  New bug for this case is #7887
	// uidam.GetInstruction(0, 1, TRUE); This line should be removed
	EXPECT_TRUE(uidam.ChooseContextMenuItem(SET_NEXT_STATEMENT));	

	EXPECT_TRUE(bps.SetBreakpoint());
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
	EXPECT_TRUE(dbg.StopDebugging(ASSUME_NORMAL_TERMINATION));

	dbg.StepOver();
	EXPECT_TRUE(bps.EnableAllBreakpoints());
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK));
	EXPECT_TRUE(dam.InstructionContains(0, csAddress, FALSE));

	//Remove all BPs
	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


void CDisAsmIDETest::SetNextStatementViaCtrlShiftF7NoCVINFO(void)
{
	LOGTESTHEADER("SetNextStatementViaCtrlShiftF7NoCVINFO");

	CString csAddress, InstructionSaved;
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());

	// We need this Step Over to locate caret at the current addres
	EXPECT_TRUE(dbg.StepOver());
	InstructionSaved = dam.GetLines(10, 1);
	csAddress = InstructionSaved.Left(8);

	EXPECT_TRUE(dam.GotoSymbol(csAddress));

	// TODO: (dklem 09/16/98) Remove the following line when #6013 gets fixed
	// #6013 was split into severeal bugs.  New bug for this case is #7887
	// uidam.GetInstruction(0, 1, TRUE); This line should be removed
	MST.DoKeys("^+{F7}");

	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dam.InstructionContains(-1, csAddress, FALSE));

}


void CDisAsmIDETest::NavigateStackNoCVINFO(void)
{
	LOGTESTHEADER("NavigateStackNoCVINFO");

	CString csAddress, InstructionSaved;
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
	EXPECT_TRUE(stk.NavigateStack(0));
	// Check if DAM window came up
	EXPECT_TRUE(dam.InstructionContains(0, "push        ebp", FALSE));

	//Check that DAM comes up when the debugee is restarted
	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dam.InstructionContains(0, "push        ebp", FALSE));
}

void CDisAsmIDETest::PgUpDownCtrlHomeEndNoCVINFO(void)
{

	LOGTESTHEADER("PgUpDownCtrlHomeEndNoCVINFO");
	
	// Restart 
	EXPECT_TRUE(dbg.Restart());
	dam.Enable();

	// Test RUN_TO_CURSOR
	EXPECT_TRUE(dam.InstructionContains(0, "push        ebp", FALSE));

	// TODO: (dklem 09/16/98) This is a dangerous place.  We can go out of scope, so RUN_TO_CURSOR will hang the child because of while(1) cycle
	MST.DoKeys("{PGDN}");

	// TODO: (dklem 09/16/98) Remove the following line when #6013 gets fixed
	// #6013 was split into severeal bugs.  New bug for this case is #7887
	// uidam.GetInstruction(0, 1, TRUE); This line should be removed
	EXPECT_TRUE(uidam.ChooseContextMenuItem(RUN_TO_CURSOR));
	EXPECT_TRUE(dbg.StepInto());
	// TODO (dklem 09/16/98) This won't work correct (result doesn't mean that we really moved to the new position) until #6774 gets fixed.  Fixed in 8337.0
	// Check that this gives an error if the following line: 
	// EXPECT_TRUE(uidam.ChooseContextMenuItem(RUN_TO_CURSOR));
	// above is commented ( when #6774 gets fixed )
	// The problem here is if RUN_TO_CURSOR doesn't work, StepInto will position EIP to the IP next to "push        ebp"
	// and this IP will be at the top of the DAM.  -1 in the next statement means pushing hte Up arrow.  However, since #6774
	// the IP will remain at the position next to "push        ebp" and the next statement doesn't make a sense.
	// So, try this test with commented EXPECT_TRUE(uidam.ChooseContextMenuItem(RUN_TO_CURSOR)); must give an error at the next line
	// If you shure that #6774 is fixed or doesn't exist, don't bother with this
	EXPECT_TRUE(!dam.InstructionContains(-1, "push        ebp", FALSE));
	
	// Return the caret to the original position.  This is necessary for the nex test PgUpDownCtrlHomeEnd()
	MST.DoKeys("{UP}");
	
	PgUpDownCtrlHomeEnd();
}

void CDisAsmIDETest::PgUpDownCtrlHomeEnd(void)
{

	CString csInitialAddress, csNewAddress, csNewAddress1;
	
	csInitialAddress = (dam.GetInstruction()).Left(8);
	
	// Check PhUp and PgDown
// TODO: (dklem 09/16/98) Uncomment the following /* */ block when #6763 is fixed
	MST.DoKeys("{PGUP 5}");
	csNewAddress = (dam.GetInstruction()).Left(8);
	EXPECT_TRUE(csNewAddress < csInitialAddress);

	MST.DoKeys("{PGDN 5}");
	csNewAddress = (dam.GetInstruction()).Left(8);
	EXPECT_TRUE(csNewAddress == csInitialAddress);

	MST.DoKeys("{PGDN 5}");
	csNewAddress = (dam.GetInstruction()).Left(8);
	EXPECT_TRUE(csNewAddress > csInitialAddress);

	MST.DoKeys("{PGUP 5}");
	csNewAddress = (dam.GetInstruction()).Left(8);
	// TODO: (dklem 09/16/98) Uncomment the following line when #6763 gets fixed
	EXPECT_TRUE(csNewAddress == csInitialAddress);

	// Check Ctrl+End and Ctrl+Home
/*	MST.DoKeys("^{END}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress);
	EXPECT_TRUE(csNewAddress > csInitialAddress);

	MST.DoKeys("^{HOME}");
	csNewAddress = (dam.GetInstruction()).Left(8);
	// TODO: (dklem 09/16/98) Uncomment the following line when #6774 gets fixed.  Fixed in 8337.0
	EXPECT_TRUE(csNewAddress == csInitialAddress);
*/
	// Check Arrows Up and Down
	MST.DoKeys("{HOME}{UP}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress);
	// TODO: (dklem 09/16/98) Uncomment the following line when #6774 gets fixed.  Fixed in 8337.0
	EXPECT_TRUE(csNewAddress < csInitialAddress);

	MST.DoKeys("{HOME}{UP}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	// TODO: (dklem 09/16/98) Uncomment the following line when #6774 gets fixed.  Fixed in 8337.0
	EXPECT_TRUE(csNewAddress1 < csNewAddress);

	MST.DoKeys("{HOME}{DOWN}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 == csNewAddress);

	MST.DoKeys("{HOME}{DOWN}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	// TODO: (dklem 09/16/98) Uncomment the following 2 lines when #6774 gets fixed.  Fixed in 8337.0
	EXPECT_TRUE(csNewAddress1 > csNewAddress);
	EXPECT_TRUE(csNewAddress1 == csInitialAddress);

	MST.DoKeys("{HOME}{DOWN}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress);
	// TODO: (dklem 09/16/98) Uncomment the following line when #6774 gets fixed.  Fixed in 8337.0
	EXPECT_TRUE(csNewAddress > csInitialAddress);

	MST.DoKeys("{HOME}{DOWN}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 > csNewAddress);

	MST.DoKeys("{HOME}{UP}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	EXPECT_TRUE(csNewAddress1 == csNewAddress);

	MST.DoKeys("{HOME}{UP}");
	MST.DoKeys("{HOME}+({RIGHT 8})^(c)");	// set cursor at beginning of line and copy first 8 characters to clipboard
	GetClipText(csNewAddress1);
	// TODO: (dklem 09/16/98) Uncomment the following line when #6774 gets fixed.  Fixed in 8337.0
	EXPECT_TRUE(csNewAddress1 == csInitialAddress);

	MinimalTestResizeMoveWindow();

}

void CDisAsmIDETest::VerifyDockingView(void)
{
	LOGTESTHEADER("VerifyDockingView");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(uidam.EnableDockingView(TRUE));
	EXPECT_TRUE(uidam.IsDockingViewEnabled());
	// TODO: (dklem 09/14/98) Uncomment the following line when #6572 gets fixed
	// The bug #6572 is Memory leak while resizing DAM window. This leak is not that bad in the Release version
	// Based on my tests the Debug vesion consumes ~71,000K (at the peak) while running this test suite, Release version - 23,000K (at the peak) , which is acceptable
	// Comment the next line if the Debug version of MSDEV causes Out of Memory
	 TestResizeMoveWindow(TRUE, TRUE);	

	EXPECT_TRUE(uidam.EnableDockingView(FALSE));
	EXPECT_TRUE(!uidam.IsDockingViewEnabled());
	// TODO: (dklem 09/14/98) Uncomment the following line when #6572 gets fixed
	// The bug #6572 is Memory leak while resizing DAM window. This leak is not that bad in the Release version
	// Based on my tests the Debug vesion consumes ~71,000K (at the peak) while running this test suite, Release version - 23,000K (at the peak) , which is acceptable
	// Comment the next line if the Debug version of MSDEV causes Out of Memory
	TestResizeMoveWindow(FALSE, TRUE);	

	EXPECT_TRUE(uidam.ChooseContextMenuItem(CLOSE_HIDE));

}


void CDisAsmIDETest::MinimalTestResizeMoveWindow()
{

// TODO remove the next line immediately
//	return;
	
	BOOL bInitiallyMaximized;

	uidam.Activate();
	HWND hwnd = uidam.HWnd();

	if(IsZoomed(hwnd))
	{
		bInitiallyMaximized = TRUE;
		ShowWindow(hwnd, SW_RESTORE);
	}
	else 
		bInitiallyMaximized = FALSE;
	
	

	// TODO: (dklem 09/14/98) Uncomment the following 4 lines when #6572 gets fixed
	// The bug #6572 is Memory leak while resizing DAM window. This leak is not that bad in the Release version
	// Based on my tests the Debug vesion consumes ~71,000K (at the peak) while running this test suite, Release version - 23,000K (at the peak) , which is acceptable
	// Comment the next 4 lines if the Debug version of MSDEV causes Out of Memory
	TestResizeMoveWindow(FALSE, FALSE);

	// Restore the DAM window before turning Docking View on
	if(bInitiallyMaximized)
		ShowWindow(hwnd, SW_MAXIMIZE);
	
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));

	TestResizeMoveWindow(TRUE, FALSE);
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
}

void CDisAsmIDETest::TestResizeMoveWindow(BOOL bIsDocked, BOOL bFullTest)
{
	int nPositionMoved;
	uidam.Activate();

	BOOL DragFullWindows = TRUE;
	EXPECT_TRUE(GetDragFullWindows(&DragFullWindows));

	// Try minimum hor and ver sizes
	nPositionMoved = uidam. ResizeWindow(bIsDocked, SLeft, DRight, -1, DragFullWindows);
	uidam.ResizeWindow(bIsDocked, SLeft, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uidam.ResizeWindow(bIsDocked, SBottom, DUp, -1, DragFullWindows);
	uidam.ResizeWindow(bIsDocked, SBottom, DDown, nPositionMoved, DragFullWindows);

	// Move the window
	nPositionMoved = uidam.MoveWindow(bIsDocked, DRight, 20, DragFullWindows);
	nPositionMoved = uidam.MoveWindow(bIsDocked, DDown, 20, DragFullWindows);
	nPositionMoved = uidam.MoveWindow(bIsDocked, DLeft, 20, DragFullWindows);
	nPositionMoved = uidam.MoveWindow(bIsDocked, DUp, 20, DragFullWindows);

	if(!bFullTest) return;

	// Try to go beyond screen borders
	nPositionMoved = uidam.ResizeWindow(bIsDocked, SLeft, DLeft, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
		EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
		uidam.Activate();
	}
	uidam.ResizeWindow(bIsDocked, SLeft, DRight, nPositionMoved, DragFullWindows);

	nPositionMoved = uidam.ResizeWindow(bIsDocked, SRight, DRight, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
		EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
		uidam.Activate();
	}
	uidam.ResizeWindow(bIsDocked, SRight, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uidam.ResizeWindow(bIsDocked, SBottom, DDown, -1, DragFullWindows);
	if(!DragFullWindows)
	{
		EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
		EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
		uidam.Activate();
	}
	uidam.ResizeWindow(bIsDocked, SBottom, DUp, nPositionMoved, DragFullWindows);

	nPositionMoved = uidam.ResizeWindow(bIsDocked, STop, DUp, -1, DragFullWindows);
	// If we move it to far, the title bar is not available, we need to reactivate memory window
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	uidam.Activate();
	uidam.ResizeWindow(bIsDocked, STop, DDown, 10, DragFullWindows);

	// I like to move it move it
	nPositionMoved = uidam.MoveWindow(bIsDocked, DRight, -1, DragFullWindows);
	// If we move it to far, the title bar is not available, we need to reactivate memory window
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	uidam.Activate();
	uidam.MoveWindow(bIsDocked, DLeft, nPositionMoved, DragFullWindows);

	nPositionMoved = uidam.MoveWindow(bIsDocked, DLeft, -1, DragFullWindows);
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	uidam.Activate();
	uidam.MoveWindow(bIsDocked, DRight, nPositionMoved, DragFullWindows);

	nPositionMoved = uidam.MoveWindow(bIsDocked, DDown, -1, DragFullWindows);
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	uidam.Activate();
	uidam.MoveWindow(bIsDocked, DUp, nPositionMoved, DragFullWindows);

	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	uidam.Activate();

	nPositionMoved = uidam.MoveWindow(bIsDocked, DUp, -1, DragFullWindows);
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	EXPECT_TRUE(uidam.ChooseContextMenuItem(DOCKING_VIEW));
	uidam.Activate();
	uidam.MoveWindow(bIsDocked, DDown, nPositionMoved, DragFullWindows);

}

void CDisAsmIDETest::PopulateDamWindowText(void)

{
	// This is a part of DAM window.  Copy it here if it changes.  I used the following steps
	// Use Ctrl+Sfift+F8 to select/remove rectangle 
	// I used the following steps
	// 1. Turn Off Code Byte in debugee's DAM, seclect lines you want to check and copy them
	// 2. Open a new text window and paste copied lines there.
	// 3. Select the very left rectangular part which contains addresses and delete it
	// 4. Select the part which left in the text window using Ctrl+Sfift+F8 (this is important). And copy this part.
	// 5. Switch to this file and select lines belonging to the csDamWindowTextAtTemplateFooLine array between
	//	   " and ",
	// 6. Press Ctrl+V.  
	// 7. Remove (addressh)
	// It is assumed in the SourceAnnotation that each line in csDamWindowTextAtTemplateFooLine[]
	// wchich begins with space or \t is a Source code line !
		
	csDamWindowTextAtTemplateFooLine.Add("																				");
	csDamWindowTextAtTemplateFooLine.Add("        TemplateFoo <char>(chJustChar);     /* First line for tests */			");
			// TODO (dklem 02/01/99)  ERROR.  Looks like a bug in 60.
			//'mov         al,byte ptr [chJustChar]' looks as 'mov         al,byte ptr [ebp - whatever]'
			// The next 1 line should replace the 2 line
	/*"mov         al,byte ptr [chJustChar]											");
	"push        eax																");
	"call																			");
	"add         esp,4																");
	"        TemplateFoo <int>(nJustInt);											");
	"mov         ecx,dword ptr [nJustInt]											");
	"push        ecx																");
	"call																			");
	"add         esp,4																");
	"																				");
	"        goto Ku_Ku;															");
	"jmp         Ku_Ku+																");
	"                nJustInt = 7;													");
	"Ku_Ku:																			");
	"                nJustInt = 17;													");
	"mov         dword ptr [nJustInt],												");
	"																				");
	"        FuncWithArg(1);														");
	"push        1																	");
	"call																			");
	"add         esp,4																");
	"        Func(dJustDouble);														");
	"mov         edx,dword ptr [ebp-												");
	"push        edx																");
	"mov         eax,dword ptr [dJustDouble]										");
	"push        eax																");
	"call			 																");
	"add         esp,8																");
	"																				");
	"        OverloadedFunc();														");
	"call						 													");
	"        OverloadedFunc(nJustInt);												");
	"mov         ecx,dword ptr [nJustInt]											");
	"push        ecx																");
	"call						 													");
	"add         esp,4																");
	"																				");
	" hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadRoutine, (void*) pJustInt, 0L,&dwThreadId);");
	"mov         esi,esp															");
	"lea         edx,[dwThreadId]													");
	"push        edx																");
	"push        0																	");
	"mov         eax,dword ptr [pJustInt]											");
	"push        eax																");
	"push        offset @ILT+									");
	"push        0																	");
	"push        0																	");
	// If you remove the following line, add a line containing a mangeled name (@)
	"call        dword ptr [__imp__CreateThread@24									");	
	"cmp         esi,esp															");
	"call        																");
	"mov         dword ptr [hThread],eax											");
	"     ResumeThread(hThread);													");
	*/
	csDamWindowTextAtTemplateFooLine.Add("mov         al,byte ptr [														");
	csDamWindowTextAtTemplateFooLine.Add("push        eax																");
	csDamWindowTextAtTemplateFooLine.Add("call																			");
	csDamWindowTextAtTemplateFooLine.Add("add         esp,4																");
	csDamWindowTextAtTemplateFooLine.Add("        TemplateFoo <int>(nJustInt);											");

	// code-gen is different for these two instructions for processor pack.
	if(1) //xbox - CMDLINE->GetBooleanValue("PP", FALSE))
	{
		csDamWindowTextAtTemplateFooLine.Add("mov         eax,dword ptr [													");
		csDamWindowTextAtTemplateFooLine.Add("push        eax																");
	}
	else
	{
		csDamWindowTextAtTemplateFooLine.Add("mov         ecx,dword ptr [													");
		csDamWindowTextAtTemplateFooLine.Add("push        ecx																");
	}

	csDamWindowTextAtTemplateFooLine.Add("call																			");
	csDamWindowTextAtTemplateFooLine.Add("add         esp,4																");
	csDamWindowTextAtTemplateFooLine.Add("																				");
	csDamWindowTextAtTemplateFooLine.Add("        goto Ku_Ku;															");
	csDamWindowTextAtTemplateFooLine.Add("jmp         Ku_Ku+																");
	csDamWindowTextAtTemplateFooLine.Add("                nJustInt = 7;													");
	csDamWindowTextAtTemplateFooLine.Add("Ku_Ku:																			");
	csDamWindowTextAtTemplateFooLine.Add("                nJustInt = 17;													");
	csDamWindowTextAtTemplateFooLine.Add("mov         dword ptr [														");
	csDamWindowTextAtTemplateFooLine.Add("																				");
	csDamWindowTextAtTemplateFooLine.Add("        FuncWithArg(1);														");
	csDamWindowTextAtTemplateFooLine.Add("push        1																	");
	csDamWindowTextAtTemplateFooLine.Add("call																			");
	csDamWindowTextAtTemplateFooLine.Add("add         esp,4																");
	csDamWindowTextAtTemplateFooLine.Add("        Func(dJustDouble);														");
	csDamWindowTextAtTemplateFooLine.Add("mov         edx,dword ptr [													");
	csDamWindowTextAtTemplateFooLine.Add("push        edx																");
	csDamWindowTextAtTemplateFooLine.Add("mov         eax,dword ptr [													");
	csDamWindowTextAtTemplateFooLine.Add("push        eax																");
	csDamWindowTextAtTemplateFooLine.Add("call			 																");
	csDamWindowTextAtTemplateFooLine.Add("add         esp,8																");
	csDamWindowTextAtTemplateFooLine.Add("																				");
	csDamWindowTextAtTemplateFooLine.Add("        OverloadedFunc();														");
	csDamWindowTextAtTemplateFooLine.Add("call						 													");
	csDamWindowTextAtTemplateFooLine.Add("        OverloadedFunc(nJustInt);												");
	csDamWindowTextAtTemplateFooLine.Add("mov         ecx,dword ptr [													");
	csDamWindowTextAtTemplateFooLine.Add("push        ecx																");
	csDamWindowTextAtTemplateFooLine.Add("call						 													");
	csDamWindowTextAtTemplateFooLine.Add("add         esp,4																");
	csDamWindowTextAtTemplateFooLine.Add("																				");
	csDamWindowTextAtTemplateFooLine.Add(" hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadRoutine, (void*) pJustInt, 0L,&dwThreadId);");
	csDamWindowTextAtTemplateFooLine.Add("mov         esi,esp															");
	csDamWindowTextAtTemplateFooLine.Add("lea         edx,[																");
	csDamWindowTextAtTemplateFooLine.Add("push        edx																");
	csDamWindowTextAtTemplateFooLine.Add("push        0																	");
	csDamWindowTextAtTemplateFooLine.Add("mov         eax,dword ptr [													");
	csDamWindowTextAtTemplateFooLine.Add("push        eax																");
	csDamWindowTextAtTemplateFooLine.Add("push        offset @ILT+									");
	csDamWindowTextAtTemplateFooLine.Add("push        0																	");
	csDamWindowTextAtTemplateFooLine.Add("push        0																	");
	// If you remove the following line, add a line containing a mangeled name (@)
	csDamWindowTextAtTemplateFooLine.Add("call        dword ptr [__imp__CreateThread@24									");	
}


////////////////////// OLD TESTS ///////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////////////////////////////////////////////////////////////////////////////////

BOOL CDisAsmIDETest::GoAfterScrollDisassemblyWnd(  )
{
	COSource		src;	// WinslowF - added to call GoToLine. It's not in UIWB any more.
	BOOL retval = TRUE;

	CODebug codebug;
	EXPECT( codebug.Restart() );

	// Open DAM
	UIDAM dam = UIDebug::ShowDockWindow( IDW_DISASSY_WIN, TRUE );
	EXPECT( dam.IsActive() );

	int i;
	for ( i = 0; i < 200; i++)
	{
		MST.DoKeys ("+{pgdn}",FALSE,2000); //wait for idling
		if (MST.WFndWndWait("Browse for Folder", FW_NOCASE | FW_PART,0)
			|| MST.WFndWndWait("Find Source", FW_NOCASE | FW_PART,0))
			MST.WButtonClick( "Cancel" );
	}
		
	if ( !UIWB.VerifySubstringAtLine( "???" ) )
	{
		EXPECT ( src.GoToLine("0xf00000") );
	}

	if ( !UIWB.VerifySubstringAtLine( "???" ) )
	{
		EXPECT ( src.GoToLine("0xffffff") );
	}
		
	if ( !UIWB.VerifySubstringAtLine( "???" ) )
	{ 
   		m_pLog->RecordInfo("0x00ffffff : The app is too big: test case should be revised" );
		retval = FALSE;
	}

// YS: ToDo: we should use Go instead of StopDebugging

	//	EXPECT(	codebug.Go(NULL,NULL,NULL,0) );
	//	CString TitleRun = (CString)"[" + GetLocString(IDSS_DBG_RUN) + "]";
	//	MST.WFndWndWait(TitleRun, FW_PART, 10);
	//	
	//	UIWB.WaitForTermination();

	codebug.StopDebugging();		// TODO: WinslowF - Above three lines are replaced by this due to focus problems in Chicago.
// YS: end of block for ToDo

	WriteLog(PASSED, "01 : Scrolling Disassembly Window didn't corrupt Debugging as expected" );

	return retval;
}


	//~~~~~~~~~~~~~~~~~~~~~~~~~
/*
*	GO_TO_SOURCE,	
*	SHOW_NEX_STATEMENT,
*	INSERT_REMOVE_BREAK_POINT,
*	TOGGLE_BREAKPOINT,
*	RUN_TO_CURSOR,				
*	SET_NEXT_STATEMENT,
*	SOURCE_ANNOTATION,
*	CODE_BYTES,
	CODE_INJECTION,
*	DOCKING_VIEW,
*	CLOSE_HIDE
*/




