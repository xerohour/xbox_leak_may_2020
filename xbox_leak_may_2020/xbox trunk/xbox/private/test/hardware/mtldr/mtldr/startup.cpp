/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    startup.c

Abstract:

    This module contains routine to startup CRT and mtldr system.

--*/

#include "mtldrp.h"

#if __cplusplus
extern "C" {
#endif

int __cdecl main(int, char **, char **);
VOID WINAPI DbgBreakPoint( VOID );

#if __cplusplus
}
#endif

LONG
WINAPI
MtLdrUnhandledException(
    PEXCEPTION_POINTERS p
    )
{
    DbgPrint( "MTLDR: unhandled exception %p\n", p );
    DbgBreakPoint();
    return EXCEPTION_EXECUTE_HANDLER;
}

DWORD
WINAPI
MtLdrMainThread(
    PVOID KernelLoaderBlock
    )
/*++

Routine Description:

    This is a main entry point of boot-loader that call regular "C" main
    function.

Arguments:

    KernelLoaderBlock - The loader block required by LdrLoadSystemImage

Return Value:

    Always 0 (unused)

--*/
{
    UNREFERENCED_PARAMETER( KernelLoaderBlock );

    //__try {
        main( 0, NULL, NULL );
    //} __except( MtLdrUnhandledException(GetExceptionInformation()) ) {
    //}

    return 0;
}

BOOL
InitProcess(
    PVOID KernelLoaderBlock
    )
{
    HANDLE ThreadHandle;

    //
    // Initialize system loader and all TLS related stuff
    //

    LdrInitializeSystemLoader( KernelLoaderBlock );

    //
    // We have to create separate thread to execute any code that uses
    // Winsock APIs.  This is necessary because Winsock APIs rely on TLS
    // data to be initialized correctly and TLS data only gets initialized
    // in thread created by CreateThread (we are now running in the
    // context of kernel thread)
    //

    ThreadHandle = CreateThread( NULL, 0,
        (LPTHREAD_START_ROUTINE)MtLdrMainThread, KernelLoaderBlock, 0, NULL );

    if ( !ThreadHandle ) {
        DbgPrint( "MTLDR: unable to create main thread\n" );
    } else {
        CloseHandle( ThreadHandle );
    }

    return TRUE;
}

__declspec( naked )
VOID
__cdecl
mainCRTStartup(
    PVOID KernelLoaderBlock
    )
/*++

Routine Description:

    This is a absolute entry point of boot-loader. We need to define it here
    instead of using the version in XTL because we can't use XTL.  The code
    has to be written so that it contains the pattern at specific offset that
    imagebld looking for

Arguments:

    KernelLoaderBlock - The loader block required by LdrLoadSystemImage

Return Value:

    Always TRUE

--*/
{
    //
    // Use naked function and in-line assembly so that it contains particular
    // pattern at particular offset that imagebld is looking for
    //

    __asm {
        push    ebp
        mov     ebp, esp
        push    ecx
        xchg    ecx, ecx
        xchg    ecx, ecx
        xchg    ecx, ecx
        xchg    ecx, ecx
        nop
        mov     dword ptr [ebp-4], 0x0ff00110
    }

    InitProcess( KernelLoaderBlock );

    __asm {
        leave
        ret     0
    }
}

#define chartodigit(c)    ((c) >= '0' && (c) <= '9' ? (c) - '0' : -1)

long __cdecl atol(const CHAR *nptr)
{
    int c;              // current char
    long total;         // current total
    int sign;           // if '-', then negative, otherwise positive

    while ( *nptr == ' ' || *nptr == '\t' ) {
        ++nptr;
    }

    c = (int)(UCHAR)*nptr++;
    sign = c;           // save sign indication

    if (c == '-' || c == '+') {
        c = (int)(UCHAR)*nptr++;
    }

    total = 0;

    while ( (c = chartodigit(c)) != -1 ) {
        total = 10 * total + c; // accumulate digit
        c = (UCHAR)*nptr++;     // get next char
    }

    if (sign == '-') {
        return -total;
    } else {
        return total;
    }
}
