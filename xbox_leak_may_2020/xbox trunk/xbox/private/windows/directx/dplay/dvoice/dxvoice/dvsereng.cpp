/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dvserverengine.cpp
 *  Content:    Implements the CDirectVoiceServerEngine class.
 *
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/18/99    rodtoll Created It
 *  09/01/2000  georgioc rewrote/ported to xbox
 ***************************************************************************/
#include "dvsereng.h"
#include "dvntos.h"
#include "dvshared.h"
#include "dvserver.h"
#include "in_core.h"

#define SERVER_POOL_MESSAGE      1
#define SERVER_POOL_PLAYERLIST   2
#define SERVER_POOL_SPEECH       3

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::CDirectVoiceServerEngine"
// 
// Constructor
// 
// Initializes the object into the uninitialized state
//
CDirectVoiceServerEngine::CDirectVoiceServerEngine( DIRECTVOICESERVEROBJECT *lpObject
):  m_dwSignature(VSIG_SERVERENGINE), 
    m_lpMessageHandler(NULL),
    m_lpUserContext(NULL),
    m_lpObject(lpObject),
    m_dvidLocal(0),
    m_dwCurrentState(DVSSTATE_NOTINITIALIZED),
    m_lpSessionTransport(NULL),
    m_lpdwMessageElements(NULL),
    m_dwNumMessageElements(0),
    m_dwNextHostOrderID(0)
{

    InitializeCriticalSection(&m_CS);
    m_SpeechBufferAllocs = 0;
    m_MsgBufferAllocs = 0;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::~CDirectVoiceServerEngine"
//
// Destructor
//
// Frees the resources associated with the server.  Shuts the server down
// if it is running.
//
// Server objects should never be destructed directly, the COM Release
// method should be used.
//
// Called By:
// DVS_Release (When reference count reaches 0)
//
// Locks Required:
// - Global Write Lock
//
CDirectVoiceServerEngine::~CDirectVoiceServerEngine()
{
    HRESULT hr;

    DV_AUTO_LOCK(&m_CS);    

    DPVF_ENTER();

    // Stop the session if it's running.
    hr = StopSession(0, FALSE , DV_OK );

    if( hr != DV_OK && hr != DVERR_NOTHOSTING )
    {
        DPVF( DPVF_ERRORLEVEL, "StopSession Failed hr=0x%x", hr );
    }

    if( m_lpdwMessageElements != NULL )
        delete [] m_lpdwMessageElements;

    m_dwSignature = VSIG_SERVERENGINE_FREE;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::TransmitMessage"
// 
// TransmitMessage
//
// This function sends a notification to the notification handler.  Before transmitting
// it, the notify elements are checked to ensure the specified notification is activated.
// (or notification array is NULL).
//
// Called By:
// - Multiple locations throughout dvsereng.cpp
// 
// Locks Required:
// - m_csNotifyLock - The notification lock
// 
void CDirectVoiceServerEngine::TransmitMessage( DWORD dwMessageType, LPVOID lpdvData, DWORD dwSize )
{
    DPVF_ENTER();

    DV_AUTO_LOCK(&m_CS);
    if( m_lpMessageHandler != NULL )
    {
        BOOL fSend = FALSE;     

        if( m_dwNumMessageElements == 0 )
        {
            fSend = TRUE;
        }
        else
        {
            for( DWORD dwIndex = 0; dwIndex < m_dwNumMessageElements; dwIndex++ )
            {
                if( m_lpdwMessageElements[dwIndex] == dwMessageType )
                {
                    fSend = TRUE;
                    break;
                }
            }
        }

        if( fSend )
        {
            (*m_lpMessageHandler)( m_lpUserContext, dwMessageType,lpdvData );               
        }


    }

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::StartupMulticast"
//
// StartupMulticast
//
// This function is called to initialize the multicast portion of the server
// object.  
//
// Called By:
// - StartSession
//
// Locks Required:
// - None
//
HRESULT CDirectVoiceServerEngine::StartupMulticast()
{
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::ShutdownMulticast"
//
// ShutdownMulticast
//
// This function is called to shutdown the multicast portion of the server
// object.  
//
// Called By:
// - StartSession
//
// Locks Required:
// - None
//
HRESULT CDirectVoiceServerEngine::ShutdownMulticast()
{

    return DV_OK;
}


// Handles initializing a server object for host migration
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::HostMigrateStart"
//
// HostMigrateStart
//
// This function is called on the object which is to become the new host when 
// the host migrates.  It is used instead of the traditional startup to 
// ensure that the object is initialized correctly in the migration case.
//
// Called By:
// - DV_HostMigrate
//
// Locks Required:
// - None
// 
HRESULT CDirectVoiceServerEngine::HostMigrateStart(LPDVSESSIONDESC lpSessionDesc, DWORD dwHostOrderIDSeed )
{
    HRESULT hr;
    
    DPVF_ENTER();

    DPVF(  DPVF_ENTRYLEVEL, "DVSE::HostMigrateStart() Begin" );

    // Start 
    hr = StartSession( lpSessionDesc, 0, dwHostOrderIDSeed );

    // Fail
    if( hr != DV_OK ) 
    {
        DPVF( DPVF_ERRORLEVEL, "CDirectVoiceServerEngine::HostMigrateStart Failed Hr=0x%x", hr );
        return hr;
    }

    hr = InformClientsOfMigrate();

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Unable to inform users of host migration hr=0x%x", hr );
    }

    return hr;
}

#ifdef DVS_ENABLE_PEER_SESSION
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Send_CreatePlayer"
HRESULT CDirectVoiceServerEngine::Send_CreatePlayer( DVID dvidTarget, CVoicePlayer *pPlayer )
{
    PDVPROTOCOLMSG_PLAYERJOIN pdvPlayerJoin;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PVOID pvSendContext;
    HRESULT hr;
                          
    DPVF_ENTER();

    pBufferDesc = GetTransmitBuffer( sizeof( DVPROTOCOLMSG_PLAYERJOIN ), &pvSendContext,SERVER_POOL_MESSAGE );

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc error" );
        return DVERR_OUTOFMEMORY;
    }

    pdvPlayerJoin = (PDVPROTOCOLMSG_PLAYERJOIN) pBufferDesc->pBufferData;

    pdvPlayerJoin->dwType = DVMSGID_CREATEVOICEPLAYER;
    pdvPlayerJoin->dvidID = pPlayer->GetPlayerID();
    pdvPlayerJoin->dwFlags = pPlayer->GetTransportFlags();
    pdvPlayerJoin->dwHostOrderID = pPlayer->GetHostOrder();

    hr = m_lpSessionTransport->SendToIDS( &dvidTarget, 1, pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending create player to 0x%x hr=0x%x", dvidTarget, hr );
        ReturnTransmitBuffer( pvSendContext );        
    }

    return hr;    
}
#endif

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Send_ConnectRefuse"
HRESULT CDirectVoiceServerEngine::Send_ConnectRefuse( DVID dvidID, HRESULT hrReason )
{
    PDVPROTOCOLMSG_CONNECTREFUSE pdvConnectRefuse;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();

    pBufferDesc = GetTransmitBuffer( sizeof( DVPROTOCOLMSG_CONNECTREFUSE ), &pvSendContext, SERVER_POOL_MESSAGE);

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc error" );
        return DVERR_OUTOFMEMORY;
    }

    pdvConnectRefuse = (PDVPROTOCOLMSG_CONNECTREFUSE) pBufferDesc->pBufferData;

    pdvConnectRefuse->dwType = DVMSGID_CONNECTREFUSE;
    pdvConnectRefuse->hresResult = hrReason;
    pdvConnectRefuse->ucVersionMajor = DVPROTOCOL_VERSION_MAJOR;
    pdvConnectRefuse->ucVersionMinor = DVPROTOCOL_VERSION_MINOR;
    pdvConnectRefuse->dwVersionBuild = DVPROTOCOL_VERSION_BUILD;    

    hr = m_lpSessionTransport->SendToIDS( &dvidID, 1, pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending disconnect confirm migrated to all hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );        
    }

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Send_DisconnectConfirm"
HRESULT CDirectVoiceServerEngine::Send_DisconnectConfirm( DVID dvidID, HRESULT hrReason )
{
    PDVPROTOCOLMSG_DISCONNECT pdvDisconnectConfirm;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();

    pBufferDesc = GetTransmitBuffer( sizeof( DVPROTOCOLMSG_DISCONNECT ), &pvSendContext, SERVER_POOL_MESSAGE );

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc error" );
        return DVERR_OUTOFMEMORY;
    }

    pdvDisconnectConfirm = (PDVPROTOCOLMSG_DISCONNECT) pBufferDesc->pBufferData;

    pdvDisconnectConfirm->dwType = DVMSGID_DISCONNECTCONFIRM;
    pdvDisconnectConfirm->hresDisconnect = hrReason;

    hr = m_lpSessionTransport->SendToIDS( &dvidID, 1, pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending disconnect confirm migrated to all hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );        
    }

    return hr;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Send_ConnectAccept"
