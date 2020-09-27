///////////////////////////////////////////////////////////////////////////////
//	LOCCASE.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script.	 

#include "stdafx.h"
#include "loccase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
										   
IMPLEMENT_TEST(/*999,*/ CLocalsCases, CDbgTestBase, "Locals", -1, CVarsWndSubSuite)


// each function that runs a test has this prototype.
typedef void (CLocalsCases::*PTR_TEST_FUNC)(void); 

// Information about a test to be put in the test header
#define LOGTESTHEADER(strTestName) 	m_strLogInfo.Format("# - %d.  %s. Total # - %d, Passed - %d %%", ++m_TestNumber, strTestName, m_TotalNumberOfTests, (m_TestNumber*100)/m_TotalNumberOfTests);  \
									LogTestHeader(m_strLogInfo);


// we randomly pick out of the following list of tests and run them.
// insert functions for new tests to the end of the list.

PTR_TEST_FUNC tests[] = {
	&CLocalsCases::NavigateStackToParentFunctionAndBack,
	&CLocalsCases::VerifyThatLocalsUpdatesViaRegisterWindow,				
	&CLocalsCases::VerifyLocPaneWhenVariablesChangeViaExec,
//TODO dklem (07/29/980 make this test available when bug #5190 is fixed
	&CLocalsCases::VerifyLocPaneWhenVariablesChangeViaMemoryWindow,
	&CLocalsCases::VerifyLocPaneWhenVariablesChangeViaWatchWindow,
	&CLocalsCases::VerifyLocPaneWhenVariablesChangeViaLocalsWindow,
	&CLocalsCases::VerifyLocPaneWhenVariablesChangeViaQuickWatchWindow,
	&CLocalsCases::VerifyThatFunctionArgumentsAppearWhenPrologReached,	
	&CLocalsCases::VerifyThatLocalsAppearAfterExecutingProlog,			
	&CLocalsCases::CheckLocalsWhenSwitchToThisAutoAndBack,
	&CLocalsCases::StepThroughFunctionWithNoArguments,					
	&CLocalsCases::StepThroughPrologWithThisPaneActive,					

};

void CLocalsCases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CLocalsCases::Run(void)
	
	{
	/******************
	 * INITIALIZATION *
	 ******************/

	if (!CreateXboxProjectFromSource("Locals Application", "src\\locpane",
			"Locals Application.cpp"))
		return;

	// all tests expect the ide to be in this initial state.
	EXPECT_TRUE(dbg.StepOver());

	m_TestNumber = 0;
	m_TotalNumberOfTests = sizeof(tests) / sizeof(PTR_TEST_FUNC);
	
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

	dbg.StopDebugging(ASSUME_NORMAL_TERMINATION) ;
} 

	/*********
	 * TESTS *
	 *********/

void CLocalsCases::VerifyLocPaneWhenVariablesChangeViaExec(void)
{
	LOGTESTHEADER("VerifyLocPaneWhenVariablesChangeViaExec");
	
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());
	LogTestHeader("Check that initial values are correct");
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustInt", "1965", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("chJustChar", "97 'a'", "char", NOT_EXPANDABLE));

	// Now do 2 steps over and begin test 
	EXPECT_TRUE(dbg.StepOver(2));
	// Check that initial values change via execution
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustInt", "1966", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("chJustChar", "98 'b'", "char", NOT_EXPANDABLE));
}


void CLocalsCases::VerifyLocPaneWhenVariablesChangeViaMemoryWindow(void)
{
	LOGTESTHEADER("VerifyLocPaneWhenVariablesChangeViaMemoryWindow");
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	// Open Memory window
	EXPECT_TRUE(mem.Enable());
	EXPECT_TRUE(mem.SetMemoryData("chJustChar", 'e', MEM_FORMAT_ASCII));
	EXPECT_TRUE(locals.VerifyLocalInfo("chJustChar", "101 'e'", "char", NOT_EXPANDABLE));
	EXPECT_TRUE(mem.Disable());
}

