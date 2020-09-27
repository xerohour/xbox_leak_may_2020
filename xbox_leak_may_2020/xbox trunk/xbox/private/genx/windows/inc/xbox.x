/************************************************************************
*                                                                       *
*   Xbox.h -- This module defines the Xbox APIs                         *
*                                                                       *
*   Copyright (c) 2000 - 2001 Microsoft Corp. All rights reserved.      *
*                                                                       *
************************************************************************/
#ifndef _XBOX_
#define _XBOX_

;begin_internal

//
// All APIs unique to Xbox that live in XAPILIB.LIB should be
// defined in this header file
//

#ifndef _XBOXP_
#define _XBOXP_

;end_internal

//
// Define API decoration for direct importing of DLL references.
//

;begin_both
#define XBOXAPI

#ifdef __cplusplus
extern "C" {
#endif	
;end_both

XBOXAPI
PVOID
WINAPI
XLoadSectionA(
    IN LPCSTR pSectionName
    );
#define XLoadSection  XLoadSectionA

XBOXAPI
BOOL
WINAPI
XFreeSectionA(
    IN LPCSTR pSectionName
    );
#define XFreeSection  XFreeSectionA

XBOXAPI
HANDLE
WINAPI
XGetSectionHandleA(
    IN LPCSTR pSectionName
    );
#define XGetSectionHandle  XGetSectionHandleA

XBOXAPI
PVOID
WINAPI
XLoadSectionByHandle(
    IN HANDLE hSection
    );

XBOXAPI
BOOL
WINAPI
XFreeSectionByHandle(
    HANDLE hSection
    );

XBOXAPI
DWORD
WINAPI
XGetSectionSize(
    HANDLE hSection
    );

#define MAX_DISPLAY_BLOCKS  50001

XBOXAPI
DWORD
WINAPI
XGetDisplayBlocks(
	IN LPCSTR lpPathName
	);


#define XSAVEGAME_NOCOPY   1

XBOXAPI
DWORD
WINAPI
XCreateSaveGame(
    IN LPCSTR lpRootPathName,
    IN LPCWSTR lpSaveGameName,
    IN DWORD dwCreationDisposition,
    IN DWORD dwCreateFlags,
    OUT LPSTR lpPathBuffer,
    IN UINT uSize
    );

XBOXAPI
DWORD
WINAPI
XDeleteSaveGame(
    IN LPCSTR lpRootPathName,
    IN LPCWSTR lpSaveGameName
    );

#define MAX_GAMENAME     128

typedef struct _XGAME_FIND_DATA {
    WIN32_FIND_DATAA wfd;
    CHAR szSaveGameDirectory[MAX_PATH];
    WCHAR szSaveGameName[MAX_GAMENAME];
} XGAME_FIND_DATA, *PXGAME_FIND_DATA;

XBOXAPI
HANDLE
WINAPI
XFindFirstSaveGame(
    IN LPCSTR lpRootPathName,
    OUT PXGAME_FIND_DATA pFindGameData
    );

XBOXAPI
BOOL
WINAPI
XFindNextSaveGame(
    IN HANDLE hFindGame,
    OUT PXGAME_FIND_DATA pFindGameData
    );

XBOXAPI
BOOL
WINAPI
XFindClose(
    IN HANDLE hFind
    );

#define MAX_NICKNAME 32

XBOXAPI
BOOL
WINAPI
XSetNicknameW(
    IN LPCWSTR lpNickname,
    IN BOOL fPreserveCase
    );
#define XSetNickname XSetNicknameW

XBOXAPI
HANDLE
WINAPI
XFindFirstNicknameW(
    IN BOOL fThisTitleOnly,
    OUT LPWSTR lpNickname,
    IN UINT uSize
    );
#define XFindFirstNickname XFindFirstNicknameW

XBOXAPI
BOOL
WINAPI
XFindNextNicknameW(
    IN HANDLE hFindNickname,
    OUT LPWSTR lpNickname,
    IN UINT uSize
    );
#define XFindNextNickname XFindNextNicknameW

#define MAX_CONTENT_DISPLAY_NAME 128

typedef struct _XCONTENT_FIND_DATA {
    WIN32_FIND_DATAA wfd;
    DWORD dwOfferingId;
    DWORD dwFlags;
    CHAR szContentDirectory[MAX_PATH];
    WCHAR szDisplayName[MAX_CONTENT_DISPLAY_NAME];
} XCONTENT_FIND_DATA, *PXCONTENT_FIND_DATA;

XBOXAPI
HANDLE
WINAPI
XFindFirstContent(
    IN LPCSTR lpRootPathName,
    IN DWORD dwFlagFilter,
    OUT PXCONTENT_FIND_DATA pFindContentData
    );

XBOXAPI
BOOL
WINAPI
XFindNextContent(
    IN HANDLE hFindContent,
    OUT PXCONTENT_FIND_DATA pFindContentData
    );

;begin_both
#define MAX_SONG_NAME       32
#define MAX_SOUNDTRACK_NAME 32
#define MAX_SOUNDTRACKS     100
#define MAX_SONGS_IN_SNDTRK 500
;end_both

typedef struct _XSOUNDTRACK_DATA {
    UINT    uSoundtrackId;
    UINT    uSongCount;
    UINT    uSoundtrackLength;
    WCHAR   szName[MAX_SOUNDTRACK_NAME];
} XSOUNDTRACK_DATA, *PXSOUNDTRACK_DATA;

XBOXAPI
HANDLE
WINAPI
XFindFirstSoundtrack(
    OUT PXSOUNDTRACK_DATA pSoundtrackData
    );

XBOXAPI
BOOL
WINAPI
XFindNextSoundtrack(
    IN HANDLE hFindHandle,
    OUT PXSOUNDTRACK_DATA pSoundtrackData
    );

XBOXAPI
HANDLE
WINAPI
XOpenSoundtrackSong(
    IN DWORD dwSongId,
    IN BOOL fAsyncMode
    );

XBOXAPI
BOOL
WINAPI
XGetSoundtrackSongInfo(
    IN DWORD dwSoundtrackId,
    IN UINT uIndex,
    OUT PDWORD pdwSongId,
    OUT PDWORD pdwSongLength,
    OUT OPTIONAL PWSTR szNameBuffer,
    IN UINT uBufferSize
    );

;begin_internal

#define MAX_SONGS                   500 // obsolete - replaced by public constant: MAX_SONGS_IN_SNDTRK
#define WMADESC_LIST_SIZE           6
#define STDB_VERSION                1
#define STDB_STSIG                  0x021371
#define STDB_LISTSIG                0x031073
#define STDB_PAGE_SIZE              512
#define INVALID_STID                0xFFFFFFFF

typedef struct {

    UINT    Version;
    UINT    StCount;
    UINT    NextStId;
    UINT    StBlocks[MAX_SOUNDTRACKS];
    UINT    NextSongId;



} STDB_HDR, *PSTDB_HDR;

typedef struct {

    UINT    Signature;
    UINT    Id;
    UINT    SongCount;
    UINT    ListBlocks[(MAX_SONGS_IN_SNDTRK / WMADESC_LIST_SIZE) + 1];
    UINT    SoundtrackLength;
    WCHAR   Name[MAX_SOUNDTRACK_NAME];

} STDB_STDESC, *PSTDB_STDESC;


typedef struct {

    DWORD Signature;
    UINT  StId;
    UINT  ListIndex;
    BOOL  InUse;
    DWORD SongIds[WMADESC_LIST_SIZE];
    DWORD SongLengths[WMADESC_LIST_SIZE];
    WCHAR SongNames[WMADESC_LIST_SIZE][MAX_SONG_NAME];



} STDB_LIST, *PSTDB_LIST;

typedef enum _XC_VALUE_INDEX
{
    XC_TIMEZONE_BIAS = 0,
    XC_TZ_STD_NAME,
    XC_TZ_STD_DATE,
    XC_TZ_STD_BIAS,
    XC_TZ_DLT_NAME,
    XC_TZ_DLT_DATE,
    XC_TZ_DLT_BIAS,
    XC_LANGUAGE,
    XC_VIDEO_FLAGS,
    XC_AUDIO_FLAGS,
    XC_PARENTAL_CONTROL_GAMES,
    XC_PARENTAL_CONTROL_PASSWORD,
    XC_PARENTAL_CONTROL_MOVIES,
    XC_ONLINE_IP_ADDRESS,
    XC_ONLINE_DNS_ADDRESS,
    XC_ONLINE_DEFAULT_GATEWAY_ADDRESS,
    XC_ONLINE_SUBNET_ADDRESS,
    XC_MISC_FLAGS,
    XC_DVD_REGION,

    // end of user configurations
    XC_MAX_OS = 0xff,

    // start of factory settings
    XC_FACTORY_START_INDEX,
    XC_FACTORY_SERIAL_NUMBER = XC_FACTORY_START_INDEX,
    XC_FACTORY_ETHERNET_ADDR,
    XC_FACTORY_ONLINE_KEY,
    XC_FACTORY_AV_REGION,
    XC_FACTORY_GAME_REGION,

    // end of factory settings
    XC_MAX_FACTORY = 0x1ff,

    // special value to access the encryption section of the EEPROM
    // or to access the entire EEPROM at once
    XC_ENCRYPTED_SECTION = 0xfffe,
    XC_MAX_ALL = 0xffff

} XC_VALUE_INDEX;

;end_internal

#define XC_LANGUAGE_UNKNOWN         0     ;internal
#define XC_LANGUAGE_ENGLISH         1
#define XC_LANGUAGE_JAPANESE        2
#define XC_LANGUAGE_GERMAN          3
#define XC_LANGUAGE_FRENCH          4
#define XC_LANGUAGE_SPANISH         5
#define XC_LANGUAGE_ITALIAN         6

XBOXAPI
DWORD
WINAPI
XGetLanguage(
    VOID
    );

#define XC_AV_PACK_SCART            0
#define XC_AV_PACK_HDTV             1
#define XC_AV_PACK_VGA              2
#define XC_AV_PACK_RFU              3
#define XC_AV_PACK_SVIDEO           4
#define XC_AV_PACK_STANDARD         6
#define XC_AV_PACK_NONE             7     ;internal

XBOXAPI
DWORD
WINAPI
XGetAVPack(
    VOID
    );

#define XC_VIDEO_STANDARD_NTSC_M    1
#define XC_VIDEO_STANDARD_NTSC_J    2
#define XC_VIDEO_STANDARD_PAL_I     3
#define XC_VIDEO_STANDARD_PAL_M     4     ;internal

XBOXAPI
DWORD
WINAPI
XGetVideoStandard(
    VOID
    );

#define XC_VIDEO_FLAGS_WIDESCREEN   0x00000001
#define XC_VIDEO_FLAGS_HDTV_720p    0x00000002
#define XC_VIDEO_FLAGS_HDTV_1080i   0x00000004
#define XC_VIDEO_FLAGS_HDTV_480p    0x00000008
#define XC_VIDEO_FLAGS_LETTERBOX    0x00000010
#define XC_VIDEO_FLAGS_PAL_60Hz     0x00000040

XBOXAPI
DWORD
WINAPI
XGetVideoFlags(
    VOID
    );

#define XC_AUDIO_FLAGS_STEREO       0x00000000
#define XC_AUDIO_FLAGS_MONO         0x00000001
#define XC_AUDIO_FLAGS_SURROUND     0x00000002
#define XC_AUDIO_FLAGS_ENABLE_AC3   0x00010000
#define XC_AUDIO_FLAGS_ENABLE_DTS   0x00020000

#define XC_AUDIO_FLAGS_BASICMASK    0x0000FFFF
#define XC_AUDIO_FLAGS_ENCODEDMASK  0xFFFF0000

#define XC_AUDIO_FLAGS_BASIC(c)      ((DWORD)(c) & XC_AUDIO_FLAGS_BASICMASK)
#define XC_AUDIO_FLAGS_ENCODED(c)    ((DWORD)(c) & XC_AUDIO_FLAGS_ENCODEDMASK)
#define XC_AUDIO_FLAGS_COMBINED(b,e) (XC_AUDIO_FLAGS_BASIC(b) | XC_AUDIO_FLAGS_ENCODED(e))

XBOXAPI
DWORD
WINAPI
XGetAudioFlags(
    VOID
    );

#define XC_PC_ESRB_ALL              0
#define XC_PC_ESRB_ADULT            1
#define XC_PC_ESRB_MATURE           2
#define XC_PC_ESRB_TEEN             3
#define XC_PC_ESRB_EVERYONE         4
#define XC_PC_ESRB_KIDS_TO_ADULTS   5
#define XC_PC_ESRB_EARLY_CHILDHOOD  6

XBOXAPI
DWORD
WINAPI
XGetParentalControlSetting(
    VOID
    );

;begin_both
#define XC_GAME_REGION_NA             0x00000001
#define XC_GAME_REGION_JAPAN          0x00000002
#define XC_GAME_REGION_RESTOFWORLD    0x00000004
#define XC_GAME_REGION_MANUFACTURING  0x80000000
;end_both

XBOXAPI
DWORD
WINAPI
XGetGameRegion(
    VOID
    );

;begin_internal

#define XC_MISC_FLAG_AUTOPOWERDOWN  0x0001
#define XC_MISC_FLAG_DONT_USE_DST   0x0002

//
// Config value routines specifically for OS config sector
//
XBOXAPI
DWORD
WINAPI
XSetValue(
    IN ULONG ulValueIndex,
    IN ULONG ulType,
    IN PVOID pValue,
    IN ULONG cbValueLength
    );

XBOXAPI
DWORD
WINAPI
XQueryValue(
    IN ULONG ulValueIndex,
    OUT PULONG pulType,
    OUT PVOID pValue,
    IN ULONG cbValueLength,
    OUT PULONG pcbResultLength
    );

//
// HACK to simulate XInput APIs using another machine on the network
//
#ifdef _XINPUT_REMOTE
#define XInitDevices                    XInitDevicesRM
#define XGetDevices                     XGetDevicesRM
#define XGetDeviceChanges               XGetDeviceChangesRM
#define XInputOpen                      XInputOpenRM
#define XInputClose                     XInputCloseRM
#define XInputGetState                  XInputGetStateRM
#define XInputPoll                      XInputPollRM
#define XInputSetState                  XInputSetStateRM
#define XInputGetCapabilities           XInputGetCapabilitiesRM
#define XMountMUA                       XMountMUARM
#define XUnmountMU                      XUnmountMURM
#define XMUPortFromDriveLetterA         XMUPortFromDriveLetterARM
#define XMUSlotFromDriveLetterA         XMUSlotFromDriveLetterARM
#define XInputDebugInitKeyboardQueue    XInputDebugInitKeyboardQueueRM
#define XInputDebugGetKeystroke         XInputDebugGetKeystrokeRM
#endif // _XINPUT_REMOTE

;end_internal

typedef struct _XPP_DEVICE_TYPE
{
    ULONG Reserved[3];
} XPP_DEVICE_TYPE, *PXPP_DEVICE_TYPE;

extern XPP_DEVICE_TYPE XDEVICE_TYPE_GAMEPAD_TABLE;
extern XPP_DEVICE_TYPE XDEVICE_TYPE_MEMORY_UNIT_TABLE;
extern XPP_DEVICE_TYPE XDEVICE_TYPE_VOICE_MICROPHONE_TABLE;
extern XPP_DEVICE_TYPE XDEVICE_TYPE_VOICE_HEADPHONE_TABLE;


#define     XDEVICE_TYPE_GAMEPAD           (&XDEVICE_TYPE_GAMEPAD_TABLE)
#define     XDEVICE_TYPE_MEMORY_UNIT       (&XDEVICE_TYPE_MEMORY_UNIT_TABLE)
#define     XDEVICE_TYPE_VOICE_MICROPHONE   (&XDEVICE_TYPE_VOICE_MICROPHONE_TABLE)
#define     XDEVICE_TYPE_VOICE_HEADPHONE    (&XDEVICE_TYPE_VOICE_HEADPHONE_TABLE)

;begin_internal
#ifdef _XBOX_ //only define this if xbox.h was already included.
extern XPP_DEVICE_TYPE XDEVICE_TYPE_IR_REMOTE_TABLE;
#define  XDEVICE_TYPE_IR_REMOTE       (&XDEVICE_TYPE_IR_REMOTE_TABLE)
extern   XPP_DEVICE_TYPE XDEVICE_TYPE_DVD_CODE_SERVER_TABLE;
#define  XDEVICE_TYPE_DVD_CODE_SERVER (&XDEVICE_TYPE_DVD_CODE_SERVER_TABLE)
#endif
;end_internal

#ifdef DEBUG_KEYBOARD
#include <xkbd.h>
#endif

#define     XDEVICE_PORT0               0
#define     XDEVICE_PORT1               1
#define     XDEVICE_PORT2               2
#define     XDEVICE_PORT3               3

#define     XDEVICE_NO_SLOT             0
#define     XDEVICE_TOP_SLOT            0
#define     XDEVICE_BOTTOM_SLOT         1

#define     XDEVICE_PORT0_MASK          (1 << XDEVICE_PORT0)
#define     XDEVICE_PORT1_MASK          (1 << XDEVICE_PORT1)
#define     XDEVICE_PORT2_MASK          (1 << XDEVICE_PORT2)
#define     XDEVICE_PORT3_MASK          (1 << XDEVICE_PORT3)
#define     XDEVICE_PORT0_TOP_MASK      (1 << XDEVICE_PORT0)
#define     XDEVICE_PORT1_TOP_MASK      (1 << XDEVICE_PORT1)
#define     XDEVICE_PORT2_TOP_MASK      (1 << XDEVICE_PORT2)
#define     XDEVICE_PORT3_TOP_MASK      (1 << XDEVICE_PORT3)
#define     XDEVICE_PORT0_BOTTOM_MASK   (1 << (XDEVICE_PORT0 + 16))
#define     XDEVICE_PORT1_BOTTOM_MASK   (1 << (XDEVICE_PORT1 + 16))
#define     XDEVICE_PORT2_BOTTOM_MASK   (1 << (XDEVICE_PORT2 + 16))
#define     XDEVICE_PORT3_BOTTOM_MASK   (1 << (XDEVICE_PORT3 + 16))

typedef struct _XDEVICE_PREALLOC_TYPE
{
    PXPP_DEVICE_TYPE DeviceType;
    DWORD            dwPreallocCount;
} XDEVICE_PREALLOC_TYPE, *PXDEVICE_PREALLOC_TYPE;

#define XGetPortCount() 4

extern BOOL XPP_XInitDevicesHasBeenCalled; ;internal

XBOXAPI
VOID
WINAPI
XInitDevices(
    DWORD                  dwPreallocTypeCount,
    PXDEVICE_PREALLOC_TYPE PreallocTypes
    );

XBOXAPI
DWORD
WINAPI
XGetDevices(
    IN PXPP_DEVICE_TYPE DeviceType
    );

XBOXAPI
BOOL
WINAPI
XGetDeviceChanges(
    IN PXPP_DEVICE_TYPE DeviceType,
    OUT PDWORD pdwInsertions,
    OUT PDWORD pdwRemovals
    );

;begin_internal

#ifdef _XBOX_ //only define this if xbox.h was already included.
XBOXAPI
DWORD
WINAPI
XPeekDevices(
    IN  PXPP_DEVICE_TYPE DeviceType,
    IN  OUT PDWORD pLastGotten,
    IN  OUT PDWORD pStale
    );
#endif

;end_internal


#include <PSHPACK1.H>

typedef struct _XINPUT_GAMEPAD
{
    WORD    wButtons;
    BYTE    bAnalogButtons[8];
    SHORT   sThumbLX;
    SHORT   sThumbLY;
    SHORT   sThumbRX;
    SHORT   sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

#define XINPUT_GAMEPAD_DPAD_UP          0x00000001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
#define XINPUT_GAMEPAD_START            0x00000010
#define XINPUT_GAMEPAD_BACK             0x00000020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080

#define XINPUT_GAMEPAD_A                0
#define XINPUT_GAMEPAD_B                1
#define XINPUT_GAMEPAD_X                2
#define XINPUT_GAMEPAD_Y                3
#define XINPUT_GAMEPAD_BLACK            4
#define XINPUT_GAMEPAD_WHITE            5
#define XINPUT_GAMEPAD_LEFT_TRIGGER     6
#define XINPUT_GAMEPAD_RIGHT_TRIGGER    7

#define XINPUT_GAMEPAD_MAX_CROSSTALK	30

typedef struct _XINPUT_RUMBLE
{
   WORD   wLeftMotorSpeed;
   WORD   wRightMotorSpeed;
} XINPUT_RUMBLE, *PXINPUT_RUMBLE;

typedef struct _XINPUT_STATE
{
    DWORD dwPacketNumber;
    union
    {
        XINPUT_GAMEPAD Gamepad;
    };
} XINPUT_STATE, *PXINPUT_STATE;

;begin_internal
#ifdef _XBOX_
typedef struct _XINPUT_IR_REMOTE
{
    WORD  wKeyCode;
    WORD  wTimeDelta;
} XINPUT_IR_REMOTE, *PXINPUT_IR_REMOTE;

typedef struct _XINPUT_STATE_INTERNAL
{
    DWORD dwPacketNumber;
    union
    {
        XINPUT_GAMEPAD   Gamepad;
        XINPUT_IR_REMOTE IrRemote;
    };
} XINPUT_STATE_INTERNAL, *PXINPUT_STATE_INTERNAL;
#endif
;end_internal

#define XINPUT_FEEDBACK_HEADER_INTERNAL_SIZE 58
typedef struct _XINPUT_FEEDBACK_HEADER
{
    DWORD           dwStatus;
    HANDLE OPTIONAL hEvent;
    BYTE            Reserved[XINPUT_FEEDBACK_HEADER_INTERNAL_SIZE];
} XINPUT_FEEDBACK_HEADER, *PXINPUT_FEEDBACK_HEADER;

typedef struct _XINPUT_FEEDBACK
{
    XINPUT_FEEDBACK_HEADER Header;
    union
    {
      XINPUT_RUMBLE Rumble;
    };
} XINPUT_FEEDBACK, *PXINPUT_FEEDBACK;

typedef struct _XINPUT_CAPABILITIES
{
    BYTE    SubType;
    WORD    Reserved;
    union
    {
      XINPUT_GAMEPAD Gamepad;
    } In;
    union
    {
      XINPUT_RUMBLE Rumble;
    } Out;
} XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

#include <POPPACK.H>

#define XINPUT_DEVSUBTYPE_GC_GAMEPAD              0x01
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT          0x02
#define XINPUT_DEVSUBTYPE_GC_WHEEL                0x10
#define XINPUT_DEVSUBTYPE_GC_ARCADE_STICK         0x20
#define XINPUT_DEVSUBTYPE_GC_DIGITAL_ARCADE_STICK 0x21
#define XINPUT_DEVSUBTYPE_GC_FLIGHT_STICK         0x30
#define XINPUT_DEVSUBTYPE_GC_SNOWBOARD            0x40

typedef struct _XINPUT_POLLING_PARAMETERS
{
    BYTE       fAutoPoll:1;
    BYTE       fInterruptOut:1;
    BYTE       ReservedMBZ1:6;
    BYTE       bInputInterval;
    BYTE       bOutputInterval;
    BYTE       ReservedMBZ2;
} XINPUT_POLLING_PARAMETERS, *PXINPUT_POLLING_PARAMETERS;

XBOXAPI
HANDLE
WINAPI
XInputOpen(
    IN PXPP_DEVICE_TYPE DeviceType,
    IN DWORD dwPort,
    IN DWORD dwSlot,
    IN PXINPUT_POLLING_PARAMETERS pPollingParameters OPTIONAL
    );

XBOXAPI
VOID
WINAPI
XInputClose(
    IN HANDLE hDevice
    );

XBOXAPI
DWORD
WINAPI
XInputGetState(
    IN HANDLE hDevice,
    OUT PXINPUT_STATE  pState
    );

XBOXAPI
DWORD
WINAPI
XInputPoll(
    IN HANDLE hDevice
    );

XBOXAPI
DWORD
WINAPI
XInputSetState(
    IN HANDLE hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
    );

XBOXAPI
DWORD
WINAPI
XInputGetCapabilities(
    IN HANDLE hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
    );

XBOXAPI
DWORD
WINAPI
XMountMUA(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT PCHAR pchDrive
    );
#define XMountMU  XMountMUA

;begin_internal

XBOXAPI
DWORD
WINAPI
XMountMURootA(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT PCHAR pchDrive
    );
#define XMountMURoot XMountMURootA

;end_internal

XBOXAPI
DWORD
WINAPI
XUnmountMU(
    IN DWORD dwPort,
    IN DWORD dwSlot
    );

XBOXAPI
DWORD
WINAPI
XMUPortFromDriveLetterA(
    IN CHAR chDrive
    );
#define XMUPortFromDriveLetter  XMUPortFromDriveLetterA

XBOXAPI
DWORD
WINAPI
XMUSlotFromDriveLetterA(
    IN CHAR chDrive
    );
#define XMUSlotFromDriveLetter  XMUSlotFromDriveLetterA

#define MAX_MUNAME 32

XBOXAPI
DWORD
WINAPI
XMUNameFromDriveLetter(
    IN CHAR chDrive,
    OUT LPWSTR lpName,
    IN UINT cchName
    );

;begin_internal

XBOXAPI
DWORD
WINAPI
XMUNameFromPortSlot(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT LPWSTR lpName,
    IN UINT cchName
    );

XBOXAPI
DWORD
WINAPI
XReadMUMetaData(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    IN LPVOID lpBuffer,
    IN DWORD dwByteOffset,
    IN DWORD dwNumberOfBytesToRead
    );

XBOXAPI
DWORD
WINAPI
XMUWriteNameToDriveLetter(
    IN CHAR chDrive,
    IN LPCWSTR lpName
    );

XBOXAPI
DWORD
WINAPI
XCleanMUFromRoot(
    IN CHAR chDrive,
    PCSTR pszPreserveDir OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XCleanDrive(
    IN CHAR chDrive
    );

;end_internal

#define XINIT_MOUNT_UTILITY_DRIVE   0x00000001  ;both
#define XINIT_FORMAT_UTILITY_DRIVE  0x00000002  ;both
#define XINIT_LIMIT_DEVKIT_MEMORY   0x00000004  ;both
#define XINIT_NO_SETUP_HARD_DISK    0x00000008  ;internal
#define XINIT_DONT_MODIFY_HARD_DISK 0x00000010  ;both

XBOXAPI
BOOL
WINAPI
XMountUtilityDrive(
    IN BOOL fFormatClean
    );

XBOXAPI
BOOL
WINAPI
XFormatUtilityDrive(
    VOID
    );

XBOXAPI
DWORD
WINAPI
XMountAlternateTitleA(
    IN LPCSTR lpRootPath,
    IN DWORD dwAltTitleId,
    OUT PCHAR pchDrive
    );
#define XMountAlternateTitle  XMountAlternateTitleA

XBOXAPI
DWORD
WINAPI
XUnmountAlternateTitleA(
    IN CHAR chDrive
    );
#define XUnmountAlternateTitle  XUnmountAlternateTitleA

XBOXAPI
DWORD
WINAPI
XGetDiskSectorSizeA(
    IN LPCSTR lpRootPathName
    );
#define XGetDiskSectorSize  XGetDiskSectorSizeA

XBOXAPI
DWORD
WINAPI
XGetDiskClusterSizeA(
    IN LPCSTR lpRootPathName
    );
#define XGetDiskClusterSize  XGetDiskClusterSizeA

;begin_internal
#ifdef _XBOX_ //only define this if xbox.h was already included.
#include <pshpack1.h>
typedef struct _XDCS_DVD_CODE_INFORMATION
{
    WORD    bcdVersion;
    DWORD   dwCodeLength;
} XDCS_DVD_CODE_INFORMATION, *PXDCS_DVD_CODE_INFORMATION;
#include <poppack.h>

typedef struct _XDCS_ASYNC_DOWNLOAD_REQUEST
{
    DWORD  dwDeviceInstance;
    PVOID  pvBuffer;
    ULONG  ulOffset;
    ULONG  ulLength;
    ULONG  ulBytesRead;
    ULONG  ulStatus;
    HANDLE hCompleteEvent;
} XDCS_ASYNC_DOWNLOAD_REQUEST, *PXDCS_ASYNC_DOWNLOAD_REQUEST;

DWORD
WINAPI
XDCSGetInformation(
    IN  DWORD  dwPort,
    OUT PDWORD pdwDeviceInstance,
    OUT PXDCS_DVD_CODE_INFORMATION pDvdCodeInformation
    );

DWORD
WINAPI
XDCSDownloadCode(
    DWORD   dwDeviceInstance,
    PVOID   pvBuffer,
    ULONG   ulOffset,
    ULONG   ulLength,
    PULONG  pulBytesRead
    );

DWORD
WINAPI
XDCSDownloadCodeAsync(
    IN OUT PXDCS_ASYNC_DOWNLOAD_REQUEST pXDCSDownloadRequest
    );
#endif //_XBOX_


DWORD
WINAPI
XAutoPowerDownSet(
    BOOL fAutoPowerDown
    );

DWORD
WINAPI
XAutoPowerDownGet(
    BOOL *pfAutoPowerDown
    );

void
WINAPI
XAutoPowerDownResetTimer();

#if DBG
void
WINAPI
XAutoPowerDownDebugSetTimeout(
    LONGLONG llTimeout
    );
#endif //DBG

BOOL
WINAPI
XapiSetLocalTime(
    IN CONST SYSTEMTIME *lpLocalTime
    );

DWORD
WINAPI
XapipQueryTimeZoneInformation(
    OUT PTIME_ZONE_INFORMATION TimeZoneInformation,
    OUT PBOOL pfUseDST
    );

DWORD
WINAPI
XapipSetTimeZoneInformation(
    IN PTIME_ZONE_INFORMATION TimeZoneInformation
    );

BOOL
WINAPI
XapipUseDaylightSavingTime();

;end_internal


;begin_internal
#ifndef MAX_LAUNCH_DATA_SIZE
;end_internal
;begin_both
#define MAX_LAUNCH_DATA_SIZE 3072

typedef struct _LAUNCH_DATA
{
    BYTE Data[MAX_LAUNCH_DATA_SIZE];
} LAUNCH_DATA, *PLAUNCH_DATA;
;end_both
;begin_internal
#endif // ! MAX_LAUNCH_DATA_SIZE
;end_internal

typedef struct _LD_LAUNCH_DASHBOARD
{
    DWORD dwReason;
    DWORD dwContext;
    DWORD dwParameter1;
    DWORD dwParameter2;
    BYTE  Reserved[MAX_LAUNCH_DATA_SIZE - 16];
} LD_LAUNCH_DASHBOARD, *PLD_LAUNCH_DASHBOARD;

#define XLD_LAUNCH_DASHBOARD_MAIN_MENU  0 // Does not return to application
#define XLD_LAUNCH_DASHBOARD_ERROR      1 // Does not return to application
#define XLD_LAUNCH_DASHBOARD_MEMORY     2
#define XLD_LAUNCH_DASHBOARD_SETTINGS   3
#define XLD_LAUNCH_DASHBOARD_MUSIC      4
#define XLD_LAUNCH_DASHBOARD_BOOT       5 // Not supported at this time ;internal


;begin_internal
#define XLD_LAUNCH_DASHBOARD_NETWORK_CONFIGURATION	6
#define XLD_LAUNCH_DASHBOARD_NEW_ACCOUNT_SIGNUP         7 
#define XLD_LAUNCH_DASHBOARD_MESSAGE_SERVER_INFO        8
#define XLD_LAUNCH_DASHBOARD_POLICY_DOCUMENT            9
#define XLD_LAUNCH_DASHBOARD_ONLINE_MENU		10
#define XLD_LAUNCH_DASHBOARD_FORCED_NAME_CHANGE         11
#define XLD_LAUNCH_DASHBOARD_FORCED_BILLING_EDIT        12
;end_internal
//
// When XDash is launched with XLD_LAUNCH_DASHBOARD_ERROR,
// LD_LAUNCH_DASHBOARD.dwParameter1 field contains one of
// the following error codes.
//
#define XLD_ERROR_INVALID_XBE           1
#define XLD_ERROR_INVALID_HARD_DISK     2
#define XLD_ERROR_XBE_REGION            3
#define XLD_ERROR_XBE_PARENTAL_CONTROL  4
#define XLD_ERROR_XBE_MEDIA_TYPE        5

//
// When the dwReason is XLD_LAUNCH_DASHBOARD_SETTINGS,
// LD_LAUNCH_DASHBOARD.dwParameter1 will have 0 or more
// of the following flags set.
//
#define XLD_SETTINGS_CLOCK              0x01 // Does not return to application with context
#define XLD_SETTINGS_TIMEZONE           0x02 // Does not return to application with context
#define XLD_SETTINGS_LANGUAGE           0x04 // Does not return to application with context
#define XLD_SETTINGS_VIDEO              0x08
#define XLD_SETTINGS_AUDIO              0x10

;begin_internal
//
// When the dwReason is XLD_LAUNCH_DASHBOARD_POLICY_DOCUMENT,
// LD_LAUNCH_DASHBOARD.dwParameter1 will have 1 or probably more of 
// the following flags set.
//
#define XLD_POLICY_SUBSCRIPTION_AGREEMENT 0x01
#define XLD_POLICY_TERMS_OF_USE           0x02
#define XLD_POLICY_CODE_OF_CONDUCT        0x04 
#define XLD_POLICY_PRIVACY_STATEMENT      0x08 
;end_internal

typedef struct _LD_FROM_DASHBOARD
{
    DWORD dwContext;
    BYTE  Reserved[MAX_LAUNCH_DATA_SIZE - 4];
} LD_FROM_DASHBOARD, *PLD_FROM_DASHBOARD;

typedef struct _LD_FROM_DEBUGGER_CMDLINE
{
    CHAR szCmdLine[MAX_LAUNCH_DATA_SIZE];
} LD_FROM_DEBUGGER_CMDLINE, *PLD_FROM_DEBUGGER_CMDLINE;

#define XLDEMO_RUNMODE_KIOSKMODE        0x01
#define XLDEMO_RUNMODE_USERSELECTED     0x02

typedef struct _LD_DEMO  // Required for launching into and out of demos, data type is LDT_TITLE
{
    DWORD dwID;
    DWORD dwRunmode;
    DWORD dwTimeout;
    CHAR  szLauncherXBE[64];
    CHAR  szLaunchedXBE[64];
    BYTE  Reserved[MAX_LAUNCH_DATA_SIZE - 140];
} LD_DEMO, *PLD_DEMO;

;begin_internal

typedef struct _LD_FROM_TITLE_UPDATE
{
    DWORD   dwContext;
    HRESULT hr;
    BYTE    Reserved[MAX_LAUNCH_DATA_SIZE - 8];
} LD_FROM_TITLE_UPDATE, *PLD_FROM_TITLE_UPDATE;

;end_internal

XBOXAPI
DWORD
WINAPI
XLaunchNewImageA(
    IN LPCSTR lpImagePath,
    IN PLAUNCH_DATA pLaunchData
    );
#define XLaunchNewImage XLaunchNewImageA

#define LDT_TITLE                 0
#define LDT_LAUNCH_DASHBOARD      1 ;internal
#define LDT_FROM_DASHBOARD        2
#define LDT_FROM_DEBUGGER_CMDLINE 3
#define LDT_TITLE_UPDATE          4 ;internal
#define LDT_FROM_TITLE_UPDATE     6 ;internal

XBOXAPI
DWORD
WINAPI
XGetLaunchInfo(
    OUT PDWORD pdwLaunchDataType,
    OUT PLAUNCH_DATA pLaunchData
    );

;begin_internal

XBOXAPI
DWORD
WINAPI
XWriteTitleInfoAndRebootA(
    IN LPCSTR pszLaunchPath,
    IN LPCSTR pszDDrivePath,
    IN DWORD dwLaunchDataType,
    IN DWORD dwTitleId,
    IN PLAUNCH_DATA pLaunchData
    );
#define XWriteTitleInfoAndReboot XWriteTitleInfoAndRebootA

;end_internal

;begin_internal
//
// Register or deregister a notification routine
// which will be called when a thread is created or deleted.
//
// NOTE: The XTHREAD_NOTIFICATION structure must remain
// valid until the thread notification routine is deregistered.
// For example, you can use a global variable for this.
// But you should NOT use a local variable inside a function.
//
;end_internal

typedef VOID (WINAPI *XTHREAD_NOTIFY_PROC)(BOOL fCreate);
typedef struct _XTHREAD_NOTIFICATION {
    LIST_ENTRY ListEntry;
    XTHREAD_NOTIFY_PROC pfnNotifyRoutine;
} XTHREAD_NOTIFICATION, *PXTHREAD_NOTIFICATION;

XBOXAPI
VOID
WINAPI
XRegisterThreadNotifyRoutine(
    IN OUT PXTHREAD_NOTIFICATION pThreadNotification,
    IN BOOL fRegister
    );

XBOXAPI
VOID
WINAPI
XSetProcessQuantumLength(
    IN DWORD dwMilliseconds
    );

XBOXAPI
DWORD
WINAPI
XGetProcessQuantumLength(
    VOID
    );

XBOXAPI
BOOL
WINAPI
XSetFileCacheSize(
    IN SIZE_T dwCacheSize
    );

XBOXAPI
SIZE_T
WINAPI
XGetFileCacheSize(
    VOID
    );

XBOXAPI
LPVOID
WINAPI
XPhysicalAlloc(
    IN SIZE_T dwSize,
    IN ULONG_PTR ulPhysicalAddress,
    IN ULONG_PTR ulAlignment,
    IN DWORD flProtect
    );

XBOXAPI
SIZE_T
WINAPI
XPhysicalSize(
    IN LPVOID lpAddress
    );

XBOXAPI
VOID
WINAPI
XPhysicalProtect(
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD flNewProtect
    );

XBOXAPI
VOID
WINAPI
XPhysicalFree(
    IN LPVOID lpAddress
    );

XBOXAPI
DWORD
WINAPI
XQueryMemoryProtect(
    IN LPVOID lpAddress
    );

#ifdef _DEBUG

#define XVER_DEVKIT 1
#define XVER_RETAIL 2

XBOXAPI
DWORD
WINAPI
XDebugGetSystemVersionA(
    OUT LPSTR pszVersionString,
    IN UINT cchVersionString
    );
#define XDebugGetSystemVersion XDebugGetSystemVersionA

XBOXAPI
DWORD
WINAPI
XDebugGetXTLVersionA(
    OUT LPSTR pszVersionString,
    IN UINT cchVersionString
    );
#define XDebugGetXTLVersion XDebugGetXTLVersionA

#endif // _DEBUG

#define XCALCSIG_SIGNATURE_SIZE         20

typedef struct _XCALCSIG_SIGNATURE {
    BYTE Signature[XCALCSIG_SIGNATURE_SIZE];
} XCALCSIG_SIGNATURE, *PXCALCSIG_SIGNATURE;

#define XCALCSIG_FLAG_NON_ROAMABLE      (0x00000001)

XBOXAPI
HANDLE
WINAPI
XCalculateSignatureBegin(
    IN DWORD dwFlags
    );

XBOXAPI
HANDLE
WINAPI
XCalculateSignatureBeginEx(
    IN DWORD dwFlags,
    IN DWORD dwAltTitleId
    );

XBOXAPI
DWORD
WINAPI
XCalculateSignatureUpdate(
    IN HANDLE hCalcSig,
    IN const BYTE *pbData,
    IN ULONG cbData
    );

XBOXAPI
DWORD
WINAPI
XCalculateSignatureEnd(
    IN HANDLE hCalcSig,
    OUT PXCALCSIG_SIGNATURE pSignature
    );

XBOXAPI
ULONG 
WINAPI
XAutoPowerDownTimeRemaining();

;begin_both
#ifdef __cplusplus
}
#endif
;end_both

#endif  // _XBOXP_  ;internal

#endif // _XBOX_
