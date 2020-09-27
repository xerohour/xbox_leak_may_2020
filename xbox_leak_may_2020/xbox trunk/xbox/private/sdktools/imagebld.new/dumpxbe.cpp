/*++

Copyright (c) 2001-2002  Microsoft Corporation

Module Name:

    dumpxbe.cpp

Abstract:

    This module implements a utility program to dump an Xbox executable image.

--*/

#include "imgbldp.h"

//
// Padding characters used when dumping indented lines.
//
const char ImgbDumpPadding[] = "   ";

const char *ImgbApprovedStatus[] = {
    "unapproved",
    "possibly approved",
    "approved",
    "expired"
};

LPSTR
ImgbGetDumpTimeStampString(
    PULONG TimeDateStamp
    )
{
    LPSTR pszTimeStampString;

    pszTimeStampString = ctime((time_t*)TimeDateStamp);

    if (pszTimeStampString == NULL) {
        pszTimeStampString = "invalid\n";
    }

    return pszTimeStampString;
}

LPSTR
ImgbGetDumpCertificateKeyString(
    PUCHAR CertificateKey
    )
{
    static CHAR Buffer[80];

    sprintf(Buffer, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        CertificateKey[0], CertificateKey[1], CertificateKey[2], CertificateKey[3],
        CertificateKey[4], CertificateKey[5], CertificateKey[6], CertificateKey[7],
        CertificateKey[8], CertificateKey[9], CertificateKey[10], CertificateKey[11],
        CertificateKey[12], CertificateKey[13], CertificateKey[14], CertificateKey[15]);

    return Buffer;
}

LPVOID
ImgbGetDumpDataFromVirtualAddress(
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

        if (FileByteOffset >= ImgbInputFileSize) {
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
        ImgbInputFileSize) {
        return NULL;
    }

    //
    // Don't return anything if the section headers end past the end of the
    // file.
    //

    if (((ULONG)ImageHeader->SectionHeaders - (ULONG)ImageHeader->BaseAddress +
        (ImageHeader->NumberOfSections * sizeof(XBEIMAGE_SECTION))) >= ImgbInputFileSize) {
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

            if (FileByteOffset >= ImgbInputFileSize) {
                return NULL;
            }

            return (LPBYTE)ImageHeader + FileByteOffset;
        }

        ImageSection++;
    }

    return NULL;
}

VOID
ImgbDumpXbeCertificate(
    PXBEIMAGE_HEADER ImageHeader
    )
{
    PXBEIMAGE_CERTIFICATE Certificate;
    ULONG Index;

    //
    // Verify that the image has a certificate.
    //

    if (ImageHeader->Certificate == NULL) {
        return;
    }

    //
    // Map the virtual address of the certificate to its memory mapped virtual
    // address.
    //

    Certificate = (PXBEIMAGE_CERTIFICATE)ImgbGetDumpDataFromVirtualAddress(
        ImageHeader, ImageHeader->Certificate, sizeof(XBEIMAGE_CERTIFICATE));

    if (Certificate == NULL) {
        return;
    }

    printf("CERTIFICATE\n");
    printf("%s%8X size of certificate\n", ImgbDumpPadding, Certificate->SizeOfCertificate);
    printf("%s%08X time date stamp %s", ImgbDumpPadding, Certificate->TimeDateStamp, ImgbGetDumpTimeStampString(&Certificate->TimeDateStamp));
    printf("%s%08X title id\n", ImgbDumpPadding, Certificate->TitleID);
    printf("%s%8X allowed media types\n", ImgbDumpPadding, Certificate->AllowedMediaTypes);
    printf("%s%8X game region\n", ImgbDumpPadding, Certificate->GameRegion);
    printf("%s%8X game ratings\n", ImgbDumpPadding, Certificate->GameRatings);
    printf("%s%8X disk number\n", ImgbDumpPadding, Certificate->DiskNumber);
    printf("%s%8X version\n", ImgbDumpPadding, Certificate->Version);

    if (Certificate->SizeOfCertificate > FIELD_OFFSET(XBEIMAGE_CERTIFICATE,
        OnlineServiceName)) {
        printf("%s%8X online service name\n", ImgbDumpPadding,
            Certificate->OnlineServiceName);
    }

    printf("\n");

    printf("   Title name: %ws\n\n", Certificate->TitleName);

    printf("      LAN key: %s\n", ImgbGetDumpCertificateKeyString(Certificate->LANKey));
    printf("Signature key: %s\n", ImgbGetDumpCertificateKeyString(Certificate->SignatureKey));
    printf("\n");

    if (Certificate->AlternateTitleIDs[0] != 0) {

        printf("Title alternate title ids:\n");

        for (Index = 0; Index < XBEIMAGE_ALTERNATE_TITLE_ID_COUNT; Index++) {

            if (Certificate->AlternateTitleIDs[Index] == 0) {
                break;
            }

            printf("%s%08x    Signature key: %s\n", ImgbDumpPadding, Certificate->AlternateTitleIDs[Index],
                ImgbGetDumpCertificateKeyString(Certificate->AlternateSignatureKeys[Index]));
        }

        printf("\n");
    }
}

