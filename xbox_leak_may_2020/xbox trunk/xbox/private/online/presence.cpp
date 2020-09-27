/*++

Copyright (c) Microsoft Corporation.  All rights reserved

Description:
    Module implementing Xbox Presence APIs

Module Name:

    presence.cpp

--*/

#include "xonp.h"
#include "xonver.h"

// BUGBUG: Need to lock friend list during certain operations

//
// Server configuration information for presence and notification POST requests
// to front doors.
//
typedef struct
{
    DWORD dwService;                  // Service ID number
    CHAR * szUrl;                     // relative URL of ISAPI front door
    CHAR * szContentType;             // Service content type
    DWORD dwContentType;              // Size of the service content type
} XPRESENCE_SERVERS, *PXPRESENCE_SERVERS;

const XPRESENCE_SERVERS g_PresenceServers[2] =
{
    { XONLINE_PRESENCE_SERVICE, "/xpnfront/xpresence.srf",     "Content-Type: xon/1\r\n", 21 },
    { XONLINE_PRESENCE_SERVICE, "/xpnfront/xnotification.srf",  "Content-Type: xon/1\r\n", 21 }
};

enum SERVER_TYPE
{
    serverPresence = 0,
    serverNotification
};

//
// Description tag appended to HELLO message.  Used by operations to track
// version numbers of requests.
//
// BUGBUG: Should consider building this dynamically with version number
//
#define SZ_HELLO_DESCRIPTION "XONM4"

//
// POST response buffer sizes.  Small size is used for simple (like friend request) POSTs.
// Large size is used for complex (like ALIVE and SYNC) POSTs.
//
#define DW_SMALL_RECEIVE_BUFFER 256
#define DW_LARGE_RECEIVE_BUFFER 2048

//
// Timeout for POST requests.  After 30 seconds, requests will timeout with a failure.
//
#define DW_POST_TIMEOUT         30000

//
// Timeout for game invite retrieval from hard drive.
// Number of minutes * Number of seconds per minute * number of 100-nanoseconds per second (10000000)
//
#define DW_GAMEINVITE_TIMEOUT   9000000000
//
// Enumeration of all types of requests.  Helps in Continue() function to determine how to handle
// responses.
//
enum XPRESENCE_REQUESTTYPE
{
    NotificationEnumerate = 0,             // Enumeration of notifications
    NotificationSetUserData,               // Set UserData
    PresenceAlive,                         // Alive request
    FriendsGeneral,                        // General type for all other friend requests
    FriendsSync,                           // Sync request
    FriendsPresenceUpdate,                 // Friends LIST request from Presence queue
    FriendsListUpdate,                     // Friends LIST request from List Change queue
    FriendsGameInviteUpdate,               // Friends LIST request from Game Invite queue
    FriendsGameInviteAnswerUpdate,         // Friends LIST request from Game Invite Answer queue
    FriendsEnumerateNop,                   // Friends enumeration with no notifications currently pending
    LockoutlistGeneral                     // General type for all lockoutlist requests
};

enum XPRESENCE_CACHESTATE
{
    CacheDone = 0,
    CacheOpening,
    CacheWriting,
    CacheClosing,
    CacheDeleting,
    CacheReading
};

struct XPRESENCE_GAMEINVITE_RECORD
{
    BYTE           index;
    ULONGLONG      qwUserID;
    XONLINE_FRIEND InvitingFriend;
    FILETIME       FileTime;
};

//
// Double Linked List of XRL Upload handles and response buffers.  Every presence and friend list task handle
// has one of these lists.  During a work pump, one entry in this handle list is pumped and the pointer is set
// to the next one in the list.  The only task handle that will ever have more than one entry in this handle
// list is the general friend operation handle (from FriendsStartup()).
//
typedef struct _XPRESENCE_HANDLE_LIST
{
    XONLINETASK_HANDLE hTask;
    PBYTE pBuffer;
    _XPRESENCE_HANDLE_LIST *pPrev;
    _XPRESENCE_HANDLE_LIST *pNext;
} XPRESENCE_HANDLE_LIST, *PXPRESENCE_HANDLE_LIST;

//
// Linked list of friend request errors.  Friend errors are returned as notifications.  We store them in
// this linked list until the client can retrieve them with the FriendsGetResults() function.
//
typedef struct _XPRESENCE_FRIENDS_ERROR_LIST
{
    DWORD                         dwUserIndex;
    XUID                          xuid;
    HRESULT                       hr;
    _XPRESENCE_FRIENDS_ERROR_LIST *pNext;
} XPRESENCE_FRIENDS_ERROR_LIST, *PXPRESENCE_FRIENDS_ERROR_LIST;

//
// Extended Task Handle structure for friends and presence services.  It stores extra context about the tasks
// as they pertain to friends and presence.
//
struct XPRESENCE_EXTENDED_HANDLE
{
    XONLINETASK_CONTEXT           XPresenceTaskHandle;  // Generic task handle context
    XPRESENCE_REQUESTTYPE         Type;                 // The type of task handle
    BOOL                          fPresenceIndicated;   // TRUE if XONLINE_S_NOTIFICATION_UPTODATE has been returned, FALSE otherwise
#if DBG
    DWORD                         dwSignature;          // Debug only:  signature used for error checking
#endif
    DWORD                         dwUserIndex;          // Index of user associated with this task
    SGADDR                        sgaddr;               // SGADDR of Xbox
    PXPRESENCE_FRIENDS_ERROR_LIST pFriendsErrorList;    // Errors returned from friend requests
    PXPRESENCE_HANDLE_LIST        pHandleList;          // List of XRL Upload task handles
};

//
// Extended Task Handle structure for caching.  It stores extra context about the tasks
// as it pertains to caching.
//
struct XPRESENCE_CACHEEXTENDED_HANDLE
{
    XONLINETASK_CONTEXT  XPresenceTaskHandle;  // Generic task handle context
    HANDLE               hCache;
    XONLINETASK_HANDLE   hTask;
    XPRESENCE_CACHESTATE cachestate;
    XONLC_CONTEXT        context;
    XPRESENCE_GAMEINVITE_RECORD  indexData;
#if DBG
    DWORD                dwSignature;          // Debug only:  signature used for error checking
#endif
};

//
// Debug signature and validation macro for task handle
//
#define XPRESENCE_SIGNATURE                         'XPRE'
#define IsValidXPresenceHandle(TaskHandle)          (((PXPRESENCE_EXTENDED_HANDLE)TaskHandle)->dwSignature == XPRESENCE_SIGNATURE)
#define IsValidXCachePresenceHandle(TaskHandle)     (((PXPRESENCE_CACHEEXTENDED_HANDLE)TaskHandle)->dwSignature == XPRESENCE_SIGNATURE)

HRESULT
CXo::XOnlineFeedbackSend(
            IN DWORD dwUserIndex,
            IN XUID xTargetUser,
            IN XONLINE_FEEDBACK_TYPE FeedbackType,
            IN LPCWSTR lpszNickname,
            IN HANDLE hWorkEvent,
            OUT PXONLINETASK_HANDLE phTask
            )
{
    return XONLINETASK_S_SUCCESS;
}

//---------------------------------------------------------------------------
//
// XOnlineNotificationSetState
//
// Public API.
//
// Set the state of the user.  Calls into CS client code which will send
// the new state information up to the CS during the next heartbeat.
//
BOOL
CXo::XOnlineNotificationSetState(
    IN DWORD dwUserIndex,
    IN DWORD dwStateFlags,
    IN XNKID sessionID,
    IN DWORD cbStateData,
    IN PBYTE pStateData
    )
{
    XoEnter_("XOnlineNotificationSetState", FALSE);
    XoCheck_((dwUserIndex < XONLINE_MAX_LOGON_USERS), FALSE);
    XoCheck_(IsValidStateFlags(dwStateFlags), FALSE);
    XoCheck_(cbStateData <= MAX_STATEDATA_SIZE && ((cbStateData == 0) == (pStateData == NULL)), FALSE);
    XoCheck_(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags), FALSE);

    //
    // Add number of guest users to state flags
    //
    dwStateFlags &= ~P_STATE_MASK_GUESTS;
    dwStateFlags |= (NumGuests(m_rgLogonUsers[dwUserIndex].xuid.qwUserID) << 5);

    //
    // Set the state.  Currently always returns S_OK
    //
    return(XnLogonSetPState(dwUserIndex, dwStateFlags, &sessionID, cbStateData, pStateData));
}

