
#include "precomp.h"
#pragma hdrstop

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  cdimage.c   Program for generating ISO-9660 CD images from a tree      //
//              of files (commonly referred to as premastering).           //
//                                                                         //
//              Code is targeted as Win32 console application.             //
//                                                                         //
//              Author: Tom McGuire (tommcg)                               //
//                                                                         //
//              Original version written February 1993.                    //
//                                                                         //
//              (C) Copyright 1993-2000, Microsoft Corporation             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

typedef struct _TRANSFER_CONTEXT {
    HANDLE hOpenContext;
    HANDLE hEventComplete;
    } TRANSFER_CONTEXT, *PTRANSFER_CONTEXT;

PDIRENTRY pRootDir;

PDIRENTRY pStartOfLevel[ MAX_LEVELS ];
PDIRENTRY pAlternateStartOfLevel[ MAX_LEVELS ];

LPSTR pszSourceTree;
LPSTR pszTargetFile;

HANDLE hTargetFile;
HANDLE hTargetContext;

CHAR szRootPath[ MAX_NAME_LENGTH ];
CHAR   chVolumeLabel[ 32 + 1 ] =  "CD_ROM";     // mixed case
CHAR  uchVolumeLabel[ 32 + 1 ] =  "CD_ROM";     // upper case
WCHAR wchVolumeLabel[ 32 + 1 ] = L"DVD_ROM";    // mixed case (Unicode)

WCHAR UnicodeRootPath[ MAX_NAME_LENGTH ];

CHAR IsoVolumeCreationTime[ 18 ];       // yyyymmddhhmmsshho

UCHAR cGlobalIsoTimeStamp[ 7 ];
signed char IsoLocalTimeBias;
LONGLONG LocalTimeBiasInFileTimeUnits;

PUCHAR pFileBuffer;

PBNODE pFirstBlockHashTree = NULL;
PBNODE pWholeFileHashTree;

DWORD nDirectories;

DWORD dwHeaderBlocks;
DWORD dwLookupTableSize;
DWORD dwLookupTableAllocation;
DWORD dwJolietLookupTableSize;
DWORD dwJolietLookupTableAllocation;
DWORD dwStartingJolietDirectoryBlock;
DWORD dwStartingIsoDirectoryBlock;
DWORD dwStartingJolietPathTableBlock;
DWORD dwStartingIsoPathTableBlock;
DWORD dwStartingFileBlock;
DWORD dwTotalImageBlocks;
DWORD dwTotalFileBlocks;
DWORD dwSourceSectorSize;
DWORD dwTotalBufferBytes;
DWORD dwReserveBufferBytes;
DWORD dwMaxQueuedWriteBufferBytes;
DWORD dwNextMetaBlockNumber;
DWORD dwMetaSlackStartingBlock;
DWORD dwMetaSlackNumberOfBlocks;
DWORD dwBlockSize = CD_SECTORSIZE;      // default, but may be changed by option
DWORD dwSlackBytesToWriteAtEndOfFiles;
DWORD dwMetaPaddingBeforeDirectoriesBytes;

DWORDLONG dwlMetaPaddingBeforeDirectoriesOffset;
DWORDLONG dwlTotalHeaderBytes;           // dwStartingFileBlock * dwBlockSize
DWORDLONG dwlTotalImageBytes;            // dwTotalImageBlocks * dwBlockSize
DWORDLONG dwlTotalMetaBytes;             // dwlTotalHeaderBytes plus final block CRC bytes if any
DWORDLONG dwlTotalInitialBytes;
DWORDLONG dwlTotalFileSizes;
DWORDLONG dwlTotalNumberOfFiles;
DWORDLONG dwlTotalNumberOfDirectories;
DWORDLONG dwlNextFileScanReportThreshold;
DWORDLONG dwlNextDirectoryScanReportThreshold;
DWORDLONG dwlTotalDupFiles;
DWORDLONG dwlTotalDupFileBytes;          // allocation size
DWORDLONG dwlReportRawSize;
DWORDLONG dwlReportAllocSize;
DWORDLONG dwlShowTreeAlloc;

DWORD    dwStubFileSize;
DWORD    dwStubFileBlock;
LPSTR    StubFileName;
CHAR     StubSourceFile[ MAX_PATH ];
PCHAR    StubFileContents;
FILETIME ftStubTime;

ULONG ElToritoBootSectorFileSize;
PVOID ElToritoBootSectorFileData;
ULONG ElToritoBootCatalogBlock;
ULONG ElToritoBootSectorBlock;
WORD  ElToritoLoadSegment;

ULONG AutoCrcHeaderBlock;
ULONG AutoCrcFinalBlock;

ULONG ulAnchorSectorNumber = 0;
ULONG ulAnchorSlackSector  = 0;

ULONG HeaderCrc = 0xFFFFFFFF;      // initial CRC value
ULONG FileDataCrc;                 // must start at zero (final header crc)

DEBUGCODE( DWORDLONG dwlNextHeaderOffset );
DEBUGCODE( DWORDLONG dwlNextFileDataOffset );


//
//  All these boolean flags are default FALSE
//

BOOL bReportZeroLength;
BOOL bReportExceedLevels;
BOOL bReportAllFiles;
BOOL bReportInvalidDosNames;
BOOL bReportInvalidIsoNames;
BOOL bUseGlobalTime;
BOOL bAllowLongNames;
BOOL bDontUpcase;
BOOL bOptimizeStorage;
BOOL bOptimizeFast;
BOOL bShowDuplicates;
BOOL bOptimizeDiamond;
BOOL bOptimizeFileNames;
BOOL bSuppressOptimizeFileNames;
BOOL bIgnoreMaxImageSize;
BOOL bAllocationSummary;
BOOL bHiddenFiles;
BOOL bUseAnsiFileNames;
BOOL bRestrictToNt3xCompatibleNames;
BOOL bViolatesNt3xCompatibility;
BOOL bEncodeJoliet;
BOOL bGenerateShortNames;
BOOL bEncodeGMT;
BOOL bElTorito;
BOOL bEncodeAutoCrc;
BOOL bCrcCorrectionInTvd;
BOOL bUnicodeNames;
BOOL bScanOnly;
BOOL bSignImage;
BOOL bSignImageAttempt;
BOOL bSignFooter;
BOOL bRoundDirectorySizes;
BOOL bHackI386Directory;
BOOL bAllowExactAlignedDirs;
BOOL bReportBadShortNames;
BOOL bReportBadShortNameWarning;
BOOL bSuppressWarnSameFirstBlockNotIdentical;
BOOL bContinueAfterFailedSourceOpen;
BOOL bOpenReadWrite = FALSE;
BOOL bFastShortNameGeneration;

// Udf options
BOOL bEncodeUdf;
BOOL bEncodeOnlyUdf;
BOOL bUdfEmbedData;
BOOL bUdfSparse;
BOOL bUdfEmbedFID;
BOOL bUdfUseLongADs;
BOOL bUdfUseRandomExtents;
BOOL bUdfVideoZone;
BOOL bUdf8BitFileNames;
BOOL bUdfXbox;

extern USHORT	 CURRENT_UDF_VERSION;
extern DWORDLONG UniqueID;
extern DWORD     ulVATICBLocation;
#ifdef DONTCOMPILE

    BOOL bOneDirectoryRecordPerSector;
    BOOL bOneDirectoryRecordPerTwoSectors;
    BOOL bOptimizeDepthFirst;
    BOOL bOptimizeDepthAfter;
    BOOL bOptimizeReverseSort;
    BOOL bModifyOrder;

    #define MAX_SPECIFIED_DIRS 100

    LPSTR DirectoryFirst[ MAX_SPECIFIED_DIRS ];
    DWORD nDirectoryFirst;

    LPSTR ReverseSort[ MAX_SPECIFIED_DIRS ];
    DWORD nReverseSort;

#endif // DONTCOMPILE

PDIRENTRY pHackI386Directory;

LPSTR ImageSignatureDescription;
MD5_HASH SignatureHash = MD5_INITIAL_VALUE;
UCHAR SignatureVersionText[ 64 ] = CDIMAGE_SIGNATURE_TEXT;

OSVERSIONINFO OsInfo = { sizeof( OSVERSIONINFO ) };

FILETIME ftGlobalFileTime;

union {
    WCHAR W[ MAX_PATH_LENGTH ];        // 128K
     CHAR A[ MAX_PATH_LENGTH ];        // 64K
    } DescentNameBuffer,
      ReportNameBuffer;

union {
    WIN32_FIND_DATAA A;
    WIN32_FIND_DATAW W;
    } FindData;

void DeleteTargetFile( void );
void OpenTargetFile( void );
void CloseTargetFile( void );
void ScanSourceTree( void );
PDIRENTRY AlphabeticalInsertNode( PDIRENTRY pFirst, PDIRENTRY pNew );
void LinkDirectories( PDIRENTRY pDirNode, UINT uLevel );
void ComputeHeaderBlocks( void );
void ComputeDirectorySizes( void );
void ComputeStartingFileBlockLocation( void );
void AssignDirectoryBlockNumbers( void );
void WriteSecondAnchorSector( void );
void GenerateAndWriteLookupTables( void );
void PadCopy( PUCHAR pDest, UINT uLength, LPSTR pSource );
void MakeLittleEndian( PUCHAR pMem, UINT uBytes, DWORD dwValue );
void MakeBigEndian( PUCHAR pMem, UINT uBytes, DWORD dwValue );
void MakeDualEndian( PUCHAR pMem, UINT uBytes, DWORD dwValue );
void WriteDirectories( void );
void WriteFiles( void );
VOID TransferFileThread( PVOID pParam );
VOID UdfTransferFileThread( PVOID pParam );
BOOL TransferAndCheckForDuplicateFast( PDIRENTRY pFile,
                                       DWORDLONG dwlOffset,
                                       HANDLE    hReadContext,
                                       DWORDLONG dwlFileSize );
BOOL TransferAndCheckForDuplicateSlow( PDIRENTRY pFile,
                                       DWORDLONG dwlOffset,
                                       HANDLE    hReadContext,
                                       DWORDLONG dwlFileSize );
LPSTR AllocFullName( PDIRENTRY pFile );
VOID ReadTargetFile( DWORDLONG dwlOffset, PVOID pBuffer, DWORD dwBytesToRead );
VOID MakeFileDuplicate( PDIRENTRY pFile, PDIRENTRY pPrimary );
VOID ReportTransferProgress( DWORDLONG dwlOffset );
VOID ReportHeaderProgress( DWORDLONG dwlOffset );

PUCHAR MakeRootEntries( PUCHAR pMem, PDIRENTRY pDir );
PUCHAR MakeRootEntry( PUCHAR pMem, PDIRENTRY pDir, UCHAR DirName );

PUCHAR MakeJolietRootEntries( PUCHAR pMem, PDIRENTRY pDir );
PUCHAR MakeJolietRootEntry( PUCHAR pMem, PDIRENTRY pDir, UCHAR DirName );

void GetFilePath( PDIRENTRY pDir, LPSTR pBuffer );
void MakeIsoTimeStampFromFileTime( PUCHAR pMem, FILETIME ftFileTime );
void MakeIsoVolumeCreationTime( PCHAR pMem, FILETIME ftFileTime );
BOOL NonDchars( LPSTR pszFileName );
void ParseOptions( int argc, char *argv[] );
void ParseTimeStamp( LPSTR pArg );
void Usage( void );
BOOL IsDigit( char ch );
BOOL IsValidDosName( LPSTR pszFileName );
BOOL IsAcceptableDosName( LPSTR pszFileName );
BOOL IsAcceptableDosNameW( LPWSTR pszFileName );
BOOL IsValidIsoDirectoryName( LPSTR pszFileName );
BOOL IsValidIsoFileName( LPSTR pszFileName );
void DuplicateFileStorageOptimization( void );
void CopyRight( void );
UINT CompareAligned( PVOID p1, PVOID p2, UINT nSize );
BOOL IsFileNameDotOrDotDot( LPCSTR pszFileName );
void ReportAllocationSummary( void );

LPWSTR AnsiToUnicode( IN LPSTR AnsiBuffer, IN OUT OPTIONAL LPWSTR UnicodeBuffer );
LPSTR UnicodeToAnsi( IN LPWSTR UnicodeBuffer, IN OUT OPTIONAL LPSTR AnsiBuffer );
BOOL IsFileNameDotOrDotDotW( LPCWSTR pszUnicodeName );
PDIRENTRY JolietInsertNode( PDIRENTRY pFirst, PDIRENTRY pNew );
void ComputeJolietDirectorySizes( void );
void GenerateShortNames( void );
void GetStubFileContents( void );
void GenerateStubs( void );

void WritePrimaryVolumeDescriptor( ULONG SectorNumber );
void WriteJolietVolumeDescriptor( ULONG SectorNumber );
void WriteElToritoBootVolumeDescriptor( ULONG SectorNumber );
void WriteVolumeDescriptorTerminator( ULONG SectorNumber );
void WriteElToritoBootCatalog( ULONG SectorNumber );
void WriteElToritoBootSector( ULONG SectorNumber );
void UniCopy( PUCHAR pDest, UINT uLength, LPSTR pSource );
void CopyAndInvertUnicode( LPWSTR TargetBuffer, LPWSTR SourceBuffer, ULONG ByteCount );
void GenerateAndWriteJolietLookupTables( void );
void WriteJolietDirectories( void );
void LinkAlternateDirectories( PDIRENTRY pDirNode, UINT uLevel );
PDIRENTRY RecursiveGenerateShortNames( PDIRENTRY pParentDir );
PDIRENTRY GenerateAndInsertShortName( PDIRENTRY pFirstFile, PDIRENTRY pNewFile );
void ComputeAlternateDirectorySizes( void );
void GenerateAndWriteAlternateLookupTables( void );
void WriteAlternateDirectories( void );
void WriteStubs( void );
void ComputeModifiedOrdering( void );

void ShowIsoTree( void );
void ShowJolietTree( void );
void ShowAlternateTree( void );

PDIRENTRY StartRecursiveDescent( void );


VOID EnqueueUdfFiles(PTRANSFER_CONTEXT pTx, PDWORDLONG pdwlTotalSourceBytes);

PDIRENTRY
RecursiveDescent(
    PDIRENTRY pParentDir,
    LPSTR     DescentBufferPointer,
    ULONG     Depth,
    ULONG     NameLengthOfParents
    );

BOOL IsValidJolietFileName( LPWSTR FileName );

PDIRENTRY StartRecursiveDescentJoliet( void );

PDIRENTRY
RecursiveDescentJoliet(
    PDIRENTRY pParentDir,
    LPWSTR    DescentBufferPointer,
    ULONG     Depth,
    ULONG     NameLengthOfParents
    );

LPSTR  GenerateFullName(  PDIRENTRY pDir, LPSTR  pBuffer );
LPWSTR GenerateFullNameW( PDIRENTRY pDir, LPWSTR pBuffer );
LPSTR  GenerateFullNameSource(  PDIRENTRY pDir, LPSTR  pBuffer );
LPWSTR GenerateFullNameSourceW( PDIRENTRY pDir, LPWSTR pBuffer );

VOID ReadElToritoBootSectorFile( PCHAR ElToritoBootSectorFileName );
VOID WriteAutoCrcHeaderBlock( VOID );
VOID WriteAutoCrcFinalBlock( VOID );

VOID
HeaderWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    );

VOID
FileDataWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    );

VOID
LargeDataSplitWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    );

VOID
GenericWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    );

DWORD
AllocateMetaBlocks(
    IN DWORD SizeInBytes
    );

DWORD
AllocateMetaBlocksSectorAligned(
    IN DWORD SizeInBytes
    );

VOID
WriteZeroedSectors(
    IN DWORD SectorNumber,
    IN DWORD NumberOfSectors
    );

VOID
WriteZeroedBlocks(
    IN DWORD BlockNumber,
    IN DWORD NumberOfBlocks
    );

VOID
GenerateSignatureDescription(
    VOID
    );

BOOL
InitSignatureStuff(
    VOID
    );

BOOL
GenerateImageSignature(
    IN  PMD5_HASH HashValue,
    IN  LPSTR     SignatureDescription,
    OUT PDWORD    SignatureIdentfier,
    OUT PUCHAR    SignatureBuffer
    );


VOID
ReportScanProgress(
    VOID
    )
    {
    fprintf( stderr ,
        "\rScanning source tree (%I64d files in %I64d directories) ",
        dwlTotalNumberOfFiles,
        dwlTotalNumberOfDirectories
        );
    fflush( stdout );
    }


VOID
IncrementTotalNumberOfDirectories(
    VOID
    )
    {
    if ( ++dwlTotalNumberOfDirectories >= dwlNextDirectoryScanReportThreshold ) {
        dwlNextDirectoryScanReportThreshold += SCAN_REPORT_INCREMENT;
        ReportScanProgress();
        }
    }


VOID
IncrementTotalNumberOfFiles(
    VOID
    )
    {
    if ( ++dwlTotalNumberOfFiles >= dwlNextFileScanReportThreshold ) {
        dwlNextFileScanReportThreshold += SCAN_REPORT_INCREMENT;
        ReportScanProgress();
        }
    }


//////////////////////////////////////////////////////////////////////////////

#ifdef DONTCOMPILE

void ShowTree( PDIRENTRY pFirstNode, UINT uLevel ) {

    PDIRENTRY p;

    for ( p = pFirstNode; p != NULL; p = p->pNextTargetFile ) {

        printf( "%*s%s %9I64d bytes %9d block-location\r\n",
                uLevel * 2,
                "",
                p->pszFileName,
                p->dwlFileSize,
                p->dwStartingBlock );

        if ( p->dwFlags & IS_DIRECTORY ) {
            ASSERT( p->pDirectoryInfo != NULL );
            ShowTree( p->pDirectoryInfo->pFirstTargetFile, uLevel + 1 );
            }
        }
    }

#endif // DONTCOMPILE

#ifdef DONTCOMPILE

VOID
TestMD5(
    VOID
    );

#endif // DONTCOMPILE


void _cdecl main( int argc, char *argv[] ) {

    SYSTEMTIME st;
    ULONG SectorNumber;
    int i;

//  TestMD5();

    CopyRight();

    //
    //  CDIMAGE makes extensive use of 64-bit OVERLAPPED file i/o, which is
    //  currently only supported on WinNT, and not on Win95 or Win32s.
    //

    GetVersionEx( &OsInfo );

    //
    //  CDIMAGE requires NT 4.0 and will not work on NT 3.51 or Win95.
    //

    if (( OsInfo.dwPlatformId != VER_PLATFORM_WIN32_NT ) ||
        ( OsInfo.dwMajorVersion < 4 )) {

        printf( CDIMAGE_NAME " requires Windows NT 4.0 or Windows 2000\r\n" );
        fflush( stdout );
        exit( 1 );
        }

    SetErrorMode( SEM_FAILCRITICALERRORS );

    InitializeErrorHandler();

    InitializeRecyclePackage();

    MyHeapInit();

    InitializeNtDllPointers();  //  CDIMAGE will still work even if this fails,
                                //  although GetSectorSize() will have to take
                                //  the slow path-name-based route, and
                                //  MyIsDebuggerPresent will always return
                                //  FALSE.

    //
    //  To make two subsequently created images of the same tree of files
    //  the same, chop the time portion of the stamp to midnight.  This will
    //  cause the root directory to get the same time stamp twice on two
    //  subsequently created images on the same day.
    //

    GetSystemTime( &st );

    st.wHour         = 0;
    st.wMinute       = 0;
    st.wSecond       = 0;
    st.wMilliseconds = 0;

    SystemTimeToFileTime( &st, &ftGlobalFileTime );

    ParseOptions( argc, argv );     // this might change ftGlobalFileTime

    if ( bUseAnsiFileNames )
        SetFileApisToANSI();
    else
        SetFileApisToOEM();


    if ( bEncodeGMT ) {

        IsoLocalTimeBias = 0;
        NsrLocalTimeBias = 0;
        LocalTimeBiasInFileTimeUnits = 0;

        }

    else {

        TIME_ZONE_INFORMATION tz;
        LONG CurrentBias = 0;

        switch( GetTimeZoneInformation( &tz )) {

            case TIME_ZONE_ID_STANDARD:

                CurrentBias = tz.Bias + tz.StandardBias;
                break;

            case TIME_ZONE_ID_DAYLIGHT:

                CurrentBias = tz.Bias + tz.DaylightBias;
                break;

            case TIME_ZONE_ID_UNKNOWN:

                CurrentBias = tz.Bias;
                break;

            default:

                ErrorExit(
                    GETLASTERROR,
                    "\r\nERROR: Unable to determine time zone bias\r\n"
                    );

            }

        NsrLocalTimeBias = (short) -( CurrentBias );

        ASSERT(( NsrLocalTimeBias >= TIMESTAMP_Z_MIN ) &&
               ( NsrLocalTimeBias <= TIMESTAMP_Z_MAX ));

        // IsoLocalTimeBias is number of 15 minute intervals from -48 (west) to +52 (east)

        ASSERT((( -( CurrentBias ) / 15 ) >= -48 ) && (( -( CurrentBias ) / 15 ) <= +52 ));

        IsoLocalTimeBias = (signed char)( -( CurrentBias ) / 15 );

        ASSERT(( IsoLocalTimeBias >= -48 ) && ( IsoLocalTimeBias <= +52 ));

        if (( IsoLocalTimeBias < -48 ) || ( IsoLocalTimeBias > +52 )) {
            IsoLocalTimeBias = 0;
            }

        LocalTimeBiasInFileTimeUnits = (LONGLONG) IsoLocalTimeBias      // 15 minute intervals
                                     * (LONGLONG) 15                    // minutes
                                     * (LONGLONG) 60                    // seconds
                                     * (LONGLONG) 1000                  // milliseconds
                                     * (LONGLONG) 1000                  // microseconds
                                     * (LONGLONG) 10;                   // 100-nanoseconds

        if ( bUseGlobalTime ) {

            //
            //  User-specified time is in local time, but we need it
            //  to be system time.
            //

            *(UNALIGNED DWORDLONG*)&ftGlobalFileTime -= LocalTimeBiasInFileTimeUnits;

            }
        }

    MakeIsoTimeStampFromFileTime( cGlobalIsoTimeStamp, ftGlobalFileTime );
    MakeNsrTimeStampFromFileTime( &GlobalNsrTimeStamp, ftGlobalFileTime );

    MakeIsoVolumeCreationTime( IsoVolumeCreationTime, ftGlobalFileTime );

    i = 1;

    while (( i < argc ) && ( argv[ i ] == NULL ))
        ++i;

    if ( i < argc ) {

        pszSourceTree = argv[ i++ ];

        while (( i < argc ) && ( argv[ i ] == NULL ))
            ++i;

        if ( i < argc ) {

            pszTargetFile = argv[ i ];

            if ( bScanOnly )
                Usage();

            }
        else {

            if ( ! bScanOnly )
                Usage();

            }
        }
    else {
        Usage();
        }


#ifndef DVDIMAGE_ONLY

    if ( bSignImage ) {
        if ( ! InitSignatureStuff() ) {
            bSignImage = FALSE;
            }
        }

    if ( bSignImage ) {
        bEncodeAutoCrc = TRUE;     // BUGBUG: requirement for now, so force on
        }

#endif // ! DVDIMAGE_ONLY


    InitializeThreadPool(( bOptimizeStorage ) && ( ! bOptimizeFast ) ? 5 : 4 );

    dwTotalBufferBytes = AdjustWorkingSetAndLockImage(
                             bOptimizeStorage ? 0x200000 : 0x100000,
                             FX_BLOCKSIZE
                             );

    if ( ! bScanOnly )
        DeleteTargetFile();

    printf( "\r\nScanning source tree " );
    fflush( stdout );

    dwlNextFileScanReportThreshold      = SCAN_REPORT_INCREMENT;
    dwlNextDirectoryScanReportThreshold = SCAN_REPORT_INCREMENT;

    ScanSourceTree();

    printf( "\rScanning source tree complete (%I64d files in %I64d directories)\r\n",
            dwlTotalNumberOfFiles,
            dwlTotalNumberOfDirectories
            );
    fflush( stdout );

    printf( "\r\nComputing directory information " );
    fflush( stdout );

//  ShowTree( pRootDir, 0 );

    LinkDirectories( pRootDir, 0 );

#ifdef DONTCOMPILE
    if ( bModifyOrder )
        ComputeModifiedOrdering();
#endif // DONTCOMPILE

    ComputeHeaderBlocks();      // ISO-9660 Volume Descriptor sequence

    if( bElTorito ) {
        ElToritoBootCatalogBlock = AllocateMetaBlocksSectorAligned( 1 );
        }

    if ( ! bEncodeOnlyUdf ) {

        if ( bEncodeJoliet ) {

            if ( bGenerateShortNames )
                GenerateShortNames();
            else
                GenerateStubs();

            ComputeJolietDirectorySizes();

            ComputeAlternateDirectorySizes();

            }

        else {

            if( bEncodeUdf ) {
			
    		    GenerateShortNames();
            }

            ComputeDirectorySizes();

            }
        }

    else {

        GenerateStubs();

        ComputeAlternateDirectorySizes();

        }

    AssignDirectoryBlockNumbers();

    if ( bEncodeUdf ) {
        if (bUdfVideoZone)
        {
            SetCurrentUDFVersion(0x102);
            printf("\nUDF Video Zone Compatibility - Setting version to 1.02");
        }
        else
		    SetCurrentUDFVersion(0x150);

		ASSERT( dwBlockSize == CD_SECTORSIZE );

        if ( dwNextMetaBlockNumber < ANCHOR_SECTOR ) {

            //
            //  Waste remaining sectors up to ANCHOR_SECTOR.
            //

            ASSERT( dwMetaSlackStartingBlock == 0 );

            dwMetaSlackStartingBlock  = dwNextMetaBlockNumber;
            dwMetaSlackNumberOfBlocks = ANCHOR_SECTOR - dwMetaSlackStartingBlock;
            dwNextMetaBlockNumber     = ANCHOR_SECTOR + 1;

            DEBUGCODE( printf( "\r\nWARNING: UDF meta data has %d sectors of slack space\r\n",
                               dwMetaSlackNumberOfBlocks ));

            }

        else {

            //
            //  Even if dwMetaSlackNumberOfBlocks is zero,
            //  dwMetaSlackStartingBlock should be non-zero in this
            //  case, indicating the anchor has been correctly skipped.
            //

            ASSERT( dwMetaSlackStartingBlock != 0 );

            }

        ComputeUDFDirectorySizes();
        AssignUDFDirectoryBlockNumbers();

        }

    if( bElTorito ) {
        ElToritoBootSectorBlock = AllocateMetaBlocksSectorAligned( ElToritoBootSectorFileSize );
        }

    ASSERT( IS_SECTOR_ALIGNED( dwNextMetaBlockNumber * dwBlockSize ));

    if ( bEncodeAutoCrc ) {
        AutoCrcHeaderBlock = AllocateMetaBlocksSectorAligned( 1 );
        }

    ComputeStartingFileBlockLocation();

    if ( bEncodeUdf )
        ComputeUDFPartitionLength();

    printf( "\rComputing directory information complete\r\n" );
    fflush( stdout );

    printf( "\r\nImage file %s %I64d bytes %s\r\n",
            bScanOnly ? "would be" : "is",
            dwlTotalImageBytes,
            bOptimizeStorage ? "(before optimization)" : "" );
    fflush( stdout );

    if (( ! bIgnoreMaxImageSize ) &&
        ( ! bOptimizeStorage    ) &&
        ( dwlTotalImageBytes > MAX_IMAGE_BYTES )) {

        ErrorExit(
            0,
            "ERROR: Image is %I64d bytes too large for 74-minute CD (" MAX_IMAGE_BYTES_TEXT ")\r\n"
            "       (use -m to override%s\r\n",
            dwlTotalImageBytes - MAX_IMAGE_BYTES,
            " or try -o to optimize storage)"
            );
        }

    if (( bScanOnly ) && ( ! bOptimizeStorage )) {

        printf( "\r\nNo image file created (-q option selected)\r\n" );
        printf( "Done.\r\n" );
        fflush( stdout );

        if ( bAllocationSummary ) {
            ReportAllocationSummary();
            }

        exit( 0 );
        }

    OpenTargetFile();

    if ( bScanOnly ) {
        printf( "\r\nReading %I64d files in %I64d directories\r\n",
                dwlTotalNumberOfFiles,
                dwlTotalNumberOfDirectories
                );
        }
    else {
        printf( "\r\nWriting %I64d files in %I64d directories to %s\r\n",
                dwlTotalNumberOfFiles,
                dwlTotalNumberOfDirectories,
                pszTargetFile
                );
        }

    printf( "\r\n" );

    fflush( stdout );

    if (( bOptimizeStorage ) && ( ! bScanOnly )) {

        if ( bOptimizeFast ) {

            //
            //  Need to maintain three free read buffers for
            //  optimal read-ahead speed while queueing as
            //  many writes as possible.
            //

            dwReserveBufferBytes = 3 * FX_BLOCKSIZE;

            }

        else {

            //
            //  For slow optimization where we're reading both
            //  the source file and it's suspect duplicate, we
            //  need twice as many free read-ahead buffers.
            //

            dwReserveBufferBytes = 6 * FX_BLOCKSIZE;

            }
        }

    else {

        dwReserveBufferBytes = 0;

        }

    dwTotalBufferBytes = InitializeBufferAllocator(
                             dwTotalBufferBytes,
                             dwReserveBufferBytes + ( 2 * FX_BLOCKSIZE ),
                             FX_BLOCKSIZE
                             );

    dwMaxQueuedWriteBufferBytes = dwTotalBufferBytes - dwReserveBufferBytes;

    WriteFiles();

  
    DEBUGCODE( printf("\r\nDone transfering files - writing directory information\r\n" ));

#ifdef DONTCOMPILE

    printf( "\r\n" );

    if ( bEncodeJoliet ) {
        ShowJolietTree();
        ShowAlternateTree();
        if ( bGenerateShortNames )
            ShowIsoTree();
        }

    printf( "\r\n" );

#endif // DONTCOMPILE

    WriteZeroedSectors( 0, 16 );    // initial 16 sectors of zeros

    SectorNumber = 16;              // start of volume descriptor sequence

    WritePrimaryVolumeDescriptor( SectorNumber++ );

    if ( bElTorito )
        WriteElToritoBootVolumeDescriptor( SectorNumber++ );

    if ( bEncodeJoliet )
        WriteJolietVolumeDescriptor( SectorNumber++ );

    WriteVolumeDescriptorTerminator( SectorNumber++ );

    if ( bEncodeUdf ) {

        //
        //  UDF volume recognition sequence must follow immediately
        //  behind ISO-9660 volume descriptor terminator.
        //

        WriteBEA01Descriptor( SectorNumber++ );
		WriteNSRDescriptor(SectorNumber++);
        WriteTEA01Descriptor( SectorNumber++ );

        }

    if ( bElTorito ) {

        WriteElToritoBootCatalog( SectorNumber++ );
        }

    if ( bEncodeJoliet ) {
        GenerateAndWriteJolietLookupTables();
        }

    if ( bEncodeJoliet || bEncodeOnlyUdf ) {
        GenerateAndWriteAlternateLookupTables();
        }
    else {
        GenerateAndWriteLookupTables();
        }


    if (( bEncodeOnlyUdf ) || (( bEncodeJoliet ) && ( ! bGenerateShortNames ))) {
        ASSERT( dwStubFileBlock != 0 );
        WriteStubs();
        }
    else {
        ASSERT( dwStubFileBlock == 0 );
        }

    if ( dwlMetaPaddingBeforeDirectoriesOffset ) {

        PVOID pBuffer = AllocateBuffer( dwMetaPaddingBeforeDirectoriesBytes, TRUE );

        HeaderWrite( dwlMetaPaddingBeforeDirectoriesOffset, pBuffer, dwMetaPaddingBeforeDirectoriesBytes );

        }


    ASSERT( IS_SECTOR_ALIGNED( dwlNextHeaderOffset ));


    if ( ! bEncodeOnlyUdf ) {

        if ( bEncodeJoliet ) {

            WriteJolietDirectories();

            WriteAlternateDirectories();

            }

        else {

            WriteDirectories();

            }
        }

    else {

        WriteAlternateDirectories();

        }


    if ( bEncodeUdf ) {

        ASSERT( dwBlockSize == CD_SECTORSIZE );

        if ( dwMetaSlackNumberOfBlocks != 0 ) {
            ASSERT( dwMetaSlackStartingBlock + dwMetaSlackNumberOfBlocks == ANCHOR_SECTOR );
            WriteZeroedBlocks( dwMetaSlackStartingBlock, dwMetaSlackNumberOfBlocks );
            }

        // write UDF Anchor Pointer and Volume Descriptor Sequences

        // BUGBUG - write directories first because partition size might change
        WriteUDFDirectories();

        WriteUDFAnchorAndVDS();

		
		ulAnchorSectorNumber = dwTotalImageBlocks-1;
      

        WriteSecondAnchorSector();
		
	    }

    if ( bElTorito ) {
        WriteElToritoBootSector( ElToritoBootSectorBlock );
        }

    if ( bEncodeAutoCrc ) {
        ASSERT( ! bEncodeUdf );     // UDF writes sectors out of sequential order
        WriteAutoCrcHeaderBlock();
        }

    if (( bEncodeAutoCrc ) || ( bSignImage )) {
        WriteAutoCrcFinalBlock();
        }

    if ( bReportAllFiles )
        printf( "\r\n" );
    else
        fprintf( stderr, "\r100%% complete\r\n" );

    fflush( stdout );

    if ( bSignImage ) {
        printf( "\r\nImage contains digital signature\r\n" );
        fflush( stdout );
        }

    if ( bOptimizeStorage ) {

        printf( "\r\nStorage optimization saved %I64d files, %I64d bytes (%d%% of image)\r\n",
                dwlTotalDupFiles,
                dwlTotalDupFileBytes,
                ( 100 - (DWORD)(( dwlTotalImageBytes * 100 ) / dwlTotalInitialBytes )));

        fflush( stdout );

//
//  Note that percent of bytes in files is a more impressive figure, but less
//  meaningful that percent of bytes in image.
//
//              (DWORD)(((DWORDLONG)dwTotalDupFileBytes * 100 ) / ( dwTotalFileBlocks * dwBlockSize )));
//

        printf( "\r\nAfter optimization, image file %s %I64d bytes\r\n",
                bScanOnly ? "would be" : "is",
                dwlTotalImageBytes
              );

        fflush( stdout );

        if (( ! bIgnoreMaxImageSize ) && ( dwlTotalImageBytes > MAX_IMAGE_BYTES )) {

            ErrorExit(
                0,
                "ERROR: Image is %I64d bytes too large for 74-minute CD (" MAX_IMAGE_BYTES_TEXT ")\r\n"
                "       (use -m to override%s\r\n",
                dwlTotalImageBytes - MAX_IMAGE_BYTES,
                ")"
                );
            }
        }

    else {

        printf( "\r\nFinal image file %s %I64d bytes\r\n",
                bScanOnly ? "would be" : "is",
                dwlTotalImageBytes
              );

        }

    DEBUGCODE( printf( "\r\nFlushing buffers\r\n" ));

    CloseTargetFile();

    DEBUGCODE( printf( "\r\nMaxBtreeDepth=%d\r\n", nMaxBtreeDepth ));

    if ( bViolatesNt3xCompatibility ) {

        printf(
            "\r\n"
            "WARNING: This image contains filenames and/or directory names that are\r\n"
            "         NOT COMPATIBLE with Windows NT 3.51.  If compatibility with\r\n"
            "         Windows NT 3.51 is required, use the -nt switch rather than\r\n"
            "         the -n switch.\r\n"
            "\r\n"
            );
        fflush( stdout );
        }

    if ( bReportBadShortNameWarning ) {
        printf(
            "\r\n"
            "WARNING: This image contains filenames and/or directory names that might be\r\n"
            "         inaccessible to 16-bit applications running under Windows NT 4.0\r\n"
            "         without Service Pack 2+.  An obscure bug in Windows NT 4.0 prevents\r\n"
            "         16-bit apps from opening some enumerated files and directories that\r\n"
            "         have long names when the hexadecimal \"uniquifier\" of the generated\r\n"
            "         short name contains non-numeric characters (a function of the offset\r\n"
            "         of the directory entry within the directory).  This problem only\r\n"
            "         occurs for 16-bit apps that try to open enumerated files on the CD,\r\n"
            "         not for 16-bit apps that open specific files according to a list of\r\n"
            "         filenames such as most 16-bit setup applications.  For more info\r\n"
            "         about this particular issue, email \"cdmakers\".\r\n"
            "\r\n"
            );
        fflush( stdout );
        }

    if ( bEncodeUdf ) {

        printf(
            "\r\n"
            "WARNING: This image contains UDF file system structures.\r\n"
            "\r\n"
            );
        if(bUdfEmbedData || bUdfSparse || bUdfEmbedFID || bOptimizeStorage)
            printf("Space saved because of embedding, sparseness or optimization = %u\r\n",gdwSpaceSaved);

        if(bUdfSparse)
            printf("\r\nWARNING: This image may be unusable on Win9x due to possible sparse files\r\n");

        fflush( stdout );
        }

    if ( bSignImageAttempt && ! bSignImage ) {

        printf(
            "\r\n"
            "WARNING: IMAGE DOES NOT CONTAIN DIGITAL SIGNATURE AS REQUESTED.\r\n"
            "\r\n"
            );
        fflush( stdout );
        }

    if ( bScanOnly ) {
        printf( "\r\nNo image file created (-q option selected)\r\n" );
        }

    printf( "\r\nDone.\r\n" );
    fflush( stdout );

    if ( bAllocationSummary ) {
        ReportAllocationSummary();
        }

    exit( 0 );

    }


