/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    obmanage.c

Abstract:

    This module implements general routines related to the object manager.

--*/

#include "obp.h"

//
// Pointer to the directory object for the root of the object space.
//
POBJECT_DIRECTORY ObpRootDirectoryObject;

//
// Pointer to the directory object for the DOS devices space.
//
POBJECT_DIRECTORY ObpDosDevicesDirectoryObject;

//
// Pointer to the directory object for the devices space.
//
POBJECT_DIRECTORY ObpIoDevicesDirectoryObject;

//
// Pointer to the directory object for the Win32 named objects space.
//
POBJECT_DIRECTORY ObpWin32NamedObjectsDirectoryObject;

//
// Event used by object types that cannot be signaled.
//
INITIALIZED_KEVENT(ObpDefaultObject, SynchronizationEvent, TRUE);

//
// Name of the DOS devices directory object.
//
INITIALIZED_OBJECT_STRING_RDATA(ObpDosDevicesString, "\\??");
INITIALIZED_OBJECT_STRING_RDATA(ObpIoDevicesString, "\\Device");
INITIALIZED_OBJECT_STRING_RDATA(ObpWin32NamedObjectsString, "\\Win32NamedObjects");

//
// Local support.
//

BOOLEAN                      
ObpCreatePermanentDirectoryObject(
    IN POBJECT_STRING DirectoryName OPTIONAL,
    OUT POBJECT_DIRECTORY *DirectoryObject
    )
/*++

Routine Description:

    This routine creates a permament directory object with the supplied name.

Arguments:

    DirectoryName - Supplies the name of the directory object.

    DirectoryObject - Supplies the buffer to receive the pointer to the
        directory object.

Return Value:

    Returns TRUE if the directory object was created, else FALSE.

--*/
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE Handle;

    InitializeObjectAttributes(&ObjectAttributes, DirectoryName, OBJ_PERMANENT,
        NULL, NULL);

    status = NtCreateDirectoryObject(&Handle, &ObjectAttributes);

    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    status = ObReferenceObjectByHandle(Handle, &ObDirectoryObjectType,
        (PVOID *)DirectoryObject);

    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    NtClose(Handle);

    return TRUE;
}

BOOLEAN
ObInitSystem(
    VOID
    )
/*++

Routine Description:

    This routine initializes the object manager.

Arguments:

    None.

Return Value:

    Returns TRUE if the object manager was successfully initialized, else FALSE.

--*/
{
    //
    // Initialize the object handle table.
    //

    ObpObjectHandleTable.HandleCount = 0;
    ObpObjectHandleTable.FirstFreeTableEntry = -1;
    ObpObjectHandleTable.NextHandleNeedingPool = 0;
    ObpObjectHandleTable.RootTable = NULL;

    //
    // Null out the DOS drive letters array.
    //

    RtlZeroMemory(ObpDosDevicesDriveLetterMap, sizeof(ObpDosDevicesDriveLetterMap));

    //
    // Create the root directory object.
    //

    if (!ObpCreatePermanentDirectoryObject(NULL, &ObpRootDirectoryObject)) {
        return FALSE;
    }

    //
    // Create the DOS devices directory object.
    //

    if (!ObpCreatePermanentDirectoryObject(&ObpDosDevicesString,
        &ObpDosDevicesDirectoryObject)) {
        return FALSE;
    }

    //
    // Create the I/O devices directory object.
    //

    if (!ObpCreatePermanentDirectoryObject(&ObpIoDevicesString,
        &ObpIoDevicesDirectoryObject)) {
        return FALSE;
    }

    //
    // Create the Win32 named objects directory object.
    //

    if (!ObpCreatePermanentDirectoryObject(&ObpWin32NamedObjectsString,
        &ObpWin32NamedObjectsDirectoryObject)) {
        return FALSE;
    }

    return TRUE;
}

VOID
ObDissectName(
    IN OBJECT_STRING Path,
    OUT POBJECT_STRING FirstName,
    OUT POBJECT_STRING RemainingName
    )

/*++

Routine Description:

    This routine cracks a path.  It picks off the first element in the
    given path name and provides both it and the remaining part.  A path
    is a set of file names separated by backslashes.  If a name begins
    with a backslash, the FirstName is the string immediately following
    the backslash.  Here are some examples:

        Path           FirstName    RemainingName
        ----           ---------    -------------
        empty          empty        empty

        \              empty        empty

        A              A            empty

        \A             A            empty

        A\B\C\D\E      A            B\C\D\E

        *A?            *A?          empty


    Note that both output strings use the same string buffer memory of the
    input string, and are not necessarily null terminated.

    Also, this routine makes no judgement as to the legality of each
    file name componant.  This must be done separatly when each file name
    is extracted.

Arguments:

    Path - The full path name to crack.

    FirstName - The first name in the path.  Don't allocate a buffer for
        this string.

    RemainingName - The rest of the path.  Don't allocate a buffer for this
        string.

Return Value:

    None.

--*/

{
    ULONG i = 0;
    ULONG PathLength;
    ULONG FirstNameStart;

    //
    //  Make both output strings empty for now
    //

    FirstName->Length = 0;
    FirstName->MaximumLength = 0;
    FirstName->Buffer = NULL;

    RemainingName->Length = 0;
    RemainingName->MaximumLength = 0;
    RemainingName->Buffer = NULL;

    PathLength = Path.Length / sizeof(OCHAR);

    //
    //  Check for an empty input string
    //

    if (PathLength == 0) {

        return;
    }

    //
    //  Skip over a starting backslash, and make sure there is more.
    //

    if ( Path.Buffer[0] == OTEXT('\\') ) {

        i = 1;
    }

    //
    //  Now run down the input string until we hit a backslash or the end
    //  of the string, remembering where we started;
    //

    for ( FirstNameStart = i;
          (i < PathLength) && (Path.Buffer[i] != OTEXT('\\'));
          i += 1 ) {

        NOTHING;
    }

    //
    //  At this point all characters up to (but not including) i are
    //  in the first part.   So setup the first name
    //

    FirstName->Length = (USHORT)((i - FirstNameStart) * sizeof(OCHAR));
    FirstName->MaximumLength = FirstName->Length;
    FirstName->Buffer = &Path.Buffer[FirstNameStart];

    //
    //  Now the remaining part needs a string only if the first part didn't
    //  exhaust the entire input string.  We know that if anything is left
    //  that is must start with a backslash.  Note that if there is only
    //  a trailing backslash, the length will get correctly set to zero.
    //

    if (i < PathLength) {

        RemainingName->Length = (USHORT)((PathLength - (i + 1)) * sizeof(OCHAR));
        RemainingName->MaximumLength = RemainingName->Length;
        RemainingName->Buffer = &Path.Buffer[i + 1];
    }

    //
    //  And return to our caller
    //

    return;
}
