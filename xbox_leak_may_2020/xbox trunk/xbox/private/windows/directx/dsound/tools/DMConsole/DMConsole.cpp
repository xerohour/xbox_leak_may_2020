//-----------------------------------------------------------------------------
// File: DMConsole.cpp
//
// Desc: Xbox side of 'Xbox Experimenter' plugin for DMP.
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//
// Revised 9/26 to include DSP Server code
//
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBUtil.h>
#include <XBFont.h>
#include <XBHelp.h>

#include "DebugCmd.h"
#include "DMConsole.h"
#include <xbdm.h>
#include "dsstdfx.h"
#include <xtl.h>
// END NEW

#include "dspserver.h"
#include "wavbndlr.h"

#define XBOX_EXPERIMENTER_NAME "Xbox Experimenter"

static const DWORD MIDI_INDICATOR_OFF_DELAY = 500; //msec

extern CRITICAL_SECTION g_cs;
CXBoxSample g_xbApp;
BOOL dbgstrcmpn( LPCSTR sz1, LPCSTR sz2, int n );
int dbgstrlen( LPCSTR sz );
IDirectMusicLoader8*      g_pLoader         = NULL;
IDirectMusicPerformance8* g_pPerformance    = NULL;
//IDirectMusicPort*         g_pIDirectMusicPort = NULL;

extern void WbCheckStatusAll(void);

//------------------------------------------------------------------------------
//  PreDownloadEffectsImage
//------------------------------------------------------------------------------
void __cdecl
PreDownloadEffectsImage(void)
/*++

Routine Description:

    Stops the RMS display when a new image is about to be downloaded

Arguments:

    None

Return Value:

    None

--*/
{
    // stop reading data until download complete
    g_xbApp.m_bDisplayRMS = FALSE;
}

//------------------------------------------------------------------------------
//  DownloadEffectsImage
//------------------------------------------------------------------------------
void __cdecl
DownloadEffectsImage(
                     IN LPDSEFFECTIMAGEDESC pImageDes, 
                     IN int                 count,
                     IN char**              pEffectNames, 
                     IN int*                pEffectIndices
                     )
/*++

Routine Description:

    Looks for the rms effect index in the downloaded image to reset
    the rms display

Arguments:

    OUT pImageDes -         Pointer to the DSEFFECTIMAGEDESC 
                            structure that is returned by the 
                            method, which describes the DSP 
                            scratch image. 
    OUT count -             Number of effects in image
    OUT pEffectNames -      Array of effect name strings 
                            (each null terminated)
    OUT pEffectIndices -    Array of effect indices

Return Value:

    None

--*/
{
    // if we've got an RMS effect, find it and start reading effect data again
    int     i;
    bool    bFound;

    g_xbApp.m_pEffectsImageDesc = pImageDes;
    g_xbApp.m_bDisplayRMS = TRUE;
}

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
//  { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Adjust volume" },
    { XBHELP_A_BUTTON,      XBHELP_PLACEMENT_2, L"Reset peak meters" }
//    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Adjust groove\nlevel" },
};

#define NUM_HELP_CALLOUTS 2


//-----------------------------------------------------------------------------
// Name: InitDirectMusic
// Desc: Init DM
//-----------------------------------------------------------------------------
HRESULT InitDirectMusic() 
{
    // Initialize DirectMusic
    DirectMusicInitialize();

    // Create loader object
    DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL, 
                               IID_IDirectMusicLoader8, (VOID**)&g_pLoader );

    // Create performance object
    DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
                               IID_IDirectMusicPerformance8, (VOID**)&g_pPerformance );

    // Initialize the performance with the standard audio path.
    // The flags (final) argument allows us to specify whether or not we want
    // DirectMusic to create a thread on our behalf to process music, using 
    // DMUS_INITAUDIO_NOTHREADS.  The default is for DirectMusic to create its
    // own thread; DMUS_INITAUDIO_NOTHREADS tells DirectMusic not to do this, 
    // and the app will periodically call DirectMusicDoWork().  For software 
    // emulation on alpha hardware, it's generally better to have DirectMusic
    // create its own thread. On real hardware, periodically calling 
    // DirectMusicDoWork may provide a better option.
    g_pPerformance->InitAudioX( 0, 0, 128, 0 );

    g_pPerformance->AddNotificationType( GUID_NOTIFICATION_SEGMENT );

    // Now DirectMusic will play in the background, so continue on with our task
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetVolume()
// Desc: Set volume on the main music audipath
// Note: The argument to IDirectMusicAudioPath::SetVolume is attenuation in
//       hundredths of a dB.  Attenuation more than 60dB is silent, so
//       that's our effective minimum.  
// Note: No longer used, per raid #9045.
//-----------------------------------------------------------------------------
HRESULT SetVolume( LONG lVolume )
{
    // Change range from [0,100] to [-6000,0]
    lVolume = lVolume * 60 - 6000;

    static lOldVolume = 0;
    if (lOldVolume != lVolume)
    {
        lOldVolume = lVolume;    
        // Change volume on all audiopaths
        CAudiopathItem *pAudiopathTmp = (CAudiopathItem *)g_xbApp.m_lstAudiopaths.GetHead();
        while( pAudiopathTmp )   
        {
            pAudiopathTmp->m_pAudiopath8->SetVolume( lVolume, 0 );
            pAudiopathTmp = pAudiopathTmp->GetNext();
        }
    }
 
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetGrooveLevel()
// Desc: Change the groove level of the performance
//-----------------------------------------------------------------------------
HRESULT SetGrooveLevel( INT dwGrooveLevel )
{
    CHAR chGrooveLevel = (CHAR)dwGrooveLevel;
    g_pPerformance->SetGlobalParam( GUID_PerfMasterGrooveLevel, 
                                    &chGrooveLevel, sizeof(CHAR) );
    return S_OK;
}


//-----------------------------------------------------------------------------
// Variable declaration block:
// These are the variables exposed to the "set" command. Don't forget to update
// g_nVars to reflect the number of entries
//-----------------------------------------------------------------------------
const DCCMDVARDEF g_rgDCCMDVars[] = 
{
    { "groove", &g_xbApp.m_fGrooveLevel, SDOS_R4, RCmdChange },
    { "volume", &g_xbApp.m_fVolume, SDOS_R4, RCmdChange },
};
const UINT g_nVars = 2;


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    // Note: We defined g_xbApp as a global to give access to debug channel
    if( FAILED( g_xbApp.Create() ) )
        return;
    g_xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Application class constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
{
    // draw as fast as possible to sample more peak values
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    m_fGrooveLevel =  50.0f;
    m_fVolume      = 100.0f;        
    m_fMIDICounter =   0.0f;
    m_pDSound      = NULL;
    m_bSynthInitialized = FALSE;
    m_bDrawHelp = FALSE;
    m_bDisplayRMS = TRUE;

    for (DWORD k=0; k<PEAK_NUM_BINS; k++)
    {
        m_fMaxLevels[k] = 0.0f;
        m_fHistoryBufferAverage[k] = 0.0f;
        for (DWORD j=0; j<HISTORY_BUFFER_SIZE; j++)
            m_fLevelsHistoryBuffer[k][j] = 0.0f;
    }
    m_dwHistoryBufferIndex = 0;

	m_fLastMIDICounterValue = 0.0f;
	m_dwMIDIIndicatorOnTime = 0;
}




//-----------------------------------------------------------------------------
// Name: DownloadScratch
// Desc: Downloads a DSP scratch image to the DSP
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DownloadScratch(PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize = 0;
    PVOID pBuffer = NULL;
    HRESULT hr = S_OK;

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
        OutputDebugString( _T("Failed to open the dsp image file.\n") );
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
            OutputDebugString( _T("\n Failed to open the dsp image file.\n") );
            hr = HRESULT_FROM_WIN32(err);
        }

    }

    if( SUCCEEDED(hr) )
    {
        // call dsound api to download the image..
        if (SUCCEEDED(hr))
        {
            DSEFFECTIMAGELOC dsEffectImageLoc;
            dsEffectImageLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
            dsEffectImageLoc.dwCrosstalkIndex = I3DL2_CHAIN_XTALK;
            hr = m_pDSound->DownloadEffectsImage( pBuffer,
                                                  dwSize,
                                                  &dsEffectImageLoc,
                                                  &m_pEffectsImageDesc );
            m_bDisplayRMS = TRUE;
        }
    }

    delete[] pBuffer;

    if( hFile != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hFile );
    }
    
    return hr;
}



