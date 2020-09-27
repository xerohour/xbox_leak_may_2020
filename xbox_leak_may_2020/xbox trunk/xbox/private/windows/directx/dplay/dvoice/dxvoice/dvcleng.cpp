/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dvclientengine.cpp
 *  Content:    Implementation of class for DirectXVoice Client
 *
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 * 07/19/99     rodtoll Created
 * 09/01/2000   georgioc started rewrite/port to xbox
========================================================================== */

#include "dvntos.h"
#include <dvoicep.h>
#include "dvcleng.h"
#include "dvshared.h"
#include "mixutils.h"
#include "sndutils.h"
#include "dvrecsub.h"
#include "dvclient.h"
#include "dvshared.h"
#include "dvsndt.h"
#include "dvserver.h"
#include "dvdxtran.h"
#include "in_core.h"

// Forces full duplex mode
//#define __FORCEFULLDUPLEX

// # of ms of inactivity before a multicast user is considered to have
// timed-out.
#define DV_MULTICAST_USERTIMEOUT_PERIOD     300000

// # of ms of inactivity before an incoming audio stream is considered to 
// have stopped.  Used to determine when to send PLAYERVOICESTOP 
// message.
#define PLAYBACK_RECEIVESTOP_TIMEOUT        500

// # of ms the notify thread sleeps without notification to wakeup 
// and perform house cleaning.
#define DV_CLIENT_NOTIFYWAKEUP_TIMEOUT      100

// # of ms before a connect request is considered to have been lost
#define DV_CLIENT_CONNECT_RETRY_TIMEOUT     1250

// # of ms before we should timeout a connect request completely
#define DV_CLIENT_CONNECT_TIMEOUT           30000

// Maximum count notification semaphores can have 
#define DVCLIENT_NOTIFY_MAXSEMCOUNT         20

//// TODO: Needs tuning.
// # of ms to wait for disconnect reply from server before timing out.
#define DV_CLIENT_DISCONNECT_TIMEOUT        10000


#define DV_CLIENT_SRCQUALITY_INVALID        ((DIRECTSOUNDMIXER_SRCQUALITY) 0xFFFFFFFF)

#define CLIENT_POOLS_NUM_TARGETS_BUFFERED   10

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::CDirectVoiceClientEngine"
//
// Constructor
//
// Initializes object to uninitialized state.  Must call Initialize succesfully before 
// the object can be used (except GetCompressionTypes which can be called at any time).  
//
CDirectVoiceClientEngine::CDirectVoiceClientEngine( DIRECTVOICECLIENTOBJECT *lpObject
    ):  m_dwSignature(VSIG_CLIENTENGINE),
        m_lpFramePool(NULL),
        m_lpObject(lpObject),
        m_lpSessionTransport(NULL),
        m_lpUserContext(NULL),
        m_tmpFrame(NULL),
        m_RecordSubSystem(NULL),
        m_dvidServer(0),
        m_bLastPeak(0),
        m_bLastPlaybackPeak(0),
        m_bLastTransmitted(FALSE),
        m_bMsgNum(0),
        m_bSeqNum(0),
        m_dwLastConnectSent(0),
        m_dwCurrentState(DVCSTATE_NOTINITIALIZED),
        m_hrConnectResult(DV_OK),
        m_hrDisconnectResult(DV_OK),
        m_lpdvServerMigrated(NULL),
        m_pFramePool(NULL),
        m_pMessageElements(NULL),
        m_dwNumMessageElements(0),
        m_fSessionLost(FALSE),
        m_fLocalPlayerNotify(FALSE),
        m_dwHostOrderID(DVPROTOCOL_HOSTORDER_INVALID),
        m_pdvidTargets(NULL),
        m_dwNumTargets(0),
        m_dwTargetVersion(0),
        m_dwPlayActiveCount(0),
        m_fLocalPlayerAvailable(FALSE),
        m_fNotifyQueueEnabled(FALSE),
        m_pBufferDescAllocation(NULL),
        m_pMessagePoolAllocation(NULL),
        m_pSpeechBufferAllocation(NULL)

{

    InitializeListHead(&m_NotifyList);
    memset(&m_dvClientConfig, 0 ,sizeof(DVCLIENTCONFIG));

    InitializeCriticalSection(&m_CS);

    m_SpeechBufferAllocs = 0;
    m_MsgBufferAllocs = 0;

}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::~CDirectVoiceClientEngine"
// Destructor
//
// This function requires a write lock to complete.  
//
// If the object is connected to a session, it will be disconnected
// by this function.
//
// Releases the resources associated with the object and stops the 
// notifythread.
//
// Locks Required:
// - Class Write Lock
//
// Called By:
// DVC_Release when reference count reaches 0.
//
CDirectVoiceClientEngine::~CDirectVoiceClientEngine()
{
    DPVF_ENTER();

    //
    // BUGBUG to we need DPC sync. here?

    if( m_dwCurrentState != DVCSTATE_IDLE && 
        m_dwCurrentState != DVCSTATE_NOTINITIALIZED )
    {
        Cleanup();
    }

    if( m_lpdvServerMigrated != NULL )
    {
        m_lpdvServerMigrated->Release();
        m_lpdvServerMigrated = NULL;
    }

    if( m_pMessageElements != NULL )
        DV_POOL_FREE(m_pMessageElements);       

    //
    // lower irql..

    NotifyQueue_Free();

    if( m_pdvidTargets != NULL )
    {
        DV_POOL_FREE (m_pdvidTargets);
    }

    m_dwSignature = VSIG_CLIENTENGINE_FREE;
    DPVF_EXIT();
}

// InternalSetNotifyMask
//
// Sets the list of valid notifiers for this object.
//
// must be called at DPC 
//
// Called By:
// DVC_SetNotifyMask
//
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::InternalSetNotifyMask" 
HRESULT CDirectVoiceClientEngine::InternalSetNotifyMask( LPDWORD lpdwMessages, DWORD dwNumElements )
{

    DPVF_ENTER();

    // Delete previous elements
    if( m_pMessageElements != NULL )
    {
        DV_POOL_FREE(m_pMessageElements);
        m_pMessageElements = NULL;
    }

    m_dwNumMessageElements = dwNumElements;

    // Make copies of the message elements into our own message array.
    if( dwNumElements > 0 )
    {
        m_pMessageElements = (PDWORD) DV_POOL_ALLOC(sizeof(DWORD)*dwNumElements);

        if( m_pMessageElements == NULL )
        {
            DPVF( DPVF_ERRORLEVEL, "Initialize: Error allocating memory" );
            return DVERR_OUTOFMEMORY;
        }

        memcpy( m_pMessageElements, lpdwMessages, sizeof(DWORD)*dwNumElements );
    }
    else
    {
        m_pMessageElements = NULL;
    }   

    DPVF_EXIT();
    return DV_OK;

}


// SetNotifyMask
//
// Sets the list of valid notifiers for this object.
//
// Locks Needed:
// - ReadLock to check status and then releases it.
// - m_csNotifyLock to update notification list
//
// Called By:
// DVC_SetNotifyMask
//
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SetNotifyMask" 
HRESULT CDirectVoiceClientEngine::SetNotifyMask( LPDWORD lpdwMessages, DWORD dwNumElements )
{
    HRESULT hr;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "Enter" );
#ifndef ASSUME_VALID_PARAMETERS
    hr = DV_ValidMessageArray( lpdwMessages, dwNumElements, FALSE );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL,"ValidMessageArray Failed 0x%x", hr ); 
        DPVF_EXIT();
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

    if( m_dwCurrentState == DVCSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not initialized" );
        DPVF_EXIT();
        return DVERR_NOTINITIALIZED;
    }

    if( m_lpMessageHandler == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot specify message mask there is no callback function" );
        return DVERR_NOCALLBACK;
    }       

    hr = InternalSetNotifyMask( lpdwMessages, dwNumElements );

    DPVF( DPVF_APIPARAM, "Returning hr=0x%x", hr );

    DPVF_EXIT();
    return DV_OK;
}

// Initialize
//
// Initializes this object into a state where it can be used to Connect to a session.  Sets the 
// notification function, notification mask and the transport object that will be used.
//
// Starts the notification thread.
//
// Locks Required:
// - Class Write Lock
// 
// Called By: 
// DV_Initialize
//
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::Initialize"
HRESULT CDirectVoiceClientEngine::Initialize(
    CDirectVoiceTransport *lpTransport,
    LPDVMESSAGEHANDLER lpdvHandler,
    LPVOID lpUserContext,
    LPDWORD lpdwMessages,
    DWORD dwNumElements )
{
    HRESULT hr;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    DPVF( DPVF_ENTRYLEVEL, "Enter" );

    DPVF( DPVF_APIPARAM, "Param: lpTransport = 0x%p lpdvHandler = 0x%p lpUserContext = 0x%p dwNumElements = %d", lpTransport, lpdvHandler, lpUserContext, dwNumElements );   

    if( lpTransport == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid transport" );
        DPVF_EXIT();
        return DVERR_INVALIDPOINTER;
    }

#ifndef ASSUME_VALID_PARAMETERS
    hr = DV_ValidMessageArray( lpdwMessages, dwNumElements, FALSE );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "ValidMessageArray Failed hr = 0x%x", hr );   
        DPVF_EXIT();
        return hr;
    }   
    
    DPVF( DPVF_APIPARAM, "Message IDs=%d", dwNumElements );
    
#if DBG
    if( lpdwMessages != NULL )
    {
        for( DWORD dwIndex = 0; dwIndex < dwNumElements; dwIndex++ )
        {
            DPVF( DPVF_APIPARAM, "MessageIDs[%d] = %d", dwIndex, lpdwMessages[dwIndex] );
        }
    }
#endif

    if( lpdvHandler == NULL && lpdwMessages != NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot specify message mask there is no callback function" );
        DPVF_EXIT();
        return DVERR_NOCALLBACK;
    }   

#endif

    if( m_dwCurrentState != DVCSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Already Initialized" );
    }

    m_dwLastConnectSent = 0;
    m_dwSynchBegin = 0;
    SetCurrentState( DVCSTATE_IDLE );   

    m_lpMessageHandler = lpdvHandler;
    
    hr = InternalSetNotifyMask( lpdwMessages, dwNumElements );

    if( FAILED( hr ) )
    {
        SetCurrentState( DVCSTATE_NOTINITIALIZED ); 
        DPVF( DPVF_ERRORLEVEL, "SetNotifyMask Failed hr=0x%x", hr );
        DPVF_EXIT();
        return hr;
    }

    m_lpSessionTransport = lpTransport;
    m_lpUserContext = lpUserContext;

    m_dvidLocal = 0;

    DPVF_EXIT();

    hr = m_lpSessionTransport->Initialize();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::Connect"
// Connect
//
// Implements the IDirectXVoiceClient::Connect function.
//
// Locks Required:
// - Write Lock
//
// Called By:
// DVC_Connect
//
HRESULT CDirectVoiceClientEngine::Connect(LPDVCLIENTCONFIG lpClientConfig, DWORD dwFlags )
{
    HRESULT hr; 

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "Enter" );

#ifndef ASSUME_VALID_PARAMETERS
    hr = DV_ValidClientConfig( lpClientConfig );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid Client Config hr=0x%x", hr );
        DPVF_EXIT();
        return hr;
    }

#endif

    DV_AUTO_LOCK(&m_CS);

    DV_DUMP_CC( lpClientConfig );

    if( m_dwCurrentState == DVCSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Object not initialized" );
        DPVF_EXIT();
        return DVERR_NOTINITIALIZED;
    }

    if( m_dwCurrentState == DVCSTATE_CONNECTING ||
        m_dwCurrentState == DVCSTATE_DISCONNECTING )
    {
        DPVF( DPVF_ERRORLEVEL, "Already connecting or disconnecting" );
        return DVERR_ALREADYPENDING;
    }

    if( m_dwCurrentState == DVCSTATE_CONNECTED )
    {
        DPVF( DPVF_ERRORLEVEL, "Already connected" );
        DPVF_EXIT();
        return DVERR_CONNECTED;
    }

    // RESET Session flags that need to be reset on every connect
    m_fSessionLost = FALSE;
    m_fLocalPlayerNotify = FALSE;
    m_fLocalPlayerAvailable = FALSE;
    m_dwHostOrderID = DVPROTOCOL_HOSTORDER_INVALID; 

    memcpy( &m_dvClientConfig, lpClientConfig, sizeof( DVCLIENTCONFIG ) );

    if( m_dvClientConfig.dwThreshold == DVTHRESHOLD_DEFAULT )
    {
        m_dvClientConfig.dwThreshold = s_dwDefaultSensitivity;
    }

    if( m_dvClientConfig.dwThreshold == DVTHRESHOLD_UNUSED )
    {
        m_dvClientConfig.dwThreshold = s_dwDefaultSensitivity;
    }

    m_dwLastConnectSent = 0;
    m_dwSynchBegin = 0;
    SetCurrentState( DVCSTATE_CONNECTING );

    //
    // initialize notification queue
    //

    hr = DV_InitializeList(&m_NotificationElementPool,
                           m_lpSessionTransport->GetMaxPlayers(),
                           sizeof(CNotifyElement),
                           DVLAT_RAW);

    if (FAILED(hr)) {
        ASSERT(FALSE);
        DPVF_EXIT();
        return hr;
    }

    m_fNotifyQueueEnabled = TRUE;
    
    m_dwPlayActiveCount = 0;
    InitializeListHead( &m_PlayActivePlayersList);
    InitializeListHead( &m_PlayAddPlayersList);
    InitializeListHead( &m_NotifyActivePlayersList);
    InitializeListHead( &m_NotifyAddPlayersList);

    hr = SetupInitialBuffers(); 

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "SetupBuffersInitial Failed 0x%x", hr );
        DPVF_EXIT();
        goto CONNECT_ERROR;
    }        

    hr = m_lpSessionTransport->EnableReceiveHook( m_lpObject, DVTRANSPORT_OBJECTTYPE_CLIENT );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "EnableReceiveHook Failed 0x%x", hr );
        DPVF_EXIT();
        goto CONNECT_ERROR;
    }

    m_dvidServer = m_lpSessionTransport->GetServerID();
    m_dvidLocal = m_lpSessionTransport->GetLocalID();

    //
    // Initialize the name table
    //

    m_voiceNameTable.Initialize();

    //
    // Send connect request to the server
    //

    m_dwLastConnectSent = GetTickCount();
    m_dwSynchBegin = m_dwLastConnectSent;

    hr = Send_ConnectRequest();
    
    DPVF(  DPVF_INFOLEVEL, "DVCE::Connect() - Sending Request to server" );

    if( FAILED( hr ) )
    {
        DPVF(  DPVF_ERRORLEVEL, "Error on send 0x%x", hr  );
        DPVF_EXIT();
        goto CONNECT_ERROR;
    }

    DPVF_EXIT();
    return DVERR_PENDING;
    
CONNECT_ERROR:

    SetCurrentState( DVCSTATE_IDLE );       
               
    m_voiceNameTable.DeInitialize((m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER),m_lpUserContext,m_lpMessageHandler);
    
    FreeBuffers();
    
    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::Send_SessionLost"
