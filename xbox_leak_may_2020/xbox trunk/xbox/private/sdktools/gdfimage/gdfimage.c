/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    gdfimage.c

Abstract:

    This module implements a utility program to create a GDF disc image.

--*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gdformat.h>
#include <xboxverp.h>
#include "cd.h"

typedef struct _GDF_FILE_NODE {
    struct _GDF_FILE_NODE *Link;
    ULONG DirectoryByteOffset;
    ULONG FirstSector;
    ULONG FileSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[MAX_PATH];
} GDF_FILE_NODE, *PGDF_FILE_NODE;

typedef struct _FST_FILE_ITEM {
    struct _FST_FILE_ITEM *Link;
    ULONG FirstSector;
    ULONG LastSector;
    ULONG DirectoryIndex;
    ULONG FileIndex;
    ULONG FileOffset;
} FST_FILE_ITEM, *PFST_FILE_ITEM;

typedef struct _FST_STRING_ITEM {
    struct _FST_STRING_ITEM *Link;
    LPSTR String;
    int cch;
} FST_STRING_ITEM, *PFST_STRING_ITEM;

//
// Define the upper case table used for file name comparisons.  This matches the
// case table used by the file system.
//
const UCHAR GdfUpperCaseTable[256] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x7e,0x7f,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
    0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xf7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0x3f
};

//
// Define the first sector that we'll start allocating data sectors from.  This
// is chosen so that we're past the starting sector number for a UDF volume
// descriptor sequence.
//
#define GDF_STARTING_DATA_SECTOR            (256 + 8)

#define GDF_ELTORITO_BOOT_CATALOG_SECTOR    24
#define GDF_ELTORITO_BOOT_SECTOR            25

//
// Stores the path to the source directory as supplied by the user.
//
LPWSTR GdfSourceDirectoryPath;

//
// Stores the path to the target file as supplied by the user.
//
LPWSTR GdfTargetFilePath;

//
// Stores the file handle to access the target file.
//
HANDLE GdfTargetFileHandle = INVALID_HANDLE_VALUE;

//
// Indicates whether we're making an emulation image
//
BOOLEAN GdfCreateFST;

//
// Stores the file handle to the FST file
//
HANDLE GdfFSTFileHandle = INVALID_HANDLE_VALUE;

//
// Stores the last sector that has been allocated.
//
ULONG GdfEndOfMedia = GDF_STARTING_DATA_SECTOR;

//
// Stores the current block number offset in the output file
//
ULONG GdfCurrentMediaPosition;

//
// Giant transfer buffer for read/write operations.
//
BYTE GdfTransferBuffer[128 * 1024];

//
// Buffer to hold the boot sector to create a PC bootable media.
//
BYTE GdfBootSector[SECTOR_SIZE];

//
// Flag to indicate whether or not we're creating a PC bootable media.
//
BOOLEAN GdfHaveBootSector;

//
// Stores the FST table information for generating a file for DVD emulation
//
PFST_FILE_ITEM GdfFSTFileList;
PFST_FILE_ITEM *GdfFSTFileListEnd;
PFST_STRING_ITEM GdfFSTStringList;
PFST_STRING_ITEM *GdfFSTStringListEnd;
ULONG GdfStringTableSize;
ULONG GdfDirectoryDirectoryIndex;
ULONG GdfDirectoryFileIndex;

//
// Caches the first sector and file size of XBOXROM.BIN from the root directory
// of the target media.  Used when creating a PC bootable media.
//
ULONG GdfXBOXROMFirstSector;
ULONG GdfXBOXROMFileSize;

//
// Flag to indicate whether or not we've seen a signal that we should abnormally
// terminate.
//
BOOLEAN GdfAbnormalTermination;

ULONG
GdfByteSwap32(
    DWORD dwValue
    )
{
    __asm {
        mov     eax, dwValue
        bswap   eax
    }
}

USHORT
GdfByteSwap16(
    WORD wValue
    )
{
    __asm {
        mov     ax, wValue
        xchg    al, ah
    }
}

DECLSPEC_NORETURN
VOID
GdfExitProcess(
    int ExitCode
    )
{
    if (GdfTargetFileHandle != INVALID_HANDLE_VALUE) {

        CloseHandle(GdfTargetFileHandle);

        //
        // If an error has occurred, then delete the output file.
        //

        if (ExitCode != 0) {
            DeleteFileW(GdfTargetFilePath);
        }
    }

    ExitProcess(ExitCode);
}

VOID
GdfAppendUnicodeToAnsi(
    LPSTR psz,
    LPCWSTR pwsz
    )
{
    int cch = lstrlenA(psz);

    if(!WideCharToMultiByte(CP_ACP, 0, pwsz, -1, psz + cch, MAX_PATH - cch, NULL,
        NULL)) {
        fprintf(stderr, "GDFIMAGE: internal error\n");
        GdfExitProcess(1);
    }
}

PFST_STRING_ITEM
GdfAllocateFSTStringA(
    LPCSTR psz,
    PULONG StringIndex
    )
{
    PFST_STRING_ITEM StringItem;
    int cch;

    //
    // Allocate a string
    //

    cch = lstrlenA(psz);
    StringItem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(FST_STRING_ITEM) + cch + 1);

    if(!StringItem) {
        fprintf(stderr, "GDFIMAGE: Out of memory.\n");
        GdfExitProcess(1);
    }

    //
    // Fill in the string
    //

    StringItem->String = (LPSTR)(StringItem + 1);
    memcpy(StringItem->String, psz, cch);
    StringItem->String[cch] = 0;
    StringItem->cch = cch;

    //
    // Add it to the list
    //

    if(!GdfFSTStringListEnd)
        GdfFSTStringListEnd = &GdfFSTStringList;
    StringItem->Link = *GdfFSTStringListEnd;
    *GdfFSTStringListEnd = StringItem;
    GdfFSTStringListEnd = &StringItem->Link;

    //
    // Adjust the character count
    //

    *StringIndex = GdfStringTableSize;
    GdfStringTableSize += cch + 1;

    return StringItem;
}

PFST_STRING_ITEM
GdfAllocateFSTStringW(
    LPCWSTR pwsz,
    PULONG StringIndex
    )
{
    char sz[MAX_PATH];

    sz[0] = 0;
    GdfAppendUnicodeToAnsi(sz, pwsz);
    return GdfAllocateFSTStringA(sz, StringIndex);
}

