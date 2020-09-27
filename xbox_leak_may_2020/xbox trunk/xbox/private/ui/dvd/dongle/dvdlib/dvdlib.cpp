/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    dvdlib.cpp

Abstract:

    This module contains wrapper routines used by Xbox Dashboard to access
    DVD playback library located in hardware key.

--*/

#include "dvdlibp.h"

#ifdef DEVKIT
#define ON_DISK_DONGLE
#endif

//
// Import the kernel's public key for dongle validation
//

extern "C" PBYTE XePublicKeyData;

//
// Base address of memory containing DVD-decoder binary
//

PVOID DvdCodeBaseAddress;

//
// Minimum version number of DVD key is 1.01
//

#define DVDKEY_MINIMUM_VERSION 0x0101

//
// The image name for ON_DISK_DONGLE build has to be full path since we will
// load the code off the harddisk if we couldn't find the dongle. In free build,
// however, the file name is just only the module name so that the debugger
// could load the symbol.
//

#if DBG
INITIALIZED_OBJECT_STRING( DvdLibFile, "\\Device\\Harddisk0\\Partition2\\dvdkeyd.bin" );
#else
INITIALIZED_OBJECT_STRING( DvdLibFile, "\\Device\\Harddisk0\\Partition2\\dvdkey.bin" );
#endif

