/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    init.c

Abstract:

    Entry point and initialization routines for test harness

--*/

#include "precomp.h"
#include "loader.h"
#include "logging.h"
#include "xlaunch.h"

volatile BOOL HarnessQuitFlag = FALSE;
SIZE_T DriveCount = 0;

//
// Unhandled Exception Filter
//

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter;

//
// Current fscache size in bytes
//

SIZE_T HarnessCurrentCacheSize;

LONG
WINAPI
HarnessUnhandledExceptionFilter(
    LPEXCEPTION_POINTERS ExceptionInfo
    )
{
    int x = 0;
    int offset = 0;
    DWORD probe;
    BOOL ProbeFlag = FALSE;

    DbgPrint(
        "HARNESS: Unhandled exception 0x%x (%s)\n  type \"!cxr 0x%p; !kb\" to get stack trace\n\n",
        ExceptionInfo->ExceptionRecord->ExceptionCode,
        NtStatusSymbolicName(ExceptionInfo->ExceptionRecord->ExceptionCode),
        ExceptionInfo->ContextRecord
        );

    //
    // print out the entire excetion record and context to make debugging easier 
    // if a machine barfs and the serial dies
    // the exception information in the msvc system has more info, explore using this if neccessary
    //
    
    DbgPrint(
        "HARNESS: complete exception information:\nException Record:\n"   \
        "\tExceptionCode        0x%8.8x (%s)\n"    \
        "\tContextRecord:       0x%8.8x\n"         \
        "\tExceptionRecord:     0x%8.8x\n",
        ExceptionInfo->ExceptionRecord->ExceptionCode,
        NtStatusSymbolicName(ExceptionInfo->ExceptionRecord->ExceptionCode),
        ExceptionInfo->ContextRecord,
        ExceptionInfo->ExceptionRecord);
    
    DbgPrint(
        "HARNESS: Context Record:\n"  \
        "\tEax=%8.8x Ebx=%8.8x Ecx=%8.8x Edx=%8.8x Esi=%8.8x Edi=%8.8x\n" \
        "\tEip=%8.8x Esp=%8.8x Ebp=%8.8x EFlags=%8.8x\n" \
        "\tContextFlags=%8.8x\n" \
        "\tSegCs=%8.8x\n" \
        "\tSegSs=%8.8x\n" \
        "\tFloatSave=%8.8x\n",
        ExceptionInfo->ContextRecord->Eax,
        ExceptionInfo->ContextRecord->Ebx,
        ExceptionInfo->ContextRecord->Ecx,
        ExceptionInfo->ContextRecord->Edx,
        ExceptionInfo->ContextRecord->Esi,
        ExceptionInfo->ContextRecord->Edi,
        ExceptionInfo->ContextRecord->Eip,
        ExceptionInfo->ContextRecord->Esp,
        ExceptionInfo->ContextRecord->Ebp,
        ExceptionInfo->ContextRecord->EFlags,
        ExceptionInfo->ContextRecord->ContextFlags,
        ExceptionInfo->ContextRecord->SegCs,
        ExceptionInfo->ContextRecord->SegSs,
        ExceptionInfo->ContextRecord->FloatSave);

    //
    // stack dump from context ESP
    // probe it first
    // lots of hardcoded stuff, make flexable later
    //
    
    _try {
        probe = *(DWORD *)(ExceptionInfo->ContextRecord->Esp);
        probe = *(DWORD *)(ExceptionInfo->ContextRecord->Esp + 156);
    }
    _except(EXCEPTION_EXECUTE_HANDLER ) {
        ProbeFlag = TRUE;
    }
    
    if (FALSE == ProbeFlag) {
        DbgPrint("HARNESS: stack dump from context:\n");
        for (x = 0; x < 10; x++) {
            offset = 16*x;
            DbgPrint("\t%8.8x  %8.8x %8.8x %8.8x %8.8x\n",
                     ExceptionInfo->ContextRecord->Esp + offset,
                     *(DWORD *)(ExceptionInfo->ContextRecord->Esp + offset),
                     *(DWORD *)(ExceptionInfo->ContextRecord->Esp + offset + 4),
                     *(DWORD *)(ExceptionInfo->ContextRecord->Esp + offset + 8),
                     *(DWORD *)(ExceptionInfo->ContextRecord->Esp + offset + 12));
        }
    }

    //
    // so we have some idea of how long we have been running
    //

    DbgPrint( "HARNESS: the system has been running for %lu seconds\n", GetTickCount()/1000 );

    //
    // Use in-line assembly so that we can turn it off easier under
    // kernel debugger ("eb eip 90")
    //
    __asm int 3;

    if ( PreviousExceptionFilter ) {
        return PreviousExceptionFilter( ExceptionInfo );
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

VOID
QueryDriveSymbolicLink(
    OCHAR DriveLetter
    )
{
    OCHAR Drive[7];
    NTSTATUS Status;
    OBJECT_STRING NtFileName, Result;
    OBJECT_ATTRIBUTES ObjA;
    HANDLE LinkHandle;
    OCHAR ResultBuffer[256];

    //
    // Convert DOS style path name to NT style path name
    //

    Drive[0] = OTEXT('\\');
    Drive[1] = OTEXT('?');
    Drive[2] = OTEXT('?');
    Drive[3] = OTEXT('\\');
    Drive[4] = DriveLetter;
    Drive[5] = OTEXT(':');
    Drive[6] = 0;

    RtlInitObjectString( &NtFileName, Drive );

    InitializeObjectAttributes(
        &ObjA,
        &NtFileName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtOpenSymbolicLinkObject(
                &LinkHandle,
                &ObjA
                );

    if ( !NT_SUCCESS(Status) ) {
        return;
    }

    Result.Buffer = ResultBuffer;
    Result.Length = 0;
    Result.MaximumLength = sizeof(ResultBuffer);

    Status = NtQuerySymbolicLinkObject(
                LinkHandle,
                &Result,
                NULL
                );

    if ( NT_SUCCESS(Status) ) {
        DbgPrint( "HARNESS: found logical drive %c: (%Z)\n", DriveLetter, &Result );
        ++DriveCount;
    }

    NtClose( LinkHandle );
}

VOID
WINAPI
HarnessHangDetection(
    VOID
    )
/*++

Routine Description:

    This is a background thread used for detecting hung threads.

Arguments:

    None

Return Value:

    None, the thread never returns

--*/
{
    KIRQL Irql;
    PKPROCESS Process;
    PKTHREAD Thread;
    PLIST_ENTRY Next;
    ULONG CurrentTicks;
    ULONG DetectInterval;
    ULONG Timeout;

    Timeout = GetProfileIntA( "GeneralSettings", "HungTimeout", 300000 );

    DetectInterval = GetProfileIntA( "GeneralSettings",
                                     "HungDetectInterval", 60000 );

    SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_HIGHEST );

    for (;;) {

        KeRaiseIrql( DISPATCH_LEVEL, &Irql );
        Process = KeGetCurrentThread()->ApcState.Process;
        Next = Process->ThreadListHead.Flink;
        CurrentTicks = *KeTickCount;

        while ( Next != &Process->ThreadListHead ) {
            Thread = CONTAINING_RECORD( Next, KTHREAD, ThreadListEntry );
            if ( Thread->TlsData && Thread->State == Waiting && \
                 Thread->WaitMode == KernelMode && \
                 CurrentTicks-Thread->WaitTime > Timeout ) {
                DbgPrint( "HARNESS: thread %p seems to be hung for more than %d seconds\n",
                          Thread, (CurrentTicks-Thread->WaitTime)/1000 );
                if ( (HarnessCurrentCacheSize/PAGE_SIZE) > 16 ) {
                    __asm int 3
                }
            }
            Next = Thread->ThreadListEntry.Flink;
        }

        KeLowerIrql(Irql);
        Sleep( DetectInterval );
    }
}

VOID
WINAPI
HarnessFscSizerThread(
    VOID
    )
/*++

Routine Description:

    This is a thread that performs neccessay background processing. It
    randomly changes file cache size every one minute. It is useful for
    stress testing

Arguments:

    None

Return Value:

    None

--*/
{
    ULONG Seed;
    SIZE_T DefaultCacheSize;
    FILETIME SystemTime, LocalTime;
    CONST SmallFileCacheThresholdPages = DriveCount * 4;
    SYSTEMTIME xSystemTime;
    
    HarnessCurrentCacheSize = XGetFileCacheSize();
    DefaultCacheSize = HarnessCurrentCacheSize;
    GetSystemTimeAsFileTime( &SystemTime );
    FileTimeToLocalFileTime( &SystemTime, &LocalTime );
    Seed = LocalTime.dwHighDateTime + LocalTime.dwLowDateTime;

    for (;;) {

        //
        // Refresh settings from .ini file to see if it has been changed
        // We only do this in stress mode
        //

        if ( XTestLibIsStressTesting() ) {

            //
            // we want to avoid starving threads in low cache in stress
            // to do this, we use the total number of drives (DriveCount)
            // and multiply that by a wild guess (we'll start with 4) at the 
            // number of threads that will be accessing this device. We want to avoid
            // dropping below this number...
            //
            // stress has been running with very few pages, and this results in many 
            // threads getting blocked for long periods of time, whicc lessens the stress
            // load on the storage hardware
            //
            // we'll still do the minimum size, but we'll only do it a maximum of 25% of the time
            // note : we could get stuck at the minimum in low-memory situations
            //
            
            // the old way
            //if ( (RtlRandom(&Seed) % 100) > 50 ) {
            //    HarnessCurrentCacheSize = (RtlRandom(&Seed) % SmallFileCacheThresholdPages) + 1;
            //} else {
            //    HarnessCurrentCacheSize = (RtlRandom(&Seed) % (2048-SmallFileCacheThresholdPages)) + 1;
            //}

            // the new way
            switch(RtlRandom(&Seed) % 11)    {
                
                //
                // maximum cache size
                //

                case 0:
                    HarnessCurrentCacheSize = 2048;
                break;

                //
                // random but at least 'Minimum adequate' cache size
                //

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    HarnessCurrentCacheSize = (RtlRandom(&Seed) % (2048-SmallFileCacheThresholdPages)) + SmallFileCacheThresholdPages;
                break;

                //
                // 'Minimum adequate' cache size
                //

                case 10:
                    HarnessCurrentCacheSize = SmallFileCacheThresholdPages;
                break;
                
                //
                // Minimum cache size - we may not want to do this!
                // we could get stuck here, remove if private testing shows this wedging the box
                //
                //
                //case 11:
                //    HarnessCurrentCacheSize = 1;
                //break;
            }

            HarnessCurrentCacheSize *= PAGE_SIZE;
            XSetFileCacheSize( HarnessCurrentCacheSize );
            HarnessCurrentCacheSize = XGetFileCacheSize();

        } else {

            //
            // just get the current size - don't mess with it outside of stressmode
            //

            HarnessCurrentCacheSize = XGetFileCacheSize();

        }

        //
        // try to force debugger to reconnect
        //

        *KdDebuggerNotPresent = FALSE;

        Sleep( 60 * 1000 );

        GetSystemTime(&xSystemTime);
        DbgPrint( "HARNESS: system running for %lu seconds. Date:%2.2d\\%2.2d\\%2.2d Time:%2.2d:%2.2d:%2.2d : Cache: %d bytes\n",
                  GetTickCount()/1000,
                  xSystemTime.wYear,
                  xSystemTime.wMonth,
                  xSystemTime.wDay,
                  xSystemTime.wHour,
                  xSystemTime.wMinute,
                  xSystemTime.wSecond,
                  HarnessCurrentCacheSize);
    }
}

VOID
HarnessCreateBackgroundThreads(
    VOID
    )
/*++

Routine Description:

    This routine create thread to randomly change file cache size. This is
    useful for stress testing

Arguments:

    None

Return Value:

    None

--*/
{
    HANDLE ThreadHandle;

    ThreadHandle = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) \
                                 HarnessHangDetection, NULL, 0,
                                 NULL );

    if ( !ThreadHandle ) {
        KdPrint((
            "HARNESS: unable to create hang detection thread (%s)\n",
            WinErrorSymbolicName(GetLastError())
            ));
        HarnessLog(
            XLL_WARN,
            "Unable to create hang detection thread (%s)",
            WinErrorSymbolicName(GetLastError())
            );
    } else {
        CloseHandle( ThreadHandle );
    }

    ThreadHandle = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) \
                                 HarnessFscSizerThread, NULL, 0,
                                 NULL );

    if ( !ThreadHandle ) {
        KdPrint((
            "HARNESS: unable to create fsc sizer thread (%s)\n",
            WinErrorSymbolicName(GetLastError())
            ));
        HarnessLog(
            XLL_WARN,
            "Unable to create fsc sizer thread (%s)",
            WinErrorSymbolicName(GetLastError())
            );
    } else {
        CloseHandle( ThreadHandle );
    }
}

