/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    dirlog.c

Abstract:

    This program contains routines to log all objects in Xbox directory.

Author:

    Sakphong Chanbai (schanbai) 03-May-2000

Environment:

    Xbox

Revision History:

--*/


#include "precomp.h"
#include "testutil.h"


HANDLE HeapHandle;


int __cdecl
UnicodeCompare(
    const void *arg1,
    const void *arg2
    )
{
    POBJECT_DIRECTORY_INFORMATION s1, s2;
    ULONG i, j;

    s1 = (POBJECT_DIRECTORY_INFORMATION)arg1;
    s2 = (POBJECT_DIRECTORY_INFORMATION)arg2;

    for ( i=0, j=0; (i < s1->Name.Length / sizeof(WCHAR)) && \
        (j < s2->Name.Length / sizeof(WCHAR)); \
        i++, j++) {
        if ( s1->Name.Buffer[i] < s2->Name.Buffer[j] ) {
            return -1;
        } else if ( s1->Name.Buffer[i] > s2->Name.Buffer[j] ) {
            return +1;
        }
    }

    if (i > j) {
        return -1;
    } else if (i < j) {
        return +1;
    } else {
        return 0;
    }
}


HANDLE
HandleSymbolicLink(
    IN  HANDLE LogHandle,
    IN  HANDLE DirectoryHandle,
    IN  PWSTR Link
    )
{
    NTSTATUS Status;
    HANDLE LinkHandle = NULL;
    OBJECT_ATTRIBUTES ObjA;
    UNICODE_STRING String, Result;
    CHAR AnsiBuffer[1024]; 
    WCHAR ResultBuffer[1024];

    RtlInitUnicodeString( &String, Link );

    InitializeObjectAttributes(
        &ObjA,
        &String,
        OBJ_CASE_INSENSITIVE,
        DirectoryHandle,
        NULL
        );

    xSetFunctionName( LogHandle, "NtOpenSymbolicLinkObject" );
    sprintf( AnsiBuffer, "%ws", Link );
    xStartVariation( LogHandle, AnsiBuffer );

    Status = NtOpenSymbolicLinkObject(
                &LinkHandle,
                SYMBOLIC_LINK_QUERY,
                &ObjA
                );

    if ( !NT_SUCCESS(Status) ) {
        xLog( LogHandle, XLL_FAIL, "%ws, status = 0x%x", Link, Status );
        xEndVariation( LogHandle );
        return NULL;
    } else {
        xLog( LogHandle, XLL_PASS, "pass" );
        xEndVariation( LogHandle );
    }

    xSetFunctionName( LogHandle, "NtQuerySymbolicLinkObject" );
    xStartVariation( LogHandle, AnsiBuffer );

    Result.Buffer = ResultBuffer;
    Result.Length = 0;
    Result.MaximumLength = sizeof(ResultBuffer);

    Status = NtQuerySymbolicLinkObject(
                LinkHandle,
                &Result,
                NULL
                );

    if ( !NT_SUCCESS(Status) ) {
        xLog( LogHandle, XLL_FAIL, "%ws, status = 0x%x", Link, Status );
        xEndVariation( LogHandle );
    } else {
        Result.Buffer[ Result.Length/sizeof(WCHAR) ] = 0;
        xLog( LogHandle, XLL_PASS, "%ws", Result.Buffer );
        xEndVariation( LogHandle );
    }

    return LinkHandle;
}