// only needed if we run update in a separate thread
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
((CXBoxSample *)(lpParameter))->UpdatePeakLevels();
// should never return
return (0);
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    if( FAILED( InitDirectMusic() ) )
        return E_FAIL;

    if( FAILED( m_FontBig.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_FontSmall.Create( m_pd3dDevice, "font12.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    DirectSoundCreate( NULL, &m_pDSound, NULL );
    if( FAILED( DownloadScratch( "D:\\media\\dsstdfx.bin" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    DirectSoundUseFullHRTF();

    m_bDrawHelp = FALSE;

// Start thread that checks for DSP refresh calls from DSPBuilder
    CDSPServer::Run(m_pDSound, 80, 1000, PreDownloadEffectsImage,
                    DownloadEffectsImage);

// Start thread that runs calculations on RMS effect (if present)
// Not needed if main render loop is fast enough
//  CreateThread (NULL, 0, ThreadProc, this, 0, NULL);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: IsAnyButtonActive()
// Desc: TRUE if any button depressed or any thumbstick offset on the given
//       controller.
//-----------------------------------------------------------------------------
BOOL IsAnyButtonActive( const XBGAMEPAD* pGamePad )
{
    // Check digital buttons
    if( pGamePad->wButtons )
        return TRUE;

    // Check analog buttons
    for( DWORD i = 0; i < 8; ++i )
    {
        if( pGamePad->bAnalogButtons[ i ] )
            return TRUE;
    }

    // Check thumbsticks
    if( pGamePad->fX1 || pGamePad->fY1 || pGamePad->fX2 || pGamePad->fX2  )
        return TRUE;

    // Nothing active
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetPrimaryController()
// Desc: The primary controller is the first controller used by a player.
//       If no controller has been used or the controller has been removed,
//       the primary controller is the controller inserted at the lowest 
//       port number. Function returns NULL if no controller is inserted.
//-----------------------------------------------------------------------------
const XBGAMEPAD* GetPrimaryController()
{
    static INT nPrimaryController = -1;

    // If primary controller has been set and hasn't been removed, use it
    const XBGAMEPAD* pGamePad = NULL;
    if( nPrimaryController != -1 )
    {
        pGamePad = &g_Gamepads[ nPrimaryController ];
        if( pGamePad->hDevice != NULL )
            return pGamePad;
    }

    // Primary controller hasn't been set or has been removed...

    // Examine each inserted controller to see if any is being used
    INT nFirst = -1;
    for( DWORD i=0; i < XGetPortCount(); ++i )
    {
        pGamePad = &g_Gamepads[i];
        if( pGamePad->hDevice != NULL )
        {
            // Remember the lowest inserted controller ID
            if( nFirst == -1 )
                nFirst = i;

            // If any button is active, we found the primary controller
            if( IsAnyButtonActive( pGamePad ) )
            {
                nPrimaryController = i;
                return pGamePad;
            }
        }
    }

    // No controllers are inserted
    if( nFirst == -1 )
        return NULL;

    // The primary controller hasn't been set and no controller has been
    // used yet, so return the controller on the lowest port number
    pGamePad = &g_Gamepads[ nFirst ];
    return pGamePad;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    const XBGAMEPAD* pGamepad = GetPrimaryController();
    if( pGamepad )
    {
        // Toggle help
        if( pGamepad->wPressedButtons & XINPUT_GAMEPAD_BACK ) 
        {
            m_bDrawHelp = !m_bDrawHelp;
        }

        // NEW - reset peak levels
        if ( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] && m_bDisplayRMS)
        {
            DSOUTPUTLEVELS dsLevels;
            for (int k=0; k<PEAK_NUM_BINS; k++)
            {
                m_fMaxLevels[k] = 0.0f;
            } 
            m_pDSound->GetOutputLevels(&dsLevels, TRUE);
        }

        // Adjust volume
        /*
        m_fVolume += pGamepad->fY1 * m_fElapsedTime * 100.0f;
        if( m_fVolume <   1.0f )   m_fVolume =   1.0f;
        if( m_fVolume > 100.0f )   m_fVolume = 100.0f;
        SetVolume( (INT)m_fVolume);
        */

        // Adjust sound
        /*
        m_fGrooveLevel += pGamepad->fY2 * m_fElapsedTime * 100.0f;
        if( m_fGrooveLevel <   1.0f )   m_fGrooveLevel =   1.0f;
        if( m_fGrooveLevel > 100.0f )   m_fGrooveLevel = 100.0f;
        SetGrooveLevel( (INT)(m_fGrooveLevel - 50) );
        */

        // Play sound based on button pressed
        /*
        if( pGamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) 
            PlaySound( g_pSound1 );

        if( pGamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_A] ) 
            PlaySound( g_pSound2 );
        */

        // Handle requests for debug output
        /*
        if( m_DefaultGamepad.bPressedAnalogButtons[0] )
        {
            DCCMDPrintf("Framerate is %s\n", m_strFrameRate );
        }
        if( m_DefaultGamepad.bPressedAnalogButtons[1]  )
        {
            DCCMDPrintf("Current texture is %s\n", m_szTexFile );
        }
        */
    }

    // Process any pending commands from the debug channel
    DCHandleCmds(); 

    // Process any pending buffers
    //DCHandleBuffers();


    // NEW - Do Fast Fourier Transform for volume LED display
//  if (m_bDisplayRMS)
//      FourierTransform(); 

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    EnterCriticalSection(&g_cs);
    
    /* Replaced by DirectMusic's own thread
    // Give DM a chance to keep playing. The quantum is the amount of time in
    // mS we expect until DirectMusicDoWork() will be called again. At 60fps,
    // the quantum is 16.667.
    const DWORD dwFps = 60;
    const DWORD dwQuantum = 1000 / dwFps;
    DirectMusicDoWork( dwQuantum );
    */

    DMUS_NOTIFICATION_PMSG *pNotificationMsg;
    // S_FALSE means no more messages, and pNotificationMsg is NULL
    if( S_OK == g_pPerformance->GetNotificationPMsg( &pNotificationMsg ) )
    {
        if( GUID_NOTIFICATION_SEGMENT == pNotificationMsg->guidNotificationType )
        {
            HandleSegmentNotification( pNotificationMsg );
        }
        g_pPerformance->FreePMsg( reinterpret_cast<DMUS_PMSG*>(pNotificationMsg) );
    }

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0xff0000ff, 1.0f, 0L );

    // Begin the scene
    m_pd3dDevice->BeginScene();
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_FontBig, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        /*
        m_FontBig.DrawText( 64, 150, 0xffffff00, L"Volume:" );
        {
            struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
            BACKGROUNDVERTEX v[4];
            FLOAT x1 = 200, x2 = x1 + (340*m_fVolume)/100;
            FLOAT y1 = 152, y2 = y1 + 20;
            v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffffff;
            v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffffff;
            v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xffff0000;
            v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xffff0000;

            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
            m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
        }
        */

        /*
        m_FontBig.DrawText( 64, 200, 0xffffff00, L"Groove Level:" );
        {
            struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
            BACKGROUNDVERTEX v[4];
            FLOAT x1 = 200, x2 = x1 + (340*m_fGrooveLevel)/100;
            FLOAT y1 = 200, y2 = y1 + 20;
            v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffffff;
            v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffffff;
            v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xff00ff00;
            v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xff00ff00;

            m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
            m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
        }
        // Display the groove level number
        {
            WCHAR wcstr[MAX_PATH];
            wsprintfW(wcstr, L"(%d)", (INT)(m_fGrooveLevel - 50) );
            m_FontBig.DrawText( 64, 220, 0xffffff00, wcstr );
        }
        */

        // Synth stats
/*      if( g_pIDirectMusicPort )
        {
            DMUS_SYNTHSTATS8 dmSynthStats;
            ZeroMemory( &dmSynthStats, sizeof( DMUS_SYNTHSTATS8 ) );
            dmSynthStats.dwSize = sizeof( DMUS_SYNTHSTATS8 );
            if( SUCCEEDED( g_pIDirectMusicPort->GetRunningStats( (LPDMUS_SYNTHSTATS)&dmSynthStats ) ) )
            {
                WCHAR wcstr[MAX_PATH];
                if( dmSynthStats.dwValidStats & DMUS_SYNTHSTATS_VOICES )
                {
                    wsprintfW(wcstr, L"%d", dmSynthStats.dwVoices );
                    m_FontBig.DrawText( 64, 250, 0xffffff00, L"Voices" );
                    m_FontBig.DrawText( 300, 250, 0xffffff00, wcstr );
                }
                if( dmSynthStats.dwValidStats & DMUS_SYNTHSTATS_TOTAL_CPU )
                {
                    wsprintfW(wcstr, L"%d.%d%%", dmSynthStats.dwTotalCPU/100, dmSynthStats.dwTotalCPU%100 );
                    m_FontBig.DrawText( 64, 270, 0xffffff00, L"Total CPU" );
                    m_FontBig.DrawText( 300, 270, 0xffffff00, wcstr );
                }
                if( dmSynthStats.dwValidStats & DMUS_SYNTHSTATS_CPU_PER_VOICE )
                {
                    wsprintfW(wcstr, L"%d.%d%%", dmSynthStats.dwCPUPerVoice/100, dmSynthStats.dwCPUPerVoice%100 );
                    m_FontBig.DrawText( 64, 290, 0xffffff00, L"CPU per voice" );
                    m_FontBig.DrawText( 300, 290, 0xffffff00, wcstr );
                }
                if( dmSynthStats.dwValidStats & DMUS_SYNTHSTATS_LOST_NOTES )
                {
                    wsprintfW(wcstr, L"%d", dmSynthStats.dwLostNotes );
                    m_FontBig.DrawText( 64, 310, 0xffffff00, L"Lost notes" );
                    m_FontBig.DrawText( 300, 310, 0xffffff00, wcstr );
                }
                if( dmSynthStats.dwValidStats & DMUS_SYNTHSTATS_PEAK_VOLUME )
                {
                    wsprintfW(wcstr, L"%ddb", dmSynthStats.lPeakVolume );
                    m_FontBig.DrawText( 64, 330, 0xffffff00, L"Peak volume" );
                    m_FontBig.DrawText( 300, 330, 0xffffff00, wcstr );
                }
                if( dmSynthStats.dwValidStats & DMUS_SYNTHSTATS_FREE_MEMORY )
                {
                    wsprintfW(wcstr, L"%d", dmSynthStats.dwFreeMemory );
                    m_FontBig.DrawText( 64, 350, 0xffffff00, L"Free memory" );
                    m_FontBig.DrawText( 300, 350, 0xffffff00, wcstr );
                }
                wsprintfW(wcstr, L"%d", dmSynthStats.dwSynthMemUse );
                m_FontBig.DrawText( 64, 370, 0xffffff00, L"Memory use" );
                m_FontBig.DrawText( 300, 370, 0xffffff00, wcstr );
            }
        }*/

        WCHAR szString[256];
        swprintf (szString,L"Audio Console App with DSP Server (%i)", _XTL_VER);
        m_FontBig.DrawText(  64, 50, 0xffffffff, szString);
        
        CConnectionItem *pConnection = (CConnectionItem *)m_lstConnections.GetHead();

        if(pConnection)
        {
            DWORD dwOffset = 80;

            while(pConnection)
            {
                MultiByteToWideChar(CP_ACP, 0, pConnection->m_szName, -1, szString, sizeof(szString) / sizeof(szString[0]));
                wcscat(szString, L" connected");
                
                m_FontBig.DrawText( 64, (FLOAT)dwOffset, 0xffffff00, szString );
                
                dwOffset += 20;

                pConnection = pConnection->GetNext();
            }

            dwOffset += 20;

            if( m_bSynthInitialized )
            {
				DWORD dwTicks = GetTickCount();
				if (m_fMIDICounter != m_fLastMIDICounterValue)
				{
					m_fLastMIDICounterValue = m_fMIDICounter;
					m_dwMIDIIndicatorOnTime = dwTicks;
				}
				if ((dwTicks - m_dwMIDIIndicatorOnTime) > MIDI_INDICATOR_OFF_DELAY)
					m_FontBig.DrawText( 64, (FLOAT)dwOffset, 0xff666666, L"MIDI idle");
				else
					m_FontBig.DrawText( 64, (FLOAT)dwOffset, 0xff00aa00, L"MIDI received" );
			}
        }
        else
        {
            m_FontBig.DrawText( 110, 160, 0xffffff00, L"This tool is designed for composers and sound" );
            m_FontBig.DrawText( 110, 180, 0xffffff00, L"designers to audition their content on the Xbox" );
            m_FontBig.DrawText( 110, 200, 0xffffff00, L"hardware.  It is used in conjunction with PC-" );
            m_FontBig.DrawText( 110, 220, 0xffffff00, L"based audio tools, such as DirectMusic Producer");
            m_FontBig.DrawText( 110, 240, 0xffffff00, L"for Xbox, Xbox Wave Bundler, and DSP Builder,");
            m_FontBig.DrawText( 110, 260, 0xffffff00, L"which manage content and effects program" );
            m_FontBig.DrawText( 110, 280, 0xffffff00, L"downloading to the Xbox as well as playback of");
            m_FontBig.DrawText( 110, 300, 0xffffff00, L"that content. For more information, please" );
            m_FontBig.DrawText( 110, 320, 0xffffff00, L"consult the tools chapter of the Audio Best" );
			m_FontBig.DrawText( 110, 340, 0xffffff00, L"Practices Guide at https://xds.xbox.com");
        }
        // NEW - Draw Volume LEDs
// Probably needs critical section to handle when effects program changed via DSP Builder
//      EnterCriticalSection(&m_criticalSection);
        if (m_bDisplayRMS)
        {
            UpdatePeakLevels();
            RenderPeakLevels();
        }
//      LeaveCriticalSection(&m_criticalSection);
    }
    // End scene
    m_pd3dDevice->EndScene();

    LeaveCriticalSection(&g_cs);
    
    WbCheckStatusAll();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Adds a remote connection to the application.
//-----------------------------------------------------------------------------
void CXBoxSample::AddConnection(LPCSTR pszConnection)
{
    CConnectionItem *       pConnection;
    
    EnterCriticalSection(&g_cs);
    
    if(pConnection = new CConnectionItem)
    {
        strncpy(pConnection->m_szName, pszConnection, sizeof(pConnection->m_szName));
        m_lstConnections.AddTail(pConnection);
    }

    LeaveCriticalSection(&g_cs);
}

//-----------------------------------------------------------------------------
// Removes a remote connection to the application.
//-----------------------------------------------------------------------------
void CXBoxSample::RemoveConnection(LPCSTR pszConnection)
{
    CConnectionItem *       pConnection;
    
    EnterCriticalSection(&g_cs);
    
    for(pConnection = (CConnectionItem *)m_lstConnections.GetHead(); pConnection; pConnection = pConnection->GetNext())
    {
        if(!strncmp(pConnection->m_szName, pszConnection, sizeof(pConnection->m_szName)))
        {
            m_lstConnections.Remove(pConnection);
            delete pConnection;

            break;
        }
    }

    LeaveCriticalSection(&g_cs);
}

//-----------------------------------------------------------------------------
// Name: SendNotification()
// Desc: Sends a notification with an encoded HRESULT
//-----------------------------------------------------------------------------
void SendNotification( char *strName, DWORD dwData1, DWORD dwData2 )
{
    // Pass the result back to the Windows application
    char strNotification[CCH_MAXCMDLEN];

    // Zero buffer
    ZeroMemory( strNotification, sizeof(char) * CCH_MAXCMDLEN );

    // Copy command prefix and name
    strcpy( strNotification, g_szCmd );
    strcat( strNotification, strName );

    char strNumber[16];

    // Add dwData1
    _itoa( dwData1, strNumber, 10 );
    strcat( strNotification, strNumber );

    if( dwData2 != 0xFFFFFFFF )
    {
        // Add a space
        strcat( strNotification, " " );

        // Add dwData2
        _itoa( dwData2, strNumber, 10 );
        strcat( strNotification, strNumber );
    }

    DmSendNotificationString( strNotification );
}

//-----------------------------------------------------------------------------
// Name: HandleSegmentNotification()
// Desc: Handle a segment notification
//-----------------------------------------------------------------------------
void CXBoxSample::HandleSegmentNotification( DMUS_NOTIFICATION_PMSG *pNotificationMsg )
{
    if( pNotificationMsg->punkUser == NULL )
    {
        return;
    }

    // Find this segment
    CSegmentItem *pSegmentItem = (CSegmentItem *)g_xbApp.m_lstSegments.GetHead();
    while( pSegmentItem )
    {
        if( pSegmentItem->m_pSegmentState8 == pNotificationMsg->punkUser )
        {
            break;
        }
        pSegmentItem = pSegmentItem->GetNext();
    }

    // If we found the segment
    if( pSegmentItem )
    {
        // If we're stopping, release our segment state
        if( (pNotificationMsg->dwNotificationOption == DMUS_NOTIFICATION_SEGEND)
        ||  (pNotificationMsg->dwNotificationOption == DMUS_NOTIFICATION_SEGABORT) )
        {
            pSegmentItem->m_pSegmentState8->Release();
            pSegmentItem->m_pSegmentState8 = NULL;
        }

        // Pass the notification on the the Windows application
        SendNotification( "!Segment ", pSegmentItem->m_dwSegmentID, pNotificationMsg->dwNotificationOption );
    }
}


//-----------------------------------------------------------------------------
// Name: Adds a connection to the application.
//-----------------------------------------------------------------------------
void RCmdAddConnection(int argc, char *argv[])
{
    CHAR                    szName[0x100];
    int                     i;
    
    if(argc < 2)
    {
        OutputDebugString("Invalid number of arguments");
        return;
    }

    strcpy(szName, argv[1]);

    for(i = 2; i < argc; i++)
    {
        strcat(szName, " ");
        strcat(szName, argv[i]);
    }

    g_xbApp.AddConnection(szName);
}


//-----------------------------------------------------------------------------
// Name: Removes a connection to the application.
//-----------------------------------------------------------------------------
void RCmdRemoveConnection(int argc, char *argv[])
{
    CHAR                    szName[0x100];
    int                     i;
    
    if(argc < 2)
    {
        OutputDebugString("Invalid number of arguments");
        return;
    }

    strcpy(szName, argv[1]);

    for(i = 2; i < argc; i++)
    {
        strcat(szName, " ");
        strcat(szName, argv[i]);
    }

    g_xbApp.RemoveConnection(szName);
}


//-----------------------------------------------------------------------------
// Name: Initializes the Xbox Experimenter functions
//-----------------------------------------------------------------------------
void RCmdInitializeExperimenter(int argc, char *argv[])
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    
    g_xbApp.AddConnection(XBOX_EXPERIMENTER_NAME);
}


//-----------------------------------------------------------------------------
// Name: Uninitializes the Xbox Experimenter functions
//-----------------------------------------------------------------------------
void RCmdUninitializeExperimenter(int argc, char *argv[])
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    
    // Stop the performance
    g_pPerformance->StopEx( 0, 0, 0 );

    // Wait for everything to really stop
    Sleep(10);

    // Clear the default audiopath and port
    g_pPerformance->SetDefaultAudioPath( NULL );
/*  if( g_pIDirectMusicPort )
    {
        g_pIDirectMusicPort->Release();
        g_pIDirectMusicPort = NULL;
    }*/

    // Delete all the segments
    while( !g_xbApp.m_lstSegments.IsEmpty() )
    {
        // Get the first segment
        CSegmentItem *pSegmentItem = (CSegmentItem *)g_xbApp.m_lstSegments.RemoveHead();

        // If the segment state exists, stop it
        if( pSegmentItem->m_pSegmentState8 )
        {
            g_pPerformance->StopEx( pSegmentItem->m_pSegmentState8, 0, 0 );
            pSegmentItem->m_pSegmentState8->Release();
            pSegmentItem->m_pSegmentState8 = NULL;
        }

        // Unload the segment
        if( pSegmentItem->m_pSegment8 )
        {
            pSegmentItem->m_pSegment8->Unload( g_pPerformance );

            // Release the segment
            pSegmentItem->m_pSegment8->Release();
            pSegmentItem->m_pSegment8 = NULL;
        }

        // Delete the item
        delete pSegmentItem;
    }

    // Delete all the audiopaths
    while( !g_xbApp.m_lstAudiopaths.IsEmpty() )
    {
        CAudiopathItem *pAudiopathItem = (CAudiopathItem *)g_xbApp.m_lstAudiopaths.RemoveHead();

        // Release the Audiopath
        if( pAudiopathItem->m_pAudiopath8 )
        {
            pAudiopathItem->m_pAudiopath8->Release();
            pAudiopathItem->m_pAudiopath8 = NULL;
        }

        // Delete the item
        delete pAudiopathItem;
    }

    //ASSERT( g_xbApp.m_lstScripts.IsEmpty() );

    g_xbApp.RemoveConnection(XBOX_EXPERIMENTER_NAME);
}


//-----------------------------------------------------------------------------
// Name: RCmdPlay
// Desc: Sets the new texture to be used
//-----------------------------------------------------------------------------
void RCmdPlay(int argc, char *argv[])
{
    HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 5 )
    {
        DCCMDPrintf("Need to specify a segment ID, flags, audiopathID, and default audiopath ID \n");
        hr = E_INVALIDARG;
    }

    DWORD dwSegmentID = 0xFFFFFFFF;
    CSegmentItem *pSegmentItem = NULL;

    if( SUCCEEDED(hr) )
    {
        dwSegmentID = atoi(argv[1]);

        // Ensure this segment exists
        CSegmentItem *pSegmentTmp = (CSegmentItem *)g_xbApp.m_lstSegments.GetHead();
        while( pSegmentTmp )
        {
            if( dwSegmentID == pSegmentTmp->m_dwSegmentID )
            {
                pSegmentItem = pSegmentTmp;
                break;
            }
            pSegmentTmp = pSegmentTmp->GetNext();
        }
    }

    if( NULL == pSegmentItem )
    {
        OutputDebugString( "RCmdPlay: Segment ID not found\n" );
        hr = E_INVALIDARG;
    }

    // Ensure this segment isn't already playing
    if( SUCCEEDED(hr) )
    {
        if( pSegmentItem->m_pSegmentState8 )
        {
            OutputDebugString( "RCmdPlay: Segment already playing\n" );
            hr = E_FAIL;
        }
    }

    DWORD dwPlayFlags = 0;
    DWORD dwAudiopathID = 0;
    DWORD dwStandardAudiopathID = 0;
    IDirectMusicAudioPath *pIDirectMusicAudioPath = NULL;
    
    if( SUCCEEDED(hr) )
    {
        dwPlayFlags = atoi(argv[2]);
        dwAudiopathID = atoi(argv[3]);
        dwStandardAudiopathID = atoi(argv[4]);

        // If specified, ensure this audiopath exists
        if( dwAudiopathID )
        {
            CAudiopathItem *pAudiopathTmp = (CAudiopathItem *)g_xbApp.m_lstAudiopaths.GetHead();
            while( pAudiopathTmp )
            {
                if( dwAudiopathID == pAudiopathTmp->m_dwAudiopathID )
                {
                    pIDirectMusicAudioPath = pAudiopathTmp->m_pAudiopath8;
                    if( pIDirectMusicAudioPath)
                    {
                        pIDirectMusicAudioPath->AddRef();
                    }
                    break;
                }
                pAudiopathTmp = pAudiopathTmp->GetNext();
            }
        }

        // If specified, create a standard audiopath
        if( !pIDirectMusicAudioPath
        &&  dwStandardAudiopathID )
        {
            hr = g_pPerformance->CreateStandardAudioPath( dwStandardAudiopathID, 128, TRUE, &pIDirectMusicAudioPath );

            if( FAILED(hr)
            ||  !pIDirectMusicAudioPath )
            {
                OutputDebugString( "RCmdPlay: Unable to create standard Audiopath\n" );
                if( SUCCEEDED(hr) )
                {
                    hr = E_INVALIDARG;
                }
            }
        }
    }

    if( SUCCEEDED(hr)
    &&  dwAudiopathID
    &&  NULL == pIDirectMusicAudioPath )
    {
        OutputDebugString( "RCmdPlay: Audiopath ID not found\n" );
        hr = E_INVALIDARG;
    }

    IDirectMusicSegmentState8 *pIDirectMusicSegmentState8 = NULL;

    // Play segment on the default audio path 
    if( SUCCEEDED(hr) ) 
    { // DMUS_SEGF_SEGMENTEND | DMUS_SEGF_ALIGN | DMUS_SEGF_VALID_START_TICK 
//        pSegmentItem->m_pSegment8->SetStartPoint(768*19);
        hr = g_pPerformance->PlaySegmentEx( pSegmentItem->m_pSegment8, NULL, NULL, dwPlayFlags ,  
                                            0, &pIDirectMusicSegmentState8, NULL, pIDirectMusicAudioPath );
    } 
  
    if( SUCCEEDED( hr ) ) 
    {
        // Save the segment state
        // No need to AddRef() - PlaySegmentEx did that for us
        pSegmentItem->m_pSegmentState8 = pIDirectMusicSegmentState8;
    }

    if( pIDirectMusicAudioPath) 
    { 
        pIDirectMusicAudioPath->Release();
    }

    // Pass the result back to the Windows application
    SendNotification( "!PlayResult ", dwSegmentID, hr );
}

//-----------------------------------------------------------------------------
// Name: RCmdStop
// Desc: Stops the segment
//-----------------------------------------------------------------------------
void RCmdStop(int argc, char *argv[])
{
    // Check our arguments
    if( argc < 3 )
    {
        DCCMDPrintf("Need to specify an ID and flags\n");
        return;
    }

    const DWORD dwSegmentID = atoi(argv[1]);
    const DWORD dwStopFlags = atoi(argv[2]);

    CSegmentItem *pSegmentItem = (CSegmentItem *)g_xbApp.m_lstSegments.GetHead();
    while( pSegmentItem )
    {
        if( dwSegmentID == pSegmentItem->m_dwSegmentID )
        {
            if( pSegmentItem->m_pSegmentState8 )
            {
                g_pPerformance->StopEx( pSegmentItem->m_pSegmentState8, 0, 0 );
                // Don't release the segment state here - wait for the SEGABORT notification
            }
            else
            {
                OutputDebugString( "RCmdStop: Segment not playing\n" );
            }

            return;
        }
        pSegmentItem = pSegmentItem->GetNext();
    }

    if( NULL == pSegmentItem )
    {
        OutputDebugString( "RCmdStop: Segment not found\n" );
    }
}

//-----------------------------------------------------------------------------
// Name: RCmdChange
// Desc: Called after changing one of our lighting values, so that we can
//       reset the light
//-----------------------------------------------------------------------------
void RCmdChange( void * pvAddr )
{
    //g_pd3dDevice->SetLight( 0, &g_xbApp.m_Light );
}

//-----------------------------------------------------------------------------
// Name: RCmdSearchDirectory
// Desc: sets the search directory
//-----------------------------------------------------------------------------
void RCmdSearchDirectory(int argc, char *argv[])
{
    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify the search directory\n");
        return;
    }

    // Tell DirectMusic where the default search path is
    g_pLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
                                   argv[1], TRUE );
}