void CLocalsCases::VerifyLocPaneWhenVariablesChangeViaWatchWindow(void)
{
	LOGTESTHEADER("VerifyLocPaneWhenVariablesChangeViaWatchWindow");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	//Change Watch Window
	EXPECT_TRUE(watch.AddWatch("nJustInt"));
	EXPECT_TRUE(watch.SetWatchValue("nJustInt", "1993"));
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustInt", "1993", "int", NOT_EXPANDABLE));
}

void CLocalsCases::VerifyLocPaneWhenVariablesChangeViaLocalsWindow(void)
{
	LOGTESTHEADER("VerifyLocPaneWhenVariablesChangeViaLocalsWindow");
	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	//Change Locals Window
	EXPECT_TRUE(locals.SetLocalValue("nJustInt", "1974"));
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustIntRef", "1974", "int &", NOT_EXPANDABLE));
}

void CLocalsCases::VerifyLocPaneWhenVariablesChangeViaQuickWatchWindow(void)
{

	LOGTESTHEADER("VerifyLocPaneWhenVariablesChangeViaQuickWatchWindow");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* First line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	//Change Quick Watch Window
	EXPECT_TRUE(uiqw.Activate());
	EXPECT_TRUE(uiqw.SetExpression("dJustDouble"));
	EXPECT_TRUE(uiqw.Recalc());
	EXPECT_TRUE(uiqw.SetNewValue("3.141592"));
	EXPECT_TRUE(uiqw.Recalc());
	EXPECT_TRUE(NULL ==uiqw.Close());
	EXPECT_TRUE(locals.VerifyLocalInfo("dJustDouble", "3.1415920000000", "double", NOT_EXPANDABLE));
	

}

void CLocalsCases::VerifyThatFunctionArgumentsAppearWhenPrologReached(void)
{
	LOGTESTHEADER("VerifyThatFunctionArgumentsAppearWhenPrologReached");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* Second line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	//Step into function
	EXPECT_TRUE(dbg.StepInto(1));
	EXPECT_TRUE(locals.LocalsCountIs(1));
	EXPECT_TRUE(locals.VerifyLocalInfo("chArg", "98 'b'", "char", NOT_EXPANDABLE));
}

