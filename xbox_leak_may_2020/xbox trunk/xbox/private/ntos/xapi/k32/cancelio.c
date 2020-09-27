/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    cancelio.c

Abstract:

    This module implements the Win32 CancelIo service.

--*/

#include "basedll.h"
#pragma hdrstop
#include <idexchan.h>

VOID
XapiDpcForCancelIo(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{
    PLIST_ENTRY CancelListHead;
    PLIST_ENTRY NextListEntry;
    PIRP Irp;

    CancelListHead = (PLIST_ENTRY)DeferredContext;

    //
    // Run through the list of IRPs to be canceled.
    //

    NextListEntry = CancelListHead->Flink;

    while (NextListEntry != CancelListHead) {

        Irp = CONTAINING_RECORD(NextListEntry, IRP, Tail.Overlay.DeviceQueueEntry.DeviceListEntry);
        NextListEntry = NextListEntry->Flink;

        Irp->IoStatus.Status = STATUS_CANCELLED;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
}

BOOL
WINAPI
CancelIo(
    HANDLE hFile
    )
/*++

Routine Description:

    This routine cancels all of the outstanding I/O for the specified handle
    for the specified file.

Arguments:

    hFile - Supplies the handle to the file whose pending I/O is to be
        canceled.

Return Value:

    TRUE -- The operation was successful.

    FALSE -- The operation failed.  Extended error status is available using
        GetLastError.

--*/
{
    LIST_ENTRY CancelListHead;
    NTSTATUS status;
    PFILE_OBJECT FileObject;
    KIRQL OldIrql;
    PLIST_ENTRY NextListEntry;
    PIRP Irp;
    KDPC WorkerDpc;

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    InitializeListHead(&CancelListHead);

    //
    // Reference the file object.
    //

    status = ObReferenceObjectByHandle(hFile, IoFileObjectType,
        (PVOID*)&FileObject);

    if (!NT_SUCCESS(status)) {
        XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Scan through the IDEX device queue for any pending IRPs that originated
    // from the supplied file object.  Synchronize with the IDEX channel
    // dispatcher by raising to DPC level.
    //

    OldIrql = KeRaiseIrqlToDpcLevel();

    NextListEntry = IdexChannelObject->DeviceQueue.DeviceListHead.Flink;

    while (NextListEntry != &IdexChannelObject->DeviceQueue.DeviceListHead) {

        Irp = CONTAINING_RECORD(NextListEntry, IRP, Tail.Overlay.DeviceQueueEntry.DeviceListEntry);
        NextListEntry = NextListEntry->Flink;

        if ((Irp->Tail.Overlay.OriginalFileObject == FileObject) &&
            ((Irp->Flags & IRP_NO_CANCELIO) == 0)) {
            RemoveEntryList(&Irp->Tail.Overlay.DeviceQueueEntry.DeviceListEntry);
            InsertTailList(&CancelListHead, &Irp->Tail.Overlay.DeviceQueueEntry.DeviceListEntry);
        }
    }

    KeLowerIrql(OldIrql);

    //
    // If there are any IRPs that need to be canceled, then do so from a worker
    // DPC.  This is done for two reasons.  First, for packets marked as "must
    // complete" using IoMarkIrpMustComplete, IoCompleteRequest must be called
    // at DPC level.  Second, if the number of "must complete" packets reach
    // zero and a shutdown request is pending, then the kernel routine
    // HalMustCompletePacketsFinished expects to be called from a DPC stack, not
    // an arbitrary thread stack.
    //

    if (!IsListEmpty(&CancelListHead)) {
        KeInitializeDpc(&WorkerDpc, XapiDpcForCancelIo, &CancelListHead);
        KeInsertQueueDpc(&WorkerDpc, NULL, NULL);
    }

    //
    // Dereference the file object.
    //

    ObDereferenceObject(FileObject);

    return TRUE;
}
