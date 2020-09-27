/*==========================================================================;
 *
 *  Copyright (C) 1999 - 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dpvoice.h
 *  Content:    DirectPlayVoice include file
//@@BEGIN_MSINTERNAL
 *  History:
 *  Date        By          Reason
 *  =====       =======     ==================================================
 *  07/01/99    rodtoll     created
 *  06/23/2000  rodtoll     Bug #37556 - Hexify the DPVERR codes
 //@@END_MSINTERNAL
 ***************************************************************************/

#ifndef __DVOICE__
#define __DVOICE__


/****************************************************************************
 *
 * DirectPlayVoice Callback Functions
 *
 ****************************************************************************/
typedef HRESULT (FAR PASCAL *PDVMESSAGEHANDLER)(
    PVOID   pvUserContext,
    DWORD   dwMessageType,
    LPVOID  lpMessage
);

typedef PDVMESSAGEHANDLER LPDVMESSAGEHANDLER;

/****************************************************************************
 *
 * DirectPlayVoice Datatypes (Non-Structure / Non-Message)
 *
 ****************************************************************************/

typedef DWORD DVID, *LPDVID, *PDVID;

/****************************************************************************
 *
 * DirectPlayVoice Message Types
 *
 ****************************************************************************/

#define DVMSGID_BASE                        0x0000

#define DVMSGID_MINBASE                     (DVMSGID_CREATEVOICEPLAYER)
#define DVMSGID_CREATEVOICEPLAYER           (DVMSGID_BASE+0x0001)
#define DVMSGID_DELETEVOICEPLAYER           (DVMSGID_BASE+0x0002)
#define DVMSGID_SESSIONLOST                 (DVMSGID_BASE+0x0003)
#define DVMSGID_PLAYERVOICESTART            (DVMSGID_BASE+0x0004)
#define DVMSGID_PLAYERVOICESTOP             (DVMSGID_BASE+0x0005)
#define DVMSGID_RECORDSTART                 (DVMSGID_BASE+0x0006)
#define DVMSGID_RECORDSTOP                  (DVMSGID_BASE+0x0007)
#define DVMSGID_CONNECTRESULT               (DVMSGID_BASE+0x0008)
#define DVMSGID_DISCONNECTRESULT            (DVMSGID_BASE+0x0009)
#define DVMSGID_INPUTLEVEL                  (DVMSGID_BASE+0x000A)
#define DVMSGID_VOICEPERIPHERALNOTPRESENT   (DVMSGID_BASE+0x000B)
#define DVMSGID_HOSTMIGRATED                (DVMSGID_BASE+0x000C)
#define DVMSGID_SETTARGETS                  (DVMSGID_BASE+0x000D)
#define DVMSGID_PLAYEROUTPUTLEVEL           (DVMSGID_BASE+0x000E)
#define DVMSGID_LOCALHOSTSETUP              (DVMSGID_BASE+0x0012)
#define DVMSGID_MAXBASE                     (DVMSGID_LOCALHOSTSETUP)

/****************************************************************************
 *
 * DirectPlayVoice Constants
 *
 ****************************************************************************/

#define DVID_SYS                0

//
// Used to identify the session host in client/server
//
#define DVID_SERVERPLAYER       1

//
// Used to target all players
//
#define DVID_ALLPLAYERS         0

//
// Used to identify the main buffer
//
#define DVID_REMAINING          0xFFFFFFFF

//
// Input level range
//
#define DVINPUTLEVEL_MIN                    0x00000000
#define DVINPUTLEVEL_MAX                    0x00000063  // 99 decimal

#define DVNOTIFYPERIOD_MINPERIOD            20

//
// Use the default value
//
#define DVTHRESHOLD_DEFAULT               0xFFFFFFFF

//
// Sensitivity Ranges
//
#define DVTHRESHOLD_MIN                   0x00000000
#define DVTHRESHOLD_MAX                   0x00000063    // 99 decimal

//
// Sensitivity field is not used
//
#define DVTHRESHOLD_UNUSED                0xFFFFFFFE

//
// Session Types
//
#define DVSESSIONTYPE_PEER                  0x00000001
#define DVSESSIONTYPE_MIXING                0x00000002
#define DVSESSIONTYPE_FORWARDING            0x00000003
#define DVSESSIONTYPE_ECHO                  0x00000004


/****************************************************************************
 *
 * DirectPlayVoice Flags
 *
 ****************************************************************************/

//
// Mute the recording
//
#define DVCLIENTCONFIG_RECORDMUTE           0x00000001

//
// Mute the playback
//
#define DVCLIENTCONFIG_PLAYBACKMUTE         0x00000002

//
// Voice Activation manual mode
//
#define DVCLIENTCONFIG_MANUALVOICEACTIVATED 0x00000004

//
// Only playback voices that have buffers created for them
//
#define DVCLIENTCONFIG_MUTEGLOBAL           0x00000010

//
// Enable automatic voice activation
//
#define DVCLIENTCONFIG_AUTOVOICEACTIVATED   0x00000020

//
// disables recording from the local player
// will also be set automatically if the voice peripheral gets removed
//

#define DVCLIENTCONFIG_HALFDUPLEX           0x00000040

//
// determines the behavior of DirectPlay Voice in the case where
// there is no voice peripheral attached to dwPort. If this flag is set,
// all voices that would have been sent to the voice peripheral for 
// playback will not be played at all. If this flag is not set, 
// all voices that would have been sent to the voice peripheral 
// for playback will be sent to the main audio output device (MCPX).
//

#define DVCLIENTCONFIG_MUTEIFNODEVICE       0x00000080

//
// API flags dwFlags field in Connect, etc
//

//
// Shutdown the voice session without migrating the host
//
#define DVFLAGS_NOHOSTMIGRATE               0x00000008

