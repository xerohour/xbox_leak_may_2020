/*++

Copyright (c) 1998  Microsoft Corporation

Module Name:

    stacks.c

Abstract:

    WinDbg Extension Api

Author:

    Adrian J. Oney (adriao) 07-28-1998

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

#define ETHREAD_NOT_READABLE    1
#define THREAD_VALID            2
#define FIRST_THREAD_VALID      3
#define NO_THREADS              4

struct _BLOCKER_TREE ;
typedef struct _BLOCKER_TREE BLOCKER_TREE, *PBLOCKER_TREE ;

BOOL
StacksValidateProcess(
    IN PKPROCESS RealProcessBase,
    IN PKPROCESS ProcessContents
    );

BOOL
StacksValidateThread(
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase
    );

VOID StacksDumpProcessAndThread(
    IN PKPROCESS ProcessContents,
    IN PKPROCESS RealProcessBase,
    IN ULONG     ThreadDesc,
    IN PETHREAD  Thread,
    IN PETHREAD  RealThreadBase,
    IN PBLOCKER_TREE BlockerTree,
    IN ULONG     Verbosity
    );

VOID StacksGetThreadStateName(
    IN PETHREAD Thread,
    OUT PCHAR Dest
    );

VOID
DumpThreadBlockageInfo (
    IN char *pad,
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase,
    IN ULONG Verbosity
    );

VOID
DumpThreadStackInfo (
    IN char *pad,
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase,
    IN ULONG Verbosity
    );

extern ULONG STeip, STebp, STesp;

PKPROCESS ProcessLastDump;
ULONG ThreadLastDump;

ULONG TotalProcessCommit;

#ifdef TARGET_i386
VOID GetStackTraceRegs(ULONG,PULONG,PULONG,PULONG);
#endif

struct _BLOCKER_TREE {
   char const *Symbolic ;
   PBLOCKER_TREE Child ;
   PBLOCKER_TREE Sibling ;
   PBLOCKER_TREE Parent ;
   BOOL Nested ;
} ;

VOID
GetThreadBlocker (
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase,
    IN ULONG Verbosity,
    IN PBLOCKER_TREE BlockerTree,
    OUT PCHAR Buffer
    );

BOOL
BlockerTreeWalk(
   IN OUT PBLOCKER_TREE *blockerHead,
   IN char *szSymbolic
   ) ;

PBLOCKER_TREE
BlockerTreeBuild(
   VOID
   ) ;

VOID
BlockerTreeFree(
   IN PBLOCKER_TREE BlockerTree
   ) ;

DECLARE_API( stacks )

/*++

Routine Description:

    Dumps the active process list.

Arguments:

    None.

Return Value:

    None.

--*/

