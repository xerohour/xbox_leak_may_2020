/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    main.cpp

Abstract:

    This program contains routines to stress and test fiber and fiber
    related APIs

--*/

#include "threading.h"

VOID
WINAPI
FiberProc(
    PFIBER_PARAMETERS fpm
    )
{
    ASSERT( fpm );
    ASSERT( fpm->MainFiber );

    InterlockedIncrement( &fpm->Counter );
    SwitchToFiber( fpm->MainFiber );
}

VOID
FiberStackSizeTest(
    IN HANDLE LogHandle,
    IN SIZE_T Size
    )
{
    LPVOID Fiber;
    CHAR Variation[64];

    if ( Size <= 1024*1024 ) {
        sprintf( Variation, "%dK stack size", Size/1024 );
    } else {
        sprintf( Variation, "%dM stack size", Size/(1024*1024) );
    }

    BEGIN_VARIATION( Variation )
    {
        Fiber = CreateFiber( Size, 0, 0 );
        LOG_PASS_FAIL( Fiber != NULL );
        if ( Fiber ) {
            DeleteFiber( Fiber );
        }
    }
    END_VARIATION();
}

VOID
FiberTest(
    IN HANDLE LogHandle
    )
{
    LPVOID Fiber, MainFiber=NULL;
    SIZE_T f, NumberOfFibers;
    FIBER_PARAMETERS fpm;

    xSetFunctionName( LogHandle, "CreateFiber/DeleteFiber" );

    FiberStackSizeTest( LogHandle, 0 );
    FiberStackSizeTest( LogHandle, 4096 );
    FiberStackSizeTest( LogHandle, 16*1024 );
    FiberStackSizeTest( LogHandle, 128*1024 );

    //
    // Don't run the cases that consume lots of stack space in stress
    //

    if ( !XTestLibIsStressTesting() ) {
        FiberStackSizeTest( LogHandle, 1*1024*1024 );
        FiberStackSizeTest( LogHandle, 2*1024*1024 );
        FiberStackSizeTest( LogHandle, 5*1024*1024 );
        FiberStackSizeTest( LogHandle, 16*1024*1024 );
    }

    //
    // Create lots of fibers if we are not in stress
    //

    if ( !XTestLibIsStressTesting() ) {

        PVOID *Fibers = NULL;
        NumberOfFibers = 2000;

        BEGIN_VARIATION( "Create lots of fibers" )
        {
            Fibers = (PVOID *)LocalAlloc( LPTR, NumberOfFibers*sizeof(PVOID) );
            if ( !Fibers ) {
                LOG_MSG_BLOCK( "couldn't allocate buffer" );
            } else {
                for ( f=0; f<NumberOfFibers; f++ ) {
                    Fibers[f] = CreateFiber( 16384, 0, 0 );
                    if ( !Fibers[f] ) {
                        break;
                    }
                }
                LOG_PASS_FAIL( f == NumberOfFibers );
            }
        }
        END_VARIATION();

        if ( Fibers ) {
            for ( f=0; f<NumberOfFibers && Fibers[f]; f++ ) {
                DeleteFiber( Fibers[f] );
            }
            LocalFree( Fibers );
        }
    }

    xSetFunctionName( LogHandle, "ConvertThreadToFiber" );

    BEGIN_VARIATION( "Convert current thread to fiber" )
    {
        MainFiber = ConvertThreadToFiber( 0 );
        if ( MainFiber ) {
            LOG_PASS();
        } else {
            LOG_MSG_FAIL( "Cannot convert thread to fiber, test blocked" );
        }
    }
    END_VARIATION();

    //
    // Bail out if ConvertThreadToFiber fails, nothing else we could do now
    //

    if ( !MainFiber ) {
        return;
    }

    xSetFunctionName( LogHandle, "SwitchToFiber" );

    BEGIN_VARIATION( "Switch to lots of fiber" )
    {
        NumberOfFibers = 10000;
        fpm.Counter = 0;
        fpm.ExpectedCounter = NumberOfFibers;
        fpm.LogHandle = LogHandle;
        fpm.MainFiber = MainFiber;

        for ( f=0; f<NumberOfFibers; f++ ) {
            Fiber = CreateFiber( 16384, (LPFIBER_START_ROUTINE)FiberProc, &fpm );
            if ( !Fiber ) {
                LOG_MSG_BLOCK( "unable to create fiber" );
                break;
            }
            SwitchToFiber( Fiber );
            DeleteFiber( Fiber );
        }

        if ( f == NumberOfFibers ) {
            LOG_PASS_FAIL( NumberOfFibers == fpm.ExpectedCounter );
        }
    }
    END_VARIATION();
}
