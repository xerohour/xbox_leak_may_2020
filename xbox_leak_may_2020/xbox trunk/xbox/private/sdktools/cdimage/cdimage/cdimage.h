
#define CDIMAGE_NAME    "CDIMAGE"               //
#define CDIMAGE_VERSION STRINGIZE( VERSION )    //  careful, use only a-chars
#define CDIMAGE_DATE    "(12/06/2000 TM)"       //

#ifdef XBOX

    #undef  CDIMAGE_NAME
    #define CDIMAGE_NAME "XDiscImage"

#endif // XBOX


#ifdef DVDIMAGE_ONLY

    #undef  CDIMAGE_NAME
    #define CDIMAGE_NAME "DVDIMAGE"

#endif // DVDIMAGE_ONLY


#define MAJOR_VERSION_TEXT  CDIMAGE_NAME " " CDIMAGE_VERSION
#define MINOR_VERSION_TEXT  CDIMAGE_DATE

#define CDIMAGE_SIGNATURE_TEXT "Microsoft CD-ROM Disc Signature Version 2.0"


#define CD_SECTORSIZE 2048                      // constant for CDs



//
//  Implemention Note:  block numbers are 32-bits in ISO-9660, but offsets
//  are represented as 64-bit since block numbers are multiplied by 2048 (2^11).
//  This means that the max image size is ( 2^32 * 2^11 ) = 2^43 or 8 terabytes
//  (0x80000000000 or 8,796,093,022,208 bytes).
//

#define MAX_LEVELS  256

//
//  74-minute CD:                                        4,440 seconds
//  times 75 frames (sectors) per second:              333,000 frames (sectors)
//  times 2048 bytes per frame (sector)            681,984,000 bytes
//

#define MAX_IMAGE_BYTES         681984000       // 74-minute CD maximum bytes
#define MAX_IMAGE_BYTES_TEXT    "681,984,000"

#ifdef JOEHOL

    #undef  MAX_IMAGE_BYTES
    #undef  MAX_IMAGE_BYTES_TEXT

    #define MAX_IMAGE_BYTES         679477248       // fudge down to 648.0MB
    #define MAX_IMAGE_BYTES_TEXT    "679,477,248"

    #define PRIVATE_BUILD_TEXT  "Private version for Windows NT CD Burn Lab\n\n"

#endif

#define MAX_QUEUED_WRITES     4
#define MAX_QUEUED_READS      4
#define MAX_QUEUED_OPENS      4

#define SCAN_REPORT_INCREMENT 500


#define IS_DIRECTORY    0x80000000  // use sign bit so compiler can optimize
#define IS_DUPLICATE    0x00000001
#define IS_WRITTEN      0x00000002
#define IS_HIDDEN       0x00000080
#define CHECK_SHORTNAME 0x00008000
#define NO_DIRENTRY     0x00000100


typedef struct _FILETIME_INFO   FILETIME_INFO,   *PFILETIME_INFO;
typedef struct _DIRECTORY_INFO  DIRECTORY_INFO,  *PDIRECTORY_INFO;
typedef struct _UDFINFO         UDFINFO,         *PUDFINFO;
typedef struct _DIRENTRY        DIRENTRY,        *PDIRENTRY;

struct _FILETIME_INFO {
    FILETIME ftLastWriteTime;
    FILETIME ftLastAccessTime;
    FILETIME ftCreationTime;
    };

struct _DIRECTORY_INFO {
    PDIRENTRY pFirstTargetFile;
    PDIRENTRY pNextTargetDir;
    PDIRENTRY pAlternateFirstFile;
    PDIRENTRY pAlternateNextDir;
    DWORD     dwUnicodeBlock;
    DWORD     dwUnicodeDirSize;
    DWORD     dwUnicodeIndex;
    DWORD     dwDirIndex;
    };

struct _UDFINFO {
    ULONG ulFIDLength;        // FID extent length in bytes (for Dir only)
    ULONG ulICBBlockNumber;   // Location of ICB for file or dir FID extent follows
    ULONG ulLinkCount;        // File Link Count
    };

