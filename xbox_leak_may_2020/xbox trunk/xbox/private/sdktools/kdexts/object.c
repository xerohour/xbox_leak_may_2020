/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    object.c

Abstract:

    WinDbg Extension Api

Author:

    Ramon J San Andres (ramonsa) 5-Nov-1993

Environment:

    User Mode.

Revision History:

--*/


#include "precomp.h"
#pragma hdrstop

typedef PVOID (*ENUM_LIST_ROUTINE)(
                                  IN PLIST_ENTRY ListEntry,
                                  IN PVOID Parameter
                                  );


ULONG EXPRLastDump = 0;

static POBJECT_DIRECTORY ObpRootDirectoryObject = NULL;
static OCHAR             ObjectNameBuffer[ MAX_PATH ];

BOOLEAN
DumpObjectsForType(
                  IN PVOID            pObjectHeader,
                  IN POBJECT_HEADER   ObjectHeader,
                  IN PVOID            Parameter
                  );

PVOID
WalkRemoteList(
              IN PLIST_ENTRY       Head,
              IN ENUM_LIST_ROUTINE EnumRoutine,
              IN PVOID             Parameter
              );

POSTR
GetObjectName(
             PVOID Object
             );


ULONG HighestUserAddress = (ULONG)MM_HIGHEST_USER_ADDRESS;

DECLARE_API( object )

/*++

Routine Description:

    Dump an object manager object.

Arguments:

    args - [TypeName]

Return Value:

    None

--*/
{
    ULONG   ObjectToDump;
    ULONG   NumberOfObjects;
    char ExprBuf[256] ;

    if (!FetchObjectManagerVariables(FALSE)) {
        return;
    }

    ObjectToDump    = EXPRLastDump;

    //
    // If the argument looks like a path, try to chase it.
    //

    if (args[0] == '\\') {

        PVOID object;

        object = FindObjectByName((PUCHAR) args, NULL);

        if (object != NULL) {
            DumpObject("", object, NULL, 0xffffffff);
        } else {
            dprintf("Object %s not found\n", args);
        }
        return;
    }

    //
    // If the argument is -r or -R, reload the cached symbol information
    //

    if (!strcmp(args, "-r")) {
        FetchObjectManagerVariables(TRUE);
        return;
    }

    sscanf(args, "%s", ExprBuf);

    if (ExprBuf[0]) {
        if (IsHexNumber(ExprBuf) || ExprBuf[0] == '-') {
            sscanf(ExprBuf, "%lx", &ObjectToDump) ;
        } else {
            ObjectToDump = GetExpression(ExprBuf);
            if (ObjectToDump==0) {
                dprintf("An error occured trying to evaluate the expression\n") ;
                return;
            }
        }
        DumpObject("", (PVOID)ObjectToDump, NULL, 0xFFFFFFFF);
        return;
    }

    dprintf( "*** invalid syntax.\n" );
    return;
}



DECLARE_API( obja )

