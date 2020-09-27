///////////////////////////////////////////////////////////////////////////////
//	BreakpointsCases.cpp
//											 
//	Created by:			Date:			
//		MichMa				3/12/98	
//
//	Description:								 
//		Implementation of CECBreakpointsCases		 

#include "stdafx.h"
#include "BreakpointsCases.h"
#include "..\cleanup.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
							   
IMPLEMENT_TEST(CECBreakpointsCases, CECTestSet, "Edit & Continue: Breakpoints", -1, CECBreakpointsSubsuite)


void CECBreakpointsCases::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


// tests for hitting location breakpoints set on new code.
CArray<CLocationTest, CLocationTest> LocationTests;
// tests for hitting data breakpoints set on new locals.
CArray<CDataTest, CDataTest> DataTests;
// tracks how many tests have been performed.
int giTestsPerformed = 0;


void CECBreakpointsCases::Run()

{
	// setup some paths we will use.
	if(GetSystem() & SYSTEM_DBCS)
		m_strProjBase = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü ec breakpoints app";
	else
		m_strProjBase = "ec breakpoints app";

	m_strSrcFullPath = GetCWD() + "src\\ec breakpoints app";
	m_strProjDirFullPath = m_strSrcFullPath + "\\" + m_strProjBase;

	// clean-up from the last run.
	OurKillFile(m_strProjDirFullPath + "\\Debug\\", "*.*");
	RemoveDirectory(m_strProjDirFullPath + "\\Debug");
	OurKillFile(m_strProjDirFullPath + "\\", "*.*");

	// set the options for the project we are building for this test.
	CProjWizOptions *pWin32AppWizOpt = new(CWin32AppWizOptions);
	pWin32AppWizOpt->m_strLocation = m_strSrcFullPath;
	pWin32AppWizOpt->m_strName = m_strProjBase;	
	
	// create the project.
	EXPECT_SUCCESS(proj.New(pWin32AppWizOpt));

	// create a new exe source file (to be edited) from the base exe source file.
	CopyFile(m_strSrcFullPath + "\\" + "base exe.cpp", m_strProjDirFullPath + "\\" + m_strProjBase + " exe.cpp", FALSE);
	// this appears to be a win32 timing problem. when running the test over the net, the server seems to keep the file 
	// locked even after CopyFile returns. an immediate call to SetFileAttributes will fail with a sharing violation.
	// so we wait a second here to make sure the server is totally finished processing the CopyFile request.
	Sleep(1000);
	// make the source file writable so we can edit it.
	SetFileAttributes(m_strProjDirFullPath + "\\" + m_strProjBase + " exe.cpp", FILE_ATTRIBUTE_NORMAL);

	// add source to the project.
	EXPECT_SUCCESS(proj.AddFiles(m_strProjBase + " exe.cpp"));
	// build the project.
	EXPECT_SUCCESS(proj.Build());
	// get the project into the intial debugging state.
	EXPECT_TRUE(dbg.StepInto());
	// attach the COSource object to the source file that the debugger opened.
	EXPECT_SUCCESS(src.AttachActive());
	
	// initialize info for the tests to perform.
	InitTests();
	// we only want to randomly do each test once.
	RandomNumberCheckList rnclTests(LocationTests.GetSize() + DataTests.GetSize());
	// the index of the next randomly selected test.
	int iTest;
	// flags whether or not a selected test was done.
	BOOL bTestDone;

	// the ranges of the different types of breakpoints tests within the random number checklist.
	int iLocationTestsStart = 0;
	int iLocationTestsEnd = LocationTests.GetSize() - 1;
	int iDataTestsStart = LocationTests.GetSize();
	int iDataTestsEnd = iDataTestsStart + DataTests.GetSize() - 1;
 
	// randomly do each test once.
	while((iTest = rnclTests.GetNext()) != -1)
	
	{
		// check if the test index selected is within the range of a CLocationTest test.
		if((iTest >= iLocationTestsStart) && (iTest <= iLocationTestsEnd) && (LocationTests.GetSize() > 0))
			bTestDone = DoTest(LocationTests[iTest - iLocationTestsStart]);
		// check if the test index selected is within the range of a CDataTest test.
		else if((iTest >= iDataTestsStart) && (iTest <= iDataTestsEnd) && (DataTests.GetSize() > 0))
			bTestDone = DoTest(DataTests[iTest - iDataTestsStart]);

		// if we were able to do the test, reset the debugger to the inital state.
		if(bTestDone)
			{EXPECT_TRUE(GetBackToStartOfWinMain());}
		// if we were unable to do the test at this time, make its index available again.
		else
			rnclTests.UndoLast();
	}

	// clean up.
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_SUCCESS(proj.Close());
}


