/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvdxtran.h
 *  Content:	Definition of transport class providing DirectXVoice transport
 *              through the IDirectXVoiceTransport interface.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/23/99		rodtoll	Modified from dvdptran.h
 * 09/01/2000   georgioc started rewrite/port to xbox
 ***************************************************************************/
#ifndef __DVDPTRANSPORT_H
#define __DVDPTRANSPORT_H

#include "dvntos.h"
#include "dvtran.h"

// CDirectVoiceDirectXTransport
//
// Implements the transport system using the IDirectXVoiceTransport
// interface that will be implemented by both DirectPlay and 
// DirectNet.
//
// This class handles the interaction between an DirectPlayVoice engine
// and the transport level.
// 
volatile class CDirectVoiceDirectXTransport : public CDirectVoiceTransport
{
public:
	CDirectVoiceDirectXTransport( LPDIRECTPLAYVOICETRANSPORT lpTransport );
	~CDirectVoiceDirectXTransport();

	HRESULT Initialize();

public:
	HRESULT AddPlayerEntry( DVID dvidPlayer, LPVOID lpData );
	HRESULT DeletePlayerEntry( DVID dvidPlayer );
	HRESULT GetPlayerEntry( DVID dvidPlayer, CVoicePlayer **lplpPlayer );
	HRESULT MigrateHost( DVID dvidNewHost );	

	DWORD GetMaxPlayers( );

	inline LPDIRECTPLAYVOICETRANSPORT GetTransportInterface( ) { return m_lpTransport; };

    DVID GetLocalID();
	inline DVID GetServerID() { return m_dpidServer; };

public:
	HRESULT SendToServer( PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags );
	HRESULT SendToAll( PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags );
	HRESULT SendToIDS( PDVID pdvidTargets, DWORD dwNumTargets, PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags );

public: // Remote Server Synchronization functions
	HRESULT CreateGroup( LPDVID dvidGroup );
	HRESULT DeleteGroup( DVID dvidGroup );
	HRESULT AddPlayerToGroup( LPDVID dvidGroup, DVID dvidPlayer ); 
	HRESULT RemovePlayerFromGroup( DVID dvidGroup, DVID dvidPlayer );
	BOOL ConfirmValidEntity( DVID dvid );
	BOOL ConfirmValidGroup( DVID dvid );
	BOOL ConfirmLocalHost( );
	BOOL ConfirmSessionActive();
    VOID DoWork();
	BOOL IsPlayerInGroup( DVID dvidGroup, DVID dvidPlayer );
	HRESULT GetTransportSettings( LPDWORD lpdwSessionType, LPDWORD lpdwFlags );

	HRESULT EnableReceiveHook( LPDIRECTVOICEOBJECT dvObject, DWORD dwObjectType );
	HRESULT DisableReceiveHook(  );
	HRESULT WaitForDetachCompletion( );	
	void DestroyTransport( );

	// Debug / Test Only
	//HRESULT SetInfo( DPID dpidServer, DPID dpidClient );

protected:
	HRESULT SendHelper( PDVID pdvidTargets, DWORD dwNumTargets, PDVTRANSPORT_BUFFERDESC pBufferDesc, PVOID pvContext, DWORD dwFlags );

protected:
	LPDIRECTPLAYVOICETRANSPORT m_lpTransport;		// Transport interface 
	DPNID m_dpidServer;								// DPID of the session host
	BOOL m_bLocalServer;							// Is the host on same interface as the client
	BOOL m_bActiveSession;							// Is there a session active on the transport
	DWORD m_dwTransportFlags;						// Flags describing the session
	CDirectVoiceEngine *m_lpVoiceEngine;			// Engine this transport is working for
	DWORD m_dwMaxPlayers;							// Maximum # of players this session can have
	BOOL  m_initialized;							// Has this object been initialized?
			
	DVTRANSPORTINFO m_dvTransportInfo;				// Information about the transport
	DPNID m_dpidLocalPlayer;							// DPID of the local client	
	DWORD m_dwDuumy;
	DWORD m_dwObjectType;

	BOOL	m_fAdvised;

	LIST_ENTRY m_PlayerList;
};

#endif
