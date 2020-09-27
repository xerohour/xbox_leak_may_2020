
#pragma warning( disable: 4200 )    // zero sized array in structure
#pragma warning( disable: 4201 )    // nameless struct/union
#pragma warning( disable: 4213 )    // cast on l-value
#pragma warning( disable: 4214 )    // bitfields other than int
#pragma warning( disable: 4514 )    // unreferenced inline function

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <xtl.h>
#pragma warning( disable: 4200 4201 4213 4214 4514 )
#include "covfile.h"

unsigned int __cdecl _rotl( unsigned int value, int shift );

#pragma intrinsic( strlen, _rotl )

#pragma optimize( "at", on )    // optimize for speed and assume no aliasing

VOID
PackByteVectorToBitVector(
    IN  DWORD ByteCount,
    IN  PBYTE ByteVector,
    OUT PBYTE BitVector
    )
    {
    DWORD EightByteCount;
    DWORD BitsRemaining;
    DWORD Source1;
    DWORD Source2;
    DWORD PadBits;
    DWORD PackedByte;

    EightByteCount = ByteCount / 8;

    while ( EightByteCount-- ) {

        Source1 = ( *(DWORD*)( ByteVector     )) & 0x01010101;
        Source2 = ( *(DWORD*)( ByteVector + 4 )) & 0x01010101;

        ByteVector += 8;

        *BitVector++ = (BYTE)(( Source1 <<  7 ) | ( Source2 <<  3 ) |
                              ( Source1 >>  2 ) | ( Source2 >>  6 ) |
                              ( Source1 >> 11 ) | ( Source2 >> 24 ) |
                              ( Source1 >> 20 ) | ( Source2 >> 15 ));
        }

    BitsRemaining = ByteCount & 7;

    if ( BitsRemaining ) {

        PadBits    = 8 - BitsRemaining;
        PackedByte = 0;

        while ( BitsRemaining-- ) {
            PackedByte = ( PackedByte << 1 ) | ( *ByteVector++ & 1 );
            }

        *BitVector = (BYTE)( PackedByte << PadBits );

        }
    }

VOID
UnpackBitVectorToByteVector(
    IN  DWORD BitCount,
    IN  PBYTE BitVector,
    OUT PBYTE ByteVector
    )
    {
    DWORD EightBitCount;
    DWORD BitsRemaining;
    DWORD Target1;
    DWORD Target2;
    DWORD PackedByte;

    EightBitCount = BitCount / 8;

    while ( EightBitCount-- ) {

        PackedByte = *BitVector++;

        Target1 = (( PackedByte >>  7 ) |
                   ( PackedByte <<  2 ) |
                   ( PackedByte << 11 ) |
                   ( PackedByte << 20 ));

        Target2 = (( PackedByte >>  3 ) |
                   ( PackedByte <<  6 ) |
                   ( PackedByte << 15 ) |
                   ( PackedByte << 24 ));

        *(DWORD*)( ByteVector )     = Target1 & 0x01010101;
        *(DWORD*)( ByteVector + 4 ) = Target2 & 0x01010101;

        ByteVector += 8;

        }

    BitsRemaining = BitCount & 7;

    if ( BitsRemaining ) {

        PackedByte = *BitVector;

        while ( BitsRemaining-- ) {
            *ByteVector++ = (BYTE)(( PackedByte & 0x80 ) >> 7 );
            PackedByte <<= 1;
            }
        }
    }

VOID
OrBufferWithBuffer(
    IN PVOID TargetBuffer,
    IN PVOID SourceBuffer,
    IN ULONG Size
    )
    {
    ULONG CountLongs = Size / 4;
    ULONG CountBytes = Size & 3;

    while ( CountLongs-- ) {
        *((UNALIGNED ULONG*) TargetBuffer )++ |= *((UNALIGNED ULONG*) SourceBuffer )++;
        }

    while ( CountBytes-- ) {
        *((UNALIGNED UCHAR*) TargetBuffer )++ |= *((UNALIGNED UCHAR*) SourceBuffer )++;
        }
    }

VOID
XorBufferWithBuffer(
    IN PVOID TargetBuffer,
    IN PVOID SourceBuffer,
    IN ULONG Size
    )
    {
    ULONG CountLongs = Size / 4;
    ULONG CountBytes = Size & 3;

    while ( CountLongs-- ) {
        *((UNALIGNED ULONG*) TargetBuffer )++ ^= *((UNALIGNED ULONG*) SourceBuffer )++;
        }

    while ( CountBytes-- ) {
        *((UNALIGNED UCHAR*) TargetBuffer )++ ^= *((UNALIGNED UCHAR*) SourceBuffer )++;
        }
    }

VOID
BufferAndNotBuffer(
    IN PVOID TargetBuffer,
    IN PVOID SourceBuffer,
    IN ULONG Size
    )
    {
    ULONG CountLongs = Size / 4;
    ULONG CountBytes = Size & 3;

    while ( CountLongs-- ) {
        *((UNALIGNED ULONG*) TargetBuffer )++ &= ~( *((UNALIGNED ULONG*) SourceBuffer )++ );
        }

    while ( CountBytes-- ) {
        *((UNALIGNED UCHAR*) TargetBuffer )++ &= ~( *((UNALIGNED UCHAR*) SourceBuffer )++ );
        }
    }

ULONG
__inline
CountBitsSetInValue(
    IN ULONG Value
    )
    {
    ULONG Count = 0;

    while ( Value != 0 ) {
        Count += ( Value & 1 );
        Value >>= 1;
        }

    return Count;
    }

ULONG
CountBitsSetInBuffer(
    IN PVOID Buffer,
    IN ULONG Size
    )
    {
    ULONG CountLongs = Size / 4;
    ULONG CountBytes = Size & 3;
    ULONG CountBits  = 0;

    while ( CountLongs-- ) {
        CountBits += CountBitsSetInValue( *((UNALIGNED ULONG*) Buffer )++ );
        }

    while ( CountBytes-- ) {
        CountBits += CountBitsSetInValue( *((UNALIGNED UCHAR*) Buffer )++ );
        }

    return CountBits;
    }

BOOL
DoesBufferContainAllZeros(
    IN PVOID Buffer,
    IN DWORD Size
    )
    {
    ULONG CountLongs = Size / 4;
    ULONG CountBytes = Size & 3;

    while ( CountLongs-- ) {
        if ( *((UNALIGNED ULONG*) Buffer )++ ) {
            return FALSE;
            }
        }

    while ( CountBytes-- ) {
        if ( *((UNALIGNED UCHAR*) Buffer )++ ) {
            return FALSE;
            }
        }

    return TRUE;
    }

BOOL
DoesBufferContainOnlyHex01s(
    IN PVOID Buffer,
    IN DWORD Size
    )
    {
    ULONG CountLongs = Size / 4;
    ULONG CountBytes = Size & 3;

    while ( CountLongs-- ) {
        if (( *((UNALIGNED ULONG*) Buffer )++ ) & 0xFEFEFEFE ) {
            return FALSE;
            }
        }

    while ( CountBytes-- ) {
        if (( *((UNALIGNED UCHAR*) Buffer )++ ) & 0xFE ) {
            return FALSE;
            }
        }

    return TRUE;
    }

ULONG
HashName(                   // case insensitive
    IN LPCSTR Name
    )
    {
    ULONG Length = strlen( Name );
    ULONG Hash   = ~ Length;

    while ( Length-- )
        Hash = _rotl( Hash, 3 ) ^ ( *Name++ & 0xDF );     // mask case bit

    return Hash;
    }

#pragma optimize( "", on )      // restore to default optimizations

