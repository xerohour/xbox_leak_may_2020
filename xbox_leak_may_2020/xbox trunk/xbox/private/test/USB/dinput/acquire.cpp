/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    Acquire.cpp

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


BOOL CALLBACK Acquire_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

struct EnumCallbackState
    {
    DWORD threadID;
    DWORD numFound;
    DWORD mode;
    IDirectInput8* pDI;
    HANDLE hLog;
    };


/*

Routine Description:

    

Arguments:

    

Return Value:

    

*/
DWORD Acquire_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HRESULT Acquire();

    HRESULT Unacquire();
    */
    xSetFunctionName(hLog, "Acquire");

    HRESULT result;
    IDirectInput8 *pDI = NULL;
    IDirectInputDevice8 *pDID = NULL;
    unsigned i;
    DIDEVCAPS diDevCaps;
    DIPROPDWORD dipdw;
    diDevCaps.dwSize = sizeof(DIDEVCAPS);
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = false;

    __asm int 3;

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


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Checking plugged in devices")
        {
        EnumCallbackState state = { ThreadID, 0, 1, pDI, hLog };
        result = pDI->EnumDevices(0, Acquire_Test_Callback, (void*)&state, DIEDFL_ENUMANDNOTIFYINSERT);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        xLog(hLog, XLL_INFO, "Found %u devices", state.numFound);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, Acquire_Test_Callback, NULL, DIEDFL_HALTNOTIFYINSERT); });
    xSetFunctionName(hLog, "Acquire");


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////


/*
    TESTCASE("Reciently removed")
        {
        // TODO plug in device x
        // TODO unplug device x
        GUID guid = { x, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = state->pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
*/

    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    xSetFunctionName(hLog, "Acquire");
    TESTCASE("Mini Stress")
        {
        GUID guid = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);
        if(result != DI_OK || !pDID)
            {
            char buffer[1024];
            sprintf(buffer, "CreateDevice failed (ec: %lX)", result);
            BLOCKED(0, buffer);
            }
        DebugPrint("DITEST(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned maxLoop = 10000;
        for(i=0; i<maxLoop; i++)
            {
            if(i%(maxLoop/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxLoop);
            pDID->SetProperty(DIPROP_AUTOPOLL, &dipdw.diph);
            pDID->Acquire();
            pDID->Unacquire();
            }
        } ENDTESTCASE_CLEAN({ if(pDID) { pDID->Release(); pDID=NULL; } DebugPrint("   (%d) 100%%\n", ThreadID); });


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



BOOL CALLBACK Acquire_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
    {
    //__asm int 3;

    if(!pvRef)
        {
        DebugPrint("DITEST(-): Found Device: but state is NULL\n");
        return DIENUM_CONTINUE;
        }

    EnumCallbackState *state = (EnumCallbackState*)pvRef;
    HANDLE hLog = state->hLog;
    ++state->numFound;

    HRESULT result;
    char buffer[1024];
    sprintf(buffer, "Device #%d", state->numFound);

    IDirectInputDevice8 *pDID = NULL;
    DIDEVCAPS diDevCaps;
    DIPROPDWORD dipdw;
    diDevCaps.dwSize = sizeof(DIDEVCAPS);
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = false;

    xSetFunctionName(hLog, "Acquire");
    TESTCASE(buffer)
        {
        result = state->pDI->CreateDevice(lpddi->guidInstance, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK || !pDID)
            {
            sprintf(buffer, "CreateDevice failed (ec: %lX)", result);
            BLOCKED(0, buffer);
            }

        pDID->SetProperty(DIPROP_AUTOPOLL, &dipdw.diph);
        result = pDID->Acquire();

        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;

    xSetFunctionName(hLog, "Unacquire");
    TESTCASE(buffer)
        {
        if(!pDID)
            {
            sprintf(buffer, "CreateDevice failed (ec: %lX)", result);
            BLOCKED(0, buffer);
            }

        result = pDID->Unacquire();

        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });

    return DIENUM_CONTINUE;
    }
