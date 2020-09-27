/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xetest.cpp

Abstract:

    This program contains routines to test XE loader. Security test variations
    are available only under devkit as the image file needs to be modified
    during run-time.

--*/

#include "xetestp.h"

//
//
//
#define XIMG_LDR_IMAGE          (XapiGetCurrentProcess()->XeLdrImage)
#define XIMG_HEADER             (XapiGetCurrentProcess()->XeLdrImage->ImageHeader)
#define XIMG_VARHEADER          (XapiGetCurrentProcess()->XeLdrImage->VarImageHeader)
#define XIMG_NUM_SECTIONS       (XapiGetCurrentProcess()->XeLdrImage->ImageHeader-> \
                                    OptionalHeader.NumberOfSections)
#define XIMG_FIRST_SECTION      (XapiGetCurrentProcess()->XeLdrImage-> \
                                    FirstSectionHeader)

#define SECTION_ID_TO_NAME(id)  XeTestSectionName[id]
#define SECTION_ID_TO_INDEX(id) ((id) + XeTestSectionIdToSectionIndexBias)

CHAR XeTestSectionName[MAX_SECTIONS][9];
ULONG XeTestSectionIdToSectionIndexBias;

#pragma code_seg( "xetest1" )

PVOID
XeTestAllocateAllMemory(
    VOID
    )
/*++

Routine Description:

    Utility routine used to allocate all available memory returned by
    GlobalMemoryStatus. It puts system under low memory situation.

Arguments:

    None

Return Value:

    Pointer to allocated memory

--*/
{
    MEMORYSTATUS ms;
    PVOID BigChunk = NULL;
    LONG MemSize;

    GlobalMemoryStatus( &ms );
    MemSize = ms.dwAvailPhys;

    do {
        MemSize -= PAGE_SIZE;
        BigChunk = VirtualAlloc( 0, MemSize, MEM_COMMIT, PAGE_READWRITE );
    } while ( BigChunk == NULL && MemSize > 0 );

    ASSERT( BigChunk );

    return BigChunk;
}

#pragma code_seg( "xetest2" )

VOID
XeTestFreeAllMemory(
    IN PVOID P
    )
/*++

Routine Description:

    This routine free memory allocated by XeTestAllocateAllMemory

Arguments:

    P - Pointer returned from XeTestAllocateAllMemory

Return Value:

    None

--*/
{
    ASSERT( P );

    if ( P ) {
        VirtualFree( P, 0, MEM_RELEASE );
    }
}

#pragma code_seg( "xetest3" )

PXIMAGE_SECTION_HEADER
XeTestGetSectionHeader(
    ULONG SectionID
    )
/*++

Routine Description:

    This routine returns section header of ID specified by SectionID.

Arguments:

    SectionID - Zero-based section ID

Return Value:

    Pointer to section header

--*/
{
    ULONG SectionIndex = SECTION_ID_TO_INDEX(SectionID);

    ASSERT( SectionID < MAX_SECTIONS );
    ASSERT( SectionIndex < XIMG_NUM_SECTIONS );

    return XIMG_FIRST_SECTION + SectionIndex;
}

#pragma code_seg( "xetest4" )

BOOL
XeTestInitialize(
    VOID
    )
/*++

Routine Description:

    Initialize section name cache (XeTestectionName) and section header cache
    (XeTestSectionHeader) for faster access

Arguments:

    None

Return Value:

    TRUE if success, otherwise FALSE

--*/
{
    SIZE_T Section, SectionID;
    PXIMAGE_SECTION_HEADER SectionHeader;

    //
    // Look for index of section ID=0 (SECTN000)
    //

    SectionHeader = XIMG_FIRST_SECTION;
    for ( Section=0; Section<XIMG_NUM_SECTIONS; Section++ ) {
        if ( !strncmp(SECTION_NAME_PREFIX "000", (char*)SectionHeader->Name, \
                      XIMAGE_SECTION_NAME_MAXLEN) ) {
            XeTestSectionIdToSectionIndexBias = Section;
            break;
        }
        SectionHeader++;
    }

    if ( XeTestSectionIdToSectionIndexBias >= XIMG_NUM_SECTIONS ) {
        return FALSE;
    }

    for ( SectionID=0; SectionID<MAX_SECTIONS; SectionID++ ) {
        wsprintfA( XeTestSectionName[SectionID], "%s%03lu",
                   SECTION_NAME_PREFIX, SectionID );

        if ( (XeTestGetSectionHeader(SectionID)->SectionFlags & \
              XIMAGE_SECFLAG_IMPLICIT) != 0) {

            //
            // Verify that we have all required sections non-preloaded
            //

            DbgPrint( "XETEST: Section %.*s is not a non-preloaded\n",
                      XIMAGE_SECTION_NAME_MAXLEN,
                      XeTestSectionName[SectionID] );

            return FALSE;
        }
    }

    return TRUE;
}