void CLocalsCases::VerifyThatLocalsAppearAfterExecutingProlog(void)
{

	LOGTESTHEADER("VerifyThatLocalsAppearAfterExecutingProlog");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* Second line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	//Step into function and execute prolog
	EXPECT_TRUE(dbg.StepInto(2));
	EXPECT_TRUE(locals.LocalsCountIs(3));
	EXPECT_TRUE(locals.VerifyLocalInfo("chArg", "98 'b'", "char", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustLocalInt", "UNKNOWN", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("dJustLocalDouble", "UNKNOWN", "double", NOT_EXPANDABLE));
}


void CLocalsCases::NavigateStackToParentFunctionAndBack(void)
{

	LOGTESTHEADER("NavigateStackToParentFunctionAndBack");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* Second line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	//Step into function and execute prolog
	EXPECT_TRUE(dbg.StepInto(2));
	EXPECT_TRUE(stk.NavigateStack("WinMain"));
	EXPECT_TRUE(locals.LocalsCountIs(8));
	EXPECT_TRUE(locals.VerifyLocalInfo("hInstance", "UNKNOWN", "HINSTANCE__ *", COLLAPSED));
	EXPECT_TRUE(locals.VerifyLocalInfo("hPrevInstance", "UNKNOWN", "HINSTANCE__ *", COLLAPSED));
	EXPECT_TRUE(locals.VerifyLocalInfo("lpCmdLine", "UNKNOWN", "char *", COLLAPSED));
	EXPECT_TRUE(locals.VerifyLocalInfo("nCmdShow", "1", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("dJustDouble", "2.9200000000000", "double", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("chJustChar", "98 'b'", "char", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustIntRef", "1966", "int &", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustInt", "1966", "int", NOT_EXPANDABLE));

	EXPECT_TRUE(stk.NavigateStack("FuncWithArg"));
	EXPECT_TRUE(locals.LocalsCountIs(3));
	EXPECT_TRUE(locals.VerifyLocalInfo("chArg", "98 'b'", "char", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustLocalInt", "UNKNOWN", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("dJustLocalDouble", "UNKNOWN", "double", NOT_EXPANDABLE));

}

void CLocalsCases::CheckLocalsWhenSwitchToThisAutoAndBack(void)
{
	
	LOGTESTHEADER("CheckLocalsWhenSwitchToThisAutoAndBack");	

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* Second line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	//Step into function and execute prolog
	EXPECT_TRUE(dbg.StepInto(2));
	uivar.Activate();
	EXPECT_TRUE(ERROR_SUCCESS == uivar.SetPane(PANE_THIS)); 
	Sleep(500);
	EXPECT_TRUE(ERROR_SUCCESS == uivar.SetPane(PANE_LOCALS));
	EXPECT_TRUE(locals.LocalsCountIs(3));
	EXPECT_TRUE(locals.VerifyLocalInfo("chArg", "98 'b'", "char", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustLocalInt", "UNKNOWN", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("dJustLocalDouble", "UNKNOWN", "double", NOT_EXPANDABLE));

//TODO dklem (07/29/980 make this test available when Atuto pan is available
	uivar.Activate();
	EXPECT_TRUE(ERROR_SUCCESS == uivar.SetPane(PANE_AUTO));
	Sleep(500);
	EXPECT_TRUE(ERROR_SUCCESS == uivar.SetPane(PANE_LOCALS));
	EXPECT_TRUE(locals.LocalsCountIs(3));
	EXPECT_TRUE(!locals.LocalDoesNotExist("chArg"));
	EXPECT_TRUE(!locals.LocalDoesNotExist("nJustLocalInt"));
	EXPECT_TRUE(!locals.LocalDoesNotExist("dJustLocalDouble"));


}

void CLocalsCases::VerifyThatLocalsUpdatesViaRegisterWindow(void)
{

	LOGTESTHEADER("VerifyThatLocalsUpdatesViaRegisterWindow");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* Last FuncWithArg line */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	//Step into function and return
	EXPECT_TRUE(dbg.StepInto(1));
	EXPECT_TRUE(regs.Enable());
	EXPECT_TRUE(regs.SetRegister(EAX, "00000079"));
	EXPECT_TRUE(dbg.StepInto(1));
	EXPECT_TRUE(locals.VerifyLocalInfo("chJustChar", "121 'y'", "char", NOT_EXPANDABLE));
}


void CLocalsCases::StepThroughFunctionWithNoArguments(void)
{
	LOGTESTHEADER("StepThroughFunctionWithNoArguments");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* Third line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	EXPECT_TRUE(dbg.StepInto(1));
	EXPECT_TRUE(locals.LocalsCountIs(0));
	EXPECT_TRUE(dbg.StepInto(1));
	EXPECT_TRUE(locals.LocalsCountIs(0));

}

void CLocalsCases::StepThroughPrologWithThisPaneActive(void)
{
	LOGTESTHEADER("StepThroughPrologWithThisPaneActive");

	// Restart and Go to the line at which we want to begin our tests
	EXPECT_TRUE(dbg.Restart());
	EXPECT_TRUE(src.Find("/* Second line for tests */", FALSE, FALSE, FALSE, 0 /*FD_DOWN*/)); 
	EXPECT_TRUE(dbg.StepToCursor());

	uivar.Activate();
	EXPECT_TRUE(ERROR_SUCCESS == uivar.SetPane(PANE_THIS)); 
	EXPECT_TRUE(dbg.StepInto(2));
	EXPECT_TRUE(locals.LocalsCountIs(3));
	EXPECT_TRUE(locals.VerifyLocalInfo("chArg", "98 'b'", "char", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("nJustLocalInt", "UNKNOWN", "int", NOT_EXPANDABLE));
	EXPECT_TRUE(locals.VerifyLocalInfo("dJustLocalDouble", "UNKNOWN", "double", NOT_EXPANDABLE));
}