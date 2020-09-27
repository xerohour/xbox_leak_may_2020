/*++

Copyright(c) 1992  Microsoft Corporation

Module Name:

    Walk.c

Abstract:

    This module contains the support for "Walking".


Author:

    Kent Forschmiedt (kentf) January 1, 1996

Environment:

    Win32, User Mode

--*/

#include "precomp.h"
#pragma hdrstop


//
//  Externals
//

extern DMTLFUNCTYPE DmTlFunc;
extern char         abEMReplyBuf[];
extern DEBUG_EVENT  falseBPEvent;


extern CRITICAL_SECTION csWalk;
extern CRITICAL_SECTION csThreadProcList;


#ifdef HAS_DEBUG_REGS
//
// Initializer defined in dm.h
//
DWORD DebugRegDataSizes [] = DEBUG_REG_DATA_SIZES;
#define NDEBUG_REG_DATA_SIZES  (sizeof(DebugRegDataSizes) / sizeof(*DebugRegDataSizes))

#endif


#define IsDataBp(_bptp) ((_bptp) == bptpDataC || (_bptp) == bptpDataR || \
                         (_bptp) == bptpDataW || (_bptp) == bptpDataW)
                         
//
//  Walk Structure.
//
//  Contains information to perform a walk on a thread.
//
typedef struct _WALK {
    LIST_ENTRY  AllWalkEntry;   //  Member of AllWalkList
    LIST_ENTRY  ThreadWalkEntry;//  Member of hthd->WalkList

    LIST_ENTRY  GroupWalkList;  //  Binding to breakpoints

    HTHDX       hthd;           //  thread
    BPTP        BpType;         //  Breakpoint type
    BOOL        Active;         //  Active flag
    DWORD       GlobalCount;    //  All thread ref count
    DWORD       LocalCount;     //  per-thread ref count

    union {
        struct {
            DWORD       AddrStart;      // Range Begin
            DWORD       AddrEnd;        // Range End
            PBREAKPOINT StartBP;        // BP on range entry
            DWORD       SavedSP;        // Saved Stack Pointer
            BOOL        fInRange;       // Whether we were in range (last step)
        } Range;

        struct {
            DWORD       DataAddr;       // Data Address
            DWORD       DataSize;       // Data Size
            PVOID       DataContents;   // For change detection
        } Data;
    } u;

    METHOD      Method;         //  Walk method
    int         Register;

} WALK;
typedef struct _WALK      *PWALK;

//
// This node binds a walk structure to one or more breakpoints.
//
// One of these nodes will be allocated for each binding of
// a WALK to a breakpoint.
//
typedef struct _WALK_GROUP_ENTRY {

    //
    // Here is the list of walks that belongs to a breakpoint.
    // This is a list of WALK_GROUP_ENTRY nodes; all nodes on
    // this list represent the same breakpoint, each represents
    // a different WALK.
    //
    
    LIST_ENTRY  WalksInGroup;

    //
    // Here is the list of breakpoints which are using this walk.
    // All WALK_GROUP_ENTRY nodes in this list refer to the same
    // WALK, while each refers to a different breakpoint.
    //

    LIST_ENTRY  GroupsUsingWalk;

    //
    // Here is the walk associated with this node:
    //

    PWALK Walk;

} WALK_GROUP_ENTRY, *PWALK_GROUP_ENTRY;


//
// This is the data that gets propigated through MethodWalk.
//

typedef struct _WALK_INFO {
    BREAKPOINT*     SafetyBP;
} WALK_INFO, *PWALK_INFO;



//
//  Local variables
//
LIST_ENTRY  AllWalkList;


PWALK
SetWalkForThread(
    HPRCX   hprc,
    HTHDX   hthd,
    DWORD   Addr,
    DWORD   Size,
    BPTP    BpType,
    BOOL    Global,
    BOOL    fEmulate
    );

BOOL
RemoveWalkThread (
    HPRCX hprc,
    HTHDX hthd,
    DWORD Addr,
    DWORD Size,
    BPTP  BpType,
    BOOL
    );

BOOL
SetupWalk(
    PWALK Walk,
    BOOL Continuing
    );

PWALK
AllocateWalk(
    HTHDX hthd,
    DWORD Addr,
    DWORD Size,
    BPTP BpType,
    BOOL fEmulate
    );

BOOL
DeallocateWalk(
    PWALK
    );

BOOL
RemoveWalkEntry(
    PWALK Walk,
    BOOL Global
    );

PWALK
FindWalk (
    HTHDX hthd,
    DWORD Addr,
    DWORD Size,
    BPTP BpType
    );

PWALK
FindWalkForHthd(
    HANDLE hWalk,
    HTHDX hthd
    );

PBREAKPOINT
FindBpForWalk(
    PVOID pWalk
    );

int
MethodWalk(
    DEBUG_EVENT*,
    HTHDX,
    DWORD,
    PWALK_INFO
    );

VOID
AddWalkToGroupList(
    PLIST_ENTRY GroupList,
    PWALK Walk
    );

VOID
RemoveWalkBindings(
    PWALK Walk
    );

VOID
DuplicateWalkBindings(
    PWALK OldWalk,
    PWALK NewWalk
    );

BOOL
InRange(
    UOFFSET LowRange,
    UOFFSET HighRange,
    UOFFSET Pc
    );

BOOL
KuFreezeThread(
    HTHDX   hthd,
    BOOL*   Froze
    );

BOOL
KuThawThread(
    HTHDX   hthd
    );

int
DrAllocateDebugRegister(
    HTHDX   hthd,
    DWORD   Addr,
    DWORD   Size,
    DWORD   BpType
    );

BOOL
DrDeallocateDebugRegister(
    HTHDX   hthd,
    int     Register
    );

BOOL
DrIsDebugRegisterInUse(
    HTHDX   hthd,
    int     Register
    );

BOOL
DrSetupDebugRegister(
    HTHDX   hthd,
    int     Register
    );

VOID
DrClearDebugRegister(
    HTHDX   hthd,
    int     Register
    );

VOID
DrSetDebugRegisterInUse(
    HTHDX   hthd,
    int     Register,
    BOOL    InUse
    );

ULONG
DrGetDebugRegisterDataSize(
    HTHDX   hthd,
    int     Register
    );


BREAKPOINT*
SetupSafetyBp(
    HTHDX       hthd,
    METHOD*     Mehtod
    );


//*******************************************************************
//
//                      Exported Functions
//
//******************************************************************


VOID
ExprBPInitialize(
    VOID
    )
{
    InitializeListHead (&AllWalkList);
}


VOID
ExprBPCreateThread(
    HPRCX   hprc,
    HTHDX   hthd
    )
/*++

Routine Description:

    If global walking, adds walk to new thread. Called when a
    new thread is created.

Arguments:

    hprc    -   Supplies process

    hthd    -   Supplies thread

Return Value:

    None
--*/

