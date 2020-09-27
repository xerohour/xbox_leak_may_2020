/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fsrtl.c

Abstract:

    This module implements shared file system driver routines.

--*/

#include "iop.h"

BOOLEAN
IopCopyTemplateFileName(
    IN POBJECT_STRING TargetTemplateFileName,
    OUT POBJECT_STRING SourceTemplateFileName
    )
/*++

Routine Description:

    This routine copies the template file name from the source to the
    destination and validates that the template string is valid (for example,
    an asterisk must be followed by the end of the string or a period).

Arguments:

    TargetTemplateFileName - Specifies the target template file name.

    SourceTemplateFileName - Specifies the source template file name.

Return Value:

    Returns TRUE if the source template file name is valid, else FALSE.

--*/
{
    BOOLEAN SeenAsterisk;
    POSTR SourceTemplateFileNameCurrent;
    POSTR SourceTemplateFileNameEnd;
    POSTR TargetTemplateFileNameCurrent;
    OCHAR Char;

    SeenAsterisk = FALSE;

    SourceTemplateFileNameCurrent = SourceTemplateFileName->Buffer;
    SourceTemplateFileNameEnd = (POSTR)((PUCHAR)SourceTemplateFileNameCurrent +
        SourceTemplateFileName->Length);
    TargetTemplateFileNameCurrent = TargetTemplateFileName->Buffer;

    while (SourceTemplateFileNameCurrent < SourceTemplateFileNameEnd) {

        Char = *SourceTemplateFileNameCurrent++;

        //
        // Force an asterisk to be followed by a period.
        //

        if (SeenAsterisk) {

            if (Char != OTEXT('.')) {
                return FALSE;
            }

            SeenAsterisk = FALSE;
        }

        if (Char == OTEXT('*')) {
            SeenAsterisk = TRUE;
        } else if (Char >= OTEXT('a') && Char <= OTEXT('z')) {
            Char = (OCHAR)(Char - (OTEXT('a') - OTEXT('A')));
        }

        *TargetTemplateFileNameCurrent++ = Char;
    }

    return TRUE;
}

NTSTATUS
IoCreateDirectoryEnumContext(
    IN POBJECT_STRING TemplateFileName,
    OUT PDIRECTORY_ENUM_CONTEXT *ReturnedDirectoryEnumContext
    )
/*++

Routine Description:

    This routine allocates and prepares a directory enumeration context given
    the supplied template file name.  The template file name is also validated.

Arguments:

    TemplateFileName - Specifies the template file name for the enumeration.

    ReturnedDirectoryEnumContext - Specifies the location to receive the
        allocated directory enumeration context.

Return Value:

    Status of operation.

--*/
{
    ULONG TemplateFileNameLength;
    PDIRECTORY_ENUM_CONTEXT DirectoryEnumContext;

    if ((TemplateFileName == NULL) || (TemplateFileName->Length == 0) ||
        (TemplateFileName->Length == sizeof(OCHAR)) &&
            (TemplateFileName->Buffer[0] == OTEXT('*'))) {

        //
        // No template name was specified or else "*" was specified, so we'll
        // match all file names.
        //

        TemplateFileNameLength = 0;

    } else {
        TemplateFileNameLength = TemplateFileName->Length;
    }

    //
    // Allocate and zero initialize the directory enumeration context.
    //

    DirectoryEnumContext = (PDIRECTORY_ENUM_CONTEXT)ExAllocatePoolWithTag(
        sizeof(DIRECTORY_ENUM_CONTEXT) + TemplateFileNameLength, 'eDoI');

    if (DirectoryEnumContext == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(DirectoryEnumContext, sizeof(DIRECTORY_ENUM_CONTEXT));

    if (TemplateFileNameLength != 0) {

        //
        // A template name was specified.  Convert the name to uppercase and
        // store it in the directory enumeration context.
        //

        DirectoryEnumContext->TemplateFileName.Length =
            (USHORT)TemplateFileNameLength;
        DirectoryEnumContext->TemplateFileName.MaximumLength =
            (USHORT)TemplateFileNameLength;
        DirectoryEnumContext->TemplateFileName.Buffer =
            (POSTR)((PUCHAR)DirectoryEnumContext +
            sizeof(DIRECTORY_ENUM_CONTEXT));

        if (!IopCopyTemplateFileName(&DirectoryEnumContext->TemplateFileName,
            TemplateFileName)) {
            ExFreePool(DirectoryEnumContext);
            return STATUS_INVALID_PARAMETER;
        }
    }

    *ReturnedDirectoryEnumContext = DirectoryEnumContext;

    return STATUS_SUCCESS;
}

BOOLEAN
IoIsNameInExpression(
    IN POBJECT_STRING TemplateFileName,
    IN POBJECT_STRING FileName
    )
/*++

Routine Description:

    This routine checks if the supplied file name matches the supplied search
    specification template.

Arguments:

    TemplateFileName - Specifies the search specification.

    FileName - Specifies the file name to match against the template.

Return Value:

    Returns TRUE if the file name matches the template file name, else FALSE.

--*/
{
    POSTR TemplateFileNameCurrent;
    POSTR TemplateFileNameEnd;
    POSTR FileNameCurrent;
    POSTR FileNameEnd;
    OCHAR Char;
    OCHAR TemplateChar;

    ASSERT(TemplateFileName->Length > 0);

    //
    // If we somehow get a name with no characters in it, reject it.
    //

    if (FileName->Length == 0) {
        return FALSE;
    }

    TemplateFileNameCurrent = TemplateFileName->Buffer;
    TemplateFileNameEnd = (POSTR)((PUCHAR)TemplateFileNameCurrent +
        TemplateFileName->Length);
    FileNameCurrent = FileName->Buffer;
    FileNameEnd = (POSTR)((PUCHAR)FileNameCurrent + FileName->Length);

    while (TemplateFileNameCurrent < TemplateFileNameEnd) {

        TemplateChar = *TemplateFileNameCurrent++;

        if (TemplateChar == OTEXT('*')) {

            //
            // If the asterisk is the last character in the string, then consume
            // the rest of the file name.
            //

            if (TemplateFileNameCurrent == TemplateFileNameEnd) {
                return TRUE;
            }

            //
            // Only allow an asterisk to be followed by the start of an
            // extension.  This has already been verified by
            // IoCopyTemplateFileName.
            //

            ASSERT(*TemplateFileNameCurrent == OTEXT('.'));
            TemplateFileNameCurrent++;

            //
            // Consume characters from the file name until we find the start of
            // an extension or the end of the string.
            //

            do {

                if (FileNameCurrent == FileNameEnd) {
                    break;
                }

                Char = *FileNameCurrent++;

            } while (Char != OTEXT('.'));

            continue;
        }

        //
        // If we're out of characters to match in the file name, then the
        // template is longer than the name, so fail the match.
        //

        if (FileNameCurrent == FileNameEnd) {
            return FALSE;
        }

        Char = *FileNameCurrent++;

        if (TemplateChar != OTEXT('?')) {

            //
            // Upcase the character if necessary.
            //

            if (Char >= OTEXT('a') && Char <= OTEXT('z')) {
                Char = (OCHAR)(Char - (OTEXT('a') - OTEXT('A')));
            }

            //
            // If the character doesn't match the template, then we're done.
            //

            if (Char != TemplateChar) {
                return FALSE;
            }
        }
    }

    //
    // We ran out of characters in the template.  If we consumed the entire
    // file name, then this is a match.
    //

    return (BOOLEAN)(FileNameCurrent == FileNameEnd);
}
