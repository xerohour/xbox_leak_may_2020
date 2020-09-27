
#include <precomp.h>

//
//  pestuff.c
//
//  Author: Tom McGuire (tommcg) 2/97 - 3/98
//
//  Copyright (C) Microsoft, 1997-2000.
//
//  MICROSOFT CONFIDENTIAL
//


#define MAX_SYMBOL_NAME_LENGTH  2048

#if ( _MSC_VER >= 1300 )
    #define DEBUGBREAK __debugbreak();
#else
    #define DEBUGBREAK   DebugBreak();
#endif

#ifndef IMAGE_REL_BASED_SECTION
#define IMAGE_REL_BASED_SECTION 6
#endif

#ifndef IMAGE_REL_BASED_REL32
#define IMAGE_REL_BASED_REL32   7
#endif

BOOL
PeValidateRelocations(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            FileMapped,
    IN ULONG             FileSize
    );

BOOL
PeValidateImports(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            FileMapped,
    IN ULONG             FileSize
    );

BOOL
PeValidateExports(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            FileMapped,
    IN ULONG             FileSize
    );

BOOL
PeValidateResources(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            FileMapped,
    IN ULONG             FileSize
    );


typedef struct _SYMBOL_CONTEXT SYMBOL_CONTEXT, *PSYMBOL_CONTEXT;

struct _SYMBOL_CONTEXT {
    SYMBOL_TREE NewDecoratedSymbolTree;
    SYMBOL_TREE NewUndecoratedSymbolTree;
    SYMBOL_TREE OldUndecoratedSymbolTree;
    ULONG       NewImageBase;
    ULONG       OldImageBase;
    ULONG       SymbolOptionFlags;
    PRIFT_TABLE RiftTable;
#ifdef TESTCODE
    HANDLE      OutFile;
#endif
    };


typedef struct _RELOC_ARRAY_ENTRY RELOC_ARRAY_ENTRY, *PRELOC_ARRAY_ENTRY;

struct _RELOC_ARRAY_ENTRY {
    ULONG  RelocRva;
    UCHAR  RelocType;
    USHORT HighAdjValue;
    };


#ifndef PATCH_APPLY_CODE_ONLY

#ifdef TESTCODE
ULONG CountDecoratedMatches;
ULONG CountUndecoratedMatches;
#endif

LPCSTR ImagehlpImportNames[] = {
           "SymInitialize",
           "SymGetOptions",
           "SymSetOptions",
           "SymLoadModule",
           "SymGetModuleInfo",
           "SymEnumerateSymbols",
           "UnDecorateSymbolName",
           "SymUnloadModule",
           "SymCleanup"
           };

#define COUNT_IMAGEHLP_IMPORTS ( sizeof( ImagehlpImportNames ) / sizeof( ImagehlpImportNames[ 0 ] ))

//
//  NOTE:  Above names must be in SAME ORDER as the prototypes below.
//

union {

    VOID ( __stdcall * Imports[ COUNT_IMAGEHLP_IMPORTS ] )();

    struct {
        BOOL  ( __stdcall * SymInitialize        )( HANDLE, LPCSTR, BOOL );
        DWORD ( __stdcall * SymGetOptions        )( VOID );
        DWORD ( __stdcall * SymSetOptions        )( DWORD );
        DWORD ( __stdcall * SymLoadModule        )( HANDLE, HANDLE, LPCSTR, LPCSTR, DWORD, DWORD );
        BOOL  ( __stdcall * SymGetModuleInfo     )( HANLDE, DWORD, PIMAGEHLP_MODULE );
        BOOL  ( __stdcall * SymEnumerateSymbols  )( HANDLE, DWORD, PSYM_ENUMSYMBOLS_CALLBACK, PVOID );
        BOOL  ( __stdcall * UnDecorateSymbolName )( LPCSTR, LPSTR, DWORD, DWORD );
        BOOL  ( __stdcall * SymUnloadModule      )( HANDLE, DWORD );
        BOOL  ( __stdcall * SymCleanup           )( HANDLE );
        };
    } Imagehlp;


BOOL ImagehlpCritSectInitialized;
CRITICAL_SECTION ImagehlpCritSect;
HANDLE hLibImagehlp;
HANDLE hProc;
IMAGEHLP_MODULE ImagehlpModuleInfo;

VOID
InitImagehlpCritSect(
    VOID
    )
    {
    if ( ! ImagehlpCritSectInitialized ) {
        InitializeCriticalSection( &ImagehlpCritSect );
        ImagehlpCritSectInitialized = TRUE;
        hProc = GetCurrentProcess();
        }
    }


BOOL
LoadImagehlp(
    VOID
    )
    {
    HANDLE hLib;
    ULONG i;

    if ( Imagehlp.Imports[ COUNT_IMAGEHLP_IMPORTS - 1 ] == NULL ) {

        hLib = LoadLibrary( "dbghelp.dll" );

        if ( hLib == NULL ) {
            return FALSE;
            }

        for ( i = 0; i < COUNT_IMAGEHLP_IMPORTS; i++ ) {

            Imagehlp.Imports[ i ] = GetProcAddress( hLib, ImagehlpImportNames[ i ] );

            if ( Imagehlp.Imports[ i ] == NULL ) {

                FreeLibrary( hLib );
                return FALSE;
                }
            }

        hLibImagehlp = hLib;
        }

    return TRUE;
    }


#ifdef BUILDING_PATCHAPI_DLL

VOID
UnloadImagehlp(
    VOID
    )
    {
    if ( hLibImagehlp ) {
        FreeLibrary( hLibImagehlp );
        hLibImagehlp = NULL;
        Imagehlp.Imports[ COUNT_IMAGEHLP_IMPORTS - 1 ] = NULL;
        DeleteCriticalSection( &ImagehlpCritSect );
        ImagehlpCritSectInitialized = FALSE;
        }
    }

#endif // BUILDING_PATCHAPI_DLL

#endif // ! PATCH_APPLY_CODE_ONLY


PIMAGE_NT_HEADERS
__fastcall
GetNtHeader(
    IN PVOID MappedFile,
    IN ULONG MappedFileSize
    )
    {
    PIMAGE_DOS_HEADER DosHeader;
    PIMAGE_NT_HEADERS RetHeader;
    PIMAGE_NT_HEADERS NtHeader;

    RetHeader = NULL;

    __try {

        if ( MappedFileSize >= 0x200 ) {

            DosHeader = (PIMAGE_DOS_HEADER) MappedFile;

            if ( DosHeader->e_magic == IMAGE_DOS_SIGNATURE ) {

                NtHeader = (PIMAGE_NT_HEADERS)((PUCHAR) MappedFile + DosHeader->e_lfanew );

                if (((PUCHAR) NtHeader + sizeof( IMAGE_NT_HEADERS )) <= ((PUCHAR) MappedFile + MappedFileSize )) {

                    if ( NtHeader->Signature == IMAGE_NT_SIGNATURE ) {

                        if ( NtHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC ) {

                            RetHeader = NtHeader;

                            }
                        }
                    }
                }
            }
        }
    __except( EXCEPTION_EXECUTE_HANDLER ) {
        }

    return RetHeader;
    }


BOOL
__fastcall
IsImageRvaInExecutableSection(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN ULONG             Rva
    )
    {
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG SectionCount;
    ULONG i;

    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {
        if (( Rva >= SectionHeader[ i ].VirtualAddress ) &&
            ( Rva <  SectionHeader[ i ].VirtualAddress + SectionHeader[ i ].Misc.VirtualSize )) {

            return (( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE ) ? TRUE : FALSE );
            }
        }

    return FALSE;
    }


ULONG
__fastcall
ImageRvaToFileOffset(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN ULONG             Rva
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
        return ((PUCHAR)MappedBase + MappedOffset );
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
    IN  PUCHAR            MappedBase,
    IN  ULONG             MappedSize
    )
    {
    PUCHAR Directory;
    ULONG  LocalSize;
    ULONG  Rva;

    Rva = ImageDirectoryRvaAndSize( NtHeader, DirectoryIndex, &LocalSize );

    if ( Rva ) {

        Directory = ImageRvaToMappedAddress( NtHeader, Rva, MappedBase, MappedSize );

        if (( Directory ) && (( Directory + LocalSize ) <= ( MappedBase + MappedSize ))) {

            if ( DirectorySize ) {
                *DirectorySize = LocalSize;
                }

            return Directory;
            }
        }

    return NULL;
    }


ULONG
__fastcall
FileOffsetToImageRva(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN ULONG             FileOffset
    )
    {
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG SectionCount;
    ULONG i;

    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {
        if (( FileOffset >= SectionHeader[ i ].PointerToRawData ) &&
            ( FileOffset <  SectionHeader[ i ].PointerToRawData + SectionHeader[ i ].SizeOfRawData )) {

            return ( SectionHeader[ i ].VirtualAddress + ( FileOffset - SectionHeader[ i ].PointerToRawData ));
            }
        }

    return 0;
    }


ULONG
MappedAddressToImageRva(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PVOID             MappedAddress,
    IN PVOID             MappedFile
    )
    {
    LONG FileOffset = (PUCHAR)MappedAddress - (PUCHAR)MappedFile;

    if ( FileOffset > 0 ) {
        return FileOffsetToImageRva( NtHeader, FileOffset );
        }

    return 0;
    }


BOOL
RebaseMappedImage(
    IN PUCHAR            MappedFile,
    IN ULONG             FileSize,
    IN PIMAGE_NT_HEADERS NtHeader,
    IN ULONG             NewBase
    )
    {
    PIMAGE_BASE_RELOCATION RelocBlock;
    LONG                   RelocAmount;
    LONG                   RelocDirRemaining;
    ULONG                  RelocDirSize;
    PUCHAR                 RelocBlockMa;
    PUCHAR                 RelocFixupMa;
    ULONG                  RelocCount;
    PUSHORT                RelocEntry;
    PUCHAR                 MappedFileEnd;
    BOOL                   Modified;

    //
    //  Carefully rebase the image, ignoring invalid info as much as possible
    //  without taking an access violation.  We don't want to use try/except
    //  here because this code needs to be workable without any imports from
    //  kernel32.dll.  This code is not intended to catch errors in invalid
    //  rebase info -- it is intended to silently do the best it can at
    //  rebasing the image in memory.  If the rebase info is valid, it will
    //  correctly rebase the image.  If the rebase info is not valid, it will
    //  attempt to avoid causing an access violation.
    //

    ASSERT( NtHeader->OptionalHeader.ImageBase != NewBase );
    ASSERT(( NewBase & 0x0000FFFF ) == 0 );
    ASSERT(( NewBase & 0xFFFF0000 ) != 0 );

    Modified      = FALSE;
    MappedFileEnd = MappedFile + FileSize;
    RelocAmount   = NewBase - NtHeader->OptionalHeader.ImageBase;

    RelocBlock = ImageDirectoryMappedAddress( NtHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC, &RelocDirSize, MappedFile, FileSize );

    if ( RelocBlock ) {

        NtHeader->OptionalHeader.ImageBase = NewBase;
        Modified = TRUE;

        RelocDirRemaining = (LONG)RelocDirSize;

        while ( RelocDirRemaining > 0 ) {

            if (( RelocBlock->SizeOfBlock <= (ULONG)RelocDirRemaining ) &&
                ( RelocBlock->SizeOfBlock > sizeof( IMAGE_BASE_RELOCATION ))) {

                //
                //  ImageRvaToMappedAddress returns NULL if the Rva is zero,
                //  but that is a valid base address of a reloc block.  Use
                //  ImageRvaToFileOffset instead.
                //

                RelocBlockMa = MappedFile + ImageRvaToFileOffset( NtHeader, RelocBlock->VirtualAddress );

                if ( RelocBlockMa ) {

                    RelocEntry = (PUSHORT)((ULONG)RelocBlock + sizeof( IMAGE_BASE_RELOCATION ));
                    RelocCount = ( RelocBlock->SizeOfBlock - sizeof( IMAGE_BASE_RELOCATION )) / sizeof( USHORT );

                    while ( RelocCount-- ) {

                        RelocFixupMa = RelocBlockMa + ( *RelocEntry & 0x0FFF );

                        if ( RelocFixupMa < MappedFileEnd ) {

                            switch ( *RelocEntry >> 12 ) {

                                case IMAGE_REL_BASED_HIGHLOW:

                                    *(UNALIGNED LONG *)RelocFixupMa += RelocAmount;
                                    break;

                                case IMAGE_REL_BASED_LOW:

                                    *(UNALIGNED USHORT *)RelocFixupMa = (USHORT)( *(UNALIGNED SHORT *)RelocFixupMa + RelocAmount );
                                    break;

                                case IMAGE_REL_BASED_HIGH:

                                    *(UNALIGNED USHORT *)RelocFixupMa = (USHORT)((( *(UNALIGNED USHORT *)RelocFixupMa << 16 ) + RelocAmount ) >> 16 );
                                    break;

                                case IMAGE_REL_BASED_HIGHADJ:

                                    ++RelocEntry;
                                    --RelocCount;

                                    *(UNALIGNED USHORT *)RelocFixupMa = (USHORT)((( *(UNALIGNED USHORT *)RelocFixupMa << 16 ) + *(UNALIGNED SHORT *)RelocEntry + RelocAmount + 0x8000 ) >> 16 );
                                    break;

                                //
                                //  Just skip and continue if we don't
                                //  recognize the reloc type.
                                //

                                }
                            }

                        ++RelocEntry;

                        }
                    }
                }
            else {
                break;  // invalid reloc structure
                }

            RelocDirRemaining -= RelocBlock->SizeOfBlock;
            RelocBlock = (PIMAGE_BASE_RELOCATION)((ULONG)RelocBlock + RelocBlock->SizeOfBlock );

            }
        }

    return Modified;
    }


BOOL
UnBindMappedImage(
    IN PUCHAR            MappedFile,
    IN ULONG             FileSize,
    IN PIMAGE_NT_HEADERS NtHeader
    )
    {
    PIMAGE_SECTION_HEADER    SectionHeader;
    ULONG                    SectionCount;
    DWORDLONG                SectionName;
    PIMAGE_IMPORT_DESCRIPTOR ImportDesc;
    PVOID                    BoundImportDir;
    ULONG                    BoundImportSize;
    PULONG                   OriginalIat;
    PULONG                   BoundIat;
    PUCHAR                   MappedFileEnd;
    BOOL                     Modified;
    ULONG                    i;

    Modified       = FALSE;
    MappedFileEnd  = MappedFile + FileSize;
    BoundImportDir = ImageDirectoryMappedAddress( NtHeader, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, &BoundImportSize, MappedFile, FileSize );

    if ( BoundImportDir ) {

        //
        //  Zero the bound import directory and pointers to bound
        //  import directory.
        //

        ZeroMemory( BoundImportDir, BoundImportSize );

        NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT ].VirtualAddress = 0;
        NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT ].Size           = 0;

        Modified = TRUE;
        }

    //
    //  Now walk imports and zero the TimeDate and
    //  ForwarderChain fields.
    //

    ImportDesc = ImageDirectoryMappedAddress( NtHeader, IMAGE_DIRECTORY_ENTRY_IMPORT, NULL, MappedFile, FileSize );

    if ( ImportDesc ) {

        while (((ULONG)ImportDesc <= ((ULONG)MappedFileEnd - sizeof( IMAGE_IMPORT_DESCRIPTOR ))) &&
               ( ImportDesc->Characteristics )) {

            if ( ImportDesc->TimeDateStamp ) {

                //
                //  This is a bound import.  Copy the unbound
                //  IAT over the bound IAT to restore.
                //

                ImportDesc->TimeDateStamp = 0;
                Modified = TRUE;

                OriginalIat = ImageRvaToMappedAddress( NtHeader, (ULONG)ImportDesc->OriginalFirstThunk, MappedFile, FileSize );
                BoundIat    = ImageRvaToMappedAddress( NtHeader, (ULONG)ImportDesc->FirstThunk,         MappedFile, FileSize );

                if (( OriginalIat ) && ( BoundIat )) {

                    while (((PUCHAR)( OriginalIat + 1 ) <= MappedFileEnd ) &&
                           ((PUCHAR)( BoundIat    + 1 ) <= MappedFileEnd ) &&
                           ( *OriginalIat )) {

                        *BoundIat++ = *OriginalIat++;
                        }
                    }
                }

            if ( ImportDesc->ForwarderChain ) {
                 ImportDesc->ForwarderChain = 0;
                 Modified = TRUE;
                 }

            ++ImportDesc;
            }
        }

    //
    //  The bind utility marks the .idata section as read-only so we want to
    //  change it back to read-write.
    //

    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {

        SectionName = *(UNALIGNED DWORDLONG*)( &SectionHeader[ i ].Name );
        SectionName |= 0x2020202020202020;  // fast lower case

        if ( SectionName == 0x202061746164692E ) {      // ".idata  "

            if ( ! ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_WRITE )) {

                SectionHeader[ i ].Characteristics |= ( IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE );
                Modified = TRUE;
                }

            break;
            }
        }

    return Modified;
    }


BOOL
SmashLockPrefixesInMappedImage(
    IN PUCHAR            MappedFile,
    IN ULONG             FileSize,
    IN PIMAGE_NT_HEADERS NtHeader,
    IN UCHAR             NewOpCode      // X86_OPCODE_NOP or X86_OPCODE_LOCK
    )
    {
    PIMAGE_LOAD_CONFIG_DIRECTORY LoadConfig;
    PUCHAR*                      LockPrefixEntry;
    PUCHAR                       LockPrefixInstruction;
    BOOL                         Modified;

    Modified   = FALSE;
    LoadConfig = ImageDirectoryMappedAddress( NtHeader, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG, NULL, MappedFile, FileSize );

    if ( LoadConfig ) {

        if ( LoadConfig->LockPrefixTable ) {

            //
            //  The LoadConfig->LockPrefixTable field and
            //  the lock prefix addresses are stored in the
            //  image as image VA, not RVA values.
            //

            LockPrefixEntry = ImageVaToMappedAddress( NtHeader, (ULONG)( LoadConfig->LockPrefixTable ), MappedFile, FileSize );

            if ( LockPrefixEntry ) {

                while ( *LockPrefixEntry ) {

                    LockPrefixInstruction = ImageVaToMappedAddress( NtHeader, (ULONG)( *LockPrefixEntry ), MappedFile, FileSize );

                    if ( LockPrefixInstruction ) {

                        if ( *LockPrefixInstruction != NewOpCode ) {

                            //
                            //  Lock prefix instruction is not what we want,
                            //  so modify it.
                            //

                            *LockPrefixInstruction = NewOpCode;
                            Modified = TRUE;
                            }
                        }

                    ++LockPrefixEntry;

                    }
                }
            }
        }

    return Modified;
    }


USHORT
ChkSum(
    IN USHORT  Initial,
    IN PUSHORT Buffer,
    IN ULONG   Bytes
    )
    {
    PUSHORT p = Buffer;
    ULONG WordsRemaining = Bytes / 2;
    ULONG WordsInChunk;
    ULONG SumChunk;
    ULONG SumTotal;

    SumTotal = Initial;

    while ( WordsRemaining ) {

        WordsInChunk = WordsRemaining;

        if ( WordsInChunk > 0x10000 ) {
             WordsInChunk = 0x10000;
             }

        WordsRemaining -= WordsInChunk;

        SumChunk = 0;

        do  {
            SumChunk += *p++;
            }
        while ( --WordsInChunk != 0 );

        SumTotal += ( SumChunk >> 16 ) + ( SumChunk & 0xFFFF );
        }

    if ( Bytes % 2 ) {

        SumTotal += *((PBYTE) p);
        }

    return (USHORT)(( SumTotal >> 16 ) + ( SumTotal & 0xFFFF ));
    }


BOOL
NormalizeCoffImage(
    IN OUT PIMAGE_NT_HEADERS NtHeader,
    IN OUT PUCHAR            MappedFile,
    IN     ULONG             FileSize,
    IN     ULONG             OptionFlags,
    IN     PVOID             OptionData,
    IN     ULONG             NewFileCoffBase,
    IN     ULONG             NewFileCoffTime
    )
    {
    BOOL Modified = FALSE;

    UNREFERENCED_PARAMETER( OptionData );

    if ( ! ( OptionFlags & PATCH_OPTION_NO_REBASE )) {

        if (( NewFileCoffTime != 0 ) && ( NtHeader->FileHeader.TimeDateStamp != NewFileCoffTime )) {
             NtHeader->FileHeader.TimeDateStamp = NewFileCoffTime;
             Modified = TRUE;
             }

        if (( NewFileCoffBase != 0 ) && ( NtHeader->OptionalHeader.ImageBase != NewFileCoffBase )) {
            Modified |= RebaseMappedImage( MappedFile, FileSize, NtHeader, NewFileCoffBase );
            }
        }

    if ( ! ( OptionFlags & PATCH_OPTION_NO_BINDFIX )) {
        Modified |= UnBindMappedImage( MappedFile, FileSize, NtHeader );
        }

    if ( ! ( OptionFlags & PATCH_OPTION_NO_LOCKFIX )) {
        Modified |= SmashLockPrefixesInMappedImage( MappedFile, FileSize, NtHeader, X86_OPCODE_LOCK );
        }

    //
    //  If the target file has zero checksum (PATCH_OPTION_NO_CHECKSUM),
    //  set the checksum in this image to zero.
    //
    //  Otherwise, if we either modified the image or if the image has
    //  a zero checksum, recompute the correct checksum.
    //

    if ( OptionFlags & PATCH_OPTION_NO_CHECKSUM ) {

        if ( NtHeader->OptionalHeader.CheckSum != 0 ) {
             NtHeader->OptionalHeader.CheckSum = 0;
             Modified = TRUE;
             }
        }

    else {

        if ( Modified ) {
             *(volatile ULONG *)( &NtHeader->OptionalHeader.CheckSum ) = 0;
             NtHeader->OptionalHeader.CheckSum = ChkSum( 0, (PVOID)MappedFile, FileSize ) + FileSize;
             }
        }

    return Modified;
    }


//
//  For some odd reason, the VC4 compiler says there is unreachable code
//  in the GetNewRvaFromRiftTable and FindRiftTableEntryForOldRva functions,
//  but I can't find it by inspection, and the VC5 and VC6 compilers don't
//  complain about it, so it's probably just a VC4 bug.  So, if the compiler
//  version is previous to VC5, disable this particular warning.
//

#if ( _MSC_VER < 1100 )
#pragma warning( disable: 4702 )    // unreachable code
#endif

ULONG
__fastcall
GetNewRvaFromRiftTable(
    IN PRIFT_TABLE RiftTable,
    IN ULONG OldRva
    )
    {
    PRIFT_ENTRY RiftEntryArray;
    ULONG NewRva;
    ULONG Index;
    ULONG MinIndexInclusive;
    ULONG MaxIndexExclusive;
    LONG  Displacement;
    BOOL  Found;

    //
    //  Rift table is in sorted order by OldRva, so do a binary search for
    //  matching or nearest preceding OldRva value.
    //

    RiftEntryArray    = RiftTable->RiftEntryArray;
    MaxIndexExclusive = RiftTable->RiftEntryCount;
    MinIndexInclusive = 0;
    Index             = 0;
    Found             = FALSE;

    while (( ! Found ) && ( MinIndexInclusive < MaxIndexExclusive )) {

        Index = ( MinIndexInclusive + MaxIndexExclusive ) / 2;    // won't overflow

        if ( RiftEntryArray[ Index ].OldFileRva < OldRva ) {
            MinIndexInclusive = Index + 1;
            }
        else if ( RiftEntryArray[ Index ].OldFileRva > OldRva ) {
            MaxIndexExclusive = Index;
            }
        else {
            Found = TRUE;
            break;
            }
        }

    if ( ! Found ) {

        //
        //  MinIndex is pointing to next highest entry, which could also be
        //  the zeroth entry if the search value was lower than anything in
        //  the table.
        //

        if ( MinIndexInclusive == 0 ) {
            return OldRva;  // zero displacement
            }

        Index = MinIndexInclusive - 1;
        }

    Displacement = (LONG)( RiftEntryArray[ Index ].NewFileRva - RiftEntryArray[ Index ].OldFileRva );

#ifndef PATCH_APPLY_CODE_ONLY

    //
    //  If we're updating the RiftUsageArray during compression, we want to
    //  mark the contributing entry as being used.
    //

    if ( RiftTable->RiftUsageArray != NULL ) {
         RiftTable->RiftUsageArray[ Index ] = 1;
         }

#endif

    NewRva = OldRva + Displacement;

    return NewRva;
    }


ULONG
__fastcall
FindRiftTableEntryForOldRva(
    IN PRIFT_TABLE RiftTable,
    IN ULONG OldRva
    )
    {
    PRIFT_ENTRY RiftEntryArray;
    ULONG MinIndexInclusive;
    ULONG MaxIndexExclusive;
    ULONG Index;
    BOOL  Found;

    //
    //  Rift table is in sorted order by OldRva, so do a binary search for
    //  matching or nearest preceding OldRva value.
    //

    RiftEntryArray    = RiftTable->RiftEntryArray;
    MaxIndexExclusive = RiftTable->RiftEntryCount;
    MinIndexInclusive = 0;
    Index             = 0;
    Found             = FALSE;

    while (( ! Found ) && ( MinIndexInclusive < MaxIndexExclusive )) {

        Index = ( MinIndexInclusive + MaxIndexExclusive ) / 2;    // won't overflow

        if ( RiftEntryArray[ Index ].OldFileRva < OldRva ) {
            MinIndexInclusive = Index + 1;
            }
        else if ( RiftEntryArray[ Index ].OldFileRva > OldRva ) {
            MaxIndexExclusive = Index;
            }
        else {
            Found = TRUE;
            break;
            }
        }

    if ( ! Found ) {

        //
        //  MinIndex is pointing to next highest entry, which could also be
        //  the zeroth entry if the search value was lower than anything in
        //  the table.
        //

        if ( MinIndexInclusive == 0 ) {
            return 0;
            }

        Index = MinIndexInclusive - 1;
        }

    return Index;
    }


#if ( _MSC_VER < 1100 )
#pragma warning( default: 4702 )    // unreachable code
#endif


VOID
__inline
ChangeOldRvaToNewRva(
    IN PRIFT_TABLE RiftTable,
    IN OUT PVOID AddressOfRvaToChange
    )
    {
    //
    //  Assuming all addresses of RVAs in a PE image are aligned.
    //

    PULONG RvaToChange = AddressOfRvaToChange;

    *RvaToChange = GetNewRvaFromRiftTable(
                       RiftTable,
                       *RvaToChange
                       );
    }