#pragma code_seg( "xetest5" )

BOOL
VerifySectionRefCount(
    IN USHORT RefCountValue,
    IN ULONG SectionID
    )
/*++

Routine Description:

    This routine verifies referent count value of section specified by
    SectionID.

Arguments:

    RefCountValue - Value of referent cound to be verified

    SectionID - Zero-based section ID

Return Value:

    TRUE if referent count is correct, FALSE otherwise

--*/
{
    ULONG SectionIndex = SECTION_ID_TO_INDEX(SectionID);
    PUSHORT SectionRefCount;

    ASSERT( SectionID < MAX_SECTIONS );
    ASSERT( SectionIndex < XIMG_NUM_SECTIONS );

    SectionRefCount = (PUSHORT)(XapiGetCurrentProcess()-> \
                                XeLdrImage->LoadedSections + SectionIndex );

#if DBG
    if ( *SectionRefCount != RefCountValue ) {
        KdPrint(( "XETEST: %.*s%03u has refcount=%u, expected %u\n",
                  XIMAGE_SECTION_NAME_MAXLEN, *SectionRefCount,
                  RefCountValue ));
    }
#endif

    return *SectionRefCount == RefCountValue;
}

#pragma code_seg( "xetest6" )

BOOL
VerifyAllSectionsRefCount(
    IN USHORT RefCountValue
    )
/*++

Routine Description:

    This routine verifies referent count value of all section from ID=0 to
    ID=MAX_SECTIONS-1

Arguments:

    RefCountValue - Value of referent cound to be verified

Return Value:

    TRUE if all referent counts are correct, FALSE otherwise

--*/
{
    SIZE_T SectionID;

    for ( SectionID=0; SectionID<MAX_SECTIONS; SectionID++ ) {
        if ( !VerifySectionRefCount(RefCountValue, SectionID) ) {
            return FALSE;
        }
    }

    return TRUE;
}

#pragma code_seg( "xetest7" )

VOID
FileOffsetFromSectionID(
    IN  ULONG SectionID,
    OUT PULONG HeadFileOffset,
    OUT PULONG BodyFileOffset,
    OUT PULONG TailFileOffset
    )
/*++

Routine Description:

    This routine calculates file offset starting from beginning of the image
    file from SectionID.

Arguments:

    SectionID - Zero-based section ID

    HeadFileOffset - Output file offset of section head in image file

    BodyFileOffset - Output file offset of section body in image file

    TailFileOffset - Output file offset of section tail in image file

Return Value:

    None

--*/
{
    PXIMAGE_SECTION_HEADER SectionHeader;
    SIZE_T NumberOfBodyPages, HeadSize, PrevPageTailSize;

    SectionHeader = XeTestGetSectionHeader( SectionID );

    PrevPageTailSize = SectionHeader->VirtualAddress % LDR_PAGE_SIZE;

    //
    // Calculate offset in file for head page
    //

    *HeadFileOffset = (XIMG_HEADER->PagesInHeaders + SectionHeader->HeadPage) * \
        LDR_PAGE_SIZE + PrevPageTailSize;

    //
    // Calculate offset in file for body page if any
    //

    HeadSize = LDR_PAGE_SIZE - PrevPageTailSize;

    if ( SectionHeader->VirtualAddress <= HeadSize ) {
        NumberOfBodyPages = 0;
    } else {
        NumberOfBodyPages = (SectionHeader->VirtualSize - HeadSize) / LDR_PAGE_SIZE;
    }

    //
    // If there is no body page, offset of body page is offset of head page
    //

    if ( NumberOfBodyPages ) {
        *BodyFileOffset = (XIMG_HEADER->PagesInHeaders + SectionHeader->BodyPage) * \
            LDR_PAGE_SIZE + PrevPageTailSize;
    } else {
        *BodyFileOffset = *HeadFileOffset;
    }

    //
    // Calculate offset in file for tail page
    //

    if ( SectionHeader->TailPage != SectionHeader->HeadPage ) {
        *TailFileOffset = (XIMG_HEADER->PagesInHeaders + SectionHeader->TailPage) * \
            LDR_PAGE_SIZE + PrevPageTailSize;
    } else {
        *TailFileOffset = *BodyFileOffset;
    }
}

