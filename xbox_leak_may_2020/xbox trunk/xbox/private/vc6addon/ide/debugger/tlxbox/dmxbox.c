/*
 *
 * dmxbox.cpp
 *
 * Xbox implementation of the DM
 *
 */

#include "precomp.h"
#pragma hdrstop
#include "fiber.h"
#include "fbrdbg.h"
//#include "xfiber.h"

#define REPLY(TYPE, name) TYPE name = (TYPE)LpDmMsg->rgb

/****************************************************************************/
//
// ContinueDebugEvent() queue.
//  We can only have one debug event pending per process, but there may be
//  one event pending for each process we are debugging.
//
//  There are 200 entries in a static sized queue.  If there are ever more
//  than 200 debug events  pending, AND windbg actually handles them all in
//  less than 1/5 second, we will be in trouble.  Until then, sleep soundly.
//
/****************************************************************************/
typedef struct tagCQUEUE {
    struct tagCQUEUE *next;
    DWORD  pid;
    DWORD  tid;
	BOOL fException;
} CQUEUE, *LPCQUEUE;

static LPCQUEUE lpcqFirst;
static LPCQUEUE lpcqLast;
static LPCQUEUE lpcqFree;
static CQUEUE cqueue[200];
static CRITICAL_SECTION csContinueQueue;

static DWORD dwExecState;
static PVOID rgpvDataBreak[4];
static BYTE rgbDataBreak[4][2];

extern HANDLE hEventCreateProcess;
extern HANDLE hEventContinue;
extern DEBUG_EVENT falseSSEvent;
extern CRITICAL_SECTION csKillQueue;
extern PKILLSTRUCT KillQueue;
extern BOOL         fDisconnected;
extern HPRCX        prcList;

HANDLE hevtReboot;
HANDLE hevtStartBreak;
DWORD g_tidBreak;
PVOID g_pvThreadStart;
DWORD dwTidStopped;
BOOL fDeqStopped;
PDMN_SESSION psessNotify;
BOOL g_fXapiDataValid;
DM_XTLDATA g_dmxd;

DEQ *pdeqFirst;
CRITICAL_SECTION csDEQ;
HANDLE hevtDEQ;

DWORD XboxNotification(ULONG, DWORD);
void GetModLoadDebugEvent(PDMN_MODLOAD, DEBUG_EVENT *);

BOOL FEnsureXapiData(void)
{
    if(!g_fXapiDataValid && SUCCEEDED(DmGetXtlData(&g_dmxd)))
        g_fXapiDataValid = TRUE;
    return g_fXapiDataValid;
}

BOOL WriteBreakPoint(PBREAKPOINT pbp)
{
	return SUCCEEDED(DmSetBreakpoint((PVOID)pbp->addr.addr.off));
}

BOOL RestoreBreakPoint(PBREAKPOINT pbp)
{
	return SUCCEEDED(DmRemoveBreakpoint((PVOID)pbp->addr.addr.off));
}

#ifdef XBOX
#define GR(reg) pcr->reg = dmcr.reg
#define SR(reg) dmcr.reg = pcr->reg

