/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    handle.c

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

BOOL
DumpHandles (
    IN HANDLE       HandleToDump,
    IN PVOID        pObjectType,
    IN ULONG        Flags
    );

BOOLEAN
DumpHandle(
   IN PVOID                Object,
   IN HANDLE               Handle,
   IN PVOID                pObjectType,
   IN ULONG                Flags
   );

DECLARE_API( handle  )

/*++

Routine Description:

    Dump the active handles

Arguments:

    args - [handle-to-dump [flags [process [TypeName]]]]
            if handle-to-dump is 0 dump all

Return Value:

    None

--*/

{

    ULONG        ProcessToDump;
    HANDLE       HandleToDump;
    ULONG        Flags;
    ULONG        Result;
    ULONG        nArgs;
    char         TypeName[ MAX_PATH ];
    PVOID        pObjectType;

    HandleToDump  = (HANDLE)0xFFFFFFFF;
    Flags         = 0x3; //by default dump bodies and objects for in use entries
    ProcessToDump = 0xFFFFFFFF;
    pObjectType   = NULL;

    nArgs = sscanf(args,"%lx %lx %lx",&HandleToDump,&Flags,&pObjectType);

    if (HandleToDump == (HANDLE)0xFFFFFFFF) {
        HandleToDump = 0;
    }

    if (HandleToDump != 0) {
        Flags |= 0x4;
    }

    if (pObjectType != 0) {
        dprintf("Searching for handles of type 0x%lx\n", pObjectType);
    }

    DumpHandles (HandleToDump, pObjectType, Flags);

    return;
}



//+---------------------------------------------------------------------------
//
//  Function:   DumpHandles
//
//  Synopsis:   Dump the handle table for the given process
//
//  Arguments:  [ProcessContents] -- process to dump
//              [RealProcessBase] -- base address of the process
//              [HandleToDump]    -- handle to look for - if 0 dump all
//              [pObjectType]     -- object type to look for
//              [Flags]           -- flags passed thru to DumpHandle
//
//  Returns:    TRUE if successful
//
//  History:    1-12-1998   benl   Created
//
//  Notes: Each segment of table has 0xFF or 8 bits worth of entries
//         the handle number's lowest 2 bit are application defined
//         so the indexes are gotten from the 3 8 bits ranges after
//         the first 2 bits
//
//----------------------------------------------------------------------------

BOOL
DumpHandles (
    IN HANDLE       HandleToDump,
    IN PVOID        pObjectType,
    IN ULONG        Flags
    )

{
    POBJECT_HANDLE_TABLE    HandleTablePointer;
    OBJECT_HANDLE_TABLE     HandleTable;
    ULONG                   ulRead;
    ULONG                   ulTopLevel;
    PVOID*                  rgEntries[OB_HANDLES_PER_TABLE];
    ULONG                   ulHandleNum = ((ULONG)HandleToDump & ~(sizeof(ULONG) - 1));

    HandleTablePointer = (POBJECT_HANDLE_TABLE)GetNtDebuggerData(ObpObjectHandleTable);

    if (!ReadMemory((DWORD)HandleTablePointer,
                    &HandleTable,
                    sizeof(OBJECT_HANDLE_TABLE),
                    &ulRead)) {

        dprintf("%08lx: Unable to read handle table\n", HandleTablePointer);
        return FALSE;
    }

    dprintf("Handle Table at %x with %d %s in use\n",
            HandleTablePointer,
            HandleTable.HandleCount,
            (HandleTable.HandleCount == 1) ? "Entry" : "Entries");

    if (ulHandleNum != 0) {

        if (ulHandleNum >= (ULONG)HandleTable.NextHandleNeedingPool) {
            dprintf("Invalid handle: 0x%x\n", ulHandleNum);
            return FALSE;
        }

        if (!ReadMemory((DWORD)(HandleTable.RootTable) +
                        (ulHandleNum >> OB_HANDLES_PER_TABLE_SHIFT),
                        &ulTopLevel,
                        sizeof(ulTopLevel),
                        &ulRead)) {
            dprintf("%08lx: Unable to read handle table\n",
                    (DWORD)(HandleTable.RootTable) +
                    (ulHandleNum >> OB_HANDLES_PER_TABLE_SHIFT));
            return FALSE;
        }

        if (!ulTopLevel) {
            dprintf("Invalid handle: 0x%x\n", ulHandleNum);
            return FALSE;
        }

        if (!ReadMemory(ulTopLevel +
                        (ulHandleNum & (OB_HANDLES_PER_TABLE * sizeof(PVOID) - 1)),
                        (PVOID)rgEntries,
                        sizeof(PVOID),
                        &ulRead)) {
            dprintf("%08lx: Unable to read handle table\n",
                    ulTopLevel);
            return FALSE;
        }

        DumpHandle(rgEntries[0], HandleToDump, pObjectType, Flags);

    } else {

        while (ulHandleNum < (ULONG)HandleTable.NextHandleNeedingPool) {

            if ((ulHandleNum & (OB_HANDLES_PER_TABLE * sizeof(PVOID) - 1)) == 0) {

                if (!ReadMemory((DWORD)(HandleTable.RootTable) +
                                (ulHandleNum >> OB_HANDLES_PER_TABLE_SHIFT),
                                &ulTopLevel,
                                sizeof(ulTopLevel),
                                &ulRead)) {
                    dprintf("%08lx: Unable to read handle table\n",
                            (DWORD)(HandleTable.RootTable) +
                            (ulHandleNum >> OB_HANDLES_PER_TABLE_SHIFT));
                    return FALSE;
                }

                if (!ulTopLevel) {
                    dprintf("%08lx: Unable to read handle table\n",
                            (DWORD)(HandleTable.RootTable) +
                            (ulHandleNum >> OB_HANDLES_PER_TABLE_SHIFT));
                    return FALSE;
                }

                if (!ReadMemory(ulTopLevel,
                                (PVOID)rgEntries,
                                sizeof(rgEntries),
                                &ulRead)) {
                    dprintf("%08lx: Unable to read handle table\n",
                            ulTopLevel);
                    return FALSE;
                }
            }

            DumpHandle(rgEntries[((ulHandleNum & (OB_HANDLES_PER_TABLE * sizeof(PVOID) - 1))) >> 2],
                (HANDLE)ulHandleNum, pObjectType, Flags);

            ulHandleNum += sizeof(ULONG);
        }

    } //endif on a specific handle

    return TRUE;
} // DumpHandles


