
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#ifndef WIN32
#define WIN32 0x0400
#endif

#pragma warning( disable: 4001 4035 4115 4200 4201 4204 4209 4214 4514 4699 )
#include <windows.h>
#include <wincrypt.h>
#pragma warning( disable: 4201 )
#include <imagehlp.h>
#pragma warning( disable: 4001 4035 4115 4200 4201 4204 4209 4214 4514 4699 )

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "patchapi.h"
#include "patchprv.h"
#include "patchlzx.h"
#include "md5.h"
#include "misc.h"
#include "redblack.h"

typedef unsigned __int64 QWORD;

CHAR TextBuffer[ 65000 ];

CHAR OldDirRoot[ MAX_PATH ];
CHAR NewDirRoot[ MAX_PATH ];
CHAR PatchDirRoot[ MAX_PATH ];
CHAR OldPsymPath[ MAX_PATH ];
CHAR NewPsymPath[ MAX_PATH ];
CHAR OldSymPath[ MAX_PATH ];
CHAR NewSymPath[ MAX_PATH ];
CHAR NewFileList[ MAX_PATH ];
CHAR *NewDirRootEnd;
CHAR *OldDirRootEnd;
CHAR *PatchDirRootEnd;
CHAR PatchCache[ MAX_PATH ];
CHAR *PatchCacheEnd;
BOOL bIncremental;
BOOL bSubDirectories;
BOOL bCopyNonPatches;
BOOL bNoSymWarn;
BOOL bNoAffinity;
BOOL bUseBadSyms;
BOOL bCrcName;
BOOL bSourceIsForest;
BOOL bCaching;
BOOL bCreateFallbackFiles;


#define HASH_HEX_LENGTH ( sizeof( MD5_HASH ) * 2 + 1 )

typedef struct _a_NEWFILE NEWFILE;

struct _a_NEWFILE {
    NEWFILE     * Next;         // link to next with same base filename.ext
    NAME_NODE   * FileNode;     // ptr to parent node in NewFileTree
    NAME_NODE   * MD5Node;      // ptr to this file's node in MD5Tree
    FILETIME      FileTime;     // last modified time
    DWORD         FileSize;     // size of file
    char          Path[ 1 ];    // relative path w/trailing sep, ie "uniproc\"
    };

typedef struct _a_OLDFILE OLDFILE;

struct _a_OLDFILE {
    OLDFILE * Next;     // link to next with same base filename.ext
    char Path[ 1 ];     // relative path w/trailing sep, ie "uniproc\"
    };

typedef struct _a_SORTNODE SORTNODE;

struct _a_SORTNODE {
    NEWFILE   * NewFile;
    NAME_NODE * Node;
    };

typedef struct _a_PRODUCTFILE PRODUCTFILE;

struct _a_PRODUCTFILE {
    NEWFILE * FirstNewFile;     // link to first new file with this base filename.ext
    OLDFILE * FirstOldFile;     // link to first old file with this base filename.ext
    };

typedef struct _a_PathNode {
    struct _a_PathNode *Parent;     // ptr to parent PATHNODE, or NULL if at top
    CHAR                Name[ 1 ];  // path component at this level w/trail, ie "system32\"
    } PATHNODE;

typedef struct _a_NameNode {
    PATHNODE           *Parent;     // ptr to parent PATHNODE
    struct _a_NameNode *Next;       // link to next with same hash
    DWORD               HashValue;  // full hash value (before modulo)
    MD5_HASH            Digest;     // MD5 of this file, all 0 if unknown
    CHAR               *BaseName;   // ptr into Name, ie, "kernel32.dll"
    CHAR                Name[ 1 ];  // path rel to parent PATHNODE or just base name
    } NAMENODE;

#define     MANIFEST_DATA_FILE_NAME     "MANIFEST.DAT"
#define     MANIFEST_TEMP_FILE_NAME     "MANIFEST.$$$"

/* manifest.dat file record format */

#include <pshpack1.h>

typedef unsigned char DIGEST[16];

typedef struct
{
    unsigned long       dwSignature;    /* should be 'M','S','M','F' */
    unsigned long       dwVersion;      /* should be = 1 */
    unsigned long       cTotalFiles;    /* number of files */
    unsigned __int64    cbTotalSize;    /* total size of all files */
} MANIFEST_HEADER;

#define MANIFEST_SIGNATURE      (0x464D534D)
#define MANIFEST_VERSION        (0x00000001)

typedef struct
{
    unsigned short      cchFileName;    /* length of file name w/o NUL */
    unsigned long       cbFileSize;     /* size of this file */
    DIGEST              digest;         /* digest of this file */
/*  char                szFileName[cchFileName];  name of this file */
} MANIFEST_RECORD;

#include <poppack.h>


#define HASH_TABLE_WIDTH (10000)

PATHNODE *OldDirTree;
DWORD NameHash;
NAMENODE *NameNode;
NAMENODE * HashTable[ HASH_TABLE_WIDTH ];

LPSTR OldSymPathArray[ 1 ] = { OldSymPath };

LPSTR FileNameArray[] = { NewDirRoot, OldDirRoot };

PATCH_SYMLOAD_CALLBACK MySymLoadCallback;

PATCH_OPTION_DATA OptionData = {
    sizeof( PATCH_OPTION_DATA ),
    0,
    (LPCSTR)NewSymPath,
    (LPCSTR*)OldSymPathArray,
    0,
    MySymLoadCallback,
    FileNameArray
    };

ULONG OptionFlags = PATCH_OPTION_USE_LZX_BEST  |
                    PATCH_OPTION_USE_LZX_LARGE |
                    PATCH_OPTION_INTERLEAVE_FILES;

QWORD TotalPatchSourceSizes;
QWORD TotalPatchTargetSizes;
ULONG CountPatchFiles;

ULONG CountCopiedFiles;
QWORD TotalCopiedSizes;
ULONG ReturnCode;

ULONG ErrorCount;

HANDLE SubAllocator;
ULONG CountNewFiles;
NAME_TREE NewFileTree;
NAME_TREE MD5Tree;

const char szZeroLengthFileHash[] = "D41D8CD98F00B204E9800998ECF8427E";

#pragma data_seg( ".shared" )
LONG SharedInstanceCounter = 0;
#pragma data_seg()

LONG WINAPI _InterlockedExchangeAdd( LONG*, LONG );
#pragma intrinsic( _InterlockedExchangeAdd )


BOOL
CALLBACK
MySymLoadCallback(
    IN ULONG  WhichFile,
    IN LPCSTR SymbolFileName,
    IN ULONG  SymType,
    IN ULONG  SymbolFileCheckSum,
    IN ULONG  SymbolFileTimeDate,
    IN ULONG  ImageFileCheckSum,
    IN ULONG  ImageFileTimeDate,
    IN PVOID  CallbackContext
    );

DWORD
HashString(
    LPCSTR String
    );

BOOL
GetFirstCandidate(
    LPSTR  OldDirRoot,
    LPSTR  OldDirFileName,
    LPCSTR NewFileName,
    CHAR  *CandidateHash
    );

BOOL
GetNextCandidate(
    LPSTR  OldDirRoot,
    LPSTR  OldDirFileName,
    LPCSTR NewFileName,
    CHAR  *CandidateHash
    );

VOID
BuildOldDirTree(
    LPSTR OldDirRoot
    );

VOID
RecursiveBuildOldDirTree(
    LPSTR OldDirPath,
    PATHNODE *Parent
    );

BOOL
LoadTreeUsingManifest(
    LPSTR RootPath,
    LPSTR RootPathEnd,
    PATHNODE * Parent
    );

VOID
BuildDirPath(
    LPSTR Buffer,
    PATHNODE *Parent
    );

VOID
AssociateNewFileDuplicates(
    VOID
    );

ULONG
MakePatchesForFile(
    LPCSTR   NewDirFileName,    // base file name in NewDirRoot, ie "kernel32.dll" in "new\uniproc\kernel32.dll"
    LPSTR    OldDirFileName,    // pos for base file name in OldDirRoot, ie "x" in "old\uniproc\x"
                                // or top base if bSourceIsForest, ie "x" in "old\x", not drilled down
    LPSTR    PatchDirFileName,  // pos for base file name in PatchDirRoot, ie "x" in "patches\uniproc\x"
    FILETIME NewFileLastWriteTime,
    DWORD    NewFileSize
    );

BOOL
AdjustPatchTime(
    LPCSTR   PatchPathName,     // full path to patch file
    FILETIME PatchTimeStamp     // time to store in patch
    );

void CopyRight( void ) {
    printf(
        "\n"
        "MPATCHES " PATCH_VERSION_TEXT " Patch Creation Utility\n"
        "Copyright (C) Microsoft, 1997-2001\n"
        "\n"
        );
    }


