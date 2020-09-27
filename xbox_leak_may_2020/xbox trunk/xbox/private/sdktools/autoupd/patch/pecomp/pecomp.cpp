
#include <stdlib.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN 1
#define STRICT
#include <windows.h>


typedef unsigned __int64 QWORD;
typedef unsigned __int32 RVA;

#ifndef ROUNDUP
#define ROUNDUP( x, n ) ((((x) + ((n) - 1 )) / (n)) * (n))
#endif

#ifndef ROUNDUP2
#define ROUNDUP2( x, n ) (((x) + ((n) - 1 )) & ~((n) - 1 ))
#endif

#ifndef MIN
#define MIN( a, b ) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX( a, b ) (((a)>(b))?(a):(b))
#endif

#ifndef countof
#define countof( array ) (sizeof(array)/sizeof(array[0]))
#endif


#define MAX_DIFFS_DISPLAYED 25



#define PECOMP_IGNORE_TIMESTAMPS    0x0000000000000001
#define PECOMP_IGNORE_BINDING_INFO  0x0000000000000002
#define PECOMP_IGNORE_VERSION_INFO  0x0000000000000004
#define PECOMP_IGNORE_BASE_ADDRESS  0x0000000000000008
#define PECOMP_IGNORE_IMAGE_SIZE    0x0000000000000010
#define PECOMP_IGNORE_DOS_HEADER    0x0000000000000020

//      PECOMP_IGNORE_IMPORT_ORDER  0x0000000000000040  (this is difficult because IAT slots move around)

#define PECOMP_IGNORE_LOCALIZATION  0x0000000100000000
#define PECOMP_IGNORE_ALL_RESOURCES 0x0000000200000000

#define PECOMP_SHOW_PRIMARY_DIFFS   0x8000000000000000
#define PECOMP_SHOW_IGNORED_DIFFS   0x4000000000000000

#define PECOMP_SILENT               0x1000000000000000

#define PECOMP_ONLY_PE              0x0800000000000000
#define PECOMP_PE_AS_RAW            0x0400000000000000

#define PECOMP_DEFAULT_FLAGS        0x00000000FFFFFFFF


#define GLE ((DWORD) -1)

#define NORETURN __declspec( noreturn )


QWORD gCompFlags = PECOMP_DEFAULT_FLAGS;

ULONG gDiffSectionCount;

enum IGNORE_TYPE
{
    IgnoreQWORD,
    IgnoreDWORD,
    IgnoreWORD,
    IgnoreBYTE,
    IgnoreTEXT,
};


struct IGNORE_VALUE
{
    PBYTE pbIgnore;
    ULONG cbIgnore;
};


IGNORE_VALUE* gIgnoreArray;
ULONG         gIgnoreCount;


void
AddIgnoreValue(
    IGNORE_TYPE eType,
    QWORD       qwValue
    );


void
ZeroIgnoreValuesInBufferPair(
    PBYTE pbBuffer1,
    PBYTE pbBuffer2,
    ULONG cbBuffer1,
    ULONG cbBuffer2
    );


VOID
NORETURN
ErrorExit(
    DWORD  dwGLE,
    LPCSTR pszFormat,
    ...
    );


VOID __inline ReportDiffNewSection( VOID )
{
    gDiffSectionCount = 0;
}


VOID
ReportDiff(
    LPCSTR pszFormat,
    ...
    );


BOOL
MyMapViewOfFileA(
    IN  LPCSTR  FileName,
    OUT SIZE_T *FileSize,
    OUT HANDLE *FileHandle,
    OUT PVOID  *MapBase
    );

ULONG
CompareRawBytes(
    PBYTE  pbData1,
    PBYTE  pbData2,
    ULONG  cbData1,
    ULONG  cbData2,
    LPCSTR SectName
    );

ULONG
CompareFields32(
    DWORD  Value1,
    DWORD  Value2,
    LPCSTR FieldName,
    LPCSTR SectName
    );

PIMAGE_NT_HEADERS
__fastcall
GetNtHeader(
    IN PVOID  MappedFile,
    IN SIZE_T MappedFileSize
    );

SIZE_T
PeCompareMappedFiles32(
    PVOID  MappedFile1,     // destructive!
    SIZE_T FileSize1,
    PVOID  MappedFile2,     // destructive!
    SIZE_T FileSize2
    );

BOOL
RebaseMappedImage32(
    IN PBYTE               MappedFile,
    IN ULONG               FileSize,
    IN PIMAGE_NT_HEADERS32 NtHeader,
    IN ULONG               NewBase
    );

BOOL
UnbindMappedImage(
    IN PVOID MappedFile,
    IN ULONG FileSize,
    IN PIMAGE_NT_HEADERS NtHeader
    );

BOOL
SmashLockPrefixesInMappedImage(
    IN PVOID             MappedFile,
    IN ULONG             FileSize,
    IN PIMAGE_NT_HEADERS NtHeader
    );

VOID
NormalizePe32FileForComparison(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    );

VOID
NormalizePe32Header(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    );

VOID
NormalizePe32Imports(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    );

VOID
NormalizePe32Exports(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    );

VOID
NormalizePe32DebugInfo(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    );

VOID
NormalizePe32Resources(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    );


struct _RESINFO
{
    PIMAGE_NT_HEADERS32 NtHeader;
    PVOID MappedFile;
    ULONG FileSize;
    ULONG ResourceBaseRva;
    PBYTE ResourceBaseMapped;
    ULONG Depth;
    BOOL  VersionTree;
};

typedef struct _RESINFO RESINFO;


struct _RESINFO2
{
    PIMAGE_NT_HEADERS32 NtHeader1;
    PIMAGE_NT_HEADERS32 NtHeader2;
    PVOID MappedFile1;
    PVOID MappedFile2;
    ULONG FileSize1;
    ULONG FileSize2;
    ULONG ResourceBaseRva1;
    ULONG ResourceBaseRva2;
    PBYTE ResourceBaseMapped1;
    PBYTE ResourceBaseMapped2;
    ULONG Depth;
    LPSTR DisplayBuffer;
};

typedef struct _RESINFO2 RESINFO2;


VOID
__fastcall
NormalizePe32ResourceDirectoryTree(
    PIMAGE_RESOURCE_DIRECTORY ResourceDir,
    RESINFO* ResInfo
    );

ULONG
__fastcall
ComparePe32ResourceDirectoryTree(
    PIMAGE_RESOURCE_DIRECTORY ResourceDir1,
    PIMAGE_RESOURCE_DIRECTORY ResourceDir2,
    RESINFO2* ResInfo
    );


VOID __fastcall StripTrailingSpaces( LPSTR String );

VOID
NORETURN
Usage( void )
{
    fprintf(
        stdout,
        "\n"
        "Usage: PECOMP [Options] ExeFile1 ExeFile2\n"
        "\n"
        "\tOptions:\n"
        "\n"
        "\t\t/OnlyPE                (indicate error if files are not PE)\n"
        "\t\t/PeIgnoreAllResources  (versus layout-independent comparison)\n"
        "\t\t/PeIgnoreLocalization  (compare resource structure, not data)\n"
        "\t\t/PeCompareAsRaw        (compare PE files as raw versus PE smart)\n"
        "\t\t/ShowDiffs             (verbose mode displays each difference)\n"
        "\t\t/Silent                (only set return code for batch mode)\n"
//      "\t\t/ShowIgnoredDiffs      (not yet implemented)\n"
        "\t\t/IgnoreQWORD:hexval    (ignore 8-byte QWORDs of given value)\n"
        "\t\t/IgnoreDWORD:hexval    (ignore 4-byte DWORDs of given value)\n"
        "\t\t/IgnoreWORD:hexval     (ignore 2-byte WORDs of given value)\n"
        "\t\t/IgnoreBYTE:hexval     (ignore single BYTEs of given value)\n"
        "\t\t/IgnoreTEXT:abcdefg    (ignore specific text, ascii and unicode)\n"
        "\n"
        "\tReturn codes:\n"
        "\n"
        "\t\t     zero: identical or equivalent files\n"
        "\t\t non-zero: number of differences between files\n"
        "\t\t999999999: error occurred, comparison not performed\n"
        "\n"
        );

    exit( 999999999 );
}


