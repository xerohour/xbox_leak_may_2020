/*
 *
 * dmbreak.c
 *
 * Breakpoint management
 *
 */

#include "dmp.h"

#define MAX_BPL_BREAKS 128
#define DMBP_ENABLED 1
#define DMBP_NOENABLE 2

typedef struct _BPL {
	int cBreaks;
	BYTE *rgpbBreaks[MAX_BPL_BREAKS];
	BYTE rgbInstrs[MAX_BPL_BREAKS];
	BYTE rgbFlags[MAX_BPL_BREAKS];
	struct _BPL *pbplNext;
} BPL;

BPL *pbplRoot;
BPL *pbplScratch;
RTL_CRITICAL_SECTION csBplScratch;
RTL_CRITICAL_SECTION csHwBps;
BPL *pbplTrace;
int iBreakTrace;
BOOL fHaltAllThreads;
DWORD dwExecState;
struct _HWBP {
	PVOID pvAddr;
	WORD wType;
	WORD wSize;
} rghwbp[4];

void InitBreakpoints(void)
{
	InitializeCriticalSection(&csBplScratch);
	InitializeCriticalSection(&csHwBps);
	DwExchangeDr6(0);
}

void DoEnableBreak(BPL *pbpl, int iBreak)
{
	KIRQL irqlSav;
	HARDWARE_PTE pte;

	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	if(!(pbpl->rgbFlags[iBreak] & (DMBP_ENABLED | DMBP_NOENABLE)) &&
			MmDbgWriteCheck(pbpl->rgpbBreaks[iBreak], &pte)) {
		pbpl->rgbInstrs[iBreak] = *pbpl->rgpbBreaks[iBreak];
		*pbpl->rgpbBreaks[iBreak] = 0xcc;
		pbpl->rgbFlags[iBreak] |= DMBP_ENABLED;
		MmDbgReleaseAddress(pbpl->rgpbBreaks[iBreak], &pte);
	}
	KeLowerIrql(irqlSav);
}

void DoDisableBreak(BPL *pbpl, int iBreak)
{
	KIRQL irqlSav;
	HARDWARE_PTE pte;

	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	if(pbpl->rgbFlags[iBreak] & DMBP_ENABLED &&
			MmDbgWriteCheck(pbpl->rgpbBreaks[iBreak], &pte)) {
		*pbpl->rgpbBreaks[iBreak] = pbpl->rgbInstrs[iBreak];
		MmDbgReleaseAddress(pbpl->rgpbBreaks[iBreak], &pte);
	}
	pbpl->rgbFlags[iBreak] &= ~DMBP_ENABLED;
	KeLowerIrql(irqlSav);
}

void DoSyncBreak(BPL *pbpl, int iBreak)
{
	KIRQL irqlSav;

	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	if(pbpl->rgbFlags[iBreak] & DMBP_ENABLED &&
			!(MmDbgReadCheck(pbpl->rgpbBreaks[iBreak]) ||
			*pbpl->rgpbBreaks[iBreak] != 0xcc))
		pbpl->rgbFlags[iBreak] &= ~DMBP_ENABLED;
	KeLowerIrql(irqlSav);
}

BOOL FFindBreak(BYTE *pbAddr, BPL **ppbplRet, int *piBreak, BOOL fAllocNew)
{
	BPL **ppbpl;
	BPL *pbpl;
	int iMic, iMac, i;
	BOOL fRet = TRUE;
	KIRQL irqlSav;

	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	if(!pbplRoot) {
		*ppbplRet = NULL;
		if(!fAllocNew)
			goto retfalse;
		pbplRoot = pbplScratch;
		pbplScratch = NULL;
		if(!pbplRoot)
			goto retfalse;
		RtlZeroMemory(pbplRoot, sizeof(BPL));
	}

	/* Find which BPL this goes in */
	for(ppbpl = &pbplRoot; pbpl = *ppbpl, pbpl->pbplNext;) {
		ppbpl = &pbpl->pbplNext;
		if(pbAddr >= pbpl->rgpbBreaks[0] && pbAddr < (*ppbpl)->rgpbBreaks[0])
			break;
	}

	*ppbplRet = pbpl;

	/* See if we can find the breakpoint */
again:
	iMic = 0;
	iMac = pbpl->cBreaks;
	while(iMic < iMac) {
		i = (iMic + iMac) / 2;
		if(pbAddr < pbpl->rgpbBreaks[i])
			iMac = i;
		else if(pbAddr > pbpl->rgpbBreaks[i])
			iMic = i + 1;
		else {
			*piBreak = i;
			break;
		}
	}
	if(iMic == iMac) {
		/* No such breakpoint, but if we've been requested to allocate space
		 * we need to ensure there is space */
		if(pbpl->cBreaks == MAX_BPL_BREAKS && fAllocNew) {
			BPL *pbplNew = pbplScratch;
			pbplScratch = NULL;
			if(!pbplNew) {
				*ppbplRet = NULL;
				goto retfalse;
			}
			pbplNew->cBreaks = pbpl->cBreaks / 2;
			pbpl->cBreaks -= pbplNew->cBreaks;
			memcpy(pbplNew->rgpbBreaks, &pbpl->rgpbBreaks[pbpl->cBreaks],
				pbplNew->cBreaks * sizeof (BYTE *));
			memcpy(pbplNew->rgbInstrs, &pbpl->rgbInstrs[pbpl->cBreaks],
				pbplNew->cBreaks);
			memcpy(pbplNew->rgbFlags, &pbpl->rgbFlags[pbpl->cBreaks],
				pbplNew->cBreaks);
			pbplNew->pbplNext = pbpl->pbplNext;
			pbpl->pbplNext = pbplNew;
			if(pbAddr >= pbplNew->rgpbBreaks[0])
				pbpl = pbplNew;
			goto again;
		}
		*piBreak = iMic;
retfalse:
		fRet = FALSE;
	}
	KeLowerIrql(irqlSav);
	return fRet;
}