/* Xbox context doesn't match x86 context, so do the dance */
BOOL DbgGetThreadContext(HTHDX hthd, PCONTEXT pcr)
{
	CONTEXT dmcr;
	SR(ContextFlags);
	if(FAILED(DmGetThreadContext(hthd->tid, &dmcr)))
		return FALSE;
	GR(ContextFlags);
	if((dmcr.ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {
		GR(Esp);
		GR(Ebp);
		GR(Eip);
		GR(EFlags);
		GR(SegSs);
		GR(SegCs);
	}
	if((dmcr.ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {
		GR(Eax);
		GR(Ebx);
		GR(Ecx);
		GR(Edx);
		GR(Edi);
		GR(Esi);
	}
	return TRUE;
}

BOOL DbgSetThreadContext(HTHDX hthd, PCONTEXT pcr)
{
	CONTEXT dmcr;
	SR(ContextFlags);
	if((dmcr.ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {
		SR(Esp);
		SR(Ebp);
		SR(Eip);
		SR(EFlags);
		SR(SegSs);
		SR(SegCs);
	}
	if((dmcr.ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {
		SR(Eax);
		SR(Ebx);
		SR(Ecx);
		SR(Edx);
		SR(Edi);
		SR(Esi);
	}
	return SUCCEEDED(DmSetThreadContext(hthd->tid, &dmcr));
}
#else
BOOL DbgGetThreadContext(HTHDX hthd, PCONTEXT pcr)
{
	return(SUCCEEDED(DmGetThreadContext(hthd->tid, (PCONTEXT)pcr)));
}

BOOL DbgSetThreadContext(HTHDX hthd, PCONTEXT pcr)
{
	return SUCCEEDED(DmSetThreadContext(hthd->tid, (PCONTEXT)pcr));
}
#endif

#ifdef REGSYNC
void EnsureHthdReg(HTHDX hthdx, DWORD dwFlags)
{
	DWORD dwHave = hthdx->ctx.ContextFlags;
	DWORD dwNeed = dwFlags & ~dwHave;
	if(dwNeed) {
		hthdx->ctx.ContextFlags = dwNeed;
		DmGetThreadContext(hthdx->tid, (PCONTEXT)&hthdx->ctx);
		hthdx->ctx.ContextFlags |= dwHave;
	}
}
#endif

BOOL DbgReadMemory(HPRCX hprc, LPCVOID lpOffset, LPVOID lpb, DWORD cb, LPDWORD lpRead)
{
	DWORD cbT;
	if(!lpRead)
		lpRead = &cbT;
	return(SUCCEEDED(DmGetMemory(lpOffset, cb, lpb, lpRead)) && *lpRead == cb);
}

BOOL DbgWriteMemory(HPRCX hprc, LPVOID lpOffset, LPBYTE lpb, DWORD cb, LPDWORD lpWritten)
{
	DWORD cbT;
	if(!lpWritten)
		lpWritten = &cbT;
	return(SUCCEEDED(DmSetMemory(lpOffset, cb, lpb, lpWritten)) && *lpWritten == cb);
}

void DmSetFocus(HPRCX hprc)
{
	/* Xbox apps are always in focus, so don't need to do anything */
}

DWORD ProcessThreadStatCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
	REPLY(LPTST, lptst);
	DM_THREADINFO dmti;

    if (!hthd) {
        WaitForSingleObject(hprc->hEventCreateThread, INFINITE);
        hthd = HTHDXFromHPIDHTID(lpdbb->hpid, lpdbb->htid);
        assert(hthd != 0);
        if (!hthd) {
            LpDmMsg->xosdRet = xosdBadThread;
            return sizeof(TST);
        }
    }

	if(FAILED(DmGetThreadInfo(hthd->tid, &dmti))) {
		LpDmMsg->xosdRet = xosdBadThread;
		return sizeof(TST);
	}

	ZeroMemory(lptst, sizeof *lptst);
    lptst->dwThreadID = hthd->tid;

	if (hthd->szThreadName[0])
		sprintf(lptst->rgchThreadID, "%.*s", IDSTRINGSIZE-1, hthd->szThreadName );
	else
		lptst->rgchThreadID[0] = 0;

	lptst->dwSuspendCount = dmti.SuspendCount;
    lptst->dwSuspendCountMax = MAXIMUM_SUSPEND_COUNT;

	lptst->dwPriority = dmti.Priority;
	lptst->dwPriorityMax = 31;

	sprintf(lptst->rgchPriority, "%2d", lptst->dwPriority);

    if (hthd->tstate & ts_running) {
        lptst->dwState = tstRunning;
        _tcscpy(lptst->rgchState, "Running");
    } else if (hthd->tstate & ts_stopped) {
        lptst->dwState = tstStopped;
        if (hthd->tstate & ts_frozen) {
            //This thread is suspended via threadsuspenditself
            //So it can only get suspended once.
            lptst->dwSuspendCount = 1;
        }
        _tcscpy(lptst->rgchState, "Stopped");
    } else if (hthd->tstate & ts_dead) {
        lptst->dwState = tstExiting;
        _tcscpy(lptst->rgchState, "Exiting");
    } else if (hthd->tstate & ts_destroyed) {
        lptst->dwState = tstDead;
        _tcscpy(lptst->rgchState, "Dead");
    } else {
        lptst->dwState = tstRunnable;
        _tcscpy(lptst->rgchState, "Pre-run");
    }


    if (hthd->tstate & ts_rip ) {
        lptst->dwState |= tstRip;
        _tcscat(lptst->rgchState, ", RIPped");
    } else if (hthd->tstate & ts_first) {
        lptst->dwState |= tstExcept1st;
        _tcscat(lptst->rgchState, ", 1st chance");
    } else if (hthd->tstate & ts_second) {
        lptst->dwState |= tstExcept2nd;
        _tcscat(lptst->rgchState, ", 2nd chance");
    }


    if (hthd->tstate & ts_frozen) {
        lptst->dwState |= tstFrozen;
        _tcscat(lptst->rgchState, ", suspended");
    }

	LpDmMsg->xosdRet = xosdNone;
	return sizeof (TST);
}

BOOL MakeThreadSuspendItself(HTHDX hthd)
{
	return SUCCEEDED(DmSuspendThread(hthd->tid));
}

ULONG ProcessGetSpecialReg(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
	DWORD cb = 0;

	switch (*(DWORD*)lpdbb->rgbVar) {
	case CV_ALLREG_ERR:
		{
			DM_THREADINFO dmti;

			/* Last error is first in xapi data */
			if(FEnsureXapiData() && SUCCEEDED(DmGetThreadInfo(hthd->tid,
                    &dmti)) && dmti.TlsBase &&
                    SUCCEEDED(DmGetMemory((PBYTE)dmti.TlsBase +
                    g_dmxd.LastErrorOffset, sizeof(DWORD), LpDmMsg->rgb,
                    NULL)))
				cb = sizeof(DWORD);
		}
		break;
	}

    if (cb)
    {
        LpDmMsg->xosdRet = xosdNone;
        return cb;
    }

    LpDmMsg->xosdRet = xosdInvalidParameter;
    return 0;
}

ULONG ProcessSetSpecialReg(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
	/* We don't have any registers that we can set */
	LpDmMsg->xosdRet = xosdInvalidParameter;
    return 0;
}

VOID
ActionNLGDispatch (
    DEBUG_EVENT*	pde,
    HTHDX			hthd,
    DWORD			unused,
    LPVOID			lpv
    )
/*++

Routine Description:


--*/
{
    ADDR                addrPC, addrReturn;
    HNLG                hnlg;
    LPNLG               lpnlg;
    NLG_DESTINATION     nlgDest;
    XOSD                xosd;
    BOOL                fStop = FALSE;
    DWORD               cb;
    BREAKPOINT*			bp = (BREAKPOINT*)lpv;
    METHOD*				ContinueSSMethod;
	BREAKPOINT*			bpT = NULL;

    assert(bp);

    AddrFromHthdx (&addrPC, hthd);

    hnlg = CheckNLG (hthd->hprc, hthd, NLG_DISPATCH, &addrPC);
	assert (hnlg);
    ClearBPFlag (hthd);

    RestoreInstrBP (hthd, bp);

    lpnlg = LLLock (hnlg);

    addrReturn = lpnlg->addrNLGReturn;

    xosd = AddrReadMemory (
        hthd->hprc,
        hthd,
        &lpnlg->addrNLGDestination,
        &nlgDest,
        sizeof ( NLG_DESTINATION ),
        &cb
        );

    LLUnlock (hnlg);

    if (nlgDest.dwSig == NLG_SIG) {

		fStop = FALSE;

		if (nlgDest.dwCode == NLG_COM_PLUS_THUNK) {

			if (hthd->hprc->fStepInto) {
				fStop = TRUE;
			}

		} else if (nlgDest.dwCode != NLG_DESTRUCTOR_ENTER) {

			CANSTEP CanStep;
			ADDR    addrSP;

			GetAddrOff (addrSP) = GetSPFromNLGDest (hthd, &nlgDest);

			//
			// NOTE: This is really a stack-based BP
			//
			
			if (GetAddrOff (hthd->addrStack) < GetAddrOff (addrSP)) {

				fStop = TRUE;


#ifndef TARGET_i386

				SetAddrOff (&addrPC, nlgDest.uoffDestination);
				GetCanStep (hthd->hprc->hpid, hthd->htid, &addrPC, &CanStep);

				switch (CanStep.Flags) {

					case CANSTEP_YES:
						GetAddrOff (addrPC) += CanStep.PrologOffset;
						break;

					default:
						fStop = FALSE;
//						assert (FALSE);
						break;
				}
#endif
			}
		}

		if (fStop) {
		
//				ConsumeAllThreadEvents (hthd);
			SetAddrOff (&addrPC, nlgDest.uoffDestination);
			bpT = SetBP (hthd->hprc,
						 hthd,
						 bptpExec,
						 bpnsStop,
						 &addrPC,
						 (HPID)INVALID
						 );

			RegisterExpectedEvent (hthd->hprc,
								   hthd,
								   BREAKPOINT_DEBUG_EVENT,
								   (DWORD)bpT,
								   DONT_NOTIFY,
								   ActionNLGDestination,
								   FALSE,
								   bpT
								   );
		}
    }

    //
    // Keep ourselves registered. Then Consume will remove this BP
    //
	
    RegisterExpectedEvent (
        hthd->hprc,
        hthd,
        BREAKPOINT_DEBUG_EVENT,
        (DWORD)lpv,
        DONT_NOTIFY,
        ActionNLGDispatch,
        FALSE,
        lpv
        );

    ContinueSSMethod = (METHOD*)MHAlloc(sizeof(METHOD));
    assert (ContinueSSMethod);
    ContinueSSMethod->notifyFunction = MethodContinueSS;
    ContinueSSMethod->lparam         = ContinueSSMethod;
    ContinueSSMethod->lparam2        = lpv;
    if(lpv && lpv != EMBEDDED_BP)
        ++((PBREAKPOINT)lpv)->cthd;
    SingleStep (hthd, ContinueSSMethod, FALSE, FALSE);

} // ActionNLGDispatch

/*** PROCESSDMFNONLOCALGOTO
 *
 * PURPOSE:
 *
 * PARAMETERS:
 *
 * RETURN:
 *
 * DESCRIPTION:
 *
 *      This is called in response to a dmfNonLocalGoto command from the EM.
 *
 ****************************************************************************/

VOID
ProcessNonLocalGoto (
    HPRCX hprc,
    HTHDX hthd,
    LPDBB lpdbb
    )
{
    LPNLG   lpnlg = (LPNLG)lpdbb->rgbVar;
    HNLG    hnlg;
    XOSD    xosd = xosdNone;

    if ( lpnlg->fEnable ) {
        hnlg = LLCreate ( hprc->llnlg );

        if ( !hnlg ) {
            /*
            ** REVIEW: Memory Failure
            */
            assert ( FALSE );
        }
        else {
            LPNLG lpnlgT;
            BREAKPOINT *bp;
            LLAdd ( hprc->llnlg, hnlg );
            lpnlgT = LLLock ( hnlg );

            *lpnlgT = *lpnlg;

            emiAddr ( lpnlgT->addrNLGDispatch ) = 0;
            emiAddr ( lpnlgT->addrNLGReturn ) = 0;
            emiAddr ( lpnlgT->addrNLGReturn2 ) = 0;
            emiAddr ( lpnlgT->addrNLGDestination ) = 0;

            LLUnlock ( hnlg );
        }
    }
    else {
        hnlg = LLFind ( hprc->llnlg, NULL, &lpnlg->hemi, (LONG)nfiHEMI );
        if ( !hnlg ) {
            /*
            ** We better have it otherwise the EM shouldn't be telling us
            ** about to remove it.
            */
            assert ( FALSE );
        } else {
            LPNLG   lpnlgT = LLLock ( hnlg );

            BREAKPOINT *bp = FindBP( hprc, hthd, bptpExec, bpnsStop, &lpnlgT->addrNLGDispatch, TRUE);
            EXPECTED_EVENT *ee = PeeIsEventExpected(NULL, BREAKPOINT_DEBUG_EVENT, (DWORD)bp, TRUE);
            if (ee) {
                ConsumeSpecifiedEvent(ee);
            }

            LLUnlock ( hnlg );

            LLDelete ( hprc->llnlg, hnlg );
        }

    }

    Reply(0, &xosd, lpdbb->hpid);
} // ProcessDmfNonLocalGoto

/***
 *
 * PURPOSE:
 *
 * PARAMETERS:
 *
 * RETURN:
 *
 * DESCRIPTION:
 *
 ****************************************************************************/

INT
WINAPI
NLGComp (
    LPNLG lpnlg,
    LPV lpvKey,
    LONG lParam
    )
{
    NFI nfi = (NFI)lParam;

    switch ( nfi ) {
        case nfiHEMI :
            if ( lpnlg->hemi == *(LPHEMI)lpvKey ) {
                return fCmpEQ;
            } else {
                return fCmpLT;
            }
            break;

        default :
            assert ( FALSE );
			return fCmpEQ;
            break;
    }

} // NLGComp
/*** CHECKNLG
 *
 * PURPOSE:
 *
 * PARAMETERS:
 *
 * RETURN:
 *
 * DESCRIPTION:
 *
 ****************************************************************************/

HNLG
CheckNLG (
    HPRCX hprc,
    HTHDX hthd,
    NLG_LOCATION nlgLoc,
    LPADDR lpaddrPC
    )
{
    HNLG    hnlg = hnlgNull;
    HNLG    hnlgRet = hnlgNull;

    while ( !hnlgRet && ( hnlg = LLNext ( hprc->llnlg, hnlg ) ) ) {
                LPNLG   lpnlg = LLLock ( hnlg );
                LPADDR  lpaddr;
                LPADDR  lpaddr2 = NULL;

        switch ( nlgLoc ) {
            case NLG_DISPATCH :
                lpaddr = &lpnlg->addrNLGDispatch;
                break;

            case NLG_RETURN :
                lpaddr = &lpnlg->addrNLGReturn;
                lpaddr2 = &lpnlg->addrNLGReturn2;
                break;

            default :
                assert ( FALSE );
        }

        if ( FAddrsEq ( *lpaddr, *lpaddrPC ) ) {
            hnlgRet = hnlg;
        } else if ((lpaddr2 != NULL) && FAddrsEq ( *lpaddr2, *lpaddrPC ) ) {
            hnlgRet = hnlg;
        }
        LLUnlock ( hnlg );
    }

    return ( hnlgRet );

} // CheckNLG


VOID
ActionNLGDestination (
    DEBUG_EVENT* pde,
    HTHDX hthd,
    DWORD unused,
    LPVOID lpv
    )
{
    BREAKPOINT* bp = (BREAKPOINT*)lpv;

    assert(bp);
    RemoveBP(bp);
    hthd->fReturning = FALSE;
    ConsumeAllThreadEvents(hthd, FALSE);

    NotifyEM(&falseSSEvent, hthd, 0, 0);
}

void ProcessRemoteQuit(void) { _asm int 3 }

void ProcessVirtualQueryCmd(HPRCX hprc, LPDBB lpdbb)
{
	REPLY(MEMORY_BASIC_INFORMATION *, pmbi);
	LPCVOID lpv;
	BYTE b;
	DWORD cb;
	int cbRet;

	memset(pmbi, 0, sizeof *pmbi);
	lpv = (LPCVOID)((LPADDR)lpdbb->rgbVar)->addr.off;
	if(SUCCEEDED(DmGetMemory(lpv, 1, &b, &cb)))
	{
		LpDmMsg->xosdRet = xosdNone;
		/* this isn't really right, but it'll do for now */
		pmbi->BaseAddress = (PVOID)((DWORD)lpv & 0xfffff000);
		pmbi->AllocationBase = pmbi->BaseAddress;
		pmbi->AllocationProtect = cb ? PAGE_EXECUTE_READWRITE : PAGE_NOACCESS;
		pmbi->RegionSize = 0x1000;
		pmbi->State = cb ? MEM_COMMIT : MEM_FREE;
		pmbi->Protect = pmbi->AllocationProtect;
		pmbi->Type = MEM_PRIVATE;
		cbRet = sizeof *pmbi;
	}
	else
	{
		LpDmMsg->xosdRet = xosdGeneral;
		cbRet = 0;
	}

	Reply(cbRet, LpDmMsg, lpdbb->hpid);
}

void ProcessQuerySelectorCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb) { _asm int 3 }
void ProcessQueryTlsBaseCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb) { _asm int 3 }
void ProcessDebugActiveCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb) { _asm int 3 }

void ProcessGetDmInfoCmd(HPRCX hprc, LPDBB lpdbb, DWORD cb)
{
	REPLY(LPDMINFO, lpi);

	LpDmMsg->xosdRet = xosdNone;

	memset(lpi, 0, sizeof (LPDMINFO));

	/* I wish I knew what all these were.  These are just copied from the
	 * Win32x86 dm */
    lpi->mAsync = asyncRun |
                  asyncMem |
                  asyncStop |
                  asyncBP  |
                  asyncKill |
                  asyncWP |
                  asyncSpawn;
    lpi->fHasThreads = 1;
    lpi->fReturnStep = 0;
    lpi->fAlwaysFlat = 1;
    lpi->fHasReload  = 0;

    lpi->cbSpecialRegs = 0;
    lpi->MajorVersion = 0;
    lpi->MinorVersion = 0;

    lpi->Breakpoints = bptsExec |
                       bptsDataC |
                       bptsDataW |
                       bptsDataR |
                       bptsDataExec;

	lpi->Processor.Type = mptix86;
	lpi->Processor.Endian = endLittle;
	lpi->Processor.Level = 6;
	lpi->Processor.Flags = g_ProcFlags;

	//
    // hack so that TL can call tlfGetVersion before
    // reply buffer is initialized.
    //
    if ( cb >= (FIELD_OFFSET(DBB, rgbVar) + sizeof(DMINFO)) ) {
        memcpy(lpdbb->rgbVar, lpi, sizeof(DMINFO));
    }

    Reply( sizeof(DMINFO), LpDmMsg, lpdbb->hpid );
}

DWORD ProcessAsyncGoCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
    XOSD       xosd = xosdNone;
    DEBUG_EVENT de;

    DEBUG_PRINT("ProcessAsyncGoCmd called.\n\r");

    if ((hthd->tstate & ts_frozen)) {
		if (hthd->tstate & ts_stopped) {
			//
			// if at a debug event, it won't really be suspended,
			// so just clear the flag.
			//
			hthd->tstate &= ~ts_frozen;

		} else if (FAILED(DmResumeThread(hthd->tid))) {

            xosd = xosdBadThread;

        } else {

            hthd->tstate &= ~ts_frozen;

            /*
             * deal with dead, frozen, continued thread:
             */
            if ((hthd->tstate & ts_dead) && !(hthd->tstate & ts_stopped)) {

				_asm int 3

                de.dwDebugEventCode = DESTROY_THREAD_DEBUG_EVENT;
                de.dwProcessId = hprc->pid;
                de.dwThreadId = hthd->tid;
                NotifyEM(&de, hthd, 0, NULL);
                FreeHthdx(hthd);

                hprc->pstate &= ~ps_deadThread;
                for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {
                    if (hthd->tstate & ts_dead) {
                        hprc->pstate |= ps_deadThread;
                    }
                }

            }
        }
    }

    Reply(0, &xosd, lpdbb->hpid);
    return(xosd);
}

void ProcessAsyncStopCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
	/* Do we need to worry about stop/go? */
	XOSD xosd = SUCCEEDED(DmHaltThread(0)) ? xosdNone : xosdUnknown;
	Reply(0, &xosd, lpdbb->hpid);
}

BOOL
Enqueue_KillCmd(
    HPRCX   hprc
    )
{
    PKILLSTRUCT pk = NULL;

    pk = (PKILLSTRUCT)malloc (sizeof(KILLSTRUCT));
    pk->hprc = hprc;

    EnterCriticalSection (&csKillQueue);

    pk->next = KillQueue;
    KillQueue = pk;

    LeaveCriticalSection (&csKillQueue);

    return TRUE;
}

BOOL
Dequeue_KillCmd(
    HPRCX*  lphprc
    )
{
    PKILLSTRUCT pk = NULL;

    EnterCriticalSection(&csKillQueue);

    pk = KillQueue;
    if (pk) {
        KillQueue = pk->next;
    }

    LeaveCriticalSection(&csKillQueue);

    if (!pk) {
        return FALSE;
    }

    *lphprc = pk->hprc;
    free (pk);

    return TRUE;
}

BOOL
ProcessTerminateProcessCmd(
    HPRCX hprc,
    HTHDX hthd,
    LPDBB lpdbb
    )
{
    assert (hprc);
    return Enqueue_KillCmd (hprc);
}

VOID
ExitFailed(
    HPRCX   hprc,
    LPVOID  lpv
    )
/*++

Routine Description:

    This routine is called when we failed to receive the final exit event
    within a specified ammount of time.

Comments:

    Be very careful with this.  The process COULD HAVE exited successfully,
    but not yet notified the debugger.  Do not make any assumptions.

--*/
{
    if (InterlockedSetFlag (&hprc->fExited) == FLAG_ALREADY_SET) {

        //
        // Somebody beat us to the punch.
        //

        return;
    }

    ProcessUnloadCmd (hprc, NULL, NULL);
}

VOID
FlushThreadContexts(
    HPRCX   hprc
    )
{
    HTHDX   hthd = NULL;

    assert (hprc);
    for (hthd = hprc->hthdChild; hthd; hthd = hthd->next) {

        if (hthd->fContextDirty) {
            DbgSetThreadContext(hthd, &hthd->ctx);
            hthd->fContextDirty = FALSE;
        }
        hthd->fWowEvent = FALSE;
    }
}

VOID
CompleteTerminateProcessCmd(
    )
/*++

Routine Description:

    Terminate the process.


--*/
{
    HPRCX               hprc;
    HTHDX               hthd;
    BOOL                fStopped = FALSE;

    if (!Dequeue_KillCmd (&hprc)) {
        assert (FALSE);
        return ;
    }
    assert (hprc);

#if 0
    if (CrashDump) {
        ProcessUnloadCmd (hprc, NULL, NULL);
        return;
    }
#endif

	// we should probably flush the DEQ here as well
    ConsumeAllProcessEvents (hprc, TRUE);
    //ClearAllDebugRegisters (hprc);
    RemoveAllHprcBP (hprc);
    FlushThreadContexts (hprc);

    //
    // Setting the state to ps_killed means that nothing will ever
    // attempt to operate on the process again.  This is critical
    // for Win95.
    //

    hprc->pstate |= ps_killed;

    if ((hprc->pstate & ps_dead) || (hprc->rwHand == (HANDLE)INVALID)) {

        if (InterlockedSetFlag (&hprc->fExited) == FLAG_SET) {
            ProcessUnloadCmd (hprc, NULL, NULL);
        }

    } else {
		
		/* Make the box reboot */
		DmReboot(DMBOOT_WAIT | DMBOOT_WARM);

        for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {
            if (hthd->tstate & ts_stopped) {
                fStopped = TRUE;
                break;
            }
        }

        //
        // If we are stopped, we will never get the PROCESS_EXIT event,
        // so unload the proc manually here.
        //

        if (fStopped) {

            if (InterlockedSetFlag (&hprc->fExited) == FLAG_SET) {
                ProcessUnloadCmd (hprc, NULL, NULL);
            }

        } else {
            hprc->hExitFailed = CreateTimerEvent (hprc,
                                                  ExitFailed,
                                                  NULL,
                                                  2000,
                                                  TRUE);
        }
    }

}

int
NumberOfThreads(
    HPRCX hprc
    )
{
    int nthrds=1;
    HTHDX hthd = hprc->hthdChild;
    hthd = hthd->next;
    while(hthd != NULL){
        nthrds++;
        hthd = hthd->next;
    }

    return nthrds;
}

VOID
GetActiveFibers(
    HPRCX   hprc,
    LPVOID* buf
    )
{
    HTHDX hthd = hprc->hthdChild;
    TEB teb;
    int ct = 0;
    int cbr;

    //
    // Determine the fiber loaded on eachthread
    // Don't display that fiber
    //

    while(hthd != NULL){
        DbgReadMemory(hprc,(LPCVOID)hthd->offTeb,
                    (LPVOID)&teb, sizeof(teb),&cbr);
        DbgReadMemory(hprc,(LPCVOID)teb.NtTib.Self,
                    (LPVOID)&teb,sizeof(teb),&cbr);
        buf[ct++] = teb.NtTib.FiberData;
        hthd = hthd->next;
    }

}

void LocalProcessSystemServiceCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
    LPSSS       lpsss =  (LPSSS) lpdbb->rgbVar;

    switch( lpsss->ssvc ) {
    case ssvcFiberDebug:
    {
        HFBRX hfbr = hprc->FbrLst;
        OFBRS   ofbrs = *((OFBRS *) lpsss->rgbData);
        DWORD iAfbrs;
        LPVOID *Actvfbrs;
        int cb=0;
        iAfbrs = NumberOfThreads(hthd->hprc);
        Actvfbrs = malloc(iAfbrs*sizeof(LPVOID));
        GetActiveFibers(hthd->hprc,Actvfbrs);

        if(ofbrs.op == OFBR_SET_FBRCNTX){
            hprc->pFbrCntx = ofbrs.FbrCntx;
        } else if(ofbrs.op == OFBR_ENABLE_FBRS) {
            hprc->fUseFbrs = TRUE;
        } else if(ofbrs.op == OFBR_DISABLE_FBRS) {
            hprc->fUseFbrs = FALSE;
        } else if(ofbrs.op == OFBR_QUERY_LIST_SIZE) {
            cb = sizeof(int);
            //count size of the list of fibers
            while(hfbr){
                DWORD i;
                BOOL fskip;//skip fibers loaded in threads
                for(fskip = FALSE,i=0;i < iAfbrs; i++){
                    if(Actvfbrs[i] == hfbr->fbrstrt)
                        fskip = TRUE;
                }
                if(!fskip){
                    cb +=4;
                }
                hfbr = hfbr->next;
            }
            //put byte count at the beginning
            memcpy(lpsss->rgbData+sizeof(int),&cb,sizeof(int));
            cb = 2*sizeof(int);
            memcpy(lpsss->rgbData,&cb,sizeof(int));
            lpsss->cbReturned = cb;
        } else if(ofbrs.op == OFBR_GET_LIST){
            cb = sizeof(int);
            while(hfbr){
                BOOL fskip;//skip fibers loaded in threads
                DWORD i;
                for(fskip = FALSE,i=0;i<iAfbrs;i++){
                    if(Actvfbrs[i] == hfbr->fbrstrt)
                        fskip = TRUE;
                }
                if(!fskip){
                    memcpy(lpsss->rgbData+cb,&(hfbr->fbrcntx),4);
                    cb +=4;
                }
                hfbr = hfbr->next;
            }
            //put byte count at the beginning
            memcpy(lpsss->rgbData,&cb,sizeof(int));
            lpsss->cbReturned = cb;
        }

        LpDmMsg->xosdRet = xosdNone;
        memcpy(LpDmMsg->rgb,lpsss->rgbData,cb);
        Reply (cb, LpDmMsg, lpdbb->hpid);
        free(Actvfbrs);
    }
    break;

    default:
	    LpDmMsg->xosdRet = xosdUnsupported;
	    Reply(0, LpDmMsg, lpdbb->hpid);
        break;
    }
}

void ProcessSSVCCustomCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb) { _asm int 3 }
void ProcessIoctlGenericCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb) { _asm int 3 }