PFST_FILE_ITEM
GdfCreateFSTEntry(
    ULONG FirstSector,
    ULONG FileSize,
    BOOL InsertAtEnd
    )
{
    PFST_FILE_ITEM FileItem;
    PFST_FILE_ITEM *InsertLocation;

    //
    // Allocate an entry
    //

    FileItem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(FST_FILE_ITEM));

    if(!FileItem) {
        fprintf(stderr, "GDFIMAGE: Out of memory.\n");
        GdfExitProcess(1);
    }

    //
    // Fill it in
    //

    FileItem->FirstSector = FirstSector;
    FileItem->LastSector = FirstSector + ((FileSize + ((1 << SECTOR_SHIFT)
        - 1)) >> SECTOR_SHIFT) - 1;

    //
    // Add it to the list
    //

    if(InsertAtEnd) {
        InsertLocation = GdfFSTFileListEnd;
        if(!InsertLocation)
            InsertLocation = GdfFSTFileListEnd = &GdfFSTFileList;
    } else {
        InsertLocation = &GdfFSTFileList;
        while(*InsertLocation && FirstSector > (*InsertLocation)->FirstSector)
            InsertLocation = &(*InsertLocation)->Link;
    }

    FileItem->Link = *InsertLocation;
    *InsertLocation = FileItem;
    if(InsertLocation == GdfFSTFileListEnd)
        GdfFSTFileListEnd = &FileItem->Link;

    return FileItem;
}

VOID
GdfFillRootDirectoryName(
    LPSTR psz,
    LPCWSTR pwsz
    )
{
    WCHAR Drive[_MAX_DRIVE];
    WCHAR Dir[_MAX_DIR];
    WCHAR Name[_MAX_FNAME];
    WCHAR Ext[_MAX_EXT];
    WCHAR Path[_MAX_PATH];

    //
    // Strip out the drive letter
    //

    _wfullpath(Path, pwsz, _MAX_PATH - 1);
    _wsplitpath(Path, Drive, Dir, Name, Ext);
    _wmakepath(Path, NULL, Dir, Name, Ext);

    //
    // Convert to ANSI
    //

    psz[0] = 0;
    GdfAppendUnicodeToAnsi(psz, Path);
}

VOID
GdfReadBootSectorFile(
    LPCWSTR pwszBootSectorPath
    )
{
    HANDLE hFile;
    LARGE_INTEGER FileSize;
    DWORD cbRead;
    PULONG BootSectorXBOXROMFirstSector;
    PULONG BootSectorXBOXROMFileSize;

    //
    // Open the boot sector file.
    //

    hFile = CreateFileW(pwszBootSectorPath, GENERIC_READ, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "GDFIMAGE: Unable to open boot sector file.\n");
        GdfExitProcess(1);
    }

    //
    // Get the size of the boot sector file and verify that it's exactly one
    // sector in size.
    //

    if (!GetFileSizeEx(hFile, &FileSize)) {
        fprintf(stderr, "GDFIMAGE: Failed to access boot sector file.\n");
        GdfExitProcess(1);
    }

    if (FileSize.QuadPart != sizeof(GdfBootSector)) {
        fprintf(stderr, "GDFIMAGE: Boot sector size is not 2048 bytes.\n");
        GdfExitProcess(1);
    }

    //
    // Read in the boot sector file.
    //

    if (!ReadFile(hFile, GdfBootSector, sizeof(GdfBootSector), &cbRead, NULL) ||
        (cbRead != sizeof(GdfBootSector))) {
        fprintf(stderr, "GDFIMAGE: Failed to read boot sector file.\n");
        GdfExitProcess(1);
    }

    //
    // Validate that the boot sector has the two magic entries to store the
    // first sector and file size of XBOXROM.BIN.
    //

    BootSectorXBOXROMFirstSector = (PULONG)(GdfBootSector + SECTOR_SIZE - 10);
    BootSectorXBOXROMFileSize = (PULONG)(GdfBootSector + SECTOR_SIZE - 6);

    if ((*BootSectorXBOXROMFirstSector != 0xCDCDCDCD) ||
        (*BootSectorXBOXROMFileSize != 0xDCDCDCDC)) {
        fprintf(stderr, "GDFIMAGE: Invalid boot sector file.\n");
        GdfExitProcess(1);
    }

    //
    // Indicate that we are creating a bootable media.
    //

    GdfHaveBootSector = TRUE;

    CloseHandle(hFile);
}

VOID
GdfProcessCommandLineOptions(
    int argc,
    wchar_t *argv[]
    )
{
    ULONG SourceDirectoryPathLength;

    //
    // Skip past the executable file name parameter (argv[0]).
    //

    argv++;
    argc--;

    //
    // If no arguments were specified, then print out the tool's usage.
    //

    if (argc == 0) {
PrintUsage:
        printf("Usage: GDFIMAGE [options] sourceroot targetfile\n\n"
            "  /B[file]      Specifies the boot sector file\n"
            "  /FST          Creates a file for DVD emulation rather than for DVD\n");
        GdfExitProcess(0);
    }

    //
    // Process the command line options.
    //

    do {

        if (argv[0][0] == L'-' || argv[0][0] == L'/') {

            switch (argv[0][1]) {

                case L'?':
                    goto PrintUsage;

                case L'b':
                case L'B':
                    GdfReadBootSectorFile(&argv[0][2]);
                    break;

                case L'F':
                case L'f':
                    if(0 != lstrcmpiW(&argv[0][2], L"ST"))
                        goto PrintUsage;
                    GdfCreateFST = TRUE;
                    break;

                default:
                    fprintf(stderr, "GDFIMAGE: Invalid command line.\n");
                    GdfExitProcess(1);
            }

            argv++;
            argc--;

        } else {
            break;
        }

    } while (argc > 0);

    //
    // There should be exactly two arguments remaining.
    //

    if (argc != 2) {
        fprintf(stderr, "GDFIMAGE: Invalid command line.\n");
        GdfExitProcess(1);
    }

    GdfSourceDirectoryPath = argv[0];
    GdfTargetFilePath = argv[1];

    //
    // If the user gives us a source path with a trailing slash, remove it.
    //

    SourceDirectoryPathLength = lstrlenW(GdfSourceDirectoryPath);

    if ((SourceDirectoryPathLength > 0) &&
        ((GdfSourceDirectoryPath[SourceDirectoryPathLength - 1] == L'\\') ||
        (GdfSourceDirectoryPath[SourceDirectoryPathLength - 1] == L'/'))) {
        GdfSourceDirectoryPath[SourceDirectoryPathLength - 1] = L'\0';
    }
}

