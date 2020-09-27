
//
//  coversys.c
//
//  tommcg, 3/1/95
//

#include <ntddk.h>
#include <ntimage.h>
#include <stdarg.h>
#include "coversys.h"

//
//  I can't figure out how to include both ntddk.h and ntrtl.h and ntexapi.h,
//  so I putting private typedefs and prototypes here.  These interfaces should
//  probably be public to DDK developers anyway.  Shoot me.
//

typedef struct _SYSTEM_BASIC_INFORMATION {
    ULONG Reserved;
    ULONG TimerResolution;
    ULONG PageSize;
    ULONG NumberOfPhysicalPages;
    ULONG LowestPhysicalPageNumber;
    ULONG HighestPhysicalPageNumber;
    ULONG AllocationGranularity;
    ULONG MinimumUserModeAddress;
    ULONG MaximumUserModeAddress;
    KAFFINITY ActiveProcessorsAffinityMask;
    CCHAR NumberOfProcessors;
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
    HANDLE Section;                 // Not filled in
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    UCHAR  FullPathName[ 256 ];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES {
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[ 1 ];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
    SystemPathInformation,
    SystemProcessInformation,
    SystemCallCountInformation,
    SystemDeviceInformation,
    SystemProcessorPerformanceInformation,
    SystemFlagsInformation,
    SystemCallTimeInformation,
    SystemModuleInformation
    } SYSTEM_INFORMATION_CLASS;

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation (
    IN  SYSTEM_INFORMATION_CLASS SystemInformationClass,
    OUT PVOID  SystemInformation,
    IN  ULONG  SystemInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

int __cdecl _vsnprintf( char *, size_t, const char *, va_list );

#ifndef STDCALL
#define STDCALL __stdcall
#endif

#ifndef INLINE
#define INLINE __inline
#endif

#define ROUNDUP2( x, n ) ((((ULONG)(x)) + (((ULONG)(n)) - 1 )) & ~(((ULONG)(n)) - 1 ))

//#define DECLSPEC __declspec( dllexport )
#define DECLSPEC

typedef struct _REGISTERED_BINARY REGISTERED_BINARY, *PREGISTERED_BINARY;

struct _REGISTERED_BINARY {
    PREGISTERED_BINARY pNext;                   // singly linked list
    PCHAR              BaseOfLoggingRegion;
    ULONG              SizeOfLoggingRegion;
    ULONG              CheckSumOfBinary;
    PDRIVER_OBJECT     DriverObject;
    PDRIVER_UNLOAD     DriverUnload;
    PVOID              CapturedLogging;
    CHAR               BinaryName[ 1 ];         // variable length
    };

NTSTATUS
STDCALL
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

VOID
STDCALL
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
STDCALL
DeviceOpen(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
STDCALL
DeviceClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
STDCALL
DeviceIoctl(
    IN PDEVICE_OBJECT IoctlDeviceObject,
    IN PIRP Irp
    );

VOID
STDCALL
RegisterNoImportCoverageBinaries(
    VOID
    );

VOID
STDCALL
RegisterNoImportCoverageBinary(
    IN PVOID ImageBase
    );

DECLSPEC
VOID
STDCALL
CoverageRegisterBinary(
    IN LPCSTR BinaryName,
    IN ULONG  CheckSumOfBinary,
    IN PCHAR  BaseOfLoggingRegion,
    IN ULONG  SizeOfLoggingRegion
    );

DECLSPEC
VOID
STDCALL
CoverageRegisterUnloadHook(
    IN     LPCSTR         BinaryName,
    IN OUT PDRIVER_OBJECT DriverObject
    );

DECLSPEC
VOID
STDCALL
CoverageUnRegisterBinary(
    IN LPCSTR BinaryName
    );

VOID
STDCALL
CoverageUnloadDriver(
    IN PDRIVER_OBJECT DriverObject
    );

VOID
STDCALL
CoverageUnRegisterBinaryNode(
    IN PREGISTERED_BINARY pNode
    );


NTSTATUS
STDCALL
CoverageEnumerateBinaries(
    IN  ULONG  BufferSize,
    OUT PCHAR  Buffer,
    OUT PULONG ActualSize
    );

NTSTATUS
STDCALL
CoveragePollLogging(
    IN  LPCSTR BinaryName,
    IN  ULONG  BufferSize,
    OUT PVOID  Buffer,
    OUT PULONG ActualSize,
    OUT PULONG CheckSum
    );

NTSTATUS
STDCALL
CoverageFlushLogging(
    IN LPCSTR BinaryName OPTIONAL
    );

NTSTATUS
STDCALL
CoverageStoreLogHandle(
    IN LPCSTR BinaryName,
    IN ULONG  LogHandle
    );

VOID
OrBufferWithBuffer(
    IN OUT PVOID TargetBuffer,
    IN     PVOID SourceBuffer,
    IN     ULONG Size
    );

VOID
MoveLoggingInformationToBuffer(
    IN     ULONG BufferSize,
    IN OUT PVOID ActiveBuffer,
       OUT PVOID ReturnBuffer
    );

VOID
CoverageDbgPrint(
    IN LONG  InfoLevel,
    IN PCHAR Format,
    ...
    );


//
//  Default data segment for uninitialized data is .bss which is then
//  merged into initialized data segment .data.  We want these following
//  uninitialized data objects to be in .bss or .data because it must
//  be non-paged memory.
//

FAST_MUTEX RegistrationMutex;

#pragma data_seg( "PAGEDATA" )  // remainder of data in this driver is pageable

//
//  Internal CoverageDbgPrint is paged code.  This whole driver is essentially
//  paged code.  If we need to call CoverageDbgPrint in the future from
//  non-paged code, we need to move this CoverageDbgLogLevel variable to
//  non-paged data section.
//

enum {
    DBGLOG_NONE    = 0,
    DBGLOG_ERROR   = 1,
    DBGLOG_WARNING = 2,
    DBGLOG_INFO    = 3,
    DBGLOG_DEBUG   = 4
    } CoverageDbgLogLevel;

BOOLEAN DriverInitialized;

PREGISTERED_BINARY RegisteredBinaryList = (PREGISTERED_BINARY)&RegisteredBinaryList;

#pragma  code_seg( "INIT" )      //  DriverEntry is discardable after init
#pragma const_seg( "INITDATA" )  //  for const strings in code
#pragma  data_seg( "INITDATA" )  //  for strings in INIT code


NTSTATUS
STDCALL
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
    {
    RTL_QUERY_REGISTRY_TABLE RegConfigData[ 2 ];
    PDEVICE_OBJECT           DeviceObject = NULL;
    UNICODE_STRING           DeviceName;
    NTSTATUS                 Status;
    ULONG                    DefaultDebugLogLevel;

    RtlZeroMemory( &RegConfigData, sizeof( RegConfigData ));

    CoverageDbgLogLevel  = DBGLOG_INFO;
    DefaultDebugLogLevel = DBGLOG_INFO;

    RegConfigData[ 0 ].Flags         = RTL_QUERY_REGISTRY_DIRECT;
    RegConfigData[ 0 ].Name          = L"DebugLogLevel";
    RegConfigData[ 0 ].EntryContext  = &CoverageDbgLogLevel;
    RegConfigData[ 0 ].DefaultType   = REG_DWORD;
    RegConfigData[ 0 ].DefaultData   = &DefaultDebugLogLevel;
    RegConfigData[ 0 ].DefaultLength = sizeof( ULONG );

    RtlQueryRegistryValues(
        RTL_REGISTRY_ABSOLUTE | RTL_REGISTRY_OPTIONAL,
        RegistryPath->Buffer,
        RegConfigData,
        NULL,
        NULL
        );

    RtlInitUnicodeString( &DeviceName, L"\\DosDevices\\Coverage" );

    Status = IoCreateDevice(
                 DriverObject,
                 0,
                 &DeviceName,
                 FILE_DEVICE_UNKNOWN,
                 0,
                 TRUE,
                 &DeviceObject
                 );

    if ( NT_SUCCESS( Status )) {

        DriverObject->DriverUnload                         = DriverUnload;
        DriverObject->MajorFunction[IRP_MJ_CREATE]         = DeviceOpen;
        DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DeviceClose;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoctl;

        ExInitializeFastMutex( &RegistrationMutex );

        DriverInitialized = TRUE;

        CoverageDbgPrint( DBGLOG_INFO, "COVERAGE: Driver loaded\n" );

        //
        //  Now attempt to register "no import" coverage binaries
        //  that have already been loaded.
        //

        RegisterNoImportCoverageBinaries();
        }
    else {
        CoverageDbgPrint( DBGLOG_ERROR, "COVERAGE: Driver load failed (%X)\n", Status );
        }

    return Status;
    }


VOID
STDCALL
RegisterNoImportCoverageBinaries(
    VOID
    )
    {
    SYSTEM_BASIC_INFORMATION BasicInformation;
    PRTL_PROCESS_MODULES pModuleInformation;
    RTL_PROCESS_MODULES ModuleInformation;
    ULONG AllocationSize;
    NTSTATUS Status;
    ULONG i;

    Status = ZwQuerySystemInformation(
                 SystemBasicInformation,
                 &BasicInformation,
                 sizeof( BasicInformation ),
                 NULL
                 );

    if ( NT_SUCCESS( Status )) {

        ModuleInformation.NumberOfModules = 0;

        ZwQuerySystemInformation(
           SystemModuleInformation,
           &ModuleInformation,
           sizeof( ModuleInformation ),
           NULL
           );

        if ( ModuleInformation.NumberOfModules != 0 ) {

            AllocationSize = sizeof( RTL_PROCESS_MODULES ) +
                             sizeof( RTL_PROCESS_MODULE_INFORMATION ) *
                             ModuleInformation.NumberOfModules;

            pModuleInformation = ExAllocatePoolWithTag(
                                     PagedPool,
                                     AllocationSize,
                                     'RVOC'
                                     );

            if ( pModuleInformation != NULL ) {

                Status = ZwQuerySystemInformation(
                            SystemModuleInformation,
                            pModuleInformation,
                            AllocationSize,
                            NULL
                            );

                if ( NT_SUCCESS( Status )) {

                    for ( i = 0; i < pModuleInformation->NumberOfModules; i++ ) {

                        if ((ULONG)pModuleInformation->Modules[ i ].ImageBase > BasicInformation.MaximumUserModeAddress ) {

#if 0   // too verbose

                            CoverageDbgPrint(
                                 DBGLOG_DEBUG,
                                 "COVERAGE: Inspecting module at %08X (%s)\n",
                                 pModuleInformation->Modules[ i ].ImageBase,
                                 pModuleInformation->Modules[ i ].FullPathName + pModuleInformation->Modules[ i ].OffsetToFileName
                                 );

#endif  // too verbose

                            RegisterNoImportCoverageBinary( pModuleInformation->Modules[ i ].ImageBase );

                            }
                        }
                    }

                else {
                    CoverageDbgPrint( DBGLOG_DEBUG, "COVERAGE: Failed querying system modules info (%X)\n", Status );
                    }

                ExFreePool( pModuleInformation );
                }

            else {
                CoverageDbgPrint( DBGLOG_DEBUG, "COVERAGE: Failed allocating memory for querying system modules info\n" );
                }
            }

        else {
            CoverageDbgPrint( DBGLOG_DEBUG, "COVERAGE: Failed querying system modules info\n" );
            }
        }

    else {
        CoverageDbgPrint( DBGLOG_DEBUG, "COVERAGE: Failed querying system info (%X)\n", Status );
        }
    }


VOID
STDCALL
RegisterNoImportCoverageBinary(
    IN PVOID ImageBase
    )
    {
    PIMAGE_DOS_HEADER     DosHeader;
    PIMAGE_NT_HEADERS     NtHeader;
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONGLONG             SectionName;
    ULONG                 SectionCount;
    PCHAR                 SectionBase;
    ULONG                 OriginalCheckSum;
    LPSTR                 OriginalFileName;
    PCHAR                 BaseOfLoggingRegion;
    ULONG                 SizeOfLoggingRegion;
    ULONG                 i;

    //
    //  We assume that each module's PE header is resident at its mapped
    //  base address.  We walk the PE header and PE section table looking
    //  for ".cover" section.  If we find it, we register the binary.
    //
    //  If we do eventually make this routine callable other than during
    //  DriverEntry, then we'll need to move it to the PAGE section rather
    //  than the INIT section.
    //

    try {

        if ( ! MmIsAddressValid( ImageBase )) {
            leave;
            }

        DosHeader = (PIMAGE_DOS_HEADER) ImageBase;

        if ( DosHeader->e_magic == IMAGE_DOS_SIGNATURE ) {

            NtHeader = (PIMAGE_NT_HEADERS)((PCHAR)ImageBase + DosHeader->e_lfanew );

            if ( ! MmIsAddressValid( NtHeader )) {
                leave;
                }
            }
        else {
            NtHeader = (PIMAGE_NT_HEADERS)( ImageBase );
            }

        if ( NtHeader->Signature != IMAGE_NT_SIGNATURE ) {
            leave;
            }

        SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
        SectionCount  = NtHeader->FileHeader.NumberOfSections;

        if ( ! MmIsAddressValid( &SectionHeader[ SectionCount ] )) {
            leave;
            }

        for ( i = 0; i < SectionCount; i++ ) {

            //
            //  To make section name comparison fast, read it into an 8-byte
            //  DWORDLONG, do a sneaky lowercase by ORing all the characters
            //  with 0x20, which also has the effect of "normalizing" any
            //  zero byte characters to spaces.  Then we can just do a simple
            //  DWORDLONG comparison with the ".cover  " equivalent value.
            //

            SectionName  = *(UNALIGNED ULONGLONG*)( &SectionHeader[ i ].Name );
            SectionName |= 0x2020202020202020;

#if 0   // too verbose

            CoverageDbgPrint(
                 DBGLOG_DEBUG,
                 "COVERAGE: Inspecting section \"%c%c%c%c%c%c%c%c\" at %08X\n",
                 (UCHAR)( SectionName >>  0 ),
                 (UCHAR)( SectionName >>  8 ),
                 (UCHAR)( SectionName >> 16 ),
                 (UCHAR)( SectionName >> 24 ),
                 (UCHAR)( SectionName >> 32 ),
                 (UCHAR)( SectionName >> 40 ),
                 (UCHAR)( SectionName >> 48 ),
                 (UCHAR)( SectionName >> 56 ),
                 (PCHAR)ImageBase + SectionHeader[ i ].VirtualAddress
                 );

#endif  // too verbose

            if ( SectionName == 0x20207265766F632E ) {      // ".cover  "

                //
                //  Data stored in coverage section looks like this:
                //
                //      Offset 0: DWORD  SizeOfLoggingRegion
                //      Offset 4: DWORD  CheckSumOfOriginalBinary
                //      Offset 8: CHAR[] FileNameOfOriginalBinary (null terminated string)
                //      Offset X: CHAR[] LoggingBuffer (size is SizeOfLoggingRegion)
                //
                //      X is computed as ROUNDUP2(( strlen( name ) + 1 + 8 ), 32 ) to
                //      place the LoggingBuffer on a 32-byte boundary.
                //

                SectionBase = (PVOID)((PCHAR)ImageBase + SectionHeader[ i ].VirtualAddress );

                CoverageDbgPrint( DBGLOG_DEBUG, "COVERAGE: Found \".cover\" section in image %08X at %08X\n", ImageBase, SectionBase );

                if ( MmIsAddressValid( SectionBase )) {

                    SizeOfLoggingRegion = *(PULONG)( SectionBase + 0 );
                    OriginalCheckSum    = *(PULONG)( SectionBase + 4 );
                    OriginalFileName    = SectionBase + 8;
                    BaseOfLoggingRegion = SectionBase + ROUNDUP2(( strlen( OriginalFileName ) + 1 + 8 ), 32 );

                    CoverageRegisterBinary(
                        OriginalFileName,
                        OriginalCheckSum,
                        BaseOfLoggingRegion,
                        SizeOfLoggingRegion
                        );
                    }

                break;
                }
            }
        }

    except( EXCEPTION_EXECUTE_HANDLER ) {
        }
    }


#pragma  code_seg( "PAGE" )      // remainder of code in this driver is pageable
#pragma const_seg( "PAGEDATA" )  // for const strings in code
#pragma  data_seg( "PAGEDATA" )  // remainder of data in this driver is pageable

VOID
CoverageDbgPrint(
    IN LONG  InfoLevel,
    IN PCHAR Format,
    ...
    )
    {
    CHAR Buffer[ 512 ];
    va_list ArgList;

    va_start( ArgList, Format );

    if ( InfoLevel <= CoverageDbgLogLevel ) {

        _vsnprintf( Buffer, sizeof( Buffer ), Format, ArgList );
        Buffer[ sizeof( Buffer ) - 2 ] = '\n';
        Buffer[ sizeof( Buffer ) - 1 ] = 0;
        DbgPrint( Buffer );
        }

    va_end( ArgList );
    }


//
//  Note that following DeviceOpen and DeviceClose implementations are the
//  exact same code.  The linker is kind enough to "fold" them into a single
//  piece of code (identical comdat elimination), so no need to obfuscate the
//  source code here by creating a single function named DeviceOpenClose that
//  does both duties.
//

NTSTATUS
STDCALL
DeviceOpen(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
    {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return STATUS_SUCCESS;
    }


NTSTATUS
STDCALL
DeviceClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
    {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    return STATUS_SUCCESS;
    }


NTSTATUS
STDCALL
DeviceIoctl(
    IN PDEVICE_OBJECT IoctlDeviceObject,
    IN PIRP Irp
    )
    {
    PIO_STACK_LOCATION IrpStack      = IoGetCurrentIrpStackLocation( Irp );
    PVOID              OutBuffer     = Irp->UserBuffer;
    ULONG              OutBufferSize = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    ULONG              InBufferSize  = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
    PVOID              InBuffer      = IrpStack->Parameters.DeviceIoControl.Type3InputBuffer;
    ULONG              ActualSize    = 0;
    NTSTATUS           Status;

    switch ( IrpStack->Parameters.DeviceIoControl.IoControlCode ) {

        case IOCTL_COVERAGE_ENUM_BINARIES:

            Status = CoverageEnumerateBinaries(
                         OutBufferSize,
                         OutBuffer,
                         &ActualSize
                         );
            break;

        case IOCTL_COVERAGE_POLL_LOGGING:

            Status = CoveragePollLogging(
                         InBuffer,              /* pszBinaryName      */
                         OutBufferSize,
                         OutBuffer,
                         &ActualSize,
                         (PULONG)InBufferSize   /* return checksum */
                         );
            break;

        case IOCTL_COVERAGE_FLUSH_LOGGING:

            Status = CoverageFlushLogging(
                         InBuffer                /* pszBinaryName */
                         );
            break;

        default:

            Status = STATUS_INVALID_PARAMETER;

        }

    Irp->IoStatus.Status      = Status;
    Irp->IoStatus.Information = ActualSize;

    IoCompleteRequest( Irp, IO_NO_INCREMENT );

    return Status;
    }


//
//  Following support functions are inlined, so do not actually exist
//  as functions in the binary.
//

VOID
INLINE
AcquireRegistrationMutex(
    VOID
    )
    {
    ExAcquireFastMutex( &RegistrationMutex );
    }


VOID
INLINE
AcquireRegistrationMutexIfInitialized(
    VOID
    )
    {
    if ( DriverInitialized ) ExAcquireFastMutex( &RegistrationMutex );
    }


VOID
INLINE
ReleaseRegistrationMutex(
    VOID
    )
    {
    ExReleaseFastMutex( &RegistrationMutex );
    }


VOID
INLINE
ReleaseRegistrationMutexIfInitialized(
    VOID
    )
    {
    if ( DriverInitialized ) ExReleaseFastMutex( &RegistrationMutex );
    }


PREGISTERED_BINARY
INLINE
AllocateRegisteredBinaryNode(
    IN ULONG BinaryNameLength
    )
    {
    return ExAllocatePoolWithTag(
               PagedPool,
               sizeof( REGISTERED_BINARY ) + BinaryNameLength,
               'RVOC'
               );
    }


PREGISTERED_BINARY
INLINE
AllocateRegisteredBinaryNodeWithName(
    IN LPCSTR BinaryName
    )
    {
    ULONG NameLength = strlen( BinaryName );
    PREGISTERED_BINARY pNode = AllocateRegisteredBinaryNode( NameLength );

    if ( pNode != NULL ) {
        RtlZeroMemory( pNode, sizeof( *pNode ) - sizeof( pNode->BinaryName ));
        RtlCopyMemory( pNode->BinaryName, BinaryName, NameLength + 1 );
        }

    return pNode;
    }


VOID
INLINE
AddRegisteredBinaryNodeToList(
    IN PREGISTERED_BINARY pPrev,
    IN PREGISTERED_BINARY pNode
    )
    {
    pNode->pNext = pPrev->pNext;
    pPrev->pNext = pNode;
    }


//
//  Following functions are exported for coverage-instrumented binaries to
//  register at load and deregister at unload with the coverage.sys driver.
//  Note that these functions are in paged memory, so should never instrument
//  a driver with /Entry or /EntryExports if the instrumented entry points
//  get called at DPC level or higher where touching non-paged code or data
//  is not allowed.
//


DECLSPEC
VOID
STDCALL
CoverageRegisterBinary(
    IN LPCSTR BinaryName,               // must be lowercase or case consistent
    IN ULONG  CheckSumOfBinary,         // checksum of original binary
    IN PCHAR  BaseOfLoggingRegion,      // instrumented logging region base
    IN ULONG  SizeOfLoggingRegion       // instrumented logging region size
    )
    {
    PCHAR EndOfLoggingRegion;
    PREGISTERED_BINARY pNode;
    PREGISTERED_BINARY pPrev;
    BOOLEAN Found = FALSE;

    //
    //  This is an exported entry point, and it's possible that this will
    //  get called before our DriverEntry gets a chance to initialize (when
    //  a binary that imports this binary gets loaded first).  If this
    //  happens, it is a bad thing because we can never get a DriverObject
    //  created for this driver, so we can never communicate with user mode.
    //  Detect if this is the case and display a warning.
    //

    if ( ! DriverInitialized ) {
        CoverageDbgPrint(
            DBGLOG_WARNING,
            "COVERAGE: %s loaded before coverage.sys initialized, kernel-mode\n"
            "    logging data may be lost (inaccessible).\n",
            BinaryName
            );
        }

    AcquireRegistrationMutexIfInitialized();

    EndOfLoggingRegion = BaseOfLoggingRegion + SizeOfLoggingRegion - 1;

    //
    //  Walk list of registered binaries looking for name or logging region
    //

    pPrev = (PREGISTERED_BINARY)&RegisteredBinaryList;
    pNode = RegisteredBinaryList;

    while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

        //
        //  Test if names match.  Names are always lowercase from BBCOVER.
        //

        if ( strcmp( pNode->BinaryName, BinaryName ) == 0 ) {

            //
            //  Reloading binary with same name.  If same checksum, just
            //  update new logging buffer location and continue.
            //

            if (( pNode->CheckSumOfBinary    != CheckSumOfBinary    ) ||
                ( pNode->SizeOfLoggingRegion != SizeOfLoggingRegion )) {

                //
                //  Reloading different version of binary with same name.
                //  Must invalidate any existing data for previous binary.
                //

                if ( pNode->CapturedLogging ) {

                    CoverageDbgPrint(
                        DBGLOG_ERROR,
                        "COVERAGE: %s logging data lost when new version of driver\n"
                        "    was loaded because data was not saved after previous version\n"
                        "    of driver was unloaded.\n",
                        BinaryName
                        );

                    ExFreePool( pNode->CapturedLogging );
                    pNode->CapturedLogging  = NULL;
                    pNode->CheckSumOfBinary = CheckSumOfBinary;

                    }
                }

            CoverageDbgPrint(
                DBGLOG_INFO,
                "COVERAGE: %s registered (%x - %x) (reloaded)\n",
                BinaryName,
                BaseOfLoggingRegion,
                EndOfLoggingRegion
                );

            pNode->BaseOfLoggingRegion = BaseOfLoggingRegion;
            pNode->SizeOfLoggingRegion = SizeOfLoggingRegion;

            Found = TRUE;
            break;
            }

        else {

            //
            //  Not the same binary name, sanity test logging regions.
            //

            if (( pNode->BaseOfLoggingRegion != NULL ) &&
                (( pNode->BaseOfLoggingRegion < EndOfLoggingRegion  ) ^
                 (( pNode->BaseOfLoggingRegion + pNode->SizeOfLoggingRegion - 1 ) < BaseOfLoggingRegion ))) {

                //
                //  Some other binary already logging in requested range.
                //  This should never happen since a driver that can unload
                //  must have gone through our unload hook which would have
                //  zeroed the registered logging region.  We might hit this
                //  if we're not instrumenting to hook unload routines.
                //

                CoverageDbgPrint(
                    DBGLOG_WARNING,
                    "COVERAGE: %s logging region (%x - %x) overlaps region\n"
                    "    previously occupied by %s (%s - %s).  Must assume\n"
                    "    %s has been unloaded and will now be deregistered.\n",
                    BinaryName,
                    BaseOfLoggingRegion,
                    EndOfLoggingRegion,
                    pNode->BinaryName,
                    pNode->BaseOfLoggingRegion,
                    pNode->BaseOfLoggingRegion + pNode->SizeOfLoggingRegion - 1,
                    pNode->BinaryName
                    );

                pNode->BaseOfLoggingRegion = NULL;

                }
            }

        pPrev = pNode;
        pNode = pNode->pNext;

        }

    if ( ! Found ) {

        //
        //  New binary, add to list.
        //

        pNode = AllocateRegisteredBinaryNodeWithName( BinaryName );

        if ( pNode ) {

            pNode->BaseOfLoggingRegion = BaseOfLoggingRegion;
            pNode->SizeOfLoggingRegion = SizeOfLoggingRegion;
            pNode->CheckSumOfBinary    = CheckSumOfBinary;

            AddRegisteredBinaryNodeToList( pPrev, pNode );

            CoverageDbgPrint(
                DBGLOG_INFO,
                "COVERAGE: %s registered (%x - %x)\n",
                BinaryName,
                BaseOfLoggingRegion,
                EndOfLoggingRegion
                );

            }

        else {

            CoverageDbgPrint(
                DBGLOG_ERROR,
                "COVERAGE: %s could not be registered (failed to allocate memory).\n",
                BinaryName
                );
            }
        }

    ReleaseRegistrationMutexIfInitialized();
    }


DECLSPEC
VOID
STDCALL
CoverageRegisterUnloadHook(
    IN     LPCSTR         BinaryName,
    IN OUT PDRIVER_OBJECT DriverObject
    )
    {
    PREGISTERED_BINARY pNode;
    PDRIVER_UNLOAD TempDriverUnload;

    AcquireRegistrationMutexIfInitialized();

    pNode = RegisteredBinaryList;

    while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

        if ( strcmp( pNode->BinaryName, BinaryName ) == 0 ) {

            try {

                //
                //  First try to dereference DriverObject and verify it
                //  really is a pointer to a DriverObject before storing
                //  any values in the DriverObject or in pNode.
                //

                if (( DriverObject != NULL ) &&
                    ( DriverObject->Type == IO_TYPE_DRIVER )) {

                    TempDriverUnload = DriverObject->DriverUnload;

                    if ( TempDriverUnload != NULL ) {

                        //
                        //  Only hook drivers that have an Unload routine
                        //  because drivers without Unload routines are not
                        //  unloadable.
                        //

                        DriverObject->DriverUnload = CoverageUnloadDriver;

                        //
                        //  If successfully read and updated DriverObject's
                        //  DriverUnload pointer, then update pNode to reflect
                        //  that we've hooked this driver's unload routine.
                        //

                        pNode->DriverUnload = TempDriverUnload;
                        pNode->DriverObject = DriverObject;

                        }

                    leave;
                    }

                CoverageDbgPrint(
                    DBGLOG_WARNING,
                    "COVERAGE: %s could not hook unload -- must save coverage\n"
                    "    logging data before unloading %s or coverage logging\n"
                    "    data will be lost.\n",
                    BinaryName,
                    BinaryName
                    );
                }

            except ( EXCEPTION_EXECUTE_HANDLER ) {

                CoverageDbgPrint(
                    DBGLOG_ERROR,
                    "COVERAGE: %s exception %x hooking unload\n",
                    BinaryName,
                    GetExceptionCode()
                    );
                }

            break;

            }

        pNode = pNode->pNext;

        }

    ReleaseRegistrationMutexIfInitialized();

    }


VOID
STDCALL
CoverageUnloadDriver(
    IN PDRIVER_OBJECT DriverObject
    )
    {
    PREGISTERED_BINARY pNode;
    PCHAR BaseOfLogging;
    ULONG SizeOfLogging;
    BOOLEAN Found = FALSE;

    AcquireRegistrationMutexIfInitialized();

    pNode = RegisteredBinaryList;

    while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

        //
        //  pNode->DriverObject is non-NULL only for binaries that successfully
        //  hooked the DriverUnload routine.
        //

        if ( pNode->DriverObject == DriverObject ) {

            //
            //  First call driver's original unload routine to let it clean
            //  up and log its cleanup activity in its logging buffer.
            //  Implementation note: if driver's unload routine causes
            //  another coverage-instrumented driver to unload or load,
            //  we'll deadlock on the mutex if this driver doesn't return
            //  from its unload first.
            //

            ( pNode->DriverUnload )( DriverObject );

            //
            //  Now unregister the binary while the driver is still mapped.
            //  This will capture the logging buffer for later polling.
            //

            CoverageUnRegisterBinaryNode( pNode );

            Found = TRUE;
            break;
            }

        pNode = pNode->pNext;
        }

    if ( ! Found ) {

        CoverageDbgPrint(
            DBGLOG_ERROR,
            "COVERAGE: CoverageUnloadDriver called for unregistered binary.\n"
            "    Don't know original UnloadDriver routine to call.\n"
            );

        DbgBreakPoint();
        }

    ReleaseRegistrationMutexIfInitialized();

    }


VOID
STDCALL
CoverageUnRegisterBinaryNode(
    IN PREGISTERED_BINARY pNode
    )
    {
    PCHAR CaptureBuffer;
    PCHAR BaseOfLogging;
    ULONG SizeOfLogging;

    //
    //  Allocate a paged-pool buffer to store logging buffer
    //  and save the binary's logging buffer contents into it.
    //

    BaseOfLogging = pNode->BaseOfLoggingRegion;
    SizeOfLogging = pNode->SizeOfLoggingRegion;

    if ( BaseOfLogging != NULL ) {

        //
        //  Allocate capture buffer and copy logging information
        //  into it.
        //

        CaptureBuffer = ExAllocatePoolWithTag(
                            PagedPool,
                            SizeOfLogging,
                            'RVOC'
                            );

        if ( CaptureBuffer != NULL ) {

            MoveLoggingInformationToBuffer(
                SizeOfLogging,
                BaseOfLogging,
                CaptureBuffer
                );

            //
            //  Now, if an existing capture buffer already exists for
            //  this binary, OR the newly captured contents into the
            //  existing capture buffer and free the newly allocated
            //  capture buffer.
            //
            //  Buf if an existing capture buffer does not exist, use
            //  the newly allocated capture buffer as the capture
            //  buffer for this node (will be freed when polled).
            //

            if ( pNode->CapturedLogging == NULL ) {
                 pNode->CapturedLogging = CaptureBuffer;
                 }

            else {

                OrBufferWithBuffer(
                    pNode->CapturedLogging,
                    CaptureBuffer,
                    SizeOfLogging
                    );

                ExFreePool( CaptureBuffer );
                }

            CoverageDbgPrint(
                DBGLOG_WARNING,
                "COVERAGE: %s unloaded, captured logging needs to be saved\n",
                pNode->BinaryName
                );
            }

        else {

            CoverageDbgPrint(
                DBGLOG_ERROR,
                "COVERAGE: %s logging data lost when driver unloaded because\n"
                "    failed to allocate memory for logging buffer.\n",
                pNode->BinaryName
                );
            }

        //
        //  Once unregistered, set these to NULL so we know we've
        //  unregistered when try to re-register.
        //

        pNode->BaseOfLoggingRegion = NULL;
        pNode->DriverObject        = NULL;
        pNode->DriverUnload        = NULL;
        }

    else {

        //
        //  BaseOfLogging is NULL.  This can only happen if reloaded
        //  but we didn't catch the first unload.  This is just a
        //  sanity test to keep us from blowing up.
        //

        CoverageDbgPrint(
            DBGLOG_ERROR,
            "COVERAGE: %s logging data lost when driver unloaded because\n"
            "    driver already marked as unloaded.\n",
            pNode->BinaryName
            );
        }
    }


DECLSPEC
VOID
STDCALL
CoverageUnRegisterBinary(
    IN LPCSTR BinaryName                // must be lowercase or case consistent
    )
    {
    PREGISTERED_BINARY pNode;
    BOOLEAN Found = FALSE;

    AcquireRegistrationMutexIfInitialized();

    pNode = RegisteredBinaryList;

    while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

        if ( strcmp( pNode->BinaryName, BinaryName ) == 0 ) {

            CoverageUnRegisterBinary( pNode->BinaryName );

            Found = TRUE;
            break;
            }

        pNode = pNode->pNext;
        }

    if ( ! Found ) {

        CoverageDbgPrint(
            DBGLOG_ERROR,
            "COVERAGE: CoverageUnRegisterBinary called for unregistered binary %s.\n",
            BinaryName
            );
        }

    ReleaseRegistrationMutexIfInitialized();
    }


