//-----------------------------------------------------------------------------
// File: VoiceDSP.cpp
//
// Desc: A sample to show how to utilize dsound and the DSPs to do voice processing
// in realtime and apply DSP fx to incoming or outgoing voices.
// A dsp image containing Flange is loaded using DownloadEffectsImage
// so hawk microphone data can be routed through the FX to demonstrate real time
// voice morphing. The output of the FX is then routed in the dsp to the SRC effect
// which then outputs 8khz 32 bit data in system memory. We then stream that data
// to the hawk headphone. See FXIMG.INI for what we load to the DSP 
// Hist: 10.15.01
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define DSOUND_IFACE_VERSION DSOUND_IFACE_VERSION_LATEST

#include "voicedsp.h"


#if DBG

ULONG g_dwVoiceDSPDebugLevel = DEBUG_LEVEL_WARNING;

#endif

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------

XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle DSP Voice Effect" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Change Oscillator Frequency" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Select EQ parameter" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Change EQ parameter" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle DLS2" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle ParamEQ" }

};

#define NUM_HELP_CALLOUTS 7

// Struct definition for describing effect chains
typedef struct {
    WCHAR * szDescription;
    DWORD   dwMixBinMask;
    DWORD   dwNumInputs;
    DWORD   dwNumOutputs;
    DWORD   dwEffectIndex;
} EFFECT_DESCRIPTION;

typedef enum
{
    DLS2_CutoffFrequency,
    DLS2_Resonance,
    PARAMEQ_Frequency,
    PARAMEQ_Gain,
    PARAMEQ_Q,
    NUM_OPTIONS,
} FILTER_OPTION;

#define RES_SCALE 1000
#define PEQ_GAIN_SCALE 400


#define MIXBIN_FXSEND_5  (1 << DSMIXBIN_FXSEND_5)
#define MIXBIN_FXSEND_2  (1 << DSMIXBIN_FXSEND_2)
#define MIXBIN_FXSEND_3  (1 << DSMIXBIN_FXSEND_3)
#define MIXBIN_CENTER (1 << DSMIXBIN_FRONT_CENTER)


// List of all effect chains, mixbins, and parameters
EFFECT_DESCRIPTION g_aEffectDescriptions[] =
{
    { L"None",          MIXBIN_CENTER | MIXBIN_FXSEND_5, 0, 0, -1},
    { L"Flange",        MIXBIN_FXSEND_2, 3, 2, AMPMOD_CHAIN_AMPMOD},
    { L"AmpMod",        MIXBIN_FXSEND_3, 3, 2, FLANGE_CHAIN_FLANGE},
};

typedef struct
{
    DWORD dwScale;          // Joystick scale factor
    WCHAR * szDescription;  // Description of option
} OPTION_STRUCT;

OPTION_STRUCT g_aOptions[] =
{
    { 5000,  L"DLS2 Filter Freq" },
    { 10000, L"DLS2 Resonance" },
    { 5000, L"ParamEQ Filter Freq" },
    { 10000, L"ParamEQ Filter Gain" },
    { 10000, L"ParamEQ Filter Q" },
};

//
// Simple macro to verify that the packet size is OK for a particular XMO
//
inline HRESULT XMOFILTER_VERIFY_PACKET_SIZE( XMEDIAINFO& xMediaInfo, DWORD dwPacketSize )
{
    if( xMediaInfo.dwOutputSize > dwPacketSize )
        return E_FAIL;
    if( xMediaInfo.dwOutputSize ) 
        if( dwPacketSize % xMediaInfo.dwOutputSize != 0 )
            return E_FAIL;
    if( dwPacketSize < xMediaInfo.dwMaxLookahead )
        return E_FAIL;

    return S_OK;
}

CFilterGraph::CFilterGraph()
{

    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    //
    // init some locals
    //

    m_pDSBufferOscillator = NULL;
    m_lOscillatorFrequency = 100;
    m_dwPortNumber = -1;
    m_dwCount= 0;
    m_dwOldPosition = 0xFFFFFFFF;
    m_pDSound = NULL;

    m_dwCurrentEffect = 0;

    m_dwParam = 0;
    ZeroMemory( &m_dsfd, sizeof( DSFILTERDESC ) );

    m_lParamEQFreq = 0;
    m_lParamEQGain = 0;
    m_lParamEQQ = 0;

    m_fDLS2Freq = 0;
    m_lDLS2Resonance = 0;

}

CFilterGraph::~CFilterGraph()
{

    if (m_pDSBufferOscillator) {
        m_pDSBufferOscillator->Release();
    }

    if (m_pDSound) {
        m_pDSound->Release();
    }

}


