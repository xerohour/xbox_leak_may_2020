#include "precomp.h"
#pragma hdrstop

extern DMTLFUNCTYPE DmTlFunc;

extern DEBUG_EVENT  falseSSEvent;
extern METHOD       EMNotifyMethod;

extern BYTE abEMReplyBuf[];
extern HINSTANCE hInstance; // The DM DLLs hInstance

extern CRITICAL_SECTION csThreadProcList;

BOOL
IsPassingException(
    HPRCX   hprc
    )
/*++

Routine Description:

    TRUE if there is an exception and we are passing it on to the app.  This
    routine looks at the exception status of the thread we got the debug event
    on.  It doesn't make sense to look at any other thread.

--*/
{
    HTHDX   hthd = HTHDXFromPIDTID (hprc->pid, hprc->lastTidDebugEvent);

	if (hthd == NULL) {
		return FALSE;
	}
	
    return ((hthd->tstate & (ts_first | ts_second)) && !hthd->fExceptionHandled);
}

void
ThreadContinueDebugEventEx(
    HTHDX   hthd,
    ULONG   QtContinue
    )
/*++

Routine Description:

    This function marks the thread as running again and queues the correct
    continue event (depending on if we are processing exceptions or not).

--*/
{
    ULONG   ContinueCmd;
    HTHDX   hthdT;

    if (IsPassingException (hthd->hprc)) {
        ContinueCmd = (ULONG)DBG_EXCEPTION_NOT_HANDLED;
    } else {
        ContinueCmd = (ULONG)DBG_CONTINUE;
    }

    SetDebugEventThreadState (hthd->hprc, ts_running);

    for (hthdT = hthd->hprc->hthdChild; hthdT; hthdT = hthdT->next)
        hthdT->fExceptionHandled = FALSE;

    AddQueue (QtContinue,
              hthd->hprc->pid,
              hthd->tid,
              ContinueCmd,
              0);
}


void
ThreadContinueDebugEvent(
    HTHDX   hthd
    )
{
    ThreadContinueDebugEventEx (hthd, QT_CONTINUE_DEBUG_EVENT);
}


VOID
SendDBCErrorStep(
    HPRCX hprc
    )
/*++

Routine Description:

    This function notifies the user when an invalid step command is
    attempted

Arguments:

    hprc        - Supplies the thread handle to be stepped.

Return Value:

    None.

--*/
{
    char buf[1000];
#ifndef XBOX //LOCBUG
    if (!LoadString(hInstance, IDS_CANT_TRACE, buf, sizeof(buf))) {
        assert(FALSE);
    }
#else
	strcpy(buf, "can't trace");
#endif

    SendDBCError(hprc, xosdCannotStep, buf);
} // SendDBCErrorStep



VOID
SingleStep(
    HTHDX hthd,
    METHOD* notify,
    BOOL stopOnBP,
    BOOL fInFuncEval
    )
/*++

Routine Description:

    This function is used to do a single step operation on the specified
    thread.

Arguments:

    hthd        - Supplies the thread handle to be stepped.

    notify      -

    stopOnBp    - Supplies TRUE if a bp at current PC should cause a stop

    fInFuncEval - Supplies TRUE if called by the fucntion evaluation code

Return Value:

    None.

--*/
{
    ReturnStepEx( hthd, notify, stopOnBP, fInFuncEval, NULL, NULL, TRUE, TRUE);
}


VOID
SingleStepEx(
    HTHDX hthd,
    METHOD* notify,
    BOOL stopOnBP,
    BOOL fInFuncEval,
    BOOL fDoContinue
    )
/*++

Routine Description:

    This function is used to do a single step operation on the specified
    thread.

Arguments:

    hthd        - Supplies the thread handle to be stepped.

    notify      -

    stopOnBp    - Supplies TRUE if a bp at current PC should cause a stop

    fInFuncEval - Supplies TRUE if called by the fucntion evaluation code

    fDoContinue - Supplies TRUE if thread should be continued

Return Value:

    None.

--*/
{
    ReturnStepEx( hthd, notify, stopOnBP, fInFuncEval, NULL, NULL, fDoContinue,
                  TRUE);
}


VOID
ReturnStep(
    HTHDX hthd,
    METHOD* notify,
    BOOL stopOnBP,
    BOOL fInFuncEval,
    LPADDR addrRA,
    LPADDR addrStack
    )
/*++

Routine Description:

    This function is used to do a single step operation on the specified
    thread.

Arguments:

    hthd        - Supplies the thread handle to be stepped.

    notify      -

    stopOnBp    - Supplies TRUE if a bp at current PC should cause a stop

    fInFuncEval - Supplies TRUE if called by the fucntion evaluation code

    addrRA      -

    addrStack    -

Return Value:

    None.

--*/
{
    ReturnStepEx( hthd, notify, stopOnBP, fInFuncEval, addrRA, addrStack, TRUE, TRUE);
}


UOFFSET
GetReturnDestination(
    HTHDX   hthd
    )
/*++

Routine Description:

    Gets the destination of the return address from the current PC.

Arguments:


Return Value:


--*/
{
    ADDR    CurrentAddress;
    UOFFSET ReturnAddress   = 0;


    AddrFromHthdx (&CurrentAddress, hthd);

    assert (IsRet (hthd, &CurrentAddress));

    if (IsRet (hthd, &CurrentAddress)) {
#ifdef TARGET_i386

        ULONG   cb;

        DbgReadMemory (hthd->hprc,
                       (LPCVOID) STACK_POINTER (hthd),
                       &ReturnAddress,
                       sizeof (ReturnAddress),
                       &cb
                       );

#else

        ReturnAddress = GetNextOffset (hthd, FALSE);

#endif
    }

    return ReturnAddress;
}



VOID
ReturnStepEx(
    HTHDX hthd,
    METHOD* notify,
    BOOL stopOnBP,
    BOOL fInFuncEval,
    LPADDR addrRA,
    LPADDR addrStack,
    BOOL fDoContinue,
    BOOL fCheckOrpc
    )
/*++

Routine Description:

    This function is used to do a single step operation on the specified
    thread.

Arguments:

    hthd        - Supplies the thread handle to be stepped.

    notify      -

    stopOnBp    - Supplies TRUE if a bp at current PC should cause a stop

    fInFuncEval - Supplies TRUE if called by the fucntion evaluation code

    addrRA      - Supplies address to step to

    addrStack    - Supplies

    fDoContinue - Supplies TRUE if thread should be continued now

    fCheckOrpc  - Supplies TRUE if we should check for ORPC sections (and do
                  the appropiate thing if found), and FALSE to ignore ORPC
                  sections.

Return Value:

    None.

--*/
{
    ADDR                currAddr;
    ADDR                currAddrActual;
    int                 lpf = 0;
    BREAKPOINT*         bp;
    ULONG               ContinueCmd;
	BOOL                fTraceOn = TRUE;

#if 0
    //
    //  If we are stepping into an ORPC section, CheckAndSetup ... will take
    //  care of everything for us.  Just return.
    //

    if (fCheckOrpc && CheckAndSetupForOrpcSection (hthd)) {
        return;
    }
    
#endif

    //
    //  Get the current IP of the thread
    //

    AddrFromHthdx(&currAddr, hthd);
    currAddrActual = currAddr;

    bp = AtBP(hthd);

    if (!stopOnBP && !bp) {
        bp = FindBP(hthd->hprc, hthd, bptpExec, (BPNS)-1, &currAddr, FALSE);
        SetBPFlag(hthd, bp);
    }

    //
    //  Check if we are on a BP
    //
    if (bp == EMBEDDED_BP) {

        DPRINT(3, ("-- At embedded BP, skipping it.\n"));

        //
        //  If it isnt a BP we set then just increment past it
        //  & pretend that a single step actually took place
        //


        ClearBPFlag(hthd);
        hthd->fIsCallDone = FALSE;
        if (addrRA == NULL) {
            IncrementIP(hthd);
            NotifyEM(&falseSSEvent, hthd, 0, (LPVOID)0);
        } else {
            AddrFromHthdx(&hthd->addrFrom, hthd);
            IncrementIP(hthd);
            hthd->fReturning = TRUE;
            SetupReturnStep(hthd, fDoContinue, addrRA, addrStack);
        }

    } else if (bp) {

        RestoreInstrBP(hthd, bp);
        /* Need to keep this bp alive until we're done */
        ++bp->cthd;
        ClearBPFlag(hthd);

        //
        // Issue the single step command
        //

        if (!addrRA) {
            RegisterExpectedEvent(hthd->hprc,
                                  hthd,
                                  EXCEPTION_DEBUG_EVENT,
                                  (DWORD)EXCEPTION_SINGLE_STEP,
                                  notify,
                                  SSActionReplaceByte,
                                  FALSE,
                                  bp);

            SetupSingleStep(hthd, TRUE, TRUE);
        } else {
            // Step over the bp and then continue.
            RegisterExpectedEvent(hthd->hprc,
                                    hthd,
                                    EXCEPTION_DEBUG_EVENT,
                                    (DWORD)EXCEPTION_SINGLE_STEP,
                                    DONT_NOTIFY,
                                    (ACVECTOR) SSActionRBAndContinue,
                                    FALSE,
                                    bp);

            SetupSingleStep(hthd, FALSE, TRUE);
            AddrFromHthdx(&hthd->addrFrom, hthd);
            hthd->fReturning = TRUE;
            SetupReturnStep(hthd, fDoContinue, addrRA, addrStack);
        }

    } else {    // bp == NULL

        //
        // Determine if the current instruction is a breakpoint
        //  instruction.  If it is then based on the stopOnBP flag we either
        //  execute to hit the breakpoint or skip over it and create a
        //  single step event
        //

        IsCall(hthd, &currAddr, &lpf, FALSE);

        if (lpf == INSTR_CANNOT_STEP) {
            SendDBCErrorStep(hthd->hprc);
            return;
        }

        if (lpf == INSTR_BREAKPOINT) {

            if (stopOnBP) {
                /*
                 * We were instructed to stop on breakpoints
                 * Just issue an execute command and execute
                 * the breakpoint.
                 */

                ThreadContinueDebugEvent (hthd);
            } else {
                /*
                 * else just increment past it
                 * & pretend that a single step actually took place
                 */

                DPRINT(3, ("    At an embedded bp -- ignoring\n\r"));

                IncrementIP(hthd);

                hthd->fIsCallDone = FALSE;
                ClearBPFlag(hthd);
                if (notify) {
                    (notify->notifyFunction)(&falseSSEvent,
                                             hthd,
                                             0,
                                             notify->lparam);
                } else {
                    NotifyEM(&falseSSEvent, hthd, 0, (LPVOID)0);
                }
            }

        } else {


            if (lpf == INSTR_CANNOT_TRACE) {

				if (!addrRA) {
                     bp = SetBP( hthd->hprc,
                                 hthd,
                                 bptpExec,
                                 bpnsStop,
                                 &currAddr,
                                 (HPID) INVALID);
                     bp->isStep = TRUE;
					 fTraceOn   = FALSE;
				}

            }


            //
            // Place this on our list of expected events
            //

            RegisterExpectedEvent(hthd->hprc,
                                  hthd,
                                  EXCEPTION_DEBUG_EVENT,
                                  (DWORD)EXCEPTION_SINGLE_STEP,
                                  notify,
                                  NO_ACTION,
                                  FALSE,
                                  NULL);

            //
            // Issue the single step command
            //

            if (!addrRA) {
                if (hthd->fDisplayReturnValues && 
                    IsRet (hthd, &currAddrActual) 
                    // && !CheckNLG (hthd -> hprc, hthd, NLG_RETURN, &currAddrActual)
                    ) {
                    //
                    // If we're supposed to be fetching the return value,
                    // Record the current address in hthd -> addrFrom and 
                    // NotifyEM will get it displayed.
                    //

                    AddrFromHthdx(&hthd->addrFrom, hthd);
                    NotifyEM(&FuncExitEvent, hthd, 0, &currAddrActual);
                }

                SetupSingleStep(hthd, TRUE, fTraceOn);
            } else {
                AddrFromHthdx(&hthd->addrFrom, hthd);
                hthd->fReturning = TRUE;
                SetupReturnStep(hthd, fDoContinue, addrRA, addrStack);
            }
        }
    }

    return;
}                               /* ReturnStepEx() */



void
IncrementIP(
    HTHDX hthd
    )
{
    PC(hthd) += BP_SIZE;
    assert(hthd->tstate & ts_stopped);
    hthd->fContextDirty = TRUE;

    return;
}                               /* IncrementIP() */




void
DecrementIP(
    HTHDX hthd
    )
{
    // M00BUG -- two byte version of int 3

    PC(hthd) -= BP_SIZE;
    assert(hthd->tstate & ts_stopped);
    hthd->fContextDirty = TRUE;

    return;
}                               /* DecrementIP() */


VOID
MoveIPToException(
    HTHDX hthd,
    LPDEBUG_EVENT pde
    )
/*++

Routine Description:

    This function moves the EIP for a thread to where an exception occurred.
    This is primarily used for breakpoints.  There are two advantages of
    this over simply decrementing the IP: (1) its CPU-independent, and
    (2) it helps work around an NT bug.  The NT bug is, if an app which is
    NOT being debugged has a hard-coded INT 3, and the user starts post-
    mortem debugging, then when NT gives us the INT 3 exception, it will
    give us the wrong EIP: it gives us the address of the INT 3, rather
    than one byte past that.  But it gives us the correct ExceptionAddress.

Arguments:


Return Value:

    None

--*/

{
    PC(hthd) = (DWORD) EXADDR(pde);
    assert(hthd->tstate & ts_stopped);
    hthd->fContextDirty = TRUE;
}




