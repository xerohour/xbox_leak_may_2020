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

namespace xdashsettings
{
    /***********************
    ************************
    *** Begin Test Cases ***
    ************************
    ***********************/

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////// CLOCK ////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockInitialBootGoodBattery( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a new user receives their XBox
        // BEFORE their 20 day battery dies, they can boot in to the default
        // title, and the clock is set to the correct time.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_FACTORY_SETTINGS_GOOD_BATTERY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_INITIAL_BOOT_GOOD_BATTERY );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockInitialBootBadBattery( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a new user receives their XBox
        // AFTER their 20 day battery dies, they can boot in to the default
        // title, and the clock is set to the correct time.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_FACTORY_SETTINGS_BAD_BATTERY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_INITIAL_BOOT_BAD_BATTERY );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockStartRange( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that a user cannot boot to the XDash and set
        // the clock to a date BEFORE the RTM date of the XBox.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( L"%s %s", STEP_VERIFY_CLOCK_CANT_SET_PRIOR_TO, gc_lpwszCLOCK_VALID_DATA[SETTINGS_AREA_CLOCK_START_RANGE] );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockEndRange( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that a user cannot boot to the XDash and set
        // the clock to a date AFTER the largest allowable date for the XBox.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( L"%s %s", STEP_VERIFY_CLOCK_CANT_SET_AFTER, gc_lpwszCLOCK_VALID_DATA[SETTINGS_AREA_CLOCK_END_RANGE] );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockMonthsUseValidNumDays( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that the Default Title clock accepts the proper
        // number of days for each month of a non-leap year.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK_NON_LEAP_YEAR );
        pStepsLog->AddStep( STEP_SELECT_CLOCK_EACH_MONTH );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_NUM_DAYS_FOR_CURRENT_MONTH );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockFebUseValidNumDaysLeapYear( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that the Default Title clock accepts the proper
        // number of days for Februrary for a leap year.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK_LEAP_YEAR );
        pStepsLog->AddStep( STEP_SELECT_CLOCK_MONTH_FEB );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_NUM_DAYS_FOR_CURRENT_MONTH );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockDatesStoredToXDisk( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that the Default Title will store the dates
        // and times the user sets on to the XDisk, and displays them upon
        // next boot.
        //////////////////////////////////////////////////////////////////////////

        // Subset of the valid data we we to use for the clock stored to disk test
        enum ClockValidDataWithinList clockData[] = {
            SETTINGS_AREA_CLOCK_START_RANGE,
            SETTINGS_AREA_CLOCK_END_RANGE
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK_INFO_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_INFO_ACCORD_TO_VAR );

        // For each clock setting we wish to use
        for( size_t clockSetting = 0; clockSetting < ARRAYSIZE( clockData ); ++clockSetting )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Set Data/Time to: %s", xlogVariationNum, gc_lpwszCLOCK_VALID_DATA[clockData[clockSetting]] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( stepsVariation );
            
            TESTCASE( xlogVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockSetUsingXZone( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that the Default Title will allow the XZone to
        // update it's Date / Time information.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_CLOCK_INCORRECT );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_XZONE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_XZONE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_COUNTRY_CURRENT );
        pStepsLog->AddStep( STEP_SELECT_XZONE_SET_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockXZoneSetsDSTInfo( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that the Default Title will allow the XZone to
        // update it's Date / Time for countries with DST information.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_FACTORY_SETTINGS_GOOD_BATTERY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_XZONE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_XZONE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_COUNTRY_WITH_DST );
        pStepsLog->AddStep( STEP_SELECT_XZONE_SET_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_FOR_DST_CORRECTLY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_FOR_DST_CORRECTLY );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockManualSetRemovesDSTInfo( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that the Default Title will reset the DST
        // information after a user has set the Clock on the XZone, and then
        // manually changes the time/date information.
        //////////////////////////////////////////////////////////////////////////

        //BUGBUG: Possibly need to do this for each settable field on the clock.
        //        May be able to get around this by doing a code review with the
        //        developer

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_FACTORY_SETTINGS_GOOD_BATTERY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_XZONE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_XZONE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_COUNTRY_WITH_DST );
        pStepsLog->AddStep( STEP_SELECT_XZONE_SET_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK_NEW );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_DST_RESET );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockDSTUpdatesXBoxAlreadyOn( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the XBox is set to honor daylight
        // savings time, if you leave the XBox on, at the appropriate moment
        // the clock will be updated.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_DATE_JUST_BEFORE_DST );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_ACTION_WAIT_FOR_DST );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_FOR_DST_CORRECTLY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_FOR_DST_CORRECTLY );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockDSTUpdatesXBoxPoweredOn( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the XBox is set to honor daylight
        // savings time, if power off the XBox, and power it back on AFTER
        // the DST rollover time has passed, the clock will be updated.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_DATE_JUST_BEFORE_DST );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_ACTION_WAIT_FOR_DST );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_FOR_DST_CORRECTLY );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockXZoneSetClockPowerOff( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the XZone is in the middle of setting
        // the XBox clock, and the user presses Power, we will do the correct
        // thing.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_XZONE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_XZONE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_COUNTRY_WITH_DST );
        pStepsLog->AddStep( STEP_SELECT_XZONE_SET_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_STARTED );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_NOT_UPDATED );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockXZoneSetClockUnplugPower( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the XZone is in the middle of setting
        // the XBox clock, and the user unplugs the power cord, we will do the
        // correct thing.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_XZONE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_XZONE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_COUNTRY_WITH_DST );
        pStepsLog->AddStep( STEP_SELECT_XZONE_SET_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_STARTED );
        pStepsLog->AddStep( STEP_REMOVE_POWER_PLUG_FROM_XBOX );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_INSERT_PLUG_IN_TO_XBOX );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_NOT_UPDATED );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockXZoneSetClockUnplugBB( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the XZone is in the middle of setting
        // the XBox clock, and the user unplugs the Broadband connection, we will
        // do the correct thing.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_XZONE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_XZONE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_COUNTRY_WITH_DST );
        pStepsLog->AddStep( STEP_SELECT_XZONE_SET_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_STARTED );
        pStepsLog->AddStep( STEP_REMOVE_BB_PLUG_FROM_XBOX );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_NOTIFICATION );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_NOT_UPDATED );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_XZONE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_XZONE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_COUNTRY_WITH_DST );
        pStepsLog->AddStep( STEP_SELECT_XZONE_SET_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CLOCK_SET_CORRECTLY );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD ClockNewSettingUpdatesClockOnly( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the user selects a new time/date option
        // in the default title, no other settings are affected.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_NOTE_CONFIG_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_CLOCK_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_CLOCK_NEW );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_SETTINGS_ACCORD_TO_NOTE );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////// LANGUAGE ////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////

    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD LangXBoxRegionsCorrectLanguage( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that the XBox is defaulting to the correct
        // language for the various regions we support.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_FROM_REGION_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_LANGUAGE_ACCORD_TO_VAR );

        // For each clock setting we wish to use
        for( size_t xboxRegion = 0; xboxRegion < ARRAYSIZE( gc_lpwszXBOX_NATIVE_REGIONS ); ++xboxRegion )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: XBox Region: %s", xlogVariationNum, gc_lpwszXBOX_NATIVE_REGIONS[xboxRegion] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( stepsVariation );
            
            TESTCASE( xlogVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD LangNewLangDisplayAndConfig( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the user selects a new language in the
        // default title, the change is immediate, and does not require a
        // reboot.  It also verifies that the setting is stored to the XDisk,
        // and upon reboot, the Language setting is remembered.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_FROM_REGION_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_LANGUAGE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_LANGUAGE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_LANGUAGE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_LANGUAGE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_LANGUAGE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_LANGUAGE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_LANGUAGE_ACCORD_TO_VAR );

        // For each clock setting we wish to use
        for( size_t language = 0; language < ARRAYSIZE( gc_lpwszXBOX_DT_LANGUAGES ); ++language )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Language: %s", xlogVariationNum, gc_lpwszXBOX_DT_LANGUAGES[language] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( stepsVariation );
            
            TESTCASE( xlogVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD LangNewLangUpdatesLangOnly( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the user selects a new language in the
        // default title, no other settings are affected.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_NOTE_CONFIG_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_LANGUAGE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_LANGUAGE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_LANGUAGE_NEW );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_LANGUAGE );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_SETTINGS_ACCORD_TO_NOTE );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////// VIDEO ////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD VideoInitalBootCorrectDefault( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that upon initial boot of the XBox, the video
        // setting is correctly defaulted to the 'Normal' choice.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_FACTORY_SETTINGS_GOOD_BATTERY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_VIDEO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_VIDEO_SCREEN );
        pStepsLog->AddStep( L"%s %s", STEP_VERIFY_VIDEO_SET_TO, gc_lpwszXBOX_DT_VIDEO_OPTIONS[SETTINGS_VIDEO_DEFAULT] );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD VideoNewSettingDisplayAndConfig( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the user selects a new video option in
        // the default title, the change is immediate, and does not require a
        // reboot.  It also verifies that the setting is stored to the XDisk,
        // and upon reboot, the setting is remembered.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_FROM_REGION_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_VIDEO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_VIDEO_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_VIDEO_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_VIDEO_SET_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_VIDEO_SET_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING );

        // For each clock setting we wish to use
        for( size_t videoSetting = 0; videoSetting < ARRAYSIZE( gc_lpwszXBOX_DT_VIDEO_OPTIONS ); ++videoSetting )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Video setting: %s", xlogVariationNum, gc_lpwszXBOX_DT_VIDEO_OPTIONS[videoSetting] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( stepsVariation );
            
            TESTCASE( xlogVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }
    
        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD VideoNewSettingUpdatesVideoOnly( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the user selects a new video option
        // in the default title, no other settings are affected.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_NOTE_CONFIG_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_VIDEO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_VIDEO_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_VIDEO_NEW );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_VIDEO_SET_CORRECTLY );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_SETTINGS_ACCORD_TO_NOTE );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////// AUDIO ////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD AudioInitalBootCorrectDefault( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that upon initial boot of the XBox, the audio
        // setting is correctly defaulted to the 'Normal' choice, no matter
        // which A/V pack is installed.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_AVPACK_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_XBOX_FACTORY_SETTINGS_GOOD_BATTERY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_AUDIO_SCREEN );
        pStepsLog->AddStep( L"%s %s", STEP_VERIFY_AUDIO_SET_TO, gc_lpwszXBOX_DT_AUDIO_OPTIONS[SETTINGS_AUDIO_DEFAULT] );

        // For each A/V pack in our list
        for( size_t avPack = 0; avPack < ARRAYSIZE( gc_lpwszXBOX_AV_PACKS ); ++avPack )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: A/V pack installed: %s", xlogVariationNum, gc_lpwszXBOX_AV_PACKS[avPack] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( stepsVariation );
            
            TESTCASE( xlogVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }

        return 0;
    };

    
    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD AudioChoicesNoAVPack( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the XBox does not have an A/V pack
        // installed, they will only have a subset of the audio options
        // available to them in the default title.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_AVPACK_NONE );
        pStepsLog->AddStep( STEP_SETUP_XBOX_FACTORY_SETTINGS_GOOD_BATTERY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_AUDIO_SCREEN );
        pStepsLog->AddStep( L"%s %s and %s", STEP_VERIFY_AUDIO_CHOICES_AVAILABLE_ARE, gc_lpwszXBOX_DT_AUDIO_OPTIONS[SETTINGS_AUDIO_STEREO], gc_lpwszXBOX_DT_AUDIO_OPTIONS[SETTINGS_AUDIO_MONO] );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD AudioChoicesWithAVPack( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the XBox DOES have an A/V pack
        // installed, they will have the full list of audio options
        // available to them in the default title.
        //////////////////////////////////////////////////////////////////////////

        const size_t buffSize = 512;    // Buffer size to be used by our static choice list
        const WCHAR* sepText = L", ";   // Used to separate each choice within the step
        size_t numCharsCopied = 0;      // Number of characters copied in to our buffer

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        size_t numCharsToBeCopied = 0;                  // The number of characters the current choice will end up copying
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running
        WCHAR choicesAvailable[buffSize];               // The audio choices available to the user with an A/V pack installed

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_AVPACK_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_XBOX_FACTORY_SETTINGS_GOOD_BATTERY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_AUDIO_SCREEN );

        ZeroMemory( choicesAvailable, sizeof( WCHAR ) * buffSize );
        for( size_t choice = 0; choice < ARRAYSIZE( gc_lpwszXBOX_DT_AUDIO_OPTIONS ); ++choice )
        {
            // The number of characters to be copied in this concat operation
            numCharsToBeCopied = wcslen( gc_lpwszXBOX_DT_AUDIO_OPTIONS[choice] );

            // If we are not on our last choice, add the length of our sep text to the char copy total
            if( 1 != ARRAYSIZE( gc_lpwszXBOX_DT_AUDIO_OPTIONS ) - choice )
            {
                numCharsToBeCopied += wcslen( sepText );
            }


            // If we don't have enough room in our buffer, let the user know and bail the test case
            if( buffSize - ( numCharsCopied + numCharsToBeCopied ) <= 0 )
            {
                DebugPrint( "SettingsTests():AudioChoicesWithAVPack():Out of buffer space!!\n" );
                return -1;
            }
            
            wcsncat( choicesAvailable, gc_lpwszXBOX_DT_AUDIO_OPTIONS[choice], buffSize - numCharsCopied );
            numCharsCopied += wcslen( gc_lpwszXBOX_DT_AUDIO_OPTIONS[choice] );

            // If we are not on our last choice, append the separator text
            if( 1 != ARRAYSIZE( gc_lpwszXBOX_DT_AUDIO_OPTIONS ) - choice )
            {
                wcsncat( choicesAvailable, sepText, buffSize - numCharsCopied );
                numCharsCopied += wcslen( sepText );
            }

        }

        pStepsLog->AddStep( L"%s %s", STEP_VERIFY_AUDIO_CHOICES_AVAILABLE_ARE, choicesAvailable );

        // For each A/V pack in our list
        for( size_t avPack = 0; avPack < ARRAYSIZE( gc_lpwszXBOX_AV_PACKS ); ++avPack )
        {
            // Skip the variation if there is no A/V pack installed
            if( avPack > AVPACK_NONE)
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: A/V pack installed: %s", xlogVariationNum, gc_lpwszXBOX_AV_PACKS[avPack] );

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


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD AudioNewSettingConfig( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the user selects a new audio option in
        // the default title, the change is stored to the XDisk, and upon reboot,
        // the setting is remembered.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_AVPACK_ANY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_AUDIO_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO_OPTION_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_AUDIO_SET_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_AUDIO_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_AUDIO_SET_ACCORD_TO_VAR );

        // For each audio option we wish to use
        for( size_t audioOption = 0; audioOption < ARRAYSIZE( gc_lpwszXBOX_DT_AUDIO_OPTIONS ); ++audioOption )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Audio option: %s", xlogVariationNum, gc_lpwszXBOX_DT_AUDIO_OPTIONS[audioOption] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( stepsVariation );
            
            TESTCASE( xlogVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }
    
        return 0;
    };


    /*
    Routine Description:


    Arguments:

        HANDLE hLog - handle to an XLOG object
        CStepsLog* pStepsLog - Pointer to the steps logging object

    Return Value:

        DWORD -

    */
    DWORD AudioAVRemovedAfterOptionSet( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if the user selects an audio choice that
        // requires an A/V pack to be installed, and then the user switches
        // the A/V packs around, the correct options are set and stored
        // to the XDisk.
        //////////////////////////////////////////////////////////////////////////

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_XBOX_AVPACK_ANY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_AUDIO_SCREEN );
        pStepsLog->AddStep( L"%s %s", STEP_SELECT_AUDIO_OPTION_TO_BE, gc_lpwszXBOX_DT_AUDIO_OPTIONS[SETTINGS_AUDIO_ANALOG] );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_REMOVE_AVPACK_FROM_XBOX );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_AUDIO_SCREEN );
        pStepsLog->AddStep( L"%s %s", STEP_VERIFY_AUDIO_SET_TO, gc_lpwszXBOX_DT_AUDIO_OPTIONS[SETTINGS_AUDIO_STEREO] );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_SETUP_XBOX_AVPACK_ANY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SETTINGS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_AUDIO );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SETTINGS_AUDIO_SCREEN );
        pStepsLog->AddStep( L"%s %s", STEP_VERIFY_AUDIO_SET_TO, gc_lpwszXBOX_DT_AUDIO_OPTIONS[SETTINGS_AUDIO_STEREO] );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;
    
        return 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////// PARENTAL CONTROL ////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

}; // namespace