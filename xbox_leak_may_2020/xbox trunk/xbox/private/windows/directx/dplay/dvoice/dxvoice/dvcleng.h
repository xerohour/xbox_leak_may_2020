/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvclientengine.h
 *  Content:	Definition of class for DirectXVoice Client
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/06/99		rodtoll	Created it
 * 09/01/2000  georgioc started rewrite for xbox
 ***************************************************************************/

#ifndef __DVCLIENTENGINE_H
#define __DVCLIENTENGINE_H

#include "dvengine.h"
#include "dvtran.h"
#include "frame.h"
#include "inqueue2.h"
#include "dvprot.h"
#include "vnametbl.h"

#include "dvsndt.h"

#define DVCSTATE_NOTINITIALIZED		0x00000000
#define DVCSTATE_IDLE				0x00000001
#define DVCSTATE_CONNECTING			0x00000002
#define DVCSTATE_CONNECTED			0x00000003
#define DVCSTATE_DISCONNECTING		0x00000004
#define DVCSTATE_DISCONNECTED       0x00000005
struct DIRECTVOICECLIENTOBJECT;

// Size in bytes of the fixed size elements
#define DV_CLIENT_NOTIFY_ELEMENT_SIZE	256

#if defined(DEBUG) || defined(DBG)
#define CHECKLISTINTEGRITY			CheckListIntegrity
#else
#define CHECKLISTINTEGRITY()		
#endif

#define VSIG_CLIENTENGINE		'ELCV'
#define VSIG_CLIENTENGINE_FREE	'ELC_'