__declspec( noreturn ) void __cdecl main( int argc, char** argv )
{
    LPSTR  FileName1    = NULL;
    SIZE_T FileSize1    = 0;
    HANDLE FileHandle1  = NULL;
    PVOID  MappedFile1  = NULL;

    LPSTR  FileName2    = NULL;
    SIZE_T FileSize2    = 0;
    HANDLE FileHandle2  = NULL;
    PVOID  MappedFile2  = NULL;


    for ( int arg = 1; arg < argc; arg++ )
    {
        char* p = argv[ arg ];

        if ( strchr( p, '?' ))
        {
            Usage();
        }

        if (( *p == '/' ) || ( *p == '-' ))
        {
            ++p;

            if ( ! _stricmp( p, "OnlyPE" ))
            {
                gCompFlags |= PECOMP_ONLY_PE;
            }
            else if ( ! _stricmp( p, "PeOnly" ))
            {
                gCompFlags |= PECOMP_ONLY_PE;
            }
            else if ( ! _stricmp( p, "PeCompareAsRaw" ))
            {
                gCompFlags |= PECOMP_PE_AS_RAW;
            }
            else if ( ! _stricmp( p, "Silent" ))
            {
                gCompFlags |= PECOMP_SILENT;
            }
            else if ( ! _stricmp( p, "ShowDiffs" ))
            {
                gCompFlags |= PECOMP_SHOW_PRIMARY_DIFFS;
            }
            else if ( ! _stricmp( p, "ShowPrimaryDiffs" ))
            {
                gCompFlags |= PECOMP_SHOW_PRIMARY_DIFFS;
            }
            else if ( ! _stricmp( p, "ShowIgnoredDiffs" ))
            {
                gCompFlags |= PECOMP_SHOW_IGNORED_DIFFS;
            }
            else if ( ! _stricmp( p, "PeIgnoreAllResources" ))
            {
                gCompFlags |= PECOMP_IGNORE_ALL_RESOURCES;
            }
            else if ( ! _stricmp( p, "IgnoreAllResources" ))
            {
                gCompFlags |= PECOMP_IGNORE_ALL_RESOURCES;
            }
            else if ( ! _stricmp( p, "PeIgnoreLocalization" ))
            {
                gCompFlags |= PECOMP_IGNORE_LOCALIZATION;
            }
            else if ( ! _stricmp( p, "IgnoreLocalization" ))
            {
                gCompFlags |= PECOMP_IGNORE_LOCALIZATION;
            }
            else if ( ! _strnicmp( p, "IgnoreQWORD:", 12 ))
            {
                AddIgnoreValue( IgnoreQWORD, _strtoui64( p + 12, NULL, 16 ));
            }
            else if ( ! _strnicmp( p, "IgnoreDWORD:", 12 ))
            {
                AddIgnoreValue( IgnoreDWORD, _strtoui64( p + 12, NULL, 16 ));
            }
            else if ( ! _strnicmp( p, "IgnoreWORD:", 11 ))
            {
                AddIgnoreValue( IgnoreWORD, _strtoui64( p + 11, NULL, 16 ));
            }
            else if ( ! _strnicmp( p, "IgnoreBYTE:", 11 ))
            {
                AddIgnoreValue( IgnoreBYTE, _strtoui64( p + 11, NULL, 16 ));
            }
            else if ( ! _strnicmp( p, "IgnoreTEXT:", 11 ))
            {
                AddIgnoreValue( IgnoreTEXT, (QWORD)( p + 11 ));
            }
            else
            {
                Usage();
            }
        }
        else if ( FileName1 == NULL )
        {
            FileName1 = p;
        }
        else if ( FileName2 == NULL )
        {
            FileName2 = p;
        }
        else
        {
            Usage();
        }
    }

    if ( FileName2 == NULL )
    {
        Usage();
    }

    if ( gCompFlags & PECOMP_SILENT )
    {
        gCompFlags &= ~PECOMP_SHOW_PRIMARY_DIFFS;
        gCompFlags &= ~PECOMP_SHOW_IGNORED_DIFFS;
    }

    BOOL Success;

    Success = MyMapViewOfFileA(
                  FileName1,
                  &FileSize1,
                  &FileHandle1,
                  &MappedFile1
                  );

    if ( ! Success )
    {
        ErrorExit( GLE, "Failed to open file %s.\n", FileName1 );
    }

    Success = MyMapViewOfFileA(
                  FileName2,
                  &FileSize2,
                  &FileHandle2,
                  &MappedFile2
                  );

    if ( ! Success )
    {
        ErrorExit( GLE, "Failed to open file %s.\n", FileName2 );
    }

    SIZE_T DiffCount = MAX( FileSize1, FileSize2 );

    LPCSTR FileType = "Raw";

    __try
    {
        PIMAGE_NT_HEADERS NtHeader1 = GetNtHeader( MappedFile1, FileSize1 );
        PIMAGE_NT_HEADERS NtHeader2 = GetNtHeader( MappedFile2, FileSize2 );

        if ( NtHeader1 && NtHeader2 )
        {
            if ( gCompFlags & PECOMP_PE_AS_RAW )
            {
                NtHeader1 = NULL;   // force RAW comparison
                NtHeader2 = NULL;
            }
        }
        else
        {
            if ( gCompFlags & PECOMP_ONLY_PE )
            {
                ErrorExit( 0, "File %s is not a PE file.\n", ( NtHeader1 ? FileName2 : FileName1 ));
            }
        }

        if ( FileSize1 == FileSize2 )   // quick test for identical files
        {
            if ( memcmp( MappedFile1, MappedFile2, FileSize1 ) == 0 )
            {
                if ( ! ( gCompFlags & PECOMP_SILENT ))
                {
                    fprintf( stdout, "IDENTICAL\n" );
                }

                exit( 0 );
            }
        }

        if ( NtHeader1 && NtHeader2 )   // pe comparsion
        {
            FileType = "PE";

            if ( NtHeader1->OptionalHeader.Magic != NtHeader2->OptionalHeader.Magic )
            {
                ErrorExit( 0, "Mismatched PE header magic types.\n" );
            }

            if ( NtHeader1->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC )
            {
                FileType = "PE32";

                DiffCount = PeCompareMappedFiles32(
                                MappedFile1,
                                FileSize1,
                                MappedFile2,
                                FileSize2
                                );
            }
            else if ( NtHeader1->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC )
            {
                FileType = "PE64";

                ErrorExit( 0, "PE64 file comparison is not yet supported.\n" );
            }
            else
            {
                ErrorExit( 0, "Unrecognized PE header magic type.\n" );
            }
        }
        else    // raw comparsion
        {

            ZeroIgnoreValuesInBufferPair(
                (PBYTE) MappedFile1,
                (PBYTE) MappedFile2,
                FileSize1,
                FileSize2
                );

            DiffCount = CompareRawBytes(
                            (PBYTE) MappedFile1,
                            (PBYTE) MappedFile2,
                            FileSize1,
                            FileSize2,
                            "RawFile"
                            );

            ReportDiffNewSection();

            DiffCount += CompareFields32(
                            FileSize1,
                            FileSize2,
                            "FileSize",
                            "RawFile"
                            );
        }
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        ErrorExit( GetExceptionCode(), "Exception occurred while comparing files.\n" );
    }

    if ( ! ( gCompFlags & PECOMP_SILENT ))
    {
        if ( DiffCount != 0 )
        {
            if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
            {
                fprintf( stderr, "\n" );
                fflush( stderr );
            }
        }

        if ( DiffCount == 0 )
        {
            fprintf( stdout, "EQUIVALENT (%s)\n", FileType );
        }
        else if ( DiffCount <= 100 )
        {
            fprintf( stdout, "DIFFERENT (%s): %u or more differences.\n", FileType, DiffCount );
        }
        else
        {
            fprintf( stdout, "DIFFERENT (%s): more than 100 differences.\n", FileType, DiffCount );
        }
    }

    exit( DiffCount );
}


void
AddIgnoreValue(
    IGNORE_TYPE eType,
    QWORD       qwValue
    )
{
    gIgnoreArray = (IGNORE_VALUE*) realloc( gIgnoreArray, (( gIgnoreCount + 2 ) * sizeof( gIgnoreArray[0] )));

    if ( gIgnoreArray == NULL )
    {
        ErrorExit( ERROR_OUTOFMEMORY, "Failed allocating memory for ignore array.\n" );
    }

    if ( eType == IgnoreTEXT )
    {
        if ( qwValue == 0 )
        {
            ErrorExit( 0, "Invalid /IgnoreTEXT.\n" );
        }

        LPCSTR pbTextA = (LPCSTR) qwValue;
        ULONG  cbTextA = strlen( pbTextA );

        if ( cbTextA == 0 )
        {
            ErrorExit( 0, "Invalid /IgnoreTEXT (must be at least one character)\n." );
        }

        gIgnoreArray[ gIgnoreCount ].pbIgnore = (PBYTE) malloc( cbTextA + 1 );

        if ( gIgnoreArray[ gIgnoreCount ].pbIgnore == NULL )
        {
            ErrorExit( ERROR_OUTOFMEMORY, "Failed allocating memory for ignore string.\n" );
        }

        memcpy( gIgnoreArray[ gIgnoreCount ].pbIgnore, pbTextA, cbTextA + 1 );
        gIgnoreArray[ gIgnoreCount ].cbIgnore = cbTextA;
        gIgnoreCount++;

        gIgnoreArray[ gIgnoreCount ].pbIgnore = (PBYTE) malloc(( cbTextA + 1 ) * 2 );

        if ( gIgnoreArray[ gIgnoreCount ].pbIgnore == NULL )
        {
            ErrorExit( ERROR_OUTOFMEMORY, "Failed allocating memory for ignore string.\n" );
        }

        LPWSTR pbTextU = (LPWSTR) gIgnoreArray[ gIgnoreCount ].pbIgnore;
        mbstowcs( pbTextU, pbTextA, cbTextA + 1 );
        ULONG cnTextU = wcslen( pbTextU );

        if ( cnTextU > 0 )
        {
            gIgnoreArray[ gIgnoreCount ].cbIgnore = cnTextU * 2;
            gIgnoreCount++;
        }
    }
    else
    {
        if ( qwValue == 0 )
        {
            ErrorExit( 0, "Invalid /Ignore value (must be non-zero).\n" );
        }

        ULONG cb = 0;

        switch ( eType )
        {
            case IgnoreQWORD: cb = 8; break;
            case IgnoreDWORD: cb = 4; break;
            case IgnoreWORD:  cb = 2; break;
            case IgnoreBYTE:  cb = 1; break;
        }

        if ( cb == 0 )
        {
            ErrorExit( 0, "Invalid /Ignore type.\n" );
        }

        gIgnoreArray[ gIgnoreCount ].pbIgnore = (PBYTE) malloc( cb );

        if ( gIgnoreArray[ gIgnoreCount ].pbIgnore == NULL )
        {
            ErrorExit( ERROR_OUTOFMEMORY, "Failed allocating memory for ignore value.\n" );
        }

        memcpy( gIgnoreArray[ gIgnoreCount ].pbIgnore, &qwValue, cb );
        gIgnoreArray[ gIgnoreCount ].cbIgnore = cb;
        gIgnoreCount++;
    }
}


