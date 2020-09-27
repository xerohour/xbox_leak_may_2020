/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#ifndef __XONLINE__
#define __XONLINE__

#ifdef __cplusplus
extern "C" {
#endif

//@@BEGIN_MSINTERNAL
//@@BEGIN_CLIENTONLY
//@@END_MSINTERNAL

// ====================================================================
// XOnline Startup & Cleanup
//

typedef struct {

    DWORD           dwReserved;

} XONLINE_STARTUP_PARAMS, * PXONLINE_STARTUP_PARAMS;

XBOXAPI
HRESULT 
WINAPI 
XOnlineStartup(
    IN PXONLINE_STARTUP_PARAMS  pxosp
    );

XBOXAPI
HRESULT
WINAPI 
XOnlineCleanup();

//@@BEGIN_MSINTERNAL
//@@END_CLIENTONLY
//@@END_MSINTERNAL

// ====================================================================
// XOnline Error Codes
//

#define FACILITY_XONLINE            21

// Generic Errors
#define XONLINE_E_OVERFLOW                             _HRESULT_TYPEDEF_(0x80150001L)
#define XONLINE_E_NO_SESSION                           _HRESULT_TYPEDEF_(0x80150002L)
#define XONLINE_E_USER_NOT_LOGGED_ON                   _HRESULT_TYPEDEF_(0x80150003L)
#define XONLINE_E_NO_GUEST_ACCESS                      _HRESULT_TYPEDEF_(0x80150004L)
#define XONLINE_E_NOT_INITIALIZED                      _HRESULT_TYPEDEF_(0x80150005L)
#define XONLINE_E_NO_USER                              _HRESULT_TYPEDEF_(0x80150006L)
#define XONLINE_E_INTERNAL_ERROR                       _HRESULT_TYPEDEF_(0x80150007L)
#define XONLINE_E_OUT_OF_MEMORY                        _HRESULT_TYPEDEF_(0x80150008L)
#define XONLINE_E_TASK_BUSY		                       _HRESULT_TYPEDEF_(0x80150009L)
#define XONLINE_E_SERVER_ERROR	                       _HRESULT_TYPEDEF_(0x8015000AL)
#define XONLINE_E_IO_ERROR	                           _HRESULT_TYPEDEF_(0x8015000BL)
#define XONLINE_E_BAD_CONTENT_TYPE                     _HRESULT_TYPEDEF_(0x8015000CL)


// XOnlineLogon task successful return states
#define XONLINE_S_LOGON_CONNECTION_ESTABLISHED         _HRESULT_TYPEDEF_(0x00000002L)

// XOnlineLogon task failure return values
#define XONLINE_E_LOGON_NO_NETWORK_CONNECTION          _HRESULT_TYPEDEF_(0x80151001L)
#define XONLINE_E_LOGON_NO_IP_ADDRESS                  _HRESULT_TYPEDEF_(0x80151002L)
#define XONLINE_E_LOGON_NO_DNS_SERVICE                 _HRESULT_TYPEDEF_(0x80151003L)
#define XONLINE_E_LOGON_DNS_LOOKUP_FAILED              _HRESULT_TYPEDEF_(0x80151004L)
#define XONLINE_E_LOGON_DNS_LOOKUP_TIMEDOUT            _HRESULT_TYPEDEF_(0x80151005L)
#define XONLINE_E_LOGON_INVALID_XBOX_ONLINE_INFO       _HRESULT_TYPEDEF_(0x80151006L)
#define XONLINE_E_LOGON_MACS_FAILED                    _HRESULT_TYPEDEF_(0x80151007L)
#define XONLINE_E_LOGON_MACS_TIMEDOUT                  _HRESULT_TYPEDEF_(0x80151008L)
#define XONLINE_E_LOGON_AUTHENTICATION_FAILED          _HRESULT_TYPEDEF_(0x80151009L)
#define XONLINE_E_LOGON_AUTHENTICATION_TIMEDOUT        _HRESULT_TYPEDEF_(0x8015100AL)
#define XONLINE_E_LOGON_AUTHORIZATION_FAILED           _HRESULT_TYPEDEF_(0x8015100BL)
#define XONLINE_E_LOGON_AUTHORIZATION_TIMEDOUT         _HRESULT_TYPEDEF_(0x8015100CL)
#define XONLINE_E_LOGON_XBOX_ACCOUNT_INVALID           _HRESULT_TYPEDEF_(0x8015100DL)
#define XONLINE_E_LOGON_XBOX_ACCOUNT_BANNED            _HRESULT_TYPEDEF_(0x8015100EL)
#define XONLINE_E_LOGON_UPDATE_REQUIRED                _HRESULT_TYPEDEF_(0x8015100FL)
#define XONLINE_E_LOGON_SG_SERVICE_FAILED              _HRESULT_TYPEDEF_(0x80151010L)
#define XONLINE_E_LOGON_SG_SERVICE_TIMEDOUT            _HRESULT_TYPEDEF_(0x80151011L)
#define XONLINE_E_LOGON_PRESENCE_SERVICE_FAILED        _HRESULT_TYPEDEF_(0x80151012L)
#define XONLINE_E_LOGON_PRESENCE_SERVICE_TIMEDOUT      _HRESULT_TYPEDEF_(0x80151013L)
#define XONLINE_E_LOGON_SERVICE_UNAVAILABLE            _HRESULT_TYPEDEF_(0x80151014L)
#define XONLINE_E_LOGON_CONNECTION_LOST                _HRESULT_TYPEDEF_(0x80151015L)

// Service errors after XOnlineLogon task completion
#define XONLINE_E_LOGON_SERVICE_NOT_REQUESTED          _HRESULT_TYPEDEF_(0x80151020L)
#define XONLINE_E_LOGON_SERVICE_NOT_AUTHORIZED         _HRESULT_TYPEDEF_(0x80151021L)
#define XONLINE_E_LOGON_NO_SUBSCRIPTION                _HRESULT_TYPEDEF_(0x80151022L)

// User errors after XOnlineLogon task completion
#define XONLINE_E_LOGON_USER_ACCOUNT_INVALID           _HRESULT_TYPEDEF_(0x80151030L)
#define XONLINE_E_LOGON_USER_ACCOUNT_BANNED            _HRESULT_TYPEDEF_(0x80151031L)
#define XONLINE_E_LOGON_USER_ACCOUNT_SUSPENDED         _HRESULT_TYPEDEF_(0x80151032L)
#define XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT      _HRESULT_TYPEDEF_(0x80151033L)

// Other generic auth realted errors
#define XONLINE_E_LOGON_MU_NOT_MOUNTED                 _HRESULT_TYPEDEF_(0x80151040L)
#define XONLINE_E_LOGON_MU_IO_ERROR                    _HRESULT_TYPEDEF_(0x80151041L)
#define XONLINE_E_LOGON_NOT_LOGGED_ON                  _HRESULT_TYPEDEF_(0x80151042L)

// Errors returned by Presence/Notification
#define XONLINE_S_NOTIFICATION_UPTODATE                _HRESULT_TYPEDEF_(0x00000003L)
#define XONLINE_S_NOTIFICATION_FRIEND_RESULT           _HRESULT_TYPEDEF_(0x00000004L)
#define XONLINE_E_NOTIFICATION_BAD_CONTENT_TYPE        _HRESULT_TYPEDEF_(0x80155000L)
#define XONLINE_E_NOTIFICATION_REQUEST_TOO_SMALL       _HRESULT_TYPEDEF_(0x80155001L)
#define XONLINE_E_NOTIFICATION_INVALID_MESSAGE_TYPE    _HRESULT_TYPEDEF_(0x80155002L)
#define XONLINE_E_NOTIFICATION_NO_ADDRESS              _HRESULT_TYPEDEF_(0x80155003L)
#define XONLINE_E_NOTIFICATION_INVALID_PUID            _HRESULT_TYPEDEF_(0x80155004L)
#define XONLINE_E_NOTIFICATION_NO_CONNECTION           _HRESULT_TYPEDEF_(0x80155005L)
#define XONLINE_E_NOTIFICATION_SEND_FAILED             _HRESULT_TYPEDEF_(0x80155006L)
#define XONLINE_E_NOTIFICATION_RECV_FAILED             _HRESULT_TYPEDEF_(0x80155007L)
#define XONLINE_E_NOTIFICATION_MESSAGE_TRUNCATED       _HRESULT_TYPEDEF_(0x80155008L)
#define XONLINE_E_NOTIFICATION_SERVER_BUSY             _HRESULT_TYPEDEF_(0x80152001L)
#define XONLINE_E_NOTIFICATION_LIST_FULL               _HRESULT_TYPEDEF_(0x80152002L)
#define XONLINE_E_NOTIFICATION_BLOCKED                 _HRESULT_TYPEDEF_(0x80152003L)
#define XONLINE_E_NOTIFICATION_FRIEND_PENDING          _HRESULT_TYPEDEF_(0x80152004L)
#define XONLINE_E_NOTIFICATION_FLUSH_TICKETS           _HRESULT_TYPEDEF_(0x80152005L)

//  Errors returned by matchmaking
#define XONLINE_E_MATCH_INVALID_SESSION_ID             _HRESULT_TYPEDEF_(0x80155100L)  // specified session id does not exist
#define XONLINE_E_MATCH_INVALID_TITLE_ID               _HRESULT_TYPEDEF_(0x80155101L)  // specified title id is zero, or does not exist
#define XONLINE_E_MATCH_INVALID_DATA_TYPE              _HRESULT_TYPEDEF_(0x80155102L)  // attribute ID or parameter type specifies an invalid data type
#define XONLINE_E_MATCH_REQUEST_TOO_SMALL              _HRESULT_TYPEDEF_(0x80155103L)  // the request did not meet the minimum length for a valid request
#define XONLINE_E_MATCH_REQUEST_TRUNCATED              _HRESULT_TYPEDEF_(0x80155104L)  // the self described length is greater than the actual buffer size
#define XONLINE_E_MATCH_INVALID_SEARCH_REQ             _HRESULT_TYPEDEF_(0x80155105L)  // the search request was invalid
#define XONLINE_E_MATCH_INVALID_OFFSET                 _HRESULT_TYPEDEF_(0x80155106L)  // one of the attribute/parameter offsets in the request was invalid.  Will be followed by the zero based offset number.
#define XONLINE_E_MATCH_INVALID_ATTR_TYPE              _HRESULT_TYPEDEF_(0x80155107L)  // the attribute type was something other than user or session
#define XONLINE_E_MATCH_INVALID_VERSION                _HRESULT_TYPEDEF_(0x80155108L)  // bad protocol version in request
#define XONLINE_E_MATCH_OVERFLOW                       _HRESULT_TYPEDEF_(0x80155109L)  // an attribute or parameter flowed past the end of the request
#define XONLINE_E_MATCH_INVALID_RESULT_COL             _HRESULT_TYPEDEF_(0x8015510AL)  // referenced stored procedure returned a column with an unsupported data type
#define XONLINE_E_MATCH_INVALID_STRING                 _HRESULT_TYPEDEF_(0x8015510BL)  // string with length-prefix of zero, or string with no terminating null
#define XONLINE_E_MATCH_STRING_TOO_LONG                _HRESULT_TYPEDEF_(0x8015510CL)  // string exceeded 400 characters
#define XONLINE_E_MATCH_BLOB_TOO_LONG                  _HRESULT_TYPEDEF_(0x8015510DL)  // blob exceeded 800 bytes
#define XONLINE_E_MATCH_PUID_MISMATCH                  _HRESULT_TYPEDEF_(0x8015510EL)  // PUIDS did not match in attribute when added/changing players in a session
#define XONLINE_E_MATCH_INVALID_PLAYER                 _HRESULT_TYPEDEF_(0x8015510FL)  // player is not in the current session
#define XONLINE_E_MATCH_INVALID_ATTRIBUTE_ID           _HRESULT_TYPEDEF_(0x80155110L)  // attribute id is invalid
#define XONLINE_E_MATCH_PLAYER_ALREADY_EXISTS          _HRESULT_TYPEDEF_(0x80155111L)  // player already exists in the db
#define XONLINE_E_MATCH_SESSION_ALREADY_EXISTS         _HRESULT_TYPEDEF_(0x80155112L)  // session id already exists in the db
#define XONLINE_E_MATCH_NO_TITLES                      _HRESULT_TYPEDEF_(0x80155113L)  // no titles in the db
#define XONLINE_E_MATCH_TITLE_ALREADY_EXISTS           _HRESULT_TYPEDEF_(0x80155114L)  // title already exists in the db
#define XONLINE_E_MATCH_CRITICAL_DB_ERR                _HRESULT_TYPEDEF_(0x80155115L)  // critical error in db
#define XONLINE_E_MATCH_NOT_ENOUGH_COLUMNS             _HRESULT_TYPEDEF_(0x80155116L)  // search result set had too few columns
#define XONLINE_E_MATCH_PERMISSION_DENIED              _HRESULT_TYPEDEF_(0x80155117L)  // incorrect permissions set on search sp
#define XONLINE_E_MATCH_INVALID_PART_SCHEME            _HRESULT_TYPEDEF_(0x80155118L)  // title specified an invalid partitioning scheme
#define XONLINE_E_MATCH_INVALID_PARAM                  _HRESULT_TYPEDEF_(0x80155119L)  // bad parameter passed to sp
#define XONLINE_E_MATCH_USE_SETPLAYER_PROC             _HRESULT_TYPEDEF_(0x8015511AL)  // attempt to use p_match_set_int_attribute to add/change player in session
#define XONLINE_E_MATCH_PLAYER_REQUIRED                _HRESULT_TYPEDEF_(0x8015511BL)  // attempt to set a per-player attribute with invalid PUID
#define XONLINE_E_MATCH_PLAYER_NOT_NEEDED              _HRESULT_TYPEDEF_(0x8015511CL)  // attempt to set a session attribute with non-zero PUID
#define XONLINE_E_MATCH_DATA_TYPE_MISMATCH             _HRESULT_TYPEDEF_(0x8015511DL)  // data type specified in attr id did not match type of attr being set
#define XONLINE_E_MATCH_SERVER_ERROR                   _HRESULT_TYPEDEF_(0x8015511EL)  // error on server not correctable by client
#define XONLINE_E_MATCH_NO_USERS                       _HRESULT_TYPEDEF_(0x8015511FL)  // no authenticated users in search request.
#define XONLINE_E_MATCH_INVALID_BLOB                   _HRESULT_TYPEDEF_(0x80155120L)  // invalid blob attribute

// Errors returned by billing/offerings
#define XONLINE_E_OFFERING_BAD_REQUEST                 _HRESULT_TYPEDEF_(0x80153001L)  // server received incorrectly formatted request 
#define XONLINE_E_OFFERING_INVALID_USER                _HRESULT_TYPEDEF_(0x80153002L)  // cannot find account for this user
#define XONLINE_E_OFFERING_INVALID_OFFER_ID            _HRESULT_TYPEDEF_(0x80153003L)  // offer does not exist
#define XONLINE_E_OFFERING_INELIGIBLE_FOR_OFFER        _HRESULT_TYPEDEF_(0x80153004L)  // user/title not allowed to purchase offer
#define XONLINE_E_OFFERING_OFFER_EXPIRED               _HRESULT_TYPEDEF_(0x80153005L)  // offer no longer available
#define XONLINE_E_OFFERING_SERVICE_UNREACHABLE         _HRESULT_TYPEDEF_(0x80153006L)  // apparent connectivity problems
#define XONLINE_E_OFFERING_PURCHASE_BLOCKED            _HRESULT_TYPEDEF_(0x80153007L)  // this user is not allowed to make purchases
#define XONLINE_E_OFFERING_PURCHASE_DENIED             _HRESULT_TYPEDEF_(0x80153008L)  // this user's payment is denied by billing provider
#define XONLINE_E_OFFERING_BILLING_SERVER_ERROR        _HRESULT_TYPEDEF_(0x80153009L)  // nonspecific billing provider error
#define XONLINE_E_OFFERING_OFFER_NOT_CANCELABLE        _HRESULT_TYPEDEF_(0x8015300AL)  // either this offer doesn't exist, or it's marked as un-cancelable
#define XONLINE_E_OFFERING_NOTHING_TO_CANCEL           _HRESULT_TYPEDEF_(0x8015300BL)  // this user doesn't have one of these anyways
#define XONLINE_E_OFFERING_PERMISSION_DENIED           _HRESULT_TYPEDEF_(0x80154001L)  // permission denied


//@@BEGIN_MSINTERNAL
// ====================================================================
// Localization defines

typedef enum {
    LANGUAGE_CURRENT  = -1,
    LANGUAGE_ENGLISH  = 0,
    LANGUAGE_JAPANESE,
    LANGUAGE_GERMAN,
    LANGUAGE_FRENCH,
    LANGUAGE_SPANISH,
    LANGUAGE_ITALIAN
} XONLINE_LANGUAGES;


// ====================================================================
// Localization defines

#define XOnlineIsOnlineError(hr) (((hr) & 0xFFFF0000L) == 0x80150000L)

#define LANGUAGE_COUNT                6

//@@END_MSINTERNAL



//@@BEGIN_MSINTERNAL

// ====================================================================
// SGADDR

#pragma pack(push, 1)

typedef struct {
    IN_ADDR         inaSg;              // IP address of the SG for the client
    DWORD           dwSpiSg;            // Pseudo-random identifier assigned by the SG
    ULONGLONG       qwXboxID;           // Unique identifier of client machine account
    BYTE            abReserved[4];      // Reserved (must be zero)
} SGADDR;

#pragma pack(pop)

//@@END_MSINTERNAL

//@@BEGIN_MSINTERNAL
//@@BEGIN_CLIENTONLY
//@@END_MSINTERNAL

// ====================================================================
// XOnline Task Pump
//

DECLARE_HANDLE(XONLINETASK_HANDLE);
typedef XONLINETASK_HANDLE* PXONLINETASK_HANDLE;

//
// Define the two global state values
//
#define XONLINETASK_S_RUNNING                   (S_OK)
#define XONLINETASK_S_SUCCESS                   (S_FALSE)

XBOXAPI
HRESULT 
WINAPI
XOnlineTaskContinue(
    IN XONLINETASK_HANDLE hTask
    );

XBOXAPI
HRESULT 
WINAPI
XOnlineTaskClose(
    IN XONLINETASK_HANDLE hTask
    );

//@@BEGIN_MSINTERNAL
//@@END_CLIENTONLY
//@@END_MSINTERNAL


// ====================================================================
// XOnline Authentication
//

#define XONLINE_NAME_SIZE                       16
#define XONLINE_MAX_NAME_LENGTH                 (XONLINE_NAME_SIZE - 1)
#define XONLINE_KINGDOM_SIZE                    12
#define XONLINE_MAX_KINGDOM_LENGTH              (XONLINE_KINGDOM_SIZE - 1)
#define XONLINE_USERNAME_SIZE                   28 // (XONLINE_NAME_SIZE + XONLINE_KINGDOM_SIZE)
#define XONLINE_USERDOMAIN_SIZE                 20
#define XONLINE_MAX_USERDOMAIN_LENGTH           (XONLINE_USERDOMAIN_SIZE - 1)
#define XONLINE_REALM_NAME_SIZE                 24
#define XONLINE_MAX_REALM_NAME_LENGTH           (XONLINE_REALM_NAME_SIZE - 1)
#define XONLINE_MAX_TICKET_LENGTH             1024
#define XONLINE_KEY_LENGTH                      16
#define XONLINE_PIN_LENGTH                       4
#define XONLINE_MAX_LOGON_USERS                  4

//
// Services that can be passed to XOnlineLogon
//
#define XONLINE_BILLING_OFFERING_SERVICE    ((DWORD)4)
#define XONLINE_MATCHMAKING_SERVICE         ((DWORD)6)
#define XONLINE_STATISTICS_SERVICE          ((DWORD)7)
#define XONLINE_INVALID_SERVICE             ((DWORD)0)

//
// Used in dwUserFlags in XUID
//
#define XONLINE_USER_GUEST_MASK             0x00000003
#define XONLINE_USER_VOICE_NOT_ALLOWED      0x00010000
#define XONLINE_USER_PURCHASE_NOT_ALLOWED   0x00020000
#define XONLINE_USER_LOCKED_OUT             0x00040000;
#define XONLINE_USER_NICKNAME_NOT_ALLOWED   0x00080000;
#define XONLINE_USER_MUST_CHANGE_NAME       0x00100000;

#define XOnlineIsUserVoiceAllowed(dwUserFlags) (((dwUserFlags) & XONLINE_USER_VOICE_NOT_ALLOWED) == 0)

#define XOnlineIsUserPurchaseAllowed(dwUserFlags) (((dwUserFlags) & XONLINE_USER_PURCHASE_NOT_ALLOWED) == 0)

#define XOnlineIsUserLockedOut(dwUserFlags) (((dwUserFlags) & XONLINE_USER_LOCKED_OUT) != 0)

#define XOnlineIsUserNicknameAllowed(dwUserFlags) (((dwUserFlags) & XONLINE_USER_NICKNAME_NOT_ALLOWED) == 0)

#define XOnlineUserMustChangeName(dwUserFlags) (((dwUserFlags) & XONLINE_USER_MUST_CHANGE_NAME) != 0)

#define XOnlineIsUserGuest(dwUserFlags) (((dwUserFlags) & XONLINE_USER_GUEST_MASK) != 0)

#define XOnlineUserGuestNumber(dwUserFlags) ((dwUserFlags) & XONLINE_USER_GUEST_MASK)

#define XOnlineSetUserGuestNumber(dwUserFlags,guestNumber) ((dwUserFlags) |= ((guestNumber) & XONLINE_USER_GUEST_MASK))

#pragma pack(push, 4)

typedef struct {
    ULONGLONG qwUserID;
    DWORD dwUserFlags;
} XUID;

// 
//  Type of a offering ID
//
typedef DWORD    XONLINEOFFERING_ID;

//@@BEGIN_MSINTERNAL
//@@BEGIN_CLIENTONLY
//@@END_MSINTERNAL

#define XONLINE_USER_RESERVED_SIZE              72
#define XONLINE_MAX_STORED_ONLINE_USERS         16

DECLARE_HANDLE(XPININPUTHANDLE);

//
// Used in dwUserOptions in XONLINE_USER
//
#define XONLINE_USER_OPTION_REQUIRE_PIN     0x00000001
#define XONLINE_USER_OPTION_CAME_FROM_MU    0x80000000
#define XONLINE_USER_OPTION_MU_PORT_MASK    0x60000000
#define XONLINE_USER_OPTION_MU_PORT_SHIFT           29
#define XONLINE_USER_OPTION_MU_SLOT_MASK    0x10000000
#define XONLINE_USER_OPTION_MU_SLOT_SHIFT           28

typedef struct {
    XUID xuid;
    CHAR name[XONLINE_NAME_SIZE];
    CHAR kingdom[XONLINE_KINGDOM_SIZE];
    DWORD dwUserOptions;
    BYTE pin[XONLINE_PIN_LENGTH];
    BYTE reserved[XONLINE_USER_RESERVED_SIZE];
    HRESULT hr;
    DWORD index;
} XONLINE_USER, *PXONLINE_USER;

typedef struct {
    DWORD          dwServiceID;
    IN_ADDR        serviceIP;
    WORD           wServicePort;
    WORD           wReserved;
} XONLINE_SERVICE_INFO, *PXONLINE_SERVICE_INFO;

#pragma pack(pop)

XBOXAPI
HRESULT
WINAPI
XOnlineGetUsers (
    OUT PXONLINE_USER pUsers,
    OUT DWORD *pcUsers
    );

XBOXAPI
XPININPUTHANDLE
WINAPI
XOnlinePINStartInput(
    IN PXINPUT_STATE pInputState
    );

XBOXAPI
HRESULT
WINAPI
XOnlinePINDecodeInput(
    IN XPININPUTHANDLE handle,
    IN PXINPUT_STATE pInputState,
    IN PBYTE pPINByte
    );

XBOXAPI
HRESULT
WINAPI
XOnlinePINEndInput (
    IN XPININPUTHANDLE handle
    );

XBOXAPI
HRESULT
WINAPI
XOnlineLogon(
    IN PXONLINE_USER pUsers,
    IN DWORD* pdwServiceIDs,
    IN DWORD cServices,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE pHandle
    );

XBOXAPI
HRESULT
WINAPI
XOnlineLogonTaskGetResults(
    IN XONLINETASK_HANDLE hLogonTask
    );
    
XBOXAPI
PXONLINE_USER
WINAPI
XOnlineGetLogonUsers();

XBOXAPI
HRESULT
WINAPI
XOnlineGetServiceInfo(
    IN DWORD dwServiceID,
    OUT PXONLINE_SERVICE_INFO pServiceInfo
    );

// ====================================================================
// XOnline Title Update (Security updates)
//

// ====================================================================
// XOnline Offerings
//

#define XO_CURRENCY_EUR     1
#define XO_CURRENCY_GBP     2
#define XO_CURRENCY_JPY     4
#define XO_CURRENCY_KRW     8

// 
//  Type of a offering ID
//

typedef DWORD    XONLINEOFFERING_ID;


typedef struct {
    DWORD dwWholePart;
    DWORD dwFractionalPart;
    BYTE  bCurrencyFormat;
    WCHAR rgchISOCurrencyCode[3];
} XONLINE_PRICE, *PXONLINE_PRICE;


XBOXAPI
HRESULT 
WINAPI 
XOnlineOfferingPurchase( 
    IN DWORD dwUserIndex, 
    IN XONLINEOFFERING_ID OfferingId, 
    IN HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask 
    );

XBOXAPI
HRESULT 
WINAPI 
XOnlineOfferingCancel( 
    IN DWORD dwUserIndex, 
    IN XONLINEOFFERING_ID OfferingId, 
    IN HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask 
    );

XBOXAPI
HRESULT
WINAPI
XOnlineOfferingGetDetails ( 
    IN DWORD dwUserIndex,
    IN XONLINEOFFERING_ID OfferingId,
    IN WORD wLanguage,
    IN DWORD dwDescriptionIndex,
    IN PBYTE pbBuffer,
    IN DWORD cbBuffer,
    IN HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask 
    );

XBOXAPI
HRESULT
WINAPI 
XOnlineOfferingDetailsGetResults ( 
    IN XONLINETASK_HANDLE hTask,
    OUT PBYTE *pDetailsBuffer,
    OUT DWORD *pcbDetailsLength,
    OUT XONLINE_PRICE *pGrossPrice,
    OUT XONLINE_PRICE *pNetPrice 
    );

XBOXAPI
DWORD
WINAPI
XOnlineOfferingDetailsMaxSize(
    IN DWORD cbTitleSpecificDataMaxSize
    );

XBOXAPI
HRESULT
WINAPI
XOnlineOfferingPriceFormat( 
    IN XONLINE_PRICE *Price,
    IN OUT LPWSTR    lpwszFormattedPrice,
    IN OUT DWORD     *cbLength,
    IN DWORD         dwExtendedCharsFilter
    );


//@@BEGIN_MSINTERNAL
// (..for now, as we only expect this to be used via XOnlineContentVerify)
//

// user flags
#define XONLINE_LOGON_CONTROLLER1    0x0001   // logged-in user at controller 1
#define XONLINE_LOGON_CONTROLLER2    0x0002   // logged-in user at controller 2
#define XONLINE_LOGON_CONTROLLER3    0x0004   // logged-in user at controller 3
#define XONLINE_LOGON_CONTROLLER4    0x0008   // logged-in user at controller 4
#define XONLINE_LOGON_MACHINE        0x0010   // machine account
#define XONLINE_LOGON_ALL            0xffff   // machine and all logged-in users


// Function used to verify rights.   Will return
// success if any of the currently logged-in users, 
// or the machine, have rights to access the given
// offering id.
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineOfferingVerifyLicense( 
    DWORD dwOfferingId, 
    DWORD dwUserAccounts,
    HANDLE hWorkEvent,
    XONLINETASK_HANDLE *phTask 
    );


// ====================================================================
// XOnline Content Delivery
//
XBOXAPI
HRESULT
WINAPI
XOnlineContentInstallFromDVD(
    IN PSTR szResourcePath,
    IN DWORD dwTitleId,
    IN XONLINEOFFERING_ID OfferingId,
    IN DWORD dwBitFlags,
    IN PBYTE pbSymmetricKey,
    IN DWORD cbSymmetricKey,
    IN PBYTE pbPublicKey,
    IN DWORD cbPublicKey,
    IN HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

// ====================================================================
// Autoupdate
//

typedef struct
{
    DWORD dwReason;
    DWORD dwContext;
    DWORD dwParameter1;
    DWORD dwParameter2;
    DWORD dwTitleId;
    DWORD dwBaseVersion;
    DWORD dwUpdateFlags;
    BYTE  rgbTitleKey[16];
    BYTE  rgbSymmetricKey[16];
    BYTE  rgbPublicKey[284];
    DWORD cbPublicKey;
    CHAR  szUpdatePath[MAX_PATH];
} LAUNCH_UPDATE_INFO, *PLAUNCH_UPDATE_INFO;

#define LAUNCH_UPDATE_FLAG_ONLINE	        ((DWORD)0x00000001)
#define LAUNCH_UPDATE_FLAG_USE_DEFAULT_KEYS ((DWORD)0x80000000)

#define IS_NEW_DASH_FEATURE(dwReason) (((dwReason) & 0x80000000)?TRUE:FALSE)

XBOXAPI
DWORD
WINAPI
XOnlineLaunchNewImage(
    IN LPCSTR lpImagePath,
    IN PLAUNCH_DATA pLaunchData
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineTitleUpdate(
    IN DWORD dwContext
    );

XBOXAPI
HRESULT 
WINAPI 
XOnlineTitleUpdateFromDVD(
    IN DWORD dwContext,
    IN PSTR szUpdatePath,
    IN DWORD dwTitleId,
    IN PBYTE pbSymmetricKey,
    IN DWORD cbSymmetricKey,
    IN PBYTE pbPublicKey,
    IN DWORD cbPublicKey
    );

XBOXAPI
HRESULT 
WINAPI
XOnlineTitleUpdateInternal(
    DWORD dwTitleId,
    DWORD dwTitleOldVersion,
    PBYTE pbTitleKey,
    HANDLE hWorkEvent,
    PXONLINETASK_HANDLE phTask
    );

XBOXAPI
HRESULT 
WINAPI
XOnlineTitleUpdateFromDVDInternal(
    PSTR szUpdatePath,
    DWORD dwTitleId,
    DWORD dwTitleOldVersion,
    PBYTE pbTitleKey,
    PBYTE pbSymmetricKey,
    DWORD cbSymmetricKey,
    PBYTE pbPublicKey,
    DWORD cbPublicKey,
    HANDLE hWorkEvent,
    PXONLINETASK_HANDLE phTask
    );

XBOXAPI
HRESULT
WINAPI
XOnlineTitleUpdateGetProgress(
    IN XONLINETASK_HANDLE hTask,
    OUT DWORD *pdwPercentDone,
    OUT ULONGLONG *pqwNumerator,
    OUT ULONGLONG *pqwDenominator
    );

//@@END_MSINTERNAL



//@@BEGIN_MSINTERNAL
//@@END_CLIENTONLY
//@@END_MSINTERNAL

// ===============================================================
// Offering enumeration APIs
//


// 
// Offering Type bitflags
//
#define    XONLINE_OFFERING_SUBSCRIPTION        0x1
#define    XONLINE_OFFERING_CONTENT             0x2


//
// The list of devices from which to enumerate offerings
//
typedef enum {
    XONLINEOFFERING_ENUM_DEVICE_DVD = 0,
    XONLINEOFFERING_ENUM_DEVICE_HD,
    XONLINEOFFERING_ENUM_DEVICE_ONLINE,
    XONLINEOFFERING_ENUM_DEVICE_MAX
} XONLINEOFFERING_ENUM_DEVICE, *PXONLINEOFFERING_ENUM_DEVICE;



//
// Offering enumeration parameters struct
//
typedef struct
{
    DWORD       dwOfferingType; // Filter on offering type
    DWORD       dwBitFilter;    // Bitfield for filtering offerings
    DWORD       dwDescriptionIndex;// Publisher-specific index
    DWORD       dwMaxResults;   // Desired max number of results
    FILETIME    ftActiveAfter;  // Return items newer than this date 
} XONLINEOFFERING_ENUM_PARAMS, *PXONLINEOFFERING_ENUM_PARAMS;

//
// Offering enumeration record
//
typedef struct {
    XONLINEOFFERING_ID  OfferingId;             // Offering ID
    DWORD               dwOfferingType;         // Offering type
    DWORD               dwBitFlags;             // Package-specific flags
    DWORD               cbPackageSize;          // Package wire size (bytes)
    DWORD               cbInstallSize;          // Installed size (blocks)
    FILETIME            ftActivationDate;       // Activation date of package
    DWORD               dwRating;               // Package rating
    DWORD               cbTitleSpecificData;    // Size of data blob (bytes)
    PBYTE               pbTitleSpecificData;    // Pointer to data blob
} XONLINEOFFERING_INFO, *PXONLINEOFFERING_INFO;

#define XONLINEOFFERING_ENUM_MAX_TITLE_DATA_SIZE    (8192)

//@@BEGIN_MSINTERNAL
//@@BEGIN_CLIENTONLY
//@@END_MSINTERNAL

//
// Define a special state code for enumeration
//
#define XONLINETASK_S_RESULTS_AVAIL                 ((HRESULT)2001)

//
// Function to kick off an offering enumeration request for the current title. 
//
// The caller must pass in a pEnumParame data structure to specify the enumeration
// parameters:
//   dwOfferingType - The type of offerings the caller is interested in.
//   dwBitFilter - Return offerings that have one or more of the specified bits set
//   dwMaxResults - Hints the number of results the caller wishes the server to 
//          return in each batch. Setting this to zero indicates that the server
//          should try to pack as many results into the supplied client buffer.
//          If the client buffer is not large enough to hold the specified number
//          of records, the server may return less records than specified. Use
//          the XOnlineOfferingMaxSize API to calculate the max buffer size required
//          to hold the desired number of results.
//   ftActiveAfter - Return offerings activated on or after this date 
//          (required for online)
//
// This API requires a work buffer that can either be supplied by the caller, or
// can be allocated as part of the task context (if NULL). cbBuffer specifies the 
// size of the buffer either case. It is the caller's responsibility to ensure that
// the buffer size is large enough to hold at least one offering record.
//
// if XOnlineTaskContinue returns XONLINETASK_S_SUCCESS, then the enumeration has
// completed successfully. The caller should call XOnlineOfferingEnumerateGetResults
// the get the final list of enumeration records. On the other hand, if this function
// returns XONLINETASK_S_RESULTS_AVAIL, this means that a batch of enumeration records
// are available, and that there are more results after these. The caller should also
// call XOnlineOfferingEnumerateGetResults to get the current batch of results, and
// then continue to call XOnlineTaskContinue to get the next batch of results.
//
XBOXAPI
HRESULT 
WINAPI
XOnlineOfferingEnumerate(
    IN XONLINEOFFERING_ENUM_DEVICE Device,
    IN DWORD dwUserIndex,
    IN PXONLINEOFFERING_ENUM_PARAMS pEnumParams,
    IN OPTIONAL PBYTE pbBuffer,
    IN DWORD cbBuffer,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to retrieve the results of an offering enumeration.
//
// Note: Callers will NOT have to free prgpOfferingInfo when they are done,
// but they must call XOnlineTaskClose(hTask) AFTER they are done with
// prgpOfferingInfo.
//
// prgpOfferingInfo returns a pointer to a vector of pointers to 
// XONLINEOFFERING_INFO structures (vs. pointer to an array of pointers to
// structs). i.e. ((*prgpOfferingInfo)[i])->wszName is the name of the ith 
// returned record.
//
// *pfMoreResults will indicate if there are more results available for this
// search criteria.
//
XBOXAPI
HRESULT 
WINAPI
XOnlineOfferingEnumerateGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT PXONLINEOFFERING_INFO **prgpOfferingInfo,
    OUT DWORD *pdwReturnedResults,
    OUT BOOL *pfMoreResults
    );

//
// Utility function to determine the absolute upper bound memory 
// requirements for receiving a specified number of offering entries.
//
// The max size is calculated based on pEnumParams->Device and
// pEnumDevice->dwMaxResults. cbTitleSpecificDataMaxSize is an 
// optional parameter that allows the title to override the max size
// for the rgbTitleSpecificData blob (to conserve memory). If this
// value is zero, then the hard limit for the blob size will be used
// in the calculations (which results in a bigger memory requirement).
//
XBOXAPI
DWORD 
WINAPI
XOnlineOfferingEnumerateMaxSize(
    IN PXONLINEOFFERING_ENUM_PARAMS pEnumParams,
    IN OPTIONAL DWORD cbTitleSpecificDataMaxSize
    );

//
// Function to kick off downloading and installing a
// content package as specified by the ID.
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineContentInstall(
    IN XONLINEOFFERING_ID OfferingId,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to get the progress of an active content installation
//
XBOXAPI
HRESULT
WINAPI
XOnlineContentInstallGetProgress(
    IN XONLINETASK_HANDLE hTask,
    OUT OPTIONAL DWORD *pdwPercentDone,
    OUT OPTIONAL ULONGLONG *pqwNumerator,
    OUT OPTIONAL ULONGLONG *pqwDenominator
    );

//
// Function to kick off verification of a content package
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineContentVerify(
    IN XONLINEOFFERING_ID OfferingId,
    IN OPTIONAL PBYTE pbBuffer,
    IN OUT DWORD *pcbBuffer,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to kick off the removal of a content package
// This blows away everything, including all content data
// under the title area, and user area.
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineContentRemove(
    IN XONLINEOFFERING_ID OfferingId,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Utility function to compute the installation root directory of a
// content package.
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineContentGetRootDirectory(
    IN XONLINEOFFERING_ID OfferingId,
    IN BOOL fUserData,
    IN OUT DWORD *pcbRootDirectory,
    OUT CHAR *szRootDirectory
    );

//@@BEGIN_MSINTERNAL
//@@END_CLIENTONLY
//@@END_MSINTERNAL


// ====================================================================
// XOnline Matchmaking
//

typedef struct _XONLINE_ATTRIBUTE {
    DWORD dwAttributeID;
    BOOL fChanged;
    union {
        struct {
            ULONGLONG      qwValue;
        } integer;
        struct {
            LPWSTR         pwszValue;
        } string;
        struct {
            PVOID          pvValue;
            DWORD          dwLength;
        } blob;
    } info;
} XONLINE_ATTRIBUTE, *PXONLINE_ATTRIBUTE;

typedef struct _XONLINE_ATTRIBUTE_SPEC {
    DWORD            type;
    DWORD            length;
} XONLINE_ATTRIBUTE_SPEC, *PXONLINE_ATTRIBUTE_SPEC;

#define X_MAX_STRING_ATTRIBUTE_LEN         400
#define X_MAX_BLOB_ATTRIBUTE_LEN           800

#define X_ATTRIBUTE_SCOPE_MASK             0x80000000
#define X_ATTRIBUTE_SCOPE_GLOBAL           0x80000000
#define X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC   0x00000000

#define X_ATTRIBUTE_DATATYPE_MASK          0x00F00000
#define X_ATTRIBUTE_DATATYPE_INTEGER       0x00000000
#define X_ATTRIBUTE_DATATYPE_STRING        0x00100000
#define X_ATTRIBUTE_DATATYPE_BLOB          0x00200000

#define X_ATTRIBUTE_ID_MASK                0x0000FFFF

//@@BEGIN_MSINTERNAL
#define X_ATTRIBUTE_DATATYPE_NULL          0x00F00000

#define X_ATTRIBUTE_TYPE_MASK              0x0F000000
#define X_ATTRIBUTE_TYPE_USER              0x01000000
#define X_ATTRIBUTE_TYPE_SESSION           0x00000000

#define X_ATTRIBUTE_USER_PUID               (DWORD)(X_ATTRIBUTE_SCOPE_GLOBAL |      \
                                                    X_ATTRIBUTE_TYPE_USER |         \
                                                    X_ATTRIBUTE_DATATYPE_INTEGER |  \
                                                    0x00000001)

#define X_ATTRIBUTE_RESERVED               ~(X_ATTRIBUTE_SCOPE_MASK | \
                                             X_ATTRIBUTE_DATATYPE_MASK | \
                                             X_ATTRIBUTE_TYPE_MASK | \
                                             X_ATTRIBUTE_ID_MASK)
//@@END_MSINTERNAL

#pragma pack(push, 1)

typedef struct
{
    DWORD  dwResultLength;
    XNKID  SessionID;
    XNADDR HostAddress;
    XNKEY  KeyExchangeKey;
    DWORD  dwPublicAvailable;
    DWORD  dwPrivateAvailable;
    DWORD  dwPublicCurrent;
    DWORD  dwPrivateCurrent;
    DWORD  dwNumAttributes;
} XMATCH_SEARCHRESULT, *PXMATCH_SEARCHRESULT;

#pragma pack(pop)

//@@BEGIN_MSINTERNAL
//@@BEGIN_CLIENTONLY
//@@END_MSINTERNAL

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionCreate(
    IN DWORD dwPublicCurrent,
    IN DWORD dwPublicAvailable,
    IN DWORD dwPrivateCurrent,
    IN DWORD dwPrivateAvailable,
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionUpdate(
    IN XNKID SessionID,
    IN DWORD dwPublicCurrent,
    IN DWORD dwPublicAvailable,
    IN DWORD dwPrivateCurrent,
    IN DWORD dwPrivateAvailable,
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionGetInfo(
    IN XONLINETASK_HANDLE hTask,
    OUT XNKID *pSessionID,
    OUT XNKEY *pKeyExchangeKey
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionDelete(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSessionFindFromID(
    IN XNKID SessionID,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearch(
    IN DWORD dwProcedureIndex,
    IN DWORD dwNumResults,
    IN DWORD dwNumAttributes,
    IN PXONLINE_ATTRIBUTE pAttributes,
    IN DWORD dwResultsLen,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

XBOXAPI
HRESULT 
WINAPI
XOnlineMatchSearchGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT PXMATCH_SEARCHRESULT **prgpSearchResults,
    OUT DWORD *pdwReturnedResults
    );
 
XBOXAPI
HRESULT
WINAPI
XOnlineMatchSearchParse(
    IN PXMATCH_SEARCHRESULT pSearchResult,
    IN DWORD dwNumSessionAttributes,
    IN PXONLINE_ATTRIBUTE_SPEC pSessionAttributeSpec,
    OUT PVOID pQuerySession
    );

// ====================================================================
// Presence & Notification
//

#define XONLINE_NOTIFICATION_TYPE_FRIENDREQUEST    0x00000001
#define XONLINE_NOTIFICATION_TYPE_FRIENDSTATUS     0x00000002
#define XONLINE_NOTIFICATION_TYPE_GAMEINVITE       0x00000004
#define XONLINE_NOTIFICATION_TYPE_GAMEINVITEANSWER 0x00000008
#define XONLINE_NOTIFICATION_TYPE_ALL              0xFFFFFFFF

#define XONLINE_FRIENDSTATE_FLAG_NONE              0x00000000
#define XONLINE_FRIENDSTATE_FLAG_ONLINE            0x00000001
#define XONLINE_FRIENDSTATE_FLAG_PLAYING           0x00000002
#define XONLINE_FRIENDSTATE_FLAG_CLOAKED           0x00000004
#define XONLINE_FRIENDSTATE_FLAG_VOICE             0x00000008
#define XONLINE_FRIENDSTATE_FLAG_JOINABLE          0x00000010
#define XONLINE_FRIENDSTATE_MASK_GUESTS            0x00000060
#define XONLINE_FRIENDSTATE_FLAG_RESERVED0         0x00000080
#define XONLINE_FRIENDSTATE_FLAG_SENTINVITE        0x04000000
#define XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE    0x08000000
#define XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED    0x10000000
#define XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED    0x20000000
#define XONLINE_FRIENDSTATE_FLAG_REQUEST           0x40000000
#define XONLINE_FRIENDSTATE_FLAG_PENDING           0x80000000

#define XONLINE_FRIENDSTATE_GET_GUESTS(dwState) ((dwState & XONLINE_FRIENDSTATE_MASK_GUESTS) >> 5)
#define XONLINE_FRIENDSTATE_SET_GUESTS(dwState, bGuests) (dwState &= (bGuests << 5) & XONLINE_FRIENDSTATE_MASK_GUESTS)

typedef enum {
    XONLINE_NOTIFICATION_EVENT_SERVICE,
    NUM_XONLINE_NOTIFICATION_EVENT_TYPES
} XONLINE_NOTIFICATION_EVENT_TYPE;

typedef struct {
    XONLINE_NOTIFICATION_EVENT_TYPE     type;
    union {
        struct {
            DWORD                       dwCode;
        } service;
    } info;
} XONLINE_NOTIFICATION_MSG, *PXONLINE_NOTIFICATION_MSG;

XBOXAPI
BOOL
WINAPI
XOnlineNotificationSetState(
    IN DWORD dwUserIndex,
    IN DWORD dwStateFlags,
    IN XNKID sessionID,
    IN DWORD cbStateData,
    IN PBYTE pStateData
    );

XBOXAPI
HRESULT
WINAPI
XOnlineNotificationSetUserData(
    IN DWORD dwUserIndex,
    IN DWORD cbUserData,
    IN PBYTE pUserData,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

XBOXAPI
BOOL
WINAPI
XOnlineNotificationIsPending(
    IN DWORD dwUserIndex,
    IN DWORD dwType
    );
    
XBOXAPI
HRESULT
WINAPI
XOnlineNotificationEnumerate(
    IN DWORD dwUserIndex,
    IN PXONLINE_NOTIFICATION_MSG pbBuffer,
    IN WORD wBufferCount,
    IN DWORD dwNotificationTypes,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    );


// ====================================================================
// Friends
//

typedef enum {
    XONLINE_REQUEST_NO,
    XONLINE_REQUEST_YES,
    XONLINE_REQUEST_BLOCK
} XONLINE_REQUEST_ANSWER_TYPE;

typedef enum {
    XONLINE_GAMEINVITE_NO,
    XONLINE_GAMEINVITE_YES,
    XONLINE_GAMEINVITE_REMOVE
} XONLINE_GAMEINVITE_ANSWER_TYPE;

#define MAX_FRIENDS         100
#define MAX_STATEDATA_SIZE  8
#define MAX_USERDATA_SIZE   24

typedef struct {
    XUID                    xuid;           
    CHAR                    username[XONLINE_USERNAME_SIZE];
    DWORD                   friendState;
    XNKID                   sessionID;
    DWORD                   titleID;
    BYTE                    StateDataSize;
    BYTE                    StateData[MAX_STATEDATA_SIZE];
    BYTE                    UserDataSize;
    BYTE                    UserData[MAX_USERDATA_SIZE];
} XONLINE_FRIEND, *PXONLINE_FRIEND;

typedef struct {
    XONLINE_FRIEND InvitingFriend;
    XUID           xuidAcceptedFriend;
    FILETIME       InviteAcceptTime;
} XONLINE_ACCEPTED_GAMEINVITE, *PXONLINE_ACCEPTED_GAMEINVITE;

typedef enum {
    XONLINE_FRIENDS_RESULTS_FRIENDREQUESTFAILED = 0,
    XONLINE_FRIENDS_RESULTS_FRIENDREMOVEFAILED,
    NUM_XONLINE_FRIENDS_RESULTS_TYPES
} XONLINE_FRIENDS_RESULTS_TYPE;

typedef struct {
    XONLINE_FRIENDS_RESULTS_TYPE      type;
    union {
        struct {
            XUID                     xuid;
            HRESULT                  hr;
        } requestError;
        struct {
            XUID                     xuid;
            HRESULT                  hr;
        } removeError;
    } info;
} XONLINE_FRIENDS_RESULTS_ERROR, *PXONLINE_FRIENDS_RESULTS_ERROR;

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsStartup(
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    );


XBOXAPI
HRESULT
WINAPI
XOnlineFriendsGetResults(
    OUT DWORD *pdwUserIndex,
    OUT XUID *pxuidTargetUser
    );

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsEnumerate(
    IN DWORD dwUserIndex,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

XBOXAPI
DWORD
WINAPI
XOnlineFriendsGetLatest(
    IN DWORD dwUserIndex,
    IN DWORD dwFriendBufferCount,
    OUT PXONLINE_FRIEND pFriendBuffer
    );

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsRemove(
    IN DWORD dwUserIndex,
    IN PXONLINE_FRIEND pFriend
    );

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsRequest(
    IN DWORD dwUserIndex,
    IN XUID xuidToUser
    );

//@@BEGIN_MSINTERNAL
XBOXAPI
HRESULT
WINAPI
XOnlineFriendsRequestByName(
    IN DWORD dwUserIndex,
    IN PSTR pszUserName
    );
//@@END_MSINTERNAL
XBOXAPI
HRESULT
WINAPI
XOnlineFriendsGameInvite(
    IN DWORD dwUserIndex,
    IN XNKID SessionID,
    IN DWORD dwFriendListCount,
    IN PXONLINE_FRIEND pToFriendList
    );

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsRevokeGameInvite(
    IN DWORD dwUserIndex,
    IN XNKID SessionID,
    IN DWORD dwFriendListCount,
    IN PXONLINE_FRIEND pToFriendList
    );

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsAnswerRequest(
    IN DWORD dwUserIndex,
    IN PXONLINE_FRIEND pToFriend,
    IN XONLINE_REQUEST_ANSWER_TYPE Answer
    );

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsAnswerGameInvite(
    IN DWORD dwUserIndex,
    IN PXONLINE_FRIEND pToFriend,
    IN XONLINE_GAMEINVITE_ANSWER_TYPE Answer
    );

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsGetAcceptedGameInvite(
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

XBOXAPI
HRESULT
WINAPI
XOnlineFriendsGetAcceptedGameInviteResult(
    IN XONLINETASK_HANDLE hTask,
    OUT PXONLINE_ACCEPTED_GAMEINVITE pAcceptedGameInvite
    );

// ====================================================================
// LockoutList
//

#define MAX_LOCKOUTUSERS      250

typedef struct {
    XUID                    xuid;           
    CHAR                    username[XONLINE_USERNAME_SIZE];
} XONLINE_LOCKOUTUSER, *PXONLINE_LOCKOUTUSER;

XBOXAPI
DWORD
WINAPI
XOnlineLockoutlistGet(
    IN DWORD dwUserIndex,
    IN DWORD dwLockoutUserBufferCount,
    OUT PXONLINE_LOCKOUTUSER pLockoutUserBuffer
    );

XBOXAPI
HRESULT
WINAPI
XOnlineLockoutlistStartup(
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

XBOXAPI
HRESULT
WINAPI
XOnlineLockoutlistAdd(
    IN DWORD dwUserIndex,
    IN XUID xUserID,
    IN CHAR *pszUsername
    );

XBOXAPI
HRESULT
WINAPI
XOnlineLockoutlistRemove(
    IN DWORD dwUserIndex,
    IN XUID xUserID
    );

XBOXAPI
HRESULT
WINAPI
XOnlineVerifyNickname(
    IN LPCWSTR lpNickname,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    );

// ====================================================================
// XOnline Feedback
//

typedef enum {
	XONLINE_FEEDBACK_NEG_NICKNAME,
	XONLINE_FEEDBACK_NEG_GAMEPLAY,
	XONLINE_FEEDBACK_NEG_SCREAMING,
	XONLINE_FEEDBACK_NEG_HARASSMENT,
	XONLINE_FEEDBACK_NEG_LEWDNESS,
	XONLINE_FEEDBACK_POS_ATTITUDE,
	XONLINE_FEEDBACK_POS_SESSION,
	NUM_XONLINE_FEEDBACK_TYPES
} XONLINE_FEEDBACK_TYPE;

XBOXAPI
HRESULT
WINAPI
XOnlineFeedbackSend(
            IN DWORD dwUserIndex,
            IN XUID xTargetUser,
            IN XONLINE_FEEDBACK_TYPE FeedbackType,
            IN LPCWSTR lpszNickname,
            IN HANDLE hWorkEvent,
            OUT PXONLINETASK_HANDLE phTask
            );

// ====================================================================
// Statistics
//

#define XONLINE_STAT_RANK       ((WORD)0xFFFF)
#define XONLINE_STAT_RATING     ((WORD)0xFFFE)

typedef enum _XONLINE_STAT_TYPE {
	XONLINE_STAT_NONE,
	XONLINE_STAT_CHAR,
	XONLINE_STAT_SHORT,
	XONLINE_STAT_LONG,
	XONLINE_STAT_LONGLONG,
	XONLINE_STAT_FLOAT,
	XONLINE_STAT_DOUBLE
} XONLINE_STAT_TYPE;

typedef struct _XONLINE_STAT {
	WORD wID;
	XONLINE_STAT_TYPE type;
	
	union {
		CHAR cValue;
		SHORT sValue;
		LONG lValue;
		LONGLONG qwValue;
		float fValue;
		double dValue;             
	};
} XONLINE_STAT, *PXONLINE_STAT;

typedef struct _XONLINE_STAT_SPEC {
	XUID xuidUser;
	DWORD dwLeaderBoardID;
	DWORD dwNumStats;
	PXONLINE_STAT pStats;
} XONLINE_SET_STAT, *PXONLINE_STAT_SPEC;
 
typedef struct _XONLINE_STAT_USER {
	XUID xuidUser;
	CHAR szUsername[XONLINE_USERNAME_SIZE];
} XONLINE_STAT_USER, *PXONLINE_STAT_USER;

XBOXAPI
HRESULT
WINAPI
XOnlineStatSet(
	IN DWORD dwNumStatSpecs,
	IN PXONLINE_STAT_SPEC pStatSpecs,
	IN HANDLE hWorkEvent,
	OUT PXONLINETASK_HANDLE phTask
);

XBOXAPI
HRESULT
WINAPI
XOnlineStatGet(
    IN DWORD dwNumStatSpecs,
    IN OUT PXONLINE_STAT_SPEC pStatSpecs,
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE phTask
);

XBOXAPI
HRESULT
WINAPI
XOnlineStatLeaderEnumerate(
	IN XUID* pxuidPagePivot,                           
	IN DWORD dwPageStart,                                              
	IN DWORD dwPageSize,
	IN DWORD dwLeaderboardID,
	IN DWORD dwNumStatsPerUser,
	IN DWORD *pStatsPerUser,
	OUT PXONLINE_STAT_USER pUsers,
	OUT PXONLINE_STAT pStats,
	IN HANDLE hWorkEvent,
	OUT PXONLINETASK_HANDLE phTask
);

XBOXAPI
HRESULT
WINAPI
XOnlineStatLeaderEnumerateGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT DWORD *pdwReturnedResults
);


//@@BEGIN_MSINTERNAL

// ********************************************************************
// Internal structures and functions begin here
// ********************************************************************

// ====================================================================
// Internal API hresults
//

#define XONLINE_S_ACCOUNTS_NAME_TAKEN            _HRESULT_TYPEDEF_(0x00000010L)

#define XONLINE_E_ACCOUNTS_INVALID_KINGDOM       _HRESULT_TYPEDEF_(0x80154001L)
#define XONLINE_E_ACCOUNTS_INVALID_USER          _HRESULT_TYPEDEF_(0x80154002L)
#define XONLINE_E_ACCOUNTS_USER_NOT_LOCKED_OUT   _HRESULT_TYPEDEF_(0x80154003L)
#define XONLINE_E_ACCOUNTS_ACCOUNT_INACTIVE      _HRESULT_TYPEDEF_(0x80154004L)

//
// Define macors to determine special states
//
#define XONLINETASK_STATUS_AVAILABLE(hr) \
            (((hr) == XONLINETASK_S_RUNNING)?FALSE:TRUE)
            
#define XONLINETASK_STATUS_SUCCESSFUL_COMPLETION(hr) \
            (((hr) == XONLINETASK_S_SUCCESS)?TRUE:FALSE)
            
#define XONLINETASK_STATUS_FAILED(hr)           (FAILED(hr))

#define XONLINETASK_SET_COMPLETION_STATUS(hr) \
            { if (SUCCEEDED(hr)) hr = XONLINETASK_S_SUCCESS; }


// ====================================================================
// XOnline Accounts
//

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NOTE: ANY CHANGES TO THESE DEFINEs MUST ALSO 
// BE MADE TO
//  //depot/xonline/private/common/mc/xon.cs
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define MAX_FIRSTNAME_SIZE       64
#define MAX_LASTNAME_SIZE        64
#define MAX_EMAIL_SIZE           129
#define MAX_PHONE_PREFIX_SIZE    12
#define MAX_PHONE_NUMBER_SIZE    12
#define MAX_PHONE_EXTENSION_SIZE 12
#define MAX_STREET_SIZE          128
#define MAX_CITY_SIZE            64
#define MAX_DISTRICT_SIZE        64
#define MAX_STATE_SIZE           64                                              
#define MAX_COUNTRYCODE_SIZE     2
#define MAX_POSTALCODE_SIZE      16
#define MAX_CC_NAME_SIZE         64
#define MAX_CC_TYPE_SIZE         32
#define MAX_CC_NUMBER_SIZE       24
#define MAX_CC_EXPIRATION_SIZE   6  
#define MAX_BANK_CODE_SIZE       64
#define MAX_BRANCH_CODE_SIZE     64
#define MAX_CHECK_DIGITS_SIZE    64


// priamry info required for payment
//
typedef struct 
{
    WCHAR    wszNickname[XONLINE_NAME_SIZE];
    WCHAR    wszKingdom[XONLINE_KINGDOM_SIZE];
    BYTE     rgbPin[XONLINE_PIN_LENGTH];
    
    WCHAR    wszFirstName[MAX_FIRSTNAME_SIZE];
    WCHAR    wszLastName[MAX_LASTNAME_SIZE];
    WCHAR    wszStreet1[MAX_STREET_SIZE];
    WCHAR    wszStreet2[MAX_STREET_SIZE];
    WCHAR    wszStreet3[MAX_STREET_SIZE];
    WCHAR    wszCity[MAX_CITY_SIZE];
    WCHAR    wszState[MAX_STATE_SIZE];
    BYTE     bCountryId;
    WCHAR    wszPostalCode[MAX_POSTALCODE_SIZE];
    WCHAR    wszPhonePrefix[MAX_PHONE_PREFIX_SIZE];
    WCHAR    wszPhoneNumber[MAX_PHONE_NUMBER_SIZE];
    WCHAR    wszPhoneExtension[MAX_PHONE_EXTENSION_SIZE];
    
    WCHAR    wszCardHolder[MAX_CC_NAME_SIZE];
    BYTE     bCardTypeId;
    WCHAR    wszCardNumber[MAX_CC_NUMBER_SIZE];
    FILETIME ftCardExpiration;
    
    WCHAR    wszEmail[MAX_EMAIL_SIZE];
    WCHAR    wszBankCode[MAX_BANK_CODE_SIZE];
    WCHAR    wszBranchCode[MAX_BRANCH_CODE_SIZE];
    WCHAR    wszCheckDigits[MAX_CHECK_DIGITS_SIZE];
} 
USER_ACCOUNT_INFO, *PUSER_ACCOUNT_INFO;

typedef struct
{
    WCHAR    wszNickname[XONLINE_NAME_SIZE];
    WCHAR    wszRealm[XONLINE_KINGDOM_SIZE];
} 
USER_XNAME, *PUSER_XNAME;


XBOXAPI
HRESULT
WINAPI
_XOnlineAccountTempCreate(
    IN PXONLINE_USER pUser, 
    IN HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask 
    );

XBOXAPI
HRESULT
WINAPI
_XOnlineAccountTempCreateGetResults(
    IN  XONLINETASK_HANDLE hTask,
    OUT PXONLINE_USER pUser
    );

XBOXAPI
HRESULT
WINAPI
_XOnlineGetTags(
    IN WORD wCountryId, 
    IN WORD wMaxTags, 
    IN LPCWSTR pwsName, 
    IN HANDLE hWorkEvent, 
    OUT XONLINETASK_HANDLE *phTask
    );

XBOXAPI
HRESULT
WINAPI
_XOnlineGetTagsResults(
    IN  XONLINETASK_HANDLE hTask,
    OUT LPWSTR* pszTags,
    OUT WORD* pwTagCount
    );


XBOXAPI
HRESULT
WINAPI
_XOnlineReserveName(
    IN LPCWSTR pwsGamerName,
    IN LPCWSTR pwsKingdom,
    IN WORD wMaxNames, 
    IN WORD wCountryId, 
    IN HANDLE hWorkEvent, 
    OUT XONLINETASK_HANDLE *phTask
    );



XBOXAPI
HRESULT
WINAPI
_XOnlineGetReserveNameResults(
    IN  XONLINETASK_HANDLE hTask,
    OUT PUSER_XNAME *ppNames,
    OUT DWORD* pdwNameCount
    );

XBOXAPI
HRESULT
WINAPI
_XOnlineCreateAccount(
    IN  USER_ACCOUNT_INFO* pAccountInfo,
    IN  HANDLE hWorkEvent, 
    OUT XONLINETASK_HANDLE *phTask
    );


XBOXAPI
HRESULT
WINAPI
_XOnlineGetCreateAccountResults(
    IN  XONLINETASK_HANDLE hTask,
    OUT PXONLINE_USER pUser
    );


// ====================================================================
// XOnline Authentication
//

#define XONLINE_E_LOGON_MACHINE_AUTHENTICATION_FAILED          _HRESULT_TYPEDEF_(0x80151900L)

#define XONLINE_USER_SIGNATURE_LENGTH           8
#define XONLINE_KERBEROS_PRINCIPLE_NAME_SIZE   64

#define XONLINE_MAX_HD_ONLINE_USERS             8
#define XONLINE_MAX_ONLINE_USERS_PER_SECTOR     4

typedef struct {

    XUID xuid;
    CHAR name[XONLINE_NAME_SIZE];
    CHAR kingdom[XONLINE_KINGDOM_SIZE];
    DWORD dwUserOptions;
    BYTE pin[XONLINE_PIN_LENGTH];

    //
    // The following 5 fields are marked as reserved in the public structure
    // The combined size should add up to XONLINE_USER_RESERVED_SIZE
    //
    CHAR domain[XONLINE_USERDOMAIN_SIZE];
    CHAR realm[XONLINE_REALM_NAME_SIZE];
    BYTE key[XONLINE_KEY_LENGTH];
    DWORD dwSignatureTime;
    BYTE signature[XONLINE_USER_SIGNATURE_LENGTH];
    
    HRESULT hr;
    DWORD index;

} XONLINEP_USER, *PXONLINEP_USER;

//@@END_CLIENTONLY
//
// These Services are hidden from title developers
//
#define XONLINE_PRESENCE_SERVICE                ((DWORD)1)
#define XONLINE_MESSAGE_SERVICE                 ((DWORD)2)
#define XONLINE_AUTO_UPDATE_SERVICE             ((DWORD)3)
#define XONLINE_USER_ACCOUNT_SERVICE            ((DWORD)5)

//
// These Services are for testing against the sgs
//
#define XONLINE_SG_DATA_ENUM_SERVICE            ((DWORD)257)
#define XONLINE_SG_DATA_TEST_SERVICE            ((DWORD)258)
#define XONLINE_SG_PERF_ENUM_SERVICE            ((DWORD)259)
#define XONLINE_SG_PERF_TEST_SERVICE            ((DWORD)260)
//@@BEGIN_CLIENTONLY

#pragma pack(push, 4)

typedef struct
{
    XUID xuid;
    CHAR name[XONLINE_NAME_SIZE];
    CHAR kingdom[XONLINE_KINGDOM_SIZE];
    DWORD dwUserOptions;
    BYTE pin[XONLINE_PIN_LENGTH];
    CHAR domain[XONLINE_USERDOMAIN_SIZE];
    CHAR realm[XONLINE_REALM_NAME_SIZE];
    BYTE key[XONLINE_KEY_LENGTH];
    DWORD dwSignatureTime;
    BYTE signature[XONLINE_USER_SIGNATURE_LENGTH];
} XC_ONLINE_USER_ACCOUNT_STRUCT, *PXC_ONLINE_USER_ACCOUNT_STRUCT;

typedef XC_ONLINE_USER_ACCOUNT_STRUCT XC_ONLINE_MACHINE_ACCOUNT_STRUCT, *PXC_ONLINE_MACHINE_ACCOUNT_STRUCT;

#define CONFIG_DATA_LENGTH 492
#define MACHINE_ACCOUNT_CONFIG_SECTOR   1
#define BASE_USER_ACCOUNT_CONFIG_SECTOR 2

// ====================================================================
// XOnline Content Delivery
//

//
// Function to kick off an enumeration of all active titles played on the
// Xbox. All titles that have been played on the Xbox and not removed will
// be enumerated. This operation, upon success, returns dwMaxResults title IDs,
// of the total number of titles on the system, whichever is less. The actual
// list of Title IDs can be retrieved using XOnlineEnumerateTitlesGetResults().
// Subsequent title IDs can be retrieved by calling 
// XOnlineEnumerateTitlesContinue().
//
XBOXAPI
HRESULT 
WINAPI
XOnlineEnumerateTitlesBegin(
    IN DWORD dwMaxResults,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

XBOXAPI
HRESULT 
WINAPI
XOnlineEnumerateTitlesContinue(
    IN XONLINETASK_HANDLE hTask
    );

//
// Function to retrieve the results of a title enumeration.
//
XBOXAPI
HRESULT 
WINAPI
XOnlineEnumerateTitlesGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT DWORD **prgTitleID,
    OUT DWORD *pdwTitleID
    );

// ====================================================================
// XRL (XOnlineDownload and XOnlineUpload) Functions
//

//
// Define extended status codes for download and upload
//
#define XONLINEDOWNLOAD_EXSTATUS_SUCCESS        (200)
#define XONLINEDOWNLOAD_EXSTATUS_TRANSERROR(es) (((es >= 400) && (es < 500))?TRUE:FALSE)
#define XONLINEDOWNLOAD_EXSTATUS_PERMERROR(es)  ((es >= 500)?TRUE:FALSE)
#define XONLINEDOWNLOAD_EXSTATUS_ANYERROR(es)   ((es >= 400)?TRUE:FALSE)

#define XONLINEUPLOAD_EXSTATUS_SUCCESS            (200)
#define XONLINEUPLOAD_EXSTATUS_NOCONTENT        (204)
#define XONLINEUPLOAD_EXSTATUS_PARTIALCONTENT   (206)
#define XONLINEUPLOAD_EXSTATUS_TRANSERROR(es)   (((es >= 400) && (es < 500))?TRUE:FALSE)
#define XONLINEUPLOAD_EXSTATUS_PERMERROR(es)    ((es >= 500)?TRUE:FALSE)
#define XONLINEUPLOAD_EXSTATUS_ANYERROR(es)     ((es >= 400)?TRUE:FALSE)

//
// Define flags for XOnlineDownloadFile
//
#define XON_DF_FORCE_DOWNLOAD            ((DWORD)0x00000001)

//
// XRL access functions
//

//
// Function to kick off downloading a resource from a service
// to memory
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineDownloadToMemory(
    IN DWORD dwServiceID,
    IN LPCSTR szResourcePath, 
    IN PBYTE pbBuffer,
    IN DWORD cbBuffer, 
    IN OPTIONAL PBYTE pbExtraHeaders,
    IN OPTIONAL DWORD cbExtraHeaders,
    IN DWORD dwTimeout,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to kick off downloading a resoruce from a service
// to a file
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineDownloadFile(
    IN DWORD dwServiceID,
    IN LPCSTR szResourcePath, 
    IN PBYTE pbBuffer,
    IN DWORD cbBuffer, 
    IN OPTIONAL PBYTE pbExtraHeaders,
    IN OPTIONAL DWORD cbExtraHeaders,
    IN DWORD dwFlags,
    IN LPCSTR szLocalPath, 
    IN DWORD dwTimeout,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to get the download progress
//
XBOXAPI
HRESULT
WINAPI 
XOnlineDownloadGetProgress(
    IN XONLINETASK_HANDLE hTask,
    OUT OPTIONAL DWORD *pdwPercentDone,
    OUT OPTIONAL ULARGE_INTEGER *puliNumerator,
    OUT OPTIONAL ULARGE_INTEGER *puliDenominator
    );

//
// Function to return the results of a download
//
XBOXAPI
HRESULT
WINAPI
XOnlineDownloadGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT OPTIONAL LPBYTE *ppbBuffer,
    OUT OPTIONAL DWORD *pcbBuffer,
    OUT OPTIONAL ULARGE_INTEGER *puliTotalReceived,
    OUT OPTIONAL ULARGE_INTEGER *puliContentLength,
    OUT OPTIONAL DWORD *pdwExtendedStatus,
    OUT OPTIONAL FILETIME *pftTimestamp,
    OUT OPTIONAL FILETIME *pftLastModified
    );

//
// Function to kick off uploading a memory buffer to the specified service
//
XBOXAPI
HRESULT
WINAPI 
XOnlineUploadFromMemory(
    IN DWORD dwServiceID,
    IN LPCSTR szTargetPath, 
    IN PBYTE pbBuffer,
    IN OUT DWORD *pcbBuffer, 
    IN OPTIONAL PBYTE pbExtraHeaders,
    IN OPTIONAL DWORD cbExtraHeaders,
    IN PBYTE pbDataToUpload,
    IN DWORD cbDataToUpload,
    IN DWORD dwTimeout,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to kick off uploading a local file to the specified service
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineUploadFile(
    IN DWORD dwServiceID,
    IN LPCSTR szTargetPath, 
    IN PBYTE pbBuffer,
    IN OUT DWORD *pcbBuffer, 
    IN OPTIONAL PBYTE pbExtraHeaders,
    IN OPTIONAL DWORD cbExtraHeaders,
    IN LPCSTR szLocalPath, 
    IN DWORD dwTimeout,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to get the upload progress
//
XBOXAPI
HRESULT
WINAPI 
XOnlineUploadGetProgress(
    IN XONLINETASK_HANDLE hTask,
    OUT OPTIONAL DWORD *pdwPercentDone,
    OUT OPTIONAL ULARGE_INTEGER *puliNumerator,
    OUT OPTIONAL ULARGE_INTEGER *puliDenominator
    );

//
// Function to return the results of an upload
//
XBOXAPI
HRESULT
WINAPI
XOnlineUploadGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT OPTIONAL LPBYTE *ppbBuffer,
    OUT OPTIONAL DWORD *pcbBuffer,
    OUT OPTIONAL ULARGE_INTEGER *puliTotalReceived,
    OUT OPTIONAL ULARGE_INTEGER *puliContentLength,
    OUT OPTIONAL DWORD *pdwExtendedStatus,
    OUT OPTIONAL FILETIME *pftTimestamp
    );

//@@END_CLIENTONLY

// ====================================================================
// XOnline Presence & Notification
//

//
// Current version is v0.17
//
#define CURRENT_XPPROTOCOL_VER 0x00000011

//
// Various protocol related constants
//
#define MIN_PMSG_SIZE               4
#define MAX_PMSG_SIZE               0x10000 //~64k
#define MAX_HELLO_DESCRIPTION_LEN   80
#define MAX_ACCTNAME_BYTES          64
#define MAX_NICKNAME_BYTES          24
#define MAX_TITLE_STATE_BYTES       8
#define MAX_BUDDIES                 100

enum P_MSG_TYPES
{
    // Incoming messages
    PMSG_MIN_REQ_TYPE   = 1000,
    PMSG_HELLO          = 1000,
    PMSG_ALIVE          = 1001,
    PMSG_SYNC           = 1002,
    PMSG_REFRESH        = 1003,
    PMSG_ADD            = 1004,
    PMSG_DELETE         = 1005,
    PMSG_ACCEPT         = 1006,
    PMSG_REJECT         = 1007,
    PMSG_BLOCK          = 1008,
    PMSG_UNBLOCK        = 1009,
    PMSG_INVITE         = 1010,
    PMSG_CANCEL         = 1011,
    PMSG_INVITE_ANSWER  = 1012,
    PMSG_NICKNAME       = 1013,
    PMSG_STATE          = 1014,
    PMSG_DEAD_XBOX      = 1015,
    PMSG_DEAD_SG        = 1016,
    PMSG_ANNOUNCE       = 1017,
    PMSG_SUBSCRIBED     = 1018,
    PMSG_UNSUBSCRIBE    = 1019,
    PMSG_ADDED          = 1020,
    PMSG_ACCEPTED       = 1021,
    PMSG_REMOVED        = 1022,
    PMSG_KICK           = 1023,
    PMSG_MAX_REQ_TYPE   = 1023,

    // Outgoing messages
    PMSG_MIN_REPLY_TYPE = 1101,
    PMSG_ALIVE_REPLY    = 1101,
    PMSG_SYNC_REPLY     = 1102,
    PMSG_MAX_REPLY_TYPE = 1102
};

#define P_STATE_MASK_NONE           0x0000
#define P_STATE_MASK_ONLINE         0x0001 // Set if online
#define P_STATE_MASK_PLAYING        0x0002 // Set if playing a game
#define P_STATE_MASK_CLOAKED        0x0004 // Set to fake being offline
#define P_STATE_MASK_VOICE          0x0008 // Set if supports voice
#define P_STATE_MASK_JOINABLE       0x0010 // Set if session is joinable
#define P_STATE_MASK_GUESTS         0x0060
#define P_STATE_MASK_RESERVED0      0x0080

#define P_BUDDY_STATUS_OK      0 // Mutualy accepted buddy
#define P_BUDDY_STATUS_PENDING 1 // Buddy request pending accetance
#define P_BUDDY_STATUS_REQUEST 2 // Buddy request to accept or reject

enum P_INVITE_REPLIES
{
    PINVITE_REPLY_NO    = 0,
    PINVITE_REPLY_YES   = 1
};

enum P_QUEUE_TYPES
{
    //
    // Outgoing queued response items
    //
    PQUEUE_LIST_CHANGE   = 0,
    PQUEUE_PRESENCE      = 1,
    PQUEUE_INVITE        = 2,
    PQUEUE_INVITE_ANSWER = 3
};

enum P_LIST_OPERATIONS
{
    PLIST_BUDDY_NOP     = 0,
    PLIST_BUDDY_ADD     = 1,
    PLIST_BUDDY_ADDED   = 2,
    PLIST_BUDDY_ACCEPT  = 3,
    PLIST_BUDDY_REJECT  = 4,
    PLIST_BUDDY_DELETE  = 5,
    PLIST_BLOCK_NOP     = 6,
    PLIST_BLOCK_BLOCK   = 7,
    PLIST_BLOCK_UNBLOCK = 8
};

#define IGNOREME_SESSID     0
#define MSGTYPE_ACK         0xffffffff

//
// Current version is v0.17
//
#define CURRENT_XQPROTOCOL_VER 0x00000011

//
// Various protocol related constants
//
#define MIN_QMSG_SIZE               4
#define MAX_QMSG_SIZE               0x10000 //~64k
#define MAX_HELLO_DESCRIPTION_LEN   80

enum Q_MSG_TYPES
{
    //
    // Incoming messages
    //
    QMSG_HELLO = 0,
    QMSG_USER_INFO = 1,
    QMSG_ADD = 2,
    QMSG_DELETE = 3,
    QMSG_DELETE_MATCHES = 4,
    QMSG_DEAD_XBOX = 5,
    QMSG_DEAD_SG = 6,
    QMSG_LIST = 7,
    QMSG_DEQUEUE = 8,
    QMSG_MAX_REQ_TYPE = 8,

    //
    // Outgoing messages
    //
    QMSG_LIST_REPLY = 100,
    QMSG_MAX_REPLY_TYPE = 100
};
//----------------------------------------------------------------------------+
//  Structures
//

#ifdef __cplusplus

#pragma pack(push, 1)

//
// This is the basic message framing structure from which the queue and presence
// server protocols are derived.
//
// dwMsgType is an enumerated value unique across protocols
// dwMsgLen is the amount of message data that follows this header
// dwSeqNum is a client-assigned sequence number used to resolve async races
// dwSessID is a connection server assigned session ID used to resolve async races
//
struct BASE_MSG_HEADER
{
    DWORD dwMsgType;
    DWORD dwMsgLen;
    DWORD dwSeqNum;
    SGADDR sgaddr;

    // BYTE rgbMsgData[ dwMsgLen ];
};

//
// This is the basic acknowledgement message used in all derived protocols to
// signal reception of certain messages and indicate their resulting status.
//
struct BASE_ACK_MSG : public BASE_MSG_HEADER
{
    DWORD dwAckMsgType;     // dwMsgType of message being acknowledged
    DWORD dwAckSeqNum;      // dwSeqNum of message being acknowledged
    ULONGLONG qwAckUserID;
    HRESULT hrAck;
};

//
// This must be the first message sent to the server after establishing
// a TCP connection
//
struct Q_HELLO_MSG : public BASE_MSG_HEADER
{
    DWORD dwProtocolVersion;    // Hi word | lo word

    // char szDescription[];    // NULL-terminated ASCII string
};

//
// Before sending any messages pertaining to a user, this message must be
// sent to the server to establish the user's queues.
//
struct Q_USER_INFO_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
};

//
// Add a new item asynchronously to one or more users' Qs, given the item type
//
struct Q_ADD_MSG : public BASE_MSG_HEADER
{
    WORD wQType;
    WORD wUserCount;

    // ULONGLONG qwUserIDs[ dwUserCount ];
    // BYTE rgbData[];
};

//
// Delete an item or range of items of the given item type from a user's Q
//
struct Q_DELETE_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    WORD wQType;
    DWORD dwFirstItemID;
    DWORD dwLastItemID;      // Set == dwFirstItemID to delete one item
};

//
// Delete any matching items of the given item type from one or more users' Qs
//
struct Q_DELETE_MATCHES_MSG : public BASE_MSG_HEADER
{
    WORD wQType;
    WORD wMatchAttrNum;   // must be zero for now
    WORD wUserCount;
    
    // ULONGLONG qwUserIDs[ dwUserCount ];
    // BYTE rgbMatchData[ MatchAttrLen ];
};

//
// Inform the Q server that the given XBox just went offline, so any
// items pertaining to this address that are no longer relevant should be
// deleted.
//
struct Q_DEAD_XBOX_MSG : public BASE_MSG_HEADER
{
};

//
// Inform the Q server that the given SG just went offline, so any users
// who were on this SG should be considered offline also.  This will end
// up emptying many Q's, as well as potentially deleting many individual items.
//
struct Q_DEAD_SG_MSG : public BASE_MSG_HEADER
{
};

//
// Ask the Q server to send a Q_LIST_REPLY_MSG containing up to the number of
// items given of a particular type for a certain user.
//
struct Q_LIST_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    WORD  wQType;
    DWORD dwStartItemID;
    WORD  wMaxItems;
    DWORD dwMaxTotalItemDataSize;
};

//
// Ask the Q server to send a Q_LIST_REPLY_MSG containing up to the number of
// items given of a particular type for a certain user.  The items returned
// are removed from the queue.
//
struct Q_DEQUEUE_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    WORD  wQType;
    DWORD dwStartItemID;
    WORD  wMaxItems;
    DWORD dwMaxTotalItemDataSize;
};

//
// The asynchronous reply to a Q_LIST_MSG or Q_DEQUEUE_MSG 
//
struct Q_LIST_REPLY_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    WORD  wQType;
    HRESULT hr;
    DWORD dwNextItemID;
    WORD  wQLeft;
    WORD  wNumItems;
    DWORD dwTotalItemDataSize;
    DWORD dwQFlags;
    DWORD dwSeqQFlags;

    // Q_LIST_ITEM rgItems[ dwNumItems ];
};

struct Q_LIST_ITEM
{
    DWORD dwItemID;
    WORD wItemLen;

    // BYTE rgbItemData[ dwItemLen ];
};

//
// This must be the first message sent to the server after establishing
// a TCP connection
//
struct P_HELLO_MSG : public BASE_MSG_HEADER
{
    DWORD dwProtocolVersion;    // Hi word | lo word
    // char szDescription[];    // NULL-terminated ASCII string
};

//
// This establishes the user's presence in the system
//
struct P_ALIVE_MSG : public BASE_MSG_HEADER
{
    // User Info
    ULONGLONG qwUserID;
    DWORD dwTitleID;
    WORD  cbAcctName;
    // Sync info
    DWORD dwBuddyListVersion;
    DWORD dwBlockListVersion;
    // State into
    DWORD dwState;
    ULONGLONG qwMatchSessionID;
    WORD  cbNickname;
    WORD  cbTitleStuff;
    // char szAcctName[];               // ASCII Null terminated
    // byte rgbNickName[];              // Binary
    // byte rgbTitleStuff[];            // Binary
};

//
// The asynchronous reply to a P_ALIVE_MSG is contained in one of these
//
struct P_REPLY_BUDDY
{
    ULONGLONG qwBuddyID;
    WORD  cbAcctName;                   // Includes the '\0'
    BYTE  bStatus;
    // char szAcctName;                 // ASCII Null terminated
};

struct P_REPLY_BLOCK
{
    ULONGLONG qwBlockID;
    WORD  cbAcctName;                   // Includes the '\0'
    // char szAcctName;                 // ASCII Null terminated
};

struct P_ALIVE_REPLY_MSG : public BASE_MSG_HEADER
{
    HRESULT hr;                         // On fail other fields set to 0
    DWORD dwBuddyListVersion;
    WORD  cBuddiesSent;
    DWORD dwBlockListVersion;
    WORD  cBlocksSent;
    // P_REPLY_BUDDY rgBuddies[]
    // P_REPLY_BLOCK rgBlocks[]
};

//
// Resync the buddy and block lists
//
struct P_SYNC_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    DWORD dwBuddyListVersion;
    DWORD dwBlockListVersion;
};

//
// The asynchronous reply to a P_SYNC_MSG is contained in one of these
//
struct P_SYNC_REPLY_MSG : public BASE_MSG_HEADER
{
    HRESULT hr;                         // On fail other fields set to 0
    DWORD dwBuddyListVersion;
    WORD  cBuddiesSent;
    DWORD dwBlockListVersion;
    WORD  cBlocksSent;
    // P_REPLY_BUDDY rgBuddies[]
    // P_REPLY_BLOCK rgBlocks[]
};

//
// Refresh the presence and invitation notification queues
//
struct P_REFRESH_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
};

//
// Attempt to add the given person to the sender's buddy list, either by the
// person's ID or name.
//
struct P_ADD_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwSenderID;
    ULONGLONG qwBuddyID;
    WORD  cbBuddyAcctname;
    // char szBuddyAcctName[ cbBuddyAcctname];   // ASCII Null terminated
                                                    // only used if qwBuddyID == 0
};

//
// Delete a buddy from my buddy list
//
struct P_DELETE_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwBuddyID;
};

//
// Accept a request to be someone's buddy
//
struct P_ACCEPT_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwBuddyID;
};

//
// Reject a request to be someone's buddy
//
struct P_REJECT_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwBuddyID;
    BOOL fNever;
};

//
// Add a user to my blocklist 
//
struct P_BLOCK_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwBlockID;       
};

//
// Remove the a user from my blocklist 
// If qwBlockID == 0, clear the entire block list.
//
struct P_UNBLOCK_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwBlockID;       
};

struct P_INVITE_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    DWORD dwTitleID;
    ULONGLONG qwMatchSessionID;
    WORD  cInvitees;
    // ULONGLONG rgqwInvitees[ dwNumInvitees ];
};

