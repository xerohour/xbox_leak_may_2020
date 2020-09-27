/*++

Copyright (c) 2000-2002  Microsoft Corporation

Module Name:

    xbeimage.h

Abstract:

    This module contains the structures that describe the Xbox Executable (XBE)
    format.

--*/

#ifndef _XBEIMAGE_
#define _XBEIMAGE_

#include <crypto\xcrypt.h>

#include <pshpack1.h>

//
// Define the signature bytes for the XBE image header.  This corresponds to the
// string "XBEH".
//

#define XBEIMAGE_SIGNATURE                      0x48454258

//
// Define the standard base address for an XBE image.
//

#define XBEIMAGE_STANDARD_BASE_ADDRESS          0x00010000

//
// Define the entry point signature for an XBE image.
//

typedef
VOID
(__cdecl *PXBEIMAGE_ENTRY_POINT)(
    VOID
    );

//
// Define the maximum length in characters of a title name in the XBE image
// certificate.
//

#define XBEIMAGE_TITLE_NAME_LENGTH              40

//
// Define the number of alternate title identifiers that can be stored in the
// XBE image certificate.
//

#define XBEIMAGE_ALTERNATE_TITLE_ID_COUNT       16

//
// Define the length of the keys stored in the XBE image certificate.
//

#define XBEIMAGE_CERTIFICATE_KEY_LENGTH         16

//
// Define the structure of the XBE certificate raw data key.
//

typedef UCHAR XBEIMAGE_CERTIFICATE_KEY[XBEIMAGE_CERTIFICATE_KEY_LENGTH];

//
// Define the game region flags for the XBE image certificate.
//

#define XBEIMAGE_GAME_REGION_NA                 0x00000001
#define XBEIMAGE_GAME_REGION_JAPAN              0x00000002
#define XBEIMAGE_GAME_REGION_RESTOFWORLD        0x00000004
#define XBEIMAGE_GAME_REGION_MANUFACTURING      0x80000000

//
// Define the media type flags for the XBE image certificate.
//

#define XBEIMAGE_MEDIA_TYPE_HARD_DISK           0x00000001
#define XBEIMAGE_MEDIA_TYPE_DVD_X2              0x00000002
#define XBEIMAGE_MEDIA_TYPE_DVD_CD              0x00000004
#define XBEIMAGE_MEDIA_TYPE_CD                  0x00000008
#define XBEIMAGE_MEDIA_TYPE_DVD_5_RO            0x00000010
#define XBEIMAGE_MEDIA_TYPE_DVD_9_RO            0x00000020
#define XBEIMAGE_MEDIA_TYPE_DVD_5_RW            0x00000040
#define XBEIMAGE_MEDIA_TYPE_DVD_9_RW            0x00000080
#define XBEIMAGE_MEDIA_TYPE_DONGLE              0x00000100
#define XBEIMAGE_MEDIA_TYPE_MEDIA_BOARD         0x00000200
#define XBEIMAGE_MEDIA_TYPE_NONSECURE_HARD_DISK 0x40000000
#define XBEIMAGE_MEDIA_TYPE_NONSECURE_MODE      0x80000000
#define XBEIMAGE_MEDIA_TYPE_MEDIA_MASK          0x00FFFFFF

//
// Define the structure of the XBE certificate.
//