/*++

Routine Description:

    Dump an object's attributes

Arguments:

    args -

Return Value:

    None

--*/
{
    OBJECT_STRING ObjectString;
    DWORD dwAddrObja;
    OBJECT_ATTRIBUTES Obja;
    DWORD dwAddrString;
    CHAR Symbol[256];
    LPSTR StringData;
    DWORD Displacement;
    BOOL b;

    //
    // Evaluate the argument string to get the address of
    // the Obja to dump.
    //

    dwAddrObja = GetExpression(args);
    if ( !dwAddrObja ) {
        return;
    }


    //
    // Get the symbolic name of the Obja
    //

    GetSymbol((LPVOID)dwAddrObja,Symbol,&Displacement);

    //
    // Read the obja from the debuggees address space into our
    // own.

    b = ReadMemory(
                  (DWORD)dwAddrObja,
                  &Obja,
                  sizeof(Obja),
                  NULL
                  );
    if ( !b ) {
        return;
    }
    StringData = NULL;
    if ( Obja.ObjectName ) {
        dwAddrString = (DWORD)Obja.ObjectName;
        b = ReadMemory(
                      (DWORD)dwAddrString,
                      &ObjectString,
                      sizeof(ObjectString),
                      NULL
                      );
        if ( !b ) {
            return;
        }

        StringData = (LPSTR)LocalAlloc(
                                      LMEM_ZEROINIT,
                                      ObjectString.Length+sizeof(OBJECT_NULL)
                                      );

        b = ReadMemory(
                      (DWORD)ObjectString.Buffer,
                      StringData,
                      ObjectString.Length,
                      NULL
                      );
        if ( !b ) {
            LocalFree(StringData);
            return;
        }
        ObjectString.Buffer = (POSTR)StringData;
        ObjectString.MaximumLength = ObjectString.Length+(USHORT)sizeof(OBJECT_NULL);
    }

    //
    // We got the object name in ObjectString. StringData is NULL if no name.
    //

    dprintf(
           "Obja %s+%lx at %lx:\n",
           Symbol,
           Displacement,
           dwAddrObja
           );
    if ( StringData ) {
        dprintf("\t%s is " POSTR_FORMATCODE "\n",
                Obja.RootDirectory ? "Relative Name" : "Full Name",
                ObjectString.Buffer
               );
        LocalFree(StringData);
    }
    if ( Obja.Attributes ) {
        if ( Obja.Attributes & OBJ_INHERIT ) {
            dprintf("\tOBJ_INHERIT\n");
        }
        if ( Obja.Attributes & OBJ_PERMANENT ) {
            dprintf("\tOBJ_PERMANENT\n");
        }
        if ( Obja.Attributes & OBJ_EXCLUSIVE ) {
            dprintf("\tOBJ_EXCLUSIVE\n");
        }
        if ( Obja.Attributes & OBJ_CASE_INSENSITIVE ) {
            dprintf("\tOBJ_CASE_INSENSITIVE\n");
        }
        if ( Obja.Attributes & OBJ_OPENIF ) {
            dprintf("\tOBJ_OPENIF\n");
        }
    }
}

BOOLEAN
DumpObjectsForType(
                  IN PVOID            pObjectHeader,
                  IN POBJECT_HEADER   ObjectHeader,
                  IN PVOID            Parameter
                  )
{
    PVOID Object;
    PULONG NumberOfObjects = (PULONG)Parameter;

    *NumberOfObjects += 1;
    Object = (PVOID)&(((POBJECT_HEADER)pObjectHeader)->Body);
    DumpObject( "", Object, NULL, 0xFFFFFFFF );
    return TRUE;
}

BOOLEAN
FetchObjectManagerVariables(
                           BOOLEAN ForceReload
                           )
{
    ULONG        Result;
    DWORD        Addr;
    static BOOL  HaveObpVariables = FALSE;

    if (HaveObpVariables && !ForceReload) {
        return TRUE;
    }

    Addr = GetNtDebuggerData( ObpRootDirectoryObject );
    if ( !Addr ||
         !ReadMemory( Addr,
                      &ObpRootDirectoryObject,
                      sizeof(ObpRootDirectoryObject),
                      &Result) ) {
        dprintf("%08lx: Unable to get value of ObpRootDirectoryObject\n",Addr );
        return FALSE;
    }

    HaveObpVariables = TRUE;
    return TRUE;
}

PVOID
WalkRemoteList(
              IN PLIST_ENTRY       Head,
              IN ENUM_LIST_ROUTINE EnumRoutine,
              IN PVOID             Parameter
              )
{
    ULONG       Result;
    PVOID       Element;
    LIST_ENTRY  ListEntry;
    PLIST_ENTRY Next;

    if ( !ReadMemory( (DWORD)Head,
                      &ListEntry,
                      sizeof( ListEntry ),
                      &Result) ) {
        dprintf( "%08lx: Unable to read list\n", Head );
        return NULL;
    }

    Next = ListEntry.Flink;
    while (Next != Head) {

        if ( !ReadMemory( (DWORD)Next,
                          &ListEntry,
                          sizeof( ListEntry ),
                          &Result) ) {
            dprintf( "%08lx: Unable to read list\n", Next );
            return NULL;
        }

        Element = (EnumRoutine)( Next, Parameter );
        if (Element != NULL) {
            return Element;
        }

        if ( CheckControlC() ) {
            return NULL;
        }

        Next = ListEntry.Flink;
    }

    return NULL;
}