VOID
OrBufferWithBuffer(
    IN PVOID TargetBuffer,
    IN PVOID SourceBuffer,
    IN ULONG Size
    )
    {
    ULONG CountLongs = Size / 4;
    ULONG CountBytes = Size & 3;

    while ( CountLongs-- ) {
        *((UNALIGNED ULONG*) TargetBuffer )++ |= *((UNALIGNED ULONG*) SourceBuffer )++;
        }

    while ( CountBytes-- ) {
        *((UNALIGNED UCHAR*) TargetBuffer )++ |= *((UNALIGNED UCHAR*) SourceBuffer )++;
        }
    }


//
//  Following functions are exported via corresponding IOCTLs to allow
//  user-mode coverage service to poll and flush the kernel-mode logging
//  information.  Note that probe-and-capture of caller parameters is not
//  bulletproof here (caller could supply kernel-mode addresses for arguments),
//  but this driver is not intended as a released product.  Basic verification
//  is performed to prevent "accidental" mistakes by the caller.
//


NTSTATUS
STDCALL
CoveragePollLogging(
    IN  LPCSTR BinaryName,
    IN  ULONG  BufferSize,
    OUT PVOID  Buffer,
    OUT PULONG ActualSize,
    OUT PULONG CheckSum
    )
    {
    PREGISTERED_BINARY pNode;
    PCHAR BaseOfLogging;
    ULONG SizeOfLogging;
    NTSTATUS Status;

    if ( BinaryName == NULL ) {
        return STATUS_INVALID_PARAMETER;
        }

    Status = STATUS_SUCCESS;

    AcquireRegistrationMutex();

    try {

        if ( ActualSize ) {
            *ActualSize = 0;
            }

        if ( CheckSum ) {
            *CheckSum = 0;
            }

        pNode = RegisteredBinaryList;

        while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

            if ( strcmp( pNode->BinaryName, BinaryName ) == 0 ) {

                if ( CheckSum ) {
                    *CheckSum = pNode->CheckSumOfBinary;
                    }

                SizeOfLogging = pNode->SizeOfLoggingRegion;

                if ( SizeOfLogging != 0 ) {

                    if ( ActualSize ) {
                        *ActualSize = SizeOfLogging;
                        }

                    if (( Buffer == NULL ) || ( BufferSize < SizeOfLogging )) {

                        Status = STATUS_BUFFER_TOO_SMALL;
                        leave;
                        }

                    BaseOfLogging = pNode->BaseOfLoggingRegion;

                    if ( BaseOfLogging != NULL ) {

                        MoveLoggingInformationToBuffer(
                            SizeOfLogging,
                            BaseOfLogging,
                            Buffer
                            );

                        if ( pNode->CapturedLogging != NULL ) {

                            OrBufferWithBuffer(
                                Buffer,
                                pNode->CapturedLogging,
                                SizeOfLogging
                                );

                            ExFreePool( pNode->CapturedLogging );
                            pNode->CapturedLogging = NULL;
                            }
                        }

                    else if ( pNode->CapturedLogging != NULL ) {

                        RtlCopyMemory(
                            Buffer,
                            pNode->CapturedLogging,
                            SizeOfLogging
                            );

                        ExFreePool( pNode->CapturedLogging );
                        pNode->CapturedLogging = NULL;
                        }

                    else {

                        RtlZeroMemory(
                            Buffer,
                            SizeOfLogging
                            );
                        }
                    }

                leave;
                }

            pNode = pNode->pNext;
            }

        Status = STATUS_NOT_FOUND;
        }

    except ( EXCEPTION_EXECUTE_HANDLER ) {
        Status = GetExceptionCode();
        }

    ReleaseRegistrationMutex();

    return Status;
    }