//-----------------------------------------------------------------------------
// Name: Initialize
//
// Desc: Creates a streaming graph between 2 XMOs: a source, and a target. 
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::Initialize()
{
    HRESULT        hr;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    //
    // download the standard DirectSound effecs image
    //
    if( FAILED( DownloadEffectsImage("d:\\media\\fximg.bin") ) )
        return E_FAIL;

    m_bDrawHelp = FALSE;
    m_bPaused = FALSE;

    InitializeStreamingObjects();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FindHeadsetPort()
// Desc: Returns the controller port of the first communicator device found
//-----------------------------------------------------------------------------
int CFilterGraph::FindHeadsetPort()
{
    DWORD dwConnectedMicrophones = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
    DWORD dwConnectedHeadphones = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);

    if(dwConnectedMicrophones != dwConnectedHeadphones)
    {
        DebugPrint("CXBoxSample::Voice()...dwConnectedMicrophones != dwConnectedHeadphone\n");
        return -1;
    }

    if(!dwConnectedMicrophones || !dwConnectedHeadphones)
    {
        return -1;
    }

    // Go through each port and see if there's a headset on it
    for(int i = 0; i < XGetPortCount(); ++i)
    {
        if(dwConnectedMicrophones & (1 << i))
        {
            return i;
        }
    }

    return -1;
}

HRESULT CFilterGraph::InitializeStreamingObjects()
{

    HRESULT hr = S_OK;
    DWORD          dwMinSize;
    XMEDIAINFO     xMediaInfo;
    DSSTREAMDESC   dssd;
    DSMIXBINS               dsMixBins;
    DSMIXBINVOLUMEPAIR      dsMixBinArray[8];

    memset(dsMixBinArray,0,sizeof(dsMixBinArray));

    //
    // enumerate a hawk device
    //    

    if (m_dwPortNumber == -1) {
        m_dwPortNumber = FindHeadsetPort();
    }
        
    if (m_dwPortNumber == -1) {
        return S_OK;
    }

    m_dwPacketSize     = PACKET_SIZE;
    m_dwMaxBufferCount = PACKET_CNT;

    m_wfxAudioFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_wfxAudioFormat.nChannels = 1;
    m_wfxAudioFormat.nSamplesPerSec = VOICE_SAMPLING_RATE;
    m_wfxAudioFormat.nAvgBytesPerSec = 2 * VOICE_SAMPLING_RATE;
    m_wfxAudioFormat.nBlockAlign = 2;
    m_wfxAudioFormat.wBitsPerSample = 16;
    m_wfxAudioFormat.cbSize = 0;


    DBG_WARN(("Xbox Communicator found in port %d!\n", m_dwPortNumber));

    //
    //  Create the source xmo (microphone)
    //

    hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, m_dwPortNumber , XVOICE_TOTAL_REGIONS,
                                 &m_wfxAudioFormat, &m_pSourceXMO);

    if( FAILED( hr ) )
    {
        DBG_ERROR(("Failed to open hawk mic, hr %x", hr));
        return hr;
    }

    //
    // create the in-between transform XMO (Dsound)
    //

    ZeroMemory( &dssd, sizeof(dssd) );
    dssd.dwFlags              = 0;
    dssd.dwMaxAttachedPackets = PACKET_CNT;
    dssd.lpMixBins            = &dsMixBins;
    dssd.lpwfxFormat          = &m_wfxAudioFormat;

    dsMixBins.dwMixBinCount = 3;
    dsMixBins.lpMixBinVolumePairs = dsMixBinArray;

    dsMixBinArray[0].dwMixBin = DSMIXBIN_FXSEND_5;
    dsMixBinArray[1].dwMixBin = DSMIXBIN_FRONT_LEFT;
    dsMixBinArray[2].dwMixBin = DSMIXBIN_FRONT_RIGHT;

    hr = DirectSoundCreateStream( &dssd, 
                                  (LPDIRECTSOUNDSTREAM*)&m_pTransformXMO );
    if( FAILED(hr) ) 
    {          
        DBG_ERROR( ("XmoFilter: DirectSoundCreateXMOStream failed with %x", hr) );
        DBG_ERROR( ("Init: Could not open a target audio XMO\n") );
        return hr;
    }

    //
    // create the target XMO (hawk)
    //

    hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, m_dwPortNumber , XVOICE_TOTAL_REGIONS,
                                 &m_wfxAudioFormat, &m_pTargetXMO);

    if( FAILED( hr ) )
    {
        DBG_ERROR(("Failed to open hawk headphone, hr %x", hr));
        return hr;
    }


    //
    // At this point we should have all the XMOs.
    // Figure out the minimum packet size we need to pass media buffers
    // in between XMOs. All we do is verifying that our size (PACKET_SIZE)
    // is both larger than the min of all XMOs and its length aligned.
    // As a last check we make sure its also larger than the maxLookahead
    //
    // At the end of all this, m_dwMinPacketSize is the minimum packet size
    // for this filter graph.
    //
    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    if( m_pSourceXMO )
    {
        m_pSourceXMO->GetInfo( &xMediaInfo );
        m_dwMinPacketSize = xMediaInfo.dwOutputSize;

        XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
    }

    if( m_pTransformXMO )
    {
        m_pTransformXMO->GetInfo( &xMediaInfo );
        m_dwMinPacketSize = max( xMediaInfo.dwOutputSize, m_dwMinPacketSize );

        XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
    }

    if( m_pTargetXMO )
    {
        m_pTargetXMO->GetInfo( &xMediaInfo );
        m_dwMinPacketSize = max( xMediaInfo.dwOutputSize, m_dwMinPacketSize );

        XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
    }

    ASSERT( m_pTargetXMO );
    ASSERT( m_pSourceXMO );
    ASSERT( m_pTransformXMO );

    //
    // make sure SRC effect delay line is aligned with our packet size
    //

    ASSERT(m_pEffectsImageDesc->aEffectMaps[SRC_CHAIN_SRC].dwScratchSize%m_dwPacketSize == 0);

    // Initialize the linked lists
    m_SourcePendingList.Initialize();
    m_TransformPendingList.Initialize();
    m_TargetPendingList.Initialize();

    //
    // Allocate one large buffer for incoming/outgoing data. The buffer is
    // going to be divided among our media context packets, which will be 
    // used to stream data between XMOs
    //
    dwMinSize = MAXBUFSIZE;
    m_pTransferBuffer = (BYTE *)XPhysicalAlloc( dwMinSize,
                                                MAXULONG_PTR,
                                                0,
                                                PAGE_READWRITE | PAGE_NOCACHE );

    ZeroMemory( m_pTransferBuffer, dwMinSize );

    //
    // Allocate a context pool. A context is associated with a packet buffer
    // and used to track the pending packets attached to the source or target
    // XMO.
    dwMinSize = ( PACKET_CNT * sizeof(MEDIA_PACKET_CTX) );
    m_pPacketContextPool = new BYTE[dwMinSize];
    ZeroMemory( m_pPacketContextPool, dwMinSize );

    //
    // We attach all available packet buffers in the beginning to prime the
    // graph: As each packet is completed by the source XMO, the status is
    // udpated in its packet context and we pass the packet to the next XMO.
    //
    for( DWORD i = 0; i < PACKET_CNT; i++ ) 
    {
        MEDIA_PACKET_CTX* pCtx = &((MEDIA_PACKET_CTX*)m_pPacketContextPool)[i];

        ZeroMemory( pCtx, sizeof(MEDIA_PACKET_CTX) );
        pCtx->dwCompletedSize = m_dwPacketSize;
        pCtx->pBuffer         = &m_pTransferBuffer[i*PACKET_SIZE];

        AttachPacketToSourceXMO( pCtx );
    }

    hr = CreateSineWaveBuffer(m_lOscillatorFrequency,&m_pDSBufferOscillator);

    return hr;


}


