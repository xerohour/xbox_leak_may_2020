
/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    api.h

Abstract:

    This header file is used to cause the correct machine/platform specific
    data structures to be used when compiling for a non-hosted platform.

Author:

    Wesley Witt (wesw) 2-Aug-1993

Environment:

    User Mode

--*/



#undef i386
#undef _X86_
#undef MIPS
#undef _MIPS_
#undef ALPHA
#undef _ALPHA_
#undef PPC
#undef _PPC_

//
// Get rid of as much of Windows as possible
//

#define  NOGDICAPMASKS
#define  NOVIRTUALKEYCODES
#define  NOWINMESSAGES
#define  NOWINSTYLES
#define  NOSYSMETRICS
#define  NOMENUS
#define  NOICONS
#define  NOKEYSTATES
#define  NOSYSCOMMANDS
#define  NORASTEROPS
#define  NOSHOWWINDOW
#define  OEMRESOURCE
#define  NOATOM
#define  NOCLIPBOARD
#define  NOCOLOR
#define  NOCTLMGR
#define  NODRAWTEXT
#define  NOGDI
#define  NOKERNEL
#define  NOUSER
#define  NOMB
#define  NOMEMMGR
#define  NOMETAFILE
#define  NOMINMAX
#define  NOMSG
#define  NOOPENFILE
#define  NOSCROLL
#define  NOSERVICE
#define  NOSOUND
#define  NOTEXTMETRIC
#define  NOWH
#define  NOWINOFFSETS
#define  NOCOMM
#define  NOKANJI
#define  NOHELP
#define  NOPROFILER
#define  NODEFERWINDOWPOS

#define ADDRESS_NOT_VALID 0
#define ADDRESS_VALID 1
#define ADDRESS_TRANSITION 2


//-----------------------------------------------------------------------------------------
//
// intel x86
//
//-----------------------------------------------------------------------------------------
#if defined(TARGET_i386)

#pragma message( "Compiling for target = x86" )

#define EXR_ADDRESS_BIAS 0

#define _X86_

#if defined(HOST_MIPS)
#define MIPS
#endif

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntos.h>
#include <ntdbg.h>
#include <iop.h>
#include <windows.h>
#include <dbghelp.h>
#include <wdbgexts.h>
#include <stdlib.h>
#include <string.h>

#if defined(HOST_MIPS)
#undef MIPS
#undef R4000
#undef R3000
#endif
#define i386

#ifdef POOL_TAGGING
#undef ExAllocatePool
#undef ExAllocatePoolWithQuota
#endif

#ifndef __cplusplus
#include <mi.h>
#endif

#ifdef POOL_TAGGING
#undef ExAllocatePool
#undef ExAllocatePoolWithQuota
#endif

#undef i386
#if defined(HOST_MIPS)
#define R4000
#define R3000
#define MIPS
#endif

#if defined(HOST_MIPS)
#undef _cdecl
#define _cdecl
#endif

#if defined(HOST_ALPHA)
#undef _cdecl
#define _cdecl
#endif

#if defined(HOST_PPC)
#undef _cdecl
#define _cdecl
#endif

#if !defined(HOST_i386)
#undef _X86_
#endif

#else

//-----------------------------------------------------------------------------------------
//
// unknown platform
//
//-----------------------------------------------------------------------------------------
#error "Unsupported target CPU"

#endif


//-----------------------------------------------------------------------------------------
//
//  api declaration macros & api access macros
//
//-----------------------------------------------------------------------------------------

extern WINDBG_EXTENSION_APIS ExtensionApis;

#define KD_OBJECT_HEADER_TO_NAME_INFO( roh, loh ) (POBJECT_HEADER_NAME_INFO) \
    (((loh)->Flags & OB_FLAG_NAMED_OBJECT) ? OBJECT_HEADER_TO_OBJECT_HEADER_NAME_INFO(roh) : NULL)

//-----------------------------------------------------------------------------------------
//
//  prototypes for internal non-exported support functions
//
//-----------------------------------------------------------------------------------------

//
// get data from DebuggerData or GetExpression as appropriate
//

char ___SillyString[];
#ifdef __cplusplus
extern "C"
#endif
KDDEBUGGER_DATA32 KdDebuggerData;