BOOL FIsBreakpoint(BYTE *pbAddr)
{
	BPL *pbpl;
	int iBreak;

	return(FFindBreak(pbAddr, &pbpl, &iBreak, FALSE));
}

BOOL FGetNextBreakpoint(BYTE **ppb)
{
	BPL *pbpl;
	int iBreak;
	BYTE *pb = *ppb + 1;
	KIRQL irqlSav;
	BOOL fRet;

	/* Find where this breakpoint would go and return whatever is there */
	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	FFindBreak(pb, &pbpl, &iBreak, FALSE);
	if(pbpl && iBreak == pbpl->cBreaks) {
		pbpl = pbpl->pbplNext;
		iBreak = 0;
	}
	if(pbpl) {
		*ppb = pbpl->rgpbBreaks[iBreak];
		fRet = TRUE;
	} else
		fRet = FALSE;
	KeLowerIrql(irqlSav);
	return fRet;
}

void EnableBreakpointsInRange(PBYTE pbMin, PBYTE pbMax, BOOL fForce)
{
    KIRQL irqlSav;
    BPL *pbpl;
    int iBreak;
    DWORD dwMask = fForce ? ~DMBP_ENABLED : -1;

    /* If fForce is set, we'll assume that any breakpoints that were previously
     * set in this range have been overwritten by something else, and that
     * we should consider them already disabled */

    KeRaiseIrql(HIGH_LEVEL, &irqlSav);
    /* Find the first breakpoint in our range */
    FFindBreak(pbMin, &pbpl, &iBreak, FALSE);
    while(pbpl && iBreak == pbpl->cBreaks) {
        pbpl = pbpl->pbplNext;
        iBreak = 0;
    }
    while(pbpl && pbpl->rgpbBreaks[iBreak] < pbMax) {
        pbpl->rgbFlags[iBreak] &= dwMask;
        DoEnableBreak(pbpl, iBreak++);
        while(pbpl && iBreak == pbpl->cBreaks) {
            pbpl = pbpl->pbplNext;
            iBreak = 0;
        }
    }
    KeLowerIrql(irqlSav);
}

void DisableBreakpointsInRange(PBYTE pbMin, PBYTE pbMax)
{
    KIRQL irqlSav;
    BPL *pbpl;
    int iBreak;

    KeRaiseIrql(HIGH_LEVEL, &irqlSav);
    /* Find the first breakpoint in our range */
    FFindBreak(pbMin, &pbpl, &iBreak, FALSE);
    while(pbpl && iBreak == pbpl->cBreaks) {
        pbpl = pbpl->pbplNext;
        iBreak = 0;
    }
    while(pbpl && pbpl->rgpbBreaks[iBreak] < pbMax) {
        DoDisableBreak(pbpl, iBreak++);
        while(pbpl && iBreak == pbpl->cBreaks) {
            pbpl = pbpl->pbplNext;
            iBreak = 0;
        }
    }
    KeLowerIrql(irqlSav);
}

BOOL FGetMemory(BYTE *pbAddr, BYTE *pbValue)
{
	BOOL fRet = TRUE;
	BOOL fBP;
	BPL *pbpl;
	int iBreak;
	KIRQL irqlSav;

	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	fBP = FFindBreak(pbAddr, &pbpl, &iBreak, FALSE) &&
		(pbpl->rgbFlags[iBreak] & DMBP_ENABLED);
	if(fBP)
		*pbValue = pbpl->rgbInstrs[iBreak];
	else if(MmDbgReadCheck(pbAddr))
		*pbValue = *pbAddr;
	else
		fRet = FALSE;
	KeLowerIrql(irqlSav);
	return fRet;
}

BOOL FSetMemory(BYTE *pbAddr, BYTE bValue)
{
	BOOL fRet = TRUE;
	BOOL fBP;
	BPL *pbpl;
	int iBreak;
	KIRQL irqlSav;
	HARDWARE_PTE pte;

	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	fBP = FFindBreak(pbAddr, &pbpl, &iBreak, FALSE) &&
		(pbpl->rgbFlags[iBreak] & DMBP_ENABLED);
	if(fBP)
		pbpl->rgbInstrs[iBreak] = bValue;
	else if(MmDbgWriteCheck(pbAddr, &pte)) {
		*pbAddr = bValue;
		MmDbgReleaseAddress(pbAddr, &pte);
	} else
		fRet = FALSE;
	KeLowerIrql(irqlSav);
	return fRet;
}

BOOL FAddBreakpoint(BYTE *pbAddr)
{
	BPL *pbpl;
	int iBreak;
	int iT;
	BOOL fRet = FALSE;
	KIRQL irqlSav;

	/* If we need a new BPL during the course of execution, we won't be able
	 * to allocate one since we're at a high irql.  Allocate it early so we
	 * have it when we need it */
	RtlEnterCriticalSection(&csBplScratch);
	if(!pbplScratch)
		pbplScratch = DmAllocatePool(sizeof(BPL));

	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	if(FFindBreak(pbAddr, &pbpl, &iBreak, TRUE)) {
		DoEnableBreak(pbpl, iBreak);
		/* Already got a breakpoint there */
		goto rettrue;
	}

	if(!pbpl) {
		/* Can't add one */
		goto retfalse;
	}

	ASSERT(pbpl->cBreaks < MAX_BPL_BREAKS);

	for(iT = pbpl->cBreaks++; iT-- > iBreak; ) {
		pbpl->rgpbBreaks[iT + 1] = pbpl->rgpbBreaks[iT];
		pbpl->rgbInstrs[iT + 1] = pbpl->rgbInstrs[iT];
		pbpl->rgbFlags[iT + 1] = pbpl->rgbFlags[iT];
	}

    /* BUGBUG -- we add the breakpoint regardless of whether or not there's
     * actually any code there.  The assumption is that if the section is not
     * loaded, we'll just mark this guy disabled when the section loads and
     * then reenable the breakpoint, forcing the int 3 into memory.  The
     * problem is that we might be going through this initial enable code
     * after the section is loaded but before we got notified.  In that case,
     * we'll lose the original instruction and be stuck with an int 3.  For
     * now, I'm going to ignore that problem as it seems highly infrequent */

	pbpl->rgpbBreaks[iBreak] = pbAddr;
	pbpl->rgbFlags[iBreak] = 0;
	DoEnableBreak(pbpl, iBreak);
rettrue:
	fRet = TRUE;
retfalse:
	KeLowerIrql(irqlSav);
	RtlLeaveCriticalSection(&csBplScratch);
	return fRet;
}

