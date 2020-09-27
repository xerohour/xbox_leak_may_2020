/*++

Copyright (c) 1985-2000  Microsoft Corporation

Module Name:

    mmtimer.c

Abstract:

    This module implements the Win32 multimedia timer services.

--*/

#include "basedll.h"

//
// Define the maximum number of multimedia timers allowed.
//

#define MAXIMUM_MMTIMERS                (MAXIMUM_WAIT_OBJECTS - 1)

//
// Structure used to bind the parameters from timeSetEvent to its corresponding
// kernel timer object.
//

#include <pshpack4.h>

typedef struct _MMTIMER {
    KTIMER KernelTimer;
    LPTIMECALLBACK lpTimeCallback;
    DWORD dwTimeCallbackData;
    BOOLEAN Allocated;
    UCHAR Flags;
    USHORT SerialNumber;
    LARGE_INTEGER FireTime;
    LARGE_INTEGER Interval;
} MMTIMER, *PMMTIMER;

#include <poppack.h>

//
// Structure used to bind all of the timer state that is allocated on the timer
// thread's stack.
//

typedef struct _MMTIMER_THREAD_STATE {
    MMTIMER Timers[MAXIMUM_MMTIMERS];
    KEVENT ThreadEvent;
} MMTIMER_THREAD_STATE, *PMMTIMER_THREAD_STATE;

INITIALIZED_CRITICAL_SECTION(XapiMmTimerCriticalSection);
USHORT XapiMmTimerSerialNumber;
PMMTIMER_THREAD_STATE XapiMmTimerThreadState;