ULONG ProcessGetTimeStamp(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
	REPLY(LPTCSR, lptcsr);
	DM_XBE xbe;
	HRESULT hr;

	hr = DmGetXbeInfo((LPSTR)lpdbb->rgbVar, &xbe);
	if(hr == XBDM_NOSUCHFILE)
		LpDmMsg->xosdRet = xosdFileNotFound;
	else if(FAILED(hr))
		LpDmMsg->xosdRet = xosdUnknown;
	else {
		LpDmMsg->xosdRet = xosdNone;
		lptcsr->TimeStamp = xbe.TimeStamp;
		lptcsr->CheckSum = xbe.CheckSum;
	}
	return sizeof *lptcsr;
}

HRESULT HrEnsureNotifier(void)
{
    if(!psessNotify)
        return DmOpenNotificationSession(DM_PERSISTENT, &psessNotify);
    return XBDM_NOERR;
}

DWORD AttachNotification(DWORD dwNotification, DWORD dwParam)
{
	if((dwNotification & DM_NOTIFICATIONMASK) == DM_BREAK) {
		PDMN_BREAK pdmbr = (PDMN_BREAK)dwParam;
		/* This is our halt breakpoint, so we just need to capture the
		 * thread id */
		g_tidBreak = pdmbr->ThreadId;
		SetEvent(hevtStartBreak);
	}
	return 0;
}