HRESULT DmSetBreakpoint(PVOID pbAddr)
{
	return FAddBreakpoint(pbAddr) ? XBDM_NOERR : E_FAIL;
}

BOOL FRemoveBreakpoint(BYTE *pbAddr)
{
	KIRQL irqlSav;
	BOOL fRet;
	BPL *pbpl;
	int iBreak;

	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	if(FFindBreak(pbAddr, &pbpl, &iBreak, FALSE)) {
		DoDisableBreak(pbpl, iBreak);
		for(--pbpl->cBreaks; iBreak < pbpl->cBreaks; ++iBreak) {
			pbpl->rgpbBreaks[iBreak] = pbpl->rgpbBreaks[iBreak + 1];
			pbpl->rgbInstrs[iBreak] = pbpl->rgbInstrs[iBreak + 1];
			pbpl->rgbFlags[iBreak] = pbpl->rgbFlags[iBreak + 1];
		}
		fRet = TRUE;
	} else
		fRet = FALSE;
	KeLowerIrql(irqlSav);
	return fRet;
}

HRESULT DmRemoveBreakpoint(PVOID pbAddr)
{
    return FRemoveBreakpoint(pbAddr) ? XBDM_NOERR : E_FAIL;
}

void RemoveAllBreakpoints(void)
{
	KIRQL irqlSav;
	BPL *pbpl;
	int iBreak;

	/* Remove int 3 breakpoints */
	KeRaiseIrql(HIGH_LEVEL, &irqlSav);
	for(pbpl = pbplRoot; pbpl; pbpl = pbpl->pbplNext) {
		for(iBreak = 0; iBreak < pbpl->cBreaks; ++iBreak)
			DoDisableBreak(pbpl, iBreak);
		pbpl->cBreaks = 0;
	}
	KeLowerIrql(irqlSav);

	/* Remove hardware breakpoints */
	for(iBreak = 0; iBreak < 4; ++iBreak)
		SetDreg(iBreak, NULL, 0, 0, FALSE);
}

void DisableBreakpoint(BYTE *pbAddr)
{
	BPL *pbpl;
	int iBreak;

	if(FFindBreak(pbAddr, &pbpl, &iBreak, FALSE))
		DoDisableBreak(pbpl, iBreak);
}

void SetHwbp(int ihwbp)
{
	BYTE bLen;
	BYTE bRw;

	if(rghwbp[ihwbp].wType == DMBREAK_NONE)
		SetDreg(ihwbp, NULL, 0, 0, FALSE);
	else {
		bLen = rghwbp[ihwbp].wSize - 1;
		switch(rghwbp[ihwbp].wType) {
		case DMBREAK_READWRITE:
			bRw = 3;
			break;
		case DMBREAK_WRITE:
			bRw = 1;
			break;
		case DMBREAK_EXECUTE:
			bRw = 0;
			bLen = 0;
			break;
		}
		/* Default to one-byte if size is invalid */
		if(bLen == 2 || bLen > 3)
			bLen = 0;
		SetDreg(ihwbp, rghwbp[ihwbp].pvAddr, bRw, bLen, TRUE);
	}
}

void SyncHwbps(void)
{
	int ihwbp;

    EnterCriticalSection(&csHwBps);
	for(ihwbp = 0; ihwbp < 4; ++ihwbp)
		if(rghwbp[ihwbp].wType != DMBREAK_NONE)
			SetHwbp(ihwbp);
    LeaveCriticalSection(&csHwBps);
}

HRESULT DmSetDataBreakpoint(BYTE *pbAddr, DWORD dwType, DWORD dwSize)
{
	int ihwbp;
    HRESULT hr = XBDM_NOERR;

    /* Validate the size */
	if(dwType != DMBREAK_NONE && !(dwSize == 1 || dwSize == 2 || dwSize == 4))
		return E_INVALIDARG;

    EnterCriticalSection(&csHwBps);
	/* First see whether this address/type combo is already used */
	for(ihwbp = 0; ihwbp < 4; ++ihwbp) {
		if(rghwbp[ihwbp].pvAddr == pbAddr) {
			/* If the type is none, we're going to clear this out */
			if(dwType == DMBREAK_NONE) {
				rghwbp[ihwbp].wType = DMBREAK_NONE;
				SetHwbp(ihwbp);
            } else if(rghwbp[ihwbp].wType == dwType)
                goto done;
		}
	}

	/* If we're clearing, then we've done our job */
    if(dwType == DMBREAK_NONE)
        goto done;

	/* Now find an unused slot */
	for(ihwbp = 0; ihwbp < 4; ++ihwbp) {
		if(rghwbp[ihwbp].wType == DMBREAK_NONE) {
			rghwbp[ihwbp].pvAddr = pbAddr;
			rghwbp[ihwbp].wType = (WORD)dwType;
			rghwbp[ihwbp].wSize = (WORD)dwSize;
			SetHwbp(ihwbp);
            goto done;
		}
	}

	hr = E_OUTOFMEMORY;
done:
    LeaveCriticalSection(&csHwBps);
    return hr;
}