VOID
__inline
SwapRelocs(
    PRELOC_ARRAY_ENTRY One,
    PRELOC_ARRAY_ENTRY Two
    )
    {
    RELOC_ARRAY_ENTRY Tmp;

    Tmp  = *One;
    *One = *Two;
    *Two =  Tmp;
    }


VOID
__fastcall
RelocQsort(
    PRELOC_ARRAY_ENTRY LowerBound,
    PRELOC_ARRAY_ENTRY UpperBound
    )
    {
    PRELOC_ARRAY_ENTRY Lower = LowerBound;
    PRELOC_ARRAY_ENTRY Upper = UpperBound;
    PRELOC_ARRAY_ENTRY Pivot = Lower + (( Upper - Lower ) / 2 );
    ULONG PivotRva = Pivot->RelocRva;

    do  {

        while (( Lower <= Upper ) && ( Lower->RelocRva <= PivotRva )) {
            ++Lower;
            }

        while (( Upper >= Lower ) && ( Upper->RelocRva >= PivotRva )) {
            --Upper;
            }

        if ( Lower < Upper ) {
            SwapRelocs( Lower++, Upper-- );
            }
        }

    while ( Lower <= Upper );

    if ( Lower < Pivot ) {
        SwapRelocs( Lower, Pivot );
        Pivot = Lower;
        }
    else if ( Upper > Pivot ) {
        SwapRelocs( Upper, Pivot );
        Pivot = Upper;
        }

    if ( LowerBound < ( Pivot - 1 )) {
        RelocQsort( LowerBound, Pivot - 1 );
        }

    if (( Pivot + 1 ) < UpperBound ) {
        RelocQsort( Pivot + 1, UpperBound );
        }
    }


VOID
TransformOldFile_PE_Relocations(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PVOID             FileMappedImage,
    IN ULONG             FileSize,
    IN PRIFT_TABLE       RiftTable,
    IN PUCHAR            HintMap
    )
    {
    PUCHAR                 MappedFile;
    PUCHAR                 MappedFileEnd;
    ULONG                  ImageBaseVa;
    ULONG                  ImageLastVa;
    PUCHAR                 ImageFirstSectionMa;     // Mapped address
    ULONG                  ImageFirstSectionVa;     // Virtual address
    ULONG                  RelocDirOff;
    ULONG                  RelocDirSize;
    LONG                   RelocDirRemaining;
    PIMAGE_BASE_RELOCATION RelocBlock;
    ULONG                  RelocBlockBaseMa;        // Mapped address
    ULONG                  RelocBlockBaseVa;        // Virtual address
    ULONG                  RelocCount;
    PUSHORT                RelocEntry;
    PUSHORT                RelocFirst;
    UCHAR                  RelocType;
    ULONG                  RelocFixupMa;            // Mapped address
    ULONG                  RelocFixupVa;            // Virtual address
    ULONG                  RelocFixupRva;
    ULONG                  RelocTargetVa;           // Virtual address
    ULONG                  RelocTargetRva;
    ULONG                  NewRva;
    ULONG                  NewVa;
    PRELOC_ARRAY_ENTRY     RelocArray;
    ULONG                  RelocArrayCount;
    ULONG                  RelocArrayIndex;
    PIMAGE_SECTION_HEADER  SectionHeader;
    ULONG                  SectionCount;
    DWORDLONG              SectionName;
    PUCHAR                 p;
    ULONG                  i;

#ifdef TESTCODE

    ULONG CountRelocChanges = 0;

#endif // TESTCODE

    MappedFile    = FileMappedImage;
    MappedFileEnd = MappedFile + FileSize;
    ImageBaseVa   = NtHeader->OptionalHeader.ImageBase;
    RelocDirOff   = ImageDirectoryOffsetAndSize( NtHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC, &RelocDirSize );

    if (( RelocDirOff ) && (( RelocDirOff + RelocDirSize ) <= FileSize )) {

        memset( HintMap + RelocDirOff, 0x03, RelocDirSize );  // NOTE: may need to be OR'd if other bits are mutually exlusive

        //  allocate an array for the new reloc entries, approximating the needed size

        RelocArray = MyVirtualAlloc( sizeof( *RelocArray ) * ( RelocDirSize / sizeof(USHORT)));

        if ( RelocArray != NULL ) {

            RelocArrayCount = 0;

            RelocBlock = (PIMAGE_BASE_RELOCATION)( MappedFile + RelocDirOff );

            RelocDirRemaining = (LONG)RelocDirSize;

            while ( RelocDirRemaining > 0 ) {

                if (( RelocBlock->SizeOfBlock <= (ULONG)RelocDirRemaining ) &&
                    ( RelocBlock->SizeOfBlock > sizeof( IMAGE_BASE_RELOCATION ))) {

                    //
                    //  ImageRvaToMappedAddress returns NULL if the Rva is zero,
                    //  but that is a valid base address of a reloc block.  Use
                    //  ImageRvaToFileOffset instead.
                    //

                    RelocBlockBaseMa = (ULONG)MappedFile + ImageRvaToFileOffset( NtHeader, RelocBlock->VirtualAddress );

                    if ( RelocBlockBaseMa ) {

                        RelocBlockBaseVa = RelocBlock->VirtualAddress + ImageBaseVa;
                        RelocEntry       = (PUSHORT)((ULONG)RelocBlock + sizeof( IMAGE_BASE_RELOCATION ));
                        RelocCount       = ( RelocBlock->SizeOfBlock - sizeof( IMAGE_BASE_RELOCATION )) / sizeof( USHORT );

                        while ( RelocCount-- ) {

                            RelocType = (UCHAR)( *RelocEntry >> 12 );

                            if ( RelocType != IMAGE_REL_BASED_ABSOLUTE ) {

                                RelocFixupMa  = RelocBlockBaseMa + ( *RelocEntry & 0x0FFF );
                                RelocFixupVa  = RelocBlockBaseVa + ( *RelocEntry & 0x0FFF );
                                RelocFixupRva = RelocFixupVa - ImageBaseVa;

                                RelocArray[ RelocArrayCount ].RelocRva  = GetNewRvaFromRiftTable(
                                                                              RiftTable,
                                                                              RelocFixupRva
                                                                              );

                                RelocArray[ RelocArrayCount ].RelocType = RelocType;

                                switch ( RelocType ) {

                                    case IMAGE_REL_BASED_HIGHLOW:

                                        if ( RelocFixupMa < (ULONG)MappedFileEnd ) {

                                            *(UNALIGNED ULONG *)( HintMap + ( RelocFixupMa - (ULONG)MappedFile )) |= 0x01010101;    // don't transform again during E8 pass

                                            //
                                            //  Target is a 32-bit VA that we want to
                                            //  change to the corresponding VA in the
                                            //  new file.
                                            //

                                            RelocTargetVa  = *(UNALIGNED ULONG *) RelocFixupMa;
                                            RelocTargetRva = RelocTargetVa - ImageBaseVa;

                                            NewRva = GetNewRvaFromRiftTable(
                                                         RiftTable,
                                                         RelocTargetRva
                                                         );

                                            if ( NewRva != RelocTargetRva ) {

                                                NewVa = NewRva + ImageBaseVa;
                                                *(UNALIGNED ULONG *) RelocFixupMa = NewVa;
#ifdef TESTCODE
                                                ++CountRelocChanges;
#endif // TESTCODE
                                                }

                                            }

                                        break;

                                    case IMAGE_REL_BASED_LOW:
                                    case IMAGE_REL_BASED_HIGH:

                                        if ( RelocFixupMa < (ULONG)MappedFileEnd ) {
                                            *(UNALIGNED USHORT *)( HintMap + ( RelocFixupMa - (ULONG)MappedFile )) |= 0x0101; // don't transform again during E8 pass
                                            }

                                        break;

                                    case IMAGE_REL_BASED_HIGHADJ:

                                        if ( RelocFixupMa < (ULONG)MappedFileEnd ) {
                                            *(UNALIGNED USHORT *)( HintMap + ( RelocFixupMa - (ULONG)MappedFile )) |= 0x0101; // don't transform again during E8 pass
                                            }

                                        ++RelocEntry;
                                        --RelocCount;

                                        RelocArray[ RelocArrayCount ].HighAdjValue = *RelocEntry;

                                        break;
                                    }

                                ++RelocArrayCount;
                                }

                            ++RelocEntry;
                            }
                        }
                    }
                else {

#ifdef TESTCODE
                    printf( "\rInvalid .reloc directory\n" );
#endif TESTCODE

                    break;  // invalid reloc structure
                    }


                RelocDirRemaining -= RelocBlock->SizeOfBlock;
                RelocBlock = (PIMAGE_BASE_RELOCATION)((ULONG)RelocBlock + RelocBlock->SizeOfBlock );
                }

#ifdef TESTCODE

            printf( "\r%9d modified reloc targets\n", CountRelocChanges );

#endif TESTCODE

            //
            //  Now we want to reconstruct the .reloc table based on the new values
            //  hoping that it will more closely match the .reloc table in the new
            //  file.
            //
            //  First we want to sort our RelocArray by Rva.
            //

            if ( RelocArrayCount > 1 ) {
                RelocQsort( &RelocArray[ 0 ], &RelocArray[ RelocArrayCount - 1 ] );

#ifdef TESTCODE

                for ( i = 0; i < RelocArrayCount - 1; i++ ) {
                    if ( RelocArray[ i ].RelocRva > RelocArray[ i + 1 ].RelocRva ) {

                        printf( "\nReloc sort failed at index %d of %d\n", i, RelocArrayCount );

                        for ( i = 0; i < RelocArrayCount; i++ ) {
                            printf( "%08X\n", RelocArray[ i ].RelocRva );
                            }

                        exit( 1 );
                        break;
                        }
                    }

#endif // TESTCODE

                }

            RelocDirRemaining = (LONG)RelocDirSize;

            //
            //  Look for .reloc section to determine max size we can use for new
            //  .reloc data (may be bigger than old RelocDirSize).
            //

            SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
            SectionCount  = NtHeader->FileHeader.NumberOfSections;

            for ( i = 0; i < SectionCount; i++ ) {

                SectionName = *(UNALIGNED DWORDLONG*)( &SectionHeader[ i ].Name );
                SectionName |= 0x2020202020202020;  // fast lower case

                if ( SectionName == 0x2020636F6C65722E ) {      // ".reloc  "

                    if (( RelocDirOff >= SectionHeader[ i ].PointerToRawData ) &&
                        ( RelocDirOff <  SectionHeader[ i ].PointerToRawData + SectionHeader[ i ].SizeOfRawData )) {

                        RelocDirRemaining = ( SectionHeader[ i ].PointerToRawData + SectionHeader[ i ].SizeOfRawData ) - RelocDirOff;
                        }
                    }
                }

            RelocDirRemaining &= ~1;    // force to even value
            RelocBlock = (PIMAGE_BASE_RELOCATION)( MappedFile + RelocDirOff );
            RelocArrayIndex = 0;

            while (( RelocDirRemaining > sizeof( IMAGE_BASE_RELOCATION )) &&
                   ( RelocArrayIndex < RelocArrayCount )) {

                RelocBlock->VirtualAddress = ( RelocArray[ RelocArrayIndex ].RelocRva & 0xFFFFF000 );
                RelocFirst = RelocEntry    = (PUSHORT)((ULONG)RelocBlock + sizeof( IMAGE_BASE_RELOCATION ));
                RelocDirRemaining         -= sizeof( IMAGE_BASE_RELOCATION );

                while (( RelocDirRemaining > 0 ) &&
                       ( RelocArrayIndex < RelocArrayCount ) &&
                       (( RelocArray[ RelocArrayIndex ].RelocRva & 0xFFFFF000 ) == RelocBlock->VirtualAddress )) {

                    *RelocEntry++ = (USHORT)(( RelocArray[ RelocArrayIndex ].RelocType << 12 ) | ( RelocArray[ RelocArrayIndex ].RelocRva & 0x00000FFF ));
                    RelocDirRemaining -= sizeof( USHORT );

                    if ((( RelocArray[ RelocArrayIndex ].RelocType << 12 ) == IMAGE_REL_BASED_HIGHADJ ) &&
                        ( RelocDirRemaining > 0 )) {

                        *RelocEntry++ = RelocArray[ RelocArrayIndex ].HighAdjValue;
                        RelocDirRemaining -= sizeof( USHORT );
                        }

                    ++RelocArrayIndex;
                    }

                if (( RelocDirRemaining > 0 ) && ((ULONG)RelocEntry & 2 )) {
                    *RelocEntry++ = 0;
                    RelocDirRemaining -= sizeof( USHORT );
                    }

                RelocBlock->SizeOfBlock = ((ULONG)RelocEntry - (ULONG)RelocFirst ) + sizeof( IMAGE_BASE_RELOCATION );
                RelocBlock = (PIMAGE_BASE_RELOCATION)((ULONG)RelocBlock + RelocBlock->SizeOfBlock );
                }

            MyVirtualFree( RelocArray );
            }
        }

    else {

        //
        //  No relocation table exists for this binary.  We can still perform
        //  this transformation for x86 images by inferring some of the
        //  relocation targets in the mapped image by scanning the image for
        //  4-byte values that are virtual addresses that fall within the
        //  mapped image range.  We start with the address of the first
        //  section, assuming no relocs occur in the image header.
        //

        if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {

            ImageFirstSectionMa = MappedFile  + IMAGE_FIRST_SECTION( NtHeader )->PointerToRawData;
            ImageFirstSectionVa = ImageBaseVa + IMAGE_FIRST_SECTION( NtHeader )->VirtualAddress;
            ImageLastVa         = ImageBaseVa + NtHeader->OptionalHeader.SizeOfImage;

            for ( p = ImageFirstSectionMa; p < ( MappedFileEnd - 4 ); p++ ) {

                RelocTargetVa = *(UNALIGNED ULONG *) p;

                if (( RelocTargetVa >= ImageFirstSectionVa ) && ( RelocTargetVa < ImageLastVa )) {

                    //
                    //  This looks like a 32-bit VA that points within the image,
                    //  so we'll transform it to the corresponding new address.
                    //

                    *(UNALIGNED ULONG *)( HintMap + ((ULONG)p - (ULONG)MappedFile )) |= 0x01010101; // don't transform again during E8 pass

                    RelocTargetRva = RelocTargetVa - ImageBaseVa;

                    NewRva = GetNewRvaFromRiftTable( RiftTable, RelocTargetRva );

                    if ( NewRva != RelocTargetRva ) {
                        NewVa = NewRva + ImageBaseVa;
                        *(UNALIGNED ULONG *) p = NewVa;

#ifdef TESTCODE
                        ++CountRelocChanges;
#endif // TESTCODE

                        }

                    p += 3;
                    }
                }

#ifdef TESTCODE

            printf( "\r%9d modified inferred reloc targets (old algorithm)\n", CountRelocChanges );

#endif // TESTCODE

            }
        }
    }


VOID
TransformOldFile_PE_InferRelocsX86(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PVOID             FileMappedImage,
    IN ULONG             FileSize,
    IN PRIFT_TABLE       RiftTable,
    IN PUCHAR            HintMap
    )
{
    //
    //  No relocation table exists for this binary.  We can still perform
    //  this transformation for x86 images by inferring some of the
    //  relocation targets in the mapped image by scanning the image for
    //  4-byte values that appear to be virtual addresses that fall within
    //  the image size range.  We start with the address of the first
    //  section, assuming no relocs occur in the image header.
    //

    PBYTE MappedFile    = (PBYTE) FileMappedImage;
    ULONG ImageBaseVa   = NtHeader->OptionalHeader.ImageBase;
    ULONG ImageLastVa   = NtHeader->OptionalHeader.SizeOfImage + ImageBaseVa;
    ULONG FileOffset    = NtHeader->OptionalHeader.SizeOfHeaders;
    ULONG LastOffset    = FileSize - 4;
    BYTE  HighByte      = (BYTE)( ImageBaseVa >> 24 );

#ifdef TESTCODE
    ULONG CountRelocChanges = 0;
#endif TESTCODE

    while ( FileOffset < LastOffset )
    {
        if ( MappedFile[ FileOffset + 3 ] == HighByte )
        {
            ULONG RelocTargetVa = *(ULONG UNALIGNED*)( MappedFile + FileOffset );

            if (( RelocTargetVa > ImageBaseVa ) &&
                ( RelocTargetVa < ImageLastVa ))
            {
                ULONG HintValue = *(ULONG UNALIGNED*)( HintMap + FileOffset );

                if (( HintValue & 0x02020202 ) == 0 )
                {
                    *(ULONG UNALIGNED*)( HintMap + FileOffset ) = ( HintValue | 0x01010101 ); // don't transform again during E8 pass, and as a target this cannot be the first byte of an instruction

                    {
                        ULONG OldTargetRva = RelocTargetVa - ImageBaseVa;
                        ULONG NewTargetRva = GetNewRvaFromRiftTable( RiftTable, OldTargetRva );

                        if ( NewTargetRva != OldTargetRva )
                        {
                            *(ULONG UNALIGNED*)( MappedFile + FileOffset ) = ( ImageBaseVa + NewTargetRva );
#ifdef TESTCODE
                            ++CountRelocChanges;
#endif TESTCODE
                        }
                    }

                    FileOffset += 4;
                    continue;
                }
            }
        }

        FileOffset++;
    }

#ifdef TESTCODE
    printf( "\r%9d modified inferred reloc targets (new algorithm)\n", CountRelocChanges );
#endif TESTCODE

}


VOID
TransformOldFile_PE_Exports(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PVOID             FileMappedImage,
    IN ULONG             FileSize,
    IN PRIFT_TABLE       RiftTable,
    IN PUCHAR            HintMap
    )
    {
    PUCHAR                  MappedFile;
    PUCHAR                  MappedFileEnd;
    ULONG                   FileOffset;
    ULONG                   ExportDirOff;
    ULONG                   ExportDirSize;
    PIMAGE_EXPORT_DIRECTORY ExportBlock;
    ULONG                   EntryCount;
    PULONG                  Entry;

    MappedFile    = FileMappedImage;
    MappedFileEnd = MappedFile + FileSize;

    ExportDirOff = ImageDirectoryOffsetAndSize( NtHeader, IMAGE_DIRECTORY_ENTRY_EXPORT, &ExportDirSize );

    if (( ExportDirOff ) && (( ExportDirOff + ExportDirSize ) <= FileSize )) {

        memset( HintMap + ExportDirOff, 0x03, ExportDirSize );  // NOTE: may need to be OR'd if other mutually exclusive bits are used

        ExportBlock = (PIMAGE_EXPORT_DIRECTORY)( MappedFile + ExportDirOff );

        EntryCount = ExportBlock->NumberOfFunctions;
        FileOffset = ImageRvaToFileOffset( NtHeader, (ULONG) ExportBlock->AddressOfFunctions );

        if (( FileOffset + EntryCount * sizeof( ULONG )) <= FileSize ) {
            memset( HintMap + FileOffset, 0x03, EntryCount * sizeof( ULONG )); // NOTE: may need to be OR'd if other bits are used
            }

        Entry = (PULONG)( MappedFile + FileOffset );

        while (( EntryCount-- ) && ((PUCHAR)( Entry + 1 ) <= MappedFileEnd )) {
            ChangeOldRvaToNewRva( RiftTable, Entry++ );
            }

        EntryCount = ExportBlock->NumberOfNames;
        FileOffset = ImageRvaToFileOffset( NtHeader, (ULONG) ExportBlock->AddressOfNames );

        if (( FileOffset + EntryCount * sizeof( ULONG )) <= FileSize ) {
            memset( HintMap + FileOffset, 0x03, EntryCount * sizeof( ULONG )); // NOTE: may need to be OR'd if other bits are used
            }

        Entry = (PULONG)( MappedFile + FileOffset );

        while (( EntryCount-- ) && ((PUCHAR)( Entry + 1 ) <= MappedFileEnd )) {
            ChangeOldRvaToNewRva( RiftTable, Entry++ );
            }

        EntryCount = ExportBlock->NumberOfNames;
        FileOffset = ImageRvaToFileOffset( NtHeader, (ULONG) ExportBlock->AddressOfNameOrdinals );

        if (( FileOffset + EntryCount * sizeof( USHORT )) <= FileSize ) {
            memset( HintMap + FileOffset, 0x03, EntryCount * sizeof( USHORT )); // NOTE: may need to be OR'd if other bits are used
            }

        ChangeOldRvaToNewRva( RiftTable, &ExportBlock->Name );
        ChangeOldRvaToNewRva( RiftTable, &ExportBlock->AddressOfFunctions );
        ChangeOldRvaToNewRva( RiftTable, &ExportBlock->AddressOfNames );
        ChangeOldRvaToNewRva( RiftTable, &ExportBlock->AddressOfNameOrdinals );
        }

    return;
    }


VOID
TransformOldFile_PE_Imports(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PVOID             FileMappedImage,
    IN ULONG             FileSize,
    IN PRIFT_TABLE       RiftTable,
    IN PUCHAR            HintMap
    )
    {
    PUCHAR                   MappedFile;
    PUCHAR                   MappedFileEnd;
    ULONG                    FileOffset;
    ULONG                    ImportDirOff;
    ULONG                    ImportDirSize;
    ULONG                    ImportByNameDataOffset;
    PIMAGE_IMPORT_BY_NAME    ImportByNameData;
    PIMAGE_IMPORT_DESCRIPTOR ImportBlock;
    PIMAGE_THUNK_DATA        ThunkDataStart;
    PIMAGE_THUNK_DATA        ThunkData;

    MappedFile    = FileMappedImage;
    MappedFileEnd = MappedFile + FileSize;

    ImportDirOff = ImageDirectoryOffsetAndSize( NtHeader, IMAGE_DIRECTORY_ENTRY_IMPORT, &ImportDirSize );

    if (( ImportDirOff ) && (( ImportDirOff + ImportDirSize ) <= FileSize )) {

        memset( HintMap + ImportDirOff, 0x03, ImportDirSize );  // NOTE: may need to be OR'd if other mutually exclusive bits are used

        ImportBlock = (PIMAGE_IMPORT_DESCRIPTOR)( MappedFile + ImportDirOff );

        while (( (PUCHAR)( ImportBlock + 1 ) <= MappedFileEnd ) &&
               ( ImportBlock->OriginalFirstThunk != 0 )) {

            if ( ! ImportBlock->TimeDateStamp ) {

                FileOffset = ImageRvaToFileOffset( NtHeader, (ULONG) ImportBlock->OriginalFirstThunk );

                ThunkData = ThunkDataStart = (PIMAGE_THUNK_DATA)( MappedFile + FileOffset );

                while (( (PUCHAR)( ThunkData + 1 ) <= MappedFileEnd ) &&
                       ( ThunkData->u1.Ordinal != 0 )) {

                    if ( ! IMAGE_SNAP_BY_ORDINAL( ThunkData->u1.Ordinal )) {

                        ImportByNameDataOffset = ImageRvaToFileOffset( NtHeader, (ULONG) ThunkData->u1.AddressOfData );

                        ImportByNameData = (PIMAGE_IMPORT_BY_NAME)( MappedFile + ImportByNameDataOffset );

                        memset( HintMap + ImportByNameDataOffset, 0x03, strlen( (LPSTR)ImportByNameData->Name ) + 3 ); // NOTE: may need to be OR'd if other bits are used

                        ChangeOldRvaToNewRva( RiftTable, &ThunkData->u1.AddressOfData );
                        }

                    ThunkData++;
                    }

                memset( HintMap + FileOffset, 0x03, ((ULONG)ThunkData - (ULONG)ThunkDataStart )); // NOTE: may need to be OR'd if other bits are used

                FileOffset = ImageRvaToFileOffset( NtHeader, (ULONG) ImportBlock->FirstThunk );

                ThunkData = ThunkDataStart = (PIMAGE_THUNK_DATA)( MappedFile + FileOffset );

                while (( (PUCHAR)( ThunkData + 1 ) <= MappedFileEnd ) &&
                       ( ThunkData->u1.Ordinal != 0 )) {

                    if ( ! IMAGE_SNAP_BY_ORDINAL( ThunkData->u1.Ordinal )) {

                        ImportByNameDataOffset = ImageRvaToFileOffset( NtHeader, (ULONG) ThunkData->u1.AddressOfData );

                        ImportByNameData = (PIMAGE_IMPORT_BY_NAME)( MappedFile + ImportByNameDataOffset );

                        memset( HintMap + ImportByNameDataOffset, 0x03, strlen( (LPSTR)ImportByNameData->Name ) + 3 ); // NOTE: may need to be OR'd if other bits are used

                        ChangeOldRvaToNewRva( RiftTable, &ThunkData->u1.AddressOfData );
                        }

                    ThunkData++;
                    }

                memset( HintMap + FileOffset, 0x03, ((ULONG)ThunkData - (ULONG)ThunkDataStart )); // NOTE: may need to be OR'd if other bits are used
                }

            ChangeOldRvaToNewRva( RiftTable, &ImportBlock->Name );
            ChangeOldRvaToNewRva( RiftTable, &ImportBlock->OriginalFirstThunk );
            ChangeOldRvaToNewRva( RiftTable, &ImportBlock->FirstThunk );

            ImportBlock++;
            }
        }

    //
    //  BUGBUG:  Another other big thing that will prevent long matches through
    //           the IMAGE_IMPORT_BY_NAME entries is the Hint values which
    //           may change (from implib for dll being imported).  The Hint
    //           values are stored between each of the names.  Maybe a separate
    //           "hint rift table" to fix those, or zero out all the hints in
    //           both the old and new files, then after the new file is built,
    //           go fill-in the hint values.
    //
    //           Currently we have no infrastructure for pre-modification of
    //           the new file before compression and post-modification after
    //           decompression.
    //

    return;
    }


#ifdef DONTCOMPILE  // jmps2 and jmps3 are experimental