{
    PWALK       Walk;
    PLIST_ENTRY List;
    HTHDX       hthdT;
    PWALK       twalk;
    DWORD       Addr;
    DWORD       Count;
    BOOL        fEmulate = FALSE;

    //
    //  If there are global walks, set them in this thread
    //

    //
    //  Get a walk list from any thread in this process and
    //  traverse it, copying any global walks.  Note that we
    //  can use any walk list because global walks are common
    //  to all threads.
    //

    EnterCriticalSection (&csWalk);

    EnterCriticalSection (&csThreadProcList);
    hthdT = hprc->hthdChild;

    while (hthdT && hthdT == hthd) {
        hthdT = hthdT->nextSibling;
    }

    LeaveCriticalSection (&csThreadProcList);

    if (hthdT) {

        List = hthdT->WalkList.Flink;

        while (List != &hthdT->WalkList) {
            Walk = CONTAINING_RECORD (List, WALK, ThreadWalkEntry);
            List = List->Flink;

            if ( Walk->GlobalCount > 0 ) {

                if (Walk->BpType == bptpRange) {

                    Addr = Walk->u.Range.AddrStart ;
                    Count = Walk->u.Range.AddrEnd - Addr;

                } else {

                    Addr = Walk->u.Data.DataAddr;
                    Count = Walk->u.Data.DataSize;

                    // If the one we are duping is eumlated, emulate this
                    // one also
                    
                    if (Walk->Register == -1) {
                        fEmulate = TRUE;
                    }
                }
                    

                    
                twalk = SetWalkForThread (hprc,
                                          hthd,
                                          Addr,
                                          Count,
                                          Walk->BpType,
                                          TRUE,
                                          fEmulate
                                          );
                //
                // bind the new walk record the same as the old:
                //
                //
                
                DuplicateWalkBindings (Walk, twalk);

            }
        }
    }

    LeaveCriticalSection(&csWalk);
}


VOID
ExprBPExitThread (
    HPRCX   hprc,
    HTHDX   hthd
    )
/*++

Routine Description:

    Removes walk in a thread, called when the thread is gone.

Arguments:

    hprc    -   Supplies process

    hthd    -   Supplies thread

Return Value:

    None

--*/

{
    PLIST_ENTRY List;
    PWALK       Walk;
    DWORD       GlobalCount;
    DWORD       LocalCount;

    EnterCriticalSection (&csWalk);

    List = hthd->WalkList.Flink;

    while ( List != &hthd->WalkList ) {

        Walk = CONTAINING_RECORD(List, WALK, ThreadWalkEntry);
        List = List->Flink;

        GlobalCount = Walk->GlobalCount;
        LocalCount  = Walk->LocalCount;
        while ( GlobalCount-- ) {
            RemoveWalkEntry (Walk, TRUE);
        }
        while ( LocalCount-- ) {
            RemoveWalkEntry (Walk, FALSE);
        }
    }

    LeaveCriticalSection (&csWalk);
}


VOID
ExprBPContinue(
    HPRCX   hprc,
    HTHDX   hthd
    )
/*++

Routine Description:

    Continues walking. Called as a result of a continue command.

Arguments:

    hprc    -   Supplies process

    hthd    -   Supplies thread

Return Value:

    None

--*/

{
    PWALK       Walk;
    PLIST_ENTRY List;


    if (!hthd) {
        return;
    }
    
    //
    //  See if we have a walk on the thread
    //

    EnterCriticalSection (&csWalk);

    List = hthd->WalkList.Flink;
    while (List != &hthd->WalkList) {

        Walk = CONTAINING_RECORD (List, WALK, ThreadWalkEntry);
        List = List->Flink;

        if (!Walk->Active) {
            SetupWalk (Walk, TRUE);
        }
    }
    
    LeaveCriticalSection (&csWalk);
}



VOID
ExprBPResetBP(
    HTHDX hthd,
    PBREAKPOINT bp
    )
/*++

Routine Description:

    After stepping off of a hardware BP, reset debug register(s)
    before continuing.

Arguments:

    hthd - Supplies the thread which has been stepped.

    bp - Supplies the BREAKPOINT

Return Value:

    none

--*/
{
    PWALK   Walk;

    Walk = FindWalkForHthd (bp->hWalk, hthd);
    
    if (Walk->Register >= 0) {
        assert (DrIsDebugRegisterInUse (hthd, Walk->Register));
        DrSetupDebugRegister (hthd, Walk->Register);
    }

}



VOID
ExprBPClearBPForStep(
    HTHDX hthd
    )
/*++

Routine Description:

    Turn off a hardware breakpoint to allow a single step to occur.
    This is necessary for x86 exec breakpoints, but not for data read/write.

Arguments:

    hthd - Supplies the thread which is going to be stepped.

Return Value:

    none

--*/
{
    BREAKPOINT* bp;
    PWALK       Walk;

    bp = AtBP (hthd);
    assert (bp);

    assert (bp->hWalk);

    Walk = FindWalkForHthd (bp->hWalk, hthd);

    if (Walk->Register >= 0) {
        assert (DrIsDebugRegisterInUse (hthd, Walk->Register));
        DrClearDebugRegister (hthd, Walk->Register);
    }
}


void
ExprBPRestoreDebugRegs(
    HTHDX   hthd
    )
/*++

Routine Description:

    Restore the CPU debug registers to the state that we last put
    them in.  This routine is needed because the system trashes
    the debug registers after initializing the DLLs and before the
    app entry point is executed.

Arguments:

    hthd    - Supplies descriptor for thread whose registers need fixing.

Return Value:

    None

--*/
{
#ifndef HAS_DEBUG_REGS

    Unreferenced (hthd);

#else

    PWALK       Walk;
    PLIST_ENTRY List;

    EnterCriticalSection (&csWalk);

    List = hthd->WalkList.Flink;

    while (List != &hthd->WalkList) {

        Walk = CONTAINING_RECORD(List, WALK, ThreadWalkEntry);
        List = List->Flink;

        if (Walk->Active && Walk->Register >= 0 &&
            DrIsDebugRegisterInUse (hthd, Walk->Register)) {

            DrSetupDebugRegister (hthd, Walk->Register);
        }
    }
    LeaveCriticalSection (&csWalk);

#endif
}



HANDLE
GetNewWalk(
    HPRCX   hprc,
    HTHDX   hthd,
    DWORD   Addr,
    DWORD   Size,
    DWORD   BpType,
    BOOL    fEmulate
    )
/*++

Routine Description:

    Sets up a walk.  Returns a handle which may be used to associate this
    walk with a breakpoint structure.

Arguments:

    hprc    -   Supplies process

    hthd    -   Supplies thread

    Addr    -   Supplies address

    Size    -   Supplies size of memory to watch

    BpType  -   Supplies type of breakpoint

Return Value:

    A handle to the new list of walks

--*/

