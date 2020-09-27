/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    stress.h

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


#ifndef __STRESS_HXX_INCLUDED__
#define __STRESS_HXX_INCLUDED__


#define THREADS_NUMBER              10
#define MAX_OBJECTS_PER_THREAD      50000

extern HANDLE *aHandles[ THREADS_NUMBER ];
extern UINT auHandlesNo[ THREADS_NUMBER ];


DWORD
WINAPI 
StressHandleTableThread( 
	LPVOID lpParam
    );


DWORD
WINAPI
CleanHandleTableThread( 
    LPVOID lpParam
    );


#endif // __STRESS_HXX_INCLUDED__