VOID
TransformOldFile_PE_RelativeJmps2(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            OldFileMapped,
    IN ULONG             OldFileSize,
    IN PUCHAR            NewFileMapped,         // OPTIONAL
    IN ULONG             NewFileSize,           // OPTIONAL
    IN PRIFT_TABLE       RiftTable,
    IN PUCHAR            HintMap
    )
    {
    PIMAGE_NT_HEADERS     NewNtHeader;
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG                 SectionCount;
    PUCHAR                SectionStart;
    PUCHAR                SectionExtent;
    PUCHAR                SearchExtent;
    ULONG                 SectionLength;
    ULONG                 SectionOffset;
    ULONG                 SectionBaseRva;
    ULONG                 SectionLastRva;
    ULONG                 ImageLastRva;
    LONG                  Displacement;
    LONG                  NewDisplacement;
    ULONG                 OffsetInSection;
    ULONG                 OriginRva;
    ULONG                 TargetRva;
    ULONG                 NewOriginRva;
    ULONG                 NewTargetRva;
    ULONG                 i;
    PUCHAR                p;

    //
    //  For each executable section, scan for opcodes that indicate
    //  relative call or branch instructions (different opcodes for
    //  different machine types).
    //

#ifdef TESTCODE

    ULONG CountRelativeBranchChanges = 0;
    ULONG CountRiftModifications = 0;
    ULONG CountRiftDeletions = 0;

    ULONG CountUnmatchedBranches = 0;

    ULONG CountUnmatchedE8 = 0;
    ULONG CountUnmatchedE9 = 0;
    ULONG CountUnmatched0F = 0;

    ULONG CountUnmatchedE8Targets = 0;
    ULONG CountUnmatchedE9Targets = 0;
    ULONG CountUnmatched0FTargets = 0;

    ULONG CountUnmatchedE8Followers = 0;
    ULONG CountUnmatchedE9Followers = 0;
    ULONG CountUnmatched0FFollowers = 0;

    ULONG CountUnmatchedE8Instructions = 0;
    ULONG CountUnmatchedE9Instructions = 0;
    ULONG CountUnmatched0FInstructions = 0;

    ULONG CountBranchInversions = 0;

#endif // TESTCODE

    NewNtHeader   = NewFileMapped ? GetNtHeader( NewFileMapped, NewFileSize ) : NULL;
    ImageLastRva  = NtHeader->OptionalHeader.SizeOfImage;
    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {

        if ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE ) {

            SectionBaseRva = SectionHeader[ i ].VirtualAddress;
            SectionLength  = MIN( SectionHeader[ i ].Misc.VirtualSize, SectionHeader[ i ].SizeOfRawData );
            SectionOffset  = SectionHeader[ i ].PointerToRawData;
            SectionStart   = OldFileMapped + SectionOffset;
            SectionLastRva = SectionBaseRva + SectionLength;

            if (( SectionOffset < OldFileSize ) &&
                (( SectionOffset + SectionLength ) <= OldFileSize )) {

                if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {

                    SearchExtent = SectionStart + SectionLength - 5;

                    for ( p = SectionStart; p < SearchExtent; p++ ) {

                        if (( *p == 0xE8 ) ||       // call relative
                            ( *p == 0xE9 ) ||       // jmp relative
                            (( *p == 0x0F ) &&      // jcc relative (0F 8x)
                             (( *( p + 1 ) & 0xF0 ) == 0x80 ) &&
                             ( ++p < SearchExtent ))) {

                            //
                            //  Relative displacement is stored as
                            //  32-bit signed value following these
                            //  opcodes.  The displacement is relative
                            //  to the NEXT instruction, which is at
                            //  (p + 5).
                            //

                            Displacement = *(UNALIGNED LONG*)( p + 1 );

                            //
                            //  We expect a lot of false positives here because
                            //  occurences of <E8>, <E9>, and <0F><8x> will
                            //  likely occur in other parts of the instruction
                            //  stream so now we validate that the TargetRva
                            //  falls within the image and within an executable
                            //  section.
                            //
                            //  Also, for jmp and jcc instructions, verify that
                            //  the displacement is larger than +/- 127 because
                            //  if it wasn't, the instruction should have been
                            //  encoded as an 8-bit near branch, not a 32-bit
                            //  branch.  This prevents us from falsely matching
                            //  data that looks like:
                            //
                            //      xxE9xxxx 00000000
                            //

                            if (( *p == 0xE8 ) ||
                                ( Displacement >  127 ) ||
                                ( Displacement < -128 )) {

                                OffsetInSection = ( p + 5 ) - SectionStart;
                                OriginRva       = SectionBaseRva + OffsetInSection;
                                TargetRva       = OriginRva + Displacement;

                                if ((( TargetRva >= SectionBaseRva ) &&
                                     ( TargetRva <  SectionLastRva )) ||
                                    (( TargetRva <  ImageLastRva ) &&
                                     ( IsImageRvaInExecutableSection( NtHeader, TargetRva )))) {

                                    //
                                    //  Looks like a valid TargetRva.
                                    //

#ifndef PATCH_APPLY_CODE_ONLY

                                    //
                                    //  If we're creating the patch, then we need
                                    //  to validate the corresponding branch in the
                                    //  new file (might want to modify a rift entry).
                                    //

                                    if ( NewFileMapped != NULL ) {     // we're compressing

                                        BOOL OriginNext  = FALSE;
                                        BOOL OriginFound = FALSE;
                                        BOOL TargetNext  = FALSE;
                                        BOOL TargetFound = FALSE;

                                        ULONG  RiftIndexOrigin = FindRiftTableEntryForOldRva( RiftTable, OriginRva );
                                        ULONG  NewOriginRva = OriginRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexOrigin ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexOrigin ].OldFileRva );
                                        PUCHAR NewOriginMa  = NewFileMapped + ImageRvaToFileOffset( NewNtHeader, NewOriginRva );
#ifdef TESTCODE
                                        ULONG NewOriginRva1 = NewOriginRva;
                                        ULONG NewOriginMo1  = NewOriginMa - NewFileMapped;
                                        ULONG NewOriginVa1  = NewOriginRva + NewNtHeader->OptionalHeader.ImageBase;

                                        ULONG NewOriginRva2 = 0;
                                        ULONG NewOriginMo2  = 0;
                                        ULONG NewOriginVa2  = 0;

                                        UCHAR OldInstruction = *p;
                                        UCHAR NewInstruction = *( NewOriginMa - 5 );
                                        BOOL  InstructionsMatch = ( *p == *( NewOriginMa - 5 ));
                                        BOOL  FollowersMatch    = ( *( p + 5 ) == *NewOriginMa );
#endif // TESTCODE

                                        if (( *p == *( NewOriginMa - 5 )) &&    // instructions match, and
                                            (( *p == 0xE9 ) ||                  //   jmp instruction, or
                                             ( *( p + 5 ) == *NewOriginMa ))) { //   followers match

                                            OriginFound = TRUE;
                                            }

                                        else {

                                            if (( RiftIndexOrigin + 1 ) < RiftTable->RiftEntryCount ) {

                                                NewOriginRva = OriginRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva );
                                                NewOriginMa  = NewFileMapped + ImageRvaToFileOffset( NewNtHeader, NewOriginRva );
#ifdef TESTCODE
                                                NewOriginRva2 = NewOriginRva;
                                                NewOriginMo2  = NewOriginMa - NewFileMapped;
                                                NewOriginVa2  = NewOriginRva + NewNtHeader->OptionalHeader.ImageBase;
#endif // TESTCODE

                                                if (( *p == *( NewOriginMa - 5 )) &&    // instructions match, and
                                                    (( *p == 0xE9 ) ||                  //   jmp instruction, or
                                                     ( *( p + 5 ) == *NewOriginMa ))) { //   followers match

                                                    OriginFound = TRUE;
                                                    OriginNext  = TRUE;
                                                    }
                                                }
                                            }

                                        if ( OriginFound ) {

                                            ULONG  RiftIndexTarget = FindRiftTableEntryForOldRva( RiftTable, TargetRva );
                                            ULONG  NewTargetRva = TargetRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexTarget ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexTarget ].OldFileRva );
                                            PUCHAR NewTargetMa  = NewFileMapped + ImageRvaToFileOffset( NewNtHeader, NewTargetRva );
                                            PUCHAR TargetMa = OldFileMapped + ImageRvaToFileOffset( NtHeader, TargetRva );

                                            if ( *NewTargetMa == *TargetMa ) {  // target instructions match
                                                TargetFound = TRUE;
                                                }

                                            else {

                                                if (( RiftIndexTarget + 1 ) < RiftTable->RiftEntryCount ) {

                                                    NewTargetRva = TargetRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva );
                                                    NewTargetMa  = NewFileMapped + ImageRvaToFileOffset( NewNtHeader, NewTargetRva );

                                                    if ( *NewTargetMa == *TargetMa ) {  // target instructions match
                                                        TargetFound = TRUE;
                                                        TargetNext  = TRUE;
                                                        }
                                                    }
                                                }

                                            if ( TargetFound ) {    // target and origin found

                                                if ( OriginNext ) {

                                                    //
                                                    //  Coast the rift entry at [RiftIndexOrigin+1]
                                                    //  backwards to the Rva of the instruction.
                                                    //

                                                    LONG Delta = (LONG)( RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva - OriginRva );

                                                    RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva -= Delta;
                                                    RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].NewFileRva -= Delta;
#ifdef TESTCODE
                                                    ++CountRiftModifications;
#endif // TESTCODE
                                                    ASSERT( RiftTable->RiftEntryArray[ RiftIndexOrigin ].OldFileRva <= RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva );

                                                    if ( RiftTable->RiftEntryArray[ RiftIndexOrigin ].OldFileRva == RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva ) {
                                                         RiftTable->RiftEntryArray[ RiftIndexOrigin ].NewFileRva =  RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].NewFileRva;
#ifdef TESTCODE
                                                         ++CountRiftDeletions;
#endif // TESTCODE
                                                         }
                                                    }

                                                if ( TargetNext ) {

                                                    //
                                                    //  Coast the rift entry at [RiftIndexTarget+1]
                                                    //  backwards to the Rva of the target.
                                                    //

                                                    LONG Delta = (LONG)( RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva - TargetRva );

                                                    RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva -= Delta;
                                                    RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].NewFileRva -= Delta;
#ifdef TESTCODE
                                                    ++CountRiftModifications;
#endif // TESTCODE
                                                    ASSERT( RiftTable->RiftEntryArray[ RiftIndexTarget ].OldFileRva <= RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva );

                                                    if ( RiftTable->RiftEntryArray[ RiftIndexTarget ].OldFileRva == RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva ) {
                                                         RiftTable->RiftEntryArray[ RiftIndexTarget ].NewFileRva =  RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].NewFileRva;
#ifdef TESTCODE
                                                         ++CountRiftDeletions;
#endif // TESTCODE
                                                         }
                                                    }
                                                }
                                            }
#ifdef TESTCODE
                                        if ( ! (( OriginFound ) && ( TargetFound ))) {

                                            ++CountUnmatchedBranches;

                                            switch ( *p ) {

                                                case 0xE8:

                                                    ++CountUnmatchedE8;

                                                    if ( OriginFound ) {
                                                        ++CountUnmatchedE8Targets;
                                                        }
                                                    else if ( InstructionsMatch ) {
                                                        ASSERT( ! FollowersMatch );
                                                        ++CountUnmatchedE8Followers;
                                                        }
                                                    else {
                                                        ++CountUnmatchedE8Instructions;

                                                        printf( "\rUnmatched E8 at old RVA %08X (VA %08X, FO %08X)\n"
                                                                  "    with either new RVA %08X (VA %08X, FO %08X)\n"
                                                                  "     or backcoasted RVA %08X (VA %08X, FO %08X)\n\n",
                                                                OriginRva - 5,
                                                                ( OriginRva - 5 ) + NtHeader->OptionalHeader.ImageBase,
                                                                p - OldFileMapped,
                                                                NewOriginRva1,
                                                                NewOriginVa1,
                                                                NewOriginMo1,
                                                                NewOriginRva2,
                                                                NewOriginVa2,
                                                                NewOriginMo2
                                                              );
                                                        }


                                                    break;

                                                case 0xE9:

                                                    ++CountUnmatchedE9;

                                                    if ( OriginFound ) {
                                                        ++CountUnmatchedE9Targets;
                                                        }
                                                    else {
                                                        ++CountUnmatchedE9Instructions;
                                                        }

                                                    break;

                                                default:

                                                    ++CountUnmatched0F;

                                                    if ( OriginFound ) {
                                                        ++CountUnmatched0FTargets;
                                                        }
                                                    else if ( InstructionsMatch ) {
                                                        ASSERT( ! FollowersMatch );
                                                        ++CountUnmatched0FFollowers;
                                                        }
                                                    else {
                                                        ++CountUnmatched0FInstructions;
                                                        }

                                                    if ( ! InstructionsMatch ) {
                                                        if (( OldInstruction & ~1 ) == ( NewInstruction & ~1 )) {
                                                            ++CountBranchInversions;
                                                            }
                                                        }

                                                    break;

                                                }
                                            }
#endif // TESTCODE
                                        }

#endif // PATCH_APPLY_CODE_ONLY

                                    NewTargetRva = GetNewRvaFromRiftTable( RiftTable, TargetRva );
                                    NewOriginRva = GetNewRvaFromRiftTable( RiftTable, OriginRva );

                                    NewDisplacement = NewTargetRva - NewOriginRva;

                                    if ( NewDisplacement != Displacement ) {
                                        *(UNALIGNED LONG*)( p + 1 ) = NewDisplacement;
#ifdef TESTCODE
                                        ++CountRelativeBranchChanges;
#endif // TESTCODE
                                        }

                                    p += 4;
                                    }
                                }
                            }
                        }
                    }

                else if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_ALPHA ) {

                    //
                    //  Need to implement the scan for Alpha platform
                    //  relative call/jmp/jcc opcodes.
                    //

                    }
                }
            }
        }

#ifdef TESTCODE

    printf( "\r%d modified relative branches\n", CountRelativeBranchChanges );
    printf( "%d rift back-coasting modifications due to branch inspection\n", CountRiftModifications );
    printf( "%d rift deletions due to branch inspection back-coasting\n", CountRiftDeletions );
    printf( "%d total unmatched relative branches, composed of:\n", CountUnmatchedBranches );

    printf( "\t%d unmatched E8 (call)\n", CountUnmatchedE8 );
    printf( "\t\t%d unmatched E8 (call) instructions\n", CountUnmatchedE8Instructions );
    printf( "\t\t%d unmatched E8 (call) followers\n", CountUnmatchedE8Followers );
    printf( "\t\t%d unmatched E8 (call) targets\n", CountUnmatchedE8Targets );

    printf( "\t%d unmatched E9 (jmp)\n",    CountUnmatchedE9 );
    printf( "\t\t%d unmatched E9 (jmp) instructions\n", CountUnmatchedE9Instructions );
    printf( "\t\t%d unmatched E9 (jmp) targets\n", CountUnmatchedE9Targets );

    printf( "\t%d unmatched 0F 8x (jcc)\n", CountUnmatched0F );
    printf( "\t\t%d unmatched 0F 8x (jcc) instructions\n", CountUnmatched0FInstructions );
    printf( "\t\t\t%d unmatched 0F 8x (jcc) instruction inversions\n", CountBranchInversions );
    printf( "\t\t%d unmatched 0F 8x (jcc) followers\n", CountUnmatched0FFollowers );
    printf( "\t\t%d unmatched 0F 8x (jcc) targets\n", CountUnmatched0FTargets );

#endif // TESTCODE

    }


#ifdef _M_IX86

__inline char * mymemchr( char *buf, int c, unsigned count ) {

    __asm {
            mov     edi, buf
            mov     ecx, count
            mov     eax, c
            repne   scasb
            lea     eax, [edi-1]
            jz      RETURNIT
            xor     eax, eax
RETURNIT:
        }
    }

#else // ! _M_IX86

#define mymemchr memchr

#endif // ! _M_IX86



VOID
TransformOldFile_PE_RelativeJmps3(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            OldFileMapped,
    IN ULONG             OldFileSize,
    IN PUCHAR            NewFileMapped,         // OPTIONAL
    IN ULONG             NewFileSize,           // OPTIONAL
    IN PRIFT_TABLE       RiftTable,
    IN PUCHAR            HintMap
    )
    {
    PIMAGE_NT_HEADERS     NewNtHeader;
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG                 SectionCount;
    PUCHAR                SectionStart;
    PUCHAR                SectionExtent;
    ULONG                 SearchExtent;
    ULONG                 SectionLength;
    ULONG                 SectionOffset;
    ULONG                 SectionBaseRva;
    ULONG                 SectionLastRva;
    ULONG                 ImageLastRva;
    LONG                  DisplacementValue;
    LONG                  NewDisplacement;
    LONG                  OffsetToRvaAdjust;
    ULONG                 FileOffset;
    ULONG                 TargetOffset;
    UCHAR                 Instruction;
    ULONG                 InstructionLength;
    ULONG                 DisplacementOrigin;
    ULONG                 DisplacementOffset;
    BOOL                  Skip;
    ULONG                 OffsetInSection;
    ULONG                 OriginRva;
    ULONG                 TargetRva;
    ULONG                 NewOriginRva;
    ULONG                 NewTargetRva;
    ULONG                 i;
    ULONG                 j;
    PUCHAR                p;

    //
    //  For each executable section, scan for opcodes that indicate
    //  relative call or branch instructions (different opcodes for
    //  different machine types).
    //

#ifdef TESTCODE

    ULONG CountRelativeBranchChanges = 0;
    ULONG CountRiftModifications = 0;
    ULONG CountRiftDeletions = 0;

    ULONG CountUnmatchedBranches = 0;

    ULONG CountUnmatchedE8 = 0;
    ULONG CountUnmatchedE9 = 0;
    ULONG CountUnmatched0F = 0;

    ULONG CountUnmatchedE8Targets = 0;
    ULONG CountUnmatchedE9Targets = 0;
    ULONG CountUnmatched0FTargets = 0;

    ULONG CountUnmatchedE8Followers = 0;
    ULONG CountUnmatchedE9Followers = 0;
    ULONG CountUnmatched0FFollowers = 0;

    ULONG CountUnmatchedE8Instructions = 0;
    ULONG CountUnmatchedE9Instructions = 0;
    ULONG CountUnmatched0FInstructions = 0;

    ULONG CountBranchInversions = 0;

#endif // TESTCODE

    NewNtHeader   = NewFileMapped ? GetNtHeader( NewFileMapped, NewFileSize ) : NULL;
    ImageLastRva  = NtHeader->OptionalHeader.SizeOfImage - 1;
    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {

        if ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE ) {

            SectionBaseRva = SectionHeader[ i ].VirtualAddress;
            SectionLength  = MIN( SectionHeader[ i ].Misc.VirtualSize, SectionHeader[ i ].SizeOfRawData );
            SectionOffset  = SectionHeader[ i ].PointerToRawData;
            SectionStart   = OldFileMapped + SectionOffset;
            SectionLastRva = SectionBaseRva + SectionLength;
            OffsetToRvaAdjust = SectionHeader[ i ].VirtualAddress - SectionHeader[ i ].PointerToRawData;

            if (( SectionOffset < OldFileSize ) &&
                (( SectionOffset + SectionLength ) <= OldFileSize )) {

                if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {

                    SearchExtent = SectionOffset + SectionLength - 6;

                    for ( FileOffset = SectionOffset; FileOffset < SearchExtent; FileOffset++ ) {

                        Instruction = OldFileMapped[ FileOffset ];

                        switch ( Instruction ) {

                            case 0xE8:

                                InstructionLength  = 6;     // E8 xx xx xx xx yy
                                DisplacementOrigin = FileOffset + 5;
                                DisplacementOffset = 1;
                                break;

                            case 0xE9:

                                InstructionLength  = 5;     // E9 xx xx xx xx
                                DisplacementOrigin = FileOffset + 5;
                                DisplacementOffset = 1;
                                break;

                            case 0x0F:

                                if (( OldFileMapped[ FileOffset + 1 ] & 0xF0 ) != 0x80 ) {
                                    continue;
                                    }

                                InstructionLength  = 7;     // 0F 8x yy yy yy yy zz
                                DisplacementOrigin = FileOffset + 6;
                                DisplacementOffset = 2;
                                break;

                            default:

                                continue;

                            }

                        Skip = FALSE;

                        for ( j = 0; j < InstructionLength; j++ ) {
                            if ( HintMap[ FileOffset + j ] & 0x01 ) {
                                Skip = TRUE;
                                break;
                                }
                            }

                        if ( Skip ) {
                            continue;
                            }

                        DisplacementValue = *(UNALIGNED LONG*)( OldFileMapped + FileOffset + DisplacementOffset );

                        if (( Instruction != 0xE8 ) &&
                            (( DisplacementValue > 127 ) || ( DisplacementValue < -128 ))) {

                            continue;
                            }

                        OriginRva = DisplacementOrigin + OffsetToRvaAdjust;
                        TargetRva = OriginRva + DisplacementValue;

                        if ( TargetRva > ImageLastRva ) {
                            continue;
                            }

                        TargetOffset = ImageRvaToFileOffset( NtHeader, TargetRva );

                        if ( HintMap[ TargetOffset ] & 0x01 ) {
                            continue;
                            }

#ifndef PATCH_APPLY_CODE_ONLY

                        //
                        //  If we're creating the patch, then we want
                        //  to validate the corresponding branch in the
                        //  new file (might want to modify a rift entry).
                        //

                        if ( NewFileMapped != NULL ) {     // we're compressing

                            BOOL OriginNext  = FALSE;
                            BOOL OriginFound = FALSE;
                            BOOL TargetNext  = FALSE;
                            BOOL TargetFound = FALSE;

                            ULONG  RiftIndexOrigin = FindRiftTableEntryForOldRva( RiftTable, OriginRva );
                            ULONG  NewOriginRva = OriginRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexOrigin ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexOrigin ].OldFileRva );
                            PUCHAR NewOriginMa  = NewFileMapped + ImageRvaToFileOffset( NewNtHeader, NewOriginRva );
#ifdef TESTCODE
                            ULONG NewOriginRva1 = NewOriginRva;
                            ULONG NewOriginMo1  = NewOriginMa - NewFileMapped;
                            ULONG NewOriginVa1  = NewOriginRva + NewNtHeader->OptionalHeader.ImageBase;

                            ULONG NewOriginRva2 = 0;
                            ULONG NewOriginMo2  = 0;
                            ULONG NewOriginVa2  = 0;

                            BOOL InstructionsMatch;
                            BOOL FollowersMatch;
                            BOOL BranchInversion;

                            BranchInversion = FALSE;

                            if ( Instruction == 0x0F ) {
                                InstructionsMatch = ( *( NewOriginMa - 6 ) == Instruction ) && ( *( NewOriginMa - 5 ) == OldFileMapped[ FileOffset + 1 ] );

                                if ( ! InstructionsMatch ) {
                                    BranchInversion = ( *( NewOriginMa - 6 ) == Instruction ) && (( *( NewOriginMa - 5 ) & ~1 ) == ( OldFileMapped[ FileOffset + 1 ] & ~1 ));
                                    }
                                }
                            else {
                                InstructionsMatch = ( *( NewOriginMa - 5 ) == Instruction );
                                }

                            FollowersMatch = ( *NewOriginMa == OldFileMapped[ DisplacementOrigin ] );

#endif // TESTCODE

                            if (( InstructionsMatch ) &&
                                (( FollowersMatch ) || ( Instruction == 0xE9 ))) {

                                OriginFound = TRUE;
                                }

                            else {

                                if (( RiftIndexOrigin + 1 ) < RiftTable->RiftEntryCount ) {

                                    NewOriginRva = OriginRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva );
                                    NewOriginMa  = NewFileMapped + ImageRvaToFileOffset( NewNtHeader, NewOriginRva );
#ifdef TESTCODE
                                    NewOriginRva2 = NewOriginRva;
                                    NewOriginMo2  = NewOriginMa - NewFileMapped;
                                    NewOriginVa2  = NewOriginRva + NewNtHeader->OptionalHeader.ImageBase;
#endif // TESTCODE

                                    if ( Instruction == 0x0F ) {
                                        InstructionsMatch = ( *( NewOriginMa - 6 ) == Instruction ) && ( *( NewOriginMa - 5 ) == OldFileMapped[ FileOffset + 1 ] );
                                        }
                                    else {
                                        InstructionsMatch = ( *( NewOriginMa - 5 ) == Instruction );
                                        }

                                    FollowersMatch = ( *NewOriginMa == OldFileMapped[ DisplacementOrigin ] );

                                    if (( InstructionsMatch ) &&
                                        (( FollowersMatch ) || ( Instruction == 0xE9 ))) {

                                        OriginFound = TRUE;
                                        OriginNext  = TRUE;
                                        }
                                    }
                                }

                            if ( OriginFound ) {

                                ULONG  RiftIndexTarget = FindRiftTableEntryForOldRva( RiftTable, TargetRva );
                                ULONG  NewTargetRva = TargetRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexTarget ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexTarget ].OldFileRva );
                                PUCHAR NewTargetMa  = NewFileMapped + ImageRvaToFileOffset( NewNtHeader, NewTargetRva );
                                PUCHAR TargetMa = OldFileMapped + ImageRvaToFileOffset( NtHeader, TargetRva );

                                if ( *NewTargetMa == *TargetMa ) {  // target instructions match
                                    TargetFound = TRUE;
                                    }

                                else {

                                    if (( RiftIndexTarget + 1 ) < RiftTable->RiftEntryCount ) {

                                        NewTargetRva = TargetRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva );
                                        NewTargetMa  = NewFileMapped + ImageRvaToFileOffset( NewNtHeader, NewTargetRva );

                                        if ( *NewTargetMa == *TargetMa ) {  // target instructions match
                                            TargetFound = TRUE;
                                            TargetNext  = TRUE;
                                            }
                                        }
                                    }

                                if ( TargetFound ) {    // target and origin found

                                    if ( OriginNext ) {

                                        //
                                        //  Coast the rift entry at [RiftIndexOrigin+1]
                                        //  backwards to the Rva of the instruction.
                                        //

                                        LONG Delta = (LONG)( RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva - OriginRva );

                                        RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva -= Delta;
                                        RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].NewFileRva -= Delta;
#ifdef TESTCODE
                                        ++CountRiftModifications;
#endif // TESTCODE
                                        ASSERT( RiftTable->RiftEntryArray[ RiftIndexOrigin ].OldFileRva <= RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva );

                                        if ( RiftTable->RiftEntryArray[ RiftIndexOrigin ].OldFileRva == RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].OldFileRva ) {
                                             RiftTable->RiftEntryArray[ RiftIndexOrigin ].NewFileRva =  RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].NewFileRva;
#ifdef TESTCODE
                                             ++CountRiftDeletions;
#endif // TESTCODE
                                             }
                                        }

                                    if ( TargetNext ) {

                                        //
                                        //  Coast the rift entry at [RiftIndexTarget+1]
                                        //  backwards to the Rva of the target.
                                        //

                                        LONG Delta = (LONG)( RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva - TargetRva );

                                        RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva -= Delta;
                                        RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].NewFileRva -= Delta;
#ifdef TESTCODE
                                        ++CountRiftModifications;
