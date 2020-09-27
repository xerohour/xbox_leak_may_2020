/*==========================================================================
 * Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 * File:       dvserverengine.h
 * Content:    Definition of class for DirectXVoice Server
 * History:
 *   Date		By		Reason
 *   ====		==		======
 *	07/06/99	rodtoll	Created It
 * 09/01/2000  georgioc started rewrite/port to xbox 
 ***************************************************************************/
#ifndef __DVSERVERENGINE_H
#define __DVSERVERENGINE_H

#include "dvengine.h"
#include "dvtran.h"
#include "frame.h"
#include "inqueue2.h"
#include "dvprot.h"
#include "vnametbl.h"

struct DIRECTVOICESERVEROBJECT;

#define DVSSTATE_NOTINITIALIZED		0x00000000
#define DVSSTATE_IDLE				0x00000001
#define DVSSTATE_STARTUP			0x00000002
#define DVSSTATE_RUNNING			0x00000003
#define DVSSTATE_SHUTDOWN			0x00000004

// CDirectVoiceClientEngine
//
// This class represents the IDirectXVoiceServer interface.
//
#define VSIG_SERVERENGINE		'EVSV'
#define VSIG_SERVERENGINE_FREE	'EVS_'
//
volatile class CDirectVoiceServerEngine: public CDirectVoiceEngine
{

public:
	CDirectVoiceServerEngine( DIRECTVOICESERVEROBJECT *lpObject );
	~CDirectVoiceServerEngine();

public: // IDirectXVoiceServer Interface

	HRESULT HostMigrateStart(LPDVSESSIONDESC lpSessionDesc, DWORD dwHostOrderIDSeed = 0 );
    virtual HRESULT StartSession(LPDVSESSIONDESC lpSessionDesc, DWORD dwFlags, DWORD dwHostOrderIDSeed = 0 );
    virtual HRESULT StopSession(DWORD dwFlags, BOOL fSilent=FALSE, HRESULT hrResult = DV_OK );
    virtual HRESULT GetSessionDesc(LPDVSESSIONDESC lpSessionDescBuffer );
    virtual HRESULT SetTransmitTarget(DVID dvidSource, PDVID pdvidTargets, DWORD dwNumTargets, DWORD dwFlags);
    virtual HRESULT GetTransmitTarget(DVID dvidSource, LPDVID lpdvidTargets, PDWORD pdwNumElements, DWORD dwFlags );
	virtual HRESULT MigrateHost( DVID dvidNewHost, LPDIRECTPLAYVOICESERVER lpdvServer );    
	virtual HRESULT SetNotifyMask( LPDWORD lpdwMessages, DWORD dwNumElements );	

public: // CDirectVoiceEngine Members
	HRESULT Initialize( CDirectVoiceTransport *lpTransport, LPDVMESSAGEHANDLER lpdvHandler, LPVOID lpUserContext, LPDWORD lpdwMessages, DWORD dwNumElements );
	virtual BOOL ReceiveSpeechMessage( DVID dvidSource, LPVOID lpMessage, DWORD dwSize );
	HRESULT StartTransportSession();
	HRESULT StopTransportSession();
	HRESULT AddPlayer( DVID dvID );
	HRESULT RemovePlayer( DVID dvID );
	HRESULT CreateGroup( DVID dvID );
	HRESULT DeleteGroup( DVID dvID );
	HRESULT AddPlayerToGroup( DVID dvidGroup, DVID dvidPlayer );
	HRESULT RemovePlayerFromGroup( DVID dvidGroup, DVID dvidPlayer );

	inline DWORD GetCurrentState() { return m_dwCurrentState; };	

protected:

    HRESULT Send_SessionLost( HRESULT hrReason );
    HRESULT Send_HostMigrateLeave( );
    HRESULT Send_HostMigrated();
    HRESULT Send_DisconnectConfirm( DVID dvid, HRESULT hrReason );
#ifdef DVS_ENABLE_PEER_SESSION
    HRESULT Send_DeletePlayer( DVID dvid );
    HRESULT Send_CreatePlayer( DVID dvidTarget, CVoicePlayer *pPlayer );
#endif
    HRESULT Send_ConnectRefuse( DVID dvid, HRESULT hrReason );
    HRESULT Send_ConnectAccept( DVID dvid );

protected:

	BOOL CheckProtocolCompatible( BYTE ucMajor, BYTE ucMinor, DWORD dwBuild );

#ifdef DVS_ENABLE_PEER_SESSION
	HRESULT SendPlayerList( DVID dvidSource, DWORD dwHostOrderID );
#endif

	HRESULT InternalSetNotifyMask( LPDWORD lpdwMessages, DWORD dwNumElements );	

	void DoPlayerDisconnect( DVID dvidPlayer, BOOL bInformPlayer );
	void TransmitMessage( DWORD dwMessageType, LPVOID lpdvData, DWORD dwSize );
	void SetCurrentState( DWORD dwState );
	HRESULT CreatePlayerEntry( DVID dvidSource, PDVPROTOCOLMSG_SETTINGSCONFIRM lpdvSettingsConfirm, DWORD dwHostOrderID, CVoicePlayer **ppPlayer );

	BOOL HandleDisconnect( DVID dvidSource, PDVPROTOCOLMSG_DISCONNECT lpdvDisconnect, DWORD dwSize );
	BOOL HandleConnectRequest( DVID dvidSource, PDVPROTOCOLMSG_CONNECTREQUEST lpdvConnectRequest, DWORD dwSize );
	BOOL HandleSettingsConfirm( DVID dvidSource, PDVPROTOCOLMSG_SETTINGSCONFIRM lpdvSettingsConfirm, DWORD dwSize );
	BOOL HandleSettingsReject( DVID dvidSource, PDVPROTOCOLMSG_GENERIC lpdvGeneric, DWORD dwSize );

#ifdef DVS_ENABLE_MULTICAST_SESSION
	BOOL HandleSpeechWithTarget( DVID dvidSource, PDVPROTOCOLMSG_SPEECHWITHTARGET lpdvSpeech, DWORD dwSize );
#endif
	BOOL HandleSpeech( DVID dvidSource, PDVPROTOCOLMSG_SPEECHHEADER lpdvSpeech, DWORD dwSize );	

	PDVTRANSPORT_BUFFERDESC GetTransmitBuffer( DWORD dwSize, LPVOID *ppvContext, DWORD PoolIndex );
    HRESULT SendComplete( PDVEVENTMSG_SENDCOMPLETE pSendComplete );
    void ReturnTransmitBuffer( PVOID pvContext );

	HRESULT BuildAndSendTargetUpdate( DVID dvidSource, CVoicePlayer *pPlayerInfo );

	BOOL CheckForMigrate( DWORD dwFlags, BOOL fSilent );
	HRESULT InformClientsOfMigrate();
	void WaitForBufferReturns();

	DWORD					m_dwSignature;			// Signature 

	LPDVMESSAGEHANDLER		m_lpMessageHandler;		// User message handler
	LPVOID					m_lpUserContext;		// User context for message handler
	DVID					m_dvidLocal;			// DVID of the transport player for this host
	DWORD					m_dwCurrentState;		// Current state of the engine
    CDirectVoiceTransport	*m_lpSessionTransport;	// Transport for the session
	DVSESSIONDESC			m_dvSessionDesc;		// Description of session
	DWORD					m_dwTransportFlags;		// Flags for the transport session
	DWORD					m_dwTransportSessionType;
													// Type of transport session (client/server or peer to peer)
protected: // Client Server specific information

	HRESULT SetupBuffers();
	HRESULT FreeBuffers();

	HRESULT StartupMulticast();
	HRESULT ShutdownMulticast();
    void CleanupActiveList();

protected:
    
    CRITICAL_SECTION         m_CS;
	DIRECTVOICESERVEROBJECT *m_lpObject;			// Pointer to the COM object this is running in 

	LPDWORD					m_lpdwMessageElements;	// Array containing the DVMSGID_XXXX values for all the
													// notifications developer wishes to receive.
													// If this is NULL all notifications are active
	DWORD					m_dwNumMessageElements;	// # of elements in the m_lpdwMessageElements array
	DWORD					m_dwNextHostOrderID;
	HRESULT					m_hrStopSessionResult;	// Reason that the session was stopped

	BYTE					m_padding[3];    

	LIST_ENTRY				m_PlayerActiveList;

    CVoiceNameTable         m_voiceNameTable;

    LIST_ENTRY   m_PlayerContextPool;
    LIST_ENTRY   m_MessagePool;
    LIST_ENTRY   m_PlayerListPool;
    LIST_ENTRY   m_SpeechBufferPool;

    LIST_ENTRY   m_BufferDescPool;

    PVOID                     m_pBufferDescAllocation;
    PVOID                     m_pMessagePoolAllocation;
    PVOID                     m_pSpeechBufferAllocation;
    PVOID                     m_pPlayerListPoolAllocation;


    DWORD        m_MsgBufferAllocs;
    DWORD        m_SpeechBufferAllocs;

public:
    __inline void *__cdecl operator new(size_t size)
    {
        return ExAllocatePoolWithTag(size, 'esvd');
    }

    __inline void __cdecl operator delete(void *pv)
    {
        ExFreePool(pv);
    }

};


#endif
