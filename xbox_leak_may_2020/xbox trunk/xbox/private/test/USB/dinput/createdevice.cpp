/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    CreateDevice.cpp

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


BOOL CALLBACK CreateDevice_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

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
DWORD CreateDevice_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HRESULT CreateDevice(
      REFGUID rguid,
      LPDIRECTINPUTDEVICE *lplpDirectInputDevice,
      LPUNKNOWN pUnkOuter
    );

    HRESULT CreateDeviceEx(
      REFGUID rguid,
      REFIID riid,
      LPVOID *pvOut,
      LPUNKNOWN pUnkOuter
    );
    */
    xSetFunctionName(hLog, "CreateDevice");

    HRESULT result;
    IDirectInput8 *pDI = NULL;
    IDirectInputDevice8 *pDID = NULL;
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
        result = pDI->EnumDevices(0, CreateDevice_Test_Callback, (void*)&state, DIEDFL_ENUMANDNOTIFYINSERT);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        xLog(hLog, XLL_INFO, "Found %u devices", state.numFound);
        } ENDTESTCASE_CLEAN({ pDI->EnumDevices(0, CreateDevice_Test_Callback, NULL, DIEDFL_HALTNOTIFYINSERT); });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    xSetFunctionName(hLog, "CreateDevice");
    TESTCASE("{ 0xFFFF 0 0 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
        result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID != NULL)
            {
            TESTFAIL(hLog, "Device != NULL (ec: %lX)", result);
            }

        if(pDID) pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
    TESTCASE("{ 0 0 0 0 0 0 0 0 0 0 1 }")
        {
        GUID guid = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
        result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID != NULL)
            {
            TESTFAIL(hLog, "Device != NULL (ec: %lX)", result);
            }

        if(pDID) pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
    TESTCASE("{ 0 1 0 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID != NULL)
            {
            TESTFAIL(hLog, "Device != NULL (ec: %lX)", result);
            }

        if(pDID) pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
    TESTCASE("{ 0 0 1 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID != NULL)
            {
            TESTFAIL(hLog, "Device != NULL (ec: %lX)", result);
            }

        if(pDID) pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
    TESTCASE("{ 0 0 0 1 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID != NULL)
            {
            TESTFAIL(hLog, "Device != NULL (ec: %lX)", result);
            }

        if(pDID) pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
    TESTCASE("{ 0x20 0 0 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID != NULL)
            {
            TESTFAIL(hLog, "Device != NULL (ec: %lX)", result);
            }

        if(pDID) pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
    TESTCASE("{ 0x21 0 0 0 0 0 0 0 0 0 0 }")
        {
        GUID guid = { 0x21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID != NULL)
            {
            TESTFAIL(hLog, "Device != NULL (ec: %lX)", result);
            }

        if(pDID) pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });


/*
    TESTCASE("Reciently removed")
        {
        // TODO plug in device x
        // TODO unplug device x
        GUID guid = { x, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        result = state->pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);

        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID != NULL)
            {
            TESTFAIL(hLog, "Device != NULL (ec: %lX)", result);
            }

        if(pDID) pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
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
            result = pDI->CreateDevice(guid, (IDirectInputDevice8**)&pDID, NULL);
            if(pDID) pDID->Release();
            pDID = NULL;
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



BOOL CALLBACK CreateDevice_Test_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
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

    xSetFunctionName(hLog, "CreateDevice");
    TESTCASE(buffer)
        {
        result = state->pDI->CreateDevice(lpddi->guidInstance, (IDirectInputDevice8**)&pDID, NULL);

        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID == NULL)
            {
            TESTFAIL(hLog, "Device == NULL (ec: %lX)", result);
            break;
            }

        pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
/*
    xSetFunctionName(hLog, "CreateDeviceEx");
    TESTCASE(buffer)
        {
        result = state->pDI->CreateDeviceEx(lpddi->guidInstance, IID_IDirectInputDevice8, (void**)&pDID, NULL);

        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);

        if(pDID == NULL)
            {
            TESTFAIL(hLog, "Device == NULL (ec: %lX)", result);
            break;
            }

        pDID->Release();
        pDID = NULL;
        } ENDTESTCASE_CLEAN({ if(pDID) pDID->Release(); pDID=NULL; });
*/


    return DIENUM_CONTINUE;
    }
