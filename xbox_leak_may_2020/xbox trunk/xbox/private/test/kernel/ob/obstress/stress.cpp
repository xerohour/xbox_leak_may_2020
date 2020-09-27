/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    stress.cpp

Abstract:

    This program contains routines to stress object manager

Author:

    Silviu Calinoiu (silviuc) 18-Mar-1997

Environment:

    Xbox

Revision History:

    04-May-2000     schanbai

        Ported to Xbox

--*/


#include "pch.h"
#include "utils.h"
#include "stress.h"

//
// constant definitions
//

#define MAX_OBJECTS_PER_LOOP    1000
#define LOOPS_NO_PER_THREAD     1000

enum {
    StressCreateUnnamedEvents       = 0,
    StressCreateNamedEvents         = 1,
    StressCreateUnnamedSemaphores   = 2,
    StressCreateNamedSemaphores     = 3,
    StressCreateUnnamedMutexes      = 4,
    StressCreateNamedMutexes        = 5,
    StressDuplicateHandles          = 6,
    StressCloseHandles              = 7,
    StressOperationsNo              = 8
};


//
// global data
//

HANDLE *aHandles[ THREADS_NUMBER ];
UINT auHandlesNo[ THREADS_NUMBER ];

    
BOOL
CreateObjects( 
    UINT uThreadIndex,
    UINT uObjectType
    )
{
    UINT uObjectsToCreate;
    UINT uObjectsAlreadyCreated;
    UINT uCrtObject;
    int nResult;
    OCHAR szName[ 128 ];
    ULONG Seed = GetTickCount();

    /*
    DbgMessage(
        "CreateObjects( %u ) - begin - %u handles already\n",
        uThreadIndex,
        auHandlesNo[ uThreadIndex ]
        );
    */

    //
    // how many objects to create this time
    //

    uObjectsToCreate = abs( RtlRandom(&Seed) ) % MAX_OBJECTS_PER_LOOP;
    uObjectsAlreadyCreated = auHandlesNo[ uThreadIndex ];

    if ( uObjectsAlreadyCreated + uObjectsToCreate > MAX_OBJECTS_PER_THREAD ) {
        uObjectsToCreate = MAX_OBJECTS_PER_THREAD - uObjectsAlreadyCreated;
    }

    //
    // create the objects
    //

    switch ( uObjectType ) {

    case StressCreateUnnamedEvents:
        for ( uCrtObject = 0; uCrtObject < uObjectsToCreate; uCrtObject++ ) {

            aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] = \
                CreateEvent( NULL, FALSE, FALSE, NULL );
            
            if ( aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] == NULL ) {
                DbgMessage(
                    "Thread %u, CreateEvent failed, error %u.\n",
                    uThreadIndex, 
                    GetLastError()
                    );
            } else {
                uObjectsAlreadyCreated++;
            }
        }
        break;

    case StressCreateNamedEvents:
        for ( uCrtObject = 0; uCrtObject < uObjectsToCreate; uCrtObject++ ) {
            //
            // the name of the object - we might have duplicates
            //

            nResult = soprintf(
                            szName,
                            OTEXT( "Event%u" ),
                            uCrtObject
                            );

            if ( nResult < 0 ) {
                DbgMessage( "_sntprintf buffer too small.\n" );
                continue;
            }

            aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] = \
                CreateEvent( NULL, FALSE, TRUE, szName );
            
            if ( aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] == NULL ) {
                DbgMessage(
                    "Thread %u, CreateEvent failed, error %u.\n",
                    uThreadIndex, 
                    GetLastError()
                    );
            } else {
                uObjectsAlreadyCreated++;
            }
        }
        break;

    case StressCreateUnnamedSemaphores:
        for ( uCrtObject = 0; uCrtObject < uObjectsToCreate; uCrtObject++ ) {
            aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] = \
                CreateSemaphore( NULL, 1, 1, NULL );
            
            if ( aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] == NULL ) {
                DbgMessage(
                    "Thread %u, CreateSemaphore failed, error %u.\n",
                    uThreadIndex, 
                    GetLastError()
                    );
            } else {
                uObjectsAlreadyCreated++;
            }
        }
        break;

    case StressCreateNamedSemaphores:
        for ( uCrtObject = 0; uCrtObject < uObjectsToCreate; uCrtObject++ ) {
            //
            // the name of the object - we might have duplicates
            //

            nResult = soprintf( 
                            szName,
                            OTEXT( "Semaphore%u" ),
                            uCrtObject
                            );

            if ( nResult < 0 ) {
                DbgMessage( "_sntprintf buffer too small.\n" );
                continue;
            }

            aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] = \
                CreateSemaphore( NULL, 0, 1, szName );
            
            if ( aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] == NULL ) {
                DbgMessage(
                    "Thread %u, CreateSemaphore failed, error %u.\n",
                    uThreadIndex, 
                    GetLastError()
                    );
            } else {
                uObjectsAlreadyCreated++;
            }
        }
        break;

    case StressCreateUnnamedMutexes:
        for ( uCrtObject = 0; uCrtObject < uObjectsToCreate; uCrtObject++ ) {
            aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] = \
                CreateMutex( NULL, FALSE, NULL );
            
            if ( aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] == NULL ) {
                DbgMessage(
                    "Thread %u, CreateMutex failed, error %u.\n",
                    uThreadIndex, 
                    GetLastError()
                    );
            } else {
                uObjectsAlreadyCreated++;
            }
        }
        break;

    case StressCreateNamedMutexes:
        for ( uCrtObject = 0; uCrtObject < uObjectsToCreate; uCrtObject++ ) {
            //
            // the name of the object - we might have duplicates
            //

            nResult = soprintf( 
                            szName,
                            OTEXT( "Mutex%u" ),
                            uCrtObject
                            );

            if ( nResult < 0 ) {
                DbgMessage( "_sntprintf buffer too small.\n" );
                continue;
            }

            aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] = \
                CreateMutex( NULL, TRUE, szName );
            
            if ( aHandles[ uThreadIndex ][ uObjectsAlreadyCreated ] == NULL ) {
                DbgMessage(
                    "Thread %u, CreateMutex failed, error %u.\n",
                    uThreadIndex, 
                    GetLastError()
                    );
            } else {
                uObjectsAlreadyCreated++;
            }
        }
        break;

    default:
        ASSERT( FALSE );
    }

    ASSERT( uObjectsAlreadyCreated <= MAX_OBJECTS_PER_THREAD );

    auHandlesNo[ uThreadIndex ] = uObjectsAlreadyCreated;

    /*
    DbgMessage(
        "CreateObjects( %u ) - end - %u handles now\n",
        uThreadIndex,
        auHandlesNo[ uThreadIndex ]
        );
    */

    if ( uObjectsAlreadyCreated >= MAX_OBJECTS_PER_THREAD ) {
        //
        // the table is full
        //
        return FALSE;
    } else {
        //
        // still have space for some handles
        //
        return TRUE;
    }
}