#pragma code_seg( "xetest8" )

NTSTATUS
XeTestReadImageFile(
    IN  SIZE_T FileOffset,
    OUT PVOID  Buffer,
    IN  SIZE_T BufferLength
    )
/*++

Routine Description:

    Utility routine to open image file and write data from specify Buffer to
    file starting from file offset specify in FileOffset

Arguments:

    FileOffset - Specify file offset to write from the begin of the file

    Buffer - Pointer to data buffer to write

    BufferLength - Number of bytes to write

Return Value:

    NT status code of the operation

--*/
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjA;
    IO_STATUS_BLOCK IoStatusBlock;
    HANDLE FileHandle;
    LARGE_INTEGER ReadOffset;

    InitializeObjectAttributes( &ObjA, &XIMG_LDR_IMAGE->ImageFileName,
                                OBJ_CASE_INSENSITIVE, 0, 0 );

    Status = NtOpenFile( &FileHandle, FILE_GENERIC_READ, &ObjA, &IoStatusBlock,
                         FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT | \
                         FILE_RANDOM_ACCESS );

    if ( !NT_SUCCESS(Status) ) {
        return Status;
    }

    ReadOffset.HighPart = 0;
    ReadOffset.LowPart = FileOffset;

    Status = NtReadFile( FileHandle, 0, 0, 0, &IoStatusBlock, Buffer,
                         BufferLength, &ReadOffset );

    NtClose( FileHandle );

    return Status;
}

#pragma code_seg( "xetest9" )

NTSTATUS
XeTestWriteImageFile(
    IN SIZE_T FileOffset,
    IN PVOID  Buffer,
    IN SIZE_T BufferLength
    )
/*++

Routine Description:

    Utility routine to open image file and write data from specify Buffer to
    file starting from file offset specify in FileOffset

Arguments:

    FileOffset - Specify file offset to write from the begin of the file

    Buffer - Pointer to data buffer to write

    BufferLength - Number of bytes to write

Return Value:

    NT status code of the operation

--*/
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjA;
    IO_STATUS_BLOCK IoStatusBlock;
    HANDLE FileHandle;
    LARGE_INTEGER WriteOffset;

    InitializeObjectAttributes( &ObjA, &XIMG_LDR_IMAGE->ImageFileName,
                                OBJ_CASE_INSENSITIVE, 0, 0 );

    Status = NtOpenFile( &FileHandle, FILE_GENERIC_WRITE, &ObjA, &IoStatusBlock,
                         FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT | \
                         FILE_RANDOM_ACCESS );

    if ( !NT_SUCCESS(Status) ) {
        return Status;
    }

    WriteOffset.HighPart = 0;
    WriteOffset.LowPart = FileOffset;

    Status = NtWriteFile( FileHandle, 0, 0, 0, &IoStatusBlock, Buffer,
                          BufferLength, &WriteOffset );

    NtClose( FileHandle );

    return Status;
}

#pragma code_seg( "xetest10" )

BOOL
XeTestReadWriteSection(
    IN BOOL   ReadOperation,
    IN ULONG  SectionID,
    IN PVOID  Buffer,
    IN SIZE_T BufferLength
    )
/*++

Routine Description:

    Utility routine to read/write data to/from body page of section specified by
    SectionID

Arguments:

    ReadOperaion - Non-zero for read operation and vice versa

    SectionID - Id of the section

    Buffer - Pointer to data to write

    BufferLength - Size in bytes of the buffer

Return Value:

    TRUE if the operation success, otherwise FALSE

--*/
{
    NTSTATUS Status;
    ULONG HeadFileOffset, BodyFileOffset, TailFileOffset;

    FileOffsetFromSectionID( SectionID, &HeadFileOffset, &BodyFileOffset,
                             &TailFileOffset );

    if ( ReadOperation ) {
        Status = XeTestReadImageFile( BodyFileOffset, Buffer, BufferLength );
    } else {
        Status = XeTestWriteImageFile( BodyFileOffset, Buffer, BufferLength );
    }

    SetLastError( RtlNtStatusToDosError(Status) );

    return NT_SUCCESS( Status );
}

#pragma code_seg( "xetest11" )

VOID
BasicSectionLoadUnloadTest(
    VOID
    )