VOID
ImgbAlterUnapprovedLibraryStatus(
    PXBEIMAGE_LIBRARY_VERSION LibraryVersion,
    int ApprovalLevel
    )
{
    LibraryVersion->ApprovedLibrary = (USHORT)ApprovalLevel;
}

VOID
ImgbDumpXbeLibraryVersions(
    PXBEIMAGE_HEADER ImageHeader
    )
{
    PXBEIMAGE_LIBRARY_VERSION LibraryVersion;
    PXBEIMAGE_LIBRARY_VERSION XapiLibraryVersion;
    ULONG Index;
    ULONG ApprovalStatus;

    //
    // Verify that the image has a library versions section.
    //

    if (ImageHeader->NumberOfLibraryVersions == 0) {
        return;
    }

    //
    // Map the virtual address of the library versions to its memory mapped
    // virtual address.
    //

    LibraryVersion = (PXBEIMAGE_LIBRARY_VERSION)ImgbGetDumpDataFromVirtualAddress(
        ImageHeader, ImageHeader->LibraryVersions,
        ImageHeader->NumberOfLibraryVersions * sizeof(XBEIMAGE_LIBRARY_VERSION));

    if (LibraryVersion == NULL) {
        return;
    }

    if (ImageHeader->XapiLibraryVersion != NULL) {
        XapiLibraryVersion = (PXBEIMAGE_LIBRARY_VERSION)ImgbGetDumpDataFromVirtualAddress(
            ImageHeader, ImageHeader->XapiLibraryVersion,
            sizeof(XBEIMAGE_LIBRARY_VERSION));
    } else {
        XapiLibraryVersion = NULL;
    }

    //
    // Use the check approval process to modify approved bits.
    //

    CheckLibraryApprovalStatus(XapiLibraryVersion, LibraryVersion,
        ImageHeader->NumberOfLibraryVersions, ImgbAlterUnapprovedLibraryStatus);

    printf("LIBRARY VERSIONS\n");

    for (Index = 0; Index < ImageHeader->NumberOfLibraryVersions; Index++) {
        ApprovalStatus = LibraryVersion->ApprovedLibrary;

        printf("%s%8.8s %d.%d.%d.%d%s [%s]\n", ImgbDumpPadding, LibraryVersion->LibraryName,
            LibraryVersion->MajorVersion, LibraryVersion->MinorVersion,
            LibraryVersion->BuildVersion, LibraryVersion->QFEVersion,
            (LibraryVersion->DebugBuild ? " [debug]" : ""),
            ImgbApprovedStatus[ApprovalStatus]);
        LibraryVersion++;
    }

    printf("\n");
}

VOID
ImgbDumpXbeImportDirectories(
    PXBEIMAGE_HEADER ImageHeader
    )
{
    PXBEIMAGE_IMPORT_DESCRIPTOR RawImportDescriptor;
    PXBEIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
    PWCHAR ImageName;

    //
    // Don't dump anything if the image doesn't import from an image other than
    // the kernel.
    //

    if (ImageHeader->ImportDirectory == NULL) {
        return;
    }

    //
    // Loop over all of the import modules and dump out the name.
    //

    printf("NON-KERNEL IMPORT MODULES\n");

    RawImportDescriptor = ImageHeader->ImportDirectory;

    for (;;) {

        ImportDescriptor = (PXBEIMAGE_IMPORT_DESCRIPTOR)ImgbGetDumpDataFromVirtualAddress(
            ImageHeader, RawImportDescriptor, sizeof(XBEIMAGE_IMPORT_DESCRIPTOR));

        if (ImportDescriptor == NULL) {
            break;
        }

        if (ImportDescriptor->ImageThunkData == NULL) {
            break;
        }

        ImageName = (PWCHAR)ImgbGetDumpDataFromVirtualAddress(ImageHeader,
            ImportDescriptor->ImageName, sizeof(WCHAR));

        if (ImageName == NULL) {
            break;
        }

        printf("%s%ws\n", ImgbDumpPadding, ImageName);

        RawImportDescriptor++;
    }

    printf("\n");
}

