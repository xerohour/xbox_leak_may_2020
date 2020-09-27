/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    coverage.cpp

Abstract:

    This module implements routines which provide support for collecting code
    coverage data for bbcover version 3.5

--*/

#include "coveragep.h"

INITIALIZED_CRITICAL_SECTION( CoverageRegistrationLock );

PREGISTERED_BINARY RegisteredBinaryList = (PREGISTERED_BINARY)&RegisteredBinaryList;

DWORD dwPollBufferSize = 0x100000;
PVOID pPollBuffer;



/*

Routine Description:

    RtlAppendAsciizToString was yanked from the libraries, 
    so a version is coded up here.

Arguments:

    STRING *ctString    - counted string 
    char *str           - ascii string to tack onto the end of ctString

Return Value:

    NTSTATUS

*/
NTSTATUS RtlAppendAsciizToString(STRING *ctString, char *str)
    {
    if(!str) return STATUS_INVALID_PARAMETER;
    USHORT len = (USHORT)strlen(str);

    if(len + ctString->Length > ctString->MaximumLength) return STATUS_BUFFER_TOO_SMALL;

    strcpy(ctString->Buffer+ctString->Length, str);
    ctString->Length = ctString->Length + len;

    return STATUS_SUCCESS;
    }


PVOID
CoverageAllocatePool(
    IN SIZE_T Size
    )
{
    PVOID p = ExAllocatePoolWithTag( Size, 'RVOC' );

    if ( p ) {
        RtlZeroMemory( p, Size );
    }

    return p;
}

PVOID
CoverageReAllocPool(
    IN SIZE_T Size,
    IN PVOID Ptr
    )
/*++

Routine Description:

    This routine reallocates a block of memory from kernel pool. This function
    resizes a memory block and change other memory block properties.

Arguments:

    Size - New size of memory block in bytes

    Ptr - Pointer to the block of memory that the function reallocates

Return Value:

    If the function succeeds, the return value is a pointer to the reallocated
    memory block and Ptr will be freed.  If the function fails, the return
    value is NULL

--*/
{
    PVOID NewPtr;
    SIZE_T PreviousSize;

    NewPtr = CoverageAllocatePool( Size );

    if ( NewPtr && Ptr ) {
        PreviousSize = ExQueryPoolBlockSize(Ptr);
        ASSERT( Size > PreviousSize );
        RtlCopyMemory( NewPtr, Ptr, PreviousSize );
        ExFreePool( Ptr );
    }

    return NewPtr;
}

PREGISTERED_BINARY
GetRegisteredBinaryList(
    VOID
    )
/*++

Routine Description:

    This in-line routine returns pointer to the list of registered binaries.

Arguments:

    None

Return Value:

    Pointer to the head of registered binary

--*/
{
    return RegisteredBinaryList;
}

PREGISTERED_BINARY
GetRegisteredBinaryListHead(
    VOID
    )
/*++

Routine Description:

    This in-line routine returns pointer to head of the list of registered
    binaries.

Arguments:

    None

Return Value:

    Pointer to the head of registered binary

--*/
{
    return (PREGISTERED_BINARY)&RegisteredBinaryList;
}

VOID
AcquireRegistrationLock(
    VOID
    )
/*++

Routine Description:

    This routine acquires the registration lock by entering critical section

Arguments:

    None

Return Value:

    None

--*/
{
    RtlEnterCriticalSection( &CoverageRegistrationLock );
}

VOID
ReleaseRegistrationLock(
    VOID
    )
/*++

Routine Description:

    This routine releases the registration lock by leaving critical section

Arguments:

    None

Return Value:

    None

--*/
{
    RtlLeaveCriticalSection( &CoverageRegistrationLock );
}

PREGISTERED_BINARY
INLINE
AllocateRegisteredBinaryNode(
    IN ULONG BinaryNameLength
    )
/*++

Routine Description:

    This routine allocates buffer for new registered binrary node.  The memory
    is allocated from kernel non-paged pool with pool tag 'COVR'

Arguments:

    BinaryNameLength - Length in bytes of binary name

Return Value:

    Pointer to newly allocated buffer or NULL

--*/
{
    return (PREGISTERED_BINARY)CoverageAllocatePool(
        sizeof(REGISTERED_BINARY) + BinaryNameLength );
}

PREGISTERED_BINARY
AllocateRegisteredBinaryNodeWithNameAndGUID(
    IN LPCSTR BinaryName,
    IN LPCSTR pszGUID
    )
/*++

Routine Description:

    Routine to allocate new node and initialize binary name and its GUID

Arguments:

    BinaryName - Name of binary being registered

    pszGUID - GUID of binary being registered

Return Value:

    Pointer to new registered node or NULL

--*/
{
    ULONG NameLength = strlen(BinaryName) + 1;
    ULONG GUIDLength = MD5_SIZE;
    PREGISTERED_BINARY pNode = AllocateRegisteredBinaryNode( NameLength );

    if ( pNode != NULL ) {
        RtlZeroMemory( pNode, sizeof( *pNode ) - sizeof( pNode->BinaryName ));
        RtlCopyMemory( pNode->BinaryName, BinaryName, NameLength );
        RtlCopyMemory( &(pNode->md5), pszGUID, GUIDLength );
    }

    return pNode;
}

VOID
INLINE
AddRegisteredBinaryNodeToList(
    IN PREGISTERED_BINARY pPrev,
    IN PREGISTERED_BINARY pNode
    )
{
    pNode->pNext = pPrev->pNext;
    pPrev->pNext = pNode;
}

