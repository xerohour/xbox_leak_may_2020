/************************************************************************
*                                                                       *
*   winbase.h -- This module defines the 32-Bit Windows Base APIs       *
*                                                                       *
*   Copyright (c) 1990-2001, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/
#ifndef _WINBASE_
#define _WINBASE_

;begin_internal
/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1985-2001, Microsoft Corporation

Module Name:

    winbasep.h

Abstract:

    Private
    Procedure declarations, constant definitions and macros for the Base
    component.

--*/
#ifndef _WINBASEP_
#define _WINBASEP_
;end_internal

//
// Define API decoration for direct importing of DLL references.
//

;begin_noxbox

#if !defined(_ADVAPI32_)
#define WINADVAPI DECLSPEC_IMPORT
#else
#define WINADVAPI
#endif

;end_noxbox

#define WINBASEAPI

;begin_both
#ifdef __cplusplus
extern "C" {
#endif
;end_both

/*
 * Compatibility macros
 */

#define GetCurrentTime()                GetTickCount()

#define Yield()

#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2

#define TIME_ZONE_ID_INVALID ((DWORD)0xFFFFFFFF)

#define WAIT_FAILED ((DWORD)0xFFFFFFFF)
#define WAIT_OBJECT_0       ((STATUS_WAIT_0 ) + 0 )

#define WAIT_ABANDONED         ((STATUS_ABANDONED_WAIT_0 ) + 0 )
#define WAIT_ABANDONED_0       ((STATUS_ABANDONED_WAIT_0 ) + 0 )

#define WAIT_IO_COMPLETION                  STATUS_USER_APC
#define STILL_ACTIVE                        STATUS_PENDING
#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE
#define CONTROL_C_EXIT                      STATUS_CONTROL_C_EXIT
#define MoveMemory RtlMoveMemory
#define CopyMemory RtlCopyMemory
#define FillMemory RtlFillMemory
#define ZeroMemory RtlZeroMemory
#define HeapAlloc   RtlAllocateHeap
#define HeapReAlloc RtlReAllocateHeap
#define HeapSize    RtlSizeHeap
#define InitializeCriticalSection RtlInitializeCriticalSection
#define DeleteCriticalSection     RtlDeleteCriticalSection
#define EnterCriticalSection      RtlEnterCriticalSection
#define LeaveCriticalSection      RtlLeaveCriticalSection
#define TryEnterCriticalSection   RtlTryEnterCriticalSection
#if !defined(_NTOS_)
#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedDecrement       _InterlockedDecrement
#define InterlockedExchange        _InterlockedExchange
#define InterlockedExchangeAdd     _InterlockedExchangeAdd
#define InterlockedIncrement       _InterlockedIncrement
#endif // !defined(_NTOS_)

//
// File creation flags must start at the high end since they
// are combined with the attributes
//

#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

//
// Define possible return codes from the CopyFileEx callback routine
//

#define PROGRESS_CONTINUE   0
#define PROGRESS_CANCEL     1
#define PROGRESS_QUIET      3

//
// Define CopyFileEx callback routine state change values
//

#define CALLBACK_CHUNK_FINISHED         0x00000000
#define CALLBACK_STREAM_SWITCH          0x00000001

//
// Define CopyFileEx option flags
//

#define COPY_FILE_FAIL_IF_EXISTS        0x00000001
#define COPY_FILE_OPEN_SOURCE_FOR_WRITE 0x00000004


//
//  File structures
//

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    DWORD   Offset;
    DWORD   OffsetHigh;
    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef LPVOID PSECURITY_ATTRIBUTES;
typedef LPVOID LPSECURITY_ATTRIBUTES;

//
//  File System time stamps are represented with the following structure:
//

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

//
// System time is represented with the following structure:
//


typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;


typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
    LPVOID lpThreadParameter
    );
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef VOID (WINAPI *PFIBER_START_ROUTINE)(
    LPVOID lpFiberParameter
    );
typedef PFIBER_START_ROUTINE LPFIBER_START_ROUTINE;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

;begin_noxbox

typedef RTL_CRITICAL_SECTION_DEBUG CRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG PCRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG LPCRITICAL_SECTION_DEBUG;

;end_noxbox

#define MUTEX_MODIFY_STATE MUTANT_QUERY_STATE
#define MUTEX_ALL_ACCESS MUTANT_ALL_ACCESS

;begin_noxbox

typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;          // Obsolete field...do not use
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        };
    };
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

//
//


#define FreeModule(hLibModule) FreeLibrary((hLibModule))
#define MakeProcInstance(lpProc,hInstance) (lpProc)
#define FreeProcInstance(lpProc) (lpProc)

;end_noxbox

/* Global Memory Flags */
#define GMEM_FIXED          0x0000
#define GMEM_MOVEABLE       0x0002
#define GMEM_NOCOMPACT      0x0010
#define GMEM_NODISCARD      0x0020
#define GMEM_ZEROINIT       0x0040
#define GMEM_MODIFY         0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE          0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_NOTIFY         0x4000
#define GMEM_LOWER          GMEM_NOT_BANKED
#define GMEM_VALID_FLAGS    0x7F72
#define GMEM_INVALID_HANDLE 0x8000

#define GHND                (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR                (GMEM_FIXED | GMEM_ZEROINIT)

#define GlobalDiscard( h )      GlobalReAlloc( (h), 0, GMEM_MOVEABLE )

/* Flags returned by GlobalFlags (in addition to GMEM_DISCARDABLE) */
#define GMEM_DISCARDED      0x4000
#define GMEM_LOCKCOUNT      0x00FF

typedef struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

/* Local Memory Flags */
#define LMEM_FIXED          0x0000
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010
#define LMEM_NODISCARD      0x0020
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080
#define LMEM_DISCARDABLE    0x0F00
#define LMEM_VALID_FLAGS    0x0F72
#define LMEM_INVALID_HANDLE 0x8000

#define LHND                (LMEM_MOVEABLE | LMEM_ZEROINIT)
#define LPTR                (LMEM_FIXED | LMEM_ZEROINIT)

#define NONZEROLHND         (LMEM_MOVEABLE)
#define NONZEROLPTR         (LMEM_FIXED)

#define LocalDiscard( h )   LocalReAlloc( (h), 0, LMEM_MOVEABLE )

/* Flags returned by LocalFlags (in addition to LMEM_DISCARDABLE) */
#define LMEM_DISCARDED      0x4000
#define LMEM_LOCKCOUNT      0x00FF

//
// dwCreationFlag values
//

;begin_noxbox

#define DEBUG_PROCESS               0x00000001
#define DEBUG_ONLY_THIS_PROCESS     0x00000002

;end_noxbox

#define CREATE_SUSPENDED            0x00000004

#define THREAD_PRIORITY_LOWEST          THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL    (THREAD_PRIORITY_LOWEST+1)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_HIGHEST         THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL    (THREAD_PRIORITY_HIGHEST-1)
#define THREAD_PRIORITY_ERROR_RETURN    (MAXLONG)

#define THREAD_PRIORITY_TIME_CRITICAL   THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE            THREAD_BASE_PRIORITY_IDLE

;begin_noxbox

//
// Debug APIs
//
#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

typedef struct _EXCEPTION_DEBUG_INFO {
    EXCEPTION_RECORD ExceptionRecord;
    DWORD dwFirstChance;
} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO, *LPCREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO, *LPCREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO, *LPEXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO, *LPEXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID lpBaseOfDll;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpImageName;
    WORD fUnicode;
} LOAD_DLL_DEBUG_INFO, *LPLOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO, *LPUNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD fUnicode;
    WORD nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO, *LPOUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO, *LPRIP_INFO;


typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO Exception;
        CREATE_THREAD_DEBUG_INFO CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO ExitThread;
        EXIT_PROCESS_DEBUG_INFO ExitProcess;
        LOAD_DLL_DEBUG_INFO LoadDll;
        UNLOAD_DLL_DEBUG_INFO UnloadDll;
        OUTPUT_DEBUG_STRING_INFO DebugString;
        RIP_INFO RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

;end_noxbox

#if !defined(MIDL_PASS)
typedef PCONTEXT LPCONTEXT;
typedef PEXCEPTION_RECORD LPEXCEPTION_RECORD;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;
#endif

;begin_noxbox

#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5

#define FILE_TYPE_UNKNOWN   0x0000
#define FILE_TYPE_DISK      0x0001
#define FILE_TYPE_CHAR      0x0002
#define FILE_TYPE_REMOTE    0x8000

#define STD_INPUT_HANDLE    ((DWORD)-10)
#define STD_OUTPUT_HANDLE   ((DWORD)-11)
#define STD_ERROR_HANDLE    ((DWORD)-12)

;end_noxbox

#define IGNORE              0       // Ignore signal
#define INFINITE            0xFFFFFFFF  // Infinite timeout

;begin_noxbox

#define FS_CASE_IS_PRESERVED            FILE_CASE_PRESERVED_NAMES
#define FS_CASE_SENSITIVE               FILE_CASE_SENSITIVE_SEARCH
#define FS_UNICODE_STORED_ON_DISK       FILE_UNICODE_ON_DISK
#define FS_PERSISTENT_ACLS              FILE_PERSISTENT_ACLS
#define FS_VOL_IS_COMPRESSED            FILE_VOLUME_IS_COMPRESSED
#define FS_FILE_COMPRESSION             FILE_FILE_COMPRESSION
#define FS_FILE_ENCRYPTION              FILE_SUPPORTS_ENCRYPTION