//
// Disable host migration in the voice session
//
#define DVSESSION_NOHOSTMIGRATION           0x00000001

//
// Server controlled targetting
//
#define DVSESSION_SERVERCONTROLTARGET       0x00000002

//
// Player is in half duplex mode
//
#define DVPLAYERCAPS_HALFDUPLEX             0x00000001

//
// Specifies that player is the local player
//
#define DVPLAYERCAPS_LOCAL                  0x00000002


/****************************************************************************
 *
 * DirectPlayVoice Interface Pointer definitions
 *
 ****************************************************************************/

typedef struct IDirectPlayVoiceClient IDirectPlayVoiceClient;
typedef IDirectPlayVoiceClient *LPDIRECTPLAYVOICECLIENT, *PDIRECTPLAYVOICECLIENT;

typedef struct IDirectPlayVoiceServer IDirectPlayVoiceServer;
typedef IDirectPlayVoiceServer *LPDIRECTPLAYVOICESERVER, *PDIRECTPLAYVOICESERVER;

//@@BEGIN_MSINTERNAL

typedef struct IDirectPlayVoiceNotify IDirectPlayVoiceNotify;
typedef IDirectPlayVoiceNotify *LPDIRECTPLAYVOICENOTIFY, *PDIRECTPLAYVOICENOTIFY;

typedef struct IDirectPlayVoiceTransport IDirectPlayVoiceTransport;
typedef IDirectPlayVoiceTransport *LPDIRECTPLAYVOICETRANSPORT, *PDIRECTPLAYVOICETRANSPORT;

/****************************************************************************
 *
 * DirectPlayVoice Structures (Non-Message)
 *
 ****************************************************************************/

// DVTRANSPORTINFO
//
typedef struct
{
    // = sizeof( DVTRANSPORTINFO )
    DWORD           dwSize;
    // Combination of following flags:
    // DVTRANSPORT_MIGRATEHOST, DVTRANSPORT_MULTICAST
    // DVTRANSPORT_LOCALHOST
    DWORD           dwFlags;
    // Session Type the transport is running.  One of:
    // DVTRANSPORT_SESSION_PEERTOPEER
    // DVTRANSPORT_SESSION_CLIENTSERVER
    DWORD           dwSessionType;
    // ID of the player (Regular ID, not system) that
    // is the sesion host.
    DVID            dvidSessionHost;
    // ID of the local player (Regular ID, not system)
    DVID            dvidLocalID;
    // Maximum # of players allowed in the session,
    // 0 = unlimited.
    DWORD           dwMaxPlayers;
} DVTRANSPORTINFO, *LPDVTRANSPORTINFO, *PDVTRANSPORTINFO;


//
// Tunable definitions for deterministic memory usage
//

#define DV_MAX_REMOTE_PLAYERS   32
#define DV_MAX_QUEUED_SPEECH_FRAMES 32

//@@END_MSINTERNAL

//
// DirectPlayVoice Client Configuration
// (Connect / GetClientConfig)
//
typedef struct
{
    DWORD   dwSize;                 // Size of this structure
    DWORD   dwFlags;                // Flags for client config (DVCLIENTCONFIG_...)
    DWORD   dwThreshold;            // Voice Activation Threshold 
    DWORD   dwPort;                 // Player Number(slot # on the xbox) to use for this dvoice instance
} DVCLIENTCONFIG, *LPDVCLIENTCONFIG, *PDVCLIENTCONFIG;

//
// DirectPlayVoice Session Description
// (Host / GetSessionDesc)
//
typedef struct
{
    DWORD   dwSize;                 // Size of this structure
    DWORD   dwFlags;                // Session flags (DVSESSION_...)
    DWORD   dwSessionType;          // Session type (DVSESSIONTYPE_...)
    GUID    guidCT;                 // Compression Type to use
//@@BEGIN_MSINTERNAL
    DWORD   dwBufferQuality;        // Buffer quality
    DWORD   dwBufferAggressiveness; // Buffer aggresiveness
//@@END_MSINTERNAL

} DVSESSIONDESC, *LPDVSESSIONDESC, *PDVSESSIONDESC;

/****************************************************************************
 *
 * DirectPlayVoice message handler call back structures
 *
 ****************************************************************************/

//
// Result of the Connect() call.  (If it wasn't called Async)
// (DVMSGID_CONNECTRESULT)
//

typedef struct
{
    DWORD   dwSize;                         // Size of this structure
} DVMSG_VOICEPERIPHERALNOTPRESENT, *LPDVMSG_VOICEPERIPHERALNOTPRESENT, *PDVMSG_VOICEPERIPHERALNOTPRESENT;

//
// Result of the Connect() call.  (If it wasn't called Async)
// (DVMSGID_CONNECTRESULT)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    HRESULT hrResult;                       // Result of the Connect() call
} DVMSG_CONNECTRESULT, *LPDVMSG_CONNECTRESULT, *PDVMSG_CONNECTRESULT;

//
// A new player has entered the voice session
// (DVMSGID_CREATEVOICEPLAYER)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DVID    dvidPlayer;                     // DVID of the player who joined
    DWORD   dwFlags;                        // Player flags (DVPLAYERCAPS_...)
    PVOID   pvPlayerContext;                // Context value for this player (user set)
} DVMSG_CREATEVOICEPLAYER, *LPDVMSG_CREATEVOICEPLAYER, *PDVMSG_CREATEVOICEPLAYER;

//
// A player has left the voice session
// (DVMSGID_DELETEVOICEPLAYER)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DVID    dvidPlayer;                     // DVID of the player who left
    PVOID   pvPlayerContext;                // Context value for the player
} DVMSG_DELETEVOICEPLAYER, *LPDVMSG_DELETEVOICEPLAYER, *PDVMSG_DELETEVOICEPLAYER;