{
    ULONG Result;
    LIST_ENTRY List;
    PLIST_ENTRY Next;
    PLIST_ENTRY NextThread;
    ULONG ProcessHead;
    PKPROCESS Process;
    KPROCESS ProcessContents;
    PETHREAD Thread;
    ETHREAD ThreadContents;
    ULONG UserProbeAddress;
    ULONG Verbosity = 0 ;
    PBLOCKER_TREE blockerTree ;

    blockerTree = BlockerTreeBuild() ;

    sscanf(args, "%lx", &Verbosity);

    dprintf("Thread  Thread   ThreadState  Blocker\n") ;

    Process = (PKPROCESS)GetNtDebuggerDataValue( PsActiveProcessHead );
    if (!Process) {
        dprintf("Unable to get value of PsActiveProcessHead\n");
        goto Exit;
    }

    if (!ReadMemory( (DWORD)Process, &ProcessContents, sizeof(KPROCESS), &Result )) {
        dprintf("Unable to read _KPROCESS at %lx\n",Process);
        goto Exit;
    }

    if (!StacksValidateProcess(Process, &ProcessContents)) {

        dprintf("Process list damaged, or maybe symbols are incorrect?\n%lx\n",Process);
        goto Exit;
    }

    NextThread = ProcessContents.ThreadListHead.Flink;

    if (NextThread == &Process->ThreadListHead) {

        StacksDumpProcessAndThread(&ProcessContents, Process, NO_THREADS, NULL, NULL, blockerTree, Verbosity) ;

    } else {

        while ( NextThread != &Process->ThreadListHead) {

            Thread = (PETHREAD)(CONTAINING_RECORD(NextThread,KTHREAD,ThreadListEntry));
            if (!ReadMemory((DWORD)Thread,
                        &ThreadContents,
                        sizeof(ETHREAD),
                        &Result)) {

                StacksDumpProcessAndThread(&ProcessContents, Process, ETHREAD_NOT_READABLE, NULL, NULL, blockerTree, Verbosity) ;

                dprintf("Unable to read _ETHREAD at %lx\n",Thread);
                break;
            }

            if (!StacksValidateThread(&ThreadContents, Thread)) {

                StacksDumpProcessAndThread(&ProcessContents, Process, ETHREAD_NOT_READABLE, NULL, NULL, blockerTree, Verbosity) ;
            } else if (NextThread == ProcessContents.ThreadListHead.Flink) {

                StacksDumpProcessAndThread(&ProcessContents, Process, FIRST_THREAD_VALID, &ThreadContents, Thread, blockerTree, Verbosity) ;
            } else {

                StacksDumpProcessAndThread(&ProcessContents, Process, THREAD_VALID, &ThreadContents, Thread, blockerTree, Verbosity) ;
            }

            NextThread = ((PKTHREAD)&ThreadContents)->ThreadListEntry.Flink;

            if (CheckControlC()) {
                goto Exit;
            }
        }
    }
Exit:
   BlockerTreeFree(blockerTree) ;
   return;
}

BOOL
StacksValidateProcess(
    IN PKPROCESS RealProcessBase,
    IN PKPROCESS ProcessContents
    )
{
    return TRUE ;
}

VOID StacksDumpProcessAndThread(
    IN PKPROCESS     ProcessContents,
    IN PKPROCESS     RealProcessBase,
    IN ULONG         ThreadDesc,
    IN PETHREAD      Thread,
    IN PETHREAD      RealThreadBase,
    IN PBLOCKER_TREE BlockerTree,
    IN ULONG         Verbosity
    )
{
    OBJECT_HANDLE_TABLE HandleTable;
    ULONG NumberOfHandles;
    ULONG Result;
    CHAR  ThreadState[13] ;
    CHAR  ThreadBlocker[256] ;
    UINT  i ;
    struct THREAD_STATE_NAMES *ThreadStateName ;

    NumberOfHandles = 0;
    if (ReadMemory((DWORD)GetNtDebuggerData(ObpObjectHandleTable),
                     &HandleTable,
                     sizeof(HandleTable),
                     &Result)) {

        NumberOfHandles = HandleTable.HandleCount;
    }

    switch(ThreadDesc) {

        case FIRST_THREAD_VALID:
        case THREAD_VALID:
            StacksGetThreadStateName(Thread, ThreadState) ;
            i=strlen(ThreadState) ;
            while(i<11) ThreadState[i++]=' ' ;
            ThreadState[i]='\0' ;
            GetThreadBlocker (Thread, RealThreadBase, Verbosity, BlockerTree, ThreadBlocker) ;
            dprintf("%06lx  %08lx  %s  %s\n",
                Thread->UniqueThread,
                RealThreadBase,
                ThreadState,
                ThreadBlocker
                );

            break ;

        case NO_THREADS:

            dprintf("------  NOTHREADS\n");

            break ;

        case ETHREAD_NOT_READABLE:

            dprintf("------  NO ETHREAD DATA\n");

            break ;
    }

    if ((ThreadDesc != FIRST_THREAD_VALID) && (ThreadDesc != THREAD_VALID)) {

        return ;
    }
    //DumpThreadBlockageInfo("    ", Thread, RealThreadBase, Verbosity) ;
    DumpThreadStackInfo("    ", Thread, RealThreadBase, Verbosity) ;
}

