/*++

Copyright (c) 1990-2001  Microsoft Corporation

Module Name:

    thread.c

Abstract:

    This module implements Win32 Thread Local Storage APIs

--*/

#include "basedll.h"
#pragma hdrstop

//
// Array of per thread TLS slots.
//
__declspec(thread) PVOID XapiTlsSlots[TLS_MINIMUM_AVAILABLE];

//
// Bitmap of the allocated TLS slots.
//
ULONG XapiTlsAllocBitmap[TLS_MINIMUM_AVAILABLE / 32] = {
    0xFFFFFFFF
};

DWORD
APIENTRY
TlsAlloc(
    VOID
    )
/*++

Routine Description:

    A TLS index may be allocated using TlsAlloc.  Win32 garuntees a
    minimum number of TLS indexes are available in each process.  The
    constant TLS_MINIMUM_AVAILABLE defines the minimum number of
    available indexes.  This minimum is at least 64 for all Win32
    systems.

Arguments:

    None.

Return Value:

    Not-0xffffffff - Returns a TLS index that may be used in a
        subsequent call to TlsFree, TlsSetValue, or TlsGetValue.  The
        storage associated with the index is initialized to NULL.

    0xffffffff - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    DWORD dwBitmapIndex;
    DWORD dwBitIndex;
    DWORD dwTlsIndex;

    XapiAcquireProcessLock();

    dwTlsIndex = 0xffffffff;

    for (dwBitmapIndex = 0; dwBitmapIndex < ARRAYSIZE(XapiTlsAllocBitmap);
        dwBitmapIndex++) {

        //
        // If the bitmap location contains a non-zero value, then there's at
        // least TLS index that we can allocate from this block.  Find the
        // lowest free bit and remove it from the bitmap.
        //

        if (XapiTlsAllocBitmap[dwBitmapIndex] != 0) {
            dwBitIndex = RtlFindFirstSetRightMember(XapiTlsAllocBitmap[dwBitmapIndex]);
            XapiTlsAllocBitmap[dwBitmapIndex] &= ~(1 << dwBitIndex);
            dwTlsIndex = (dwBitmapIndex * 32) + dwBitIndex;
            break;
        }
    }

    XapiReleaseProcessLock();

    return dwTlsIndex;
}

LPVOID
APIENTRY
TlsGetValue(
    DWORD dwTlsIndex
    )

/*++

Routine Description:

    This function is used to retrive the value in the TLS storage
    associated with the specified index.

    If the index is valid this function clears the value returned by
    GetLastError(), and returns the value stored in the TLS slot
    associated with the specified index.  Otherwise a value of NULL is
    returned with GetLastError updated appropriately.

    It is expected, that DLLs will use TlsAlloc and TlsGetValue as
    follows:

      - Upon DLL initialization, a TLS index will be allocated using
        TlsAlloc.  The DLL will then allocate some dynamic storage and
        store its address in the TLS slot using TlsSetValue.  This
        completes the per thread initialization for the initial thread
        of the process.  The TLS index is stored in instance data for
        the DLL.

      - Each time a new thread attaches to the DLL, the DLL will
        allocate some dynamic storage and store its address in the TLS
        slot using TlsSetValue.  This completes the per thread
        initialization for the new thread.

      - Each time an initialized thread makes a DLL call requiring the
        TLS, the DLL will call TlsGetValue to get the TLS data for the
        thread.

Arguments:

    dwTlsIndex - Supplies a TLS index allocated using TlsAlloc.  The
        index specifies which TLS slot is to be located.  Translating a
        TlsIndex does not prevent a TlsFree call from proceding.

Return Value:

    NON-NULL - The function was successful. The value is the data stored
        in the TLS slot associated with the specified index.

    NULL - The operation failed, or the value associated with the
        specified index was NULL.  Extended error status is available
        using GetLastError.  If this returns non-zero, the index was
        invalid.

--*/
{
    RIP_ON_NOT_TRUE("TlsGetValue()", (dwTlsIndex < TLS_MINIMUM_AVAILABLE));

    return XapiTlsSlots[dwTlsIndex];
}

