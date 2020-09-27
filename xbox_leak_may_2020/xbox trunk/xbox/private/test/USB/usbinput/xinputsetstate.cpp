/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XInputSetState.cpp

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
DWORD USBInput::XInputSetState_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DWORD
    XInputSetState(
        IN HANDLE hDevice,
        IN OUT PXINPUT_FEEDBACK pFeedback
        );
    */
    xSetFunctionName(hLog, "XInputSetState");

    HANDLE device;
    DWORD err;
    XINPUT_FEEDBACK state;
    XINPUT_FEEDBACK *states;
    XINPUT_POLLING_PARAMETERS pollingValid = {1, 0, 0, 8, 8, 0};

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
        err = XInputSetState(0, NULL);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputSetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputSetState (ec: %ld)", err);
        } ENDTESTCASE;
    TESTCASE("INVALID_HANDLE_VALUE NULL")
        {
        BUGGED(0, "Crashes on NULL");
        err = XInputSetState(INVALID_HANDLE_VALUE, NULL);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputSetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputSetState (ec: %ld)", err);
        } ENDTESTCASE;
    TESTCASE("valid NULL")
        {
        BUGGED(0, "Crashes on NULL");
        err = XInputSetState(device, NULL);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputSetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputSetState (ec: %ld)", err);
        } ENDTESTCASE;
        */


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("GAMEPAD port0")
        {
        state.Header.hEvent = NULL;
        state.Rumble.wLeftMotorSpeed = 100;
        state.Rumble.wRightMotorSpeed = 100;
        err = XInputSetState(device, &state);
        if(err == ERROR_IO_PENDING)
            TESTPASS(hLog, "XInputSetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputSetState (ec: %ld)", err);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("INVALID_HANDLE_VALUE")
        {
        err = XInputSetState(INVALID_HANDLE_VALUE, &state);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputSetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputSetState (ec: %ld)", err);
        } ENDTESTCASE;
    TESTCASE("0x7777")
        {
        err = XInputSetState((HANDLE)0x7777, &state);
        if(err != ERROR_SUCCESS)
            TESTPASS(hLog, "XInputSetState (ec: %ld)", err);
        else
            TESTFAIL(hLog, "XInputSetState (ec: %ld)", err);
        } ENDTESTCASE;
        */


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        DebugPrint("USBINPUT(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxStress = 100000;
        states = new XINPUT_FEEDBACK[maxStress];
        if(!states) 
            {
            BLOCKED(0, "Memory alloction failed");
            break;
            }

        unsigned failures = 0;
        for(unsigned i=0; i<maxStress; i++)
            {
            states[i].Header.hEvent = NULL;
            states[i].Rumble.wLeftMotorSpeed = (WORD)maxStress;
            states[i].Rumble.wRightMotorSpeed = (WORD)maxStress;
            if(i%(maxStress/10) == 0)
                {
                DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }
            err = XInputSetState(device, &states[i]);
            if(err != ERROR_IO_PENDING) ++failures;
            }
        if(failures)
            TESTFAIL(hLog, "XInputSetState mini stress failures: %u", failures);

        i = 0;
        while(states[maxStress-1].Header.dwStatus == ERROR_IO_PENDING)
            {
            SleepEx(1000, TRUE);
            if((++i%10) == 0)
                {
                DebugPrint("   (%d) Waiting for IO to complete...\n", ThreadID);
                }
            }
        } ENDTESTCASE_CLEAN({ delete[] states; /*DebugPrint("   (%d) 100%%\n", ThreadID);*/ });


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
