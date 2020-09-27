/*
 *
 * dmprofil.c
 *
 * Profile objects
 *
 */

#include "dmp.h"

// Profile object
typedef struct _POB {
    LIST_ENTRY le;
    ULONG ulNext;
    int iulInterval;
    PDMPROFILE_HANDLER pfn;
} POB, *PPOB;

void DeletePob(PVOID);
extern KEVENT kevtNull;
OBJECT_TYPE obtPob = { DmAllocatePoolWithTag, DmFreePool, NULL, DeletePob, NULL, &kevtNull, 'opmd' };
LIST_ENTRY lePobHead = { &lePobHead, &lePobHead };
BOOL g_fResetInterval;
ULONG ulNextProfInt;
ULONG ulCurProfBase;
ULONG rgulIntervalTable[] =
{
        1221,
        2441,
        4883,
        9766,
       19531,
       39063,
       78125,
      156250,
      312500,
      625000,
     1250000,
     2500000,
     5000000,
     5000000 | 0x80000000
};

void QueuePob(PPOB ppob)
{
    ULONG ulNext;
    PLIST_ENTRY ple;
    PPOB ppobBefore;
    BOOL fStart;

    /* Dequeue if already on the list */
    _asm cli
    if(ppob->le.Flink) {
        RemoveEntryList(&ppob->le);
    }
    _asm sti

    /* Start the profile interrupt or reset the profile interval if
     * necessary */
    ulNext = 1 << ppob->iulInterval;
    fStart = ulCurProfBase == 0;
    if(fStart || ulNext < ulCurProfBase) {
        g_dmi.HalProfileIntervalRoutine(rgulIntervalTable[ppob->iulInterval]);
        ulCurProfBase = ulNext;
        if(fStart)
            g_dmi.HalStartProfileRoutine(0);
    }

    /* Now set the time to fire and queue */
    _asm cli
    ppob->ulNext = ulNextProfInt + ulNext;
    ple = lePobHead.Flink;
    while(ple != &lePobHead) {
        ppobBefore = CONTAINING_RECORD(ple, POB, le);
        if((int)(ppob->ulNext - ppobBefore->ulNext) < 0)
            /* Here's where we insert */
            break;
        ple = ple->Flink;
    }
    /* ple now points to the following entry or to the list head if we go at
     * the end */
    ppob->le.Flink = ple;
    ppob->le.Blink = ple->Blink;
    ple->Blink->Flink = &ppob->le;
    ple->Blink = &ppob->le;
    _asm sti
}

void DeletePob(PVOID pv)
{
    PPOB ppob = (PPOB)pv;
    KIRQL irqlSav = KeRaiseIrqlToDpcLevel();

    /* Dequeue this guy */
    _asm {
        pushfd
        cli
    }
    RemoveEntryList(&ppob->le);
    _asm popfd

    /* If the list is now empty, end the interrupt */
    if(lePobHead.Flink == &lePobHead) {
        g_dmi.HalStopProfileRoutine(0);
        ulCurProfBase = 0;
    } else if(1 << ppob->iulInterval == ulCurProfBase) {
        /* May need to change the interval, but we need to wait until the next
         * scheduled interrupt is sent so we keep the interrupt chain timed
         * correctly */
        PLIST_ENTRY ple;
        _asm {
            pushfd
            cli
        }
        g_fResetInterval = 32;
        for(ple = lePobHead.Flink; ple != &lePobHead; ple = ple->Flink) {
            ppob = CONTAINING_RECORD(ple, POB, le);
            if(ppob->iulInterval < g_fResetInterval)
                g_fResetInterval = ppob->iulInterval + 1;
        }
        _asm popfd
    }
    KeLowerIrql(irqlSav);
}

HRESULT DmStartProfile(PHANDLE ph, ULONG ulInterval, PDMPROFILE_HANDLER pfn)
{
    PPOB ppob;
    NTSTATUS st;
    int iul;

    st = ObCreateObject(&obtPob, NULL, sizeof(POB), &ppob);
    if(!NT_SUCCESS(st))
        return HrFromStatus(st, E_FAIL);
    RtlZeroMemory(ppob, sizeof *ppob);
    
    /* Make sure we can get a handle for this guy */
    st = ObOpenObjectByPointer(ppob, &obtPob, ph);
    if(!NT_SUCCESS(st)) {
        ObDereferenceObject(ppob);
        return HrFromStatus(st, E_FAIL);
    }
    
    /* Find an interval to fire on */
    ulInterval &= 0x7fffffff;
    for(iul = 0; (int)rgulIntervalTable[iul] >= 0; ++iul) {
        if(ulInterval <= rgulIntervalTable[iul])
            break;
    }
    if(iul) {
        /* See whether greater or smaller interval is closest */
        if(rgulIntervalTable[iul] - ulInterval >
                ulInterval - rgulIntervalTable[iul - 1])
            --iul;
    }
    if((int)rgulIntervalTable[iul] < 0)
        --iul;
    ppob->iulInterval = iul;
    ppob->pfn = pfn;

    /* Queue the object and return */
    ObDereferenceObject(ppob);
    QueuePob(ppob);
    return XBDM_NOERR;
}

HRESULT DmStopProfile(HANDLE h)
{
    PPOB ppob;
    NTSTATUS st;

    /* If we're at raised irql (like inside the ISR), this isn't going to
     * work */
    if(KeGetCurrentIrql() > DISPATCH_LEVEL)
        return E_FAIL;
    st = NtClose(h);
    return NT_SUCCESS(st) ? XBDM_NOERR : HrFromStatus(st, E_FAIL);
}

void ProfInt(PKTRAP_FRAME ptf)
{
	DMN_PROFINT dmpi;
    PLIST_ENTRY ple;
    BOOL fReset;

	dmpi.SegCs = (USHORT)ptf->SegCs;
    dmpi.EFlags = ptf->EFlags;
	dmpi.Eip = ptf->Eip;
    
    /* Walk the list of installed interrupt handlers and call anybody who's
     * due */
    ple = lePobHead.Flink;
    while(ple != &lePobHead) {
        PPOB ppob = CONTAINING_RECORD(ple, POB, le);
        if((int)(ppob->ulNext - ulNextProfInt) > 0)
            /* Nobody else due */
            break;
        ple = ple->Flink;
        if(ppob->ulNext == ulNextProfInt) {
            if(g_fResetInterval) {
                g_dmi.HalProfileIntervalRoutine(rgulIntervalTable[g_fResetInterval -
                    1]);
                ulCurProfBase = 1 << (g_fResetInterval - 1);
                g_fResetInterval = 0;
            }
            ppob->pfn(&dmpi);
            QueuePob(ppob);
        }
    }

    /* Update our interrupt time */
    ulNextProfInt += ulCurProfBase;
}
