///////////////////////////////////////////////////////////////////////////////
//	BPCASE3.CPP
//											 
//	Created by:			Date:			
//		MichMa				10/10/97	
//
//	Description:								 
//		Implementation of CBp3Cases		 

#include "stdafx.h"
#include "bpcase3.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
			 	 
											   
IMPLEMENT_TEST(CBp3Cases, CDbgTestBase, "Breakpoints Pri3", -1, CbpSubSuite)


// each function that runs a test has this prototype.
typedef void (CBp3Cases::*PTR_TEST_FUNC)(void); 

// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.
PTR_TEST_FUNC tests[] = {
	&CBp3Cases::BreakOnGlobalConstructorAndRestart,
	&CBp3Cases::BreakOnFunctionContainingScopeOperator,
	&CBp3Cases::BreakOnOverloadedSymbolSetBeforeStartDebugging,
	&CBp3Cases::BreakOnOverloadedSymbolSetAfterStartDebugging,
	&CBp3Cases::StepOverFunctionThatHitsBp,
	&CBp3Cases::BreakWhenAggregateExpressionChanges,
	&CBp3Cases::BreakOnMultipleBreakpoints,
	//xbox &CBp3Cases::BreakOnDllFuncThatWasUnloadedAfterSettingBp,
	&CBp3Cases::CompareGlobalAndLocalDataBpPerformance,
	&CBp3Cases::StepOutOfFunctionThatHitsBp,
	&CBp3Cases::StepToCursorThatHitsBp,
	//xbox &CBp3Cases::HitBpInDllSetBeforeStartDebugging,
	//xbox &CBp3Cases::HitBpInDllSetAfterStartDebugging,
	&CBp3Cases::HitLocationBpInExeSetWhileDebuggeeRunning,
	//xbox &CBp3Cases::HitLocationBpInDllSetWhileDebuggeeRunning,
	&CBp3Cases::HitDataBpInExeSetWhileDebuggeeRunning,
	&CBp3Cases::EditBpCodeWhenSourceIsInForeground,
	&CBp3Cases::EditBpCodeWhenSourceIsOpenButNotInForeground,
	&CBp3Cases::EditBpCodeWhenSourceIsNotOpen,
	// TODO(michma): re-enable when bug vs98:36090 is fixed.
	//&CBp3Cases::EditBpCodeWhenInputIsRequiredToFindSource,
	&CBp3Cases::HitBpWithPassCount,
	&CBp3Cases::HitBpAfterInterruptingPassCountAndRestarting,
	&CBp3Cases::HitBpSetInStack,
	&CBp3Cases::RemoveBpSetInStack,
	&CBp3Cases::HitBpWhenInputRequiredToFindSource
};

void CBp3Cases::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


void CBp3Cases::Run()

{
	if (!CreateXboxProjectFromSource("breakpoints pri2", "src\\breakpoints pri2",
				"breakpoints pri2 exe.cpp"))
			return;

	// we only want to randomly run each test once.
	RandomNumberCheckList rncl(sizeof(tests) / sizeof(PTR_TEST_FUNC));
	// indexes into the test function array.
	int iTest;
	
	/*
	// randomly run each test once.
	while((iTest = rncl.GetNext()) != -1)
		(this->*(tests[iTest]))();
	*/
	for (iTest = 0; iTest < (sizeof(tests) / sizeof(PTR_TEST_FUNC)); iTest++)
		(this->*(tests[iTest]))();
}


void CBp3Cases::InitTestState()
{
	EXPECT_TRUE(dbg.Restart());
}