void
ZeroIgnoreValuesInBufferPair(
    PBYTE pbBuffer1,
    PBYTE pbBuffer2,
    ULONG cbBuffer1,
    ULONG cbBuffer2
    )
{
    for ( unsigned i = 0; i < gIgnoreCount; i++ )
    {
        PBYTE pbIgnore = gIgnoreArray[ i ].pbIgnore;
        ULONG cbIgnore = gIgnoreArray[ i ].cbIgnore;

        if ( cbIgnore > 0 )
        {
            if ( cbIgnore <= cbBuffer1 )
            {
                for ( unsigned j = 0; j < (( cbBuffer1 - cbIgnore ) + 1 ); j++ )
                {
                    if ( pbBuffer1[ j ] == *pbIgnore )
                    {
                        if ( memcmp( pbBuffer1 + j + 1, pbIgnore + 1, cbIgnore - 1 ) == 0 )
                        {
                            memset( pbBuffer1 + j, 0, cbIgnore );

                            for ( unsigned k = 0; ( k < cbIgnore ) && (( j + k ) < cbBuffer2 ); k++ )
                            {
                                pbBuffer2[ j + k ] = 0;
                            }
                        }
                    }
                }
            }

            if ( cbIgnore <= cbBuffer2 )
            {
                for ( unsigned j = 0; j < (( cbBuffer2 - cbIgnore ) + 1 ); j++ )
                {
                    if ( pbBuffer2[ j ] == *pbIgnore )
                    {
                        if ( memcmp( pbBuffer2 + j + 1, pbIgnore + 1, cbIgnore - 1 ) == 0 )
                        {
                            memset( pbBuffer2 + j, 0, cbIgnore );

                            for ( unsigned k = 0; ( k < cbIgnore ) && (( j + k ) < cbBuffer1 ); k++ )
                            {
                                pbBuffer1[ j + k ] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}


BOOL
MyMapViewOfFileByHandle(
    IN  HANDLE  FileHandle,
    OUT SIZE_T *FileSize,
    OUT PVOID  *MapBase
    )
    {
    ULONG  InternalFileSizeLow;
    ULONG  InternalFileSizeHigh;
    SIZE_T InternalFileSize;
    HANDLE InternalMapHandle;
    PVOID  InternalMapBase;

    InternalFileSizeLow = GetFileSize( FileHandle, &InternalFileSizeHigh );

    InternalFileSize = ((SIZE_T)( InternalFileSizeHigh ) << 32 ) | InternalFileSizeLow;

    if ( InternalFileSize == 0 ) {
        *MapBase  = NULL;
        *FileSize = 0;
        return TRUE;
        }

    InternalMapHandle = CreateFileMapping(
                            FileHandle,
                            NULL,
                            PAGE_WRITECOPY,
                            0,
                            0,
                            NULL
                            );

    if ( InternalMapHandle != NULL ) {

        InternalMapBase = MapViewOfFile(
                              InternalMapHandle,
                              FILE_MAP_COPY,
                              0,
                              0,
                              0
                              );

        CloseHandle( InternalMapHandle );

        if ( InternalMapBase != NULL ) {

            *MapBase  = InternalMapBase;
            *FileSize = InternalFileSize;

            return TRUE;
            }
        }

    return FALSE;
    }


BOOL
MyMapViewOfFileA(
    IN  LPCSTR  FileName,
    OUT SIZE_T *FileSize,
    OUT HANDLE *FileHandle,
    OUT PVOID  *MapBase
    )
    {
    HANDLE InternalFileHandle;
    BOOL   Success;

    InternalFileHandle = CreateFileA(
                             FileName,
                             GENERIC_READ,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_FLAG_SEQUENTIAL_SCAN,
                             NULL
                             );

    if ( InternalFileHandle != INVALID_HANDLE_VALUE ) {

        Success = MyMapViewOfFileByHandle(
                      InternalFileHandle,
                      FileSize,
                      MapBase
                      );

        if ( Success ) {

            *FileHandle = InternalFileHandle;

            return TRUE;
            }

        CloseHandle( InternalFileHandle );
        }

    return FALSE;
    }


PIMAGE_NT_HEADERS
__fastcall
GetNtHeader(
    IN PVOID  MappedFile,
    IN SIZE_T MappedFileSize
    )
{
    PIMAGE_DOS_HEADER DosHeader;
    PIMAGE_NT_HEADERS RetHeader;
    PIMAGE_NT_HEADERS NtHeader;

    RetHeader = NULL;

    __try
    {
        if ( MappedFileSize >= 0x200 )
        {
            DosHeader = (PIMAGE_DOS_HEADER) MappedFile;

            if ( DosHeader->e_magic == IMAGE_DOS_SIGNATURE )
            {
                NtHeader = (PIMAGE_NT_HEADERS)((PBYTE) MappedFile + DosHeader->e_lfanew );

                if (((PBYTE) NtHeader + sizeof( IMAGE_NT_HEADERS )) <= ((PBYTE) MappedFile + MappedFileSize ))
                {
                    if ( NtHeader->Signature == IMAGE_NT_SIGNATURE )
                    {
                        RetHeader = NtHeader;
                    }
                }
            }
        }
    }
    __except( EXCEPTION_EXECUTE_HANDLER ) {}

    return RetHeader;
}


ULONG
__fastcall
ImageRvaToFileOffset(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN ULONG Rva
    )
    {
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG SectionCount;
    ULONG i;

    if ( Rva < NtHeader->OptionalHeader.SizeOfHeaders ) {
        return Rva;
        }

    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {
        if (( Rva >= SectionHeader[ i ].VirtualAddress ) &&
            ( Rva <  SectionHeader[ i ].VirtualAddress + SectionHeader[ i ].SizeOfRawData )) {

            return ( SectionHeader[ i ].PointerToRawData + ( Rva - SectionHeader[ i ].VirtualAddress ));
            }
        }

    return 0;
    }


PVOID
__fastcall
ImageRvaToMappedAddress(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN ULONG             Rva,
    IN PVOID             MappedBase,
    IN ULONG             MappedSize
    )
    {
    ULONG MappedOffset = ImageRvaToFileOffset( NtHeader, Rva );

    if (( MappedOffset ) && ( MappedOffset < MappedSize )) {
        return ( (PBYTE) MappedBase + MappedOffset );
        }

    return NULL;
    }


ULONG
__fastcall
ImageVaToFileOffset(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN ULONG             Va
    )
    {
    return ImageRvaToFileOffset( NtHeader, Va - NtHeader->OptionalHeader.ImageBase );
    }


PVOID
__fastcall
ImageVaToMappedAddress(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN ULONG             Va,
    IN PVOID             MappedBase,
    IN ULONG             MappedSize
    )
    {
    return ImageRvaToMappedAddress( NtHeader, Va - NtHeader->OptionalHeader.ImageBase, MappedBase, MappedSize );
    }


ULONG
__fastcall
ImageDirectoryRvaAndSize(
    IN  PIMAGE_NT_HEADERS NtHeader,
    IN  ULONG             DirectoryIndex,
    OUT PULONG OPTIONAL   DirectorySize
    )
    {
    if ( DirectoryIndex < NtHeader->OptionalHeader.NumberOfRvaAndSizes ) {

        if ( DirectorySize ) {
            *DirectorySize = NtHeader->OptionalHeader.DataDirectory[ DirectoryIndex ].Size;
            }

        return NtHeader->OptionalHeader.DataDirectory[ DirectoryIndex ].VirtualAddress;
        }

    return 0;
    }


ULONG
__fastcall
ImageDirectoryOffsetAndSize(
    IN  PIMAGE_NT_HEADERS NtHeader,
    IN  ULONG             DirectoryIndex,
    OUT PULONG OPTIONAL   DirectorySize
    )
    {
    ULONG Rva = ImageDirectoryRvaAndSize( NtHeader, DirectoryIndex, DirectorySize );

    if ( Rva ) {
        return ImageRvaToFileOffset( NtHeader, Rva );
        }

    return 0;
    }


PVOID
__fastcall
ImageDirectoryMappedAddress(
    IN  PIMAGE_NT_HEADERS NtHeader,
    IN  ULONG             DirectoryIndex,
    OUT PULONG OPTIONAL   DirectorySize,
    IN  PVOID             MappedBase,
    IN  ULONG             MappedSize
    )
{
    PVOID Directory;
    ULONG LocalSize;
    ULONG Rva;

    Rva = ImageDirectoryRvaAndSize( NtHeader, DirectoryIndex, &LocalSize );

    Directory = ImageRvaToMappedAddress( NtHeader, Rva, MappedBase, MappedSize );

    if ( ( Directory ) && (((PBYTE) Directory + LocalSize ) <= ((PBYTE) MappedBase + MappedSize )) )
    {
        if ( DirectorySize )
        {
            *DirectorySize = LocalSize;
        }

        return Directory;
    }

    return NULL;
}


SIZE_T
PeCompareMappedFiles32(
    PVOID  MappedFile1,     // destructive!
    SIZE_T FileSize1,
    PVOID  MappedFile2,     // destructive!
    SIZE_T FileSize2
    )
{
    SIZE_T MaxFileSize = MAX( FileSize1, FileSize2 );
    SIZE_T ReturnValue = MaxFileSize;

    __try
    {
        PIMAGE_NT_HEADERS32 NtHeader1 = GetNtHeader( MappedFile1, FileSize1 );
        PIMAGE_NT_HEADERS32 NtHeader2 = GetNtHeader( MappedFile2, FileSize2 );
        BOOL Success;

        if (( NtHeader1 == NULL ) || ( NtHeader2 == NULL ))
        {
            __leave;
        }

        if ( NtHeader1->OptionalHeader.Magic != NtHeader2->OptionalHeader.Magic )
        {
            __leave;
        }

        if ( NtHeader1->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC )
        {
            __leave;
        }

        ULONG BaseAddress1 = NtHeader1->OptionalHeader.ImageBase;
        ULONG BaseAddress2 = NtHeader2->OptionalHeader.ImageBase;

        if ( BaseAddress2 != BaseAddress1 )
        {
            if ( gCompFlags & PECOMP_IGNORE_BASE_ADDRESS )
            {
                Success = RebaseMappedImage32(
                                   (PBYTE) MappedFile2,
                                   FileSize2,
                                   NtHeader2,
                                   BaseAddress1
                                   );

                if ( ! Success )
                {
                    Success = RebaseMappedImage32(
                                       (PBYTE) MappedFile1,
                                       FileSize1,
                                       NtHeader1,
                                       BaseAddress2
                                       );
                }

                if ( ! Success )
                {
                    ErrorExit( 0, "Failed to rebase PE files to same base address for comparison.\n" );
                }
            }
        }

        //
        //  BUGBUG: If want to display ignored errors, must do it before
        //          overwrite ignored regions with zeros.
        //

        NormalizePe32FileForComparison( MappedFile1, FileSize1, NtHeader1 );

        NormalizePe32FileForComparison( MappedFile2, FileSize2, NtHeader2 );

        //
        //  Now compare section by section, header to header, and
        //  slack space before and after header.
        //

        SIZE_T DiffCount = 0;

        ReportDiffNewSection();

        if ( ! ( gCompFlags & PECOMP_IGNORE_DOS_HEADER ))
        {
            PIMAGE_DOS_HEADER DosHeader1 = (PIMAGE_DOS_HEADER) MappedFile1;
            PIMAGE_DOS_HEADER DosHeader2 = (PIMAGE_DOS_HEADER) MappedFile2;

            ULONG cbData1 = DosHeader1->e_lfanew;
            ULONG cbData2 = DosHeader2->e_lfanew;

            ZeroIgnoreValuesInBufferPair(
                (PBYTE) DosHeader1,
                (PBYTE) DosHeader2,
                cbData1,
                cbData2
                );

            DiffCount += CompareRawBytes( (PBYTE) DosHeader1, (PBYTE) DosHeader2, cbData1, cbData2, "MzHeader" );
        }

        ReportDiffNewSection();

        #define COMPARE_FIELDS( p1, p2, field ) \
            CompareFields32( (p1)->field, (p2)->field, #field, SectName );

        #define SectName "PeHeader"

        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, Signature );

        ReportDiffNewSection();

        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, FileHeader.Machine );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, FileHeader.NumberOfSections );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, FileHeader.TimeDateStamp );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, FileHeader.PointerToSymbolTable );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, FileHeader.NumberOfSymbols );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, FileHeader.SizeOfOptionalHeader );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, FileHeader.Characteristics );

        ReportDiffNewSection();

        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.Magic );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.MajorLinkerVersion );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.MinorLinkerVersion );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfCode );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfInitializedData );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfUninitializedData );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.AddressOfEntryPoint );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.BaseOfCode );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.BaseOfData );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.ImageBase );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SectionAlignment );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.FileAlignment );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.MajorOperatingSystemVersion );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.MinorOperatingSystemVersion );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.MajorImageVersion );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.MinorImageVersion );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.MajorSubsystemVersion );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.MinorSubsystemVersion );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.Win32VersionValue );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfImage );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfHeaders );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.CheckSum );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.Subsystem );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.DllCharacteristics );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfStackReserve );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfStackCommit );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfHeapReserve );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.SizeOfHeapCommit );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.LoaderFlags );
        DiffCount += COMPARE_FIELDS( NtHeader1, NtHeader2, OptionalHeader.NumberOfRvaAndSizes );

        ReportDiffNewSection();

        static const LPCSTR PeDirNameAddr[ IMAGE_NUMBEROF_DIRECTORY_ENTRIES ] =
        {
            "Directory[ EXPORT      ].Address",
            "Directory[ IMPORT      ].Address",
            "Directory[ RESOURCE    ].Address",
            "Directory[ EXCEPTION   ].Address",
            "Directory[ SECURITY    ].Address",
            "Directory[ BASERELOC   ].Address",
            "Directory[ DEBUG       ].Address",
            "Directory[ ARCH        ].Address",
            "Directory[ GLOBALPTR   ].Address",
            "Directory[ TLS         ].Address",
            "Directory[ LOAD_CONFIG ].Address",
            "Directory[ BOUNDIMPORT ].Address",
            "Directory[ IAT         ].Address",
            "Directory[ DELAYIMPORT ].Address",
            "Directory[ COMDESC     ].Address",
            "Directory[ 15          ].Address"
        };

        static const LPCSTR PeDirNameSize[ IMAGE_NUMBEROF_DIRECTORY_ENTRIES ] =
        {
            "Directory[ EXPORT      ].Size",
            "Directory[ IMPORT      ].Size",
            "Directory[ RESOURCE    ].Size",
            "Directory[ EXCEPTION   ].Size",
            "Directory[ SECURITY    ].Size",
            "Directory[ BASERELOC   ].Size",
            "Directory[ DEBUG       ].Size",
            "Directory[ ARCH        ].Size",
            "Directory[ GLOBALPTR   ].Size",
            "Directory[ TLS         ].Size",
            "Directory[ LOAD_CONFIG ].Size",
            "Directory[ BOUNDIMPORT ].Size",
            "Directory[ IAT         ].Size",
            "Directory[ DELAYIMPORT ].Size",
            "Directory[ COMDESC     ].Size",
            "Directory[ 15          ].Size"
        };

        for ( unsigned i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++ )
        {
            if ( i != IMAGE_DIRECTORY_ENTRY_BASERELOC )
            {
                DiffCount += CompareFields32(
                                 NtHeader1->OptionalHeader.DataDirectory[ i ].VirtualAddress,
                                 NtHeader2->OptionalHeader.DataDirectory[ i ].VirtualAddress,
                                 PeDirNameAddr[ i ],
                                 SectName
                                 );
            }

            if ( i != IMAGE_DIRECTORY_ENTRY_RESOURCE )
            {
                DiffCount += CompareFields32(
                                 NtHeader1->OptionalHeader.DataDirectory[ i ].Size,
                                 NtHeader2->OptionalHeader.DataDirectory[ i ].Size,
                                 PeDirNameSize[ i ],
                                 SectName
                                 );
            }
        }

        #undef SectName

        ReportDiffNewSection();

        ULONG ResourceDirRva1  = NtHeader1->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_RESOURCE ].VirtualAddress;
        ULONG ResourceDirSize1 = NtHeader1->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_RESOURCE ].Size;
        PVOID ResourceDirPtr1  = ImageRvaToMappedAddress( NtHeader1, ResourceDirRva1, MappedFile1, FileSize1 );

        ULONG ResourceDirRva2  = NtHeader2->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_RESOURCE ].VirtualAddress;
        ULONG ResourceDirSize2 = NtHeader2->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_RESOURCE ].Size;
        PVOID ResourceDirPtr2  = ImageRvaToMappedAddress( NtHeader2, ResourceDirRva2, MappedFile2, FileSize2 );

        if ( ! ( gCompFlags & PECOMP_IGNORE_ALL_RESOURCES ))
        {
            if (( ResourceDirPtr1 ) && ( ResourceDirPtr2 ))
            {
                RESINFO2 ResInfo;

                ResInfo.NtHeader1           = NtHeader1;
                ResInfo.NtHeader2           = NtHeader2;
                ResInfo.MappedFile1         = MappedFile1;
                ResInfo.MappedFile2         = MappedFile2;
                ResInfo.FileSize1           = FileSize1;
                ResInfo.FileSize2           = FileSize2;
                ResInfo.ResourceBaseRva1    = ResourceDirRva1;
                ResInfo.ResourceBaseRva2    = ResourceDirRva2;
                ResInfo.ResourceBaseMapped1 = (PBYTE) ResourceDirPtr1;
                ResInfo.ResourceBaseMapped2 = (PBYTE) ResourceDirPtr2;
                ResInfo.Depth               = 0;
                ResInfo.DisplayBuffer       = NULL;

                if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
                {
                    ResInfo.DisplayBuffer = (LPSTR) VirtualAlloc( NULL, 0x10000, MEM_COMMIT, PAGE_READWRITE );
                    strcpy( ResInfo.DisplayBuffer, "Resource" );
                }

                DiffCount += ComparePe32ResourceDirectoryTree(
                                 (PIMAGE_RESOURCE_DIRECTORY) ResourceDirPtr1,
                                 (PIMAGE_RESOURCE_DIRECTORY) ResourceDirPtr2,
                                 &ResInfo
                                 );

                if ( ResInfo.DisplayBuffer )
                {
                    VirtualFree( ResInfo.DisplayBuffer, 0, MEM_RELEASE );
                }

                memset( ResourceDirPtr1, 0, ResourceDirSize1 );
                memset( ResourceDirPtr2, 0, ResourceDirSize2 );
            }
        }

        ReportDiffNewSection();

        ULONG RelocDirRva1 = NtHeader1->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BASERELOC ].VirtualAddress;

        PIMAGE_SECTION_HEADER RsrcSection1  = NULL;
        PIMAGE_SECTION_HEADER RelocSection1 = NULL;

        {
            PIMAGE_SECTION_HEADER SectionHeader1 = IMAGE_FIRST_SECTION( NtHeader1 );
            ULONG SectionCount1 = NtHeader1->FileHeader.NumberOfSections;

            while ( SectionCount1-- )
            {
                if (( ResourceDirRva1 >= SectionHeader1->VirtualAddress ) &&
                    ( ResourceDirRva1 <  SectionHeader1->VirtualAddress + SectionHeader1->SizeOfRawData ))
                {
                    RsrcSection1 = SectionHeader1;
                }

                else if (( RelocDirRva1 >= SectionHeader1->VirtualAddress ) &&
                         ( RelocDirRva1 <  SectionHeader1->VirtualAddress + SectionHeader1->SizeOfRawData ))
                {
                    RelocSection1 = SectionHeader1;
                }

                SectionHeader1++;
            }
        }

        PIMAGE_SECTION_HEADER SectionHeader1 = IMAGE_FIRST_SECTION( NtHeader1 );
        PIMAGE_SECTION_HEADER SectionHeader2 = IMAGE_FIRST_SECTION( NtHeader2 );
        ULONG                 SectionCount1  = NtHeader1->FileHeader.NumberOfSections;
        ULONG                 SectionCount2  = NtHeader2->FileHeader.NumberOfSections;
        ULONG                 SectionCountM  = MIN( SectionCount1, SectionCount2 );

        while ( SectionCountM-- )
        {
            CHAR SectName1[ sizeof( SectionHeader1->Name ) + 1 ];
            CHAR SectName2[ sizeof( SectionHeader2->Name ) + 1 ];

            memcpy( SectName1, SectionHeader1->Name, sizeof( SectionHeader1->Name ));
            SectName1[ sizeof( SectionHeader1->Name ) ] = 0;

            memcpy( SectName2, SectionHeader2->Name, sizeof( SectionHeader2->Name ));
            SectName2[ sizeof( SectionHeader2->Name ) ] = 0;

            StripTrailingSpaces( SectName1 );
            StripTrailingSpaces( SectName2 );

            if ( _stricmp( SectName1, SectName2 ) == 0 )
            {
                #define SectName SectName1

                DiffCount += COMPARE_FIELDS( SectionHeader1, SectionHeader2, Characteristics );

                if ( SectionHeader1 != RelocSection1 )  // reloc section might move
                {
                    DiffCount += COMPARE_FIELDS( SectionHeader1, SectionHeader2, VirtualAddress );
                }

                if ( SectionHeader1 != RsrcSection1 )   // resource section might grow
                {
                    DiffCount += COMPARE_FIELDS( SectionHeader1, SectionHeader2, Misc.VirtualSize );
                    DiffCount += COMPARE_FIELDS( SectionHeader1, SectionHeader2, SizeOfRawData );
                }

                #undef SectName

            }
            else
            {
                DiffCount++;

                if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
                {
                    ReportDiffNewSection();

                    ReportDiff(
                        "Section %s does not correspond to section %s at same index\n",
                        SectName1,
                        SectName2
                        );
                }
            }

            SectionHeader1++;
            SectionHeader2++;
        }

        SectionHeader1 = IMAGE_FIRST_SECTION( NtHeader1 );
        SectionHeader2 = IMAGE_FIRST_SECTION( NtHeader2 );
        SectionCountM  = MIN( SectionCount1, SectionCount2 );

        PBYTE pbData1 = (PBYTE)( SectionHeader1 + SectionCount1 );
        PBYTE pbData2 = (PBYTE)( SectionHeader2 + SectionCount2 );
        PBYTE pbStop1 = (PBYTE) MappedFile1 + NtHeader1->OptionalHeader.SizeOfHeaders;
        PBYTE pbStop2 = (PBYTE) MappedFile2 + NtHeader2->OptionalHeader.SizeOfHeaders;
        ULONG cbData1 = ( pbStop1 > pbData1 ) ? ( pbStop1 - pbData1 ) : 0;
        ULONG cbData2 = ( pbStop2 > pbData2 ) ? ( pbStop2 - pbData2 ) : 0;

        DiffCount += CompareRawBytes( pbData1, pbData2, cbData1, cbData2, "PeHeader" );

        while ( SectionCountM-- )
        {
            CHAR SectName1[ sizeof( SectionHeader1->Name ) + 1 ];
            CHAR SectName2[ sizeof( SectionHeader2->Name ) + 1 ];

            memcpy( SectName1, SectionHeader1->Name, sizeof( SectionHeader1->Name ));
            SectName1[ sizeof( SectionHeader1->Name ) ] = 0;

            memcpy( SectName2, SectionHeader2->Name, sizeof( SectionHeader2->Name ));
            SectName2[ sizeof( SectionHeader2->Name ) ] = 0;

            StripTrailingSpaces( SectName1 );
            StripTrailingSpaces( SectName2 );

            if ( _stricmp( SectName1, SectName2 ) == 0 )
            {
                ZeroIgnoreValuesInBufferPair(
                    (PBYTE) MappedFile1 + SectionHeader1->PointerToRawData,
                    (PBYTE) MappedFile2 + SectionHeader2->PointerToRawData,
                    SectionHeader1->SizeOfRawData,
                    SectionHeader2->SizeOfRawData
                    );

                DiffCount += CompareRawBytes(
                                 (PBYTE) MappedFile1 + SectionHeader1->PointerToRawData,
                                 (PBYTE) MappedFile2 + SectionHeader2->PointerToRawData,
                                 SectionHeader1->SizeOfRawData,
                                 SectionHeader2->SizeOfRawData,
                                 SectName1
                                 );
            }
            else
            {
                DiffCount += MAX( SectionHeader1->SizeOfRawData, SectionHeader2->SizeOfRawData );
            }

            SectionHeader1++;
            SectionHeader2++;
        }

        if ( SectionCount1 != SectionCount2 )
        {
            DiffCount++;    // already reported in PE header diffs
        }

        ReturnValue = DiffCount;

        #undef COMPARE_FIELDS
    }

    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        ErrorExit( GetExceptionCode(), "Exception occurred while comparing PE32 files.\n" );
    }

    return ReturnValue;
}