//-----------------------------------------------------------------------------
// Name: RCmdClearCache
// Desc: clears the cache
//-----------------------------------------------------------------------------
void RCmdClearCache(int argc, char *argv[])
{
    UNREFERENCED_PARAMETER( argc );
    UNREFERENCED_PARAMETER( argv );

    // Tell DirectMusic to clear its cache
    HRESULT hr = g_pLoader->ClearCache( GUID_DirectMusicAllTypes );

    // Pass the result back to the Windows application
    SendNotification( "!ClearCacheResult ", hr, 0xFFFFFFFF );
}

//-----------------------------------------------------------------------------
// Name: RCmdLoad
// Desc: loads a segment
//-----------------------------------------------------------------------------
void RCmdLoad(int argc, char *argv[])
{
    HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 3 )
    {
        DCCMDPrintf("Need to specify the ID and filename\n");
        hr = E_INVALIDARG;
    }

    DWORD dwSegmentID = 0xFFFFFFFF;
    CSegmentItem *pSegmentItem = NULL;
    
    if( SUCCEEDED(hr) )
    {
        dwSegmentID = atoi(argv[1]);

        // Ensure this segment ID doesn't exist
        pSegmentItem = (CSegmentItem *)g_xbApp.m_lstSegments.GetHead();
        while( pSegmentItem )
        {
            if( dwSegmentID == pSegmentItem->m_dwSegmentID )
            {
                OutputDebugString( "RCmdLoad: Segment ID already in use\n" );
                hr = E_INVALIDARG;
                break;
            }
            pSegmentItem = pSegmentItem->GetNext();
        }
    }

    // Load segment
    IDirectMusicSegment8 *pIDirectMusicSegment8 = NULL;
    if( SUCCEEDED(hr) )
    {
        hr = g_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, 
                                            argv[2], (VOID**)&pIDirectMusicSegment8 );
    }
 
    if( SUCCEEDED( hr ) )
    {
        // Download music
        hr = pIDirectMusicSegment8->Download( g_pPerformance );
    }

    if( SUCCEEDED( hr ) )
    {
        // Add segment to list
        pSegmentItem = new CSegmentItem;
        pSegmentItem->m_dwSegmentID = dwSegmentID;
        pSegmentItem->m_pSegment8 = pIDirectMusicSegment8;
        pSegmentItem->m_pSegment8->AddRef();
        g_xbApp.m_lstSegments.AddHead( pSegmentItem );
    }

    // Release segment
    if( pIDirectMusicSegment8 )
    {
        pIDirectMusicSegment8->Release();
    }

    // Pass the result back to the Windows application
    SendNotification( "!LoadResult ", dwSegmentID, hr );
}

