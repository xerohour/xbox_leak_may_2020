/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    nls.c

Abstract:

    This module implements NLS support functions for NT.

Author:

    Mark Lucovsky (markl) 16-Apr-1991

Environment:

    Kernel or user-mode

Revision History:

    16-Feb-1993    JulieB    Added Upcase Rtl Routines.
    08-Mar-1993    JulieB    Moved Upcase Macro to ntrtlp.h.
    02-Apr-1993    JulieB    Fixed RtlAnsiCharToUnicodeChar to use transl. tbls.
    02-Apr-1993    JulieB    Fixed BUFFER_TOO_SMALL check.
    28-May-1993    JulieB    Fixed code to properly handle DBCS.

--*/

#include "ntrtlp.h"

NTSTATUS
RtlAnsiStringToUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PANSI_STRING SourceString,
    IN BOOLEAN AllocateDestinationString
    )

/*++

Routine Description:

    This functions converts the specified ansi source string into a
    Unicode string. The translation is done with respect to the
    current system locale information.

Arguments:

    DestinationString - Returns a unicode string that is equivalent to
        the ansi source string. The maximum length field is only
        set if AllocateDestinationString is TRUE.

    SourceString - Supplies the ansi source string that is to be
        converted to unicode.

    AllocateDestinationString - Supplies a flag that controls whether or
        not this API allocates the buffer space for the destination
        string.  If it does, then the buffer must be deallocated using
        RtlFreeUnicodeString (note that only storage for
        DestinationString->Buffer is allocated by this API).

Return Value:

    SUCCESS - The conversion was successful

    !SUCCESS - The operation failed.  No storage was allocated and no
        conversion was done.  None.

--*/

{
    ULONG UnicodeLength;
    ULONG Index;
    NTSTATUS st;

    RTL_PAGED_CODE();

    UnicodeLength = RtlAnsiStringToUnicodeSize(SourceString);
    if ( UnicodeLength > MAXUSHORT ) {
        return STATUS_INVALID_PARAMETER_2;
        }

    DestinationString->Length = (USHORT)(UnicodeLength - sizeof(UNICODE_NULL));
    if ( AllocateDestinationString ) {
        DestinationString->MaximumLength = (USHORT)UnicodeLength;
        DestinationString->Buffer = RtlAllocateStringRoutine(UnicodeLength);
        if ( !DestinationString->Buffer ) {
            return STATUS_NO_MEMORY;
            }
        }
    else {
        if ( DestinationString->Length >= DestinationString->MaximumLength ) {
            return STATUS_BUFFER_OVERFLOW;
            }
        }

    st = RtlMultiByteToUnicodeN(
             DestinationString->Buffer,
             DestinationString->Length,
             &Index,
             SourceString->Buffer,
             SourceString->Length
             );

    if (!NT_SUCCESS(st)) {
        if ( AllocateDestinationString ) {
            RtlFreeStringRoutine(DestinationString->Buffer);
            DestinationString->Buffer = NULL;
        }

        return st;
    }

    DestinationString->Buffer[Index / sizeof(WCHAR)] = UNICODE_NULL;

    return STATUS_SUCCESS;
}

NTSTATUS
RtlUnicodeStringToAnsiString(
    OUT PANSI_STRING DestinationString,
    IN PUNICODE_STRING SourceString,
    IN BOOLEAN AllocateDestinationString
    )

/*++

Routine Description:

    This functions converts the specified unicode source string into an
    ansi string. The translation is done with respect to the
    current system locale information.

Arguments:

    DestinationString - Returns an ansi string that is equivalent to the
        unicode source string.  If the translation can not be done,
        an error is returned.  The maximum length field is only set if
        AllocateDestinationString is TRUE.

    SourceString - Supplies the unicode source string that is to be
        converted to ansi.

    AllocateDestinationString - Supplies a flag that controls whether or
        not this API allocates the buffer space for the destination
        string.  If it does, then the buffer must be deallocated using
        RtlFreeAnsiString (note that only storage for
        DestinationString->Buffer is allocated by this API).

Return Value:

    SUCCESS - The conversion was successful

    !SUCCESS - The operation failed.  No storage was allocated and no
        conversion was done.  None.

--*/