//---------------------------------------------------------------------------
//
// XOnlineNotificationSetUserData
//
// Public API.
//
// Set the userdata of the user.  This is posted to the presence front door
// directly.
//
HRESULT
CXo::XOnlineNotificationSetUserData(
    IN DWORD dwUserIndex,
    IN DWORD cbUserData,
    IN PBYTE pUserData,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineNotificationSetUserData");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(cbUserData <= MAX_USERDATA_SIZE && ((cbUserData == 0) == (pUserData == NULL)));

    HRESULT                    hr              = S_OK;
    P_NICKNAME_MSG             *pMsg           = NULL;
    PBYTE                      pbMsgBuffer     = NULL;
    UINT                       cbMsgBuffer     = 0;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle = (PXPRESENCE_EXTENDED_HANDLE) phTask;

    //
    // If the ALIVE message has not been sent (ie. XOnlineLogon has not been called yet), then we
    // store the userdata in a global and send it up with the ALIVE message.  Otherwise (the ALIVE
    // has already been sent), we send the userdata directly to the presence front door.
    //
    if (m_fAliveDone == FALSE)
    {
        XoCheck(phTask == NULL);
        XoCheck(hEvent == NULL);

        //
        // If they specified a UserData, create the buffer and copy the userdata
        // into it.
        //
        if (cbUserData != 0)
        {
            m_Users[dwUserIndex].pbUserData = (PBYTE) SysAlloc(cbUserData, PTAG_XONLINE_USERDATA);

            if (m_Users[dwUserIndex].pbUserData == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto Error;
            }

            m_Users[dwUserIndex].cbUserData = (WORD) cbUserData;
            memcpy(m_Users[dwUserIndex].pbUserData, pUserData, cbUserData);
        }
    }
    else // The ALIVE message has already been sent
    {
        XoCheck(phTask != NULL);
        XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

        //
        // Create a task handle to return to the client
        //
        *phTask = NULL;

        hr = CreateContext( &pExtendedHandle, hEvent, NotificationSetUserData, dwUserIndex );

        if (SUCCEEDED(hr))
        {
            //
            // Create and send the UserData message
            //
            hr = CreateMsg(PMSG_NICKNAME, sizeof(P_NICKNAME_MSG) + cbUserData, pExtendedHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

            if (FAILED(hr))
            {
                goto Error;
            }

            pMsg->qwUserID  = m_Users[dwUserIndex].qwUserID;
            pMsg->cbNickname = (WORD) cbUserData;
            memcpy(((PBYTE)pMsg)+sizeof(P_NICKNAME_MSG), pUserData, cbUserData);

            hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, pExtendedHandle, serverPresence);

            *phTask = (XONLINETASK_HANDLE) pExtendedHandle;
        }
    }

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineNotificationIsPending
//
// Public API.
//
// Check if a notification queue has pending notifications.
//
BOOL
CXo::XOnlineNotificationIsPending(
    IN DWORD dwUserIndex,
    IN DWORD dwType
    )
{
    XoEnter_("XOnlineNotificationIsPending", FALSE);
    XoCheck_(dwUserIndex < XONLINE_MAX_LOGON_USERS, FALSE);
    XoCheck_(IsValidNotificationType(dwType), FALSE);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT   hr        = S_OK;
    DWORD     dwNQFlags = 0;
    ULONGLONG qwUser    = 0;
    DWORD     dwSeqNum;

    //
    // Call into client CS code to check if a tickle has been received
    //
    if (!XnLogonGetQFlags(dwUserIndex, &qwUser, &dwNQFlags, &dwSeqNum))
    {
        return(FALSE);
    }
    else
    {
        return (!!(dwType & dwNQFlags));
    }
}

//---------------------------------------------------------------------------
//
// XOnlineNotificationEnumerate
//
// 
//
HRESULT
CXo::XOnlineNotificationEnumerate(
    IN DWORD dwUserIndex,
    IN PXONLINE_NOTIFICATION_MSG pbBuffer,
    IN WORD wBufferCount,
    IN DWORD dwNotificationTypes,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineNotificationEnumerate");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(pbBuffer != NULL);
    XoCheck(wBufferCount != 0);
    XoCheck(IsValidNotificationType(dwNotificationTypes));
    XoCheck(phTask != NULL);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT                    hr              = S_OK;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle = (PXPRESENCE_EXTENDED_HANDLE) phTask;

    *phTask = NULL;

    /*
    hr = CreateContext( &pExtendedHandle, hEvent, NotificationEnumerate, dwUserIndex );
    if (FAILED(hr))
    {
        goto Error;
    }

    hr = XOnlineNotificationEnumeratePriv(dwUserIndex, pbBuffer, wBufferCount, dwNotificationTypes, pExtendedHandle);

    *phTask = (XONLINETASK_HANDLE) pExtendedHandle;

  Error:

    return hr;
    */

    return(XoLeave(E_NOTIMPL));
}


//---------------------------------------------------------------------------
//
// NotificationEnumeratePriv
//
// 
//
HRESULT
CXo::NotificationEnumeratePriv(
    IN DWORD dwUserIndex,
    IN PXONLINE_NOTIFICATION_MSG pbBuffer,
    IN WORD wBufferCount,
    IN DWORD dwNotificationTypes,
    IN PXPRESENCE_EXTENDED_HANDLE pExtendedHandle
    )
{
    HRESULT       hr          = S_OK;
    Q_DEQUEUE_MSG *pMsg       = NULL;
    PBYTE         pbMsgBuffer = NULL;
    UINT          cbMsgBuffer = 0;

    hr = CreateMsg(QMSG_DEQUEUE, sizeof(Q_DEQUEUE_MSG), pExtendedHandle, serverNotification, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

    if (FAILED(hr))
    {
        goto Error;
    }

    switch (dwNotificationTypes)
    {
    case XONLINE_NOTIFICATION_TYPE_FRIENDREQUEST:
        pMsg->wQType = PQUEUE_LIST_CHANGE;
        break;
        
    case XONLINE_NOTIFICATION_TYPE_FRIENDSTATUS:
        pMsg->wQType = PQUEUE_PRESENCE;
        break;

    case XONLINE_NOTIFICATION_TYPE_GAMEINVITE:
        pMsg->wQType = PQUEUE_INVITE;
        break;

    case XONLINE_NOTIFICATION_TYPE_GAMEINVITEANSWER:
        pMsg->wQType = PQUEUE_INVITE_ANSWER;
        break;
    }

    pMsg->qwUserID = m_Users[dwUserIndex].qwUserID;
    pMsg->dwStartItemID = 0;
    pMsg->wMaxItems = wBufferCount;
    pMsg->dwMaxTotalItemDataSize = DW_LARGE_RECEIVE_BUFFER;

//@@@ [drm] This needs to be reworked.  Can't change dwQFlags without dwSeqQFlags 
//@@@    XnLogonSetQFlags(dwUserIndex, 0, dwNotificationTypes);

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_LARGE_RECEIVE_BUFFER, pExtendedHandle, serverNotification);

  Error:

    return hr;
}


//---------------------------------------------------------------------------
//
// NotificationSendAlive
//
// Private API
//
HRESULT
CXo::NotificationSendAlive(
    IN PXONLINE_USER pUsers,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    HRESULT                    hr              = S_OK;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle = (PXPRESENCE_EXTENDED_HANDLE) phTask;
    P_ALIVE_MSG                *pMsg           = NULL;
    PBYTE                      pbMsgBuffer     = NULL;
    UINT                       cbMsgBuffer     = 0;
    DWORD                      dwSeqNum        = 0;
    DWORD                      i               = 0;

    Assert(pUsers != NULL);
    Assert(phTask != NULL);

    *phTask = NULL;

    m_fAliveDone = TRUE;

    hr = CreateContext( &pExtendedHandle, hEvent, PresenceAlive, 0 );

    if (SUCCEEDED(hr))
    {
        for (i = 0; i < XONLINE_MAX_LOGON_USERS; i += 1)
        {
            if (pUsers[i].xuid.qwUserID != 0 && !XOnlineIsUserGuest(pUsers[i].xuid.dwUserFlags))
            {
                m_Users[i].qwUserID = pUsers[i].xuid.qwUserID;
                RtlZeroMemory(m_Users[i].szUserName, XONLINE_USERNAME_SIZE);
                sprintf(m_Users[i].szUserName, "%s%c%s", pUsers[i].name, KINGDOM_SEPERATOR_CHAR, pUsers[i].kingdom);

                hr = CreateMsg(PMSG_ALIVE, sizeof(P_ALIVE_MSG) + strlen(m_Users[i].szUserName) + 1 + m_Users[i].cbUserData, pExtendedHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, &dwSeqNum);

                m_Users[i].dwSeqNum = dwSeqNum;

                if (FAILED(hr))
                {
                    goto Error;
                }

                pMsg->qwUserID = m_Users[i].qwUserID;
                pMsg->sgaddr = pExtendedHandle->sgaddr;
                pMsg->dwTitleID = m_dwTitleId;
                pMsg->cbAcctName = strlen(m_Users[i].szUserName) + 1;
                pMsg->dwBuddyListVersion = 0;
                pMsg->dwBlockListVersion = 0;
                pMsg->dwState = P_STATE_MASK_CLOAKED | P_STATE_MASK_ONLINE;
                pMsg->qwMatchSessionID = 0;
                pMsg->cbNickname = m_Users[i].cbUserData;
                pMsg->cbTitleStuff = 0;
                memcpy(((PBYTE)pMsg)+sizeof(P_ALIVE_MSG), m_Users[i].szUserName, strlen(m_Users[i].szUserName) + 1);
                memcpy(((PBYTE)pMsg)+sizeof(P_ALIVE_MSG)+strlen(m_Users[i].szUserName)+1, m_Users[i].pbUserData, m_Users[i].cbUserData);

                hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_LARGE_RECEIVE_BUFFER, pExtendedHandle, serverPresence);
            }
        }

        *phTask = (XONLINETASK_HANDLE) pExtendedHandle;
    }

  Error:

    return hr;
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsStartup
//
// 
//
HRESULT
CXo::XOnlineFriendsStartup(
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineFriendsStartup");
    XoCheck(phTask != NULL);

    HRESULT hr = S_OK;

    m_precordGameInvite = (PXPRESENCE_GAMEINVITE_RECORD) SysAllocZ(sizeof(XPRESENCE_GAMEINVITE_RECORD), PTAG_XPRESENCE_GAMEINVITE_RECORD);
    if (m_precordGameInvite == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    m_hCacheGameInvite     = NULL;
    m_hTaskGameInvite      = NULL;
    m_cachestateGameInvite = CacheDone;
    m_fGameInvite          = FALSE;

    *phTask = NULL;

    hr = CreateContext( &m_pFriendsHandle, hEvent, FriendsGeneral, 0 );

    if (SUCCEEDED(hr))
    {
        *phTask = (XONLINETASK_HANDLE) m_pFriendsHandle;
    }

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// FriendsGetResults
//
// 
//
HRESULT
CXo::XOnlineFriendsGetResults(
    OUT DWORD *pdwUserIndex,
    OUT XUID *pxuidTargetUser
    )
{
    XoEnter("XOnlineFriendsGetResults");
    XoCheck(pdwUserIndex != NULL);
    XoCheck(pxuidTargetUser != NULL);

    HRESULT                       hr     = S_OK;
    PXPRESENCE_FRIENDS_ERROR_LIST pError = NULL;

    if (m_pFriendsHandle->pFriendsErrorList != NULL)
    {
        hr = m_pFriendsHandle->pFriendsErrorList->hr;
        *pdwUserIndex = m_pFriendsHandle->pFriendsErrorList->dwUserIndex;
        *pxuidTargetUser = m_pFriendsHandle->pFriendsErrorList->xuid;

        pError = m_pFriendsHandle->pFriendsErrorList;
        m_pFriendsHandle->pFriendsErrorList = m_pFriendsHandle->pFriendsErrorList->pNext;
        SysFree(pError);
    }

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsEnumerate
//
// 
//
HRESULT
CXo::XOnlineFriendsEnumerate(
    IN DWORD dwUserIndex,
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineFriendsEnumerate");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(phTask != NULL);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT                    hr              = S_OK;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle = (PXPRESENCE_EXTENDED_HANDLE) phTask;

    *phTask = NULL;

    hr = CreateContext( &pExtendedHandle, hEvent, FriendsSync, dwUserIndex );

    if (SUCCEEDED(hr))
    {
        pExtendedHandle->fPresenceIndicated = FALSE;

        *phTask = (XONLINETASK_HANDLE) pExtendedHandle;
    }

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsGetLatest
//
// 
//
DWORD
CXo::XOnlineFriendsGetLatest(
    IN DWORD dwUserIndex,
    IN DWORD dwFriendBufferCount,
    OUT PXONLINE_FRIEND pFriendBuffer
    )
{
    XoEnter_("XOnlineFriendsGetLatest", 0);
    XoCheck_(dwUserIndex < XONLINE_MAX_LOGON_USERS, 0);
    XoCheck_(dwFriendBufferCount == MAX_FRIENDS, 0);
    XoCheck_(pFriendBuffer != NULL, 0);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    memcpy( pFriendBuffer, m_Users[dwUserIndex].FriendList, dwFriendBufferCount * sizeof(XONLINE_FRIEND));

    return(m_Users[dwUserIndex].cNumFriends);
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsRemove
//
// 
//
HRESULT
CXo::XOnlineFriendsRemove(
    IN DWORD dwUserIndex,
    IN PXONLINE_FRIEND pFriend
    )
{
    XoEnter("XOnlineFriendsRemove");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(pFriend != NULL);
    XoCheck(pFriend->xuid.qwUserID != 0);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT      hr          = S_OK;
    P_DELETE_MSG *pMsg       = NULL;
    PBYTE        pbMsgBuffer = NULL;
    UINT         cbMsgBuffer = 0;

    hr = CreateMsg(PMSG_DELETE, sizeof(P_DELETE_MSG), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

    if (FAILED(hr))
    {
        goto Error;
    }

    pMsg->qwUserID  = m_Users[dwUserIndex].qwUserID;
    pMsg->qwBuddyID = pFriend->xuid.qwUserID;

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pFriendsHandle, serverPresence);

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsRequest
//
// 
//
HRESULT
CXo::XOnlineFriendsRequest(
    IN DWORD dwUserIndex,
    IN XUID xuidToUser
    )
{
    XoEnter("XOnlineFriendsRequest");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(xuidToUser.qwUserID != 0);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT   hr          = S_OK;
    P_ADD_MSG *pMsg       = NULL;
    PBYTE     pbMsgBuffer = NULL;
    UINT      cbMsgBuffer = 0;

    hr = CreateMsg(PMSG_ADD, sizeof(P_ADD_MSG), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

    if (FAILED(hr))
    {
        goto Error;
    }

    pMsg->qwSenderID = m_Users[dwUserIndex].qwUserID;
    pMsg->qwBuddyID = xuidToUser.qwUserID;
    pMsg->cbBuddyAcctname = 0;

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pFriendsHandle, serverPresence);

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsRequestByName
//
// 
//
HRESULT
CXo::XOnlineFriendsRequestByName(
    IN DWORD dwUserIndex,
    IN PSTR pszUserName
    )
{
    XoEnter("XOnlineFriendsRequestByName");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(pszUserName != NULL);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT   hr          = S_OK;
    P_ADD_MSG *pMsg       = NULL;
    PBYTE     pbMsgBuffer = NULL;
    UINT      cbMsgBuffer = 0;

    hr = CreateMsg(PMSG_ADD, sizeof(P_ADD_MSG) + strlen(pszUserName), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

    if (FAILED(hr))
    {
        goto Error;
    }

    pMsg->qwSenderID = m_Users[dwUserIndex].qwUserID;
    pMsg->qwBuddyID = 0;
    pMsg->cbBuddyAcctname = (WORD) strlen(pszUserName);
    memcpy(((PBYTE)pMsg)+sizeof(P_ADD_MSG), pszUserName, strlen(pszUserName));

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pFriendsHandle, serverPresence);

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsGameInvite
//
// 
//
HRESULT
CXo::XOnlineFriendsGameInvite(
    IN DWORD dwUserIndex,
    IN XNKID SessionID,
    IN DWORD dwFriendListCount,
    IN PXONLINE_FRIEND pToFriendList
    )
{
    XoEnter("XOnlineFriendsGameInvite");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(dwFriendListCount > 0);
    XoCheck(pToFriendList != NULL);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT      hr          = S_OK;
    P_INVITE_MSG *pMsg       = NULL;
    PBYTE        pbMsgBuffer = NULL;
    UINT         cbMsgBuffer = 0;
    DWORD        i           = 0;
    ULONGLONG    *pqwUserID  = NULL;

    hr = CreateMsg(PMSG_INVITE, sizeof(P_INVITE_MSG) + (dwFriendListCount * sizeof(ULONGLONG)), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

    if (FAILED(hr))
    {
        goto Error;
    }

    pMsg->qwUserID = m_Users[dwUserIndex].qwUserID;
    pMsg->dwTitleID = m_dwTitleId;
    pMsg->qwMatchSessionID = *((ULONGLONG *)(&SessionID));
    pMsg->cInvitees = (WORD) dwFriendListCount;

    pqwUserID = (ULONGLONG *) (((PBYTE)pMsg)+sizeof(P_INVITE_MSG));

    for (i = 0; i < dwFriendListCount; i += 1)
    {
        pqwUserID[i] = pToFriendList[i].xuid.qwUserID;
    }

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pFriendsHandle, serverPresence);

    if (SUCCEEDED(hr))
    {
        for (i = 0; i < dwFriendListCount; i += 1)
        {
            SetUserFlags(&m_Users[dwUserIndex], pToFriendList[i].xuid.qwUserID, XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED | XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED, FALSE);
            SetUserFlags(&m_Users[dwUserIndex], pToFriendList[i].xuid.qwUserID, XONLINE_FRIENDSTATE_FLAG_SENTINVITE, TRUE);
        }
    }

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsRevokeGameInvite
//
// 
//
HRESULT
CXo::XOnlineFriendsRevokeGameInvite(
    IN DWORD dwUserIndex,
    IN XNKID SessionID,
    IN DWORD dwFriendListCount,
    IN PXONLINE_FRIEND pToFriendList
    )
{
    XoEnter("XOnlineFriendsRevokeGameInvite");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck((dwFriendListCount == 0) == (pToFriendList == NULL));
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT      hr          = S_OK;
    P_CANCEL_MSG *pMsg       = NULL;
    PBYTE        pbMsgBuffer = NULL;
    UINT         cbMsgBuffer = 0;
    DWORD        i           = 0;
    ULONGLONG    *pqwUserID  = NULL;

    hr = CreateMsg(PMSG_CANCEL, sizeof(P_CANCEL_MSG) + (dwFriendListCount * sizeof(ULONGLONG)), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

    if (FAILED(hr))
    {
        goto Error;
    }

    pMsg->qwUserID = m_Users[dwUserIndex].qwUserID;
    pMsg->qwMatchSessionID = *((ULONGLONG *)(&SessionID));
    pMsg->cInvitees = (WORD) dwFriendListCount;

    pqwUserID = (ULONGLONG *) (((PBYTE)pMsg)+sizeof(P_CANCEL_MSG));

    for (i = 0; i < dwFriendListCount; i += 1)
    {
        pqwUserID[i] = pToFriendList[i].xuid.qwUserID;
    }

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pFriendsHandle, serverPresence);

    if (SUCCEEDED(hr))
    {
        for (i = 0; i < dwFriendListCount; i += 1)
        {
            SetUserFlags(&m_Users[dwUserIndex], pToFriendList[i].xuid.qwUserID, XONLINE_FRIENDSTATE_FLAG_SENTINVITE | XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED | XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED, FALSE);
        }
    }

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsAnswerRequest
//
// 
//
HRESULT
CXo::XOnlineFriendsAnswerRequest(
    IN DWORD dwUserIndex,
    IN PXONLINE_FRIEND pToFriend,
    IN XONLINE_REQUEST_ANSWER_TYPE Answer
    )
{
    XoEnter("XOnlineFriendsAnswerRequest");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(pToFriend != NULL);
    XoCheck(Answer == XONLINE_REQUEST_YES || Answer == XONLINE_REQUEST_NO || Answer == XONLINE_REQUEST_BLOCK);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT      hr          = S_OK;
    P_ACCEPT_MSG *pAcceptMsg = NULL;
    P_REJECT_MSG *pRejectMsg = NULL;
    PBYTE        pbMsgBuffer = NULL;
    UINT         cbMsgBuffer = 0;

    switch (Answer)
    {
    case XONLINE_REQUEST_YES:
    {
        hr = CreateMsg(PMSG_ACCEPT, sizeof(P_ACCEPT_MSG), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pAcceptMsg, NULL);

        if (FAILED(hr))
        {
            goto Error;
        }

        pAcceptMsg->qwUserID  = m_Users[dwUserIndex].qwUserID;
        pAcceptMsg->qwBuddyID = pToFriend->xuid.qwUserID;

        break;
    }
    case XONLINE_REQUEST_NO:
    case XONLINE_REQUEST_BLOCK:
    {
        hr = CreateMsg(PMSG_REJECT, sizeof(P_REJECT_MSG), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pRejectMsg, NULL);

        if (FAILED(hr))
        {
            goto Error;
        }

        pRejectMsg->qwUserID  = m_Users[dwUserIndex].qwUserID;
        pRejectMsg->qwBuddyID = pToFriend->xuid.qwUserID;
        pRejectMsg->fNever = FALSE;

        break;
    }
    }
    
    if (Answer == XONLINE_REQUEST_BLOCK)
    {
        pRejectMsg->fNever = TRUE;
    }

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pFriendsHandle, serverPresence);

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsAnswerGameInvite
//
// 
//
HRESULT
CXo::XOnlineFriendsAnswerGameInvite(
    IN DWORD dwUserIndex,
    IN PXONLINE_FRIEND pToFriend,
    IN XONLINE_GAMEINVITE_ANSWER_TYPE Answer
    )
{
    XoEnter("XOnlineFriendsAnswerGameInvite");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(pToFriend != NULL);
    XoCheck(Answer == XONLINE_GAMEINVITE_NO || Answer == XONLINE_GAMEINVITE_YES || Answer == XONLINE_GAMEINVITE_REMOVE);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT             hr          = S_OK;
    P_INVITE_ANSWER_MSG *pMsg       = NULL;
    P_DELETE_MSG        *pMsgDel    = NULL;
    PBYTE               pbMsgBuffer = NULL;
    UINT                cbMsgBuffer = 0;

    if (Answer == XONLINE_GAMEINVITE_REMOVE)
    {
        hr = CreateMsg(PMSG_DELETE, sizeof(P_DELETE_MSG), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsgDel, NULL);

        if (FAILED(hr))
        {
            goto Error;
        }

        pMsgDel->qwUserID  = m_Users[dwUserIndex].qwUserID;
        pMsgDel->qwBuddyID = pToFriend->xuid.qwUserID;

        hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pFriendsHandle, serverPresence);
    }
    else
    {
        hr = CreateMsg(PMSG_INVITE_ANSWER, sizeof(P_INVITE_ANSWER_MSG), m_pFriendsHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

        if (FAILED(hr))
        {
            goto Error;
        }

        pMsg->qwUserID = m_Users[dwUserIndex].qwUserID;
        pMsg->qwHostID = pToFriend->xuid.qwUserID;
        pMsg->qwMatchSessionID = *((ULONGLONG *)&(pToFriend->sessionID));

        if (Answer == XONLINE_GAMEINVITE_NO)
        {
            pMsg->wAnswer = PINVITE_REPLY_NO;
            SetUserFlags(&m_Users[dwUserIndex], pToFriend->xuid.qwUserID, XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE, FALSE);
        }
        else if (Answer == XONLINE_GAMEINVITE_YES)
        {
            pMsg->wAnswer = PINVITE_REPLY_YES;

            if ( pToFriend->titleID != m_dwTitleId )
            {
                m_precordGameInvite->index = 1;
                m_precordGameInvite->qwUserID = m_Users[dwUserIndex].qwUserID;
                memcpy(&(m_precordGameInvite->InvitingFriend), pToFriend, sizeof(XONLINE_FRIEND));
                GetSystemTimeAsFileTime( &(m_precordGameInvite->FileTime));
                m_fGameInvite = TRUE;
            }

            SetUserFlags(&m_Users[dwUserIndex], pToFriend->xuid.qwUserID, XONLINE_FRIENDSTATE_FLAG_SENTINVITE | XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED | XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED | XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE, FALSE);
        }

        hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pFriendsHandle, serverPresence);
    }

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsGetAcceptedGameInvite()
//
HRESULT
CXo::XOnlineFriendsGetAcceptedGameInvite(
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineFriendsGetAcceptedGameInvite");
    XoCheck(phTask != NULL);

    HRESULT                         hr              = S_OK;
    PXPRESENCE_CACHEEXTENDED_HANDLE pExtendedHandle = NULL;

    //
    // Create a task handle to return to the client
    //
    *phTask = NULL;

    pExtendedHandle = (PXPRESENCE_CACHEEXTENDED_HANDLE) SysAllocZ(sizeof(XPRESENCE_CACHEEXTENDED_HANDLE), PTAG_XPRESENCE_EXTENDED_HANDLE);
    if (pExtendedHandle == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    TaskInitializeContext(&(pExtendedHandle->XPresenceTaskHandle));
    pExtendedHandle->XPresenceTaskHandle.pfnContinue = GameInviteContinue;
    pExtendedHandle->XPresenceTaskHandle.pfnClose = GameInviteClose;
    pExtendedHandle->XPresenceTaskHandle.hEventWorkAvailable = hEvent;

#if DBG
    pExtendedHandle->dwSignature = XPRESENCE_SIGNATURE;
#endif

    pExtendedHandle->cachestate = CacheOpening;
    pExtendedHandle->indexData.index = 1;

    hr = CacheOpen(XONLC_TYPE_GAMEINVITE, NULL, hEvent, &(pExtendedHandle->hCache), &(pExtendedHandle->hTask));
    if(FAILED(hr))
    {
        goto Error;
    }

    *phTask = (XONLINETASK_HANDLE) pExtendedHandle;

Cleanup:
    return(XoLeave(hr));

Error:

    if (pExtendedHandle != NULL)
    {
        SysFree(pExtendedHandle);
    }

    goto Cleanup;
}


//---------------------------------------------------------------------------
//
// XOnlineFriendsGetAcceptedGameInviteResult()
//
HRESULT
CXo::XOnlineFriendsGetAcceptedGameInviteResult(
    IN XONLINETASK_HANDLE hTask,
    OUT PXONLINE_ACCEPTED_GAMEINVITE pAcceptedGameInvite
    )
{
    XoEnter("XOnlineFriendsGetAcceptedGameInviteResult");
    XoCheck(pAcceptedGameInvite != NULL);
    XoCheck(IsValidXCachePresenceHandle(hTask));

    HRESULT                         hr              = S_OK;
    PXPRESENCE_CACHEEXTENDED_HANDLE pExtendedHandle = (PXPRESENCE_CACHEEXTENDED_HANDLE) hTask;
    PXPRESENCE_GAMEINVITE_RECORD    pGameInvite = NULL;
    FILETIME                        CurrentTime;

    if (pExtendedHandle->context.bValidDataInCache)
    {
        pGameInvite = (PXPRESENCE_GAMEINVITE_RECORD)(pExtendedHandle->context.pbIndexData);

        GetSystemTimeAsFileTime(&CurrentTime);

        if ((CurrentTime.dwLowDateTime - pGameInvite->FileTime.dwLowDateTime) < DW_GAMEINVITE_TIMEOUT && pGameInvite->InvitingFriend.titleID == m_dwTitleId)
        {
            memcpy( &pAcceptedGameInvite->InvitingFriend, &pGameInvite->InvitingFriend, sizeof(XONLINE_FRIEND));
            pAcceptedGameInvite->xuidAcceptedFriend.qwUserID = pGameInvite->qwUserID;
            memcpy( &pAcceptedGameInvite->InviteAcceptTime, &pGameInvite->FileTime, sizeof(FILETIME));
        }
        else
        {
            hr = S_FALSE;
        }
    }
    else
    {
        hr = S_FALSE;
    }

    return(XoLeave(hr));
}

//---------------------------------------------------------------------------
//
// XOnlineLockoutlistGet
//
// 
//
DWORD
CXo::XOnlineLockoutlistGet(
    IN DWORD dwUserIndex,
    IN DWORD dwLockoutUserBufferCount,
    OUT PXONLINE_LOCKOUTUSER pLockoutUserBuffer
    )
{
    XoEnter_("XOnlineLockoutlistGet", 0);
    XoCheck_(dwUserIndex < XONLINE_MAX_LOGON_USERS, 0);
    XoCheck_(dwLockoutUserBufferCount == MAX_LOCKOUTUSERS, 0);
    XoCheck_(pLockoutUserBuffer != NULL, 0);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    memcpy( pLockoutUserBuffer, m_Users[dwUserIndex].LockoutList, dwLockoutUserBufferCount * sizeof(XONLINE_LOCKOUTUSER));

    return(m_Users[dwUserIndex].cNumLockoutUsers);
}


//---------------------------------------------------------------------------
//
// XOnlineLockoutlistStartup
//
// 
//
HRESULT
CXo::XOnlineLockoutlistStartup(
    IN HANDLE hEvent,
    OUT PXONLINETASK_HANDLE phTask
    )
{
    XoEnter("XOnlineLockoutlistStartup");
    XoCheck(phTask != NULL);

    HRESULT hr = S_OK;

    *phTask = NULL;

    hr = CreateContext( &m_pLockoutlistHandle, hEvent, LockoutlistGeneral, 0 );

    if (SUCCEEDED(hr))
    {
        *phTask = (XONLINETASK_HANDLE) m_pLockoutlistHandle;
    }

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineLockoutlistAdd
//
// 
//
HRESULT
CXo::XOnlineLockoutlistAdd(
    IN DWORD dwUserIndex,
    IN XUID xUserID,
    IN CHAR *pszUsername
    )
{
    XoEnter("XOnlineLockoutlistAdd");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(xUserID.qwUserID != 0);
    XoCheck(pszUsername != NULL);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT     hr          = S_OK;
    P_BLOCK_MSG *pMsg       = NULL;
    PBYTE       pbMsgBuffer = NULL;
    UINT        cbMsgBuffer = 0;

    hr = CreateMsg(PMSG_BLOCK, sizeof(P_BLOCK_MSG), m_pLockoutlistHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

    if (FAILED(hr))
    {
        goto Error;
    }

    pMsg->qwUserID  = m_Users[dwUserIndex].qwUserID;
    pMsg->qwBlockID = xUserID.qwUserID;

    AddLockoutUser(&m_Users[dwUserIndex], xUserID.qwUserID, pszUsername);

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pLockoutlistHandle, serverPresence);

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// XOnlineLockoutlistRemove
//
// 
//
HRESULT
CXo::XOnlineLockoutlistRemove(
    IN DWORD dwUserIndex,
    IN XUID xUserID
    )
{
    XoEnter("XOnlineLockoutlistRemove");
    XoCheck(dwUserIndex < XONLINE_MAX_LOGON_USERS);
    XoCheck(xUserID.qwUserID != 0);
    XoCheck(!XOnlineIsUserGuest(m_rgLogonUsers[dwUserIndex].xuid.dwUserFlags));

    HRESULT       hr          = S_OK;
    P_UNBLOCK_MSG *pMsg       = NULL;
    PBYTE         pbMsgBuffer = NULL;
    UINT        cbMsgBuffer = 0;

    hr = CreateMsg(PMSG_UNBLOCK, sizeof(P_UNBLOCK_MSG), m_pLockoutlistHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, NULL);

    if (FAILED(hr))
    {
        goto Error;
    }

    pMsg->qwUserID  = m_Users[dwUserIndex].qwUserID;
    pMsg->qwBlockID = xUserID.qwUserID;

    RemoveLockoutUser(&m_Users[dwUserIndex], xUserID.qwUserID);

    hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_SMALL_RECEIVE_BUFFER, m_pLockoutlistHandle, serverPresence);

  Error:

    return(XoLeave(hr));
}


//---------------------------------------------------------------------------
//
// CreateContext
//
HRESULT
CXo::CreateContext(
    OUT PXPRESENCE_EXTENDED_HANDLE *ppExtendedHandle,
    IN  HANDLE                  hEvent,
    IN  XPRESENCE_REQUESTTYPE   Type,
    IN  DWORD                   dwUserIndex
    )
{
    HRESULT hr = S_OK;
    
    (*ppExtendedHandle) = (PXPRESENCE_EXTENDED_HANDLE) SysAllocZ(sizeof(XPRESENCE_EXTENDED_HANDLE),
        PTAG_XPRESENCE_EXTENDED_HANDLE);
    if ((*ppExtendedHandle) == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    TaskInitializeContext(&((*ppExtendedHandle)->XPresenceTaskHandle));
    (*ppExtendedHandle)->XPresenceTaskHandle.pfnClose = PresenceClose;
    (*ppExtendedHandle)->XPresenceTaskHandle.hEventWorkAvailable = hEvent;

    switch (Type)
    {
        case NotificationEnumerate:
        case NotificationSetUserData:
        case PresenceAlive:
            (*ppExtendedHandle)->XPresenceTaskHandle.pfnContinue = NotificationContinue;
            break;

        case FriendsGeneral:
        case LockoutlistGeneral:
            (*ppExtendedHandle)->XPresenceTaskHandle.pfnContinue = FriendsContinue;
            break;

        case FriendsSync:
        case FriendsPresenceUpdate:
        case FriendsListUpdate:
        case FriendsGameInviteUpdate:
        case FriendsGameInviteAnswerUpdate:
        case FriendsEnumerateNop:
            (*ppExtendedHandle)->XPresenceTaskHandle.pfnContinue = FriendsEnumerateContinue;
            break;
    }

    XnLogonGetStatus(&(*ppExtendedHandle)->sgaddr);

    (*ppExtendedHandle)->Type              = Type;
    (*ppExtendedHandle)->dwUserIndex       = dwUserIndex;
    (*ppExtendedHandle)->pFriendsErrorList = NULL;
    (*ppExtendedHandle)->pHandleList       = NULL;
#if DBG
    (*ppExtendedHandle)->dwSignature       = XPRESENCE_SIGNATURE;
#endif

Cleanup:
    return hr;

Error:

    if ((*ppExtendedHandle) != NULL)
    {
        SysFree(*ppExtendedHandle);
    }

    goto Cleanup;
}


//---------------------------------------------------------------------------
//
// CreateMsg
//
// 
//
HRESULT
CXo::CreateMsg(
    IN DWORD dwMsgType,
    IN DWORD dwMsgLen,
    IN PXPRESENCE_EXTENDED_HANDLE pExtendedHandle,
    IN SERVER_TYPE Server,
    OUT PBYTE *ppbMsgBuffer,
    OUT UINT  *pcbMsgBuffer,
    OUT PBYTE *ppbMsg,
    OUT DWORD *pdwSeqNum
    )
{
    HRESULT         hr          = S_OK;
    PBYTE           pbMsg       = NULL;
    PBYTE           pbReturnMsg = NULL;
    P_HELLO_MSG     *pPHelloMsg = NULL;
    Q_HELLO_MSG     *pQHelloMsg = NULL;
    BASE_MSG_HEADER *pBaseMsg   = NULL;
    DWORD           dwBufferLen = 0;

    if (Server == serverNotification)
    {
        dwBufferLen = dwMsgLen + sizeof(Q_HELLO_MSG) + strlen(SZ_HELLO_DESCRIPTION) + 1;
    }
    else
    {
        dwBufferLen = dwMsgLen + sizeof(P_HELLO_MSG) + strlen(SZ_HELLO_DESCRIPTION) + 1;
    }

    pbMsg = (PBYTE) SysAlloc(dwBufferLen, PTAG_XPRESENCE_MESSAGE_BUFFER);
    if (pbMsg == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    *ppbMsgBuffer = pbMsg;
    *pcbMsgBuffer = dwBufferLen;

    if (Server == serverNotification)
    {
        pQHelloMsg = (Q_HELLO_MSG *) pbMsg;
        pQHelloMsg->dwMsgType = QMSG_HELLO;
        pQHelloMsg->dwMsgLen = sizeof(Q_HELLO_MSG) - sizeof(BASE_MSG_HEADER) + strlen(SZ_HELLO_DESCRIPTION) + 1;
        pQHelloMsg->dwSeqNum = m_dwSeqNum++;
        pQHelloMsg->sgaddr = pExtendedHandle->sgaddr;
        pQHelloMsg->dwProtocolVersion = CURRENT_XPPROTOCOL_VER;
        pbMsg += sizeof(Q_HELLO_MSG);
    }
    else
    {
        pPHelloMsg = (P_HELLO_MSG *) pbMsg;
        pPHelloMsg->dwMsgType = PMSG_HELLO;
        pPHelloMsg->dwMsgLen = sizeof(P_HELLO_MSG) - sizeof(BASE_MSG_HEADER) + strlen(SZ_HELLO_DESCRIPTION) + 1;
        pPHelloMsg->dwSeqNum = m_dwSeqNum++;
        pPHelloMsg->sgaddr = pExtendedHandle->sgaddr;
        pPHelloMsg->dwProtocolVersion = CURRENT_XPPROTOCOL_VER;
        pbMsg += sizeof(P_HELLO_MSG);
    }

    strcpy((PSTR)pbMsg, SZ_HELLO_DESCRIPTION); pbMsg += strlen(SZ_HELLO_DESCRIPTION) + 1;

    *ppbMsg = pbMsg;

    pBaseMsg = (BASE_MSG_HEADER *) pbMsg;
    pBaseMsg->dwMsgType = dwMsgType;
    pBaseMsg->dwMsgLen = dwMsgLen - sizeof(BASE_MSG_HEADER);
    pBaseMsg->dwSeqNum = m_dwSeqNum++;
    pBaseMsg->sgaddr = pExtendedHandle->sgaddr;

    if (pdwSeqNum != NULL)
    {
        *pdwSeqNum = pBaseMsg->dwSeqNum;
    }

  Error:

    return hr;
}


//---------------------------------------------------------------------------
//
// SendMsg
//
// 
//
HRESULT
CXo::SendMsg(
    IN PBYTE pbMsgBuffer,
    IN UINT cbMsgBuffer,
    IN DWORD dwBufferSize,
    IN PXPRESENCE_EXTENDED_HANDLE pExtendedHandle,
    IN SERVER_TYPE Server
    )
{
    DWORD                  dwSendBufferLen = dwBufferSize;
    HRESULT                hr              = S_OK;
    PXPRESENCE_HANDLE_LIST pNewHandle      = NULL;
    PXPRESENCE_HANDLE_LIST pNextHandle     = NULL;

    pNewHandle = (PXPRESENCE_HANDLE_LIST) SysAlloc(sizeof(XPRESENCE_HANDLE_LIST), PTAG_XPRESENCE_HANDLE_LIST);
    if (pNewHandle == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    pNewHandle->pBuffer = pbMsgBuffer;

    if (pExtendedHandle->pHandleList == NULL)
    {
        pExtendedHandle->pHandleList = pNewHandle;
        pNewHandle->pPrev = pNewHandle;
        pNewHandle->pNext = pNewHandle;
    }
    else
    {
        pNextHandle = pExtendedHandle->pHandleList;
        pExtendedHandle->pHandleList = pNewHandle;
        pNewHandle->pPrev = pNextHandle->pPrev;
        pNewHandle->pNext = pNextHandle;
        pNextHandle->pPrev = pNewHandle;
        pNewHandle->pPrev->pNext = pNewHandle;
    }
 
    hr = XOnlineUploadFromMemory( g_PresenceServers[Server].dwService, g_PresenceServers[Server].szUrl, NULL, 
                                  &dwSendBufferLen, (PBYTE)g_PresenceServers[Server].szContentType, g_PresenceServers[Server].dwContentType,
                                  pbMsgBuffer, cbMsgBuffer,
                                  DW_POST_TIMEOUT,
                                  pExtendedHandle->XPresenceTaskHandle.hEventWorkAvailable, 
                                  &(pExtendedHandle->pHandleList->hTask) );

  Error:

    return hr;
}


//---------------------------------------------------------------------------
//
// NotificationContinue()
//
HRESULT
CXo::NotificationContinue(
    XONLINETASK_HANDLE hTask
    )
{
    HRESULT                    hr               = XONLINETASK_S_RUNNING;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle  = (PXPRESENCE_EXTENDED_HANDLE)hTask;
    PBYTE                      pbUpload         = NULL;
    DWORD                      dwStatus         = 0;
    DWORD                      cbUpload         = 0;
    PXPRESENCE_HANDLE_LIST     pHandle          = NULL;
    ULARGE_INTEGER             uliContentLength;
    ULARGE_INTEGER             uliTotalReceived;

    Assert(hTask != NULL);
    Assert(IsValidXPresenceHandle(hTask));

    if (pExtendedHandle->pHandleList == NULL)
    {
        hr = XONLINETASK_S_SUCCESS;
        goto Error;
    }
    else
    {
        pHandle = pExtendedHandle->pHandleList;
        pExtendedHandle->pHandleList = pExtendedHandle->pHandleList->pNext;

        hr = XOnlineTaskContinue(pHandle->hTask);

        if (hr != XONLINETASK_S_RUNNING)
        {
            if (FAILED(hr))
            {
                pHandle = NULL;
                goto Error;
            }

            if (hr != XONLINETASK_S_SUCCESS)
            {
                // BUGBUG: Should never get here
                pHandle = NULL;
                Assert(FALSE);
                __asm int 3;
                goto Error;
            }

            hr = XOnlineUploadGetResults(pHandle->hTask, &pbUpload, &cbUpload, &uliTotalReceived, &uliContentLength, &dwStatus, NULL);

            SysFree(pHandle->pBuffer);
            pHandle->pBuffer = NULL;

            if (pHandle->pNext == pHandle)
            {
                pExtendedHandle->pHandleList = NULL;
            }
            else
            {
                pHandle->pNext->pPrev = pHandle->pPrev;
                pHandle->pPrev->pNext = pHandle->pNext;
            }

            if (pbUpload == NULL || dwStatus != 200)
            {
                hr = E_FAIL; // BUGBUG: find a better error code
                goto Error;
            }

            if (SUCCEEDED(hr))
            {
                if (pExtendedHandle->Type == PresenceAlive)
                {
                    P_ALIVE_REPLY_MSG *pAliveReplyMsg = (P_ALIVE_REPLY_MSG *) pbUpload;
                    P_REPLY_BUDDY *pFriend = (P_REPLY_BUDDY *) (((PBYTE)pbUpload) + sizeof(P_ALIVE_REPLY_MSG));
                    P_REPLY_BLOCK *pLockoutUser = (P_REPLY_BLOCK *) (((PBYTE)pbUpload) + sizeof(P_ALIVE_REPLY_MSG));
                    DWORD i = 0;

                    if (FAILED(pAliveReplyMsg->hr))
                    {
                        hr = pAliveReplyMsg->hr;
                        goto Error;
                    }
                    else
                    {
                        ResetFriendList(pAliveReplyMsg->dwSeqNum, pAliveReplyMsg->dwBuddyListVersion, pAliveReplyMsg->cBuddiesSent, pFriend);

                        for (i = 0; i < pAliveReplyMsg->cBuddiesSent; i += 1)
                        {
                            pFriend = (P_REPLY_BUDDY *) (((PBYTE)pFriend) + sizeof(P_REPLY_BUDDY) + pFriend->cbAcctName);
                        }

                        ResetLockoutList(pAliveReplyMsg->dwSeqNum, pAliveReplyMsg->dwBlockListVersion, pAliveReplyMsg->cBlocksSent, (P_REPLY_BLOCK *) pFriend);
                    }
                }

                if (pExtendedHandle->pHandleList == NULL)
                {
                    hr = XONLINETASK_S_SUCCESS;
                }
                else
                {
                    hr = XONLINETASK_S_RUNNING;
                }
            }
            else
            {
                goto Error;
            }

            XOnlineTaskClose(pHandle->hTask);
            SysFree(pHandle);
            pHandle = NULL;
        }

        pHandle = NULL;
    }

    pHandle = NULL;

  Error:

    if (pHandle != NULL)
    {
        XOnlineTaskClose(pHandle->hTask);
        SysFree(pHandle);
        pHandle = NULL;
    }

    if (SUCCEEDED(hr) && hr != XONLINETASK_S_SUCCESS)
    {
        hr = XONLINETASK_S_RUNNING;
    }

    return hr;
}


//---------------------------------------------------------------------------
//
// FriendsEnumerateContinue()
//
HRESULT
CXo::FriendsEnumerateContinue(
    XONLINETASK_HANDLE hTask
    )
{
    HRESULT                    hr               = XONLINETASK_S_RUNNING;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle  = (PXPRESENCE_EXTENDED_HANDLE)hTask;
    PBYTE                      pbUpload         = NULL;
    DWORD                      dwStatus         = 0;
    DWORD                      cbUpload         = 0;
    PXPRESENCE_HANDLE_LIST     pHandle          = NULL;
    ULARGE_INTEGER             uliContentLength;
    ULARGE_INTEGER             uliTotalReceived;

    Assert(hTask != NULL);
    Assert(IsValidXPresenceHandle(hTask));

    if (!pExtendedHandle->fPresenceIndicated)
    {
        if (XOnlineNotificationIsPending(pExtendedHandle->dwUserIndex, XONLINE_NOTIFICATION_TYPE_FRIENDREQUEST) != TRUE)
        {
            pExtendedHandle->fPresenceIndicated = TRUE;
            hr = XONLINE_S_NOTIFICATION_UPTODATE;
            goto Error;
        }
    }

    if (pExtendedHandle->pHandleList == NULL)
    {
        hr = FriendListRefresh(pExtendedHandle);
        if (FAILED(hr))
        {
            hr = hr;
            goto Error;
        }
    }
    else
    {
        pHandle = pExtendedHandle->pHandleList;
        pExtendedHandle->pHandleList = pExtendedHandle->pHandleList->pNext;

        hr = XOnlineTaskContinue(pHandle->hTask);

        if (hr != XONLINETASK_S_RUNNING)
        {
            if (FAILED(hr))
            {
                pHandle = NULL;
                goto Error;
            }

            if (hr != XONLINETASK_S_SUCCESS)
            {
                // BUGBUG: Should never get here
                pHandle = NULL;
                Assert(FALSE);
                __asm int 3;
                goto Error;
            }

            hr = XOnlineUploadGetResults(pHandle->hTask, &pbUpload, &cbUpload, &uliTotalReceived, &uliContentLength, &dwStatus, NULL);

            SysFree(pHandle->pBuffer);
            pHandle->pBuffer = NULL;

            if (pHandle->pNext == pHandle)
            {
                pExtendedHandle->pHandleList = NULL;
            }
            else
            {
                pHandle->pNext->pPrev = pHandle->pPrev;
                pHandle->pPrev->pNext = pHandle->pNext;
            }

            if (pbUpload == NULL || dwStatus != 200)
            {
                hr = E_FAIL; // BUGBUG: find a better error code
                goto Error;
            }

            if (SUCCEEDED(hr))
            {
                if (pExtendedHandle->Type == FriendsSync)
                {
                    P_SYNC_REPLY_MSG *pSyncReplyMsg = (P_SYNC_REPLY_MSG *) pbUpload;
                    P_REPLY_BUDDY *pFriend = (P_REPLY_BUDDY *) (((PBYTE)pbUpload) + sizeof(P_SYNC_REPLY_MSG));

                    ResetFriendList(pSyncReplyMsg->dwSeqNum, pSyncReplyMsg->dwBuddyListVersion, pSyncReplyMsg->cBuddiesSent, pFriend);
                }
                else
                {
                    DWORD dwLastItemID = 0;
                    Q_LIST_REPLY_MSG *pQListReply = (Q_LIST_REPLY_MSG *) pbUpload;
                    dwLastItemID = PreprocessNotifications(pQListReply);

                    if (pQListReply->wQLeft > 0)
                    {
                        DWORD dwQType = 0;

                        if (pExtendedHandle->Type == FriendsPresenceUpdate)
                            dwQType = XONLINE_NOTIFICATION_TYPE_FRIENDSTATUS;
                        else if (pExtendedHandle->Type == FriendsListUpdate)
                            dwQType = XONLINE_NOTIFICATION_TYPE_FRIENDREQUEST;
                        else if (pExtendedHandle->Type == FriendsGameInviteUpdate)
                            dwQType = XONLINE_NOTIFICATION_TYPE_GAMEINVITE;
                        else if (pExtendedHandle->Type == FriendsGameInviteAnswerUpdate)
                            dwQType = XONLINE_NOTIFICATION_TYPE_GAMEINVITEANSWER;

//@@@ [drm] This needs to be reworked.  Can't change dwQFlags without dwSeqQFlags 
//@@@                        XnLogonSetQFlags(pExtendedHandle->dwUserIndex, dwQType, 0);
                    }
                }

                hr = FriendListRefresh(pExtendedHandle);
                if (FAILED(hr))
                {
                    goto Error;
                }
            }
            else
            {
                goto Error;
            }

            XOnlineTaskClose(pHandle->hTask);
            SysFree(pHandle);
            pHandle = NULL;
        }
    }

    pHandle = NULL;

  Error:

    if (pHandle != NULL)
    {
        XOnlineTaskClose(pHandle->hTask);
        SysFree(pHandle);
        pHandle = NULL;
    }

    if (SUCCEEDED(hr) && hr != XONLINE_S_NOTIFICATION_UPTODATE)
    {
        hr = XONLINETASK_S_RUNNING;
    }

    return hr;
}


//---------------------------------------------------------------------------
//
// FriendsContinue()
//
HRESULT
CXo::FriendsContinue(
    XONLINETASK_HANDLE hTask
    )
{
    HRESULT                    hr               = XONLINETASK_S_RUNNING;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle  = (PXPRESENCE_EXTENDED_HANDLE)hTask;
    PBYTE                      pbUpload         = NULL;
    DWORD                      dwStatus         = 0;
    DWORD                      cbUpload         = 0;
    PXPRESENCE_HANDLE_LIST     pHandle          = NULL;
    ULARGE_INTEGER             uliContentLength;
    ULARGE_INTEGER             uliTotalReceived;

    Assert(hTask != NULL);
    Assert(IsValidXPresenceHandle(hTask));

    hr = HandleGameInvite(hTask);
    if (FAILED(hr))
    {
        goto Error;
    }

    if (pExtendedHandle->pFriendsErrorList != NULL)
    {
        hr = XONLINE_S_NOTIFICATION_FRIEND_RESULT;
        goto Error;
    }

    if (pExtendedHandle->pHandleList != NULL)
    {
        pHandle = pExtendedHandle->pHandleList;
        pExtendedHandle->pHandleList = pExtendedHandle->pHandleList->pNext;

        hr = XOnlineTaskContinue(pHandle->hTask);

        if (hr != XONLINETASK_S_RUNNING)
        {
            if (FAILED(hr))
            {
                pHandle = NULL;
                goto Error;
            }

            if (hr != XONLINETASK_S_SUCCESS)
            {
                // BUGBUG: Should never get here
                pHandle = NULL;
                Assert(FALSE);
                __asm int 3;
                goto Error;
            }

            hr = XOnlineUploadGetResults(pHandle->hTask, &pbUpload, &cbUpload, &uliTotalReceived, &uliContentLength, &dwStatus, NULL);

            SysFree(pHandle->pBuffer);
            pHandle->pBuffer = NULL;

            if (pHandle->pNext == pHandle)
            {
                pExtendedHandle->pHandleList = NULL;
            }
            else
            {
                pHandle->pNext->pPrev = pHandle->pPrev;
                pHandle->pPrev->pNext = pHandle->pNext;
            }

            if (pbUpload == NULL || dwStatus != 200)
            {
                hr = E_FAIL; // BUGBUG: find a better error code
                goto Error;
            }

            if (FAILED(hr))
            {
                goto Error;
            }

            XOnlineTaskClose(pHandle->hTask);
            SysFree(pHandle);
            pHandle = NULL;
        }
    }

    pHandle = NULL;

  Error:

    if (pHandle != NULL)
    {
        XOnlineTaskClose(pHandle->hTask);
        SysFree(pHandle);
        pHandle = NULL;
    }

    if (SUCCEEDED(hr) && hr != XONLINE_S_NOTIFICATION_FRIEND_RESULT)
    {
        hr = XONLINETASK_S_RUNNING;
    }

    return hr;
}


//---------------------------------------------------------------------------
//
// HandleGameInvite()
//
HRESULT
CXo::HandleGameInvite(
    XONLINETASK_HANDLE hTask
    )
{
    HRESULT                    hr               = XONLINETASK_S_RUNNING;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle  = (PXPRESENCE_EXTENDED_HANDLE)hTask;

    //
    // Pump the cache task handle, if there is one
    //
    if (m_hTaskGameInvite != NULL)
    {
        hr = XOnlineTaskContinue(m_hTaskGameInvite);

        if (FAILED(hr))
        {
            goto Error;
        }
    }

    //
    // Depending on the current state of the cache, either open the cache, write to it
    // or close it.
    //
    switch (m_cachestateGameInvite)
    {
        //
        // If there's nothing going on with the Cache at the moment...
        //
        case CacheDone:
        {
            //
            // If there's an accepted game invitation that hasn't been written yet, start the cache process.
            // Otherwise, break out of this loop -- there's nothing to do.
            //
            if (m_fGameInvite)
            {
                //
                // Change state to CacheOpening and start the open process
                //
                m_cachestateGameInvite = CacheOpening;

                hr = CacheOpen(XONLC_TYPE_GAMEINVITE, NULL, pExtendedHandle->XPresenceTaskHandle.hEventWorkAvailable, &m_hCacheGameInvite, &m_hTaskGameInvite);
                if(FAILED(hr))
                {
                    goto Error;
                }
            }
            else
            {
                //
                // There's nothing to do... break out of the loop
                //
                goto Error;
            }

            break;
        }

        //
        // The cache is currently being opened
        //
        case CacheOpening:
        {
            //
            // If opening the cache is completed...
            //
            if (hr != XONLINETASK_S_RUNNING)
            {
                //
                // Change to the next state: writing to the cache
                //
                m_cachestateGameInvite = CacheWriting;
                m_fGameInvite = FALSE;

                hr = CacheUpdate( m_hCacheGameInvite, m_hTaskGameInvite, (PBYTE)m_precordGameInvite, NULL);
                if(FAILED(hr))
                {
                    goto Error;
                }
            }

            break;
        }

        //
        // The cache is currently being written to
        //
        case CacheWriting:
        {
            //
            // If writing to the cache is completed
            //
            if (hr != XONLINETASK_S_RUNNING)
            {
                //
                // Change to next state: close the cache
                //
                m_cachestateGameInvite = CacheClosing;

                hr = CacheClose( m_hCacheGameInvite, m_hTaskGameInvite );
                if(FAILED(hr))
                {
                    goto Error;
                }
            }

            break;
        }

        //
        // The cache is currently being closed
        //
        case CacheClosing:
        {
            //
            // If closing the cache is complete
            //
            if(hr != XONLINETASK_S_RUNNING)
            {
                //
                // Change to next state: all done -- nothing to do
                //
                m_cachestateGameInvite = CacheDone;

                XOnlineTaskClose(m_hTaskGameInvite);

                m_hCacheGameInvite = NULL;
                m_hTaskGameInvite = NULL;
            }

            break;
        }

    }

  Error:

    return hr;
}


//---------------------------------------------------------------------------
//
// GameInviteContinue()
//
HRESULT
CXo::GameInviteContinue(
    XONLINETASK_HANDLE hTask
    )
{
    HRESULT                         hr               = XONLINETASK_S_RUNNING;
    PXPRESENCE_CACHEEXTENDED_HANDLE pExtendedHandle  = (PXPRESENCE_CACHEEXTENDED_HANDLE) hTask;

    Assert(hTask != NULL);
    Assert(IsValidXCachePresenceHandle(hTask));

    //
    // Pump the cache task handle, if there is one
    //
    if (pExtendedHandle->hTask != NULL)
    {
        hr = XOnlineTaskContinue(pExtendedHandle->hTask);

        if (FAILED(hr))
        {
            goto Error;
        }
    }

    //
    // Depending on the current state of the cache, either open the cache, read from it
    // or close it.
    //
    switch (pExtendedHandle->cachestate)
    {
        //
        // The cache is currently being opened
        //
        case CacheOpening:
        {
            //
            // If opening the cache is completed...
            //
            if (hr != XONLINETASK_S_RUNNING)
            {
                //
                // Lookup & Retrieve
                //
                memset(&(pExtendedHandle->context), 0, sizeof(XONLC_CONTEXT));

                pExtendedHandle->context.pbIndexData = (PBYTE)(&(pExtendedHandle->indexData));
                pExtendedHandle->context.pbRecordBuffer = NULL;
                pExtendedHandle->context.dwRecordBufferSize = 0;

                hr = CacheLookup(pExtendedHandle->hCache, &(pExtendedHandle->context));
                if(FAILED(hr))
                {
                    goto Error;
                }

                //
                // Retrieved game invite successfully.
                //

                //
                // If the game invite is for this title, then delete it from the cache.
                //
                if (((PXPRESENCE_GAMEINVITE_RECORD)(pExtendedHandle->context.pbIndexData))->InvitingFriend.titleID == m_dwTitleId)
                {
                    //
                    // Delete from cache
                    //
                    hr = CacheDelete(pExtendedHandle->hCache, &(pExtendedHandle->context));
                    if(FAILED(hr))
                    {
                        goto Error;
                    }
                }

                //
                // Change to the next state: closing the cache
                //
                pExtendedHandle->cachestate = CacheClosing;

                hr = CacheClose( pExtendedHandle->hCache, pExtendedHandle->hTask );
                if(FAILED(hr))
                {
                    goto Error;
                }
            }

            break;
        }

        //
        // The cache is currently being closed
        //
        case CacheClosing:
        {
            //
            // If closing the cache is complete
            //
            if(hr != XONLINETASK_S_RUNNING)
            {
                //
                // Change to next state: all done -- nothing to do
                //
                pExtendedHandle->cachestate = CacheDone;

                XOnlineTaskClose(pExtendedHandle->hTask);

                pExtendedHandle->hCache = NULL;
                pExtendedHandle->hTask = NULL;

                return XONLINETASK_S_SUCCESS;
            }

            break;
        }

        //
        // If Cache is done, return XONLINETASK_S_SUCCESS
        //
        case CacheDone:
        {
            return XONLINETASK_S_SUCCESS;
        }

    }

  Error:

    if (SUCCEEDED(hr))
    {
        hr = XONLINETASK_S_RUNNING;
    }

    return hr;
}


//---------------------------------------------------------------------------
//
// GameInviteClose()
//
VOID
CXo::GameInviteClose(
    XONLINETASK_HANDLE hTask
    )
{
    HRESULT                    hr               = S_OK;
    PXPRESENCE_CACHEEXTENDED_HANDLE pExtendedHandle  = (PXPRESENCE_CACHEEXTENDED_HANDLE)hTask;

    Assert(hTask != NULL);
    Assert(IsValidXCachePresenceHandle(hTask));

    if (pExtendedHandle->hTask != NULL)
    {
        XOnlineTaskClose(pExtendedHandle->hTask);
    }

    SysFree(pExtendedHandle);
}


//---------------------------------------------------------------------------
//
// PresenceClose()
//
VOID
CXo::PresenceClose(
    XONLINETASK_HANDLE hTask
    )
{
    HRESULT                    hr               = S_OK;
    PXPRESENCE_EXTENDED_HANDLE pExtendedHandle  = (PXPRESENCE_EXTENDED_HANDLE)hTask;
    PXPRESENCE_HANDLE_LIST     pHandle          = NULL;

    Assert(hTask != NULL);
    Assert(IsValidXPresenceHandle(hTask));

    if (m_pFriendsHandle == pExtendedHandle)
    {
        SysFree(m_precordGameInvite);
    }

    if (pExtendedHandle->pHandleList != NULL)
    {
        pExtendedHandle->pHandleList->pPrev->pNext = NULL;

        while (pExtendedHandle->pHandleList != NULL)
        {
            XOnlineTaskClose(pExtendedHandle->pHandleList->hTask);
            pHandle = pExtendedHandle->pHandleList;
            pExtendedHandle->pHandleList = pExtendedHandle->pHandleList->pNext;
            SysFree(pHandle);
        }
    }

    SysFree(pExtendedHandle);
}


//---------------------------------------------------------------------------
//
// GetUserFromID
//
// 
//
PXPRESENCE_USER
CXo::GetUserFromID(
    IN ULONGLONG qwUserID
    )
{
    DWORD i = 0;

    for (i = 0; i < XONLINE_MAX_LOGON_USERS; i += 1)
    {
        if (m_Users[i].qwUserID == qwUserID)
        {
            break;
        }
    }

    return &m_Users[i];
}


//---------------------------------------------------------------------------
//
// GetUserIndexFromID
//
// 
//
DWORD
CXo::GetUserIndexFromID(
    IN ULONGLONG qwUserID
    )
{
    DWORD i = 0;

    for (i = 0; i < XONLINE_MAX_LOGON_USERS; i += 1)
    {
        if (m_Users[i].qwUserID == qwUserID)
        {
            break;
        }
    }

    return i;
}


//---------------------------------------------------------------------------
//
// GetUserFromSeqNum
//
// 
//
PXPRESENCE_USER
CXo::GetUserFromSeqNum(
    IN DWORD dwSeqNum
    )
{
    DWORD i = 0;

    for (i = 0; i < XONLINE_MAX_LOGON_USERS; i += 1)
    {
        if (m_Users[i].dwSeqNum == dwSeqNum)
        {
            break;
        }
    }

    return &m_Users[i];
}


//---------------------------------------------------------------------------
//
// FriendListRefresh
//
// 
//
HRESULT
CXo::FriendListRefresh(
    IN PXPRESENCE_EXTENDED_HANDLE pExtendedHandle
    )
{
    HRESULT    hr          = S_OK;
    P_SYNC_MSG *pMsg       = NULL;
    PBYTE      pbMsgBuffer = NULL;
    UINT       cbMsgBuffer = 0;
    DWORD      dwSeqNum    = 0;

    if (m_Users[pExtendedHandle->dwUserIndex].fFriendListSynced == FALSE)
    {
        pExtendedHandle->Type = FriendsSync;

        hr = CreateMsg(PMSG_SYNC, sizeof(P_SYNC_MSG), pExtendedHandle, serverPresence, &pbMsgBuffer, &cbMsgBuffer, (PBYTE *) &pMsg, &dwSeqNum);

        if (FAILED(hr))
        {
            goto Error;
        }

        m_Users[pExtendedHandle->dwUserIndex].dwSeqNum = dwSeqNum;
        pMsg->qwUserID = m_Users[pExtendedHandle->dwUserIndex].qwUserID;
        pMsg->dwBuddyListVersion = m_Users[pExtendedHandle->dwUserIndex].dwFriendListVersion;
        pMsg->dwBlockListVersion = 0;  // BUGBUG: When we support block lists, be sure to set this to correct block list version

        hr = SendMsg(pbMsgBuffer, cbMsgBuffer, DW_LARGE_RECEIVE_BUFFER, pExtendedHandle, serverPresence);

        if (FAILED(hr))
        {
            goto Error;
        }
    }
    else
    {
        XONLINE_NOTIFICATION_MSG Buffer[20]; // BUGBUG: Arbitrary sized buffer

        if (XOnlineNotificationIsPending(pExtendedHandle->dwUserIndex, XONLINE_NOTIFICATION_TYPE_FRIENDREQUEST) == TRUE)
        {
            pExtendedHandle->Type = FriendsListUpdate;
            hr = NotificationEnumeratePriv(pExtendedHandle->dwUserIndex, Buffer, 20, XONLINE_NOTIFICATION_TYPE_FRIENDREQUEST, pExtendedHandle);
        }
        else if (XOnlineNotificationIsPending(pExtendedHandle->dwUserIndex, XONLINE_NOTIFICATION_TYPE_FRIENDSTATUS) == TRUE)
        {
            pExtendedHandle->Type = FriendsPresenceUpdate;
            hr = NotificationEnumeratePriv(pExtendedHandle->dwUserIndex, Buffer, 20, XONLINE_NOTIFICATION_TYPE_FRIENDSTATUS, pExtendedHandle);
        }
        else if (XOnlineNotificationIsPending(pExtendedHandle->dwUserIndex, XONLINE_NOTIFICATION_TYPE_GAMEINVITE) == TRUE)
        {
            pExtendedHandle->Type = FriendsGameInviteUpdate;
            hr = NotificationEnumeratePriv(pExtendedHandle->dwUserIndex, Buffer, 20, XONLINE_NOTIFICATION_TYPE_GAMEINVITE, pExtendedHandle);
        }
        else if (XOnlineNotificationIsPending(pExtendedHandle->dwUserIndex, XONLINE_NOTIFICATION_TYPE_GAMEINVITEANSWER) == TRUE)
        {
            pExtendedHandle->Type = FriendsGameInviteAnswerUpdate;
            hr = NotificationEnumeratePriv(pExtendedHandle->dwUserIndex, Buffer, 20, XONLINE_NOTIFICATION_TYPE_GAMEINVITEANSWER, pExtendedHandle);
        }
        else
        {
            pExtendedHandle->Type = FriendsEnumerateNop;
            hr = S_OK;
        }

        if (FAILED(hr))
        {
            goto Error;
        }
    }

  Error:

    return hr;
}


//---------------------------------------------------------------------------
//
// ResetFriendList()
//
VOID
CXo::ResetFriendList(
    DWORD dwSeqNum,
    DWORD dwFriendListVersion,
    WORD cFriends,
    P_REPLY_BUDDY *pFriend
    )
{
    WORD            wFriendIndex = 0;
    PXPRESENCE_USER pUser       = NULL;

    pUser = GetUserFromSeqNum(dwSeqNum);

    pUser->fFriendListSynced = TRUE;

    if (pUser->dwFriendListVersion != dwFriendListVersion)
    {
        pUser->dwFriendListVersion = dwFriendListVersion;
        pUser->cNumFriends = cFriends;

        for (wFriendIndex = 0; wFriendIndex < cFriends; wFriendIndex += 1)
        {
            pUser->FriendList[wFriendIndex].xuid.qwUserID = pFriend->qwBuddyID;
            memcpy( pUser->FriendList[wFriendIndex].username, ((PBYTE)pFriend) + sizeof(P_REPLY_BUDDY), pFriend->cbAcctName );

            pUser->FriendList[wFriendIndex].friendState = 0;

            if (pFriend->bStatus & P_BUDDY_STATUS_PENDING)
            {
                pUser->FriendList[wFriendIndex].friendState |= XONLINE_FRIENDSTATE_FLAG_PENDING;
            }
            else if (pFriend->bStatus & P_BUDDY_STATUS_REQUEST)
            {
                pUser->FriendList[wFriendIndex].friendState |= XONLINE_FRIENDSTATE_FLAG_REQUEST;
            }

            pFriend = (P_REPLY_BUDDY *) (((PBYTE)pFriend) + sizeof(P_REPLY_BUDDY) + pFriend->cbAcctName);
        }

        for (; wFriendIndex < MAX_FRIENDS; wFriendIndex += 1)
        {
            pUser->FriendList[wFriendIndex].xuid.qwUserID = 0;
        }
    }
}


//---------------------------------------------------------------------------
//
// ResetLockoutList()
//
VOID
CXo::ResetLockoutList(
    DWORD dwSeqNum,
    DWORD dwLockoutListVersion,
    WORD cLockouts,
    P_REPLY_BLOCK *pLockout
    )
{
    WORD            wLockoutIndex = 0;
    PXPRESENCE_USER pUser       = NULL;

    pUser = GetUserFromSeqNum(dwSeqNum);

    pUser->cNumLockoutUsers = cLockouts;

    for (wLockoutIndex = 0; wLockoutIndex < cLockouts; wLockoutIndex += 1)
    {
        pUser->LockoutList[wLockoutIndex].xuid.qwUserID = pLockout->qwBlockID;
        memcpy( pUser->LockoutList[wLockoutIndex].username, ((PBYTE)pLockout) + sizeof(P_REPLY_BLOCK), pLockout->cbAcctName );

        pLockout = (P_REPLY_BLOCK *) (((PBYTE)pLockout) + sizeof(P_REPLY_BLOCK) + pLockout->cbAcctName);
    }

    for (; wLockoutIndex < MAX_LOCKOUTUSERS; wLockoutIndex += 1)
    {
        pUser->LockoutList[wLockoutIndex].xuid.qwUserID = 0;
    }
}


//---------------------------------------------------------------------------
//
// PreprocessNotifications()
//
DWORD
CXo::PreprocessNotifications(
    Q_LIST_REPLY_MSG *pQListReply
    )
{
    HRESULT                       hr            = S_OK;
    Q_LIST_ITEM                   *pQListItem   = NULL;
    DWORD                         dwItemID      = 0;
    PXPRESENCE_USER               pUser         = NULL;
    WORD                          i             = 0;
    PXPRESENCE_FRIENDS_ERROR_LIST pFriendsError = NULL;

    pUser = GetUserFromID(pQListReply->qwUserID);

    pQListItem = (Q_LIST_ITEM *) (((PBYTE)pQListReply) + sizeof(Q_LIST_REPLY_MSG));

    for (i = 0; i < pQListReply->wNumItems; i += 1)
    {
        switch (pQListReply->wQType)
        {
        case PQUEUE_LIST_CHANGE:
        {
            P_LIST_CHANGE_ITEM *pItem = NULL;
            pItem = (P_LIST_CHANGE_ITEM *) (((PBYTE)pQListItem) + sizeof(Q_LIST_ITEM));

            if (pItem->hr != S_OK)
            {
                //
                // Create a new friend error structure and store the error in the linked list.
                // This error can then be retrieved later while pumping the FriendsStartup() task
                // handle.
                //
                pFriendsError = (PXPRESENCE_FRIENDS_ERROR_LIST) SysAlloc(sizeof(XPRESENCE_FRIENDS_ERROR_LIST), PTAG_XPRESENCE_EXTENDED_HANDLE);
                pFriendsError->pNext = m_pFriendsHandle->pFriendsErrorList;
                m_pFriendsHandle->pFriendsErrorList = pFriendsError;

                pFriendsError->hr          = pItem->hr;
                pFriendsError->xuid        = *((XUID *) &pItem->qwTargetID);
                pFriendsError->dwUserIndex = GetUserIndexFromID(pQListReply->qwUserID);

                //
                // Set the TaskContinue event so this error is picked up on the next TaskContinue
                // call.
                //
                SetEvent(m_pFriendsHandle->XPresenceTaskHandle.hEventWorkAvailable);

                break;
            }

            if ((pItem->dwListVersion - pUser->dwFriendListVersion) > 1)
            {
                pUser->fFriendListSynced = FALSE;
            }

            switch (pItem->wOperationID)
            {
            case PLIST_BUDDY_NOP:
            {
                break;
            }

            case PLIST_BUDDY_ADD:
            {
                UpdateUser(pUser, pItem->qwTargetID, ((PBYTE)pItem)+sizeof(P_LIST_CHANGE_ITEM), pItem->cbTargetAcctName, 0, 0, XONLINE_FRIENDSTATE_FLAG_PENDING, 0, 0, NULL, 0, NULL, pItem->dwListVersion);

                break;
            }
            case PLIST_BUDDY_ADDED:
            {
                UpdateUser(pUser, pItem->qwTargetID, ((PBYTE)pItem)+sizeof(P_LIST_CHANGE_ITEM), pItem->cbTargetAcctName, 0, 0, XONLINE_FRIENDSTATE_FLAG_REQUEST, 0, 0, NULL, 0, NULL, pItem->dwListVersion);

                break;
            }

            case PLIST_BUDDY_ACCEPT:
            {
                UpdateUser(pUser, pItem->qwTargetID, ((PBYTE)pItem)+sizeof(P_LIST_CHANGE_ITEM), pItem->cbTargetAcctName, 0, 0, 0, 0, 0, NULL, 0, NULL, pItem->dwListVersion);

                break;
            }

            case PLIST_BUDDY_REJECT:
            {
                RemoveUser(pUser, pItem->qwTargetID, pItem->dwListVersion);
    
                break;
            }

            case PLIST_BUDDY_DELETE:
            {
                RemoveUser(pUser, pItem->qwTargetID, pItem->dwListVersion);

                break;
            }
            }
            
            break;
        }

        case PQUEUE_PRESENCE:
        {
            PBYTE pTitleStuff = NULL;
            PBYTE pUserData = NULL;
            P_PRESENCE_ITEM *pItem = NULL;
            pItem = (P_PRESENCE_ITEM *) (((PBYTE)pQListItem) + sizeof(Q_LIST_ITEM));
            pUserData = ((PBYTE) pItem) + sizeof(P_PRESENCE_ITEM);
            pTitleStuff = ((PBYTE) pItem) + sizeof(P_PRESENCE_ITEM) + pItem->cbNickname;
            UpdateUser(pUser, pItem->qwBuddyID, NULL, 0, pItem->dwTitleID, pItem->dwState, 0, pItem->qwMatchSessionID, pItem->cbTitleStuff, pTitleStuff, pItem->cbNickname, pUserData, 0);

            break;
        }

        case PQUEUE_INVITE:
        {
            // possible BUGBUG:  Ignoring qwMatchSessionID and dwTitleID for now.  This should already be taken care of by presence updates

            P_INVITATION_ITEM *pItem = NULL;
            pItem = (P_INVITATION_ITEM *) (((PBYTE)pQListItem) + sizeof(Q_LIST_ITEM));
            SetUserFlags(pUser, pItem->qwHostID, XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE, TRUE);

            break;
        }

        case PQUEUE_INVITE_ANSWER:
        {
            P_INVITATION_ANSWER_ITEM *pItem = NULL;
            pItem = (P_INVITATION_ANSWER_ITEM *) (((PBYTE)pQListItem) + sizeof(Q_LIST_ITEM));

            if (pItem->wAnswer == PINVITE_REPLY_NO)
            {
                SetUserFlags(pUser, pItem->qwInviteeID, XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED, TRUE);
            }
            else
            {
                SetUserFlags(pUser, pItem->qwInviteeID, XONLINE_FRIENDSTATE_FLAG_INVITEACCEPTED, TRUE);
            }

            break;
        }

        }

        if (dwItemID < pQListItem->dwItemID)
        {
            dwItemID = pQListItem->dwItemID;
        }

        pQListItem = (Q_LIST_ITEM *) (((PBYTE) pQListItem) + pQListItem->wItemLen + sizeof(Q_LIST_ITEM));
    }

    return dwItemID;
}


//---------------------------------------------------------------------------
//
// UpdateUser()
//
HRESULT
CXo::UpdateUser(
    PXPRESENCE_USER pUser,
    ULONGLONG qwFriendID,
    PBYTE pUserName,
    DWORD cbUserName,
    DWORD dwTitleID,
    DWORD dwState,
    DWORD dwFriendFlags,
    ULONGLONG qwMatchSessionID,
    WORD cbStateData,
    PBYTE pStateData,
    WORD cbUserData,
    PBYTE pUserData,
    DWORD dwVersion
    )
{
    HRESULT hr = S_OK;
    DWORD   i  = 0;

    for (i = 0; i < pUser->cNumFriends; i += 1)
    {
        if (pUser->FriendList[i].xuid.qwUserID == qwFriendID)
        {
            break;
        }
    }

    pUser->FriendList[i].xuid.qwUserID = qwFriendID;
    pUser->FriendList[i].titleID = dwTitleID;
    pUser->FriendList[i].friendState = dwState;
    pUser->FriendList[i].friendState |= dwFriendFlags;
    pUser->FriendList[i].sessionID = *((XNKID *)&qwMatchSessionID);
    pUser->FriendList[i].StateDataSize = (BYTE) cbStateData;
    pUser->FriendList[i].UserDataSize = (BYTE) cbUserData;

    if (cbUserName != 0 && pUserName != NULL)
    {
        memcpy(pUser->FriendList[i].username, pUserName, cbUserName);
    }

    if (cbStateData != 0 && pStateData != NULL)
    {
        memcpy( pUser->FriendList[i].StateData, pStateData, cbStateData );
    }

    if (cbUserData != 0 && pUserData != NULL)
    {
        memcpy( pUser->FriendList[i].UserData, pUserData, cbUserData );
    }

    if (dwVersion != 0)
    {
        pUser->dwFriendListVersion = dwVersion;
    }

    if (i == pUser->cNumFriends)
    {
        pUser->cNumFriends = (WORD) (i + 1);
    }

    return hr;
}


//---------------------------------------------------------------------------
//
// SetUserFlags()
//
VOID
CXo::SetUserFlags(
    PXPRESENCE_USER pUser,
    ULONGLONG qwFriendID,
    DWORD dwFlags,
    BOOL fSet
    )
{
    DWORD   i  = 0;

    for (i = 0; i < pUser->cNumFriends; i += 1)
    {
        if (pUser->FriendList[i].xuid.qwUserID == qwFriendID)
        {
            if (fSet)
            {
                pUser->FriendList[i].friendState |= dwFlags;
            }                                           
            else
            {
                pUser->FriendList[i].friendState &= ~dwFlags;
            }
            break;
        }
    }
}


//---------------------------------------------------------------------------
//
// RemoveUser()
//
HRESULT
CXo::RemoveUser(
    IN PXPRESENCE_USER pUser,
    IN ULONGLONG qwFriendID,
    DWORD dwVersion
    )
{
    DWORD i = 0;
    BOOL fMatch = FALSE;

    for (i = 0; i < pUser->cNumFriends; i += 1)
    {
        if (pUser->FriendList[i].xuid.qwUserID == qwFriendID)
        {
            i += 1;
            fMatch = TRUE;
            break;
        }
    }

    for (; i < pUser->cNumFriends; i += 1)
    {
        pUser->FriendList[i-1].xuid.qwUserID = pUser->FriendList[i].xuid.qwUserID;
        memcpy(pUser->FriendList[i-1].username, pUser->FriendList[i].username, XONLINE_USERNAME_SIZE);
        pUser->FriendList[i-1].friendState = pUser->FriendList[i].friendState;
        pUser->FriendList[i-1].titleID = pUser->FriendList[i].titleID;
        pUser->FriendList[i-1].StateDataSize = pUser->FriendList[i].StateDataSize;
        pUser->FriendList[i-1].UserDataSize = pUser->FriendList[i].UserDataSize;
        pUser->FriendList[i-1].sessionID = pUser->FriendList[i].sessionID;
        memcpy(pUser->FriendList[i-1].StateData, pUser->FriendList[i].StateData, pUser->FriendList[i].StateDataSize);
        memcpy(pUser->FriendList[i-1].UserData, pUser->FriendList[i].UserData, pUser->FriendList[i].UserDataSize);
    }

    if (dwVersion != 0)
    {
        pUser->dwFriendListVersion = dwVersion;
    }

    if (fMatch == TRUE)
    {
        pUser->cNumFriends -= 1;
    }

    return S_OK;
}


//---------------------------------------------------------------------------
//
// AddLockoutUser()
//
HRESULT
CXo::AddLockoutUser(
    PXPRESENCE_USER pUser,
    ULONGLONG qwLockoutID,
    LPSTR pszUserName
    )
{
    HRESULT hr = S_OK;
    DWORD   i  = 0;

    for (i = 0; i < pUser->cNumLockoutUsers; i += 1)
    {
        if (pUser->LockoutList[i].xuid.qwUserID == qwLockoutID)
        {
            break;
        }
    }

    pUser->LockoutList[i].xuid.qwUserID = qwLockoutID;

    if (pszUserName != NULL)
    {
        memcpy(pUser->LockoutList[i].username, pszUserName, strlen(pszUserName)+1);
    }

    if (i == pUser->cNumLockoutUsers)
    {
        pUser->cNumLockoutUsers = (WORD) (i + 1);
    }

    return hr;
}


//---------------------------------------------------------------------------
//
// RemoveLockoutUser()
//
HRESULT
CXo::RemoveLockoutUser(
    IN PXPRESENCE_USER pUser,
    IN ULONGLONG qwLockoutID
    )
{
    DWORD i = 0;
    BOOL fMatch = FALSE;

    for (i = 0; i < pUser->cNumLockoutUsers; i += 1)
    {
        if (pUser->LockoutList[i].xuid.qwUserID == qwLockoutID)
        {
            i += 1;
            fMatch = TRUE;
            break;
        }
    }

    for (; i < pUser->cNumLockoutUsers; i += 1)
    {
        pUser->LockoutList[i-1].xuid.qwUserID = pUser->LockoutList[i].xuid.qwUserID;
        memcpy(pUser->LockoutList[i-1].username, pUser->LockoutList[i].username, XONLINE_USERNAME_SIZE);
    }

    if (fMatch == TRUE)
    {
        pUser->cNumLockoutUsers -= 1;
    }

    return S_OK;
}


//---------------------------------------------------------------------------
//
// IsValidStateFlags()
//
BOOL
CXo::IsValidStateFlags(
    DWORD dwStateFlags
    )
{
    if (dwStateFlags == (dwStateFlags & (XONLINE_FRIENDSTATE_FLAG_ONLINE | XONLINE_FRIENDSTATE_FLAG_PLAYING | XONLINE_FRIENDSTATE_FLAG_CLOAKED | XONLINE_FRIENDSTATE_FLAG_VOICE | XONLINE_FRIENDSTATE_FLAG_JOINABLE)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


//---------------------------------------------------------------------------
//
// IsValidNotificationType()
//
BOOL
CXo::IsValidNotificationType(DWORD dwType)
{
    if (dwType == XONLINE_NOTIFICATION_TYPE_ALL || (dwType == (dwType & (XONLINE_NOTIFICATION_TYPE_FRIENDREQUEST | XONLINE_NOTIFICATION_TYPE_FRIENDSTATUS | XONLINE_NOTIFICATION_TYPE_GAMEINVITE | XONLINE_NOTIFICATION_TYPE_GAMEINVITEANSWER))))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


//---------------------------------------------------------------------------
//
// NumGuests()
//
WORD
CXo::NumGuests(ULONGLONG qwUserID)
{
    DWORD i          = 0;
    WORD  wNumGuests = 0;

    for (i = 0; i < XONLINE_MAX_LOGON_USERS; i += 1)
    {
        if (m_rgLogonUsers[i].xuid.qwUserID == qwUserID && XOnlineIsUserGuest(m_rgLogonUsers[i].xuid.dwUserFlags))
        {
            wNumGuests += 1;
        }
    }

    return wNumGuests;
}
