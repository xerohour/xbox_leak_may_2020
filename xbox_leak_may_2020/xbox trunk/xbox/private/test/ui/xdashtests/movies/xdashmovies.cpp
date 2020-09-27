/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    XDashMovies.cpp

Abstract:

    The DLL exports two of the mandatory APIs: StartTest and EndTest.
    TESTINI.INI must be modified correctly so that the
    harness will load and call StartTest/EndTest.

    Please see "x-box test harness.doc" for more information.

Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10/16/2000  Created

*/


#include <xlog.h>
#include "stdafx.h"
#include "XDashMovies.h"
#include "tests.h"

namespace xdashmovies
{
    static BOOL printSteps = true;

    /*

    Routine Description:


    Arguments:

        HANDLE hLog - to the logging object
        CStepsLog* pStepsLog - Object used to log our test steps

    Return Value:

        DWORD

    */
    DWORD XDashMoviesRunAll( HANDLE hLog, CStepsLog* pStepsLog )
    {
        struct APIFUNCT
        {
            APITEST_FUNC f;
            WCHAR *name;
            char *aname;
        };
    
        #define BUILTAPISTRUCT(n) { n , L#n , #n }

        // Define all the API tests
        APIFUNCT testlist[] =
        {
            /*
            */
            BUILTAPISTRUCT( Test1Here )
            /*
            */
        };

        // Run the suite of tests

        // run each test
        for( size_t j = 0; j < ARRAYSIZE( testlist ); j++ )
        {
            xSetFunctionName(hLog, testlist[j].aname);
            DebugPrint( "%s: Entering test = '%ws()'\n", COMPONENT_NAME_A, testlist[j].name );

            // Log the testcase name to our steps log
            pStepsLog->SetCaseName( testlist[j].name );
            testlist[j].f( hLog, pStepsLog );

            // Log the test case (if the user wishes to)
            if ( printSteps )
            {
                pStepsLog->LogTestCase();
            }

            // Clear out the log for our next test
            pStepsLog->ClearTestCase();
        }

        return 0;
    }
}; // namespace

/*

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    hLog - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

*/
VOID WINAPI XDashMoviesStartTest( HANDLE hLog )
{
    using namespace xdashmovies;

    // test settings
    const int buffLen = 1024;
    
    char* ownerAlias     = new char[buffLen];
    char* componentMajor = new char[buffLen];
    char* componentMinor = new char[buffLen];
    char* stepsFileName  = new char[buffLen];
    int HTMLLogFormat    = 0;

    if( ownerAlias && componentMajor && componentMinor && stepsFileName )
    {
        // Object used to log our steps
        CStepsLog StepsLog;

        // Pull out internal INI file settings
        GetProfileStringA( DLL_NAME_A,   "Owner",           "Owner",          ownerAlias,     buffLen );
        GetProfileStringA( DLL_NAME_A,   "ComponentMajor",  "ComponentMajor", componentMajor, buffLen );
        GetProfileStringA( DLL_NAME_A,   "ComponentMinor",  "ComponentMinor", componentMinor, buffLen );
        GetProfileStringA( DLL_NAME_A,   "StepsFile",       "StepsFile",      stepsFileName,  buffLen );
        printSteps = GetProfileIntA( DLL_NAME_A, "PrintSteps", 0 );
        HTMLLogFormat = GetProfileIntA( DLL_NAME_A, "HTMLStepsLog", 1 );

        // Set the format for our STEPS log file
        StepsLog.SetHTMLLogFormat( HTMLLogFormat );

        xSetOwnerAlias( hLog, ownerAlias );
        xSetComponent( hLog, componentMajor, componentMinor );
        DebugPrint( "%s: **************************************************************\n", COMPONENT_NAME_A );
        DebugPrint( "%s: Entering %sStartTest()\n", COMPONENT_NAME_A, COMPONENT_NAME_A );
        DebugPrint( "%s: Internal INI Settings:\n", COMPONENT_NAME_A );
        DebugPrint( "   Owner           = '%s'\n", ownerAlias );
        DebugPrint( "   Component Major = '%s'\n", componentMajor );
        DebugPrint( "   Component Minor = '%s'\n", componentMinor );

        // Open our Steps Log File
        if (printSteps)
        {
            StepsLog.OpenLog( stepsFileName );
        }

        XDashMoviesRunAll( hLog, &StepsLog );

        if ( printSteps )
        {
            // Close our Steps Log File
            StepsLog.CloseLog();
        }

        DebugPrint( "%s: Leaving %sStartTest()\n", COMPONENT_NAME_A, COMPONENT_NAME_A );
        DebugPrint( "%s: **************************************************************\n", COMPONENT_NAME_A );
        xSetOwnerAlias( hLog, NULL );
    }
    else // Memory allocation failed
    {
        DebugPrint( "**ERROR: %sStartTest():Could not allocate memory!!", COMPONENT_NAME_A );
    }

    if( ownerAlias )
    {
        delete[] ownerAlias;
        ownerAlias = NULL;
    }

    if( componentMajor )
    {
        delete[] componentMajor;
        componentMajor = NULL;
    }
    
    if( componentMinor )
    {
        delete[] componentMinor;
        componentMinor = NULL;
    }
    
    if( stepsFileName )
    {
        delete[] componentMinor;
        componentMinor = NULL;
    }
}


/*

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

*/
VOID WINAPI XDashMoviesEndTest(void)
{
}


#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xdashmovies )
#pragma data_seg()

BEGIN_EXPORT_TABLE( xdashmovies )
    EXPORT_TABLE_ENTRY( "StartTest", XDashMoviesStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XDashMoviesEndTest )
END_EXPORT_TABLE( xdashmovies )