{

    PWALK       Walk;
    PLIST_ENTRY GroupList = NULL;

    if ( !hprc ) {
        return NULL;
    }

    //
    // If a thread is specified, we use that specific thread,
    // otherwise we must set the walk in all existing threads,
    // plus we must set things up so that we walk all future
    // threads too (while this walk is active).
    //
    
    if (hthd) {

        //
        // This is for a per-thread walk.
        //
        
        Walk = SetWalkForThread (hprc,
                                 hthd,
                                 Addr,
                                 Size,
                                 BpType,
                                 FALSE,
                                 fEmulate);

        if (Walk) {
            GroupList = MHAlloc (sizeof (LIST_ENTRY));
            InitializeListHead (GroupList);
            AddWalkToGroupList (GroupList, Walk);
        }

    } else {

        //
        // This is for a walk in all thread (Global walk); the more normal
        // case.
        //
        
        GroupList = MHAlloc (sizeof (LIST_ENTRY));
        InitializeListHead (GroupList);

        EnterCriticalSection (&csThreadProcList);

        for ( hthd = (HTHDX)hprc->hthdChild;
              hthd;
              hthd = hthd->nextSibling ) {

            Walk = SetWalkForThread (hprc,
                                     hthd,
                                     Addr,
                                     Size,
                                     BpType,
                                     TRUE,
                                     fEmulate);

            if (Walk) {
                AddWalkToGroupList (GroupList, Walk);
            }
        }

        LeaveCriticalSection (&csThreadProcList);

        if (IsListEmpty (GroupList)) {
            MHFree (GroupList);
            GroupList = NULL;
        }
    }

    return (HANDLE) GroupList;
}


BOOL
RemoveWalk(
    HANDLE hWalk,
    BOOL Global
    )

/*++

Routine Description:

    Remove a group of walks.

Arguments:


Return Value:


--*/

{
    PLIST_ENTRY GroupListHead = (PLIST_ENTRY)hWalk;
    PLIST_ENTRY List;
    PWALK_GROUP_ENTRY Entry;
    PWALK Walk;

    List = GroupListHead->Flink;

    while (List != GroupListHead) {
        Entry = CONTAINING_RECORD(List, WALK_GROUP_ENTRY, WalksInGroup);
        List = List->Flink;
        Walk = Entry->Walk;

        RemoveWalkEntry(Walk, Global);
    }

    //
    // The list head pointed to by GroupList will be freed
    // when the last entry is deleted by DeallocateWalk.
    //

    return TRUE;
}



BOOL
RemoveWalkEntry(
    PWALK   Walk,
    BOOL    fGlobal
    )
{
    HTHDX   hthd = Walk->hthd;
    BOOL    Froze = FALSE;
    BOOL    Ok;

    Ok = KuFreezeThread (hthd, &Froze);

    //
    //  Remove the walk
    //

    fGlobal ? Walk->GlobalCount-- : Walk->LocalCount--;

    if (Walk->GlobalCount == 0 && Walk->LocalCount  == 0) {

        //
        //  If the walk is active, the method will eventually
        //  be called. Otherwise we must call the method
        //  ourselves.
        //

        if (!Walk->Active) {
            MethodWalk (NULL, hthd, 0, hthd->WalkData);
        }
    }

    if (Froze) {
        KuThawThread (hthd);
    }

    return TRUE;
}


PBREAKPOINT
GetWalkBPFromBits(
    HTHDX   hthd,
    DWORD   bits
    )
{
    //
    // NOTE: this function is x86 only.
    //
    
#ifndef HAS_DEBUG_REGS
    return NULL;
#else

    PWALK       Walk;
    PLIST_ENTRY List;
    PBREAKPOINT bp = NULL;

    EnterCriticalSection (&csWalk);

    //
    // This only finds the first match.  If more than one BP was
    // matched by the CPU, we won't notice.
    //

    List = hthd->WalkList.Flink;

    while (List != &hthd->WalkList) {
        Walk = CONTAINING_RECORD (List, WALK, ThreadWalkEntry);
        List = List->Flink;

        if ( Walk->Register >= 0 &&
             DrIsDebugRegisterInUse (hthd, Walk->Register)) {

            if (bits & (1 << Walk->Register)) {
                // hit!
                bp = FindBpForWalk (Walk);
                break;
            }
        }
    }

    LeaveCriticalSection (&csWalk);

    return bp;
#endif
}


BOOL
InRange(
    UOFFSET LowRange,
    UOFFSET HighRange,
    UOFFSET Pc
    )
{
    return (LowRange <= Pc && Pc < HighRange);
}



BOOL
CheckWalk(
    PWALK Walk
    )
/*++

Routine Description:

    This decides whether a data or range breakpoint should fire.

    Current implementation handles:

        Data change, emulated or implemented on hardware write BP
        Range BP

    Not handled:

        Emulated data read/write BP

Arguments:

    Walk - Supplies the thread and breakpoint info

Return Value:

    TRUE if the breakpoint should fire, FALSE if it should be ignored.
    This implementation is conservative; unhandled cases always return TRUE.

--*/
{
    PVOID Data;
    DWORD dwSize;
    BOOL ret = TRUE;

    if (Walk->BpType == bptpDataC) {
        if (Walk->u.Data.DataContents && (Data = MHAlloc(Walk->u.Data.DataSize))) {
            if (DbgReadMemory(Walk->hthd->hprc,
                              (PVOID)Walk->u.Data.DataAddr,
                              Data,
                              Walk->u.Data.DataSize,
                              &dwSize)) {
                ret = (memcmp(Data, Walk->u.Data.DataContents, Walk->u.Data.DataSize) != 0);
            }
            MHFree(Data);
        }
    }
    else if (Walk->BpType == bptpRange) {

        ret = !InRange (Walk->u.Range.AddrStart,
                        Walk->u.Range.AddrEnd,
                        PC (Walk->hthd));
    }

    return ret;
}


BOOL
CheckDataBP(
    HTHDX hthd,
    PBREAKPOINT Bp
    )
/*++

Routine Description:

    This decides whether a breakpoint should fire.  If it is not
    a data breakpoint, it should fire.  If it is a data breakpoint,
    decide whether it has been satisfied.

Arguments:

    hthd - Supplies the thread that stopped

    Bp   - Supplies the breakpoint that was hit

Return Value:

    TRUE if the breakpoint has really fired, FALSE if it should be ignored.

--*/
{
    PWALK Walk;

    assert (hthd);
    assert (Bp);

    if (!Bp->hWalk) {
        return TRUE;
    }

    Walk = FindWalkForHthd (Bp->hWalk, hthd);

    assert (Walk);

    if (!Walk) {
        return TRUE;
    }

    return CheckWalk(Walk);
}

//*******************************************************************
//
//                      Local Functions
//
//******************************************************************

