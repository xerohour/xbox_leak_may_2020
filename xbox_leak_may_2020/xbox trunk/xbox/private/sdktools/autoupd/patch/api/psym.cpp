
#include <precomp.h>

//
//  psym.cpp
//
//  Author: Tom McGuire (tommcg)
//
//  Copyright (C) Microsoft, 2000-2001.
//

#ifndef PATCH_APPLY_CODE_ONLY

#define TBUFSIZE 0x10000                // sizeof ScratchBuffer allocation

#define NAME_HASH_TABLE_COUNT 0x2000    // 8192 x sizeof(PVOID) == 32K or 64K
#define SYM_HASH_TABLE_COUNT  0x2000    // 8192 x sizeof(PVOID) == 32K or 64K

typedef const unsigned char *PCBYTE;


struct NAMENODE
{
    NAMENODE* Next;
    ULONG     Hash;                     // high 16 bits contains name length
    CHAR      Name[ 0 ];                // variable length, null terminated
};


struct FRAGMENT
{
    FRAGMENT* Next;
    ULONG     SymOffset;
    ULONG     FragRva;
};


struct SYMNODE
{
    SYMNODE*  Next;
    NAMENODE* NameId;
    FRAGMENT  BaseFrag;

#ifdef DEBUGCODE
    ULONG     MatchCount;
#endif

};


class NAMETABLE
{

public:

    NAMENODE* __fastcall InsertName( LPCSTR Name );
    NAMENODE* __fastcall LookupName( LPCSTR Name );
    NAMENODE* __fastcall LocateNode( LPCSTR Name, BOOL Insert );

    NAMETABLE( HANDLE hSubAllocator );

//private:

    HANDLE      m_SubAllocator;
    NAMENODE**  m_NameHashTable;

};


NAMETABLE::NAMETABLE( HANDLE hSubAllocator )
{
    m_SubAllocator  = hSubAllocator;
    m_NameHashTable = (NAMENODE**) SubAllocate(
                                       hSubAllocator,
                                       NAME_HASH_TABLE_COUNT * sizeof( PVOID )
                                       );
}


NAMENODE* __fastcall NAMETABLE::LookupName( LPCSTR Name )
{
    return LocateNode( Name, false );
}


NAMENODE* __fastcall NAMETABLE::InsertName( LPCSTR Name )
{
    return LocateNode( Name, true );
}


NAMENODE* __fastcall NAMETABLE::LocateNode( LPCSTR Name, BOOL Insert )
{
    ASSERT( m_NameHashTable != NULL );

    ULONG Length = 0;
    ULONG Hash   = (ULONG) -1;

    while ( Name[ Length ] != 0 )
    {
        Hash = _rotl( Hash, 3 ) ^ *(UNALIGNED WORD *)&Name[ Length++ ];
    }

    ASSERT( Length < 0x10000 );

    Hash = (( Hash ^ ( Hash >> 16 )) & 0xFFFF ) | ( Length << 16 );

    ASSERT(( Hash >> 16 ) == Length );

    NAMENODE** Link = &m_NameHashTable[ Hash % NAME_HASH_TABLE_COUNT ];
    NAMENODE*  Node = *Link;

    while ( Node != NULL )
    {
        Link = &Node->Next;

        if ( Hash == Node->Hash )
        {
            if ( memcmp( Name, Node->Name, Length ) == 0 )
            {
                return Node;
            }
        }

        Node = *Link;
    }

    if ( Insert )
    {
        Node = (NAMENODE*) SubAllocate( m_SubAllocator, sizeof( NAMENODE ) + Length + 1 );

        if ( Node != NULL )
        {
            Node->Hash = Hash;

#ifdef DONTCOMPILE  // not necessary since SubAllocate returns zeroed memory

            Node->Next = NULL;

            memcpy( Node->Name, Name, Length + 1 );

#endif

            memcpy( Node->Name, Name, Length );

            *Link = Node;
        }
    }

    return Node;
}