VOID
GdfCheckSourceDirectoryExists(
    VOID
    )
{
    DWORD dwFileAttributes;

    dwFileAttributes = GetFileAttributesW(GdfSourceDirectoryPath);

    if (dwFileAttributes == 0xFFFFFFFF) {
        fprintf(stderr, "GDFIMAGE: Source path does not exist.\n");
        GdfExitProcess(1);
    }

    if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        fprintf(stderr, "GDFIMAGE: Source path is not a directory.\n");
        GdfExitProcess(1);
    }
}

VOID
GdfOpenTargetFile(
    VOID
    )
{
    WCHAR SourceDrive[_MAX_DRIVE];
    WCHAR FSTDrive[_MAX_DRIVE];
    WCHAR FSTDir[_MAX_DIR];
    WCHAR FSTName[_MAX_FNAME];
    WCHAR FSTExt[_MAX_EXT];
    WCHAR BINPath[_MAX_PATH];
    int cch;

    if(GdfCreateFST) {

        //
        // First we have to ensure that the target file and the source
        // directory tree are on the same drive
        //

        _wfullpath(BINPath, GdfSourceDirectoryPath, _MAX_PATH - 1);
        _wsplitpath(BINPath, SourceDrive, NULL, NULL, NULL);
        _wfullpath(BINPath, GdfTargetFilePath, _MAX_PATH - 1);
        _wsplitpath(BINPath, FSTDrive, FSTDir, FSTName, FSTExt);
        if(SourceDrive[0] && FSTDrive[0] && 0 != lstrcmpiW(SourceDrive,
            FSTDrive))
        {
            fprintf(stderr, "GDFIMAGE: Source directory and target file must be on the same drive.\n");
            GdfExitProcess(1);
        }

        //
        // Now we create the FST file
        //

        GdfFSTFileHandle = CreateFileW(GdfTargetFilePath, GENERIC_READ |
            GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (GdfFSTFileHandle == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "GDFIMAGE: Unable to open target file.\n");
            GdfExitProcess(1);
        }

        //
        // Construct a file name for the directory BIN file and add it to the
        // string table
        //

        _wmakepath(BINPath, NULL, FSTDir, NULL, NULL);
        cch = lstrlenW(BINPath);
        if(cch > 0 && BINPath[cch - 1] == '\\')
            BINPath[--cch] = 0;
        GdfAllocateFSTStringW(BINPath, &GdfDirectoryDirectoryIndex);
        if(0 == lstrcmpiW(FSTExt, L"BIN")) {
            _wmakepath(BINPath, NULL, NULL, FSTName, L"FST.BIN");
            GdfAllocateFSTStringW(BINPath, &GdfDirectoryFileIndex);
            _wmakepath(BINPath, FSTDrive, FSTDir, FSTName, L"FST.BIN");
        } else {
            _wmakepath(BINPath, NULL, NULL, FSTName, L"BIN");
            GdfAllocateFSTStringW(BINPath, &GdfDirectoryFileIndex);
            _wmakepath(BINPath, FSTDrive, FSTDir, FSTName, L"BIN");
        }

        //
        // Now create the BIN file
        //

        GdfTargetFileHandle = CreateFileW(BINPath, GENERIC_READ |
            GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (GdfTargetFileHandle == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "GDFIMAGE: Unable to open target file.\n");
            GdfExitProcess(1);
        }
    } else {
        GdfTargetFileHandle = CreateFileW(GdfTargetFilePath, GENERIC_READ |
            GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (GdfTargetFileHandle == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "GDFIMAGE: Unable to open target file.\n");
            GdfExitProcess(1);
        }
    }
}

VOID
GdfWriteTargetFile(
    LPVOID lpvBuffer,
    DWORD cbNumberOfBytes
    )
{
    DWORD cbWritten;

    if (GdfAbnormalTermination) {
        GdfExitProcess(1);
    }

    if (!WriteFile(GdfTargetFileHandle, lpvBuffer, cbNumberOfBytes, &cbWritten,
        NULL) || (cbWritten != cbNumberOfBytes)) {
        fprintf(stderr, "GDFIMAGE: Failed to write to target file.\n");
        GdfExitProcess(1);
    }
}

VOID
GdfSeekByteTargetFile(
    PULARGE_INTEGER puliByteOffset
    )
{
    if (INVALID_SET_FILE_POINTER == SetFilePointer(GdfTargetFileHandle,
        (LONG)puliByteOffset->LowPart, (LPLONG)&(puliByteOffset->HighPart),
        FILE_BEGIN) && GetLastError() != NO_ERROR) {
        fprintf(stderr, "GDFIMAGE: Failed to access target file.\n");
        GdfExitProcess(1);
    }
}

VOID
GdfSeekSectorTargetFile(
    DWORD dwSectorNumber
    )
{
    ULARGE_INTEGER uliByteOffset;

    uliByteOffset.QuadPart = ((ULONGLONG)dwSectorNumber << SECTOR_SHIFT);

    GdfSeekByteTargetFile(&uliByteOffset);
}

VOID
GdfWriteFilledSectors(
    DWORD dwNumberOfSectors,
    BYTE Fill
    )
{
    BYTE SectorOfZeroes[SECTOR_SIZE];

    FillMemory(SectorOfZeroes, sizeof(SectorOfZeroes), Fill);

    while (dwNumberOfSectors--) {
        GdfWriteTargetFile(SectorOfZeroes, sizeof(SectorOfZeroes));
    }
}