{
    ULONG AnsiLength;
    ULONG Index;
    NTSTATUS st;
    NTSTATUS ReturnStatus = STATUS_SUCCESS;

    RTL_PAGED_CODE();

    AnsiLength = RtlUnicodeStringToAnsiSize(SourceString);
    if ( AnsiLength > MAXUSHORT ) {
        return STATUS_INVALID_PARAMETER_2;
        }

    DestinationString->Length = (USHORT)(AnsiLength - 1);
    if ( AllocateDestinationString ) {
        DestinationString->MaximumLength = (USHORT)AnsiLength;
        DestinationString->Buffer = RtlAllocateStringRoutine(AnsiLength);
        if ( !DestinationString->Buffer ) {
            return STATUS_NO_MEMORY;
            }
        }
    else {
        if ( DestinationString->Length >= DestinationString->MaximumLength ) {
            /*
             * Return STATUS_BUFFER_OVERFLOW, but translate as much as
             * will fit into the buffer first.  This is the expected
             * behavior for routines such as GetProfileStringA.
             * Set the length of the buffer to one less than the maximum
             * (so that the trail byte of a double byte char is not
             * overwritten by doing DestinationString->Buffer[Index] = '\0').
             * RtlUnicodeToMultiByteN is careful not to truncate a
             * multibyte character.
             */
            if (!DestinationString->MaximumLength) {
                return STATUS_BUFFER_OVERFLOW;
            }
            ReturnStatus = STATUS_BUFFER_OVERFLOW;
            DestinationString->Length = DestinationString->MaximumLength - 1;
            }
        }

    st = RtlUnicodeToMultiByteN(
             DestinationString->Buffer,
             DestinationString->Length,
             &Index,
             SourceString->Buffer,
             SourceString->Length
             );

    if (!NT_SUCCESS(st)) {
        if ( AllocateDestinationString ) {
            RtlFreeStringRoutine(DestinationString->Buffer);
            DestinationString->Buffer = NULL;
        }

        return st;
    }

    DestinationString->Buffer[Index] = '\0';

    return ReturnStatus;
}

NTSTATUS
RtlUpcaseUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PCUNICODE_STRING SourceString,
    IN BOOLEAN AllocateDestinationString
    )

/*++

Routine Description:

    This functions converts the specified unicode source string into an
    upcased unicode string. The translation is done with respect to the
    current system locale information.

Arguments:

    DestinationString - Returns a unicode string that is the upcased equivalent
        to the unicode source string.  The maximum length field is only set if
        AllocateDestinationString is TRUE.

    SourceString - Supplies the unicode source string that is to being
        upcased.

    AllocateDestinationString - Supplies a flag that controls whether or
        not this API allocates the buffer space for the destination
        string.  If it does, then the buffer must be deallocated using
        RtlFreeUnicodeString (note that only storage for
        DestinationString->Buffer is allocated by this API).

Return Value:

    SUCCESS - The conversion was successful

    !SUCCESS - The operation failed.  No storage was allocated and no
        conversion was done.  None.

--*/

{
    ULONG Index;
    ULONG StopIndex;

    RTL_PAGED_CODE();

    if ( AllocateDestinationString ) {
        DestinationString->MaximumLength = SourceString->Length;
        DestinationString->Buffer = RtlAllocateStringRoutine((ULONG)DestinationString->MaximumLength);
        if ( !DestinationString->Buffer ) {
            return STATUS_NO_MEMORY;
            }
        }
    else {
        if ( SourceString->Length > DestinationString->MaximumLength ) {
            return STATUS_BUFFER_OVERFLOW;
            }
        }

    StopIndex = ((ULONG)SourceString->Length) / sizeof( WCHAR );

    for (Index = 0; Index < StopIndex; Index++) {
        DestinationString->Buffer[Index] = (WCHAR)NLS_UPCASE(SourceString->Buffer[Index]);
    }

    DestinationString->Length = SourceString->Length;

    return STATUS_SUCCESS;
}


NTSTATUS
RtlDowncaseUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PUNICODE_STRING SourceString,
    IN BOOLEAN AllocateDestinationString
    )

/*++

Routine Description:

    This functions converts the specified unicode source string into a
    downcased unicode string. The translation is done with respect to the
    current system locale information.

Arguments:

    DestinationString - Returns a unicode string that is the downcased
        equivalent to the unicode source string.  The maximum length field
        is only set if AllocateDestinationString is TRUE.

    SourceString - Supplies the unicode source string that is to being
        downcased.

    AllocateDestinationString - Supplies a flag that controls whether or
        not this API allocates the buffer space for the destination
        string.  If it does, then the buffer must be deallocated using
        RtlFreeUnicodeString (note that only storage for
        DestinationString->Buffer is allocated by this API).

Return Value:

    SUCCESS - The conversion was successful

    !SUCCESS - The operation failed.  No storage was allocated and no
        conversion was done.  None.

--*/