typedef struct _DIRENTRY {

    LPSTR               pszFileName;        // offset 0
    LPWSTR              pszUnicodeName;     // offset 4
    DWORDLONG           dwlFileSize;        // offset 8
    DWORD               dwStartingBlock;    // offset 16
    DWORD               dwFlags;            // offset 20 IS_DIRECTORY, IS_DUPLICATE, IS_WRITTEN, IS_HIDDEN
    WORD                wFileNameLength;    // offset 24
    WORD                wUnicodeNameLength; // offset 26

    PDIRENTRY           pParentDir;         // offset 28
    PDIRENTRY           pNextSourceFile;    // offset 32
    PDIRENTRY           pNextTargetFile;    // offset 36
    PDIRENTRY           pAlternateNextFile; // offset 40

    PFILETIME_INFO      pFileTimeInfo;      // offset 44 valid iff ( ! bUseGlobalTime )
    PUDFINFO            pUdfInfo;           // offset 48 valid iff ( bEncodeUDF )

    union {                                 // IS_DIRECTORY, IS_DUPLICATE exclusive
        PDIRECTORY_INFO pDirectoryInfo;     // offset 52 valid iff ( dwFlags & IS_DIRECTORY )
        PDIRENTRY       pPrimary;           // offset 52 valid iff ( dwFlags & IS_DUPLICATE )
        };
                                            // offset 56 (length of retail structure)
#ifdef DEBUG
    ULONG               dwDirOffset;        // offset 56
    ULONG               dwUnicodeDirOffset; // offset 60
#endif

    /*
    BYTE                IcbFile[CD_SECTORSIZE]; // valid iff ( bEncodeUDF)
    */
                                                // offset 64 (length of debug structure)
    };


//
//  The following variable and function declarations are defined in and
//  exported from cdimage.c.
//

extern OSVERSIONINFO OsInfo;
extern WCHAR         wchVolumeLabel[];
extern PDIRENTRY     pStartOfLevel[];
extern DWORDLONG     dwlTotalNumberOfFiles;
extern DWORDLONG     dwlTotalNumberOfDirectories;
extern DWORD         dwNextMetaBlockNumber;
extern DWORD         dwTotalImageBlocks;
extern LONGLONG      LocalTimeBiasInFileTimeUnits;
extern FILETIME      ftGlobalFileTime;
extern BOOL          bScanOnly;
extern BOOL          bUseGlobalTime;
extern BOOL          bUseAnsiFileNames;
extern BOOL          bOptimizeStorage;
extern BOOL          bContinueAfterFailedSourceOpen;
extern LPSTR         pszSourceTree;
extern LPSTR         pszTargetFile;
extern CHAR          chVolumeLabel[];
extern CHAR          uchVolumeLabel[];
extern WCHAR         wchVolumeLabel[];
extern DWORD         dwBlockSize;
extern PBNODE pFirstBlockHashTree;

extern DWORDLONG dwlTotalDupFiles;
extern DWORDLONG dwlTotalDupFileBytes;          // allocation size

extern BOOL bOpenReadWrite;
extern BOOL bShowDuplicates;

#define IS_SECTOR_ALIGNED( AbsoluteOffset ) (( (AbsoluteOffset) % CD_SECTORSIZE ) == 0 )

#ifdef DEBUG

    #define SECTOR_NUMBER_FROM_BLOCK_NUMBER( BlockNumber ) \
                ( ASSERT( IS_SECTOR_ALIGNED( BlockNumber * (DWORDLONG) dwBlockSize )) ? \
                ( BlockNumber / ( CD_SECTORSIZE / dwBlockSize )) : 0 )

#else

    #define SECTOR_NUMBER_FROM_BLOCK_NUMBER( BlockNumber ) ( BlockNumber / ( CD_SECTORSIZE / dwBlockSize ))

#endif // DEBUG

#define CD_BLOCKS_FROM_SIZE(x) ((DWORD)(ROUNDUP2_64((x),dwBlockSize)/dwBlockSize))

#define BLOCK_NUMBER_FROM_SECTOR_NUMBER( SectorNumber ) ( SectorNumber * ( CD_SECTORSIZE / dwBlockSize ))

VOID
WriteZeroedSectors(
    IN DWORD SectorNumber,
    IN DWORD NumberOfSectors
    );

LPWSTR
AnsiToUnicode(
    IN LPSTR AnsiBuffer,
    IN OUT OPTIONAL LPWSTR UnicodeBuffer
    );

DWORD
AllocateMetaBlocks(
    IN DWORD SizeInBytes
    );

VOID
HeaderWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    );

DWORD
PaddedFileDataWrite(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    );

DWORD
FileDataRead(
    IN DWORDLONG Offset,
    IN PVOID     Buffer,
    IN ULONG     Length
    );

VOID
GenericRead(
    IN DWORDLONG dwlOffset,
    IN DWORD dwLength,
    OUT PVOID pBuffer
    );

BOOL SetImageLength(
    IN LONGLONG llOffset
    );