//
// Result of the Disconnect() call.  (If it wasn't called Async)
// (DVMSGID_DISCONNECTRESULT)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    HRESULT hrResult;                       // Result of the Disconnect() call
} DVMSG_DISCONNECTRESULT, *LPDVMSG_DISCONNECTRESULT, *PDVMSG_DISCONNECTRESULT;

//
// The voice session host has migrated.
// (DVMSGID_HOSTMIGRATED)
//
typedef struct
{
    DWORD                   dwSize;         // Size of this structure
    DVID                    dvidNewHostID;  // DVID of the player who is now the host
    LPDIRECTPLAYVOICESERVER pdvServerInterface;
                                            // Pointer to the new host object (if local player is now host)
} DVMSG_HOSTMIGRATED, *LPDVMSG_HOSTMIGRATED, *PDVMSG_HOSTMIGRATED;

//
// The current input level / recording volume on the local machine
// (DVMSGID_INPUTLEVEL)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DWORD   dwPeakLevel;                    // Current peak level of the audio
    LONG    lRecordVolume;                  // Current recording volume
    PVOID   pvLocalPlayerContext;           // Context value for the local player
} DVMSG_INPUTLEVEL, *LPDVMSG_INPUTLEVEL, *PDVMSG_INPUTLEVEL;

//
// The local client is about to become the new host
// (DVMSGID_LOCALHOSTSETUP)
//
typedef struct
{
    DWORD               dwSize;             // Size of this structure
    PVOID               pvContext;          // Context value to be passed to Initialize() of new host object
    PDVMESSAGEHANDLER   pMessageHandler;    // Message handler to be used by new host object
} DVMSG_LOCALHOSTSETUP, *LPDVMSG_LOCALHOSTSETUP, *PDVMSG_LOCALHOSTSETUP;

//
// The current peak level of an individual player's incoming audio stream as it is
// being played back.
// (DVMSGID_PLAYEROUTPUTLEVEL)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DVID    dvidSourcePlayerID;                   // DVID of the player
    DWORD   dwPeakLevel;                    // Peak level of the player's stream
    PVOID   pvPlayerContext;                // Context value for the player
} DVMSG_PLAYEROUTPUTLEVEL, *LPDVMSG_PLAYEROUTPUTLEVEL, *PDVMSG_PLAYEROUTPUTLEVEL;

//
// An audio stream from the specified player has started playing back on the local client.
// (DVMSGID_PLAYERVOICESTART).
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DVID    dvidSourcePlayerID;             // DVID of the Player
    PVOID   pvPlayerContext;                // Context value for this player
} DVMSG_PLAYERVOICESTART, *LPDVMSG_PLAYERVOICESTART, *PDVMSG_PLAYERVOICESTART;

//
// The audio stream from the specified player has stopped playing back on the local client.
// (DVMSGID_PLAYERVOICESTOP)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DVID    dvidSourcePlayerID;             // DVID of the player
    PVOID   pvPlayerContext;                // Context value for this player
} DVMSG_PLAYERVOICESTOP, *LPDVMSG_PLAYERVOICESTOP, *PDVMSG_PLAYERVOICESTOP;

//
// Transmission has started on the local machine
// (DVMSGID_RECORDSTART)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DWORD   dwPeakLevel;                    // Peak level that caused transmission to start
    PVOID   pvLocalPlayerContext;           // Context value for the local player
} DVMSG_RECORDSTART, *LPDVMSG_RECORDSTART, *PDVMSG_RECORDSTART;

//
// Transmission has stopped on the local machine
// (DVMSGID_RECORDSTOP)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DWORD   dwPeakLevel;                    // Peak level that caused transmission to stop
    PVOID   pvLocalPlayerContext;           // Context value for the local player
} DVMSG_RECORDSTOP, *LPDVMSG_RECORDSTOP, *PDVMSG_RECORDSTOP;

//
// The voice session has been lost
// (DVMSGID_SESSIONLOST)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    HRESULT hrResult;                       // Reason the session was disconnected
} DVMSG_SESSIONLOST, *LPDVMSG_SESSIONLOST, *PDVMSG_SESSIONLOST;

//
// The target list has been updated for the local client
// (DVMSGID_SETTARGETS)
//
typedef struct
{
    DWORD   dwSize;                         // Size of this structure
    DWORD   dwNumTargets;                   // # of targets
    PDVID   pdvidTargets;                   // An array of DVIDs specifying the current targets
} DVMSG_SETTARGETS, *LPDVMSG_SETTARGETS, *PDVMSG_SETTARGETS;

//@@BEGIN_MSINTERNAL

/****************************************************************************
 *
 * DirectPlayVoice Transport Defines / Types / Constants -- MS INTERNAL
 *
 ****************************************************************************/

// Transport session is peer-to-peer
#define DVTRANSPORT_SESSION_PEERTOPEER      0x00000001

// Transport session is client/server
#define DVTRANSPORT_SESSION_CLIENTSERVER    0x00000002

// Host can migrate
#define DVTRANSPORT_MIGRATEHOST             0x00000001

// Multicast optimizations are enabled
#define DVTRANSPORT_MULTICAST               0x00000002

// The local player is the host of the session
#define DVTRANSPORT_LOCALHOST               0x00000004

// Send the message guaranteed
#define DVTRANSPORT_SEND_GUARANTEED         0x00000001
#define DVTRANSPORT_SEND_SYNC               0x00000002

#define DVPEFLAGS_FIRSTPLAYER               0x00000001

#define DVTRANSPORT_OBJECTTYPE_SERVER       0x00000001
#define DVTRANSPORT_OBJECTTYPE_CLIENT       0x00000002
#define DVTRANSPORT_OBJECTTYPE_BOTH         (DVTRANSPORT_OBJECTTYPE_SERVER | DVTRANSPORT_OBJECTTYPE_CLIENT)

