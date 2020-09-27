
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  crc.h       Code for efficiently computing CRC-32 values.              //
//                                                                         //
//              Author: Tom McGuire (tommcg)                               //
//                                                                         //
//              Original version written November, 1992.                   //
//                                                                         //
//              (C) Copyright 1992-1996, Microsoft Corporation             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

extern const unsigned long  CrcTable32[];       // defined in crc.c
extern const unsigned short CrcTable16[];       // defined in crc.c

//
//  Crc32 implementation in 'C' for other platforms.
//

unsigned long __inline Crc32( unsigned long InitialCrc, const void *Buffer, unsigned long Bytes ) {

    unsigned long Crc = InitialCrc;
    const unsigned char *p = Buffer;
    unsigned long Count = Bytes;

    while ( Count-- )
        Crc = ( Crc >> 8 ) ^ CrcTable32[ ((unsigned char)(Crc)) ^ *p++ ];

    return Crc;
    }


unsigned short __inline Crc16( unsigned short InitialCrc, const void *Buffer, unsigned long Bytes ) {

    unsigned short Crc = InitialCrc;
    const unsigned char *p = Buffer;
    unsigned long Count = Bytes;

    while ( Count-- )
        Crc = (unsigned short)(( Crc << 8 ) ^ CrcTable16[ (unsigned char)( Crc >> 8 ) ^ *p++ ] );

    return Crc;
    }