typedef struct _OBJECT_INFO {
    POBJECT_HEADER pObjectHeader;
    OBJECT_HEADER ObjectHeader;
    OBJECT_TYPE ObjectType;
    OBJECT_HEADER_NAME_INFO NameInfo;
    union {
        ULONG PoolTag;
        CHAR TypeName[4];
    };
    OCHAR ObjectName[ 256 ];
    OCHAR FileSystemName[ 32 ];
    CHAR Message[ 256 ];
} OBJECT_INFO, *POBJECT_INFO;



BOOLEAN
GetObjectInfo(
             PVOID Object,
             IN POBJECT_HEADER OptObjectHeader OPTIONAL,
             POBJECT_INFO ObjectInfo
             )
{
    ULONG           Result;
    POBJECT_HEADER_NAME_INFO pNameInfo;
    BOOLEAN         PagedOut;
    OBJECT_STRING   ObjectName;
    POSTR           FileSystemName;
    FILE_OBJECT     FileObject;

    PagedOut = FALSE;
    memset( ObjectInfo, 0, sizeof( *ObjectInfo ) );
    ObjectInfo->pObjectHeader = OBJECT_TO_OBJECT_HEADER( Object );
    if (!ReadMemory( (DWORD)ObjectInfo->pObjectHeader,
                     &ObjectInfo->ObjectHeader,
                     sizeof( ObjectInfo->ObjectHeader ),
                     &Result
                   )
       ) {
        if ((ULONG)Object >= HighestUserAddress && (ULONG)Object < 0xF0000000) {
            sprintf( ObjectInfo->Message, "%08lx: object is paged out.", Object );
            if (!ARGUMENT_PRESENT( OptObjectHeader )) {
                return FALSE;
            }
            ObjectInfo->ObjectHeader = *OptObjectHeader;
            PagedOut = TRUE;
        } else {
            sprintf( ObjectInfo->Message, "%08lx: not a valid object (ObjectHeader invalid)", Object );
            return FALSE;
        }
    }

    if (!ReadMemory( (DWORD)ObjectInfo->ObjectHeader.Type,
                     &ObjectInfo->ObjectType,
                     sizeof( ObjectInfo->ObjectType ),
                     &Result
                   )
       ) {
        sprintf( ObjectInfo->Message, "%08lx: Not a valid object (ObjectType invalid)", Object );
        return FALSE;
    }

    ObjectInfo->PoolTag = ObjectInfo->ObjectType.PoolTag;

    if (PagedOut) {
        return TRUE;
    }

    if (ObjectInfo->PoolTag == 'eliF') {
        if (!ReadMemory( (DWORD)Object,
                         &FileObject,
                         sizeof( FileObject ),
                         &Result
                       )
           ) {
            sprintf( ObjectInfo->Message, "%08lx: unable to read file object for name\n", Object );
        } else {
            ObjectName.Length = 0;
            FileSystemName = GetObjectName( FileObject.DeviceObject );
            if (FileSystemName != NULL) {
                ocscpy( ObjectInfo->FileSystemName, FileSystemName );
            }
        }
    } else {
        pNameInfo = KD_OBJECT_HEADER_TO_NAME_INFO( ObjectInfo->pObjectHeader, &ObjectInfo->ObjectHeader );
        if (pNameInfo == NULL) {
            return TRUE;
        }

        if (!ReadMemory( (DWORD)pNameInfo,
                         &ObjectInfo->NameInfo,
                         sizeof( ObjectInfo->NameInfo ),
                         &Result
                       )
           ) {
            sprintf( ObjectInfo->Message, "*** unable to read object name info at %08x\n", pNameInfo );
            return FALSE;
        }

        ObjectName = ObjectInfo->NameInfo.Name;
    }

    if (ObjectName.Length >= sizeof( ObjectInfo->ObjectName )) {
        ObjectName.Length = sizeof( ObjectInfo->ObjectName ) - sizeof( UNICODE_NULL );
    }

    if (ObjectName.Length != 0) {
        if (!ReadMemory( (DWORD)ObjectName.Buffer,
                         ObjectInfo->ObjectName,
                         ObjectName.Length,
                         &Result
                       )
           ) {
            ocscpy( ObjectInfo->ObjectName, OTEXT("(*** Name not accessable ***)") );
        } else {
            ObjectInfo->ObjectName[ ObjectName.Length / sizeof( OCHAR ) ] = OBJECT_NULL;
        }
    }

    return TRUE;
}