typedef struct _XBEIMAGE_CERTIFICATE {

    //
    // Specifies the number of bytes stored in this certificate structure.
    //
    ULONG SizeOfCertificate;

    //
    // Specifies the time that the certificate was last modified expressed as
    // the number of seconds elapsed since midnight (00:00:000), January 1,
    // 1970, coordinated universal time.
    //
    ULONG TimeDateStamp;

    //
    // Specifies the identifier of the title that this XBE image belongs to.
    //
    ULONG TitleID;

    //
    // Specifies the null-terminated name of the title.
    //
    WCHAR TitleName[XBEIMAGE_TITLE_NAME_LENGTH];

    //
    // Specifies the identifiers of other titles that this XBE image is allowed
    // to access via the XTL storage APIs.
    //
    ULONG AlternateTitleIDs[XBEIMAGE_ALTERNATE_TITLE_ID_COUNT];

    //
    // Specifies the types of media that the XBE image is allowed to run off of.
    //
    ULONG AllowedMediaTypes;

    //
    // Specifies the regions that this XBE image is allowed to run in.
    //
    ULONG GameRegion;

    //
    // Specifies the rating of this XBE image.
    //
    ULONG GameRatings;

    //
    // Specifies the zero-based disk number of the title for a multi-disc game.
    //
    ULONG DiskNumber;

    //
    // Specifies the version of the title as tracked by the certification
    // process.
    //
    ULONG Version;

    //
    // Specifies the raw data used for generating the LAN key.  The LAN key is a
    // shared key used for network play.
    //
    XBEIMAGE_CERTIFICATE_KEY LANKey;

    //
    // Specifies the raw data used for generating the signature key.  The
    // signature key is used for signing the title's saved games.
    //
    XBEIMAGE_CERTIFICATE_KEY SignatureKey;

    //
    // Specifies the signature keys corresponding to the alternate title
    // identifiers.
    //
    XBEIMAGE_CERTIFICATE_KEY AlternateSignatureKeys[XBEIMAGE_ALTERNATE_TITLE_ID_COUNT];

    //
    // Specifies the original size of this certificate when the image was
    // first built
    //
    ULONG OriginalSizeOfCertificate;

    //
    // Specifies the online service to contact for online games.
    //
    ULONG OnlineServiceName;

} XBEIMAGE_CERTIFICATE, *PXBEIMAGE_CERTIFICATE;

//
// Define the size of the certificate structure in the first release of the XBE
// loader.
//

#define XBEIMAGE_CERTIFICATE_BASE_SIZEOF \
    (FIELD_OFFSET(XBEIMAGE_CERTIFICATE, AlternateSignatureKeys) + \
    (sizeof(XBEIMAGE_CERTIFICATE_KEY) * XBEIMAGE_ALTERNATE_TITLE_ID_COUNT))

//
// Define the structure of the XBE import directory.  A directory entry exists
// for each image that the XBE image imports from.
//

typedef struct _XBEIMAGE_IMPORT_DESCRIPTOR {

    //
    // Specifies the pointer to the array of import thunks.  All import thunks
    // are specified by ordinal.
    //
    PIMAGE_THUNK_DATA ImageThunkData;

    //
    // Specifies the null-terminated name of the image to import from.
    //
    PWCHAR ImageName;

} XBEIMAGE_IMPORT_DESCRIPTOR, *PXBEIMAGE_IMPORT_DESCRIPTOR;

//
// Defines the flags that apply to XBE image sections.
//

#define XBEIMAGE_SECTION_WRITEABLE              0x00000001
#define XBEIMAGE_SECTION_PRELOAD                0x00000002
#define XBEIMAGE_SECTION_EXECUTABLE             0x00000004
#define XBEIMAGE_SECTION_INSERTFILE             0x00000008
#define XBEIMAGE_SECTION_HEAD_PAGE_READONLY     0x00000010
#define XBEIMAGE_SECTION_TAIL_PAGE_READONLY     0x00000020

//
// Define the structure of the XBE image section.
//

typedef struct _XBEIMAGE_SECTION {

    //
    // Specifies attributes about the section.
    //
    ULONG SectionFlags;

    //
    // Specifies the base virtual address of the section.
    //
    ULONG VirtualAddress;

    //
    // Specifies the number of bytes this section occupies in memory.
    //
    ULONG VirtualSize;

    //
    // Specifies the starting file byte offset of the section.
    //
    ULONG PointerToRawData;

    //
    // Specifies the number of bytes that the section occupies in the file.  The
    // difference between the virtual size and this field is filled with zeroes.
    //
    ULONG SizeOfRawData;

    //
    // Specifies the name of the section.
    //
    PUCHAR SectionName;

    //
    // Specifies at run time, the number of times that this section has been
    // loaded.  A section is unloaded when the reference count is decremented to
    // zero.
    //
    ULONG SectionReferenceCount;

    //
    // Specifies pointers to integers used to track the shared page reference
    // count for the head and tail portions of the section.  A page is
    // decommitted when the shared reference count is decremented to zero.
    //
    PUSHORT HeadSharedPageReferenceCount;
    PUSHORT TailSharedPageReferenceCount;

    //
    // Specifies the SHA1 digest of the section.
    //
    UCHAR SectionDigest[XC_DIGEST_LEN];

} XBEIMAGE_SECTION, *PXBEIMAGE_SECTION;