DWORD
WINAPI
XapiMmTimerThread(
    LPVOID lpThreadParameter
    )
{
    ULONG Index;
    MMTIMER_THREAD_STATE MmTimerThreadState;
    KWAIT_BLOCK WaitBlocks[MAXIMUM_WAIT_OBJECTS];
    PVOID WaitObjects[MAXIMUM_WAIT_OBJECTS];
    USHORT SerialNumbers[MAXIMUM_WAIT_OBJECTS];
    ULONG WaitObjectCount;
    NTSTATUS status;
    PMMTIMER MmTimer;
    LPTIMECALLBACK lpTimeCallback;
    DWORD dwTimeCallbackData;
    UINT uTimerID;
    UINT fuEvent;
    LARGE_INTEGER DueTime;

    //
    // Initialize the event used to signal that this thread needs to rebuild its
    // wait object array.
    //

    KeInitializeEvent(&MmTimerThreadState.ThreadEvent, SynchronizationEvent,
        FALSE);

    //
    // Initialize the kernel timers.
    //

    for (Index = 0; Index < MAXIMUM_MMTIMERS; Index++) {
        KeInitializeTimerEx(&MmTimerThreadState.Timers[Index].KernelTimer,
            SynchronizationTimer);
        MmTimerThreadState.Timers[Index].Allocated = FALSE;
    }

    //
    // The thread's state is now initialized.  Notify the thread that created us
    // that we're ready.
    //

    XapiMmTimerThreadState = &MmTimerThreadState;

    KeSetEvent((PKEVENT)lpThreadParameter, EVENT_INCREMENT, FALSE);

    //
    // Initially, we wait on only our thread event.
    //

    WaitObjects[0] = &MmTimerThreadState.ThreadEvent;
    WaitObjectCount = 1;

    for (;;) {

        //
        // Wait to be notified of the arrival or removal of a timer or for a
        // timer to be signaled.
        //

        status = KeWaitForMultipleObjects(WaitObjectCount, WaitObjects, WaitAny,
            UserRequest, UserMode, FALSE, NULL, WaitBlocks);

        if (status == STATUS_WAIT_0) {

            //
            // We've been signaled due to an addition or removal of a timer
            // from our thread state structure.  Recompute the local wait object
            // array.
            //

            EnterCriticalSection(&XapiMmTimerCriticalSection);

RecomputeWaitObjects:
            WaitObjectCount = 1;

            for (Index = 0; Index < MAXIMUM_WAIT_OBJECTS - 1; Index++) {

                //
                // If the timer has been allocated, add it to the wait object
                // array and capture the timer's serial number.  We need the
                // serial number to deal with a race where this thread is woken
                // in response to a signaled timer and another thread has
                // already called timeKillEvent.
                //

                if (MmTimerThreadState.Timers[Index].Allocated) {
                    SerialNumbers[WaitObjectCount] =
                        MmTimerThreadState.Timers[Index].SerialNumber;
                    WaitObjects[WaitObjectCount] =
                        &MmTimerThreadState.Timers[Index].KernelTimer;
                    WaitObjectCount++;
                }
            }

            LeaveCriticalSection(&XapiMmTimerCriticalSection);

        } else if (status >= STATUS_WAIT_1 && status < (STATUS_WAIT_1 +
            MAXIMUM_MMTIMERS)) {

            //
            // We've been signaled due to a timer firing.  Verify that the timer
            // is still allocated and that the serial number matches what we
            // think it should be.
            //

            EnterCriticalSection(&XapiMmTimerCriticalSection);

            MmTimer = CONTAINING_RECORD(WaitObjects[status], MMTIMER, KernelTimer);

            if (MmTimer->Allocated &&
                (MmTimer->SerialNumber == SerialNumbers[status])) {

                //
                // The timer is still allocated.  Depending on the timer flags,
                // either set an event, pulse an event, or invoke a callback.
                //

                fuEvent = (UINT)MmTimer->Flags;

                if ((fuEvent & TIME_CALLBACK_EVENT_SET) != 0) {

                    NtSetEvent((HANDLE)MmTimer->lpTimeCallback, NULL);

                } else if ((fuEvent & TIME_CALLBACK_EVENT_PULSE) != 0) {

                    NtPulseEvent((HANDLE)MmTimer->lpTimeCallback, NULL);

                } else {

                    //
                    // Capture the timer data to the stack so that we can invoke
                    // the callback outside of the critical section.
                    //

                    lpTimeCallback = MmTimer->lpTimeCallback;
                    dwTimeCallbackData = MmTimer->dwTimeCallbackData;
                    Index = MmTimer - &MmTimerThreadState.Timers[0];
                    uTimerID = MAKELONG(Index + 1, MmTimer->SerialNumber);

                    LeaveCriticalSection(&XapiMmTimerCriticalSection);

                    //
                    // Invoke the callback.  The callback or another thread may
                    // end up calling timeKillEvent for our timer ID so we can't
                    // touch the MMTIMER structure until we validate it again.
                    //

                    lpTimeCallback(uTimerID, 0, dwTimeCallbackData, 0, 0);

                    //
                    // Reenter the critical section and verify that the timer
                    // hasn't been killed.
                    //

                    EnterCriticalSection(&XapiMmTimerCriticalSection);

                    if (!MmTimer->Allocated ||
                        (MmTimer->SerialNumber != SerialNumbers[status])) {
                        LeaveCriticalSection(&XapiMmTimerCriticalSection);
                        continue;
                    }
                }

                if ((fuEvent & TIME_PERIODIC) != 0) {

                    //
                    // Initialize the timer with its new due time.  If the timer
                    // has already expired, then set the due time to zero so
                    // that we can immediately wake up and process the timer
                    // again.
                    //

                    MmTimer->FireTime.QuadPart -= MmTimer->Interval.QuadPart;

                    DueTime.QuadPart = KeQueryInterruptTime() -
                        MmTimer->FireTime.QuadPart;

                    if (DueTime.QuadPart > 0) {
                        DueTime.QuadPart = 0;
                    }

                    KeSetTimerEx(&MmTimer->KernelTimer, DueTime, 0, NULL);

                } else {

                    //
                    // Mark the timer as unallocated.
                    //

                    MmTimer->Allocated = FALSE;

                    //
                    // Jump into the code to recompute the wait objects array.
                    //

                    goto RecomputeWaitObjects;
                }
            }

            LeaveCriticalSection(&XapiMmTimerCriticalSection);

        } else {
            XDBGERR("XAPI", "XapiMmTimerThread woke for unexpected reason: %08x\n", status);
        }
    }
}