VOID
MoveLoggingInformationToBuffer(
    IN     ULONG BufferSize,
    IN OUT PVOID ActiveBuffer,
       OUT PVOID ReturnBuffer
    )
    {

    //
    //  The purpose of this function is to copy the contents of the logging
    //  buffer to the output buffer and reset the logging buffer to zero at
    //  the same time.  Since this can occur realtime while new logging is
    //  written to the logging buffer, must be multiprocessor/multithread
    //  safe.  This means atomic "FetchAndSet" of each element to zero.
    //  Caller should handle exceptions from invalid memory access.
    //

#ifdef _M_IX86

    __asm {

        mov     ecx, BufferSize
        xor     eax, eax
        mov     esi, ActiveBuffer
        mov     edx, ecx
        mov     edi, ReturnBuffer
        shr     ecx, 2          ; ecx is now number of dwords to move
        jz      BYTES

LOOPD:  xchg    [esi], eax      ; xchg implies lock prefix (mp safe)
        add     esi, 4
        stosd                   ; mov dword ptr [edi++], eax
        xor     eax, eax
        dec     ecx
        jnz     LOOPD

BYTES:  and     edx, 3          ; edx is now remainder odd bytes to move
        jz      DONE

LOOPB:  xchg    [esi], al       ; xchg implies lock prefix (mp safe)
        add     esi, 1
        stosb                   ; mov byte ptr [edi++], al
        mov     al, 0
        dec     edx
        jnz     LOOPB

DONE:
        }

#else // not x86

    #error "Must implement MoveLoggingInformationToBuffer() for this platform"

#endif // _M_IX86

    }


