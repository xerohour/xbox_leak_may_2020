
/*

    sym7.cpp  - Pdb symbol extractor including BBT OMAP translation and
                layout of common PE structures.  Primarily designed for
                MS patch engine, but can also be used to examine image
                layout since the output representation is fully expanded
                flat text.

    Author: Tom McGuire (tommcg)

    Original version written November-December 2000.

    Copyright (C) Microsoft, 2000-2001.  All rights reserved.

*/


#pragma warning( disable: 4201 )    // nameless struct/union
#pragma warning( disable: 4214 )    // bitfields other than int

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

typedef unsigned __int64 QWORD;
typedef const BYTE * PCBYTE;

#include <vcver.h>
#include <pdb.h>
#include <cv.h>
#include <undname.h>

#include <delayimp.h>
#include <stdlib.h>
#include <stdio.h>

const unsigned PsymMajorVersion = 1;
const unsigned PsymMinorVersion = 1;


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
#define countof( array ) ( sizeof( array ) / sizeof( array[ 0 ] ))
#endif

#ifdef DEBUG

#define DEBUGCODE( x ) x
#define ASSERT( x ) ((x)?(1):AssertionFailure( #x, __FILE__, __LINE__ ))

void
__stdcall
AssertionFailure(
    const char *Expression,
    const char *SourceFile,
    unsigned    SourceLine
)
{
    fflush( stdout );
    fprintf( stderr, "SYM7: ASSERTION FAILED: %s (%u)\n%s\n", SourceFile, SourceLine, Expression );
    fflush( stderr );
    __debugbreak();
    exit( 1 );
}

#endif /* DEBUG */


#ifndef ASSERT
#define ASSERT( x )
#endif

#ifndef DEBUGCODE
#define DEBUGCODE( x )
#endif

#include "nametable.h"

#include "md5.c"


PVOID
MapViewOfFileReadOnly(
    IN  LPCSTR FileName,
    OUT ULONG *FileSize
    );



struct OMAP
{
    ULONG Rva1;
    ULONG Rva2;
};


struct SYMNODE
{
    ULONG     RvaInImg;
    ULONG     RvaOrig;
    ULONG     SymSize;
    WORD      SymType;
    WORD      SymSort;
    NAMENODE* SymNameId;
    NAMENODE* OrgNameId;
    SYMNODE*  ParentSym;
    SYMNODE*  FirstChild;
    SYMNODE*  NextSibling;
};


VOID
__fastcall
QsortSymTableByRvaInImg(
    SYMNODE** LowerBound,
    SYMNODE** UpperBound
    );

VOID
__fastcall
QsortSymTableByRvaInSrc(
    SYMNODE** LowerBound,
    SYMNODE** UpperBound
    );

unsigned
__fastcall
FirstSrcSymTableIndexForRva(
    SYMNODE** SymTableSortedByOrig,
    ULONG     SymTableCount,
    ULONG     RvaInSrc
    );


#ifdef SYMCOUNTER

ULONG SymTypeCounter[ 0x4000 ];

#endif


class PdbSymbolHandler {

public:

    NAMETABLE   NameTable;

    SYMNODE**   PrimarySymTable;
    ULONG       PrimarySymTableCount;

    SYMNODE**   FragmentSymTable;
    ULONG       FragmentSymTableCount;
    ULONG       FragmentSymTableMax;

    #define     SymTableMax 0x400000        // 4 million entries, 16MB alloc

    PBYTE       ExeFileMapped;
    ULONG       ExeFileSize;

    PBYTE       ResourceBaseMapped;
    ULONG       ResourceBaseRva;
    SYMNODE*    ResourceRootSym;

    PIMAGE_NT_HEADERS     NtHeader;
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG                 SectionCount;

    PDB*        pdb;                        // PDB handle
    DBI*        dbi;                        // DBI handle

    CVSEGMAP*   cvSegMap;
    ULONG       nSegMap;

    ULONG*      pImgSegToRvaMap;
    ULONG       nImgSegToRvaMap;

    ULONG*      pSrcSegToRvaMap;
    ULONG       nSrcSegToRvaMap;

    OMAP*       pOmapToSrc;                 // map from img address to symbol
    OMAP*       pOmapToImg;                 // map from symbol address to img

    ULONG       nOmapToSrc;                 // count of pOmapToSrc entries
    ULONG       nOmapToImg;                 // count of pOmapToImg entries

    BOOL        DbgOmap;
    BOOL        PdbOmap;

    BOOL        AnyExecutableSections;
    BOOL        AnyWritableSections;
    BOOL        AnyDebugDirectories;

    ULONG       CountOfModSyms;
    ULONG       CountOfPublics;
    ULONG       CountOfGlobals;

    ULONG       ExeTime;
    QWORD       ExeVersion;

    MD5_HASH    ExeFileMD5;
    MD5_HASH    DbgFileMD5;
    MD5_HASH    PdbFileMD5;

    ULONG       PdbFileSize;
    ULONG       PdbTime;

    ULONG       DbgFileSize;
    ULONG       DbgTime;

    ULONG       SymConfidence;              // 0..100
    ULONG       SymConfThreshold;
    ULONG       SymsMatched;
    ULONG       SymsUnmatch;
    ULONG       TranslationCount;
    DWORD*      TranslationArray;

    LPSTR       ExeName;                    // points to filename in ExePath
    LPSTR       PdbName;                    // points to filename in PdbPath
    LPSTR       DbgName;                    // points to filename in DbgPath
    LPSTR       DbgNameInExe;               // if not null, "dll\foo.dbg"

    CHAR        ExePath[ PDB_MAX_PATH ];
    CHAR        PdbPath[ PDB_MAX_PATH ];
    CHAR        DbgPath[ PDB_MAX_PATH ];

    CHAR        szNewSymName[ 16 ];

    BOOL OpenExe( LPCSTR ExeFile );
    BOOL OpenPdb( LPCSTR ExeFile, LPCSTR SymbolPath );
    BOOL LoadSymbols( ULONG ConfidenceThreshold = 50  );

    ULONG TranslateOmapFromSrcToImg( ULONG Rva );
    ULONG TranslateOmapFromImgToSrc( ULONG Rva );

    PdbSymbolHandler()
    {
        ExeFileMapped   = NULL;
        ExeFileSize     = 0;
        ResourceBaseMapped = NULL;
        ResourceBaseRva = 0;
        ResourceRootSym = NULL;
        PrimarySymTable = NULL;
        PrimarySymTableCount = 0;
        FragmentSymTable = NULL;
        FragmentSymTableCount = 0;
        FragmentSymTableMax = 0;
        pdb             = NULL;
        dbi             = NULL;
        cvSegMap        = NULL;
        nSegMap         = 0;
        pImgSegToRvaMap = NULL;
        nImgSegToRvaMap = 0;
        pSrcSegToRvaMap = NULL;
        nSrcSegToRvaMap = 0;
        pOmapToSrc      = NULL;
        pOmapToImg      = NULL;
        nOmapToSrc      = 0;
        nOmapToImg      = 0;
        DbgOmap         = FALSE;
        PdbOmap         = FALSE;
        AnyExecutableSections = FALSE;
        AnyWritableSections   = FALSE;
        AnyDebugDirectories   = FALSE;
        CountOfModSyms  = 0;
        CountOfPublics  = 0;
        CountOfGlobals  = 0;
        ExeTime         = 0;
        ExeVersion      = 0;
        ExeFileMD5.Word32[ 0 ] = 0;
        PdbFileMD5.Word32[ 0 ] = 0;
        DbgFileMD5.Word32[ 0 ] = 0;
        PdbTime         = 0;
        PdbFileSize     = 0;
        DbgTime         = 0;
        DbgFileSize     = 0;
        TranslationCount = 0;
        TranslationArray = NULL;
        ExeName         = NULL;
        PdbName         = NULL;
        DbgName         = NULL;
        DbgNameInExe    = NULL;
        *ExePath        = 0;
        *PdbPath        = 0;
        *DbgPath        = 0;
        RootPublicSym   = NULL;
        RootGlobalSym   = NULL;
        RootModuleSym   = NULL;
        RootPeSym       = NULL;
        SymConfidence   = 0;
        SymConfThreshold = 0;
        SymsMatched = 0;
        SymsUnmatch = 0;

    };

#ifdef DONTCOMPILE  // BUGBUG: Lot of stuff to tear down not implemented here

   ~PdbSymbolHandler()
    {
        //
        //  BUGBUG: Incomplete.
        //

        delete cvSegMap;

        if ( dbi )
        {
            dbi->Close();
        }

        if ( pdb )
        {
            pdb->Close();
        }

        if ( PrimarySymTable )
        {
            VirtualFree( PrimarySymTable, 0, MEM_RELEASE );
        }

        if ( ExeFileMapped )
        {
            UnmapViewOfFile( ExeFileMapped );
        }

        if ( TranslationArray )
        {
            delete TranslationArray;
        }

    };

#endif

    ULONG BuildScopedSymbolName( LPSTR Buffer, SYMNODE* Sym )
    {
        if ( Sym )
        {
            ULONG Offset = BuildScopedSymbolName( Buffer, Sym->ParentSym );

            memcpy( Buffer + Offset, Sym->SymNameId->Name, Sym->SymNameId->Length );
            Offset += Sym->SymNameId->Length;
            Buffer[ Offset++ ] = '\\';
            Buffer[ Offset ] = 0;

            return Offset;
        }

        return 0;
    }


    VOID DumpSym( SYMNODE* SymNode )
    {
        CHAR szParentName[ 1024 ];

        *szParentName = 0;

        BuildScopedSymbolName( szParentName, SymNode->ParentSym );

        printf(
            "%06X (orig=%06X) (size=%4u) (type=%04X) %s%s\n",
            SymNode->RvaInImg,
            SymNode->RvaOrig,
            SymNode->SymSize,
            SymNode->SymType,
            szParentName,
            SymNode->SymNameId->Name
            );
    }


    VOID DumpSyms( VOID )
    {
        printf( "Symbols (%u):\n", PrimarySymTableCount );

        for ( unsigned i = 0; i < PrimarySymTableCount; i++ )
        {
            DumpSym( PrimarySymTable[ i ] );
        }

        printf( "\n" );

    }


    VOID DumpOmapToImg( VOID )
    {
        printf( "\nOmapToImg:\n" );

        for ( unsigned i = 0; i < nOmapToImg; i++ )
        {
            printf(
                "%08X %08X\n",
                pOmapToImg[ i ].Rva1,
                pOmapToImg[ i ].Rva2
                );
        }

        printf( "\n" );
    }


    VOID DumpOmapToSrc( VOID )
    {
        printf( "\nOmapToSrc:\n" );

        for ( unsigned i = 0; i < nOmapToSrc; i++ )
        {
            printf(
                "%08X %08X\n",
                pOmapToSrc[ i ].Rva1,
                pOmapToSrc[ i ].Rva2
                );
        }

        printf( "\n" );
    }



//private:

    VOID MyGetFileVersionInfo( void );
    BOOL LoadOmapFromDbg( void );
    BOOL ValidateOmap( void );
    VOID LoadModuleSymbols( void );
    VOID LoadImageDerivedSymbols( void );
    ULONG __fastcall ImageRvaToFileOffset( ULONG RvaInImg );
    PVOID __fastcall ImageRvaToMappedAddress( ULONG RvaInImg );
    VOID __fastcall ProcessCvSym( CVSYM* pcvsym );
    VOID BubbleSortPrimarySymTable( void );
    SYMNODE*      CreateSymbol( SYMNODE* ParentSym, LPCSTR SymName, ULONG SymSort, ULONG SymType, ULONG SymSize, ULONG RvaInImg, ULONG RvaOrig );
    SYMNODE*  AddPrimarySymbol( SYMNODE* ParentSym, LPCSTR SymName, ULONG SymSort, ULONG SymType, ULONG SymSize, ULONG RvaInImg, ULONG RvaOrig );
    SYMNODE*  AddPrimarySymbol( SYMNODE* ParentSym, LPCSTR SymName, ULONG SymSort, ULONG SymType, ULONG SymSize, ULONG RvaInImg );
    SYMNODE* AddFragmentSymbol( SYMNODE* ParentSym, LPCSTR SymName, ULONG SymSort, ULONG SymType, ULONG SymSize, ULONG RvaInImg, ULONG RvaOrig );

    VOID
    __fastcall
    LoadResourceSymsRecursive(
        IN PIMAGE_RESOURCE_DIRECTORY ResourceDir,
        IN SYMNODE* ParentSym,
        IN LPSTR    pszBuffer
        );

    VOID
    __fastcall
    ConvertDollarSymbols(
        SYMNODE* ParentSym
        );

    VOID
    DetermineSymbolConfidence(
        VOID
        );


    SYMNODE* NestedScopeParentSym;

    SYMNODE* RootPublicSym;
    SYMNODE* RootGlobalSym;
    SYMNODE* RootModuleSym;
    SYMNODE* RootPeSym;

};


VOID PdbSymbolHandler::MyGetFileVersionInfo( void )
{
    DWORD Blah;
    DWORD Size = GetFileVersionInfoSize( ExePath, &Blah );

    if ( Size )
    {
        PBYTE Buffer = new BYTE[ Size ];

        if ( Buffer )
        {
            if ( GetFileVersionInfo( ExePath, 0, Size, Buffer ))
            {
                VS_FIXEDFILEINFO* VersionInfo;

                if ( VerQueryValue( Buffer, "\\", (PVOID*)&VersionInfo, (PUINT)&Blah ))
                {
                    ExeVersion = ((((QWORD) VersionInfo->dwFileVersionMS ) << 32 ) |
                                   ((QWORD) VersionInfo->dwFileVersionLS ));
                }

                DWORD* Translation;

                if ( VerQueryValue( Buffer, "\\VarFileInfo\\Translation", (PVOID*)&Translation, (PUINT)&Size ))
                {
                    TranslationCount = ( Size / sizeof( DWORD ));
                    TranslationArray = new DWORD[ TranslationCount ];

                    memcpy( TranslationArray, Translation, TranslationCount * sizeof( DWORD ));
                }
            }

            delete Buffer;
        }
    }

}


BOOL
PdbSymbolHandler::OpenExe(
    LPCSTR ExeFile
    )
{
    GetFullPathName( ExeFile, PDB_MAX_PATH, ExePath, &ExeName );

    ExeFileMapped = (PBYTE) MapViewOfFileReadOnly( ExeFile, &ExeFileSize );

    if ( ExeFileMapped == NULL )
    {
        SetLastError( ERROR_FILE_NOT_FOUND );
        return FALSE;
    }

    BOOL Success = FALSE;

    __try
    {
        PIMAGE_DOS_HEADER DosHeader;

        DosHeader = (PIMAGE_DOS_HEADER) ExeFileMapped;
        NtHeader  = (PIMAGE_NT_HEADERS) ExeFileMapped;

        if ( DosHeader->e_magic == IMAGE_DOS_SIGNATURE )
        {
            NtHeader = (PIMAGE_NT_HEADERS)( ExeFileMapped + DosHeader->e_lfanew );
        }

        if ( NtHeader->Signature != IMAGE_NT_SIGNATURE )
        {
            __leave;
        }

        ExeTime          = NtHeader->FileHeader.TimeDateStamp;
        SectionHeader    = IMAGE_FIRST_SECTION( NtHeader );
        SectionCount     = NtHeader->FileHeader.NumberOfSections;
        nImgSegToRvaMap  = SectionCount;
        pImgSegToRvaMap  = new ULONG[ nImgSegToRvaMap ];

        for ( unsigned i = 0; i < SectionCount; i++ )
        {
            pImgSegToRvaMap[ i ] = SectionHeader[ i ].VirtualAddress;

            if ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE )
            {
                AnyExecutableSections = TRUE;
            }

            if ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_WRITE )
            {
                AnyWritableSections = TRUE;
            }
        }

        __try
        {

            ULONG DebugDirectoryRva   = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_DEBUG ].VirtualAddress;
            ULONG DebugDirectorySize  = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_DEBUG ].Size;
            ULONG DebugDirectoryCount = DebugDirectorySize / sizeof( IMAGE_DEBUG_DIRECTORY );

            if (( DebugDirectoryRva ) && ( DebugDirectoryCount )) {

                AnyDebugDirectories = TRUE;

                for ( unsigned i = 0; i < SectionCount; i++ )
                {
                    if (( DebugDirectoryRva >= SectionHeader[ i ].VirtualAddress ) &&
                        ( DebugDirectoryRva <  SectionHeader[ i ].VirtualAddress + SectionHeader[ i ].SizeOfRawData ))
                    {
                        ULONG OffsetInSection = DebugDirectoryRva - SectionHeader[ i ].VirtualAddress;

                        PIMAGE_DEBUG_DIRECTORY DebugDirectory;

                        DebugDirectory = (PIMAGE_DEBUG_DIRECTORY)
                                         ( ExeFileMapped
                                           + SectionHeader[ i ].PointerToRawData
                                           + OffsetInSection
                                         );

                        for ( unsigned j = 0; j < DebugDirectoryCount; j++ )
                        {
                            if ( DebugDirectory[ j ].Type == IMAGE_DEBUG_TYPE_MISC )
                            {
                                PIMAGE_DEBUG_MISC DebugMiscData = (PIMAGE_DEBUG_MISC)( ExeFileMapped + DebugDirectory[ j ].PointerToRawData );

                                if ( DebugMiscData->DataType == IMAGE_DEBUG_MISC_EXENAME )
                                {
                                    if ( ! DebugMiscData->Unicode )
                                    {
                                        DbgNameInExe = _strdup( (LPCSTR) DebugMiscData->Data );

                                        if ( DbgNameInExe )
                                        {
                                            _strlwr( DbgNameInExe );
                                        }

                                        break;
                                    }
                                }
                            }
                        }

                        break;
                    }
                }
            }
        }
        __except( EXCEPTION_EXECUTE_HANDLER ) {}

        MyGetFileVersionInfo();

        ComputeCompleteMD5( ExeFileMapped, ExeFileSize, &ExeFileMD5 );

        Success = TRUE;
    }

    __except( EXCEPTION_EXECUTE_HANDLER ) {}

    if ( ! Success )
    {
        SetLastError( ERROR_BAD_EXE_FORMAT );
        return FALSE;
    }

    return TRUE;
}