class SYMTABLE
{

public:

    SYMNODE* __fastcall InsertSym( NAMENODE* NameId );
    SYMNODE* __fastcall InsertSym( LPCSTR Name );
    SYMNODE* __fastcall LookupSym( NAMENODE* NameId );
    SYMNODE* __fastcall LookupSym( LPCSTR Name );
    SYMNODE* __fastcall LocateNode( NAMENODE* NameId, BOOL Insert );

    SYMTABLE( NAMETABLE* NameTable, HANDLE SubAllocator );

//private:

    HANDLE      m_SubAllocator;
    SYMNODE**   m_SymHashTable;
    NAMETABLE*  m_NameTable;

};


SYMTABLE::SYMTABLE( NAMETABLE* NameTable, HANDLE hSubAllocator )
{
    m_NameTable    = NameTable;
    m_SubAllocator = hSubAllocator;

    m_SymHashTable = (SYMNODE**) SubAllocate(
                                     hSubAllocator,
                                     SYM_HASH_TABLE_COUNT * sizeof( PVOID )
                                     );
}


SYMNODE* __fastcall SYMTABLE::LookupSym( NAMENODE* NameId )
{
    return LocateNode( NameId, false );
}


SYMNODE* __fastcall SYMTABLE::LookupSym( LPCSTR Name )
{
    NAMENODE* NameNode = m_NameTable->LookupName( Name );

    if ( NameNode != NULL )
    {
        return LocateNode( NameNode, false );
    }

    return FALSE;
}


SYMNODE* __fastcall SYMTABLE::InsertSym( NAMENODE* NameId )
{
    return LocateNode( NameId, true );
}


SYMNODE* __fastcall SYMTABLE::InsertSym( LPCSTR Name )
{
    NAMENODE* NameNode = m_NameTable->InsertName( Name );

    if ( NameNode != NULL )
    {
        return LocateNode( NameNode, true );
    }

    return FALSE;
}


SYMNODE* __fastcall SYMTABLE::LocateNode( NAMENODE* NameId, BOOL Insert )
{
    ASSERT( m_SymHashTable != NULL );

    SYMNODE** Link = &m_SymHashTable[ ((ULONG)NameId >> 2 ) % SYM_HASH_TABLE_COUNT ];
    SYMNODE*  Node = *Link;

    while ( Node != NULL )
    {
        Link = &Node->Next;

        if ( NameId == Node->NameId )
        {
            return Node;
        }

        Node = *Link;
    }

    if ( Insert )
    {
        Node = (SYMNODE*) SubAllocate( m_SubAllocator, sizeof( SYMNODE ));

        if ( Node != NULL )
        {
            Node->NameId = NameId;

#ifdef DONTCOMPILE  // not necessary since SubAllocate returns zeroed memory

            Node->Next               = NULL;
            Node->BaseFrag.Next      = NULL;
            Node->BaseFrag.SymOffset = 0;
            Node->BaseFrag.FragRva   = 0;

            DEBUGCODE( Node->MatchCount = 0 );
#endif

            *Link = Node;
        }
    }

    return Node;
}


#ifdef _M_IX86

//
//  x86 compiler doesn't have intrinsic memchr, so we'll do our own.  Note that
//  "repne scasb" is slower (2X) than simply comparing one byte at a time on
//  Pentium II and III.
//

PCBYTE
__inline
__fastcall
ScanForChar(
    IN PCBYTE Buffer,
    IN ULONG  BufLength,
    IN CHAR   SearchFor
    )
{
    PCBYTE p;
    ULONG  n;

    for ( p = Buffer, n = BufLength; n > 0; p++, n-- )
    {
        if ( *p == SearchFor )
        {
            return p;
        }
    }

    return NULL;
}

#else   // ! _M_IX86

PCBYTE
__inline
__fastcall
ScanForChar(
    IN PCBYTE Buffer,
    IN ULONG  BufLength,
    IN CHAR   SearchFor
    )
{
    return memchr( Buffer, SearchFor, BufLength );
}

