/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvdxtran.cpp
 *  Content:	Implementation of transport class providing DirectXVoice transport
 *              through the IDirectXVoiceTransport interface.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/23/99		rodtoll	Modified from dvdptran.cpp
 * 08/03/99		rodtoll	Modified to conform to new base class and minor fixes
 *						for dplay integration. 
 * 08/04/99		rodtoll	Modified to allow group targets
 * 08/10/99		rodtoll	Removed TODO pragmas
 * 08/25/99		rodtoll	Fixed group membership check
 * 08/30/99		rodtoll	Modified SendToServer to send to the server player in 
 *						client/server sessions.
 * 08/31/99		rodtoll	Updated to use new debug libs
 * 09/01/99		rodtoll	Updated so that constructor no longer calls into dplay
 * 				rodtoll	Added check for valid pointers in func calls  
 * 09/02/99		rodtoll	Added checks to handle case no local player created 
 * 09/20/99		rodtoll	Added memory alloc failure checks 
 * 09/21/99		rodtoll	Fixed memory leak
 * 10/05/99		rodtoll	Additional comments and DPF_MODNAMEs
 * 11/23/99		rodtoll	Split CheckForValid into Group and Player
 * 12/16/99		rodtoll Bug #122629 - As part of new host migration update how
 *						sends to server before first response are sent.
 * 01/14/2000	rodtoll	Renamed SendToID to SendToIDS and updated parameter list
 *						to accept multiple targets.
 *				rodtoll	Added GetNumPlayers call
 * 01/17/2000	rodtoll	Debug statement removed that limited max players to 30
 * 03/28/2000   rodtoll Moved nametable from here to upper level classes
 *              rodtoll Removed uneeded functions/members 
 * 04/07/2000   rodtoll Updated to support new DP <--> DPV interface
 *              rodtoll Updated to support no copy sends
 *              rodtoll Bug #32179 - Prevent multiple client/server registrations on transport 
 * 06/21/2000	rodtoll Bug #36820 - Host migrates to wrong client when client/server are on same interface
 *						Condition exists where host sends leave message, client attempts to start new host
 *						which fails because old host still registered.  Now deregistering is two step
 *						process DisableReceiveHook then DestroyTransport.  
 * 07/22/20000	rodtoll Bug #40296, 38858 - Crashes due to shutdown race condition
 *   				  Now ensures that all threads from transport have left and that
 *					  all notificatinos have been processed before shutdown is complete.  
 *						
 ***************************************************************************/

#include "dvntos.h"
#include "in_core.h"
#include "dvdxtran.h"


//#pragma warning( disable : 4786 )  



