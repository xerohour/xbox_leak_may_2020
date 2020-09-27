#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>

#include "dsptest.h"
#include "fximg.h"

#include "..\dsp\dspserver.h"


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_BLACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" }    
};

#define NUM_HELP_CALLOUTS 2



//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{

#ifdef BREAK_ON_START
    _asm int 3;
#endif

    CXBoxSample *pXbApp;
    pXbApp = new CXBoxSample();

    if (pXbApp == NULL) {
        return;
    }

    if( FAILED( pXbApp->Create() ) )
        return;
    pXbApp->Run();
}


//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    m_bDrawHelp = FALSE;
    m_fGPCycles = 0.0f;
    m_fEPCycles = 0.0f;
    m_fEPMinCycles = MAX_EP_IDLE_CYCLES;
    m_fEPMaxCycles = 0;
    m_dwGPMinCycles = MAX_GP_IDLE_CYCLES;
    m_dwGPMaxCycles = 0;
    m_dwCount = 0;
    m_dwCurrentEnv = 0;
    m_dwDelta = 0;
    m_pDirectSound = 0;

    m_fMaxMagnitude = 0.0;

    m_bDoDFT = FALSE;

    m_dwScreenSelected = DRAW_CYCLES;

    //
    // effect to use for snooping current audio data from its delay line
    //

    //m_dwEffectIndex = DELAY_CHAIN_DELAY;

    memset(m_fMaxLevels,0,sizeof(m_fMaxLevels));
    memset(m_szCurrentReverb,0,sizeof(m_szCurrentReverb));

}

//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT                 hr  = DS_OK;
    DSMIXBINS               dsMixBins;
    DSMIXBINVOLUMEPAIR      dsMixBinArray[8];

    memset(dsMixBinArray,0,sizeof(dsMixBinArray));

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
	


#ifdef USE_DSOUND
    hr = DirectSoundCreate(NULL, &m_pDirectSound, NULL);
#endif

#ifdef DOWNLOAD_SCRATCH_IMAGE
    DownloadScratch("d:\\media\\fximg.bin");
#endif


#ifdef ENABLE_DOLBY_DOWNLOAD
    hr = LoadDolbyCode();
#endif

    dsMixBins.dwMixBinCount = 6;
    dsMixBins.lpMixBinVolumePairs = dsMixBinArray;

    if (dsMixBins.dwMixBinCount > 1) {

        for (DWORD i=0;i<dsMixBins.dwMixBinCount;i++) {
    
            dsMixBinArray[i].dwMixBin = i;
            dsMixBinArray[i].lVolume =0;
            //m_pDirectSound->SetMixBinHeadroom(i, 0);
    
        }

    }
   
    //
    // last two speakers are FX sends
    //

    dsMixBins.dwMixBinCount = 2;
    dsMixBinArray[0].dwMixBin = DSMIXBIN_FXSEND_3;
    dsMixBinArray[1].dwMixBin = DSMIXBIN_FXSEND_4;

    //
    // create audio buffer
    //

    if(SUCCEEDED(hr)) hr = PlayLoopingBuffer("d:\\media\\sounds\\Heli.wav", &m_pDSBuffer,0);//DSBCAPS_CTRL3D);
    //if(SUCCEEDED(hr)) hr = CreateSineWaveBuffer(1,&m_pDSBuffer);

    m_pDSBuffer->SetMixBins(&dsMixBins);
    m_pDSBuffer->SetHeadroom(0);

#if SRC_TEST

    dsMixBins.dwMixBinCount = 1;
    dsMixBinArray[0].dwMixBin = DSMIXBIN_FRONT_LEFT;


    hr = VerifySRCEffect(&dsMixBins);
    if(FAILED(hr))
        return hr;