VOID
ImgbDumpXbeTlsDirectory(
    PXBEIMAGE_HEADER ImageHeader
    )
{
    PIMAGE_TLS_DIRECTORY TlsDirectory;

    //
    // Verify that the image has a TLS directory.
    //

    if (ImageHeader->TlsDirectory == NULL) {
        return;
    }

    //
    // Map the virtual address of the TLS directory to its memory mapped virtual
    // address.
    //

    TlsDirectory = (PIMAGE_TLS_DIRECTORY)ImgbGetDumpDataFromVirtualAddress(
        ImageHeader, ImageHeader->TlsDirectory, sizeof(IMAGE_TLS_DIRECTORY));

    if (TlsDirectory == NULL) {
        return;
    }

    printf("THREAD LOCAL STORAGE DIRECTORY\n");
    printf("%s%08X raw data start address\n", ImgbDumpPadding, TlsDirectory->StartAddressOfRawData);
    printf("%s%08X raw data end address\n", ImgbDumpPadding, TlsDirectory->EndAddressOfRawData);
    printf("%s%08X TLS index address\n", ImgbDumpPadding, TlsDirectory->AddressOfIndex);
    printf("%s%08X TLS callbacks address\n", ImgbDumpPadding, TlsDirectory->AddressOfCallBacks);
    printf("%s%8X size of zero fill\n", ImgbDumpPadding, TlsDirectory->SizeOfZeroFill);
    printf("%s%8X characteristics\n", ImgbDumpPadding, TlsDirectory->Characteristics);
    printf("\n");
}

VOID
ImgbDumpXbeSectionHeaders(
    PXBEIMAGE_HEADER ImageHeader
    )
{
    PXBEIMAGE_SECTION ImageSection;
    ULONG Index;
    LPCSTR SectionName;

    //
    // Verify that the image has sections.
    //

    if (ImageHeader->NumberOfSections == 0) {
        return;
    }

    //
    // Map the virtual address of the section headers to its memory mapped
    // virtual address.
    //

    ImageSection = (PXBEIMAGE_SECTION)ImgbGetDumpDataFromVirtualAddress(
        ImageHeader, ImageHeader->SectionHeaders,
        ImageHeader->NumberOfSections * sizeof(XBEIMAGE_SECTION));

    if (ImageSection == NULL) {
        return;
    }

    for (Index = 0; Index < ImageHeader->NumberOfSections; Index++) {

        SectionName = (LPCSTR)((LPBYTE)ImageHeader +
            (ULONG)ImageSection->SectionName - (ULONG)ImageHeader->BaseAddress);

        printf("SECTION HEADER #%d  %s\n", Index + 1, SectionName);
        printf("%s%8X virtual address\n", ImgbDumpPadding, ImageSection->VirtualAddress);
        printf("%s%8X virtual size\n", ImgbDumpPadding, ImageSection->VirtualSize);
        printf("%s%8X file pointer to raw data\n", ImgbDumpPadding, ImageSection->PointerToRawData);
        printf("%s%8X size of raw data\n", ImgbDumpPadding, ImageSection->SizeOfRawData);
        printf("%s%08X head shared page reference count address\n", ImgbDumpPadding, ImageSection->HeadSharedPageReferenceCount);
        printf("%s%08X tail shared page reference count address\n", ImgbDumpPadding, ImageSection->TailSharedPageReferenceCount);
        printf("%s%8X flags\n", ImgbDumpPadding, ImageSection->SectionFlags);

        if (ImageSection->SectionFlags & XBEIMAGE_SECTION_WRITEABLE) {
            printf("%s         Writeable\n", ImgbDumpPadding);
        }
        if (ImageSection->SectionFlags & XBEIMAGE_SECTION_PRELOAD) {
            printf("%s         Preload\n", ImgbDumpPadding);
        }
        if (ImageSection->SectionFlags & XBEIMAGE_SECTION_EXECUTABLE) {
            printf("%s         Executable\n", ImgbDumpPadding);
        }
        if (ImageSection->SectionFlags & XBEIMAGE_SECTION_INSERTFILE) {
            printf("%s         Inserted file\n", ImgbDumpPadding);
        }
        if (ImageSection->SectionFlags & XBEIMAGE_SECTION_HEAD_PAGE_READONLY) {
            printf("%s         Head page read-only\n", ImgbDumpPadding);
        }
        if (ImageSection->SectionFlags & XBEIMAGE_SECTION_TAIL_PAGE_READONLY) {
            printf("%s         Tail page read-only\n", ImgbDumpPadding);
        }

        printf("\n");

        ImageSection++;
    }
}