HRESULT DmIsBreakpoint(PVOID pvAddr, LPDWORD pdwType)
{
    /* Look for fixed BPs first */
    if(FIsBreakpoint(pvAddr))
        *pdwType = DMBREAK_FIXED;
    else {
        /* See if we can find this in the hwbp list */
        int ihwbp;

        *pdwType = DMBREAK_NONE;
        EnterCriticalSection(&csHwBps);
        for(ihwbp = 0; ihwbp < 4; ++ihwbp) {
            if(rghwbp[ihwbp].wType != DMBREAK_NONE) {
                *pdwType = rghwbp[ihwbp].wType;
                break;
            }
        }
        LeaveCriticalSection(&csHwBps);
    }

    return XBDM_NOERR;
}

BOOL FMatchDataBreak(ULONG ulDr6, PDMN_DATABREAK pdmdb)
{
	int ihwbp;

#if 0
	/* If a trace, it's not a data break */
	if(ulDr6 & 0xc000)
		return FALSE;
#endif
	/* We'll return the first data breakpoint we find */
	for(ihwbp = 0; ihwbp < 4; ++ihwbp) {
		if(ulDr6 & (1 << ihwbp) && rghwbp[ihwbp].wType != DMBREAK_NONE) {
			pdmdb->BreakType = rghwbp[ihwbp].wType;
			pdmdb->DataAddress = rghwbp[ihwbp].pvAddr;
			return TRUE;
		}
	}
	return FALSE;
}

void DoStopGo(BOOL fGo)
{
	PETHREAD pthr;
	KIRQL irql;
	PLIST_ENTRY ple;
	DMTD *pdmtd;

	irql = KeRaiseIrqlToDpcLevel();
	if((dwExecState != DMN_EXEC_START) != !fGo) {
		/* Enumerate all threads and stop or start  those which are Xapi
		 * threads and which are not already marked as stopped */
		ple = pprocSystem->ThreadListHead.Flink;
		while(ple != &pprocSystem->ThreadListHead) {
			pthr = (PETHREAD)CONTAINING_RECORD(ple, KTHREAD, ThreadListEntry);
			ple = ple->Flink;
			pdmtd = (PDMTD)pthr->DebugData;
			/* If no thread data, we ignore this thread */
			if(pdmtd) {
			/* If execution is pending, we start any thread that is stopped */
				if(dwExecState == DMN_EXEC_PENDING && (pdmtd->DebugFlags
					& DMFLAG_STOPPED) && pdmtd->DebugEvent)
				{
					KeSetEvent(pdmtd->DebugEvent, EVENT_INCREMENT, FALSE);
				} else if(pthr->Tcb.TlsData && !(pdmtd->DebugFlags &
					DMFLAG_STOPPED) && pthr != PsGetCurrentThread())
				{
					if(fGo)
						KeResumeThread(&pthr->Tcb);
					else
						KeSuspendThread(&pthr->Tcb);
				}
			}
		}
		dwExecState = fGo ? DMN_EXEC_START : DMN_EXEC_STOP;
	}
	KeLowerIrql(irql);
}

void DoContinue(PETHREAD pthr, BOOL fException)
{
	KIRQL irql;
	DMTD *pdmtd;

	irql = KeRaiseIrqlToDpcLevel();
	pdmtd = (PDMTD)pthr->DebugData;
	if(dwExecState != DMN_EXEC_PENDING && pdmtd && pdmtd->DebugFlags &
		DMFLAG_STOPPED)
	{
		/* If all threads are suspended and we're going to continue this
		 * thread, then we're going to need to suspend it as well before it's
		 * marked as running */
		if(dwExecState != DMN_EXEC_START && pthr->Tcb.TlsData)
			KeSuspendThread(&pthr->Tcb);
		if(fException)
			pdmtd->DebugFlags |= DMFLAG_EXCEPTION;
		/* This thread could stay suspended forever unless we mark it as
		 * not stopped */
		pdmtd->DebugFlags &= ~DMFLAG_STOPPED;
		KeSetEvent(pdmtd->DebugEvent, EVENT_INCREMENT, FALSE);
	}
	KeLowerIrql(irql);
}

HRESULT DmGo(void)
{
	NotifyComponents(DM_EXEC, DMN_EXEC_START);
	DoStopGo(TRUE);
	return XBDM_NOERR;
}

HRESULT DmStop(void)
{
	DoStopGo(FALSE);
	NotifyComponents(DM_EXEC, DMN_EXEC_STOP);
	return XBDM_NOERR;
}