#endif // TESTCODE
                                        ASSERT( RiftTable->RiftEntryArray[ RiftIndexTarget ].OldFileRva <= RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva );

                                        if ( RiftTable->RiftEntryArray[ RiftIndexTarget ].OldFileRva == RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].OldFileRva ) {
                                             RiftTable->RiftEntryArray[ RiftIndexTarget ].NewFileRva =  RiftTable->RiftEntryArray[ RiftIndexTarget + 1 ].NewFileRva;
#ifdef TESTCODE
                                             ++CountRiftDeletions;
#endif // TESTCODE
                                             }
                                        }
                                    }
                                }
#ifdef TESTCODE
                            if ( ! (( OriginFound ) && ( TargetFound ))) {

                                ++CountUnmatchedBranches;

                                switch ( Instruction ) {

                                    case 0xE8:

                                        ++CountUnmatchedE8;

                                        if ( OriginFound ) {
                                            ++CountUnmatchedE8Targets;
                                            }
                                        else if ( InstructionsMatch ) {
                                            ASSERT( ! FollowersMatch );
                                            ++CountUnmatchedE8Followers;
                                            }
                                        else {
                                            ++CountUnmatchedE8Instructions;

                                            printf( "\rUnmatched E8 at old RVA %08X (VA %08X, FO %08X)\n"
                                                      "    with either new RVA %08X (VA %08X, FO %08X)\n"
                                                      "     or backcoasted RVA %08X (VA %08X, FO %08X)\n\n",
                                                    OriginRva - 5,
                                                    ( OriginRva - 5 ) + NtHeader->OptionalHeader.ImageBase,
                                                    FileOffset,
                                                    NewOriginRva1,
                                                    NewOriginVa1,
                                                    NewOriginMo1,
                                                    NewOriginRva2,
                                                    NewOriginVa2,
                                                    NewOriginMo2
                                                  );
                                            }


                                        break;

                                    case 0xE9:

                                        ++CountUnmatchedE9;

                                        if ( OriginFound ) {
                                            ++CountUnmatchedE9Targets;
                                            }
                                        else {
                                            ++CountUnmatchedE9Instructions;
                                            }

                                        break;

                                    default:

                                        ++CountUnmatched0F;

                                        if ( OriginFound ) {
                                            ++CountUnmatched0FTargets;
                                            }
                                        else if ( InstructionsMatch ) {
                                            ASSERT( ! FollowersMatch );
                                            ++CountUnmatched0FFollowers;
                                            }
                                        else {
                                            ++CountUnmatched0FInstructions;
                                            }

                                        if ( BranchInversion ) {
                                            ++CountBranchInversions;
                                            }

                                        break;

                                    }
                                }
#endif // TESTCODE
                            }

#endif // PATCH_APPLY_CODE_ONLY

                        NewTargetRva = GetNewRvaFromRiftTable( RiftTable, TargetRva );
                        NewOriginRva = GetNewRvaFromRiftTable( RiftTable, OriginRva );

                        NewDisplacement = NewTargetRva - NewOriginRva;

                        if ( NewDisplacement != DisplacementValue ) {
                            *(UNALIGNED LONG*)( OldFileMapped + FileOffset + DisplacementOffset ) = NewDisplacement;
#ifdef TESTCODE
                            ++CountRelativeBranchChanges;
#endif // TESTCODE
                            }

                        FileOffset = DisplacementOrigin - 1;
                        }
                    }

                else if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_ALPHA ) {

                    //
                    //  Need to implement the scan for Alpha platform
                    //  relative call/jmp/jcc opcodes.
                    //

                    }
                }
            }
        }

#ifdef TESTCODE

    printf( "\r%d modified relative branches\n", CountRelativeBranchChanges );
    printf( "%d rift back-coasting modifications due to branch inspection\n", CountRiftModifications );
    printf( "%d rift deletions due to branch inspection back-coasting\n", CountRiftDeletions );
    printf( "%d total unmatched relative branches, composed of:\n", CountUnmatchedBranches );

    printf( "\t%d unmatched E8 (call)\n", CountUnmatchedE8 );
    printf( "\t\t%d unmatched E8 (call) instructions\n", CountUnmatchedE8Instructions );
    printf( "\t\t%d unmatched E8 (call) followers\n", CountUnmatchedE8Followers );
    printf( "\t\t%d unmatched E8 (call) targets\n", CountUnmatchedE8Targets );

    printf( "\t%d unmatched E9 (jmp)\n",    CountUnmatchedE9 );
    printf( "\t\t%d unmatched E9 (jmp) instructions\n", CountUnmatchedE9Instructions );
    printf( "\t\t%d unmatched E9 (jmp) targets\n", CountUnmatchedE9Targets );

    printf( "\t%d unmatched 0F 8x (jcc)\n", CountUnmatched0F );
    printf( "\t\t%d unmatched 0F 8x (jcc) instructions\n", CountUnmatched0FInstructions );
    printf( "\t\t\t%d unmatched 0F 8x (jcc) instruction inversions\n", CountBranchInversions );
    printf( "\t\t%d unmatched 0F 8x (jcc) followers\n", CountUnmatched0FFollowers );
    printf( "\t\t%d unmatched 0F 8x (jcc) targets\n", CountUnmatched0FTargets );

#endif // TESTCODE

    }


#endif // DONTCOMPILE

VOID
TransformOldFile_PE_RelativeJmps(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PVOID             FileMappedImage,
    IN ULONG             FileSize,
    IN PRIFT_TABLE       RiftTable,
    IN PUCHAR            HintMap
    )
    {
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG                 SectionCount;
    PUCHAR                SectionStart;
    PUCHAR                SearchExtent;
    ULONG                 SectionLength;
    ULONG                 SectionOffset;
    ULONG                 SectionBaseRva;
    ULONG                 ImageLastRva;
    LONG                  Displacement;
    LONG                  NewDisplacement;
    ULONG                 OffsetInSection;
    ULONG                 OriginRva;
    ULONG                 TargetRva;
    ULONG                 NewOriginRva;
    ULONG                 NewTargetRva;
    ULONG                 TargetOffset;
    BOOL                  Skip;
    ULONG                 i;
    ULONG                 j;
    PUCHAR                p;

    //
    //  For each executable section, scan for opcodes that indicate
    //  relative branch instructions (different opcodes for different
    //  machine types).
    //

#ifdef TESTCODE

    ULONG CountRelativeBranchChanges = 0;

#endif // TESTCODE

    ImageLastRva  = NtHeader->OptionalHeader.SizeOfImage;
    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {

        if ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE ) {

            SectionBaseRva = SectionHeader[ i ].VirtualAddress;
            SectionLength  = MIN( SectionHeader[ i ].Misc.VirtualSize, SectionHeader[ i ].SizeOfRawData );
            SectionOffset  = SectionHeader[ i ].PointerToRawData;
            SectionStart   = (PUCHAR)FileMappedImage + SectionOffset;

            if (( SectionOffset < FileSize ) &&
                (( SectionOffset + SectionLength ) <= FileSize )) {

                if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {

                    SearchExtent = SectionStart + SectionLength - 5;

                    for ( p = SectionStart; p < SearchExtent; p++ ) {

                        if (( *p == 0xE9 ) ||       // jmp relative32 (E9)
                            (( *p == 0x0F ) &&      // jcc relative32 (0F 8x)
                             (( *( p + 1 ) & 0xF0 ) == 0x80 ) &&
                             ( ++p < SearchExtent ))) {

                            //
                            //  Validate that instruction and target are NOT
                            //  something we've already identified as something
                            //  else (reloc target, etc).
                            //

                            Skip = FALSE;

                            if (( *p & 0xF0 ) == 0x80 ) {
                                if ( HintMap[ SectionOffset + ( p - SectionStart ) - 1 ] & 0x01 ) {
                                    Skip = TRUE;
                                    }
                                }

                            if ( ! Skip ) {
                                for ( j = 0; j < 5; j++ ) {
                                    if ( HintMap[ SectionOffset + ( p - SectionStart ) + j ] & 0x01 ) {
                                        Skip = TRUE;
                                        break;
                                        }
                                    }
                                }

                            if ( ! Skip ) {

                                //
                                //  Relative displacement is stored as 32-bit
                                //  signed value following these opcodes.  The
                                //  displacement is relative to the NEXT
                                //  instruction, which is at (p + 5).
                                //
                                //  Also, for jmp and jcc instructions, verify that
                                //  the displacement is larger than +/- 127 because
                                //  if it wasn't, the instruction should have been
                                //  encoded as an 8-bit near branch, not a 32-bit
                                //  branch.  This prevents us from falsely matching
                                //  data that looks like:
                                //
                                //      xxE9xxxx 00000000
                                //

                                Displacement = *(UNALIGNED LONG*)( p + 1 );

                                if (( Displacement >  127 ) ||
                                    ( Displacement < -128 )) {

                                    OffsetInSection = ( p + 5 ) - SectionStart;
                                    OriginRva       = SectionBaseRva + OffsetInSection;
                                    TargetRva       = OriginRva + Displacement;

                                    //
                                    //  We expect a lot of false positives here because
                                    //  occurences of <E9>, and <0F><8x> will
                                    //  likely occur in other parts of the instruction
                                    //  stream so now we validate that the TargetRva
                                    //  falls within the image and within an executable
                                    //  section.
                                    //

                                    if ( TargetRva < ImageLastRva ) {

                                        TargetOffset = ImageRvaToFileOffset( NtHeader, TargetRva );

                                        if ( ! ( HintMap[ TargetOffset ] & 0x01 )) {

                                            //
                                            //  Looks like a valid TargetRva, so lookup its
                                            //  corresponding "new" RVA in the rift table.
                                            //

                                            NewTargetRva = GetNewRvaFromRiftTable( RiftTable, TargetRva );
                                            NewOriginRva = GetNewRvaFromRiftTable( RiftTable, OriginRva );

                                            NewDisplacement = NewTargetRva - NewOriginRva;

                                            if (( NewDisplacement >  127 ) ||
                                                ( NewDisplacement < -128 )) {

                                                if ( NewDisplacement != Displacement ) {

                                                    *(UNALIGNED LONG*)( p + 1 ) = NewDisplacement;
#ifdef TESTCODE
                                                    ++CountRelativeBranchChanges;
#endif // TESTCODE
                                                    }
                                                }

                                            else {

                                                //
                                                //  If new displacement is 8 bits, it would be
                                                //  encoded as an 8-bit relative instruction.
                                                //  For E9, instructions, that is EB.  For
                                                //  0F 8x instructions, that is 7x.  In both
                                                //  cases, we're shrinking the instruction stream.
                                                //  We'll leave the extra bytes alone.
                                                //

                                                if ( *p == 0xE9 ) {

                                                    *p = 0xEB;
                                                    *( p + 1 ) = (CHAR) NewDisplacement;
                                                    }

                                                else {

                                                    *( p - 1 ) = (UCHAR)(( *p & 0x0F ) | ( 0x70 ));
                                                    *p = (CHAR) NewDisplacement;
                                                    }

#ifdef TESTCODE
                                                ++CountRelativeBranchChanges;
#endif // TESTCODE

                                                }

                                            p += 4;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                else if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_ALPHA ) {

                    //
                    //  Need to implement the scan for Alpha platform
                    //  relative jmp/jcc opcodes.
                    //

                    }
                }
            }
        }

#ifdef TESTCODE

    printf( "\r%9d modified relative branches\n", CountRelativeBranchChanges );

#endif // TESTCODE

    }


VOID
TransformOldFile_PE_RelativeCalls(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PVOID             FileMappedImage,
    IN ULONG             FileSize,
    IN PRIFT_TABLE       RiftTable,
    IN PUCHAR            HintMap
    )
    {
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG                 SectionCount;
    PUCHAR                SectionStart;
    PUCHAR                SearchExtent;
    ULONG                 SectionLength;
    ULONG                 SectionOffset;
    ULONG                 SectionBaseRva;
    ULONG                 ImageLastRva;
    LONG                  Displacement;
    LONG                  NewDisplacement;
    ULONG                 OffsetInSection;
    ULONG                 OriginRva;
    ULONG                 TargetRva;
    ULONG                 NewOriginRva;
    ULONG                 NewTargetRva;
    ULONG                 TargetOffset;
    BOOL                  Skip;
    ULONG                 i;
    ULONG                 j;
    PUCHAR                p;

    //
    //  For each executable section, scan for opcodes that indicate
    //  relative call or branch instructions (different opcodes for
    //  different machine types).
    //

#ifdef TESTCODE

    ULONG CountRelativeCallChanges = 0;

#endif // TESTCODE

    ImageLastRva  = NtHeader->OptionalHeader.SizeOfImage;
    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {

        if ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE ) {

            SectionBaseRva = SectionHeader[ i ].VirtualAddress;
            SectionLength  = MIN( SectionHeader[ i ].Misc.VirtualSize, SectionHeader[ i ].SizeOfRawData );
            SectionOffset  = SectionHeader[ i ].PointerToRawData;
            SectionStart   = (PUCHAR)FileMappedImage + SectionOffset;

            if (( SectionOffset < FileSize ) &&
                (( SectionOffset + SectionLength ) <= FileSize )) {

                if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {

                    SearchExtent = SectionStart + SectionLength - 5;

                    for ( p = SectionStart; p < SearchExtent; p++ ) {

                        if ( *p == 0xE8 ) {         // call relative32

                            //
                            //  Validate that instruction and target are NOT
                            //  something we've already identified as something
                            //  else (reloc target, etc).
                            //

                            Skip = FALSE;

                            for ( j = 0; j < 5; j++ ) {
                                if ( HintMap[ SectionOffset + ( p - SectionStart ) + j ] & 0x01 ) {
                                    Skip = TRUE;
                                    break;
                                    }
                                }

                            if ( ! Skip ) {

                                //
                                //  Relative displacement is stored as 32-bit
                                //  signed value following these opcodes.  The
                                //  displacement is relative to the NEXT
                                //  instruction, which is at (p + 5).
                                //

                                Displacement    = *(UNALIGNED LONG*)( p + 1 );
                                OffsetInSection = ( p + 5 ) - SectionStart;
                                OriginRva       = SectionBaseRva + OffsetInSection;
                                TargetRva       = OriginRva + Displacement;

                                //
                                //  We expect a lot of false positives here because
                                //  occurences of <E8> will
                                //  likely occur in other parts of the instruction
                                //  stream so now we validate that the TargetRva
                                //  falls within the image and within an executable
                                //  section.
                                //

                                if ( TargetRva < ImageLastRva ) {

                                    TargetOffset = ImageRvaToFileOffset( NtHeader, TargetRva );

                                    if ( ! ( HintMap[ TargetOffset ] & 0x01 )) {

                                        //
                                        //  Looks like a valid TargetRva, so lookup its
                                        //  corresponding "new" RVA in the rift table.
                                        //

                                        NewTargetRva = GetNewRvaFromRiftTable( RiftTable, TargetRva );
                                        NewOriginRva = GetNewRvaFromRiftTable( RiftTable, OriginRva );

                                        NewDisplacement = NewTargetRva - NewOriginRva;

                                        if ( NewDisplacement != Displacement ) {

                                            *(UNALIGNED LONG*)( p + 1 ) = NewDisplacement;
#ifdef TESTCODE
                                            ++CountRelativeCallChanges;
#endif // TESTCODE
                                            }

                                        p += 4;
                                        }
                                    }
                                }
                            }
                        }
                    }

                else if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_ALPHA ) {

                    //
                    //  Need to implement the scan for Alpha platform
                    //  relative call/jmp/jcc opcodes.
                    //

                    }
                }
            }
        }

#ifdef TESTCODE

    printf( "\r%9d modified relative calls\n", CountRelativeCallChanges );

#endif // TESTCODE

    }


VOID
TransformOldFile_PE_MarkNonExe(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            OldFileMapped,
    IN ULONG             OldFileSize,
    IN PUCHAR            HintMap
    )
    {
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG SectionCount;
    ULONG Offset;
    ULONG Size;
    ULONG Rva;
    ULONG i;

    UNREFERENCED_PARAMETER( OldFileMapped );

    //
    //  Need to mark all non-exectuble bytes in hint map:
    //
    //      Image header
    //      All PE image directories (import, export, etc)
    //      All non-executable sections
    //      All relocation targets
    //         (a reloc target can be in the middle of an instruction, but
    //          never the first byte of an instruction)
    //
    //  NOTE: This is the first painting routine run, so it does memset
    //        instead of bitwise OR, knowing that only zeros already exist.
    //

    SectionHeader = IMAGE_FIRST_SECTION( NtHeader );
    SectionCount  = NtHeader->FileHeader.NumberOfSections;

    for ( i = 0; i < SectionCount; i++ ) {

        if ( ! ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE )) {

            Offset = SectionHeader[ i ].PointerToRawData;

            if ( Offset < OldFileSize ) {

                Size = SectionHeader[ i ].SizeOfRawData;

                if ( Size > ( OldFileSize - Offset )) {
                     Size = ( OldFileSize - Offset );
                     }

                memset( HintMap + Offset, 0x01, Size );
                }
            }
        }

    memset( HintMap, 0x03, NtHeader->OptionalHeader.SizeOfHeaders );

    for ( i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++ ) {

        Rva  = NtHeader->OptionalHeader.DataDirectory[ i ].VirtualAddress;
        Size = NtHeader->OptionalHeader.DataDirectory[ i ].Size;

        if (( Rva != 0 ) && ( Size != 0 )) {

            Offset = ImageRvaToFileOffset( NtHeader, Rva );

            if (( Offset != 0 ) && ( Offset < OldFileSize )) {

                if ( Size > ( OldFileSize - Offset )) {
                     Size = ( OldFileSize - Offset );
                     }

                memset( HintMap + Offset, 0x03, Size );
                }
            }
        }
    }


typedef struct _RES_RECURSION_CONTEXT {
    PRIFT_TABLE RiftTable;
    PUCHAR      ResBase;
    PUCHAR      ResEnd;
    ULONG       ResSize;
    ULONG       ResDone;
    ULONG       ResTime;
    ULONG       OldResRva;
    ULONG       NewResRva;
    BOOL        AnyErrors;
    } RES_RECURSION_CONTEXT, *PRES_RECURSION_CONTEXT;


VOID
TransformResourceRecursive(
    IN PRES_RECURSION_CONTEXT ResContext,
    IN IMAGE_RESOURCE_DIRECTORY UNALIGNED* ResDir
    )
    {
    IMAGE_RESOURCE_DIRECTORY_ENTRY UNALIGNED* ResEntry;
    IMAGE_RESOURCE_DATA_ENTRY      UNALIGNED* ResData;
    ULONG ResCount;
    ULONG NewOff;
    ULONG NewRva;

    if (((PUCHAR)( ResDir     ) < ResContext->ResBase ) ||
        ((PUCHAR)( ResDir + 1 ) > ResContext->ResEnd  )) {

        DEBUGCODE(
            printf(
                "\rTransformResource: ResDir at RVA %06X out of range %06X-%06X\n",
                ResContext->OldResRva + ((PUCHAR)ResDir - ResContext->ResBase ),
                ResContext->OldResRva,
                ResContext->OldResRva + ResContext->ResSize
                )
            );

        ResContext->AnyErrors = TRUE;
        return;
        }

    ResContext->ResDone += sizeof( *ResDir );

    if ( ResContext->ResDone > ResContext->ResSize ) {
        DEBUGCODE( printf( "\rTransformResource: ResDone %u exceeds ResSize %u\n", ResContext->ResDone, ResContext->ResSize ));
        ResContext->AnyErrors = TRUE;
        return;
        }

    if ( ResDir->TimeDateStamp != ResContext->ResTime ) {
         ResDir->TimeDateStamp  = ResContext->ResTime;
         }

    ResCount = ResDir->NumberOfNamedEntries + ResDir->NumberOfIdEntries;
    ResEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)( ResDir + 1 );

    while ( ResCount > 0 ) {

        if ((PUCHAR)( ResEntry + 1 ) > ResContext->ResEnd ) {

            DEBUGCODE(
                printf(
                    "\rTransformResource: ResEntry at RVA %06X out of range %06X-%06X\n",
                    ResContext->OldResRva + ((PUCHAR)ResEntry - ResContext->ResBase ),
                    ResContext->OldResRva,
                    ResContext->OldResRva + ResContext->ResSize
                    )
                );

            ResContext->AnyErrors = TRUE;
            return;
        }

        ResContext->ResDone += sizeof( *ResEntry );

        if ( ResContext->ResDone > ResContext->ResSize ) {
            DEBUGCODE( printf( "\rTransformResource: ResDone %u exceeds ResSize %u\n", ResContext->ResDone, ResContext->ResSize ));
            ResContext->AnyErrors = TRUE;
            return;
            }

        if ( ResEntry->DataIsDirectory ) {

            if (( ResEntry->OffsetToDirectory + sizeof( IMAGE_RESOURCE_DIRECTORY )) > ResContext->ResSize ) {

                DEBUGCODE(
                    printf(
                        "\rTransformResource: ResEntry->OffsetToDirectory %06X (+%u) exceeds ResDirSize %06X\n",
                        ResEntry->OffsetToDirectory,
                        sizeof( IMAGE_RESOURCE_DIRECTORY ),
                        ResContext->ResSize
                        )
                    );

                ResContext->AnyErrors = TRUE;

                //
                //  This is not fatal, but previously this would trigger bad resource for backward compat.
                //

                }

            else {

                TransformResourceRecursive(
                    ResContext,
                    (PIMAGE_RESOURCE_DIRECTORY)( ResContext->ResBase + ResEntry->OffsetToDirectory )
                    );

                if ( ResContext->ResDone > ResContext->ResSize ) {
                    ResContext->AnyErrors = TRUE;
                    return;
                    }
                }
            }

        else {

            ResData = (PIMAGE_RESOURCE_DATA_ENTRY)( ResContext->ResBase + ResEntry->OffsetToData );

            if (((PUCHAR)( ResData     ) < ResContext->ResBase ) ||
                ((PUCHAR)( ResData + 1 ) > ResContext->ResEnd  )) {

                DEBUGCODE(
                    printf(
                        "\rTransformResource: ResData at RVA %06X out of range %06X-%06X\n",
                        ResContext->OldResRva + ((PUCHAR)ResData - ResContext->ResBase ),
                        ResContext->OldResRva,
                        ResContext->OldResRva + ResContext->ResSize
                        )
                    );

                ResContext->AnyErrors = TRUE;

                //
                //  This is not fatal, and did not previously trigger bad resource directory for backwards compat.
                //
                }

            else {

                ResContext->ResDone += ResData->Size;

                if ( ResContext->ResDone > ResContext->ResSize ) {
                    DEBUGCODE( printf( "\rTransformResource: ResDone %u exceeds ResSize %u\n", ResContext->ResDone, ResContext->ResSize ));
                    ResContext->AnyErrors = TRUE;
                    return;
                    }

                //
                //  ResData->OffsetToData is an RVA, not offset from ResBase.
                //

                NewRva = GetNewRvaFromRiftTable( ResContext->RiftTable, ResData->OffsetToData );

                if ( ResData->OffsetToData != NewRva ) {
                     ResData->OffsetToData  = NewRva;
                     }
                }
            }

        NewRva = GetNewRvaFromRiftTable( ResContext->RiftTable, ResContext->OldResRva + ResEntry->OffsetToDirectory );
        NewOff = NewRva - ResContext->NewResRva;

        if ( ResEntry->OffsetToDirectory != NewOff ) {
             ResEntry->OffsetToDirectory  = NewOff;
             }

        if ( ResEntry->NameIsString ) {

            NewRva = GetNewRvaFromRiftTable( ResContext->RiftTable, ResContext->OldResRva + ResEntry->NameOffset );
            NewOff = NewRva - ResContext->NewResRva;

            if ( ResEntry->NameOffset != NewOff ) {
                 ResEntry->NameOffset  = NewOff;
                 }
            }

        ++ResEntry;
        --ResCount;
        }
    }


BOOL
TransformOldFile_PE_Resources(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            OldFileMapped,
    IN ULONG             OldFileSize,
    IN ULONG             NewFileResTime,
    IN PRIFT_TABLE       RiftTable
    )
    {
    RES_RECURSION_CONTEXT ResContext;

    ResContext.ResBase = ImageDirectoryMappedAddress(
                             NtHeader,
                             IMAGE_DIRECTORY_ENTRY_RESOURCE,
                             &ResContext.ResSize,
                             OldFileMapped,
                             OldFileSize
                             );

    if (( ResContext.ResBase != 0 ) && ( ResContext.ResSize >= sizeof( IMAGE_RESOURCE_DIRECTORY ))) {

        ResContext.ResEnd    = ResContext.ResBase + ResContext.ResSize;
        ResContext.ResDone   = 0;
        ResContext.OldResRva = ImageDirectoryRvaAndSize( NtHeader, IMAGE_DIRECTORY_ENTRY_RESOURCE, NULL );
        ResContext.NewResRva = GetNewRvaFromRiftTable( RiftTable, ResContext.OldResRva );
        ResContext.ResTime   = NewFileResTime;
        ResContext.RiftTable = RiftTable;
        ResContext.AnyErrors = FALSE;

        TransformResourceRecursive(
            &ResContext,
            (PIMAGE_RESOURCE_DIRECTORY) ResContext.ResBase
            );

        return ( ! ResContext.AnyErrors );
        }

    return TRUE;
    }


BOOL
TransformCoffImage(
    IN OUT PULONG            TransformOptions,
    IN OUT PIMAGE_NT_HEADERS NtHeader,
    IN OUT PUCHAR            OldFileMapped,
    IN     ULONG             OldFileSize,
    IN     ULONG             NewFileResTime,
    IN OUT PRIFT_TABLE       RiftTable,
    IN OUT PUCHAR OPTIONAL   HintMap,
    ...
    )
    {
    ULONG  OptionsX = *TransformOptions;
    PUCHAR InternalHintMap = NULL;
    BOOL   Success = FALSE;

#ifndef PATCH_APPLY_CODE_ONLY

    if ( OptionsX & PATCH_TRANSFORM_CREATION ) {

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RELOCS )) {

            __try {
                Success = PeValidateRelocations( NtHeader, OldFileMapped, OldFileSize );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                Success = FALSE;
                }

            if ( ! Success ) {
                OptionsX |= PATCH_TRANSFORM_NO_RELOCS;
                }
            }

#ifdef DONTCOMPILE

        //
        //  This code turns off NEW_IRELOC if a reloc directory exists,
        //  presumably to save extended options dword in patch header,
        //  and to make new patches compatibile with old applyers when
        //  otherwise don't need new applyer.
        //

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RELOCS )) {

            if ( OptionsX & PATCH_TRANSFORM_PE_IRELOC_2 ) {

                __try {

                    if ( NtHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 ) {

                        //
                        //  Don't need PATCH_TRANSFORM_PE_IRELOC_2 semantics
                        //  (costs extra DWORD for extended options in header)
                        //

                        OptionsX &= ~PATCH_TRANSFORM_PE_IRELOC_2;
                        }

                    else {

                        ULONG RelocDirSize = 0;
                        ULONG RelocDirOff  = ImageDirectoryOffsetAndSize( NtHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC, &RelocDirSize );

                        if (( RelocDirOff != 0 ) && (( RelocDirOff + RelocDirSize ) <= OldFileSize )) {

                            //
                            //  Don't need PATCH_TRANSFORM_PE_IRELOC_2 semantics
                            //  (costs extra DWORD for extended options in header)
                            //

                            OptionsX &= ~PATCH_TRANSFORM_PE_IRELOC_2;
                            }
                        }
                    }
                __except( EXCEPTION_EXECUTE_HANDLER ) {
                    }
                }
            }