//+---------------------------------------------------------------------------
//
//  Function:   DumpHandle
//
//  Synopsis:   Dump a particular Handle
//
//  Arguments:  [pHandleTableEntry] --  entry to dump
//              [Handle]            --  handle number of entry
//              [pObjectType]       --  only dump if object type matches this
//                                      if NULL dump everything
//              [Flags]             --  flags if 0x2 also dump the object
//                                            if 0x4 dump free entries
//
//  Returns:
//
//  History:    1-12-1998   benl   Created
//              1-12-1998   benl   modified
//
//  Notes:
//
//----------------------------------------------------------------------------

BOOLEAN
DumpHandle(
    IN PVOID                Object,
    IN HANDLE               Handle,
    IN PVOID                pObjectType,
    IN ULONG                Flags
    )
{
    ULONG         ulObjectHeaderAddr;
    ULONG         Result;

    ULONG         HandleAttributes;
    OBJECT_HEADER ObjectHeader;
    PVOID         ObjectBody;

    if (Object == NULL) {
        //only print if flag is set to 4
        if (Flags & 4)
        {
            dprintf("%08lx: zeroed handle\n", Handle);
        }
        return TRUE;
    }

    if (((ULONG_PTR)Object & 1) != 0) {
        //only print if flag is set to 4
        if (Flags & 4)
        {
            dprintf("%08lx: free handle\n", Handle);
        }
        return TRUE;
    }

    //actual hdr is sign extend value with the lowest 3 bits cancelled out
    //top bit marks whether entry is locked
    //lower 3 bits mark auditing, inheritance and protection
    ulObjectHeaderAddr = (ULONG)OBJECT_TO_OBJECT_HEADER(Object);

    if (!ReadMemory(ulObjectHeaderAddr,
                    &ObjectHeader,
                    sizeof(ObjectHeader),
                    &Result)) {
        dprintf("%08lx: Unable to read nonpaged object header\n",
                ulObjectHeaderAddr);
        return FALSE;
    }

    if (pObjectType != NULL && ObjectHeader.Type != pObjectType) {
        return TRUE;
    }

    ObjectBody = (PVOID) (ulObjectHeaderAddr + FIELD_OFFSET(OBJECT_HEADER, Body));
    dprintf("%08lx: Object: %08lx", Handle, ObjectBody);

    dprintf("\n");
    if (Flags & 2) {
        DumpObject( "    ",ObjectBody, &ObjectHeader,Flags );
    }

    EXPRLastDump = (ULONG)ObjectBody;
    dprintf("\n");
    return TRUE;
} // DumpHandle