#endif  // ! _M_IX86


int
__inline
__fastcall
MySimpleCompareCaseInsensitive(
    IN PCBYTE One,
    IN PCBYTE Two,
    IN ULONG  Len
    )
{
    while ( Len-- )
    {
        int Diff = ( *One | 0x20 ) - ( *Two | 0x20 );

        if ( Diff != 0 )
        {
            return Diff;
        }

        One++;
        Two++;
    }

    return 0;
}



DWORDLONG
__fastcall
HexStrToUnsigned64(
    IN LPCSTR HexStr
    )
{
    LPCSTR    p = HexStr;
    DWORDLONG Value = 0;
    DWORDLONG NewValue;
    ULONG     Digit;

    for ( ;; )
    {
        Digit = ( *p++ ) - 0x30;

        NewValue = Value << 4;

        if ( Digit > 9 )
        {
            Digit = ( Digit & 0x1F ) - 7;

            if ( ( Digit > 15 ) || ( Digit < 10 ) )
            {
                return Value;
            }
        }

        Value = NewValue | Digit;
    }
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

    for ( ;; )
    {
        Digit = ( *p++ ) - 0x30;

        NewValue = Value << 4;

        if ( Digit > 9 )
        {
            Digit = ( Digit & 0x1F ) - 7;

            if ( ( Digit > 15 ) || ( Digit < 10 ) )
            {
                return Value;
            }
        }

        Value = NewValue | Digit;
    }
}


BOOL __inline __fastcall IsWhiteSpace( BYTE b )
{
    return(( b == ' ' ) || ( b == '\t' ) || ( b == '\r' ));
}


PCBYTE
__fastcall
GetNextIniValue(
    PCBYTE IniLine,
    PCBYTE EndOfFile,
    PBYTE  NameBuffer,
    ULONG  NameBufSize,
    PBYTE  ValueBuffer,
    ULONG  ValueBufSize
    )
{
    if (( IniLine != NULL ) && ( IniLine < EndOfFile ))
    {
        PBYTE pNameOut  = NameBuffer;
        PBYTE pNameEnd  = NameBuffer + NameBufSize - 1;
        PBYTE pValueOut = ValueBuffer;
        PBYTE pValueEnd = ValueBuffer + ValueBufSize - 1;

        BOOL BeforeEquals = TRUE;
        BOOL IgnoreSpaces = TRUE;
        BOOL InsideQuotes = FALSE;
        BOOL AfterComment = FALSE;

        PCBYTE p;

        for ( p = IniLine; ( p < EndOfFile ) && ( *p != '\n' ) && ( *p != 0 ); p++ )
        {
            if ( ! AfterComment )
            {
                if ( *p == '\"' )
                {         // start or end of quoted string
                    InsideQuotes = ( ! InsideQuotes );
                    IgnoreSpaces = ( ! InsideQuotes );
                    continue;
                }

                if ( ! InsideQuotes )
                {
                    if ( ( *p == '=' ) && ( BeforeEquals ) )
                    {
                        BeforeEquals = FALSE;
                        IgnoreSpaces = TRUE;
                        continue;
                    }

                    if ( *p == ';' )
                    {      // rest of line is comment
                        AfterComment = TRUE;
                        continue;
                    }
                }

                if ( ( ! IgnoreSpaces ) || ( ! IsWhiteSpace( *p )) )
                {
                    IgnoreSpaces = FALSE;

                    if ( BeforeEquals )
                    {
                        if ( pNameOut < pNameEnd )
                        {
                            *pNameOut++ = *p;
                        }
                    }
                    else
                    {
                        if ( pValueOut < pValueEnd )
                        {
                            *pValueOut++ = *p;
                        }
                    }
                }
            }
        }

        while ( ( pNameOut > NameBuffer ) && ( IsWhiteSpace( *( pNameOut - 1 ))) )
        {
            --pNameOut;
        }

        if ( pNameOut <= pNameEnd )
        {
            *pNameOut = 0;
        }

        while ( ( pValueOut > ValueBuffer ) && ( IsWhiteSpace( *( pValueOut - 1 ))) )
        {
            --pValueOut;
        }

        if ( pValueOut <= pValueEnd )
        {
            *pValueOut = 0;
        }

        return ( p < EndOfFile ) ? ( p + 1 ) : EndOfFile;
    }

    return NULL;
}