HRESULT CDirectVoiceClientEngine::Send_SessionLost()
{
    PDVPROTOCOLMSG_SESSIONLOST pSessionLost;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    LPVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();
    pBufferDesc = GetTransmitBuffer(sizeof( DVPROTOCOLMSG_SESSIONLOST ), &pvSendContext, FALSE );

    if( pBufferDesc == NULL )
    {
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY;       
    }

    pSessionLost = (PDVPROTOCOLMSG_SESSIONLOST) pBufferDesc->pBufferData;

    // Send connection request to the server
    pSessionLost->dwType = DVMSGID_SESSIONLOST;
    pSessionLost->hresReason = DVERR_SESSIONLOST;
    
    // Fixed so that it gets sent
    hr = m_lpSessionTransport->SendToAll( pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr != DVERR_PENDING && hr != DV_OK )
    {
        DPVF( 0, "Error sending connect request hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );
    }
    // Pending = OK = expected
    else
    {
        hr = DV_OK;
    }
    
    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::Send_ConnectRequest"
HRESULT CDirectVoiceClientEngine::Send_ConnectRequest()
{
    PDVPROTOCOLMSG_CONNECTREQUEST pConnectRequest;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    LPVOID pvSendContext;
    HRESULT hr;

    DPVF_ENTER();
    pBufferDesc = GetTransmitBuffer(sizeof( DVPROTOCOLMSG_CONNECTREQUEST ),&pvSendContext, FALSE );

    if( pBufferDesc == NULL )
    {
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY;       
    }

    pConnectRequest = (PDVPROTOCOLMSG_CONNECTREQUEST) pBufferDesc->pBufferData;

    // Send connection request to the server
    pConnectRequest->dwType = DVMSGID_CONNECTREQUEST;
    pConnectRequest->ucVersionMajor = DVPROTOCOL_VERSION_MAJOR;
    pConnectRequest->ucVersionMinor = DVPROTOCOL_VERSION_MINOR;
    pConnectRequest->dwVersionBuild = DVPROTOCOL_VERSION_BUILD; 
    
    hr = m_lpSessionTransport->SendToServer( pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr != DVERR_PENDING && hr != DV_OK )
    {
        DPVF( 0, "Error sending connect request hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );
    }
    // Pending = OK = expected
    else
    {
        hr = DV_OK;
    }
    
    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::Send_DisconnectRequest"
HRESULT CDirectVoiceClientEngine::Send_DisconnectRequest()
{
    PDVPROTOCOLMSG_GENERIC pDisconnectRequest;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    LPVOID pvSendContext;
    HRESULT hr; 

    DPVF_ENTER();
    pBufferDesc = GetTransmitBuffer(sizeof( DVPROTOCOLMSG_GENERIC ), &pvSendContext, FALSE );

    if( pBufferDesc == NULL )
    {
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY;       
    }

    pDisconnectRequest = (PDVPROTOCOLMSG_GENERIC) pBufferDesc->pBufferData;

    // Send connection request to the server
    pDisconnectRequest->dwType = DVMSGID_DISCONNECT;
    
    hr = m_lpSessionTransport->SendToServer( pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr != DVERR_PENDING && hr != DV_OK )
    {
        DPVF( 0, "Error sending connect request hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );
    }
    // Pending = OK = expected
    else
    {
        hr = DV_OK;
    }
    
    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::Send_SettingsConfirm"
HRESULT CDirectVoiceClientEngine::Send_SettingsConfirm()
{
    PDVPROTOCOLMSG_SETTINGSCONFIRM pSettingsConfirm;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    LPVOID pvSendContext;
    HRESULT hr; 

    DPVF_ENTER();
    pBufferDesc = GetTransmitBuffer(sizeof( DVPROTOCOLMSG_SETTINGSCONFIRM ), &pvSendContext, FALSE );

    if( pBufferDesc == NULL )
    {
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY;       
    }

    pSettingsConfirm = (PDVPROTOCOLMSG_SETTINGSCONFIRM) pBufferDesc->pBufferData;

    // Send connection request to the server
    pSettingsConfirm->dwType = DVMSGID_SETTINGSCONFIRM;
    pSettingsConfirm->dwHostOrderID = m_dwHostOrderID;
    pSettingsConfirm->dwFlags = 0;

    hr = m_lpSessionTransport->SendToServer( pBufferDesc, pvSendContext, DVTRANSPORT_SEND_GUARANTEED );

    if( hr != DVERR_PENDING && hr != DV_OK )
    {
        DPVF( 0, "Error sending connect request hr=0x%x", hr );
        ReturnTransmitBuffer( pvSendContext );
    }
    // Pending = OK = expected
    else
    {
        hr = DV_OK;
    }
    
    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::Disconnect"
// Disconnect
//
// Implements the IDirectXVoiceClient::Disconnect function
//
// Locks Required:
// - Global Lock
//
// Called By:
// DVC_Disconnect
//
HRESULT CDirectVoiceClientEngine::Disconnect( DWORD dwFlags )
{
    HRESULT hr;

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "Enter" );
    DPVF( DPVF_APIPARAM, "dwFlags = 0x%x", dwFlags );    

    DV_AUTO_LOCK(&m_CS);

    if( m_dwCurrentState == DVCSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not initialized" );
        DPVF_EXIT();
        return DVERR_NOTINITIALIZED;
    }

    if( m_dwCurrentState == DVCSTATE_CONNECTING )
    {
    
        DPVF(  DPVF_INFOLEVEL, "DVCE::Disconnect() Abort connection" );

        // Handle Connect
        m_hrConnectResult = DVERR_CONNECTABORTED;

        SendConnectResult();

        DoSignalDisconnect( DVERR_CONNECTABORTED );     

        DPVF( DPVF_INFOLEVEL, "Returning DVERR_CONNECTABORTING" );
    
        DPVF_EXIT();
        return DVERR_CONNECTABORTING;
    }

    DPVF( DPVF_INFOLEVEL, "State Good.." );   
    
    if( m_dwCurrentState == DVCSTATE_DISCONNECTING )
    {
        DPVF( DPVF_ERRORLEVEL, "Already disconnecting." );

        return DV_OK;
    }
    else if( m_dwCurrentState != DVCSTATE_CONNECTED )
    {
        DPVF(  DPVF_ERRORLEVEL, "Not Connected" );     
        DPVF( DPVF_APIPARAM, "Returning DVERR_NOTCONNECTED" );           
        DPVF_EXIT();
        return DVERR_NOTCONNECTED;
    }
    else
    {
        m_dwSynchBegin = GetTickCount();    

        // Set current state to disconnecting before we release the lock
        SetCurrentState( DVCSTATE_DISCONNECTING );      

        DPVF( DPVF_ERRORLEVEL, "Disconnect request about to be sent" );

        hr = Send_DisconnectRequest();

        DPVF( DPVF_INFOLEVEL, "Disconnect request transmitted hr=0x%x", hr );

        if( FAILED( hr ) )
        {
            DPVF(  DPVF_INFOLEVEL, "DVCE::Disconnect - Error on send 0x%x", hr );
            hr = DV_OK;

            return hr;

        }
        else
        {
            DPVF( DPVF_INFOLEVEL, "Disconnect sent" );
        }
    }

    DPVF( DPVF_INFOLEVEL, "Returning DVERR_PENDING" );

    DPVF_EXIT();

    return DVERR_PENDING;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::GetSessionDesc"
// GetSessionDesc
//
// Retrieves the current session description.
//
// Called By:
// DVC_GetSessionDesc
//
// Locks Required:
// - Global Read Lock
//
HRESULT CDirectVoiceClientEngine::GetSessionDesc( PDVSESSIONDESC lpSessionDesc )
{
    
    HRESULT hr;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    DPVF(  DPVF_ENTRYLEVEL, "Enter" );

    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "lpSessionDescBuffer = 0x%p", lpSessionDesc ); 
#ifndef ASSUME_VALID_PARAMETERS
    if( lpSessionDesc == NULL)
    {
        DPVF( DPVF_ERRORLEVEL, "Session desc pointer bad" );
        DPVF_EXIT();
        return DVERR_INVALIDPOINTER;
    }

    if( lpSessionDesc->dwSize != sizeof( DVSESSIONDESC ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid size on session desc" );
        DPVF_EXIT();
        return DVERR_INVALIDPARAM;
    }
#endif

    hr = CheckConnected();
    if( FAILED(hr))
    {
        DPVF( DPVF_ERRORLEVEL, "Object not initalized" );
        DPVF_EXIT();
        return hr;
    }

    memcpy( lpSessionDesc, &m_dvSessionDesc, sizeof( DVSESSIONDESC ) ); 

    DPVF( DPVF_APIPARAM, "Returning DV_OK" );

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::GetClientConfig"
// GetClientConfig
// 
// Retrieves the current client configuration.
//
// Called By:
// DVC_GetClientConfig
//
// Locks Required:
// - Global Read Lock
HRESULT CDirectVoiceClientEngine::GetClientConfig( LPDVCLIENTCONFIG lpClientConfig )
{
    HRESULT hr;
    DV_AUTO_LOCK(&m_CS);


    DPVF_ENTER();
    DPVF(  DPVF_ENTRYLEVEL, "Enter" );

    DPVF( DPVF_APIPARAM, "lpClientConfig = 0x%p", lpClientConfig );
#ifndef ASSUME_VALID_PARAMETERS
    if( lpClientConfig == NULL)
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid pointer" );
        DPVF_EXIT();
        return E_POINTER;
    }

    if( lpClientConfig->dwSize != sizeof( DVCLIENTCONFIG ) )
    {
        DPVF(  DPVF_ERRORLEVEL, "Invalid Size on clientconfig" );
        DPVF_EXIT();
        return DVERR_INVALIDPARAM;
    }
#endif
    
    hr = CheckConnected();
    if(FAILED(hr))
    {
        DPVF( DPVF_ERRORLEVEL, "Object not initialized" );
        DPVF_EXIT();
        return hr;
    }

    memcpy( lpClientConfig, &m_dvClientConfig, sizeof( DVCLIENTCONFIG ) );

    if( lpClientConfig->dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED )
    {
        lpClientConfig->dwThreshold = DVTHRESHOLD_UNUSED;
    }

    DV_DUMP_CC( lpClientConfig );

    DPVF(  DPVF_ENTRYLEVEL, "End" );
    DPVF( DPVF_APIPARAM, "Returning DV_OK" );

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SetClientConfig"
// SetClientConfig
//
// Sets the current client configuration.
//
// Called By:
// DVC_SetClientConfig
//
// Locks Required:
// - Global Write Lock
//
HRESULT CDirectVoiceClientEngine::SetClientConfig( LPDVCLIENTCONFIG lpClientConfig )
{
    HRESULT hr;
    BOOL bPlaybackChange = FALSE,
         bRecordChange = FALSE,
         bSensitivityChange = FALSE;


    DV_AUTO_LOCK(&m_CS);
     

    DPVF_ENTER();
    DPVF(  DPVF_ENTRYLEVEL, "Enter" );
    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "lpClientConfig=0x%p", lpClientConfig );

#ifndef ASSUME_VALID_PARAMETERS
    if(lpClientConfig == NULL)
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid pointer" );
        DPVF_EXIT();
        return E_POINTER;
    }

    DV_DUMP_CC( lpClientConfig );

    if( lpClientConfig->dwSize != sizeof( DVCLIENTCONFIG ) )
    {
        DPVF(  DPVF_ERRORLEVEL, "DVCE::SetClientConfig() Error parameters" );
        DPVF_EXIT();
        return DVERR_INVALIDPARAM;
    }

    hr = DV_ValidClientConfig( lpClientConfig );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error validating Clientconfig hr=0x%x", hr );
        DPVF_EXIT();
        return hr;
    }       

#endif

    hr = CheckConnected();
    if(FAILED(hr))
    {
        DPVF_EXIT();
        return hr;
    }

    if( !(lpClientConfig->dwFlags & DVCLIENTCONFIG_MANUALVOICEACTIVATED ) )
    {
        m_dvClientConfig.dwThreshold = DVTHRESHOLD_UNUSED;
    }
    else if( m_dvClientConfig.dwThreshold != lpClientConfig->dwThreshold )
    {
        if( lpClientConfig->dwThreshold == DVTHRESHOLD_DEFAULT )
        {
            m_dvClientConfig.dwThreshold = s_dwDefaultSensitivity;
        }
        else
        {
            m_dvClientConfig.dwThreshold = lpClientConfig->dwThreshold;
        }
    }

#if DBG
    // If the settings have changed
    if( (m_dvClientConfig.dwFlags & DVCLIENTCONFIG_RECORDMUTE) !=
       (lpClientConfig->dwFlags & DVCLIENTCONFIG_RECORDMUTE) )
    {
        if( lpClientConfig->dwFlags & DVCLIENTCONFIG_RECORDMUTE )
        {
            DPVF( DPVF_INFOLEVEL, "Record Muted: Yielding focus" );
        }
        else
        {
            DPVF( DPVF_INFOLEVEL, "Record Un-Muted: Attempting to reclaim focus" );          
        }
    }
#endif

    //
    // if we were half duplex, and now theya re setting us to full duplex
    // we need to init the record subsystem
    //

    if((m_dvClientConfig.dwFlags & DVCLIENTCONFIG_HALFDUPLEX ) &&
       !(lpClientConfig->dwFlags & DVCLIENTCONFIG_HALFDUPLEX) )
    {

        //
        // remove half duplex flag
        //

        m_dvClientConfig.dwFlags &= ~DVCLIENTCONFIG_HALFDUPLEX;

        hr = InitializeSoundSystem();
        if (FAILED(hr)) {

            //
            // we failed, back to half duplex
            //

            m_dvClientConfig.dwFlags |= DVCLIENTCONFIG_HALFDUPLEX;

            DPVF_EXIT();
            return hr;

        }

    }

    m_dvClientConfig.dwFlags = lpClientConfig->dwFlags;
    
    DPVF(  DPVF_ENTRYLEVEL, "Returning DV_OK" );

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::CheckForAndRemoveTarget"
//
// CheckForAndRemoveTarget
//
// Checks the current target list for the specified ID and removes it from
// the target list if it is in the target list.
//
HRESULT CDirectVoiceClientEngine::CheckForAndRemoveTarget( DVID dvidID )
{
    HRESULT hr = DV_OK;
    
    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    //
    // Search the list of targets
    //

    for( DWORD dwIndex = 0; dwIndex < m_dwNumTargets; dwIndex++ )
    {
        if( m_pdvidTargets[dwIndex] == dvidID )
        {
            if( m_dwNumTargets == 1 )
            {
                hr = InternalSetTransmitTarget( NULL, 0 );
            }
            // We'll re-use the current target array
            else
            {
                // Collapse the list by either ommiting the last element (if the
                // one we want to remove is last, or by moving last element into
                // the place in the list we're removing.
                if( dwIndex+1 != m_dwNumTargets )
                {
                    m_pdvidTargets[dwIndex] = m_pdvidTargets[m_dwNumTargets-1];
                }
                
                hr = InternalSetTransmitTarget( m_pdvidTargets, m_dwNumTargets-1 );
            }
            
            break;
        }
    }

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::InternalSetTransmitTarget"
//
// InternalSetTransmitTarget
//
// Does the work of setting the target.  (Assumes values have been validated).
//
// This function is safe to pass a pointer to the current target array.  It works
// on a temporary.
// 
//
HRESULT CDirectVoiceClientEngine::InternalSetTransmitTarget( PDVID pdvidTargets, DWORD dwNumTargets )
{
    DWORD dwRequiredSize;
    PBYTE pbDataBuffer;
    PDVMSG_SETTARGETS pdvSetTarget;
    

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);
    // No targets? set list to NULL
    if( dwNumTargets == 0 )
    {
        // Close memory leak
        //
        // Hawk Bug #
        //
        if( m_pdvidTargets != NULL )
        {
            DV_POOL_FREE (m_pdvidTargets);
        }

        m_pdvidTargets = NULL;      
    }
    // Otherwise allocate new list and copy
    else
    {
        PDVID pTmpTargetList;
        
        pTmpTargetList = (DVID *) DV_POOL_ALLOC(sizeof(DVID)*dwNumTargets);

        if( pTmpTargetList == NULL )
        {
            DV_POOL_FREE (m_pdvidTargets);
            m_dwNumTargets = 0;
            DPVF( DPVF_ERRORLEVEL, "Memory alloc failure" );
            DPVF_EXIT();
            return DVERR_OUTOFMEMORY;
        }

        memcpy( pTmpTargetList, pdvidTargets, dwNumTargets*sizeof(DVID) );

        // Kill off old target list
        if( m_pdvidTargets != NULL )
            DV_POOL_FREE(m_pdvidTargets);   

        m_pdvidTargets = pTmpTargetList;
    }

    m_dwNumTargets = dwNumTargets;
    m_dwTargetVersion++;

    dwRequiredSize  = m_dwNumTargets * sizeof( DVID );
    dwRequiredSize += sizeof( DVMSG_SETTARGETS );

    pbDataBuffer = (PUCHAR)DV_POOL_ALLOC(dwRequiredSize);

    if( pbDataBuffer == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Error allocating memory!" );
        DPVF_EXIT();
        return TRUE;
    }

    pdvSetTarget = (PDVMSG_SETTARGETS) pbDataBuffer;

    pdvSetTarget->pdvidTargets = (PDVID) (pbDataBuffer+sizeof(DVMSG_SETTARGETS));
    pdvSetTarget->dwNumTargets = m_dwNumTargets;
    pdvSetTarget->dwSize = sizeof( DVMSG_SETTARGETS );

    memcpy( pdvSetTarget->pdvidTargets, m_pdvidTargets, sizeof(DVID)*m_dwNumTargets );

    NotifyQueue_Add(  DVMSGID_SETTARGETS, pdvSetTarget, dwRequiredSize );   

    DV_POOL_FREE(pbDataBuffer);

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SetTransmitTarget"
//
// SetTransmitTarget
//
// Sets the current transmit target.
//
// Called by:
// - DVC_SetTransmitTarget
//
// Locks Required:
// - Global Write Lock
//
HRESULT CDirectVoiceClientEngine::SetTransmitTarget( PDVID pdvidTargets, DWORD dwNumTargets, DWORD dwFlags )
{
    HRESULT hr;
    DWORD dwIndex;  
    
    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "Begin" );
    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "Params: pdvidTargets = 0x%p dwNumTargets = %d dwFlags = 0x%x", pdvidTargets, dwNumTargets, dwFlags );

#ifndef ASSUME_VALID_PARAMETERS
    if( dwFlags != 0 )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid flags" );
        DPVF_EXIT();
        return DVERR_INVALIDFLAGS;
    }

    // Check that the target list is valid
    hr = DV_ValidTargetList( pdvidTargets, dwNumTargets );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Target list is not valid" );
        DPVF_EXIT();
        return hr;
    }

#endif

    hr = CheckConnected();
    if(FAILED(hr))
    {

        if(m_dwCurrentState != DVCSTATE_CONNECTING) {
           DPVF( DPVF_ERRORLEVEL, "Not connected" );
           DPVF_EXIT();
           return hr;
        }

    } 
    else
    {

        if( m_dvSessionDesc.dwFlags & DVSESSION_SERVERCONTROLTARGET )
        {
            DPVF( DPVF_ERRORLEVEL, "Denied.  Server controlled target" );
            DPVF_EXIT();
            return DVERR_NOTALLOWED;
        }

        if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER )
        {

            // Loop through target list, confirm they are valid entries
            for( dwIndex = 0; dwIndex < dwNumTargets; dwIndex++ )
            {
                if( !m_voiceNameTable.IsEntry(pdvidTargets[dwIndex]) )
                {

                    if( !m_lpSessionTransport->ConfirmValidGroup( pdvidTargets[dwIndex] ) )
                    {
                        DPVF( DPVF_ERRORLEVEL, "Invalid target" );
                        return DVERR_INVALIDTARGET;
                    }
                    
                } 
            }

        }

    }

    hr = InternalSetTransmitTarget( pdvidTargets, dwNumTargets );

    DPVF( DPVF_ENTRYLEVEL, "Done" );

    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::GetTransmitTarget"
//
// GetTransmitTarget
//
// Retrieves the current transmission target.  
//
// Called By:
// - DVC_GetTransmitTarget
//
// Locks Required: 
// - Read Lock Required
//
HRESULT CDirectVoiceClientEngine::GetTransmitTarget( LPDVID lpdvidTargets, PDWORD pdwNumElements, DWORD dwFlags )
{
    HRESULT hr = DV_OK;
    
    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "Begin" );
    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "Params: lpdvidTargets = 0x%p pdwNumElements = 0x%x dwFlags = 0x%x", lpdvidTargets, pdwNumElements, dwFlags );
#ifndef ASSUME_VALID_PARAMETERS
    if( pdwNumElements == NULL)
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid pointer to num of elements" );
        DPVF_EXIT();
        return DVERR_INVALIDPOINTER;
    }

    if(pdwNumElements != NULL && 
       *pdwNumElements > 0)
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid target list buffer specified" );
        DPVF_EXIT();
        return DVERR_INVALIDPOINTER;
    }

    if( pdwNumElements == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Must specify a ptr for # of elements" );
        DPVF_EXIT();
        return DVERR_INVALIDPARAM;
    }
    
    if( dwFlags != 0 )
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid flags" );
        return DVERR_INVALIDFLAGS;
    }