BOOLEAN DoHalt(void)
{
	BOOLEAN fHaltNow;
	PETHREAD pthr = PsGetCurrentThread();
	BOOL fHaltAgain = FALSE;
	PLIST_ENTRY ple;
	DMTD *pdmtd = (PDMTD)pthr->DebugData;

	if(fHaltAllThreads & HF_DEFERRED) {
		/* We've come in here with deferred work to do.  We clear the
		 * deferred state and do the work */
		fHaltAllThreads &= ~HF_DEFERRED;
		fHaltAllThreads |= HF_CLEANUP;
		/* We can't tolerate coming here at the wrong irql */
		if(KeGetCurrentIrql() != PASSIVE_LEVEL)
			KeBugCheck(IRQL_NOT_LESS_OR_EQUAL);
		ProcessDfns();
	}

	fHaltNow = ((fHaltAllThreads & HF_BREAK) && pthr->Tcb.TlsData) ||
		(pdmtd && (pdmtd->DebugFlags & DMFLAG_HALTTHREAD));
	/* Only halt a thread if it's not already stopped */
	if(fHaltNow && pdmtd && (pdmtd->ExceptionContext ||
			(pdmtd->DebugFlags & DMFLAG_STOPPED)))
		fHaltNow = FALSE;
	if(!fHaltNow && !(fHaltAllThreads & HF_CLEANUP))
		return FALSE;
	fHaltAllThreads &= ~(HF_BREAK | HF_CLEANUP);
	fHaltAgain = FALSE;
	if(pdmtd)
		pdmtd->DebugFlags &= ~DMFLAG_HALTTHREAD;
	ple = pprocSystem->ThreadListHead.Flink;
	while(ple != &pprocSystem->ThreadListHead) {
		pthr = (PETHREAD)CONTAINING_RECORD(ple, KTHREAD, ThreadListEntry);
		ple = ple->Flink;
		pdmtd = (PDMTD)pthr->DebugData;
		if(pdmtd && (pdmtd->DebugFlags & DMFLAG_HALTTHREAD))
			fHaltAgain = TRUE;
	}
	KeGetCurrentPrcb()->DebugHaltThread = fHaltAgain ? DoHalt : NULL;
	return fHaltNow;
}

HRESULT DmHaltThread(DWORD tid)
{
	PETHREAD pthr;
	DMTD *pdmtd;
	BOOL fGotThread = FALSE;

	if(tid) {
		NTSTATUS st;
		st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
		if(NT_SUCCESS(st)) {
			/* We can only halt xapi threads, and we want to be able to halt
			 * any xapi thread */
			if(pthr->Tcb.TlsData) {
				fGotThread = TRUE;
				pdmtd = pthr->DebugData;
				if(pdmtd)
					pdmtd->DebugFlags |= DMFLAG_HALTTHREAD;
			}
			ObDereferenceObject(pthr);
		} else
			pthr = NULL;
	} else {
		SetupHalt(HF_BREAK);
		return XBDM_NOERR;
	}

	if(fGotThread && pdmtd) {
		KeGetCurrentPrcb()->DebugHaltThread = DoHalt;
		return XBDM_NOERR;
	}
	return fGotThread ? E_UNEXPECTED : XBDM_NOTHREAD;
}

void SetupHalt(DWORD dw)
{
	_asm {
		pushfd
		cli
		mov eax, dw
		or fHaltAllThreads, eax
		popfd
	}
	KeGetCurrentPrcb()->DebugHaltThread = DoHalt;
}

HRESULT DmContinueThread(DWORD tid, BOOL fException)
{
	NTSTATUS st;
	PETHREAD pthr;
	DMTD *pdmtd;

	if(dwExecState == DMN_EXEC_PENDING)
		return XBDM_NOTHREAD;
	st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
	if(NT_SUCCESS(st))
		ObDereferenceObject(pthr);
	else
		return XBDM_NOTHREAD;
	pdmtd = pthr->DebugData;
	if(!pdmtd)
		return XBDM_NOTHREAD;
	if(!(pdmtd->DebugFlags & DMFLAG_STOPPED))
		return XBDM_NOTSTOPPED;
	DoContinue(pthr, fException);
	return XBDM_NOERR;
}

void PrepareToStop(void)
{
	DMTD *pdmtd = DmGetCurrentDmtd();

	/* This thread is going to stop at an exception.  We need to prepare for
	 * the stop before we can advertise it, lest somebody try to continue
	 * before we've marked the stop */
	if(!pdmtd) {
		// what now?
		ASSERT(FALSE);
		return;
	}
	if(pdmtd->DebugEvent) {
		/* Mark this thread as stopped */
		KeResetEvent(pdmtd->DebugEvent);
		pdmtd->DebugFlags |= DMFLAG_STOPPED;
		/* Stop everything */
		DmStop();
	}
}

BOOL FStopAtException(void)
{
	NTSTATUS st;
	DMTD *pdmtd = DmGetCurrentDmtd();
	BOOL fRet = TRUE;

	if(!pdmtd) {
		// what now?
		ASSERT(FALSE);
	} else if(pdmtd->DebugEvent) {
		/* Wait for the continue */
		st = KeWaitForSingleObject(pdmtd->DebugEvent, UserRequest, UserMode,
			FALSE, NULL);
		ASSERT(NT_SUCCESS(st));
		fRet = pdmtd->DebugFlags & DMFLAG_EXCEPTION;
		pdmtd->DebugFlags &= ~(DMFLAG_STOPPED | DMFLAG_EXCEPTION);
		/* If we're going to do a function step, we need to set everything
		 * up */
		if((pdmtd->DebugFlags & (DMFLAG_FUNCCALL | DMFLAG_INFUNCCALL)) ==
			DMFLAG_FUNCCALL)
		{
			PFCS pfcs = pdmtd->FuncCallData;
			pdmtd->DebugFlags |= DMFLAG_INFUNCCALL;
			ASSERT(pdmtd->ExceptionContext && pfcs);
			pfcs->SavedEip = pdmtd->ExceptionContext->Eip;
			pdmtd->ExceptionContext->Eip = (ULONG)SetupFuncCall;
		}
	}
	return fRet;
}