UCHAR *StacksWaitReasonList[] = {
    "Executive",
    "FreePage",
    "PageIn",
    "PoolAllocation",
    "DelayExecution",
    "Suspended",
    "UserRequest",
    "WrExecutive",
    "WrFreePage",
    "WrPageIn",
    "WrPoolAllocation",
    "WrDelayExecution",
    "WrSuspended",
    "WrUserRequest",
    "WrEventPairHigh",
    "WrEventPairLow",
    "WrLpcReceive",
    "WrLpcReply",
    "WrVirtualMemory",
    "WrPageOut",
    "WrRendezvous",
    "WrFsCacheIn",
    "WrFsCacheOut",
    "Spare4",
    "Spare5",
    "Spare6",
    "Spare7"};


VOID StacksGetThreadStateName(
    IN PETHREAD Thread,
    OUT PCHAR Dest
    )
{
    switch (Thread->Tcb.State) {
        case Initialized: strcpy(Dest, "INITIALIZED"); break;
        case Ready:       strcpy(Dest, "READY"); break;
        case Running:     strcpy(Dest, "RUNNING"); break;
        case Standby:     strcpy(Dest, "STANDBY"); break;
        case Terminated:  strcpy(Dest, "TERMINATED"); break;
        case Waiting:     strcpy(Dest, "BLOCKED"); break;
        case Transition:  strcpy(Dest, "TRANSITION"); break;
        default:          strcpy(Dest, "????") ; break ;
    }
}


BOOL
StacksValidateThread (
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase
    )
{
    if (Thread->Tcb.Header.Type != ThreadObject) {
        dprintf("TYPE mismatch for thread object at %lx\n",RealThreadBase);
        return FALSE;
    }
    return TRUE ;
}