//-----------------------------------------------------------------------------
// Name: AttachPacketToSourceXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       source XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // reset packet size
    pCtx->dwCompletedSize = m_dwPacketSize;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Attach to the source XMO (first xmo in the chain)
    hr = m_pSourceXMO->Process( NULL, &xmb );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }

    // Add this context to our source list
    m_SourcePendingList.Add( &pCtx->ListEntry );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: AttachPacketToTransformXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       transform (in-between) XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToTransformXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer         
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Attach to the target XMO (first xmo in the chain)
    hr = m_pTransformXMO->Process( &xmb, NULL );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }

    // Add this context to our target list
    m_TransformPendingList.Add( &pCtx->ListEntry );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: AttachPacketToTargetXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       target XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    //
    // figure out where we are in the circular buffer of the SRC FX.
    // Then stream the appropriate number of samples to the target XMO
    // 

    // retrieve effect parameters so we can getthe current buffer index from the DSP
    // delay line

    LPCDSFX_SAMPLE_RATE_CONVERTER_PARAMS pSrcParams = (LPCDSFX_SAMPLE_RATE_CONVERTER_PARAMS)m_pEffectsImageDesc->aEffectMaps[SRC_CHAIN_SRC].lpvStateSegment; 

    //
    // figure out where to read
    //

    DWORD dwCircularBufferSize = m_pEffectsImageDesc->aEffectMaps[SRC_CHAIN_SRC].dwScratchSize;
    DWORD dwSamplesToStream = m_dwPacketSize/sizeof(WORD);

    //
    // the circular buffer index increments in 4 byte increments since each sample is
    // 32 bit. Our hawk packet needs 16 bit samples
    //

    DWORD dwSrcPacketSize = m_dwPacketSize*(sizeof(DWORD)/sizeof(WORD));
    DWORD dwCurrentPosition = pSrcParams->dwScratchSampleOffset;

    PDWORD pSrcData;
    PWORD pData = (PWORD)pCtx->pBuffer;
    BOOL bReSync = FALSE;

    if (m_dwOldPosition == -1) {

        //
        // this is the first time through the loop, just record the position and bail
        // attach to source XMO so packet is not lost
        //

        m_dwOldPosition = dwCurrentPosition - (dwCurrentPosition%(dwSrcPacketSize));
        AttachPacketToSourceXMO( pCtx );
        return S_OK;
    }

    //
    // do the circular buffer -> packet streaming math
    // if the current postion has moved more than 3 packet sizes than the old position
    // re-synchronize
    //

    if (m_dwOldPosition > dwCurrentPosition) {

        if ((dwCurrentPosition + (dwCircularBufferSize-m_dwOldPosition)) > dwSrcPacketSize*3) {

            bReSync = TRUE;

        }

        if ((dwCurrentPosition + (dwCircularBufferSize-m_dwOldPosition)) < dwSrcPacketSize) {
               
            //
            // if we poll too fast the SRC effect circular buffer index might have not
            // progressed far enough for us to submit a full packet.
            //

            DBG_WARN(("positions too close Old pos %x, current %x\n",m_dwOldPosition,dwCurrentPosition));
            AttachPacketToSourceXMO( pCtx );
            return S_OK;

        }

    } else {

        if ((dwCurrentPosition - m_dwOldPosition) > dwSrcPacketSize*3) {

            bReSync = TRUE;

        }

        if ((dwCurrentPosition - m_dwOldPosition) < dwSrcPacketSize) {

            DBG_WARN(("positions too close Old pos %x, current %x\n",m_dwOldPosition,dwCurrentPosition));
            AttachPacketToSourceXMO( pCtx );
            Sleep(5);
            return S_OK;
        }

    }

    if (bReSync) {

        DBG_WARN(("Drifted current %x, old %x\n",dwCurrentPosition,m_dwOldPosition));
        m_dwOldPosition = dwCurrentPosition - (dwCurrentPosition%(dwSrcPacketSize));
        DBG_WARN(("New Old pos %x\n",m_dwOldPosition));

    }

    //
    // create VA of base offset into the delay line
    // we always align the the current reading position to the packet size
    // this way we dont need to worry about wrapping with a partial packet 
    // at the end of the delay line
    //

    pSrcData = (PDWORD)((PUCHAR)m_pEffectsImageDesc->aEffectMaps[SRC_CHAIN_SRC].lpvScratchSegment+m_dwOldPosition);            

    //
    // convert 32 bit container to 16 bit container
    //

    for (DWORD i=0;i<dwSamplesToStream;i++) {

        *pData++ = WORD(*pSrcData++ >> 16);

    }

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer         
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Attach to the target XMO (first xmo in the chain)
    hr = m_pTargetXMO->Process( &xmb, NULL );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }

    // Add this context to our target list
    m_TargetPendingList.Add( &pCtx->ListEntry );

    //
    // increment index into SRC effect delay line
    //

    m_dwOldPosition = (m_dwOldPosition+dwSrcPacketSize)%dwCircularBufferSize;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DownloadEffectsImage
