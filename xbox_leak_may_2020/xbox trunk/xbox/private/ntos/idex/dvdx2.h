/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    dvdx2.h

Abstract:

    This module contains the definitions and structures for interfacing with a
    DVD-X2 ATAPI drive.

--*/

#ifndef _DVDX2_
#define _DVDX2_

#include <windef.h>
#include <xcrypt.h>
#include <sha.h>
#include <rc4.h>

#include <pshpack1.h>

//
// DVD-X2 specific additional sense codes.
//

#define DVDX2_ADSENSE_SPEED_REDUCTION_REQUESTED     0x80    // sense key 0x01
#define DVDX2_ADSENSE_GENERAL_READ_ERROR            0x80    // sense key 0x03
#define DVDX2_ADSENSE_COPY_PROTECTION_FAILURE       0x80    // sense key 0x05
#define DVDX2_ADSENSE_COMMAND_ERROR                 0x81    // sense key 0x05
#define DVDX2_ADSENSE_INSUFFICIENT_TIME             0x2E    // sense key 0x06

//
// DVD-X2 specific mode sense and select extensions.
//

#define DVDX2_MODE_PAGE_ADVANCED_DRIVE_CONTROL      0x20
#define DVDX2_MODE_PAGE_AUTHENTICATION              0x3E

//
// DVD-X2 advanced drive control page.
//

typedef struct _DVDX2_ADVANCED_DRIVE_CONTROL_PAGE {
    UCHAR PageCode : 6;         //  DVDX2_MODE_PAGE_ADVANCED_DRIVE_CONTROL
    UCHAR Reserved : 1;
    UCHAR PSBit : 1;
    UCHAR PageLength;
    UCHAR SpindleSpeedControl;
    UCHAR Reserved2[9];
} DVDX2_ADVANCED_DRIVE_CONTROL_PAGE, *PDVDX2_ADVANCED_DRIVE_CONTROL_PAGE;

typedef struct _DVDX2_ADVANCED_DRIVE_CONTROL {
    MODE_PARAMETER_HEADER10 Header;
    DVDX2_ADVANCED_DRIVE_CONTROL_PAGE AdvancedDriveControlPage;
} DVDX2_ADVANCED_DRIVE_CONTROL, *PDVDX2_ADVANCED_DRIVE_CONTROL;

//
// Define the DVD-X2 spindle speed control parameters.
//

#define DVDX2_SPINDLE_SPEED_MINIMUM                 0
#define DVDX2_SPINDLE_SPEED_MEDIUM                  1
#define DVDX2_SPINDLE_SPEED_MAXIMUM                 2

//
// DVD-X2 authentication page.
//

typedef struct _DVDX2_AUTHENTICATION_PAGE {
    UCHAR PageCode : 6;         //  DVDX2_MODE_PAGE_AUTHENTICATION
    UCHAR Reserved : 1;
    UCHAR PSBit : 1;
    UCHAR PageLength;
    UCHAR PartitionArea;
    UCHAR CDFValid;
    UCHAR Authentication;
    UCHAR DiscCategoryAndVersion;
    UCHAR DrivePhaseLevel;
    UCHAR ChallengeID;
    ULONG ChallengeValue;
    ULONG ResponseValue;
    ULONG Reserved2;
} DVDX2_AUTHENTICATION_PAGE, *PDVDX2_AUTHENTICATION_PAGE;

typedef struct _DVDX2_AUTHENTICATION {
    MODE_PARAMETER_HEADER10 Header;
    DVDX2_AUTHENTICATION_PAGE AuthenticationPage;
} DVDX2_AUTHENTICATION, *PDVDX2_AUTHENTICATION;

#define DVDX2_CDF_INVALID                           0x00
#define DVDX2_CDF_VALID                             0x01

//
// DVD-X2 host challenge response entry.
//

typedef struct _DVDX2_HOST_CHALLENGE_RESPONSE_ENTRY {
    UCHAR ChallengeLevel;
    UCHAR ChallengeID;
    ULONG ChallengeValue;
    UCHAR ResponseModifier;
    ULONG ResponseValue;
} DVDX2_HOST_CHALLENGE_RESPONSE_ENTRY, *PDVDX2_HOST_CHALLENGE_RESPONSE_ENTRY;

//
// Define the maximum number of entries allocated in the host challenge response
// table.
//

#define DVDX2_HOST_CHALLENGE_RESPONSE_ENTRY_COUNT   23

//
// DVD-X2 host challenge response table.
//

typedef struct _DVDX2_HOST_CHALLENGE_RESPONSE_TABLE {
    UCHAR Version;
    UCHAR NumberOfEntries;
    DVDX2_HOST_CHALLENGE_RESPONSE_ENTRY Entries[DVDX2_HOST_CHALLENGE_RESPONSE_ENTRY_COUNT];
} DVDX2_HOST_CHALLENGE_RESPONSE_TABLE, *PDVDX2_HOST_CHALLENGE_RESPONSE_TABLE;

//
// DVD-X2 control data structure.
//

typedef struct _DVDX2_CONTROL_DATA {
    UCHAR Length[2];
    UCHAR Reserved[2];
    DVD_LAYER_DESCRIPTOR LayerDescriptor;
    UCHAR Reserved2[15];
    UCHAR Reserved3[736];
    DVDX2_HOST_CHALLENGE_RESPONSE_TABLE HostChallengeResponseTable;
    UCHAR Reserved4[32];
    LARGE_INTEGER ContentSourceTimeStamp;
    UCHAR Reserved5[20];
    GUID ContentSourceIdentifier;
    UCHAR Reserved6[84];
    LARGE_INTEGER AuthoringTimeStamp;
    UCHAR Reserved7[19];
    UCHAR AuthoringSystemType;
    GUID AuthoringSystemIdentifier;
    UCHAR AuthoringHash[XC_DIGEST_LEN];
    UCHAR AuthoringSignature[XC_ENC_SIGNATURE_SIZE];
    LARGE_INTEGER MasteringTimeStamp;
    UCHAR Reserved8[19];
    UCHAR MasteringSystemType;
    GUID MasteringSystemIdentifier;
    UCHAR MasteringHash[XC_DIGEST_LEN];
    UCHAR MasteringSignature[64];
    UCHAR Reserved9;
} DVDX2_CONTROL_DATA, *PDVDX2_CONTROL_DATA;

//
// Define the layer and block number that holds the control data structure.
//

#define DVDX2_CONTROL_DATA_LAYER                    1
#define DVDX2_CONTROL_DATA_BLOCK_NUMBER             0x00FD0200

#include <poppack.h>

#endif  // DVDX2