VOID
GdfWriteFSTFile(
    VOID
    )
{
    ULONG Temp;
    char StringTemp[32];
    ULARGE_INTEGER Offset;
    PFST_FILE_ITEM FileItem;
    PFST_STRING_ITEM StringItem;

    //
    // Close the target file and switch to the FST
    //

    CloseHandle(GdfTargetFileHandle);
    GdfTargetFileHandle = GdfFSTFileHandle;

    //
    // First write out a header
    //

    memset(StringTemp, 0, sizeof StringTemp);
    strcpy(StringTemp, "AMC Virtual Media");
    GdfWriteTargetFile(StringTemp, sizeof StringTemp);
    memset(StringTemp, 0, sizeof StringTemp);
    strcpy(StringTemp, "Xbox DVD-ROM");
    GdfWriteTargetFile(StringTemp, sizeof StringTemp);
    Temp = 1715632;
    GdfWriteTargetFile(&Temp, 4);
    GdfWriteTargetFile(&Temp, 4);
    Offset.QuadPart = 192;
    GdfSeekByteTargetFile(&Offset);
    Temp = 0xABCD;
    GdfWriteTargetFile(&Temp, 2);
    Temp = time(NULL);
    GdfWriteTargetFile(&Temp, 4);

    //
    // Count up the number of file entries we have
    //

    Temp = 0;
    FileItem = GdfFSTFileList;
    while(FileItem != NULL) {
        ++Temp;
        FileItem = FileItem->Link;
    }

    //
    // Write out the size of the entry list and the size of the string table
    //

    GdfWriteTargetFile(&Temp, 4);
    GdfWriteTargetFile(&GdfStringTableSize, 4);

    //
    // Write out the file entries
    //

    FileItem = GdfFSTFileList;
    while(FileItem != NULL) {
        GdfWriteTargetFile(&FileItem->FirstSector, 20);
        FileItem = FileItem->Link;
    }

    //
    // Write out the string table
    //

    StringItem = GdfFSTStringList;
    while(StringItem != NULL) {
        GdfWriteTargetFile(StringItem->String, StringItem->cch + 1);
        StringItem = StringItem->Link;
    }
}

ULONG
GdfAllocateSectors(
    ULONG NumberOfBytes
    )
{
    ULONG FirstSector;

    FirstSector = GdfEndOfMedia;

    GdfEndOfMedia += (((NumberOfBytes + SECTOR_SIZE - 1) & ~SECTOR_MASK) >> SECTOR_SHIFT);

    return FirstSector;
}

int
GdfCompareFileNames(
    LPCWSTR pwszFileName1,
    LPCWSTR pwszFileName2
    )
{
    LONG n1, n2;
    LPCWSTR Limit;
    UCHAR c1, c2;

    //
    // The following code is adapted from RtlCompareString in ntos\rtl\string.c.
    //

    n1 = (LONG)lstrlenW(pwszFileName1);
    n2 = (LONG)lstrlenW(pwszFileName2);
    Limit = pwszFileName1 + (n1 <= n2 ? n1 : n2);
    while (pwszFileName1 < Limit) {
        c1 = (UCHAR)(*pwszFileName1++);
        c2 = (UCHAR)(*pwszFileName2++);
        if (c1 != c2) {
            c1 = GdfUpperCaseTable[c1];
            c2 = GdfUpperCaseTable[c2];
            if (c1 != c2) {
                return (LONG)c1 - (LONG)c2;
            }
        }
    }
    return n1 - n2;
}

VOID
GdfBuildSourceDirectoryList(
    LPCWSTR pwszSourceDirectoryPath,
    PGDF_FILE_NODE *ReturnedFileNodeList
    )
{
    HANDLE hFind;
    WCHAR wszPathName[MAX_PATH + 16];
    WIN32_FIND_DATAW FindFileData;
    PGDF_FILE_NODE FileNodeList;
    LPWSTR pwszTemp;
    PGDF_FILE_NODE NewFileNode;
    PGDF_FILE_NODE *PreviousFileNode;
    PGDF_FILE_NODE FileNode;

    //
    // Loop over the directory and allocate a GDF_FILE_NODE for each entry that
    // we want to store.  Build an unordered linked list of these nodes.
    //

    FileNodeList = NULL;

    lstrcpyW(wszPathName, pwszSourceDirectoryPath);
    lstrcatW(wszPathName, L"\\*");

    hFind = FindFirstFileW(wszPathName, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE) {

        if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            fprintf(stderr, "GDFIMAGE: Failed to enumerate files.\n");
            GdfExitProcess(1);
        }

        *ReturnedFileNodeList = NULL;
        return;
    }

    do {

        //
        // Skip the "." and ".." special directory entries.
        //

        if ((lstrcmpW(FindFileData.cFileName, L".") == 0) ||
            (lstrcmpW(FindFileData.cFileName, L"..") == 0)) {
            continue;
        }

        //
        // Files sizes are limited to 32-bits.
        //

        if (FindFileData.nFileSizeHigh != 0) {
            fprintf(stderr, "GDFIMAGE: Source file '%ws' too large.\n",
                FindFileData.cFileName);
            GdfExitProcess(1);
        }

        //
        // File names must use only the lower 256 characters of Unicode.
        //

        for (pwszTemp = FindFileData.cFileName; *pwszTemp != L'\0'; *pwszTemp++) {
            if (*pwszTemp >= 256) {
                fprintf(stderr, "GDFIMAGE: Source file '%ws' contains unsupported characters.\n",
                    FindFileData.cFileName);
                GdfExitProcess(1);
            }
        }

        //
        // Allocate a new file node and initialize its contents.
        //

        NewFileNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
            sizeof(GDF_FILE_NODE));

        if (NewFileNode == NULL) {
            fprintf(stderr, "GDFIMAGE: Out of memory.\n");
            GdfExitProcess(1);
        }

        NewFileNode->FileSize = FindFileData.nFileSizeLow;
        NewFileNode->FileAttributes = FindFileData.dwFileAttributes;
        NewFileNode->FileNameLength = lstrlenW(FindFileData.cFileName);
        lstrcpyW(NewFileNode->FileName, FindFileData.cFileName);

        //
        // Insert the node in sorted order by name.
        //

        PreviousFileNode = &FileNodeList;
        FileNode = FileNodeList;

        while (FileNode != NULL) {

            if (GdfCompareFileNames(NewFileNode->FileName, FileNode->FileName) <= 0) {
                break;
            }

            PreviousFileNode = &FileNode->Link;
            FileNode = FileNode->Link;
        }

        *PreviousFileNode = NewFileNode;
        NewFileNode->Link = FileNode;

    } while (FindNextFileW(hFind, &FindFileData));

    if (GetLastError() != ERROR_NO_MORE_FILES) {
        fprintf(stderr, "GDFIMAGE: Failed to enumerate files.\n");
        GdfExitProcess(1);
    }

    FindClose(hFind);

    //
    // Return the list to the caller.
    //

    *ReturnedFileNodeList = FileNodeList;
}

