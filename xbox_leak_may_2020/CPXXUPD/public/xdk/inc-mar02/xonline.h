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



#ifdef __cplusplus
}
#endif

#endif
