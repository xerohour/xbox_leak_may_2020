/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    support.c

Abstract:

    This module implements various conversion routines
    that transform Win32 parameters into NT parameters.

Author:

    Mark Lucovsky (markl) 20-Sep-1990

Revision History:

--*/

#include "basedll.h"

POBJECT_ATTRIBUTES
XapiFormatObjectAttributes(
    OUT POBJECT_ATTRIBUTES ObjectAttributes,
    OUT POBJECT_STRING ObjectName,
    IN PCOSTR lpName
    )

/*++

Routine Description:

    This function transforms a Win32 security attributes structure into
    an NT object attributes structure.  It returns the address of the
    resulting structure (or NULL if SecurityAttributes was not
    specified).

Arguments:

    ObjectAttributes - Returns an initialized NT object attributes structure.

    ObjectName - Returns an initialized OBJECT_STRING structure.

    lpName - Supplies the name of the object relative to the
        ObWin32NamedObjectsDirectory() object directory.

Return Value:

    NON-NULL - Returns the ObjectAttributes value.  The structure is
        properly initialized by this function.

--*/

{
    RtlInitObjectString(ObjectName, lpName);

    InitializeObjectAttributes(
        ObjectAttributes,
        ObjectName,
        OBJ_OPENIF,
        ObWin32NamedObjectsDirectory(),
        NULL
        );

    return ObjectAttributes;
}

PLARGE_INTEGER
XapiFormatTimeOut(
    OUT PLARGE_INTEGER TimeOut,
    IN DWORD Milliseconds
    )

/*++

Routine Description:

    This function translates a Win32 style timeout to an NT relative
    timeout value.

Arguments:

    TimeOut - Returns an initialized NT timeout value that is equivalent
         to the Milliseconds parameter.

    Milliseconds - Supplies the timeout value in milliseconds.  A value
         of -1 indicates indefinite timeout.

Return Value:


    NULL - A value of null should be used to mimic the behavior of the
        specified Milliseconds parameter.

    NON-NULL - Returns the TimeOut value.  The structure is properly
        initialized by this function.

--*/

{
    if ( (LONG) Milliseconds == -1 ) {
        return( NULL );
        }
    TimeOut->QuadPart = UInt32x32To64( Milliseconds, 10000 );
    TimeOut->QuadPart *= -1;
    return TimeOut;
}
