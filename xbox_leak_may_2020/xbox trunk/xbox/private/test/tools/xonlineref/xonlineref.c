/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xonlineref.c

Abstract:

  This module is a common library for starting and stopping the xbox online stack

Author:

  Steven Kehrli (steveke) 21-Jan-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



namespace XOnlineRefNamespace {

// XOnlineRefCount is the online reference count
LONG    XOnlineRefCount = 0;
// hXOnlineRefMutex is a handle to the online ref mutex
HANDLE  hXOnlineRefMutex = NULL;

} // namespace XOnlineRefNamespace



using namespace XOnlineRefNamespace;

ULONG
WINAPI
XOnlineAddRef()
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Initializes the xbox online stack if necessary.  Increases the ref count for the xbox online stack.

Return Value:

  The new reference count.

------------------------------------------------------------------------------*/
{
    // XOnlineNewRefCount is the new reference count;
    ULONG    XOnlineNewRefCount = 0;
    // XNetRefCount is the xnet ref count
    ULONG    XNetRefCount = 0;
    // hResult is the result code returned from XOnlineStartup
    HRESULT  hResult = S_OK;
    // iLastError is the error code returned from XOnlineStartup
    INT      iLastError = 0;



    // Synchronize access to this function

    if (NULL == hXOnlineRefMutex) {
        hXOnlineRefMutex = CreateMutexA(NULL, FALSE, "XOnlineRefMutex");
    }

    WaitForSingleObject(hXOnlineRefMutex, INFINITE);

    if (0 == XOnlineRefCount) {
        // Add xnet ref
        XNetRefCount = XNetAddRef();

        if (0 < XNetRefCount) {
            // Initialize xonline
            hResult = XOnlineStartup(NULL);

            if (FAILED(hResult)) {
                // Get the error code
                iLastError = HRESULT_CODE(hResult);

                // Release xnet
                XNetRelease();
            }
        }
        else {
            // Get the error code
            iLastError = GetLastError();
        }
    }

    // Increment the ref count
    if (0 == iLastError) {
        XOnlineNewRefCount = (ULONG) ++XOnlineRefCount;
    }

    ReleaseMutex(hXOnlineRefMutex);

    if (0 != iLastError) {
        SetLastError(iLastError);
    }

    return XOnlineNewRefCount;
}



ULONG
WINAPI
XOnlineRelease()
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Decreases the ref count for the xbox online stack.  Terminates the xbox online stack if necessary.

Return Value:

  The new reference count.

------------------------------------------------------------------------------*/
{
    // XOnlineNewRefCount is the new reference count;
    ULONG  XOnlineNewRefCount = 0;



    // Synchronize access to this function
    WaitForSingleObject(hXOnlineRefMutex, INFINITE);

    // Decrement the ref count
    ASSERT(0 < XOnlineRefCount);

    XOnlineNewRefCount = (ULONG) --XOnlineRefCount;

    if (0 == XOnlineRefCount) {
        // Terminate the online stack
        XOnlineCleanup();

        // Release xnet ref
        XNetRelease();
    }

    ReleaseMutex(hXOnlineRefMutex);

    return XOnlineNewRefCount;
}
