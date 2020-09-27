/*++

Copyright (c) Microsoft Corporation.  All right reserved.

Module Name:

    certspyp.h.

Abstract:

    This is a pre-compiled header module for certspy.

--*/

#if !defined(_CERTSPYP_INCLUDED_)
#define _CERTSPYP_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __cplusplus
extern "C" {
#endif

#include <ntos.h>
#include <mi.h>
#include <ldr.h>
#include <xbeimage.h>
#include <xtl.h>
#include <xbdm.h>
#include <xboxverp.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

#ifndef CONSTANT_OBJECT_STRING
#define CONSTANT_OBJECT_STRING(s) { sizeof(s) - sizeof(OCHAR), sizeof(s), s }
#endif

//
// Define bit definition of kernel export flags
//

#define KEF_INTERCEPT_AT_STARTUP    1

#pragma pack(1)

//
// Define structure of ordinal thunk for Intel x86 processor
//
// Here is what the stub looks like
//
//      60          pushad
//      8d542432    lea     edx, [esp+32]
//      b9nnnnnnnn  mov     ecx, OridnalNumber
//      e8nnnnnnnn  call    @CertSpyLogOrdinalCall
//      61          popad
//      e9nnnnnnnn  jmp     OriginalFunction
//

typedef struct {

    BYTE   __pushad;
    DWORD  __lea_edx_esp_plus_32;
    BYTE   __mov_ecx_immediate;
    DWORD  ImmediateValue;
    BYTE   __call_LogFunction;
    DWORD  OffsetOfLogFunction;
    BYTE   __popad;
    BYTE   __jmp_far;
    DWORD  ImmediateJmpAddress;

} ORDINAL_THUNK, *PORDINAL_THUNK;

typedef enum {

    PT_NONE = 0,
    PT_LONG,
    PT_LONGLONG,
    PT_PSTR,
    PT_PWSTR,
    PT_POOLTAG,
    PT_PSTRING,
    PT_POBJATTR,
    PT_PUSTRING

} PARAMETER_TYPE;

//
// Define the structure of xboxkrnl export functions and their parameter
// encoding
//

typedef struct {

    PCSTR   FunctionName;
    UCHAR   Parameters[12];
    PORDINAL_THUNK Thunk;

} KERNEL_EXPORT_API, *PKERNEL_EXPORT_API;

#pragma pack()

//
// List of xboxkrnl.exe export functions and byte-encoded parameters
//

const KERNEL_EXPORT_API KernelExports[] = {

    {  NULL, PT_NONE },
    { "AvGetSavedDataAddress",              PT_NONE },
    { "AvSendTVEncoderOption",              PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "AvSetDisplayMode",                   PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "AvSetSavedDataAddress",              PT_NONE },
    { "DbgBreakPoint",                      PT_NONE },
    { "DbgBreakPointWithStatus",            PT_LONG },
    { "DbgLoadImageSymbols",                PT_PSTRING, PT_LONG, PT_LONG },
    { "DbgPrint",                           PT_PSTR },
    { "HalReadSMCTrayState",                PT_LONG, PT_LONG },
    { "DbgPrompt",                          PT_PSTR, PT_LONG, PT_LONG },
    { "DbgUnLoadImageSymbols",              PT_PSTRING, PT_LONG, PT_LONG },
    { "ExAcquireReadWriteLockExclusive",    PT_LONG },
    { "ExAcquireReadWriteLockShared",       PT_LONG },
    { "ExAllocatePool",                     PT_LONG },
    { "ExAllocatePoolWithTag",              PT_LONG, PT_POOLTAG },
    {  NULL, PT_NONE },                     // ExEventObjectType
    { "ExFreePool",                         PT_LONG },
    { "ExInitializeReadWriteLock",          PT_LONG },
    { "ExInterlockedAddLargeInteger",       PT_LONG, PT_LONGLONG },
    { "ExInterlockedAddLargeStatistic",     PT_LONG, PT_LONG },
    { "ExInterlockedCompareExchange64",     PT_LONG, PT_LONG, PT_LONG },
    {  NULL, PT_NONE },                     // ExMutantObjectType
    { "ExQueryPoolBlockSize",               PT_LONG },
    { "ExQueryNonVolatileSetting",          PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "ExReadWriteRefurbInfo",              PT_LONG, PT_LONG, PT_LONG },
    { "ExRaiseException",                   PT_LONG },
    { "ExRaiseStatus",                      PT_LONG },
    { "ExReleaseReadWriteLock",             PT_LONG },
    { "ExSaveNonVolatileSetting",           PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    {  NULL, PT_NONE },                     // ExSemaphoreObjectType
    {  NULL, PT_NONE },                     // ExTimerObjectType
    { "ExfInterlockedInsertHeadList",       PT_NONE },
    { "ExfInterlockedInsertTailList",       PT_NONE },
    { "ExfInterlockedRemoveHeadList",       PT_NONE },
    { "FscGetCacheSize",                    PT_NONE },
    { "FscInvalidateIdleBlocks",            PT_NONE },
    { "FscSetCacheSize",                    PT_LONG },
    { "HalClearSoftwareInterrupt",          PT_NONE },
    { "HalDisableSystemInterrupt",          PT_LONG },
    {  NULL, NULL },                        // HalDiskCachePartitionCount
    {  NULL, NULL },                        // HalDiskModelNumber
    {  NULL, NULL },                        // HalDiskSerialNumber
    { "HalEnableSystemInterrupt",           PT_LONG, PT_LONG },
    { "HalGetInterruptVector",              PT_LONG, PT_LONG },
    { "HalReadSMBusValue",                  PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "HalReadWritePCISpace",               PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "HalRegisterShutdownNotification",    PT_LONG, PT_LONG },
    { "HalRequestSoftwareInterrupt",        PT_LONG },
    { "HalReturnToFirmware",                PT_LONG },
    { "HalWriteSMBusValue",                 PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "InterlockedCompareExchange",         PT_LONG, PT_LONG, PT_LONG },
    { "InterlockedDecrement",               PT_LONG },
    { "InterlockedIncrement",               PT_LONG },
    { "InterlockedExchange",                PT_LONG, PT_LONG },
    { "InterlockedExchangeAdd",             PT_LONG, PT_LONG },
    { "InterlockedFlushSList",              PT_LONG },
    { "InterlockedPopEntrySList",           PT_LONG },
    { "InterlockedPushEntrySList",          PT_LONG, PT_LONG },
    { "IoAllocateIrp",                      PT_LONG },
    { "IoBuildAsynchronousFsdRequest",      PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoBuildDeviceIoControlRequest",      PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoBuildSynchronousFsdRequest",       PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoCheckShareAccess",                 PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    {  NULL, NULL },                        // IoCompletionObjectType
    { "IoCreateDevice",                     PT_LONG, PT_LONG, PT_PSTRING, PT_LONG, PT_LONG, PT_LONG },
    { "IoCreateFile",                       PT_LONG, PT_LONG, PT_POBJATTR, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoCreateSymbolicLink",               PT_PSTRING, PT_PSTRING },
    { "IoDeleteDevice",                     PT_LONG },
    { "IoDeleteSymbolicLink",               PT_PSTRING },
    {  NULL, NULL },                        // IoDeviceObjectType
    {  NULL, NULL },                        // IoFileObjectType
    { "IoFreeIrp",                          PT_LONG },
    { "IoInitializeIrp",                    PT_LONG, PT_LONG, PT_LONG },
    { "IoInvalidDeviceRequest",             PT_LONG, PT_LONG },
    { "IoQueryFileInformation",             PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoQueryVolumeInformation",           PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoQueueThreadIrp",                   PT_LONG },
    { "IoRemoveShareAccess",                PT_LONG, PT_LONG },
    { "IoSetIoCompletion",                  PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoSetShareAccess",                   PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoStartNextPacket",                  PT_LONG },
    { "IoStartNextPacketByKey",             PT_LONG, PT_LONG },
    { "IoStartPacket",                      PT_LONG, PT_LONG, PT_LONG },
    { "IoSynchronousDeviceIoControlRequest",PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IoSynchronousFsdRequest",            PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "IofCallDriver",                      PT_NONE },
    { "IofCompleteRequest",                 PT_NONE },
    {  NULL, NULL },                        // KdDebuggerEnabled
    {  NULL, NULL },                        // KdDebuggerNotPresent
    { "IoDismountVolume",                   PT_LONG },
    { "IoDismountVolumeByName",             PT_PSTRING },
    { "KeAlertResumeThread",                PT_LONG },
    { "KeAlertThread",                      PT_LONG, PT_LONG },
    { "KeBoostPriorityThread",              PT_LONG, PT_LONG },
    { "KeBugCheck",                         PT_LONG },
    { "KeBugCheckEx",                       PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "KeCancelTimer",                      PT_LONG },
    { "KeConnectInterrupt",                 PT_LONG },
    { "KeDelayExecutionThread",             PT_LONG, PT_LONG, PT_LONG },
    { "KeDisconnectInterrupt",              PT_LONG },
    { "KeEnterCriticalRegion",              PT_NONE },
    {  NULL, NULL },                        // MmGlobalData
    { "KeGetCurrentIrql",                   PT_NONE },
    { "KeGetCurrentThread",                 PT_NONE },
    { "KeInitializeApc",                    PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "KeInitializeDeviceQueue",            PT_LONG },
    { "KeInitializeDpc",                    PT_LONG, PT_LONG, PT_LONG },
    { "KeInitializeEvent",                  PT_LONG, PT_LONG, PT_LONG },
    { "KeInitializeInterrupt",              PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "KeInitializeMutant",                 PT_LONG, PT_LONG },
    { "KeInitializeQueue",                  PT_LONG, PT_LONG },
    { "KeInitializeSemaphore",              PT_LONG, PT_LONG, PT_LONG },
    { "KeInitializeTimerEx",                PT_LONG, PT_LONG },
    { "KeInsertByKeyDeviceQueue",           PT_LONG, PT_LONG, PT_LONG },
    { "KeInsertDeviceQueue",                PT_LONG, PT_LONG },
    { "KeInsertHeadQueue",                  PT_LONG, PT_LONG },
    { "KeInsertQueue",                      PT_LONG, PT_LONG },
    { "KeInsertQueueApc",                   PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "KeInsertQueueDpc",                   PT_LONG, PT_LONG, PT_LONG },
    {  NULL, NULL },                        // KeInterruptTime}
    { "KeIsExecutingDpc",                   PT_NONE },
    { "KeLeaveCriticalRegion",              PT_NONE },
    { "KePulseEvent",                       PT_LONG, PT_LONG, PT_LONG },
    { "KeQueryBasePriorityThread",          PT_LONG },
    { "KeQueryInterruptTime",               PT_NONE },
    { "KeQueryPerformanceCounter",          PT_NONE },
    { "KeQueryPerformanceFrequency",        PT_NONE },
    { "KeQuerySystemTime",                  PT_LONG },
    { "KeRaiseIrqlToDpcLevel",              PT_NONE },
    { "KeRaiseIrqlToSynchLevel",            PT_NONE },
    { "KeReleaseMutant",                    PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "KeReleaseSemaphore",                 PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "KeRemoveByKeyDeviceQueue",           PT_LONG, PT_LONG },
    { "KeRemoveDeviceQueue",                PT_LONG },
    { "KeRemoveEntryDeviceQueue",           PT_LONG, PT_LONG },
    { "KeRemoveQueue",                      PT_LONG, PT_LONG, PT_LONG },
    { "KeRemoveQueueDpc",                   PT_LONG },
    { "KeResetEvent",                       PT_LONG },
    { "KeRestoreFloatingPointState",        PT_LONG },
    { "KeResumeThread",                     PT_LONG },
    { "KeRundownQueue",                     PT_LONG },
    { "KeSaveFloatingPointState",           PT_LONG },
    { "KeSetBasePriorityThread",            PT_LONG, PT_LONG },
    { "KeSetDisableBoostThread",            PT_LONG, PT_LONG },
    { "KeSetEvent",                         PT_LONG, PT_LONG, PT_LONG },
    { "KeSetEventBoostPriority",            PT_LONG, PT_LONG },
    { "KeSetPriorityProcess",               PT_LONG, PT_LONG },
    { "KeSetPriorityThread",                PT_LONG, PT_LONG },
    { "KeSetTimer",                         PT_LONG, PT_LONGLONG, PT_LONG },
    { "KeSetTimerEx",                       PT_LONG, PT_LONGLONG, PT_LONG, PT_LONG },
    { "KeStallExecutionProcessor",          PT_LONG },
    { "KeSuspendThread",                    PT_LONG },
    { "KeSynchronizeExecution",             PT_LONG, PT_LONG, PT_LONG },
    {  NULL, NULL },                        // KeSystemTime
    { "KeTestAlertThread",                  PT_LONG },
    {  NULL, NULL },                        // KeTickCount
    {  NULL, NULL },                        // KeTimeIncrement
    { "KeWaitForMultipleObjects",           PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "KeWaitForSingleObject",              PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "KfRaiseIrql",                        PT_NONE },
    { "KfLowerIrql",                        PT_NONE },
    {  NULL, NULL },                        // KiBugCheckData
    { "KiUnlockDispatcherDatabase",         PT_LONG },
    {  NULL, NULL },                        // LaunchDataPage
    { "MmAllocateContiguousMemory",         PT_LONG },
    { "MmAllocateContiguousMemoryEx",       PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "MmAllocateSystemMemory",             PT_LONG, PT_LONG },
    { "MmClaimGpuInstanceMemory",           PT_LONG, PT_LONG },
    { "MmCreateKernelStack",                PT_LONG, PT_LONG },
    { "MmDeleteKernelStack",                PT_LONG, PT_LONG },
    { "MmFreeContiguousMemory",             PT_LONG },
    { "MmFreeSystemMemory",                 PT_LONG, PT_LONG },
    { "MmGetPhysicalAddress",               PT_LONG },
    { "MmIsAddressValid",                   PT_LONG },
    { "MmLockUnlockBufferPages",            PT_LONG, PT_LONG, PT_LONG },
    { "MmLockUnlockPhysicalPage",           PT_LONG, PT_LONG },
    { "MmMapIoSpace",                       PT_LONG, PT_LONG, PT_LONG },
    { "MmPersistContiguousMemory",          PT_LONG, PT_LONG, PT_LONG },
    { "MmQueryAddressProtect",              PT_LONG },
    { "MmQueryAllocationSize",              PT_LONG },
    { "MmQueryStatistics",                  PT_LONG },
    { "MmSetAddressProtect",                PT_LONG, PT_LONG, PT_LONG },
    { "MmUnmapIoSpace",                     PT_LONG, PT_LONG },
    { "NtAllocateVirtualMemory",            PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtCancelTimer",                      PT_LONG, PT_LONG },
    { "NtClearEvent",                       PT_LONG },
    { "NtClose",                            PT_LONG },
    { "NtCreateDirectoryObject",            PT_LONG, PT_POBJATTR },
    { "NtCreateEvent",                      PT_LONG, PT_POBJATTR, PT_LONG, PT_LONG },
    { "NtCreateFile",                       PT_LONG, PT_LONG, PT_POBJATTR, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtCreateIoCompletion",               PT_LONG, PT_LONG, PT_POBJATTR, PT_LONG },
    { "NtCreateMutant",                     PT_LONG, PT_POBJATTR, PT_LONG },
    { "NtCreateSemaphore",                  PT_LONG, PT_POBJATTR, PT_LONG, PT_LONG },
    { "NtCreateTimer",                      PT_LONG, PT_POBJATTR, PT_LONG },
    { "NtDeleteFile",                       PT_POBJATTR },
    { "NtDeviceIoControlFile",              PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtDuplicateObject",                  PT_LONG, PT_LONG, PT_LONG },
    { "NtFlushBuffersFile",                 PT_LONG, PT_LONG },
    { "NtFreeVirtualMemory",                PT_LONG, PT_LONG, PT_LONG },
    { "NtFsControlFile",                    PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtOpenDirectoryObject",              PT_LONG, PT_POBJATTR },
    { "NtOpenFile",                         PT_LONG, PT_LONG, PT_POBJATTR, PT_LONG, PT_LONG, PT_LONG },
    { "NtOpenSymbolicLinkObject",           PT_LONG, PT_POBJATTR },
    { "NtProtectVirtualMemory",             PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtPulseEvent",                       PT_LONG, PT_LONG },
    { "NtQueueApcThread",                   PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtQueryDirectoryFile",               PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_PSTRING, PT_LONG },
    { "NtQueryDirectoryObject",             PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtQueryEvent",                       PT_LONG, PT_LONG },
    { "NtQueryFullAttributesFile",          PT_POBJATTR, PT_LONG },
    { "NtQueryInformationFile",             PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtQueryIoCompletion",                PT_LONG, PT_LONG },
    { "NtQueryMutant",                      PT_LONG, PT_LONG },
    { "NtQuerySemaphore",                   PT_LONG, PT_LONG },
    { "NtQuerySymbolicLinkObject",          PT_LONG, PT_PSTRING, PT_LONG },
    { "NtQueryTimer",                       PT_LONG, PT_LONG },
    { "NtQueryVirtualMemory",               PT_LONG, PT_LONG },
    { "NtQueryVolumeInformationFile",       PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtReadFile",                         PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtReadFileScatter",                  PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtReleaseMutant",                    PT_LONG, PT_LONG },
    { "NtReleaseSemaphore",                 PT_LONG, PT_LONG, PT_LONG },
    { "NtRemoveIoCompletion",               PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtResumeThread",                     PT_LONG, PT_LONG },
    { "NtSetEvent",                         PT_LONG, PT_LONG },
    { "NtSetInformationFile",               PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtSetIoCompletion",                  PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtSetSystemTime",                    PT_LONG, PT_LONG },
    { "NtSetTimerEx",                       PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtSignalAndWaitForSingleObjectEx",   PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtSuspendThread",                    PT_LONG, PT_LONG },
    { "NtUserIoApcDispatcher",              PT_LONG, PT_LONG, PT_LONG },
    { "NtWaitForSingleObject",              PT_LONG, PT_LONG, PT_LONG },
    { "NtWaitForSingleObjectEx",            PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtWaitForMultipleObjectsEx",         PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtWriteFile",                        PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtWriteFileGather",                  PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "NtYieldExecution",                   PT_NONE },
    { "ObCreateObject",                     PT_LONG, PT_POBJATTR, PT_LONG, PT_LONG },
    {  NULL, NULL },                        // ObDirectoryObjectType
    { "ObInsertObject",                     PT_LONG, PT_POBJATTR, PT_LONG, PT_LONG },
    { "ObMakeTemporaryObject",              PT_LONG },
    { "ObOpenObjectByName",                 PT_POBJATTR, PT_LONG, PT_LONG, PT_LONG },
    { "ObOpenObjectByPointer",              PT_LONG, PT_LONG, PT_LONG },
    {  NULL, NULL },                        // ObpObjectHandleTable
    { "ObReferenceObjectByHandle",          PT_LONG, PT_LONG, PT_LONG },
    { "ObReferenceObjectByName",            PT_PSTRING, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "ObReferenceObjectByPointer",         PT_LONG, PT_LONG },
    {  NULL, NULL },                        // ObSymbolicLinkObjectType
    { "ObfDereferenceObject",               PT_NONE },
    { "ObfReferenceObject",                 PT_NONE },
    { "PhyGetLinkState",                    PT_LONG },
    { "PhyInitialize",                      PT_LONG, PT_LONG },
    { "PsCreateSystemThread",               PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "PsCreateSystemThreadEx",             PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "PsQueryStatistics",                  PT_LONG },
    { "PsSetCreateThreadNotifyRoutine",     PT_LONG },
    { "PsTerminateSystemThread",            PT_LONG },
    {  NULL, NULL },                        // PsThreadObjectType
    { "RtlAnsiStringToUnicodeString",       PT_PUSTRING, PT_PSTRING, PT_LONG },
    { "RtlAppendStringToString",            PT_PSTRING, PT_PSTRING },
    { "RtlAppendUnicodeStringToString",     PT_PUSTRING, PT_PUSTRING },
    { "RtlAppendUnicodeToString",           PT_PUSTRING, PT_PWSTR },
    { "RtlAssert",                          PT_PSTR, PT_PSTR, PT_LONG, PT_PSTR },
    { "RtlCaptureContext",                  PT_LONG },
    { "RtlCaptureStackBackTrace",           PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "RtlCharToInteger",                   PT_PSTR, PT_LONG, PT_LONG },
    { "RtlCompareMemory",                   PT_LONG, PT_LONG, PT_LONG },
    { "RtlCompareMemoryUlong",              PT_LONG, PT_LONG, PT_LONG },
    { "RtlCompareString",                   PT_PSTRING, PT_PSTRING, PT_LONG },
    { "RtlCompareUnicodeString",            PT_PUSTRING, PT_PUSTRING, PT_LONG },
    { "RtlCopyString",                      PT_PSTRING, PT_PSTRING },
    { "RtlCopyUnicodeString",               PT_PUSTRING, PT_PUSTRING },
    { "RtlCreateUnicodeString",             PT_PUSTRING, PT_PWSTR },
    { "RtlDowncaseUnicodeChar",             PT_LONG },
    { "RtlDowncaseUnicodeString",           PT_PUSTRING, PT_PUSTRING, PT_LONG },
    { "RtlEnterCriticalSection",            PT_LONG },
    { "RtlEnterCriticalSectionAndRegion",   PT_LONG },
    { "RtlEqualString",                     PT_PSTRING, PT_PSTRING, PT_LONG },
    { "RtlEqualUnicodeString",              PT_PUSTRING, PT_PUSTRING, PT_LONG },
    { "RtlExtendedIntegerMultiply",         PT_LONGLONG, PT_LONG },
    { "RtlExtendedLargeIntegerDivide",      PT_LONGLONG, PT_LONG, PT_LONG },
    { "RtlExtendedMagicDivide",             PT_LONGLONG, PT_LONGLONG, PT_LONG },
    { "RtlFillMemory",                      PT_LONG, PT_LONG, PT_LONG },
    { "RtlFillMemoryUlong",                 PT_LONG, PT_LONG, PT_LONG },
    { "RtlFreeAnsiString",                  PT_PSTRING },
    { "RtlFreeUnicodeString",               PT_PUSTRING },
    { "RtlGetCallersAddress",               PT_LONG, PT_LONG },
    { "RtlInitAnsiString",                  PT_PSTRING, PT_PSTR },
    { "RtlInitUnicodeString",               PT_PUSTRING, PT_PWSTR },
    { "RtlInitializeCriticalSection",       PT_LONG },
    { "RtlIntegerToChar",                   PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "RtlIntegerToUnicodeString",          PT_LONG, PT_LONG, PT_PUSTRING },
    { "RtlLeaveCriticalSection",            PT_LONG },
    { "RtlLeaveCriticalSectionAndRegion",   PT_LONG },
    { "RtlLowerChar",                       PT_LONG },
    { "RtlMapGenericMask",                  PT_LONG, PT_LONG },
    { "RtlMoveMemory",                      PT_LONG, PT_LONG, PT_LONG },
    { "RtlMultiByteToUnicodeN",             PT_LONG, PT_LONG, PT_LONG, PT_PSTR, PT_LONG },
    { "RtlMultiByteToUnicodeSize",          PT_LONG, PT_PSTR, PT_LONG },
    { "RtlNtStatusToDosError",              PT_LONG },
    { "RtlRaiseException",                  PT_LONG },
    { "RtlRaiseStatus",                     PT_LONG },
    { "RtlTimeFieldsToTime",                PT_LONG, PT_LONG },
    { "RtlTimeToTimeFields",                PT_LONG, PT_LONG },
    { "RtlTryEnterCriticalSection",         PT_LONG },
    { "RtlUlongByteSwap",                   PT_LONG },
    { "RtlUnicodeStringToAnsiString",       PT_PSTRING, PT_PUSTRING, PT_LONG },
    { "RtlUnicodeStringToInteger",          PT_PUSTRING, PT_LONG, PT_LONG },
    { "RtlUnicodeToMultiByteN",             PT_LONG, PT_LONG, PT_LONG, PT_PWSTR, PT_LONG },
    { "RtlUnicodeToMultiByteSize",          PT_LONG, PT_PUSTRING, PT_LONG },
    { "RtlUnwind",                          PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "RtlUpcaseUnicodeChar",               PT_LONG },
    { "RtlUpcaseUnicodeString",             PT_PUSTRING, PT_PUSTRING, PT_LONG },
    { "RtlUpcaseUnicodeToMultiByteN",       PT_LONG, PT_LONG, PT_LONG, PT_PWSTR, PT_LONG },
    { "RtlUpperChar",                       PT_LONG },
    { "RtlUpperString",                     PT_PSTRING, PT_PSTRING },
    { "RtlUshortByteSwap",                  PT_LONG },
    { "RtlWalkFrameChain",                  PT_LONG, PT_LONG, PT_LONG },
    { "RtlZeroMemory",                      PT_LONG, PT_LONG },
    {  NULL, NULL },                        // XboxEEPROMKey
    {  NULL, NULL },                        // XboxHardwareInfo
    {  NULL, NULL },                        // XboxHDKey
    {  NULL, NULL },                        // XboxKrnlVersion
    {  NULL, NULL },                        // XboxSignatureKey
    {  NULL, NULL },                        // XeImageFileName
    { "XeLoadSection",                      PT_LONG },
    { "XeUnloadSection",                    PT_LONG },
    { "READ_PORT_BUFFER_UCHAR",             PT_LONG, PT_LONG, PT_LONG },
    { "READ_PORT_BUFFER_USHORT",            PT_LONG, PT_LONG, PT_LONG },
    { "READ_PORT_BUFFER_ULONG",             PT_LONG, PT_LONG, PT_LONG },
    { "WRITE_PORT_BUFFER_UCHAR",            PT_LONG, PT_LONG, PT_LONG },
    { "WRITE_PORT_BUFFER_USHORT",           PT_LONG, PT_LONG, PT_LONG },
    { "WRITE_PORT_BUFFER_ULONG",            PT_LONG, PT_LONG, PT_LONG },
    { "XcSHAInit",                          PT_LONG },
    { "XcSHAUpdate",                        PT_LONG, PT_LONG, PT_LONG },
    { "XcSHAFinal",                         PT_LONG, PT_LONG },
    { "XcRC4Key",                           PT_LONG, PT_LONG, PT_LONG },
    { "XcRC4Crypt",                         PT_LONG, PT_LONG, PT_LONG },
    { "XcHMAC",                             PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "XcPKEncPublic",                      PT_LONG, PT_LONG, PT_LONG },
    { "XcPKDecPrivate",                     PT_LONG, PT_LONG, PT_LONG },
    { "XcPKGetKeyLen",                      PT_LONG },
    { "XcVerifyPKCS1Signature",             PT_LONG, PT_LONG, PT_LONG },
    { "XcModExp",                           PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "XcDESKeyParity",                     PT_LONG, PT_LONG },
    { "XcKeyTable",                         PT_LONG, PT_LONG, PT_LONG },
    { "XcBlockCrypt",                       PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "XcBlockCryptCBC",                    PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG, PT_LONG },
    { "XcCryptService",                     PT_LONG, PT_LONG },
    { "XcUpdateCrypto",                     PT_LONG, PT_LONG },
    { "RtlRip",                             PT_PSTR, PT_PSTR, PT_PSTR },
    {  NULL, NULL },                        // XboxLANKey
    {  NULL, NULL },                        // XboxAlternateSignatureKeys
    {  NULL, NULL },                        // XePublicKeyData
    {  NULL, NULL },                        // HalBootSMCVideoMode
    {  NULL, NULL },                        // IdexChannelObject
    { "HalIsResetOrShutdownPending",        PT_NONE },
    { "IoMarkIrpMustComplete",              PT_LONG },
    { "HalInitiateShutdown",                PT_NONE },
    { "RtlSnprintf",                        PT_PSTR, PT_LONG, PT_PSTR },
    { "RtlSprintf",                         PT_PSTR, PT_PSTR },
    { "RtlVsnprintf",                       PT_PSTR, PT_LONG, PT_PSTR, PT_LONG },
    { "RtlVsprintf",                        PT_PSTR, PT_PSTR, PT_LONG },
    { "HalEnableSecureTrayEject",           PT_NONE },
    { "HalWriteSMCScratchRegister",         PT_LONG },
    {  NULL, NULL },                        // Unused
    {  NULL, NULL },                        // Unused
    {  NULL, NULL },                        // Unused
    { "XProfpControl",                      PT_LONG, PT_LONG },
    { "XProfpGetData",                      PT_NONE },
    { "IrtClientInitFast",                  PT_NONE },
    { "IrtSweep",                           PT_LONG },
    { "MmDbgAllocateMemory",                PT_LONG, PT_LONG },
    { "MmDbgFreeMemory",                    PT_LONG, PT_LONG },
    { "MmDbgQueryAvailablePages",           PT_NONE },
    { "MmDbgReleaseAddress",                PT_LONG, PT_LONG },
    { "MmDbgWriteCheck",                    PT_LONG, PT_LONG },
};

static const SIZE_T KernelExportSize = ARRAYSIZE(KernelExports);

extern BYTE KernelExportFlags[KernelExportSize];
extern PORDINAL_THUNK InterceptThunks[KernelExportSize];
extern PIMAGE_THUNK_DATA ImageThunks[KernelExportSize];

HRESULT
WINAPI
CertSpyCommandProcessor(
    IN  PCSTR  szCommand,
    OUT PSTR   szResponse,
    IN  SIZE_T cchResponse,
    IN  PDM_CMDCONT pdmcc
    );

__inline
VOID
CertSpyFlushICache(
    VOID
    )
{
    __asm {
        wbinvd
        push    ebx
        xor     eax, eax
        cpuid
        pop     ebx
    }
}

NTSTATUS
CertSpyHookKernelImportOrdinal(
    IN ULONG OrdinalNumber
    );

NTSTATUS
CertSpyUnhookKernelImportOrdinal(
    IN ULONG OrdinalNumber
    );

#ifdef __cplusplus
}
#endif

#endif // _CERTSPYP_INCLUDED_