// Desc: Downloads an effects image to the DSP
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::DownloadEffectsImage(PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer = NULL;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC EffectLoc;

    // open scratch image file generated by xps2 tool
    hFile = CreateFile( pszScratchFile,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL );

    if( hFile == INVALID_HANDLE_VALUE )
    {
        DWORD err;

        err = GetLastError();
        OUTPUT_DEBUG_STRING( "Failed to open the dsp image file.\n" );
        hr = HRESULT_FROM_WIN32(err);
    }

    if( SUCCEEDED(hr) )
    {
        // Determine the size of the scratch image by seeking to
        // the end of the file
        dwSize = SetFilePointer( hFile, 0, NULL, FILE_END );
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    
        // Allocate memory to read the scratch image from disk
        pBuffer = new BYTE[dwSize];

        // Read the image in
        DWORD dwBytesRead;
        BOOL bResult = ReadFile( hFile,
                                 pBuffer,
                                 dwSize,
                                 &dwBytesRead,
                                 0 );
        
        if (!bResult)
        {
            DWORD err;

            err = GetLastError();
            OUTPUT_DEBUG_STRING( "\n Failed to open the dsp image file.\n" );
            hr = HRESULT_FROM_WIN32(err);
        }

    }

    if( SUCCEEDED(hr) )
    {
        // call dsound api to download the image..
        EffectLoc.dwI3DL2ReverbIndex = DSFX_IMAGELOC_UNUSED;
        EffectLoc.dwCrosstalkIndex = DSFX_IMAGELOC_UNUSED;

        hr = m_pDSound->DownloadEffectsImage( pBuffer,
                                              dwSize,
                                              &EffectLoc,
                                              &m_pEffectsImageDesc );
    }

    delete[] pBuffer;

    if( hFile != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hFile );
    }

    return hr;
}