VOID
DumpThreadBlockageInfo (
    IN char *Pad,
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase,
    IN ULONG Verbosity
    )
{
    #define MAX_STACK_FRAMES  40
    TIME_FIELDS Times;
    LARGE_INTEGER RunTime;
    ULONG Address;
    ULONG Result;
    KMUTANT WaitObject;
    PVOID PointerWaitObject = &WaitObject;
    PKWAIT_BLOCK WaitBlock;
    KWAIT_BLOCK OutsideBlock;
    ULONG WaitOffset;
    PKPROCESS Process;
    CHAR Buffer[80];
    ULONG KeTimeIncrement;
    ULONG TimeIncrement;
    ULONG frames = 0;
    ULONG i;
    ULONG displacement;
    EXTSTACKTRACE stk[MAX_STACK_FRAMES];
#ifdef TARGET_i386
    struct {
        KSWITCHFRAME Frame;
        DWORD SavedEbp;
    } SwitchFrame;
#endif


    if (Thread->Tcb.State == Waiting) {
        dprintf("%s (%s) %s %s\n",
            Pad,
            StacksWaitReasonList[Thread->Tcb.WaitReason],
            (Thread->Tcb.WaitMode==KernelMode) ? "KernelMode" : "UserMode",Thread->Tcb.Alertable ? "Alertable" : "Non-Alertable");
        if ( Thread->Tcb.SuspendCount ) {
            dprintf("SuspendCount %lx\n",Thread->Tcb.SuspendCount);
        }

        WaitOffset =
               (ULONG)Thread->Tcb.WaitBlockList - (ULONG)RealThreadBase;

        if (WaitOffset > (ULONG)sizeof(ETHREAD)) {
            if (!ReadMemory((DWORD)Thread->Tcb.WaitBlockList,
                            &OutsideBlock,
                            sizeof(KWAIT_BLOCK),
                            &Result)) {
                dprintf("%sunable to get Wait object\n",Pad);
                goto BadWaitBlock;
            }
            WaitBlock = &OutsideBlock;
        } else {
            WaitBlock = (PKWAIT_BLOCK)((ULONG)Thread + WaitOffset);
        }

        do {

            dprintf("%s    %lx  ",Pad,WaitBlock->Object);
            if (!ReadMemory((DWORD)WaitBlock->Object,
                            &WaitObject,
                            sizeof(KMUTANT),
                            &Result)) {

                dprintf("%sunable to get Wait object\n",Pad);
                break;
            }

            switch (WaitObject.Header.Type) {
                case EventNotificationObject:
                    dprintf("NotificationEvent\n");
                    break;
                case EventSynchronizationObject:
                    dprintf("SynchronizationEvent\n");
                    break;
                case SemaphoreObject:
                    dprintf("Semaphore Limit 0x%lx\n",
                             ((PKSEMAPHORE)PointerWaitObject)->Limit);
                    break;
                case ThreadObject:
                    dprintf("Thread\n");
                    break;
                case TimerNotificationObject:
                    dprintf("NotificationTimer\n");
                    break;
                case TimerSynchronizationObject:
                    dprintf("SynchronizationTimer\n");
                    break;
                case EventPairObject:
                    dprintf("EventPair\n");
                    break;
                case ProcessObject:
                    dprintf("ProcessObject\n");
                    break;
                case MutantObject:
                    dprintf("Mutant - owning thread %lx\n",
                            ((PKMUTANT)PointerWaitObject)->OwnerThread);
                    break;
                case QueueObject:
                    dprintf("QueueObject\n");
                    break;
                default:
                    dprintf("Unknown\n");
                    break;
            }

            if (WaitBlock->NextWaitBlock == Thread->Tcb.WaitBlockList) {
                break;
            }
            WaitOffset =
                   (ULONG)WaitBlock->NextWaitBlock - (ULONG)RealThreadBase;

            if (WaitOffset > (ULONG)sizeof(ETHREAD)) {

                if (!ReadMemory((DWORD)WaitBlock->NextWaitBlock,
                                &OutsideBlock,
                                sizeof(KWAIT_BLOCK),
                                &Result)) {
                    dprintf("%sunable to get Wait object\n",Pad);
                    break;
                }
                WaitBlock = &OutsideBlock;
            } else {
                WaitBlock = (PKWAIT_BLOCK)((ULONG)Thread + WaitOffset);
            }
        } while ( TRUE );
    }

BadWaitBlock:

    if (Thread->IrpList.Flink != Thread->IrpList.Blink ||
        Thread->IrpList.Flink != &RealThreadBase->IrpList
       ) {

        ULONG IrpListHead = (ULONG)&RealThreadBase->IrpList;
        PLIST_ENTRY Next;
        IRP IrpContents;
        PIRP p;
        ULONG Counter = 0;

        Next = Thread->IrpList.Flink;
        dprintf("%sIRP List:\n",Pad);
        while (((ULONG)Next != IrpListHead) && (Counter < 17)) {
            Counter += 1;
            Address = (ULONG)CONTAINING_RECORD(Next,IRP,ThreadListEntry);
            if (!ReadMemory((DWORD)Address,
                           &IrpContents,
                           sizeof(IRP),
                           &Result)) {
                dprintf( "%sunable to get IRP object\n", Pad );
                break;
            }

            p = &IrpContents;
            dprintf("%s    %08lx: (%04x,%04x) Flags: %08lx  Buffer: %08lx\n",
                    Pad,Address,p->Type,p->Size,p->Flags,p->UserBuffer);

            Next = p->ThreadListEntry.Flink;
        }
    }

}