struct P_CANCEL_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwMatchSessionID;
    WORD  cInvitees;
    // ULONGLONG rgqwInvitees[ dwNumInvitees ];
};

struct P_INVITE_ANSWER_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwHostID;
    ULONGLONG qwMatchSessionID;
    WORD wAnswer;
};

//
// This supplies the latest nickname info for a user
//
struct P_NICKNAME_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    WORD cbNickname;
    // byte rgbNickName[];              // Binary
};

//
// This supplies the latest state info for a user
//
struct P_STATE_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    DWORD dwState;
    ULONGLONG qwMatchSessionID;
    WORD cbTitleStuff;
    // byte rgbTitleStuff[];            // Binary
};

//
// Inform the P server that the given XBox just went offline, so any users
// who were on this XBox should be considered offline also.
//
struct P_DEAD_XBOX_MSG : public BASE_MSG_HEADER
{
};

//
// Inform the P server that the given SG just went offline, so any users
// who were on this SG should be considered offline also.
//
struct P_DEAD_SG_MSG : public BASE_MSG_HEADER
{
};

//
// This tells the destination user's P server that the listed source users
// have added a subscription for the destination user and are expected to 
// subscribe back if online
//
struct P_ANNOUNCE_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwDestUserID;
    WORD  cSourceUserIDs;
    // ULONGLONG qwSourceUserIDs[ dwCountSourceUsers ];
};