typedef struct _DVTRANSPORT_BUFFERDESC
{
    DWORD   dwBufferSize;
    PBYTE   pBufferData;
    LONG    lRefCount;
    PVOID   pvContext;
    DWORD   dwObjectType;
    DWORD   dwFlags;
} DVTRANSPORT_BUFFERDESC, *PDVTRANSPORT_BUFFERDESC;

/*
 * DIRECTVOICENOTIFY DEFINES
 *
 * Used to identify the type of notification in calls
 * to IDirectPlayVoiceNotify::NotifyEvent
 */

// No longer used
#define DVEVENT_STARTSESSION                0x00000001

// If the transport session is lost or shutdown
#define DVEVENT_STOPSESSION                 0x00000002

// A player was added to the system,
// Param1 = DVID of new player
// Param2 = Player context (set by handler and then returned)
#define DVEVENT_ADDPLAYER                   0x00000003

// A player disconnected.
// Param1 = DVID of disconnected player
// Param1 = Player context
#define DVEVENT_REMOVEPLAYER                0x00000004

// A group was created.  Param1 = DVID of created group
#define DVEVENT_CREATEGROUP                 0x00000005

// A group was deleted.  Param1 = DVID of deleted group
#define DVEVENT_DELETEGROUP                 0x00000006

// Player was added to a group.
// Param1 = DVID of group    Param2 = DVID of player
#define DVEVENT_ADDPLAYERTOGROUP            0x00000007

// Player was removed from the group
// Param1 = DVID of group    Param2 = DVID of player
#define DVEVENT_REMOVEPLAYERFROMGROUP       0x00000008

// Called when the host migrates
// Param1 = DVID of new host (player ID, not system ID)
#define DVEVENT_MIGRATEHOST                 0x00000009

// Called when a buffer the voice layer has given
// the transport is completed.
// Param1 = pointer to DVEVENTMSG_SENDCOMPLETE structure
//
#define DVEVENT_SENDCOMPLETE                0x0000000A

typedef struct _DVEVENTMSG_SENDCOMPLETE
{
    LPVOID                  pvUserContext;
    HRESULT                 hrSendResult;
} DVEVENTMSG_SENDCOMPLETE, *PDVEVENTMSG_SENDCOMPLETE;

//@@END_MSINTERNAL



/****************************************************************************
 *
 * DirectPlayVoice Functions
 *
 ****************************************************************************/

#define IID_IDirectPlayVoiceClient 1
#define IID_IDirectPlayVoiceServer 2

extern HRESULT WINAPI DirectPlayVoiceCreate(
    DWORD dwIID,
    void **ppvInterface,
    void *pUnknown);

extern HRESULT WINAPI XDirectPlayVoiceCreate(
	DWORD dwIID, 
	void** ppvInterface);

/****************************************************************************
 *
 * DirectPlay8 Application Interfaces
 *
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ULONG WINAPI IDirectPlayVoiceClient_AddRef(LPDIRECTPLAYVOICECLIENT pArg1);
ULONG WINAPI IDirectPlayVoiceClient_Release(LPDIRECTPLAYVOICECLIENT pArg1);

HRESULT WINAPI IDirectPlayVoiceClient_Initialize(LPDIRECTPLAYVOICECLIENT pArg0, PVOID pArg1, PDVMESSAGEHANDLER pArg2, PVOID pArg3, LPDWORD pArg4, DWORD Arg5);
HRESULT WINAPI IDirectPlayVoiceClient_Connect(LPDIRECTPLAYVOICECLIENT pArg0, PDVCLIENTCONFIG pArg1, DWORD Arg2 );
HRESULT WINAPI IDirectPlayVoiceClient_Disconnect(LPDIRECTPLAYVOICECLIENT pArg0, DWORD Arg1 );
HRESULT WINAPI IDirectPlayVoiceClient_DoWork(LPDIRECTPLAYVOICECLIENT pArg0);
HRESULT WINAPI IDirectPlayVoiceClient_GetSessionDesc(LPDIRECTPLAYVOICECLIENT pArg0, PDVSESSIONDESC pArg1 );
HRESULT WINAPI IDirectPlayVoiceClient_GetClientConfig(LPDIRECTPLAYVOICECLIENT pArg0, PDVCLIENTCONFIG pArg1 );
HRESULT WINAPI IDirectPlayVoiceClient_SetClientConfig(LPDIRECTPLAYVOICECLIENT pArg0, PDVCLIENTCONFIG pArg1 );
HRESULT WINAPI IDirectPlayVoiceClient_CreateSoundTarget(LPDIRECTPLAYVOICECLIENT pArg0, DVID dvidID, PWAVEFORMATEX *ppwfxMediaFormat, XMediaObject **ppMediaObject);
HRESULT WINAPI IDirectPlayVoiceClient_DeleteSoundTarget(LPDIRECTPLAYVOICECLIENT pArg0, DVID dvidID, XMediaObject **ppMediaObject);
HRESULT WINAPI IDirectPlayVoiceClient_SetTransmitTargets(LPDIRECTPLAYVOICECLIENT pArg0, PDVID pArg1, DWORD Arg2, DWORD Arg3 );
HRESULT WINAPI IDirectPlayVoiceClient_GetTransmitTargets(LPDIRECTPLAYVOICECLIENT pArg0, PDVID pArg1, PDWORD pArg2, DWORD Arg3 );
HRESULT WINAPI IDirectPlayVoiceClient_SetNotifyMask(LPDIRECTPLAYVOICECLIENT pArg0, PDWORD pArg1, DWORD Arg2 );


#ifdef __cplusplus
}
#endif // __cplusplus

/*
 * DIRECTVOICE Interface Definition
 *
 */