#endif


    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    LARGE_INTEGER liFreq;
    FLOAT         fSecondsPerTick;
    LARGE_INTEGER liStart;
    LARGE_INTEGER liCurrent;

    int i;
    DWORD dwValue;

    switch (m_dwScreenSelected) {
    case DRAW_SPECTRUM:
        if (m_bDoDFT) {
            FourierTransform();
        }
        break;

    case DRAW_CYCLES:
        //
        // get some DSP usage statistics straight form Y-ram and print them outacoustic.acoustic.wavacoustic.wavwav
        //acoustic.wav

#ifdef TRACK_TOTAL_GP_USAGE
        //
        // track total USAGE
        //

        dwValue = *(PDWORD)(0xFE830000+512-2*sizeof(DWORD));
        // convert used cycles to idle cycles
        m_fGPCycles = (FLOAT)(MAX_GP_IDLE_CYCLES-dwValue);

#else

        //
        // track a single FX usage
        //
        dwValue = *(PDWORD)(0xFE830000+512-4*sizeof(DWORD));
        m_fGPCycles = (FLOAT)(dwValue);

#endif

        // display used cycles
        //m_fGPCycles = (FLOAT)(dwValue);

        if (m_fGPCycles < m_dwGPMinCycles) {
            m_dwGPMinCycles = (DWORD)m_fGPCycles;
        }

        if (m_fGPCycles > m_dwGPMaxCycles) {
            m_dwGPMaxCycles = (DWORD)m_fGPCycles;
        }

        // used cycles in EP variable
        dwValue = MAX_EP_IDLE_CYCLES-*(PDWORD)(0xFE800000+0x5a000+0x4*sizeof(DWORD)); // burned cycles

        m_fEPCycles = (FLOAT) dwValue;

        if (m_fEPCycles <= m_fEPMinCycles) {
            m_fEPMinCycles = m_fEPCycles;
        }

        if (m_fEPCycles > m_fEPMaxCycles) {
            m_fEPMaxCycles = m_fEPCycles;
        }

        if (m_dwCount>1000) {

            m_dwCount = 0;
            m_fEPMinCycles = MAX_EP_IDLE_CYCLES;
            m_fEPMaxCycles = 0;
            m_dwGPMinCycles = MAX_GP_IDLE_CYCLES;
            m_dwGPMaxCycles = 0;

        }

        break;
    }


    m_dwCount++;

    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START ) 
    {

        //
        // progress to next screen
        //

        m_dwScreenSelected = (m_dwScreenSelected+1)%MAX_SCREENS;

    }

    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {

        SetFXOscillatorParameters(m_pDirectSound,OSCILLATOR_CHAIN_OSCILLATOR,(FLOAT)15100);

    }


    
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{

    switch (m_dwScreenSelected) {
    case DRAW_SPECTRUM:
        RenderSpectrum();
        break;
    case DRAW_CYCLES:
        RenderDSPUsage();
        break;
    }

    return S_OK;
}

HRESULT
CXBoxSample::RenderDSPUsage()
{
    
    FLOAT fYPos = 70, fXPos=50;
    WCHAR szString[256];
    DWORD dwValue=0;

    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    DirectSoundDoWork();

    // Begin the scene
    m_pd3dDevice->BeginScene();    

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    swprintf (szString,L"Current reverb: %ws",m_szCurrentReverb);
    m_Font.DrawText( fXPos, fYPos, 0xFF0000FF, szString);

    fYPos += 30.0;

    dwValue = (DWORD)m_fGPCycles;
    swprintf (szString,L"GP DSP Idle cycles per Frame: %d",dwValue);
    m_Font.DrawText( fXPos, fYPos, 0xFFFF0000, szString);

    fYPos += 30.0;

    dwValue = (DWORD)m_fEPCycles;
    swprintf (szString,L"EP DSP Idle cycles per Frame: %d",dwValue);
    m_Font.DrawText( fXPos, fYPos, 0xFF0000FF, szString);

    if (dwValue > m_dwDelta) {
        m_dwDelta = dwValue;
    }

    
    // Draw the GP Avg cycles Usage bar
    {
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
        BACKGROUNDVERTEX v[4];
        FLOAT x1 = 100, x2 = x1 + (340*m_fGPCycles)/MAX_GP_IDLE_CYCLES;
        FLOAT y1 = 130, y2 = y1 + 20;
        v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffffff;
        v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffffff;
        v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xffff0000;
        v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xffff0000;

        m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
    }

    // Draw the EP cycles bar
    {
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
        BACKGROUNDVERTEX v[4];
        FLOAT x1 = 100, x2 = x1 + (340*m_fEPCycles)/MAX_EP_IDLE_CYCLES;
        FLOAT y1 = 190, y2 = y1 + 20;
        v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffffff;
        v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffffff;
        v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xff0000ff;
        v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xff0000ff;

        m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
    }

    
    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
		m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"DSPTest" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
		m_Font.End();
    }

    m_pd3dDevice->EndScene();
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}


