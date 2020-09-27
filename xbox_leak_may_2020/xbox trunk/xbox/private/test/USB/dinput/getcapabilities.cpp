/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    GetCapabilities.cpp

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


BOOL CALLBACK GetCapabilities_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

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
DWORD GetCapabilities_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HRESULT GetCapabilities(
      LPDIDEVCAPS lpDIDevCaps
    );
    */
    xSetFunctionName(hLog, "GetCapabilities");

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

    //__asm int 3;

    result = DirectInput8Create(NULL, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL);
    if(result != DI_OK)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "Unable to create the DirectInput interface (ec: %lX)", result);
        xEndVariation(hLog);
        return -1;
        }

    GUID guid = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);
    if(result != DI_OK || !pDID)
        {
        xStartVariation(hLog, "Initializing State");
        xLog(hLog, XLL_BLOCK, "CreateDevice failed (ec: %lX)", result);
        xEndVariation(hLog);
        return -1;
        }
    pDID->SetProperty(DIPROP_AUTOPOLL, &dipdw.diph);
    pDID->Acquire();

    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("NULL")
        {
        diDevCaps.dwSize = sizeof(DIDEVCAPS);
        result = pDID->GetCapabilities(NULL);
        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Checking plugged in devices")
        {
        EnumCallbackState state = { ThreadID, 0, 1, pDI, hLog };
        result = pDI->EnumDevices(0, GetCapabilities_Test_Callback, (void*)&state, DIEDFL_ENUMANDNOTIFYINSERT);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        xLog(hLog, XLL_INFO, "Found %u devices", state.numFound);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, GetCapabilities_Test_Callback, NULL, DIEDFL_HALTNOTIFYINSERT); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("dwSize == 0")
        {
        diDevCaps.dwSize = 0;
        result = pDID->GetCapabilities(NULL);
        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("dwSize == 1")
        {
        diDevCaps.dwSize = 0;
        result = pDID->GetCapabilities(NULL);
        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("dwSize == 0xFFFF")
        {
        diDevCaps.dwSize = 0xFFFF;
        result = pDID->GetCapabilities(NULL);
        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("dwSize == ~0")
        {
        diDevCaps.dwSize = ~0;
        result = pDID->GetCapabilities(NULL);
        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("Recently removed")
        {
        // TODO plug in device x
        // TODO unplug device x
        BLOCKED(0, "Need USB Simulator");
        } ENDTESTCASE;


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        //DebugPrint("DITEST(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned maxLoop = 100000;
        for(i=0; i<maxLoop; i++)
            {
            //if(i%(maxLoop/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxLoop);
            diDevCaps.dwSize = sizeof(DIDEVCAPS);
            result = pDID->GetCapabilities(&diDevCaps);
            }
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    pDID->Unacquire();
    pDID->Release();
    pDI->Release();
    return 0;
    }



BOOL CALLBACK GetCapabilities_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
    {
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

    TESTCASE(buffer)
        {
        result = state->pDI->CreateDevice(lpddi->guidInstance, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK || !pDID)
            {
            sprintf(buffer, "CreateDevice failed (ec: %lX)", result);
            BLOCKED(0, buffer);
            }

        pDID->SetProperty(DIPROP_AUTOPOLL, &dipdw.diph);
        pDID->Acquire();
        result = pDID->GetCapabilities(&diDevCaps);

        if(result == DI_OK)
            {
            TESTPASS(hLog, "(ec: %lX)", result);
            /*
            DebugPrint("--------------------------------------------\n", diDevCaps.dwAxes);
            DebugPrint("diDevCaps.dwAxes                = 0x%0.8x\n", diDevCaps.dwAxes);
            DebugPrint("diDevCaps.dwButtons             = 0x%0.8x\n", diDevCaps.dwButtons);
            DebugPrint("diDevCaps.dwDevType             = 0x%0.8x\n", diDevCaps.dwDevType);
            DebugPrint("diDevCaps.dwFFDriverVersion     = 0x%0.8x\n", diDevCaps.dwFFDriverVersion);
            DebugPrint("diDevCaps.dwFFMinTimeResolution = 0x%0.8x\n", diDevCaps.dwFFMinTimeResolution);
            DebugPrint("diDevCaps.dwFFSamplePeriod      = 0x%0.8x\n", diDevCaps.dwFFSamplePeriod);
            DebugPrint("diDevCaps.dwFirmwareRevision    = 0x%0.8x\n", diDevCaps.dwFirmwareRevision);
            DebugPrint("diDevCaps.dwFlags               = 0x%0.8x\n", diDevCaps.dwFlags);
            DebugPrint("diDevCaps.dwHardwareRevision    = 0x%0.8x\n", diDevCaps.dwHardwareRevision);
            DebugPrint("diDevCaps.dwPOVs                = 0x%0.8x\n", diDevCaps.dwPOVs);
            */
            }
        else
            {
            TESTFAIL(hLog, "(ec: %lX)", result);
            }

        pDID->Unacquire();
        pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });

    return DIENUM_CONTINUE;
    }