#endif DONTCOMPILE

#ifdef DONTCOMPILE  // UNDONE: Implement PeValidateImports/Exports/Resources

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_IMPORTS )) {

            __try {
                Success = PeValidateImports( NtHeader, OldFileMapped, OldFileSize );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                Success = FALSE;
                }

            if ( ! Success ) {
                OptionsX |= PATCH_TRANSFORM_NO_IMPORTS;
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_EXPORTS )) {

            __try {
                Success = PeValidateExports( NtHeader, OldFileMapped, OldFileSize );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                Success = FALSE;
                }

            if ( ! Success ) {
                OptionsX |= PATCH_TRANSFORM_NO_EXPORTS;
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RESOURCE )) {

            __try {
                Success = PeValidateResources( NtHeader, OldFileMapped, OldFileSize );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                Success = FALSE;
                }

            if ( ! Success ) {
                OptionsX |= PATCH_TRANSFORM_NO_RESOURCE;
                }
            }

#endif DONTCOMPILE

        *TransformOptions = OptionsX;
        }


#endif // ! PATCH_APPLY_CODE_ONLY


#ifndef PATCH_APPLY_CODE_ONLY

    //
    //  Initialize (zero) the rift usage array
    //

    if ( RiftTable->RiftUsageArray != NULL ) {
        ZeroMemory( RiftTable->RiftUsageArray, RiftTable->RiftEntryAlloc * sizeof( RiftTable->RiftUsageArray[ 0 ] ));
        }

