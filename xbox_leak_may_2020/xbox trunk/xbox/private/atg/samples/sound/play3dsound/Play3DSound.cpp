//-----------------------------------------------------------------------------
// File: Play3DSound.cpp
//
// Desc: This sample demonstrates how to use the 3D capabilities of 
//       DirectSound, moving a sound source and listener in 3D.
//
// Hist: 12.15.00 - New for December XDK release
//       03.09.01 - Rewritten for April XDK release
//       01.21.02 - Added Listener orientation
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBSound.h>
#include <XBHelp.h>
#include "dsound.h"
#include "dsstdfx.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle sound" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Change sound" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle source/\nlistener" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle Headphones" },
    { XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Increase volume" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Decrease volume" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Move object in Y" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Move object in X/Z" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Change wall\nmaterials" },
    { XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_2, L"Triggers:\nChange Environment" },
};

#define NUM_HELP_CALLOUTS 11



//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------

struct D3DVERTEX
{
    D3DXVECTOR3 p;           // position
    D3DCOLOR    c;           // color
};
#define D3DFVF_D3DVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)


// Constants to define our world space
#define XMIN -10
#define XMAX 10
#define ZMIN -10
#define ZMAX 10
#define YMIN 0
#define YMAX 5

#define WALL1_XMIN ( XMIN )
#define WALL1_XMAX ( XMAX )
#define WALL1_ZMIN ( ZMAX - 0.12f * ( ZMAX - ZMIN ) )
#define WALL1_ZMAX ( ZMAX - 0.10f * ( ZMAX - ZMIN ) )

#define WALL2_XMIN ( XMIN + 0.25f * ( XMAX - XMIN ) )
#define WALL2_XMAX ( XMIN + 0.75f * ( XMAX - XMIN ) )
#define WALL2_ZMIN ( ZMAX - 0.42f * ( ZMAX - ZMIN ) )
#define WALL2_ZMAX ( ZMAX - 0.40f * ( ZMAX - ZMIN ) )

// Constants for colors
#define SOURCE_COLOR 0xFFFF0000
#define LISTENER_COLOR 0xFF0000FF

// Constants for scaling input
#define MOTION_SCALE 10.0f
#define VOLUME_SCALE  5.0f

// List of wav files to cycle through
char * g_aszFileNames[] = 
{
    "D:\\Media\\Sounds\\heli.wav",
    "D:\\Media\\Sounds\\DockingMono.wav",
    "D:\\Media\\Sounds\\EngineStartMono.wav",
    "D:\\Media\\Sounds\\MaleDialog1.wav",
    "D:\\Media\\Sounds\\MiningMono.wav",
    "D:\\Media\\Sounds\\MusicMono.wav",
    "D:\\Media\\Sounds\\Dolphin4.wav",
};
static const DWORD NUM_SOUNDS = sizeof( g_aszFileNames ) / sizeof( g_aszFileNames[0] );

typedef struct
{
    WCHAR *szName;
    LONG lHFLevel;
    FLOAT flLFRatio;
} I3DL2MATERIAL;

I3DL2MATERIAL g_aMaterials[] =
{
    { L"None",           0,    0.0f  },
    { L"Single Window", -2800, 0.71f },
    { L"Double Window", -5000, 0.40f },
    { L"Thin Door",     -1800, 0.66f },
    { L"Thick Door",    -4400, 0.64f },
    { L"Wood Wall",     -4000, 0.50f },
    { L"Brick Wall",    -5000, 0.60f },
    { L"Stone Wall",    -6000, 0.68f },
    { L"Curtain",       -1200, 0.15f },
};
static const DWORD NUM_MATERIALS = sizeof( g_aMaterials ) / sizeof( g_aMaterials[0] );

typedef struct
{
    WCHAR *         szName;
    DSI3DL2LISTENER ds3dl;
} I3DL2ENVIRONMENT;
    