HRESULT CDirectVoiceServerEngine::Send_ConnectAccept( DVID dvidID )
{
    PDVPROTOCOLMSG_CONNECTACCEPT pdvConnectAccept;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();

    pBufferDesc = GetTransmitBuffer( sizeof( DVPROTOCOLMSG_CONNECTACCEPT ), &pvSendContext, SERVER_POOL_MESSAGE );

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc error" );
        return DVERR_OUTOFMEMORY;
    }

    pdvConnectAccept = (PDVPROTOCOLMSG_CONNECTACCEPT) pBufferDesc->pBufferData;

    pdvConnectAccept->dwType = DVMSGID_CONNECTACCEPT;
    pdvConnectAccept->dwSessionFlags = m_dvSessionDesc.dwFlags;
    pdvConnectAccept->dwSessionType = m_dvSessionDesc.dwSessionType;
    pdvConnectAccept->ucVersionMajor = DVPROTOCOL_VERSION_MAJOR;
    pdvConnectAccept->ucVersionMinor = DVPROTOCOL_VERSION_MINOR;
    pdvConnectAccept->dwVersionBuild = DVPROTOCOL_VERSION_BUILD;
    pdvConnectAccept->guidCT = m_dvSessionDesc.guidCT;

    hr = m_lpSessionTransport->SendToIDS( &dvidID, 1, pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending connect accept to player hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );        
    }

    return hr;
}


#ifdef DVS_ENABLE_PEER_SESSION
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Send_DeletePlayer"
HRESULT CDirectVoiceServerEngine::Send_DeletePlayer( DVID dvidID )
{
    PDVPROTOCOLMSG_PLAYERQUIT pdvPlayerQuit;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();

    pBufferDesc = GetTransmitBuffer( sizeof( DVPROTOCOLMSG_PLAYERQUIT ), &pvSendContext, SERVER_POOL_MESSAGE);

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc error" );
        return DVERR_OUTOFMEMORY;
    }

    pdvPlayerQuit = (PDVPROTOCOLMSG_PLAYERQUIT) pBufferDesc->pBufferData;

    pdvPlayerQuit->dwType = DVMSGID_DELETEVOICEPLAYER;
    pdvPlayerQuit->dvidID = dvidID;

    hr = m_lpSessionTransport->SendToAll( pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending delete player migrated to all hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );        
    }

    return hr;
}
#endif
        
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Send_SessionLost"
HRESULT CDirectVoiceServerEngine::Send_SessionLost( HRESULT hrReason )
{
    PDVPROTOCOLMSG_SESSIONLOST pdvSessionLost;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();

    pBufferDesc = GetTransmitBuffer( sizeof( DVPROTOCOLMSG_SESSIONLOST ), &pvSendContext, SERVER_POOL_MESSAGE);

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc error" );
        return DVERR_OUTOFMEMORY;
    }

    pdvSessionLost = (PDVPROTOCOLMSG_SESSIONLOST) pBufferDesc->pBufferData;

    pdvSessionLost->dwType = DVMSGID_SESSIONLOST;
    pdvSessionLost->hresReason = hrReason;

    hr = m_lpSessionTransport->SendToAll( pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending host migrated to all hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );        
    }

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Send_HostMigrateLeave"
HRESULT CDirectVoiceServerEngine::Send_HostMigrateLeave( )
{
    PDVPROTOCOLMSG_HOSTMIGRATELEAVE pdvHostMigrateLeave;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();

    pBufferDesc = GetTransmitBuffer( sizeof( DVPROTOCOLMSG_HOSTMIGRATELEAVE ), &pvSendContext, SERVER_POOL_MESSAGE);

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc error" );
        return DVERR_OUTOFMEMORY;
    }

    pdvHostMigrateLeave = (PDVPROTOCOLMSG_HOSTMIGRATELEAVE) pBufferDesc->pBufferData;

    pdvHostMigrateLeave->dwType = DVMSGID_HOSTMIGRATELEAVE;

    // Send this message with sync.  Sync messages do not generate callbacks
    //
    hr = m_lpSessionTransport->SendToAll( pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED | DVTRANSPORT_SEND_SYNC );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending host migrated to all hr=0x%x", hr );
    }

    ReturnTransmitBuffer( pvSendContext );        

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Send_HostMigrated"
HRESULT CDirectVoiceServerEngine::Send_HostMigrated()
{
    PDVPROTOCOLMSG_HOSTMIGRATED pvMigrated;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();

    pBufferDesc = GetTransmitBuffer( sizeof( DVPROTOCOLMSG_HOSTMIGRATED ), &pvSendContext, SERVER_POOL_MESSAGE );

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc error" );
        return DVERR_OUTOFMEMORY;
    }

    pvMigrated = (PDVPROTOCOLMSG_HOSTMIGRATED) pBufferDesc->pBufferData;

    pvMigrated->dwType = DVMSGID_HOSTMIGRATED;

    hr = m_lpSessionTransport->SendToAll( pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending host migrated to all hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );        
    }

    return hr;
}

#undef DPF_MODNAME 
#define DPF_MODNAME "CDirectVoiceServerEngine::InformClientsOfMigrate"
//
//
// This function will 
//  
HRESULT CDirectVoiceServerEngine::InformClientsOfMigrate()
{
    DPVF( DPVF_ERRORLEVEL, "Informing clients of migration" );

    return Send_HostMigrated();
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::StartSession"
//
// StartSession
//
// This function handles starting the directplayvoice session in the associated directplay/net
// session.  It also handles startup for a new host when the host migrates.  (Called by 
// HostMigrateStart in this case).
//
// Called By:
// - DV_StartSession
// - HostMigrateStart
//
// Locks Required:
// - Global Write Lock
//
HRESULT CDirectVoiceServerEngine::StartSession(LPDVSESSIONDESC lpSessionDesc, DWORD dwFlags, DWORD dwHostOrderIDSeed )
{
    HRESULT hr;
    XMediaObject *pConverter;
    XMEDIAINFO xmi;
    WAVEFORMATEX wfx;

    DPVF_ENTER();

    DPVF( DPVF_ENTRYLEVEL, "Enter" );
    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "Param: lpSessionDesc = 0x%p  dwFlags = 0x%x", lpSessionDesc, dwFlags );

#if DBG
    if( dwFlags !=0 )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid flags specified" );
        return DVERR_INVALIDFLAGS;
    }

    hr = DV_ValidSessionDesc( lpSessionDesc );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error validating session description.  hr=0x%x", hr );
        return hr;
    }

    DV_DUMP_SD( lpSessionDesc );    
#endif

    DV_AUTO_LOCK(&m_CS);

    switch( m_dwCurrentState )
    {
    case DVSSTATE_SHUTDOWN:
    case DVSSTATE_RUNNING:
    case DVSSTATE_STARTUP:
        DPVF( DPVF_ERRORLEVEL, "Session is already in progress." );
        return DVERR_HOSTING;
    case DVSSTATE_NOTINITIALIZED:
        DPVF( DPVF_ERRORLEVEL, "Object not initialized" );
        return DVERR_NOTINITIALIZED;
    }

    if( m_lpSessionTransport == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid transport" );
        return DVERR_INVALIDOBJECT;
    }

    // Retrieve the information about the dplay/dnet session
    hr = m_lpSessionTransport->GetTransportSettings( &m_dwTransportSessionType, &m_dwTransportFlags );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Could not retrieve transport settings hr=0x%x", hr );
        return hr;
    }

    // Peer-to-peer mode not available in client/server mode
    if( m_dwTransportSessionType == DVTRANSPORT_SESSION_CLIENTSERVER &&
        (lpSessionDesc->dwSessionType == DVSESSIONTYPE_PEER) )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot host peer session in client/server transport" );
        return DVERR_NOTSUPPORTED;
    }

    // Server control target not available if host migration is enabled
    if( (m_dwTransportFlags & DVTRANSPORT_MIGRATEHOST) &&
        (lpSessionDesc->dwFlags & DVSESSION_SERVERCONTROLTARGET) )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot support host migration with server controlled targetting" );
        return DVERR_NOTSUPPORTED;
    }

    //
    // get compresssion info but instantiating a decode XMO for this guid..
    // then we will kill it..
    //

    memcpy(&wfx, s_lpwfxPrimaryFormat,sizeof(wfx));

    hr = DV_CreateConverter(lpSessionDesc->guidCT,
                            &pConverter,
                            &wfx,
                            NULL,
                            s_dwUnCompressedFrameSize,
                            FALSE);

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Compression type not supported. hr=0x%x", hr );
        return DVERR_COMPRESSIONNOTSUPPORTED;
    }

    memset(&xmi,0,sizeof(xmi));
    xmi.dwInputSize = 1;

    //
    // get compressed size from record subsystem instance of decoder
    //

    hr = pConverter->GetInfo(&xmi);

    if (FAILED(hr)) {
        return hr;
    }

    s_dwCompressedFrameSize = xmi.dwInputSize;
    s_dwUnCompressedFrameSize = xmi.dwOutputSize;


    DPVF(DPVF_INFOLEVEL, "Compressed frame size 0x%x, unCompressedSize 0x%x\n",
        s_dwCompressedFrameSize,
        s_dwUnCompressedFrameSize);

    //
    // delete converter XMO
    //

    ASSERT(pConverter->Release() == 0);

    SetCurrentState( DVSSTATE_STARTUP );

    // Scrub the session description

    memcpy( &m_dvSessionDesc, lpSessionDesc, sizeof( DVSESSIONDESC ) );

    m_dwNextHostOrderID = dwHostOrderIDSeed;

    hr = SetupBuffers();

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Failed to setup buffer pools hr=0x%x", hr );
        SetCurrentState( DVSSTATE_IDLE );        
        goto STARTSESSION_RETURN;
    }
