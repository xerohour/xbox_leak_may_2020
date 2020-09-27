////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//
// the follow file defines the VIDEO_PARAMETER structure and calling parameters
// change to the ChangeDisplaySettingEx() Win32 API.
//
// The audience for this file are Win32 developers who want to call
// ChangeDisplaySettingEx with the CDS_VIDEOPARAMETERS flag set, and
// display driver developers who want to implement the VIDEO_PARAMETERS
// escape in their Control() function.

// Display driver Escape value to get/set the parameters.
//
#define VIDEO_PARAMETERS        3077    // escape value used in Control()

// Flags for ChangeDisplaySettings
// these must match the flags in WINUSER.H

#define CDS_UPDATEREGISTRY  0x00000001
#define CDS_TEST            0x00000002
#define CDS_FULLSCREEN      0x00000004
#define CDS_GLOBAL          0x00000008
#define CDS_SET_PRIMARY     0x00000010
#define CDS_VIDEOPARAMETERS 0x00000020
#define CDS_NORESET			 0x10000000

/* Return values for ChangeDisplaySettings */
#define DISP_CHANGE_SUCCESSFUL       0
#define DISP_CHANGE_RESTART          1
#define DISP_CHANGE_FAILED          -1
#define DISP_CHANGE_BADMODE         -2
#define DISP_CHANGE_NOTUPDATED      -3
#define DISP_CHANGE_BADFLAGS        -4
#define DISP_CHANGE_BADPARAM        -5
#define DISP_CHANGE_BADESC          -6

// the GUID for this structure is:
//    {02C62061-1097-11d1-920F-00A024DF156E}
// or  static const GUID <<name>> = { 0x2c62061, 0x1097, 0x11d1, { 0x92, 0xf, 0x0, 0xa0, 0x24, 0xdf, 0x15, 0x6e } };
// or DEFINE_GUID(<<name>>,           0x2c62061, 0x1097, 0x11d1,   0x92, 0xf, 0x0, 0xa0, 0x24, 0xdf, 0x15, 0x6e);

#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct _GUID {          // size is 16 // same as winnt.h
    DWORD Data1;
    WORD   Data2;
    WORD   Data3;
    BYTE  Data4[8];
} GUID;
#endif

typedef struct _VIDEOPARAMETERS {
    GUID  Guid;                         // GUID for this structure
    DWORD dwOffset;                     // leave it 0 for now.
    DWORD dwCommand;                    // VP_COMMAND_*            SET or GET
    DWORD dwFlags;                      // bitfield, defined below SET or GET
    DWORD dwMode;                       // bitfield, defined below SET or GET
    DWORD dwTVStandard;                 // bitfield, defined below SET or GET
    DWORD dwAvailableModes;             // bitfield, defined below GET
    DWORD dwAvailableTVStandard;        // bitfield, defined below GET
    DWORD dwFlickerFilter;              // value                   SET or GET
    DWORD dwOverScanX;                  // value                   SET or GET
    DWORD dwOVerScanY;                  //                         SET or GET
    DWORD dwMaxUnscaledX;               // value                   SET or GET
    DWORD dwMaxUnscaledY;               //                         SET or GET
    DWORD dwPositionX;                  // value                   SET or GET
    DWORD dwPositionY;                  //                         SET or GET
    DWORD dwBrightness;                 // value                   SET or GET
    DWORD dwContrast;                   // value                   SET or GET
    DWORD dwCPType;                     // copy protection type    SET or GET
    DWORD dwCPCommand;                  // VP_CP_CMD_
    DWORD dwCPStandard;                 // what TV standards CP is available on. GET
    DWORD dwCPKey;
    DWORD bCP_APSTriggerBits;           // (a dword for alignment) SET(bits 0 and 1 valid).
    BYTE  bOEMCopyProtection[256];      // oem specific copy protection data SET or GET
} VIDEOPARAMETERS, *PVIDEOPARAMETERS, FAR *LPVIDEOPARAMETERS;

#define VP_COMMAND_GET          0x0001  // size set, return caps.
                                        // returned Flags = 0 if not supported.
#define VP_COMMAND_SET          0x0002  // size and params set.

#define VP_FLAGS_TV_MODE        0x0001
#define VP_FLAGS_TV_STANDARD    0x0002
#define VP_FLAGS_FLICKER        0x0004
#define VP_FLAGS_OVERSCAN       0x0008
#define VP_FLAGS_MAX_UNSCALED   0x0010  // do not use on SET
#define VP_FLAGS_POSITION       0x0020
#define VP_FLAGS_BRIGHTNESS     0x0040
#define VP_FLAGS_CONTRAST       0x0080
#define VP_FLAGS_COPYPROTECT    0x0100

#define VP_MODE_WIN_GRAPHICS    0x0001
#define VP_MODE_TV_PLAYBACK     0x0002  // optimize for TV video playback

#define VP_TV_STANDARD_NTSC_M   0x0001  //        75 IRE Setup
#define VP_TV_STANDARD_NTSC_M_J 0x0002  // Japan,  0 IRE Setup
#define VP_TV_STANDARD_PAL_B    0x0004
#define VP_TV_STANDARD_PAL_D    0x0008
#define VP_TV_STANDARD_PAL_H    0x0010
#define VP_TV_STANDARD_PAL_I    0x0020
#define VP_TV_STANDARD_PAL_M    0x0040
#define VP_TV_STANDARD_PAL_N    0x0080
#define VP_TV_STANDARD_SECAM_B  0x0100
#define VP_TV_STANDARD_SECAM_D  0x0200
#define VP_TV_STANDARD_SECAM_G  0x0400
#define VP_TV_STANDARD_SECAM_H  0x0800
#define VP_TV_STANDARD_SECAM_K  0x1000
#define VP_TV_STANDARD_SECAM_K1 0x2000
#define VP_TV_STANDARD_SECAM_L  0x4000
#define VP_TV_STANDARD_WIN_VGA  0x8000

#define VP_CP_TYPE_APS_TRIGGER  0x0001  // DVD trigger bits only
#define VP_CP_TYPE_MACROVISION  0x0002  // full macrovision data available

#define VP_CP_CMD_ACTIVATE      0x0001  // CP command type
#define VP_CP_CMD_DEACTIVATE    0x0002
#define VP_CP_CMD_CHANGE        0x0004


typedef struct _MACROVISION {
    WORD    wVersion;
    WORD    wFlags;
    BYTE    bCPCData;
    BYTE    bCPSData[34];
} MACROVISION, *PMACROVISION, FAR *LPMACROVISION;

#define MV_FLAGS_CPC_ONLY   0x0001
#define MV_FLAGS_CPS_ONLY   0x0002      // dont know if this is valid or not.
#define MV_FLAGS_CPC_CPS    0x0004      // both 8 bit and 132 bits are def'ed.