VOID
HarnessMountAvailableMUs(
    VOID
    ) 
{
    INT Port, Slot;
    OCHAR Drive;
    BOOL bFormatSuccessful = FALSE;
    ANSI_STRING ostMU;
    char pszMU[64];
    ostMU.Length = 0;
    ostMU.MaximumLength = 63;
    ostMU.Buffer = pszMU;

    if ( GetProfileIntA("GeneralSettings", "DontMountMUs", FALSE) ) {
        return;
    }

    Sleep( 2000 );

    for ( Port=0; Port<XGetPortCount(); Port++ ) {

        //
        // if formatting of MUs was needed, do it here before we mount them
        //

        if ( FormatAllMUs ) {
            for ( Slot = 0; Slot < 2; Slot++ ) {
                if ( NT_SUCCESS( MU_CreateDeviceObject( Port, Slot, &ostMU ) ) ) {

                    bFormatSuccessful = XapiFormatFATVolume( &ostMU );
                    MU_CloseDeviceObject( Port, Slot );

                    if ( bFormatSuccessful ) {
                        DbgPrint( "HARNESS: success formatting MU Port:%d Slot:%d\n", Port, Slot );
                    } else {
                        DbgPrint( "HARNESS: failure formatting MU Port:%d Slot:%d\n", Port, Slot );
                    }
                }
            }
        }

        XMountMU( Port, XDEVICE_TOP_SLOT, &Drive );
        XMountMU( Port, XDEVICE_BOTTOM_SLOT, &Drive );
    }
}

