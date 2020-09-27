/********************************************************************************
FILE: DMTEST1.CPP

PURPOSE:    
    Contains the "main code segment" for DANROSE's set of DMusic tests
    from which all the subfunctions (individual test cases) are called.  
********************************************************************************/

#include "globals.h"
#include "help_dowork.h"
#include "help_Definitions.h"
#include "help_suitelist.h"
#include "help_testcases.h"

TESTPARAMS g_TestParams = {0};

/********************************************************************************
Main code segment for the dmusic tests.
********************************************************************************/
VOID WINAPI DMTest1StartTest(HANDLE LogHandle)
{
    HRESULT hr = S_OK;
    SUITELIST *pSuiteList = NULL;
    DWORD dwPassed = 0;
    DWORD dwFailed = 0;

    //Set the log handle.
    //TODO: Remedy this; it's crappy design!!!
    g_hLog = LogHandle;

    //Initialize our network.
	// initialize the Xbox Secure Network Library (SNL).

	if ( 0 == XNetAddRef())
	{
		Log(ABORTLOGLEVEL, "[DMTEST1] Error: Unable to initialize Xbox SNL (error %d)\n", GetLastError() );
		hr = E_FAIL;
	}

    //Make sure nobody screwed up the static test array with dupes, tests with invalid types, etc.
    CHECKRUN(SanityCheckTestCases());

    //Run all of the test suites.
    ALLOCATEANDCHECK(pSuiteList, SUITELIST);
    CHECKRUN(pSuiteList->Load(g_szAppName, g_TestCases, g_dwNumTestCases));
    CHECKRUN(pSuiteList->CheckSuites());
    CHECKRUN(pSuiteList->RunSuites(&dwPassed, &dwFailed));
    SAFEDELETE(pSuiteList);


    //Free network.
    XNetRelease();

    //Either bail out (if we're in a suite with other tests) or loop forever (we want to view test results).
    EndTestRun();    
    
}





/********************************************************************************
Placeholder fuunction - does nothing.
********************************************************************************/
VOID WINAPI DMTest1EndTest( VOID )
{
}


/********************************************************************************
Export function pointers of StartTest and EndTest
********************************************************************************/
//lint -e10
//lint -e129
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( DMTest1 )
#pragma data_seg()

BEGIN_EXPORT_TABLE( DMTest1 )
    EXPORT_TABLE_ENTRY( "StartTest", DMTest1StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DMTest1EndTest )
END_EXPORT_TABLE( DMTest1 )
//lint +e10
//lint +e129



   
