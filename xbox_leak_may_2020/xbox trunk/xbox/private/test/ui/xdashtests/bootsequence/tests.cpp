/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    Tests.cpp

Abstract:


Author:

    Jim Helm (jimhelm)

Environment:

    XBox

Revision History:
    10-18-2000  Created

Notes:

*/

#include "stdafx.h"
#include "commontest.h"
#include "tests.h"

namespace xdashboot
{
    /***********************
    ************************
    *** Begin Test Cases ***
    ************************
    ***********************/


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD Test1Here( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that 
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( L"Step 1" );
        pStepsLog->AddStep( L"Step 2" );
        pStepsLog->AddStep( L"Step 3" );
        pStepsLog->AddStep( L"Step 4" );

        // For every screen in the XDash
        _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
        _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Variation Here", xlogVariationNum );

        // Log the test case variation to our Steps File
        pStepsLog->AddVariation( stepsVariation );

        TESTCASE( xlogVariation )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };
}; // namespace