void CopyRight( void ) {

    printf( "\r\n"

#ifdef DVDIMAGE_ONLY

            "%s DVD-ROM Premastering Utility\r\n"

#else   // ! DVDIMAGE_ONLY

            "%s CD-ROM and DVD-ROM Premastering Utility\r\n"

#endif // DVDIMAGE_ONLY

            "Copyright (C) Microsoft, 1993-2001.  All rights reserved.\r\n"
#ifdef XBOX
            "For Xbox Use Only\r\n"
#else  // XBOX
            "For Microsoft internal use only.\r\n"
#endif // XBOX
            "\r\n",
            MAJOR_VERSION_TEXT
          );

#ifdef PRIVATE_BUILD_TEXT
    printf( PRIVATE_BUILD_TEXT );
#endif

    fflush( stdout );

    }


VOID
AbortTargetFile(
    PVOID pContext
    )
    {
    if ( hTargetFile != NULL )
        AbortWriter( (HANDLE)pContext );
    }


void OpenTargetFile( void ) {

    dwlTotalInitialBytes = dwlTotalImageBytes;

    if ( ! bScanOnly ) {

        hTargetContext = CreateWriteContext( pszTargetFile,         // file name
                                             0,                     // sector size
                                             dwlTotalInitialBytes,  // initial size
                                             MAX_QUEUED_WRITES );   // concurrent

        if ( hTargetContext == NULL ) {
            ErrorExit( GETLASTERROR,
                       "ERROR: Unable to create file \"%s\" of %I64d bytes\r\n",
                       pszTargetFile,
                       dwlTotalInitialBytes );
            }


        hTargetFile = GetFileHandleFromWriteContext( hTargetContext );

        //
        //  NOTE:  This app assumes that a sector size of dwBlockSize (512 or 2048)
        //         is sufficient alignment for overlapped/unbuffered i/o on
        //         the target image file.  It's ok for the target image file's
        //         sector alignment requirement to be smaller than dwBlockSize, but
        //         it cannot be larger than dwBlockSize since we do writes to the
        //         target in dwBlockSize aligned offsets and lengths.
        //

#ifdef DEBUG
        {
        DWORD dwSectorSize;

        dwSectorSize = GetSectorSize( hTargetFile, NULL, pszTargetFile );

        ASSERT( dwSectorSize != 0 );
        ASSERT( dwSectorSize <= dwBlockSize );
        ASSERT( ISALIGN2( dwBlockSize, dwSectorSize ));
        }
#endif

        RegisterTerminationHandler( AbortTargetFile, (PVOID)hTargetContext );

        }
    }


void CloseTargetFile( void ) {
    DWORDLONG dwlLength;
    hTargetFile = NULL;
    dwlLength=(DWORDLONG) dwTotalImageBlocks * (DWORDLONG) dwBlockSize;
    ASSERT( IS_SECTOR_ALIGNED( dwlLength ));
    CloseWriteContext( hTargetContext, dwlLength);
    }


PDIRENTRY
NewDirNode(
    IN BOOL IsDirectory
    )
    {
    PDIRENTRY NewNode = MyAllocNeverFree( sizeof( DIRENTRY ));

    if ( IsDirectory ) {
        NewNode->dwFlags = IS_DIRECTORY;
        NewNode->pDirectoryInfo = MyAllocNeverFree( sizeof( DIRECTORY_INFO ));
        }

    if ( ! bUseGlobalTime )
        NewNode->pFileTimeInfo = MyAllocNeverFree( sizeof( FILETIME_INFO ));

    if ( bEncodeUdf )
    {
        NewNode->pUdfInfo = MyAllocNeverFree( sizeof( UDFINFO ));
        NewNode->pUdfInfo->ulICBBlockNumber=0;
        NewNode->pUdfInfo->ulLinkCount=0;
    }

    return NewNode;
    }


VOID DetermineRootPath( VOID ) {

    WCHAR  UnicodeBuffer[ MAX_NAME_LENGTH ];
    WCHAR  UnicodeSource[ MAX_NAME_LENGTH ];
    CHAR   AnsiBuffer[ MAX_NAME_LENGTH ];
    ULONG  Length;
    LPSTR  NotUsed;
    LPWSTR NotUsedW;

    *AnsiBuffer = 0;        // in case GetFullPathName fails

    GetFullPathNameA(
        pszSourceTree,
        MAX_NAME_LENGTH,
        AnsiBuffer,
        &NotUsed
        );

    Length = strlen( AnsiBuffer );

    if ( Length == 0 ) {
        strcpy( AnsiBuffer, "." );
        Length = 1;
        }

    if (( AnsiBuffer[ Length - 1 ] != '\\' ) &&
        ( AnsiBuffer[ Length - 1 ] != ':' )) {
        strcat( AnsiBuffer, "\\" );
        }

    strcpy( szRootPath, AnsiBuffer );

    if ( bUnicodeNames ) {

        AnsiToUnicode( pszSourceTree, UnicodeSource );

        *UnicodeBuffer = 0;     // in case GetFullPathName fails

        GetFullPathNameW(
            UnicodeSource,
            MAX_NAME_LENGTH,
            UnicodeBuffer,
            &NotUsedW
            );

        Length = lstrlenW( UnicodeBuffer );

        if ( Length == 0 ) {
            lstrcpyW( UnicodeBuffer, (LPCWSTR)L"." );
            Length = 1;
            }

        if (( UnicodeBuffer[ Length - 1 ] != L'\\' ) &&
            ( UnicodeBuffer[ Length - 1 ] != L':' )) {
            lstrcatW( UnicodeBuffer, (LPCWSTR)L"\\" );
            }

        lstrcpyW( UnicodeSource, (LPCWSTR)L"\\\\?\\" );

        if (( wcsncmp( UnicodeBuffer, (LPCWSTR)L"\\\\",    2 ) == 0 ) &&
            ( wcsncmp( UnicodeBuffer, (LPCWSTR)L"\\\\.\\", 4 ) != 0 )) {

            lstrcatW( UnicodeSource, (LPCWSTR)L"UNC" );
            lstrcatW( UnicodeSource, UnicodeBuffer + 1 );

            }

        else {

            lstrcatW( UnicodeSource, UnicodeBuffer );

            }

        lstrcpyW( UnicodeRootPath, UnicodeSource );

        }

    }


void ScanSourceTree( void ) {

    HANDLE hFind;

    DetermineRootPath();

    if ( ! bDontUpcase )
        strupr( szRootPath );

    pRootDir = NewDirNode( TRUE );
    pRootDir->pszFileName         = "";
    pRootDir->pszUnicodeName      = (LPWSTR)L"";
    pRootDir->wFileNameLength     = 1;
    pRootDir->wUnicodeNameLength  = 1;
    pRootDir->pParentDir          = pRootDir;

    ASSERT( pRootDir->pDirectoryInfo != NULL );

    if ( ! bUseGlobalTime ) {

        ASSERT( pRootDir->pFileTimeInfo != NULL );

        //
        //  Note that the Joliet root directory name is also a single byte.
        //  For both cases, the name of the root directory is a single 0x00.
        //

        if ( bUnicodeNames ) {

            hFind = FindFirstFileW( UnicodeRootPath, &FindData.W );

            if ( hFind == INVALID_HANDLE_VALUE ) {

                //
                //  Try removing trailing backslash if not "c:\"
                //

                LPWSTR p = wcschr( UnicodeRootPath, 0 ) - 1;

                if (( *p == L'\\' ) && ( *( p - 1 ) != L':' )) {

                    *p = 0;     // truncate trailing backslash

                    hFind = FindFirstFileW( UnicodeRootPath, &FindData.W );

                    *p = '\\';  // replace trailing backslash

                    }
                }
            }

        else {

            hFind = FindFirstFileA( szRootPath, &FindData.A );

            if (( hFind == INVALID_HANDLE_VALUE ) && ( strlen( szRootPath ) > 1 )) {

                //
                //  Try removing trailing backslash if not "c:\"
                //

                LPSTR p = strchr( szRootPath, 0 ) - 1;

                if (( *p == '\\' ) && ( *( p - 1 ) != ':' )) {

                    *p = 0;     // truncate trailing backslash

                    hFind = FindFirstFileA( szRootPath, &FindData.A );

                    *p = '\\';  // replace trailing backslash

                    }
                }
            }


        if ( hFind != INVALID_HANDLE_VALUE ) {
            FindClose( hFind );
            if ( bUnicodeNames ) {
                pRootDir->pFileTimeInfo->ftLastWriteTime  = FindData.W.ftLastWriteTime;
                pRootDir->pFileTimeInfo->ftLastAccessTime = FindData.W.ftLastAccessTime;
                pRootDir->pFileTimeInfo->ftCreationTime   = FindData.W.ftCreationTime;
                }
            else {
                pRootDir->pFileTimeInfo->ftLastWriteTime  = FindData.A.ftLastWriteTime;
                pRootDir->pFileTimeInfo->ftLastAccessTime = FindData.A.ftLastAccessTime;
                pRootDir->pFileTimeInfo->ftCreationTime   = FindData.A.ftCreationTime;
                }
            }
        else {
            pRootDir->pFileTimeInfo->ftLastWriteTime  = ftGlobalFileTime;
            pRootDir->pFileTimeInfo->ftLastAccessTime = ftGlobalFileTime;
            pRootDir->pFileTimeInfo->ftCreationTime   = ftGlobalFileTime;
            }

        }


    if ( bUnicodeNames ) {
        pRootDir->pDirectoryInfo->pFirstTargetFile = StartRecursiveDescentJoliet();
        }
    else {
        pRootDir->pDirectoryInfo->pFirstTargetFile = StartRecursiveDescent();
        }

    if ( pRootDir->pDirectoryInfo->pFirstTargetFile == NULL ) {
        ErrorExit( 0, "ERROR: No files found in \"%s\"\r\n", szRootPath );
        }
    }


PDIRENTRY AlphabeticalInsertNode( PDIRENTRY pFirst, PDIRENTRY pNew ) {

    PDIRENTRY pPrev, pNext;
    PCHAR p, q;
    INT iCmp;

    if (( p = strchr( pNew->pszFileName, '.' )) != NULL )
        *p = 0x01;      // for sorting purposes

    for ( pPrev = NULL, pNext = pFirst;
          pNext != NULL;
          pPrev = pNext, pNext = pNext->pNextTargetFile ) {

        if (( q = strchr( pNext->pszFileName, '.' )) != NULL )
            *q = 0x01;      // for sorting purposes

        iCmp = strcmp( pNew->pszFileName, pNext->pszFileName );

        if ( q != NULL )
            *q = '.';       // restore character changed for sorting

        if ( iCmp < 0 )
            break;

        }

    if ( p != NULL )
        *p = '.';       // restore character changed for sorting

    pNew->pNextTargetFile = pNext;

    if ( pPrev == NULL )
        return pNew;
    else {
        pPrev->pNextTargetFile = pNew;
        return pFirst;
        }

    }


void LinkDirectories( PDIRENTRY pDirNode, UINT uLevel ) {

    PDIRENTRY p;

    if ( uLevel >= MAX_LEVELS ) {
        ErrorExit( 0, "ERROR: Directory depth greater than %d levels.\r\n", MAX_LEVELS );
        }

    p = pStartOfLevel[ uLevel ];

    if ( p == NULL )
        pStartOfLevel[ uLevel ] = pDirNode;
    else {
        ASSERT( p->pDirectoryInfo != NULL );
        while ( p->pDirectoryInfo->pNextTargetDir != NULL ) {
            p = p->pDirectoryInfo->pNextTargetDir;
            ASSERT( p->pDirectoryInfo != NULL );
            }
        p->pDirectoryInfo->pNextTargetDir = pDirNode;
        }

    for ( p = pDirNode->pDirectoryInfo->pFirstTargetFile; p != NULL; p = p->pNextTargetFile ) {
        if ( p->dwFlags & IS_DIRECTORY ) {
            ASSERT( p->pDirectoryInfo != NULL );
            LinkDirectories( p, uLevel + 1 );
            }
        }
    }


void ComputeDirectorySizes( void ) {

    PDIRENTRY pDir, pFile;
    DWORD dwDirOffset, dwTableOffset, dwNextBound, dwNextLimit, dwEntryLen;
    UINT uLevel;
    CHAR OddNameBuffer[ 224 ];      // 221 is absolute max name length
    CHAR EvenNameBuffer[ 224 ];
    UINT NameIndex = 0;

    dwTableOffset = 0;
    nDirectories = 0;

    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {

        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );

            //
            //  Each table entry is 8 bytes plus the length of the name,
            //  but the next entry must start at a word (2-byte) boundary,
            //  so round-up offset if necessary.
            //

            ASSERT( pDir->wFileNameLength != 0 );

            if (( dwTableOffset += ( 8 + pDir->wFileNameLength )) & 1 )
                ++dwTableOffset;

            pDir->pDirectoryInfo->dwDirIndex = ++nDirectories;

            if ( pDir->pParentDir->pDirectoryInfo->dwDirIndex > 0xFFFF ) {

                ErrorExit(
                    0,
                    "ERROR: Too many directories in volume (directory number of a parent directory\r\n"
                    "       cannot exceed 65535 because it is stored in a 16-bit field).\r\n"
                    );
                }

#ifdef DONTCOMPILE

            if ( bOneDirectoryRecordPerSector || bOneDirectoryRecordPerTwoSectors ) {
                dwDirOffset = CD_SECTORSIZE * 2 * ( bOneDirectoryRecordPerTwoSectors ? 2 : 1 );
                for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {
                    dwDirOffset += CD_SECTORSIZE * ( bOneDirectoryRecordPerTwoSectors ? 2 : 1 );
                    }
                }
            else

#endif // DONTCOMPILE

                {

                dwDirOffset = 34 * 2;               // for "." and ".." entries
                dwNextBound = CD_SECTORSIZE;        // don't straddle sector boundary

                if ( bReportBadShortNames ) {
                    ZeroMemory( OddNameBuffer,  sizeof( OddNameBuffer ));
                    ZeroMemory( EvenNameBuffer, sizeof( EvenNameBuffer ));
                    NameIndex = 2;                  // for . and .. entries
                    }

                if ( bAllowExactAlignedDirs )
                    dwNextLimit = CD_SECTORSIZE;     // more efficient packing
                else
                    dwNextLimit = CD_SECTORSIZE - 1; // don't break MSCDEX

                for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {

                    ASSERT( pFile->wFileNameLength != 0 );
                    ASSERT( pFile->wFileNameLength <= 221 );

                    dwEntryLen  = pFile->wFileNameLength + 33;
                    dwEntryLen += dwEntryLen & 1;

                    if (( dwDirOffset + dwEntryLen ) > dwNextLimit ) {
                        dwDirOffset  = dwNextBound;
                        dwNextBound += CD_SECTORSIZE;
                        dwNextLimit += CD_SECTORSIZE;
                        }

                    DEBUGCODE( pFile->dwDirOffset = dwDirOffset );

                    if ( bReportBadShortNames ) {

                        //
                        //  Check for potential conflict with bug in NT 4.0
                        //  where CDFS shortname generation fails when the hex
                        //  offset of the direntry shifted right 5 bits
                        //  contains hex characters 'a' through 'f' (non-
                        //  numeric hex characters).
                        //

                        PCHAR WhichNameBuffer = ( NameIndex++ & 1 ) ? OddNameBuffer : EvenNameBuffer;
                        BOOL  CheckShortName  = ( pFile->dwFlags & CHECK_SHORTNAME ) ? TRUE : FALSE;
                        BOOL  IsDirectory     = ( pFile->dwFlags & IS_DIRECTORY    ) ? TRUE : FALSE;

                        if ( ! CheckShortName ) {

                            if ( bOptimizeFileNames || IsDirectory ) {

                                if (( WhichNameBuffer[ pFile->wFileNameLength ] == '.' ) ||
                                    ( WhichNameBuffer[ pFile->wFileNameLength ] == ' ' )) {

                                    CheckShortName = TRUE;

                                    }
                                }
                            }

                        if ( CheckShortName ) {

                            if (( ! IsDirectory ) && ( ! bOptimizeFileNames )) {

                                if ( IsAcceptableDosName( pFile->pszFileName )) {

                                    CheckShortName = FALSE;

                                    }
                                }
                            }

                        if ( CheckShortName ) {

                            CHAR  Uniquifier[ 12 ];
                            PCHAR p;

                            sprintf( Uniquifier, "%X", dwDirOffset >> 5 );

                            for ( p = Uniquifier; *p; p++ ) {

                                if ( *p > '9' ) {

                                    printf(
                                        "\r\nWARNING: %sname \"%s\" may be inaccessible to 16-bit apps "
                                        "under NT 4.0 (see footnote).\r\n",
                                        IsDirectory ? "Directory " : "File",
                                        GenerateFullName( pFile, ReportNameBuffer.A )
                                        );

                                    fflush( stdout );
                                    bReportBadShortNameWarning = TRUE;
                                    break;

                                    }
                                }
                            }

                        CopyMemory( WhichNameBuffer, pFile->pszFileName, pFile->wFileNameLength );

                        }

                    dwDirOffset += dwEntryLen;

                    }
                }

            //
            //  ISO-9660 6.8.1.3 specifies that the length of a directory
            //  INCLUDES the unused bytes at the end of the last sector,
            //  meaning this number should be rounded up to CD_SECTORSIZE.
            //  Since NT 3.1 media, we have always encoded actual versus
            //  rounded and never heard of any problems.
            //

            if ( bRoundDirectorySizes ) {
                pDir->dwlFileSize = ROUNDUP2( dwDirOffset, CD_SECTORSIZE );
                }
            else {
                pDir->dwlFileSize = dwDirOffset;
                }
            }
        }

    dwLookupTableSize       = dwTableOffset;
    dwLookupTableAllocation = ROUNDUP2( dwLookupTableSize, dwBlockSize );

    }


void ComputeAlternateDirectorySizes( void ) {

    PDIRENTRY pDir, pFile;
    DWORD dwDirOffset, dwTableOffset, dwNextBound, dwNextLimit, dwEntryLen;
    UINT uLevel;

    dwTableOffset = 0;
    nDirectories = 0;

    for ( uLevel = 0; pAlternateStartOfLevel[ uLevel ]; uLevel++ ) {

        for ( pDir = pAlternateStartOfLevel[ uLevel ];
              pDir;
              pDir = pDir->pDirectoryInfo->pAlternateNextDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );

            //
            //  Each table entry is 8 bytes plus the length of the name,
            //  but the next entry must start at a word (2-byte) boundary,
            //  so round-up offset if necessary.
            //

            ASSERT( pDir->wFileNameLength != 0 );

            if (( dwTableOffset += ( 8 + pDir->wFileNameLength )) & 1 )
                ++dwTableOffset;

            pDir->pDirectoryInfo->dwDirIndex = ++nDirectories;

            if ( pDir->pParentDir->pDirectoryInfo->dwDirIndex > 0xFFFF ) {

                ErrorExit(
                    0,
                    "ERROR: Too many directories in volume (directory number of a parent directory\r\n"
                    "       cannot exceed 65535 because it is stored in a 16-bit field).\r\n"
                    );
                }

            dwDirOffset = 34 * 2;               // for "." and ".." entries
            dwNextBound = CD_SECTORSIZE;        // don't straddle sector boundary

            if ( bAllowExactAlignedDirs )
                dwNextLimit = CD_SECTORSIZE;     // more efficient packing
            else
                dwNextLimit = CD_SECTORSIZE - 1; // don't break MSCDEX

            for ( pFile = pDir->pDirectoryInfo->pAlternateFirstFile; pFile; pFile = pFile->pAlternateNextFile ) {

                ASSERT( pFile->wFileNameLength != 0 );

                dwEntryLen  = pFile->wFileNameLength + 33;
                dwEntryLen += dwEntryLen & 1;

                if (( dwDirOffset + dwEntryLen ) > dwNextLimit ) {
                    dwDirOffset  = dwNextBound;
                    dwNextBound += CD_SECTORSIZE;
                    dwNextLimit += CD_SECTORSIZE;
                    }

                DEBUGCODE( pFile->dwDirOffset = dwDirOffset );

                dwDirOffset += dwEntryLen;

                }

            //
            //  ISO-9660 6.8.1.3 specifies that the length of a directory
            //  INCLUDES the unused bytes at the end of the last sector,
            //  meaning this number should be rounded up to CD_SECTORSIZE.
            //  Since NT 3.1 media, we have always encode actual versus
            //  rounded and never heard of any problems.
            //

            if ( bRoundDirectorySizes ) {
                pDir->dwlFileSize = ROUNDUP2( dwDirOffset, CD_SECTORSIZE );
                }
            else {
                pDir->dwlFileSize = dwDirOffset;
                }

            }
        }

    dwLookupTableSize       = dwTableOffset;
    dwLookupTableAllocation = ROUNDUP2( dwLookupTableSize, dwBlockSize );

    }


void ComputeHeaderBlocks( void ) {

    DWORD dwHeaderSectors = 18;

    // initial sectors 0-15 plus PVD and Terminator sectors = 18

    if ( bElTorito )
        dwHeaderSectors++;       // BVD for El Torito

    if ( bEncodeJoliet )
        dwHeaderSectors++;       // SVD for Joliet

    if ( bEncodeUdf )
        dwHeaderSectors += 3;    // NSR descriptors "BEA01" "NSR02" "TEA01"

    ASSERT(( ! bEncodeUdf ) || ( dwHeaderSectors < ANCHOR_SECTOR ));

    dwHeaderBlocks = BLOCK_NUMBER_FROM_SECTOR_NUMBER( dwHeaderSectors );

    dwNextMetaBlockNumber = dwHeaderBlocks;

    }


void AssignDirectoryBlockNumbers( void ) {

    PDIRENTRY pDir;
    UINT uLevel;

    ASSERT( dwHeaderBlocks != 0 );      //  assume this is already computed
    ASSERT( IS_SECTOR_ALIGNED( dwNextMetaBlockNumber * dwBlockSize ));

    ASSERT(( dwLookupTableAllocation != 0 ) || ( dwJolietLookupTableAllocation != 0 ));

    if ( dwJolietLookupTableAllocation )
        dwStartingJolietPathTableBlock = AllocateMetaBlocks( dwJolietLookupTableAllocation * 2 );

    if ( dwLookupTableAllocation )
        dwStartingIsoPathTableBlock = AllocateMetaBlocks( dwLookupTableAllocation * 2 );

    if (( bEncodeJoliet ) || ( bEncodeOnlyUdf )) {

        if ( ! bGenerateShortNames ) {

            ASSERT( dwStubFileSize > 0 );

            dwStubFileBlock = AllocateMetaBlocks( dwStubFileSize );

            }
        }

    if ( ! IS_SECTOR_ALIGNED( (DWORDLONG) dwNextMetaBlockNumber * dwBlockSize )) {

        dwlMetaPaddingBeforeDirectoriesOffset = (DWORDLONG) dwNextMetaBlockNumber * dwBlockSize;
        dwMetaPaddingBeforeDirectoriesBytes   = (DWORD)( ROUNDUP2_64(((DWORDLONG) dwNextMetaBlockNumber * dwBlockSize ), CD_SECTORSIZE )
                                              - ((DWORDLONG) dwNextMetaBlockNumber * dwBlockSize ));

        dwNextMetaBlockNumber += CD_BLOCKS_FROM_SIZE( dwMetaPaddingBeforeDirectoriesBytes );

        }


    DEBUGCODE( dwStartingJolietDirectoryBlock = dwNextMetaBlockNumber );

    if ( bEncodeJoliet ) {

        for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {
            for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

                ASSERT( pDir->pDirectoryInfo != NULL );
                ASSERT( pDir->pDirectoryInfo->dwUnicodeDirSize != 0 );

                pDir->pDirectoryInfo->dwUnicodeBlock =
                            AllocateMetaBlocksSectorAligned(
                                    pDir->pDirectoryInfo->dwUnicodeDirSize );

                }
            }
        }

    DEBUGCODE( dwStartingIsoDirectoryBlock = dwNextMetaBlockNumber );

    if (( bEncodeJoliet ) || ( bEncodeOnlyUdf )) {

        for ( uLevel = 0; pAlternateStartOfLevel[ uLevel ]; uLevel++ ) {
            for ( pDir = pAlternateStartOfLevel[ uLevel ];
                  pDir;
                  pDir = pDir->pDirectoryInfo->pAlternateNextDir ) {

                ASSERT( pDir->pDirectoryInfo != NULL );
                ASSERT( pDir->dwlFileSize != 0 );
                ASSERT( pDir->dwlFileSize < 0xFFFFFFFF );

                pDir->dwStartingBlock = AllocateMetaBlocksSectorAligned( (DWORD)( pDir->dwlFileSize ));

                }
            }

        if ( ! bGenerateShortNames ) {

            ASSERT( dwStubFileBlock != 0 );

            //
            //  We know the stub file is linked at
            //  pRootDir->pDirectoryInfo->pAlternateFirstFile,
            //  and we need to update its starting block location.
            //

            ASSERT( pRootDir->pDirectoryInfo != NULL );
            ASSERT( pRootDir->pDirectoryInfo->pAlternateFirstFile != NULL );
            ASSERT( pRootDir->pDirectoryInfo->pAlternateFirstFile->dwlFileSize == dwStubFileSize );
            ASSERT( pRootDir->pDirectoryInfo->pAlternateFirstFile->dwStartingBlock == 0 );

            pRootDir->pDirectoryInfo->pAlternateFirstFile->dwStartingBlock = dwStubFileBlock;

            }
        }

    else {

        for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {
            for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

                ASSERT( pDir->dwlFileSize != 0 );
                ASSERT( pDir->dwlFileSize < 0xFFFFFFFF );

                pDir->dwStartingBlock = AllocateMetaBlocksSectorAligned( (DWORD)( pDir->dwlFileSize ));

                }
            }
        }
    }


void ComputeStartingFileBlockLocation( void ) {

    ASSERT(( ! bEncodeUdf ) || ( dwNextMetaBlockNumber > ANCHOR_SECTOR ));

    dwStartingFileBlock = dwNextMetaBlockNumber;
    dwTotalImageBlocks  = dwStartingFileBlock
                        + ROUNDUP2( dwTotalFileBlocks, ( CD_SECTORSIZE / dwBlockSize ))
                        + (( bEncodeAutoCrc || bSignImage ) ? CD_BLOCKS_FROM_SIZE( CD_SECTORSIZE ) : 0 );

    if (bEncodeUdf)
    {
       ulAnchorSlackSector = dwTotalImageBlocks;
       dwTotalImageBlocks = ROUNDUP2(dwTotalImageBlocks + 1, 32768 / CD_SECTORSIZE);
       ulAnchorSectorNumber = dwTotalImageBlocks - 1;
    }

    dwlTotalHeaderBytes = (DWORDLONG)dwStartingFileBlock * dwBlockSize;
    dwlTotalMetaBytes   = dwlTotalHeaderBytes + (( bEncodeAutoCrc || bSignImage ) ? CD_SECTORSIZE : 0 );
    dwlTotalImageBytes  = (DWORDLONG)dwTotalImageBlocks  * dwBlockSize;

    }


void PadCopy( PUCHAR pDest, UINT uLength, LPSTR pSource ) {

    for ( ; uLength && *pSource; uLength-- )
        *pDest++ = *pSource++;

    for ( ; uLength; uLength-- )
        *pDest++ = ' ';

    }


void MakeDualEndian( PUCHAR pMem, UINT uBytes, DWORD dwValue ) {

    MakeLittleEndian( pMem, uBytes, dwValue );
    MakeBigEndian( pMem + uBytes, uBytes, dwValue );

    }


void MakeLittleEndian( PUCHAR pMem, UINT uBytes, DWORD dwValue ) {

    switch ( uBytes ) {
        case 4:
            *(UNALIGNED DWORD*)pMem = dwValue;
            break;
        case 2:
            ASSERT( dwValue < 0x10000 );
            *(UNALIGNED WORD*)pMem = (WORD)dwValue;
            break;
        default:
            ASSERT(( uBytes == 4 ) || ( uBytes == 2 ));
        }
    }


void MakeBigEndian( PUCHAR pMem, UINT uBytes, DWORD dwValue ) {

    ASSERT(( uBytes > 2 ) || ( dwValue < 0x10000 ));

    switch ( uBytes ) {
        case 4:
            *pMem++ = *(((PUCHAR)&dwValue) + 3 );
            *pMem++ = *(((PUCHAR)&dwValue) + 2 );
            // don't break, fall through
        case 2:
            *pMem++ = *(((PUCHAR)&dwValue) + 1 );
            *pMem++ = *(((PUCHAR)&dwValue) + 0 );
            break;

        default:
            ASSERT(( uBytes == 4 ) || ( uBytes == 2 ));

        }
    }


void GenerateAndWriteLookupTables( void ) {

    PUCHAR pL, pM, pLookupTableL, pLookupTableM;
    PUCHAR pBuffer;
    UINT uLevel, uLen;
    PDIRENTRY pDir;
    DWORD dwTableSize;
    DWORD dwCombinedTableSize;
    DWORDLONG dwlOffset;

    dwTableSize = dwLookupTableAllocation;              // local copy

    ASSERT( ISALIGN2( dwTableSize, dwBlockSize ));

    dwCombinedTableSize = dwTableSize * 2;

    pBuffer = AllocateBuffer( dwCombinedTableSize, TRUE );

    pL = pLookupTableL = pBuffer;                       // LittleEndian table
    pM = pLookupTableM = pBuffer + dwTableSize;         // BigEndian table

    //
    // root directory is odd, so do it separately, then start loop at level 1
    //

    *pL = 0x01;
    *pM = 0x01;

    ASSERT( pRootDir->dwStartingBlock != 0 );

    MakeLittleEndian( pL + 2, 4, pRootDir->dwStartingBlock );
    MakeBigEndian(    pM + 2, 4, pRootDir->dwStartingBlock );

    ASSERT( pRootDir->pParentDir->pDirectoryInfo->dwDirIndex != 0 );
    ASSERT( pRootDir->pParentDir->pDirectoryInfo->dwDirIndex < 0x10000 );

    MakeLittleEndian( pL + 6, 2, pRootDir->pParentDir->pDirectoryInfo->dwDirIndex );
    MakeBigEndian(    pM + 6, 2, pRootDir->pParentDir->pDirectoryInfo->dwDirIndex );

    pL += 10;
    pM += 10;

    for ( uLevel = 1; pStartOfLevel[ uLevel ]; uLevel++ ) {
        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );
            ASSERT( pDir->wFileNameLength != 0 );

            uLen = (UINT) pDir->wFileNameLength;

            *pL = (UCHAR) uLen;
            *pM = (UCHAR) uLen;

            ASSERT( pDir->dwStartingBlock != 0 );

            MakeLittleEndian( pL + 2, 4, pDir->dwStartingBlock );
            MakeBigEndian(    pM + 2, 4, pDir->dwStartingBlock );

            ASSERT( pDir->pParentDir->pDirectoryInfo->dwDirIndex != 0 );
            ASSERT( pDir->pParentDir->pDirectoryInfo->dwDirIndex < 0x10000 );

            MakeLittleEndian( pL + 6, 2, pDir->pParentDir->pDirectoryInfo->dwDirIndex );
            MakeBigEndian(    pM + 6, 2, pDir->pParentDir->pDirectoryInfo->dwDirIndex );

            memcpy( pL + 8, pDir->pszFileName, uLen );
            memcpy( pM + 8, pDir->pszFileName, uLen );

            pL += ( uLen + 8 ) + ( uLen & 1 );
            pM += ( uLen + 8 ) + ( uLen & 1 );

            ASSERT( pL < ( pLookupTableL + dwTableSize ));
            ASSERT( pM < ( pLookupTableM + dwTableSize ));  // redundant

            }
        }

    ASSERT( dwStartingIsoPathTableBlock != 0 );

    dwlOffset = (DWORDLONG)dwStartingIsoPathTableBlock * dwBlockSize;

    HeaderWrite( dwlOffset, pBuffer, dwCombinedTableSize );

    dwlOffset += dwCombinedTableSize;

    ASSERT( ROUNDUP2_64( dwlOffset, CD_SECTORSIZE ) == ((DWORDLONG)dwStartingJolietDirectoryBlock * dwBlockSize ));

    }