VOID
StepOver(
    HTHDX hthd,
    METHOD* notify,
    BOOL stopOnBP,
    BOOL fInFuncEval
    )
{
    StepOverEx (hthd, notify, stopOnBP, fInFuncEval, TRUE);
}



VOID
StepOverEx(
    HTHDX hthd,
    METHOD* notify,
    BOOL stopOnBP,
    BOOL fInFuncEval,
    BOOL fCheckOrpc
    )
/*++

Routine Desription:


Arguments:

    fCheckOrpc - Supplies TRUE if we should check for ORPC sections; FALSE if
                 the step function should ignore ORPC sections (treat them as
                 normal code).

Return Value:


--*/
{
    ADDR        currAddr;
    int         lpf = 0;
    BREAKPOINT  *bp, *atbp;
    HPRCX       hprc=hthd->hprc;
    METHOD      *method;

    DPRINT(3, ("** SINGLE STEP OVER  "));

#if 0
    //
    //      if we are stepping into an ORPC section, the following function will
    //      take care of everythging for us and return TRUE.  Just return.
    //

    if (fCheckOrpc && CheckAndSetupForOrpcSection (hthd)) {
        return;
    }
#endif // !KERNEL

    //
    //  Get the current IP of the thread
    //

    AddrFromHthdx(&currAddr, hthd);

    //
    //  Determine what type of instruction we are presently on
    //

    IsCall(hthd, &currAddr, &lpf, TRUE);

    //
    //  If the instruction is not a call or an intrpt then do a SS
    //

    if (lpf == INSTR_TRACE_BIT) {

        ReturnStepEx (hthd,
                      notify,
                      stopOnBP,
                      fInFuncEval,
                      NULL,
                      NULL,
                      TRUE,
                      fCheckOrpc);

        return;
    } else if (lpf == INSTR_CANNOT_STEP) {
        SendDBCErrorStep(hthd->hprc);
        return;
    }

        ExprBPContinue(hthd->hprc, hthd);

    //
    //  If the instruction is a BP then "uncover" it
    //

    if (lpf == INSTR_BREAKPOINT) {

        DPRINT(5, ("  We have hit a breakpoint instruction\n\r"));

        if (stopOnBP) {
            /*
            **  We were instructed to stop on breakpoints
            **  Just issue an execute command and execute
            **  the breakpoint.
            */

            ThreadContinueDebugEvent (hthd);

        } else {

            IncrementIP(hthd);
            hthd->fIsCallDone = FALSE;
            ClearBPFlag(hthd);
            if (notify) {
                (notify->notifyFunction)(&falseSSEvent, hthd, 0, notify->lparam);
            } else {
                NotifyEM(&falseSSEvent, hthd, 0, (LPVOID)0);
            }
        }

    } else {

        /*
        **  If control gets to this point, then the instruction
        **  that we are at is either a call or an interrupt.
        */
        BOOL fDisplayReturnValues = hthd->fDisplayReturnValues && notify->lparam;

        atbp = AtBP(hthd);
        if (!stopOnBP && !atbp) {
            ADDR AddrPC;

            AddrFromHthdx(&AddrPC, hthd);
            atbp = FindBP(hthd->hprc, hthd, bptpExec, (BPNS)-1, &AddrPC, FALSE);
            SetBPFlag(hthd, atbp);
        }

        if (atbp) {
            /*
            ** Put the single step on our list of expected
            ** events and set the action to "Replace Byte and Continue"
            ** without notifying the EM
            */


            RestoreInstrBP(hthd, atbp);
            ClearBPFlag(hthd);

            //
            // We don't want to Continue automatically when return values are
            // being displayed because we need to note the addr the Call is
            // going to after all the thunk walking is done.
            //
            if (!fDisplayReturnValues) {
                RegisterExpectedEvent(hthd->hprc,
                                        hthd,
                                        EXCEPTION_DEBUG_EVENT,
                                        (DWORD)EXCEPTION_SINGLE_STEP,
                                        DONT_NOTIFY,
                                        (ACVECTOR) SSActionRBAndContinue,
                                        FALSE,
					atbp);
                /*
                **  Issue the single step
                */

                if (lpf != INSTR_CANNOT_TRACE) {
                    SetupSingleStep(hthd, FALSE, TRUE);
                }
            }
        }

        if (lpf == INSTR_IS_CALL || lpf == INSTR_CANNOT_TRACE) {

			RANGESTEP *rs = NULL;
			
			//
            // Set a BP after this call instruction.
            //

            bp = SetBP (hprc, hthd, bptpExec, bpnsStop, &currAddr, (HPID)INVALID);

			assert(bp);

            //
            //  Make a copy of the notification method.
            //

            method  = (METHOD*)MHAlloc(sizeof(METHOD));
            *method = *notify;

            //
            //  Store the breakpoint with this notification method
            //

            method->lparam2 = (LPVOID)bp;


			//
            // On a step over, we want to get back to the same frame.
            // The EBP is saved across functions (even if it's not the FP)
            // So it will be the same when the safety BP is hit - if not
            // we are defintely on the wrong frame
            // ALPHA NOTE: If you set SavedEBP to 0 then this will default
            // to the old behavior.
			//

			//
			// BUGBUG: by casting method->lparam to a RANGESTEP* you assume
			// this is being called by MethodRangeStep or RangeStep.  This
			// IS NOT IN GENERAL TRUE. WE MUST FIX THIS.
			//


			//
			// NOTE: in addition we only use the frame-based BP when we are not
			// in a prolog.  This allows EBP-modifying helper functions to get
			// called in the prolog.  The EH_prolog function, which is called
			// when using C++ exceptions and optimizing /Os is an example of
			// this.
			//
			
			rs = (RANGESTEP*) method->lparam;

			if (rs) {
				ADDR	EIP = {0};
				CANSTEP	CanStep = {0};

				AddrFromHthdx (&EIP, hthd);
				
				GetCanStep (hprc->hpid, hthd->htid, &EIP, &CanStep);

				if (CanStep.Flags == CANSTEP_YES && CanStep.PrologOffset) {
					rs->SavedEBP = 0;
				} else {
					rs->SavedEBP = FRAME_POINTER (hthd);
				}
			}

            if (fDisplayReturnValues && lpf == INSTR_IS_CALL) {

                ACVECTOR action = NO_ACTION;
                LPVOID lParam = NULL;
                RANGESTEP* rs = method->lparam;   // ??? OK?

                rs->fGetReturnValue = TRUE;
                rs->safetyBP = bp;

                if ( atbp ) {
                    ++atbp->cthd;
                    action = SSActionReplaceByte;
                    lParam = atbp;
                }

                assert (!hthd->fReturning);

                RegisterExpectedEvent(
                    hthd->hprc,
                    hthd,
                    EXCEPTION_DEBUG_EVENT,
                    (DWORD)EXCEPTION_SINGLE_STEP,
                    method,
                    action,
                    FALSE,
                    lParam);

                RegisterExpectedEvent(hthd->hprc,
                        hthd,
                        BREAKPOINT_DEBUG_EVENT,
                        (DWORD)bp,
                        method,
                        action,
                        FALSE,
                        lParam);
                
                SetupSingleStep (hthd, TRUE, TRUE);

                return;
            }

            /*
            ** Place this on our list of expected events
            ** (Let the action function do the notification, other-
            ** wise the EM will receive a breakpoint notification,
            ** rather than a single step notification).NOTE:
            ** This is the reason why we make a copy of the notif-
            ** ication method -- because the action function must
            ** know which notification method to use, in addition
            ** to the breakpoint that was created.
            */
			

            RegisterExpectedEvent(hthd->hprc,
                    hthd,
                    BREAKPOINT_DEBUG_EVENT,
                    (DWORD)bp,
                    DONT_NOTIFY,
                    (ACVECTOR) SSActionStepOver,
                    FALSE,
                    method);


            DPRINT(7, ("PID= %lx  TID= %lx\n", hprc->pid, hthd->tid));

        }


        /*
        **  Issue the execute command
        */

        ThreadContinueDebugEvent (hthd);

        //
        // If we hit a DIFFERENT BP while we are in the called routine we
        // must clear this (and ALL other consumable events) from the
        // expected event queue.
        //

    }
    return;
}                               /* StepOver() */



void
SSActionRemoveBP(
    DEBUG_EVENT* de,
    HTHDX hthd,
    DWORD unused,
    METHOD* method
    )
/*++

Routine Description:

    This action function is called upon the receipt of a breakpoint
    event in order to remove the breakpoint and fake a single step event.
    Note that the lparam to this action function must be a METHOD* and
    the BREAKPOINT* to remove must be stored in the lparam2 field of the
    method.

Arguments:


Return Value:


--*/
{
    BREAKPOINT* bp = (BREAKPOINT*)method->lparam2;

    Unreferenced( de );

    DEBUG_PRINT("** SS Action Remove BP called\n");

	// Remove the temporary breakpoint
    RemoveBP(bp);

    // Notify whoever is concerned, that a SS event has occured
    (method->notifyFunction)(&falseSSEvent, hthd, 0, method->lparam);

    // Free the temporary notification method.
    MHFree(method);
}

void
SSActionStepOver(
    DEBUG_EVENT* de,
    HTHDX hthd,
    DWORD unused,
    METHOD* method
    )
/*++

Routine Description:

    This action function is called upon the receipt of a breakpoint
    event in order to remove the breakpoint and fake a single step event.

  	This version has been modified to make stepover work.
	In the case of a step over - if the function is recursive we want to stop on
	the correct frame.

    Note that the lparam to this action function must be a METHOD* and
    the BREAKPOINT* to remove must be stored in the lparam2 field of the
    method.

Arguments:


Return Value:


--*/
{
    BREAKPOINT* bp = (BREAKPOINT*)method->lparam2;
	RANGESTEP *rs= (RANGESTEP *)method->lparam;

    Unreferenced( de );

	//
    // What if SavedEBP is not the frame pointer? Retail? Retail w/o FPO?
    // OK here goes the thinking:
    // EBP may not be the frame pointer - but it is guaranteed across
    // function calls - even in retail - so when we hit the safety BP
    // it must be equal to the SavedEBP
	//
    // Use of the FRAME_POINTER macro is to allow this to compile on Alpha
    // however I'm not convinced this algorithm will work for the Alpha.
	//
    // If there is no rs structure we are stepping in disassembly
    // we still use the old algorithm.
	//

	if (!rs || rs->SavedEBP <= FRAME_POINTER(hthd)) {

		DbgTrace ((TR_RANGESTEP, "SSActionStepOver: hit BP.  Notifying.\n"));
		
		DEBUG_PRINT("** SS Action Remove BP called\n");

		RemoveBP(bp);
		(method->notifyFunction)(&falseSSEvent, hthd, 0, method->lparam);
		MHFree(method);

	} else {

		//
        // We have hit the safety BP in a recursive instance of a call
        // Therefore we are not really at the end of the rangestep.
        // restore the bp, do a step, then run.
        //

		METHOD*	ContinueSOMethod = NULL;

		DbgTrace ((TR_RANGESTEP,
				   "SSActionStepOver: hit BP in recursive instance\n"
				   ));
		
        ContinueSOMethod = (METHOD*) MHAlloc(sizeof(METHOD));
        ContinueSOMethod->notifyFunction = (ACVECTOR)MethodContinueSS;
        ContinueSOMethod->lparam = ContinueSOMethod;
        ContinueSOMethod->lparam2 = bp;
        if(bp && bp != EMBEDDED_BP)
            ++bp->cthd;
        ClearBPFlag(hthd);
		RestoreInstrBP(hthd, bp);

		//
		// Reenable
		
        RegisterExpectedEvent(hthd->hprc, hthd,
                              BREAKPOINT_DEBUG_EVENT,
                              (DWORD) bp,
                              DONT_NOTIFY,
                              (ACVECTOR) SSActionStepOver,
                              FALSE,
                              method);

		SingleStepEx(hthd, ContinueSOMethod, FALSE, FALSE, TRUE);
	}
}


void
SSActionReplaceByte(
    DEBUG_EVENT *de,
    HTHDX hthd,
    DWORD unused,
    BREAKPOINT *bp
    )
/*++

Routine Description:

    This action function is called upon the receipt of a single step
    event in order to replace the breakpoint instruction (INT 3, 0xCC)
    that was written over.

Arguments:


Return Value:


--*/
{
    Unreferenced( de );
    Unreferenced( unused );

    /* Keep this breakpoint around */
    EnterCriticalSection(&csThreadProcList);
    /* We're only going to restore the BP if it's still in use */
    if(bp->instances) {
        if (bp->hWalk) {
            DPRINT(5, ("** SS Action Replace Byte is really a data BP\n"));
            ExprBPResetBP(hthd, bp);
        } else {
            DPRINT(5, ("** SS Action Replace Byte at %d:%04x:%08x with %x\n",
                       ADDR_IS_FLAT(bp->addr), bp->addr.addr.seg, bp->addr.addr.off, BP_OPCODE));
            WriteBreakPoint( bp );
        }
    }
    ReleaseBP(bp);
    LeaveCriticalSection(&csThreadProcList);

    return;
}



void
SSActionRBAndContinue(
    DEBUG_EVENT *de,
    HTHDX hthd,
    DWORD unused,
    BREAKPOINT *bp
    )
/*++

Routine Description:

    This action function is called upon the receipt of a single step
    event in order to replace the breakpoint instruction (INT 3, 0xCC)
    that was written over, and then continuing execution.

Arguments:


Return Value:


--*/
{
    Unreferenced( de );
    Unreferenced( unused );

    if (bp->hWalk) {
        //
        // Really a hardware BP, let walk manager fix it.
        //
        DPRINT(5, ("** SS Action RB and continue is really a data BP\n"));
        ExprBPResetBP(hthd, bp);
    } else {
        DPRINT(5, ("** SS Action RB and Continue: Replace byte @ %d:%04x:%08x with %x\n",
                    ADDR_IS_FLAT(bp->addr), bp->addr.addr.seg, bp->addr.addr.off, BP_OPCODE));

        WriteBreakPoint( bp );
    }

    ThreadContinueDebugEvent (hthd);
}