BOOL
KuFreezeThread(
    HTHDX   hthd,
    BOOL*   Froze
    )
{
	return TRUE;
#if 0
#ifdef KERNEL
    return TRUE;
#else

    if ( !(hthd->tstate & ts_running) || (hthd->tstate & ts_frozen) ) {
        return TRUE;
    } else if (SuspendThread (hthd->rwHand) != -1L) {
        if (Froze) {
            *Froze = TRUE;
        }

        hthd->tstate |= ts_frozen;
        hthd->context.ContextFlags = CONTEXT_CONTROL;
        DbgGetThreadContext (hthd, &hthd->context);
    } else {
        return FALSE;
    }

    return TRUE;

#endif
#endif
}

BOOL
KuThawThread(
    HTHDX   hthd
    )
{
	return TRUE;
#if 0
#ifdef KERNEL
    return TRUE;
#else

    assert (hthd->tstate & ts_frozen);
    
    if (!ResumeThread (hthd->rwHand)) {
        return FALSE;
    }

    hthd->tstate &= ~ts_frozen;
    return TRUE;

#endif
#endif
}


PWALK
SetWalkForThread(
    HPRCX   hprc,
    HTHDX   hthd,
    DWORD   Addr,
    DWORD   Size,
    DWORD   BpType,
    BOOL    Global,
    BOOL    fEmulate
    )
/*++

Routine Description:

    Sets up a walk in a specific thread

Arguments:

    hprc    -   Supplies process

    hthd    -   Supplies thread

    Addr    -   Supplies address

    Size    -   Supplies Size

    BpType  -   Supplies type (read, read/write, change, exec, range)

    Global  -   Supplies global flag

Return Value:

    BOOL    -   TRUE if Walk set

--*/

{
    PWALK   Walk            = NULL;
    BOOL    AllocatedWalk   = FALSE;    
    BOOL    Ok              = FALSE;
    BOOL    Froze           = FALSE;


    Walk = FindWalk (hthd, Addr, Size, BpType);

    if (Walk) {

        //
        //  If the walk is already active, just increment the
        //  reference count and we're done.
        //

        //
        // If it is not active, fall through and activate it
        //

        if (Walk->Active) {
            Global ? Walk->GlobalCount++ : Walk->LocalCount++;
            Ok = TRUE;
            goto Done;
        }

    } else {

        //
        //  Allocate a walk for this thread.
        //

        if ( Walk = AllocateWalk( hthd, Addr, Size, BpType, fEmulate) ) {
            AllocatedWalk = TRUE;
        } else {
            goto Done;
        }
    }

    //
    //  We have to freeze the specified thread in order to get
    //  the current address.
    //
    
    Ok = KuFreezeThread (hthd, &Froze);
    
    if (Ok) {

        //
        //  Increment reference count
        //

        Global ? Walk->GlobalCount++ : Walk->LocalCount++;

        Ok = SetupWalk (Walk, FALSE);

        
        //
        //  Resume the thread if we froze it.
        //
        
        if (Froze) {
            KuThawThread (hthd);
        }
    }

Done:

    if (!Ok) {
        if (Walk && AllocatedWalk) {
            DeallocateWalk (Walk);
        }
        Walk = NULL;
    }

    return Walk;
}




BOOL
RemoveWalkThread (
    HPRCX   hprc,
    HTHDX   hthd,
    DWORD   Addr,
    DWORD   Size,
    BPTP    BpType,
    BOOL    Global
    )
/*++

Routine Description:

    Removes a walk in a specific thread

Arguments:

    hprc    -   Supplies process

    hthd    -   Supplies thread

    Addr    -   Supplies address

    Size    -   Supplies Size

    BpType  -   Supplies breakpoint type

    Global  -   Supplies global flag

Return Value:

    BOOL    -   TRUE if Walk removed

--*/

{
    PWALK       Walk;

    if (Walk = FindWalk (hthd, Addr, Size, BpType)) {
        return RemoveWalkEntry (Walk, Global);
    } else {
        return FALSE;
    }
}



BOOL
SetupWalk(
    PWALK   Walk,
    BOOL    Continuing
    )
/*++

Routine Description:

    Starts walking.

Arguments:

    Walk    -   Supplies the walk sructure

    Continuing - Supplies a flag saying that the thread is being continued

Return Value:

    BOOL    -   TRUE if done

--*/
{
    BREAKPOINT* bp;
    ACVECTOR    action  = NO_ACTION;
    HTHDX       hthd = Walk->hthd;
    DWORD       dwSize;
    BOOL        fSucc;
    PWALK_INFO  WalkInfo = (PWALK_INFO) hthd->WalkData;

    if (!(hthd->tstate & ts_stopped) || Continuing) {

        if (Walk->BpType == bptpDataC) {

            //
            // Remember contents for change detection.
            //

            if (Walk->u.Data.DataContents) {
                MHFree (Walk->u.Data.DataContents);
            }
            
            Walk->u.Data.DataContents = MHAlloc (Walk->u.Data.DataSize);

            //
            // If we can not read the data, just fire on any write
            //

            fSucc = DbgReadMemory (hthd->hprc,
                                   (PVOID)Walk->u.Data.DataAddr,
                                   Walk->u.Data.DataContents,
                                   Walk->u.Data.DataSize,
                                   &dwSize);

            if (!fSucc) {
                MHFree (Walk->u.Data.DataContents);
                Walk->u.Data.DataContents = NULL;
            }
        }

        if (Walk->Register >= 0) {

            // check whether the register is currently being used ?? ?
            
            fSucc = DrSetupDebugRegister (hthd, Walk->Register);
            assert (fSucc);
            Walk->Active = TRUE;
            DrSetDebugRegisterInUse (hthd, Walk->Register, TRUE);

        } else {


            //
            // RangeBP within range or an emulated DataBP.
            //

            if (Walk->BpType == bptpRange) {


                if (Walk->u.Range.SavedSP == 0) {
                        //
                        // This really should not happen, but sometimes
                        // if we start on a call instruction
                        // ProcessContinueCmd () gives us control only after
                        // the call.  Hence we may potentially have not
                        // initialized SP.  If we note the SP here, it will
                        // do.
                        //

                        Walk->u.Range.SavedSP = STACK_POINTER (hthd);
                    }
                    


                if (InRange (Walk->u.Range.AddrStart,
                             Walk->u.Range.AddrEnd,
                             PC (hthd))) {

                    //
                    // We are currently in Range.
                    //

                    //
                    // Setup the safety BP in case we run too far.
                    //
                
                    if (WalkInfo->SafetyBP == NULL) {
                        WalkInfo->SafetyBP = SetupSafetyBp (hthd,
                                                            &Walk->Method);
                    }
                
                }
            }
                    

            //
            // If the thread is sitting on a BP, the step off of BP code
            // does its thing first, then the walk method gets control.
            //


            bp = AtBP (hthd);

            if (!bp) {

                Walk->Active = TRUE;

                //
                //  Setup a single step
                //
                
                if (!SetupSingleStep (hthd, FALSE,TRUE)) {
                    return FALSE;
                }

                //
                //  Place a single step on our list of expected events.
                //
                
                if (!(hthd->tstate & ts_stopped) || Continuing) {
                    RegisterExpectedEvent (hthd->hprc,
                                           hthd,
                                           EXCEPTION_DEBUG_EVENT,
                                           (DWORD)EXCEPTION_SINGLE_STEP,
                                           &(Walk->Method),
                                           action,
                                           FALSE,
                                           NULL);
                }
            }

        }

        //
        // This code is unnecessary (wrong?) because the shell does the
        // gating breakpoint stuff.
        //
        
#if 0
        else {

            //
            // Range BP, current IP is out of range.
            //

            //
            // This implementation only works reliably for function
            // scoped range BPs.  If a range has an entry point other
            // than its lowest address, it will fail to activate the
            // range stepper on entry.
            //

            ADDR        bpAddr;
            BREAKPOINT* pbp;

            WalkInfo->fInRange = FALSE;
            WalkInfo->oldSP = STACK_POINTER (hthd);
            WalkInfo->SafetyBP = NULL;
            
            AddrFromHthdx (&bpAddr, hthd);
            bpAddr.addr.off = Walk->u.Range.AddrStart;

            pbp = FindBP (Walk->hthd->hprc,
                          Walk->hthd,
                          bptpExec,
                          bpnsStop,
                          &bpAddr,
                          TRUE);


            if (!pbp) {
                METHOD* method;

                method = (METHOD*) MHAlloc (sizeof (METHOD));
                
                *method = Walk->Method;
                
                Walk->u.Range.StartBP = SetBP (Walk->hthd->hprc,
                                               Walk->hthd,
                                               bptpExec,
                                               bpnsStop,
                                               &bpAddr,
                                               (HPID)INVALID);
                                      
                method->lparam2 = (LPVOID) Walk->u.Range.StartBP;

                RegisterExpectedEvent (Walk->hthd->hprc,
                                       Walk->hthd,
                                       BREAKPOINT_DEBUG_EVENT,
                                       (DWORD) Walk->u.Range.StartBP,
                                       DONT_NOTIFY,
                                       SSActionRemoveBP,
                                       FALSE,
                                       method);
            }
        }
#endif
    }

    return TRUE;
}