PCBYTE
__fastcall
MyFindIniSection(
    LPCSTR SectionName,
    PCBYTE IniFile,
    ULONG  IniFileSize
    )
{
    PCBYTE p = IniFile;
    ULONG  n = strlen( SectionName );
    PCBYTE z = IniFile + IniFileSize - ( n + 2 );

    while ( p < z )
    {
        p = ScanForChar( p, z - p, '[' );

        if ( p == NULL )
        {
            break;
        }

        if ( ( p == IniFile ) || ( *( p - 1 ) == '\n' ) )
        {
            ++p;

            if ( *( p + n ) == ']' )
            {
                if ( MySimpleCompareCaseInsensitive( p, (PCBYTE)SectionName, n ) == 0 )
                {
                    p += n + 1;

                    z = ScanForChar( p, z - p, '\n' );

                    if ( z != NULL )
                    {
                        p = z + 1;
                    }

                    return p;
                }
            }
        }

        ++p;
    }

    return NULL;
}


PCBYTE
__fastcall
MyFindIniValue(
    PCBYTE IniSection,
    PCBYTE IniFileOrSectionEnd,
    LPCSTR ValueName,
    PBYTE  ValueBuffer,
    ULONG  ValueBufSize
    )
{
    BYTE   FieldName[ 256 ];
    PCBYTE NextLine = IniSection;
    PCBYTE ThisLine;
    ULONG  ValueNameLength = strlen( ValueName );

    for ( ;; )
    {
        ThisLine = NextLine;            // start of this line

        NextLine = GetNextIniValue( ThisLine, IniFileOrSectionEnd, FieldName, sizeof( FieldName ), ValueBuffer, ValueBufSize );

        if ( NextLine == NULL )         // end of file
        {
            break;
        }

        if ( *FieldName == '[' )        // end of section (start of next)
        {
            break;
        }

        if ( MySimpleCompareCaseInsensitive( FieldName, (PCBYTE)ValueName, ValueNameLength ) == 0 )
        {
            return ThisLine;            // line containing found name/value
        }
    }

    return NULL;
}


PCBYTE
__fastcall
GetNextPsymSymbol(
    PCBYTE IniLine,
    PCBYTE EndOfFile,
    ULONG* SymbolRva,
    ULONG* SymbolOffset,
    PBYTE  NameBuffer,
    ULONG  NameBufSize
    )
{
    BYTE   RvaTextBuffer[ 32 ];
    PCBYTE NextLine = IniLine;
    ULONG  RvaValue;
    ULONG  Offset = 0;
    PBYTE  p;

    for ( ;; )
    {
        NextLine = GetNextIniValue(
                                  NextLine,
                                  EndOfFile,
                                  RvaTextBuffer,
                                  sizeof( RvaTextBuffer ),
                                  NameBuffer,
                                  NameBufSize
                                  );

        if ( ( NextLine == NULL ) || ( *RvaTextBuffer == '[' ) )
        {
            //
            //  End of file, or start of next section
            //

            return NULL;
        }

        if (( *RvaTextBuffer == 0 ) || ( *NameBuffer == 0 ))
        {
            //
            //  Blank line or not a valid Rva/Name pair.
            //

            continue;
        }

        //
        //  We want to ignore symbols starting with "PE\" because we have
        //  our own non-symbol rift generation code for PE RVA fields.
        //

        if ( ( *(DWORD*)( NameBuffer ) & 0x00FFFFFF ) == 0x005C4550 )
        {
            continue;
        }

        RvaValue = HexStrToUnsigned32( (LPCSTR) RvaTextBuffer );

        break;
    }

    //
    //  Look for "\$BB+123" in the symbol name to determine if this
    //  symbol is a BBT fragment.
    //

    for ( p = NameBuffer; *p != 0; p++ )
    {
        if ( ( *p == '\\' ) && ( *(UNALIGNED DWORD*)( p + 1 ) == '+BB$' ) )
        {
            *p = 0;             // terminate at parent symbol name

            p += 5;             // point at decimal offset

            while ( ( *p >= '0' ) && ( *p <= '9' ) )
            {
                Offset = ( Offset * 10 ) + ( *p++ - '0' );
            }

            break;
        }
    }

    *SymbolOffset = Offset;
    *SymbolRva    = RvaValue;

    return NextLine;
}


