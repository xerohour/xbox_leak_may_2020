/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XInputGetState.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    08-17-2000  Created

Notes:

*/

#include "stdafx.h"
#include <xbox.h>
#include <xlog.h>
#include "commontest.h"
#include "usbInput.h"


/*

Routine Description:

    

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:


Notes:
    "winerror.h"

*/
DWORD USBInput::XInputGetState_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DWORD
    XInputGetState(
        IN HANDLE hDevice,
        OUT PXINPUT_STATE  pState
        );
    */
    xSetFunctionName(hLog, "XInputGetState");

    HANDLE device;
    DWORD err;
    XINPUT_STATE state;
    XINPUT_POLLING_PARAMETERS pollingValid;
    pollingValid.fAutoPoll = 1;
    pollingValid.fInterruptOut = 0;
    pollingValid.ReservedMBZ1 = 0;
    pollingValid.bInputInterval = 8;  
    pollingValid.bOutputInterval = 8;
    pollingValid.ReservedMBZ2 = 0;

    DWORD time = GetTickCount();

    device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, &pollingValid);
    if(device == NULL)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to open device in port 0 is one plugged in? All tests skipped! (ec: %lu)", GetLastError());
        xEndVariation(hLog);
        return -1;
        }


    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("0 NULL")
        {
        BUGGED(0, "Crashes on NULL");
        err = XInputGetState(0, NULL);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputGetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputGetState (ec: %ld)", err);
        } ENDTESTCASE;
    TESTCASE("INVALID_HANDLE_VALUE NULL")
        {
        BUGGED(0, "Crashes on NULL");
        err = XInputGetState(INVALID_HANDLE_VALUE, NULL);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputGetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputGetState (ec: %ld)", err);
        } ENDTESTCASE;
    TESTCASE("valid NULL")
        {
        BUGGED(0, "Crashes on NULL");
        err = XInputGetState(device, NULL);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputGetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputGetState (ec: %ld)", err);
        } ENDTESTCASE;
        */


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("GAMEPAD port0")
        {
        err = XInputGetState(device, &state);
        if(err == ERROR_SUCCESS)
            TESTPASS(hLog, "XInputGetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputGetState (ec: %ld)", err);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("INVALID_HANDLE_VALUE")
        {
        err = XInputGetState(INVALID_HANDLE_VALUE, &state);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputGetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputGetState (ec: %ld)", err);
        } ENDTESTCASE;
    TESTCASE("0x7777")
        {
        err = XInputGetState((HANDLE)0x7777, &state);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputGetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputGetState (ec: %ld)", err);
        } ENDTESTCASE;
        */


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        //DebugPrint("USBINPUT(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxStress = 100000;
        unsigned failures = 0;
        for(unsigned i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0)
                {
                //DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }
            err = XInputGetState(device, &state);
            if(err != ERROR_SUCCESS) ++failures;
            }
        if(failures)
            TESTFAIL(hLog, "XInputGetState mini stress failures: %u", failures);
        } ENDTESTCASE_CLEAN({ /*DebugPrint("   (%d) 100%%\n", ThreadID);*/ });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Cleanup
    //////////////////////////////////////////////////////////////////////////
    XInputClose(device);

    return 0;
    }