VOID
DumpThreadStackInfo (
    IN char *Pad,
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase,
    IN ULONG Verbosity
    )
{
    #define MAX_STACK_FRAMES  40
    TIME_FIELDS Times;
    LARGE_INTEGER RunTime;
    ULONG Address;
    ULONG Result;
    KMUTANT WaitObject;
    PVOID PointerWaitObject = &WaitObject;
    PKWAIT_BLOCK WaitBlock;
    KWAIT_BLOCK OutsideBlock;
    ULONG WaitOffset;
    PKPROCESS Process;
    CHAR Buffer[256];
    ULONG KeTimeIncrement;
    ULONG TimeIncrement;
    ULONG frames = 0;
    ULONG i;
    ULONG displacement;
    EXTSTACKTRACE stk[MAX_STACK_FRAMES];
#ifdef TARGET_i386
    struct {
        KSWITCHFRAME Frame;
        DWORD SavedEbp;
    } SwitchFrame;
#endif

    if (Verbosity==0) {
        return ;
    }

#ifdef TARGET_i386

    if (Thread->Tcb.State == Running) {
        GetStackTraceRegs( 0, &STeip, &STebp, &STesp );
        SetThreadForOperation( (PULONG)&RealThreadBase );
        frames = StackTrace( STebp, STesp, STeip, stk, MAX_STACK_FRAMES );
    } else {
        //
        // Get SwitchFrame and perform backtrace providing EBP,ESP,EIP
        // (full FPO backtrace context)
        //
        // N.B. The dword immediately preceding the switch frame contains
        //      the saved EBP.
        //
        ZeroMemory( &SwitchFrame, sizeof(SwitchFrame) );
        ReadMemory(
            (DWORD)Thread->Tcb.KernelStack,
            (PVOID)&SwitchFrame,
            sizeof(SwitchFrame),
            &Result
            );
        if (Result == sizeof(SwitchFrame)) {
            STeip = SwitchFrame.Frame.RetAddr;
            STebp = SwitchFrame.SavedEbp;
            STesp = (ULONG) Thread->Tcb.KernelStack + sizeof(KSWITCHFRAME);
            SetThreadForOperation( (PULONG)&RealThreadBase );
            frames = StackTrace( STebp, STesp, STeip, stk, MAX_STACK_FRAMES );
        }
    }

#elif defined(TARGET_MIPS) || defined(TARGET_ALPHA) || defined(TARGET_PPC)

    if (Thread->Tcb.State != Running) {
        SetThreadForOperation( (PULONG)&RealThreadBase );
        frames = StackTrace( (DWORD)Thread->Tcb.KernelStack,
                             (DWORD)Thread->Tcb.KernelStack,
                             0,
                             stk,
                             MAX_STACK_FRAMES
                             );
    }

#endif

    for (i=1; i<frames; i++) {

        if ((i==0)&&(Verbosity>=2)) {
#ifdef TARGET_PPC
            dprintf( "\n%s      ChildEBP EIP\n", Pad );
#else
            dprintf( "\n%s      ChildEBP EIP\n", Pad );
#endif
        }

        Buffer[0] = '!';
        GetSymbol((LPVOID)stk[i].ProgramCounter, Buffer, &displacement);

        if (Verbosity>=2) {
#ifdef TARGET_PPC
            dprintf( "      %s%08x %08x %s",
                     Pad,
                     stk[i].FramePointer,
                     stk[i].ProgramCounter,
                     Buffer
                   );
#else
            dprintf( "      %s%08x %08x %s(%08x, %08x, %08x)",
                     Pad,
                     stk[i].FramePointer,
                     stk[i].ProgramCounter,
                     Buffer,
                     stk[i].Args[0],
                     stk[i].Args[1],
                     stk[i].Args[2]
                     );
#endif
        } else if (Verbosity==1) {
            dprintf( "                                %s%s",
                     Pad,
                     Buffer
                   ) ;
        }

        if (displacement) {
            dprintf( "+0x%x", displacement );
        }
        dprintf("\n") ;
    }
}