VOID
GdfLayoutSourceDirectoryList(
    PGDF_FILE_NODE FileNodeList,
    PULONG ReturnedDirectoryFileSize
    )
{
    PGDF_FILE_NODE FileNode;
    ULONG DirectoryByteOffset;
    ULONG EntryBytesRequired;

    //
    // Layout the directory entries.  For now, we store the entries as a simple
    // alphabetized list.
    //

    FileNode = FileNodeList;
    DirectoryByteOffset = 0;

    while (FileNode != NULL) {

        //
        // Compute the number of bytes required to hold the directory entry.
        //

        EntryBytesRequired = FIELD_OFFSET(GDF_DIRECTORY_ENTRY, FileName) +
            FileNode->FileNameLength;
        EntryBytesRequired = (EntryBytesRequired + sizeof(ULONG) - 1) & ~(sizeof(ULONG) - 1);

        //
        // If there aren't enough bytes in the sector to hold this directory
        // entry, then skip ahead to the next sector.
        //

        if ((SECTOR_SIZE - (DirectoryByteOffset & SECTOR_MASK)) < EntryBytesRequired) {
            DirectoryByteOffset = (DirectoryByteOffset + SECTOR_SIZE - 1) & ~SECTOR_MASK;
        }

        FileNode->DirectoryByteOffset = DirectoryByteOffset;

        DirectoryByteOffset += EntryBytesRequired;
        FileNode = FileNode->Link;
    }

    *ReturnedDirectoryFileSize = DirectoryByteOffset;
}

VOID
GdfWriteSourceDirectoryList(
    PGDF_FILE_NODE FileNodeList,
    ULONG DirectoryFirstSector,
    ULONG DirectoryFileSize
    )
{
    ULONG NumberOfSectors;
    PGDF_FILE_NODE FileNode;
    ULARGE_INTEGER uliByteOffset;
    GDF_DIRECTORY_ENTRY DirectoryEntry;
    UCHAR FileName[MAX_PATH];
    PWSTR pwszTemp;
    PUCHAR pszTemp;

    //
    // Fill the area that the directory will occupy with negative ones so that
    // the stream is set up properly for the padding needed for incomplete
    // sectors.
    //

    NumberOfSectors = (((DirectoryFileSize + SECTOR_SIZE - 1) & ~SECTOR_MASK) >> SECTOR_SHIFT);
    GdfSeekSectorTargetFile(DirectoryFirstSector);
    GdfWriteFilledSectors(NumberOfSectors, 0xFF);

    //
    // Loop over the file node list and write out each directory entry.
    //

    FileNode = FileNodeList;

    while (FileNode != NULL) {

        //
        // Fill in the directory entry.
        //

        ZeroMemory(&DirectoryEntry, sizeof(DirectoryEntry));

        DirectoryEntry.FirstSector = FileNode->FirstSector;
        DirectoryEntry.FileSize = FileNode->FileSize;
        DirectoryEntry.FileAttributes = (UCHAR)FileNode->FileAttributes;
        DirectoryEntry.FileNameLength = (UCHAR)FileNode->FileNameLength;

        if (FileNode->Link != NULL) {
            DirectoryEntry.RightEntryIndex = (USHORT)((FileNode->Link->DirectoryByteOffset >> 2));
        }

        //
        // Build the 8-bit version of the file name.
        //

        pwszTemp = FileNode->FileName;
        pszTemp = FileName;

        while (*pwszTemp != L'\0') {
            *pszTemp++ = (UCHAR)(*pwszTemp++);
        }

        //
        // Seek to the directory entry's byte offset and write out the entry.
        //

        uliByteOffset.QuadPart = ((ULONGLONG)DirectoryFirstSector << SECTOR_SHIFT) +
            FileNode->DirectoryByteOffset;
        GdfSeekByteTargetFile(&uliByteOffset);
        GdfWriteTargetFile(&DirectoryEntry, FIELD_OFFSET(GDF_DIRECTORY_ENTRY, FileName));
        GdfWriteTargetFile(FileName, DirectoryEntry.FileNameLength);

        //
        // Advance to the next node.
        //

        FileNode = FileNode->Link;
    }
}

VOID
GdfFreeSourceDirectoryList(
    PGDF_FILE_NODE FileNodeList
    )
{
    PGDF_FILE_NODE NextLink;

    while (FileNodeList != NULL) {
        NextLink = FileNodeList->Link;
        HeapFree(GetProcessHeap(), 0, FileNodeList);
        FileNodeList = NextLink;
    }
}

VOID
GdfProcessSourceFileForFST(
    PULONG ReturnedFileFirstSector,
    ULONG FileSize,
    LPCWSTR FileName,
    ULONG DirectoryStringIndex
    )
{
    PFST_FILE_ITEM FSTEntry;
    ULONG FirstSector;

    //
    // Trivial case.  If the file is empty, then we don't need to allocate any
    // sectors for the file.
    //

    if (FileSize == 0) {
        *ReturnedFileFirstSector = 0;
        return;
    }

    //
    // Allocate space for this file
    //

    FirstSector = GdfAllocateSectors(FileSize);

    //
    // Create an entry in the FST
    //

    FSTEntry = GdfCreateFSTEntry(FirstSector, FileSize, TRUE);
    FSTEntry->FileOffset = 0;
    FSTEntry->DirectoryIndex = DirectoryStringIndex;

    //
    // Create a string table entry for this file name
    //

    GdfAllocateFSTStringW(FileName, &FSTEntry->FileIndex);

    *ReturnedFileFirstSector = FirstSector;
}

VOID
GdfProcessSourceFile(
    LPCWSTR pwszSourceFilePath,
    PULONG ReturnedFileFirstSector,
    ULONG FileSize
    )
{
    HANDLE hFile;
    ULONG FileFirstSector;
    DWORD cbBytesRemaining;
    DWORD cbRead;
    DWORD cbZeroPadding;

    //
    // Trivial case.  If the file is empty, then we don't need to allocate any
    // sectors for the file.
    //

    if (FileSize == 0) {
        *ReturnedFileFirstSector = 0;
        return;
    }

    //
    // Open the source file.
    //

    hFile = CreateFileW(pwszSourceFilePath, GENERIC_READ, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL |
        FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "GDFIMAGE: Unable to open source file.\n");
        GdfExitProcess(1);
    }

    //
    // Allocate the sectors for the file.
    //

    FileFirstSector = GdfAllocateSectors(FileSize);
    GdfSeekSectorTargetFile(FileFirstSector);

    //
    // Copy the source file to the target file.
    //

    cbBytesRemaining = FileSize;

    do {

        if (!ReadFile(hFile, GdfTransferBuffer, sizeof(GdfTransferBuffer),
            &cbRead, NULL)) {
            fprintf(stderr, "GDFIMAGE: Unable to read source file.\n");
            GdfExitProcess(1);
        }

        GdfWriteTargetFile(GdfTransferBuffer, cbRead);

        cbBytesRemaining -= cbRead;

    } while (cbBytesRemaining > 0);

    //
    // Pad the rest of the last sector with zeroes.
    //

    cbZeroPadding = SECTOR_SIZE - (FileSize & SECTOR_MASK);

    if (cbZeroPadding != SECTOR_SIZE) {
        FillMemory(GdfTransferBuffer, cbZeroPadding, 0);
        GdfWriteTargetFile(GdfTransferBuffer, cbZeroPadding);
    }

    CloseHandle(hFile);

    *ReturnedFileFirstSector = FileFirstSector;
}