VOID
NTAPI
InternalCoverageRegisterBinary(
    IN LPCSTR BinaryName,               // must be lowercase or case consistent
    IN LPCSTR pszGUID,
    IN ULONG  CheckSumOfBinary,         // checksum of original binary
    IN PCHAR  BaseOfLoggingRegion,      // instrumented logging region base
    IN ULONG  SizeOfLoggingRegion       // instrumented logging region size
    )
{
    PCHAR EndOfLoggingRegion;
    PREGISTERED_BINARY pNode;
    PREGISTERED_BINARY pPrev;
    BOOLEAN Found = FALSE;

    AcquireRegistrationLock();

    EndOfLoggingRegion = BaseOfLoggingRegion + SizeOfLoggingRegion - 1;

    //
    //  Walk list of registered binaries looking for name or logging region
    //

    pPrev = (PREGISTERED_BINARY)&RegisteredBinaryList;
    pNode = RegisteredBinaryList;

    while ( pNode != (PREGISTERED_BINARY)&RegisteredBinaryList ) {

        //
        //  Test if names match.  Names are always lowercase from BBCOVER.
        //

        if ( strcmp(pNode->BinaryName, BinaryName) == 0 ) {

            //
            //  Reloading binary with same name.  If not same checksum or
            //  size of logging region, need to invalidate old data.
            //

            if (( pNode->CheckSumOfBinary    != CheckSumOfBinary    ) ||
                ( pNode->SizeOfLoggingRegion != SizeOfLoggingRegion )) {

                //
                //  Reloading different version of binary with same name.
                //  Must invalidate any existing data for previous binary.
                //

                if ( pNode->CapturedLogging ) {

                    if ( !DoesBufferContainAllZeros(pNode->CapturedLogging, \
                                                    pNode->SizeOfLoggingRegion) ) {

                        DbgPrint(
                            "COVERAGE: %s logging data lost when new version of binary\n"
                            "    was loaded because data was not saved after previous version\n"
                            "    of binary was unloaded.\n",
                            BinaryName
                            );
                    }

                    ExFreePool( pNode->CapturedLogging );

                    pNode->CapturedLogging     = NULL;
                    pNode->CheckSumOfBinary    = CheckSumOfBinary;
                    pNode->BaseOfLoggingRegion = NULL;
                    pNode->SizeOfLoggingRegion = 0;

                }
            }

            //
            //  Names match, but now need to determine:
            //
            //      1) original driver still loaded, just re-registering (pnode->base == newbase)
            //      2) original driver unloaded, now reloading (pnode->base == NULL)
            //      3) original driver still loaded, loading new instance (pnode->base != NULL) && (pnode->base != newbase)
            //

            if ( pNode->BaseOfLoggingRegion == BaseOfLoggingRegion ) {

                //
                // Re-registering at same address, nothing to do.
                //

                DbgPrint(
                    "COVERAGE: %s re-registered at same address (0x%x - 0x%x)\n",
                    BinaryName, BaseOfLoggingRegion, EndOfLoggingRegion );

                Found = TRUE;
                break;

            }

            else if ( pNode->BaseOfLoggingRegion == NULL ) {

                //
                //  Originally unloaded, now reloading at new address.
                //

                DbgPrint( "COVERAGE: %s registered (0x%x - 0x%x) (reloaded)\n",
                          BinaryName, BaseOfLoggingRegion, EndOfLoggingRegion );

                pNode->BaseOfLoggingRegion = BaseOfLoggingRegion;
                pNode->SizeOfLoggingRegion = SizeOfLoggingRegion;

                Found = TRUE;
                break;

            }

            else if ( pNode->BaseOfLoggingRegion != BaseOfLoggingRegion ) {

                //
                //  Original is still loaded (or unload hook never called),
                //  so need to create new instance. (below)
                //

                DbgPrint( "COVERAGE: %s re-registered (0x%x - 0x%x)\n",
                          BinaryName, BaseOfLoggingRegion, EndOfLoggingRegion );

                break;

            }
        } else {

            //
            //  Not the same binary name, sanity test logging regions.
            //

            if ( (pNode->BaseOfLoggingRegion != NULL) &&
                 ((pNode->BaseOfLoggingRegion < EndOfLoggingRegion) ^ \
                  ((pNode->BaseOfLoggingRegion + pNode->SizeOfLoggingRegion - 1) < \
                   BaseOfLoggingRegion )) ) {

                //
                //  Some other binary already logging in requested range.
                //  This should never happen since a driver that can unload
                //  must have gone through our unload hook which would have
                //  zeroed the registered logging region.  We might hit this
                //  if we're not instrumenting to hook unload routines.
                //

                DbgPrint(
                    "COVERAGE: %s logging region (0x%x - 0x%x) overlaps region\n"
                    "    previously occupied by %s (%s - %s).  Must assume\n"
                    "    %s has been unloaded and will now be deregistered.\n",
                    BinaryName, BaseOfLoggingRegion, EndOfLoggingRegion,
                    pNode->BinaryName, pNode->BaseOfLoggingRegion,
                    pNode->BaseOfLoggingRegion + pNode->SizeOfLoggingRegion - 1,
                    pNode->BinaryName );

                pNode->BaseOfLoggingRegion = NULL;

            }
        }

        pPrev = pNode;
        pNode = pNode->pNext;

    }

    if ( !Found ) {

        //
        //  New binary, add to list.
        //

        pNode = AllocateRegisteredBinaryNodeWithNameAndGUID( BinaryName, pszGUID );

        if ( pNode ) {

            pNode->BaseOfLoggingRegion = BaseOfLoggingRegion;
            pNode->SizeOfLoggingRegion = SizeOfLoggingRegion;
            pNode->CheckSumOfBinary    = CheckSumOfBinary;

            AddRegisteredBinaryNodeToList( pPrev, pNode );

            DbgPrint( "COVERAGE: %s registered (0x%x - 0x%x)\n", BinaryName,
                      BaseOfLoggingRegion, EndOfLoggingRegion );

        } else {

            DbgPrint(
                "COVERAGE: %s could not be registered (failed to allocate memory).\n",
                BinaryName );

        }
    }

    ReleaseRegistrationLock();
}

VOID
NTAPI
CoverageRegisterBinaryWithStruct(
    IN PBBREG_HEADER pBBHeader
    )
{
    InternalCoverageRegisterBinary(
        (PCHAR)pBBHeader + pBBHeader->dwOffsetOfBinaryName,
        (PCHAR)pBBHeader + pBBHeader->dwOffsetOfGUID,
        pBBHeader->dwCheckSumOfBinary,
        (PCHAR)pBBHeader + pBBHeader->dwOffsetOfLoggingRegion,
        pBBHeader->dwSizeOfLoggingRegion
        );
}