void GenerateAndWriteAlternateLookupTables( void ) {

    PUCHAR pL, pM, pLookupTableL, pLookupTableM;
    PUCHAR pBuffer;
    UINT uLevel, uLen;
    PDIRENTRY pDir;
    DWORD dwTableSize;
    DWORD dwCombinedTableSize;
    DWORDLONG dwlOffset;

    dwTableSize = dwLookupTableAllocation;              // local copy

    ASSERT( ISALIGN2( dwTableSize, dwBlockSize ));

    dwCombinedTableSize = dwTableSize * 2;

    pBuffer = AllocateBuffer( dwCombinedTableSize, TRUE );

    pL = pLookupTableL = pBuffer;                       // LittleEndian table
    pM = pLookupTableM = pBuffer + dwTableSize;         // BigEndian table

    //
    // root directory is odd, so do it separately, then start loop at level 1
    //

    *pL = 0x01;
    *pM = 0x01;

    ASSERT( pRootDir->dwStartingBlock != 0 );

    MakeLittleEndian( pL + 2, 4, pRootDir->dwStartingBlock );
    MakeBigEndian(    pM + 2, 4, pRootDir->dwStartingBlock );

    ASSERT( pRootDir->pParentDir->pDirectoryInfo->dwDirIndex != 0 );
    ASSERT( pRootDir->pParentDir->pDirectoryInfo->dwDirIndex < 0x10000 );

    MakeLittleEndian( pL + 6, 2, pRootDir->pParentDir->pDirectoryInfo->dwDirIndex );
    MakeBigEndian(    pM + 6, 2, pRootDir->pParentDir->pDirectoryInfo->dwDirIndex );

    pL += 10;
    pM += 10;

    for ( uLevel = 1; pAlternateStartOfLevel[ uLevel ]; uLevel++ ) {
        for ( pDir = pAlternateStartOfLevel[ uLevel ];
              pDir;
              pDir = pDir->pDirectoryInfo->pAlternateNextDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );
            ASSERT( pDir->wFileNameLength != 0 );

            uLen = (UINT) pDir->wFileNameLength;

            *pL = (UCHAR) uLen;
            *pM = (UCHAR) uLen;

            ASSERT( pDir->dwStartingBlock != 0 );

            MakeLittleEndian( pL + 2, 4, pDir->dwStartingBlock );
            MakeBigEndian(    pM + 2, 4, pDir->dwStartingBlock );

            ASSERT( pDir->pParentDir->pDirectoryInfo->dwDirIndex != 0 );
            ASSERT( pDir->pParentDir->pDirectoryInfo->dwDirIndex < 0x10000 );

            MakeLittleEndian( pL + 6, 2, pDir->pParentDir->pDirectoryInfo->dwDirIndex );
            MakeBigEndian(    pM + 6, 2, pDir->pParentDir->pDirectoryInfo->dwDirIndex );

            memcpy( pL + 8, pDir->pszFileName, uLen );
            memcpy( pM + 8, pDir->pszFileName, uLen );

            pL += ( uLen + 8 ) + ( uLen & 1 );
            pM += ( uLen + 8 ) + ( uLen & 1 );

            ASSERT( pL < ( pLookupTableL + dwTableSize ));
            ASSERT( pM < ( pLookupTableM + dwTableSize ));  // redundant

            }
        }

    ASSERT( dwStartingIsoPathTableBlock != 0 );

    dwlOffset = (DWORDLONG)dwStartingIsoPathTableBlock * dwBlockSize;

    HeaderWrite( dwlOffset, pBuffer, dwCombinedTableSize );

    }


void WriteDirectories( void ) {

    PDIRENTRY pDir, pFile;
    PUCHAR pBuffer, pMem, pBoundary, pLimit;
    UINT uLevel, uNameLen, uEntryLen;
    DWORD dwDirAllocation;
    DWORDLONG dwlOffset;

    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {
        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );
            ASSERT( pDir->dwlFileSize != 0 );
            ASSERT( pDir->dwlFileSize < 0x100000000 );

            dwDirAllocation = ROUNDUP2( pDir->dwlFileSize, CD_SECTORSIZE );

            pBuffer = AllocateBuffer( dwDirAllocation, TRUE );

            pMem = MakeRootEntries( pBuffer, pDir );

            pBoundary = pBuffer + CD_SECTORSIZE;

            if ( bAllowExactAlignedDirs )
                pLimit = pBoundary;         // more efficient packing
            else
                pLimit = pBoundary - 1;     // don't break MSCDEX

            for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {

                if ( ! ( pFile->dwFlags & NO_DIRENTRY )) {

                    ASSERT( pFile->wFileNameLength != 0 );
                    ASSERT( pFile->dwlFileSize < 0x100000000 );

                    uNameLen   = (UINT) pFile->wFileNameLength;
                    uEntryLen  = uNameLen + 33;
                    uEntryLen += uEntryLen & 1;

                    if (( pMem + uEntryLen ) > pLimit ) {
                        pMem = pBoundary;
                        pBoundary += CD_SECTORSIZE;
                        pLimit    += CD_SECTORSIZE;
                        }

                    ASSERT(( pFile->dwDirOffset == (DWORD)( pMem - pBuffer )) || ( bContinueAfterFailedSourceOpen ));
                    ASSERT(( pMem + uEntryLen ) < ( pBuffer + dwDirAllocation ));
                    ASSERT(( pFile->dwlFileSize == 0 ) || ( pFile->dwStartingBlock != 0 ));

                    *pMem = (UCHAR) uEntryLen;
                    MakeDualEndian( pMem +  2, 4, pFile->dwStartingBlock );
                    MakeDualEndian( pMem + 10, 4, (DWORD)( pFile->dwlFileSize ));

                    *( pMem + 28 ) = 0x01;
                    *( pMem + 31 ) = 0x01;
                    *( pMem + 32 ) = (UCHAR) uNameLen;

                    if ( bUseGlobalTime )
                        memcpy( pMem + 18, cGlobalIsoTimeStamp, 6 );
                    else {
                        ASSERT( pFile->pFileTimeInfo != NULL );
                        MakeIsoTimeStampFromFileTime( pMem + 18, pFile->pFileTimeInfo->ftLastWriteTime );
                        }

                    if ( pFile->dwFlags & IS_HIDDEN ) {
                        *( pMem + 25 ) |= 0x01;
                        }

                    if ( pFile->dwFlags & IS_DIRECTORY ) {

                        *( pMem + 25 ) |= 0x02;

                        memcpy( pMem + 33, pFile->pszFileName, uNameLen );

                        ASSERT( ! ( pFile->dwFlags & IS_DUPLICATE ));
                        ASSERT( pFile->pDirectoryInfo != NULL );
                        ASSERT( pFile->dwStartingBlock != 0 );
                        ASSERT( pFile->dwlFileSize != 0 );

                        }

                    else {

                        if ( bOptimizeFileNames ) {
                            memcpy( pMem + 33, pFile->pszFileName, uNameLen );
                            }
                        else {
                            memcpy( pMem + 33, pFile->pszFileName, uNameLen - 2 );
                            memcpy( pMem + 33 + uNameLen - 2, ";1", 2 );
                            }

                        //
                        //  pFile->pDirectoryInfo and pFile->pPrimary share the
                        //  same address, so we can only assert that pDirectoryInfo
                        //  is NULL if not IS_DUPLICATE.
                        //

                        ASSERT(( pFile->dwFlags & IS_DUPLICATE ) || ( pFile->pDirectoryInfo == NULL ));

                        }

                    pMem += uEntryLen;

                    }

                else {

                    ASSERT(( pFile->dwFlags & NO_DIRENTRY ) && ( bContinueAfterFailedSourceOpen ));

                    }
                }

            dwlOffset = (DWORDLONG)( pDir->dwStartingBlock ) * dwBlockSize;

            ASSERT( IS_SECTOR_ALIGNED( dwlOffset ));

            HeaderWrite( dwlOffset, pBuffer, dwDirAllocation );

            }
        }
    }



void WriteAlternateDirectories( void ) {

    PDIRENTRY pDir, pFile;
    PUCHAR pBuffer, pMem, pBoundary, pLimit;
    UINT uLevel, uNameLen, uEntryLen;
    DWORD dwDirAllocation;
    DWORDLONG dwlOffset;

    for ( uLevel = 0; pAlternateStartOfLevel[ uLevel ]; uLevel++ ) {
        for ( pDir = pAlternateStartOfLevel[ uLevel ];
              pDir;
              pDir = pDir->pDirectoryInfo->pAlternateNextDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );
            ASSERT( pDir->dwlFileSize != 0 );
            ASSERT( pDir->dwlFileSize < 0x100000000 );

            dwDirAllocation = ROUNDUP2( pDir->dwlFileSize, CD_SECTORSIZE );

            pBuffer = AllocateBuffer( dwDirAllocation, TRUE );

            pMem = MakeRootEntries( pBuffer, pDir );

            pBoundary = pBuffer + CD_SECTORSIZE;

            if ( bAllowExactAlignedDirs )
                pLimit = pBoundary;         // more efficient packing
            else
                pLimit = pBoundary - 1;     // don't break MSCDEX

            for ( pFile = pDir->pDirectoryInfo->pAlternateFirstFile; pFile; pFile = pFile->pAlternateNextFile ) {

                if ( ! ( pFile->dwFlags & NO_DIRENTRY )) {

                    ASSERT( pFile->wFileNameLength != 0 );
                    ASSERT( pFile->dwlFileSize < 0x100000000 );

                    uNameLen   = (UINT) pFile->wFileNameLength;
                    uEntryLen  = uNameLen + 33;
                    uEntryLen += uEntryLen & 1;

                    if (( pMem + uEntryLen ) > pLimit ) {
                        pMem = pBoundary;
                        pBoundary += CD_SECTORSIZE;
                        pLimit    += CD_SECTORSIZE;
                        }

                    ASSERT(( pFile->dwDirOffset == (DWORD)( pMem - pBuffer )) || ( bContinueAfterFailedSourceOpen ));
                    ASSERT(( pMem + uEntryLen ) < ( pBuffer + dwDirAllocation ));
                    ASSERT(( pFile->dwlFileSize == 0 ) || ( pFile->dwStartingBlock != 0 ));

                    *pMem = (UCHAR) uEntryLen;
                    MakeDualEndian( pMem +  2, 4, pFile->dwStartingBlock );
                    MakeDualEndian( pMem + 10, 4, (DWORD)( pFile->dwlFileSize ));

                    *( pMem + 28 ) = 0x01;
                    *( pMem + 31 ) = 0x01;
                    *( pMem + 32 ) = (UCHAR) uNameLen;

                    if ( bUseGlobalTime )
                        memcpy( pMem + 18, cGlobalIsoTimeStamp, 6 );
                    else {
                        ASSERT( pFile->pFileTimeInfo != NULL );
                        MakeIsoTimeStampFromFileTime( pMem + 18, pFile->pFileTimeInfo->ftLastWriteTime );
                        }

                    if ( pFile->dwFlags & IS_HIDDEN ) {
                        *( pMem + 25 ) |= 0x01;
                        }

                    if ( pFile->dwFlags & IS_DIRECTORY ) {

                        *( pMem + 25 ) |= 0x02;

                        memcpy( pMem + 33, pFile->pszFileName, uNameLen );

                        ASSERT( ! ( pFile->dwFlags & IS_DUPLICATE ));
                        ASSERT( pFile->pDirectoryInfo != NULL );
                        ASSERT( pFile->dwStartingBlock != 0 );
                        ASSERT( pFile->dwlFileSize != 0 );

                        }

                    else {

                        if ( bOptimizeFileNames ) {
                            memcpy( pMem + 33, pFile->pszFileName, uNameLen );
                            }
                        else {
                            memcpy( pMem + 33, pFile->pszFileName, uNameLen - 2 );
                            memcpy( pMem + 33 + uNameLen - 2, ";1", 2 );
                            }

                        //
                        //  pFile->pDirectoryInfo and pFile->pPrimary share the
                        //  same address, so we can only assert that pDirectoryInfo
                        //  is NULL if not IS_DUPLICATE.
                        //

                        ASSERT(( pFile->dwFlags & IS_DUPLICATE ) || ( pFile->pDirectoryInfo == NULL ));

                        }

                    pMem += uEntryLen;

                    }

                else {

                    ASSERT(( pFile->dwFlags & NO_DIRENTRY ) && ( bContinueAfterFailedSourceOpen ));

                    }
                }

            dwlOffset = (DWORDLONG)( pDir->dwStartingBlock ) * dwBlockSize;

            ASSERT( IS_SECTOR_ALIGNED( dwlOffset ));

            HeaderWrite( dwlOffset, pBuffer, dwDirAllocation );

            }
        }
    }


PUCHAR MakeRootEntry( PUCHAR pMem, PDIRENTRY pDir, UCHAR DirName ) {

    ASSERT( pDir->dwFlags & IS_DIRECTORY );
    ASSERT( pDir->dwStartingBlock != 0 );

    *pMem = 34;

    MakeDualEndian( pMem +  2, 4, pDir->dwStartingBlock );
    MakeDualEndian( pMem + 10, 4, (DWORD)( pDir->dwlFileSize ));

    if ( bUseGlobalTime )
        memcpy( pMem + 18, cGlobalIsoTimeStamp, 6 );
    else {
        ASSERT( pDir->pFileTimeInfo != NULL );
        ASSERT( *(UNALIGNED DWORDLONG*)&( pDir->pFileTimeInfo->ftLastWriteTime ) != 0 );
        MakeIsoTimeStampFromFileTime( pMem + 18, pDir->pFileTimeInfo->ftLastWriteTime );
        }

    *( pMem + 25 ) = 0x02;      // flag for directory
    *( pMem + 28 ) = 0x01;      // volume sequence number (big endian)
    *( pMem + 31 ) = 0x01;      // volume sequence number (little endian)
    *( pMem + 32 ) = 0x01;      // length of name
    *( pMem + 33 ) = DirName;   // single byte name (0x00 or 0x01 for parent)

    return pMem + 34;
    }


PUCHAR MakeRootEntries( PUCHAR pMem, PDIRENTRY pDir ) {

    pMem = MakeRootEntry( pMem, pDir,             0 );      // "." entry
    pMem = MakeRootEntry( pMem, pDir->pParentDir, 1 );      // ".." entry

    return pMem;
    }


PUCHAR MakeJolietRootEntry( PUCHAR pMem, PDIRENTRY pDir, UCHAR DirName ) {

    ASSERT( pDir->dwFlags & IS_DIRECTORY );
    ASSERT( pDir->pDirectoryInfo != NULL );
    ASSERT( pDir->pDirectoryInfo->dwUnicodeBlock != 0 );

    *pMem = 34;

    MakeDualEndian( pMem +  2, 4, pDir->pDirectoryInfo->dwUnicodeBlock );
    MakeDualEndian( pMem + 10, 4, pDir->pDirectoryInfo->dwUnicodeDirSize );

    if ( bUseGlobalTime )
        memcpy( pMem + 18, cGlobalIsoTimeStamp, 6 );
    else {
        ASSERT( pDir->pFileTimeInfo != NULL );
        ASSERT( *(UNALIGNED DWORDLONG*)&( pDir->pFileTimeInfo->ftLastWriteTime ) != 0 );
        MakeIsoTimeStampFromFileTime( pMem + 18, pDir->pFileTimeInfo->ftLastWriteTime );
        }

    *( pMem + 25 ) = 0x02;      // flag for directory
    *( pMem + 28 ) = 0x01;      // volume sequence number (big endian)
    *( pMem + 31 ) = 0x01;      // volume sequence number (little endian)
    *( pMem + 32 ) = 0x01;      // length of name
    *( pMem + 33 ) = DirName;   // single byte name (0x00 or 0x01 for parent)

    return pMem + 34;
    }


PUCHAR MakeJolietRootEntries( PUCHAR pMem, PDIRENTRY pDir ) {

    pMem = MakeJolietRootEntry( pMem, pDir,             0 );    // "." entry
    pMem = MakeJolietRootEntry( pMem, pDir->pParentDir, 1 );    // ".." entry

    return pMem;
    }


void GetFilePath( PDIRENTRY pDir, LPSTR pBuffer ) {

    if ( pDir->pParentDir == pDir )             // root parent points to itself
        strcpy( pBuffer, szRootPath );
    else {
        GetFilePath( pDir->pParentDir, pBuffer );
        strcat( pBuffer, pDir->pszFileName );
        strcat( pBuffer, "\\" );
        }
    }


LPSTR
AllocFullName(
    PDIRENTRY pFile
    )
    {
    CHAR szFullName[ MAX_PATH_LENGTH ];         // this is a 64K stack buffer
    LPSTR pAlloc;

    GetFilePath( pFile->pParentDir, szFullName );
    strcat( szFullName, pFile->pszFileName );

    pAlloc = MyHeapAlloc( strlen( szFullName ) + 1 );
    strcpy( pAlloc, szFullName );

    return pAlloc;
    }


void GetFilePathW( PDIRENTRY pDir, LPWSTR pBuffer ) {

    if ( pDir->pParentDir == pDir )             // root parent points to itself
        lstrcpyW( pBuffer, UnicodeRootPath );
    else {
        GetFilePathW( pDir->pParentDir, pBuffer );
        lstrcatW( pBuffer, pDir->pszUnicodeName );
        lstrcatW( pBuffer, (LPCWSTR) L"\\" );
        }
    }

LPWSTR
AllocFullUnicodeName(
    PDIRENTRY pFile
    )
    {
    WCHAR szFullName[ MAX_PATH_LENGTH ];        // this is a 128K stack buffer
    LPWSTR pAlloc;

    GetFilePathW( pFile->pParentDir, szFullName );
    lstrcatW( szFullName, pFile->pszUnicodeName );

    pAlloc = MyHeapAlloc(( lstrlenW( szFullName ) + 1 ) * 2 );
    lstrcpyW( pAlloc, szFullName );

    return pAlloc;
    }


VOID
WarnTwoFilesSameFirstBlockNotIdentical(
    PDIRENTRY pOriginalFile,
    PDIRENTRY pSimilarFile
    )
    {
    if ( ! bSuppressWarnSameFirstBlockNotIdentical ) {

        printf(
            "\r\n"
            "WARNING: These two files are identical for the first %d bytes, but differ\r\n"
            "at some point beyond that.  This could be intentional, but it might indicate\r\n"
            "that one of these two source files is corrupt:\r\n",
            FX_BLOCKSIZE
            );

        if ( bUnicodeNames ) {
            ASSERT( pOriginalFile->pszUnicodeName );
            GenerateFullNameSourceW( pOriginalFile, DescentNameBuffer.W );
            printf( "  %S\r\n", DescentNameBuffer.W );
            GenerateFullNameSourceW( pSimilarFile, DescentNameBuffer.W );
            printf( "  %S\r\n", DescentNameBuffer.W );
            }
        else {
            ASSERT( pOriginalFile->pszFileName );
            GenerateFullNameSource( pOriginalFile, DescentNameBuffer.A );
            printf( "  %s\r\n", DescentNameBuffer.A );
            GenerateFullNameSource( pSimilarFile, DescentNameBuffer.A );
            printf( "  %s\r\n", DescentNameBuffer.A );
            }

        fflush( stdout );

        }
    }

void WriteFiles( void ) {

    PDIRENTRY pDir, pFile, pPrev;
    UINT uLevel;
    DWORDLONG dwlTotalSourceBytes;
    DWORDLONG dwlFileSize;
    TRANSFER_CONTEXT Tx;
    LPSTR pszFileName;
    LPWSTR pszUnicodeName;
    BOOL Success;
    PUCHAR    pBuffer;

    dwSourceSectorSize = GetSectorSize(
                             NULL,
                             *UnicodeRootPath ? UnicodeRootPath : NULL,
                             *szRootPath ? szRootPath : NULL
                             );

    ASSERT( dwSourceSectorSize != 0 );

    Tx.hOpenContext   = CreateOpenContext( MAX_QUEUED_OPENS );
    Tx.hEventComplete = AllocateAutoEvent();

    if(bEncodeUdf)
	{
        StartThread( UdfTransferFileThread, &Tx, THREAD_PRIORITY_NORMAL );
	}
    else
	{
        StartThread( TransferFileThread, &Tx, THREAD_PRIORITY_NORMAL );
	}

    if ( ! bReportAllFiles ) {
        fprintf(stderr, "0%% complete" );
        fflush( stdout );
        fflush( stderr );
        }

    dwlTotalSourceBytes = 0;

    if ( bHackI386Directory )
        {
        if ( pHackI386Directory )
            {

            //
            //  Create a reverse pointing linked list using the pNextSourceFile
            //  field, then walk the reverse ordered list writing files.
            //

            pPrev = NULL;

            for ( pFile = pHackI386Directory->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile )
                {
                pFile->pNextSourceFile = pPrev;
                pPrev = pFile;
                }

            for ( pFile = pPrev; pFile; pFile = pFile->pNextSourceFile )
                {
                if ( ! ( pFile->dwFlags & ( IS_DIRECTORY | IS_WRITTEN )))
                    {
					HANDLE hFile;

					if(pFile->pszUnicodeName)
					{
						hFile=CreateFileW(pFile->pszUnicodeName,
									      0,
										  FILE_SHARE_READ,
										  NULL,
										  OPEN_EXISTING,
										  0,
										  NULL);
					}
					else
					{
						hFile=CreateFileA(pFile->pszFileName,
									      0,
										  FILE_SHARE_READ,
										  NULL,
										  OPEN_EXISTING,
										  0,
										  NULL);
					}

					if(INVALID_HANDLE_VALUE == hFile)
					{
						// assume file size is unchanged
						dwlFileSize = pFile->dwlFileSize;
					}
					else
					{
						dwlFileSize = GetFileSize64( hFile );

						if(dwlFileSize != pFile->dwlFileSize)
						{
							printf("\r\nWarning the size for %s [%S] has been changed - ",pFile->pszFileName,pFile->pszUnicodeName);

							if(ROUNDUP2_64(dwlFileSize,dwBlockSize) <= ROUNDUP2_64(pFile->dwlFileSize,dwBlockSize))
							{
								printf("still fits in allocation unit - continuing\r\n");
							}
							else
							{
								printf("bigger than allocation unit - image may exceed max size\r\n");
							}
						}
					}


                    if ( dwlFileSize > 0 ) {

                        pFile->dwFlags |= IS_WRITTEN;

                        if ( pFile->pszUnicodeName ) {
                            pszUnicodeName = AllocFullUnicodeName( pFile );
                            pszFileName = NULL;
                            }
                        else {
                            pszFileName = AllocFullName( pFile );
                            pszUnicodeName = NULL;
                            }

                        Success = QueueOpenFile(
                                      Tx.hOpenContext,
                                      pszUnicodeName,
                                      pszFileName,
                                      dwSourceSectorSize,
                                      FX_BLOCKSIZE,
                                      dwlFileSize,
                                      MAX_QUEUED_READS,
                                      pFile
                                      );

                        if ( Success ) {

                            dwlTotalSourceBytes += dwlFileSize;

                            }

                        else {

                            pFile->dwFlags |= NO_DIRENTRY;  // don't create directory entry
                            MyHeapFree( pszFileName ? pszFileName : (PVOID) pszUnicodeName );

                            dwlTotalFileSizes  -= dwlFileSize;
                            dwlTotalImageBytes -= ROUNDUP2_64( dwlFileSize, dwBlockSize );
                            dwTotalImageBlocks -= CD_BLOCKS_FROM_SIZE( dwlFileSize );

                            }
                        }
                    }
                }
            }
        else {

            printf( "\r\nWARNING: -y5 specified, but no \\i386 directory exists\r\n" );
            fflush( stdout );
            }
        }

	if(bEncodeUdf)
	{
		EnqueueUdfFiles(&Tx,&dwlTotalSourceBytes);
	}
	else
	{
		for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {
			for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {
				for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {
					if ( ! ( pFile->dwFlags & ( IS_DIRECTORY | IS_WRITTEN ))) {

						dwlFileSize = pFile->dwlFileSize;

							if ( dwlFileSize > 0 ) {

							pFile->dwFlags |= IS_WRITTEN;

							if ( pFile->pszUnicodeName ) {
								pszUnicodeName = AllocFullUnicodeName( pFile );
								pszFileName = NULL;
								}
							else {
								pszFileName = AllocFullName( pFile );
								pszUnicodeName = NULL;
								}

                            Success = QueueOpenFile(
                                          Tx.hOpenContext,
                                          pszUnicodeName,
                                          pszFileName,
                                          dwSourceSectorSize,
                                          FX_BLOCKSIZE,
                                          dwlFileSize,
                                          MAX_QUEUED_READS,
                                          pFile
                                          );

							if ( Success ) {

								dwlTotalSourceBytes += dwlFileSize;

								}

							else {

								pFile->dwFlags |= NO_DIRENTRY;  // don't create directory entry
								MyHeapFree( pszFileName ? pszFileName : (PVOID) pszUnicodeName );

								dwlTotalFileSizes  -= dwlFileSize;
								dwlTotalImageBytes -= ROUNDUP2_64( dwlFileSize, dwBlockSize );
								dwTotalImageBlocks -= CD_BLOCKS_FROM_SIZE( dwlFileSize );

								}
							}
						}
					}
				}
			}
	}

    CloseOpenContext( Tx.hOpenContext );
    WaitForSingleObject( Tx.hEventComplete, INFINITE );
    RecycleAutoEvent( Tx.hEventComplete );

    if (bEncodeUdf)
    {
		dwSlackBytesToWriteAtEndOfFiles = (ulAnchorSlackSector - ulAnchorSlackSector + 1) * dwBlockSize;
        
        pBuffer = AllocateBuffer( dwSlackBytesToWriteAtEndOfFiles, TRUE );

        FileDataWrite(
            (DWORDLONG) dwTotalImageBlocks * dwBlockSize,
            pBuffer,
            dwSlackBytesToWriteAtEndOfFiles
            );

        dwlTotalFileSizes  += dwSlackBytesToWriteAtEndOfFiles;
        dwlTotalImageBytes += ROUNDUP2_64( dwSlackBytesToWriteAtEndOfFiles, dwBlockSize );
        dwTotalImageBlocks += CD_BLOCKS_FROM_SIZE( dwSlackBytesToWriteAtEndOfFiles );
    }

#ifdef DEBUG
    if(!bEncodeUdf)
        ASSERT( dwlTotalSourceBytes == dwlTotalFileSizes );
#endif
    ASSERT( (DWORDLONG)dwTotalImageBlocks * dwBlockSize == dwlTotalImageBytes );

    }


VOID TransferFileThread( PVOID pParam ) {

    PTRANSFER_CONTEXT pTX = pParam;
    HANDLE    hOpenContext = pTX->hOpenContext;
    PDIRENTRY pFile;
    HANDLE    hReadContext;
    DWORDLONG dwlFileSize;
    DWORDLONG dwlOffset;
    DWORDLONG dwlTotalRead;
    DWORD     dwBytesRead;
    DWORD     dwBytesToWrite;
    DWORD     dwStatus;
    PUCHAR    pBuffer;
    LPSTR     pszFileName;
    LPWSTR    pszUnicodeName;
    DWORD     dwNextBlock;
    BOOL      bAllocate;

    dwNextBlock = dwStartingFileBlock;

    DEBUGCODE( dwlNextFileDataOffset = (DWORDLONG)dwNextBlock * dwBlockSize );

    ASSERT( dwSourceSectorSize != 0 );

    for (;;) {

        hReadContext = WaitForOpen(
                            hOpenContext,
                            &pszUnicodeName,
                            &pszFileName,
                            &pFile
                            );

        if ( hReadContext == NULL ) {
            break;
            }

        if ( bReportAllFiles ) {
            if ( pszUnicodeName )
                printf( "\r\n%S", pszUnicodeName );
            else
                printf( "\r\n%s", pszFileName );
            fflush( stdout );
            }

        dwlFileSize = pFile->dwlFileSize;

        ASSERT( dwlFileSize != 0 );

        dwlOffset = (DWORDLONG)dwNextBlock * dwBlockSize;

        if ( bOptimizeStorage ) {
            if ( bOptimizeFast ) {
                bAllocate = TransferAndCheckForDuplicateFast( pFile,
                                                              dwlOffset,
                                                              hReadContext,
                                                              dwlFileSize );
                }
            else {
                bAllocate = TransferAndCheckForDuplicateSlow( pFile,
                                                              dwlOffset,
                                                              hReadContext,
                                                              dwlFileSize );
                }
            }

        else {

            dwStatus     = 0;
            dwlTotalRead = 0;

            while ( dwStatus == 0 ) {

                dwStatus = WaitForRead( hReadContext, &pBuffer, &dwBytesRead );

                ASSERT( ISALIGN2( dwBytesRead, dwBlockSize ) || ( dwStatus != 0 ));

                dwBytesToWrite = ROUNDUP2( dwBytesRead, dwBlockSize );

                ZeroMemory( pBuffer + dwBytesRead, dwBytesToWrite - dwBytesRead );

                FileDataWrite( dwlOffset, pBuffer, dwBytesToWrite );

                dwlOffset += dwBytesToWrite;

                dwlTotalRead += dwBytesRead;
                }

            ASSERT( dwlTotalRead == dwlFileSize );

            bAllocate = TRUE;

            }

        CloseReadContext( hReadContext, TRUE );

        if ( pszUnicodeName )
            MyHeapFree( pszUnicodeName );

        if ( pszFileName )
            MyHeapFree( pszFileName );

        if ( bAllocate ) {
            pFile->dwStartingBlock = dwNextBlock;
            dwNextBlock += CD_BLOCKS_FROM_SIZE( dwlFileSize );
#ifdef DEBUG
            if ( bOptimizeStorage ) {
                dwlNextFileDataOffset = (DWORDLONG) dwNextBlock * dwBlockSize;
                }
#endif // DEBUG

            }
        }


    if ( ! IS_SECTOR_ALIGNED( (DWORDLONG) dwNextBlock * dwBlockSize )) {

        dwSlackBytesToWriteAtEndOfFiles = (DWORD)
             (ROUNDUP2_64(((DWORDLONG) dwNextBlock * dwBlockSize ), CD_SECTORSIZE )
             - ((DWORDLONG) dwNextBlock * dwBlockSize ));

        pBuffer = AllocateBuffer( dwSlackBytesToWriteAtEndOfFiles, TRUE );

        FileDataWrite(
            (DWORDLONG) dwNextBlock * dwBlockSize,
            pBuffer,
            dwSlackBytesToWriteAtEndOfFiles
            );

        dwNextBlock += CD_BLOCKS_FROM_SIZE( dwSlackBytesToWriteAtEndOfFiles );

        }


    ASSERT( IS_SECTOR_ALIGNED( (DWORDLONG) dwNextBlock * dwBlockSize ));

    dwTotalImageBlocks = dwNextBlock;

    if ( bEncodeAutoCrc )
        dwTotalImageBlocks += BLOCK_NUMBER_FROM_SECTOR_NUMBER( 1 );

    dwlTotalImageBytes = (DWORDLONG)dwTotalImageBlocks * dwBlockSize;

    SetEvent( pTX->hEventComplete );

    }

VOID UdfTransferFileThread( PVOID pParam )
{
    PTRANSFER_CONTEXT pTX = pParam;
    HANDLE    hOpenContext = pTX->hOpenContext;
    PDIRENTRY pFile;
    HANDLE    hReadContext;
    LPSTR     pszFileName;
    LPWSTR    pszUnicodeName;
    DWORD     dwNextBlock;
    DWORD   dwNextIcbBlock;
    DEBUGCODE( DWORD dwStartingDataBlock );

    dwNextIcbBlock = dwStartingFileBlock;
    dwNextBlock = dwNextIcbBlock + GetIcbBlockCount();

    DEBUGCODE( printf("ICB blocks start at %u and go to %u\r\n",dwNextIcbBlock,dwNextBlock) );

    DEBUGCODE(dwStartingDataBlock = dwNextBlock);

    DEBUGCODE( dwlNextFileDataOffset = (DWORDLONG)dwNextBlock * dwBlockSize );
    ASSERT( dwSourceSectorSize != 0 );

    for (;;)
    {

        hReadContext = WaitForOpen(
                            hOpenContext,
                            &pszUnicodeName,
                            &pszFileName,
                            &pFile
                            );

        if ( hReadContext == NULL )
        {
            break;
        }

        if ( bReportAllFiles )
        {
            if ( pszUnicodeName )
                printf( "%S [%S]\r\n", pszUnicodeName,pFile->pszUnicodeName );
            else
                printf( "%s\r\n", pszFileName );
            fflush( stdout );
        }

#ifdef UDF_DBG
        printf("Starting block = %u\r\n",dwNextBlock);
        printf( "[%S]\r\n",pFile->pszUnicodeName );
        printf("Make extent\r\n");
#endif

        ASSERT(dwNextIcbBlock < dwStartingDataBlock);

        MakeFileExtent(pFile,&dwNextBlock,dwNextIcbBlock,hReadContext);

        dwNextIcbBlock++;

        CloseReadContext(hReadContext,TRUE);

        if ( pszUnicodeName )
            MyHeapFree( pszUnicodeName );

        if ( pszFileName )
            MyHeapFree( pszFileName );
    }

    if ( ! IS_SECTOR_ALIGNED( (DWORDLONG) dwNextBlock * dwBlockSize ))
    {
		BYTE* pBuffer;

        dwSlackBytesToWriteAtEndOfFiles = (DWORD)
             (ROUNDUP2_64(((DWORDLONG) dwNextBlock * dwBlockSize ), CD_SECTORSIZE )
             - ((DWORDLONG) dwNextBlock * dwBlockSize ));

        pBuffer = AllocateBuffer( dwSlackBytesToWriteAtEndOfFiles, TRUE );

        FileDataWrite(
            (DWORDLONG) dwNextBlock * dwBlockSize,
            pBuffer,
            dwSlackBytesToWriteAtEndOfFiles
            );

        dwNextBlock += CD_BLOCKS_FROM_SIZE( dwSlackBytesToWriteAtEndOfFiles );
    }

    ASSERT( IS_SECTOR_ALIGNED( (DWORDLONG) dwNextBlock * dwBlockSize ));

    dwTotalImageBlocks = dwNextBlock;

	/*
    if ( bEncodeAutoCrc )
        dwTotalImageBlocks += BLOCK_NUMBER_FROM_SECTOR_NUMBER( 1 );
		*/

    dwlTotalImageBytes = (DWORDLONG)dwTotalImageBlocks * dwBlockSize;

    SetEvent( pTX->hEventComplete );
}