/*++

Routine Description:

    Test basic section loading/unloading under normal and low memory condition

Arguments:

    None

Return Value:

    None

--*/
{
    SIZE_T i;
    PVOID p;

    KdPrint(( "XETEST: basic load/unload under normal and low memory\n" ));

    //
    // All sections ref count should be zero
    //

    VerifyAllSectionsRefCount( 0 );

    //
    // Basic loading of non-preload section should work fine
    //

    for ( i=0; i<MAX_SECTIONS; i++ ) {
        if ( XLoadSection(SECTION_ID_TO_NAME(i)) == NULL ) {
            KdPrint(( "XETEST: XLoadSection[%lu] failed unexpectedly (0x%x)\n",
                      __LINE__, GetLastError() ));
        } else {
            XFreeSection( SECTION_ID_TO_NAME(i) );
        }
    }

    VerifyAllSectionsRefCount( 0 );

    //
    // Put the system into low memory situation and try to load sections
    //

    p = XeTestAllocateAllMemory();

    for ( i=0; i<MAX_SECTIONS; i++ ) {
        if ( XLoadSection(SECTION_ID_TO_NAME(i)) != NULL ) {
            KdPrint(( "XETEST: XLoadSection[%lu](%s) success unexpectly\n",
                      __LINE__, SECTION_ID_TO_NAME(i) ));
            XFreeSection( SECTION_ID_TO_NAME(i) );
        }
    }

    VerifyAllSectionsRefCount( 0 );

    //
    // Free back all the memory and try to load sections again
    //

    XeTestFreeAllMemory( p );

    for ( i=0; i<MAX_SECTIONS; i++ ) {
        if ( XLoadSection(SECTION_ID_TO_NAME(i)) == NULL ) {
            KdPrint(( "XETEST: XLoadSection[%lu] failed unexpectedly (0x%x)\n",
                      __LINE__, GetLastError() ));
        } else {
            XFreeSection( SECTION_ID_TO_NAME(i) );
        }
    }

    VerifyAllSectionsRefCount( 0 );
}

#pragma code_seg( "xetest12" )

VOID
TamperedSectionTest(
    VOID
    )
/*++

Routine Description

    This routine modifies image file on disk and tries to load/unload tampered
    section.

Arguments:

    None

Return Value:

    None

--*/
{
    PVOID SectionPtr;
    BYTE SaveByte;
    ULONG id;

    KdPrint(( "XETEST: tampered section loading\n" ));

    for ( id=0; id<MAX_SECTIONS; id++ ) {

        //
        // Verify reference count
        //

        VerifySectionRefCount( 0, id );

        //
        // Save the original content of the image
        //

        if ( !XeTestReadWriteSection(TRUE, id, &SaveByte, 1) ) {
            KdPrint(( "XETEST: unable to read section from file (0x%x)\n",
                      GetLastError() ));
            return;
        }

        //
        // Write new byte value to the body page
        //

        SaveByte = (BYTE)~SaveByte;

        if ( !XeTestReadWriteSection(FALSE, id, &SaveByte, 1) ) {
            KdPrint(( "XETEST: write image section failed (0x%x)\n", GetLastError() ));
            return;
        }

        //
        // Try to load the tampered section now
        //

        SectionPtr = XLoadSection( SECTION_ID_TO_NAME(id) );

        if ( SectionPtr ) {
            KdPrint(( "XETEST: tampered section was unexpectedly loaded!\n" ));
            XFreeSection( SECTION_ID_TO_NAME(id) );
        }

        //
        // Verify that there is no page actually loaded or allocated
        //

        if ( !IsBadCodePtr((PROC)SectionPtr) ) {
            KdPrint(( "XETEST: page unexpectedly allocated for tampered section" ));
        }

        //
        // Verify reference count of this section again
        //

        VerifySectionRefCount( 0, id );

        //
        // Restore original image content that we modified
        //

        SaveByte = (BYTE)~SaveByte;

        if ( !XeTestReadWriteSection(FALSE, id, &SaveByte, 1) ) {
            KdPrint(( "XETEST: restore image section failed (0x%x)\n", GetLastError() ));
        }
    }
}

#pragma code_seg( "xetest13" )

int __cdecl main( void )
{
    if ( !XeTestInitialize() ) {
        return FALSE;
    }

    BasicSectionLoadUnloadTest();
    TamperedSectionTest();

    KdPrint(( "XETEST: finish\n" ));
    Sleep( INFINITE );
    return TRUE;
}

#pragma code_seg()
