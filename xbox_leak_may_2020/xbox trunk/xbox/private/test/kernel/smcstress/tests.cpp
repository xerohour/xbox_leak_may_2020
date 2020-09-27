/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tests.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

    TRAYSTATE_RESET         B'01110000'	;0x70
    TRAYSTATE_MEDIADETECT   B'01100000'	;0x60
    TRAYSTATE_NOMEDIA       B'01000000'	;0x40
    TRAYSTATE_TRAYCLOSED    B'00000000'	;0x00
    TRAYSTATE_CLOSING       B'01010000'	;0x50
    TRAYSTATE_OPENING       B'00110000'	;0x30
    TRAYSTATE_UNLOADING     B'00100000'	;0x20
    TRAYSTATE_TRAYOPEN      B'00010000'	;0x10

*/

#include "stdafx.h"
#include "commontest.h"
#include "smcstress.h"

extern "C"
    {
    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );
    }

static void WriteSMC(unsigned char addr, unsigned char value)
    {
    HalWriteSMBusValue(0x20, addr, FALSE, value);
    }

static DWORD ReadSMC(unsigned char addr)
    {
    DWORD value = 0xCCCCCCCC;
    HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
    }

/*

Routine Description:

    Tests the HalReadSMBusValue API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    DWORD - 

*/
DWORD SMCStress::HalReadSMBusValue_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );
    */
    xSetFunctionName(hLog, "HalReadSMBusValue");
    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // Version
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("CPU Temp")
        {
        char ver[3];
        ver[0] = (char)ReadSMC(0x01);
        ver[1] = (char)ReadSMC(0x01);
        ver[2] = (char)ReadSMC(0x01);

        TESTPASS(hLog, "Version %c %c %c", ver[0], ver[1], ver[2]);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // DVD tray and video state
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("DVD Tray State")
        {
        DWORD value = ReadSMC(0x03) & 0x70;
        if(value == 0x40 || value == 0x00) // closed (with or without media)
            {
            TESTPASS(hLog, "DVD Tray State 0x%X", value);
            }
        else
            {
            TESTFAIL(hLog, "DVD Tray State 0x%X", value);
            }
        } ENDTESTCASE;
    TESTCASE("AV Mode")
        {
        DWORD value = ReadSMC(0x04);
        if(value == 7) // NO AV pack
            {
            TESTFAIL(hLog, "AV Mode 0x%X", value);
            }
        else
            {
            TESTPASS(hLog, "AV Mode 0x%X", value);
            }
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Temperature
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("CPU Temp")
        {
        DWORD value = ReadSMC(0x09);
        if(value > 0 && value < 100)
            TESTPASS(hLog, "CPU temp: %lu C", value);
        else
            TESTFAIL(hLog, "CPU temp: %lu C", value);
        } ENDTESTCASE;
    TESTCASE("Air Temp")
        {
        DWORD value = ReadSMC(0x0A);
        if(value > 0 && value < 100)
            TESTPASS(hLog, "Air temp: %lu C", value);
        else
            TESTFAIL(hLog, "Air temp: %lu C", value);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // protocol failure test (bug 6225)
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Protocol Failure Test")
        {
        DWORD tValue = ReadSMC(0x04);
        DWORD value = 0xCCCCCCCC;
        for(unsigned i=0; i<10000; i++)
            {
            value = ReadSMC(0x04);
            if(value != tValue)
                {
                TESTFAIL(hLog, "Old VMODE (0x%X) != New VMODE (0x%X) after %u tries", tValue, value, i);
                //DebugPrint("%d%d%d (0x%08X)\n", !!(value & 0x04), !!(value & 0x02), !!(value & 0x01), value);
                break;
                }
            Sleep(5); // without this we can starve out the SMC causing a shutdown ... :)
            }

        if(tValue == value) TESTPASS(hLog, "no errors");
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Error Flag
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Error Code")
        {
        unsigned failures = 0;
        for(unsigned i=0; i<0xFF; i++)
            {
            WriteSMC(0x0E, (unsigned char)i);
            DWORD value = ReadSMC(0x0F);
            if(value != i)
                {
                ++failures;
                TESTFAIL(hLog, "Error Code (%u != %u)", value, i);
                }
            }

        if(!failures)
            TESTPASS(hLog, "Error Code");
        } ENDTESTCASE_CLEAN({WriteSMC(0x0E, 0x00);});

/*
    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        DebugPrint("SMCSTRESS(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned int maxStress = 10000;
        unsigned failures = 0;
        for(i=0; i<maxStress; i++)
            {
            if(i%(maxStress/10) == 0)
                {
                DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }

            swprintf(buffer, L"nic%u", i);
            if(XSetNickname(buffer, TRUE) != TRUE)
                {
                ++failures;
                }
            }
        if(failures)
            TESTFAIL(hLog, "Mini stress failures: %u", failures);
        else
            TESTPASS(hLog, "Mini stress failures: %u", failures);
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); });

        */

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


/*

Routine Description:

    Tests the HalWriteSMBusValue API

Arguments:

    HANDLE hLog - handle to an XLOG object
    DWORD ThreadID - unique id of the thread - used to keep tests from 
        corrupting the same file

Return Value:

    DWORD - 

*/
DWORD SMCStress::HalWriteSMBusValue_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );
    */
    xSetFunctionName(hLog, "HalWriteSMBusValue");
    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        DebugPrint("SMCSTRESS(%d): This test takes a long time to run...\n", ThreadID);
        unsigned int maxStress;
        if(stressMode) maxStress = 10;
        else maxStress = 1;
        unsigned percentage = (maxStress/10 == 0) ? 1 : maxStress/10;
        unsigned i;
        for(i=0; i<maxStress; i++)
            {
            if(i%(percentage) == 0)
                {
                DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxStress);
                //LogResourceStatus(NULL, true);
                }

            for(unsigned red=0; red<0xF; red++)
                {
                for(unsigned green=0; green<0xF; green++)
                    {
                    // set the LED color
                    WriteSMC(0x08, (red<<4) + green);
                    WriteSMC(0x07, 0x01);

                    // set no system error
                    WriteSMC(0x0E, 0x00);

                    // use default fan control
                    WriteSMC(0x05, 0x00);

                    Sleep(250);
                    }
                }
            }
        TESTPASS(hLog, "Mini stress");
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
    WriteSMC(0x08, 0x0F); // set led to green
    WriteSMC(0x07, 0x01);
    WriteSMC(0x07, 0x00); // set led to default behavior


    return 0;
    }

