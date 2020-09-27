/*++

Copyright (c) Microsoft Corporation.  All right reserved.

Module Name:

    certspy.cpp

Abstract:

    This module contains main entry-point of certspy debugger extension.

--*/

#include "certspyp.h"

//
// Global notification session for module load notification from xbdm
//

PDMN_SESSION DmSession;

//
// Define bitmap that contains flag to intercept kernel thunk during startup
//

BYTE KernelExportFlags[KernelExportSize];

//
// Define array that contains intercepted kernel thunk index by ordinal
//

PORDINAL_THUNK InterceptThunks[KernelExportSize];

//
// Define array that contains image thunk data index by ordinal
//

PIMAGE_THUNK_DATA ImageThunks[KernelExportSize];

//
// Flag indicates that the XBE has been loaded by the kernel
//

BOOLEAN CertSpyXbeHasBeenLoaded;

VOID
FASTCALL
CertSpyLogKernelCall(
    IN ULONG  OrdinalNumber,
    IN PULONG StackFrame
    )
{
    PUCHAR Params;
    BOOLEAN DecodeParams = FALSE;
    POBJECT_STRING ObjectString;
    const KERNEL_EXPORT_API* Api = &KernelExports[OrdinalNumber];

    Params = (PUCHAR)Api->Parameters;

    Params = (PUCHAR)Api->Parameters;
    StackFrame++; // Skip over return address

    const SIZE_T BufferSize = 1024;
    SIZE_T BytesLeft = BufferSize;
    SIZE_T CharCount;
    CHAR TextBuffer[BufferSize];
    CHAR* Buffer = TextBuffer;

    CharCount = _snprintf(Buffer, BytesLeft, "certspy!kcall %d", OrdinalNumber);

    Buffer += CharCount;
    BytesLeft -= CharCount;

    while (*Params && BytesLeft) {

        CharCount = 0;

        switch (*Params) {

        case PT_PSTRING:
            CharCount = _snprintf(Buffer, BytesLeft, " %Z", *StackFrame);
            break;

        case PT_POBJATTR:
            ObjectString = ((POBJECT_ATTRIBUTES)*StackFrame)->ObjectName;
            CharCount = _snprintf(Buffer, BytesLeft, " %Z", ObjectString);
            break;

        case PT_LONGLONG:
            CharCount = _snprintf(Buffer, BytesLeft, " 0x%I64X", *StackFrame);
            StackFrame++;
            break;

        default:
            CharCount = _snprintf(Buffer, BytesLeft, " 0x%X", *StackFrame);
        }

        Buffer += CharCount;
        StackFrame++;
        Params++;

        if (BytesLeft > CharCount) {
            BytesLeft -= CharCount;
        } else {
            BytesLeft = 0;
        }
    }

    TextBuffer[BufferSize - 1] = 0;
    DmSendNotificationString(TextBuffer);
}

PORDINAL_THUNK
CertSpyBuildKernelThunk(
    IN ULONG OrdinalNumber,
    IN ULONG_PTR FunctionAddress
    )
{
    PORDINAL_THUNK Thunk;

    //
    // Allocate memory out of executable region.  Note that executeable
    // region are only virtual address and debug region.
    //

    Thunk = (PORDINAL_THUNK)DmAllocatePool(sizeof(*Thunk));

    if (Thunk) {

        //
        // Build the thunk by manually writing x86 op-code
        //

        Thunk->__pushad = 0x60;
        Thunk->__lea_edx_esp_plus_32 = 0x2024548D;
        Thunk->__mov_ecx_immediate = 0xB9;
        Thunk->ImmediateValue = OrdinalNumber;
        Thunk->__call_LogFunction = 0xE8;
        Thunk->OffsetOfLogFunction = (ULONG_PTR)CertSpyLogKernelCall - \
            (ULONG_PTR)&Thunk->__popad;
        Thunk->__popad = 0x61;
        Thunk->__jmp_far = 0xE9;
        Thunk->ImmediateJmpAddress = (ULONG_PTR)FunctionAddress - \
            (ULONG_PTR)&Thunk->ImmediateJmpAddress - \
            sizeof(Thunk->ImmediateJmpAddress);
    }

    return Thunk;
}

