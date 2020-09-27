//-----------------------------------------------------------------------------
// File: VideoTest.cpp
//
// Desc: This sample is a standalone program used to do basic calibration on a
//       TV. It supplies a set of on-screen instructions to ensure that the
//       TV is set to optimimum output settings.
//
// Hist: 04.14.00 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <DSound.h>
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBSound.h>
#include "Resource.h"




//-----------------------------------------------------------------------------
// Structures and Macros
//-----------------------------------------------------------------------------
struct SCREENVERTEX
{
    D3DXVECTOR4 pos;   // The transformed position for the vertex point.
    DWORD       color; // The vertex color. 
};
#define D3DFVF_SCREENVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


struct SCREENVERTEX_TEX
{
    FLOAT x, y, z, rhw;
    FLOAT tu, tv;
};
#define D3DFVF_SCREENVERTEX_TEX (D3DFVF_XYZRHW|D3DFVF_TEX1)


struct D3DVERTEX
{
    D3DXVECTOR3 p;           // position
    D3DCOLOR    c;           // color
};
#define D3DFVF_D3DVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)


struct D3DVERTEX_TEX
{
    D3DXVECTOR3 p;           // position
    D3DCOLOR    c;           // color
    FLOAT       tu, tv;     // texture
};
#define D3DFVF_D3DVERTEX_TEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)


enum State
{
   START,
   STAGE1,
   STAGE2,
   STAGE3,
   STAGE4,
   ENDSTAGE
};

// Constants to define our world space
#define XMIN -10
#define XMAX 10
#define ZMIN -10
#define ZMAX 10
#define YMIN 0
#define YMAX 5


// Some colors
#define SEMITRANS_BLACK 0x40000000
#define BLACK           0xff000000
#define WHITE           0xffffffff
#define YELLOW          0xffffff00
#define RED             0xffff0000
#define DARK_RED        0xff500000
#define DARK_GREEN      0xff008000




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Packed resources
    CXBPackedResource       m_xprResource;

    // Font
    CXBFont                 m_Font;

    // Current display state
    enum State              m_eCurrentState;

    // Track last button pressed
    INT                     m_iLastButton;

    // Geometry
    LPDIRECT3DVERTEXBUFFER8 m_pTestPatternVB; // Buffer to hold vertices for test pattern
    LPDIRECT3DTEXTURE8      m_pTestPatternTexture;
    
    // Geometry for floor, source, and listener
    LPDIRECT3DVERTEXBUFFER8 m_pFloorVB;             // Quad for the floor
    LPDIRECT3DVERTEXBUFFER8 m_pSourceVB;            // Quad for the source
    LPDIRECT3DVERTEXBUFFER8 m_pListenerVB;          // Quad for the listener
    LPDIRECT3DVERTEXBUFFER8 m_pGridVB;              // Lines to grid the floor
    LPDIRECT3DTEXTURE8      m_pSpeakerTexture;

    // Sound variables
    CWaveFile               m_wfSound;              // Wave file
    LONG                    m_lVolume;              // Current volume
    LPDIRECTSOUND8          m_pDSound;              // DirectSound object
    LPDIRECTSOUNDBUFFER8    m_pDSBuffer;            // DirectSoundBuffer
    BYTE*                   m_pSampleData;          // Sample data from wav

    // Sound source and listener positions
    D3DXVECTOR3             m_vSourcePosition;      // Source position vector
    D3DXVECTOR3             m_vListenerPosition;    // Listener position vector

    // Transform matrices
    D3DXMATRIX              m_matWorld;             // World transform
    D3DXMATRIX              m_matView;              // View transform
    D3DXMATRIX              m_matProj;              // Projection transform

    // Drawing functions
    VOID    DrawLine( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD dwStartColor, DWORD dwEndColor );
    VOID    DrawRectOutline( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD dwColor );
    VOID    DrawRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD dwStartColor, DWORD dwEndColor );
    VOID    DrawTextBox( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, WCHAR* strText );
    VOID    DrawTestPattern();
    VOID    DrawStartText();
    VOID    DrawStage1Text();
    VOID    DrawStage2Text();
    VOID    DrawStage3Text();
    VOID    DrawStage4Text();