VOID
ImgbDumpXbeImageHeader(
    VOID
    )
{
    PXBEIMAGE_HEADER ImageHeader;

    //
    // Validate that the file is large enough to contain the base portion of the
    // image header.
    //

    if (ImgbInputFileSize < sizeof(XBEIMAGE_HEADER)) {
        ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
    }

    //
    // Validate the signature.
    //

    ImageHeader = (PXBEIMAGE_HEADER)ImgbInputFileMappingView;

    if (ImageHeader->Signature != XBEIMAGE_SIGNATURE) {
        ImgbResourcePrintErrorAndExit(IDS_INVALID_CORRUPT_INPUT_FILE, ImgbInputFilePath);
    }

    //
    // Dump the image header fields.
    //

    printf("IMAGE HEADER VALUES\n");
    printf("%s%08X base address\n", ImgbDumpPadding, ImageHeader->BaseAddress);
    printf("%s%8X size of headers\n", ImgbDumpPadding, ImageHeader->SizeOfHeaders);
    printf("%s%8X size of image\n", ImgbDumpPadding, ImageHeader->SizeOfImage);
    printf("%s%8X size of image header\n", ImgbDumpPadding, ImageHeader->SizeOfImageHeader);
    printf("%s%08X time date stamp %s", ImgbDumpPadding, ImageHeader->TimeDateStamp, ImgbGetDumpTimeStampString(&ImageHeader->TimeDateStamp));
    printf("%s%08X certificate address\n", ImgbDumpPadding, ImageHeader->Certificate);
    printf("%s%8X number of sections\n", ImgbDumpPadding, ImageHeader->NumberOfSections);
    printf("%s%08X section headers address\n", ImgbDumpPadding, ImageHeader->SectionHeaders);
    printf("%s%8X initialization flags\n", ImgbDumpPadding, ImageHeader->InitFlags);
    if (ImageHeader->InitFlags & XINIT_MOUNT_UTILITY_DRIVE) {
        printf("%s         Mount utility drive\n", ImgbDumpPadding);
    }
    if (ImageHeader->InitFlags & XINIT_FORMAT_UTILITY_DRIVE) {
        printf("%s         Format utility drive\n", ImgbDumpPadding);
    }
    if (ImageHeader->InitFlags & XINIT_LIMIT_DEVKIT_MEMORY) {
        printf("%s         Limit development kit memory\n", ImgbDumpPadding);
    }
    if (ImageHeader->InitFlags & XINIT_NO_SETUP_HARD_DISK) {
        printf("%s         Don't setup hard disk\n", ImgbDumpPadding);
    }
    if (ImageHeader->InitFlags & XINIT_DONT_MODIFY_HARD_DISK) {
        printf("%s         Don't modify hard disk\n", ImgbDumpPadding);
    }
    printf("%s         %dK utility drive cluster size\n", ImgbDumpPadding,
        (16 << ((ImageHeader->InitFlags & XINIT_UTILITY_DRIVE_CLUSTER_SIZE_MASK) >>
        XINIT_UTILITY_DRIVE_CLUSTER_SIZE_SHIFT)));
    printf("%s%08X entry point address\n", ImgbDumpPadding, ImageHeader->AddressOfEntryPoint);
    printf("%s%08X thread local storage directory address\n", ImgbDumpPadding, ImageHeader->TlsDirectory);
    printf("%s%8X size of stack commit\n", ImgbDumpPadding, ImageHeader->SizeOfStackCommit);
    printf("%s%8X size of heap commit\n", ImgbDumpPadding, ImageHeader->SizeOfHeapCommit);
    printf("%s%8X size of heap reserve\n", ImgbDumpPadding, ImageHeader->SizeOfHeapReserve);
    printf("%s%08X original base address\n", ImgbDumpPadding, ImageHeader->NtBaseOfDll);
    printf("%s%8X original size of image\n", ImgbDumpPadding, ImageHeader->NtSizeOfImage);
    printf("%s%08X original checksum\n", ImgbDumpPadding, ImageHeader->NtCheckSum);
    printf("%s%08X original time date stamp %s", ImgbDumpPadding, ImageHeader->NtTimeDateStamp, ImgbGetDumpTimeStampString(&ImageHeader->NtTimeDateStamp));
    printf("%s%08X debug path name address\n", ImgbDumpPadding, ImageHeader->DebugPathName);
    printf("%s%08X debug file name address\n", ImgbDumpPadding, ImageHeader->DebugFileName);
    printf("%s%08X debug Unicode file name address\n", ImgbDumpPadding, ImageHeader->DebugUnicodeFileName);
    printf("%s%08X kernel image thunk address\n", ImgbDumpPadding, ImageHeader->XboxKernelThunkData);
    printf("%s%08X non-kernel import directory address\n", ImgbDumpPadding, ImageHeader->ImportDirectory);
    printf("%s%8X number of library versions\n", ImgbDumpPadding, ImageHeader->NumberOfLibraryVersions);
    printf("%s%08X library versions address\n", ImgbDumpPadding, ImageHeader->LibraryVersions);
    printf("%s%08X kernel library version address\n", ImgbDumpPadding, ImageHeader->XboxKernelLibraryVersion);
    printf("%s%08X XAPI library version address\n", ImgbDumpPadding, ImageHeader->XapiLibraryVersion);
    printf("%s%08X logo bitmap address\n", ImgbDumpPadding, ImageHeader->MicrosoftLogo);
    printf("%s%8X logo bitmap size\n", ImgbDumpPadding, ImageHeader->SizeOfMicrosoftLogo);
    printf("\n");

    if (ImageHeader->DebugPathName != NULL) {
        printf("Debug path name: %s\n\n", (ULONG)ImageHeader +
            (ULONG)ImageHeader->DebugPathName - (ULONG)ImageHeader->BaseAddress);
    }

    //
    // Dump the image certificate.
    //

    ImgbDumpXbeCertificate(ImageHeader);

    //
    // Dump the library versions.
    //

    ImgbDumpXbeLibraryVersions(ImageHeader);

    //
    // Dump the import directories.
    //

    ImgbDumpXbeImportDirectories(ImageHeader);

    //
    // Dump the image TLS directory.
    //

    ImgbDumpXbeTlsDirectory(ImageHeader);

    //
    // Dump the image section header fields.
    //

    ImgbDumpXbeSectionHeaders(ImageHeader);
}