#ifdef __cplusplus

struct IDirectPlayVoiceClient
{

    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IDirectPlayVoiceClient_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IDirectPlayVoiceClient_Release(this);
    }

    /*** IDirectPlayVoiceClient methods ***/
    __inline HRESULT STDMETHODCALLTYPE Initialize (PVOID pArg1, PDVMESSAGEHANDLER pArg2, PVOID pArg3, LPDWORD pArg4, DWORD Arg5 )
    {
        return IDirectPlayVoiceClient_Initialize(this,pArg1,pArg2,pArg3,pArg4, Arg5);
    }

    __inline HRESULT STDMETHODCALLTYPE Connect (PDVCLIENTCONFIG pArg1, DWORD Arg2 )
    {
        return IDirectPlayVoiceClient_Connect(this,pArg1,Arg2);
    }

    __inline HRESULT STDMETHODCALLTYPE Disconnect (DWORD Arg1 )
    {
        return IDirectPlayVoiceClient_Disconnect(this,Arg1);
    }

    __inline HRESULT STDMETHODCALLTYPE DoWork ()
    {

        return IDirectPlayVoiceClient_DoWork(this);

    }

    __inline HRESULT STDMETHODCALLTYPE GetSessionDesc (PDVSESSIONDESC pArg1 )
    {
        return IDirectPlayVoiceClient_GetSessionDesc(this,pArg1);
    }

    __inline HRESULT STDMETHODCALLTYPE GetClientConfig (PDVCLIENTCONFIG pArg1 )
    {
        return IDirectPlayVoiceClient_GetClientConfig(this,pArg1);
    }

    __inline HRESULT STDMETHODCALLTYPE SetClientConfig (PDVCLIENTCONFIG pArg1 )
    {
        return IDirectPlayVoiceClient_SetClientConfig(this,pArg1);
    }

    __inline HRESULT STDMETHODCALLTYPE CreateSoundTarget( DVID dvidID, PWAVEFORMATEX *ppwfxMediaFormat, XMediaObject **ppMediaObject)
    {
        return IDirectPlayVoiceClient_CreateSoundTarget( this, dvidID, ppwfxMediaFormat, ppMediaObject);
    }

    __inline HRESULT STDMETHODCALLTYPE DeleteSoundTarget( DVID dvidID, XMediaObject **ppMediaObject )
    {
        return IDirectPlayVoiceClient_DeleteSoundTarget( this, dvidID, ppMediaObject );
    }

    __inline HRESULT STDMETHODCALLTYPE SetTransmitTargets (PDVID pArg1, DWORD Arg2, DWORD Arg3 )
    {
        return IDirectPlayVoiceClient_SetTransmitTargets(this, pArg1, Arg2, Arg3);
    }

    __inline HRESULT STDMETHODCALLTYPE GetTransmitTargets (PDVID pArg1, PDWORD pArg2, DWORD Arg3 )
    {
        return IDirectPlayVoiceClient_GetTransmitTargets(this, pArg1, pArg2, Arg3);
    }

    __inline HRESULT STDMETHODCALLTYPE SetNotifyMask (PDWORD pArg1, DWORD Arg2 )
    {
        return IDirectPlayVoiceClient_SetNotifyMask(this, pArg1, Arg2);
    }

};

#endif // __cplusplus

//@@BEGIN_MSINTERNAL

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ULONG WINAPI IDirectPlayVoiceNotify_AddRef(LPDIRECTPLAYVOICENOTIFY pArg1);
ULONG WINAPI IDirectPlayVoiceNotify_Release(LPDIRECTPLAYVOICENOTIFY pArg1);

HRESULT WINAPI IDirectPlayVoiceNotify_Initialize(LPDIRECTPLAYVOICENOTIFY pArg1);
HRESULT WINAPI IDirectPlayVoiceNotify_NotifyEvent(LPDIRECTPLAYVOICENOTIFY pArg0, DWORD Arg1, DWORD Arg2, DWORD Arg3);
HRESULT WINAPI IDirectPlayVoiceNotify_ReceiveSpeechMessage(LPDIRECTPLAYVOICENOTIFY pArg0, DVID Arg1, DVID Arg2, PVOID pArg3, DWORD Arg4);

#ifdef __cplusplus
}
#endif // __cplusplus


#ifdef __cplusplus

struct IDirectPlayVoiceNotify
{

    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IDirectPlayVoiceNotify_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IDirectPlayVoiceNotify_Release(this);
    }

    // Initialize
    //
    // Initializes the DirectPlayVoice interface associated with this
    // interface.  During this call DirectPlayVoice will call
    // GetSessionInfo on the associated Transport interface.
    //

    __inline HRESULT STDMETHODCALLTYPE Initialize()
    {
        return IDirectPlayVoiceNotify_Initialize(this);
    }

    // NotifyEvent
    //
    // Called when an event occurs that DirectPlayVoice needs to be informed
    // of.  See descriptions of DVEVENT_XXXXX for how the parameters are
    // used for each message.
    //
    // DWORD - Type of message (DVEVENT_XXXXXX)
    // DWORD - Param1
    // DWORD - Param2
    //

    __inline HRESULT STDMETHODCALLTYPE NotifyEvent(DWORD Arg1, DWORD Arg2, DWORD Arg3)
    {
        return IDirectPlayVoiceNotify_NotifyEvent(this, Arg1, Arg2, Arg3);
    }

    // ReceiveSpeechMessage
    //
    // Called when a message is received by DirectPlay that is for
    // DirectPlayVoice.
    //
    // DVID - Source of the message
    // LPVOID - Pointer to message buffer
    // DWORD - Size of the received message
    //
    __inline HRESULT STDMETHODCALLTYPE ReceiveSpeechMessage(DVID Arg1, DVID Arg2, PVOID pArg3, DWORD Arg4)
    {
        return IDirectPlayVoiceNotify_ReceiveSpeechMessage(this, Arg1, Arg2, pArg3, Arg4);
    }

};

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ULONG WINAPI IDirectPlayVoiceTransport_AddRef(LPDIRECTPLAYVOICETRANSPORT pArg1);
ULONG WINAPI IDirectPlayVoiceTransport_Release(LPDIRECTPLAYVOICETRANSPORT pArg1);