BOOL
PdbSymbolHandler::OpenPdb(
    LPCSTR ExeFile,
    LPCSTR SymbolPath
    )
{
    CHAR TmpPath[ PDB_MAX_PATH ];
    CHAR ErrText[ cbErrMax ];

    *TmpPath = 0;
    *ErrText = 0;

    EC ec = 0;

    BOOL Success = PDBOpenValidate3(
                      ExeFile,
                      SymbolPath,
                      &ec,
                      ErrText,
                      TmpPath,
                      NULL,
                      NULL,
                      &pdb
                      );

    if ( ! Success )
    {
        SetLastError( ERROR_FILE_NOT_FOUND );
        return FALSE;
    }

    if ( *TmpPath )
    {
        GetFullPathName( TmpPath, PDB_MAX_PATH, DbgPath, &DbgName );
    }

    pdb->QueryPDBName( TmpPath );
    GetFullPathName( TmpPath, PDB_MAX_PATH, PdbPath, &PdbName );

    PdbFileSize = 0;

    PBYTE PdbFileMapped = (PBYTE) MapViewOfFileReadOnly( PdbPath, &PdbFileSize );

    if ( PdbFileMapped )
    {
        __try
        {
            ComputeCompleteMD5( PdbFileMapped, PdbFileSize, &PdbFileMD5 );
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            ZeroMemory( &PdbFileMD5, sizeof( PdbFileMD5 ));
        }

        UnmapViewOfFile( PdbFileMapped );
    }

    PdbTime = pdb->QuerySignature();    // BUGBUG: assuming this is timestamp

    Success = pdb->OpenDBI( NULL, pdbRead, &dbi );

    if ( ! Success )
    {
        SetLastError( ERROR_OPEN_FAILED );
        return FALSE;
    }

    Dbg* dbg;

    if ( dbi->OpenDbg( dbgtypeOmapToSrc, &dbg ))
    {
        nOmapToSrc = dbg->QuerySize();

        if ( nOmapToSrc )
        {
            pOmapToSrc = new OMAP[ nOmapToSrc ];
            dbg->QueryNext( nOmapToSrc, pOmapToSrc );
        }

        dbg->Close();
    }

    if ( dbi->OpenDbg( dbgtypeOmapFromSrc, &dbg ))
    {
        nOmapToImg = dbg->QuerySize();

        if ( nOmapToImg )
        {
            pOmapToImg = new OMAP[ nOmapToImg ];
            dbg->QueryNext( nOmapToImg, pOmapToImg );
        }

        dbg->Close();
    }

    PdbOmap = nOmapToSrc || nOmapToImg;

    if ((( nOmapToSrc == 0 ) || ( nOmapToImg == 0 )) && ( *DbgPath != 0 ))
    {
        //
        //  No OMAP in .pdb, attempt to find OMAP in .dbg
        //

        LoadOmapFromDbg();
    }

    if ( nOmapToImg != 0 )
    {
        //
        //  Need original section map.  The original section headers might
        //  be in the .pdb as dbgtypeSectionHdrOrig, otherwise reconstruct
        //  from QuerySecMap section sizes and section alignment.
        //

        if ( dbi->OpenDbg( dbgtypeSectionHdrOrig, &dbg ))
        {
            ULONG nSrcSectHdr = dbg->QuerySize();

            if ( nSrcSectHdr != 0 )
            {
                PIMAGE_SECTION_HEADER pSrcSectHdr = new IMAGE_SECTION_HEADER[ nSrcSectHdr ];

                pSrcSegToRvaMap = new ULONG[ nSrcSectHdr ];
                nSrcSegToRvaMap = nSrcSectHdr;

                dbg->QueryNext( nSrcSectHdr, pSrcSectHdr );

                for ( unsigned i = 0; i < nSrcSectHdr; i++ )
                {
                    pSrcSegToRvaMap[ i ] = pSrcSectHdr[ i ].VirtualAddress;
                }

                DEBUGCODE( printf( "Loaded dbgtypeSectionHdrOrig from .pdb (%u sections)\n", nSrcSectHdr ));
            }

            dbg->Close();
        }

        if ( nSrcSegToRvaMap == 0 )
        {
            //
            //  Must reconstruct original segment addresses.
            //

            long cbSegMap = 0;

            dbi->QuerySecMap( NULL, &cbSegMap );

            ASSERT( cbSegMap != 0 );

            if ( cbSegMap != 0 )
            {
                CVSEGMAP* cvSegMap = (CVSEGMAP*) new BYTE[ cbSegMap ];

                dbi->QuerySecMap( (PBYTE) cvSegMap, &cbSegMap );

                ULONG nSegMap = ( cbSegMap - sizeof( DWORD )) / sizeof( CVSEG );

                pSrcSegToRvaMap = new ULONG[ nSegMap ];

                ULONG RvaOfNextSection = pOmapToImg[ 0 ].Rva1;

                ASSERT( NtHeader->OptionalHeader.SectionAlignment != 0 );
                ASSERT(( NtHeader->OptionalHeader.SectionAlignment & ( NtHeader->OptionalHeader.SectionAlignment - 1 )) == 0 );
                ASSERT(( RvaOfNextSection & ( NtHeader->OptionalHeader.SectionAlignment - 1 )) == 0 );

                for ( nSrcSegToRvaMap = 0; nSrcSegToRvaMap < nSegMap; nSrcSegToRvaMap++ )
                {
                    if ( cvSegMap->rgcvseg[ nSrcSegToRvaMap ].wFlags.fAbs )
                    {
                        break;
                    }

                    ASSERT( cvSegMap->rgcvseg[ nSrcSegToRvaMap ].isegPhysical == nSrcSegToRvaMap + 1 );
                    ASSERT( cvSegMap->rgcvseg[ nSrcSegToRvaMap ].ib == 0 );

                    pSrcSegToRvaMap[ nSrcSegToRvaMap ] = RvaOfNextSection;

                    RvaOfNextSection += ROUNDUP2( cvSegMap->rgcvseg[ nSrcSegToRvaMap ].cb, NtHeader->OptionalHeader.SectionAlignment );
                }
            }
        }
    }

    if ( nSrcSegToRvaMap == 0 )
    {
        nSrcSegToRvaMap = nImgSegToRvaMap;
        pSrcSegToRvaMap = pImgSegToRvaMap;
    }

    ValidateOmap();

    return TRUE;
}


BOOL
PdbSymbolHandler::LoadOmapFromDbg( void )
{
    PBYTE DbgFileMapped = (PBYTE) MapViewOfFileReadOnly( DbgPath, &DbgFileSize );
    BOOL  Success = FALSE;

    __try {

        PIMAGE_SEPARATE_DEBUG_HEADER DebugFileHeader;
        PIMAGE_DEBUG_DIRECTORY       DebugDirectory;
        ULONG                        DebugDirectoryCount;

        DebugFileHeader = (PIMAGE_SEPARATE_DEBUG_HEADER) DbgFileMapped;

        if ( DebugFileHeader->Signature != IMAGE_SEPARATE_DEBUG_SIGNATURE ) {
            __leave;
            }

        DbgTime = DebugFileHeader->TimeDateStamp;

        __try
        {
            ComputeCompleteMD5( DbgFileMapped, DbgFileSize, &DbgFileMD5 );
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            ZeroMemory( &DbgFileMD5, sizeof( DbgFileMD5 ));
        }

        DebugDirectory = (PIMAGE_DEBUG_DIRECTORY)(
                            DbgFileMapped
                              + sizeof( IMAGE_SEPARATE_DEBUG_HEADER )
                              + DebugFileHeader->NumberOfSections * sizeof( IMAGE_SECTION_HEADER )
                              + DebugFileHeader->ExportedNamesSize
                            );

        DebugDirectoryCount = DebugFileHeader->DebugDirectorySize / sizeof( IMAGE_DEBUG_DIRECTORY );

        while ( DebugDirectoryCount-- ) {

            switch ( DebugDirectory->Type )
            {
                case IMAGE_DEBUG_TYPE_OMAP_TO_SRC:
                case IMAGE_DEBUG_TYPE_OMAP_FROM_SRC:

                    Success = TRUE;
                    DbgOmap = TRUE;

                    PBYTE p = new BYTE[ DebugDirectory->SizeOfData ];
                    ULONG n = DebugDirectory->SizeOfData / sizeof( OMAP );

                    CopyMemory(
                        p,
                        DebugDirectory->PointerToRawData + DbgFileMapped,
                        DebugDirectory->SizeOfData
                        );

                    if ( DebugDirectory->Type == IMAGE_DEBUG_TYPE_OMAP_TO_SRC )
                    {
                        pOmapToSrc = (OMAP*)p;
                        nOmapToSrc = n;
                    }
                    else
                    {
                        pOmapToImg = (OMAP*)p;
                        nOmapToImg = n;
                    }
            }

            DebugDirectory++;
        }
    }

    __except( EXCEPTION_EXECUTE_HANDLER ) {}

    UnmapViewOfFile( DbgFileMapped );

    return Success;
}


PVOID
MapViewOfFileReadOnly(
    IN  LPCSTR FileName,
    OUT ULONG *FileSize
    )
{
    HANDLE FileHandle;
    HANDLE MapHandle;
    PVOID  MapBase = NULL;
    ULONG  LocalSize = 0;

    FileHandle = CreateFile(
                     FileName,
                     GENERIC_READ,
                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                     NULL,
                     OPEN_EXISTING,
                     FILE_FLAG_RANDOM_ACCESS,
                     NULL
                     );

    if ( FileHandle != INVALID_HANDLE_VALUE )
    {

        LocalSize = GetFileSize( FileHandle, NULL );

        MapHandle = CreateFileMapping(
                        FileHandle,
                        NULL,
                        PAGE_READONLY,
                        0,
                        0,
                        NULL
                        );

        CloseHandle( FileHandle );

        if ( MapHandle != NULL )
        {
            MapBase = MapViewOfFile(
                          MapHandle,
                          FILE_MAP_READ,
                          0,
                          0,
                          0
                          );

            CloseHandle( MapHandle );
        }
    }

    if (( MapBase ) && ( FileSize ))
    {
        *FileSize = LocalSize;
    }

    return MapBase;
}


BOOL
PdbSymbolHandler::LoadSymbols(
    ULONG ConfidenceThreshold
    )
{
    SymConfThreshold = ConfidenceThreshold;

    ASSERT( PrimarySymTable == NULL );

    PrimarySymTable = (SYMNODE**) VirtualAlloc(
                                      NULL,
                                      SymTableMax * sizeof( PVOID ),   // 4MB!!!
                                      MEM_COMMIT,
                                      PAGE_READWRITE
                                      );

    if ( PrimarySymTable == NULL )
    {
        return FALSE;
    }

    LoadImageDerivedSymbols();

    SymConfidence = 100;

    if ( dbi )
    {

        ULONG StartingCount;
        GSI* gsi;

        if ( dbi->OpenPublics( &gsi ))
        {
            RootPublicSym = CreateSymbol( NULL, "Public", 2000, 0, 0, 0, 0 );

            NestedScopeParentSym = RootPublicSym;

            StartingCount = PrimarySymTableCount;

            PBYTE pbSym = NULL;

            while (( pbSym = gsi->NextSym( pbSym )) != NULL )
            {
                ProcessCvSym( (CVSYM*) pbSym );
            }

            gsi->Close();

            CountOfPublics = PrimarySymTableCount - StartingCount;
        }

        if ( dbi->OpenGlobals( &gsi ))
        {
            RootGlobalSym = CreateSymbol( NULL, "Global", 3000, 0, 0, 0, 0 );

            NestedScopeParentSym = RootGlobalSym;

            StartingCount = PrimarySymTableCount;

            PBYTE pbSym = NULL;

            while (( pbSym = gsi->NextSym( pbSym )) != NULL )
            {
                ProcessCvSym( (CVSYM*) pbSym );
            }

            gsi->Close();

            CountOfGlobals = PrimarySymTableCount - StartingCount;
        }

        RootModuleSym = CreateSymbol( NULL, "Module", 4000, 0, 0, 0, 0 );

        NestedScopeParentSym = RootModuleSym;

        StartingCount = PrimarySymTableCount;

        LoadModuleSymbols();

        CountOfModSyms = PrimarySymTableCount - StartingCount;

        //
        //  Now that we have pdb basic symbols and image derived symbols loaded,
        //  we can compare import and export names to their pdb symbol names
        //  to determine if the symbol names appear to match.
        //

        DetermineSymbolConfidence();

        if ( SymConfidence < SymConfThreshold )
        {
            fprintf( stderr, "SYM7: Symbol confidence failure (%u%%)\n", SymConfidence );
            return FALSE;
        }

        if ( SymConfidence < 50 )
        {
            fprintf( stderr, "SYM7: Warning: symbol confidence only %u%%\n", SymConfidence );
        }

        //
        //  Special consideration for $-prefixed symbols here with respect to
        //  symbols use for patching.  Since the compiler generates $xxx labels
        //  at compile time that are not symbolic with respect to source, they
        //  can actually cause the patch matcher to match the wrong things since
        //  the same $-symbol can reference different things in two different
        //  versions of same binary.  The problem is that the $E and $L symbols
        //  have globally incremental values as part of the label.  We've asked
        //  the compiler team to produce "real" symbols for these, but in the
        //  meantime we convert $Exxx symbols to $EE_yyy where yyy is a counter
        //  based on the parent scope symbol.  This will hopefully match up the
        //  $E and $L symbols for modules where no new $E or $L symbols have been
        //  introduced or deleted, but will cause the matches to be wrong when
        //  new $E or $L symbols have been introduced or deleted.
        //
        //  Now we want to convert symbols named $L12345 to $LL_1 etc within each
        //  parent symbol scope ascending by original RVA.
        //

        ConvertDollarSymbols( RootGlobalSym );
        ConvertDollarSymbols( RootModuleSym );

        if ( nOmapToSrc )
        {
            //
            //  Create symbols for separated BBT fragments.
            //
            //  Both the pOmapToSrc and SymTable are sorted by image RVA ascending,
            //  but we want to be careful to stop at the end of the original sym
            //  table "primary" entries so we don't attempt to name fragments as
            //  children of other fragments.
            //

            FragmentSymTable = PrimarySymTable + PrimarySymTableCount;
            FragmentSymTableCount = 0;
            FragmentSymTableMax = SymTableMax - PrimarySymTableCount;

            //
            //  Need primary sym table sorted by original address at this point
            //  so we can lookup symbols by original address.  We don't need to
            //  fine-tune bubble-sort the names within matching RVAs for these
            //  lookups.
            //

            ASSERT( PrimarySymTableCount > 1 );

            QsortSymTableByRvaInSrc(
                &PrimarySymTable[ 0 ],
                &PrimarySymTable[ PrimarySymTableCount - 1 ]
                );

            for ( unsigned i = 0; i < nOmapToSrc; i++ )
            {
                ULONG RvaInImg = pOmapToSrc[ i ].Rva1;
                ULONG RvaInSrc = pOmapToSrc[ i ].Rva2;

                if (( RvaInImg != 0 ) && ( RvaInSrc != 0 ))
                {
                    unsigned TableIndex = FirstSrcSymTableIndexForRva(
                                              PrimarySymTable,
                                              PrimarySymTableCount,
                                              RvaInSrc
                                              );

                    while ( TableIndex < PrimarySymTableCount ) {

                        SYMNODE* SymNode = PrimarySymTable[ TableIndex++ ];

                        if ( SymNode->RvaOrig >= RvaInSrc )
                        {
                            break;
                        }

                        if (( SymNode->RvaInImg != 0 ) &&
                            ( SymNode->SymType & 0xFF00 ) != 0xAA00 )
                        {
                            int SrcOffset = RvaInSrc - SymNode->RvaOrig;

                            ASSERT( SrcOffset > 0 );

                            if ( SrcOffset > 0 )
                            {
                                CHAR szSymNameExtension[ 16 ];

                                sprintf( szSymNameExtension, "$BB+%u", SrcOffset );

                                AddFragmentSymbol( SymNode, szSymNameExtension, 0, 0xBB01, 0, RvaInImg, RvaInSrc );
                            }
                        }
                    }
                }
            }

            //
            //  Now we can combine the fragment symbols into the primary sym table.
            //

            PrimarySymTableCount += FragmentSymTableCount;

            FragmentSymTable = NULL;

        }
    }

    //
    //  Now sort the sym table by image RVA ascending.  We use quick sort on
    //  the RVAs, then we go back with bubble sort for the names within each
    //  equivalent RVA.
    //

    ASSERT( PrimarySymTableCount > 1 );

    QsortSymTableByRvaInImg(
        &PrimarySymTable[ 0 ],
        &PrimarySymTable[ PrimarySymTableCount - 1 ]
        );

    BubbleSortPrimarySymTable();

    //
    //  BUGBUG:  Delete consequtive duplicates.
    //

    return TRUE;
}