#ifdef DVS_ENABLE_MULTICAST_SESSION
    if( lpSessionDesc->dwSessionType == DVSESSIONTYPE_FORWARDING )
    {
        hr = StartupMulticast();
                       
        if( FAILED( hr ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Unable to initializing multicast hr=0x%x", hr );
            SetCurrentState( DVSSTATE_IDLE );
            FreeBuffers();
            
            goto STARTSESSION_RETURN;
        }
    }
#endif

    // Setup name table
    m_voiceNameTable.Initialize();

    InitializeListHead(&m_PlayerActiveList);
    SetCurrentState( DVSSTATE_RUNNING );

    // Tell DirectPlay we're alive and want to see incoming traffic
    hr = m_lpSessionTransport->EnableReceiveHook( m_lpObject, DVTRANSPORT_OBJECTTYPE_SERVER );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Failed on call to EnableReceiveHook hr=0x%x", hr );
#ifdef DVS_ENABLE_MULTICAST_SESSION
        if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_FORWARDING )
        {
            ShutdownMulticast();
        }
#endif
        m_voiceNameTable.DeInitialize(TRUE, m_lpUserContext,m_lpMessageHandler);
        
        FreeBuffers();
        
        SetCurrentState( DVSSTATE_IDLE );
                
        goto STARTSESSION_RETURN;
    }

STARTSESSION_RETURN:

    DPVF( DPVF_ENTRYLEVEL, "Success" );

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::CheckForMigrate"
BOOL CDirectVoiceServerEngine::CheckForMigrate( DWORD dwFlags, BOOL fSilent )
{
    DPVF_ENTER();

    // We should shutdown the session if:
    //
    // 1. We're not in peer to peer mode
    // 2. "No host migration" flag was specified in session description
    // 3. "No host migrate" flag was specified on call to StopSession
    // 4. We were not asked to be silent
    // 5. There isn't a host migrate
    // 
    if( (m_dvSessionDesc.dwSessionType != DVSESSIONTYPE_PEER ) || 
        (m_dvSessionDesc.dwFlags & DVSESSION_NOHOSTMIGRATION) || 
        (dwFlags & DVFLAGS_NOHOSTMIGRATE) ||
        fSilent ||
        !(m_dwTransportFlags & DVTRANSPORT_MIGRATEHOST) )
    {
        DPVF( DPVF_ERRORLEVEL, "Destroying session." );
        return FALSE;
    }
    // A migration is possible
    else
    {
        return TRUE;
    }
}



#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::StopSession"
//
// StopSession
//
// This function is responsible for shutting down the directplayvoice session.  In sessions 
// without host migration, this function will simply cleanup the memory and detach itself
// from directplay.  This way directplay will take care of the host migration.  To use this
// option, specify fSilent = TRUE.
// 
// In addition, this function is called when an fatal error occurs while the session is 
// running.  
//
// It is also called when the user calls StopSession.
//
// Called By:
// - HandleMixerThreadError
// - HandleStopTransportSession
// - DVS_StopSession
// 
// Locks Required:
// - Global Write Lock
// 
HRESULT CDirectVoiceServerEngine::StopSession(DWORD dwFlags, BOOL fSilent, HRESULT hrResult )
{
    DPVF_ENTER();

    DPVF( DPVF_ENTRYLEVEL, "Begin" );
    DPVF( DPVF_APIPARAM, "Param: dwFlags = 0x%x", dwFlags );


    if( dwFlags & ~(DVFLAGS_NOHOSTMIGRATE) )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid flags specified" );
        return DVERR_INVALIDFLAGS;
    }

    DV_AUTO_LOCK(&m_CS);

    // We need to be initialized
    if( m_dwCurrentState == DVSSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not Initialized" );
        return DVERR_NOTINITIALIZED;
    }   

    if( m_dwCurrentState != DVSSTATE_RUNNING )
    {
        DPVF( DPVF_ERRORLEVEL, "Session is not running" );
        return DVERR_NOTHOSTING;
    }

    // Check to see if there is migration going on.  If a migration should
    // happen then we do not transmit a session lost message
    //
    if( !FAILED( hrResult ) )
    {
        if( !CheckForMigrate( dwFlags, fSilent ) )
        {
            Send_SessionLost( DVERR_SESSIONLOST );
        }
        // Host is migrating.  Inform the users
        else if( m_dwTransportFlags & DVTRANSPORT_MIGRATEHOST )
        {
            Send_HostMigrateLeave();
        }
    }

    // Wait until all the outstanding sends have completed  
    WaitForBufferReturns();

    // Disable receives
    m_lpSessionTransport->DisableReceiveHook( );

    // Waits for transport threads to be done inside our layer
    m_lpSessionTransport->WaitForDetachCompletion();    

    SetCurrentState( DVSSTATE_SHUTDOWN );

    // Kill name table
    m_voiceNameTable.DeInitialize(TRUE,m_lpUserContext,m_lpMessageHandler);

    // Cleanup the active list
    CleanupActiveList();

#ifdef DVS_ENABLE_MULTICAST_SESSION
    if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_FORWARDING )
    {
        ShutdownMulticast();
    }
#endif
    FreeBuffers();
    
    SetCurrentState( DVSSTATE_IDLE );

    // Check to see if the transport session was closed
    if( hrResult == DVERR_SESSIONLOST )
    {
        DVMSG_SESSIONLOST dvMsgLost;
        dvMsgLost.dwSize = sizeof( DVMSG_SESSIONLOST );
        dvMsgLost.hrResult = hrResult;

        TransmitMessage( DVMSGID_SESSIONLOST, &dvMsgLost, sizeof( DVPROTOCOLMSG_SESSIONLOST ) );
    }

    DPVF( DPVF_ENTRYLEVEL, "Done" );

    return DV_OK;
}