//-----------------------------------------------------------------------------
// Name: Cleanup
// Desc: Clean up resources
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::Cleanup()
{
    // Cleanup: delete any XMO's
    if( m_pSourceXMO )       
        m_pSourceXMO->Release();
    if( m_pTargetXMO )       
        m_pTargetXMO->Release();
    if( m_pDSound )
        m_pDSound->Release();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: dBToHardwareCoeff
// Desc: Calculates coefficient value[1] for DLS2 filter from resonance in dB
//-----------------------------------------------------------------------------
ULONG dBToHardwareCoeff( LONG lResonance )
{
    FLOAT fResonance = (FLOAT)lResonance;

    if( fResonance > 22.5f )
        fResonance = 22.5f;

    double fQ = pow( 10.0, -0.05*fResonance);
    unsigned int dwQ = (unsigned int)(fQ*(1<<15));
    if( dwQ > 0xFFFF )
        dwQ = 0xFFFF;

    return (ULONG)dwQ;
}

///
/// Set up necessary types
/// 

typedef short* LPSHORT;

///
/// Setup the frequency, and define PI
///

static const double PI = 3.1415926535;


HRESULT
CreateSineWaveBuffer( double dFrequency, LPDIRECTSOUNDBUFFER8 * ppBuffer )
{
    HRESULT hr = S_OK;
    LPDIRECTSOUNDBUFFER8 pBuffer = NULL;
    WORD * pData = NULL;
    DWORD dwBytes = 0;
    double dArg = 0.0;
    double dSinVal = 0.0;
    WORD wVal = 0;
    DSBUFFERDESC dsbd;
    WAVEFORMATEX wfx;

    DSMIXBINS               dsMixBins;
    DSMIXBINVOLUMEPAIR      dsMixBinArray[8];

    memset(dsMixBinArray,0,sizeof(dsMixBinArray));

    //
    // Initialize a wave format structure
    //

    ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

    wfx.wFormatTag      = WAVE_FORMAT_PCM;      // PCM data
    wfx.nChannels       = 1;                    // Mono
    wfx.nSamplesPerSec  = 48000;                 // 48kHz
    wfx.nAvgBytesPerSec = 96000;                 // 48kHz * 2 bytes / sample
    wfx.nBlockAlign     = 2;                    // sample size in bytes
    wfx.wBitsPerSample  = 16;                   // 16 bit samples
    wfx.cbSize          = 0;                    // No extra data

    //
    // Intialize the buffer description
    ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );

    dsbd.dwSize = sizeof( DSBUFFERDESC );
    dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
    dsbd.lpwfxFormat = &wfx;
    dsbd.lpMixBins = &dsMixBins;

    //
    // oscillator output goes to FXSEND 4
    //

    dsMixBins.dwMixBinCount = 1;
    dsMixBins.lpMixBinVolumePairs = dsMixBinArray;
    dsMixBinArray[0].dwMixBin = DSMIXBIN_FXSEND_4;

    if (*ppBuffer == NULL) {

        // Check arguments
        if( !ppBuffer || dFrequency < 0 )
            return E_INVALIDARG;
    
        *ppBuffer = NULL;
    
        // Create the buffer
        hr = DirectSoundCreateBuffer( &dsbd, &pBuffer );
        if( FAILED( hr ) )
            return hr;

    } else {

        pBuffer = *ppBuffer;

    }

    // Get a pointer to buffer data to fill
    hr = pBuffer->Lock( 0, dsbd.dwBufferBytes, (VOID **)&pData, &dwBytes, NULL, NULL, 0 );
    if( FAILED( hr ) )
        return hr;

    // Now fill the buffer, 1 16-bit sample at a time
    for( DWORD i = 0; i < dwBytes/sizeof(WORD); i++ )
    {
        // Convert sample offset to radians
        dArg = (double)i / wfx.nSamplesPerSec * D3DX_PI * 2;

        // Calculate the sin
        dSinVal = sin( dFrequency * dArg );

        // Scale to sample format
        wVal = WORD( dSinVal * 32767 );

        // Store the sample
        pData[i] = wVal;
    }

    // Start the sine wave looping
    hr = pBuffer->Play( 0, 0, DSBPLAY_LOOPING );
    if( FAILED( hr ) )
        return hr;

    // return the buffer
    *ppBuffer = pBuffer;

    return hr;
}