#endif

    hr = CheckConnected();
    if(FAILED(hr))
    {

        DPVF( DPVF_ERRORLEVEL, "error hr = %x",hr );
        DPVF_EXIT();
        return hr;
    }

    if( *pdwNumElements < m_dwNumTargets )
    {
        hr = DVERR_BUFFERTOOSMALL;
    }
    else
    {
        memcpy( lpdvidTargets, m_pdvidTargets,m_dwNumTargets*sizeof(DVID) );
    }
    
    *pdwNumElements = m_dwNumTargets;

    DPVF(  DPVF_ENTRYLEVEL, "Success" );

    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::FindSoundTarget"
//
// FindSoundTarget
//
// Look for sound target buffer for the specified user.
//
// If it exists, return it in lpcsTarget
//
// Called By:
// - CreateUserBuffer
//
// called at dpc
//
HRESULT CDirectVoiceClientEngine::FindSoundTarget( DVID dvidID, CSoundTarget **lpcsTarget, CVoicePlayer **ppPlayer)
{
    HRESULT hr;
    CVoicePlayer *p;
    
    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    hr = m_voiceNameTable.GetEntry( dvidID, ppPlayer, TRUE );

    p = *ppPlayer;

    if (FAILED(hr)) {

        ASSERT(*ppPlayer == NULL);
        DPVF_EXIT();
        return DVERR_INVALIDPLAYER;
    }

    //
    // we found the remote player, see if they have a SoundTarget
    //

    p->GetSoundTarget(lpcsTarget);

    if (*lpcsTarget!=NULL) {
        DPVF_EXIT();
        return DV_OK;
    }

    DPVF_EXIT();
    return DVERR_GENERIC;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::CreateSoundTarget"
//
// Create3DSoundBuffer
//
// Creates a mixing buffer (a sound target) for the specified user ID.
//
// Called By:
// - DVC_CreateUserBuffer
//
// Locks Required:
// - Global Read Lock
//
HRESULT CDirectVoiceClientEngine::CreateSoundTarget( DVID dvidID, PWAVEFORMATEX *ppwfx, XMediaObject **ppMediaObject)
{
    HRESULT hr;     
    LONG lResult;
    CSoundTarget *lpstTarget = NULL;
    CVoicePlayer *pPlayer;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "Begin" );
    DPVF( DPVF_APIPARAM, "Params: dvidID = 0x%x", dvidID);

    ASSERT(ppMediaObject);

#ifndef ASSUME_VALID_PARAMETERS

    if( dvidID == m_dvidLocal )
    {
        DPVF( DPVF_ERRORLEVEL, "Cannot create buffer for local player!" );
        DPVF_EXIT();
        return DVERR_INVALIDPLAYER;
    }

#endif

    if( m_dwCurrentState == DVCSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not initialized" );
        return DVERR_NOTINITIALIZED;
    }
    else if( m_dwCurrentState != DVCSTATE_CONNECTED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not connected" );
        return DVERR_NOTCONNECTED;
    }

    if( dvidID != DVID_ALLPLAYERS && 
        dvidID != DVID_REMAINING && 
        !m_voiceNameTable.IsEntry(dvidID)   )
    {
        if( !m_lpSessionTransport->ConfirmValidGroup( dvidID ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Invalid player/group ID" );
            
            return DVERR_INVALIDPLAYER;
        }

    }    

    if( dvidID == DVID_REMAINING )
    {
        //
        // dont support this mode.
        // if they want to mix in data
        // they can open the hawk or MCPX output device
        // and create a sound buffer on it
        //
        DPVF_EXIT();
        return DVERR_NOTSUPPORTED;
    }

    // Check for existing buffer.. if it already exists return it
    // (Note: Adds a reference to the player)
    hr = FindSoundTarget( dvidID, &lpstTarget, &pPlayer );

    if( hr == DV_OK )
    {
        pPlayer->Release();

        DPVF( DPVF_ERRORLEVEL, "CreateUserBuffer: Find of buffer failed. hr=0x%x", hr );
        DPVF_EXIT();
        return DVERR_ALREADYBUFFERED;

    } else if (hr == DVERR_INVALIDPLAYER) {
        return hr;
    }

    DPVF( DPVF_INFOLEVEL, "Creating buffer using user buffer" );

    hr = pPlayer->CreateSoundTarget(&lpstTarget);

    if (FAILED(hr)) {

        pPlayer->Release();

        DPVF( DPVF_ERRORLEVEL, "CreateUserBuffer: Failed allocating sound target" );
        return hr;
    }

    lpstTarget->AddRef();
    *ppMediaObject = (XMediaObject *) lpstTarget;

    //
    // tell them the format to expect audio data in
    //

    if (ppwfx) {

        *ppwfx = s_lpwfxPrimaryFormat;

    }

    DPVF( DPVF_ENTRYLEVEL, "Done" );

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::DeleteSoundTarget"
//
// Delete3DSoundBuffer
//
// Removes the specified ID from the mixer buffer list.  Further speech from
// the specified player will be played in the remaining buffer.
//
// Called By:
// - DVC_DeleteUserBuffer
//
// Locks Required:
// - Global Write Lock
//
HRESULT CDirectVoiceClientEngine::DeleteSoundTarget( DVID dvidID, XMediaObject **ppMediaObject )
{
    HRESULT hr; 
    DWORD dwMode;
    LONG lResult;
    CSoundTarget *lpstTarget;
    CVoicePlayer *pPlayer;    

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "Begin" );
    // 7/31/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
    DPVF( DPVF_APIPARAM, "Params: dvidID = 0x%x ppMediaObj = 0x%p", dvidID, ppMediaObject );
    
#ifndef ASSUME_VALID_PARAMETERS
    if( ppMediaObject == NULL)
    {
        DPVF( DPVF_ERRORLEVEL, "Invalid pointer" );
        return E_POINTER;
    }
    
#endif

    if( m_dwCurrentState == DVCSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not initialized" );
        return DVERR_NOTINITIALIZED;
    }   
    else if( m_dwCurrentState != DVCSTATE_CONNECTED &&
             m_dwCurrentState != DVCSTATE_DISCONNECTING )
    {
        DPVF( DPVF_ERRORLEVEL, "Not connected" );
        return DVERR_NOTCONNECTED;
    }

    // Handle request to disable 3D on the main buffer
    if( dvidID == DVID_REMAINING )
    {
        DPVF_EXIT();
        return DVERR_NOTSUPPORTED;
    }   

    hr = FindSoundTarget( dvidID, &lpstTarget, &pPlayer );

    if( FAILED( hr ) )
    {

        if(hr != DVERR_INVALIDPLAYER)
        {
            pPlayer->Release();
            DPVF( DPVF_ERRORLEVEL, "Unable to retrieve sound target" );
            DPVF_EXIT();
            return DVERR_NOTBUFFERED;
        }

        DPVF( DPVF_ERRORLEVEL, "find of player failed. hr=0x%x", hr );
        DPVF_EXIT();
        return hr;
    }

    pPlayer->GetSoundTarget(&lpstTarget) ;
    if( lpstTarget != *ppMediaObject )
    {
        DPVF( DPVF_ERRORLEVEL, "Buffer passed in does not belong to specified id" );     
        pPlayer->Release();
        return DVERR_INVALIDPARAM;
    }

    DPVF( DPVF_SOUNDTARGET_DEBUG_LEVEL, "SOUNDTARGET: [0x%x] DESTROY3DBUFFER ", dvidID );

    // Get rid of the initial reference CreateSoundTarget added
    lResult = lpstTarget->Release();

    // remove from player context
    pPlayer->DeleteSoundTarget();
    pPlayer->Release();

    *ppMediaObject = NULL;

    DPVF( DPVF_ENTRYLEVEL, "Done" );

    DPVF_EXIT();
    return DV_OK;
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DIRECTPLAY/NET --> DirectXVoiceClient Interface
//

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::ReceiveSpeechMessage"
// ReceiveSpeechMessage
//
// Called by DirectPlay/DirectNet when a DirectXVoice message is received
//
// Called By:
// - DV_ReceiveSpeechMessage
//
// Locks Required:
// - None
//
BOOL CDirectVoiceClientEngine::ReceiveSpeechMessage( DVID dvidSource, LPVOID lpMessage, DWORD dwSize )
{
    BOOL fResult;
    PDVPROTOCOLMSG_FULLMESSAGE lpdvFullMessage;

    DPVF_ENTER();
    ASSERT( lpMessage );
    ASSERT( dwSize > 0 );

    lpdvFullMessage = (PDVPROTOCOLMSG_FULLMESSAGE) lpMessage;

    switch( lpdvFullMessage->dvGeneric.dwType )
    {
    case DVMSGID_HOSTMIGRATELEAVE:
        fResult = HandleHostMigrateLeave( dvidSource, static_cast<PDVPROTOCOLMSG_HOSTMIGRATELEAVE>(lpMessage), dwSize );
        break;
    case DVMSGID_HOSTMIGRATED:
        fResult = HandleHostMigrated( dvidSource, static_cast<PDVPROTOCOLMSG_HOSTMIGRATED>(lpMessage),dwSize );
        break;
    case DVMSGID_CONNECTREFUSE:
        fResult = HandleConnectRefuse( dvidSource, static_cast<PDVPROTOCOLMSG_CONNECTREFUSE>(lpMessage), dwSize );
        break;
    case DVMSGID_CONNECTACCEPT:
        fResult = HandleConnectAccept( dvidSource, static_cast<PDVPROTOCOLMSG_CONNECTACCEPT>(lpMessage), dwSize );
        break;
    case DVMSGID_CREATEVOICEPLAYER:
        fResult = HandleCreateVoicePlayer( dvidSource, static_cast<PDVPROTOCOLMSG_PLAYERJOIN>(lpMessage), dwSize );
        break;
    case DVMSGID_DELETEVOICEPLAYER:
        fResult = HandleDeleteVoicePlayer( dvidSource, static_cast<PDVPROTOCOLMSG_PLAYERQUIT>(lpMessage), dwSize );
        break;
    case DVMSGID_SPEECH:
        fResult = HandleSpeech( dvidSource, static_cast<PDVPROTOCOLMSG_SPEECHHEADER>(lpMessage), dwSize );
        break;
    case DVMSGID_SPEECHBOUNCE:
        fResult = HandleSpeechBounce( dvidSource, static_cast<PDVPROTOCOLMSG_SPEECHHEADER>(lpMessage), dwSize );
        break;
    case DVMSGID_SPEECHWITHFROM:
        fResult = HandleSpeechWithFrom( dvidSource, static_cast<PDVPROTOCOLMSG_SPEECHWITHFROM>(lpMessage), dwSize );
        break;
    case DVMSGID_DISCONNECTCONFIRM:
        fResult = HandleDisconnectConfirm( dvidSource, static_cast<PDVPROTOCOLMSG_DISCONNECT>(lpMessage), dwSize);
        break;
    case DVMSGID_SETTARGETS:
        fResult = HandleSetTarget( dvidSource, static_cast<PDVPROTOCOLMSG_SETTARGET>(lpMessage), dwSize );
        break;
    case DVMSGID_SESSIONLOST:
        fResult = HandleSessionLost( dvidSource, static_cast<PDVPROTOCOLMSG_SESSIONLOST>(lpMessage), dwSize );
        break;
    case DVMSGID_PLAYERLIST:
        fResult = HandlePlayerList( dvidSource, static_cast<PDVPROTOCOLMSG_PLAYERLIST>(lpMessage), dwSize );
        break;
    default:
        DPVF(  DPVF_WARNINGLEVEL, "DVCE::ReceiveSpeechMessage() Ignoring non-speech message id=0x%x from=0x%x", 
             lpdvFullMessage->dvGeneric.dwType, dvidSource );
        DPVF_EXIT();
        return FALSE;
    }

    DPVF_EXIT();
    return fResult;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleSetTarget"
// HandleSetTarget
//
// Handles server settarget messages.  Sets the local target.
//
BOOL CDirectVoiceClientEngine::HandleSetTarget( DVID dvidSource, PDVPROTOCOLMSG_SETTARGET lpdvSetTarget, DWORD dwSize )
{
    HRESULT hr;

    DPVF_ENTER();
    hr = InternalSetTransmitTarget( (DWORD *) &lpdvSetTarget[1], lpdvSetTarget->dwNumTargets );

    ASSERT( hr == DV_OK );

    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleHostMigrateLeave"
BOOL CDirectVoiceClientEngine::HandleHostMigrateLeave( DVID dvidSource, PDVPROTOCOLMSG_HOSTMIGRATELEAVE lpdvHostMigrateLeave, DWORD dwSize )
{
    // Call RemovePlayer with the ID of the person who sent this, 
    // which should be the host.
    
    DPVF_ENTER();
    DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: Triggered by DVMSGID_HOSTMIGRATELEAVE" );
    MigrateHost_RunElection();

    DPVF_EXIT();
    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleHostMigrated"
BOOL CDirectVoiceClientEngine::HandleHostMigrated( DVID dvidSource, PDVPROTOCOLMSG_HOSTMIGRATED lpdvHostMigrated, DWORD dwSize )
{
    HRESULT hr;    

    // We're not yet connected, so we can't send our settings to the server yet.
    // However, because of the write lock we know that when the connection
    // completes we'll have the right host.  (Transparently to the client).
    //

    DPVF_ENTER();

    DV_AUTO_LOCK(&m_CS);
    hr = CheckConnected();
    if( FAILED(hr))
    {
        DPVF( DPVF_WARNINGLEVEL, "Could not respond to new host yet, not yet initialized" );
        DPVF_EXIT();
        return TRUE;
    }

    if( dvidSource != m_dvidServer )
    {
        DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: Ignoring host migration from 0x%x -- 0x%x is server", dvidSource, m_dvidServer );
        DPVF_EXIT();
        return TRUE;
    }

    DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: Player 0x%x received host migrated message!", m_dvidLocal );
    DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: Sending player confirm message" );
    DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: According to message Player 0x%x is new host", dvidSource );

    hr = Send_SettingsConfirm();

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Internal send failed hr=0x%x", hr );
    }
    else
    {
        hr=DV_OK;
    }

    DPVF_EXIT();
    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleConnectRefuse"
// HandleConnectRefuse
//
// Handles connection refusals
//
BOOL CDirectVoiceClientEngine::HandleConnectRefuse( DVID dvidSource, PDVPROTOCOLMSG_CONNECTREFUSE lpdvConnectRefuse, DWORD dwSize )
{   
    
    ASSERT( dwSize == sizeof( DVPROTOCOLMSG_CONNECTREFUSE ) );

    DPVF_ENTER();

    DV_AUTO_LOCK(&m_CS);
    m_hrConnectResult = lpdvConnectRefuse->hresResult;
    DPVF_EXIT();
    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::DoConnectResponse"
void CDirectVoiceClientEngine::DoConnectResponse()
{
    HANDLE hThread;
    HRESULT hr;
    

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    if( m_dwCurrentState != DVCSTATE_CONNECTING )
    {
        DPVF( DPVF_WARNINGLEVEL, "Aborting Connection & server response arrived" );
        DPVF_EXIT();
        return;
    }

    // Handle sound initialization
    m_hrConnectResult = InitializeSoundSystem();

    if( m_hrConnectResult != DV_OK )
    {
        DPVF( DPVF_ERRORLEVEL, "Sound Initialization Failed hr=0x%x", m_hrConnectResult );
        goto EXIT_ERROR;
    }

    m_hrConnectResult = SetupSpeechBuffer();

    if( FAILED( m_hrConnectResult ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Could not init speech buffers hr=0x%x", m_hrConnectResult );
        goto EXIT_ERROR;
    }

    DPVF(  DPVF_INFOLEVEL, "DVCE::HandleConnectAccept() - Sound Initialized" );

    // We need to make player available in client/server because there will be no indication
    if( m_dvSessionDesc.dwSessionType != DVSESSIONTYPE_PEER )
    {
        m_fLocalPlayerAvailable = TRUE;
    }

    SetCurrentState( DVCSTATE_CONNECTED );

    DPVF(  DPVF_ENTRYLEVEL, "DVCE::HandleConnectAccept() Success" );

    SendConnectResult();    

    hr = Send_SettingsConfirm();

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Failed to send connect confirmation hr=0x%x", hr );
        DPVF( DPVF_ERRORLEVEL, "Other threads will cleanup because connection must be gone" );
    }

    DPVF_EXIT();
    return;

EXIT_ERROR:

    DPVF( DPVF_ERRORLEVEL, "Initialize Failed hr=0x%x", m_hrConnectResult );
    DV_DUMP_GUID( m_dvSessionDesc.guidCT );

    SendConnectResult();
    
    Cleanup();
    
    DPVF_EXIT();
    return;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleConnectAccept"
// HandleConnectAccepts
//
// Handles connect accepts.  Sets connected flag, finishes initialization, informs the 
// connect function to proceed (if it's waiting).
//
BOOL CDirectVoiceClientEngine::HandleConnectAccept( DVID dvidSource, PDVPROTOCOLMSG_CONNECTACCEPT lpdvConnectAccept, DWORD dwSize )
{
    char tmpString[100];
    
    DVPROTOCOLMSG_FULLMESSAGE dvMessage;
    WAVEFORMATEX wfx;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    ASSERT( dwSize >= sizeof( DVPROTOCOLMSG_CONNECTACCEPT ) );

    DPVF(  DPVF_ENTRYLEVEL, "DVCE::HandleConnectAccept() Entry" );


    // We're already connected, server is responding to the earlier request
    if( m_dwCurrentState != DVCSTATE_CONNECTING )
    {
        DPVF_EXIT();
        return TRUE;
    }

    // Inform transport layer who the server is. (So it no longer thinks it's DPID_ALL).
    m_lpSessionTransport->MigrateHost( dvidSource );
    m_dvidServer = m_lpSessionTransport->GetServerID();
    
    m_hrConnectResult = DV_OK;

    DPVF(  DPVF_INFOLEVEL, "Connect Accept Received" );

    m_dvSessionDesc.dwSize = sizeof( DVSESSIONDESC );
    m_dvSessionDesc.dwBufferAggressiveness = 0;
    m_dvSessionDesc.dwBufferQuality = 0;
    m_dvSessionDesc.dwFlags = lpdvConnectAccept->dwSessionFlags;
    m_dvSessionDesc.guidCT = lpdvConnectAccept->guidCT;
    m_dvSessionDesc.dwSessionType = lpdvConnectAccept->dwSessionType;

    //
    // initialize decompressor
    //

    //
    // create an inbound converter. We will later give it to the first voice player we see
    // we need it now to get compression parameters
    //

    memcpy(&wfx, s_lpwfxPrimaryFormat,sizeof(wfx));
    m_hrConnectResult = DV_CreateConverter(m_dvSessionDesc.guidCT,
                            &m_pInBoundAudioConverter,
                            &wfx,
                            NULL,
                            0,
                            FALSE);

    //
    // extract info from converter
    //

    BuildCompressionInfo();

    if( FAILED(m_hrConnectResult ) )
    {

        DPVF( DPVF_ERRORLEVEL, "Invalid Compression Type" );

        SendConnectResult();
        Cleanup();                  

    }
    else
    {
        DoConnectResponse();
    }

    DPVF_EXIT();
    return TRUE;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandlePlayerList"
BOOL CDirectVoiceClientEngine::HandlePlayerList( DVID dvidSource, PDVPROTOCOLMSG_PLAYERLIST lpdvPlayerList, DWORD dwSize )
{
    DVPROTOCOLMSG_PLAYERJOIN dvMsgPlayerJoin;       // Used to fake out HandleCreateVoicePlayer
    DWORD dwIndex;
    DVPROTOCOLMSG_PLAYERLIST_ENTRY *pdvPlayerList = (DVPROTOCOLMSG_PLAYERLIST_ENTRY *) &lpdvPlayerList[1];
    

    DPVF_ENTER();
    // Check the size
    ASSERT( ((dwSize - sizeof( DVPROTOCOLMSG_PLAYERLIST )) % sizeof( DVPROTOCOLMSG_PLAYERLIST_ENTRY )) == 0 );

    DV_AUTO_LOCK(&m_CS);

    // Get our host order ID
    m_dwHostOrderID = lpdvPlayerList->dwHostOrderID;

    DPVF( DPVF_INFOLEVEL, "Received player list.  Unpacking %d entries", lpdvPlayerList->dwNumEntries );

    for( dwIndex = 0; dwIndex < lpdvPlayerList->dwNumEntries; dwIndex++ )
    {
        dvMsgPlayerJoin.dwType = DVMSGID_CREATEVOICEPLAYER;
        dvMsgPlayerJoin.dvidID =  pdvPlayerList[dwIndex].dvidID;
        dvMsgPlayerJoin.dwFlags = pdvPlayerList[dwIndex].dwPlayerFlags;
        dvMsgPlayerJoin.dwHostOrderID = pdvPlayerList[dwIndex].dwHostOrderID;

        if( !HandleCreateVoicePlayer( dvidSource, &dvMsgPlayerJoin, sizeof( DVPROTOCOLMSG_PLAYERJOIN ) ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Handle voice player failed during unpack" );
            
            DPVF_EXIT();
            return FALSE;
        }
    }

    DPVF_EXIT();
    return TRUE;
    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyComplete_RemotePlayer"
//
// NotifyComplete_RemotePlayer
//
// This is a completion function for when notification of a new remote player has been processed
//
void CDirectVoiceClientEngine::NotifyComplete_RemotePlayer( PVOID pvContext, CNotifyElement *pElement )
{
    CVoicePlayer *pPlayer = (CVoicePlayer *) pvContext;
    PDVMSG_CREATEVOICEPLAYER pCreatePlayer = (PDVMSG_CREATEVOICEPLAYER) pElement->m_element.Data;

    ASSERT( pPlayer != NULL );
    ASSERT( pCreatePlayer->dwSize = sizeof( DVMSG_CREATEVOICEPLAYER ) );

    pPlayer->SetContext( pCreatePlayer->pvPlayerContext );
    pPlayer->SetAvailable( TRUE );
    
    pPlayer->Release();
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyComplete_DeletePlayer"
//
// NotifyComplete_RemotePlayer
//
// This is a completion function for when notification of a new remote player has been processed
//
void CDirectVoiceClientEngine::NotifyComplete_DeletePlayer( PVOID pvContext, CNotifyElement *pElement )
{
    CVoicePlayer *pPlayer = (CVoicePlayer *) pvContext;
    PDVMSG_DELETEVOICEPLAYER pDeletePlayer = (PDVMSG_DELETEVOICEPLAYER) pElement->m_element.Data;

    ASSERT( pPlayer != NULL );
    ASSERT( pDeletePlayer->dwSize = sizeof( DVMSG_DELETEVOICEPLAYER ) );

    pPlayer->SetContext( pDeletePlayer->pvPlayerContext );
    pPlayer->SetAvailable( TRUE );
    
    pPlayer->Release();
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyComplete_LocalPlayer"
//
// NotifyComplete_LocalPlayer
//
// This is a completion function for when notification of the local player has been processed
//
void CDirectVoiceClientEngine::NotifyComplete_LocalPlayer( PVOID pvContext, CNotifyElement *pElement )
{
    CDirectVoiceClientEngine *pvEngine = (CDirectVoiceClientEngine *) pvContext;
    PDVMSG_CREATEVOICEPLAYER pCreatePlayer = (PDVMSG_CREATEVOICEPLAYER) pElement->m_element.Data;

    ASSERT( pCreatePlayer->dwSize = sizeof( DVMSG_CREATEVOICEPLAYER ) );

    pvEngine->m_pvLocalPlayerContext = pCreatePlayer->pvPlayerContext;
    pvEngine->m_fLocalPlayerAvailable = TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleCreateVoicePlayer"
//
// HandleCreateVoicePlayer
//
// Performs initialization required to create the specified user's record.
//
// 
// When a player is added they are added to the nametable as well as the
// pending lists for notify thread and playback thread.
//
// Both of these threads wakeup and:
// - Add any players on the "add list"
// - Remove any players who are marked disconnecting
//
BOOL CDirectVoiceClientEngine::HandleCreateVoicePlayer( DVID dvidSource, PDVPROTOCOLMSG_PLAYERJOIN lpdvCreatePlayer, DWORD dwSize )
{
    CVoicePlayer *pNewPlayer;
    HRESULT hr;
    
    QUEUE_PARAMS queueParams;
    WAVEFORMATEX wfx;

    DPVF_ENTER();

    DV_AUTO_LOCK(&m_CS);

    ASSERT( dwSize == sizeof( DVPROTOCOLMSG_PLAYERJOIN ) );

    hr = CheckConnected();
    if( FAILED(hr))
    {
        DPVF_EXIT();
        return TRUE;
    }


    hr = m_voiceNameTable.GetEntry( lpdvCreatePlayer->dvidID, &pNewPlayer, TRUE );

    // Ignore duplicate players
    if( hr == DV_OK )
    {
        pNewPlayer->Release();

        DPVF_EXIT();
        return TRUE;
    }

    DPVF( DPVF_INFOLEVEL, "Received Create for player ID 0x%x",lpdvCreatePlayer->dvidID );

    // Do not create a queue or a player entry for ourselves
    // Not needed
    if( lpdvCreatePlayer->dvidID != m_lpSessionTransport->GetLocalID() )
    {
        PLIST_ENTRY pEntry;           
        
        pEntry = RemoveTailList(&m_PlayerContextPool);
        pNewPlayer = (CVoicePlayer *) CONTAINING_RECORD(pEntry, CVoicePlayer, m_PoolEntry);        

        DPVF( DPVF_INFOLEVEL, "Creating player record %x",pNewPlayer );

        ASSERT(pNewPlayer);

        hr = pNewPlayer->Initialize(lpdvCreatePlayer->dvidID,
                                    lpdvCreatePlayer->dwHostOrderID,
                                    m_dvClientConfig.dwPort,
                                    lpdvCreatePlayer->dwFlags,
                                    NULL,
                                    &m_PlayerContextPool);


        if( FAILED( hr ) )
        {
            pNewPlayer->Release();
            DPVF( DPVF_ERRORLEVEL, "Unable to initialize player record hr=0x%x", hr );
            DPVF_EXIT();

            return TRUE;
        }
    
        //
        // create our queue
        //
    
        queueParams.wFrameSize = (USHORT)s_dwCompressedFrameSize;
        queueParams.bInnerQueueSize = DV_CT_INNER_QUEUE_SIZE;
        queueParams.wQueueId = lpdvCreatePlayer->dvidID;
        queueParams.wMSPerFrame = (WORD) (1000 * (WORD)s_dwCompressedFrameSize) / 
            ((WORD)s_lpwfxPrimaryFormat->nSamplesPerSec * 2);

        queueParams.pFramePool = m_pFramePool;

        hr = pNewPlayer->CreateQueue( &queueParams );

        if( FAILED( hr ) )
        {
            pNewPlayer->Release();
            DPVF( DPVF_ERRORLEVEL, "Unable to create queue for player hr=0x%x", hr );
            DPVF_EXIT();

            return TRUE;
        }
    
        //
        // create converter(decode) for playback data
        // its ok to pass null for pInBoundConverter. See below
        //

        memcpy(&wfx, s_lpwfxPrimaryFormat, sizeof(wfx));

        hr = pNewPlayer->CreateMediaObjects(
                     m_dvSessionDesc.guidCT,
                     &wfx,
                     s_dwUnCompressedFrameSize,
                     m_pInBoundAudioConverter,
                     (m_RecordSubSystem ? FALSE : TRUE)); //check half duplex mode, output to mcpx
        
        if ( m_pInBoundAudioConverter) {

            //
            // for the first player, we pass on the converter we created upon ConnectAccept
            // we did then to get compression info cached.
            // all players after the first will get a new converter (but same compression info..)
            //

            m_pInBoundAudioConverter = NULL;

        } 
                    
        
        if( FAILED( hr ) )
        {
            pNewPlayer->Release();
            DPVF( DPVF_ERRORLEVEL, "Unable to get create converter for player hr=0x%x", hr );
            DPVF_EXIT();

            return TRUE;
        }

        hr = m_voiceNameTable.AddEntry( lpdvCreatePlayer->dvidID, pNewPlayer );

        if( FAILED( hr ) )
        {
            // Main ref
            pNewPlayer->Release();

            DPVF( DPVF_ERRORLEVEL, "Unable to add entry to nametable hr=0x%x", hr );
            DPVF_EXIT();
            return TRUE;
        }

        if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_FORWARDING )
        {
            pNewPlayer->SetAvailable(TRUE);
        }

        // Add a reference for the player to the "Playback Add List"
        
        pNewPlayer->AddRef();
        pNewPlayer->AddToPlayList( &m_PlayAddPlayersList);

        // Add a reference for the player to the "Notify Add List"
        pNewPlayer->AddRef();
        pNewPlayer->AddToNotifyList( &m_NotifyAddPlayersList );        

        // This will now be released by the callback, unless this is multicast
        //
        // Release our personal reference
        if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_FORWARDING )
        {
            pNewPlayer->Release();
        }
    }
    else
    {
        DPVF( DPVF_SPAMLEVEL, "Local player, no player record required" );
    }

#ifdef DVC_ENABLE_PEER_SESSION
    if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER )
    {
        DVMSG_CREATEVOICEPLAYER dvCreatePlayer;     
        dvCreatePlayer.dvidPlayer = lpdvCreatePlayer->dvidID;
        dvCreatePlayer.dwFlags = lpdvCreatePlayer->dwFlags; 
        dvCreatePlayer.dwSize = sizeof( DVMSG_CREATEVOICEPLAYER );
        dvCreatePlayer.pvPlayerContext = NULL;

        DPVF( DPVF_INFOLEVEL, "Queue Local player notification" );

        // Prevents double notification for local player
        if( lpdvCreatePlayer->dvidID != m_lpSessionTransport->GetLocalID() )
        {
            NotifyQueue_Add( DVMSGID_CREATEVOICEPLAYER, &dvCreatePlayer, sizeof( DVMSG_CREATEVOICEPLAYER ), 
                            pNewPlayer, NotifyComplete_RemotePlayer );
        } 
        else if( !m_fLocalPlayerNotify )
        {
            // Add local player flag to notification
            dvCreatePlayer.dwFlags |= DVPLAYERCAPS_LOCAL;

            // Notify of local player (don't create player record)
            NotifyQueue_Add( DVMSGID_CREATEVOICEPLAYER, &dvCreatePlayer, sizeof( DVMSG_CREATEVOICEPLAYER ), 
                            this, NotifyComplete_LocalPlayer );
            m_fLocalPlayerNotify = TRUE;
        } 
    }
#endif

    DPVF_EXIT();
    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::MigrateHost_RunElection"
//
// MigrateHost_RunElection
//
// Runs the host migration algorithm
//
HRESULT CDirectVoiceClientEngine::MigrateHost_RunElection(  )
{
    HRESULT hr;
    DWORD dwHostOrderID = DVPROTOCOL_HOSTORDER_INVALID;
    DVID dvidNewHost;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    if( m_dwCurrentState == DVCSTATE_DISCONNECTING )
    {
        DPVF( DPVF_WARNINGLEVEL, "HOST MIGRATION: We're disconnecting, no need to run algorithm" );
        DPVF_EXIT();
        return TRUE;
    }

    DWORD dwTransportSessionType, dwTransportFlags; 

    hr = m_lpSessionTransport->GetTransportSettings( &dwTransportSessionType, &dwTransportFlags );

    if( FAILED( hr ) )
    {
        ASSERT( FALSE );
        DPVF( DPVF_ERRORLEVEL, "Unable to retrieve transport settings hr=0x%x", hr );
        DPVF_EXIT();
        return DV_OK;
    }
    
    if( (m_dvSessionDesc.dwSessionType != DVSESSIONTYPE_PEER ) || 
        (m_dvSessionDesc.dwFlags & DVSESSION_NOHOSTMIGRATION) || 
        !(dwTransportFlags & DVTRANSPORT_MIGRATEHOST) )
    {
        DPVF( DPVF_WARNINGLEVEL, "HOST MIGRATION: Host migration is disabled." );
        DPVF( DPVF_WARNINGLEVEL, "HOST MIGRATION: Host is gone.  Session will exit soon."  );

        if( m_dwCurrentState == DVCSTATE_CONNECTING )
        {
            DPVF( DPVF_ERRORLEVEL, "Aborting connection..." );
            SendConnectResult();
        }

        DoSessionLost(DVERR_SESSIONLOST);

        DPVF_EXIT();
        return DV_OK;
    }   

    // This shortcut prevents problems if this is called twice.
    if( m_lpdvServerMigrated != NULL )
    {
        DPVF( DPVF_INFOLEVEL, "Skipping calling removeplayer again as host already migrated" );
        DPVF_EXIT();
        return DV_OK;
    }

    DPVF( DPVF_WARNINGLEVEL,"HOST MIGRATION: Player 0x%x is running election algorithm", this->m_dvidLocal );

    // Trust me.  Do this.
    dvidNewHost = m_dvidLocal;
    dwHostOrderID = m_voiceNameTable.GetLowestHostOrderID(&dvidNewHost);

    DPVF( DPVF_WARNINGLEVEL, "HOST MIGRATION: New Host is [0x%x]", dvidNewHost );

    m_lpSessionTransport->MigrateHost( dvidNewHost );
    m_dvidServer = m_lpSessionTransport->GetServerID();     

    DPVF( DPVF_WARNINGLEVEL, "HOST MIGRATION: Host is (2) [0x%x]", m_dvidServer );

    // No one was found and we're not properly connected yet
    if( dwHostOrderID == DVPROTOCOL_HOSTORDER_INVALID && m_dwCurrentState != DVCSTATE_CONNECTED )
    {
        DPVF( DPVF_ERRORLEVEL, "There is no one to take over session.  Disconnect" );

        // We're connecting.. expected behaviour
        if( m_dwCurrentState == DVCSTATE_CONNECTING )
        {
            DPVF( DPVF_ERRORLEVEL, "Aborting connection..." );
            
            SendConnectResult();
            DoSessionLost(DVERR_SESSIONLOST);
        }
        // We're already disconnecting
        else if( m_dwCurrentState == DVCSTATE_DISCONNECTING )
        {
            DPVF( DPVF_ERRORLEVEL, "Already disconnecting.." );
        }
    }
    // Candidate was found, it's us!
    else if( m_dwHostOrderID <= dwHostOrderID )
    {

        HRESULT hr;

        m_lpSessionTransport->MigrateHost( m_dvidLocal );
        m_dvidServer = m_lpSessionTransport->GetServerID();     
        
        DPVF( DPVF_WARNINGLEVEL, "HOST MIGRATION: We think we're the host!  Our Host Order ID=0x%x", m_dwHostOrderID );
        
        hr = HandleLocalHostMigrateCreate();            

        if( FAILED( hr ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Host migrate failed hr=0x%x", hr );
        }
    }

    // Someone was elected -- not us!
    //
    // We send a settings confirm message in case we ignored host migration message
    // during our election.  (Small, but reproducable window).
    //
    // If we do get it after this host may get > 1 settings confirm from us (which is handled).
    //
    else if( dwHostOrderID != DVPROTOCOL_HOSTORDER_INVALID )
    {
        hr = Send_SettingsConfirm();

        if( FAILED( hr ) )
        {
            DPVF( DPVF_ERRORLEVEL, "Settings confirm message failed! sent hr=0x%x", hr );
        }

        DVMSG_HOSTMIGRATED dvHostMigrated;
        dvHostMigrated.dvidNewHostID = m_dvidServer;
        dvHostMigrated.pdvServerInterface = NULL;
        dvHostMigrated.dwSize = sizeof( DVMSG_HOSTMIGRATED );

        DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: Sending notification because of election" );        
        
        NotifyQueue_Add( DVMSGID_HOSTMIGRATED, &dvHostMigrated, sizeof( DVMSG_HOSTMIGRATED )  );    
    }

    DPVF_EXIT();
    return DV_OK;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleDeleteVoicePlayer"
//
// HandleDeleteVoicePlayer
//
// Handles the DVMSGID_DELETEVOICEPLAYER message.
//
BOOL CDirectVoiceClientEngine::HandleDeleteVoicePlayer( DVID dvidSource, PDVPROTOCOLMSG_PLAYERQUIT lpdvDeletePlayer, DWORD dwSize )
{
    CVoicePlayer *pPlayer;
    HRESULT hr;    

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    ASSERT( dwSize == sizeof( DVPROTOCOLMSG_PLAYERQUIT ) );

    hr = m_voiceNameTable.GetEntry( lpdvDeletePlayer->dvidID, &pPlayer, TRUE );

    // If there is a player entry for the given ID, 
    // Handle removing them from the local player table
    if( pPlayer != NULL )
    {

        DPVF( DPVF_WARNINGLEVEL, "Deleting voice players %x, local id %x\n",
             lpdvDeletePlayer->dvidID,
             m_dvidLocal);

        // Remove the entry, this will also drop the reference count
        hr = m_voiceNameTable.DeleteEntry( lpdvDeletePlayer->dvidID );

        if( FAILED( hr ) )
        {
            DPVF( 0, "Error, could not find entry 0x%x to delete hr=0x%x", dvidSource, hr );
        }

        // Mark player record as disconnected
        pPlayer->SetDisconnected();

        CheckForAndRemoveTarget( lpdvDeletePlayer->dvidID );

        // If there are any buffers for this player, delete them
        // We don't need to destroy them, we want to save them so the user can call
        // Delete3DUserBuffer
        //

        if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER )
        {
#ifdef DVC_ENABLE_PEER_SESSION
            DVMSG_DELETEVOICEPLAYER dvMsgDeletePlayer;
            
            dvMsgDeletePlayer.dvidPlayer = lpdvDeletePlayer->dvidID;
            dvMsgDeletePlayer.dwSize = sizeof( DVMSG_DELETEVOICEPLAYER );
            dvMsgDeletePlayer.pvPlayerContext = pPlayer->GetContext();

            pPlayer->SetContext( NULL );

            // By making this synchronous we ensure that the voice notification has completed before the dplay8 
            // callback is called. 
            // 
            NotifyQueue_Add( DVMSGID_DELETEVOICEPLAYER, &dvMsgDeletePlayer, sizeof( DVMSG_DELETEVOICEPLAYER ), pPlayer, NotifyComplete_DeletePlayer);
#endif
        } else {

            pPlayer->Release();

        }
        
    } else if (lpdvDeletePlayer->dvidID == m_dvidLocal) {

        //
        // local player notification
        //

        DVMSG_DELETEVOICEPLAYER dvMsgDelete;    
        dvMsgDelete.dvidPlayer = m_dvidLocal;
        dvMsgDelete.dwSize = sizeof( DVMSG_DELETEVOICEPLAYER );
        dvMsgDelete.pvPlayerContext = m_pvLocalPlayerContext;

        m_pvLocalPlayerContext = NULL;
        m_fLocalPlayerNotify = FALSE;        
        m_fLocalPlayerAvailable = FALSE;

        TransmitMessage( DVMSGID_DELETEVOICEPLAYER, &dvMsgDelete, sizeof( DVMSG_DELETEVOICEPLAYER ) );

    }

    DPVF_EXIT();
    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::QueueSpeech"
//
// QueueSpeech
//
// Process and queue incoming audio
//
BOOL CDirectVoiceClientEngine::QueueSpeech( DVID dvidSource, PDVPROTOCOLMSG_SPEECHHEADER pdvSpeechHeader, PBYTE pbData, DWORD dwSize )
{
    CVoicePlayer *pPlayerInfo;
    HRESULT hr;

    DPVF_ENTER();
    // Only start receiving voice if the local player is active
    if( !m_fLocalPlayerAvailable )
    {
        DPVF( 1, "Ignoring incoming audio, local player has not been indicated" );
        DPVF_EXIT();
        return TRUE;
    }
    
    hr = m_voiceNameTable.GetEntry( dvidSource, &pPlayerInfo, TRUE );

    if( FAILED( hr ) )
    {
        DPVF( 1, "Received speech for player who is not in nametable hr=0x%x", hr );
        DPVF_EXIT();
        return TRUE;
    }

    if( !pPlayerInfo->IsAvailable() )
    {
        DPVF( 1, "Player is not yet available, ignoring speech" );
    }
    else
    {
        hr = pPlayerInfo->HandleReceive( pdvSpeechHeader, pbData, dwSize );

        if( FAILED( hr ) )
        {
            pPlayerInfo->Release();
            DPVF( 1, "Received speech could not be buffered hr=0x%x", hr );
            DPVF_EXIT();
            return TRUE;
        }

    }

    // Release our reference to the player
    pPlayerInfo->Release();
        
    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleSpeechWithFrom"
//
// HandleSpeech
//
// Handles speech data messages
//
BOOL CDirectVoiceClientEngine::HandleSpeechWithFrom( DVID dvidSource, PDVPROTOCOLMSG_SPEECHWITHFROM lpdvSpeech, DWORD dwSize )
{
    HRESULT hr;
    CVoicePlayer *pPlayerInfo;  

    DPVF_ENTER();
    ASSERT( dwSize >= sizeof( DVPROTOCOLMSG_SPEECHWITHFROM ) );

    DPVF( DPVF_INFOLEVEL, "Received multicast speech!" );

    if( lpdvSpeech->dvidFrom == m_dvidLocal )
    {
        DPVF( DPVF_SPAMLEVEL, "Ignoring loopback speech!" ); 
        DPVF_EXIT();
        return TRUE;
    }

    hr = m_voiceNameTable.GetEntry( lpdvSpeech->dvidFrom, &pPlayerInfo, TRUE );
    
    if( FAILED( hr ) )
    {
        DVPROTOCOLMSG_PLAYERJOIN dvPlayerJoin;
        dvPlayerJoin.dwFlags = 0;
        dvPlayerJoin.dvidID = lpdvSpeech->dvidFrom;
        HandleCreateVoicePlayer( lpdvSpeech->dvidFrom, &dvPlayerJoin, sizeof( DVPROTOCOLMSG_PLAYERJOIN ) );
    }
    else
    {
        pPlayerInfo->Release();
    }

    DPVF_EXIT();
    return QueueSpeech( lpdvSpeech->dvidFrom, &lpdvSpeech->dvHeader, (PBYTE) &lpdvSpeech[1], dwSize-sizeof(DVPROTOCOLMSG_SPEECHWITHFROM) );

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleSpeechBounce"
//
// HandleSpeech
//
// Handles speech data messages
//
BOOL CDirectVoiceClientEngine::HandleSpeechBounce( DVID dvidSource, PDVPROTOCOLMSG_SPEECHHEADER lpdvSpeech, DWORD dwSize )
{
    DPVF_ENTER();
    ASSERT( dwSize >= sizeof( DVPROTOCOLMSG_SPEECHHEADER ) );

    DPVF( DPVF_INFOLEVEL, "Received speech bounce!" );

    DPVF_EXIT();
    return QueueSpeech( dvidSource, lpdvSpeech, (PBYTE) &lpdvSpeech[1], dwSize - sizeof( DVPROTOCOLMSG_SPEECHHEADER ) );
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleSpeech"
//
// HandleSpeech
//
// Handles speech data messages
//
BOOL CDirectVoiceClientEngine::HandleSpeech( DVID dvidSource, PDVPROTOCOLMSG_SPEECHHEADER lpdvSpeech, DWORD dwSize )
{
    ASSERT( dwSize >= sizeof( DVPROTOCOLMSG_SPEECHHEADER ) );

    // Ignore speech from ourselves
    if( dvidSource == m_dvidLocal )
    {
        DPVF( DPVF_SPAMLEVEL, "Ignoring loopback speech!" );     
        DPVF_EXIT();
        return TRUE;
    }

    DPVF( DPVF_INFOLEVEL, "Received bare speech!" );

    DPVF_EXIT();
    return QueueSpeech( dvidSource, lpdvSpeech, (PBYTE) &lpdvSpeech[1], dwSize - sizeof( DVPROTOCOLMSG_SPEECHHEADER ) );
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::Cleanup"
//
// Cleanup
//
// WARNING: Do not call this function multiple times on the same object.
//
// This function shuts down the recording and playback threads, shuts down
// the sound system and unhooks the object from the dplay object.
// 
// Called By:
// - DoDisconnect
// - Destructor 
// - HandleConnectAccept
// - NotifyThread
//
// Locks Required:
// - Global Write Lock
//
void CDirectVoiceClientEngine::Cleanup()
{
   
    DPVF( DPVF_ERRORLEVEL, "Cleanup called!" );    

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    // We only need to cleanup if we're not idle
    if( m_dwCurrentState == DVCSTATE_IDLE )
    {
        DPVF( DPVF_INFOLEVEL, "Cleanup not required" );
        DPVF_EXIT();
        return;
    }
    
    // Disable notifications, no notifications after this point can be made
    NotifyQueue_Disable();  

    // The following code is extremely sensitive.
    //
    // Be careful about the order here, it's important.

    DPVF( DPVF_WARNINGLEVEL, "Waiting for outstanding sends" );  
    // Wait for outstanding buffer sends to complete before
    // continuing.  Otherwise you have potential crash / leak
    // condition
    WaitForBufferReturns();

    DPVF( DPVF_WARNINGLEVEL, "Disconnecting transport" );
    // After this function returns DirectPlay will no longer sends
    // us indication, but some may still be in progress.
    m_lpSessionTransport->DisableReceiveHook( );

    DPVF( DPVF_INFOLEVEL, "Flushing notification queue" );
    // Ensuring that all notifications have been sent 

    NotifyQueue_IndicateNext(); 

    // Waiting for transport to return all the threads it is indicating into us on.
    DPVF( DPVF_WARNINGLEVEL, "Waiting for transport threads to complete" );
    m_lpSessionTransport->WaitForDetachCompletion();

    // Re-enable notifications
    NotifyQueue_Enable();

    CleanupPlaybackLists();
    CleanupNotifyLists();

    // Inform player of their own exit if they were connected!
    if( m_fLocalPlayerNotify )
    {
        DVMSG_DELETEVOICEPLAYER dvMsgDelete;    
        dvMsgDelete.dvidPlayer = m_dvidLocal;
        dvMsgDelete.dwSize = sizeof( DVMSG_DELETEVOICEPLAYER );
        dvMsgDelete.pvPlayerContext = m_pvLocalPlayerContext;

        m_pvLocalPlayerContext = NULL;
        m_fLocalPlayerNotify = FALSE;        
        m_fLocalPlayerAvailable = FALSE;

        
        TransmitMessage( DVMSGID_DELETEVOICEPLAYER, &dvMsgDelete, sizeof( DVMSG_DELETEVOICEPLAYER ) );
    }    

    m_voiceNameTable.DeInitialize( (m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER), m_lpUserContext, m_lpMessageHandler);

    //
    // Hold off on the shutdown of the sound system so user notifications
    // of delete players on unravel of nametable can be handled correctly. 
    // 

    ShutdownSoundSystem();

    DPVF( DPVF_WARNINGLEVEL, "Sound system shutdown" );

    FreeBuffers();

    if( m_pFramePool != NULL )
    {
        delete m_pFramePool;
        m_pFramePool = NULL;
    }

    SetCurrentState( DVCSTATE_IDLE );

    if( m_pdvidTargets != NULL )
    {
        DV_POOL_FREE(m_pdvidTargets);
        m_pdvidTargets = NULL;
        m_dwNumTargets = 0;
        m_dwTargetVersion = 0;
    }

    if( m_lpdvServerMigrated != NULL )
    {
        m_lpdvServerMigrated->Release();
        m_lpdvServerMigrated = NULL;
    }

    DPVF_EXIT();
}

// WaitForBufferReturns
//
// This function waits until oustanding sends have completed before continuing
// we use this to ensure we don't deregister with outstanding sends.
// 
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::WaitForBufferReturns"
void CDirectVoiceClientEngine::WaitForBufferReturns()
{
    ULONG cnt = 0;
    DPVF_ENTER();

    DV_AUTO_LOCK(&m_CS);

    if((m_MsgBufferAllocs == 0) && (m_SpeechBufferAllocs == 0)) {
        DPVF_EXIT();                                         
        return;
    }        

    while( m_MsgBufferAllocs || m_SpeechBufferAllocs) 
    {
        //
        // call dplay do work
        //

        m_lpSessionTransport->DoWork();

        Sleep( 20 );

        if (cnt++ > 100) {
            break;
        }
    }

    DPVF_EXIT();
    return;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::DoDisconnect"
//
// DoDisconnect
//
// Performs a disconnection and informs the callback function.
//
// Used for both session lost and normal disconnects.
//
// Called By:
// - NotifyThread
// 
void CDirectVoiceClientEngine::DoDisconnect()
{
    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    // Guard to prevent this function from being called more then once on the
    // same object
    if( m_dwCurrentState == DVCSTATE_IDLE ||
        m_dwCurrentState == DVCSTATE_NOTINITIALIZED ) {
        DPVF_EXIT();
        return;
    }
        
    m_dwCurrentState = DVCSTATE_DISCONNECTING;
    
    DPVF( DPVF_INFOLEVEL, "DoDisconnect called!" );
    
    if( m_fSessionLost )
    {
        DVMSG_SESSIONLOST dvSessionLost;
        dvSessionLost.hrResult = m_hrDisconnectResult;
        dvSessionLost.dwSize = sizeof( DVMSG_SESSIONLOST );
        
        NotifyQueue_Add( DVMSGID_SESSIONLOST, &dvSessionLost, sizeof( DVMSG_SESSIONLOST ) );
    }
    else
    {
        DPVF( DPVF_INFOLEVEL, "Signalling disconnect result" );
        SendDisconnectResult();
        DPVF( DPVF_INFOLEVEL, "Done signalling disconnect" );
    }

    Cleanup();
    DPVF( DPVF_INFOLEVEL, "Disconnect and Cleanup complete" );

    m_dwCurrentState = DVCSTATE_DISCONNECTED;

    DPVF_EXIT();
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleDisconnect"
// HandleDisconnect
//
// This function is called when a disconnect message is received from the
// server.  
//
BOOL CDirectVoiceClientEngine::HandleDisconnectConfirm( DVID dvidSource, PDVPROTOCOLMSG_DISCONNECT lpdvDisconnect, DWORD dwSize )
{
    ASSERT( dwSize == sizeof( DVPROTOCOLMSG_DISCONNECT ) );

    DPVF_ENTER();
    DPVF(  DPVF_INFOLEVEL, "DisconnectConfirm received, signalling worker [res=0x%x]", lpdvDisconnect->hresDisconnect );

    DoSignalDisconnect( lpdvDisconnect->hresDisconnect );

    DPVF(  DPVF_SPAMLEVEL, "DisconnectConfirm received, signalled worker" );  

    DPVF_EXIT();
    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleSessionLost"
BOOL CDirectVoiceClientEngine::HandleSessionLost( DVID dvidSource, PDVPROTOCOLMSG_SESSIONLOST lpdvSessionLost, DWORD dwSize )
{
    ASSERT( dwSize == sizeof( DVPROTOCOLMSG_SESSIONLOST ) );

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "DVCE::HandleSessionLost() begin" );

    DPVF( DPVF_ERRORLEVEL, "<><><><><><><> Session Host has shutdown - Voice Session is gone." );

    DoSessionLost( lpdvSessionLost->hresReason );

    DPVF_EXIT();
    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::StartTransportSession"
HRESULT CDirectVoiceClientEngine::StartTransportSession( )
{
    DPVF_ENTER();
    DPVF_EXIT();
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::StopTransportSession"
// StopSession
//
// This function is called when the directplay session is lost or stops
// before DirectXVoice is disconnected.
//
HRESULT CDirectVoiceClientEngine::StopTransportSession()
{
    DPVF_ENTER();
    DoSessionLost( DVERR_SESSIONLOST );
    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::AddPlayer"
HRESULT CDirectVoiceClientEngine::AddPlayer( DVID dvID )
{
    DPVF_ENTER();
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::HandleLocalHostMigrateCreate"
HRESULT CDirectVoiceClientEngine::HandleLocalHostMigrateCreate()
{
    LPDIRECTVOICESERVEROBJECT lpdvsServerObject = NULL;
    LPBYTE lpSessionBuffer = NULL;
    DWORD dwSessionSize = 0;
    HRESULT hr = DPN_OK;
    CDirectVoiceDirectXTransport *pTransport;

    DPVF_ENTER();
    DPVF( DPVF_ERRORLEVEL, ("Local client has become the new host.  Creating a host" ));

    hr = XDirectPlayVoiceCreate(IID_IDirectPlayVoiceServer,(PVOID *) &lpdvsServerObject );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Unable to create server object. hr=0x%x", hr );
        DPVF_EXIT();
        goto HOSTCREATE_FAILURE;
    }

    lpdvsServerObject->lIntRefCnt++;

    DVMSG_LOCALHOSTSETUP dvMsgLocalHostSetup;
    dvMsgLocalHostSetup.dwSize = sizeof( DVMSG_LOCALHOSTSETUP );
    dvMsgLocalHostSetup.pvContext = NULL;
    dvMsgLocalHostSetup.pMessageHandler = NULL;

    TransmitMessage( DVMSGID_LOCALHOSTSETUP, &dvMsgLocalHostSetup,  sizeof( DVMSG_LOCALHOSTSETUP ) );                   

    //// BUGBUG: This should always be possible, but if we implement new transport
    ////         it won't be.
    ////
    pTransport = (CDirectVoiceDirectXTransport *) m_lpSessionTransport; 

    hr = DV_Initialize( lpdvsServerObject, pTransport->GetTransportInterface(), dvMsgLocalHostSetup.pMessageHandler, dvMsgLocalHostSetup.pvContext, NULL, 0 );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Unable to initialize the server object hr=0x%x", hr );
        goto HOSTCREATE_FAILURE;
    }

    hr = lpdvsServerObject->lpDVServerEngine->HostMigrateStart( &m_dvSessionDesc, m_dwHostOrderID+DVMIGRATE_ORDERID_OFFSET );

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Error starting server object hr=0x%x", hr );
        goto HOSTCREATE_FAILURE;
    }

    // Grab a reference local of the new host interface
    m_lpdvServerMigrated = (LPDIRECTPLAYVOICESERVER) lpdvsServerObject;

    DVMSG_HOSTMIGRATED dvHostMigrated;
    dvHostMigrated.dvidNewHostID = m_lpSessionTransport->GetLocalID();
    dvHostMigrated.pdvServerInterface = (LPDIRECTPLAYVOICESERVER) lpdvsServerObject;
    dvHostMigrated.dwSize = sizeof( DVMSG_HOSTMIGRATED );

    NotifyQueue_Add( DVMSGID_HOSTMIGRATED, &dvHostMigrated, sizeof( DVMSG_HOSTMIGRATED )  );    
    
    DPVF_EXIT();
    return DV_OK;

HOSTCREATE_FAILURE:

    DPVF( DPVF_ERRORLEVEL, "Informing clients of our failure to create host" );

    Send_SessionLost();

    if( lpdvsServerObject != NULL )
    {
        IDirectPlayVoiceServer_Release((LPDIRECTPLAYVOICESERVER) lpdvsServerObject );
    }

    DPVF_EXIT();
    return hr;
}

// Handles remove player message
//
// This message triggers handling of host migration if the player
// who has dropped out happens to be the session host.
//
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::RemovePlayer"
HRESULT CDirectVoiceClientEngine::RemovePlayer( DVID dvID )
{
    HRESULT hr;
    
    CVoicePlayer *pVoicePlayer;
    DVPROTOCOLMSG_PLAYERQUIT dvPlayerQuit;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    if( m_dwCurrentState == DVCSTATE_DISCONNECTING )
    {
        DPVF( DPVF_WARNINGLEVEL, "Ignoring transport disconnect for 0x%x -- client is disconnecting", dvID );
        DPVF_EXIT();
        return DV_OK;
    }

    dvPlayerQuit.dwType = DVMSGID_DELETEVOICEPLAYER;
    dvPlayerQuit.dvidID = dvID;

    HandleDeleteVoicePlayer( 0, &dvPlayerQuit, sizeof( DVPROTOCOLMSG_PLAYERQUIT ) );        

    // The person who dropped out was the server
    if( dvID == m_lpSessionTransport->GetServerID() )
    {
        DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: Checking to see if remove of 0x%x is host 0x%x", dvID, m_lpSessionTransport->GetServerID() );
        DPVF( DPVF_HOSTMIGRATE_DEBUG_LEVEL, "HOST MIGRATION: Triggered by Remove Player Message" );

        MigrateHost_RunElection();
    }

    DPVF_EXIT();
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SetCurrentState"
// SetCurrentState
//
// Sets the current state of the client engine
// 
// must alwaysbe called at DPC for synchronization
//
void CDirectVoiceClientEngine::SetCurrentState( DWORD dwState )
{
    DV_AUTO_LOCK(&m_CS);
    m_dwCurrentState = dwState;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::CheckShouldSendMessage"
//
// CheckShouldSendMessage
//
// Checks the notification mask to see if the specified message type should
// be sent to the user. 
//
// called at DPC only
BOOL CDirectVoiceClientEngine::CheckShouldSendMessage( DWORD dwMessageType )
{
    BOOL fSend = FALSE;         

    DPVF_ENTER();

    DV_AUTO_LOCK(&m_CS);
    if( m_lpMessageHandler == NULL )
    {
        DPVF_EXIT();
        return FALSE;
    }
    
    if( m_dwNumMessageElements == 0 )
    {
        DPVF_EXIT();
        return TRUE;
    }
    else
    {
        for( DWORD dwIndex = 0; dwIndex < m_dwNumMessageElements; dwIndex++ )
        {
            if( m_pMessageElements[dwIndex] == dwMessageType )
            {
                DPVF_EXIT();
                return TRUE;
            }
        }
    }

    DPVF_EXIT();
    return FALSE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::TransmitMessage"
//
// TransmitMessage
//
// Called to send a notification to the user.  
//
// Only the notify thread should call this function, all other threads should queue up 
// notifications by calling NotifyQueue_Add.
//
// Called By:
// - NotifyThread.
//
void CDirectVoiceClientEngine::TransmitMessage( DWORD dwMessageType, LPVOID lpData, DWORD dwSize )
{    

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    if( CheckShouldSendMessage( dwMessageType ) )
    {
        (*m_lpMessageHandler)( m_lpUserContext, dwMessageType, lpData );                
    }

    DPVF_EXIT();

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::InitializeSoundSystem"
// InitializeSoundSystem
//
// Starts up the sound system based on the parameters.
//
HRESULT CDirectVoiceClientEngine::InitializeSoundSystem()
{
    HRESULT hr;
    ULONG temp;
    XMediaObject *pInboundConverter;

    DPVF_ENTER();
    DPVF( DPVF_ENTRYLEVEL, "DVCE::InitializeSoundSystem() Begin" );
    
    //
    // we dont initialize the playback subsystem explicitly.
    // instead, when voice from a remote user arrives, we then create
    // a streaming DS object for that users data
    // then each frame gets mixed in automatically by dsound
    //

    //
    // init record subsystem
    //

    if (!(m_dvClientConfig.dwFlags & DVCLIENTCONFIG_HALFDUPLEX)) {

        if (m_RecordSubSystem == NULL) {

            m_RecordSubSystem = new CClientRecordSubSystem( this );
            // Check that the converter is valid
            if( m_RecordSubSystem == NULL  )
            {
                DPVF(  DPVF_ERRORLEVEL, "Memory alloc failure" );
                return DVERR_RECORDSYSTEMERROR;
            }
    
            hr = m_RecordSubSystem->Initialize();
    
            if( FAILED( hr ) )
            {
                DPVF( DPVF_ERRORLEVEL, "Record Sub Error during init hr=0x%x", hr );
                SwitchToHalfDuplex();

            }

        }

    }

    if (m_pFramePool == NULL) {

        /////////////////////////////////////////////////////////////////////////////////////////
        // Build the frame pool
        m_pFramePool = new CFramePool( (USHORT)s_dwCompressedFrameSize );            
    
        if( m_pFramePool == NULL )
        {
            DPVF( DPVF_ERRORLEVEL, "Unable to allocate frame pool" );
            DPVF_EXIT();
            return DVERR_OUTOFMEMORY;
        }
    
        DPVF( DPVF_INFOLEVEL, "FULL DUPLEX INIT: End ==========" );

    }

    DPVF( DPVF_ENTRYLEVEL, "DVCE::InitializeSoundSystem() End" );

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::ShutdownSoundSystem"
// ShutdownSoundSystem
//
// Stop the sound system
//
HRESULT CDirectVoiceClientEngine::ShutdownSoundSystem()
{
    HRESULT hr;    

    DPVF_ENTER();
    DPVF(  DPVF_ENTRYLEVEL, "DVCE::ShutdownSoundSystem() Begin" );

    //
    // delete the record subsystem, if it exists
    //

    DV_AUTO_LOCK(&m_CS);

    if (m_RecordSubSystem) {
        delete m_RecordSubSystem;
        m_RecordSubSystem = NULL;
    }    

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SendConnectResult"
__inline HRESULT CDirectVoiceClientEngine::BuildCompressionInfo()
{

    ULONG temp;
    HRESULT hr;
    XMEDIAINFO xmi;
    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    memset(&xmi,0,sizeof(xmi));
    xmi.dwInputSize = 1;

    //
    // get compressed size from record subsystem instance of decoder
    //

    hr = m_pInBoundAudioConverter->GetInfo(&xmi);

    if (FAILED(hr)) {
        DPVF_EXIT();
        return hr;
    }

    s_dwCompressedFrameSize = xmi.dwInputSize;
    s_dwUnCompressedFrameSize = xmi.dwOutputSize;

    DPVF(DPVF_INFOLEVEL, "CompressionInfo: Encode Frame %x, Decoded Frame %x",
        s_dwCompressedFrameSize,
        s_dwUnCompressedFrameSize);

    DPVF_EXIT();
    return hr;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SendConnectResult"
HRESULT CDirectVoiceClientEngine::SendConnectResult()
{
    DVMSG_CONNECTRESULT dvConnect;

    DPVF_ENTER();

    dvConnect.hrResult = m_hrConnectResult;
    dvConnect.dwSize = sizeof( DVMSG_CONNECTRESULT );

    DPVF_EXIT();
    return NotifyQueue_Add( DVMSGID_CONNECTRESULT, &dvConnect, sizeof( DVMSG_CONNECTRESULT ) );
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SendDisconnectResult"
HRESULT CDirectVoiceClientEngine::SendDisconnectResult()
{
    DPVF_ENTER();
    DVMSG_DISCONNECTRESULT dvDisconnect;
    dvDisconnect.hrResult = m_hrDisconnectResult;
    dvDisconnect.dwSize = sizeof( DVMSG_DISCONNECTRESULT );

    DPVF_EXIT();
    return NotifyQueue_Add( DVMSGID_DISCONNECTRESULT, &dvDisconnect, sizeof( DVMSG_DISCONNECTRESULT ) );
}

// DoWork routine
//
// All-purpose function that executes queued work items
//
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::DoWork"
HRESULT CDirectVoiceClientEngine::DoWork()
{
    DWORD dwWaitPeriod;
    LONG lWaitResult;
    DWORD dwPowerLevel;
    DWORD dwLastTimeoutCheck;
    
    DWORD dwCurTime;

    CNotifyElement *neElement;
    HRESULT hr;
    DVID  dvidMessageTarget;

    DVMSG_INPUTLEVEL dvInputLevel;        

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    dvInputLevel.dwSize = sizeof( DVMSG_INPUTLEVEL );

    if (m_dwCurrentState == DVCSTATE_NOTINITIALIZED) {

        return DVERR_NOTINITIALIZED;

    }

    // Setup last time we checked to right now.
    dwLastTimeoutCheck = GetTickCount();
    
    do
    {

        dwCurTime = GetTickCount();

        //
        // get a snapshot of the events
        // clear event flags at DPC
        // this is just like a driver DPC syncronizing with its ISR
        //

        if (m_dwCurrentState == DVCSTATE_DISCONNECTED) {

            return DVERR_CONNECTIONLOST;

        }

        hr = NotifyQueue_IndicateNext();

        if( FAILED( hr ) )
        {
            DPVF( DPVF_ERRORLEVEL, "NotifyQueue_IndicateNext Failed hr=0x%x", hr );
        }

        // If we're connecting, check for timeout on the connection
        // request.
        if((m_dwCurrentState == DVCSTATE_CONNECTING) && (m_dwSynchBegin != 0) )
        {
            
            if( ( dwCurTime - m_dwSynchBegin ) > DV_CLIENT_CONNECT_TIMEOUT )
            {
                DPVF( DPVF_ERRORLEVEL, "Connection Timed-Out.  Returning NOVOICESESSION" );
                m_hrConnectResult = DVERR_NOVOICESESSION;
                SendConnectResult();
                Cleanup();              
                continue;               
            }
            else if( ( dwCurTime - m_dwLastConnectSent ) > DV_CLIENT_CONNECT_RETRY_TIMEOUT )
            {
                DPVF( DPVF_WARNINGLEVEL, "Connect Request Timed-Out" );
                DPVF( DPVF_WARNINGLEVEL, "Re-sending connection request" );

                hr = Send_ConnectRequest();

                if( FAILED( hr ) )
                {
                    DPVF( DPVF_ERRORLEVEL, "Error sending connection request.  Send error hr=0x%x", hr );

                    m_hrConnectResult = DVERR_SENDERROR;

                    SendConnectResult();
                    Cleanup();                  
                    continue;
                }
                
                m_dwLastConnectSent = dwCurTime;
            }
        }
        // If we're disconnecting, check for timeout on the disconnect
        else if( m_dwCurrentState == DVCSTATE_DISCONNECTING )
        {
            DPVF( DPVF_INFOLEVEL, "Checking timeout on disconnect.  Waited %d so far", dwCurTime - m_dwSynchBegin );
            
            if( ( dwCurTime - m_dwSynchBegin ) > DV_CLIENT_DISCONNECT_TIMEOUT )
            {
                DPVF( DPVF_WARNINGLEVEL, "Disconnect Request Timed-Out" );
                DoSignalDisconnect( DVERR_TIMEOUT );                
            }

            if (m_fSessionLost == TRUE) {

                //
                // disconnect has been signalled, cleanup
                //

                DoDisconnect();

            }

        }

        // Take care of the periodic checks
        if (m_dwCurrentState == DVCSTATE_CONNECTED)
        {
            dwCurTime = GetTickCount();

            // Update pending / other lists
            UpdateActiveNotifyPendingList( );
            
            // If we're running a multicast session.. check for timed-out users
            if(m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_FORWARDING )
            {
                if( ( dwCurTime - dwLastTimeoutCheck ) > DV_MULTICAST_USERTIMEOUT_PERIOD )
                {
                    CheckForUserTimeout(dwCurTime);

                    dwLastTimeoutCheck = dwCurTime;         
                }
            }

            if (m_fLocalPlayerAvailable &&
                CheckShouldSendMessage( DVMSGID_INPUTLEVEL )) {

                dvInputLevel.pvLocalPlayerContext = m_pvLocalPlayerContext; 
                dvInputLevel.dwPeakLevel = m_bLastPeak;

                NotifyQueue_Add( DVMSGID_INPUTLEVEL, &dvInputLevel, sizeof( DVMSG_INPUTLEVEL ) );

            }

            QueuePlayerLevelNotifications();

        }

        if(m_dwCurrentState == DVCSTATE_IDLE )
        {
            return DV_OK;
        }

    } while(FALSE);

    //
	// If the notification of the local player hasn't been processed and we're in peer to peer mode, don't
	// allow recording to start until after the player has been indicated
    //

	if((m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER) &&
       !m_fLocalPlayerAvailable )
	{
		DPVF( RRI_DEBUGOUTPUT_LEVEL, "Local player has not yet been indicated, not doing Record/Playback work" );
        DPVF_EXIT();
        return DV_OK;

	} 

    if (m_RecordSubSystem) {

        hr = m_RecordSubSystem->RecordFSM();
        if (FAILED(hr)) {
            
            //
            // hawk got removed, delete the record subsystem and switch to half duplex
            //

            SwitchToHalfDuplex();

            DPVF_EXIT();
            return DVERR_RECORDSYSTEMERROR;
        }
    
        dwCurTime = GetTickCount();
    
    }

    //
    // do some playback work
    //

    hr = DoPlaybackWork();
    if (FAILED(hr)) {

        ASSERT(!m_RecordSubSystem);
        if (!m_RecordSubSystem) {

            //
            // hmm race condition: either session really got lost
            // or hawk got removed between RecordFSM returning and DoPlaybackWork getting called
            // BUGBUG deal with this by doing a session disconnect for now.. Evenutally
            // we should ask USB if the hawk is still in , if it is dont loose the session, just switch
            // to half duplex
            //

            DoSessionLost(DVERR_SESSIONLOST);

        } else {

            SwitchToHalfDuplex();

        }

    }

    DPVF_EXIT();
    return DV_OK;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SwitchToHalfDuplex"
//
// SwitchToHalfDuplex
// 
VOID CDirectVoiceClientEngine::SwitchToHalfDuplex()
{

    PLIST_ENTRY         currentEntry;
    CVoicePlayer *      pCurrentPlayer;
    WAVEFORMATEX        wfx;
    HRESULT             hr;
    DVMSG_VOICEPERIPHERALNOTPRESENT dvVoiceNotPresent;

    DV_AUTO_LOCK(&m_CS);

    delete m_RecordSubSystem;

    DPVF( DPVF_ERRORLEVEL, "Switching to half duplex since no hawk available!!!!");

    memcpy(&wfx, s_lpwfxPrimaryFormat, sizeof(wfx));

    m_RecordSubSystem = NULL;
    m_dvClientConfig.dwFlags |= DVCLIENTCONFIG_HALFDUPLEX;

    //
    // now switch the playback output for each remote player from hawk, to MCPX
    //

    // Update list 
    UpdateActivePlayPendingList(  );

    currentEntry = m_PlayActivePlayersList.Flink;

    while( currentEntry != &m_PlayActivePlayersList ) {

        pCurrentPlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_PlayListEntry );
        currentEntry = currentEntry->Flink;

        //
        // switch player to use MCPX for audio output
        // mute playback if the caller specified the right flag
        //

        hr = pCurrentPlayer->SwitchInternalAudioTarget(
            &wfx,
            TRUE, 
            (m_dvClientConfig.dwFlags & DVCLIENTCONFIG_MUTEIFNODEVICE) ? TRUE : FALSE);

        if (FAILED(hr)) {

            //
            // ok quit tryin, everythin is messed up
            // disconnect session
            //

            DoSessionLost(DVERR_SESSIONLOST);
            return;

        }

    }

    //
    // send a message to the user that the input device has been removed
    //

    dvVoiceNotPresent.dwSize = sizeof( DVMSG_VOICEPERIPHERALNOTPRESENT);
    NotifyQueue_Add( DVMSGID_VOICEPERIPHERALNOTPRESENT, &dvVoiceNotPresent, sizeof( DVMSG_VOICEPERIPHERALNOTPRESENT ) );                

}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::QueuePlayerLevelNotifications"
//
// SendPlayerLevels
// 
// Send player level notifications, but only if there is a message handler
// and the player level message is active.
//
VOID CDirectVoiceClientEngine::QueuePlayerLevelNotifications()
{
    CVoicePlayer *pCurrentPlayer;
    DVMSG_PLAYEROUTPUTLEVEL dvPlayerLevel;
    PLIST_ENTRY currentEntry;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();

    if( CheckShouldSendMessage( DVMSGID_PLAYEROUTPUTLEVEL ))
    {

        currentEntry = m_NotifyActivePlayersList.Flink;

        while(currentEntry != &m_NotifyActivePlayersList )
        {
            pCurrentPlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_NotifyListEntry );

            ASSERT( pCurrentPlayer != NULL );

            if( pCurrentPlayer->IsReceiving() )
            {
                dvPlayerLevel.dwSize = sizeof( DVMSG_PLAYEROUTPUTLEVEL );
                dvPlayerLevel.dvidSourcePlayerID = pCurrentPlayer->GetPlayerID();
                dvPlayerLevel.dwPeakLevel = pCurrentPlayer->GetLastPeak();
                dvPlayerLevel.pvPlayerContext = pCurrentPlayer->GetContext();

                NotifyQueue_Add( DVMSGID_PLAYEROUTPUTLEVEL, &dvPlayerLevel, sizeof( DVMSG_PLAYEROUTPUTLEVEL ) );                
            }

            currentEntry = currentEntry->Flink;
        } 

    }   

    DPVF_EXIT();
    return;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::CheckForUserTimeout"
//
// CheckForUserTimeout
//
// Run the list of users and check for user timeouts in multicast sessions
//
void CDirectVoiceClientEngine::CheckForUserTimeout( DWORD dwCurTime )
{
    PLIST_ENTRY currentEntry;
    CVoicePlayer *pCurrentPlayer;
    DVPROTOCOLMSG_PLAYERQUIT msgPlayerQuit;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    msgPlayerQuit.dwType = DVMSGID_DELETEVOICEPLAYER;
    currentEntry = m_NotifyActivePlayersList.Flink;

    while(currentEntry != &m_NotifyActivePlayersList)
    {   

        pCurrentPlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_NotifyListEntry );

        if( dwCurTime - pCurrentPlayer->GetLastPlayback() > DV_MULTICAST_USERTIMEOUT_PERIOD )
        {
            msgPlayerQuit.dvidID = pCurrentPlayer->GetPlayerID();
            HandleDeleteVoicePlayer( pCurrentPlayer->GetPlayerID(), &msgPlayerQuit, sizeof( DVPROTOCOLMSG_PLAYERQUIT ) );
        }

        currentEntry = currentEntry->Flink;

    };

    DPVF(  DPVF_INFOLEVEL, "Done Enum" );

    DPVF_EXIT();
    return;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::CleanupPlaybackList"

// Cleanup any outstanding entries on the playback lists
void CDirectVoiceClientEngine::CleanupPlaybackLists()
{
    PLIST_ENTRY currentEntry;
    CVoicePlayer *pVoicePlayer;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    m_dwPlayActiveCount = 0;

    currentEntry = m_PlayActivePlayersList.Flink;

    while(currentEntry != &m_PlayActivePlayersList) 
    {
        pVoicePlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_PlayListEntry );
        pVoicePlayer->RemoveFromPlayList();
        pVoicePlayer->Release();
        currentEntry = m_PlayActivePlayersList.Flink;
    }; 

    currentEntry = m_PlayAddPlayersList.Flink;

    while(currentEntry != &m_PlayAddPlayersList)
    {
        pVoicePlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_PlayListEntry );
        pVoicePlayer->RemoveFromPlayList();
        pVoicePlayer->Release();
        currentEntry =  m_PlayAddPlayersList.Flink;
    };

    DPVF_EXIT();
    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::CleanupNotifyLists"

void CDirectVoiceClientEngine::CleanupNotifyLists(  )
{
    PLIST_ENTRY currentEntry;
    CVoicePlayer *pVoicePlayer;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    currentEntry = m_NotifyActivePlayersList.Flink;

    while( currentEntry != &m_NotifyActivePlayersList )
    {
        pVoicePlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_NotifyListEntry );
        pVoicePlayer->RemoveFromNotifyList();
        pVoicePlayer->Release();
        currentEntry = m_NotifyActivePlayersList.Flink;
    }

    currentEntry = m_NotifyAddPlayersList.Flink;

    while( currentEntry != &m_NotifyAddPlayersList )
    {
        pVoicePlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_NotifyListEntry );
        pVoicePlayer->RemoveFromNotifyList();
        pVoicePlayer->Release();
        currentEntry = m_NotifyAddPlayersList.Flink;
    }

    DPVF_EXIT();
    
}

// UpdateActivePendingList
//
// This function looks at the pending list and moves those elements on the pending list to the active list
//
// This function also looks at the active list and removes those players who are disconnected
//
// There are three four lists in the system:
// - Playback Thread
// - Notify Thread
// - Host Migration List
//

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::UpdateActivePlayPendingList"

void CDirectVoiceClientEngine::UpdateActivePlayPendingList( )
{
    PLIST_ENTRY currentEntry;
    CVoicePlayer *pVoicePlayer;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    // Add players who are pending
    currentEntry = m_PlayAddPlayersList.Flink;

    while( currentEntry != &m_PlayAddPlayersList )
    {
        pVoicePlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_PlayListEntry );

        pVoicePlayer->RemoveFromPlayList();
        pVoicePlayer->AddToPlayList( &m_PlayActivePlayersList );
        m_dwPlayActiveCount++;

        currentEntry = m_PlayAddPlayersList.Flink;
    }

    // Remove players who have disconnected
    currentEntry = m_PlayActivePlayersList.Flink;

    while( currentEntry != &m_PlayActivePlayersList )
    {
        pVoicePlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_PlayListEntry );

        currentEntry = currentEntry->Flink;

        // If current player has disconnected, remove them from active list
        // and release the reference the list has
        if( pVoicePlayer->IsDisconnected() )
        {
            m_dwPlayActiveCount--;            
            pVoicePlayer->RemoveFromPlayList();

            pVoicePlayer->Release();
        }
    }

    DPVF_EXIT();

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::UpdateActiveNotifePendingList"

void CDirectVoiceClientEngine::UpdateActiveNotifyPendingList( )
{
    PLIST_ENTRY currentEntry;
    CVoicePlayer *pVoicePlayer;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    // Add players who are pending
    currentEntry = m_NotifyAddPlayersList.Flink;

    while( currentEntry != &m_NotifyAddPlayersList )
    {
        pVoicePlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_NotifyListEntry );

        pVoicePlayer->RemoveFromNotifyList();
        pVoicePlayer->AddToNotifyList( &m_NotifyActivePlayersList );

        currentEntry = m_NotifyAddPlayersList.Flink;
    }

    // Remove players who have disconnected
    currentEntry = m_NotifyActivePlayersList.Flink;

    while( currentEntry != &m_NotifyActivePlayersList )
    {
        pVoicePlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_NotifyListEntry );

        currentEntry = currentEntry->Flink;

        // If current player has disconnected, remove them from active list
        // and release the reference the list has
        if( pVoicePlayer->IsDisconnected() )
        {
            pVoicePlayer->RemoveFromNotifyList();
            pVoicePlayer->Release();
        }
    }

    DPVF_EXIT();

}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::DoPlaybackWork"
HRESULT CDirectVoiceClientEngine::DoPlaybackWork()
{

    DWORD               dwCurPlayer;
    BOOL                bContinueEnum;
    DVPROTOCOLMSG_PLAYERQUIT    dvPlayerQuit;
    HRESULT             hr = DV_OK;
    DWORD               dwTmp;
    DVMSG_PLAYERVOICESTART dvMsgPlayerVoiceStart;
    DVMSG_PLAYERVOICESTOP  dvMsgPlayerVoiceStop;
    CVoicePlayer        *pCurrentPlayer;
    PLIST_ENTRY         currentEntry;
    BOOL                fSilence, fLost;
    DWORD               dwSeqNum, dwMsgNum;
    CFrame              *frTmpFrame;
    DWORD               dwFrameFlags;
    BYTE                bHighPeak = 0;
    DWORD               dwCurrTime;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();

    //
    // BUGBUG make available time equal zero to force this function to do single quantum of work
    //

    dvMsgPlayerVoiceStart.dwSize = sizeof( DVMSG_PLAYERVOICESTART );
    dvMsgPlayerVoiceStop.dwSize = sizeof( DVMSG_PLAYERVOICESTOP );

    do
    {

        // Update list 
        UpdateActivePlayPendingList(  );

        currentEntry = m_PlayActivePlayersList.Flink;

        while( currentEntry != &m_PlayActivePlayersList )
        {
            pCurrentPlayer = CONTAINING_RECORD( currentEntry, CVoicePlayer, m_PlayListEntry );

            currentEntry = currentEntry->Flink;

            if( m_dvClientConfig.dwFlags & DVCLIENTCONFIG_PLAYBACKMUTE )
            {
                frTmpFrame = pCurrentPlayer->Dequeue();
                frTmpFrame->Return();
                continue;
            }

            hr = pCurrentPlayer->GetNextFrameAndDecompress(&dwFrameFlags, &dwSeqNum, &dwMsgNum );

            if( FAILED( hr ) )
            {
                DPVF( DPVF_ERRORLEVEL, "Unable to get frame from player hr=0x%x", hr );
                goto EXIT_PLAYBACK;
            }


            if( pCurrentPlayer->GetLastPeak() > bHighPeak )
            {
                bHighPeak = pCurrentPlayer->GetLastPeak();
            }

#if DBG
            if( dwFrameFlags & DV_FRAMEF_IS_LOST )
            {
                DPVF( DPVF_CLIENT_SEQNUM_DEBUG_LEVEL, "SEQ: Dequeue: Lost Frame" );  
                DPVF( DPVF_GLITCH_DEBUG_LEVEL, "GLITCH: Dequeue: Packet was lost.  Speech gap will occur." );
            }
            else if( dwFrameFlags & DV_FRAMEF_IS_SILENCE)
            {
                DPVF( DPVF_CLIENT_SEQNUM_DEBUG_LEVEL, "SEQ: Dequeue: Silent Frame" );    
            }
            else
            {
                DPVF( DPVF_CLIENT_SEQNUM_DEBUG_LEVEL, "SEQ: Dequeue: Msg [%d] Seq [%d]", dwMsgNum, dwSeqNum );
            }
#endif

            // If the player sent us silence, increment the silent count
            if( dwFrameFlags & DV_FRAMEF_IS_SILENCE )
            {

                //
                // If we're receiving on this user
                //

                if( pCurrentPlayer->IsReceiving() )
                {

                    //
                    // If it exceeds the max.
                    //

                    dwCurrTime = GetTickCount();

                    if( (dwCurrTime - pCurrentPlayer->GetLastPlayback()) > PLAYBACK_RECEIVESTOP_TIMEOUT )
                    {
                        pCurrentPlayer->SetReceiving(FALSE);

                        dvMsgPlayerVoiceStop.dwSize = sizeof( dvMsgPlayerVoiceStop );
                        dvMsgPlayerVoiceStop.dvidSourcePlayerID = pCurrentPlayer->GetPlayerID();
                        dvMsgPlayerVoiceStop.pvPlayerContext = pCurrentPlayer->GetContext();
                        NotifyQueue_Add( DVMSGID_PLAYERVOICESTOP, &dvMsgPlayerVoiceStop, sizeof( dvMsgPlayerVoiceStop ) );      
                    }
                }

                bHighPeak = 0;
            }
            else
            {
                //
                // We received data and this is the first one
                //

                if( !pCurrentPlayer->IsReceiving() )
                {

                    dvMsgPlayerVoiceStart.dvidSourcePlayerID = pCurrentPlayer->GetPlayerID();
                    dvMsgPlayerVoiceStart.pvPlayerContext = pCurrentPlayer->GetContext();
                    
                    NotifyQueue_Add( DVMSGID_PLAYERVOICESTART, &dvMsgPlayerVoiceStart, sizeof(DVMSG_PLAYERVOICESTART) );

                    pCurrentPlayer->SetReceiving(TRUE);

                }
            }

            
        } // for each player

        //
        // we go through this loop only once..
        //

    } while (FALSE);

    m_bLastPlaybackPeak = bHighPeak;

EXIT_PLAYBACK:

    DPVF_EXIT();
    return hr;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::MigrateHost"
// MigrateHost
// 
// This function is called by DV_HostMigrate when the local client has received a host
// migration notification.  
//
//
HRESULT CDirectVoiceClientEngine::MigrateHost( DVID dvidNewHost, LPDIRECTPLAYVOICESERVER lpdvServer )
{
    return DV_OK;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyQueue_IndicateNext"
HRESULT CDirectVoiceClientEngine::NotifyQueue_IndicateNext()
{
    HRESULT hr = DV_OK; 
    CNotifyElement *neElement;

    while (!IsListEmpty(&m_NotifyList)) {

        neElement = (CNotifyElement *) DV_REMOVE_TAIL(&m_NotifyList);
    
        if( FAILED( neElement == NULL ))
        {
            DPVF( DPVF_ERRORLEVEL, "NotifyQueue Empty!");
        }
        else
        {
            DPVF( DPVF_SPAMLEVEL, "Sending notification type=0x%x", neElement->m_dwType );
    
            TransmitMessage( neElement->m_dwType, 
                             &neElement->m_element, 
                             neElement->m_dwDataSize );                 
    
            DPVF( DPVF_SPAMLEVEL, "Returning notification" );
    
            //
            // return element to pool
            //
    
            NotifyQueue_ElementFree( neElement );
        }

    }

    DPVF_EXIT();
    return hr;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyQueue_Add"
// Queue up a notification for the user
HRESULT CDirectVoiceClientEngine::NotifyQueue_Add( DWORD dwMessageType, LPVOID lpData, DWORD dwDataSize, PVOID pvContext, CNotifyElement::PNOTIFY_COMPLETE pNotifyFunc )
{
    CNotifyElement *lpNewElement;
    HRESULT hr;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    if( !m_fNotifyQueueEnabled )
    {
        DPVF( DPVF_WARNINGLEVEL, "Ignoring indication, queue disabled" );
        DPVF_EXIT();
        return DV_OK;
    }

    lpNewElement = (CNotifyElement *) DV_REMOVE_TAIL(&m_NotificationElementPool);

    if( lpNewElement == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Failed to get a block for a notifier" );
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY;
    }

    // Setup the notification callbacks, if there are ones
    lpNewElement->pvContext = pvContext; 
    lpNewElement->pNotifyFunc = pNotifyFunc;

#if DBG
    if( dwMessageType == DVMSGID_PLAYERVOICESTOP )
    {
        PDVMSG_PLAYERVOICESTOP pMsgStop = (PDVMSG_PLAYERVOICESTOP) lpData;

        ASSERT( pMsgStop->dwSize == sizeof( DVMSG_PLAYERVOICESTOP ) );
        ASSERT( dwDataSize == sizeof( DVMSG_PLAYERVOICESTOP ) );        
    }

    if( dwMessageType == DVMSGID_PLAYERVOICESTART )
    {
        PDVMSG_PLAYERVOICESTART pMsgStart = (PDVMSG_PLAYERVOICESTART) lpData;

        ASSERT( pMsgStart->dwSize == sizeof( DVMSG_PLAYERVOICESTART ) );
        ASSERT( dwDataSize == sizeof( DVMSG_PLAYERVOICESTART ) );       
    }

    if( dwMessageType == DVMSGID_PLAYEROUTPUTLEVEL )
    {
        PDVMSG_PLAYEROUTPUTLEVEL pMsgOutput = (PDVMSG_PLAYEROUTPUTLEVEL) lpData;

        ASSERT( pMsgOutput->dwSize == sizeof( DVMSG_PLAYEROUTPUTLEVEL ) );
        ASSERT( dwDataSize == sizeof( DVMSG_PLAYEROUTPUTLEVEL ) );      
    }
#endif

    lpNewElement->m_dwType = dwMessageType;
    lpNewElement->m_dwDataSize = dwDataSize;

    ASSERT(dwDataSize <= DV_CLIENT_NOTIFY_ELEMENT_SIZE);

    memcpy( &lpNewElement->m_element, lpData, dwDataSize ); 

    // Fixups for internal pointers 
    //
    // Required for certain message types (currently only DVMSGID_SETTARGETS)
    if( dwMessageType == DVMSGID_SETTARGETS )
    {
        PDVMSG_SETTARGETS pdvSetTarget;

        pdvSetTarget = (PDVMSG_SETTARGETS) lpNewElement->m_element.Data;

        pdvSetTarget->pdvidTargets = (PDVID) &pdvSetTarget[1];
        lpNewElement->m_dwDataSize = sizeof( DVMSG_SETTARGETS );
    }

    //
    // add this element to our pending notification list
    //

    DV_INSERT_HEAD(&m_NotifyList,lpNewElement);

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyQueue_ElementFree"
HRESULT CDirectVoiceClientEngine::NotifyQueue_ElementFree( CNotifyElement *lpElement )
{
    DPVF_ENTER();
    // Call the notification function, if there is one
    if( lpElement->pNotifyFunc )
    {
        (*lpElement->pNotifyFunc)(lpElement->pvContext,lpElement);
        lpElement->pNotifyFunc = NULL;
        lpElement->pvContext = NULL;
    }
        
    // Return notifier to the fixed pool manager

    DV_INSERT_TAIL(&m_NotificationElementPool,lpElement);

    DPVF_EXIT();
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyQueue_Enable"
void CDirectVoiceClientEngine::NotifyQueue_Enable()
{
    DV_AUTO_LOCK(&m_CS);
    m_fNotifyQueueEnabled = TRUE;    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyQueue_Disable"
void CDirectVoiceClientEngine::NotifyQueue_Disable()
{
    DV_AUTO_LOCK(&m_CS);
    m_fNotifyQueueEnabled = FALSE;    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::NotifyQueue_Free"
HRESULT CDirectVoiceClientEngine::NotifyQueue_Free()
{
    CNotifyElement *lpTmpElement;
    CNotifyElement *lpIteratorElement;
    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    lpIteratorElement = (CNotifyElement *)DV_GET_LIST_HEAD(&m_NotifyList);

    while( lpIteratorElement != NULL )
    {
        lpTmpElement = lpIteratorElement;
        lpIteratorElement = (CNotifyElement *) DV_GET_NEXT(&m_NotifyList, lpIteratorElement);

        //
        // elementFree calls notigfications functions
        // we lower irql so they are not done at DPC
        //

        NotifyQueue_ElementFree( lpTmpElement );
    }

    DV_FreeList(&m_NotificationElementPool, DVLAT_RAW);

    DPVF_EXIT();
    return DV_OK;
}

//
// HANDLERS FOR WHEN WE HANDLE REMOTE SERVERS
//
//

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::CreateGroup"
HRESULT CDirectVoiceClientEngine::CreateGroup( DVID dvID )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::DeleteGroup"
HRESULT CDirectVoiceClientEngine::DeleteGroup( DVID dvID )
{
    CheckForAndRemoveTarget( dvID );

    // If there are any buffers for this player, delete them
    //
    // Leave the buffer around so the user can call Delete3DSoundBuffer
    //
    // DeleteSoundTarget( dvID );

    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::AddPlayerToGroup"
HRESULT CDirectVoiceClientEngine::AddPlayerToGroup( DVID dvidGroup, DVID dvidPlayer )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::RemovePlayerFromGroup"
HRESULT CDirectVoiceClientEngine::RemovePlayerFromGroup( DVID dvidGroup, DVID dvidPlayer )
{
    return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::DoSessionLost"
void CDirectVoiceClientEngine::DoSessionLost( HRESULT hrDisconnectResult )
{
    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();
    DPVF( DPVF_ERRORLEVEL, "#### SESSION LOST [hr=0x%x]", hrDisconnectResult );
    m_fSessionLost = TRUE;
    SetCurrentState(DVCSTATE_DISCONNECTING);
    DoSignalDisconnect( hrDisconnectResult );

    DPVF_EXIT();
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::DoSignalDisconnect"
void CDirectVoiceClientEngine::DoSignalDisconnect( HRESULT hrDisconnectResult )
{
    DPVF_ENTER();
    DPVF( DPVF_INFOLEVEL, "#### Disconnecting [hr=0x%x]", hrDisconnectResult );

    DV_AUTO_LOCK(&m_CS);

    m_hrDisconnectResult = hrDisconnectResult;
    DPVF_EXIT();
}


#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::GetTransmitBuffer"
PDVTRANSPORT_BUFFERDESC CDirectVoiceClientEngine::GetTransmitBuffer(DWORD dwSize, LPVOID *ppvSendContext, BOOLEAN bUseSpeechPool)
{
    PDVTRANSPORT_BUFFERDESC pNewBuffer = NULL;
    PLIST_ENTRY pEntry;

    DPVF_ENTER();
    DV_AUTO_LOCK(&m_CS);

    pNewBuffer = (PDVTRANSPORT_BUFFERDESC)DV_REMOVE_TAIL(&m_BufferDescPool);

    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: Got a buffer desc address 0x%p", (void *) pNewBuffer );

    if( pNewBuffer == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Error getting transmit buffer" );
        goto GETTRANSMITBUFFER_ERROR;
    }

    pNewBuffer->lRefCount = 0;
    pNewBuffer->dwObjectType = DVTRANSPORT_OBJECTTYPE_CLIENT;
    pNewBuffer->dwFlags = 0;
    
    if (bUseSpeechPool) {

        m_SpeechBufferAllocs++;
        pNewBuffer->pvContext = &m_SpeechBufferPool;
        pNewBuffer->pBufferData = (PUCHAR)DV_REMOVE_TAIL(&m_SpeechBufferPool);
        
    } else {

        m_MsgBufferAllocs++;

        pNewBuffer->pvContext = &m_MessagePool;
        pNewBuffer->pBufferData = (PUCHAR)DV_REMOVE_TAIL(&m_MessagePool);

    }


    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: Got a buffer value at address 0x%p", (void *) pNewBuffer->pBufferData );

    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: nInUse  = %i", IsListEmpty(&m_BufferDescPool));  

    if( pNewBuffer->pBufferData == NULL )
    {
        DPVF( 0, "Error getting buffer for buffer desc" );
        goto GETTRANSMITBUFFER_ERROR;
    }
    
    pNewBuffer->dwBufferSize = dwSize;

    *ppvSendContext = pNewBuffer;

    DPVF_EXIT();
    return pNewBuffer;

GETTRANSMITBUFFER_ERROR:

    if (bUseSpeechPool) {

        m_SpeechBufferAllocs--;
        
    } else {

        m_MsgBufferAllocs--;

    }

    if( pNewBuffer != NULL && pNewBuffer->pBufferData != NULL )
    {
        DV_INSERT_TAIL((PLIST_ENTRY)pNewBuffer->pvContext,
                            pNewBuffer->pBufferData);
    }

    if( pNewBuffer != NULL )
    {
        DV_INSERT_TAIL(&m_BufferDescPool,
                            pNewBuffer);

    }

    DPVF_EXIT();
    return NULL;
    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::ReturnTransmitBuffer"
// ReturnTransmitBuffer
//
// PDVTRANSPORT_BUFFERDESC pBufferDesc - Buffer description of buffer to return
// LPVOID lpvContext - Context value to be used when returning the buffer 
// 
void CDirectVoiceClientEngine::ReturnTransmitBuffer( PVOID pvContext )
{
    PDVTRANSPORT_BUFFERDESC pBufferDesc = (PDVTRANSPORT_BUFFERDESC) pvContext;
    PLIST_ENTRY pListHead = (PLIST_ENTRY) pBufferDesc->pvContext;

    DPVF_ENTER();
    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: Returning a buffer desc at address 0x%p", (void *) pBufferDesc );
    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: Returning a buffer at address 0x%p", (void *) pBufferDesc->pBufferData );

    DV_AUTO_LOCK(&m_CS);

    if (pListHead == &m_SpeechBufferPool) {
        m_SpeechBufferAllocs--;
    }
        
    if (pListHead == &m_MessagePool) {
        m_MsgBufferAllocs--;
    }

    // Release memory
    DV_INSERT_TAIL(pListHead, pBufferDesc->pBufferData);

    // Release buffer description
    DV_INSERT_TAIL(&m_BufferDescPool,pBufferDesc);

    DPVF( DPVF_BUFFERDESC_DEBUG_LEVEL, "BUFFERDESC: nInUse  = %i", IsListEmpty(&m_BufferDescPool) );

    DPVF_EXIT();

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SendComplete"
HRESULT CDirectVoiceClientEngine::SendComplete( PDVEVENTMSG_SENDCOMPLETE pSendComplete )
{
    ReturnTransmitBuffer( pSendComplete->pvUserContext );
    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceClientEngine::SetupInitialBuffers"
// SetupBuffersInitial
//
// This function sets up the first transmit buffers which do not vary
// in size w/the compression type.
//
HRESULT CDirectVoiceClientEngine::SetupInitialBuffers()
{
    HRESULT hr = DV_OK;
    
    DPVF_ENTER();

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

        InitializeListHead(&m_PlayerContextPool);
        hr = DV_InitializeList(&m_PlayerContextPool,
                          m_lpSessionTransport->GetMaxPlayers(),
                          1,
                          DVLAT_CVOICEPLAYER);

    }


    if (FAILED(hr)) {

        DPVF( 0, "Error buffer pools" );
        FreeBuffers();
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY; 
    }

    DPVF_EXIT();
    return DV_OK;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::SetupSpeechBuffer"
// SetupSpeechBuffer
//
// This function sets up the buffer pool for speech sends, whose size will
// depend on the compression type.  Must be done after we know CT but 
// before we do first speech transmission.
//
HRESULT CDirectVoiceClientEngine::SetupSpeechBuffer()
{
    ULONG size;
    HRESULT hr;
    DPVF_ENTER();

    if( m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER)
    {
        size = sizeof( DVPROTOCOLMSG_SPEECHHEADER )+s_dwCompressedFrameSize+COMPRESSION_SLUSH;
    }
    else
    {
        size = sizeof( DVPROTOCOLMSG_SPEECHWITHTARGET ) + s_dwCompressedFrameSize + 
            (sizeof( DVID )*CLIENT_POOLS_NUM_TARGETS_BUFFERED)+COMPRESSION_SLUSH;
    }

    InitializeListHead(&m_SpeechBufferPool);
    hr = DV_InitializeList(&m_SpeechBufferPool,
                      m_lpSessionTransport->GetMaxPlayers()*DV_CT_FRAMES_PER_PERIOD,
                      size,
                      DVLAT_RAW);

    if(FAILED(hr))
    {
        DPVF( 0, "Error creating transmit buffers" );
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY;
    }

    m_pSpeechBufferAllocation = m_SpeechBufferPool.Flink;

    DPVF_EXIT();
    return DV_OK;

}
  
#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::FreeBuffers"
HRESULT CDirectVoiceClientEngine::FreeBuffers()
{

    DPVF_ENTER();

    DV_FreeList(&m_BufferDescPool, DVLAT_RAW);
    DV_FreeList(&m_MessagePool, DVLAT_RAW);
    DV_FreeList(&m_PlayerContextPool, DVLAT_CVOICEPLAYER);
    DV_FreeList(&m_SpeechBufferPool, DVLAT_RAW);

    DV_POOL_FREE(m_pBufferDescAllocation);
    DV_POOL_FREE(m_pMessagePoolAllocation);
    DV_POOL_FREE(m_pSpeechBufferAllocation);

    m_pBufferDescAllocation = NULL;
    m_pMessagePoolAllocation = NULL;
    m_pSpeechBufferAllocation = NULL;
    
    DPVF_EXIT();
    return DV_OK;
    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceServerEngine::CheckConnected"

HRESULT CDirectVoiceClientEngine::CheckConnected()
{

    DPVF_ENTER();
    if(m_dwCurrentState == DVCSTATE_NOTINITIALIZED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not initialized" );
        return DVERR_NOTINITIALIZED;
    }

    if(m_dwCurrentState != DVCSTATE_CONNECTED )
    {
        DPVF( DPVF_ERRORLEVEL, "Not connected" );
        return DVERR_NOTCONNECTED;
    }

    DPVF_EXIT();
    return DV_OK;
}