BOOL InsideRange( HTHDX, ADDR*, ADDR*, ADDR* );
BRANCH_LIST * GetBranchList ( HTHDX, ADDR*, ADDR* );
RANGESTRUCT * SetupRange ( HTHDX, ADDR*, ADDR*, BRANCH_LIST *, BOOL, BOOL, METHOD* );
VOID AddRangeBp( RANGESTRUCT*, ADDR*, BOOL );
VOID SetRangeBp( RANGESTRUCT* );
VOID RemoveRangeBp( RANGESTRUCT* );
BOOL GetThunkTarget( HTHDX, RANGESTRUCT*, ADDR*, ADDR* );

VOID RecoverFromSingleStep( ADDR*, RANGESTRUCT*);
BOOL ContinueFromInsideRange( ADDR*, RANGESTRUCT*);
BOOL ContinueFromOutsideRange( ADDR*, RANGESTRUCT*);


#if 0

BOOL
SmartRangeStep(
    HTHDX       hthd,
    UOFF32      offStart,
    UOFF32      offEnd,
    BOOL        fStopOnBP,
    BOOL        fStepOver
    )

/*++

Routine Description:

    This function is used to implement range stepping the the DM.  Range
    stepping is used to cause all instructions between a pair of addresses
    to be executed.

    The segment is implied to be the current segment.  This is validated
    in the EM.

Arguments:

    hthd      - Supplies the thread to be stepped.

    offStart  - Supplies the initial offset in the range

    offEnd    - Supplies the final offset in the range

    fStopOnBP - Supplies TRUE if stop on an initial breakpoint

    fStepOver - Supplies TRUE if to step over call type instructions

Return Value:

    TRUE if successful.  If the disassembler fails, a breakpoint cannot
    be set or other problems, FALSE will be returned, and the caller will
    fall back to the slow range step method.

--*/

{
    BRANCH_LIST  *BranchList;
    METHOD       *Method;
    RANGESTRUCT  *RangeStruct;
    ADDR         AddrStart;
    ADDR         AddrEnd;

    //
    //  Initialize start and end addresses
    //

    AddrInit(&AddrStart, 0, PcSegOfHthdx(hthd), offStart,
                hthd->fAddrIsFlat, hthd->fAddrOff32, FALSE, hthd->fAddrIsReal);

    AddrInit(&AddrEnd, 0, PcSegOfHthdx(hthd), offEnd,
                hthd->fAddrIsFlat, hthd->fAddrOff32, FALSE, hthd->fAddrIsReal);



    //
    //  Locate all the branch instructions inside the range (and their
    //  targets if available) and obtain a branch list.
    //
    BranchList  = GetBranchList( hthd, &AddrStart, &AddrEnd );

    if (!BranchList) {
        return FALSE;
    }

    //
    //  Setup range step method
    //
    Method = (METHOD*)MHAlloc(sizeof(METHOD));
    assert( Method );

    Method->notifyFunction  = (ACVECTOR)MethodSmartRangeStep;

    //
    //  Set up the range structure (this will set all safety breakpoints).
    //
    RangeStruct = SetupRange( hthd, &AddrStart, &AddrEnd, BranchList,
                                             fStopOnBP, fStepOver, Method );
    if (!RangeStruct) {
        MHFree(BranchList);
        return FALSE;
    }

    //
    //  Now let the thread run.
    //

    ThreadContinueDebugEvent (hthd);

#ifdef KERNEL
    hthd->tstate |= ts_stepping;
#endif

    return TRUE;
}
#endif


BOOL
InsideRange(
    HTHDX   hthd,
    ADDR   *AddrStart,
    ADDR   *AddrEnd,
    ADDR   *Addr
    )
{
    ADDR    AddrS;
    ADDR    AddrE;
    ADDR    AddrC;

    assert( AddrStart );
    assert( AddrEnd );
    assert( Addr );

    if ( ADDR_IS_LI(*Addr) ) {
        return FALSE;
    }

    AddrS = *AddrStart;
    AddrE = *AddrEnd;
    AddrC = *Addr;

    if (!ADDR_IS_FLAT(AddrS)) {
        if ( !TranslateAddress(hthd->hprc, hthd, &AddrS, TRUE) ) {
            return FALSE;
        }
    }

    if (!ADDR_IS_FLAT(AddrE)) {
        if ( !TranslateAddress(hthd->hprc, hthd, &AddrE, TRUE) ) {
            return FALSE;
        }
    }

    if (!ADDR_IS_FLAT(AddrC)) {
        if ( !TranslateAddress(hthd->hprc, hthd, &AddrC, TRUE) ) {
            return FALSE;
        }
    }

    if ( GetAddrOff( AddrC ) >= GetAddrOff( AddrS ) &&
                            GetAddrOff( AddrC ) <= GetAddrOff( AddrE ) ) {

        return TRUE;
    }

    return FALSE;
}


#if 0
BRANCH_LIST *
GetBranchList (
    HTHDX   hthd,
    ADDR   *AddrStart,
    ADDR   *AddrEnd
    )
/*++

Routine Description:

    Locates all the branch instructions within a range and builds a
    branch list.

Arguments:

    hthd        -   Supplies thread

    AddrStart   -   Supplies start of range

    AddrEnd     -   Supplies end of range

Return Value:

    BRANCH_LIST *   -   Pointer to branch list.

--*/
{
    void        *Memory;
    BRANCH_LIST *BranchList = NULL;
    BRANCH_LIST *BranchListTmp;
    DWORD        RangeSize;
    LONG         Length;
    BYTE        *Instr;
    DWORD        ListSize;
    DWORD        i;
    ADDR         Addr;

    assert( AddrStart );
    assert( AddrEnd );

    RangeSize  =  GetAddrOff(*AddrEnd) - GetAddrOff(*AddrStart) + 1;

    //
    //  Read the code.
    //
    Memory = MHAlloc( RangeSize );
    assert( Memory );

    if (!Memory) {
        return NULL;
    }

    //
    //  Allocate and initialize the branch list structure
    //
    ListSize   = sizeof( BRANCH_LIST );
    BranchList = (BRANCH_LIST *)MHAlloc( ListSize );

    assert( BranchList );

    if (!BranchList) {
        MHFree(Memory);
        return NULL;
    }

    BranchList->AddrStart = *AddrStart;
    BranchList->AddrEnd   = *AddrEnd;
    BranchList->Count     = 0;


    Addr = *AddrStart;

    AddrReadMemory(hthd->hprc, hthd, &Addr, Memory, RangeSize, &Length );
#ifndef KERNEL
    assert(Length==(LONG)RangeSize);
#endif
    //
    // If the code crosses a page boundary and the second
    // page is not present, the read will be short.
    // Fail, and we will fall back to the slow step code.
    //

    if (Length != (LONG)RangeSize) {
        MHFree(BranchList);
        MHFree(Memory);
        return NULL;
    }


    //
    //  Unassemble the code and determine where all branches are.
    //
    Instr  = (BYTE *)Memory;

    while ( Length > 0 ) {

        BOOL    IsBranch;
        BOOL    TargetKnown;
        BOOL    IsCall;
        BOOL    IsTable;
        ADDR    Target;
        DWORD   Consumed;

        //
        //  Unassemble one instruction
        //
        Consumed = BranchUnassemble(hthd,
                                    (void *)Instr,
                                    Length,
                                    &Addr,
                                    &IsBranch,
                                    &TargetKnown,
                                    &IsCall,
                                    &IsTable,
                                    &Target );

        assert( Consumed > 0 );
        if ( Consumed == 0 ) {

            //
            //  Could not unassemble the instruction, give up.
            //

            MHFree(BranchList);
            BranchList = NULL;
            Length = 0;

        } else {

            if (IsBranch && IsTable &&
                    InsideRange(hthd, AddrStart, AddrEnd, &Target)) {

                //
                // this is a vectored jump with the table included
                // in the source range.  Rather than try to figure
                // out what we can and cannot disassemble, punt
                // here and let the slow step code catch it.
                //

                MHFree(BranchList);
                BranchList = NULL;
                Length = 0;

            } else if ( IsBranch ) {

                //
                //  If instruction is a branch, and the branch falls outside
                //  of the range, add a branch node to the list.
                //

                BOOLEAN fAdded = FALSE;

                if ( TargetKnown ) {
                    if ( ADDR_IS_FLAT(Target) ) {
                        if ( GetAddrOff(Target) != 0 ) {
                            GetAddrSeg(Target) = PcSegOfHthdx(hthd);
                        }
                    } else {
                        ADDR_IS_REAL(Target) = (BYTE)hthd->fAddrIsReal;
                    }
                }

                if ( !InsideRange( hthd, AddrStart, AddrEnd, &Target ) ||
                                                            !TargetKnown ) {

                    //
                    // this loop is to ensure that we dont get duplicate
                    // breapoints set
                    //
                    for (i=0; i<BranchList->Count; i++) {

                        if ( TargetKnown &&
                        FAddrsEq( BranchList->BranchNode[i].Target, Target ) ) {
                            break;
                        }
                    }

                    if (i == BranchList->Count) {
                        ListSize += sizeof( BRANCH_NODE );
                        BranchListTmp = (BRANCH_LIST *)MHRealloc( BranchList,
                                                                ListSize );
                        assert( BranchListTmp );
                        BranchList = BranchListTmp;

                        BranchList->BranchNode[ BranchList->Count ].TargetKnown=
                                                                   TargetKnown;
                        BranchList->BranchNode[ BranchList->Count ].IsCall =
                                                                   IsCall;
                        BranchList->BranchNode[ BranchList->Count ].Addr   =
                                                                   Addr;
                        BranchList->BranchNode[ BranchList->Count ].Target =
                                                                   Target;

                        BranchList->Count++;

                        fAdded = TRUE;
                    }
                }

#if defined(TARGET_MIPS)
                //
                //  If the delay slot falls outside the range, add
                //  the branch to the list no matter what the target.
                //
                if ( !fAdded && GetAddrOff(Addr) + 4 > GetAddrOff(*AddrEnd) ) {
                    ListSize += sizeof( BRANCH_NODE );
                    BranchListTmp = (BRANCH_LIST *)MHRealloc( BranchList,
                                                            ListSize );
                    assert( BranchListTmp );
                    BranchList = BranchListTmp;

                    BranchList->BranchNode[ BranchList->Count ].TargetKnown =
                                                                   TargetKnown;
                    BranchList->BranchNode[ BranchList->Count ].IsCall      =
                                                                   IsCall;
                    BranchList->BranchNode[ BranchList->Count ].Addr        =
                                                                   Addr;
                    BranchList->BranchNode[ BranchList->Count ].Target      =
                                                                   Target;

                    BranchList->Count++;
                }
#endif
            }

            Instr            += Consumed;
            GetAddrOff(Addr) += Consumed;
            Length           -= Consumed;
        }
    }

    MHFree( Memory );

    return BranchList;
}
#endif


RANGESTRUCT *
SetupRange (
    HTHDX        hthd,
    ADDR        *AddrStart,
    ADDR        *AddrEnd,
    BRANCH_LIST *BranchList,
    BOOL         fStopOnBP,
    BOOL         fStepOver,
    METHOD      *Method
    )