HRESULT WINAPI IDirectPlayVoiceTransport_DoWork(LPDIRECTPLAYVOICETRANSPORT pArg0, DWORD Arg1);
HRESULT WINAPI IDirectPlayVoiceTransport_Advise(LPDIRECTPLAYVOICETRANSPORT pArg0, LPUNKNOWN pArg1, DWORD pArg2);
HRESULT WINAPI IDirectPlayVoiceTransport_UnAdvise(LPDIRECTPLAYVOICETRANSPORT pArg0, DWORD Arg1);
HRESULT WINAPI IDirectPlayVoiceTransport_IsGroupMember(LPDIRECTPLAYVOICETRANSPORT pArg0, DVID Arg1, DVID Arg2);
HRESULT WINAPI IDirectPlayVoiceTransport_SendSpeech(LPDIRECTPLAYVOICETRANSPORT pArg0, DVID Arg1, DVID Arg2, PVOID pArg3, PVOID pArg4, DWORD Arg5);
HRESULT WINAPI IDirectPlayVoiceTransport_GetSessionInfo(LPDIRECTPLAYVOICETRANSPORT pArg0, PDVTRANSPORTINFO pArg1);
HRESULT WINAPI IDirectPlayVoiceTransport_IsValidEntity(LPDIRECTPLAYVOICETRANSPORT pArg0, DVID Arg1, PBOOL pArg2);
HRESULT WINAPI IDirectPlayVoiceTransport_SendSpeechEx(LPDIRECTPLAYVOICETRANSPORT pArg0, DVID Arg1, DWORD Arg2, PDVID pArg3, PVOID pArg4, PVOID Arg5, DWORD Arg6);
HRESULT WINAPI IDirectPlayVoiceTransport_IsValidGroup(LPDIRECTPLAYVOICETRANSPORT pArg0, DVID Arg1, PBOOL pArg2);
HRESULT WINAPI IDirectPlayVoiceTransport_IsValidPlayer(LPDIRECTPLAYVOICETRANSPORT pArg0, DVID Arg1, PBOOL pArg2);

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus

struct IDirectPlayVoiceTransport
{

    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IDirectPlayVoiceTransport_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IDirectPlayVoiceTransport_Release(this);
    }

    /*** IDirectPlayVoiceTransport methods ***/

    // DoWork
    //

    __inline HRESULT STDMETHODCALLTYPE DoWork(DWORD Arg1)
    {
        return IDirectPlayVoiceTransport_DoWork(this, Arg1);
    }

    // Advise
    //
    // Advises the transport to call us back via the interface passed in the
    // LLPUNKNOWN parameter.  QueryInterface on the LPUNKNOWN for a
    // IDirectPlayVoiceNotify.  Must call IDirectPlayVoiceNotify::INitialize
    // on the interface before returning.
    //
    // LPUNKNOWN - IUnknown interface for the IDirectPlayVoiceNotify to
    //             make notifications on.
    // DWORD - Voice Object Type (DVTRANSPORT_OBJECTTYPE_XXXX)
    //

    __inline HRESULT STDMETHODCALLTYPE Advise(LPUNKNOWN pArg1, DWORD Arg2)
    {
        return IDirectPlayVoiceTransport_Advise(this, pArg1, Arg2);
    }

    // UnAdvise
    //
    // Tells the transport that we no longer need to be called back on our
    // notify interface.  The transport should Release the instance of the
    // notify interface that they have.
    //
    // DWORD - Voice Object Type (DVTRANSPORT_OBJECTTYPE_XXXX)
    //

    __inline HRESULT STDMETHODCALLTYPE UnAdvise(DWORD Arg1)
    {
        return IDirectPlayVoiceTransport_UnAdvise(this, Arg1);
    }

    // IsGroupMember
    //
    // This function returns DP_OK if the specified user is a member of
    // the specified group.
    //
    // DVID - DVID of the group to check
    // DVID - DVID of the player
    //
    __inline HRESULT STDMETHODCALLTYPE IsGroupMember(DVID Arg1, DVID Arg2)
    {
        return IDirectPlayVoiceTransport_IsGroupMember(this, Arg1, Arg2);
    }


    // SendSpeech
    //
    // Transmits a message from the specified user ID to the specified user ID.
    // (Speech specific).  Messages sent through this interface are always
    // sent ASYNC and if the DVTRANSPORT_SEND_GUARANTEED flag is specified
    // then the message MUST be sent guaranteed.
    //
    // DVID - ID of the player this will be from.
    // DVID - ID of the player this will be sent to.
    // LPVOID - Pointer to the data to send
    // LPVOID - User context for send
    // DWORD - FLags (Combination of DVTRANSPORT_SEND_GUARANTEED).
    //

    __inline HRESULT STDMETHODCALLTYPE SendSpeech(DVID Arg1, DVID Arg2, PVOID pArg3, PVOID pArg4, DWORD Arg5)
    {
        return IDirectPlayVoiceTransport_SendSpeech(this, Arg1, Arg2, pArg3, pArg4, Arg5);
    }

    // GetSessionInfo
    //
    // Fills the passed structure with details on the session that is running
    // on the transport object.  See description of DVTRANSPORTINFO for details.
    //
    __inline HRESULT STDMETHODCALLTYPE GetSessionInfo(PDVTRANSPORTINFO pArg1)
    {
        return IDirectPlayVoiceTransport_GetSessionInfo(this, pArg1);
    }

    // IsValidEntity
    //
    // Checks to see if specified user is valid player or group in session
    // DVID = ID of the entity to check
    // LPBOOL = Pointer to BOOL to place result.  TRUE for Valid Player/
    //          Group, FALSE if it is not.
    //
    // Not needed in Client/Server Mode
    __inline HRESULT STDMETHODCALLTYPE IsValidEntity(DVID Arg1, PBOOL pArg2)
    {
        return IDirectPlayVoiceTransport_IsValidEntity(this, Arg1, pArg2);
    }

    // SendSpeechEx
    //
    // Transmits a message from the specified user ID to the specified user ID.
    // (Speech specific).  Messages sent through this interface are always
    // sent ASYNC and if the DVTRANSPORT_SEND_GUARANTEED flag is specified
    // then the message MUST be sent guaranteed.
    //
    // DVID - ID of the player this will be from.
    // DWORD - count on entries in (DVID *)[] array
    // LPDVID - Array of send targets
    // LPVOID - Pointer to the data to send
    // LPVOID - User context for send
    // DWORD - Flags (Combination of DVTRANSPORT_SEND_GUARANTEED).
    //

    __inline HRESULT STDMETHODCALLTYPE SendSpeechEx(DVID Arg1, DWORD Arg2, PDVID pArg3, PVOID pArg4, PVOID pArg5, DWORD Arg6)
    {
        return IDirectPlayVoiceTransport_SendSpeechEx(this, Arg1, Arg2, pArg3, pArg4, pArg5, Arg6);
    }

    // IsValidGroup
    //
    // Checks to see if the specified ID is a valid Group ID
    //
    // DVID = ID of the entity to check
    //
    __inline HRESULT STDMETHODCALLTYPE IsValidGroup(DVID Arg1, PBOOL pArg2)
    {
        return IDirectPlayVoiceTransport_IsValidGroup(this, Arg1, pArg2);
    }

    // IsValidPlayer
    //
    // Checks to see if the specified ID is a valid Player ID
    //
    // DVID = ID of the entity to check
    //
    __inline HRESULT STDMETHODCALLTYPE IsValidPlayer(DVID Arg1, PBOOL pArg2)
    {
        return IDirectPlayVoiceTransport_IsValidPlayer(this, Arg1, pArg2);
    }



};