HANDLE
OpenObject(
    IN  HANDLE LogHandle,
    IN  HANDLE DirectoryHandle,
    IN  PWSTR  ObjectName,
    IN  POBJECT_DIRECTORY_INFORMATION ObjDirPtr
    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjA;
    UNICODE_STRING String;
    CHAR AnsiBuffer[1024];
    HANDLE ObjectHandle;

    RtlInitUnicodeString( &String, ObjectName );

    InitializeObjectAttributes(
        &ObjA,
        &String,
        OBJ_CASE_INSENSITIVE,
        DirectoryHandle,
        NULL
        );

    if ( wcsncmp(
            ObjDirPtr->TypeName.Buffer,
            L"Timer",
            ObjDirPtr->TypeName.Length/sizeof(WCHAR)) == 0 ) {
        xSetFunctionName( LogHandle, "NtOpenTimer" );
        Status = NtOpenTimer(
                    &ObjectHandle,
                    MAXIMUM_ALLOWED,
                    &ObjA
                    );
    } else if ( wcsncmp(
                    ObjDirPtr->TypeName.Buffer,
                    L"Event",
                    ObjDirPtr->TypeName.Length/sizeof(WCHAR)) == 0 ) {
        xSetFunctionName( LogHandle, "NtOpenEvent" );
        Status = NtOpenEvent(
                    &ObjectHandle,
                    MAXIMUM_ALLOWED,
                    &ObjA
                    );
    } else if ( wcsncmp(
                    ObjDirPtr->TypeName.Buffer,
                    L"Mutant",
                    ObjDirPtr->TypeName.Length/sizeof(WCHAR)) == 0 ) {
        xSetFunctionName( LogHandle, "NtOpenMutant" );
        Status = NtOpenMutant(
                    &ObjectHandle,
                    MAXIMUM_ALLOWED,
                    &ObjA
                    );
    } else if ( wcsncmp(
                    ObjDirPtr->TypeName.Buffer,
                    L"Section",
                    ObjDirPtr->TypeName.Length/sizeof(WCHAR)) == 0 ) {
        xSetFunctionName( LogHandle, "NtOpenSection" );
        Status = NtOpenSection(
                    &ObjectHandle,
                    MAXIMUM_ALLOWED,
                    &ObjA
                    );
    } else if ( wcsncmp(
                    ObjDirPtr->TypeName.Buffer,
                    L"Semaphore",
                    ObjDirPtr->TypeName.Length/sizeof(WCHAR)) == 0 ) {
        xSetFunctionName( LogHandle, "NtOpenSemaphore" );
        Status = NtOpenSemaphore(
                    &ObjectHandle,
                    MAXIMUM_ALLOWED,
                    &ObjA
                    );
    } else {
        xSetFunctionName( LogHandle, "NtQueryDirectoryObject" );
        sprintf( AnsiBuffer, "%wZ", &ObjDirPtr->Name );
        xStartVariation( LogHandle, AnsiBuffer );
        xLog( LogHandle, XLL_PASS, "Type = %wZ", &ObjDirPtr->TypeName );
        xEndVariation( LogHandle );
        return NULL;
    }

    sprintf( AnsiBuffer, "%ws", ObjectName );
    xStartVariation( LogHandle, AnsiBuffer );

    if ( NT_SUCCESS(Status) ) {
        xLog( LogHandle, XLL_PASS, "handle = %p", ObjectHandle );
    } else {
        xLog( LogHandle, XLL_FAIL, "Status = 0x%X", Status );
        ObjectHandle = NULL;
    }


    xEndVariation( LogHandle );
    return ObjectHandle;
}


