/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    tls.cpp

Abstract:

    This program contains routines to stress and test Thread Local Storage

Author:

    John Daly

Environment:

    Xbox

Revision History:

--*/


#include "threading.h"

//
// static thread data in TLS
//

#define TLS_ARRAY_ELEMENTS 64
__declspec( thread ) DWORD tls_array[TLS_ARRAY_ELEMENTS] = {0};


DWORD
WINAPI
ThreadLocalStorageTestThreadProc(
    PTHREAD_PARAMETERS tpm
    )
{
    #define TOTAL_DYNAMIC_TLS_AVAILABLE (TLS_MINIMUM_AVAILABLE + TLS_EXPANSION_SLOTS)
    HANDLE LogHandle = tpm->LogHandle; // make log handle visible
    LPSTR Variation = {"TLS"};
    DWORD x;
    DWORD DynamicTLSSlots = 0;
    PVOID pTLS;
    BOOL ErrorFlag = FALSE;
    DWORD DynTlsArray[TOTAL_DYNAMIC_TLS_AVAILABLE + 1] = {0};   // + 1 to contain the end of the list
    
    //
    // since we are logging from the test thread, I have to cheat all over the place...
    //

    xSetComponent( LogHandle, "Kernel", "Ps" );
    xSetFunctionName( LogHandle, "ThreadLocalStorageTestThreadProc" );

    BEGIN_VARIATION( Variation ) {

        //
        // create dynamic TLS until failure
        // change, check, free dynamic TLS
        //
        
        ErrorFlag = FALSE;

        while ( ~0 != (DynTlsArray[DynamicTLSSlots] = TlsAlloc()) &&
                (DynamicTLSSlots <  TOTAL_DYNAMIC_TLS_AVAILABLE)) {
            ++DynamicTLSSlots;
        }

        for ( x = 0; x < DynamicTLSSlots; ++x ) {
            if ( 0 == TlsSetValue(DynTlsArray[x],&x) ) {
                LOG_MSG_FAIL("TlsSetValue failed!");
                ErrorFlag = TRUE;
            }
        }

        for ( x = 0; x < DynamicTLSSlots; ++x ) {
            pTLS = TlsGetValue(DynTlsArray[x]);
            if ( x != *(DWORD *)pTLS ) {
                LOG_MSG_FAIL("TlsGetValue failed!");
                ErrorFlag = TRUE;
            }
        }

        for ( x = 0; x < DynamicTLSSlots; ++x ) {
            if ( FALSE == TlsFree(DynTlsArray[x]) ) {
                LOG_MSG_FAIL("TlsFree failed!");
                ErrorFlag = TRUE;
            }
        }
        
        if ( ErrorFlag ) {
            LOG_MSG_FAIL("Tls Test failure detected!");
        } else {
            LOG_MSG_PASS("Tls Test succeeded in all cases");
        }

        //
        // change, check static TLS
        //

        for ( x = 0; x < TLS_ARRAY_ELEMENTS; x++ ) {
            tls_array[x] = x;
        }

        ErrorFlag = FALSE;
        for ( x = 0; x < TLS_ARRAY_ELEMENTS; x++ ) {
            if ( tls_array[x] != x ) {
                LOG_MSG_FAIL("tls_array access failed!");
                ErrorFlag = TRUE;
            }
        }

        if ( ErrorFlag ) {
            LOG_MSG_FAIL("tls_array access failure detected!");
        } else {
            LOG_MSG_PASS("tls_array access succeeded in all cases");
        }

    } END_VARIATION();

    return (0);
}

VOID
ThreadLocalStorageVariation(
    HANDLE LogHandle,
    LPSTR  Variation
    )
{
    HANDLE ThreadHandle;
    THREAD_PARAMETERS tpm;

    BEGIN_VARIATION( Variation )
    {
        tpm.LogHandle = LogHandle;
        ThreadHandle = CreateThread(
                            0,
                            4096 * 16,  // 64k
                            (LPTHREAD_START_ROUTINE)ThreadLocalStorageTestThreadProc,
                            (LPVOID)&tpm,
                            0,
                            &tpm.ThreadID
                            );

        if ( !ThreadHandle ) {
            LOG_MSG_BLOCK( "CreateThread failed" );
        } else {
            LOG_MSG_PASS("tls test ran");
            WaitForSingleObject( ThreadHandle, INFINITE );
            CloseHandle( ThreadHandle );
        }
    }
    END_VARIATION();
}


VOID
ThreadLocalStorageTest(
    HANDLE LogHandle
    )
{
    xSetFunctionName( LogHandle, "ThreadLocalStorageTest" );
    ThreadLocalStorageVariation( LogHandle, "ThreadLocalStorageTestRunner" );
}