// WaitForBufferReturns
//
// This function waits until oustanding sends have completed before continuing
// we use this to ensure we don't deregister with outstanding sends.
// 
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::WaitForBufferReturns"
void CDirectVoiceServerEngine::WaitForBufferReturns()
{
    ULONG cnt = 0;
    DV_AUTO_LOCK(&m_CS);

    if((m_MsgBufferAllocs == 0) && (m_SpeechBufferAllocs == 0)) {
        DPVF_EXIT();                                         
        return;
    }        

    while( m_MsgBufferAllocs || m_SpeechBufferAllocs) 
    {
        m_lpSessionTransport->DoWork();
        Sleep( 20 );
        if (cnt ++ > 100) {
            break;
        }
    }

    return;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::GetSessionDesc"
//
// GetSessionDesc
//
// Called to retrieve the description of the current session.  
//
// Called By:
// - DVS_GetSessionDesc
//
// Locks Required:
// - Global Read Lock
//
HRESULT CDirectVoiceServerEngine::GetSessionDesc( LPDVSESSIONDESC lpSessionDesc )
{
    DPVF_ENTER();

    DPVF( DPVF_ENTRYLEVEL, "Enter" );
    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "Param: lpSessionDesc = 0x%p", lpSessionDesc );
#ifndef ASSUME_VALID_PARAMETERS
    if( lpSessionDesc == NULL)
    {
        DPVF( DPVF_ERRORLEVEL, "Session desc pointer bad" );
        return DVERR_INVALIDPOINTER;
    }

    if( lpSessionDesc->dwSize != sizeof( DVSESSIONDESC ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid size on session desc" );
        return DVERR_INVALIDPARAM;
    }   
#endif

    DV_AUTO_LOCK(&m_CS);

    // We need to be initialized
    if( m_dwCurrentState == DVSSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not Initialized" );
        return DVERR_NOTINITIALIZED;
    }   

    if( m_dwCurrentState != DVSSTATE_RUNNING )
    {
        DPVF( DPVF_ERRORLEVEL, "No host running" );
        return DVERR_NOTHOSTING;
    }

    memcpy( lpSessionDesc, &m_dvSessionDesc, sizeof( DVSESSIONDESC ) );     
    
    DV_DUMP_SD( (LPDVSESSIONDESC) lpSessionDesc );
    DPVF( DPVF_ENTRYLEVEL, "Done, Returning DV_OK" );

    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::SetTransmitTarget"
//
// SetTransmitTarget
//
// This function sets the transmit target for the specified user.  Only available in sessions with the 
// DVSESSION_SERVERCONTROLTARGET flag specified.
//
// Called By:
// - DVS_SetTransmitTarget
//
// Locks Required:
// - Global Write Lock
//
HRESULT CDirectVoiceServerEngine::SetTransmitTarget(DVID dvidSource, PDVID pdvidTargets, DWORD dwNumTargets, DWORD dwFlags)
{
    HRESULT hr;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    DPVF( DPVF_APIPARAM, "Param: dvidSource: 0x%x pdvidTargets: 0x%p  dwNumTargets: %d dwFlags: 0x%x", dvidSource, pdvidTargets, dwNumTargets, dwFlags );

#ifndef ASSUME_VALID_PARAMETERS
    hr = DV_ValidTargetList( pdvidTargets, dwNumTargets );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid target list hr=0x%x", hr );
        return hr;
    }

    // Flags must be 0.
    if( dwFlags != 0 )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid flags" );
        return DVERR_INVALIDFLAGS;
    }
#endif

    // We need to be initialized
    if( m_dwCurrentState == DVSSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not Initialized" );
        return DVERR_NOTINITIALIZED;
    }

    // We need to be running
    if( m_dwCurrentState != DVSSTATE_RUNNING )
    {
        DPVF( DPVF_ERRORLEVEL, "Not hosting" );
        return DVERR_NOTCONNECTED;
    }

    // Only if servercontroltarget is active
    if( !(m_dvSessionDesc.dwFlags & DVSESSION_SERVERCONTROLTARGET) )
    {
        DPVF( DPVF_ERRORLEVEL, "Only available with the DVSESSION_SERVERCONTROLTARGET session flag" );
        return DVERR_NOTALLOWED;
    }

    // Parameter checks
    if( !m_voiceNameTable.IsEntry( dvidSource ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid source player" );
        return DVERR_INVALIDPLAYER;
    }

    if( dwNumTargets > 0 )
    {
        for( DWORD dwIndex = 0; dwIndex < dwNumTargets; dwIndex++ )
        {
            if( !m_voiceNameTable.IsEntry( pdvidTargets[dwIndex] ) )
            {
                if( !m_lpSessionTransport->ConfirmValidGroup( pdvidTargets[dwIndex] ) )
                {
                    DPVF( DPVF_ERRORLEVEL, "Invalid target player/group" );
                    return DVERR_INVALIDTARGET;
                }
            } 
        }
    }

    if( dvidSource == DVID_ALLPLAYERS )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot set the target for all or none" );
        return DVERR_INVALIDPLAYER;
    }   

    // Grab the player and set the target field
    CVoicePlayer *pPlayerInfo;

    hr = m_voiceNameTable.GetEntry( dvidSource, (CVoicePlayer **) &pPlayerInfo, TRUE ); 

    if( FAILED( hr ) || pPlayerInfo == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Unable to lookup player entry.  hr=0x%x", hr );
        return DVERR_INVALIDPLAYER;
    }

    hr = pPlayerInfo->SetPlayerTargets( pdvidTargets, dwNumTargets );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Unable to set player target hr=0x%x", hr );
        pPlayerInfo->Release();
        return hr;
    }
    
    hr = BuildAndSendTargetUpdate( dvidSource, pPlayerInfo );

    pPlayerInfo->Release();

    DPVF( DPVF_ENTRYLEVEL, "Done" );

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::BuildAndSendTargetUpdate"
//
// BuildAndSendTargetUpdate
//
// This function builds and sends a message with a target list to the specified
// user.
//
HRESULT CDirectVoiceServerEngine::BuildAndSendTargetUpdate( DVID dvidSource,CVoicePlayer *pPlayerInfo )
{
    // Grab the player and set the target field
    HRESULT hr;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    DWORD dwTransmitSize;
    PDVPROTOCOLMSG_SETTARGET pSetTargetMsg;
    PVOID pvSendContext;

    DPVF_ENTER();

    // Protect target information
    DV_AUTO_LOCK(&m_CS);

    dwTransmitSize = sizeof( DVPROTOCOLMSG_SETTARGET ) + (pPlayerInfo->GetNumTargets()*sizeof(DVID));

    pBufferDesc = GetTransmitBuffer( dwTransmitSize, &pvSendContext,SERVER_POOL_MESSAGE);

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Failed to alloc memory" );
        return DVERR_BUFFERTOOSMALL;
    }

    pSetTargetMsg = (PDVPROTOCOLMSG_SETTARGET) pBufferDesc->pBufferData;

    // Send the message to the player
    pSetTargetMsg->dwType = DVMSGID_SETTARGETS;
    pSetTargetMsg->dwNumTargets = pPlayerInfo->GetNumTargets();
    
    memcpy( &pSetTargetMsg[1], pPlayerInfo->GetTargetList(), sizeof( DVID ) * pPlayerInfo->GetNumTargets() );

    hr = m_lpSessionTransport->SendToIDS( &dvidSource, 1, pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Unable to send target set message hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );
    }

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::GetTransmitTarget"
//
// GetTransmitTarget
//
// This function returns the current transmission target of the specified user.  
//
// Only available in sessions with the DVSESSION_SERVERCONTROLTARGET flag.
//
// Called By:
// - DVS_GetTransmitTarget
//
// Locks Required:
// - Global Read Lock
//
HRESULT CDirectVoiceServerEngine::GetTransmitTarget(DVID dvidSource, LPDVID lpdvidTargets, PDWORD pdwNumElements, DWORD dwFlags )
{
    HRESULT hr; 
    CVoicePlayer *pPlayerInfo;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();

    DPVF( DPVF_APIPARAM, "Param: dvidSource = 0x%x lpdvidTargets = 0x%p pdwNumElements = 0x%p dwFlags = 0x%x", dvidSource, lpdvidTargets, pdwNumElements, dwFlags );

#ifndef ASSUME_VALID_PARAMETERS
    if( pdwNumElements == NULL)
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid pointer" );
        return DVERR_INVALIDPOINTER;
    }

    if( pdwNumElements != NULL && 
        *pdwNumElements > 0 )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid target list buffer specified" );
        return DVERR_INVALIDPOINTER;
    }   

    if( pdwNumElements == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "You must provider a ptr # of elements" );
        return DVERR_INVALIDPARAM;
    }   

    // Flags must be 0.
    if( dwFlags != 0 )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid flags specified" );
        return DVERR_INVALIDFLAGS;
    }
#endif

    // We need to be initialized
    if( m_dwCurrentState == DVSSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Object not initialized" );
        return DVERR_NOTINITIALIZED;
    }

    // We need to be running
    if( m_dwCurrentState != DVSSTATE_RUNNING )
    {
        DPVF( DPVF_ERRORLEVEL, "No session running" );
        return DVERR_NOTCONNECTED;
    }

    // Only if servercontroltarget is active
    if( !(m_dvSessionDesc.dwFlags & DVSESSION_SERVERCONTROLTARGET) )
    {
        DPVF( DPVF_ERRORLEVEL, "Only available with the DVSESSION_SERVERCONTROLTARGET session flag" );
        return DVERR_NOTALLOWED;
    }

    if( dvidSource == DVID_ALLPLAYERS )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot get target for all or none" );
        return DVERR_INVALIDPLAYER;
    }

    // Parameter checks
    if( !m_voiceNameTable.IsEntry( dvidSource ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Specified player does not exist" );
        return DVERR_INVALIDPLAYER;
    } 

    // Grab the player and set the target field

    hr = m_voiceNameTable.GetEntry( dvidSource, (CVoicePlayer **) &pPlayerInfo, TRUE ); 

    if( FAILED( hr ) || pPlayerInfo == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Unable to lookup player entry.  hr=0x%x", hr );
        return DVERR_INVALIDPLAYER;
    }

    if( *pdwNumElements < pPlayerInfo->GetNumTargets() )
    {
        hr = DVERR_BUFFERTOOSMALL;
    }
    else
    {
        memcpy( lpdvidTargets, pPlayerInfo->GetTargetList(), pPlayerInfo->GetNumTargets()*sizeof(DVID) );
    }
    
    *pdwNumElements  = pPlayerInfo->GetNumTargets();

    pPlayerInfo->Release();

    DPVF( DPVF_ENTRYLEVEL, "Done" );

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::Initialize"
//
// Initialize
//
// This function is responsible for connecting the DirectPlayVoiceServer object
// to the associated transport session.  It sets up the object and makes
// it ready for a call to StartSession.
//
// Called By:
// - DV_Initialize
//
// Locks Required:
// - Global Write Lock
// 
HRESULT CDirectVoiceServerEngine::Initialize( CDirectVoiceTransport *lpTransport, LPDVMESSAGEHANDLER lpdvHandler, LPVOID lpUserContext, LPDWORD lpdwMessages, DWORD dwNumElements )
{
    HRESULT hr;

    DPVF_ENTER();

    DPVF( DPVF_ENTRYLEVEL, "Begin" );
    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "Param: lpTransport = 0x%p lpdvHandler = 0x%p lpUserContext = 0x%p ", lpTransport, lpdvHandler, lpUserContext );

    if( lpTransport == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid pointer" );
        return E_POINTER;
    }   

    DPVF( DPVF_ENTRYLEVEL, "Enter" );    
    DPVF( DPVF_APIPARAM, "lpdwMessages = 0x%p dwNumElements = %d", lpdwMessages, dwNumElements );

#ifndef ASSUMME_VALID_PARAMETERS
    hr = DV_ValidMessageArray( lpdwMessages, dwNumElements, TRUE );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid message array hr=0x%x", hr );
        return hr;
    }
#endif
    DPVF( DPVF_APIPARAM, "Message IDs=%d", dwNumElements );

    if( lpdwMessages != NULL )
    {
        for( DWORD dwIndex = 0; dwIndex < dwNumElements; dwIndex++ )
        {
            DPVF( DPVF_APIPARAM, "MessageIDs[%d] = %d", dwIndex, lpdwMessages[dwIndex] );
        }
    }

    DV_AUTO_LOCK(&m_CS);
    
    if( m_dwCurrentState != DVSSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Already initialized");
    }

    if( lpdvHandler == NULL && lpdwMessages != NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot specify message mask there is no callback function" );
        return DVERR_NOCALLBACK;
    }

    SetCurrentState( DVSSTATE_IDLE );   

    m_lpMessageHandler = lpdvHandler;

    hr = InternalSetNotifyMask( lpdwMessages, dwNumElements );

    if( FAILED( hr ) )
    {
        SetCurrentState( DVSSTATE_NOTINITIALIZED ); 
        DPVF( DPVF_ERRORLEVEL, "SetNotifyMask Failed hr=0x%x", hr );
        return hr;
    }   

    m_lpSessionTransport = lpTransport;
    m_lpUserContext = lpUserContext;

    hr = m_lpSessionTransport->Initialize();
    if (FAILED(hr)) {

        return hr;

    }

    m_dvidLocal = m_lpSessionTransport->GetLocalID();

    DPVF( DPVF_ENTRYLEVEL, "DVSE::Initialize() End" );

    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::ReceiveSpeechMessage"