VOID
QueryObject(
    IN  HANDLE LogHandle,
    IN  HANDLE ObjectHandle,
    IN  PWSTR  Path,
    IN  PWSTR  ObjectName
    )
{
#define LARGE_BUFFER_SIZE (32*1024)
    NTSTATUS Status;
    CHAR VariationBuffer[1024];
    PUCHAR LargeBuffer = _alloca(LARGE_BUFFER_SIZE);
    POBJECT_BASIC_INFORMATION pObjBasicInfo;
    POBJECT_NAME_INFORMATION pObjNameInfo;
    POBJECT_TYPE_INFORMATION pObjTypeInfo;
    POBJECT_TYPES_INFORMATION pObjTypesInfo;
    POBJECT_HANDLE_FLAG_INFORMATION pObjHandleFlagInfo;

    xSetFunctionName( LogHandle, "NtQueryObject (ObjectBasicInformation)" );
    sprintf( VariationBuffer, "%ws\\%ws", Path, ObjectName );
    xStartVariation( LogHandle, VariationBuffer );
    pObjBasicInfo = (POBJECT_BASIC_INFORMATION)LargeBuffer;
    
    Status = NtQueryObject(
                ObjectHandle,
                ObjectBasicInformation,
                pObjBasicInfo,
                sizeof(OBJECT_BASIC_INFORMATION),
                NULL
                );

    if ( NT_SUCCESS(Status) ) {
        xLog(
            LogHandle,
            XLL_PASS,
            "HandleCount=%d PointerCount=%d",
            pObjBasicInfo->HandleCount,
            pObjBasicInfo->PointerCount
            );
    } else {
        xLog(
            LogHandle,
            Status == STATUS_INFO_LENGTH_MISMATCH ? XLL_BLOCK : XLL_FAIL,
            "Status = 0x%X",
            Status
            );
    }

    xEndVariation( LogHandle );

    xSetFunctionName( LogHandle, "NtQueryObject (ObjectNameInformation)" );
    xStartVariation( LogHandle, VariationBuffer );
    pObjNameInfo = (POBJECT_NAME_INFORMATION)LargeBuffer;

    Status = NtQueryObject(
                ObjectHandle,
                ObjectNameInformation,
                pObjNameInfo,
                LARGE_BUFFER_SIZE,
                NULL
                );

    if ( NT_SUCCESS(Status) ) {
        xLog(
            LogHandle,
            XLL_PASS,
            "%ws",
            pObjNameInfo->Name.Buffer ? pObjNameInfo->Name.Buffer : L"<noname>"
            );
    } else {
        xLog(
            LogHandle,
            Status == STATUS_INFO_LENGTH_MISMATCH ? XLL_BLOCK : XLL_FAIL,
            "Status = 0x%X",
            Status
            );
    }

    xEndVariation( LogHandle );

    xSetFunctionName( LogHandle, "NtQueryObject (ObjectTypeInformation)" );
    xStartVariation( LogHandle, VariationBuffer );
    pObjTypeInfo = (POBJECT_TYPE_INFORMATION)LargeBuffer;

    Status = NtQueryObject(
                ObjectHandle,
                ObjectTypeInformation,
                pObjTypeInfo,
                LARGE_BUFFER_SIZE,
                NULL
                );

    if ( NT_SUCCESS(Status) ) {
        xLog(
            LogHandle,
            XLL_PASS,
            "%ws",
            pObjTypeInfo->TypeName.Buffer
            );
    } else {
        xLog(
            LogHandle,
            Status == STATUS_INFO_LENGTH_MISMATCH ? XLL_BLOCK : XLL_FAIL,
            "Status = 0x%X",
            Status
            );
    }
    xEndVariation( LogHandle );

    xSetFunctionName( LogHandle, "NtQueryObject (ObjectTypesInformation)" );
    xStartVariation( LogHandle, VariationBuffer );
    pObjTypesInfo =  (POBJECT_TYPES_INFORMATION)LargeBuffer;

    Status = NtQueryObject(
                ObjectHandle,
                ObjectTypesInformation,
                pObjTypesInfo,
                LARGE_BUFFER_SIZE,
                NULL
                );

    if ( NT_SUCCESS(Status) ) {
        xLog(
            LogHandle,
            XLL_PASS,
            "number of types = %d",
            pObjTypesInfo->NumberOfTypes
            );
    } else {
        xLog(
            LogHandle,
            Status == STATUS_INFO_LENGTH_MISMATCH ? XLL_BLOCK : XLL_FAIL,
            "Status = 0x%X",
            Status
            );
    }

    xEndVariation( LogHandle );

    xSetFunctionName( LogHandle, "NtQueryObject (ObjectHandleFlagInformation)" );
    xStartVariation( LogHandle, VariationBuffer );
    pObjHandleFlagInfo = (POBJECT_HANDLE_FLAG_INFORMATION)LargeBuffer;

    Status = NtQueryObject(
                ObjectHandle,
                ObjectHandleFlagInformation,
                pObjHandleFlagInfo,
                sizeof(OBJECT_HANDLE_FLAG_INFORMATION),
                NULL
                );

    if ( NT_SUCCESS(Status) ) {
        xLog(
            LogHandle,
            XLL_PASS,
            "Inherit=%d ProtectFromClose=%d",
            pObjHandleFlagInfo->Inherit,
            pObjHandleFlagInfo->ProtectFromClose
            );
    } else {
        xLog(
            LogHandle,
            Status == STATUS_INFO_LENGTH_MISMATCH ? XLL_BLOCK : XLL_FAIL,
            "Status = 0x%X",
            Status
            );
    }

    xEndVariation( LogHandle );
}