//
// This tells the destination user's P server that the listed source users
// have added a subscription for the destination user
//
struct P_SUBSCRIBED_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwDestUserID;
    WORD  cSourceUserIDs;
    // ULONGLONG qwSourceUserIDs[ dwCountSourceUsers ];
};

//
// This tells the destination users' P server that the source user just went
// offline, which implies that any subscription for the source user should
// be removed.
//
struct P_UNSUBSCRIBE_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwSourceUserID;
    WORD  cDestUserIDs;
    // ULONGLONG qwDestUserIDs[ dwCountDestUsers ];
};

//
// Tell your new buddy's P server that you made a request to become buddies.
//
struct P_ADDED_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwBuddyID;
    DWORD dwVersion;
    WORD  cbBuddyAcctName;
    // char szBuddyAcctName[ cbBuddyAcctName ];  // ASCII string null terminated
};

//
// Tell your new buddy's P server that you accepted his request to become buddies.
//
struct P_ACCEPTED_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwBuddyID;
    DWORD dwVersion;
};

//
// Tell your ex-buddy's P server that you are no longer buddies.
//
struct P_REMOVED_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    ULONGLONG qwBuddyID;
    DWORD dwVersion;
};

// Kick a user off the system
//
struct P_KICK_MSG : public BASE_MSG_HEADER
{
    ULONGLONG qwUserID;
    BOOL fFlushTickets;
};