ULONG
CertSpyLookupOrdinalFromExportDirectory(
    IN ULONG ImageBase,
    IN PIMAGE_EXPORT_DIRECTORY ExportDirectory,
    IN ULONG AddressOfFunction
    )
{
    ULONG Entry;
    PULONG AddressOfFunctions;

    AddressOfFunctions = (PULONG)(ImageBase + \
        (ULONG)ExportDirectory->AddressOfFunctions);

    for (Entry=0; Entry<ExportDirectory->NumberOfFunctions; Entry++) {
        if (AddressOfFunctions[Entry] == AddressOfFunction - \
            (ULONG_PTR)PsNtosImageBase) {
            return Entry + 1;
        }
    }

    return ~0UL;
}

NTSTATUS
CertSpyHookKernelImportOrdinal(
    IN ULONG OrdinalNumber
    )
{
    NTSTATUS Status;
    PIMAGE_THUNK_DATA ImageThunkData = NULL;
    PORDINAL_THUNK Thunk;

    if (OrdinalNumber < KernelExportSize) {
        ImageThunkData = ImageThunks[OrdinalNumber];
    }

    if (!ImageThunkData) {

        if (CertSpyXbeHasBeenLoaded) {
            return STATUS_ORDINAL_NOT_FOUND;
        }

        //
        // Add to defer list if image hasn't been loaded into memory
        //

        KernelExportFlags[OrdinalNumber] |= KEF_INTERCEPT_AT_STARTUP;
        return STATUS_PENDING;
    }

    if (InterceptThunks[OrdinalNumber]) {
        return STATUS_SUCCESS;
    }

    Thunk = CertSpyBuildKernelThunk(OrdinalNumber, ImageThunkData->u1.Function);

    if (!Thunk) {
        return STATUS_NO_MEMORY;
    }

    //
    // Mark image thunk data read/write
    //

    PVOID BaseAddress = (PVOID)&ImageThunkData->u1.Function;
    SIZE_T RegionSize = sizeof(ImageThunkData->u1.Function);
    ULONG OldProtect;

    Status = NtProtectVirtualMemory(&BaseAddress, &RegionSize,
                PAGE_READWRITE, &OldProtect);

    if (NT_SUCCESS(Status)) {
        ImageThunkData->u1.Function = (ULONG_PTR)Thunk;
        InterceptThunks[OrdinalNumber] = Thunk;
        CertSpyFlushICache();
    }

    return Status;
}

NTSTATUS
CertSpyUnhookKernelImportOrdinal(
    IN ULONG OrdinalNumber
    )
{
    NTSTATUS Status;
    PIMAGE_THUNK_DATA ImageThunkData = NULL;
    PORDINAL_THUNK Thunk;

    if (OrdinalNumber < KernelExportSize) {
        ImageThunkData = ImageThunks[OrdinalNumber];
    }

    if (!ImageThunkData) {
        return STATUS_ORDINAL_NOT_FOUND;
    }

    Thunk = InterceptThunks[OrdinalNumber];

    if (!Thunk) {
        return STATUS_SUCCESS;
    }

    //
    // Mark image thunk data read/write
    //

    PVOID BaseAddress = (PVOID)&ImageThunkData->u1.Function;
    SIZE_T RegionSize = sizeof(ImageThunkData->u1.Function);
    ULONG OldProtect;

    Status = NtProtectVirtualMemory(&BaseAddress, &RegionSize,
                PAGE_READWRITE, &OldProtect);

    if (NT_SUCCESS(Status)) {
        ImageThunkData->u1.Function = (ULONG_PTR)Thunk->ImmediateJmpAddress + \
            (ULONG_PTR)&Thunk->ImmediateJmpAddress + sizeof(Thunk->ImmediateJmpAddress);
        CertSpyFlushICache();
        InterceptThunks[OrdinalNumber] = NULL;
        DmFreePool(Thunk);
    }

    return Status;
}