NTSTATUS
ListObjectsInDirectory(
    HANDLE                  LogHandle,
    HANDLE                  Handle,
    PUNICODE_STRING         Dir,
    PWCHAR                  Path
)
{
    OBJECT_ATTRIBUTES ObjA;
    NTSTATUS Status;
    HANDLE NewHandle;
    PWCHAR NewPath = NULL;
    WCHAR devbuf[256];
    ULONG Context;
    ULONG c, RetLen;
    ULONG BufferLen;
    PVOID Buffer;
    OBJECT_DIRECTORY_INFORMATION *ObjDirPtr;
    PCHAR AnsiBuffer;

    AnsiBuffer = HeapAlloc( HeapHandle, HEAP_ZERO_MEMORY, 1024 );
    if ( !AnsiBuffer ) {
        return STATUS_NO_MEMORY;
    }

    InitializeObjectAttributes(
        &ObjA,
        Dir,
        OBJ_CASE_INSENSITIVE,
        Handle,
        NULL
        );

    wcsncpy( devbuf, Dir->Buffer, sizeof(devbuf) / sizeof(devbuf[0]) );
    devbuf[sizeof(devbuf) - 1] = L'\0';

    xSetFunctionName( LogHandle, "NtOpenDirectoryObject" );
    sprintf( AnsiBuffer, "%ws", devbuf );
    xStartVariation( LogHandle, AnsiBuffer );
    Status = NtOpenDirectoryObject( &NewHandle, MAXIMUM_ALLOWED, &ObjA );

    if ( !NT_SUCCESS(Status) ) {
        DbgPrint(
            "DIRLOG: NtOpenDirectoryObject failed 0x%x for directory %ws\n",
            Status,
            devbuf
            );

        xLog(
            LogHandle,
            XLL_BLOCK,
            "NtOpenDirectoryObject failed 0x%x for directory %ws",
            Status,
            devbuf
            );
        xEndVariation( LogHandle );
        HeapFree( HeapHandle, 0, AnsiBuffer );
        return Status;
    } else {
        xLog( LogHandle, XLL_PASS, "%ws", devbuf );
        xEndVariation( LogHandle );
    }

    xSetFunctionName( LogHandle, "NtQueryDirectoryObject" );

    Context = 0;
    BufferLen = 0x10000;
    Buffer = HeapAlloc( HeapHandle, HEAP_ZERO_MEMORY, BufferLen );

    if ( !Buffer ) {
        DbgPrint( "DIRLOG: unable to allocate memory\n" );
        xStartVariation( LogHandle, "memory allocation" );
        xLog( LogHandle, XLL_BLOCK, "unable to allocate memory" );
        xEndVariation( LogHandle );
        HeapFree( HeapHandle, 0, AnsiBuffer );
        return STATUS_NO_MEMORY;
    }

    xStartVariation( LogHandle, AnsiBuffer );
    Status = NtQueryDirectoryObject(
                NewHandle,
                Buffer,
                BufferLen,
                FALSE,
                TRUE,
                &Context,
                &RetLen
                );

    if ( !NT_SUCCESS(Status) && Status != STATUS_NO_MORE_ENTRIES ) {
        DbgPrint(
            "DIRLOG: NtQueryDirectoryObject failed 0x%x for directory %ws\n",
            Status,
            devbuf
            );

        xLog(
            LogHandle,
            XLL_BLOCK,
            "NtQueryDirectoryObject failed 0x%x for directory %ws",
            Status,
            devbuf
            );
        xEndVariation( LogHandle );
    }

    if ( NT_SUCCESS(Status) ) {

        xLog( LogHandle, XLL_PASS, "%ws", devbuf );
        xEndVariation( LogHandle );

        for ( ObjDirPtr=(POBJECT_DIRECTORY_INFORMATION)Buffer, c=0; \
              ObjDirPtr->Name.Length || ObjDirPtr->TypeName.Length; \
              c++, ObjDirPtr++ ) {
           ;
        }

        qsort( Buffer, c, sizeof(*ObjDirPtr), UnicodeCompare );

        for ( ObjDirPtr=(POBJECT_DIRECTORY_INFORMATION)Buffer; \
              ObjDirPtr->Name.Length || ObjDirPtr->TypeName.Length; \
              ObjDirPtr++ ) {

            wcsncpy( devbuf, ObjDirPtr->Name.Buffer, sizeof(devbuf)/sizeof(devbuf[0]) );
            devbuf[sizeof(devbuf)/sizeof(devbuf[0]) - 1] = L'\0';

            NewPath = HeapAlloc(
                        HeapHandle,
                        HEAP_ZERO_MEMORY, 
                        (wcslen(Path) + 1 + wcslen(devbuf) + 1) * sizeof(WCHAR)
                        );

            if ( !NewPath ) {
                DbgPrint( "DIRLOG: Memory allocation failed for path buffer!\n" );
                xStartVariation( LogHandle, "memory allocation" );
                xLog( LogHandle, XLL_BLOCK, "Memory allocation failed for path buffer!" );
                xEndVariation( LogHandle );
                break;
            }

            wcscpy ( NewPath, Path );
            if ( Path[wcslen(Path)-1] != '\\' && devbuf[wcslen(devbuf)-1] != '\\') {
                wcscat( NewPath, L"\\" );
            }
            wcscat( NewPath, devbuf );

            if ( wcsncmp(
                    ObjDirPtr->TypeName.Buffer,
                    L"Directory",
                    ObjDirPtr->TypeName.Length/sizeof(WCHAR)) == 0 ) {

                ListObjectsInDirectory(
                    LogHandle,
                    NewHandle,
                    &ObjDirPtr->Name,
                    NewPath
                    );
            } else {

                UNICODE_STRING String;
                WCHAR StringBuffer[1024];
                HANDLE ObjHandle = NULL;

                String.Buffer = StringBuffer;
                String.Length = 0;
                String.MaximumLength = sizeof(StringBuffer);

                if ( wcsncmp(
                        ObjDirPtr->TypeName.Buffer,
                        L"SymbolicLink",
                        ObjDirPtr->TypeName.Length/sizeof(WCHAR)) == 0 ) {

                    ObjHandle = HandleSymbolicLink( LogHandle, NewHandle, devbuf );
                } else {
                    ObjHandle = OpenObject(
                                    LogHandle,
                                    NewHandle,
                                    devbuf,
                                    ObjDirPtr
                                    );
                }
                    
                if ( ObjHandle ) {
                    QueryObject( LogHandle, ObjHandle, Path, devbuf );
                    NtClose( ObjHandle );
                }
            }

            HeapFree( HeapHandle, 0, NewPath );
        }
    }

    HeapFree( HeapHandle, 0, AnsiBuffer );
    HeapFree( HeapHandle, 0, Buffer );
    NtClose( NewHandle );
    return Status;
}