#define GetNtDebuggerData(NAME)                                    \
    (HaveDebuggerData()?                                           \
        ( ( (KdDebuggerData.Header.OwnerTag = KDBG_TAG),           \
            (KdDebuggerData.Header.Size = sizeof(KdDebuggerData)), \
            Ioctl( IG_GET_DEBUGGER_DATA, &KdDebuggerData, sizeof(KdDebuggerData))),  \
        (ULONG) KdDebuggerData.NAME)                               \
        :                                                          \
        GetExpression( (strcpy(___SillyString, #NAME), ___SillyString) ))


#define GetNtDebuggerDataValue(NAME)                                   \
    (HaveDebuggerData()?                                               \
        GetUlongFromAddress(                                           \
          (((KdDebuggerData.Header.OwnerTag = KDBG_TAG),                \
           (KdDebuggerData.Header.Size = sizeof(KdDebuggerData)),      \
           Ioctl( IG_GET_DEBUGGER_DATA, &KdDebuggerData, sizeof(KdDebuggerData))),  \
        (ULONG)KdDebuggerData.NAME))                                   \
        :                                                              \
        GetUlongValue( (strcpy(___SillyString, #NAME), ___SillyString) ))

BOOL
HaveDebuggerData(
    VOID
    );

BOOL
ReadPcr(
    USHORT  Processor,
    PVOID   Pcr,
    PULONG  AddressOfPcr,
    HANDLE  hThread
    );

ULONG
GetUlongFromAddress (
    ULONG Location
    );

ULONG
GetUlongValue (
    PCHAR String
    );

#ifndef __cplusplus
ULONG
MiGetFrameFromPte (
    PMMPTE lpte
    );
#endif

#define move(dst, src)\
try {\
    ReadMemory((DWORD) (src), &(dst), sizeof(dst), NULL);\
} except (EXCEPTION_EXECUTE_HANDLER) {\
    return;\
}

#define moveBlock(dst, src, size)\
try {\
    ReadMemory((DWORD) (src), &(dst), (size), NULL);\
} except (EXCEPTION_EXECUTE_HANDLER) {\
    return;\
}


#define _KB (PAGE_SIZE/1024)


//
//  Read remote memory to local space, saving the remote pointer
//

__inline BOOLEAN
ReadAtAddress(
    PVOID RemoteAddress,
    PVOID LocalAddress,
    ULONG ObjectSize,
    PVOID *SavedRemoteAddress OPTIONAL
    )
{
    ULONG _r;
    PVOID Temp;

    Temp = RemoteAddress;

    if (!ReadMemory( (ULONG)RemoteAddress, LocalAddress, ObjectSize, &_r ) || (_r < ObjectSize)) {

        dprintf("Can't Read Memory at %08lx\n", RemoteAddress);
        return FALSE;
    }

    if (SavedRemoteAddress) {

        *SavedRemoteAddress = Temp;
    }

    return TRUE;
}

//
//  Splay helpers similar to the regular RTL
//

#define DbgRtlParent(Links) (           \
    (PRTL_SPLAY_LINKS)(Links).Parent \
    )

#define DbgRtlLeftChild(Links) (           \
    (PRTL_SPLAY_LINKS)(Links).LeftChild \
    )

#define DbgRtlRightChild(Links) (           \
    (PRTL_SPLAY_LINKS)(Links).RightChild \
    )

__inline BOOLEAN
DbgRtlIsRightChild(
    RTL_SPLAY_LINKS Links,
    PRTL_SPLAY_LINKS pLinks,
    PRTL_SPLAY_LINKS Parent
    )
{
    if (DbgRtlParent(Links) == pLinks) {

        return FALSE;
    }

    if (!ReadAtAddress(DbgRtlParent(Links), Parent, sizeof(RTL_SPLAY_LINKS), NULL)) {

        return FALSE;
    }

    if (DbgRtlRightChild(*Parent) == pLinks) {

        return TRUE;
    }

    return FALSE;
}

__inline BOOLEAN
DbgRtlIsLeftChild(
    RTL_SPLAY_LINKS Links,
    PRTL_SPLAY_LINKS pLinks,
    PRTL_SPLAY_LINKS Parent
    )
{
    if (DbgRtlParent(Links) == pLinks) {

        return FALSE;
    }

    if (!ReadAtAddress(DbgRtlParent(Links), Parent, sizeof(RTL_SPLAY_LINKS), NULL)) {

        return FALSE;
    }

    if (DbgRtlLeftChild(*Parent) == pLinks) {

        return TRUE;
    }

    return FALSE;
}

/////////////////////////////////////////////
//
//  KdExts.c
//
/////////////////////////////////////////////
BOOLEAN
IsCheckedBuild(
    PBOOLEAN Checked
    );

/////////////////////////////////////////////
//
//  Cxr.c
//
/////////////////////////////////////////////
VOID
DumpCxr(
    PCONTEXT Context
    );


/////////////////////////////////////////////
//
//  CritSec.c
//
/////////////////////////////////////////////

PLIST_ENTRY
DumpCritSec(
    HANDLE  hCurrentProcess,
    DWORD   dwAddrCritSec,
    BOOLEAN bDumpIfUnowned
    );



/////////////////////////////////////////////
//
//  Device.c
//
/////////////////////////////////////////////

VOID
DumpDevice(
    PVOID DeviceAddress,
    ULONG FieldWidth,
    BOOLEAN FullDetail
    );

/////////////////////////////////////////////
//
// Devnode.c
//
/////////////////////////////////////////////

typedef struct _FLAG_NAME {
    ULONG Flag;
    PUCHAR Name;
} FLAG_NAME, *PFLAG_NAME;

VOID
DumpFlags(
         ULONG Depth,
         LPSTR FlagDescription,
         ULONG Flags,
         PFLAG_NAME FlagTable
         );

VOID
xdprintf(
        ULONG  Depth,
        PCCHAR S,
        ...
        );

BOOLEAN
xReadMemory (
            PVOID S,
            PVOID D,
            ULONG Len
            );

/////////////////////////////////////////////
//
//  Driver.c
//
/////////////////////////////////////////////

VOID
DumpDriver(
    PVOID DriverAddress,
    ULONG FieldWidth,
    ULONG Flags
    );

/////////////////////////////////////////////
//
//  Help.c
//
/////////////////////////////////////////////

VOID
SpecificHelp (
    VOID
    );



/////////////////////////////////////////////
//
//  Locks.c
//
/////////////////////////////////////////////

VOID
DumpStaticFastMutex (
    IN PCHAR Name
    );

/////////////////////////////////////////////
//
//  Memory.c
//
/////////////////////////////////////////////
#ifndef __cplusplus
VOID
MemoryUsage (
    IN PMMPFN PfnArray,
    IN ULONG LowPage,
    IN ULONG HighPage,
    IN ULONG IgnoreInvalidFrames
    );

/////////////////////////////////////////////
//
//  Mi.c
//
/////////////////////////////////////////////

ULONG
MiGetFrameFromPte (
    IN PMMPTE lpte
    );

ULONG
MiGetFreeCountFromPteList (
    IN PMMPTE Pte
    );

ULONG
MiGetNextFromPteList (
    IN PMMPTE Pte
    );

ULONG
MiGetPageFromPteList (
    IN ULONG Pte
    );

PMMPTE
DbgGetPdeAddress(
    IN PVOID VirtualAddress
    );

PMMPTE
DbgGetPteAddress(
    IN PVOID VirtualAddress
    );

PVOID
DbgGetVirtualAddressMappedByPte(
    IN PMMPTE Pte
    );

ULONG
DbgGetPteSize(
    VOID
    );
#endif
/////////////////////////////////////////////
//
//  Registry.c
//
/////////////////////////////////////////////
USHORT
GetKcbName(
    DWORD KcbAddr,
    PWCHAR NameBuffer,
    ULONG  BufferSize
);


/////////////////////////////////////////////
//
//  Object.c
//
/////////////////////////////////////////////
#if defined( __cplusplus )
extern "C" {
#endif
extern ULONG EXPRLastDump;
#if defined( __cplusplus )
}
#endif

typedef BOOLEAN (*ENUM_TYPE_ROUTINE)(
    IN PVOID            pObjectHeader,
    IN POBJECT_HEADER   ObjectHeader,
    IN PVOID            Parameter
    );

//
// Object Table Entry Structure
//
typedef struct _OBJECT_TABLE_ENTRY {
    ULONG       NonPagedObjectHeader;
    ACCESS_MASK GrantedAccess;
} OBJECT_TABLE_ENTRY, *POBJECT_TABLE_ENTRY;
#define LOG_OBJECT_TABLE_ENTRY_SIZE 1

BOOLEAN
FetchObjectManagerVariables(
    BOOLEAN ForceReload
    );

PVOID
FindObjectByName(
    IN PUCHAR Path,
    IN PVOID RootObject
    );

POBJECT_TYPE
FindObjectType(
    IN PUCHAR TypeName
    );

BOOLEAN
DumpObject(
    IN char     *Pad,
    IN PVOID    Object,
    IN POBJECT_HEADER OptObjectHeader OPTIONAL,
    IN ULONG    Flags
    );

BOOLEAN
WalkObjectsByType(
    IN PUCHAR               ObjectTypeName,
    IN ENUM_TYPE_ROUTINE    EnumRoutine,
    IN PVOID                Parameter
    );

BOOLEAN
CaptureObjectName(
    IN PVOID            pObjectHeader,
    IN POBJECT_HEADER   ObjectHeader,
    IN PWSTR            Buffer,
    IN ULONG            BufferSize
    );

VOID
DumpObjectName(
   PVOID ObjectAddress
   );


/////////////////////////////////////////////
//
//  Process.c
//
/////////////////////////////////////////////

extern UCHAR *WaitReasonList[];

PVOID
GetCurrentProcessAddress(
    DWORD    Processor,
    HANDLE hCurrentThread,
    PETHREAD CurrentThread
    );

PVOID
GetCurrentThreadAddress(
    USHORT Processor,
    HANDLE hCurrentThread
    );

BOOL
DumpProcess (
    IN char * pad,
    IN PKPROCESS ProcessContents,
    IN PKPROCESS RealProcessBase,
    IN ULONG Flags,
    IN PCHAR ImageFileName
    );

BOOL
DumpThread (
    IN ULONG Processor,
    IN char *Pad,
    IN PETHREAD Thread,
    IN PETHREAD RealThreadBase,
    IN ULONG Flags
    );

VOID
dumpSymbolicAddress(
    ULONG Address,
    PUCHAR Buffer,
    BOOL AlwaysShowHex
    );

BOOLEAN
FetchProcessStructureVariables(
    VOID
    );

PVOID
LookupUniqueId(
    HANDLE UniqueId
    );

ULONG
GetAddressState(
    IN PVOID VirtualAddress
    );

typedef struct _PROCESS_COMMIT_USAGE {
    UCHAR ImageFileName[ 16 ];
    ULONG CommitCharge;
    ULONG NumberOfPrivatePages;
    ULONG NumberOfLockedPages;
} PROCESS_COMMIT_USAGE, *PPROCESS_COMMIT_USAGE;

PPROCESS_COMMIT_USAGE
GetProcessCommit (
    PULONG TotalCommitCharge,
    PULONG NumberOfProcesses
    );



/////////////////////////////////////////////
//
//  Util.c
//
/////////////////////////////////////////////

VOID
DumpImageName(
    IN PKPROCESS ProcessContents
    );

typedef VOID
(*PDUMP_SPLAY_NODE_FN)(
    PVOID RemoteAddress,
    ULONG Level
    );

ULONG
DumpSplayTree(
    IN PVOID pSplayLinks,
    IN PDUMP_SPLAY_NODE_FN DumpNodeFn
    );

VOID
DumpUnicode(
    UNICODE_STRING u
    );

BOOLEAN
IsHexNumber(
   const char *szExpression
   );

BOOLEAN
IsDecNumber(
   const char *szExpression
   );

#ifndef __cplusplus
typedef 
BOOLEAN 
(WINAPI *POOLFILTER) (
    PCHAR Tag,
    PCHAR Filter,
    ULONG Flags,
    PPOOL_HEADER PoolHeader,
    ULONG BlockSize,
    PVOID Data,
    PVOID Context
    );


void SearchPool(
    ULONG TagName,
    ULONG RestartAddr,
    POOLFILTER Filter,
    PVOID Context 
    );

BOOLEAN
CheckSingleFilter (
    PCHAR Tag,
    PCHAR Filter
    );
#endif

/////////////////////////////////////////////
//
//  Exsup.c
//
/////////////////////////////////////////////

VOID
InterpretExceptionData(
    PLAST_EXCEPTION_LOG LogRecord,
    PVOID *Terminator,
    PVOID *Filter,
    PVOID *Handler
    );



//
// Miscellaneous includes for semi-private NT definitions
//

#include <heap.h>
#include <pool.h>
