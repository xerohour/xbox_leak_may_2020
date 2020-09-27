/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    imagebld.cpp

Abstract:

    This module implements a utility program to build an Xbox executable image.

--*/

#include "imgbldp.h"

//
// Stores the file handle to access the input file.
//
HANDLE ImgbInputFileHandle = INVALID_HANDLE_VALUE;

//
// Stores the number of bytes in the input file.
//
ULONG ImgbInputFileSize;

//
// Stores the file mapping handle to access the input file.
//
HANDLE ImgbInputFileMappingHandle;

//
// Stores the file mapping view pointer to access the input file.
//
LPVOID ImgbInputFileMappingView;

//
// Stores the fully resolved input file path.
//
CHAR ImgbInputFileFullPath[MAX_PATH+1];

//
// Stores the pointer to the file name in the ImgbInputFileFullPath buffer.
//
LPSTR ImgbInputFileFilePart;

//
// Stores the file handle to access the output file.
//
HANDLE ImgbOutputFileHandle = INVALID_HANDLE_VALUE;

//
// Stores the pointer to the NT headers from the input file.
//
PIMAGE_NT_HEADERS ImgbNtHeader;

//
// Stores the pointer to the root XBE image header for the output file.
//
PIMGB_XBEIMAGE_HEADER ImgbXbeImageHeader;

//
// Giant transfer buffer for read/write operations.
//
BYTE ImgbTransferBuffer[128 * 1024];

//
// Stores the compressed Microsoft logo bitmap stored in every XBE image.
//
const UCHAR ImgbMicrosoftLogo[] = {
    #include "msftlogo.h"
};

//
// Signifes whether to suppress the library approval warnings
//
BOOLEAN ImgbNoWarnLibraryApproval;

DECLSPEC_NORETURN
VOID
ImgbExitProcess(
    int ExitCode
    )
{
    if (ImgbOutputFileHandle != INVALID_HANDLE_VALUE) {

        CloseHandle(ImgbOutputFileHandle);

        //
        // If an error has occurred, then delete the output file.
        //

        if (ExitCode != 0) {
            DeleteFile(ImgbOutputFilePath);
        }
    }

    exit(ExitCode);
}

VOID
ImgbOpenInputOutputFiles(
    VOID
    )
{
    //
    // Verify that the input and output file paths were specified.
    //

    if (ImgbInputFilePath == NULL) {
        ImgbResourcePrintErrorAndExit(IDS_MISSING_REQUIRED_OPTION, "/IN:");
    }

    if (ImgbOutputFilePath == NULL) {
        ImgbResourcePrintErrorAndExit(IDS_MISSING_REQUIRED_OPTION, "/OUT:");
    }

    //
    // Resolve the full path name of the input file.
    //

    if (GetFullPathName(ImgbInputFilePath, MAX_PATH, ImgbInputFileFullPath,
        &ImgbInputFileFilePart) == 0) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_OPEN_INPUT_FILE, ImgbInputFilePath);
    }

    //
    // Create the output file.
    //

    ImgbOutputFileHandle = CreateFile(ImgbOutputFilePath, GENERIC_READ |
        GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (ImgbOutputFileHandle == INVALID_HANDLE_VALUE) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_CREATE_OUTPUT_FILE, ImgbOutputFilePath);
    }

    //
    // Open the input file.
    //

    ImgbInputFileHandle = CreateFile(ImgbInputFileFullPath, GENERIC_READ,
        FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (ImgbInputFileHandle == INVALID_HANDLE_VALUE) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_OPEN_INPUT_FILE, ImgbInputFilePath);
    }

    //
    // Obtain the size of the input file.
    //

    ImgbInputFileSize = GetFileSize(ImgbInputFileHandle, NULL);

    //
    // Create a copy-on-write section of the input file.
    //

    ImgbInputFileMappingHandle = CreateFileMapping(ImgbInputFileHandle, NULL,
        PAGE_WRITECOPY, 0, 0, NULL);

    if (ImgbInputFileMappingHandle == NULL) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_READ_INPUT_FILE, ImgbInputFilePath);
    }

    //
    // Map a copy-on-write view of the input file section.
    //

    ImgbInputFileMappingView = MapViewOfFile(ImgbInputFileMappingHandle,
        FILE_MAP_COPY, 0, 0, 0);

    if (ImgbInputFileMappingView == NULL) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_READ_INPUT_FILE, ImgbInputFilePath);
    }
}

VOID
ImgbProcessInputSectionHeaders(
    VOID
    )
{
    ULONG SectionsRemaining;
    ULONG SectionMask;
    ULONG LastVirtualAddress;
    ULONG LastEndingVirtualAddress;
    PIMAGE_SECTION_HEADER SectionHeader;
    UCHAR SectionName[IMAGE_SIZEOF_SHORT_NAME + 1];
    ULONG RelativeLoadAddress;

    //
    // Trim off all discardable sections from the end of the image.  The linker
    // may place a non-discardable section (such as .rsrc) between discardable
    // sections so we can't just stop when we hit the first discardable section.
    //

    SectionsRemaining = ImgbNtHeader->FileHeader.NumberOfSections;

    while (SectionsRemaining) {

        SectionHeader = IMAGE_FIRST_SECTION(ImgbNtHeader) +
            SectionsRemaining - 1;

        if ((SectionHeader->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0) {

            //
            // Compute the number of bytes that will be required to map the
            // executable sections.  Align that number up to the section
            // alignment of the executable file.
            //

            ImgbXbeImageHeader->SizeOfExecutableImage =
                SectionHeader->VirtualAddress + SectionHeader->Misc.VirtualSize -
                IMAGE_FIRST_SECTION(ImgbNtHeader)->VirtualAddress;

            SectionMask = ImgbNtHeader->OptionalHeader.SectionAlignment - 1;

            ImgbXbeImageHeader->SizeOfExecutableImage =
                (ImgbXbeImageHeader->SizeOfExecutableImage + SectionMask) & ~SectionMask;

            if (ImgbXbeImageHeader->SizeOfExecutableImage > IMGB_MAXIMUM_IMAGE_SIZE) {
                ImgbResourcePrintErrorAndExit(IDS_IMAGE_TOO_LARGE);
            }

            break;
        }

        SectionsRemaining--;
    }

    //
    // Walk through each of the image's sections and it to the section list
    // head.
    //

    LastVirtualAddress = 0;
    LastEndingVirtualAddress = 0;
    SectionHeader = IMAGE_FIRST_SECTION(ImgbNtHeader);

    while (SectionsRemaining) {

        //
        // Verify that the section headers are in increasing virtual address
        // order.
        //

        if (SectionHeader->VirtualAddress <= LastVirtualAddress) {
            ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
        }

        //
        // Verify that the file data exists in the input file.
        //

        if ((SectionHeader->PointerToRawData > ImgbInputFileSize) ||
            (SectionHeader->PointerToRawData + SectionHeader->SizeOfRawData > ImgbInputFileSize)) {
            ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
        }

        //
        // Verify that the section actually contains data.  The linker won't
        // generate a section header for a section that doesn't contribute any
        // code or data.  The below code assumes that VirtualSize will never be
        // zero.
        //

        if (SectionHeader->Misc.VirtualSize == 0) {
            ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
        }

        //
        // Build the section name on the stack.
        //

        memcpy(SectionName, SectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME);
        SectionName[IMAGE_SIZEOF_SHORT_NAME] = '\0';

        //
        // Increment the number of XBE sections.
        //

        ImgbXbeImageHeader->ImageHeader.NumberOfSections++;

        //
        // Increment the number of executable sections.
        //

        ImgbXbeImageHeader->SectionHeaders.NumberOfExecutableSections++;

        //
        // Add the size of the section name to the total for the XBE section
        // header.
        //

        ImgbXbeImageHeader->SectionHeaders.SizeOfSectionNames += strlen((PSTR)SectionName) + 1;

        //
        // Check if the section is a preload section or not.
        //

        if (ImgbSearchNoPreloadList((PSTR)SectionName)) {
            SectionHeader->Characteristics &= ~IMAGE_SCN_MEM_PRELOAD;
        } else {
            SectionHeader->Characteristics |= IMAGE_SCN_MEM_PRELOAD;
        }

        //
        // Compute the load address for the section relative to the first
        // section.  The first image section will always start on a page
        // boundary.
        //

        RelativeLoadAddress = SectionHeader->VirtualAddress -
            IMAGE_FIRST_SECTION(ImgbNtHeader)->VirtualAddress;

        //
        // If this the first section or if the section does not start on the
        // same page as the last section, then we'll need another shared
        // page reference counter.
        //

        if ((RelativeLoadAddress == 0) ||
            (PAGE_ALIGN(RelativeLoadAddress) !=
                PAGE_ALIGN(LastEndingVirtualAddress -
                IMAGE_FIRST_SECTION(ImgbNtHeader)->VirtualAddress))) {
            ImgbXbeImageHeader->SectionHeaders.NumberOfSharedPageReferenceCounts++;
        }

        //
        // If the inclusive ending byte of the section isn't on the same
        // page as the starting byte of the section, then we'll need another
        // shared page reference counter.
        //

        if (PAGE_ALIGN(RelativeLoadAddress) !=
            PAGE_ALIGN(RelativeLoadAddress + SectionHeader->Misc.VirtualSize - 1)) {
            ImgbXbeImageHeader->SectionHeaders.NumberOfSharedPageReferenceCounts++;
        }

        LastVirtualAddress = SectionHeader->VirtualAddress;
        LastEndingVirtualAddress = LastVirtualAddress + SectionHeader->Misc.VirtualSize - 1;

        SectionsRemaining--;
        SectionHeader++;
    }

    //
    // Attach the import directory header to the list of headers.
    //

    InsertTailList(&ImgbXbeImageHeader->HeadersListHead,
        &ImgbXbeImageHeader->SectionHeaders.HeadersListEntry);
}

VOID
ImgbEnsureVirtualAddressIsPreload(
    ULONG VirtualAddress
    )
{
    PIMAGE_SECTION_HEADER SectionHeader;
    UCHAR SectionName[IMAGE_SIZEOF_SHORT_NAME + 1];

    SectionHeader = ImgbVirtualAddressToSectionHeader(ImgbNtHeader, VirtualAddress);

    if ((SectionHeader->Characteristics & IMAGE_SCN_MEM_PRELOAD) == 0) {

        memcpy(SectionName, SectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME);
        SectionName[IMAGE_SIZEOF_SHORT_NAME] = '\0';

        ImgbResourcePrintWarning(IDS_IGNORING_NOPRELOAD, SectionName);

        SectionHeader->Characteristics |= IMAGE_SCN_MEM_PRELOAD;
    }
}

VOID
ImgbProcessInputImportDescriptors(
    VOID
    )
{
    ULONG ImportDescriptorBytesRemaining;
    PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
    ULONG NumberOfNonKernelImports;
    ULONG SizeOfNonKernelImageNames;
    LPCSTR ImageName;
    PIMAGE_THUNK_DATA OriginalImageThunkData;
    PIMAGE_THUNK_DATA ImageThunkData;

    //
    // Verify that the image hasn't been bound, which screws up the import image
    // thunks that the XBE loader needs to use.
    //

    if (ImgbNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size != 0) {
        ImgbResourcePrintErrorAndExit(IDS_BOUND_IMAGES_UNSUPPORTED);
    }

    //
    // Walk through each of the import descriptors.
    //

    ImportDescriptorBytesRemaining =
        ImgbNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
    ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)
        ImgbImageDataDirectoryToData(ImgbNtHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
    NumberOfNonKernelImports = 0;
    SizeOfNonKernelImageNames = 0;

    while (ImportDescriptorBytesRemaining >= sizeof(IMAGE_IMPORT_DESCRIPTOR)) {

        //
        // If we find the NULL import descriptor, then break out now.
        //

        if (ImportDescriptor->Characteristics == 0) {
            break;
        }

        //
        // If this is the import descriptor for the kernel, then save the RVA
        // for the thunk data in the image header.
        //
        // Otherwise, increment the number of non kernel imports and the number
        // of bytes required to hold the image names.
        //

        ImageName = (LPCSTR)ImgbVirtualAddressToData(ImgbNtHeader, ImportDescriptor->Name);

        if (_stricmp(ImageName, "xboxkrnl.exe") == 0) {
            ImgbXbeImageHeader->ImageHeader.XboxKernelThunkData =
                (PIMAGE_THUNK_DATA)ImportDescriptor->FirstThunk;
        } else {
            NumberOfNonKernelImports++;
            SizeOfNonKernelImageNames += (strlen(ImageName) + sizeof(UCHAR)) * sizeof(WCHAR);
        }

        //
        // Walk through the image thunks and verify that all entries are import
        // by ordinal.
        //

        OriginalImageThunkData = (PIMAGE_THUNK_DATA)ImgbVirtualAddressToData(ImgbNtHeader,
            ImportDescriptor->OriginalFirstThunk);
        ImageThunkData = (PIMAGE_THUNK_DATA)ImgbVirtualAddressToData(ImgbNtHeader,
            ImportDescriptor->FirstThunk);

        while (ImageThunkData->u1.Ordinal != 0) {

            //
            // Verify that the original image thunk data matches the actual
            // image thunk data.  The XBE loader will only walk throught the
            // actual image thunk data and expects that the array will be filled
            // with ordinals.
            //

            if (OriginalImageThunkData->u1.Ordinal != ImageThunkData->u1.Ordinal) {
                ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE,
                    ImgbInputFilePath);
            }

            //
            // Verify that this is a ordinal import.
            //

            if (!IMAGE_SNAP_BY_ORDINAL(ImageThunkData->u1.Ordinal)) {
                ImgbResourcePrintErrorAndExit(IDS_CANNOT_IMPORT_BY_NAME, ImageName);
            }

            OriginalImageThunkData++;
            ImageThunkData++;
        }

        //
        // The XBE loader needs to access the section containing these import
        // thunks at boot time.  Ensure that the section is not marked as
        // NOPRELOAD.
        //

        ImgbEnsureVirtualAddressIsPreload(ImportDescriptor->FirstThunk);

        //
        // Advance to the next import descriptor.
        //

        ImportDescriptor++;
        ImportDescriptorBytesRemaining -= sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }

    //
    // We don't need to write out any additional headers if this image doesn't
    // import from anything besides the kernel.
    //

    if (NumberOfNonKernelImports != 0) {

        ImgbXbeImageHeader->ImportDescriptorHeader.NumberOfNonKernelImports =
            NumberOfNonKernelImports;

        //
        // Compute the virtual size of the import directory header.  This is the
        // number of bytes that we'll write out for the non-kernel imports.
        //
        // Note that we need to add one descriptor in order to terminate the
        // array.
        //

        ImgbXbeImageHeader->ImportDescriptorHeader.VirtualSize =
            (NumberOfNonKernelImports + 1) * sizeof(XBEIMAGE_IMPORT_DESCRIPTOR) +
            SizeOfNonKernelImageNames;

        //
        // Attach the import directory header to the list of headers.
        //

        InsertTailList(&ImgbXbeImageHeader->HeadersListHead,
            &ImgbXbeImageHeader->ImportDescriptorHeader.HeadersListEntry);

    } else {
        ImgbXbeImageHeader->ImportDescriptorHeader.VirtualSize = 0;
    }
}