//*******************************************************************
//
//                      WALK Stuff
//
//******************************************************************


PWALK
AllocateWalk (
    HTHDX       hthd,
    DWORD       Addr,
    DWORD       Size,
    BPTP        BpType,
    BOOL        fEmulate
    )
/*++

Routine Description:

    Allocates new Walk structure and adds it to the list

Arguments:

    hthd    -   Supplies thread

    Addr    -   Supplies address

    Size    -   Supplies Size

    BpType  -   Read, write, change, exec

    fEmulate-   Suggestion to the DM to eumlate this walk.

Return Value:

    PWALK   -   Walk created

--*/
{
    PWALK   Walk;
    DWORD   i;

    EnterCriticalSection(&csWalk);

    if ( Walk = (PWALK)MHAlloc( sizeof( WALK ) ) ) {

        Walk->hthd          = hthd;
        Walk->GlobalCount   = 0;
        Walk->LocalCount    = 0;
        Walk->Active        = FALSE;
        Walk->BpType        = BpType;
        Walk->Register      = -1;

        Walk->u.Data.DataContents  = NULL;
        Walk->u.Range.StartBP       = NULL;

        if (hthd->WalkData == NULL) {
            hthd->WalkData = MHAlloc (sizeof (WALK_INFO));
            memset (hthd->WalkData, 0, sizeof (WALK_INFO));
        }

        Walk->Method.notifyFunction     = (ACVECTOR)MethodWalk;
        Walk->Method.lparam             = hthd->WalkData;

        InitializeListHead (&Walk->GroupWalkList);
        InsertTailList (&AllWalkList, &Walk->AllWalkEntry);
        InsertTailList (&hthd->WalkList, &Walk->ThreadWalkEntry);

        if (BpType == bptpRange) {

            Walk->Register          = -1;
            Walk->u.Range.AddrStart = Addr;
            Walk->u.Range.AddrEnd   = Addr + Size;
            Walk->u.Range.SavedSP   = 0;
            Walk->u.Range.fInRange  = FALSE;

        } else if (Addr == 0) {

            Walk->u.Data.DataAddr   = 0;
            Walk->u.Data.DataSize   = 0;
            Walk->Register          = -1;
            Walk->u.Range.AddrStart = 0;
            Walk->u.Range.AddrEnd   = 0;

        } else if (IsDataBp (BpType)) {

            Walk->u.Data.DataAddr      = Addr;
            Walk->u.Data.DataSize      = Size;

            if (!fEmulate) {
                Walk->Register = DrAllocateDebugRegister (hthd,
                                                          Addr,
                                                          Size,
                                                          BpType);
            } else {
                Walk->Register = -1;
            }

         } else {

            assert (FALSE); // the BpType was invalid
         }
    }

    LeaveCriticalSection (&csWalk);

    return Walk;
}




BOOL
DeallocateWalk (
    PWALK   Walk
    )
/*++

Routine Description:

    Takes a walk out of the list and frees its memory.

Arguments:

    Walk    -   Supplies Walk to deallocate

Return Value:


    BOOLEAN -   TRUE if deallocated

--*/
{
    EnterCriticalSection(&csWalk);

    RemoveEntryList (&Walk->AllWalkEntry);
    RemoveEntryList (&Walk->ThreadWalkEntry);

    RemoveWalkBindings (Walk);

    if (Walk->Register >= 0) {
        DrDeallocateDebugRegister (Walk->hthd, Walk->Register);
    }
    
    if (IsDataBp (Walk->BpType) && Walk->u.Data.DataContents) {
        MHFree (Walk->u.Data.DataContents);
    }

    MHFree (Walk);

    LeaveCriticalSection (&csWalk);

    return TRUE;
}




PWALK
FindWalk(
    HTHDX       hthd,
    DWORD       Addr,
    DWORD       Size,
    BPTP        BpType
    )
/*++

Routine Description:

    Finds a walk

Arguments:

    hthd        -   Supplies thread

    Addr        -   Supplies Address

    Size        -   Supplies Size

    BpType      -   Supplies type of BP

Return Value:

    PWALK       -   Found Walk

--*/