I3DL2ENVIRONMENT g_aEnvironments[] =
{
    { L"Default",           { DSI3DL2_ENVIRONMENT_PRESET_DEFAULT }        },
    { L"Generic",           { DSI3DL2_ENVIRONMENT_PRESET_GENERIC }        },
    { L"Padded Cell",       { DSI3DL2_ENVIRONMENT_PRESET_PADDEDCELL }     },
    { L"Room",              { DSI3DL2_ENVIRONMENT_PRESET_ROOM }           },
    { L"Bathroom",          { DSI3DL2_ENVIRONMENT_PRESET_BATHROOM }       },
    { L"Living Room",       { DSI3DL2_ENVIRONMENT_PRESET_LIVINGROOM }     },
    { L"Stone Room",        { DSI3DL2_ENVIRONMENT_PRESET_STONEROOM }      },
    { L"Auditorium",        { DSI3DL2_ENVIRONMENT_PRESET_AUDITORIUM }     },
    { L"Concert Hall",      { DSI3DL2_ENVIRONMENT_PRESET_CONCERTHALL }    },
    { L"Cave",              { DSI3DL2_ENVIRONMENT_PRESET_CAVE }           },
    { L"Arena",             { DSI3DL2_ENVIRONMENT_PRESET_ARENA }          },
    { L"Hangar",            { DSI3DL2_ENVIRONMENT_PRESET_HANGAR }         },
    { L"Carpeted Hallway",  { DSI3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY }},
    { L"Hallway",           { DSI3DL2_ENVIRONMENT_PRESET_HALLWAY }        },
    { L"Stone Corridor",    { DSI3DL2_ENVIRONMENT_PRESET_STONECORRIDOR }  },
    { L"Alley",             { DSI3DL2_ENVIRONMENT_PRESET_ALLEY }          },
    { L"Forest",            { DSI3DL2_ENVIRONMENT_PRESET_FOREST }         },
    { L"City",              { DSI3DL2_ENVIRONMENT_PRESET_CITY }           },
    { L"Mountains",         { DSI3DL2_ENVIRONMENT_PRESET_MOUNTAINS }      },
    { L"Quarry",            { DSI3DL2_ENVIRONMENT_PRESET_QUARRY }         },
    { L"Plain",             { DSI3DL2_ENVIRONMENT_PRESET_PLAIN }          },
    { L"Parking Lot",       { DSI3DL2_ENVIRONMENT_PRESET_PARKINGLOT }     },
    { L"Sewer Pipe",        { DSI3DL2_ENVIRONMENT_PRESET_SEWERPIPE }      },
    { L"Underwater",        { DSI3DL2_ENVIRONMENT_PRESET_UNDERWATER }     },
};
static const DWORD NUM_ENVIRONMENTS = sizeof( g_aEnvironments ) / sizeof( g_aEnvironments[0] );

static const FLOAT DOPPLER_FACTOR = 2.0f;


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                 m_Font;                 // Font object
    CXBHelp                 m_Help;                 // Help object

    // Sound members
    CWaveFile               m_awfSounds[NUM_SOUNDS];// Wave file parsers
    DWORD                   m_dwCurrent;            // Current sound
    BOOL                    m_bPlaying;             // Are we playing?
    LONG                    m_lVolume;              // Current volume
    LPDIRECTSOUND8          m_pDSound;              // DirectSound object
    LPDIRECTSOUNDBUFFER8    m_pDSBuffer;            // DirectSoundBuffer
    BYTE *                  m_pbSampleData;         // Sample data from wav
    BOOL                    m_bHeadphones;          // True if headphones enabled

    // Sound source and listener positions
    D3DXVECTOR3             m_vSourcePosition;      // Source position vector
    D3DXVECTOR3             m_vListenerPosition;    // Listener position vector
    FLOAT                   m_fListenerAngle;       // Listener orientation angle in x-z
    D3DXVECTOR3             m_vListenerOrientationTop; // Listener top orientation vector

    // Transform matrices
    D3DXMATRIX              m_matWorld;             // World transform
    D3DXMATRIX              m_matView;              // View transform
    D3DXMATRIX              m_matProj;              // Projection transform

    // Models for floor, source, and listener
    LPDIRECT3DVERTEXBUFFER8 m_pvbFloor;             // Quad for the floor
    LPDIRECT3DVERTEXBUFFER8 m_pvbSource;            // Quad for the source
    LPDIRECT3DVERTEXBUFFER8 m_pvbListener;          // Quad for the listener
    LPDIRECT3DVERTEXBUFFER8 m_pvbGrid;              // Lines to grid the floor
    LPDIRECT3DVERTEXBUFFER8 m_pvbWall1;             // 1st wall
    LPDIRECT3DVERTEXBUFFER8 m_pvbWall2;             // 2nd wall

    D3DCOLOR        m_cSource;                      // Color for sound source
    D3DCOLOR        m_cListener;                    // Color for listener

    BOOL            m_bDrawHelp;                    // Should we draw help?
    BOOL            m_bControlSource;               // Control source (TRUE) or
                                                    // listener (FALSE)

    DWORD           m_dwOcclusionMaterial;          // Occlusion material
    DWORD           m_dwObstructionMaterial;        // Obstruction material
    FLOAT           m_fObstructFactor;              // Obstruction factor
    DWORD           m_dwEnvironment;                // Environment

    HRESULT SwitchToSound( DWORD dwIndex );         // Sets up a different sound
    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP
    FLOAT CalculateObstructionFactor();             // Calculates obstruction factor

public:
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    CXBoxSample();
};