BOOL
RebaseMappedImage32(
    IN PBYTE               MappedFile,
    IN ULONG               FileSize,
    IN PIMAGE_NT_HEADERS32 NtHeader,
    IN ULONG               NewBase
    )
{
    PIMAGE_BASE_RELOCATION RelocBlock;
    LONG    RelocAmount;
    LONG    RelocDirRemaining;
    ULONG   RelocDirSize;
    PBYTE   RelocBlockMa;
    PBYTE   RelocFixupMa;
    ULONG   RelocCount;
    PUSHORT RelocEntry;
    ULONG   RelocType;
    ULONG   RelocOffset;
    PBYTE   MappedFileEnd;

    MappedFileEnd = MappedFile + FileSize;
    RelocAmount   = NewBase - NtHeader->OptionalHeader.ImageBase;

    if ( RelocAmount == 0 )
    {
        return TRUE;    // same address
    }

    if ( NtHeader->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED )
    {
        return FALSE;       // not relocatable
    }

    NtHeader->OptionalHeader.ImageBase = NewBase;

    RelocBlock = (PIMAGE_BASE_RELOCATION) ImageDirectoryMappedAddress( NtHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC, &RelocDirSize, MappedFile, FileSize );

    //
    //  If no reloc data, but no RELOCS_STRIPPED flag, this indicates
    //  a resource-only or forwarder-only dll that does not require
    //  any relocations to be mapped at other than preferred address.
    //

    if ( RelocBlock )
    {
        RelocDirRemaining = (LONG)RelocDirSize;

        while ( RelocDirRemaining > 0 )
        {
            if ( ( RelocBlock->SizeOfBlock <= (ULONG)RelocDirRemaining ) &&
                 ( RelocBlock->SizeOfBlock > sizeof( IMAGE_BASE_RELOCATION )) )
            {
                RelocBlockMa = MappedFile + ImageRvaToFileOffset( NtHeader, RelocBlock->VirtualAddress );
                RelocEntry   = (PUSHORT)((ULONG)RelocBlock + sizeof( IMAGE_BASE_RELOCATION ));
                RelocCount   = ( RelocBlock->SizeOfBlock - sizeof( IMAGE_BASE_RELOCATION )) / sizeof( USHORT );

                while ( RelocCount-- )
                {
                    RelocOffset  = *RelocEntry & 0x0FFF;
                    RelocType    = *RelocEntry >> 12;
                    RelocFixupMa = RelocBlockMa + RelocOffset;

                    if ( RelocFixupMa < MappedFileEnd )
                    {
                        switch ( RelocType )
                        {
                            case IMAGE_REL_BASED_HIGHLOW:

                                *(LONG UNALIGNED*)RelocFixupMa += RelocAmount;
                                break;

                            case IMAGE_REL_BASED_LOW:

                                *(USHORT UNALIGNED*)RelocFixupMa = (USHORT)( *(SHORT UNALIGNED*)RelocFixupMa + RelocAmount );
                                break;

                            case IMAGE_REL_BASED_HIGH:

                                *(USHORT UNALIGNED*)RelocFixupMa = (USHORT)((( *(USHORT UNALIGNED*)RelocFixupMa << 16 ) + RelocAmount ) >> 16 );
                                break;

                            case IMAGE_REL_BASED_HIGHADJ:

                                ++RelocEntry;
                                --RelocCount;

                                *(USHORT UNALIGNED*)RelocFixupMa = (USHORT)((( *(USHORT UNALIGNED*)RelocFixupMa << 16 ) +
                                                                               *(SHORT UNALIGNED*)RelocEntry + RelocAmount + 0x8000 ) >> 16 );
                                break;

                            case IMAGE_REL_BASED_ABSOLUTE:

                                break;

                            default:

                                ReportDiff(
                                       "Skipping unrecognized relocation type %u for RVA 0x%08X in second file\n",
                                       RelocType,
                                       RelocBlock->VirtualAddress + RelocOffset
                                       );

                                break;

                        }
                    }

                    ++RelocEntry;
                }
            }

            RelocDirRemaining -= RelocBlock->SizeOfBlock;
            RelocBlock = (PIMAGE_BASE_RELOCATION)((ULONG)RelocBlock + RelocBlock->SizeOfBlock );
        }
    }

    return TRUE;
}