NTSTATUS
WINAPI
HarnessEntryPoint(
    VOID
    )
{
    OCHAR drive;
#if DBG
    CHAR SystemVersion[16];
    CHAR XTLVersion[16];
#endif

#if 0
    //
    // Initialize Trap03 stuff (currenty disabled)
    //

    HarnessTrap03TlsIndex = TlsAlloc();
    if ( HarnessTrap03TlsIndex == (DWORD)-1 ) {
        DbgPrint( "HARNESS: couldn't allocate thread local storage\n" );
        return STATUS_NO_MEMORY;
    }

    HarnessInitializeTrap03Handler();
#endif

    PreviousExceptionFilter = SetUnhandledExceptionFilter( HarnessUnhandledExceptionFilter );

    DbgPrint( "\nHARNESS: Start testing\n" );

#if DBG
    XDebugGetSystemVersion( SystemVersion, ARRAY_SIZE(SystemVersion) );
    XDebugGetXTLVersion( XTLVersion, ARRAY_SIZE(XTLVersion) );
    DbgPrint( "HARNESS: system version %s, XTL version %s\n", SystemVersion, XTLVersion );
#endif

    //
    // Load and initialize global settings from testini.ini
    //

    HarnessInitGlobalVars();

    //
    // 1.) we may only want to run with a matched kernel
    // 2.) and with a later box in some circumstances
    //
    
    if ( GetProfileIntA("GeneralSettings", "ForceMatchedKernel", FALSE) == TRUE ) {

        //
        // compare our build number to the kernel build number
        // reboot to default title if no match
        //
        
        if (XboxKrnlVersion->Build != VER_PRODUCTBUILD) {

            //
            // reboot to default title
            //

            XLaunchNewImage(
                NULL,
                NULL);
        }
    }

    if ( GetProfileIntA("GeneralSettings", "ForceNewInitTable", FALSE) == TRUE ) {

        //
        // compare our init table version to the latest one
        // Note: this is ugly, hard coded, and will have to change fro time to time
        // this case should only be hit if someone is running a new build on an old black box
        //
    
    }
    
    if ( GetProfileIntA("GeneralSettings", "DisableAutoOff", TRUE) == TRUE ) {
        XAutoPowerDownSet(FALSE);
    }

    //
    // Initialize core peripheral port support
    //

    if ( GetProfileIntA("GeneralSettings", "DontInitDevices", FALSE) == 0 ) {

        XInitDevices( 0, NULL );

        //
        // Mount all inserted MUs, and format MUs if needed
        //

        HarnessMountAvailableMUs();
    }

    //
    // Display logical drives settings
    //

    for ( drive='A'; drive<='Z'; drive++ ) {
        QueryDriveSymbolicLink( drive );
    }

    //
    // Create thread to randomly change the file cache size
    //

    HarnessCreateBackgroundThreads();

    //
    // Initialize logging engine
    //

    if ( !HarnessInitializeLogging() ) {
        DbgPrint( "HARNESS: Unable to initialize logging engine, all tests blocked\n" );
        return STATUS_NO_LOG_SPACE;
    }

    //
    // Load and run test modules from TESTINI.INI
    //

    HarnessLoadTestModules();

    //
    // Close and clean up logging
    //

    HarnessShutdownLogging();
    DbgPrint( "HARNESS: Finish testing\n" );

    FlushDiskCache();

    DbgPrint(
        "HARNESS: sleep indefinitely, you may start file transfer using xbcp.exe\n"
        "  or press Ctrl+C and enter \'ed harness!HarnessQuitFlag 1;g\' to terminate\n"
        );

    while ( HarnessQuitFlag == FALSE ) {
        Sleep( 2000 );
    }

    DbgPrint( "HARNESS: terminating\n" );
    FlushDiskCache();
    SetUnhandledExceptionFilter( PreviousExceptionFilter );

    return STATUS_SUCCESS;
}

