/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xbeimport.cpp

Abstract:

    This module contains routine to dump kernel import in XBE file.

--*/

#include "xbeimportp.h"

const PCHAR KernelExports[] = {
    "Undefined",
    "AvGetSavedDataAddress",
    "AvSendTVEncoderOption",
    "AvSetDisplayMode",
    "AvSetSavedDataAddress",
    "DbgBreakPoint",
    "DbgBreakPointWithStatus",
    "DbgLoadImageSymbols",
    "DbgPrint",
    "HalReadSMCTrayState",
    "DbgPrompt",
    "DbgUnLoadImageSymbols",
    "ExAcquireReadWriteLockExclusive",
    "ExAcquireReadWriteLockShared",
    "ExAllocatePool",
    "ExAllocatePoolWithTag",
    "ExEventObjectType",
    "ExFreePool",
    "ExInitializeReadWriteLock",
    "ExInterlockedAddLargeInteger",
    "ExInterlockedAddLargeStatistic",
    "ExInterlockedCompareExchange64",
    "ExMutantObjectType",
    "ExQueryPoolBlockSize",
    "ExQueryNonVolatileSetting",
    "ExReadWriteRefurbInfo",
    "ExRaiseException",
    "ExRaiseStatus",
    "ExReleaseReadWriteLock",
    "ExSaveNonVolatileSetting",
    "ExSemaphoreObjectType",
    "ExTimerObjectType",
    "ExfInterlockedInsertHeadList",
    "ExfInterlockedInsertTailList",
    "ExfInterlockedRemoveHeadList",
    "FscGetCacheSize",
    "FscInvalidateIdleBlocks",
    "FscSetCacheSize",
    "HalClearSoftwareInterrupt",
    "HalDisableSystemInterrupt",
    "HalDiskCachePartitionCount",
    "HalDiskModelNumber",
    "HalDiskSerialNumber",
    "HalEnableSystemInterrupt",
    "HalGetInterruptVector",
    "HalReadSMBusValue",
    "HalReadWritePCISpace",
    "HalRegisterShutdownNotification",
    "HalRequestSoftwareInterrupt",
    "HalReturnToFirmware",
    "HalWriteSMBusValue",
    "InterlockedCompareExchange",
    "InterlockedDecrement",
    "InterlockedIncrement",
    "InterlockedExchange",
    "InterlockedExchangeAdd",
    "InterlockedFlushSList",
    "InterlockedPopEntrySList",
    "InterlockedPushEntrySList",
    "IoAllocateIrp",
    "IoBuildAsynchronousFsdRequest",
    "IoBuildDeviceIoControlRequest",
    "IoBuildSynchronousFsdRequest",
    "IoCheckShareAccess",
    "IoCompletionObjectType",
    "IoCreateDevice",
    "IoCreateFile",
    "IoCreateSymbolicLink",
    "IoDeleteDevice",
    "IoDeleteSymbolicLink",
    "IoDeviceObjectType",
    "IoFileObjectType",
    "IoFreeIrp",
    "IoInitializeIrp",
    "IoInvalidDeviceRequest",
    "IoQueryFileInformation",
    "IoQueryVolumeInformation",
    "IoQueueThreadIrp",
    "IoRemoveShareAccess",
    "IoSetIoCompletion",
    "IoSetShareAccess",
    "IoStartNextPacket",
    "IoStartNextPacketByKey",
    "IoStartPacket",
    "IoSynchronousDeviceIoControlRequest",
    "IoSynchronousFsdRequest",
    "IofCallDriver",
    "IofCompleteRequest",
    "KdDebuggerEnabled",
    "KdDebuggerNotPresent",
    "IoDismountVolume",
    "IoDismountVolumeByName",
    "KeAlertResumeThread",
    "KeAlertThread",
    "KeBoostPriorityThread",
    "KeBugCheck",
    "KeBugCheckEx",
    "KeCancelTimer",
    "KeConnectInterrupt",
    "KeDelayExecutionThread",
    "KeDisconnectInterrupt",
    "KeEnterCriticalRegion",
    "MmGlobalData",
    "KeGetCurrentIrql",
    "KeGetCurrentThread",
    "KeInitializeApc",
    "KeInitializeDeviceQueue",
    "KeInitializeDpc",
    "KeInitializeEvent",
    "KeInitializeInterrupt",
    "KeInitializeMutant",
    "KeInitializeQueue",
    "KeInitializeSemaphore",
    "KeInitializeTimerEx",
    "KeInsertByKeyDeviceQueue",
    "KeInsertDeviceQueue",
    "KeInsertHeadQueue",
    "KeInsertQueue",
    "KeInsertQueueApc",
    "KeInsertQueueDpc",
    "KeInterruptTime",
    "KeIsExecutingDpc",
    "KeLeaveCriticalRegion",
    "KePulseEvent",
    "KeQueryBasePriorityThread",
    "KeQueryInterruptTime",
    "KeQueryPerformanceCounter",
    "KeQueryPerformanceFrequency",
    "KeQuerySystemTime",
    "KeRaiseIrqlToDpcLevel",
    "KeRaiseIrqlToSynchLevel",
    "KeReleaseMutant",
    "KeReleaseSemaphore",
    "KeRemoveByKeyDeviceQueue",
    "KeRemoveDeviceQueue",
    "KeRemoveEntryDeviceQueue",
    "KeRemoveQueue",
    "KeRemoveQueueDpc",
    "KeResetEvent",
    "KeRestoreFloatingPointState",
    "KeResumeThread",
    "KeRundownQueue",
    "KeSaveFloatingPointState",
    "KeSetBasePriorityThread",
    "KeSetDisableBoostThread",
    "KeSetEvent",
    "KeSetEventBoostPriority",
    "KeSetPriorityProcess",
    "KeSetPriorityThread",
    "KeSetTimer",
    "KeSetTimerEx",
    "KeStallExecutionProcessor",
    "KeSuspendThread",
    "KeSynchronizeExecution",
    "KeSystemTime",
    "KeTestAlertThread",
    "KeTickCount",
    "KeTimeIncrement",
    "KeWaitForMultipleObjects",
    "KeWaitForSingleObject",
    "KfRaiseIrql",
    "KfLowerIrql",
    "KiBugCheckData",
    "KiUnlockDispatcherDatabase",
    "LaunchDataPage",
    "MmAllocateContiguousMemory",
    "MmAllocateContiguousMemoryEx",
    "MmAllocateSystemMemory",
    "MmClaimGpuInstanceMemory",
    "MmCreateKernelStack",
    "MmDeleteKernelStack",
    "MmFreeContiguousMemory",
    "MmFreeSystemMemory",
    "MmGetPhysicalAddress",
    "MmIsAddressValid",
    "MmLockUnlockBufferPages",
    "MmLockUnlockPhysicalPage",
    "MmMapIoSpace",
    "MmPersistContiguousMemory",
    "MmQueryAddressProtect",
    "MmQueryAllocationSize",
    "MmQueryStatistics",
    "MmSetAddressProtect",
    "MmUnmapIoSpace",
    "NtAllocateVirtualMemory",
    "NtCancelTimer",
    "NtClearEvent",
    "NtClose",
    "NtCreateDirectoryObject",
    "NtCreateEvent",
    "NtCreateFile",
    "NtCreateIoCompletion",
    "NtCreateMutant",
    "NtCreateSemaphore",
    "NtCreateTimer",
    "NtDeleteFile",
    "NtDeviceIoControlFile",
    "NtDuplicateObject",
    "NtFlushBuffersFile",
    "NtFreeVirtualMemory",
    "NtFsControlFile",
    "NtOpenDirectoryObject",
    "NtOpenFile",
    "NtOpenSymbolicLinkObject",
    "NtProtectVirtualMemory",
    "NtPulseEvent",
    "NtQueueApcThread",
    "NtQueryDirectoryFile",
    "NtQueryDirectoryObject",
    "NtQueryEvent",
    "NtQueryFullAttributesFile",
    "NtQueryInformationFile",
    "NtQueryIoCompletion",
    "NtQueryMutant",
    "NtQuerySemaphore",
    "NtQuerySymbolicLinkObject",
    "NtQueryTimer",
    "NtQueryVirtualMemory",
    "NtQueryVolumeInformationFile",
    "NtReadFile",
    "NtReadFileScatter",
    "NtReleaseMutant",
    "NtReleaseSemaphore",
    "NtRemoveIoCompletion",
    "NtResumeThread",
    "NtSetEvent",
    "NtSetInformationFile",
    "NtSetIoCompletion",
    "NtSetSystemTime",
    "NtSetTimerEx",
    "NtSignalAndWaitForSingleObjectEx",
    "NtSuspendThread",
    "NtUserIoApcDispatcher",
    "NtWaitForSingleObject",
    "NtWaitForSingleObjectEx",
    "NtWaitForMultipleObjectsEx",
    "NtWriteFile",
    "NtWriteFileGather",
    "NtYieldExecution",
    "ObCreateObject",
    "ObDirectoryObjectType",
    "ObInsertObject",
    "ObMakeTemporaryObject",
    "ObOpenObjectByName",
    "ObOpenObjectByPointer",
    "ObpObjectHandleTable",
    "ObReferenceObjectByHandle",
    "ObReferenceObjectByName",
    "ObReferenceObjectByPointer",
    "ObSymbolicLinkObjectType",
    "ObfDereferenceObject",
    "ObfReferenceObject",
    "PhyGetLinkState",
    "PhyInitialize",
    "PsCreateSystemThread",
    "PsCreateSystemThreadEx",
    "PsQueryStatistics",
    "PsSetCreateThreadNotifyRoutine",
    "PsTerminateSystemThread",
    "PsThreadObjectType",
    "RtlAnsiStringToUnicodeString",
    "RtlAppendStringToString",
    "RtlAppendUnicodeStringToString",
    "RtlAppendUnicodeToString",
    "RtlAssert",
    "RtlCaptureContext",
    "RtlCaptureStackBackTrace",
    "RtlCharToInteger",
    "RtlCompareMemory",
    "RtlCompareMemoryUlong",
    "RtlCompareString",
    "RtlCompareUnicodeString",
    "RtlCopyString",
    "RtlCopyUnicodeString",
    "RtlCreateUnicodeString",
    "RtlDowncaseUnicodeChar",
    "RtlDowncaseUnicodeString",
    "RtlEnterCriticalSection",
    "RtlEnterCriticalSectionAndRegion",
    "RtlEqualString",
    "RtlEqualUnicodeString",
    "RtlExtendedIntegerMultiply",
    "RtlExtendedLargeIntegerDivide",
    "RtlExtendedMagicDivide",
    "RtlFillMemory",
    "RtlFillMemoryUlong",
    "RtlFreeAnsiString",
    "RtlFreeUnicodeString",
    "RtlGetCallersAddress",
    "RtlInitAnsiString",
    "RtlInitUnicodeString",
    "RtlInitializeCriticalSection",
    "RtlIntegerToChar",
    "RtlIntegerToUnicodeString",
    "RtlLeaveCriticalSection",
    "RtlLeaveCriticalSectionAndRegion",
    "RtlLowerChar",
    "RtlMapGenericMask",
    "RtlMoveMemory",
    "RtlMultiByteToUnicodeN",
    "RtlMultiByteToUnicodeSize",
    "RtlNtStatusToDosError",
    "RtlRaiseException",
    "RtlRaiseStatus",
    "RtlTimeFieldsToTime",
    "RtlTimeToTimeFields",
    "RtlTryEnterCriticalSection",
    "RtlUlongByteSwap",
    "RtlUnicodeStringToAnsiString",
    "RtlUnicodeStringToInteger",
    "RtlUnicodeToMultiByteN",
    "RtlUnicodeToMultiByteSize",
    "RtlUnwind",
    "RtlUpcaseUnicodeChar",
    "RtlUpcaseUnicodeString",
    "RtlUpcaseUnicodeToMultiByteN",
    "RtlUpperChar",
    "RtlUpperString",
    "RtlUshortByteSwap",
    "RtlWalkFrameChain",
    "RtlZeroMemory",
    "XboxEEPROMKey",
    "XboxHardwareInfo",
    "XboxHDKey",
    "XboxKrnlVersion",
    "XboxSignatureKey",
    "XeImageFileName",
    "XeLoadSection",
    "XeUnloadSection",
    "READ_PORT_BUFFER_UCHAR",
    "READ_PORT_BUFFER_USHORT",
    "READ_PORT_BUFFER_ULONG",
    "WRITE_PORT_BUFFER_UCHAR",
    "WRITE_PORT_BUFFER_USHORT",
    "WRITE_PORT_BUFFER_ULONG",
    "XcSHAInit",
    "XcSHAUpdate",
    "XcSHAFinal",
    "XcRC4Key",
    "XcRC4Crypt",
    "XcHMAC",
    "XcPKEncPublic",
    "XcPKDecPrivate",
    "XcPKGetKeyLen",
    "XcVerifyPKCS1Signature",
    "XcModExp",
    "XcDESKeyParity",
    "XcKeyTable",
    "XcBlockCrypt",
    "XcBlockCryptCBC",
    "XcCryptService",
    "XcUpdateCrypto",
    "RtlRip",
    "XboxLANKey",
    "XboxAlternateSignatureKeys",
    "XePublicKeyData",
    "HalBootSMCVideoMode",
    "IdexChannelObject",
    "HalIsResetOrShutdownPending",
    "IoMarkIrpMustComplete",
    "HalInitiateShutdown",
    "RtlSnprintf",
    "RtlSprintf",
    "RtlVsnprintf",
    "RtlVsprintf",
    "HalEnableSecureTrayEject",
    "HalWriteSMCScratchRegister",
    "Undefined",
    "Undefined",
    "Undefined",
    "XProfpControl",
    "XProfpGetData",
    "IrtClientInitFast",
    "IrtSweep",
    "MmDbgAllocateMemory",
    "MmDbgFreeMemory",
    "MmDbgQueryAvailablePages",
    "MmDbgReleaseAddress",
    "MmDbgWriteCheck",
};

