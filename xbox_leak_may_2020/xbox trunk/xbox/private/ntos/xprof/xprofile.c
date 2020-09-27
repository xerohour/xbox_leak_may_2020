/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xprofile.c

Abstract:

    Kernel profiler functions

--*/

#ifdef _XBOX_ENABLE_PROFILING

#include "ntos.h"
#include "xprofp.h"

//
// Global data structures maintained by the profiler
//
XProfpGlobals XProfpData;

XProfpGlobals* XProfpGetData() {
    return &XProfpData;
}


VOID __declspec(naked) __stdcall
_CAP_Start_Profiling(
    VOID* caller,
    VOID* callee
    )

/*++

Routine Description:

    Just before a function is about to be called

Arguments:

    caller - Specifies the caller function
    callee - Specifies the called function

Return Value:

    NONE

--*/

{
    /*
        ULONG* buf;

    loop:
        buf = XProfpData.bufnext
        if buf >= XProfpData.bufend, return
        atomic incremnt of XProfpData.bufnext by 4 ULONGs
        if failed, goto loop

        buf[0] = timestamp-high
        buf[1] = timestamp-low
        buf[2] = caller
        buf[3] = callee

     */

    __asm {
        push    eax
        push    ecx
        push    edx
        lea     ecx, XProfpData
        mov     eax, [ecx]          ; eax = XProfpData.bufnext
    loop1:
        cmp     eax, [ecx+4]        ; XProfpData.bufnext >= XProfpData.bufend?
        jae     ret1
        lea     edx, [eax+16]       ; XProfpData.bufnext += 4
        cmpxchg [ecx], edx
        jnz     loop1

        mov     ecx, eax
        rdtsc
        mov     [ecx], edx          ; timestamp-high
        mov     [ecx+4], eax        ; timestamp-low
        mov     eax, [esp+16]       ; caller - 3 pushes + return address
        mov     [ecx+8], eax
        mov     eax, [esp+20]       ; callee
        mov     [ecx+12], eax

    ret1:
        pop     edx
        pop     ecx
        pop     eax
        ret     8
    }
}


VOID __declspec(naked) __stdcall
_CAP_End_Profiling(
    VOID* caller
    )
/*++

Routine Description:

    Just after a function call is made

Arguments:

    caller - Specifies the caller function

Return Value:

    NONE

--*/

{
    // Same logic as above, except the record is smaller by 1 ULONG

    __asm {
        push    eax
        push    ecx
        push    edx
        lea     ecx, XProfpData
        mov     eax, [ecx]          ; eax = XProfpData.bufnext
    loop1:
        cmp     eax, [ecx+4]        ; XProfpData.bufnext >= XProfpData.bufend?
        jae     ret1
        lea     edx, [eax+12]       ; XProfpData.bufnext += 3
        cmpxchg [ecx], edx
        jnz     loop1

        mov     ecx, eax
        rdtsc
        bts     edx, 29             ; timestamp-high | XPROFREC_CALL_END
        mov     [ecx], edx
        mov     [ecx+4], eax        ; timestamp-low
        mov     eax, [esp+16]       ; caller
        mov     [ecx+8], eax

    ret1:
        pop     edx
        pop     ecx
        pop     eax
        ret     4
    }
}


VOID __declspec(naked) __stdcall
XProfpEmitControlRecord(
    ULONG type,
    ULONG param
    )

/*++

Routine Description:

    Emit a CAP control record

Arguments:

    type - Specifies the control record type
    param - Type-specific parameter

Return Value:

    NONE

--*/

{
    // Identical to _CAP_Start_Profiling except
    // the record is marked as XPROFREC_CONTROL.

    __asm {
        push    eax
        push    ecx
        push    edx
        lea     ecx, XProfpData
        mov     eax, [ecx]          ; eax = XProfpData.bufnext
    loop1:
        cmp     eax, [ecx+4]        ; XProfpData.bufnext >= XProfpData.bufend?
        jae     ret1
        lea     edx, [eax+16]       ; XProfpData.bufnext += 4
        cmpxchg [ecx], edx
        jnz     loop1

        mov     ecx, eax
        rdtsc
        or      edx, XPROFREC_CONTROL
        mov     [ecx], edx          ; timestamp-high
        mov     [ecx+4], eax        ; timestamp-low
        mov     eax, [esp+16]       ; caller - 3 pushes + return address
        mov     [ecx+8], eax
        mov     eax, [esp+20]       ; callee
        mov     [ecx+12], eax

    ret1:
        pop     edx
        pop     ecx
        pop     eax
        ret     8
    }
}


//
// Default profiling data buffer size is 32MB
//
#define XPROF_DEFAULT_BUFSIZE 32
#define XProfpCalcBufferSize(param) \
        ((((param) & 0xffff) ? ((param) & 0xffff) : XPROF_DEFAULT_BUFSIZE) * 1024*1024)

//
// Acquire and release global access lock
//
#define XProfpLock() InterlockedCompareExchange(&XProfpData.lock, 1, 0)
#define XProfpUnlock() (XProfpData.lock = 0)

//
// Disable / enable interrupts
//
#define XProfpDisableInterrupts() KfRaiseIrql(HIGH_LEVEL)
#define XProfpRestoreInterrupts(irql) KfLowerIrql(irql)