BOOL TransferAndCheckForDuplicateSlow( PDIRENTRY pFile,
                                       DWORDLONG dwlOffset,
                                       HANDLE    hReadContext,
                                       DWORDLONG dwlFileSize ) {

    PDIRENTRY       pFileDup;
    DWORD           dwBytesRead;
    DWORDLONG       dwlTotalRead;
    DWORD           dwBytesToWrite;
    DWORD           dwBytesToHash;
    DWORD           dwStatus;
    DWORDLONG       dwlDupOffset;
    PUCHAR          pBuffer;
    PUCHAR          pDupBuffer;
    PBNODE          pBnode;
    BOOL            bDuplicate;
    HANDLE          hDupReadContext;
    DWORD           dwDupStatus;
    DWORD           dwDupBytesRead;
    LPSTR           pDupFileName;
    LPWSTR          pDupUnicodeName;
    DWORD           dwDiamondTime;
    ULONG           PossibleCrc;
    MD5_HASH        PossibleHash;
    HASH_AND_LENGTH FirstBlockHashAndLength;

    InitMD5( &FirstBlockHashAndLength.Hash );
    FirstBlockHashAndLength.Length = dwlFileSize;

    PossibleCrc   = FileDataCrc;
    PossibleHash  = SignatureHash;
    bDuplicate    = FALSE;
    pDupBuffer    = NULL;
    pFileDup      = NULL;
    dwlDupOffset  = 0;
    dwDiamondTime = 0;          // not necessary, but prevents compiler warning

    dwStatus = WaitForRead( hReadContext, &pBuffer, &dwBytesRead );

    ASSERT( ISALIGN2( dwBytesRead, dwBlockSize ) || ( dwStatus != 0 ));
    ASSERT(( dwBytesRead == FX_BLOCKSIZE ) || ( dwBytesRead == dwlFileSize ));

    dwlTotalRead = dwBytesRead;

    dwBytesToHash  = ROUNDUP2( dwBytesRead, 64 );
    dwBytesToWrite = ROUNDUP2( dwBytesRead, dwBlockSize );
    ZeroMemory( pBuffer + dwBytesRead, dwBytesToWrite - dwBytesRead );

    if ( bEncodeAutoCrc )
        PossibleCrc = Crc32( PossibleCrc, pBuffer, dwBytesToWrite );

    if ( bSignImage )
        UpdateMD5_Multiple64ByteChunks( &PossibleHash, pBuffer, dwBytesToWrite );

    if (( bOptimizeDiamond ) && ( *(UNALIGNED DWORD*)pBuffer == 'FCSM' )) {
        dwDiamondTime = *(UNALIGNED DWORD*)( pBuffer + 0x36 );
        *(UNALIGNED DWORD*)( pBuffer + 0x36 ) = 0;
        }

    UpdateMD5_Multiple64ByteChunks( &FirstBlockHashAndLength.Hash, pBuffer, dwBytesToHash );

    if (( bOptimizeDiamond ) && ( *(UNALIGNED DWORD*)pBuffer == 'FCSM' )) {
        *(UNALIGNED DWORD*)( pBuffer + 0x36 ) = dwDiamondTime;
        }

    bDuplicate = LookupOrAddBnode(
                     &pFirstBlockHashTree,
                     &FirstBlockHashAndLength,
                     pFile,
                     &pBnode
                     );

    if ( bDuplicate ) {

        pFileDup = pBnode->pContext;

        ASSERT( pFileDup->dwlFileSize == dwlFileSize );

        dwlDupOffset = (DWORDLONG)( pFileDup->dwStartingBlock ) * dwBlockSize;

        if ( pFileDup->pszUnicodeName ) {
            pDupUnicodeName = AllocFullUnicodeName( pFileDup );
            pDupFileName = NULL;
            }
        else {
            pDupFileName = AllocFullName( pFileDup );
            pDupUnicodeName = NULL;
            }

        hDupReadContext = CreateReadContext( pDupUnicodeName,
                                             pDupFileName,
                                             NULL,
                                             dwSourceSectorSize,
                                             FX_BLOCKSIZE,
                                             2,
                                             NULL,
                                             0,
                                             pFileDup->dwlFileSize );

        dwDupStatus = WaitForRead( hDupReadContext, &pDupBuffer, &dwDupBytesRead );

        ASSERT( dwDupStatus == dwStatus );
        ASSERT( dwDupBytesRead == dwBytesRead );

        if (( bOptimizeDiamond ) && ( *(UNALIGNED DWORD*)pDupBuffer == 'FCSM' )) {
            *(UNALIGNED DWORD*)( pDupBuffer + 0x36 ) = dwDiamondTime;
            }

        bDuplicate = ( memcmp( pBuffer, pDupBuffer, dwBytesRead ) == 0 );

        if ( ! bDuplicate )
            StopIssuingReads( hDupReadContext );

        ReleaseBuffer( pDupBuffer );

        if ( ! bDuplicate ) {

            CloseReadContext( hDupReadContext, TRUE );

            GenericWrite( dwlOffset, pBuffer, dwBytesToWrite );

            }

        else {

            QueueMaybeWrite( hTargetContext,
                             pBuffer,
                             dwlOffset,
                             dwBytesToWrite,
                             NULL,
                             NULL,
                             dwMaxQueuedWriteBufferBytes );
            }

        dwlOffset += dwBytesToWrite;

        ReportTransferProgress( dwlOffset );

        while ( dwStatus == 0 ) {

            dwStatus = WaitForRead( hReadContext, &pBuffer, &dwBytesRead );

            ASSERT( ISALIGN2( dwBytesRead, dwBlockSize ) || ( dwStatus != 0 ));

            dwlTotalRead += dwBytesRead;

            if ( bDuplicate ) {

                dwDupStatus = WaitForRead( hDupReadContext, &pDupBuffer, &dwDupBytesRead );

                ASSERT( dwDupStatus == dwStatus );
                ASSERT( dwDupBytesRead == dwBytesRead );

                bDuplicate = ( memcmp( pBuffer, pDupBuffer, dwBytesRead ) == 0 );

                ReleaseBuffer( pDupBuffer );

                if ( ! bDuplicate ) {

                    StopIssuingReads( hDupReadContext );

                    FlushMaybeQueue( hTargetContext );

                    CloseReadContext( hDupReadContext, TRUE );

                    }
                }

            dwBytesToWrite = ROUNDUP2( dwBytesRead, dwBlockSize );
            ZeroMemory( pBuffer + dwBytesRead, dwBytesToWrite - dwBytesRead );

            if ( bEncodeAutoCrc )
                PossibleCrc = Crc32( PossibleCrc, pBuffer, dwBytesToWrite );

            if ( bSignImage )
                UpdateMD5_Multiple64ByteChunks( &PossibleHash, pBuffer, dwBytesToWrite );

            if ( bDuplicate ) {

                QueueMaybeWrite( hTargetContext,
                                 pBuffer,
                                 dwlOffset,
                                 dwBytesToWrite,
                                 NULL,
                                 NULL,
                                 dwMaxQueuedWriteBufferBytes );
                }
            else {

                GenericWrite( dwlOffset, pBuffer, dwBytesToWrite );

                }

            dwlOffset += dwBytesToWrite;

            ReportTransferProgress( dwlOffset );
            }


        if ( bDuplicate ) {

            CloseReadContext( hDupReadContext, TRUE );
            DiscardMaybeQueue( hTargetContext );
            MakeFileDuplicate( pFile, pFileDup );

            }
        else {

            WarnTwoFilesSameFirstBlockNotIdentical( pFileDup, pFile );

            FlushMaybeQueue( hTargetContext );
            FileDataCrc   = PossibleCrc;
            SignatureHash = PossibleHash;

            }

        if ( pDupUnicodeName )
            MyHeapFree( pDupUnicodeName );

        if ( pDupFileName )
            MyHeapFree( pDupFileName );

        }

    else {

        GenericWrite( dwlOffset, pBuffer, dwBytesToWrite );

        dwlOffset += dwBytesToWrite;

        ReportTransferProgress( dwlOffset );

        while ( dwStatus == 0 ) {

            dwStatus = WaitForRead( hReadContext, &pBuffer, &dwBytesRead );

            ASSERT( ISALIGN2( dwBytesRead, dwBlockSize ) || ( dwStatus != 0 ));

            dwlTotalRead += dwBytesRead;

            dwBytesToWrite = ROUNDUP2( dwBytesRead, dwBlockSize );
            ZeroMemory( pBuffer + dwBytesRead, dwBytesToWrite - dwBytesRead );

            if ( bEncodeAutoCrc )
                PossibleCrc = Crc32( PossibleCrc, pBuffer, dwBytesToWrite );

            if ( bSignImage )
                UpdateMD5_Multiple64ByteChunks( &PossibleHash, pBuffer, dwBytesToWrite );

            GenericWrite( dwlOffset, pBuffer, dwBytesToWrite );

            dwlOffset += dwBytesToWrite;

            ReportTransferProgress( dwlOffset );
            }

        FileDataCrc   = PossibleCrc;
        SignatureHash = PossibleHash;
        }

    ASSERT( dwlTotalRead == dwlFileSize );

    return ( ! bDuplicate );
    }


BOOL TransferAndCheckForDuplicateFast( PDIRENTRY pFile,
                                       DWORDLONG dwlOffset,
                                       HANDLE    hReadContext,
                                       DWORDLONG dwlFileSize ) {

    PDIRENTRY       pFileDup;
    PDIRENTRY       pFirstDup;
    DWORD           dwBytesRead;
    DWORDLONG       dwlTotalRead;
    DWORD           dwBytesToWrite;
    DWORD           dwBytesToHash;
    DWORD           dwStatus;
    PUCHAR          pBuffer;
    PBNODE          pBnode;
    BOOL            bFirstBlockMatch;
    BOOL            bWholeFileMatch;
    BOOL            bNewFile;
    DWORD           dwDiamondTime;
    ULONG           PossibleCrc;
    MD5_HASH        PossibleHash;
    HASH_AND_LENGTH FirstBlockHashAndLength;
    HASH_AND_LENGTH WholeFileHashAndLength;

    InitMD5( &FirstBlockHashAndLength.Hash );
    FirstBlockHashAndLength.Length = dwlFileSize;

    PossibleCrc  = FileDataCrc;
    PossibleHash = SignatureHash;

    bNewFile = TRUE;

    dwlTotalRead  = 0;
    dwDiamondTime = 0;          // not necessary, but prevents compiler warning

    dwStatus = WaitForRead( hReadContext, &pBuffer, &dwBytesRead );

    ASSERT( ISALIGN2( dwBytesRead, dwBlockSize ) || ( dwStatus != 0 ));
    ASSERT(( dwBytesRead == FX_BLOCKSIZE ) || ( dwBytesRead == dwlFileSize ));

    dwlTotalRead += dwBytesRead;

    dwBytesToHash  = ROUNDUP2( dwBytesRead, 64 );
    dwBytesToWrite = ROUNDUP2( dwBytesRead, dwBlockSize );

    ZeroMemory( pBuffer + dwBytesRead, dwBytesToWrite - dwBytesRead );

    if ( bEncodeAutoCrc )
        PossibleCrc = Crc32( PossibleCrc, pBuffer, dwBytesToWrite );

    if ( bSignImage )
        UpdateMD5_Multiple64ByteChunks( &PossibleHash, pBuffer, dwBytesToWrite );

    if (( bOptimizeDiamond ) && ( *(UNALIGNED DWORD*)pBuffer == 'FCSM' )) {
        dwDiamondTime = *(UNALIGNED DWORD*)( pBuffer + 0x36 );
        *(UNALIGNED DWORD*)( pBuffer + 0x36 ) = 0;
        }

    UpdateMD5_Multiple64ByteChunks( &FirstBlockHashAndLength.Hash, pBuffer, dwBytesToHash );
    WholeFileHashAndLength = FirstBlockHashAndLength;   // copy whole structure

    if (( bOptimizeDiamond ) && ( *(UNALIGNED DWORD*)pBuffer == 'FCSM' )) {
        *(UNALIGNED DWORD*)( pBuffer + 0x36 ) = dwDiamondTime;
        }

    bFirstBlockMatch = LookupOrAddBnode(
                           &pFirstBlockHashTree,
                           &FirstBlockHashAndLength,
                           pFile,
                           &pBnode
                           );

    if ( bFirstBlockMatch ) {

        pFirstDup = pBnode->pContext;

        ASSERT( pFirstDup->dwlFileSize == dwlFileSize );

        QueueMaybeWrite( hTargetContext,
                         pBuffer,
                         dwlOffset,
                         dwBytesToWrite,
                         NULL,
                         NULL,
                         dwMaxQueuedWriteBufferBytes );
        }

    else {

        pFirstDup = NULL;

        GenericWrite( dwlOffset, pBuffer, dwBytesToWrite );

        }

    dwlOffset += dwBytesToWrite;

    ReportTransferProgress( dwlOffset );

    while ( dwStatus == 0 ) {

        dwStatus = WaitForRead( hReadContext, &pBuffer, &dwBytesRead );

        ASSERT( ISALIGN2( dwBytesRead, dwBlockSize ) || ( dwStatus != 0 ));

        dwlTotalRead  += dwBytesRead;
        dwBytesToWrite = ROUNDUP2( dwBytesRead, dwBlockSize );
        dwBytesToHash  = ROUNDUP2( dwBytesRead, 64 );

        ZeroMemory( pBuffer + dwBytesRead, dwBytesToWrite - dwBytesRead );

        UpdateMD5_Multiple64ByteChunks( &WholeFileHashAndLength.Hash, pBuffer, dwBytesToHash );

        if ( bEncodeAutoCrc )
            PossibleCrc = Crc32( PossibleCrc, pBuffer, dwBytesToWrite );

        if ( bSignImage )
            UpdateMD5_Multiple64ByteChunks( &PossibleHash, pBuffer, dwBytesToWrite );

        if ( bFirstBlockMatch ) {

            QueueMaybeWrite( hTargetContext,
                             pBuffer,
                             dwlOffset,
                             dwBytesToWrite,
                             NULL,
                             NULL,
                             dwMaxQueuedWriteBufferBytes );
            }

        else {

            GenericWrite( dwlOffset, pBuffer, dwBytesToWrite );

            }

        dwlOffset += dwBytesToWrite;

        ReportTransferProgress( dwlOffset );

        }

    ASSERT( dwlTotalRead == dwlFileSize );

    bWholeFileMatch = LookupOrAddBnode(
                          &pWholeFileHashTree,
                          &WholeFileHashAndLength,
                          pFile,
                          &pBnode
                          );

    if ( bFirstBlockMatch ) {

        if ( bWholeFileMatch ) {

            //
            //  There is an extremely high probability that this file is a
            //  duplicate.  Odds are 1/2^128 (1 in 3.4e38 or 340 undecillion)
            //  that the MD5 hashes of two different files with equal length
            //  are not the same, assuming of course that MD5 hash values
            //  are flatly distributed.
            //

            DiscardMaybeQueue( hTargetContext );    // don't write to image

            pFileDup = pBnode->pContext;

            ASSERT( pFileDup->dwlFileSize == dwlFileSize );

            MakeFileDuplicate( pFile, pFileDup );

            bNewFile = FALSE;

            }

        else {

            ASSERT( pFirstDup != NULL );

            WarnTwoFilesSameFirstBlockNotIdentical( pFirstDup, pFile );

            FlushMaybeQueue( hTargetContext );      // commit to image

            FileDataCrc   = PossibleCrc;
            SignatureHash = PossibleHash;
            }
        }

    else {

        FileDataCrc   = PossibleCrc;
        SignatureHash = PossibleHash;
        }

    return bNewFile;

    }


VOID
MakeFileDuplicate(
    PDIRENTRY pFile,
    PDIRENTRY pPrimary
    )
    {
    DWORDLONG dwlFileSize;

    pFile->dwFlags        |= IS_DUPLICATE;
    pFile->pPrimary        = pPrimary;
    pFile->dwStartingBlock = pPrimary->dwStartingBlock;

    dwlFileSize = pFile->dwlFileSize;

    dwlTotalDupFiles++;
    dwlTotalDupFileBytes += ROUNDUP2_64( dwlFileSize, dwBlockSize );
    dwlTotalImageBytes   -= ROUNDUP2_64( dwlFileSize, dwBlockSize );
    dwTotalImageBlocks   -= CD_BLOCKS_FROM_SIZE( dwlFileSize );

    if ( bShowDuplicates ) {

        if ( pFile->pszUnicodeName ) {

            ASSERT( pFile->pPrimary->pszUnicodeName );

            GenerateFullNameSourceW( pFile, DescentNameBuffer.W );
            printf( "\r\n%S is duplicate of ", DescentNameBuffer.W );
            GenerateFullNameSourceW( pFile->pPrimary, DescentNameBuffer.W );
            printf( "%S\r\n", DescentNameBuffer.W );
            fflush( stdout );

            }

        else {

            ASSERT( pFile->pszFileName );
            ASSERT( pFile->pPrimary->pszFileName );

            GenerateFullNameSource( pFile, DescentNameBuffer.A );
            printf( "\r\n%s is duplicate of ", DescentNameBuffer.A );
            GenerateFullNameSource( pFile->pPrimary, DescentNameBuffer.A );
            printf( "%s\r\n", DescentNameBuffer.A );
            fflush( stdout );

            }
        }
    }


VOID
ReportTransferProgress(
    DWORDLONG dwlOffset
    )
    {
    static DWORDLONG dwlNextOffset;

    if ( bReportAllFiles )
        return;

    if ( dwlOffset > dwlNextOffset ) {

        DWORDLONG dwlTotalBytesDone;
        DWORDLONG dwlBytesPerPercent;
        DWORDLONG dwlBytesNextPercent;
        DWORD     dwPercentComplete;
        static DWORD dwOldPercent = 0;

        dwlTotalBytesDone   = dwlOffset - dwlTotalMetaBytes;
        dwPercentComplete   = (DWORD)(( dwlTotalBytesDone * 100) / dwlTotalImageBytes );
        dwlBytesPerPercent  = dwlTotalImageBytes / 100;
        dwlBytesNextPercent = ( dwPercentComplete + 1 ) * dwlBytesPerPercent;
        dwlNextOffset       = dwlBytesNextPercent + dwlTotalMetaBytes;

        // bugbug, this should fix the UDF progress, but not screw up the ISO
        dwPercentComplete = dwPercentComplete % 100;

        if(dwOldPercent > dwPercentComplete)
            dwPercentComplete = dwOldPercent;

        dwOldPercent = dwPercentComplete;

        fprintf(stderr, "\r%d%% complete", dwPercentComplete );
        fflush( stderr );
        }

    }


VOID
ReportHeaderProgress(
    DWORDLONG dwlOffset
    )
    {
    static DWORDLONG dwlNextOffset;

    if ( bReportAllFiles )
        return;

    if ( dwlOffset > dwlNextOffset ) {

        DWORDLONG dwlTotalFileBytes;
        DWORDLONG dwlTotalBytesDone;
        DWORDLONG dwlBytesPerPercent;
        DWORDLONG dwlBytesNextPercent;
        DWORD     dwPercentComplete;
        static DWORD dwOldPercent = 0;

        dwlTotalFileBytes   = dwlTotalImageBytes - dwlTotalMetaBytes;
        dwlTotalBytesDone   = dwlTotalFileBytes + dwlOffset;
        dwPercentComplete   = (DWORD)(( dwlTotalBytesDone * 100 ) / dwlTotalImageBytes );
        dwlBytesPerPercent  = dwlTotalImageBytes / 100;
        dwlBytesNextPercent = ( dwPercentComplete + 1 ) * dwlBytesPerPercent;
        dwlNextOffset       = dwlBytesNextPercent - dwlTotalFileBytes;

                // bugbug, this should fix the UDF progress, but not screw up the ISO
        dwPercentComplete = dwPercentComplete % 100;

        if(dwOldPercent > dwPercentComplete)
            dwPercentComplete = dwOldPercent;

        dwOldPercent = dwPercentComplete;

        fprintf( stderr, "\r%d%% complete", dwPercentComplete );
        fflush( stderr );
        }

    }


void MakeIsoTimeStampFromFileTime( PUCHAR pMem, FILETIME ftFileTime ) {

    SYSTEMTIME st;

    ZeroMemory( &st, sizeof( st ));      // in case FileTimeToSystemTime fails.

    *(UNALIGNED DWORDLONG*)&ftFileTime += LocalTimeBiasInFileTimeUnits;

    FileTimeToSystemTime( &ftFileTime, &st );

    *( pMem + 0 ) = (UCHAR)( st.wYear - 1900 );
    *( pMem + 1 ) = (UCHAR)( st.wMonth );
    *( pMem + 2 ) = (UCHAR)( st.wDay );
    *( pMem + 3 ) = (UCHAR)( st.wHour );
    *( pMem + 4 ) = (UCHAR)( st.wMinute );
    *( pMem + 5 ) = (UCHAR)( st.wSecond );
    *( pMem + 6 ) = (UCHAR)( IsoLocalTimeBias );

    }


void MakeIsoVolumeCreationTime( PCHAR pMem, FILETIME ftFileTime ) {

    SYSTEMTIME st;

    ZeroMemory( &st, sizeof( st ));      // in case FileTimeToSystemTime fails.

    *(UNALIGNED DWORDLONG*)&ftFileTime += LocalTimeBiasInFileTimeUnits;

    FileTimeToSystemTime( &ftFileTime, &st );

    sprintf(
        pMem,
        "%04d%02d%02d%02d%02d%02d%02d%c",
        st.wYear,
        st.wMonth,
        st.wDay,
        st.wHour,
        st.wMinute,
        st.wSecond,
        0,                  // hundredths of seconds
        IsoLocalTimeBias
        );

    }


void DeleteTargetFile( void ) {

    if ( ! DeleteFile( pszTargetFile )) {

        DWORD ErrorCode = GetLastError();


        if (( ErrorCode != ERROR_PATH_NOT_FOUND ) &&
            ( ErrorCode != ERROR_FILE_NOT_FOUND )) {

            ErrorExit( GETLASTERROR,
                       "ERROR: Could not delete existing file \"%s\"\r\n",
                       pszTargetFile );

            }
        }
    }


BOOL NonDchars( LPSTR pszFileName ) {

    PUCHAR p;
    UCHAR  c;

    //
    //  Note -- we'll consider a '.' to be a dchar here since
    //  it is valid in filenames, so when calling this routine
    //  for valid directory names, you'll have to special-case
    //  the '.' and check for it separately from call to NonDchars.
    //

    for ( p = (PUCHAR) pszFileName; ( c = *p ) != '\0'; p++ )

        if ((( c < '0' ) && ( c != '.' )) ||
            (( c > '9' ) && ( c <  'A' )) ||
            (( c > 'Z' ) && ( c != '_' )))

            return TRUE;

    return FALSE;

    }


void ParseOptions( int argc, char *argv[] ) {

    BOOL bJS = FALSE;
    BOOL bUS = FALSE;
    char *p;
    int i;

    //
    //  The option arguments may appear anywhere in the command line
    //  (beginning, end, middle, mixed), so for each option we encounter,
    //  we'll mark it NULL after processing it.  Then, the other Parse
    //  routines can loop through the whole command line again, and just
    //  ignore NULLs.
    //

    for ( i = 1; i < argc; i++ ) {

        p = argv[ i ];

        if (( p ) && ( *p == '-' ) || ( *p == '/' )) {     // process flags

            ++p;

            if (strcmp(p, "xbox") == 0) {

                bUdfXbox = TRUE;
                argv[ i ] = NULL;
                continue;
            }

            switch( tolower( *p )) {

                case '?':
                    Usage();
                    break;

                case 't':
                    bUseGlobalTime = TRUE;
                    ParseTimeStamp( p + 1 );
                    argv[ i ] = NULL;
                    break;

                case 'g':
                    bEncodeGMT = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 'h':
                    bHiddenFiles = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 'l':

                    if ( strlen( p + 1 ) > 32 ) {
                        ErrorExit( 0, "ERROR: Maximum volume label length is 32 characters\r\n" );
                        }

                    strcpy(  chVolumeLabel, p + 1 );                //  chVolumeLabel is mixed case
                    strcpy( uchVolumeLabel, p + 1 );
                    strupr( uchVolumeLabel );                       // uchVolumeLabel is upper case
                    AnsiToUnicode( chVolumeLabel, wchVolumeLabel ); // wchVolumeLabel is mixed case
                    argv[ i ] = NULL;
                    break;

                case 'u':
                    switch ( tolower( *( ++p ) ))
                    {
                        case '2':
                            bEncodeOnlyUdf      = TRUE;

                        case '1':
                            bEncodeUdf          = TRUE;
                            bUnicodeNames       = TRUE;
                            bOptimizeFileNames  = TRUE;
                            bIgnoreMaxImageSize = TRUE;
                            break;

                        case 'r':
                            bUS = TRUE;
                            strcpy( StubSourceFile, p + 1 );
                            if ( *StubSourceFile == 0 ) {
                                Usage();
                                }
                            break;

                        case 'e':
                            bUdfEmbedData=TRUE;
                            break;

                        case 's':
                            bUdfSparse=TRUE;
                            break;

                        case 'f':
                            bUdfEmbedFID=TRUE;
                            break;

                        case 'v':
                            bUdfVideoZone = TRUE;
                            break;

                        default:
                            Usage();
                        }
                    argv[ i ] = NULL;
                    break;


#ifndef DVDIMAGE_ONLY

                case 'j':
                    switch ( tolower( *( ++p ) )) {
                        case '1':
                            bGenerateShortNames = TRUE;

                            // fall through, don't break
                        case '2':
                            bEncodeJoliet       = TRUE;
                            bUnicodeNames       = TRUE;
                            bOptimizeFileNames  = TRUE;
                            break;
                        case 's':
                            bJS = TRUE;
                            strcpy( StubSourceFile, p + 1 );
                            if ( *StubSourceFile == 0 ) {
                                Usage();
                                }
                            break;
                        default:
                            Usage();
                        }
                    argv[ i ] = NULL;
                    break;

                case 'w':

                    //
                    //  Assume at least level 1.
                    //

                    bReportExceedLevels    = TRUE;
                    bReportInvalidIsoNames = TRUE;

                    p++;

                    if ( *p >= '2' )
                        bReportInvalidDosNames = TRUE;

                    if ( *p >= '3' )
                        bReportZeroLength = TRUE;

                    if ( *p >= '4' )
                        bReportAllFiles = TRUE;

                    argv[ i ] = NULL;
                    break;

                case 'n':

                    bAllowLongNames = TRUE;
                    bOptimizeFileNames = TRUE;

                    if ( tolower( *( p + 1 )) == 't' )
                        bRestrictToNt3xCompatibleNames = TRUE;

                    argv[ i ] = NULL;
                    break;

                case 'd':
                    bDontUpcase = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 'c':
                    bUseAnsiFileNames = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 'o':

                    bOptimizeStorage   = TRUE;  // store duplicate files once
                    bOptimizeFileNames = TRUE;  // default, no trailing ";1"
                    bOptimizeFast      = TRUE;  // default, use MD5

                    for (;;) {

                        switch ( tolower( *( ++p ) )) {

                            case 'x':           // obsolete, now the default
                            case 'f':           // obsolete, now the default
                                continue;

                            case 's':
                                bShowDuplicates = TRUE;
                                continue;

                            case 'i':
                                bOptimizeDiamond = TRUE;
                                continue;

                            case 'c':
                                bOptimizeFast = FALSE;  // revert to slow binary comparisons
                                continue;

#ifdef DONTCOMPILE
                            case 'd':

                                bOptimizeDepthFirst = TRUE;

                                if ( *( p + 1 ) == '[' ) {
                                    p = p + 2;
                                    for (;;) {
                                        char *q = strchr( p, ',' );
                                        if ( q == NULL )
                                            q = strchr( p, ']' );
                                        if ( q == NULL )
                                            Usage();
                                        if ( q > p ) {
                                            if ( nDirectoryFirst >= MAX_SPECIFIED_DIRS ) {
                                                ErrorExit( 0, "ERROR: Can specify up to %d directories with %s\r\n", MAX_SPECIFIED_DIRS, "-od" );
                                                }
                                            DirectoryFirst[ nDirectoryFirst ] = MyAllocNeverFree(( q - p ) + 1 );
                                            memcpy( DirectoryFirst[ nDirectoryFirst++ ], p, q - p );
                                            }
                                        if ( *q == ']' ) {
                                            p = q;
                                            break;
                                            }
                                        p = q + 1;
                                        }
                                    }

                                continue;

                            case 'a':

                                bOptimizeDepthAfter = TRUE;

                                if ( *( p + 1 ) == '[' ) {
                                    p = p + 2;
                                    for (;;) {
                                        char *q = strchr( p, ',' );
                                        if ( q == NULL )
                                            q = strchr( p, ']' );
                                        if ( q == NULL )
                                            Usage();
                                        if ( q > p ) {
                                            if ( nDirectoryFirst >= MAX_SPECIFIED_DIRS ) {
                                                ErrorExit( 0, "ERROR: Can specify up to %d directories with %s\r\n", MAX_SPECIFIED_DIRS, "-oa" );
                                                }
                                            DirectoryFirst[ nDirectoryFirst ] = MyAllocNeverFree(( q - p ) + 1 );
                                            memcpy( DirectoryFirst[ nDirectoryFirst++ ], p, q - p );
                                            }
                                        if ( *q == ']' ) {
                                            p = q;
                                            break;
                                            }
                                        p = q + 1;
                                        }
                                    }

                                continue;

                            case 'z':

                                if ( *( p + 1 ) == '[' ) {
                                    p = p + 2;
                                    for (;;) {
                                        char *q = strchr( p, ',' );
                                        if ( q == NULL )
                                            q = strchr( p, ']' );
                                        if ( q == NULL )
                                            Usage();
                                        if ( q > p ) {
                                            if ( nReverseSort >= MAX_SPECIFIED_DIRS ) {
                                                ErrorExit( 0, "ERROR: Can specify up to %d directories with %s\r\n", MAX_SPECIFIED_DIRS, "-oz" );
                                                }
                                            ReverseSort[ nReverseSort ] = MyAllocNeverFree(( q - p ) + 1 );
                                            memcpy( ReverseSort[ nReverseSort++ ], p, q - p );
                                            }
                                        if ( *q == ']' ) {
                                            p = q;
                                            break;
                                            }
                                        p = q + 1;
                                        }
                                    }
                                else {
                                    bOptimizeReverseSort = TRUE;
                                    }

                                continue;

#endif // DONTCOMPILE

                            case ';':

                                //
                                //  Undocumented -; option disables the filename optimizations
                                //  and always appends the ";0" to filenames and puts trailing
                                //  dots on filenames without extensions.  This behavior is
                                //  typically enabled with -n or -o.
                                //

                                bSuppressOptimizeFileNames = TRUE;
                                continue;

                            case 0:
                                break;

                            default:        // invalid -o option
                                Usage();
                            }
                        break;
                        }
                    argv[ i ] = NULL;
                    break;

                case 'm':
                    bIgnoreMaxImageSize = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 'a':
                    bAllocationSummary = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 'x':
                    bEncodeAutoCrc = TRUE;

                    if ( tolower( *( p + 1 )) == 'x' ) {    // -xx
                        bCrcCorrectionInTvd = TRUE;
                        }

                    argv[ i ] = NULL;
                    break;

                case 'b':
                    bElTorito = TRUE;
                    ReadElToritoBootSectorFile( p + 1 );
                    argv[ i ] = NULL;

                    //
                    //  The etfsboot.com El Torito boot sector file that we're
                    //  currently using for NT product media does not correctly
                    //  handle filenames encoded with trailing ";1", so we'll
                    //  explicitly turn off ";1" encoding here.  This can be
                    //  overridden with the -; or -y1 switches.
                    //

                    bOptimizeFileNames = TRUE;
                    break;

                case 'q':
                    bScanOnly = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 'k':
                    bContinueAfterFailedSourceOpen = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 's':
                    {
                    unsigned length = strlen( p + 1 );

                    if ( length == 0 ) {
                        ErrorExit( 0, "ERROR: Must specify server and RPC endpoint name immediately following -s\r\n" );
                        }

                    SignatureRpcEndpointName = MyAllocNeverFree( length + 1 );
                    memcpy( SignatureRpcEndpointName, p + 1, length + 1 );
                    bSignImageAttempt = TRUE;
                    bSignImage = TRUE;
                    argv[ i ] = NULL;
                    break;
                    }

                case ';':

                    //
                    //  Undocumented -; option disables the filename optimizations
                    //  and always appends the ";0" to filenames and puts trailing
                    //  dots on filenames without extensions.  This behavior is
                    //  typically enabled with -n or -o.
                    //

                    bSuppressOptimizeFileNames = TRUE;
                    argv[ i ] = NULL;
                    break;

                case 'y':

                    for (;;) {
                        switch ( tolower( *( ++p ) )) {

                            case '1':
                            case ';':
                                bSuppressOptimizeFileNames = TRUE;
                                continue;

                            case '2':
                                bRoundDirectorySizes = TRUE;
                                continue;

#ifdef DONTCOMPILE

                            case '3':
                                bOneDirectoryRecordPerSector = TRUE;
                                bRoundDirectorySizes = TRUE;
                                continue;

                            case '4':
                                bOneDirectoryRecordPerTwoSectors = TRUE;
                                bRoundDirectorySizes = TRUE;
                                continue;

#endif // DONTCOMPILE

                            case '5':
                                bHackI386Directory = TRUE;
                                continue;

                            case '6':
                                bAllowExactAlignedDirs = TRUE;
                                continue;

                            case '7':
                                bReportBadShortNames = TRUE;
                                continue;

                            case 'd':
                                bSuppressWarnSameFirstBlockNotIdentical = TRUE;
                                continue;

                            case 'b':
                                dwBlockSize = 512;
                                continue;

                            case 0:
                                break;

                            case 'w':
                                bOpenReadWrite=TRUE;
                                break;

                            case 'l':
                                bUdfUseLongADs=TRUE;
                                break;

                            case 'r':
								srand(time(NULL));
                                bUdfUseRandomExtents=TRUE;
                                break;

                            case 't':
                                ElToritoLoadSegment = (WORD)strtoul( p + 1, NULL, 16 );
                                break;

                            case 'f':
                                bFastShortNameGeneration = TRUE;
                                break;

                            default:        // invalid -t option
                                Usage();
                            }

                        break;
                        }

                    argv[ i ] = NULL;
                    break;


#endif // DVDIMAGE_ONLY

                default:
                    ErrorExit( 0, "ERROR: Invalid flag \"-%c\"\r\n", *p );

                }
            }

        else {

            //
            //  Assume this is a file pattern argument.  Just leave it alone.
            //

            }

        }

    if (bUdfXbox) {

        bUdf8BitFileNames = TRUE;

        // -u1

        bEncodeUdf          = TRUE;
        bUnicodeNames       = TRUE;
        bOptimizeFileNames  = TRUE;
        bIgnoreMaxImageSize = TRUE;

        if (bElTorito) {

            // -j1

            bGenerateShortNames = TRUE;

        } else {

            // -u2

            bEncodeOnlyUdf      = TRUE;

            bUdfEmbedFID=TRUE;
        }
    }

#ifdef DVDIMAGE_ONLY

    if ( ! bEncodeOnlyUdf ) {

        bEncodeUdf          = TRUE;
        bUnicodeNames       = TRUE;
        bEncodeJoliet       = TRUE;
        bGenerateShortNames = TRUE;
        bOptimizeFileNames  = TRUE;
        bIgnoreMaxImageSize = TRUE;

        }

#endif // DVDIMAGE_ONLY

    if ( bSuppressOptimizeFileNames ) {
        bOptimizeFileNames = FALSE;
        }

#ifdef DONTCOMPILE

    if ( bOptimizeDepthFirst || bOptimizeDepthAfter || bOptimizeReverseSort || nDirectoryFirst || nReverseSort ) {
        bModifyOrder = TRUE;
        }

    if ( bOptimizeDepthFirst && bOptimizeDepthAfter ) {
        ErrorExit( 0, "ERROR: Cannot specify both -od and -oa\r\n" );
        }

#endif // DONTCOMPILE

    if (( bEncodeOnlyUdf ) && ( bAllowLongNames || bDontUpcase || bEncodeJoliet || bOptimizeDiamond)) {
        ErrorExit( 0, "ERROR: With -u2, cannot use -n, -nt, -d, -j1, -j2, or -oi\r\n" );
    }

    if(bFastShortNameGeneration)
    {
        printf("Using fast short name generation will cause different short names to be generated\r\n");
    }

    if (( !bEncodeOnlyUdf ) && ( bUdfSparse || bUdfEmbedData || bUdfEmbedFID)) {
        ErrorExit( 0, "ERROR: Cannot use -ue, -us, -uf without -u2\r\n" );
        }

    if(bUdfVideoZone && !bEncodeUdf)
    {
        ErrorExit(0, "ERROR: Cannot use -uv without -u1 or -u2\r\n");
    }

    if(bUdfVideoZone && (bUdfSparse || bUdfEmbedData || bUdfEmbedFID || bOptimizeStorage))
    {
        ErrorExit(0, "ERROR: Cannot use -uv with -us -ue -uf or -o \r\n");
    }

    if(!bEncodeOnlyUdf && bEncodeUdf && (bUdfSparse || bUdfEmbedData))
    {
        ErrorExit(0, "ERROR: Cannot use -us or -uf with -u1\r\n");
    }

    if(bEncodeUdf && bAllowLongNames)
    {
        ErrorExit(0, "ERROR: Cannot use -n or -nt with -u1 or -u2\r\n");
    }

    if(bEncodeUdf && !bEncodeOnlyUdf && (bUdfUseLongADs || bUdfUseRandomExtents))
    {
        ErrorExit(0, "ERROR: Cannot use -yl or -yr with -u1\r\n");
    }

    if(bUdfSparse)
    {
        printf("\r\nWARNING: This image may not work on Windows9x due to sparse files\r\n");
    }

    if (( bEncodeUdf ) && ( bAllocationSummary || bEncodeAutoCrc || bSignImage)) {
        ErrorExit( 0, "ERROR: With -u1 and -u2, cannot use -a, -s, or -x\r\n" );
        }

    if (( bEncodeJoliet ) && ( bAllowLongNames || bDontUpcase )) {
        ErrorExit( 0, "ERROR: With -j1 and -j2, cannot use -n, -nt, or -d\r\n" );
        }

    if (( bEncodeJoliet ) && ( strlen( chVolumeLabel ) > 16 )) {
        ErrorExit( 0, "ERROR: Maximum Joliet Unicode volume label length is 16 characters\r\n" );
        }

    if (( bDontUpcase ) && ( bRestrictToNt3xCompatibleNames )) {
        ErrorExit( 0, "ERROR: With -nt, cannot use -d\r\n" );
        }

    if (( bReportBadShortNames ) && ( bContinueAfterFailedSourceOpen )) {
        ErrorExit( 0, "ERROR: With -k, cannot use -y7\r\n" );
        }

    if (( dwBlockSize != CD_SECTORSIZE ) && ( bEncodeUdf )) {
        ErrorExit( 0, "ERROR: With -u1 and -u2, cannot use -yb (512 byte blocks)\r\n" );
        }

    if (( bJS ) && (( ! bEncodeJoliet ) || ( bGenerateShortNames ))) {
        ErrorExit( 0, "ERROR: Can only use -js with -j2\r\n" );
        }

    if (( bUS ) && ( ! bEncodeOnlyUdf )) {
        ErrorExit( 0, "ERROR: Can only use -us with -u2\r\n" );
        }

    if ((( bEncodeJoliet ) && ( ! bGenerateShortNames )) || ( bEncodeOnlyUdf )) {
        GetStubFileContents();
        }

    if ( dwBlockSize != CD_SECTORSIZE ) {
        printf( "\r\nWARNING: 512 byte blocksizes are for testing only!  DO NOT use for retail discs!\r\n" );
        }

    }