struct P_LIST_CHANGE_ITEM
{
    DWORD dwListVersion;
    ULONGLONG qwTargetID;
    WORD  wOperationID;
    HRESULT hr;
    WORD  cbTargetAcctName;
    // char szTargetAcctName[ cbTargetAcctName ];  // ASCII string null terminated
};

struct P_PRESENCE_ITEM
{
    ULONGLONG qwBuddyID;
    DWORD dwTitleID;
    DWORD dwState;
    ULONGLONG qwMatchSessionID;
    WORD  cbNickname;
    WORD  cbTitleStuff;
    // byte rgbNickName[];              // Binary
    // byte rgbTitleStuff[];            // Binary
};

struct P_INVITATION_ITEM
{
    ULONGLONG qwHostID;
    ULONGLONG qwMatchSessionID;
    DWORD dwTitleID;
};

struct P_INVITATION_ANSWER_ITEM
{
    ULONGLONG qwInviteeID;
    ULONGLONG qwMatchSessionID;
    WORD  wAnswer;
};

#pragma pack(pop)

#endif // __cplusplus

// ====================================================================
// XOnline Matchmaking
//

#define X_ATTRIBUTE_RESERVED1_MASK          0x70000000
#define X_ATTRIBUTE_RESERVED2_MASK          0x000F0000
#define X_ATTRIBUTE_RESERVED3_MASK          0x0F000000

