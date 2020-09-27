/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    xfiber.h

Abstract:

    This module defines fiber structures

--*/

#ifndef _XFIBER_H
#define _XFIBER_H

//
// Structure to hold the per fiber instance data.
//

typedef struct _XFIBER {
    PVOID FiberData;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID KernelStack;
} XFIBER, *PXFIBER;

#endif // _XFIBER_H