UINT
WINAPI
timeSetEvent(
    UINT uDelay,
    UINT uResolution,
    LPTIMECALLBACK lpTimeCallback,
    DWORD dwTimeCallbackData,
    UINT fuEvent
    )
{
    KEVENT ThreadReadyEvent;
    HANDLE hThread;
    ULONG Index;
    UINT uTimerID;
    PMMTIMER MmTimer;
    USHORT SerialNumber;

    EnterCriticalSection(&XapiMmTimerCriticalSection);

    //
    // Check if the multimedia timer thread has been started yet.
    //

    if (XapiMmTimerThreadState == NULL) {

        //
        // Create an event that the multimedia timer thread will signal when it
        // has finished initializating itself.
        //

        KeInitializeEvent(&ThreadReadyEvent, NotificationEvent, FALSE);

        //
        // Create the multimedia timer thread with the default stack size for
        // the title.
        //

        hThread = CreateThread(NULL, 0, XapiMmTimerThread, &ThreadReadyEvent, 0,
            NULL);

        if (hThread == NULL) {
            XDBGERR("XAPI", "XapiMmTimerThread cannot be created\n");
            LeaveCriticalSection(&XapiMmTimerCriticalSection);
            return 0;
        }

        CloseHandle(hThread);

        //
        // Wait for the multimedia timer thread to initialize itself.
        //

        KeWaitForSingleObject(&ThreadReadyEvent, Executive, UserMode, FALSE,
            NULL);

        //
        // At this point, the global timer thread state should have been filled
        // in.
        //

        ASSERT(XapiMmTimerThreadState != NULL);
    }

    uTimerID = 0;

    for (Index = 0; Index < MAXIMUM_WAIT_OBJECTS - 1; Index++) {

        if (!XapiMmTimerThreadState->Timers[Index].Allocated) {

            SerialNumber = XapiMmTimerSerialNumber++;

            MmTimer = &XapiMmTimerThreadState->Timers[Index];

            MmTimer->Allocated = TRUE;
            MmTimer->Flags = (UCHAR)fuEvent;
            MmTimer->SerialNumber = SerialNumber;
            MmTimer->lpTimeCallback = lpTimeCallback;
            MmTimer->dwTimeCallbackData = dwTimeCallbackData;

            //
            // Compute the initial due time for the timer and start the timer.
            //
            // We track the fire time and period interval ourselves instead of
            // using the period functionality of the kernel timer to avoid
            // timer drift that can occur.
            //

            MmTimer->Interval.QuadPart = -10000 * (LONGLONG)uDelay;

            MmTimer->FireTime.QuadPart = KeQueryInterruptTime() -
                MmTimer->Interval.QuadPart;

            KeSetTimerEx(&MmTimer->KernelTimer, MmTimer->Interval, 0, NULL);

            //
            // Signal the multimedia timer thread that it needs to add a timer
            // to its wait list.
            //

            KeSetEvent(&XapiMmTimerThreadState->ThreadEvent, EVENT_INCREMENT,
                FALSE);

            uTimerID = MAKELONG(Index + 1, SerialNumber);
            break;
        }
    }

    LeaveCriticalSection(&XapiMmTimerCriticalSection);

    return uTimerID;
}

MMRESULT
WINAPI
timeKillEvent(
    UINT uTimerID
    )
{
    MMRESULT mmresult;
    ULONG Index;
    PMMTIMER MmTimer;

    EnterCriticalSection(&XapiMmTimerCriticalSection);

    Index = LOWORD(uTimerID) - 1;

    //
    // Validate that this is a legal timer identifier.  The timer thread must be
    // already running in order to have obtained a valid identifier from
    // timeSetEvent.  The timer identifier must be in legal range and the timer
    // must be marked as having been allocated.
    //

    if ((XapiMmTimerThreadState != NULL) && (Index < MAXIMUM_MMTIMERS) &&
        XapiMmTimerThreadState->Timers[Index].Allocated &&
        XapiMmTimerThreadState->Timers[Index].SerialNumber == HIWORD(uTimerID)) {

        //
        // Cancel the timer and mark the timer as unallocated.
        //

        MmTimer = &XapiMmTimerThreadState->Timers[Index];

        KeCancelTimer(&MmTimer->KernelTimer);

        MmTimer->Allocated = FALSE;

        //
        // Signal the multimedia timer thread that it needs to remove a timer
        // from its wait list.
        //

        KeSetEvent(&XapiMmTimerThreadState->ThreadEvent, EVENT_INCREMENT, FALSE);

        mmresult = MMSYSERR_NOERROR;

    } else {
        mmresult = TIMERR_NOCANDO;
    }

    LeaveCriticalSection(&XapiMmTimerCriticalSection);

    return mmresult;
}

MMRESULT
WINAPI
timeGetSystemTime(
    LPMMTIME lpTime,
    UINT wSize
    )
{
    lpTime->u.ms = NtGetTickCount();
    lpTime->wType = TIME_MS;

    return TIMERR_NOERROR;
}
