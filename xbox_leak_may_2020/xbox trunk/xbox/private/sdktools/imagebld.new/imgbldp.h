/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    imgbldp.h

Abstract:

    This module contains the private data structures and procedure prototypes
    for the utility program to build an Xbox executable image.

--*/

#ifndef _IMGBLDP_
#define _IMGBLDP_

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <errno.h>
#include <xbeimage.h>
#include <cklibver.h>
#include <sha.h>
#include "imgbldrc.h"

//
// Define the size of a processor page.
//

#define PAGE_SIZE                               0x1000

//
// Define a macro to align a virtual address to a page boundary.
//

#define PAGE_ALIGN(Va) ((PVOID)((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1)))

//
// Define a macro to obtain the byte offset within a page for a virtual address.
//

#define BYTE_OFFSET(Va) ((ULONG)((LONG_PTR)(Va) & (PAGE_SIZE - 1)))

//
// Define a macro to round a number of bytes to the nearest page boundary.
//

#define ROUND_TO_PAGES(Size) (((ULONG_PTR)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

//
// Define the structure to track sections that should not be preloaded.
//

typedef struct _IMGB_NOPRELOAD {
    LIST_ENTRY ListEntry;
    LPSTR SectionName;
} IMGB_NOPRELOAD, *PIMGB_NOPRELOAD;

//
// Define the structure to track files that should be inserted into the image as
// sections.
//

typedef struct _IMGB_INSERTFILE {
    LIST_ENTRY ListEntry;
    HANDLE FileHandle;
    ULONG FileSize;
    LPSTR FilePath;
    LPSTR SectionName;
    ULONG FileByteOffset;
    BOOLEAN NoPreload;
    BOOLEAN ReadOnly;
} IMGB_INSERTFILE, *PIMGB_INSERTFILE;

//
// Define the default alignment of /INSERTFILE sections.
//

#define IMGB_INSERTFILE_SECTION_ALIGNMENT       32

//
// Define the maximum number of bytes that may be mapped by an XBE image.
//

#define IMGB_MAXIMUM_IMAGE_SIZE                 0x80000000

//
// Define the structures to track data associated with the various subheaders of
// an XBE image.
//

typedef struct _IMGB_GENERIC_HEADER {
    LIST_ENTRY HeadersListEntry;
    PVOID VirtualAddress;
    ULONG VirtualSize;
    virtual VOID Write() = 0;
} IMGB_GENERIC_HEADER, *PIMGB_GENERIC_HEADER;

typedef struct _IMGB_XBEIMAGE_CERTIFICATE_HEADER: public IMGB_GENERIC_HEADER {
    VOID Write();
} IMGB_XBEIMAGE_CERTIFICATE_HEADER, *PIMGB_XBEIMAGE_CERTIFICATE_HEADER;

typedef struct _IMGB_XBEIMAGE_IMPORT_DESCRIPTOR_HEADER: public IMGB_GENERIC_HEADER {
    ULONG NumberOfNonKernelImports;
    ULONG SizeOfNonKernelImageNames;
    VOID Write();
} IMGB_XBEIMAGE_IMPORT_DESCRIPTOR_HEADER, *PIMGB_XBEIMAGE_IMPORT_DESCRIPTOR_HEADER;

typedef struct _IMGB_TLS_RAW_DATA_HEADER: public IMGB_GENERIC_HEADER {
    PIMAGE_TLS_DIRECTORY TlsDirectory;
    LPBYTE RawData;
    VOID Write();
} IMGB_TLS_RAW_DATA_HEADER, *PIMGB_TLS_RAW_DATA_HEADER;

typedef struct _IMGB_XBEIMAGE_LIBRARY_VERSION_HEADER: public IMGB_GENERIC_HEADER {
    PXBEIMAGE_LIBRARY_VERSION LibraryVersions;
    ULONG XboxKernelOffset;
    ULONG XapiOffset;
    VOID Write();
} IMGB_XBEIMAGE_LIBRARY_VERSION_HEADER, *PIMGB_XBEIMAGE_LIBRARY_VERSION_HEADER;

typedef struct _IMGB_XBEIMAGE_SECTION_HEADERS: public IMGB_GENERIC_HEADER {
    ULONG NumberOfExecutableSections;
    ULONG NumberOfInsertFileSections;
    ULONG SizeOfSectionNames;
    ULONG NumberOfSharedPageReferenceCounts;
    VOID Write();
} IMGB_XBEIMAGE_SECTION_HEADERS, *PIMGB_XBEIMAGE_SECTION_HEADERS;

typedef struct _IMGB_DEBUG_PATHS_HEADER: public IMGB_GENERIC_HEADER {
    VOID Write();
} IMGB_DEBUG_PATHS_HEADER, *PIMGB_DEBUG_PATHS_HEADER;

typedef struct _IMGB_MICROSOFT_LOGO_HEADER: public IMGB_GENERIC_HEADER {
    VOID Write();
} IMGB_MICROSOFT_LOGO_HEADER, *PIMGB_MICROSOFT_LOGO_HEADER;

typedef struct _IMGB_PEHEADER_HEADER: public IMGB_GENERIC_HEADER {
    VOID Write();
} IMGB_PEHEADER_HEADER, *PIMGB_PEHEADER_HEADER;

//
// Define the structure to track all of the contributions to an XBE image.
//