void
CloseHandles( 
    UINT uThreadIndex
    )
{
    UINT uIndexStart;
    UINT uHandlesToClose;
    UINT uIndexEnd;
    UINT uCrtHandle;
    ULONG Seed = GetTickCount();

    /*
    DbgMessage(
        "CloseHandles( %u ) - begin - %u handles for now\n",
        uThreadIndex,
        auHandlesNo[ uThreadIndex ]
        );
    */

    if ( auHandlesNo[ uThreadIndex ] == 0 ) {
        //
        // nothing to close
        //
        return;
    }

    //
    // start closing from this index
    //

    uIndexStart = abs( RtlRandom(&Seed) ) % auHandlesNo[ uThreadIndex ];
    
    //
    // how many handles to close
    //

    uHandlesToClose = abs( RtlRandom(&Seed) ) % MAX_OBJECTS_PER_LOOP;

    if ( uIndexStart + uHandlesToClose > auHandlesNo[ uThreadIndex ] ) {
        uHandlesToClose = auHandlesNo[ uThreadIndex ] - uIndexStart;
    }

    if ( uHandlesToClose == 0 ) {
        //
        // nothing to do
        //
        return;
    }

    //
    // close the handles
    //

    uIndexEnd = uIndexStart + uHandlesToClose;

    ASSERT( uIndexEnd <= auHandlesNo[ uThreadIndex ] );

    /*
    DbgMessage(
        "CloseHandles: total %u, start %u, end %u, will close %u\n",
        auHandlesNo[ uThreadIndex ],
        uIndexStart,
        uIndexEnd,
        uHandlesToClose
        );
    */

    for ( uCrtHandle = uIndexStart; uCrtHandle < uIndexEnd; uCrtHandle++ ) {
        CloseHandle( aHandles[ uThreadIndex ][ uCrtHandle ] );
    }

    if ( uIndexEnd < auHandlesNo[ uThreadIndex ] ) {
        //
        // keep the end of the buffer
        //

        MoveMemory(
            &aHandles[ uThreadIndex ][ uIndexStart ],
            &aHandles[ uThreadIndex ][ uIndexEnd ],
            ( auHandlesNo[ uThreadIndex ] - uIndexEnd ) * sizeof( aHandles[ uThreadIndex ][ uIndexEnd ] )
            );
    }

    ASSERT( auHandlesNo[ uThreadIndex ] >= uHandlesToClose );

    auHandlesNo[ uThreadIndex ] -= uHandlesToClose;

    /*
    DbgMessage(
        "CloseHandles( %u ) - end - %u handles now\n",
        uThreadIndex,
        auHandlesNo[ uThreadIndex ]
        );
    */
}


