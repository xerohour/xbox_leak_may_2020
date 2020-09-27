/*************************************************************************
**                                                                      **
**                              EVENT.C                                 **
**                                                                      **
**************************************************************************
**                                                                      **
**  This file contains the expected event checker.  When the default    **
**  event handling for a debug event (from the OS) is not the           **
**  desired handling, then an expected event may be registered.         **
**  For every event which occurs the expected event handler checks      **
**  to see if it is the expected event and if so then it will           **
**  cause the action function associated with the expected event        **
**  to be executed.  The default event handler will not be executed     **
**                                                                      **
*************************************************************************/

#include "precomp.h"
#pragma hdrstop

/************************************************************************/


/************************************************************************/

extern EXPECTED_EVENT   masterEE, *eeList;
extern HTHDXSTRUCT      masterTH;
extern HPRCXSTRUCT      masterPR;
extern HTHDX            thdList;
extern HPRCX            prcList;
extern HTHDX            ourHTHD;
extern HPRCX            ourHPRC;
extern DEBUG_EVENT      falseSSEvent;
extern METHOD           EMNotifyMethod;
extern char             lpbBuffer;
extern DDVECTOR         DebugDispatchTable[];
extern CMD_DESC         CommandDispatchTable[];
extern CRITICAL_SECTION csEventList;

/************************************************************************/

/************************************************************************/



PEXPECTED_EVENT
RegisterExpectedEvent(
    HPRCX       hprc,
    HTHDX       hthd,
    DWORD       eventCode,
    DWORD       subClass,
    METHOD    * notifier,
    ACVECTOR    action,
    BOOL        fPersistent,
    LPVOID      lparam
    )
/*++


Routine Description:

    This function is used to register an expected event.  When a registered
    event occurs, the normal dispatcher is not called; instead an optional
    notifier function and an action function are called.

    If an event is marked Persistent, it will not be discarded when another
    event occurs.

    If hthd is supplied, an exact process/thread match is required to score
    a hit.  If hthd is 0 then any thread in hprc will be matched to it.

Arguments:

    hprc        - Supplies descriptor for process to check for expected event

    hthd        - Supplies optional thread to check for expected event

    eventCode   - Supplies event code to check for

    subClass    - Supplies sub-class of event code (for exceptions)

    notifier    - Supplies optional notifier procedure

    action      - Supplies procedure to be called on event

    fPersistent - Supplies flag to mark event as persistent

    lparam      - Supplies Optional data for action routine.

Return Value:

    Address of the event just registered (NULL on failure)

--*/
{
    PEXPECTED_EVENT ee;

    DPRINT(5, ("[REE: DE=%x, SC=%x, HTHD=%x]\n", eventCode, subClass, hthd));

    /* Handy asserts */
    if (eventCode == BREAKPOINT_DEBUG_EVENT) {
        assert(subClass != 0);
    }


    /* Allocate the structure */

    ee = (PEXPECTED_EVENT) MHAlloc(sizeof(EXPECTED_EVENT));
    if (ee == NULL) {
        return NULL;
    }

    /* Attach it to the expected event list */

    EnterCriticalSection(&csEventList);

    ee->next        = eeList->next;
    eeList->next    = ee;

    /* Stuff it */

    ee->hprc        = hprc;
    ee->hthd        = hthd;
    ee->eventCode   = eventCode;
    ee->subClass    = subClass;
    ee->notifier    = notifier;
    ee->action      = action;
    ee->fPersistent = fPersistent;
    ee->lparam      = lparam;

    LeaveCriticalSection(&csEventList);

    return ee;
}                       /* RegisterExpectedEvent() */



PEXPECTED_EVENT
PeeIsEventExpected(
    HTHDX hthd,
    DWORD eventCode,
    DWORD subClass,
    BOOL bRemove
    )