#if 0

#define PAGE_START(address) (((ULONG_PTR)address) & ~((ULONG_PTR)PAGE_SIZE - 1))

DWORD HarnessTrap03TlsIndex;


//
// External Trap03 handler in assembly module
//

BOOL
HarnessInitializeTrap03Handler(
    VOID
    );


VOID
GetStackLimits(
   OUT PULONG LowLimit,
   OUT PULONG HighLimit
   );


PVOID
GetRegistrationHead(
    VOID
    );


EXCEPTION_DISPOSITION
RtlpExecuteHandlerForException (
   IN PEXCEPTION_RECORD ExceptionRecord,
   IN PVOID EstablisherFrame,
   IN OUT PCONTEXT ContextRecord,
   IN OUT PVOID DispatcherContext,
   IN PEXCEPTION_ROUTINE ExceptionRoutine
   );


LPVOID
WINAPI
SafeTlsGetValue(
    DWORD Index
    )

/*++

Routine Description:

    This function is used to retrive the value in the TLS storage
    associated with the specified index. This function will be called
    inside trap handler so checking for valid XapiThread pointer is
    necessary as system threads don't have XapiThread pointer

Arguments:

    dwTlsIndex - Supplies a TLS index allocated using TlsAlloc.

Return Value:

    NON-NULL - The function was successful. The value is the data stored
        in the TLS slot associated with the specified index.

    NULL - The operation failed, or the value associated with the
        specified index was NULL.

--*/

