/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    dvdlib.cpp

Abstract:

    This module contains definition of procudures and data structures used
    by DVD playback library located in hardware key.

--*/

#if __cplusplus
extern "C" {
#endif

#include <xcrypt.h>

extern ULONG_PTR DvdAPIsBegin, DvdAPIsEnd;
extern ULONG_PTR DashAPIsBegin, DashAPIsEnd;

//
// Constants for LZX compression
//

#define LZX_WINDOW_SIZE     (128*1024)
#define LZX_CHUNK_SIZE      (32*1024)
#define LZX_WORKSPACE       (256*1024)

//
// This structure is the header for each compression block
//
typedef struct _LZXBOX_BLOCK
{
    USHORT  CompressedSize;
    USHORT  UncompressedSize;

} LZXBOX_BLOCK, *PLZXBOX_BLOCK;

#pragma pack(1)

typedef struct _IMPORT_API {

    UCHAR      __jmp;
    ULONG_PTR  Relative;
    UCHAR      __int3;

} IMPORT_API, *PIMPORT_API;

#if 0
typedef struct _DVDKEY_HEADER {

    XDCS_DVD_CODE_INFORMATION  xdcs;
    UCHAR   RawData[ ANYSIZE_ARRAY ];
} DVDKEY_HEADER, *PDVDKEY_HEADER;
#endif

#pragma pack()

typedef ULONG_PTR EXPORT_API;
typedef EXPORT_API *PEXPORT_API;

BOOL
WINAPI
DvdKeyInitialize(
    OUT PUCHAR DvdRegion,
    OUT PULONG ExtendedErrorInfo
    );

#if __cplusplus
}
#endif