#define X_MATCH_PROTOCOL_VERSION        0x01000000 // 1.0.0000
#define X_MIN_SEARCH_RESULT_COLUMNS     9

//
//  Reserved title search sproc indices
//
#define X_SSINDEX_GET_SESSION           (DWORD)-1   // special sproc to retrieve session by session id
#define X_SSINDEX_GET_ALL_SESSION_INFO  (DWORD)-2   // special sproc that retrieves all sessions for a given
                                                    // title id, as well as all session attributes

//----------------------------------------------------------------------------+
//  Structures
//

#pragma pack(push, 1)

//
// Internal search result structure
//
typedef struct
{
    ULONG               ulResultLen;
    double              fSelectionProbability;
    ULONG               ulNetworkDistance;
    ULONG               ulRank;
    XMATCH_SEARCHRESULT SearchResult;
} XMATCH_SEARCHRESULT_INTERNAL, *PXMATCH_SEARCHRESULT_INTERNAL;

//
//  This structure defines a session.  When used on the client, will be
//  included in a larger structure pre-pended with an async task struct.
//  The following structure is identical to the host advertisement wire
//  protocol.
//  The raw data for session title, application data and attributes will
//  typically follow this structure in the same area of memory, and on the
//  wire.
//
typedef struct
{
    DWORD               dwMessageLength;
    DWORD               dwProtocolVersion;
    XNKID               SessionID;
    DWORD               dwTitleID;
    XNADDR              HostAddress;
    DWORD               dwPublicAvailable;
    DWORD               dwPrivateAvailable;
    DWORD               dwPublicCurrent;
    DWORD               dwPrivateCurrent;
    DWORD               dwNumAttributes;
} XMATCH_SESSION, *PXMATCH_SESSION;