void CECBreakpointsCases::InitTests(void)

{	
	//******************************* LOCATION TESTS ********************************

	CLocationTest LocationTest;
	LocationTest.m_bTestDone = FALSE;

	// break on new code.
	LocationTest.m_ecEditType = EC_NO_STACK_FRAME;
	LocationTests.Add(LocationTest);
	
	LocationTest.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	LocationTests.Add(LocationTest); 
	
	LocationTest.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	LocationTests.Add(LocationTest); 

	LocationTest.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	LocationTests.Add(LocationTest); 
	
	LocationTest.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	LocationTests.Add(LocationTest); 

	LocationTest.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	LocationTests.Add(LocationTest); 

	//******************************* DATA TESTS ********************************

	CDataTest DataTest;
	DataTest.m_bTestDone = FALSE;
	DataTest.m_bConditional = FALSE;

	// data breakpoint on new local.
	DataTest.m_ecEditType = EC_NO_STACK_FRAME;
	DataTests.Add(DataTest);
	
	DataTest.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	DataTests.Add(DataTest); 
	
	DataTest.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	DataTests.Add(DataTest); 

	DataTest.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	DataTests.Add(DataTest); 

	DataTest.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	DataTests.Add(DataTest); 

	DataTest.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	DataTests.Add(DataTest); 

	// conditional breakpoint on new local.
	DataTest.m_bConditional = TRUE;

	DataTest.m_ecEditType = EC_NO_STACK_FRAME;
	DataTests.Add(DataTest);
	
	DataTest.m_ecEditType = EC_CURRENT_FRAME_AFTER_IP;
	DataTests.Add(DataTest); 
	
	DataTest.m_ecEditType = EC_CURRENT_FRAME_BEFORE_IP;
	DataTests.Add(DataTest); 

	DataTest.m_ecEditType = EC_CURRENT_FRAME_AT_IP;
	DataTests.Add(DataTest); 

	DataTest.m_ecEditType = EC_PARENT_FRAME_AFTER_IP;
	DataTests.Add(DataTest); 

	DataTest.m_ecEditType = EC_PARENT_FRAME_BEFORE_IP;
	DataTests.Add(DataTest); 

	DataTest.m_bConditional = FALSE;
}


//********************************** LOCATION TESTS *********************************

BOOL CECBreakpointsCases::DoTest(CLocationTest &LocationTest)