DECLSPEC_NORETURN
VOID
ImgbDumpExecutable(
    int argc,
    char *argv[]
    )
{
    //
    // Print out the logo banner.
    //

    ImgbResourcePrintLogoBanner();

    //
    // Skip past the /DUMP parameter.
    //

    argv++;
    argc--;

    //
    // If no arguments were supplied, then print out the usage text.
    //

    if (argc == 0) {
PrintUsageTextAndExit:
        ImgbResourcePrintRange(stderr, IDS_DUMPXBE_USAGE);
        ImgbExitProcess(0);
    }

    //
    // Process the command line options.
    //

    do {

        if (argv[0][0] == '-' || argv[0][0] == '/') {

            //
            // Attempt to match the help switches.
            //

            if ((_stricmp(&argv[0][1], "?") == 0) ||
                (_stricmp(&argv[0][1], "HELP") == 0)) {
                goto PrintUsageTextAndExit;
            }

            //
            // This is an unrecognized option, so error out.
            //

            ImgbResourcePrintErrorAndExit(IDS_UNRECOGNIZED_OPTION, argv[0]);

        } else {

            //
            // Ignore multiple input file names.
            //

            if (ImgbInputFilePath == NULL) {
                ImgbInputFilePath = argv[0];
            }
        }

        argv++;
        argc--;

    } while (argc > 0);

    //
    // If no file name was specified, then print out the usage text.
    //

    if (ImgbInputFilePath == NULL) {
        goto PrintUsageTextAndExit;
    }

    printf("Dump of file %s\n\n", ImgbInputFilePath);

    //
    // Open the input file.
    //

    ImgbInputFileHandle = CreateFile(ImgbInputFilePath, GENERIC_READ,
        FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (ImgbInputFileHandle == INVALID_HANDLE_VALUE) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_OPEN_INPUT_FILE, ImgbInputFilePath);
    }

    //
    // Obtain the size of the input file.
    //

    ImgbInputFileSize = GetFileSize(ImgbInputFileHandle, NULL);

    //
    // Create a mapping of the input file.  We make it copy-on-write because
    // we may need to modify library approval bits as part of our check.
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

    //
    // Start dumping the file at the image header.
    //

    ImgbDumpXbeImageHeader();

    //
    // Exit the application.
    //

    ImgbExitProcess(0);
}