{
    ULONG Index;
    ULONG StopIndex;

    RTL_PAGED_CODE();

    if ( AllocateDestinationString ) {
        DestinationString->MaximumLength = SourceString->Length;
        DestinationString->Buffer = RtlAllocateStringRoutine((ULONG)DestinationString->MaximumLength);
        if ( !DestinationString->Buffer ) {
            return STATUS_NO_MEMORY;
            }
        }
    else {
        if ( SourceString->Length > DestinationString->MaximumLength ) {
            return STATUS_BUFFER_OVERFLOW;
            }
        }

    StopIndex = ((ULONG)SourceString->Length) / sizeof( WCHAR );

    for (Index = 0; Index < StopIndex; Index++) {
        DestinationString->Buffer[Index] = (WCHAR)NLS_DOWNCASE(SourceString->Buffer[Index]);
    }

    DestinationString->Length = SourceString->Length;

    return STATUS_SUCCESS;
}


WCHAR
RtlUpcaseUnicodeChar(
    IN WCHAR SourceCharacter
    )

/*++

Routine Description:

    This function translates the specified unicode character to its
    equivalent upcased unicode chararacter.  The purpose for this routine
    is to allow for character by character upcase translation.  The
    translation is done with respect to the current system locale
    information.


Arguments:

    SourceCharacter - Supplies the unicode character to be upcased.

Return Value:

    Returns the upcased unicode equivalent of the specified input character.

--*/

{
    RTL_PAGED_CODE();

    //
    // Note that this needs to reference the translation table !
    //

    return (WCHAR)NLS_UPCASE(SourceCharacter);
}



WCHAR
RtlDowncaseUnicodeChar(
    IN WCHAR SourceCharacter
    )

/*++

Routine Description:

    This function translates the specified unicode character to its
    equivalent downcased unicode chararacter.  The purpose for this routine
    is to allow for character by character downcase translation.  The
    translation is done with respect to the current system locale
    information.


Arguments:

    SourceCharacter - Supplies the unicode character to be downcased.

Return Value:

    Returns the downcased unicode equivalent of the specified input character.

--*/

{
    RTL_PAGED_CODE();

    //
    // Note that this needs to reference the translation table !
    //

    return (WCHAR)NLS_DOWNCASE(SourceCharacter);
}


VOID
RtlFreeUnicodeString(
    IN OUT PUNICODE_STRING UnicodeString
    )

/*++

Routine Description:

    This API is used to free storage allocated by
    RtlAnsiStringToUnicodeString.  Note that only UnicodeString->Buffer
    is free'd by this routine.

Arguments:

    UnicodeString - Supplies the address of the unicode string whose
        buffer was previously allocated by RtlAnsiStringToUnicodeString.

Return Value:

    None.

--*/

{
    RTL_PAGED_CODE();

    if (UnicodeString->Buffer) {
        RtlFreeStringRoutine(UnicodeString->Buffer);
        memset( UnicodeString, 0, sizeof( *UnicodeString ) );
        }
}


VOID
RtlFreeAnsiString(
    IN OUT PANSI_STRING AnsiString
    )

/*++

Routine Description:

    This API is used to free storage allocated by
    RtlUnicodeStringToAnsiString.  Note that only AnsiString->Buffer
    is free'd by this routine.

Arguments:

    AnsiString - Supplies the address of the ansi string whose buffer
        was previously allocated by RtlUnicodeStringToAnsiString.

Return Value:

    None.

--*/

{
    RTL_PAGED_CODE();

    if (AnsiString->Buffer) {
        RtlFreeStringRoutine(AnsiString->Buffer);
        memset( AnsiString, 0, sizeof( *AnsiString ) );
        }
}


