/*++

Copyright (c) 1999  Microsoft Corporation

Module Name:

    process.c

Abstract:

    Process management routines

--*/

#include "basedll.h"
#pragma hdrstop

BOOL
WINAPI
IsBadReadPtr(
    CONST VOID *lp,
    UINT_PTR cb
    )

/*++

Routine Description:

    This function verifies that the range of memory specified by the
    input parameters can be read by the calling process.

    If the entire range of memory is accessible, then a value of FALSE
    is returned; otherwise, a value of TRUE is returned.

    Note that since Win32 is a pre-emptive multi-tasking environment,
    the results of this test are only meaningful if the other threads in
    the process do not manipulate the range of memory being tested by
    this call.  Even after a pointer validation, an application should
    use the structured exception handling capabilities present in the
    system to guard access through pointers that it does not control.

Arguments:

    lp - Supplies the base address of the memory that is to be checked
        for read access.

    cb - Supplies the length in bytes to be checked.

Return Value:

    TRUE - Some portion of the specified range of memory is not accessible
        for read access.

    FALSE - All pages within the specified range have been successfully
        read.

--*/

{

    PSZ EndAddress;
    PSZ StartAddress;

    //
    // If the structure has zero length, then do not probe the structure for
    // read accessibility or alignment.
    //

    if (cb != 0) {

        //
        // If it is a NULL pointer just return TRUE, they are always bad
        //
        if (lp == NULL) {
            return TRUE;
            }

        StartAddress = (PSZ)lp;

        //
        // Compute the ending address of the structure and probe for
        // read accessibility.
        //

        EndAddress = StartAddress + cb - 1;
        if ( EndAddress < StartAddress ) {
           return TRUE;
            }
        else {
            try {
                *(volatile CHAR *)StartAddress;
                StartAddress = (PCHAR)((ULONG_PTR)StartAddress & (~((LONG)PAGE_SIZE - 1)));
                EndAddress = (PCHAR)((ULONG_PTR)EndAddress & (~((LONG)PAGE_SIZE - 1)));
                while (StartAddress != EndAddress) {
                    StartAddress = StartAddress + PAGE_SIZE;
                    *(volatile CHAR *)StartAddress;
                    }
                }
            except(EXCEPTION_EXECUTE_HANDLER) {
                return TRUE;
                }
            }
        }
    return FALSE;
}

BOOL
WINAPI
IsBadWritePtr(
    LPVOID lp,
    UINT_PTR cb
    )
/*++

Routine Description:

    This function verifies that the range of memory specified by the
    input parameters can be written by the calling process.

    If the entire range of memory is accessible, then a value of FALSE
    is returned; otherwise, a value of TRUE is returned.

    Note that since Win32 is a pre-emptive multi-tasking environment,
    the results of this test are only meaningful if the other threads in
    the process do not manipulate the range of memory being tested by
    this call.  Even after a pointer validation, an application should
    use the structured exception handling capabilities present in the
    system to guard access through pointers that it does not control.

    Also not that implementations are free to do a write test by reading
    a value and then writing it back.

Arguments:

    lp - Supplies the base address of the memory that is to be checked
        for write access.

    cb - Supplies the length in bytes to be checked.

Return Value:

    TRUE - Some portion of the specified range of memory is not accessible
        for write access.

    FALSE - All pages within the specified range have been successfully
        written.

--*/
{
    PSZ EndAddress;
    PSZ StartAddress;

    //
    // If the structure has zero length, then do not probe the structure for
    // write accessibility.
    //

    if (cb != 0) {

        //
        // If it is a NULL pointer just return TRUE, they are always bad
        //
        if (lp == NULL) {
            return TRUE;
            }

        StartAddress = (PCHAR)lp;

        //
        // Compute the ending address of the structure and probe for
        // write accessibility.
        //

        EndAddress = StartAddress + cb - 1;
        if ( EndAddress < StartAddress ) {
            return TRUE;
            }
        else {
            try {
                *(volatile CHAR *)StartAddress = *(volatile CHAR *)StartAddress;
                StartAddress = (PCHAR)((ULONG_PTR)StartAddress & (~((LONG)PAGE_SIZE - 1)));
                EndAddress = (PCHAR)((ULONG_PTR)EndAddress & (~((LONG)PAGE_SIZE - 1)));
                while (StartAddress != EndAddress) {
                    StartAddress = StartAddress + PAGE_SIZE;
                    *(volatile CHAR *)StartAddress = *(volatile CHAR *)StartAddress;
                    }
                }
            except(EXCEPTION_EXECUTE_HANDLER) {
                return TRUE;
                }
            }
        }
    return FALSE;
}

