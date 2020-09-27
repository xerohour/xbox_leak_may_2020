/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:      xonline.h
 *  Content:   Xbox online include file
 ***************************************************************************/

#ifndef __XONLINE__
#define __XONLINE__

#ifdef __cplusplus
extern "C" {
#endif


// ====================================================================
// XOnline Error Codes
//

#define FACILITY_XONLINE                 21

#define XONLINE_E_OVERFLOW          _HRESULT_TYPEDEF_(0x80150001L)
#define XONLINE_E_NO_SESSION        _HRESULT_TYPEDEF_(0x80150002L)


// ====================================================================
// XOnline Task Pump
//

//
// Generic task handle type
//
DECLARE_HANDLE(XONLINETASK_HANDLE);
typedef XONLINETASK_HANDLE* PXONLINETASK_HANDLE;

//
// Define the values for supported work flags
//
#define XONLINETASK_WORKFLAG_DONT_CONTINUE      ((DWORD)0x00000001)
#define XONLINETASK_WORKFLAG_PARTIAL_RESULTS    ((DWORD)0x00000002)

//
// Define macros to abstract the work flags
//
#define XONLINETASK_WORKFLAGS_RESET(pdwWorkFlags)       \
                { if (pdwWorkFlags) *(pdwWorkFlags) = 0; }
                
#define XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags)   \
                { if (pdwWorkFlags) *(pdwWorkFlags) |= XONLINETASK_WORKFLAG_DONT_CONTINUE; }
                
#define XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags)       \
                ((((dwWorkFlags) & XONLINETASK_WORKFLAG_DONT_CONTINUE) != 0)? TRUE : FALSE)

#define XONLINETASK_WORKFLAGS_SET_PARTIAL_RESULTS(pdwWorkFlags) \
                { if (pdwWorkFlags) *(pdwWorkFlags) |= XONLINETASK_WORKFLAG_PARTIAL_RESULTS; }
                
#define XONLINETASK_WORKFLAGS_CONTAINS_PARTIAL_RESULTS(dwWorkFlags)     \
                ((((dwWorkFlags) & XONLINETASK_WORKFLAG_PARTIAL_RESULTS) != 0)? TRUE : FALSE)

//
// The set of API that manages work
//

//
// Continuation function to execute the task associated with the handle
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineTaskContinue(
    IN XONLINETASK_HANDLE hTask,
    IN DWORD dwMaxExecutionTimeSlice,
    OUT DWORD *pdwWorkFlags
    );

//
// Function to cancel a task
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineTaskCancel(
    IN XONLINETASK_HANDLE hTask,
    OUT DWORD *pdwWorkFlags
    );

//
// Function to release a task handle, and all its task-allocated resources
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineTaskCloseHandle(
    XONLINETASK_HANDLE hTask
    );

// ====================================================================
// XOnline Authentication
//

#define XONLINE_USERNAME_SIZE                   64
#define XONLINE_MAX_USERNAME_LENGTH             XONLINE_USERNAME_SIZE - 1
#define XONLINE_MAX_TICKET_LENGTH              512
#define XONLINE_MAX_CHALLENGE_LENGTH          1024
#define XONLINE_MAX_ENCRYPTION_HEADER_LENGTH    48
#define XONLINE_KEY_LENGTH                      16
#define XONLINE_USER_RESERVED_LENGTH            24
#define XONLINE_PIN_LENGTH                       4
//#define XONLINE_SERVICE_NAME_LENGTH            4
#define XONLINE_SERVICE_NAME_LENGTH             16 // 16 for now, should be 4
#define XONLINE_MAX_LOGON_USERS                  4
#define XONLINE_MAX_STORED_ONLINE_USERS          8
#define XONLINE_SIGNATURE_LENGTH                16

#define XONLINE_LOCAL_HARD_DRIVE              '\0'

#define XONLINE_USER_OPTION_REQUIRE_PIN          1

//
// Use string names for Service names for M1
//
#if 0

#define XONLINE_CONNECTION_SERVICE          ((DWORD)1)
#define XONLINE_PRESENCE_SERVICE            ((DWORD)2)
#define XONLINE_NOTIFICATION_SERVICE        ((DWORD)3)
#define XONLINE_USER_PROFILE_SERVICE        ((DWORD)4)
#define XONLINE_MATCHMAKING_SERVICE         ((DWORD)5)
#define XONLINE_BILLING_SERVICE             ((DWORD)6)
#define XONLINE_CATALOG_REFERRAL_SERVICE    ((DWORD)7)
#define XONLINE_DOWNLOAD_SERVICE            ((DWORD)8)
#define XONLINE_UPLOAD_SERVICE              ((DWORD)9)

#else

#define XONLINE_CONNECTION_SERVICE          "XCON"
#define XONLINE_PRESENCE_SERVICE            "XPRS"
#define XONLINE_NOTIFICATION_SERVICE        "XNOT"
#define XONLINE_USER_PROFILE_SERVICE        "XUPS"
#define XONLINE_MATCHMAKING_SERVICE         "XMTC"
#define XONLINE_BILLING_SERVICE             "XBIL"
#define XONLINE_CATALOG_REFERRAL_SERVICE    "XCAT"
#define XONLINE_DOWNLOAD_SERVICE            "XDWN"
#define XONLINE_UPLOAD_SERVICE              "XUPL"

#endif

DECLARE_HANDLE(XPININPUTHANDLE);
DECLARE_HANDLE(XSERVICEHANDLE);

//
// The lastLogon field is the most recent local time that this particular user 
// performed a successful logon on this Xbox. The username field is the unique 
// online name associated with the online user. The pin is an optional game pad 
// key combination used to locally block out unauthorized access of the user. 
// The reserved field is used by the internal implementation and should not be 
// modified in any way.
//
typedef struct {
    FILETIME lastLogon;
    CHAR username[XONLINE_USERNAME_SIZE];
    BYTE pin[XONLINE_PIN_LENGTH];
    DWORD dwOptions;
    BYTE reserved[XONLINE_USER_RESERVED_LENGTH];
} XONLINE_USER, *PXONLINE_USER;

typedef struct {
    CHAR serviceName[XONLINE_SERVICE_NAME_LENGTH]; // This will eventual be a DWORD
    HRESULT hr;
    XSERVICEHANDLE handle;
} XONLINE_SERVICE, *PXONLINE_SERVICE;

typedef struct {
    CHAR           serviceName[XONLINE_SERVICE_NAME_LENGTH]; // This will eventual be a DWORD
    FILETIME       ticketExpirationTime;
    DWORD          dwServiceIP;
    WORD           wServicePort;
    WORD           wReserved;
} XONLINE_SERVICE_INFO, *PXONLINE_SERVICE_INFO;

//
// XOnlineSetupAllAccounts is a temporary routine that can be used to setup
// a fake xbox machine account dwNumUserAccount fake xbox user accounts.
// All accounts use the password "password", and have fixed names.
// The accounts are persisted to disk and will be remembered until overwritten.
// This is a temporary function that will be removed later when the real
// account creation code is ready to use.
//
XBOXAPI
void
WINAPI
XOnlineSetupTempAccounts(
    IN BOOL bSetupMachineAccount,
    IN DWORD dwNumUserAccount 
    );

//
// XOnlineHasMachineAccount can be used to test whether an online account has 
// already been established for this Xbox.  If this function returns FALSE, then 
// all online gaming functionality of the title should be disabled and not 
// presented to the user. This function does not communicate with the server to 
// verify if the account is currently valid, that should be accomplished by an 
// XOnlineLogon call.
//
XBOXAPI
BOOL
WINAPI
XOnlineHasMachineAccount();

//
// A fixed set of online users (XONLINE_MAX_STORED_ONLINE_USERS to be exact) can 
// have their information stored on the local Xbox hard drive or each XMU unit. 
// The XOnlineGetUsers function is used to retrieve all of the online users that 
// have their information stored on the local Xbox hard drive or on a particular 
// XMU unit.
//
XBOXAPI
HRESULT
WINAPI
XOnlineGetUsers (
    IN CHAR chDrive,
    OUT PXONLINE_USER pUsers,
    OUT DWORD *pcUsers
    );

//
// The XOnlineStartPINInput function is used to start the PIN collection process 
// for a particular gamepad. The current input state of the gamepad is passed in 
// and a handle is returned for use by XOnlineDecodePINInput and 
// XOnlineEndPINInput.
//
XBOXAPI
XPININPUTHANDLE
WINAPI
XOnlineStartPINInput(
    IN PXINPUT_STATE pInputState
    );

//
// The XOnlineDecodePINInput function is used to decode newly collected gamepad 
// input and possibly return a byte that corresponds to a new PIN code that the 
// user just entered. If S_OK is returned, then a new pin byte is returned.
// If S_FALSE is returned, then no new PIN code byte was entered during this cycle.
// Any other return code represents a hard failure.
//
XBOXAPI
HRESULT
WINAPI
XOnlineDecodePINInput(
    IN XPININPUTHANDLE handle,
    IN PXINPUT_STATE pInputState,
    IN PBYTE pPINByte
    );

//
// The XOnlineEndPINInput function is used to end a gamepad PIN collection 
// process, and release the corresponding memory.
//
XBOXAPI
HRESULT
WINAPI
XOnlineEndPINInput (
    IN XPININPUTHANDLE handle
    );

//
// The XOnlineLogon function is used to start an asynchronous logon process 
// which involves communicating with the authentication server to authenticate 
// users using Kerberos and obtain tickets used for future communication with 
// various requested online services. The service handles returned by this 
// routine can be used to call XRL routines. XOnlineLogon can be called again, 
// but all service handles from the previous logon call will be lost. Therefore, 
// we recommend titles to first collect all user information and service names 
// ahead of time and only make one logon call. If the title supports adding or 
// removing users midway through the game, then the title will need to call 
// XOnlineChangeLogonUsers to change the set of users that are logged on. Titles 
// should prompt new users for their optional PIN code but not bother users that 
// have already logged on. XOnlineLogon is NOT thread safe, thus titles should 
// avoid making a second XOnlineLogon call before the first logon task is 
// completed.
//
XBOXAPI
HRESULT
WINAPI
XOnlineLogon(
    IN PXONLINE_USER pUsers,
    IN PXONLINE_SERVICE pServices,
    IN DWORD cServices,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE pHandle
    );

//
// The XOnlineGetServiceInfo function provides the title developer with 
// information about a logged on service that enables it to perform their own 
// custom communication mechanism.
//
XBOXAPI
PXONLINE_SERVICE_INFO
WINAPI
XOnlineGetServiceInfo(
    IN XSERVICEHANDLE serviceHandle
    );

// ====================================================================
// XOnline Title Update (Security updates)
//

//
// When the asynchronous XOnlineLogon task finally returns ER_NEED_TO_UPDATE. 
// The game title will need to download an update before any online game playing 
// for this title can occur. The way to do that is to call XOnlineUpdateTitle. 
// XOnlineUpdateTitle will download the updated binaries for this game title 
// onto the hard drive and reboot the Xbox. Upon reboot, the ROM will 
// automatically apply the update to the game title and the game should be able 
// to successfully logon from then on (at least until the next update is released
// ), even for future reboots. Before calling XOnlineUpdateTitle, the game title 
// should inform the player that an update is about to happen and that the Xbox 
// will reboot after the update. The player should have the option to NOT update 
// at this time and possibly proceed playing the game without any online 
// functionality.
//
XBOXAPI
HRESULT
WINAPI
XOnlineUpdateTitle(
    IN HANDLE hWorkEvent,
    OUT PXONLINETASK_HANDLE pHandle
    );

// ====================================================================
// XOnline Content Delivery
//

//
// Content ID type
//
typedef ULONGLONG       XONLINECONTENT_ID;

//
// The list of devices from which to enumerate content
//
typedef enum {
    XONLINECONTENT_ENUM_DEVICE_DVD = 0,
    XONLINECONTENT_ENUM_DEVICE_HD,
    XONLINECONTENT_ENUM_DEVICE_ONLINE,
    XONLINECONTENT_ENUM_DEVICE_MAX
} XONLINECONTENT_ENUM_DEVICE, *PXONLINECONTENT_ENUM_DEVICE;

//
// Data structure representing catalog information about a specific content
// package
//
typedef struct {
    XONLINECONTENT_ID   ContentId;
    DWORD               cbPackageSize;
    DWORD               cbInstallSize;
    FILETIME            ftCreationDate;
    PCHAR               szName;
    DWORD               cbName;
    PCHAR               szDescription;
    DWORD               cbDescription;
    PCHAR               szContentRating;
    DWORD               cbContentRating;
    PVOID               pvTitleSpecificData;
    DWORD               cbTitleSpecificData;
} XONLINECATALOG, *PXONLINECATALOG;

//
// Function to kick off a content enumeration request. All available content 
// on the specified device will be enumerated and returned. This function will
// attempt to get the first dwMaxResults records or the total number of records
// available, whichever is less. This if further constrained by the size of
// the buffer provided/requested. Enumeration will stop when either the desired
// number of records are reached, or when the buffer fills up. Use the
// XOnlineEnumerateContentGetMaxResultsSize to obtain the upper bound buffer
// requirement for a given number of records.
//
XBOXAPI
HRESULT 
WINAPI
XOnlineEnumerateContentBegin(
    IN XONLINECONTENT_ENUM_DEVICE edDevice,
    IN OPTIONAL PBYTE pbBuffer,
    IN DWORD cbBuffer,
    IN DWORD dwMaxResults,
    IN OPTIONAL FILETIME *pftCreatedAfter,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to pick up from a previous enumeration. This kicks off an
// enumeration the get the next dwMaxResults records. The same
// pftCreatedAfter criterion is maintained for continuation enumerations.
//
XBOXAPI
HRESULT
WINAPI
XOnlineEnumerateContentContinue(
    IN XONLINETASK_HANDLE hTask
    );

//
// Function to retrieve the results of a content enumeration.
//
// Note: Callers will NOT have to free prgpCatalog when they are done,
// but they must call XOnlineTaskFree(hTask) AFTER they are done with
// prgcCatalog.
//
// prgpCatalog returns a pointer to a vector of pointers to 
// XONLINECATALOG structures (vs. pointer to an array of structs).
// i.e. ((*prgpCatalog)[i])->wszName is the name of the ith returned
// record.
//
XBOXAPI
HRESULT 
WINAPI
XOnlineEnumerateContentGetResults(
    IN XONLINETASK_HANDLE hTask,
    OUT PXONLINECATALOG **prgpCatalog,
    OUT DWORD *pdwReturnedResults,
    OUT BOOL *pfPartialResults
    );

//
// Utility function to determine the absolute upper bound memory 
// requirements for receiving a specified number of catalog entries
//
XBOXAPI
DWORD 
WINAPI
XOnlineEnumerateContentGetMaxResultsSize(
    IN DWORD dwMaxResults
    );

//
// Function to kick off downloading and installing a
// content package as specified by the content ID.
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineInstallContent(
    IN XONLINECONTENT_ID ContentId,
    IN OPTIONAL HANDLE hWorkEvent,
    OUT XONLINETASK_HANDLE *phTask
    );

//
// Function to get the progress of an active content installation
//
XBOXAPI
HRESULT
WINAPI
XOnlineInstallContentGetProgress(
    IN XONLINETASK_HANDLE hTask,
    OUT OPTIONAL DWORD *pdwPercentDone,
    OUT OPTIONAL ULARGE_INTEGER *puliNumerator,
    OUT OPTIONAL ULARGE_INTEGER *puliDenominator
    );

//
// Function to kick off verification of a content package
//
XBOXAPI
HRESULT 
WINAPI 
XOnlineVerifyContent(
    IN XONLINECONTENT_ID ContentId,
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
XOnlineRemoveContent(
    IN XONLINECONTENT_ID ContentId,
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
XOnlineGetContentRootDirectory(
    IN XONLINECONTENT_ID ContentId,
    IN BOOL fUserData,
    IN OUT DWORD *pcbRootDirectory,
    OUT CHAR *szRootDirectory
    );


// ====================================================================
// XOnline Matchmaking
//


#define XMATCH_S_NO_MORE_RESULTS            S_FALSE
#define XMATCH_ATTRIBUTE_PUBLIC_AVAILABLE   0x81000000
#define XMATCH_ATTRIBUTE_PRIVATE_AVAILABLE  0x81000001
#define X_MAX_STRING_ATTRIBUTE_LEN          400 // maximum length of string attribute
#define X_MAX_BLOB_ATTRIBUTE_LEN            800 // maximum length of blob attribute
#define X_HOST_ADDRESS_LEN              40  // maximum length of host address
#define X_KEY_LEN                       16  // byte length of encryption keys


//
//  Attribute ID fields
//
#define X_ATTRIBUTE_SCOPE_MASK              0x80000000
#define X_ATTRIBUTE_SCOPE_GLOBAL            0x80000000
#define X_ATTRIBUTE_SCOPE_TITLE_SPECIFIC    0x00000000

#define X_ATTRIBUTE_TYPE_MASK               0x0F000000
#define X_ATTRIBUTE_TYPE_USER               0x00000000
#define X_ATTRIBUTE_TYPE_SESSION            0x01000000

#define X_ATTRIBUTE_DATATYPE_MASK           0x00F00000
#define X_ATTRIBUTE_DATATYPE_INTEGER        0x00000000
#define X_ATTRIBUTE_DATATYPE_STRING         0x00100000
#define X_ATTRIBUTE_DATATYPE_BLOB           0x00200000

#define X_ATTRIBUTE_ID_MASK                 0x0000FFFF

#pragma pack(push, 1)

//
//  This structure defines the base session information 
//  returned to the client after a successful search.
//
typedef struct
{
    DWORD               dwResultLength;
    ULONGLONG           qwSessionID;
    BYTE                bHostAddress[X_HOST_ADDRESS_LEN];
    BYTE                bKeyExchangeKey[X_KEY_LEN];
    DWORD               dwPublicAvailable;
    DWORD               dwPrivateAvailable;
    DWORD               dwNumAttributes;
} XMATCH_SEARCHRESULT, *PXMATCH_SEARCHRESULT;

#pragma pack(pop)


//
// Function to open a session creation request
//
HRESULT
WINAPI
XMatchOpenSession(
    IN  ULONGLONG           qwSessionID,
    IN  DWORD               dwNumAttributes,
    IN  DWORD               dwAttributesLen,
    IN  HANDLE              hWorkEvent,
    OUT XONLINETASK_HANDLE* phSession
    );

//
// Function to add an integer attribute to a session creation request
//
HRESULT
WINAPI
XMatchSetSessionInt(
    IN  XONLINETASK_HANDLE  hSession,
    IN  DWORD               dwAttributeID,
    IN  ULONGLONG           qwUserPUID,
    IN  ULONGLONG           qwAttributeValue
    );

//
// Function to add a string attribute to a session creation request
//
HRESULT
WINAPI
XMatchSetSessionString(
    IN  XONLINETASK_HANDLE  hSession,
    IN  DWORD               dwAttributeID,
    IN  ULONGLONG           qwUserPUID,
    IN  LPCWSTR             pwszAttributeValue
    );

//
// Function to add a blob attribute to a session creation request
//
HRESULT
WINAPI
XMatchSetSessionBlob(
    IN  XONLINETASK_HANDLE  hSession,
    IN  DWORD               dwAttributeID,
    IN  ULONGLONG           qwUserPUID,
    IN  DWORD               dwAttributeLength,
    IN  PVOID               pvAttributeValue
    );

//
// Function to add a player to a session creation request.  This
// must be called before any player-specific attributes for a
// particular player are added.
//
HRESULT
WINAPI
XMatchAddPlayer(
    IN  XONLINETASK_HANDLE  hSession,
    IN  ULONGLONG           qwUserPUID
    );
    
//
// Function to remove a player from a session creation request.
// This will remove all player-specific attributes for this
// player.
//
HRESULT
WINAPI
XMatchRemovePlayer(
    IN  XONLINETASK_HANDLE  hSession,
    IN  ULONGLONG           qwUserPUID
    );
    
//
// Function to send the session creation request to the server.
// The hSession task handle must be called repeatedly until
// there is no more work to do.
//
HRESULT
WINAPI
XMatchSendSessionDescription(
    IN  XONLINETASK_HANDLE  hSession
    );

//
// Function to retrieve the session ID that was created in the
// create session call.  It will return zero until the session
// creation has been completed.
//
HRESULT
WINAPI
XMatchGetSessionID(
    IN  XONLINETASK_HANDLE  hSession,
    OUT PULONGLONG          pqwSessionID
    );

//
// Function to create a delete session request.
//
HRESULT
WINAPI
XMatchDeleteSession(
    IN  ULONGLONG           qwSessionID,
    IN  HANDLE              hWorkEvent,
    OUT PXONLINETASK_HANDLE phDelete
    );

//
// Function to search for a particular session ID.
// The hSearch task handle must be called repeatedly until
// there is no more work to do.
//
HRESULT
WINAPI
XMatchFindSessionFromID(
    IN  ULONGLONG           qwSessionID,
    IN  HANDLE              hWorkEvent,
    OUT PXONLINETASK_HANDLE phSearch
    );
    
//
// Function to open a session search request
//
HRESULT
WINAPI
XMatchCreateSearch(
        IN      DWORD                       dwProcedureIndex,
        IN      DWORD                       dwNumParameters,
        IN      DWORD                       dwParametersLen,
        IN      DWORD                       dwResultsLen,
        IN  HANDLE                          hWorkEvent,
        OUT     PXONLINETASK_HANDLE     phSearch
        );

//
// Function to append an integer parameter to a search request
//
HRESULT
WINAPI
XMatchSearchAppendInt(
        IN      XONLINETASK_HANDLE      hSearch,
        IN      ULONGLONG                       qwParameterValue
        );

//
// Function to append a string parameter to a search request
//
HRESULT
WINAPI
XMatchSearchAppendString(
        IN      XONLINETASK_HANDLE      hSearch,
        IN      LPCWSTR                 pwszParameterValue
        );
        
//
// Function to append an blob parameter to a search request
//
HRESULT
WINAPI
XMatchSearchAppendBlob(
        IN      XONLINETASK_HANDLE      hSearch,
        IN      DWORD                       dwParameterLen,
        IN      LPVOID                      pvParameterValue
        );
        
//
// Function to send the search request to the server.
// The hSearch task handle must be called repeatedly until
// there is no more work to do.
//
HRESULT
WINAPI
XMatchSendSearch(
        IN      XONLINETASK_HANDLE      hSearch
        );
        
//
// Function to retrieve the first search result from the
// search request.  It will RIP until the hSearch task
// handle has no more work to do and the search results are
// ready to be retrieved.
//
HRESULT
WINAPI
XMatchGetFirstSearchResult(
        IN      XONLINETASK_HANDLE          hSearch,
        OUT     PXMATCH_SEARCHRESULT*   ppResult
        );

//
// Function to retrieve the next search result from the
// search request.  It will RIP until the hSearch task
// handle has no more work to do and the search results are
// ready to be retrieved.
//
HRESULT
WINAPI
XMatchGetNextSearchResult(
        IN      XONLINETASK_HANDLE          hSearch,
        OUT     PXMATCH_SEARCHRESULT*   ppResult
        );



#ifdef __cplusplus
}
#endif

#endif