DWORD LaunchNotification(DWORD dwNotification, DWORD dwParam)
{
	DEBUG_EVENT de;
	DMN_MODLOAD dmml;

	memset(&de, 0, sizeof de);
	switch(dwNotification & DM_NOTIFICATIONMASK) {
	case DM_CREATETHREAD:
#define pdmct ((PDMN_CREATETHREAD)dwParam)
		/* This is our first thread, so we flag its create info and wake up the
		 * launching thread to continue with the launch */
		g_tidBreak = pdmct->ThreadId;
		g_pvThreadStart = pdmct->StartAddress;
		SetEvent(hevtStartBreak);
		break;
#undef pdmct
	case DM_EXCEPTION:
#define pdme ((PDMN_EXCEPTION)dwParam)
		/* If this was a first-chance exception, then we will just continue
		 * it */
		if((pdme->Flags & DM_EXCEPT_FIRSTCHANCE) &&
			!(pdme->Flags & DM_EXCEPT_NONCONTINUABLE))
		{
			DmContinueThread(pdme->ThreadId, TRUE);
			DmGo();
			break;
		}
		/* Otherwise fall through */
#undef pdme
	case DM_BREAK:
		/* Uh oh, we failed to launch for some reason.  We need to inform the
		 * shell that there is no process, which we will do by leaving
		 * the start thread ID at zero */
		assert(!g_tidBreak);
		if((dwNotification & DM_NOTIFICATIONMASK) == DM_BREAK)
			/* Explicit launch failure, so reboot */
			DmReboot(DMBOOT_WAIT | DMBOOT_WARM);
#ifndef XBOX
		else
			/* Forward the error back on to kd if necessary */
			DmSendCommand(NULL, "kd exceptif", NULL, 0);
#endif
		SetEvent(hevtStartBreak);
		break;
	}
	return 0;
}

HRESULT HrLaunchTitle(SOS *psos)
{
	HRESULT hr;
	char szPathName[MAX_PATH];
	char szDirName[MAX_PATH];
	char szTitleName[MAX_PATH];
	char szTitleExt[8];
	char szDriveName[8];
	int cchDirName;
	char *szPath;
    char *szArgs;

	hr = HrEnsureNotifier();
	if(FAILED(hr))
		return hr;

#ifndef XBOX
	/* Reboot the box if we have to */
	if(dwExecState != DMN_EXEC_PENDING) {
        assert(!hevtReboot);
		hevtReboot = CreateEvent(NULL, FALSE, FALSE, NULL);
		hr = DmNotify(psessNotify, DM_EXEC, XboxNotification);
		if(SUCCEEDED(hr)) {
			DmReboot(DMBOOT_WAIT | DMBOOT_WARM);
			if(WaitForSingleObject(hevtReboot, 120000) == WAIT_TIMEOUT)
				hr = XBDM_CONNECTIONLOST;
			else {
				assert(dwExecState == DMN_EXEC_PENDING);
			}
		}
		CloseHandle(hevtReboot);
		hevtReboot = NULL;
	}

	if(FAILED(hr))
		return hr;
#endif

	/* Find the directory */
	szPath = psos->lszRemoteExe;
	/* Since splitpath can't understand names like 'xe:\foo', we need to look
	 * for something that appears to be of that form */
	if((szPath[0] == 'x' || szPath[0] == 'X') && szPath[1] && szPath[2] == ':')
		++szPath;
	_splitpath(szPath, szDriveName, szDirName, szTitleName, szTitleExt);
	cchDirName = strlen(szDirName);
	if(cchDirName && szDirName[cchDirName - 1] == '\\')
		szDirName[--cchDirName] = 0;
	sprintf(szPathName, "%s\\%s", szDriveName, szDirName[0] == '\\' ?
		szDirName + 1 : szDirName);
	strcat(szTitleName, szTitleExt);
    szArgs = psos->lszCmdLine;
    switch(szArgs[0]) {
    case 0:
        /* No args */
        szArgs = NULL;
        break;
    case ' ':
        /* VC prepends a space to the args, which we want to strip */
        ++szArgs;
        break;
    }
	return DmSetTitle(szPathName, szTitleName, szArgs);
}

enum {
	CONNECT_LAUNCH,
	CONNECT_ATTACH,
	CONNECT_ATTACH_RUNNING,
	CONNECT_ATTACH_STOPPED,
	CONNECT_ATTACH_ASSERT,
};

typedef struct _CONN {
    int mdConnect;
    DMN_MODLOAD dmmlXbe;
    HANDLE hevtLoaded;
} CONN, *PCONN;

DWORD ConnectThread(LPVOID pvParam)
{
	/* We're going to connect to a running process.  We do so by synthesizing
	 * all of the events the debugger would normally expect to see during
	 * the course of process launch */
	int mdConnect;
	DWORD rgtid[128];
	DWORD ctid;
	DWORD itid;
	DWORD dwNotify;
	DEBUG_EVENT de;
	DMN_MODLOAD dmml;
	DMN_BREAK dmbr;
	DM_THREADSTOP dmts;
    DM_THREADINFO dmti;
	CONTEXT cr;
	HPRCX hprc;
    PDM_WALK_MODULES pdmwm;
    CONN conn;

    /* Grab our parameters */
    memcpy(&conn, pvParam, sizeof conn);
    SetEvent(conn.hevtLoaded);
    mdConnect = conn.mdConnect;

	/* Set up our notifications */
	DmStopOn(DMSTOP_CREATETHREAD | DMSTOP_FCE, TRUE);
	for(dwNotify = 1; dwNotify <= DM_NOTIFYMAX; ++dwNotify)
		DmNotify(psessNotify, dwNotify, XboxNotification);

	memset(&de, 0, sizeof de);
	/* Send the create process message.  If we're launching, we have the ID
	 * of the first thread, and if we're connecting, we'll just pick the first
	 * one in the thread list */
	ctid = sizeof(rgtid) / sizeof(DWORD);
	if(FAILED(DmGetThreadList(rgtid, &ctid)))
		ctid = rgtid[0] = 0;
	de.dwDebugEventCode = CREATE_PROCESS_DEBUG_EVENT;
	de.dwProcessId = 1;
	if(mdConnect == CONNECT_LAUNCH) {
		de.dwThreadId = g_tidBreak;
		de.u.CreateProcessInfo.lpStartAddress = g_pvThreadStart;
	} else 
		de.dwThreadId = rgtid[0];
	de.u.CreateProcessInfo.hFile = &conn.dmmlXbe;
	de.u.CreateProcessInfo.lpImageName = (PVOID)0x00010000;
	de.u.CreateProcessInfo.lpBaseOfImage = conn.dmmlXbe.BaseAddress;

    if(SUCCEEDED(DmGetThreadInfo(de.dwThreadId, &dmti)))
        de.u.CreateProcessInfo.lpThreadLocalBase = dmti.TlsBase;
    else
        de.u.CreateProcessInfo.lpThreadLocalBase = NULL;
	ResetEvent(hEventContinue);
	ProcessDebugEvent(&de);
	WaitForSingleObject(hEventCreateProcess, INFINITE);
	WaitForSingleObject(hEventContinue, INFINITE);
	hprc = HPRCFromPID(1);
	assert(hprc);
	assert(hprc->pstate & ps_connect);

	/* Notify of the created threads */
	memset(&de.u, 0, sizeof de.u);
	de.dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT;
	/* Loop over all the threads to notify of their creation.  If we're
	 * launching, we hit all of them and skip the one we already sent -- which
	 * hopefully should total zero notifications (why would any other threads
	 * exist if we've only created the first one?).  If we're not launching,
	 * we skip the first thread since we already sent that one with the
	 * process */
	for(itid = mdConnect != CONNECT_LAUNCH; itid < ctid; ++itid)
		{
		if(mdConnect != CONNECT_LAUNCH || rgtid[itid] != g_tidBreak) {
			/* For each thread, we need to wait for the DM to acknowledge
			 * creation and continuation of the thread, and only then can we
			 * mark the thread as running */
			ResetEvent(hprc->hEventCreateThread);
			ResetEvent(hEventContinue);
			de.dwThreadId = rgtid[itid];
            if(SUCCEEDED(DmGetThreadInfo(de.dwThreadId, &dmti)))
                de.u.CreateThread.lpThreadLocalBase = dmti.TlsBase;
            else
                de.u.CreateThread.lpThreadLocalBase = NULL;
			ProcessDebugEvent(&de);
			WaitForSingleObject(hprc->hEventCreateThread, INFINITE);
			WaitForSingleObject(hEventContinue, INFINITE);
			SetDebugEventThreadState(hprc, ts_running);
		}
	}

	/* Enumerate the loaded DLLs and advise */
    pdmwm = NULL;
	while(SUCCEEDED(DmWalkLoadedModules(&pdmwm, &dmml))) {
        if(dmml.BaseAddress != conn.dmmlXbe.BaseAddress) {
		    GetModLoadDebugEvent(&dmml, &de);
		    ProcessDebugEvent(&de);
        }
	}
	DmCloseLoadedModules(pdmwm);

	/* At this point, we shouldn't have any deq's pending or in the works,
	 * though we might have some continue events needing to be flushed */
	assert(!dwTidStopped);
	assert(!fDeqStopped);
	assert(!pdeqFirst);
	DequeueAllEvents(TRUE, TRUE);

	switch(mdConnect) {
	case CONNECT_LAUNCH:
		/* We've sent all appropriate messages, and we're just waiting for
		 * that breakpoint we're about to hit, so we'll continue the
		 * initial thread and let the fireworks fly */
		hprc->pstate &= ~ps_connect;
		DmContinueThread(g_tidBreak, FALSE);
		DmGo();
		break;
	case CONNECT_ATTACH:
		/* this should have been converted to something else, but we'll just
		 * treat it like connect to running */
		assert(FALSE);
		// fall through
	case CONNECT_ATTACH_RUNNING:
		/* We forced this stop, which means we're connecting to a running
		 * process.  We'll repost the breakpoint event and let it trigger
		 * everything else */
		hprc->pstate &= ~ps_connect;
		cr.ContextFlags = CONTEXT_FULL;
		if(FAILED(DmGetThreadContext(g_tidBreak, (PCONTEXT)&cr)) ||
				!(cr.ContextFlags & CONTEXT_INTEGER))
			return xosdLineNotConnected;
		dmbr.ThreadId = g_tidBreak;
		dmbr.Address = (PVOID)cr.Eip;
		XboxNotification(DM_BREAK | DM_STOPTHREAD, (DWORD)&dmbr);
		break;
	case CONNECT_ATTACH_STOPPED:
		/* We didn't force this stop, so we'll say we've finished the load,
		 * and then we'll go through every stopped thread and post a reason
		 * for its stopped state */
		ConsumeAllProcessEvents (hprc, FALSE);
		ResetEvent(hEventContinue);
		de.dwDebugEventCode = LOAD_COMPLETE_DEBUG_EVENT;
		NotifyEM( &de, HTHDXFromPIDTID(1, rgtid[0]), 0, 0L);
		WaitForSingleObject( hEventContinue, INFINITE );

		/* We're connected now, so post those debug events */
		hprc->pstate &= ~(ps_connect | ps_preStart);
		mdConnect = CONNECT_ATTACH_RUNNING;
		for(itid = 0; itid < ctid; ++itid) {
			if(SUCCEEDED(DmIsThreadStopped(rgtid[itid], &dmts))) {
				if(dmts.NotifiedReason == DM_ASSERT) {
					CONTEXT dmcr;
					/* We missed the assert text, so we'll continue past the
					 * assert with an invalid continue and force the assert
					 * to repeat */
					dmcr.ContextFlags = CONTEXT_INTEGER;
					DmGetThreadContext(rgtid[itid], &dmcr);
					dmcr.Eax = 0;
					DmSetThreadContext(rgtid[itid], &dmcr);
					DmContinueThread(rgtid[itid], FALSE);
					if(mdConnect == CONNECT_ATTACH_RUNNING)
						mdConnect = CONNECT_ATTACH_ASSERT;
				} else if(dmts.NotifiedReason != DM_NONE) {
					mdConnect = CONNECT_ATTACH_STOPPED;
					XboxNotification(dmts.NotifiedReason | DM_STOPTHREAD,
						(DWORD)&dmts.u);
					break;
				} else {
					/* We should have continued these threads */
					assert(FALSE);
				}
			}
		}
		assert(mdConnect != CONNECT_ATTACH_RUNNING);
		/* If our only stops are on asserts, we need to continue to force the
		 * assert to reappear */
		if(mdConnect == CONNECT_ATTACH_ASSERT)
			DmGo();
		break;
	}

	/* Our work here is done */
	return 0;
}