{
    if ( PsGetCurrentThread()->XapiThread ) {
        return TlsGetValue( Index );
    }

    return NULL;
}


ULONG
EspFromTrapFrame(
    IN PKTRAP_FRAME TrapFrame
    )

/*++

Routine Description:

    This routine fetches the correct esp from a trapframe, accounting
    for whether the frame is a user or kernel mode frame, and whether
    it has been edited.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame from which volatile context
        should be copied into the context record.

Return Value:

    Value of Esp.

--*/

{
    if ((TrapFrame->SegCs & FRAME_EDITED) == 0) {

        //  Kernel mode frame which has had esp edited,
        //  value of Esp is in TempEsp.

        return TrapFrame->TempEsp;

    } else {

        //  Kernel mode frame has has not had esp edited, compute esp.

        return (ULONG)&TrapFrame->HardwareEsp;
    }
}


VOID
EspToTrapFrame(
    IN PKTRAP_FRAME TrapFrame,
    IN ULONG Esp
    )

/*++

Routine Description:

    This routine sets the specified value Esp into the trap frame,
    accounting for whether the frame is a user or kernel mode frame,
    and whether it has been edited before.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame from which volatile context
        should be copied into the context record.

    Esp - New value for Esp.

Return Value:

    None.

--*/

{
    ULONG   OldEsp;

    OldEsp = EspFromTrapFrame(TrapFrame);

    //
    //  Kernel mode esp can't be lowered or iret emulation will fail
    //

    if (Esp < OldEsp)
        KeBugCheck(SET_OF_INVALID_CONTEXT);

    //
    //  Edit frame, setting edit marker as needed.
    //

    if ((TrapFrame->SegCs & FRAME_EDITED) == 0) {

        //  Kernel frame that has already been edited,
        //  store value in TempEsp.

        TrapFrame->TempEsp = Esp;

    } else {

        //  Kernel frame for which Esp is being edited first time.
        //  Save real SegCs, set marked in SegCs, save Esp value.

        if (OldEsp != Esp) {
            TrapFrame->TempSegCs = TrapFrame->SegCs;
            TrapFrame->SegCs = TrapFrame->SegCs & ~FRAME_EDITED;
            TrapFrame->TempEsp = Esp;
        }
    }
}


