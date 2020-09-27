/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    process.c

Abstract:

    WinDbg Extension Api

Author:

    Wesley Witt (wesw) 15-Aug-1993

Environment:

    User Mode.

Revision History:

--*/

#include "precomp.h"
#pragma hdrstop

extern ULONG STeip, STebp, STesp;

PKPROCESS ProcessLastDump;
ULONG ThreadLastDump;

ULONG TotalProcessCommit;

#ifdef TARGET_i386
VOID GetStackTraceRegs(ULONG,PULONG,PULONG,PULONG);
#endif

BOOLEAN
GetTheSystemTime (
    OUT PLARGE_INTEGER Time
    );

CHAR * SecImpLevel[] = {
            "Anonymous",
            "Identification",
            "Impersonation",
            "Delegation" };

#define SecImpLevels(x) (x < sizeof( SecImpLevel ) / sizeof( PSTR ) ? \
                        SecImpLevel[ x ] : "Illegal Value" )

typedef BOOLEAN (WINAPI *PENUM_PROCESS_CALLBACK)(PVOID ProcessAddress, PVOID Process, PVOID ThreadAddress, PVOID Thread);


DECLARE_API( process )

/*++

Routine Description:

    Dumps the active process list.

Arguments:

    None.

Return Value:

    None.

--*/

{
    ULONG ProcessToDump;
    ULONG Flags;
    ULONG Result;
    LIST_ENTRY List;
    PLIST_ENTRY Next;
    ULONG ProcessHead;
    PKPROCESS Process;
    KPROCESS ProcessContents;
    PETHREAD Thread;
    ETHREAD ThreadContents;
    PCHAR ImageFileName;
    CHAR  Buf[256];

    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (dwCurrentPc);

    ProcessToDump = 0xFFFFFFFF;
    Flags = 0xFFFFFFFF;

    RtlZeroMemory(Buf, 256);
    sscanf(args,"%lx %lx %s",&ProcessToDump, &Flags, Buf);

    if (Buf[0] != '\0') {
        ImageFileName = Buf;
    } else {
        ImageFileName = NULL;
    }

    if (ProcessToDump == 0xFFFFFFFF || ProcessToDump == 0) {
        ProcessToDump = GetNtDebuggerDataValue(PsActiveProcessHead);
        if (ProcessToDump == 0) {
            ProcessToDump = (ULONG)GetCurrentProcessAddress( dwProcessor, hCurrentThread, NULL );
            if (ProcessToDump == 0) {
                dprintf("Unable to get current process pointer.\n");
                return;
            }
        }
        if (Flags == 0xFFFFFFFF) {
            Flags = 3;
        }
    }

    Process = (PKPROCESS)ProcessToDump;

    if (!ReadMemory( (DWORD)Process, &ProcessContents, sizeof(KPROCESS), &Result )) {
        dprintf("Unable to read _KPROCESS at %lx\n",Process);
        return;
    }

    if (DumpProcess ("", &ProcessContents, Process, Flags, ImageFileName) && (Flags & 6)) {
        Next = ProcessContents.ThreadListHead.Flink;

        while ( Next != &Process->ThreadListHead) {

            Thread = (PETHREAD)(CONTAINING_RECORD(Next,KTHREAD,ThreadListEntry));
            if (!ReadMemory((DWORD)Thread,
                            &ThreadContents,
                            sizeof(ETHREAD),
                            &Result)) {
                dprintf("Unable to read _ETHREAD at %lx\n",Thread);
                break;
            }

            if (!DumpThread(dwProcessor,"        ", &ThreadContents, Thread, Flags)) {
                break;
                }


            Next = ((PKTHREAD)&ThreadContents)->ThreadListEntry.Flink;

            if (CheckControlC()) {
                return;
            }
        }
        EXPRLastDump = (ULONG)Process;
        ProcessLastDump = Process;
        dprintf("\n");
    }

    return;
}