// CDirectVoiceClientEngine
//
// This class represents the IDirectXVoiceClient interface.
//
//
volatile class CDirectVoiceClientEngine: public CDirectVoiceEngine
{
protected:

    typedef enum { NOTIFY_FIXED,   // Structure stored in fixed
    			   NOTIFY_DYNAMIC  // Memory allocated in dynamic
    			 } ElementType;

    struct CNotifyElement
    {
    	typedef VOID (*PNOTIFY_COMPLETE)(PVOID pvContext, CNotifyElement *pElement);		

    	DWORD			m_dwType;
    	struct _Element
    	{
    		BYTE			Data[DV_CLIENT_NOTIFY_ELEMENT_SIZE];
    	} m_element;

    	DWORD				m_dwDataSize;		
    	PVOID				pvContext;
    	PNOTIFY_COMPLETE	pNotifyFunc;

        __inline void *__cdecl operator new(size_t size)
        {
            return ExAllocatePoolWithTag(size, 'lcvd');
        }

        __inline void __cdecl operator delete(void *pv)
        {
            ExFreePool(pv);
        }

    };

	
public:
	CDirectVoiceClientEngine( DIRECTVOICECLIENTOBJECT *lpObject );
	~CDirectVoiceClientEngine();

    __inline void *__cdecl operator new(size_t size)
    {
        return ExAllocatePoolWithTag(size, 'lcvd');
    }

    __inline void __cdecl operator delete(void *pv)
    {
        ExFreePool(pv);
    }


public: // IDirectXVoiceClient Interface
	HRESULT Connect(LPDVCLIENTCONFIG lpClientConfig, DWORD dwFlags );
	HRESULT Disconnect( DWORD dwFlags );
	HRESULT GetSessionDesc( LPDVSESSIONDESC lpSessionDescBuffer );
	HRESULT GetClientConfig( LPDVCLIENTCONFIG lpClientConfig );
	HRESULT SetClientConfig( LPDVCLIENTCONFIG lpClientConfig );
	HRESULT GetCompressionTypes( LPVOID lpBuffer, LPDWORD lpdwSize, LPDWORD lpdwNumElements, DWORD dwFlags );
	HRESULT SetTransmitTarget( PDVID dvidTarget, DWORD dwNumTargets, DWORD dwFlags );
	HRESULT GetTransmitTarget( LPDVID lpdvidTargets, PDWORD pdwNumElements, DWORD dwFlags );
    HRESULT CreateSoundTarget( DVID dvidID, PWAVEFORMATEX *ppwfx, XMediaObject **ppMediaObject);
    HRESULT DeleteSoundTarget( DVID dvidID, XMediaObject **ppMediaObject);
	HRESULT SetNotifyMask( LPDWORD lpdwMessages, DWORD dwNumElements );
    HRESULT DoWork();

public: // CDirectVoiceEngine Members
	HRESULT Initialize( CDirectVoiceTransport *lpTransport, LPDVMESSAGEHANDLER lpdvHandler, LPVOID lpUserContext, LPDWORD lpdwMessages, DWORD dwNumElements );
	BOOL ReceiveSpeechMessage( DVID dvidSource, LPVOID lpMessage, DWORD dwSize );
	HRESULT StartTransportSession();
	HRESULT StopTransportSession();
	HRESULT AddPlayer( DVID dvID );
	HRESULT RemovePlayer( DVID dvID );
	HRESULT CreateGroup( DVID dvID );
	HRESULT DeleteGroup( DVID dvID );
	HRESULT AddPlayerToGroup( DVID dvidGroup, DVID dvidPlayer );
	HRESULT RemovePlayerFromGroup( DVID dvidGroup, DVID dvidPlayer );
	HRESULT MigrateHost( DVID dvidNewHost, LPDIRECTPLAYVOICESERVER lpdvServer );

	HRESULT MigrateHost_RunElection();

	inline DWORD GetCurrentState() { return m_dwCurrentState; };	

protected: // Message handlers

	HRESULT InternalSetNotifyMask( LPDWORD lpdwMessages, DWORD dwNumElements );

	BOOL QueueSpeech( DVID dvidSource, PDVPROTOCOLMSG_SPEECHHEADER pdvSpeechHeader, PBYTE pbData, DWORD dwSize );

	BOOL HandleConnectRefuse( DVID dvidSource, PDVPROTOCOLMSG_CONNECTREFUSE lpdvConnectRefuse, DWORD dwSize );
	BOOL HandleCreateVoicePlayer( DVID dvidSource, PDVPROTOCOLMSG_PLAYERJOIN lpdvCreatePlayer, DWORD dwSize );
	BOOL HandleDeleteVoicePlayer( DVID dvidSource, PDVPROTOCOLMSG_PLAYERQUIT lpdvDeletePlayer, DWORD dwSize );
	BOOL HandleSpeech( DVID dvidSource, PDVPROTOCOLMSG_SPEECHHEADER lpdvSpeech, DWORD dwSize );
	BOOL HandleSpeechWithFrom( DVID dvidSource, PDVPROTOCOLMSG_SPEECHWITHFROM lpdvSpeech, DWORD dwSize );	
	BOOL HandleSpeechBounce( DVID dvidSource, PDVPROTOCOLMSG_SPEECHHEADER lpdvSpeech, DWORD dwSize );
	BOOL HandleConnectAccept( DVID dvidSource, PDVPROTOCOLMSG_CONNECTACCEPT lpdvConnectAccept, DWORD dwSize );
	BOOL HandleDisconnectConfirm( DVID dvidSource, PDVPROTOCOLMSG_DISCONNECT lpdvDisconnect, DWORD dwSize );
	BOOL HandleSetTarget( DVID dvidSource, PDVPROTOCOLMSG_SETTARGET lpdvSetTarget, DWORD dwSize );
	BOOL HandleSessionLost( DVID dvidSource, PDVPROTOCOLMSG_SESSIONLOST lpdvSessionLost, DWORD dwSize );
	BOOL HandlePlayerList( DVID dvidSource, PDVPROTOCOLMSG_PLAYERLIST lpdvPlayerList, DWORD dwSize );
	BOOL HandleHostMigrated( DVID dvidSource, PDVPROTOCOLMSG_HOSTMIGRATED lpdvHostMigrated, DWORD dwSize );
	BOOL HandleHostMigrateLeave( DVID dvidSource, PDVPROTOCOLMSG_HOSTMIGRATELEAVE lpdvHostMigrateLeave, DWORD dwSize );
	HRESULT HandleLocalHostMigrateCreate();

	friend class CClientRecordSubSystem;

protected:

    HRESULT DoPlaybackWork();
    HRESULT CheckConnected();
	void CheckListIntegrity();

	void DoSessionLost(HRESULT hrReason);
	void DoSignalDisconnect(HRESULT hrDisconnectReason);

	// Actually send the message to the client app
	void TransmitMessage( DWORD dwMessageType, LPVOID lpData, DWORD dwSize );

	void Cleanup();
	void DoDisconnect();
	void DoConnectResponse();
	void WaitForBufferReturns();

	void SetCurrentState( DWORD dwState );

protected:

    //
    // sound related functions
    //

	HRESULT InitializeSoundSystem();
    HRESULT BuildCompressionInfo();
	HRESULT ShutdownSoundSystem();
    VOID SwitchToHalfDuplex();


	void CheckForUserTimeout( DWORD dwCurTime );
#if DVC_ENABLE_PLAYBACK_LEVEL_TRACKING
	VOID SendPlayerLevels();
#endif
	BOOL CheckShouldSendMessage( DWORD dwMessageType );
	
	void UpdateActivePlayPendingList();
	void UpdateActiveNotifyPendingList();
	void CleanupNotifyLists();
	void CleanupPlaybackLists();

	PDVTRANSPORT_BUFFERDESC GetTransmitBuffer( DWORD dwSize, LPVOID *ppvContext, BOOLEAN bUseSpeechPool );
    HRESULT SendComplete( PDVEVENTMSG_SENDCOMPLETE pSendComplete );
    void ReturnTransmitBuffer( PVOID pvContext );

    HRESULT Send_ConnectRequest();
    HRESULT Send_DisconnectRequest();
    HRESULT Send_SessionLost();
    HRESULT Send_SettingsConfirm();

	DWORD						m_dwSignature;

    CFramePool                  *m_lpFramePool;			// Pool of frames
    CDirectVoiceTransport		*m_lpSessionTransport;	// Transport for the session
	DVCLIENTCONFIG				m_dvClientConfig;		// Sound general config
	DVSESSIONDESC				m_dvSessionDesc;		// Session configuration
	LPDVMESSAGEHANDLER			m_lpMessageHandler;		// User message handler
	LPVOID						m_lpUserContext;		// User context for message handler
	DVID						m_dvidServer;			// DVID of the server

	PDVID						m_pdvidTargets;			// DVID of the current target(s) (Protected by m_csTargetLock)
	DWORD						m_dwNumTargets;			// # of targets (Protected by m_csTargetLock)
	DWORD						m_dwTargetVersion;		// Increment each time targets are changed

	HRESULT						InternalSetTransmitTarget( PDVID pdvidTargets, DWORD dwNumTargets );
	HRESULT						CheckForAndRemoveTarget( DVID dvidID );
	
	volatile DWORD    			m_dwCurrentState;		// Current engine state
	CFrame						*m_tmpFrame;			// Tmp frame for receiving
	LPDIRECTPLAYVOICESERVER		m_lpdvServerMigrated;	// Stores reference to migrated host

protected: // user attached sound targets

    HRESULT                     FindSoundTarget(DVID dvidID,
                                                CSoundTarget **lpcsTarget,
                                                CVoicePlayer **ppPlayer);

protected: // Notification queue 

	// Queue up a notification for the user
	HRESULT NotifyQueue_Add( DWORD dwMessageType, LPVOID lpData, DWORD dwDataSize, PVOID pvContext = NULL, CNotifyElement::PNOTIFY_COMPLETE pNotifyFunc = NULL );
	HRESULT NotifyQueue_Free();
	HRESULT NotifyQueue_ElementFree( CNotifyElement *lpElement );
	HRESULT NotifyQueue_IndicateNext();
	void NotifyQueue_Disable();
	void NotifyQueue_Enable();

	static void NotifyComplete_LocalPlayer( PVOID pvContext, CNotifyElement *pElement );
	static void NotifyComplete_RemotePlayer( PVOID pvContext, CNotifyElement *pElement );
    static void NotifyComplete_DeletePlayer(PVOID pvContext, CNotifyElement *pElement );

	BOOL						m_fNotifyQueueEnabled;
	LIST_ENTRY				    m_NotifyList;		// List of notifications

	HRESULT						SendConnectResult();
	HRESULT						SendDisconnectResult();
    VOID                        QueuePlayerLevelNotifications();

protected: 

	HRESULT SetupInitialBuffers();
	HRESULT SetupSpeechBuffer();
	HRESULT FreeBuffers();

protected: // Sound System Information


    CClientRecordSubSystem      *m_RecordSubSystem;       // record subsystem

	DWORD						m_dwEventFlags;		    // bitfield of event flags
	HRESULT						m_hrDisconnectResult;	// Result of the disconnect result
	HRESULT						m_hrConnectResult;		// Result of the connection request
	DVID						m_dvidLocal;			// Local DVID

protected:

    CRITICAL_SECTION            m_CS;
	// Compression Control Data
    XMediaObject               *m_pInBoundAudioConverter;

	CFramePool					*m_pFramePool;			// Frame pool

	DIRECTVOICECLIENTOBJECT		*m_lpObject;			// Cached pointer to the COM interface 

	DWORD 						m_dwSynchBegin;			// GetTickCount at Connect/Disconnect start

	LPDWORD						m_pMessageElements;	// Buffer with notifiers
	DWORD						m_dwNumMessageElements;	// Number of notifiers 

	DWORD						m_dwLastConnectSent;
	DWORD						m_dwHostOrderID;

	PVOID						m_pvLocalPlayerContext;

	BYTE						m_bLastPeak;			// Last frame peak
    BYTE                        m_bLastPlaybackPeak;    // last overall playback peak
	BYTE						m_bMsgNum;				// Last msg # transmitted
	BYTE						m_bSeqNum;				// Last sequence # transmitted
	
	BOOL						m_bLastTransmitted;		// Was last frame sent?
	BOOL						m_fSessionLost;			// Flag indicating session was lost
	BOOL						m_fLocalPlayerNotify;	// Has notification been sent for local player
	BOOL						m_fLocalPlayerAvailable;

    CVoiceNameTable             m_voiceNameTable;

	DWORD                       m_dwPlayActiveCount;
	LIST_ENTRY  				m_PlayActivePlayersList;
	LIST_ENTRY					m_PlayAddPlayersList;

	LIST_ENTRY					m_NotifyActivePlayersList;
	LIST_ENTRY					m_NotifyAddPlayersList;

	LIST_ENTRY     		      m_NotificationElementPool;		// Frame pool for notifications
    LIST_ENTRY                m_BufferDescPool;
    LIST_ENTRY                m_MessagePool;
    LIST_ENTRY                m_SpeechBufferPool;

    LIST_ENTRY                m_PlayerContextPool;

    PVOID                     m_pBufferDescAllocation;
    PVOID                     m_pMessagePoolAllocation;
    PVOID                     m_pSpeechBufferAllocation;


protected:

    DWORD                     m_SpeechBufferAllocs;
    DWORD                     m_MsgBufferAllocs;

};

#endif