XOSD XboxConnect(int mdConnect)
{
	/* To connect to the active process, we need to suspend execution so that
	 * we can find a thread to break into -- either we find a thread that's
	 * already stopped or we halt into any available thread.  Once we set that
	 * up, we synthesize a create process event to get things rolling */
	DWORD rgtid[128];
	DWORD ctid;
	DWORD itid;
	DWORD tidStopped;
	DM_THREADSTOP dmts;
    CONN conn;
    DMN_MODLOAD dmmlTls;
    BOOL fSawXBE;
    PDM_WALK_MODULES pdmwm;
    HANDLE hthr;

	/* Suspend execution while we sort things out */
	DmStop();
	DmSendCommand(NULL, "kd disable", NULL, 0);

	/* Make sure our cleanup thread is going */
	if(!StartDmPollThread())
		return xosdUnknown;

	if(mdConnect == CONNECT_ATTACH) {
		/* Before we can do anything, we need to find a thread that's stopped
		 * on a breakpoint or exception.  We'll continue any thread we find
		 * that's not stopped for a good reason */
		ctid = sizeof rgtid / sizeof(DWORD);
		if(FAILED(DmGetThreadList(rgtid, &ctid)) || ctid == 0)
			return xosdLineNotConnected;
		tidStopped = 0;
		for(itid = 0; itid < ctid; ++itid) {
			if(SUCCEEDED(DmIsThreadStopped(rgtid[itid], &dmts)))
			{
				if(dmts.NotifiedReason == DM_NONE || dmts.NotifiedReason ==
						DM_DEBUGSTR)
					DmContinueThread(rgtid[itid], FALSE);
				else
					mdConnect = CONNECT_ATTACH_STOPPED;
			}
		}
		if(mdConnect == CONNECT_ATTACH)
		{
			XOSD xosd;
			/* No stopped thread, so we need to stop one */
			ResetEvent(hevtStartBreak);
			/* Make sure we see the first break */
			if(FAILED(DmNotify(psessNotify, DM_BREAK, AttachNotification)))
				return xosdGeneral;
			DmGo();
			DmHaltThread(0);
			if(WaitForSingleObject(hevtStartBreak, 10000) == WAIT_TIMEOUT)
				xosd = xosdUnknown;
			else
				xosd = xosdNone;
			DmNotify(psessNotify, DM_NONE, AttachNotification);
			if(xosd != xosdNone)
				return xosd;
			mdConnect = CONNECT_ATTACH_RUNNING;
		}
	}

    /* We need to make sure we can find the XBE module.  We may be running on
     * an old build that doesn't report the XBE, so we'll key off of the last
     * module in the list that has TLS if that's the case */
    pdmwm = NULL;
    fSawXBE = FALSE;
    dmmlTls.Flags = 0;
    while(SUCCEEDED(DmWalkLoadedModules(&pdmwm, &conn.dmmlXbe))) {
        if(conn.dmmlXbe.Flags & DMN_MODFLAG_XBE) {
            fSawXBE = TRUE;
            break;
        } else if(conn.dmmlXbe.Flags & DMN_MODFLAG_TLS)
            memcpy(&dmmlTls, &conn.dmmlXbe, sizeof dmmlTls);
    }
    DmCloseLoadedModules(pdmwm);
    if(!fSawXBE) {
        if(dmmlTls.Flags)
            /* Didn't see the XBE, but we did find a module with TLS.  Pretend
             * it's the XBE */
            memcpy(&conn.dmmlXbe, &dmmlTls, sizeof dmmlTls);
        else
            return xosdBadProcess;
    }

	/* Now that our state is set up, we spin off a thread to send all of the
	 * notifications through and we'll continue with startup */
    conn.hevtLoaded = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(!conn.hevtLoaded)
        return xosdOutOfMemory;
    conn.mdConnect = mdConnect;
    hthr = CreateThread(NULL, 0, ConnectThread, &conn, 0, NULL);
    if(!hthr) {
        CloseHandle(conn.hevtLoaded);
        return xosdOutOfMemory;
    }
    CloseHandle(hthr);

    /* Wait until the new thread has copied out the connection info before
     * returning */
    WaitForSingleObject(conn.hevtLoaded, INFINITE);
    CloseHandle(conn.hevtLoaded);
	return xosdNone;
}

void ProcessProgLoadCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
	XOSD xosd = xosdNone;
	HPRCX hprc1, hprcT;

    fDisconnected = FALSE;

    assert (lpdbb -> dmf == dmfProgLoad);

     /*
     * For various strange reasons the list of processes may not have
     * been completely cleared.  If not do so now
     */

    for (hprc1 = prcList; hprc1 != hprcxNull; hprc1 = hprcT) {
        hprcT = hprc1->next;

        if (hprc1->pstate & ps_dead) {
            FreeProcess( hprc1, FALSE );
        }
    }

	/* Get our first notifications set up */
	if(!hevtStartBreak)
		hevtStartBreak = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!hevtStartBreak)
		xosd = xosdUnknown;
	g_tidBreak = 0;
	if(FAILED(HrEnsureNotifier()))
		xosd = xosdLineNotConnected;
	else {
		DmNotify(psessNotify, DM_NONE, XboxNotification);
		if(FAILED(DmNotify(psessNotify, DM_EXEC, XboxNotification)))
			xosd = xosdLineNotConnected;
	}
	assert(dwExecState != DMN_EXEC_REBOOT);

	/* If we're in a pending state, then we are expected to launch the app.
	 * If we're running something right now, but it's not debuggable, then
	 * we'll treat it as pending so we can force a launch.  What we do is to
	 * set the title to run, ask for a breakpoint, and start execution.  We
	 * set ourselves up to send the create process when the first create
	 * thread arrives, so that we can get the process start address correct.
	 * Then we continue with the remainder of the normal launch process */
	if(xosd == xosdNone) {
		if(dwExecState == DMN_EXEC_PENDING || DmConnectDebugger(TRUE) ==
            XBDM_NOTDEBUGGABLE)
		{
			HRESULT hr;
			PRL prl;

			DPRINT(5, (_T("Launching the title\n")));
			UnpackLoadCmd(lpdbb->rgbVar, &prl);
			hr = HrLaunchTitle(&prl);
			if(hr == XBDM_NOSUCHFILE)
				xosd = xosdFileNotFound;
			else if(FAILED(hr))
				xosd = xosdUnknown;
			else {
				hr = DmSetInitialBreakpoint();
				if(SUCCEEDED(hr))
					hr = DmStopOn(DMSTOP_CREATETHREAD, TRUE);
				if(FAILED(hr)) {
					DPRINT(5, (_T("Can't start\n")));
					/* If we can't get our startup breakpoint, we'll start the
					 * machine but fail to launch */
					DmGo();
					xosd = xosdGeneral;
#ifndef XBOX
					goto LaunchErr;
#endif
				} else {
					ResetEvent(hevtStartBreak);
					DmNotify(psessNotify, DM_CREATETHREAD, LaunchNotification);
					DmNotify(psessNotify, DM_BREAK, LaunchNotification);
					DmNotify(psessNotify, DM_EXCEPTION, LaunchNotification);
					DmGo();
					/* We wait for the creation of the first thread, but we
					 * won't wait any longer than sixty seconds */
					WaitForSingleObject(hevtStartBreak, 60000);
					DmNotify(psessNotify, DM_NONE, LaunchNotification);
					DPRINT(5, (_T("Title launched, hit break\n")));
					if(g_tidBreak && SUCCEEDED(DmConnectDebugger(TRUE)))
						xosd = XboxConnect(CONNECT_LAUNCH);
					else {
#ifndef XBOX
						char szError[256];
LaunchErr:
						xosd = xosdGeneral;
						if(LoadString(hInstance, IDS_ERR_NOSTART, szError,
								sizeof szError))
							SendDBCError(hprc, xosd, szError);
#endif
					}
				}
			}
		} else
			xosd = XboxConnect(CONNECT_ATTACH);
	}

	/* If we succeeded, then the reply will come from the create process
	 * event.  Otherwise we need to reply right now */
	if(xosd != xosdNone)
		Reply(0, &xosd, lpdbb->hpid);
}