//
// Define the length of the library name stored in a library version structure.
//

#define XBEIMAGE_LIBRARY_VERSION_NAME_LENGTH    8

//
// Define the structure of a XBE library version descriptor.
//

typedef struct _XBEIMAGE_LIBRARY_VERSION {

    //
    // Specifies ASCII text padded with nulls to identify the library.
    //
    UCHAR LibraryName[XBEIMAGE_LIBRARY_VERSION_NAME_LENGTH];

    //
    // Specifies the major version number of the library.
    //
    USHORT MajorVersion;

    //
    // Specifies the minor version number of the library.
    //
    USHORT MinorVersion;

    //
    // Specifies the build version number of the library.
    //
    USHORT BuildVersion;

    //
    // Specifies the QFE version number of the library.
    //
    USHORT QFEVersion : 13;

    //
    // Specifies the approved status of this library
    // 0 = unapproved
    // 1 = conditionally approved
    // 2 = approved
    USHORT ApprovedLibrary : 2;

    //
    // Specifies TRUE if this is a debug build of the library, else FALSE.
    //
    USHORT DebugBuild : 1;

} XBEIMAGE_LIBRARY_VERSION, *PXBEIMAGE_LIBRARY_VERSION;

//
// Define the structure of the XBE image header.  All XBE images contain this
// structure at file byte offset zero.
//