public:
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

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
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_iLastButton       = XINPUT_GAMEPAD_A;
    m_eCurrentState     = START;

    m_pTestPatternVB      = NULL;
    m_pTestPatternTexture = NULL;

    m_pFloorVB          = NULL;
    m_pSourceVB         = NULL;
    m_pListenerVB       = NULL;
    m_pGridVB           = NULL;
    m_pSpeakerTexture   = NULL;

    m_lVolume           = DSBVOLUME_MAX;
    m_pSampleData       = NULL;
    m_vSourcePosition   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vListenerPosition = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    WAVEFORMATEX    wfSoundFormat;
    DWORD           dwSoundSampleSize;
    VOID*           pvBuffer;
    int i,j;

    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return E_FAIL;

    // Load the packed resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
        return E_FAIL;
    m_pTestPatternTexture = m_xprResource.GetTexture( resource_TestPattern_OFFSET );
    m_pSpeakerTexture     = m_xprResource.GetTexture( resource_Speaker_OFFSET );

    // Initialize vertices for rendering a square
    SCREENVERTEX_TEX g_TestPatternVertices[] =
    {
        {   0 - 0.5f, 480 - 0.5f, 0.0f, 1.0f,    0, 480 },
        {   0 - 0.5f,   0 - 0.5f, 0.0f, 1.0f,    0,   0 },
        { 640 - 0.5f, 480 - 0.5f, 0.0f, 1.0f,  640, 480 },

        { 640 - 0.5f, 480 - 0.5f, 0.0f, 1.0f,  640, 480 },
        {   0 - 0.5f,   0 - 0.5f, 0.0f, 1.0f,    0,   0 },
        { 640 - 0.5f,   0 - 0.5f, 0.0f, 1.0f,  640,   0 },
    };

    // Create the vertex buffer.
    m_pd3dDevice->CreateVertexBuffer( 6*sizeof(SCREENVERTEX_TEX),
                                      D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, 
                                      &m_pTestPatternVB );

    // Fill the vertex buffer.
    VOID* pVertices;
    m_pTestPatternVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
    memcpy( pVertices, g_TestPatternVertices, sizeof(g_TestPatternVertices) );
    m_pTestPatternVB->Unlock();

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
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pFloorVB );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX_TEX ), 0, 0, 0, &m_pSourceVB );
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( D3DVERTEX ), 0, 0, 0, &m_pListenerVB );
    m_pd3dDevice->CreateVertexBuffer( 2 * ( ( ZMAX - ZMIN + 1 ) + ( XMAX - XMIN + 1 ) ) * sizeof( D3DVERTEX ), 0, 0, 0, &m_pGridVB );
    
    // Fill the VB for the floor
    D3DVERTEX* pSoundVertices;
    m_pFloorVB->Lock( 0, 0, (BYTE **)&pSoundVertices, 0 );
    pSoundVertices[0].p = D3DXVECTOR3( XMIN, 0.0f, ZMIN ); pSoundVertices[0].c = 0xFF101010;
    pSoundVertices[1].p = D3DXVECTOR3( XMIN, 0.0f, ZMAX ); pSoundVertices[1].c = 0xFF101010;
    pSoundVertices[2].p = D3DXVECTOR3( XMAX, 0.0f, ZMIN ); pSoundVertices[2].c = 0xFF101010;
    pSoundVertices[3].p = D3DXVECTOR3( XMAX, 0.0f, ZMAX ); pSoundVertices[3].c = 0xFF101010;
    m_pFloorVB->Unlock();

    // Fill the VB for the grid
    m_pGridVB->Lock( 0, 0, (BYTE **)&pSoundVertices, 0 );
    for( i = ZMIN, j = 0; i <= ZMAX; i++, j++ )
    {
        pSoundVertices[ j * 2 ].p     = D3DXVECTOR3( XMIN, 0, (FLOAT)i ); pSoundVertices[ j * 2 ].c     = 0xFF00A000;
        pSoundVertices[ j * 2 + 1 ].p = D3DXVECTOR3( XMAX, 0, (FLOAT)i ); pSoundVertices[ j * 2 + 1 ].c = 0xFF00A000;
    }
    for( i = XMIN; i <= XMAX; i++, j++ )
    {
        pSoundVertices[ j * 2 ].p     = D3DXVECTOR3( (FLOAT)i, 0, ZMIN ); pSoundVertices[ j * 2 ].c     = 0xFF00A000;
        pSoundVertices[ j * 2 + 1 ].p = D3DXVECTOR3( (FLOAT)i, 0, ZMAX ); pSoundVertices[ j * 2 + 1 ].c = 0xFF00A000;
    }
    m_pGridVB->Unlock();

    // Create DirectSound interface to the MCP
    if( FAILED( DirectSoundCreate( DSDEVID_MCPX, &m_pDSound, NULL ) ) )
        return E_FAIL;

    // Load up a wave file
    if( FAILED( m_wfSound.Open( "D:\\Media\\Sounds\\TestTone.wav" ) ) )
        return E_FAIL;

    // Check that it is a mono wav as we are going to use it for 3D positioning
    if( FAILED( m_wfSound.GetFormat( &wfSoundFormat, sizeof( WAVEFORMATEX ) ) ) )
        return E_FAIL;

    if( wfSoundFormat.nChannels != 1 )
    {
        OutputDebugString( _T(".wav file must be mono \n") );
            return E_FAIL;
    }

    // Create a sound buffer, we will be handling the memory for the buffer
    // ourselves in the sample, not via DSound
    DSBUFFERDESC dsbdesc;
    ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
    dsbdesc.dwSize = sizeof( DSBUFFERDESC );

    // Note that DSBCAPS_LOCDEFER means that the hardware voice will not
    // be allocated until playback
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_LOCDEFER;

    // NOTE: Retail dsound will have problems if we pass in dwBufferBytes=0
    // So in retail we patch this to 1.