void ProcessSpawnOrphanCmd(HPRCX hprc, HTHDX hthd, LPDBB lpdbb)
{
	XOSD xosd;
	SOS sos;
	HRESULT hr;
	HPRCX hprc1, hprcT;

    fDisconnected = FALSE;

    if (!UnpackLoadCmd(lpdbb->rgbVar, &sos)) {
        assert(FALSE); // Can't deal with failure
    }

    assert (lpdbb -> dmf == dmfSpawnOrphan);
	assert( sos.lpso!=NULL );

    for (hprc1 = prcList; hprc1 != hprcxNull; hprc1 = hprcT) {
        hprcT = hprc1->next;

        if (hprc1->pstate & ps_dead) {
            FreeProcess( hprc1, FALSE );
        }
    }

	hr = HrLaunchTitle(&sos);
	/* We don't want to hear anything about this title */
	DmCloseNotificationSession(psessNotify);
	psessNotify = NULL;
	if(SUCCEEDED(hr))
		DmGo();
	if(hr == XBDM_NOSUCHFILE)
		xosd = xosdFileNotFound;
	else if(FAILED(hr))
		xosd = xosdUnknown;
	else
		xosd = xosdNone;

    if (xosd == xosdNone) {
        sos.lpso -> dwPid = 1;
        LoadString (hInstance, IDS_EXECUTE_OK, sos.lpso -> rgchErr, sizeof (sos.lpso -> rgchErr));
    } else {
        sos.lpso -> dwPid = 0;
        LoadString (hInstance, IDS_EXECUTE_FAILED, sos.lpso -> rgchErr, sizeof (sos.lpso -> rgchErr));
    }

    memcpy (LpDmMsg -> rgb, sos.lpso, sizeof (SPAWNORPHAN));
    LpDmMsg->xosdRet = xosd;
    Reply (sizeof (SPAWNORPHAN), LpDmMsg, lpdbb -> hpid);
}
	
void InitDEQ(void)
{
	hevtDEQ = CreateEvent(NULL, FALSE, FALSE, NULL);
	InitializeCriticalSection(&csDEQ);
	pdeqFirst = NULL;
}

void PostDEQ(DEBUG_EVENT *pde, BOOL fStopThread)
{
	DEQ *pdeq;
	DEQ **ppdeq;
	char *pch;

	EnterCriticalSection(&csDEQ);
	pdeq = MHAlloc(sizeof(DEQ));
	pdeq->de = *pde;
	/* We may have some supplemental data that we need to stick in here as
	 * well */
	switch(pde->dwDebugEventCode)
	{
	case LOAD_DLL_DEBUG_EVENT:
		pdeq->dmml = *(PDMN_MODLOAD)pde->u.LoadDll.hFile;
		break;
	case CREATE_PROCESS_DEBUG_EVENT:
		if(pde->u.CreateProcessInfo.hFile)
			pdeq->dmml = *(PDMN_MODLOAD)pde->u.CreateProcessInfo.hFile;
		break;
	}
	pdeq->fNeedsContinue = fStopThread;
	pdeq->pdeqNext = NULL;
	for(ppdeq = &pdeqFirst; *ppdeq; ppdeq = &(*ppdeq)->pdeqNext);
	*ppdeq = pdeq;
	if(!fDeqStopped)
		SetEvent(hevtDEQ);
	LeaveCriticalSection(&csDEQ);
}

BOOL WaitDEQ(DEQ *pdeqOut, DWORD dwWaitTime)
{
	DEQ *pdeq;

	do {
		if(WaitForSingleObject(hevtDEQ, dwWaitTime) == WAIT_TIMEOUT)
			return FALSE;
		EnterCriticalSection(&csDEQ);
		if(fDeqStopped)
			pdeq = NULL;
		else if(pdeq = pdeqFirst) {
			fDeqStopped = TRUE;
			pdeqFirst = pdeq->pdeqNext;
		}
		LeaveCriticalSection(&csDEQ);
	} while(!pdeq);

	if(pdeq->fNeedsContinue)
		dwTidStopped = pdeq->de.dwThreadId;

	*pdeqOut = *pdeq;
	MHFree(pdeq);
	/* May need to fix up the resulting debug event */
	switch(pdeqOut->de.dwDebugEventCode)
	{
	case LOAD_DLL_DEBUG_EVENT:
		pdeqOut->de.u.LoadDll.hFile = (HANDLE)&pdeqOut->dmml;
		break;
	case CREATE_PROCESS_DEBUG_EVENT:
		if(pdeqOut->de.u.CreateProcessInfo.hFile)
			pdeqOut->de.u.CreateProcessInfo.hFile = (HANDLE)&pdeqOut->dmml;
		break;
	}

	return TRUE;
}

DWORD HandleBreakNotification(DWORD dwExceptCode, BOOL fStop, PDMN_BREAK pdmbr)
{
	DEBUG_EVENT de;
	EXCEPTION_RECORD *per;

	memset(&de, 0, sizeof de);
	de.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
	de.dwProcessId = 1;
	de.dwThreadId = pdmbr->ThreadId;
	de.u.Exception.dwFirstChance = 0;
	per = &de.u.Exception.ExceptionRecord;
	per->ExceptionCode = dwExceptCode;
	per->ExceptionAddress = pdmbr->Address;
	if(dwExceptCode == STATUS_BREAKPOINT)
	{
		per->NumberParameters = 1;
		per->ExceptionInformation[0] = BREAKPOINT_BREAK;
	}
	else
		per->NumberParameters = 0;
	PostDEQ(&de, fStop);
	return 0;
}

DWORD HandleDataBreakNotification(BOOL fStop, PDMN_DATABREAK pdmdb)
{
	DEBUG_EVENT de;
	EXCEPTION_RECORD *per;

	memset(&de, 0, sizeof de);
	de.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
	de.dwProcessId = 1;
	de.dwThreadId = pdmdb->ThreadId;
	de.u.Exception.dwFirstChance = 0;
	per = &de.u.Exception.ExceptionRecord;
	per->ExceptionCode = STATUS_SINGLE_STEP;
	per->ExceptionAddress = pdmdb->Address;
	/* We'll fill in the data address and break type as parameters */
	per->NumberParameters = 2;
	per->ExceptionInformation[0] = (DWORD)pdmdb->DataAddress;
	per->ExceptionInformation[1] = pdmdb->BreakType;
	PostDEQ(&de, fStop);
	return 0;
}

void GetModLoadDebugEvent(PDMN_MODLOAD pdmml, DEBUG_EVENT *pde)
{
	HPRCX hprc;

	memset(pde, 0, sizeof *pde);
	pde->dwDebugEventCode = LOAD_DLL_DEBUG_EVENT;
	pde->dwProcessId = 1;
	/* We'll take any valid thread ID */
	hprc = HPRCFromPID(1);
	if(hprc && hprc->hthdChild)
		pde->dwThreadId = hprc->hthdChild->tid;
	pde->u.LoadDll.hFile = (HANDLE)pdmml; // for LoadDll's use
	pde->u.LoadDll.lpBaseOfDll = pdmml->BaseAddress;
	pde->u.LoadDll.lpImageName = (PVOID)0x00010000;
}

DWORD HandleModLoadNotification(PDMN_MODLOAD pdmml)
{
	DEBUG_EVENT de;

	GetModLoadDebugEvent(pdmml, &de);
	PostDEQ(&de, FALSE);
	return 0;
}

DWORD HandleDebugStrNotification(BOOL fStop, PDMN_DEBUGSTR pdmds)
{
	DEBUG_EVENT de;

	memset(&de, 0, sizeof de);
	de.dwDebugEventCode = OUTPUT_DEBUG_STRING_EVENT;
	de.dwProcessId = 1;
	de.dwThreadId = pdmds->ThreadId;
	de.u.DebugString.nDebugStringLength = (WORD)pdmds->Length;
	/* We need to take responsibility for allocating a copy of the string
	 * data */
	de.u.DebugString.lpDebugStringData = MHAlloc(pdmds->Length + 1);
	memcpy(de.u.DebugString.lpDebugStringData, pdmds->String, pdmds->Length);
	PostDEQ(&de, fStop);
	return 0;
}

DWORD HandleExceptionNotification(BOOL fStop, PDMN_EXCEPTION pdme)
{
	EXCEPTION_RECORD *per;
	DEBUG_EVENT de;

	memset(&de, 0, sizeof de);
	de.dwProcessId = 1;
	de.dwThreadId = pdme->ThreadId;
	de.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
	de.u.Exception.dwFirstChance = pdme->Flags & DM_EXCEPT_FIRSTCHANCE;
	per = &de.u.Exception.ExceptionRecord;
	per->ExceptionCode = pdme->Code;
	per->ExceptionAddress = pdme->Address;
	if(pdme->Flags & DM_EXCEPT_NONCONTINUABLE)
		per->ExceptionFlags |= EXCEPTION_NONCONTINUABLE;
    if(pdme->Code == EXCEPTION_ACCESS_VIOLATION) {
		per->NumberParameters = 2;
		per->ExceptionInformation[0] = pdme->Information[0];
		per->ExceptionInformation[1] = pdme->Information[1];
	}
	PostDEQ(&de, fStop);
	return 0;
}

DWORD HandleFiberNotification(BOOL fStop, PDMN_FIBER pdmfib)
{
    HPRCX hprc;
    hprc = HPRCFromPID(1);

    /* Only post the fiber event if we're supporting fibers */
    if(hprc->fUseFbrs) {
        EXCEPTION_RECORD *per;
	    DEBUG_EVENT de;

	    memset(&de, 0, sizeof de);
	    de.dwProcessId = 1;
        /* We'll put in any valid thread id */
	    de.dwThreadId = hprc->hthdChild->tid;
	    de.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
	    de.u.Exception.dwFirstChance = 0;
	    per = &de.u.Exception.ExceptionRecord;
	    per->ExceptionCode = EXCEPTION_FIBER_DEBUG;
	    per->ExceptionAddress = NULL;
        if(pdmfib->Create) {
            per->NumberParameters = 3;
            per->ExceptionInformation[0] = ecreate_fiber;
            per->ExceptionInformation[2] = (ULONG)pdmfib->StartAddress;
        } else {
            per->NumberParameters = 2;
            per->ExceptionInformation[0] = edelete_fiber;
        }
        per->ExceptionInformation[1] = pdmfib->FiberId;
	    PostDEQ(&de, fStop);
    }
	return 0;
}