//-----------------------------------------------------------------------------
// Name: RCmdUnload
// Desc: unloads a segment
//-----------------------------------------------------------------------------
void RCmdUnload(int argc, char *argv[])
{
    HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify the ID\n");
        hr = E_INVALIDARG;
    }

    DWORD dwSegmentID = 0xFFFFFFFF;
    CSegmentItem *pSegmentItem = NULL;
    if( SUCCEEDED(hr) )
    {
        dwSegmentID = atoi(argv[1]);

        // Ensure this segment ID exists
        CSegmentItem *pSegmentTemp = (CSegmentItem *)g_xbApp.m_lstSegments.GetHead();
        while( pSegmentTemp )
        {
            if( dwSegmentID == pSegmentTemp->m_dwSegmentID )
            {
                pSegmentItem = pSegmentTemp;
                break;
            }
            pSegmentTemp = pSegmentTemp->GetNext();
        }
    }

    if( NULL == pSegmentItem )
    {
        OutputDebugString( "RCmdUnload: Segment ID not found\n" );
        hr = E_INVALIDARG;
    }

    // If the segment state exists, stop it
    if( SUCCEEDED(hr)
    &&  pSegmentItem->m_pSegmentState8 )
    {
        g_pPerformance->StopEx( pSegmentItem->m_pSegmentState8, 0, 0 );
        pSegmentItem->m_pSegmentState8->Release();
        pSegmentItem->m_pSegmentState8 = NULL;
    }

    // Unload the segment
    if( SUCCEEDED(hr) )
    {
        hr = pSegmentItem->m_pSegment8->Unload( g_pPerformance );
    }

    if( pSegmentItem )
    {
        // Release the segment
        pSegmentItem->m_pSegment8->Release();
        pSegmentItem->m_pSegment8 = NULL;

        // Remove the item from the list
        g_xbApp.m_lstSegments.Remove( pSegmentItem );

        // Delete the item
        delete pSegmentItem;
    }

    // Pass the result back to the Windows application
    SendNotification( "!UnloadResult ", dwSegmentID, hr );
}