PVOID
FindObjectByName(
                IN PUCHAR Path,
                IN PVOID RootObject
                )
{
    ULONG Result, i, j;
    POBJECT_DIRECTORY pDirectoryObject;
    OBJECT_DIRECTORY DirectoryObject;
    POBJECT_HEADER_NAME_INFO pDirectoryEntry;
    OBJECT_HEADER_NAME_INFO DirectoryEntry;
    OBJECT_INFO ObjectInfo;

    BOOLEAN foundMatch = FALSE;

    PUCHAR nextPath;

    if (RootObject == NULL) {

        if (!FetchObjectManagerVariables(FALSE)) {
            return NULL;
        }

        RootObject = (PVOID) ObpRootDirectoryObject;
    }

    pDirectoryObject = (POBJECT_DIRECTORY)RootObject;

    //
    // See if we've reached the end of the path, at which point we know
    // that RootObject is the object to be dumped.
    //

    if (*Path == '\0') {
        return RootObject;
    }

    //
    // Scan the path looking for another delimiter or for the end of the
    // string.

    nextPath = Path;

    while ((*nextPath != '\0') &&
           (*nextPath != '\\')) {

        nextPath++;
    }

    //
    // if we found a delimeter remove it from the next path and use it to
    // truncate the current path.
    //

    if (*nextPath == '\\') {
        *nextPath = '\0';
        nextPath++;
    }

    //
    // Make sure there's a path node here.  If not, recursively call ourself
    // with the remainder of the path.
    //

    if (*Path == '\0') {
        return FindObjectByName(nextPath, RootObject);
    }

    if (!ReadMemory( (DWORD)pDirectoryObject,
                     &DirectoryObject,
                     sizeof( DirectoryObject ),
                     &Result
                   )
       ) {
        dprintf( "Unable to read directory object at %x\n", RootObject );
        return FALSE;
    }

    for (i=0; i<OB_NUMBER_HASH_BUCKETS; i++) {
        if (DirectoryObject.HashBuckets[ i ] != NULL) {
            pDirectoryEntry = DirectoryObject.HashBuckets[ i ];
            while (pDirectoryEntry != NULL) {

                if (!ReadMemory( (DWORD)pDirectoryEntry,
                                 &DirectoryEntry,
                                 sizeof( DirectoryEntry ),
                                 &Result
                               )
                   ) {
                    // dprintf( "Unable to read directory entry at %x\n", pDirectoryEntry );
                    break;
                }

                if (!GetObjectInfo( OBJECT_HEADER_NAME_INFO_TO_OBJECT(pDirectoryEntry), NULL, &ObjectInfo)) {
                    // dprintf( " - %s\n", ObjectInfo.Message );
                } else {
                    foundMatch = TRUE;

                    for (j = 0;
                        (Path[j] != '\0') && (ObjectInfo.ObjectName[j] != L'\0');
                        j++) {

                        if (tolower(Path[j]) !=
                            towlower(ObjectInfo.ObjectName[j])) {
                            foundMatch = FALSE;
                            break;
                        }

                    }

                    if (foundMatch) {

                        if ((Path[j] == '\0') &&
                            (ObjectInfo.ObjectName[j] == L'\0')) {

                            return FindObjectByName(nextPath, OBJECT_HEADER_NAME_INFO_TO_OBJECT(pDirectoryEntry));
                        }

                    }

                }
                pDirectoryEntry = DirectoryEntry.ChainLink;
            }
        }
    }

    return NULL;
}