VOID
ImgbProcessInputTlsDirectory(
    VOID
    )
{
    PIMAGE_TLS_DIRECTORY TlsDirectory;
    LPBYTE RawData;
    LPBYTE EndRawData;

    //
    // There's only work to do if the TLS data directory entry is valid.
    //

    if (ImgbNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size != 0) {

        TlsDirectory = (PIMAGE_TLS_DIRECTORY)
            ImgbImageDataDirectoryToData(ImgbNtHeader, IMAGE_DIRECTORY_ENTRY_TLS);

        ImgbXbeImageHeader->TlsRawDataHeader.TlsDirectory = TlsDirectory;

        //
        // Store the RVA of the TLS directory in the image header.
        //

        ImgbXbeImageHeader->ImageHeader.TlsDirectory = (PIMAGE_TLS_DIRECTORY)
            ImgbNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress;

        //
        // Ensure that the TLS directory structure is marked as preload.  The
        // XAPI startup code will need to create a thread, so this data must be
        // around at boot.
        //

        ImgbEnsureVirtualAddressIsPreload(ImgbNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

        //
        // Check if there's any __declspec(thread) data for this image.  The
        // .tls section is marked as discardable so we need to make a copy of
        // the raw data in the headers if the thread data is non-zero (not
        // common).
        //

        if ((TlsDirectory->EndAddressOfRawData - TlsDirectory->StartAddressOfRawData) != 0) {

            //
            // The linker doesn't bother to fill in SizeOfZeroFill.  The common
            // case is that all of the thread data is zero, in which case we can
            // avoid storing any data in the headers for TLS.
            //

            RawData = (LPBYTE)ImgbLoadAddressToData(ImgbNtHeader,
                TlsDirectory->StartAddressOfRawData);
            EndRawData = (LPBYTE)ImgbLoadAddressToData(ImgbNtHeader,
                TlsDirectory->EndAddressOfRawData);

            while (EndRawData > RawData) {

                if (*(EndRawData - 1) != 0) {
                    break;
                }

                TlsDirectory->SizeOfZeroFill++;
                EndRawData--;
            }

            //
            // If we hit any non-zero bytes, then we'll need to add a header to
            // the image to hold this data since the .tls section will be lost
            // when we throw away discardable sections.
            //

            if (EndRawData > RawData) {

                ImgbXbeImageHeader->TlsRawDataHeader.VirtualSize = EndRawData - RawData;
                ImgbXbeImageHeader->TlsRawDataHeader.RawData = RawData;

                //
                // Attach the TLS header to the list of headers.
                //

                InsertTailList(&ImgbXbeImageHeader->HeadersListHead,
                    &ImgbXbeImageHeader->TlsRawDataHeader.HeadersListEntry);
            }
        }
    }
}

VOID
ImgbPrintUnapprovedLibraryWarning(
    PXBEIMAGE_LIBRARY_VERSION LibraryVersion,
    int ApprovalLevel
    )
{
    char LibraryName[9];
    int ids;

    switch(ApprovalLevel) {
    default:
        ids = IDS_UNAPPROVED;
        break;
    case 1:
        ids = IDS_POSSIBLY_UNAPPROVED;
        break;
    case -1:
        ids = IDS_EXPIRED;
        break;
    }

    if(ApprovalLevel >= 0)
        LibraryVersion->ApprovedLibrary = ApprovalLevel;
    if(!ImgbNoWarnLibraryApproval) {
        memcpy(LibraryName, LibraryVersion->LibraryName, 8);
        LibraryName[8] = 0;
        ImgbResourcePrintWarning(ids, LibraryName);
    }
}

VOID
ImgbProcessLibraryVersions(
    VOID
    )
{
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG SectionIndex;
    PXBEIMAGE_LIBRARY_VERSION StartingLibraryVersion;
    PXBEIMAGE_LIBRARY_VERSION EndingLibraryVersion;
    PXBEIMAGE_LIBRARY_VERSION SourceLibraryVersion;
    PXBEIMAGE_LIBRARY_VERSION LibraryVersion;
    PXBEIMAGE_LIBRARY_VERSION XapiLibraryVersion;
    int ApprovedStatus = 2;

    ImgbXbeImageHeader->LibraryVersionHeader.XboxKernelOffset = MAXULONG;
    ImgbXbeImageHeader->LibraryVersionHeader.XapiOffset = MAXULONG;
    XapiLibraryVersion = NULL;

    SectionHeader = ImgbNameToSectionHeader(ImgbNtHeader, (PUCHAR)".xbld",
        &SectionIndex);

    if (SectionHeader != NULL) {

        //
        // The .XBLD section should be marked as discardable.  We're going to
        // duplicate it in the headers section so that we don't need to worry
        // about tracking this informational section at runtime.
        //
        // Because this image may have been incrementally linked, we're going
        // to need to make a copy of the version stamps so that we can
        // compress out the empty space
        //
        
        StartingLibraryVersion = (PXBEIMAGE_LIBRARY_VERSION)
            ImgbAllocateMemory(SectionHeader->Misc.VirtualSize);
        
        //
        // Compute the starting and ending pointers to the library versions.
        //

        SourceLibraryVersion =
            (PXBEIMAGE_LIBRARY_VERSION)ImgbVirtualAddressToData(ImgbNtHeader,
            SectionHeader->VirtualAddress);
        EndingLibraryVersion = (PXBEIMAGE_LIBRARY_VERSION)
            ((PUCHAR)SourceLibraryVersion + (SectionHeader->Misc.VirtualSize));

        ImgbXbeImageHeader->LibraryVersionHeader.LibraryVersions =
            StartingLibraryVersion;

        //
        // Walk through the library versions and cache the offsets to the
        // XBOXKRNL and XAPI libraries.
        //

        LibraryVersion = StartingLibraryVersion;

        while (SourceLibraryVersion + 1 <= EndingLibraryVersion) {

            if(*(PULONG)SourceLibraryVersion == 0) {

                //
                // Filler from the ilink.  Skip one dword and try again
                //

                SourceLibraryVersion = (PXBEIMAGE_LIBRARY_VERSION)
                    ((ULONG_PTR)SourceLibraryVersion + sizeof(ULONG));
                continue;
            }

            memcpy(LibraryVersion, SourceLibraryVersion,
                sizeof(XBEIMAGE_LIBRARY_VERSION));
            ++SourceLibraryVersion;

            if (memcmp(LibraryVersion->LibraryName, "XBOXKRNL", XBEIMAGE_LIBRARY_VERSION_NAME_LENGTH) == 0) {
                ImgbXbeImageHeader->LibraryVersionHeader.XboxKernelOffset =
                    (PUCHAR)LibraryVersion - (PUCHAR)StartingLibraryVersion;
            }

            if ((memcmp(LibraryVersion->LibraryName, "XAPILIB\0", XBEIMAGE_LIBRARY_VERSION_NAME_LENGTH) == 0) ||
                (memcmp(LibraryVersion->LibraryName, "XAPILIBD", XBEIMAGE_LIBRARY_VERSION_NAME_LENGTH) == 0)) {
                ImgbXbeImageHeader->LibraryVersionHeader.XapiOffset =
                    (PUCHAR)LibraryVersion - (PUCHAR)StartingLibraryVersion;
                XapiLibraryVersion = LibraryVersion;
            }

            LibraryVersion++;
        }

        ImgbXbeImageHeader->ImageHeader.NumberOfLibraryVersions =
            LibraryVersion - StartingLibraryVersion;
        ImgbXbeImageHeader->LibraryVersionHeader.VirtualSize =
            (PUCHAR)LibraryVersion - (PUCHAR)StartingLibraryVersion;


        //
        // Check the library approval status
        //

        ApprovedStatus = CheckLibraryApprovalStatus(XapiLibraryVersion,
            StartingLibraryVersion, LibraryVersion -
            StartingLibraryVersion, ImgbPrintUnapprovedLibraryWarning);

        //
        // Attach the library version header to the list of headers.
        //

        InsertTailList(&ImgbXbeImageHeader->HeadersListHead,
            &ImgbXbeImageHeader->LibraryVersionHeader.HeadersListEntry);
    }

    //
    // If we didn't find a library version for the kernel or XAPI, then print
    // out a warning.
    //

    if (ImgbXbeImageHeader->LibraryVersionHeader.XboxKernelOffset == MAXULONG) {
        ImgbResourcePrintWarning(IDS_NO_VERSION_DETECTED, "XBOXKRNL");
    }

    if (ImgbXbeImageHeader->LibraryVersionHeader.XapiOffset == MAXULONG) {
        ImgbResourcePrintWarning(IDS_NO_VERSION_DETECTED, "XAPI");
    }

    //
    // Check to see whether we're linking to any imports other than the kernel
    //

    if(!ImgbNoWarnLibraryApproval &&
        ImgbXbeImageHeader->ImportDescriptorHeader.VirtualSize != 0)
    {
        ImgbResourcePrintWarning(IDS_UNAPPROVED, "<debug extension import>");
        ApprovedStatus = 0;
    }

    //
    // If we're linking to any unapproved libraries, the print out a warning
    //

    if(ApprovedStatus < 2 && !ImgbNoWarnLibraryApproval)
        ImgbResourcePrintWarning(IDS_APPROVAL);
}

VOID
ImgbAddDebugPaths(
    VOID
    )
{
    //
    // Compute the size of the header.  We need enough space to write out the
    // full file path as an 8-bit null-terminated string and the file name as a
    // null-terminated Unicode string.
    //

    ImgbXbeImageHeader->DebugPathsHeader.VirtualSize =
        ((strlen(ImgbInputFileFullPath) + 1) * sizeof(CHAR)) +
        ((strlen(ImgbInputFileFilePart) + 1) * sizeof(WCHAR));

    //
    // Attach the debug paths header to the list of headers.
    //

    InsertTailList(&ImgbXbeImageHeader->HeadersListHead,
        &ImgbXbeImageHeader->DebugPathsHeader.HeadersListEntry);
}

VOID
ImgbCheckDuplicateInsertFileSection(
    PIMGB_INSERTFILE CheckInsertFileLink
    )
{
    PLIST_ENTRY NextListEntry;
    PIMGB_INSERTFILE InsertFileLink;
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG SectionsRemaining;
    UCHAR SectionName[IMAGE_SIZEOF_SHORT_NAME + 1];

    //
    // Check the list of insert files for a duplicate.  Only check as far as the
    // supplied node.
    //

    NextListEntry = ImgbInsertFileList.Flink;

    while (NextListEntry != &CheckInsertFileLink->ListEntry) {

        InsertFileLink = CONTAINING_RECORD(NextListEntry, IMGB_INSERTFILE,
            ListEntry);

        if (_stricmp(InsertFileLink->SectionName, CheckInsertFileLink->SectionName) == 0) {
            ImgbResourcePrintErrorAndExit(IDS_INSERTFILE_CONFLICTING_NAME,
                CheckInsertFileLink->SectionName);
        }

        NextListEntry = InsertFileLink->ListEntry.Flink;
    }

    //
    // Check the executable sections for a duplicate.
    //

    SectionHeader = IMAGE_FIRST_SECTION(ImgbNtHeader);
    SectionsRemaining = ImgbNtHeader->FileHeader.NumberOfSections;

    while (SectionsRemaining) {

        memcpy(SectionName, SectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME);
        SectionName[IMAGE_SIZEOF_SHORT_NAME] = '\0';

        if (_stricmp((PSTR)SectionName, CheckInsertFileLink->SectionName) == 0) {
            ImgbResourcePrintErrorAndExit(IDS_INSERTFILE_CONFLICTING_EXE,
                CheckInsertFileLink->SectionName);
        }

        SectionHeader++;
        SectionsRemaining--;
    }
}

VOID
ImgbProcessInsertFiles(
    VOID
    )
{
    PLIST_ENTRY NextListEntry;
    PIMGB_INSERTFILE InsertFileLink;
    ULONG RelativeLoadAddress;
    ULONG AlignedFileSize;

    //
    // Bail out early if there's no files to be inserted.
    //

    if (IsListEmpty(&ImgbInsertFileList)) {
        return;
    }

    //
    // Check for duplicate section names.  Search both the list of executable
    // sections that we're going to keep in the file and the list of files that
    // we're to insert.
    //

    NextListEntry = ImgbInsertFileList.Flink;

    while (NextListEntry != &ImgbInsertFileList) {

        InsertFileLink = CONTAINING_RECORD(NextListEntry, IMGB_INSERTFILE,
            ListEntry);

        ImgbCheckDuplicateInsertFileSection(InsertFileLink);

        NextListEntry = InsertFileLink->ListEntry.Flink;
    }

    //
    // Open each of the /INSERTFILE sections and get the size of the file.
    //

    RelativeLoadAddress = ImgbXbeImageHeader->SizeOfExecutableImage;
    NextListEntry = ImgbInsertFileList.Flink;

    while (NextListEntry != &ImgbInsertFileList) {

        InsertFileLink = CONTAINING_RECORD(NextListEntry, IMGB_INSERTFILE,
            ListEntry);

        //
        // If we don't already know that the section is a no preload section,
        // then go check the no preload list.
        //

        if (!InsertFileLink->NoPreload) {
            InsertFileLink->NoPreload = ImgbSearchNoPreloadList(InsertFileLink->SectionName);
        }

        //
        // Open the /INSERTFILE section.
        //

        InsertFileLink->FileHandle = CreateFile(InsertFileLink->FilePath,
            GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
            NULL);

        if (InsertFileLink->FileHandle == INVALID_HANDLE_VALUE) {
            ImgbResourcePrintErrorAndExit(IDS_CANNOT_OPEN_INPUT_FILE,
                InsertFileLink->FilePath);
        }

        //
        // Get the raw size of the /INSERTFILE section.
        //

        InsertFileLink->FileSize = GetFileSize(InsertFileLink->FileHandle, NULL);

        if (InsertFileLink->FileSize == INVALID_FILE_SIZE) {
            ImgbResourcePrintErrorAndExit(IDS_CANNOT_READ_INPUT_FILE,
                InsertFileLink->FilePath);
        }

        AlignedFileSize = (InsertFileLink->FileSize +
            IMGB_INSERTFILE_SECTION_ALIGNMENT - 1) & ~(IMGB_INSERTFILE_SECTION_ALIGNMENT - 1);

        //
        // Verify that adding this file doesn't throw us over the maximum
        // allowed size for an XBE image.
        //

        if ((ULONGLONG)RelativeLoadAddress + AlignedFileSize > IMGB_MAXIMUM_IMAGE_SIZE) {
            ImgbResourcePrintErrorAndExit(IDS_IMAGE_TOO_LARGE);
        }

        //
        // Increment the number of XBE sections.
        //

        ImgbXbeImageHeader->ImageHeader.NumberOfSections++;

        //
        // Increment the number of /INSERTFILE sections.
        //

        ImgbXbeImageHeader->SectionHeaders.NumberOfInsertFileSections++;

        //
        // Add the size of the section name to the total for the XBE section
        // header.
        //

        ImgbXbeImageHeader->SectionHeaders.SizeOfSectionNames +=
            strlen((PSTR)InsertFileLink->SectionName) + 1;

        //
        // If the section does not start on the same page as the last section,
        // then we'll need another shared page reference counter.
        //

        if (PAGE_ALIGN(RelativeLoadAddress) != PAGE_ALIGN(RelativeLoadAddress - 1)) {
            ImgbXbeImageHeader->SectionHeaders.NumberOfSharedPageReferenceCounts++;
        }

        //
        // If the inclusive ending byte of the section isn't on the same
        // page as the starting byte of the section, then we'll need another
        // shared page reference counter.
        //

        if (PAGE_ALIGN(RelativeLoadAddress) !=
            PAGE_ALIGN(RelativeLoadAddress + AlignedFileSize - 1)) {
            ImgbXbeImageHeader->SectionHeaders.NumberOfSharedPageReferenceCounts++;
        }

        RelativeLoadAddress += AlignedFileSize;

        NextListEntry = InsertFileLink->ListEntry.Flink;
    }

    ImgbXbeImageHeader->SizeOfInsertFilesImage = RelativeLoadAddress -
        ImgbXbeImageHeader->SizeOfExecutableImage;
}

VOID
ImgbLayoutOutputHeaders(
    VOID
    )
{
    ULONG SizeOfHeaders;
    PLIST_ENTRY NextListEntry;
    PIMGB_GENERIC_HEADER GenericHeader;

    //
    // Compute the size of the base image header.
    //

    SizeOfHeaders = sizeof(XBEIMAGE_HEADER);

    //
    // Walk through each of the headers and assign them base addresses.
    //

    NextListEntry = ImgbXbeImageHeader->HeadersListHead.Flink;

    while (NextListEntry != &ImgbXbeImageHeader->HeadersListHead) {

        GenericHeader = CONTAINING_RECORD(NextListEntry, IMGB_GENERIC_HEADER,
            HeadersListEntry);

        //
        // Compute how many bytes this portion of the header contributes to the
        // overall header.
        //

        if (GenericHeader->VirtualSize != 0) {

            //
            // Align the total header size up to a DWORD boundary.
            //

            SizeOfHeaders = (SizeOfHeaders + sizeof(DWORD) - 1) & (~(sizeof(DWORD) - 1));

            //
            // Assign this header its base address.
            //

            GenericHeader->VirtualAddress =
                (LPBYTE)XBEIMAGE_STANDARD_BASE_ADDRESS + SizeOfHeaders;

            //
            // Add this header's contribution to the total header size.
            //

            SizeOfHeaders += GenericHeader->VirtualSize;

        } else {
            GenericHeader->VirtualAddress = 0;
        }

        NextListEntry = GenericHeader->HeadersListEntry.Flink;
    }

    //
    // Align the total header size up to a DWORD boundary.
    //

    SizeOfHeaders = (SizeOfHeaders + sizeof(DWORD) - 1) & (~(sizeof(DWORD) - 1));

    //
    // Store the total size of the image headers back in the main image header.
    //

    ImgbXbeImageHeader->ImageHeader.SizeOfHeaders = SizeOfHeaders;

    //
    // Compute the number of bytes required to map the header, the executable
    // sections, and the /INSERTFILE sections.
    //

    ImgbXbeImageHeader->ImageHeader.SizeOfImage =
        ROUND_TO_PAGES(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders) +
        ImgbXbeImageHeader->SizeOfExecutableImage +
        ImgbXbeImageHeader->SizeOfInsertFilesImage;

    if (ImgbXbeImageHeader->ImageHeader.SizeOfImage > IMGB_MAXIMUM_IMAGE_SIZE) {
        ImgbResourcePrintErrorAndExit(IDS_IMAGE_TOO_LARGE);
    }

    //
    // If there were any contributions to the secondary headers, then place the
    // virtual addresses of these headers in the main image header.
    //

    if (ImgbXbeImageHeader->CertificateHeader.VirtualSize != 0) {
        ImgbXbeImageHeader->ImageHeader.Certificate = (PXBEIMAGE_CERTIFICATE)
            ImgbXbeImageHeader->CertificateHeader.VirtualAddress;
    }

    if (ImgbXbeImageHeader->SectionHeaders.VirtualSize != 0) {
        ImgbXbeImageHeader->ImageHeader.SectionHeaders =
            (PXBEIMAGE_SECTION)ImgbXbeImageHeader->SectionHeaders.VirtualAddress;
    }

    if (ImgbXbeImageHeader->ImportDescriptorHeader.VirtualSize != 0) {
        ImgbXbeImageHeader->ImageHeader.ImportDirectory =
            (PXBEIMAGE_IMPORT_DESCRIPTOR)ImgbXbeImageHeader->ImportDescriptorHeader.VirtualAddress;
    }

    if (ImgbXbeImageHeader->LibraryVersionHeader.VirtualSize != 0) {

        ImgbXbeImageHeader->ImageHeader.LibraryVersions =
            (PXBEIMAGE_LIBRARY_VERSION)ImgbXbeImageHeader->LibraryVersionHeader.VirtualAddress;

        if (ImgbXbeImageHeader->LibraryVersionHeader.XboxKernelOffset != MAXULONG) {
            ImgbXbeImageHeader->ImageHeader.XboxKernelLibraryVersion = (PXBEIMAGE_LIBRARY_VERSION)
                ((ULONG)ImgbXbeImageHeader->LibraryVersionHeader.VirtualAddress +
                ImgbXbeImageHeader->LibraryVersionHeader.XboxKernelOffset);
        }

        if (ImgbXbeImageHeader->LibraryVersionHeader.XapiOffset != MAXULONG) {
            ImgbXbeImageHeader->ImageHeader.XapiLibraryVersion = (PXBEIMAGE_LIBRARY_VERSION)
                ((ULONG)ImgbXbeImageHeader->LibraryVersionHeader.VirtualAddress +
                ImgbXbeImageHeader->LibraryVersionHeader.XapiOffset);
        }
    }

    if (ImgbXbeImageHeader->DebugPathsHeader.VirtualSize != 0) {

        ImgbXbeImageHeader->ImageHeader.DebugUnicodeFileName = (PWCHAR)
            (PWCHAR)ImgbXbeImageHeader->DebugPathsHeader.VirtualAddress;

        ImgbXbeImageHeader->ImageHeader.DebugPathName = (PUCHAR)
            ((ULONG)ImgbXbeImageHeader->DebugPathsHeader.VirtualAddress +
            (strlen(ImgbInputFileFilePart) + 1) * sizeof(WCHAR));

        ImgbXbeImageHeader->ImageHeader.DebugFileName =
            ImgbXbeImageHeader->ImageHeader.DebugPathName +
            (ImgbInputFileFilePart - ImgbInputFileFullPath);
    }

    if (ImgbXbeImageHeader->MicrosoftLogoHeader.VirtualSize != 0) {
        ImgbXbeImageHeader->ImageHeader.MicrosoftLogo = (PVOID)
            ImgbXbeImageHeader->MicrosoftLogoHeader.VirtualAddress;
        ImgbXbeImageHeader->ImageHeader.SizeOfMicrosoftLogo =
            ImgbXbeImageHeader->MicrosoftLogoHeader.VirtualSize;
    }
}

VOID
ImgbAddPEHeader(
    VOID
    )
{
    ULONG SizeOfNtHeaders;
    ULONG AdjustedSizeOfHeaders;

    //
    // Compute the number of bytes we'll need to store in the image in order to
    // add the NT header.  The size is counted as the number of bytes to the
    // first header, not the number of bytes stored in the header itself.
    //

    SizeOfNtHeaders = IMAGE_FIRST_SECTION(ImgbNtHeader)->VirtualAddress;

    if (ImgbEmitPEHeader) {

        //
        // The first section will start on a page boundary.  Take the current size
        // of the headers, add in the number of bytes required for the PE header,
        // and round that up to the page boundary.
        //

        AdjustedSizeOfHeaders =
            ROUND_TO_PAGES(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders + SizeOfNtHeaders);

        ImgbXbeImageHeader->ImageHeader.SizeOfHeaders = AdjustedSizeOfHeaders;

        //
        // Compute the virtual size of the PE headers.  These are placed
        // immediately before the first executable section's data.
        //

        ImgbXbeImageHeader->PEHeaderHeader.VirtualSize = SizeOfNtHeaders;

        //
        // Attach the PE header to the list of headers.
        //

        InsertTailList(&ImgbXbeImageHeader->HeadersListHead,
            &ImgbXbeImageHeader->PEHeaderHeader.HeadersListEntry);

        //
        // Compute the number of bytes required to map the header, the executable
        // sections, and the /INSERTFILE sections.
        //

        ImgbXbeImageHeader->ImageHeader.SizeOfImage =
            ROUND_TO_PAGES(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders) +
            ImgbXbeImageHeader->SizeOfExecutableImage +
            ImgbXbeImageHeader->SizeOfInsertFilesImage;

        if (ImgbXbeImageHeader->ImageHeader.SizeOfImage > IMGB_MAXIMUM_IMAGE_SIZE) {
            ImgbResourcePrintErrorAndExit(IDS_IMAGE_TOO_LARGE);
        }

    } else {
        AdjustedSizeOfHeaders =
            ROUND_TO_PAGES(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders);
    }

    //
    // Store the address of the PE headers back in the XBE headers.
    //

    ImgbXbeImageHeader->ImageHeader.NtBaseOfDll =
        ImgbXbeImageHeader->PEHeaderHeader.VirtualAddress = (PVOID)
        (XBEIMAGE_STANDARD_BASE_ADDRESS + AdjustedSizeOfHeaders -
        SizeOfNtHeaders);
}

VOID
ImgbRelocateImageAfterHeaders(
    VOID
    )
{
    ULONG SizeOfHeadersPageAligned;
    ULONG NewBaseAddress;
    ULONG RelocationBytesRemaining;
    PIMAGE_BASE_RELOCATION RelocationBlock;

    //
    // Start sections on a page aligned byte offset from the headers.
    //

    SizeOfHeadersPageAligned = ROUND_TO_PAGES(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders);

    //
    // Compute the new PE base address of the image.
    //

    NewBaseAddress = XBEIMAGE_STANDARD_BASE_ADDRESS + SizeOfHeadersPageAligned -
        IMAGE_FIRST_SECTION(ImgbNtHeader)->VirtualAddress;

    //
    // Verify that the image still has its relocation records.
    //

    RelocationBytesRemaining = ImgbNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

    if (RelocationBytesRemaining == 0) {
        ImgbResourcePrintErrorAndExit(IDS_MISSING_RELOCATIONS);
    }

    RelocationBlock = (PIMAGE_BASE_RELOCATION)ImgbImageDataDirectoryToData(ImgbNtHeader,
        IMAGE_DIRECTORY_ENTRY_BASERELOC);

    //
    // Relocate the image.
    //

    ImgbRelocateImage(ImgbNtHeader, (ULONG)ImgbNtHeader->OptionalHeader.ImageBase,
        NewBaseAddress, RelocationBlock, RelocationBytesRemaining);

    //
    // Relocate the addresses stored in the XBE image header.
    //

    if (ImgbXbeImageHeader->ImageHeader.AddressOfEntryPoint != NULL) {
        ImgbXbeImageHeader->ImageHeader.AddressOfEntryPoint = (PXBEIMAGE_ENTRY_POINT)
            ((ULONG)ImgbXbeImageHeader->ImageHeader.AddressOfEntryPoint + NewBaseAddress);
    }

    if (ImgbXbeImageHeader->ImageHeader.XboxKernelThunkData != NULL) {
        ImgbXbeImageHeader->ImageHeader.XboxKernelThunkData = (PIMAGE_THUNK_DATA)
            ((ULONG)ImgbXbeImageHeader->ImageHeader.XboxKernelThunkData + NewBaseAddress);
    }

    if (ImgbXbeImageHeader->ImageHeader.TlsDirectory != NULL) {

        ImgbXbeImageHeader->ImageHeader.TlsDirectory = (PIMAGE_TLS_DIRECTORY)
            ((ULONG)ImgbXbeImageHeader->ImageHeader.TlsDirectory + NewBaseAddress);

        //
        // Now that the image has been relocated, go back and change the
        // addresses of the raw thread data to point at the TLS raw data header.
        // Note that if all of the TLS data has been turned into zero fill, then
        // VirtualAddress and VirtualSize are zero and the following will cause
        // the pointers to be NULL.
        //

        ImgbXbeImageHeader->TlsRawDataHeader.TlsDirectory->StartAddressOfRawData =
            (ULONG)ImgbXbeImageHeader->TlsRawDataHeader.VirtualAddress;
        ImgbXbeImageHeader->TlsRawDataHeader.TlsDirectory->EndAddressOfRawData =
            (ULONG)ImgbXbeImageHeader->TlsRawDataHeader.VirtualAddress +
            ImgbXbeImageHeader->TlsRawDataHeader.VirtualSize;
    }

    //
    // Remember the base address for future fixups.
    //

    ImgbXbeImageHeader->NewBaseAddress = NewBaseAddress;
}

VOID
ImgbWriteOutputFile(
    LPCVOID lpvBuffer,
    DWORD cbNumberOfBytes
    )
{
    DWORD cbWritten;

    if (!WriteFile(ImgbOutputFileHandle, lpvBuffer, cbNumberOfBytes, &cbWritten,
        NULL) || (cbWritten != cbNumberOfBytes)) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_WRITE_OUTPUT_FILE, ImgbOutputFilePath);
    }
}