void CBp3Cases::BreakOnGlobalConstructorAndRestart(void)
{	
	LogTestHeader("BreakOnGlobalConstructorAndRestart");
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_VALIDBP(bps.SetBreakpoint("CBase::CBase"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of CBase::CBase()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("CBase::CBase"));
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of CBase::CBase()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("CBase::CBase"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
}


// note that the <class>::<function> case is covered by BreakOnGlobalConstructorAndRestart(), so we only
// have to test global functions with scope operators here.
void CBp3Cases::BreakOnFunctionContainingScopeOperator(void)
{  
	LogTestHeader("BreakOnFunctionContainingScopeOperator");
	EXPECT_VALIDBP(bps.SetBreakpoint("::Func"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState(); 
}


void CBp3Cases::BreakOnOverloadedSymbolSetBeforeStartDebugging(void)
{
	LogTestHeader("BreakOnOverloadedSymbolSetBeforeStartDebugging");
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_VALIDBP(bps.SetBreakpoint("OverloadedFunc"));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	EXPECT_TRUE(dbg.ResolveAmbiguity(1));
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of OverloadedFunc(void)"));
	EXPECT_TRUE(stk.CurrentFunctionIs("OverloadedFunc()"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState(); 
}


void CBp3Cases::BreakOnOverloadedSymbolSetAfterStartDebugging(void)
{
	LogTestHeader("BreakOnOverloadedSymbolSetAfterStartDebugging");
	EXPECT_VALIDBP(bps.SetAmbiguousBreakpoint("OverloadedFunc", 2));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of OverloadedFunc(int)"));
	EXPECT_TRUE(stk.CurrentFunctionIs("OverloadedFunc(int 1)"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState(); 
}


void CBp3Cases::StepOverFunctionThatHitsBp(void)
{
	LogTestHeader("StepOverFunctionThatHitsBp");
	EXPECT_TRUE(src.Find("Func();"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(src.Find("first line of Func()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.StepOver());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	//xbox EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION));
	InitTestState(); 
}


void CBp3Cases::BreakWhenAggregateExpressionChanges(void)
{
	LogTestHeader("BreakWhenAggregateExpressionChanges");
	EXPECT_VALIDBP(bps.SetBreakOnExpr("gCBaseObj", COBP_TYPE_IF_EXP_CHANGED));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("gCBaseObj change"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState(); 
}


void CBp3Cases::BreakOnMultipleBreakpoints(void)
{
	LogTestHeader("BreakOnMultipleBreakpoints");
	EXPECT_TRUE(src.Find("multiple breakpoints hit"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(src.Find("init iLocal in Func()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_VALIDBP(bps.SetBreakOnExpr("iGlobal + iLocal == 2", COBP_TYPE_IF_EXP_TRUE));
	EXPECT_VALIDBP(bps.SetBreakOnExpr("iGlobal", COBP_TYPE_IF_EXP_CHANGED));
	EXPECT_VALIDBP(bps.SetBreakpoint("MultipleBreakpointsHitLabel"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(cxx.ExpressionValueIs("iGlobal + iLocal", 2));
	EXPECT_TRUE(dbg.CurrentLineIs("multiple breakpoints hit"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("multiple breakpoints hit"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// TODO(michma): we need to do this extra step because of bug 38172.
	EXPECT_TRUE(dbg.StepInto());
	InitTestState(); 
}


void CBp3Cases::BreakOnDllFuncThatWasUnloadedAfterSettingBp(void)
{
	LogTestHeader("BreakOnDllFuncThatWasUnloadedAfterSettingBp");
	EXPECT_TRUE(src.Find("FreeLibrary(hDll)"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_VALIDBP(bps.SetBreakpoint("DllFunc"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of DllFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("DllFunc"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState(); 
}


void CBp3Cases::CompareGlobalAndLocalDataBpPerformance(void)
{
	LogTestHeader("CompareGlobalAndLocalDataBpPerformance");
	EXPECT_TRUE(src.Find("for(i = 0; i < iLoopNum; i++)"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_VALIDBP(bps.SetBreakOnExpr("iLocal", COBP_TYPE_IF_EXP_CHANGED));
	time_t timeStart = time(NULL);
	EXPECT_TRUE(dbg.Go());
	int timeLocalBp = time(NULL) - timeStart;
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_TRUE(dbg.Restart()); 
	EXPECT_TRUE(src.Find("for(i = 0; i < iLoopNum; i++)"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_VALIDBP(bps.SetBreakOnExpr("iGlobal", COBP_TYPE_IF_EXP_CHANGED));
	timeStart = time(NULL);
	EXPECT_TRUE(dbg.Go());
	int timeGlobalBp = time(NULL) - timeStart;
	EXPECT_TRUE(timeLocalBp > timeGlobalBp);
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// TODO(michma): we need to do this extra step because of bug 38172.
	EXPECT_TRUE(dbg.StepInto());
	InitTestState(); 
}


void CBp3Cases::StepOutOfFunctionThatHitsBp(void)
{
	LogTestHeader("StepOutOfFunctionThatHitsBp");
	EXPECT_TRUE(src.Find("first line of Func()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(src.Find("line within Func()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.StepOut());
	EXPECT_TRUE(dbg.CurrentLineIs("line within Func()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	//xbox EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION));
	InitTestState(); 
}


void CBp3Cases::StepToCursorThatHitsBp(void)
{
	LogTestHeader("StepToCursorThatHitsBp");
	EXPECT_TRUE(src.Find("first line of Func()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(src.Find("line within Func()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("Func"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	//xbox EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION));
	InitTestState(); 
}


/*xbox void CBp3Cases::HitBpInDllSetBeforeStartDebugging(void)
{
	LogTestHeader("HitBpInDllSetBeforeStartDebugging");
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_SUCCESS(prj.SetAdditionalDLLs("Debug\\" + m_strProjBase + " dll.dll"));
	EXPECT_VALIDBP(bps.SetBreakpoint("DllFunc"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of DllFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("DllFunc"));
	EXPECT_TRUE(dbg.Restart()); 	
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of DllFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("DllFunc"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_SUCCESS(prj.RemoveAdditionalDLLs(1));
	EXPECT_TRUE(dbg.StopDebugging());
	InitTestState(); 	
}*/


/*xbox void CBp3Cases::HitBpInDllSetAfterStartDebugging(void)
{
	LogTestHeader("HitBpInDllSetAfterStartDebugging");
	EXPECT_SUCCESS(prj.SetAdditionalDLLs("Debug\\" + m_strProjBase + " dll.dll"));
	EXPECT_VALIDBP(bps.SetBreakpoint("DllFunc"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of DllFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("DllFunc"));
	EXPECT_TRUE(dbg.Restart()); 	
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of DllFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("DllFunc"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_SUCCESS(prj.RemoveAdditionalDLLs(1));
	EXPECT_TRUE(dbg.StopDebugging());
	InitTestState(); 	
}*/


void CBp3Cases::HitLocationBpInExeSetWhileDebuggeeRunning(void)
{
	LogTestHeader("HitLocationBpInExeSetWhileDebuggeeRunning");
	EXPECT_TRUE(cxx.SetExpressionValue("giDelay", 60));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	EXPECT_TRUE(src.Find("line within main()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.CurrentLineIs("line within main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState(); 	
}


/*xbox void CBp3Cases::HitLocationBpInDllSetWhileDebuggeeRunning(void)
{
	LogTestHeader("HitLocationBpInDllSetWhileDebuggeeRunning");
	EXPECT_TRUE(cxx.SetExpressionValue("giDelay", 60));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	EXPECT_SUCCESS(prj.SetAdditionalDLLs("Debug\\" + m_strProjBase + " dll.dll"));
	EXPECT_VALIDBP(bps.SetBreakpoint("DllFunc"));
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of DllFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("DllFunc"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_SUCCESS(prj.RemoveAdditionalDLLs(1));
	EXPECT_TRUE(dbg.StopDebugging());
	InitTestState(); 	
}*/


void CBp3Cases::HitDataBpInExeSetWhileDebuggeeRunning(void)
{
	LogTestHeader("HitDataBpInExeSetWhileDebuggeeRunning");
	EXPECT_TRUE(cxx.SetExpressionValue("giDelay", 60));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	EXPECT_VALIDBP(bps.SetBreakOnExpr("iGlobal == 100", COBP_TYPE_IF_EXP_TRUE));
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK));
	EXPECT_TRUE(cxx.ExpressionValueIs("iGlobal", 100));
	EXPECT_TRUE(dbg.CurrentLineIs("iGlobal = 100"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	// TODO(michma): we need to do this extra step because of bug 38172.
	EXPECT_TRUE(dbg.StepInto());
	InitTestState(); 	
}


void CBp3Cases::EditBpCodeWhenSourceIsInForeground(void)
{
	LogTestHeader("EditBpCodeWhenSourceIsInForeground");
	bp *pBp = bps.SetBreakpoint("Func");
	EXPECT_TRUE(bps.EditCode(pBp));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


void CBp3Cases::EditBpCodeWhenSourceIsOpenButNotInForeground(void)
{
	LogTestHeader("EditBpCodeWhenSourceIsOpenButNotInForeground");
	bp *pBp = bps.SetBreakpoint("Func");
	EXPECT_SUCCESS(src.Create());
	EXPECT_TRUE(bps.EditCode(pBp));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_SUCCESS(src.Close());
	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


void CBp3Cases::EditBpCodeWhenSourceIsNotOpen(void)
{
	LogTestHeader("EditBpCodeWhenSourceIsNotOpen");
	bp *pBp = bps.SetBreakpoint("Func");
	EXPECT_SUCCESS(src.AttachActive());
	EXPECT_SUCCESS(src.Close());
	EXPECT_TRUE(bps.EditCode(pBp));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
}


#undef TMP_CASE_DIR
#define TMP_CASE_DIR TMP_PROJ_DIR "\\EditBpCodeWhenInputIsRequiredToFindSource"

void CBp3Cases::EditBpCodeWhenInputIsRequiredToFindSource(void)
{
	LogTestHeader("EditBpCodeWhenInputIsRequiredToFindSource");
	bp *pBp = bps.SetBreakpoint("Func");
	EXPECT_SUCCESS(src.AttachActive());
	EXPECT_SUCCESS(src.Close());
	// need to create a unique directory for moving source in this test, because if source
	// has already been 'found' at the same location by previous test of
	// HitBpWhenInputRequiredToFindSource, no 'Find Source' dialog will be displayed!
	CreateDirectory(TMP_CASE_DIR,  NULL);
	KillFiles(TMP_CASE_DIR, "*.*");
	EXPECT_TRUE(MoveFile(m_strProjectDir + "\\" + m_strProjectName + ".cpp", (TMP_CASE_DIR "\\") + m_strProjectName + ".cpp"));
	EXPECT_TRUE(bps.EditCode(pBp, TMP_CASE_DIR));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_SUCCESS(src.AttachActive());
	EXPECT_SUCCESS(src.Close());
	EXPECT_TRUE(MoveFile((TMP_CASE_DIR "\\") + m_strProjectName + ".cpp", m_strProjectDir + "\\" + m_strProjectName + ".cpp"));
	RemoveDirectory(TMP_CASE_DIR);
	EXPECT_TRUE(bps.EditCode(pBp));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of Func()"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_SUCCESS(prj.Close());

	EXPECT_SUCCESS(prj.Open(m_strProjectDir + "\\" + m_strProjectName));
	InitTestState();
}


void CBp3Cases::HitBpWithPassCount(void)
{
	LogTestHeader("HitBpWithPassCount");
	EXPECT_TRUE(src.Find("line within loop"));
	EXPECT_SUCCESS(src.AttachActive());
	int iLine = src.GetCurrentLine();
	bp *pBp = bps.SetBreakpoint(iLine, NULL, 100);
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("line within loop"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(cxx.ExpressionValueIs("i", 100));
	EXPECT_TRUE(bps.RemainingPassCountIs(pBp, 0));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CBp3Cases::HitBpAfterInterruptingPassCountAndRestarting(void)

{
	LogTestHeader("HitBpAfterInterruptingPassCountAndRestarting");
	EXPECT_TRUE(src.Find("dll loaded"));
	EXPECT_TRUE(dbg.StepToCursor());
	bp *pBp1 = bps.SetBreakpoint("DllFunc", 50);
	EXPECT_TRUE(src.Find("line within loop"));
	bp *pBp2 = bps.SetBreakpoint();
	
	for(int i = 0; i < 5; i++)
		EXPECT_TRUE(dbg.Go());

	EXPECT_TRUE(dbg.CurrentLineIs("line within loop"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(cxx.ExpressionValueIs("i", 4));
	EXPECT_TRUE(bps.RemainingPassCountIs(pBp1, 46));
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(bps.RemainingPassCountIs(pBp1, 50));
	EXPECT_TRUE(bps.RemoveBreakpoint(pBp2));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of DllFunc"));
	EXPECT_TRUE(stk.CurrentFunctionIs("DllFunc"));
	EXPECT_TRUE(bps.RemainingPassCountIs(pBp1, 0));
	EXPECT_TRUE(dbg.StepOut());
	EXPECT_TRUE(cxx.ExpressionValueIs("i", 50));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CBp3Cases::HitBpSetInStack(void)
{
	LogTestHeader("HitBpSetInStack");
	EXPECT_VALIDBP(bps.SetBreakpoint("Func"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(stk.ToggleBreakpoint(1));
	EXPECT_TRUE(dbg.Go());
	// emmang@xbox.com - move mouse focus to edit window
	dbg.ShowNextStatement();
	EXPECT_TRUE(dbg.CurrentLineIs("line after call to Func()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CBp3Cases::RemoveBpSetInStack(void)
{
	LogTestHeader("RemoveBpSetInStack");
	EXPECT_VALIDBP(bps.SetBreakpoint("Func"));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(stk.ToggleBreakpoint(1));
	EXPECT_TRUE(stk.ToggleBreakpoint(1));
	//xbox EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


#undef TMP_CASE_DIR
#define TMP_CASE_DIR TMP_PROJ_DIR "\\HitBpWhenInputRequiredToFindSource"

void CBp3Cases::HitBpWhenInputRequiredToFindSource(void)
{
	LogTestHeader("HitBpWhenInputRequiredToFindSource");
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_TRUE(dbg.StepInto());
	EXPECT_TRUE(dbg.ShowNextStatement());
	EXPECT_TRUE(src.Find("line within main()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_SUCCESS(src.AttachActive());
	EXPECT_SUCCESS(src.Close());
	// need to create a unique directory for moving source in this test, because if source
	// has already been 'found' at the same location by previous test of
	// EditBpCodeWhenInputIsRequiredToFindSource, no 'Find Source' dialog will be displayed!
	CreateDirectory(TMP_CASE_DIR,  NULL);
	KillFiles(TMP_CASE_DIR, "*.*");
	EXPECT_TRUE(MoveFile(m_strProjectDir + "\\" + m_strProjectName + ".cpp", (TMP_CASE_DIR "\\") + m_strProjectName + ".cpp"));
	//WaitStepInstructions("Source file moved, gonna wait for Find Source dialog");
	//Sleep(5000);
	UIWB.m_pTarget->SetHandleMsgs(FALSE); //handle Find Source manually
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	EXPECT_TRUE(dbg.Wait(WAIT_FOR_BREAK, TRUE));
	EXPECT_TRUE(dbg.SupplySourceLocation(TMP_CASE_DIR));
	UIWB.m_pTarget->SetHandleMsgs(TRUE); //restore automatic handling of Find Source
	EXPECT_TRUE(dbg.CurrentLineIs("line within main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_SUCCESS(src.AttachActive());
	EXPECT_SUCCESS(src.Close());
	EXPECT_TRUE(MoveFile((TMP_CASE_DIR "\\") + m_strProjectName + ".cpp", m_strProjectDir + "\\" + m_strProjectName + ".cpp"));
	RemoveDirectory(TMP_CASE_DIR);
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("line within main()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("main"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	EXPECT_TRUE(dbg.StopDebugging());
	EXPECT_SUCCESS(prj.Close());

	EXPECT_SUCCESS(prj.Open(m_strProjectDir + "\\" + m_strProjectName));
	InitTestState();
}