DWORD HandleCreateThreadNotification(BOOL fStop, PDMN_CREATETHREAD pdmct)
{
	DEBUG_EVENT de;
    DM_THREADINFO dmti;

	memset(&de, 0, sizeof de);
	de.dwDebugEventCode = CREATE_THREAD_DEBUG_EVENT;
	de.dwProcessId = 1;
	de.dwThreadId = pdmct->ThreadId;
	de.u.CreateThread.lpStartAddress = pdmct->StartAddress;
    if(SUCCEEDED(DmGetThreadInfo(de.dwThreadId, &dmti)))
        de.u.CreateThread.lpThreadLocalBase = dmti.TlsBase;
    else
        de.u.CreateThread.lpThreadLocalBase = NULL;
    PostDEQ(&de, fStop);
	return 0;
}

DWORD HandleDestroyThreadNotification(DWORD tid)
{
	DEBUG_EVENT de;

	memset(&de, 0, sizeof de);
	de.dwDebugEventCode = EXIT_THREAD_DEBUG_EVENT;
	de.dwProcessId = 1;
	de.dwThreadId = tid;
	PostDEQ(&de, FALSE);
	return 0;
}

DWORD HandleAssertNotification(BOOL fStop, PDMN_DEBUGSTR pdmds)
{
	char chRet;

	chRet = XboxAssert(pdmds);
	if(fStop) {
		/* We stopped for this assert, so we need to set the reply and
		 * continue the thread */
		CONTEXT dmcr;
		dmcr.ContextFlags = CONTEXT_INTEGER;
		DmGetThreadContext(pdmds->ThreadId, &dmcr);
		dmcr.Eax = chRet;
		DmSetThreadContext(pdmds->ThreadId, &dmcr);
		DmContinueThread(pdmds->ThreadId, FALSE);
		DmGo();
	}
	return 0;
}

DWORD HandleRipNotification(BOOL fStop, PDMN_DEBUGSTR pdmds)
{
	DEBUG_EVENT de;
	LPSTR psz;

	memset(&de, 0, sizeof de);
	de.dwProcessId = 1;
	de.dwThreadId = pdmds->ThreadId;
	de.dwDebugEventCode = RIP_EVENT;
	/* We need to keep a copy of the debugstr, if we have one */
	if(pdmds->String) {
		psz = MHAlloc(pdmds->Length + 1);
		memcpy(psz, pdmds->String, pdmds->Length);
		psz[pdmds->Length] = 0;
	} else
		psz = NULL;
	de.u.RipInfo.dwError = (DWORD)psz;
	PostDEQ(&de, fStop);
	return 0;
}

DWORD XboxNotification(DWORD dwMsg, DWORD dwParam)
{
	BOOL fStop = dwMsg & DM_STOPTHREAD;
    BOOL fSetEvent;

	switch(dwMsg & DM_NOTIFICATIONMASK)
	{
	case DM_ASSERT:
		return HandleAssertNotification(fStop, (PDMN_DEBUGSTR)dwParam);
	case DM_BREAK:
		return HandleBreakNotification(STATUS_BREAKPOINT, fStop,
			(PDMN_BREAK)dwParam);
	case DM_SINGLESTEP:
		return HandleBreakNotification(STATUS_SINGLE_STEP, fStop,
			(PDMN_BREAK)dwParam);
	case DM_DATABREAK:
		return HandleDataBreakNotification(fStop, (PDMN_DATABREAK)dwParam);
	case DM_MODLOAD:
		return HandleModLoadNotification((PDMN_MODLOAD)dwParam);
	case DM_RIP:
		return HandleRipNotification(fStop, (PDMN_DEBUGSTR)dwParam);
	case DM_DEBUGSTR:
		return HandleDebugStrNotification(fStop, (PDMN_DEBUGSTR)dwParam);
	case DM_EXCEPTION:
		return HandleExceptionNotification(fStop, (PDMN_EXCEPTION)dwParam);
	case DM_CREATETHREAD:
		return HandleCreateThreadNotification(fStop,
			(PDMN_CREATETHREAD)dwParam);
    case DM_FIBER:
        return HandleFiberNotification(fStop, (PDMN_FIBER)dwParam);
	case DM_DESTROYTHREAD:
		if(fStop) {
			DmContinueThread(dwParam, FALSE);
			DmGo();
		}
		return HandleDestroyThreadNotification(dwParam);
	case DM_EXEC:
        fSetEvent = FALSE;
		if(hevtReboot) {
			if(dwExecState == DMN_EXEC_REBOOT && dwParam != DMN_EXEC_REBOOT)
				fSetEvent = TRUE;
		}
#ifndef XBOX
		else if(dwExecState == DMN_EXEC_REBOOT) {
			DEBUG_EVENT de;
			HPRCX hprc;
			PDMN_SESSION psess;

			/* Need to send process termination */
			memset(&de, 0, sizeof de);
			de.dwDebugEventCode = EXIT_PROCESS_DEBUG_EVENT;
			de.dwProcessId = 1;
			hprc = HPRCFromPID(1);
			if(hprc && hprc->hthdChild)
				de.dwThreadId = hprc->hthdChild->tid;
			else
				de.dwThreadId = 0;
			PostDEQ(&de, FALSE);
			/* Don't allow any more notifications until the process restarts */
			psess = psessNotify;
			psessNotify = NULL;
			DmCloseNotificationSession(psess);
		}
#endif
		dwExecState = dwParam;
        if(fSetEvent)
            SetEvent(hevtReboot);
		break;
	}

	//DequeueAllEvents(TRUE, FALSE);
	return 0;
}

void ProcessSpecialEvent(DEBUG_EVENT *pde, HTHDX hthd) { _asm int 3 }

VOID
ProcessFiberEvent(
    DEBUG_EVENT* pde,
    HTHDX        hthd
    )
{
    HPRCX hprc = hthd->hprc;
    HFBRX hfbr;
    EXCEPTION_DEBUG_INFO *dbginfo = (EXCEPTION_DEBUG_INFO *) &(pde->u);
    EFBR    efbr = dbginfo->ExceptionRecord.ExceptionInformation[0];

    // There are only two cases - Create Fiber and Delete Fiber

    switch(efbr) {
    case ecreate_fiber:

        hfbr = (HFBRX)MHAlloc(sizeof(HFBRXSTRUCT));
        memset(hfbr, 0, sizeof(*hfbr));

        // Add to the process's list of fibers
        hfbr->next = hprc->FbrLst;
        hprc->FbrLst = hfbr;

        // Grab the start and context pointers for the fiber

        hfbr->fbrstrt = (LPVOID)dbginfo->ExceptionRecord.ExceptionInformation[1];
        hfbr->fbrcntx = (LPVOID)dbginfo->ExceptionRecord.ExceptionInformation[2];

        // Have the debuggee continue after the exception
        hthd->tstate &= ~(ts_stopped|ts_first|ts_second);
        hthd->tstate |= ts_running;
        AddQueue( QT_CONTINUE_DEBUG_EVENT,
                  hthd->hprc->pid,
                  hthd->tid,
                DBG_CONTINUE,
                0);
        break;
    case edelete_fiber:
        {
            HFBRX  prevhfbr;
            LPVOID fbrstrt = (LPVOID)dbginfo->ExceptionRecord.ExceptionInformation[1];
            LPVOID fbrcntx = (LPVOID)dbginfo->ExceptionRecord.ExceptionInformation[2];

            hfbr = hprc->FbrLst;
            if((hfbr->fbrstrt == fbrstrt) &&
                    (hfbr->fbrcntx == fbrcntx)){
                    hprc->FbrLst = hfbr->next;
                    MHFree(hfbr);
            } else {
                prevhfbr = hfbr;
                hfbr = hfbr->next;
                while(hfbr){
                    if((hfbr->fbrstrt == fbrstrt) &&
                        (hfbr->fbrcntx == fbrcntx)){
                        prevhfbr->next = hfbr->next;
                        MHFree(hfbr);
                        break;
                    } else {
                        prevhfbr = hfbr;
                        hfbr = hfbr->next;
                    }
                }
            }
            // Have the debuggee continue after the exception
        hthd->tstate &= ~(ts_stopped|ts_first|ts_second);
        hthd->tstate |= ts_running;
        AddQueue( QT_CONTINUE_DEBUG_EVENT,
                  hthd->hprc->pid,
                  hthd->tid,
                DBG_CONTINUE,
                0);
        break;
        }
    }
}

void ProcessOleEvent(DEBUG_EVENT *pde, HTHDX hthd) { _asm int 3 }
void ProecssEntryPointEvent(DEBUG_EVENT *pde, HTHDX hthd) { _asm int 3 }
void ProcessSegmentLoadEvent(DEBUG_EVENT *pde, HTHDX hthd) { _asm int 3 }
void ProcessEntryPointEvent(DEBUG_EVENT *pde, HTHDX hthd) { _asm int 3 }

BOOL DequeueAllEvents(BOOL fForce, BOOL fConsume)
{
    LPCQUEUE    lpcq;
    BOOL        fDid = FALSE;
    HPRCX       hprc;
    HTHDX       hthd;

    EnterCriticalSection(&csContinueQueue);

    while ( lpcq=lpcqFirst ) {
        hprc = HPRCFromPID (lpcq->pid);

        if (hprc) {
            for (hthd = hprc->hthdChild; hthd; hthd = hthd->next) {
                if (hthd->fContextDirty) {
                    DbgSetThreadContext(hthd, &hthd->ctx);

                    hthd->fContextDirty = FALSE;
                }
                hthd->fWowEvent = FALSE;
            }

            //assert (hprc->lastTidDebugEvent == lpcq->tid);
			/* Since we continue to process debug events which don't actually
			 * stop execution, it's possible we've got a queued continue event
			 * for an older debug event.  If so, we skip it */
			if(hprc->lastTidDebugEvent != lpcq->tid) {
				assert(lpcq->tid != dwTidStopped);
				goto skipit;
			}
        }

        //UserTimerGo();

#if 0
        if (hprc) {
            assert (!hprc->fSoftBroken);
            for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {
                assert (!hthd->fSoftBroken);
            }
        }
#endif


		EnterCriticalSection(&csDEQ);
		if(lpcq->tid == dwTidStopped) {
			fDid = SUCCEEDED(DmContinueThread(lpcq->tid, lpcq->fException));
			/* Make sure everything else is running too */
			DmGo();
			dwTidStopped = 0;
			assert(fDeqStopped);
		} else
			fDid = fDeqStopped;
		if(fDeqStopped && pdeqFirst)
			SetEvent(hevtDEQ);
		fDeqStopped = FALSE;
		/* The only way we should be able to get here without doing any
		 * work is if we're creating the process */
		assert(fDid || !hprc || (hprc->pstate & ps_connect));
		LeaveCriticalSection(&csDEQ);

skipit:
        lpcqFirst = lpcq->next;
        if (lpcqFirst == NULL) {
            lpcqLast = NULL;
        }

        lpcq->next = lpcqFree;
        lpcqFree   = lpcq;
    }

    LeaveCriticalSection(&csContinueQueue);
    return fDid;
}                               /* DequeueContinueDebugEvents() */