HRESULT DmSetupFunctionCall(DWORD tid)
{
	NTSTATUS st;
	PETHREAD pthr;
	DMTD *pdmtd;
	KIRQL irqlSav;
	HRESULT hr;

	irqlSav = KeRaiseIrqlToDpcLevel();
	st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
	if(NT_SUCCESS(st)) {
		ObDereferenceObject(pthr);
		pdmtd = pthr->DebugData;
		if(!pdmtd)
			hr = XBDM_NOTHREAD;
		else if(!pdmtd->ExceptionContext || (pdmtd->DebugFlags &
				(DMFLAG_STARTTHREAD | DMFLAG_INFUNCCALL)))
			hr = XBDM_NOTSTOPPED;
		else if(pdmtd->DebugFlags & DMFLAG_FUNCCALL)
			hr = XBDM_NOERR;
		else {
			pdmtd->FuncCallData = DmAllocatePoolWithTag(sizeof(FCS), 'cfmd');
			if(pdmtd->FuncCallData) {
				PFCS pfcs = pdmtd->FuncCallData;
				pfcs->ReportedEsp = (ULONG)&pfcs->SavedEip;
				pdmtd->DebugFlags |= DMFLAG_FUNCCALL;
				hr = XBDM_NOERR;
			} else
				hr = E_OUTOFMEMORY;
		}
	} else
		hr = XBDM_NOTHREAD;
	KeLowerIrql(irqlSav);
	return hr;
}

DWORD DwPreFuncCall(PFCS *ppfcs)
{
	DMTD *pdmtd = DmGetCurrentDmtd();

	*ppfcs = pdmtd->FuncCallData;
	pdmtd->DebugFlags &= ~(DMFLAG_FUNCCALL | DMFLAG_INFUNCCALL);

	return (BYTE *)(&(*ppfcs)->SavedEip) - (BYTE *)(*ppfcs)->ReportedEsp;
}

void PostFuncCall(PFCS pfcs)
{
	DmFreePool(pfcs);
}

void ExceptionsToKd(void)
{
	PLIST_ENTRY ple;
	DMTD *pdmtd;
	PETHREAD pthr;
	KIRQL irqlSav;

	irqlSav = KeRaiseIrqlToDpcLevel();
	ple = pprocSystem->ThreadListHead.Flink;
	while(ple != &pprocSystem->ThreadListHead) {
		pthr = (PETHREAD)CONTAINING_RECORD(ple, KTHREAD, ThreadListEntry);
		ple = ple->Flink;
		pdmtd = (PDMTD)pthr->DebugData;
		if(pdmtd) {
			if(pdmtd->ExceptionContext) {
				/* Any thread stopped on an exception gets continued to kd */
				pdmtd->DebugFlags &= ~DMFLAG_STOPPED;
				pdmtd->DebugFlags |= DMFLAG_KDEXCEPT;
				if(pdmtd->DebugEvent)
					KeSetEvent(pdmtd->DebugEvent, EVENT_INCREMENT, FALSE);
			} else if(dwExecState == DMN_EXEC_STOP && pthr->Tcb.TlsData)
				/* We implicitly resume all stopped threads */
				KeResumeThread(&pthr->Tcb);
		}
	}
	dwExecState = DMN_EXEC_START;
	KeLowerIrql(irqlSav);
	/* Notify of execution change */
	NotifyComponents(DM_EXEC, dwExecState);
}

void ResumeAfterBreakpoint(PCONTEXT pcr)
{
	BPL *pbpl;
	int iBreak;

	if(FFindBreak((BYTE *)pcr->Eip, &pbpl, &iBreak, FALSE)) {
		/* This is our breakpoint, so get the instruction back in there and
		 * execute it */
		DoDisableBreak(pbpl, iBreak);
		pcr->EFlags |= 0x100; // single step
		pbplTrace = pbpl;
		iBreakTrace = iBreak;
	} else if(MmDbgReadCheck((PVOID)pcr->Eip)) {
		if(*(BYTE *)pcr->Eip == 0xcc)
			/* Not our breakpoint, so skip the int 3 */
			++pcr->Eip;
		else if(*(BYTE *)pcr->Eip == 0xcd && *(BYTE *)(pcr->Eip + 1) == 0x03)
			/* A two byte breakpoint to skip */
			pcr->Eip += 2;
	}
}

BOOL FBreakTrace(PCONTEXT pcr)
{
	if(pbplTrace) {
		pcr->EFlags &= ~0x100;
		DoEnableBreak(pbplTrace, iBreakTrace);
		pbplTrace = NULL;
		return TRUE;
	}
	return FALSE;
}

HRESULT DmGetThreadList(LPDWORD rgdwThreads, LPDWORD pcThreads)
{
	DWORD cthr;
	KIRQL irql;
	PLIST_ENTRY ple;
	PETHREAD pthr;
	PDMTD pdmtd;
	HRESULT hr = XBDM_NOERR;

    if (!pcThreads || !rgdwThreads)
        return E_INVALIDARG;

	/* We need to disable thread switches while walking the thread list */
	irql = KeRaiseIrqlToDpcLevel();
	ple = pprocSystem->ThreadListHead.Flink;
	cthr = 0;
	/* If execution is pending, we say there are no threads */
	if(dwExecState != DMN_EXEC_PENDING) {
		while(ple != &pprocSystem->ThreadListHead && cthr < *pcThreads) {
			pthr = (PETHREAD)CONTAINING_RECORD(ple, KTHREAD, ThreadListEntry);
			ple = ple->Flink;
			pdmtd = (PDMTD)pthr->DebugData;
			/* We'll only list XAPI or stopped threads for now */
			if(pthr->Tcb.TlsData || (pdmtd && (pdmtd->DebugFlags &
					DMFLAG_STOPPED)))
				rgdwThreads[cthr++] = (DWORD)pthr->UniqueThread;
		}
		if(ple != &pprocSystem->ThreadListHead)
			/* Too many threads */
			hr = XBDM_BUFFER_TOO_SMALL;
	}
	KeLowerIrql(irql);

	*pcThreads = cthr;
	return hr;
}