//-----------------------------------------------------------------------------
// Name: RCmdPanic
// Desc: Stop everything
//-----------------------------------------------------------------------------
void RCmdPanic(int argc, char *argv[])
{
    UNREFERENCED_PARAMETER( argc );
    UNREFERENCED_PARAMETER( argv );

    // Stop all segments
    CSegmentItem *pSegmentItem = (CSegmentItem *)g_xbApp.m_lstSegments.GetHead();
    while( pSegmentItem )
    {
        if( pSegmentItem->m_pSegmentState8 )
        {
            g_pPerformance->StopEx( pSegmentItem->m_pSegmentState8, 0, 0 );
            pSegmentItem->m_pSegmentState8->Release();
            pSegmentItem->m_pSegmentState8 = NULL;
        }
        pSegmentItem = pSegmentItem->GetNext();
    }

    // Stop the performance
    HRESULT hr = g_pPerformance->StopEx( 0, 0, 0 );

    // Pass the result back to the Windows application
    SendNotification( "!PanicResult ", hr, 0xFFFFFFFF );
}

//-----------------------------------------------------------------------------
// Name: RCmdCreateAudiopath
// Desc: Loads and instantiates an audiopath
//-----------------------------------------------------------------------------
void RCmdCreateAudiopath(int argc, char *argv[])
{
    HRESULT hr = S_OK;

    bool fCreateStandard = false;
    // Check if we should create a standard audiopath
    if(dbgstrcmpn("createStandardAudiopath", argv[0], dbgstrlen("createStandardAudiopath")))
    {
        fCreateStandard = true;
    }

    // Check our arguments
    if( fCreateStandard )
    {
        if( argc < 4 )
        {
            DCCMDPrintf("Need to specify the ID, type, and # of PChannels\n");
            hr = E_INVALIDARG;
        }
    }
    else
    {
        if( argc < 3 )
        {
            DCCMDPrintf("Need to specify the ID and filename\n");
            hr = E_INVALIDARG;
        }
    }

    DWORD dwAudiopathID = 0xFFFFFFFF;
    CAudiopathItem *pAudiopathItem = NULL;
    
    if( SUCCEEDED(hr) )
    {
        dwAudiopathID = atoi(argv[1]);

        // Ensure this audiopath ID doesn't exist
        pAudiopathItem = (CAudiopathItem *)g_xbApp.m_lstAudiopaths.GetHead();
        while( pAudiopathItem )
        {
            if( dwAudiopathID == pAudiopathItem->m_dwAudiopathID )
            {
                OutputDebugString( "RCmdCreateAudiopath: Audiopath ID already in use\n" );
                hr = E_INVALIDARG;
                break;
            }
            pAudiopathItem = pAudiopathItem->GetNext();
        }
    }

    // Initialize variables
    IUnknown *punkAudiopathConfig = NULL;
    IDirectMusicAudioPath8 *pIDirectMusicAudioPath8 = NULL;

    // Check if we should create a standard audiopath
    if(fCreateStandard)
    {
        // Create standard audiopath
        if( SUCCEEDED(hr) )
        {
            DWORD dwType = atoi(argv[2]);
            DWORD dwPChannels = atoi(argv[3]);
            hr = g_pPerformance->CreateStandardAudioPath( dwType, dwPChannels, TRUE, &pIDirectMusicAudioPath8 );
        }
    }
    else
    {
        // Load audiopath config
        if( SUCCEEDED(hr) )
        {
            hr = g_pLoader->LoadObjectFromFile( CLSID_DirectMusicAudioPathConfig, IID_IUnknown,
                                                argv[2], (VOID**)&punkAudiopathConfig );
        }
 
        // Create the audiopath
        if( SUCCEEDED( hr ) )
        {
            hr = g_pPerformance->CreateAudioPath( punkAudiopathConfig, TRUE, &pIDirectMusicAudioPath8 );
        }
    }

    if( SUCCEEDED( hr ) )
    {
        // Add audiopath to list
        pAudiopathItem = new CAudiopathItem;
        pAudiopathItem->m_dwAudiopathID = dwAudiopathID;
        pAudiopathItem->m_pAudiopath8 = pIDirectMusicAudioPath8;
        pAudiopathItem->m_pAudiopath8->AddRef();
        g_xbApp.m_lstAudiopaths.AddHead( pAudiopathItem );
    }

    // Release audiopath config
    if( punkAudiopathConfig )
    {
        punkAudiopathConfig->Release();
    }

    // Release audiopath
    if( pIDirectMusicAudioPath8 )
    {
        pIDirectMusicAudioPath8->Release();
    }

    // Pass the result back to the Windows application
    SendNotification( "!CreateResult ", dwAudiopathID, hr );
}