//
//  This structure defines the information returned to the client
//  after a session has been created.
//
typedef struct
{
    XNKID               SessionID;
    XNKEY               KeyExchangeKey;
} XMATCH_SESSION_INFO, *PXMATCH_SESSION_INFO;

//
//  This structure is passed up to delete a session.
//
typedef struct
{
    DWORD               dwMessageLength;
    DWORD               dwProtocolVersion;
    XNKID               SessionID;
    DWORD               dwTitleID;
} XMATCH_SESSION_DELETE, *PXMATCH_SESSION_DELETE;

//
//  This structure is the body of dead xbox requests.
//

typedef struct
{
    DWORD               dwMessageLength;
    DWORD               dwProtocolVersion;
    SGADDR              sgaddr;
} XMATCH_DEAD_XBOX, *PXMATCH_DEAD_XBOX;

//
//  This structure is the body of dead SG requests.
//

typedef struct
{
    DWORD               dwMessageLength;
    DWORD               dwProtocolVersion;
    SGADDR              sgaddr;
} XMATCH_DEAD_SG, *PXMATCH_DEAD_SG;

//
//  This structure defines the information passed to the server
//  to initiate a search for a game session.
//
typedef struct
{
    DWORD               dwMessageLength;
    DWORD               dwProtocolVersion;
    DWORD               dwTitleID;
    DWORD               dwProcedureIndex;
    DWORD               dwNumParameters;
} XMATCH_SEARCH, *PXMATCH_SEARCH;

#pragma pack(pop)


// ====================================================================
// XOnline Billing
//

//
// Wire structures can't be shared with the c# ISAPIs, so 
// this section has been moved to the top of billing.cpp. Visit us
// there at our new home!  
//

// ====================================================================
// XOnline Statistics
//

#pragma pack(push, 1)

struct XSTATS_MSG_SETDATA
{
    DWORD dwTitleId;
    ULONGLONG ullUserId;
    DWORD dwLeaderBoardType;
    DWORD dwAttrCount;
};

struct XSTATS_MSG_ATTRIBUTEDATA
{
    WORD wAttrId;
    BYTE bAttrType;

    union
    {
        BYTE bValue;
		WORD wValue;
		DWORD dwValue;
		ULONGLONG ullValue;
		float fValue;
		double dValue;             
	};        
};

#pragma pack(pop)


// ====================================================================
// Common sturctures for Kerberos pre-auth and authdata
//

//
// Xbox special ASN.1 Pre-auth data types
//
#define KRB5_PADATA_CLIENT_VERSION          132
#define KRB5_PADATA_XBOX_SERVICE_REQUEST    201
#define KRB5_PADATA_XBOX_SERVICE_ADDRESS    202
#define KRB5_PADATA_XBOX_ACCOUNT_CREATION   203

//
// SALT flags for pre-auth encryption
//
#define KERB_KDC_REQ_NONCE_CKSUM_SALT       1026
#define KERB_PA_CLIENT_VER_SALT             1027
#define KERB_PA_XBOX_SERVICE_REQUEST_SALT   1201
#define KERB_PA_XBOX_SERVICE_ADDRESS_SALT   1202
#define KERB_PA_XBOX_ACCOUNT_CREATION_SALT  1203

//
// Maximum services per logon
//
#define XONLINE_MAX_NUMBER_SERVICE 12

#define KINGDOM_SEPERATOR_CHAR ('.')
#define KINGDOM_SEPERATOR_STRING (".")

#pragma pack(push, 1)

typedef struct
{
    WORD wMajorVersion;
    WORD wMinorVersion;
    WORD wBuildNumber;
    WORD wQFENumber;
} XBOX_LIBRARY_VERSION;

//
// Pre-auth structure sent from Xbox to KDC 
// during Xbox TGS cycle.
// Encrypted with Session Key of TGT
//
typedef struct _XKERB_PA_XBOX_SERVICE_REQUEST
{
    #define XONLINE_SERVICE_REQUEST_VERSION 1
    WORD                    wServiceRequestVersion;
    BYTE					abEthernetAddr[6];
    XBOX_LIBRARY_VERSION    clientVersion;
    DWORD                   dwTitleID;
    DWORD                   dwTitleVersion;
    DWORD                   dwTitleRegion;

    //
    // Specifies the qwUserIDs in the order that they should be in
    // the returned XKERB_PA_XBOX_SERVICE_ADDRESS and XKERB_AD_XBOX.
    // The Xbox KDC will check that the qwUserIDs in this list
    // are all specified in the TGT.
    //
    ULONGLONG               qwUserID[XONLINE_MAX_LOGON_USERS];
    
    DWORD                   dwNumServices;
    DWORD                   dwServiceID[XONLINE_MAX_NUMBER_SERVICE];
} XKERB_PA_XBOX_SERVICE_REQUEST, *PXKERB_PA_XBOX_SERVICE_REQUEST;

typedef struct _XKERB_PA_XBOX_SERVICE_RESULT
{
    DWORD       dwServiceID;
    HRESULT     hr;             // S_FALSE means the service is at another site
    WORD        wServicePort;     // Only if hr == S_OK
    WORD        wReserved;
} XKERB_PA_XBOX_SERVICE_RESULT;

//
// Pre-auth structure sent from Xbox KDC 
// to Xbox client during TGS cycle reply.
// Encrypted with Session Key of TGT
//
typedef struct _XKERB_PA_XBOX_SERVICE_ADDRESS
{
    HRESULT     hr;
    HRESULT     hrUser[XONLINE_MAX_LOGON_USERS];
    // The high 16 bits of dwUserFlags should come from the UODB User Table.
    DWORD       dwUserFlags[XONLINE_MAX_LOGON_USERS];
    IN_ADDR     siteIPAddress;
    DWORD       dwNumServices;
    XKERB_PA_XBOX_SERVICE_RESULT     serviceResult[XONLINE_MAX_NUMBER_SERVICE];
} XKERB_PA_XBOX_SERVICE_ADDRESS, *PXKERB_PA_XBOX_SERVICE_ADDRESS;

//
// Pre-auth structure sent back from MACS server
// for newly created machine account
// Encrypted with online key
//
typedef struct _XKERB_PA_XBOX_ACCOUNT_CREATION
{
    ULONGLONG qwUserID;
    CHAR name[XONLINE_NAME_SIZE];
    CHAR kingdom[XONLINE_KINGDOM_SIZE];
    CHAR domain[XONLINE_USERDOMAIN_SIZE];
    CHAR realm[XONLINE_REALM_NAME_SIZE];
    BYTE key[XONLINE_KEY_LENGTH];
} XKERB_PA_XBOX_ACCOUNT_CREATION, *PXKERB_PA_XBOX_ACCOUNT_CREATION;

#define KERB_AUTH_DATA_XBOX  200

//
// auth-data structure in every TKT issued from the Xbox KDC
//
typedef struct _XKERB_AD_XBOX
{
    #define XONLINE_AUTHDATA_VERSION 1
    WORD                     wAuthDataVersion; 
    BYTE					 abEthernetAddr[6];
    DWORD                    dwAuthDataSize; 
    XBOX_LIBRARY_VERSION     clientVersion;
    DWORD                    dwTitleID;
    DWORD                    dwTitleVersion;
    DWORD                    dwTitleRegion;
    ULONGLONG                qwXboxID;
    // The high 16 bits of dwUserFlags should come from the UODB User Table.
    XUID                     users[XONLINE_MAX_LOGON_USERS]; 
    DWORD                    dwNumServices;
    DWORD                    dwServiceID[XONLINE_MAX_NUMBER_SERVICE];
} XKERB_AD_XBOX, *PXKERB_AD_XBOX;

typedef struct _XKERB_AD_XBOX_WITH_SIGNATURE
{
    BYTE                     ServerSignature[XONLINE_KEY_LENGTH];
    XKERB_AD_XBOX             authData;
} XKERB_AD_XBOX_WITH_SIGNATURE, *PXKERB_AD_XBOX_WITH_SIGNATURE;

#pragma pack(pop)

// ---------------------------------------------------------------------------------------
// XB/SG/Server shared definitions
// ---------------------------------------------------------------------------------------

#ifdef __cplusplus

#include <pshpack1.h>

struct CAuthData : public XKERB_AD_XBOX
{
    UINT            GetCb()             { return(dwAuthDataSize); }
    void            SetCb(UINT cb)      { dwAuthDataSize = (DWORD)cb; }
    UINT            GetSvcIdCount()     { return(dwNumServices); }
    DWORD *         GetSvcIdVector()    { return(dwServiceID); }
};

struct CKeyExHdr
{
    // Definitions -------------------------------------------------------------------

    #define KEYEX_TYPE_XBTOXB_INIT  0x0100  // CKeyExXbToXb | DH_GX | HMAC_SHA
    #define KEYEX_TYPE_XBTOXB_RESP  0x0101  // CKeyExXbToXb | DH_GX | HMAC_SHA
    #define KEYEX_TYPE_XBTOSG_INIT  0x0102  // CKeyExXbToSgInit | DH_GX | APREQ
    #define KEYEX_TYPE_SGTOXB_RESP  0x0103  // CKeyExSgToXbResp | DH_GX | APREP
    #define KEYEX_TYPE_NATOPEN_INIT 0x0104  // CKeyExNatOpen
    #define KEYEX_TYPE_NATOPEN_RESP 0x0105  // CKeyExNatOpen

    #define KEYEX_TYPE_DH_GX        0x0110  // diffie-hellman g^X (or g^Y)
    #define KEYEX_TYPE_HMAC_SHA     0x0111  // HMAC-SHA-1 digest (XbToXb)
    #define KEYEX_TYPE_KERB_APREQ   0x0112  // CKeyExKerbApReq (XbToSg)
    #define KEYEX_TYPE_KERB_APREP   0x0113  // CKeyExKerbApRep (SgToXb)
    #define KEYEX_TYPE_NULL_APREQ   0x0114  // CKeyExNullApReq (XbToSg; insecure; test only)
    #define KEYEX_TYPE_NULL_APREP   0x0115  // CKeyExNullApRep (SgToXb; insecure; test only)

    // Data --------------------------------------------------------------------------

    WORD            _wType;             // See KEYEX_TYPE_* above
    WORD            _cbEnt;             // Size of this entry (including header)
};

struct CKeyExXbToSgInit : public CKeyExHdr
{
    // Definitions -----------------------------------------------------------------------

    #define XSIF_CONNECTION_SERVICE 0x00000001  // Initiator requests connection service

    #define KEYEX_USER_GUEST_MASK         0x1F
    #define KEYEX_USER_PERMUTE_MASK       0x20
    #define KEYEX_USER_PERMUTATION_SHIFT     6
    
    // Data ------------------------------------------------------------------------------

    DWORD           _dwFlags;                   // See XSIF_* above
    DWORD           _dwSpiInit;                 // SPI of the initiator
    BYTE            _abNonceInit[8];            // Nonce of the initiator
    DWORD           _dwUserPerm;                // Permutation and guest bits for all 4 users
};

struct CKeyExSgToXbResp : public CKeyExHdr
{
    // Definitions -----------------------------------------------------------------------

    #define SXRF_ENCRYPT_DES        0x00000001  // SG wants DES encryption
    #define SXRF_ENCRYPT_3DES       0x00000002  // SG wants 3DES encryption

    // Data ------------------------------------------------------------------------------

    DWORD           _dwFlags;                   // See SXRF_* above
    DWORD           _dwSpiInit;                 // SPI of the initiator
    DWORD           _dwSpiResp;                 // SPI of the responder
    BYTE            _abNonceInit[8];            // Nonce of the initiator
    BYTE            _abNonceResp[8];            // Nonce of the responder
    WORD            _wXbToSgTimeoutInSecs;      // Secs of no inbound data/pulse before disconnecting
    WORD            _wXbToSgPulseTimeoutInSecs; // Secs of no outbound data/pulse before sending SECMSG_TYPE_XBTOSGPULSE
    SGADDR          _sgaddrInit;                // SGADDR assigned to the initiator
    IN_ADDR         _inaInit;                   // IP address of initiator (as seen by SG)
    WORD            _wPortInit;                 // IP port of initiator (as seen by SG)
};

struct CKeyExKerbApReq : public CKeyExHdr
{
    // Definitions -----------------------------------------------------------------------

    #define CBKERBAPREQ             800         // Maximum size of Kerberos Ap request

    // Data ------------------------------------------------------------------------------