VOID
ImgbSeekByteOutputFile(
    ULONG dwFileByteOffset
    )
{
    LARGE_INTEGER liByteOffset;

    liByteOffset.QuadPart = dwFileByteOffset;

    if (!SetFilePointerEx(ImgbOutputFileHandle, liByteOffset, NULL, FILE_BEGIN)) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_WRITE_OUTPUT_FILE, ImgbOutputFilePath);
    }
}

VOID
ImgbReadOutputFile(
    LPVOID lpvBuffer,
    DWORD cbNumberOfBytes
    )
{
    DWORD cbRead;

    if (!ReadFile(ImgbOutputFileHandle, lpvBuffer, cbNumberOfBytes, &cbRead,
        NULL) || (cbRead != cbNumberOfBytes)) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_WRITE_OUTPUT_FILE, ImgbOutputFilePath);
    }
}

VOID
ImgbReadInputFile(
    LPVOID lpvBuffer,
    DWORD cbNumberOfBytes
    )
{
    DWORD cbRead;

    if (!ReadFile(ImgbInputFileHandle, lpvBuffer, cbNumberOfBytes, &cbRead,
        NULL) || (cbRead != cbNumberOfBytes)) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_READ_INPUT_FILE, ImgbInputFilePath);
    }
}

VOID
ImgbWriteZeroPadding(
    ULONG NumberOfBytes
    )
{
    BYTE Zeroes[1024];
    ULONG BytesThisPass;

    ZeroMemory(Zeroes, sizeof(Zeroes));

    while (NumberOfBytes > 0) {
        BytesThisPass = min(NumberOfBytes, sizeof(Zeroes));
        ImgbWriteOutputFile(Zeroes, BytesThisPass);
        NumberOfBytes -= BytesThisPass;
    }
}