//
// ReceiveSpeechMessage
//
// Responsible for processing a speech message when it is received.  
//
// Called By:
// - DV_ReceiveSpeechMessage
//
// Locks Required:
// - None 
//
BOOL CDirectVoiceServerEngine::ReceiveSpeechMessage( DVID dvidSource, LPVOID lpMessage, DWORD dwSize )
{
    PDVPROTOCOLMSG_FULLMESSAGE lpdvFullMessage;
    DPVF_ENTER();

    ASSERT( lpMessage );
    ASSERT( dwSize > 0 );

    lpdvFullMessage = (PDVPROTOCOLMSG_FULLMESSAGE) lpMessage;

    switch( lpdvFullMessage->dvGeneric.dwType )
    {
    case DVMSGID_SPEECHBOUNCE:
        // Ignore speech bounces, only reason we get is because we're sending to all or targetting
        // a client on the same ID as us.
        return TRUE;
    case DVMSGID_CONNECTREQUEST:
        return HandleConnectRequest( dvidSource, static_cast<PDVPROTOCOLMSG_CONNECTREQUEST>(lpMessage), dwSize );
    case DVMSGID_DISCONNECT:
        return HandleDisconnect( dvidSource, static_cast<PDVPROTOCOLMSG_DISCONNECT>(lpMessage), dwSize);
    case DVMSGID_SETTINGSCONFIRM:
        return HandleSettingsConfirm( dvidSource, static_cast<PDVPROTOCOLMSG_SETTINGSCONFIRM>(lpMessage), dwSize );
    case DVMSGID_SETTINGSREJECT:
        return HandleSettingsReject( dvidSource, static_cast<PDVPROTOCOLMSG_GENERIC>(lpMessage), dwSize );
    case DVMSGID_SPEECHWITHTARGET:
#ifdef DVS_ENABLE_MULTICAST_SESSION
        return HandleSpeechWithTarget( dvidSource, static_cast<PDVPROTOCOLMSG_SPEECHWITHTARGET>(lpMessage), dwSize );
#else
        ASSERT(FALSE);
        return FALSE;
#endif
    case DVMSGID_SPEECH:
        return HandleSpeech( dvidSource, static_cast<PDVPROTOCOLMSG_SPEECHHEADER>(lpMessage), dwSize );
    default:
        DPVF(  DPVF_WARNINGLEVEL, "DVSE::ReceiveSpeechMessage() Ignoring Non-Speech Message id=0x%x from=0x%x", 
             lpdvFullMessage->dvGeneric.dwType, dvidSource );
        return FALSE;
    }
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::HandleSpeech"
// HandleSpeech
//
// Handles processing of incoming speech packets (in echo server mode).
//
// How speech is handled depends on session type.  If the session is client/server, the 
// packet is buffered in the appropriate user's queue.  If the session is multicast,
// the packet is forwarded to the packet's target.
//
BOOL CDirectVoiceServerEngine::HandleSpeech( DVID dvidSource, PDVPROTOCOLMSG_SPEECHHEADER lpdvSpeech, DWORD dwSize )
{
    HRESULT hr = DV_OK;
    
    DPVF(DPVF_SPAMLEVEL, "(mixin/echo mode not supported)Ignoring Speech Message from=0x%x", 
         dvidSource );

    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::HandleSpeechWithTarget"
// HandleSpeechWithTarget
//
// Handles processing of incoming speech packets.
//
// How speech is handled depends on session type.  If the session is client/server, the 
// packet is buffered in the appropriate user's queue.  If the session is multicast,
// the packet is forwarded to the packet's target.
//

#ifdef DVS_ENABLE_MULTICAST_SESSION

BOOL CDirectVoiceServerEngine::HandleSpeechWithTarget( DVID dvidSource, PDVPROTOCOLMSG_SPEECHWITHTARGET lpdvSpeech, DWORD dwSize )
{
    DPVF_ENTER();

    DPVF(DPVF_ENTRYLEVEL, "Enter");

    DWORD dwSpeechSize;     // Size of speech portion in bytes
    DWORD dwTargetSize;     // Size of targetting info in bytes
    PBYTE pSourceSpeech;    // Pointer to speech within the packet
    HRESULT hr;
                                                                                 
    ASSERT( dwSize >= sizeof( DVPROTOCOLMSG_SPEECHWITHTARGET ) );

    dwTargetSize = lpdvSpeech->dwNumTargets*sizeof(DVID);

    ASSERT( dwSize >= (dwTargetSize + sizeof( DVPROTOCOLMSG_SPEECHWITHTARGET )+dwTargetSize ) );
    
    dwSpeechSize = dwSize - sizeof( DVPROTOCOLMSG_SPEECHWITHTARGET ) - dwTargetSize;

    pSourceSpeech = (PBYTE) lpdvSpeech;
    pSourceSpeech += sizeof( DVPROTOCOLMSG_SPEECHWITHTARGET ) + dwTargetSize;
    if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_FORWARDING )
    {
        PDVTRANSPORT_BUFFERDESC pBufferDesc;
        PDVPROTOCOLMSG_SPEECHWITHFROM pdvSpeechWithFrom;
        PVOID pvSendContext;

        DWORD dwTransmitSize = (dwSize - dwTargetSize - sizeof( DVPROTOCOLMSG_SPEECHWITHTARGET )) + sizeof( DVPROTOCOLMSG_SPEECHWITHFROM );
                               
        ASSERT(dwTransmitSize <= s_dwCompressedFrameSize + sizeof( DVPROTOCOLMSG_SPEECHWITHFROM ) );

        pBufferDesc = GetTransmitBuffer( dwTransmitSize, &pvSendContext,SERVER_POOL_SPEECH );

        if( pBufferDesc == NULL )
        {
            DPVF( DPVF_ERRORLEVEL, "Error allocating memory" );
            return FALSE;
        }

        pdvSpeechWithFrom = (PDVPROTOCOLMSG_SPEECHWITHFROM) pBufferDesc->pBufferData;

        pdvSpeechWithFrom->dvHeader.dwType = DVMSGID_SPEECHWITHFROM;
        pdvSpeechWithFrom->dvHeader.bMsgNum = lpdvSpeech->dvHeader.bMsgNum;
        pdvSpeechWithFrom->dvHeader.bSeqNum = lpdvSpeech->dvHeader.bSeqNum;
        pdvSpeechWithFrom->dvidFrom = dvidSource;

        memcpy( &pdvSpeechWithFrom[1], pSourceSpeech, dwSpeechSize );
        
        hr = m_lpSessionTransport->SendToIDS( (PDVID) &lpdvSpeech[1], lpdvSpeech->dwNumTargets, pBufferDesc, pvSendContext, 0 );

        if( hr == DVERR_PENDING )
        {
            hr = DV_OK;
        }
        else if( FAILED( hr ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Failed sending to ID hr=0x%x", hr );
            ReturnTransmitBuffer( pvSendContext );
        }       
    }

    DPVF(DPVF_ENTRYLEVEL, "Exit");

    return TRUE;
}

#endif


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::DoPlayerDisconnect"
//
// Performs the work of removing the specified player from the session.
//
// Optionally informs the specified player of their disconnection.  
//
// Called By:
// - HandleDisconnect (Player requests disconnect)
// - RemovePlayer (Dplay tells us player disconnected)
//
// Locks Required:
// - Global Write Lock
//
void CDirectVoiceServerEngine::DoPlayerDisconnect( DVID dvidPlayer, BOOL bInformPlayer ) 
{
    CVoicePlayer *pPlayerInfo = NULL;
    HRESULT hr;

    DPVF_ENTER();

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Disconnecting player 0x%x", dvidPlayer );

    DV_AUTO_LOCK(&m_CS);
    if( m_dwCurrentState != DVSSTATE_RUNNING )
    {
        DPVF( DPVF_ERRORLEVEL, "Player disconnect ignored, not running" );
        return;
    }

    hr = m_voiceNameTable.GetEntry( dvidPlayer, &pPlayerInfo, TRUE );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_WARNINGLEVEL, "Error retrieving player entry. hr=0x%x Player may have dropped", hr );
        return;
    }
    else
    {
        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Retrieved record" );   
    }

    // Mark record as disconnected
    pPlayerInfo->SetDisconnected();

    // Release reference and remove from the nametable
    m_voiceNameTable.DeleteEntry( dvidPlayer );

    // Remove player from the "active" list!
    pPlayerInfo->RemoveFromNotifyList();
    pPlayerInfo->Release();

#ifdef DVS_ENABLE_PEER_SESSION
    // If we're peer to peer session, inform players of disconnection
    if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER )
    {
        hr = Send_DeletePlayer( dvidPlayer );

        if( FAILED( hr ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Error sending deleteplayer to all hr=0x%x", hr );
        }

        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Sent player Quit to all" );                
    }
#endif
    // If specified, send a message to the user to inform them of the disconnection
    if( bInformPlayer )
    {
        hr = Send_DisconnectConfirm( dvidPlayer, DV_OK );

        if( FAILED( hr ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Error sending disconnect confirm hr=0x%x", hr );
        }
        else
        {
            DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Sent disconnect confirm to player" );                          
        }
    }

    DVMSG_DELETEVOICEPLAYER dvDeletePlayer;
    dvDeletePlayer.dvidPlayer = dvidPlayer;
    dvDeletePlayer.dwSize = sizeof( DVMSG_DELETEVOICEPLAYER );
    dvDeletePlayer.pvPlayerContext = pPlayerInfo->GetContext();
    pPlayerInfo->SetContext( NULL );

    // Release reference for find
    pPlayerInfo->Release(); 

    TransmitMessage( DVMSGID_DELETEVOICEPLAYER, &dvDeletePlayer, sizeof( DVMSG_DELETEVOICEPLAYER ) );
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::HandleDisconnect"
//
// HandleDisconnect
//
// Called when a DVMSGID_DISCONNECT message is received.
//
// Called By:
// - ReceiveSpeechMessage
//
// Locks Required:
// - None
//
BOOL CDirectVoiceServerEngine::HandleDisconnect( DVID dvidSource, PDVPROTOCOLMSG_DISCONNECT lpdvDisconnect, DWORD dwSize )
{
    DoPlayerDisconnect( dvidSource, TRUE );

    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::CreatePlayerEntry"
// 
// CreatePlayerEntry
//
// Performs the actual work of creating a player entity.  The work performed depends on the 
// type of session.
//
// Called By:
// - HandleSettingsConfirm
//
// Locks Required:
// - None
//
HRESULT CDirectVoiceServerEngine::CreatePlayerEntry( DVID dvidSource, PDVPROTOCOLMSG_SETTINGSCONFIRM lpdvSettingsConfirm, DWORD dwHostOrderID, CVoicePlayer **ppPlayer )
{
    HRESULT hr;
    CVoicePlayer *pNewPlayer;
    PLIST_ENTRY pEntry;

    DPVF_ENTER();

    DV_AUTO_LOCK(&m_CS);

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Requesting create [ID=0x%x]",dvidSource );

    pEntry = RemoveTailList(&m_PlayerContextPool);
    pNewPlayer = (CVoicePlayer *) CONTAINING_RECORD(pEntry, CVoicePlayer, m_PoolEntry);


    if( pNewPlayer == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "CDirectVoiceServerEngine::CreatePlayerEntry() Alloc failure on player struct" );
        return DVERR_OUTOFMEMORY;
    }

    hr = pNewPlayer->Initialize( dvidSource, dwHostOrderID, 0, lpdvSettingsConfirm->dwFlags, NULL, &m_PlayerContextPool);

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error initializing new player record hr=0x%x", hr );
        return hr;
    }

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Created regular player" );     

    hr = m_voiceNameTable.AddEntry( dvidSource, pNewPlayer );

    // Add failed.. release our entry, destroying player
    if( FAILED( hr ) )
    {
        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Error adding player to nametable hr=0x%x", hr );
        pNewPlayer->Release();
        return hr;
    }

    // Add player to the "active" list!

    pNewPlayer->AddToNotifyList(&m_PlayerActiveList);
    pNewPlayer->AddRef();

    *ppPlayer = pNewPlayer;

    return DV_OK;
}

#ifdef DVS_ENABLE_PEER_SESSION
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::SendPlayerList"
//
// SendPlayerList
//
// This function sends a bunch of DVMSGID_PLAYERLIST messages to the 
// client containing the list of current players.
//
// This will send multiple structues if the number 
// 
HRESULT CDirectVoiceServerEngine::SendPlayerList( DVID dvidSource, DWORD dwHostOrderID )
{
    BOOL bContinueEnum = FALSE;
    CVoicePlayer *lpPlayer;     
    LIST_ENTRY *pSearch;

    HRESULT hr = DV_OK;

    BOOL  fAtLeastOneSent = FALSE;
    DWORD dwCurrentBufferLoc;
    DWORD dwNumInCurrentPacket;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    PDVPROTOCOLMSG_PLAYERLIST lpdvPlayerList;
    DVPROTOCOLMSG_PLAYERLIST_ENTRY *pdvPlayerEntries;
    PVOID pvSendContext;
    
    DPVF_ENTER();

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Building player list" );

    pBufferDesc = GetTransmitBuffer(DVPROTOCOL_PLAYERLIST_MAXSIZE, &pvSendContext,SERVER_POOL_PLAYERLIST);

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Out of memory!" );
        return DVERR_OUTOFMEMORY;
    }

    lpdvPlayerList = (PDVPROTOCOLMSG_PLAYERLIST) pBufferDesc->pBufferData;

    pdvPlayerEntries = (DVPROTOCOLMSG_PLAYERLIST_ENTRY *) &lpdvPlayerList[1];

    lpdvPlayerList->dwType = DVMSGID_PLAYERLIST;
    lpdvPlayerList->dwHostOrderID = dwHostOrderID;  
    
    dwNumInCurrentPacket = 0;
    dwCurrentBufferLoc = sizeof(DVPROTOCOLMSG_PLAYERLIST); 

    DV_AUTO_LOCK(&m_CS);

    pSearch = m_PlayerActiveList.Flink;

    while( pSearch != &m_PlayerActiveList )
    {
        lpPlayer = CONTAINING_RECORD( pSearch, CVoicePlayer, m_NotifyListEntry );

        // We need to split the packet, start a new packet, transmit this one.
        if( (dwCurrentBufferLoc+sizeof(DVPROTOCOLMSG_PLAYERLIST_ENTRY)) > DVPROTOCOL_PLAYERLIST_MAXSIZE )
        {
            // Wrap up current packet and transmit
            lpdvPlayerList->dwNumEntries = dwNumInCurrentPacket;

            pBufferDesc->dwBufferSize = dwCurrentBufferLoc;
            
            hr = m_lpSessionTransport->SendToIDS( &dvidSource, 1,pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );             

            if( hr == DVERR_PENDING )
            {
                hr = DV_OK;
            }
            else if( FAILED( hr ) )
            {
                DPVF( DPVF_WARNINGLEVEL, "Error on internal send hr=0x%x (Didn't get playerlist)", hr );
                ReturnTransmitBuffer( pvSendContext );
                return hr;
            }               

            // Reset for further players

            DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Transmitting playerlist chunk %d players", dwNumInCurrentPacket );

            pBufferDesc = GetTransmitBuffer(DVPROTOCOL_PLAYERLIST_MAXSIZE, &pvSendContext, SERVER_POOL_PLAYERLIST);

            if( pBufferDesc == NULL )
            {
                DPVF( DPVF_ERRORLEVEL, "Out of memory!" );
                return DVERR_OUTOFMEMORY;
            }

            lpdvPlayerList = (PDVPROTOCOLMSG_PLAYERLIST) pBufferDesc->pBufferData;
            pdvPlayerEntries = (DVPROTOCOLMSG_PLAYERLIST_ENTRY *) &lpdvPlayerList[1];
            lpdvPlayerList->dwType = DVMSGID_PLAYERLIST;
            lpdvPlayerList->dwHostOrderID = dwHostOrderID;  
            dwCurrentBufferLoc = sizeof(DVPROTOCOLMSG_PLAYERLIST);          
            dwNumInCurrentPacket = 0;
            fAtLeastOneSent = TRUE;

        }

        pdvPlayerEntries[dwNumInCurrentPacket].dvidID = lpPlayer->GetPlayerID();
        pdvPlayerEntries[dwNumInCurrentPacket].dwPlayerFlags = lpPlayer->GetTransportFlags();
        pdvPlayerEntries[dwNumInCurrentPacket].dwHostOrderID = lpPlayer->GetHostOrder();

        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "PlayerList: Adding player ID 0x%x", lpPlayer->GetPlayerID() );     

        dwNumInCurrentPacket++;
        dwCurrentBufferLoc += sizeof(DVPROTOCOLMSG_PLAYERLIST_ENTRY);

        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "PlayerList: Got next player" );                    
        pSearch = pSearch->Flink;
    }

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "PlayerList: Build Complete" ); 

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "PlayerList: Total of %d entries", dwNumInCurrentPacket );

    // Remaining entries to be sent
    // 
    // (Or empty packet just so user gets their ID)
    // 
    if( !fAtLeastOneSent  )
    {
        // Wrap up current packet and transmit
        lpdvPlayerList->dwNumEntries = dwNumInCurrentPacket;

        pBufferDesc->dwBufferSize = dwCurrentBufferLoc;     
                
        hr = m_lpSessionTransport->SendToIDS( &dvidSource, 1, pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );                    

        if( hr == DVERR_PENDING || hr == DV_OK )
        {
            DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Playerlist sent" );
        }
        else
        {
            ReturnTransmitBuffer( pvSendContext );
            DPVF( DPVF_ERRORLEVEL, "Error on internal send hr=0x%x (Didn't get playerlist)", hr );
        }
    }

    return hr;
}
#endif

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::HandleSettingsConfirm"
//
// HandleSettingsConfirm
//
// Called to handle the DVMSGID_SETTINGSCONFIRM message.  Creates a player entry for
// the specified player and optionally informs all players in the session.
//
// Called By:
// - ReceiveSpeechMessage
//
// Locks Required:
// - Global Write Lock
//
BOOL CDirectVoiceServerEngine::HandleSettingsConfirm( DVID dvidSource, PDVPROTOCOLMSG_SETTINGSCONFIRM lpdvSettingsConfirm, DWORD dwSize )
{
    HRESULT hr;
    CVoicePlayer *pPlayer;
    
    DPVF_ENTER();

    DPVF( DPVF_ENTRYLEVEL, "Start [ID=0x%x]", dvidSource );
    
    DV_AUTO_LOCK(&m_CS);
    if( m_dwCurrentState != DVSSTATE_RUNNING )
    {
        DPVF( DPVF_ERRORLEVEL, "Ignoring settings confirm message, not hosting" );
        return TRUE;
    }

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Received settings confirm [ID=0x%x]", dvidSource );

    DWORD dwHostOrderID;    

    // This is a host migration version of this message, so re-use existing 
    // host order ID
    //
    if( lpdvSettingsConfirm->dwHostOrderID != DVPROTOCOL_HOSTORDER_INVALID )
    {
        dwHostOrderID = lpdvSettingsConfirm->dwHostOrderID; 

        // Further reduce chances of duplicate ID, if we received a host order ID > then
        // the last ID offset the next value by offset again.
        if( dwHostOrderID > m_dwNextHostOrderID )
        {
            m_dwNextHostOrderID += DVMIGRATE_ORDERID_OFFSET;
        }
    }
    else
    {
        dwHostOrderID = m_dwNextHostOrderID;        
        m_dwNextHostOrderID++;
    }

    hr = CreatePlayerEntry( dvidSource, lpdvSettingsConfirm, dwHostOrderID, &pPlayer );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_WARNINGLEVEL, "Error creating player entry. [ID=0x%x] hr=0x%x", dvidSource, hr );
        DPVF( DPVF_WARNINGLEVEL, "Normal during host migration" );
        return TRUE;
    }
    else
    {
        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Player Created [ID=0x%x]", dvidSource );
    }
