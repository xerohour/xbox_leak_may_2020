/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    GetDeviceStatus.cpp

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


BOOL CALLBACK GetDeviceStatus_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

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
DWORD GetDeviceStatus_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HRESULT GetDeviceStatus(
      REFGUID rguidInstance  
    );
    */
    xSetFunctionName(hLog, "GetDeviceStatus");

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


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Checking plugged in devices")
        {
        EnumCallbackState state = { ThreadID, 0, 1, pDI, hLog };
        result = pDI->EnumDevices(0, GetDeviceStatus_Test_Callback, (void*)&state, DIEDFL_ENUMANDNOTIFYINSERT);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        xLog(hLog, XLL_INFO, "Found %u devices", state.numFound);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, GetDeviceStatus_Test_Callback, NULL, DIEDFL_HALTNOTIFYINSERT); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("{ 0xFFFF 0 0 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->GetDeviceStatus(guid);
        if(result == DIERR_INVALIDPARAM)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("{ 0 0 0 0 0 0 0 0 0 0 1 }")
        {
        GUID guid = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
        result = pDI->GetDeviceStatus(guid);
        if(result == DIERR_INVALIDPARAM)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("{ 0 1 0 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->GetDeviceStatus(guid);
        if(result == DIERR_INVALIDPARAM)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("{ 0 0 1 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->GetDeviceStatus(guid);
        if(result == DIERR_INVALIDPARAM)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("{ 0 0 0 1 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->GetDeviceStatus(guid);
        if(result == DIERR_INVALIDPARAM)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("{ 0x20 0 0 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->GetDeviceStatus(guid);
        if(result == DIERR_INVALIDPARAM)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
    TESTCASE("{ 0x21 0 0 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0x21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->GetDeviceStatus(guid);
        if(result == DIERR_INVALIDPARAM)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;

/*
    TESTCASE("Reciently removed")
        {
        // TODO plug in device x
        // TODO unplug device x
        GUID guid = { x, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->GetDeviceStatus(guid);
        if(result == DIERR_INVALIDPARAM)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;
*/

    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        GUID guid = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        //DebugPrint("DITEST(%d): This test takes a long time to run...\n", ThreadID);
        const unsigned maxLoop = 100000;
        for(i=0; i<maxLoop; i++)
            {
            //if(i%(maxLoop/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxLoop);
            result = pDI->GetDeviceStatus(guid);
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



BOOL CALLBACK GetDeviceStatus_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
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

    TESTCASE(buffer)
        {
        result = state->pDI->GetDeviceStatus(lpddi->guidInstance);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE;

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
