/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    sysload.c

Abstract:

    This module contains the code to load DLLs into the system portion of
    the address space and calls the DLL at its initialization entry point.

--*/

#include "mtldrp.h"

NTSTATUS
MiSnapThunk(
    IN PVOID DllBase,
    IN PVOID ImageBase,
    IN PIMAGE_THUNK_DATA NameThunk,
    OUT PIMAGE_THUNK_DATA AddrThunk,
    IN PIMAGE_EXPORT_DIRECTORY ExportDirectory,
    IN ULONG ExportSize,
    IN BOOLEAN SnapForwarder
    );

VOID
LdrInitializeSystemLoader(
    PVOID KernelLoaderBlock
    )
{
    ULONG TlsSize;

    ASSERT( KernelLoaderBlock );

    //
    // Initialize the global loader lists and locks.
    //

    XeLdrImage = (PLDR_CURRENT_IMAGE)KernelLoaderBlock;

    TlsSize = (_tls_used.EndAddressOfRawData -
        _tls_used.StartAddressOfRawData) + _tls_used.SizeOfZeroFill;

    //
    // The base of the TLS section must be 16-byte aligned.  We know
    // that fs:StackBase is 16-byte aligned, ensuring that TlsSize is
    // 16-byte aligned is sufficient
    //
    TlsSize = (TlsSize + 15) & ~15;

    //
    // Add four bytes for the index pointer
    //
    TlsSize += 4;
    *(PULONG)(_tls_used.AddressOfIndex) = (int)TlsSize / -4;
}

NTSTATUS
MiLoadSystemImage(
    IN PVOID ImageBuffer,
    IN PSTRING ImageFileName,
    OUT PVOID *EntryPoint OPTIONAL
    )