VOID
NormalizePe32FileForComparison(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    )
{
    if ( gCompFlags & PECOMP_IGNORE_BINDING_INFO )
    {
        UnbindMappedImage(
            MappedFile,
            FileSize,
            NtHeader
            );

        //
        //  BUGBUG: Not dealing with bound delay imports here because there
        //          is currently no tool that actually binds delay imports.
        //
    }

    SmashLockPrefixesInMappedImage( MappedFile, FileSize, NtHeader );

    NormalizePe32Exports( MappedFile, FileSize, NtHeader );

    NormalizePe32Imports( MappedFile, FileSize, NtHeader );

    NormalizePe32DebugInfo( MappedFile, FileSize, NtHeader );

    NormalizePe32Resources( MappedFile, FileSize, NtHeader );

    NormalizePe32Header( MappedFile, FileSize, NtHeader );
}


VOID
NormalizePe32Header(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    )
{
    FileSize = FileSize;

    PIMAGE_SECTION_HEADER SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    ULONG SectionCount = NtHeader->FileHeader.NumberOfSections;

    while ( SectionCount-- )
    {
        SectionHeader->SizeOfRawData    = ROUNDUP2( SectionHeader->SizeOfRawData,    NtHeader->OptionalHeader.FileAlignment );
        SectionHeader->Misc.VirtualSize = ROUNDUP2( SectionHeader->Misc.VirtualSize, NtHeader->OptionalHeader.SectionAlignment );
        SectionHeader++;
    }

    NtHeader->OptionalHeader.SizeOfHeaders = ROUNDUP2( NtHeader->OptionalHeader.SizeOfHeaders, NtHeader->OptionalHeader.FileAlignment );

    NtHeader->OptionalHeader.CheckSum = 0;

    if ( gCompFlags & PECOMP_IGNORE_TIMESTAMPS )
    {
        NtHeader->FileHeader.TimeDateStamp = 0;
    }

    if ( gCompFlags & PECOMP_IGNORE_VERSION_INFO )
    {
        NtHeader->OptionalHeader.MajorLinkerVersion = 0;
        NtHeader->OptionalHeader.MinorLinkerVersion = 0;
        NtHeader->OptionalHeader.MajorOperatingSystemVersion = 0;
        NtHeader->OptionalHeader.MinorOperatingSystemVersion = 0;
        NtHeader->OptionalHeader.MajorImageVersion = 0;
        NtHeader->OptionalHeader.MinorImageVersion = 0;
        NtHeader->OptionalHeader.MajorSubsystemVersion = 0;
        NtHeader->OptionalHeader.MinorSubsystemVersion = 0;
        NtHeader->OptionalHeader.Win32VersionValue = 0;
    }

    if ( gCompFlags & PECOMP_IGNORE_DOS_HEADER )
    {
        PBYTE pbDosHeader = (PBYTE) MappedFile;
        PBYTE pbNtHeader  = (PBYTE) NtHeader;

        memset( pbDosHeader, 0, ( pbNtHeader - pbDosHeader ));
    }

    if ( gCompFlags & PECOMP_IGNORE_IMAGE_SIZE )
    {
        NtHeader->OptionalHeader.SizeOfCode = 0;
        NtHeader->OptionalHeader.SizeOfInitializedData = 0;
        NtHeader->OptionalHeader.SizeOfUninitializedData = 0;
        NtHeader->OptionalHeader.SizeOfImage = 0;
        NtHeader->OptionalHeader.SizeOfHeaders = 0;
    }
}