//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_bDrawHelp = FALSE;

    // Sounds
    m_lVolume = DSBVOLUME_MAX;
    m_pbSampleData = NULL;
    m_bHeadphones = FALSE;

    // Positions
    m_vSourcePosition   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vListenerPosition = D3DXVECTOR3( 0.0f, 0.0f, ZMIN );

    // listener default orientation
    m_fListenerAngle = 0.0f;
    m_vListenerOrientationTop   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    m_dwOcclusionMaterial   = 3;    // Thin door
    m_dwObstructionMaterial = 6;    // Brick wall
    m_dwEnvironment         = 0;
}




//-----------------------------------------------------------------------------
// Name: DownloadEffectsImage
// Desc: Downloads an effects image to the DSP
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DownloadEffectsImage(PCHAR pszScratchFile)
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
        EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
        EffectLoc.dwCrosstalkIndex = I3DL2_CHAIN_XTALK;

        hr = m_pDSound->DownloadEffectsImage( pBuffer,
                                              dwSize,
                                              &EffectLoc,
                                              &pDesc );
    }

    delete[] pBuffer;

    if( hFile != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hFile );
    }
    
    return hr;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    int i, j;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create DirectSound
    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    // There are 2 options for 3-D sound processing:
    // 1) DirectSoundUseFullHRTF - full hardware HRTF-based processing
    // 2) DirectSoundUseLightHRTF - hardware HRTF processing, but without
    //      any vertical component (azimuth only).  Saves ~60k of memory
    DirectSoundUseFullHRTF();

    // Exaggerate the doppler effect for demonstration purposes.
    m_pDSound->SetDopplerFactor( DOPPLER_FACTOR, DS3D_IMMEDIATE );
    
    // download the standard DirectSound effecs image
    if( FAILED( DownloadEffectsImage("d:\\media\\dsstdfx.bin") ) )
        return E_FAIL;

    //
    // For testing purposes:
    // Make sure that all our files have the same format,
    // since we're re-using the samd DirectSoundBuffer for
    // all the files.  First, we have to determine how
    // much space to allocate for the wave format, since
    // the XBOXADPCMWAVEFORMAT is 2 bytes larger than
    // WAVEFORMATEX.
    //
    DWORD          cbFirst;
    WAVEFORMATEX * pwfFirst = NULL;
    DWORD          cbWalk;
    WAVEFORMATEX * pwfWalk = NULL;
    for( i = 0; i < NUM_SOUNDS; i++ )
    {
        if( FAILED( m_awfSounds[ i ].Open( g_aszFileNames[i] ) ) )
            return XBAPPERR_MEDIANOTFOUND;

        // Testing only:
        // Verify they're all the same format, since we're only going to have
        // one sound buffer.
        if( i == 0 )
        {
            // Allocate space for the wave format
            m_awfSounds[ i ].GetFormat( NULL, 0, &cbFirst );
            pwfFirst = (WAVEFORMATEX *)new BYTE[ cbFirst ];

            if( FAILED( m_awfSounds[ i ].GetFormat( pwfFirst, cbFirst ) ) )
                return E_FAIL;
        }
        else
        {
            // Allocate space for the wave format
            m_awfSounds[ i ].GetFormat( NULL, 0, &cbWalk );
            pwfWalk = (WAVEFORMATEX *)new BYTE[ cbWalk ];

            if( FAILED( m_awfSounds[ i ].GetFormat( pwfWalk, cbWalk ) ) )
                return E_FAIL;

            if( cbWalk != cbFirst ||
                memcmp( pwfFirst, pwfWalk, cbFirst ) )
            {
                OUTPUT_DEBUG_STRING( "All wav files should have the same format!\n" );
                return E_FAIL;
            }

            delete[] pwfWalk;
        }
    }

    // Create a sound buffer of 0 size, since we're going to use
    // SetBufferData
    DSBUFFERDESC dsbdesc;
    ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
    dsbdesc.dwSize = sizeof( DSBUFFERDESC );

    // If fewer than 256 buffers are in existence at all points during 
    //   the game, it may be more efficient not to use LOCDEFER
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_LOCDEFER;
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat = pwfFirst;

    if( FAILED( m_pDSound->CreateSoundBuffer( &dsbdesc, &m_pDSBuffer, NULL ) ) )
        return E_FAIL;

    // Set the transform matrices
    D3DXVECTOR3 vEyePt      = D3DXVECTOR3( XMIN, 45.0f,  ZMAX / 2.0f );
    D3DXVECTOR3 vLookatPt   = D3DXVECTOR3( XMIN,  0.0f,  ZMAX / 2.0f );
    D3DXVECTOR3 vUpVec      = D3DXVECTOR3( 0.0f,  0.0f,  1.0f );
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 10000.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Create our vertex buffers
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbFloor );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbSource );
    m_pd3dDevice->CreateVertexBuffer( 3 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbListener );
    m_pd3dDevice->CreateVertexBuffer( 2 * ( ( ZMAX - ZMIN + 1 ) + ( XMAX - XMIN + 1 ) ) * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbGrid );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbWall1 );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pvbWall2 );
    
    D3DVERTEX * pVertices;

    // Fill the VB for the listener
    m_pvbListener->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR3( -0.5f, 0.0f, -1.0f ); pVertices[0].c = LISTENER_COLOR; // m_cListener;
    pVertices[1].p = D3DXVECTOR3(  0.0f, 0.0f,  1.0f ); pVertices[1].c = LISTENER_COLOR; // m_cListener;
    pVertices[2].p = D3DXVECTOR3(  0.5f, 0.0f, -1.0f ); pVertices[2].c = LISTENER_COLOR; // m_cListener;

    // Fill the VB for the source
    m_pvbSource->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR3( -0.5f, 0.0f, -0.5f ); pVertices[0].c = SOURCE_COLOR; // m_cSource;
    pVertices[1].p = D3DXVECTOR3( -0.5f, 0.0f,  0.5f ); pVertices[1].c = SOURCE_COLOR; // m_cSource;
    pVertices[2].p = D3DXVECTOR3(  0.5f, 0.0f, -0.5f ); pVertices[2].c = SOURCE_COLOR; // m_cSource;
    pVertices[3].p = D3DXVECTOR3(  0.5f, 0.0f,  0.5f ); pVertices[3].c = SOURCE_COLOR; // m_cSource

    // Fill the VB for the floor
    m_pvbFloor->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR3( XMIN, 0.0f, ZMIN ); pVertices[0].c = 0xFF101010;
    pVertices[1].p = D3DXVECTOR3( XMIN, 0.0f, ZMAX ); pVertices[1].c = 0xFF101010;
    pVertices[2].p = D3DXVECTOR3( XMAX, 0.0f, ZMIN ); pVertices[2].c = 0xFF101010;
    pVertices[3].p = D3DXVECTOR3( XMAX, 0.0f, ZMAX ); pVertices[3].c = 0xFF101010;
    m_pvbFloor->Unlock();

    // Fill the VB for the grid
    m_pvbGrid->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    for( i = ZMIN, j = 0; i <= ZMAX; i++, j++ )
    {
        pVertices[ j * 2 ].p     = D3DXVECTOR3( XMIN, 0, (FLOAT)i ); pVertices[ j * 2 ].c     = 0xFF00A000;
        pVertices[ j * 2 + 1 ].p = D3DXVECTOR3( XMAX, 0, (FLOAT)i ); pVertices[ j * 2 + 1 ].c = 0xFF00A000;
    }
    for( i = XMIN; i <= XMAX; i++, j++ )
    {
        pVertices[ j * 2 ].p     = D3DXVECTOR3( (FLOAT)i, 0, ZMIN ); pVertices[ j * 2 ].c     = 0xFF00A000;
        pVertices[ j * 2 + 1 ].p = D3DXVECTOR3( (FLOAT)i, 0, ZMAX ); pVertices[ j * 2 + 1 ].c = 0xFF00A000;
    }
    m_pvbGrid->Unlock();

    // Set up first (full width) wall
    m_pvbWall1->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR3( WALL1_XMIN, 0.0f, WALL1_ZMIN ); pVertices[0].c = 0xFF808080;
    pVertices[1].p = D3DXVECTOR3( WALL1_XMIN, 0.0f, WALL1_ZMAX ); pVertices[1].c = 0xFF808080;
    pVertices[2].p = D3DXVECTOR3( WALL1_XMAX, 0.0f, WALL1_ZMIN ); pVertices[2].c = 0xFF808080;
    pVertices[3].p = D3DXVECTOR3( WALL1_XMAX, 0.0f, WALL1_ZMAX ); pVertices[3].c = 0xFF808080;
    m_pvbWall1->Unlock();

    // Set up second (smaller) wall
    m_pvbWall2->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = D3DXVECTOR3( WALL2_XMIN, 0.0f, WALL2_ZMIN ); pVertices[0].c = 0xFF808080;
    pVertices[1].p = D3DXVECTOR3( WALL2_XMIN, 0.0f, WALL2_ZMAX ); pVertices[1].c = 0xFF808080;
    pVertices[2].p = D3DXVECTOR3( WALL2_XMAX, 0.0f, WALL2_ZMIN ); pVertices[2].c = 0xFF808080;
    pVertices[3].p = D3DXVECTOR3( WALL2_XMAX, 0.0f, WALL2_ZMAX ); pVertices[3].c = 0xFF808080;
    m_pvbWall2->Unlock();

    // Set up and play our initial sound
    m_dwCurrent = 0;
    SwitchToSound( m_dwCurrent );
    m_bPlaying = TRUE;
    m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    // Clean up our memory allocations
    delete[] pwfFirst;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SwitchToSound