#ifdef DVS_ENABLE_PEER_SESSION
    if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER )
    {
        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Sending current player list [ID=0x%x]", dvidSource );
        
        hr = SendPlayerList( dvidSource, dwHostOrderID );

        if( FAILED( hr ) )
        {
            DPVF( DPVF_WARNINGLEVEL, "Unable to send player list to player [ID=0x%x] hr=0x%x", dvidSource, hr );
        }
        else
        {
            DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Playerlist sent [[ID=0x%x]", dvidSource );
        }

        hr = Send_CreatePlayer( DVID_ALLPLAYERS, pPlayer );

        if( FAILED( hr ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Send to all for new player failed hr=0x%x", hr );
        }
        else
        {
            DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Informed players of join of [ID=0x%x]", dvidSource );
        }
    }
#endif
    DVMSG_CREATEVOICEPLAYER dvCreatePlayer;
    dvCreatePlayer.dvidPlayer = dvidSource;
    dvCreatePlayer.dwFlags = lpdvSettingsConfirm->dwFlags;
    dvCreatePlayer.dwSize = sizeof( DVMSG_CREATEVOICEPLAYER );
    dvCreatePlayer.pvPlayerContext = NULL;

    TransmitMessage( DVMSGID_CREATEVOICEPLAYER, &dvCreatePlayer, sizeof( DVMSG_CREATEVOICEPLAYER ) );

    pPlayer->SetContext( dvCreatePlayer.pvPlayerContext );

    // Release our reference to the player 
    pPlayer->Release();

    DPVF( DPVF_ENTRYLEVEL, "Done processing [ID=0x%x]", dvidSource );

    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::HandleSettingsReject"
//
// HandleSettingsReject
//
// This message type is ignored.
//
BOOL CDirectVoiceServerEngine::HandleSettingsReject( DVID dvidSource, PDVPROTOCOLMSG_GENERIC lpdvGeneric, DWORD dwSize )
{
    return TRUE;
}

BOOL CDirectVoiceServerEngine::CheckProtocolCompatible( BYTE ucMajor, BYTE ucMinor, DWORD dwBuild ) 
{
    /*
    if( ucMajor != DVPROTOCOL_VERSION_MAJOR ||
        ucMinor != DVPROTOCOL_VERSION_MINOR ||
        dwBuild != DVPROTOCOL_VERSION_BUILD )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }*/
    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::HandleConnectRequest"
//
// HandleConnectRequest
//
// This function is responsible for responding to user connect requests.  It is the server's
// oportunity to reject or accept players.  Called in response to a DVMSGID_CONNECTREQUEST
// message.
//
// Called By:
// - ReceiveSpeechMessage
//
// Locks Required:
// - Global Read Lock
// 
BOOL CDirectVoiceServerEngine::HandleConnectRequest( DVID dvidSource, PDVPROTOCOLMSG_CONNECTREQUEST lpdvConnectRequest, DWORD dwSize )
{
    HRESULT hr;

    DPVF_ENTER();

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Receive Connect Request.. From [ID=0x%x]", dvidSource );
    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Processing Request.. [ID=0x%x]", dvidSource );

    // Handle case where we've shutdown or starting up and we
    // receive this message
    //
    if( m_dwCurrentState != DVSSTATE_RUNNING )
    {
        hr = Send_ConnectRefuse( dvidSource, DVERR_NOTHOSTING );

        if( FAILED( hr ) )
        {
            DPVF( DPVF_WARNINGLEVEL, "Error! Failed on internal send hr=0x%x", hr );
        }

        return TRUE;
    }

    if( !CheckProtocolCompatible( lpdvConnectRequest->ucVersionMajor, 
                               lpdvConnectRequest->ucVersionMinor, 
                               lpdvConnectRequest->dwVersionBuild ) )
    {
        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Protocol is not compatible.  [ID=0x%x] Current=%d.%d b%d User=%d.%d b%d", dvidSource,
                            DVPROTOCOL_VERSION_MAJOR, 
                            DVPROTOCOL_VERSION_MINOR,
                            DVPROTOCOL_VERSION_BUILD,
                            lpdvConnectRequest->ucVersionMajor, 
                            lpdvConnectRequest->ucVersionMinor, 
                            lpdvConnectRequest->dwVersionBuild );
        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Rejecting connection request. [ID=0x%x]", dvidSource );

        hr = Send_ConnectRefuse( dvidSource, DVERR_INCOMPATIBLEVERSION );

        if( FAILED( hr ) )
        {
            DPVF( DPVF_WARNINGLEVEL, "Error! Failed on internal send hr=0x%x", hr );
        }       

        return TRUE;
    }

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Processing Request..2 [ID=0x%x]", dvidSource );

    hr = Send_ConnectAccept( dvidSource );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error sending player's connect request: hr=0x%x", hr );
        //// TODO: Handle this case better
    }
    else
    {
        DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Sent connect request [ID=0x%x]", dvidSource );
    }

    DPVF( DPVF_PLAYERMANAGE_DEBUG_LEVEL, "Processing Request..4 [ID=0x%x]", dvidSource );

    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::StopTransportSession"
//
// StopTransportSession
// 
// This function is called by the transport when the transport session
// is stopped.
//
// Called By:
// - DV_NotifyEvent
//
// Locks Required:
// - None
//
HRESULT CDirectVoiceServerEngine::StopTransportSession()
{
    StopSession(0,FALSE,DVERR_SESSIONLOST);
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::StartTransportSession"
HRESULT CDirectVoiceServerEngine::StartTransportSession( )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::AddPlayer"
HRESULT CDirectVoiceServerEngine::AddPlayer( DVID dvID )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::RemovePlayer"
HRESULT CDirectVoiceServerEngine::RemovePlayer( DVID dvID )
{
    if( m_voiceNameTable.IsEntry( dvID ) )
    {
        DoPlayerDisconnect( dvID, FALSE );
    }

    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::CreateGroup"
HRESULT CDirectVoiceServerEngine::CreateGroup( DVID dvID )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::DeleteGroup"
HRESULT CDirectVoiceServerEngine::DeleteGroup( DVID dvID )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::AddPlayerToGroup"
HRESULT CDirectVoiceServerEngine::AddPlayerToGroup( DVID dvidGroup, DVID dvidPlayer )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::RemovePlayerFromGroup"
HRESULT CDirectVoiceServerEngine::RemovePlayerFromGroup( DVID dvidGroup, DVID dvidPlayer )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::SetCurrentState"
// SetCurrentState
//
// Sets the current state of the client engine
// 
void CDirectVoiceServerEngine::SetCurrentState( DWORD dwState )
{
    DV_AUTO_LOCK(&m_CS);
    m_dwCurrentState = dwState;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::MigrateHost"
//
// MigrateHost
//
// This function is responsible for stoping the host in the case where the host
// suddenly migrates from this client.
//
// In most cases the session will be lost before this occurs on the local object
// and this will never get called.
// 
HRESULT CDirectVoiceServerEngine::MigrateHost( DVID dvidNewHost, LPDIRECTPLAYVOICESERVER lpdvServer )
{
    // Cleanup... 
//  return StopSession( DVFLAGS_SYNC, TRUE );
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::InternalSetNotifyMask"
// 
// SetNotifyMask
//
HRESULT CDirectVoiceServerEngine::InternalSetNotifyMask( LPDWORD lpdwMessages, DWORD dwNumElements )
{

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    if( m_lpdwMessageElements != NULL )
    {
        delete [] m_lpdwMessageElements;
    }

    m_dwNumMessageElements = dwNumElements;

    // Make copies of the message elements into our own message array.
    if( m_dwNumMessageElements > 0 )
    {
        m_lpdwMessageElements = new DWORD[m_dwNumMessageElements];

        if( m_lpdwMessageElements == NULL )
        {
            DPVF( DPVF_ERRORLEVEL, "Initialize: Error allocating memory" );
            return DVERR_OUTOFMEMORY;
        }

        memcpy( m_lpdwMessageElements, lpdwMessages, sizeof(DWORD)*m_dwNumMessageElements );
    }
    else
    {
        m_lpdwMessageElements = NULL;
    }   

    DPVF( DPVF_ENTRYLEVEL, "Done" );

    return DV_OK;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::SetNotifyMask"
// 
// SetNotifyMask
//
// This function sets the notification mask for the DirectPlayVoice object.
//
// The array passed in lpdwMessages specify the ID's of the notifications the user wishes to
// receive.  This or specifying NULL for the array turns on all notifications.
//
// Called By:
// - DVS_SetNotifyMask
//
// Locks Required:
// - m_csNotifyLock (Notification array lock)
//
HRESULT CDirectVoiceServerEngine::SetNotifyMask( LPDWORD lpdwMessages, DWORD dwNumElements )
{
    HRESULT hr;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    DPVF( DPVF_ENTRYLEVEL, "Enter" );    
    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "lpdwMessages = 0x%p dwNumElements = %d", lpdwMessages, dwNumElements );

#ifndef ASSUME_VALID_PARAMETERS
    hr = DV_ValidMessageArray( lpdwMessages, dwNumElements, TRUE );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid message array hr=0x%x", hr);
        return hr;
    }
#endif
    DPVF( DPVF_APIPARAM, "Message IDs=%d", dwNumElements );

    if( lpdwMessages != NULL )
    {
        for( DWORD dwIndex = 0; dwIndex < dwNumElements; dwIndex++ )
        {
            DPVF( DPVF_APIPARAM, "MessageIDs[%d] = %d", dwIndex, lpdwMessages[dwIndex] );
        }
    }

    if( m_dwCurrentState == DVSSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not initialized" );
        return DVERR_NOTINITIALIZED;
    }

    if( m_lpMessageHandler == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot specify message mask there is no callback function" );
        return DVERR_NOCALLBACK;
    }   

    hr = InternalSetNotifyMask( lpdwMessages, dwNumElements );

    DPVF( DPVF_ENTRYLEVEL, "Done" );

    return hr;

}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::CleanupActiveList"
void CDirectVoiceServerEngine::CleanupActiveList()
{
    LIST_ENTRY *pSearch;
    CVoicePlayer *pVoicePlayer;

    DPVF_ENTER();

    DV_AUTO_LOCK(&m_CS);

    pSearch = m_PlayerActiveList.Flink;

    while( pSearch != &m_PlayerActiveList )
    {
        pVoicePlayer = CONTAINING_RECORD( pSearch, CVoicePlayer, m_NotifyListEntry );

        pSearch = pSearch->Flink;

        pVoicePlayer->RemoveFromNotifyList();
        pVoicePlayer->Release();
    }

}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::GetTransmitBuffer"
PDVTRANSPORT_BUFFERDESC CDirectVoiceServerEngine::GetTransmitBuffer( DWORD dwSize, LPVOID *ppvSendContext, DWORD dwPoolNumber )
{
    PDVTRANSPORT_BUFFERDESC pNewBuffer = NULL;
    DWORD dwFPMIndex = 0xFFFFFFFF;
    DWORD dwWastedSpace = 0xFFFFFFFF;
    DWORD dwSearchFPMIndex;    
    PLIST_ENTRY  pListHead;    

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);


    pNewBuffer = (PDVTRANSPORT_BUFFERDESC) DV_REMOVE_TAIL(&m_BufferDescPool);

    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: Got a buffer desc address 0x%p", (void *) pNewBuffer );

    if( pNewBuffer == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Error getting transmit buffer" );
        goto GETTRANSMITBUFFER_ERROR;
    }

    pNewBuffer->lRefCount = 0;
    pNewBuffer->dwObjectType = DVTRANSPORT_OBJECTTYPE_SERVER;
    pNewBuffer->dwFlags = 0;
    
    switch (dwPoolNumber) {
    case SERVER_POOL_MESSAGE:
        m_MsgBufferAllocs++;
        pListHead = &m_MessagePool;
        break;
    case SERVER_POOL_PLAYERLIST:
        pListHead = &m_PlayerListPool;
        break;
    case SERVER_POOL_SPEECH:
        m_SpeechBufferAllocs++;
        pListHead = &m_SpeechBufferPool;
        break;
    }

    pNewBuffer->pvContext = pListHead;    
    pNewBuffer->pBufferData = (PBYTE) DV_REMOVE_TAIL(pListHead);    

    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: Got a buffer value at address 0x%p", (void *) pNewBuffer->pBufferData );

    if( pNewBuffer->pBufferData == NULL )
    {
        DPVF( 0, "Error getting buffer for buffer desc" );
        goto GETTRANSMITBUFFER_ERROR;
    }
    
    pNewBuffer->dwBufferSize = dwSize;
    *ppvSendContext = pNewBuffer;

    return pNewBuffer;

GETTRANSMITBUFFER_ERROR:
   
    if (dwPoolNumber == SERVER_POOL_MESSAGE) {

        m_SpeechBufferAllocs--;
        
    } else if (dwPoolNumber == SERVER_POOL_SPEECH) {

        m_MsgBufferAllocs--;

    }

    if( pNewBuffer != NULL && pNewBuffer->pBufferData != NULL )
    {
        DV_INSERT_TAIL(pListHead,pNewBuffer->pBufferData );
    }

    if( pNewBuffer != NULL )
    {
        DV_INSERT_TAIL(&m_BufferDescPool,pNewBuffer );
    }    

    return NULL;
    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::ReturnTransmitBuffer"
// ReturnTransmitBuffer
//
// PDVTRANSPORT_BUFFERDESC pBufferDesc - Buffer description of buffer to return
// LPVOID lpvContext - Context value to be used when returning the buffer 
// 
void CDirectVoiceServerEngine::ReturnTransmitBuffer( PVOID pvContext )
{
    PDVTRANSPORT_BUFFERDESC pBufferDesc = (PDVTRANSPORT_BUFFERDESC) pvContext;
    PLIST_ENTRY pPool = (PLIST_ENTRY) pBufferDesc->pvContext;    

    DPVF_ENTER();

    if (pPool == &m_SpeechBufferPool) {
        m_SpeechBufferAllocs--;
    }
        
    if (pPool == &m_MessagePool) {
        m_MsgBufferAllocs--;
    }

    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: Returning a buffer desc at address 0x%p", (void *) pBufferDesc );
    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: Returning a buffer at address 0x%p", (void *) pBufferDesc->pBufferData );    

    // Release memory
    DV_INSERT_TAIL(pPool,pBufferDesc->pBufferData );

    // Release buffer description
    DV_INSERT_TAIL(&m_BufferDescPool,pBufferDesc);
    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::SendComplete"
HRESULT CDirectVoiceServerEngine::SendComplete( PDVEVENTMSG_SENDCOMPLETE pSendComplete )
{
    DPVF_ENTER();
    ReturnTransmitBuffer( pSendComplete->pvUserContext );
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::SetupBuffers"
HRESULT CDirectVoiceServerEngine::SetupBuffers()
{
    HRESULT hr = DV_OK;    

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    InitializeListHead(&m_BufferDescPool);
    hr = DV_InitializeList(&m_BufferDescPool,
                      m_lpSessionTransport->GetMaxPlayers()*DV_CT_FRAMES_PER_PERIOD,
                      sizeof(DVTRANSPORT_BUFFERDESC),
                      DVLAT_RAW);
    if (SUCCEEDED(hr)) {

        m_pBufferDescAllocation = m_BufferDescPool.Flink;

        InitializeListHead(&m_MessagePool);
        hr = DV_InitializeList(&m_MessagePool,
                          m_lpSessionTransport->GetMaxPlayers()*2,
                          sizeof(DVPROTOCOLMSG_FULLMESSAGE),
                          DVLAT_RAW);

    }

    if (SUCCEEDED(hr)) {

        m_pMessagePoolAllocation = m_MessagePool.Flink;

        InitializeListHead(&m_PlayerListPool);
        hr = DV_InitializeList(&m_PlayerListPool,
                          m_lpSessionTransport->GetMaxPlayers(),
                          DVPROTOCOL_PLAYERLIST_MAXSIZE,
                          DVLAT_RAW);

    }

    if (SUCCEEDED(hr)) {

        DWORD size;

        m_pPlayerListPoolAllocation = m_PlayerListPool.Flink;

        size = sizeof(DVPROTOCOLMSG_SPEECHWITHFROM) + \
            s_dwCompressedFrameSize + COMPRESSION_SLUSH;

        InitializeListHead(&m_SpeechBufferPool);
        hr = DV_InitializeList(&m_SpeechBufferPool,
                          m_lpSessionTransport->GetMaxPlayers()*DV_CT_FRAMES_PER_PERIOD,
                          size,
                          DVLAT_RAW);
        
    }

    if (SUCCEEDED(hr)) {

        m_pSpeechBufferAllocation = m_SpeechBufferPool.Flink;

        InitializeListHead(&m_PlayerContextPool);
        hr = DV_InitializeList(&m_PlayerContextPool,
                          m_lpSessionTransport->GetMaxPlayers(),
                          1,
                          DVLAT_CVOICEPLAYER);

    }


    if (FAILED(hr)) {

        DPVF( 0, "Error buffer pools" );
        FreeBuffers();
        return DVERR_OUTOFMEMORY; 
    }

    return DV_OK;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::FreeBuffers"
HRESULT CDirectVoiceServerEngine::FreeBuffers()
{

    DPVF_ENTER();

    DV_FreeList(&m_BufferDescPool, DVLAT_RAW);
    DV_FreeList(&m_MessagePool, DVLAT_RAW);
    DV_FreeList(&m_PlayerListPool, DVLAT_RAW);
    DV_FreeList(&m_PlayerContextPool, DVLAT_CVOICEPLAYER);
    DV_FreeList(&m_SpeechBufferPool, DVLAT_RAW);

    DV_POOL_FREE(m_pBufferDescAllocation);
    DV_POOL_FREE(m_pMessagePoolAllocation);
    DV_POOL_FREE(m_pSpeechBufferAllocation);
    DV_POOL_FREE(m_pPlayerListPoolAllocation);

    m_pPlayerListPoolAllocation = NULL;
    m_pBufferDescAllocation = NULL;
    m_pMessagePoolAllocation = NULL;
    m_pSpeechBufferAllocation = NULL;


    return DV_OK;

}