{
    PWALK   Walk;
    PWALK   FoundWalk = NULL;
    PLIST_ENTRY List;

    EnterCriticalSection (&csWalk);

    List = hthd->WalkList.Flink;

    while ( List != &hthd->WalkList ) {

        Walk = CONTAINING_RECORD (List, WALK, ThreadWalkEntry);
        List = List->Flink;

        if ( Walk->BpType == BpType ) {
        
            if (BpType == bptpRange) {

                if (Walk->u.Range.AddrStart == Addr &&
                    Walk->u.Range.AddrEnd == Addr + Size) {
                    FoundWalk = Walk;
                    break;
                }

            } else {
            
                if ((Walk->u.Data.DataAddr == 0) ||
                    (Walk->u.Data.DataAddr == Addr) ) {

                    if ( Walk->Register == -1 ) {

                        FoundWalk = Walk;
                        break;

                    } else if (Size <= DrGetDebugRegisterDataSize (hthd,
                                          Walk->Register)) {
                                          
                        FoundWalk = Walk;
                        break;
                    }
                }
            }
        }
    }

    LeaveCriticalSection (&csWalk);

    return FoundWalk;
}


PWALK
FindWalkForHthd(
    HANDLE  hWalk,
    HTHDX   hthd
    )
/*++

Routine Description:


Arguments:


Return Value:


--*/

{
    PLIST_ENTRY         GroupList = (PLIST_ENTRY)hWalk;
    PWALK_GROUP_ENTRY   Entry;
    PLIST_ENTRY         List;
    PWALK               Walk;

    List = GroupList->Flink;

    while (List != GroupList) {
        Entry = CONTAINING_RECORD (List, WALK_GROUP_ENTRY, WalksInGroup);
        List = List->Flink;
        Walk = Entry->Walk;

        if (Walk->hthd == hthd) {
            return Walk;
        }
    }
    return NULL;
}



BOOL
IsWalkInGroup(
    HANDLE hWalk,
    PVOID  pWalk
    )
/*++

Routine Description:

    Tells if a specific WALK* is in a group of walks.

--*/
{
    PLIST_ENTRY         GroupList = (PLIST_ENTRY)hWalk;
    PWALK_GROUP_ENTRY   Entry;
    PLIST_ENTRY         List;
    PWALK               Walk;

    List = GroupList->Flink;
    
    while (List != GroupList) {

        Entry = CONTAINING_RECORD (List, WALK_GROUP_ENTRY, WalksInGroup);
        
        if ((PWALK)pWalk == Entry->Walk) {
            return TRUE;
        }
        
        List = List->Flink;
    }
    return FALSE;
}



PBREAKPOINT
FindBpForWalk(
    PVOID pWalk
    )
/*++

Routine Description:

    Walk through all the BPs in the system and find one with pWalk in it's
    group list (hWalk).  Note: if two BPs both had the same walk in their
    group list, this function would fail to work properly.

--*/
{

    PBREAKPOINT pbp;

    EnterCriticalSection (&csThreadProcList);

    pbp = bpList;
    while (pbp) {
        if (pbp->hWalk && IsWalkInGroup (pbp->hWalk, pWalk)) {
            break;
        }
        pbp = pbp->next;
    }

    LeaveCriticalSection (&csThreadProcList);

    return pbp;
}


int
MethodWalk(
    DEBUG_EVENT*    de,
    HTHDX           hthd,
    DWORD           unused,
    PWALK_INFO      WalkInfo
    )
/*++

Routine Description:

    When we enter the walk method, we must check whether any of the Walks
    on this thread have fired.  The first one that we find that fired, we
    will send to the EM and any other's that would've fired are lost.

Arguments:

    de      -   Supplies debug event

    hthd    -   Supplies thread

    unused  -



Return Value:

    Nothing meaningful.


Comments:

    NOTE: the CheckBpt () function is not a very nice function.
    It can Set and Remove BPs EVEN WHEN IT RETURNS FALSE, which
    means it can wreak havoc with our Walk list.  Be extra careful
    if we call CheckBpt ().
    
--*/
{
    HPRCX       hprc = hthd->hprc;
    WALK*       Walk;
    BOOL        fDoContinue = FALSE;
    BOOL        fDeactivateWalks = FALSE;
    LIST_ENTRY* List;

    Unreferenced (unused);
    Unreferenced (WalkInfo);

    assert (WalkInfo);

    //
    // However we got here, we do not need this BP anymore.
    //

    if (WalkInfo->SafetyBP) {

        //
        // If the expected event for the safety bp is not on the expected
        // events list then the safety bp was removed as part of that event.
        //

        EXPECTED_EVENT* ee;

        ee = PeeIsEventExpected (hthd,
                                 BREAKPOINT_DEBUG_EVENT,
                                 (DWORD) WalkInfo->SafetyBP,
                                 TRUE);
        
        if (ee) {
            MHFree (ee);
            RemoveBP (WalkInfo->SafetyBP);
        }
        
        WalkInfo->SafetyBP = NULL;
    }

    //
    // Iterate through all walks on this thread.  In each iteration we must
    // setup for the next walk and check if we have an event to fire.
    //

    for (List = hthd->WalkList.Flink; List != &hthd->WalkList;
         List = List->Flink) {
        
        Walk = CONTAINING_RECORD (List, WALK, ThreadWalkEntry);

            
        if (Walk->Active && (Walk->GlobalCount || Walk->LocalCount)) {

            BOOL    fPotentialHit = FALSE;
            
            if (Walk->BpType == bptpRange) {

                if (InRange (Walk->u.Range.AddrStart,
                             Walk->u.Range.AddrEnd,
                             PC (hthd))) {

                    //
                    // We are currently in Range
                    //
                                        
                    Walk->u.Range.fInRange = TRUE;
                    Walk->u.Range.SavedSP = STACK_POINTER (hthd);

                    fPotentialHit = FALSE;

                } else {

                    //
                    // We are not currently in Range
                    //

                    if (Walk->u.Range.fInRange &&
                        STACK_POINTER (hthd) >= Walk->u.Range.SavedSP) {

                        //
                        // If we were in Range on the last step, we
                        // have a potential hit.
                        //

                        fPotentialHit = TRUE;
                    }

                    Walk->u.Range.fInRange = FALSE;

                    //
                    // NB: should we have an "emergency" check; i.e.:
                    // if (STACK_POINTER (hthd) > Walk->u.Range.SavedSP) {
                    //    fPotentialHit = TRUE;
                    //
                    //  for the case that we somehow miss the transition and
                    //  still have Walk->fInRange == FALSE ? ? ?
                    //
                    //  Are there any potential problems with this?
                    //
                }

            } else {
                fPotentialHit = TRUE;
            }
            

            if (fPotentialHit && CheckWalk (Walk) &&
                CheckBpt (hthd, FindBpForWalk (Walk))) {

                //
                // Tell the EM it stopped.
                //

                fDeactivateWalks = TRUE;
                Walk->Active = FALSE;
                ConsumeAllThreadEvents (hthd, FALSE);

                SetExceptionAddress (&falseBPEvent, PC (hthd));

                NotifyEM (&falseBPEvent,
                          hthd,
                          0,
                          (LPVOID)FindBpForWalk (Walk));
                break;
            }

        }
    }

    //
    // Some of the walks could have been marked for deletion while looking
    // at them.  Delete them now.
    //

    List = hthd->WalkList.Flink;

    while (List != &hthd->WalkList) {
    
        Walk = CONTAINING_RECORD (List, WALK, ThreadWalkEntry);
        List = List->Flink;

        if (fDeactivateWalks) {
            Walk->Active = FALSE;
        }

        if (Walk->Active) {
            fDoContinue = TRUE;
        }
        
        if (Walk->GlobalCount == 0 && Walk->LocalCount == 0) {

            //
            // If this walk was marked for deletion, delete it now.
            //
        
            DeallocateWalk (Walk);

        } else if (Walk->Active) {

            //
            // If this walk is still active, setup for continuing the walk.
            //
            
            SetupWalk (Walk, TRUE);

        }

    }
    
    if (fDoContinue) {
        SetDebugEventThreadState (hthd->hprc, ts_running);
        AddQueue (QT_CONTINUE_DEBUG_EVENT,
                  hprc->pid,
                  hthd->tid,
                  DBG_CONTINUE,
                  0);
    }

    return TRUE;
}



