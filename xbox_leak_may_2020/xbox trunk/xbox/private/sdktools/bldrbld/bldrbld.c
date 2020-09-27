/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    bldrbld.c

Abstract:

    This module implements a utility program to combine the 16-bit boot loader
    and the 32-bit boot loader into a single loader image.

--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <bldr.h>

ULONG
xxxLdrRelocateImage (
    IN PVOID ImageToRelocate,
    IN PVOID LoadAddress,
    IN PUCHAR LoaderName,
    IN ULONG Success,
    IN ULONG Conflict,
    IN ULONG Invalid
    );

VOID
ReadFileIntoMemory(
    LPCSTR lpFileName,
    LPVOID *ppvFileContents,
    DWORD *pdwFileSize
    )
{
    HANDLE hFile;
    DWORD dwBytesRead;

    hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "BLDRBLD: error: unable to open %s\n", lpFileName);
        exit(1);
    }

    *pdwFileSize = GetFileSize(hFile, NULL);
    *ppvFileContents = LocalAlloc(LMEM_FIXED, *pdwFileSize);

    if (*ppvFileContents == NULL) {
        fprintf(stderr, "BLDRBLD: error: cannot allocate buffer for %s\n", lpFileName);
        exit(1);
    }

    if (!ReadFile(hFile, *ppvFileContents, *pdwFileSize, &dwBytesRead, NULL) ||
        (dwBytesRead != *pdwFileSize)) {
        fprintf(stderr, "BLDRBLD: error: cannot read from %s\n", lpFileName);
        exit(1);
    }

    CloseHandle(hFile);
}


typedef struct _BOOTLDRPARAM {
    DWORD Bldr32EntryPoint;
    CHAR  CommandLine[64];
} BOOTLDRPARAM, *PBOOTLDRPARAM;