VOID
ImgbSignImageHeaders(
    VOID
    )
{
    PUCHAR PrivateKeyData;
    PXBEIMAGE_HEADER ImageHeader;
    ULONG HeaderDigestLength;
    UCHAR HeaderDigest[XC_DIGEST_LEN];

    PrivateKeyData = ImgbPrivateKeyData;

    //
    // Allocate a buffer to hold all of the image headers.
    //

    ImageHeader = (PXBEIMAGE_HEADER)
        ImgbAllocateMemory(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders);

    //
    // Read the image headers back from the output file.
    //

    ImgbSeekByteOutputFile(0);
    ImgbReadOutputFile(ImageHeader, ImgbXbeImageHeader->ImageHeader.SizeOfHeaders);

    //
    // The encrypted header digest starts at the field after the encrypted
    // header field in the main image header.
    //

    HeaderDigestLength = ImgbXbeImageHeader->ImageHeader.SizeOfHeaders -
        FIELD_OFFSET(XBEIMAGE_HEADER, BaseAddress);

    //
    // Calculate the SHA1 digest for the headers.
    //

    XCCalcDigest((PUCHAR)&ImageHeader->BaseAddress, HeaderDigestLength,
        HeaderDigest);

    ImgbFreeMemory(ImageHeader);

    //
    // Encrypt the header digest.
    //

    XCSignDigest(HeaderDigest, PrivateKeyData,
        ImgbXbeImageHeader->ImageHeader.EncryptedDigest);

    //
    // Write out the main image header with the encrypted header digest.
    //

    ImgbSeekByteOutputFile(0);
    ImgbWriteOutputFile(&ImgbXbeImageHeader->ImageHeader, sizeof(XBEIMAGE_HEADER));
}