typedef struct _XBEIMAGE_HEADER {

    //
    // Contains XBEIMAGE_SIGNATURE.
    //
    ULONG Signature;

    //
    // Specifies the encrypted digest of the image headers starting at the
    // field following this field.
    //
    UCHAR EncryptedDigest[XC_ENC_SIGNATURE_SIZE];

    //
    // Specifies the base address for the image, which is the pointer to the
    // XBEIMAGE_HEADER structure.
    //
    PVOID BaseAddress;

    //
    // Specifies the number of bytes that are used to store the image headers.
    //
    ULONG SizeOfHeaders;

    //
    // Specifies the number of bytes required to map the entire image.
    //
    ULONG SizeOfImage;

    //
    // Specifies the number of bytes stored in this image header structure.
    //
    ULONG SizeOfImageHeader;

    //
    // Specifies the time that the image was created expressed as the number of
    // seconds elapsed since midnight (00:00:000), January 1, 1970, coordinated
    // universal time.
    //
    ULONG TimeDateStamp;

    //
    // Specifies the virtual address of the XBE certificate.
    //
    PXBEIMAGE_CERTIFICATE Certificate;

    //
    // Specifies the number of sections that are contained in the image.
    //
    ULONG NumberOfSections;

    //
    // Specifies the virtual address of the XBE image section headers.
    //
    PXBEIMAGE_SECTION SectionHeaders;

    //
    // Specifies initialization flags to control the behavior of the XBE image.
    // See the XINIT_* flags.
    //
    ULONG InitFlags;

    //
    // Specifies the virtual address of the entry point of the XBE image.
    //
    PXBEIMAGE_ENTRY_POINT AddressOfEntryPoint;

    //
    // Specifies the virtual address of the IMAGE_TLS_DIRECTORY structure.  The
    // virtual address may be NULL if the image does not require any thread
    // local storage.
    //
    PIMAGE_TLS_DIRECTORY TlsDirectory;

    //
    // Specifies the default number of bytes to allocate for a stack for a
    // thread created by CreateThread.
    //
    ULONG SizeOfStackCommit;

    //
    // Specifies the number of bytes to reserve and commit for the default
    // process heap.
    //
    ULONG SizeOfHeapReserve;
    ULONG SizeOfHeapCommit;

    //
    // Specifies the pointer to the portable executable (PE) header of the
    // original file.  This pointer may be NULL if the image was not built with
    // the /PEHEADER switch specified.
    //
    PVOID NtBaseOfDll;

    //
    // Contains values from the IMAGE_NT_HEADERS of the original portable
    // executable (PE) file that was used to build this XBE image.  Used for
    // debugging purposes.
    //
    ULONG NtSizeOfImage;
    ULONG NtCheckSum;
    ULONG NtTimeDateStamp;

    //
    // Specifies the null-terminated full path to the original portable
    // executable (PE) file that was used to build this XBE image.  Used for
    // debugging purposes.
    //
    PUCHAR DebugPathName;

    //
    // Specifies the null-terminated file name of the original portable
    // executable (PE) file that was used to build this XBE image.  Used for
    // debugging purposes.
    //
    PUCHAR DebugFileName;
    PWCHAR DebugUnicodeFileName;

    //
    // Specifies the virtual address of the array of import thunks from the
    // Xbox kernel.  All import thunks must be by ordinal.  The array is
    // terminated by a NULL import thunk.
    //
    PIMAGE_THUNK_DATA XboxKernelThunkData;

    //
    // Specifies the virtual address of the array of import directories for
    // image imports other than those from the Xbox kernel.  The array is
    // terminated by an entry with a NULL ImageThunkData field.  This virtual
    // address may be NULL if no additional imports are required to run the
    // image.
    //
    // These import directories are only processed on a developer kit version of
    // the kernel.  For the retail version of the kernel, this field must be
    // NULL.
    //
    PXBEIMAGE_IMPORT_DESCRIPTOR ImportDirectory;

    //
    // Specifies the number of library versions that are contained in the image.
    //
    ULONG NumberOfLibraryVersions;

    //
    // Specifies the virtual address of the array of library version
    // descriptors.
    //
    PXBEIMAGE_LIBRARY_VERSION LibraryVersions;

    //
    // Specifies the virtual addresses of the XBOXKRNL and XAPI library version
    // descriptors.  These should be pointers within the above LibraryVersions
    // section.  These are maintained as separate pointers to avoid having
    // search code in the loader for these library version descriptors.  These
    // virtual addresses may be NULL if the image was not linked against these
    // libraries.
    //
    PXBEIMAGE_LIBRARY_VERSION XboxKernelLibraryVersion;
    PXBEIMAGE_LIBRARY_VERSION XapiLibraryVersion;

    //
    // Specifies the virtual address of the Microsoft logo bitmap used for
    // copyright purposes and the number of bytes used by the logo.
    //
    PVOID MicrosoftLogo;
    ULONG SizeOfMicrosoftLogo;

} XBEIMAGE_HEADER, *PXBEIMAGE_HEADER;

//
// Define the size of the image header structure in the first release of the XBE
// loader.
//

#define XBEIMAGE_HEADER_BASE_SIZEOF \
    (FIELD_OFFSET(XBEIMAGE_HEADER, SizeOfMicrosoftLogo) + sizeof(ULONG))

//
// Define the initialization flags stored in the image header.
//

#define XINIT_MOUNT_UTILITY_DRIVE               0x00000001
#define XINIT_FORMAT_UTILITY_DRIVE              0x00000002
#define XINIT_LIMIT_DEVKIT_MEMORY               0x00000004
#define XINIT_NO_SETUP_HARD_DISK                0x00000008
#define XINIT_DONT_MODIFY_HARD_DISK             0x00000010
#define XINIT_UTILITY_DRIVE_CLUSTER_SIZE_MASK   0xC0000000

#define XINIT_UTILITY_DRIVE_CLUSTER_SIZE_SHIFT  30
#define XINIT_UTILITY_DRIVE_16K_CLUSTER_SIZE    0x00000000
#define XINIT_UTILITY_DRIVE_32K_CLUSTER_SIZE    0x40000000
#define XINIT_UTILITY_DRIVE_64K_CLUSTER_SIZE    0x80000000
#define XINIT_UTILITY_DRIVE_128K_CLUSTER_SIZE   0xC0000000

#include <poppack.h>

#endif // XBEIMAGE