#define OF_READ             0x00000000
#define OF_WRITE            0x00000001
#define OF_READWRITE        0x00000002
#define OF_SHARE_COMPAT     0x00000000
#define OF_SHARE_EXCLUSIVE  0x00000010
#define OF_SHARE_DENY_WRITE 0x00000020
#define OF_SHARE_DENY_READ  0x00000030
#define OF_SHARE_DENY_NONE  0x00000040
#define OF_PARSE            0x00000100
#define OF_DELETE           0x00000200
#define OF_VERIFY           0x00000400
#define OF_CANCEL           0x00000800
#define OF_CREATE           0x00001000
#define OF_PROMPT           0x00002000
#define OF_EXIST            0x00004000
#define OF_REOPEN           0x00008000

#define OFS_MAXPATHNAME 128
typedef struct _OFSTRUCT {
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    WORD Reserved1;
    WORD Reserved2;
    CHAR szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *LPOFSTRUCT, *POFSTRUCT;

;end_noxbox

#ifndef _NTOS_

WINBASEAPI
LONG
WINAPI
InterlockedIncrement(
    IN OUT LPLONG lpAddend
    );

WINBASEAPI
LONG
WINAPI
InterlockedDecrement(
    IN OUT LPLONG lpAddend
    );

WINBASEAPI
LONG
WINAPI
InterlockedExchange(
    IN OUT LPLONG Target,
    IN LONG Value
    );

#define InterlockedExchangePointer(Target, Value) \
    (PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value))

WINBASEAPI
LONG
WINAPI
InterlockedExchangeAdd(
    IN OUT LPLONG Addend,
    IN LONG Value
    );

WINBASEAPI
LONG
WINAPI
InterlockedCompareExchange (
    IN OUT LPLONG Destination,
    IN LONG Exchange,
    IN LONG Comperand
    );

#ifdef __cplusplus
// Use a function for C++ so X86 will generate the same errors as RISC.
__inline
PVOID
__cdecl
__InlineInterlockedCompareExchangePointer (
    IN OUT PVOID *Destination,
    IN PVOID ExChange,
    IN PVOID Comperand
    )
{
    return((PVOID)InterlockedCompareExchange((PLONG)Destination, (LONG)ExChange, (LONG)Comperand));
}
#define InterlockedCompareExchangePointer __InlineInterlockedCompareExchangePointer

#else

#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    (PVOID)InterlockedCompareExchange((PLONG)(Destination), (LONG)(ExChange), (LONG)(Comperand))

#endif

#endif /* NT_INCLUDED */

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
FreeResource(
        IN HGLOBAL hResData
        );

WINBASEAPI
LPVOID
WINAPI
LockResource(
        IN HGLOBAL hResData
        );

#define UnlockResource(hResData) ((hResData), 0)
#define MAXINTATOM 0xC000
#define MAKEINTATOM(i)  (LPTSTR)((ULONG_PTR)((WORD)(i)))
#define INVALID_ATOM ((ATOM)0)

int
WINAPI
WinMain(
    IN HINSTANCE hInstance,
    IN HINSTANCE hPrevInstance,
    IN LPSTR lpCmdLine,
    IN int nShowCmd
    );

WINBASEAPI
BOOL
WINAPI
FreeLibrary(
    IN OUT HMODULE hLibModule    ;public_NT
    IN OUT HINSTANCE hLibModule  ;public_chicago
    );

WINBASEAPI
DECLSPEC_NORETURN
VOID
WINAPI
FreeLibraryAndExitThread(
    IN HMODULE hLibModule,
    IN DWORD dwExitCode
    );

WINBASEAPI
BOOL
WINAPI
DisableThreadLibraryCalls(
    IN HMODULE hLibModule
    );

WINBASEAPI
FARPROC
WINAPI
GetProcAddress(
    IN HMODULE hModule,    ;public_NT
    IN HINSTANCE hModule,  ;public_chicago
    IN LPCSTR lpProcName
    );

;end_noxbox

WINBASEAPI
HGLOBAL
WINAPI
GlobalAlloc(
    IN UINT uFlags,
    IN SIZE_T dwBytes
    );

WINBASEAPI
HGLOBAL
WINAPI
GlobalReAlloc(
    IN HGLOBAL hMem,
    IN SIZE_T dwBytes,
    IN UINT uFlags
    );

WINBASEAPI
UINT
WINAPI
GlobalFlags(
    IN HGLOBAL hMem
    );

#define GlobalSize LocalSize
#define GlobalLock LocalLock
#define GlobalHandle LocalHandle
#define GlobalUnlock LocalUnlock
#define GlobalFree LocalFree

;begin_noxbox

WINBASEAPI
SIZE_T
WINAPI
GlobalCompact(
    IN DWORD dwMinFree
    );

WINBASEAPI
VOID
WINAPI
GlobalFix(
    IN HGLOBAL hMem
    );

WINBASEAPI
VOID
WINAPI
GlobalUnfix(
    IN HGLOBAL hMem
    );

WINBASEAPI
LPVOID
WINAPI
GlobalWire(
    IN HGLOBAL hMem
    );

WINBASEAPI
BOOL
WINAPI
GlobalUnWire(
    IN HGLOBAL hMem
    );

;end_noxbox

WINBASEAPI
VOID
WINAPI
GlobalMemoryStatus(
    IN OUT LPMEMORYSTATUS lpBuffer
    );

;begin_noxbox