ULONG
__fastcall
PdbSymbolHandler::ImageRvaToFileOffset( ULONG RvaInImg )
{
    if ( RvaInImg < NtHeader->OptionalHeader.SizeOfHeaders )
    {
        return RvaInImg;
    }

    for ( unsigned i = 0; i < SectionCount; i++ )
    {
        if ( RvaInImg >= SectionHeader[ i ].VirtualAddress )
        {
            ULONG OffsetInSection = RvaInImg - SectionHeader[ i ].VirtualAddress;
            ULONG SectionFileSize = ROUNDUP2( SectionHeader[ i ].SizeOfRawData, NtHeader->OptionalHeader.FileAlignment );

            if ( OffsetInSection < SectionFileSize )
            {
                return SectionHeader[ i ].PointerToRawData + OffsetInSection;
            }
        }
    }

    return 0;
}


PVOID
__fastcall
PdbSymbolHandler::ImageRvaToMappedAddress( ULONG RvaInImg )
{
    ULONG MappedOffset = ImageRvaToFileOffset( RvaInImg );

    if (( MappedOffset != 0 ) && ( MappedOffset < ExeFileSize ))
    {
        return (PVOID)( ExeFileMapped + MappedOffset );
    }

    return NULL;
}



VOID PdbSymbolHandler::LoadImageDerivedSymbols( VOID )
{
    __try
    {
        CHAR szSymbolName[ 2048 ];

        ASSERT( NtHeader->Signature == IMAGE_NT_SIGNATURE );

        RootPeSym = CreateSymbol( NULL, "PE", 100, 0, 0, 0, 0 );

        SYMNODE* PeHeaderSym = AddPrimarySymbol(
                                   RootPeSym,
                                   "Header",
                                   0,
                                   0xAA01,
                                   NtHeader->OptionalHeader.SizeOfHeaders,
                                   0,
                                   0
                                   );

        AddPrimarySymbol(
            PeHeaderSym,
            "NT",
            0,
            0xAA02,
            0,
            (PBYTE)( NtHeader ) - ExeFileMapped,
            0
            );

        AddPrimarySymbol(
            RootPeSym,
            "End",
            0xFFFF,
            0xAAFF,
            0,
            NtHeader->OptionalHeader.SizeOfImage - 1,
            0
            );

        //
        //  Create symbols for start of each section.
        //
        //  NOTE: When matching symbols, these should be lower
        //        priority than "real" symbols because a "real"
        //        symbol at the start of a section could have
        //        moved for purposes of patch matching.
        //

        SYMNODE* RootSectionSym = CreateSymbol( RootPeSym, "Section", 100, 0, 0, 0, 0 );

        #define SECT_NAME_SIZE  sizeof( SectionHeader[ 0 ].Name )

        for ( unsigned i = 0; i < SectionCount; i++ )
        {
            memcpy( szSymbolName, SectionHeader[ i ].Name, SECT_NAME_SIZE );

            char *p = szSymbolName + SECT_NAME_SIZE;

            do
            {
                *p-- = 0;
            }
            while (( p >= szSymbolName ) && ( *p == ' ' ));

            AddPrimarySymbol(
                RootSectionSym,
                szSymbolName,
                i + 1,
                0xAA03,
                SectionHeader[ i ].Misc.VirtualSize,
                SectionHeader[ i ].VirtualAddress,
                0
                );
        }

        //
        //  Create symbols for location of each PE data directory.
        //

        const static char* DirectoryName[ 16 ] = {
            "Export",
            "Import",
            "Resource",
            "Exception",
            "Security",
            "Relocation",
            "Debug",
            "Architecture",
            "GlobalPtr",
            "TLS",
            "LoadConfig",
            "BoundImport",
            "IAT",
            "DelayImport",
            "COM",
            "15"
        };

        SYMNODE* DirectorySymNode[ 16 ];

        ZeroMemory( &DirectorySymNode, sizeof( DirectorySymNode ));

        SYMNODE* RootPeDirSym = CreateSymbol( RootPeSym, "Directory", 200, 0, 0, 0, 0 );

        unsigned nDirectories = NtHeader->OptionalHeader.NumberOfRvaAndSizes;

        if ( nDirectories > 16 )
        {
             nDirectories = 16;
        }

        for ( unsigned i = 0; i < nDirectories; i++ )
        {
            if ( NtHeader->OptionalHeader.DataDirectory[ i ].VirtualAddress )
            {
                DirectorySymNode[ i ] = AddPrimarySymbol(
                                            RootPeDirSym,
                                            DirectoryName[ i ],
                                            i + 1,
                                            0xAA04,
                                            NtHeader->OptionalHeader.DataDirectory[ i ].Size,
                                            NtHeader->OptionalHeader.DataDirectory[ i ].VirtualAddress
                                            );
            }
        }

        //
        //  Create symbol for entry point (DllMain or MainCrtStartup):
        //
        //      PE\EntryPoint
        //

        if ( NtHeader->OptionalHeader.AddressOfEntryPoint )
        {
            AddPrimarySymbol(
                RootPeSym,
                "EntryPoint",
                1,
                0xAA02,
                0,
                NtHeader->OptionalHeader.AddressOfEntryPoint
                );
        }

        //
        //  Create symbols for exports:
        //
        //      PE\Directory\Export\NameOfDll               (name of this dll)
        //      PE\Directory\Export\AddressOfFunctions
        //      PE\Directory\Export\AddressOfNames
        //      PE\Directory\Export\AddressOfNameOrdinals
        //
        //      for each entry in AddressOfNames:
        //
        //         PE\Directory\Export\AddressOfName\Foo    (pointer to "Foo")
        //         PE\Export\Foo                            (actual function)
        //
        //      for each entry in AddressOfFunctions that doesn't have a name
        //
        //         PE\Export\Ordinal\123                    (actual function)
        //
        //      for each entry in AddressOfFunctions that is a forwarder
        //
        //         PE\Export\Forward\ntdll.ForwardName
        //

        __try
        {
            ULONG ExportDirRva  = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress;
            ULONG ExportDirSize = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].Size;
            ULONG ExportDirEnd  = ExportDirRva + ExportDirSize;

            PIMAGE_EXPORT_DIRECTORY ExportDir = (PIMAGE_EXPORT_DIRECTORY) ImageRvaToMappedAddress( ExportDirRva );

            if ( ExportDir )
            {
                SYMNODE* DirExportSym = DirectorySymNode[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

                SYMNODE* RootExportSym = CreateSymbol( RootPeSym, "Export", 300, 0, 0, 0, 0 );

                if ( ExportDir->Name != 0 )
                {
                    LPSTR MappedName = (LPSTR)ImageRvaToMappedAddress( ExportDir->Name );
                    ULONG NameLength = MappedName ? strlen( MappedName ) + 1 : 0;

                    AddPrimarySymbol(
                        DirExportSym,
                        "DllName",
                        1,
                        0xAA05,
                        NameLength,
                        ExportDir->Name
                        );
                }

                if ( ExportDir->AddressOfFunctions != 0 )
                {
                    AddPrimarySymbol(
                        DirExportSym,
                        "AddressOfFunctions",
                        2,
                        0xAA05,
                        ExportDir->NumberOfFunctions * sizeof( ULONG ),
                        ExportDir->AddressOfFunctions
                        );
                }

                if ( ExportDir->AddressOfNames != 0 )
                {
                    AddPrimarySymbol(
                        DirExportSym,
                        "AddressOfNames",
                        3,
                        0xAA05,
                        ExportDir->NumberOfNames * sizeof( ULONG ),
                        ExportDir->AddressOfNames
                        );
                }

                if ( ExportDir->AddressOfNameOrdinals != 0 )
                {
                    AddPrimarySymbol(
                        DirExportSym,
                        "AddressOfNameOrdinals",
                        4,
                        0xAA05,
                        ExportDir->NumberOfNames * sizeof( USHORT ),
                        ExportDir->AddressOfNameOrdinals
                        );
                }

                PULONG  ExportFunctionArray      = (PULONG) ImageRvaToMappedAddress( ExportDir->AddressOfFunctions );
                PULONG  ExportNameArray          = (PULONG) ImageRvaToMappedAddress( ExportDir->AddressOfNames );
                PUSHORT ExportNameToOrdinalArray = (PUSHORT)ImageRvaToMappedAddress( ExportDir->AddressOfNameOrdinals );

                if ( ExportFunctionArray )
                {
                    PBYTE ExportOrdinalHasName = new BYTE[ ExportDir->NumberOfFunctions ];

                    ZeroMemory( ExportOrdinalHasName, ExportDir->NumberOfFunctions );

                    if (( ExportNameArray ) && ( ExportNameToOrdinalArray ))
                    {
                        SYMNODE* AddressOfNameSym = CreateSymbol( DirExportSym, "AddressOfName", 1, 0, 0, 0, 0 );

                        for ( unsigned i = 0; i < ExportDir->NumberOfNames; i++ )
                        {
                            ULONG RvaOfNameInImg    = ExportNameArray[ i ];
                            LPSTR MappedNameInImage = (LPSTR)ImageRvaToMappedAddress( RvaOfNameInImg );

                            if ( MappedNameInImage != 0 )
                            {
                                AddPrimarySymbol(
                                    AddressOfNameSym,
                                    MappedNameInImage,
                                    1,
                                    0xAA05,
                                    strlen( MappedNameInImage ) + 1,
                                    RvaOfNameInImg
                                    );

                                ULONG Ordinal = ExportNameToOrdinalArray[ i ];

                                if ( Ordinal < ExportDir->NumberOfFunctions )
                                {
                                    ExportOrdinalHasName[ Ordinal ] = TRUE;

                                    AddPrimarySymbol(
                                        RootExportSym,
                                        MappedNameInImage,
                                        1,
                                        0xAA85,
                                        0,
                                        ExportFunctionArray[ Ordinal ]
                                        );
                                }
                            }
                        }
                    }

                    SYMNODE* ExportOrdinalSym = NULL;
                    SYMNODE* ExportForwardSym = NULL;

                    for ( unsigned i = 0; i < ExportDir->NumberOfFunctions; i++ )
                    {
                        ULONG FunctionRva = ExportFunctionArray[ i ];

                        if ( ! ExportOrdinalHasName[ i ] )
                        {
                            if ( ExportOrdinalSym == NULL )
                            {
                                ExportOrdinalSym = CreateSymbol( RootExportSym, "Ordinal", 2, 0, 0, 0, 0 );
                            }

                            ULONG Ordinal = ExportDir->Base + i;

                            sprintf( szSymbolName, "%u", Ordinal );

                            AddPrimarySymbol(
                                ExportOrdinalSym,
                                szSymbolName,
                                2,
                                0xAA05,
                                0,
                                FunctionRva
                                );
                        }

                        if (( FunctionRva > ExportDirRva ) &&
                            ( FunctionRva < ExportDirEnd ))
                        {
                            if ( ExportForwardSym == NULL )
                            {
                                ExportForwardSym = CreateSymbol( RootExportSym, "Forward", 3, 0, 0, 0, 0 );
                            }

                            LPSTR MappedName = (LPSTR)ImageRvaToMappedAddress( FunctionRva );

                            AddPrimarySymbol(
                                ExportForwardSym,
                                MappedName,
                                3,
                                0xAA95,
                                0,
                                FunctionRva
                                );
                        }
                    }

                    delete ExportOrdinalHasName;
                }
            }
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            fprintf( stderr, "SYM7: Exception %08X processing PE export info\n", GetExceptionCode() );
        }

        //
        //  for each dll import descriptor:
        //
        //     PE\Directory\Import\Descriptor\foo.dll
        //     PE\Directory\Import\Descriptor\foo.dll\AddressOfName
        //     PE\Directory\Import\Descriptor\foo.dll\NameTable
        //
        //     for each import entry in this dll:
        //
        //        PE\Import\foo.dll\FooBar
        //        PE\Directory\Import\Descriptor\foo.dll\NameTable\HintAndName\FooBar
        //
        //        ...or...
        //
        //        PE\Import\foo.dll\Ordinal\123
        //

        __try
        {
            ULONG ImportDescRva = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress;

            PIMAGE_IMPORT_DESCRIPTOR ImportDesc = (PIMAGE_IMPORT_DESCRIPTOR) ImageRvaToMappedAddress( ImportDescRva );

            if ( ImportDesc )
            {
                SYMNODE* DirImportSym = DirectorySymNode[ IMAGE_DIRECTORY_ENTRY_IMPORT ];

                SYMNODE* RootImportSym = CreateSymbol( RootPeSym, "Import", 400, 0, 0, 0, 0 );

                SYMNODE* ImpDescSym = CreateSymbol( DirImportSym, "Descriptor", 1, 0, 0, 0, 0 );

                while ( ImportDesc->Characteristics != 0 )
                {
                    LPSTR ImportDllName = (LPSTR) ImageRvaToMappedAddress( ImportDesc->Name );

                    if ( ImportDllName )
                    {
                        strcpy( szSymbolName, ImportDllName );
                       _strlwr( szSymbolName );

                        SYMNODE* DllImpSym = CreateSymbol( RootImportSym, szSymbolName, 50, 0, 0, 0, 0 );
                        SYMNODE* NameTableSym = NULL;

                        SYMNODE* DescriptorSym = AddPrimarySymbol(
                            ImpDescSym,
                            szSymbolName,
                            1,
                            0xAA06,
                            sizeof( IMAGE_IMPORT_DESCRIPTOR ),
                            ImportDescRva
                            );

                        AddPrimarySymbol(
                            DescriptorSym,
                            "AddressOfDllName",
                            1,
                            0xAA06,
                            strlen( ImportDllName ) + 1,
                            ImportDesc->Name
                            );

                        PIMAGE_THUNK_DATA NameTable = (PIMAGE_THUNK_DATA) ImageRvaToMappedAddress( ImportDesc->OriginalFirstThunk );

                        if ( NameTable )
                        {
                            unsigned Count = 0;

                            while ( NameTable[ Count ].u1.Ordinal != 0 )
                            {
                                Count++;
                            }

                            unsigned Size = ( Count + 1 ) * sizeof( NameTable[ 0 ] );

                            NameTableSym = AddPrimarySymbol(
                                                DescriptorSym,
                                                "NameTable",
                                                1,
                                                0xAA06,
                                                Size,
                                                ImportDesc->OriginalFirstThunk
                                                );

                            SYMNODE* DllOrdinalSym = NULL;
                            SYMNODE* HintAndNameSym = NULL;

                            for ( unsigned i = 0; i < Count; i++ )
                            {
                                PIMAGE_THUNK_DATA Thunk = &NameTable[ i ];

                                ULONG RvaInIat = ImportDesc->FirstThunk + ( i * sizeof( IMAGE_THUNK_DATA ));

                                if ( IMAGE_SNAP_BY_ORDINAL( Thunk->u1.Ordinal ))
                                {
                                    if ( DllOrdinalSym == NULL )
                                    {
                                        DllOrdinalSym = CreateSymbol( DllImpSym, "Ordinal", 2, 0, 0, 0, 0 );
                                    }

                                    sprintf( szSymbolName, "%u", IMAGE_ORDINAL( Thunk->u1.Ordinal ));

                                    AddPrimarySymbol(
                                        DllOrdinalSym,
                                        szSymbolName,
                                        1,
                                        0xAA06,
                                        0,
                                        RvaInIat
                                        );
                                }
                                else    // import by name
                                {
                                    PIMAGE_IMPORT_BY_NAME ImportByName = (PIMAGE_IMPORT_BY_NAME) ImageRvaToMappedAddress( Thunk->u1.AddressOfData );

                                    if ( ImportByName )
                                    {
                                        if ( HintAndNameSym == NULL )
                                        {
                                            HintAndNameSym = CreateSymbol( NameTableSym, "HintAndName", 2, 0, 0, 0, 0 );
                                        }

                                        AddPrimarySymbol(
                                            DllImpSym,
                                            (LPSTR) ImportByName->Name,
                                            1,
                                            0xAA86,
                                            sizeof( PVOID ),
                                            RvaInIat
                                            );

                                        AddPrimarySymbol(
                                            HintAndNameSym,
                                            (LPSTR) ImportByName->Name,
                                            1,
                                            0xAA06,
                                            sizeof( ImportByName->Hint ) + strlen( (LPSTR) ImportByName->Name ) + 1,
                                            Thunk->u1.AddressOfData
                                            );
                                    }
                                }
                            }
                        }
                    }

                    ImportDescRva += sizeof( *ImportDesc );
                    ImportDesc++;
                }
                while ( ImportDesc->Characteristics != 0 );

            }
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            fprintf( stderr, "SYM7: Exception %08X processing PE import info\n", GetExceptionCode() );
        }

        __try
        {
            //
            //  for each delay load dll import descriptor:
            //
            //     PE\Directory\DelayImport\Descriptor\foo.dll
            //     PE\Directory\DelayImport\Descriptor\foo.dll\AddressOfDllName
            //     PE\Directory\DelayImport\Descriptor\foo.dll\NameTable
            //     PE\Directory\DelayImport\Descriptor\foo.dll\BoundTable
            //     PE\Directory\DelayImport\Descriptor\foo.dll\UnloadTable
            //     PE\DelayImport\foo.dll\HMODULE
            //
            //     for each delay load import entry in this dll:
            //
            //        PE\DelayImport\foo.dll\FooBar
            //        PE\Directory\DelayImport\Descriptor\foo.dll\NameTable\HintAndName\FooBar
            //
            //        ...or...
            //
            //        PE\DelayImport\foo.dll\Ordinal\123  ...etc...
            //

            ULONG DelayImportDescRva = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT ].VirtualAddress;

            PImgDelayDescr DelayImportDesc = (PImgDelayDescr) ImageRvaToMappedAddress( DelayImportDescRva );

            if ( DelayImportDesc )
            {
                SYMNODE* DirDelaySym = DirectorySymNode[ IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT ];

                SYMNODE* RootDelaySym = CreateSymbol( RootPeSym, "DelayImport", 500, 0, 0, 0, 0 );

                SYMNODE* DelayDescSym = CreateSymbol( DirDelaySym, "Descriptor", 1, 0, 0, 0, 0 );

                while (( DelayImportDesc->rvaDLLName ) &&
                       ( DelayImportDesc->rvaINT ))
                {
                    ImgDelayDescr DelayDesc;

                    memcpy( &DelayDesc, DelayImportDesc, sizeof( DelayDesc ));

                    if ( ! ( DelayDesc.grAttrs & dlattrRva ))
                    {
                        ULONG ImageBase = NtHeader->OptionalHeader.ImageBase;

                        if ( DelayDesc.rvaDLLName >= ImageBase )
                        {
                             DelayDesc.rvaDLLName -= ImageBase;
                        }

                        if ( DelayDesc.rvaHmod >= ImageBase )
                        {
                             DelayDesc.rvaHmod -= ImageBase;
                        }

                        if ( DelayDesc.rvaIAT >= ImageBase )
                        {
                             DelayDesc.rvaIAT -= ImageBase;
                        }

                        if ( DelayDesc.rvaINT >= ImageBase )
                        {
                             DelayDesc.rvaINT -= ImageBase;
                        }

                        if ( DelayDesc.rvaBoundIAT >= ImageBase )
                        {
                             DelayDesc.rvaBoundIAT -= ImageBase;
                        }

                        if ( DelayDesc.rvaUnloadIAT >= ImageBase )
                        {
                             DelayDesc.rvaUnloadIAT -= ImageBase;
                        }
                    }

                    ASSERT( DelayDesc.rvaDLLName   < NtHeader->OptionalHeader.SizeOfImage );
                    ASSERT( DelayDesc.rvaHmod      < NtHeader->OptionalHeader.SizeOfImage );
                    ASSERT( DelayDesc.rvaIAT       < NtHeader->OptionalHeader.SizeOfImage );
                    ASSERT( DelayDesc.rvaINT       < NtHeader->OptionalHeader.SizeOfImage );
                    ASSERT( DelayDesc.rvaBoundIAT  < NtHeader->OptionalHeader.SizeOfImage );
                    ASSERT( DelayDesc.rvaUnloadIAT < NtHeader->OptionalHeader.SizeOfImage );

                    LPSTR DelayDllName = (LPSTR) ImageRvaToMappedAddress( DelayDesc.rvaDLLName );

                    if ( DelayDllName )
                    {
                        strcpy( szSymbolName, DelayDllName );
                       _strlwr( szSymbolName );

                        SYMNODE* DelayDllSym = CreateSymbol( RootDelaySym, szSymbolName, 50, 0, 0, 0, 0 );
                        SYMNODE* NameTableSym = NULL;

                        SYMNODE* DescriptorSym = AddPrimarySymbol(
                            DelayDescSym,
                            szSymbolName,
                            1,
                            0xAA07,
                            sizeof( IMAGE_IMPORT_DESCRIPTOR ),
                            DelayImportDescRva
                            );

                        AddPrimarySymbol(
                            DescriptorSym,
                            "AddressOfDllName",
                            1,
                            0xAA07,
                            strlen( DelayDllName ) + 1,
                            DelayDesc.rvaDLLName
                            );

                        AddPrimarySymbol(
                            DelayDllSym,
                            "HMODULE",
                            1,
                            0xAA07,
                            0,
                            DelayDesc.rvaHmod
                            );

                        unsigned TableSize = 0;

                        PIMAGE_THUNK_DATA NameTable = (PIMAGE_THUNK_DATA) ImageRvaToMappedAddress( DelayDesc.rvaINT );

                        if ( NameTable )
                        {
                            unsigned Count = 0;

                            while ( NameTable[ Count ].u1.Ordinal != 0 )
                            {
                                Count++;
                            }

                            TableSize = ( Count + 1 ) * sizeof( NameTable[ 0 ] );
                        }

                        NameTableSym = AddPrimarySymbol(
                                           DescriptorSym,
                                           "NameTable",
                                           1,
                                           0xAA07,
                                           TableSize,
                                           DelayDesc.rvaINT
                                           );

                        if ( DelayDesc.rvaBoundIAT )
                        {
                            AddPrimarySymbol(
                                DescriptorSym,
                                "BoundTable",
                                1,
                                0xAA07,
                                TableSize,
                                DelayDesc.rvaBoundIAT
                                );
                        }

                        if ( DelayDesc.rvaUnloadIAT )
                        {
                            AddPrimarySymbol(
                                DescriptorSym,
                                "UnloadTable",
                                1,
                                0xAA07,
                                TableSize,
                                DelayDesc.rvaUnloadIAT
                                );
                        }

                        SYMNODE* DllOrdinalSym = NULL;
                        SYMNODE* HintAndNameSym = NULL;

                        if ( NameTable )
                        {
                            for ( unsigned ThunkIndex = 0;; ThunkIndex++ )
                            {
                                PIMAGE_THUNK_DATA Thunk = &NameTable[ ThunkIndex ];

                                if ( Thunk->u1.Ordinal == 0 )
                                {
                                    break;
                                }

                                ULONG RvaInIat = DelayDesc.rvaIAT + ( ThunkIndex * sizeof( IMAGE_THUNK_DATA ));

                                if ( IMAGE_SNAP_BY_ORDINAL( Thunk->u1.Ordinal ))
                                {
                                    if ( DllOrdinalSym == NULL )
                                    {
                                        DllOrdinalSym = CreateSymbol( DelayDllSym, "Ordinal", 2, 0, 0, 0, 0 );
                                    }

                                    sprintf( szSymbolName, "%u", IMAGE_ORDINAL( Thunk->u1.Ordinal ));

                                    AddPrimarySymbol(
                                        DllOrdinalSym,
                                        szSymbolName,
                                        1,
                                        0xAA07,
                                        0,
                                        RvaInIat
                                        );
                                }
                                else    // import by name
                                {
                                    PIMAGE_IMPORT_BY_NAME ImportByName = (PIMAGE_IMPORT_BY_NAME) ImageRvaToMappedAddress( Thunk->u1.AddressOfData );

                                    if ( ImportByName )
                                    {
                                        if ( HintAndNameSym == NULL )
                                        {
                                            HintAndNameSym = CreateSymbol( NameTableSym, "HintAndName", 2, 0, 0, 0, 0 );
                                        }

                                        AddPrimarySymbol(
                                            DelayDllSym,
                                            (LPSTR) ImportByName->Name,
                                            1,
                                            0xAA07,
                                            0,
                                            RvaInIat
                                            );

                                        AddPrimarySymbol(
                                            HintAndNameSym,
                                            (LPSTR) ImportByName->Name,
                                            1,
                                            0xAA07,
                                            sizeof( ImportByName->Hint ) + strlen( (LPSTR) ImportByName->Name ) + 1,
                                            Thunk->u1.AddressOfData
                                            );
                                    }
                                }
                            }
                        }
                    }

                    DelayImportDescRva += sizeof( *DelayImportDesc );
                    DelayImportDesc++;
                }
            }
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            fprintf( stderr, "SYM7: Exception %08X processing PE delay-import info\n", GetExceptionCode() );
        }

        //
        //  for each resource entry
        //
        //      PE\Resource\Root\#RT_BITMAP\#2502
        //      PE\Resource\Root\#RT_BITMAP\#2502\#Lang0409
        //      PE\Resource\Root\#RT_BITMAP\#2502\#Lang0409\Data
        //      PE\Resource\Root\#RT_HTML\BLANK.HTM
        //      PE\Resource\Root\#RT_HTML\BLANK.HTM\AddressOfName
        //      PE\Resource\Root\#RT_HTML\BLANK.HTM\#Lang0409
        //      PE\Resource\Root\#RT_HTML\BLANK.HTM\#Lang0409\Data
        //

        __try
        {
            // resource info

            ResourceBaseRva    = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_RESOURCE ].VirtualAddress;
            ResourceBaseMapped = (PBYTE) ImageRvaToMappedAddress( ResourceBaseRva );

            if ( ResourceBaseMapped )
            {
                ResourceRootSym = CreateSymbol( RootPeSym, "Resource", 800, 0, 0, 0, 0 );

                PIMAGE_RESOURCE_DIRECTORY RootDir = (PIMAGE_RESOURCE_DIRECTORY) ResourceBaseMapped;

                ULONG SizeOfData = sizeof( IMAGE_RESOURCE_DIRECTORY );

                ULONG Entries = RootDir->NumberOfNamedEntries
                              + RootDir->NumberOfIdEntries;

                SizeOfData += Entries * sizeof( IMAGE_RESOURCE_DIRECTORY_ENTRY );

                AddPrimarySymbol(
                    ResourceRootSym,
                    "Root",
                    1,
                    0xAA08,
                    SizeOfData,
                    ResourceBaseRva
                    );

                LoadResourceSymsRecursive(
                    RootDir,
                    ResourceRootSym,
                    szSymbolName
                    );
            }
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            fprintf( stderr, "SYM7: Exception %08X processing PE resource info\n", GetExceptionCode() );
        }

        //
        //  for each debug directory entry
        //
        //      PE\Directory\Debug\Misc
        //      PE\Directory\Debug\NB10
        //

        __try
        {
            // debug directories

            ULONG DebugDirectoryRva   = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_DEBUG ].VirtualAddress;
            ULONG DebugDirectorySize  = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_DEBUG ].Size;
            ULONG DebugDirectoryCount = DebugDirectorySize / sizeof( IMAGE_DEBUG_DIRECTORY );

            PIMAGE_DEBUG_DIRECTORY DebugDirectory = (PIMAGE_DEBUG_DIRECTORY) ImageRvaToMappedAddress( DebugDirectoryRva );

            if (( DebugDirectory ) && ( DebugDirectoryCount )) {

                SYMNODE* DebugDirSym = DirectorySymNode[ IMAGE_DIRECTORY_ENTRY_DEBUG ];

                for ( unsigned j = 0; j < DebugDirectoryCount; j++ )
                {
                    ULONG rvaData = DebugDirectory[ j ].AddressOfRawData;
                    PBYTE pbData  = (PBYTE) ImageRvaToMappedAddress( rvaData );
                    ULONG cbData  = DebugDirectory[ j ].SizeOfData;
                    LPSTR pszName = NULL;

                    if (( pbData ) && ( cbData ))
                    {
                        switch ( DebugDirectory[ j ].Type )
                        {
                            case IMAGE_DEBUG_TYPE_COFF:          pszName = "COFF";          break;
                            case IMAGE_DEBUG_TYPE_FPO:           pszName = "FPO";           break;
                            case IMAGE_DEBUG_TYPE_MISC:          pszName = "Misc";          break;
                            case IMAGE_DEBUG_TYPE_EXCEPTION:     pszName = "Exception";     break;
                            case IMAGE_DEBUG_TYPE_FIXUP:         pszName = "Fixup";         break;
                            case IMAGE_DEBUG_TYPE_OMAP_TO_SRC:   pszName = "OmapImgToSrc";  break;
                            case IMAGE_DEBUG_TYPE_OMAP_FROM_SRC: pszName = "OmapSrcToImg";  break;
                            case IMAGE_DEBUG_TYPE_CLSID:         pszName = "CLSID";         break;

                            case IMAGE_DEBUG_TYPE_CODEVIEW:

                                pszName = "Codeview";

                                if (( pbData[ 0 ] > 0x22 ) && ( pbData[ 0 ] < 127 ) &&
                                    ( pbData[ 1 ] > 0x22 ) && ( pbData[ 1 ] < 127 ) &&
                                    ( pbData[ 2 ] > 0x22 ) && ( pbData[ 2 ] < 127 ) &&
                                    ( pbData[ 3 ] > 0x22 ) && ( pbData[ 3 ] < 127 ))
                                {
                                    memcpy( szSymbolName, pbData, 4 );  // NB10, RSDS, etc.
                                    szSymbolName[ 4 ] = 0;
                                    pszName = szSymbolName;
                                }

                                break;

                            default:

                                sprintf( szSymbolName, "Type_%X", DebugDirectory[ j ].Type );
                                pszName = szSymbolName;
                                break;
                        }

                        AddPrimarySymbol(
                            DebugDirSym,
                            pszName,
                            1,
                            0xAA09,
                            cbData,
                            rvaData
                            );
                    }
                }
            }
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            fprintf( stderr, "SYM7: Exception %08X processing PE debug info\n", GetExceptionCode() );
        }

        //
        //  for LoadConfig directory:
        //
        //      PE\Directory\LoadConfig\LockPrefixTable
        //      PE\Directory\LoadConfig\EditList
        //

        __try
        {
            // LoadConfig directory

            ULONG LoadConfigRva = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG ].VirtualAddress;

            if ( LoadConfigRva != 0 )
            {
                PIMAGE_LOAD_CONFIG_DIRECTORY LoadConfig = (PIMAGE_LOAD_CONFIG_DIRECTORY) ImageRvaToMappedAddress( LoadConfigRva );

                if ( LoadConfig )
                {
                    //
                    //  The LockPrefixTable and EditList fields are stored
                    //  as full pointers, not RVAs, so we subtract the image
                    //  base address to convert them to RVAs.
                    //

                    ULONG ImageBase = NtHeader->OptionalHeader.ImageBase;

                    if ( LoadConfig->LockPrefixTable > ImageBase )
                    {
                        ULONG  LockPrefixTableSize = 0;
                        ULONG  LockPrefixTableRva  = LoadConfig->LockPrefixTable - ImageBase;
                        PVOID* LockPrefixTablePtr  = (PVOID*) ImageRvaToMappedAddress( LockPrefixTableRva );

                        if ( LockPrefixTablePtr )
                        {
                            for ( unsigned Count = 0; LockPrefixTablePtr[ Count ] != NULL; Count++ );

                            LockPrefixTableSize = ( Count + 1 ) * sizeof( LockPrefixTablePtr[ 0 ] );
                        }

                        AddPrimarySymbol(
                            DirectorySymNode[ IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG ],
                            "LockPrefixTable",
                            1,
                            0xAA09,
                            LockPrefixTableSize,
                            LockPrefixTableRva
                            );

                        //
                        //  Note that we don't create symbols for all the
                        //  actual lock prefix instruction locations.  We
                        //  could, but they would all be ambiguous because
                        //  we don't know what function name scope each one
                        //  is under, and their order is not significant.
                        //  At this point in time we don't have the pdb
                        //  info loaded yet to determine function scope.
                        //
                    }

                    if ( LoadConfig->EditList > ImageBase )
                    {
                        ULONG EditListRva = LoadConfig->EditList - ImageBase;

                        //
                        //  I can find no reference to LoadConfig EditList
                        //  usage anywhere in the WinXP sources.
                        //

                        AddPrimarySymbol(
                            DirectorySymNode[ IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG ],
                            "EditList",
                            1,
                            0xAA09,
                            0,
                            EditListRva
                            );
                    }
                }
            }
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            fprintf( stderr, "SYM7: Exception %08X processing PE load config info\n", GetExceptionCode() );
        }


    }

    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        fprintf( stderr, "SYM7: Exception %08X processing PE info\n", GetExceptionCode() );
    }

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