#endif // __cplusplus

//@@END_MSINTERNAL


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ULONG WINAPI IDirectPlayVoiceServer_AddRef(LPDIRECTPLAYVOICESERVER pArg1);
ULONG WINAPI IDirectPlayVoiceServer_Release(LPDIRECTPLAYVOICESERVER pArg1);

HRESULT WINAPI IDirectPlayVoiceServer_Initialize(LPDIRECTPLAYVOICESERVER pArg0,PVOID pArg1, PDVMESSAGEHANDLER pArg2, PVOID pArg3, LPDWORD pArg4, DWORD Arg5 );
HRESULT WINAPI IDirectPlayVoiceServer_StartSession(LPDIRECTPLAYVOICESERVER pArg0,PDVSESSIONDESC pArg1, DWORD Arg2);
HRESULT WINAPI IDirectPlayVoiceServer_StopSession(LPDIRECTPLAYVOICESERVER pArg0,DWORD Arg1);
HRESULT WINAPI IDirectPlayVoiceServer_GetSessionDesc(LPDIRECTPLAYVOICESERVER pArg0,PDVSESSIONDESC pArg1 );
HRESULT WINAPI IDirectPlayVoiceServer_SetTransmitTargets(LPDIRECTPLAYVOICESERVER pArg0,DVID Arg1, PDVID pArg2, DWORD Arg3, DWORD Arg4 );
HRESULT WINAPI IDirectPlayVoiceServer_GetTransmitTargets(LPDIRECTPLAYVOICESERVER pArg0,DVID Arg1, PDVID pArg2, PDWORD Arg3, DWORD Arg4 );
HRESULT WINAPI IDirectPlayVoiceServer_SetNotifyMask(LPDIRECTPLAYVOICESERVER pArg0,PDWORD pArg1, DWORD Arg2 );

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus

struct IDirectPlayVoiceServer
{
    /*** IUnknown methods ***/

    __inline ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return IDirectPlayVoiceServer_AddRef(this);
    }

    __inline ULONG STDMETHODCALLTYPE Release(void)
    {
        return IDirectPlayVoiceServer_Release(this);
    }

    /*** IDirectPlayVoiceServer methods ***/
    __inline HRESULT STDMETHODCALLTYPE Initialize (PVOID pArg1, PDVMESSAGEHANDLER pArg2, PVOID pArg3, LPDWORD pArg4, DWORD Arg5 )
    {
        return IDirectPlayVoiceServer_Initialize(this,pArg1,pArg2,pArg3,pArg4, Arg5);
    }

    __inline HRESULT STDMETHODCALLTYPE StartSession(PDVSESSIONDESC pArg1, DWORD Arg2)
    {
        return IDirectPlayVoiceServer_StartSession(this,pArg1,Arg2);
    }

    __inline HRESULT STDMETHODCALLTYPE StopSession(DWORD Arg1)
    {
        return IDirectPlayVoiceServer_StopSession(this,Arg1);
    }

    __inline HRESULT STDMETHODCALLTYPE GetSessionDesc (PDVSESSIONDESC pArg1 )
    {
        return IDirectPlayVoiceServer_GetSessionDesc(this,pArg1);
    }

    __inline HRESULT STDMETHODCALLTYPE SetTransmitTargets (DVID Arg1, PDVID pArg2, DWORD Arg3, DWORD Arg4 )
    {
        return IDirectPlayVoiceServer_SetTransmitTargets(this, Arg1, pArg2, Arg3, Arg4);
    }

    __inline HRESULT STDMETHODCALLTYPE GetTransmitTargets (DVID Arg1, PDVID pArg2, PDWORD pArg3, DWORD Arg4 )
    {
        return IDirectPlayVoiceServer_GetTransmitTargets(this, Arg1, pArg2, pArg3, Arg4);
    }

    __inline HRESULT STDMETHODCALLTYPE SetNotifyMask (PDWORD pArg1, DWORD Arg2 )
    {
        return IDirectPlayVoiceServer_SetNotifyMask(this, pArg1, Arg2);
    }

};

