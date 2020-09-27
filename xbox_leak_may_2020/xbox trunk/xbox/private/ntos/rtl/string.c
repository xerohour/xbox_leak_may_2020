/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    string.c

Abstract:

    This module defines functions for manipulating counted strings (STRING).
    A counted string is a data structure containing three fields.  The Buffer
    field is a pointer to the string itself.  The MaximumLength field contains
    the maximum number of bytes that can be stored in the memory pointed to
    by the Buffer field.  The Length field contains the current length, in
    bytes, of the string pointed to by the Buffer field.  Users of counted
    strings should not make any assumptions about the existence of a null
    byte at the end of the string, unless the null byte is explicitly
    included in the Length of the string.

Author:

    Steve Wood (stevewo) 31-Mar-1989

Revision History:

    22-Sep-1993    JulieB    Fixed TO_UPPER macro for chars above 0x7f.


--*/

#include "ntrtlp.h"

VOID
RtlCopyString(
    OUT PSTRING DestinationString,
    IN PSTRING SourceString OPTIONAL
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
    PSZ src, dst;
    ULONG n;

    if (ARGUMENT_PRESENT( SourceString )) {
        dst = DestinationString->Buffer;
        src = SourceString->Buffer;
        n = SourceString->Length;
        if ((USHORT)n > DestinationString->MaximumLength) {
            n = DestinationString->MaximumLength;
            }
        DestinationString->Length = (USHORT)n;
        while (n) {
            *dst++ = *src++;
            n--;
            }
        }
    else {
        DestinationString->Length = 0;
        }
}

CHAR
RtlUpperChar (
    register IN CHAR Character
    )

/*++

Routine Description:

    This routine returns a character uppercased
.
Arguments:

    IN CHAR Character - Supplies the character to upper case

Return Value:

    CHAR - Uppercased version of the character
--*/

{
    WCHAR wCh;

    wCh = NLS_UPCASE((WCHAR)((UCHAR)Character));
    return (wCh < 256) ? wCh : '?';
}

CHAR
RtlLowerChar (
    register IN CHAR Character
    )

/*++

Routine Description:

    This routine returns a character lowercased
.
Arguments:

    IN CHAR Character - Supplies the character to lower case

Return Value:

    CHAR - Lowercased version of the character
--*/

{
    WCHAR wCh;

    wCh = NLS_DOWNCASE((WCHAR)((UCHAR)Character));
    return (wCh < 256) ? wCh : '?';
}

LONG
RtlCompareString(
    IN PSTRING String1,
    IN PSTRING String2,
    IN BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlCompareString function compares two counted strings.  The return
    value indicates if the strings are equal or String1 is less than String2
    or String1 is greater than String2.

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

    PUCHAR s1, s2, Limit;
    LONG n1, n2;
    UCHAR c1, c2;

    s1 = String1->Buffer;
    s2 = String2->Buffer;
    n1 = String1->Length;
    n2 = String2->Length;
    Limit = s1 + (n1 <= n2 ? n1 : n2);
    if (CaseInSensitive) {
        while (s1 < Limit) {
            c1 = *s1++;
            c2 = *s2++;
            if (c1 !=c2) {
                c1 = RtlUpperChar(c1);
                c2 = RtlUpperChar(c2);
                if (c1 != c2) {
                    return (LONG)c1 - (LONG)c2;
                }
            }
        }

    } else {
        while (s1 < Limit) {
            c1 = *s1++;
            c2 = *s2++;
            if (c1 != c2) {
                return (LONG)c1 - (LONG)c2;
            }
        }
    }

    return n1 - n2;
}

BOOLEAN
RtlEqualString(
    IN PSTRING String1,
    IN PSTRING String2,
    IN BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlEqualString function compares two counted strings for equality.

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

    PUCHAR s1, s2, Limit;
    LONG n1, n2;
    UCHAR c1, c2;

    n1 = String1->Length;
    n2 = String2->Length;
    if (n1 == n2) {
        s1 = String1->Buffer;
        s2 = String2->Buffer;
        Limit = s1 + n1;
        if (CaseInSensitive) {
            while (s1 < Limit) {
                c1 = *s1++;
                c2 = *s2++;
                if (c1 != c2) {
                    c1 = RtlUpperChar(c1);
                    c2 = RtlUpperChar(c2);
                    if (c1 != c2) {
                        return FALSE;
                    }
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
RtlPrefixString(
    IN PSTRING String1,
    IN PSTRING String2,
    IN BOOLEAN CaseInSensitive
    )

/*++

Routine Description:

    The RtlPrefixString function determines if the String1 counted string
    parameter is a prefix of the String2 counted string parameter.

    The CaseInSensitive parameter specifies if case is to be ignored when
    doing the comparison.

Arguments:

    String1 - Pointer to the first string.

    String2 - Pointer to the second string.

    CaseInsensitive - TRUE if case should be ignored when doing the
        comparison.

Return Value:

    Boolean value that is TRUE if String1 equals a prefix of String2 and
    FALSE otherwise.

--*/

{
    PSZ s1, s2;
    USHORT n;
    UCHAR c1, c2;

    s1 = String1->Buffer;
    s2 = String2->Buffer;
    n = String1->Length;
    if (String2->Length < n) {
        return( FALSE );
        }

    if (CaseInSensitive) {
        while (n) {
            c1 = *s1++;
            c2 = *s2++;

            if (c1 != c2 && RtlUpperChar(c1) != RtlUpperChar(c2)) {
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
RtlUpperString(
    IN PSTRING DestinationString,
    IN PSTRING SourceString
    )

/*++

Routine Description:

    The RtlUpperString function copies the SourceString to the
    DestinationString, converting it to upper case.  The MaximumLength
    and Buffer fields of DestinationString are not modified by this
    function.

    The number of bytes copied from the SourceString is either the
    Length of SourceString or the MaximumLength of DestinationString,
    whichever is smaller.

Arguments:

    DestinationString - Pointer to the destination string.

    SourceString - Pointer to the source string.

Return Value:

    None.

--*/

{
    PSZ src, dst;
    ULONG n;

    dst = DestinationString->Buffer;
    src = SourceString->Buffer;
    n = SourceString->Length;
    if ((USHORT)n > DestinationString->MaximumLength) {
        n = DestinationString->MaximumLength;
        }
    DestinationString->Length = (USHORT)n;
    while (n) {
        *dst++ = RtlUpperChar(*src++);
        n--;
        }
}

NTSTATUS
RtlAppendStringToString (
    IN PSTRING Destination,
    IN PSTRING Source
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

    if (n) {
        if ((n + Destination->Length) > Destination->MaximumLength) {
            return( STATUS_BUFFER_TOO_SMALL );
            }

        RtlMoveMemory( &Destination->Buffer[ Destination->Length ],
                       Source->Buffer,
                       n
                     );
        Destination->Length += n;
        }

    return( STATUS_SUCCESS );
}