ULONG_PTR
FindThreadFromStackPointerThisProcess(
    ULONG_PTR StackPointer,
    ULONG_PTR Process
    )
{
    ULONG_PTR list;
    LIST_ENTRY listValue;
    ULONG result;
    ULONG_PTR next;
    ULONG_PTR thread;
    ULONG_PTR threadHead;
    ULONG_PTR stackBase;
    ULONG_PTR stackBaseValue;
    ULONG_PTR stackLimit;
    ULONG_PTR stackLimitValue;

    //
    // Calculate a pointer to the ThreadListHead within that structure and
    // read it.
    //

    list = Process + FIELD_OFFSET( KPROCESS, ThreadListHead );
    if (ReadMemory( list,
                    &listValue,
                    sizeof(ULONG_PTR),
                    &result ) == 0) {
        dprintf("Unable to get value of thread list head\n");
        return 0;
    }

    next = (ULONG_PTR)listValue.Flink;
    threadHead = list;

    while (next != threadHead) {

        //
        // On control-c readmemory will fail and this call would return
        //

        //
        // Derive a pointer to the thread structure
        //

        thread = (ULONG_PTR)CONTAINING_RECORD( (PVOID)next,
                                               KTHREAD,
                                               ThreadListEntry );

        //
        // We need two values from the thread structure: the kernel thread
        // base and the kernel thread limit.
        //

        stackBase = thread +
                    FIELD_OFFSET( ETHREAD, Tcb ) +
                    FIELD_OFFSET( KTHREAD, StackBase );

        stackLimit = thread +
                    FIELD_OFFSET( ETHREAD, Tcb ) +
                    FIELD_OFFSET( KTHREAD, StackLimit );

        if (ReadMemory( stackBase,
                        &stackBaseValue,
                        sizeof(stackBaseValue),
                        &result ) == 0) {
            dprintf("Unable to get value of stack base (0x%08x)\n",
                     stackBase);
            return 0;
        }

        if (StackPointer <= stackBaseValue) {

            if (ReadMemory( stackLimit,
                            &stackLimitValue,
                            sizeof(stackLimitValue),
                            &result ) == 0) {
                dprintf("Unable to get value of stack limit\n");
                return 0;
            }

            if (StackPointer >  stackLimitValue) {

                //
                // We have found our thread.
                //

                return thread;
            }
        }

        //
        // Look at the next thread
        //

        list = thread +
               FIELD_OFFSET( KTHREAD, ThreadListEntry );

        if (ReadMemory( list,
                        &listValue,
                        sizeof(ULONG_PTR),
                        &result ) == 0) {
            dprintf("Unable to read value of ThreadListEntry\n");
            return 0;
        }

        next = (ULONG_PTR)listValue.Flink;
    }

    return 0;
}



ULONG_PTR
FindThreadFromStackPointer(
    ULONG_PTR StackPointer
    )
{
    ULONG_PTR processHead;
    ULONG_PTR list;
    LIST_ENTRY listValue;
    ULONG_PTR next;
    ULONG_PTR process;
    ULONG_PTR thread;
    ULONG result;

    //
    // First check the idle process, which is not included in the PS
    // process list.
    //

    process = GetExpression( "xboxkrnl!KeIdleProcess" );
    if (process != 0) {

        if (ReadMemory( process,
                        &process,
                        sizeof(PVOID),
                        &result ) != 0) {

            thread = FindThreadFromStackPointerThisProcess( StackPointer,
                                                            process );

            if (thread != 0) {
                return thread;
            }
        }
    }

    //
    // Now check the single system process.
    //

    process = GetNtDebuggerDataValue(PsActiveProcessHead);
    if (process != 0) {

        thread = FindThreadFromStackPointerThisProcess( StackPointer,
                                                        process );

        if (thread != 0) {
            return thread;
        }
    }

    return 0;
}

DECLARE_API( thread )

/*++

Routine Description:

    Dumps the specified thread.

Arguments:

    None.

Return Value:

    None.

--*/