/*++

Routine Description:

    Helper function for RangeStep.

Arguments:

    hthd        -   Supplies thread

    AddrStart   -   Supplies start of range

    AddrEnd     -   Supplies end of range

    BranchList  -   Supplies branch list

    fStopOnBP   -   Supplies fStopOnBP flag

    fStepOver   -   Supplies fStepOver flag

Return Value:

    RANGESTRUCT *   -   Pointer to range structure

--*/
{
    RANGESTRUCT *RangeStruct;
    BREAKPOINT  *Bp;
    DWORD        i;
    BOOLEAN      fAddedAtEndOfRange = FALSE;
    ADDR         Addr;
    CANSTEP      CanStep;
    DWORD        dwOffset;
    DWORD        dwSize;
    DWORD        dwPC;


    assert( AddrStart );
    assert( AddrEnd );
    assert( BranchList );
    assert( Method );

    //
    //  Allocate and initialize the range structure
    //
    RangeStruct = (RANGESTRUCT *)MHAlloc( sizeof(RANGESTRUCT) );
    assert( RangeStruct );

    RangeStruct->hthd        = hthd;
    RangeStruct->BranchList  = BranchList;
    RangeStruct->fStepOver   = fStepOver;
    RangeStruct->fStopOnBP   = fStopOnBP;
    RangeStruct->BpCount     = 0;
    RangeStruct->BpAddrs     = NULL;
    RangeStruct->BpList      = NULL;
    RangeStruct->fSingleStep = FALSE;
    RangeStruct->fInCall     = FALSE;
    RangeStruct->Method      = Method;

    Method->lparam           = RangeStruct;


    //
    //  If the given range has branches, set branch breakpoints according to
    //  the fStepOver flag.
    //
    if ( BranchList->Count > 0 ) {

        if ( fStepOver ) {

            //
            //  Ignore calls (Were stepping over them), set BPs in all
            //  known target (if outside of range) and all branch instructions
            //  with unknown targets.
            //
            for ( i=0; i < BranchList->Count; i++ ) {

#if defined(TARGET_MIPS)
                //
                //  If delay slot is outside range, set breakpoint at next
                //  instruction after delay slot (i.e. the return address)
                //
                if ( GetAddrOff(BranchList->BranchNode[i].Addr) + 4 > GetAddrOff(*AddrEnd) ) {
                    ADDR Addr = BranchList->BranchNode[i].Addr;
                    GetAddrOff(Addr) += 8;
                    AddRangeBp( RangeStruct, &Addr, FALSE );
                    fAddedAtEndOfRange = TRUE;
                }
#endif
                if ( !BranchList->BranchNode[i].IsCall ) {
                    if ( !BranchList->BranchNode[i].TargetKnown ) {

                        AddRangeBp( RangeStruct, &BranchList->BranchNode[i].Addr, FALSE );

                    } else if ( !InsideRange( hthd, AddrStart, AddrEnd, &BranchList->BranchNode[i].Target ) ) {

                        AddRangeBp( RangeStruct, &BranchList->BranchNode[i].Target, FALSE );
                    }
                }
            }

        } else {

            //
            //  Set BPs in all branches/calls with unknown targets, all
            //  branch targets (if outside of range) and all  call targets
            //  for which we have source.
            //
            for ( i=0; i < BranchList->Count; i++ ) {

#if defined(TARGET_MIPS)
                //
                //  If delay slot is outside range, set breakpoint at next
                //  instruction after delay slot (i.e. the return address)
                //
                if ( GetAddrOff(BranchList->BranchNode[i].Addr) + 4 > GetAddrOff(*AddrEnd) ) {
                    ADDR Addr = BranchList->BranchNode[i].Addr;
                    GetAddrOff(Addr) += 8;
                    AddRangeBp( RangeStruct, &Addr, FALSE );
                    fAddedAtEndOfRange = TRUE;
                }
#endif
                if ( !BranchList->BranchNode[i].TargetKnown ) {

                    AddRangeBp( RangeStruct, &BranchList->BranchNode[i].Addr, FALSE );

                } else if ( !InsideRange( hthd, AddrStart, AddrEnd, &BranchList->BranchNode[i].Target ) ) {

                    if ( !BranchList->BranchNode[i].IsCall ) {

                        AddRangeBp( RangeStruct, &BranchList->BranchNode[i].Target, FALSE );

                    } else {

                        //
                        //  BUGBUG - If debugging WOW, we dont set a
                        //  breakpoint in a function prolog, instead we set the
                        //  breakpoint in the call instruction and single step
                        //  to the function.
                        //
                        if (!ADDR_IS_FLAT(BranchList->BranchNode[i].Addr) ) {

                            AddRangeBp( RangeStruct, &BranchList->BranchNode[i].Addr, FALSE );

                        } else {

                            //
                            // Dont look for source lines until we hit the call target.
                            //
                            // If the call target is a thunk, we might not be able to interpret
                            // it until the thread is sitting on it.  Therefore, we always have
                            // to stop at the call target, so there is no point in looking now
                            // to see whether there is source data for it until we get there.
                            //

                            //
                            // Walk thunks, as far as we can.
                            //

                            dwPC = GetAddrOff(BranchList->BranchNode[i].Target);
                            while (IsThunk(hthd, dwPC, NULL, &dwPC, NULL )) {
                                GetAddrOff(BranchList->BranchNode[i].Target) = dwPC;
                            }

                            AddRangeBp( RangeStruct, &BranchList->BranchNode[i].Target, FALSE );

                        }
                    }
                }
            }
        }
    }

    if ( !fAddedAtEndOfRange ) {
        //
        //  We always set a safety breakpoint at the instruction past the end
        //  of the range.
        //
        ADDR Addr = *AddrEnd;
        GetAddrOff(Addr) += 1;
        AddRangeBp( RangeStruct, &Addr, FALSE );
    }

    //
    //  If we currently are at a BP and the address is not already in the
    //  list, then we must setup a single step for the instruction.
    //
    Bp = AtBP(hthd);

    if ( Bp == EMBEDDED_BP ) {

        //
        // we must step off the harcoded bp
        //

        ClearBPFlag( hthd );
        IncrementIP( hthd );
        hthd->fIsCallDone = FALSE;

    } else if ( Bp ) {

        //
        //  Make sure that the BP is not in the list
        //
        for ( i=0; i<RangeStruct->BpCount; i++ ) {
            if ( FAddrsEq( RangeStruct->BpAddrs[i], Bp->addr )) {
                break;
            }
        }

        if ( i >= RangeStruct->BpCount ) {
            //
            //  We have to single step the breakpoint.
            //
            ClearBPFlag( hthd );
            RestoreInstrBP( RangeStruct->hthd, Bp );
            RangeStruct->PrevAddr  = Bp->addr;
            RangeStruct->fSingleStep = TRUE;

            //
            //  Set the fInCall flag so that the stepping method knows whether
            //  or not it should stop stepping in case we get out of the range.
            //
            for ( i=0; i < RangeStruct->BranchList->Count; i++ ) {
                if ( FAddrsEq( Bp->addr,
                               RangeStruct->BranchList->BranchNode[i].Addr ) ) {
                    RangeStruct->fInCall =
                                 RangeStruct->BranchList->BranchNode[i].IsCall;
                    break;
                }
            }

#ifdef TARGET_i386

#ifndef KERNEL
            HthdReg(RangeStruct->hthd,EFlags) |= TF_BIT_MASK;
            RangeStruct->hthd->fContextDirty = TRUE;
#endif

#else   // i386
            {
                ADDR         Addr;
                UOFF32       NextOffset;

                NextOffset = GetNextOffset( hthd, RangeStruct->fStepOver );

#ifndef KERNEL
                if ( NextOffset != 0x00000000 ) {
                    AddrInit( &Addr, 0, 0, NextOffset, TRUE, TRUE, FALSE, FALSE );
                    RangeStruct->TmpAddr = Addr;
                    RangeStruct->TmpBp = SetBP( RangeStruct->hthd->hprc,
                                                RangeStruct->hthd,
                                                bptpExec,
                                                bpnsStop,
                                                &Addr,
                                                (HPID) INVALID);
                }
                assert( RangeStruct->TmpBp );

#else   // KERNEL


                AddrInit( &Addr, 0, 0, NextOffset, TRUE, TRUE, FALSE, FALSE );

                GetCanStep(RangeStruct->hthd->hprc->hpid,
                           RangeStruct->hthd->htid,
                           &Addr,
                           &CanStep);

                if (CanStep.Flags == CANSTEP_YES) {
                    GetAddrOff(Addr) += CanStep.PrologOffset;
                }

                RangeStruct->TmpAddr = Addr;
                RangeStruct->TmpBp = SetBP( RangeStruct->hthd->hprc,
                                            RangeStruct->hthd,
                                            bptpExec,
                                            bpnsStop,
                                            &Addr,
                                            (HPID) INVALID);
                assert( RangeStruct->TmpBp );
#endif   // KERNEL

            }
#endif   // i386
        }
    }

    SetRangeBp( RangeStruct );

    return RangeStruct;
}

VOID
AddRangeBp(
    RANGESTRUCT *RangeStruct,
    ADDR        *Addr,
    BOOL         fSet
    )
/*++

Routine Description:

    Sets a breakpoint at a particular address and adds it to the breakpoint
    list in a RANGESTRUCT

Arguments:

    RangeStruct -   Supplies pointer to range structure

    Offset      -   Supplies flat address of breakpoint

    fSet        -   Supplies flag which if true causes the BP to be set

Return Value:

    None

--*/
{
    BREAKPOINT      **BpList;
    ADDR            *BpAddrs;
    DWORD           i;

    assert( RangeStruct );
    assert( Addr );

    //
    //  Add the breakpoint to the list in the range structure
    //
    if ( RangeStruct->BpList ) {
        assert( RangeStruct->BpCount > 0 );
        assert( RangeStruct->BpAddrs );

        //
        //  Do not add duplicates
        //
        for ( i=0; i<RangeStruct->BpCount; i++ ) {
            if ( FAddrsEq( RangeStruct->BpAddrs[i], *Addr ) ) {
                return;
            }
        }

        BpList  = ( BREAKPOINT** )MHRealloc( RangeStruct->BpList, sizeof( BREAKPOINT *) * (RangeStruct->BpCount + 1) );
        BpAddrs = ( ADDR* )MHRealloc( RangeStruct->BpAddrs, sizeof( ADDR ) * (RangeStruct->BpCount + 1) );
    } else {
        assert( RangeStruct->BpCount == 0 );
        assert( RangeStruct->BpAddrs == NULL );
        BpList  = ( BREAKPOINT** )MHAlloc( sizeof( BREAKPOINT * ) );
        BpAddrs = ( ADDR* )MHAlloc( sizeof( ADDR ) );
    }

    assert( BpList );
    assert( BpAddrs );

    BpList[RangeStruct->BpCount]   = NULL;
    BpAddrs[ RangeStruct->BpCount] = *Addr;

    if ( fSet ) {

        BpList[ RangeStruct->BpCount ] =
            SetBP( RangeStruct->hthd->hprc,
                    RangeStruct->hthd,
                    bptpExec,
                    bpnsStop,
                    Addr,
                    (HPID) INVALID
                    );

        assert( BpList[ RangeStruct->BpCount ] );
    }

    RangeStruct->BpCount++;
    RangeStruct->BpList     = BpList;
    RangeStruct->BpAddrs    = BpAddrs;
}


VOID
SetRangeBp(
    RANGESTRUCT *RangeStruct
    )
/*++

Routine Description:

    Sets the breakpoints in the range

Arguments:

    RangeStruct -   Supplies pointer to range structure

Return Value:

    None

--*/

{
    BOOL    BpSet;
    DWORD   Class;
    DWORD   SubClass;

    assert( RangeStruct );

    if ( RangeStruct->fSingleStep ) {
#ifdef TARGET_i386
        Class    =  EXCEPTION_DEBUG_EVENT;
        SubClass =  (DWORD)STATUS_SINGLE_STEP;
#else
        Class    =  BREAKPOINT_DEBUG_EVENT;
        SubClass =  NO_SUBCLASS;
#endif
    } else {
        Class    =  BREAKPOINT_DEBUG_EVENT;
        SubClass =  NO_SUBCLASS;
    }

    //
    //  Register the expected breakpoint event.
    //
    RegisterExpectedEvent(
                          RangeStruct->hthd->hprc,
                          RangeStruct->hthd,
                          Class,
                          SubClass,
                          RangeStruct->Method,
                          NO_ACTION,
                          FALSE,
                          NULL);

    if ( RangeStruct->BpCount ) {

        assert( RangeStruct->BpList );
        assert( RangeStruct->BpAddrs );

        //
        //  Set all the breakpoints at once
        //
        BpSet = SetBPEx( RangeStruct->hthd->hprc,
                        RangeStruct->hthd,
                        (HPID) INVALID,
                        RangeStruct->BpCount,
                        RangeStruct->BpAddrs,
                        RangeStruct->BpList,
                        0
                        //DBG_CONTINUE
                        );

        assert( BpSet );
    }
}

VOID
RemoveRangeBp(
    RANGESTRUCT *RangeStruct
    )
/*++

Routine Description:

    Sets the breakpoints in the range

Arguments:

    RangeStruct -   Supplies pointer to range structure

Return Value:

    None

--*/

{
    BOOL        BpRemoved;

    assert( RangeStruct );

    if ( RangeStruct->BpCount ) {

        assert( RangeStruct->BpList );
        assert( RangeStruct->BpAddrs );

        //
        //  Reset all the breakpoints at once
        //
        BpRemoved = RemoveBPEx( RangeStruct->BpCount,
                                RangeStruct->BpList
                                );
    }
}


#if 0
BOOL
GetThunkTarget(
    HTHDX       hthd,
    RANGESTRUCT *RangeStruct,
    ADDR        *AddrThunk,
    ADDR        *AddrTarget
    )
{
    BOOL    fGotTarget = FALSE;
    BYTE   *Buffer;
    DWORD   BufferSize;
    LONG    Length;
    BYTE   *Instr;
    ADDR    Addr;

    assert( RangeStruct );
    assert( AddrThunk );
    assert( AddrTarget );

#define PADDINGSIZE 4

    BufferSize = 16;
    Buffer     = (BYTE *)MHAlloc( BufferSize + PADDINGSIZE );
    assert( Buffer );
    memset( Buffer + BufferSize, 0, PADDINGSIZE);

    //
    //  Disassemble instructions until a branch is found.
    //
    if ( Buffer ) {

        Addr   = *AddrThunk;

        while ( TRUE ) {

            AddrReadMemory(RangeStruct->hthd->hprc,
                           RangeStruct->hthd,
                           &Addr,
                           Buffer,
                           BufferSize,
                           &Length );
            assert(Length==(LONG)BufferSize);

            //
            //  Unassemble the code and determine where all branches are.
            //
            Instr  = (BYTE *)Buffer;

            while ( Length > 0 ) {

                BOOL    IsBranch;
                BOOL    TargetKnown;
                BOOL    IsCall;
                BOOL    IsTable;
                ADDR    Target;
                DWORD   Consumed;

                //
                //  Unassemble one instruction
                //
                Consumed = BranchUnassemble(hthd,
                                            (void *)Instr,
                                            Length,
                                            &Addr,
                                            &IsBranch,
                                            &TargetKnown,
                                            &IsCall,
                                            &IsTable,
                                            &Target );

                if ( Consumed == 0 ) {
                    //
                    //  Could not unassemble the instruction.
                    //
                    Length = 0;

                } else {

                    //
                    //  If instruction is a branch, this is our guy.
                    //
                    if ( IsBranch ) {

                        if ( ADDR_IS_FLAT(Target) ) {
                            if ( GetAddrOff(Target) != 0 ) {
                                GetAddrSeg(Target) = PcSegOfHthdx(RangeStruct->hthd);
                            }
                        } else {
                            ADDR_IS_REAL(Target) = RangeStruct->hthd->fAddrIsReal;
                        }

                        if ( TargetKnown ) {

                            *AddrTarget = Target;

                        } else
                        if ( FAddrsEq( *AddrThunk, Addr ) ) {

                            UOFF32  NextOffset;

#ifdef TARGET_i386
                            UNREFERENCED_PARAMETER( NextOffset );

                            //
                            //  Dont have the target but we are at
                            //  the jump/call instruction. Will set the
                            //  target to the current address and let
                            //  the caller decide what to do.
                            //

#else
                            //
                            //  We are at the branch instruction, so the
                            //  target can be determined.
                            //
                            NextOffset = GetNextOffset( RangeStruct->hthd, RangeStruct->fStepOver );

                            GetAddrOff( Addr ) = NextOffset;
#endif

                            *AddrTarget = Addr;

                        } else {

                            *AddrTarget = Addr;

                        }

                        fGotTarget = TRUE;
                        goto GetOut;
                    }

                    Instr            += Consumed;
                    GetAddrOff(Addr) += Consumed;
                    Length           -= Consumed;
                }
            }
        }

    GetOut:
        MHFree( Buffer );
    }

    return fGotTarget;
}
#endif // 0




