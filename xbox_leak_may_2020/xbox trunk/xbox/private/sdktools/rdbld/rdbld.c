/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    RDBLD.c

Abstract:

    This module implements a utility program to extract the code out of romdec32.exe

--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xcrypt.h>
#include <sha.h>
#include <bldr.h>
#include <conio.h>

_CRTIMP void __cdecl exit(int);

void
RomdecCalcHash(
    PUCHAR DataToHash,
    ULONG HashSize,
    PUCHAR Hash
)
{}
//#include "romhash.h"

void GetPasswd(LPCSTR szPrompt, LPSTR sz, int cchMax)
{
    char ch;
    int ich = 0;

    _cputs(szPrompt);
    for(;;) {
        ch = (char)_getch();
        switch(ch) {
        case 8:
            if(ich)
                --ich;
            break;
        case 10:
        case 13:
            sz[ich] = 0;
            _putch('\r');
            _putch('\n');
            return;
        default:
            if(ich < cchMax)
                sz[ich++] = ch;
            break;
        }
    }
}

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
        fprintf(stderr, "RDBLD : Error : unable to open %s\n", lpFileName);
        exit(1);
    }

    *pdwFileSize = GetFileSize(hFile, NULL);
    *ppvFileContents = LocalAlloc(LMEM_FIXED, *pdwFileSize);

    if (*ppvFileContents == NULL) {
        fprintf(stderr, "RDBLD : Error : cannot allocate buffer for %s\n", lpFileName);
        exit(1);
    }

    if (!ReadFile(hFile, *ppvFileContents, *pdwFileSize, &dwBytesRead, NULL) ||
        (dwBytesRead != *pdwFileSize)) {
        fprintf(stderr, "RDBLD : Error : cannot read from %s\n", lpFileName);
        exit(1);
    }

    CloseHandle(hFile);
}

