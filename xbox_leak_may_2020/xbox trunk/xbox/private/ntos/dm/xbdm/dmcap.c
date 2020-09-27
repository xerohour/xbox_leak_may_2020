/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    dmcap.c

Abstract:

    This module implements routines to collect call-attributed profile (CAP)
    data for Xbox titles.

--*/

#include "dmp.h"

//
// Default profiling data buffer size is 32MB
//

#define XPROF_DEFAULT_BUFSIZE 32

#define XProfpCalcBufferSize(param) \
    ((((param) & 0xffff) ? ((param) & 0xffff) : \
        XPROF_DEFAULT_BUFSIZE) * 1024*1024)

//
// Acquire and release global access lock
//

#define XProfpLock() InterlockedCompareExchange(&DmProfileData->lock, 1, 0)
#define XProfpUnlock() (DmProfileData->lock = 0)

//
// Disable/enable interrupts
//

#define XProfpDisableInterrupts() KfRaiseIrql(HIGH_LEVEL)
#define XProfpRestoreInterrupts(irql) KfLowerIrql(irql)

BOOL DmProfileEnable;
XProfpGlobals *DmProfileData;
PSWAP_CONTEXT_NOTIFY_ROUTINE DmProfileSavedCtxSwapNotify;

__declspec(naked)
VOID
__stdcall
_CAP_Start_Profiling(
    VOID* caller,
    VOID* callee
    )
{
    /*
        ULONG* buf;

    loop:
        buf = DmProfileData->bufnext
        if buf >= DmProfileData->bufend, return
        Atomical incremnt DmProfileData->bufnext by 4 ULONGs
        if failed, goto loop

        buf[0] = timestamp-high
        buf[1] = timestamp-low
        buf[2] = caller
        buf[3] = callee

     */

    __asm {
        push    ecx
        mov     ecx, DmProfileData
        test    ecx, ecx
        jz      ret2

        push    eax
        push    edx
        mov     eax, [ecx]          ; eax = DmProfileData->bufnext
loop1:
        cmp     eax, [ecx+4]        ; DmProfileData->bufnext >= DmProfileData->bufend?
        jae     ret1
        lea     edx, [eax+16]       ; DmProfileData->bufnext += 4
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
        pop     eax
ret2:
        pop     ecx
        ret     8
    }
}

__declspec(naked)
VOID
__stdcall
_CAP_End_Profiling(
    VOID* caller
    )
{
    // Same logic as above, except the record is smaller by 1 ULONG

    __asm {
        push    ecx
        mov     ecx, DmProfileData
        test    ecx, ecx
        jz      ret2

        push    eax
        push    edx
        mov     eax, [ecx]          ; eax = DmProfileData->bufnext
loop1:
        cmp     eax, [ecx+4]        ; DmProfileData->bufnext >= DmProfileData->bufend?
        jae     ret1
        lea     edx, [eax+12]       ; DmProfileData->bufnext += 3
        cmpxchg [ecx], edx
        jnz     loop1

        mov     ecx, eax
        rdtsc
        bts     edx, 29             ; timestamp-high | XPROFREC_FASTCAP_END
        mov     [ecx], edx
        mov     [ecx+4], eax        ; timestamp-low
        mov     eax, [esp+16]       ; caller
        mov     [ecx+8], eax

ret1:
        pop     edx
        pop     eax
ret2:
        pop     ecx
        ret     4
    }
}

__declspec(naked)
VOID
__stdcall
_CAP_Enter_Function(
    VOID* caller
    )
{
    __asm {
        push    ecx
        mov     ecx, DmProfileData
        test    ecx, ecx
        jz      ret2

        push    eax
        push    edx
        mov     eax, [ecx]          ; eax = DmProfileData->bufnext
loop1:
        cmp     eax, [ecx+4]        ; DmProfileData->bufnext >= DmProfileData->bufend?
        jae     ret1
        lea     edx, [eax+12]       ; DmProfileData->bufnext += 3
        cmpxchg [ecx], edx
        jnz     loop1

        mov     ecx, eax
        rdtsc
        bts     edx, 31             ; timestamp-high | XPROFREC_CALLCAP_START
        mov     [ecx], edx
        mov     [ecx+4], eax        ; timestamp-low
        mov     eax, [esp+16]       ; caller
        mov     [ecx+8], eax

ret1:
        pop     edx
        pop     eax
ret2:
        pop     ecx
        ret     4
    }
}