#if 0
void
MethodSmartRangeStep(
    DEBUG_EVENT* de,
    HTHDX hthd,
    DWORD unused,
    RANGESTRUCT* RangeStruct
    )
{
    ADDR    AddrCurrent;
    BOOL    fSingleStep = FALSE;

    assert( de );
    assert( RangeStruct );

    //
    //  Get the current address
    //
    AddrFromHthdx( &AddrCurrent, hthd );

    if ( RangeStruct->fSingleStep ) {
        //
        //  Recover from single step
        //
        RecoverFromSingleStep( &AddrCurrent, RangeStruct );
        fSingleStep = TRUE;
    }

    //
    //  See what we must do now.
    //
    if ( InsideRange( hthd,
                    &RangeStruct->BranchList->AddrStart,
                    &RangeStruct->BranchList->AddrEnd,
                    &AddrCurrent ) ) {


        //
        //  Still inside the range.
        //
        if ( ContinueFromInsideRange( &AddrCurrent, RangeStruct ) ) {
            return;
        }

    } else {

        //
        //  Outside the range
        //
        if ( fSingleStep && RangeStruct->fStepOver && RangeStruct->fInCall ) {
            //
            //  Recovering from a single step, continue.
            //
            RangeStruct->fInCall = FALSE;
            RegisterExpectedEvent(  RangeStruct->hthd->hprc,
                                    RangeStruct->hthd,
                                    BREAKPOINT_DEBUG_EVENT,
                                    NO_SUBCLASS,
                                    RangeStruct->Method,
                                    NO_ACTION,
                                    FALSE,
                                    NULL);

            ThreadContinueDebugEventEx (hthd,
                                      RangeStruct->fSingleStep ?
                                            QT_TRACE_DEBUG_EVENT :
                                            QT_CONTINUE_DEBUG_EVENT
                                    );

            return;
        }
        if ( ContinueFromOutsideRange( &AddrCurrent, RangeStruct ) ) {
            return;
        }
    }

    //
    //  If we get here then we must clean up all the allocated resources
    //  and notify the EM.
    //

    if ( RangeStruct->BpCount > 0 ) {

        assert( RangeStruct->BpList );
        assert( RangeStruct->BpAddrs );

        RemoveRangeBp( RangeStruct );

        MHFree( RangeStruct->BpList );
        MHFree( RangeStruct->BpAddrs );
    }

    assert( RangeStruct->BranchList );
    MHFree( RangeStruct->BranchList );

    assert( RangeStruct->Method );
    MHFree( RangeStruct->Method );

    MHFree( RangeStruct );

    //
    //  Notify the EM that this thread has stopped.
    //
#ifdef KERNEL
    hthd->tstate &= ~ts_stepping;
#endif
    hthd->tstate &= ~ts_running;
    hthd->tstate |=  ts_stopped;
    NotifyEM(&falseSSEvent, hthd, 0, (LPVOID)0);
}
#endif


VOID
RecoverFromSingleStep(
    ADDR        *AddrCurrent,
    RANGESTRUCT* RangeStruct
    )
{
    BREAKPOINT *Bp;
    DWORD       i;

    assert( AddrCurrent );
    assert( RangeStruct );
    assert( RangeStruct->fSingleStep );



    //
    //  Recovering from a single step.
    //  Reset previous BP
    //

    Bp = FindBP( RangeStruct->hthd->hprc,
                 RangeStruct->hthd,
                 bptpExec,
                 (BPNS)-1,
                 &RangeStruct->PrevAddr,
                 FALSE );

    assert( Bp );

    if ( Bp ) {
        WriteBreakPoint( Bp );
    }



#ifdef TARGET_i386

#ifndef KERNEL
    //
    //  Clear trace flag
    //
    //
    HthdReg(RangeStruct->hthd,EFlags) &= ~(TF_BIT_MASK);
    RangeStruct->hthd->fContextDirty = TRUE;

#endif  // KERNEL


#else   // TARGET_i386

    //
    //  Remove temporary breakpoint
    //
    assert( FAddrsEq( RangeStruct->TmpBp->addr, *AddrCurrent ) );

    assert( RangeStruct->TmpBp );
    RemoveBP( RangeStruct->TmpBp );
    RangeStruct->TmpBp = NULL;

#endif  // TARGET_i386

    RangeStruct->fSingleStep = FALSE;
}


BOOL
ContinueFromInsideRange(
    ADDR        *AddrCurrent,
    RANGESTRUCT *RangeStruct
    )
/*++

Routine Description:

Return Value:

Comments:

    ORPC debugging has not yet been implemented for smart range stepping.

--*/
{

    DWORD       i;
    BREAKPOINT *Bp;
    UOFF32      NextOffset;
    ADDR        NextAddr;
    BOOL        fContinue   = FALSE;

    assert( AddrCurrent );
    assert( RangeStruct );
        assert( !RangeStruct -> hthd -> fDisplayReturnValues); // AutoDisplay of return values code should be added to this routine.  See RAID 2914

    Bp = AtBP(RangeStruct->hthd);

    if ( RangeStruct->BranchList->Count > 0 && Bp ) {

        if ( Bp != EMBEDDED_BP ) {

            //
            //  Look for the branch node corresponding to this address.
            //  When found, determine the target address and set a
            //  safety breakpoint there if necessary. Then let the
            //  thread continue.
            //
            for ( i=0; i < RangeStruct->BranchList->Count; i++ ) {

                if ( FAddrsEq( RangeStruct->BranchList->BranchNode[i].Addr,
                                                            *AddrCurrent ) ) {

                    //
                    //  This is our guy.
                    //

                    //
                    //  Determine the next address
                    //
                    RangeStruct->fInCall = RangeStruct->BranchList->BranchNode[i].IsCall;
#ifdef TARGET_i386
                    UNREFERENCED_PARAMETER( NextOffset );
                    UNREFERENCED_PARAMETER( NextAddr );
#else   // TARGET_i386
                    NextOffset = GetNextOffset( RangeStruct->hthd, RangeStruct->fStepOver );
                    AddrInit(&NextAddr, 0, 0, NextOffset, TRUE, TRUE, FALSE, FALSE );
#endif  // TARGET_i386


                    //
                    //  We have to single step the current instruction.
                    //  We set a temporary breakpoint at the next offset,
                    //  recover the current breakpoint and set the flags to
                    //  reset the breakpoint when we hit the temporary
                    //  breakpoint.
                    //
                    ClearBPFlag( RangeStruct->hthd );
                    RestoreInstrBP( RangeStruct->hthd, Bp );
                    RangeStruct->PrevAddr = *AddrCurrent;

                    RangeStruct->fSingleStep = TRUE;
#ifdef TARGET_i386
#ifndef KERNEL
                    HthdReg(RangeStruct->hthd,EFlags) |= TF_BIT_MASK;
                    RangeStruct->hthd->fContextDirty = TRUE;
#endif // KERNEL
#else   // TARGET_i386
                    RangeStruct->TmpAddr  = NextAddr;
                    RangeStruct->TmpBp = SetBP( RangeStruct->hthd->hprc,
                                                RangeStruct->hthd,
                                                bptpExec,
                                                bpnsStop,
                                                &NextAddr,
                                                (HPID) INVALID);
                    assert( RangeStruct->TmpBp );
#endif  // TARGET_i386

                    //
                    //  Register the expected event.
                    //
                    RegisterExpectedEvent(
                                        RangeStruct->hthd->hprc,
                                        RangeStruct->hthd,
#ifdef TARGET_i386
                                        EXCEPTION_DEBUG_EVENT,
                                        (DWORD)STATUS_SINGLE_STEP,
#else   // TARGET_i386
                                        BREAKPOINT_DEBUG_EVENT,
                                        NO_SUBCLASS,
#endif  // TARGET_i386
                                        RangeStruct->Method,
                                        NO_ACTION,
                                        FALSE,
                                        NULL);

                    ThreadContinueDebugEventEx (RangeStruct->hthd,
                                                RangeStruct->fSingleStep ?
                                                    QT_TRACE_DEBUG_EVENT :
                                                    QT_CONTINUE_DEBUG_EVENT
                                                );
                    fContinue = TRUE;
                    break;
                }
            }
        }

    } else {

        //
        //  We might end up here if continuing from a single step.
        //
        RegisterExpectedEvent(
                                RangeStruct->hthd->hprc,
                                RangeStruct->hthd,
                                BREAKPOINT_DEBUG_EVENT,
                                NO_SUBCLASS,
                                RangeStruct->Method,
                                NO_ACTION,
                                FALSE,
                                NULL);

        ThreadContinueDebugEventEx (RangeStruct->hthd,
                                    RangeStruct->fSingleStep ?
                                        QT_TRACE_DEBUG_EVENT :
                                        QT_CONTINUE_DEBUG_EVENT
                                    );

        fContinue = TRUE;
    }

    return fContinue;
}


BOOL
ContinueFromOutsideRange(
    ADDR        *AddrCurrent,
    RANGESTRUCT *RangeStruct
    )
{

    BOOL        fContinue = FALSE;
    ADDR        Addr;
    CANSTEP     CanStep;
    BREAKPOINT *Bp;

    assert( AddrCurrent );
    assert( RangeStruct );
        assert( !RangeStruct -> hthd -> fDisplayReturnValues); // AutoDisplay of return values code should be added to this routine.  See RAID 2914

    Bp = AtBP(RangeStruct->hthd);

    if (Bp == EMBEDDED_BP) {

        //
        // always stop.
        //

    } else if (!RangeStruct->fInCall) {

        //
        // if we werent in a call, this should just be some other line
        // of code in the same function (or the parent function?), so stop.
        //

    } else {

        //
        // stopping after a call instruction.
        // this might actually not be a new function; we just know
        // that there was a call instruction in the source line.
        //

        GetCanStep(RangeStruct->hthd->hprc->hpid,
                   RangeStruct->hthd->htid,
                   AddrCurrent,
                   &CanStep
                   );

        switch ( CanStep.Flags ) {

        case CANSTEP_YES:

            //
            // if there is a known prolog, run ahead to the end
            //
            if ( CanStep.PrologOffset > 0 ) {

                Addr = *AddrCurrent;
                GetAddrOff(Addr) += CanStep.PrologOffset;
                AddRangeBp( RangeStruct, &Addr, TRUE );

                RegisterExpectedEvent(
                    RangeStruct->hthd->hprc,
                    RangeStruct->hthd,
                    BREAKPOINT_DEBUG_EVENT,
                    NO_SUBCLASS,
                    RangeStruct->Method,
                    NO_ACTION,
                    FALSE,
                    NULL);

                ClearBPFlag( RangeStruct->hthd );
                if ( Bp ) {
                    RestoreInstrBP( RangeStruct->hthd, Bp );
                }

                ThreadContinueDebugEvent (RangeStruct->hthd);
                fContinue = TRUE;
            }
            break;

#if 0
        case CANSTEP_THUNK:

            if ( GetThunkTarget( RangeStruct->hthd, RangeStruct, AddrCurrent, &Addr ) ) {

                DWORD   Class;
                DWORD   SubClass;

                if ( FAddrsEq( *AddrCurrent, Addr ) ) {

                    //
                    //  Could not determine target address. Set a single
                    //  step and continue.
                    //

                    RangeStruct->PrevAddr = *AddrCurrent;

#ifdef TARGET_i386
#ifndef KERNEL
                    RangeStruct->hthd->context.EFlags |= TF_BIT_MASK;
                    RangeStruct->hthd->fContextDirty = TRUE;
#endif  // KERNEL
                    Class    = EXCEPTION_DEBUG_EVENT;
                    SubClass = (DWORD)STATUS_SINGLE_STEP;

#else   // TARGET_i386
                    {
                        ADDR         Addr;
                        UOFF32       NextOffset;
                        NextOffset = GetNextOffset( RangeStruct->hthd,
                                                   RangeStruct->fStepOver );
                        if ( NextOffset != 0x00000000 ) {
                            AddrInit( &Addr, 0, 0, NextOffset, TRUE, TRUE, FALSE, FALSE );
                            RangeStruct->TmpAddr = Addr;
                            RangeStruct->TmpBp = SetBP( RangeStruct->hthd->hprc,
                                                        RangeStruct->hthd,
                                                        bptpExec,
                                                        bpnsStop,
                                                        &Addr,
                                                        (HPID) INVALID);
                            assert( RangeStruct->TmpBp );
                        }
                        Class    = BREAKPOINT_DEBUG_EVENT;
                        SubClass = NO_SUBCLASS;
                    }
#endif  // TARGET_i386

                    RegisterExpectedEvent(
                                            RangeStruct->hthd->hprc,
                                            RangeStruct->hthd,
                                            Class,
                                            SubClass,
                                            RangeStruct->Method,
                                            NO_ACTION,
                                            FALSE,
                                            NULL);

                } else {

                    //
                    //  Set breakpoint at target address and continue
                    //
                    AddRangeBp( RangeStruct, &Addr, TRUE );

                    RegisterExpectedEvent(
                                            RangeStruct->hthd->hprc,
                                            RangeStruct->hthd,
                                            BREAKPOINT_DEBUG_EVENT,
                                            NO_SUBCLASS,
                                            RangeStruct->Method,
                                            NO_ACTION,
                                            FALSE,
                                            NULL);
                }

                //
                //  Remove current breakpoint
                //
                ClearBPFlag( RangeStruct->hthd );
                if ( Bp ) {
                    RestoreInstrBP( RangeStruct->hthd, Bp );
                }

                ThreadContinueDebugEventEx (hthd,
                                            RangeStruct->fSingleStep ?
                                                QT_TRACE_DEBUG_EVENT :
                                                QT_CONTINUE_DEBUG_EVENT);
                fContinue = TRUE;
            }

            break;
#endif  // 0

        case CANSTEP_NO:

            //
            // no source here.
            // step some more...
            // what is going to happen here? do we just keep stepping
            // until we get somewhere that has source?
            //

            //
            //  Register the expected event.
            //
            RegisterExpectedEvent(
                RangeStruct->hthd->hprc,
                RangeStruct->hthd,
                BREAKPOINT_DEBUG_EVENT,
                NO_SUBCLASS,
                RangeStruct->Method,
                NO_ACTION,
                FALSE,
                NULL);

            ClearBPFlag( RangeStruct->hthd );
            if ( Bp ) {
                RestoreInstrBP( RangeStruct->hthd, Bp );
            }

            ThreadContinueDebugEventEx (RangeStruct->hthd,
                                        RangeStruct->fSingleStep ?
                                            QT_TRACE_DEBUG_EVENT :
                                            QT_CONTINUE_DEBUG_EVENT);
            fContinue = TRUE;
            break;
        }
    }

    return fContinue;
}

