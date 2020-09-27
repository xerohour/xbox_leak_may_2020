/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    fiber.c

Abstract:

    This module implements the Win32 fiber services.

--*/

#include "basedll.h"
#pragma hdrstop
#include "dm.h"

//
// Per thread fiber data pointer.
//
__declspec(thread) LPVOID XapiCurrentFiber;

//
// For threads that have been converted to fibers, this per thread structure
// holds its fiber data.
//
__declspec(thread) XFIBER XapiThreadFiberData;

VOID
XapiFiberStartup(
    LPFIBER_START_ROUTINE lpStartAddress
    )
{
    __try {

        lpStartAddress(GetFiberData());

    } __except (UnhandledExceptionFilter(GetExceptionInformation())) {

        //
        // UnhandledExceptionFilter will return either EXCEPTION_CONTINUE_SEARCH,
        // in which case, the exception search will stop since we're the top of
        // the exception stack, or it will return EXCEPTION_CONTINUE_EXECUTION.
        // We'll never execute this handler.
        //

        ASSERT(FALSE);
    }

    RIP("Fiber should not return.");

    KeBugCheck(0);
}

LPVOID
WINAPI
CreateFiber(
    IN DWORD dwStackSize,
    IN LPFIBER_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter
    )
{
    PVOID StackBase;
    PXFIBER Fiber;
    PULONG_PTR Context;

    //
    // Allocate the stack for the fiber.
    //

    if (dwStackSize == 0) {
        dwStackSize = XeImageHeader()->SizeOfStackCommit;
    }

    if (dwStackSize < KERNEL_STACK_SIZE) {
        dwStackSize = KERNEL_STACK_SIZE;
    }

    dwStackSize = ROUND_TO_PAGES(dwStackSize);

    StackBase = MmCreateKernelStack(dwStackSize, FALSE);

    if (StackBase == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }

    //
    // Allocate the fiber structure at the top of the fiber stack.
    //

    Fiber = (PXFIBER)StackBase - 1;

    //
    // Initialize the fiber state.
    //

    Fiber->FiberData = lpParameter;
    Fiber->StackBase = StackBase;
    Fiber->StackLimit = (PUCHAR)StackBase - dwStackSize;

    //
    // Initialize the start context for the fiber.  The start context consists
    // of the parameter to XapiFiberStartup, a dummy return address for the
    // 'call' to XapiFiberStartup (we use XapiFiberStartup itself in order to
    // make the kernel debugger happy), a dummy parameter to SwitchToFiber, the
    // return address for SwitchToFiber, and the initial non-volatile context.
    //

    Context = (PULONG_PTR)Fiber - 1;

    *(Context--) = (ULONG_PTR)lpStartAddress;
    *(Context--) = (ULONG_PTR)XapiFiberStartup;
    *(Context--) = 0;
    *(Context--) = (ULONG_PTR)XapiFiberStartup;
    *(Context--) = 0;
    *(Context--) = 0;
    *(Context--) = 0;
    *(Context--) = 0;
    *(Context) = (ULONG_PTR)EXCEPTION_CHAIN_END;

    Fiber->KernelStack = Context;

#if 0
    // Tell the debugger we've created the fiber
    if(KeGetCurrentPrcb()->DebugMonitorData) {
        _asm {
            mov eax, BREAKPOINT_CREATE_FIBER
            mov ecx, Fiber
            mov edx, lpStartAddress
            int 2dh
            int 3
        }
    }
#endif

    return Fiber;
}

VOID
WINAPI
DeleteFiber(
    IN LPVOID lpFiber
    )
{
    PXFIBER Fiber;

    RIP_ON_NOT_TRUE("DeleteFiber()", (lpFiber != NULL));

    Fiber = (PXFIBER)lpFiber;

    //
    // Unlike Win32, we cannot delete the currently executing fiber.  Win32
    // would call ExitThread here, but if we do that, then that doesn't end up
    // freeing the fiber's stack, but ends up killing a thread that called
    // ConvertThreadToFiber.
    //

    if (Fiber->KernelStack == NULL) {
        RIP("DeleteFiber() - Cannot delete an active fiber.");
    }

    //
    // Fibers created from ConvertThreadToFiber use the stack of the calling
    // thread.  We can't delete this fiber without destroying the thread.  We
    // could choose to delete just the fiber structure, but that alters the
    // behavior of the original API too much.
    //

    if (Fiber->StackLimit == NULL) {
        RIP("DeleteFiber() - Cannot delete a fiber from ConvertThreadToFiber.");
    }

#if 0
    // Tell the debugger this fiber is going away
    if(KeGetCurrentPrcb()->DebugMonitorData) {
        _asm {
            mov eax, BREAKPOINT_DELETE_FIBER
            mov ecx, Fiber
            int 2dh
            int 3
        }
    }
#endif

    //
    // Delete the fiber's stack.  The fiber structure is allocated as a part of
    // the stack, so Fiber is not valid after this point.
    //

    MmDeleteKernelStack(Fiber->StackBase, Fiber->StackLimit);
}