void ParseTimeStamp( LPSTR pArg ) {

    //
    //  Expected form: month/day/year/hour/minute/second
    //  with any non-digit character(s) being a delimiter.
    //

    SYSTEMTIME st;
    UINT i, arg[ 6 ] = { 0, 0, 0, 0, 0, 0 };
    char *p = pArg;

    if ( *p ) {

        for ( i = 0; i < 6; i++ ) {

            while ( IsDigit( *p ))
                arg[ i ] = ( arg[ i ] * 10 ) + ( *p++ - '0' );

            while (( *p ) && ( ! IsDigit( *p ))) ++p;

            }

        if ( arg[ 2 ] < 1000 ) {
            ErrorExit( 0, "ERROR: Must specify a 4 digit year with -t\r\n" );
        }

        st.wYear         = (WORD) arg[ 2 ];
        st.wMonth        = (WORD) arg[ 0 ];
        st.wDay          = (WORD) arg[ 1 ];
        st.wHour         = (WORD) arg[ 3 ];
        st.wMinute       = (WORD) arg[ 4 ];
        st.wSecond       = (WORD) arg[ 5 ];
        st.wMilliseconds = 0;

        if (( st.wYear   > 2150 ) ||
            ( st.wMonth  >   12 ) ||
            ( st.wDay    >   31 ) ||
            ( st.wHour   >   23 ) ||
            ( st.wMinute >   59 ) ||
            ( st.wSecond >   59 ) ||
            ( ! SystemTimeToFileTime( &st, &ftGlobalFileTime )))
            {
            ErrorExit( 0, "ERROR: Invalid time: %s\r\n", pArg );
            }
        }
    }


void Usage( void ) {

    printf(

"Usage: " CDIMAGE_NAME " [options] sourceroot targetfile\r\n"
"\r\n"
"       -l  volume label, no spaces (e.g. -lMYLABEL)\r\n"
"       -t  time stamp for all files and directories, no spaces, any delimiter\r\n"
"             (e.g. -t12/31/2000,15:01:00)\r\n"
"       -g  encode GMT time for files rather than local time\r\n"
"       -h  include hidden files and directories\r\n"

#ifdef XBOX

"       -xbox  encode a file system compatible with Xbox\r\n"
"       -o  optimize storage by encoding duplicate files only once\r\n"
"       -oc slower duplicate file detection using binary comparisons rather\r\n"
"             than MD5 hash values\r\n"
"       -os show duplicate files while creating image\r\n"
"           (-o options can be combined like -ocs)\r\n"
"       -q  scan source files only, don't create an image file\r\n"
"       -w  warning level followed by number (e.g. -w4)\r\n"
"           1  report non-ISO or non-Joliet compliant filenames or depth\r\n"
"           2  report non-DOS compliant filenames\r\n"
"           3  report zero-length files\r\n"
"           4  report each file name copied to image\r\n"

          );

#else

#ifdef DVDIMAGE_ONLY

"       -u1 encode \"UDF-Bridge\" media\r\n"
"       -u2 encode \"UDF\" file system without a mirror ISO-9660 file system\r\n"
"             (requires a UDF capable operating system to read the files)\r\n"
"       -ur non-UDF \"readme.txt\" file for images encoded with -u2 option\r\n"
"             (e.g. -usc:\\location\\readme.txt). This file will be visible as\r\n"
"             the only file in the root directory of the disc on systems that\r\n"
"             do not support the UDF format.\r\n"
"       -us sparse UDF files\r\n"
"       -ue embed file data in UDF extent entry\r\n"
"       -uf embed UDF FID entries\r\n"
"       -uv UDF Video Zone compatibility enforced\r\n"

#else // ! DVDIMAGE_ONLY

"       -n  allow long filenames (longer than DOS 8.3 names)\r\n"
"       -nt allow long filenames, restricted to NT 3.51 compatibility\r\n"
"             (-nt and -d cannot be used together)\r\n"
"       -d  don't force lowercase filenames to uppercase\r\n"
"       -c  use ANSI filenames versus OEM filenames from source\r\n"
"       -j1 encode Joliet Unicode filenames AND generate DOS-compatible 8.3\r\n"
"             filenames in the ISO-9660 name space (can be read by either\r\n"
"             Joliet systems or conventional ISO-9660 systems, but some of the\r\n"
"             filenames in the ISO-9660 name space might be changed to comply\r\n"
"             with DOS 8.3 and/or ISO-9660 naming restrictions)\r\n"
"       -j2 encode Joliet Unicode filenames without standard ISO-9660 names\r\n"
"             (requires a Joliet operating system to read files from the CD)\r\n"
"           When using the -j1 or -j2 options, the -n, -nt, and -d options\r\n"
"             do not apply and cannot be used.\r\n"
"       -js non-Joliet \"readme.txt\" file for images encoded with -j2 option\r\n"
"             (e.g. -jsc:\\location\\readme.txt). This file will be visible as\r\n"
"             the only file in the root directory of the disc on systems that\r\n"
"             do not support the Joliet format (Windows 3.1, NT 3.x, etc).\r\n"
"       -u1 encode \"UDF-Bridge\" media\r\n"
"       -u2 encode \"UDF\" file system without a mirror ISO-9660 file system\r\n"
"             (requires a UDF capable operating system to read the files)\r\n"
"       -ur non-UDF \"readme.txt\" file for images encoded with -u2 option\r\n"
"             (e.g. -usc:\\location\\readme.txt). This file will be visible as\r\n"
"             the only file in the root directory of the disc on systems that\r\n"
"             do not support the UDF format.\r\n"
"       -us sparse UDF files\r\n"
"       -ue embed file data in UDF extent entry\r\n"
"       -uf embed UDF FID entries\r\n"
"       -uv UDF Video Zone compatibility enforced\r\n"
"       -b  \"El Torito\" boot sector file, no spaces\r\n"
"             (e.g. -bc:\\location\\cdboot.bin)\r\n"
"       -s  sign image file with digital signature (no spaces, provide RPC\r\n"
"             server and endpoint name like -sServerName:EndPointName)\r\n"
"       -x  compute and encode \"AutoCRC\" values in image\r\n"
"       -o  optimize storage by encoding duplicate files only once\r\n"
"       -oc slower duplicate file detection using binary comparisons rather\r\n"
"             than MD5 hash values\r\n"
"       -oi ignore diamond compression timestamps when comparing files\r\n"
"       -os show duplicate files while creating image\r\n"
"           (-o options can be combined like -ocis)\r\n"

#ifdef DONTCOMPILE

"       -od order directory and file contents in depth-as-encountered order\r\n"
"             versus level-first order, and store specified directories ahead\r\n"
"             of all others (just -od or use -od[dir1,dir2,dir2\\subdir1] to\r\n"
"             specify directories to store first)\r\n"
"       -oa order directory and file contents in depth-after-encountered order\r\n"
"             versus level-first order, and store specified directories ahead\r\n"
"             of all others (just -od or use -oa[dir1,dir2,dir2\\subdir1] to\r\n"
"             specify directories to store first)\r\n"
"       -oz order directory and file contents in reverse alphabetical order\r\n"
"             for the directories specified or for all directories if none\r\n"
"             are specified (just -oz or -oz[dir1,dir2,dir2\\subdir1])\r\n"
"           (-o options can be combined like -ocisd[dir1,dir2]z[dir1,dir2])\r\n"

#endif // DONTCOMPILE

"       -w  warning level followed by number (e.g. -w4)\r\n"
"           1  report non-ISO or non-Joliet compliant filenames or depth\r\n"
"           2  report non-DOS compliant filenames\r\n"
"           3  report zero-length files\r\n"
"           4  report each file name copied to image\r\n"
"       -y  test option followed by number (e.g. -y1), used to generate\r\n"
"             non-standard variations of ISO-9660 for testing purposes:\r\n"
"           1 encode trailing version number ';1' on filenames (7.5.1)\r\n"
"           2 round directory sizes to multiples of 2K (6.8.1.3)\r\n"

#ifdef DONTCOMPILE

"           3 one directory record per logical sector (6.8.1.3)\r\n"
"           4 one directory record per two logical sectors (6.8.1.3)\r\n"

#endif // DONTCOMPILE

"           5 write \\i386 directory files first, in reverse sort order\r\n"
"           6 allow directory records to be exactly aligned at ends of sectors\r\n"
"               (ISO-9660 6.8.1.1 conformant but breaks MSCDEX)\r\n"
"           7 warn about generated shortnames for 16-bit apps under NT 4.0\r\n"
"           b blocksize 512 bytes rather than 2048 bytes\r\n"
"           d suppress warning for non-identical files with same initial 64K\r\n"
"           l UDF - long ads used in file entries instead of short ads\r\n"
"           r UDF - number of ad's is random\r\n"
"           w open source files with write sharing\r\n"
"           t load segment in hex for El Torito boot image (e.g. -yt7C0)\n"
"           f use a faster way to generate short names\n"
"       -k  (keep) create image even if fail to open some of the source files\r\n"
"       -m  ignore maximum image size of " MAX_IMAGE_BYTES_TEXT " bytes\r\n"
"       -a  allocation summary shows file and directory sizes\r\n"
"       -q  scan source files only, don't create an image file\r\n"

#endif // ! DVDIMAGE_ONLY

"\r\n"

          );

#ifndef DVDIMAGE_ONLY

    printf(

"       NOTE: Many of these options allow you to create CD images\r\n"
"             that are NOT compliant with ISO-9660 and may also\r\n"
"             NOT be compatibile with one or more operating systems.\r\n"
"             If you want strict ISO and DOS compliance, use the -w2\r\n"
"             warning level and correct any discrepencies reported.\r\n"
"             YOU are responsible for insuring that any generated CDs\r\n"
"             are compatible with all appropriate operating systems.\r\n"
"             Also note that Microsoft company information is placed\r\n"
"             in the image volume header, so don't use this program\r\n"
"             to generate CDs for companies other than Microsoft.\r\n"
"\r\n"

          );

#endif // ! DVDIMAGE_ONLY

#endif

    fflush( stdout );
    exit( 1 );

    }


BOOL IsDigit( char ch ) {

    if (( ch >= '0' ) && ( ch <= '9' ))
        return TRUE;
    else
        return FALSE;
    }


BOOL IsAcceptableDosName( LPSTR pszFileName ) {

    LPSTR  pBegin, pDot, pEnd;
    PUCHAR p;
    UCHAR  c;

    //
    //  Check for 8.3 format and valid characters.  Make sure no more
    //  than one '.' in name and that it's not the first character.
    //  We only check for minimal invalid characters here versus the
    //  more strict IsValidDosName() test.  Note that all extended
    //  (>127) characters are considered "valid" by this test.
    //

    pBegin = pszFileName;
    pEnd   = strchr( pBegin, '\0' );
    pDot   = strchr( pBegin, '.' );

    if ( pDot == NULL )
        pDot = pEnd;
    else {
        if ( strchr( pDot + 1, '.' ))
            return FALSE;
        }

    if (( pDot == pBegin ) || (( pDot - pBegin ) > 8 ) || (( pEnd - pDot ) > 4 ))
        return FALSE;

    for ( p = (PUCHAR) pszFileName; ( c = *p ) != '\0'; p++ ) {
        if (( c <= 0x20 ) || ( strchr( "*?:;,+<>/\\\"\'[]", c )))
            return FALSE;

        }

    return TRUE;
    }


BOOL IsAcceptableDosNameW( LPWSTR pszUnicodeName ) {

    LPWSTR pBegin, pDot, pEnd;
    PWCHAR p;
    WCHAR  c;

    //
    //  Check for 8.3 format and valid characters.  Make sure no more
    //  than one '.' in name and that it's not the first character.
    //  We only check for minimal invalid characters here versus the
    //  more strict IsValidDosName() test.  Note that all extended
    //  (>127) characters are considered "valid" by this test.
    //

    pBegin = pszUnicodeName;
    pEnd   = wcschr( pBegin, L'\0' );
    pDot   = wcschr( pBegin, L'.' );

    if ( pDot == NULL )
        pDot = pEnd;
    else {
        if ( wcschr( pDot + 1, L'.' ))
            return FALSE;
        }

    if (( pDot == pBegin ) || (( pDot - pBegin ) > 8 ) || (( pEnd - pDot ) > 4 ))
        return FALSE;

    for ( p = (PWCHAR) pszUnicodeName; ( c = *p ) != L'\0'; p++ ) {
        if (( c <= 0x20 ) || ( wcschr( (PWCHAR)L"*?:;,+<>/\\\"\'[]", c )))
            return FALSE;

        }

    return TRUE;
    }


BOOL IsValidDosName( LPSTR pszFileName ) {

    LPSTR  pBegin, pDot, pEnd;
    PUCHAR p;
    UCHAR  c;

    //
    //  Check for 8.3 format and valid characters.  Make sure no more
    //  than one '.' in name and that it's not the first character.
    //  List of valid DOS filename characters taken from Microsoft
    //  MS-DOS Programmer's Reference.
    //

    pBegin = pszFileName;
    pEnd   = strchr( pBegin, '\0' );
    pDot   = strchr( pBegin, '.' );

    if ( pDot == NULL )
        pDot = pEnd;
    else {
        if ( strchr( pDot + 1, '.' ))
            return FALSE;
        }

    if (( pDot == pBegin ) || (( pDot - pBegin ) > 8 ) || (( pEnd - pDot ) > 4 ))
        return FALSE;

    for ( p = (PUCHAR) pszFileName; ( c = *p ) != '\0'; p++ ) {

        if ( ! ((( c >= 'A' ) && ( c <= 'Z' )) ||
                (( c >= 'a' ) && ( c <= 'z' )) ||
                (( c >= '0' ) && ( c <= '9' )) ||
                ( strchr( ".!#$%^&()-_{}~", c ))))

            return FALSE;

        }

    return TRUE;

    }


BOOL IsValidIsoDirectoryName( LPSTR pszFileName ) {

    ULONG nLen = strlen( pszFileName );
    ULONG i;
    CHAR  c;

    if ( nLen > 31 )
        return FALSE;

    for ( i = 0; i < nLen; i++ ) {

        c = pszFileName[ i ];

        if ( ! ((( c >= 'A' ) && ( c <= 'Z' )) ||
                (( c >= '0' ) && ( c <= '9' )) ||
                (( c == '_' )))) {

            return FALSE;
            }
        }

    return TRUE;

    }


BOOL IsValidIsoFileName( LPSTR pszFileName ) {

    ULONG nLen = strlen( pszFileName );
    LPSTR pDot = strchr( pszFileName, '.' );
    ULONG i;
    CHAR  c;

    if ( pDot ) {

        if ( strchr( pDot + 1, '.' ))
            return FALSE;                               // more than one '.'

        if ( nLen > 31 )
            return FALSE;

        }

    else {

        if ( nLen > 30 )
            return FALSE;

        }

    for ( i = 0; i < nLen; i++ ) {

        c = pszFileName[ i ];

        if ( ! ((( c >= 'A' ) && ( c <= 'Z' )) ||
                (( c >= '0' ) && ( c <= '9' )) ||
                (( c == '_' ) || ( c == '.' )))) {

            return FALSE;
            }
        }

    return TRUE;
    }


BOOL IsValidNt3xCompatibleFileName( LPSTR pszFileName ) {

    signed char *p = pszFileName;

    while ( *p != 0 )
        if ( *p++ <= 32 )
            return FALSE;

    return TRUE;

    }


BOOL IsValidNt3xCompatibleDirectoryName( LPSTR pszFileName ) {

    signed char *p = pszFileName;

    while ( *p != 0 )
        if ( *p++ <= 32 )
            return FALSE;

    if (( p - pszFileName ) > 37 )  // NT barfs on dirnames longer than 37
        return FALSE;

    return TRUE;

    }


#ifdef DONTCOMPILE  // use memcmp instead

UINT CompareAligned( PVOID p1, PVOID p2, UINT nSize ) {

    //
    //  Buffers are expected to be DWORD aligned.  The return
    //  value is the offset of the miscompare, or nSize if
    //  buffers are equivalent.  The offset returned may be a
    //  non-DWORD aligned number such as 6 if that is the byte
    //  offset of the first miscompare.
    //

    UINT nAligned = nSize & ( ~ ( sizeof( DWORD ) - 1 ));
    UINT n = 0;

    while (( n < nAligned ) && ( *(PDWORD)p1 == *(PDWORD)p2 )) {
        p1 = (PDWORD)p1 + 1;
        p2 = (PDWORD)p2 + 1;
        n += sizeof( DWORD );
        }

    while (( n < nSize ) && ( *(PUCHAR)p1 == *(PUCHAR)p2 )) {
        p1 = (PUCHAR)p1 + 1;
        p2 = (PUCHAR)p2 + 1;
        n++;
        }

    return n;
    }

#endif // DONTCOMPILE

ULONG EncodingNameLength( LPSTR pFileName, BOOL IsDirectory ) {

    ULONG NameLength = strlen( pFileName );

    if (( ! IsDirectory ) && ( ! bOptimizeFileNames )) {
        if ( ! strchr( pFileName, '.' ))
            NameLength += 3;        // trailing ".;1"
        else
            NameLength += 2;        // trailing ";1"
        }

    return NameLength;
    }



BOOL IsFileNameDotOrDotDot( LPCSTR pszFileName ) {

    //
    //  What we're looking for is DOT-NULL or DOT-DOT-NULL.  Any trash
    //  beyond the NULL is legal, so we'll mask off all but first two
    //  bytes for the DOT-NULL and three bytes for the DOT-DOT-NULL
    //  comparisons.  Note dependence on little-endian byte ordering.
    //

    DWORD dwFirstFourBytes = *(UNALIGNED DWORD *)pszFileName;

    if ((( dwFirstFourBytes & 0x0000FFFF ) == 0x0000002E ) ||
        (( dwFirstFourBytes & 0x00FFFFFF ) == 0x00002E2E )) {

        return TRUE;
        }

    return FALSE;
    }


BOOL IsFileNameDotOrDotDotW( LPCWSTR pszUnicodeName ) {

    //
    //  What we're looking for is DOT-NULL or DOT-DOT-NULL.  Any trash
    //  beyond the NULL is legal, so we'll mask off all but first two
    //  chars for the DOT-NULL and three chars for the DOT-DOT-NULL
    //  comparisons.  Note dependence on little-endian byte ordering.
    //

    DWORDLONG dwlFirstEightBytes = *(UNALIGNED DWORDLONG *)pszUnicodeName;
    DWORD     dwFirstFourBytes = (DWORD)dwlFirstEightBytes;

    if (( dwFirstFourBytes == 0x0000002E ) ||
        (( dwlFirstEightBytes & 0x0000FFFFFFFFFFFF )
                             == 0x00000000002E002E )) {

        return TRUE;
        }

    return FALSE;
    }



LPSTR GenerateFullName( PDIRENTRY pDir, LPSTR pBuffer ) {

    if ( pDir->pParentDir == pDir )             // root parent points to itself
        *pBuffer = 0;
    else {
        GenerateFullName( pDir->pParentDir, pBuffer );
        strcat( pBuffer, "\\" );
        strcat( pBuffer, pDir->pszFileName );
        }

    return pBuffer;
    }


LPWSTR GenerateFullNameW( PDIRENTRY pDir, LPWSTR pBuffer ) {

    if ( pDir->pParentDir == pDir )             // root parent points to itself
        *pBuffer = 0;
    else {
        GenerateFullNameW( pDir->pParentDir, pBuffer );
        lstrcatW( pBuffer, (LPCWSTR) L"\\" );
        lstrcatW( pBuffer, pDir->pszUnicodeName );
        }

    return pBuffer;
    }


LPSTR GenerateFullNameSource( PDIRENTRY pDir, LPSTR pBuffer ) {

    GetFilePath( pDir->pParentDir, pBuffer );
    strcat( pBuffer, pDir->pszFileName );
    return pBuffer;
    }


LPWSTR GenerateFullNameSourceW( PDIRENTRY pDir, LPWSTR pBuffer ) {

    GetFilePathW( pDir->pParentDir, pBuffer );
    lstrcatW( pBuffer, pDir->pszUnicodeName );
    return pBuffer;
    }


void ReportAllocationFiles( PDIRENTRY pDirectory ) {

    PDIRENTRY pNode;
    DWORDLONG dwlRawSize, dwlAllocSize;

    ASSERT( pDirectory->pDirectoryInfo != NULL );

    for ( pNode = pDirectory->pDirectoryInfo->pFirstTargetFile; pNode; pNode = pNode->pNextTargetFile ) {

        if ( pNode->dwFlags & IS_DIRECTORY ) {

            ASSERT( pNode->pDirectoryInfo != NULL );

            if ( bEncodeJoliet ) {

                ASSERT( pNode->pDirectoryInfo->dwUnicodeDirSize != 0 );

                dwlRawSize   = pNode->pDirectoryInfo->dwUnicodeDirSize;
                dwlAllocSize = ROUNDUP2_64( dwlRawSize, CD_SECTORSIZE );

                dwlReportRawSize   += dwlRawSize;
                dwlReportAllocSize += dwlAllocSize;

                GenerateFullNameW( pNode, ReportNameBuffer.W );

                printf(
                    "%10I64d %10I64d %5s %S\r\n",
                    dwlRawSize,
                    dwlAllocSize,
                    "[DIR]",
                    ReportNameBuffer.W
                    );

                if ( bGenerateShortNames ) {

                    ASSERT( pNode->pszFileName != NULL );
                    ASSERT( pNode->dwlFileSize != 0 );

                    dwlRawSize   = pNode->dwlFileSize;
                    dwlAllocSize = ROUNDUP2_64( dwlRawSize, CD_SECTORSIZE );

                    dwlReportRawSize   += dwlRawSize;
                    dwlReportAllocSize += dwlAllocSize;

                    GenerateFullName( pNode, ReportNameBuffer.A );

                    printf(
                        "%10I64d %10I64d %5s %s\r\n",
                        dwlRawSize,
                        dwlAllocSize,
                        "[DIR]",
                        ReportNameBuffer.A
                        );

                    }
                }

            else {

                ASSERT( pNode->dwlFileSize != 0 );

                dwlRawSize   = pNode->dwlFileSize;
                dwlAllocSize = ROUNDUP2_64( dwlRawSize, CD_SECTORSIZE );

                dwlReportRawSize   += dwlRawSize;
                dwlReportAllocSize += dwlAllocSize;

                GenerateFullName( pNode, ReportNameBuffer.A );

                printf(
                    "%10I64d %10I64d %5s %s\r\n",
                    dwlRawSize,
                    dwlAllocSize,
                    "[DIR]",
                    ReportNameBuffer.A
                    );

                }

            ReportAllocationFiles( pNode );

            }

        else {      // not a directory

            dwlRawSize = pNode->dwlFileSize;

            dwlAllocSize = ( pNode->dwFlags & IS_DUPLICATE ) ? 0 :
                           ROUNDUP2_64( dwlRawSize, dwBlockSize );

            dwlReportRawSize   += dwlRawSize;
            dwlReportAllocSize += dwlAllocSize;

            if ( bEncodeJoliet ) {

                GenerateFullNameW( pNode, ReportNameBuffer.W );

                printf(
                    "%10I64d %10I64d %5s %S\r\n",
                    dwlRawSize,
                    dwlAllocSize,
                    "",
                    ReportNameBuffer.W
                    );

                if ( bGenerateShortNames ) {

                    ASSERT( pNode->pszFileName != NULL );

                    GenerateFullName( pNode, ReportNameBuffer.A );

                    printf(
                        "                            %s\r\n",
                        ReportNameBuffer.A
                        );

                    }
                }

            else {

                GenerateFullName( pNode, ReportNameBuffer.A );

                printf(
                    "%10I64d %10I64d %5s %s\r\n",
                    dwlRawSize,
                    dwlAllocSize,
                    "",
                    ReportNameBuffer.A
                    );

                }
            }
        }
    }


void ReportAllocationSummary( void ) {

    printf( "\r\n"
            "===================== Allocation Summary =====================\r\n"
            "\r\n"
            "  Raw Size   Blk Size\r\n"
            " ---------  ---------\r\n"

          );

    printf( "%10d %10d %5s %s\r\n",
            16 * CD_SECTORSIZE,
            16 * CD_SECTORSIZE,
            "[SYS]",
            "[ISO-9660 System Area (not used)]"
          );

    printf( "%10d %10d %5s %s\r\n",
            CD_SECTORSIZE,
            CD_SECTORSIZE,
            "[SYS]",
            "[ISO-9660 Primary Volume Descriptor]"
          );

    dwlReportRawSize   = 17 * CD_SECTORSIZE;
    dwlReportAllocSize = 17 * CD_SECTORSIZE;

    if ( bElTorito ) {

        printf( "%10d %10d %5s %s\r\n",
                CD_SECTORSIZE,
                CD_SECTORSIZE,
                "[SYS]",
                "[ISO-9660 Boot Volume Descriptor (El Torito)]"
              );

        dwlReportRawSize   += CD_SECTORSIZE;
        dwlReportAllocSize += CD_SECTORSIZE;

        }

    if ( bEncodeJoliet ) {

        printf( "%10d %10d %5s %s\r\n",
                CD_SECTORSIZE,
                CD_SECTORSIZE,
                "[SYS]",
                "[ISO-9660 Secondary Volume Descriptor (Joliet)]"
              );

        dwlReportRawSize   += CD_SECTORSIZE;
        dwlReportAllocSize += CD_SECTORSIZE;

        }

    printf( "%10d %10d %5s %s\r\n",
            CD_SECTORSIZE,
            CD_SECTORSIZE,
            "[SYS]",
            "[ISO-9660 Volume Descriptor Terminator]"
          );

    dwlReportRawSize   += CD_SECTORSIZE;
    dwlReportAllocSize += CD_SECTORSIZE;

    ASSERT( dwlReportAllocSize == ((DWORDLONG)dwHeaderBlocks * dwBlockSize ));

    if ( bElTorito ) {

        printf( "%10d %10d %5s %s\r\n",
                CD_SECTORSIZE,
                CD_SECTORSIZE,
                "[SYS]",
                "[El Torito Boot Catalog]"
              );

        dwlReportRawSize   += CD_SECTORSIZE;
        dwlReportAllocSize += CD_SECTORSIZE;

        printf( "%10d %10d %5s %s\r\n",
                ElToritoBootSectorFileSize,
                ROUNDUP2( ElToritoBootSectorFileSize, CD_SECTORSIZE ),
                "[SYS]",
                "[El Torito Boot Sector File]"
              );

        dwlReportRawSize   += ElToritoBootSectorFileSize;
        dwlReportAllocSize += ROUNDUP2( ElToritoBootSectorFileSize, CD_SECTORSIZE );

        }

    if ( bEncodeJoliet ) {

        printf( "%10d %10d %5s %s\r\n",
                dwJolietLookupTableSize,
                dwJolietLookupTableAllocation,
                "[SYS]",
                "[Joliet Type-L Path Table]"
              );

        printf( "%10d %10d %5s %s\r\n",
                dwJolietLookupTableSize,
                dwJolietLookupTableAllocation,
                "[SYS]",
                "[Joliet Type-M Path Table]"
              );

        dwlReportRawSize   += dwJolietLookupTableSize       * 2;
        dwlReportAllocSize += dwJolietLookupTableAllocation * 2;

        }

    printf( "%10d %10d %5s %s\r\n",
            dwLookupTableSize,
            dwLookupTableAllocation,
            "[SYS]",
            "[ISO-9660 Type-L Path Table]"
          );

    printf( "%10d %10d %5s %s\r\n",
            dwLookupTableSize,
            dwLookupTableAllocation,
            "[SYS]",
            "[ISO-9660 Type-M Path Table]"
          );

    dwlReportRawSize   += dwLookupTableSize       * 2;
    dwlReportAllocSize += dwLookupTableAllocation * 2;

    if ( bEncodeJoliet && ( ! bGenerateShortNames )) {

        printf( "%10d %10d %5s %s\r\n",
                dwStubFileSize,
                ROUNDUP2( dwStubFileSize, dwBlockSize ),
                "",
                "[Joliet Stub File for Non-Joliet Systems]"
              );

        dwlReportRawSize   += dwStubFileSize;
        dwlReportAllocSize += ROUNDUP2( dwStubFileSize, dwBlockSize );

        }

    if ( dwlMetaPaddingBeforeDirectoriesOffset ) {

        printf( "%10d %10d %5s %s\r\n",
                dwMetaPaddingBeforeDirectoriesBytes,
                dwMetaPaddingBeforeDirectoriesBytes,
                "",
                "[Padding to align directories on sector boundary]"
              );

        dwlReportRawSize   += dwMetaPaddingBeforeDirectoriesBytes;
        dwlReportAllocSize += dwMetaPaddingBeforeDirectoriesBytes;

        }

    //
    //  Need to add padding blocks to get block alignment to sector alignment
    //  for first directory (root directory) entry.
    //

    if ( bEncodeAutoCrc ) {

        //
        //  Actually, the CRC block comes after the directory blocks but
        //  before the first file block.  But, since we're reporting
        //  directories mixed with files, we'll report the CRC block
        //  allocation here.
        //

        printf( "%10d %10d %5s %s\r\n",
                CD_SECTORSIZE,
                CD_SECTORSIZE,
                "[SYS]",
                "[AutoCRC Header Signature Block]"
              );

        dwlReportRawSize   += CD_SECTORSIZE;
        dwlReportAllocSize += CD_SECTORSIZE;

        }

    if ( bEncodeJoliet ) {

        printf( "%10d %10d %5s %s\r\n",
                pRootDir->pDirectoryInfo->dwUnicodeDirSize,
                ROUNDUP2( pRootDir->pDirectoryInfo->dwUnicodeDirSize, CD_SECTORSIZE ),
                "[DIR]",
                "\\" );

        printf( "%10d %10d %5s %s\r\n",
                (DWORD)( pRootDir->dwlFileSize ),
                ROUNDUP2( pRootDir->dwlFileSize, CD_SECTORSIZE ),
                "[DIR]",
                "\\" );

        dwlReportRawSize   += pRootDir->pDirectoryInfo->dwUnicodeDirSize + pRootDir->dwlFileSize;
        dwlReportAllocSize += ROUNDUP2( pRootDir->pDirectoryInfo->dwUnicodeDirSize, CD_SECTORSIZE )
                            + ROUNDUP2( pRootDir->dwlFileSize, CD_SECTORSIZE );

        }

    else {

        printf( "%10d %10d %5s %s\r\n",
                (DWORD)( pRootDir->dwlFileSize ),
                ROUNDUP2( pRootDir->dwlFileSize, CD_SECTORSIZE ),
                "[DIR]",
                "\\" );

        dwlReportRawSize   += pRootDir->dwlFileSize;
        dwlReportAllocSize += ROUNDUP2( pRootDir->dwlFileSize, CD_SECTORSIZE );

        }

    ReportAllocationFiles( pRootDir );

    if ( dwSlackBytesToWriteAtEndOfFiles ) {

        printf( "%10d %10d %5s %s\r\n",
                dwSlackBytesToWriteAtEndOfFiles,
                dwSlackBytesToWriteAtEndOfFiles,
                "",
                "[Padding to align end of image on sector boundary]"
              );

        dwlReportRawSize   += dwSlackBytesToWriteAtEndOfFiles;
        dwlReportAllocSize += dwSlackBytesToWriteAtEndOfFiles;

        }

    //
    //  Need to add padding blocks to get block alignment to sector alignment
    //  for final CRC block.
    //

    if ( bEncodeAutoCrc ) {

        printf( "%10d %10d %5s %s\r\n",
                CD_SECTORSIZE,
                CD_SECTORSIZE,
                "[SYS]",
                "[AutoCRC Image Signature Block]"
              );

        dwlReportRawSize   += CD_SECTORSIZE;
        dwlReportAllocSize += CD_SECTORSIZE;

        }

    printf( " ---------  ---------\r\n" );

    printf( "%10I64d %10I64d [TOT] %I64d files in %I64d directories\r\n\r\n",
            dwlReportRawSize,
            dwlReportAllocSize,
            dwlTotalNumberOfFiles,
            dwlTotalNumberOfDirectories );

    ASSERT( dwlReportAllocSize == dwlTotalImageBytes );

    }


LPWSTR
AnsiToUnicode(
    IN              LPSTR  AnsiBuffer,
    IN OUT OPTIONAL LPWSTR UnicodeBuffer
    )
    {
    int Length = strlen( AnsiBuffer ) + 1;
    int Result;

    if ( UnicodeBuffer == NULL ) {
        UnicodeBuffer = MyAllocNeverFree( Length * 2 );
        }

    Result = MultiByteToWideChar(
                bUseAnsiFileNames ? CP_ACP : CP_OEMCP,
                0,
                AnsiBuffer,
                Length,
                UnicodeBuffer,
                Length
                );

    if ( Result == 0 ) {

        DEBUGCODE(
            ErrorExit(
                GETLASTERROR,
                "ERROR: AnsiToUnicode( \"%s\" ) conversion failed\r\n",
                AnsiBuffer
                )
            );
        }

    return UnicodeBuffer;
    }


