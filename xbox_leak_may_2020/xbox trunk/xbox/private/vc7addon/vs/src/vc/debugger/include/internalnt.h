
// contains items culled from NT DDK headers
// sorted by which file they were copied from

// this macro to use undefined types as pointers
#define	SOME_POINTER(foo)	void*

// from ntdef.h
typedef LONG NTSTATUS;

//
// Unicode strings are counted 16-bit character strings. If they are
// NULL terminated, Length does not include trailing NULL.
//

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

// from privnt.h
typedef LONG KPRIORITY;

// from ntdef.h
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

// from ntpsapi.h

//
// Thread Information Classes
//

typedef enum _THREADINFOCLASS {
    ThreadBasicInformation,
    ThreadTimes,
    ThreadPriority,
    ThreadBasePriority,
    ThreadAffinityMask,
    ThreadImpersonationToken,
    ThreadDescriptorTableEntry,
    ThreadEnableAlignmentFaultFixup,
    ThreadEventPair_Reusable,
    ThreadQuerySetWin32StartAddress,
    ThreadZeroTlsCell,
    ThreadPerformanceCount,
    ThreadAmILastThread,
    ThreadIdealProcessor,
    ThreadPriorityBoost,
    ThreadSetTlsArrayAddress,
    ThreadIsIoPending,
    ThreadHideFromDebugger,
    MaxThreadInfoClass
    } THREADINFOCLASS;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

//
// Basic Thread Information
//  NtQueryInformationThread using ThreadBasicInfo
//

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    SOME_POINTER(PTEB) TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG_PTR AffinityMask;
    KPRIORITY Priority;
    LONG BasePriority;
} THREAD_BASIC_INFORMATION;
typedef THREAD_BASIC_INFORMATION *PTHREAD_BASIC_INFORMATION;

#ifdef _IA64_
#include <pshpck16.h>
#endif

typedef struct _FIBER {

    PVOID FiberData;

    //
    // Matches first three DWORDs of TEB
    //

    struct _EXCEPTION_REGISTRATION_RECORD *ExceptionList;
    PVOID StackBase;
    PVOID StackLimit;

    //
    // Used by base to free a thread's stack
    //

    PVOID DeallocationStack;

    CONTEXT FiberContext;

    SOME_POINTER(PWX86TIB) Wx86Tib;

#ifdef  _IA64_
    PVOID DeallocationBStore;
    PVOID BStoreLimit;
#endif

} FIBER, *PFIBER;

#ifdef _IA64_
#include <poppack.h>
#endif

//
// Gdi command batching
//

#define GDI_BATCH_BUFFER_SIZE 310

typedef struct _GDI_TEB_BATCH {
    ULONG    Offset;
    ULONG_PTR HDC;
    ULONG    Buffer[GDI_BATCH_BUFFER_SIZE];
} GDI_TEB_BATCH,*PGDI_TEB_BATCH;


//
// Wx86 thread state information
//

typedef struct _Wx86ThreadState {
    PULONG  CallBx86Eip;
    PVOID   DeallocationCpu;
    BOOLEAN UseKnownWx86Dll;
    char    OleStubInvoked;
} WX86THREAD, *PWX86THREAD;


//
//  TEB - The thread environment block
//

#define STATIC_UNICODE_BUFFER_LENGTH 261
#define WIN32_CLIENT_INFO_LENGTH 62

#define WIN32_CLIENT_INFO_SPIN_COUNT 1

typedef struct _TEB {
    NT_TIB NtTib;
    PVOID  EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID ActiveRpcHandle;
    PVOID ThreadLocalStoragePointer;
    SOME_POINTER(PPEB) ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    PVOID CsrClientThread;
    PVOID Win32ThreadInfo;          // PtiCurrent
    ULONG User32Reserved[26];       // user32.dll items
    ULONG UserReserved[5];          // Winsrv SwitchStack
    PVOID WOW32Reserved;            // used by WOW
    LCID CurrentLocale;
    ULONG FpSoftwareStatusRegister; // offset known by outsiders!
    PVOID SystemReserved1[54];      // Used by FP emulator
    NTSTATUS ExceptionCode;         // for RaiseUserException
    UCHAR SpareBytes1[44];
    GDI_TEB_BATCH GdiTebBatch;      // Gdi batching
    CLIENT_ID RealClientId;
    HANDLE GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    PVOID GdiThreadLocalInfo;
    ULONG_PTR Win32ClientInfo[WIN32_CLIENT_INFO_LENGTH]; // User32 Client Info
    PVOID glDispatchTable[233];     // OpenGL
    ULONG_PTR glReserved1[29];      // OpenGL
    PVOID glReserved2;              // OpenGL
    PVOID glSectionInfo;            // OpenGL
    PVOID glSection;                // OpenGL
    PVOID glTable;                  // OpenGL
    PVOID glCurrentRC;              // OpenGL
    PVOID glContext;                // OpenGL
    ULONG LastStatusValue;
    UNICODE_STRING StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[STATIC_UNICODE_BUFFER_LENGTH];
    PVOID DeallocationStack;
    PVOID TlsSlots[TLS_MINIMUM_AVAILABLE];
    LIST_ENTRY TlsLinks;
    PVOID Vdm;
    PVOID ReservedForNtRpc;
    PVOID DbgSsReserved[2];
    ULONG HardErrorsAreDisabled;
    PVOID Instrumentation[16];
    PVOID WinSockData;              // WinSock
    ULONG GdiBatchCount;
    BOOLEAN InDbgPrint;
    BOOLEAN SpareB1;
    BOOLEAN SpareB2;
    BOOLEAN SpareB3;
    ULONG Spare3;
    PVOID ReservedForPerf;
    PVOID ReservedForOle;
    ULONG WaitingOnLoaderLock;
    WX86THREAD Wx86Thread;
    PVOID *TlsExpansionSlots;
#ifdef  _IA64_
    PVOID DeallocationBStore;
    PVOID BStoreLimit;
#endif
    LCID ImpersonationLocale;      // Current locale of impersonated user
    ULONG IsImpersonating;         // Thread impersonation status
    PVOID NlsCache;                // NLS thread cache
} TEB;
typedef TEB *PTEB;

// from ntstatus.h

//
// MessageId: STATUS_DLL_NOT_FOUND
//
// MessageText:
//
//  {Unable To Locate DLL}
//  The dynamic link library %hs could not be found in the specified path %hs.
//
#define STATUS_DLL_NOT_FOUND             ((NTSTATUS)0xC0000135L)

//
// MessageId: STATUS_SUSPEND_COUNT_EXCEEDED
//
// MessageText:
//
//  An attempt was made to suspend a thread whose suspend count was at its maximum.
//
#define STATUS_SUSPEND_COUNT_EXCEEDED    ((NTSTATUS)0xC000004AL)

//
// MessageId: STATUS_THREAD_IS_TERMINATING
//
// MessageText:
//
//  An attempt was made to suspend a thread that has begun termination.
//
#define STATUS_THREAD_IS_TERMINATING     ((NTSTATUS)0xC000004BL)

//
// MessageId: STATUS_DLL_INIT_FAILED
//
// MessageText:
//
//  {DLL Initialization Failed}
//  Initialization of the dynamic link library %hs failed. The process is terminating abnormally.
//
#define STATUS_DLL_INIT_FAILED           ((NTSTATUS)0xC0000142L)