void InitEventQueue(void)
{
    int n;
    int i;

    InitializeCriticalSection(&csContinueQueue);

    n = sizeof(cqueue) / sizeof(CQUEUE);
    for (i = 0; i < n-1; i++) {
        cqueue[i].next = &cqueue[i+1];
    }
    cqueue[n-1].next = NULL;
    lpcqFree = &cqueue[0];
    lpcqFirst = NULL;
    lpcqLast = NULL;
}


void QueueContinueDebugEvent(DWORD dwPid, DWORD dwTid, BOOL fException)
{
    LPCQUEUE lpcq;

    EnterCriticalSection(&csContinueQueue);

    lpcq = lpcqFree;
    assert(lpcq);

    lpcqFree = lpcq->next;

    lpcq->next = NULL;
    if (lpcqLast) {
        lpcqLast->next = lpcq;
    }
    lpcqLast = lpcq;

    if (!lpcqFirst) {
        lpcqFirst = lpcq;
    }

    lpcq->pid = dwPid;
    lpcq->tid = dwTid;
    lpcq->fException = fException;

    LeaveCriticalSection(&csContinueQueue);

    return;
}                               /* QueueContinueDebugEvent() */

void AddQueue (DWORD dwType, DWORD dwPid, DWORD dwTid, DWORD dwData,
	DWORD dwLen)
{
    HPRCX   hprc = HPRCFromPID (dwPid);

    assert (hprc);

    switch (dwType) {
        case QT_CONTINUE_DEBUG_EVENT:
        case QT_TRACE_DEBUG_EVENT:

            //
            // Another hack to the DM: this one's for soft async stop
            // support.
            //

#if 0
            if (hprc) {

                if (hprc->fSoftBroken) {
                    HTHDX   hthd = NULL;

                    hprc->fSoftBroken = FALSE;

                    for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {
                        if (hthd->fSoftBroken) {
                            hthd->fSoftBroken = FALSE;
                            VERIFY (ResumeThread (hthd->rwHand) != -1);
                        }
                    }

                    // successfully soft broke, so bailout

                    return;
                }
            }
#endif

            //
            // The thread id MUST BE hprc->lastTidDebugEvent.  Also, we
            // cannot necessarily find this during the dequeue because
            // at the time of dequeue the process structure may be gone.
            //

            if (hprc) {
				HTHDX hthd;
                dwTid = hprc->lastTidDebugEvent;
				/* We're in trouble if we try to continue this thread while 
				 * it's still marked stopped */
				hthd = HTHDXFromPIDTID(dwPid, dwTid);
				/* It is actually possible to get here if we have no hthd --
				 * in that case, we've gotten a continue in response to a
				 * thread terminate, and we're only going through the motions
				 * to keep the deq alive */
				assert(!hthd || !(hthd->tstate & ts_stopped));
            }

			assert(dwData == DBG_CONTINUE ||
				dwData == DBG_EXCEPTION_NOT_HANDLED);
#if 0
            if (!CrashDump)
#endif
			{
                QueueContinueDebugEvent (dwPid, dwTid,
					dwData == DBG_EXCEPTION_NOT_HANDLED);
            }
            break;

        case QT_RELOAD_MODULES:
        case QT_REBOOT:
        case QT_CRASH:
        case QT_RESYNC:
            assert(!"Unsupported usermode QType in AddQueue.");
            break;

        case QT_DEBUGSTRING:
            assert(!"Is this a bad idea?");
            DMPrintShellMsg( "%s", (LPSTR)dwData );
            free((LPSTR)dwData);
            break;

    }

    if (dwType == QT_CONTINUE_DEBUG_EVENT) {
        SetEvent( hEventContinue );
    }

    return;
}

void UserTimerStop(void)
{
}

VOID
RemoveFiberList(
    HPRCX hprc
    )
{
    HFBRX hfbr = hprc->FbrLst;
    HFBRX next = NULL;
    while(hfbr){
        next = hfbr->next;
        MHFree(hfbr);
        hfbr = next;
    }
    hprc->FbrLst = NULL;
}

BOOL
SetupDebugRegister(
    HTHDX       hthd,
    int         Register,
    int         DataSize,
    DWORD       DataAddr,
    DWORD       BpType
    )
{
	DWORD dwType;
	
	assert(Register >= 0 && Register < 4);

	ClearDebugRegister(hthd, Register);

	switch(BpType) {
    case bptpDataR:
        dwType = DMBREAK_READWRITE;
        break;

    case bptpDataW:
    case bptpDataC:
        dwType = DMBREAK_WRITE;
        break;

    case bptpDataExec:
        dwType = DMBREAK_EXECUTE;
        break;

    default:
        assert(!"Invalid BpType!!");
        return FALSE;
	}

	if(SUCCEEDED(DmSetDataBreakpoint((PVOID)DataAddr, dwType, DataSize))) {
		rgbDataBreak[Register][0] = (BYTE)dwType;
		rgbDataBreak[Register][1] = (BYTE)DataSize;
		rgpvDataBreak[Register] = (PVOID)DataAddr;
	}
	return rgbDataBreak[Register][0] != DMBREAK_NONE;
}

VOID
ClearDebugRegister(
    HTHDX   hthd,
    int     Register
    )
{
	assert(Register >= 0 && Register < 4);

	if(rgbDataBreak[Register][0] != DMBREAK_NONE) {
		PVOID pv;
		int i;

		rgbDataBreak[Register][0] = DMBREAK_NONE;
		pv = rgpvDataBreak[Register];

		/* Clear this one out first */
		DmSetDataBreakpoint(pv, DMBREAK_NONE, 0);

		for(i = 0; i < 4; ++i) {
			if(rgbDataBreak[i][0] != DMBREAK_NONE && rgpvDataBreak[i] == pv &&
					FAILED(DmSetDataBreakpoint(pv, rgbDataBreak[i][0],
					rgbDataBreak[i][1])))
				rgbDataBreak[i][0] = DMBREAK_NONE;
		}
	}
}

VOID
ClearAllDebugRegisters(
	HPRCX hprc
	)
{
	int i;

	for(i = 0; i < 4; ++i)
		ClearDebugRegister(NULL, i);
}

BOOL
DecodeSingleStepEvent(
    HTHDX           hthd,
    DEBUG_EVENT*    de,
    PDWORD          eventCode,
    PDWORD          subClass
    )
{
	/* We've encoded the data breakpoint information in the debug event, if
	 * it was a data breakpoint.  But GetWalkBPFromBits wants the actual dr6
	 * value, so if this is truly a data breakpoint, we'll walk our list and
	 * set the appropriate register bit if we find one */
	PEXCEPTION_RECORD per = &de->u.Exception.ExceptionRecord;

	if(per->NumberParameters) {
		/* A data breakpoint */
		int i;
		
		for(i = 0; i < 4; ++i) {
			if((DWORD)rgpvDataBreak[i] == per->ExceptionInformation[0] &&
				rgbDataBreak[i][0] == per->ExceptionInformation[1])
			{
				DWORD dwDr6 = 1 << i;
				PBREAKPOINT bp;
				 
				bp = GetWalkBPFromBits (hthd, dwDr6);

				if (bp && CheckDataBP (hthd, bp)) {
					de->dwDebugEventCode = *eventCode = BREAKPOINT_DEBUG_EVENT;
					*subClass = (DWORD) bp;
					per->ExceptionCode = *subClass;
				}
			}
		}
	}
	return TRUE;
}

DWORD ExecNotify(DWORD dw, DWORD dwState)
{
    dwExecState = dwState;
    return 0;
}

XOSD AttachOrReboot(void)
{
    XOSD xosd = xosdNone;
    HRESULT hr;

    if(FAILED(HrEnsureNotifier()))
        return xosdLineNotConnected;

    /* At init, we clear everything out */
    DmNotify(psessNotify, DM_NONE, NULL);
    
    /* Get the current execution state */
    DmNotify(psessNotify, DM_EXEC, ExecNotify);
    DmNotify(psessNotify, DM_NONE, NULL);

    /* If we're running an active process, we need to ask whether we should
     * attach to it or launch a new one */
    if((dwExecState == DMN_EXEC_START || dwExecState == DMN_EXEC_STOP) &&
        SUCCEEDED(DmConnectDebugger(TRUE)))
    {
        char szMsg[1024];
        char szTitle[256];
        char sz[512];
        DM_XBE xbe;

        xbe.LaunchPath[0] = 0;
        if(FAILED(DmGetXbeInfo(NULL, &xbe)))
            LoadString(hInstance, IDS_UNKNOWN_XBE, xbe.LaunchPath,
                sizeof xbe.LaunchPath);

        if(LoadString(hInstance, IDS_ATTACH, sz, sizeof sz)) {
            sprintf(szMsg, sz, xbe.LaunchPath);
            szTitle[0] = 0;
            LoadString(hInstance, IDS_XBOX_TITLE, szTitle, sizeof szTitle);
            if(MessageBox(NULL, szMsg, szTitle, MB_YESNO) == IDYES) {
                /* Need to reboot.  We need to wait for the reboot to
                 * complete */
                assert(!hevtReboot);
		        hevtReboot = CreateEvent(NULL, FALSE, FALSE, NULL);
		        hr = DmNotify(psessNotify, DM_EXEC, XboxNotification);
		        if(SUCCEEDED(hr)) {
			        DmReboot(DMBOOT_WAIT | DMBOOT_WARM);
			        if(WaitForSingleObject(hevtReboot, 120000) == WAIT_TIMEOUT)
				        xosd = xosdLineNotConnected;
			        else {
				        assert(dwExecState == DMN_EXEC_PENDING);
			        }
		        }
		        CloseHandle(hevtReboot);
		        hevtReboot = NULL;
                DmNotify(psessNotify, DM_NONE, NULL);
            }
        }
    }

    return xosd;
}