{
	// this test will be performed.
	giTestsPerformed++;

	// log the test header.
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST %d: Breaking on new code using edit type \"%s\".", 
					   giTestsPerformed, szecEditTypes[LocationTest.m_ecEditType]);
	
	// each type of edit requires different steps.
	switch(LocationTest.m_ecEditType)
	
	{
		// edited function does not have an active stack frame.
		case EC_NO_STACK_FRAME: 
		{
			// insert the code after the first line of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc1()"));
			AddCodeToSource(LocationTest.m_ecEditType);
			// set a bp on the new code.
			EXPECT_VALIDBP(bps.SetBreakpoint());
			break;
		}

		// edited function is active on top of the stack. edit occurs after the ip.
		case EC_CURRENT_FRAME_AFTER_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the first line of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc1()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the code after the first line of the function.
			AddCodeToSource(LocationTest.m_ecEditType);
			// set a bp on the new code.
			EXPECT_VALIDBP(bps.SetBreakpoint());
			break;
		}

		// edited function is active on top of the stack. edit occurs before the ip.
		case EC_CURRENT_FRAME_BEFORE_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the last line of the function to edit.
			EXPECT_TRUE(src.Find("last line of ExeFunc1()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the new code after the first line of the function.
			EXPECT_TRUE(src.Find("first line of ExeFunc1()"));
			AddCodeToSource(LocationTest.m_ecEditType);
			// set a bp on the new code.
			EXPECT_VALIDBP(bps.SetBreakpoint());
			// step out of the edited function.
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger landed on the correct line and function.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to ExeFunc1()"));
			EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
			// reset the ip to the call into the edited function.
			EXPECT_TRUE(src.Find("ExeFunc1();"));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}

		// edited function is active on top of the stack. edit occurs at the ip.
		case EC_CURRENT_FRAME_AT_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the last line of the function to edit.
			EXPECT_TRUE(src.Find("last line of ExeFunc1()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the code before the last line of the function.
			AddCodeToSource(LocationTest.m_ecEditType);
			// set a bp on the new code.
			EXPECT_VALIDBP(bps.SetBreakpoint());
			// step out of the edited function.
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger landed on the correct line and function.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to ExeFunc1()"));
			EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
			// reset the ip to the call into the edited function.
			EXPECT_TRUE(src.Find("ExeFunc1();"));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs after the ip.
		case EC_PARENT_FRAME_AFTER_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc2()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the code before the last line of the function to edit.
			EXPECT_TRUE(src.Find("last line of ExeFunc1()"));
			AddCodeToSource(LocationTest.m_ecEditType);
			// set a bp on the new code.
			EXPECT_VALIDBP(bps.SetBreakpoint());
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs before the ip.
		case EC_PARENT_FRAME_BEFORE_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc2()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the code after the first line of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc1()"));
			AddCodeToSource(LocationTest.m_ecEditType);
			// set a bp on the new code.
			EXPECT_VALIDBP(bps.SetBreakpoint());
			// step out of the child function.
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger landed in the correct place.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to ExeFunc2()"));
			EXPECT_TRUE(stk.CurrentFunctionIs("ExeFunc1"));
			// step out of the edited function so we can re-enter it to hit the bp.
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger landed in the correct place.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to ExeFunc1()"));
			EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
			// reset the ip to the call into the edited function.
			EXPECT_TRUE(src.Find("ExeFunc1();"));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}
	}

	// hit the bp on the new code.
	EXPECT_TRUE(dbg.Go());
	// verify the debugger landed in the right place.
	EXPECT_TRUE(dbg.CurrentLineIs((CString)"while(0); // " + szecEditTypes[LocationTest.m_ecEditType]));
	EXPECT_TRUE(stk.CurrentFunctionIs("ExeFunc1"));
	// clear the breakpoint.
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// test was able to execute.
	LocationTest.m_bTestDone = TRUE;
	return TRUE;
}


void CECBreakpointsCases::AddCodeToSource(EC_EDIT_TYPE ecEditType)

{
	// set the cursor up according to the type of edit being performed.
	switch(ecEditType)
	{
		case EC_NO_STACK_FRAME:
		case EC_CURRENT_FRAME_AFTER_IP:
		case EC_CURRENT_FRAME_BEFORE_IP:
		case EC_PARENT_FRAME_BEFORE_IP:
		{
			src.TypeTextAtCursor("{END}{ENTER}");
			break;
		}
		case EC_CURRENT_FRAME_AT_IP:
		{
			src.TypeTextAtCursor("{TAB}");
			break;
		}
		case EC_PARENT_FRAME_AFTER_IP:
		{
			src.TypeTextAtCursor("{HOME}{ENTER}{UP}{TAB}");
			break;
		}
	}

	// insert the code.
	src.TypeTextAtCursor("while(0);", TRUE);
	// append a comment stating the edit type for later use.
	src.TypeTextAtCursor((CString)" // " + szecEditTypes[ecEditType]);

	// editing at the current ip requires an Enter here to separate it from the next line,
	// and also an Up to put the cursor on the line added.
	if(ecEditType == EC_CURRENT_FRAME_AT_IP)
		src.TypeTextAtCursor("{ENTER}{UP}");
}


//********************************** DATA TESTS *********************************

BOOL CECBreakpointsCases::DoTest(CDataTest &DataTest)