VOID
ImgbConfoundHeaderData(
    VOID
    )
{
    ULONG *Key;

    //
    // Use part of the public key as the confounder
    //

    Key = (ULONG*)&ImgbPublicKeyData[128];

    //
    // Confound the image base address and the location of the kernel import
    // data
    //

    *(ULONG *)&ImgbXbeImageHeader->ImageHeader.AddressOfEntryPoint ^=
        Key[0] ^ Key[4];
    *(ULONG *)&ImgbXbeImageHeader->ImageHeader.XboxKernelThunkData ^=
        Key[1] ^ Key[2];
}

VOID
ImgbEmitOutputFile(
    VOID
    )
{
    PLIST_ENTRY NextListEntry;
    PIMGB_GENERIC_HEADER GenericHeader;

    //
    // Initialize the header bytes to zero.
    //

    ImgbSeekByteOutputFile(0);
    ImgbWriteZeroPadding(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders);

    //
    // Walk through each of the headers and write them out.
    //

    NextListEntry = ImgbXbeImageHeader->HeadersListHead.Flink;

    while (NextListEntry != &ImgbXbeImageHeader->HeadersListHead) {

        GenericHeader = CONTAINING_RECORD(NextListEntry, IMGB_GENERIC_HEADER,
            HeadersListEntry);

        //
        // If this header is supposed to be emitted, then write it out.
        //

        if (GenericHeader->VirtualSize != 0) {
            ImgbSeekByteOutputFile((ULONG)GenericHeader->VirtualAddress -
                (ULONG)XBEIMAGE_STANDARD_BASE_ADDRESS);
            GenericHeader->Write();
        }

        NextListEntry = GenericHeader->HeadersListEntry.Flink;
    }

    //
    // Write out the main image header without the encrypted header digest.
    //

    ImgbSeekByteOutputFile(0);
    ImgbWriteOutputFile(&ImgbXbeImageHeader->ImageHeader, sizeof(XBEIMAGE_HEADER));

    //
    // Compute the digest of the image headers, encrypt the digest, and rewrite
    // the main image header with the encrypted header digest.
    //

    ImgbSignImageHeaders();
}

VOID
_IMGB_XBEIMAGE_CERTIFICATE_HEADER::Write(
    VOID
    )
{
    XBEIMAGE_CERTIFICATE Certificate;

    RtlZeroMemory(&Certificate, sizeof(Certificate));

    Certificate.OriginalSizeOfCertificate =
        Certificate.SizeOfCertificate = sizeof(XBEIMAGE_CERTIFICATE);
    Certificate.TimeDateStamp = ImgbXbeImageHeader->ImageHeader.TimeDateStamp;
    Certificate.TitleID = ImgbTestTitleID;
    Certificate.Version = ImgbVersion;
    Certificate.AllowedMediaTypes = ImgbTestAllowedMediaTypes;
    Certificate.GameRegion = ImgbTestGameRegion;
    Certificate.GameRatings = ImgbTestGameRatings;

    RtlCopyMemory(Certificate.TitleName, ImgbTestTitleName, sizeof(ImgbTestTitleName));
    RtlCopyMemory(Certificate.AlternateTitleIDs, ImgbTestAlternateTitleIDs,
        sizeof(ImgbTestAlternateTitleIDs));

    RtlCopyMemory(Certificate.LANKey, ImgbTestLANKey, XBEIMAGE_CERTIFICATE_KEY_LENGTH);
    RtlCopyMemory(Certificate.SignatureKey, ImgbTestSignatureKey, XBEIMAGE_CERTIFICATE_KEY_LENGTH);

    RtlCopyMemory(Certificate.AlternateSignatureKeys,
        ImgbTestAlternateSignatureKeys, sizeof(ImgbTestAlternateSignatureKeys));

    ImgbWriteOutputFile(&Certificate, sizeof(XBEIMAGE_CERTIFICATE));
}

