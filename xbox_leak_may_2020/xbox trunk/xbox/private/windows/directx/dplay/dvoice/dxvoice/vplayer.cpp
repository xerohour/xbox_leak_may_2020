/*==========================================================================
 *
 *  Copyright (C) 1999 - 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       vplayer.h
 *  Content:    Voice Player Entry
 *              
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  03/26/00    rodtoll Created
 * 09/01/2000  georgioc started rewrite for xbox 
 ***************************************************************************/

#include "vplayer.h"
#include "dvprot.h"
#include "in_core.h"
#include "sndutils.h"
#include "Dvengine.h"

//
// BUGBUG use dsound eventually
//

#define DV_USE_HAWK_HEADPHONE 1

CVoicePlayer::CVoicePlayer()
{

    Reset();
}

CVoicePlayer::~CVoicePlayer()
{
    if( IsInitialized() )
        DeInitialize();

}

void CVoicePlayer::Reset()
{
    m_dwFlags = 0;
    m_dvidPlayer = 0;
    m_lRefCount = 0;
    m_pInBoundAudioConverter = NULL;
    m_lpInputQueue = NULL;
    m_dwLastData = 0;
    m_dwHostOrderID = 0xFFFFFFFF;
    m_bLastPeak = 0;
    m_dwLastPlayback = 0;
    m_dwNumSilentFrames = 0;
    m_dwTransportFlags = 0;
    m_dwNumLostFrames = 0;
    m_dwNumSpeechFrames = 0;
    m_dwNumReceivedFrames = 0;
    m_pvPlayerContext = NULL;
    m_dwNumTargets = 0;
    m_pdvidTargets = NULL;
    m_pPlaybackMediaObject = NULL;
    m_pSoundTarget = NULL;
    m_pMediaPacketBuffer = NULL;

    InitializeListHead(&m_AvailableList);
    InitializeListHead(&m_PendingList);
    InitializeCriticalSection(&m_CS);

}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::SetPlayerTargets"
//
// Assumes the array has been checked for validity
// 
HRESULT CVoicePlayer::SetPlayerTargets( PDVID pdvidTargets, DWORD dwNumTargets )
{
    DV_AUTO_LOCK(&m_CS);

    delete [] m_pdvidTargets;

    if( dwNumTargets == 0 )
    {
        m_pdvidTargets = NULL;
    }
    else
    {
        m_pdvidTargets = new DVID[dwNumTargets];

        if( m_pdvidTargets == NULL )
        {
            m_pdvidTargets = NULL;
            m_dwNumTargets = 0;
            DPVF( DPVF_ERRORLEVEL, "Error allocating memory" );
            return DVERR_OUTOFMEMORY;
        }

        memcpy( m_pdvidTargets, pdvidTargets, sizeof(DVID)*dwNumTargets );
    }

    m_dwNumTargets = dwNumTargets;
    
    return DV_OK;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::Initialize"

HRESULT CVoicePlayer::Initialize(
    const DVID dvidPlayer,
    const DWORD dwHostOrder,
    const DWORD dwPortNumber,
    DWORD dwFlags,
    PVOID pvContext, 
    PLIST_ENTRY pOwner) 
{

    DV_AUTO_LOCK(&m_CS);

    m_lRefCount = 1;
    m_pOwner = pOwner;
    m_dvidPlayer = dvidPlayer;
    m_dwHostOrderID = dwHostOrder;
    m_dwLastData = GetTickCount();
    m_dwLastPlayback = 0;
    m_dwTransportFlags = dwFlags;
    m_pvPlayerContext = pvContext;

    ASSERT(dwPortNumber <= 255);
    m_bPortNumber = (UCHAR)dwPortNumber;

    m_dwFlags |= VOICEPLAYER_FLAGS_INITIALIZED;

    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::CreateSoundTarget"

HRESULT CVoicePlayer::CreateSoundTarget(CSoundTarget **ppSoundTarget)
{
    HRESULT hr;

    DPVF_ENTER();

    ASSERT(ppSoundTarget);

    DV_AUTO_LOCK(&m_CS);
    if (m_pSoundTarget) {
        return DVERR_ALREADYBUFFERED;
    }

    m_pSoundTarget = new CSoundTarget();

    hr = m_pSoundTarget->Initialize(m_dwUnCompressedFrameSize);
    if (FAILED(hr)) {

        return hr;
    }

    *ppSoundTarget = m_pSoundTarget;


    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::DeleteSoundTarget"

void CVoicePlayer::DeleteSoundTarget()
{
    HRESULT hr;

    DV_AUTO_LOCK(&m_CS);
    m_pSoundTarget = NULL;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::CreateQueue"

HRESULT CVoicePlayer::CreateQueue( PQUEUE_PARAMS pQueueParams )
{
    HRESULT hr;
    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();

    m_lpInputQueue = new CInputQueue2();
    
    if( m_lpInputQueue == NULL )
    {
        DPVF( 0, "Error allocating memory" );
        return DVERR_OUTOFMEMORY;
    }
    
    hr = m_lpInputQueue->Initialize( pQueueParams );

    if( FAILED( hr ) )
    {
        DPVF(0, "Failed initializing queue hr=0x%x", hr );
        delete m_lpInputQueue;
        m_lpInputQueue = NULL;

        return hr;
    }

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::CreateMediaObjects"

HRESULT CVoicePlayer::CreateMediaObjects(
    const GUID &guidCT,
    PWAVEFORMATEX pwfxTargetFormat,
    DWORD dwUncompressedFrameSize,
    XMediaObject *pConverter,
    BOOLEAN fUseMcpx
    )
{
    HRESULT hr;
    DPVF_ENTER();

    //
    // create decompressor
    //

    if (pConverter == NULL) {

        hr = DV_CreateConverter(guidCT,
                                &m_pInBoundAudioConverter,
                                pwfxTargetFormat,
                                NULL,
                                dwUncompressedFrameSize,
                                FALSE);
    
        if( FAILED( hr ) )
        {
            DPVF( 0, "Error creating audio converter hr=0x%x" , hr );
            return hr;
        }

    } else {

        m_pInBoundAudioConverter = pConverter;
        hr = NOERROR;
    }

    m_dwUnCompressedFrameSize = dwUncompressedFrameSize;

    //
    // instantiate our playback XMO. By default this is the voice peripheral's headphone
    //

    hr = SwitchInternalAudioTarget(pwfxTargetFormat,fUseMcpx, FALSE);
    return hr;
}

HRESULT CVoicePlayer::DeInitialize()
{
    KIRQL irql;
    FreeResources();

    RIRQL(irql);
    InsertHeadList(m_pOwner,&m_PoolEntry);
    LIRQL(irql);

    return DV_OK;
}


HRESULT CVoicePlayer::SwitchInternalAudioTarget(
    PWAVEFORMATEX pwfxTargetFormat,
    BOOL fUseMcpx,
    BOOL fMute
    )
{

    HRESULT hr = S_OK;
    LPDSSTREAMDESC pDsStreamDesc;

#ifdef SILVER

    DWORD deviceId;

#endif // SILVER

    XMEDIAINFO xmi;

    ULONG i;
    DVMEDIAPACKET *pBuffer;


    if( m_pPlaybackMediaObject != NULL )
    {

        ASSERT(m_pPlaybackMediaObject->Release() == 0);
        m_pPlaybackMediaObject = NULL;

        DV_FreeMediaPacketPool(&m_PendingList,TRUE);
        DV_FreeMediaPacketPool(&m_AvailableList,TRUE);

        if (m_pMediaPacketBuffer) {
    
            DV_POOL_FREE(m_pMediaPacketBuffer);
            m_pMediaPacketBuffer = NULL;
    
        }

    }

    if (fMute) {

        //
        // do not create any playback object
        // they dont want us to send any data for playback
        //


        return S_OK;

    }

    if (fUseMcpx) {

        //
        // switch from hawk to mcpx
        //


#ifdef SILVER

        deviceId = DSDEVID_MCPX;

#endif // SILVER

    } else {

#ifdef SILVER

        deviceId = DSDEVID_VOICE(m_bPortNumber);

#endif // SILVER

#if DV_USE_VOICE_HEADPHONE
        //
        // hawk output
        //
    
        hr = XVoiceCreateMediaObjectInternal(
            XDEVICE_TYPE_VOICE_HEADPHONE,
            m_bPortNumber,
            DV_CT_FRAMES_PER_PERIOD,
            pwfxTargetFormat,
            DV_VoicePlayerPlaybackPacketCompletion,
            this,
            &m_pPlaybackMediaObject);
    
        if (FAILED(hr)) {
            DPVF( DPVF_ERRORLEVEL, "Could not instatiate hawk XMO!" );
            DPVF_EXIT();
    
            return hr;
        }
#endif

    }

    pDsStreamDesc =(LPDSSTREAMDESC) DV_POOL_ALLOC(sizeof(DSSTREAMDESC));
    ASSERT(pDsStreamDesc);

    memset(pDsStreamDesc, 0, sizeof(DSSTREAMDESC));

#ifdef SILVER

    pDsStreamDesc->dwSize = sizeof(DSSTREAMDESC);

#endif // SIVER

    pDsStreamDesc->lpwfxFormat = pwfxTargetFormat;
    pDsStreamDesc->dwMaxAttachedPackets = DV_CT_FRAMES_PER_PERIOD;
    pDsStreamDesc->lpfnCallback = DV_VoicePlayerPlaybackPacketCompletion;
    pDsStreamDesc->lpvContext = this;

#ifdef SILVER

    hr = DirectSoundCreateStream(deviceId, pDsStreamDesc, (LPDIRECTSOUNDSTREAM *)&m_pPlaybackMediaObject, NULL);

#else // SILVER

    hr = DirectSoundCreateStream(pDsStreamDesc, (LPDIRECTSOUNDSTREAM *)&m_pPlaybackMediaObject);

#endif // SILVER

    if (FAILED(hr)) {

        DPVF(DPVF_ERRORLEVEL,"DirectSoundCreateStream for mcpx!!!");
        ASSERT(FALSE);
        return hr;

    }

    DV_POOL_FREE(pDsStreamDesc);


#if DBG

    //
    // get the minimum input requirements for the playback XMO
    //

    memset(&xmi,0,sizeof(xmi));

    xmi.dwInputSize = 1;
    hr = m_pPlaybackMediaObject->GetInfo(&xmi);

    ASSERT(!FAILED(hr));

    //
    // make sure decompressed frames are integer multiples
    //

    if (xmi.dwFlags & XMO_STREAMF_FIXED_PACKET_ALIGNMENT) {
        ASSERT((m_dwUnCompressedFrameSize%xmi.dwInputSize) == 0);
    }

#endif

    //
    // init our pool of media buffers
    //
    
    InitializeListHead(&m_AvailableList);    

    hr =DV_InitializeList(&m_AvailableList,
                          DV_CT_FRAMES_PER_PERIOD,
                          sizeof(DVMEDIAPACKET),
                          DVLAT_RAW);

    //
    // save the single pool allocation done for this list
    //

    m_pMediaPacketBuffer = m_AvailableList.Flink;


    ASSERT(SUCCEEDED(hr));
    hr = DV_InitMediaPacketPool(&m_AvailableList,m_dwUnCompressedFrameSize, TRUE);

    ASSERT(SUCCEEDED(hr));

    return hr;

}


#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::HandleReceive"

HRESULT CVoicePlayer::HandleReceive( PDVPROTOCOLMSG_SPEECHHEADER pdvSpeechHeader, PBYTE pbData, DWORD dwSize )
{
    CFrame tmpFrame;
    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();

    tmpFrame.SetSeqNum( pdvSpeechHeader->bSeqNum );
    tmpFrame.SetMsgNum( pdvSpeechHeader->bMsgNum );
    tmpFrame.SetIsSilence( FALSE );
    tmpFrame.SetFrameLength( (WORD)dwSize );
    tmpFrame.UserOwn_SetData( pbData, dwSize );

    DPVF( DPVF_CLIENT_SEQNUM_DEBUG_LEVEL, "SEQ: Receive: Msg [%d] Seq [%d]", pdvSpeechHeader->bMsgNum, pdvSpeechHeader->bSeqNum );       

    // STATSBLOCK: Begin
    //m_stats.m_dwPRESpeech++;
    // STATSBLOCK: End
        
    if (m_lpInputQueue->Enqueue( tmpFrame )) {

        m_dwLastData = GetTickCount();
        DPVF( DPVF_INFOLEVEL, "Received speech is buffered!" );
        m_dwNumReceivedFrames++;

    } else {

        DPVF( DPVF_ERRORLEVEL, "Received speech could not be queued!!!!" );

    }

    return DV_OK;
}

CFrame *CVoicePlayer::Dequeue()
{
    CFrame *frTmpFrame;     
    frTmpFrame = m_lpInputQueue->Dequeue(); 

    return frTmpFrame;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::DecompressInbound"

HRESULT CVoicePlayer::DeCompressInBound(PDVMEDIAPACKET pBuffer)
{
    HRESULT hr;
    CFrame *pCurrentFrame = (CFrame *)pBuffer->pvReserved;
    XMEDIAPACKET xmp;
    DWORD dwResultSize;

    DPVF( DPVF_INFOLEVEL, "Decompressing packet %x",pBuffer );

    //
    // prepare input buffer
    //

    xmp.dwMaxSize = pCurrentFrame->GetFrameLength();
    xmp.pdwStatus = NULL;
    xmp.pdwCompletedSize = NULL;
    xmp.pvBuffer = pCurrentFrame->GetDataPointer();

    if (pCurrentFrame->GetIsSilence()) {

        //
        // assume 16 bit pcm. pre init with silence
        //

        memset( pBuffer->xmp.pvBuffer,0x00, pBuffer->dwCompletedSize );

    }

    hr = m_pInBoundAudioConverter->Process(&xmp,
                                           &pBuffer->xmp);

    if( FAILED( hr ) )
    {
        DPVF( 0, "Failed converting audio hr=0x%x", hr );
        return hr;
    }

    ASSERT(pBuffer->dwCompletedSize == m_dwUnCompressedFrameSize);

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoicePlayer::GetNextFrameAndDecompress"

HRESULT CVoicePlayer::GetNextFrameAndDecompress(PDWORD pdwReturnFlags, DWORD *pdwSeqNum, DWORD *pdwMsgNum )
{    
    BYTE bLastPeak;
    HRESULT hr;
    PDVMEDIAPACKET pBuffer;

    DV_AUTO_LOCK(&m_CS);
    CFrame *frTmpFrame;

    //
    // dequeue frame
    //

    frTmpFrame = Dequeue();
   
    if( frTmpFrame->GetIsLost() )
    {
        *pdwReturnFlags |= DV_FRAMEF_IS_LOST;
        m_dwNumLostFrames++;

        frTmpFrame->Return();

        return DV_OK;
    }

    //
    // get media buffer
    //

    pBuffer = (PDVMEDIAPACKET)DV_REMOVE_TAIL(&m_AvailableList);
    if (!pBuffer) {

        frTmpFrame->Return();
        return DV_OK;

    }

    pBuffer->pvReserved = (PVOID)frTmpFrame;


    //
    // if we have received no frames, an attempt to dequeue will produce a
    // a silent frame. In that case we bail..
    //

    if( !frTmpFrame->GetIsSilence() )
    {
        m_dwLastPlayback = GetTickCount();
        m_dwNumSpeechFrames++;
        m_bLastPeak = FindPeak( (PBYTE) pBuffer->xmp.pvBuffer, m_dwUnCompressedFrameSize, FALSE );
    }
    else
    {
        m_dwNumSilentFrames++;
        *pdwReturnFlags |= DV_FRAMEF_IS_SILENCE;
        m_bLastPeak = 0;

        //
        // return frame and buffer and bail
        //

        DV_INSERT_TAIL(&m_AvailableList,pBuffer);
        frTmpFrame->Return();
        return DV_OK;

    }

    *pdwSeqNum = frTmpFrame->GetSeqNum();
    *pdwMsgNum = frTmpFrame->GetMsgNum();

    // If the frame was not silence, decompress it and then 
    // mix it into the mixer buffer
    DPVF(  DPVF_INFOLEVEL, "Player: 0x%x getting frame.. it's speech", GetPlayerID() );

    hr = DeCompressInBound(pBuffer);

    if( FAILED( hr ) )
    {
        frTmpFrame->Return();

        DV_INSERT_TAIL(&m_AvailableList,pBuffer);

        DPVF( 0, "Failed converting audio hr=0x%x", hr );
        return hr;
    }

    *pdwReturnFlags = 0;

    //
    // if the current player has a Sound Target, send it this data..
    //

    if( m_pSoundTarget !=NULL )
    {
        hr = m_pSoundTarget->MixInSingle((PUCHAR)pBuffer->xmp.pvBuffer);

        frTmpFrame->Return();
        DV_INSERT_TAIL(&m_AvailableList,pBuffer);

        return hr;

    } else {

        if (m_pPlaybackMediaObject) {

            //
            // add this media buffer on our pending pool
            //
    
            DV_INSERT_TAIL(&m_PendingList,pBuffer);
    
            //
            // If we didn't mix into any user created buffers, then
            // submit this packet to the dsound xmo
            //
                        
            hr = m_pPlaybackMediaObject->Process( 
                     &pBuffer->xmp, //pBuffer
                     NULL);
            
            if( FAILED( hr ) )
            {
                DPVF( DPVF_ERRORLEVEL, "Unable to ProcessIn frame." );
                return hr;
            }

        } else {

            //
            // no playback, return frame
            //

            frTmpFrame->Return();

        }

    }

    return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DV_PlaybackPacketCompletion"

VOID DV_VoicePlayerPlaybackPacketCompletion(PVOID pThis, PVOID pContext2, DWORD dwStatus)
{
    PDVMEDIAPACKET pPacket = (PDVMEDIAPACKET)pContext2;
    CVoicePlayer *This = (CVoicePlayer *) pThis;

    //
    // check previous buffer completions.
    //

    DPVF(DPVF_INFOLEVEL, "PlaybackPacketCOmpletion:Voice player(%x) playback packet (%x) done.",
        pThis,
        pContext2);

    //
    // return frame to frame queue
    //

    ((CFrame *)pPacket->pvReserved)->Return();

    //
    // remove context from pending pool
    //

    DV_REMOVE_ENTRY(pPacket);
    DV_INSERT_TAIL(&This->m_AvailableList,pPacket);

}

void CVoicePlayer::FreeResources()
{

    DV_AUTO_LOCK(&m_CS);

    if( m_lpInputQueue != NULL )
    {
        delete m_lpInputQueue;
        m_lpInputQueue = NULL;
    }

    if( m_pInBoundAudioConverter != NULL )
    {
        ASSERT(m_pInBoundAudioConverter->Release() == 0);
        m_pInBoundAudioConverter = NULL;
    }

    if( m_pPlaybackMediaObject != NULL )
    {
        ASSERT(m_pPlaybackMediaObject->Release() == 0);
        m_pPlaybackMediaObject = NULL;
    }

    if( m_pdvidTargets != NULL )
    {
        delete [] m_pdvidTargets;
        m_pdvidTargets = NULL;
    }

    if (m_pMediaPacketBuffer) {

        DV_FreeMediaPacketPool(&m_PendingList,TRUE);
        DV_FreeMediaPacketPool(&m_AvailableList,TRUE);

        DV_POOL_FREE(m_pMediaPacketBuffer);
        m_pMediaPacketBuffer = NULL;

    }

    Reset();
}