void __declspec( noreturn ) Usage( void ) {
    printf(
"Usage:  MPATCHES [options] OldFilesPath NewFilesPath PatchFilesPath\n"
"\n"
"        For each file in the NewFilesPath directory,\n"
"          If same named file exists in OldFilesPath directory,\n"
"            Create patch file in PatchFilesPath directory,\n"
"          Else if -COPYOTHERS was specified,\n"
"            Copy file from NewFilesPath to PatchFilesPath.\n"
"\n"
"        Options:\n"
"\n"
"          -NOBINDFIX   Turn off automatic compensation for bound imports in\n"
"                       the the old file.  The default is to ignore binding\n"
"                       data in the old file during patch creation which will\n"
"                       cause the application of the patch to succeed whether\n"
"                       or not the old file on the target machine is bound, not\n"
"                       bound, or even bound to different import addresses.\n"
"                       If the files are not Win32 binaries, this option is\n"
"                       ignored and has no effect.\n"
"\n"
"          -NOLOCKFIX   Turn off automatic compensation for smashed lock prefix\n"
"                       instructions.  If the files are not Win32 binaries,\n"
"                       this option is ignored and has no effect.\n"
"\n"
"          -NOREBASE    Turn off automatic internal rebasing of old file to new\n"
"                       file's image base address.  If the files are not Win32\n"
"                       binaries, this option is ignored and has no effect.\n"
"\n"
"          -NORESTIME   Turn off automatic fixup of resource section timestamps\n"
"                       (ignored if not Win32 binaries).\n"
"\n"
"          -NOCHECKSUM  Enable automatic compensation for old file's checksums.\n"
"\n"
"          -NOSTORETIME Don't store the timestamp of the new file in the patch\n"
"                       file.  Instead, set the timestamp of the patch file to\n"
"                       the timestamp of the new file.\n"
"\n"
"          -FAILBIGGER  If patch file is bigger than simple compressed file,\n"
"                       don't create the patch file (takes longer).\n"
"\n"
"          -NEWSYMPATH:PathName[;PathName]\n"
"\n"
"                       For files in NewFilesPath, search for symbol file(s) in\n"
"                       this path location (recursive search each path).  If\n"
"                       not specified, the default is the same directory as the\n"
"                       NewFile.\n"
"\n"
"          -OLDSYMPATH:PathName[;PathName]\n"
"\n"
"                       For files in OldFilesPath, search for symbol file(s) in\n"
"                       this path location (recursive search each path).  If\n"
"                       not specified, the default is the same directory as the\n"
"                       OldFile.\n"
"\n"
"          -NOSYMS      Don't use debug symbol files when creating the patch.\n"
"\n"
"          -NOSYMFAIL   Don't fail to create patch if symbols cannot be loaded.\n"
"\n"
"          -NOSYMWARN   Don't warn if symbols can't be found or don't match the\n"
"                       corresponding file (symbol checksum mismatch).\n"
"\n"
"          -USEBADSYMS  Rather than ignoring symbols if the checksums don't\n"
"                       match the corresponding files, use the bad symbols.\n"
"\n"
"          -E8          Force E8 call translation for x86 binaries.\n"
"\n"
"          -NOE8        Force no E8 call translation for x86 binaries.\n"
"\n"
"                       If neither -E8 or -NOE8 are specified, and the files\n"
"                       are x86 binaries, the patch will be built internally\n"
"                       twice and the smaller will be chosen for output.\n"
"\n"
"          -COPYOTHERS  For files in NewFilesPath for which no corresponding\n"
"                       file exists in OldFilesPath, copy the new file to the\n"
"                       PatchFilesPath directory.  If -FAILBIGGER is also\n"
"                       specified, this will cause the new file to be copied to\n"
"                       the patch directory if the patch file would be larger\n"
"                       than simply compressing the new file.\n"
"\n"
"          -SUBDIRS     Process subdirectories too.\n"
"\n"
"          -FOREST      Use OldFilesPath as a forest of potentially-useful old\n"
"                       files, and create a patch from every file found there\n"
"                       with the same base name as the new file.  OldFilesPath\n"
"                       is searched recursively for each base name.\n"
"\n"
"          -FILES:list  The specified list is a text file containing a list of\n"
"                       the files in NewFilesPath that are actually needed.\n"
"                       By default, patches are created for all files found in\n"
"                       NewFilesPath.  A list contains one file name per line.\n"
"\n"
"          -FALLBACKS   Create fallback files: null patches or uncompressed,\n"
"                       whichever is smaller.\n"
"\n"
"          -CACHE:path  Use the specified path as a cache of patches created.\n"
"                       The cache is checked for each patch is created, and\n"
"                       and patches which are created are added to the cache.\n"
"                       Using a cache can substantially reduce the amount of\n"
"                       time required for subsequent MPATCHES sessions.\n"
"                       By default, no cache is created or used.  The cache is\n"
"                       organized for MPATCHES use only.\n"
"\n"
"          -CHANGED     Only create patches if existing patch file has an older\n"
"                       date/time stamp as the corresponding new file.  Used to\n"
"                       incrementally updating patches for only those files\n"
"                       that changed.\n"
"\n"
"          -CRCNAME     Name patch files with CRC of normalized old file as\n"
"                       part of the filename (like foo.dll.e75a3b7d._p).\n"
"\n"
"                       Be careful using -CRCNAME together with -CHANGED.  When\n"
"                       these are used together, all files in the target patch\n"
"                       directory matching foo.dll.*._p will be deleted other\n"
"                       than the exact patch file name.  In other words, don't\n"
"                       use the same target patch directory to build patches\n"
"                       from more than one set of old files.\n"
"\n"
"          -WINDOW:nn   Limit the LZX compression memory window to 2^nn bytes\n"
"                       (nn is a power of two >= 17: 128KB).  Patch creation\n"
"                       memory requirement may be 10X this amount, but patch\n"
"                       applier will only this amount of memory plus the size\n"
"                       of the output file (available in version 1.99+).\n"
"\n"
"          -MSPATCHA_WIN2000  Assure the patch file can be applied with version\n"
"                       1.94 of MSPATCHA.DLL (Windows 2000).  May increase size\n"
"                       of patch file if old and new file sizes combined are\n"
"                       larger than 8MB.\n"
"\n"
"          -MSPATCHA_WINXP    Assure the patch file can be used with version\n"
"                       5.1 of MSPATCHA.DLL (Windows XP).  May increase size\n"
"                       of patch file if old and new file sizes combined are\n"
"                       larger than 32MB.\n"
"\n"
"          MPATCHES will also look for environment variables named \"MPATCHES\"\n"
"          followed by an underscore and the name of the option.  Command line\n"
"          specified options override environment variable options.  Examples:\n"
"\n"
"              MPATCHES_NOSTORETIME=1\n"
"              MPATCHES_NEWSYMPATH=c:\\winnt\\symbols;\\\\server\\share\\symbols\n"
"\n"

        );
    exit( 1 );
    }


DWORDLONG
GetFileSizeByName(
    IN LPCSTR FileName
    )
    {
    DWORDLONG FileSizeReturn;
    ULONG     FileSizeHigh;
    ULONG     FileSizeLow;
    HANDLE    hFile;

    FileSizeReturn = 0xFFFFFFFFFFFFFFFF;

    hFile = CreateFile(
                FileName,
                GENERIC_READ,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
                );

    if ( hFile != INVALID_HANDLE_VALUE ) {

        FileSizeLow = GetFileSize( hFile, &FileSizeHigh );

        if (( FileSizeLow != 0xFFFFFFFF ) || ( GetLastError() == NO_ERROR )) {

            FileSizeReturn = ((DWORDLONG)FileSizeHigh << 32 ) | FileSizeLow;
            }

        CloseHandle( hFile );
        }

    return FileSizeReturn;
    }


BOOL
IsDirectory(
    LPCSTR PathName
    )
{
    DWORD Attributes;

    Attributes = GetFileAttributes( PathName );

    if ( Attributes == 0xFFFFFFFF ) {

        return FALSE;
        }

    if ( Attributes & FILE_ATTRIBUTE_DIRECTORY ) {

        return TRUE;
        }

    return FALSE;
}


BOOL
IsFile(
    LPCSTR PathName
    )
{
    DWORD Attributes;

    Attributes = GetFileAttributes( PathName );

    if ( Attributes == 0xFFFFFFFF ) {

        return FALSE;
        }

    if ( Attributes & FILE_ATTRIBUTE_DIRECTORY ) {

        return FALSE;
        }

    return TRUE;
}


BOOL
CreateNewDirectoryForFile(
    LPSTR FileName
    )
{
    BOOL Result = FALSE;
    CHAR *p = strrchr( FileName, '\\' );

    if ( p == NULL ) {
        return FALSE;
        }

    *p = '\0';

    if ( GetFileAttributes( FileName ) == 0xFFFFFFFF ) {

        switch ( GetLastError() ) {

        case ERROR_FILE_NOT_FOUND:
            Result = CreateDirectory( FileName, NULL );
            break;

        case ERROR_PATH_NOT_FOUND:
            Result = CreateNewDirectoryForFile( FileName ) &&
                     CreateDirectory( FileName, NULL );
            break;
            }
        }

    *p = '\\';

    return( Result );
}


BOOL
GetMpatchEnvironVar(
    IN  LPCSTR VarName,
    OUT LPSTR  Buffer
    )
    {
    CHAR LocalBuffer[ 256 ];
    CHAR EnvironName[ 256 ];
    LPSTR EnvironValue;

    EnvironValue = Buffer ? Buffer : LocalBuffer;

    sprintf( EnvironName, "mpatches_%s", VarName );

    if ( GetEnvironmentVariable( EnvironName, EnvironValue, 65500 )) {

        if ( Buffer == NULL ) {

            if (( *LocalBuffer == '0' ) && ( strtoul( LocalBuffer, NULL, 0 ) == 0 )) {
                return FALSE;
                }
            }

        return TRUE;
        }

    return FALSE;
    }


VOID
StripTrailingBackslash(
    IN LPSTR PathName
    )
    {
    LPSTR p;

    if (( PathName[ 0 ] != '\0' ) &&
        ( PathName[ 1 ] == ':'  ) &&
        ( PathName[ 2 ] == '\\' ) &&
        ( PathName[ 3 ] == '\0' )) {

        return;
        }

    p = PathName + strlen( PathName );

    while (( p > PathName ) && ( *( --p ) == '\\' )) {
        *p = 0;
        }
    }


VOID
AppendTrailingBackslash(
    IN LPSTR PathName
    )
    {
    LPSTR p;

    p = PathName + strlen( PathName );

    if (( p > PathName ) && ( *( p - 1 ) != '\\' )) {
        *p++ = '\\';
        *p = 0;
        }
    }


BOOL
CALLBACK
MySymLoadCallback(
    IN ULONG  WhichFile,
    IN LPCSTR SymbolFileName,
    IN ULONG  SymType,
    IN ULONG  SymbolFileCheckSum,
    IN ULONG  SymbolFileTimeDate,
    IN ULONG  ImageFileCheckSum,
    IN ULONG  ImageFileTimeDate,
    IN PVOID  CallbackContext
    )
    {
    LPCSTR *FileNameArray = CallbackContext;
    LPCSTR SymTypeText;

    if (( SymType == SymNone ) || ( SymType == SymExport )) {

        //
        //  Symbols could not be found.
        //

        if ( ! bNoSymWarn ) {

            printf(
                "\n"
                "WARNING: no debug symbols for %s\n\n",
                FileNameArray[ WhichFile ]
                );
            }

        return TRUE;
        }

    //
    //  Note that the Old file checksum is the checksum AFTER normalization,
    //  so if the original .dbg file was updated with bound checksum, the
    //  old file's checksum will not match the symbol file's checksum.  But,
    //  binding a file does not change its TimeDateStamp, so that should be
    //  a valid comparison.  But, .sym files don't have a TimeDateStamp, so
    //  the SymbolFileTimeDate may be zero.  If either the checksums match
    //  or the timedate stamps match, we'll say its valid.
    //

    if (( ImageFileCheckSum == SymbolFileCheckSum ) ||
        ( ImageFileTimeDate == SymbolFileTimeDate )) {

        return TRUE;
        }

    if ( ! bNoSymWarn ) {

        switch ( SymType ) {
            case SymNone:     SymTypeText = "No";       break;
            case SymCoff:     SymTypeText = "Coff";     break;
            case SymCv:       SymTypeText = "CodeView"; break;
            case SymPdb:      SymTypeText = "Pdb";      break;
            case SymExport:   SymTypeText = "Export";   break;
            case SymDeferred: SymTypeText = "Deferred"; break;
            case SymSym:      SymTypeText = "Sym";      break;
            case 0x1001:      SymTypeText = ".psym";    break;
            default:          SymTypeText = "Unknown";  break;
            }

        printf(
            "\n"
            "WARNING: %s symbols %s don't match %s:\n"
            "    symbol file checksum (%08X) does not match image (%08X), and\n"
            "    symbol file timedate (%08X) does not match image (%08X).\n\n",
            SymTypeText,
            SymbolFileName,
            FileNameArray[ WhichFile ],
            SymbolFileCheckSum,
            ImageFileCheckSum,
            SymbolFileTimeDate,
            ImageFileTimeDate
            );
        }

    return bUseBadSyms;
    }


VOID
RecursiveDescent(
    VOID
    );

VOID
BuildUsingFileList(
    VOID
    );

VOID
MyCreatePath(
    IN LPSTR FullFileName
    );

VOID
DeleteOtherPatchFiles(
    IN LPCSTR Directory,
    IN LPCSTR FileName,
    IN LPCSTR DontDeleteSignature
    );


int __cdecl
SortNodeCompare(
    const void * _SortNode1,
    const void * _SortNode2
    )
{
    const SORTNODE * SortNode1 = _SortNode1;
    const SORTNODE * SortNode2 = _SortNode2;
    int i;

    i = min( SortNode1->Node->NameLength, SortNode2->Node->NameLength );

    i = strncmp( SortNode1->Node->Name, SortNode2->Node->Name, i );

    if ( i == 0 ) {

        i = SortNode1->Node->NameLength - SortNode2->Node->NameLength;
        }

    return( i );
}

void
ReportCompressionRatio(
    QWORD qwSourceSize,
    QWORD qwTargetSize
    )
{
    double Percent = 0;
    double Ratio = 0;

    if ( qwSourceSize != 0 )
    {
        Percent = ((( signed __int64 )( qwSourceSize - qwTargetSize ) * 100.0 ) / qwSourceSize );

        if ( Percent > 99.99 )
        {
             Percent = 99.99;
        }
    }

    if ( qwTargetSize != 0 )
    {
        Ratio = ((double) qwSourceSize / qwTargetSize );

        if ( Ratio > 99999.99 )
        {
             Ratio = 99999.99;
        }
    }

    printf( "%10I64u bytes (%5.2f%%, %8.2f:1)\n", qwTargetSize, Percent, Ratio );

}