NTSTATUS
STDCALL
CoverageFlushLogging(
    IN LPCSTR BinaryName OPTIONAL
    )
    {
    PREGISTERED_BINARY pNode;
    PCHAR BaseOfLogging;
    ULONG SizeOfLogging;
    NTSTATUS Status = STATUS_SUCCESS;

    AcquireRegistrationMutex();

    try {

        pNode = RegisteredBinaryList;

        while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

            if (( BinaryName == NULL ) ||
                ( strcmp( pNode->BinaryName, BinaryName ) == 0 )) {

                BaseOfLogging = pNode->BaseOfLoggingRegion;

                if ( BaseOfLogging != NULL ) {

                    SizeOfLogging = pNode->SizeOfLoggingRegion;

                    if ( SizeOfLogging != 0 ) {

                        try {
                            RtlZeroMemory( BaseOfLogging, SizeOfLogging );
                            }
                        except ( EXCEPTION_EXECUTE_HANDLER ) {
                            Status = GetExceptionCode();
                            }
                        }
                    }

                if ( pNode->CapturedLogging ) {
                    ExFreePool( pNode->CapturedLogging );
                    pNode->CapturedLogging = NULL;
                    }

                if ( BinaryName != NULL ) {
                    break;
                    }
                }

            pNode = pNode->pNext;
            }

        if (( BinaryName != NULL ) && ( Status == STATUS_SUCCESS )) {
            Status = STATUS_NOT_FOUND;
            }
        }

    except ( EXCEPTION_EXECUTE_HANDLER ) {
        Status = GetExceptionCode();
        }

    ReleaseRegistrationMutex();

    return Status;
    }


