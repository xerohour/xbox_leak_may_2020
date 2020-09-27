/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    bldr.h

Abstract:

    Header file for Xbox boot loader

--*/

#ifndef _BLDR_H
#define _BLDR_H

//
// size of romdec code
//
#define ROM_DEC_SIZE                0x200

//
// define the starting physical address where the ROM decrypts the boot loader
// must match BLDR_BOOT_ORIGIN in bldr.inc
//
#define BLDR_BOOT_ORIGIN            0x00090000

//
// define the starting physical address where the boot loader relocates itself
// must match BLDR_RELOCATED_ORIGIN in bldr.inc
//
#define BLDR_RELOCATED_ORIGIN       0x00400000

//
// size of the boot loader.  This is a fixed size that the RomDec code expects to decrypt
// must match ROMLDR_SIZE in bldr.inc
//
#define BLDR_BLOCK_SIZE             (24*1024)

//
// size of the preloader.  This is a fixed size that the romdec code expects to
// compute the hash of the preloader
//
#define PRELDR_BLOCK_SIZE           (21 * 512)

//
// location of the romdec pieces
//
#define ROMDEC_HASH                 88
#define ROMDEC_N                    100

//
// a signature that is checked by the romdec code
// must match ROMLDR_SIGNATURE in bldr.inc
//
#define BLDR_SIGNATURE              0x7854794A

//
// same as XC_DIGEST_LEN
//
#define XBOOT_DIGEST_LEN            20

typedef struct _XBOOT_PARAM
{
    ULONG       UncompressedKernelDataSize;
    ULONG       InitTableSize;

    //
    // The Signature field must be at the following byte offset.  Add new fields
    // to the front of this structure: the XBOOT_PARAM structure is placed at
    // the end of the boot loader image.
    //

    ULONG       Signature;
    ULONG       CompressedKernelSize;
    UCHAR       MainRomDigest[XBOOT_DIGEST_LEN];       // digest of main rom (kernel)
} XBOOT_PARAM, *PXBOOT_PARAM;

//
// constants for lzx compression
//
#define LZX_WINDOW_SIZE             (128*1024)
#define LZX_CHUNK_SIZE              (32*1024)
#define LZX_WORKSPACE               (256*1024)

//
// this structure is the header for each compression block
//
typedef struct _LZXBOX_BLOCK
{
    USHORT      CompressedSize;
    USHORT      UncompressedSize;

} LZXBOX_BLOCK, *PLZXBOX_BLOCK;

//
// this structures lives at PsNtosImageBase.IMAGE_DOS_HEADER.e_res2 (must be
// less than 10 USHORTs)
//
typedef struct _XDATA_SECTION_HEADER
{
    ULONG       SizeOfUninitializedData;
    ULONG       SizeOfInitializedData;
    ULONG       PointerToRawData;
    ULONG       VirtualAddress;
} XDATA_SECTION_HEADER, *PXDATA_SECTION_HEADER;

//
// Define a routine to reenter the boot loader in order to boot an alternate
// ROM at the supplied base physical address.
//

typedef
VOID
(__fastcall *PBLDR_LOAD_MEDIA_ROM_ROUTINE)(
    ULONG_PTR MediaROMBaseAddress
    );



#endif // _BLDR_H