UCHAR XePublicKeyData[] = {
    0x52,0x53,0x41,0x31,0x08,0x01,0x00,0x00,
    0x00,0x08,0x00,0x00,0xFF,0x00,0x00,0x00,
    0x01,0x00,0x01,0x00,0x9B,0x83,0xD4,0xD5,
    0xDE,0x16,0x25,0x8E,0xE5,0x15,0xF2,0x18,
    0x9D,0x19,0x1C,0xF8,0xFE,0x91,0xA5,0x83,
    0xAE,0xA5,0xA8,0x95,0x3F,0x01,0xB2,0xC9,
    0x34,0xFB,0xC7,0x51,0x2D,0xAC,0xFF,0x38,
    0xE6,0xB6,0x7B,0x08,0x4A,0xDF,0x98,0xA3,
    0xFD,0x31,0x81,0xBF,0xAA,0xD1,0x62,0x58,
    0xC0,0x6C,0x8F,0x8E,0xCD,0x96,0xCE,0x6D,
    0x03,0x44,0x59,0x93,0xCE,0xEA,0x8D,0xF4,
    0xD4,0x6F,0x6F,0x34,0x5D,0x50,0xF1,0xAE,
    0x99,0x7F,0x1D,0x92,0x15,0xF3,0x6B,0xDB,
    0xF9,0x95,0x8B,0x3F,0x54,0xAD,0x37,0xB5,
    0x4F,0x0A,0x58,0x7B,0x48,0xA2,0x9F,0x9E,
    0xA3,0x16,0xC8,0xBD,0x37,0xDA,0x9A,0x37,
    0xE6,0x3F,0x10,0x1B,0xA8,0x4F,0xA3,0x14,
    0xFA,0xBE,0x12,0xFB,0xD7,0x19,0x4C,0xED,
    0xAD,0xA2,0x95,0x8F,0x39,0x8C,0xC4,0x69,
    0x0F,0x7D,0xB8,0x84,0x0A,0x99,0x5C,0x53,
    0x2F,0xDE,0xF2,0x1B,0xC5,0x1D,0x4C,0x43,
    0x3C,0x97,0xA7,0xBA,0x8F,0xC3,0x22,0x67,
    0x39,0xC2,0x62,0x74,0x3A,0x0C,0xB5,0x57,
    0x01,0x3A,0x67,0xC6,0xDE,0x0C,0x0B,0xF6,
    0x08,0x01,0x64,0xDB,0xBD,0x81,0xE4,0xDC,
    0x09,0x2E,0xD0,0xF1,0xD0,0xD6,0x1E,0xBA,
    0x38,0x36,0xF4,0x4A,0xDD,0xCA,0x39,0xEB,
    0x76,0xCF,0x95,0xDC,0x48,0x4C,0xF2,0x43,
    0x8C,0xD9,0x44,0x26,0x7A,0x9E,0xEB,0x99,
    0xA3,0xD8,0xFB,0x30,0xA8,0x14,0x42,0x82,
    0x8D,0xB4,0x31,0xB3,0x1A,0xD5,0x2B,0xF6,
    0x32,0xBC,0x62,0xC0,0xFE,0x81,0x20,0x49,
    0xE7,0xF7,0x58,0x2F,0x2D,0xA6,0x1B,0x41,
    0x62,0xC7,0xE0,0x32,0x02,0x5D,0x82,0xEC,
    0xA3,0xE4,0x6C,0x9B,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

SIZE_T InputFileSize;

DWORD
VerifyImageHeaderEncryptedDigest(
    PXBEIMAGE_HEADER ImageHeader
    )
/*++

Routine Description:

    This routine verifies that the encrypted header digest stored in the image
    header matches the digest of the data in the rest of the image headers.

Arguments:

    ImageHeader - Pointer to XBE image header.

Return Value:

    Error code of operation.

--*/
{
    PUCHAR PublicKeyData;
    PUCHAR Workspace;
    ULONG HeaderDigestLength;
    UCHAR HeaderDigest[XC_DIGEST_LEN];
    BOOLEAN Verified;

    PublicKeyData = XePublicKeyData;

    //
    // Allocate a workspace to do the digest verification.
    //

    Workspace = (PUCHAR)LocalAlloc(LPTR, XCCalcKeyLen(PublicKeyData) * 2);

    if (Workspace == NULL) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    //
    // The encrypted header digest starts at the field after the encrypted
    // header field in the main image header.
    //

    HeaderDigestLength = ImageHeader->SizeOfHeaders -
        FIELD_OFFSET(XBEIMAGE_HEADER, BaseAddress);

    //
    // Calculate the SHA1 digest for the headers.
    //

    XCCalcDigest((PUCHAR)&ImageHeader->BaseAddress, HeaderDigestLength,
        HeaderDigest);

    //
    // Verify that the SHA1 digest matches the encrypted header digest.
    //

    Verified = XCVerifyDigest(ImageHeader->EncryptedDigest, PublicKeyData,
        Workspace, HeaderDigest);

    LocalFree(Workspace);

    if (Verified) {
        return ERROR_SUCCESS;
    } else {
        return ERROR_TRUST_FAILURE;
    }
}

LPVOID
GetDataFromVirtualAddress(
    PXBEIMAGE_HEADER ImageHeader,
    PVOID VirtualAddress,
    ULONG NumberOfBytes
    )
{
    PXBEIMAGE_SECTION ImageSection;
    ULONG FileByteOffset;
    ULONG Index;

    //
    // Return a pointer to data in the image header if the virtual address
    // resides in the image header.
    //

    if (((ULONG)ImageHeader->BaseAddress <= (ULONG)VirtualAddress) &&
        ((ULONG)VirtualAddress + NumberOfBytes <= (ULONG)ImageHeader->BaseAddress +
            ImageHeader->SizeOfHeaders)) {

        FileByteOffset = (ULONG)VirtualAddress - (ULONG)ImageHeader->BaseAddress;

        if (FileByteOffset >= InputFileSize) {
            return NULL;
        }

        return (LPBYTE)ImageHeader + FileByteOffset;
    }

    //
    // Verify that the image has sections.
    //

    if (ImageHeader->NumberOfSections == 0) {
        return NULL;
    }

    //
    // Don't return anything if the section headers start past the end of the
    // file.
    //

    if (((ULONG)ImageHeader->SectionHeaders - (ULONG)ImageHeader->BaseAddress) >=
        InputFileSize) {
        return NULL;
    }

    //
    // Don't return anything if the section headers end past the end of the
    // file.
    //

    if (((ULONG)ImageHeader->SectionHeaders - (ULONG)ImageHeader->BaseAddress +
        (ImageHeader->NumberOfSections * sizeof(XBEIMAGE_SECTION))) >= InputFileSize) {
        return NULL;
    }

    //
    // Loop over all of the sections and return the address of the data that
    // maps the requested virtual address and size.
    //

    ImageSection = (PXBEIMAGE_SECTION)((LPBYTE)ImageHeader +
        (ULONG)ImageHeader->SectionHeaders - (ULONG)ImageHeader->BaseAddress);

    for (Index = 0; Index < ImageHeader->NumberOfSections; Index++) {

        if ((ImageSection->VirtualAddress <= (ULONG)VirtualAddress) &&
            ((ULONG)VirtualAddress + NumberOfBytes <= ImageSection->VirtualAddress +
                ImageSection->SizeOfRawData)) {

            FileByteOffset = ImageSection->PointerToRawData +
                ((ULONG)VirtualAddress - ImageSection->VirtualAddress);

            if (FileByteOffset >= InputFileSize) {
                return NULL;
            }

            return (LPBYTE)ImageHeader + FileByteOffset;
        }

        ImageSection++;
    }

    return NULL;
}

void usage(void)
{
    fprintf(stderr, "Usage:\txbeimport <XBE file>\n");
    exit(-1);
}

void __cdecl main(int argc, char* argv[])
{
    DWORD ErrorCode;
    PCHAR pszFileName = NULL;
    HANDLE MapHandle = NULL;
    HANDLE FileHandle = INVALID_HANDLE_VALUE;
    PXBEIMAGE_HEADER XbeHeader = NULL;
    ULONG_PTR XboxKernelThunkData;
    PIMAGE_THUNK_DATA ImageThunkData;
    ULONG OrdinalNumber;
    ULONG *Key;

    if (argc != 2) {
        usage();
    }

    pszFileName = argv[1];

    if (!pszFileName) {
        usage();
    }

    //
    // Open a input XBE file and create a memory-mapped file
    //

    FileHandle = CreateFile(pszFileName, GENERIC_READ, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (FileHandle == INVALID_HANDLE_VALUE) {
        goto cleanup;
    }

    InputFileSize = GetFileSize(FileHandle, NULL);

    MapHandle = CreateFileMapping(FileHandle, NULL, PAGE_READONLY, 0, 0,
        NULL);

    if (MapHandle == NULL) {
        goto cleanup;
    }

    XbeHeader = (PXBEIMAGE_HEADER)MapViewOfFile(MapHandle, FILE_MAP_READ,
        0, 0, 0);

    if (XbeHeader == NULL) {
        goto cleanup;
    }

    //
    // Check for a valid XBE image signature
    //

    if ((XbeHeader->Signature != XBEIMAGE_SIGNATURE) ||
        (XbeHeader->SizeOfHeaders <= sizeof(XBEIMAGE_HEADER)) ||
        (XbeHeader->SizeOfHeaders > XbeHeader->SizeOfImage) ||
        (XbeHeader->BaseAddress != (PVOID)XBEIMAGE_STANDARD_BASE_ADDRESS)) {

        fprintf(stderr, "%s is not a valid XBE image\n", argv[1]);
        SetLastError(ERROR_SUCCESS);
        goto cleanup;
    }

    if (VerifyImageHeaderEncryptedDigest(XbeHeader) != ERROR_SUCCESS) {
        fprintf(stderr, "Error: \"%s\" is not signed with DEVKIT key\n", pszFileName);
        SetLastError(ERROR_SUCCESS);
        goto cleanup;
    }

    Key = (ULONG*)&XePublicKeyData[128];
    XboxKernelThunkData = (ULONG_PTR)XbeHeader->XboxKernelThunkData;
    XboxKernelThunkData ^= Key[1] ^ Key[2];
    XboxKernelThunkData = (ULONG_PTR)GetDataFromVirtualAddress(XbeHeader,
                                        (PVOID)XboxKernelThunkData, 4);

    fprintf(stdout, "%s:\n\n", pszFileName);
    fprintf(stdout, "Ordinal  Function Name\n");
    fprintf(stdout, "-------  -------------\n");

    ImageThunkData = (PIMAGE_THUNK_DATA)XboxKernelThunkData;

    while (ImageThunkData->u1.Ordinal != 0) {

        OrdinalNumber = IMAGE_ORDINAL(ImageThunkData->u1.Ordinal);

        if (OrdinalNumber >= ARRAYSIZE(KernelExports)) {
            fprintf(stderr, "Cannot find import ordinal %d\n", OrdinalNumber);
            break;
        }

        fprintf(stdout, "    %3d  %s\n", OrdinalNumber,
            KernelExports[OrdinalNumber]);
        ImageThunkData++;
    }

cleanup:

    ErrorCode = GetLastError();

    if (ErrorCode != ERROR_SUCCESS) {

        char MsgBuf[256];

        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            MsgBuf, sizeof(MsgBuf), NULL);
        fprintf(stderr, "%s\n", MsgBuf);
    }

    if (XbeHeader) {
        UnmapViewOfFile(XbeHeader);
    }

    if (MapHandle) {
        CloseHandle(MapHandle);
    }

    if (FileHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(FileHandle);
    }
}
