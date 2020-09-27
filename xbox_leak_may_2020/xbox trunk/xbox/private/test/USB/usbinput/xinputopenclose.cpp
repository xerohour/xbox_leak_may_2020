/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    XInputOpenClose.cpp

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
DWORD USBInput::XInputOpenClose_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HANDLE
    XInputOpen(
        IN DWORD dwType,
        IN DWORD dwPort,
        IN DWORD dwSlot,
        IN PXINPUT_POLLING_PARAMETERS pPollingParameters OPTIONAL
        );

    VOID
    XInputClose(
        IN HANDLE hDevice
        );
    */
    xSetFunctionName(hLog, "XInputOpen");

    HANDLE device;
    DWORD time = GetTickCount();

    XINPUT_POLLING_PARAMETERS pollingValid;
    pollingValid.fAutoPoll = 1;
    pollingValid.fInterruptOut = 0;
    pollingValid.ReservedMBZ1 = 0;
    pollingValid.bInputInterval = 8;  
    pollingValid.bOutputInterval = 8;
    pollingValid.ReservedMBZ2 = 0;

    XINPUT_POLLING_PARAMETERS pollingFast = { 1, 0, 0, 1, 1, 0 };
    XINPUT_POLLING_PARAMETERS pollingSlow = { 1, 0, 0, 255, 255, 0 };
    XINPUT_POLLING_PARAMETERS pollingMed  = { 1, 0, 0, 128, 128, 0 };
    XINPUT_POLLING_PARAMETERS pollingManual  = { 0, 0, 0, 4, 4, 0 };
    XINPUT_POLLING_PARAMETERS pollingInterrupt = { 0, 1, 0, 4, 4, 0 };


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("GAMEPAD port0 slot0 NULL")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, NULL);
        if(device != NULL)
            {
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
            XInputClose(device);
            }
        else
            TESTFAIL(hLog, "Unable to open device is one plugged in? (ec: %lu)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port0 slot0 valid polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, &pollingValid);
        if(device != NULL)
            {
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
            XInputClose(device);
            }
        else
            TESTFAIL(hLog, "Unable to open device (ec: %lu)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port0 slot0 slow polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, &pollingSlow);
        if(device != NULL)
            {
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
            XInputClose(device);
            }
        else
            TESTFAIL(hLog, "Unable to open device (ec: %lu)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port0 slot0 medium polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, &pollingMed);
        if(device != NULL)
            {
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
            XInputClose(device);
            }
        else
            TESTFAIL(hLog, "Unable to open device (ec: %lu)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port0 slot0 fast polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, &pollingFast);
        if(device != NULL)
            {
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
            XInputClose(device);
            }
        else
            TESTFAIL(hLog, "Unable to open device (ec: %lu)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port0 slot0 manual polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, &pollingManual);
        if(device != NULL)
            {
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
            XInputClose(device);
            }
        else
            TESTFAIL(hLog, "Unable to open device (ec: %lu)", GetLastError());
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port0 slot0 interrupt polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, &pollingInterrupt);
        if(device != NULL)
            {
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
            XInputClose(device);
            }
        else
            TESTFAIL(hLog, "Unable to open device (ec: %lu)", GetLastError());
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    /*
    TESTCASE("invalid type port0 slot0 valid polling")
        {
        device = XInputOpen(~0, 0, 0, &pollingValid);
        if(device == NULL)
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
        else
            {
            TESTFAIL(hLog, "dev == 0x%X (ec: %lu)", device, GetLastError());
            XInputClose(device);
            }
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port0 slot1 valid polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 1, &pollingValid);
        if(device == NULL)
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
        else
            {
            TESTFAIL(hLog, "dev == 0x%X (ec: %lu)", device, GetLastError());
            XInputClose(device);
            }
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port99 slot0 valid polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 99, 0, &pollingValid);
        if(device == NULL)
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
        else
            {
            TESTFAIL(hLog, "dev == 0x%X (ec: %lu)", device, GetLastError());
            XInputClose(device);
            }
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port~0 slot0 valid polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, ~0, 0, &pollingValid);
        if(device == NULL)
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
        else
            {
            TESTFAIL(hLog, "dev == 0x%X (ec: %lu)", device, GetLastError());
            XInputClose(device);
            }
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port0 slot~0 valid polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, ~0, &pollingValid);
        if(device == NULL)
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
        else
            {
            TESTFAIL(hLog, "dev == 0x%X (ec: %lu)", device, GetLastError());
            XInputClose(device);
            }
        } ENDTESTCASE;
    TESTCASE("GAMEPAD port(XGetPortCount) slot0 valid polling")
        {
        device = XInputOpen(XDEVICE_TYPE_GAMEPAD, XGetPortCount(), 0, &pollingValid);
        if(device == NULL)
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
        else
            {
            TESTFAIL(hLog, "dev == 0x%X (ec: %lu)", device, GetLastError());
            XInputClose(device);
            }
        } ENDTESTCASE;
    TESTCASE("MU port1 slot0 valid polling")
        {
        device = XInputOpen(XDEVICE_TYPE_MEMORY_UNIT, 1, 0, &pollingValid);
        if(device == NULL)
            TESTPASS(hLog, "(ec: %lu)", GetLastError());
        else
            {
            TESTFAIL(hLog, "dev == 0x%X (ec: %lu)", device, GetLastError());
            XInputClose(device);
            }
        } ENDTESTCASE;
*/


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        DebugPrint("USBINPUT(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxStress = 1000;
        unsigned failures = 0;
        for(unsigned i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0)
                {
                DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }
            device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0, 0, &pollingValid);
            if(device == NULL) ++failures;
            else XInputClose(device);
            }
        if(failures)
            TESTFAIL(hLog, "XInputOpen mini stress failures: %u", failures);
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); });


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

    return 0;
    }