VOID
NTAPI
RegisterNoImportCoverageBinary(
    IN PVOID ImageBase
    )
{
    PIMAGE_DOS_HEADER DosHeader;
    PIMAGE_NT_HEADERS NtHeader;
    ULONGLONG SectionName;
    ULONG SectionCount;
    PCHAR SectionBase;
    ULONG i;
    PIMAGE_SECTION_HEADER SectionHeader;

    //
    //  We assume that each module's PE header is resident at its mapped
    //  base address.  We walk the PE header and PE section table looking
    //  for ".cover" section.  If we find it, we register the binary.
    //
    //  If we do eventually make this routine callable other than during
    //  DriverEntry, then we'll need to move it to the PAGE section rather
    //  than the INIT section.
    //

    __try {

        if ( !MmIsAddressValid(ImageBase) ) {
            __leave;
        }

        DosHeader = (PIMAGE_DOS_HEADER)ImageBase;

        if ( DosHeader->e_magic == IMAGE_DOS_SIGNATURE ) {

            NtHeader = RtlImageNtHeader( ImageBase );

            if ( ! MmIsAddressValid(NtHeader) ) {
                __leave;
            }

        } else {

            NtHeader = (PIMAGE_NT_HEADERS)ImageBase;

        }

        if ( NtHeader->Signature != IMAGE_NT_SIGNATURE ) {
            __leave;
        }

        SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
        SectionCount  = NtHeader->FileHeader.NumberOfSections;

        if ( !MmIsAddressValid(&SectionHeader[SectionCount]) ) {
            __leave;
        }

        for ( i=0; i<SectionCount; i++ ) {

            //
            //  To make section name comparison fast, read it into an 8-byte
            //  DWORDLONG, do a sneaky lowercase by ORing all the characters
            //  with 0x20, which also has the effect of "normalizing" any
            //  zero byte characters to spaces.  Then we can just do a simple
            //  DWORDLONG comparison with the ".cover  " equivalent value.
            //

            SectionName = *(UNALIGNED ULONGLONG*)( &SectionHeader[ i ].Name );
            SectionName |= 0x2020202020202020;

#if 0   // too verbose

            KdPrint(( "COVERAGE: Inspecting section \"%c%c%c%c%c%c%c%c\" at %08X\n",
                      (UCHAR)(SectionName >> 0), (UCHAR)(SectionName >> 8),
                      (UCHAR)(SectionName >> 16), (UCHAR)(SectionName >> 24),
                      (UCHAR)(SectionName >> 32), (UCHAR)(SectionName >> 40),
                      (UCHAR)(SectionName >> 48), (UCHAR)(SectionName >> 56),
                      (PCHAR)ImageBase + SectionHeader[i].VirtualAddress ));

#endif  // too verbose

            if ( SectionName == 0x20207265766F632E ) {      // ".cover  "

                //
                //  Data stored in coverage section looks like this:
                //
                //      Offset 0: DWORD  SizeOfLoggingRegion
                //      Offset 4: DWORD  CheckSumOfOriginalBinary
                //      Offset 8: CHAR[] FileNameOfOriginalBinary (null terminated string)
                //      Offset X: CHAR[] LoggingBuffer (size is SizeOfLoggingRegion)
                //
                //      X is computed as ROUNDUP2(( strlen( name ) + 1 + 8 ), 32 ) to
                //      place the LoggingBuffer on a 32-byte boundary.
                //

                SectionBase = (PCHAR)((PCHAR)ImageBase + SectionHeader[i].VirtualAddress);

                KdPrint(( "COVERAGE: Found \".cover\" section in image %08X at %08X\n",
                           ImageBase, SectionBase ));

                if ( MmIsAddressValid(SectionBase) ) {
                    CoverageRegisterBinaryWithStruct( (PBBREG_HEADER)SectionBase );
                }

                break;
            }
        }
    } __except( EXCEPTION_EXECUTE_HANDLER ) {
          KdPrint(( "COVERAGE: exception 0x%x while registerring image base = %p\n",
                    GetExceptionCode(), ImageBase ));
    }
}

NTSTATUS
CoverageInternalOpenLogFile(
    IN PREGISTERED_BINARY Node
    )