//-----------------------------------------------------------------------------
// Name: FreqToHardwareCoeff
// Desc: Calculates coefficient value[0] for DLS2 filter from a frequency
//-----------------------------------------------------------------------------
ULONG FreqToHardwareCoeff( FLOAT fFreq )
{
    FLOAT fNormCutoff = fFreq / 48000.0f;

    // Filter is ineffective out of these ranges, so why
    // bother even trying?
    if( fFreq < 30.0f )
        return 0x8000;
    if( fFreq > 8000.0f )
        return 0x0;

    FLOAT fFC = FLOAT( 2.0f * sin( D3DX_PI * fNormCutoff ) );
    LONG lOctaves = LONG( 4096.0f * log( fFC ) / log( 2.0f ) );

    return (ULONG)lOctaves & 0xFFFF;
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates, such as checking async xmo status, and
//       updating source file position for our progress bar
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::FrameMove()
{
    HRESULT hr = S_OK;
    LPDIRECTSOUNDSTREAM pDSStream = (LPDIRECTSOUNDSTREAM)m_pTransformXMO;

    //
    // check if hawk is plugged in
    //

    if (m_dwPortNumber == -1) {

        hr = InitializeStreamingObjects();

    } else {

        if (SUCCEEDED(hr)) {
    
            LONG lDelta;

            if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
            {
                if( m_dwParam == 0 )
                    m_dwParam = NUM_OPTIONS - 1;
                else
                    m_dwParam = ( m_dwParam - 1 );
            }
    
            if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
            {
                m_dwParam = ( m_dwParam + 1 ) % NUM_OPTIONS;
            }
    
            //
            // Change frequency of oscillator driving the voice FX
            //

            lDelta = LONG(m_DefaultGamepad.fX1 * m_fElapsedTime * FREQ_STEP);
          
            if (abs(lDelta) > 0) {

                if (m_lOscillatorFrequency < 500) {
    
                    lDelta /= 10;

                }
    

                m_lOscillatorFrequency = m_lOscillatorFrequency + lDelta;

                if (m_lOscillatorFrequency < 0) {
                    m_lOscillatorFrequency = MAX_OSCILLATOR_FREQUENCY;
                }

                if (m_lOscillatorFrequency > MAX_OSCILLATOR_FREQUENCY) {
                    m_lOscillatorFrequency = 0;
                }
              
                hr = CreateSineWaveBuffer(m_lOscillatorFrequency,&m_pDSBufferOscillator);

            }
        
            // Toggle effect
            if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
            {
                m_dwCurrentEffect = ( m_dwCurrentEffect + 1 ) % ( sizeof( g_aEffectDescriptions ) / sizeof( g_aEffectDescriptions[0] ) );
        
                //
                // change mixbin assignment
                //
        
                DSMIXBINS               dsMixBins;
                DSMIXBINVOLUMEPAIR      dsMixBinArray[8];
            
                dsMixBins.dwMixBinCount = 0;
                dsMixBins.lpMixBinVolumePairs = dsMixBinArray;
                memset(dsMixBinArray,0,sizeof(dsMixBinArray));
        
                for (DWORD i=0;i<32;i++) {
        
                    if ((1 << i) & g_aEffectDescriptions[m_dwCurrentEffect].dwMixBinMask) {
        
                        dsMixBinArray[dsMixBins.dwMixBinCount++].dwMixBin = i;
        
                    }
        
                }
        
                hr = pDSStream->SetMixBins(&dsMixBins);
        
            }
    
        }
    
        // Instruct routine to poll the filter graph so completed packets can be
        // moved through the streaming chain.
    
        if (SUCCEEDED(hr) && (m_dwPortNumber != -1)) {
            hr = TransferData();
        }
            
        BOOLEAN bSetFilter = FALSE;

        if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
        {
            m_dsfd.dwMode ^= DSFILTER_MODE_DLS2;
            bSetFilter = TRUE;
        }
    
        if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
        {
            m_dsfd.dwMode ^= DSFILTER_MODE_PARAMEQ;
            bSetFilter = TRUE;
        }

        if (SUCCEEDED(hr)) {
    
            // Adjust value of currently selected parameter
            LONG lDelta = LONG(m_DefaultGamepad.fX2 * m_fElapsedTime * g_aOptions[ m_dwParam ].dwScale );
            if ((abs(lDelta) > 0)  || bSetFilter)  {

                switch( m_dwParam )
                {
                case DLS2_CutoffFrequency:
                    m_fDLS2Freq += lDelta;
                    if( m_fDLS2Freq < 0.0f )
                        m_fDLS2Freq = 0.0f;
                    else if( m_fDLS2Freq > 8000 )
                        m_fDLS2Freq = 8000;
                    break;
                case DLS2_Resonance:
                    m_lDLS2Resonance += lDelta;
                    if( m_lDLS2Resonance < 0 )
                        m_lDLS2Resonance = 0;
                    else if( m_lDLS2Resonance > 22 * RES_SCALE )
                        m_lDLS2Resonance = 22 * RES_SCALE;
                    break;

                case PARAMEQ_Frequency:
                    m_lParamEQFreq += lDelta;
                    if( m_lParamEQFreq < 0 )
                        m_lParamEQFreq = 0;
                    else if( m_lParamEQFreq > 8000 )
                        m_lParamEQFreq = 8000;
                    break;
                case PARAMEQ_Gain:
                    m_lParamEQGain += lDelta;
                    if( m_lParamEQGain < -8192 )
                        m_lParamEQGain = -8192;
                    else if( m_lParamEQGain > 8191 )
                        m_lParamEQGain = 8191;
                    break;
                case PARAMEQ_Q:
                    m_lParamEQQ += lDelta;
                    if( m_lParamEQQ < 0 )
                        m_lParamEQQ = 0;
                    else if( m_lParamEQQ / RES_SCALE > 7 )
                        m_lParamEQQ = 7 * RES_SCALE;
                    break;
                }
            
                // 
                // Note!!!:
                // For stereo buffers, where you can only use DLS2 OR ParamEQ, you must 
                // set both pairs of coefficients (one for each channel).
                //
                m_dsfd.adwCoefficients[0] = FreqToHardwareCoeff( m_fDLS2Freq );
                m_dsfd.adwCoefficients[1] = dBToHardwareCoeff( m_lDLS2Resonance / RES_SCALE );
                m_dsfd.adwCoefficients[2] = FreqToHardwareCoeff( FLOAT(m_lParamEQFreq) );
                m_dsfd.adwCoefficients[3] = m_lParamEQGain & 0xFFFF;
                m_dsfd.dwQCoefficient = m_lParamEQQ / RES_SCALE;
            
                pDSStream->SetFilter( &m_dsfd );

            }
    
        }


    }

    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }


    return hr;
}


