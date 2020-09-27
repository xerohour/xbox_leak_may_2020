/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    USBSimulator.cpp

Abstract:

    Implementation of DLL Exports

Author:

    Josh Poley (jpoley)

Revision History:

Notes:
    Proxy/Stub Information
        To build a separate proxy/stub DLL, 
        run nmake -f USBSimulatorps.mk in the project directory.

*****************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "COMUSBSimulator.h"

#include "USBSimulator_i.c"
#include "Duke.h"
#include "MU.h"
#include "Hawk.h"
#include "Simulator.h"
#include "Debugger.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Duke, CDuke)
OBJECT_ENTRY(CLSID_MU, CMU)
OBJECT_ENTRY(CLSID_Hawk, CHawk)
OBJECT_ENTRY(CLSID_Simulator, CSimulator)
OBJECT_ENTRY(CLSID_Debugger, CDebugger)
END_OBJECT_MAP()


/*****************************************************************************

Routine Description:

    DllMain

    DLL Entry Point

Arguments:


Return Value:

    
*****************************************************************************/
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
    {
    if(dwReason == DLL_PROCESS_ATTACH)
        {
        _Module.Init(ObjectMap, hInstance, &LIBID_USBSIMULATORLib);
        DisableThreadLibraryCalls(hInstance);
        }
    else if(dwReason == DLL_PROCESS_DETACH)
        {
        _Module.Term();
        }
    return TRUE;    // ok
    }


/*****************************************************************************

Routine Description:

    DllCanUnloadNow

    Used to determine whether the DLL can be unloaded by OLE

Arguments:

    none

Return Value:

    
*****************************************************************************/
STDAPI DllCanUnloadNow(void)
    {
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
    }


/*****************************************************************************

Routine Description:

    DllGetClassObject

    Returns a class factory to create an object of the requested type

Arguments:

Return Value:

*****************************************************************************/
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
    {
    return _Module.GetClassObject(rclsid, riid, ppv);
    }


/*****************************************************************************

Routine Description:

    DllRegisterServer

    Adds entries to the system registry

Arguments:

Return Value:

*****************************************************************************/
STDAPI DllRegisterServer(void)
    {
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
    }


/*****************************************************************************

Routine Description:

    DllUnregisterServer

    Removes entries from the system registry

Arguments:

    none

Return Value:
    
*****************************************************************************/
STDAPI DllUnregisterServer(void)
    {
    return _Module.UnregisterServer(TRUE);
    }