{
    ULONG       Address;
    ULONG       Flags;
    ULONG       result;
    PETHREAD    Thread;
    ETHREAD     ThreadContents;
    char        threadExprBuf[256] ;

    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (dwCurrentPc);

    Address = 0xFFFFFFFF;
    Flags = 6;

    threadExprBuf[0] = '\0';

    sscanf(args,"%s %lx",threadExprBuf,&Flags);

    if (threadExprBuf[0]) {
        if (IsHexNumber(threadExprBuf)) {
            sscanf(threadExprBuf, "%lx", &Address) ;
        } else {
            Address = GetExpression(threadExprBuf);
            if (Address==0) {
                dprintf("An error occured trying to evaluate the expression\n") ;
                return;
            }
        }
    }

    if (Address == 0xFFFFFFFF) {
        Address = (ULONG)GetCurrentThreadAddress( (USHORT)dwProcessor, hCurrentThread);
    }

    Thread = (PETHREAD)(PVOID)Address;
    if ( !ReadMemory( (DWORD)Thread,
                      &ThreadContents,
                      sizeof(ETHREAD),
                      &result) ) {
        dprintf("%08lx: Unable to get thread contents\n", Thread );
        return;
    }

    if (ThreadContents.Tcb.Header.Type != ThreadObject &&
        Address > MM_USER_PROBE_ADDRESS) {

        ULONG_PTR stackThread;

        //
        // What was passed in was not a thread.  Maybe it was a kernel stack
        // pointer.  Search the thread stack ranges to find out.
        //

        dprintf("%x is not a thread object, interpreting as stack value...\n",Address);
        stackThread = FindThreadFromStackPointer( (ULONG_PTR)Address );
        if (stackThread != 0) {

            if ( !ReadMemory( stackThread,
                              &ThreadContents,
                              sizeof(ETHREAD),
                              &result) ) {
                dprintf("%08lx: Unable to get thread contents\n", Address );
                return;
            }

            Thread = (PETHREAD)stackThread;
        }
    }

    DumpThread (dwProcessor,"", &ThreadContents, Thread, Flags);
    EXPRLastDump = (ULONG)Thread;
    ThreadLastDump = (ULONG)Thread;
    return;

}

BOOL
DumpProcess(
    IN char * pad,
    IN PKPROCESS ProcessContents,
    IN PKPROCESS RealProcessBase,
    IN ULONG Flags,
    IN PCHAR    ImageFileName OPTIONAL
    )
{
    OBJECT_HANDLE_TABLE HandleTable;
    ULONG NumberOfHandles;
    WCHAR Buf[256];
    ULONG Result;
    STRING  string1, string2;

    //
    // Get the image file name
    //
    strcpy((PCHAR)Buf,"System Process");

    if (ImageFileName != NULL) {
        RtlInitString(&string1, ImageFileName);
        RtlInitString(&string2, (PCSZ) Buf);
        if (RtlCompareString(&string1, &string2, TRUE) != 0) {
            return TRUE;
        }
    }

    NumberOfHandles = 0;
    if (ReadMemory((DWORD)GetNtDebuggerData(ObpObjectHandleTable),
                     &HandleTable,
                     sizeof(HandleTable),
                     &Result)) {

        NumberOfHandles = HandleTable.HandleCount;
    }

    dprintf("%sPROCESS %08lx\n",
            pad,
            RealProcessBase
           );

    dprintf("%s    ObjectTable: %08lx  TableSize: %3u.\n",
            pad,
            GetNtDebuggerData(ObpObjectHandleTable),
            NumberOfHandles
            );

    dprintf("%s    Image: %s\n",pad,Buf);

    if (!(Flags & 1)) {
        dprintf("\n");
        return TRUE;
    }

    dprintf("%s    BasePriority                      %ld\n", pad, ProcessContents->BasePriority);


    dprintf("\n");
    return TRUE;
}


UCHAR *WaitReasonList[] = {
    (PUCHAR) "Executive",
    (PUCHAR) "FreePage",
    (PUCHAR) "PageIn",
    (PUCHAR) "PoolAllocation",
    (PUCHAR) "DelayExecution",
    (PUCHAR) "Suspended",
    (PUCHAR) "UserRequest",
    (PUCHAR) "WrExecutive",
    (PUCHAR) "WrFreePage",
    (PUCHAR) "WrPageIn",
    (PUCHAR) "WrPoolAllocation",
    (PUCHAR) "WrDelayExecution",
    (PUCHAR) "WrSuspended",
    (PUCHAR) "WrUserRequest",
    (PUCHAR) "WrEventPairHigh",
    (PUCHAR) "WrEventPairLow",
    (PUCHAR) "WrLpcReceive",
    (PUCHAR) "WrLpcReply",
    (PUCHAR) "WrVirtualMemory",
    (PUCHAR) "WrPageOut",
    (PUCHAR) "WrRendezvous",
    (PUCHAR) "WrFsCacheIn",
    (PUCHAR) "WrFsCacheOut",
    (PUCHAR) "Spare4",
    (PUCHAR) "Spare5",
    (PUCHAR) "Spare6",
    (PUCHAR) "Spare7"};