LPSTR UnicodeToAnsi(
    IN              LPWSTR UnicodeBuffer,
    IN OUT OPTIONAL LPSTR  AnsiBuffer
    )
    {
    int Length = lstrlenW( UnicodeBuffer ) + 1;
    int Result;

    if ( AnsiBuffer == NULL ) {
        AnsiBuffer = MyAllocNeverFree( Length );
        }

    Result = WideCharToMultiByte(
                bUseAnsiFileNames ? CP_ACP : CP_OEMCP,
                0,
                UnicodeBuffer,
                Length,
                AnsiBuffer,
                Length,
                NULL,
                NULL
                );

    if ( Result == 0 ) {

        DEBUGCODE(
            ErrorExit(
                GETLASTERROR,
                "ERROR: UnicodeToAnsi( \"%S\" ) conversion failed\r\n",
                UnicodeBuffer
                )
            );
        }

    return AnsiBuffer;
    }


int __fastcall CompareRawUnicodeStrings( LPCWSTR String1, LPCWSTR String2 ) {

    PUSHORT p1 = (PUSHORT)String1;
    PUSHORT p2 = (PUSHORT)String2;

    while (( *p1 == *p2 ) && ( *p1 )) {
        ++p1;
        ++p2;
        }

    return ( *p1 < *p2 ) ? -1 : ( *p1 > *p2 ) ? +1 : 0;
    }


PDIRENTRY JolietInsertNode( PDIRENTRY pFirst, PDIRENTRY pNew ) {

    PDIRENTRY pPrev, pNext;

    for ( pPrev = NULL, pNext = pFirst;
          pNext != NULL;
          pPrev = pNext, pNext = pNext->pNextTargetFile ) {

        //
        //  According to RickDew, use the "bag of bytes" sorting
        //  method rather than case-insensitive sorting or ISO-9660
        //  sorting (filename then extension).
        //

        if ( CompareRawUnicodeStrings( pNew->pszUnicodeName, pNext->pszUnicodeName ) < 0 )
            break;

        }

    pNew->pNextTargetFile = pNext;

    if ( pPrev == NULL )
        return pNew;
    else {
        pPrev->pNextTargetFile = pNew;
        return pFirst;
        }

    }


void ComputeJolietDirectorySizes( void ) {

    PDIRENTRY pDir, pFile;
    DWORD dwDirOffset, dwTableOffset, dwNextBound, dwEntryLen;
    DWORD dwNameLength;
    UINT uLevel;
    WCHAR OddNameBuffer[ 112 ];      // 110 is absolute max name length
    WCHAR EvenNameBuffer[ 112 ];
    UINT NameIndex = 0;

    dwTableOffset = 0;
    nDirectories = 0;

    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {

        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );

            //
            //  Each table entry is 8 bytes plus the length in bytes of the
            //  name, but the next entry must start at a word (2-byte)
            //  boundary, so round-up offset if necessary.
            //

            ASSERT( pDir->wUnicodeNameLength != 0 );

            dwNameLength = pDir->wUnicodeNameLength;

            if ( pDir != pRootDir )             // except for root dir name
                dwNameLength *= 2;              // unicode characters to bytes

            if (( dwTableOffset += ( 8 + dwNameLength )) & 1 )
                ++dwTableOffset;

            pDir->pDirectoryInfo->dwUnicodeIndex = ++nDirectories;

            if ( pDir->pParentDir->pDirectoryInfo->dwUnicodeIndex > 0xFFFF ) {

                ErrorExit(
                    0,
                    "ERROR: Too many directories in volume (directory number of a parent directory\r\n"
                    "       cannot exceed 65535 because it is stored in a 16-bit field).\r\n"
                    );
                }

            dwDirOffset = 34 * 2;               // for "." and ".." entries

            dwNextBound = CD_SECTORSIZE;        // can't straddle a sector boundary

            if ( bReportBadShortNames ) {
                ZeroMemory( OddNameBuffer,  sizeof( OddNameBuffer ));
                ZeroMemory( EvenNameBuffer, sizeof( EvenNameBuffer ));
                NameIndex = 2;                  // for . and .. entries
                }

            //
            //  Note that MSCDEX will never read the Joliet namespace, so
            //  not necessary to avoid exactly aligned directory records
            //  at ends of sectors.
            //

            for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {

                ASSERT( pFile->wUnicodeNameLength != 0 );
                ASSERT( pFile->wUnicodeNameLength <= 110 );

                dwEntryLen  = ( pFile->wUnicodeNameLength * 2 ) + 33;
                dwEntryLen += dwEntryLen & 1;

                if (( dwDirOffset + dwEntryLen ) > dwNextBound ) {
                    dwDirOffset  = dwNextBound;
                    dwNextBound += CD_SECTORSIZE;
                    }

                DEBUGCODE( pFile->dwUnicodeDirOffset = dwDirOffset );

                if ( bReportBadShortNames ) {

                    //
                    //  Check for potential conflict with bug in NT 4.0
                    //  where CDFS shortname generation fails when the hex
                    //  offset of the direntry shifted right 5 bits
                    //  contains hex characters 'a' through 'f' (non-
                    //  numeric hex characters).
                    //

                    PWCHAR WhichNameBuffer = ( NameIndex++ & 1 ) ? OddNameBuffer : EvenNameBuffer;
                    BOOL   CheckShortName  = ( pFile->dwFlags & CHECK_SHORTNAME ) ? TRUE : FALSE;
                    BOOL   IsDirectory     = ( pFile->dwFlags & IS_DIRECTORY    ) ? TRUE : FALSE;

                    if ( ! CheckShortName ) {

                        if ( bOptimizeFileNames || IsDirectory ) {

                            if (( WhichNameBuffer[ pFile->wUnicodeNameLength ] == '.' ) ||
                                ( WhichNameBuffer[ pFile->wUnicodeNameLength ] == ' ' )) {

                                CheckShortName = TRUE;

                                }
                            }
                        }

                    if ( CheckShortName ) {

                        if (( ! IsDirectory ) && ( ! bOptimizeFileNames )) {

                            if ( IsAcceptableDosNameW( pFile->pszUnicodeName )) {

                                CheckShortName = FALSE;

                                }
                            }
                        }

                    if ( CheckShortName ) {

                        CHAR  Uniquifier[ 12 ];
                        PCHAR p;

                        sprintf( Uniquifier, "%X", dwDirOffset >> 5 );

                        for ( p = Uniquifier; *p; p++ ) {

                            if ( *p > '9' ) {

                                printf(
                                    "\r\nWARNING: %sname \"%S\" may be inaccessible to 16-bit apps "
                                    "under NT 4.0 (see footnote).\r\n",
                                    IsDirectory ? "Directory " : "File",
                                    GenerateFullNameW( pFile, ReportNameBuffer.W )
                                    );

                                fflush( stdout );
                                bReportBadShortNameWarning = TRUE;
                                break;

                                }
                            }
                        }

                    CopyMemory( WhichNameBuffer, pFile->pszUnicodeName, pFile->wUnicodeNameLength * 2 );

                    }

                dwDirOffset += dwEntryLen;

                }

            //
            //  ISO-9660 6.8.1.3 specifies that the length of a directory
            //  INCLUDES the unused bytes at the end of the last sector,
            //  meaning this number should be rounded up to CD_SECTORSIZE.
            //  Since NT 3.1 media, we have always encode actual versus
            //  rounded and never heard of any problems.
            //

            if ( bRoundDirectorySizes ) {
                pDir->pDirectoryInfo->dwUnicodeDirSize = ROUNDUP2( dwDirOffset, CD_SECTORSIZE );
                }
            else {
                pDir->pDirectoryInfo->dwUnicodeDirSize = dwDirOffset;
                }

            }
        }

    dwJolietLookupTableSize       = dwTableOffset;
    dwJolietLookupTableAllocation = ROUNDUP2( dwTableOffset, dwBlockSize );

    }



void WritePrimaryVolumeDescriptor( ULONG SectorNumber ) {

    PUCHAR pHeader;
    DWORD  dwBlock;

    ASSERT( SectorNumber == 16 );       // PVD must be at sector 16

    pHeader = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pHeader[   0 ] = 0x01;      // primary volume descriptor
    pHeader[   6 ] = 0x01;      // volume descriptor version
    pHeader[ 881 ] = 0x01;      // file structure version

    memcpy(  pHeader +   1, "CD001", 5 );

    PadCopy( pHeader +   8,  32, "" );
    PadCopy( pHeader +  40,  32, uchVolumeLabel );          // upper case label
    PadCopy( pHeader + 190, 128, uchVolumeLabel );          // upper case label
    PadCopy( pHeader + 318, 128, "MICROSOFT CORPORATION" );
    PadCopy( pHeader + 446, 128, "MICROSOFT CORPORATION, ONE MICROSOFT WAY, REDMOND WA 98052, (425) 882-8080" );
    PadCopy( pHeader + 574, 128, MAJOR_VERSION_TEXT " " MINOR_VERSION_TEXT );
    PadCopy( pHeader + 702, 111, "" );

    memcpy(  pHeader + 813, IsoVolumeCreationTime, 17 );    // volume creation time
    memcpy(  pHeader + 830, "0000000000000000",    17 );    // last modification time (not specified)
    memcpy(  pHeader + 847, "0000000000000000",    17 );    // expiration time (not specified)
    memcpy(  pHeader + 864, "0000000000000000",    17 );    // effective time (not specified)

    // total volume size in blocks
    MakeDualEndian( pHeader +  80, 4, dwTotalImageBlocks );

    // volume set size (always 1)
    MakeDualEndian( pHeader + 120, 2, 1 );

    // volume sequence (always 1)
    MakeDualEndian( pHeader + 124, 2, 1 );

    // logical block size (2048 or 512)
    MakeDualEndian( pHeader + 128, 2, dwBlockSize );

    // path table size (in bytes [including terminating 0-byte?])
    MakeDualEndian( pHeader + 132, 4, dwLookupTableSize );

    ASSERT( dwStartingIsoPathTableBlock != 0 );

    dwBlock = dwStartingIsoPathTableBlock;

    // Type-L path table block location
    MakeLittleEndian( pHeader + 140, 4, dwBlock );
    dwBlock += CD_BLOCKS_FROM_SIZE( dwLookupTableAllocation );

    // Type-M path table block location
    MakeBigEndian( pHeader + 148, 4, dwBlock );
    dwBlock += CD_BLOCKS_FROM_SIZE( dwLookupTableAllocation );

    // Root directory "." entry gets copied at offset 156

    MakeRootEntry( pHeader + 156, pRootDir, 0 );

    HeaderWrite( SectorNumber * CD_SECTORSIZE, pHeader, CD_SECTORSIZE );

    }


void WriteJolietVolumeDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    DWORD  dwBlock;

    ASSERT( SectorNumber > 16 );

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pBuffer[   0 ] = 0x02;      // secondary volume descriptor
    pBuffer[   6 ] = 0x01;      // volume descriptor version
    pBuffer[ 881 ] = 0x01;      // file structure version

    memcpy(  pBuffer +   1, "CD001", 5 );

    memcpy(  pBuffer +  88, "%/E", 3 );                     // Joliet UCS-2 Level 3 escape sequence

    UniCopy( pBuffer +   8,  32, "" );
    UniCopy( pBuffer +  40,  32, chVolumeLabel );           // mixed case label
    UniCopy( pBuffer + 190, 128, chVolumeLabel );           // mixed case label
    UniCopy( pBuffer + 318, 128, "Microsoft Corporation" );
    UniCopy( pBuffer + 446, 128, "Microsoft Corporation, One Microsoft Way, Redmond WA 98052" );
    UniCopy( pBuffer + 574, 128, MAJOR_VERSION_TEXT " " MINOR_VERSION_TEXT );
    UniCopy( pBuffer + 702, 111, "" );

    memcpy(  pBuffer + 813, IsoVolumeCreationTime, 17 );    // volume creation time
    memcpy(  pBuffer + 830, "0000000000000000",    17 );    // last modification time (not specified)
    memcpy(  pBuffer + 847, "0000000000000000",    17 );    // expiration time (not specified)
    memcpy(  pBuffer + 864, "0000000000000000",    17 );    // effective time (not specified)

    // total volume size in blocks
    MakeDualEndian( pBuffer +  80, 4, dwTotalImageBlocks );

    // volume set size (always 1)
    MakeDualEndian( pBuffer + 120, 2, 1 );

    // volume sequence (always 1)
    MakeDualEndian( pBuffer + 124, 2, 1 );

    // logical block size (2048 or 512)
    MakeDualEndian( pBuffer + 128, 2, dwBlockSize );

    // path table size (in bytes [including terminating 0-byte?])
    MakeDualEndian( pBuffer + 132, 4, dwJolietLookupTableSize );

    ASSERT( dwStartingJolietPathTableBlock != 0 );

    dwBlock = dwStartingJolietPathTableBlock;

    // Type-L path table block location
    MakeLittleEndian( pBuffer + 140, 4, dwBlock );
    dwBlock += CD_BLOCKS_FROM_SIZE( dwJolietLookupTableAllocation );

    // Type-M path table block location
    MakeBigEndian( pBuffer + 148, 4, dwBlock );
    dwBlock += CD_BLOCKS_FROM_SIZE( dwJolietLookupTableAllocation );

    // Root directory "." entry gets copied at offset 156

    MakeJolietRootEntry( pBuffer + 156, pRootDir, 0 );

    HeaderWrite( SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


const unsigned char CompaqBootPatch[] = {
    0xFA, 0x33, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C, 0x8B, 0xF4, 0x8E, 0xC0,
    0x8E, 0xD8, 0xFB, 0xEB, 0x51, 0x43, 0x44, 0x2D, 0x52, 0x4F, 0x4D, 0x20,
    0x6E, 0x6F, 0x74, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C, 0x65,
    0x20, 0x6F, 0x6E, 0x20, 0x74, 0x68, 0x69, 0x73, 0x20, 0x73, 0x79, 0x73,
    0x74, 0x65, 0x6D, 0x2E, 0x52, 0x65, 0x6D, 0x6F, 0x76, 0x65, 0x20, 0x43,
    0x44, 0x2D, 0x52, 0x4F, 0x4D, 0x20, 0x61, 0x6E, 0x64, 0x20, 0x70, 0x72,
    0x65, 0x73, 0x73, 0x20, 0x45, 0x4E, 0x54, 0x45, 0x52, 0x20, 0x6B, 0x65,
    0x79, 0x20, 0x74, 0x6F, 0x20, 0x63, 0x6F, 0x6E, 0x74, 0x69, 0x6E, 0x75,
    0x65, 0x2E, 0xBD, 0x11, 0x7C, 0xB4, 0x13, 0xB0, 0x00, 0xB9, 0x23, 0x00,
    0xB6, 0x00, 0xB2, 0x00, 0xB7, 0x00, 0xB3, 0x07, 0xCD, 0x10, 0xBD, 0x34,
    0x7C, 0xB4, 0x13, 0xB0, 0x00, 0xB9, 0x2E, 0x00, 0xB6, 0x01, 0xB2, 0x00,
    0xB7, 0x00, 0xB3, 0x07, 0xCD, 0x10, 0xB4, 0x00, 0xCD, 0x16, 0x3C, 0x00,
    0x74, 0xF8, 0x80, 0xFC, 0x1C, 0x75, 0xF3, 0xEA, 0xF0, 0xFF, 0x00, 0xF0
    };

//
//  The above 16-bit x86 code stream is produced from this code sequence:
//
//  .MODEL tiny
//  .DATA
//
//  .CODE
//  ORG 7C00h
//
//  Start:
//
//  CLI                                     ;Disable interrupts
//  XOR  AX,AX                              ;Initialize registers
//  MOV  SS,AX                              ;and segment pointers
//  MOV  SP,7C00h                           ;
//  MOV  SI,SP                              ;Setup stack at load point
//  MOV  ES,AX                              ;
//  MOV  DS,AX                              ;
//
//  STI                                     ;Enable interrupts
//
//  JMP CODE                                ;
//  BOOT_STRING  DB  'CD-ROM not bootable on this system.'
//  BOOT_STRING2 DB  'Remove CD-ROM and press ENTER key to continue.'
//
//  CODE:
//
//  MOV  BP,OFFSET BOOT_STRING              ;Setup string pointer for display
//  MOV  AH,13H                             ;Service
//  MOV  AL,0                               ;sub service
//  MOV  CX,35                              ;String size
//  MOV  DH,0                               ;Row
//  MOV  DL,0                               ;Collumn
//  MOV  BH,0                               ;Display Page
//  MOV  BL,07H                             ;Attribute
//
//  INT  10H                                ;
//
//  MOV  BP,OFFSET BOOT_STRING2             ;Setup string pointer for display
//  MOV  AH,13H                             ;Service
//  MOV  AL,0                               ;sub service
//  MOV  CX,46                              ;String size
//  MOV  DH,1                               ;Row
//  MOV  DL,0                               ;Collumn
//  MOV  BH,0                               ;Display Page
//  MOV  BL,07H                             ;Attribute
//
//  INT  10H                                ;
//
//  KEY_LOOP:
//  MOV AH,00H                              ;Service (Read next KeyChar)
//  INT 16H                                 ;Wait for key press
//  CMP AL,0H                               ;Check for extended scan code
//  JZ  KEY_LOOP                            ;
//  CMP AH,1CH                              ;If ENTER pressed...
//  JNZ KEY_LOOP                            ;
//                                          ;Reboot
//  DB  0EAH                                ;Far JMP
//  DW  0FFF0H,0F000H                       ;Offset:Segment
//
//  END Start
//


void WriteElToritoBootVolumeDescriptor( ULONG SectorNumber ) {

    PUCHAR pBuffer;

    ASSERT( SectorNumber == 17 );   // El Torito BVD must be at sector 17

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pBuffer[ 0 ] = 0x00;        // boot volume descriptor
    pBuffer[ 6 ] = 0x01;        // volume descriptor version

    memcpy( pBuffer + 1, "CD001", 5 );
    memcpy( pBuffer + 7, "EL TORITO SPECIFICATION", 23 );

    *(UNALIGNED DWORD*)( pBuffer + 0x47 ) = SECTOR_NUMBER_FROM_BLOCK_NUMBER( ElToritoBootCatalogBlock );

    //
    //  06/10/96  Compaq gave us this instruction stream to place at
    //            offset 512 in the El Torito BVD to prevent errant
    //            BIOS from locking up on CD-ROM boot and prompt user
    //            to remove CD before rebooting.
    //

    memcpy( pBuffer + 512, CompaqBootPatch, sizeof( CompaqBootPatch ));

    HeaderWrite( SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


void WriteVolumeDescriptorTerminator( ULONG SectorNumber ) {

    PUCHAR pBuffer;

    ASSERT( SectorNumber > 16 );

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    pBuffer[ 0 ] = 0xFF;        // terminator volume descriptor
    pBuffer[ 6 ] = 0x01;        // volume descriptor version

    memcpy( pBuffer + 1, "CD001", 5 );

    if ( bCrcCorrectionInTvd ) {

        DWORD PartialCrc = Crc32( HeaderCrc, pBuffer, ( CD_SECTORSIZE - 4 ));

        *(UNALIGNED DWORD*)( pBuffer + ( CD_SECTORSIZE - 4 )) = PartialCrc;

        }

    HeaderWrite( SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


void WriteElToritoBootCatalog( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    DWORD  CheckSum;
    BYTE   BootMediaType;
    WORD   BootSectorCount;
    ULONG  i;

    ASSERT( SectorNumber == SECTOR_NUMBER_FROM_BLOCK_NUMBER( ElToritoBootCatalogBlock ));

    pBuffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    //
    //  First 0x20 bytes is the validation entry
    //

    pBuffer[ 0x00 ] = 0x01;     // Validation Entry Header ID
    pBuffer[ 0x01 ] = 0x00;     // Platform ID = 80x86

    memcpy( pBuffer + 4, "Microsoft Corporation", 21 );

    pBuffer[ 0x1E ] = 0x55;     // key byte
    pBuffer[ 0x1F ] = 0xAA;     // key byte

    for ( CheckSum = 0, i = 0; i < 0x20; i += 2 ) {
        CheckSum += *(UNALIGNED WORD*)( pBuffer + i );
        }

    CheckSum = ( 0x10000 - ( CheckSum & 0xFFFF ));

    *(UNALIGNED WORD*)( pBuffer + 0x1C ) = (WORD)CheckSum;

#ifdef DEBUG

    //
    //  Verify checksum algorithm
    //

    for ( CheckSum = 0, i = 0; i < 0x20; i += 2 ) {
        CheckSum += *(UNALIGNED WORD*)( pBuffer + i );
        }

    ASSERT(( CheckSum & 0xFFFF ) == 0 );

#endif // DEBUG

    //
    //  Second 0x20 bytes is the initial/default entry.
    //

    //
    //  If ElToritoBootSectorFileSize is exactly the size of a floppy image,
    //  we'll assume that floppy emulation mode is desired and set the
    //  appropriate BootMediaType and set the BootSectorCount to 1.
    //  Otherwise, we assume the boot sector file is a "no emulation mode"
    //  boot loader program and load the whole file from the CD at boot time.
    //

    switch ( ElToritoBootSectorFileSize ) {
        case 1228800:
            BootMediaType   = 1;        //  1.2MB floppy
            BootSectorCount = 1;
            break;
        case 1474560:
            BootMediaType   = 2;        //  1.44MB floppy
            BootSectorCount = 1;
            break;
        case 2949120:
            BootMediaType   = 3;        //  2.88MB floppy
            BootSectorCount = 1;
            break;
        default:
            BootMediaType   = 0;        //  "no emulation" mode
            BootSectorCount = (WORD)( ROUNDUP2( ElToritoBootSectorFileSize, 0x200 ) / 0x200 );
            break;
        }

    pBuffer[ 0x20 ] = 0x88;             // Initial/Default Entry Boot Indicator
    pBuffer[ 0x21 ] = BootMediaType;

    // Load Segment (where to load in DOS memory).  If zero, loads at 0x07C0.

    *(UNALIGNED WORD*)(pBuffer + 0x22) = ElToritoLoadSegment;

    // Sector Count (number of 512-byte "virtual sectors" to load from CD):

    *(UNALIGNED WORD*)( pBuffer + 0x26 ) = BootSectorCount;

    // Load address (CD sector number) of boot sector file:

    *(UNALIGNED DWORD*)( pBuffer + 0x28 ) = SECTOR_NUMBER_FROM_BLOCK_NUMBER( ElToritoBootSectorBlock );

    HeaderWrite( SectorNumber * CD_SECTORSIZE, pBuffer, CD_SECTORSIZE );

    }


void WriteElToritoBootSector( ULONG SectorNumber ) {

    PUCHAR pBuffer;
    ULONG  AllocSize;

    AllocSize = ROUNDUP2( ElToritoBootSectorFileSize, CD_SECTORSIZE );

    pBuffer = AllocateBuffer( AllocSize, FALSE );

    CopyMemory( pBuffer, ElToritoBootSectorFileData, ElToritoBootSectorFileSize );

    if ( AllocSize > ElToritoBootSectorFileSize )
        ZeroMemory( pBuffer   + ElToritoBootSectorFileSize,
                    AllocSize - ElToritoBootSectorFileSize );

    HeaderWrite( SectorNumber * CD_SECTORSIZE, pBuffer, AllocSize );

    }


void CopyAndInvertUnicode( LPWSTR TargetBuffer, LPWSTR SourceBuffer, ULONG ByteCount ) {

    PUCHAR Target = (PUCHAR) TargetBuffer;
    PUCHAR Source = (PUCHAR) SourceBuffer;
    UCHAR Byte1, Byte2;

    ByteCount &= ~1;            // insure even

    while ( ByteCount ) {

        Byte1 = *Source++;
        Byte2 = *Source++;

        *Target++ = Byte2;
        *Target++ = Byte1;

        ByteCount -= 2;
        }
    }


void UniCopy( PUCHAR pDest, UINT uLength, LPSTR pSource ) {

    CHAR  AnsiBuffer[ 256 ];
    WCHAR UnicodeBuffer[ 128 ];
    PCHAR p, q;

    ASSERT( uLength < sizeof( AnsiBuffer ));
    ASSERT( strlen( pSource ) < sizeof( AnsiBuffer ));

    strcpy( AnsiBuffer, pSource );

    q = &AnsiBuffer[ uLength / 2 ];

    *q = 0;

    p = strchr( AnsiBuffer, 0 );

    while ( p < q )
        *p++ = ' ';

    AnsiToUnicode( AnsiBuffer, UnicodeBuffer );

    CopyAndInvertUnicode( (LPWSTR)pDest, UnicodeBuffer, uLength );

    }


void GenerateAndWriteJolietLookupTables( void ) {

    PUCHAR pL, pM, pLookupTableL, pLookupTableM;
    PUCHAR pBuffer;
    UINT uLevel, uLen;
    PDIRENTRY pDir;
    DWORD dwTableSize;
    DWORD dwCombinedTableSize;
    DWORDLONG dwlOffset;

    dwTableSize = dwJolietLookupTableAllocation;              // local copy

    ASSERT( ISALIGN2( dwTableSize, dwBlockSize ));

    //
    //  Note: Not sure if Joliet requires a type-M (big endian) path table,
    //  since all known Joliet systems at this time only use the type-L
    //  table, but since Joliet does not explicitly state the type-M table
    //  is optional, and ISO-9660's description of the SVD's type-M table
    //  (8.5.10) does not say the type-M table is optional, we'll always
    //  encode a type-M table even though it is a waste of space on the CD.
    //

    dwCombinedTableSize = dwTableSize * 2;

    pBuffer = AllocateBuffer( dwCombinedTableSize, TRUE );

    pL = pLookupTableL = pBuffer;                       // LittleEndian table
    pM = pLookupTableM = pBuffer + dwTableSize;         // BigEndian table

    //
    // root directory is odd, so do it separately, then start loop at level 1
    //

    *pL = 0x01;     // length of name
    *pM = 0x01;     // length of name

    MakeLittleEndian( pL + 2, 4, pRootDir->pDirectoryInfo->dwUnicodeBlock );
    MakeBigEndian(    pM + 2, 4, pRootDir->pDirectoryInfo->dwUnicodeBlock );

    ASSERT( pRootDir->pParentDir->pDirectoryInfo->dwUnicodeIndex != 0 );
    ASSERT( pRootDir->pParentDir->pDirectoryInfo->dwUnicodeIndex < 0x10000 );

    MakeLittleEndian( pL + 6, 2, pRootDir->pParentDir->pDirectoryInfo->dwUnicodeIndex );
    MakeBigEndian(    pM + 6, 2, pRootDir->pParentDir->pDirectoryInfo->dwUnicodeIndex );

    pL += 10;
    pM += 10;

    for ( uLevel = 1; pStartOfLevel[ uLevel ]; uLevel++ ) {
        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );
            ASSERT( pDir->wUnicodeNameLength != 0 );

            uLen = (UINT) pDir->wUnicodeNameLength * 2;

            *pL = (UCHAR) uLen;
            *pM = (UCHAR) uLen;

            MakeLittleEndian( pL + 2, 4, pDir->pDirectoryInfo->dwUnicodeBlock );
            MakeBigEndian(    pM + 2, 4, pDir->pDirectoryInfo->dwUnicodeBlock );

            ASSERT( pDir->pParentDir->pDirectoryInfo->dwUnicodeIndex != 0 );
            ASSERT( pDir->pParentDir->pDirectoryInfo->dwUnicodeIndex < 0x10000 );

            MakeLittleEndian( pL + 6, 2, pDir->pParentDir->pDirectoryInfo->dwUnicodeIndex );
            MakeBigEndian(    pM + 6, 2, pDir->pParentDir->pDirectoryInfo->dwUnicodeIndex );

            //
            //  Both the L-path table and M-path table get a big-endian
            //  unicode name, so CopyAndInvert the name once into the
            //  L-path table, and then just memcpy to the M-path table.
            //

            CopyAndInvertUnicode( (LPWSTR)( pL + 8 ), pDir->pszUnicodeName, uLen );
            memcpy( pM + 8, pL + 8, uLen );

            pL += ( uLen + 8 ) + ( uLen & 1 );
            pM += ( uLen + 8 ) + ( uLen & 1 );

            ASSERT( pL < ( pLookupTableL + dwTableSize ));
            ASSERT( pM < ( pLookupTableM + dwTableSize ));  // redundant

            }
        }

    //
    //  Joliet lookup tables are written just behind the header blocks.
    //

    ASSERT( dwStartingJolietPathTableBlock != 0 );

    dwlOffset = (DWORDLONG)dwStartingJolietPathTableBlock * dwBlockSize;

    HeaderWrite( dwlOffset, pBuffer, dwCombinedTableSize );

    }


void WriteJolietDirectories( void ) {

    PDIRENTRY pDir, pFile;
    PUCHAR pBuffer, pMem, pBoundary;
    UINT uLevel, uNameLen, uEntryLen;
    DWORD dwDirAllocation;
    DWORDLONG dwlOffset;

    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {
        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {

            ASSERT( pDir->pDirectoryInfo != NULL );
            ASSERT( pDir->pDirectoryInfo->dwUnicodeDirSize != 0 );

            dwDirAllocation = ROUNDUP2( pDir->pDirectoryInfo->dwUnicodeDirSize, CD_SECTORSIZE );

            pBuffer = AllocateBuffer( dwDirAllocation, TRUE );

            pMem = MakeJolietRootEntries( pBuffer, pDir );

            pBoundary = pBuffer + CD_SECTORSIZE;

            //
            //  Note that MSCDEX will never read the Joliet namespace, so
            //  not necessary to avoid exactly aligned directory records
            //  at ends of sectors.
            //

            for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {

                if ( ! ( pFile->dwFlags & NO_DIRENTRY )) {

                    ASSERT( pFile->wUnicodeNameLength != 0 );
                    ASSERT( pFile->dwlFileSize < 0x100000000 );

                    uNameLen   = (UINT) pFile->wUnicodeNameLength * 2;
                    uEntryLen  = uNameLen + 33;
                    uEntryLen += uEntryLen & 1;

                    if (( pMem + uEntryLen ) > pBoundary ) {
                        pMem = pBoundary;
                        pBoundary += CD_SECTORSIZE;
                        }

                    ASSERT(( pFile->dwUnicodeDirOffset == (DWORD)( pMem - pBuffer )) || ( bContinueAfterFailedSourceOpen ));
                    ASSERT(( pMem + uEntryLen ) < ( pBuffer + dwDirAllocation ));

                    *pMem = (UCHAR) uEntryLen;

                    if ( pFile->dwFlags & IS_HIDDEN ) {
                        *( pMem + 25 ) |= 0x01;
                        }

                    if ( pFile->dwFlags & IS_DIRECTORY ) {

                        *( pMem + 25 ) |= 0x02;

                        ASSERT( ! ( pFile->dwFlags & IS_DUPLICATE ));
                        ASSERT( pFile->pDirectoryInfo != NULL );
                        ASSERT( pFile->pDirectoryInfo->dwUnicodeBlock   != 0 );
                        ASSERT( pFile->pDirectoryInfo->dwUnicodeDirSize != 0 );

                        MakeDualEndian( pMem +  2, 4, pFile->pDirectoryInfo->dwUnicodeBlock );
                        MakeDualEndian( pMem + 10, 4, pFile->pDirectoryInfo->dwUnicodeDirSize );

                        }

                    else {

                        //
                        //  pFile->pDirectoryInfo and pFile->pPrimary share the
                        //  same address, so we can only assert that pDirectoryInfo
                        //  is NULL if not IS_DUPLICATE.
                        //

                        ASSERT(( pFile->dwFlags & IS_DUPLICATE ) || ( pFile->pDirectoryInfo == NULL ));
                        ASSERT(( pFile->dwlFileSize == 0 ) || ( pFile->dwStartingBlock != 0 ));

                        MakeDualEndian( pMem +  2, 4, pFile->dwStartingBlock );
                        MakeDualEndian( pMem + 10, 4, (DWORD)( pFile->dwlFileSize ));

                        }

                    *( pMem + 28 ) = 0x01;
                    *( pMem + 31 ) = 0x01;
                    *( pMem + 32 ) = (UCHAR) uNameLen;

                    if ( bUseGlobalTime )
                        memcpy( pMem + 18, cGlobalIsoTimeStamp, 6 );
                    else {
                        ASSERT( pFile->pFileTimeInfo != NULL );
                        MakeIsoTimeStampFromFileTime( pMem + 18, pFile->pFileTimeInfo->ftLastWriteTime );
                        }

                    CopyAndInvertUnicode( (LPWSTR)( pMem + 33 ), pFile->pszUnicodeName, uNameLen );

                    pMem += uEntryLen;

                    }

                else {

                    ASSERT(( pFile->dwFlags & NO_DIRENTRY ) && ( bContinueAfterFailedSourceOpen ));

                    }
                }

            dwlOffset = (DWORDLONG)( pDir->pDirectoryInfo->dwUnicodeBlock ) * dwBlockSize;

            ASSERT( IS_SECTOR_ALIGNED( dwlOffset ));

            HeaderWrite( dwlOffset, pBuffer, dwDirAllocation );

            }
        }
    }


BOOL StripDotsAndSpaces( LPSTR Buffer ) {

    LPSTR Source = Buffer;
    LPSTR Target = Buffer;
    BOOL  Modify = FALSE;
    CHAR ch;

    for (;;) {

        switch ( ch = *Source++ ) {

            case 0:

                *Target = 0;
                return Modify;

            case ' ':
            case '.':

                Modify = TRUE;
                break;

            default:

                *Target++ = ch;

            }
        }
    }


BOOL ReplaceNonIsoCharsWithUnderscore( LPSTR FileName ) {

    LPSTR NamePointer = FileName;
    BOOL Modify = FALSE;
    CHAR ch;

    while (( ch = *NamePointer ) != 0 ) {

        if ( ! ((( ch >= '0' ) && ( ch <= '9' )) ||
                (( ch >= 'A' ) && ( ch <= 'Z' )) ||
                (( ch == '_' )))) {

            *NamePointer = '_';
            Modify = TRUE;
            }

        ++NamePointer;

        }

    return Modify;
    }


BOOL __inline TruncateString( LPSTR String, ULONG Length ) {

    if ( strlen( String ) > Length ) {
        String[ Length ] = 0;
        return TRUE;
        }

    return FALSE;
    }


void GetStubFileContents( void ) {

    if ( *StubSourceFile ) {

        HANDLE hFile;
        DWORD  Actual;
        BOOL   Success;

        StubFileName = strrchr( StubSourceFile, '\\' );

        if ( StubFileName == NULL ) {
             StubFileName = strrchr( StubSourceFile, ':' );
             }

        if ( StubFileName == NULL ) {
             StubFileName = StubSourceFile;
             }
        else {
             StubFileName++;
             }

        hFile = CreateFile(
                    StubSourceFile,
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_SEQUENTIAL_SCAN,
                    NULL
                    );

        if ( hFile == INVALID_HANDLE_VALUE ) {

            ErrorExit(
                GETLASTERROR,
                "ERROR: Could not open stub file \"%s\"\r\n",
                StubSourceFile
                );
            }

        dwStubFileSize = GetFileSize( hFile, NULL );

        if ( dwStubFileSize == 0xFFFFFFFF ) {

            ErrorExit(
                GETLASTERROR,
                "ERROR: Could not determine stub file size \"%s\"\r\n",
                StubSourceFile
                );
            }

        if ( dwStubFileSize == 0 ) {

            ErrorExit(
                0,
                "ERROR: Stub file \"%s\" size is zero\r\n",
                StubSourceFile
                );
            }

        StubFileContents = MyAllocNeverFree( dwStubFileSize );

        Success = ReadFile(
                      hFile,
                      StubFileContents,
                      dwStubFileSize,
                      &Actual,
                      NULL
                      );

        if ( ! Success ) {

            ErrorExit(
                GETLASTERROR,
                "ERROR: Failure reading stub file \"%s\"\r\n",
                StubSourceFile
                );
            }

        if ( Actual != dwStubFileSize ) {

            ErrorExit(
                0,
                "ERROR: Failure reading stub file \"%s\"\r\n"
                "Actual bytes (%d) not equal to requested (%d)\r\n",
                StubSourceFile,
                Actual,
                dwStubFileSize
                );
            }

        GetFileTime( hFile, NULL, NULL, &ftStubTime );

        CloseHandle( hFile );

        _strupr( StubFileName );

        if ( ! IsValidDosName( StubFileName )) {
            ErrorExit(
                0,
                "ERROR: Stub file name \"%s\" is not DOS 8.3 compliant\r\n",
                StubFileName
                );
            }

        if ( ! IsValidIsoFileName( StubFileName )) {
            ErrorExit(
                0,
                "ERROR: Stub file name \"%s\" is not ISO-9660 compliant\r\n",
                StubFileName
                );
            }
        }

    else {

        StubFileName = "README.TXT";

        if ( bEncodeOnlyUdf ) {

            StubFileContents =
"This disc contains a \"UDF\" file system and requires an operating system\r\n"
"that supports the ISO-13346 \"UDF\" file system specification.\r\n";

            }

        else {

            StubFileContents =
"This disc contains Unicode file names and requires an operating system\r\n"
"that supports the ISO-9660 \"Joliet\" CD-ROM file system specification\r\n"
"such as Microsoft Windows 95 or Microsoft Windows NT 4.0.\r\n";

            }

        dwStubFileSize = strlen( StubFileContents );
        ftStubTime = ftGlobalFileTime;

        }
    }


void GenerateStubs( void ) {

    PDIRENTRY pStub = NewDirNode( FALSE );

    ASSERT(   StubFileName != NULL );
    ASSERT(  *StubFileName != 0 );
    ASSERT( dwStubFileSize != 0 );
    ASSERT( StubFileContents != NULL );

    pStub->pszFileName     = StubFileName;
    pStub->wFileNameLength = (WORD)strlen( StubFileName );
    pStub->dwlFileSize     = dwStubFileSize;
    pStub->pParentDir      = pRootDir;

    if ( pStub->pFileTimeInfo ) {
         pStub->pFileTimeInfo->ftLastWriteTime  = ftStubTime;
         pStub->pFileTimeInfo->ftLastAccessTime = ftStubTime;
         pStub->pFileTimeInfo->ftCreationTime   = ftStubTime;
         }

    pRootDir->pDirectoryInfo->pAlternateFirstFile = pStub;

    pAlternateStartOfLevel[ 0 ] = pRootDir;

    }


void GenerateShortNames( void ) {

    pRootDir->pDirectoryInfo->pAlternateFirstFile = RecursiveGenerateShortNames( pRootDir );

    LinkAlternateDirectories( pRootDir, 0 );

    }


PDIRENTRY RecursiveGenerateShortNames( PDIRENTRY pParentDir ) {

    PDIRENTRY pFile, pFirst;

    ASSERT( pParentDir->pDirectoryInfo != NULL );
    ASSERT( pParentDir->pDirectoryInfo->pAlternateFirstFile == NULL );

    pFirst = NULL;

    for ( pFile = pParentDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {

        pFirst = GenerateAndInsertShortName( pFirst, pFile );

        if ( pFile->dwFlags & IS_DIRECTORY ) {

            ASSERT( pFile->pDirectoryInfo != NULL );
            ASSERT( pFile->pDirectoryInfo->pAlternateFirstFile == NULL );

            pFile->pDirectoryInfo->pAlternateFirstFile = RecursiveGenerateShortNames( pFile );

            }
        }

    ASSERT( pParentDir->pDirectoryInfo->pAlternateFirstFile == NULL );

    return pFirst;
    }


UINT DecimalDigitsInInteger( UINT Integer ) {

    return (( Integer <         10 ) ? 1 :
            ( Integer <        100 ) ? 2 :
            ( Integer <       1000 ) ? 3 :
            ( Integer <      10000 ) ? 4 :
            ( Integer <     100000 ) ? 5 :
            ( Integer <    1000000 ) ? 6 :
            ( Integer <   10000000 ) ? 7 :
            ( Integer <  100000000 ) ? 8 :
            ( Integer < 1000000000 ) ? 9 : 10 );
    }

VOID GenerateShortNameForFile( PDIRENTRY pNewFile, LPSTR ShortName , UINT UniqueNumber) {

    CHAR AnsiName[ MAX_NAME_LENGTH ];
    LPSTR Extension;
    LPSTR End;
    BOOL bModified = FALSE;
    UINT AnsiLength;
    UINT Digits;
    UINT Index;

    //
    //  All the generated names in this directory must be unique.
    //

    ASSERT( pNewFile->pszFileName    == NULL );
    ASSERT( pNewFile->pszUnicodeName != NULL );

    UnicodeToAnsi( pNewFile->pszUnicodeName, AnsiName );

    if ( pNewFile->dwFlags & IS_DIRECTORY ) {

        //
        //  Directories can't have extensions.
        //

        Extension = NULL;
        }

    else {

        //
        //  Find extension separator (last dot) and break name
        //  into base and extension by writing a terminator over
        //  the separator.
        //

        Extension = strrchr( AnsiName, '.' );

        if ( Extension != NULL ) {

            *Extension++ = 0;

            StripDotsAndSpaces( Extension );

            TruncateString( Extension, 3 );

            strupr( Extension );

            ReplaceNonIsoCharsWithUnderscore( Extension );

            }
        }

    StripDotsAndSpaces( AnsiName );

    TruncateString( AnsiName, 8 );

    strupr( AnsiName );

    ReplaceNonIsoCharsWithUnderscore( AnsiName );

    AnsiLength = strlen( AnsiName );

    if ( AnsiLength == 0 )
        bModified = TRUE;

    memcpy( ShortName, AnsiName, AnsiLength + 1 );

    if ( bModified || (1 != UniqueNumber))
    {

        // this limits us to 10 million files w/ generated short names if fast generate short names is on

        Digits = DecimalDigitsInInteger( UniqueNumber );

        if ( Digits > 7 ) {
            ErrorExit(
                0,
                "ERROR: Unable to generate unique short name for %S\r\n",
                pNewFile->pszUnicodeName
                );
            }

        Index = AnsiLength;

        if (( Index + Digits ) > 7 ) {
            Index = 7 - Digits;
            }

        sprintf( ShortName + Index, "_%d", UniqueNumber++ );
    }

    if ( Extension ) {

        *( End = strchr( ShortName, 0 )) = '.';
        strcpy( End + 1, Extension );

        }
    }

PDIRENTRY GenerateAndInsertShortName( PDIRENTRY pFirstFile, PDIRENTRY pNewFile )
    {
    PDIRENTRY pPrev, pNext, pReturn;
    INT iCmp;
    CHAR ShortName[ 8 + 3 + 1 + 1 ];
    static UINT UniqueNumber = 1;
    UINT Length;

    //
    //  Since we the generated file name is an ISO-9660 character set name,
    //  we can assume the period is sorted lower than any other character,
    //  so we don't have to replace it for sorting purposes.
    //

    if(!bFastShortNameGeneration)
    {
        UniqueNumber = 1;
    }

    ASSERT(NULL != pNewFile);

    pPrev = NULL;
    pNext = pFirstFile;

    // get the short file name
    GenerateShortNameForFile(pNewFile, ShortName, UniqueNumber++ );
    ASSERT( strlen( ShortName ) > 0 );

    while(NULL != pNext)
    {

        iCmp = strcmp( ShortName, pNext->pszFileName );

        // do we have a collision
        if(0 == iCmp)
        {
            // get a new short file name
            GenerateShortNameForFile(pNewFile, ShortName, UniqueNumber++ );
            ASSERT( strlen( ShortName ) > 0 );

            // do I need to start at the beginning again?
            if(pPrev != NULL)
            {
                // our current position in the list may be invalidated, due to
                // short name generation
                if(strcmp( ShortName, pPrev->pszFileName ) < 0)
                {
                    // reset pointers and loop
                    pPrev = NULL;
                    pNext = pFirstFile;
                }
            }
        }
        else
        {
            // no collision

            // does it fit between the prev & next nodes?
            if ( iCmp < 0 )
            {
                break;          // insert here
            }

            // get ready for next loop
            pPrev = pNext;
            pNext = pNext->pAlternateNextFile;
        }
    }

    // we know where to insert the node, so stick it in
    pNewFile->pAlternateNextFile = pNext;

    if ( pPrev == NULL )
    {
        // list is empty or this node is supposed to go first
        pReturn = pNewFile;
    }
    else
    {
        // it goes in the middle/end of the list

        pNext = pPrev->pAlternateNextFile;

        pPrev->pAlternateNextFile = pNewFile;

        pReturn = pFirstFile;
    }

    // allocate & store the short file name in the node
    ASSERT( pNewFile->pszFileName == NULL );

    Length = strlen( ShortName );
    pNewFile->pszFileName = MyAllocNeverFree( Length + 1 );
    memcpy( pNewFile->pszFileName, ShortName, Length );

    pNewFile->wFileNameLength = (WORD)Length;

    ASSERT( bOptimizeFileNames );       // assume no trailing dot or ";1"

    // return the head node (which might have changed)
    return pReturn;

    }

void LinkAlternateDirectories( PDIRENTRY pDirNode, UINT uLevel ) {

    PDIRENTRY p;

    ASSERT( uLevel < MAX_LEVELS );

    if (( bReportExceedLevels ) && ( uLevel >= 8 )) {
        printf( "\r\nWARNING: Non-ISO directory depth exceeds 8 levels: \"%s\"\r\n",
                GenerateFullName( pDirNode, ReportNameBuffer.A ));
        fflush( stdout );
        }

    p = pAlternateStartOfLevel[ uLevel ];

    if ( p == NULL )
        pAlternateStartOfLevel[ uLevel ] = pDirNode;
    else {
        ASSERT( p->pDirectoryInfo != NULL );
        while ( p->pDirectoryInfo->pAlternateNextDir != NULL ) {
            p = p->pDirectoryInfo->pAlternateNextDir;
            ASSERT( p->pDirectoryInfo != NULL );
            }
        p->pDirectoryInfo->pAlternateNextDir = pDirNode;
        }

    for ( p = pDirNode->pDirectoryInfo->pAlternateFirstFile;
          p != NULL;
          p = p->pAlternateNextFile ) {

          if ( p->dwFlags & IS_DIRECTORY ) {
              ASSERT( p->pDirectoryInfo != NULL );
              LinkAlternateDirectories( p, uLevel + 1 );
              }
          }
    }


void WriteStubs( void ) {

    DWORD     dwAlloc   = ROUNDUP2( dwStubFileSize, dwBlockSize );
    PUCHAR    pBuffer   = AllocateBuffer( dwAlloc, TRUE );
    DWORDLONG dwlOffset = (DWORDLONG) dwStubFileBlock * dwBlockSize;

    CopyMemory( pBuffer, StubFileContents, dwStubFileSize );

    HeaderWrite( dwlOffset, pBuffer, dwAlloc );

    ASSERT( ROUNDUP2_64( dwlOffset + dwAlloc, CD_SECTORSIZE ) == ((DWORDLONG)dwStartingJolietDirectoryBlock * dwBlockSize ));

    }


#ifdef DONTCOMPILE


void ShowIsoTreeRecursive( PDIRENTRY pFirstNode, UINT uLevel ) {

    PDIRENTRY p;

    for ( p = pFirstNode; p != NULL; p = p->pNextTargetFile ) {

        ASSERT( p->pszFileName != NULL );

#ifdef SHOW_EACH_ENTRY

        printf(
            "%*s%s\r\n",
            uLevel * 2,
            "",
            p->pszFileName
            );

#endif

        #error "files and dirs have different rounding"

        if ( ! ( p->dwFlags & IS_DUPLICATE ))
            dwlShowTreeAlloc += ROUNDUP2_64( p->dwlFileSize, dwBlockSize )
                              + p->pDirectoryInfo ?
                                    ROUNDUP2( p->pDirectoryInfo->dwUnicodeDirSize, CD_SECTORSIZE ) :
                                    0;

        if ( p->dwFlags & IS_DIRECTORY )
            ShowIsoTreeRecursive( p->pDirectoryInfo->pFirstTargetFile, uLevel + 1 );

        }

    }


void ShowJolietTreeRecursive( PDIRENTRY pFirstNode, UINT uLevel ) {

    PDIRENTRY p;

    for ( p = pFirstNode; p != NULL; p = p->pNextTargetFile ) {

        ASSERT( p->pszUnicodeName != NULL );

#ifdef SHOW_EACH_ENTRY

        printf(
            "%*s%S\r\n",
            uLevel * 2,
            "",
            p->pszUnicodeName
            );

#endif

        #error "files and dirs have different rounding"

        if ( ! ( p->dwFlags & IS_DUPLICATE ))
            dwlShowTreeAlloc += ROUNDUP2_64( p->dwlFileSize, CD_SECTORSIZE )
                              + p->pDirectoryInfo ?
                                    ROUNDUP2( p->pDirectoryInfo->dwUnicodeDirSize, CD_SECTORSIZE ) :
                                    0;

        if ( p->dwFlags & IS_DIRECTORY )
            ShowJolietTreeRecursive( p->pDirectoryInfo->pFirstTargetFile, uLevel + 1 );

        }
    }


void ShowAlternateTreeRecursive( PDIRENTRY pFirstNode, UINT uLevel ) {

    PDIRENTRY p;

    for ( p = pFirstNode; p != NULL; p = p->pAlternateNextFile ) {

        ASSERT( p->pszFileName != NULL );

#ifdef SHOW_EACH_ENTRY

        printf(
            "%*s%s\r\n",
            uLevel * 2,
            "",
            p->pszFileName
            );

#endif

        #error "files and dirs have different rounding"

        if ( ! ( p->dwFlags & IS_DUPLICATE ))
            dwlShowTreeAlloc += ROUNDUP2_64( p->dwlFileSize, CD_SECTORSIZE )
                              + p->pDirectoryInfo ?
                                    ROUNDUP2( p->pDirectoryInfo->dwUnicodeDirSize, CD_SECTORSIZE ) :
                                    0;

        if ( p->dwFlags & IS_DIRECTORY )
            ShowAlternateTreeRecursive( p->pDirectoryInfo->pAlternateFirstFile, uLevel + 1 );

        }
    }



void ShowIsoTree( void ) {

    dwlShowTreeAlloc = 0;

    ShowIsoTreeRecursive( pRootDir, 0 );

    printf( "Total Iso allocation is %I64d\r\n", dwlShowTreeAlloc );

    }


void ShowJolietTree( void ) {

    dwlShowTreeAlloc = 0;

    ShowJolietTreeRecursive( pRootDir, 0 );

    printf( "Total Joliet allocation is %I64d\r\n", dwlShowTreeAlloc );

    }


void ShowAlternateTree( void ) {

    dwlShowTreeAlloc = 0;

    ShowAlternateTreeRecursive( pRootDir, 0 );

    printf( "Total Alternate allocation is %I64d\r\n", dwlShowTreeAlloc );

    }


#endif // DONTCOMPILE


PDIRENTRY
StartRecursiveDescent(
    VOID
    )
    {
    ULONG NameLength = strlen( szRootPath );
    PDIRENTRY Return;
    HANDLE hFind;

    memcpy( DescentNameBuffer.A, szRootPath, NameLength );

    DescentNameBuffer.A[ NameLength     ] = '*';    // append "*" to DescentNameBuffer
    DescentNameBuffer.A[ NameLength + 1 ] = 0;      // terminate the string

    hFind = FindFirstFileA( DescentNameBuffer.A, &FindData.A );

    if ( hFind == INVALID_HANDLE_VALUE ) {
        ErrorExit(
            GETLASTERROR,
            "ERROR: Failure enumerating files in directory \"%s\"\r\n",
            szRootPath
            );
        }

    FindClose( hFind );

    DescentNameBuffer.A[ NameLength ] = 0;      // chop off the trailing "*"

    Return = RecursiveDescent(
                    pRootDir,
                    DescentNameBuffer.A + NameLength,
                    1,  // depth of root directory
                    1   // name length of root directory
                    );

    return Return;
    }


PDIRENTRY
RecursiveDescent(
    PDIRENTRY pParentDir,
    LPSTR     DescentBufferPointer,
    ULONG     Depth,
    ULONG     NameLengthOfParents
    )
    {
    PDIRENTRY pFirstNode, pNewNode;
    PCHAR     pFindName;
    ULONG     NameLength;
    ULONG     AllocLength;
    BOOL      IsDirectory;
    HANDLE    hFind;
    DWORDLONG dwlFileSize;

    IncrementTotalNumberOfDirectories();

    pFirstNode = NULL;

    *( DescentBufferPointer + 0 ) = '*';    // append "*" to DescentNameBuffer
    *( DescentBufferPointer + 1 ) = 0;      // terminate string

    hFind = FindFirstFileA( DescentNameBuffer.A, &FindData.A );

    if ( hFind == INVALID_HANDLE_VALUE ) {

        if ( GetLastError() == ERROR_PATH_NOT_FOUND ) {

            WarnFailedSourceOpen(
                0,
                "Failure enumerating files in directory \"%s\"\r\n"
                "The specific error code (path not found) could indicate that the directory\r\n"
                "was deleted by another process during the directory scan, or it could indicate\r\n"
                "that the directory name contains some Unicode characters that do not have a\r\n"
                "corresponding %s character mapping (try %susing -c, or use -j1 or -j2 for\r\n"
                "full Unicode names)\r\n",
                GenerateFullNameSource( pParentDir, ReportNameBuffer.A ),
                bUseAnsiFileNames ? "ANSI" : "OEM",
                bUseAnsiFileNames ? "not " : ""
                );
            }

        else if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {

            WarnFailedSourceOpen(
                GETLASTERROR,
                "Failure enumerating files in directory \"%s\"\r\n",
                GenerateFullNameSource( pParentDir, ReportNameBuffer.A )
                );
            }

        return NULL;
        }

    *( DescentBufferPointer + 0 ) = 0;      // restore DescentNameBuffer

    do {

        if ((( ! ( FindData.A.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )) ||
             ( bHiddenFiles )) &&
            ( ! ( IsFileNameDotOrDotDot( FindData.A.cFileName )))) {

            IsDirectory = ( FindData.A.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? TRUE : FALSE;

            pNewNode = NewDirNode( IsDirectory );

            if ( FindData.A.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) {
                pNewNode->dwFlags |= IS_HIDDEN;
                }

            pNewNode->pParentDir = pParentDir;

            if ( pNewNode->pFileTimeInfo ) {
                 pNewNode->pFileTimeInfo->ftLastWriteTime  = FindData.A.ftLastWriteTime;
                 pNewNode->pFileTimeInfo->ftLastAccessTime = FindData.A.ftLastAccessTime;
                 pNewNode->pFileTimeInfo->ftCreationTime   = FindData.A.ftCreationTime;
                 }

            //
            //  In Win32/NT, there is only an AlternateFileName iff
            //  the regular filename is not FAT-compatible, either
            //  because of filename length, or invalid FAT characters.
            //
            //  ISO allows up to 30 characters (31 with the dot), but
            //  we'll also take up to 221 characters (maximum encodable
            //  length) if bAllowLongNames is specified.
            //

            if ( *FindData.A.cAlternateFileName != 0 ) {
                pNewNode->dwFlags |= CHECK_SHORTNAME;
                }

            if (( ! bDontUpcase ) || ( bRestrictToNt3xCompatibleNames )) {
                strupr( FindData.A.cFileName );
                }

            if ( ! bAllowLongNames ) {

                BOOL ShortNameExists = ( *FindData.A.cAlternateFileName != 0 );
                BOOL LongNameIs8Dot3 = IsAcceptableDosName( FindData.A.cFileName );

                if ( ShortNameExists ) {

                    //
                    //  Test that underlying operating system is not generating
                    //  short names for names that don't need short names.
                    //  There is a bug in NT 4.0 CDFS that will generate short
                    //  names unnecessarily.
                    //

                    if ( LongNameIs8Dot3 ) {
                        *FindData.A.cAlternateFileName = 0;
                        }
                    }

                else {

                    //
                    //  Test that the underlying operating system is generating
                    //  short names for unacceptable DOS filenames.  There is a
                    //  registry setting in NT that will cause NTFS to not
                    //  generate short names and we need to blow up if that is
                    //  happening.
                    //

                    if ( ! LongNameIs8Dot3 ) {

                        WarnFailedSourceOpen(
                            0,
                            "Long %sname with no 8.3 shortname provided by file system:\r\n"
                            "\"%s%s\"\r\n",
                            IsDirectory ? "directory " : "file",
                            GenerateFullNameSource( pParentDir, ReportNameBuffer.A ),
                            FindData.A.cFileName
                            );

                        continue;
                        }
                    }
                }


            pFindName  = (PCHAR)&FindData.A.cFileName;
            NameLength = EncodingNameLength( pFindName, IsDirectory );

            if (( *FindData.A.cAlternateFileName != 0 ) &&
                (( NameLength > 221 ) ||
                 (( ! bAllowLongNames ) &&
                  ( ! IsValidDosName( pFindName ))) ||
                 (( bRestrictToNt3xCompatibleNames ) &&
                  ((( IsDirectory ) &&
                    ( ! IsValidNt3xCompatibleDirectoryName( pFindName ))) ||
                   (( ! IsDirectory ) &&
                    ( ! IsValidNt3xCompatibleFileName( pFindName ))))))) {

                //
                //  Use alternate filename
                //

                if (( ! bDontUpcase ) || ( bRestrictToNt3xCompatibleNames )) {
                    strupr( FindData.A.cAlternateFileName );
                    }

                pFindName  = (PCHAR)&FindData.A.cAlternateFileName;
                NameLength = EncodingNameLength( pFindName, IsDirectory );

                printf(
                    "\r\n%60s\r\nWARNING: Using alternate %sname \"%s\" for \"%s\\%s\"\r\n",
                    "",
                    IsDirectory ? "directory " : "file",
                    FindData.A.cAlternateFileName,
                    GenerateFullName( pParentDir, ReportNameBuffer.A ),
                    FindData.A.cFileName
                    );
                fflush( stdout );

                }

            if ( NameLength > 221 ) {

                WarnFailedSourceOpen(
                    0,
                    "%sname \"%s\\%s\" is longer than 221 characters\r\n",
                    IsDirectory ? "Directory " : "File",
                    GenerateFullName( pParentDir, ReportNameBuffer.A ),
                    pFindName
                    );

                continue;
                }

            if ( strchr( pFindName, ';' )) {

                WarnFailedSourceOpen(
                    0,
                    "%sname \"%s\\%s\" contains a semicolon which has special meaning on ISO-9660 CD-ROMs\r\n",
                    IsDirectory ? "Directory " : "File",
                    GenerateFullName( pParentDir, ReportNameBuffer.A ),
                    pFindName
                    );

                continue;
                }

            if ((   bAllowLongNames                 ) &&
                ( ! bRestrictToNt3xCompatibleNames  ) &&
                ( ! bViolatesNt3xCompatibility      )) {

                if (((   IsDirectory ) && ( ! IsValidNt3xCompatibleDirectoryName( pFindName ))) ||
                    (( ! IsDirectory ) && ( ! IsValidNt3xCompatibleFileName( pFindName )))) {

                    bViolatesNt3xCompatibility = TRUE;

                    }
                }

            if ( IsDirectory || bOptimizeFileNames )
                AllocLength = NameLength + 1;   // terminator
            else
                AllocLength = NameLength - 1;   // terminator minus ";1"

            ASSERT( NameLength != 0 );
            ASSERT( NameLength < 65535 );

            pNewNode->wFileNameLength = (WORD)NameLength;
            pNewNode->pszFileName  = MyAllocNeverFree( AllocLength );

            strcpy( pNewNode->pszFileName, pFindName );

            pFirstNode = AlphabeticalInsertNode( pFirstNode, pNewNode );

            if (( bReportExceedLevels ) &&
                (( NameLengthOfParents + NameLength + Depth ) > 255 )) {

                printf( "\r\n%60s\r\nWARNING: Non-ISO directory depth exceeds 255 characters: \"%s\"\r\n",
                        "",
                        GenerateFullName( pNewNode, ReportNameBuffer.A ));
                fflush( stdout );
                }

            if ( IsDirectory ) {

                if (( bReportExceedLevels ) && ( Depth >= 8 )) {

                    printf( "\r\n%60s\r\nWARNING: Non-ISO directory depth exceeds 8 levels: \"%s\"\r\n",
                            "",
                            GenerateFullName( pNewNode, ReportNameBuffer.A ));
                    fflush( stdout );
                    }

                if (( bReportInvalidDosNames ) && ( ! IsValidDosName( pNewNode->pszFileName ))) {

                    printf( "\r\n%60s\r\nWARNING: Non-DOS directory name: \"%s\"\r\n",
                            "",
                            GenerateFullName( pNewNode, ReportNameBuffer.A ));
                    fflush( stdout );
                    }

                if (( bReportInvalidIsoNames ) && ( ! IsValidIsoDirectoryName( pNewNode->pszFileName ))) {

                    printf( "\r\n%60s\r\nWARNING: Non-ISO directory name: \"%s\"\r\n",
                            "",
                            GenerateFullName( pNewNode, ReportNameBuffer.A ));
                    fflush( stdout );
                    }

                if (( bHackI386Directory ) && ( ! pHackI386Directory )) {
                    if (( pParentDir == pRootDir ) && ( stricmp( pNewNode->pszFileName, "i386" ) == 0 )) {
                        pHackI386Directory = pNewNode;
                        }
                    }
                }

            else {      //  this is a file, not a directory

                if (( bReportInvalidDosNames ) && ( ! IsValidDosName( pNewNode->pszFileName ))) {

                    printf( "\r\n%60s\r\nWARNING: Non-DOS filename: \"%s\"\r\n",
                            "",
                            GenerateFullName( pNewNode, ReportNameBuffer.A ));
                    fflush( stdout );
                    }

                if (( bReportInvalidIsoNames ) && ( ! IsValidIsoFileName( pNewNode->pszFileName ))) {

                    printf( "\r\n%60s\r\nWARNING: Non-ISO filename: \"%s\"\r\n",
                            "",
                            GenerateFullName( pNewNode, ReportNameBuffer.A ));
                    fflush( stdout );
                    }

                dwlFileSize = ((DWORDLONG)( FindData.A.nFileSizeHigh ) << 32 )
                                          | FindData.A.nFileSizeLow;

                if ( dwlFileSize > 0xFFFFFFFF ) {
                    ErrorExit(
                        0,
                        "ERROR: File larger than 4GB: \"%s\"\r\n",
                        GenerateFullName( pNewNode, ReportNameBuffer.A )
                        );
                    }

                if (( bReportZeroLength ) && ( dwlFileSize == 0 )) {
                    printf( "\r\n%60s\r\nWARNING: File \"%s\" is 0-length.\r\n",
                            "",
                            GenerateFullName( pNewNode, ReportNameBuffer.A ));
                    fflush( stdout );
                    }

                pNewNode->dwlFileSize = dwlFileSize;

                dwTotalFileBlocks += CD_BLOCKS_FROM_SIZE( dwlFileSize );
                dwlTotalFileSizes += dwlFileSize;

                IncrementTotalNumberOfFiles();

                if ( ! bOptimizeFileNames ) {

                    //
                    //  If no dot already in filename, append trailing dot.
                    //  Note the space has already been allocated for this
                    //  via EncodingNameLength() calculation.
                    //

                    if ( ! strchr( pNewNode->pszFileName, '.' )) {
                        strcat( pNewNode->pszFileName, "." );
                        }
                    }
                }
            }
        }

    while ( FindNextFile( hFind, &FindData.A ));

    if ( GetLastError() != ERROR_NO_MORE_FILES ) {
        WarnFailedSourceOpen(
            GETLASTERROR,
            "Failure enumerating files in directory \"%s\"\r\n",
            GenerateFullNameSource( pParentDir, ReportNameBuffer.A )
            );
        }

    FindClose( hFind );

    for ( pNewNode  = pFirstNode;
          pNewNode != NULL;
          pNewNode  = pNewNode->pNextTargetFile ) {

        if ( pNewNode->dwFlags & IS_DIRECTORY ) {

            ASSERT( pNewNode->pDirectoryInfo != NULL );

            NameLength = strlen( pNewNode->pszFileName );

            memcpy( DescentBufferPointer, pNewNode->pszFileName, NameLength );

            *( DescentBufferPointer + NameLength     ) = '\\';  // append backslash
            *( DescentBufferPointer + NameLength + 1 ) = 0;     // terminate

            pNewNode->pDirectoryInfo->pFirstTargetFile =
                RecursiveDescent(
                    pNewNode,
                    DescentBufferPointer + NameLength + 1,
                    Depth + 1,
                    NameLengthOfParents + NameLength
                    );

            }
        }

    return pFirstNode;

    }


BOOL IsValidJolietFileName( LPWSTR FileName ) {

    //
    //  Only looking for invalid characters, not invalid length.
    //

    WCHAR c;

    while (( c = *FileName++ ) != 0 ) {

        if (( c  < 0x0020 ) ||      // control characters
            ( c == 0x002A ) ||      // '*' (asterisk)
            ( c == 0x002F ) ||      // '/' (slash)
            ( c == 0x003A ) ||      // ':' (colon)
            ( c == 0x003B ) ||      // ';' (semicolon)
            ( c == 0x003F ) ||      // '?' (question)
            ( c == 0x005C )) {      // '\' (backslash)

            return FALSE;

            }
        }

    return TRUE;
    }


PDIRENTRY
StartRecursiveDescentJoliet(
    VOID
    )
    {
    ULONG NameLength = lstrlenW( UnicodeRootPath );

    memcpy( DescentNameBuffer.W, UnicodeRootPath, ( NameLength + 1 ) * 2 );

    return RecursiveDescentJoliet(
                pRootDir,
                DescentNameBuffer.W + NameLength,
                1,  // depth of root directory
                1   // name length of root directory (in bytes)
                );
    }


PDIRENTRY
RecursiveDescentJoliet(
    PDIRENTRY pParentDir,
    LPWSTR    DescentBufferPointer,
    ULONG     Depth,
    ULONG     NameLengthOfParents
    )
    {
    PDIRENTRY pFirstNode, pNewNode;
    ULONG     NameLength;
    BOOL      IsDirectory;
    HANDLE    hFind;
    DWORDLONG dwlFileSize;

    IncrementTotalNumberOfDirectories();

    pFirstNode = NULL;

    *( DescentBufferPointer + 0 ) = L'*';   // append "*" to DescentNameBuffer
    *( DescentBufferPointer + 1 ) = 0;      // terminate string

    hFind = FindFirstFileW( DescentNameBuffer.W, &FindData.W );

    if ( hFind == INVALID_HANDLE_VALUE ) {
        if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {
            WarnFailedSourceOpen(
                GETLASTERROR,
                "Failure enumerating files in directory \"%S\"\r\n",
                GenerateFullNameSourceW( pParentDir, ReportNameBuffer.W )
                );
            }
        return NULL;
        }

    *( DescentBufferPointer + 0 ) = 0;      // restore DescentNameBuffer

    do {

        if ((( ! ( FindData.W.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )) ||
             ( bHiddenFiles )) &&
            ( ! ( IsFileNameDotOrDotDotW( FindData.W.cFileName )))) {

            IsDirectory = ( FindData.W.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? TRUE : FALSE;

            pNewNode = NewDirNode( IsDirectory );

            if ( FindData.W.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) {
                pNewNode->dwFlags |= IS_HIDDEN;
                }

            if ( *FindData.W.cAlternateFileName != 0 ) {
                pNewNode->dwFlags |= CHECK_SHORTNAME;
                }

            pNewNode->pParentDir = pParentDir;

            if ( pNewNode->pFileTimeInfo ) {
                 pNewNode->pFileTimeInfo->ftLastWriteTime  = FindData.W.ftLastWriteTime;
                 pNewNode->pFileTimeInfo->ftLastAccessTime = FindData.W.ftLastAccessTime;
                 pNewNode->pFileTimeInfo->ftCreationTime   = FindData.W.ftCreationTime;
                 }

            NameLength = lstrlenW( FindData.W.cFileName );

            if ( ! bEncodeOnlyUdf ) {

                if ( NameLength > 110 ) {

                    WarnFailedSourceOpen(
                        0,
                        "Unicode %sname \"%S\\%S\" is longer than 110 characters\r\n",
                        IsDirectory ? "directory " : "file",
                        GenerateFullNameW( pParentDir, ReportNameBuffer.W ),
                        FindData.W.cFileName
                        );

                    continue;
                    }

                if ( wcschr( FindData.W.cFileName, L';' )) {

                    WarnFailedSourceOpen(
                        0,
                        "Unicode %sname \"%S\\%S\" contains a semicolon which has special meaning on ISO-9660 CD-ROMS\r\n",
                        IsDirectory ? "directory " : "file",
                        GenerateFullNameW( pParentDir, ReportNameBuffer.W ),
                        FindData.W.cFileName
                        );

                    continue;
                    }
                }


            ASSERT( NameLength < 65535 );

            pNewNode->wUnicodeNameLength = (WORD)NameLength;
            pNewNode->pszUnicodeName = MyAllocNeverFree(( NameLength + 1 ) * 2 );

            lstrcpyW( pNewNode->pszUnicodeName, FindData.W.cFileName );
			//printf("[%ls]\r\n", pNewNode->pszUnicodeName);

            pFirstNode = JolietInsertNode( pFirstNode, pNewNode );

            if ( ! bEncodeOnlyUdf ) {

                if (( bReportInvalidIsoNames ) && ( NameLength > 64 )) {

                    printf( "\r\n%60s\r\nWARNING: Joliet unicode %sname exceeds 128 bytes: \"%S\"\r\n",
                            "",
                            IsDirectory ? "directory " : "file",
                            GenerateFullNameW( pNewNode, ReportNameBuffer.W ));
                    fflush( stdout );
                    }

                if (( bReportInvalidIsoNames ) &&
                    ( ! IsValidJolietFileName( FindData.W.cFileName ))) {

                    printf( "\r\n%60s\r\nWARNING: Joliet unicode %sname contains invalid characters: \"%S\"\r\n",
                            "",
                            IsDirectory ? "directory " : "file",
                            GenerateFullNameW( pNewNode, ReportNameBuffer.W ));
                    fflush( stdout );
                    }


                if (( bReportExceedLevels ) &&
                    ((( NameLengthOfParents ) + ( NameLength * 2 ) + Depth ) > 240 ))  {

                    printf( "\r\n%60s\r\nWARNING: Joliet directory depth exceeds 240 bytes: \"%S\"\r\n",
                            "",
                            GenerateFullNameW( pNewNode, ReportNameBuffer.W ));
                    fflush( stdout );
                    }
                }

            if ( ! IsDirectory ) {

                dwlFileSize = ((DWORDLONG)( FindData.W.nFileSizeHigh ) << 32 )
                                          | FindData.W.nFileSizeLow;

                if ( dwlFileSize > 0xFFFFFFFF ) {
                    ErrorExit(
                        0,
                        "ERROR: File larger than 4GB: \"%S\"\r\n",
                        GenerateFullNameW( pNewNode, ReportNameBuffer.W )
                        );
                    }

                if (( bReportZeroLength ) && ( dwlFileSize == 0 )) {
                    printf( "\r\n%60s\r\nWARNING: File \"%S\" is 0-length.\r\n",
                            "",
                            GenerateFullNameW( pNewNode, ReportNameBuffer.W ));
                    fflush( stdout );
                    }

                pNewNode->dwlFileSize = dwlFileSize;

                dwTotalFileBlocks += CD_BLOCKS_FROM_SIZE( dwlFileSize );
                dwlTotalFileSizes += dwlFileSize;

                IncrementTotalNumberOfFiles();

                }
            }
        }

    while ( FindNextFileW( hFind, &FindData.W ));

     if ( GetLastError() != ERROR_NO_MORE_FILES ) {
        WarnFailedSourceOpen(
            GETLASTERROR,
            "Failure enumerating files in directory \"%S\"\r\n",
            GenerateFullNameSourceW( pParentDir, ReportNameBuffer.W )
            );
        }

    FindClose( hFind );

    for ( pNewNode  = pFirstNode;
          pNewNode != NULL;
          pNewNode  = pNewNode->pNextTargetFile ) {

        if ( pNewNode->dwFlags & IS_DIRECTORY ) {

            ASSERT( pNewNode->pDirectoryInfo != NULL );

            NameLength = pNewNode->wUnicodeNameLength;

            ASSERT( NameLength == (ULONG)lstrlenW( pNewNode->pszUnicodeName ));

            memcpy( DescentBufferPointer, pNewNode->pszUnicodeName, NameLength * 2 );

            *( DescentBufferPointer + NameLength     ) = L'\\';  // append backslash
            *( DescentBufferPointer + NameLength + 1 ) = 0;      // terminate

            pNewNode->pDirectoryInfo->pFirstTargetFile =
                RecursiveDescentJoliet(
                    pNewNode,
                    DescentBufferPointer + NameLength + 1,
                    Depth + 1,
                    NameLengthOfParents + NameLength * 2
                    );

            }
        }

    return pFirstNode;

    }


VOID ReadElToritoBootSectorFile( PCHAR ElToritoBootSectorFileName ) {

    HANDLE Handle;
    DWORD  Actual;
    BOOL   Success;
    DWORD  HighSize = 0;

    Handle = CreateFile(
                 ElToritoBootSectorFileName,
                 GENERIC_READ,
                 FILE_SHARE_READ,
                 NULL,
                 OPEN_EXISTING,
                 FILE_FLAG_SEQUENTIAL_SCAN,
                 NULL
                 );

    if ( Handle == INVALID_HANDLE_VALUE ) {

        ErrorExit(
            GETLASTERROR,
            "ERROR: Could not open boot sector file \"%s\"\r\n",
            ElToritoBootSectorFileName
            );
        }

    ElToritoBootSectorFileSize = GetFileSize( Handle, &HighSize );

    if ( ElToritoBootSectorFileSize == 0xFFFFFFFF ) {

        ErrorExit(
            GETLASTERROR,
            "ERROR: Could not determine boot sector file size \"%s\"\r\n",
            ElToritoBootSectorFileName
            );
        }

    if ( ElToritoBootSectorFileSize == 0 ) {

        ErrorExit(
            0,
            "ERROR: Boot sector file \"%s\" size is zero\r\n",
            ElToritoBootSectorFileName
            );
        }

    if (( ElToritoBootSectorFileSize > ( 0xFFFF * 0x200 )) || ( HighSize != 0 )) {

        ErrorExit(
            0,
            "ERROR: Boot sector file \"%s\" size is too large\r\n",
            ElToritoBootSectorFileName
            );
        }

    ElToritoBootSectorFileData = MyAllocNeverFree( ElToritoBootSectorFileSize );

    Success = ReadFile(
                  Handle,
                  ElToritoBootSectorFileData,
                  ElToritoBootSectorFileSize,
                  &Actual,
                  NULL
                  );

    if ( ! Success ) {

        ErrorExit(
            GETLASTERROR,
            "ERROR: Failure reading boot sector file \"%s\"\r\n",
            ElToritoBootSectorFileName
            );

        }

    if ( Actual != ElToritoBootSectorFileSize ) {

        ErrorExit(
            0,
            "ERROR: Failure reading boot sector file \"%s\"\r\n"
            "Actual bytes (%d) not equal to requested (%d)\r\n",
            ElToritoBootSectorFileName,
            Actual,
            ElToritoBootSectorFileSize
            );

        }

    CloseHandle( Handle );

    }


VOID
ComputeAutoCrcBlockValues(
    IN OUT PUCHAR Buffer,
    IN     DWORD  ExclusiveCrc
    )
    {
    DWORD InclusiveCrc;

    CopyMemory( Buffer + ( CD_SECTORSIZE - 24 ), "ExclCRC", 8 );
    CopyMemory( Buffer + ( CD_SECTORSIZE - 12 ), "AutoCRC", 8 );

    *(UNALIGNED DWORD*)( Buffer + ( CD_SECTORSIZE - 16 )) = ExclusiveCrc;

    InclusiveCrc = Crc32( ExclusiveCrc, Buffer, ( CD_SECTORSIZE - 4 ));

    *(UNALIGNED DWORD*)( Buffer + ( CD_SECTORSIZE - 4 )) = InclusiveCrc;
    }


VOID WriteAutoCrcHeaderBlock( VOID ) {

    PUCHAR    Buffer;
    DWORDLONG dwlOffset;

    ASSERT( bEncodeAutoCrc );
    ASSERT( AutoCrcHeaderBlock != 0 );

    dwlOffset = (DWORDLONG)AutoCrcHeaderBlock * dwBlockSize;

    ASSERT( IS_SECTOR_ALIGNED( dwlOffset ));

    ASSERT( dwlOffset == dwlNextHeaderOffset );
    DEBUGCODE( dwlNextHeaderOffset = 0xFFFFFFFF ); // should be final header write

    Buffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

    ComputeAutoCrcBlockValues( Buffer, HeaderCrc );

    if ( bSignImage )
        UpdateMD5_Multiple64ByteChunks( &SignatureHash, Buffer, CD_SECTORSIZE );

    GenericWrite( dwlOffset, Buffer, CD_SECTORSIZE );

    ReportHeaderProgress( dwlOffset + CD_SECTORSIZE );

    }


VOID WriteAutoCrcFinalBlock( VOID ) {

    DWORDLONG dwlOffset;
    PUCHAR    Buffer;

    ASSERT(( bEncodeAutoCrc ) || ( bSignImage ));

    AutoCrcFinalBlock = dwTotalImageBlocks - BLOCK_NUMBER_FROM_SECTOR_NUMBER( 1 );

    dwlOffset = (DWORDLONG)AutoCrcFinalBlock * dwBlockSize;

    ASSERT( IS_SECTOR_ALIGNED( dwlOffset ));

    ASSERT(( ! bSignImage ) || ( AutoCrcHeaderBlock != 0 ));

    Buffer = AllocateBuffer( CD_SECTORSIZE, TRUE );

#ifndef DVDIMAGE_ONLY

    if ( bSignImage ) {

        UCHAR     SignatureBuffer[ SIGN_SIZE ];
        DWORD     SignatureIdentifier;
        DWORDLONG BytesHashed;
        BOOL      Success;

        //
        //  Mark this block as a "SIGNATURE" block with version number of
        //  signature format used (2.0 now).  Verification programs can
        //  use this version number to determine if they are capable of
        //  verifying this image.  If the high version number remains the
        //  same, older verification programs capable of verifying the same
        //  high version number should continue to work.  If the format changes
        //  such that older verification programs that know how to verify the
        //  existing high version number won't work on the new format, the high
        //  version number should be rev'd.
        //
        //  Version 0.0 (all offsets and sizes in decimal):
        //
        //  Offset  0, (2048 - 12) bytes, all zeros (including version number field described in v1.1 below)
        //  Offset (2048 - 12),  8 bytes, "AutoCRC" with terminating NULL
        //  Offset (2048 -  4),  4 bytes, CRC-32 value INCLUDING data in this sector up to offset (2048 - 4)
        //
        //  Version 1.1 (all offsets and sizes in decimal):
        //
        //  Offset   0,         10 bytes, "SIGNATURE" with terminating NULL
        //  Offset  10,          2 bytes, don't care (usually zeros)
        //  Offset  12,          4 bytes, version number of signature sector format (ver 1.2 is encoded little endian 0x00010002)
        //  Offset  16,         64 bytes, MD5 signature (not including hash of any data in this sector)
        //  Offset  80,        420 bytes, reserved for signature extensions
        //  Offset 500,          4 bytes, block number of next signature block (linked list rooted in final sector, terminated with zero)
        //  Offset 504,          8 bytes, reserved for signature extensions
        //  Offset 512, 2048 - 524 bytes, don't care (usually zeros, could be used for serial number)
        //  Offset 2048 - 12,    8 bytes, "AutoCRC" with terminating NULL
        //  Offset 2048 -  4,    4 bytes, CRC-32 value INCLUDING data in this sector up to offset (2048 - 4)
        //
        //  Version 2.0 (all offsets and sizes in decimal):
        //
        //  Offset   0,                 10 bytes, "SIGNATURE" with terminating NULL
        //  Offset  10,                  2 bytes, don't care (usually zeros)
        //  Offset  12,                  4 bytes, version number of signature sector format (ver 2.1 is encoded little endian 0x00020001)
        //  Offset  16,                  4 bytes, sector number where hash begins (if present, hashing wraps to sector zero when reaches final sector and continues up to this sector minus 1)
        //  Offset  20,                 12 bytes, reserved
        //
        //  Offset  32, first variable length [signature] record:
        //
        //      Offset 0,  4 bytes, length of this record (multiple of 16 bytes)
        //      Offset 4,  4 bytes, identifier of this record type identifier
        //                              0x00000000 - 0x7FFFFFFF is user-defined
        //                              0x80000000 - 0xFFFFFFFF is reserved
        //                              0x80000001              is what we're using
        //      Offset 8,  4 bytes, identifier of this record
        //      Offset 12, 4 bytes, length of signature data
        //      Offset 16, N bytes, signature data
        //
        //  Algorithm:
        //
        //      Offset = 32;
        //      while (( Offset < 2020 ) && ( dword@( Offset ) != 0 )) {
        //          // signature data @( Offset + 16 ), length dword@( Offset + 12 )
        //          Offset += dword@( Offset )
        //          }
        //
        //  ...etc... up to Offset CD_SECTORSIZE-28 (2020)
        //
        //  Offset 2048 - 24,    8 bytes, "ExclCRC" with terminating NULL
        //  Offset 2048 - 16,    4 bytes, CRC-32 value up to but EXCLUDING data in this sector
        //                                  (this value has NOT been finalized with XOR 0xFFFFFFFF).
        //  Offset 2048 - 12,    8 bytes, "AutoCRC" with terminating NULL
        //  Offset 2048 -  4,    4 bytes, CRC-32 value up to and INCLUDING data in this sector
        //                                  up to offset 2044 (this value has NOT been finalized
        //                                  with XOR 0xFFFFFFFF).
        //
        //  NOTE: CRC-32 value at end of sector which INCLUDES all data in sector
        //        will need to be re-computed whenever data in the signature sector
        //        is modified.  To re-compute the CRC, use the stored CRC value at
        //        offset 2032 as the initial CRC value for the sector.
        //
        //  NOTE: To maintain compatibility with existing version of CDIMAGE and
        //        CRC.EXE, the Header CRC (starting at sector zero) should be
        //        initialized to 0xFFFFFFFF, but the FileData CRC (starting at
        //        sector just beyond Header AutoCrc sector) should be initialized
        //        to 0x00000000.

        //
        //  Finalize signature
        //

        ASSERT( sizeof( SignatureVersionText ) == 64 );

#ifdef DEBUG2

        printf( "\r\nHash prefinal: " );
        {
        unsigned j;
        for ( j = 0; j < 16; j++ )
            printf( "%02x ", SignatureHash.Byte[ j ] );
        }
        printf( "\r\n" );

#endif // DEBUG2

        UpdateMD5_64ByteChunk( &SignatureHash, SignatureVersionText );

        BytesHashed = dwlOffset + 64;

        FinalizeMD5( &SignatureHash, BytesHashed );

#ifdef DEBUG2

        printf( "Hashed bytes:  0x%X\r\n", (DWORD)BytesHashed );
        printf( "Hashed text:   %s\r\n", SignatureVersionText );
        printf( "Hash value:    " );

        {
        unsigned j;
        for ( j = 0; j < 16; j++ )
            printf( "%02x ", SignatureHash.Byte[ j ] );
        }
        printf( "\r\n\r\n" );

#endif // DEBUG2

        ZeroMemory( &SignatureBuffer, sizeof( SignatureBuffer ));
        SignatureIdentifier = 0;

        ASSERT( ImageSignatureDescription != NULL );

        Success = GenerateImageSignature(
                      &SignatureHash,
                      ImageSignatureDescription,
                      &SignatureIdentifier,
                      SignatureBuffer
                      );

        if ( Success ) {

            ASSERT(( ! bEncodeAutoCrc ) || ( dwStartingFileBlock == ( AutoCrcHeaderBlock + BLOCK_NUMBER_FROM_SECTOR_NUMBER( 1 ))));

            CopyMemory( Buffer, "SIGNATURE", 10 );
            *(DWORD*)( Buffer + 12 ) = 0x00020000;
            *(DWORD*)( Buffer + 16 ) = dwStartingFileBlock;
            *(DWORD*)( Buffer + 32 ) = ROUNDUP2(( 16 + SIGN_SIZE ), 16 );
            *(DWORD*)( Buffer + 36 ) = 0x80000001;
            *(DWORD*)( Buffer + 40 ) = SignatureIdentifier;
            *(DWORD*)( Buffer + 44 ) = SIGN_SIZE;
            CopyMemory( Buffer + 48, SignatureBuffer, SIGN_SIZE );

#ifdef DEBUG2
            {
            ULONG i;

            printf( "\r\nSignature:\r\n" );
            for ( i = 0; i < SIGN_SIZE; i++ ) {
                if (( i % 16 ) == 0 ) {
                    printf( "\r\n" );
                    }
                printf( "%02x ", SignatureBuffer[ i ] );
                }
            printf( "\r\n\r\n" );
            }
#endif // DEBUG2

            }

        else {

            bSignImage = FALSE;

            }
        }

#endif // ! DVDIMAGE_ONLY


    if ( bEncodeAutoCrc )
        ComputeAutoCrcBlockValues( Buffer, FileDataCrc );

    GenericWrite( dwlOffset, Buffer, CD_SECTORSIZE );

    }


VOID
HeaderWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    )
    {
    ASSERT( ISALIGN2_64( Offset, dwBlockSize ));
    ASSERT( ISALIGN2( Length, dwBlockSize ));
    ASSERT( ISALIGN2( Buffer, dwBlockSize ));
    ASSERT(( Offset == dwlNextHeaderOffset ) || ( ! bEncodeAutoCrc ));

    if ( bEncodeAutoCrc )
        HeaderCrc = Crc32( HeaderCrc, Buffer, Length );

    if ( bSignImage )
        UpdateMD5_Multiple64ByteChunks( &SignatureHash, Buffer, Length );

    GenericWrite( Offset, Buffer, Length );

    ReportHeaderProgress( Offset + Length );

    DEBUGCODE( dwlNextHeaderOffset = Offset + Length );

    }


VOID
FileDataWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    )
    {
    ASSERT( ISALIGN2_64( Offset, dwBlockSize ));
    ASSERT( ISALIGN2( Length, dwBlockSize ));
    ASSERT( ISALIGN2( Buffer, dwBlockSize ));
    ASSERT(( Offset == dwlNextFileDataOffset ) || ( ! bEncodeAutoCrc ));

    if ( bEncodeAutoCrc )
        FileDataCrc = Crc32( FileDataCrc, Buffer, Length );

    if ( bSignImage )
        UpdateMD5_Multiple64ByteChunks( &SignatureHash, Buffer, Length );

    GenericWrite( Offset, Buffer, Length );

    ReportTransferProgress( Offset + Length );

    DEBUGCODE( dwlNextFileDataOffset = Offset + Length );

    }

DWORD
PaddedFileDataWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    )
{
    DWORD dwLength;

    dwLength=ROUNDUP2(Length,CD_SECTORSIZE);
    if(dwLength != Length)
        ZeroMemory((BYTE*)Buffer+Length,dwLength-Length);
    FileDataWrite(Offset,Buffer,dwLength);

    return dwLength;
}



typedef struct _SPLIT_WRITE SPLIT_WRITE, *PSPLIT_WRITE;

struct _SPLIT_WRITE {
    LONG  ReferenceCount;
    PVOID BufferToFree;
    };


VOID
SplitWriteCompletion(
    PVOID     SplitBuffer,
    DWORDLONG SplitOffset,
    ULONG     SplitLength,
    PVOID     Context
    )
    {
    PSPLIT_WRITE SplitWrite = Context;

    //
    //  ASSUMPTION: Completion routine always called from same
    //              thread, so InterlockedDecrement is not necessary.
    //

    if ( --SplitWrite->ReferenceCount == 0 ) {
        ReleaseBuffer( SplitWrite->BufferToFree );
        MyHeapFree( SplitWrite );
        }

    UNREFERENCED( SplitBuffer );
    UNREFERENCED( SplitOffset );
    UNREFERENCED( SplitLength );
    }


VOID
LargeDataSplitWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    )
    {
    PSPLIT_WRITE SplitWriteContext = MyHeapAlloc( sizeof( SPLIT_WRITE ));
    ULONG SplitCount = ROUNDUP2( Length, FX_BLOCKSIZE ) / FX_BLOCKSIZE;
    ULONG SplitLength;

    SplitWriteContext->ReferenceCount = SplitCount;
    SplitWriteContext->BufferToFree   = Buffer;

    while ( SplitCount-- ) {

        SplitLength = ( Length < FX_BLOCKSIZE ) ? Length : FX_BLOCKSIZE;

        IssueWrite(
            hTargetContext,
            Buffer,
            Offset,
            SplitLength,
            SplitWriteCompletion,
            SplitWriteContext
            );

        Buffer = (PCHAR)Buffer + SplitLength;
        Offset += SplitLength;
        Length -= SplitLength;

        }

    ASSERT( Length == 0 );

    }


VOID
GenericWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    )
    {
    if ( Length > FX_BLOCKSIZE ) {
        LargeDataSplitWrite( Offset, Buffer, Length );
        }
    else {
        IssueWrite( hTargetContext, Buffer, Offset, Length, NULL, NULL );
        }
    }


VOID
GenericRead(
    IN DWORDLONG dwlOffset,
    IN DWORD dwLength,
    OUT PVOID pBuffer
    )
    {
    HANDLE hFile;
    OVERLAPPED Overlapped;
    BOOL bResult;
    DWORD dwBytesRead;

//    printf("Reading %u at %I64u from the image\r\n",dwLength,dwlOffset);
    FlushWriter(hTargetContext);

    hFile=GetFileHandleFromWriteContext(hTargetContext);

    dwBytesRead             = 0;
    Overlapped.Internal     = 0;
    Overlapped.InternalHigh = 0;
    Overlapped.Offset       = (DWORD)( dwlOffset );
    Overlapped.OffsetHigh   = (DWORD)( dwlOffset >> 32 );
    Overlapped.hEvent       = AllocateManualEvent();

    bResult=ReadFile(hFile,pBuffer,dwLength,&dwBytesRead,&Overlapped);

    if ( ! bResult ) {

        if( GetLastError() == ERROR_IO_PENDING ) {

            bResult=GetOverlappedResult(hFile,&Overlapped,&dwBytesRead,TRUE);

            if ( ! bResult ) {
                ErrorExit( GETLASTERROR, "GenericRead GetOverlappedResult failed\r\n" );
                }
            }

        else {
            ErrorExit( GETLASTERROR, "GenericRead ReadFile failed\r\n" );
            }
        }

    if ( dwBytesRead != dwLength ) {
        ErrorExit( 0, "GenericRead incorrect length (requested %d, got %d)\r\n", dwLength, dwBytesRead );
        }

    RecycleManualEvent( Overlapped.hEvent );
    }


BOOL SetImageLength(LONGLONG llOffset)
{
    HANDLE hFile;
    BOOL bResult;

    hFile=GetFileHandleFromWriteContext(hTargetContext);

    bResult = SetFilePointer(hFile,(LONG) llOffset, NULL, FILE_BEGIN);

    if (bResult )
    {
        bResult=SetEndOfFile(hFile);

        if ( ! bResult )
        {
            ErrorExit( GETLASTERROR, "SetImageLength SetEndOfFile failed\r\n" );
        }
    }
    else
    {
        ErrorExit( GETLASTERROR, "SetImageLength SetFilePointer failed\r\n" );
    }

    return bResult;
}

DWORD
AllocateMetaBlocks(
    IN DWORD SizeInBytes
    )
    {
    DWORD SizeInBlocks = CD_BLOCKS_FROM_SIZE( SizeInBytes );
    DWORD BlockNumber  = dwNextMetaBlockNumber;

    ASSERT( SizeInBytes != 0 );
    ASSERT( BlockNumber != 0 );

    if (( bEncodeUdf ) && ( dwMetaSlackStartingBlock == 0 )) {

        if ((( BlockNumber                ) <= ANCHOR_SECTOR ) &&
            (( BlockNumber + SizeInBlocks ) >  ANCHOR_SECTOR )) {

            //
            //  This allocation would span the ANCHOR_SECTOR, so move
            //  the allocation beyond the ANCHOR_SECTOR.  Note the
            //  reserved ANCHOR_SECTOR is only one block in length.
            //

            DEBUGCODE( printf( "\r\nWARNING: ISO-9660 meta data spans UDF anchor sector,\r\n"
                               "         wasting %d blocks to avoid UDF anchor\r\n",
                               ANCHOR_SECTOR - BlockNumber
                             ));

            dwMetaSlackStartingBlock  = BlockNumber;
            dwMetaSlackNumberOfBlocks = ANCHOR_SECTOR - BlockNumber;
            BlockNumber               = ANCHOR_SECTOR + 1;

            }
        }

    dwNextMetaBlockNumber = BlockNumber + SizeInBlocks;

    return BlockNumber;
    }


DWORD
AllocateMetaBlocksSectorAligned(
    IN DWORD SizeInBytes
    )
    {
    DWORD SizeInBlocks = CD_BLOCKS_FROM_SIZE( ROUNDUP2( SizeInBytes, CD_SECTORSIZE ));
    DWORD BlockNumber  = dwNextMetaBlockNumber;

    ASSERT( IS_SECTOR_ALIGNED( (DWORDLONG) BlockNumber * dwBlockSize ));

    ASSERT( SizeInBytes != 0 );
    ASSERT( BlockNumber != 0 );

    if (( bEncodeUdf ) && ( dwMetaSlackStartingBlock == 0 )) {

        if ((( BlockNumber                ) <= ANCHOR_SECTOR ) &&
            (( BlockNumber + SizeInBlocks ) >  ANCHOR_SECTOR )) {

            //
            //  This allocation would span the ANCHOR_SECTOR, so move
            //  the allocation beyond the ANCHOR_SECTOR.  Note the
            //  reserved ANCHOR_SECTOR is only one block in length.
            //

            DEBUGCODE( printf( "\r\nWARNING: ISO-9660 meta data spans UDF anchor sector,\r\n"
                               "         wasting %d blocks to avoid UDF anchor\r\n",
                               ANCHOR_SECTOR - BlockNumber
                             ));

            dwMetaSlackStartingBlock  = BlockNumber;
            dwMetaSlackNumberOfBlocks = ANCHOR_SECTOR - BlockNumber;
            BlockNumber               = ANCHOR_SECTOR + 1;

            }
        }

    dwNextMetaBlockNumber = BlockNumber + SizeInBlocks;

    return BlockNumber;
    }


VOID
WriteZeroedSectors(
    IN DWORD SectorNumber,
    IN DWORD NumberOfSectors
    )
    {
    DWORD SizeInBytes = NumberOfSectors * CD_SECTORSIZE;
    PVOID Buffer      = AllocateBuffer( SizeInBytes, TRUE );

    HeaderWrite((DWORDLONG)SectorNumber * CD_SECTORSIZE, Buffer, SizeInBytes );
    }


VOID
WriteZeroedBlocks(
    IN DWORD BlockNumber,
    IN DWORD NumberOfBlocks
    )
    {
    DWORD SizeInBytes = NumberOfBlocks * dwBlockSize;
    PVOID Buffer      = AllocateBuffer( SizeInBytes, TRUE );

    HeaderWrite((DWORDLONG)BlockNumber * dwBlockSize, Buffer, SizeInBytes );
    }


#ifndef DVDIMAGE_ONLY

VOID
GenerateSignatureDescription(
    VOID
    )
    {
    DWORD ComputerNameLength = MAX_COMPUTERNAME_LENGTH + 1;
    CHAR  ComputerName[ MAX_COMPUTERNAME_LENGTH + 1 ] = "?";
    CHAR  TargetName[ MAX_PATH ] = "?";
    CHAR  SourceName[ MAX_PATH ] = "?";
    LPSTR Terminator;
    DWORD SignatureDescriptionLength;

    GetComputerName( ComputerName, &ComputerNameLength );

    GetFullPathName( pszSourceTree, MAX_PATH, SourceName, &Terminator );

    Terminator = strchr( SourceName, 0 );

    if (( Terminator > SourceName ) && ( *( Terminator - 1 ) != '\\' )) {
        *Terminator++ = '\\';
        *Terminator   = '\0';
        }

    if ( pszTargetFile )
        GetFullPathName( pszTargetFile, MAX_PATH, TargetName, &Terminator );
    else
        strcpy( TargetName, "no image file" );

    SignatureDescriptionLength = strlen( "%s on %s %s to %s [%s]" )
                               + strlen( MAJOR_VERSION_TEXT )
                               + strlen( ComputerName )
                               + strlen( SourceName )
                               + strlen( TargetName )
                               + strlen( uchVolumeLabel )
                               - 10     // "%s%s%s%s%s"
                               + 1;     // terminator

    ImageSignatureDescription = MyAllocNeverFree( SignatureDescriptionLength );

    // "CDIMAGE 2.25 on TOMMCGMIPS d:\cdfstest to g:\test.img [LABEL]"

    sprintf(
        ImageSignatureDescription,
        "%s on %s %s to %s [%s]",
        MAJOR_VERSION_TEXT,
        ComputerName,
        SourceName,
        TargetName,
        uchVolumeLabel
        );

    }


BOOL
InitSignatureStuff(
    VOID
    )
    {
    printf( "\r\nConnecting to signature server... " );
    fflush( stdout );

    GenerateSignatureDescription();

    if ( ConnectToSignatureServer( ImageSignatureDescription )) {

        printf( "\rConnected to signature server    \r\n" );
        fflush( stdout );

        return TRUE;
        }

    else {

        CHAR ErrorTextBuffer[ 256 ];

        printf(
            "\r\n"
            "WARNING: Failed to connect to signature server,\r\n"
            "         \"%s\".\r\n"
            "         (%s)\r\n"
            "\r\n"
            "         IMAGE WILL NOT BE SIGNED.\r\n"
            "\r\n",
            SignatureRpcEndpointBindingString,
            ErrorTextFromErrorCode( GetLastError(), ErrorTextBuffer )
            );

        fflush( stdout );
        return FALSE;
        }
    }


BOOL
GenerateImageSignature(
    IN  PMD5_HASH HashValue,
    IN  LPSTR     SignatureDescription,
    OUT PDWORD    SignatureIdentfier,
    OUT PUCHAR    SignatureBuffer
    )
    {
    BOOL Success;

    printf( "\rGetting signature from signature server... " );
    fflush( stdout );

    Success = GenerateSignatureFromHashValue(
                  HashValue,
                  SignatureDescription,
                  SignatureIdentfier,
                  SignatureBuffer
                  );

    if ( Success ) {

        printf( "\r                                            \r" );
        fflush( stdout );

        return TRUE;
        }

    else {

        printf(
            "\r\n"
            "WARNING: Failed to acquire signature from RPC signature server\r\n"
            "         (%s)\r\n"
            "\r\n"
            "         IMAGE WILL NOT BE SIGNED.\r\n"
            "\r\n",
            ErrorTextFromErrorCode( GetLastError(), ReportNameBuffer.A )
            );

        fflush( stdout );
        return FALSE;
        }
    }

#endif // ! DVDIMAGE_ONLY


#ifdef DONTCOMPILE

void RecursiveLinkSourceFirst( PDIRENTRY pFirst, PDIRENTRY *pChain ) {

    PDIRENTRY pFile;

    for ( pFile = pFirst; pFile; pFile = pFile->pNextTargetFile ) {

        (*pChain)->pNextSourceFile = pFile;
        *pChain = pFile;

        if ( pFile->dwFlags & IS_DIRECTORY ) {
            RecursiveLinkSourceFirst( pFile->pDirectoryInfo->pFirstTargetFile, pChain );
            }
        }
    }


void RecursiveLinkSourceAfter( PDIRENTRY pFirst, PDIRENTRY *pChain ) {

    PDIRENTRY pFile;

    for ( pFile = pFirst; pFile; pFile = pFile->pNextTargetFile ) {

        (*pChain)->pNextSourceFile = pFile;
        *pChain = pFile;

        }

    for ( pFile = pFirst; pFile; pFile = pFile->pNextTargetFile ) {

        if ( pFile->dwFlags & IS_DIRECTORY ) {
            RecursiveLinkSourceFirst( pFile->pDirectoryInfo->pFirstTargetFile, pChain );
            }
        }
    }


void LinkSourceLevel( void ) {

    PDIRENTRY pFile, pDir, pChain;
    UINT uLevel;

    pChain = pRootDir;

    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ ) {
        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir ) {
            for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile ) {

                pChain->pNextSourceFile = pFile;
                pChain = pFile;

                }
            }
        }
    }