#ifdef _DEBUG
    dsbdesc.dwBufferBytes = 0;
#else
    dsbdesc.dwBufferBytes = 1;
#endif
    dsbdesc.lpwfxFormat = &wfSoundFormat;

    if( FAILED( m_pDSound->CreateSoundBuffer( &dsbdesc, &m_pDSBuffer, NULL ) ) )
        return E_FAIL;

    // now load in the sound sample data into the DSound buffer

    // Find out how big the sample is
    m_wfSound.GetDuration( &dwSoundSampleSize );

    // Set our allocation to that size
    pvBuffer = malloc( dwSoundSampleSize );
    if( !pvBuffer )
        return E_FAIL;
    m_pSampleData = (BYTE *)pvBuffer;

    // Read sample data from the file
    m_wfSound.ReadSample( 0, m_pSampleData, dwSoundSampleSize, &dwSoundSampleSize );

    // Set up values for the new buffer
    m_pDSBuffer->SetBufferData( m_pSampleData, dwSoundSampleSize );
    m_pDSBuffer->SetLoopRegion( 0, dwSoundSampleSize );
    m_pDSBuffer->SetCurrentPosition( 0 );

    // Note: If the application doesn't care about vertical HRTF positioning,
    //       calling Set3DHRTFAzimuthOnly() can save about 60k of memory.