//-----------------------------------------------------------------------------
// Name: RCmdSetDefaultAudiopath
// Desc: Sets the default audiopath
//-----------------------------------------------------------------------------
void RCmdSetDefaultAudiopath(int argc, char *argv[])
{
    HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify an audiopath ID\n");
        hr = E_INVALIDARG;
    }

    DWORD dwAudiopathID = 0xFFFFFFFF;
    CAudiopathItem *pAudiopathItem = NULL;

    if( SUCCEEDED(hr) )
    {
        dwAudiopathID = atoi(argv[1]);

        // Ensure this audiopath exists
        CAudiopathItem *pAudiopathTmp = (CAudiopathItem *)g_xbApp.m_lstAudiopaths.GetHead();
        while( pAudiopathTmp )
        {
            if( dwAudiopathID == pAudiopathTmp->m_dwAudiopathID )
            {
                pAudiopathItem = pAudiopathTmp;
                break;
            }
            pAudiopathTmp = pAudiopathTmp->GetNext();
        }
    }

    if( NULL == pAudiopathItem )
    {
        OutputDebugString( "RCmdSetDefaultAudiopath: Audiopath ID not found\n" );
        hr = E_INVALIDARG;
    }

    // Set as default audiopath
    if( SUCCEEDED(hr) )
    {
        hr = g_pPerformance->SetDefaultAudioPath( pAudiopathItem->m_pAudiopath8 );
    }

    // Get the default port
    if( SUCCEEDED(hr) )
    {
/*      if( g_pIDirectMusicPort )
        {
            g_pIDirectMusicPort->Release();
            g_pIDirectMusicPort = NULL;
        }*/

/*      pAudiopathItem->m_pAudiopath8->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0,
            GUID_All_Objects, 0, IID_IDirectMusicPort, (void **)&g_pIDirectMusicPort );*/
    }

    // Pass the result back to the Windows application
    SendNotification( "!SetDefaultResult ", dwAudiopathID, hr );
}

