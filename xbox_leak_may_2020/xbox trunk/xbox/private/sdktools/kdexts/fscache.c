/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    fscache.c

Abstract:

    WinDbg Extension Api

--*/

#include "precomp.h"
#pragma hdrstop
#include "fscache.h"

VOID
DumpFsCacheLruList()
{
    PLIST_ENTRY LruList;
    LIST_ENTRY LruListNode;
    ULONG result;
    PFSCACHE_ELEMENT Element;
    PLIST_ENTRY ListEntry;
    FSCACHE_ELEMENT ElementNode;
    FSCACHE_EXTENSION CacheExtension;

    LruList = (PLIST_ENTRY)GetNtDebuggerData(FscLruList);

    if (!ReadMemory((DWORD)LruList, &LruListNode, sizeof(LIST_ENTRY), &result)) {
        dprintf("cannot read FscLruList\n");
        return;
    }

    ListEntry = LruListNode.Blink;

    while (ListEntry != LruList) {

        Element = CONTAINING_RECORD(ListEntry, FSCACHE_ELEMENT, ListEntry);

        if (!ReadMemory((DWORD)Element, &ElementNode, sizeof(FSCACHE_ELEMENT), &result)) {
            dprintf("cannot read element %p\n", Element);
            return;
        }

        if (ElementNode.CacheExtension == NULL) {
            dprintf("%p:    free,     buffer=%p\n",
                Element,
                PAGE_ALIGN(ElementNode.CacheBuffer)
                );
        } else {
            dprintf("%p: %c%c usage=%02x, buffer=%p block=%08x ",
                Element,
                ElementNode.ReadInProgress ? 'R' : 'P',
                ElementNode.ReadWaiters ? 'W' : ' ',
                ElementNode.UsageCount,
                PAGE_ALIGN(ElementNode.CacheBuffer),
                ElementNode.BlockNumber);

            if (ReadMemory((DWORD)ElementNode.CacheExtension, &CacheExtension,
                sizeof(FSCACHE_EXTENSION), &result)) {
                DumpObjectName(CacheExtension.TargetDeviceObject);
            }
            dprintf("\n");
        }

        ListEntry = (PLIST_ENTRY)ElementNode.ListEntry.Blink;
    }
}


DECLARE_API(fscache)
{
    DumpFsCacheLruList();
}