//  m_pDSound->Set3DHRTFAzimuthOnly();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene. As this code only changes text, there is no real animation
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DVERTEX*     pVertices;
    D3DVERTEX_TEX* pTexVertices;
    D3DXVECTOR3    vListenerOld = m_vListenerPosition;
    D3DXVECTOR3    vSourceOld   = m_vSourcePosition;

    if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 0 )
    {
        if( m_iLastButton != XINPUT_GAMEPAD_A )
        {
            m_iLastButton = XINPUT_GAMEPAD_A;

            switch(m_eCurrentState)
            {
                case START:
                    m_eCurrentState = STAGE1;
                    break;

                case STAGE1:
                    m_eCurrentState = STAGE2;
                    break;

                case STAGE2:
                    m_eCurrentState = STAGE3;
                    break;

                case STAGE3:
                    m_eCurrentState = STAGE4;
                    break;

                case STAGE4:
                    m_eCurrentState = START;
                    break;
                default:
                    m_eCurrentState = START;
            }
        }
    }
    else
    {
        m_iLastButton = !XINPUT_GAMEPAD_A;
    }

    // Increase/Decrease volume
    m_lVolume += LONG( ( ( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP   ) - 
                         ( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) ) * 50 );

    // Increase/Decrease volume
    m_lVolume += LONG( ( m_DefaultGamepad.fY2 ) * 50 );

    // Make sure volume is in the appropriate range
    if( m_lVolume < DSBVOLUME_MIN )
        m_lVolume = DSBVOLUME_MIN;
    if( m_lVolume > DSBVOLUME_MAX )
        m_lVolume = DSBVOLUME_MAX;

    // Handle movement of sound
    m_vSourcePosition.x += m_DefaultGamepad.fX1 * 0.2f;
    if( m_vSourcePosition.x < XMIN )
        m_vSourcePosition.x = XMIN;
    else if( m_vSourcePosition.x > XMAX )
        m_vSourcePosition.x = XMAX;

    m_vSourcePosition.z += m_DefaultGamepad.fY1 * 0.2f;
    if( m_vSourcePosition.z < ZMIN )
        m_vSourcePosition.z = ZMIN;
    else if( m_vSourcePosition.z > ZMAX )
        m_vSourcePosition.z = ZMAX;

    // Update source/listener vertex buffers
    #define SIZE 1.0f

    m_pSourceVB->Lock( 0, 0, (BYTE **)&pTexVertices, 0 );
    pTexVertices[0].p = m_vSourcePosition + D3DXVECTOR3( -(SIZE), 0.0f, -(SIZE) ); pTexVertices[0].c = D3DCOLOR_RGBA(0xff,0x00,0x00,0xff); pTexVertices[0].tu = 0.0f; pTexVertices[0].tv = 0.0f; 
    pTexVertices[1].p = m_vSourcePosition + D3DXVECTOR3( -(SIZE), 0.0f,  (SIZE) ); pTexVertices[1].c = D3DCOLOR_RGBA(0xff,0x00,0x00,0xff); pTexVertices[1].tu = 0.0f; pTexVertices[1].tv = 1.0f; 
    pTexVertices[2].p = m_vSourcePosition + D3DXVECTOR3(  (SIZE), 0.0f, -(SIZE) ); pTexVertices[2].c = D3DCOLOR_RGBA(0xff,0x00,0x00,0xff); pTexVertices[2].tu = 1.0f; pTexVertices[2].tv = 0.0f; 
    pTexVertices[3].p = m_vSourcePosition + D3DXVECTOR3(  (SIZE), 0.0f,  (SIZE) ); pTexVertices[3].c = D3DCOLOR_RGBA(0xff,0x00,0x00,0xff); pTexVertices[3].tu = 1.0f; pTexVertices[3].tv = 1.0f; 
    m_pSourceVB->Lock( 0, 0, (BYTE **)&pTexVertices, 0 );

    m_pListenerVB->Lock( 0, 0, (BYTE **)&pVertices, 0 );
    pVertices[0].p = m_vListenerPosition + D3DXVECTOR3( -0.5f, 0.0f, -0.5f ); pVertices[0].c = D3DCOLOR_RGBA(0xff,0xff,0x00,0xff);
    pVertices[1].p = m_vListenerPosition + D3DXVECTOR3( -0.5f, 0.0f,  0.5f ); pVertices[1].c = D3DCOLOR_RGBA(0xff,0xff,0x00,0xff);
    pVertices[2].p = m_vListenerPosition + D3DXVECTOR3(  0.5f, 0.0f, -0.5f ); pVertices[2].c = D3DCOLOR_RGBA(0xff,0xff,0x00,0xff);
    pVertices[3].p = m_vListenerPosition + D3DXVECTOR3(  0.5f, 0.0f,  0.5f ); pVertices[3].c = D3DCOLOR_RGBA(0xff,0xff,0x00,0xff);
    m_pListenerVB->Lock( 0, 0, (BYTE **)&pVertices, 0 );

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

    // Commit position/velocity changes
    m_pDSound->CommitDeferredSettings();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawLine()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawLine( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                            DWORD dwStartColor, DWORD dwEndColor )
{
    SCREENVERTEX v[2];
    v[0].pos = D3DXVECTOR4( x1, y1, 0.5f, 1.0f );   v[0].color = dwStartColor;
    v[1].pos = D3DXVECTOR4( x2, y2, 0.5f, 1.0f );   v[1].color = dwEndColor;
    
    // Render the line
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    m_pd3dDevice->SetVertexShader( D3DFVF_SCREENVERTEX) ;
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, v, sizeof(SCREENVERTEX) );
}