void ComputeModifiedOrdering( void ) {

    PDIRENTRY pChain = pRootDir;

    if ( bOptimizeDepthFirst ) {
        RecursiveLinkSourceFirst( pRootDir->pDirectoryInfo->pFirstTargetFile, &pChain );
        }
    else if ( bOptimizeDepthAfter ) {
        RecursiveLinkSourceAfter( pRootDir->pDirectoryInfo->pFirstTargetFile, &pChain );
        }
    else {
        LinkSourceLevel();
        }
    }

#endif // DONTCOMPILE

DWORD
FileDataRead(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    )
{
    DWORD dwBytesRead;
    DWORD dwHigh;
    DWORD dwLow;

    dwLow=(DWORD) Offset & 0x00000000FFFFFFFF;
    dwHigh=(DWORD) (Offset >> 32);
    ASSERT(((DWORDLONG) (dwHigh << 32) | (DWORDLONG) dwLow) == Offset);

    SetFilePointer(hTargetFile,(LONG)dwLow,(PLONG)&dwHigh, FILE_BEGIN);
    if(!ReadFile(hTargetFile,Buffer,Length,&dwBytesRead,NULL))
        return 0;
    else
        return dwBytesRead;
}

VOID EnqueueUdfFiles(PTRANSFER_CONTEXT pTx, PDWORDLONG pdwlTotalSourceBytes)
{
	PDIRENTRY pDir;
	PDIRENTRY pFile;
	UINT uLevel;
	LPWSTR pszUnicodeName;
	PDIRENTRY apSpecialDirs[2];
	BOOL fVideoTsInfoPresent = FALSE;
	DWORDLONG dwlFileSize;
	INT iIndex;

	ZeroMemory(apSpecialDirs, 2 * sizeof(PDIRENTRY));

	if(bUdfVideoZone)
	{
		// enqueue VIDEO_TS files and AUDIO_TS files first

		for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ )
		{
			for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir )
			{
				if(0 == wcscmp(pDir->pszUnicodeName,L"VIDEO_TS"))
				{
					apSpecialDirs[0] = pDir;
				}
				else if(0 == wcscmp(pDir->pszUnicodeName,L"AUDIO_TS"))
				{
					apSpecialDirs[1] = pDir;
				}
			}
		}

		if(apSpecialDirs[0] == NULL)
		{
			ErrorExit(0, "Error: VIDEO_TS directory does not exist\n");
		}

		for(iIndex = 0; iIndex < 2; iIndex++)
		{
			if(apSpecialDirs[iIndex] == NULL)
			{
				continue;
			}

			for ( pFile = apSpecialDirs[iIndex]->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile )
			{
				if ( ! ( pFile->dwFlags & ( IS_DIRECTORY | IS_WRITTEN )))
				{

					dwlFileSize = pFile->dwlFileSize;

					if ( dwlFileSize > 0 )
					{

						pFile->dwFlags |= IS_WRITTEN;

						pszUnicodeName = AllocFullUnicodeName( pFile );

						if((iIndex == 0) &&
						   (0 == wcscmp(pFile->pszUnicodeName,L"VIDEO_TS.IFO")))
						{
							fVideoTsInfoPresent = TRUE;
						}

						if ( QueueOpenFile(
									  pTx->hOpenContext,
									  pszUnicodeName,
									  NULL,
									  CD_SECTORSIZE,
									  0,
									  dwlFileSize,
									  MAX_QUEUED_READS,
									  pFile
									  ) )
						{

							*pdwlTotalSourceBytes += dwlFileSize;

						}
						else
						{
							pFile->dwFlags |= NO_DIRENTRY;  // don't create directory entry
							MyHeapFree( pszUnicodeName );

							dwlTotalFileSizes  -= dwlFileSize;
							dwlTotalImageBytes -= ROUNDUP2_64( dwlFileSize, dwBlockSize );
							dwTotalImageBlocks -= CD_BLOCKS_FROM_SIZE( dwlFileSize );
						}
					}
				}
			}
		}

		if(!fVideoTsInfoPresent)
		{
			ErrorExit(0,"Error: File VIDEO_TS\\VIDEO_TS.IFO not present or is 0 length");
		}
	}


    for ( uLevel = 0; pStartOfLevel[ uLevel ]; uLevel++ )
	{
        for ( pDir = pStartOfLevel[ uLevel ]; pDir; pDir = pDir->pDirectoryInfo->pNextTargetDir )
		{
			if(bUdfVideoZone)
			{
				// Skip VIDEO_TS and AUDIO_TS directories

				if((pDir == apSpecialDirs[0]) ||
				   (pDir == apSpecialDirs[1]))
				{
					continue;
				}

			}

            for ( pFile = pDir->pDirectoryInfo->pFirstTargetFile; pFile; pFile = pFile->pNextTargetFile )
			{
                if ( ! ( pFile->dwFlags & ( IS_DIRECTORY | IS_WRITTEN )))
				{

                    dwlFileSize = pFile->dwlFileSize;

                    if ( dwlFileSize > 0 )
					{

                        pFile->dwFlags |= IS_WRITTEN;

                        pszUnicodeName = AllocFullUnicodeName( pFile );

                        if ( QueueOpenFile(
                                      pTx->hOpenContext,
                                      pszUnicodeName,
                                      NULL,
                                      CD_SECTORSIZE,
                                      0,
                                      dwlFileSize,
                                      MAX_QUEUED_READS,
                                      pFile
                                      ) )
						{

                            *pdwlTotalSourceBytes += dwlFileSize;

                        }
                        else
						{
                            pFile->dwFlags |= NO_DIRENTRY;  // don't create directory entry
                            MyHeapFree( (PVOID) pszUnicodeName );

                            dwlTotalFileSizes  -= dwlFileSize;
                            dwlTotalImageBytes -= ROUNDUP2_64( dwlFileSize, dwBlockSize );
                            dwTotalImageBlocks -= CD_BLOCKS_FROM_SIZE( dwlFileSize );
                        }
                    }
                }
            }
        }
    }
}