typedef struct _IMGB_XBEIMAGE_HEADER {
    LIST_ENTRY HeadersListHead;
    XBEIMAGE_HEADER ImageHeader;
    IMGB_XBEIMAGE_CERTIFICATE_HEADER CertificateHeader;
    IMGB_XBEIMAGE_IMPORT_DESCRIPTOR_HEADER ImportDescriptorHeader;
    IMGB_TLS_RAW_DATA_HEADER TlsRawDataHeader;
    IMGB_XBEIMAGE_LIBRARY_VERSION_HEADER LibraryVersionHeader;
    IMGB_XBEIMAGE_SECTION_HEADERS SectionHeaders;
    IMGB_DEBUG_PATHS_HEADER DebugPathsHeader;
    IMGB_MICROSOFT_LOGO_HEADER MicrosoftLogoHeader;
    IMGB_PEHEADER_HEADER PEHeaderHeader;
    ULONG NewBaseAddress;
    ULONG SizeOfExecutableImage;
    ULONG SizeOfInsertFilesImage;
} IMGB_XBEIMAGE_HEADER, *PIMGB_XBEIMAGE_HEADER;

//
// Sub-utility programs.
//

DECLSPEC_NORETURN
VOID
ImgbDumpExecutable(
    int argc,
    char *argv[]
    );

//
// General support functions.
//

DECLSPEC_NORETURN
VOID
ImgbExitProcess(
    int ExitCode
    );

LPVOID
ImgbAllocateMemory(
    SIZE_T cbBytes
    );

VOID
ImgbFreeMemory(
    LPVOID lpMemory
    );

//
// Command line support functions.
//

VOID
ImgbProcessCommandLineOptions(
    int argc,
    char *argv[]
    );

BOOLEAN
ImgbSearchNoPreloadList(
    LPCSTR pszSectionName
    );

//
// Portable Executable (PE) module support functions.
//

PIMAGE_SECTION_HEADER
ImgbNameToSectionHeader(
    PIMAGE_NT_HEADERS NtHeader,
    PUCHAR SearchName,
    PULONG SectionIndex
    );

PIMAGE_SECTION_HEADER
ImgbVirtualAddressToSectionHeader(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG VirtualAddress
    );

LPVOID
ImgbVirtualAddressToData(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG VirtualAddress
    );

LPVOID
ImgbImageDataDirectoryToData(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG DataDirectoryIndex
    );

LPVOID
ImgbLoadAddressToData(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG LoadAddress
    );

VOID
ImgbRelocateImage(
    PIMAGE_NT_HEADERS NtHeader,
    ULONG OldBaseAddress,
    ULONG NewBaseAddress,
    PIMAGE_BASE_RELOCATION NextBlock,
    ULONG TotalCountBytes
    );

//
// String resource support functions.
//

VOID
ImgbResourcePrintfV(
    FILE *file,
    UINT uStringID,
    va_list args
    );

VOID
ImgbResourcePrintf(
    FILE *file,
    UINT uStringID,
    ...
    );

VOID
ImgbResourcePrintErrorAndExit(
    UINT uStringID,
    ...
    );

VOID
ImgbResourcePrintWarning(
    UINT uStringID,
    ...
    );

VOID
ImgbResourcePrintRange(
    FILE *file,
    UINT uStartingStringID
    );

VOID
ImgbResourcePrintLogoBanner(
    VOID
    );

//
// C++ default operator overrides.
//

__inline LPVOID __cdecl operator new(size_t cbBytes)
{
    return ImgbAllocateMemory(cbBytes);
}

__inline VOID __cdecl operator delete(LPVOID lpMemory)
{
    ImgbFreeMemory(lpMemory);
}

//
// External symbols.
//

extern LPSTR ImgbInputFilePath;
extern LPSTR ImgbOutputFilePath;
extern LIST_ENTRY ImgbNoPreloadList;
extern LIST_ENTRY ImgbInsertFileList;
extern BOOLEAN ImgbEmitPEHeader;
extern ULONG ImgbSizeOfStack;
extern ULONG ImgbInitFlags;
extern ULONG ImgbVersion;
extern ULONG ImgbTestGameRegion;
extern ULONG ImgbTestAllowedMediaTypes;
extern ULONG ImgbTestGameRatings;
extern ULONG ImgbTestTitleID;
extern ULONG ImgbTestAlternateTitleIDs[XBEIMAGE_ALTERNATE_TITLE_ID_COUNT];
extern XBEIMAGE_CERTIFICATE_KEY ImgbTestAlternateSignatureKeys[XBEIMAGE_ALTERNATE_TITLE_ID_COUNT];
extern WCHAR ImgbTestTitleName[XBEIMAGE_TITLE_NAME_LENGTH];
extern UCHAR ImgbTestLANKey[XBEIMAGE_CERTIFICATE_KEY_LENGTH];
extern UCHAR ImgbTestSignatureKey[XBEIMAGE_CERTIFICATE_KEY_LENGTH];
extern HANDLE ImgbInputFileHandle;
extern ULONG ImgbInputFileSize;
extern HANDLE ImgbInputFileMappingHandle;
extern LPVOID ImgbInputFileMappingView;
extern UCHAR ImgbPrivateKeyData[];
extern UCHAR ImgbPublicKeyData[];
extern BOOLEAN ImgbNoWarnLibraryApproval;

#endif // IMGBLDP
