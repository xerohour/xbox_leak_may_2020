/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    animate.c

Abstract:

    Startup animation implementation.

--*/

#include <stddef.h>

#pragma code_seg("INIT")
#pragma data_seg("INIT_RW")
#pragma const_seg("INIT_RD")

#include "ntos.h"
#include "stdio.h"
#include "stdlib.h"
#include "wtypes.h"

#include "ani.h"

// Tell linker to put startup animation code and data into INIT section
#pragma comment(linker, "/merge:INIT_RD=INIT")
#pragma comment(linker, "/merge:INIT_RW=INIT")
#pragma comment(linker, "/merge:D3D=INIT")
#pragma comment(linker, "/merge:D3D_RD=INIT")
#pragma comment(linker, "/merge:D3D_RW=INIT")
#pragma comment(linker, "/merge:XGRPH=INIT")
#pragma comment(linker, "/merge:XGRPH_RD=INIT")

// We always want to link with the animation code, so that we can
// keep the build from breaking. Thats why we use a global to
// decide whether to run the animation or not. The global tricks
// the linker into linking in all the code the animation uses.

#ifdef NOANI
BOOL gBootAnimation_DoAnimation = FALSE;
#else
BOOL gBootAnimation_DoAnimation = TRUE;
#endif

#ifdef BOOTSOUND
BOOL gBootAnimation_DoSound = TRUE;
#else
BOOL gBootAnimation_DoSound = FALSE;
#endif

// Background animation thread.
HANDLE g_hThread;

// Entrypoing into the animation thread.
VOID AnipStartAnimationThread(PKSTART_ROUTINE StartRoutine, PVOID StartContext);

// Main animation routine as defined in the animation library.
VOID AnipRunAnimation();

#define CONTIGUOUS_BLOCK_SIZE           (5 * 1024 * 1024 / 2)
#define AGP_APERTURE_BYTES              (64*1024*1024)
#define INSTANCE_MEM_MAXSIZE            (20*1024)
#define NV_INSTANCE_SIZE                (INSTANCE_MEM_MAXSIZE)

//------------------------------------------------------------------------
// Starts the animation which will run on a background thread.  This API
// returns immediately.
//

BOOL g_bShortVersion;

void AniStartAnimation(BOOLEAN fShort)
{
    NTSTATUS Status;

    if (gBootAnimation_DoAnimation){

        g_bShortVersion = fShort;

        Status = PsCreateSystemThreadEx(&g_hThread,
                                        0,
                                        0x4000,  // Stack size, 16K
                                        0,
                                        NULL,
                                        NULL,
                                        NULL,
                                        FALSE,
                                        FALSE,
                                        AnipStartAnimationThread);

        if (!NT_SUCCESS(Status))
        {
            // RIP(("AniStartAnimation - Unable to create thread."));
            g_hThread = NULL;
        }
    }
}

//------------------------------------------------------------------------
// Shut down the animation.  This will block until the animation finishes.
//
void AniTerminateAnimation()
{
    if (g_hThread)
    {
        NTSTATUS Status;
#if DBG
        int start = NtGetTickCount();
#endif

        // Wait for it to go away.
        Status = NtWaitForSingleObjectEx(g_hThread, KernelMode, FALSE, NULL);

#if DBG
        DbgPrint("Boot animation wait %d\n", NtGetTickCount() - start);

        if (Status == STATUS_TIMEOUT)
        {
            //RIP(("AniTerminateAnimation - Animation is stuck!"));
        }
#endif

        NtClose(g_hThread);

        g_hThread = NULL;
    }
}

void AnipBreak()
{
#if DBG
   _asm int 3;
#endif
}

#if DBG
int gcMemAllocsContiguous = 0;
#endif

//------------------------------------------------------------------------
// Blocks until the animation has completed (until the animation is ready
// to display the Microsoft logo).
//
void AniBlockOnAnimation(void)
{
    extern KEVENT g_EventLogoWaiting;

    NTSTATUS status;
    PETHREAD ThreadObject;
    PVOID WaitObjects[2];
    KWAIT_BLOCK WaitBlocks[2];

    if (g_hThread)
    {
        status = ObReferenceObjectByHandle(g_hThread, &PsThreadObjectType,
            (PVOID*)&ThreadObject);

        if (NT_SUCCESS(status))
        {
            WaitObjects[0] = ThreadObject;
            WaitObjects[1] = &g_EventLogoWaiting;

            KeWaitForMultipleObjects(2, WaitObjects, WaitAny, Executive,
                KernelMode, FALSE, NULL, WaitBlocks);

            ObDereferenceObject(ThreadObject);
        }
    }
}

//------------------------------------------------------------------------
// MemAllocContiguous
//
void *MemAllocContiguous(size_t Size, DWORD Alignment)
{
#if DBG
    gcMemAllocsContiguous++;
#endif

    return MmAllocateContiguousMemoryEx(
            Size,
            0,
            AGP_APERTURE_BYTES - NV_INSTANCE_SIZE,
            Alignment,
            PAGE_READWRITE | PAGE_WRITECOMBINE);
}

//------------------------------------------------------------------------
// MemFreeContiguous
//
void MemFreeContiguous(void *pv)
{
#if DBG
    if (gcMemAllocsContiguous <= 0)
    {
        AnipBreak();
    }
    gcMemAllocsContiguous--;
#endif

    MmFreeContiguousMemory(pv);
}

//------------------------------------------------------------------------
// Main animation procedure.  Defers to the startup animation library.
//
VOID AnipStartAnimationThread(
    PKSTART_ROUTINE StartRoutine, 
    PVOID StartContext
    )
{
    AnipRunAnimation();

    // Make this thread go away.
    PsTerminateSystemThread(0);
}

///////////////////////////////////////////////////////////////////////////////
// Defined so we don't have to pull libc in
typedef void (__cdecl *_PVFV)(void);

int __cdecl atexit(_PVFV func)
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Define a couple of debug-only methods used in XGRAPHICS that normally
// are implemented in XTL.

#ifdef STARTUPANIMATION

long __cdecl _ftol2(float x)
{
    DWORD result[2];
    unsigned short oldcw;
    unsigned short newcw;

    _asm
    {
        fstcw   [oldcw]         ; get control word
        fwait                   ; synchronize

        mov ax, [oldcw]         ; round mode saved
        or  ah, 0ch             ; set chop rounding mode
        mov [newcw], ax         ; back to memory

        fldcw   [newcw]         ; reset rounding
        fistp   qword ptr [result]  ; store chopped integer
        fldcw   [oldcw]         ; restore rounding

        mov eax, dword ptr [result]
        mov edx, dword ptr [result+4]
    }
}

#define D_EXP(x) ((unsigned short *)&(x)+3)
#define D_HI(x) ((unsigned long *)&(x)+1)
#define D_LO(x) ((unsigned long *)&(x))

#define IS_D_QNAN(x)    ((*D_EXP(x) & 0x7ff8) == 0x7ff8)
#define IS_D_SNAN(x)    ((*D_EXP(x) & 0x7ff8) == 0x7ff0 && \
                         (*D_HI(x) << 13 || *D_LO(x)))
                         
int __cdecl _isnan(double x)
{
    if (IS_D_SNAN(x) || IS_D_QNAN(x)) {
        return 1;
    }
    return 0;
}

VOID
XDebugError(PCHAR Module, PCHAR Format, ...)
{
    _asm int 3;
}

void Sleep(DWORD Milliseconds)
{
    _asm int 3;
}

VOID
OutputDebugStringA(
    IN LPCSTR lpOutputString
    )
{
    DbgPrint((PSTR)lpOutputString);
}

#endif