/*++

Routine Description:

    This routine reads the image pages from the specified section into
    the system and returns the address of the DLL's header.

    At successful completion, the Section is referenced so it remains
    until the system image is unloaded.

Arguments:

    ImageFileName - Supplies the full path name (including the image name)
                    of the image to load.

    ImageHandle - Returns an opaque pointer to the referenced section object
                  of the image that was loaded.

    ImageBaseAddress - Returns the image base within the system.

Return Value:

    Status of the load operation.

--*/
{
    NTSTATUS status;
    PCHAR BaseAddress;
    PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
    ULONG ImportDescriptorSize;
    ULONG NumberOfImportDescriptors;
    PIMAGE_EXPORT_DIRECTORY ExportDirectory;
    ULONG ExportDirectorySize;
    PIMAGE_THUNK_DATA OriginalThunk;
    PIMAGE_THUNK_DATA Thunk;
    ULONG DebugDirectorySize;

    BaseAddress = (PCHAR)ImageBuffer;

    //
    // Relocate the image at its new base address.
    //

    try {
        status = (NTSTATUS)LdrRelocateImage(BaseAddress, "SYSLDR",
            (ULONG)STATUS_SUCCESS, (ULONG)STATUS_CONFLICTING_ADDRESSES,
            (ULONG)STATUS_INVALID_IMAGE_FORMAT);
    } except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Bind to the images from the module's import table.
    //

    ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)RtlImageDirectoryEntryToData(
        BaseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ImportDescriptorSize);

    if (ImportDescriptor != NULL) {

        //
        // Count the number of images that we'll need to bind to.
        //

        NumberOfImportDescriptors = 0;

        while (ImportDescriptor->Name != 0 && ImportDescriptor->FirstThunk != 0) {
            NumberOfImportDescriptors++;
            ImportDescriptor++;
        }

        //
        // Only xboxkrnl.exe imports will be binded
        //

        if (NumberOfImportDescriptors != 1) {
            KdPrint(( "LDR: invalid number of import descriptor (%lu)\n",NumberOfImportDescriptors ));
            return STATUS_INVALID_IMAGE_PROTECT;
        }

        ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)RtlImageDirectoryEntryToData(
            BaseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ImportDescriptorSize);

        if (ImportDescriptor->Name != 0 && ImportDescriptor->FirstThunk != 0) {

            //
            // Build the file name to the module.  This path won't be accurate
            // for xboxkrnl.exe and hal.dll, but the above code that checks for
            // already loaded modules ignores the directory so it doesn't
            // matter.
            //

            if ( _stricmp("xboxkrnl.exe", BaseAddress + ImportDescriptor->Name) ) {
                KdPrint(( "LDR: invalid import module (%s)\n", BaseAddress + ImportDescriptor->Name ));
                return STATUS_INVALID_IMAGE_PROTECT;
            }

            //
            // Snap the import thunk to the export directory.
            //

            ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData(
                PsNtosImageBase, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT,
                &ExportDirectorySize);

            if (ExportDirectory == NULL) {
                status = STATUS_DRIVER_ENTRYPOINT_NOT_FOUND;
                goto CleanupAndExit;
            }

            OriginalThunk = (PIMAGE_THUNK_DATA)(BaseAddress +
                ImportDescriptor->OriginalFirstThunk);
            Thunk = (PIMAGE_THUNK_DATA)(BaseAddress +
                ImportDescriptor->FirstThunk);

            while (OriginalThunk->u1.AddressOfData != 0) {

                status = MiSnapThunk(PsNtosImageBase, BaseAddress,
                    OriginalThunk, Thunk, ExportDirectory, ExportDirectorySize,
                    FALSE);

                if (!NT_SUCCESS(status)) {
                    KdPrint(("MiLoadSystemImage: %Z couldn't bind to export from %s\n",
                        &ImageFileName, BaseAddress + ImportDescriptor->Name));
                    goto CleanupAndExit;
                }

                OriginalThunk++;
                Thunk++;
            }
        }
    }

    if (ARGUMENT_PRESENT(EntryPoint)) {
        *EntryPoint = BaseAddress + \
            RtlImageNtHeader(BaseAddress)->OptionalHeader.AddressOfEntryPoint;
    }

    //
    // Notify the debugger of the newly loaded module.
    //

    if (RtlImageDirectoryEntryToData(BaseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_DEBUG,
        &DebugDirectorySize) != NULL) {

#ifdef DEVKIT

        //
        // Insert loaded module into loaded module list
        //

        PIMAGE_NT_HEADERS Hdr = RtlImageNtHeader(ImageBuffer);

        PLDR_DATA_TABLE_ENTRY Ldr = ExAllocatePoolWithTag( 
            sizeof(LDR_DATA_TABLE_ENTRY) + 
            ImageFileName->MaximumLength*sizeof(WCHAR), 'rdlM' );

        if ( Ldr ) {
            RtlZeroMemory( Ldr, sizeof(LDR_DATA_TABLE_ENTRY) );
            Ldr->DllBase = ImageBuffer;
            Ldr->SizeOfImage = Hdr->OptionalHeader.SizeOfImage;
            Ldr->CheckSum = Hdr->OptionalHeader.CheckSum;
            Ldr->LoadedImports = NULL;
            Ldr->LoadCount = 1;
            Ldr->Flags = LDRP_ENTRY_PROCESSED | LDRP_SYSTEM_MAPPED;
            Ldr->FullDllName.Buffer = (PWSTR)((ULONG_PTR)Ldr + sizeof(LDR_DATA_TABLE_ENTRY));
            Ldr->FullDllName.Length = 0;
            Ldr->FullDllName.MaximumLength = ImageFileName->MaximumLength * sizeof(WCHAR);
            RtlAnsiStringToUnicodeString( &Ldr->FullDllName, ImageFileName, FALSE );
            Ldr->BaseDllName = Ldr->FullDllName;

            ExInterlockedInsertTailList( DmGetCurrentDmi()->LoadedModuleList,
                &Ldr->InLoadOrderLinks );
        }

#endif

        DbgLoadImageSymbols(ImageFileName, BaseAddress, (ULONG_PTR)-1);
    }

    //
    // The module is now successfully loaded.
    //

    status = STATUS_SUCCESS;