int
__cdecl
main(
    int argc,
    char *argv[]
    )
{
    LPVOID Bldr32FileContents;
    DWORD Bldr32FileSize;
    PIMAGE_NT_HEADERS NtHeader;
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG Bldr32BootBaseAddress;
    ULONG Bldr32BaseAddress;
    NTSTATUS status;
    ULONG Bldr32EntryPoint;
    HANDLE hOutputFile;
    DWORD dwBytesWritten;
    BOOL fPreloader = FALSE;

    BOOTLDRPARAM BootLdrParam;
    DWORD PreloaderPreface[4];
    DWORD cbParams;

    //
    // Look for -p to see if we're doing the preloader
    //
    if(argc >= 2 && 0 == _stricmp(argv[1], "-p")) {
        --argc;
        ++argv;
        fPreloader = TRUE;
    }

    //
    // Verify that we have the correct number of command line arguments.
    //
    if (argc != 3) {
        fprintf(stderr, "BLDRBLD: error: invalid command line\n");
        exit(1);
    }

    //
    // Read the input boot loader images into memory.
    //

    ReadFileIntoMemory(argv[1], &Bldr32FileContents, &Bldr32FileSize);

    //
    // Go find the NT header.
    //

    NtHeader = RtlImageNtHeader(Bldr32FileContents);

    if (NtHeader == NULL) {
        fprintf(stderr, "BLDRBLD: error: corrupt 32-bit boot loader\n");
        exit(1);
    }

    //
    // Go find the .text section header.  For now, we assume this is the first
    // section in the image.
    //

    SectionHeader = IMAGE_FIRST_SECTION(NtHeader);

    if (strcmp(SectionHeader->Name, ".text") != 0) {
        fprintf(stderr, "BLDRBLD: error: .text section isn't first section\n");
        exit(1);
    }

    //
    // Compute the base address for the 32-bit boot loader:
    //    Bldr32EntryPoint (4 bytes)
    //    ROMImagePhysicalAddress (4 bytes)
    //    CommandLine (80 bytes)
    //    .text section

    if(fPreloader) {
        Bldr32BootBaseAddress = 0 - ROM_DEC_SIZE - PRELDR_BLOCK_SIZE + 16;
        Bldr32BaseAddress = Bldr32BootBaseAddress;
    } else {
        Bldr32BootBaseAddress = BLDR_BOOT_ORIGIN + sizeof(BootLdrParam);
        Bldr32BaseAddress = BLDR_RELOCATED_ORIGIN + sizeof(BootLdrParam);
    }
    Bldr32BootBaseAddress -= SectionHeader->VirtualAddress;
    Bldr32BaseAddress -= SectionHeader->VirtualAddress;

    //
    // Relocate the 32-bit boot loader.
    //

    printf("BLDRBLD: Relocating to %08x\n", Bldr32BaseAddress);

    __try {
        status = (NTSTATUS)xxxLdrRelocateImage((PVOID)Bldr32FileContents,
            (PVOID)Bldr32BaseAddress, "BLDRBLD", (ULONG)STATUS_SUCCESS,
            (ULONG)STATUS_CONFLICTING_ADDRESSES, (ULONG)STATUS_INVALID_IMAGE_FORMAT);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    if (!NT_SUCCESS(status)) {
        fprintf(stderr, "BLDRBLD: error: error %08x while relocating 32-bit image\n",
            status);
        exit(1);
    }

    //
    // Fixup the 16-bit boot loader with the entry point of the 32-bit boot
    // loader.  This needs to be relative to BLDR_BOOT_ORIGIN since at the point
    // this is invoked, the image hasn't been relocated to BLDR_RELOCATED_ORIGIN.
    //

    Bldr32EntryPoint = Bldr32BootBaseAddress +
        NtHeader->OptionalHeader.AddressOfEntryPoint;

    printf("BLDRBLD: Entry point=%08x\n", Bldr32EntryPoint);

    //
    // Create the output file.
    //

    hOutputFile = CreateFile(argv[2], GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hOutputFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "BLDRBLD: error: cannot write to %s\n", argv[2]);
        exit(1);
    }

    //
    // Write out boot loader parameters
    //
    if(fPreloader) {
        memset(PreloaderPreface, 0, sizeof PreloaderPreface);
        // Start with a 32-bit-rel JMP instruction
        PreloaderPreface[0] = ((Bldr32EntryPoint -
            (0xFFFFFE00 - PRELDR_BLOCK_SIZE) - 5) << 8) | 0xE9;
        // Fetch the SHA function block pointer from the DWORD before the
        // entry point and insert it at an 8-byte offset
        PreloaderPreface[2] = ((PULONG)((PUCHAR)Bldr32FileContents +
            Bldr32EntryPoint - Bldr32BaseAddress))[-1];
        // Fetch the public key pointer from the 2 DWORDs before the
        // entry point and insert it at a 12-byte offset
        PreloaderPreface[3] = ((PULONG)((PUCHAR)Bldr32FileContents +
            Bldr32EntryPoint - Bldr32BaseAddress))[-2];
        if (!WriteFile(hOutputFile, PreloaderPreface, sizeof(PreloaderPreface),
            &dwBytesWritten, NULL) || (sizeof(PreloaderPreface) !=
            dwBytesWritten))
        {
            fprintf(stderr, "BLDRBLD: error: cannot write to %s\n", argv[2]);
            exit(1);
        }
    } else {
        memset(&BootLdrParam, 0, sizeof(BootLdrParam));
        BootLdrParam.Bldr32EntryPoint = Bldr32EntryPoint;

        if (!WriteFile(hOutputFile, &BootLdrParam, sizeof(BootLdrParam),
            &dwBytesWritten, NULL) || (sizeof(BootLdrParam) != dwBytesWritten))
        {
            fprintf(stderr, "BLDRBLD: error: cannot write to %s\n", argv[2]);
            exit(1);
        }
    }

    //
    // Write out the 32-bit boot loader's .text section.
    //

    if (!WriteFile(hOutputFile, (PUCHAR)Bldr32FileContents +
        SectionHeader->PointerToRawData, SectionHeader->SizeOfRawData,
        &dwBytesWritten, NULL) || (SectionHeader->SizeOfRawData != dwBytesWritten)) {
        fprintf(stderr, "BLDRBLD: error: cannot write to %s\n", argv[2]);
        exit(1);
    }

    CloseHandle(hOutputFile);

    return 0;
}