/*++

Routine Description:

    This function will go though the list of expected events and
    return the first event which matches all of the required criteria.
    The event is removed from the list of expected events if bRemove
    is TRUE.

Arguments:

    hthd        - Supplies descriptor for Thread in which the event occured

    eventCode   - Supplies Event code which occured

    subClass    - Supplies sub-class of the event code

    bRemove     - Removes from the expected event list.

Return Value:

    The event if one was located, NULL otherwise.

--*/
{
    PEXPECTED_EVENT    prev=eeList;
    PEXPECTED_EVENT    ee;


    if ((hthd == NULL) && ((eventCode == CREATE_THREAD_DEBUG_EVENT) ||
                           (eventCode == CREATE_PROCESS_DEBUG_EVENT))) {
        return (PEXPECTED_EVENT)NULL;
    }

    EnterCriticalSection(&csEventList);

    //assert(hthd != NULL);

    /* Try to find an event with the given description */


    for (ee = prev->next; ee; prev=prev->next,ee=ee->next) {

        if (((hthd == 0) ||
             (ee->hthd==hthd) ||
             (ee->hprc == hthd->hprc && ee->hthd==(HTHDX)0)) &&
            (ee->eventCode==eventCode) &&
            ((ee->subClass==NO_SUBCLASS) ||
             ee->subClass==subClass)) {

            /* Found it, remove it from the list */

            if (bRemove) {
                prev->next = ee->next;
            }

            LeaveCriticalSection(&csEventList);

            DPRINT(5, ("Event Expected: %x\n", ee));

            /* and return it to the caller */

            return ee;
        }
    }

    LeaveCriticalSection(&csEventList);

    DPRINT(5, ("\n"));

    return (PEXPECTED_EVENT)NULL;
}                                   /* PeeIsEventExpected() */


void
ConsumeAllThreadEvents(
    HTHDX   hthd,
    BOOL    fClearPersistent
    )
/*++

Routine Description:

    This function will go through the list of expected events and
    remove all events found associated with the specified Thread.

Arguments:

    hthd                - Supplies thread descriptor

    fClearPersistent    - If FALSE, events marked "persistent" will
                          not be cleared.

Return Value:

    None

--*/
{
    PEXPECTED_EVENT    prev;
    PEXPECTED_EVENT    ee;
    PEXPECTED_EVENT    eet;

	if (hthd->hprc->hAsyncStopRequest) {
		VERIFY (RemoveTimerEvent (hthd->hprc, hthd->hprc->hAsyncStopRequest));
		hthd->hprc->hAsyncStopRequest = NULL;
	}
		
	if (hthd && hthd->hprc) {
		hthd->hprc->fStepInto = FALSE;
	}

    // It is a little unfortunate that this must go here.

    ClearThreadWalkFlags (hthd);
    
    /* Try to find events for the specified thread */

    EnterCriticalSection(&csEventList);

    prev = eeList;
    for (ee = eeList->next; ee; ee = eet){

        eet = ee->next;

        if (ee->hthd != hthd || (!fClearPersistent && ee->fPersistent)) {

            prev = ee;

        } else {

            /* Found one, remove it from the list */

            prev->next = eet;

            /* Check if it was a breakpoint event*/

            if (ee->eventCode==EXCEPTION_DEBUG_EVENT
                && ee->subClass==EXCEPTION_BREAKPOINT) {

                /* it was a breakpoint event,    */
                /* must free the bp structure    */
                RemoveBP((PBREAKPOINT)ee->lparam);

            } else if ( ee->eventCode==BREAKPOINT_DEBUG_EVENT ) {

                RemoveBP((PBREAKPOINT)ee->subClass);

            }

            /* Free the event structure      */
            MHFree(ee);
        }
    }

    LeaveCriticalSection(&csEventList);

    return;
}


void
ConsumeAllProcessEvents(
    HPRCX   hprc,
    BOOL    fClearPersistent
    )
/*++

Routine Description:

    This function will go through the list of expected events and
    remove all events found associated with the specified Process.

Arguments:

    hprc                - Supplies process descriptor

    fClearPersistent    - If FALSE, events marked "persistent" will
                          not be cleared.

Return Value:

    None

--*/
{
    PEXPECTED_EVENT    prev;
    PEXPECTED_EVENT    ee;
    PEXPECTED_EVENT    eet;

	if (hprc) {
		hprc->fStepInto = FALSE;
	}
	
	if (hprc->hAsyncStopRequest) {
		VERIFY (RemoveTimerEvent (hprc, hprc->hAsyncStopRequest));
		hprc->hAsyncStopRequest = NULL;
	}

	ClearProcessWalkFlags (hprc);
    
    /* Try to find events for the specified prcess */

    EnterCriticalSection(&csEventList);

    prev = eeList;
    for ( ee = prev->next; ee; ee = eet ) {

        eet = ee->next;

        if (ee->hprc != hprc || (!fClearPersistent && ee->fPersistent)) {

            prev = ee;

        } else {

            /* Found one, remove it from the list */

            prev->next = ee->next;

            /* Check if it was a breakpoint event*/

            if (ee->eventCode==EXCEPTION_DEBUG_EVENT
                && ee->subClass==EXCEPTION_BREAKPOINT) {

                /* it was a breakpoint event,    */
                /* must free the bp structure    */
                RemoveBP((PBREAKPOINT)ee->lparam);

            } else if ( ee->eventCode==BREAKPOINT_DEBUG_EVENT ) {

                RemoveBP((PBREAKPOINT)ee->subClass);

            }

            /* Free the event structure      */
            MHFree(ee);
        }
    }

    LeaveCriticalSection(&csEventList);
    return;
}