//-----------------------------------------------------------------------------
// Name: RCmdReleaseAudiopath
// Desc: releases an audiopath
//-----------------------------------------------------------------------------
void RCmdReleaseAudiopath(int argc, char *argv[])
{
    HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify the ID\n");
        hr = E_INVALIDARG;
    }

    DWORD dwAudiopathID = 0xFFFFFFFF;
    CAudiopathItem *pAudiopathItem = NULL;
    if( SUCCEEDED(hr) )
    {
        dwAudiopathID = atoi(argv[1]);

        // Ensure this audiopath ID exists
        CAudiopathItem *pAudiopathTemp = (CAudiopathItem *)g_xbApp.m_lstAudiopaths.GetHead();
        while( pAudiopathTemp )
        {
            if( dwAudiopathID == pAudiopathTemp->m_dwAudiopathID )
            {
                pAudiopathItem = pAudiopathTemp;
                break;
            }
            pAudiopathTemp = pAudiopathTemp->GetNext();
        }
    }

    if( NULL == pAudiopathItem )
    {
        OutputDebugString( "RCmdReleaseAudiopath: Audiopath ID not found\n" );
        hr = E_INVALIDARG;
    }

    // Remove the audiopath as the default audiopath, if necessary
    if( SUCCEEDED(hr) )
    {
        IDirectMusicAudioPath8 *pIDirectMusicAudioPath8 = NULL;
        g_pPerformance->GetDefaultAudioPath( &pIDirectMusicAudioPath8 );
        if( pIDirectMusicAudioPath8 == pAudiopathItem->m_pAudiopath8 )
        {
            g_pPerformance->SetDefaultAudioPath( NULL );

/*          if( g_pIDirectMusicPort )
            {
                g_pIDirectMusicPort->Release();
                g_pIDirectMusicPort = NULL;
            }*/
        }

        if( pIDirectMusicAudioPath8 )
        {
            pIDirectMusicAudioPath8->Release();
        }

        // Release the Audiopath
        pAudiopathItem->m_pAudiopath8->Release();
        pAudiopathItem->m_pAudiopath8 = NULL;

        // Remove the item from the list
        g_xbApp.m_lstAudiopaths.Remove( pAudiopathItem );

        // Delete the item
        delete pAudiopathItem;
    }

    // Pass the result back to the Windows application
    SendNotification( "!ReleaseResult ", dwAudiopathID, hr );
}