void
DuplicateHandles( 
    UINT uThreadIndex
    )
{
    UINT uIndexStart;
    UINT uIndexEnd;
    UINT uCrtHandle;
    UINT uHandlesToDup;
    UINT uCrtHandleNumber;
    BOOL bResult;
    HANDLE hCurrentProcess;
    ULONG Seed = GetTickCount();

    if ( auHandlesNo[ uThreadIndex ] == 0 ) {
        //
        // nothing to duplicate
        //
        return;
    }

    hCurrentProcess = NtCurrentProcess();

    //
    // start duplicating from this index
    //

    uIndexStart = abs( RtlRandom(&Seed) ) % auHandlesNo[ uThreadIndex ];
    
    //
    // how many handles to duplicate
    //

    uHandlesToDup = abs( RtlRandom(&Seed) ) % MAX_OBJECTS_PER_LOOP;

    if ( uIndexStart + uHandlesToDup > auHandlesNo[ uThreadIndex ] ) {
        uHandlesToDup = auHandlesNo[ uThreadIndex ] - uIndexStart;
    }

    if ( auHandlesNo[ uThreadIndex ] + uHandlesToDup > MAX_OBJECTS_PER_THREAD ) {
        uHandlesToDup = MAX_OBJECTS_PER_THREAD - auHandlesNo[ uThreadIndex ];
    }

    if ( uHandlesToDup == 0 ) {
        //
        // nothing to do
        //
        return;
    }

    //
    // duplicate the handles
    //

    uIndexEnd = uIndexStart + uHandlesToDup;

    ASSERT( uIndexEnd <= auHandlesNo[ uThreadIndex ] );
    ASSERT( auHandlesNo[ uThreadIndex ] + uHandlesToDup <= MAX_OBJECTS_PER_THREAD );

    uCrtHandleNumber = auHandlesNo[ uThreadIndex ];

    for ( uCrtHandle = uIndexStart; uCrtHandle < uIndexEnd; uCrtHandle++ ) {
        bResult = DuplicateHandle( 
                        hCurrentProcess,
                        aHandles[ uThreadIndex ][ uCrtHandle ],
                        hCurrentProcess,
                        &aHandles[ uThreadIndex ][ uCrtHandleNumber ],
                        DUPLICATE_SAME_ACCESS,
                        FALSE,
                        0
                        );
        
        if ( bResult == FALSE ) {
            DbgMessage(
                "Thread %u: DuplicateHandles: cannot duplicate handle %p\n",
                uThreadIndex,
                aHandles[ uThreadIndex ][ uCrtHandle ]
                );
        } else {
            uCrtHandleNumber++;
        }
    }

    auHandlesNo[ uThreadIndex ] = uCrtHandleNumber;
}


DWORD
WINAPI 
StressHandleTableThread( 
	LPVOID lpParam
    )
{
    UINT uStressThreadIndex;
    UINT uCrtLoop;
    UINT uOperationType;
    BOOL bContinue;
    DWORD dwSleepTime;
    ULONG Seed = GetTickCount();

    uStressThreadIndex = PtrToUlong( lpParam );

    //
    // loop
    //

    for ( uCrtLoop = 0; uCrtLoop < LOOPS_NO_PER_THREAD; uCrtLoop++ ) {
        //
        // choose an action to take
        //

        uOperationType = abs( RtlRandom(&Seed) ) % StressOperationsNo;

        switch ( uOperationType ) {
        case StressCloseHandles:
            //
            // close some of our handles
            //
            CloseHandles( uStressThreadIndex );
            break;

        case StressDuplicateHandles:
            //
            // duplicate some of our handles
            //
            DuplicateHandles( uStressThreadIndex );
            break;

        default:
            //
            // create new objects
            //

            bContinue = CreateObjects( uStressThreadIndex, uOperationType );

            if ( bContinue == FALSE ) {
                //
                // our internal handle table is full
                //
                DbgMessage(
                    "Stress thread %u, returning...\n", 
                    uStressThreadIndex
                    );

                return 0;
            }
        }

        //
        // sleep for some time
        //

        if ( ( uCrtLoop % 100 ) == 0 ) {
            dwSleepTime = abs( RtlRandom(&Seed) ) % 5000;

            DbgMessage(
                "Stress thread %u, sleeping...\n", 
                uStressThreadIndex
                );

            Sleep( dwSleepTime );
        }
    }

    return 0;
}


DWORD
WINAPI
CleanHandleTableThread( 
    LPVOID lpParam
    )
{
    UINT uCrtHandle;
    UINT uThreadIndex;
    ULONG Seed = GetTickCount();

    uThreadIndex = PtrToUlong( lpParam );

    /*
    DbgMessage(
        "CleanHandleTableThread( %u ) - begin - %u handles for now\n",
        uThreadIndex,
        auHandlesNo[ uThreadIndex ]
        );
    */

    //
    // close the handles corresponding to this thread
    //

    if ( ( abs( RtlRandom(&Seed) ) % 2 ) == 0 ) {
        for ( uCrtHandle = 0; uCrtHandle < auHandlesNo[ uThreadIndex ]; uCrtHandle++ ) {
            CloseHandle( aHandles[ uThreadIndex ][ uCrtHandle ] );
        }
    } else {
        for ( uCrtHandle = auHandlesNo[ uThreadIndex ]; uCrtHandle > 0 ; uCrtHandle-- ) {
            CloseHandle( aHandles[ uThreadIndex ][ uCrtHandle - 1 ] );
        }
    }

    auHandlesNo[ uThreadIndex ] = 0;
    return 0;
}