VOID
RangeStepContinue(
        HTHDX   hthd,
        LPVOID  Args
        );


typedef struct _RANGE_STEP_CONTINUE_ARGS
{
        UOFF32  offStart;
        UOFF32  offEnd;
        BOOL    fStopOnBP;
        BOOL    fstepOver;

} RANGE_STEP_CONTINUE_ARGS;

VOID
RangeStep(
    HTHDX       hthd,
    UOFF32      offStart,
    UOFF32      offEnd,
    BOOL        fStopOnBP,
    BOOL        fstepOver
    )
/*++

Routine Description:

    This function is used to implement range stepping the the DM.  Range
    stepping is used to cause all instructions between a pair of addresses
    to be executed.

    The segment is implied to be the current segment.  This is validated
    in the EM.

    Range stepping is done by registering an expected debug event at the
    end of a step and seeing if the current program counter is still in
    the correct range.  If it is not then the range step is over, if it
    is then a new event is register and we loop.

Arguments:

    hthd      - Supplies the thread to be stepped.

    offStart  - Supplies the initial offset in the range

    offEnd    - Supplies the final offset in the range

    fStopOnBP - Supplies TRUE if stop on an initial breakpoint

    fStepOver - Supplies TRUE if to step over call type instructions

Return Value:

    None.

Comments:

    This function has been broken into two.  RangeStep and RangeStepContinue.
    See RangeStepContinue for most of the functionality.

--*/
{
    RANGE_STEP_CONTINUE_ARGS*       Args;

    Args = (RANGE_STEP_CONTINUE_ARGS*) MHAlloc (sizeof (*Args));

    Args->offStart = offStart;
    Args->offEnd = offEnd;
    Args->fStopOnBP = fStopOnBP;
    Args->fstepOver = fstepOver;

    EnsureOleRpcStatus (hthd, RangeStepContinue, Args);
}


VOID
RangeStepContinue(
    HTHDX   hthd,
    LPVOID  Args
    )
/*++

Routine Description:

    See the function RangeStep for a description of the Args fields.  In the
    normal case -- where we are not changing our OLE debugging state -- this
    function is just caled from EnsureOleRpcStatus ().

--*/
{
    RANGE_STEP_CONTINUE_ARGS*   rscArgs = (RANGE_STEP_CONTINUE_ARGS*) Args;

    UOFF32  offStart = rscArgs->offStart;
    UOFF32  offEnd   = rscArgs->offEnd;
    BOOL    fStopOnBP= rscArgs->fStopOnBP;
    BOOL    fstepOver= rscArgs->fstepOver;

    RANGESTEP * rs;
    METHOD *    method;
    HPRCX       hprc = hthd->hprc;
    int         lpf  = 0;
    ADDR        addr;


    MHFree (rscArgs);
    rscArgs = NULL;

    //
    //  Create and fill a range step structure
    //

    rs = (RANGESTEP*) MHAlloc(sizeof(RANGESTEP));
    rs->hthd        = hthd;
    rs->addrStart   = offStart;
    rs->addrEnd     = offEnd;
    rs->segCur      = PcSegOfHthdx(hthd);
    rs->fInThunk    = FALSE;
    rs->safetyBP    = NULL;
    rs->SavedSeg    = 0;
    rs->SavedAddrStart = 0;
    rs->SavedAddrEnd = 0;
    rs->fSkipProlog = FALSE;
    rs->fGetReturnValue = FALSE;
    rs->SavedEBP = 0;

    //
    //  Create a notification method for this range step
    //

    method  = (METHOD*) MHAlloc(sizeof(METHOD));
    method->notifyFunction  = (ACVECTOR)MethodRangeStep;
    method->lparam          = rs;
    rs->method              = method;

    if ( fstepOver ) {
        rs->stepFunction = StepOver;
    } else {
        rs->stepFunction = SingleStep;

       /*
        *  Check to see if we are currently at a call instruction.  If we
        *      are then we need to set a breakpoint at the end of the call
        *      instruction as the "safety" breakpoint.
        *
        *      This will allow us to recover back to the current level
        *      if the call we are just about to step into does not have
        *      any source information (in which case the range step
        *      is defined to continue).
        */

        AddrInit(&addr, 0, rs->segCur, offStart,
                hthd->fAddrIsFlat, hthd->fAddrOff32, FALSE, hthd->fAddrIsReal);
        IsCall( hthd, &addr, &lpf, FALSE);
        if ( lpf == INSTR_IS_CALL ) {
            rs->safetyBP = SetBP(hprc, hthd, bptpExec, bpnsStop, &addr, (HPID)INVALID);
        } else if (lpf == INSTR_CANNOT_STEP) {
            SendDBCErrorStep(hthd->hprc);
            return;
        }
    }

    //
    //  Call the step over function to send notifications
    //  to the RangeStepper (NOT THE EM!)
    //

    (rs->stepFunction)(hthd, method, fStopOnBP, FALSE);

}                               /* RangeStep() */

//#include <simpldis.h>

ULONGLONG
QwGetreg(
	PVOID	pv,
	int		reg
	);

ULONG
GetInstructionSize(
	HTHDX	hthd,
	UOFF32	validAddr,
	UOFF32	destAddr,
	UOFF32*	lpOffset
	);

#if 0
ULONG
GetInstructionSize(
	HTHDX	hthd,
	UOFF32	validAddr,
	UOFF32	destAddr,
	UOFF32*	lpOffset
	)
/*++

Routine Description:

	Get the size of the instruction thisAddr is pointing to.
	
Arguments:

	validAddr - some valid address <= thisAddr.  On x86 we will disasm
				from validAddr to thisAddr to try to get the instruction.

	destAddr - the address we want the size of.  This can be either at
			   the beginning of the instruction or within the instruction.
	

Return Value:

	The size of the instruction.  Zero on failure.

--*/
{

	int			bytes = 0;
	BYTE		buffer [20];
	SIMPLEDIS	sdis;
	UOFF32		disasmAddr = 0;
	ULONG		cb = 0;
	BOOL		fSucc = FALSE;
	

	if (validAddr > destAddr) {
		assert (FALSE);
		return 0;
	}


	
	for (bytes = 0, disasmAddr = validAddr;
		 disasmAddr <= destAddr;
		 disasmAddr += bytes) {
		 

		cb = 0;
		
		fSucc = DbgReadMemory (hthd->hprc,
							   (PVOID) disasmAddr,
							   buffer,
							   sizeof (buffer),
							   &cb);

		assert (fSucc);

		bytes = SimplyDisassemble (buffer,
								   cb,
								   disasmAddr,
								   Simple_Arch_X86,
								   &sdis,
								   NULL,
								   NULL,
								   NULL,
								   QwGetreg,
								   (PVOID) hthd
								   );

		if (bytes <= 0) {

			//
			// Invalid address or instruction.
			//
			
			assert (FALSE);
			return 0;
		}
	}
		

	if (lpOffset) {
		*lpOffset = destAddr - (disasmAddr - bytes);
	}
	
	return bytes;
}
#endif		
	
	
	


VOID
MethodRangeStep(
    DEBUG_EVENT* de,
    HTHDX hthd,
    DWORD unused,
    RANGESTEP* rs
    )