/*++

Routine Description:

    This routine opens coverage log file under c:\devkit\coverage\<BinaryName>\
    <BinaryName>.covdata.  It also read in previously saved data if exists.
    New covdata file will be created if the checksym of the binary does not
    match the previously saved data.

Arguments:

    Node - Registered node of the binary

Return Value:

    NT status code

--*/
{
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING PathString;
    OCHAR UniquifierBuffer[18];
    OCHAR PathBuffer[MAX_PATH+100];
    IO_STATUS_BLOCK IoStatusBlock;
    NTSTATUS Status = STATUS_SUCCESS;
    HANDLE Handle = NULL;
    FILE_NETWORK_OPEN_INFORMATION NetworkInfo;
    PCOVFILE_HEADER pCovFileHeader = NULL;
    PCOVFILE_HEADER pCovHeader;
    ULONG NumberOfBytesRead;
    DWORD dwHeaderSize;
    BOOL fRecurse = FALSE;

    ASSERT( Node->LogFileHandle == NULL );

    __try {

        //
        // Make sure we have coverage directory created under c:\devkit\coverage
        // We don't have to check for c:\devkit first because we were loaded
        // from c:\devkit\dxt\coverage.dxt
        //

        PathString.Buffer = PathBuffer;
        PathString.Length = 0;
        PathString.MaximumLength = sizeof(PathBuffer);

        Status = RtlAppendAsciizToString( &PathString,
            "\\Device\\Harddisk0\\Partition1\\Devkit\\Coverage\\" );

        if ( !NT_SUCCESS(Status) ) {
            __leave;
        }

        InitializeObjectAttributes( &ObjA, &PathString, OBJ_CASE_INSENSITIVE,
                                    NULL, NULL );

        Status = NtCreateFile( &Handle, FILE_LIST_DIRECTORY | SYNCHRONIZE,
                               &ObjA, &IoStatusBlock, NULL,
                               FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | \
                               FILE_SHARE_WRITE, FILE_CREATE, \
                               FILE_DIRECTORY_FILE | \
                               FILE_SYNCHRONOUS_IO_NONALERT | \
                               FILE_OPEN_FOR_BACKUP_INTENT );

        if ( NT_SUCCESS(Status) ) {
            NtClose( Handle );
            Handle = NULL;
        } else if ( Status != STATUS_OBJECT_NAME_COLLISION ) {
            DbgPrint( "COVERAGE: LOST DATA for %s because failed to open directory\n"
                      "    %Z (0x%x)\n", Node->BinaryName,
                      &PathString, Status );
            __leave;
        }

        //
        // Now create a directory named <BinaryName> under Coverage
        //

        Status = RtlAppendAsciizToString( &PathString, Node->BinaryName );

        if ( !NT_SUCCESS(Status) ) {
            __leave;
        }

        Status = NtCreateFile( &Handle, FILE_LIST_DIRECTORY | SYNCHRONIZE,
                               &ObjA, &IoStatusBlock, NULL,
                               FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | \
                               FILE_SHARE_WRITE, FILE_CREATE, \
                               FILE_DIRECTORY_FILE | \
                               FILE_SYNCHRONOUS_IO_NONALERT | \
                               FILE_OPEN_FOR_BACKUP_INTENT );

        if ( NT_SUCCESS(Status) ) {
            NtClose( Handle );
            Handle = NULL;
        } else if ( Status != STATUS_OBJECT_NAME_COLLISION ) {
            DbgPrint( "COVERAGE: LOST DATA for %s because failed to open directory\n"
                      "    directory %Z (0x%x)\n", Node->BinaryName,
                      &PathString, Status );
            __leave;
        }

        //
        // Now create a log file for this binary and also append checksum of
        // the binary if the this a newer version
        //

        Status = RtlAppendAsciizToString( &PathString, "\\" );

        if ( !NT_SUCCESS(Status) ) {
            __leave;
        }

        Status = RtlAppendAsciizToString( &PathString, Node->BinaryName );

        if ( !NT_SUCCESS(Status) ) {
            __leave;
        }

        if ( Node->dwUniquifier ) {
            sprintf( UniquifierBuffer, "%.08X.%X", Node->CheckSumOfBinary,
                     Node->dwUniquifier );
            Status = RtlAppendAsciizToString( &PathString, UniquifierBuffer );
            if ( !NT_SUCCESS(Status) ) {
                __leave;
            }
        }

        Status = RtlAppendAsciizToString( &PathString, ".covdata" );

        if ( !NT_SUCCESS(Status) ) {
            __leave;
        }

        Status = NtCreateFile( &Handle, GENERIC_READ | GENERIC_WRITE | \
                               SYNCHRONIZE | FILE_READ_ATTRIBUTES, &ObjA,
                               &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL,
                               FILE_SHARE_READ, FILE_OPEN_IF,
                               FILE_RANDOM_ACCESS | FILE_WRITE_THROUGH
                               );

        if ( !NT_SUCCESS(Status) ) {
            DbgPrint( "COVERAGE: LOST DATA for %s because failed to open file\n"
                      "    %Z (0x%x)\n", Node->BinaryName,
                      &PathString, Status );
            __leave;
        }

        //
        // Query size of the file to see if we already have previously saved data
        //

        Status = NtQueryInformationFile( Handle, &IoStatusBlock,
                                         &NetworkInfo, sizeof(NetworkInfo),
                                         FileNetworkOpenInformation );

        if ( !NT_SUCCESS(Status) ) {
            KdPrint(( "COVERAGE: LOST DATA for %s because failed to query file\n"
                      "    %Z (0x%x)\n", Node->BinaryName, &PathString, Status ));
            __leave;
        }

        pCovFileHeader = (PCOVFILE_HEADER)CoverageAllocatePool(
            COVFILE_HEADER_MIN_SIZE );

        if ( !pCovFileHeader ) {
            Status = STATUS_NO_MEMORY;
            KdPrint(( "COVERAGE: LOST DATA for %s because failed to allocate memory\n",
                      Node->BinaryName ));
            __leave;
        }

        //
        // Read in the previously saved data if exists
        //

        NumberOfBytesRead = 0;

        if ( NetworkInfo.EndOfFile.QuadPart >= COVFILE_HEADER_MIN_SIZE ) {

            Status = NtReadFile( Handle, 0, 0, 0, &IoStatusBlock,
                                 pCovFileHeader, COVFILE_HEADER_MIN_SIZE,
                                 0 );

            if ( NT_SUCCESS(Status) ) {
                NumberOfBytesRead = IoStatusBlock.Information;
            } else {
                KdPrint(( "COVERAGE: LOST DATA for %s because failed to read file\n"
                          "    %Z (0x%x)\n", Node->BinaryName, &PathString, Status ));
                __leave;
            }
        }

        if ( NumberOfBytesRead != COVFILE_HEADER_MIN_SIZE ) {

            //
            // New file being created
            //

            KdPrint(( "COVERAGE: New file: %Z\n", &PathString ));

            RtlZeroMemory( pCovFileHeader, COVFILE_HEADER_MIN_SIZE );

            pCovFileHeader->dwSignature  = COVFILE_SIGNATURE;
            pCovFileHeader->dwVersion    = COVFILE_VERSION;
            pCovFileHeader->dwCheckSum   = Node->CheckSumOfBinary;
            pCovFileHeader->dwBlockCount = Node->SizeOfLoggingRegion;

            //
            // Header length is defined to be COVFILE_HEADER + GUID + one DIR_ENTRY,
            // rounded up to 512
            //

            pCovFileHeader->dwHeaderLength = ROUNDUP2( sizeof(COVFILE_HEADER) +
                MD5_SIZE + sizeof(COVFILE_DIRECTORY), COVFILE_HEADER_MIN_SIZE );

            //
            // Put GUID at end of header
            //

            pCovFileHeader->dwOffsetOfGUID = sizeof(COVFILE_HEADER);

            //
            // Put the first dir entry after the GUID
            //

            pCovFileHeader->dwOffsetOfFirstDir = ROUNDUP2( sizeof(COVFILE_HEADER) +
                                                           MD5_SIZE, 4 );

            RtlCopyMemory( (PCHAR)pCovFileHeader + pCovFileHeader->dwOffsetOfGUID,
                           &(Node->md5), MD5_SIZE );

            NetworkInfo.EndOfFile.QuadPart = COVFILE_HEADER_MIN_SIZE;

        } else if ( pCovFileHeader->dwSignature != COVFILE_SIGNATURE ||
                    pCovFileHeader->dwCheckSum != Node->CheckSumOfBinary ||
                    pCovFileHeader->dwBlockCount != Node->SizeOfLoggingRegion ) {

            //
            //  Existing file does not match checksum, close existing and open new.
            //

            DbgPrint( "COVERMON: Binary %s contains different checksum than %Z\n",
                      Node->BinaryName, &PathString );

            Node->dwUniquifier++;
            fRecurse = TRUE;
            Status = STATUS_PENDING;

            __leave;

        } else if ( pCovFileHeader->dwVersion > COVFILE_VERSION ) {

            //
            //  File is of newer version than we understand.
            //

            DbgPrint( "COVERMON: %Z is %s version than covermon understands\n",
                      &PathString, "newer" );

            Node->dwUniquifier++;
            fRecurse = TRUE;
            Status = STATUS_PENDING;

            __leave;

        } else if ( pCovFileHeader->dwVersion < COVFILE_VERSION ) {

            //
            //  File is of older version than we understand.
            //

            DbgPrint( "COVERMON: %Z is %s version than covermon understands\n",
                      &PathString, "older" );

            Node->dwUniquifier++;
            fRecurse = TRUE;
            Status = STATUS_PENDING;

            __leave;

        } else if ( pCovFileHeader->dwHeaderLength > COVFILE_HEADER_MIN_SIZE ) {

            //
            //  More header needs to be read and cached.
            //

            dwHeaderSize = pCovFileHeader->dwHeaderLength;
            pCovHeader = pCovFileHeader;

            pCovFileHeader = (PCOVFILE_HEADER)CoverageReAllocPool(
                dwHeaderSize, pCovHeader );

            if ( pCovFileHeader == NULL ) {

                DbgPrint( "COVERMON: LOST DATA for %s because failed to "
                          "allocate memory (%d bytes).\n", Node->BinaryName,
                          dwHeaderSize );

                pCovFileHeader = pCovHeader;
                Status = STATUS_NO_MEMORY;
                __leave;
            }

            NumberOfBytesRead = 0;

            Status = NtReadFile( Handle, 0, 0, 0, &IoStatusBlock,
                                 (PCHAR)pCovFileHeader + COVFILE_HEADER_MIN_SIZE,
                                 dwHeaderSize - COVFILE_HEADER_MIN_SIZE,
                                 0);

            if ( NT_SUCCESS(Status) ) {
                NumberOfBytesRead = IoStatusBlock.Information;
            } else {
                KdPrint(( "COVERAGE: LOST DATA for %s because failed to read file\n"
                          "    %Z (0x%x)\n", Node->BinaryName, &PathString, Status ));
                __leave;
            }
        }

        Node->pCachedCovFileHeader = pCovFileHeader;
        Node->LogFileHandle = Handle;
        Node->FileSize = NetworkInfo.EndOfFile;
        Status = STATUS_SUCCESS;

    } __finally {
        if ( !NT_SUCCESS(Status) ) {
            if ( Handle ) {
                NtClose( Handle );
            }
            if ( pCovFileHeader ) {
                ExFreePool( pCovFileHeader );
            }
        } else {
            Node->LogFileHandle = Handle;
        }
    }

    return fRecurse ? CoverageInternalOpenLogFile( Node ) : Status;
}