VOID
ConsumeSpecifiedEvent(
    PEXPECTED_EVENT eeDel
    )
{
    PEXPECTED_EVENT    prev;
    PEXPECTED_EVENT    ee;
    PEXPECTED_EVENT    eet;

    /* Try to find events for the specified prcess */

    EnterCriticalSection(&csEventList);

    prev = eeList;
    for ( ee = prev->next; ee; ee = eet ) {

        eet = ee->next;

        if (ee != eeDel) {

            prev = ee;

        } else {

            /* Found it, remove it from the list */

            prev->next = ee->next;

            /* Check if it was a breakpoint event*/

            if (ee->eventCode==EXCEPTION_DEBUG_EVENT
                && ee->subClass==EXCEPTION_BREAKPOINT) {

                /* it was a breakpoint event,    */
                /* must free the bp structure    */
                RemoveBP((PBREAKPOINT)ee->lparam);

            } else if ( ee->eventCode==BREAKPOINT_DEBUG_EVENT ) {

                RemoveBP((PBREAKPOINT)ee->subClass);

            }

            /* Free the event structure      */
            MHFree(ee);

            break;
        }
    }

    LeaveCriticalSection(&csEventList);
    return;
}


#if DBG

char* szEvents [] = {
        "<None>",
        "Exception",
        "CreateThread",
        "CreateProcess",
        "ExitThread",
        "ExitProcess",
        "LoadDll",
        "UnloadDll",
        "DebugString",
        "Rip",
        "Breakpoint",
        "CheckBreakpoint",
        "SegmentLoad",
&        "DestroyProcess",
        "DestroyThread",
        "AttachDeadlock",
        "Entrypoint",
        "LoadComplete",
        "InputDebugString",
        "Message",
        "MessageSend",
        "FuncExit",
        "Ole",
        "Fiber",
        "Generic",
        "BogusWin95",
        "MAX"
};

char*
szOrpcEvents [] = {
        "Nil",
        "Unrecognized",
        "ClientGetBufferSize",
        "ClientFillBuffer",
        "ServerNotify",
        "ServerGetBufferSize",
        "ServerFillBuffer",
        "ClientNotify"
};

#if 0
char*
OleEventToStr(
    DWORD   eventCode
    )
{
    if (eventCode < 0 || eventCode >= orpcClientNotify) {
        return "<Bad Event>";
    }

    return szOrpcEvents [eventCode];
}
#endif
    
VOID
DumpEventHeader(
    )
{
    DebugPrint ("  Event     Hthd      Class          SubClass\n");
}

char*
EventToStr(
    DWORD   eventCode
    )
{
    if (eventCode < 0 || eventCode >= MAX_EVENT_CODE) {
        return "<Bad Event>";
    }

    return szEvents [eventCode];
}


VOID
DumpBpHeader(
    );

VOID
DumpBp(
    PBREAKPOINT
    );
    
VOID
DumpEventSubClass(
    DWORD   event,
    DWORD   subClass
    )
{
    switch (event) {

#if 0
        case OLE_DEBUG_EVENT:
            DebugPrint ("%s\n", OleEventToStr (subClass));
            break;
#endif

        case BREAKPOINT_DEBUG_EVENT:
#if 0
            DebugPrint ("\n    ");
            DumpBpHeader ();
            DebugPrint ("    ");
            DumpBp ((PBREAKPOINT) subClass);
#endif

            DebugPrint ("bp = %#x; addr = %#x\n",
                        subClass,
                        ((PBREAKPOINT) subClass)->addr.addr.off);
            break;

        default:
            DebugPrint ("\n");
    }
}
                
    

VOID
DumpEvent(
    PEXPECTED_EVENT ee
    )
{
    DebugPrint (" %c%08x  %08x  %-15.15s",
            ee->fPersistent ? '*' : ' ',
            ee,
            ee->hthd,
            EventToStr (ee->eventCode));

    DumpEventSubClass (ee->eventCode, ee->subClass);
}

VOID
DumpEvents(
    )
{
    PEXPECTED_EVENT ee;

    // NOTE: this does not lock the critical section.  Be careful.

    try {
    
        DumpEventHeader ();
        
        for (ee = eeList->next ; ee; ee = ee->next) {
            DumpEvent (ee);
        
        }
    }

    except (EXCEPTION_EXECUTE_HANDLER) {

        DebugPrint ("<Error: exception dumping evenet list>\n");
    }
    

}

#endif