BOOL
UnbindMappedImage(
    IN PVOID MappedFile,
    IN ULONG FileSize,
    IN PIMAGE_NT_HEADERS NtHeader
    )
{
    PIMAGE_SECTION_HEADER    SectionHeader;
    ULONG                    SectionCount;
    QWORD                    SectionName;
    PIMAGE_IMPORT_DESCRIPTOR ImportDesc;
    PVOID                    BoundImportDir;
    ULONG                    BoundImportSize;
    PULONG                   OriginalIat;
    PULONG                   BoundIat;
    PBYTE                    MappedFileEnd;
    ULONG                    i;

    MappedFileEnd  = (PBYTE) MappedFile + FileSize;
    BoundImportDir = ImageDirectoryMappedAddress( NtHeader, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, &BoundImportSize, MappedFile, FileSize );

    if ( BoundImportDir )
    {
        //
        //  Zero the bound import directory and pointers to bound
        //  import directory.
        //

        ZeroMemory( BoundImportDir, BoundImportSize );

        NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT ].VirtualAddress = 0;
        NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT ].Size           = 0;
    }

    //
    //  Now walk imports and zero the TimeDate and
    //  ForwarderChain fields.
    //

    ImportDesc = (PIMAGE_IMPORT_DESCRIPTOR) ImageDirectoryMappedAddress( NtHeader, IMAGE_DIRECTORY_ENTRY_IMPORT, NULL, MappedFile, FileSize );

    if ( ImportDesc )
    {
        while ( ((ULONG)ImportDesc < ((ULONG)MappedFileEnd - sizeof( IMAGE_IMPORT_DESCRIPTOR ))) &&
                ( ImportDesc->Characteristics ) )
        {
            if ( ImportDesc->TimeDateStamp )
            {
                //
                //  This is a bound import.  Copy the unbound
                //  IAT over the bound IAT to restore.
                //

                ImportDesc->TimeDateStamp = 0;

                OriginalIat = (PULONG) ImageRvaToMappedAddress( NtHeader, (ULONG)ImportDesc->OriginalFirstThunk, MappedFile, FileSize );
                BoundIat    = (PULONG) ImageRvaToMappedAddress( NtHeader, (ULONG)ImportDesc->FirstThunk,         MappedFile, FileSize );

                if (( OriginalIat ) && ( BoundIat ))
                {
                    while ( ((PBYTE)OriginalIat < MappedFileEnd ) &&
                            ((PBYTE)BoundIat    < MappedFileEnd ) &&
                            ( *OriginalIat ) )
                    {
                        *BoundIat++ = *OriginalIat++;
                    }
                }
            }

            if ( ImportDesc->ForwarderChain )
            {
                ImportDesc->ForwarderChain = 0;
            }

            ++ImportDesc;
        }
    }

    //
    //  The bind utility marks the .idata section as read-only so we want to
    //  change it back to read-write if a separate .idata section exists.
    //

    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ )
    {
        SectionName  = *(QWORD UNALIGNED*)( &SectionHeader[ i ].Name );
        SectionName |= 0x2020202020202020;          // fast lower case

        if ( SectionName == 0x202061746164692E )    // ".idata  "
        {
            if ( ! ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_WRITE ) )
            {
                SectionHeader[ i ].Characteristics |= ( IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE );
            }

            break;
        }
    }

    return TRUE;
}


BOOL
SmashLockPrefixesInMappedImage(
    IN PVOID             MappedFile,
    IN ULONG             FileSize,
    IN PIMAGE_NT_HEADERS NtHeader
    )
{
    PIMAGE_LOAD_CONFIG_DIRECTORY LoadConfig;
    PBYTE* LockPrefixEntry;
    PBYTE  LockPrefixInstruction;

    LoadConfig = (PIMAGE_LOAD_CONFIG_DIRECTORY) ImageDirectoryMappedAddress( NtHeader, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, NULL, MappedFile, FileSize );

    if (( LoadConfig ) && ( LoadConfig->LockPrefixTable ))
    {
        //
        //  The LoadConfig->LockPrefixTable field and
        //  the lock prefix addresses are stored in the
        //  image as image VA, not RVA values.
        //

        LockPrefixEntry = (PBYTE*) ImageVaToMappedAddress( NtHeader, (ULONG)( LoadConfig->LockPrefixTable ), MappedFile, FileSize );

        if ( LockPrefixEntry )
        {
            while ( *LockPrefixEntry )
            {
                LockPrefixInstruction = (PBYTE) ImageVaToMappedAddress( NtHeader, (ULONG)( *LockPrefixEntry ), MappedFile, FileSize );

                if ( LockPrefixInstruction )
                {
                    if ( *LockPrefixInstruction != 0x90 )
                    {
                         *LockPrefixInstruction  = 0x90;
                    }
                }

                ++LockPrefixEntry;
            }
        }
    }

    return TRUE;
}


VOID
NormalizePe32Exports(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    )
{
    ULONG ExportDirRva  = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress;

    PIMAGE_EXPORT_DIRECTORY ExportDir = (PIMAGE_EXPORT_DIRECTORY) ImageRvaToMappedAddress( NtHeader, ExportDirRva, MappedFile, FileSize );

    if ( ExportDir )
    {
        if ( gCompFlags & PECOMP_IGNORE_TIMESTAMPS )
        {
            ExportDir->TimeDateStamp = 0;
        }

        if ( gCompFlags & PECOMP_IGNORE_VERSION_INFO )
        {
            ExportDir->MajorVersion = 0;
            ExportDir->MinorVersion = 0;
        }

        if ( ExportDir->Name != 0 )
        {
            LPSTR MappedName = (LPSTR) ImageRvaToMappedAddress( NtHeader, ExportDir->Name, MappedFile, FileSize );

            if ( MappedName )
            {
                _strlwr( MappedName );
            }
        }
    }
}


VOID
NormalizePe32Imports(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    )
{
    ULONG ImportDescRva = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress;

    PIMAGE_IMPORT_DESCRIPTOR ImportDesc = (PIMAGE_IMPORT_DESCRIPTOR) ImageRvaToMappedAddress( NtHeader, ImportDescRva, MappedFile, FileSize );

    if ( ImportDesc )
    {
        while ( ImportDesc->Characteristics != 0 )
        {
            LPSTR ImportDllName = (LPSTR) ImageRvaToMappedAddress( NtHeader, ImportDesc->Name, MappedFile, FileSize );

            if ( ImportDllName )
            {
                _strlwr( ImportDllName );
            }

            if ( gCompFlags & PECOMP_IGNORE_BINDING_INFO )
            {
                ImportDesc->TimeDateStamp = 0;
            }

            PIMAGE_THUNK_DATA OrgThunk = (PIMAGE_THUNK_DATA) ImageRvaToMappedAddress( NtHeader, ImportDesc->OriginalFirstThunk, MappedFile, FileSize );

            if (( OrgThunk == NULL ) && ( ImportDesc->TimeDateStamp == 0 ))
            {
                // not bound, use IAT
                OrgThunk = (PIMAGE_THUNK_DATA) ImageRvaToMappedAddress( NtHeader, ImportDesc->FirstThunk, MappedFile, FileSize );
            }

            if ( OrgThunk )
            {
                for ( unsigned i = 0; OrgThunk[ i ].u1.Ordinal != 0; i++ )
                {
                    if ( ! IMAGE_SNAP_BY_ORDINAL( OrgThunk[ i ].u1.Ordinal ))
                    {
                        PIMAGE_IMPORT_BY_NAME ImportByName = (PIMAGE_IMPORT_BY_NAME) ImageRvaToMappedAddress( NtHeader, OrgThunk[ i ].u1.AddressOfData, MappedFile, FileSize );

                        if ( ImportByName )
                        {
                            ImportByName->Hint = 0;
                        }
                    }
                }
            }

            ImportDesc++;
        }
    }
}


VOID
NormalizePe32DebugInfo(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    )
{
    ULONG DebugDirectoryRva   = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_DEBUG ].VirtualAddress;
    ULONG DebugDirectorySize  = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_DEBUG ].Size;
    ULONG DebugDirectoryCount = DebugDirectorySize / sizeof( IMAGE_DEBUG_DIRECTORY );

    PIMAGE_DEBUG_DIRECTORY DebugDirectory = (PIMAGE_DEBUG_DIRECTORY) ImageRvaToMappedAddress( NtHeader, DebugDirectoryRva, MappedFile, FileSize );

    if ( DebugDirectory )
    {
        for ( unsigned j = 0; j < DebugDirectoryCount; j++ )
        {
            if ( gCompFlags & PECOMP_IGNORE_TIMESTAMPS )
            {
                DebugDirectory[ j ].TimeDateStamp = 0;
            }

            if ( gCompFlags & PECOMP_IGNORE_VERSION_INFO )
            {
                DebugDirectory[ j ].MajorVersion = 0;
                DebugDirectory[ j ].MinorVersion = 0;
            }

            DebugDirectory[ j ].PointerToRawData = 0;

            PBYTE pbData = (PBYTE) ImageRvaToMappedAddress( NtHeader, DebugDirectory[ j ].AddressOfRawData, MappedFile, FileSize );
            ULONG cbData = DebugDirectory[ j ].SizeOfData;

            if (( pbData ) && ( cbData ))
            {
                memset( pbData, 0, cbData );
            }
        }
    }
}


VOID
NormalizePe32Resources(
    PVOID MappedFile,
    ULONG FileSize,
    PIMAGE_NT_HEADERS32 NtHeader
    )
{
    ULONG ResourceBaseRva    = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_RESOURCE ].VirtualAddress;
    ULONG ResourceTotalSize  = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_RESOURCE ].Size;
    PBYTE ResourceBaseMapped = (PBYTE) ImageRvaToMappedAddress( NtHeader, ResourceBaseRva, MappedFile, FileSize );

    if ( ResourceBaseMapped )
    {
        PIMAGE_SECTION_HEADER ResourceSection = NULL;
        PIMAGE_SECTION_HEADER SectionHeader   = IMAGE_FIRST_SECTION( NtHeader );
        ULONG                 SectionCount    = NtHeader->FileHeader.NumberOfSections;

        while ( SectionCount-- )
        {
            if (( ResourceBaseRva >= SectionHeader->VirtualAddress ) &&
                ( ResourceBaseRva <  SectionHeader->VirtualAddress + SectionHeader->SizeOfRawData ))
            {
                ResourceSection = SectionHeader;
                break;
            }

            SectionHeader++;
        }

        if ( gCompFlags & PECOMP_IGNORE_ALL_RESOURCES )
        {
            if ( ResourceSection )
            {
                PBYTE pbData = (PBYTE)MappedFile + ResourceSection->PointerToRawData;
                ULONG cbData = ResourceSection->SizeOfRawData;

                memset( pbData, 0, cbData );
            }
            else
            {
                memset( ResourceBaseMapped, 0, ResourceTotalSize );
            }
        }
        else
        {
            RESINFO ResInfo;

            ResInfo.ResourceBaseRva    = ResourceBaseRva;
            ResInfo.ResourceBaseMapped = ResourceBaseMapped;
            ResInfo.NtHeader           = NtHeader;
            ResInfo.MappedFile         = MappedFile;
            ResInfo.FileSize           = FileSize;
            ResInfo.Depth              = 0;
            ResInfo.VersionTree        = FALSE;

            PIMAGE_RESOURCE_DIRECTORY RootDir = (PIMAGE_RESOURCE_DIRECTORY) ResourceBaseMapped;

            NormalizePe32ResourceDirectoryTree( RootDir, &ResInfo );

            if ( ResourceSection )
            {
                if ( ResourceSection->SizeOfRawData > ResourceTotalSize )
                {
                    //
                    //  The stupid localization tool inserts non-zero padding
                    //  bytes to fill-out the resource section to FileAlignment
                    //  boundary.  The pattern looks like "PADDINGXXPADDING".
                    //

                    ULONG cbPadding = ResourceSection->SizeOfRawData - ResourceTotalSize;
                    PBYTE pbPadding = (PBYTE)MappedFile + ResourceSection->PointerToRawData + ResourceTotalSize;

                    memset( pbPadding, 0, cbPadding );
                }
            }
        }
    }
}


