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
    10-09-2000  Created

Notes:

*/

#include "stdafx.h"
#include "commontest.h"
#include "tests.h"

namespace xdashmemory
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
    DWORD MemColDukeMUVarsDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Collections main screen displays
        // properly with variations of Dukes and MUs.
        //
        // It also ensures that Dukes and MUs can be Hot-Plugged on this screen,
        // and it will behave as expected.
        //////////////////////////////////////////////////////////////////////////

        size_t testVariationNum = 0;                        // The Variation Number of the test we are running
        WCHAR portConfiguration[VARIATION_BUFFER_SIZE];     // The String representation of the XBox Port Configuration we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];         // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        for ( size_t port1 = 0; port1 < ARRAYSIZE(validDukeAndMUConfigs); ++port1 )
        {
            for ( size_t port2 = 0; port2 < ARRAYSIZE(validDukeAndMUConfigs); ++port2 )
            {
                for ( size_t port3 = 0; port3 < ARRAYSIZE(validDukeAndMUConfigs); ++port3 )
                {
                    for ( size_t port4 = 0; port4 < ARRAYSIZE(validDukeAndMUConfigs); ++port4 )
                    {
                        _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
                        _snwprintf( portConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Port 1: %s, Port 2: %s, Port 3: %s, Port 4: %s", testVariationNum, validDukeAndMUConfigs[port1], validDukeAndMUConfigs[port2], validDukeAndMUConfigs[port3], validDukeAndMUConfigs[port4]);
  
                        // Log the test case variation to our Steps File
                        pStepsLog->AddVariation( portConfiguration );

                        TESTCASE( testVariation )
                        {
                            TESTMNUL( hLog, "Manual Test" );
                        } ENDTESTCASE;
                    }
                }
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
    DWORD MemColMDVariousSizesDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Collection main screen displays
        // properly with Memory Devices that are filled with various amounts of
        // data.  It verifies that the information displayed when you
        // "highlight" on of these areas is displayed correctly.
        //////////////////////////////////////////////////////////////////////////

        size_t testVariationNum = 0;                        // The Variation Number of the test we are running
        WCHAR deviceConfiguration[VARIATION_BUFFER_SIZE];   // The String representation of the XBox Device Configuration we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];         // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // Xdisk verified with each MD block configuration
        for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
        {
            _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
            _snwprintf( deviceConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Xdisk, MemArea Block Configuration: %s", testVariationNum, gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( deviceConfiguration );

            TESTCASE( testVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }


        // All Dukes in all ports, and each port on each Duke tested with the MD block configurations
        for( size_t port = 1; port <= gc_nNUM_XBOX_PORTS; ++port )
        {
            for( size_t dukeSlot = 1; dukeSlot <= gc_nNUM_DUKE_SLOTS; ++dukeSlot )
            {
                for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
                {
                    _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
                    _snwprintf( deviceConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Port: %d, Duke Slot: %d, MemArea Block Configuration: %s", testVariationNum, port, dukeSlot, gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );
  
                    // Log the test case variation to our Steps File
                    pStepsLog->AddVariation( deviceConfiguration );

                    TESTCASE( testVariation )
                    {
                        TESTMNUL( hLog, "Manual Test" );
                    } ENDTESTCASE;
                }
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
    DWORD MemColCannotSelectEmptyMD( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Collection main screen does
        // note allow the user to select a Memory Device that is empty.
        //////////////////////////////////////////////////////////////////////////

        size_t testVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR memArea[VARIATION_BUFFER_SIZE];           // The String representation of the XBox Memory Area we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_EMPTY );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++currentMemArea )
        {
            _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
            _snwprintf( memArea, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s", testVariationNum, gc_lpwszMEM_AREAS[currentMemArea] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( memArea );

            TESTCASE( testVariation )
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
    DWORD MemAreaTitleDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Device (area) screen properly
        // displays the title of the selected Memory Device (area)
        //////////////////////////////////////////////////////////////////////////

        size_t testVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR memArea[VARIATION_BUFFER_SIZE];           // The String representation of the XBox Memory Area we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MD_TEXT_REFLECTS_VARIATION );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++currentMemArea )
        {
            _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
            _snwprintf( memArea, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s", testVariationNum, gc_lpwszMEM_AREAS[currentMemArea] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( memArea );

            TESTCASE( testVariation )
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
    DWORD MemAreaFreeSpaceDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Device (area) screen Properly
        // Displays the amount of space that's avialable on a MD
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t testVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR memDeviceInfo[VARIATION_BUFFER_SIZE];     // The String representation of the XBox Memory Device and Configuration we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MD_SPACE_REFLECTS_VARIATION );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
            {
                _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
                _snwprintf( memDeviceInfo, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, MemArea Block Configuration: %s", testVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );

                // Log the test case variation to our Steps File
                pStepsLog->AddVariation( memDeviceInfo );

                TESTCASE( testVariation )
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
    DWORD MemAreaSGWrapping( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // As games have more and more save games, there comes a point at which
        // the game must start to "wrap" the save games to the next line of the
        // display.  This test is to ensure that wrapping is working as expected.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_XDISK
        };

        WCHAR* memAreaSavedGameConfigs[] = {
            L"A Title with 1 saved game less than 1 screen line of saved games",
            L"A Title with EXACTLY 1 screen line of saved games",
            L"A Title with 1 saved game more than 1 screen line of saved games",
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING );
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            for( size_t sgConfig = 0; sgConfig < ARRAYSIZE( memAreaSavedGameConfigs ); ++sgConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Saved Game Configuration: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], memAreaSavedGameConfigs[sgConfig] );

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
    DWORD MemAreaTitlesPerScreen( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // As games have more and more save games, there comes a point at which
        // the game must start to "wrap" the save games.  This will cause less
        // "titles (games)" to appear on the screen.  This test will verify that
        // no matter how many lines of saved games we have, the displays always
        // looks good.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_XDISK
        };

        WCHAR* memAreaSavedGameConfigs[] = {
            L"10 Titles on the memory area, each with no more than 1 lines of saved game.",
            L"10 Titles on the memory area, the 1st title should have 2 lines saved games.",
            L"10 Titles on the memory area, the 1st title should have 3 lines saved games.",
            L"10 Titles on the memory area, the 1st title should have 4 lines saved games."
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING );
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            for( size_t sgConfig = 0; sgConfig < ARRAYSIZE( memAreaSavedGameConfigs ); ++sgConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Saved Game Configuration: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], memAreaSavedGameConfigs[sgConfig] );

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
    DWORD MemAreaGeneralDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will ensure that the Memory Area screen displays correctly
        // with variations of the number of titles and saved games that are
        // being presented to the user.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING );
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            for( size_t sgConfig = 0; sgConfig < ARRAYSIZE( gc_lpwszMEM_AREA_SAVEGAME_CONFIGS ); ++sgConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Saved Game Configuration: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], gc_lpwszMEM_AREA_SAVEGAME_CONFIGS[sgConfig] );

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
    DWORD MemAreaHWDeviceRemoved( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user has selected a Memory Area, and
        // then hardware is removed (currently selected MU, or the Duke), that the
        // xdash takes the user to the appropriate location.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1
        };

        // List of the hardware to be removed for this test
        WCHAR* hardwareToRemove[] = {
            L"Remove MU 1 from Duke 1",
            L"Remove Duke 1"
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_SOME_DATA );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            for( size_t hardwareDevice = 0; hardwareDevice < ARRAYSIZE( hardwareToRemove ); ++hardwareDevice )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Hardware to Remove: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], hardwareToRemove[hardwareDevice] );

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
    DWORD MemAreaSorting( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that the titles and saved games are displayed
        // in alphabetical order by the default title.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_TITLES_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_GAMES_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_TITLES_SORTED_ALPHABETICALLY );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAMES_SORTED_ALPHABETICALLY ); 

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // For each variation in the names
            for( size_t nameVar = 0; nameVar < ARRAYSIZE( gc_lpwszGAME_AND_TITLE_NAMES ); ++nameVar )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Titles/Games: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], gc_lpwszGAME_AND_TITLE_NAMES[nameVar] );

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
    DWORD MemAreaSavedGameMetaData( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test that when the user highlights a saved game, the META data
        // for that game is displayed properly with different variations.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING );
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // All META data missing
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: ALL Meta data MISSING", xlogVariationNum );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( stepsVariation );

            TESTCASE( xlogVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        

            // All META data MAX characters
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Maximum amount of chars for all pieces of META data simultaneously", xlogVariationNum );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( stepsVariation );

            TESTCASE( xlogVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;


            // SAVED GAME META: Variations in the name of a saved game
            for( size_t sgName = 0; sgName < ARRAYSIZE( gc_lpwszMETA_SAVED_GAME_NAME_CONFIGS ); ++sgName )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Normal META data, except Saved Game Name: %s", xlogVariationNum, gc_lpwszMETA_SAVED_GAME_NAME_CONFIGS[sgName] );
 
                // Log the test case variation to our Steps File
                pStepsLog->AddVariation( stepsVariation );

                TESTCASE( xlogVariation )
                {
                    TESTMNUL( hLog, "Manual Test" );
                } ENDTESTCASE;
            }
        
            // SAVED GAME META: Variations in the name of the game that a saved game is from (Parent game)
            for( size_t sgParentName = 0; sgParentName < ARRAYSIZE( gc_lpwszMETA_SAVED_GAME_PARENT_NAME_CONFIGS ); ++sgParentName )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Normal META data, except Saved Game Parent Name: %s", xlogVariationNum, gc_lpwszMETA_SAVED_GAME_PARENT_NAME_CONFIGS[sgParentName] );

                TESTCASE( xlogVariation )
                {
                    TESTMNUL( hLog, "Manual Test" );
                } ENDTESTCASE;
            }
        
            // SAVED GAME META: Variations in the size of the saved game
            for( size_t sgSize = 0; sgSize < ARRAYSIZE( gc_lpwszMETA_SAVED_GAME_SIZE_CONFIGS ); ++sgSize )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Normal META data, except Saved Game Size: %s", xlogVariationNum, gc_lpwszMETA_SAVED_GAME_SIZE_CONFIGS[sgSize] );

                TESTCASE( xlogVariation )
                {
                    TESTMNUL( hLog, "Manual Test" );
                } ENDTESTCASE;
            }

            // SAVED GAME META: Variations in the time that a game was saved
            for( size_t sgTime = 0; sgTime < ARRAYSIZE( gc_lpwszMETA_SAVED_GAME_TIME_CONFIGS ); ++sgTime )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Normal META data, except Saved Game Time: %s", xlogVariationNum, gc_lpwszMETA_SAVED_GAME_TIME_CONFIGS[sgTime] );

                TESTCASE( xlogVariation )
                {
                    TESTMNUL( hLog, "Manual Test" );
                } ENDTESTCASE;
            }

            // SAVED GAME META: Variations in the images representing a saved game
            for( size_t sgImage = 0; sgImage < ARRAYSIZE( gc_lpwszMETA_SAVED_GAME_IMAGE_CONFIGS ); ++sgImage )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Normal META data, except Saved Game Image: %s", xlogVariationNum, gc_lpwszMETA_SAVED_GAME_IMAGE_CONFIGS[sgImage] );

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
    DWORD SavedGameTitleSingleSGDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user selects a single saved game
        // that the title of the "saved game" dialog is displayed appropriately.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_VERIFY_TITLE_FROM_PARENT );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING);
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED);

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // SAVED GAME META: Variations in the name of the game that a saved game is from (Parent game)
            for( size_t sgParentName = 0; sgParentName < ARRAYSIZE( gc_lpwszMETA_SAVED_GAME_PARENT_NAME_CONFIGS ); ++sgParentName )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Select a SINGLE game from Memory Area: %s, Saved Game's Parent Name: %s", xlogVariationNum, gc_lpwszMEM_AREAS[currentMemArea], gc_lpwszMETA_SAVED_GAME_PARENT_NAME_CONFIGS[sgParentName] );

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
    DWORD SavedGameTitleMultipleSGDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user selects multiple saved games
        // that the title of the "saved game" dialog is displayed appropriately.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_XDISK
        };

        // Subset of the generic saved game selections we want to make
        enum SavedGamesGenericWithinList genSGs[] = {
            SAVED_GAMES_MULTIPLE,
            SAVED_GAMES_ALL
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_VERIFY_TITLE_CORRECT_NUM_GAMES_SELECTED );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING);
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED);

        // For each memory area available
        for( size_t currentMemArea = 0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // For each generic saved game config in our subset
            for( size_t sgConfig = 0; sgConfig < ARRAYSIZE( genSGs ); ++sgConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Select MULTIPLE games from Memory Area: %s, Saved Games to Select: %s", xlogVariationNum, gc_lpwszMEM_AREAS[currentMemArea], gc_lpwszGENERIC_SAVED_GAME_CONFIGS[genSGs[sgConfig]] );

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
    DWORD SavedGameNavCancel( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user selects a saved game, and then
        // chooses "cancel", they are navigated to the appropriate screen.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1
        };

        // Subset of the generic saved game selections we want to make
        enum SavedGamesGenericWithinList genSGs[] = {
            SAVED_GAMES_SINGLE,
            SAVED_GAMES_MULTIPLE
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_CANCEL );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // For each generic saved game config in our subset
            for( size_t sgConfig = 0; sgConfig < ARRAYSIZE( genSGs ); ++sgConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Games to Select: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], gc_lpwszGENERIC_SAVED_GAME_CONFIGS[genSGs[sgConfig]] );

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
    DWORD SavedGameNavCopy( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user selects a saved game, and then
        // chooses "copy", they are navigated to the appropriate screen.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1
        };

        // Subset of the generic saved game selections we want to make
        enum SavedGamesGenericWithinList genSGs[] = {
            SAVED_GAMES_SINGLE,
            SAVED_GAMES_MULTIPLE
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_COPY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // For each generic saved game config in our subset
            for( size_t sgConfig = 0; sgConfig < ARRAYSIZE( genSGs ); ++sgConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Games to Select: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], gc_lpwszGENERIC_SAVED_GAME_CONFIGS[genSGs[sgConfig]] );

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
    DWORD DeleteOnlySavedGameFromMD( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user deletes the only saved game from
        // a Memory Device (area), the UI is correct.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ONE_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]] );

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
    DWORD DeleteSingleSavedGameFromMD( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that deleting a single saved game from a Memory
        // Device, with different saved game and block configurations works as
        // expected.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // Each memory area available
        for( size_t currentMemArea = 0; currentMemArea < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++currentMemArea )
        {
            // Each Single Saved Game configuration
            for( size_t currentSavedGameConfig = 0; currentSavedGameConfig < ARRAYSIZE( gc_lpwszSINGLE_SAVED_GAME_CONFIGS ); ++currentSavedGameConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Game Configuration: %s", xlogVariationNum, gc_lpwszMEM_AREAS[currentMemArea], gc_lpwszSINGLE_SAVED_GAME_CONFIGS[currentSavedGameConfig] );

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
    DWORD DeleteGameRemovesGameProperMD( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that deleting a single saved game from a Memory
        // Area only deletes the game from that Memory Area, and not any other
        // area.  Each area in this case must have the SAME game on the MU.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_AREAS_SAME_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_VERIFY_ALL_MEM_AREAS_PROPER_FILES );

        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area to delete game from: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]] );

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
    DWORD DeleteSingleGameFromMDPowerOff( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that deleting a single saved game from a Memory
        // Device, and the user pressing the power button, completes the delete
        // as expected
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_IN_VAR_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_STARTED );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );

        // Each memory area available
        for( size_t currentMemArea = 0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s", xlogVariationNum, gc_lpwszMEM_AREAS[currentMemArea] );

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
    DWORD DeleteSingleGameFromMDPullsPlug( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that deleting a single saved game from a Memory
        // Device, and the user pulls the plug, only the files that were being
        // deleted has been affected.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_IN_VAR_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_STARTED );
        pStepsLog->AddStep( STEP_REMOVE_POWER_PLUG_FROM_XBOX );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEM_AREA_FILES_OK );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAMES_MARKED_BAD );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );

        // Each memory area available
        for( size_t currentMemArea = 0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s", xlogVariationNum, gc_lpwszMEM_AREAS[currentMemArea] );

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
    DWORD DeleteMultipleSavedGamesFromMD( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that deleting multiple saved games from a Memory
        // Device, with different saved game and block configurations works as
        // expected.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // Each memory area available
        for( size_t currentMemArea = 0; currentMemArea < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++currentMemArea )
        {
            // Each Single Saved Game configuration
            for( size_t currentSavedGameConfig = 0; currentSavedGameConfig < ARRAYSIZE( gc_lpwszMULTIPLE_SAVED_GAME_CONFIGS ); ++currentSavedGameConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Game Configuration: %s", xlogVariationNum, gc_lpwszMEM_AREAS[currentMemArea], gc_lpwszMULTIPLE_SAVED_GAME_CONFIGS[currentSavedGameConfig] );

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
    DWORD DeleteSavedGameNavConfirmCancel( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user selects a saved game, and then
        // chooses delete, that they can select 'cancel' from the Delete Game
        // confirmation dialog, and they are navigated to the appropriate screen.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1
        };

        // Subset of the generic saved game selections we want to make
        enum SavedGamesGenericWithinList genSGs[] = {
            SAVED_GAMES_SINGLE,
            SAVED_GAMES_MULTIPLE
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_CANCEL );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // For each generic saved game config in our subset
            for( size_t sgConfig = 0; sgConfig < ARRAYSIZE( genSGs ); ++sgConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Games to Select: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], gc_lpwszGENERIC_SAVED_GAME_CONFIGS[genSGs[sgConfig]] );

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
    DWORD DeleteTitleFromMD( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that you can delete an entire title from the
        // XBox Memory Areas.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_TITLE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_TITLE_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_TITLES_DELETED );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // For each Title and saved game combination to be removed entirely from the XBox
            for( size_t varConfig = 0; varConfig < ARRAYSIZE( gc_lpwszDELETE_TITLE_SAVED_GAME_CONFIGS ); ++varConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Title to Remove: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], gc_lpwszDELETE_TITLE_SAVED_GAME_CONFIGS[varConfig] );

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
    DWORD DeleteTitleNavConfirmCancel( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user selects a Title to be deleted,
        // and then chooses "cancel", that they are navigated back to the
        // appropriate screen.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1
        };

        // Subset of the generic saved game selections we want to make
    enum DeleteTitleGameConfigsWithinList titleConfigs[] = {
            DELETE_2_TITLES_TITLE1_1_SAVED_GAME_TITLE2_1_SAVED_GAME_SELECT_TITLE1,
            DELETE_2_TITLES_TITLE1_1_SAVED_GAME_TITLE2_1_SAVED_GAME_SELECT_TITLE2,
            DELETE_3_TITLES_TITLE1_1_SAVED_GAME_TITLE2_1_SAVED_GAME_TITLE3_1_SAVED_GAME_SELECT_TITLE2
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_TITLE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_TITLE_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_CANCEL );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // For each generic saved game config in our subset
            for( size_t titleConfig = 0; titleConfig < ARRAYSIZE( titleConfigs ); ++titleConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Area: %s, Title to Select: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[currentMemArea]], gc_lpwszDELETE_TITLE_SAVED_GAME_CONFIGS[titleConfigs[titleConfig]] );

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
    DWORD HWInsertedDuringOperation( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if hardware is inserted during a saved game
        // operation (copy or delete), the program will continue as expected.
        //
        // NOTE: The hardware used in this case is NOT the hardware that's either
        // the source, or destiation Duke/Memory Area
        //////////////////////////////////////////////////////////////////////////

        // A sub-list of possible memory areas to copy to / from for this
        // test case
        CopyFromToMD MDCopyList[] = {
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_XDISK },
            { MEMORY_AREA_XDISK,     MEMORY_AREA_MU1_DUKE1 }
        };

        // List of ports to use where the hardware will be inserted
        WCHAR* portsToUse[] = {
            L"2"
        };

        // Hardware devices to be used for the insertion
        WCHAR* hardwareDevicesToUse[] = {
            (WCHAR*)validDukeAndMUConfigs[DUKE_E_E],
            (WCHAR*)gc_lpwszMEM_AREAS[MEMORY_AREA_MU1_DUKE2],
            (WCHAR*)gc_lpwszMEM_AREAS[MEMORY_AREA_MU2_DUKE1]
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_INITIATE_ACTION_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_INSERT_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );

        // For each memory area from/to combination available
        for( size_t currentCopy = 0; currentCopy < ARRAYSIZE( MDCopyList ); ++currentCopy )
        {
            // For each port we wish to use
            for( size_t port = 0; port < ARRAYSIZE( portsToUse ); ++port )
            {
                // for each piece of hardware to be inserted
                for( size_t hardware = 0; hardware < ARRAYSIZE( hardwareDevicesToUse ); ++hardware )
                {
                    // for each action we can perform on the saved game
                    for( size_t action = 0; action < ARRAYSIZE( gc_lpwszSAVED_GAME_ACTIONS ); ++action )
                    {
                        // for each generic saved game configuration that can be on a memory device
                        for( size_t gameConfig = 0; gameConfig < ARRAYSIZE( gc_lpwszGENERIC_SAVED_GAME_CONFIGS ); ++gameConfig )
                        {
                            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM %s to %s, Hardward to Insert: %s in Port %s, Action: %s, Saved Game Config: %s", xlogVariationNum, gc_lpwszMEM_AREAS[MDCopyList[currentCopy].FromMD], gc_lpwszMEM_AREAS[MDCopyList[currentCopy].ToMD], hardwareDevicesToUse[hardware], portsToUse[port], gc_lpwszSAVED_GAME_ACTIONS[action], gc_lpwszGENERIC_SAVED_GAME_CONFIGS[gameConfig] );

                            // Log the test case variation to our Steps File
                            pStepsLog->AddVariation( stepsVariation );

                            TESTCASE( xlogVariation )
                            {
                                TESTMNUL( hLog, "Manual Test" );
                            } ENDTESTCASE;
                        }
                    }
                }
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
    DWORD HWRemovedDuringOperation( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if hardware is removed during a saved game
        // operation (copy or delete), the program will continue as expected.
        //
        // NOTE: The hardware used in this case is NOT the hardware that's either
        // the source, or destiation Duke/Memory Area
        //////////////////////////////////////////////////////////////////////////

        // A sub-list of possible memory areas to copy to / from for this
        // test case
        CopyFromToMD MDCopyList[] = {
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_XDISK },
            { MEMORY_AREA_XDISK,     MEMORY_AREA_MU1_DUKE1 }
        };

        // List of ports to use where the hardware will be inserted
        WCHAR* portsToUse[] = {
            L"2"
        };

        WCHAR* hardwareDevicesToUse[] = {
            (WCHAR*)validDukeAndMUConfigs[DUKE_E_E],
            (WCHAR*)gc_lpwszMEM_AREAS[MEMORY_AREA_MU1_DUKE2],
            (WCHAR*)gc_lpwszMEM_AREAS[MEMORY_AREA_MU2_DUKE1]
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_INITIATE_ACTION_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );

        // For each memory area from/to combination available
        for( size_t currentCopy = 0; currentCopy < ARRAYSIZE( MDCopyList ); ++currentCopy )
        {
            // For each port we wish to use
            for( size_t port = 0; port < ARRAYSIZE( portsToUse ); ++port )
            {
                // for each piece of hardware to be inserted
                for( size_t hardware = 0; hardware < ARRAYSIZE( hardwareDevicesToUse ); ++hardware )
                {
                    // for each action we can perform on the saved game
                    for( size_t action = 0; action < ARRAYSIZE( gc_lpwszSAVED_GAME_ACTIONS ); ++action )
                    {
                        // for each generic saved game configuration that can be on a memory device
                        for( size_t gameConfig = 0; gameConfig < ARRAYSIZE( gc_lpwszGENERIC_SAVED_GAME_CONFIGS ); ++gameConfig )
                        {
                            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM %s to %s, Hardward to Remove: %s from Port %s, Action: %s, Saved Game Config: %s", xlogVariationNum, gc_lpwszMEM_AREAS[MDCopyList[currentCopy].FromMD], gc_lpwszMEM_AREAS[MDCopyList[currentCopy].ToMD], hardwareDevicesToUse[hardware], portsToUse[port], gc_lpwszSAVED_GAME_ACTIONS[action], gc_lpwszGENERIC_SAVED_GAME_CONFIGS[gameConfig] );

                            // Log the test case variation to our Steps File
                            pStepsLog->AddVariation( stepsVariation );

                            TESTCASE( xlogVariation )
                            {
                                TESTMNUL( hLog, "Manual Test" );
                            } ENDTESTCASE;
                        }
                    }
                }
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
    DWORD HWRemovedDuringOperationFail( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if hardware is removed during a saved game
        // operation (copy or delete), the program will continue as expected.
        //
        // It will also verify that the files that were being copied/deleted are
        // able to be cleaned up after the failure.
        //
        // NOTE: The hardware used in this case IS the hardware that's either
        // the source, or destiation Duke/Memory Area
        //////////////////////////////////////////////////////////////////////////

        // A sub-list of possible memory areas to copy to / from for this
        // test case
        CopyFromToMD MDCopyList[] = {
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_XDISK },
            { MEMORY_AREA_XDISK,     MEMORY_AREA_MU1_DUKE1 }
        };

        // List of ports to use where the hardware will be inserted
        WCHAR* portsToUse[] = {
            L"1"
        };

        // Hardware devices to be used for the insertion/removal
        WCHAR* hardwareDevicesToUse[] = {
            (WCHAR*)validDukeAndMUConfigs[DUKE_M_E],
            (WCHAR*)gc_lpwszMEM_AREAS[MEMORY_AREA_MU1_DUKE1]
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_INITIATE_ACTION_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_NOTIFICATION );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );
        pStepsLog->AddStep( STEP_INSERT_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEM_AREA_FILES_OK );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAMES_MARKED_BAD );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // For each memory area from/to combination available
        for( size_t currentCopy = 0; currentCopy < ARRAYSIZE( MDCopyList ); ++currentCopy )
        {
            // For each port we wish to use
            for( size_t port = 0; port < ARRAYSIZE( portsToUse ); ++port )
            {
                // for each piece of hardware to be inserted
                for( size_t hardware = 0; hardware < ARRAYSIZE( hardwareDevicesToUse ); ++hardware )
                {
                    // for each action we can perform on the saved game
                    for( size_t action = 0; action < ARRAYSIZE( gc_lpwszSAVED_GAME_ACTIONS ); ++action )
                    {
                        // for each generic saved game configuration that can be on a memory device
                        for( size_t gameConfig = 0; gameConfig < ARRAYSIZE( gc_lpwszGENERIC_SAVED_GAME_CONFIGS ); ++gameConfig )
                        {
                            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM %s to %s, Hardward to Remove: %s in Port %s, Action: %s, Saved Game Config: %s", xlogVariationNum, gc_lpwszMEM_AREAS[MDCopyList[currentCopy].FromMD], gc_lpwszMEM_AREAS[MDCopyList[currentCopy].ToMD], hardwareDevicesToUse[hardware], portsToUse[port], gc_lpwszSAVED_GAME_ACTIONS[action], gc_lpwszGENERIC_SAVED_GAME_CONFIGS[gameConfig] );

                            // Log the test case variation to our Steps File
                            pStepsLog->AddVariation( stepsVariation );

                            TESTCASE( xlogVariation )
                            {
                                TESTMNUL( hLog, "Manual Test" );
                            } ENDTESTCASE;
                        }
                    }
                }
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
    DWORD HWSourceRemovedBeforeCopy( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if hardware is removed during a saved game
        // copy operation, before the operation is initiated, the program will
        // continue as expected.
        //////////////////////////////////////////////////////////////////////////

        // A sub-list of possible memory areas to copy to / from for this
        // test case

        enum MemoryAreasWithinList MAToUse [] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_MU2_DUKE1,
            MEMORY_AREA_MU1_DUKE2,
            MEMORY_AREA_MU2_DUKE2,
            MEMORY_AREA_MU1_DUKE3,
            MEMORY_AREA_MU2_DUKE3,
            MEMORY_AREA_MU1_DUKE4,
            MEMORY_AREA_MU2_DUKE4
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_SOME_DATA );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_COPY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // For each memory area in our list
        for( size_t memArea = 0; memArea < ARRAYSIZE( MAToUse ); ++memArea )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Source/Hardware to Remove %s", xlogVariationNum, gc_lpwszMEM_AREAS[MAToUse[memArea]] );

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
    DWORD CopySingleSGFromMDToMD( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user copies a single saved game from 
        // one MD to another MD, it works as expected.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_DEST_MEM_AREA_ENOUGH_SPACE );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );

        // From each Memory Device (area)
        for( size_t fromMD = 0; fromMD < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++fromMD )
        {
            // To each Memory Device (area)
            for( size_t toMD = 0; toMD < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++toMD )
            {
                // If the "TO" memory area is the same as the "From", don't log/perform this test
                if( fromMD != toMD )
                {
                    // For each game variation
                    for( size_t gameVar = 0; gameVar < ARRAYSIZE( gc_lpwszSINGLE_SAVED_GAME_CONFIGS ); ++gameVar )
                    {
                        _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                        _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM %s to %s, Game Variation: %s", xlogVariationNum, gc_lpwszMEM_AREAS[fromMD], gc_lpwszMEM_AREAS[toMD], gc_lpwszSINGLE_SAVED_GAME_CONFIGS[gameVar] );

                        // Log the test case variation to our Steps File
                        pStepsLog->AddVariation( stepsVariation );

                        TESTCASE( xlogVariation )
                        {
                            TESTMNUL( hLog, "Manual Test" );
                        } ENDTESTCASE;
                    }
                }
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
    DWORD CopySingleSGFromMDToMDPowerOff( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user copies a single saved game from 
        // one MD to another MD, and they press the XBox power button, that the
        // copy operation is completed.
        //////////////////////////////////////////////////////////////////////////

        // A sub-list of possible memory areas to copy to / from for this
        // test case
        CopyFromToMD MDCopyList[] = {
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_XDISK },
            { MEMORY_AREA_XDISK,     MEMORY_AREA_MU1_DUKE1 }
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_IN_VAR_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_SETUP_DEST_MEM_AREA_ENOUGH_SPACE );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_STARTED );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );

        // For each memory area from/to combination available
        for( size_t currentCopy = 0; currentCopy < ARRAYSIZE( MDCopyList ); ++currentCopy )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: SOURCE MD %s, DEST MD: %s", xlogVariationNum, gc_lpwszMEM_AREAS[MDCopyList[currentCopy].FromMD], gc_lpwszMEM_AREAS[MDCopyList[currentCopy].ToMD] );

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
    DWORD CopySingleSGFromMDToMDPullsPlug( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user copies a single saved game from 
        // one MD to another MD, and they pull the plug on the XBox, that the
        // destination either doesn't contain the files, or the files that
        // were being copied are marked bad and can be deleted without harming
        // other files.
        //////////////////////////////////////////////////////////////////////////

        // A sub-list of possible memory areas to copy to / from for this
        // test case
        CopyFromToMD MDCopyList[] = {
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_XDISK },
            { MEMORY_AREA_XDISK,     MEMORY_AREA_MU1_DUKE1 }
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_IN_VAR_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_SETUP_DEST_MEM_AREA_ENOUGH_SPACE );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_STARTED );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_REMOVE_POWER_PLUG_FROM_XBOX );
        pStepsLog->AddStep( STEP_VERIFY_XBOX_POWERED_OFF );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEM_AREA_FILES_OK );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAMES_MARKED_BAD );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );

        // For each memory area from/to combination available
        for( size_t currentCopy = 0; currentCopy < ARRAYSIZE( MDCopyList ); ++currentCopy )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: SOURCE MD %s, DEST MD: %s", xlogVariationNum, gc_lpwszMEM_AREAS[MDCopyList[currentCopy].FromMD], gc_lpwszMEM_AREAS[MDCopyList[currentCopy].ToMD] );

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
    DWORD CopySingleSGAllUnitsFull( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user tries to copy a game, and all
        // Memory Areas are full, things work as expected.
        //////////////////////////////////////////////////////////////////////////

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DUKE_M_M );
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_UNITS_FULL );
        pStepsLog->AddStep( STEP_SETUP_XDISK_SOME_DATA );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_XDISK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_EACH_MU );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_NOT_ALLOWED );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );

        TESTCASE( "" )
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
    DWORD CopySingleSGAllUnitsFullHSFull( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user tries to copy a game, and all
        // Memory Areas are full, and they hot-swap an MU with another FULL unit,
        // that they are not allowed to select it.
        //////////////////////////////////////////////////////////////////////////

        // Memory Areas to swap out
        enum MemoryAreasWithinList MAToUse [] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_MU2_DUKE4
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DUKE_M_M );
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_UNITS_FULL );
        pStepsLog->AddStep( STEP_SETUP_XDISK_SOME_DATA );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_XDISK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_INSERT_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_NOT_ALLOWED );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );

        // For each Memory Unit Memory Area we wish to try
        for( size_t memArea = 0; memArea < ARRAYSIZE( MAToUse ); ++memArea )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM XDisk to %s, Remove FULL %s, Insert FULL %s", xlogVariationNum, gc_lpwszMEM_AREAS[MAToUse[memArea]], gc_lpwszMEM_AREAS[MAToUse[memArea]], gc_lpwszMEM_AREAS[MAToUse[memArea]] );

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
    DWORD CopySingleSGAllUnitsFullHSEmpty( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user tries to copy a game, and all
        // Memory Areas are full, and they hot-swap an MU that's empty, that they
        // are allowed to save the game to that MU
        //////////////////////////////////////////////////////////////////////////

        // Memory Areas to swap out
        enum MemoryAreasWithinList MAToUse [] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_MU2_DUKE4
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DUKE_M_M );
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_UNITS_FULL );
        pStepsLog->AddStep( STEP_SETUP_XDISK_SOME_DATA );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_XDISK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_INSERT_MEM_UNIT_EMPTY );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_EMPTY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );

        // For each Memory Unit Memory Area we wish to try
        for( size_t memArea = 0; memArea < ARRAYSIZE( MAToUse ); ++memArea )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM XDisk to %s, Remove FULL %s, Insert Empty (DEST) %s", xlogVariationNum, gc_lpwszMEM_AREAS[MAToUse[memArea]], gc_lpwszMEM_AREAS[MAToUse[memArea]], gc_lpwszMEM_AREAS[MAToUse[memArea]] );

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
    DWORD CopySingleSGHSFullEmptyFull( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user tries to copy a game, and all
        // Memory Areas are full, and they hot swap an empty MU, then HS a full
        // MU in it's place, that they aren't allowed to use that MU
        //////////////////////////////////////////////////////////////////////////

        // Memory Areas to swap out
        enum MemoryAreasWithinList MAToUse [] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_MU2_DUKE4
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DUKE_M_M );
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_UNITS_FULL );
        pStepsLog->AddStep( STEP_SETUP_XDISK_SOME_DATA );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_XDISK );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_INSERT_MEM_UNIT_EMPTY );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_INSERT_MEM_UNIT_FULL );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_NOT_ALLOWED );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );

        // For each Memory Unit Memory Area we wish to try
        for( size_t memArea = 0; memArea < ARRAYSIZE( MAToUse ); ++memArea )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM XDisk to %s, Remove FULL %s, Insert Empty (DEST) %s", xlogVariationNum, gc_lpwszMEM_AREAS[MAToUse[memArea]], gc_lpwszMEM_AREAS[MAToUse[memArea]], gc_lpwszMEM_AREAS[MAToUse[memArea]] );

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
    DWORD CopyMultipleSGsFromMDToMD( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user copies multiple saved games from 
        // one MD to another MD, it works as expected.
        //////////////////////////////////////////////////////////////////////////

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_DEST_MEM_AREA_ENOUGH_SPACE );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );

        // From each Memory Device (area)
        for( size_t fromMD = 0; fromMD < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++fromMD )
        {
            // To each Memory Device (area)
            for( size_t toMD = 0; toMD < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++toMD )
            {
                // If the "TO" memory area is the same as the "From", don't log/perform this test
                if( fromMD != toMD )
                {
                    // For each game variation
                    for( size_t gameVar = 0; gameVar < ARRAYSIZE( gc_lpwszMULTIPLE_SAVED_GAME_CONFIGS ); ++gameVar )
                    {
                        _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                        _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM %s to %s, Game Variation: %s", xlogVariationNum, gc_lpwszMEM_AREAS[fromMD], gc_lpwszMEM_AREAS[toMD], gc_lpwszMULTIPLE_SAVED_GAME_CONFIGS[gameVar] );

                        // Log the test case variation to our Steps File
                        pStepsLog->AddVariation( stepsVariation );

                        TESTCASE( xlogVariation )
                        {
                            TESTMNUL( hLog, "Manual Test" );
                        } ENDTESTCASE;
                    }
                }
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
    DWORD CopySingleSGFileVerification( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user copies a single saved game from 
        // one type of MD to each other type of MD, it works as expected, and
        // the file is inspected for correctness.
        //////////////////////////////////////////////////////////////////////////

        // A sub-list of possible memory areas to copy to / from for this
        // test case
        CopyFromToMD MDCopyList[] = {
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_MU1_DUKE2 },
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_XDISK },
            { MEMORY_AREA_XDISK,     MEMORY_AREA_MU1_DUKE2 }
        };


        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_DEST_MEM_AREA_ENOUGH_SPACE );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );
        pStepsLog->AddStep( STEP_PERFORM_FILE_COMPARE_ACCORD_TO_VAR );

        // For each memory area from/to combination available
        for( size_t currentCopy = 0; currentCopy < ARRAYSIZE( MDCopyList ); ++currentCopy )
        {
            // For each type of saved game possible
            for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM %s to %s, Game Variation: 1 saved game - %s", xlogVariationNum, gc_lpwszMEM_AREAS[MDCopyList[currentCopy].FromMD], gc_lpwszMEM_AREAS[MDCopyList[currentCopy].ToMD], gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );

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
    DWORD CopyMultipleSGsFileVerification( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user copies multiple saved games from 
        // one type of MD to each other type of MD, it works as expected, and
        // the file is inspected for correctness.
        //////////////////////////////////////////////////////////////////////////

        // A sub-list of possible memory areas to copy to / from for this
        // test case
        CopyFromToMD MDCopyList[] = {
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_MU1_DUKE2 },
            { MEMORY_AREA_MU1_DUKE1, MEMORY_AREA_XDISK },
            { MEMORY_AREA_XDISK,     MEMORY_AREA_MU1_DUKE2 }
        };


        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_SETUP_DEST_MEM_AREA_ENOUGH_SPACE );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_COMPLETED );
        pStepsLog->AddStep( STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SELECTION );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_CORRECT_FILES_EXIST );
        pStepsLog->AddStep( STEP_PERFORM_FILE_COMPARE_ACCORD_TO_VAR );

        // For each memory area from/to combination available
        for( size_t currentCopy = 0; currentCopy < ARRAYSIZE( MDCopyList ); ++currentCopy )
        {
            // For each type of saved game possible
            for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: FROM %s to %s, Game Variation: 2+ saved games - %s", xlogVariationNum, gc_lpwszMEM_AREAS[MDCopyList[currentCopy].FromMD], gc_lpwszMEM_AREAS[MDCopyList[currentCopy].ToMD], gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );

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
    DWORD MemCopyDukeMUVarsDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Copy main screen displays
        // properly with variations of Dukes and MUs.
        //
        // It also ensures that Dukes and MUs can be Hot-Plugged on this screen,
        // and it will behave as expected.
        //////////////////////////////////////////////////////////////////////////

        size_t testVariationNum = 0;                        // The Variation Number of the test we are running
        WCHAR portConfiguration[VARIATION_BUFFER_SIZE];     // The String representation of the XBox Port Configuration we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];         // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        for (size_t port1 = 0; port1 < ARRAYSIZE(validDukeAndMUConfigs); port1++)
        {
            for (size_t port2 = 0; port2 < ARRAYSIZE(validDukeAndMUConfigs); port2++)
            {
                for (size_t port3 = 0; port3 < ARRAYSIZE(validDukeAndMUConfigs); port3++)
                {
                    for (size_t port4 = 0; port4 < ARRAYSIZE(validDukeAndMUConfigs); port4++)
                    {
                        _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
                        _snwprintf( portConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: %s, %s, %s, %s", testVariationNum, validDukeAndMUConfigs[port1], validDukeAndMUConfigs[port2], validDukeAndMUConfigs[port3], validDukeAndMUConfigs[port4]);
  
                        // Log the test case variation to our Steps File
                        pStepsLog->AddVariation( portConfiguration );

                        TESTCASE( testVariation )
                        {
                            TESTMNUL( hLog, "Manual Test" );
                        } ENDTESTCASE;
                    }
                }
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
    DWORD MemCopyDukeHotPlugVarsDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Copy main screen displays
        // properly with variations of Dukes and MUs.
        //
        // It also ensures that Dukes can be Hot-Plugged on this screen,
        // and it will behave as expected.
        //////////////////////////////////////////////////////////////////////////

        enum PortConfigurationsWithinList dukeSetup[] = {
            DUKE_E_E,
            DUKE_M_E,
            DUKE_E_M,
            DUKE_M_M
        };

        size_t testVariationNum = 0;                        // The Variation Number of the test we are running
        WCHAR portConfiguration[VARIATION_BUFFER_SIZE];     // The String representation of the XBox Port Configuration we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];         // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_DEFAULT_DUKE_SOME_DATA );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_DEFAULT_DUKE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_COPY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SKIP_VARS_BASED_ON_DEFAULT_DUKE );
        pStepsLog->AddStep( STEP_INSERT_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // Plug in Dukes (all ports) that have both MA slots used
        _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
        _snwprintf( portConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: ALL XBox Ports, Duke Variation: %s", testVariationNum, validDukeAndMUConfigs[DUKE_M_M] );

        // Log the test case variation to our Steps File
        pStepsLog->AddVariation( portConfiguration );

        TESTCASE( testVariation )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;


        // Plug in Dukes (all ports) that are empty (No MA slots used)
        _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
        _snwprintf( portConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: ALL XBox Ports, Duke Variation: %s", testVariationNum, validDukeAndMUConfigs[DUKE_E_E] );

        // Log the test case variation to our Steps File
        pStepsLog->AddVariation( portConfiguration );

        TESTCASE( testVariation )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;

    
        // For each individual port on the XBox
        for ( size_t xboxPort = 1; xboxPort <= gc_nNUM_XBOX_PORTS; ++xboxPort )
        {
            // For each Duke Variations available to a port
            for ( size_t dukeVariation = 0; dukeVariation < ARRAYSIZE( dukeSetup ); ++dukeVariation )
            {
                _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
                _snwprintf( portConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: XBox Port %d, Duke Variation: %s", testVariationNum, xboxPort, validDukeAndMUConfigs[dukeSetup[dukeVariation]] );
  
                // Log the test case variation to our Steps File
                pStepsLog->AddVariation( portConfiguration );

                TESTCASE( testVariation )
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
    DWORD MemCopyMUHotPlugVarsDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Copy main screen displays
        // properly with variations of MUs within Dukes.
        //
        // It also ensures that MUs can be Hot-Plugged on this screen,
        // and it will behave as expected.
        //////////////////////////////////////////////////////////////////////////

        size_t testVariationNum = 0;                        // The Variation Number of the test we are running
        WCHAR portConfiguration[VARIATION_BUFFER_SIZE];     // The String representation of the XBox Port Configuration we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];         // The String representation holding the variation number we are running

        // If the user wishes to print the steps, let's log them
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_DEFAULT_DUKE_SOME_DATA );
        pStepsLog->AddStep( STEP_SETUP_XBOX_PORTS_DUKE_E_E );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_DEFAULT_DUKE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_COPY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SKIP_VARS_BASED_ON_DEFAULT_DUKE );
        pStepsLog->AddStep( STEP_INSERT_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // Plug in MU's (all Dukes, all ports) that have both MA slots used
        _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
        _snwprintf( portConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Insert MU's in to EACH Duke slot on ALL Dukes", testVariationNum );

        // Log the test case variation to our Steps File
        pStepsLog->AddVariation( portConfiguration );

        TESTCASE( testVariation )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;

        // For each individual port on the XBox
        for( size_t xboxPort = 1; xboxPort <= gc_nNUM_XBOX_PORTS; ++xboxPort )
        {
            // For each Duke slot in each Duke
            for( size_t dukeSlot = 1; dukeSlot <= gc_nNUM_DUKE_SLOTS; ++dukeSlot )
            {
                _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
                _snwprintf( portConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Memory Unit in Duke slot %d of the Duke in Port %d", testVariationNum, dukeSlot, xboxPort );
  
                // Log the test case variation to our Steps File
                pStepsLog->AddVariation( portConfiguration );

                TESTCASE( testVariation )
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
    DWORD MemCopyMDVariousSizesDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Copy main screen displays
        // properly with Memory Area that are filled with various amounts of
        // data.  It verifies that the information displayed when you
        // "highlight" on of these areas is displayed correctly.
        //////////////////////////////////////////////////////////////////////////

        size_t testVariationNum = 0;                        // The Variation Number of the test we are running
        WCHAR deviceConfiguration[VARIATION_BUFFER_SIZE];   // The String representation of the XBox Device Configuration we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];         // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_DEFAULT_DUKE_SOME_DATA );
        pStepsLog->AddStep( STEP_SKIP_VARS_BASED_ON_DEFAULT_DUKE );
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_DEFAULT_DUKE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_COPY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );

        // Xdisk verified with each MD block configuration
        for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
        {
            _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
            _snwprintf( deviceConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Xdisk, MemArea Block Configuration: %s", testVariationNum, gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( deviceConfiguration );

            TESTCASE( testVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }


        // All Dukes in all ports, and each port on each Duke tested with the MD block configurations
        for( size_t port = 1; port <= gc_nNUM_XBOX_PORTS; ++port )
        {
            for( size_t dukeSlot = 1; dukeSlot <= gc_nNUM_DUKE_SLOTS; ++dukeSlot )
            {
                for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
                {
                    _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
                    _snwprintf( deviceConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Port: %d, Duke slot: %d, MemArea Block Configuration: %s", testVariationNum, port, dukeSlot, gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );
  
                    // Log the test case variation to our Steps File
                    pStepsLog->AddVariation( deviceConfiguration );

                    TESTCASE( testVariation )
                    {
                        TESTMNUL( hLog, "Manual Test" );
                    } ENDTESTCASE;
                }
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
    DWORD MemCopyDestMUNotEnoughRoom( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This case verifies that the Memory Copy main screen displays
        // a Memory Area that does not have enough space to hold the saved
        // game being copied is displayed (marked) properly, and that that
        // Memory Area cannot be selected.
        //////////////////////////////////////////////////////////////////////////

        size_t testVariationNum = 0;                        // The Variation Number of the test we are running
        WCHAR deviceConfiguration[VARIATION_BUFFER_SIZE];   // The String representation of the XBox Device Configuration we'll be using
        WCHAR testVariation[VARIATION_BUFFER_SIZE];         // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SKIP_VARS_BASED_ON_DEFAULT_DUKE );
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_DEFAULT_DUKE_SOME_DATA );
        pStepsLog->AddStep( STEP_SETUP_DEST_MEM_AREA_NOT_ENOUGH_SPACE );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_DEFAULT_DUKE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_COPY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_DEST_MEM_AREA_MARKED_INVALID );
        pStepsLog->AddStep( STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_ACTION_NOT_ALLOWED );

        // For each Memory Area available on the XBox
        for( size_t memArea = 0; memArea < ARRAYSIZE( gc_lpwszMEM_AREAS ); ++memArea )
        {
            _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
            _snwprintf( deviceConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Destination Memory Area: %s", testVariationNum, gc_lpwszMEM_AREAS[memArea] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( deviceConfiguration );

            TESTCASE( testVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }

        // Xdisk verified with each MD block configuration
        for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
        {
            _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
            _snwprintf( deviceConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Xdisk, MemArea Block Configuration: %s", testVariationNum, gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );

            // Log the test case variation to our Steps File
            pStepsLog->AddVariation( deviceConfiguration );

            TESTCASE( testVariation )
            {
                TESTMNUL( hLog, "Manual Test" );
            } ENDTESTCASE;
        }


        // All Dukes in all ports, and each port on each Duke tested with the MD block configurations
        for( size_t port = 1; port <= gc_nNUM_XBOX_PORTS; ++port )
        {
            for( size_t dukeSlot = 1; dukeSlot <= gc_nNUM_DUKE_SLOTS; ++dukeSlot )
            {
                for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( gc_lpwszMEM_AREA_BLOCK_CONFIGS ); ++blockConfig )
                {
                    _snwprintf( testVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++testVariationNum );
                    _snwprintf( deviceConfiguration, VARIATION_BUFFER_SIZE, L"Variation %d: Port: %d, Duke slot: %d, MemArea Block Configuration: %s", testVariationNum, port, dukeSlot, gc_lpwszMEM_AREA_BLOCK_CONFIGS[blockConfig] );
  
                    // Log the test case variation to our Steps File
                    pStepsLog->AddVariation( deviceConfiguration );

                    TESTCASE( testVariation )
                    {
                        TESTMNUL( hLog, "Manual Test" );
                    } ENDTESTCASE;
                }
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
    DWORD MemCopyTitleSingleSGDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user selects a single saved game
        // and tries to copy it, that the title of the "XBox saved game copy"
        // screen is displayed appropriately.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_IN_VAR_TWO_SAVED_GAMES );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_COPY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_TITLE_FROM_PARENT );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING);
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED);

        // For each memory area available
        for( size_t currentMemArea=0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // SAVED GAME META: Variations in the name of the game that a saved game is from (Parent game)
            for( size_t sgParentName = 0; sgParentName < ARRAYSIZE( gc_lpwszMETA_SAVED_GAME_PARENT_NAME_CONFIGS ); ++sgParentName )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Select a SINGLE game from Memory Area %s, Saved Game's Parent Name: %s", xlogVariationNum, gc_lpwszMEM_AREAS[currentMemArea], gc_lpwszMETA_SAVED_GAME_PARENT_NAME_CONFIGS[sgParentName] );

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
    DWORD MemCopyTitleMultipleSGDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a user selects multiple saved games
        // that the title of the "XBox saved game copy" screen is displayed
        // appropriately.
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_XDISK
        };

        // Subset of the generic saved game selections we want to make
        enum SavedGamesGenericWithinList genSGs[] = {
            SAVED_GAMES_MULTIPLE,
            SAVED_GAMES_ALL
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_IN_VAR_MAX_SAVED_GAMES );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_COPY );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_SAVED_GAME_COPY_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_TITLE_CORRECT_NUM_GAMES_SELECTED );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING);
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED);

        // For each memory area available
        for( size_t currentMemArea = 0; currentMemArea < ARRAYSIZE( memDevices ); ++currentMemArea )
        {
            // For each generic saved game config in our subset
            for( size_t sgConfig = 0; sgConfig < ARRAYSIZE( genSGs ); ++sgConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Select games from Memory Area: %s, Saved Games to Select: %s", xlogVariationNum, gc_lpwszMEM_AREAS[currentMemArea], gc_lpwszGENERIC_SAVED_GAME_CONFIGS[genSGs[sgConfig]] );

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
    DWORD BootToMemCleanUpMAFullDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a title boots the user in to memory
        // management clean-up mode, the display is OK when the title requests
        // various amounts of space free when all Mem Areas are empty.
        //////////////////////////////////////////////////////////////////////////

        // Memory area block configurations to use for this test case
        enum MemoryAreaBlockConfigsWithinList BlockConfigsToUse[] = {
            MEMORY_BLOCK_CONFIG_1_BLOCK,
            MEMORY_BLOCK_CONFIG_HALF_BLOCKS,
            MEMORY_BLOCK_CONFIG_MAX_MINUS_1_BLOCKS,
            MEMORY_BLOCK_CONFIG_MAX__BLOCKS,
            MEMORY_BLOCK_CONFIG_MAX_PLUS_1_BLOCKS
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_AREAS_FULL );
        pStepsLog->AddStep( STEP_INSERT_DVD_GAME_MEM_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_BLOCK_CONFIG_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING);
        pStepsLog->AddStep( STEP_VERIFY_TEXT_NOT_CLIPPED);

        // For each memory block configuration available
        for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( BlockConfigsToUse ); ++blockConfig )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Title Should Request: %s", xlogVariationNum, gc_lpwszMEM_AREA_BLOCK_CONFIGS[BlockConfigsToUse[blockConfig]] );

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
    DWORD BootToMemCleanUpMAEmptyDisplay( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a title boots the user in to memory
        // management clean-up mode, and specifies any amount of space needed
        // when all memory areas are empty, that they are NOT placed in to
        // Memory Management clean-up mode.
        //////////////////////////////////////////////////////////////////////////

        // Memory area block configurations to use for this test case
        enum MemoryAreaBlockConfigsWithinList BlockConfigsToUse[] = {
            MEMORY_BLOCK_CONFIG_0_BLOCKS,
            MEMORY_BLOCK_CONFIG_1_BLOCK,
            MEMORY_BLOCK_CONFIG_MAX_MINUS_1_BLOCKS,
            MEMORY_BLOCK_CONFIG_MAX__BLOCKS
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_AREAS_EMPTY );
        pStepsLog->AddStep( STEP_INSERT_DVD_GAME_MEM_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_BLOCK_CONFIG_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP );

        // For each memory block configuration available
        for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( BlockConfigsToUse ); ++blockConfig )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Title Should Request: %s", xlogVariationNum, gc_lpwszMEM_AREA_BLOCK_CONFIGS[BlockConfigsToUse[blockConfig]] );

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
    DWORD BootToMemCleanUpMAFull0blocks( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a title boots the user in to memory
        // management clean-up mode and requests 0 blocks of space, and all
        // memory areas are FULL, that they are NOT placed in to memory
        // managment clean-up mode.
        //////////////////////////////////////////////////////////////////////////

        // Memory area block configurations to use for this test case
        enum MemoryAreaBlockConfigsWithinList BlockConfigsToUse[] = {
            MEMORY_BLOCK_CONFIG_0_BLOCKS
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_AREAS_FULL );
        pStepsLog->AddStep( STEP_INSERT_DVD_GAME_MEM_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_BLOCK_CONFIG_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP );

        // For each memory block configuration available
        for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( BlockConfigsToUse ); ++blockConfig )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: Title Should Request: %s", xlogVariationNum, gc_lpwszMEM_AREA_BLOCK_CONFIGS[BlockConfigsToUse[blockConfig]] );

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
    DWORD BootToMemCleanUpMAFullDeleteAll( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a title boots the user in to memory
        // management clean-up mode and requests x blocks of space, all
        // memory areas are FULL, and the user deletes all files off the memory
        // area, that they are taken out of clean-up mode
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        // Memory area block configurations to use for this test case
        enum MemoryAreaBlockConfigsWithinList BlockConfigsToUse[] = {
            MEMORY_BLOCK_CONFIG_1_BLOCK,
            MEMORY_BLOCK_CONFIG_HALF_BLOCKS,
            MEMORY_BLOCK_CONFIG_MAX_MINUS_1_BLOCKS,
            MEMORY_BLOCK_CONFIG_MAX__BLOCKS,
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_AREAS_FULL );
        pStepsLog->AddStep( STEP_INSERT_DVD_GAME_MEM_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_SELECT_MEM_AREA_BLOCK_CONFIG_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME_ALL );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP );

        // For each memory device we wish to try
        for( size_t memDevice = 0; memDevice < ARRAYSIZE( memDevices ); ++memDevice )
        {
            // For each memory block configuration available
            for( size_t blockConfig = 0; blockConfig < ARRAYSIZE( BlockConfigsToUse ); ++blockConfig )
            {
                _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
                _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: TITLE SHOULD REQUEST: %s, MEM AREA TO USE: %s", xlogVariationNum, gc_lpwszMEM_AREA_BLOCK_CONFIGS[BlockConfigsToUse[blockConfig]], gc_lpwszMEM_AREAS[memDevices[memDevice]] );

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
    DWORD BootToMemCleanUpClearSpace( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a title boots the user in to memory
        // management clean-up mode and requests x blocks of space, the user
        // can delete some files and the XDash will take them OUT of memory
        // management clean-up mode.  (The user will first delete a file and
        // NOT be taken out of clean up mode, and then then will delete another
        // file, and WILL be taken out of clean up mode)
        //////////////////////////////////////////////////////////////////////////

        // Used to setup the variations for this test
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_XDISK
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_MEM_AREA_MAX_SAVED_GAMES_1_BLOCK_EACH );
        pStepsLog->AddStep( STEP_INSERT_DVD_GAME_MEM_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_TITLE_SHOULD_REQUEST_MAX_MINUS_2_BLOCKS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_SELECT_DEVICE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_SAVED_GAME );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_OPTIONS_DIALOG );
        pStepsLog->AddStep( STEP_SELECT_DELETE );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_GAME_DELETE_DIALOG );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_DEVICE_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_SCREEN_UPDATED );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP );

        // For each memory device we wish to try
        for( size_t memDevice = 0; memDevice < ARRAYSIZE( memDevices ); ++memDevice )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: MEM AREA TO USE: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[memDevice]] );

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
    DWORD BootToMemCleanUpHotSwapMA( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a title boots the user in to memory
        // management clean-up mode and all memory areas are full, the user can
        // hot-swap an empty MU and the XDash will take the user out of
        // clean-up mode.
        //////////////////////////////////////////////////////////////////////////

        // The MU to Hot-Swap
        enum MemoryAreasWithinList memDevices[] = {
            MEMORY_AREA_MU1_DUKE1,
            MEMORY_AREA_MU2_DUKE1,
            MEMORY_AREA_MU1_DUKE2,
            MEMORY_AREA_MU2_DUKE2,
            MEMORY_AREA_MU1_DUKE3,
            MEMORY_AREA_MU2_DUKE3,
            MEMORY_AREA_MU1_DUKE4,
            MEMORY_AREA_MU2_DUKE4
        };

        size_t xlogVariationNum = 0;                    // The Variation Number of the test we are running
        WCHAR stepsVariation[VARIATION_BUFFER_SIZE];    // The String representation of the test variation we want logged to our steps file
        WCHAR xlogVariation[VARIATION_BUFFER_SIZE];     // The String representation holding the variation number we are running

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_AREAS_FULL );
        pStepsLog->AddStep( STEP_INSERT_DVD_GAME_MEM_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_TITLE_SHOULD_REQUEST_MAXBLOCKS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_INSERT_MEM_UNIT_1_SAVED_GAME_1_BLOCK );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_REMOVE_HARDWARE_ACCORD_TO_VAR );
        pStepsLog->AddStep( STEP_INSERT_MEM_UNIT_EMPTY );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP );

        // For each memory device we wish to try
        for( size_t memDevice = 0; memDevice < ARRAYSIZE( memDevices ); ++memDevice )
        {
            _snwprintf( xlogVariation, VARIATION_BUFFER_SIZE, L"Variation %d", ++xlogVariationNum );
            _snwprintf( stepsVariation, VARIATION_BUFFER_SIZE, L"Variation %d: MEM UNIT TO USE: %s", xlogVariationNum, gc_lpwszMEM_AREAS[memDevices[memDevice]] );

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
    DWORD BootToMemCleanUpNavAway( HANDLE hLog, CStepsLog* pStepsLog )
    {
        //////////////////////////////////////////////////////////////////////////
        // This test will verify that if a title boots the user in to memory
        // management clean-up mode, and they navigate AWAY from the memory
        // management area, that they are taken OUT of memory management
        // clean up mode.
        //////////////////////////////////////////////////////////////////////////

        // Setup the steps for this case
        pStepsLog->AddStep( STEP_SETUP_ALL_MEM_AREAS_FULL );
        pStepsLog->AddStep( STEP_INSERT_DVD_GAME_MEM_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_POWER );
        pStepsLog->AddStep( STEP_TITLE_SHOULD_REQUEST_MAXBLOCKS );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP );
        pStepsLog->AddStep( STEP_PRESS_BACK );
        pStepsLog->AddStep( STEP_VERIFY_ROOT_MENU_SCREEN );
        pStepsLog->AddStep( STEP_SELECT_MEMORY_MANAGEMENT );
        pStepsLog->AddStep( STEP_PRESS_ACCEPT );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN );
        pStepsLog->AddStep( STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP );

        TESTCASE( L"" )
        {
            TESTMNUL( hLog, "Manual Test" );
        } ENDTESTCASE;

        return 0;
    };
}; // namespace