BREAKPOINT*
SetupSafetyBp(
    HTHDX       hthd,
    METHOD*     Method
    )
{
    ADDR        currAddr;
    int         lpf;
    BREAKPOINT* SafetyBp = NULL;


    AddrFromHthdx (&currAddr, hthd);
    IsCall (hthd, &currAddr, &lpf, FALSE);

    if (lpf == INSTR_IS_CALL) {
    
        //
        // Set a safety breakpoint on the return site to prevent
        // running free over system calls and other mysterious
        // cases.
        //

        //NB: this code needs to be moved out of the loop.
        //It deals with all walks on a thread, not just one.
        
        SafetyBp = SetBP (hthd->hprc,
                          hthd,
                          bptpExec,
                          bpnsStop,
                          &currAddr,
                          (HPID)INVALID);

        //
        // Register an expected event in case this mystery BP is
        // hit.
        //

        RegisterExpectedEvent (hthd->hprc,
                               hthd,
                               BREAKPOINT_DEBUG_EVENT,
                               (DWORD) SafetyBp,
                               Method,
                               NO_ACTION,
                               FALSE,
                               NULL);
    }

    return SafetyBp;
}



VOID
DuplicateWalkBindings(
    PWALK OldWalk,
    PWALK NewWalk
    )
/*++

Routine Description:

    Copy the breakpoint bindings for a walk into another walk.
    This is used for duplicationg global watchpoints.

Arguments:

    OldWalk - Supplies a WALK which is bound to one or more breakpoints.

    NewWalk - Supplies a WALK which is to be bound to the same breakpoints
            as OldWalk.

Return Value:

    None

--*/
{
    PLIST_ENTRY OldList;
    PWALK_GROUP_ENTRY OldEntry;
    PWALK_GROUP_ENTRY NewEntry;

    //
    // Run down the list of breakpoints that OldWalk is bound to,
    // and create equivalent bindings for NewWalk.
    //

    OldList = OldWalk->GroupWalkList.Flink;

    while (OldList != &OldWalk->GroupWalkList) {

        //
        // Find each breakpoint
        //

        OldEntry = CONTAINING_RECORD(OldList, WALK_GROUP_ENTRY, GroupsUsingWalk);

        NewEntry = MHAlloc(sizeof(WALK_GROUP_ENTRY));

        NewEntry->Walk = NewWalk;

        //
        // add this to the Walk's list of BP bindings:
        //

        InsertTailList (&NewWalk->GroupWalkList, &NewEntry->GroupsUsingWalk);

        //
        // And add it to the BP's list of walks:
        //

        InsertTailList(&OldEntry->WalksInGroup, &NewEntry->WalksInGroup);
        OldList = OldList->Flink;

    }
}


VOID
RemoveWalkBindings(
    PWALK Walk
    )
/*++

Routine Description:

    Delete all of the bindings from a WALK to breakpoints (Group lists).
    The GROUP_LIST_ENTRY binding nodes will be freed.

Arguments:

    Walk - Supplies the WALK which is to be unbound.

Return Value:

    None

--*/
{
    //
    // remove all GROUP_LIST_ENTRY bindings for a walk
    //

    PLIST_ENTRY List;
    PLIST_ENTRY PossibleGroupListHead;
    PWALK_GROUP_ENTRY Entry;

    List = Walk->GroupWalkList.Flink;

    while (List != &Walk->GroupWalkList) {

        Entry = CONTAINING_RECORD(List, WALK_GROUP_ENTRY, GroupsUsingWalk);
        List = List->Flink;

        // clean this entry out of the lists...
        PossibleGroupListHead = Entry->WalksInGroup.Flink;
        RemoveEntryList(&Entry->WalksInGroup);
        if (IsListEmpty(PossibleGroupListHead)) {
            MHFree(PossibleGroupListHead);
        }

        RemoveEntryList(&Entry->GroupsUsingWalk);

        MHFree(Entry);
    }
}


VOID
AddWalkToGroupList(
    PLIST_ENTRY GroupList,
    PWALK Walk
    )
/*++

Routine Description:

    Bind a WALK to a breakpoint's walk group list.

Arguments:

    GroupList - Supplies the list header for a breakpoint's group list

    Walk - Supplies a WALK structure which is to be added to the group list.

Return Value:

    None

--*/
{
    PWALK_GROUP_ENTRY Entry;

    Entry = MHAlloc(sizeof(WALK_GROUP_ENTRY));

    Entry->Walk = Walk;
    InsertTailList (GroupList, &Entry->WalksInGroup);
    InsertTailList (&Walk->GroupWalkList, &Entry->GroupsUsingWalk);
}



VOID
ClearProcessWalkFlags(
    HPRCX   hprc
    )
{
    HTHDX   hthd;

    for (hthd = hprc->hthdChild; hthd; hthd = hthd->nextSibling) {
        ClearThreadWalkFlags (hthd);
    }
}

    
VOID
ClearThreadWalkFlags(
    HTHDX   hthd
    )
{
    PWALK       walk;
    PLIST_ENTRY list;
    

    EnterCriticalSection (&csWalk);
    
    for (list = hthd->WalkList.Flink;
         list != &hthd->WalkList;
         list = list->Flink) {

        walk = CONTAINING_RECORD (list, WALK, ThreadWalkEntry);

        walk->Active = FALSE;
    }

    LeaveCriticalSection (&csWalk);
}

//
//  Virtualized debug register Functions.
//


