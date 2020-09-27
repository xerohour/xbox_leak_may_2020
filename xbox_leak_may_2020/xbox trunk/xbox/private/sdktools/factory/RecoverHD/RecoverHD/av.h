/*--

Copyright (c) Microsoft Corporation

Module Name:

    av.h

Abstract:

    This module contains the public data structures and procedure
    prototypes to identify AV packs and program TV encoder

--*/

#ifndef _AV_H
#define _AV_H

#if !defined(_NTSYSTEM_)
#define NTKERNELAVAPI DECLSPEC_IMPORT
#else
#define NTKERNELAVAPI
#endif

//
// AV pack identifiers
//
#define AV_PACK_NONE                      0x00000000

#define AV_PACK_STANDARD                  0x00000001
#define AV_PACK_RFU                       0x00000002
#define AV_PACK_SCART                     0x00000003
#define AV_PACK_HDTV                      0x00000004
#define AV_PACK_VGA                       0x00000005
#define AV_PACK_SVIDEO                    0x00000006
#define AV_PACK_MAX                       0x00000007

#define AV_PACK_MASK                      0x000000FF

//
// AV_ region types
//
#define AV_STANDARD_NTSC_M                0x00000100
#define AV_STANDARD_NTSC_J                0x00000200
#define AV_STANDARD_PAL_I                 0x00000300
#define AV_STANDARD_PAL_M                 0x00000400
#define AV_STANDARD_MAX                   0x00000500

#define AV_STANDARD_MASK                  0x0000FF00
#define AV_STANDARD_BIT_SHIFT             8

// Allows 16:9 for all SDTV and DTV modes.  HDTV 
// modes (720p and 1080i) are assumed to support 16:9.
//
#define AV_FLAGS_WIDESCREEN               0x00010000
#define AV_FLAGS_LETTERBOX                0x00100000

#define AV_ASPECT_RATIO_MASK              (AV_FLAGS_WIDESCREEN | AV_FLAGS_LETTERBOX)

// 480i support is assumed.
#define AV_FLAGS_HDTV_480i                0x00000000
#define AV_FLAGS_HDTV_720p                0x00020000
#define AV_FLAGS_HDTV_1080i               0x00040000
#define AV_FLAGS_HDTV_480p                0x00080000

#define AV_HDTV_MODE_MASK                 (AV_FLAGS_HDTV_480p | AV_FLAGS_HDTV_720p | AV_FLAGS_HDTV_1080i | AV_FLAGS_HDTV_480i)

// Whether the display is interlaced or not.
#define AV_FLAGS_INTERLACED               0x00200000

// Indicates a field-rendered mode.
#define AV_FLAGS_FIELD                    0x01000000

// Indicates that this is in the funky pixel aspect ratio mode.
#define AV_FLAGS_10x11PAR                 0x02000000

// The refresh rate supported by the display.
#define AV_FLAGS_60Hz                     0x00400000
#define AV_FLAGS_50Hz                     0x00800000

#define AV_REFRESH_MASK                   (AV_FLAGS_60Hz | AV_FLAGS_50Hz)

#define AV_USER_FLAGS_MASK                (AV_ASPECT_RATIO_MASK | AV_HDTV_MODE_MASK | AV_FLAGS_60Hz)
#define AV_USER_FLAGS_BIT_SHIFT           16

//
// TV encoder options
//

// !!! Do not change the MACROVISION_MODE value without
//    updating the DVD software!

#define AV_OPTION_MACROVISION_MODE        1
#define AV_OPTION_ENABLE_CC               2
#define AV_OPTION_DISABLE_CC              3
#define AV_OPTION_SEND_CC_DATA            4
#define AV_QUERY_CC_STATUS                5
#define AV_QUERY_AV_CAPABILITIES          6
#define AV_OPTION_BLANK_SCREEN            9
#define AV_OPTION_MACROVISION_COMMIT      10
#define AV_OPTION_FLICKER_FILTER          11
#define AV_OPTION_ZERO_MODE               12
#define AV_OPTION_QUERY_MODE              13
#define AV_OPTION_ENABLE_LUMA_FILTER      14
#define AV_OPTION_GUESS_FIELD             15
#define AV_QUERY_ENCODER_TYPE             16
#define AV_QUERY_MODE_TABLE_VERSION       17

//
// TV encoder types.
//

#define AV_ENCODER_CONEXANT_871           0

//
// Macrovision modes
//

#define AV_MV_OFF                         0
#define AV_MV_AGC_ONLY                    1
#define AV_MV_TWO_STRIPES_PLUS_AGC        2
#define AV_MV_FOUR_STRIPES_PLUS_AGC       3

NTKERNELAVAPI
ULONG
NTAPI
AvSetDisplayMode(
    IN  PVOID RegisterBase,
    IN  ULONG Step,
    IN  ULONG DisplayMode,            // mode from the AV_MODE list (avmode.h)
    IN  ULONG SourceColorFormat,      // D3DFORMAT
    IN  ULONG Pitch,
    IN  ULONG FrameBuffer
    );

NTKERNELAVAPI
VOID
NTAPI
AvSendTVEncoderOption(
    IN  PVOID RegisterBase,
    IN  ULONG Option,
    IN  ULONG Param,
    OUT PULONG Result
    );

NTKERNELAVAPI
PVOID
NTAPI
AvGetSavedDataAddress(
    VOID
    );

NTKERNELAVAPI
VOID
NTAPI
AvSetSavedDataAddress(
    PVOID Address
    );

VOID
NTAPI
AvRelocateSavedDataAddress(
    IN PVOID Address,
    IN SIZE_T NumberOfBytes
    );

NTKERNELAVAPI
ULONG
NTAPI
AvSMCVideoModeToAVPack(
    ULONG VideoMode
    );

#endif // _AV_H