VOID
__fastcall
NormalizePe32ResourceDirectoryTree(
    PIMAGE_RESOURCE_DIRECTORY ResourceDir,
    RESINFO* ResInfo
    )
{
    ResInfo->Depth++;

    if ( gCompFlags & PECOMP_IGNORE_TIMESTAMPS )
    {
        ResourceDir->TimeDateStamp = 0;
    }

    if ( gCompFlags & PECOMP_IGNORE_VERSION_INFO )
    {
        ResourceDir->MajorVersion = 0;
        ResourceDir->MinorVersion = 0;
    }

    ULONG ResourceCount = ResourceDir->NumberOfNamedEntries
                        + ResourceDir->NumberOfIdEntries;

    PIMAGE_RESOURCE_DIRECTORY_ENTRY ResourceEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)( ResourceDir + 1 );

    while ( ResourceCount-- )
    {
        ULONG OffsetToData = ResourceEntry->OffsetToData & 0x7FFFFFFF;

        if ( ResourceEntry->DataIsDirectory )
        {
            if (( ResourceEntry->Name == (DWORD) RT_VERSION ) && ( ResInfo->Depth == 1  ))
            {
                ResInfo->VersionTree = TRUE;
            }

            NormalizePe32ResourceDirectoryTree(
                (PIMAGE_RESOURCE_DIRECTORY)( ResInfo->ResourceBaseMapped + OffsetToData ),
                ResInfo
                );

            if (( ResourceEntry->Name == (DWORD) RT_VERSION ) && ( ResInfo->Depth == 1  ))
            {
                ResInfo->VersionTree = FALSE;
            }
        }
        else
        {
            PIMAGE_RESOURCE_DATA_ENTRY ResourceData = (PIMAGE_RESOURCE_DATA_ENTRY)( ResInfo->ResourceBaseMapped + OffsetToData );

            if ( gCompFlags & PECOMP_IGNORE_LOCALIZATION )
            {
                ResourceData->CodePage = 0;

                if (( ResInfo->Depth == 3 ) &&
                    ( ! ResourceEntry->NameIsString ) &&
                    ( ResourceEntry->Id >= 0x400 ))
                {
                    ResourceEntry->Id = 0;
                }
            }

            if (( ResInfo->VersionTree ) && ( gCompFlags & PECOMP_IGNORE_VERSION_INFO ))
            {
                //
                //  BUGBUG:  Contrary to what the winnt.h header file says, the
                //           IMAGE_RESOURCE_DATA_ENTRY.OffsetToData field appears
                //           to be an RVA, not offset from beginning of directory.
                //

                ULONG rvaData = ResourceData->OffsetToData;
                PBYTE pbData  = (PBYTE) ImageRvaToMappedAddress( ResInfo->NtHeader, rvaData, ResInfo->MappedFile, ResInfo->FileSize );
                ULONG cbData  = ResourceData->Size;

                if (( pbData ) && ( cbData ))
                {
                    memset( pbData, 0, cbData );
                }

                ResourceData->OffsetToData = 0;
                ResourceData->Size = 0;
            }
        }

        ResourceEntry++;
    }

    ResInfo->Depth--;
}


ULONG
CompareRawBytes(
    PBYTE  pbData1,
    PBYTE  pbData2,
    ULONG  cbData1,
    ULONG  cbData2,
    LPCSTR SectName
    )
{
    ULONG cbDataM = MIN( cbData1, cbData2 );
    ULONG DiffCount = 0;
    ULONG i;

    for ( i = 0; i < cbDataM; i++ )
    {
        if ( pbData1[ i ] != pbData2[ i ] )
        {
            ++DiffCount;
        }
    }

    for ( ; i < cbData1; i++ )
    {
        if ( pbData1[ i ] != 0 )
        {
            ++DiffCount;
        }
    }

    for ( ; i < cbData2; i++ )
    {
        if ( pbData2[ i ] != 0 )
        {
            ++DiffCount;
        }
    }

    if (( DiffCount != 0 ) && ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS ))
    {
        ULONG DisplayCount = 0;

        ReportDiffNewSection();

        for ( i = 0; i < cbDataM; i++ )
        {
            if ( pbData1[ i ] != pbData2[ i ] )
            {
                if ( DisplayCount > MAX_DIFFS_DISPLAYED )
                {
                    break;
                }

                DisplayCount++;

                ReportDiff(
                    "%-8s (%05X):  %02X %02X  %c %c\n",
                    SectName,
                    i,
                    pbData1[ i ],
                    pbData2[ i ],
                    ((signed char)pbData1[ i ] > 0x20 ) ? pbData1[ i ] : ' ',
                    ((signed char)pbData2[ i ] > 0x20 ) ? pbData2[ i ] : ' '
                    );
            }
        }

        for ( ; i < cbData1; i++ )
        {
            if ( pbData1[ i ] != 0 )
            {
                if ( DisplayCount > MAX_DIFFS_DISPLAYED )
                {
                    break;
                }

                DisplayCount++;

                ReportDiff(
                    "%-8s (%05X):  %02X %02X  %c %c\n",
                    SectName,
                    i,
                    pbData1[ i ],
                    0,
                    ((signed char)pbData1[ i ] > 0x20 ) ? pbData1[ i ] : ' ',
                    ' '
                    );

            }
        }

        for ( ; i < cbData2; i++ )
        {
            if ( pbData2[ i ] != 0 )
            {
                if ( DisplayCount > MAX_DIFFS_DISPLAYED )
                {
                    break;
                }

                DisplayCount++;

                ReportDiff(
                    "%-8s (%05X):  %02X %02X  %c %c\n",
                    SectName,
                    i,
                    0,
                    pbData2[ i ],
                    ' ',
                    ((signed char)pbData2[ i ] > 0x20 ) ? pbData2[ i ] : ' '
                    );
            }
        }

        if ( DiffCount > DisplayCount )
        {
            ReportDiffNewSection();

            ReportDiff(
                "%-8s %u more differences not shown\n",
                SectName,
                DiffCount - DisplayCount
                );
        }
    }

    if ( DiffCount != 0 )
    {
        ReportDiffNewSection();
    }

    return DiffCount;
}


ULONG
CompareFields32(
    DWORD  Value1,
    DWORD  Value2,
    LPCSTR FieldName,
    LPCSTR SectName
    )
{
    if ( Value1 != Value2 )
    {
        if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
        {
            ReportDiff(
                "%-8s->%-45s %08X != %08X\n",
                SectName,
                FieldName,
                Value1,
                Value2
                );
        }

        return 1;
    }

    return 0;
}


VOID __fastcall StripTrailingSpaces( LPSTR String )
{
    LPSTR p = String + strlen( String ) - 1;

    while (( p >= String ) && ( *p == ' ' ))
    {
        *p-- = 0;
    }
}


BOOL
__fastcall
IsMatchingResourceString(
    const IMAGE_RESOURCE_DIR_STRING_U *String1,
    const IMAGE_RESOURCE_DIR_STRING_U *String2
    )
{
    if ( String1->Length != String2->Length )
    {
        return FALSE;
    }

    unsigned Length = String1->Length;

    for ( unsigned i = 0; i < Length; i++ )
    {
        if ( String1->NameString[ i ] != String2->NameString[ i ] )
        {
            return FALSE;
        }

    }

    return TRUE;
}


const LPCSTR ResTypeToName[] =                  //  taken from winuser.h
                {
                    NULL,                       //  0
                    "RT_CURSOR",                //  1
                    "RT_BITMAP",                //  2
                    "RT_ICON",                  //  3
                    "RT_MENU",                  //  4
                    "RT_DIALOG",                //  5
                    "RT_STRING",                //  6
                    "RT_FONTDIR",               //  7
                    "RT_FONT",                  //  8
                    "RT_ACCELERATOR",           //  9
                    "RT_RCDATA",                // 10
                    "RT_MESSAGETABLE",          // 11
                    "RT_GROUP_CURSOR",          // 12
                    NULL,                       // 13
                    "RT_GROUP_ICON",            // 14
                    NULL,                       // 15
                    "RT_VERSION",               // 16
                    "RT_DLGINCLUDE",            // 17
                    NULL,                       // 18
                    "RT_PLUGPLAY",              // 19
                    "RT_VXD",                   // 20
                    "RT_ANICURSOR",             // 21
                    "RT_ANIICON",               // 22
                    "RT_HTML",                  // 23
                    "RT_MANIFEST",              // 24
                    NULL                        // 25
                };


LPSTR
GetResourceEntryName(
    IN  PIMAGE_RESOURCE_DIRECTORY_ENTRY ResEntry,
    IN  PBYTE ResBase,
    IN  ULONG Depth,
    OUT LPSTR Buffer
    )
{
    if ( ResEntry->NameIsString )
    {
        PIMAGE_RESOURCE_DIR_STRING_U ResNameU = (PIMAGE_RESOURCE_DIR_STRING_U)( ResBase + ResEntry->NameOffset );

        strcpy( Buffer, "???" );    // in case translation failure

        int cbActual = WideCharToMultiByte(
                           CP_OEMCP,
                           0,
                           ResNameU->NameString,
                           ResNameU->Length,
                           Buffer,
                           0xFFFF,
                           NULL,
                           NULL
                           );

        Buffer[ cbActual ] = 0;     // force termination
    }
    else
    {
        if (( Depth == 1 ) &&
            ( ResEntry->Id < countof( ResTypeToName )) &&
            ( ResTypeToName[ ResEntry->Id ] != NULL ))
        {
            strcpy( Buffer, ResTypeToName[ ResEntry->Id ] );
        }
        else if (( Depth == 3 ) &&
                 ( ! ResEntry->DataIsDirectory ) &&
                 ( ResEntry->Id >= 0x400 ))
        {
            DWORD n = VerLanguageName( ResEntry->Id, Buffer + 1, 0xFFFF );
            Buffer[ 0     ] = '[';
            Buffer[ n + 1 ] = ']';
            Buffer[ n + 2 ] = 0;
        }
        else
        {
            sprintf( Buffer, "#%u", ResEntry->Id );
        }
    }

    return Buffer;
}