VOID
CoverageInternalCloseLogFile(
    PREGISTERED_BINARY Node
    )
/*++

Routine Description:

    This routine closes log file for the binary specified by registration
    node.  It also free memory allocated by CoverageInternalOpenLogFile.

Arguments:

    Node - Pointer to registration node

Return Value:

    None

--*/
{
    if ( Node->LogFileHandle != NULL ) {

        DbgPrint( "COVERMON: Closing covdata file for %s\n", Node->BinaryName );
        Node->dwUniquifier = 0;

        NtClose( Node->LogFileHandle );
        Node->LogFileHandle = NULL;

        if ( Node->pCachedCovFileHeader != NULL ) {
            ExFreePool( Node->pCachedCovFileHeader );
            Node->pCachedCovFileHeader = NULL;
        }
    }
}

VOID
CoverageInternalCloseLogFileAllBinaries(
    VOID
    )
/*++

Routine Description:

    This routine closes log files for all binaries.  It is called during the
    reboot notification.

Arguments:

    None

Return Value:

    None

--*/
{
    PREGISTERED_BINARY Node;

    AcquireRegistrationLock();

    Node = GetRegisteredBinaryList();
    while ( Node != GetRegisteredBinaryListHead() ) {
        CoverageInternalCloseLogFile( Node );
        Node = Node->pNext;
    }

    ReleaseRegistrationLock();
}

NTSTATUS
NTAPI
CoverageFlushLogging(
    IN PCSTR BinaryName OPTIONAL
    )