typedef struct _MEMORYSTATUSEX {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;

WINBASEAPI
BOOL
WINAPI
GlobalMemoryStatusEx(
    IN OUT LPMEMORYSTATUSEX lpBuffer
    );

;end_noxbox

WINBASEAPI
HLOCAL
WINAPI
LocalAlloc(
    IN UINT uFlags,
    IN SIZE_T uBytes
    );

WINBASEAPI
HLOCAL
WINAPI
LocalReAlloc(
    IN HLOCAL hMem,
    IN SIZE_T uBytes,
    IN UINT uFlags
    );

WINBASEAPI
LPVOID
WINAPI
LocalLock(
    IN HLOCAL hMem
    );

WINBASEAPI
HLOCAL
WINAPI
LocalHandle(
    IN LPCVOID pMem
    );

WINBASEAPI
BOOL
WINAPI
LocalUnlock(
    IN HLOCAL hMem
    );

WINBASEAPI
SIZE_T
WINAPI
LocalSize(
    IN HLOCAL hMem
    );

WINBASEAPI
UINT
WINAPI
LocalFlags(
    IN HLOCAL hMem
    );

WINBASEAPI
HLOCAL
WINAPI
LocalFree(
    IN HLOCAL hMem
    );

;begin_noxbox

WINBASEAPI
SIZE_T
WINAPI
LocalShrink(
    IN HLOCAL hMem,
    IN UINT cbNewSize
    );

WINBASEAPI
SIZE_T
WINAPI
LocalCompact(
    IN UINT uMinFree
    );

;end_noxbox

WINBASEAPI
LPVOID
WINAPI
VirtualAlloc(
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD flAllocationType,
    IN DWORD flProtect
    );

WINBASEAPI
BOOL
WINAPI
VirtualFree(
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD dwFreeType
    );

WINBASEAPI
BOOL
WINAPI
VirtualProtect(
    IN  LPVOID lpAddress,
    IN  SIZE_T dwSize,
    IN  DWORD flNewProtect,
    OUT PDWORD lpflOldProtect
    );

WINBASEAPI
DWORD
WINAPI
VirtualQuery(
    IN LPCVOID lpAddress,
    OUT PMEMORY_BASIC_INFORMATION lpBuffer,
    IN DWORD dwLength
    );

WINBASEAPI
LPVOID
WINAPI
VirtualAllocEx(
    IN HANDLE hProcess,
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD flAllocationType,
    IN DWORD flProtect
    );

WINBASEAPI
BOOL
WINAPI
VirtualFreeEx(
    IN HANDLE hProcess,
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD dwFreeType
    );

WINBASEAPI
BOOL
WINAPI
VirtualProtectEx(
    IN  HANDLE hProcess,
    IN  LPVOID lpAddress,
    IN  SIZE_T dwSize,
    IN  DWORD flNewProtect,
    OUT PDWORD lpflOldProtect
    );

WINBASEAPI
DWORD
WINAPI
VirtualQueryEx(
    IN HANDLE hProcess,
    IN LPCVOID lpAddress,
    OUT PMEMORY_BASIC_INFORMATION lpBuffer,
    IN DWORD dwLength
    );

WINBASEAPI
HANDLE
WINAPI
HeapCreate(
    IN DWORD flOptions,
    IN SIZE_T dwInitialSize,
    IN SIZE_T dwMaximumSize
    );

WINBASEAPI
BOOL
WINAPI
HeapDestroy(
    IN OUT HANDLE hHeap
    );

;begin_internal
WINBASEAPI
DWORD
WINAPI
HeapCreateTagsW(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPCWSTR lpTagPrefix,
    IN LPCWSTR lpTagNames
    );

typedef struct _HEAP_TAG_INFO {
    DWORD dwNumberOfAllocations;
    DWORD dwNumberOfFrees;
    DWORD dwBytesAllocated;
} HEAP_TAG_INFO, *PHEAP_TAG_INFO;
typedef PHEAP_TAG_INFO LPHEAP_TAG_INFO;

WINBASEAPI
LPCWSTR
WINAPI
HeapQueryTagW(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN WORD wTagIndex,
    IN BOOL bResetCounters,
    OUT LPHEAP_TAG_INFO TagInfo
    );
;end_internal

#if !defined(_NTURTL_)

WINBASEAPI
LPVOID
WINAPI
HeapAlloc(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN SIZE_T dwBytes
    );

WINBASEAPI
LPVOID
WINAPI
HeapReAlloc(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPVOID lpMem,
    IN SIZE_T dwBytes
    );

WINBASEAPI
SIZE_T
WINAPI
HeapSize(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPCVOID lpMem
    );

#endif // !defined(_NTURTL_)

WINBASEAPI
BOOL
WINAPI
HeapFree(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPVOID lpMem
    );

WINBASEAPI
BOOL
WINAPI
HeapValidate(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPCVOID lpMem
    );

WINBASEAPI
SIZE_T
WINAPI
HeapCompact(
    IN HANDLE hHeap,
    IN DWORD dwFlags
    );

WINBASEAPI
HANDLE
WINAPI
GetProcessHeap( VOID );

;begin_noxbox

WINBASEAPI
DWORD
WINAPI
GetProcessHeaps(
    IN DWORD NumberOfHeaps,
    OUT PHANDLE ProcessHeaps
    );

;end_noxbox

typedef struct _PROCESS_HEAP_ENTRY {
    PVOID lpData;
    DWORD cbData;
    BYTE cbOverhead;
    BYTE iRegionIndex;
    WORD wFlags;
    union {
        struct {
            HANDLE hMem;
            DWORD dwReserved[ 3 ];
        } Block;
        struct {
            DWORD dwCommittedSize;
            DWORD dwUnCommittedSize;
            LPVOID lpFirstBlock;
            LPVOID lpLastBlock;
        } Region;
    };
} PROCESS_HEAP_ENTRY, *LPPROCESS_HEAP_ENTRY, *PPROCESS_HEAP_ENTRY;

#define PROCESS_HEAP_REGION             0x0001
#define PROCESS_HEAP_UNCOMMITTED_RANGE  0x0002
#define PROCESS_HEAP_ENTRY_BUSY         0x0004
#define PROCESS_HEAP_ENTRY_MOVEABLE     0x0010
#define PROCESS_HEAP_ENTRY_DDESHARE     0x0020

WINBASEAPI
BOOL
WINAPI
HeapLock(
    IN HANDLE hHeap
    );

WINBASEAPI
BOOL
WINAPI
HeapUnlock(
    IN HANDLE hHeap
    );

;begin_internal

typedef struct _HEAP_SUMMARY {
    DWORD cb;
    SIZE_T cbAllocated;
    SIZE_T cbCommitted;
    SIZE_T cbReserved;
    SIZE_T cbMaxReserve;
} HEAP_SUMMARY, *PHEAP_SUMMARY;
typedef PHEAP_SUMMARY LPHEAP_SUMMARY;

BOOL
WINAPI
HeapSummary(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    OUT LPHEAP_SUMMARY lpSummary
    );

BOOL
WINAPI
HeapExtend(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN LPVOID lpBase,
    IN DWORD dwBytes
    );

typedef struct _HEAP_USAGE_ENTRY {
    struct _HEAP_USAGE_ENTRY *lpNext;
    PVOID lpAddress;
    DWORD dwBytes;
    DWORD dwReserved;
} HEAP_USAGE_ENTRY, *PHEAP_USAGE_ENTRY;

typedef struct _HEAP_USAGE {
    DWORD cb;
    SIZE_T cbAllocated;
    SIZE_T cbCommitted;
    SIZE_T cbReserved;
    SIZE_T cbMaxReserve;
    PHEAP_USAGE_ENTRY lpEntries;
    PHEAP_USAGE_ENTRY lpAddedEntries;
    PHEAP_USAGE_ENTRY lpRemovedEntries;
    DWORD Reserved[ 8 ];
} HEAP_USAGE, *PHEAP_USAGE;

BOOL
WINAPI
HeapUsage(
    IN HANDLE hHeap,
    IN DWORD dwFlags,
    IN BOOL bFirstCall,
    IN BOOL bLastCall,
    OUT PHEAP_USAGE lpUsage
    );

;end_internal

WINBASEAPI
BOOL
WINAPI
HeapWalk(
    IN HANDLE hHeap,
    IN OUT LPPROCESS_HEAP_ENTRY lpEntry
    );

;begin_noxbox

WINBASEAPI
DWORD
WINAPI
GetShortPathName%(
    IN LPCTSTR% lpszLongPath,
    OUT LPTSTR%  lpszShortPath,
    IN DWORD    cchBuffer
    );

WINBASEAPI
DWORD
WINAPI
GetLongPathName%(
    IN LPCTSTR% lpszShortPath,
    OUT LPTSTR%  lpszLongPath,
    IN DWORD    cchBuffer
    );

WINBASEAPI
BOOL
WINAPI
GetProcessTimes(
    IN HANDLE hProcess,
    OUT LPFILETIME lpCreationTime,
    OUT LPFILETIME lpExitTime,
    OUT LPFILETIME lpKernelTime,
    OUT LPFILETIME lpUserTime
    );

WINBASEAPI
HANDLE
WINAPI
GetCurrentProcess(
    VOID
    );

WINBASEAPI
DWORD
WINAPI
GetCurrentProcessId(
    VOID
    );

WINBASEAPI
DECLSPEC_NORETURN
VOID
WINAPI
ExitProcess(
    IN UINT uExitCode
    );

WINBASEAPI
BOOL
WINAPI
TerminateProcess(
    IN HANDLE hProcess,
    IN UINT uExitCode
    );

WINBASEAPI
BOOL
WINAPI
GetExitCodeProcess(
    IN HANDLE hProcess,
    OUT LPDWORD lpExitCode
    );

WINBASEAPI
VOID
WINAPI
FatalExit(
    IN int ExitCode
    );

;end_noxbox

#ifndef GetCurrentProcess
#define GetCurrentProcess() NtCurrentProcess()
#endif

WINBASEAPI
VOID
WINAPI
RaiseException(
    IN DWORD dwExceptionCode,
    IN DWORD dwExceptionFlags,
    IN DWORD nNumberOfArguments,
    IN CONST ULONG_PTR *lpArguments
    );

WINBASEAPI
LONG
WINAPI
UnhandledExceptionFilter(
    IN struct _EXCEPTION_POINTERS *ExceptionInfo
    );

typedef LONG (WINAPI *PTOP_LEVEL_EXCEPTION_FILTER)(
    struct _EXCEPTION_POINTERS *ExceptionInfo
    );
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;

WINBASEAPI
LPTOP_LEVEL_EXCEPTION_FILTER
WINAPI
SetUnhandledExceptionFilter(
    IN LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
    );

extern __declspec(thread) PVOID XapiCurrentFiber;
#define GetCurrentFiber() XapiCurrentFiber
#define GetFiberData() (*(PVOID *)(GetCurrentFiber()))

WINBASEAPI
LPVOID
WINAPI
CreateFiber(
    IN DWORD dwStackSize,
    IN LPFIBER_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter
    );

WINBASEAPI
VOID
WINAPI
DeleteFiber(
    IN LPVOID lpFiber
    );

WINBASEAPI
LPVOID
WINAPI
ConvertThreadToFiber(
    IN LPVOID lpParameter
    );

WINBASEAPI
VOID
WINAPI
SwitchToFiber(
    IN LPVOID lpFiber
    );

WINBASEAPI
BOOL
WINAPI
SwitchToThread(
    VOID
    );

WINBASEAPI
HANDLE
WINAPI
CreateThread(
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN DWORD dwStackSize,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN DWORD dwCreationFlags,
    OUT LPDWORD lpThreadId
    );

;begin_noxbox

WINBASEAPI
HANDLE
WINAPI
GetCurrentThread(
    VOID
    );

;end_noxbox

#ifndef GetCurrentThread
#define GetCurrentThread() NtCurrentThread()
#endif

WINBASEAPI
DWORD
WINAPI
GetCurrentThreadId(
    VOID
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
SetProcessPriorityBoost(
    IN HANDLE hProcess,
    IN BOOL bDisablePriorityBoost
    );

WINBASEAPI
BOOL
WINAPI
GetProcessPriorityBoost(
    IN HANDLE hProcess,
    OUT PBOOL pDisablePriorityBoost
    );

WINBASEAPI
HANDLE
WINAPI
OpenThread(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    DWORD dwThreadId
    );

;end_noxbox

WINBASEAPI
BOOL
WINAPI
SetThreadPriority(
    IN HANDLE hThread,
    IN int nPriority
    );

WINBASEAPI
BOOL
WINAPI
SetThreadPriorityBoost(
    IN HANDLE hThread,
    IN BOOL bDisablePriorityBoost
    );

WINBASEAPI
BOOL
WINAPI
GetThreadPriorityBoost(
    IN HANDLE hThread,
    OUT PBOOL pDisablePriorityBoost
    );

WINBASEAPI
int
WINAPI
GetThreadPriority(
    IN HANDLE hThread
    );

WINBASEAPI
BOOL
WINAPI
GetThreadTimes(
    IN HANDLE hThread,
    OUT LPFILETIME lpCreationTime,
    OUT LPFILETIME lpExitTime,
    OUT LPFILETIME lpKernelTime,
    OUT LPFILETIME lpUserTime
    );

WINBASEAPI
DECLSPEC_NORETURN
VOID
WINAPI
ExitThread(
    IN DWORD dwExitCode
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
TerminateThread(
    IN OUT HANDLE hThread,
    IN DWORD dwExitCode
    );

;end_noxbox

WINBASEAPI
BOOL
WINAPI
GetExitCodeThread(
    IN HANDLE hThread,
    OUT LPDWORD lpExitCode
    );

WINBASEAPI
DWORD
WINAPI
GetLastError(
    VOID
    );

WINBASEAPI
VOID
WINAPI
SetLastError(
    IN DWORD dwErrCode
    );

#define HasOverlappedIoCompleted(lpOverlapped) ((lpOverlapped)->Internal != STATUS_PENDING)

WINBASEAPI
BOOL
WINAPI
GetOverlappedResult(
    IN HANDLE hFile,
    IN LPOVERLAPPED lpOverlapped,
    OUT LPDWORD lpNumberOfBytesTransferred,
    IN BOOL bWait
    );

WINBASEAPI
HANDLE
WINAPI
CreateIoCompletionPort(
    IN HANDLE FileHandle,
    IN HANDLE ExistingCompletionPort,
    IN ULONG_PTR CompletionKey,
    IN DWORD NumberOfConcurrentThreads
    );

WINBASEAPI
BOOL
WINAPI
GetQueuedCompletionStatus(
    IN  HANDLE CompletionPort,
    OUT LPDWORD lpNumberOfBytesTransferred,
    OUT PULONG_PTR lpCompletionKey,
    OUT LPOVERLAPPED *lpOverlapped,
    IN  DWORD dwMilliseconds
    );

WINBASEAPI
BOOL
WINAPI
PostQueuedCompletionStatus(
    IN HANDLE CompletionPort,
    IN DWORD dwNumberOfBytesTransferred,
    IN ULONG_PTR dwCompletionKey,
    IN LPOVERLAPPED lpOverlapped
    );

;begin_noxbox

#define SEM_FAILCRITICALERRORS      0x0001
#define SEM_NOGPFAULTERRORBOX       0x0002
#define SEM_NOALIGNMENTFAULTEXCEPT  0x0004
#define SEM_NOOPENFILEERRORBOX      0x8000

WINBASEAPI
UINT
WINAPI
SetErrorMode(
    IN UINT uMode
    );

#if !defined(MIDL_PASS)
WINBASEAPI
BOOL
WINAPI
GetThreadContext(
    IN HANDLE hThread,
    IN OUT LPCONTEXT lpContext
    );

WINBASEAPI
BOOL
WINAPI
SetThreadContext(
    IN HANDLE hThread,
    IN CONST CONTEXT *lpContext
    );
#endif

;end_noxbox

WINBASEAPI
DWORD
WINAPI
SuspendThread(
    IN HANDLE hThread
    );

WINBASEAPI
DWORD
WINAPI
ResumeThread(
    IN HANDLE hThread
    );


typedef
VOID
(APIENTRY *PAPCFUNC)(
    ULONG_PTR dwParam
    );

WINBASEAPI
DWORD
WINAPI
QueueUserAPC(
    IN PAPCFUNC pfnAPC,
    IN HANDLE hThread,
    IN ULONG_PTR dwData
    );


;begin_noxbox

WINBASEAPI
BOOL
WINAPI
IsDebuggerPresent(
    VOID
    );

;end_noxbox

WINBASEAPI
VOID
WINAPI
DebugBreak(
    VOID
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
WaitForDebugEvent(
    IN LPDEBUG_EVENT lpDebugEvent,
    IN DWORD dwMilliseconds
    );

WINBASEAPI
BOOL
WINAPI
ContinueDebugEvent(
    IN DWORD dwProcessId,
    IN DWORD dwThreadId,
    IN DWORD dwContinueStatus
    );

WINBASEAPI
BOOL
WINAPI
DebugActiveProcess(
    IN DWORD dwProcessId
    );

WINBASEAPI
VOID
WINAPI
InitializeCriticalSection(
    OUT LPCRITICAL_SECTION lpCriticalSection
    );

#if !defined(_NTRTL_)

WINBASEAPI
VOID
WINAPI
EnterCriticalSection(
    IN OUT LPCRITICAL_SECTION lpCriticalSection
    );

WINBASEAPI
VOID
WINAPI
LeaveCriticalSection(
    IN OUT LPCRITICAL_SECTION lpCriticalSection
    );

WINBASEAPI
BOOL
WINAPI
TryEnterCriticalSection(
    IN OUT LPCRITICAL_SECTION lpCriticalSection
    );

WINBASEAPI
VOID
WINAPI
DeleteCriticalSection(
    IN OUT LPCRITICAL_SECTION lpCriticalSection
    );

#endif // !defined(_NTRTL_)

;end_noxbox

WINBASEAPI
BOOL
WINAPI
SetEvent(
    IN HANDLE hEvent
    );

WINBASEAPI
BOOL
WINAPI
ResetEvent(
    IN HANDLE hEvent
    );

WINBASEAPI
BOOL
WINAPI
PulseEvent(
    IN HANDLE hEvent
    );

WINBASEAPI
BOOL
WINAPI
ReleaseSemaphore(
    IN HANDLE hSemaphore,
    IN LONG lReleaseCount,
    OUT LPLONG lpPreviousCount
    );

WINBASEAPI
BOOL
WINAPI
ReleaseMutex(
    IN HANDLE hMutex
    );

WINBASEAPI
DWORD
WINAPI
WaitForSingleObject(
    IN HANDLE hHandle,
    IN DWORD dwMilliseconds
    );

WINBASEAPI
DWORD
WINAPI
WaitForMultipleObjects(
    IN DWORD nCount,
    IN CONST HANDLE *lpHandles,
    IN BOOL bWaitAll,
    IN DWORD dwMilliseconds
    );

WINBASEAPI
VOID
WINAPI
Sleep(
    IN DWORD dwMilliseconds
    );

;begin_noxbox

WINBASEAPI
HGLOBAL
WINAPI
LoadResource(
    IN HMODULE hModule,   ;public_NT
    IN HINSTANCE hModule, ;public_chicago
    IN HRSRC hResInfo
    );

WINBASEAPI
DWORD
WINAPI
SizeofResource(
    IN HMODULE hModule,   ;public_NT
    IN HINSTANCE hModule, ;public_chicago
    IN HRSRC hResInfo
    );

WINBASEAPI
DWORD
WINAPI
GetLogicalDrives(
    VOID
    );

;end_noxbox

typedef struct _BY_HANDLE_FILE_INFORMATION {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

WINBASEAPI
BOOL
WINAPI
GetFileInformationByHandle(
    IN HANDLE hFile,
    OUT LPBY_HANDLE_FILE_INFORMATION lpFileInformation
    );

;begin_noxbox

WINBASEAPI
DWORD
WINAPI
GetFileType(
    IN HANDLE hFile
    );

;end_noxbox

WINBASEAPI
DWORD
WINAPI
GetFileSize(
    IN HANDLE hFile,
    OUT LPDWORD lpFileSizeHigh
    );

WINBASEAPI
BOOL
WINAPI
GetFileSizeEx(
    HANDLE hFile,
    PLARGE_INTEGER lpFileSize
    );

WINBASEAPI
BOOL
WINAPI
WriteFile(
    IN HANDLE hFile,
    IN LPCVOID lpBuffer,
    IN DWORD nNumberOfBytesToWrite,
    OUT LPDWORD lpNumberOfBytesWritten,
    IN LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
ReadFile(
    IN HANDLE hFile,
    OUT LPVOID lpBuffer,
    IN DWORD nNumberOfBytesToRead,
    OUT LPDWORD lpNumberOfBytesRead,
    IN LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
FlushFileBuffers(
    IN HANDLE hFile
    );

WINBASEAPI
BOOL
WINAPI
DeviceIoControl(
    IN HANDLE hDevice,
    IN DWORD dwIoControlCode,
    IN LPVOID lpInBuffer,
    IN DWORD nInBufferSize,
    OUT LPVOID lpOutBuffer,
    IN DWORD nOutBufferSize,
    OUT LPDWORD lpBytesReturned,
    IN LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
SetEndOfFile(
    IN HANDLE hFile
    );

WINBASEAPI
DWORD
WINAPI
SetFilePointer(
    IN HANDLE hFile,
    IN LONG lDistanceToMove,
    IN PLONG lpDistanceToMoveHigh,
    IN DWORD dwMoveMethod
    );

WINBASEAPI
BOOL
WINAPI
SetFilePointerEx(
    HANDLE hFile,
    LARGE_INTEGER liDistanceToMove,
    PLARGE_INTEGER lpNewFilePointer,
    DWORD dwMoveMethod
    );

#define FindClose(hFindFile) CloseHandle(hFindFile)

WINBASEAPI
BOOL
WINAPI
GetFileTime(
    IN HANDLE hFile,
    OUT LPFILETIME lpCreationTime,
    OUT LPFILETIME lpLastAccessTime,
    OUT LPFILETIME lpLastWriteTime
    );

WINBASEAPI
BOOL
WINAPI
SetFileTime(
    IN HANDLE hFile,
    IN CONST FILETIME *lpCreationTime,
    IN CONST FILETIME *lpLastAccessTime,
    IN CONST FILETIME *lpLastWriteTime
    );

WINBASEAPI
BOOL
WINAPI
CloseHandle(
    IN OUT HANDLE hObject
    );

WINBASEAPI
BOOL
WINAPI
DuplicateHandle(
    IN HANDLE hSourceProcessHandle,
    IN HANDLE hSourceHandle,
    IN HANDLE hTargetProcessHandle,
    OUT LPHANDLE lpTargetHandle,
    IN DWORD dwDesiredAccess,
    IN BOOL bInheritHandle,
    IN DWORD dwOptions
    );

;begin_noxbox

#define HINSTANCE_ERROR 32

WINBASEAPI
DWORD
WINAPI
LoadModule(
    IN LPCSTR lpModuleName,
    IN LPVOID lpParameterBlock
    );

;end_noxbox

WINBASEAPI
int
WINAPI
MulDiv(
    IN int nNumber,
    IN int nNumerator,
    IN int nDenominator
    );

WINBASEAPI
VOID
WINAPI
GetSystemTime(
    OUT LPSYSTEMTIME lpSystemTime
    );

WINBASEAPI
VOID
WINAPI
GetSystemTimeAsFileTime(
    OUT LPFILETIME lpSystemTimeAsFileTime
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
SetSystemTime(
    IN CONST SYSTEMTIME *lpSystemTime
    );

;end_noxbox

WINBASEAPI
VOID
WINAPI
GetLocalTime(
    OUT LPSYSTEMTIME lpSystemTime
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
SetLocalTime(
    IN CONST SYSTEMTIME *lpSystemTime
    );

WINBASEAPI
VOID
WINAPI
GetSystemInfo(
    OUT LPSYSTEM_INFO lpSystemInfo
    );

;end_noxbox

typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
SystemTimeToTzSpecificLocalTime(
    IN LPTIME_ZONE_INFORMATION lpTimeZoneInformation,
    IN LPSYSTEMTIME lpUniversalTime,
    OUT LPSYSTEMTIME lpLocalTime
    );

;end_noxbox

WINBASEAPI
DWORD
WINAPI
GetTimeZoneInformation(
    OUT LPTIME_ZONE_INFORMATION lpTimeZoneInformation
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
SetTimeZoneInformation(
    IN CONST TIME_ZONE_INFORMATION *lpTimeZoneInformation
    );

;end_noxbox


//
// Routines to convert back and forth between system time and file time
//

WINBASEAPI
BOOL
WINAPI
SystemTimeToFileTime(
    IN CONST SYSTEMTIME *lpSystemTime,
    OUT LPFILETIME lpFileTime
    );

WINBASEAPI
BOOL
WINAPI
FileTimeToLocalFileTime(
    IN CONST FILETIME *lpFileTime,
    OUT LPFILETIME lpLocalFileTime
    );

WINBASEAPI
BOOL
WINAPI
LocalFileTimeToFileTime(
    IN CONST FILETIME *lpLocalFileTime,
    OUT LPFILETIME lpFileTime
    );

WINBASEAPI
BOOL
WINAPI
FileTimeToSystemTime(
    IN CONST FILETIME *lpFileTime,
    OUT LPSYSTEMTIME lpSystemTime
    );

WINBASEAPI
LONG
WINAPI
CompareFileTime(
    IN CONST FILETIME *lpFileTime1,
    IN CONST FILETIME *lpFileTime2
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
FileTimeToDosDateTime(
    IN CONST FILETIME *lpFileTime,
    OUT LPWORD lpFatDate,
    OUT LPWORD lpFatTime
    );

WINBASEAPI
BOOL
WINAPI
DosDateTimeToFileTime(
    IN WORD wFatDate,
    IN WORD wFatTime,
    OUT LPFILETIME lpFileTime
    );

;end_noxbox

WINBASEAPI
DWORD
WINAPI
GetTickCount(
    VOID
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
SetSystemTimeAdjustment(
    IN DWORD dwTimeAdjustment,
    IN BOOL  bTimeAdjustmentDisabled
    );

WINBASEAPI
BOOL
WINAPI
GetSystemTimeAdjustment(
    OUT PDWORD lpTimeAdjustment,
    OUT PDWORD lpTimeIncrement,
    OUT PBOOL  lpTimeAdjustmentDisabled
    );

#if !defined(MIDL_PASS)
WINBASEAPI
DWORD
WINAPI
FormatMessage%(
    IN DWORD dwFlags,
    IN LPCVOID lpSource,
    IN DWORD dwMessageId,
    IN DWORD dwLanguageId,
    OUT LPTSTR% lpBuffer,
    IN DWORD nSize,
    IN va_list *Arguments
    );
#endif

#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define FORMAT_MESSAGE_FROM_STRING     0x00000400
#define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY  0x00002000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK  0x000000FF

;end_noxbox

//
// _l Compat Functions
//

WINBASEAPI
int
WINAPI
lstrcmp%(
    IN LPCTSTR% lpString1,
    IN LPCTSTR% lpString2
    );

WINBASEAPI
int
WINAPI
lstrcmpi%(
    IN LPCTSTR% lpString1,
    IN LPCTSTR% lpString2
    );

WINBASEAPI
LPTSTR%
WINAPI
lstrcpyn%(
    OUT LPTSTR% lpString1,
    IN LPCTSTR% lpString2,
    IN int iMaxLength
    );

WINBASEAPI
LPTSTR%
WINAPI
lstrcpy%(
    OUT LPTSTR% lpString1,
    IN LPCTSTR% lpString2
    );

WINBASEAPI
LPTSTR%
WINAPI
lstrcat%(
    IN OUT LPTSTR% lpString1,
    IN LPCTSTR% lpString2
    );

WINBASEAPI
int
WINAPI
lstrlen%(
    IN LPCTSTR% lpString
    );

;begin_noxbox

WINBASEAPI
HFILE
WINAPI
OpenFile(
    IN LPCSTR lpFileName,
    OUT LPOFSTRUCT lpReOpenBuff,
    IN UINT uStyle
    );

WINBASEAPI
HFILE
WINAPI
_lopen(
    IN LPCSTR lpPathName,
    IN int iReadWrite
    );

WINBASEAPI
HFILE
WINAPI
_lcreat(
    IN LPCSTR lpPathName,
    IN int  iAttribute
    );

WINBASEAPI
UINT
WINAPI
_lread(
    IN HFILE hFile,
    OUT LPVOID lpBuffer,
    IN UINT uBytes
    );

WINBASEAPI
UINT
WINAPI
_lwrite(
    IN HFILE hFile,
    IN LPCSTR lpBuffer,
    IN UINT uBytes
    );

WINBASEAPI
long
WINAPI
_hread(
    IN HFILE hFile,
    OUT LPVOID lpBuffer,
    IN long lBytes
    );

WINBASEAPI
long
WINAPI
_hwrite(
    IN HFILE hFile,
    IN LPCSTR lpBuffer,
    IN long lBytes
    );

WINBASEAPI
HFILE
WINAPI
_lclose(
    IN OUT HFILE hFile
    );

WINBASEAPI
LONG
WINAPI
_llseek(
    IN HFILE hFile,
    IN LONG lOffset,
    IN int iOrigin
    );

WINADVAPI
BOOL
WINAPI
IsTextUnicode(
    IN CONST LPVOID lpBuffer,
    IN int cb,
    IN OUT LPINT lpi
    );

;end_noxbox

WINBASEAPI
DWORD
WINAPI
TlsAlloc(
    VOID
    );

#define TLS_OUT_OF_INDEXES (DWORD)0xFFFFFFFF

WINBASEAPI
LPVOID
WINAPI
TlsGetValue(
    IN DWORD dwTlsIndex
    );

WINBASEAPI
BOOL
WINAPI
TlsSetValue(
    IN DWORD dwTlsIndex,
    IN LPVOID lpTlsValue
    );

WINBASEAPI
BOOL
WINAPI
TlsFree(
    IN DWORD dwTlsIndex
    );

typedef
VOID
(WINAPI *LPOVERLAPPED_COMPLETION_ROUTINE)(
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
DWORD
WINAPI
SleepEx(
    IN DWORD dwMilliseconds,
    IN BOOL bAlertable
    );

WINBASEAPI
DWORD
WINAPI
WaitForSingleObjectEx(
    IN HANDLE hHandle,
    IN DWORD dwMilliseconds,
    IN BOOL bAlertable
    );

WINBASEAPI
DWORD
WINAPI
WaitForMultipleObjectsEx(
    IN DWORD nCount,
    IN CONST HANDLE *lpHandles,
    IN BOOL bWaitAll,
    IN DWORD dwMilliseconds,
    IN BOOL bAlertable
    );

WINBASEAPI
DWORD
WINAPI
SignalObjectAndWait(
    IN HANDLE hObjectToSignal,
    IN HANDLE hObjectToWaitOn,
    IN DWORD dwMilliseconds,
    IN BOOL bAlertable
    );

WINBASEAPI
BOOL
WINAPI
ReadFileEx(
    IN HANDLE hFile,
    OUT LPVOID lpBuffer,
    IN DWORD nNumberOfBytesToRead,
    IN LPOVERLAPPED lpOverlapped,
    IN LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );

WINBASEAPI
BOOL
WINAPI
WriteFileEx(
    IN HANDLE hFile,
    IN LPCVOID lpBuffer,
    IN DWORD nNumberOfBytesToWrite,
    IN LPOVERLAPPED lpOverlapped,
    IN LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );

WINBASEAPI
BOOL
WINAPI
ReadFileScatter(
    IN HANDLE hFile,
    IN FILE_SEGMENT_ELEMENT aSegmentArray[],
    IN DWORD nNumberOfBytesToRead,
    OUT LPDWORD lpNumberOfBytesRead,
    IN LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
WriteFileGather(
    IN HANDLE hFile,
    OUT FILE_SEGMENT_ELEMENT aSegmentArray[],
    IN DWORD nNumberOfBytesToWrite,
    OUT LPDWORD lpNumberOfBytesWritten,
    IN LPOVERLAPPED lpOverlapped
    );

//
// Dual Mode API below this line. Dual Mode Structures also included.
//

typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[ MAX_PATH ];
    CHAR cAlternateFileName[ 14 ];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
typedef WIN32_FIND_DATAA WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAA PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAA LPWIN32_FIND_DATA;

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

WINBASEAPI
HANDLE
WINAPI
CreateMutexA(
    IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
    IN BOOL bInitialOwner,
    IN LPCSTR lpName
    );
#define CreateMutex CreateMutexA

WINBASEAPI
HANDLE
WINAPI
OpenMutexA(
    IN DWORD dwDesiredAccess,
    IN BOOL bInheritHandle,
    IN LPCSTR lpName
    );
#define OpenMutex OpenMutexA

WINBASEAPI
HANDLE
WINAPI
CreateEventA(
    IN LPSECURITY_ATTRIBUTES lpEventAttributes,
    IN BOOL bManualReset,
    IN BOOL bInitialState,
    IN LPCSTR lpName
    );
#define CreateEvent CreateEventA

WINBASEAPI
HANDLE
WINAPI
OpenEventA(
    IN DWORD dwDesiredAccess,
    IN BOOL bInheritHandle,
    IN LPCSTR lpName
    );
#define OpenEvent OpenEventA

WINBASEAPI
HANDLE
WINAPI
CreateSemaphoreA(
    IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    IN LONG lInitialCount,
    IN LONG lMaximumCount,
    IN LPCSTR lpName
    );
#define CreateSemaphore CreateSemaphoreA

WINBASEAPI
HANDLE
WINAPI
OpenSemaphoreA(
    IN DWORD dwDesiredAccess,
    IN BOOL bInheritHandle,
    IN LPCSTR lpName
    );
#define OpenSemaphore OpenSemaphoreA

typedef
VOID
(APIENTRY *PTIMERAPCROUTINE)(
    LPVOID lpArgToCompletionRoutine,
    DWORD dwTimerLowValue,
    DWORD dwTimerHighValue
    );

WINBASEAPI
HANDLE
WINAPI
CreateWaitableTimerA(
    IN LPSECURITY_ATTRIBUTES lpTimerAttributes,
    IN BOOL bManualReset,
    IN LPCSTR lpTimerName
    );
#define CreateWaitableTimer CreateWaitableTimerA

WINBASEAPI
HANDLE
WINAPI
OpenWaitableTimerA(
    IN DWORD dwDesiredAccess,
    IN BOOL bInheritHandle,
    IN LPCSTR lpTimerName
    );
#define OpenWaitableTimer OpenWaitableTimerA

WINBASEAPI
BOOL
WINAPI
SetWaitableTimer(
    IN HANDLE hTimer,
    IN const LARGE_INTEGER *lpDueTime,
    IN LONG lPeriod,
    IN PTIMERAPCROUTINE pfnCompletionRoutine,
    IN LPVOID lpArgToCompletionRoutine,
    IN BOOL fResume
    );

WINBASEAPI
BOOL
WINAPI
CancelWaitableTimer(
    IN HANDLE hTimer
    );

;begin_noxbox

WINBASEAPI
DWORD
WINAPI
GetLogicalDriveStrings%(
    IN DWORD nBufferLength,
    OUT LPTSTR% lpBuffer
    );

WINBASEAPI
HMODULE    ;public_NT
HINSTANCE  ;public_chicago
WINAPI
LoadLibrary%(
    IN LPCTSTR% lpLibFileName
    );

WINBASEAPI
HMODULE    ;public_NT
HINSTANCE  ;public_chicago
WINAPI
LoadLibraryEx%(
    IN LPCTSTR% lpLibFileName,
    IN HANDLE hFile,
    IN DWORD dwFlags
    );

#define DONT_RESOLVE_DLL_REFERENCES     0x00000001
#define LOAD_LIBRARY_AS_DATAFILE        0x00000002
#define LOAD_WITH_ALTERED_SEARCH_PATH   0x00000008

WINBASEAPI
DWORD
WINAPI
GetModuleFileName%(
    IN HMODULE hModule,    ;public_NT
    IN HINSTANCE hModule,  ;public_chicago
    OUT LPTSTR% lpFilename,
    IN DWORD nSize
    );

WINBASEAPI
HMODULE
WINAPI
GetModuleHandle%(
    IN LPCTSTR% lpModuleName
    );

;end_noxbox

WINBASEAPI
VOID
WINAPI
OutputDebugString%(
    IN LPCTSTR% lpOutputString
    );

;begin_noxbox

WINBASEAPI
HRSRC
WINAPI
FindResource%(
    IN HMODULE hModule,    ;public_NT
    IN HINSTANCE hModule,  ;public_chicago
    IN LPCTSTR% lpName,
    IN LPCTSTR% lpType
    );

WINBASEAPI
HRSRC
WINAPI
FindResourceEx%(
    IN HMODULE hModule,    ;public_NT
    IN HINSTANCE hModule,  ;public_chicago
    IN LPCTSTR% lpType,
    IN LPCTSTR% lpName,
    IN WORD    wLanguage
    );

#ifdef STRICT
typedef BOOL (CALLBACK* ENUMRESTYPEPROC%)(HMODULE hModule, LPTSTR% lpType,     ;public_NT
typedef BOOL (CALLBACK* ENUMRESTYPEPROC)(HINSTANCE hModule, LPTSTR% lpType,   ;public_chicago
        LONG_PTR lParam);
typedef BOOL (CALLBACK* ENUMRESNAMEPROC%)(HMODULE hModule, LPCTSTR% lpType,    ;public_NT
typedef BOOL (CALLBACK* ENUMRESNAMEPROC)(HINSTANCE hModule, LPCTSTR% lpType,  ;public_chicago
        LPTSTR% lpName, LONG_PTR lParam);
typedef BOOL (CALLBACK* ENUMRESLANGPROC%)(HMODULE hModule, LPCTSTR% lpType,    ;public_NT
typedef BOOL (CALLBACK* ENUMRESLANGPROC)(HINSTANCE hModule, LPCTSTR% lpType,  ;public_chicago
        LPCTSTR% lpName, WORD  wLanguage, LONG_PTR lParam);
#else
typedef FARPROC ENUMRESTYPEPROC%;
typedef FARPROC ENUMRESNAMEPROC%;
typedef FARPROC ENUMRESLANGPROC%;
#endif

WINBASEAPI
BOOL
WINAPI
EnumResourceTypes%(
    IN HMODULE hModule,    ;public_NT
    IN HINSTANCE hModule,  ;public_chicago
    IN ENUMRESTYPEPROC% lpEnumFunc,
    IN LONG_PTR lParam
    );


WINBASEAPI
BOOL
WINAPI
EnumResourceNames%(
    IN HMODULE hModule,    ;public_NT
    IN HINSTANCE hModule,  ;public_chicago
    IN LPCTSTR% lpType,
    IN ENUMRESNAMEPROC% lpEnumFunc,
    IN LONG_PTR lParam
    );

WINBASEAPI
BOOL
WINAPI
EnumResourceLanguages%(
    IN HMODULE hModule,    ;public_NT
    IN HINSTANCE hModule,  ;public_chicago
    IN LPCTSTR% lpType,
    IN LPCTSTR% lpName,
    IN ENUMRESLANGPROC% lpEnumFunc,
    IN LONG_PTR lParam
    );

WINBASEAPI
UINT
WINAPI
GetProfileInt%(
    IN LPCTSTR% lpAppName,
    IN LPCTSTR% lpKeyName,
    IN INT nDefault
    );

WINBASEAPI
DWORD
WINAPI
GetProfileString%(
    IN LPCTSTR% lpAppName,
    IN LPCTSTR% lpKeyName,
    IN LPCTSTR% lpDefault,
    OUT LPTSTR% lpReturnedString,
    IN DWORD nSize
    );

WINBASEAPI
BOOL
WINAPI
WriteProfileString%(
    IN LPCTSTR% lpAppName,
    IN LPCTSTR% lpKeyName,
    IN LPCTSTR% lpString
    );

WINBASEAPI
DWORD
WINAPI
GetProfileSection%(
    IN LPCTSTR% lpAppName,
    OUT LPTSTR% lpReturnedString,
    IN DWORD nSize
    );

WINBASEAPI
BOOL
WINAPI
WriteProfileSection%(
    IN LPCTSTR% lpAppName,
    IN LPCTSTR% lpString
    );

WINBASEAPI
UINT
WINAPI
GetPrivateProfileInt%(
    IN LPCTSTR% lpAppName,
    IN LPCTSTR% lpKeyName,
    IN INT nDefault,
    IN LPCTSTR% lpFileName
    );

WINBASEAPI
DWORD
WINAPI
GetPrivateProfileString%(
    IN LPCTSTR% lpAppName,
    IN LPCTSTR% lpKeyName,
    IN LPCTSTR% lpDefault,
    OUT LPTSTR% lpReturnedString,
    IN DWORD nSize,
    IN LPCTSTR% lpFileName
    );

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileString%(
    IN LPCTSTR% lpAppName,
    IN LPCTSTR% lpKeyName,
    IN LPCTSTR% lpString,
    IN LPCTSTR% lpFileName
    );

WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSection%(
    IN LPCTSTR% lpAppName,
    OUT LPTSTR% lpReturnedString,
    IN DWORD nSize,
    IN LPCTSTR% lpFileName
    );

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileSection%(
    IN LPCTSTR% lpAppName,
    IN LPCTSTR% lpString,
    IN LPCTSTR% lpFileName
    );


WINBASEAPI
DWORD
WINAPI
GetPrivateProfileSectionNames%(
    OUT LPTSTR% lpszReturnBuffer,
    IN DWORD nSize,
    IN LPCTSTR% lpFileName
    );

WINBASEAPI
BOOL
WINAPI
GetPrivateProfileStruct%(
    IN LPCTSTR% lpszSection,
    IN LPCTSTR% lpszKey,
    OUT LPVOID   lpStruct,
    IN UINT     uSizeStruct,
    IN LPCTSTR% szFile
    );

WINBASEAPI
BOOL
WINAPI
WritePrivateProfileStruct%(
    IN LPCTSTR% lpszSection,
    IN LPCTSTR% lpszKey,
    IN LPVOID   lpStruct,
    IN UINT     uSizeStruct,
    IN LPCTSTR% szFile
    );

WINBASEAPI
UINT
WINAPI
GetDriveType%(
    IN LPCTSTR% lpRootPathName
    );

WINBASEAPI
UINT
WINAPI
GetSystemDirectory%(
    OUT LPTSTR% lpBuffer,
    IN UINT uSize
    );

WINBASEAPI
DWORD
WINAPI
GetTempPath%(
    IN DWORD nBufferLength,
    OUT LPTSTR% lpBuffer
    );

WINBASEAPI
UINT
WINAPI
GetTempFileName%(
    IN LPCTSTR% lpPathName,
    IN LPCTSTR% lpPrefixString,
    IN UINT uUnique,
    OUT LPTSTR% lpTempFileName
    );

WINBASEAPI
BOOL
WINAPI
SetCurrentDirectory%(
    IN LPCTSTR% lpPathName
    );

WINBASEAPI
DWORD
WINAPI
GetCurrentDirectory%(
    IN DWORD nBufferLength,
    OUT LPTSTR% lpBuffer
    );

WINBASEAPI
BOOL
WINAPI
GetDiskFreeSpace%(
    IN LPCTSTR% lpRootPathName,
    OUT LPDWORD lpSectorsPerCluster,
    OUT LPDWORD lpBytesPerSector,
    OUT LPDWORD lpNumberOfFreeClusters,
    OUT LPDWORD lpTotalNumberOfClusters
    );

;end_noxbox

WINBASEAPI
BOOL
WINAPI
GetDiskFreeSpaceExA(
    IN LPCSTR lpDirectoryName,
    OUT PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    OUT PULARGE_INTEGER lpTotalNumberOfBytes,
    OUT PULARGE_INTEGER lpTotalNumberOfFreeBytes
    );
#define GetDiskFreeSpaceEx GetDiskFreeSpaceExA

WINBASEAPI
BOOL
WINAPI
CreateDirectoryA(
    IN LPCSTR lpPathName,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#define CreateDirectory CreateDirectoryA

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
CreateDirectoryEx%(
    IN LPCTSTR% lpTemplateDirectory,
    IN LPCTSTR% lpNewDirectory,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

;end_noxbox

WINBASEAPI
BOOL
WINAPI
RemoveDirectoryA(
    IN LPCSTR lpPathName
    );
#define RemoveDirectory RemoveDirectoryA

;begin_noxbox

WINBASEAPI
DWORD
WINAPI
GetFullPathName%(
    IN LPCTSTR% lpFileName,
    IN DWORD nBufferLength,
    OUT LPTSTR% lpBuffer,
    OUT LPTSTR% *lpFilePart
    );

;end_noxbox

WINBASEAPI
HANDLE
WINAPI
CreateFileA(
    IN LPCSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
    );
#define CreateFile CreateFileA

WINBASEAPI
BOOL
WINAPI
SetFileAttributesA(
    IN LPCSTR lpFileName,
    IN DWORD dwFileAttributes
    );
#define SetFileAttributes SetFileAttributesA

WINBASEAPI
DWORD
WINAPI
GetFileAttributesA(
    IN LPCSTR lpFileName
    );
#define GetFileAttributes GetFileAttributesA

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

WINBASEAPI
BOOL
WINAPI
GetFileAttributesExA(
    IN LPCSTR lpFileName,
    IN GET_FILEEX_INFO_LEVELS fInfoLevelId,
    OUT LPVOID lpFileInformation
    );
#define GetFileAttributesEx GetFileAttributesExA

WINBASEAPI
BOOL
WINAPI
DeleteFileA(
    IN LPCSTR lpFileName
    );
#define DeleteFile DeleteFileA

WINBASEAPI
HANDLE
WINAPI
FindFirstFileA(
    IN LPCSTR lpFileName,
    OUT LPWIN32_FIND_DATA lpFindFileData
    );
#define FindFirstFile FindFirstFileA

WINBASEAPI
BOOL
WINAPI
FindNextFileA(
    IN HANDLE hFindFile,
    OUT LPWIN32_FIND_DATAA lpFindFileData
    );
#define FindNextFile FindNextFileA

;begin_noxbox

WINBASEAPI
DWORD
WINAPI
SearchPath%(
    IN LPCTSTR% lpPath,
    IN LPCTSTR% lpFileName,
    IN LPCTSTR% lpExtension,
    IN DWORD nBufferLength,
    OUT LPTSTR% lpBuffer,
    OUT LPTSTR% *lpFilePart
    );

;end_noxbox

WINBASEAPI
BOOL
WINAPI
CopyFileA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN BOOL bFailIfExists
    );
#define CopyFile CopyFileA

typedef
DWORD
(WINAPI *LPPROGRESS_ROUTINE)(
    LARGE_INTEGER TotalFileSize,
    LARGE_INTEGER TotalBytesTransferred,
    LARGE_INTEGER StreamSize,
    LARGE_INTEGER StreamBytesTransferred,
    DWORD dwStreamNumber,
    DWORD dwCallbackReason,
    HANDLE hSourceFile,
    HANDLE hDestinationFile,
    LPVOID lpData OPTIONAL
    );

WINBASEAPI
BOOL
WINAPI
CopyFileExA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN LPBOOL pbCancel OPTIONAL,
    IN DWORD dwCopyFlags
    );
#define CopyFileEx CopyFileExA

WINBASEAPI
BOOL
WINAPI
MoveFileA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName
    );
#define MoveFile MoveFileA

WINBASEAPI
BOOL
WINAPI
MoveFileExA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN DWORD dwFlags
    );
#define MoveFileEx MoveFileExA

WINBASEAPI
BOOL
WINAPI
MoveFileWithProgressA(
    IN LPCSTR lpExistingFileName,
    IN LPCSTR lpNewFileName,
    IN LPPROGRESS_ROUTINE lpProgressRoutine OPTIONAL,
    IN LPVOID lpData OPTIONAL,
    IN DWORD dwFlags
    );
#define MoveFileWithProgress MoveFileWithProgressA

#define MOVEFILE_REPLACE_EXISTING       0x00000001
#define MOVEFILE_COPY_ALLOWED           0x00000002
#define MOVEFILE_WRITE_THROUGH          0x00000008

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
SetVolumeLabel%(
    IN LPCTSTR% lpRootPathName,
    IN LPCTSTR% lpVolumeName
    );

WINBASEAPI
VOID
WINAPI
SetFileApisToOEM( VOID );

WINBASEAPI
VOID
WINAPI
SetFileApisToANSI( VOID );

WINBASEAPI
BOOL
WINAPI
AreFileApisANSI( VOID );

;end_noxbox

WINBASEAPI
BOOL
WINAPI
GetVolumeInformationA(
    IN LPCSTR lpRootPathName,
    OUT LPSTR lpVolumeNameBuffer,
    IN DWORD nVolumeNameSize,
    OUT LPDWORD lpVolumeSerialNumber,
    OUT LPDWORD lpMaximumComponentLength,
    OUT LPDWORD lpFileSystemFlags,
    OUT LPSTR lpFileSystemNameBuffer,
    IN DWORD nFileSystemNameSize
    );
#define GetVolumeInformation GetVolumeInformationA

WINBASEAPI
BOOL
WINAPI
CancelIo(
    IN HANDLE hFile
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
SetPriorityClass(
    IN HANDLE hProcess,
    IN DWORD dwPriorityClass
    );

WINBASEAPI
DWORD
WINAPI
GetPriorityClass(
    IN HANDLE hProcess
    );

;end_noxbox

WINBASEAPI
BOOL
WINAPI
IsBadReadPtr(
    IN CONST VOID *lp,
    IN UINT_PTR ucb
    );

WINBASEAPI
BOOL
WINAPI
IsBadWritePtr(
    IN LPVOID lp,
    IN UINT_PTR ucb
    );

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
IsBadHugeReadPtr(
    IN CONST VOID *lp,
    IN UINT_PTR ucb
    );

WINBASEAPI
BOOL
WINAPI
IsBadHugeWritePtr(
    IN LPVOID lp,
    IN UINT_PTR ucb
    );

;end_noxbox

WINBASEAPI
BOOL
WINAPI
IsBadCodePtr(
    IN FARPROC lpfn
    );

WINBASEAPI
BOOL
WINAPI
IsBadStringPtr%(
    IN LPCTSTR% lpsz,
    IN UINT_PTR ucchMax
    );

;begin_noxbox

#define MAX_COMPUTERNAME_LENGTH 15

WINBASEAPI
BOOL
WINAPI
GetComputerName% (
    OUT LPTSTR% lpBuffer,
    IN OUT LPDWORD nSize
    );

WINBASEAPI
BOOL
WINAPI
SetComputerName% (
    IN LPCTSTR% lpComputerName
    );



typedef enum _COMPUTER_NAME_FORMAT {
    ComputerNameNetBIOS,
    ComputerNameDnsHostname,
    ComputerNameDnsDomain,
    ComputerNameDnsFullyQualified,
    ComputerNamePhysicalNetBIOS,
    ComputerNamePhysicalDnsHostname,
    ComputerNamePhysicalDnsDomain,
    ComputerNamePhysicalDnsFullyQualified,
    ComputerNameMax
} COMPUTER_NAME_FORMAT ;

WINBASEAPI
BOOL
WINAPI
GetComputerNameEx% (
    IN COMPUTER_NAME_FORMAT NameType,
    OUT LPTSTR% lpBuffer,
    IN OUT LPDWORD nSize
    );

WINBASEAPI
BOOL
WINAPI
SetComputerNameEx% (
    IN COMPUTER_NAME_FORMAT NameType,
    IN LPCTSTR% lpBuffer
    );

WINBASEAPI
BOOL
WINAPI
DnsHostnameToComputerName% (
    IN LPTSTR% Hostname,
    OUT LPTSTR% ComputerName,
    IN OUT LPDWORD nSize
    );


WINADVAPI
BOOL
WINAPI
GetUserName% (
    OUT LPTSTR% lpBuffer,
    IN OUT LPDWORD nSize
    );

//
// Thread pool API's
//


typedef WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACK ;

WINBASEAPI
BOOL
WINAPI
RegisterWaitForSingleObject(
    PHANDLE phNewWaitObject,
    HANDLE hObject,
    WAITORTIMERCALLBACK Callback,
    PVOID Context,
    ULONG dwMilliseconds,
    ULONG dwFlags
    );

WINBASEAPI
HANDLE
WINAPI
RegisterWaitForSingleObjectEx(
    HANDLE hObject,
    WAITORTIMERCALLBACK Callback,
    PVOID Context,
    ULONG dwMilliseconds,
    ULONG dwFlags
    );

WINBASEAPI
BOOL
WINAPI
UnregisterWait(
    HANDLE WaitHandle
    );

WINBASEAPI
BOOL
WINAPI
UnregisterWaitEx(
    HANDLE WaitHandle,
    HANDLE CompletionEvent
    );

WINBASEAPI
BOOL
WINAPI
QueueUserWorkItem(
    LPTHREAD_START_ROUTINE Function,
    PVOID Context,
    ULONG Flags
    );

WINBASEAPI
BOOL
WINAPI
BindIoCompletionCallback (
    HANDLE FileHandle,
    LPOVERLAPPED_COMPLETION_ROUTINE Function,
    ULONG Flags
    );

WINBASEAPI
HANDLE
WINAPI
CreateTimerQueue(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
CreateTimerQueueTimer(
    PHANDLE phNewTimer,
    HANDLE TimerQueue,
    WAITORTIMERCALLBACK Callback,
    PVOID Parameter,
    DWORD DueTime,
    DWORD Period,
    ULONG Flags
    ) ;

WINBASEAPI
BOOL
WINAPI
ChangeTimerQueueTimer(
    HANDLE TimerQueue,
    HANDLE Timer,
    ULONG DueTime,
    ULONG Period
    );

WINBASEAPI
BOOL
WINAPI
DeleteTimerQueueTimer(
    HANDLE TimerQueue,
    HANDLE Timer,
    HANDLE CompletionEvent
    );

WINBASEAPI
BOOL
WINAPI
DeleteTimerQueueEx(
    HANDLE TimerQueue,
    HANDLE CompletionEvent
    );

WINBASEAPI
HANDLE
WINAPI
SetTimerQueueTimer(
    HANDLE TimerQueue,
    WAITORTIMERCALLBACK Callback,
    PVOID Parameter,
    DWORD DueTime,
    DWORD Period,
    BOOL PreferIo
    );

WINBASEAPI
BOOL
WINAPI
CancelTimerQueueTimer(
    HANDLE TimerQueue,
    HANDLE Timer
    );

WINBASEAPI
BOOL
WINAPI
DeleteTimerQueue(
    HANDLE TimerQueue
    );


;end_noxbox

//
// Performance counter API's
//

WINBASEAPI
BOOL
WINAPI
QueryPerformanceCounter(
    OUT LARGE_INTEGER *lpPerformanceCount
    );

WINBASEAPI
BOOL
WINAPI
QueryPerformanceFrequency(
    OUT LARGE_INTEGER *lpFrequency
    );

// DOS and OS/2 Compatible Error Code definitions returned by the Win32 Base
// API functions.
//

#include <winerror.h>

/* Abnormal termination codes */

#define TC_NORMAL       0
#define TC_HARDERR      1
#define TC_GP_TRAP      2
#define TC_SIGNAL       3

;begin_noxbox


//
// New Volume Mount Point API.
//

WINBASEAPI
HANDLE
WINAPI
FindFirstVolumeA(
    LPSTR lpszVolumeName,
    DWORD cchBufferLength
    );
WINBASEAPI
HANDLE
WINAPI
FindFirstVolumeW(
    LPWSTR lpszVolumeName,
    DWORD cchBufferLength
    );
#ifdef UNICODE
#define FindFirstVolume FindFirstVolumeW
#else
#define FindFirstVolume FindFirstVolumeA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindNextVolumeA(
    HANDLE hFindVolume,
    LPSTR lpszVolumeName,
    DWORD cchBufferLength
    );
WINBASEAPI
BOOL
WINAPI
FindNextVolumeW(
    HANDLE hFindVolume,
    LPWSTR lpszVolumeName,
    DWORD cchBufferLength
    );
#ifdef UNICODE
#define FindNextVolume FindNextVolumeW
#else
#define FindNextVolume FindNextVolumeA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindVolumeClose(
    HANDLE hFindVolume
    );

WINBASEAPI
BOOL
WINAPI
GetVolumePathNameA(
    LPCSTR lpszFileName,
    LPSTR lpszVolumePathName,
    DWORD cchBufferLength
    );
WINBASEAPI
BOOL
WINAPI
GetVolumePathNameW(
    LPCWSTR lpszFileName,
    LPWSTR lpszVolumePathName,
    DWORD cchBufferLength
    );
#ifdef UNICODE
#define GetVolumePathName GetVolumePathNameW
#else
#define GetVolumePathName GetVolumePathNameA
#endif // !UNICODE


;end_noxbox

;begin_internal

;begin_noxbox

typedef struct _VIRTUAL_BUFFER {
    PVOID Base;
    PVOID CommitLimit;
    PVOID ReserveLimit;
} VIRTUAL_BUFFER, *PVIRTUAL_BUFFER;

BOOLEAN
CreateVirtualBuffer(
    OUT PVIRTUAL_BUFFER Buffer,
    IN ULONG CommitSize OPTIONAL,
    IN ULONG ReserveSize OPTIONAL
    );

int
VirtualBufferExceptionHandler(
    IN ULONG ExceptionCode,
    IN PEXCEPTION_POINTERS ExceptionInfo,
    IN OUT PVIRTUAL_BUFFER Buffer
    );

BOOLEAN
ExtendVirtualBuffer(
    IN PVIRTUAL_BUFFER Buffer,
    IN PVOID Address
    );

BOOLEAN
TrimVirtualBuffer(
    IN PVIRTUAL_BUFFER Buffer
    );

BOOLEAN
FreeVirtualBuffer(
    IN PVIRTUAL_BUFFER Buffer
    );

;end_noxbox

//
// filefind stucture shared with ntvdm, jonle
// see mvdm\dos\dem\demsrch.c
//
typedef struct _FINDFILE_HANDLE {
    HANDLE DirectoryHandle;
    PVOID FindBufferBase;
    PVOID FindBufferNext;
    ULONG FindBufferLength;
    ULONG FindBufferValidLength;
    RTL_CRITICAL_SECTION FindBufferLock;
} FINDFILE_HANDLE, *PFINDFILE_HANDLE;

#define BASE_FIND_FIRST_DEVICE_HANDLE (HANDLE)1

;begin_noxbox

WINBASEAPI
BOOL
WINAPI
GetDaylightFlag(VOID);

WINBASEAPI
BOOL
WINAPI
SetDaylightFlag(
    BOOL fDaylight
    );

;end_noxbox

;end_internal

WINBASEAPI
int
WINAPI
wvsprintf%(
    OUT LPTSTR%,
    IN LPCTSTR%,
    IN va_list arglist);

WINBASEAPI
int
WINAPIV
wsprintf%(
    OUT LPTSTR%,
    IN LPCTSTR%,
    ...);

#define CP_ACP                    0           // default to ANSI code page
#define CP_UTF8                   65001       // UTF-8 translation

WINBASEAPI
int
WINAPI
MultiByteToWideChar(
    IN UINT     CodePage,
    IN DWORD    dwFlags,
    IN LPCSTR   lpMultiByteStr,
    IN int      cbMultiByte,
    OUT LPWSTR  lpWideCharStr,
    IN int      cchWideChar);

WINBASEAPI
int
WINAPI
WideCharToMultiByte(
    IN UINT     CodePage,
    IN DWORD    dwFlags,
    IN LPCWSTR  lpWideCharStr,
    IN int      cchWideChar,
    OUT LPSTR   lpMultiByteStr,
    IN int      cbMultiByte,
    IN LPCSTR   lpDefaultChar,
    OUT LPBOOL  lpUsedDefaultChar);

;begin_internal
/* Max number of characters. Doesn't include termination character */
#define WSPRINTF_LIMIT 1024
;end_internal

WINBASEAPI
LPSTR
WINAPI
CharUpperA(
    IN OUT LPSTR lpsz);
WINBASEAPI
LPWSTR
WINAPI
CharUpperW(
    IN OUT LPWSTR lpsz);
#ifdef UNICODE
#define CharUpper  CharUpperW
#else
#define CharUpper  CharUpperA
#endif // !UNICODE

WINBASEAPI
LPSTR
WINAPI
CharLowerA(
    IN OUT LPSTR lpsz);
WINBASEAPI
LPWSTR
WINAPI
CharLowerW(
    IN OUT LPWSTR lpsz);
#ifdef UNICODE
#define CharLower  CharLowerW
#else
#define CharLower  CharLowerA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
SetRect(
    OUT LPRECT lprc,
    IN int xLeft,
    IN int yTop,
    IN int xRight,
    IN int yBottom);

WINBASEAPI
BOOL
WINAPI
SetRectEmpty(
    OUT LPRECT lprc);

WINBASEAPI
BOOL
WINAPI
CopyRect(
    OUT LPRECT lprcDst,
    IN CONST RECT *lprcSrc);

WINBASEAPI
BOOL
WINAPI
InflateRect(
    IN OUT LPRECT lprc,
    IN int dx,
    IN int dy);

WINBASEAPI
BOOL
WINAPI
IntersectRect(
    OUT LPRECT lprcDst,
    IN CONST RECT *lprcSrc1,
    IN CONST RECT *lprcSrc2);

WINBASEAPI
BOOL
WINAPI
UnionRect(
    OUT LPRECT lprcDst,
    IN CONST RECT *lprcSrc1,
    IN CONST RECT *lprcSrc2);

WINBASEAPI
BOOL
WINAPI
SubtractRect(
    OUT LPRECT lprcDst,
    IN CONST RECT *lprcSrc1,
    IN CONST RECT *lprcSrc2);

WINBASEAPI
BOOL
WINAPI
OffsetRect(
    IN OUT LPRECT lprc,
    IN int dx,
    IN int dy);

WINBASEAPI
BOOL
WINAPI
IsRectEmpty(
    IN CONST RECT *lprc);

WINBASEAPI
BOOL
WINAPI
EqualRect(
    IN CONST RECT *lprc1,
    IN CONST RECT *lprc2);

WINBASEAPI
BOOL
WINAPI
PtInRect(
    IN CONST RECT *lprc,
    IN POINT pt);


#ifdef __cplusplus      ;both
}                       ;both
#endif                  ;both


#endif  // ndef _WINBASEP_  ;internal

#endif // _WINBASE_