int
__cdecl
main(
    int argc,
    char *argv[]
    )
{
    LPVOID RomDecFileContents;
    DWORD RomDecFileSize;
    PIMAGE_NT_HEADERS NtHeader;
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG RomDecBaseAddress;
    NTSTATUS status;
    HANDLE hOutputFile;
    DWORD dwBytesWritten;
    DWORD SectionSize;
    LPVOID RStartupFileContents;
    DWORD RStartupFileSize;
    PVOID PadBuffer;
    DWORD PadSize;
    HANDLE hPreldrFile = INVALID_HANDLE_VALUE;
    BYTE rgbPreldr[PRELDR_BLOCK_SIZE - 0x180];
    BYTE rgbRomdec[ROM_DEC_SIZE];
    DWORD cb;
    PBYTE pbEncryptedRomdec;
    BOOL fXM3P = FALSE;
    A_SHA_CTX SHAHash;
    FILETIME ft;
    BYTE rgbHash[XC_DIGEST_LEN];
    PBYTE pbPreldrKey;
    char ch;
    char szPasswd1[64];
    char szPasswd2[64];

    //
    // See whether we're doing an XM3P build
    //
    if(argc >= 2 && 0 == _stricmp(argv[1], "-xm3p")) {
        if(argc < 4)
            goto badcmdline;
        argc -= 3;
        argv += 3;
        hPreldrFile = CreateFile(argv[-1], GENERIC_READ, 0, NULL,
            OPEN_EXISTING, 0, NULL);
        if(INVALID_HANDLE_VALUE == hPreldrFile) {
noreadpreldr:
            fprintf(stderr, "RDBLD: cannot read %s\n", argv[-1]);
            exit(1);
        }
        memset(rgbPreldr, 0, sizeof rgbPreldr);
        if(!ReadFile(hPreldrFile, rgbPreldr, sizeof rgbPreldr, &cb, 0) ||
                cb != GetFileSize(hPreldrFile, NULL))
            goto noreadpreldr;
        CloseHandle(hPreldrFile);
        hPreldrFile = CreateFile(argv[0], GENERIC_WRITE, 0, NULL,
            CREATE_ALWAYS, 0, NULL);
        if(INVALID_HANDLE_VALUE == hPreldrFile) {
nowritepreldr:
            fprintf(stderr, "RDBLD: cannot write %s\n", argv[0]);
            exit(1);
        }
        fXM3P = TRUE;
    }

    //
    // Verify that we have the correct number of command line arguments.
    //

    if (argc != 4) {
badcmdline:
        fprintf(stderr, "RDBLD : Error : invalid command line\n");
        exit(1);
    }

    //
    // Read the input images into memory.
    //
    ReadFileIntoMemory(argv[1], &RomDecFileContents, &RomDecFileSize);
    printf("RDBLD - Reading file %s (%d bytes)\n", argv[1], RomDecFileSize);

    ReadFileIntoMemory(argv[2], &RStartupFileContents, &RStartupFileSize);
    printf("RDBLD - Reading file %s (%d bytes)\n", argv[2], RStartupFileSize);


    //
    // Go find the NT header.
    //

    NtHeader = RtlImageNtHeader(RomDecFileContents);

    if (NtHeader == NULL) {
        fprintf(stderr, "RDBLD : Error : corrupt image\n");
        exit(1);
    }

    //
    // Go find the .text section header.  For now, we assume this is the first
    // section in the image.
    //

    SectionHeader = IMAGE_FIRST_SECTION(NtHeader);

    if (strcmp(SectionHeader->Name, ".text") != 0) {
        fprintf(stderr, "RDBLD : Error : .text section isn't first section\n");
        exit(1);
    }

    //
    // Compute the base address for the RomDec.  It will reside at highest 512 bytes of the 
    // address space plus the PE headers are stripped off.
    //
    RomDecBaseAddress = 0xFFFFFFFF - 0x1FF - SectionHeader->VirtualAddress;

    printf("RDBLD - Rebasing to %08X, code will start at %08X\n", RomDecBaseAddress, 0xFFFFFFFF - 0x1FF);

    //
    // Relocate 
    //

    __try {
        status = (NTSTATUS)xxxLdrRelocateImage((PVOID)RomDecFileContents,
            (PVOID)RomDecBaseAddress, "RDBLD", (ULONG)STATUS_SUCCESS,
            (ULONG)STATUS_CONFLICTING_ADDRESSES, (ULONG)STATUS_INVALID_IMAGE_FORMAT);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    if (!NT_SUCCESS(status)) {
        fprintf(stderr, "RDBLD : Error : error %08x while relocating 32-bit image\n",
            status);
        exit(1);
    }

    SectionSize = SectionHeader->Misc.VirtualSize;
    if (SectionSize + RStartupFileSize > ROM_DEC_SIZE) {
        fprintf(stderr, "RDBLD : Error : Total size (%d + %d) exceeds %d bytes\n",
            SectionSize, RStartupFileSize, ROM_DEC_SIZE);
        exit(1);
    }
    printf("RDBLD: 32-bit size is %d bytes\n", SectionSize);
    printf("RDBLD: 16-bit size is %d bytes\n", RStartupFileSize);

    //
    // Initialize our buffer to NOP padding
    //
    memset(rgbRomdec, 0x90, sizeof rgbRomdec);

    //
    // Assemble the romdec contents
    //
    memcpy(rgbRomdec, (PUCHAR)RomDecFileContents +
        SectionHeader->PointerToRawData, SectionSize);
    memcpy(rgbRomdec + sizeof rgbRomdec - RStartupFileSize,
        RStartupFileContents, RStartupFileSize);

    //
    // If we're building XM3P, we need to hash the preldr into the romdec
    // and generate a random key
    //
    if(fXM3P) {

        //
        // Make sure everything will fit
        //
        if(ROMDEC_N < RStartupFileSize + 12 || SectionSize >
            ROM_DEC_SIZE - ROMDEC_N)
        {
            fprintf(stderr, "RDBLD: XM3P nonce won't fit\n");
            exit(1);
        }
        if(ROMDEC_HASH < RStartupFileSize + 16 || SectionSize >
            ROM_DEC_SIZE - ROMDEC_HASH)
        {
            fprintf(stderr, "RDBLD: XM3P hash won't fit\n");
            exit(1);
        }

        //
        // We'll be encrypting the romdec, so get a password first
        //

        GetPasswd("Enter password:", szPasswd1, sizeof szPasswd1);
        if(strlen(szPasswd1) < 8) {
            fprintf(stderr, "Password must be >= 8 chars\n");
            exit(1);
        }
        GetPasswd("Enter it again:", szPasswd2, sizeof szPasswd2);
        if(strcmp(szPasswd1, szPasswd2)) {
            fprintf(stderr, "Passwords do not match\n");
            exit(1);
        }
        memset(szPasswd2, 0, sizeof szPasswd2);

        //
        // The random key comes first
        //

        A_SHAInit(&SHAHash);
        GetSystemTimeAsFileTime(&ft);
        A_SHAUpdate(&SHAHash, (PBYTE)&ft, sizeof ft);
        _cputs("Enter random data:");
        do {
            ch = (char)_getch();
            _asm {
                lea ecx, ft
                rdtsc
                mov [ecx], eax
                mov [ecx+4], edx
            }
            A_SHAUpdate(&SHAHash, (PBYTE)&ft, sizeof ft);
        } while(ch != 10 && ch != 13);
        _putch('\r');
        _putch('\n');
        GetSystemTimeAsFileTime(&ft);
        A_SHAUpdate(&SHAHash, (PBYTE)&ft, sizeof ft);
        A_SHAFinal(&SHAHash, rgbHash);
        XCSymmetricEncDec(rgbRomdec + ROM_DEC_SIZE - ROMDEC_N,
            ROMDEC_N - RStartupFileSize, rgbHash, sizeof rgbHash);

        //
        // Now that we have the random key, we encrypt the preldr's public
        // key with it
        //
        pbPreldrKey = (PBYTE)((PULONG)rgbPreldr)[3];
        ((PULONG)rgbPreldr)[3] = 0;
        pbPreldrKey = pbPreldrKey - (PUCHAR)(0UL - ROM_DEC_SIZE -
            PRELDR_BLOCK_SIZE) + rgbPreldr;
        XCSymmetricEncDec(pbPreldrKey, XC_PUBLIC_KEYDATA_SIZE,
            rgbRomdec + ROM_DEC_SIZE - ROMDEC_N, 12);

        //
        // Now we hash the preldr into the romdec
        //

        RomdecCalcHash(rgbPreldr, sizeof rgbPreldr, rgbRomdec + ROM_DEC_SIZE -
            ROMDEC_HASH);

        //
        // Finally, we write the updated preloader
        //

        printf("RDBLD: Wrote new preldr file %s\n", argv[0]);
        if(!WriteFile(hPreldrFile, rgbPreldr, sizeof rgbPreldr,
                &dwBytesWritten, NULL) || dwBytesWritten != sizeof rgbPreldr)
            goto nowritepreldr;
        CloseHandle(hPreldrFile);

        //
        // Now we have to encrypt the romdec.  Format is 8-byte confounder +
        // encrypt(20 bytes digest + data).  Confounder is RC4 stream from the
        // romdec hash (which is random by virtue of the key we generated
        // above).  RC4 key for the encryption stream is the nonce encrypted
        // with the password hash
        //

        pbEncryptedRomdec = malloc(cb = sizeof rgbRomdec + 28);
        memcpy(pbEncryptedRomdec + 28, rgbRomdec, sizeof rgbRomdec);
        A_SHAInit(&SHAHash);
        A_SHAUpdate(&SHAHash, rgbRomdec, sizeof rgbRomdec);
        A_SHAFinal(&SHAHash, pbEncryptedRomdec + 8);
        XCSymmetricEncDec(pbEncryptedRomdec, 8, pbEncryptedRomdec + 8, 8);

        A_SHAInit(&SHAHash);
        A_SHAUpdate(&SHAHash, szPasswd1, strlen(szPasswd1));
        A_SHAFinal(&SHAHash, rgbHash);
        XCSymmetricEncDec(pbEncryptedRomdec, 8, rgbHash, sizeof rgbHash);
        XCSymmetricEncDec(pbEncryptedRomdec + 8, sizeof rgbRomdec + 20,
            pbEncryptedRomdec, 8);
        XCSymmetricEncDec(pbEncryptedRomdec, 8, rgbHash, sizeof rgbHash);
        memset(szPasswd1, 0, sizeof szPasswd1);
    } else {
        pbEncryptedRomdec = rgbRomdec;
        cb = sizeof rgbRomdec;
    }

    //
    // Create the output file.
    //
    printf("RDBLD - Writing output file %s\n", argv[3]);

    hOutputFile = CreateFile(argv[3], GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hOutputFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "RDBLD : Error : cannot write to %s\n", argv[3]);
        exit(1);
    }

    //
    // write out text section
    //
    printf("RDBLD - Writing data (%d bytes)\n", ROM_DEC_SIZE);

    if (!WriteFile(hOutputFile, pbEncryptedRomdec, cb,
        &dwBytesWritten, NULL) || (cb != dwBytesWritten)) {
        fprintf(stderr, "RDBLD : Error : cannot write to %s\n", argv[3]);

        CloseHandle(hOutputFile);
        exit(1);
    }


    CloseHandle(hOutputFile);

    return 0;
}