VOID
GetThreadBlocker (
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase,
    IN ULONG Verbosity,
    IN PBLOCKER_TREE BlockerTree,
    OUT PCHAR Buffer
    )
{
    #define MAX_STACK_FRAMES  40
    TIME_FIELDS Times;
    LARGE_INTEGER RunTime;
    ULONG Address;
    ULONG Result;
    KMUTANT WaitObject;
    PVOID PointerWaitObject = &WaitObject;
    PKWAIT_BLOCK WaitBlock;
    KWAIT_BLOCK OutsideBlock;
    ULONG WaitOffset;
    PKPROCESS Process;
    ULONG KeTimeIncrement;
    ULONG TimeIncrement;
    ULONG frames = 0;
    ULONG i;
    ULONG displacement;
    PBLOCKER_TREE blockerCur ;
    EXTSTACKTRACE stk[MAX_STACK_FRAMES];
#ifdef TARGET_i386
    struct {
        KSWITCHFRAME Frame;
        DWORD SavedEbp;
    } SwitchFrame;
#endif


#ifdef TARGET_i386

    if (Thread->Tcb.State == Running) {
        GetStackTraceRegs( 0, &STeip, &STebp, &STesp );
        SetThreadForOperation( (PULONG)&RealThreadBase );
        frames = StackTrace( STebp, STesp, STeip, stk, MAX_STACK_FRAMES );
    } else {
        //
        // Get SwitchFrame and perform backtrace providing EBP,ESP,EIP
        // (full FPO backtrace context)
        //
        // N.B. The dword immediately preceding the switch frame contains
        //      the saved EBP.
        //
        ZeroMemory( &SwitchFrame, sizeof(SwitchFrame) );
        ReadMemory(
            (DWORD)Thread->Tcb.KernelStack,
            (PVOID)&SwitchFrame,
            sizeof(SwitchFrame),
            &Result
            );
        if (Result == sizeof(SwitchFrame)) {
            STeip = SwitchFrame.Frame.RetAddr;
            STebp = SwitchFrame.SavedEbp;
            STesp = (ULONG) Thread->Tcb.KernelStack + sizeof(KSWITCHFRAME);
            SetThreadForOperation( (PULONG)&RealThreadBase );
            frames = StackTrace( STebp, STesp, STeip, stk, MAX_STACK_FRAMES );
        }
    }

#elif defined(TARGET_MIPS) || defined(TARGET_ALPHA) || defined(TARGET_PPC)

    if (Thread->Tcb.State != Running) {
        SetThreadForOperation( (PULONG)&RealThreadBase );
        frames = StackTrace( (DWORD)Thread->Tcb.KernelStack,
                             (DWORD)Thread->Tcb.KernelStack,
                             0,
                             stk,
                             MAX_STACK_FRAMES
                             );
    }

#endif

    Buffer[0] = '!';
    if (frames == 0) {

         strcpy(Buffer, "?? Kernel stack not resident ??") ;
    } else {

        if (Thread->Tcb.State == Running) {

            GetSymbol((LPVOID)stk[0].ProgramCounter, Buffer, &displacement);
            sprintf(Buffer+strlen(Buffer), "+0x%x", displacement );

        } else if (Verbosity>=1) {

            GetSymbol((LPVOID)stk[0].ProgramCounter, Buffer, &displacement);
        } else {

            blockerCur = BlockerTree ;
            for(i=0; i<frames; i++) {

                GetSymbol((LPVOID)stk[i].ProgramCounter, Buffer, &displacement);
                if (!BlockerTreeWalk(&blockerCur, Buffer)) break ;
            }
            if (Buffer[0]) {
               strcat(Buffer, "+") ;
            }
            sprintf(Buffer+strlen(Buffer), "0x%x", displacement );
        }
    }
}

