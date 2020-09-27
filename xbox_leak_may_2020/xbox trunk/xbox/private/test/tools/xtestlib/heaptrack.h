/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    heaptrack.h

Abstract:

    Routines to tracking private heap created by HeapCreate/HeapDestroy

Author:

    Sakphong Chanbai (schanbai) 11-Jul-2000

Environment:

    Xbox

Revision History:

--*/


#ifndef __HEAPTRACK_INCLUDED__

#define __HEAPTRACK_INCLUDED__


BOOL
InitializeHeapHandleTracking(
    VOID
    );


VOID
UninitializeHeapHandleTracking(
    VOID
    );


#endif // __HEAPTRACK_INCLUDED__