//-----------------------------------------------------------------------------
// Name: DrawRectOutline()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawRectOutline( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                                   DWORD dwColor )
{
    DrawLine( x1, y1, x1, y2, dwColor, dwColor );
    DrawLine( x1, y1, x2, y1, dwColor, dwColor );
    DrawLine( x2, y1, x2, y2, dwColor, dwColor );
    DrawLine( x1, y2, x2, y2, dwColor, dwColor );
}




//-----------------------------------------------------------------------------
// Name: DrawRect()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                            DWORD dwStartColor, DWORD dwEndColor )
{
    SCREENVERTEX v[4];
    v[0].pos = D3DXVECTOR4( x1-0.5f, y1-0.5f, 1.0f, 1.0f );  v[0].color = dwStartColor;
    v[1].pos = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );  v[1].color = dwStartColor;
    v[2].pos = D3DXVECTOR4( x1-0.5f, y2-0.5f, 1.0f, 1.0f );  v[2].color = dwEndColor;
    v[3].pos = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );  v[3].color = dwEndColor;
    
    // Render the rectangle
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    m_pd3dDevice->SetVertexShader( D3DFVF_SCREENVERTEX );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(SCREENVERTEX) );
}




//-----------------------------------------------------------------------------
// Name: DrawTextBox()
// Desc: Renders text in an outlined, semi-transparent box
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawTextBox( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                               WCHAR* strText )
{
    DrawRect( x1, y1, x2, y2, SEMITRANS_BLACK, SEMITRANS_BLACK );
    DrawRectOutline( x1, y1, x2, y2, BLACK );
    m_Font.DrawText( x1+5, y1+5, WHITE, strText );
}