#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::CDirectVoiceDirectXTransport"
CDirectVoiceDirectXTransport::CDirectVoiceDirectXTransport( LPDIRECTPLAYVOICETRANSPORT lpTransport
	):  m_lpTransport(NULL),
		m_dpidServer(DPNID_ALL_PLAYERS_GROUP),
		m_dpidLocalPlayer(0),
		m_bLocalServer(TRUE),
		m_bActiveSession(TRUE),
		m_dwTransportFlags(0),
		m_lpVoiceEngine(NULL),
		m_dwMaxPlayers(0),
		m_initialized(FALSE),
		m_dwObjectType(0),
		m_fAdvised(FALSE)
{

    lpTransport->AddRef();
    m_lpTransport = lpTransport;

	m_dvTransportInfo.dwSize = sizeof( DVTRANSPORTINFO );
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::~CDirectVoiceDirectXTransport"
CDirectVoiceDirectXTransport::~CDirectVoiceDirectXTransport()
{
	if( m_lpTransport != NULL )
		m_lpTransport->Release();
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::DestroyTransport"
// DestroyTransport
//
// This method is used to remove last references to transport that transport 
// layer has.  There was a memory leak where 
//
// 
void CDirectVoiceDirectXTransport::DestroyTransport()
{

	if( m_lpTransport != NULL )
	{
		m_lpTransport->Release();
		m_lpTransport = NULL;
	}
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::Initialize"
//
// Initialize
//
// Called from the transport when Advise is called.
//
// Used to initialize this object.
//
HRESULT CDirectVoiceDirectXTransport::Initialize(  )
{
	HRESULT hr = DV_OK;
	hr = m_lpTransport->GetSessionInfo( &m_dvTransportInfo );
	if( FAILED( hr ) )
	{
		DPVF( 0, "DXVT::Initialize: GetSessionInfo() failed!  hr=0x%x", hr );
		return hr;
	}

	m_dwMaxPlayers = (m_dvTransportInfo.dwMaxPlayers==0) ? DV_MAX_REMOTE_PLAYERS : m_dvTransportInfo.dwMaxPlayers;
    ASSERT(m_dwMaxPlayers <= DV_MAX_REMOTE_PLAYERS);

	m_dpidLocalPlayer = m_dvTransportInfo.dvidLocalID;

//  No longer needed, the server may not bee the host of the dplay session
//	m_dpidServer = m_dvTransportInfo.dvidSessionHost;
	m_dpidServer = DPNID_ALL_PLAYERS_GROUP;

	m_initialized = TRUE;
	
    //
    // add one ref count on our selfs.. DisableReceivHook will release it
    //

    AddRef();


	return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::GetMaxPlayers"
DWORD CDirectVoiceDirectXTransport::GetMaxPlayers( )
{
	return m_dwMaxPlayers;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::SendHelper"
HRESULT CDirectVoiceDirectXTransport::SendHelper( PDVID pdpidTargets, DWORD dwNumTargets, PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags )
{
	HRESULT hr=DV_OK;
    PAGED_CODE();

    if( dwNumTargets > 1 )
    {
#if DBG
        for( DWORD dwIndex = 0; dwIndex < dwNumTargets; dwIndex++ ) { 
            DPVF( DPVF_SEND_DEBUG_LEVEL, "Using multitargetted send [From=0x%x To=0x%x]", m_dpidLocalPlayer, pdpidTargets[dwIndex] );	
        }
#endif
        hr = m_lpTransport->SendSpeechEx( m_dpidLocalPlayer, dwNumTargets, pdpidTargets, pBufferDesc, pvContext, dwFlags );		
    }
    else
    {

        DPVF( DPVF_SEND_DEBUG_LEVEL, "Single target for send [From=0x%x To=0x%x]", m_dpidLocalPlayer, pdpidTargets[0] );
        hr = m_lpTransport->SendSpeech( m_dpidLocalPlayer, *pdpidTargets, pBufferDesc, pvContext, dwFlags );
    }

	return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::SendToServer"
HRESULT CDirectVoiceDirectXTransport::SendToServer( PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags )
{
    PAGED_CODE();
	if( m_dvTransportInfo.dwSessionType == DVTRANSPORT_SESSION_CLIENTSERVER )
	{
		DVID dvidTmp = DVID_SERVERPLAYER;

		DPVF( DPVF_SEND_DEBUG_LEVEL, "Sending to standard server player" );
		return SendHelper( &dvidTmp, 1, pBufferDesc, pvContext, dwFlags );
	}
	else
	{
		DPVF( DPVF_SEND_DEBUG_LEVEL, "Sending to server ID [ID=0x%x]", m_dpidServer );
		return SendHelper( &m_dpidServer, 1, pBufferDesc, pvContext, dwFlags );
	}
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::SendToIDS"
HRESULT CDirectVoiceDirectXTransport::SendToIDS( PDVID pdvidTargets, DWORD dwNumTargets, PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags )
{
	return SendHelper( pdvidTargets, dwNumTargets, pBufferDesc, pvContext, dwFlags );
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::SendToAll"
HRESULT CDirectVoiceDirectXTransport::SendToAll( PDVTRANSPORT_BUFFERDESC pBufferDesc, LPVOID pvContext, DWORD dwFlags )
{
	DVID dvidTmp = DPNID_ALL_PLAYERS_GROUP;
	return SendHelper( &dvidTmp, 1, pBufferDesc, pvContext, dwFlags );
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::ConfirmValidGroup"
BOOL CDirectVoiceDirectXTransport::ConfirmValidGroup( DVID dvid )
{
    PAGED_CODE();
	if( dvid == DVID_ALLPLAYERS )
	{
		return TRUE;
	}
	else if( m_dvTransportInfo.dwSessionType == DVTRANSPORT_SESSION_CLIENTSERVER )
	{
		return TRUE;
	}
	else 
	{
		BOOL fResult;
		HRESULT hr;
		hr = m_lpTransport->IsValidGroup( dvid, &fResult );

		if( FAILED( hr ) )
		{
			DPVF( DPVF_ERRORLEVEL, "Error confirming valid group hr=0x%x", hr );
			return FALSE;
		}
		else
		{
			return fResult;
		}
	}	

	return FALSE;
	
}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::DoWork"
VOID CDirectVoiceDirectXTransport::DoWork( )
{

    m_lpTransport->DoWork(0);

}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::ConfirmValidEntity"
//
// ConfirmValidEntity
//
// Checks to ensure that the ID passed is a valid one for the session.
//
// Will return TRUE if the player iD is one of:
// DVID_ALLPLAYERS, DVID_NOTARGET, (any value in client/server mode),
// a player in the map, or a valid Transport group.
//
BOOL CDirectVoiceDirectXTransport::ConfirmValidEntity( DVID dvid )
{
    PAGED_CODE();
	if( m_dvTransportInfo.dwSessionType == DVTRANSPORT_SESSION_CLIENTSERVER )
	{
		return TRUE;
	}
	else 
	{
		BOOL fResult;
		m_lpTransport->IsValidEntity( dvid, &fResult );
		return fResult;

	}

	return FALSE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::EnableReceiveHook"
//
// EnableReceiveHook
//
// This is used to activate the connection between the transport
// and the transport class.
//
// We call advise, which will cause Initialize to be called on this
// class before we return from Advise.
//
HRESULT CDirectVoiceDirectXTransport::EnableReceiveHook( LPDIRECTVOICEOBJECT dvObject, DWORD dwObjectType )
{
	HRESULT hr;
	
    PAGED_CODE();
	m_lpVoiceEngine = dvObject->lpDVEngine;
	m_dwObjectType = dwObjectType;

	// The transport will call Initialize on our notification interface
	// before returning from this function.
	//
	// Once we've returned from this function we should be ok.
	//// BUGBUG: Should not just cast this, should do a QueryInterface
	hr = m_lpTransport->Advise( (LPUNKNOWN) &dvObject->dvNotify, m_dwObjectType );	

	if( FAILED( hr ) )
	{
		DPVF( DPVF_ERRORLEVEL, "Advise failed.  hr=0x%x", hr );
		m_fAdvised = FALSE;
	}
	else
	{
		m_fAdvised = TRUE;
	}

	return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::WaitForDetachCompletion"
HRESULT CDirectVoiceDirectXTransport::WaitForDetachCompletion()
{
    ULONG cnt = 0;
    DPVF_ENTER();
    PAGED_CODE();
	DPVF( DPVF_WARNINGLEVEL, "# of threads remaining: %d", m_lRefCount );

	// Loop until all threads are done inside our layer
	while( m_lRefCount > 0 ){	

        Sleep( 5 );	
        cnt++;

        //
        // call dplay do work..
        //

        m_lpTransport->DoWork(0);

        //
        // bugbug we cant really wait for threads here since the DplayDoWork
        // is not being called while we are in here...
        //

        if (cnt > 200) {
            break;
        }
    }
		

    DPVF( DPVF_INFOLEVEL, "threads gone");
	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::DisableReceiveHook"
//
// DisableReceiveHook
//
// Removes the hooks into the transport and releases the interface
// reference this object holds for the transport.
//
// Also responsible for destroying the list of players maintained
// by this object.
//
HRESULT CDirectVoiceDirectXTransport::DisableReceiveHook( )
{
    PAGED_CODE();
	if( m_fAdvised )
	{
		m_lpTransport->UnAdvise( m_dwObjectType );

		m_fAdvised = FALSE;
		m_initialized = FALSE;		

        Release();

		DPVF( DPVF_SPAMLEVEL, "Unhooking Transport" );
	}

	// When this is done no more indications will be waiting.  	

	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::ConfirmLocalHost"
BOOL CDirectVoiceDirectXTransport::ConfirmLocalHost( )
{
    HRESULT hr;
    PAGED_CODE();

	if( !m_initialized ) {

        hr = Initialize();

        if (FAILED(hr)) {
            ASSERT(FALSE);
            return FALSE;
        }

    }
		
	
	if( m_dvTransportInfo.dwFlags & DVTRANSPORT_LOCALHOST )
		return TRUE;
	else
		return FALSE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::ConfirmSessionActive"
BOOL CDirectVoiceDirectXTransport::ConfirmSessionActive( )
{
	return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::GetTransportSettings"
HRESULT CDirectVoiceDirectXTransport::GetTransportSettings( LPDWORD lpdwSessionType, LPDWORD lpdwFlags )
{
	HRESULT hr = DV_OK;
	
    PAGED_CODE();
	if( !m_initialized )
		hr = Initialize();

	if( FAILED( hr ) )
	{
		DPVF( DPVF_ERRORLEVEL, "Unable to retrieve transport settings" );
		return hr;
	}

	*lpdwSessionType = m_dvTransportInfo.dwSessionType;
	*lpdwFlags = m_dvTransportInfo.dwFlags;

	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::AddPlayerEntry"
HRESULT CDirectVoiceDirectXTransport::AddPlayerEntry( DVID dvidPlayer, LPVOID lpData )
{
	return DVERR_NOTSUPPORTED;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::DeletePlayerEntry"
HRESULT CDirectVoiceDirectXTransport::DeletePlayerEntry( DVID dvidPlayer )
{
	return DVERR_NOTSUPPORTED;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::GetPlayerEntry"
//
// GetPlayerEntry
//
// Retrieves the player record for the specified player (if it exists).
//
HRESULT CDirectVoiceDirectXTransport::GetPlayerEntry( DVID dvidPlayer,  CVoicePlayer **lplpPlayer )
{
	return DVERR_NOTSUPPORTED;
}


////////////////////////////////////////////////////////////////////////
//
// USEFUL FOR REMOTE VOICE SESSIONS
//

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::CreateGroup"
HRESULT CDirectVoiceDirectXTransport::CreateGroup( LPDVID dvidGroup )
{
	return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::DeleteGroup"
HRESULT CDirectVoiceDirectXTransport::DeleteGroup( DVID dvidGroup )
{
	return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::AddPlayerToGroup"
HRESULT CDirectVoiceDirectXTransport::AddPlayerToGroup( LPDVID dvidGroup, DVID dvidPlayer )
{
	return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::RemovePlayerFromGroup"
HRESULT CDirectVoiceDirectXTransport::RemovePlayerFromGroup( DVID dvidGroup, DVID dvidPlayer )
{
	return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::IsPlayerInGroup"
BOOL CDirectVoiceDirectXTransport::IsPlayerInGroup( DVID dvidGroup, DVID dvidPlayer )
{
    PAGED_CODE();
	if( dvidGroup == DVID_ALLPLAYERS )
	{
		return TRUE;
	}

	if( dvidGroup == dvidPlayer )
	{
		return TRUE;
	}
	return (m_lpTransport->IsGroupMember( dvidGroup, dvidPlayer )==DV_OK);
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::MigrateHost"
//
// MigrateHost
//
// Updates server DPID to match new host
//
HRESULT CDirectVoiceDirectXTransport::MigrateHost( DVID dvidNewHost )
{
	m_dpidServer = dvidNewHost;

	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceDirectXTransport::MigrateHost"
DVID CDirectVoiceDirectXTransport::GetLocalID() 
{ 
    m_dwDuumy = m_dpidLocalPlayer;
    return m_dpidLocalPlayer; 
}