CleanupAndExit:

    return status;
}

NTSTATUS
MiSnapThunk(
    IN PVOID DllBase,
    IN PVOID ImageBase,
    IN PIMAGE_THUNK_DATA NameThunk,
    OUT PIMAGE_THUNK_DATA AddrThunk,
    IN PIMAGE_EXPORT_DIRECTORY ExportDirectory,
    IN ULONG ExportSize,
    IN BOOLEAN SnapForwarder
    )
/*++

Routine Description:

    This function snaps a thunk using the specified Export Section data.
    If the section data does not support the thunk, then the thunk is
    partially snapped (Dll field is still non-null, but snap address is
    set).

Arguments:

    DllBase - Base of DLL being snapped to.

    ImageBase - Base of image that contains the thunks to snap.

    Thunk - On input, supplies the thunk to snap.  When successfully
        snapped, the function field is set to point to the address in
        the DLL, and the DLL field is set to NULL.

    ExportDirectory - Supplies the Export Section data from a DLL.

    SnapForwarder - determine if the snap is for a forwarder, and therefore
       Address of Data is already setup.

Return Value:

    STATUS_SUCCESS or STATUS_DRIVER_ENTRYPOINT_NOT_FOUND or
        STATUS_DRIVER_ORDINAL_NOT_FOUND

--*/
{
    BOOLEAN Ordinal;
    USHORT OrdinalNumber;
    PULONG NameTableBase;
    PUSHORT NameOrdinalTableBase;
    PULONG Addr;
    USHORT HintIndex;
    LONG High;
    LONG Low;
    LONG Middle;
    LONG Result;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Determine if snap is by name, or by ordinal
    //

    Ordinal = (BOOLEAN)IMAGE_SNAP_BY_ORDINAL(NameThunk->u1.Ordinal);

    if (Ordinal && !SnapForwarder) {

        OrdinalNumber = (USHORT)(IMAGE_ORDINAL(NameThunk->u1.Ordinal) -
                         ExportDirectory->Base);

    } else {

        //
        // Change AddressOfData from an RVA to a VA.
        //

        if (!SnapForwarder) {
            NameThunk->u1.AddressOfData = (ULONG_PTR)ImageBase + NameThunk->u1.AddressOfData;
        }

        //
        // Lookup Name in NameTable
        //

        NameTableBase = (PULONG)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfNames);
        NameOrdinalTableBase = (PUSHORT)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfNameOrdinals);

        //
        // Before dropping into binary search, see if
        // the hint index results in a successful
        // match. If the hint index is zero, then
        // drop into binary search.
        //

        HintIndex = ((PIMAGE_IMPORT_BY_NAME)NameThunk->u1.AddressOfData)->Hint;
        if ((ULONG)HintIndex < ExportDirectory->NumberOfNames &&
            !strcmp((PSZ)((PIMAGE_IMPORT_BY_NAME)NameThunk->u1.AddressOfData)->Name,
             (PSZ)((PCHAR)DllBase + NameTableBase[HintIndex]))) {
            OrdinalNumber = NameOrdinalTableBase[HintIndex];

        } else {

            //
            // Lookup the import name in the name table using a binary search.
            //

            Low = 0;
            Middle = 0;
            High = ExportDirectory->NumberOfNames - 1;

            while (High >= Low) {

                //
                // Compute the next probe index and compare the import name
                // with the export name entry.
                //

                Middle = (Low + High) >> 1;
                Result = strcmp(&((PIMAGE_IMPORT_BY_NAME)NameThunk->u1.AddressOfData)->Name[0],
                                (PCHAR)((PCHAR)DllBase + NameTableBase[Middle]));

                if (Result < 0) {
                    High = Middle - 1;

                } else if (Result > 0) {
                    Low = Middle + 1;

                } else {
                    break;
                }
            }

            //
            // If the high index is less than the low index, then a matching
            // table entry was not found. Otherwise, get the ordinal number
            // from the ordinal table.
            //

            if (High < Low) {
                return STATUS_DRIVER_ENTRYPOINT_NOT_FOUND;
            } else {
                OrdinalNumber = NameOrdinalTableBase[Middle];
            }
        }
    }

    //
    // If OrdinalNumber is not within the Export Address Table,
    // then DLL does not implement function. Snap to LDRP_BAD_DLL.
    //

    if ((ULONG)OrdinalNumber >= ExportDirectory->NumberOfFunctions) {
        Status = STATUS_DRIVER_ORDINAL_NOT_FOUND;

    } else {

        Addr = (PULONG)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfFunctions);
        *(PULONG_PTR)&AddrThunk->u1.Function = (ULONG_PTR)DllBase + Addr[OrdinalNumber];

        // AddrThunk s/b used from here on.

        Status = STATUS_SUCCESS;