ULONG
__fastcall
ComparePe32ResourceDirectoryTree(
    PIMAGE_RESOURCE_DIRECTORY ResourceDir1,
    PIMAGE_RESOURCE_DIRECTORY ResourceDir2,
    RESINFO2* ResInfo
    )
{
    ResInfo->Depth++;

    ULONG DiffCount = 0;
    LPSTR DisplayBufPtr = NULL;

    if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
    {
        DisplayBufPtr = ResInfo->DisplayBuffer + strlen( ResInfo->DisplayBuffer );
    }

    #define COMPARE_FIELDS( p1, p2, field ) \
        CompareFields32( (p1)->field, (p2)->field, #field, ResInfo->DisplayBuffer );

    DiffCount += COMPARE_FIELDS( ResourceDir1, ResourceDir2, Characteristics );
    DiffCount += COMPARE_FIELDS( ResourceDir1, ResourceDir2, TimeDateStamp );
    DiffCount += COMPARE_FIELDS( ResourceDir1, ResourceDir2, MajorVersion );
    DiffCount += COMPARE_FIELDS( ResourceDir1, ResourceDir2, MinorVersion );
    DiffCount += COMPARE_FIELDS( ResourceDir1, ResourceDir2, NumberOfNamedEntries );
    DiffCount += COMPARE_FIELDS( ResourceDir1, ResourceDir2, NumberOfIdEntries );

    PIMAGE_RESOURCE_DIRECTORY_ENTRY ResourceEntry1 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)( ResourceDir1 + 1 );
    PIMAGE_RESOURCE_DIRECTORY_ENTRY ResourceEntry2 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)( ResourceDir2 + 1 );

    ULONG ResourceCount1 = ResourceDir1->NumberOfNamedEntries
                         + ResourceDir1->NumberOfIdEntries;

    ULONG ResourceCount2 = ResourceDir2->NumberOfNamedEntries
                         + ResourceDir2->NumberOfIdEntries;

    for ( unsigned i = 0; i < ResourceCount1; i++ )
    {
        PIMAGE_RESOURCE_DIR_STRING_U ResName1 = NULL;

        if ( ResourceEntry1[ i ].NameIsString )
        {
            ResName1 = (PIMAGE_RESOURCE_DIR_STRING_U)( ResInfo->ResourceBaseMapped1 + ResourceEntry1[ i ].NameOffset );
        }

        if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
        {
            DisplayBufPtr[ 0 ] = '\\';
            DisplayBufPtr[ 1 ] = '\0';

            GetResourceEntryName( &ResourceEntry1[ i ], ResInfo->ResourceBaseMapped1, ResInfo->Depth, DisplayBufPtr + 1 );
        }

        bool Match = false;

        for ( unsigned j = 0; j < ResourceCount2; j++ )
        {
            if ( ResourceEntry2[ j ].Name != 0xFFFFFFFF )
            {
                if ( ResourceEntry2[ j ].NameIsString )
                {
                    if ( ResName1 != NULL )
                    {
                        PIMAGE_RESOURCE_DIR_STRING_U ResName2 = (PIMAGE_RESOURCE_DIR_STRING_U)( ResInfo->ResourceBaseMapped2 + ResourceEntry2[ j ].NameOffset );

                        if ( IsMatchingResourceString( ResName1, ResName2 ))
                        {
                            Match = true;
                            break;
                        }
                    }
                }
                else
                {
                    if ( ResName1 == NULL )
                    {
                        if ( ResourceEntry1[ i ].Id == ResourceEntry2[ j ].Id )
                        {
                            Match = true;
                            break;
                        }
                    }
                }
            }
        }

        if ( Match )
        {
            DiffCount += COMPARE_FIELDS( &ResourceEntry1[ i ], &ResourceEntry2[ j ], DataIsDirectory );

            if (( ResourceEntry1[ i ].DataIsDirectory ) &&
                ( ResourceEntry2[ j ].DataIsDirectory ))
            {
                PIMAGE_RESOURCE_DIRECTORY ChildDir1 = (PIMAGE_RESOURCE_DIRECTORY)( ResInfo->ResourceBaseMapped1 + ResourceEntry1[ i ].OffsetToDirectory );
                PIMAGE_RESOURCE_DIRECTORY ChildDir2 = (PIMAGE_RESOURCE_DIRECTORY)( ResInfo->ResourceBaseMapped2 + ResourceEntry2[ j ].OffsetToDirectory );

                DiffCount += ComparePe32ResourceDirectoryTree(
                                 ChildDir1,
                                 ChildDir2,
                                 ResInfo
                                 );
            }

            else if (( ! ResourceEntry1[ i ].DataIsDirectory ) &&
                     ( ! ResourceEntry2[ j ].DataIsDirectory ))
            {
                if ( ! ( gCompFlags & PECOMP_IGNORE_LOCALIZATION ))
                {
                    PIMAGE_RESOURCE_DATA_ENTRY ResData1 = (PIMAGE_RESOURCE_DATA_ENTRY)( ResInfo->ResourceBaseMapped1 + ResourceEntry1[ i ].OffsetToData );
                    PIMAGE_RESOURCE_DATA_ENTRY ResData2 = (PIMAGE_RESOURCE_DATA_ENTRY)( ResInfo->ResourceBaseMapped2 + ResourceEntry2[ j ].OffsetToData );

                    DiffCount += COMPARE_FIELDS( ResData1, ResData2, Size );
                    DiffCount += COMPARE_FIELDS( ResData1, ResData2, CodePage );
                    DiffCount += COMPARE_FIELDS( ResData1, ResData2, Reserved );

                    ULONG rvaData1 = ResData1->OffsetToData;
                    ULONG  cbData1 = ResData1->Size;
                    PBYTE  pbData1 = (PBYTE) ImageRvaToMappedAddress( ResInfo->NtHeader1, rvaData1, ResInfo->MappedFile1, ResInfo->FileSize1 );

                    ULONG rvaData2 = ResData2->OffsetToData;
                    ULONG  cbData2 = ResData2->Size;
                    PBYTE  pbData2 = (PBYTE) ImageRvaToMappedAddress( ResInfo->NtHeader2, rvaData2, ResInfo->MappedFile2, ResInfo->FileSize2 );

                    ZeroIgnoreValuesInBufferPair( pbData1, pbData2, cbData1, cbData2 );

                    DiffCount += CompareRawBytes( pbData1, pbData2, cbData1, cbData2, ResInfo->DisplayBuffer );
                }
            }

            ResourceEntry2[ j ].Name = 0xFFFFFFFF;
        }
        else    // no match
        {
            ++DiffCount;

            if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
            {
                ReportDiff( "%-8s: left only\n", ResInfo->DisplayBuffer );
            }
        }
    }

    for ( unsigned j = 0; j < ResourceCount2; j++ )
    {
        if ( ResourceEntry2[ j ].Name != 0xFFFFFFFF )
        {
            ++DiffCount;

            if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
            {
                DisplayBufPtr[ 0 ] = '\\';
                DisplayBufPtr[ 1 ] = '\0';

                GetResourceEntryName( &ResourceEntry2[ j ], ResInfo->ResourceBaseMapped2, ResInfo->Depth, DisplayBufPtr + 1 );

                ReportDiff( "%-8s: right only\n", ResInfo->DisplayBuffer );
            }
        }
    }

    if ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )
    {
        *DisplayBufPtr = 0;     // restore display name for parent directory
    }

    #undef COMPARE_FIELDS

    ResInfo->Depth--;

    return DiffCount;
}


LPSTR
ErrorTextFromErrorCode(
    IN  DWORD ErrorCode,
    OUT LPSTR ErrorText
    )
{
    ULONG n = sprintf( ErrorText, ((LONG)ErrorCode > 0 ) ? "(%u) " : "(%X) ", ErrorCode );

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        ErrorCode,
        0,
        ErrorText + n,
        0xFFFF,
        NULL
        );

    for ( PBYTE p = (PBYTE) ErrorText; *p; p++ )
    {
        if ( *p < ' ' )
        {
             *p = ' ';
        }
    }

    StripTrailingSpaces( ErrorText );

    return ErrorText;
}


VOID
NORETURN
ErrorExitV(
    DWORD   dwGLE,
    LPCSTR  pszFormat,
    va_list vaArgs
    )
{
    if ( ! ( gCompFlags & PECOMP_SILENT ))
    {
        if ( dwGLE == GLE )
        {
             dwGLE = GetLastError();
        }

        fprintf( stdout, "ERROR: Unable to perform comparison.\n" );
        fflush( stdout );

        vfprintf( stderr, pszFormat, vaArgs );

        if ( dwGLE != 0 )
        {
            CHAR Buffer[ 1024 ];
            fprintf( stderr, "%s\n", ErrorTextFromErrorCode( dwGLE, Buffer ));
        }

        fflush( stderr );
    }

    exit( 999999999 );
}


VOID
NORETURN
ErrorExit(
    DWORD  dwGLE,
    LPCSTR pszFormat,
    ...
    )
{
    va_list vaArgs;
    va_start( vaArgs, pszFormat );
    ErrorExitV( dwGLE, pszFormat, vaArgs );
    //va_end( vaArgs );
}


VOID
ReportDiffV(
    LPCSTR  pszFormat,
    va_list vaArgs
    )
{
    if (( gCompFlags & PECOMP_SILENT ) ||
        ( ! ( gCompFlags & PECOMP_SHOW_PRIMARY_DIFFS )))
    {
        return;
    }

    if ( ! gDiffSectionCount++ )
    {
        fprintf( stderr, "\n" );
    }

    vfprintf( stderr, pszFormat, vaArgs );
}


VOID
ReportDiff(
    LPCSTR pszFormat,
    ...
    )
{
    va_list vaArgs;
    va_start( vaArgs, pszFormat );
    ReportDiffV( pszFormat, vaArgs );
    va_end( vaArgs );
}



