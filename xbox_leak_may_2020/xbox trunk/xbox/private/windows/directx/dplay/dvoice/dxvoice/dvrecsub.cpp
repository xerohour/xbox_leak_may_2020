/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ClientRecordSubSystem.cpp
 *  Content:    Recording sub-system.
 *
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 * 09/01/2000  georgioc started rewrite for xbox 
 *
 ***************************************************************************/
#include "dvrecsub.h"
#include "sndutils.h"
#include "in_core.h"
#include "dvengine.h"
#include "va.h"

#define RECORD_MAX_RESETS       10
#define RECORD_MAX_TIMEOUT      2000
#define TESTMODE_MAX_TIMEOUT    500
#define RECORD_NUM_TARGETS_INIT 0
#define RECORD_NUM_TARGETS_GROW 10

// RECORDTEST_MIN_POWER / RECORDTEST_MAX_POWER
//
// Define the max and min possible power values
//#define RECORDTEST_MIN_POWER                0
//#define RECORDTEST_MAX_POWER                100

// We have to double the # because IsMuted is called twice / pass
#define RECORDTEST_NUM_FRAMESBEFOREVOICE      6


#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::CClientRecordSubSystem"
// CClientRecordSubSystem
//
// This is the constructor for the CClientRecordSubSystem class. 
// It intiailizes the classes member variables with the appropriate
// values.
//
// Parameters:
// CDirectVoiceClientEngine *clientEngine -
//      Pointer to the client object which is using this object.
//
// Returns:
// N/A
//
CClientRecordSubSystem::CClientRecordSubSystem( 
    CDirectVoiceClientEngine *clientEngine 
    ): m_recordState(RECORDSTATE_NOTSTARTED),
       m_pConverter(NULL),
       m_pCaptureMediaObject(NULL),
       m_pwfxConverter(NULL),
       m_clientEngine(clientEngine),
       m_dwCurrentPower(0),
       m_dwSilentTime(0),
       m_lastFrameTransmitted(FALSE),
       m_msgNum(0),
       m_seqNum(0),
       m_dwLastTargetVersion(0),
       m_dwResetCount(0),
       m_dwNumSinceRestart(0), 
       m_pVa(NULL),
       m_dwFrameCount(0),
       m_dwPort(0),
       m_pMediaPacketBuffer(NULL)
{

    InitializeListHead(&m_PendingList);
    InitializeListHead(&m_CompletedList);    

}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::Initialize"
HRESULT CClientRecordSubSystem::Initialize()
{
    HRESULT hr;
    ULONG i;
    XMEDIAPACKET xmp;
    PDVMEDIAPACKET pPacket,pNextPacket;
    WAVEFORMATEX wfx;
    

    DPVF_ENTER();

    //
    // create encoder
    //

    memcpy(&wfx, m_clientEngine->s_lpwfxPrimaryFormat,sizeof(wfx));
    hr = DV_CreateConverter(m_clientEngine->m_dvSessionDesc.guidCT,
                            &m_pConverter,
                            &wfx,
                            &m_pwfxConverter,
                            m_clientEngine->s_dwUnCompressedFrameSize,
                            TRUE);

    if( FAILED( hr ) )
    {
        DPVF( DPVF_ERRORLEVEL, "Unable to create converter.  hr = 0x%x", hr );
        return hr ;
    }

    m_transmitFrame     = FALSE;
    m_dwResetCount = 0;

    m_dwLastFrameTime   = GetTickCount();
    m_dwFrameTime       = ((PVOICECODECWAVEFORMAT)m_pwfxConverter)->dwMilliSecPerFrame;
    m_dwSilentTime      = ((PVOICECODECWAVEFORMAT)m_pwfxConverter)->dwTrailFrames*m_dwFrameTime + 1;    
    m_dwSilenceTimeout  = m_dwSilentTime-1;

    m_dwPort = m_clientEngine->m_dvClientConfig.dwPort;

    //
    // retrieve hawk output (microphone) XMO
    //
    
    memcpy(&wfx, m_clientEngine->s_lpwfxPrimaryFormat,sizeof(wfx));

    hr = XVoiceCreateMediaObjectInternal(
        XDEVICE_TYPE_VOICE_MICROPHONE,
        m_dwPort,
        DV_CT_FRAMES_PER_PERIOD,
        &wfx, // IN parameter
        DV_RecordSubSystemPacketCompletion,
        this,
        &m_pCaptureMediaObject);

    PAGED_CODE();

    if (FAILED(hr)) {
        DPVF( DPVF_ERRORLEVEL, "Could not instatiate hawk XMO!" );
        ASSERT(FALSE);
        return hr;
    }

    // create and init the AGC and VA algorithm class
    m_pVa = new CVoiceActivation1();
    if (m_pVa == NULL)
    {
        DPVF( DPVF_ERRORLEVEL, "Memory alloc failure" );
        DPVF_EXIT();

        return DVERR_OUTOFMEMORY;
    }

    hr = m_pVa->Init(
            m_clientEngine->m_dvClientConfig.dwFlags, 
            m_wfxCapture.nSamplesPerSec,
            m_clientEngine->m_dvClientConfig.dwThreshold);

    if (FAILED(hr))
    {
        DPVF( DPVF_ERRORLEVEL, "Error initializing AGC and/or VA algorithm, code: %i", hr);
        DPVF_EXIT();

        delete m_pVa;
        return hr;
    }
    
    m_dwLastTargetVersion = m_clientEngine->m_dwTargetVersion;

    //
    // Alloc a bunch of media buffers
    //

    hr = DV_InitializeList(&m_PendingList,
                          DV_CT_FRAMES_PER_PERIOD,
                          sizeof(DVMEDIAPACKET),
                          DVLAT_RAW);

    if (FAILED(hr)) {
        DPVF_EXIT();

        return hr;
    }

    m_pMediaPacketBuffer = m_PendingList.Flink;

    hr = DV_InitMediaPacketPool(&m_PendingList,
                                m_clientEngine->s_dwUnCompressedFrameSize,
                                TRUE);

    if (FAILED(hr)) {

        DPVF_EXIT();

        return hr;

    }

    DPVF_EXIT();

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::~CClientRecordSubSystem"
// CClientRecordSubSystem
//
// This is the destructor for the CClientRecordSubSystem
// class.  It cleans up the allocated memory for the class
// and stops the recording device.
//
// Parameters:
// N/A
//
// Returns:
// N/A
//
CClientRecordSubSystem::~CClientRecordSubSystem()
{
    HRESULT hr;
    PDVMEDIAPACKET pPacket;

    DPVF_ENTER();

    if( m_pConverter != NULL )
        m_pConverter->Release();

    //
    // stop recording and force any pending buffers to complete
    //

    if (m_pCaptureMediaObject) {

        hr = m_pCaptureMediaObject->Flush();
        ASSERT(!FAILED(hr));
    
        //
        // free the capture device
        //
    
        m_pCaptureMediaObject->Release();

    }

    // Deinit and cleanup the AGC and VA algorthms
    if (m_pVa != NULL)
    {
        hr = m_pVa->Deinit();
        if (FAILED(hr))
        {
            DPVF(DPVF_ERRORLEVEL, "Deinit error on AGC and/or VA algorithm, code: %i", hr);
        }
        delete m_pVa;
    }

    //
    // delete all pending buffers and their data
    // all buffers should be in the pending pool...
    //

    DV_FreeMediaPacketPool(&m_PendingList,TRUE);

    DV_FreeList(&m_PendingList,DVLAT_RAW);
    DV_FreeList(&m_CompletedList, DVLAT_RAW);

    if (m_pMediaPacketBuffer) {

        DV_POOL_FREE(m_pMediaPacketBuffer);
        m_pMediaPacketBuffer = NULL;

    }

    DPVF_EXIT();

}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::IsMuted"

BOOL CClientRecordSubSystem::IsMuted()
{
    KIRQL irql;
    BOOL fMuted =  FALSE;

    DPVF_ENTER();

    if( m_dwFrameCount < RECORDTEST_NUM_FRAMESBEFOREVOICE )
    {
        DPVF( RRI_DEBUGOUTPUT_LEVEL, "Skipping first %d frames for startup burst", RECORDTEST_NUM_FRAMESBEFOREVOICE );
        m_dwFrameCount++;
        DPVF_EXIT();

        return TRUE;
    }
    
    fMuted = (m_clientEngine->m_dvClientConfig.dwFlags & DVCLIENTCONFIG_RECORDMUTE);

    DPVF_EXIT();

    return fMuted;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::StartMessage()"
void CClientRecordSubSystem::StartMessage() 
{
    BYTE bPeakLevel;
    
    DPVF_ENTER();

    if( m_clientEngine->m_dvClientConfig.dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED || 
       m_clientEngine->m_dvClientConfig.dwFlags & DVCLIENTCONFIG_MANUALVOICEACTIVATED )
    {
        m_pVa->PeakResults(&bPeakLevel);
    }
    else
    {
        bPeakLevel = 0;
    }

    DVMSG_RECORDSTART dvRecordStart;
    dvRecordStart.dwPeakLevel = bPeakLevel;
    dvRecordStart.dwSize = sizeof( DVMSG_RECORDSTART );
    dvRecordStart.pvLocalPlayerContext = m_clientEngine->m_pvLocalPlayerContext;
    
    m_clientEngine->NotifyQueue_Add( DVMSGID_RECORDSTART, &dvRecordStart, sizeof( DVMSG_RECORDSTART ) );

    DPVF( RRI_DEBUGOUTPUT_LEVEL, "RRI, Starting Message" );

    DPVF_EXIT();

    
}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::EndMessage()"
void CClientRecordSubSystem::EndMessage()
{
    BYTE bPeakLevel;
    
    DPVF_ENTER();

    if( m_clientEngine->m_dvClientConfig.dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED || 
       m_clientEngine->m_dvClientConfig.dwFlags & DVCLIENTCONFIG_MANUALVOICEACTIVATED )
    {
        m_pVa->PeakResults(&bPeakLevel);
    }
    else
    {
        bPeakLevel = 0;
    }

    DVMSG_RECORDSTOP dvRecordStop;
    dvRecordStop.dwPeakLevel = bPeakLevel;
    dvRecordStop.dwSize = sizeof( DVMSG_RECORDSTOP );
    dvRecordStop.pvLocalPlayerContext = m_clientEngine->m_pvLocalPlayerContext; 
    
    m_clientEngine->NotifyQueue_Add( DVMSGID_RECORDSTOP, &dvRecordStop, sizeof( DVMSG_RECORDSTOP ) );

    m_msgNum++;
    m_seqNum = 0;   

    DPVF( RRI_DEBUGOUTPUT_LEVEL, "RRI, Ending Message" );
    DPVF_EXIT();

}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::CheckVA()"
BOOL CClientRecordSubSystem::CheckVA()
{
    BOOL m_fVoiceDetected;
    KIRQL irql;

    DPVF_ENTER();

    // We've been muted, turn off the VA
    if( IsMuted() )
    {
        RIRQL(irql);
        m_dwSilentTime = m_dwSilenceTimeout+1;  
        LIRQL(irql);

        DPVF_EXIT();

        return FALSE;
    }

    RIRQL(irql);
    m_pVa->VAResults(&m_fVoiceDetected);
    if (!m_fVoiceDetected)
    {
        // This prevents wrap-around on silence timeout
        if( m_dwSilentTime <= m_dwSilenceTimeout )
        {
            m_dwSilentTime += m_dwFrameTime;
        }

        DPVF( DPVF_SPAMLEVEL, "### Silence Time to %d", m_dwSilentTime );        

        if( m_dwSilentTime > m_dwSilenceTimeout )
        {
            DPVF( DPVF_SPAMLEVEL, "### Silence Time exceeded %d", m_dwSilenceTimeout );              
            LIRQL(irql);
            DPVF_EXIT();

            return FALSE;
        }
    }
    else
    {
        m_dwSilentTime = 0;
        DPVF( DPVF_SPAMLEVEL, "### Transmit!!!!" );
    }

    LIRQL(irql);
    DPVF_EXIT();

    return TRUE;
}

VOID DV_RecordSubSystemPacketCompletion(
    PVOID pThis,
    PVOID pContext,
    DWORD dwStatus)
{
    CClientRecordSubSystem *This = (CClientRecordSubSystem *) pThis;
    PDVMEDIAPACKET pPacket = (PDVMEDIAPACKET) pContext;

    DPVF( DPVF_SPAMLEVEL, "DV_recordsubSystemPacketCompletion(%x), Packet = %x",
         This,
         pPacket);

    //
    // add packet to the completed list, remove from pending list
    //

    DV_REMOVE_ENTRY(pPacket);
    DV_INSERT_HEAD(&This->m_CompletedList,pPacket);

}



#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::RecordFSM"
HRESULT CClientRecordSubSystem::RecordFSM() 
{
    DWORD fIsMuted;
    PDVMEDIAPACKET pPacket, pNextPacket;
    HRESULT hr;
    ULONG i;

    DPVF_ENTER();

    //
    // if this is the very first time we are called, attach packets to hawk and bail..
    //

    if (m_recordState == RECORDSTATE_NOTSTARTED) {

        DPVF( DPVF_WARNINGLEVEL, "Entering record for first time. attaching packets to Hawk" );

        pPacket = (PDVMEDIAPACKET) DV_GET_LIST_HEAD(&m_PendingList);
    
        for (i=0;
             i<DV_CT_FRAMES_PER_PERIOD;
             i++){
            
            //
            // before we attach get next link, since XMO might complete packet
            // an dinsert on another list..
            //
    
            pNextPacket = (PDVMEDIAPACKET) DV_GET_NEXT(&m_PendingList,pPacket);
            
            //
            // attach buffer to the XMO
            //
    
            hr = m_pCaptureMediaObject->Process(NULL,
                                                &pPacket->xmp);
    
            if (FAILED(hr)) {

                DPVF( DPVF_ERRORLEVEL, "Attach to voice xmo failed with %x\n",hr );

                
                DPVF_EXIT();

                return hr;
            }
    
            pPacket = pNextPacket;
            
    
        }

        m_recordState = RECORDSTATE_IDLE;
        DPVF_EXIT();

        return DV_OK;

    }

    do {

        //
        // get a buffer from the completed list
        //

        if (!IsListEmpty(&m_CompletedList)) {

            pPacket = (PDVMEDIAPACKET) DV_REMOVE_TAIL(&m_CompletedList);

        } else {

            //
            // we have no data
            //

            DPVF( DPVF_SPAMLEVEL, "### No data available from hawk yet!" );

            DPVF_EXIT();

            return DV_OK;

        }

        fIsMuted = IsMuted();

        //
        // In this case we NEVER transmit
        // Shortcut to simplify the other cases
        //

        if( fIsMuted || !IsValidTarget() )
        {
            //
            // Go immediately to IDLE
            //

            DPVF( DPVF_RECORDLEVEL, "### IsMuted || IsValidTarget --> IDLE" );
            m_recordState = RECORDSTATE_IDLE;

        }
    
        if( !fIsMuted )
        {
            //
            // before we analyze the data, push down the current
            // relevant portions of the client config structure.
            //

            m_pVa->SetSensitivity(m_clientEngine->m_dvClientConfig.dwFlags,
                                  m_clientEngine->m_dvClientConfig.dwThreshold);

            m_pVa->AnalyzeData((PUCHAR)pPacket->xmp.pvBuffer,
                               m_clientEngine->s_dwUnCompressedFrameSize);

        }

        m_transmitFrame = FALSE;    
    
        switch( m_recordState )
        {
        case RECORDSTATE_IDLE:
    
            DPVF( DPVF_RECORDLEVEL, "### STATE: IDLE" );
    
            if( !fIsMuted&& IsValidTarget() )
            {
                if( IsPTT() )
                {
                    m_recordState = RECORDSTATE_PTT;
                    m_transmitFrame = TRUE;
                }
                else
                {
                    m_transmitFrame = CheckVA();
    
                    if( m_transmitFrame )
                    {
                        m_recordState = RECORDSTATE_VA;
                    }
    
                }
            }
    
            break;
            
        case RECORDSTATE_VA:
    
            DPVF( DPVF_RECORDLEVEL, "### STATE: VA" ); 
    
            if( IsPTT() )
            {
                DPVF( DPVF_RECORDLEVEL, "### VA --> PTT" );                
                m_recordState = RECORDSTATE_PTT;
                m_transmitFrame = TRUE;
            }
            else
            {
                m_transmitFrame = CheckVA();
                if (!m_transmitFrame)
                {
                    DPVF( DPVF_RECORDLEVEL, "### !VA --> IDLE" );      
                    m_recordState = RECORDSTATE_IDLE;
                }
            }
    
            break;
    
        case RECORDSTATE_PTT:
    
            DPVF( DPVF_RECORDLEVEL, "### STATE: PTT" );
    
            if( IsVA() )
            {
                DPVF( DPVF_RECORDLEVEL, "### PTT --> VA" );                            
                m_recordState = RECORDSTATE_VA;
                m_transmitFrame = CheckVA();
            }
            else
            {
                m_transmitFrame = TRUE;
            }
    
            break;
        }
    
        // Message Ended
        if( m_lastFrameTransmitted && !m_transmitFrame )
        {
            EndMessage();
        }
        // Message Started
        else if( !m_lastFrameTransmitted && m_transmitFrame )
        {
            StartMessage();
        }
        // Message Continuing
        else
        {
            // If the target has changed since the last frame
            if( m_clientEngine->m_dwTargetVersion != m_dwLastTargetVersion )
            {
                // If we're going to be transmitting
                if( m_transmitFrame )
                {
                    EndMessage();
                    StartMessage();
                }
            }
        }
    
        m_lastFrameTransmitted = m_transmitFrame;
        m_dwLastTargetVersion = m_clientEngine->m_dwTargetVersion;
    
        // Save the peak level to propogate up to the app
        m_pVa->PeakResults(&(m_clientEngine->m_bLastPeak));

        hr = TransmitFrame(pPacket);
        if (FAILED(hr)) {
            DPVF( DPVF_ERRORLEVEL, "### TransmitFrame failed with hr %x! ###",hr);

            //
            // put packet back to pending list, we have to discard the hawk data..
            //

        }

        //
        // re-attach this buffer to hawk and return this buffer to the pending pool
        //

        pPacket->dwCompletedSize = 0;
        DV_INSERT_TAIL(&m_PendingList,pPacket);

        //
        // attach buffer to the XMO
        //

        hr = m_pCaptureMediaObject->Process(NULL,
                                            &pPacket->xmp);

        if (FAILED(hr)) {

            DPVF( DPVF_ERRORLEVEL, "Attach to voice xmo failed with %x\n",hr );            
            DPVF_EXIT();

            return hr;
        }

        //
        // we do a single quantum of work 
        //

    } while (FALSE);

    DPVF_EXIT();

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::TransmitFrame"
// TransmitFrame
//
// This function looks at the state of the FSM, and if the latest
// frame is to be transmitted it compresses it and transmits
// it to the server (cleint/server mode) or to all the players
// (or player if whispering) in peer to peer mode.  If the frame
// is not to be transmitted this function does nothing.  
//
// It is also responsible for ensuring any microphone clicks are
// mixed into the audio stream.
//
// This function also updates the transmission statisics and
// the current transmission status.  
//
// Parameters:
// N/A
//
// Returns:
// bool - always returns true at the moment
HRESULT CClientRecordSubSystem::TransmitFrame(PDVMEDIAPACKET pPacket) 
{
    HRESULT hr = DV_OK;

    DPVF_ENTER();

    if( m_transmitFrame) 
    {
        
        if( m_clientEngine->m_dvSessionDesc.dwSessionType == DVSESSIONTYPE_PEER )
        {
            hr = BuildAndTransmitSpeechHeader(pPacket);
        }
        else
        {
            hr = BuildAndTransmitSpeechWithTarget(pPacket);
        }
    }

    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::BuildAndTransmitSpeechHeader"
HRESULT CClientRecordSubSystem::BuildAndTransmitSpeechHeader(PDVMEDIAPACKET pPacket)
{
    PDVPROTOCOLMSG_SPEECHHEADER pdvSpeechHeader;
    HRESULT hr;
    DWORD dwTargetSize;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    LPVOID pvSendContext;
    XMEDIAPACKET dstMediaPacket;

    DPVF_ENTER();

    pBufferDesc = m_clientEngine->GetTransmitBuffer(
                      sizeof(DVPROTOCOLMSG_SPEECHHEADER)+\
                      COMPRESSION_SLUSH+m_clientEngine->s_dwCompressedFrameSize,
                      &pvSendContext, TRUE );

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Failed to get buffer for transmission" );
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY;
    }

    pdvSpeechHeader = (PDVPROTOCOLMSG_SPEECHHEADER) pBufferDesc->pBufferData;

    pdvSpeechHeader->dwType = DVMSGID_SPEECH;       
    pdvSpeechHeader->bMsgNum = m_msgNum;
    pdvSpeechHeader->bSeqNum = m_seqNum;

    DPVF( DPVF_CLIENT_SEQNUM_DEBUG_LEVEL, "SEQ: Record:  Msg [%d] Seq [%d]", m_msgNum, m_seqNum );

    
    dwTargetSize = m_clientEngine->s_dwCompressedFrameSize;

    pPacket->dwCompletedSize = 0;

    //
    // prepare DST bufer
    //

    memset(&dstMediaPacket,0,sizeof(XMEDIAPACKET));
    dstMediaPacket.dwMaxSize = dwTargetSize;
    dstMediaPacket.pdwCompletedSize = &dwTargetSize;
    dstMediaPacket.pvBuffer = &pdvSpeechHeader[1];

    hr = m_pConverter->Process(&pPacket->xmp,
                               &dstMediaPacket);

    ASSERT(dwTargetSize == m_clientEngine->s_dwCompressedFrameSize);

    if( FAILED( hr ) )
    {
        m_clientEngine->ReturnTransmitBuffer( pvSendContext );        
        DPVF( DPVF_ERRORLEVEL, "Failed to perform conversion hr=0x%x", hr );
        DPVF_EXIT();
        return hr;
    }

    if( m_clientEngine->m_dwNumTargets == 0 )
    {
        DPVF( DPVF_INFOLEVEL, "Targets set to NONE since FSM.  Not transmitting" );
        m_clientEngine->ReturnTransmitBuffer( pvSendContext );
        DPVF_EXIT();
        return DV_OK;
    }
    else
    {
        pBufferDesc->dwBufferSize = sizeof(DVPROTOCOLMSG_SPEECHHEADER)+dwTargetSize;    
            
        DPVF( DPVF_RECORDLEVEL, "Transmitting %d bytes to target list", pBufferDesc->dwBufferSize );       
        hr = m_clientEngine->m_lpSessionTransport->SendToIDS( m_clientEngine->m_pdvidTargets, m_clientEngine->m_dwNumTargets,
                                                              pBufferDesc, pvSendContext, 0 );
        if( hr == DVERR_PENDING )
        {
            hr = DV_OK;
        }
        else if ( FAILED( hr ))
        {
            DPVF( DPVF_INFOLEVEL, "Send failed hr=0x%x", hr );
            m_clientEngine->ReturnTransmitBuffer( pvSendContext );
        }

        m_seqNum++; 
        
    }

    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::BuildAndTransmitSpeechWithTarget"
HRESULT CClientRecordSubSystem::BuildAndTransmitSpeechWithTarget(PDVMEDIAPACKET pPacket)
{
    PDVPROTOCOLMSG_SPEECHWITHTARGET pdvSpeechWithTarget;
    HRESULT hr;
    DWORD dwTargetSize;
    DWORD dwTransmitSize;
    DWORD dwTargetInfoSize;
    PBYTE pbBuilderLoc;
    PDVTRANSPORT_BUFFERDESC pBufferDesc;
    LPVOID pvSendContext;
    XMEDIAPACKET dstMediaPacket;

    DPVF_ENTER();

    dwTransmitSize = sizeof( DVPROTOCOLMSG_SPEECHWITHTARGET );

    // Calculate size we'll need for storing the targets

    dwTargetInfoSize = sizeof( DVID ) * m_clientEngine->m_dwNumTargets;    

    if( dwTargetInfoSize == 0 )
    {
        DPVF( DPVF_INFOLEVEL, "Targets set to NONE since FSM.  Not transmitting" );

        DPVF_EXIT();
        return DV_OK;
    }
    else
    {
        dwTransmitSize += dwTargetInfoSize; 
    }

    DPVF( DPVF_CLIENT_SEQNUM_DEBUG_LEVEL, "SEQ: Record:  Msg [%d] Seq [%d]", m_msgNum, m_seqNum );   

    dwTransmitSize += m_clientEngine->s_dwCompressedFrameSize;

    pBufferDesc = m_clientEngine->GetTransmitBuffer(
                      sizeof(DVPROTOCOLMSG_SPEECHWITHTARGET)+\
                      m_clientEngine->s_dwCompressedFrameSize+COMPRESSION_SLUSH+dwTransmitSize,
                      &pvSendContext,TRUE);

    if( pBufferDesc == NULL )
    {
        DPVF( DPVF_ERRORLEVEL, "Failed to get buffer for transmission" );
        DPVF_EXIT();
        return DVERR_OUTOFMEMORY;
    }

    pdvSpeechWithTarget = (PDVPROTOCOLMSG_SPEECHWITHTARGET) pBufferDesc->pBufferData;   
    pbBuilderLoc = (PBYTE) &pdvSpeechWithTarget[1];

    memcpy( pbBuilderLoc, m_clientEngine->m_pdvidTargets, dwTargetInfoSize );

    pdvSpeechWithTarget->dwNumTargets = m_clientEngine->m_dwNumTargets;

    pbBuilderLoc += dwTargetInfoSize;

    pdvSpeechWithTarget->dvHeader.dwType = DVMSGID_SPEECHWITHTARGET;        
    pdvSpeechWithTarget->dvHeader.bMsgNum = m_msgNum;
    pdvSpeechWithTarget->dvHeader.bSeqNum = m_seqNum;

    dwTargetSize = m_clientEngine->s_dwCompressedFrameSize;

    DPVF( DPVF_COMPRESSION_DEBUG_LEVEL, "COMPRESS: < %d --> %d ",m_clientEngine->s_dwUnCompressedFrameSize, dwTargetSize );

    memset(&dstMediaPacket,0,sizeof(XMEDIAPACKET));
    dstMediaPacket.dwMaxSize = dwTargetSize;
    dstMediaPacket.pdwCompletedSize = &dwTargetSize;
    dstMediaPacket.pvBuffer = pbBuilderLoc;

    hr = m_pConverter->Process(&pPacket->xmp,
                               &dstMediaPacket);

    ASSERT(dwTargetSize == m_clientEngine->s_dwCompressedFrameSize);

    if( FAILED( hr ) )
    {
        m_clientEngine->ReturnTransmitBuffer( pvSendContext );
        DPVF( DPVF_ERRORLEVEL, "Failed to perform conversion hr=0x%x", hr );
        DPVF_EXIT();
        return hr;
    }

    // We need to transmit header, target info and then speech data
    dwTransmitSize = sizeof( DVPROTOCOLMSG_SPEECHWITHTARGET ) + dwTargetInfoSize + dwTargetSize;

    pBufferDesc->dwBufferSize = dwTransmitSize;

    hr = m_clientEngine->m_lpSessionTransport->SendToServer( pBufferDesc, pvSendContext, 0 );

    m_seqNum++; 

    if( hr == DVERR_PENDING )
    {
        hr = DV_OK;
    }
    else if( FAILED( hr ) )
    {
        m_clientEngine->ReturnTransmitBuffer( pvSendContext );      
        DPVF( DPVF_INFOLEVEL, "Send failed hr=0x%x", hr );
    }

    DPVF_EXIT();
    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CClientRecordSubSystem::IsValidTarget"
BOOL CClientRecordSubSystem::IsValidTarget() 
{ 
    BOOL fValidTarget;
    KIRQL irql;
    
    RIRQL(irql);

    if( m_clientEngine->m_dwNumTargets > 0 )
    {
        fValidTarget = TRUE;
    }
    else
    {
        fValidTarget = FALSE;
    }

    LIRQL(irql);

    return fValidTarget;
}