#endif // __cplusplus


/****************************************************************************
 *
 * DIRECTPLAYVOICE ERRORS
 *
 * Errors are represented by negative values and cannot be combined.
 *
 ****************************************************************************/

#define _FACDPV  0x15
#define MAKE_DVHRESULT( code )          MAKE_HRESULT( 1, _FACDPV, code )

#define DV_OK                           S_OK
#define DV_FULLDUPLEX                   MAKE_HRESULT( 0, _FACDPV,  0x0005 )
#define DV_HALFDUPLEX                   MAKE_HRESULT( 0, _FACDPV,  0x000A )
#define DV_PENDING                      MAKE_HRESULT( 0, _FACDPV,  0x0010 )

#define DVERR_BUFFERTOOSMALL            MAKE_DVHRESULT(  0x001E )
#define DVERR_EXCEPTION                 MAKE_DVHRESULT(  0x004A )
#define DVERR_GENERIC                   E_FAIL
#define DVERR_INVALIDFLAGS              MAKE_DVHRESULT( 0x0078 )
#define DVERR_INVALIDOBJECT             MAKE_DVHRESULT( 0x0082 )
#define DVERR_INVALIDPARAM              E_INVALIDARG
#define DVERR_INVALIDPLAYER             MAKE_DVHRESULT( 0x0087 )
#define DVERR_INVALIDGROUP              MAKE_DVHRESULT( 0x0091 )
#define DVERR_INVALIDHANDLE             MAKE_DVHRESULT( 0x0096 )
#define DVERR_OUTOFMEMORY               E_OUTOFMEMORY
#define DVERR_PENDING                   DV_PENDING
#define DVERR_NOTSUPPORTED              E_NOTIMPL
#define DVERR_NOINTERFACE               E_NOINTERFACE
#define DVERR_SESSIONLOST               MAKE_DVHRESULT( 0x012C )
#define DVERR_NOVOICESESSION            MAKE_DVHRESULT( 0x012E )
#define DVERR_CONNECTIONLOST            MAKE_DVHRESULT( 0x0168 )
#define DVERR_NOTINITIALIZED            MAKE_DVHRESULT( 0x0169 )
#define DVERR_CONNECTED                 MAKE_DVHRESULT( 0x016A )
#define DVERR_NOTCONNECTED              MAKE_DVHRESULT( 0x016B )
#define DVERR_CONNECTABORTING           MAKE_DVHRESULT( 0x016E )
#define DVERR_NOTALLOWED                MAKE_DVHRESULT( 0x016F )
#define DVERR_INVALIDTARGET             MAKE_DVHRESULT( 0x0170 )
#define DVERR_TRANSPORTNOTHOST          MAKE_DVHRESULT( 0x0171 )
#define DVERR_COMPRESSIONNOTSUPPORTED   MAKE_DVHRESULT( 0x0172 )
#define DVERR_ALREADYPENDING            MAKE_DVHRESULT( 0x0173 )
#define DVERR_SOUNDINITFAILURE          MAKE_DVHRESULT( 0x0174 )
#define DVERR_TIMEOUT                   MAKE_DVHRESULT( 0x0175 )
#define DVERR_CONNECTABORTED            MAKE_DVHRESULT( 0x0176 )
#define DVERR_NO3DSOUND                 MAKE_DVHRESULT( 0x0177 )
#define DVERR_ALREADYBUFFERED           MAKE_DVHRESULT( 0x0178 )
#define DVERR_NOTBUFFERED               MAKE_DVHRESULT( 0x0179 )
#define DVERR_HOSTING                   MAKE_DVHRESULT( 0x017A )
#define DVERR_NOTHOSTING                MAKE_DVHRESULT( 0x017B )
#define DVERR_INVALIDDEVICE             MAKE_DVHRESULT( 0x017C )
#define DVERR_RECORDSYSTEMERROR         MAKE_DVHRESULT( 0x017D )
#define DVERR_PLAYBACKSYSTEMERROR       MAKE_DVHRESULT( 0x017E )
#define DVERR_SENDERROR                 MAKE_DVHRESULT( 0x017F )
#define DVERR_USERCANCEL                MAKE_DVHRESULT( 0x0180 )
#define DVERR_RUNSETUP                  MAKE_DVHRESULT( 0x0183 )
#define DVERR_INCOMPATIBLEVERSION       MAKE_DVHRESULT( 0x0184 )
#define DVERR_INITIALIZED               MAKE_DVHRESULT( 0x0187 )
#define DVERR_INVALIDPOINTER            E_POINTER
#define DVERR_NOTRANSPORT               MAKE_DVHRESULT( 0x0188 )
#define DVERR_NOCALLBACK                MAKE_DVHRESULT( 0x0189 )
#define DVERR_TRANSPORTNOTINIT          MAKE_DVHRESULT( 0x018A )
#define DVERR_TRANSPORTNOSESSION        MAKE_DVHRESULT( 0x018B )
#define DVERR_TRANSPORTNOPLAYER         MAKE_DVHRESULT( 0x018C )
#define DVERR_INVALIDBUFFER             MAKE_DVHRESULT( 0x018F )


#endif