NTSTATUS
DvdpSnapThunk(
    IN PVOID DllBase,
    IN PVOID ImageBase,
    IN PIMAGE_THUNK_DATA NameThunk,
    OUT PIMAGE_THUNK_DATA AddrThunk,
    IN PIMAGE_EXPORT_DIRECTORY ExportDirectory,
    IN ULONG ExportSize
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

Return Value:

    STATUS_SUCCESS or STATUS_DRIVER_ENTRYPOINT_NOT_FOUND or
        STATUS_DRIVER_ORDINAL_NOT_FOUND

--*/
{
    BOOLEAN Ordinal;
    USHORT OrdinalNumber;
    PULONG Addr;
    NTSTATUS Status;

    Ordinal = (BOOLEAN)IMAGE_SNAP_BY_ORDINAL(NameThunk->u1.Ordinal);

    if ( !Ordinal ) {
        return STATUS_DRIVER_ORDINAL_NOT_FOUND;
    }

    OrdinalNumber = (USHORT)(IMAGE_ORDINAL(NameThunk->u1.Ordinal) -
                     ExportDirectory->Base);

    if ((ULONG)OrdinalNumber >= ExportDirectory->NumberOfFunctions) {
        Status = STATUS_DRIVER_ORDINAL_NOT_FOUND;
    } else {

        Addr = (PULONG)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfFunctions);
        *(PULONG_PTR)&AddrThunk->u1.Function = (ULONG_PTR)DllBase + Addr[OrdinalNumber];
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
DvdLoadSystemImage(
    IN PVOID ImageBuffer,
    IN PSTRING ImageFileName,
    OUT PVOID *EntryPoint OPTIONAL
    )
/*++

Routine Description:

    This routine fixes up and snap imports/exports thinks for the specified
    image.

Arguments:

    ImageBuffer - Buffer contains image to be loaded

    ImageFileName - Supplies the image name of the image to load.

    EntryPoint - Optional parameter to receive image's entry point

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

    __try {
        status = (NTSTATUS)LdrRelocateImage(BaseAddress, (PUCHAR)"SYSLDR",
            (ULONG)STATUS_SUCCESS, (ULONG)STATUS_CONFLICTING_ADDRESSES,
            (ULONG)STATUS_INVALID_IMAGE_FORMAT);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
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

                status = DvdpSnapThunk(PsNtosImageBase, BaseAddress,
                    OriginalThunk, Thunk, ExportDirectory, ExportDirectorySize );

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
        if (DmGetCurrentDmi() && DmGetCurrentDmi()->LoadedModuleList)
        {
            //
            // Insert loaded module into loaded module list
            //

            USHORT Length;
            PIMAGE_NT_HEADERS Hdr = RtlImageNtHeader(ImageBuffer);

            Length = ImageFileName->MaximumLength * sizeof(WCHAR);
            PLDR_DATA_TABLE_ENTRY Ldr = (PLDR_DATA_TABLE_ENTRY)\
                MmDbgAllocateMemory( sizeof(LDR_DATA_TABLE_ENTRY) + Length, PAGE_READWRITE );

            if ( Ldr ) {

                PWCHAR p;

                RtlZeroMemory( Ldr, sizeof(LDR_DATA_TABLE_ENTRY) );
                Ldr->DllBase = ImageBuffer;
                Ldr->SizeOfImage = Hdr->OptionalHeader.SizeOfImage;
                Ldr->CheckSum = Hdr->OptionalHeader.CheckSum;
                Ldr->LoadedImports = NULL;
                Ldr->LoadCount = 1;
                Ldr->Flags = LDRP_ENTRY_PROCESSED | LDRP_SYSTEM_MAPPED;
                Ldr->FullDllName.Buffer = (PWSTR)((ULONG_PTR)Ldr + sizeof(LDR_DATA_TABLE_ENTRY));
                Ldr->FullDllName.Length = 0;
                Ldr->FullDllName.MaximumLength = Length;
                RtlAnsiStringToUnicodeString( &Ldr->FullDllName, ImageFileName, FALSE );
                Ldr->BaseDllName = Ldr->FullDllName;

                ExInterlockedInsertTailList( DmGetCurrentDmi()->LoadedModuleList,
                    &Ldr->InLoadOrderLinks );

                p = wcsstr( Ldr->FullDllName.Buffer, L".bin" );
                if ( p ) {
                    wcsncpy( p, L".exe", 4 );
                }
            }

            DbgLoadImageSymbols(ImageFileName, BaseAddress, (ULONG_PTR)-1);
        }
#endif
    }

    //
    // The module is now successfully loaded.
    //

    status = STATUS_SUCCESS;

CleanupAndExit:

    return status;
}

MI_MEMORY
DIAMONDAPI
DvdpAlloc(
    ULONG NumberOfBytes
    )
{
    return LocalAlloc( LPTR, NumberOfBytes );
}

VOID
DIAMONDAPI
DvdpFree(
    MI_MEMORY pointer
    )
{
    LocalFree( pointer );
}

NTSTATUS
DvdpDecompress(
    IN ULONG FromAddress,
    IN ULONG ToAddress,
    IN ULONG CompressedSize,
    IN ULONG UncompressedSize
    )
/*++

Routine Description:

    This routine decompresses binary downloaded from XDCS into virtual memory
    area specified by ToAddress.  The adress has to be in lower 2GB under Xbox
    so that it can be executable.

Arguments:

    FromAddress - Address of compressed binary downloaded from XDCS

    ToAddress - Address to be decompressed, must be in lower 2GB

    UncompressedSize - Total size of uncompressed image

Return Value:

    NT status code

--*/
{
    LZXDECOMPRESS Decomp;
    PLZXBOX_BLOCK Block;
    ULONG Source;
    ULONG Dest;
    UINT SourceSize;
    UINT DestSize;
    UINT BytesDecompressed;
    LDI_CONTEXT_HANDLE Handle;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    ASSERT( (ToAddress + UncompressedSize) <= 0x7FFE0000 );

    //
    // Initialize decompression engine
    //

    Decomp.fCPUtype = LDI_CPU_80386;
    Decomp.WindowSize = LZX_WINDOW_SIZE;

    SourceSize = LZX_CHUNK_SIZE;
    if ( LDICreateDecompression(&SourceSize, &Decomp, DvdpAlloc, DvdpFree, &DestSize,
        &Handle, NULL,NULL,NULL,NULL,NULL) != MDI_ERROR_NO_ERROR ) {
        return STATUS_NO_MEMORY;
    }

    Source = FromAddress;
    Dest = ToAddress;

    while(CompressedSize) {

        //
        // Read the compression block
        //
        Block = (PLZXBOX_BLOCK)Source;
        Source += sizeof(LZXBOX_BLOCK);
        CompressedSize -= sizeof Block;

        //
        // Perform decompression
        //
        if (Block->CompressedSize > CompressedSize)
            goto CleanupAndExit;
        CompressedSize -= Block->CompressedSize;
        BytesDecompressed = Block->UncompressedSize;
        if (Dest + BytesDecompressed > ToAddress + UncompressedSize)
            goto CleanupAndExit;
        if ( LDIDecompress(Handle, (PVOID)Source, Block->CompressedSize, (PVOID)Dest,
            &BytesDecompressed) != MDI_ERROR_NO_ERROR ) {
            goto CleanupAndExit;
        }

        //
        // Advance the pointers
        //
        Source += Block->CompressedSize;
        Dest += Block->UncompressedSize;

    }

    if(0 == CompressedSize && ToAddress + UncompressedSize == Dest)
        Status = STATUS_SUCCESS;

CleanupAndExit:

    LDIDestroyDecompression( Handle );
    return Status;
}

VOID
XCCalcDigest(
    IN  PBYTE  pbMsgData,
    IN  DWORD  dwMsgDataLen,
    OUT PBYTE  pbDigest
    )
/*++

Routine Description:

    This function calculates a digest (hash) for the given daata

Arguments:

    pbMsgData - Pointer to data to be calculated

    dwMsgDataLen - Length of data in bytes

    pbDigest - Output buffer of digest

Return Value:

    None

--*/
{
    UCHAR SHAHash[XC_SERVICE_SHA_CONTEXT_SIZE];
    BYTE abSHADigest[ XC_DIGEST_LEN ];

    XcSHAInit( SHAHash );

    XcSHAUpdate( SHAHash, (PBYTE)&dwMsgDataLen, sizeof(DWORD) );
    XcSHAUpdate( SHAHash, pbMsgData, dwMsgDataLen );

    XcSHAFinal( SHAHash, abSHADigest );

    memcpy( pbDigest, abSHADigest, XC_DIGEST_LEN );
}

NTSTATUS
DvdpVerifyHeaderDigest(
    IN PXBEIMAGE_HEADER ImageHeader
    )
{
    PBYTE pbToSign;
    ULONG cbToSign;
    BYTE rgbDigest[XC_DIGEST_LEN];
    NTSTATUS st;

    pbToSign = ImageHeader->EncryptedDigest +
        sizeof ImageHeader->EncryptedDigest;
    cbToSign = (PBYTE)ImageHeader + ImageHeader->SizeOfHeaders - pbToSign;

    // Calculate the header digest
    XCCalcDigest(pbToSign, cbToSign, rgbDigest);

    // Verify it
    st = XcVerifyPKCS1Signature(ImageHeader->EncryptedDigest, XePublicKeyData,
        rgbDigest) ? STATUS_SUCCESS : STATUS_TRUST_FAILURE;
    return st;
}

BOOL
NTAPI
DvdKeyInitialize(
    OUT PUCHAR DvdRegion,
    OUT PULONG ExtendedErrorInfo
    )
/*++

Routine Description:

    This is the first routine XDash supposed to call before trying to access
    other DVD-playback functions.  It will load and initialize DVD playback
    library from hardware key, perform necessary uncompress, fix up exports
    and imports thunks, security checking, and finally call real DDP_Intialize
    to initialize DVD-playback engine.

Arguments:

    DvdRegion - Pointer to a byte to receive a DVD region of the DVD key

    ExtendedErrorInfo - Pointer to unsigned long to receive extended error
        information about DVD playback error

Return Value:

    Generic Win32 error code

--*/
{
    NTSTATUS Status;
    STRING ImageName;
    PCHAR BackSlashPtr;
    XDCS_DVD_CODE_INFORMATION *DvdKey;
    PXBEIMAGE_HEADER pxih;
    PXBEIMAGE_CERTIFICATE pxcert;
    PXBEIMAGE_SECTION pxsh;
    DWORD DeviceInstance = 0;
    HANDLE FileHandle = NULL;
    BOOL fDongleInserted = FALSE;
    PUCHAR CompressedBuffer = NULL;
    SIZE_T CompressedBufferSize;
    SIZE_T UncompressedBufferSize;
    PBYTE RawData;
    DWORD error, DonglePort, BytesRead;
    XDCS_DVD_CODE_INFORMATION DvdCodeInfo = { 0, 0 };
    NTSTATUS (__cdecl *EntryPoint)( PVOID, PVOID, PVOID, PVOID );
    BYTE HeaderDigest[XC_DIGEST_LEN];

#ifdef ON_DISK_DONGLE
    OBJECT_ATTRIBUTES ObjA;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;
#endif

    __try {

        *ExtendedErrorInfo = 0;

        //
        // Look for the DVD dongle starting from left to right
        //

        DWORD dwDevices = XGetDevices( XDEVICE_TYPE_DVD_CODE_SERVER );

        for ( DonglePort=0; DonglePort<4; DonglePort++ ) {
            if ( dwDevices & (1 << DonglePort) ) {
                error = XDCSGetInformation( DonglePort, &DeviceInstance, &DvdCodeInfo );
                if ( error != ERROR_SUCCESS ) {
                    KdPrint(( "DVD: unable to read from the dongle on port %d, skipping\n", DonglePort ));
                    continue;
                }

                KdPrint(( "DVD: found dongle on port %d version %x.%x compressed code size = %d\n",
                          DonglePort, DvdCodeInfo.bcdVersion >> 8, DvdCodeInfo.bcdVersion & 0xFF,
                          DvdCodeInfo.dwCodeLength ));
                fDongleInserted = TRUE;
                break;
            }
        }

        if ( !fDongleInserted ) {
#ifdef ON_DISK_DONGLE
            //
            // We also load the code off the harddisk in ON_DISK_DONGLE build
            //

            KdPrint(( "DVD: unable to find DVD dongle, looking for one from harddisk\n" ));

TryHarddisk:

            InitializeObjectAttributes( &ObjA, &DvdLibFile, OBJ_CASE_INSENSITIVE, 0, 0 );

            Status = NtOpenFile( &FileHandle, GENERIC_READ, &ObjA, &IoStatusBlock,
                                 0, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE );

            if ( !NT_SUCCESS(Status) ) {
                KdPrint(( "DVD: unable to find %Z (%x)\n", &DvdLibFile, Status ));
                __leave;
            }

            Status = NtQueryInformationFile( FileHandle, &IoStatusBlock,
                                             &NetworkInfo, sizeof(NetworkInfo),
                                             FileNetworkOpenInformation );

            if ( !NT_SUCCESS(Status) ) {
                __leave;
            }

            ASSERT( NetworkInfo.EndOfFile.HighPart == 0 );
            ASSERT( NetworkInfo.EndOfFile.LowPart != 0 );

            DvdCodeInfo.dwCodeLength = NetworkInfo.EndOfFile.LowPart;
#else
            Status = STATUS_DEVICE_NOT_CONNECTED;
            __leave;
#endif
        }

        //
        // Preparing buffer for downloading compressed image from XDCS
        //

        ASSERT( DvdCodeBaseAddress == NULL );

        CompressedBufferSize = DvdCodeInfo.dwCodeLength;
        CompressedBuffer = (PUCHAR)ExAllocatePoolWithTag( CompressedBufferSize, '_DVD' );

        if ( !CompressedBuffer ) {
            Status = STATUS_NO_MEMORY;
            __leave;
        }

#ifdef ON_DISK_DONGLE
        //
        // For ON_DISK_DONGLE build if we couldn't find the dongle, fall back to harddisk
        //

        if ( !fDongleInserted ) {

            Status = NtReadFile( FileHandle, 0, 0, 0, &IoStatusBlock,
                                 CompressedBuffer, CompressedBufferSize, 0 );

            if ( !NT_SUCCESS(Status) ) {
                KdPrint(( "DVD: NtReadFile failed with status = %x\n", Status ));
                __leave;
            }

            BytesRead = IoStatusBlock.Information;
            error = ERROR_SUCCESS;

        } else {
            error = XDCSDownloadCode( DeviceInstance, CompressedBuffer, 0, CompressedBufferSize, &BytesRead );
        }
#else
        error = XDCSDownloadCode( DeviceInstance, CompressedBuffer, 0, CompressedBufferSize, &BytesRead );
#endif

        if ( error != ERROR_SUCCESS ) {
            Status = STATUS_UNSUCCESSFUL;
            *ExtendedErrorInfo = error;
            __leave;
        }

        if ( BytesRead != CompressedBufferSize ) {
            Status = STATUS_INFO_LENGTH_MISMATCH;
            __leave;
        }

        DvdKey = (XDCS_DVD_CODE_INFORMATION *)CompressedBuffer;

        if ( (DvdKey->bcdVersion & 0xFF00) != (DVDKEY_MINIMUM_VERSION & 0xFF00) ||
            (DvdKey->bcdVersion & 0xFF) < (DVDKEY_MINIMUM_VERSION & 0xFF)) {
            DbgPrint( "DVD: the version of this dongle is too old or incompatible (%x.%x)\n",
                      DvdKey->bcdVersion >> 8, DvdKey->bcdVersion & 0xFF );
#ifdef ON_DISK_DONGLE
            if ( !fDongleInserted ) {
                __asm int 3;
            } else {
                KdPrint(( "DVD: trying the dongle on the harddisk...\n" ));
                fDongleInserted = FALSE;
                goto TryHarddisk;
            }
#endif
            Status = STATUS_REVISION_MISMATCH;
            __leave;
        }


        //
        // Locate and authenticate the XBE header
        //

        pxih = (PXBEIMAGE_HEADER)(DvdKey + 1);
        if (pxih->Signature != XBEIMAGE_SIGNATURE || pxih->SizeOfImageHeader <
            sizeof *pxih || pxih->SizeOfHeaders < pxih->SizeOfImageHeader ||
            pxih->SizeOfHeaders > pxih->SizeOfImage ||
            pxih->SizeOfImage > DvdKey->dwCodeLength)
        {
            DbgPrint("DVD: unrecognized dongle image format\n");
            Status = STATUS_INVALID_IMAGE_FORMAT;
            __leave;
        }
        if (pxih->SizeOfImage + sizeof *DvdKey != DvdKey->dwCodeLength) {
            DbgPrint("DVD: dongle image sizes don't match\n");
            Status = STATUS_INVALID_IMAGE_FORMAT;
            __leave;
        }
        Status = DvdpVerifyHeaderDigest(pxih);
        if (!NT_SUCCESS(Status)) {
            DbgPrint("DVD: invalid image header digest\n");
            Status = STATUS_TRUST_FAILURE;
            __leave;
        }

        //
        // Locate the additional header information and ensure that it got
        // signed
        //

        pxcert = (PXBEIMAGE_CERTIFICATE)((PBYTE)pxih +
            (ULONG_PTR)pxih->Certificate - (ULONG_PTR)pxih->BaseAddress);
        pxsh = (PXBEIMAGE_SECTION)((PBYTE)pxih +
            (ULONG_PTR)pxih->SectionHeaders - (ULONG_PTR)pxih->BaseAddress);
        if (pxcert->SizeOfCertificate < XBEIMAGE_CERTIFICATE_BASE_SIZEOF ||
            (PBYTE)pxcert < (PBYTE)pxih || (PBYTE)pxsh < (PBYTE)pxih ||
            ((PBYTE)pxcert + pxcert->SizeOfCertificate) -
            (PBYTE)pxih > (int)pxih->SizeOfHeaders ||
            (PBYTE)(pxsh + 1) - (PBYTE)pxih > (int)pxih->SizeOfHeaders)
        {
            DbgPrint("DVD: invalid header data\n");
            Status = STATUS_INVALID_IMAGE_FORMAT;
            __leave;
        }

        //
        // Authenticate certificate data.  Must be signed to run from dongle
        // only
        //

        if (pxcert->AllowedMediaTypes != XBEIMAGE_MEDIA_TYPE_DONGLE) {
            DbgPrint("DVD: invalid image certificate\n");
            Status = STATUS_IMAGE_MEDIA_TYPE_VIOLATION;
            __leave;
        }
        if (pxcert->Version != DvdKey->bcdVersion) {
            DbgPrint("DVD: version stamps don't match\n");
            Status = STATUS_REVISION_MISMATCH;
            __leave;
        }

        //
        // Authenticate section data
        //

        RawData = (PBYTE)pxih + pxsh->PointerToRawData;
        if (RawData + pxsh->SizeOfRawData != (PBYTE)pxih + pxih->SizeOfImage) {
            DbgPrint("DVD: section data is invalid size\n");
            Status = STATUS_INVALID_IMAGE_FORMAT;
            __leave;
        }
        XCCalcDigest(RawData, pxsh->SizeOfRawData, HeaderDigest);
        if (memcmp(HeaderDigest, pxsh->SectionDigest, XC_DIGEST_LEN) != 0) {
            DbgPrint("DVD: invalid section digest\n");
            Status = STATUS_TRUST_FAILURE;
            __leave;
        }

        UncompressedBufferSize = pxsh->VirtualSize;
        ASSERT( UncompressedBufferSize );

        KdPrint(( "DVD: region of this dongle is %d\n", pxcert->GameRegion));

        Status = NtAllocateVirtualMemory( &DvdCodeBaseAddress, 0, &UncompressedBufferSize,
                                          MEM_TOP_DOWN | MEM_COMMIT, PAGE_EXECUTE_READWRITE );

        if ( !NT_SUCCESS(Status) ) {
            KdPrint(( "DVD: memory allocation failed\n" ));
            __leave;
        }

        Status = DvdpDecompress( (ULONG_PTR)RawData, (ULONG_PTR)DvdCodeBaseAddress,
            pxsh->SizeOfRawData, pxsh->VirtualSize);

        if ( !NT_SUCCESS(Status) ) {
            KdPrint(( "DVD: decompression failed\n" ));
            __leave;
        }

        // Fill the remainder of the page with zeroes
        if(pxsh->VirtualSize < UncompressedBufferSize) {
            memset((PBYTE)DvdCodeBaseAddress + pxsh->VirtualSize, 0,
                UncompressedBufferSize  - pxsh->VirtualSize);
        }

        BackSlashPtr = strrchr( DvdLibFile.Buffer, '\\' );

        if ( !BackSlashPtr ) {
            BackSlashPtr = DvdLibFile.Buffer;
        } else {
            BackSlashPtr++;
        }

        //
        // Fixup and snap all the imports and exports thunks
        //

        RtlInitObjectString( &ImageName, BackSlashPtr );

        Status = DvdLoadSystemImage( DvdCodeBaseAddress, &ImageName,
                                     (PVOID*)&EntryPoint );

        if ( !NT_SUCCESS(Status) ) {
            __leave;
        }

        //
        // Call the entry point so that the CRT and DDP get initialized
        //

        ULONG Error = EntryPoint( &DashAPIsBegin, &DashAPIsEnd, &DvdAPIsBegin, &DvdAPIsEnd );

        if ( !IS_ERROR(Error) ) {
            *DvdRegion = (UCHAR)pxcert->GameRegion;
            ASSERT( *DvdRegion >= 1 && *DvdRegion <= 6 );
        } else {
            Status = STATUS_UNSUCCESSFUL;
            *ExtendedErrorInfo = Error;
        }

    } __except( EXCEPTION_EXECUTE_HANDLER ) {
        Status = GetExceptionCode();
    }

    if ( FileHandle ) {
        NtClose(FileHandle);
    }

    if ( CompressedBuffer ) {
        ExFreePool( CompressedBuffer );
    }

    if ( !NT_SUCCESS(Status) && DvdCodeBaseAddress ) {
        UncompressedBufferSize = 0;
        NtFreeVirtualMemory( &DvdCodeBaseAddress, &UncompressedBufferSize, MEM_RELEASE );
    }

    if ( !NT_SUCCESS(Status) && *ExtendedErrorInfo == 0 ) {
        *ExtendedErrorInfo = RtlNtStatusToDosError( Status );
    }

    return NT_SUCCESS(Status);
}

#ifdef _FAKE_DSOUND

struct _FakeDSound {

    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)(THIS) { return S_OK; };
    STDMETHOD_(ULONG, Release)(THIS) { return S_OK; };

    // XMediaObject methods
    STDMETHOD(GetInfo)(THIS_ LPXMEDIAINFO pInfo)
    {
        pInfo->dwFlags = XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC;
        pInfo->dwMaxLookahead = 1;
        pInfo->dwInputSize = 1;
        pInfo->dwOutputSize = 1;
        return S_OK;
    };

    STDMETHOD(GetStatus)(THIS_ LPDWORD pdwStatus) { return S_OK; };

    STDMETHOD(Process)(THIS_ LPCXMEDIAPACKET pInputPacket, LPCXMEDIAPACKET pOutputPacket)
    {
        if ( pInputPacket ) {
            if ( pInputPacket->hCompletionEvent ) {
                SetEvent( pInputPacket->hCompletionEvent );
            }
            if ( pInputPacket->pdwCompletedSize ) {
                *pInputPacket->pdwCompletedSize = pInputPacket->dwMaxSize;
            }
            if ( pInputPacket->pdwStatus ) {
                *pInputPacket->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;
            }
        }

        if ( pOutputPacket ) {
            if ( pOutputPacket->hCompletionEvent ) {
                SetEvent( pOutputPacket->hCompletionEvent );
            }
            if ( pOutputPacket->pdwCompletedSize ) {
                *pOutputPacket->pdwCompletedSize = pOutputPacket->dwMaxSize;
            }
            if ( pOutputPacket->pdwStatus ) {
                *pOutputPacket->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;
            }
        }

        return S_OK;
    };

    STDMETHOD(Discontinuity)(THIS) { return S_OK; };
    STDMETHOD(Flush)(THIS) { return S_OK; };

    // IDirectSoundStream methods
    STDMETHOD(SetFrequency)(THIS_ DWORD dwFrequency) { return S_OK; };
    STDMETHOD(SetVolume)(THIS_ LONG lVolume) { return S_OK; };
#ifdef SILVER
    STDMETHOD(SetChannelVolume)(THIS_ LPCDSCHANNELVOLUME pVolume) { return S_OK; };
#else
    STDMETHOD(SetPitch)(THIS_ LONG lPitch) { return S_OK; };
    STDMETHOD(SetLFO)(THIS_ LPCDSLFODESC pLFODesc) { return S_OK; };
    STDMETHOD(SetEG)(THIS_ LPCDSENVELOPEDESC pEnvelopeDesc) { return S_OK; };
    STDMETHOD(SetFilter)(THIS_ LPCDSFILTERDESC pFilterDesc) { return S_OK; };
    STDMETHOD(SetOutputBuffer)(THIS_ LPDIRECTSOUNDBUFFER pOutputBuffer) { return S_OK; };
    STDMETHOD(SetMixBins)(THIS_ DWORD dwMixBins) { return S_OK; };
    STDMETHOD(SetMixBinVolumes)(THIS_ DWORD dwMixBins, const LONG *alVolumes) { return S_OK; };
    STDMETHOD(SetHeadroom)(THIS_ DWORD dwHeadroom) { return S_OK; };
    STDMETHOD(Pause)(THIS_ DWORD dwPause) { return S_OK; };
#endif

    // IDirectSound3DBuffer methods
    STDMETHOD(SetAllParameters)(THIS_ LPCDS3DBUFFER pds3db, DWORD dwApply) { return S_OK; };
    STDMETHOD(SetConeAngles)(THIS_ DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply) { return S_OK; };
    STDMETHOD(SetConeOrientation)(THIS_ FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) { return S_OK; };
    STDMETHOD(SetConeOutsideVolume)(THIS_ LONG lConeOutsideVolume, DWORD dwApply) { return S_OK; };
    STDMETHOD(SetMaxDistance)(THIS_ FLOAT flMaxDistance, DWORD dwApply) { return S_OK; };
    STDMETHOD(SetMinDistance)(THIS_ FLOAT flMinDistance, DWORD dwApply) { return S_OK; };
    STDMETHOD(SetMode)(THIS_ DWORD dwMode, DWORD dwApply) { return S_OK; };
    STDMETHOD(SetPosition)(THIS_ FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) { return S_OK; };
    STDMETHOD(SetVelocity)(THIS_ FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) { return S_OK; };

} _FakeDSound;

EXTERN_C
HRESULT
FakeDirectSoundCreateStream(
    LPCDSSTREAMDESC  pdssd,
    LPDIRECTSOUNDSTREAM * ppStream
    )
{
    *ppStream = (LPDIRECTSOUNDSTREAM)&_FakeDSound;
    return S_OK;
}

EXTERN_C
VOID
FakeDirectSoundDoWork(
    VOID
    )
{
}

#endif // _FAKE_DSOUND