/*++

Routine Description:

    This is the range step method for the "stupid" or single stepping range
    step method.  This is called for every instruction executed during a range
    step, except when it has decided to run free to the safety breakpoint.  It
    does that when the destination code has no source line information.

Arguments:

    de - Supplies the single step or breakpoint debug event

    hthd - Supplies the thread

    unused -

    rs - Supplies a structure containing state information for the step

Return Value:

    none

--*/
{
    DWORD       currAddr    = PC(hthd);
    int         lpf         = 0;
    HPRCX       hprc        = hthd->hprc;
    METHOD *    method;
    ADDR        AddrPC;
    CANSTEP     CanStep;
    PBREAKPOINT bp;
    DWORD       dwSize;
    EXPECTED_EVENT *ee;


    DEBUG_PRINT_3("** MethodRangeStep called: %08x-%08x  PC=%08x",
                    rs->addrStart, rs->addrEnd, currAddr);

	DbgTrace ((TR_RANGESTEP,
			  "MethodRangeStep\n"
			  "\tPC:%#x\n\tStart: %#x\n"
			  "\tEnd:%#x\n",
			  currAddr,
			  rs->addrStart,
			  rs->addrEnd));

    AddrFromHthdx(&AddrPC, hthd);

    //
    //  auto return value

    if (hthd->fReturning) {
        //
        // Out of a call.
        //
        
        hthd->fReturning = FALSE;

        assert (rs->fGetReturnValue);

        //
        // Got it!
        //

        rs->fGetReturnValue = FALSE;

        NotifyEM (&FuncExitEvent, hthd, 0, &hthd->addrFrom);
    }


    //
    //  see if we ran past a call or hit a BP.
    //

    bp = AtBP(hthd);

    if (bp && bp == rs->safetyBP) {

        //
        // We stepped over the function.  continue the range step...
        //

        //
        // Note: we only get here if we did not stop in the called function.
        // In the usual case, the safety BP has been changed into an expected
        // event and is gone by the time we get here.
        //

		//
		// If there is an expected event already dependent on this bp remove
		// it.
		//
		
        ee = PeeIsEventExpected (hthd,
								 BREAKPOINT_DEBUG_EVENT,
								 (DWORD) rs->safetyBP,
								 TRUE);

		if (ee) {
            MHFree (ee);
			ee = NULL;
        }
        RemoveBP(bp);
        rs->safetyBP = NULL;

    } else if (bp) {

        //
        // always stop on a real bp.
        //

        goto EndStep;

    }


    //
    //  Check if we are still within the range
    //

    if ((rs->addrStart > currAddr) ||
            (currAddr > rs->addrEnd) ||
            (PcSegOfHthdx(hthd) != rs->segCur)) {

		//
		// For thunking.
		//

		if (rs->SavedAddrStart) {

			DbgTrace ((TR_RANGESTEP,
					  "MethodRagneStep: [Exited Thunk]\n"
					  "\taddrStart = %#x, setting to %#x\n"
					  "\taddrEnd = %#x, setting to %#x\n",
					  rs->addrStart,
					  rs->SavedAddrStart,
					  rs->addrEnd,
					  rs->SavedAddrEnd));
				  
            rs->addrStart = rs->SavedAddrStart;
            rs->addrEnd = rs->SavedAddrEnd;
            rs->segCur = rs->SavedSeg;
            rs->SavedAddrStart = 0;
            rs->SavedAddrEnd = 0;
            rs->SavedSeg = 0;
			rs->fInThunk = FALSE;
        }
    }

    if ((rs->addrStart <= currAddr) &&
            (currAddr <= rs->addrEnd) &&
            (PcSegOfHthdx(hthd) == rs->segCur)) {

        //
        //  We still are in the range, continue stepping
        //

        //
        //  On Win95 if we try to step into a system call, the system
        //  forces execution to the instruction after the call and sends
        //  a single step.  This is because it doesnt want user-mode
        //  debuggers to step into system code.  In that case we are
        //  already at the safety bp and we should consume it.
        //

#if 0
        if (IsChicago() && rs->safetyBP &&
            (rs->stepFunction != (STEPPER)StepOver || rs->fGetReturnValue))
        {
            bp = FindBP(hprc, hthd, bptpExec, (BPNS)-1, &AddrPC, FALSE);
            if (bp == rs->safetyBP) {
                ee = PeeIsEventExpected(hthd, BREAKPOINT_DEBUG_EVENT,(DWORD) rs->safetyBP, TRUE);
                if(ee) {
                    MHFree(ee);
                }
                RemoveBP(rs->safetyBP);
                rs->safetyBP = NULL;
                if (rs->fGetReturnValue)
                    rs->fGetReturnValue = FALSE;
            }


        }
#endif



        if (rs->stepFunction != (STEPPER)StepOver) {

            //
            //    If we are doing a "Step Into" we must check for "CALL"
            //
            IsCall(hthd, &AddrPC, &lpf, FALSE);
            if (lpf == INSTR_IS_CALL) {

                //
                // Before we step into this function, lets
                // put a "safety-net" breakpoint on the instruction
                // after this call. This way if we dont have
                // source for this function, we can always continue
                // and break at this safety-net breakpoint.
                //

                // this should have been cleared
                assert(!rs->safetyBP);

                rs->safetyBP = SetBP (hprc,
                                      hthd,
                                      bptpExec,
                                      bpnsStop,
                                      &AddrPC,
                                      (HPID)INVALID);

                RegisterExpectedEvent(hprc,
                                  hthd,
                                  BREAKPOINT_DEBUG_EVENT,
                                  (DWORD) rs->safetyBP,
                                  rs->method,
                                  NO_ACTION,
                                  FALSE,
                                  NULL);

				
#if 0
                //
                // If we are doing a step into, we may end up receiving
                // the ORPC ActionOrpcClientGetBufferSize event.
                

                if (hthd->hprc->OrpcDebugging == ORPC_DEBUGGING) {

                    RegisterExpectedEvent (hthd->hprc,
                                           hthd,
                                           OLE_DEBUG_EVENT,
                                           orpcClientGetBufferSize,
                                           DONT_NOTIFY,
                                           ActionOrpcClientGetBufferSize,
                                           FALSE,
                                           (LPVOID) AddrPC.addr.off
                                           );
                }
                
#endif // !KERNEL

            }


        }

        (rs->stepFunction)(hthd, rs->method, TRUE, FALSE);

    } else {

        //
        // we have left the range.
        //
        // If we are at the safety bp && chicago && return values,
        //    we have attempted to step over a system call, failed,
        //    and ended up here.  Remove the safety bp and continue on.
        //
        // If we are at an NLG_RETURN label, we need to continue on.
        //
        // If there is source here,
        //   if we are in a prolog
        //      run to the end of the prolog
        //   else
        //      stop.
        //
        // If we were in a thunk or a call was pending,
        // we have either hit a thunk or a new function.
        //
        //  if we hit a thunk
        //      set the range to cover the thunk,
        //      set the thunk flag
        //      and continue stepping.
        //  else
        //      run to the safety bp
        //
        // if there is no source and no safety bp, stop.
        //

        //
        // Ask the debugger if we can step on this instruction
        //

		DbgTrace ((TR_RANGESTEP,
				  "MethodRangeStep:\n"
				  "\tLeft range at %#x\n", currAddr));
		
        GetCanStep (hthd->hprc->hpid, hthd->htid, &AddrPC, &CanStep);

        if (CanStep.Flags == CANSTEP_YES && !rs->fGetReturnValue) {

			if (rs->safetyBP) {
                ee = PeeIsEventExpected (hthd,
										BREAKPOINT_DEBUG_EVENT,
									    (DWORD) rs->safetyBP,
										TRUE);
                if (ee) {
                    MHFree(ee);
                }
                RemoveBP(rs->safetyBP);
                rs->safetyBP = NULL;
            }
        }

#if 0
        if (IsChicago () && rs->safetyBP && rs->fGetReturnValue) {

			//
            // If this assert fires, you probably want to add
            // its condition to the above conditional.
			//
			
            assert (rs->stepFunction == StepOver);

            bp = FindBP (hprc, hthd, bptpExec, (BPNS)-1, &AddrPC, FALSE);
			
            if (bp == rs->safetyBP) {

				ee = PeeIsEventExpected (hthd,
										BREAKPOINT_DEBUG_EVENT,
										(DWORD) rs->safetyBP,
										TRUE);

				if (ee) {
                    MHFree(ee);
                }
                RemoveBP (rs->safetyBP);
                rs->safetyBP = NULL;
                rs->fGetReturnValue = FALSE;
                goto EndStep;
            }
        }
#endif


        if (CheckNLG(hthd->hprc, hthd, NLG_RETURN, &AddrPC)) {

			//
            // We should have just stepped over a ret instruction,
            // there should be no safety BP's.
			//
			
            assert(rs->safetyBP == NULL);
            SetupNLG(hthd, NULL);
            ThreadContinueDebugEvent (hthd);

        } else if ((CanStep.Flags == CANSTEP_YES) && (rs->fSkipProlog) &&
				   (CanStep.PrologOffset > 0) && !rs->fGetReturnValue ) {

            //
            // If there is a known prolog, run ahead to the end.
            //

            ADDR Addr = AddrPC;

			DbgTrace ((TR_RANGESTEP,
					  "MethodRangeStep:\n"
					  "\tFound a prolog: start %#x end %#x.  Skipping.\n",
					  Addr.addr.off,
					  Addr.addr.off + CanStep.PrologOffset
					  ));
			
            GetAddrOff(Addr) += CanStep.PrologOffset;
            bp = SetBP(hprc, hthd, bptpExec, bpnsStop, &Addr, (HPID)INVALID);
            assert(bp);

            method = (METHOD*)MHAlloc(sizeof(METHOD));
            *method = *rs->method;

            method->lparam2 = (LPVOID)bp;

            RegisterExpectedEvent(
                    hthd->hprc,
                    hthd,
                    BREAKPOINT_DEBUG_EVENT,
                    NO_SUBCLASS,
                    DONT_NOTIFY,
                    (ACVECTOR) SSActionRemoveBP,
                    FALSE,
                    method);

            ThreadContinueDebugEvent (hthd);

        } else if ((CanStep.Flags != CANSTEP_YES) &&
				   (rs->safetyBP || rs->fInThunk) || rs->fGetReturnValue) {

			DbgTrace ((TR_RANGESTEP,
					   "MethodRangeStep: [Came from call or thunk and found no source]\n"));
			
            //
            // We came from a call or thunk, and found no source.
            //

			if (CanStep.Flags == CANSTEP_THUNK_OVER) {


				//
				// There are two cases here:
				//
				// 1) that we are not at the end of the thunk (most likely
				//    at the beginning).  In this case, we set a BP on the
				//	  last instruction of the thunk and GO.
				//
				// 2) we have come back and are now on the last instruction
				//    of the thunk.  We need to treat this like a normal
				//	  thunk step.
				//
				
				ADDR	Addr = AddrPC;
				ULONG	Size = 0;
				UOFF32	Dest = 0;
				UOFF32	Offset = 0;

				_asm int 3
				DbgTrace ((TR_RANGESTEP, "MethodRangeStep: [Thunk Over]\n"));

				Dest = GetAddrOff (AddrPC);
				
				Dest += CanStep.ThunkSize;

				Dest--;		// So we are on the previous instruction

#if 0
				Size = GetInstructionSize (hthd,
										   PC (hthd),
										   Dest,
										   &Offset);
#endif
				Size = 1;

				Dest -= Size - Offset;

				if (Dest <= PC (hthd)) {

					//
					// Normal thunk step
					//
					
					dwSize = Size;
					
					rs->fInThunk = TRUE;
					
					//
					// set new range and continue.
					//

					rs->SavedAddrStart = rs->addrStart;
					rs->SavedAddrEnd = rs->addrEnd;
					rs->SavedSeg = rs->segCur;
					rs->addrStart = currAddr;
					rs->addrEnd = currAddr + dwSize;
					rs->segCur = PcSegOfHthdx(hthd);

					(rs->stepFunction)(hthd, rs->method, TRUE, FALSE);

				} else {

					//
					// Run over this region.
					//

					GetAddrOff (Addr) = Dest;
					bp = SetBP(hprc, hthd,  bptpExec, bpnsStop, &Addr, (HPID)INVALID);
					assert(bp);

					method = (METHOD*)MHAlloc(sizeof(METHOD));
					*method = *rs->method;

					method->lparam2 = (LPVOID)bp;

					RegisterExpectedEvent(
							hthd->hprc,
							hthd,
							BREAKPOINT_DEBUG_EVENT,
							NO_SUBCLASS,
							DONT_NOTIFY,
							(ACVECTOR) SSActionRemoveBP,
							FALSE,
							method);

					ThreadContinueDebugEvent (hthd);
				}

			} else if (IsThunk (hthd, PC(hthd), NULL, NULL, &dwSize) ||
					   CanStep.Flags == CANSTEP_THUNK) {


				if (CanStep.Flags == CANSTEP_THUNK) {

					//
					// The ThunkSize provided by the shell is the actual size
					// of the thunk.  But the range stepper needs an inclusive
					// size.  Subtract one will due.
					//
					
					dwSize = CanStep.ThunkSize - 1;
				}


				rs->fInThunk = TRUE;
				
				//
                // set new range and continue.
                //

                rs->SavedAddrStart = rs->addrStart;
                rs->SavedAddrEnd = rs->addrEnd;
                rs->SavedSeg = rs->segCur;
                rs->addrStart = currAddr;
                rs->addrEnd = currAddr + dwSize;
                rs->segCur = PcSegOfHthdx(hthd);

				DbgTrace ((TR_RANGESTEP,
						  "MethodRangeStep: [Entering Thunk]\n"
						  "\tthunkStart %#x\n"
						  "\tthunkEnd %#x\n"
						  "\tcurRangeStart %#x\n"
						  "\tcurRangeEnd %#x\n"
						  "\tPC %#x\n",
						  rs->addrStart,
						  rs->addrEnd,
						  rs->SavedAddrStart,
						  rs->SavedAddrEnd,
						  currAddr));
						  
                (rs->stepFunction)(hthd, rs->method, TRUE, FALSE);

            } else if (!rs->safetyBP) {

				//
                // came from a thunk, but there was no BP
                //
				
                assert(0);
				
            } else {

				assert (rs->safetyBP);
				DbgTrace ((TR_RANGESTEP,
						   "MetodRangeStep: [Came from call or thunk, no source, safety bp set]\n"
				           "\tMake the safety BP an expected event and run free\n"
						   "\tSafetyBP dest: %#x\n",
						   rs->safetyBP->addr.addr.off));
				
                if (rs->fGetReturnValue) {
					hthd->fReturning = TRUE;
                    SetAddrOff (&hthd->addrFrom, currAddr);
                }

                //
                // Make the safety BP an expected event and run free.
                //

                method = (METHOD*)MHAlloc(sizeof(METHOD));
                *method = *rs->method;

                method->lparam2 = (LPVOID)rs->safetyBP;

				//
                // If there is an expected event already dependent on this bp
				// remove it.
				//
				
                ee = PeeIsEventExpected (hthd,
										 BREAKPOINT_DEBUG_EVENT,
										 (DWORD) rs->safetyBP,
										 TRUE);

				if (ee) {
                   MHFree(ee);
                }

				//
				// SSActionStepOver - does the remove BP when it reaches the
				// right frame
				//
				
                RegisterExpectedEvent(
                                hthd->hprc,
                                hthd,
                                BREAKPOINT_DEBUG_EVENT,
                                (DWORD)rs->safetyBP,
                                DONT_NOTIFY,
                                (ACVECTOR) SSActionStepOver,
                                FALSE,
                                method);

                //
                // The safety is the expected event now, so the rs struct
                // can forget about it.
                //

                rs->safetyBP = NULL;

                ThreadContinueDebugEvent (hthd);
            }
        } else {

EndStep:

            DEBUG_PRINT("  Ending range step\n");

            //
            // We are no longer in the range, free all consumable
            // events on the queue for this thread
            //
            ConsumeAllThreadEvents(hthd, FALSE);

            //
            //  Free the structures created for range-stepping
            //
            if (rs->safetyBP) {
                ee = PeeIsEventExpected(hthd, BREAKPOINT_DEBUG_EVENT,(DWORD) rs->safetyBP, TRUE);
                if(ee) {
                    MHFree(ee);
                }
                RemoveBP(rs->safetyBP);
            }
            MHFree(rs->method);
            MHFree(rs);

            //
            //  Notify the EM that this thread has stopped on a SS
            //
            hthd->tstate &= ~ts_running;
            hthd->tstate |=  ts_stopped;
            NotifyEM(&falseSSEvent, hthd, 0, (LPVOID)0);

        }
    }

    return;
}                   /* MethodRangeStep */


void
WtPrintCallNode(
    LPWTNODE wt
    )
{
    DWORD i;
    static CHAR margin[] =
"                                                                                ";
    i = wt->lex*3;
    if (i > 60) {
        i = 60;
    }
    DMPrintShellMsg( "%4d  %4d  %*.*s%s\r\n",
        wt->icnt,
        wt->scnt,
        i, i, margin,
        wt->fname );
}

void
WtGetSymbolName(
    HTHDX    hthd,
    LPADDR   lpaddr,
    LPSTR   *lpFname,
    LPDWORD  lpdwSymAddress,
    LPDWORD  lpdwReturnAddress
    )
/*++

Routine Description:



Arguments:


Return Value:


--*/
{
    DMSYM   DmSym;

    __try {

        DMSendRequestReply(dbceGetSymbolFromOffset,
                           hthd->hprc->hpid,
                           hthd->htid,
                           sizeof(ADDR),
                           lpaddr,
                           sizeof(DMSYM),
                           &DmSym
                           );

        *lpFname = MHStrdup( DmSym.fname );
        *lpdwSymAddress = GetAddrOff(DmSym.AddrSym);
        *lpdwReturnAddress = DmSym.Ra;

    } __except(EXCEPTION_EXECUTE_HANDLER) {

        *lpFname = NULL;
        *lpdwReturnAddress = 0;
        *lpdwSymAddress = 0;

    }
}