HRESULT CXBoxSample::DownloadScratch(PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer;
    DWORD err;
    HRESULT hr=S_OK;

    UCHAR data[4] = {0,1,2,3};

    //
    // open scratch image file generated by xps2 tool
    //


    hFile = CreateFile(
        pszScratchFile,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        fprintf(stderr,"\n Failed to open the dsp image file.Error 0x%x\n", err);
        hr = HRESULT_FROM_WIN32(err);
        _asm int 3;

    }

    if (SUCCEEDED(hr)) {

        dwSize = SetFilePointer(hFile, 0, NULL, FILE_END);              
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    
        pBuffer = new BYTE[dwSize];

        DWORD dwBytesRead;
        BOOL bResult = ReadFile(hFile,
            pBuffer,
            dwSize,
            &dwBytesRead,
            0);
        
        if (!bResult) {
            
            err = GetLastError();
            fprintf(stderr,"\n Failed to open the dsp image file.Error 0x%x\n", err);
            hr = HRESULT_FROM_WIN32(err);
    
        }

    }

    if (SUCCEEDED(hr)) {

        //
        // call dsound api to download the image..
        //

        if (SUCCEEDED(hr)) {
            DSEFFECTIMAGELOC        EffectLoc;

            EffectLoc.dwI3DL2ReverbIndex =DSFX_IMAGELOC_UNUSED;
            EffectLoc.dwCrosstalkIndex = DSFX_IMAGELOC_UNUSED;

            hr = m_pDirectSound->DownloadEffectsImage(pBuffer,
                                                      dwSize,
                                                      &EffectLoc,
                                                      &m_pEffectsImageDesc);

            if (SUCCEEDED(hr)) {

                //_asm int 3;
                m_pDirectSound->EnableHeadphones(TRUE);

            }
            
        }

    }

    if (SUCCEEDED(hr)) {

        if (m_bDoDFT) {

            //
            // allocate a buffer to hold a snapshot of the delay line
            //
    
            m_pdwAudioData = new DWORD[m_pEffectsImageDesc->aEffectMaps[m_dwEffectIndex].dwScratchSize];
    
            if (m_pdwAudioData == NULL) {
                hr = E_OUTOFMEMORY;
            }

        }

    }

    if (hFile) {
        CloseHandle(hFile);
    }
    
    delete [] pBuffer;

    return hr;
}

HRESULT CXBoxSample::VerifySRCEffect(LPDSMIXBINS pDsMixBins)
{

    HRESULT hr = S_OK;

#if SRC_TEST
    //
    // Initialize a wave format structure
    //
    WAVEFORMATEX wfx;
    LPDIRECTSOUNDBUFFER8 pBuffer = NULL;

    ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

    wfx.wFormatTag      = WAVE_FORMAT_PCM;      // PCM data
    wfx.nChannels       = 1;                    // Mono
    wfx.nSamplesPerSec  = 8000;                 
    wfx.nAvgBytesPerSec = 16000;                
    wfx.nBlockAlign     = 4;                    // sample size in bytes
    wfx.wBitsPerSample  = 32;                   // 16 bit samples
    wfx.cbSize          = 0;                    // No extra data

    //
    // Intialize the buffer description
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );

    dsbd.dwSize = sizeof( DSBUFFERDESC );
    dsbd.lpwfxFormat = &wfx;
    dsbd.lpMixBins = pDsMixBins;

    // Create the buffer
    hr = DirectSoundCreateBuffer( &dsbd, &pBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = pBuffer->SetBufferData(m_pEffectsImageDesc->aEffectMaps[SRC_CHAIN_SRC].lpvScratchSegment,
                                m_pEffectsImageDesc->aEffectMaps[SRC_CHAIN_SRC].dwScratchSize);

    hr = pBuffer->Play( 0, 0, DSBPLAY_LOOPING );
    if( FAILED( hr ) )
        return hr;

#endif
    return hr;

}

