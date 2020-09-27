/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    XDashMemory.cpp

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
    10/09/2000  Created

*/


#include <xlog.h>
#include "stdafx.h"
#include "XDashMemory.h"
#include "tests.h"

namespace xdashmemory
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
    DWORD XDashMemoryRunAll( HANDLE hLog, CStepsLog* pStepsLog )
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
            BUILTAPISTRUCT( MemColDukeMUVarsDisplay ),
            BUILTAPISTRUCT( MemColMDVariousSizesDisplay ),
            BUILTAPISTRUCT( MemColCannotSelectEmptyMD ),
            BUILTAPISTRUCT( MemAreaTitleDisplay ),
            BUILTAPISTRUCT( MemAreaFreeSpaceDisplay ),
            BUILTAPISTRUCT( MemAreaSGWrapping ),
            BUILTAPISTRUCT( MemAreaTitlesPerScreen ),
            BUILTAPISTRUCT( MemAreaGeneralDisplay ),
            BUILTAPISTRUCT( MemAreaHWDeviceRemoved ),
            BUILTAPISTRUCT( MemAreaSorting ),
            BUILTAPISTRUCT( MemAreaSavedGameMetaData ),
            BUILTAPISTRUCT( SavedGameTitleSingleSGDisplay ),
            BUILTAPISTRUCT( SavedGameTitleMultipleSGDisplay ),
            BUILTAPISTRUCT( SavedGameNavCancel ),
            BUILTAPISTRUCT( SavedGameNavCopy ),
            BUILTAPISTRUCT( DeleteOnlySavedGameFromMD ),
            BUILTAPISTRUCT( DeleteSingleSavedGameFromMD ),
            BUILTAPISTRUCT( DeleteGameRemovesGameProperMD ),
            BUILTAPISTRUCT( DeleteSingleGameFromMDPowerOff ),
            BUILTAPISTRUCT( DeleteSingleGameFromMDPullsPlug ),
            BUILTAPISTRUCT( DeleteMultipleSavedGamesFromMD ),
            BUILTAPISTRUCT( DeleteSavedGameNavConfirmCancel ),
            BUILTAPISTRUCT( DeleteTitleFromMD ),
            BUILTAPISTRUCT( DeleteTitleNavConfirmCancel ),
            BUILTAPISTRUCT( HWInsertedDuringOperation ),
            BUILTAPISTRUCT( HWRemovedDuringOperation ),
            BUILTAPISTRUCT( HWRemovedDuringOperationFail ),
            BUILTAPISTRUCT( HWSourceRemovedBeforeCopy ),
            BUILTAPISTRUCT( CopySingleSGFromMDToMD ),
            BUILTAPISTRUCT( CopySingleSGFromMDToMDPowerOff ),
            BUILTAPISTRUCT( CopySingleSGFromMDToMDPullsPlug ),
            BUILTAPISTRUCT( CopySingleSGAllUnitsFull ),
            BUILTAPISTRUCT( CopySingleSGAllUnitsFullHSFull ),
            BUILTAPISTRUCT( CopySingleSGAllUnitsFullHSEmpty ),
            BUILTAPISTRUCT( CopySingleSGHSFullEmptyFull ),
            BUILTAPISTRUCT( CopyMultipleSGsFromMDToMD ),
            BUILTAPISTRUCT( CopySingleSGFileVerification ),
            BUILTAPISTRUCT( CopyMultipleSGsFileVerification ), 
            BUILTAPISTRUCT( MemCopyDukeHotPlugVarsDisplay ),
            BUILTAPISTRUCT( MemCopyMUHotPlugVarsDisplay ),
            BUILTAPISTRUCT( MemCopyMDVariousSizesDisplay ),
            BUILTAPISTRUCT( MemCopyDestMUNotEnoughRoom ),
            BUILTAPISTRUCT( MemCopyTitleSingleSGDisplay ),
            BUILTAPISTRUCT( MemCopyTitleMultipleSGDisplay ),
            BUILTAPISTRUCT( BootToMemCleanUpMAFullDisplay ),
            BUILTAPISTRUCT( BootToMemCleanUpMAEmptyDisplay ),
            BUILTAPISTRUCT( BootToMemCleanUpMAFull0blocks ),
            BUILTAPISTRUCT( BootToMemCleanUpMAFullDeleteAll ),
            BUILTAPISTRUCT( BootToMemCleanUpClearSpace ),
            BUILTAPISTRUCT( BootToMemCleanUpHotSwapMA ),
            BUILTAPISTRUCT( BootToMemCleanUpNavAway )
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
VOID WINAPI XDashMemoryStartTest( HANDLE hLog )
{
    using namespace xdashmemory;

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

        XDashMemoryRunAll( hLog, &StepsLog );

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
        DebugPrint( "**ERROR: XDashMemoryStartTest():Could not allocate memory!!" );
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
VOID WINAPI XDashMemoryEndTest(void)
{
}


#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xdashmemory )
#pragma data_seg()

BEGIN_EXPORT_TABLE( xdashmemory )
    EXPORT_TABLE_ENTRY( "StartTest", XDashMemoryStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XDashMemoryEndTest )
END_EXPORT_TABLE( xdashmemory )
