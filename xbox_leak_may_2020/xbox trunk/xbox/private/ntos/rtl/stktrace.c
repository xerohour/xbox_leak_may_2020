/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    stktrace.c

Abstract:

    This module implements routines to snapshot a set of stack back traces
    in a data base.  Useful for heap allocators to track allocation requests
    cheaply.

Author:

    Steve Wood (stevewo) 29-Jan-1992

Revision History:

    17-May-1999 (silviuc) : added RtlWalkFrameChain that replaces the
    unsafe RtlCaptureStackBackTrace.

--*/

#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>

USHORT
RtlCaptureStackBackTrace(
    IN ULONG FramesToSkip,
    IN ULONG FramesToCapture,
    OUT PVOID *BackTrace,
    OUT PULONG BackTraceHash
    )
/*++

Routine Description:

    This routine walks up the stack frames, capturing the return address from
    each frame requested. This used to be implemented in assembly language and
    used to be unsafe in special contexts (DPC level). Right now it uses
    RtlWalkFrameChain that validates the chain of pointers and it guarantees
    not to take exceptions.

Arguments:

    FramesToSkip - frames detected but not included in the stack trace

    FramesToCapture - frames to be captured in the stack trace buffer. 
        One of the frames will be for RtlCaptureStackBackTrace.

    BackTrace - stack trace buffer

    BackTraceHash - very simple hash value that can be used to organize
      hash tables. It is just an arithmetic sum of the pointers in the
      stack trace buffer.

Return Value:

     Number of return addresses returned in the stack trace buffer.

--*/
{
    PVOID Trace [2 * MAX_STACK_DEPTH];
    ULONG FramesFound;
    ULONG HashValue;
    ULONG Index;

    //
    // One more frame to skip for the "capture" function (WalkFrameChain).
    //

    FramesToSkip++;

    //
    // Sanity checks.
    //

    if (FramesToCapture + FramesToSkip >= 2 * MAX_STACK_DEPTH) {
        return 0;
    }

    FramesFound = RtlWalkFrameChain (
        Trace,
        FramesToCapture + FramesToSkip,
        0);

    if (FramesFound <= FramesToSkip) {
        return 0;
    }

    for (HashValue = 0, Index = 0; Index < FramesToCapture; Index++) {

        if (FramesToSkip + Index >= FramesFound) {
            break;
        }

        BackTrace[Index] = Trace[FramesToSkip + Index];
        HashValue += PtrToUlong(BackTrace[Index]);
    }

    *BackTraceHash = HashValue;
    return (USHORT)Index;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// RtlWalkFrameChain
/////////////////////////////////////////////////////////////////////

//
// This section contains an algorithm for getting stack traces.
// It works only on x86. It is an improvement of
// RtlCaptureStackBackTrace which for reasons that escape me is written
// in assembly and it is unsafe (can raise exceptions). The new function
// RtlWalkFrameChain is guaranteed to not take exceptions whatever the
// call context.
//
// Note. It might be a good idea to not BBT this code. Especially I am concerned
// about the only assembly instruction used in the whole code that saves
// the value of the EBP register.
//

#ifdef NTOS_KERNEL_RUNTIME
#define _KERNEL_MODE_STACK_TRACES_       1
#define _COLLECT_FRAME_WALK_STATISTICS_  0
#else
#define _KERNEL_MODE_STACK_TRACES_       0
#define _COLLECT_FRAME_WALK_STATISTICS_  0
#endif

#define SIZE_1_KB  ((ULONG_PTR) 0x400)
#define SIZE_1_GB  ((ULONG_PTR) 0x40000000)

#define PAGE_START(address) (((ULONG_PTR)address) & ~((ULONG_PTR)PAGE_SIZE - 1))

VOID CollectFrameWalkStatistics (ULONG Index);

#if (( i386 ) && ( FPO ))
#pragma optimize( "y", off )    // disable FPO for consistent stack traces
#endif

ULONG
RtlWalkFrameChain (
    OUT PVOID *Callers,
    IN ULONG Count,
    IN ULONG Flags)

/*++

Routine Description:

    RtlWalkFrameChain

Description:

    This function tries to walk the EBP chain and fill out a vector of
    return addresses. The function works only on x86. It is possible that
    the function cannot fill the requested number of callers because somewhere
    on the stack we have a function compiled FPO (the frame register (EBP) is
    used as a normal register. In this case the function will just return with
    a less then requested count. In kernel mode the function should not take
    any exceptions (page faults) because it can be called at all sorts of
    irql levels.

    The `Flags' parameter is used for future extensions. A zero value will be
    compatible with new stack walking algorithms.

    Note. The algorithm can be somewhat improved by unassembling the return
    addresses identified. However this is impractical in kernel mode because
    the function might get called at high irql levels where page faults are
    not allowed.

Return value:

    The number of identified return addresses on the stack. This can be less
    then the Count requested if the stack ends or we encounter a FPO compiled
    function.

--*/

{
#if defined(_X86_)

    ULONG_PTR Fp, NewFp, ReturnAddress;
    ULONG Index;
    ULONG_PTR StackEnd, StackStart;
    BOOLEAN Result;

    //
    // Get the current EBP pointer which is supposed to
    // be the start of the EBP chain.
    //

    _asm mov Fp, EBP;

    StackStart = Fp;

#if _KERNEL_MODE_STACK_TRACES_

    StackEnd = (ULONG_PTR)(KeGetCurrentThread()->StackBase);

    //
    // bugbug: find a reliable way to get the stack limit in kernel mode.
    // `StackBase' is not a reliable way to get the stack end in kernel
    // mode because we might execute a DPC routine on thread's behalf.
    // There are a few other reasons why we cannot trust this completely.
    //
    // Note. The condition `PAGE_START(StackEnd) - PAGE_START(StackStart) > PAGE_SIZE'
    // is not totally safe. We can encounter a situation where in this case we
    // do not have the same stack. Can we?
    //
    // The DPC stack is actually the stack of the idle thread corresponding to
    // the current processor. Based on that we probably can figure out in almost
    // all contexts what are the real limits of the stack.
    //

    if ((StackStart > StackEnd)
        || (PAGE_START(StackEnd) - PAGE_START(StackStart) > PAGE_SIZE)) {

        StackEnd = (StackStart + PAGE_SIZE) & ~((ULONG_PTR)PAGE_SIZE - 1);
    
        //
        // Try to get one more page if possible. Note that this is not
        // 100% reliable because a non faulting address can fault if
        // appropriate locks are not held.
        //

        if (MmIsAddressValid ((PVOID)StackEnd)) {
            StackEnd += PAGE_SIZE;
        }
    }

#else

    StackEnd = (ULONG_PTR)(NtCurrentTeb()->NtTib.StackBase);

#endif // #if _KERNEL_MODE_STACK_TRACES_

    try {

        for (Index = 0; Index < Count; Index++) {

            if (Fp + sizeof(ULONG_PTR) >= StackEnd) {
                break;
            }

            NewFp = *((PULONG_PTR)(Fp + 0));
            ReturnAddress = *((PULONG_PTR)(Fp + sizeof(ULONG_PTR)));

            //
            // Figure out if the new frame pointer is ok. This validation
            // should avoid all exceptions in kernel mode because we always
            // read within the current thread's stack and the stack is
            // guaranteed to be in memory (no page faults). It is also guaranteed
            // that we do not take random exceptions in user mode because we always
            // keep the frame pointer within stack limits.
            //

            if (! (Fp < NewFp && NewFp < StackEnd)) {
                break;
            }

            //
            // Figure out if the return address is ok. If return address
            // is a stack address or <64k then something is wrong. There is
            // no reason to return garbage to the caller therefore we stop.
            //

            if (StackStart < ReturnAddress && ReturnAddress < StackEnd) {
                break;
            }

            if (ReturnAddress < 64 * SIZE_1_KB) {
                break;
            }

            //
            // Store new fp and return address and move on.
            //

            Fp = NewFp;
            Callers[Index] = (PVOID)ReturnAddress;
        }
    }
    except (EXCEPTION_EXECUTE_HANDLER) {

        //
        // The frame traversal algorithm is written so that we should
        // not get any exception. Therefore if we get some exception
        // we better debug it.
        //

#if DBG
        DbgPrint ("Unexpected exception in RtlWalkFrameChain ...\n");
        DbgBreakPoint ();
#endif
    }

    //
    // Return the number of return addresses identified on the stack.
    //

#if _COLLECT_FRAME_WALK_STATISTICS_
    CollectFrameWalkStatistics (Index);
#endif // #if _COLLECT_FRAME_WALK_STATISTICS_

    return Index;

#else

    return 0;

#endif // #if defined(_X86_)
}


#if _COLLECT_FRAME_WALK_STATISTICS_

KSPIN_LOCK FrameWalkStatisticsLock;
ULONG FrameWalkStatisticsCounters [32];
ULONG FrameWalkCollectStatisticsCalls;
BOOLEAN FrameWalkStatisticsInitialized;

VOID
CollectFrameWalkStatistics (

    ULONG Index)

/*++

Routine description:

    CollectFrameWalkStatistics

Description:

    This function computes the distribution of detectable chain
    lengths. This is used only for debugging the frame traversal
    algorithm. It proves that it is worth trying to get stack
    traces on optimized images because only about 8% of the calls
    cannot be resolved to more than two callers. A sample distribution
    computed by calling RtlWalkFrameChain for every call to
    ExAllocatePoolWithTag gave the results below:

         Length       Percentage
         0-2          5%
         3-5          20%
         6-10         40%
         10-16        35%

    With a failure rate of 5% it is worth using it.

--*/

{
    KIRQL PreviousIrql;
    ULONG I;
    ULONG Percentage;
    ULONG TotalPercentage;

    //
    // Spin lock initialization is not safe in the code below
    // but this code is used only during frame walk algorithm
    // development so there is no reason to make it bulletproof.
    //

    if (! FrameWalkStatisticsInitialized) {
        KeInitializeSpinLock (&FrameWalkStatisticsLock);
        FrameWalkStatisticsInitialized = TRUE;
    }

    KeAcquireSpinLock (
        &FrameWalkStatisticsLock,
        &PreviousIrql);

    FrameWalkCollectStatisticsCalls++;

    if (Index < 32) {
        FrameWalkStatisticsCounters[Index]++;
    }

    if (FrameWalkCollectStatisticsCalls != 0
        && (FrameWalkCollectStatisticsCalls % 60000 == 0)) {

        DbgPrint ("FrameWalk: %u calls \n", FrameWalkCollectStatisticsCalls);

        TotalPercentage = 0;

        for (I = 0; I < 32; I++) {

            Percentage = FrameWalkStatisticsCounters[I] * 100
                / FrameWalkCollectStatisticsCalls;

            DbgPrint ("FrameWalk: [%02u] %02u \n", I, Percentage);

            TotalPercentage += Percentage;
        }

        DbgPrint ("FrameWalk: total %u \n", TotalPercentage);
        DbgBreakPoint ();
    }

    KeReleaseSpinLock (
        &FrameWalkStatisticsLock,
        PreviousIrql);
}

#endif // #if _COLLECT_FRAME_WALK_STATISTICS_