VOID
__fastcall
PdbSymbolHandler::LoadResourceSymsRecursive(
    IN PIMAGE_RESOURCE_DIRECTORY ResourceDir,
    IN SYMNODE* ParentSym,
    IN LPSTR    pszBuffer
    )
{
    PIMAGE_RESOURCE_DIRECTORY_ENTRY ResourceEntry;
    PIMAGE_RESOURCE_DATA_ENTRY      ResourceData;
    PIMAGE_RESOURCE_DIR_STRING_U    ResourceNameU;
    ULONG                           ResourceCount;

    ResourceCount = ResourceDir->NumberOfNamedEntries
                  + ResourceDir->NumberOfIdEntries;

    ResourceEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)( ResourceDir + 1 );

    while ( ResourceCount-- )
    {
        ULONG OffsetToData = ResourceEntry->OffsetToData & 0x7FFFFFFF;
        ULONG RvaOfData    = ResourceBaseRva + OffsetToData;
        ULONG SizeOfData;

        if ( ResourceEntry->NameIsString )
        {
            ResourceNameU = (PIMAGE_RESOURCE_DIR_STRING_U)( ResourceBaseMapped + ResourceEntry->NameOffset );

            //
            //  BUGBUG:  I see an alternate IMAGE_RESOURCE_DIRECTORY_STRING
            //           representation alongside the unicode version in the
            //           Win32 header, but no mention of when to use it.
            //           For now we will just assume the names are unicode.
            //

            int ActualSize = WideCharToMultiByte(
                                 CP_OEMCP,
                                 0,
                                 ResourceNameU->NameString,
                                 ResourceNameU->Length,
                                 pszBuffer,
                                 2048 - 1,
                                 NULL,
                                 NULL
                                 );

            pszBuffer[ ActualSize ] = 0;    // force termination
        }
        else
        {
            sprintf( pszBuffer, "#%u", ResourceEntry->Id );

            if ( ResourceEntry->DataIsDirectory )
            {
                if (( ParentSym == ResourceRootSym ) &&
                    ( ResourceEntry->Id > 0 ) &&
                    ( ResourceEntry->Id < countof( ResTypeToName )) &&
                    ( ResTypeToName[ ResourceEntry->Id ] != NULL ))
                {
                    sprintf( pszBuffer, "#%s", ResTypeToName[ ResourceEntry->Id ] );
                }
            }
            else    // child is data
            {
                if ( ResourceEntry->Id > 0x400 )
                {
                    sprintf( pszBuffer, "#Lang%04X", ResourceEntry->Id );
                }
            }
        }

        if ( ResourceEntry->DataIsDirectory )
        {
            SizeOfData = sizeof( IMAGE_RESOURCE_DIRECTORY );

            PIMAGE_RESOURCE_DIRECTORY ChildDir = (PIMAGE_RESOURCE_DIRECTORY)( ResourceBaseMapped + OffsetToData );

            if ( ChildDir )
            {
                ULONG Entries = ChildDir->NumberOfNamedEntries
                              + ChildDir->NumberOfIdEntries;

                SizeOfData += Entries * sizeof( IMAGE_RESOURCE_DIRECTORY_ENTRY );
            }
        }
        else
        {
            SizeOfData = sizeof( IMAGE_RESOURCE_DATA_ENTRY );
        }

        SYMNODE* ResourceSym = AddPrimarySymbol(
                                   ParentSym,
                                   pszBuffer,
                                   1,
                                   ResourceEntry->DataIsDirectory ? 0xAA08 : 0xAA09,
                                   SizeOfData,
                                   RvaOfData
                                   );

        if ( ResourceEntry->NameIsString )
        {
            ResourceNameU   = (PIMAGE_RESOURCE_DIR_STRING_U)( ResourceBaseMapped + ResourceEntry->NameOffset );
            ULONG NameSize  = ( ResourceNameU->Length * sizeof( WCHAR )) + sizeof( WORD );
            ULONG RvaOfName = ResourceBaseRva + ResourceEntry->NameOffset;

            AddPrimarySymbol(
                ResourceSym,
                "AddressOfName",
                2,
                0xAA08,
                NameSize,
                RvaOfName
                );
        }

        if ( ResourceEntry->DataIsDirectory )
        {
            LoadResourceSymsRecursive(
                (PIMAGE_RESOURCE_DIRECTORY)( ResourceBaseMapped + OffsetToData ),
                ResourceSym,
                pszBuffer
                );
        }
        else
        {
            ResourceData = (PIMAGE_RESOURCE_DATA_ENTRY)( ResourceBaseMapped + OffsetToData );

            //
            //  BUGBUG:  Contrary to what the winnt.h header file says, the
            //           IMAGE_RESOURCE_DATA_ENTRY.OffsetToData field appears
            //           to be an RVA, not offset from beginning of directory.
            //

            unsigned n = sprintf( pszBuffer, "Data" );

            if ( ResourceData->CodePage != 0 )
            {
                sprintf( pszBuffer + n, "%u", ResourceData->CodePage );
            }

            AddPrimarySymbol(
                ResourceSym,
                pszBuffer,
                3,
                0xAA0A,
                ResourceData->Size,
                ResourceData->OffsetToData
                );
        }

        ResourceEntry++;
    }
}