//-----------------------------------------------------------------------------
// Name: TransferData
// Desc: If we have a packet completed from our source XMO, pass it through
//       the intermediate XMO and on to the target.
//       If we have a packet completed from our target XMO, recycle it back
//       to the source XMO.
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::TransferData()
{
    HRESULT        hr=S_OK;
    DWORD          index = 0;
    XMEDIAPACKET   xmb;
    MEDIA_PACKET_CTX* pSrcCtx = NULL;
    MEDIA_PACKET_CTX* pInBetweenCtx = NULL;
    MEDIA_PACKET_CTX* pDstCtx = NULL;
    DWORD             dwSize  = 0;

    // Setup media buffer
    ZeroMemory( &xmb, sizeof(xmb) );

    //
    // Always deal with the oldest packet first.  Oldest packets are at the 
    // head, since we insert at the tail, and remove from head
    //
    if( !m_SourcePendingList.IsListEmpty() ) 
    {
        pSrcCtx = (MEDIA_PACKET_CTX*)m_SourcePendingList.pNext; // Head
    }

    if( !m_TransformPendingList.IsListEmpty() ) 
    {
        pInBetweenCtx = (MEDIA_PACKET_CTX*)m_TransformPendingList.pNext; // Head
    }        

    if( !m_TargetPendingList.IsListEmpty() ) 
    {
        pDstCtx = (MEDIA_PACKET_CTX*)m_TargetPendingList.pNext; // Head
    }        

    // Do we have a completed source packet?
    if( pSrcCtx && pSrcCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING ) 
    {
        // A src buffer completed.
        // Remove this context from the Src pending list.
        // Send it to a codec, if present, then send it to the Target xmo
        pSrcCtx->ListEntry.Remove();

        DBG_SPAM(("Context %x received %x bytes\n",pSrcCtx, pSrcCtx->dwCompletedSize));
        ASSERT( pSrcCtx->dwCompletedSize);

        // Pass media buffer to the transform xmo.
        AttachPacketToTransformXMO( pSrcCtx );
    }

    // Do we have a completed transform packet?
    if( pInBetweenCtx && pInBetweenCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING ) 
    {
        // A in-between buffer completed.
        // Remove this context from the transform pending list.
        pInBetweenCtx->ListEntry.Remove();

        DBG_SPAM(("InBetween Context %x received %x bytes\n",pInBetweenCtx, pInBetweenCtx->dwCompletedSize));
        ASSERT( pInBetweenCtx->dwCompletedSize);

        // Pass media buffer to the target xmo.
        AttachPacketToTargetXMO( pInBetweenCtx );

    }


    // Check if we have a completed packet from the target
    if( pDstCtx && pDstCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING )
    {       
        DBG_SPAM(("Target XMO completed context %x\n",pDstCtx));

        // Remove the entry from the list
        pDstCtx->ListEntry.Remove();

        ASSERT(pDstCtx->dwCompletedSize != 0);

        AttachPacketToSourceXMO( pDstCtx );
    } 

    return S_OK;
}

static const float PEAK_MONITOR_LEFT_X = 350.;
static const float PEAK_MONITOR_BOTTOM_Y = 350.;

static const float PEAK_MONITOR_X_SIZE = 100.;
static const float PEAK_MONITOR_Y_SIZE = 250.;

static const float PEAK_MONITOR_BAR_WIDTH = 100.;
static const float PEAK_MONITOR_BAR_SPACING = 30.;