#define BEGIN_TREE()
#define END_TREE()
#define DECLARE_ENTRY(foo) BlockerTreeDeclareEntry(foo)
#define BEGIN_LIST() BlockerTreeListBegin()
#define END_LIST() BlockerTreeListEnd()

PBLOCKER_TREE gpCurrentBlocker ;

VOID
BlockerTreeListBegin(
   VOID
   )
{
   //dprintf("Nest for %x\n", gpCurrentBlocker) ;
   ASSERT(!gpCurrentBlocker->Nested) ;
   gpCurrentBlocker->Nested = TRUE ;
}

VOID
BlockerTreeListEnd(
   VOID
   )
{
   //dprintf("Unnest for %x\n", gpCurrentBlocker) ;
   gpCurrentBlocker = gpCurrentBlocker->Parent ;
   ASSERT(gpCurrentBlocker->Nested) ;
   gpCurrentBlocker->Nested = FALSE ;
}

VOID
BlockerTreeDeclareEntry(
   const char *szSymbolic
   )
{
   PBLOCKER_TREE blockerEntry ;

   blockerEntry = (PBLOCKER_TREE) malloc(sizeof(BLOCKER_TREE)) ;
   if (!blockerEntry) {
      return ;
   }

   memset(blockerEntry, 0, sizeof(BLOCKER_TREE)) ;
   blockerEntry->Symbolic = szSymbolic ;

   if (gpCurrentBlocker->Nested) {
      ASSERT(!gpCurrentBlocker->Child) ;
      //dprintf("Child %x for %x\n", blockerEntry, gpCurrentBlocker) ;
      blockerEntry->Parent = gpCurrentBlocker ;
      gpCurrentBlocker->Child = blockerEntry ;
   } else {
      ASSERT(!gpCurrentBlocker->Sibling) ;
      //dprintf("sibling %x for %x\n", blockerEntry, gpCurrentBlocker) ;
      blockerEntry->Parent = gpCurrentBlocker->Parent ;
      gpCurrentBlocker->Sibling = blockerEntry ;
   }
   gpCurrentBlocker = blockerEntry ;
}

PBLOCKER_TREE
BlockerTreeBuild(
   VOID
   )
{
   BLOCKER_TREE blockerHead ;

   memset(&blockerHead, 0, sizeof(BLOCKER_TREE)) ;

   gpCurrentBlocker = &blockerHead ;

   //
   // Generate the list...
   //
   #include "stacks.h"

   //
   // And return it.
   //
   return blockerHead.Sibling ;
}

VOID BlockerTreeFree(
   PBLOCKER_TREE BlockerHead
   )
{
   PBLOCKER_TREE blockerCur, blockerNext ;

   for(blockerCur = BlockerHead; blockerCur; blockerCur = blockerNext) {
      if (blockerCur->Child) {
         BlockerTreeFree(blockerCur->Child) ;
      }
      blockerNext = blockerCur->Sibling ;
      free(blockerCur) ;
   }
}

BOOL
BlockerTreeWalk(
   IN OUT PBLOCKER_TREE *blockerHead,
   IN char *szSymbolic
   )
{
   PBLOCKER_TREE blockerCur ;

   for(blockerCur = *blockerHead; blockerCur; blockerCur = blockerCur->Sibling) {
      if (!_strcmpi(blockerCur->Symbolic, szSymbolic)) {
         *blockerHead = blockerCur->Child ;
         return TRUE ;
      }
   }
   return FALSE ;
}


#if defined(TARGET_i386)
#define SYSTEM_TIME_ADDRESS  KI_USER_SHARED_DATA
#elif defined(TARGET_MIPS)
#define SYSTEM_TIME_ADDRESS  KIPCR2
#elif defined(TARGET_ALPHA)
#define SYSTEM_TIME_ADDRESS  KI_USER_SHARED_DATA
#elif defined(TARGET_PPC)
#define SYSTEM_TIME_ADDRESS  KI_USER_SHARED_DATA
#else
#error( "unknown target machine" );
#endif


