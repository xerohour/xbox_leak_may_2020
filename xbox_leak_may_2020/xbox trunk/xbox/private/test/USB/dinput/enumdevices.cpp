/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    EnumDevices.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    05-30-2000  Created

Notes:

*/

#include "stdafx.h"
#include "commontest.h"
#include <initguid.h>
#include <dinput.h>
#include <input.h>


BOOL CALLBACK EnumDevices_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
BOOL CALLBACK EnumDevices_Test_CallbackBlank(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

#define MODE_DIENUM_STOP 33
struct EnumCallbackState
    {
    DWORD threadID;
    DWORD numFound;
    DWORD mode;
    };


/*

Routine Description:

    

Arguments:

    

Return Value:

    

*/
DWORD EnumDevices_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HRESULT EnumDevices(
      DWORD dwDevType,
      LPDIENUMCALLBACK lpCallback,
      LPVOID pvRef,
      DWORD dwFlags
    );
    */
    xSetFunctionName(hLog, "EnumDevices");

    HRESULT result;
    IDirectInput8 *pDI = NULL;
    unsigned i;

    //__asm int 3;

    result = DirectInput8Create(NULL, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL);
    if(result != DI_OK)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to create the DirectInput interface (ec: %lX)", result);
        xEndVariation(hLog);
        return -1;
        }


    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("NULL")
        {
        BUGGED(345, "This causes the folowing tests to fail!");
        result = pDI->EnumDevices(0, NULL, NULL, DIEDFL_ENUMANDNOTIFYINSERT);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, NULL, NULL, DIEDFL_HALTNOTIFYINSERT); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Flags==DIEDFL_ENUMANDNOTIFYINSERT")
        {
        EnumCallbackState state = { ThreadID, 0, 1 };
        result = pDI->EnumDevices(0, EnumDevices_Test_Callback, (void*)&state, DIEDFL_ENUMANDNOTIFYINSERT);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        DebugPrint("DITEST(%d): Insert a new device...\n", ThreadID);
        for(unsigned j=0; j<1200; j++)
            {
            Sleep(16);
            InputPumpCallbacks();
            }

        xLog(hLog, XLL_INFO, "Found %u devices", state.numFound);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, EnumDevices_Test_Callback, NULL, DIEDFL_HALTNOTIFYINSERT); });
    TESTCASE("DIENUM_STOP")
        {
        EnumCallbackState state = { ThreadID, 0, MODE_DIENUM_STOP };
        result = pDI->EnumDevices(0, EnumDevices_Test_Callback, (void*)&state, DIEDFL_ENUMANDNOTIFYINSERT);
        if(state.numFound == 0 && result == DI_OK)
            TESTPASS(hLog, "Found %u devices (ec: %lX)", state.numFound, result);
        else
            TESTFAIL(hLog, "Found %u devices (ec: %lX)", state.numFound, result);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, EnumDevices_Test_Callback, NULL, DIEDFL_HALTNOTIFYINSERT); });
    // TODO device filter


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Flags == ~0")
        {
        EnumCallbackState state = { ThreadID, 0, 1 };
        result = pDI->EnumDevices(0, EnumDevices_Test_Callback, (void*)&state, ~0);
        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, EnumDevices_Test_Callback, NULL, DIEDFL_HALTNOTIFYINSERT); });
    TESTCASE("Filter == ~0")
        {
        EnumCallbackState state = { ThreadID, 0, 1 };
        result = pDI->EnumDevices(~0, EnumDevices_Test_Callback, (void*)&state, ~0);
        if(state.numFound == 0 && result == DI_OK)
            TESTPASS(hLog, "Found %u devices (ec: %lX)", state.numFound, result);
        else
            TESTFAIL(hLog, "Found %u devices (ec: %lX)", state.numFound, result);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, EnumDevices_Test_Callback, NULL, DIEDFL_HALTNOTIFYINSERT); });


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        EnumCallbackState state = { ThreadID, 0, 0 };
        //DebugPrint("DITEST(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned maxLoop = 1000;
        for(i=0; i<maxLoop; i++)
            {
            //if(i%(maxLoop/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxLoop);
            result = pDI->EnumDevices(0, EnumDevices_Test_CallbackBlank, (void*)&state, DIEDFL_ENUMANDNOTIFYINSERT);
            pDI->EnumDevices(0, EnumDevices_Test_CallbackBlank, NULL, DIEDFL_HALTNOTIFYINSERT);
            result = pDI->EnumDevices(0, EnumDevices_Test_CallbackBlank, (void*)&state, DIEDFL_NOTIFYREMOVE);
            pDI->EnumDevices(0, EnumDevices_Test_CallbackBlank, NULL, DIEDFL_HALTNOTIFYREMOVE);
            }
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;


    pDI->Release();
    return 0;
    }



BOOL CALLBACK EnumDevices_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
    {
    if(!pvRef)
        {
        DebugPrint("DITEST(-): Found Device: but state is NULL\n");
        return DIENUM_CONTINUE;
        }

    EnumCallbackState *state = (EnumCallbackState*)pvRef;
    if(state->mode == MODE_DIENUM_STOP)
        {
        return DIENUM_STOP;
        }

    ++state->numFound;

    if(state->mode > 0)
        {
        DebugPrint("DITEST(%d): Found Device:\n"
            "   Device Type:   %X\n"
            "   Instance GUID: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"
            "   Product GUID:  %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"
            ,
            state->threadID, lpddi->dwDevType, 
            lpddi->guidInstance.Data1, lpddi->guidInstance.Data2, lpddi->guidInstance.Data3, lpddi->guidInstance.Data4[0], lpddi->guidInstance.Data4[1], lpddi->guidInstance.Data4[2], lpddi->guidInstance.Data4[3], lpddi->guidInstance.Data4[4], lpddi->guidInstance.Data4[5], lpddi->guidInstance.Data4[6], lpddi->guidInstance.Data4[7],
            lpddi->guidProduct.Data1, lpddi->guidProduct.Data2, lpddi->guidProduct.Data3, lpddi->guidProduct.Data4[0], lpddi->guidProduct.Data4[1], lpddi->guidProduct.Data4[2], lpddi->guidProduct.Data4[3], lpddi->guidProduct.Data4[4], lpddi->guidProduct.Data4[5], lpddi->guidProduct.Data4[6], lpddi->guidProduct.Data4[7]
            );
        }

    return DIENUM_CONTINUE;
    }

BOOL CALLBACK EnumDevices_Test_CallbackBlank(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
    {
    return DIENUM_CONTINUE;
    }