BOOL
DrDeallocateDebugRegister(
    HTHDX   hthd,
    int     Register
    )
{
#ifndef HAS_DEBUG_REGS
    return TRUE;
#else

    PDEBUGREG Dr = &hthd->DebugRegs [Register];

    if (--Dr->ReferenceCount <= 0) {
        Dr->InUse = FALSE;
        ClearDebugRegister (hthd, Register);
    }

    return TRUE;
#endif
}



BOOL
DrIsDebugRegisterInUse(
    HTHDX   hthd,
    int     Register
    )
{
#ifndef HAS_DEBUG_REGS
    assert (FALSE);
    return FALSE;
#else
    return hthd->DebugRegs [Register].InUse;
#endif
}

BOOL
DrSetupDebugRegister(
    HTHDX   hthd,
    int     Register
    )
{
#ifndef HAS_DEBUG_REGS
    return TRUE;
#else

    DEBUGREG*   Dr;

    Dr = &hthd->DebugRegs [Register];

    return SetupDebugRegister (hthd,
                               Register,
                               Dr->DataSize,
                               Dr->DataAddr,
                               Dr->BpType
                               );
#endif
}

VOID
DrClearDebugRegister(
    HTHDX   hthd,
    int     Register
    )
{
#ifdef HAS_DEBUG_REGS
    ClearDebugRegister (hthd, Register);
#endif
}
    
                        
    
    
int
DrAllocateDebugRegister(
    HTHDX   hthd,
    DWORD   Addr,
    DWORD   Size,
    DWORD   BpType
    )
{
#ifndef HAS_DEBUG_REGS
    return -1;
#else

    int     Register = -1;
    int     i;

    for (i = 0; i < NDEBUG_REG_DATA_SIZES; i++) {
        if (Size == DebugRegDataSizes [i]) {
            break;
        }
    }

    if (i == NDEBUG_REG_DATA_SIZES) {

        // Not a supported size.
        
        return -1;
    }

    //
    // Look for an exact match preferablly, or an open register.
    
    
    for (i = 0; i < NUMBER_OF_DEBUG_REGISTERS; i++) {

        if (hthd->DebugRegs [i].ReferenceCount == 0) {
                Register = i;
            } else if ( (hthd->DebugRegs [i].DataAddr == Addr) &&
                        (hthd->DebugRegs [i].DataSize >= Size) &&
                        (hthd->DebugRegs [i].BpType == BpType) ) {
                Register = i;
                break;
            }
    }

    if (Register >= 0) {

        if (hthd->DebugRegs [Register].ReferenceCount == 0) {

            hthd->DebugRegs [Register].DataSize = Size;
            hthd->DebugRegs [Register].DataAddr = Addr;
            hthd->DebugRegs [Register].BpType = BpType;
            hthd->DebugRegs [Register].InUse = FALSE;
        }
        hthd->DebugRegs [Register].ReferenceCount++;
    }

    return Register;

#endif
}


ULONG
DrGetDebugRegisterDataSize(
    HTHDX   hthd,
    int     Register
    )
{
#ifdef HAS_DEBUG_REGS
    return hthd->DebugRegs [Register].DataSize;
#else
    return 0;
#endif
}

VOID
DrSetDebugRegisterInUse(
    HTHDX   hthd,
    int     Register,
    BOOL    InUse
    )
{
#ifdef HAS_DEBUG_REGS

    hthd->DebugRegs [Register].InUse = InUse;

#endif
}




//
// Debug Dumping functions
//

// *12345678  0f55abff  DataC  11   0   54564FF          1   -1       100 
//  00102020  0bff8888  Range   0   1   5555555      55555    0         0


#if DBG

#define XX(_x) case bptp##_x: return #_x


char*
BpTypeToStr(
    BPTP    BpType
    )
{
    switch (BpType)
    {
        XX (Exec);
        XX (DataC);
        XX (DataW);
        XX (DataR);
        XX (DataExec);
        XX (RegC);
        XX (RegW);
        XX (RegR);
        XX (Message);
        XX (MClass);
        XX (Int);
        XX (Range);
    }

    return "ERR";
}

#undef XX

VOID
DumpWalkHeader(
    )
{
    DebugPrint ("\n  Addr      hthd      Type   GC  LC  Strt/Adr"
                "  Stop/Cnt  Reg  Contents\n");
}



VOID
DumpWalk(
    WALK*   Walk
    )
{
    DWORD   Contents = 0;
    BPTP    BpType = Walk->BpType;
    int     Prec = 0;
    
    
    if (BpType == bptpDataC && Walk->u.Data.DataContents) {
        Prec = 1;
        memcpy (&Contents,
                Walk->u.Data.DataContents,
                min (4, Walk->u.Data.DataSize)
                );
    }

    DebugPrint (" %c%08x  %08x  %5.5s  %2d  %2d  %8x  %8x  %3d %8.*d\n",
                Walk->Active ? '*' : ' ',
                Walk,
                Walk->hthd,
                BpTypeToStr (BpType),
                Walk->GlobalCount,
                Walk->LocalCount,
                IsDataBp (BpType) ? Walk->u.Data.DataAddr : Walk->u.Range.AddrStart,
                IsDataBp (BpType) ? Walk->u.Data.DataSize : Walk->u.Range.AddrEnd,
                IsDataBp (BpType) ? Walk->Register : 0,
                Prec,
                IsDataBp (BpType) ? Contents : 0
                );

}
        

VOID
DumpWalks(
    )
{
    PLIST_ENTRY List;
    PWALK       Walk;

    DumpWalkHeader ();

    List = AllWalkList.Flink;
    
    while (List != &AllWalkList) {
        Walk = CONTAINING_RECORD (List, WALK, AllWalkEntry);
        DumpWalk (Walk);
        List = List->Flink;
    }

}

VOID
DumpThreadWalks(
    HTHDX   hthd
    )
{
    PLIST_ENTRY List;
    PWALK       Walk;

    if (hthd == NULL) {
        DebugPrint ("<Invalid hthd == NULL>\n");
        return;
    }
    
    DumpWalkHeader ();
    
    List = hthd->WalkList.Flink;

    while (List != &hthd->WalkList) {
        Walk = CONTAINING_RECORD (List, WALK, ThreadWalkEntry);
        DumpWalk (Walk);
        List = List->Flink;
    }
}

VOID
DumpGroupWalks(
    HANDLE  hWalk
    )
{
    PLIST_ENTRY         GroupListHead = (PLIST_ENTRY) hWalk;
    PLIST_ENTRY         List;
    PWALK_GROUP_ENTRY   Entry;

    if (hWalk == NULL) {
        DebugPrint ("<Invalid hWalk == NULL>\n");
        return;
    }
    
    DumpWalkHeader ();
    
    List = GroupListHead->Flink;

    while (List != GroupListHead) {
        Entry = CONTAINING_RECORD (List, WALK_GROUP_ENTRY, WalksInGroup);
        DumpWalk (Entry->Walk);
        List = List->Flink;
    }
}

#endif // DBG