BOOL
WINAPI
DllMain(
    HINSTANCE   hInstance,
    DWORD       fdwReason,
    LPVOID      lpReserved
    )
{
    UNREFERENCED_PARAMETER( hInstance );
    UNREFERENCED_PARAMETER( lpReserved );

    if ( fdwReason == DLL_PROCESS_ATTACH ) {
        HeapHandle = HeapCreate( 0, 0, 0 );
        ASSERT( HeapHandle );
    } else if ( fdwReason == DLL_PROCESS_DETACH ) {
        HeapDestroy( HeapHandle );
    }

    return HeapHandle != NULL;
}


#ifdef _UNDER_NT
VOID
_cdecl
main(
    HANDLE LogHandle
    )
#else
VOID
WINAPI
StartTest(
    HANDLE LogHandle
    )
#endif

/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/

{
    UNICODE_STRING RootDir;

    xSetComponent( LogHandle, "Kernel", "Ob" );
    KdPrint(( "DIRLOG: StartTest\n" ));

    //
    // Start enumerating from root
    //
    RtlInitUnicodeString( &RootDir, L"\\" );
    ListObjectsInDirectory( LogHandle, NULL, &RootDir, L"" );
}


VOID
WINAPI
EndTest(
    VOID
    )

/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/

{
    KdPrint(( "DIRLOG: EndTest\n" ));
}