BOOL
APIENTRY
TlsSetValue(
    DWORD dwTlsIndex,
    LPVOID lpTlsValue
    )

/*++

Routine Description:

    This function is used to store a value in the TLS storage associated
    with the specified index.

    If the index is valid this function stores the value and returns
    TRUE. Otherwise a value of FALSE is returned.

    It is expected, that DLLs will use TlsAlloc and TlsSetValue as
    follows:

      - Upon DLL initialization, a TLS index will be allocated using
        TlsAlloc.  The DLL will then allocate some dynamic storage and
        store its address in the TLS slot using TlsSetValue.  This
        completes the per thread initialization for the initial thread
        of the process.  The TLS index is stored in instance data for
        the DLL.

      - Each time a new thread attaches to the DLL, the DLL will
        allocate some dynamic storage and store its address in the TLS
        slot using TlsSetValue.  This completes the per thread
        initialization for the new thread.

      - Each time an initialized thread makes a DLL call requiring the
        TLS, the DLL will call TlsGetValue to get the TLS data for the
        thread.

Arguments:

    dwTlsIndex - Supplies a TLS index allocated using TlsAlloc.  The
        index specifies which TLS slot is to be located.  Translating a
        TlsIndex does not prevent a TlsFree call from proceding.

    lpTlsValue - Supplies the value to be stored in the TLS Slot.

Return Value:

    TRUE - The function was successful. The value lpTlsValue was
        stored.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    RIP_ON_NOT_TRUE("TlsSetValue()", (dwTlsIndex < TLS_MINIMUM_AVAILABLE));

    XapiTlsSlots[dwTlsIndex] = lpTlsValue;

    return TRUE;
}

BOOL
APIENTRY
TlsFree(
    DWORD dwTlsIndex
    )

/*++

Routine Description:

    A valid TLS index may be free'd using TlsFree.

Arguments:

    dwTlsIndex - Supplies a TLS index allocated using TlsAlloc.  If the
        index is a valid index, it is released by this call and is made
        available for reuse.  DLLs should be carefull to release any
        per-thread data pointed to by all of their threads TLS slots
        before calling this function.  It is expected that DLLs will
        only call this function (if at ALL) during their process detach
        routine.

Return Value:

    TRUE - The operation was successful. TlsAlloc is free to reallocate this
        index.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
    DWORD dwBitmapIndex;
    DWORD dwBitIndex;
    ULONG TlsDataRelativeOffset;
    KIRQL OldIrql;
    PKPROCESS Process;
    PLIST_ENTRY NextListEntry;
    PKTHREAD Thread;

    RIP_ON_NOT_TRUE("TlsFree()", (dwTlsIndex < TLS_MINIMUM_AVAILABLE));

    XapiAcquireProcessLock();

    dwBitmapIndex = dwTlsIndex / 32;
    dwBitIndex = dwTlsIndex % 32;

    //
    // Mark the TLS index as free.
    //

    XapiTlsAllocBitmap[dwBitmapIndex] |= (1 << dwBitIndex);

    //
    // Compute the TLS data relative byte offset for the supplied TLS index.
    //

    TlsDataRelativeOffset = (ULONG_PTR)&XapiTlsSlots[dwTlsIndex] -
        (ULONG_PTR)KeGetCurrentThread()->TlsData;

    //
    // Loop over the active threads and zero out the freed TLS slot.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();
    Process = KeGetCurrentThread()->ApcState.Process;
    NextListEntry = Process->ThreadListHead.Flink;

    while (NextListEntry != &Process->ThreadListHead) {

        Thread = CONTAINING_RECORD(NextListEntry, KTHREAD, ThreadListEntry);

        //
        // Only zero out the freed TLS slot for threads created by XAPI.
        //

        if (Thread->TlsData != NULL) {
            *((LPVOID*)((ULONG_PTR)Thread->TlsData + TlsDataRelativeOffset)) = NULL;
        }

        NextListEntry = Thread->ThreadListEntry.Flink;
    }

    KeLowerIrql(OldIrql);

    XapiReleaseProcessLock();

    return TRUE;
}