//-----------------------------------------------------------------------------
// Name: DrawTestPattern()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawTestPattern()
{
    // Set state for rendering the quad
    m_pd3dDevice->SetTexture( 0, m_pTestPatternTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    m_pd3dDevice->SetStreamSource( 0, m_pTestPatternVB, sizeof(SCREENVERTEX_TEX) );
    m_pd3dDevice->SetVertexShader( D3DFVF_SCREENVERTEX_TEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
}




//-----------------------------------------------------------------------------
// Name: DrawStartText()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawStartText()
{
    // Stop the sound if it is playing
    m_pDSBuffer->Stop();

    // Render instructions in an outlined, semi-transparent box
    DrawTextBox( 64, 50, 576, 135, L"Turn CONTRAST up to full, then turn BRIGHTNESS up\n"
                                   L"until you can see five different shades of black\n"
                                   L"between these lines." );

    // Draw lines
    DrawLine( 468, 135, 468, 340, BLACK,  YELLOW );
    DrawLine( 336, 340, 600, 340, YELLOW, YELLOW );
    DrawLine( 336, 340, 336, 360, YELLOW, YELLOW );
    DrawLine( 600, 340, 600, 360, YELLOW, YELLOW );
}




//-----------------------------------------------------------------------------
// Name: DrawStage1Text()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawStage1Text()
{
    // Render instructions in an outlined, semi-transparent box
    DrawTextBox( 64, 50, 576, 135, L"Now reduce BRIGHTNESS until the grey bar\n"
                                   L"disappears, then turn the BRIGHTNESS back up\n"
                                   L"until it becomes just visible." );

    // Draw lines
    DrawLine( 536, 135, 536, 340, BLACK,  YELLOW );
    DrawLine( 520, 340, 552, 340, YELLOW, YELLOW );
    DrawLine( 520, 340, 520, 360, YELLOW, YELLOW );
    DrawLine( 552, 340, 552, 360, YELLOW, YELLOW );
}




//-----------------------------------------------------------------------------
// Name: DrawStage2Text()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawStage2Text()
{
    m_pd3dDevice->SetTexture( 0, NULL );

    // Render instructions in an outlined, semi-transparent box
    DrawTextBox( 64, 50, 576, 160, L"Set CONTRAST by lowering it until the edges of the\n"
                                   L"white bar look sharp, continue lowering until white\n"
                                   L"bar begins to look not-white, then turn back up until\n"
                                   L"white bar looks white again." );

    // Draw lines
    DrawLine( 173, 160, 173, 340, BLACK,  YELLOW );
    DrawLine( 113, 340, 223, 340, YELLOW, YELLOW );
    DrawLine( 113, 340, 113, 360, YELLOW, YELLOW );
    DrawLine( 223, 340, 223, 360, YELLOW, YELLOW );
}




//-----------------------------------------------------------------------------
// Name: DrawStage3Text()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawStage3Text()
{
    m_pd3dDevice->SetTexture( 0, NULL );

    // Render instructions in an outlined, semi-transparent box
    DrawTextBox( 64, 50, 576, 110, L"Now adjust COLOR to minimise bleeding, using\n"
                                   L"the red bar as your main reference." );
}




//-----------------------------------------------------------------------------
// Name: DrawStage4Text()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawStage4Text()
{
    // Start Sound
    m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    // Render a background
    RenderGradientBackground( DARK_GREEN, BLACK );

    // Render instructions in an outlined, semi-transparent box
    DrawTextBox( 64, 50, 576, 135, L"Adjust TV sound to desired level, use UP/DOWN to\n"
                                   L"control program volume for testing and use sticks\n"
                                   L"to move the direction of the sound to test speakers." );

    // Render the volume bar
    FLOAT fVolumePercent = 100.0f * ( m_lVolume - DSBVOLUME_MIN ) / (DSBVOLUME_MAX - DSBVOLUME_MIN );
    DrawRect( 64, 390-fVolumePercent, 84, 390, RED, DARK_RED );
    DrawRectOutline( 64, 290, 84, 390, YELLOW );

    // Render the volume text
    WCHAR strVolumeText[40];
    swprintf( strVolumeText, L"Volume: %d%%", (DWORD)fVolumePercent );
    m_Font.DrawText( 64, 390, 0xffffff00, strVolumeText);

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_NONE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetVertexShader( D3DFVF_D3DVERTEX );

    // Draw the floor
    m_pd3dDevice->SetStreamSource( 0, m_pFloorVB, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Draw the grid
    m_pd3dDevice->SetStreamSource( 0, m_pGridVB, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 2 * ( ( ZMAX - ZMIN + 1 ) + ( XMAX - XMIN + 1 ) ) );

    // Draw the listener
    m_pd3dDevice->SetStreamSource( 0, m_pListenerVB, sizeof( D3DVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Draw the source
    m_pd3dDevice->SetTexture( 0, m_pSpeakerTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    m_pd3dDevice->SetVertexShader( D3DFVF_D3DVERTEX_TEX );
    m_pd3dDevice->SetStreamSource( 0, m_pSourceVB, sizeof( D3DVERTEX_TEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Depending on the app state, call one of the following methods
    switch( m_eCurrentState )
    {
        case START:
            DrawTestPattern();
            DrawStartText();
            break;

        case STAGE1:
            DrawTestPattern();
            DrawStage1Text();
            break;
        
        case STAGE2:
            DrawTestPattern();
            DrawStage2Text();
            break;
        
        case STAGE3:
            DrawTestPattern();
            DrawStage3Text();
            break;
        
        case STAGE4:
            DrawStage4Text();
            break;
        
        case ENDSTAGE:
            break;
    }

    return S_OK;
}