VOID PdbSymbolHandler::LoadModuleSymbols( VOID )
{
    Mod* ThisMod = NULL;

    for (;;)
    {
        Mod* LastMod = ThisMod;

        if ( ! dbi->QueryNextMod( LastMod, &ThisMod ))
        {
            ThisMod = NULL;
        }

        if ( LastMod != NULL )
        {
            LastMod->Close();
        }

        if ( ThisMod == NULL )
        {
            break;
        }

        char szModName[ PDB_MAX_PATH ];
        long cbModName = PDB_MAX_PATH;

        *szModName = 0;

        ThisMod->QueryName( szModName, &cbModName );

        _strlwr( szModName );

        //
        //  Sometimes szModName contains relative path or absolute path,
        //  so we strip that to just the filename.  This may leave us with
        //  two or more modules of the same name that had different paths,
        //  but that is preferable to leaving paths in symbols that may
        //  vary by different build machine root enlistments.
        //

        char* pszLastBackslash = strrchr( szModName, '\\' );
        char* pszLastForeslash = strrchr( szModName, '/'  );
        char* pszLastSlash     = MAX( pszLastBackslash, pszLastForeslash );
        char* pszModNameNoPath = pszLastSlash ? pszLastSlash + 1 : szModName;

#ifdef DONTCOMPILE  // code to uniquify duplicate module names

        if ( NameTable.LookupName( pszModNameNoPath ))
        {
            char* pszModNameEnd = pszModNameNoPath + strlen( pszModNameNoPath );
            ULONG nExtender = 2;

            do
            {
                sprintf( pszModNameEnd, "[%u]", nExtender++ );
            }
            while ( NameTable.LookupName( pszModNameNoPath ));
        }

#endif

        NestedScopeParentSym = CreateSymbol( RootModuleSym, pszModNameNoPath, 0, 0, 0, 0, 0 );

        long cbMem = 0;

        ThisMod->QuerySymbols( NULL, &cbMem );

        if ( cbMem > 0 )
        {
            PBYTE pbMem = new BYTE[ cbMem ];
            PBYTE pbEnd = pbMem + cbMem;
            PBYTE pbSym = pbMem + sizeof( DWORD );

            ThisMod->QuerySymbols( pbMem, &cbMem );

            while ( pbSym < pbEnd )
            {
                CVSYM* pcvsym = (CVSYM*)pbSym;

                ProcessCvSym( pcvsym );

                pbSym += pcvsym->cb + sizeof( WORD );
            }

            delete pbMem;
        }
    }
}


VOID __fastcall PdbSymbolHandler::ProcessCvSym( CVSYM* pcvsym )
{
    LPCSTR pszName = NULL;
    ULONG  SegNum  = 0;
    ULONG  SegOff  = 0;
    ULONG  SymSize = 0;
    BOOL   Nesting = FALSE;

#ifdef SYMCOUNTER

    if ( pcvsym->cvsymt < 0x4000 )
    {
        SymTypeCounter[ pcvsym->cvsymt ]++;
    }

#endif

    switch ( (CVSYMT) pcvsym->cvsymt )
    {
        case cvsymtEnd:

            //
            //  End of scope, pop to higher scope
            //

            ASSERT( NestedScopeParentSym != NULL );

            if ( NestedScopeParentSym != NULL )
            {
                NestedScopeParentSym = NestedScopeParentSym->ParentSym;
            }

            return;

        case cvsymtLData32_Sz:
        case cvsymtGData32_Sz:

            pszName = ((CVSYMDATA32_32*) pcvsym )->rgchName;
            SegNum  = ((CVSYMDATA32_32*) pcvsym )->lsn;
            SegOff  = ((CVSYMDATA32_32*) pcvsym )->ib;
            /* no size data */
            break;

        case cvsymtPub32_Sz:

            pszName = ((CVSYMDATA32_32*) pcvsym )->rgchName;
            SegNum  = ((CVSYMDATA32_32*) pcvsym )->lsn;
            SegOff  = ((CVSYMDATA32_32*) pcvsym )->ib;
            /* no size data */

            //
            //  Ignore public symbols containing "NULL_IMPORT_DESCRIPTOR" and
            //  "NULL_THUNK_DATA" since BBT does not have proper OMAP for them.
            //

            if (( strstr( pszName, "NULL_IMPORT_DESCRIPTOR" )) ||
                ( strstr( pszName, "NULL_THUNK_DATA"        )))
            {
                return;
            }

            break;

        case cvsymtLProc32_Sz:
        case cvsymtGProc32_Sz:

            Nesting = TRUE;
            pszName = ((CVSYMPROC32_32*) pcvsym )->rgchName;
            SegNum  = ((CVSYMPROC32_32*) pcvsym )->lsn;
            SegOff  = ((CVSYMPROC32_32*) pcvsym )->ib;
            SymSize = ((CVSYMPROC32_32*) pcvsym )->cb;
            break;

        case cvsymtLabel32_Sz:

            pszName = ((CVSYMLABEL32*) pcvsym )->rgchName;
            SegNum  = ((CVSYMLABEL32*) pcvsym )->lsn;
            SegOff  = ((CVSYMLABEL32*) pcvsym )->ib;
            /* no size data */
            break;

        case cvsymtLThread32_Sz:
        case cvsymtGThread32_Sz:

            pszName = ((CVSYMTHREAD32_32*) pcvsym )->rgchName;
            SegNum  = ((CVSYMTHREAD32_32*) pcvsym )->lsn;
            SegOff  = ((CVSYMTHREAD32_32*) pcvsym )->ib;
            /* no size data */
            break;

        case cvsymtThunk32_Sz:

            Nesting = TRUE;
            pszName = ((CVSYMTHUNK32*) pcvsym )->rgchName;
            SegNum  = ((CVSYMTHUNK32*) pcvsym )->lsn;
            SegOff  = ((CVSYMTHUNK32*) pcvsym )->ib;
            SymSize = ((CVSYMTHUNK32*) pcvsym )->cb;
            break;

        case cvsymtBlock32_Sz:

            Nesting = TRUE;
            pszName = ((CVSYMBLOCK32*) pcvsym )->rgchName;
            SegNum  = ((CVSYMBLOCK32*) pcvsym )->lsn;
            SegOff  = ((CVSYMBLOCK32*) pcvsym )->ib;
            SymSize = ((CVSYMBLOCK32*) pcvsym )->cb;
            break;

        case cvsymtWith32_Sz:

            Nesting = TRUE;
            pszName = ((CVSYMWITH32*) pcvsym )->rgchExpr;
            SegNum  = ((CVSYMWITH32*) pcvsym )->lsn;
            SegOff  = ((CVSYMWITH32*) pcvsym )->ib;
            SymSize = ((CVSYMWITH32*) pcvsym )->cb;
            break;

        case cvsymtLProcIa64_Sz:
        case cvsymtGProcIa64_Sz:

            Nesting = TRUE;
            pszName = ((CVSYMPROCIA64*) pcvsym )->rgchName;
            SegNum  = ((CVSYMPROCIA64*) pcvsym )->lsn;
            SegOff  = ((CVSYMPROCIA64*) pcvsym )->ib;
            SymSize = ((CVSYMPROCIA64*) pcvsym )->cb;
            break;

        case cvsymtLProcMips_Sz:
        case cvsymtGProcMips_Sz:

            Nesting = TRUE;
            pszName = ((CVSYMPROCMIPS_32*) pcvsym )->rgchName;
            SegNum  = ((CVSYMPROCMIPS_32*) pcvsym )->lsn;
            SegOff  = ((CVSYMPROCMIPS_32*) pcvsym )->ib;
            SymSize = ((CVSYMPROCMIPS_32*) pcvsym )->cb;
            break;

        case cvsymtAnnotation:

            //
            //  BUGBUG:  CVSYMANNOTATION rgsz field is a double-null
            //           terminated sequence of single-null terminated
            //           strings.  We'll only pick up the first name
            //           for this record here.
            //

            pszName = ((CVSYMANNOTATION*) pcvsym )->rgsz;
            SegNum  = ((CVSYMANNOTATION*) pcvsym )->lsn;
            SegOff  = ((CVSYMANNOTATION*) pcvsym )->ib;
            /* no size data */
            break;

        case cvsymtGManProc_Sz:
        case cvsymtLManProc_Sz:

            Nesting = TRUE;     // BUGBUG: we don't know this for sure
            pszName = ((CVSYMMANPROC*) pcvsym )->rgchName;
            SegNum  = ((CVSYMMANPROC*) pcvsym )->lsn;
            SegOff  = ((CVSYMMANPROC*) pcvsym )->ib;
            SymSize = ((CVSYMMANPROC*) pcvsym )->cb;
            break;

        case cvsymtLManData_Sz:
        case cvsymtGManData_Sz:

            //
            //  BUGBUG:  Don't know what struct to use for these.
            //

            ASSERT( ! "encountered cvsymtGManData_Sz" );
            return;

        //
        //  The following set of sym types do not represent unique defined
        //  objects with image addresses, but they might be useful for future
        //  patch transformations because they can still be used as markers
        //  if they are properly scoped.
        //

        case cvsymtObjName_Sz:      // object file name, already have it
        case cvsymtCompile2_Sz:     // compiler/linker id string
        case cvsymtCompile:         // compiler id string
        case cvsymtUDT_Sz:          // user defined type
        case cvsymtConstant_Sz:     // constant #define
        case cvsymtRegister_Sz:     // register variable
        case cvsymtManyReg_Sz:      // multi-register variable
        case cvsymtManyReg2_Sz:     // multi-register variable
        case cvsymtRegRel32_Sz:     // register-relative address
        case cvsymtBPRel32_Sz:      // BP-relative variable

            return;

        case cvsymtDataRef_Sz:      // call or address of global data object
        case cvsymtProcRef_Sz:      // call or address of another procedure
        case cvsymtLProcRef_Sz:     // call of address of "local" procedure

            return;

        default:

            DEBUGCODE( printf( "\nIgnored cvsymt 0x%x:\n", pcvsym->cvsymt ));

            return;

    }

    //
    //  Some symbols appear to have no names, so we ignore those unless
    //  they are the start of a new scope.
    //

    if ( pszName == NULL )
    {
        pszName = "";
    }

    if (( *pszName != 0 ) || ( Nesting ))
    {
        SYMNODE* SymNode = NULL;

        if ( SegNum != 0 )
        {
            if ( SegNum <= nSrcSegToRvaMap )
            {
                ULONG RvaInSrc = pSrcSegToRvaMap[ SegNum - 1 ] + SegOff;
                ULONG RvaInImg = TranslateOmapFromSrcToImg( RvaInSrc );

                SymNode = AddPrimarySymbol(
                              NestedScopeParentSym,
                              pszName,
                              1,
                              pcvsym->cvsymt,
                              SymSize,
                              RvaInImg,
                              RvaInSrc
                              );
            }
        }

        if ( Nesting )
        {
            NestedScopeParentSym = SymNode;
        }
    }
}