__declspec(naked)
VOID
__stdcall
_CAP_Exit_Function(
    VOID* caller
    )
{
    __asm {
        push    ecx
        mov     ecx, DmProfileData
        test    ecx, ecx
        jz      ret2

        push    eax
        push    edx
        mov     eax, [ecx]          ; eax = DmProfileData->bufnext
loop1:
        cmp     eax, [ecx+4]        ; DmProfileData->bufnext >= DmProfileData->bufend?
        jae     ret1
        lea     edx, [eax+12]       ; DmProfileData->bufnext += 3
        cmpxchg [ecx], edx
        jnz     loop1

        mov     ecx, eax
        rdtsc
        or      edx, XPROFREC_CALLCAP_END   ; timestamp-high | XPROFREC_CALLCAP_END
        mov     [ecx], edx
        mov     [ecx+4], eax        ; timestamp-low
        mov     eax, [esp+16]       ; caller
        mov     [ecx+8], eax

ret1:
        pop     edx
        pop     eax
ret2:
        pop     ecx
        ret     4
    }
}

__declspec(naked)
VOID
__cdecl
_penter(
    VOID
    )
{
    /*
        ULONG* buf;

    loop:
        buf = DmProfileData->bufnext
        if buf >= DmProfileData->bufend, return
        Atomical incremnt DmProfileData->bufnext by 4 ULONGs
        if failed, goto loop

        buf[0] = timestamp-high
        buf[1] = timestamp-low
        buf[2] = caller
        buf[3] = ESP

     */

    __asm {
        push    ecx
        mov     ecx, DmProfileData
        test    ecx, ecx
        jz      ret2

        push    eax
        push    edx
        mov     eax, [ecx]          ; eax = DmProfileData->bufnext
loop1:
        cmp     eax, [ecx+4]        ; DmProfileData->bufnext >= DmProfileData->bufend?
        jae     ret1
        lea     edx, [eax+16]       ; DmProfileData->bufnext += 4
        cmpxchg [ecx], edx
        jnz     loop1

        mov     ecx, eax
        rdtsc
        bts     edx, 30             ; timestamp-high | XPROFREC_PENTER
        mov     [ecx], edx          ; timestamp-high
        mov     [ecx+4], eax        ; timestamp-low
        mov     eax, [esp+12]       ; caller - 3 pushes + return address
        mov     [ecx+8], eax
        mov     [ecx+12], esp       ; ESP

ret1:
        pop     edx
        pop     eax
ret2:
        pop     ecx
        ret
    }
}

__declspec(naked)
VOID
__cdecl
_pexit(
    VOID
    )
{
    /*
        ULONG* buf;

    loop:
        buf = DmProfileData->bufnext
        if buf >= DmProfileData->bufend, return
        Atomical incremnt DmProfileData->bufnext by 4 ULONGs
        if failed, goto loop

        buf[0] = timestamp-high
        buf[1] = timestamp-low
        buf[2] = caller
        buf[3] = ESP

     */

    __asm {
        push    ecx
        mov     ecx, DmProfileData
        test    ecx, ecx
        jz      ret2

        push    eax
        push    edx
        mov     eax, [ecx]          ; eax = DmProfileData->bufnext
loop1:
        cmp     eax, [ecx+4]        ; DmProfileData->bufnext >= DmProfileData->bufend?
        jae     ret1
        lea     edx, [eax+16]       ; DmProfileData->bufnext += 4
        cmpxchg [ecx], edx
        jnz     loop1

        mov     ecx, eax
        rdtsc
        or      edx, XPROFREC_PEXIT ; timestamp-high | XPROFREC_PEXIT
        mov     [ecx], edx          ; timestamp-high
        mov     [ecx+4], eax        ; timestamp-low
        mov     eax, [esp+12]       ; caller - 3 pushes + return address
        mov     [ecx+8], eax
        mov     [ecx+12], esp       ; ESP

ret1:
        pop     edx
        pop     eax
ret2:
        pop     ecx
        ret
    }
}

