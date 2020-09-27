///////////////////////////////////////////////////////////////////////////////
//	ThreadsCases.cpp
//											 
//	Created by:			Date:			
//		MichMa				2/5/98	
//
//	Description:								 
//		Implementation of CThreadsCases		 

#include "stdafx.h"
#include "ThreadsCases.h"
#include "..\cleanup.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
											   
IMPLEMENT_TEST(CThreadsCases, CDebugTestSet, "Threads", -1, CThreadsSubsuite)


// each function that runs a test has this prototype.
typedef void (CThreadsCases::*PTR_TEST_FUNC)(void); 

// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.
PTR_TEST_FUNC tests[] = 
{
	&CThreadsCases::VerifyThreadInfoForMultipleThreads,
	&CThreadsCases::ChangeFocusToDifferentThreadAndBack,
	&CThreadsCases::RunMultithreadedAppToTermination,
	&CThreadsCases::HitLocationBPInThreadSetBeforeThreadCreated,
	&CThreadsCases::HitLocationBPInThreadSetAfterThreadCreated,
	&CThreadsCases::HitGlobalDataBPInThreadSetWhileFocusedOnAnotherThread,
	&CThreadsCases::HitGlobalDataBPInThreadSetWhileFocusedOnSameThread,
	&CThreadsCases::VerifyMemoryDumpOfLocalFromSecondaryThread,
	&CThreadsCases::NavigateStackInSecondaryThread,
	&CThreadsCases::StopDebuggingFromSecondaryThread,
	&CThreadsCases::SuspendAndResumeThread,
	&CThreadsCases::SetFocusToThreadAndStep,
	&CThreadsCases::GoWhenThreadWithFocusIsSuspended
};


void CThreadsCases::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


void CThreadsCases::Run(void)

{
	// default thread priority differs between nt and win9x.
	if((GetSystem() & SYSTEM_WIN_95) || (GetSystem() & SYSTEM_WIN_98) || (GetSystem() & SYSTEM_WIN_MILL))
		m_iDefaultThreadPri = 8;
	else
		m_iDefaultThreadPri = 9;
	
	// setup some paths we will use.
	CString strProjBase;

	if(GetSystem() & SYSTEM_DBCS)
		strProjBase = "ƒ[ƒ\\ƒ]ƒ^ƒ{ƒ|ƒ}ƒAƒa‚‚`Ÿ@Ÿ~Ÿ€Ÿü threads app";
	else
		strProjBase = "threads app";

	// the location of the unlocalized sources.
	CString strSrcDir = GetCWD() + "src\\threads";
	// the location of the localized project/sources.
	CString strProjDir = strSrcDir + "\\" + strProjBase;

	// clean-up from the last run.
	OurKillFile(strProjDir + "\\Debug\\", "*.*");
	RemoveDirectory(strProjDir + "\\Debug");
	OurKillFile(strProjDir + "\\", "*.*");
	RemoveDirectory(strProjDir);

	// set the options for the project we are building for this test.
	CProjWizOptions *pWin32AppWizOpt = new(CWin32AppWizOptions);
	pWin32AppWizOpt->m_strLocation = strSrcDir;
	pWin32AppWizOpt->m_strName = strProjBase;	
	
	// create the project.
	EXPECT_SUCCESS(proj.New(pWin32AppWizOpt));
	// create a new localized exe source file in the project dir from the unlocalized exe source file.
	CopyFile(strSrcDir + "\\threads.cpp", strProjDir + "\\" + strProjBase + ".cpp", FALSE);
	// make it writable so it can be cleaned up later.
	SetFileAttributes(strProjDir + "\\" + strProjBase + ".cpp", FILE_ATTRIBUTE_NORMAL);
	// add source to the project.
	EXPECT_SUCCESS(proj.AddFiles(strProjBase + ".cpp"));
	// build the project.
	EXPECT_SUCCESS(proj.Build());

	// all tests expect this initial state.
	InitTestState();
	// we only want to randomly run each test once.
	RandomNumberCheckList rncl(sizeof(tests) / sizeof(PTR_TEST_FUNC));
	// indexes into the test function array.
	int iTest;
	
	// randomly run each test once.
	while((iTest = rncl.GetNext()) != -1)
		(this->*(tests[iTest]))();
}