#ifdef DONTCOMPILE  // BUGBUG: this should be removed

void
__fastcall
SwapFragArrayEntries(
    FRAGMENT* One,
    FRAGMENT* Two
    )
{
    ULONG Temp1    = One->SymOffset;
    ULONG Temp2    = One->FragRva;

    One->SymOffset = Two->SymOffset;
    One->FragRva   = Two->FragRva;

    Two->SymOffset = Temp1;
    Two->FragRva   = Temp2;
}


VOID
__fastcall
QsortFragArray(
    FRAGMENT* LowerBound,
    FRAGMENT* UpperBound
    )
{
    FRAGMENT* Lower = LowerBound;
    FRAGMENT* Upper = UpperBound;
    FRAGMENT* Pivot = Lower + (( Upper - Lower ) / 2 );
    ULONG     Value = Pivot->SymOffset;

    do
    {
        while (( Lower <= Upper ) && ( Lower->SymOffset <= Value ))
        {
            ++Lower;
        }

        while (( Upper >= Lower ) && ( Upper->SymOffset >= Value ))
        {
            --Upper;
        }

        if ( Lower < Upper )
        {
            SwapFragArrayEntries( Lower++, Upper-- );
        }
    }
    while ( Lower <= Upper );

    if ( Lower < Pivot )
    {
        SwapFragArrayEntries( Lower, Pivot );
        Pivot = Lower;
    }
    else if ( Upper > Pivot )
    {
        SwapFragArrayEntries( Upper, Pivot );
        Pivot = Upper;
    }

    if ( LowerBound < ( Pivot - 1 ))
    {
        QsortFragArray( LowerBound, Pivot - 1 );
    }

    if (( Pivot + 1 ) < UpperBound )
    {
        QsortFragArray( Pivot + 1, UpperBound );
    }
}


#endif


#ifdef TESTCODE

VOID
DumpUnMatchedSymNodes(
    SYMTABLE* SymTable,
    LPCSTR    DumpFileName
    )
{
    CHAR TextBuffer[ 4000 ];

    HANDLE hFile = CreateFile(
                       DumpFileName,
                       GENERIC_WRITE,
                       FILE_SHARE_READ,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL
                       );

    if ( hFile != INVALID_HANDLE_VALUE )
    {
        for ( unsigned Slot = 0; Slot < SYM_HASH_TABLE_COUNT; Slot++ )
        {
            SYMNODE* SymNode = SymTable->m_SymHashTable[ Slot ];

            while ( SymNode != NULL )
            {
                if ( SymNode->MatchCount == 0 )
                {
                    DWORD Actual = sprintf( TextBuffer, "%06X %s\r\n", SymNode->BaseFrag.FragRva, SymNode->NameId->Name );
                    WriteFile( hFile, TextBuffer, Actual, &Actual, NULL );
                }

                SymNode = SymNode->Next;
            }
        }

        CloseHandle( hFile );
    }
}

#endif /* TESTCODE */