/*++

Routine Description:

    Resets (discards) any existing coverage information that has not been
    polled and saved for one or all running coverage binaries.  This can
    be useful to "zero" any coverage information prior to running a
    specific test.

Arguments:

    BinaryName - Optional name of the binary, if this argument is present
        only binary name matches will be flush

Return Value:

    NT status code

--*/
{
    NTSTATUS Status;
    PREGISTERED_BINARY Node;
    BOOL bFound = FALSE;

    AcquireRegistrationLock();

    Node = GetRegisteredBinaryList();

    while ( Node != GetRegisteredBinaryListHead() ) {
        if ( !ARGUMENT_PRESENT(BinaryName) ||
             !_stricmp(Node->BinaryName, BinaryName) ) {

            //
            // Zero out all the coverage data
            //

            if ( Node->BaseOfLoggingRegion && Node->SizeOfLoggingRegion ) {
                RtlZeroMemory( Node->BaseOfLoggingRegion, Node->SizeOfLoggingRegion );
            }

            if ( Node->CapturedLogging ) {
                ExFreePool( Node->CapturedLogging );
                Node->CapturedLogging = NULL;
            }

            bFound = TRUE;
        }

        Node = Node->pNext;
    }

    if ( ARGUMENT_PRESENT(BinaryName) && !bFound ) {
        Status = STATUS_DLL_NOT_FOUND;
    } else {
        Status = STATUS_SUCCESS;
    }

    ReleaseRegistrationLock();

    return Status;
}

BOOL
CoverageInitPoolBuffer(
    VOID
    )
/*++

Routine Description:

    This routine allocates memory for poll buffer.  The memory is allocated
    from non-paged kernel pool.

Arguments:

    None

Return Value:

    None

--*/
{
    dwPollBufferSize = ROUNDUP2( dwPollBufferSize, 8192 );
    pPollBuffer = CoverageAllocatePool( dwPollBufferSize );
    return pPollBuffer != NULL;
}

NTSTATUS
CoverageInternalUpdateLogFile(
    IN PREGISTERED_BINARY pNode,
    IN PCSTR pszTestName
    )