BOOL
WINAPI
IsBadCodePtr(
    FARPROC lpfn
    )

/*++

    Same as IsBadReadPtr with a length of 1

--*/

{
    return IsBadReadPtr((LPVOID)lpfn,1);
}

BOOL
WINAPI
IsBadStringPtrW(
    LPCWSTR lpsz,
    UINT_PTR cchMax
    )

/*++

Routine Description:

    This function verifies that the range of memory specified by the
    input parameters can be read by the calling process.

    The range is the smaller of the number of bytes covered by the
    specified NULL terminated UNICODE string, or the number of bytes
    specified by cchMax.

    If the entire range of memory is accessible, then a value of FALSE
    is returned; otherwise, a value of TRUE is returned.

    Note that since Win32 is a pre-emptive multi-tasking environment,
    the results of this test are only meaningful if the other threads in
    the process do not manipulate the range of memory being tested by
    this call.  Even after a pointer validation, an application should
    use the structured exception handling capabilities present in the
    system to guard access through pointers that it does not control.

Arguments:

    lpsz - Supplies the base address of the memory that is to be checked
        for read access.

    cchMax - Supplies the length in characters to be checked.

Return Value:

    TRUE - Some portion of the specified range of memory is not accessible
        for read access.

    FALSE - All pages within the specified range have been successfully
        read.

--*/

{

    LPCWSTR EndAddress;
    LPCWSTR StartAddress;
    WCHAR c;

    //
    // If the structure has zero length, then do not probe the structure for
    // read accessibility.
    //

    if (cchMax != 0) {

        //
        // If it is a NULL pointer just return TRUE, they are always bad
        //
        if (lpsz == NULL) {
            return TRUE;
            }

        StartAddress = lpsz;

        //
        // Compute the ending address of the structure and probe for
        // read accessibility.
        //

        EndAddress = (LPCWSTR)((PSZ)StartAddress + (cchMax*sizeof(WCHAR)) - sizeof(WCHAR));
        try {
            c = *(volatile WCHAR *)StartAddress;
            while ( c && StartAddress != EndAddress ) {
                StartAddress++;
                c = *(volatile WCHAR *)StartAddress;
                }
            }
        except(EXCEPTION_EXECUTE_HANDLER) {
            return TRUE;
            }
        }
    return FALSE;
}

BOOL
WINAPI
IsBadStringPtrA(
    LPCSTR lpsz,
    UINT_PTR cchMax
    )

/*++

Routine Description:

    This function verifies that the range of memory specified by the
    input parameters can be read by the calling process.

    The range is the smaller of the number of bytes covered by the
    specified NULL terminated UNICODE string, or the number of bytes
    specified by cchMax.

    If the entire range of memory is accessible, then a value of FALSE
    is returned; otherwise, a value of TRUE is returned.

    Note that since Win32 is a pre-emptive multi-tasking environment,
    the results of this test are only meaningful if the other threads in
    the process do not manipulate the range of memory being tested by
    this call.  Even after a pointer validation, an application should
    use the structured exception handling capabilities present in the
    system to guard access through pointers that it does not control.

Arguments:

    lpsz - Supplies the base address of the memory that is to be checked
        for read access.

    cchMax - Supplies the length in characters to be checked.

Return Value:

    TRUE - Some portion of the specified range of memory is not accessible
        for read access.

    FALSE - All pages within the specified range have been successfully
        read.

--*/

{

    LPCSTR EndAddress;
    LPCSTR StartAddress;
    CHAR c;

    //
    // If the structure has zero length, then do not probe the structure for
    // read accessibility.
    //

    if (cchMax != 0) {

        //
        // If it is a NULL pointer just return TRUE, they are always bad
        //
        if (lpsz == NULL) {
            return TRUE;
            }

        StartAddress = lpsz;

        //
        // Compute the ending address of the structure and probe for
        // read accessibility.
        //

        EndAddress = (LPCSTR)((PSZ)StartAddress + (cchMax*sizeof(CHAR)) - sizeof(CHAR));
        try {
            c = *(volatile CHAR *)StartAddress;
            while ( c && StartAddress != EndAddress ) {
                StartAddress++;
                c = *(volatile CHAR *)StartAddress;
                }
            }
        except(EXCEPTION_EXECUTE_HANDLER) {
            return TRUE;
            }
        }
    return FALSE;
}