__declspec(naked)
VOID
__stdcall
DmProfileEmitControlRecord(
    ULONG type,
    ULONG param
    )
{
    // Identical to _CAP_Start_Profiling except
    // the record is marked as XPROFREC_CONTROL.

    __asm {
        push    eax
        push    ecx
        push    edx
        mov     ecx, DmProfileData
        mov     eax, [ecx]          ; eax = DmProfileData->bufnext
loop1:
        cmp     eax, [ecx+4]        ; DmProfileData->bufnext >= DmProfileData->bufend?
        jae     ret1
        lea     edx, [eax+16]       ; DmProfileData->bufnext += 4
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

VOID
FASTCALL
DmProfileThreadSwitchNotifyCallback(
    HANDLE OldThreadId,
    HANDLE NewThreadId
    )
{
    DmProfileEmitControlRecord(XPROF_THREAD_SWITCH, (ULONG) NewThreadId);

    // Call the original notification function
    if (DmProfileSavedCtxSwapNotify) {
        (*DmProfileSavedCtxSwapNotify)(OldThreadId, NewThreadId);
    }
}

VOID
FASTCALL
DmProfileDpcDispatchNotifyCallback(
    VOID* dpcproc,
    ULONG param
    )
{
    DmProfileEmitControlRecord(XPROF_DPC_BEGIN+param, (ULONG)dpcproc);
}

NTSTATUS
DmCapControl(
    ULONG Action,
    ULONG Param
    )
{
    KIRQL irql;
    ULONG tid;
    NTSTATUS status = STATUS_SUCCESS;

    switch (Action) {
    case XPROF_START:
    case XPROF_STOP:
        if (XProfpLock() != 0) {
            // Simple-minded locking mechanism: prevent multiple threads
            // from calling start/stop/resume at the same time
            status = STATUS_DEVICE_BUSY;
            break;
        }

        tid = (ULONG) PsGetCurrentThread()->UniqueThread;

        if (Action == XPROF_START) {
            // Start a new profiling session

            //
            // Clear the existing profiling data buffer first
            //
            ULONG oldsize = XProfpCalcBufferSize(DmProfileData->start_param);
            ULONG newsize = XProfpCalcBufferSize(Param);
            ULONG* buf = DmProfileData->bufstart;

            if (buf) {
                if (!DmProfileData->bufnext_stopped) {
                    // There is already an active profiling session.
                    XProfpUnlock();
                    return STATUS_INVALID_PARAMETER;
                }

                irql = XProfpDisableInterrupts();

                DmProfileData->bufstart =
                DmProfileData->bufend =
                DmProfileData->bufnext =
                DmProfileData->bufnext_stopped = NULL;
                DmProfileData->start_type =
                DmProfileData->start_param = 0;

                *g_dmi.CtxSwapNotifyRoutine = DmProfileSavedCtxSwapNotify = NULL;
                *g_dmi.DpcDispatchNotifyRoutine = NULL;
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

            if (!buf) buf = (ULONG*)MmDbgAllocateMemory(newsize, PAGE_READWRITE|PAGE_WRITECOMBINE);
            if (buf == NULL) {
                status = STATUS_NO_MEMORY;
            } else {
                irql = XProfpDisableInterrupts();

                DmProfileData->bufstart = (ULONG*)buf;
                DmProfileData->bufnext = DmProfileData->bufstart;
                DmProfileData->bufend = (ULONG*)((UCHAR*)buf + newsize - XPROFREC_MAXSIZE);
                DmProfileData->start_param = Param;
                DmProfileData->start_type = Action;

                DmProfileEmitControlRecord(XPROF_START, tid);
                DmProfileSavedCtxSwapNotify = (PSWAP_CONTEXT_NOTIFY_ROUTINE)*g_dmi.CtxSwapNotifyRoutine;
                *g_dmi.CtxSwapNotifyRoutine = (PVOID)DmProfileThreadSwitchNotifyCallback;
                if (Param & XPROF_DPC_ENABLED) {
                    *g_dmi.DpcDispatchNotifyRoutine = (PVOID)DmProfileDpcDispatchNotifyCallback;
                }
                XProfpRestoreInterrupts(irql);
            }
        } else {
            // Stop the current profiling session

            if (!DmProfileData->bufstart) {
                // No profiling session
                status = STATUS_INVALID_PARAMETER;
            } else if (DmProfileData->bufnext_stopped) {
                // Profiling session already stopped
                //  do nothing
            } else {
                irql = XProfpDisableInterrupts();
                *g_dmi.CtxSwapNotifyRoutine = (PVOID)DmProfileSavedCtxSwapNotify;
                DmProfileSavedCtxSwapNotify = NULL;
                *g_dmi.DpcDispatchNotifyRoutine = NULL;

                DmProfileEmitControlRecord(XPROF_STOP, tid);

                DmProfileData->bufnext_stopped = DmProfileData->bufnext;
                DmProfileData->bufnext = DmProfileData->bufend;

                XProfpRestoreInterrupts(irql);
            }
        }

        XProfpUnlock();
        break;

    case XPROF_COMMENT:
        DmProfileEmitControlRecord(Action, Param);
        break;

    default:
        status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    return status;
}

DWORD
WINAPI
DmProfileControl(
    DWORD Action,
    DWORD Parameter
    )
{
    NTSTATUS Status = DmCapControl(Action, Parameter);
    return RtlNtStatusToDosError(Status);
}