    BYTE            _ab[CBKERBAPREQ];           // Kerberos AP request
};

struct CKeyExKerbApRep : public CKeyExHdr
{
    // Definitions -----------------------------------------------------------------------

    #define CBKERBAPREP             800         // Maximum size of Kerberos Ap reply

    // Data ------------------------------------------------------------------------------

    BYTE            _ab[CBKERBAPREP];           // Kerberos AP reply
};

struct CKeyExNullApReq : public CKeyExHdr
{
    BYTE            _abSha[20];                 // SHA of the rest of key-exchange message
    LARGE_INTEGER   _liTime;                    // Authenticator time of client
    CAuthData       _AuthData;                  // CAuthData of client
};

struct CKeyExNullApRep : public CKeyExHdr
{
    BYTE            _abSha[20];                 // SHA of the rest of key-exchange message
};

struct CSecMsgHdr
{
    // Definitions -----------------------------------------------------------------------

    #define SECMSG_TYPE_DELETE          0x0200  // CSecMsgDelete
    #define SECMSG_TYPE_PULSE           0x0201  // CSecMsgPulse
    #define SECMSG_TYPE_XBTOSG_PULSE    0x0202  // CSecMsgXbToSgPulse
    #define SECMSG_TYPE_SGTOXB_PULSE    0x0203  // CSecMsgSgToXbPulse
    #define SECMSG_TYPE_XBTOXB_FORWARD  0x0204  // CSecMsgXbToXbForward

    // Data ------------------------------------------------------------------------------

    WORD            _wType;                     // See SECMSG_TYPE_* above
    WORD            _cbEnt;                     // Size of this entry (including header)

};

struct CSecMsgDelete : public CSecMsgHdr
{
    // Definitions -----------------------------------------------------------------------

    #define SECMSG_DELETE_SHUTDOWN      0x0000  // Client is shutting down or logging off
    #define SECMSG_DELETE_UNREGISTER    0x0001  // Client unregistered key
    #define SECMSG_DELETE_KICK          0x0002  // Client has been kicked off the system

    // Data ------------------------------------------------------------------------------

    DWORD           _dwReason;                  // See SECMSG_DELETE_* above

};

struct CSecMsgPulse : public CSecMsgHdr
{
    // No additional data members
};

struct CSecMsgXbToSgPulse : public CSecMsgHdr
{
    // Definitions -----------------------------------------------------------------------

    #define XBPULSE_USER_INDEX_MASK     0x03    // Specifies which user index changed
    #define XBPULSE_STATE_CHANGE        0x04    // dwUserState changed
    #define XBPULSE_XNKID_CHANGE        0x08    // xnkidSession changed
    #define XBPULSE_TDATA_CHANGE        0x10    // abTitleData changed

    // Data ------------------------------------------------------------------------------

    DWORD           _dwSeqAck;                  // Acknowledgement sequence number

    // The rest of this entry is filled with a padded series of changes.  The first byte 
    // contains the user-index (low two bits), and an indication of what else follows.
    // If the STATE_CHANGE bit is set, four bytes of dwUserState comes next.  Then if
    // the XNKID_CHANGE bit is set, eight bytes of XNKID comes next.  Then if the 
    // TDATA_CHANGE bit is set, the title data comes next.  The size of the title data
    // is encoded in one byte and the contents comes next.

};

struct CSecMsgSgToXbPulse : public CSecMsgHdr
{
    // Definitions -----------------------------------------------------------------------

    #define SGPULSE_USER_INDEX_MASK     0x03    // Specifies which user index changed
    #define SGPULSE_QFLAGS_CHANGE       0x04    // dwQueueFlags changed

    // Data ------------------------------------------------------------------------------

    DWORD           _dwSeqAck;                  // Acknowledgement sequence number

    // The rest of this entry is filled with a padded series of changes.  The first byte
    // contains the user-index (low two bits), and an indication of what else follows.
    // If the QFLAGS_CHANGED bit is set, four bytes of dwQueueFlags comes next.

};

struct CSecMsgXbToXbForward : public CSecMsgHdr
{
    SGADDR          _sgaddr;                    // SGADDR of the target client

    // The rest of this entry contains the actual message being forwarded.
};

struct CSgMsgHdr
{
    // Definitions -------------------------------------------------------------------

    #define SGMSG_TYPE_AUTHDATA_REQ     0x0300  // CSgMsgAuthReq
    #define SGMSG_TYPE_AUTHDATA_REP     0x0301  // CSgMsgAuthRep
    #define SGMSG_TYPE_AUTHDATA         0x0302  // CAuthData in the payload
    #define SGMSG_TYPE_SET_QFLAGS_LAZY  0x0303  // CSgMsgSetQFlagsReq
    #define SGMSG_TYPE_SET_QFLAGS_PUSH  0x0304  // CSgMsgSetQFlagsReq
    #define SGMSG_TYPE_SET_QFLAGS_REP   0x0305  // CSgMsgSetQFlagsRep
    #define SGMSG_TYPE_KICK_REQ         0x0306  // CSgMsgKickReq
    #define SGMSG_TYPE_KICK_REP         0x0307  // CSgMsgKickRep
    #define SGMSG_TYPE_CLIENT_ADD       0x0308  // CSgMsgClient
    #define SGMSG_TYPE_CLIENT_DEL       0x0309  // CSgMsgClient
    #define SGMSG_TYPE_CLIENT_UPD       0x030A  // CSgMsgClient
    #define SGMSG_TYPE_XBTOXB_FORWARD   0x030B  // CSgMsgXbToXbForward

    // Data --------------------------------------------------------------------------

    WORD            _wType;                     // See SGMSG_TYPE_* above
    WORD            _cbEnt;                     // Size of this entry (including header)
};

struct CSgMsgAuthReq : public CSgMsgHdr
{
    DWORD           _dwReqNum;                  // Request number from requester
    IN_ADDR         _ipaZ;                      // IP address of the client on the DMZ
    WORD            _ipportZ;                   // IP port of the client on the DMZ
};

struct CSgMsgAuthRep : public CSgMsgHdr
{
    // Definitions -----------------------------------------------------------------------

    #define CBSGAUTHREPMSG  (sizeof(CSgMsgAuthRep)+sizeof(CSgMsgHdr)+sizeof(CAuthData))

    // Data ------------------------------------------------------------------------------

    DWORD           _dwReqNum;                  // Copy of _dwReqNum from request
    IN_ADDR         _ipaI;                      // IP address of the client on the Internet
    IN_ADDR         _ipaZ;                      // IP address of the client on the DMZ
    WORD            _ipportZ;                   // IP port of the client on the DMZ
    BYTE            _fNotFound;                 // TRUE if client not found (no auth-data enclosed)
};

struct CSgMsgSetQFlagsReq : public CSgMsgHdr
{
    SGADDR          _sgaddr;                    // SGADDR of the client
    ULONGLONG       _qwUserId;                  // User Id of the user
    DWORD           _dwQFlags;                  // User flags to set
    DWORD           _dwSeqQFlags;               // Sequence number of this update
};

struct CSgMsgSetQFlagsRep : public CSgMsgHdr
{
    SGADDR          _sgaddr;                    // SGADDR of the client
    ULONGLONG       _qwUserId;                  // User id of the user
    DWORD           _dwSeqQFlags;               // Sequence number of this update
    BYTE            _fNotFound;                 // TRUE if client not found
    BYTE            _fNoSuchUser;               // TRUE if SG doesn't know about qwUserId
};

struct CSgMsgKickReq : public CSgMsgHdr
{
    SGADDR          _sgaddr;                    // SGADDR of the client to kick
};

struct CSgMsgKickRep : public CSgMsgHdr
{
    SGADDR          _sgaddr;                    // SGADDR of the client that was kicked
    BYTE            _fNotFound;                 // TRUE if client not found
};

struct CSgMsgClient : public CSgMsgHdr
{
    // Definitions -----------------------------------------------------------------------

    struct CUpdate
    {
        ULONGLONG   _qwUserId;                  // User Id of the user
        DWORD       _dwPState;                  // Presence state
        XNKID       _xnkid;                     // Game session user is currently playing
        UINT        _cbData;                    // Count of bytes of title data
        BYTE        _abData[MAX_TITLE_STATE_BYTES]; // Custom presence title data
    };

    // Data ------------------------------------------------------------------------------

    SGADDR          _sgaddr;                    // SGADDR of the client
    IN_ADDR         _inaI;                      // IN_ADDR of the client
    IN_ADDR         _inaZ;                      // NAT'd IN_ADDR of the client

    // The rest of this entry contains a series of CUpdate records associated with the client,
    // if the type is SGMSG_TYPE_CLIENT_UPD.
};

struct CSgMsgXbToXbForward : public CSgMsgHdr
{
    SGADDR          _sgaddr;                    // SGADDR of the target client
    
    // The rest of this entry contains the actual message being forwarded.
};

#include <poppack.h>

#endif // __cplusplus

//@@BEGIN_CLIENTONLY

// ---------------------------------------------------------------------------------------
// XOnline Api List
// ---------------------------------------------------------------------------------------