#endif // ! PATCH_APPLY_CODE_ONLY


    //
    //  Allocated parallel "hint" mapping the same size as the old
    //  file.  Each one of the 8 bits corresponding to each byte in
    //  the old file can be used to track information about that
    //  byte during the transformations.
    //

    if ( HintMap == NULL ) {

        InternalHintMap = MyVirtualAlloc( OldFileSize );

        if ( InternalHintMap == NULL ) {
            return FALSE;   // GLE already set
            }

        HintMap = InternalHintMap;
        }

    //
    //  Apply PE image transforms (each inside try/except)
    //

    Success = TRUE;     // innocent until proven guilty

    __try {
        TransformOldFile_PE_MarkNonExe( NtHeader, OldFileMapped, OldFileSize, HintMap );
        }
    __except( EXCEPTION_EXECUTE_HANDLER ) {
        Success = FALSE;
        }

    if ( OptionsX & ( PATCH_TRANSFORM_PE_IRELOC_2 | PATCH_TRANSFORM_PE_RESOURCE_2 )) {

        //
        //  Do PE structure walking transforms first, then do reloc, then E8/0F8x.
        //

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_IMPORTS )) {
            __try {
                TransformOldFile_PE_Imports( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_EXPORTS )) {
            __try {
                TransformOldFile_PE_Exports( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RESOURCE )) {
            __try {
                TransformOldFile_PE_Resources( NtHeader, OldFileMapped, OldFileSize, NewFileResTime, RiftTable );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RELOCS )) {
            __try {

                if ( ! ( OptionsX & PATCH_TRANSFORM_PE_IRELOC_2 )) {
                    TransformOldFile_PE_Relocations( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                    }
                else {

                    ULONG RelocDirSize = 0;
                    ULONG RelocDirOff  = ImageDirectoryOffsetAndSize( NtHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC, &RelocDirSize );

                    if (( RelocDirOff ) && (( RelocDirOff + RelocDirSize ) <= OldFileSize )) {
                        TransformOldFile_PE_Relocations( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                        }
                    else if ( NtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {
                        TransformOldFile_PE_InferRelocsX86( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                        }
                    }
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RELJMPS )) {
            __try {
                TransformOldFile_PE_RelativeJmps( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RELCALLS )) {
            __try {
                TransformOldFile_PE_RelativeCalls( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }
        }

    else {

        //
        //  Old compatible transform order.
        //

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RELOCS )) {
            __try {
                TransformOldFile_PE_Relocations( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_IMPORTS )) {
            __try {
                TransformOldFile_PE_Imports( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_EXPORTS )) {
            __try {
                TransformOldFile_PE_Exports( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RELJMPS )) {
            __try {
                TransformOldFile_PE_RelativeJmps( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RELCALLS )) {
            __try {
                TransformOldFile_PE_RelativeCalls( NtHeader, OldFileMapped, OldFileSize, RiftTable, HintMap );
                }
            __except( EXCEPTION_EXECUTE_HANDLER ) {
                }
            }

        if ( ! ( OptionsX & PATCH_TRANSFORM_NO_RESOURCE )) {

#ifndef PATCH_APPLY_CODE_ONLY

            if ( OptionsX & PATCH_TRANSFORM_CREATION ) {

                __try {
                    Success = TransformOldFile_PE_Resources( NtHeader, OldFileMapped, OldFileSize, NewFileResTime, RiftTable );
                    }
                __except( EXCEPTION_EXECUTE_HANDLER ) {
                    Success = FALSE;
                    }

                if ( ! Success ) {
                    SetLastError( PATCH_ERROR_RETRY_NO_RESOURCE );  // creation caller must remap and retry with PATCH_TRANSFORM_NO_RESOURCE
                    }
                }

            else

#endif
                {   // applying: no error return

                __try {
                    TransformOldFile_PE_Resources( NtHeader, OldFileMapped, OldFileSize, NewFileResTime, RiftTable );
                    }
                __except( EXCEPTION_EXECUTE_HANDLER ) {
                    }
                }
            }
        }

    if ( InternalHintMap != NULL ) {
        MyVirtualFree( InternalHintMap );
        }

    return Success;
    }


#ifndef PATCH_APPLY_CODE_ONLY

BOOL
PeValidateRelocations(
    IN PIMAGE_NT_HEADERS NtHeader,
    IN PUCHAR            FileMapped,
    IN ULONG             FileSize
    )
{
    __try
    {
        ULONG ImageSize    = NtHeader->OptionalHeader.SizeOfImage;
        ULONG RelocDirSize = 0;
        ULONG RelocDirOff  = ImageDirectoryOffsetAndSize( NtHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC, &RelocDirSize );
        ULONG RelocBaseRva;
        ULONG RelocCount;
        ULONG BlockSize;
        USHORT UNALIGNED* RelocEntry;
        IMAGE_BASE_RELOCATION UNALIGNED* RelocBlock;

        if ( RelocDirOff != 0 )
        {
            if (( RelocDirOff + RelocDirSize ) > FileSize )
            {
                DEBUGCODE( printf( "\rWarning: .reloc dir extent (%06X+%X) exceeds file size (%06X)\n", RelocDirOff, RelocDirSize, FileSize ));
                return FALSE;
            }

            RelocBlock = (PIMAGE_BASE_RELOCATION)( FileMapped + RelocDirOff );

            while ( RelocDirSize != 0 )
            {
                if ( RelocDirSize < sizeof( IMAGE_BASE_RELOCATION ))
                {
                    DEBUGCODE( printf( "\rWarning: .reloc residual size (%u) too small for reloc block\n", RelocDirSize ));
                    return FALSE;
                }

                if ( (ULONG_PTR)RelocBlock & 3 )
                {
                    DEBUGCODE( printf( "\rWarning: .reloc block misaligned\n" ));
                    // not fatal, we use explicit unaligned dereferences
                }

                RelocBaseRva = RelocBlock->VirtualAddress;
                BlockSize    = RelocBlock->SizeOfBlock;

                if ( BlockSize > RelocDirSize )
                {
                    DEBUGCODE( printf( "\rWarning: .reloc block size %u exceeds remaining .reloc dir size %u\n", BlockSize, RelocDirSize ));
                    return FALSE;
                }

                if ( BlockSize < sizeof( IMAGE_BASE_RELOCATION ))
                {
                    DEBUGCODE( printf( "\rWarning: .reloc block size %u smaller than minimum size %u\n", BlockSize, sizeof( IMAGE_BASE_RELOCATION )));
                    return FALSE;
                }

                if ( BlockSize & 1 )
                {
                    DEBUGCODE( printf( "\rWarning: .reloc block size %u not a multiple of 2 bytes\n", BlockSize ));
                    return FALSE;
                }

                if ( RelocBaseRva >= ImageSize )
                {
                    DEBUGCODE( printf( "\rWarning: .reloc block base RVA %06X beyond image size %06X\n", RelocBaseRva, ImageSize ));
                    // not fatal unless at least one entry uses it and then exceeds imagesize
                }

                if ( RelocBaseRva & 0x0FFF )
                {
                    DEBUGCODE( printf( "\rWarning: .reloc block base RVA %06X low 12 bits not zero\n", RelocBaseRva ));
                    // not fatal, just strange
                }

                RelocEntry = (PUSHORT)( RelocBlock + 1 );
                RelocCount = ( BlockSize - sizeof( IMAGE_BASE_RELOCATION )) / sizeof( USHORT );

                if ( RelocCount == 0 )
                {
                    DEBUGCODE( printf( "\rWarning: .reloc block size %u has zero entries.\n", BlockSize ));
                    // not fatal, just strange
                }

                while ( RelocCount-- )
                {
                    ULONG RelocOffset = *RelocEntry & 0x0FFF;
                    UCHAR RelocType   = (UCHAR)( *RelocEntry >> 12 );
                    ULONG RelocRva    = RelocBaseRva + RelocOffset;
                    ULONG RelocSize   = 0;

                    switch ( RelocType )
                    {
                        case IMAGE_REL_BASED_HIGH:
                        case IMAGE_REL_BASED_LOW:

                            RelocSize = 2;
                            break;

                        case IMAGE_REL_BASED_HIGHLOW:
                        case IMAGE_REL_BASED_MIPS_JMPADDR:

                            RelocSize = 4;
                            break;

                        case IMAGE_REL_BASED_HIGHADJ:

                            if ( RelocCount == 0 )
                            {
                                DEBUGCODE( printf( "\rWarning: .reloc entry HIGHADJ with no trailing reloc entry.\n" ));
                                return FALSE;
                            }

                            RelocCount--;
                            RelocEntry++;   // skip the highadj value

                            RelocSize = 2;
                            break;

                        case IMAGE_REL_BASED_IA64_IMM64:

                            RelocRva &= 0xFFFFFFF0;     // mask low 4 bits to bundle boundary
                            RelocSize = 16;             // reloc bits scattered over 16 byte bundle
                            break;

                        case IMAGE_REL_BASED_DIR64:

                            RelocSize = 8;
                            break;

                        case IMAGE_REL_BASED_ABSOLUTE:
                        case IMAGE_REL_BASED_SECTION:
                        case IMAGE_REL_BASED_REL32:

                            break;

                        default:

                            DEBUGCODE( printf( "\rWarning: .reloc entry type %u unrecognized, ignored\n", RelocType ));
                            break;
                    }

                    if ( RelocSize != 0 )
                    {
                        if (( RelocRva + RelocSize ) > ImageSize )
                        {
                            DEBUGCODE( printf( "\rWarning: .reloc entry target RVA %06X,%X exceeds image size %06X\n", RelocRva, RelocSize, ImageSize ));
                            return FALSE;
                        }
                    }

                    RelocEntry++;
                }

                RelocBlock = (PIMAGE_BASE_RELOCATION)((PBYTE)RelocBlock + BlockSize );
                RelocDirSize -= BlockSize;
            }
        }
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        DEBUGCODE( printf( "\rWarning: exception %08X while walking .reloc directory\n", GetExceptionCode() ));
        return FALSE;
    }

    return TRUE;
}



#endif // ! PATCH_APPLY_CODE_ONLY


#ifndef PATCH_APPLY_CODE_ONLY

#ifdef TESTCODE
    ULONG BreakOnRiftInsertionOld;
    ULONG BreakOnRiftInsertionNew;
#endif


BOOL
AddRiftEntryToTable(
    IN PRIFT_TABLE RiftTable,
    IN ULONG       OldRva,
    IN ULONG       NewRva
    )
    {
    ULONG RiftIndex;

    if (( OldRva != 0 ) && ( NewRva != 0 )) {

#ifdef TESTCODE

        if (( OldRva == BreakOnRiftInsertionOld ) ||
            ( NewRva == BreakOnRiftInsertionNew ))
        {
            DEBUGBREAK;
        }

#endif

        RiftIndex = RiftTable->RiftEntryCount;

        if (( RiftIndex + 1 ) < RiftTable->RiftEntryAlloc ) {
            RiftTable->RiftEntryCount = RiftIndex + 1;
            RiftTable->RiftEntryArray[ RiftIndex ].OldFileRva = OldRva;
            RiftTable->RiftEntryArray[ RiftIndex ].NewFileRva = NewRva;
            return TRUE;
            }

#ifdef TESTCODE

        else {
            printf( "\rInsufficient RiftEntryAlloc, discarding rift entry\n" );
            }
#endif

        }

    return FALSE;
    }


BOOL
InsertRiftEntryInSortedTable(
    IN PRIFT_TABLE RiftTable,
    IN ULONG       RiftIndex,
    IN ULONG       OldRva,
    IN ULONG       NewRva
    )
    {
    if (( OldRva != 0 ) && ( NewRva != 0 )) {

#ifdef TESTCODE

        if (( OldRva == BreakOnRiftInsertionOld ) ||
            ( NewRva == BreakOnRiftInsertionNew ))
        {
            DEBUGBREAK;
        }

#endif

        //
        //  First scoot to the correct index in case RiftIndex is off by a few.
        //

        while (( RiftIndex > 0 ) && ( RiftTable->RiftEntryArray[ RiftIndex ].OldFileRva > OldRva )) {
            --RiftIndex;
            }

        while (( RiftIndex < RiftTable->RiftEntryCount ) && ( RiftTable->RiftEntryArray[ RiftIndex ].OldFileRva < OldRva )) {
            ++RiftIndex;
            }

        if ( RiftIndex < RiftTable->RiftEntryCount ) {

            if ( RiftTable->RiftEntryArray[ RiftIndex ].OldFileRva == OldRva ) {

                //
                //  Don't insert duplicates.  If it matches an existing OldRva,
                //  just warn if the NewRva doesn't match the rift.
                //

#ifdef TESTCODE

                if ( RiftTable->RiftEntryArray[ RiftIndex ].NewFileRva != NewRva ) {

                    printf( "\rAttempt to insert different rift at same OldRva\n"
                            "    OldRva:%08X NewRva:%08X (discarded)\n"
                            "    OldRva:%08X NewRva:%08X (kept)\n\n",
                            OldRva,
                            NewRva,
                            RiftTable->RiftEntryArray[ RiftIndex ].OldFileRva,
                            RiftTable->RiftEntryArray[ RiftIndex ].NewFileRva
                          );

                    return FALSE;
                    }

#endif /* TESTCODE */

                return TRUE;
                }
            }

        //
        //  Verify we have enough allocation to insert a new entry.
        //

        if (( RiftTable->RiftEntryCount + 1 ) < RiftTable->RiftEntryAlloc ) {

            //
            //  Slide everything from RiftIndex to make room for new
            //  entry at RiftIndex.
            //

            LONG CountToMove = RiftTable->RiftEntryCount - RiftIndex;

            if ( CountToMove > 0 ) {

                MoveMemory(
                    &RiftTable->RiftEntryArray[ RiftIndex + 1 ],
                    &RiftTable->RiftEntryArray[ RiftIndex ],
                    CountToMove * sizeof( RiftTable->RiftEntryArray[ 0 ] )
                    );

#ifdef DONTCOMPILE  // we don't use the RiftUsageArray when we're inserting

                if ( RiftTable->RiftUsageArray ) {

                    MoveMemory(
                        &RiftTable->RiftUsageArray[ RiftIndex + 1 ],
                        &RiftTable->RiftUsageArray[ RiftIndex ],
                        CountToMove * sizeof( RiftTable->RiftUsageArray[ 0 ] )
                        );
                    }

#endif // DONTCOMPILE

                }

#ifdef DONTCOMPILE  // we don't use the RiftUsageArray when we're inserting

            RiftTable->RiftUsageArray[ RiftIndex ] = 0;

#endif // DONTCOMPILE

            RiftTable->RiftEntryArray[ RiftIndex ].OldFileRva = OldRva;
            RiftTable->RiftEntryArray[ RiftIndex ].NewFileRva = NewRva;
            RiftTable->RiftEntryCount++;

            return TRUE;
            }
        }

    return FALSE;
    }


VOID
__inline
SwapRifts(
    PRIFT_ENTRY One,
    PRIFT_ENTRY Two
    )
    {
    RIFT_ENTRY Tmp;

    Tmp  = *One;
    *One = *Two;
    *Two =  Tmp;
    }


VOID
__fastcall
RiftQsort(
    PRIFT_ENTRY LowerBound,
    PRIFT_ENTRY UpperBound
    )
    {
    PRIFT_ENTRY Lower = LowerBound;
    PRIFT_ENTRY Upper = UpperBound;
    PRIFT_ENTRY Pivot = Lower + (( Upper - Lower ) / 2 );
    ULONG PivotRva = Pivot->OldFileRva;

    do  {

        while (( Lower <= Upper ) && ( Lower->OldFileRva <= PivotRva )) {
            ++Lower;
            }

        while (( Upper >= Lower ) && ( Upper->OldFileRva >= PivotRva )) {
            --Upper;
            }

        if ( Lower < Upper ) {
            SwapRifts( Lower++, Upper-- );
            }
        }

    while ( Lower <= Upper );

    if ( Lower < Pivot ) {
        SwapRifts( Lower, Pivot );
        Pivot = Lower;
        }
    else if ( Upper > Pivot ) {
        SwapRifts( Upper, Pivot );
        Pivot = Upper;
        }

    if ( LowerBound < ( Pivot - 1 )) {
        RiftQsort( LowerBound, Pivot - 1 );
        }

    if (( Pivot + 1 ) < UpperBound ) {
        RiftQsort( Pivot + 1, UpperBound );
        }
    }


#ifdef DONTCOMPILE  // old code

VOID
RiftSortAndRemoveDuplicates(
    IN PUCHAR            OldFileMapped,
    IN ULONG             OldFileSize,
    IN PIMAGE_NT_HEADERS OldFileNtHeader,
    IN PUCHAR            NewFileMapped,
    IN ULONG             NewFileSize,
    IN PIMAGE_NT_HEADERS NewFileNtHeader,
    IN OUT PRIFT_TABLE   RiftTable
    )
    {
    ULONG i, n;

    if ( RiftTable->RiftEntryCount > 1 ) {

        n = RiftTable->RiftEntryCount - 1;

        RiftQsort( &RiftTable->RiftEntryArray[ 0 ], &RiftTable->RiftEntryArray[ n ] );

        for ( i = 0; i < n; i++ ) {

            while (( i < n ) &&
                   ( RiftTable->RiftEntryArray[ i     ].OldFileRva ==
                     RiftTable->RiftEntryArray[ i + 1 ].OldFileRva )) {

                if ( RiftTable->RiftEntryArray[ i     ].NewFileRva !=
                     RiftTable->RiftEntryArray[ i + 1 ].NewFileRva ) {

                    //
                    //  This is an ambiguous entry since the OldRva values
                    //  match but the NewRva values do not.  Inspect the
                    //  bytes in the old and new files and choose the one
                    //  that is correct.  If both are correct, or neither is
                    //  correct, choose the lower of the two NewRva values.
                    //

                    ULONG  ChosenNewRva;
                    PUCHAR OldFileRiftMa;
                    PUCHAR NewFileRiftMa1;
                    PUCHAR NewFileRiftMa2;

#ifdef TESTCODE
                    LPSTR Method = "lower";
#endif

                    ChosenNewRva = MIN( RiftTable->RiftEntryArray[ i     ].NewFileRva,
                                        RiftTable->RiftEntryArray[ i + 1 ].NewFileRva );

                    OldFileRiftMa = ImageRvaToMappedAddress(
                                        OldFileNtHeader,
                                        RiftTable->RiftEntryArray[ i ].OldFileRva,
                                        OldFileMapped,
                                        OldFileSize
                                        );

                    NewFileRiftMa1 = ImageRvaToMappedAddress(
                                         NewFileNtHeader,
                                         RiftTable->RiftEntryArray[ i ].NewFileRva,
                                         NewFileMapped,
                                         NewFileSize
                                         );

                    NewFileRiftMa2 = ImageRvaToMappedAddress(
                                         NewFileNtHeader,
                                         RiftTable->RiftEntryArray[ i + 1 ].NewFileRva,
                                         NewFileMapped,
                                         NewFileSize
                                         );

                    //
                    //  Use try/except to touch the mapped files.
                    //

                    __try {

                        if ( OldFileRiftMa != NULL ) {

                            if ((  NewFileRiftMa1 != NULL ) &&
                                ( *NewFileRiftMa1 == *OldFileRiftMa ) &&
                                ((  NewFileRiftMa2 == NULL ) ||
                                 ( *NewFileRiftMa2 != *OldFileRiftMa ))) {

                                ChosenNewRva = RiftTable->RiftEntryArray[ i ].NewFileRva;
#ifdef TESTCODE
                                Method = "match inspection";
#endif
                                }

                            else if ((  NewFileRiftMa2 != NULL ) &&
                                     ( *NewFileRiftMa2 == *OldFileRiftMa ) &&
                                     ((  NewFileRiftMa1 == NULL ) ||
                                      ( *NewFileRiftMa1 != *OldFileRiftMa ))) {

                                ChosenNewRva = RiftTable->RiftEntryArray[ i + 1 ].NewFileRva;
#ifdef TESTCODE
                                Method = "match inspection";
#endif
                                }
                            }
                        }

                    __except( EXCEPTION_EXECUTE_HANDLER ) {
                        }

#ifdef TESTCODE
                    printf(
                        "RiftInfo contains ambiguous entries:\n"
                        "    OldRva:%08X NewRva:%08X (discarded)\n"
                        "    OldRva:%08X NewRva:%08X (kept %s)\n\n",
                        RiftTable->RiftEntryArray[ i ].OldFileRva,
                        ( RiftTable->RiftEntryArray[ i ].NewFileRva == ChosenNewRva ) ?
                          RiftTable->RiftEntryArray[ i + 1 ].NewFileRva :
                          RiftTable->RiftEntryArray[ i ].NewFileRva,
                        RiftTable->RiftEntryArray[ i ].OldFileRva,
                        ChosenNewRva,
                        Method
                        );
#endif
                    RiftTable->RiftEntryArray[ i + 1 ].NewFileRva = ChosenNewRva;
                    }



                MoveMemory(
                    &RiftTable->RiftEntryArray[ i ],
                    &RiftTable->RiftEntryArray[ i + 1 ],
                    ( n - i ) * sizeof( RIFT_ENTRY )
                    );

                --n;

                }
            }

        RiftTable->RiftEntryCount = n + 1;
        }
    }

#endif


VOID
RiftSortAndRemoveDuplicates(
    IN PUCHAR            OldFileMapped,
    IN ULONG             OldFileSize,
    IN PIMAGE_NT_HEADERS OldFileNtHeader,
    IN PUCHAR            NewFileMapped,
    IN ULONG             NewFileSize,
    IN PIMAGE_NT_HEADERS NewFileNtHeader,
    IN OUT PRIFT_TABLE   RiftTable
    )
    {
    ULONG n = RiftTable->RiftEntryCount;
    ULONG i = 0;
    ULONG t = 0;

    if ( n <= 1 ) {
        return;
        }

    RiftQsort( &RiftTable->RiftEntryArray[ 0 ], &RiftTable->RiftEntryArray[ n - 1 ] );

    while ( i < n ) {

        if (( i < ( n - 1 )) &&
            ( RiftTable->RiftEntryArray[ i     ].OldFileRva ==
              RiftTable->RiftEntryArray[ i + 1 ].OldFileRva )) {

            if ( RiftTable->RiftEntryArray[ i     ].NewFileRva !=
                 RiftTable->RiftEntryArray[ i + 1 ].NewFileRva ) {

                //
                //  This is an ambiguous entry since the OldRva values
                //  match but the NewRva values do not.  Inspect the
                //  bytes in the old and new files and choose the one
                //  that is correct.  If both are correct, or neither is
                //  correct, choose the lower of the two NewRva values.
                //

                ULONG  ChosenNewRva;
                PUCHAR OldFileRiftMa;
                PUCHAR NewFileRiftMa1;
                PUCHAR NewFileRiftMa2;

#ifdef TESTCODE
                LPSTR Method = "lower";
#endif

                ChosenNewRva = MIN( RiftTable->RiftEntryArray[ i     ].NewFileRva,
                                    RiftTable->RiftEntryArray[ i + 1 ].NewFileRva );

                OldFileRiftMa = ImageRvaToMappedAddress(
                                    OldFileNtHeader,
                                    RiftTable->RiftEntryArray[ i ].OldFileRva,
                                    OldFileMapped,
                                    OldFileSize
                                    );

                NewFileRiftMa1 = ImageRvaToMappedAddress(
                                     NewFileNtHeader,
                                     RiftTable->RiftEntryArray[ i ].NewFileRva,
                                     NewFileMapped,
                                     NewFileSize
                                     );

                NewFileRiftMa2 = ImageRvaToMappedAddress(
                                     NewFileNtHeader,
                                     RiftTable->RiftEntryArray[ i + 1 ].NewFileRva,
                                     NewFileMapped,
                                     NewFileSize
                                     );

                //
                //  Use try/except to touch the mapped files.
                //

                __try {

                    if ( OldFileRiftMa != NULL ) {

                        if ((  NewFileRiftMa1 != NULL ) &&
                            ( *NewFileRiftMa1 == *OldFileRiftMa ) &&
                            ((  NewFileRiftMa2 == NULL ) ||
                             ( *NewFileRiftMa2 != *OldFileRiftMa ))) {

                            ChosenNewRva = RiftTable->RiftEntryArray[ i ].NewFileRva;
#ifdef TESTCODE
                            Method = "match inspection";
#endif
                            }

                        else if ((  NewFileRiftMa2 != NULL ) &&
                                 ( *NewFileRiftMa2 == *OldFileRiftMa ) &&
                                 ((  NewFileRiftMa1 == NULL ) ||
                                  ( *NewFileRiftMa1 != *OldFileRiftMa ))) {

                            ChosenNewRva = RiftTable->RiftEntryArray[ i + 1 ].NewFileRva;
#ifdef TESTCODE
                            Method = "match inspection";
#endif
                            }
                        }
                    }

                __except( EXCEPTION_EXECUTE_HANDLER ) {
                    }

#ifdef TESTCODE
                printf(
                    "RiftInfo contains ambiguous entries:\n"
                    "    OldRva:%08X NewRva:%08X (discarded)\n"
                    "    OldRva:%08X NewRva:%08X (kept %s)\n\n",
                    RiftTable->RiftEntryArray[ i ].OldFileRva,
                    ( RiftTable->RiftEntryArray[ i ].NewFileRva == ChosenNewRva ) ?
                      RiftTable->RiftEntryArray[ i + 1 ].NewFileRva :
                      RiftTable->RiftEntryArray[ i ].NewFileRva,
                    RiftTable->RiftEntryArray[ i ].OldFileRva,
                    ChosenNewRva,
                    Method
                    );
#endif
                RiftTable->RiftEntryArray[ i + 1 ].NewFileRva = ChosenNewRva;
                }

            }

        else {

            if ( i != t ) {
                RiftTable->RiftEntryArray[ t ] = RiftTable->RiftEntryArray[ i ];
                }

            ++t;
            }

        ++i;
        }

    RiftTable->RiftEntryCount = t;
    }



BOOL
IsMatchingResourceString(
    IN PIMAGE_RESOURCE_DIR_STRING_U OldString,
    IN PIMAGE_RESOURCE_DIR_STRING_U NewString
    )
    {
    ULONG Length = OldString->Length;
    ULONG i;

    if ( OldString->Length != NewString->Length ) {
        return FALSE;
        }

    for ( i = 0; i < Length; i++ ) {
        if ( OldString->NameString[ i ] != NewString->NameString[ i ] ) {
            return FALSE;
            }
        }

    return TRUE;
    }


VOID
GetResourceRiftInfoRecursive(
    IN PIMAGE_RESOURCE_DIRECTORY OldResDir,
    IN PUCHAR OldResBase,
    IN PUCHAR OldResEnd,
    IN ULONG  OldResRva,
    IN PIMAGE_RESOURCE_DIRECTORY NewResDir,
    IN PUCHAR NewResBase,
    IN PUCHAR NewResEnd,
    IN ULONG  NewResRva,
    IN PRIFT_TABLE RiftTable
    )
    {
    PIMAGE_RESOURCE_DIRECTORY_ENTRY OldResEntry;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY NewResEntry;
    ULONG OldResEntryCount;
    ULONG NewResEntryCount;
    PIMAGE_RESOURCE_DATA_ENTRY OldResData;
    PIMAGE_RESOURCE_DATA_ENTRY NewResData;

    if ((( (PUCHAR) OldResDir + sizeof( IMAGE_RESOURCE_DIRECTORY )) < OldResEnd ) &&
        (( (PUCHAR) NewResDir + sizeof( IMAGE_RESOURCE_DIRECTORY )) < NewResEnd )) {

        OldResEntryCount = OldResDir->NumberOfNamedEntries + OldResDir->NumberOfIdEntries;
        OldResEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY) ( OldResDir + 1 );

        while (( OldResEntryCount > 0 ) && ((PUCHAR)OldResEntry < OldResEnd )) {

            NewResEntryCount = NewResDir->NumberOfNamedEntries + NewResDir->NumberOfIdEntries;
            NewResEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)( NewResDir + 1 );

            while ( NewResEntryCount > 0 ) {

                if ( (PUCHAR) NewResEntry > NewResEnd ) {
                    NewResEntryCount = 0;
                    break;
                    }

                if ( !OldResEntry->NameIsString && !NewResEntry->NameIsString &&
                     ( OldResEntry->Id == NewResEntry->Id )) {
                    break;
                    }
                else if (( OldResEntry->NameIsString && NewResEntry->NameIsString ) &&
                        IsMatchingResourceString(
                            (PIMAGE_RESOURCE_DIR_STRING_U)( OldResBase + OldResEntry->NameOffset ),
                            (PIMAGE_RESOURCE_DIR_STRING_U)( NewResBase + NewResEntry->NameOffset ))) {
                    break;
                    }

                ++NewResEntry;
                --NewResEntryCount;
                }

            if ( NewResEntryCount > 0 ) {

                if ( OldResEntry->NameIsString ) {

                    AddRiftEntryToTable(
                        RiftTable,
                        OldResRva + OldResEntry->NameOffset,
                        NewResRva + NewResEntry->NameOffset
                        );
                    }

                AddRiftEntryToTable(
                    RiftTable,
                    OldResRva + OldResEntry->OffsetToDirectory,
                    NewResRva + NewResEntry->OffsetToDirectory
                    );

                if ( OldResEntry->DataIsDirectory ) {

                    GetResourceRiftInfoRecursive(
                        (PIMAGE_RESOURCE_DIRECTORY)( OldResBase + OldResEntry->OffsetToDirectory ),
                        OldResBase,
                        OldResEnd,
                        OldResRva,
                        (PIMAGE_RESOURCE_DIRECTORY)( NewResBase + NewResEntry->OffsetToDirectory ),
                        NewResBase,
                        NewResEnd,
                        NewResRva,
                        RiftTable
                        );
                    }
                else {

                    OldResData = (PIMAGE_RESOURCE_DATA_ENTRY)( OldResBase + OldResEntry->OffsetToData );
                    NewResData = (PIMAGE_RESOURCE_DATA_ENTRY)( NewResBase + NewResEntry->OffsetToData );

                    AddRiftEntryToTable(
                        RiftTable,
                        OldResData->OffsetToData,
                        NewResData->OffsetToData
                        );
                    }
                }

            ++OldResEntry;
            --OldResEntryCount;
            }
        }
    }


BOOL
GetImageNonSymbolRiftInfo(
    IN PUCHAR            OldFileMapped,
    IN ULONG             OldFileSize,
    IN PIMAGE_NT_HEADERS OldFileNtHeader,
    IN PUCHAR            NewFileMapped,
    IN ULONG             NewFileSize,
    IN PIMAGE_NT_HEADERS NewFileNtHeader,
    IN HANDLE            SubAllocator,
    IN PRIFT_TABLE       RiftTable
    )
    {

    //
    //  Create rifts for sections by section names.
    //

    {
    PIMAGE_SECTION_HEADER OldSectionHeader;
    PIMAGE_SECTION_HEADER NewSectionHeader;
    ULONG                 OldSectionCount;
    ULONG                 NewSectionCount;
    ULONG                 i, j;

    OldSectionHeader = IMAGE_FIRST_SECTION( OldFileNtHeader );
    OldSectionCount  = OldFileNtHeader->FileHeader.NumberOfSections;

    NewSectionHeader = IMAGE_FIRST_SECTION( NewFileNtHeader );
    NewSectionCount  = NewFileNtHeader->FileHeader.NumberOfSections;

    ASSERT( sizeof( OldSectionHeader->Name ) == sizeof( DWORDLONG ));

    for ( i = 0; i < OldSectionCount; i++ ) {

        for ( j = 0; j < NewSectionCount; j++ ) {

            if ( *(UNALIGNED DWORDLONG *)OldSectionHeader[ i ].Name ==
                 *(UNALIGNED DWORDLONG *)NewSectionHeader[ j ].Name ) {

                //
                //  Add a rift entry for this section name match.
                //
                //  Note that we create rift values here that are minus
                //  one from the actual section boundary because if a
                //  symbol exists at the start of the section, its rift
                //  entry would conflict with this section entry.
                //

                AddRiftEntryToTable(
                    RiftTable,
                    OldSectionHeader[ i ].VirtualAddress - 1,
                    NewSectionHeader[ i ].VirtualAddress - 1
                    );

                break;
                }
            }
        }
    }


    //
    //  Create rifts for image directories.
    //

    {
    ULONG i, n;

    n = MIN( OldFileNtHeader->OptionalHeader.NumberOfRvaAndSizes,
             NewFileNtHeader->OptionalHeader.NumberOfRvaAndSizes );

    for ( i = 0; i < n; i++ ) {

        if (( OldFileNtHeader->OptionalHeader.DataDirectory[ i ].VirtualAddress ) &&
            ( OldFileNtHeader->OptionalHeader.DataDirectory[ i ].Size           ) &&
            ( NewFileNtHeader->OptionalHeader.DataDirectory[ i ].VirtualAddress ) &&
            ( NewFileNtHeader->OptionalHeader.DataDirectory[ i ].Size           )) {

            AddRiftEntryToTable(
                RiftTable,
                OldFileNtHeader->OptionalHeader.DataDirectory[ i ].VirtualAddress,
                NewFileNtHeader->OptionalHeader.DataDirectory[ i ].VirtualAddress
                );
            }
        }
    }

    //
    //  Create rifts for image export directory
    //

    {
    PIMAGE_EXPORT_DIRECTORY OldExportDir;
    PIMAGE_EXPORT_DIRECTORY NewExportDir;
    ULONG                   OldExportIndex;
    ULONG                   NewExportIndex;
    ULONG                   OldExportNameCount;
    ULONG                   NewExportNameCount;
    ULONG                   OldExportFunctionCount;
    ULONG                   NewExportFunctionCount;
    PULONG                  OldExportFunctionArray;
    PULONG                  NewExportFunctionArray;
    PUSHORT                 OldExportNameToOrdinal;
    PUSHORT                 NewExportNameToOrdinal;
    PULONG                  OldExportNameArray;
    PULONG                  NewExportNameArray;
    LPSTR                   OldExportName;
    LPSTR                   NewExportName;
    ULONG                   OldOrdinal;
    ULONG                   NewOrdinal;
    LONG                    OrdinalBaseNewToOld;
    PBYTE                   NewExportOrdinalNameExists;
    PSYMBOL_NODE            NewExportSymbolNode;
    SYMBOL_TREE             NewExportNameTree;

    OldExportDir = ImageDirectoryMappedAddress(
                       OldFileNtHeader,
                       IMAGE_DIRECTORY_ENTRY_EXPORT,
                       NULL,
                       OldFileMapped,
                       OldFileSize
                       );

    NewExportDir = ImageDirectoryMappedAddress(
                       NewFileNtHeader,
                       IMAGE_DIRECTORY_ENTRY_EXPORT,
                       NULL,
                       NewFileMapped,
                       NewFileSize
                       );

    if (( OldExportDir ) && ( NewExportDir )) {

        AddRiftEntryToTable( RiftTable, (ULONG)( OldExportDir->Name ),                  (ULONG)( NewExportDir->Name ));
        AddRiftEntryToTable( RiftTable, (ULONG)( OldExportDir->AddressOfFunctions ),    (ULONG)( NewExportDir->AddressOfFunctions ));
        AddRiftEntryToTable( RiftTable, (ULONG)( OldExportDir->AddressOfNames ),        (ULONG)( NewExportDir->AddressOfNames ));
        AddRiftEntryToTable( RiftTable, (ULONG)( OldExportDir->AddressOfNameOrdinals ), (ULONG)( NewExportDir->AddressOfNameOrdinals ));

        //
        //  Now build a tree of new export names, then walk old export names
        //  looking for matches in tree of new export names.
        //

        SymRBInitTree(
            &NewExportNameTree,
            SubAllocator
            );

        //
        //  First insert new export names.
        //

        NewExportNameCount     = NewExportDir->NumberOfNames;
        NewExportFunctionCount = NewExportDir->NumberOfFunctions;
        NewExportFunctionArray = ImageRvaToMappedAddress( NewFileNtHeader, (ULONG) NewExportDir->AddressOfFunctions,    NewFileMapped, NewFileSize );
        NewExportNameToOrdinal = ImageRvaToMappedAddress( NewFileNtHeader, (ULONG) NewExportDir->AddressOfNameOrdinals, NewFileMapped, NewFileSize );
        NewExportNameArray     = ImageRvaToMappedAddress( NewFileNtHeader, (ULONG) NewExportDir->AddressOfNames,        NewFileMapped, NewFileSize );

        if ( NewExportNameArray ) {

            for ( NewExportIndex = 0; NewExportIndex < NewExportNameCount; NewExportIndex++ ) {

                if ( NewExportNameArray[ NewExportIndex ] ) {

                    NewExportName = ImageRvaToMappedAddress( NewFileNtHeader, NewExportNameArray[ NewExportIndex ], NewFileMapped, NewFileSize );

                    if ( NewExportName ) {

                        SymRBInsert( &NewExportNameTree, NewExportName, NewExportIndex );

                        }
                    }
                }
            }

        //
        //  Walk old export names and match them up.
        //

        OldExportNameCount     = OldExportDir->NumberOfNames;
        OldExportFunctionCount = OldExportDir->NumberOfFunctions;
        OldExportFunctionArray = ImageRvaToMappedAddress( OldFileNtHeader, (ULONG) OldExportDir->AddressOfFunctions,    OldFileMapped, OldFileSize );
        OldExportNameToOrdinal = ImageRvaToMappedAddress( OldFileNtHeader, (ULONG) OldExportDir->AddressOfNameOrdinals, OldFileMapped, OldFileSize );
        OldExportNameArray     = ImageRvaToMappedAddress( OldFileNtHeader, (ULONG) OldExportDir->AddressOfNames,        OldFileMapped, OldFileSize );

        if ( OldExportNameArray ) {

            for ( OldExportIndex = 0; OldExportIndex < OldExportNameCount; OldExportIndex++ ) {

                if ( OldExportNameArray[ OldExportIndex ] ) {

                    OldExportName = ImageRvaToMappedAddress( OldFileNtHeader, OldExportNameArray[ OldExportIndex ], OldFileMapped, OldFileSize );

                    if ( OldExportName ) {

                        NewExportSymbolNode = SymRBFind( &NewExportNameTree, OldExportName );

                        if ( NewExportSymbolNode ) {

                            //
                            //  Found a name match.  The Rva field in the
                            //  symbol node contains the index into the
                            //  NewExportNameArray.
                            //
                            //  This match gives us two rift entries: one
                            //  for the locations of the names themselves,
                            //  and another for the locations of the
                            //  functions corresponding to those names.
                            //

                            NewExportIndex = NewExportSymbolNode->Rva;

                            AddRiftEntryToTable(
                                RiftTable,
                                OldExportNameArray[ OldExportIndex ],
                                NewExportNameArray[ NewExportIndex ]
                                );

                            if ( OldExportNameToOrdinal && NewExportNameToOrdinal ) {

                                OldOrdinal = OldExportNameToOrdinal[ OldExportIndex ];
                                NewOrdinal = NewExportNameToOrdinal[ NewExportIndex ];

                                if (( OldOrdinal < OldExportFunctionCount ) &&
                                    ( NewOrdinal < NewExportFunctionCount )) {

                                    AddRiftEntryToTable(
                                        RiftTable,
                                        OldExportFunctionArray[ OldOrdinal ],
                                        NewExportFunctionArray[ NewOrdinal ]
                                        );
                                    }
                                }
                            }
                        }
                    }
                }
            }

        //
        //  Now use ordinals to match any exports that don't have names.
        //  We use the NameToOrdinal table to determine if a name exists.
        //  For all ordinals that don't have a NameToOrdinal entry, we
        //  create a match.
        //

        if (( NewExportFunctionArray ) && ( NewExportNameToOrdinal )) {

            NewExportOrdinalNameExists = SubAllocate( SubAllocator, NewExportFunctionCount * sizeof( NewExportOrdinalNameExists[ 0 ] ));

            if ( NewExportOrdinalNameExists != NULL ) {

                for ( NewExportIndex = 0; NewExportIndex < NewExportNameCount; NewExportIndex++ ) {

                    NewOrdinal = NewExportNameToOrdinal[ NewExportIndex ];

                    if ( NewOrdinal < NewExportFunctionCount ) {

                        NewExportOrdinalNameExists[ NewOrdinal ] = TRUE;

                        }
                    }

                OrdinalBaseNewToOld = (LONG)NewExportDir->Base - (LONG)OldExportDir->Base;

                for ( NewOrdinal = 0; NewOrdinal < NewExportFunctionCount; NewOrdinal++ ) {

                    if ( ! NewExportOrdinalNameExists[ NewOrdinal ] ) {

                        OldOrdinal = NewOrdinal + OrdinalBaseNewToOld;

                        if ( OldOrdinal < OldExportFunctionCount ) {

                            AddRiftEntryToTable(
                                RiftTable,
                                OldExportFunctionArray[ OldOrdinal ],
                                NewExportFunctionArray[ NewOrdinal ]
                                );
                            }
                        }
                    }
                }
            }
        }
    }

    //
    //  Create rifts for image import directory
    //

    {
    PIMAGE_IMPORT_DESCRIPTOR OldImportDir;
    PIMAGE_IMPORT_DESCRIPTOR NewImportDir;

    ULONG                    OldImportDirRva;
    ULONG                    NewImportDirRva;

    ULONG                    OldImportDirIndex;
    ULONG                    NewImportDirIndex;

    LPSTR                    OldImportDllName;
    LPSTR                    NewImportDllName;

    LPSTR                    OldImportDllNameLowercase;
    LPSTR                    NewImportDllNameLowercase;

    PIMAGE_THUNK_DATA        OldImportThunk;
    PIMAGE_THUNK_DATA        NewImportThunk;

    PIMAGE_THUNK_DATA        OldImportOriginalThunk;
    PIMAGE_THUNK_DATA        NewImportOriginalThunk;

    ULONG                    OldImportThunkIndex;
    ULONG                    NewImportThunkIndex;

    PIMAGE_IMPORT_BY_NAME    OldImportByName;
    PIMAGE_IMPORT_BY_NAME    NewImportByName;

    LPSTR                    OldImportName;
    LPSTR                    NewImportName;

    SYMBOL_TREE NewImportDllNameTree;
    SYMBOL_TREE NewImportFunctionNameTree;

    PSYMBOL_NODE NewImportDllSymbolNode;
    PSYMBOL_NODE NewImportFunctionSymbolNode;



    OldImportDirRva = ImageDirectoryRvaAndSize(
                          OldFileNtHeader,
                          IMAGE_DIRECTORY_ENTRY_IMPORT,
                          NULL
                          );

    NewImportDirRva = ImageDirectoryRvaAndSize(
                          NewFileNtHeader,
                          IMAGE_DIRECTORY_ENTRY_IMPORT,
                          NULL
                          );

    OldImportDir = ImageDirectoryMappedAddress(
                       OldFileNtHeader,
                       IMAGE_DIRECTORY_ENTRY_IMPORT,
                       NULL,
                       OldFileMapped,
                       OldFileSize
                       );

    NewImportDir = ImageDirectoryMappedAddress(
                       NewFileNtHeader,
                       IMAGE_DIRECTORY_ENTRY_IMPORT,
                       NULL,
                       NewFileMapped,
                       NewFileSize
                       );

    if (( OldImportDir ) && ( NewImportDir )) {

        //
        //  Now build a tree of new import names, then walk old export names
        //  looking for matches in tree of new import names.
        //

        SymRBInitTree(
            &NewImportDllNameTree,
            SubAllocator
            );

        SymRBInitTree(
            &NewImportFunctionNameTree,
            SubAllocator
            );

        for ( NewImportDirIndex = 0; NewImportDir[ NewImportDirIndex ].Characteristics; NewImportDirIndex++ ) {

            if ( NewImportDir[ NewImportDirIndex ].Name ) {

                NewImportDllName = ImageRvaToMappedAddress( NewFileNtHeader, NewImportDir[ NewImportDirIndex ].Name, NewFileMapped, NewFileSize );

                if ( NewImportDllName ) {

                    NewImportDllNameLowercase = MySubAllocStrDup( SubAllocator, NewImportDllName );

                    if ( NewImportDllNameLowercase ) {

                        MyLowercase( NewImportDllNameLowercase );

                        SymRBInsert( &NewImportDllNameTree, NewImportDllNameLowercase, NewImportDirIndex );

                        NewImportThunk = ImageRvaToMappedAddress( NewFileNtHeader, (ULONG)NewImportDir[ NewImportDirIndex ].FirstThunk, NewFileMapped, NewFileSize );

                        if ( NewImportThunk ) {

                            for ( NewImportThunkIndex = 0; NewImportThunk[ NewImportThunkIndex ].u1.Ordinal; NewImportThunkIndex++ ) {

                                if ( ! IMAGE_SNAP_BY_ORDINAL( NewImportThunk[ NewImportThunkIndex ].u1.Ordinal )) {

                                    NewImportByName = ImageRvaToMappedAddress( NewFileNtHeader, (ULONG)NewImportThunk[ NewImportThunkIndex ].u1.AddressOfData, NewFileMapped, NewFileSize );

                                    if ( NewImportByName ) {

                                        NewImportName = MySubAllocStrDupAndCat(
                                                            SubAllocator,
                                                            NewImportDllNameLowercase,
                                                            (LPSTR)NewImportByName->Name,
                                                            '!'
                                                            );

                                        if ( NewImportName ) {

                                            SymRBInsert( &NewImportFunctionNameTree, NewImportName, NewImportThunkIndex );

                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        for ( OldImportDirIndex = 0; OldImportDir[ OldImportDirIndex ].Characteristics; OldImportDirIndex++ ) {

            if ( OldImportDir[ OldImportDirIndex ].Name ) {

                OldImportDllName = ImageRvaToMappedAddress( OldFileNtHeader, OldImportDir[ OldImportDirIndex ].Name, OldFileMapped, OldFileSize );

                if ( OldImportDllName ) {

                    OldImportDllNameLowercase = MySubAllocStrDup( SubAllocator, OldImportDllName );

                    if ( OldImportDllNameLowercase ) {

                        MyLowercase( OldImportDllNameLowercase );

                        NewImportDllSymbolNode = SymRBFind( &NewImportDllNameTree, OldImportDllNameLowercase );

                        if ( NewImportDllSymbolNode ) {

                            //
                            //  Found a matching dll import descriptor.
                            //  This will give us four rifts: one for the
                            //  descriptor itself, another for the
                            //  dll name referenced by the descriptor, and
                            //  the FirstThunk and OriginalFirstThunk
                            //  arrays.
                            //
                            //  The index of the new import descriptor is
                            //  stored in the Rva field of the node.
                            //

                            NewImportDirIndex = NewImportDllSymbolNode->Rva;

                            AddRiftEntryToTable(
                                RiftTable,
                                (ULONG)((PIMAGE_IMPORT_DESCRIPTOR)OldImportDirRva + OldImportDirIndex ),
                                (ULONG)((PIMAGE_IMPORT_DESCRIPTOR)NewImportDirRva + NewImportDirIndex )
                                );

                            AddRiftEntryToTable(
                                RiftTable,
                                (ULONG)OldImportDir[ OldImportDirIndex ].Name,
                                (ULONG)NewImportDir[ NewImportDirIndex ].Name
                                );

                            AddRiftEntryToTable(
                                RiftTable,
                                (ULONG)OldImportDir[ OldImportDirIndex ].OriginalFirstThunk,
                                (ULONG)NewImportDir[ NewImportDirIndex ].OriginalFirstThunk
                                );

                            AddRiftEntryToTable(
                                RiftTable,
                                (ULONG)OldImportDir[ OldImportDirIndex ].FirstThunk,
                                (ULONG)NewImportDir[ NewImportDirIndex ].FirstThunk
                                );

                            OldImportThunk = ImageRvaToMappedAddress( OldFileNtHeader, (ULONG)OldImportDir[ OldImportDirIndex ].FirstThunk, OldFileMapped, OldFileSize );

                            if ( OldImportThunk ) {

                                for ( OldImportThunkIndex = 0; OldImportThunk[ OldImportThunkIndex ].u1.Ordinal; OldImportThunkIndex++ ) {

                                    if ( ! IMAGE_SNAP_BY_ORDINAL( OldImportThunk[ OldImportThunkIndex ].u1.Ordinal )) {

                                        OldImportByName = ImageRvaToMappedAddress( OldFileNtHeader, (ULONG)OldImportThunk[ OldImportThunkIndex ].u1.AddressOfData, OldFileMapped, OldFileSize );

                                        if ( OldImportByName ) {

                                            OldImportName = MySubAllocStrDupAndCat(
                                                                SubAllocator,
                                                                OldImportDllNameLowercase,
                                                                (LPSTR)OldImportByName->Name,
                                                                '!'
                                                                );

                                            if ( OldImportName ) {

                                                NewImportFunctionSymbolNode = SymRBFind( &NewImportFunctionNameTree, OldImportName );

                                                if ( NewImportFunctionSymbolNode ) {

                                                    //
                                                    //  Found a matching import function name.
                                                    //  This will give us two rifts: one for the
                                                    //  FirstThunk arrays and another for the
                                                    //  OriginalFirstThunk arrays.
                                                    //
                                                    //  The index of the new import thunk is
                                                    //  stored in the Rva field of the node.
                                                    //

                                                    NewImportThunkIndex = NewImportFunctionSymbolNode->Rva;

                                                    NewImportThunk = ImageRvaToMappedAddress( NewFileNtHeader, (ULONG)NewImportDir[ NewImportDirIndex ].FirstThunk, NewFileMapped, NewFileSize );

                                                    if ( NewImportThunk ) {

                                                        AddRiftEntryToTable(
                                                            RiftTable,
                                                            (ULONG)OldImportThunk[ OldImportThunkIndex ].u1.AddressOfData,
                                                            (ULONG)NewImportThunk[ NewImportThunkIndex ].u1.AddressOfData
                                                            );
                                                        }

                                                    OldImportOriginalThunk = ImageRvaToMappedAddress( OldFileNtHeader, (ULONG)OldImportDir[ OldImportDirIndex ].OriginalFirstThunk, OldFileMapped, OldFileSize );
                                                    NewImportOriginalThunk = ImageRvaToMappedAddress( NewFileNtHeader, (ULONG)NewImportDir[ NewImportDirIndex ].OriginalFirstThunk, NewFileMapped, NewFileSize );

                                                    if ( OldImportOriginalThunk && NewImportOriginalThunk ) {

                                                        AddRiftEntryToTable(
                                                            RiftTable,
                                                            (ULONG)OldImportOriginalThunk[ OldImportThunkIndex ].u1.AddressOfData,
                                                            (ULONG)NewImportOriginalThunk[ NewImportThunkIndex ].u1.AddressOfData
                                                            );
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //
    //  Create rift entries for resources
    //

    {
    PUCHAR OldResBase, NewResBase;
    ULONG  OldResSize, NewResSize;
    ULONG  OldResRva,  NewResRva;

    OldResBase = ImageDirectoryMappedAddress(
                OldFileNtHeader,
                IMAGE_DIRECTORY_ENTRY_RESOURCE,
                &OldResSize,
                OldFileMapped,
                OldFileSize
                );

    NewResBase = ImageDirectoryMappedAddress(
                NewFileNtHeader,
                IMAGE_DIRECTORY_ENTRY_RESOURCE,
                &NewResSize,
                NewFileMapped,
                NewFileSize
                );

    if ( OldResBase && NewResBase ) {

        OldResRva = ImageDirectoryRvaAndSize( OldFileNtHeader, IMAGE_DIRECTORY_ENTRY_RESOURCE, NULL );
        NewResRva = ImageDirectoryRvaAndSize( NewFileNtHeader, IMAGE_DIRECTORY_ENTRY_RESOURCE, NULL );

        GetResourceRiftInfoRecursive(
            (PIMAGE_RESOURCE_DIRECTORY) OldResBase,
            OldResBase,
            OldResBase + OldResSize,
            OldResRva,
            (PIMAGE_RESOURCE_DIRECTORY) NewResBase,
            NewResBase,
            NewResBase + NewResSize,
            NewResRva,
            RiftTable
            );
        }
    }

    //
    //  BUGBUG: Gen rift info for other non-symbol stuff here.
    //

#ifdef TESTCODE
    printf( "\r%9d non-symbol rift entries\n", RiftTable->RiftEntryCount );
#endif

    return TRUE;
    }


VOID
MyUndecorateSymbol(
    IN  LPCSTR DecoratedSymbol,
    OUT LPSTR  UndecoratedSymbol,
    IN  DWORD  BufferSize
    )
    {
    LPCSTR d;
    LPCSTR e;
    ULONG  Len;
    ULONG  Ext;

    *UndecoratedSymbol = 0;   // in case UnDecorateSymbolName fails

    d = DecoratedSymbol;

    if (( d[ 0 ] == '.' ) &&
        ( d[ 1 ] == '.' ) &&
        ( d[ 2 ] == '?' )) {

        d += 2;
        }

    if ( *d == '?' ) {

        Imagehlp.UnDecorateSymbolName( d, UndecoratedSymbol, BufferSize, UNDNAME_NAME_ONLY );

        //
        //  UnDecorateSymbolName will strip any trailing '_nnn' (from BBT omap
        //  info), but we want to preserved it.  Check for that pattern in the
        //  original, and if found, append it to the new string.
        //

        d += strlen( d + 1 );   // point d to last character in string

        if (( *d >= '0' ) && ( *d <= '9' )) {

            do  {
                --d;
                }
            while (( *d >= '0' ) && ( *d <= '9' ));

            if ( *d == '_' ) {

                //
                //  Matches the '_nnn' pattern, append to new string.
                //

                if (( strlen( UndecoratedSymbol ) + strlen( d )) < ( BufferSize - 1 )) {
                    strcat( UndecoratedSymbol, d );
                    }
                }
            }
        }

    else {

        //
        //  Strip any preceding '_' or '@'.
        //

        if (( *d == '_' ) || ( *d == '@' )) {
            ++d;
            }

        //
        //  Find end of name as either terminator or '@nn'.
        //

        for ( e = d; ( *e ) && ( *e != '@' ); ) {
            ++e;
            }

        //
        //  Copy as much of name as will fit in the buffer.
        //

        Len = e - d;

        if ( Len > ( BufferSize - 1 )) {
             Len = ( BufferSize - 1 );
             }

        memcpy( UndecoratedSymbol, d, Len );

        if ( *e == '@' ) {

            //
            //  Skip '@nn' to append remainder of symbol
            //

            do  {
                ++e;
                }
            while (( *e >= '0' ) && ( *e <= '9' ));

            d = e;

            while ( *e ) {
                ++e;
                }

            //
            //  Now 'd' points to first character after '@nn' and 'e' points
            //  to end of the string.  If the extension will fit in the buffer,
            //  append it.
            //

            Ext = e - d;

            if (( Len + Ext ) < ( BufferSize - 1 )) {
                memcpy( UndecoratedSymbol + Len, d, Ext );
                }

            Len += Ext;
            }

        //
        //  Terminate the string.
        //

        UndecoratedSymbol[ Len ] = 0;
        }
    }


BOOL
UndecorateSymbolAndAddToTree(
    IN LPCSTR       SymbolName,
    IN ULONG        Rva,
    IN PSYMBOL_TREE SymbolTree
    )
    {
    ULONG SymbolNameSize  = strlen( SymbolName ) + 1;
    LPSTR UndecoratedName = _alloca( SymbolNameSize );
    PSYMBOL_NODE SymbolNode;

    MyUndecorateSymbol(
        SymbolName,
        UndecoratedName,
        SymbolNameSize
        );

    SymbolNode = SymRBInsert(
                     SymbolTree,
                     UndecoratedName,
                     Rva
                     );

    return ( SymbolNode != NULL );
    }


BOOL
CALLBACK
NewFileEnumSymbolsCallback(
    LPSTR SymbolName,
    ULONG SymbolAddr,
    ULONG SymbolSize,
    PVOID Context
    )
    {
    PSYMBOL_CONTEXT SymbolContext = Context;
    PSYMBOL_NODE SymbolNode;
    ULONG NewRva;

    UNREFERENCED_PARAMETER( SymbolSize );

#ifdef TESTCODE

    if ( SymbolContext->OutFile != INVALID_HANDLE_VALUE ) {

        CHAR  TextBuffer[ 16 + MAX_SYMBOL_NAME_LENGTH ];
        CHAR  Discarded;
        DWORD Actual;

        Discarded = 'X';
        NewRva = SymbolAddr;

        if ( NewRva > SymbolContext->NewImageBase ) {
            NewRva -= SymbolContext->NewImageBase;
            Discarded = ' ';
            }

        sprintf( TextBuffer, "%08X %c %s\r\n", NewRva, Discarded, SymbolName );
        WriteFile( SymbolContext->OutFile, TextBuffer, strlen( TextBuffer ), &Actual, NULL );
        }

#endif // TESTCODE

    if ( SymbolAddr > SymbolContext->NewImageBase ) {

        NewRva = SymbolAddr - SymbolContext->NewImageBase;

        SymbolNode = SymRBInsert(
                         &SymbolContext->NewDecoratedSymbolTree,
                         SymbolName,
                         NewRva
                         );

        return ( SymbolNode != NULL );
        }

    return TRUE;
    }


BOOL
CALLBACK
OldFileEnumSymbolsCallback(
    LPSTR SymbolName,
    ULONG SymbolAddr,
    ULONG SymbolSize,
    PVOID Context
    )
    {
    PSYMBOL_CONTEXT SymbolContext = Context;
    PSYMBOL_NODE SymbolNode;
    ULONG OldRva;

    UNREFERENCED_PARAMETER( SymbolSize );

#ifdef TESTCODE

    if ( SymbolContext->OutFile != INVALID_HANDLE_VALUE ) {

        CHAR  TextBuffer[ 16 + MAX_SYMBOL_NAME_LENGTH ];
        CHAR  Discarded;
        DWORD Actual;

        Discarded = 'X';
        OldRva = SymbolAddr;

        if ( OldRva > SymbolContext->OldImageBase ) {
            OldRva -= SymbolContext->OldImageBase;
            Discarded = ' ';
            }

        sprintf( TextBuffer, "%08X %c %s\r\n", OldRva, Discarded, SymbolName );
        WriteFile( SymbolContext->OutFile, TextBuffer, strlen( TextBuffer ), &Actual, NULL );
        }

#endif // TESTCODE

    if ( SymbolAddr > SymbolContext->OldImageBase ) {

        OldRva = SymbolAddr - SymbolContext->OldImageBase;

        SymbolNode = SymRBFind(
                         &SymbolContext->NewDecoratedSymbolTree,
                         SymbolName
                         );

        if ( SymbolNode ) {

            AddRiftEntryToTable( SymbolContext->RiftTable, OldRva, SymbolNode->Rva );

            SymbolNode->Hit = 1;

#ifdef TESTCODE

            CountDecoratedMatches++;

#endif // TESTCODE

            }

        else {

            //
            //  Didn't find matching new symbol.  Build a tree of unmatched
            //  old symbols with UNdecorated names.  Later we'll match up
            //  remaining unmatched new symbols to these unmatched old symbols
            //  by their undecorated names.
            //

            if ( SymbolContext->SymbolOptionFlags & PATCH_SYMBOL_UNDECORATED_TOO ) {

                return UndecorateSymbolAndAddToTree(
                           SymbolName,
                           OldRva,
                           &SymbolContext->OldUndecoratedSymbolTree
                           );
                }
            }
        }

    return TRUE;
    }


BOOL
MatchRemainingSymbolsThisNode(
    IN PSYMBOL_NODE NewDecoratedSymbolNode,
    IN PSYMBOL_TREE NewUndecoratedSymbolTree,
    IN PSYMBOL_TREE OldUndecoratedSymbolTree,
    IN PRIFT_TABLE  RiftTable
    )
    {
    if ( ! NewDecoratedSymbolNode->Hit ) {

        ULONG SymbolNameSize     = strlen( NewDecoratedSymbolNode->SymbolName ) + 1;
        LPSTR NewUndecoratedName = _alloca( SymbolNameSize );
        PSYMBOL_NODE NewUndecoratedSymbolNode;
        PSYMBOL_NODE OldUndecoratedSymbolNode;

        MyUndecorateSymbol(
            NewDecoratedSymbolNode->SymbolName,
            NewUndecoratedName,
            SymbolNameSize
            );

        OldUndecoratedSymbolNode = SymRBFind(
                                       OldUndecoratedSymbolTree,
                                       NewUndecoratedName
                                       );

        if ( OldUndecoratedSymbolNode ) {

            AddRiftEntryToTable(
                RiftTable,
                OldUndecoratedSymbolNode->Rva,
                NewDecoratedSymbolNode->Rva
                );

            OldUndecoratedSymbolNode->Hit = 1;

#ifdef TESTCODE

            CountUndecoratedMatches++;

#endif // TESTCODE

            }

        else {

            //
            //  This new symbol has no match in the old symbol tree.  Build a
            //  tree of unmatched new undecorated symbols.
            //

            NewUndecoratedSymbolNode = SymRBInsert(
                                           NewUndecoratedSymbolTree,
                                           NewUndecoratedName,
                                           NewDecoratedSymbolNode->Rva
                                           );

            return ( NewUndecoratedSymbolNode != NULL );
            }
        }

    return TRUE;
    }


BOOL
MatchRemainingSymbolsRecursive(
    IN PSYMBOL_NODE NewDecoratedSymbolNode,
    IN PSYMBOL_TREE NewUndecoratedSymbolTree,
    IN PSYMBOL_TREE OldUndecoratedSymbolTree,
    IN PRIFT_TABLE  RiftTable
    )
    {
    if ( NewDecoratedSymbolNode == RBNIL ) {
        return TRUE;
        }

    return ( MatchRemainingSymbolsRecursive( NewDecoratedSymbolNode->Left,  NewUndecoratedSymbolTree, OldUndecoratedSymbolTree, RiftTable ) &&
             MatchRemainingSymbolsRecursive( NewDecoratedSymbolNode->Right, NewUndecoratedSymbolTree, OldUndecoratedSymbolTree, RiftTable ) &&
             MatchRemainingSymbolsThisNode(  NewDecoratedSymbolNode,        NewUndecoratedSymbolTree, OldUndecoratedSymbolTree, RiftTable ));
    }


#ifdef TESTCODE

VOID
DumpUnHitSymbolNode(
    IN PSYMBOL_NODE SymbolNode,
    IN HANDLE hFile
    )
    {
    CHAR  TextBuffer[ 16 + MAX_SYMBOL_NAME_LENGTH ];
    DWORD Actual;

    if ( ! SymbolNode->Hit ) {
        sprintf( TextBuffer, "%08X   %s\r\n", SymbolNode->Rva, SymbolNode->SymbolName );
        WriteFile( hFile, TextBuffer, strlen( TextBuffer ), &Actual, NULL );
        }
    }


VOID
DumpUnHitSymbolNodesRecursive(
    IN PSYMBOL_NODE SymbolNode,
    IN HANDLE hFile
    )
    {
    if ( SymbolNode == RBNIL ) {
        return;
        }

    //
    //  The tree is in hash order, not Rva order, so the output will appear
    //  to be in random order (easily solved with sort.exe utility).
    //

    DumpUnHitSymbolNode( SymbolNode, hFile );
    DumpUnHitSymbolNodesRecursive( SymbolNode->Left,  hFile );
    DumpUnHitSymbolNodesRecursive( SymbolNode->Right, hFile );
    }


VOID
DumpUnHitSymbolNodes(
    IN PSYMBOL_TREE SymbolTree,
    IN LPCSTR DumpFileName
    )
    {
    HANDLE hFile;

    hFile = CreateFile(
                DumpFileName,
                GENERIC_WRITE,
                FILE_SHARE_READ,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
                );

    if ( hFile != INVALID_HANDLE_VALUE ) {

        DumpUnHitSymbolNodesRecursive( SymbolTree->Root, hFile );

        CloseHandle( hFile );
        }
    }

#endif // TESTCODE


BOOL
MatchRemainingSymbols(
    IN PSYMBOL_TREE NewDecoratedSymbolTree,
    IN PSYMBOL_TREE NewUndecoratedSymbolTree,
    IN PSYMBOL_TREE OldUndecoratedSymbolTree,
    IN PRIFT_TABLE  RiftTable
    )
    {
    BOOL Success;

    //
    //  Walk old tree, for each unmatched symbol, undecorate it and try to
    //  find match in the undecorated new symbol tree.  If it fails to match,
    //  add it to the old undecorated tree.
    //

    Success = MatchRemainingSymbolsRecursive(
                  NewDecoratedSymbolTree->Root,
                  NewUndecoratedSymbolTree,
                  OldUndecoratedSymbolTree,
                  RiftTable
                  );

    if ( Success ) {

        //
        //  Now we have remaining unmatched undecorated symbols in the
        //  OldUndecoratedSymbolTree and NewUndecoratedSymbolTree.
        //
        //  Here is an opportunity to do soft name matching, like for
        //  BBT omap generated symbol names (have trailing '_nnn').
        //  Unfortunately, current versions of imagehlp append an '_nnn'
        //  value that is useless becaue it is not the offset from the
        //  start of the function.
        //

#ifdef TESTCODE

        DumpUnHitSymbolNodes( OldUndecoratedSymbolTree, "UnmatchedOldSymbols.out" );
        DumpUnHitSymbolNodes( NewUndecoratedSymbolTree, "UnmatchedNewSymbols.out" );

#endif // TESTCODE

        }

    return Success;
    }


BOOL
GetImageSymbolRiftInfo(
    IN HANDLE                  OldFileHandle,
    IN PUCHAR                  OldFileMapped,
    IN ULONG                   OldFileSize,
    IN PIMAGE_NT_HEADERS       OldFileNtHeader,
    IN LPCSTR                  OldFileSymPath,
    IN ULONG                   OldFileOriginalChecksum,
    IN ULONG                   OldFileOriginalTimeDate,
    IN ULONG                   OldFileIndex,
    IN HANDLE                  NewFileHandle,
    IN PUCHAR                  NewFileMapped,
    IN ULONG                   NewFileSize,
    IN PIMAGE_NT_HEADERS       NewFileNtHeader,
    IN LPCSTR                  NewFileSymPath,
    IN ULONG                   SymbolOptionFlags,
    IN HANDLE                  SubAllocator,
    IN PRIFT_TABLE             RiftTable,
    IN PPATCH_SYMLOAD_CALLBACK SymLoadCallback,
    IN PVOID                   SymLoadContext
    )
    {
    SYMBOL_CONTEXT SymbolContext;
    DWORD SymOptions;
    ULONG OldBase;
    ULONG NewBase;
    BOOL  Success;

#ifdef TESTCODE
    ULONG InitialRiftEntries = RiftTable->RiftEntryCount;
#endif

    UNREFERENCED_PARAMETER( OldFileNtHeader );
    UNREFERENCED_PARAMETER( OldFileMapped );
    UNREFERENCED_PARAMETER( OldFileSize );
    UNREFERENCED_PARAMETER( NewFileMapped );
    UNREFERENCED_PARAMETER( NewFileSize );

    InitImagehlpCritSect();

    EnterCriticalSection( &ImagehlpCritSect );

    Success = LoadImagehlp();

    if ( Success ) {

        __try {

            SymOptions = Imagehlp.SymGetOptions();

            SymOptions &= ~SYMOPT_CASE_INSENSITIVE;
            SymOptions &= ~SYMOPT_UNDNAME;
            SymOptions &= ~SYMOPT_DEFERRED_LOADS;

            Imagehlp.SymSetOptions( SymOptions );

            Success = Imagehlp.SymInitialize( hProc, NewFileSymPath, FALSE );

            if ( Success ) {

                __try {

                    SymRBInitTree(
                        &SymbolContext.NewDecoratedSymbolTree,
                        SubAllocator
                        );

                    SymRBInitTree(
                        &SymbolContext.NewUndecoratedSymbolTree,
                        SubAllocator
                        );

                    SymRBInitTree(
                        &SymbolContext.OldUndecoratedSymbolTree,
                        SubAllocator
                        );

                    NewBase = Imagehlp.SymLoadModule( hProc, NewFileHandle, NULL, "New", (ULONG)NewFileMapped, NewFileSize );

                    Success = ( NewBase != 0 );

                    if ( Success ) {

                        __try {

                            if ( SymLoadCallback ) {

                                ZeroMemory( &ImagehlpModuleInfo, sizeof( ImagehlpModuleInfo ));
                                ImagehlpModuleInfo.SizeOfStruct = sizeof( ImagehlpModuleInfo );

                                Success = Imagehlp.SymGetModuleInfo(
                                              hProc,
                                              NewBase,
                                              &ImagehlpModuleInfo
                                              );

                                if ( Success ) {

                                    Success = SymLoadCallback(
                                                  0,
                                                  ImagehlpModuleInfo.LoadedImageName,
                                                  ImagehlpModuleInfo.SymType,
                                                  ImagehlpModuleInfo.CheckSum,
                                                  ImagehlpModuleInfo.TimeDateStamp,
                                                  NewFileNtHeader->OptionalHeader.CheckSum,
                                                  NewFileNtHeader->FileHeader.TimeDateStamp,
                                                  SymLoadContext
                                                  );
                                    }
                                }

                            if ( Success ) {

                                SymbolContext.NewImageBase      = NewBase;
                                SymbolContext.SymbolOptionFlags = SymbolOptionFlags;
                                SymbolContext.RiftTable         = RiftTable;
#ifdef TESTCODE
                                CountDecoratedMatches   = 0;
                                CountUndecoratedMatches = 0;

                                SymbolContext.OutFile = CreateFile(
                                                            "NewSymbols.out",
                                                            GENERIC_WRITE,
                                                            FILE_SHARE_READ,
                                                            NULL,
                                                            CREATE_ALWAYS,
                                                            FILE_ATTRIBUTE_NORMAL,
                                                            NULL
                                                            );
#endif // TESTCODE

                                Success = Imagehlp.SymEnumerateSymbols( hProc, NewBase, NewFileEnumSymbolsCallback, &SymbolContext );

#ifdef TESTCODE
                                if ( SymbolContext.OutFile != INVALID_HANDLE_VALUE ) {
                                    CloseHandle( SymbolContext.OutFile );
                                    }
#endif // TESTCODE
                                }
                        }

                        __except( EXCEPTION_EXECUTE_HANDLER ) {
                            Success = FALSE;
                            }

                        Imagehlp.SymUnloadModule( hProc, NewBase );
                        }
                    }

                __except( EXCEPTION_EXECUTE_HANDLER ) {
                    Success = FALSE;
                    }

                //
                //  Must do cleanup and reinitialize Imagehlp for this
                //  process identifier.  Otherwise it thinks the old
                //  module is still hanging around.
                //

                Imagehlp.SymCleanup( hProc );

                if ( Success ) {

                    Success = Imagehlp.SymInitialize( hProc, OldFileSymPath, FALSE );

                    if ( Success ) {

                        __try {

                            OldBase = Imagehlp.SymLoadModule( hProc, OldFileHandle, NULL, "Old", (ULONG)OldFileMapped, OldFileSize );

                            Success = ( OldBase != 0 );

                            if ( Success ) {

                                __try {

                                    if ( SymLoadCallback ) {

                                        ZeroMemory( &ImagehlpModuleInfo, sizeof( ImagehlpModuleInfo ));
                                        ImagehlpModuleInfo.SizeOfStruct = sizeof( ImagehlpModuleInfo );

                                        Success = Imagehlp.SymGetModuleInfo(
                                                      hProc,
                                                      OldBase,
                                                      &ImagehlpModuleInfo
                                                      );

                                        if ( Success ) {

                                            Success = SymLoadCallback(
                                                          OldFileIndex + 1,
                                                          ImagehlpModuleInfo.LoadedImageName,
                                                          ImagehlpModuleInfo.SymType,
                                                          ImagehlpModuleInfo.CheckSum,
                                                          ImagehlpModuleInfo.TimeDateStamp,
                                                          OldFileOriginalChecksum,
                                                          OldFileOriginalTimeDate,
                                                          SymLoadContext
                                                          );
                                            }
                                        }

                                    if ( Success ) {

                                        SymbolContext.OldImageBase = OldBase;
#ifdef TESTCODE
                                        SymbolContext.OutFile = CreateFile(
                                                                    "OldSymbols.out",
                                                                    GENERIC_WRITE,
                                                                    FILE_SHARE_READ,
                                                                    NULL,
                                                                    CREATE_ALWAYS,
                                                                    FILE_ATTRIBUTE_NORMAL,
                                                                    NULL
                                                                    );
#endif // TESTCODE

                                        Success = Imagehlp.SymEnumerateSymbols( hProc, OldBase, OldFileEnumSymbolsCallback, &SymbolContext );

#ifdef TESTCODE
                                        if ( SymbolContext.OutFile != INVALID_HANDLE_VALUE ) {
                                            CloseHandle( SymbolContext.OutFile );
                                            }
#endif // TESTCODE
                                        }

                                    if ( Success ) {

                                        //
                                        //  Need to match remaining decorated new symbols in tree
                                        //  with unmatched now-undecorated old symbols in other tree.
                                        //

                                        if ( SymbolOptionFlags & PATCH_SYMBOL_UNDECORATED_TOO ) {

                                            Success = MatchRemainingSymbols(
                                                          &SymbolContext.NewDecoratedSymbolTree,
                                                          &SymbolContext.NewUndecoratedSymbolTree,
                                                          &SymbolContext.OldUndecoratedSymbolTree,
                                                          RiftTable
                                                          );
                                            }
                                        }
                                    }

                                __except( EXCEPTION_EXECUTE_HANDLER ) {
                                    Success = FALSE;
                                    }

                                Imagehlp.SymUnloadModule( hProc, OldBase );
                                }
                            }

                        __except( EXCEPTION_EXECUTE_HANDLER ) {
                            Success = FALSE;
                            }

                        Imagehlp.SymCleanup( hProc );
                        }
                    }
                }
            }

        __except( EXCEPTION_EXECUTE_HANDLER ) {
            Success = FALSE;
            }
        }

    LeaveCriticalSection( &ImagehlpCritSect );

    if ( ! Success ) {
        SetLastError( ERROR_PATCH_IMAGEHLP_FAILURE );
        }

#ifdef TESTCODE
    printf( "\r%9d decorated symbol matches\n", CountDecoratedMatches );
    printf( "\r%9d undecorated symbol matches\n", CountUndecoratedMatches );
    printf( "\r%9d rift entries from symbols\n", RiftTable->RiftEntryCount - InitialRiftEntries );
#endif

    return Success;
    }


BOOL
OptimizeImageRiftInfo(
    IN PUCHAR             OldFileMapped,
    IN ULONG              OldFileSize,
    IN PIMAGE_NT_HEADERS  OldFileNtHeader,
    IN PUCHAR             NewFileMapped,
    IN ULONG              NewFileSize,
    IN PIMAGE_NT_HEADERS  NewFileNtHeader,
    IN HANDLE             SubAllocator,
    IN PRIFT_TABLE        RiftTable
    )
    {
    UNREFERENCED_PARAMETER( OldFileMapped );
    UNREFERENCED_PARAMETER( OldFileSize );
    UNREFERENCED_PARAMETER( OldFileNtHeader );
    UNREFERENCED_PARAMETER( NewFileMapped );
    UNREFERENCED_PARAMETER( NewFileSize );
    UNREFERENCED_PARAMETER( NewFileNtHeader );
    UNREFERENCED_PARAMETER( SubAllocator );
    UNREFERENCED_PARAMETER( RiftTable );

    return TRUE;
    }

#if 0   // BUGBUG: This is test code

BOOL
OptimizeImageRiftInfo(
    IN PUCHAR             OldFileMapped,
    IN ULONG              OldFileSize,
    IN PIMAGE_NT_HEADERS  OldFileNtHeader,
    IN PUCHAR             NewFileMapped,
    IN ULONG              NewFileSize,
    IN PIMAGE_NT_HEADERS  NewFileNtHeader,
    IN HANDLE             SubAllocator,
    IN PRIFT_TABLE        RiftTable
    )
    {
    PIMAGE_SECTION_HEADER SectionHeader;
    ULONG  SectionCount;
    PUCHAR SectionStart;
    PUCHAR SearchExtent;
    ULONG  SectionLength;
    ULONG  SectionOffset;
    ULONG  SectionBaseRva;
    ULONG  OldFileLastRva;
    ULONG  NewFileLastRva;
    LONG   OldDisplacement;
    LONG   NewDisplacement;
    ULONG  OffsetInSection;
    ULONG  OldOriginRva;
    ULONG  OldTargetRva;
    ULONG  TargetOffset;
    PUCHAR OldFileHintMap;
    PUCHAR NewFileHintMap;
    PUCHAR OldFileCopy;
    PUCHAR NewFileCopy;
    PVOID  OldFileCopyNtHeader;
    PVOID  NewFileCopyNtHeader;
    BOOL   Success;
    BOOL   Skip;
    ULONG  i;
    ULONG  j;
    PUCHAR p;

#ifdef TESTCODE
    ULONG CountVerifiedE8Rifts = 0;
    ULONG CountDiscoveredE8Rifts = 0;
#endif // TESTCODE

    //
    //  Stage1:
    //
    //      Trusting existing rift info, search for E8/E9/Jcc instructions in
    //      the Old file, get the corresponding rift, inspect the corresponding
    //      instruction in the New file.  If they match, great.  If they don't
    //      match, search for a correponding instruction in the new file by
    //      looking forward up to the next rift.  If find a suitable match,
    //      create a new rift entry to support it.
    //
    //      BUGBUG: We don't have the benefit of non-exe marked bytes here.
    //

    if ( OldFileNtHeader->FileHeader.Machine != NewFileNtHeader->FileHeader.Machine ) {
        return TRUE;    // not much we can do here!
        }

    //
    //  We need the HintMap to determine which bytes in the files are not
    //  executable.  The Transform function currently provides this, but
    //  we don't want to modify our mapped file views here.  So, allocate
    //  a range of VM and make a copy of the file to Transform, perform
    //  the transformations on that copy just to produce a hint map, then
    //  free the transformed copy (do this for both old and new files).
    //

    Success = FALSE;

    OldFileHintMap = MyVirtualAlloc( OldFileSize );
    NewFileHintMap = MyVirtualAlloc( NewFileSize );

    if ( OldFileHintMap && NewFileHintMap ) {

        OldFileCopy = MyVirtualAlloc( OldFileSize );

        if ( OldFileCopy ) {

            CopyMemory( OldFileCopy, OldFileMapped, OldFileSize );

            OldFileCopyNtHeader = GetNtHeader( OldFileCopy, OldFileSize );

            Success = TransformCoffImage(
                          ( PATCH_TRANSFORM_NO_RELJMPS | PATCH_TRANSFORM_NO_RELCALLS | PATCH_TRANSFORM_NO_RESOURCE ),
                          OldFileCopyNtHeader,
                          OldFileCopy,
                          OldFileSize,
                          0,
                          RiftTable,
                          OldFileHintMap
                          );

            MyVirtualFree( OldFileCopy );
            }

        if ( Success ) {

            Success = FALSE;

            NewFileCopy = MyVirtualAlloc( NewFileSize );

            if ( NewFileCopy ) {

                CopyMemory( NewFileCopy, NewFileMapped, NewFileSize );

                NewFileCopyNtHeader = GetNtHeader( NewFileCopy, NewFileSize );

                Success = TransformCoffImage(
                              ( PATCH_TRANSFORM_NO_RELJMPS | PATCH_TRANSFORM_NO_RELCALLS | PATCH_TRANSFORM_NO_RESOURCE ),
                              NewFileCopyNtHeader,
                              NewFileCopy,
                              NewFileSize,
                              0,
                              RiftTable,
                              NewFileHintMap
                              );

                MyVirtualFree( NewFileCopy );
                }
            }
        }

    if ( Success ) {

        //
        //  We now have valid OldFileHintMap and NewFileHintMap.
        //

        OldFileLastRva = OldFileNtHeader->OptionalHeader.SizeOfImage;
        NewFileLastRva = NewFileNtHeader->OptionalHeader.SizeOfImage;

        InsertRiftEntryInSortedTable( RiftTable, RiftTable->RiftEntryCount, OldFileLastRva, NewFileLastRva );

        SectionHeader  = IMAGE_FIRST_SECTION( OldFileNtHeader );
        SectionCount   = OldFileNtHeader->FileHeader.NumberOfSections;

        for ( i = 0; i < SectionCount; i++ ) {

            if ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE ) {

                SectionBaseRva = SectionHeader[ i ].VirtualAddress;
                SectionLength  = MIN( SectionHeader[ i ].Misc.VirtualSize, SectionHeader[ i ].SizeOfRawData );
                SectionOffset  = SectionHeader[ i ].PointerToRawData;
                SectionStart   = OldFileMapped + SectionOffset;

                if ( OldFileNtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {

                    SearchExtent = SectionStart + SectionLength - 5;

                    for ( p = SectionStart; p < SearchExtent; p++ ) {

                        if ( *p == 0xE8 ) {         // call relative32

                            //
                            //  Validate that instruction is not something that
                            //  the HintMap indicates is not an executable
                            //  instruction.  We're looking for a relative
                            //  call instruction here, so it would not be a
                            //  reloc target.
                            //

                            Skip = FALSE;

                            for ( j = 0; j < 5; j++ ) {
                                if ( OldFileHintMap[ SectionOffset + ( p - SectionStart ) + j ] & 0x01 ) {
                                    Skip = TRUE;
                                    break;
                                    }
                                }

                            if ( Skip ) {
                                continue;
                                }

                            //
                            //  Relative displacement is stored as 32-bit
                            //  signed value following these opcodes.  The
                            //  displacement is relative to the NEXT
                            //  instruction, which is at (p + 5).
                            //

                            OldDisplacement = *(UNALIGNED LONG*)( p + 1 );
                            OffsetInSection = ( p + 5 ) - SectionStart;
                            OldOriginRva     = SectionBaseRva + OffsetInSection;
                            OldTargetRva     = OldOriginRva + OldDisplacement;

                            //
                            //  We expect a lot of false positives here because
                            //  occurences of <E8> will
                            //  likely occur in other parts of the instruction
                            //  stream so now we validate that the TargetRva
                            //  falls within the image and within an executable
                            //  section.
                            //

                            if ( OldTargetRva < OldFileLastRva ) {

                                TargetOffset = ImageRvaToFileOffset( OldFileNtHeader, OldTargetRva );

                                if ( ! ( OldFileHintMap[ TargetOffset ] & 0x01 )) {

                                    //
                                    //  Looks like a valid TargetRva, so lookup the
                                    //  corresponding "new" RVAs in the rift table.
                                    //

                                    ULONG RiftIndexOrigin = FindRiftTableEntryForOldRva( RiftTable, OldOriginRva );
                                    ULONG RiftIndexTarget = FindRiftTableEntryForOldRva( RiftTable, OldTargetRva );

                                    ULONG NewOriginRva = OldOriginRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexOrigin ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexOrigin ].OldFileRva );
                                    ULONG NewTargetRva = OldTargetRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexTarget ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexTarget ].OldFileRva );

                                    PUCHAR NewOriginMa = ImageRvaToMappedAddress( NewFileNtHeader, NewOriginRva, NewFileMapped, NewFileSize );
                                    PUCHAR NewTargetMa = ImageRvaToMappedAddress( NewFileNtHeader, NewTargetRva, NewFileMapped, NewFileSize );

                                    PUCHAR OldTargetMa = ImageRvaToMappedAddress( OldFileNtHeader, OldTargetRva, OldFileMapped, OldFileSize );
                                    PUCHAR OldOriginMa = p + 5;

                                    if (( NewOriginMa ) && ( NewTargetMa ) && ( OldTargetMa )) {

                                        PUCHAR OldInstruct = p;
                                        PUCHAR NewInstruct = NewOriginMa - 5;

                                        //
                                        //  A verified call instruction should match the
                                        //  instruction byte, the byte following the
                                        //  instruction, and the target byte, because
                                        //  they should all be executable code that is
                                        //  not modified by relocs.
                                        //
                                        //  Also verify the NewFileHintMap.
                                        //

                                        if (( *OldInstruct == *NewInstruct ) &&
                                            ( *OldOriginMa == *NewOriginMa ) &&
                                            ( *OldTargetMa == *NewTargetMa )) {

                                            ULONG NewInstructOffset = NewInstruct - NewFileMapped;
                                            ULONG NewTargetOffset   = NewTargetMa - NewFileMapped;
                                            BOOL  Skip = FALSE;

                                            for ( j = 0; j < 5; j++ ) {
                                                if ( NewFileHintMap[ NewInstructOffset + j ] & 0x01 ) {
                                                    Skip = TRUE;
                                                    break;
                                                    }
                                                }

                                            if ( NewFileHintMap[ NewTargetOffset + j ] & 0x01 ) {
                                                Skip = TRUE;
                                                }

                                            if ( ! Skip ) {

                                                //
                                                //  This is a bonafide good match.  Add a rift entry
                                                //  for both the instruction and the target.  We do
                                                //  this so that subsequent rift insertions don't get
                                                //  between the rift that coasted to these and these.
                                                //

                                                InsertRiftEntryInSortedTable( RiftTable, RiftIndexOrigin, OldOriginRva - 5, NewOriginRva - 5 );
                                                InsertRiftEntryInSortedTable( RiftTable, RiftIndexTarget, OldTargetRva, NewTargetRva );
#ifdef TESTCODE
                                                CountVerifiedE8Rifts++;
#endif // TESTCODE

                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                else if ( OldFileNtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_ALPHA ) {

                    //
                    //  BUGBUG: Implement Alpha platform stuff here.
                    //

                    }
                }
            }

        for ( i = 0; i < SectionCount; i++ ) {

            if ( SectionHeader[ i ].Characteristics & IMAGE_SCN_MEM_EXECUTE ) {

                SectionBaseRva = SectionHeader[ i ].VirtualAddress;
                SectionLength  = MIN( SectionHeader[ i ].Misc.VirtualSize, SectionHeader[ i ].SizeOfRawData );
                SectionOffset  = SectionHeader[ i ].PointerToRawData;
                SectionStart   = OldFileMapped + SectionOffset;

                if ( OldFileNtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 ) {

                    SearchExtent = SectionStart + SectionLength - 5;

                    for ( p = SectionStart; p < SearchExtent; p++ ) {

                        if ( *p == 0xE8 ) {         // call relative32

                            //
                            //  Validate that instruction is not something that
                            //  the HintMap indicates is not an executable
                            //  instruction.  We're looking for a relative
                            //  call instruction here, so it would not be a
                            //  reloc target.
                            //

                            Skip = FALSE;

                            for ( j = 0; j < 5; j++ ) {
                                if ( OldFileHintMap[ SectionOffset + ( p - SectionStart ) + j ] & 0x01 ) {
                                    Skip = TRUE;
                                    break;
                                    }
                                }

                            if ( Skip ) {
                                continue;
                                }

                            //
                            //  Relative displacement is stored as 32-bit
                            //  signed value following these opcodes.  The
                            //  displacement is relative to the NEXT
                            //  instruction, which is at (p + 5).
                            //

                            OldDisplacement = *(UNALIGNED LONG*)( p + 1 );
                            OffsetInSection = ( p + 5 ) - SectionStart;
                            OldOriginRva     = SectionBaseRva + OffsetInSection;
                            OldTargetRva     = OldOriginRva + OldDisplacement;

                            //
                            //  We expect a lot of false positives here because
                            //  occurences of <E8> will
                            //  likely occur in other parts of the instruction
                            //  stream so now we validate that the TargetRva
                            //  falls within the image and within an executable
                            //  section.
                            //

                            if ( OldTargetRva < OldFileLastRva ) {

                                TargetOffset = ImageRvaToFileOffset( OldFileNtHeader, OldTargetRva );

                                if ( ! ( OldFileHintMap[ TargetOffset ] & 0x01 )) {

                                    //
                                    //  Looks like a valid TargetRva, so lookup the
                                    //  corresponding "new" RVAs in the rift table.
                                    //

                                    ULONG RiftIndexOrigin = FindRiftTableEntryForOldRva( RiftTable, OldOriginRva );
                                    ULONG RiftIndexTarget = FindRiftTableEntryForOldRva( RiftTable, OldTargetRva );

                                    ULONG NewOriginRva = OldOriginRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexOrigin ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexOrigin ].OldFileRva );
                                    ULONG NewTargetRva = OldTargetRva + (LONG)( RiftTable->RiftEntryArray[ RiftIndexTarget ].NewFileRva - RiftTable->RiftEntryArray[ RiftIndexTarget ].OldFileRva );

                                    PUCHAR NewOriginMa = ImageRvaToMappedAddress( NewFileNtHeader, NewOriginRva, NewFileMapped, NewFileSize );
                                    PUCHAR NewTargetMa = ImageRvaToMappedAddress( NewFileNtHeader, NewTargetRva, NewFileMapped, NewFileSize );

                                    PUCHAR OldTargetMa = ImageRvaToMappedAddress( OldFileNtHeader, OldTargetRva, OldFileMapped, OldFileSize );
                                    PUCHAR OldOriginMa = p + 5;

                                    if (( NewOriginMa ) && ( NewTargetMa ) && ( OldTargetMa )) {

                                        PUCHAR OldInstruct = p;
                                        PUCHAR NewInstruct = NewOriginMa - 5;

                                        //
                                        //  A verified call instruction should match the
                                        //  instruction byte, the byte following the
                                        //  instruction, and the target byte, because
                                        //  they should all be executable code that is
                                        //  not modified by relocs.
                                        //
                                        //  Check NewHintMap too.
                                        //

                                        if (( *OldInstruct == *NewInstruct ) &&
                                            ( *OldOriginMa == *NewOriginMa ) &&
                                            ( *OldTargetMa == *NewTargetMa )) {

                                            ULONG NewInstructOffset = NewInstruct - NewFileMapped;
                                            ULONG NewTargetOffset   = NewTargetMa - NewFileMapped;
                                            BOOL  Skip = FALSE;

                                            for ( j = 0; j < 5; j++ ) {
                                                if ( NewFileHintMap[ NewInstructOffset + j ] & 0x01 ) {
                                                    Skip = TRUE;
                                                    break;
                                                    }
                                                }

                                            if ( NewFileHintMap[ NewTargetOffset + j ] & 0x01 ) {
                                                Skip = TRUE;
                                                }

                                            if ( ! Skip ) {

                                                //
                                                //  This is a bonafide good match.
                                                //

                                                continue;
                                                }
                                            }

                                        {

                                            //
                                            //  Instructions don't match.  Scan to find
                                            //  matching instruction in new file.  Scan
                                            //  the extent of this rift entry.
                                            //

                                            PUCHAR ScanInstruct;
                                            PUCHAR LowestMaToScan;
                                            PUCHAR HighestMaToScan;
                                            ULONG  LowestRvaToScan  = RiftTable->RiftEntryArray[ RiftIndexOrigin ].NewFileRva;
                                            ULONG  HighestRvaToScan = RiftTable->RiftEntryArray[ RiftIndexOrigin + 1 ].NewFileRva;
                                            ULONG  ExpectedTargetRva = NewTargetRva;
                                            BOOL   Found = FALSE;

                                            LowestMaToScan  = ImageRvaToMappedAddress( NewFileNtHeader, LowestRvaToScan,  NewFileMapped, NewFileSize );
                                            HighestMaToScan = ImageRvaToMappedAddress( NewFileNtHeader, HighestRvaToScan, NewFileMapped, NewFileSize );

                                            HighestMaToScan -= 5;   // size of instruction

                                            for ( ScanInstruct = NewInstruct + 1; ScanInstruct <= HighestMaToScan; ScanInstruct++ ) {

                                                if ( *ScanInstruct == 0xE8 ) {

                                                    //
                                                    //  BUGBUG: check NewHintMap
                                                    //

                                                    NewOriginMa     = ScanInstruct + 5;
                                                    NewOriginRva    = MappedAddressToImageRva( NewFileNtHeader, NewOriginMa, NewFileMapped );
                                                    NewDisplacement = *(UNALIGNED LONG*)( ScanInstruct + 1 );
                                                    NewTargetRva    = NewOriginRva + NewDisplacement;
                                                    NewTargetMa     = ImageRvaToMappedAddress( NewFileNtHeader, NewTargetRva, NewFileMapped, NewFileSize );

                                                    if (( NewOriginRva ) && ( NewTargetMa )) {

                                                        //
                                                        //  We already know *OldInstruct == *ScanInstruct
                                                        //

                                                        if (( *OldOriginMa == *NewOriginMa ) &&
                                                            ( *OldTargetMa == *NewTargetMa ) &&
                                                            ( NewTargetRva == ExpectedTargetRva )) {

                                                            ULONG NewInstructOffset = ScanInstruct - NewFileMapped;
                                                            ULONG NewTargetOffset   = NewTargetMa  - NewFileMapped;
                                                            BOOL  Skip = FALSE;

                                                            for ( j = 0; j < 5; j++ ) {
                                                                if ( NewFileHintMap[ NewInstructOffset + j ] & 0x01 ) {
                                                                    Skip = TRUE;
                                                                    break;
                                                                    }
                                                                }

                                                            if ( NewFileHintMap[ NewTargetOffset + j ] & 0x01 ) {
                                                                Skip = TRUE;
                                                                }

                                                            if ( ! Skip ) {

                                                                InsertRiftEntryInSortedTable( RiftTable, RiftIndexOrigin, OldOriginRva - 5, NewOriginRva - 5 );
                                                                InsertRiftEntryInSortedTable( RiftTable, RiftIndexTarget, OldTargetRva, NewTargetRva );
                                                                Found = TRUE;
#ifdef TESTCODE
                                                                CountDiscoveredE8Rifts++;
#endif // TESTCODE
                                                                break;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }

                                            if ( Found ) {
                                                continue;
                                                }

                                            for ( ScanInstruct = NewInstruct - 1; ScanInstruct >= LowestMaToScan; ScanInstruct-- ) {

                                                if ( *ScanInstruct == 0xE8 ) {

                                                    //
                                                    //  BUGBUG: check NewHintMap
                                                    //

                                                    NewOriginMa     = ScanInstruct + 5;
                                                    NewOriginRva    = MappedAddressToImageRva( NewFileNtHeader, NewOriginMa, NewFileMapped );
                                                    NewDisplacement = *(UNALIGNED LONG*)( ScanInstruct + 1 );
                                                    NewTargetRva    = NewOriginRva + NewDisplacement;
                                                    NewTargetMa     = ImageRvaToMappedAddress( NewFileNtHeader, NewTargetRva, NewFileMapped, NewFileSize );

                                                    if (( NewOriginRva ) && ( NewTargetMa )) {

                                                        //
                                                        //  We already know *OldInstruct == *ScanInstruct
                                                        //

                                                        if (( *OldOriginMa == *NewOriginMa ) &&
                                                            ( *OldTargetMa == *NewTargetMa ) &&
                                                            ( NewTargetRva == ExpectedTargetRva )) {

                                                            ULONG NewInstructOffset = ScanInstruct - NewFileMapped;
                                                            ULONG NewTargetOffset   = NewTargetMa  - NewFileMapped;
                                                            BOOL  Skip = FALSE;

                                                            for ( j = 0; j < 5; j++ ) {
                                                                if ( NewFileHintMap[ NewInstructOffset + j ] & 0x01 ) {
                                                                    Skip = TRUE;
                                                                    break;
                                                                    }
                                                                }

                                                            if ( NewFileHintMap[ NewTargetOffset + j ] & 0x01 ) {
                                                                Skip = TRUE;
                                                                }

                                                            if ( ! Skip ) {

                                                                InsertRiftEntryInSortedTable( RiftTable, RiftIndexOrigin, OldOriginRva - 5, NewOriginRva - 5 );
                                                                InsertRiftEntryInSortedTable( RiftTable, RiftIndexTarget, OldTargetRva, NewTargetRva );
                                                                Found = TRUE;
#ifdef TESTCODE
                                                                CountDiscoveredE8Rifts++;
#endif // TESTCODE
                                                                break;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                else if ( OldFileNtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_ALPHA ) {

                    //
                    //  BUGBUG: Implement Alpha platform stuff here.
                    //

                    }
                }
            }
        }

#ifdef TESTCODE
    printf( "\r%9d verified E8 rifts\n", CountVerifiedE8Rifts );
    printf( "\r%9d discovered E8 rifts\n", CountDiscoveredE8Rifts );
#endif // TESTCODE

    if ( OldFileHintMap ) {
        MyVirtualFree( OldFileHintMap );
        }

    if ( NewFileHintMap ) {
        MyVirtualFree( NewFileHintMap );
        }

    return Success;
    }


#endif // 0 (test code)


BOOL
GenerateRiftTable(
    IN HANDLE OldFileHandle,
    IN PUCHAR OldFileMapped,
    IN ULONG  OldFileSize,
    IN ULONG  OldFileOriginalChecksum,
    IN ULONG  OldFileOriginalTimeDate,
    IN HANDLE NewFileHandle,
    IN PUCHAR NewFileMapped,
    IN ULONG  NewFileSize,
    IN ULONG  OptionFlags,
    IN PPATCH_OPTION_DATA OptionData,
    IN ULONG  OldFileIndex,
    IN PRIFT_TABLE RiftTable
    )
    {
    PIMAGE_NT_HEADERS OldFileNtHeader;
    PIMAGE_NT_HEADERS NewFileNtHeader;
    LPCSTR OldFileSymPath;
    LPCSTR NewFileSymPath;
    HANDLE SubAllocator;
    ULONG SymbolOptionFlags;
    BOOL Success = FALSE;

    UNREFERENCED_PARAMETER( OptionFlags );

    SymbolOptionFlags = 0;
    SubAllocator = NULL;

    __try {

        //
        //  See if both files are PE images.
        //

        OldFileNtHeader = GetNtHeader( OldFileMapped, OldFileSize );

        if ( OldFileNtHeader ) {

            NewFileNtHeader = GetNtHeader( NewFileMapped, NewFileSize );

            if ( NewFileNtHeader ) {

                //
                //  Both files are PE images.
                //

                SubAllocator = CreateSubAllocator( 0x100000, 0x100000 );

                if ( ! SubAllocator ) {
                    Success = FALSE;
                    __leave;
                    }

                //
                //  Even without symbols, we can still generate some
                //  basic rift info from PE import and export names, etc.
                //

                Success = GetImageNonSymbolRiftInfo(
                              OldFileMapped,
                              OldFileSize,
                              OldFileNtHeader,
                              NewFileMapped,
                              NewFileSize,
                              NewFileNtHeader,
                              SubAllocator,
                              RiftTable
                              );

                //
                //  Now attempt to get rift info from symbols.
                //

                if (( OptionData ) &&
                    ( OptionData->SizeOfThisStruct >=
                        ( offsetof( PATCH_OPTION_DATA, SymLoadContext ) +
                          sizeof( OptionData->SymLoadContext )))) {

                    SymbolOptionFlags = OptionData->SymbolOptionFlags;

                    if ( ! ( SymbolOptionFlags & PATCH_SYMBOL_NO_IMAGEHLP )) {

                        if ( OptionData->OldFileSymbolPathArray ) {

                            OldFileSymPath = OptionData->OldFileSymbolPathArray[ OldFileIndex ];
                            NewFileSymPath = OptionData->NewFileSymbolPath;

                            if (( OldFileSymPath ) && ( NewFileSymPath )) {

                                //
                                //  tommcg 11/15/2000, before loading imagehlp, attempt to open the
                                //  OldFileSymPath and NewFileSymPath directly as files to see if
                                //  they are really psyminfo files.  If the open fails, or if both
                                //  old and new cannot be opened, fall back to imagehlp.
                                //

                                Success = GetImageSymbolRiftInfoFromPsymFiles(
                                              OldFileHandle,
                                              OldFileMapped,
                                              OldFileSize,
                                              OldFileNtHeader,
                                              OldFileSymPath,
                                              OldFileOriginalChecksum,
                                              OldFileOriginalTimeDate,
                                              OldFileIndex,
                                              NewFileHandle,
                                              NewFileMapped,
                                              NewFileSize,
                                              NewFileNtHeader,
                                              NewFileSymPath,
                                              SymbolOptionFlags,
                                              SubAllocator,
                                              RiftTable,
                                              OptionData->SymLoadCallback,
                                              OptionData->SymLoadContext
                                              );

                                if ( ! Success ) {

                                    Success = GetImageSymbolRiftInfo(
                                                  OldFileHandle,
                                                  OldFileMapped,
                                                  OldFileSize,
                                                  OldFileNtHeader,
                                                  OldFileSymPath,
                                                  OldFileOriginalChecksum,
                                                  OldFileOriginalTimeDate,
                                                  OldFileIndex,
                                                  NewFileHandle,
                                                  NewFileMapped,
                                                  NewFileSize,
                                                  NewFileNtHeader,
                                                  NewFileSymPath,
                                                  SymbolOptionFlags,
                                                  SubAllocator,
                                                  RiftTable,
                                                  OptionData->SymLoadCallback,
                                                  OptionData->SymLoadContext
                                                  );
                                    }

                                if ( SymbolOptionFlags & PATCH_SYMBOL_NO_FAILURES ) {
#ifdef TESTCODE
                                    if (( ! Success ) && ( GetLastError() == ERROR_PATCH_IMAGEHLP_FAILURE )) {
                                        printf( "\rWARNING: Imagehlp.Dll failure\n" );
                                        }
#endif
                                    Success = TRUE;
                                    }
                                }
                            }
                        }
                    }

                if ( Success ) {

                    RiftSortAndRemoveDuplicates(
                        OldFileMapped,
                        OldFileSize,
                        OldFileNtHeader,
                        NewFileMapped,
                        NewFileSize,
                        NewFileNtHeader,
                        RiftTable
                        );

                    //
                    //  Now we can optimize the rift info by peeking into
                    //  the mapped files.
                    //

                    Success = OptimizeImageRiftInfo(
                                  OldFileMapped,
                                  OldFileSize,
                                  OldFileNtHeader,
                                  NewFileMapped,
                                  NewFileSize,
                                  NewFileNtHeader,
                                  SubAllocator,
                                  RiftTable
                                  );
                    }

#ifdef TESTCODE

                if ( Success ) {

                    HANDLE hFile = CreateFile(
                                       "RiftInfo.out",
                                       GENERIC_WRITE,
                                       FILE_SHARE_READ,
                                       NULL,
                                       CREATE_ALWAYS,
                                       FILE_ATTRIBUTE_NORMAL,
                                       NULL
                                       );

                    if ( hFile != INVALID_HANDLE_VALUE ) {

                        CHAR  TextBuffer[ 24 ];
                        DWORD Actual;
                        ULONG i;

                        for ( i = 0; i < RiftTable->RiftEntryCount; i++ ) {
                            sprintf( TextBuffer, "%08X %08X\r\n", RiftTable->RiftEntryArray[ i ].OldFileRva, RiftTable->RiftEntryArray[ i ].NewFileRva );
                            WriteFile( hFile, TextBuffer, 19, &Actual, NULL );
                            }

                        CloseHandle( hFile );
                        }
                    }

#endif // TESTCODE

                }
            }
        }

    __except( EXCEPTION_EXECUTE_HANDLER ) {
        SetLastError( GetExceptionCode() );
        Success = FALSE;
        }

    if ( SubAllocator ) {
        DestroySubAllocator( SubAllocator );
        }

    return Success;
    }



#endif // PATCH_APPLY_CODE_ONLY