int __cdecl main( int argc, char *argv[] ) {

    LPSTR OldFileName    = NULL;
    LPSTR NewFileName    = NULL;
    LPSTR PatchFileName  = NULL;
    LPSTR PatchCachePath = NULL;
    LPSTR arg;
    LPSTR p, q;
    LPSTR FileName;
    ULONG FileNum;
    int   i;

    DWORDLONG StartTime;
    DWORDLONG StopTime;
    DWORDLONG Elapsed;
    SYSTEMTIME SysTime;

    ULONG Hours;
    ULONG Minutes;
    ULONG Seconds;
    ULONG TotalSeconds;

    NAME_TREE_ENUM NewFileEnumerator;
    NAME_NODE * Node;
    NEWFILE * NewFile;
    ULONG NewFileIndex;
    SORTNODE * NewFilesSorted;

    ULONG WindowSizePower = 0;

    BOOL Force194 = FALSE;
    BOOL Force197 = FALSE;

    SetErrorMode( SEM_FAILCRITICALERRORS );

#ifndef DEBUG
    SetErrorMode( SEM_NOALIGNMENTFAULTEXCEPT | SEM_FAILCRITICALERRORS );
#endif

    GetSystemTimeAsFileTime( (PFILETIME)&StartTime );

    CopyRight();

    OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_PE_IRELOC_2;   // unless explicitly disabled
    OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_PE_RESOURCE_2; // unless explicitly disabled

    for ( i = 1; i < argc; i++ ) {

        arg = argv[ i ];

        if ( strchr( arg, '?' )) {
            Usage();
            }
        }

    //
    //  First get environment arguments because command-line args will
    //  override them.
    //

    if ( GetMpatchEnvironVar( "e8", NULL )) {
        OptionFlags &= ~PATCH_OPTION_USE_LZX_A;
        }

    if ( GetMpatchEnvironVar( "noe8", NULL )) {
        OptionFlags &= ~PATCH_OPTION_USE_LZX_B;
        }

    if ( GetMpatchEnvironVar( "mspatch194compat", NULL )) {
        Force194 = TRUE;
        }

    if ( GetMpatchEnvironVar( "mspatcha_win2000", NULL )) {
        Force194 = TRUE;
        }

    if ( GetMpatchEnvironVar( "mspatch197compat", NULL )) {
        Force197 = TRUE;
        }

    if ( GetMpatchEnvironVar( "mspatcha_winxp", NULL )) {
        Force197 = TRUE;
        }

    if ( GetMpatchEnvironVar( "nobindfix", NULL )) {
        OptionFlags |= PATCH_OPTION_NO_BINDFIX;
        }

    if ( GetMpatchEnvironVar( "nolockfix", NULL )) {
        OptionFlags |= PATCH_OPTION_NO_LOCKFIX;
        }

    if ( GetMpatchEnvironVar( "norebase", NULL )) {
        OptionFlags |= PATCH_OPTION_NO_REBASE;
        }

    if ( GetMpatchEnvironVar( "norestime", NULL )) {
        OptionFlags |= PATCH_OPTION_NO_RESTIMEFIX;
        }

    if ( GetMpatchEnvironVar( "nochecksum", NULL )) {
        OptionFlags |= PATCH_OPTION_NO_CHECKSUM;
        }

    if ( GetMpatchEnvironVar( "nostoretime", NULL )) {
        OptionFlags |= PATCH_OPTION_NO_TIMESTAMP;
        }

    if ( GetMpatchEnvironVar( "failbigger", NULL )) {
        OptionFlags |= PATCH_OPTION_FAIL_IF_BIGGER;
        }

    if ( GetMpatchEnvironVar( "failifbigger", NULL )) {
        OptionFlags |= PATCH_OPTION_FAIL_IF_BIGGER;
        }

    if ( GetMpatchEnvironVar( "failsame", NULL )) {
        OptionFlags |= PATCH_OPTION_FAIL_IF_SAME_FILE;
        }

    if ( GetMpatchEnvironVar( "failifsame", NULL )) {
        OptionFlags |= PATCH_OPTION_FAIL_IF_SAME_FILE;
        }

    if ( GetMpatchEnvironVar( "undecorated", NULL )) {
        OptionData.SymbolOptionFlags |= PATCH_SYMBOL_UNDECORATED_TOO;
        }

    if ( GetMpatchEnvironVar( "nosyms", NULL )) {
        OptionData.SymbolOptionFlags |= PATCH_SYMBOL_NO_IMAGEHLP;
        }

    if ( GetMpatchEnvironVar( "nosymfail", NULL )) {
        OptionData.SymbolOptionFlags |= PATCH_SYMBOL_NO_FAILURES;
        }

    if ( GetMpatchEnvironVar( "nosymwarn", NULL )) {
        bNoSymWarn = TRUE;
        }

    if ( GetMpatchEnvironVar( "noaffinity", NULL )) {
        bNoAffinity = TRUE;
        }

    if ( GetMpatchEnvironVar( "usebadsyms", NULL )) {
        bUseBadSyms = TRUE;
        }

    if ( GetMpatchEnvironVar( "copyothers", NULL )) {
        bCopyNonPatches = TRUE;
        }

    if ( GetMpatchEnvironVar( "subdirs", NULL )) {
        bSubDirectories = TRUE;
        }

    if ( GetMpatchEnvironVar( "forest", NULL )) {
        bSourceIsForest = TRUE;
        }

    if ( GetMpatchEnvironVar( "changed", NULL )) {
        bIncremental = TRUE;
        }

    if ( GetMpatchEnvironVar( "crcname", NULL )) {
        bCrcName = TRUE;
        }

    if ( GetMpatchEnvironVar( "window", TextBuffer )) {
        WindowSizePower = strtoul( TextBuffer, NULL, 0 );
        }

    if ( GetMpatchEnvironVar( "oldsympath", TextBuffer )) {

        p = TextBuffer;
        q = strchr( p, ',' );

        if ( q ) {
            *q = 0;
            }

        FileName = p;

        FileNum = 1;

        if ( q ) {

            p = q + 1;

            FileNum = strtoul( p, NULL, 0 );

            if ( FileNum == 0 ) {
                FileNum = 1;
                }
            }

        if ( FileNum == 1 ) {
            strcpy( OldSymPath, FileName );
            }
        }

    if ( GetMpatchEnvironVar( "newsympath", TextBuffer )) {

        p = TextBuffer;
        q = strchr( p, ',' );

        if ( q ) {
            *q = 0;
            }

        FileName = p;

        FileNum = 1;

        if ( q ) {

            p = q + 1;

            FileNum = strtoul( p, NULL, 0 );

            if ( FileNum == 0 ) {
                FileNum = 1;
                }
            }

        if ( FileNum == 1 ) {
            strcpy( NewSymPath, FileName );
            }
        }

    if ( GetMpatchEnvironVar( "files", TextBuffer )) {

        strcpy( NewFileList, TextBuffer );
        }

    if ( GetMpatchEnvironVar( "cache", TextBuffer )) {

        bCaching = TRUE;
        PatchCachePath = _strdup( TextBuffer );
        }

    if ( GetMpatchEnvironVar( "fallbacks", NULL )) {

        bCreateFallbackFiles = TRUE;
        }

    if ( GetMpatchEnvironVar( "notransformrelocs", NULL  )) {
        OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RELOCS;
        }

    if ( GetMpatchEnvironVar( "notransformresource", NULL  )) {
        OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RESOURCE;
        }

    if ( GetMpatchEnvironVar( "notransformimports", NULL  )) {
        OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_IMPORTS;
        }

    if ( GetMpatchEnvironVar( "notransformexports", NULL  )) {
        OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_EXPORTS;
        }

    if ( GetMpatchEnvironVar( "notransformreljmps", NULL  )) {
        OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RELJMPS;
        }

    if ( GetMpatchEnvironVar( "notransformrelcalls", NULL  )) {
        OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RELCALLS;
        }



    //
    //  Now process commandline args
    //

    for ( i = 1; i < argc; i++ ) {

        arg = argv[ i ];

        if ( strchr( arg, '?' )) {
            Usage();
            }

        if (( *arg == '-' ) || ( *arg == '/' )) {

            _strlwr( ++arg );

            if ( strcmp( arg, "e8" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_USE_LZX_A;
                }
            else if ( strcmp( arg, "noe8" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_USE_LZX_B;
                }
            else if ( strcmp( arg, "mspatch194compat" ) == 0 ) {
                Force194 = TRUE;
                }
            else if ( strcmp( arg, "mspatcha_win2000" ) == 0 ) {
                Force194 = TRUE;
                }
            else if ( strcmp( arg, "mspatch197compat" ) == 0 ) {
                Force197 = TRUE;
                }
            else if ( strcmp( arg, "mspatcha_winxp" ) == 0 ) {
                Force197 = TRUE;
                }
            else if ( strcmp( arg, "nobindfix" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_NO_BINDFIX;
                }
            else if ( strcmp( arg, "bindfix" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_NO_BINDFIX;
                }
            else if ( strcmp( arg, "nolockfix" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_NO_LOCKFIX;
                }
            else if ( strcmp( arg, "lockfix" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_NO_LOCKFIX;
                }
            else if ( strcmp( arg, "norebase" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_NO_REBASE;
                }
            else if ( strcmp( arg, "rebase" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_NO_REBASE;
                }
            else if ( strcmp( arg, "norestime" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_NO_RESTIMEFIX;
                }
            else if ( strcmp( arg, "norestimefix" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_NO_RESTIMEFIX;
                }
            else if ( strcmp( arg, "restime" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_NO_RESTIMEFIX;
                }
            else if ( strcmp( arg, "restimefix" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_NO_RESTIMEFIX;
                }
            else if ( strcmp( arg, "nochecksum" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_NO_CHECKSUM;
                }
            else if ( strcmp( arg, "checksum" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_NO_CHECKSUM;
                }
            else if ( strcmp( arg, "nostoretime" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_NO_TIMESTAMP;
                }
            else if ( strcmp( arg, "storetime" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_NO_TIMESTAMP;
                }
            else if ( strcmp( arg, "failbigger" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_FAIL_IF_BIGGER;
                }
            else if ( strcmp( arg, "nofailbigger" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_FAIL_IF_BIGGER;
                }
            else if ( strcmp( arg, "failifbigger" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_FAIL_IF_BIGGER;
                }
            else if ( strcmp( arg, "nofailifbigger" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_FAIL_IF_BIGGER;
                }
            else if ( strcmp( arg, "failifsame" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_FAIL_IF_SAME_FILE;
                }
            else if ( strcmp( arg, "failsame" ) == 0 ) {
                OptionFlags |= PATCH_OPTION_FAIL_IF_SAME_FILE;
                }
            else if ( strcmp( arg, "nofailifsame" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_FAIL_IF_SAME_FILE;
                }
            else if ( strcmp( arg, "nofailsame" ) == 0 ) {
                OptionFlags &= ~PATCH_OPTION_FAIL_IF_SAME_FILE;
                }
            else if ( strcmp( arg, "decorated" ) == 0 ) {
                OptionData.SymbolOptionFlags &= ~PATCH_SYMBOL_UNDECORATED_TOO;
                }
            else if ( strcmp( arg, "undecorated" ) == 0 ) {
                OptionData.SymbolOptionFlags |= PATCH_SYMBOL_UNDECORATED_TOO;
                }
            else if ( strcmp( arg, "nosyms" ) == 0 ) {
                OptionData.SymbolOptionFlags |= PATCH_SYMBOL_NO_IMAGEHLP;
                }
            else if ( strcmp( arg, "syms" ) == 0 ) {
                OptionData.SymbolOptionFlags &= ~PATCH_SYMBOL_NO_IMAGEHLP;
                }
            else if ( strcmp( arg, "nosymfail" ) == 0 ) {
                OptionData.SymbolOptionFlags |= PATCH_SYMBOL_NO_FAILURES;
                }
            else if ( strcmp( arg, "symfail" ) == 0 ) {
                OptionData.SymbolOptionFlags &= ~PATCH_SYMBOL_NO_FAILURES;
                }
            else if ( strcmp( arg, "nosymwarn" ) == 0 ) {
                bNoSymWarn = TRUE;
                }
            else if ( strcmp( arg, "symwarn" ) == 0 ) {
                bNoSymWarn = FALSE;
                }
            else if ( strcmp( arg, "noaffinity" ) == 0 ) {
                bNoAffinity = TRUE;
                }
            else if ( strcmp( arg, "usebadsyms" ) == 0 ) {
                bUseBadSyms = TRUE;
                }
            else if ( strcmp( arg, "nousebadsyms" ) == 0 ) {
                bUseBadSyms = FALSE;
                }
            else if ( strcmp( arg, "nobadsyms" ) == 0 ) {
                bUseBadSyms = FALSE;
                }
            else if ( strcmp( arg, "copyothers" ) == 0 ) {
                bCopyNonPatches = TRUE;
                }
            else if ( strcmp( arg, "nocopyothers" ) == 0 ) {
                bCopyNonPatches = FALSE;
                }
            else if ( strcmp( arg, "subdirs" ) == 0 ) {
                bSubDirectories = TRUE;
                }
            else if ( strcmp( arg, "forest" ) == 0 ) {
                bSourceIsForest = TRUE;
                }
            else if ( strcmp( arg, "changed" ) == 0 ) {
                bIncremental = TRUE;
                }
            else if ( strcmp( arg, "crcname" ) == 0 ) {
                bCrcName = TRUE;
                }
            else if ( strcmp( arg, "nocrcname" ) == 0 ) {
                bCrcName = FALSE;
                }
            else if ( memcmp( arg, "window:", 7 ) == 0 ) {

                WindowSizePower = strtoul( arg + 7, NULL, 0 );

                if ( WindowSizePower == 0 ) {
                    Usage();
                    }
                }

            else if ( memcmp( arg, "oldsympath:", 11 ) == 0 ) {

                p = strchr( arg, ':' ) + 1;
                q = strchr( p,   ',' );

                if ( q ) {
                    *q = 0;
                    }

                FileName = p;

                FileNum = 1;

                if ( q ) {

                    p = q + 1;

                    FileNum = strtoul( p, NULL, 0 );

                    if ( FileNum == 0 ) {
                        FileNum = 1;
                        }
                    }

                if ( FileNum != 1 ) {
                    Usage();
                    }

                strcpy( OldSymPath, FileName );
                }
            else if ( memcmp( arg, "newsympath:", 11 ) == 0 ) {

                p = strchr( arg, ':' ) + 1;
                q = strchr( p,   ',' );

                if ( q ) {
                    *q = 0;
                    }

                FileName = p;

                FileNum = 1;

                if ( q ) {

                    p = q + 1;

                    FileNum = strtoul( p, NULL, 0 );

                    if ( FileNum == 0 ) {
                        FileNum = 1;
                        }
                    }

                if ( FileNum != 1 ) {
                    Usage();
                    }

                strcpy( NewSymPath, FileName );
                }
            else if ( memcmp( arg, "files:", 6 ) == 0 ) {

                strcpy( NewFileList, strchr( arg, ':' ) + 1 );
                }
            else if ( memcmp( arg, "cache:", 6 ) == 0 ) {

                bCaching = TRUE;
                PatchCachePath = arg + 6;
                }
            else if ( strcmp( arg, "fallbacks" ) == 0 ) {

                bCreateFallbackFiles = TRUE;
                }

            else if ( strcmp( arg, "notransformrelocs" ) == 0 ) {
                OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RELOCS;
                }
            else if ( strcmp( arg, "notransformresource" ) == 0 ) {
                OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RESOURCE;
                }
            else if ( strcmp( arg, "notransformimports" ) == 0 ) {
                OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_IMPORTS;
                }
            else if ( strcmp( arg, "notransformexports" ) == 0 ) {
                OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_EXPORTS;
                }
            else if ( strcmp( arg, "notransformreljmps" ) == 0 ) {
                OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RELJMPS;
                }
            else if ( strcmp( arg, "notransformrelcalls" ) == 0 ) {
                OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RELCALLS;
                }

            else if (( strcmp( arg, "nocompare"  ) == 0 ) ||
                     ( strcmp( arg, "docompare"  ) == 0 ) ||
                     ( strcmp( arg, "compare"    ) == 0 ) ||
                     ( strcmp( arg, "noprogress" ) == 0 ) ||
                     ( strcmp( arg, "progress"   ) == 0 )) {

                ;   // politely ignore mpatch.exe options we don't support
                }

            else {
                Usage();
                }
            }

        else if ( OldFileName == NULL ) {
            OldFileName = arg;
            }
        else if ( NewFileName == NULL ) {
            NewFileName = arg;
            }
        else if ( PatchFileName == NULL ) {
            PatchFileName = arg;
            }
        else {
            Usage();
            }
        }

    if (( OldFileName == NULL ) || ( NewFileName == NULL ) || ( PatchFileName == NULL )) {
        Usage();
        }

    if ( WindowSizePower != 0 ) {

		OptionFlags |= PATCH_OPTION_EXTENDED_OPTIONS;
        OptionData.ExtendedOptionFlags |= PATCH_OPTION_SPECIFIED_WINDOW;
        OptionData.MaxLzxWindowSize = 1 << WindowSizePower;

        if ( OptionData.MaxLzxWindowSize < LZX_MINWINDOW ) {
            Usage();
            }
        }

    if ( Force194 ) {
        OptionFlags &= ~PATCH_OPTION_USE_LZX_LARGE;
        Force197 = TRUE;
        }

    if ( Force197 ) {
        OptionFlags &= ~PATCH_OPTION_INTERLEAVE_FILES;
        OptionData.ExtendedOptionFlags &= ~PATCH_OPTION_SPECIFIED_WINDOW;
        OptionData.ExtendedOptionFlags &= ~PATCH_TRANSFORM_PE_IRELOC_2;
        OptionData.ExtendedOptionFlags &= ~PATCH_TRANSFORM_PE_RESOURCE_2;
        OptionData.InterleaveMapArray = NULL;
        OptionData.MaxLzxWindowSize = 0;
        }

    if ( bSourceIsForest ) {
        bCrcName = TRUE;
        }

    //
    //  Useful on MP machines, set affinity to one processor and lower
    //  priority to below normal since we are heavily CPU intensive.
    //

    if ( ! bNoAffinity ) {

        ULONG InstanceCounterSnap = _InterlockedExchangeAdd( &SharedInstanceCounter, 1 );
        ULONG ProcessAffinityMask = 1;
        ULONG SystemAffinityMask;

        GetProcessAffinityMask(
            GetCurrentProcess(),
            &ProcessAffinityMask,
            &SystemAffinityMask
            );

        if ( ProcessAffinityMask > 1 ) {

            //
            //  Note this is just a "good effort", it is not perfect because
            //  process/system affinity mask is not always packed in the lowest
            //  bits and we're not wrapping.  If this shift doesn't result in
            //  a valid mask, SetAffinityMask will simply fail leaving us
            //  running on any available processor.
            //

            ProcessAffinityMask = 1 << ( InstanceCounterSnap & 0x1F );

            SetProcessAffinityMask( GetCurrentProcess(), ProcessAffinityMask );

            }

        SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_LOWEST );

        }

    GetFullPathName( OldFileName,   sizeof( OldDirRoot ),   OldDirRoot,   &FileName );
    GetFullPathName( NewFileName,   sizeof( NewDirRoot ),   NewDirRoot,   &FileName );
    GetFullPathName( PatchFileName, sizeof( PatchDirRoot ), PatchDirRoot, &FileName );

    StripTrailingBackslash( OldDirRoot );
    StripTrailingBackslash( NewDirRoot );

    if ( ! *OldSymPath ) {
        strcpy( OldSymPath, OldDirRoot );
        }

    if ( ! *NewSymPath ) {
        strcpy( NewSymPath, NewDirRoot );
        }

    AppendTrailingBackslash( OldDirRoot );
    AppendTrailingBackslash( NewDirRoot );
    AppendTrailingBackslash( PatchDirRoot );

    _strlwr( OldDirRoot );
    _strlwr( NewDirRoot );
    _strlwr( PatchDirRoot );

    NewDirRootEnd = NewDirRoot + strlen( NewDirRoot );
    OldDirRootEnd = OldDirRoot + strlen( OldDirRoot );
    PatchDirRootEnd = PatchDirRoot + strlen( PatchDirRoot );

    if ( bCaching ) {

        GetFullPathName( PatchCachePath, sizeof( PatchCache ), PatchCache, &FileName );
        StripTrailingBackslash( PatchCache );
        AppendTrailingBackslash( PatchCache );
        PatchCacheEnd = PatchCache + strlen( PatchCache );
        CreateNewDirectoryForFile( PatchCache );
        if ( ! IsDirectory( PatchCache )) {

            printf( "MPATCHES: Invalid path for patch cache \"%s\"\n", PatchCache );
            return( 1 );
            }
        }

    SubAllocator = CreateSubAllocator( 0x10000, 0x10000 );
    if ( SubAllocator == NULL ) {

        printf( "MPATCHES: Out of memory\n" );
        return( 1 );
        }

    NameRbInitTree( &NewFileTree, SubAllocator );
    NameRbInitTree( &MD5Tree, SubAllocator );

    if ( NewFileList[ 0 ] == '\0' ) {

        RecursiveDescent();
        }
    else {

        BuildUsingFileList();
        }

    AssociateNewFileDuplicates();

    NewFilesSorted = malloc( CountNewFiles * sizeof( SORTNODE ));
    if ( NewFilesSorted == NULL ) {

        printf( "MPATCHES: Out of memory\n" );
        return( 1 );
        }

    NewFileIndex = 0;

    Node = NameRbEnumFirst( &NewFileEnumerator, &NewFileTree );

    while ( Node != NULL ) {

        for ( NewFile = ((PRODUCTFILE *) Node->Context)->FirstNewFile;
              NewFile != NULL;
              NewFile = NewFile->Next ) {

            NewFilesSorted[ NewFileIndex ].NewFile = NewFile;
            NewFilesSorted[ NewFileIndex ].Node = Node;

            NewFileIndex++;
            }

        Node = NameRbEnumNext( &NewFileEnumerator );
        }

    qsort( NewFilesSorted, CountNewFiles, sizeof( SORTNODE ), SortNodeCompare );

    if ( bSourceIsForest ) {

        OldFileName = OldDirRootEnd;
        }

    for ( NewFileIndex = 0; NewFileIndex < CountNewFiles; NewFileIndex++ ) {

        NewFile = NewFilesSorted[ NewFileIndex ].NewFile;
        Node    = NewFilesSorted[ NewFileIndex ].Node;

        strcpy( NewDirRootEnd, NewFile->Path );
        NewFileName = strchr( NewDirRootEnd, '\0' );
        memcpy( NewFileName, Node->Name, Node->NameLength );
        NewFileName[ Node->NameLength ] = '\0';

        if ( ! bSourceIsForest ) {

            strcpy( OldDirRootEnd, NewFile->Path );
            OldFileName = strchr( OldDirRootEnd, '\0' );
            }

        strcpy( PatchDirRootEnd, NewFile->Path );
        PatchFileName = strchr( PatchDirRootEnd, '\0' );

        MakePatchesForFile( NewFileName,
                            OldFileName,
                            PatchFileName,
                            NewFile->FileTime,
                            NewFile->FileSize
                            );
        }

    GetSystemTimeAsFileTime( (PFILETIME)&StopTime );

    Elapsed = StopTime - StartTime;

    FileTimeToSystemTime( (PFILETIME)&Elapsed, &SysTime );

    Hours   = SysTime.wHour;
    Minutes = SysTime.wMinute;
    Seconds = SysTime.wSecond;

    Hours += (ULONG)( SysTime.wDay - 1 ) * 24;

    TotalSeconds = ( Hours * 3600 ) + ( Minutes * 60 ) + Seconds;

    printf( "\n\nTotal:\n\n" );

    if ( CountPatchFiles > 0 ) {

        printf( "%9u patches:               ", CountPatchFiles );

        ReportCompressionRatio( TotalPatchSourceSizes, TotalPatchTargetSizes );

        }

    else {

        printf( "%9u patches\n", 0 );

        }

    if ( TotalCopiedSizes > 0 ) {

        printf( "%9u copies:      %20I64u bytes\n",
                CountCopiedFiles,
                TotalCopiedSizes
              );

        }

    else if ( bCopyNonPatches ) {

        printf( "%9u copies\n", 0 );

        }

    if (( TotalPatchSourceSizes > 0 ) && ( TotalCopiedSizes > 0 )) {

        TotalPatchSourceSizes += TotalCopiedSizes;
        TotalPatchTargetSizes += TotalCopiedSizes;
        CountPatchFiles       += CountCopiedFiles;

        printf( "%9u total:       %20I64u bytes\n", CountPatchFiles, TotalPatchSourceSizes );
        }

    if ( TotalSeconds > 0 ) {

        printf( "%3d:%02d:%02d elapsed:     %20I64u new file bytes per second\n",
                Hours,
                Minutes,
                Seconds,
                TotalPatchSourceSizes / TotalSeconds
              );
        }

    if ( ErrorCount )
    {
        printf( "\n%9u ERRORS!!!\a\a\a\n", ErrorCount );
    }

    printf( "\n" );

    if ( ErrorCount )
    {
        return ErrorCount;
    }

    return( ReturnCode );
    }


ULONG
MakePatchesForFile(
    LPCSTR   NewDirFileName,    // base file name in NewDirRoot, ie "kernel32.dll" in "new\uniproc\kernel32.dll"
    LPSTR    OldDirFileName,    // pos for base file name in OldDirRoot, ie "x" in "old\uniproc\x"
                                // or top base if bSourceIsForest, ie "x" in "old\x", not drilled down
    LPSTR    PatchDirFileName,  // pos for base file name in PatchDirRoot, ie "x" in "patches\uniproc\x"
    FILETIME NewFileLastWriteTime,
    DWORD    NewFileSize
    )
    {
    static WIN32_FILE_ATTRIBUTE_DATA FileData;
    CHAR     OldFileHash[ HASH_HEX_LENGTH ];
    CHAR     NewFileHash[ HASH_HEX_LENGTH ];
    CHAR     CrcTextBuffer[ 16 ];
    BOOL     Success;
    ULONG    PatchFileSize;
    ULONG    Result = 0;

    Success = FALSE;

    strcpy( PatchDirFileName, "x" );
    CreateNewDirectoryForFile( PatchDirRoot );

    if ( bCreateFallbackFiles ) {

        Success = FALSE;

        //
        // create null patch
        //

        if ( ! bCaching ||
             GetFilePatchSignature( NewDirRoot,
                                      PATCH_OPTION_NO_REBASE
                                    | PATCH_OPTION_NO_BINDFIX
                                    | PATCH_OPTION_NO_LOCKFIX
                                    | PATCH_OPTION_NO_RESTIMEFIX
                                    | PATCH_OPTION_SIGNATURE_MD5,
                                    NULL,
                                    0, NULL,    // no ignore
                                    0, NULL,    // no retain
                                    sizeof( NewFileHash ),
                                    NewFileHash )) {

            if ( strlen( NewDirRootEnd ) > 25 ) {

                printf( "%s\n%-25s ", NewDirRootEnd, "" );
                }
            else {

                printf( "%-25s ", NewDirRootEnd );
                }

            printf( "base  " );

            strcpy( PatchDirFileName, NewDirFileName );
            strcat( PatchDirFileName, "._p0" );

            if ( bCaching ) {

                strcpy( PatchCacheEnd, NewDirFileName );
                strcat( PatchCacheEnd, "\\" );
                strcat( PatchCacheEnd, NewFileHash );
                strcat( PatchCacheEnd, "\\" );
                strcat( PatchCacheEnd, szZeroLengthFileHash );
                }

            if (( bIncremental ) &&
                ( GetFileAttributesEx( PatchDirRoot, GetFileExInfoStandard, &FileData )) &&
                ( CompareFileTime( &FileData.ftLastWriteTime, &NewFileLastWriteTime ) == 0 )) {

                printf( "is current\n" );
                Success = TRUE;
                }

            else if ( bCaching &&
                     CopyFile( PatchCache, PatchDirRoot, FALSE ) &&
                     AdjustPatchTime( PatchDirRoot, NewFileLastWriteTime )) {

                 printf("*");

                 Success = TRUE;
                }

            else {

                PATCH_OLD_FILE_INFO_A OldFileInfo = {
                                        sizeof( PATCH_OLD_FILE_INFO_A ),
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        NULL
                                        };

                ULONG ZeroPatchOptionFlags = OptionFlags;

                ZeroPatchOptionFlags &= ~PATCH_OPTION_FAIL_IF_BIGGER;
                ZeroPatchOptionFlags &= ~PATCH_OPTION_INTERLEAVE_FILES;

                ZeroPatchOptionFlags &= ~PATCH_OPTION_USE_LZX_A;
                ZeroPatchOptionFlags |=  PATCH_OPTION_USE_LZX_B;        // always use E8 in LZX

                ZeroPatchOptionFlags |=  PATCH_OPTION_NO_BINDFIX;
                ZeroPatchOptionFlags |=  PATCH_OPTION_NO_LOCKFIX;
                ZeroPatchOptionFlags |=  PATCH_OPTION_NO_REBASE;
                ZeroPatchOptionFlags |=  PATCH_OPTION_NO_CHECKSUM;
                ZeroPatchOptionFlags |=  PATCH_OPTION_NO_RESTIMEFIX;

                Success = CreatePatchFileEx(
                              1,
                              &OldFileInfo,
                              NewDirRoot,
                              PatchDirRoot,
                              ZeroPatchOptionFlags,
                              NULL,
                              NULL,
                              NULL
                              );

                if ( Success ) {

                    printf(" ");

                    if ( bCaching ) {

                        if ( ! CopyFile( PatchDirRoot, PatchCache, FALSE )) {

                            if ( CreateNewDirectoryForFile( PatchCache )) {

                                CopyFile( PatchDirRoot, PatchCache, FALSE );
                                }
                            }
                        }
                    }

                else {

                    ULONG ErrorCode = GetLastError();

                    CHAR ErrorText[ 20 ];

                    sprintf( ErrorText, ( ErrorCode < 0x80000000 ) ? "%d" : "%X", ErrorCode );

                    printf( "failed!!! (%s)\a\n", ErrorText );

                    ErrorCount++;
                    }
                }

            if ( Success ) {

                PatchFileSize = (ULONG) GetFileSizeByName( PatchDirRoot );

                if (( NewFileSize != 0 ) && ( PatchFileSize != 0xFFFFFFFF )) {

                    if ( PatchFileSize > NewFileSize ) {

                        printf( "failed (patch bigger than new file)\n" );

                        DeleteFile( PatchDirRoot );

                        Success = FALSE;

                        //
                        //  This should not increase error count as it is
                        //  a requested failure.
                        //
                        }

                    else {

                        ReportCompressionRatio( NewFileSize, PatchFileSize );

                        TotalPatchSourceSizes += NewFileSize;
                        TotalPatchTargetSizes += PatchFileSize;
                        CountPatchFiles++;

                        }
                    }
                }

            fflush( stdout );
            }

        //
        //  if null patch fallback doesn't work out, copy uncompressed instead
        //

        if ( ! Success ) {

            strcpy( PatchDirFileName, NewDirFileName );

            if ( CopyFile( NewDirRoot, PatchDirRoot, FALSE ) ||
                ( CreateNewDirectoryForFile( PatchDirRoot ) &&
                  CopyFile( NewDirRoot, PatchDirRoot, FALSE ))) {

                Success = TRUE;
                }

            else {

                printf( "MPATCHES: Unable to create fallback file for %s\n", NewDirFileName );

                ReturnCode = 1;
                }
            }
        }

    if ( GetFirstCandidate( OldDirRoot, OldDirFileName, NewDirFileName, OldFileHash ) &&
         ( ! bCaching ||
           GetFilePatchSignature( NewDirRoot,
                                    PATCH_OPTION_NO_REBASE
                                  | PATCH_OPTION_NO_BINDFIX
                                  | PATCH_OPTION_NO_LOCKFIX
                                  | PATCH_OPTION_NO_RESTIMEFIX
                                  | PATCH_OPTION_SIGNATURE_MD5,
                                  NULL,
                                  0, NULL,    // no ignore
                                  0, NULL,    // no retain
                                  HASH_HEX_LENGTH,
                                  NewFileHash ))) {

        do {

            *CrcTextBuffer = 0;

            if ( strlen( NewDirRootEnd ) > 25 ) {

                printf( "%s\n%-25s ", NewDirRootEnd, "" );
                }
            else {

                printf( "%-25s ", NewDirRootEnd );
                }

            printf( "patch " );

            Success = TRUE;

            if ( bCrcName ) {

                Success = GetFilePatchSignature(
                              OldDirRoot,
                              OptionFlags | PATCH_OPTION_NO_CHECKSUM,
                              &OptionData,
                              0,
                              NULL,
                              0,
                              NULL,
                              sizeof( CrcTextBuffer ),
                              CrcTextBuffer
                              );

                if ( Success ) {

                    sprintf(
                        PatchDirFileName,
                        "%s.%s._p",
                        NewDirFileName,
                        CrcTextBuffer
                        );
                    }
                }

            else {

                strcpy( PatchDirFileName, NewDirFileName );
                strcat( PatchDirFileName, "._p" );
                }

            if ( bCaching ) {

                strcpy( PatchCacheEnd, NewDirFileName );
                strcat( PatchCacheEnd, "\\" );
                strcat( PatchCacheEnd, NewFileHash );
                strcat( PatchCacheEnd, "\\" );
                strcat( PatchCacheEnd, OldFileHash );
                }

            if ( Success ) {

                if (( bIncremental ) &&
                    ( GetFileAttributesEx( PatchDirRoot, GetFileExInfoStandard, &FileData )) &&
                    ( CompareFileTime( &FileData.ftLastWriteTime, &NewFileLastWriteTime ) == 0 )) {

                    // target exists with same timestamp as source, so skip it.

                    printf( "is current\n" );
                    Success = TRUE;
                    }

                else {

                    if ( bCaching &&
                         CopyFile( PatchCache, PatchDirRoot, FALSE ) &&
                         AdjustPatchTime( PatchDirRoot, NewFileLastWriteTime )) {

                         printf("*");

                         Success = TRUE;
                        }
                    else {

                        PATCH_OLD_FILE_INFO_A OldFileInfo = {
                                                sizeof( PATCH_OLD_FILE_INFO_A ),
                                                OldDirRoot,
                                                0,
                                                NULL,
                                                0,
                                                NULL
                                                };

                        strcpy( OldPsymPath, OldSymPath );
                        AppendTrailingBackslash( OldPsymPath );
                        strcat( OldPsymPath, OldDirFileName );
                        strcat( OldPsymPath, ".psym" );

                        if ( GetFileAttributes( OldPsymPath ) != (DWORD) -1 )
                        {
                            strcpy( NewPsymPath, NewSymPath );
                            AppendTrailingBackslash( NewPsymPath );
                            strcat( NewPsymPath, NewDirFileName );
                            strcat( NewPsymPath, ".psym" );

                            if ( GetFileAttributes( NewPsymPath ) != (DWORD) -1 )
                            {
                                OptionData.NewFileSymbolPath = NewPsymPath;
                                OldSymPathArray[ 0 ]         = OldPsymPath;
                            }
                        }

                        Success = CreatePatchFileEx(
                                      1,
                                      &OldFileInfo,
                                      NewDirRoot,
                                      PatchDirRoot,
                                      OptionFlags,
                                      &OptionData,
                                      NULL,
                                      NULL
                                      );

                        if (( ! Success ) && ( GetLastError() & 0xC0000000 )) {

                            //
                            //  try without resource fixups
                            //

                            ULONG OriginalExtendedOptions = OptionData.ExtendedOptionFlags;

                            OptionData.ExtendedOptionFlags |= PATCH_TRANSFORM_NO_RESOURCE;

                            Success = CreatePatchFileEx(
                                          1,
                                          &OldFileInfo,
                                          NewDirRoot,
                                          PatchDirRoot,
                                          OptionFlags,
                                          &OptionData,
                                          NULL,
                                          NULL
                                          );

                            OptionData.ExtendedOptionFlags = OriginalExtendedOptions;

                            }

                        OptionData.NewFileSymbolPath = NewSymPath;  // restore
                        OldSymPathArray[ 0 ]         = OldSymPath;  // restore

                        if ( Success && bCaching ) {

                            // goes into cache even if larger, so we don't have to discover it again

                            if ( ! CopyFile( PatchDirRoot, PatchCache, FALSE )) {

                                if ( CreateNewDirectoryForFile( PatchCache )) {

                                    CopyFile( PatchDirRoot, PatchCache, FALSE );
                                    }
                                }
                            }

                        printf(" ");
                        }


                    if ( Success ) {

                        ULONG PatchFileSize = (ULONG) GetFileSizeByName( PatchDirRoot );

                        if (( NewFileSize != 0 ) && ( PatchFileSize != 0xFFFFFFFF )) {

                            if ( PatchFileSize > NewFileSize ) {

                                printf( "failed (patch bigger than new file)\n" );

                                DeleteFile( PatchDirRoot );
                                }
                            else {

                                ReportCompressionRatio( NewFileSize, PatchFileSize );

                                TotalPatchSourceSizes += NewFileSize;
                                TotalPatchTargetSizes += PatchFileSize;
                                CountPatchFiles++;

                                }
                            }
                        }

                    else {

                        ULONG ErrorCode = GetLastError();

                        if ( ErrorCode == ERROR_PATCH_BIGGER_THAN_COMPRESSED ) {

                            printf( "failed (patch bigger than compressed file)\n" );
                            }

                        else if ( ErrorCode == ERROR_PATCH_SAME_FILE ) {

                            printf( "skipped (same file)\n" );
                            }

                        else {

                            CHAR ErrorText[ 20 ];

                            sprintf( ErrorText, ( ErrorCode < 0x10000000 ) ? "%d" : "%X", ErrorCode );

                            printf( "failed!!! (%s)\a\n", ErrorText );

                            Result = 1;

                            ErrorCount++;
                            }
                        }
                    }
                }

            fflush( stdout );
            }
        while ( GetNextCandidate( OldDirRoot, OldDirFileName, NewDirFileName, OldFileHash ));
    }

    if ( Success ) {

        if ( ! bCreateFallbackFiles ) {

            //
            //  Found or created a patch, so make sure file does not
            //  also exist as non-patch in target directory.
            //

            strcpy( PatchDirFileName, NewDirFileName );
            DeleteFile( PatchDirRoot );
            }
        //
        //  If we're in both bIncremental and bCrcName modes, then
        //  we need to delete any other patch files for the same
        //  old file in this target directory.  This is to get rid
        //  of stale patch files.
        //

        if (( bIncremental ) && ( bCrcName )) {
            *PatchDirFileName = 0;
            DeleteOtherPatchFiles( PatchDirRoot, NewDirFileName, CrcTextBuffer );
            }
        }

    else if (( ! Success ) && ( bCopyNonPatches )) {

        if ( strlen( NewDirRootEnd ) > 25 ) {

            printf( "\r%-25s ", "" );
            }
        else {

            printf( "\r%-25s ", NewDirRootEnd );
            }

        if (( bCrcName ) && ( *CrcTextBuffer )) {

            sprintf(
                PatchDirFileName,
                "%s.%s._p",
                NewDirFileName,
                CrcTextBuffer
                );

            DeleteFile( PatchDirRoot );

            if ( bIncremental ) {
                *PatchDirFileName = 0;
                DeleteOtherPatchFiles( PatchDirRoot, NewDirFileName, CrcTextBuffer );
                }
            }

        else {

            strcpy( PatchDirFileName, NewDirFileName );
            strcat( PatchDirFileName, "._p" );
            DeleteFile( PatchDirRoot );
            }

        strcpy( PatchDirFileName, NewDirFileName );

        if (( bIncremental ) &&
            ( GetFileAttributesEx( PatchDirRoot, GetFileExInfoStandard, &FileData )) &&
            ( CompareFileTime( &FileData.ftLastWriteTime, &NewFileLastWriteTime ) == 0 )) {

            // target exists with same timestamp as source, so skip it.

            printf( "copy  is current\n" );
            Success = TRUE;
            }

        else {

            Success = CopyFile( NewDirRoot, PatchDirRoot, FALSE );

            if ( Success ) {

                printf( "copy   %10u bytes\n", NewFileSize );

                TotalCopiedSizes += NewFileSize;
                CountCopiedFiles++;

                }
            else {
                printf( "copy  failed!!! (%d)\a\n", GetLastError() );
                Result = 1;
                ErrorCount++;
                }
            }
        }

    fflush( stdout );

    return( Result );
    }


VOID
RecursiveDescent(
    VOID
    )
    {
    static WIN32_FIND_DATA FindData;
    unsigned    PathLength = strlen( NewDirRootEnd );
    LPSTR       NewDirFileName = NewDirRootEnd + PathLength;
    HANDLE      hFind;
    NAME_NODE * Node;
    NEWFILE   * NewFile;

    strcpy( NewDirFileName, "*" );

    hFind = FindFirstFile( NewDirRoot, &FindData );

    *NewDirFileName = 0;

    if ( hFind == INVALID_HANDLE_VALUE ) {
        printf( "\nFailed reading directory %s (GLE=%d)\a\n", NewDirRoot, GetLastError() );
        ReturnCode = 1;
        ErrorCount++;
        }

    else {

        do  {

            if ( ! ( FindData.dwFileAttributes & ( FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN ))) {

                _strlwr( FindData.cFileName );

                Node = NameRbInsert( &NewFileTree, FindData.cFileName );
                if ( Node == NULL ) {
                    printf( "Out of memory\n" );
                    ReturnCode = 1;
                    break;
                    }

                if ( Node->Context == NULL ) {

                    Node->Context = SubAllocate( SubAllocator, sizeof( PRODUCTFILE ));
                    if ( Node->Context == NULL ) {
                        printf( "Out of memory\n" );
                        ReturnCode = 1;
                        break;
                        }
                    }

                NewFile = SubAllocate( SubAllocator, sizeof( NEWFILE ) + PathLength );
                if ( NewFile == NULL ) {
                    printf( "Out of memory\n" );
                    ReturnCode = 1;
                    break;
                    }

                NewFile->FileNode = Node;
                NewFile->FileTime = FindData.ftLastWriteTime;
                NewFile->FileSize = FindData.nFileSizeLow;

                strcpy( NewFile->Path, NewDirRootEnd );

                NewFile->Next = ((PRODUCTFILE *) Node->Context)->FirstNewFile;
                ((PRODUCTFILE *) Node->Context)->FirstNewFile = NewFile;

                CountNewFiles++;
                }
            }
        while ( FindNextFile( hFind, &FindData ));

        if ( GetLastError() != ERROR_NO_MORE_FILES ) {
            printf( "\nFailed reading directory %s (GLE=%d)\a\n", NewDirRoot, GetLastError() );
            ErrorCount++;
            ReturnCode = 1;
            }

        FindClose( hFind );

        if ( bSubDirectories && ( ReturnCode == 0 )) {

            strcpy( NewDirFileName, "*" );

            hFind = FindFirstFile( NewDirRoot, &FindData );

            *NewDirFileName = 0;

            if ( hFind == INVALID_HANDLE_VALUE ) {
                printf( "\nFailed reading directory %s (GLE=%d)\a\n", NewDirRoot, GetLastError() );
                ErrorCount++;
                ReturnCode = 1;
                }

            else {

                do  {

                    if ((   ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) &&
                        ( ! ( FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN    )) &&
                        ( strcmp( FindData.cFileName, "."  ) != 0 ) &&
                        ( strcmp( FindData.cFileName, ".." ) != 0 )) {

                        _strlwr( FindData.cFileName );

                        if ( strcmp( FindData.cFileName, "symbols"  ) != 0 ) {

                            strcat( FindData.cFileName, "\\" );
                            strcpy( NewDirFileName, FindData.cFileName );

                            RecursiveDescent();

                            *NewDirFileName = 0;
                            }
                        }
                    }
                while ( FindNextFile( hFind, &FindData ));

                if ( GetLastError() != ERROR_NO_MORE_FILES ) {
                    printf( "\nFailed reading directory %s (GLE=%d)\a\n", NewDirRoot, GetLastError() );
                    ErrorCount++;
                    ReturnCode = 1;
                    }

                FindClose( hFind );
                }
            }
        }
    }


#if 0
VOID
OldRecursiveDescent(
    VOID
    )
    {
    static WIN32_FIND_DATA FindData;
    LPSTR  OldDirFileName   = strchr( OldDirRoot,   0 );
    LPSTR  NewDirFileName   = strchr( NewDirRoot,   0 );
    LPSTR  PatchDirFileName = strchr( PatchDirRoot, 0 );
    BOOL   FirstPatchInDir  = TRUE;
    HANDLE hFind;
    ULONG  Result;

    strcpy( NewDirFileName, "*" );

    hFind = FindFirstFile( NewDirRoot, &FindData );

    if ( hFind == INVALID_HANDLE_VALUE ) {
        *NewDirFileName = 0;
        printf( "\nFailed reading directory %s (GLE=%d)\a\n", NewDirRoot, GetLastError() );
        ErrorCount++;
        ReturnCode = 1;
        }

    else {

        do  {

            if ( ! ( FindData.dwFileAttributes & ( FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN ))) {

                _strlwr( FindData.cFileName );

                if ( FirstPatchInDir ) {
                     FirstPatchInDir = FALSE;
                     MyCreatePath( PatchDirRoot );
                     }

                strcpy( NewDirFileName, FindData.cFileName );

                Result = MakePatchesForFile( NewDirFileName,
                                             OldDirFileName,
                                             PatchDirFileName,
                                             FindData.ftLastWriteTime,
                                             FindData.nFileSizeLow
                                             );
                if ( Result != 0 ) {

                    ReturnCode = Result;
                    }
                }
            }
        while ( FindNextFile( hFind, &FindData ));

        if ( GetLastError() != ERROR_NO_MORE_FILES ) {
            *NewDirFileName = 0;
            printf( "\nFailed reading directory %s (GLE=%d)\a\n", NewDirRoot, GetLastError() );
            ErrorCount++;
            ReturnCode = 1;
            }

        FindClose( hFind );

        if ( bSubDirectories ) {

            strcpy( NewDirFileName, "*" );

            hFind = FindFirstFile( NewDirRoot, &FindData );

            if ( hFind == INVALID_HANDLE_VALUE ) {
                *NewDirFileName = 0;
                printf( "\nFailed reading directory %s (GLE=%d)\a\n", NewDirRoot, GetLastError() );
                ErrorCount++;
                ReturnCode = 1;
                }

            else {

                do  {

                    _strlwr( FindData.cFileName );

                    if ((   ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) &&
                        ( ! ( FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN    )) &&
                        ( strcmp( FindData.cFileName, "symbols"  ) != 0 ) &&
                        ( strcmp( FindData.cFileName, "."  ) != 0 ) &&
                        ( strcmp( FindData.cFileName, ".." ) != 0 )) {

                        strcat( FindData.cFileName, "\\" );
                        strcpy( NewDirFileName,   FindData.cFileName );
                        if ( ! bSourceIsForest ) {
                            strcpy( OldDirFileName, FindData.cFileName );
                            }
                        else {
                            *OldDirFileName = '\0';
                            }
                        strcpy( PatchDirFileName, FindData.cFileName );

                        RecursiveDescent();

                        }
                    }
                while ( FindNextFile( hFind, &FindData ));

                if ( GetLastError() != ERROR_NO_MORE_FILES ) {
                    *NewDirFileName = 0;
                    printf( "\nFailed reading directory %s (GLE=%d)\a\n", NewDirRoot, GetLastError() );
                    ErrorCount++;
                    ReturnCode = 1;
                    }

                FindClose( hFind );
                }
            }
        }

    *OldDirFileName   = 0;
    *NewDirFileName   = 0;
    *PatchDirFileName = 0;
    }
#endif


BOOL
GetNextFileName(
    IN OUT LPSTR * BufferPosition,
       OUT LPSTR * FileName
    )
    {
    LPSTR Position = * BufferPosition;

    enum { LOOKING_FOR_NAME,    // skipping all whitespace incl. line breaks
           COLLECTING_NAME,     // collecting until filename-breaking character
           COLLECTING_QUOTED,   // collecting until closing quote
           DISCARD_TO_NEWLINE,  // got a name, skipping to line break
           CHECK_NAME           // on a line break, maybe we got a name
         } State = LOOKING_FOR_NAME;

    while ( *Position != '\0' ) {

        switch ( State ) {

        case LOOKING_FOR_NAME:

            if ( strchr( " \t\r\n", *Position ) != NULL ) {

                Position++;
                }
            else if ( *Position == '"' ) {

                Position++;
                *FileName = Position;

                State = COLLECTING_QUOTED;
                }
            else {

                *FileName = Position;

                State = COLLECTING_NAME;
                }
            break;

        case COLLECTING_NAME:

            if ( strchr( "\r\n", *Position ) != NULL ) {

                State = CHECK_NAME;
                }

            else if ( strchr( ";+=,[] \"\t", *Position ) != NULL ) {

                *Position++ = '\0';

                State = DISCARD_TO_NEWLINE;
                }
            else {

                Position++;
                }
            break;

        case COLLECTING_QUOTED:

            if ( strchr( "\r\n", *Position ) != NULL ) {

                State = CHECK_NAME;
                }
            else if ( *Position == '"' ) {

                *Position++ = '\0';

                State = DISCARD_TO_NEWLINE;
                }
            else {

                Position++;
                }
            break;

        case DISCARD_TO_NEWLINE:

            if ( strchr( "\r\n", *Position ) != NULL ) {

                State = CHECK_NAME;
                }
            else {

                Position++;
                }
            break;

        case CHECK_NAME:

            *Position++ = '\0';

            if ( **FileName == '\0' ) {

                State = LOOKING_FOR_NAME;
                }
            else {

                _strlwr( *FileName );

                *BufferPosition = Position;

                return TRUE;
                }
            break;

            }
        }

    *BufferPosition = Position;

    switch ( State ) {

    case COLLECTING_NAME:
    case COLLECTING_QUOTED:
    case DISCARD_TO_NEWLINE:

        if ( **FileName != '\0' ) {

            _strlwr( *FileName );

            return TRUE;
            }
        }

    return FALSE;
    }


LPSTR
LoadFileListIntoBuffer(
    IN LPCSTR ListFileName
    )
    {
    HANDLE hList;
    DWORD  dwActual;
    DWORD  FileListLength;
    LPSTR  FileListBuffer = NULL;

    hList = CreateFile( ListFileName,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL
                        );

    if ( hList != INVALID_HANDLE_VALUE ) {

        FileListLength = GetFileSize( hList, NULL );

        if ( FileListLength != 0xFFFFFFFF ) {

            FileListBuffer = GlobalAlloc(GMEM_FIXED, FileListLength + 1 );
            if ( FileListBuffer != NULL ) {

                if ( ReadFile( hList,
                               FileListBuffer,
                               FileListLength,
                               &dwActual,
                               NULL ) &&
                     ( dwActual == FileListLength )) {

                    FileListBuffer[ FileListLength ] = '\0';
                    }
                else {

                    GlobalFree( FileListBuffer );
                    FileListBuffer = NULL;
                    }
                }
            }

        CloseHandle( hList );
        }

    if ( FileListBuffer == NULL ) {

        printf( "\nUnable to read list file \"%s\" (GLE=%d)\n", ListFileName, GetLastError() );
        }

    return FileListBuffer;
}


VOID
BuildUsingFileList(
    VOID
    )
    {
    WIN32_FILE_ATTRIBUTE_DATA FileInfo;
    unsigned                  PathLength;
    LPSTR                     FileListBuffer;
    LPSTR                     FileListPosition;
    LPSTR                     FileName;
    LPSTR                     BaseName;
    NAME_NODE               * Node;
    NEWFILE                 * NewFile;

    FileListBuffer = LoadFileListIntoBuffer( NewFileList );

    if ( FileListBuffer == NULL ) {

        ReturnCode = 1;
        return;
        }

    FileListPosition = FileListBuffer;

    while ( GetNextFileName( &FileListPosition, &FileName )) {

        if (( NewDirRootEnd + strlen( FileName ) - NewDirRoot ) >= sizeof( NewDirRoot )) {

            printf( "Invalid filename %s (too long)\n", FileName );
            ReturnCode = 1;
            }

        else {

            _strlwr( FileName );

            strcpy( NewDirRootEnd, FileName );

            if ( ! GetFileAttributesEx( NewDirRoot,
                                        GetFileExInfoStandard,
                                        &FileInfo ) ||
                (( FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )) {

                printf( "\nFile %s not found\n", NewDirRoot );
                ReturnCode = 1;
                }

            BaseName = strrchr( NewDirRootEnd, '\\' );

            if ( BaseName == NULL ) {

                BaseName = NewDirRootEnd;
                }
            else {

                BaseName++;
                }

            PathLength = BaseName - NewDirRootEnd;

            Node = NameRbInsert( &NewFileTree, BaseName );
            if ( Node == NULL ) {
                printf( "Out of memory\n" );
                ReturnCode = 1;
                break;
                }

            if ( Node->Context == NULL ) {

                Node->Context = SubAllocate( SubAllocator, sizeof( PRODUCTFILE ));
                if ( Node->Context == NULL ) {
                    printf( "Out of memory\n" );
                    ReturnCode = 1;
                    break;
                    }
                }

            *BaseName = 0;

            for ( NewFile = ((PRODUCTFILE *) Node->Context)->FirstNewFile;
                  NewFile != NULL;
                  NewFile = NewFile->Next ) {

                if ( strcmp( NewFile->Path, NewDirRootEnd ) == 0 ) {
                    break;
                    }
                }

            if ( NewFile == NULL ) {

                NewFile = SubAllocate( SubAllocator, sizeof( NEWFILE ) + PathLength );
                if ( NewFile == NULL ) {
                    printf( "Out of memory\n" );
                    ReturnCode = 1;
                    break;
                    }

                NewFile->FileNode = Node;
                NewFile->FileTime = FileInfo.ftLastWriteTime;
                NewFile->FileSize = FileInfo.nFileSizeLow;

                strcpy( NewFile->Path, NewDirRootEnd );

                NewFile->Next = ((PRODUCTFILE *) Node->Context)->FirstNewFile;
                ((PRODUCTFILE *) Node->Context)->FirstNewFile = NewFile;

                CountNewFiles++;
                }
            }
        }

    GlobalFree( FileListBuffer );

    *NewDirRootEnd = 0;

    if ( ReturnCode ) {

        return;
        }
    }


VOID
MyCreatePath(
    IN LPSTR FullFileName
    )
    {
    LPSTR p = FullFileName;

    while ( *p ) {
       if ( *p == '\\' ) {
            *p = 0;
            CreateDirectory( FullFileName, NULL );
            *p = '\\';
            }
        ++p;
        }
    }


VOID
DeleteOtherPatchFiles(
    IN LPCSTR Directory,
    IN LPCSTR FileName,
    IN LPCSTR DontDeleteSignature
    )
    {
    static WIN32_FIND_DATA FindData;
    CHAR NamePattern[ MAX_PATH ];
    CHAR DontDelete[ MAX_PATH ];
    HANDLE hFind;

    sprintf( DontDelete, "%s.%s._p", FileName, DontDeleteSignature );
    sprintf( NamePattern, "%s%s.*._p", Directory, FileName );

    hFind = FindFirstFile( NewDirRoot, &FindData );

    if ( hFind != INVALID_HANDLE_VALUE ) {

        do  {

            if ( ! ( FindData.dwFileAttributes & ( FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN ))) {

                if ( _stricmp( FindData.cFileName, DontDelete ) != 0 ) {

                    sprintf( NamePattern, "%s%s", Directory, FindData.cFileName );
                    DeleteFile( NamePattern );
                    }
                }
            }

        while ( FindNextFile( hFind, &FindData ));

        FindClose( hFind );
        }
    }


BOOL
GetFirstCandidate(
    LPSTR  OldDirRoot,
    LPSTR  OldDirFileName,
    LPCSTR NewFileName,
    CHAR  *CandidateHash
    )
    {
    if ( ! bSourceIsForest ) {

        strcpy( OldDirFileName, NewFileName );

        if (( GetFileAttributes( OldDirRoot ) & ( FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN )) == 0 ) {

            if ( ! bCaching ||
                 GetFilePatchSignature( OldDirRoot,
                                          PATCH_OPTION_NO_REBASE
                                        | PATCH_OPTION_NO_BINDFIX
                                        | PATCH_OPTION_NO_LOCKFIX
                                        | PATCH_OPTION_NO_RESTIMEFIX
                                        | PATCH_OPTION_SIGNATURE_MD5,
                                        NULL,
                                        0, NULL,    // no ignore
                                        0, NULL,    // no retain
                                        HASH_HEX_LENGTH,
                                        CandidateHash )) {

                return TRUE;
                }
            }

        return FALSE;   // no matching file in OldDirRoot
        }
    else {  // if bSourceIsForest is TRUE

        if ( OldDirTree == NULL ) {

            BuildOldDirTree( OldDirRoot );
            }

        NameHash = HashString( NewFileName );
        NameNode = HashTable[ NameHash % HASH_TABLE_WIDTH ];

        return( GetNextCandidate( OldDirRoot, OldDirFileName, NewFileName, CandidateHash ));
        }
    }


BOOL
DigestToHexString(
    IN  PMD5_HASH HashValue,
    OUT LPSTR     Buffer
    )
    {
    if (( HashValue->Word32[ 0 ] ||
          HashValue->Word32[ 1 ] ||
          HashValue->Word32[ 2 ] ||
          HashValue->Word32[ 3 ] ) &&
          HashToHexString( HashValue, Buffer )) {

        return TRUE;
        }

    return FALSE;
    }


BOOL
GetNextCandidate(
    LPSTR  OldDirRoot,
    LPSTR  OldDirFileName,
    LPCSTR NewFileName,
    CHAR  *CandidateHash
    )
    {
    UNREFERENCED_PARAMETER( OldDirRoot );

    while ( NameNode != NULL ) { // if bSourceIsForest == FALSE, then NameNode = NULL

        if (( NameNode->HashValue == NameHash ) &&
            ( _stricmp( NameNode->BaseName, NewFileName ) == 0 )) {

            *OldDirFileName = '\0';
            BuildDirPath( OldDirFileName, NameNode->Parent );
            strcat( OldDirFileName, NameNode->Name );

            if ( ! bCaching ||
                 DigestToHexString( &NameNode->Digest, CandidateHash ) ||
                 GetFilePatchSignature( OldDirRoot,
                                          PATCH_OPTION_NO_REBASE
                                        | PATCH_OPTION_NO_BINDFIX
                                        | PATCH_OPTION_NO_LOCKFIX
                                        | PATCH_OPTION_NO_RESTIMEFIX
                                        | PATCH_OPTION_SIGNATURE_MD5,
                                        NULL,
                                        0, NULL,    // no ignore
                                        0, NULL,    // no retain
                                        HASH_HEX_LENGTH,
                                        CandidateHash )) {

                NameNode = NameNode->Next;

                return TRUE;
                }
            }

        NameNode = NameNode->Next;
        }

    return FALSE;
    }


DWORD
HashString(
    LPCSTR String
    )
    {
    DWORD Result = 0;

    while ( *String ) {

        Result *= 137;
        Result ^= toupper( *String );
        String++;
        }

    return Result;
    }


VOID
BuildDirPath(
    LPSTR Buffer,
    PATHNODE *Parent
    )
    {

    //  recursively reconstitute path into buffer by gathering each component name from it's parent

    if ( Parent->Parent != NULL ) {

        BuildDirPath( Buffer, Parent->Parent );
        }

    strcat( Buffer, Parent->Name );
    }


VOID
BuildOldDirTree(
    LPSTR OldDirRoot
    )
    {
    OldDirTree = GlobalAlloc(GMEM_FIXED, sizeof( PATHNODE ));
    if ( OldDirTree != NULL ) {

        OldDirTree->Parent = NULL;
        OldDirTree->Name[0] = '\0';

        RecursiveBuildOldDirTree( OldDirRoot, OldDirTree );
        }
    }


VOID
RecursiveBuildOldDirTree(
    LPSTR OldDirPath,
    PATHNODE *Parent
    )
    //TODO filter these out using new name tree instead of loading the whole tree
    {
    LPSTR OldDirPathEnd = strchr( OldDirPath, 0 );
    static WIN32_FIND_DATA FindData;
    HANDLE hFind;

    if ( LoadTreeUsingManifest( OldDirRoot,
                                OldDirPathEnd,
                                Parent )) {

        return;
        }

    strcpy( OldDirPathEnd, "*" );

    hFind = FindFirstFile( OldDirPath, &FindData );

    if ( hFind == INVALID_HANDLE_VALUE ) {
        *OldDirPathEnd = '\0';
        printf( "\nFailed reading directory %s (GLE=%d)\a\n", OldDirPath, GetLastError() );
        ErrorCount++;
        ReturnCode = 1;
        }

    else {

        do {

            if (( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) &&
                ( ! ( FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )) &&
                ( _stricmp( FindData.cFileName, "symbols"  ) != 0 ) &&
                ( strcmp( FindData.cFileName, "."  ) != 0 ) &&
                ( strcmp( FindData.cFileName, ".." ) != 0 )) {

                PATHNODE *PathNode;

                strcpy( OldDirPathEnd, FindData.cFileName );
                strcat( OldDirPathEnd, "\\" );

                PathNode = GlobalAlloc( GMEM_FIXED, sizeof( PATHNODE ) + strlen( OldDirPathEnd ));
                if ( PathNode != NULL ) {

                    PathNode->Parent = Parent;
                    strcpy( PathNode->Name, OldDirPathEnd );

                    RecursiveBuildOldDirTree( OldDirPath, PathNode );
                    }
                }
            else if ( ! ( FindData.dwFileAttributes & ( FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN ))) {

                NAMENODE *NameNode;

                NameNode = GlobalAlloc( GMEM_FIXED, sizeof( NAMENODE ) + strlen( FindData.cFileName ));
                if ( NameNode != NULL ) {

                    DWORD HashIndex = HashString( FindData.cFileName );

                    NameNode->Parent = Parent;
                    NameNode->HashValue = HashIndex;
                    NameNode->BaseName = NameNode->Name;
                    memset( &NameNode->Digest, 0, sizeof( NameNode->Digest ));
                    strcpy( NameNode->Name, FindData.cFileName );

                    HashIndex %= HASH_TABLE_WIDTH;

                    NameNode->Next = HashTable[ HashIndex ];
                    HashTable[ HashIndex ] = NameNode;
                    }
                }
            }
            while ( FindNextFile( hFind, &FindData ));

            FindClose( hFind );
        }
    }


BOOL
LoadTreeUsingManifest(
    LPSTR RootPath,
    LPSTR RootPathEnd,
    PATHNODE * Parent
    )
    {
    BOOL            Success = FALSE;
    HANDLE          hManifest;
    MANIFEST_HEADER Header;
    MANIFEST_RECORD Record;
    CHAR            FileName[ MAX_PATH + 1 ];
    DWORD           dwActual;
    NAMENODE       *NameNode;
    DWORD           HashIndex;

    strcpy( RootPathEnd, MANIFEST_DATA_FILE_NAME );

    hManifest = CreateFile( RootPath,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_FLAG_SEQUENTIAL_SCAN,
                            NULL
                            );

    if ( hManifest == INVALID_HANDLE_VALUE ) {

        goto finished;
        }

    if (( ! ReadFile( hManifest,
                      &Header,
                      sizeof( Header ),
                      &dwActual,
                      NULL )) ||
        ( dwActual != sizeof( Header )) ||
        ( Header.dwSignature != MANIFEST_SIGNATURE ) ||
        ( Header.dwVersion != MANIFEST_VERSION )) {

        goto finished;
        }

    while ( Header.cTotalFiles-- ) {

        if (( ! ReadFile( hManifest,
                         &Record,
                         sizeof( Record ),
                         &dwActual,
                         NULL )) ||
            ( dwActual != sizeof( Record )) ||
            ( Record.cchFileName >= sizeof( FileName )) ||
            ( Record.cchFileName == 0 ) ||
            ( ! ReadFile( hManifest,
                         FileName,
                         Record.cchFileName,
                         &dwActual,
                         NULL )) ||
            ( dwActual != Record.cchFileName )) {

            goto finished;
            }

        //TODO store, filter out duplicate digests

        FileName[ Record.cchFileName ] = '\0';

        NameNode = GlobalAlloc( GMEM_FIXED, sizeof( NAMENODE ) + Record.cchFileName);
        if ( NameNode == NULL ) {

            goto finished;
            }

        NameNode->Parent = Parent;
        strcpy( NameNode->Name, FileName );
        NameNode->BaseName = strrchr( NameNode->Name, '\\' );
        if ( NameNode->BaseName == NULL ) {
            NameNode->BaseName = NameNode->Name;
            }
        else {
            NameNode->BaseName++;
            }
        memcpy( &NameNode->Digest, &Record.digest, sizeof( NameNode->Digest ));

        HashIndex = HashString( NameNode->BaseName );

        NameNode->HashValue = HashIndex;

        HashIndex %= HASH_TABLE_WIDTH;

        NameNode->Next = HashTable[ HashIndex ];
        HashTable[ HashIndex ] = NameNode;
        }

    Success = TRUE;

finished:

    if ( hManifest != INVALID_HANDLE_VALUE ) {

        CloseHandle( hManifest );
        }

    return( Success );
    }


VOID
AssociateNewFileDuplicates(
    VOID
    )
    {
#if 0
    NAME_TREE_ENUM   FileEnumerator;
    NAME_NODE      * Node;
    PRODUCTFILE    * ProductFile;
    NEWFILE        * NewFile;
    char             FileHash[ HASH_HEX_LENGTH ];

    Node = NameRbEnumFirst( &FileEnumerator, &NewFileTree );

    while ( Node != NULL ) {

        ProductFile = (PRODUCTFILE *) Node->Context;

        for ( NewFile = ProductFile->FirstNewFile; NewFile != NULL; NewFile = NewFile->Next ) {

            if ( NewFile->MD5Node == NULL ) {

                strcpy( NewDirRootEnd, NewFile->Path );


                if ( GetFilePatchSignature( NewDirRoot,
                                          PATCH_OPTION_NO_REBASE
                                        | PATCH_OPTION_NO_BINDFIX
                                        | PATCH_OPTION_NO_LOCKFIX
                                        | PATCH_OPTION_NO_RESTIMEFIX
                                        | PATCH_OPTION_SIGNATURE_MD5,
                                        NULL,
                                        0, NULL,    // no ignore
                                        0, NULL,    // no retain
                                        sizeof( FileHash ),
                                        FileHash )) {
                    }

                *NewDirRootEnd = 0;
                }

            printf( "new file %s%.*s\n", NewFile->Path, Node->NameLength, Node->Name );
            }

        Node = NameRbEnumNext( &FileEnumerator );
        }
#endif
    }


BOOL
AdjustPatchTime(
    LPCSTR   PatchPathName,     // full path to patch file
    FILETIME PatchTimeStamp     // time to store in patch
    )
{
    //  Used when an existing patch is dredged up from the cache.  It's possible that the patch
    //  has a timestamp which doesn't match the new file.  If so, update the patch's timestamp.
    //  Update the embedded file time in the patch header, if any.
    //  Returns TRUE if the file update is successful or no embedded time.
    //  Returns FALSE if the file update fails.  The patch file has probably
    //  been corrupted, and should be discarded.

    BOOL     Success = FALSE;
    HANDLE   hAllocator = NULL;
    HANDLE   FileHandle = INVALID_HANDLE_VALUE;
    HANDLE   MapHandle = NULL;
    PVOID    FileMapped = NULL;
    ULONG    FileSize, FileSizeHigh;
    PPATCH_HEADER_INFO HeaderInfo;
    ULONG    HeaderSize;
    ULONG    NewHeaderSize;
    DWORD    PatchFileCrc;

    __try {

        hAllocator = CreateSubAllocator( 0x2000, 0x10000 );
        if ( hAllocator == NULL ) {
            goto done;
            }

        //
        //  get a read/write mapping of the existing file
        //

        FileHandle = CreateFileA(
                        PatchPathName,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL
                        );

        if ( FileHandle == INVALID_HANDLE_VALUE ) {
            goto done;
            }

        FileSize = GetFileSize( FileHandle, &FileSizeHigh );

        if (( FileSizeHigh != 0 ) ||
            ( FileSize == 0 )) {
            goto done;
            }

        MapHandle = CreateFileMapping( FileHandle,
                                       NULL,
                                       PAGE_READWRITE,
                                       0,
                                       0,
                                       NULL
                                       );

        if ( MapHandle == NULL ) {
            goto done;
            }

        FileMapped = MapViewOfFile( MapHandle,
                                    FILE_MAP_ALL_ACCESS,
                                    0,
                                    0,
                                    0
                                    );

        if ( FileMapped == NULL ) {
            goto done;
            }

        //
        //  decode existing patch header
        //

        if ( ! DecodePatchHeader( FileMapped,
                                  FileSize,
                                  hAllocator,
                                  &HeaderSize,
                                  &HeaderInfo
                                  )) {
            goto done;
            }

        //
        //  if no internal timestamp, update time on patch file itself & exit
        //

        if ( HeaderInfo->OptionFlags & PATCH_OPTION_NO_TIMESTAMP ) {

            UnmapViewOfFile( FileMapped );
            FileMapped = NULL;

            CloseHandle( MapHandle );
            MapHandle = NULL;

            SetFileTime( FileHandle, NULL, NULL, &PatchTimeStamp );

            Success = TRUE;
            goto done;
            }

        //
        //  if internal timestamp is current, leave it alone
        //

        if ( HeaderInfo->NewFileTime == FileTimeToUlongTime( &PatchTimeStamp )) {
            Success = TRUE;
            goto done;
            }

        //
        //  update internal timestamp
        //

        HeaderInfo->NewFileTime = FileTimeToUlongTime( &PatchTimeStamp );

        //
        //  try to re-encode header.  If size changes, give up (and rebuild the patch)
        //

        NewHeaderSize = EncodePatchHeader( HeaderInfo, FileMapped );

        if (NewHeaderSize != HeaderSize ) {
            goto done;
            }

        //
        //  update CRC32 on patch file
        //

        PatchFileCrc = Crc32( 0xFFFFFFFF, FileMapped, FileSize - sizeof( ULONG ));

        *(UNALIGNED ULONG *)( (BYTE *) FileMapped + FileSize - sizeof( ULONG )) = PatchFileCrc;

        FlushViewOfFile( FileMapped, 0 );
        UnmapViewOfFile( FileMapped );
        FileMapped = NULL;

        CloseHandle( MapHandle );
        MapHandle = NULL;

        Success = TRUE;
        }

    __except ( EXCEPTION_EXECUTE_HANDLER ) {

        }

done:

    if ( FileMapped != NULL ) {
        UnmapViewOfFile( FileMapped );
        }

    if ( MapHandle != NULL ) {
        CloseHandle( MapHandle );
        }

    if ( FileHandle != INVALID_HANDLE_VALUE ) {
        CloseHandle( FileHandle );
        }

    if ( hAllocator != NULL ) {
        DestroySubAllocator( hAllocator );
        }

    return Success;
}