LONG
RtlCompareUnicodeString(
    IN PUNICODE_STRING String1,
    IN PUNICODE_STRING String2,
    IN BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlCompareUnicodeString function compares two counted strings.  The
    return value indicates if the strings are equal or String1 is less than
    String2 or String1 is greater than String2.

    The CaseInSensitive parameter specifies if case is to be ignored when
    doing the comparison.

Arguments:

    String1 - Pointer to the first string.

    String2 - Pointer to the second string.

    CaseInsensitive - TRUE if case should be ignored when doing the
        comparison.

Return Value:

    Signed value that gives the results of the comparison:

        Zero - String1 equals String2

        < Zero - String1 less than String2

        > Zero - String1 greater than String2


--*/

{

    PWCHAR s1, s2, Limit;
    LONG n1, n2;
    WCHAR c1, c2;

    s1 = String1->Buffer;
    s2 = String2->Buffer;
    n1 = String1->Length;
    n2 = String2->Length;

    ASSERT((n1 & 1) == 0);
    ASSERT((n2 & 1) == 0);
    ASSERT(!(((((ULONG_PTR)s1 & 1) != 0) || (((ULONG_PTR)s2 & 1) != 0)) && (n1 != 0) && (n2 != 0)));

    Limit = (PWCHAR)((PCHAR)s1 + (n1 <= n2 ? n1 : n2));
    if (CaseInSensitive) {
        while (s1 < Limit) {
            c1 = *s1++;
            c2 = *s2++;
            if (c1 != c2) {

                //
                // Note that this needs to reference the translation table!
                //

                c1 = NLS_UPCASE(c1);
                c2 = NLS_UPCASE(c2);
                if (c1 != c2) {
                    return (LONG)(c1) - (LONG)(c2);
                }
            }
        }

    } else {
        while (s1 < Limit) {
            c1 = *s1++;
            c2 = *s2++;
            if (c1 != c2) {
                return (LONG)(c1) - (LONG)(c2);
            }
        }
    }

    return n1 - n2;
}


BOOLEAN
RtlEqualUnicodeString(
    IN PCUNICODE_STRING String1,
    IN PCUNICODE_STRING String2,
    IN BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlEqualUnicodeString function compares two counted unicode strings for
    equality.

    The CaseInSensitive parameter specifies if case is to be ignored when
    doing the comparison.

Arguments:

    String1 - Pointer to the first string.

    String2 - Pointer to the second string.

    CaseInsensitive - TRUE if case should be ignored when doing the
        comparison.

Return Value:

    Boolean value that is TRUE if String1 equals String2 and FALSE otherwise.

--*/

{

    PWCHAR s1, s2, Limit;
    LONG n1, n2;
    WCHAR c1, c2;

    n1 = String1->Length;
    n2 = String2->Length;

    ASSERT((n1 & 1) == 0);
    ASSERT((n2 & 1) == 0);

    if (n1 == n2) {
        s1 = String1->Buffer;
        s2 = String2->Buffer;

        ASSERT(!(((((ULONG_PTR)s1 & 1) != 0) || (((ULONG_PTR)s2 & 1) != 0)) && (n1 != 0) && (n2 != 0)));

        Limit = (PWCHAR)((PCHAR)s1 + n1);
        if (CaseInSensitive) {
            while (s1 < Limit) {
                c1 = *s1++;
                c2 = *s2++;
                if ((c1 != c2) && (NLS_UPCASE(c1) != NLS_UPCASE(c2))) {
                    return FALSE;
                }
            }

            return TRUE;

        } else {
            while (s1 < Limit) {
                c1 = *s1++;
                c2 = *s2++;
                if (c1 != c2) {
                    return FALSE;
                }
            }

            return TRUE;
        }

    } else {
        return FALSE;
    }
}


BOOLEAN
RtlPrefixUnicodeString(
    IN PUNICODE_STRING String1,
    IN PUNICODE_STRING String2,
    IN BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlPrefixUnicodeString function determines if the String1
    counted string parameter is a prefix of the String2 counted string
    parameter.

    The CaseInSensitive parameter specifies if case is to be ignored when
    doing the comparison.

Arguments:

    String1 - Pointer to the first unicode string.

    String2 - Pointer to the second unicode string.

    CaseInsensitive - TRUE if case should be ignored when doing the
        comparison.

Return Value:

    Boolean value that is TRUE if String1 equals a prefix of String2 and
    FALSE otherwise.

--*/

{
    PWSTR s1, s2;
    ULONG n;
    WCHAR c1, c2;

    s1 = String1->Buffer;
    s2 = String2->Buffer;
    n = String1->Length;
    if (String2->Length < n) {
        return( FALSE );
        }

    n = n / sizeof(c1);
    if (CaseInSensitive) {
        while (n) {
            c1 = *s1++;
            c2 = *s2++;

            if ((c1 != c2) && (NLS_UPCASE(c1) != NLS_UPCASE(c2))) {
                return( FALSE );
                }

            n--;
            }
        }
    else {
        while (n) {
            if (*s1++ != *s2++) {
                return( FALSE );
                }

            n--;
            }
        }

    return TRUE;
}


VOID
RtlCopyUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PUNICODE_STRING SourceString OPTIONAL
    )

/*++

Routine Description:

    The RtlCopyString function copies the SourceString to the
    DestinationString.  If SourceString is not specified, then
    the Length field of DestinationString is set to zero.  The
    MaximumLength and Buffer fields of DestinationString are not
    modified by this function.

    The number of bytes copied from the SourceString is either the
    Length of SourceString or the MaximumLength of DestinationString,
    whichever is smaller.

Arguments:

    DestinationString - Pointer to the destination string.

    SourceString - Optional pointer to the source string.

Return Value:

    None.

--*/

{
    UNALIGNED WCHAR *src, *dst;
    ULONG n;

    if (ARGUMENT_PRESENT(SourceString)) {
        dst = DestinationString->Buffer;
        src = SourceString->Buffer;
        n = SourceString->Length;
        if ((USHORT)n > DestinationString->MaximumLength) {
            n = DestinationString->MaximumLength;
        }

        DestinationString->Length = (USHORT)n;
        RtlCopyMemory(dst, src, n);
        if (DestinationString->Length < DestinationString->MaximumLength) {
            dst[n / sizeof(WCHAR)] = UNICODE_NULL;
        }

    } else {
        DestinationString->Length = 0;
    }

    return;
}


NTSTATUS
RtlAppendUnicodeToString (
    IN PUNICODE_STRING Destination,
    IN PCWSTR Source OPTIONAL
    )

/*++

Routine Description:

    This routine appends the supplied UNICODE string to an existing
    PUNICODE_STRING.

    It will copy bytes from the Source PSZ to the destination PSTRING up to
    the destinations PUNICODE_STRING->MaximumLength field.

Arguments:

    IN PUNICODE_STRING Destination, - Supplies a pointer to the destination
                            string
    IN PWSTR Source - Supplies the string to append to the destination

Return Value:

    STATUS_SUCCESS - The source string was successfully appended to the
        destination counted string.

    STATUS_BUFFER_TOO_SMALL - The destination string length was not big
        enough to allow the source string to be appended.  The Destination
        string length is not updated.

--*/

{
    USHORT n;
    UNALIGNED WCHAR *dst;

    if (ARGUMENT_PRESENT( Source )) {
        UNICODE_STRING UniSource;

        RtlInitUnicodeString(&UniSource, Source);

        n = UniSource.Length;

        if ((n + Destination->Length) > Destination->MaximumLength) {
            return( STATUS_BUFFER_TOO_SMALL );
            }

        dst = &Destination->Buffer[ (Destination->Length / sizeof( WCHAR )) ];
        RtlMoveMemory( dst, Source, n );

        Destination->Length += n;

        if (Destination->Length < Destination->MaximumLength) {
            dst[ n / sizeof( WCHAR ) ] = UNICODE_NULL;
            }
        }

    return( STATUS_SUCCESS );
}


NTSTATUS
RtlAppendUnicodeStringToString (
    IN PUNICODE_STRING Destination,
    IN PUNICODE_STRING Source
    )

/*++

Routine Description:

    This routine will concatinate two PSTRINGs together.  It will copy
    bytes from the source up to the MaximumLength of the destination.

Arguments:

    IN PSTRING Destination, - Supplies the destination string
    IN PSTRING Source - Supplies the source for the string copy

Return Value:

    STATUS_SUCCESS - The source string was successfully appended to the
        destination counted string.

    STATUS_BUFFER_TOO_SMALL - The destination string length was not big
        enough to allow the source string to be appended.  The Destination
        string length is not updated.

--*/

{
    USHORT n = Source->Length;
    UNALIGNED WCHAR *dst;

    if (n) {
        if ((n + Destination->Length) > Destination->MaximumLength) {
            return( STATUS_BUFFER_TOO_SMALL );
            }

        dst = &Destination->Buffer[ (Destination->Length / sizeof( WCHAR )) ];
        RtlMoveMemory( dst, Source->Buffer, n );

        Destination->Length += n;

        if (Destination->Length < Destination->MaximumLength) {
            dst[ n / sizeof( WCHAR ) ] = UNICODE_NULL;
            }
        }

    return( STATUS_SUCCESS );
}


BOOLEAN
RtlCreateUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
    )
{
    ULONG cb;

    RTL_PAGED_CODE();

    cb = (wcslen( SourceString ) + 1) * sizeof( WCHAR );
    DestinationString->Buffer = RtlAllocateStringRoutine( cb );
    if (DestinationString->Buffer) {
        RtlMoveMemory( DestinationString->Buffer, SourceString, cb );
        DestinationString->MaximumLength = (USHORT)cb;
        DestinationString->Length = (USHORT)(cb - sizeof( UNICODE_NULL ));
        return( TRUE );
        }
    else {
        return( FALSE );
        }
}
