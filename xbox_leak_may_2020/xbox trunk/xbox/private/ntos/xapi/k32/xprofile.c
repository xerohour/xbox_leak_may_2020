/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xprofile.c

Abstract:

    XAPI profiling support functions

Notes:

    We access the kernel's profiling data buffer directly.
    If the kernel's data structure changes, be sure to
    update the CAP_Start_Profiling and CAP_End_Profiling
    functions accordingly.

--*/

#include "basedll.h"

#ifdef _XBOX_ENABLE_PROFILING

#include "xprofp.h"

//
// Pointer to the kernel's global profiling data structure
//
XProfpGlobals* XapipProfileData;

VOID XapipInitProfiling() {
    XapipProfileData = XProfpGetData();
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

Notes:

    All registers are preserved.

--*/

{
    /*
        ULONG* buf;

    loop:
        buf = XProfpData.bufnext
        if buf >= XProfpData.bufend, return
        Atomical incremnt XProfpData.bufnext by 4 ULONGs
        if failed, goto loop

        buf[0] = timestamp-high
        buf[1] = timestamp-low
        buf[2] = caller
        buf[3] = callee

     */

    __asm {
        push    ecx
        mov     ecx, XapipProfileData
        test    ecx, ecx
        jz      ret2

        push    eax
        push    edx
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
        pop     eax
    ret2:
        pop     ecx
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

Notes:

    All registers are preserved.

--*/

{
    // Same logic as above, except the record is smaller by 1 ULONG

    __asm {
        push    ecx
        mov     ecx, XapipProfileData
        test    ecx, ecx
        jz      ret2

        push    eax
        push    edx
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
        pop     eax
    ret2:
        pop     ecx
        ret     4
    }
}

#endif // _XBOX_ENABLE_PROFILING
