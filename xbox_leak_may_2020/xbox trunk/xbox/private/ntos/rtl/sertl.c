/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    sertl.c

Abstract:

    This Module implements many security rtl routines defined in ntseapi.h

Author:

    Jim Kelly       (JimK)     23-Mar-1990
    Robert Reichel  (RobertRe)  1-Mar-1991

Environment:

    Pure Runtime Library Routine

Revision History:


--*/

#include "ntrtlp.h"

VOID
RtlMapGenericMask(
    IN OUT PACCESS_MASK AccessMask,
    IN PGENERIC_MAPPING GenericMapping
    )

/*++

Routine Description:

    This routine maps all generic accesses in the provided access mask
    to specific and standard accesses according to the provided
    GenericMapping.

Arguments:

        AccessMask - Points to the access mask to be mapped.

        GenericMapping - The mapping of generic to specific and standard
                         access types.

Return Value:

    None.

--*/

{
    RTL_PAGED_CODE();

    if (*AccessMask & GENERIC_READ) {

        *AccessMask |= GenericMapping->GenericRead;
    }

    if (*AccessMask & GENERIC_WRITE) {

        *AccessMask |= GenericMapping->GenericWrite;
    }

    if (*AccessMask & GENERIC_EXECUTE) {

        *AccessMask |= GenericMapping->GenericExecute;
    }

    if (*AccessMask & GENERIC_ALL) {

        *AccessMask |= GenericMapping->GenericAll;
    }

    //
    // Now clear the generic flags
    //

    *AccessMask &= ~(GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL);

    return;
}