NTSTATUS
STDCALL
CoverageEnumerateBinaries(
    IN  ULONG  BufferSize,
    OUT PCHAR  Buffer,
    OUT PULONG ActualSize
    )
    {
    PREGISTERED_BINARY pNode;
    NTSTATUS Status = STATUS_SUCCESS;
    PCHAR BufferPointer = Buffer;

    AcquireRegistrationMutex();

    try {

        ULONG TotalSize = 0;

        if ( ActualSize ) {
            *ActualSize = 0;
            }

        pNode = RegisteredBinaryList;

        while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

            ULONG NameLength = strlen( pNode->BinaryName ) + 1;

            TotalSize += NameLength;

            if ( TotalSize <= BufferSize ) {
                RtlCopyMemory( BufferPointer, pNode->BinaryName, NameLength );
                BufferPointer += NameLength;
                }

            pNode = pNode->pNext;
            }

        if ( ++TotalSize <= BufferSize ) {
            *BufferPointer = 0;
            }
        else {
            Status = STATUS_BUFFER_TOO_SMALL;
            }

        if ( ActualSize ) {
            *ActualSize = TotalSize;
            }
        }
    except ( EXCEPTION_EXECUTE_HANDLER ) {
        Status = GetExceptionCode();
        }

    ReleaseRegistrationMutex();

    return Status;
    }


VOID
STDCALL
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    )
    {
    PREGISTERED_BINARY pNode, pNext;

    //
    //  NOTE: We're relying on NT's refcounting for this driver to prevent it
    //        from being unloaded while instrumented drivers that import from
    //        this driver are still loaded.  Since we may have hooked other
    //        instrumented drivers unload routines to call code in this driver
    //        first, the system would blow up if this driver were unloaded and
    //        later an instrumented driver was unloaded.
    //

    AcquireRegistrationMutex();

    pNode = RegisteredBinaryList;

    RegisteredBinaryList = (PREGISTERED_BINARY)&RegisteredBinaryList;   // make list empty

    ReleaseRegistrationMutex();

    while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

        if ( pNode->CapturedLogging ) {

            CoverageDbgPrint(
                DBGLOG_ERROR,
                "COVERAGE: %s logging data lost when coverage.sys unloaded.\n",
                pNode->BinaryName
                );

            ExFreePool( pNode->CapturedLogging );

            }

        pNext = pNode->pNext;

        ExFreePool( pNode );

        pNode = pNext;

        }

    IoDeleteDevice( DriverObject->DeviceObject );

    CoverageDbgPrint( DBGLOG_INFO, "COVERAGE: Driver unloaded\n" );

    }