VOID
ContextFromTrapFrame(
    IN PKTRAP_FRAME TrapFrame,
    IN OUT PCONTEXT ContextFrame
    )

/*++

Routine Description:

    This routine moves the selected contents of the specified trap and exception frames
    frames into the specified context frame according to the specified context
    flags.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame from which volatile context
        should be copied into the context record.

    ContextFrame - Supplies a pointer to the context frame that receives the
        context copied from the trap and exception frames.

Return Value:

    None.

--*/

{
    //
    // Set registers ebp, eip, cs, eflag, esp and ss.
    //

    ContextFrame->Ebp = TrapFrame->Ebp;
    ContextFrame->Eip = TrapFrame->Eip;

    if (((TrapFrame->SegCs & FRAME_EDITED) == 0) &&
        ((TrapFrame->EFlags & EFLAGS_V86_MASK) == 0)) {
        ContextFrame->SegCs = TrapFrame->TempSegCs & SEGMENT_MASK;
    } else {
        ContextFrame->SegCs = TrapFrame->SegCs & SEGMENT_MASK;
    }
    ContextFrame->EFlags = TrapFrame->EFlags;
    ContextFrame->SegSs = KGDT_R0_DATA;
    ContextFrame->Esp = EspFromTrapFrame(TrapFrame);

    //
    // Set integer registers edi, esi, ebx, edx, ecx, eax
    //

    ContextFrame->Edi = TrapFrame->Edi;
    ContextFrame->Esi = TrapFrame->Esi;
    ContextFrame->Ebx = TrapFrame->Ebx;
    ContextFrame->Ecx = TrapFrame->Ecx;
    ContextFrame->Edx = TrapFrame->Edx;
    ContextFrame->Eax = TrapFrame->Eax;
}


VOID
ContextToTrapFrame(
    IN OUT PKTRAP_FRAME TrapFrame,
    IN PCONTEXT ContextFrame
    )

/*++

Routine Description:

    This routine moves the selected contents of the specified context frame into
    the specified trap and exception frames according to the specified context
    flags.

Arguments:

    TrapFrame - Supplies a pointer to a trap frame that receives the volatile
        context from the context record.

    ContextFrame - Supplies a pointer to a context frame that contains the
        context that is to be copied into the trap and exception frames.

    ContextFlags - Supplies the set of flags that specify which parts of the
        context frame are to be copied into the trap and exception frames.

Return Value:

    None.

--*/