HRESULT DmIsThreadStopped(DWORD tid, PDM_THREADSTOP pdmts)
{
	PDMTD pdmtd;
	NTSTATUS st;
	HRESULT hr;
	PEXCEPTION_RECORD per;
	PETHREAD pthr;

    if (!pdmts)
        return E_INVALIDARG;
    
	st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
	if(NT_SUCCESS(st)) {
		pdmtd = pthr->DebugData;
		ObDereferenceObject(pthr);
	} else
		pdmtd = NULL;
	if(pdmtd) {
		if(pdmtd->DebugFlags & DMFLAG_STOPPED) {
			per = pdmtd->ExceptionRecord;
			/* We set the reason to none unless we can find a better reason */
			pdmts->NotifiedReason = DM_NONE;
			if(per) {
				switch(per->ExceptionCode) {
				case STATUS_BREAKPOINT:
					switch(per->ExceptionInformation[0]) {
					case BREAKPOINT_BREAK:
						pdmts->NotifiedReason = DM_BREAK;
						pdmts->u.Break.Address =
							(PVOID)pdmtd->ExceptionContext->Eip;
						pdmts->u.Break.ThreadId = tid;
						break;
					case BREAKPOINT_RTLASSERT:
						pdmts->NotifiedReason = DM_ASSERT;
						goto Debugstr;
					case BREAKPOINT_RIP:
						pdmts->NotifiedReason = DM_RIP;
						goto Debugstr;
					case BREAKPOINT_PRINT:
						pdmts->NotifiedReason = DM_DEBUGSTR;
Debugstr:
						pdmts->u.DebugStr.ThreadId = tid;
						pdmts->u.DebugStr.String = NULL;
						pdmts->u.DebugStr.Length = 0;
						break;
					}
					break;
				case STATUS_SINGLE_STEP:
					/* If a data breakpoint gets cleared after we take the
					 * breakpoint, we'll probably be wrong here, but that's
					 * OK */
					if(FMatchDataBreak(pdmtd->ExceptionDr6,
							&pdmts->u.DataBreak))
						pdmts->NotifiedReason = DM_DATABREAK;
					else
						pdmts->NotifiedReason = DM_SINGLESTEP;
					pdmts->u.Break.Address =
						(PVOID)pdmtd->ExceptionContext->Eip;
					pdmts->u.Break.ThreadId = tid;
					break;
				default:
					pdmts->NotifiedReason = DM_EXCEPTION;
					pdmts->u.Exception.ThreadId = tid;
					pdmts->u.Exception.Code = per->ExceptionCode;
					pdmts->u.Exception.Address = per->ExceptionAddress;
					pdmts->u.Exception.Flags = 0;
					if(per->ExceptionFlags & EXCEPTION_NONCONTINUABLE)
						pdmts->u.Exception.Flags |= DM_EXCEPT_NONCONTINUABLE;
					if(pdmtd->DebugFlags & DMFLAG_FCE)
						pdmts->u.Exception.Flags |= DM_EXCEPT_FIRSTCHANCE;
					if(per->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
						pdmts->u.Exception.Information[0] = 
							per->ExceptionInformation[0];
						pdmts->u.Exception.Information[1] = 
							per->ExceptionInformation[1];
					}
					break;
				}
			}
			hr = XBDM_NOERR;
		} else
			hr = XBDM_NOTSTOPPED;
	} else
		hr = XBDM_NOTHREAD;
	return hr;
}

#define GR(reg) (pcrRet->reg = pcr->reg)

