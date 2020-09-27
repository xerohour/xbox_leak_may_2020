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
    10-16-2000  Created

Notes:

*/

#include "stdafx.h"
#include "commontest.h"
#include "tests.h"

namespace xdashnav
{
    // Globals
    CXDashScreenList g_XDashScreens( "t:\\XDSCREEN.INI" );     // All screens and controls for the XBOX
    
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
    DWORD AllControlsBasicNav( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that each control on each screen of the XDash
        // responds to the basic controls (standard select and back buttons)
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_INVOKE_SCREEN_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SELECT_CONROL_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_DEST_SCREEN_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_ACCORD_TO_VAR );

        // For every screen in the XDash
        for( size_t screenIndex = 0; screenIndex < g_XDashScreens.GetNumScreens(); ++screenIndex )
        {
            // For every control on each screen
            for( size_t controlIndex = 0; controlIndex < g_XDashScreens.GetNumControlsOnScreenByIndex( screenIndex ); ++controlIndex )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: SCREEN: %S, CONTROL: %S, DEST: %S", xlogVariationNum, g_XDashScreens.GetScreenNameByIndex( screenIndex ), g_XDashScreens.GetControlNameFromScreenByIndex( screenIndex, controlIndex ), g_XDashScreens.GetControlDestFromScreenByIndex( screenIndex, controlIndex ) );

                // Log the test case variation to our Steps File
                pStepsLog->AddVariation( stepsVariation );

                TESTCASE( xlogVariation )
                {
                    TESTMNUL( hLog, "Manual Test" );
                } ENDTESTCASE;
            }
        }
    
        return 0;
    };
}; // namespace