BOOL
DumpThread (
    IN ULONG Processor,
    IN char *Pad,
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase,
    IN ULONG Flags
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

    UNREFERENCED_PARAMETER (Processor);

    if (Thread->Tcb.Header.Type != ThreadObject) {
        dprintf("TYPE mismatch for thread object at %lx\n",RealThreadBase);
        return FALSE;
    }

    dprintf("%sTHREAD %lx  tid: %lx  ",
        Pad,
        RealThreadBase,
        Thread->UniqueThread
        );


    switch (Thread->Tcb.State) {
        case Initialized:
            dprintf("%s","INITIALIZED");break;
        case Ready:
            dprintf("%s","READY");break;
        case Running:
            dprintf("%s","RUNNING");break;
        case Standby:
            dprintf("%s","STANDBY");break;
        case Terminated:
            dprintf("%s","TERMINATED");break;
        case Waiting:
            dprintf("%s","WAIT");break;
        case Transition:
            dprintf("%s","TRANSITION");break;
    }

    if (!(Flags & 2)) {
        dprintf("\n");
        return TRUE;
        }

    if (Thread->Tcb.State == Waiting) {
        dprintf(": (%s) %s %s\n",
            WaitReasonList[Thread->Tcb.WaitReason],
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
    } else {
        dprintf("\n");
    }

BadWaitBlock:
    if (!(Flags & 4)) {
        dprintf("\n");
        return TRUE;
    }


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

    Process = Thread->Tcb.ApcState.Process;
    dprintf("%sOwning Process %lx  XapiThread %lx\n", Pad, Process, Thread->Tcb.TlsData);

    GetTheSystemTime (&RunTime);

    dprintf("%sWaitTime (ticks)        %ld\n",
              Pad,
              Thread->Tcb.WaitTime);

    dprintf("%sContext Switch Count    %ld",
              Pad,
              Thread->Tcb.ContextSwitches);

    dprintf ("\n");

    //
    // Get the time increment value which is used to compute runtime.
    //
    TimeIncrement = GetNtDebuggerDataValue( KeTimeIncrement );

    RunTime.QuadPart = UInt32x32To64(Thread->Tcb.KernelTime, TimeIncrement);
    RtlTimeToElapsedTimeFields ( &RunTime, &Times);
    dprintf("%sKernelTime              %3ld:%02ld:%02ld.%04ld\n",
              Pad,
              Times.Hour,
              Times.Minute,
              Times.Second,
              Times.Milliseconds);

    dumpSymbolicAddress((ULONG)Thread->StartAddress, (PUCHAR)Buffer, TRUE);
    dprintf("%sStart Address %s\n",
        Pad,
        Buffer
        );

    dprintf("%sStack Current %lx Base %lx Limit %lx\n",
        Pad,
        Thread->Tcb.KernelStack,
        Thread->Tcb.StackBase,
        Thread->Tcb.StackLimit
        );

    dprintf("%sPriority %ld BasePriority %ld PriorityDecrement %ld DecrementCount %ld\n",
        Pad,
        Thread->Tcb.Priority,
        Thread->Tcb.BasePriority,
        Thread->Tcb.PriorityDecrement,
        Thread->Tcb.DecrementCount
        );

#ifdef TARGET_i386

    if (Thread->Tcb.State == Running) {
        ULONG Cpu;
        PROCESSORINFO ProcessorInfo;

        //
        // Default to the old (broken) processor number.
        //

        Cpu = 0;

        if (Ioctl(IG_KD_CONTEXT, &ProcessorInfo, sizeof(ProcessorInfo))) {
            if (ProcessorInfo.NumberProcessors != 1) {
                PVOID * KiProcessorBlock;

                KiProcessorBlock = (PVOID *)GetExpression("xboxkrnl!KiProcessorBlock");
                if (KiProcessorBlock) {
                    for (i = 0; i < ProcessorInfo.NumberProcessors; i++) {

                        //
                        // Check the PRCB for this processor to see
                        // if the current thread is the thread we are
                        // dumping.
                        //

                        PKPRCB ProcessorBlockN;
                        PVOID  ProcessorThread;

                        ReadMemory((DWORD)(KiProcessorBlock + i),
                                   &ProcessorBlockN,
                                   sizeof(ProcessorBlockN),
                                   &Result);

                        if (Result != sizeof(ProcessorBlockN)) {

                            //
                            // Read failed, give up.
                            //

                            break;
                        }

                        ReadMemory((DWORD)&ProcessorBlockN->CurrentThread,
                                   &ProcessorThread,
                                   sizeof(ProcessorThread),
                                   &Result);
                        if (Result != sizeof(ProcessorThread)) {

                            //
                            // Couldn't read CurrentThread from PRCB
                            // something's wrong, give up.
                            //

                            break;
                        }

                        if (ProcessorThread == (PVOID)RealThreadBase) {

                            //
                            // Found it!
                            //

                            Cpu = i;
                            break;
                        }
                    }

                    //
                    // We get here, either we found it or we didn't.
                    // If we didn't find it, it's set to the processor
                    // in the same way as it was before we decided to
                    // search for it,... 
                    //
                }
            }
        }
        GetStackTraceRegs( Cpu, &STeip, &STebp, &STesp );
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

            //
            // Terminated threads don't necessarily have decent switch frames.
            //

            if ((Thread->Tcb.State != Terminated) ||
                (PAGE_ALIGN(STebp) == PAGE_ALIGN(STesp))) {

                SetThreadForOperation( (PULONG)&RealThreadBase );
                frames = StackTrace( STebp, STesp, STeip, stk, MAX_STACK_FRAMES );
            }
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

    for (i=0; i<frames; i++) {

        if (i==0) {
#ifdef TARGET_PPC
            dprintf( "\n%sChildEBP RetAddr\n", Pad );
#else
            dprintf( "\n%sChildEBP RetAddr  Args to Child\n", Pad );
#endif
        }

        Buffer[0] = '!';
        GetSymbol((LPVOID)stk[i].ProgramCounter, (PUCHAR)Buffer, &displacement);

#ifdef TARGET_PPC
        dprintf( "%s%08x %08x %s",
                 Pad,
                 stk[i].FramePointer,
                 stk[i].ReturnAddress,
                 Buffer
               );
#else
        dprintf( "%s%08x %08x %08x %08x %08x %s",
                 Pad,
                 stk[i].FramePointer,
                 stk[i].ReturnAddress,
                 stk[i].Args[0],
                 stk[i].Args[1],
                 stk[i].Args[2],
                 Buffer
               );
#endif

        if (displacement) {
            dprintf( "+0x%x", displacement );
        }

        dprintf( "\n" );
    }

    dprintf("\n");
    return TRUE;
}

DECLARE_API( threadfields )

/*++

Routine Description:

    Displays the field offsets for ETHREAD type.

Arguments:

    None.

Return Value:

    None.

--*/

{
    UNREFERENCED_PARAMETER (args);
    UNREFERENCED_PARAMETER (hCurrentThread);
    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (dwProcessor);
    UNREFERENCED_PARAMETER (dwCurrentPc);

    dprintf(" ETHREAD structure offsets:\n\n");

    dprintf("    Tcb:                           0x%lx\n", FIELD_OFFSET(ETHREAD, Tcb) );
    dprintf("    CreateTime:                    0x%lx\n", FIELD_OFFSET(ETHREAD, CreateTime) );
    dprintf("    ExitTime:                      0x%lx\n", FIELD_OFFSET(ETHREAD, ExitTime) );
    dprintf("    ExitStatus:                    0x%lx\n", FIELD_OFFSET(ETHREAD, ExitStatus) );
    dprintf("    ReaperListEntry:               0x%lx\n", FIELD_OFFSET(ETHREAD, ReaperListEntry) );
    dprintf("    ActiveTimerListHead:           0x%lx\n", FIELD_OFFSET(ETHREAD, ActiveTimerListHead) );

    dprintf("    IrpList:                       0x%lx\n", FIELD_OFFSET(ETHREAD, IrpList) );
    dprintf("    HasTerminated:                 0x%lx\n", FIELD_OFFSET(KTHREAD, HasTerminated) );
    dprintf("    StartAddress:                  0x%lx\n", FIELD_OFFSET(ETHREAD, StartAddress) );

    return;

}

PVOID
GetCurrentProcessAddress(
    DWORD    Processor,
    HANDLE   hCurrentThread,
    PETHREAD CurrentThread
    )

{
    ULONG Result;
    ETHREAD Thread;

    if (CurrentThread == NULL) {
        CurrentThread = (PETHREAD)GetCurrentThreadAddress( (USHORT)Processor, hCurrentThread );
        if (CurrentThread == NULL) {
            return NULL;
        }
    }

    if (!ReadMemory((DWORD)CurrentThread, &Thread, sizeof(Thread), &Result)) {
        return NULL;
    }

    return Thread.Tcb.ApcState.Process;
}

PVOID
GetCurrentThreadAddress(
    USHORT Processor,
    HANDLE hCurrentThread
    )
{
    ULONG Address;

#ifdef TARGET_ALPHA

    UNREFERENCED_PARAMETER (hCurrentThread);

    ReadControlSpace( (USHORT)Processor,
                      DEBUG_CONTROL_SPACE_THREAD,
                      (PVOID)&Address,
                      sizeof(PKTHREAD)
                     );

    return CONTAINING_RECORD(Address, ETHREAD, Tcb);

#elif defined(TARGET_MIPS)

    KPRCB  Prcb;

    if (!ReadPcr(Processor, &Prcb, &Address, hCurrentThread)) {
        dprintf("Unable to read PCR for Processor %u\n",Processor);
        return NULL;
    }

    return CONTAINING_RECORD(Prcb.CurrentThread,ETHREAD,Tcb);

#elif defined(TARGET_PPC)

    KPCR  Pcr;
    PKPCR pp;
    KPRCB Prcb;
    ULONG Result;

    Processor = 0;
    pp = &Pcr;
    if (!ReadPcr(Processor, pp, &Address, hCurrentThread)) {
        dprintf("Unable to read PCR for Processor %u\n",Processor);
        return NULL;
    }

    //
    // Address -> base of the pcr, read the PCR in.
    //

    if (!ReadMemory((DWORD)pp->Prcb, &Prcb, sizeof(KPRCB), &Result)) {
        return NULL;
    }

    return CONTAINING_RECORD(Prcb.CurrentThread,ETHREAD,Tcb);

#elif defined(TARGET_i386)

    KPCR  Pcr;
    PKPCR pp;

    pp = &Pcr;
    if (!ReadPcr(Processor, pp, &Address, hCurrentThread)) {
        dprintf("Unable to read PCR for Processor %u\n",Processor);
        return NULL;
    }

    return CONTAINING_RECORD(pp->PrcbData.CurrentThread,ETHREAD,Tcb);

#else

#error( "unknown processor type" )

#endif
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



//+---------------------------------------------------------------------------
//
//  Function:   GetHandleTableAddress
//
//  Synopsis:   Return the address of the handle table given a thread handle
//
//  Arguments:  [Processor]      -- processor number
//              [hCurrentThread] -- thread handle
//
//  Returns:    address of handle table or null
//
//  History:    9-23-1998   benl   Created
//
//  Notes:
//
//----------------------------------------------------------------------------

PVOID GetHandleTableAddress(
    USHORT Processor,
    HANDLE hCurrentThread
    )
{
    return (PVOID)GetNtDebuggerDataValue(ObpObjectHandleTable);
} // GetHandleTableAddress


BOOLEAN
GetTheSystemTime (
    OUT PLARGE_INTEGER Time
    )
{
    ULONG              Result;

    ZeroMemory( Time, sizeof(*Time) );

    if (!ReadMemory( GetExpression("xboxkrnl!KeSystemTime"),
                     Time,
                     sizeof(*Time),
                     &Result) ) {
        dprintf("Unable to get system time\n");
        return FALSE;
    }

    return TRUE;
}

VOID
dumpSymbolicAddress(
    ULONG Address,
    PUCHAR Buffer,
    BOOL AlwaysShowHex
    )
{
    ULONG displacement;
    PUCHAR s;

    Buffer[0] = '!';
    GetSymbol((LPVOID)Address, Buffer, &displacement);
    s = Buffer + strlen( (PCHAR)Buffer );
    if (s == Buffer) {
        sprintf( (PCHAR)s, "0x%08x", Address );
        }
    else {
        if (displacement != 0) {
            sprintf( (PCHAR)s, "+0x%x", displacement );
            }
        if (AlwaysShowHex) {
            sprintf( (PCHAR)s, " (0x%08x)", Address );
            }
        }

    return;
}

PVOID
LookupUniqueId(
    HANDLE UniqueId
    )
{
    return NULL;
}

int
__cdecl
CmpFunc(
    const void *pszElem1,
    const void *pszElem2
    )
{
    PPROCESS_COMMIT_USAGE p1, p2;

    p1 = (PPROCESS_COMMIT_USAGE)pszElem1;
    p2 = (PPROCESS_COMMIT_USAGE)pszElem2;
    return p2->CommitCharge - p1->CommitCharge;
}

PPROCESS_COMMIT_USAGE
GetProcessCommit (
    PULONG TotalCommitCharge,
    PULONG NumberOfProcesses
    )
{
    PPROCESS_COMMIT_USAGE p;
    ULONG n;
    PKPROCESS Process;
    KPROCESS ProcessContents;
    ULONG Total;
    ULONG Result;

    Total = 0;

    n = 0;
    p = HeapAlloc( GetProcessHeap(), 0, 1 );

    Process = (PKPROCESS)GetNtDebuggerDataValue( PsActiveProcessHead );
    if (!Process) {
        dprintf("Unable to get value of PsActiveProcessHead\n");
        return 0;
    }

    if (!ReadMemory( (DWORD)Process, &ProcessContents, sizeof(KPROCESS), &Result )) {
        dprintf("Unable to read _KPROCESS at %lx\n",Process);
        return 0;
    }

    n += 1;
    p = HeapReAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, p, n * sizeof( *p ) );
    if (p != NULL) {
        strcpy( (PCHAR)p[ n-1 ].ImageFileName, "unknown");
        p[ n-1 ].CommitCharge = 0;
        p[ n-1 ].NumberOfPrivatePages = 0;
        p[ n-1 ].NumberOfLockedPages = 0;
    }

    qsort( p, n, sizeof( *p ), CmpFunc );

    *TotalCommitCharge = Total;
    *NumberOfProcesses = n;
    return p;
}


/*++

Routine Description:

    Helper function exported for other extensions to walk the process tree

Arguments:

    Callback -- a function to callback to for every thread.

Return Value:

    None.

--*/



VOID
EnumerateProcesses (
    IN PENUM_PROCESS_CALLBACK Callback
    )
{
    ULONG Result;
    PLIST_ENTRY NextThread;
    PLIST_ENTRY NextProcess;
    PKPROCESS Process;
    KPROCESS ProcessContents;
    PETHREAD Thread;
    ETHREAD  ThreadContents;

    Process = (PKPROCESS)GetNtDebuggerDataValue( PsActiveProcessHead );
    if (!Process) {
        dprintf("Unable to get value of PsActiveProcessHead, 0x%x\n", KdDebuggerData.PsActiveProcessHead);
        return;
    }

    if (!ReadMemory( (DWORD)Process, &ProcessContents, sizeof(KPROCESS), &Result )) {
        dprintf( "Unable to read _KPROCESS at %lx\n",Process );
        return;
    }

    NextThread = ProcessContents.ThreadListHead.Flink;
    while ( NextThread != &Process->ThreadListHead ) {

        Thread = (PETHREAD)(CONTAINING_RECORD( NextThread, KTHREAD, ThreadListEntry));

        if (!ReadMemory( (DWORD)Thread,
                         &ThreadContents,
                         sizeof( ETHREAD ),
                         &Result)) {
            dprintf( "Unable to read _ETHREAD at %lx in process 0x%x\n", Thread, Process );
            break;
        }

        NextThread = ((PKTHREAD)&ThreadContents)->ThreadListEntry.Flink;

        //
        //  Do callback
        //

        if (!Callback( Process, &ProcessContents, Thread, &ThreadContents )) {
            return;
        }

        if (CheckControlC()) {
            return;
        }

    } //  endwhile traversing threads
}

ULONG_PTR ZombieCount;
ULONG_PTR ZombiePool;
ULONG_PTR ZombieCommit;
ULONG_PTR ZombieResidentAvailable;

#define BLOB_LONGS 32

BOOLEAN WINAPI
CheckForZombieThread(
    IN PCHAR Tag,
    IN PCHAR Filter,
    IN ULONG Flags,
    IN PPOOL_HEADER PoolHeader,
    IN ULONG BlockSize,
    IN PVOID Data,
    IN PVOID Context
    )
{
    ULONG           result;
    ETHREAD         ThreadContents;
    PETHREAD        Thread;
    PKTHREAD        KThread;
    OBJECT_HEADER   ObjectHeaderContents;
    POBJECT_HEADER  ObjectHeader;
    ULONG           Blob[BLOB_LONGS];
    ULONG           i;
    PVOID           StackBase;
    PVOID           StackLimit;

    UNREFERENCED_PARAMETER (Flags);
    UNREFERENCED_PARAMETER (BlockSize);
    UNREFERENCED_PARAMETER (Context);

    if (PoolHeader == NULL) {
        return FALSE;
    }

    if ((PoolHeader->PoolIndex & 0x80) == 0) {
        return FALSE;
    }

    if (!CheckSingleFilter (Tag, Filter)) {
        return FALSE;
    }

    if ((PoolHeader->BlockSize << POOL_BLOCK_SHIFT) < sizeof(Blob)) {
        return FALSE;
    }

    //
    // There must be a better way to find the object header given the start
    // of a pool block ?
    //

    if (!ReadMemory ((ULONG) Data,
                    &Blob[0],
                    sizeof(Blob),
                    &result)) {
        dprintf ("Could not read process blob at %p\n", Data);
        return FALSE;
    }

    for (i = 0; i < BLOB_LONGS; i += 1) {
        KThread = (PKTHREAD)&Blob[i];
        if ((KThread->Header.Type == ThreadObject) &&
            (KThread->Header.Size == sizeof(KTHREAD) / sizeof(LONG))) {

            break;
        }
    }

    if (i == BLOB_LONGS) {
        return FALSE;
    }

    ObjectHeader = OBJECT_TO_OBJECT_HEADER ((PULONG)Data + i);

    if (!ReadMemory ((ULONG) ObjectHeader,
                    &ObjectHeaderContents,
                    sizeof(OBJECT_HEADER),
                    &result)) {
        dprintf ("Could not read process object header at %p\n", ObjectHeader);
        return FALSE;
    }

    Thread = (PETHREAD)(&ObjectHeader->Body);

    if (!ReadMemory ((ULONG) Thread,
                    &ThreadContents,
                    sizeof(ETHREAD),
                    &result)) {

        dprintf ("Could not read thread data at %p\n", Thread);
        return FALSE;
    }

    KThread = (PKTHREAD)&ThreadContents;

    if (KThread->State != Terminated) {
        return FALSE;
    }

    dprintf ("HandleCount: %u  PointerCount: %u\n",
            ObjectHeaderContents.HandleCount, ObjectHeaderContents.PointerCount);
    DumpThread (0, "", &ThreadContents, Thread, 7);

    ZombieCount += 1;

    ZombiePool += (PoolHeader->BlockSize << POOL_BLOCK_SHIFT);
    ZombieCommit += ((PCHAR)KThread->StackBase - (PCHAR)KThread->StackLimit);

    StackBase = (PVOID)((PCHAR)KThread->StackBase - 1);
    StackLimit = KThread->StackLimit;

    while (StackBase >= StackLimit) {
        if (GetAddressState(StackBase) == ADDRESS_VALID) {
            ZombieResidentAvailable += PAGE_SIZE;
        }
        StackBase = (PVOID)((PCHAR)StackBase - PAGE_SIZE);
    }

    return TRUE;
}

DECLARE_API( zombies )

/*++

Routine Description:

    Finds zombie processes and threads in non-paged pool.

Arguments:

    None.

Return Value:

    None.

--*/


{
    ULONG       Flags;
    ULONG       RestartAddress;
    ULONG       TagName;

    UNREFERENCED_PARAMETER (hCurrentThread);
    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (dwProcessor);
    UNREFERENCED_PARAMETER (dwCurrentPc);

    Flags = 1;
    RestartAddress = 0;

    sscanf(args,"%lx %lx", &Flags, &RestartAddress);

    if ((Flags & 0x3) == 0) {
        dprintf("Invalid parameter for !zombies\n");
        return;
    }

    if (Flags & 0x2) {

        dprintf("Looking for zombie threads...");

        TagName = '?rhT';
    
        ZombieCount = 0;
        ZombiePool = 0;
        ZombieCommit = 0;
        ZombieResidentAvailable = 0;

        SearchPool (TagName, RestartAddress, &CheckForZombieThread, NULL);
        SearchPool (TagName, RestartAddress, &CheckForZombieThread, NULL);

    }

    //
    // Print summary statistics last so they don't get lost on screen scroll.
    //

    if (Flags & 0x2) {
        if (ZombieCount == 0) {
            dprintf ("\n\n************ NO zombie threads found ***********\n");
        }
        else {
            dprintf ("\n\n************ %d zombie threads found ***********\n", ZombieCount);
            dprintf ("       Resident page cost : %8ld Kb\n",
                ZombieResidentAvailable / 1024);
            dprintf ("       Commit cost :        %8ld Kb\n",
                ZombieCommit / 1024);
            dprintf ("       Pool cost :          %8ld bytes\n",
                ZombiePool);
        }
    }

    return;
}