HRESULT DmGetThreadContext(DWORD tid, PCONTEXT pcrRet)
{
	PETHREAD pthr;
	PDMTD pdmtd;
	PCONTEXT pcr;
	NTSTATUS st;
	HRESULT hr;
	KIRQL irql;
	
    if (!pcrRet)
        return E_INVALIDARG;

	st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
	if(NT_SUCCESS(st)) {
		pdmtd = pthr->DebugData;
		ObDereferenceObject(pthr);
	} else
		pdmtd = NULL;
	irql = KeRaiseIrqlToDpcLevel();
	if(pdmtd) {
		DWORD dwFlags = pcrRet->ContextFlags;
		pcrRet->ContextFlags = 0;
		pcr = pdmtd->ExceptionContext;
		if(pcr) {
			if((dwFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {
				pcrRet->ContextFlags |= CONTEXT_CONTROL;
				if(pdmtd->DebugFlags & DMFLAG_FUNCCALL)
					pcrRet->Esp = ((PFCS)pdmtd->FuncCallData)->ReportedEsp;
				else
					GR(Esp);
				GR(Ebp);
				GR(Eip);
				GR(EFlags);
				GR(SegCs);
				GR(SegSs);
			}
			if((dwFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {
				pcrRet->ContextFlags |= CONTEXT_INTEGER;
				GR(Eax);
				GR(Ebx);
				GR(Ecx);
				GR(Edx);
				GR(Edi);
				GR(Esi);
			}
			if((dwFlags & CONTEXT_FLOATING_POINT) == CONTEXT_FLOATING_POINT ||
				(dwFlags & CONTEXT_EXTENDED_REGISTERS) ==
				CONTEXT_EXTENDED_REGISTERS)
			{
				pcrRet->ContextFlags |= CONTEXT_FLOATING_POINT |
					CONTEXT_EXTENDED_REGISTERS;
				memcpy(&pcrRet->FloatSave, &pcr->FloatSave,
					sizeof pcr->FloatSave);
			}
		} else if((dwFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {
			PKSWITCHFRAME pksf;
			USHORT ulCs = 0;
			USHORT ulSs = 0;
			pksf = (PKSWITCHFRAME)pthr->Tcb.KernelStack;
			pcrRet->Esp = (DWORD)(pksf + 1);
			pcrRet->Ebp = *(DWORD *)pcrRet->Esp;
			pcrRet->Eip = pksf->RetAddr;
			pcrRet->EFlags = pksf->Eflags;
			_asm {
				mov ulCs, cs
				mov ulSs, ss
			}
			pcrRet->SegCs = ulCs;
			pcrRet->SegSs = ulSs;
			pcrRet->ContextFlags = CONTEXT_CONTROL;
		} else
			pcrRet->ContextFlags = 0;
		hr = XBDM_NOERR;
	} else
		hr = XBDM_NOTHREAD;
	KeLowerIrql(irql);
	return hr;
}

#define SR(reg) (pcr->reg = pcrIn->reg)

HRESULT DmSetThreadContext(DWORD tid, PCONTEXT pcrIn)
{
	PETHREAD pthr;
	PDMTD pdmtd;
	PCONTEXT pcr;
	NTSTATUS st;
	HRESULT hr;
	KIRQL irql;
	
    if (!pcrIn)
        return E_INVALIDARG;

	st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
	if(NT_SUCCESS(st)) {
		pdmtd = pthr->DebugData;
		ObDereferenceObject(pthr);
	} else
		pdmtd = NULL;
	irql = KeRaiseIrqlToDpcLevel();
	if(!pdmtd)
		hr = XBDM_NOTHREAD;
	else if(!(pcr = pdmtd->ExceptionContext))
		hr = XBDM_NOTSTOPPED;
	else {
		if((pcrIn->ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {
			DWORD dwEflagChange = 0x100;

			if(pdmtd->DebugFlags & DMFLAG_FUNCCALL)
				((PFCS)pdmtd->FuncCallData)->ReportedEsp = pcrIn->Esp;
			else
				SR(Esp);
			SR(Ebp);
			SR(Eip);
			/* We only allow certain eflags bits to change */
			pcr->EFlags = pcr->EFlags ^ ((pcr->EFlags ^ pcrIn->EFlags) &
				dwEflagChange);
		}
		if((pcrIn->ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {
			SR(Eax);
			SR(Ebx);
			SR(Ecx);
			SR(Edx);
			SR(Edi);
			SR(Esi);
		}
		if((pcrIn->ContextFlags & CONTEXT_FLOATING_POINT) ==
				CONTEXT_FLOATING_POINT ||
				(pcrIn->ContextFlags & CONTEXT_EXTENDED_REGISTERS) ==
				CONTEXT_EXTENDED_REGISTERS)
			// BUG -- sanitize cr0 and other regs
			memcpy(&pcr->FloatSave, &pcrIn->FloatSave, sizeof pcr->FloatSave);

		hr = XBDM_NOERR;
	}
	KeLowerIrql(irql);
	return hr;
}

HRESULT DmGetThreadInfo(DWORD tid, PDM_THREADINFO pdmti)
{
	PETHREAD pthr;
	NTSTATUS st;
	HRESULT hr;
	DMTD *pdmtd;

    if (!pdmti)
        return E_INVALIDARG;

	st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
	if(NT_SUCCESS(st)) {
		pdmtd = (PDMTD)pthr->DebugData;
		pdmti->SuspendCount = pthr->Tcb.SuspendCount;
		if(dwExecState == DMN_EXEC_STOP && pthr->Tcb.TlsData && pdmtd &&
				!(pdmtd->DebugFlags & DMFLAG_STOPPED))
			/* The suspend count is artifically raised due to suspended
			 * execution */
			--pdmti->SuspendCount;
		pdmti->Priority = pthr->Tcb.Priority;
        pdmti->TlsBase = (PBYTE)pthr->Tcb.TlsData;
		ObDereferenceObject(pthr);
		hr = XBDM_NOERR;
	} else
		hr = XBDM_NOTHREAD;
	return hr;
}

HRESULT DmSetInitialBreakpoint(void)
{
	/* This is our startup breakpoint */
	if(dwExecState == DMN_EXEC_PENDING) {
		g_dmi.Flags |= DMIFLAG_BREAK;
		return XBDM_NOERR;
	}
	return E_FAIL;
}

HRESULT DmStopOn(DWORD dwFlags, BOOL fStop)
{
	{ _asm cli }
	if(fStop)
		g_grbitStopOn |= dwFlags;
	else
		g_grbitStopOn &= ~dwFlags;
	{ _asm sti }
	return XBDM_NOERR;
}

HRESULT DmReboot(DWORD dwFlags)
{
    if(!fBootWait) {
        if(dwFlags & DMBOOT_STOP)
            fBootWait = 3;
	    else if(dwFlags & DMBOOT_WAIT)
		    fBootWait = 1;
    }
    PrepareToReboot();

    if((dwFlags & DMBOOT_NODEBUG) && g_dmi.DisallowXbdm)
        *g_dmi.DisallowXbdm = TRUE;
	HalReturnToFirmware(dwFlags & DMBOOT_WARM ? HalQuickRebootRoutine :
		HalRebootRoutine);
	return XBDM_NOERR;
}

HRESULT DmSuspendThread(DWORD tid)
{
	PETHREAD pthr;
	NTSTATUS st;

	st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
	if(!NT_SUCCESS(st))
		return XBDM_NOTHREAD;
	KeSuspendThread(&pthr->Tcb);
	ObDereferenceObject(pthr);
	return XBDM_NOERR;
}

HRESULT DmResumeThread(DWORD tid)
{
	PETHREAD pthr;
	NTSTATUS st;

	st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
	if(!NT_SUCCESS(st))
		return XBDM_NOTHREAD;
	KeResumeThread(&pthr->Tcb);
	ObDereferenceObject(pthr);
	return XBDM_NOERR;
}

HRESULT DmGetXtlData(PDM_XTLDATA pdmxd)
{
    if(!g_dmxapi.XapiStarted)
        return XBDM_NOTHREAD;
    memset(pdmxd, 0, sizeof *pdmxd);
    pdmxd->LastErrorOffset = g_dmxapi.LastErrorTlsOff;
    return XBDM_NOERR;
}
