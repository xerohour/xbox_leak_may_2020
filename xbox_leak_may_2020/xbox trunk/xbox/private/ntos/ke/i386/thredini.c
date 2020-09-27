/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    thredini.c

Abstract:

    This module implements the machine dependent function to set the initial
    context and data alignment handling mode for a process or thread object.

Author:

    David N. Cutler (davec) 31-Mar-1990

Environment:

    Kernel mode only.

Revision History:

    3 April 90  bryan willman

        This version ported to 386.

--*/

#include "ki.h"

VOID
KiInitializeContextThread (
    IN PKTHREAD Thread,
    IN SIZE_T TlsDataSize,
    IN PKSYSTEM_ROUTINE SystemRoutine,
    IN PKSTART_ROUTINE StartRoutine OPTIONAL,
    IN PVOID StartContext OPTIONAL
    )

/*++

Routine Description:

    This function initializes the machine dependent context of a thread object.

Arguments:

    Thread - Supplies a pointer to a dispatcher object of type thread.

    TlsDataSize - Supplies the number of bytes reserved from the kernel stack
        for thread local storage.

    SystemRoutine - Supplies a pointer to the system function that is to be
        called when the thread is first scheduled for execution.

    StartRoutine - Supplies an optional pointer to a function that is to be
        called after the system has finished initializing the thread. This
        parameter is specified if the thread is a system thread and will
        execute totally in kernel mode.

    StartContext - Supplies an optional pointer to an arbitrary data structure
        which will be passed to the StartRoutine as a parameter. This
        parameter is specified if the thread is a system thread and will
        execute totally in kernel mode.

Return Value:

    None.

--*/

{
    PFX_SAVE_AREA NpxFrame;
    PVOID TlsData;
    PKSWITCHFRAME SwitchFrame;
    PULONG PSystemRoutine;
    PULONG PStartRoutine;
    PULONG PStartContext;

    //
    // Load up an initial NPX state.
    //

    NpxFrame = (PFX_SAVE_AREA)(((ULONG)(Thread->StackBase) -
                sizeof(FX_SAVE_AREA)));

    RtlZeroMemory((PVOID)NpxFrame, sizeof(FX_SAVE_AREA));

    NpxFrame->FloatSave.ControlWord = 0x27f;     //like fpinit but 64bit mode
    NpxFrame->FloatSave.MXCsr       = 0x1f80;    // mask all the exceptions

    //
    // Thread's NPX state is not in the coprocessor.
    //

    Thread->NpxState = NPX_STATE_NOT_LOADED;

    //
    // Zero the thread local storage area.
    //

    TlsDataSize = ALIGN_UP(TlsDataSize, ULONG);
    TlsData = ((PUCHAR)(ULONG)NpxFrame) - TlsDataSize;

    if (TlsDataSize != 0) {
        Thread->TlsData = TlsData;
        RtlZeroMemory(TlsData, TlsDataSize);
    } else {
        Thread->TlsData = NULL;
    }

    //
    // Space for arguments to KiThreadStartup.  Order of fields in the
    // switchframe is important, since args are passed on stack through
    // KiThreadStartup to PStartRoutine with PStartContext as an argument.
    //

    PStartContext = (PULONG)((ULONG)TlsData) - 1;
    PStartRoutine = PStartContext - 1;
    PSystemRoutine = PStartRoutine - 1;

    SwitchFrame = (PKSWITCHFRAME)((PUCHAR)PSystemRoutine - sizeof(KSWITCHFRAME));

    //
    // Set up thread start parameters.
    //

    *PStartContext = (ULONG)StartContext;
    *PStartRoutine = (ULONG)StartRoutine;
    *PSystemRoutine = (ULONG)SystemRoutine;

    //
    // Set up switch frame.
    //

    SwitchFrame->RetAddr = (ULONG)KiThreadStartup;

    SwitchFrame->Eflags = EFLAGS_INTERRUPT_MASK;

    SwitchFrame->ExceptionList = (ULONG)(EXCEPTION_CHAIN_END);

    //
    // Set the initial kernel stack pointer.
    //

    Thread->KernelStack = (PVOID)SwitchFrame;
    return;
}