LPVOID
WINAPI
ConvertThreadToFiber(
    IN LPVOID lpParameter
    )
{
    PXFIBER Fiber;

    //
    // Check that the current thread isn't already hosting a fiber.
    //

    if (XapiCurrentFiber != NULL) {
        RIP("ConvertThreadToFiber() - Cannot convert a thread to a fiber multiple times.");
    }

    //
    // Initialize the fiber state.
    //

    Fiber = &XapiThreadFiberData;

    Fiber->FiberData = lpParameter;
    Fiber->StackBase = KeGetCurrentThread()->StackBase;
    Fiber->StackLimit = NULL;

#if 0
    // Tell the debugger how to find a fiber
    DmTell(DMTELL_FIBERTLS, (PVOID)((ULONG_PTR)&XapiCurrentFiber -
        (ULONG_PTR)KeGetCurrentThread()->TlsData));

    // Tell the debugger we've created the fiber
    if(KeGetCurrentPrcb()->DebugMonitorData) {
        _asm {
            mov eax, BREAKPOINT_CREATE_FIBER
            mov ecx, Fiber
            xor edx, edx
            int 2dh
            int 3
        }
    }
#endif

    //
    // Make this fiber the current fiber for the thread.
    //

    XapiCurrentFiber = Fiber;

    return Fiber;
}

__declspec(naked)
VOID
WINAPI
#if DBG
XapiSwitchToFiberRetail(
#else
SwitchToFiber(
#endif
    LPVOID lpFiber
    )
{
    __asm {

        //
        // Fetch the offset to the TLS data and the base pointer to the TLS
        // data.
        //

        mov     edx,_tls_index
        mov     ecx,fs:[NT_TIB.StackBase]

        //
        // Load the pointer to the new fiber.
        //

        mov     eax,[esp+4]

        //
        // Save the old fiber's nonvolatile state and exception list.
        //

        push    ebp
        push    esi
        push    edi
        push    ebx
        push    DWORD PTR fs:[NT_TIB.ExceptionList]

        //
        // Load the pointer to the old fiber from XapiCurrentFiber.
        //

        mov     edx,[ecx+edx*4]
        mov     ecx,XapiCurrentFiber[edx]

        //
        // Switch from the old fiber's stack to the new fiber's stack.
        //

        mov     [ecx+XFIBER.KernelStack],esp
        mov     esp,[eax+XFIBER.KernelStack]

#if DBG
        //
        // Null out the kernel stack pointer so that we can use this as a flag
        // in SwitchToFiber to detect a fiber that's already running.
        //

        mov     DWORD PTR [eax+XFIBER.KernelStack], 0
#endif

        //
        // Update XapiCurrentFiber to point at the new fiber.
        //

        mov     XapiCurrentFiber[edx],eax

        //
        // Restore the new fiber's nonvolatile state and exception list.
        //

        pop     DWORD PTR fs:[NT_TIB.ExceptionList]
        pop     ebx
        pop     edi
        pop     esi
        pop     ebp

        ret     4
    }
}

#if DBG

VOID
WINAPI
SwitchToFiber(
    LPVOID lpFiber
    )
{
    PXFIBER Fiber;

    RIP_ON_NOT_TRUE("SwitchToFiber()", (lpFiber != NULL));

    //
    // Check that the current thread has called ConvertThreadToFiber.
    //

    if (XapiCurrentFiber == NULL) {
        RIP("SwitchToFiber() - Thread hasn't called ConvertThreadToFiber().");
    }

    //
    // Check that the fiber isn't already running on another thread.  We'll
    // allow a fiber to switch to itself on the same thread (Win32 documents
    // this as "unpredictable".)
    //

    Fiber = (PXFIBER)lpFiber;

    if ((Fiber->KernelStack == NULL) && (XapiCurrentFiber != Fiber)) {
        RIP("SwitchToFiber() - Fiber is already active on another thread.");
    }

    //
    // Jump to the retail version of this routine.
    //

    XapiSwitchToFiberRetail(lpFiber);
}

#endif