#define XONLINEAPILIST() \
    XONAPI_(HRESULT,                XOnlineStartup, (PXONLINE_STARTUP_PARAMS pxosp), (pxosp)) \
    XONAPI_(HRESULT,                XOnlineCleanup, (), ()) \
    XONAPI (HRESULT,                XOnlineTaskContinue, (XONLINETASK_HANDLE hTask), (hTask)) \
    XONAPI (HRESULT,                XOnlineTaskClose, (XONLINETASK_HANDLE hTask), (hTask)) \
    XONAPI (HRESULT,                XOnlineGetUsers, (PXONLINE_USER pUsers, DWORD * pcUsers), (pUsers, pcUsers)) \
    XONAPI (XPININPUTHANDLE,        XOnlinePINStartInput, (PXINPUT_STATE pInputState), (pInputState)) \
    XONAPI (HRESULT,                XOnlinePINDecodeInput, (XPININPUTHANDLE handle, PXINPUT_STATE pInputState, PBYTE pPINByte), (handle, pInputState, pPINByte)) \
    XONAPI (HRESULT,                XOnlinePINEndInput, (XPININPUTHANDLE handle), (handle)) \
    XONAPI (HRESULT,                XOnlineLogon, (PXONLINE_USER pUsers, DWORD* pdwServiceIDs, DWORD cServices, HANDLE hEvent, PXONLINETASK_HANDLE pHandle), (pUsers, pdwServiceIDs, cServices, hEvent, pHandle)) \
    XONAPI (HRESULT,                XOnlineLogonTaskGetResults, (XONLINETASK_HANDLE hLogonTask), (hLogonTask)) \
    XONAPI (PXONLINE_USER,          XOnlineGetLogonUsers, (), ()) \
    XONAPI (HRESULT,                XOnlineGetServiceInfo, (DWORD dwServiceID, PXONLINE_SERVICE_INFO pServiceInfo), (dwServiceID, pServiceInfo)) \
    XONAPI (DWORD,                  XOnlineLaunchNewImage, (LPCSTR lpImagePath, PLAUNCH_DATA pLaunchData), (lpImagePath, pLaunchData)) \
    XONAPI (HRESULT,                XOnlineTitleUpdate, (DWORD dwContext), (dwContext)) \
    XONAPI (HRESULT,                XOnlineTitleUpdateFromDVD, (DWORD dwContext, PSTR szUpdatePath, DWORD dwTitleId, PBYTE pbSymmetricKey, DWORD cbSymmetricKey, PBYTE pbPublicKey, DWORD cbPublicKey), (dwContext, szUpdatePath, dwTitleId, pbSymmetricKey, cbSymmetricKey, pbPublicKey, cbPublicKey)) \
    XONAPI (HRESULT,                XOnlineTitleUpdateInternal, (DWORD dwTitleId, DWORD dwTitleOldVersion, PBYTE pbTitleKey, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (dwTitleId, dwTitleOldVersion, pbTitleKey, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineTitleUpdateFromDVDInternal, (PSTR szUpdatePath, DWORD dwTitleId, DWORD dwTitleOldVersion, PBYTE pbTitleKey, PBYTE pbSymmetricKey, DWORD cbSymmetricKey, PBYTE pbPublicKey, DWORD cbPublicKey, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (szUpdatePath, dwTitleId, dwTitleOldVersion, pbTitleKey, pbSymmetricKey, cbSymmetricKey, pbPublicKey, cbPublicKey, hWorkEvent, phTask))\
    XONAPI (HRESULT,                XOnlineTitleUpdateGetProgress, (XONLINETASK_HANDLE hTask, DWORD * pdwPercentDone, ULONGLONG *pqwNumerator, ULONGLONG *pqwDenominator), (hTask, pdwPercentDone, pqwNumerator, pqwDenominator)) \
    XONAPI (HRESULT,                XOnlineOfferingPurchase, (DWORD dwUserIndex, XONLINEOFFERING_ID OfferingId, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwUserIndex, OfferingId, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineOfferingCancel, (DWORD dwUserIndex, XONLINEOFFERING_ID OfferingId, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwUserIndex, OfferingId, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineOfferingGetDetails, (DWORD dwUserIndex, XONLINEOFFERING_ID OfferingId, WORD wLanguage, DWORD dwDescriptionIndex, PBYTE pbBuffer, DWORD cbBuffer, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwUserIndex, OfferingId, wLanguage, dwDescriptionIndex, pbBuffer, cbBuffer, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineOfferingDetailsGetResults, (XONLINETASK_HANDLE hTask, PBYTE * pDetailsBuffer, DWORD * pcbDetailsLength, XONLINE_PRICE * pGrossPrice, XONLINE_PRICE * pNetPrice), (hTask, pDetailsBuffer, pcbDetailsLength, pGrossPrice, pNetPrice)) \
    XONAPI (DWORD,                  XOnlineOfferingDetailsMaxSize, (DWORD cbTitleSpecificDataMaxSize), (cbTitleSpecificDataMaxSize)) \
    XONAPI (HRESULT,                XOnlineOfferingPriceFormat, (XONLINE_PRICE * Price, LPWSTR lpwszFormattedPrice, DWORD *cbLength, DWORD dwExtendedCharsFilter), (Price, lpwszFormattedPrice, cbLength, dwExtendedCharsFilter)) \
    XONAPI (HRESULT,                XOnlineOfferingVerifyLicense, (DWORD dwOfferingId, DWORD dwUserAccounts, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwOfferingId, dwUserAccounts, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineOfferingEnumerate, (XONLINEOFFERING_ENUM_DEVICE Device, DWORD dwUserIndex, PXONLINEOFFERING_ENUM_PARAMS pEnumParams, PBYTE pbBuffer, DWORD cbBuffer, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask), (Device, dwUserIndex, pEnumParams, pbBuffer, cbBuffer, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineOfferingEnumerateGetResults, (XONLINETASK_HANDLE hTask, PXONLINEOFFERING_INFO **prgpOfferingInfo, DWORD *pdwReturnedResults, BOOL *pfMoreResults), (hTask, prgpOfferingInfo, pdwReturnedResults, pfMoreResults)) \
    XONAPI (DWORD,                  XOnlineOfferingEnumerateMaxSize, (PXONLINEOFFERING_ENUM_PARAMS pEnumParams, DWORD cbTitleSpecificDataMaxSize), (pEnumParams, cbTitleSpecificDataMaxSize)) \
    XONAPI (HRESULT,                XOnlineFriendsGetResults, (DWORD *pdwUserIndex, XUID *pxuidTargetUser), (pdwUserIndex, pxuidTargetUser)) \
    XONAPI (HRESULT,                XOnlineContentInstall, (XONLINEOFFERING_ID OfferingId, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (OfferingId, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineContentInstallFromDVD, (PSTR szResourcePath, DWORD dwTitleId, XONLINEOFFERING_ID OfferingId, DWORD dwBitFlags, PBYTE pbSymmetricKey, DWORD cbSymmetricKey, PBYTE pbPublicKey, DWORD cbPublicKey, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask), (szResourcePath, dwTitleId, OfferingId, dwBitFlags, pbSymmetricKey, cbSymmetricKey, pbPublicKey, cbPublicKey, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineContentInstallGetProgress, (XONLINETASK_HANDLE hTask, DWORD * pdwPercentDone, ULONGLONG *pqwNumerator, ULONGLONG *pqwDenominator), (hTask, pdwPercentDone, pqwNumerator, pqwDenominator)) \
    XONAPI (HRESULT,                XOnlineContentVerify, (XONLINEOFFERING_ID OfferingId, PBYTE pbBuffer, DWORD * pcbBuffer, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (OfferingId, pbBuffer, pcbBuffer, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineContentRemove, (XONLINEOFFERING_ID OfferingId, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (OfferingId, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineContentGetRootDirectory, (XONLINEOFFERING_ID OfferingId, BOOL fUserData, DWORD * pcbRootDirectory, CHAR * szRootDirectory), (OfferingId, fUserData, pcbRootDirectory, szRootDirectory)) \
    XONAPI (HRESULT,                XOnlineMatchSessionCreate, (DWORD dwPublicCurrent, DWORD dwPublicAvailable, DWORD dwPrivateCurrent, DWORD dwPrivateAvailable, DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (dwPublicCurrent, dwPublicAvailable, dwPrivateCurrent, dwPrivateAvailable, dwNumAttributes, pAttributes, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineMatchSessionUpdate, (XNKID SessionID, DWORD dwPublicCurrent, DWORD dwPublicAvailable, DWORD dwPrivateCurrent, DWORD dwPrivateAvailable, DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (SessionID, dwPublicCurrent, dwPublicAvailable, dwPrivateCurrent, dwPrivateAvailable, dwNumAttributes, pAttributes, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineMatchSessionGetInfo, (XONLINETASK_HANDLE hTask, XNKID * pSessionID, XNKEY * pKeyExchangeKey), (hTask, pSessionID, pKeyExchangeKey)) \
    XONAPI (HRESULT,                XOnlineMatchSessionDelete, (XNKID SessionID, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (SessionID, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineMatchSessionFindFromID, (XNKID SessionID, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (SessionID, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineMatchSearch, (DWORD dwProcedureIndex, DWORD dwNumResults, DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes, DWORD dwResultsLen, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (dwProcedureIndex, dwNumResults, dwNumAttributes, pAttributes, dwResultsLen, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineMatchSearchGetResults, (XONLINETASK_HANDLE hTask, PXMATCH_SEARCHRESULT ** prgpSearchResults, DWORD * pdwReturnedResults), (hTask, prgpSearchResults, pdwReturnedResults)) \
    XONAPI (HRESULT,                XOnlineMatchSearchParse, (PXMATCH_SEARCHRESULT pSearchResult, DWORD dwNumSessionAttributes, PXONLINE_ATTRIBUTE_SPEC pSessionAttributeSpec, PVOID pQuerySession), (pSearchResult, dwNumSessionAttributes, pSessionAttributeSpec, pQuerySession)) \
    XONAPI (BOOL,                   XOnlineNotificationSetState, (DWORD dwUserIndex, DWORD dwStateFlags, XNKID sessionID, DWORD cbStateData, PBYTE pStateData), (dwUserIndex, dwStateFlags, sessionID, cbStateData, pStateData)) \
    XONAPI (HRESULT,                XOnlineNotificationSetUserData, (DWORD dwUserIndex, DWORD cbUserData, PBYTE pUserData, HANDLE hEvent, PXONLINETASK_HANDLE phTask), (dwUserIndex, cbUserData, pUserData, hEvent, phTask)) \
    XONAPI (BOOL,                   XOnlineNotificationIsPending, (DWORD dwUserIndex, DWORD dwType), (dwUserIndex, dwType)) \
    XONAPI (HRESULT,                XOnlineNotificationEnumerate, (DWORD dwUserIndex, PXONLINE_NOTIFICATION_MSG pbBuffer, WORD wBufferCount, DWORD dwNotificationTypes, HANDLE hEvent, PXONLINETASK_HANDLE phTask), (dwUserIndex, pbBuffer, wBufferCount, dwNotificationTypes, hEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineFriendsStartup, (HANDLE hEvent, PXONLINETASK_HANDLE phTask), (hEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineFriendsEnumerate, (DWORD dwUserIndex, HANDLE hEvent, PXONLINETASK_HANDLE phTask), (dwUserIndex, hEvent, phTask)) \
    XONAPI (DWORD,                  XOnlineFriendsGetLatest, (DWORD dwUserIndex, DWORD dwFriendBufferCount, PXONLINE_FRIEND pFriendBuffer), (dwUserIndex, dwFriendBufferCount, pFriendBuffer)) \
    XONAPI (HRESULT,                XOnlineFriendsRemove, (DWORD dwUserIndex, PXONLINE_FRIEND pFriend), (dwUserIndex, pFriend)) \
    XONAPI (HRESULT,                XOnlineFriendsRequest, (DWORD dwUserIndex, XUID xuidToUser), (dwUserIndex, xuidToUser)) \
    XONAPI (HRESULT,                XOnlineFriendsRequestByName, (DWORD dwUserIndex, PSTR pszUserName), (dwUserIndex, pszUserName)) \
    XONAPI (HRESULT,                XOnlineFriendsGameInvite, (DWORD dwUserIndex, XNKID SessionID, DWORD dwFriendListCount, PXONLINE_FRIEND pToFriendList), (dwUserIndex, SessionID, dwFriendListCount, pToFriendList)) \
    XONAPI (HRESULT,                XOnlineFriendsRevokeGameInvite, (DWORD dwUserIndex, XNKID SessionID, DWORD dwFriendListCount, PXONLINE_FRIEND pToFriendList), (dwUserIndex, SessionID, dwFriendListCount, pToFriendList)) \
    XONAPI (HRESULT,                XOnlineFriendsAnswerRequest, (DWORD dwUserIndex, PXONLINE_FRIEND pToFriend, XONLINE_REQUEST_ANSWER_TYPE Answer), (dwUserIndex, pToFriend, Answer)) \
    XONAPI (HRESULT,                XOnlineFriendsAnswerGameInvite, (DWORD dwUserIndex, PXONLINE_FRIEND pToFriend, XONLINE_GAMEINVITE_ANSWER_TYPE Answer), (dwUserIndex, pToFriend, Answer)) \
    XONAPI (HRESULT,                XOnlineFriendsGetAcceptedGameInvite, (HANDLE hEvent, PXONLINETASK_HANDLE phTask), (hEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineFriendsGetAcceptedGameInviteResult, (XONLINETASK_HANDLE hTask, PXONLINE_ACCEPTED_GAMEINVITE pAcceptedGameInvite), (hTask, pAcceptedGameInvite)) \
    XONAPI (DWORD,                  XOnlineLockoutlistGet, (DWORD dwUserIndex, DWORD dwLockoutUserBufferCount, PXONLINE_LOCKOUTUSER pLockoutUserBuffer), (dwUserIndex, dwLockoutUserBufferCount, pLockoutUserBuffer)) \
    XONAPI (HRESULT,                XOnlineLockoutlistStartup, (HANDLE hEvent, PXONLINETASK_HANDLE phTask), (hEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineLockoutlistAdd, (DWORD dwUserIndex, XUID xUserID, CHAR *pszUsername), (dwUserIndex, xUserID, pszUsername)) \
    XONAPI (HRESULT,                XOnlineLockoutlistRemove, (DWORD dwUserIndex, XUID xUserID), (dwUserIndex, xUserID)) \
    XONAPI (HRESULT,                XOnlineFeedbackSend, (DWORD dwUserIndex, XUID xTargetUser, XONLINE_FEEDBACK_TYPE FeedbackType, LPCWSTR lpszNickname, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (dwUserIndex, xTargetUser, FeedbackType, lpszNickname, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineEnumerateTitlesBegin, (DWORD dwMaxResults, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwMaxResults, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineEnumerateTitlesContinue, (XONLINETASK_HANDLE hTask), (hTask)) \
    XONAPI (HRESULT,                XOnlineEnumerateTitlesGetResults, (XONLINETASK_HANDLE hTask, DWORD ** prgTitleID, DWORD * pdwTitleID), (hTask, prgTitleID, pdwTitleID)) \
    XONAPI (HRESULT,                XOnlineDownloadToMemory, (DWORD dwServiceID, LPCSTR szResourcePath, PBYTE pbBuffer, DWORD cbBuffer, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, DWORD dwTimeout, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwServiceID, szResourcePath, pbBuffer, cbBuffer, pbExtraHeaders, cbExtraHeaders, dwTimeout, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineDownloadFile, (DWORD dwServiceID, LPCSTR szResourcePath, PBYTE pbBuffer, DWORD cbBuffer, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, DWORD dwFlags, LPCSTR szLocalPath, DWORD dwTimeout, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwServiceID, szResourcePath, pbBuffer, cbBuffer, pbExtraHeaders, cbExtraHeaders, dwFlags, szLocalPath, dwTimeout, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineDownloadGetProgress, (XONLINETASK_HANDLE hTask, DWORD * pdwPercentDone, ULARGE_INTEGER * puliNumerator, ULARGE_INTEGER * puliDenominator), (hTask, pdwPercentDone, puliNumerator, puliDenominator)) \
    XONAPI (HRESULT,                XOnlineDownloadGetResults, (XONLINETASK_HANDLE hTask, LPBYTE * ppbBuffer, DWORD * pcbBuffer, ULARGE_INTEGER * puliTotalReceived, ULARGE_INTEGER * puliContentLength, DWORD * pdwExtendedStatus, FILETIME * pftTimestamp, FILETIME * pftLastModified), (hTask, ppbBuffer, pcbBuffer, puliTotalReceived, puliContentLength, pdwExtendedStatus, pftTimestamp, pftLastModified)) \
    XONAPI (HRESULT,                XOnlineUploadFromMemory, (DWORD dwServiceID, LPCSTR szTargetPath, PBYTE pbBuffer, DWORD * pcbBuffer, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, PBYTE pbDataToUpload, DWORD cbDataToUpload, DWORD dwTimeout, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwServiceID, szTargetPath, pbBuffer, pcbBuffer, pbExtraHeaders, cbExtraHeaders, pbDataToUpload, cbDataToUpload, dwTimeout, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineUploadFile, (DWORD dwServiceID, LPCSTR szTargetPath, PBYTE pbBuffer, DWORD * pcbBuffer, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, LPCSTR szLocalPath, DWORD dwTimeout, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask), (dwServiceID, szTargetPath, pbBuffer, pcbBuffer, pbExtraHeaders, cbExtraHeaders, szLocalPath, dwTimeout, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineUploadGetProgress, (XONLINETASK_HANDLE hTask, DWORD * pdwPercentDone, ULARGE_INTEGER * puliNumerator, ULARGE_INTEGER * puliDenominator), (hTask, pdwPercentDone, puliNumerator, puliDenominator)) \
    XONAPI (HRESULT,                XOnlineUploadGetResults, (XONLINETASK_HANDLE hTask, LPBYTE * ppbBuffer, DWORD * pcbBuffer, ULARGE_INTEGER * puliTotalReceived, ULARGE_INTEGER * puliContentLength, DWORD * pdwExtendedStatus, FILETIME * pftTimestamp), (hTask, ppbBuffer, pcbBuffer, puliTotalReceived, puliContentLength, pdwExtendedStatus, pftTimestamp)) \
    XONAPI (HRESULT,                XOnlineVerifyNickname, (LPCWSTR lpszNickname,  HANDLE hEvent, PXONLINETASK_HANDLE phTask), (lpszNickname, hEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineStatSet, (DWORD dwNumStatSpecs, PXONLINE_STAT_SPEC pStatSpecs, HANDLE hWorkEvent,	PXONLINETASK_HANDLE phTask), (dwNumStatSpecs, pStatSpecs, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineStatGet, (DWORD dwNumStatSpecs, PXONLINE_STAT_SPEC pStatSpecs, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (dwNumStatSpecs, pStatSpecs,  hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineStatLeaderEnumerate, (XUID* pxuidPagePivot, DWORD dwPageStart, DWORD dwPageSize, DWORD dwLeaderboardID, DWORD dwNumStatsPerUser, DWORD *pStatsPerUser, PXONLINE_STAT_USER pUsers, PXONLINE_STAT pStats, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask), (pxuidPagePivot, dwPageStart, dwPageSize, dwLeaderboardID, dwNumStatsPerUser, pStatsPerUser, pUsers, pStats, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                XOnlineStatLeaderEnumerateGetResults, (XONLINETASK_HANDLE hTask, DWORD *pdwReturnedResults), (hTask, pdwReturnedResults)) \
    XONAPI (HRESULT,                _XOnlineGetMachineID, (ULONGLONG* pqwMachineID), (pqwMachineID)) \
    XONAPI (HRESULT,                _XOnlineGetSerialNumber, (CHAR abSerialNumber[12]), (abSerialNumber)) \
    XONAPI (HRESULT,                _XOnlineGetUsersFromHD, (PXONLINE_USER pUsers, DWORD *pcUsers), (pUsers, pcUsers)) \
    XONAPI (HRESULT,                _XOnlineGetUserFromMU, (DWORD dwPort, DWORD dwSlot, PXONLINE_USER pUser), (dwPort, dwSlot, pUser)) \
    XONAPI (HRESULT,                _XOnlineAddUserToHD, (PXONLINE_USER pUser), (pUser)) \
    XONAPI (HRESULT,                _XOnlineSetUserInMU, (CHAR chDrive, PXONLINE_USER pUser), (chDrive, pUser)) \
    XONAPI (HRESULT,                _XOnlineRemoveUserFromHD, (PXONLINE_USER pUser), (pUser)) \
    XONAPI (BOOL,                   _XOnlineIsUserCredentialPresent, (PXONLINE_USER pUser), (pUser)) \
    XONAPI (HRESULT,                _XOnlineOverrideInfo, (LPSTR pszXOnlineIniFile), (pszXOnlineIniFile)) \
    XONAPI (DWORD,                  _XOnlineSetTitleId, (DWORD dwTitleId), (dwTitleId)) \
    XONAPI (DWORD,                  _XOnlineSetTitleVersion, (DWORD dwTitleVersion), (dwTitleVersion)) \
    XONAPI (DWORD,                  _XOnlineSetTitleGameRegion, (DWORD dwTitleGameRegion), (dwTitleGameRegion)) \
    XONAPI (HRESULT,                _XOnlineAccountTempCreate, (PXONLINE_USER pUser, HANDLE hEvent, PXONLINETASK_HANDLE phTask), (pUser, hEvent, phTask)) \
    XONAPI (HRESULT,                _XOnlineAccountTempCreateGetResults, (XONLINETASK_HANDLE hTask, PXONLINE_USER pUser), (hTask, pUser)) \
    XONAPI (HRESULT,                _XOnlineGetTags, (WORD wCountryId, WORD wMaxTags, LPCWSTR pwsName, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask), (wCountryId, wMaxTags, pwsName, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                _XOnlineGetTagsResults, (XONLINETASK_HANDLE hTask, LPWSTR* prgszTags, WORD* pwTagCount), (hTask, prgszTags, pwTagCount)) \
    XONAPI (HRESULT,                _XOnlineReserveName, (LPCWSTR pwsGamerName, LPCWSTR pwsRealm, WORD wMaxNames, WORD wCountryId, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask), (pwsGamerName, pwsRealm, wMaxNames, wCountryId, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                _XOnlineGetReserveNameResults, (XONLINETASK_HANDLE hTask, PUSER_XNAME* ppNames, DWORD* pdwNameCount), (hTask, ppNames, pdwNameCount)) \
    XONAPI (HRESULT,                _XOnlineCreateAccount, (USER_ACCOUNT_INFO* pAccountInfo, HANDLE hWorkEvent, XONLINETASK_HANDLE *phTask), (pAccountInfo, hWorkEvent, phTask)) \
    XONAPI (HRESULT,                _XOnlineGetCreateAccountResults, (XONLINETASK_HANDLE hTask, PXONLINE_USER pUser), (hTask, pUser)) \
    
// If the compiler chokes on the following macro expansion, it means that one or more
// of the prototypes defined in this file is out of sync with the prototype in XONLINEAPILIST.

#undef  XONAPI
#define XONAPI(ret, fname, arglist, paramlist) XBOXAPI ret WINAPI fname arglist;
#undef  XONAPI_
#define XONAPI_(ret, fname, arglist, paramlist) XBOXAPI ret WINAPI fname arglist;

XONLINEAPILIST()

// ---------------------------------------------------------------------------------------
// CXOnline
// ---------------------------------------------------------------------------------------

#if defined(__cplusplus) && defined(XNETAPILIST)

class CXOnline : public CXNet
{
    friend class __single_inheritance CXo;

public:

    // Constructor -----------------------------------------------------------------------

    #ifdef _XBOX
        __forceinline CXOnline(char * pszXbox = NULL) : CXNet(pszXbox) {}
        #define _XONAPI_ WINAPI
    #else
        __forceinline CXOnline(char * pszXbox = NULL) : CXNet(pszXbox) { _pXo = NULL; }
        __forceinline CXo *  GetXo()    { return(_pXo); }
        __forceinline CXo ** GetXoRef() { return(&_pXo); }
        #define _XONAPI_ CXOnline::
    #endif

    // API -------------------------------------------------------------------------------

    #undef  XONAPI
    #undef  XONAPI_

    #ifdef _XBOX
        #define XONAPI(ret, fname, arglist, paramlist) __forceinline ret fname arglist { return(::fname paramlist); }
        #define XONAPI_(ret, fname, arglist, paramlist) __forceinline ret fname arglist { return(::fname paramlist); }
    #else
        #define XONAPI(ret, fname, arglist, paramlist) ret fname arglist;
        #define XONAPI_(ret, fname, arglist, paramlist) ret fname arglist;
    #endif

    XONLINEAPILIST()

    // Data ------------------------------------------------------------------------------

private:

    #ifndef _XBOX
        CXo *   _pXo;
    #endif

};

#endif

//@@END_CLIENTONLY
//@@END_MSINTERNAL

#ifdef __cplusplus
}
#endif

#endif