/*++

Routine Description:

    This routine updates coverage data to log file.  The file format is based
    on bbcover version 3.5

Arguments:

    Node - Registration node of the binary

    TestName - Name of the test to be saved

Return Value:

    NT status code

--*/
{
    PCOVFILE_HEADER pHeader;
    PCOVFILE_DIRECTORY pDirEntry;
    PCOVFILE_DIRECTORY_201 pDirEntry201;
    HANDLE hFile;
    DWORD dwNameHash;
    DWORD dwVectorSize;
    PVOID pNewBitVector;
    PVOID pOldBitVector;
    PVOID pMoveBuffer;
    bool  bFound;
    DWORD dwActual;
    DWORDLONG dwlOffset;
    DWORDLONG dwlFileSize;
    DWORDLONG dwlVectorOffset;
    DWORDLONG dwlMoveBias;
    DWORDLONG dwlExtent;
    PVOID pNewHeader;
    DWORD dwNameLength;
    DWORD dwEntryLength;
    DWORD dwGrowSize;
    DWORD dwOffsetOfFirstDir;
    LPSTR pszName;
    bool  bDataHasChanged;
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_POSITION_INFORMATION CurrentPosition;

    //
    //  BUGBUG: The old code assumes that PollBufferCritSect is held.
    //

    pNewBitVector  = NULL;
    pOldBitVector  = NULL;
    pMoveBuffer    = NULL;
    pNewHeader     = NULL;
    bDataHasChanged = true;

    ASSERT( DoesBufferContainOnlyHex01s(pPollBuffer, pNode->SizeOfLoggingRegion) );

    __try {

        dwVectorSize = BITVECTOR_STORAGE_SIZE( pNode->SizeOfLoggingRegion );

        pNewBitVector = CoverageAllocatePool( dwVectorSize );

        if ( pNewBitVector == NULL ) {
            DbgPrint( "COVERMON: LOST DATA for %s because failed to allocate "
                     "memory (%d bytes).\n", pNode->BinaryName, dwVectorSize );
            Status = STATUS_NO_MEMORY;
            __leave;
        }

        *(UNALIGNED DWORD*)((PCHAR)pNewBitVector + dwVectorSize - 4 ) = 0;

        PackByteVectorToBitVector( pNode->SizeOfLoggingRegion,
                                   (PBYTE)pPollBuffer, (PBYTE)pNewBitVector );

        ASSERT( CountBitsSetInBuffer(pPollBuffer, pNode->SizeOfLoggingRegion) ==
            CountBitsSetInBuffer(pNewBitVector, dwVectorSize) );

        bFound     = false;
        hFile      = pNode->LogFileHandle;
        dwNameHash = HashName( pszTestName );
        pHeader    = pNode->pCachedCovFileHeader;
        dwOffsetOfFirstDir = pHeader->dwOffsetOfFirstDir;
        pDirEntry  = (PCOVFILE_DIRECTORY)((PCHAR)pHeader + dwOffsetOfFirstDir);

        while ( pDirEntry->dwLengthOfEntry != 0 ) {

            if ( pDirEntry->dwHashOfName == dwNameHash ) {

                //
                //  Offset of pDirEntry->szName is different for versions
                //  201 and 202 of covdata file.
                //

                if ( pHeader->dwVersion == COVFILE_VERSION_201 ) {
                    pszName = ((PCOVFILE_DIRECTORY_201)pDirEntry)->szName;
                } else {
                    ASSERT( pHeader->dwVersion == COVFILE_VERSION );
                    pszName = pDirEntry->szName;
                }

                if ( _stricmp( pszName, pszTestName ) == 0 ) {
                    bFound = true;
                    break;
                }
            }

            pDirEntry = (PCOVFILE_DIRECTORY)( (PCHAR)pDirEntry +
                                              pDirEntry->dwLengthOfEntry );
        }

        if ( bFound ) {

            //
            //  If test name found, read existing vector, OR with new vector,
            //  write back to file.
            //

            pOldBitVector = CoverageAllocatePool( dwVectorSize );

            if( pOldBitVector == NULL ){
                DbgPrint( "COVERMON: LOST DATA for %s because failed to "
                          "allocate memory (%d bytes).\n", pNode->BinaryName,
                          dwVectorSize );
                Status = STATUS_NO_MEMORY;
                __leave;
            }

            if ( pHeader->dwVersion == COVFILE_VERSION_201 ) {
                dwlVectorOffset = ((PCOVFILE_DIRECTORY_201)pDirEntry)->dwOffsetOfData;
            }
            else {
                ASSERT( pHeader->dwVersion == COVFILE_VERSION );
                dwlVectorOffset = pDirEntry->dwlOffsetOfData;
            }

            dwActual = 0;

            //
            // If there is already a saved vector, then read from covdata file
            //

            CurrentPosition.CurrentByteOffset.QuadPart = dwlVectorOffset;

            Status = NtSetInformationFile( hFile, &IoStatusBlock,
                &CurrentPosition, sizeof(CurrentPosition),
                FilePositionInformation );

            if ( !NT_SUCCESS(Status) ) {
                __leave;
            }

            Status = NtReadFile( hFile, 0, 0, 0, &IoStatusBlock, pOldBitVector,
                                 dwVectorSize, 0);

            if ( NT_SUCCESS(Status) ) {
                dwActual = IoStatusBlock.Information;
            } else {
                DbgPrint( "COVERMON: LOST DATA for %s because failed to read "
                          "covdata file (0x%x)\n", pNode->BinaryName,
                          Status );
                __leave;
            }

            //
            // 1) NewBuf = NewBuf OR OldBuf
            // 2) OldBuf = NewBuf XOR OldBuf
            // 3) If any 1's in OldBuf, then there were changes; save data
            //    otherwise, no changes, so don't save
            //
            OrBufferWithBuffer( pNewBitVector, pOldBitVector, dwVectorSize );

            ASSERT( CountBitsSetInBuffer( pNewBitVector, dwVectorSize ) >=
                CountBitsSetInBuffer( pOldBitVector, dwVectorSize ));

            XorBufferWithBuffer( pOldBitVector, pNewBitVector, dwVectorSize );

            if( DoesBufferContainAllZeros( pOldBitVector, dwVectorSize ) ){
                bDataHasChanged = false;
            } else {
                bDataHasChanged = true;
            }

            dwActual = 0;

            if( bDataHasChanged ){
                CurrentPosition.CurrentByteOffset.QuadPart = dwlVectorOffset;

                Status = NtSetInformationFile( hFile, &IoStatusBlock,
                    &CurrentPosition, sizeof(CurrentPosition),
                    FilePositionInformation );

                if ( !NT_SUCCESS(Status) ) {
                    __leave;
                }

                Status = NtWriteFile( hFile, 0, 0, 0, &IoStatusBlock,
                                      pNewBitVector, dwVectorSize, 0);

                if ( !NT_SUCCESS(Status) ) {
                    DbgPrint( "COVERMON: LOST DATA for %s because failed "
                              "to write covdata file (0x%x)\n",
                              pNode->BinaryName, Status );
                    __leave;
                }
            }

            NtFlushBuffersFile( hFile, &IoStatusBlock );

        } else {

            //
            //  If test not found, write new vector then update header (grow
            //  header if necessary).  We write new vector first in case later
            //  fail to write updated header -- file will still be valid but
            //  without new vector.  If wrote header first then failed to write
            //  vector, file would left in invalid state.
            //

            dwlVectorOffset = ROUNDUP2_64( pNode->FileSize.QuadPart, COVFILE_VECTOR_BOUNDARY );
            dwActual = 0;

            CurrentPosition.CurrentByteOffset.QuadPart = dwlVectorOffset;

            Status = NtSetInformationFile( hFile, &IoStatusBlock,
                &CurrentPosition, sizeof(CurrentPosition),
                FilePositionInformation );

            if ( !NT_SUCCESS(Status) ) {
                __leave;
            }

            Status = NtWriteFile( hFile, 0, 0, 0, &IoStatusBlock, pNewBitVector,
                                  dwVectorSize, 0);

            if ( NT_SUCCESS(Status) ) {
                dwActual = IoStatusBlock.Information;
            } else {
                DbgPrint( "COVERMON: LOST DATA for %s because failed to "
                          "write covdata file (0x%x)\n",
                          pNode->BinaryName, Status );
                __leave;
            }

            NtFlushBuffersFile( hFile, &IoStatusBlock );

            dwlFileSize = dwlVectorOffset + dwActual;

            //
            //  Now update the header.
            //

            dwNameLength  = strlen( pszTestName ) + 1;
            dwEntryLength = ROUNDUP2( sizeof( COVFILE_DIRECTORY ) + dwNameLength, 4 );

            if ((((PCHAR)pDirEntry - (PCHAR)pHeader ) + dwEntryLength + 4 ) > pHeader->dwHeaderLength ) {

                //
                //  Not enough room in existing header, need to grow it by multiple
                //  of ROUNDUP2( dwVectorSize, COVFILE_VECTOR_BOUNDARY ).
                //

                dwGrowSize  = ROUNDUPN( dwEntryLength, ROUNDUP2( dwVectorSize, COVFILE_VECTOR_BOUNDARY ));
                dwlFileSize = ROUNDUP2_64( dwlFileSize, COVFILE_VECTOR_BOUNDARY );
                dwlOffset   = ROUNDUP2_64( pHeader->dwHeaderLength, COVFILE_VECTOR_BOUNDARY );

                dwlExtent   = dwlOffset + dwGrowSize;
                dwlMoveBias = dwlFileSize - dwlOffset;
                dwActual    = 0;

                if (( pHeader->dwVersion == COVFILE_VERSION_201 ) &&
                    ( dwlFileSize + dwGrowSize > 0xF0000000 )) {

                    DbgPrint(
                        "COVERMON: LOST DATA for %s because covdata file is too large.\n"
                        "    Use covdata.exe tool to upgrade covdata file to new version.\n",
                        pNode->BinaryName
                        );

                    __leave;
                }

                pMoveBuffer = CoverageAllocatePool( dwGrowSize );

                if ( pMoveBuffer == NULL ) {

                    DbgPrint(
                        "COVERMON: LOST DATA for %s because failed to allocate buffer.\n",
                        pNode->BinaryName
                        );

                    __leave;
                }

                CurrentPosition.CurrentByteOffset.QuadPart = dwlOffset;

                Status = NtSetInformationFile( hFile, &IoStatusBlock,
                    &CurrentPosition, sizeof(CurrentPosition),
                    FilePositionInformation );

                if ( !NT_SUCCESS(Status) ) {
                    __leave;
                }

                Status = NtReadFile( hFile, 0, 0, 0, &IoStatusBlock, pMoveBuffer, dwGrowSize, 0);

                if ( NT_SUCCESS(Status) ) {
                    dwActual = IoStatusBlock.Information;
                } else {
                    DbgPrint(
                        "COVERMON: LOST DATA for %s because failed to read covdata file (0x%x)\n",
                        pNode->BinaryName, Status
                        );
                    __leave;
                }

                CurrentPosition.CurrentByteOffset.QuadPart = dwlFileSize;

                Status = NtSetInformationFile( hFile, &IoStatusBlock,
                    &CurrentPosition, sizeof(CurrentPosition),
                    FilePositionInformation );

                if ( !NT_SUCCESS(Status) ) {
                    __leave;
                }

                Status = NtWriteFile( hFile, 0, 0, 0, &IoStatusBlock, pMoveBuffer, dwActual, 0);

                if ( NT_SUCCESS(Status) ) {
                    dwActual = IoStatusBlock.Information;
                } else {
                    DbgPrint(
                        "COVERMON: LOST DATA for %s because failed to write covdata file (0x%x)\n",
                        pNode->BinaryName, Status
                        );
                    __leave;
                }

                NtFlushBuffersFile( hFile, &IoStatusBlock );

                dwlFileSize = dwlFileSize + dwActual;

                pNewHeader = CoverageReAllocPool(
                    pHeader->dwHeaderLength + dwGrowSize, pHeader );

                if ( pNewHeader == NULL ) {

                    DbgPrint(
                        "COVERMON: LOST DATA for %s because failed to allocate memory (%d bytes).\n",
                        pNode->BinaryName,
                        pHeader->dwHeaderLength + dwGrowSize
                        );

                    Status = STATUS_NO_MEMORY;
                    __leave;
                }

                pHeader = (PCOVFILE_HEADER)pNewHeader;
                pNode->pCachedCovFileHeader = pHeader;
                pHeader->dwHeaderLength += dwGrowSize;
                pDirEntry = (PCOVFILE_DIRECTORY)((PCHAR)pHeader + dwOffsetOfFirstDir);

                while ( pDirEntry->dwLengthOfEntry != 0 ) {

                    if ( pHeader->dwVersion == COVFILE_VERSION_201 ) {

                        pDirEntry201 = (PCOVFILE_DIRECTORY_201) pDirEntry;

                        if (( pDirEntry201->dwOffsetOfData >= dwlOffset ) &&
                            ( pDirEntry201->dwOffsetOfData <  dwlExtent )) {

                            ASSERT((( pDirEntry201->dwOffsetOfData + dwlMoveBias ) >> 32 ) == 0 );

                            pDirEntry201->dwOffsetOfData += (DWORD) dwlMoveBias;

                        }

                    } else {

                        ASSERT( pHeader->dwVersion == COVFILE_VERSION );

                        if (( pDirEntry->dwlOffsetOfData >= dwlOffset ) &&
                            ( pDirEntry->dwlOffsetOfData <  dwlExtent )) {

                            pDirEntry->dwlOffsetOfData += dwlMoveBias;

                        }
                    }

                    pDirEntry = (PCOVFILE_DIRECTORY)((PCHAR)pDirEntry + pDirEntry->dwLengthOfEntry );

                }
            }

            pDirEntry->dwLengthOfEntry = dwEntryLength;
            pDirEntry->dwHashOfName    = dwNameHash;

            if ( pHeader->dwVersion == COVFILE_VERSION_201 ) {

                pDirEntry201 = (PCOVFILE_DIRECTORY_201) pDirEntry;

                ASSERT(( dwlVectorOffset >> 32 ) == 0 );

                pDirEntry201->dwOffsetOfData = (DWORD)dwlVectorOffset;
                strcpy( pDirEntry201->szName, pszTestName );

            } else {

                ASSERT( pHeader->dwVersion == COVFILE_VERSION );

                pDirEntry->dwlOffsetOfData = dwlVectorOffset;
                strcpy( pDirEntry->szName, pszTestName );

            }

            CurrentPosition.CurrentByteOffset.QuadPart = 0;

            Status = NtSetInformationFile( hFile, &IoStatusBlock,
                &CurrentPosition, sizeof(CurrentPosition),
                FilePositionInformation );

            if ( !NT_SUCCESS(Status) ) {
                __leave;
            }

            Status = NtWriteFile( hFile, 0, 0, 0, &IoStatusBlock, pHeader,
                                  pHeader->dwHeaderLength, 0);

            if ( NT_SUCCESS(Status) ) {
                dwActual = IoStatusBlock.Information;
            } else {
                DbgPrint(
                    "COVERMON: LOST DATA for %s because failed to write covdata file (0x%x)\n",
                    pNode->BinaryName, Status
                    );
                __leave;
            }

            NtFlushBuffersFile( hFile, &IoStatusBlock );

            pNode->FileSize.QuadPart = dwlFileSize;   // only if everything successful
        }
    } __except( EXCEPTION_EXECUTE_HANDLER ) {

        DbgPrint( "COVERMON: LOST DATA for %s because an exception occurred\n"
                  "    while updating the file.  This probably indicates a \n"
                  "corrupt data file.\n", pNode->BinaryName );

        Status = GetExceptionCode();
    }

    if ( pMoveBuffer ) {
        ExFreePool( pMoveBuffer );
    }

    if ( pOldBitVector ) {
        ExFreePool( pOldBitVector );
    }

    if ( pNewHeader ) {
        ExFreePool( pNewBitVector );
    }

    return Status;
}