NTSTATUS
CertSpyHookKernelImportThunks(
    IN PIMAGE_THUNK_DATA ImageThunkData
    )
{
    PIMAGE_EXPORT_DIRECTORY ExportDirectory;
    ULONG ExportDirectorySize;
    PULONG AddressOfFunctions;
    ULONG OrdinalNumber;
    PORDINAL_THUNK Thunk;

    //
    // Lookup the export directory from the export executable.
    //

    ExportDirectory =
        (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData(PsNtosImageBase,
        TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ExportDirectorySize);

    if (ExportDirectory == NULL) {
        return STATUS_ORDINAL_NOT_FOUND;
    }

    ASSERT(ExportDirectory->NumberOfFunctions == KernelExportSize-1);

    AddressOfFunctions = (PULONG)((ULONG)PsNtosImageBase +
        (ULONG)ExportDirectory->AddressOfFunctions);

    //
    // Loop over the image thunks and look up the ordinal number
    //

    while (ImageThunkData->u1.Function != 0) {

        OrdinalNumber = CertSpyLookupOrdinalFromExportDirectory(
                            (ULONG)PsNtosImageBase, ExportDirectory,
                            ImageThunkData->u1.Function);

        if (OrdinalNumber != ~0UL && \
            OrdinalNumber < KernelExportSize && \
            KernelExports[OrdinalNumber].FunctionName) {

            if (ImageThunks[OrdinalNumber] == NULL) {
                ImageThunks[OrdinalNumber] = ImageThunkData;
            }

            if ((KernelExportFlags[OrdinalNumber] & KEF_INTERCEPT_AT_STARTUP) && \
                InterceptThunks[OrdinalNumber] == NULL) {

                Thunk = CertSpyBuildKernelThunk(OrdinalNumber, ImageThunkData->u1.Function);

                if (Thunk) {
                    ImageThunkData->u1.Function = (ULONG_PTR)Thunk;
                    InterceptThunks[OrdinalNumber] = Thunk;
                }
            }
        }

        ImageThunkData++;
    }

    CertSpyFlushICache();

    return STATUS_SUCCESS;
}

DWORD
NTAPI
CertSpyNotificationCallback(
    ULONG Notification,
    DWORD Parameter
    )
{
    PDMN_MODLOAD Module = (PDMN_MODLOAD)Parameter;
    PXBEIMAGE_HEADER XbeImageHeader;

    if (Notification != DM_MODLOAD) {
        return 0;
    }

    //
    // There is no way to distinguish between XBE and DLL from module-load
    // notification.  One easy way to see if the XBE has just been loaded is
    // to look at XBEIMAGE_HEADER in memory but SEH isn't working at this
    // point because the interrupts are disabled. We have to check the XBE
    // file name first before trying to read the XBE image header.
    //

    if (XeImageFileName->Length == 0 || XeImageFileName->Buffer == NULL) {
        return 0;
    }

    //
    // Locate and validate XBE image header and check for kernel import thunks
    //

    XbeImageHeader = (PXBEIMAGE_HEADER)XBEIMAGE_STANDARD_BASE_ADDRESS;

    if (XbeImageHeader->Signature == XBEIMAGE_SIGNATURE &&
        XbeImageHeader->XboxKernelThunkData != NULL &&
        CertSpyXbeHasBeenLoaded == FALSE) {
        CertSpyXbeHasBeenLoaded = TRUE;
        CertSpyHookKernelImportThunks(XbeImageHeader->XboxKernelThunkData);
    }

    return 0;
}

VOID
NTAPI
DxtEntry(
    ULONG *pfUnload
    )
{
    ULONG Result;
    HRESULT hr;

    hr = DmOpenNotificationSession(DM_PERSISTENT, &DmSession);

    if (SUCCEEDED(hr)) {
        hr = DmNotify(DmSession, DM_MODLOAD,
                (PDM_NOTIFY_FUNCTION)CertSpyNotificationCallback);
    }

    if (SUCCEEDED(hr)) {
        hr = DmRegisterCommandProcessor("certspy", CertSpyCommandProcessor);
    }

    if (SUCCEEDED(hr)) {
        *pfUnload = FALSE;
    } else {
        *pfUnload = TRUE;
    }
}