extern "C"
BOOL
GetImageSymbolRiftInfoFromPsymFiles(
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
    HANDLE OldFilePsymHandle  = NULL;
    PCBYTE OldFilePsymMapped  = NULL;
    HANDLE NewFilePsymHandle  = NULL;
    PCBYTE NewFilePsymMapped  = NULL;
    PBYTE  ScratchBuffer      = NULL;
    BOOL   CompleteSuccess    = FALSE;

    UNREFERENCED_PARAMETER( OldFileHandle );
    UNREFERENCED_PARAMETER( OldFileMapped );
    UNREFERENCED_PARAMETER( OldFileSize );
    UNREFERENCED_PARAMETER( OldFileNtHeader );

    UNREFERENCED_PARAMETER( NewFileHandle );
    UNREFERENCED_PARAMETER( NewFileMapped );
    UNREFERENCED_PARAMETER( NewFileSize );

    UNREFERENCED_PARAMETER( SymbolOptionFlags );

    __try {

        PCBYTE OldPsymInfoSection;
        PCBYTE OldPsymSymSection;
        PCBYTE NewPsymInfoSection;
        PCBYTE NewPsymSymSection;
        ULONG  OldFilePsymSize;
        ULONG  NewFilePsymSize;
        ULONG  Value32;
        BOOL   Success;
        PCBYTE p;
        PCBYTE z;

        ScratchBuffer = (PBYTE) VirtualAlloc( NULL, TBUFSIZE, MEM_COMMIT, PAGE_READWRITE );

        if ( ScratchBuffer == NULL )
        {
            __leave;
        }

        Success = MyMapViewOfFileA(
                      OldFileSymPath,
                      &OldFilePsymSize,
                      &OldFilePsymHandle,
                      (PVOID*)&OldFilePsymMapped
                      );

        if ( ! Success )
        {
            __leave;
        }

        OldPsymInfoSection = MyFindIniSection( "PsymInfo", OldFilePsymMapped, 4096 );

        if ( OldPsymInfoSection == NULL )
        {
            __leave;
        }

        z = OldFilePsymMapped + OldFilePsymSize;

        MyFindIniValue( OldPsymInfoSection, z, "PsymVer", ScratchBuffer, TBUFSIZE );

        if ( ( ScratchBuffer[ 0 ] != '1' ) || ( ScratchBuffer[ 1 ] != '.' ) )
        {
            __leave;
        }

        MyFindIniValue( OldPsymInfoSection, z, "ExeTime", ScratchBuffer, TBUFSIZE );

        if ( ( Value32 = HexStrToUnsigned32( (LPCSTR) ScratchBuffer )) != 0 )
        {
            if ( Value32 != OldFileOriginalTimeDate )
            {
                __leave;
            }
        }

        OldPsymSymSection = MyFindIniSection( "Symbols", OldFilePsymMapped, OldFilePsymSize );

        if ( OldPsymSymSection == NULL )
        {
            __leave;
        }

        Success = MyMapViewOfFileA(
                      NewFileSymPath,
                      &NewFilePsymSize,
                      &NewFilePsymHandle,
                      (PVOID*)&NewFilePsymMapped
                      );

        if ( ! Success )
        {
            __leave;
        }

        NewPsymInfoSection = MyFindIniSection( "PsymInfo", NewFilePsymMapped, 4096 );

        if ( NewPsymInfoSection == NULL )
        {
            __leave;
        }

        z = NewFilePsymMapped + NewFilePsymSize;

        MyFindIniValue( NewPsymInfoSection, z, "PsymVer", ScratchBuffer, TBUFSIZE );

        if ( ( ScratchBuffer[ 0 ] != '1' ) || ( ScratchBuffer[ 1 ] != '.' ) )
        {
            __leave;
        }

        MyFindIniValue( NewPsymInfoSection, z, "ExeTime", ScratchBuffer, TBUFSIZE );

        if ( ( Value32 = HexStrToUnsigned32( (LPCSTR) ScratchBuffer )) != 0 )
        {
            if ( Value32 != NewFileNtHeader->FileHeader.TimeDateStamp )
            {
                __leave;
            }
        }

        NewPsymSymSection = MyFindIniSection( "Symbols", NewFilePsymMapped, NewFilePsymSize );

        if ( NewPsymSymSection == NULL )
        {
            __leave;
        }

        //
        //  Now we have both old and new psym files mapped and verified.
        //

        if ( SymLoadCallback )
        {
            Success = SymLoadCallback(
                         OldFileIndex + 1,         // 1 for first old file, etc
                         OldFileSymPath,
                         0x1001,                   // fake imagehlp SymType
                         OldFileOriginalChecksum,
                         OldFileOriginalTimeDate,
                         OldFileOriginalChecksum,
                         OldFileOriginalTimeDate,
                         SymLoadContext
                         );

            if ( ! Success )
            {
                __leave;
            }

            Success = SymLoadCallback(
                         0,                        // 0 for new file
                         NewFileSymPath,
                         0x1001,                   // fake imagehlp SymType
                         NewFileNtHeader->OptionalHeader.CheckSum,
                         NewFileNtHeader->FileHeader.TimeDateStamp,
                         NewFileNtHeader->OptionalHeader.CheckSum,
                         NewFileNtHeader->FileHeader.TimeDateStamp,
                         SymLoadContext
                         );

            if ( ! Success )
            {
                __leave;
            }
        }

        //
        //  Psym files approved by caller's SymLoadCallback routine.
        //

        NAMETABLE SymNameTable( SubAllocator );
        SYMTABLE  OldSymTable( &SymNameTable, SubAllocator );
        SYMTABLE  NewSymTable( &SymNameTable, SubAllocator );

        z = NewFilePsymMapped + NewFilePsymSize;
        p = NewPsymSymSection;

        for ( ;; )
        {
            ULONG SymRva;
            ULONG SymOffset;

            p = GetNextPsymSymbol( p, z, &SymRva, &SymOffset, ScratchBuffer, TBUFSIZE );

            if ( p == NULL )
            {
                break;
            }

            SYMNODE* SymNode = NewSymTable.InsertSym( (LPCSTR) ScratchBuffer );

            if ( SymNode )
            {
                if (( SymOffset == 0 ) && ( SymNode->BaseFrag.FragRva == 0 ))
                {
                    SymNode->BaseFrag.FragRva = SymRva;
                }
                else                    // fragment
                {
                    FRAGMENT* Frag = (FRAGMENT*) SubAllocate( NewSymTable.m_SubAllocator, sizeof( FRAGMENT ));

                    if ( Frag )
                    {
                        Frag->SymOffset = SymOffset;
                        Frag->FragRva   = SymRva;

                        FRAGMENT** Link = &SymNode->BaseFrag.Next;
                        FRAGMENT*  Next = *Link;

                        while (( Next ) && ( Next->SymOffset < SymOffset ))
                        {
                            Link = &Next->Next;
                            Next = *Link;
                        }

                        Frag->Next = Next;
                        *Link = Frag;
                    }
                }
            }
        }

        //
        //  Now load the old symbols the same way.
        //

        z = OldFilePsymMapped + OldFilePsymSize;
        p = OldPsymSymSection;

        for ( ;; )
        {
            ULONG SymRva;
            ULONG SymOffset;

            p = GetNextPsymSymbol( p, z, &SymRva, &SymOffset, ScratchBuffer, TBUFSIZE );

            if ( p == NULL )
            {
                break;
            }

            SYMNODE* SymNode = OldSymTable.InsertSym( (LPCSTR) ScratchBuffer );

            if ( SymNode )
            {
                if (( SymOffset == 0 ) && ( SymNode->BaseFrag.FragRva == 0 ))
                {
                    SymNode->BaseFrag.FragRva = SymRva;
                }
                else                    // fragment
                {
                    FRAGMENT* Frag = (FRAGMENT*) SubAllocate( OldSymTable.m_SubAllocator, sizeof( FRAGMENT ));

                    if ( Frag )
                    {
                        Frag->SymOffset = SymOffset;
                        Frag->FragRva   = SymRva;

                        FRAGMENT** Link = &SymNode->BaseFrag.Next;
                        FRAGMENT*  Next = *Link;

                        while (( Next ) && ( Next->SymOffset < SymOffset ))
                        {
                            Link = &Next->Next;
                            Next = *Link;
                        }

                        Frag->Next = Next;
                        *Link = Frag;
                    }
                }
            }
        }

        //
        //  Now walk old symbols, find matching new symbol, and create
        //  rift entries.
        //

        for ( unsigned Slot = 0; Slot < SYM_HASH_TABLE_COUNT; Slot++ )
        {
            SYMNODE* OldSymNode = OldSymTable.m_SymHashTable[ Slot ];

            while ( OldSymNode != NULL )
            {
                SYMNODE* NewSymNode = NewSymTable.LookupSym( OldSymNode->NameId );

                if ( NewSymNode != NULL )
                {
                    DEBUGCODE( NewSymNode->MatchCount++ );
                    DEBUGCODE( OldSymNode->MatchCount++ );

                    FRAGMENT* OldFrag = &OldSymNode->BaseFrag;
                    FRAGMENT* NewFrag = &NewSymNode->BaseFrag;

                    do
                    {
                        ULONG OldRva = OldFrag->FragRva;
                        ULONG NewRva = NewFrag->FragRva;

                        if ( OldFrag->SymOffset <= NewFrag->SymOffset )
                        {
                            OldRva += ( NewFrag->SymOffset - OldFrag->SymOffset );
                        }
                        else
                        {
                            NewRva += ( OldFrag->SymOffset - NewFrag->SymOffset );
                        }

                        AddRiftEntryToTable( RiftTable, OldRva, NewRva );

                        ULONG OldNextFragOffset = OldFrag->Next ? OldFrag->Next->SymOffset : 0xFFFFFFFF;
                        ULONG NewNextFragOffset = NewFrag->Next ? NewFrag->Next->SymOffset : 0xFFFFFFFF;

                        if ( OldNextFragOffset <= NewNextFragOffset )
                        {
                            OldFrag = OldFrag->Next;
                        }

                        if ( NewNextFragOffset <= OldNextFragOffset )
                        {
                            NewFrag = NewFrag->Next;
                        }

                        ASSERT((( OldFrag != NULL ) && ( NewFrag != NULL )) ||
                               (( OldFrag == NULL ) && ( NewFrag == NULL )));
                    }
                    while (( OldFrag ) && ( NewFrag ));

                }

                OldSymNode = OldSymNode->Next;
            }
        }

#ifdef TESTCODE

        DumpUnMatchedSymNodes( &OldSymTable, "UnmatchedOldSymbols.out" );
        DumpUnMatchedSymNodes( &NewSymTable, "UnmatchedNewSymbols.out" );

#endif // TESTCODE

        CompleteSuccess = TRUE;
    }

    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        DEBUGCODE( printf( "\nException %08X processing psym files\n", GetExceptionCode() ));
    }

    if ( ScratchBuffer )
    {
        VirtualFree( ScratchBuffer, 0, MEM_RELEASE );
    }

    if ( OldFilePsymMapped )
    {
        UnmapViewOfFile( OldFilePsymMapped );
    }

    if ( NewFilePsymMapped )
    {
        UnmapViewOfFile( NewFilePsymMapped );
    }

    if ( OldFilePsymHandle )
    {
        CloseHandle( OldFilePsymHandle );
    }

    if ( NewFilePsymHandle )
    {
        CloseHandle( NewFilePsymHandle );
    }

    return CompleteSuccess;
}


#endif /* ! PATCH_APPLY_CODE_ONLY */