ULONG PdbSymbolHandler::TranslateOmapFromSrcToImg( ULONG Rva )
{
    if ( nOmapToImg == 0 )
    {
        return Rva;
    }

    unsigned Lo = 0;
    unsigned Hi = nOmapToImg;

    while ( Lo < Hi )
    {
        unsigned Mi = ( Lo + Hi ) / 2;

        if ( pOmapToImg[ Mi ].Rva1 == Rva )
        {
            return pOmapToImg[ Mi ].Rva2;
        }
        else if ( pOmapToImg[ Mi ].Rva1 < Rva )
        {
            Lo = Mi + 1;
        }
        else
        {
            Hi = Mi;
        }
    }

    if (( Lo == 0 ) || ( Lo == nOmapToImg ))
    {
        return 0;       // outside translation entry range
    }

    unsigned Index = Lo - 1;

    //
    //  Now pOmapToImg[ Index ] is the nearest preceding omap entry.
    //

    ASSERT( pOmapToImg[ Index ].Rva1 <= Rva );

    if (( Index + 1 ) < nOmapToImg )
    {
        ASSERT( pOmapToImg[ Index + 1 ].Rva1 > Rva );
    }

    if ( pOmapToImg[ Index ].Rva2 == 0 )
    {
        return 0;
    }

    unsigned Offset = Rva - pOmapToImg[ Index ].Rva1;

    return pOmapToImg[ Index ].Rva2 + Offset;
}


ULONG PdbSymbolHandler::TranslateOmapFromImgToSrc( ULONG Rva )
{
    if ( nOmapToSrc == 0 )
    {
        return Rva;
    }

    unsigned Lo = 0;
    unsigned Hi = nOmapToSrc;

    while ( Lo < Hi )
    {
        unsigned Mi = ( Lo + Hi ) / 2;

        if ( pOmapToSrc[ Mi ].Rva1 == Rva )
        {
            return pOmapToSrc[ Mi ].Rva2;
        }
        else if ( pOmapToSrc[ Mi ].Rva1 < Rva )
        {
            Lo = Mi + 1;
        }
        else
        {
            Hi = Mi;
        }
    }

    if (( Lo == 0 ) || ( Lo == nOmapToSrc ))
    {
        return 0;       // outside translation entry range
    }

    unsigned Index = Lo - 1;

    //
    //  Now pOmapToSrc[ Index ] is the nearest preceding omap entry.
    //

    ASSERT( pOmapToSrc[ Index ].Rva1 <= Rva );

    if (( Index + 1 ) < nOmapToSrc )
    {
        ASSERT( pOmapToSrc[ Index + 1 ].Rva1 > Rva );
    }

    if ( pOmapToSrc[ Index ].Rva2 == 0 )
    {
        return 0;
    }

    unsigned Offset = Rva - pOmapToSrc[ Index ].Rva1;

    return pOmapToSrc[ Index ].Rva2 + Offset;
}



SYMNODE*
PdbSymbolHandler::AddPrimarySymbol(
    SYMNODE*  ParentSym,
    LPCSTR    SymName,
    ULONG     SymSort,
    ULONG     SymType,
    ULONG     SymSize,
    ULONG     RvaInImg,
    ULONG     RvaOrig
    )
{
    SYMNODE* SymNode = CreateSymbol( ParentSym, SymName, SymSort, SymType, SymSize, RvaInImg, RvaOrig );

    if ( SymNode != NULL )
    {
        if ( PrimarySymTableCount < SymTableMax )
        {
            PrimarySymTable[ PrimarySymTableCount++ ] = SymNode;
        }
    }

    return SymNode;
}


SYMNODE*
PdbSymbolHandler::AddPrimarySymbol(
    SYMNODE*  ParentSym,
    LPCSTR    SymName,
    ULONG     SymSort,
    ULONG     SymType,
    ULONG     SymSize,
    ULONG     RvaInImg
    )
{
    ULONG RvaOrig = TranslateOmapFromImgToSrc( RvaInImg );

    return AddPrimarySymbol( ParentSym, SymName, SymSort, SymType, SymSize, RvaInImg, RvaOrig );
}


SYMNODE*
PdbSymbolHandler::AddFragmentSymbol(
    SYMNODE*  ParentSym,
    LPCSTR    SymName,
    ULONG     SymSort,
    ULONG     SymType,
    ULONG     SymSize,
    ULONG     RvaInImg,
    ULONG     RvaOrig
    )
{
    SYMNODE* SymNode = CreateSymbol( ParentSym, SymName, SymSort, SymType, SymSize, RvaInImg, RvaOrig );

    if ( SymNode != NULL )
    {
        if ( FragmentSymTableCount < FragmentSymTableMax )
        {
            FragmentSymTable[ FragmentSymTableCount++ ] = SymNode;
        }
    }

    return SymNode;
}



SYMNODE*
PdbSymbolHandler::CreateSymbol(
    SYMNODE*  ParentSym,
    LPCSTR    SymName,
    ULONG     SymSort,
    ULONG     SymType,
    ULONG     SymSize,
    ULONG     RvaInImg,
    ULONG     RvaOrig
    )
{
    //
    //  BUGBUG: Deal with duplicate symbols (maybe their type index was
    //          different, but that still leaves us with two identical
    //          addr/symname/symtype objects).
    //

    ASSERT( RvaInImg < 0x80000000 );
    ASSERT(( SymSort >> 16 ) == 0 );
    ASSERT(( SymType >> 16 ) == 0 );

    if ( RvaInImg < 0x80000000 )
    {
        NAMENODE* NameId = NameTable.InsertName( SymName );

        if ( NameId != NULL )
        {
            SYMNODE* SymNode = new SYMNODE;

            if ( SymNode != NULL )
            {
                SymNode->NextSibling = NULL;
                SymNode->FirstChild  = NULL;
                SymNode->OrgNameId   = NULL;
                SymNode->SymNameId   = NameId;
                SymNode->SymSort     = (WORD) SymSort;
                SymNode->SymType     = (WORD) SymType;
                SymNode->SymSize     = SymSize;
                SymNode->RvaInImg    = RvaInImg;
                SymNode->RvaOrig     = RvaOrig;
                SymNode->ParentSym   = ParentSym;

                if ( ParentSym )
                {
                    SymNode->SymSort = (WORD) MIN(( SymSort + ParentSym->SymSort ), 0xFFFF );

                    SYMNODE** Link = &ParentSym->FirstChild;
                    SYMNODE*  Next =  ParentSym->FirstChild;

                    while (( Next ) && ( Next->RvaOrig < RvaOrig ))
                    {
                        Link = &Next->NextSibling;
                        Next =  Next->NextSibling;
                    }

                    SymNode->NextSibling = Next;
                    *Link = SymNode;
                }

                return SymNode;
            }
        }
    }

    return NULL;
}


VOID
__fastcall
PdbSymbolHandler::ConvertDollarSymbols(
    SYMNODE* ParentSym
    )
{
    //
    //  For each child of this parent that is named $L, $S, or $E, change
    //  the name to ascending sequential value.  The sibling list should be
    //  in original RVA ascending order.
    //

    ULONG LastL = 0;
    ULONG LastE = 0;
    ULONG LastS = 0;

    ULONG LastRvaL = 0;
    ULONG LastRvaE = 0;
    ULONG LastRvaS = 0;

    for ( SYMNODE* Child = ParentSym->FirstChild; Child; Child = Child->NextSibling )
    {
        LPCSTR pszName = Child->SymNameId->Name;

        if (( pszName ) && ( pszName[ 0 ] == '$' ))
        {
            switch ( pszName[ 1 ] )
            {
                case 'L':

                    ASSERT( Child->RvaInImg >= LastRvaL );

                    if ( Child->RvaInImg != LastRvaL )
                    {
                        LastRvaL = Child->RvaInImg;
                        ++LastL;
                    }

                    sprintf( szNewSymName, "$LL_%u", LastL );
                    Child->OrgNameId = Child->SymNameId;
                    Child->SymNameId = NameTable.InsertName( szNewSymName );
                    break;

                case 'E':

                    ASSERT( Child->RvaInImg >= LastRvaE );

                    if ( Child->RvaInImg != LastRvaE )
                    {
                        LastRvaE = Child->RvaInImg;
                        ++LastE;
                    }

                    sprintf( szNewSymName, "$EE_%u", LastE );
                    Child->OrgNameId = Child->SymNameId;
                    Child->SymNameId = NameTable.InsertName( szNewSymName );
                    break;

                case 'S':

                    ASSERT( Child->RvaInImg >= LastRvaS );

                    if ( Child->RvaInImg != LastRvaS )
                    {
                        LastRvaS = Child->RvaInImg;
                        ++LastS;
                    }

                    sprintf( szNewSymName, "$SS_%u", LastS );
                    Child->OrgNameId = Child->SymNameId;
                    Child->SymNameId = NameTable.InsertName( szNewSymName );
                    break;

                case '$':

                    break;      // Module\chkstk.obj\$$$00001

                default:

                    DEBUGCODE( printf( "\nUnrecognized $-prefixed symbol %s\n", pszName ));
                    break;

            }
        }

        ConvertDollarSymbols( Child );
    }
}



void
__forceinline
SwapSymTableEntries(
    SYMNODE** One,
    SYMNODE** Two
    )
{
    SYMNODE *Tmp = *One;
            *One = *Two;
            *Two =  Tmp;
}


VOID
__fastcall
QsortSymTableByRvaInImg(
    SYMNODE** LowerBound,
    SYMNODE** UpperBound
    )
{
    SYMNODE** Lower = LowerBound;
    SYMNODE** Upper = UpperBound;
    SYMNODE** Pivot = Lower + (( Upper - Lower ) / 2 );
    ULONG  PivotRva = (*Pivot)->RvaInImg;

    do
    {
        while (( Lower <= Upper ) && ( (*Lower)->RvaInImg <= PivotRva ))
        {
            ++Lower;
        }

        while (( Upper >= Lower ) && ( (*Upper)->RvaInImg >= PivotRva ))
        {
            --Upper;
        }

        if ( Lower < Upper )
        {
            SwapSymTableEntries( Lower++, Upper-- );
        }
    }
    while ( Lower <= Upper );

    if ( Lower < Pivot )
    {
        SwapSymTableEntries( Lower, Pivot );
        Pivot = Lower;
    }
    else if ( Upper > Pivot )
    {
        SwapSymTableEntries( Upper, Pivot );
        Pivot = Upper;
    }

    if ( LowerBound < ( Pivot - 1 ))
    {
        QsortSymTableByRvaInImg( LowerBound, Pivot - 1 );
    }

    if (( Pivot + 1 ) < UpperBound )
    {
        QsortSymTableByRvaInImg( Pivot + 1, UpperBound );
    }
}


VOID
__fastcall
QsortSymTableByRvaInSrc(
    SYMNODE** LowerBound,
    SYMNODE** UpperBound
    )
{
    SYMNODE** Lower = LowerBound;
    SYMNODE** Upper = UpperBound;
    SYMNODE** Pivot = Lower + (( Upper - Lower ) / 2 );
    ULONG  PivotRva = (*Pivot)->RvaOrig;

    do
    {
        while (( Lower <= Upper ) && ( (*Lower)->RvaOrig <= PivotRva ))
        {
            ++Lower;
        }

        while (( Upper >= Lower ) && ( (*Upper)->RvaOrig >= PivotRva ))
        {
            --Upper;
        }

        if ( Lower < Upper )
        {
            SwapSymTableEntries( Lower++, Upper-- );
        }
    }
    while ( Lower <= Upper );

    if ( Lower < Pivot )
    {
        SwapSymTableEntries( Lower, Pivot );
        Pivot = Lower;
    }
    else if ( Upper > Pivot )
    {
        SwapSymTableEntries( Upper, Pivot );
        Pivot = Upper;
    }

    if ( LowerBound < ( Pivot - 1 ))
    {
        QsortSymTableByRvaInSrc( LowerBound, Pivot - 1 );
    }

    if (( Pivot + 1 ) < UpperBound )
    {
        QsortSymTableByRvaInSrc( Pivot + 1, UpperBound );
    }
}


int
__forceinline
__fastcall
MyStrCmp(
    const char* One,
    const char* Two
    )
{
    const unsigned char *p = (const unsigned char*) One;
    const unsigned char *q = (const unsigned char*) Two;

    while (( *p == *q ) && ( *p != 0 ))
    {
        ++p;
        ++q;
    }

    return ( *p - *q );
}


int
__forceinline
__fastcall
SymNodeCompareNames(
    SYMNODE* One,
    SYMNODE* Two
    )
{
    if (( One->SymSort - Two->SymSort ) != 0 )
    {
        return ( One->SymSort - Two->SymSort );
    }

    return strcmp( One->SymNameId->Name, Two->SymNameId->Name );
}


void PdbSymbolHandler::BubbleSortPrimarySymTable( void )
{
    //
    //  Already qsorted by RvaInImg, now fine tune it by name
    //  within each RvaInImg.
    //

    for ( unsigned i = 0; i < PrimarySymTableCount - 1; i++ )
    {
        ULONG RvaInImg = PrimarySymTable[ i ]->RvaInImg;

        for ( unsigned e = i + 1; e < PrimarySymTableCount; e++ )
        {
            if ( PrimarySymTable[ e ]->RvaInImg != RvaInImg )
            {
                break;
            }
        }

        bool Changes = true;

        while (( i < --e ) && ( Changes ))
        {
            Changes = false;

            for ( unsigned j = i; j < e; j++ )
            {
                SYMNODE* p = PrimarySymTable[ j ];
                SYMNODE* q = PrimarySymTable[ j + 1 ];

                ASSERT( p->RvaInImg == q->RvaInImg );

                if ( SymNodeCompareNames( p,  q ) > 0 )
                {
                    SwapSymTableEntries( &PrimarySymTable[ j ], &PrimarySymTable[ j + 1 ] );
                    Changes = true;
                }
            }
        }
    }
}