{
    //
    // Set registers eflag, ebp, eip, cs, esp and ss.
    // Eflags is set first, so that the auxilliary routines
    // can check the v86 bit to determine as well as cs, to
    // determine if the frame is kernel or user mode. (v86 mode cs
    // can have any value)
    //

    TrapFrame->EFlags = SANITIZE_FLAGS(ContextFrame->EFlags);
    TrapFrame->Ebp = ContextFrame->Ebp;
    TrapFrame->Eip = ContextFrame->Eip;
    EspToTrapFrame(TrapFrame, ContextFrame->Esp);

    //
    // Set integer registers edi, esi, ebx, edx, ecx, eax.
    //
    //  Can NOT call RtlMoveMemory here because the regs aren't
    //  contiguous in pusha frame, and we don't want to export
    //  bits of junk into context record.
    //

    TrapFrame->Edi = ContextFrame->Edi;
    TrapFrame->Esi = ContextFrame->Esi;
    TrapFrame->Ebx = ContextFrame->Ebx;
    TrapFrame->Ecx = ContextFrame->Ecx;
    TrapFrame->Edx = ContextFrame->Edx;
    TrapFrame->Eax = ContextFrame->Eax;
}


//
// Dispatcher context structure definition.
//

typedef struct _DISPATCHER_CONTEXT {
    PEXCEPTION_REGISTRATION_RECORD RegistrationPointer;
    } DISPATCHER_CONTEXT;


BOOLEAN
DispatchException(
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PCONTEXT ContextRecord
    )

/*++

Routine Description:

    This function attempts to dispatch an exception to a call frame based
    handler by searching backwards through the stack based call frames. The
    search begins with the frame specified in the context record and continues
    backward until either a handler is found that handles the exception, the
    stack is found to be invalid (i.e., out of limits or unaligned), or the end
    of the call hierarchy is reached.

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ContextRecord - Supplies a pointer to a context record.

Return Value:

    If the exception is handled by one of the frame based handlers, then
    a value of TRUE is returned. Otherwise a value of FALSE is returned.

--*/