VOID
DumpDirectoryObject(
                   IN char     *Pad,
                   IN PVOID    Object
                   )
{
    ULONG Result, i;
    POBJECT_DIRECTORY pDirectoryObject = (POBJECT_DIRECTORY)Object;
    OBJECT_DIRECTORY DirectoryObject;
    POBJECT_HEADER_NAME_INFO pDirectoryEntry;
    OBJECT_HEADER_NAME_INFO DirectoryEntry;
    OBJECT_INFO ObjectInfo;

    if (!ReadMemory( (DWORD)pDirectoryObject,
                     &DirectoryObject,
                     sizeof( DirectoryObject ),
                     &Result
                   )
       ) {
        dprintf( "Unable to read directory object at %x\n", Object );
        return;
    }

    for (i=0; i<OB_NUMBER_HASH_BUCKETS; i++) {
        if (DirectoryObject.HashBuckets[ i ] != NULL) {
            dprintf( "%s    HashBucket[ %02u ]: ",
                     Pad,
                     i
                   );
            pDirectoryEntry = DirectoryObject.HashBuckets[ i ];
            while (pDirectoryEntry != NULL) {
                if (!ReadMemory( (DWORD)pDirectoryEntry,
                                 &DirectoryEntry,
                                 sizeof( DirectoryEntry ),
                                 &Result
                               )
                   ) {
                    dprintf( "Unable to read directory entry at %x\n", pDirectoryEntry );
                    break;
                }

                if (pDirectoryEntry != DirectoryObject.HashBuckets[ i ]) {
                    dprintf( "%s                      ", Pad );
                }
                dprintf( "%x", OBJECT_HEADER_NAME_INFO_TO_OBJECT(pDirectoryEntry) );

                if (!GetObjectInfo( OBJECT_HEADER_NAME_INFO_TO_OBJECT(pDirectoryEntry), NULL, &ObjectInfo)) {
                    dprintf( " - %s\n", ObjectInfo.Message );
                } else {
                    dprintf( " %c%c%c%c '" POSTR_FORMATCODE "'\n",
                            ObjectInfo.TypeName[0],
                            ObjectInfo.TypeName[1],
                            ObjectInfo.TypeName[2],
                            ObjectInfo.TypeName[3],
                            ObjectInfo.ObjectName );
                }
                pDirectoryEntry = DirectoryEntry.ChainLink;
            }
        }
    }
}

VOID
DumpSymbolicLinkObject(
                      IN char     *Pad,
                      IN PVOID    Object
                      )
{
    ULONG Result, i;
    POBJECT_SYMBOLIC_LINK pSymbolicLinkObject = (POBJECT_SYMBOLIC_LINK)Object;
    OBJECT_SYMBOLIC_LINK SymbolicLinkObject;
    POSTR s, FreeBuffer;
    OBJECT_INFO ObjectInfo;

    if (!ReadMemory( (DWORD)pSymbolicLinkObject,
                     &SymbolicLinkObject,
                     sizeof( SymbolicLinkObject ),
                     &Result
                   )
       ) {
        dprintf( "Unable to read symbolic link object at %x\n", Object );
        return;
    }

    FreeBuffer = s = HeapAlloc( GetProcessHeap(),
                                HEAP_ZERO_MEMORY,
                                SymbolicLinkObject.LinkTarget.Length + sizeof( UNICODE_NULL )
                              );
    if (s == NULL ||
        !ReadMemory( (DWORD)SymbolicLinkObject.LinkTarget.Buffer,
                     s,
                     SymbolicLinkObject.LinkTarget.Length,
                     &Result
                   )
       ) {
        s = OTEXT("*** target string unavailable ***");
    }
    dprintf( "%s    Target String is '" POSTR_FORMATCODE "'\n",
             Pad,
             s
           );

    if (!GetObjectInfo( SymbolicLinkObject.LinkTargetObject, NULL, &ObjectInfo)) {
        dprintf( "%s    Target Object is %08lx - %s\n",
                Pad,
                SymbolicLinkObject.LinkTargetObject,
                ObjectInfo.Message);
    } else {
        dprintf( "%s    Target Object is %08lx %c%c%c%c '" POSTR_FORMATCODE "'\n",
                Pad,
                SymbolicLinkObject.LinkTargetObject,
                ObjectInfo.TypeName[0],
                ObjectInfo.TypeName[1],
                ObjectInfo.TypeName[2],
                ObjectInfo.TypeName[3],
                ObjectInfo.ObjectName );
    }

    if (FreeBuffer != NULL) {
        HeapFree( GetProcessHeap(), 0, FreeBuffer );
    }

    return;
}


