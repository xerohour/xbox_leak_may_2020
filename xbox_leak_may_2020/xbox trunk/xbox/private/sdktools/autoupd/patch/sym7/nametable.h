
//
//  nametable.h
//
//  Author: Tom McGuire (tommcg)
//
//  Copyright (C) Microsoft, 1997-2000.
//

#ifndef _NAMETABL_H_
#define _NAMETABL_H_

#pragma warning( push )
#pragma warning( disable: 4200 )    // zero-sized array in struct/union

#ifndef ASSERT
#define ASSERT(x)
#endif

#ifdef _M_IX86
#pragma intrinsic( memcpy )
#pragma intrinsic( memcmp )
#pragma intrinsic( strlen )
#pragma intrinsic( _rotl )
#endif

//
//  Each hash table entry is the root of a btree, so it contains a single
//  pointer.  On x86 this is 4 bytes per entry.  On 64-bit systems, this is
//  8 bytes per entry.  Allocating 64K of memory to the hash table sounds
//  reasonable for large datasets.  For 64-bit systems this is 128K.
//
//  NOTE:  Caller must maintain case consistency ("Foo" != "FOO")
//

#define HASH_TABLE_COUNT 16384

struct NAMENODE
{
    NAMENODE* Left;
    NAMENODE* Right;
    unsigned  Hash;
    unsigned  Length;
    char      Name[ 0 ];
};


class NAMETABLE
{

public:

    NAMENODE* __fastcall InsertName( const char *Name );
    NAMENODE* __fastcall LookupName( const char *Name );
    NAMENODE* __fastcall LocateNode( const char *Name, bool Insert );

    NAMETABLE();
   ~NAMETABLE();

private:

    NAMENODE** HashTable;

};


NAMETABLE::NAMETABLE()
{
    HashTable = (NAMENODE**) VirtualAlloc(
                                 NULL,
                                 sizeof(PVOID) * HASH_TABLE_COUNT,
                                 MEM_COMMIT,
                                 PAGE_READWRITE
                                 );
}


NAMETABLE::~NAMETABLE()
{
    if ( HashTable )
    {
        VirtualFree( HashTable, 0, MEM_RELEASE );
    }
}


NAMENODE* __fastcall NAMETABLE::LookupName( const char *Name )
{
    return LocateNode( Name, false );
}


NAMENODE* __fastcall NAMETABLE::InsertName( const char *Name )
{
    return LocateNode( Name, true );
}


NAMENODE* __fastcall NAMETABLE::LocateNode( const char *Name, bool Insert )
{
    ASSERT( HashTable != NULL );

    unsigned Length = strlen( Name );
    unsigned Hash   = ~Length;

    for ( unsigned i = 0; i < Length; i++ )
    {
        Hash = _rotl( Hash, 3 ) ^ (unsigned char) Name[ i ];
    }

    NAMENODE** Link = &HashTable[ Hash % HASH_TABLE_COUNT ];
    NAMENODE*  Node = *Link;

    while ( Node != NULL )
    {
        Link = &Node->Right;

        if ( Hash == Node->Hash )
        {
            if ( Length == Node->Length )
            {
                long Compare = memcmp( Name, Node->Name, Length );

                if ( Compare == 0 )
                {
                    return Node;
                }
                else if ( Compare < 0 )
                {
                    Link = &Node->Left;
                }
            }
            else if ( Length < Node->Length )
            {
                Link = &Node->Left;
            }
        }
        else if ( Hash < Node->Hash )
        {
            Link = &Node->Left;
        }

        Node = *Link;
    }

    if ( Insert )
    {
        Node = (NAMENODE*) new BYTE[ sizeof( NAMENODE ) + Length + 1 ];

        if ( Node != NULL )
        {
            Node->Left   = NULL;
            Node->Right  = NULL;
            Node->Hash   = Hash;
            Node->Length = Length;

            memcpy( Node->Name, Name, Length + 1 );

            *Link = Node;
        }
    }

    return Node;
}


#pragma warning( pop )

#endif // _NAMETABL_H_


