

//
// All APIs unique to Xbox that live in XAPILIB.LIB should be
// defined in this header file
//

#ifndef _XBOXP_
#define _XBOXP_

#define XBOXAPI

#ifdef __cplusplus
extern "C" {
#endif	
#define MAX_SONG_NAME       32
#define MAX_SOUNDTRACK_NAME 32
#define MAX_SOUNDTRACKS     100
#define MAX_SONGS_IN_SNDTRK 500

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


#define XC_LANGUAGE_UNKNOWN         0
#define XC_AV_PACK_NONE             7
#define XC_VIDEO_STANDARD_PAL_M     4
#define XC_GAME_REGION_NA             0x00000001
#define XC_GAME_REGION_JAPAN          0x00000002
#define XC_GAME_REGION_RESTOFWORLD    0x00000004
#define XC_GAME_REGION_MANUFACTURING  0x80000000

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

#ifdef _XBOX_ //only define this if xbox.h was already included.
extern XPP_DEVICE_TYPE XDEVICE_TYPE_IR_REMOTE_TABLE;
#define  XDEVICE_TYPE_IR_REMOTE       (&XDEVICE_TYPE_IR_REMOTE_TABLE)
extern   XPP_DEVICE_TYPE XDEVICE_TYPE_DVD_CODE_SERVER_TABLE;
#define  XDEVICE_TYPE_DVD_CODE_SERVER (&XDEVICE_TYPE_DVD_CODE_SERVER_TABLE)
#endif
extern BOOL XPP_XInitDevicesHasBeenCalled;

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

XBOXAPI
DWORD
WINAPI
XMountMURootA(
    IN DWORD dwPort,
    IN DWORD dwSlot,
    OUT PCHAR pchDrive
    );
#define XMountMURoot XMountMURootA


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

#define XINIT_MOUNT_UTILITY_DRIVE   0x00000001
#define XINIT_FORMAT_UTILITY_DRIVE  0x00000002
#define XINIT_LIMIT_DEVKIT_MEMORY   0x00000004
#define XINIT_NO_SETUP_HARD_DISK    0x00000008
#define XINIT_DONT_MODIFY_HARD_DISK 0x00000010
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


#ifndef MAX_LAUNCH_DATA_SIZE
#define MAX_LAUNCH_DATA_SIZE 3072

typedef struct _LAUNCH_DATA
{
    BYTE Data[MAX_LAUNCH_DATA_SIZE];
} LAUNCH_DATA, *PLAUNCH_DATA;
#endif // ! MAX_LAUNCH_DATA_SIZE
#define XLD_LAUNCH_DASHBOARD_BOOT       5 // Not supported at this time

#define XLD_LAUNCH_DASHBOARD_NETWORK_CONFIGURATION	6
#define XLD_LAUNCH_DASHBOARD_NEW_ACCOUNT_SIGNUP         7 
#define XLD_LAUNCH_DASHBOARD_MESSAGE_SERVER_INFO        8
#define XLD_LAUNCH_DASHBOARD_POLICY_DOCUMENT            9
#define XLD_LAUNCH_DASHBOARD_ONLINE_MENU		10
#define XLD_LAUNCH_DASHBOARD_FORCED_NAME_CHANGE         11
#define XLD_LAUNCH_DASHBOARD_FORCED_BILLING_EDIT        12
//
// When the dwReason is XLD_LAUNCH_DASHBOARD_POLICY_DOCUMENT,
// LD_LAUNCH_DASHBOARD.dwParameter1 will have 1 or probably more of 
// the following flags set.
//
#define XLD_POLICY_SUBSCRIPTION_AGREEMENT 0x01
#define XLD_POLICY_TERMS_OF_USE           0x02
#define XLD_POLICY_CODE_OF_CONDUCT        0x04 
#define XLD_POLICY_PRIVACY_STATEMENT      0x08

typedef struct _LD_FROM_TITLE_UPDATE
{
    DWORD   dwContext;
    HRESULT hr;
    BYTE    Reserved[MAX_LAUNCH_DATA_SIZE - 8];
} LD_FROM_TITLE_UPDATE, *PLD_FROM_TITLE_UPDATE;

#define LDT_LAUNCH_DASHBOARD      1
#define LDT_TITLE_UPDATE          4
#define LDT_FROM_TITLE_UPDATE     6

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


//
// Register or deregister a notification routine
// which will be called when a thread is created or deleted.
//
// NOTE: The XTHREAD_NOTIFICATION structure must remain
// valid until the thread notification routine is deregistered.
// For example, you can use a global variable for this.
// But you should NOT use a local variable inside a function.
//
#ifdef __cplusplus
}
#endif
#endif  // _XBOXP_