VOID
_IMGB_XBEIMAGE_IMPORT_DESCRIPTOR_HEADER::Write(
    VOID
    )
{
    ULONG ImportDirectoryByteOffset;
    ULONG ImportDirectoryNameByteOffset;
    ULONG ImportDescriptorBytesRemaining;
    PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
    LPCSTR ImageName;
    XBEIMAGE_IMPORT_DESCRIPTOR XbeImportDescriptor;
    WCHAR UnicodeImageName[MAX_PATH];

    //
    // Compute the byte offsets for the import directories and the start of the
    // import image names.
    //

    ImportDirectoryByteOffset = (ULONG)ImgbXbeImageHeader->ImportDescriptorHeader.VirtualAddress -
        XBEIMAGE_STANDARD_BASE_ADDRESS;
    ImportDirectoryNameByteOffset = ImportDirectoryByteOffset +
        (ImgbXbeImageHeader->ImportDescriptorHeader.NumberOfNonKernelImports + 1) *
        sizeof(XBEIMAGE_IMPORT_DESCRIPTOR);

    //
    // Walk through each of the import descriptors.
    //

    ImportDescriptorBytesRemaining =
        ImgbNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
    ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)
        ImgbImageDataDirectoryToData(ImgbNtHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);

    while (ImportDescriptorBytesRemaining >= sizeof(IMAGE_IMPORT_DESCRIPTOR)) {

        //
        // If we find the NULL import descriptor, then break out now.
        //

        if (ImportDescriptor->Characteristics == 0) {
            break;
        }

        //
        // If this isn't the kernel import, then add the import to the output
        // file.
        //

        ImageName = (LPCSTR)ImgbVirtualAddressToData(ImgbNtHeader, ImportDescriptor->Name);

        if (_stricmp(ImageName, "xboxkrnl.exe") != 0) {

            MultiByteToWideChar(CP_ACP, 0, ImageName, -1, UnicodeImageName,
                MAX_PATH);

            ImgbSeekByteOutputFile(ImportDirectoryNameByteOffset);
            ImgbWriteOutputFile(UnicodeImageName, wcslen(UnicodeImageName) * sizeof(WCHAR));

            XbeImportDescriptor.ImageThunkData =
                (PIMAGE_THUNK_DATA)(ImportDescriptor->FirstThunk +
                (ULONG)ImgbXbeImageHeader->NewBaseAddress);
            XbeImportDescriptor.ImageName = (PWCHAR)(ImportDirectoryNameByteOffset +
                XBEIMAGE_STANDARD_BASE_ADDRESS);

            ImgbSeekByteOutputFile(ImportDirectoryByteOffset);
            ImgbWriteOutputFile(&XbeImportDescriptor, sizeof(XBEIMAGE_IMPORT_DESCRIPTOR));

            ImportDirectoryByteOffset += sizeof(XBEIMAGE_IMPORT_DESCRIPTOR);
            ImportDirectoryNameByteOffset += (strlen(ImageName) + sizeof(UCHAR)) * sizeof(WCHAR);
        }

        //
        // Advance to the next import descriptor.
        //

        ImportDescriptor++;
        ImportDescriptorBytesRemaining -= sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }

    //
    // Write out the terminating import descriptor.
    //

    XbeImportDescriptor.ImageThunkData = NULL;
    XbeImportDescriptor.ImageName = NULL;

    ImgbSeekByteOutputFile(ImportDirectoryByteOffset);
    ImgbWriteOutputFile(&XbeImportDescriptor, sizeof(XBEIMAGE_IMPORT_DESCRIPTOR));
}

VOID
_IMGB_TLS_RAW_DATA_HEADER::Write(
    VOID
    )
{
    ImgbWriteOutputFile(ImgbXbeImageHeader->TlsRawDataHeader.RawData,
        ImgbXbeImageHeader->TlsRawDataHeader.VirtualSize);
}

VOID
_IMGB_XBEIMAGE_LIBRARY_VERSION_HEADER::Write(
    VOID
    )
{
    ImgbWriteOutputFile(ImgbXbeImageHeader->LibraryVersionHeader.LibraryVersions,
        ImgbXbeImageHeader->LibraryVersionHeader.VirtualSize);
}