VOID
GdfProcessSourceDirectory(
    LPCWSTR pwszSourceDirectoryPath,
    LPCSTR pszSourceDirectoryAnsiPath,
    PULONG ReturnedDirectoryFirstSector,
    PULONG ReturnedDirectoryFileSize
    )
{
    PGDF_FILE_NODE FileNodeList;
    ULONG DirectoryFileSize;
    ULONG DirectoryFirstSector;
    PGDF_FILE_NODE FileNode;
    WCHAR wszSourceFileName[MAX_PATH];
    char szChildDirectoryAnsiName[MAX_PATH];
    ULONG DirectoryStringIndex;
    PFST_FILE_ITEM DirectoryFSTEntry = NULL;

    printf("Processing directory %ws.\n", pwszSourceDirectoryPath);

    //
    // Build the list of files in this directory.
    //

    GdfBuildSourceDirectoryList(pwszSourceDirectoryPath, &FileNodeList);

    //
    // Check if this is a non-empty directory.
    //

    if (FileNodeList != NULL) {

        //
        // If we're making an FST file, allocate a string for this
        // directory
        //

        GdfAllocateFSTStringA(pszSourceDirectoryAnsiPath,
            &DirectoryStringIndex);

        //
        // Determine the byte offsets for all of the directory's files.
        //

        GdfLayoutSourceDirectoryList(FileNodeList, &DirectoryFileSize);

        //
        // Allocate the sectors for the directory stream.
        //

        DirectoryFirstSector = GdfAllocateSectors(DirectoryFileSize);
        if(GdfCreateFST) {
            DirectoryFSTEntry = GdfCreateFSTEntry(DirectoryFirstSector,
                DirectoryFileSize, TRUE);
            DirectoryFSTEntry->DirectoryIndex = GdfDirectoryDirectoryIndex;
            DirectoryFSTEntry->FileIndex = GdfDirectoryFileIndex;
        }


        //
        // Write out all of the files in this directory.
        //

        FileNode = FileNodeList;

        do {

            if ((FileNode->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {

                if(GdfCreateFST) {
                    GdfProcessSourceFileForFST(&FileNode->FirstSector,
                        FileNode->FileSize, FileNode->FileName,
                        DirectoryStringIndex);
                } else {
                    lstrcpyW(wszSourceFileName, pwszSourceDirectoryPath);
                    lstrcatW(wszSourceFileName, L"\\");
                    lstrcatW(wszSourceFileName, FileNode->FileName);

                    GdfProcessSourceFile(wszSourceFileName,
                        &FileNode->FirstSector, FileNode->FileSize);
                }

                //
                // If this is the root directory and we've found XBOXROM.BIN,
                // then remember where the file is located so that we can later
                // build the boot sector if necessary.
                //

                if ((pwszSourceDirectoryPath == GdfSourceDirectoryPath) &&
                    (lstrcmpiW(FileNode->FileName, L"XBOXROM.BIN") == 0)) {
                    GdfXBOXROMFirstSector = FileNode->FirstSector;
                    GdfXBOXROMFileSize = FileNode->FileSize;
                }
            }

            FileNode = FileNode->Link;

        } while (FileNode != NULL);

        //
        // Write out all of the files in this directory.
        //

        FileNode = FileNodeList;

        do {

            if ((FileNode->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {

                lstrcpyW(wszSourceFileName, pwszSourceDirectoryPath);
                lstrcatW(wszSourceFileName, L"\\");
                lstrcatW(wszSourceFileName, FileNode->FileName);

                lstrcpyA(szChildDirectoryAnsiName, pszSourceDirectoryAnsiPath);
                lstrcatA(szChildDirectoryAnsiName, "\\");
                GdfAppendUnicodeToAnsi(szChildDirectoryAnsiName,
                    FileNode->FileName);

                GdfProcessSourceDirectory(wszSourceFileName,
                    szChildDirectoryAnsiName, &FileNode->FirstSector,
                    &FileNode->FileSize);
            }

            FileNode = FileNode->Link;

        } while (FileNode != NULL);

        //
        // Write out the directory stream.
        //

        if(GdfCreateFST) {
            DirectoryFSTEntry->FileOffset = GdfCurrentMediaPosition <<
                SECTOR_SHIFT;
            GdfWriteSourceDirectoryList(FileNodeList, GdfCurrentMediaPosition,
                DirectoryFileSize);
            GdfCurrentMediaPosition += (DirectoryFileSize +
                ((1 << SECTOR_SHIFT) - 1)) >> SECTOR_SHIFT;
        } else {
            GdfWriteSourceDirectoryList(FileNodeList, DirectoryFirstSector,
                DirectoryFileSize);
        }

        //
        // Free the list of files in the directory.
        //

        GdfFreeSourceDirectoryList(FileNodeList);

    } else {

        //
        // The directory is empty.  Return a zero starting sector and size to
        // the caller.
        //

        DirectoryFirstSector = 0;
        DirectoryFileSize = 0;
    }

    *ReturnedDirectoryFirstSector = DirectoryFirstSector;
    *ReturnedDirectoryFileSize = DirectoryFileSize;
}

VOID
GdfWriteEndOfMediaPadding(
    VOID
    )
{
    ULONG OldEndOfMedia;

    OldEndOfMedia = GdfEndOfMedia;

    //
    // The media needs to be padded up to a 64K block boundary.
    //

    GdfEndOfMedia = (GdfEndOfMedia + 1 + (65536 / SECTOR_SIZE)) & ~((65536 / SECTOR_SIZE) - 1);

    //
    // Fill the padding sectors with zeroes.
    //

    GdfSeekSectorTargetFile(OldEndOfMedia);
    GdfWriteFilledSectors(GdfEndOfMedia - OldEndOfMedia, 0);
}

VOID
GdfFinalizeBootSector(
    VOID
    )
{
    PULONG BootSectorXBOXROMFirstSector;
    PULONG BootSectorXBOXROMFileSize;

    //
    // Verify that we placed an XBOXROM.BIN in the root of the image.
    //

    if (GdfXBOXROMFirstSector == 0) {
        fprintf(stderr, "GDFIMAGE: XBOXROM.BIN is required in order to a create bootable disc.\n");
        GdfExitProcess(1);
    }

    //
    // Store the first sector and file size of XBOXROM.BIN in the boot sector.
    //

    BootSectorXBOXROMFirstSector = (PULONG)(GdfBootSector + SECTOR_SIZE - 10);
    BootSectorXBOXROMFileSize = (PULONG)(GdfBootSector + SECTOR_SIZE - 6);

    *BootSectorXBOXROMFirstSector = GdfXBOXROMFirstSector;
    *BootSectorXBOXROMFileSize = GdfXBOXROMFileSize;
}

VOID
GdfWriteGdfVolumeDescriptor(
    ULONG RootDirectoryFirstSector,
    ULONG RootDirectoryFileSize
    )
{
    GDF_VOLUME_DESCRIPTOR VolumeDescriptor;

    //
    // Fill in the volume descriptor.
    //

    ZeroMemory(&VolumeDescriptor, sizeof(VolumeDescriptor));

    CopyMemory(VolumeDescriptor.HeadSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE,
        sizeof(VolumeDescriptor.HeadSignature));

    VolumeDescriptor.RootDirectoryFirstSector = RootDirectoryFirstSector;
    VolumeDescriptor.RootDirectoryFileSize = RootDirectoryFileSize;

    GetSystemTimeAsFileTime((LPFILETIME)&VolumeDescriptor.TimeStamp);

    CopyMemory(VolumeDescriptor.TailSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE,
        sizeof(VolumeDescriptor.TailSignature));

    //
    // Write out the GDF volume descriptor.
    //

    GdfWriteTargetFile(&VolumeDescriptor, sizeof(VolumeDescriptor));
}

VOID
GdfWriteIso9960PrimaryVolumeDescriptor(
    VOID
    )
{
    RAW_ISO_VD VolumeDescriptor;

    //
    // Fill in the volume descriptor.
    //

    ZeroMemory(&VolumeDescriptor, sizeof(VolumeDescriptor));

    VolumeDescriptor.DescType = VD_PRIMARY;
    CopyMemory(VolumeDescriptor.StandardId, ISO_VOL_ID, sizeof(VolumeDescriptor.StandardId));
    VolumeDescriptor.Version = VERSION_1;
    VolumeDescriptor.VolSpaceI = GdfEndOfMedia;
    VolumeDescriptor.VolSpaceM = GdfByteSwap32(GdfEndOfMedia);
    VolumeDescriptor.VolSetSizeI = 1;
    VolumeDescriptor.VolSetSizeM = GdfByteSwap16(1);
    VolumeDescriptor.VolSeqNumI = 1;
    VolumeDescriptor.VolSeqNumM = GdfByteSwap16(1);
    VolumeDescriptor.LogicalBlkSzI = SECTOR_SIZE;
    VolumeDescriptor.LogicalBlkSzM = GdfByteSwap16(SECTOR_SIZE);
    FillMemory(VolumeDescriptor.VolSetId, sizeof(VolumeDescriptor.VolSetId), ' ');
    FillMemory(VolumeDescriptor.PublId, sizeof(VolumeDescriptor.PublId), ' ');
    FillMemory(VolumeDescriptor.PreparerId, sizeof(VolumeDescriptor.PreparerId), ' ');
    FillMemory(VolumeDescriptor.AppId, sizeof(VolumeDescriptor.AppId), ' ');
    FillMemory(VolumeDescriptor.Copyright, sizeof(VolumeDescriptor.Copyright), ' ');
    FillMemory(VolumeDescriptor.Abstract, sizeof(VolumeDescriptor.Abstract), ' ');
    FillMemory(VolumeDescriptor.Bibliograph, sizeof(VolumeDescriptor.Bibliograph), ' ');
    strcpy((LPSTR)VolumeDescriptor.CreateDate, "0000000000000000");
    strcpy((LPSTR)VolumeDescriptor.ModDate, "0000000000000000");
    strcpy((LPSTR)VolumeDescriptor.ExpireDate, "0000000000000000");
    strcpy((LPSTR)VolumeDescriptor.EffectDate, "0000000000000000");
    VolumeDescriptor.FileStructVer = VERSION_1;

    //
    // Write out the ISO-9960 primary volume descriptor.
    //

    GdfWriteTargetFile(&VolumeDescriptor, sizeof(VolumeDescriptor));
}

VOID
GdfWriteElToritoBootVolumeDescriptor(
    VOID
    )
{
    UCHAR BootVolumeDescriptor[SECTOR_SIZE];

    //
    // The following code is adapted from CDIMAGE.EXE.
    //

    ZeroMemory(BootVolumeDescriptor, sizeof(BootVolumeDescriptor));

    BootVolumeDescriptor[ 0 ] = 0x00;        // boot volume descriptor
    BootVolumeDescriptor[ 6 ] = 0x01;        // volume descriptor version

    memcpy( BootVolumeDescriptor + 1, "CD001", 5 );
    memcpy( BootVolumeDescriptor + 7, "EL TORITO SPECIFICATION", 23 );

    *(UNALIGNED DWORD*)( BootVolumeDescriptor + 0x47 ) = GDF_ELTORITO_BOOT_CATALOG_SECTOR;

    //
    // Write out the El Torito boot volume descriptor.
    //

    GdfWriteTargetFile(BootVolumeDescriptor, sizeof(BootVolumeDescriptor));
}

VOID
GdfWriteIso9960TerminatingDescriptor(
    VOID
    )
{
    RAW_ISO_VD VolumeDescriptor;

    //
    // Fill in the volume descriptor.
    //

    ZeroMemory(&VolumeDescriptor, sizeof(VolumeDescriptor));

    VolumeDescriptor.DescType = VD_TERMINATOR;
    CopyMemory(VolumeDescriptor.StandardId, ISO_VOL_ID, sizeof(VolumeDescriptor.StandardId));
    VolumeDescriptor.Version = VERSION_1;

    //
    // Write out the ISO-9960 terminating descriptor.
    //

    GdfWriteTargetFile(&VolumeDescriptor, sizeof(VolumeDescriptor));
}

VOID
GdfWriteElToritoBootCatalog(
    VOID
    )
{
    UCHAR BootCatalog[SECTOR_SIZE];
    DWORD CheckSum;
    DWORD i;

    //
    // The following code is adapted from CDIMAGE.EXE.
    //

    //
    // First 0x20 bytes is the validation entry
    //

    BootCatalog[ 0x00 ] = 0x01;     // Validation Entry Header ID
    BootCatalog[ 0x01 ] = 0x00;     // Platform ID = 80x86

    memcpy( BootCatalog + 4, "Microsoft Corporation", 21 );

    BootCatalog[ 0x1E ] = 0x55;     // key byte
    BootCatalog[ 0x1F ] = 0xAA;     // key byte

    for ( CheckSum = 0, i = 0; i < 0x20; i += 2 ) {
        CheckSum += *(UNALIGNED WORD*)( BootCatalog + i );
        }

    CheckSum = ( 0x10000 - ( CheckSum & 0xFFFF ));

    *(UNALIGNED WORD*)( BootCatalog + 0x1C ) = (WORD)CheckSum;

    //
    // Second 0x20 bytes is the initial/default entry.
    //

    BootCatalog[ 0x20 ] = 0x88;             // Initial/Default Entry Boot Indicator
    BootCatalog[ 0x21 ] = 0;                // "no emulation" mode

    // Load Segment (where to load in DOS memory).  If zero, loads at 0x07C0.

    *(UNALIGNED WORD*)(BootCatalog + 0x22) = 0;

    // Sector Count (number of 512-byte "virtual sectors" to load from CD):

    *(UNALIGNED WORD*)( BootCatalog + 0x26 ) = (SECTOR_SIZE / 512);

    // Load address (CD sector number) of boot sector file:

    *(UNALIGNED DWORD*)( BootCatalog + 0x28 ) = GDF_ELTORITO_BOOT_SECTOR;

    //
    // Write out the El Torito boot catalog.
    //

    GdfWriteTargetFile(BootCatalog, sizeof(BootCatalog));
}

BOOL
WINAPI
GdfConsoleCtrlHandler(
    DWORD dwCtrlType
    )
{
    switch (dwCtrlType) {

        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            GdfAbnormalTermination = TRUE;
            return TRUE;

        default:
            return FALSE;
    }
}

int
__cdecl
wmain(
    int argc,
    wchar_t *argv[]
    )
{
    ULONG RootDirectoryFirstSector;
    ULONG RootDirectoryFileSize;
    PFST_FILE_ITEM VolumeDescriptorFSTEntry;
    char szRootDirectoryName[MAX_PATH];

    //
    // Print the banner.
    //

    fprintf(stderr, "Microsoft (R) Xbox Game Disc Premastering Utility %s\n", VER_PRODUCTVERSION_STR);
    fprintf(stderr, "Copyright (C) Microsoft Corporation 2001. All rights reserved.\n\n");

    //
    // Process the command line arguments.
    //

    GdfProcessCommandLineOptions(argc, argv);

    //
    // Check if the source path exists and that it is a directory.
    //

    GdfCheckSourceDirectoryExists();

    //
    // Set a console control handler to watch for break events so that we can
    // delete the output file.
    //

    SetConsoleCtrlHandler(GdfConsoleCtrlHandler, TRUE);

    //
    // Open the target output file.
    //

    GdfOpenTargetFile();

    //
    // Zero fill the data from the start of the volume to the first data sector.
    //

    if(!GdfCreateFST)
        GdfWriteFilledSectors(GDF_STARTING_DATA_SECTOR, 0);

    //
    // Start adding all of the files from the specified root directory.
    //

    if(GdfCreateFST)
        GdfFillRootDirectoryName(szRootDirectoryName, GdfSourceDirectoryPath);
    else
        szRootDirectoryName[0] = 0;
    GdfProcessSourceDirectory(GdfSourceDirectoryPath, szRootDirectoryName,
        &RootDirectoryFirstSector, &RootDirectoryFileSize);

    if(!GdfCreateFST) {

        //
        // Add padding sectors to the end of the media.
        //

        GdfWriteEndOfMediaPadding();

        //
        // If we're creating a PC bootable media, finalize the boot sector's
        // contents.
        //

        if (GdfHaveBootSector) {
            GdfFinalizeBootSector();
        }
    }

    //
    // Write out the GDF volume descriptor.
    //

    if(GdfCreateFST) {
        VolumeDescriptorFSTEntry =
            GdfCreateFSTEntry(GDF_VOLUME_DESCRIPTOR_SECTOR, 1 << SECTOR_SHIFT,
            FALSE);
        VolumeDescriptorFSTEntry->DirectoryIndex = GdfDirectoryDirectoryIndex;
        VolumeDescriptorFSTEntry->FileIndex = GdfDirectoryFileIndex;
        VolumeDescriptorFSTEntry->FileOffset = GdfCurrentMediaPosition << SECTOR_SHIFT;
        GdfSeekSectorTargetFile(GdfCurrentMediaPosition);
    } else
        GdfSeekSectorTargetFile(GDF_VOLUME_DESCRIPTOR_SECTOR);
    GdfWriteGdfVolumeDescriptor(RootDirectoryFirstSector, RootDirectoryFileSize);

    if(!GdfCreateFST) {

        //
        // Write out the ISO-9960 volume descriptor sequence.  Note that this is
        // only required in order to use tools such as CD burning software that
        // import .ISO images.
        //

        GdfSeekSectorTargetFile(FIRST_VD_SECTOR);

        GdfWriteIso9960PrimaryVolumeDescriptor();

        if (GdfHaveBootSector) {
            GdfWriteElToritoBootVolumeDescriptor();
        }

        GdfWriteIso9960TerminatingDescriptor();

        //
        // Write out the boot sector code.
        //

        if (GdfHaveBootSector) {
            GdfSeekSectorTargetFile(GDF_ELTORITO_BOOT_CATALOG_SECTOR);
            GdfWriteElToritoBootCatalog();
            GdfWriteTargetFile(GdfBootSector, sizeof(GdfBootSector));
        }
    }

    //
    // Write out the FST file
    //

    if(GdfCreateFST)
        GdfWriteFSTFile();

    GdfExitProcess(0);
}