#if DBG
        if ( ((ULONG_PTR)AddrThunk->u1.Function > (ULONG_PTR)ExportDirectory) &&
             ((ULONG_PTR)AddrThunk->u1.Function < ((ULONG_PTR)ExportDirectory + ExportSize)) ) {
            ASSERTMSG( "LDR: forward is not allowed", TRUE );
        }
#endif
    }

    return Status;
}

NTSTATUS
MiSnapTlsDirectory(
    PVOID ImageBase
    )
/*++

Routine Description:

    This routine snaps Thread Local Storage Directory of the caller thread
    (mtldr) to IMAGE_TLS_DIRECTORY of image loaded at specified ImageBase
    address.

Arguments:

    ImageBuffer - Pointer to location where image is loaded in memory

Return Value:

    NT status code

--*/
{
    PBYTE BaseAddress = (PBYTE)ImageBase;
    PIMAGE_TLS_DIRECTORY TlsDirectory;
    ULONG TlsDirectorySize;
    NTSTATUS Status;

    ASSERT( XeLdrImage );

    //
    // We don't have to validate image header if we get this far.
    // MiLoadSystemImage already verified the image format
    //

    TlsDirectory = (PIMAGE_TLS_DIRECTORY)RtlImageDirectoryEntryToData(
        BaseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_TLS, &TlsDirectorySize);

    if ( TlsDirectory ) {
        XeLdrImage->ImageHeader->OptionalHeader.TlsDirectory = (ULONG_PTR)TlsDirectory;
        Status = STATUS_SUCCESS;
    } else {
        Status = STATUS_NOT_FOUND;
    }

    return Status;
}

ULONG
LdrLoadSystemImage(
    IN PVOID ImageBuffer,
    IN PCSTR ImageFileName
    )
/*++

Routine Description:

    This routine loads the image pages from the specified file into the system
    and calls its initialization routine.

Arguments:

    ImageBuffer - Pointer to location where image is loaded in memory

    ImagePathName - Supplies the full path name (including the image name)
                    of the image to load.

Return Value:

    Win32 error code of the load operation
--*/
{
    NTSTATUS Status;
    ULONG (__cdecl *EntryPoint)(PVOID);
    STRING ImageString;

    RtlInitAnsiString( &ImageString, ImageFileName );

    Status = (NTSTATUS)MiLoadSystemImage(
                            ImageBuffer,
                            &ImageString,
                            (PVOID*)&EntryPoint
                            );

    if ( NT_SUCCESS(Status) ) {
        Status = MiSnapTlsDirectory( ImageBuffer );
        if ( !NT_SUCCESS(Status) ) {
            KdPrint(( "LDR: failed to snap TLS directory, image may not run\n" ));
        }
        EntryPoint(XeLdrImage);
    }

    return RtlNtStatusToDosError( Status );
}