// Desc: Switches to the given sound by:
//       1) Stop playback if we're playing
//       2) Reallocate the sample data buffer
//       3) Point the DirectSoundBuffer to the new data
//       4) Restart plyaback if needed
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::SwitchToSound( DWORD dwIndex )
{
    DWORD dwNewSize;
    DWORD dwLoopStartSample, dwLoopLengthSamples;
    DWORD cbLoopStart, cbLoopLength;

    // If we're currently playing, stop, so that we don't crash
    // when we reallocate our buffer
    if( m_bPlaying )
    {
        m_pDSBuffer->Stop();
    }

    // Calling stop doesn't immediately shut down
    // the voice, so point it away from our buffer
    m_pDSBuffer->SetBufferData( NULL, 0 );

    // Find out how big the new sample is
    m_awfSounds[ dwIndex ].GetDuration( &dwNewSize );

    // Set our allocation to that size
    if( m_pbSampleData )
        delete[] m_pbSampleData;
    m_pbSampleData = new BYTE[ dwNewSize ];
    if( !m_pbSampleData )
        return E_OUTOFMEMORY;

    // Read sample data from the file
    m_awfSounds[ dwIndex ].ReadSample( 0, m_pbSampleData, dwNewSize, &dwNewSize );

    // Check for embedded loop points
    if( SUCCEEDED( m_awfSounds[ dwIndex ].GetLoopRegion( &dwLoopStartSample, &dwLoopLengthSamples ) ) )
    {
        WAVEFORMATEX wfx;

        // We're not interested in the whole wave format, which is
        // why we can get away with just loading the core WAVEFORMATEX
        m_awfSounds[ dwIndex ].GetFormat( &wfx, sizeof( WAVEFORMATEX ) );

        // We need to convert the loop points from sample counts to
        // byte offsets, but it's slightly different between PCM and ADPCM
        if( wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM )
        {
            // For ADPCM, calculate # of blocks and multiply that
            // by bytes per block.  Xbox ADPCM is always 64 samples
            // per block.
            cbLoopStart = dwLoopStartSample / 64 * wfx.nBlockAlign;
            cbLoopLength = dwLoopLengthSamples / 64 * wfx.nBlockAlign;
        }
        else
        {
            // For PCM, multiply by bytes per sample
            DWORD cbBytesPerSample = wfx.nChannels * wfx.wBitsPerSample / 8;
            cbLoopStart = dwLoopStartSample * cbBytesPerSample;
            cbLoopLength = dwLoopLengthSamples * cbBytesPerSample;
        }
    }
    else
    {
        // Otherwise, just loop the whole file
        cbLoopStart = 0;
        cbLoopLength = dwNewSize;
    }


    // Set up values for the new buffer
    m_pDSBuffer->SetBufferData( m_pbSampleData, dwNewSize );
    m_pDSBuffer->SetLoopRegion( cbLoopStart, cbLoopLength );
    m_pDSBuffer->SetCurrentPosition( 0 );

    // If we were playing before, restart playback now
    if( m_bPlaying )
        m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CalculateObstructionFactor
// Desc: Calculates the obstruction factor for the 2nd wall.  Assumes that 
//       the second wall is actually a valid obstruction (ie, the line between
//       source and listener should cross through it).  Calculates the cos
//       of the angles made by source, wall edge, and listener on each side
//       of the wall, and returns the lesser.
//-----------------------------------------------------------------------------
FLOAT CXBoxSample::CalculateObstructionFactor()
{
    float a2, b2, c2, cosC;
    float fObLeft, fObRight;

    // We want to figure out the angle formed by the source,
    // the listener, and the edge of the wall, using law of 
    // cosines (lower case = edge, uppper case = angle):
    // c^2 = a^2 + b^2 - 2ab * cos C
    // cos C = ( c^2 - a^2 - b^2 ) / -2ab
    // We'll do this for both the left and right edges of the
    // wall, and take the smaller one.  A more accurate model
    // could be used that combined the two.
    a2 = ( m_vListenerPosition.x - WALL2_XMAX ) * ( m_vListenerPosition.x - WALL2_XMAX ) + 
         ( m_vListenerPosition.z - WALL2_ZMAX ) * ( m_vListenerPosition.z - WALL2_ZMAX );
    b2 = ( m_vSourcePosition.x - WALL2_XMAX ) * ( m_vSourcePosition.x - WALL2_XMAX ) + 
         ( m_vSourcePosition.z - WALL2_ZMAX ) * ( m_vSourcePosition.z - WALL2_ZMAX );
    c2 = ( m_vListenerPosition.x - m_vSourcePosition.x ) * ( m_vListenerPosition.x - m_vSourcePosition.x ) +
         ( m_vListenerPosition.z - m_vSourcePosition.z ) * ( m_vListenerPosition.z - m_vSourcePosition.z );

    cosC = float( ( c2 - a2 - b2 ) / ( -2.0f * sqrt( a2 ) * sqrt( b2 ) ) );

    // Smaller angle means less obstruction.
    // cos(0) = 1
    // cos(90) = 0
    // cos(180) = -1
    // Rough estimate of obstruction factor is (1 - cosC) / 2
    fObRight = ( 1.0f + cosC ) / 2.0f;

    // Now repeat for the left wall
    a2 = ( m_vListenerPosition.x - WALL2_XMIN ) * ( m_vListenerPosition.x - WALL2_XMIN ) + ( m_vListenerPosition.z - WALL2_ZMIN ) * ( m_vListenerPosition.z - WALL2_ZMIN );
    b2 = ( m_vSourcePosition.x - WALL2_XMIN ) * ( m_vSourcePosition.x - WALL2_XMIN ) + ( m_vSourcePosition.z - WALL2_ZMIN ) * ( m_vSourcePosition.z - WALL2_ZMIN );

    cosC = float( ( c2 - a2 - b2 ) / ( -2.0f * sqrt( a2 ) * sqrt( b2 ) ) );

    fObLeft = ( 1.0f + cosC ) / 2.0f;

    return ( fObLeft < fObRight ) ? fObLeft : fObRight;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DXVECTOR3     vSourceOld   = m_vSourcePosition;
    D3DXVECTOR3     vListenerOld = m_vListenerPosition;
    D3DXVECTOR3 *   pvControl;
    DWORD           dwPulse = DWORD( ( cosf( m_fAppTime * 6.0f ) + 1.0f ) * 50 );
    D3DCOLOR        cBlend = dwPulse | ( dwPulse << 8 ) | ( dwPulse << 16 );

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Increase/Decrease volume
    m_lVolume += LONG( ( m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_BLACK ] - 
                   m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_WHITE ] ) *
                   m_fElapsedTime * 
                   VOLUME_SCALE );

    // Make sure volume is in the appropriate range
    if( m_lVolume < DSBVOLUME_MIN )
        m_lVolume = DSBVOLUME_MIN;
    else if( m_lVolume > DSBVOLUME_MAX )
        m_lVolume = DSBVOLUME_MAX;

    // Toggle sound on and off
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        if( m_bPlaying )
            m_pDSBuffer->Stop( );
        else
            m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

        m_bPlaying = !m_bPlaying;
    }

    // Cycle through sounds
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        m_dwCurrent = ( m_dwCurrent + 1 ) % NUM_SOUNDS;
        SwitchToSound( m_dwCurrent );
    }

    // Switch which of source vs. listener we are moving
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_bControlSource = !m_bControlSource;
    }

    // Toggle headphones
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
    {
        m_bHeadphones = !m_bHeadphones;
        m_pDSound->EnableHeadphones( m_bHeadphones );
    }

    // Changing listener enviornment settings is expensive, so only do it when we're
    // actually changing the environment.
    BOOL bResetListener = FALSE;
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_LEFT_TRIGGER ] )
    {
        m_dwEnvironment = ( m_dwEnvironment + NUM_ENVIRONMENTS - 1 ) % NUM_ENVIRONMENTS;
        bResetListener = TRUE;
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_RIGHT_TRIGGER ] )
    {
        m_dwEnvironment = ( m_dwEnvironment + 1 ) % NUM_ENVIRONMENTS;
        bResetListener = TRUE;
    }

    if( bResetListener )
    {
        m_pDSound->SetI3DL2Listener( &g_aEnvironments[ m_dwEnvironment ].ds3dl, DS3D_DEFERRED );
    }

    // Select occlusion/obstruction materials
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
        m_dwOcclusionMaterial = ( m_dwOcclusionMaterial + 1 ) % NUM_MATERIALS;
    else if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        m_dwOcclusionMaterial = ( m_dwOcclusionMaterial + NUM_MATERIALS - 1 ) % NUM_MATERIALS;
    else if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
        m_dwObstructionMaterial = ( m_dwObstructionMaterial + 1 ) % NUM_MATERIALS;
    else if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
        m_dwObstructionMaterial = ( m_dwObstructionMaterial + NUM_MATERIALS - 1 ) % NUM_MATERIALS;

    // Set up our colors
    m_cSource   = SOURCE_COLOR   | (  m_bControlSource ? cBlend : 0 );
    m_cListener = LISTENER_COLOR | ( !m_bControlSource ? cBlend : 0 );

    // Point to the appropriate vector
    pvControl = m_bControlSource ? &m_vSourcePosition : &m_vListenerPosition;

    // Move selected object and clamp to the appropriate range
    pvControl->x += m_DefaultGamepad.fX1 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->x < XMIN )
        pvControl->x = XMIN;
    else if( pvControl->x > XMAX )
        pvControl->x = XMAX;

    pvControl->z += m_DefaultGamepad.fY1 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->z < ZMIN )
        pvControl->z = ZMIN;
    else if( pvControl->z > ZMAX )
        pvControl->z = ZMAX;

    pvControl->y += m_DefaultGamepad.fY2 * m_fElapsedTime * MOTION_SCALE;
    if( pvControl->y < YMIN )
        pvControl->y = YMIN;
    else if( pvControl->y > YMAX )
        pvControl->y = YMAX;

    // Calculate listener orientation in x-z plane
    if( m_vListenerPosition.x != vListenerOld .x ||
        m_vListenerPosition.z != vListenerOld .z )
    {
        D3DXVECTOR3 vDelta = m_vListenerPosition - vListenerOld;
        m_fListenerAngle = FLOAT( atan2( vDelta.x, vDelta.z ) );

        // set default listener orientation
        m_pDSound->SetOrientation( vDelta.x,
                                   0.0f,
                                   vDelta.z,
                                   m_vListenerOrientationTop.x,
                                   m_vListenerOrientationTop.y,
                                   m_vListenerOrientationTop.z,
                                   DS3D_DEFERRED );
    }

    // Position the sound and listener in 3D. 
    // We use DS3D_DEFERRED so that all the changes will 
    // be commited at once.
    D3DXVECTOR3 vListenerVelocity = ( m_vListenerPosition - vListenerOld ) / m_fElapsedTime;
    D3DXVECTOR3 vSoundVelocity = ( m_vSourcePosition - vSourceOld ) / m_fElapsedTime;

    // Source position/velocity/volume
    m_pDSBuffer->SetPosition( m_vSourcePosition.x, m_vSourcePosition.y, m_vSourcePosition.z, DS3D_DEFERRED );
    m_pDSBuffer->SetVelocity( vSoundVelocity.x, vSoundVelocity.y, vSoundVelocity.z, DS3D_DEFERRED );
    m_pDSBuffer->SetVolume( m_lVolume );

    // Listener position/velocity
    m_pDSound->SetPosition( m_vListenerPosition.x, m_vListenerPosition.y, m_vListenerPosition.z, DS3D_DEFERRED  );
    m_pDSound->SetVelocity( vListenerVelocity.x, vListenerVelocity.y, vListenerVelocity.z, DS3D_DEFERRED );

    // Calculate I3DL2 effects:
    BOOL bOccluded = FALSE;
    BOOL bObstructed = FALSE;

    // Occlusion - see if source and listener are on opposite sides of wall 1
    if( ( m_vSourcePosition.z > WALL1_ZMAX &&
          m_vListenerPosition.z < WALL1_ZMIN ) ||
        ( m_vSourcePosition.z < WALL1_ZMIN &&
          m_vListenerPosition.z > WALL1_ZMAX ) )
    {
        bOccluded = TRUE;
    }
    
    // Obstruction - calculate if line between source and listener
    // hits obstruction wall 2.  Calculate z-intercept value, and
    // then find x-value at that intercept.  If it's in the range 
    // of the wall, then we're obstructed
    float fGamma = ( WALL2_ZMAX - m_vSourcePosition.z ) / ( m_vListenerPosition.z - m_vSourcePosition.z );
    if( fGamma >= 0.0f && fGamma <= 1.0f )
    {
        float fxint = m_vSourcePosition.x + fGamma * ( m_vListenerPosition.x - m_vSourcePosition.x );
        if( fxint > WALL2_XMIN && fxint < WALL2_XMAX )
            bObstructed = TRUE;
    }

    //
    // Set up I3DL2BUFFER parameters
    //
    DSI3DL2BUFFER ds3db = {0};

    // Calculate obstruction factor based on angle between 
    // source, wall, and listener
    if( bObstructed )
        m_fObstructFactor = CalculateObstructionFactor();
    else
        m_fObstructFactor = 0.0f;

    // Calculate obstruction values:
    I3DL2MATERIAL mat = g_aMaterials[ m_dwObstructionMaterial ];
    ds3db.Obstruction.lHFLevel = LONG( m_fObstructFactor * mat.lHFLevel );
    ds3db.Obstruction.flLFRatio = m_fObstructFactor * mat.flLFRatio;

    // Set Occlusion values
    if( bOccluded )
    {
        ds3db.Occlusion.lHFLevel = g_aMaterials[ m_dwOcclusionMaterial ].lHFLevel;
        ds3db.Occlusion.flLFRatio = g_aMaterials[ m_dwOcclusionMaterial ].flLFRatio;
    }

    // Bring the reverb down by 6dB
    ds3db.lRoom = -600;

    m_pDSBuffer->SetI3DL2Source( &ds3db, DS3D_DEFERRED );

    // Commit position/velocity changes
    m_pDSound->CommitDeferredSettings();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    //
    // call dsound to do some work of low priority queued items such
    // as notifications, stream packets completions, etc
    //
    DirectSoundDoWork();

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0xFF000000, 1.0f, 0L );

    RenderGradientBackground( 0xFF408040, 0xFF404040 );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetVertexShader( D3DFVF_D3DVERTEX );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    // Draw the floor
    m_pd3dDevice->SetStreamSource( 0, m_pvbFloor, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Draw the grid
    m_pd3dDevice->SetStreamSource( 0, m_pvbGrid, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 2 * ( ( ZMAX - ZMIN + 1 ) + ( XMAX - XMIN + 1 ) ) );

    // Draw the first wall
    if( m_dwOcclusionMaterial != 0 )
    {
        m_pd3dDevice->SetStreamSource( 0, m_pvbWall1, sizeof( D3DVERTEX ) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }

    // Draw the second wall
    if( m_dwObstructionMaterial != 0 )
    {
        m_pd3dDevice->SetStreamSource( 0, m_pvbWall2, sizeof( D3DVERTEX ) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }

    // Draw the source
    {
        D3DXMATRIX matSource;
        D3DXMatrixTranslation( &matSource, 
                               m_vSourcePosition.x,
                               m_vSourcePosition.y,
                               m_vSourcePosition.z );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matSource );
        m_pd3dDevice->SetStreamSource( 0, m_pvbSource, sizeof( D3DVERTEX ) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }

    // Draw the listener
    {
        D3DXMATRIX matListener;
        D3DXMATRIX mat;

        D3DXMatrixTranslation( &matListener, m_vListenerPosition.x, m_vListenerPosition.y, m_vListenerPosition.z );
        D3DXMatrixRotationY( &mat, m_fListenerAngle );
        D3DXMatrixMultiply( &matListener, &mat, &matListener );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matListener );
        m_pd3dDevice->SetStreamSource( 0, m_pvbListener, sizeof( D3DVERTEX ) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR szBuff[200];

		m_Font.Begin();
        // Show frame rate
        m_Font.DrawText(  64, 50, 0xffffffff, L"Play3DSound" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Show status
        swprintf( szBuff, L"Current Sound: %S", g_aszFileNames[ m_dwCurrent ] );
        m_Font.DrawText( 64, 100, m_bPlaying ? 0xFFFFFFFF : 0xFF404040, szBuff );
        swprintf( szBuff, L"Source: <%0.1f, %0.1f, %0.1f>", m_vSourcePosition.x, m_vSourcePosition.y, m_vSourcePosition.z );
        m_Font.DrawText( 64, 130, m_cSource, szBuff );
        swprintf( szBuff, L"Listener: <%0.1f, %0.1f, %0.1f>", m_vListenerPosition.x, m_vListenerPosition.y, m_vListenerPosition.z );
        m_Font.DrawText( 64, 160, m_cListener, szBuff );
        swprintf( szBuff, L"Volume: %ddB (%0.0f%%)", m_lVolume / 100, 100.0f * ( m_lVolume - DSBVOLUME_MIN ) / (DSBVOLUME_MAX - DSBVOLUME_MIN ) );
        m_Font.DrawText( 64, 190, 0xFFFFFF00, szBuff );
        swprintf( szBuff, L"Occluder: %s", g_aMaterials[ m_dwOcclusionMaterial ].szName );
        m_Font.DrawText( 64, 220, 0xFFFFFFFF, szBuff );
        swprintf( szBuff, L"Obstructor: %s", g_aMaterials[ m_dwObstructionMaterial ].szName );
        m_Font.DrawText( 64, 250, 0xFFFFFFFF, szBuff );
        swprintf( szBuff, L"Obstruction Factor: %0.2f", m_fObstructFactor );
        m_Font.DrawText( 64, 280, 0xFFFFFFFF, szBuff );
        swprintf( szBuff, L"Environment: %s", g_aEnvironments[ m_dwEnvironment ].szName );
        m_Font.DrawText( 64, 310, 0xFFFFFFFF, szBuff );
        if( m_bHeadphones )
            m_Font.DrawText( 64, 340, 0xFFFFFF00, L"Headphones enabled");
        else
            m_Font.DrawText( 64, 340, 0x80808000, L"Headphones disabled");

		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