unsigned
__fastcall
FirstSrcSymTableIndexForRva(
    SYMNODE** SymTableSortedByOrig,
    ULONG     SymTableCount,
    ULONG     RvaInSrc
    )
{
    unsigned Lo = 0;
    unsigned Hi = SymTableCount;

    while ( Lo < Hi )
    {
        unsigned Mi = ( Lo + Hi ) / 2;

        if ( SymTableSortedByOrig[ Mi ]->RvaOrig == RvaInSrc )
        {
            while (( Mi > 0 ) && ( SymTableSortedByOrig[ Mi - 1 ]->RvaOrig == RvaInSrc ))
            {
                --Mi;
            }

            return Mi;
        }
        else if ( SymTableSortedByOrig[ Mi ]->RvaOrig < RvaInSrc )
        {
            Lo = Mi + 1;
        }
        else
        {
            Hi = Mi;
        }
    }

    ASSERT(( Lo == SymTableCount ) || ( SymTableSortedByOrig[ Lo ]->RvaOrig > RvaInSrc ));
    ASSERT(( Lo == 0 ) || ( SymTableSortedByOrig[ Lo - 1 ]->RvaOrig < RvaInSrc ));

    if ( Lo != 0 )
    {
        unsigned NearestRva = SymTableSortedByOrig[ --Lo ]->RvaOrig;

        while (( Lo > 0 ) && ( SymTableSortedByOrig[ Lo - 1 ]->RvaOrig == NearestRva ))
        {
            --Lo;
        }
    }

    ASSERT( Lo < SymTableCount );
    ASSERT( SymTableSortedByOrig[ Lo ]->RvaOrig < RvaInSrc );
    ASSERT(( Lo == 0 ) || ( SymTableSortedByOrig[ Lo - 1 ]->RvaOrig < SymTableSortedByOrig[ Lo ]->RvaOrig ));

    return Lo;
}


extern "C"
LPSTR
__cdecl
__unDName(
    LPSTR  OutputBuffer,
    LPCSTR InputName,
    INT    OutputBufferSize,
    PVOID,
    PVOID,
    USHORT Flags
    );


void
UndecNameForRoughComparison(
    LPCSTR DecoratedName,
    LPSTR  Undecorated
    )
{
    LPCSTR p = DecoratedName;

    *Undecorated = 0;

    while ( *p == '_' )
    {
        ++p;
    }

    if (( p[ 0 ] == 'i' ) &&
        ( p[ 1 ] == 'm' ) &&
        ( p[ 2 ] == 'p' ) &&
        ( p[ 3 ] == '_' ))
    {
        p += 4;

        while ( *p == '_' )
        {
            ++p;
        }
    }

    if ( *p == '?' )
    {
        __unDName( Undecorated, p, 0x10000, malloc, free, UNDNAME_NAME_ONLY );
    }
    else
    {
        //
        //  Strip any preceding '_' or '@'.
        //

        while (( *p == '@' ) || ( *p == '_' ))
        {
            ++p;
        }

        strcpy( Undecorated, p );

        //
        //  Strip any trailing @nnn.
        //

        for ( LPSTR q = Undecorated; *q; q++ )
        {
            if ( *q == '@' )
            {
                 *q = 0;
                 break;
            }
        }
    }
}


VOID
PdbSymbolHandler::DetermineSymbolConfidence(
    VOID
    )
{
    //
    //  At this point the public and image symbols should be loaded.
    //

    PCHAR PeUndecName = (PCHAR) malloc( 0x10000 );
    PCHAR CvUndecName = (PCHAR) malloc( 0x10000 );

    SymsMatched = 0;
    SymsUnmatch = 0;

    ASSERT( PrimarySymTableCount > 1 );

    QsortSymTableByRvaInImg(
        &PrimarySymTable[ 0 ],
        &PrimarySymTable[ PrimarySymTableCount - 1 ]
        );

#if 0
    printf( "\nSyms loaded before DetermineSymbolConfidence:\n" );
    DumpSyms();
    printf( "\n" );
#endif

    for ( unsigned i = 0; i < PrimarySymTableCount; i++ )
    {
        if (( PrimarySymTable[ i ]->SymType == 0xAA85 ) ||   // export by name
            ( PrimarySymTable[ i ]->SymType == 0xAA86 ))     // import by name
        {
            SYMNODE* PeSym = PrimarySymTable[ i ];

            UndecNameForRoughComparison( PeSym->SymNameId->Name, PeUndecName );

            ULONG RvaInImg = PeSym->RvaInImg;

            bool CvSymMatch = false;
            bool Forwarder  = false;

            unsigned j = i;

            while (( j > 0 ) && ( PrimarySymTable[ j - 1 ]->RvaInImg == RvaInImg ))
            {
                --j;
            }

            do
            {
                switch ( PrimarySymTable[ j ]->SymType )
                {
                    case 0xAA95:        // forwarder

                        Forwarder = true;
                        break;

                    case cvsymtLData32_Sz:
                    case cvsymtGData32_Sz:
                    case cvsymtPub32_Sz:
                    case cvsymtLProc32_Sz:
                    case cvsymtGProc32_Sz:

                        UndecNameForRoughComparison( PrimarySymTable[ j ]->SymNameId->Name, CvUndecName );

                        if ( strcmp( PeUndecName, CvUndecName ) == 0 )
                        {
                            CvSymMatch = true;
                            ++SymsMatched;
                        }

                        break;
                }
            }
            while (( ++j < PrimarySymTableCount ) &&
                   ( PrimarySymTable[ j ]->RvaInImg == RvaInImg ));

            if (( ! CvSymMatch ) && ( ! Forwarder ))
            {
                ++SymsUnmatch;

#ifdef DEBUG
                printf( "No pdb symbol name or forwarder for PE import/export:\n" );
                DumpSym( PeSym );

                unsigned j = i;

                while (( j > 0 ) && ( PrimarySymTable[ j - 1 ]->RvaInImg == RvaInImg ))
                {
                    --j;
                }

                do
                {
                    if ( PrimarySymTable[ j ] != PeSym )
                    {
                        printf( "\t" );
                        DumpSym( PrimarySymTable[ j ] );
                    }
                }
                while (( ++j < PrimarySymTableCount ) &&
                       ( PrimarySymTable[ j ]->RvaInImg == RvaInImg ));

                printf( "\n" );
#endif
            }
        }
    }

    free( PeUndecName );
    free( CvUndecName );

    if (( SymsMatched + SymsUnmatch ) != 0 )
    {
        SymConfidence = ( SymsMatched * 100 ) / ( SymsMatched + SymsUnmatch );
    }
    else
    {
        SymConfidence = 100;    // pure forwarder or resource dll
    }
}



BOOL PdbSymbolHandler::ValidateOmap( void )
{
    if (( nOmapToImg == 0 ) && ( nOmapToSrc == 0 ))
    {
        return TRUE;     // no omap to verify
    }

    BOOL Success = TRUE;

    //
    //  Verify both from and to omaps are loaded.
    //

    if ( nOmapToImg == 0 )
    {
        fprintf( stderr, "SYM7: No SrcToImg OMAP table!\n" );
        Success = FALSE;
    }

    if ( nOmapToSrc == 0 )
    {
        fprintf( stderr, "SYM7: No ImgToSrc OMAP table!\n" );
        Success = FALSE;
    }

    //
    //  Verify each omap is sorted ascending by Rva1.
    //

    for ( unsigned i = 0; i < nOmapToImg - 1; i++ )
    {
        if ( pOmapToImg[ i ].Rva1 > pOmapToImg[ i + 1 ].Rva1 )
        {
            fprintf(
                stderr,
                "SYM7: SrcToImg OMAP not sorted ascending: [%u]=%u, [%u]=%u\n",
                i,
                pOmapToImg[ i ].Rva1,
                i + 1,
                pOmapToImg[ i + 1 ].Rva1
                );

            Success = FALSE;
        }
    }

    for ( unsigned i = 0; i < nOmapToSrc - 1; i++ )
    {
        if ( pOmapToSrc[ i ].Rva1 > pOmapToSrc[ i + 1 ].Rva1 )
        {
            fprintf(
                stderr,
                "SYM7: ImgToSrc OMAP not sorted ascending: [%u]=%u, [%u]=%u\n",
                i,
                pOmapToSrc[ i ].Rva1,
                i + 1,
                pOmapToSrc[ i + 1 ].Rva1
                );

            Success = FALSE;
        }
    }

    //
    //  For each Omap entry in ImgToSrc that is nonzero, verify that
    //  SrcToImg entry produces correct round trip value.
    //

    for ( unsigned i = 0; i < nOmapToSrc; i++ )
    {
        if (( pOmapToSrc[ i ].Rva1 != 0 ) &&
            ( pOmapToSrc[ i ].Rva2 != 0 ))
        {
            ULONG RvaInImg1 = pOmapToSrc[ i ].Rva1;
            ULONG RvaInSrc1 = pOmapToSrc[ i ].Rva2;

            ULONG RvaInImg2 = TranslateOmapFromSrcToImg( RvaInSrc1 );

            if ( RvaInImg1 != RvaInImg2 )
            {
                fprintf(
                    stderr,
                    "SYM7: OMAP round trip failed: img %06X -> src %06X\n"
                    "                          but src %06X -> img %06X\n",
                    RvaInImg1,
                    RvaInSrc1,
                    RvaInSrc1,
                    RvaInImg2
                    );

                Success = FALSE;
            }
        }
    }

    //
    //  For each Omap entry in SrcToImg that is nonzero, verify that
    //  ImgToSrc entry produces correct round trip value.
    //

    for ( unsigned i = 0; i < nOmapToImg; i++ )
    {
        if (( pOmapToImg[ i ].Rva1 != 0 ) &&
            ( pOmapToImg[ i ].Rva2 != 0 ))
        {
            ULONG RvaInSrc1 = pOmapToImg[ i ].Rva1;
            ULONG RvaInImg1 = pOmapToImg[ i ].Rva2;

            ULONG RvaInSrc2 = TranslateOmapFromImgToSrc( RvaInImg1 );

            if ( RvaInSrc1 != RvaInSrc2 )
            {
                fprintf(
                    stderr,
                    "SYM7: OMAP round trip failed: src %06X -> img %06X\n"
                    "                          but img %06X -> src %06X\n",
                    RvaInSrc1,
                    RvaInImg1,
                    RvaInImg1,
                    RvaInSrc2
                    );

                Success = FALSE;
            }
        }
    }

    return Success;
}


ULONG
__fastcall
DecimalStrToUnsigned32(
    IN LPCSTR Text
    )
    {
    LPCSTR p = Text;
    ULONG  n = 0;

    //
    //  Very simplistic conversion stops at first non digit character, does
    //  not require null-terminated string, and does not skip any whitespace
    //  or commas.
    //

    while (( *p >= '0' ) && ( *p <= '9' )) {
        n = ( n * 10 ) + ( *p++ - '0' );
        }

    return n;
    }


ULONG
__fastcall
HexStrToUnsigned32(
    IN LPCSTR HexStr
    )
    {
    LPCSTR p = HexStr;
    ULONG  Value = 0;
    ULONG  NewValue;
    ULONG  Digit;

    for (;;) {

        Digit = ( *p++ ) - 0x30;

        NewValue = Value << 4;

        if ( Digit > 9 ) {

            Digit = ( Digit & 0x1F ) - 7;

            if (( Digit > 15 ) || ( Digit < 10 )) {

                return Value;
                }
            }

        Value = NewValue | Digit;

        }
    }


char* __fastcall TimeText( ULONG TimeValue, char* pszBuffer )
{
    //
    //  FILETIME units are 100ns intervals from 01/01/1601.
    //
    //  ULONG units are seconds from 01/01/1970.
    //
    //  There are 10,000,000 FILETIME units in one second.
    //
    //  There are 11,644,473,600 seconds from 01/01/1601 to 01/01/1970.
    //

    #define FILETIME_UNITS_PER_SECOND      10000000
    #define FILETIME_TO_ULONG_OFFSET    11644473600

    SYSTEMTIME SysTime;

    memset( &SysTime, 0, sizeof( SysTime ));

    union
    {
        FILETIME ft;
        QWORD    qw;
    } FileTime;

    FileTime.qw = ( TimeValue + FILETIME_TO_ULONG_OFFSET ) * FILETIME_UNITS_PER_SECOND;

    FileTimeToSystemTime( &FileTime.ft, &SysTime );

    sprintf(
        pszBuffer,
        "%04u/%02u/%02u %02u:%02u:%02u UTC",
        SysTime.wYear,
        SysTime.wMonth,
        SysTime.wDay,
        SysTime.wHour,
        SysTime.wMinute,
        SysTime.wSecond
        );

    return pszBuffer;
}


const struct { unsigned cp; const char* text; } CodePageName[] = {
        {    0,  "ASCII"                     },
        {  874,  "Windows, Thai"             },
        {  932,  "Windows, Japan"            },
        {  936,  "Windows, Chinese"          },
        {  949,  "Windows, Korean"           },
        {  950,  "Windows, Taiwan"           },
        { 1200,  "Unicode"                   },
        { 1250,  "Windows, Eastern European" },
        { 1251,  "Windows, Cyrillic"         },
        { 1252,  "Windows, ANSI"             },
        { 1253,  "Windows, Greek"            },
        { 1254,  "Windows, Turkish"          },
        { 1255,  "Windows, Hebrew"           },
        { 1256,  "Windows, Arabic"           },
        { 1257,  "Windows, Baltic"           },
        { 1361,  "Windows, Korean (Johab)"   }
};



char* __fastcall LangText( ULONG LangValue, char* pszBuffer )
{
    //
    //  Following table was taken from filever.exe sources and updated
    //  from Win32 help files.
    //

    CHAR szLanguage[ 256 ];

    *szLanguage = 0;

    VerLanguageName( LangValue >> 16, szLanguage, sizeof( szLanguage ));

    LangValue &= 0xFFFF;

    CHAR szCpDefault[ 8 ];

    sprintf( szCpDefault, "%u", LangValue );

    const char* pszCodePage = szCpDefault;

    for ( unsigned i = 0; i < ( sizeof( CodePageName ) / sizeof( CodePageName[ 0 ] )); i++ )
    {
        if ( CodePageName[ i ].cp == LangValue )
        {
            pszCodePage = CodePageName[ i ].text;
            break;
        }
    }

    sprintf(
        pszBuffer,
        "%s%s%s",
        szLanguage,
        ( *szLanguage && *pszCodePage ) ? ", " : "",
        pszCodePage
        );

    return pszBuffer;
}


void __declspec( noreturn ) Usage( void )
{
    printf(
        "\n"
        "Usage: sym7 [options] exefile sympath [outfile]\n"
        "\n"
        "            /sizes          show symbol sizes where available\n"
        "            /zeros          show symbols deleted by optimization\n"
        "            /originalrvas   show original pre-optimized addresses\n"
        "            /originalnames  show original symbol names if converted\n"
        "            /confidence:nn  reject if confidence below nn%% (default 50%)\n"
        "            /nopdb          show PE symbols only, don't open pdb file\n"
        "            /nosymfail      if can't open pdb, still show PE symbols\n"
        "\n"
        );
    exit( 1 );
}