/***    WtMethodRangeStep
**
**  Synopsis:
**
**  Entry:
**
**  Returns:
**
**  Description:
**      This method is called upon the receipt of a single step event
**      while inside of a range step. It checks if the IP is still in the
**      specified range, if it isnt then the EM is notified that the
**      process has stopped outside the range, and all the RS structs and
**      notification method are freed.
*/

void
WtMethodRangeStep(
    DEBUG_EVENT  *de,
    HTHDX        hthd,
    DWORD        unused,
    RANGESTEP    *rs
    )
{
    DWORD       currAddr    = (DWORD)PC(hthd);
    ADDR        addr;
    LPWTNODE    wt;
    LPWTNODE    wt1;
    DWORD       ra;
    DWORD       symaddr;


    AddrInit( &addr, 0, PcSegOfHthdx(hthd), currAddr,
              hthd->fAddrIsFlat, hthd->fAddrOff32, FALSE, hthd->fAddrIsReal );

    hthd->wtcurr->icnt++;

    if (rs->fIsRet) {
        rs->fIsRet = FALSE;
        WtPrintCallNode( hthd->wtcurr );
        if (hthd->wtcurr->caller) {
            wt1 = hthd->wtcurr;
            wt = wt1->caller;
            wt->scnt += wt1->icnt + wt1->scnt;
            wt->callee = NULL;
            hthd->wtcurr = wt;
            MHFree(wt1);
        }
    }


    if (rs->addrEnd == 0 || currAddr == rs->addrEnd || hthd->wtmode == 2) {

        //
        // unwind the stack, print totals
        //

        wt = hthd->wtcurr;
        while (wt) {
            WtPrintCallNode( wt );
            if (wt1 = wt->caller ) {
                wt1->scnt += wt->icnt + wt->scnt;
                MHFree(wt);
            }
            wt = wt1;
        }

    finished:
        hthd->wtmode = 0;
        ConsumeAllThreadEvents(hthd, FALSE);
        MHFree(rs->method);
        MHFree(rs);
        hthd->tstate &= ~ts_running;
        hthd->tstate |=  ts_stopped;
        NotifyEM(&falseSSEvent, hthd, 0, (LPVOID)0);
        return;
    }

    if (rs->fIsCall) {
        LPSTR p;
        //
        // we just completed a call instruction
        // the pc is the first instruction of a new function
        //
        wt = MHAlloc( sizeof(WTNODE) );
        ZeroMemory( wt, sizeof(WTNODE) );

        hthd->wtcurr->callee = wt;
        wt->caller = hthd->wtcurr;
        wt->lex = hthd->wtcurr->lex + 1;
        wt->offset = currAddr;
        wt->sp = (DWORD)STACK_POINTER(hthd);

        WtGetSymbolName( hthd, &addr, &p, &symaddr, &ra );

        if (!p) {
            p = MHAlloc( 16 );
            sprintf( p, "0x%08x", currAddr );
        } else if (symaddr != currAddr) {
            DWORD l = _tcslen(p);
            p = MHRealloc(p, l + 12);
            sprintf(p + l, "+0x%x", currAddr - symaddr);
        }
        wt->fname = p;

        //
        // put new node at head of chain.
        //

        hthd->wtcurr = wt;
    }

    if (STACK_POINTER(hthd) > hthd->wtcurr->sp) {

        //
        // attempt to compensate for unwinds and longjumps.
        // also catches cases that miss the target address.
        //

        //
        // unwind the stack, print totals
        //

        wt = hthd->wtcurr;
        while (wt && STACK_POINTER(hthd) > wt->sp) {
            WtPrintCallNode( wt );
            if (wt1 = wt->caller ) {
                wt1->scnt += wt->icnt + wt->scnt;
                MHFree(wt);
            }
            wt = wt1;
        }
        if (wt) {
            hthd->wtcurr = wt;
        } else {
            hthd->wtcurr = &hthd->wthead;
            goto finished;
        }

    }

    rs->fIsCall = FALSE;

    rs->fIsRet = IsRet(hthd, &addr);

    if (!rs->fIsRet) {
        int CallFlag;
        IsCall( hthd, &addr, &CallFlag, FALSE );
        if (CallFlag == INSTR_IS_CALL) {
            //
            // we are about to trace a call instruction
            //
            rs->fIsCall = TRUE;
            WtPrintCallNode( hthd->wtcurr );
        }
    }

    SingleStep( hthd, rs->method, TRUE, FALSE );

    return;
}                               /* WtMethodRangeStep() */



void
WtRangeStep(
    HTHDX       hthd
    )

/*++

Routine Description:

    This function is used to implement the watch trace feature in the DM.  Range
    stepping is used to cause all instructions between a pair of addresses
    to be executed.

    The segment is implied to be the current segment.  This is validated
    in the EM.

    Range stepping is done by registering an expected debug event at the
    end of a step and seeing if the current program counter is still in
    the correct range.  If it is not then the range step is over, if it
    is then a new event is register and we loop.

Arguments:

    hthd      - Supplies the thread to be stepped.

Return Value:

    None.

--*/

{
    RANGESTEP   *rs;
    METHOD      *method;
    int         CallFlag  = 0;
    ADDR        addr;
    LPSTR       fname;
    DWORD       ra;
    DWORD       symaddr;
    DWORD       instrOff;


    if (hthd->wtmode != 0) {
        DMPrintShellMsg( "wt command already running for this thread\r\n" );
        return;
    }

    AddrInit( &addr, 0, PcSegOfHthdx(hthd), (DWORD)PC(hthd),
              hthd->fAddrIsFlat, hthd->fAddrOff32, FALSE, hthd->fAddrIsReal );
    WtGetSymbolName( hthd, &addr, &fname, &symaddr, &ra );


    //
    //  Create and fill a range step structure
    //
    rs = (RANGESTEP*) MHAlloc(sizeof(RANGESTEP));
    ZeroMemory( rs, sizeof(RANGESTEP) );

    //
    //  Create a notification method for this range step
    //
    method  = (METHOD*) MHAlloc(sizeof(METHOD));
    method->notifyFunction  = (ACVECTOR)WtMethodRangeStep;
    method->lparam          = rs;

    rs->hthd             = hthd;
    rs->segCur           = PcSegOfHthdx(hthd);
    rs->method           = method;
    rs->safetyBP         = NULL;
    rs->stepFunction     = NULL;
    rs->addrStart        = (DWORD)PC(hthd);

    //
    // always tell the watch stepper that the first instruction
    // was a call.  that way, it makes a frame for the place that
    // we are returning to.
    //
    rs->fIsCall          = TRUE;

    hthd->wtcurr         = &hthd->wthead;
    ZeroMemory( hthd->wtcurr, sizeof(WTNODE) );
    hthd->wtcurr->offset = (DWORD)PC(hthd);
    hthd->wtcurr->sp     = (DWORD)STACK_POINTER(hthd);
    hthd->wtcurr->fname  = fname;
    hthd->wtmode         = 1;


    IsCall( hthd, &addr, &CallFlag, FALSE);
    if (CallFlag == INSTR_IS_CALL) {
        ra = GetAddrOff(addr);
    }

    rs->addrEnd = ra;
    DMPrintShellMsg( "Tracing %s to return address %08x\r\n", fname, ra );

    if (CallFlag == INSTR_IS_CALL) {

        //
        // This is a call instruction.  Assume that we
        // want to trace the function that is about to
        // be called.  The call instruction will be the
        // only instruction counted in the current frame.
        //

        //
        //  Call the step over function to send notifications
        //  to the RangeStepper (NOT THE EM!)
        //

        SingleStep(hthd, method, TRUE, FALSE);

    } else {

        //
        // tracing to return address.
        //
        // tell it that we just did a call so that a new
        // frame will be pushed, leaving the current frame
        // to contain this functions caller.
        //

        hthd->wtcurr->icnt = -1;
        WtMethodRangeStep(&falseSSEvent, hthd, 0, rs);
    }

    return;
}                               /* WtRangeStep() */


BOOL
SetupNLG(
    HTHDX hthd,
    LPADDR lpaddr
    )
{
    HNLG hnlg = hnlgNull;
    BOOL fRetVal = FALSE;

    hthd->fStopOnNLG = TRUE;
    while (NULL != (hnlg = LLNext ( hthd->hprc->llnlg, hnlg ))) {

		LPNLG lpnlg = LLLock ( hnlg );

        PBREAKPOINT bp = SetBP( hthd->hprc,
                                hthd,
                                bptpExec,
                                bpnsStop,
                                &lpnlg->addrNLGDispatch,
                                (HPID)INVALID);

		if (bp) {
		
			RegisterExpectedEvent (hthd->hprc,
								   hthd,
								   BREAKPOINT_DEBUG_EVENT,
								   (DWORD)bp,
								   DONT_NOTIFY,
								   ActionNLGDispatch,
								   FALSE,
								   bp);
			fRetVal = TRUE;
		}
								 
        LLUnlock ( hnlg );
    }
	
    if (lpaddr == NULL) {
        SetAddrOff ( &hthd->addrStack, STACK_POINTER(hthd));
    } else {
        SetAddrOff ( &hthd->addrStack, GetAddrOff(*lpaddr) );
    }

	return fRetVal;
}

void
ActionExceptionDuringStep(
    DEBUG_EVENT* de,
    HTHDX hthd,
    DWORD unused,
    LPVOID lpv
    )
/*++

Routine Description:

    If an exception is hit while stepping then ask the EM for the addresses
    of all possible catches and set SS breakpoints thereat.

    Dolphin V3 may go to a new method that does not involve disassembly EH
    registration nodes. See Dolphin 9036 and 8510 for details.

Arguments:

    de       - Current debug event

    hthd     - Thread where debug event occurred

Return Value:

    none

--*/
{
    DWORD subclass = de->u.Exception.ExceptionRecord.ExceptionCode;
    DWORD firstChance = de->u.Exception.dwFirstChance;
    XOSD xosd;
#if !defined(TARGET_i386)
    EXHDLR ExHdlr;
#endif
    EXHDLR *pExHdlr = NULL;

    Unreferenced(lpv);

    if ((subclass == STATUS_SINGLE_STEP) || !firstChance) {
        ProcessExceptionEvent(de, hthd);
        return;
    }
    switch (ExceptionAction(hthd->hprc, subclass)) {
        case efdNotify:
            NotifyEM(de, hthd, 0, NULL);
            break;
        // Dont bother asking for handler addresses
        case efdStop:
        case efdCommand:
            ProcessExceptionEvent(de, hthd);
            return;
    }


#if defined(TARGET_i386)
        assert(lpv != NULL);

    /* Walk the exception registration stack and get the catch locations */

    if ((pExHdlr = GetExceptionCatchLocations(hthd, lpv)) != NULL) {
        xosd = xosdNone;
    } else {
        xosd = xosdGeneral;  /* so we dont instantiate bps below*/
    }

#else   // TARGET_i386
    /* Ask the EM for catch locations */
    xosd = DMSendRequestReply(dbceExceptionDuringStep,
                       hthd->hprc->hpid,
                       hthd->htid,
                       0,
                       NULL,
                       sizeof(EXHDLR),
                       &ExHdlr
                       );
    pExHdlr = &ExHdlr;
#endif  // TARGET_i386


    if (xosd == xosdNone) {
        DWORD i;
        for (i=0; i<pExHdlr->count; i++) {
            PBREAKPOINT bp = SetBP(hthd->hprc,
                                   hthd,
                                   bptpExec,
                                   bpnsStop,
                                   pExHdlr->addr+i,
                                   0);

            if (bp != NULL) {
                METHOD * method = (METHOD *)MHAlloc(sizeof(METHOD));
                method->lparam2 = (LPVOID) bp;
                method->notifyFunction = ConsumeThreadEventsAndNotifyEM;

                RegisterExpectedEvent(hthd->hprc, hthd,
                                      BREAKPOINT_DEBUG_EVENT,
                                      (DWORD)bp,
                                      DONT_NOTIFY,
                                      (ACVECTOR) SSActionRemoveBP,
                                      FALSE,
                                      method);
            }
        }
    }

#if defined(TARGET_i386)
    if (pExHdlr) {
        MHFree(pExHdlr);
    }
#endif  // TARGET_i386

    /* Re-enable myself */
    RegisterExpectedEvent(hthd->hprc, (HTHDX)hthd,
                          EXCEPTION_DEBUG_EVENT,
                          (DWORD)NO_SUBCLASS,
                          DONT_NOTIFY,
                          ActionExceptionDuringStep,
                          FALSE,
                          lpv);
    SetDebugEventThreadState (hthd->hprc, ts_running);

    AddQueue( QT_CONTINUE_DEBUG_EVENT,
              hthd->hprc->pid,
              hthd->tid,
              (DWORD)DBG_EXCEPTION_NOT_HANDLED,
              0);
}

/*** ISINSYSTEMDLL
 *
 * PURPOSE:
 *      Determine if the given uoffDest is in a system DLL
 * INPUT:
 *
 * OUTPUT:
 *     Returns TRUE if uoffDest is in a system dll otherwise FALSE
 *
 * EXCEPTIONS:
 *
 * IMPLEMENTATION:
 *      This function takes a uoffset and determines if it is in the range of
 *      one of the system DLLs by examining the LPIAL list ( Pointer to
 *      invalid address List) that was built up in LoadDll.
 *
 ****************************************************************************/

//extern SYSTEM_INFO SystemInfo;

BOOL
IsInSystemDll (
    UOFFSET uoffDest
    )
{
	return FALSE;
} /* ISINSYSTEMDLL */