BOOLEAN
DumpObject(
          IN char     *Pad,
          IN PVOID    Object,
          IN POBJECT_HEADER OptObjectHeader OPTIONAL,
          IN ULONG    Flags
          )
{
    OBJECT_INFO ObjectInfo;

    if (!GetObjectInfo( Object, OptObjectHeader, &ObjectInfo)) {
        dprintf( "KD: %s\n", ObjectInfo.Message );
        return FALSE;
    }
    dprintf( "Object: %08lx  Type: (%08lx) %c%c%c%c\n",
             Object,
             ObjectInfo.ObjectHeader.Type,
             ObjectInfo.TypeName[0],
             ObjectInfo.TypeName[1],
             ObjectInfo.TypeName[2],
             ObjectInfo.TypeName[3]
           );
    dprintf( "    ObjectHeader: %08lx\n",
             ObjectInfo.pObjectHeader
           );

    if (!(Flags & 0x1)) {
        return TRUE;
    }

    dprintf( "%s    HandleCount: %u  PointerCount: %u\n",
             Pad,
             ObjectInfo.ObjectHeader.HandleCount,
             ObjectInfo.ObjectHeader.PointerCount
           );

    if (ObjectInfo.ObjectName[ 0 ] != UNICODE_NULL ||
        ObjectInfo.NameInfo.Directory != NULL
       ) {
        dprintf( "%s    Directory Object: %08lx  Name: " POSTR_FORMATCODE,
                 Pad,
                 ObjectInfo.NameInfo.Directory,
                 ObjectInfo.ObjectName
               );
        if (ObjectInfo.FileSystemName[0] != UNICODE_NULL) {
            dprintf( " {" POSTR_FORMATCODE "}\n", ObjectInfo.FileSystemName );
        } else {
            dprintf( "\n" );
        }
    }

    if ((Flags & 0x8)) {
        if (ObjectInfo.PoolTag == 'eriD') {
            DumpDirectoryObject( Pad, Object );
        } else if (ObjectInfo.PoolTag == 'bmyS') {
            DumpSymbolicLinkObject( Pad, Object );
        }
    }

    return TRUE;
}


POSTR
GetObjectName(
             PVOID Object
             )
{
    ULONG           Result;
    POBJECT_HEADER  pObjectHeader;
    OBJECT_HEADER   ObjectHeader;
    OBJECT_STRING   ObjectName;

    POBJECT_HEADER_NAME_INFO pNameInfo;
    OBJECT_HEADER_NAME_INFO NameInfo;

    pObjectHeader = OBJECT_TO_OBJECT_HEADER( Object );
    if ( !ReadMemory( (DWORD)pObjectHeader,
                      &ObjectHeader,
                      sizeof( ObjectHeader ),
                      &Result) ) {
        if ((ULONG)Object >= HighestUserAddress && (ULONG)Object < 0xF0000000) {
            soprintf( ObjectNameBuffer, OTEXT("(%08lx: object is paged out)"), Object );
            return ObjectNameBuffer;
        } else {
            soprintf( ObjectNameBuffer, OTEXT("(%08lx: invalid object header)"), Object );
            return ObjectNameBuffer;
        }
    }

    pNameInfo = KD_OBJECT_HEADER_TO_NAME_INFO( pObjectHeader, &ObjectHeader );
    if (pNameInfo == NULL) {
        return NULL;
    }

    if ( !ReadMemory( (DWORD)pNameInfo,
                      &NameInfo,
                      sizeof( NameInfo ),
                      &Result) ) {
        dprintf( "%08lx: Unable to read object name info\n", pNameInfo );
        return NULL;
    }

    ObjectName = NameInfo.Name;
    if (ObjectName.Length == 0 || ObjectName.Buffer == NULL) {
        return NULL;
    }

    if ( !ReadMemory( (DWORD)ObjectName.Buffer,
                      ObjectNameBuffer,
                      ObjectName.Length,
                      &Result) ) {
        soprintf( ObjectNameBuffer, OTEXT("(%08lx: name not accessable)"), ObjectName.Buffer );
    } else {
        ObjectNameBuffer[ ObjectName.Length / sizeof( OCHAR ) ] = OBJECT_NULL;
    }

    return ObjectNameBuffer;
}