void __cdecl main( int argc, char *argv[] )
{
    LPSTR ExeName = NULL;
    LPSTR PdbPath = NULL;
    LPSTR OutName = NULL;
    BOOL  SizeComments = FALSE;
    BOOL  ZeroSymbols  = FALSE;
    BOOL  OriginalRva  = FALSE;
    BOOL  OriginalName = FALSE;
    BOOL  NoPdb        = FALSE;
    BOOL  NoSymFail    = FALSE;
    ULONG Confidence   = 50;

    for ( int arg = 1; arg < argc; arg++ )
    {
        LPSTR p = argv[ arg ];

        if ( strchr( p, '?' ))
        {
            Usage();
        }

        if (( *p == '/' ) || ( *p == '-' ))
        {
            ++p;

            if (( _stricmp( p, "sizes" ) == 0 ) ||
                ( _stricmp( p, "size"  ) == 0 ))
            {
                SizeComments = TRUE;
            }
            else if (( _stricmp( p, "zeros" ) == 0 ) ||
                     ( _stricmp( p, "zero"  ) == 0 ))
            {
                ZeroSymbols = TRUE;
            }
            else if (( _stricmp( p, "originalrva"  ) == 0 ) ||
                     ( _stricmp( p, "originalrvas" ) == 0 ))
            {
                OriginalRva = TRUE;
            }
            else if (( _stricmp( p, "originalname"  ) == 0 ) ||
                     ( _stricmp( p, "originalnames" ) == 0 ))
            {
                OriginalName = TRUE;
            }
            else if (( _stricmp( p, "nopdb" ) == 0 ) ||
                     ( _stricmp( p, "nodbg" ) == 0 ))
            {
                NoPdb     = TRUE;
                NoSymFail = TRUE;
            }
            else if (( _stricmp( p, "nosymfail" ) == 0 ) ||
                     ( _stricmp( p, "nodbgfail" ) == 0 ) ||
                     ( _stricmp( p, "nopdbfail" ) == 0 ))
            {
                NoSymFail = TRUE;
            }
            else if (( _memicmp( p, "confidence:", strlen( "confidence:" )) == 0 ) ||
                     ( _memicmp( p, "conf:",       strlen( "conf:"       )) == 0 ))
            {
                Confidence = strtoul( strchr( p, ':' ) + 1, 0, 10 );
            }
            else
            {
                Usage();
            }
        }
        else if ( ExeName == NULL )
        {
            ExeName = p;
        }
        else if ( PdbPath == NULL )
        {
            PdbPath = p;
        }
        else if ( OutName == NULL )
        {
            OutName = p;
        }
        else
        {
            Usage();
        }
    }

    if ( ExeName == NULL )
    {
        Usage();
    }

    if ( PdbPath == NULL )
    {
        PdbPath = ".";
    }

    PdbSymbolHandler PdbHandler;

    BOOL Success = PdbHandler.OpenExe( ExeName );

    if ( ! Success )
    {
        switch ( GetLastError() )
        {
            case ERROR_FILE_NOT_FOUND:

                fprintf( stderr, "SYM7: Failed to find/open exe %s\n", ExeName );
                break;

            case ERROR_BAD_EXE_FORMAT:

                fprintf( stderr, "SYM7: File %s not a valid Win32 executable image\n", ExeName );
                break;

            default:

                fprintf( stderr, "SYM7: Error %u trying to open %s\n", GetLastError(), ExeName );
                break;
        }

        exit( 1 );
    }

    if (( ! PdbHandler.AnyDebugDirectories   ) &&
        ( ! PdbHandler.AnyExecutableSections ) &&
        ( ! PdbHandler.AnyWritableSections   ))
    {
        //
        //  Usually a resource-only DLL, just generate a PE map.
        //

        NoPdb = TRUE;
    }

    if ( ! NoPdb )
    {
        Success = PdbHandler.OpenPdb( ExeName, PdbPath );

        if ( ! Success )
        {
            if ( NoSymFail )
            {
                fprintf( stderr, "SYM7: WARNING: Could not find/open pdb file for %s\n", ExeName );
            }
            else
            {
                switch ( GetLastError() )
                {
                    case ERROR_FILE_NOT_FOUND:

                        fprintf( stderr, "SYM7: Could not find/open pdb file for %s\n", ExeName );
                        break;

                    case ERROR_OPEN_FAILED:

                        fprintf( stderr, "SYM7: %s %s appears to be corrupt\n", ExeName, PdbHandler.PdbPath );
                        break;

                    default:

                        fprintf( stderr, "SYM7: Error %u trying to find/open pdb file\n", GetLastError() );
                        break;
                }

                exit( 1 );
            }
        }
    }

    Success = PdbHandler.LoadSymbols( Confidence );

    if ( ! Success )
    {
        fprintf( stderr, "SYM7: Failed loading symbols for %s\n", ExeName );
        exit( 1 );
    }

    FILE* outfile = stdout;

    if ( OutName )
    {
        outfile = fopen( OutName, "wt" );

        if ( outfile == NULL )
        {
            fprintf( stderr, "SYM7: Failed to open output file %s\n", OutName );
            exit( 1 );
        }
    }

    static CHAR szBuffer[ 4096 ];

    _strlwr( PdbHandler.ExePath );
    _strlwr( PdbHandler.DbgPath );
    _strlwr( PdbHandler.PdbPath );

    fprintf( outfile, "\n" );
    fprintf( outfile, "[PsymInfo]\n" );
    fprintf( outfile, "\n" );
    fprintf( outfile, "PsymVer=%u.%u\n\n", PsymMajorVersion, PsymMinorVersion );

    if (( PdbHandler.ExeName ) && ( *PdbHandler.ExeName ))
    {
        fprintf( outfile, "ExeName=\"%s\"\n", PdbHandler.ExeName );
    }

    if ( *PdbHandler.ExePath )
    {
        fprintf( outfile, "ExePath=\"%s\"\n", PdbHandler.ExePath );
    }

    if ( PdbHandler.ExeFileMD5.Word32[ 0 ] != 0 )
    {
        Md5ToHexStr( &PdbHandler.ExeFileMD5, szBuffer );
        fprintf( outfile, "ExeHash=%s\n", szBuffer );
    }

    fprintf( outfile, "ExeSize=%08X          ; (%u)\n", PdbHandler.ExeFileSize, PdbHandler.ExeFileSize );

    fprintf( outfile, "ExeTime=%08X          ; (%s)\n", PdbHandler.ExeTime, TimeText( PdbHandler.ExeTime, szBuffer ));

    for ( unsigned i = 0; i < PdbHandler.TranslationCount; i++ )
    {
        DWORD DisplayLang = ( PdbHandler.TranslationArray[ i ] >> 16 ) |
                            ( PdbHandler.TranslationArray[ i ] << 16 );

        fprintf( outfile, "ExeLang=%08X          ; (%s)\n", DisplayLang, LangText( DisplayLang, szBuffer ));
    }

    if ( PdbHandler.ExeVersion != 0 )
    {
        fprintf( outfile, "ExeVer =%016I64X  ; (%u.%u.%u.%u)\n",
            PdbHandler.ExeVersion,
            (WORD)( PdbHandler.ExeVersion >> 48 ),
            (WORD)( PdbHandler.ExeVersion >> 32 ),
            (WORD)( PdbHandler.ExeVersion >> 16 ),
            (WORD)( PdbHandler.ExeVersion >> 0  )
            );
    }

    fprintf( outfile, "\n" );

    if ( *PdbHandler.DbgPath )
    {
        if (( PdbHandler.DbgNameInExe ) && ( *PdbHandler.DbgNameInExe ))
        {
            fprintf( outfile, "DbgName=\"%s\"\n", PdbHandler.DbgNameInExe );
        }
        else if (( PdbHandler.DbgName ) && ( *PdbHandler.DbgName ))
        {
            fprintf( outfile, "DbgName=\"%s\"\n", PdbHandler.DbgName );
        }

        fprintf( outfile, "DbgPath=\"%s\"\n", PdbHandler.DbgPath );

        if ( PdbHandler.DbgFileMD5.Word32[ 0 ] != 0 )
        {
            Md5ToHexStr( &PdbHandler.DbgFileMD5, szBuffer );
            fprintf( outfile, "DbgHash=%s\n", szBuffer );
        }

        fprintf( outfile, "DbgSize=%08X          ; (%u)\n", PdbHandler.DbgFileSize, PdbHandler.DbgFileSize );

        fprintf( outfile, "DbgTime=%08X          ; (%s)\n", PdbHandler.DbgTime, TimeText( PdbHandler.DbgTime, szBuffer ));

        if ( PdbHandler.DbgOmap )
        {
            fprintf( outfile, "DbgOmap=1                 ; (%u,%u)\n", PdbHandler.nOmapToSrc, PdbHandler.nOmapToImg );
        }

        fprintf( outfile, "\n" );
    }

    if ( PdbHandler.pdb )
    {
        if (( PdbHandler.PdbName ) && ( *PdbHandler.PdbName ))
        {
            fprintf( outfile, "PdbName=\"%s\"\n", PdbHandler.PdbName );
        }

        if ( *PdbHandler.PdbPath )
        {
            fprintf( outfile, "PdbPath=\"%s\"\n", PdbHandler.PdbPath );
        }

        if ( PdbHandler.PdbFileMD5.Word32[ 0 ] != 0 )
        {
            Md5ToHexStr( &PdbHandler.PdbFileMD5, szBuffer );
            fprintf( outfile, "PdbHash=%s\n", szBuffer );
        }

        if ( PdbHandler.PdbFileSize != 0 )
        {
            fprintf( outfile, "PdbSize=%08X          ; (%u)\n", PdbHandler.PdbFileSize, PdbHandler.PdbFileSize );
        }

        if ( PdbHandler.PdbTime != 0 )
        {
            fprintf( outfile, "PdbTime=%08X          ; (%s)\n", PdbHandler.PdbTime, TimeText( PdbHandler.PdbTime, szBuffer ));
        }

        if ( PdbHandler.PdbOmap )
        {
            fprintf( outfile, "PdbOmap=1                 ; (%u,%u)\n", PdbHandler.nOmapToSrc, PdbHandler.nOmapToImg );
        }

        if ( PdbHandler.CountOfModSyms )
        {
            fprintf( outfile, "PdbPriv=1                 ; (%u,%u)\n", PdbHandler.CountOfModSyms, PdbHandler.CountOfGlobals );
        }

        fprintf( outfile, "PdbConf=" );

        unsigned cbOut = fprintf( outfile, "%u%%", PdbHandler.SymConfidence );

        fprintf( outfile, "%*s              ; (%u:%u)\n", ( 4 - cbOut ), "", PdbHandler.SymsMatched, PdbHandler.SymsUnmatch );

        fprintf( outfile, "\n" );
    }

#ifdef SYMCOUNTER

    for ( unsigned i = 0; i < 0x4000; i++ )
    {
        if ( SymTypeCounter[ i ] )
        {
            printf( "SymTypeCounter[ 0x%X ] = %u\n", i, SymTypeCounter[ i ] );
        }
    }

    printf( "\n" );

#endif

#ifdef DEBUG

    if ( PdbHandler.dbi )
    {
        PdbHandler.dbi->DumpMods();
    }

    PdbHandler.DumpSyms();

    PdbHandler.DumpOmapToImg();

    PdbHandler.DumpOmapToSrc();

#endif

    if (( SizeComments ) && ( PdbHandler.nOmapToSrc ) && ( PdbHandler.CountOfModSyms ))
    {
        fprintf( outfile, "Note: Module symbol sizes represent original contiguous sizes.\n\n" );
    }

    fprintf( outfile, "[Symbols]\n" );
    fprintf( outfile, "\n" );

    SYMNODE*  LastParentSym = NULL;
    NAMENODE* LastSymNameId = NULL;
    ULONG     LastRva       = 0;

    for ( unsigned i = 0; i < PdbHandler.PrimarySymTableCount; i++ )
    {
        *szBuffer = 0;

        SYMNODE* SymNode = PdbHandler.PrimarySymTable[ i ];

        if (( SymNode->RvaInImg  == LastRva       ) &&
            ( SymNode->SymNameId == LastSymNameId ) &&
            ( SymNode->ParentSym == LastParentSym ))
        {
            //
            //  Don't print duplicate symbols (might be different size/type).
            //

            continue;
        }

        if (( SymNode->RvaInImg == 0 ) && ( SymNode->SymType != 0xAA01 ))
        {
            //
            //  Don't print symbols with address zero (BBT-removed symbols)
            //  unless the symbol is the PE header symbol.
            //

            continue;
        }

        if ( SymNode->SymType == 0xAA03 )   // section header
        {
            fprintf( outfile, "\n" );       // blank line before new section
        }

        PdbHandler.BuildScopedSymbolName( szBuffer, SymNode->ParentSym );

        fprintf(
            outfile,
            "%06X=\"%s%s\"",
            SymNode->RvaInImg,
            szBuffer,
            SymNode->SymNameId->Name
            );

        BOOL CommentSeparatorPrinted = FALSE;

        if (( OriginalRva ) &&
            ( SymNode->RvaOrig != SymNode->RvaInImg ) &&
            ( SymNode->RvaOrig != 0 ))
        {
            if ( ! CommentSeparatorPrinted )
            {
                CommentSeparatorPrinted = TRUE;
                fprintf( outfile, " ;" );
            }

            fprintf( outfile, " (orig=%06X)", SymNode->RvaOrig );
        }

        if (( SizeComments ) && ( SymNode->SymSize != 0 ))
        {
            if ( ! CommentSeparatorPrinted )
            {
                CommentSeparatorPrinted = TRUE;
                fprintf( outfile, " ;" );
            }

            fprintf( outfile, " (size=0x%X)", SymNode->SymSize );
        }

        if (( OriginalName ) && ( SymNode->OrgNameId != NULL ))
        {
            if ( ! CommentSeparatorPrinted )
            {
                CommentSeparatorPrinted = TRUE;
                fprintf( outfile, " ;" );
            }

            fprintf( outfile, " (name=\"%s\")", SymNode->OrgNameId->Name );
        }

        fprintf( outfile, "\n" );

        LastRva       = SymNode->RvaInImg;
        LastSymNameId = SymNode->SymNameId;
        LastParentSym = SymNode->ParentSym;
    }

    fprintf( outfile, "\n" );

    if ( ZeroSymbols )
    {
        LastParentSym = NULL;
        LastSymNameId = NULL;
        LastRva       = 0;

        for ( unsigned i = 0; i < PdbHandler.PrimarySymTableCount; i++ )
        {
            *szBuffer = 0;

            SYMNODE* SymNode = PdbHandler.PrimarySymTable[ i ];

            if (( SymNode->RvaInImg  == LastRva       ) &&
                ( SymNode->SymNameId == LastSymNameId ) &&
                ( SymNode->ParentSym == LastParentSym ))
            {
                //
                //  Don't print duplicate symbols (might be different size/type).
                //

                continue;
            }

            if (( SymNode->RvaInImg != 0 ) || ( SymNode->SymType == 0xAA01 ))
            {
                //
                //  Don't print symbols with non-zero address or PE header.
                //

                continue;
            }

            PdbHandler.BuildScopedSymbolName( szBuffer, SymNode->ParentSym );

            fprintf(
                outfile,
                "%06X=\"%s%s\"",
                SymNode->RvaInImg,
                szBuffer,
                SymNode->SymNameId->Name
                );

            BOOL CommentSeparatorPrinted = FALSE;

            if (( OriginalRva ) &&
                ( SymNode->RvaOrig != SymNode->RvaInImg ) &&
                ( SymNode->RvaOrig != 0 ))
            {
                if ( ! CommentSeparatorPrinted )
                {
                    CommentSeparatorPrinted = TRUE;
                    fprintf( outfile, " ;" );
                }

                fprintf( outfile, " (orig=%06X)", SymNode->RvaOrig );
            }

            if (( SizeComments ) && ( SymNode->SymSize != 0 ))
            {
                if ( ! CommentSeparatorPrinted )
                {
                    CommentSeparatorPrinted = TRUE;
                    fprintf( outfile, " ;" );
                }

                fprintf( outfile, " (size=0x%X)", SymNode->SymSize );
            }

            if (( OriginalName ) && ( SymNode->OrgNameId != NULL ))
            {
                if ( ! CommentSeparatorPrinted )
                {
                    CommentSeparatorPrinted = TRUE;
                    fprintf( outfile, " ;" );
                }

                fprintf( outfile, " (name=\"%s\")", SymNode->OrgNameId->Name );
            }

            fprintf( outfile, "\n" );

            LastRva       = SymNode->RvaInImg;
            LastSymNameId = SymNode->SymNameId;
            LastParentSym = SymNode->ParentSym;
        }

        fprintf( outfile, "\n" );
    }

    exit( 0 );
}