void CThreadsCases::VerifyThreadInfoForMultipleThreads(void)
{
	LogTestHeader("VerifyThreadInfoForMultipleThreads");
	// breaking in WinMain's loop should ensure that we break on source code in both threads and on all os's.
	EXPECT_TRUE(src.Find("loop inside WinMain()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside WinMain()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
	// verify primary thread info.
	CThreadInfo ThreadInfo;
	ThreadInfo.bHasFocus = TRUE;
	ThreadInfo.strID = GetThreadIDStr("dwMainThreadID");
	ThreadInfo.intSuspend = 0;
	ThreadInfo.intPriority = m_iDefaultThreadPri;
	ThreadInfo.strLocation = "WinMain";
	EXPECT_TRUE(threads.VerifyThreadInfo(ThreadInfo));
	// verify secondary thread info.
	ThreadInfo.bHasFocus = FALSE;
	ThreadInfo.strID = GetThreadIDStr("dwSecondaryThreadID");
	ThreadInfo.intSuspend = 0;
	ThreadInfo.intPriority = m_iDefaultThreadPri;
	ThreadInfo.strLocation = "SecondaryThreadFunc";
	EXPECT_TRUE(threads.VerifyThreadInfo(ThreadInfo));
	InitTestState();
}


void CThreadsCases::ChangeFocusToDifferentThreadAndBack(void)
{
	LogTestHeader("ChangeFocusToDifferentThreadAndBack");
	// TODO(michma - 2/8/98): add watch and asm verification.
	// breaking in WinMain's loop should ensure that we break on source code in both threads and on all os's.
	EXPECT_TRUE(src.Find("loop inside WinMain()"));
	EXPECT_TRUE(dbg.StepToCursor());
	CString strThreadID = GetThreadIDStr("dwSecondaryThreadID");
	EXPECT_TRUE(threads.SetThreadFocus(strThreadID));
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_TRUE(locals.VerifyLocalInfo("pdata", "0x00000000", "void *", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("intLocalSecondaryThreadFunc", "1", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.LocalsCountIs(2));
	strThreadID = GetThreadIDStr("dwMainThreadID");
	EXPECT_TRUE(threads.SetThreadFocus(strThreadID));
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside WinMain()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
	EXPECT_TRUE(locals.VerifyLocalInfo("hInst", "ADDRESS", "HINSTANCE__ *", COLLAPSED));
	EXPECT_TRUE(locals.VerifyLocalInfo("hInstPrev", "0x00000000", "HINSTANCE__ *", COLLAPSED));
	EXPECT_TRUE(locals.VerifyLocalInfo("lszCmdLine", "ADDRESS \"", "char *", COLLAPSED));
	EXPECT_TRUE(locals.VerifyLocalInfo("nCmdShow", "1", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("intLocalWinMain", "0", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.LocalsCountIs(5));
	InitTestState();
}


void CThreadsCases::RunMultithreadedAppToTermination(void)
{
	LogTestHeader("RunMultithreadedAppToTermination");
	// step passed the code that will create the secondary thread.
	EXPECT_TRUE(src.Find("line after call to CreateThread()"));
	EXPECT_TRUE(dbg.StepToCursor());
	// setting bInfiniteLoop to 0 will let the app terminate.
	EXPECT_TRUE(cxx.SetExpressionValue("bInfiniteLoop", 0));
	CString strSecondaryThreadID = GetThreadIDStr("dwSecondaryThreadID");
	CString strMainThreadID = GetThreadIDStr("dwMainThreadID");
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION));
	EXPECT_TRUE(threads.VerifyThreadExitCode(strSecondaryThreadID, 1));
	EXPECT_TRUE(threads.VerifyThreadExitCode(strMainThreadID, 0));
	InitTestState();
}


void CThreadsCases::HitLocationBPInThreadSetBeforeThreadCreated(void)
{
	LogTestHeader("HitLocationBPInThreadSetBeforeThreadCreated");
	EXPECT_TRUE(src.Find("loop inside SecondaryThreadFunc()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CThreadsCases::HitLocationBPInThreadSetAfterThreadCreated(void)
{
	LogTestHeader("HitLocationBPInThreadSetAfterThreadCreated");
	EXPECT_TRUE(src.Find("CreateThreadFunc();"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.StepOver());
	// verify that the thread has been created before setting the breakpoint.
	CThreadInfo ThreadInfo;
	ThreadInfo.bHasFocus = FALSE;
	ThreadInfo.strID = GetThreadIDStr("dwSecondaryThreadID");
	ThreadInfo.intSuspend = 0;
	ThreadInfo.intPriority = m_iDefaultThreadPri;
	ThreadInfo.strLocation = "SecondaryThreadFunc";
	EXPECT_TRUE(threads.VerifyThreadInfo(ThreadInfo));
	// now set the bp in the thread's func and verify that it is hit.
	EXPECT_TRUE(src.Find("loop inside SecondaryThreadFunc()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CThreadsCases::HitGlobalDataBPInThreadSetWhileFocusedOnAnotherThread(void)
{
	LogTestHeader("HitGlobalDataBPInThreadSetWhileFocusedOnAnotherThread");
	// remember that initially focus is on main thread (WinMain).
	EXPECT_VALIDBP(bps.SetBreakOnExpr("intGlobal", COBP_TYPE_IF_EXP_CHANGED));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("line after intGlobal changed."));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_TRUE(cxx.ExpressionValueIs("intGlobal", 1));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CThreadsCases::HitGlobalDataBPInThreadSetWhileFocusedOnSameThread(void)
{
	LogTestHeader("HitGlobalDataBPInThreadSetWhileFocusedOnSameThread");
	EXPECT_TRUE(src.Find("first line of SecondaryThreadFunc()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of SecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_VALIDBP(bps.SetBreakOnExpr("intGlobal", COBP_TYPE_IF_EXP_CHANGED));
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("line after intGlobal changed."));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_TRUE(cxx.ExpressionValueIs("intGlobal", 1));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CThreadsCases::VerifyMemoryDumpOfLocalFromSecondaryThread(void)
{
	LogTestHeader("VerifyMemoryDumpOfLocalFromSecondaryThread");
	EXPECT_TRUE(src.Find("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_TRUE(mem.MemoryDataIs("intLocalSecondaryThreadFunc", 1));
	InitTestState();
}


void CThreadsCases::NavigateStackInSecondaryThread(void)
{
	LogTestHeader("NavigateStackInSecondaryThread");
	EXPECT_TRUE(src.Find("first line of FuncCalledBySecondaryThreadFunc()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(stk.NavigateStack(1));
	EXPECT_TRUE(dbg.CurrentLineIs("line after call to FuncCalledBySecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_TRUE(locals.VerifyLocalInfo("pdata", "0x00000000", "void *", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("intLocalSecondaryThreadFunc", "1", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.LocalsCountIs(2));
	EXPECT_TRUE(stk.NavigateStack(0));
	EXPECT_TRUE(dbg.CurrentLineIs("first line of FuncCalledBySecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("FuncCalledBySecondaryThreadFunc"));
	EXPECT_TRUE(locals.LocalsCountIs(0));
	InitTestState();
}


void CThreadsCases::StopDebuggingFromSecondaryThread(void)
{
	LogTestHeader("StopDebuggingFromSecondaryThread");
	EXPECT_TRUE(src.Find("first line of SecondaryThreadFunc()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.CurrentLineIs("first line of SecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	// view registers and disassembly while in secondary thread (a different case in the testplan).
	EXPECT_TRUE(regs.Enable());
	EXPECT_TRUE(dam.Enable());
	EXPECT_TRUE(dbg.SetSteppingMode(SRC));
	EXPECT_TRUE(dbg.StopDebugging());
	InitTestState();
}


void CThreadsCases::SuspendAndResumeThread(void)
{
	LogTestHeader("SuspendAndResumeThread");
	// step over the function that creates the thread.
	EXPECT_TRUE(src.Find("CreateThreadFunc();"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.StepOver());
	// set the bp in the thread's func.
	EXPECT_TRUE(src.Find("loop inside SecondaryThreadFunc()"));
	EXPECT_VALIDBP(bps.SetBreakpoint());
	// suspend the thread and verify its new status.
	CString strThreadID = GetThreadIDStr("dwSecondaryThreadID");
	EXPECT_TRUE(threads.SuspendThread(strThreadID)); 
	CThreadInfo ThreadInfo;
	ThreadInfo.bHasFocus = FALSE;
	ThreadInfo.strID = strThreadID;
	ThreadInfo.intSuspend = 1;
	ThreadInfo.intPriority = m_iDefaultThreadPri;
	ThreadInfo.strLocation = "SecondaryThreadFunc";
	EXPECT_TRUE(threads.VerifyThreadInfo(ThreadInfo));
	// go and verify that the bp isn't hit (if app is still running after 5 seconds).
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, NOWAIT));
	Sleep(5000);
	EXPECT_TRUE(dbg.GetDebugState() == Debug_Run);
	// resume the thread and verify its new status.
	EXPECT_TRUE(dbg.Break());
	EXPECT_TRUE(threads.ResumeThread(strThreadID));
	ThreadInfo.intSuspend = 0;
	EXPECT_TRUE(threads.VerifyThreadInfo(ThreadInfo));
	// go and verify that the bp is hit.
	EXPECT_TRUE(dbg.Go());
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	EXPECT_TRUE(bps.ClearAllBreakpoints());
	InitTestState();
}


void CThreadsCases::SetFocusToThreadAndStep(void)

{
	LogTestHeader("SetFocusToThreadAndStep");
	// break in the secondary thread.
	EXPECT_TRUE(src.Find("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(dbg.StepToCursor());
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("SecondaryThreadFunc"));
	// set focus to the main thread.
	CString strThreadID = GetThreadIDStr("dwMainThreadID");
	EXPECT_TRUE(threads.SetThreadFocus(strThreadID));

	// step in the main thread (different behavior between nt and win9x).
	if((GetSystem() & SYSTEM_WIN_95) || (GetSystem() & SYSTEM_WIN_98))
	{
		EXPECT_TRUE(dbg.StepOver());
		EXPECT_TRUE(dbg.CurrentLineIs("last line of CreateThreadFunc()"));
		EXPECT_TRUE(stk.CurrentFunctionIs("CreateThreadFunc"));
	}
	// on nt we end up in ntdll.dll when switching threads.
	else
	{
		EXPECT_TRUE(stk.RunToFrame("WinMain"));
		EXPECT_TRUE(dbg.SetSteppingMode(SRC));
		EXPECT_TRUE(dbg.CurrentLineIs("loop inside WinMain()"));
		EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
	}

	InitTestState();
}


void CThreadsCases::GoWhenThreadWithFocusIsSuspended(void)
{
	LogTestHeader("GoWhenThreadWithFocusIsSuspended");
	EXPECT_TRUE(src.Find("loop inside SecondaryThreadFunc()"));
	EXPECT_TRUE(dbg.StepToCursor());
	CString strThreadID = GetThreadIDStr("dwSecondaryThreadID");
	EXPECT_TRUE(threads.SuspendThread(strThreadID));
	EXPECT_TRUE(dbg.Go(NULL, NULL, NULL, WAIT_FOR_RUN));
	// CreateThreadFunc sleeps for 3 seconds before returning. this is to ensure that the thread gets created before 
	// that function returns. so if we want to be sure we break into WinMain below, we need to sleep for 3 seconds
	// to ensure that CreateThreadFunc has returned.
	Sleep(3000);
	EXPECT_TRUE(dbg.Break());
	EXPECT_TRUE(dbg.CurrentLineIs("loop inside WinMain()"));
	EXPECT_TRUE(stk.CurrentFunctionIs("WinMain"));
	InitTestState();
}


void CThreadsCases::InitTestState(void)
{
	EXPECT_TRUE(dbg.Restart());
	// attach the COSource object to the source file that the debugger opened.
	src.AttachActive();
} 


LPCSTR CThreadsCases::GetThreadIDStr(LPCSTR szThreadIDVar)

{
	LOG->RecordInfo("GetThreadIDStr - szThreadIDVar = '%s'", szThreadIDVar);
	static char szText[12] = "";
	DWORD dwThreadID;
	
	if(!cxx.GetExpressionValue(szThreadIDVar, &dwThreadID))
	{
		LOG->RecordInfo("CThreadsCases::GetThreadIDStr - GetExpressionValue failed.");
		return (LPCSTR)szText;
	}

	sprintf(szText, "%.8x", dwThreadID);
	return (LPCSTR)szText;
}