{

    DISPATCHER_CONTEXT DispatcherContext;
    EXCEPTION_DISPOSITION Disposition;
    PEXCEPTION_REGISTRATION_RECORD RegistrationPointer;
    PEXCEPTION_REGISTRATION_RECORD NestedRegistration;
    ULONG HighAddress;
    ULONG HighLimit;
    ULONG LowLimit;
    EXCEPTION_RECORD ExceptionRecord1;

    //
    // Get current stack limits.
    //

    GetStackLimits(&LowLimit, &HighLimit);

    //
    // Start with the frame specified by the context record and search
    // backwards through the call frame hierarchy attempting to find an
    // exception handler that will handler the exception.
    //

    RegistrationPointer = GetRegistrationHead();
    NestedRegistration = 0;

    while (RegistrationPointer != EXCEPTION_CHAIN_END) {

        //
        // If the call frame is not within the specified stack limits or the
        // call frame is unaligned, then set the stack invalid flag in the
        // exception record and return FALSE. Else check to determine if the
        // frame has an exception handler.
        //

        HighAddress = (ULONG)RegistrationPointer +
            sizeof(EXCEPTION_REGISTRATION_RECORD);

        if ( ((ULONG)RegistrationPointer < LowLimit) ||
             (HighAddress > HighLimit) ||
             (((ULONG)RegistrationPointer & 0x3) != 0) ) {

#if defined(NTOS_KERNEL_RUNTIME)

            //
            // Allow for the possibility that the problem occured on the
            // DPC stack.
            //

            ULONG TestAddress = (ULONG)RegistrationPointer;

            if (((TestAddress & 0x3) == 0) &&
                KeGetCurrentIrql() >= DISPATCH_LEVEL) {

                PKPRCB Prcb = KeGetCurrentPrcb();
                ULONG DpcStack = (ULONG)Prcb->DpcStack;

                if ((Prcb->DpcRoutineActive) &&
                    (HighAddress <= DpcStack) &&
                    (TestAddress >= DpcStack - KERNEL_STACK_SIZE)) {

                    //
                    // This error occured on the DPC stack, switch
                    // stack limits to the DPC stack and restart
                    // the loop.
                    //

                    HighLimit = DpcStack;
                    LowLimit = DpcStack - KERNEL_STACK_SIZE;
                    continue;
                }
            }

#endif

            ExceptionRecord->ExceptionFlags |= EXCEPTION_STACK_INVALID;
            return FALSE;
        }

        Disposition = RtlpExecuteHandlerForException(
            ExceptionRecord,
            (PVOID)RegistrationPointer,
            ContextRecord,
            (PVOID)&DispatcherContext,
            (PEXCEPTION_ROUTINE)RegistrationPointer->Handler);

        //
        // If the current scan is within a nested context and the frame
        // just examined is the end of the context region, then clear
        // the nested context frame and the nested exception in the
        // exception flags.
        //

        if (NestedRegistration == RegistrationPointer) {
            ExceptionRecord->ExceptionFlags &= (~EXCEPTION_NESTED_CALL);
            NestedRegistration = 0;
        }

        //
        // Case on the handler disposition.
        //

        switch (Disposition) {

            //
            // The disposition is to continue execution. If the
            // exception is not continuable, then raise the exception
            // STATUS_NONCONTINUABLE_EXCEPTION. Otherwise return
            // TRUE.
            //

        case ExceptionContinueExecution :
            if ((ExceptionRecord->ExceptionFlags &
               EXCEPTION_NONCONTINUABLE) != 0) {
                ExceptionRecord1.ExceptionCode =
                                        STATUS_NONCONTINUABLE_EXCEPTION;
                ExceptionRecord1.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
                ExceptionRecord1.ExceptionRecord = ExceptionRecord;
                ExceptionRecord1.NumberParameters = 0;
                RtlRaiseException(&ExceptionRecord1);
            } else {
                return TRUE;
            }

            //
            // The disposition is to continue the search. Get next
            // frame address and continue the search.
            //

        case ExceptionContinueSearch :
            break;

            //
            // The disposition is nested exception. Set the nested
            // context frame to the establisher frame address and set
            // nested exception in the exception flags.
            //

        case ExceptionNestedException :
            ExceptionRecord->ExceptionFlags |= EXCEPTION_NESTED_CALL;
            if (DispatcherContext.RegistrationPointer > NestedRegistration) {
                NestedRegistration = DispatcherContext.RegistrationPointer;
            }
            break;

            //
            // All other disposition values are invalid. Raise
            // invalid disposition exception.
            //

        default :
            ExceptionRecord1.ExceptionCode = STATUS_INVALID_DISPOSITION;
            ExceptionRecord1.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
            ExceptionRecord1.ExceptionRecord = ExceptionRecord;
            ExceptionRecord1.NumberParameters = 0;
            RtlRaiseException(&ExceptionRecord1);
            break;
        }

        //
        // If chain goes in wrong direction or loops, report an
        // invalid exception stack, otherwise go on to the next one.
        //

        RegistrationPointer = RegistrationPointer->Next;
    }
    return FALSE;
}


VOID
WINAPI
HarnessDispatchBreakpointException(
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PKEXCEPTION_FRAME ExceptionFrame,
    IN PKTRAP_FRAME TrapFrame,
    IN BOOLEAN FirstChance
    )

/*++

Routine Description:

    This function is called to dispatch an int 3 exception to the proper mode
    and to cause the exception dispatcher to be called.

Arguments:

    ExceptionRecord - Supplies a pointer to an exception record.

    ExceptionFrame - Supplies a pointer to an exception frame. For NT386,
        this should be NULL.

    TrapFrame - Supplies a pointer to a trap frame.

    FirstChance - Supplies a boolean value that specifies whether this is
        the first (TRUE) or second (FALSE) chance for the exception.

Return Value:

    None.

--*/

{
    CONTEXT ContextFrame;
    ASSERT( ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT );
    ContextFromTrapFrame( TrapFrame, &ContextFrame );
    DispatchException(ExceptionRecord, &ContextFrame);
    ContextToTrapFrame( TrapFrame, &ContextFrame );
}

#endif
