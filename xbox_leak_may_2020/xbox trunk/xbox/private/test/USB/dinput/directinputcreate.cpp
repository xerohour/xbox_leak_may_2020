/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    DirectInput8Create.cpp

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

/*

Routine Description:

    

Arguments:

    

Return Value:

    

*/
DWORD DirectInputCreate_Test(HANDLE hLog, DWORD ThreadID)
    {
    /*
    HRESULT WINAPI DirectInput8Create(
      HINSTANCE hinst,
      DWORD dwVersion,
      REFIID riidltf,
      LPVOID * ppvOut,
      LPUNKNOWN punkOuter
    );
    */
    xSetFunctionName(hLog, "DirectInput8Create");

    HRESULT result;
    IDirectInput8 *pDI = NULL;
    unsigned i;

    DWORD time = GetTickCount();


    //////////////////////////////////////////////////////////////////////////
    // NULL & ""
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("ver=0, pID=NULL")
        {
        result = DirectInput8Create(NULL, 0, IID_IDirectInput8, NULL, NULL);
        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE_CLEAN({ if(pDI) pDI->Release(); pDI=NULL; });
    TESTCASE("ver=current, pID=NULL")
        {
        result = DirectInput8Create(NULL, DIRECTINPUT_VERSION, IID_IDirectInput8, NULL, NULL);
        if(result != DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE_CLEAN({ if(pDI) pDI->Release(); pDI=NULL; });


    //////////////////////////////////////////////////////////////////////////
    // Straight line successes
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Normal Creation")
        {
        result = DirectInput8Create(NULL, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE_CLEAN({ if(pDI) pDI->Release(); pDI=NULL; });


    //////////////////////////////////////////////////////////////////////////
    // Straight line failures
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("DI version 1")
        {
        result = DirectInput8Create(NULL, 0x0100, IID_IDirectInput8, (void**)&pDI, NULL);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE_CLEAN({ if(pDI) pDI->Release(); pDI=NULL; });
    TESTCASE("DI version 8.1")
        {
        result = DirectInput8Create(NULL, 0x0801, IID_IDirectInput8, (void**)&pDI, NULL);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE_CLEAN({ if(pDI) pDI->Release(); pDI=NULL; });
    TESTCASE("DI version 10")
        {
        result = DirectInput8Create(NULL, 0x0A00, IID_IDirectInput8, (void**)&pDI, NULL);
        if(result == DI_OK)
            TESTPASS(hLog, "(ec: %lX)", result);
        else
            TESTFAIL(hLog, "(ec: %lX)", result);
        } ENDTESTCASE_CLEAN({ if(pDI) pDI->Release(); pDI=NULL; });


    //////////////////////////////////////////////////////////////////////////
    // Mini Stress
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Mini Stress")
        {
        //DebugPrint("DITEST(%d): This test takes a long time to run...\n", ThreadID);
        unsigned maxLoop = 100000;
        for(i=0; i<maxLoop; i++)
            {
            //if(i%(maxLoop/10) == 0) DebugPrint("   (%d) %02d%%\n", ThreadID, i*100/maxLoop);
            result = DirectInput8Create(NULL, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL);
            if(pDI) pDI->Release();
            pDI = NULL;
            }
        } ENDTESTCASE_CLEAN({ DebugPrint("   (%d) 100%%\n", ThreadID); });


    //////////////////////////////////////////////////////////////////////////
    // Performance
    //////////////////////////////////////////////////////////////////////////
    TESTCASE("Performance")
        {
        xLog(hLog, XLL_INFO, "Test took %lu milliseconds to run", GetTickCount()-time);
        } ENDTESTCASE;

    return 0;
    }