//
// Thread context switch notification routine
//
extern PSWAP_CONTEXT_NOTIFY_ROUTINE KiDbgCtxSwapNotify;
PSWAP_CONTEXT_NOTIFY_ROUTINE XProfpSavedCtxSwapNotify;

VOID FASTCALL
XProfpThreadSwitchNotifyCallback(
    HANDLE OldThreadId,
    HANDLE NewThreadId
    )
{
    XProfpEmitControlRecord(XPROF_THREAD_SWITCH, (ULONG) NewThreadId);

    // Call the original notification function
    if (XProfpSavedCtxSwapNotify) {
        (*XProfpSavedCtxSwapNotify)(OldThreadId, NewThreadId);
    }
}

//
// DPC dispatch notification routine
//
extern PVOID KiDpcDispatchNotify;

VOID FASTCALL
XProfpDpcDispatchNotifyCallback(
    VOID* dpcproc,
    ULONG param
    )
{
    XProfpEmitControlRecord(XPROF_DPC_BEGIN+param, (ULONG) dpcproc);
}

NTSTATUS
XProfpControl(
    ULONG Action,
    ULONG Param
    )

/*++

Routine Description:

    Profiler control function

Arguments:

    Action - Specify the control action
    Param - Action-specific parameter

Return Value:

    Status code

--*/

{
    KIRQL irql;
    ULONG tid;
    NTSTATUS status = STATUS_SUCCESS;

    switch (Action) {
    case XPROF_START:
    case XPROF_STOP:
        if (XProfpLock() != 0) {
            // Simple-minded locking mechanism: prevent multiple threads
            // from calling  start/stop/resume at the same time
            status = STATUS_DEVICE_BUSY;
            break;
        }

        tid = (ULONG) PsGetCurrentThread()->UniqueThread;

        if (Action == XPROF_START) {
            // Start a new profiling session

            //
            // Clear the existing profiling data buffer first
            //
            ULONG oldsize = XProfpCalcBufferSize(XProfpData.start_param);
            ULONG newsize = XProfpCalcBufferSize(Param);
            ULONG* buf = XProfpData.bufstart;
                
            if (buf) {
                if (!XProfpData.bufnext_stopped) {
                    // There is already an active profiling session.
                    XProfpUnlock();
                    return STATUS_INVALID_PARAMETER;
                }

                irql = XProfpDisableInterrupts();

                XProfpData.bufstart = 
                XProfpData.bufend = 
                XProfpData.bufnext = 
                XProfpData.bufnext_stopped = NULL;
                XProfpData.start_type = 
                XProfpData.start_param = 0;

                KiDbgCtxSwapNotify = XProfpSavedCtxSwapNotify = NULL;
                KiDpcDispatchNotify = NULL;
                XProfpRestoreInterrupts(irql);

                // If the old size and the new size are the same,
                // then we'll reuse the same buffer. Otherwise,
                // we'll free the old buffer first.
                if (oldsize != newsize) {
                    MmDbgFreeMemory(buf, oldsize);
                    buf = NULL;
                }
            }

            //
            // Allocate memory for the new profiling data buffer
            //
            if (!buf) buf = (ULONG*) MmDbgAllocateMemory(newsize, PAGE_READWRITE|PAGE_WRITECOMBINE);
            if (buf == NULL) {
                status = STATUS_NO_MEMORY;
            } else {
                irql = XProfpDisableInterrupts();

                XProfpData.bufstart = (ULONG*) buf;
                XProfpData.bufnext = XProfpData.bufstart;
                XProfpData.bufend = (ULONG*) ((UCHAR*) buf + newsize - XPROFREC_MAXSIZE);
                XProfpData.start_param = Param;
                XProfpData.start_type = Action;

                XProfpEmitControlRecord(XPROF_START, tid);
                XProfpSavedCtxSwapNotify = KiDbgCtxSwapNotify;
                KiDbgCtxSwapNotify = XProfpThreadSwitchNotifyCallback;
                if (Param & XPROF_DPC_ENABLED)
                    KiDpcDispatchNotify = XProfpDpcDispatchNotifyCallback;
                XProfpRestoreInterrupts(irql);
            }
        } else {
            // Stop the current profiling session
            
            if (!XProfpData.bufstart) {
                // No profiling session
                status = STATUS_INVALID_PARAMETER;
            } else if (XProfpData.bufnext_stopped) {
                // Profiling session already stopped
                //  do nothing
            } else {
                irql = XProfpDisableInterrupts();
                KiDbgCtxSwapNotify = XProfpSavedCtxSwapNotify;
                XProfpSavedCtxSwapNotify = NULL;
                KiDpcDispatchNotify = NULL;

                XProfpEmitControlRecord(XPROF_STOP, tid);

                XProfpData.bufnext_stopped = XProfpData.bufnext;
                XProfpData.bufnext = XProfpData.bufend;

                XProfpRestoreInterrupts(irql);
            }
        }

        XProfpUnlock();
        break;

    case XPROF_COMMENT:
        XProfpEmitControlRecord(Action, Param);
        break;

    default:
        status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    return status;
}

#endif // _XBOX_ENABLE_PROFILING