{
	// this test will be performed.
	giTestsPerformed++;

	// log the test header.
	m_pLog->RecordInfo("");
	m_pLog->RecordInfo("TEST %d: Hitting %s breakpoint on new local using edit type \"%s\".", 
					   giTestsPerformed, DataTest.m_bConditional ? "conditional" : "data", 
					   szecEditTypes[DataTest.m_ecEditType]);
	
	// each type of edit requires different steps.
	switch(DataTest.m_ecEditType)
	
	{
		// edited function does not have an active stack frame.
		case EC_NO_STACK_FRAME: 
		{
			// insert the local after the first line of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc1()"));
			AddLocalToSource(DataTest);
			// do the e&c and set the breakpoint.
			ApplyCodeChanges();
			EXPECT_TRUE(SetDataBreakpoint(DataTest));
			break;
		}

		// edited function is active on top of the stack. edit occurs after the ip.
		case EC_CURRENT_FRAME_AFTER_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the first line of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc1()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the local after the first line of the function.
			AddLocalToSource(DataTest);
			// do the e&c and set the breakpoint.
			ApplyCodeChanges();
			EXPECT_TRUE(SetDataBreakpoint(DataTest));
			break;
		}

		// edited function is active on top of the stack. edit occurs before the ip.
		case EC_CURRENT_FRAME_BEFORE_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the last line of the function to edit.
			EXPECT_TRUE(src.Find("last line of ExeFunc1()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the new code after the first line of the function.
			EXPECT_TRUE(src.Find("first line of ExeFunc1()"));
			AddLocalToSource(DataTest);
			// do the e&c and set the breakpoint.
			ApplyCodeChanges();
			EXPECT_TRUE(SetDataBreakpoint(DataTest));
			// step out of the edited function.
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger landed on the correct line and function.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to ExeFunc1()"));
			EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
			// reset the ip to the call into the edited function.
			EXPECT_TRUE(src.Find("ExeFunc1();"));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}

		// edited function is active on top of the stack. edit occurs at the ip.
		case EC_CURRENT_FRAME_AT_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the last line of the function to edit.
			EXPECT_TRUE(src.Find("last line of ExeFunc1()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the code before the last line of the function.
			AddLocalToSource(DataTest);
			// do the e&c and set the breakpoint.
			ApplyCodeChanges();
			EXPECT_TRUE(SetDataBreakpoint(DataTest));
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs after the ip.
		case EC_PARENT_FRAME_AFTER_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc2()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the code before the last line of the function to edit.
			EXPECT_TRUE(src.Find("last line of ExeFunc1()"));
			AddLocalToSource(DataTest);
			// do the e&c and set the breakpoint.
			ApplyCodeChanges();
			// TODO(michma - 3/21/98): due to product bug 31357 we need to step out to edited function before setting bp.
			EXPECT_TRUE(dbg.StepOut());
			EXPECT_TRUE(SetDataBreakpoint(DataTest));
			break;
		}

		// edited function is active, but not on top of the stack. edit occurs before the ip.
		case EC_PARENT_FRAME_BEFORE_IP:
		{
			// TODO(michma): randomly select either step to cursor or bp.
			// run to the first line of the child of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc2()"));
			EXPECT_TRUE(dbg.StepToCursor());
			// insert the code after the first line of the function to edit.
			EXPECT_TRUE(src.Find("first line of ExeFunc1()"));
			AddLocalToSource(DataTest);
			// do the e&c and set the breakpoint.
			ApplyCodeChanges();
			EXPECT_TRUE(SetDataBreakpoint(DataTest));
			// step out of the child function.
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger landed in the correct place.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to ExeFunc2()"));
			EXPECT_TRUE(stk.CurrentFunctionIs("ExeFunc1"));
			// step out of the edited function so we can re-enter it to hit the bp.
			EXPECT_TRUE(dbg.StepOut());
			// verify the debugger landed in the correct place.
			EXPECT_TRUE(dbg.CurrentLineIs("line after call to ExeFunc1()"));
			EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
			// reset the ip to the call into the edited function.
			EXPECT_TRUE(src.Find("ExeFunc1();"));
			EXPECT_TRUE(dbg.SetNextStatement());
			break;
		}
	}

	// hit the bp on the new code.
	EXPECT_TRUE(dbg.Go());

	// create the name of the local added depending on the breakpoint type.
	CString strLocal;

	if(DataTest.m_bConditional)
		strLocal = (CString)"localConditionalBp" + szecEditTypes[DataTest.m_ecEditType];
	else
		strLocal = (CString)"localDataBp" + szecEditTypes[DataTest.m_ecEditType];

	// verify the debugger landed in the right place.
	EXPECT_TRUE(dbg.CurrentLineIs((CString)"while(0); // line after " + strLocal + " set to 1."));
	EXPECT_TRUE(stk.CurrentFunctionIs("ExeFunc1"));
	// verify that the new local has the correct value.
	EXPECT_TRUE(cxx.ExpressionValueIs(strLocal, 1));
	// clear the breakpoint.
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// test was able to execute.
	DataTest.m_bTestDone = TRUE;
	return TRUE;
}


void CECBreakpointsCases::AddLocalToSource(CDataTest &DataTest)

{
	// set the cursor up according to the type of edit being performed.
	switch(DataTest.m_ecEditType)
	{
		case EC_NO_STACK_FRAME:
		case EC_CURRENT_FRAME_AFTER_IP:
		case EC_CURRENT_FRAME_BEFORE_IP:
		case EC_PARENT_FRAME_BEFORE_IP:
		{
			src.TypeTextAtCursor("{END}{ENTER}");
			break;
		}
		case EC_CURRENT_FRAME_AT_IP:
		{
			src.TypeTextAtCursor("{TAB}");
			break;
		}
		case EC_PARENT_FRAME_AFTER_IP:
		{
			src.TypeTextAtCursor("{HOME}{ENTER}{UP}{TAB}");
			break;
		}
	}

	CString strLocal;

	// create the name of the local to add depending on the breakpoint type.
	if(DataTest.m_bConditional)
		strLocal = (CString)"localConditionalBp" + szecEditTypes[DataTest.m_ecEditType];
	else
		strLocal = (CString)"localDataBp" + szecEditTypes[DataTest.m_ecEditType];
	
	// insert the local.
	src.TypeTextAtCursor((CString)"int " + strLocal + " = 0;{ENTER}");

	// editing at the current ip requires a Tab here to align it with the previous line.
	if(DataTest.m_ecEditType == EC_CURRENT_FRAME_AT_IP)
		src.TypeTextAtCursor("{TAB}");
	
	// insert an assignment for the local.
	src.TypeTextAtCursor(strLocal + " = 1;{ENTER}");

	// editing at the current ip requires a Tab here to align it with the previous line.
	if(DataTest.m_ecEditType == EC_CURRENT_FRAME_AT_IP)
		src.TypeTextAtCursor("{TAB}");
	
	// insert a line after the assignment so we can verify that we broke in the right place.
	src.TypeTextAtCursor((CString)"while(0); // line after " + strLocal + " set to 1.", TRUE); 

	// editing at the current ip requires an Enter here to separate it from the next line.
	if(DataTest.m_ecEditType == EC_CURRENT_FRAME_AT_IP)
		src.TypeTextAtCursor("{ENTER}");
}


BOOL CECBreakpointsCases::SetDataBreakpoint(CDataTest &DataTest)

{
	if(DataTest.m_bConditional)
	
	{
		// put the cursor on the line where the breakpoint will be hit.
		CString strLocal = (CString)"localConditionalBp" + szecEditTypes[DataTest.m_ecEditType];
		EXPECT_TRUE(src.Find((CString)"while(0); // line after " + strLocal + " set to 1."));
		int iLine = src.GetCurrentLine();

		return bps.SetBreakpoint(
			iLine,
			(CString)"{ExeFunc1,,} " + strLocal + " == 1",
			COBP_TYPE_IF_EXP_TRUE, 0) != NULL;
	}	
	
	else
	{
		return bps.SetBreakOnExpr(
			(CString)"{ExeFunc1,,} localDataBp" + szecEditTypes[DataTest.m_ecEditType]+ " == 1", 
			COBP_TYPE_IF_EXP_TRUE) != NULL;
	}
}


//******************************* MISC FUNCTIONS *******************************

BOOL CECBreakpointsCases::GetBackToStartOfWinMain(void)

{
	/*
	if(!stk.CurrentFunctionIs("WinMain"))
		EXPECT_TRUE(stk.RunToFrame("WinMain"));
	
	EXPECT_TRUE(src.Find("first line of WinMain()"));
	EXPECT_TRUE(dbg.SetNextStatement());

	if(dbg.AtSymbol("WinMain"))
		return TRUE;
	else
		return FALSE;
	*/
	/*
	dbg.StopDebugging();
	proj.WaitUntilBuildDone(1);
	proj.RebuildAll();
	dbg.StepInto();
	*/
	return dbg.Restart();
}


// TODO(michma): this needs its own CODebug function.
void CECBreakpointsCases::ApplyCodeChanges(void)
{
	MST.DoKeys("%dy");
	proj.WaitUntilBuildDone();
}