HRESULT CXBoxSample::UpdateReverb()
{

    static const struct
    {
        LPWSTR                      pszName;
        DSI3DL2LISTENER             Properties;
    } Environments[] =
    {
        { L"Default",         { DSI3DL2_ENVIRONMENT_PRESET_DEFAULT } },
        { L"Generic",         { DSI3DL2_ENVIRONMENT_PRESET_GENERIC } },
        { L"PaddedCell",      { DSI3DL2_ENVIRONMENT_PRESET_PADDEDCELL } },
        { L"Room",            { DSI3DL2_ENVIRONMENT_PRESET_ROOM } },
        { L"Bathroom",        { DSI3DL2_ENVIRONMENT_PRESET_BATHROOM } },
        { L"LivingRoom",      { DSI3DL2_ENVIRONMENT_PRESET_LIVINGROOM } },
        { L"StoneRoom",       { DSI3DL2_ENVIRONMENT_PRESET_STONEROOM } },
        { L"Auditorium",      { DSI3DL2_ENVIRONMENT_PRESET_AUDITORIUM } },
        { L"ConcertHall",     { DSI3DL2_ENVIRONMENT_PRESET_CONCERTHALL } },
        { L"Cave",            { DSI3DL2_ENVIRONMENT_PRESET_CAVE } },
        { L"Arena",           { DSI3DL2_ENVIRONMENT_PRESET_ARENA } },
        { L"Hangar",          { DSI3DL2_ENVIRONMENT_PRESET_HANGAR } },
        { L"CarpetedHallway", { DSI3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY } },
        { L"Hallway",         { DSI3DL2_ENVIRONMENT_PRESET_HALLWAY } },
        { L"StoneCorridor",   { DSI3DL2_ENVIRONMENT_PRESET_STONECORRIDOR } },
        { L"Alley",           { DSI3DL2_ENVIRONMENT_PRESET_ALLEY } },
        { L"Forest",          { DSI3DL2_ENVIRONMENT_PRESET_FOREST } },
        { L"City",            { DSI3DL2_ENVIRONMENT_PRESET_CITY } },
        { L"Mountains",       { DSI3DL2_ENVIRONMENT_PRESET_MOUNTAINS } },
        { L"Quarry",          { DSI3DL2_ENVIRONMENT_PRESET_QUARRY } },
        { L"Plain",           { DSI3DL2_ENVIRONMENT_PRESET_PLAIN } },
        { L"ParkingLot",      { DSI3DL2_ENVIRONMENT_PRESET_PARKINGLOT } },
        { L"SewerPipe",       { DSI3DL2_ENVIRONMENT_PRESET_SEWERPIPE } },
        { L"UnderWater",      { DSI3DL2_ENVIRONMENT_PRESET_UNDERWATER } },
    };

    m_dwCurrentEnv = (m_dwCurrentEnv+1)%24;
    HRESULT hr = m_pDirectSound->SetI3DL2Listener(&Environments[m_dwCurrentEnv].Properties, DS3D_IMMEDIATE);

    memset(m_szCurrentReverb,0,sizeof(m_szCurrentReverb));
    memcpy(m_szCurrentReverb,Environments[m_dwCurrentEnv].pszName,sizeof(m_szCurrentReverb));

    if (FAILED(hr)) {
        _asm int 3;
    }

    return hr;
}