BOOLEAN
ImgbCheckForReadOnlyPage(
    PXBEIMAGE_SECTION SectionHeaders,
    ULONG VirtualAddress
    )
{
    ULONG Index;
    PXBEIMAGE_SECTION Section;
    ULONG StartingAddress;
    ULONG EndingAddress;

    //
    // Walk through the section headers and check if the virtual address is on a
    // page with contributions from a writeable section.
    //

    for (Index = 0, Section = SectionHeaders;
        Index < ImgbXbeImageHeader->ImageHeader.NumberOfSections;
        Index++, Section++) {

        //
        // Only consider sections that have bytes allocated to them and that are
        // writeable sections.
        //

        if ((Section->VirtualSize != 0) &&
            ((Section->SectionFlags & XBEIMAGE_SECTION_WRITEABLE) != 0)) {

            StartingAddress = (ULONG)PAGE_ALIGN(Section->VirtualAddress);
            EndingAddress = (ULONG)PAGE_ALIGN(Section->VirtualAddress +
                Section->VirtualSize + PAGE_SIZE - 1);

            if (StartingAddress <= VirtualAddress && VirtualAddress < EndingAddress) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

VOID
ImgbPostProcessSectionHeaders(
    VOID
    )
{
    PXBEIMAGE_SECTION SectionHeaders;
    ULONG SectionHeaderByteOffset;
    ULONG Index;
    PXBEIMAGE_SECTION Section;

    SectionHeaders = (PXBEIMAGE_SECTION)
        ImgbAllocateMemory(ImgbXbeImageHeader->ImageHeader.NumberOfSections *
        sizeof(XBEIMAGE_SECTION));

    SectionHeaderByteOffset = (ULONG)ImgbXbeImageHeader->SectionHeaders.VirtualAddress -
         XBEIMAGE_STANDARD_BASE_ADDRESS;

    //
    // Read the headers that we've written already written out to the file.
    //

    ImgbSeekByteOutputFile(SectionHeaderByteOffset);
    ImgbReadOutputFile(SectionHeaders, ImgbXbeImageHeader->ImageHeader.NumberOfSections *
        sizeof(XBEIMAGE_SECTION));

    //
    // Walk through the section headers and apply the head and tail read-only
    // page flags.
    //

    for (Index = 0, Section = SectionHeaders;
        Index < ImgbXbeImageHeader->ImageHeader.NumberOfSections;
        Index++, Section++) {

        //
        // Only consider sections that have bytes allocated to them and that is
        // not a writeable section.
        //

        if ((Section->VirtualSize != 0) &&
            ((Section->SectionFlags & XBEIMAGE_SECTION_WRITEABLE) == 0)) {

            if (ImgbCheckForReadOnlyPage(SectionHeaders,
                Section->VirtualAddress)) {
                Section->SectionFlags |= XBEIMAGE_SECTION_HEAD_PAGE_READONLY;
            }

            if (ImgbCheckForReadOnlyPage(SectionHeaders,
                Section->VirtualAddress + Section->VirtualSize - 1)) {
                Section->SectionFlags |= XBEIMAGE_SECTION_TAIL_PAGE_READONLY;
            }
        }
    }

    //
    // Write the post processed headers back out to the file.
    //

    ImgbSeekByteOutputFile(SectionHeaderByteOffset);
    ImgbWriteOutputFile(SectionHeaders, ImgbXbeImageHeader->ImageHeader.NumberOfSections *
        sizeof(XBEIMAGE_SECTION));

    ImgbFreeMemory(SectionHeaders);
}

VOID
_IMGB_XBEIMAGE_SECTION_HEADERS::Write(
    VOID
    )
{
    ULONG SectionHeaderByteOffset;
    ULONG SectionHeaderNameByteOffset;
    ULONG FirstSectionBaseAddress;
    ULONG FileByteOffset;
    PULONG ImageFileByteOffsets;
    ULONG CurrentSection;
    PIMAGE_SECTION_HEADER SectionHeader;
    PVOID EndOfSectionData;
    ULONG RawDataRemaining;
    PLIST_ENTRY NextListEntry;
    PIMGB_NOPRELOAD NoPreloadLink;
    ULONG SectionIndex;
    UCHAR SectionName[IMAGE_SIZEOF_SHORT_NAME + 1];
    XBEIMAGE_SECTION XbeSection;
    PUSHORT SharedReferenceCount;
    PUSHORT EndingSharedReferenceCount;
    BOOLEAN FirstSharedReferenceCount;
    ULONG LastEndingVirtualAddress;
    PVOID SectionData;
    PIMGB_INSERTFILE InsertFileLink;
    ULONG InsertFileVirtualAddress;
    ULONG AlignedFileSize;
    ULONG FileBytesRemaining;
    ULONG FileBytesThisPass;
    A_SHA_CTX SHAHash;

    SectionHeaderByteOffset = (ULONG)ImgbXbeImageHeader->SectionHeaders.VirtualAddress -
         XBEIMAGE_STANDARD_BASE_ADDRESS;
    SectionHeaderNameByteOffset = SectionHeaderByteOffset +
        ImgbXbeImageHeader->ImageHeader.NumberOfSections * sizeof(XBEIMAGE_SECTION) +
        ImgbXbeImageHeader->SectionHeaders.NumberOfSharedPageReferenceCounts * sizeof(USHORT);

    //
    // Compute the base virtual address of the first section.
    //

    FirstSectionBaseAddress = XBEIMAGE_STANDARD_BASE_ADDRESS +
        ROUND_TO_PAGES(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders) -
        IMAGE_FIRST_SECTION(ImgbNtHeader)->VirtualAddress;

    //
    // Allocate stack storage to hold the file byte offsets of each of the image
    // sections.
    //

    ImageFileByteOffsets = (PULONG)_alloca(ImgbXbeImageHeader->SectionHeaders.NumberOfExecutableSections * sizeof(ULONG));
    RtlZeroMemory(ImageFileByteOffsets, ImgbXbeImageHeader->SectionHeaders.NumberOfExecutableSections * sizeof(ULONG));

    //
    // The first image section will always start at the first page after the
    // image headers.
    //

    FileByteOffset = ROUND_TO_PAGES(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders);

    //
    // Pass 1: Loop over all of the PRELOAD executable sections and assign file
    // byte offsets.
    //

    SectionHeader = IMAGE_FIRST_SECTION(ImgbNtHeader);

    for (CurrentSection = 0; CurrentSection < ImgbXbeImageHeader->SectionHeaders.NumberOfExecutableSections;
        CurrentSection++, SectionHeader++) {

        //
        // When building with a non-paged size alignment, the linker will always
        // expand a section that contains uninitialized data to its full size in
        // the PE file, so here, we try to recover the actual size of the raw
        // data by scanning backwards for the first non-zero byte.
        //

        if (SectionHeader->SizeOfRawData > 0) {

            EndOfSectionData = (PUCHAR)ImgbVirtualAddressToData(ImgbNtHeader,
                SectionHeader->VirtualAddress) + SectionHeader->SizeOfRawData;
            RawDataRemaining = SectionHeader->SizeOfRawData;

            //
            // Handle the first three misaligned bytes so that we can
            // efficiently check only DWORDs below.
            //

            while ((RawDataRemaining > 0) &&
                ((RawDataRemaining & (sizeof(ULONG) - 1)) != 0)) {

                if (*((PUCHAR)EndOfSectionData - 1) != 0) {
                    break;
                }

                EndOfSectionData = (PUCHAR)EndOfSectionData - 1;
                RawDataRemaining -= sizeof(UCHAR);
            }

            //
            // Handle the aligned DWORDs.
            //

            while (RawDataRemaining > sizeof(ULONG)) {

                if (*((PULONG)EndOfSectionData - 1) != 0) {
                    break;
                }

                EndOfSectionData = (PULONG)EndOfSectionData - 1;
                RawDataRemaining -= sizeof(ULONG);
            }

            SectionHeader->SizeOfRawData = RawDataRemaining;
        }

        if ((SectionHeader->Characteristics & IMAGE_SCN_MEM_PRELOAD) != 0) {

            ImageFileByteOffsets[CurrentSection] = FileByteOffset;
            FileByteOffset += ROUND_TO_PAGES(SectionHeader->SizeOfRawData);
        }
    }

    //
    // Pass 2: Loop over all of the PRELOAD /INSERTFILE sections and assign
    // fiel byte offsets.
    //

    NextListEntry = ImgbInsertFileList.Flink;

    while (NextListEntry != &ImgbInsertFileList) {

        InsertFileLink = CONTAINING_RECORD(NextListEntry, IMGB_INSERTFILE,
            ListEntry);

        if (!InsertFileLink->NoPreload) {

            InsertFileLink->FileByteOffset = FileByteOffset;

            FileByteOffset += ROUND_TO_PAGES(InsertFileLink->FileSize);
        }

        NextListEntry = InsertFileLink->ListEntry.Flink;
    }

    //
    // Pass 3: Loop over all of the NOPRELOAD sections and assign file byte
    // offsets.  NOPRELOAD sections are ordered in the file by the order they
    // were supplied by the user.
    //

    NextListEntry = ImgbNoPreloadList.Flink;

    while (NextListEntry != &ImgbNoPreloadList) {

        NoPreloadLink = CONTAINING_RECORD(NextListEntry, IMGB_NOPRELOAD,
            ListEntry);

        SectionHeader = ImgbNameToSectionHeader(ImgbNtHeader,
            (PUCHAR)NoPreloadLink->SectionName, &SectionIndex);

        if ((SectionHeader != NULL) &&
            (SectionIndex < ImgbXbeImageHeader->SectionHeaders.NumberOfExecutableSections) &&
            (ImageFileByteOffsets[SectionIndex] == 0)) {

            ImageFileByteOffsets[SectionIndex] = FileByteOffset;
            FileByteOffset += ROUND_TO_PAGES(SectionHeader->SizeOfRawData);
        }

        NextListEntry = NoPreloadLink->ListEntry.Flink;
    }

    //
    // Pass 4: Loop over all of the NOPRELOAD /INSERTFILE sections and assign
    // fiel byte offsets.
    //

    NextListEntry = ImgbInsertFileList.Flink;

    while (NextListEntry != &ImgbInsertFileList) {

        InsertFileLink = CONTAINING_RECORD(NextListEntry, IMGB_INSERTFILE,
            ListEntry);

        if (InsertFileLink->NoPreload) {

            InsertFileLink->FileByteOffset = FileByteOffset;

            FileByteOffset += ROUND_TO_PAGES(InsertFileLink->FileSize);
        }

        NextListEntry = InsertFileLink->ListEntry.Flink;
    }

    //
    // Compute the first pointer to the shared page reference counters.
    //

    SharedReferenceCount = (PUSHORT)((ULONG)ImgbXbeImageHeader->SectionHeaders.VirtualAddress +
        ImgbXbeImageHeader->ImageHeader.NumberOfSections * sizeof(XBEIMAGE_SECTION));
    EndingSharedReferenceCount = SharedReferenceCount +
        ImgbXbeImageHeader->SectionHeaders.NumberOfSharedPageReferenceCounts;
    FirstSharedReferenceCount = TRUE;

    //
    // Write out the executable sections.
    //

    LastEndingVirtualAddress = 0;
    SectionHeader = IMAGE_FIRST_SECTION(ImgbNtHeader);

    for (CurrentSection = 0; CurrentSection < ImgbXbeImageHeader->SectionHeaders.NumberOfExecutableSections;
        CurrentSection++, SectionHeader++) {

        if (SectionHeader->SizeOfRawData > 0) {
            SectionData = ImgbVirtualAddressToData(ImgbNtHeader, SectionHeader->VirtualAddress);
        } else {
            SectionData = NULL;
        }

        //
        // Write out the section.
        //

        ImgbSeekByteOutputFile(ImageFileByteOffsets[CurrentSection]);
        ImgbWriteOutputFile(SectionData, SectionHeader->SizeOfRawData);

        ImgbWriteZeroPadding(PAGE_SIZE - BYTE_OFFSET(SectionHeader->SizeOfRawData));

        //
        // Build the section name on the stack.
        //

        memcpy(SectionName, SectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME);
        SectionName[IMAGE_SIZEOF_SHORT_NAME] = '\0';

        //
        // Write out the section name.
        //

        ImgbSeekByteOutputFile(SectionHeaderNameByteOffset);
        ImgbWriteOutputFile(SectionName, strlen((PSTR)SectionName) + 1);

        //
        // Build the section header.
        //

        XbeSection.VirtualAddress = SectionHeader->VirtualAddress +
            FirstSectionBaseAddress;
        XbeSection.VirtualSize = max(SectionHeader->SizeOfRawData,
            SectionHeader->Misc.VirtualSize);
        XbeSection.PointerToRawData = ImageFileByteOffsets[CurrentSection];
        XbeSection.SizeOfRawData = SectionHeader->SizeOfRawData;
        XbeSection.SectionName = (PUCHAR)(SectionHeaderNameByteOffset +
            XBEIMAGE_STANDARD_BASE_ADDRESS);
        XbeSection.SectionReferenceCount = 0;

        //
        // Compute the section flags.
        //

        XbeSection.SectionFlags = XBEIMAGE_SECTION_EXECUTABLE;

        if ((SectionHeader->Characteristics & IMAGE_SCN_MEM_WRITE) != 0) {
            XbeSection.SectionFlags |= XBEIMAGE_SECTION_WRITEABLE;
        }

        if ((SectionHeader->Characteristics & IMAGE_SCN_MEM_PRELOAD) != 0) {
            XbeSection.SectionFlags |= XBEIMAGE_SECTION_PRELOAD;
        }

        //
        // Compute the section digest.
        //

        XCCalcDigest((PUCHAR)SectionData, SectionHeader->SizeOfRawData, XbeSection.SectionDigest);

        //
        // Compute the pointers to the shared page reference counters for this
        // section.
        //

        if (!FirstSharedReferenceCount &&
            (PAGE_ALIGN(XbeSection.VirtualAddress) != PAGE_ALIGN(LastEndingVirtualAddress))) {
            SharedReferenceCount++;
        }

        XbeSection.HeadSharedPageReferenceCount = SharedReferenceCount;

        if (PAGE_ALIGN(XbeSection.VirtualAddress) !=
            PAGE_ALIGN(XbeSection.VirtualAddress + XbeSection.VirtualSize - 1)) {
            SharedReferenceCount++;
        }

        XbeSection.TailSharedPageReferenceCount = SharedReferenceCount;

        FirstSharedReferenceCount = FALSE;
        LastEndingVirtualAddress = XbeSection.VirtualAddress +
            XbeSection.VirtualSize - 1;

        //
        // Write out the section header.
        //

        ImgbSeekByteOutputFile(SectionHeaderByteOffset);
        ImgbWriteOutputFile(&XbeSection, sizeof(XBEIMAGE_SECTION));

        SectionHeaderByteOffset += sizeof(XBEIMAGE_SECTION);
        SectionHeaderNameByteOffset += strlen((PSTR)SectionName) + 1;
    }

    //
    // Write out the /INSERTFILE sections.
    //

    InsertFileVirtualAddress = XBEIMAGE_STANDARD_BASE_ADDRESS +
        ROUND_TO_PAGES(ImgbXbeImageHeader->ImageHeader.SizeOfHeaders) +
        ImgbXbeImageHeader->SizeOfExecutableImage;
    NextListEntry = ImgbInsertFileList.Flink;

    while (NextListEntry != &ImgbInsertFileList) {

        InsertFileLink = CONTAINING_RECORD(NextListEntry, IMGB_INSERTFILE,
            ListEntry);

        AlignedFileSize = (InsertFileLink->FileSize +
            IMGB_INSERTFILE_SECTION_ALIGNMENT - 1) & ~(IMGB_INSERTFILE_SECTION_ALIGNMENT - 1);

        //
        // Write out the section name.
        //

        ImgbSeekByteOutputFile(SectionHeaderNameByteOffset);
        ImgbWriteOutputFile(InsertFileLink->SectionName,
            strlen((PSTR)InsertFileLink->SectionName) + 1);

        //
        // Build the section header.
        //

        XbeSection.VirtualAddress = InsertFileVirtualAddress;
        XbeSection.VirtualSize = InsertFileLink->FileSize;
        XbeSection.PointerToRawData = InsertFileLink->FileByteOffset;
        XbeSection.SizeOfRawData = InsertFileLink->FileSize;
        XbeSection.SectionName = (PUCHAR)(SectionHeaderNameByteOffset +
            XBEIMAGE_STANDARD_BASE_ADDRESS);
        XbeSection.SectionReferenceCount = 0;

        //
        // Compute the section flags.
        //

        XbeSection.SectionFlags = XBEIMAGE_SECTION_INSERTFILE;

        if (!InsertFileLink->ReadOnly) {
            XbeSection.SectionFlags |= XBEIMAGE_SECTION_WRITEABLE;
        }

        if (!InsertFileLink->NoPreload) {
            XbeSection.SectionFlags |= XBEIMAGE_SECTION_PRELOAD;
        }

        //
        // Compute the pointers to the shared page reference counters for this
        // section.
        //

        if (!FirstSharedReferenceCount &&
            (PAGE_ALIGN(XbeSection.VirtualAddress) != PAGE_ALIGN(LastEndingVirtualAddress))) {
            SharedReferenceCount++;
        }

        XbeSection.HeadSharedPageReferenceCount = SharedReferenceCount;

        if (PAGE_ALIGN(XbeSection.VirtualAddress) !=
            PAGE_ALIGN(XbeSection.VirtualAddress + XbeSection.VirtualSize - 1)) {
            SharedReferenceCount++;
        }

        XbeSection.TailSharedPageReferenceCount = SharedReferenceCount;

        FirstSharedReferenceCount = FALSE;
        LastEndingVirtualAddress = XbeSection.VirtualAddress +
            XbeSection.VirtualSize - 1;

        //
        // Write out the section and compute the section digest.
        //

        ImgbSeekByteOutputFile(InsertFileLink->FileByteOffset);

        FileBytesRemaining = InsertFileLink->FileSize;

        A_SHAInit(&SHAHash);
        A_SHAUpdate(&SHAHash, (PUCHAR)&FileBytesRemaining, sizeof(ULONG));

        while (FileBytesRemaining > 0) {

            FileBytesThisPass = FileBytesRemaining >= sizeof(ImgbTransferBuffer) ?
                sizeof(ImgbTransferBuffer) : FileBytesRemaining;

            if (!ReadFile(InsertFileLink->FileHandle, ImgbTransferBuffer,
                FileBytesThisPass, &FileBytesThisPass, NULL)) {
                ImgbResourcePrintErrorAndExit(IDS_CANNOT_READ_INPUT_FILE,
                    InsertFileLink->FilePath);
            }

            A_SHAUpdate(&SHAHash, ImgbTransferBuffer, FileBytesThisPass);

            ImgbWriteOutputFile(ImgbTransferBuffer, FileBytesThisPass);

            FileBytesRemaining -= FileBytesThisPass;
        }

        ImgbWriteZeroPadding(PAGE_SIZE - BYTE_OFFSET(XbeSection.SizeOfRawData));

        A_SHAFinal(&SHAHash, XbeSection.SectionDigest);

        //
        // Write out the section header.
        //

        ImgbSeekByteOutputFile(SectionHeaderByteOffset);
        ImgbWriteOutputFile(&XbeSection, sizeof(XBEIMAGE_SECTION));

        SectionHeaderByteOffset += sizeof(XBEIMAGE_SECTION);
        SectionHeaderNameByteOffset += strlen((PSTR)InsertFileLink->SectionName) + 1;

        InsertFileVirtualAddress += AlignedFileSize;

        NextListEntry = InsertFileLink->ListEntry.Flink;
    }

    //
    // Verify that we didn't screw up the code that computed the number of
    // shared page reference counters.
    //

    if (SharedReferenceCount >= EndingSharedReferenceCount) {
        ImgbResourcePrintErrorAndExit(IDS_INTERNAL_TOOL_ERROR, __LINE__);
    }

    //
    // Apply any post processing to the section headers, such as finding
    // sections that have read-only head and tail pages.
    //

    ImgbPostProcessSectionHeaders();
}

VOID
_IMGB_DEBUG_PATHS_HEADER::Write(
    VOID
    )
{
    WCHAR FileNameUnicode[MAX_PATH];
    LPSTR pszTemp;
    LPWSTR pwszTemp;

    //
    // Convert the file name portion to "Unicode", which as far as the debugger
    // is concerned is only 16-bit wide versions of the ANSI characters.
    //

    pszTemp = ImgbInputFileFilePart;
    pwszTemp = FileNameUnicode;

    while (*pszTemp != '\0') {
        *pwszTemp++ = (WCHAR)*pszTemp++;
    }

    *pwszTemp = L'\0';

    //
    // Write out the strings.
    //

    ImgbWriteOutputFile(FileNameUnicode, (wcslen(FileNameUnicode) + 1) * sizeof(WCHAR));

    ImgbWriteOutputFile(ImgbInputFileFullPath, strlen(ImgbInputFileFullPath) + 1);
}

VOID
_IMGB_MICROSOFT_LOGO_HEADER::Write(
    VOID
    )
{
    ImgbWriteOutputFile(ImgbMicrosoftLogo, sizeof(ImgbMicrosoftLogo));
}

VOID
_IMGB_PEHEADER_HEADER::Write(
    VOID
    )
{
    PVOID OriginalNtHeader;

    //
    // The supplied NT header pointer is to a copy-on-write view of the file.
    // The headers may have been altered while preparing the XBE image, so we
    // want to go back to the original file and re-read the headers.
    //

    OriginalNtHeader = ImgbAllocateMemory(ImgbXbeImageHeader->PEHeaderHeader.VirtualSize);

    ImgbReadInputFile(OriginalNtHeader,
        ImgbXbeImageHeader->PEHeaderHeader.VirtualSize);

    ImgbWriteOutputFile(OriginalNtHeader,
        ImgbXbeImageHeader->PEHeaderHeader.VirtualSize);

    ImgbFreeMemory(OriginalNtHeader);
}

VOID
ImgbBuildOutputFile(
    VOID
    )
{
    PIMAGE_DOS_HEADER DosHeader;

    //
    // Verify that the input file is large enough to contain a DOS executable
    // header, that the DOS executable signature matches, and that the input
    // file is large enough to contain the NT headers.
    //

    DosHeader = (PIMAGE_DOS_HEADER)ImgbInputFileMappingView;

    if ((ImgbInputFileSize < sizeof(IMAGE_DOS_HEADER)) ||
        (DosHeader->e_magic != IMAGE_DOS_SIGNATURE) ||
        (ImgbInputFileSize < sizeof(IMAGE_NT_HEADERS) + (ULONG)DosHeader->e_lfanew)) {
        ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
    }

    //
    // Verify that the NT header and optional header signatures match.
    //

    ImgbNtHeader = (PIMAGE_NT_HEADERS)((LPBYTE)ImgbInputFileMappingView +
        (ULONG)DosHeader->e_lfanew);

    if ((ImgbNtHeader->Signature != IMAGE_NT_SIGNATURE) ||
        (ImgbNtHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)) {
        ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
    }

    //
    // Verify that the image was linked with /MACHINE:X86.
    //

    if (ImgbNtHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) {
        ImgbResourcePrintErrorAndExit(IDS_NOT_MACHINE_X86);
    }

    //
    // Verify that the image was linked with /SUBSYSTEM:XBOX.
    //

    if (ImgbNtHeader->OptionalHeader.Subsystem != IMAGE_SUBSYSTEM_XBOX) {
        ImgbResourcePrintErrorAndExit(IDS_NOT_SUBSYSTEM_XBOX);
    }

    //
    // Prepare the XBE image header.
    //

    ImgbXbeImageHeader = new IMGB_XBEIMAGE_HEADER;

    InitializeListHead(&ImgbXbeImageHeader->HeadersListHead);

    ImgbXbeImageHeader->ImageHeader.Signature = XBEIMAGE_SIGNATURE;
    ImgbXbeImageHeader->ImageHeader.BaseAddress = (PVOID)XBEIMAGE_STANDARD_BASE_ADDRESS;
    time((time_t *)&ImgbXbeImageHeader->ImageHeader.TimeDateStamp);
    ImgbXbeImageHeader->ImageHeader.SizeOfImageHeader = sizeof(XBEIMAGE_HEADER);
    ImgbXbeImageHeader->ImageHeader.AddressOfEntryPoint =
        (PXBEIMAGE_ENTRY_POINT)ImgbNtHeader->OptionalHeader.AddressOfEntryPoint;
    ImgbXbeImageHeader->ImageHeader.SizeOfStackCommit = (ImgbSizeOfStack != 0) ? ImgbSizeOfStack :
        ImgbNtHeader->OptionalHeader.SizeOfStackCommit;
    ImgbXbeImageHeader->ImageHeader.SizeOfHeapReserve = ImgbNtHeader->OptionalHeader.SizeOfHeapReserve;
    ImgbXbeImageHeader->ImageHeader.SizeOfHeapCommit = ImgbNtHeader->OptionalHeader.SizeOfHeapCommit;
    ImgbXbeImageHeader->ImageHeader.NtSizeOfImage = ImgbNtHeader->OptionalHeader.SizeOfImage;
    ImgbXbeImageHeader->ImageHeader.NtCheckSum = ImgbNtHeader->OptionalHeader.CheckSum;
    ImgbXbeImageHeader->ImageHeader.NtTimeDateStamp = ImgbNtHeader->FileHeader.TimeDateStamp;
    ImgbXbeImageHeader->ImageHeader.InitFlags = ImgbInitFlags;

    //
    // The XBE image certificate is a fixed size header.
    //

    ImgbXbeImageHeader->CertificateHeader.VirtualSize = sizeof(XBEIMAGE_CERTIFICATE);

    InsertTailList(&ImgbXbeImageHeader->HeadersListHead,
        &ImgbXbeImageHeader->CertificateHeader.HeadersListEntry);

    //
    // Process the section headers for the image.
    //

    ImgbProcessInputSectionHeaders();

    //
    // Process the import descriptors for the image.
    //

    ImgbProcessInputImportDescriptors();

    //
    // Process the thread local storage data directory entry for the image.
    //

    ImgbProcessInputTlsDirectory();

    //
    // Process the library versions section for the image.
    //

    ImgbProcessLibraryVersions();

    //
    // Add the debug paths strings to the image header.
    //

    ImgbAddDebugPaths();

    //
    // Always add the Microsoft logo to the image.
    //

    ImgbXbeImageHeader->MicrosoftLogoHeader.VirtualSize = sizeof(ImgbMicrosoftLogo);

    InsertTailList(&ImgbXbeImageHeader->HeadersListHead,
        &ImgbXbeImageHeader->MicrosoftLogoHeader.HeadersListEntry);

    //
    // Process the list of insert files for the image.
    //

    ImgbProcessInsertFiles();

    //
    // Compute the virtual size of the section headers.  No more sections may be
    // added or removed from the image.
    //

    ImgbXbeImageHeader->SectionHeaders.VirtualSize =
        ImgbXbeImageHeader->ImageHeader.NumberOfSections * sizeof(XBEIMAGE_SECTION) +
        ImgbXbeImageHeader->SectionHeaders.NumberOfSharedPageReferenceCounts * sizeof(USHORT) +
        ImgbXbeImageHeader->SectionHeaders.SizeOfSectionNames;

    //
    // Assign virtual addresses to the main image header and all of the
    // secondary headers.
    //

    ImgbLayoutOutputHeaders();

    //
    // Compute the size of the PE headers, and add them if the /PEHEADER switch
    // was explicitly set.
    //

    ImgbAddPEHeader();

    //
    // Relocate the image now that the size of all of the headers are known.
    //

    ImgbRelocateImageAfterHeaders();

    //
    // Confound basic data in the base header
    //
    
    ImgbConfoundHeaderData();

    //
    // Emit the output file.
    //

    ImgbEmitOutputFile();
}

void
__cdecl
main(
    int argc,
    char *argv[]
    )
{
    //
    // Process the command line arguments.
    //

    ImgbProcessCommandLineOptions(argc, argv);

    //
    // Open the input and output files.
    //

    ImgbOpenInputOutputFiles();

    //
    // Build the output file.
    //

    ImgbBuildOutputFile();
}