static const DWORD NUM_MIXBINS=1;

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene - just a progress bar denoting how much of the 
//       source file has been read.
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::Render()
{

    WCHAR szString[256];
    DWORD dwValue=0,i;
    FLOAT fXPos;
    FLOAT fYPos;
    
    FLOAT x1, x2;
    FLOAT y1, y2;
    FLOAT fPeakLevels[NUM_MIXBINS];

    FLOAT fValue = 0.;

    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    DirectSoundDoWork();

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Show title, frame rate, and help
    if( m_bDrawHelp ) {
    
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );

    } else {

        //
        // get the output levels from dsound
        //
    
        DSOUTPUTLEVELS dsOutputLevels;
        memset(&dsOutputLevels, 0 ,sizeof(dsOutputLevels));
    
        m_pDSound->GetOutputLevels(&dsOutputLevels, FALSE);
    
        //
        // digital peak levels
        //
    
        fPeakLevels[DSMIXBIN_FRONT_LEFT] = (FLOAT) dsOutputLevels.dwDigitalFrontLeftPeak;
        fPeakLevels[DSMIXBIN_FRONT_RIGHT] = (FLOAT) dsOutputLevels.dwDigitalFrontRightPeak;
        fPeakLevels[DSMIXBIN_BACK_LEFT] = (FLOAT) dsOutputLevels.dwDigitalBackLeftPeak;
        fPeakLevels[DSMIXBIN_BACK_RIGHT] = (FLOAT) dsOutputLevels.dwDigitalBackRightPeak;
        fPeakLevels[DSMIXBIN_FRONT_CENTER] = (FLOAT) dsOutputLevels.dwDigitalFrontCenterPeak;
        fPeakLevels[DSMIXBIN_LOW_FREQUENCY] = (FLOAT) dsOutputLevels.dwDigitalLowFrequencyPeak;

        fYPos = PEAK_MONITOR_BOTTOM_Y+20 ;
    
        for (i=0;i<NUM_MIXBINS;i++) {
    
            fValue = (FLOAT)(fPeakLevels[i]/pow(2,23));
            fValue = (FLOAT)(20.0*log10(fValue));
        
            fXPos = PEAK_MONITOR_LEFT_X+PEAK_MONITOR_BAR_SPACING+i*(PEAK_MONITOR_BAR_SPACING+PEAK_MONITOR_BAR_WIDTH);
            swprintf (szString,L"%2.2f",fValue);
            m_Font.DrawText( fXPos, fYPos, 0xFFFFFF00, szString);
     
    
            m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    
            // Draw left level
            {
                struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
                BACKGROUNDVERTEX v[4];
    
                fValue = (FLOAT)(fPeakLevels[i]/pow(2,23));
    
                x1 = PEAK_MONITOR_LEFT_X + PEAK_MONITOR_BAR_SPACING + i*(PEAK_MONITOR_BAR_SPACING+PEAK_MONITOR_BAR_WIDTH);
                x2 = x1 + PEAK_MONITOR_BAR_WIDTH;
                y2 = PEAK_MONITOR_BOTTOM_Y;
                y1 = y2 - PEAK_MONITOR_Y_SIZE*fValue;
        
                v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffffff;
                v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffffff;
                v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xff0000ff;
                v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xff0000ff;
        
                m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
                m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
            }
    
            // Draw current max
            {
                struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
                BACKGROUNDVERTEX v[4];
                x1 = PEAK_MONITOR_LEFT_X + PEAK_MONITOR_BAR_SPACING + i*(PEAK_MONITOR_BAR_SPACING+PEAK_MONITOR_BAR_WIDTH);
                x2 = x1 + PEAK_MONITOR_BAR_WIDTH;
    
                fValue = (FLOAT)(fPeakLevels[i]/pow(2,23));
                y1 = PEAK_MONITOR_BOTTOM_Y - PEAK_MONITOR_Y_SIZE*fValue;
                y2 = y1+3;
    
        
                v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffff00;
                v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffff00;
                v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xffff00ff;
                v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xffff00ff;
        
                m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
                m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
            }
    
    
        }
        
    
        m_Font.Begin();
    
        if (m_dwPortNumber == -1) {
            swprintf (szString,L"Voice Peripheral Not Plugged in");
            m_Font.DrawText( 50,10, 0xFFFFFF00, szString);
        }
    
        swprintf (szString,L"Current Effect: %s",g_aEffectDescriptions[m_dwCurrentEffect].szDescription);
        m_Font.DrawText( 50, 50, 0xFFFFFF00, szString);
    
        swprintf (szString,L"Oscillator Frequency %d",m_lOscillatorFrequency);
        m_Font.DrawText( 50, 80, 0xFFFFFF00, szString);
    
        for( int i = 0; i < NUM_OPTIONS; i++ )
        {
            switch( i )
            {
            case DLS2_CutoffFrequency:
                swprintf( szString, L"%s: %0.2fHz", g_aOptions[i].szDescription, m_fDLS2Freq );
                break;
            case DLS2_Resonance:
                swprintf( szString, L"%s: %lddB", g_aOptions[i].szDescription, m_lDLS2Resonance / RES_SCALE );
                break;
            case PARAMEQ_Frequency:
                swprintf( szString, L"%s: %ldHz", g_aOptions[i].szDescription, m_lParamEQFreq );
                break;
            case PARAMEQ_Gain:
                swprintf( szString, L"%s: %lddB", g_aOptions[i].szDescription, m_lParamEQGain / PEQ_GAIN_SCALE );
                break;
            case PARAMEQ_Q:
                swprintf( szString, L"%s: %ld", g_aOptions[i].szDescription, m_lParamEQQ / RES_SCALE );
                break;
            }
            m_Font.DrawText( 50, FLOAT(110 + i * 30), i == m_dwParam ? 0xFFFFFFFF : 0xFFFFFF00, szString );
        }
    
        //m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();

    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CFilterGraph xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}
