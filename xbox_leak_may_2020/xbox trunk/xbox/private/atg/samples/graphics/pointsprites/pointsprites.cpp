//-----------------------------------------------------------------------------
// File: PointSprites.cpp
//
// Desc: Sample showing how to use point sprites to do particle effects. Note:
//       this sample uses a significant number of alpha-blended pointsprites
//       (several pointsprites are used per particle, to get a motion-blur
//       effect), so it uses alot of CPU computations and uses alot of pixel 
//       fill.
//
// Performance Notes: 
//       A huge performance win for this app (as with most other apps using 
//       dynamic vertex buffers) is to multi-buffer the dynamic vertex
//       buffers. Since one vertex buffer will be tied up during render, we use
//       a free vertex buffer to fill and use for the current frame.
//       
//       This sample also gains a big boost in performance by setting a
//       smaller kickoff size, so that our dynamic-VBs can be re-locked
//       sooner after rendering. This reduces stalls significantly. Note 
//       that in a real-world app, reducing the kickoff size probably would not
//       be necessary, as there would be lots of other rendering happening
//       every frame. So kickoffs would automatically happen more often, thus
//       avoiding the problem of stalling when frequently locking dynamic
//       vertex buffers.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       03.24.01 - Pointsize scale factor changed for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBResource.h>
#include <XBUtil.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"




//-----------------------------------------------------------------------------
// PERFORMANCE NOTES.
//
// Set smaller kickoff size, so that our dynamic-VBs can be re-locked
// sooner after rendering.  This will reduce stalls in this app.
//
// Note that in a real-world app, reducing the kickoff size probably would
// not be necessary, as there would be lots of other rendering happening
// every frame.  So kickoffs would automatically happen more often, thus
// avoiding the problem of stalling when we frequently lock our "deep"
// dynamic vertex buffers.
//-----------------------------------------------------------------------------
#define NUM_PARTICLE_BUFFERS 3                  // # of vertex buffers for the particle system
#define KICKOFF_SIZE         (4 * (1<<10))      // Kick off size. Must be power of 2.
#define PUSHBUFFER_SIZE      (128*KICKOFF_SIZE) // Push buffer size as a multiple of kickoff size




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_DPAD,         XBHELP_PLACEMENT_1, L"Zoom" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Move\ncamera" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Rotate\ncamera" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Add\nparticles" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Remove\nparticles" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Animate\nemitter" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Change\ncolor" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 8




//-----------------------------------------------------------------------------
// Custom vertex type
//-----------------------------------------------------------------------------
struct PARTICLEVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
};

#define D3DFVF_PARTICLEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

struct LIGHTVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
    FLOAT       tu;
    FLOAT       tv;
};

#define D3DFVF_LIGHTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct GROUNDVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
    FLOAT       tu;
    FLOAT       tv;
};

#define D3DFVF_GROUNDVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)





//-----------------------------------------------------------------------------
// Global structs and data for the ground object
//-----------------------------------------------------------------------------
#define GROUND_SIZE  20.0f
#define GROUND_COLOR 0xddeeeeff




//-----------------------------------------------------------------------------
// Global data for the particles
//-----------------------------------------------------------------------------
struct PARTICLE
{
    BOOL        m_bSpark;     // Spark? or real particle?

    D3DXVECTOR3 m_vPos;       // Current position
    D3DXVECTOR3 m_vVel;       // Current velocity

    D3DXVECTOR3 m_vPos0;      // Initial position
    D3DXVECTOR3 m_vVel0;      // Initial velocity
    FLOAT       m_fTime0;     // Time of creation

    D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
    D3DXCOLOR   m_clrFade;    // Faded diffuse color
    FLOAT       m_fFade;      // Fade progression
};


enum PARTICLE_COLORS { COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE, NUM_COLORS };


D3DXCOLOR g_clrColor[NUM_COLORS] =
{
    D3DXCOLOR( 1.0f,   1.0f,   1.0f,   1.0f ),
    D3DXCOLOR( 1.0f,   0.5f,   0.5f,   1.0f ),
    D3DXCOLOR( 0.5f,   1.0f,   0.5f,   1.0f ),
    D3DXCOLOR( 0.125f, 0.5f,   1.0f,   1.0f )
};


DWORD g_clrColorFade[NUM_COLORS] =
{
    D3DXCOLOR( 1.0f,   0.25f,   0.25f,   1.0f ),
    D3DXCOLOR( 1.0f,   0.25f,   0.25f,   1.0f ),
    D3DXCOLOR( 0.25f,  0.75f,   0.25f,   1.0f ),
    D3DXCOLOR( 0.125f, 0.25f,   0.75f,   1.0f )
};




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
class CParticleSystem
{
protected:
    FLOAT                   m_fRadius;
    PARTICLE*               m_pParticles;
    DWORD                   m_dwMaxParticles;
    DWORD                   m_dwNumParticles;

    // Geometry
    LPDIRECT3DVERTEXBUFFER8 m_pPointSpritesVBs[NUM_PARTICLE_BUFFERS];
    LPDIRECT3DVERTEXBUFFER8 m_pLightsVBs[NUM_PARTICLE_BUFFERS];
    LPDIRECT3DVERTEXBUFFER8 m_pPointSpritesVB;
    LPDIRECT3DVERTEXBUFFER8 m_pLightsVB;
    DWORD                   m_dwCurrentBuffer;
    DWORD                   m_dwNumParticlesToRender;
    DWORD                   m_dwNumLightsToRender;

    // Rendering device
    LPDIRECT3DDEVICE8       m_pd3dDevice;

public:
    CParticleSystem( DWORD dwMaxParticles, FLOAT fRadius );
   ~CParticleSystem();

    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
    HRESULT DeleteDeviceObjects();

    HRESULT Update( FLOAT fSecsPerFrame, DWORD dwNumParticlesToEmit,
                    const D3DXCOLOR &dwEmitColor, const D3DXCOLOR &dwFadeColor,
                    FLOAT fEmitVel, D3DXVECTOR3 vPosition );

    HRESULT RenderParticles( LPDIRECT3DTEXTURE8 pTexture );
    HRESULT RenderLights( LPDIRECT3DTEXTURE8 pTexture );
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;      // Packed resources for the app
    CXBFont            m_Font;             // Font class
    CXBHelp            m_Help;             // Help class
    BOOL               m_bDrawHelp;        // Whether to draw help

    // Particle system timing
    BOOL               m_bParticleSystemRunning;
    FLOAT              m_fParticleSystemTime;
    FLOAT              m_fElapsedParticleSystemTime;

    // Ground stuff
    LPDIRECT3DVERTEXBUFFER8 m_pGroundVB;
    LPDIRECT3DTEXTURE8 m_pGroundTexture;
    D3DXPLANE          m_planeGround;

    // Particle stuff
    LPDIRECT3DTEXTURE8 m_pParticleTexture;
    CParticleSystem*   m_pParticleSystem;
    DWORD              m_dwNumParticlesToEmit;
    DWORD              m_dwParticleColor;
    BOOL               m_bAnimateEmitter;

    // Static vectors for determining view position
    D3DXVECTOR3        m_vPosition;
    D3DXVECTOR3        m_vVelocity;
    FLOAT              m_fYaw;
    FLOAT              m_fYawVelocity;
    FLOAT              m_fPitch;
    FLOAT              m_fPitchVelocity;
    D3DXMATRIX         m_matView;
    D3DXMATRIX         m_matReflectedView;
    D3DXMATRIX         m_matOrientation;

protected:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

public:
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
// Desc: Application constructor.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Set the push buffer and kickoff size. See comment about performance at
    // the top of this file.
    Direct3D_SetPushBufferSize( PUSHBUFFER_SIZE, KICKOFF_SIZE );

    // Init member variables
    m_bDrawHelp            = FALSE;

    m_bParticleSystemRunning     = TRUE;
    m_fParticleSystemTime        = 0.0f;
    m_fElapsedParticleSystemTime = 0.0f;

    m_pGroundTexture       = NULL;
    m_pGroundVB            = NULL;
    m_planeGround          = D3DXPLANE( 0.0f, 1.0f, 0.0f, 0.0f );

    m_pParticleTexture     = NULL;
    m_pParticleSystem      = new CParticleSystem( 4096, 0.03f );
    m_dwNumParticlesToEmit = 10;
    m_bAnimateEmitter      = FALSE;
    m_dwParticleColor      = COLOR_WHITE;

    m_vPosition            = D3DXVECTOR3( 0.0f, 3.0f,-4.0f );
    m_vVelocity            = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_fYaw                 = 0.0f;
    m_fYawVelocity         = 0.0f;
    m_fPitch               = 0.5f;
    m_fPitchVelocity       = 0.0f;
    D3DXMatrixTranslation( &m_matView, 0.0f, 0.0f, 10.0f );
    D3DXMatrixTranslation( &m_matOrientation, 0.0f, 0.0f, 0.0f );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;

    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create textures
    m_pGroundTexture   = m_xprResource.GetTexture( resource_Ground2_OFFSET );
    m_pParticleTexture = m_xprResource.GetTexture( resource_Particle_OFFSET );

    // Create vertex buffer for ground object
    hr = m_pd3dDevice->CreateVertexBuffer( 4*sizeof(GROUNDVERTEX),
                                           D3DUSAGE_WRITEONLY, D3DFVF_GROUNDVERTEX,
                                           D3DPOOL_MANAGED, &m_pGroundVB );
    if( FAILED(hr) )
        return E_FAIL;

    // Fill vertex buffer
    GROUNDVERTEX* pVertices;
    m_pGroundVB->Lock( 0, 0, (BYTE**)&pVertices, NULL );
    pVertices[0].v     = GROUND_SIZE * D3DXVECTOR3( -0.5f, 0.0f, -0.5f );
    pVertices[0].color = GROUND_COLOR;
    pVertices[0].tu    = 0.0f;
    pVertices[0].tv    = 0.0f;
    pVertices[1].v     = GROUND_SIZE * D3DXVECTOR3( -0.5f, 0.0f, +0.5f );
    pVertices[1].color = GROUND_COLOR;
    pVertices[1].tu    = 0.0f;
    pVertices[1].tv    = 3.0f;
    pVertices[2].v     = GROUND_SIZE * D3DXVECTOR3( +0.5f, 0.0f, +0.5f );
    pVertices[2].color = GROUND_COLOR;
    pVertices[2].tu    = 3.0f;
    pVertices[2].tv    = 3.0f;
    pVertices[3].v     = GROUND_SIZE * D3DXVECTOR3( +0.5f, 0.0f, -0.5f );
    pVertices[3].color = GROUND_COLOR;
    pVertices[3].tu    = 3.0f;
    pVertices[3].tv    = 0.0f;
    m_pGroundVB->Unlock();

    // Initialize the particle system
    if( FAILED( hr = m_pParticleSystem->InitDeviceObjects( m_pd3dDevice ) ) )
        return hr;

    // Set the world matrix
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Animate the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // De-accelerate the camera movement (for smooth motion)
    FLOAT fScale = max( 0.0f, ( 1.0f - 2.0f*m_fElapsedTime) );
    m_vVelocity      *= fScale;
    m_fYawVelocity   *= fScale;
    m_fPitchVelocity *= fScale;

    // Update velocities from the gamepad
    m_vVelocity.x    += 1.0f*m_fElapsedTime*m_DefaultGamepad.fX1; // Slide left/right
    m_vVelocity.y    += 1.0f*m_fElapsedTime*m_DefaultGamepad.fY1; // Slide up/down
    if( m_DefaultGamepad.wButtons&XINPUT_GAMEPAD_DPAD_UP )   
        m_vVelocity.z += 1.0f*m_fElapsedTime;
    if( m_DefaultGamepad.wButtons&XINPUT_GAMEPAD_DPAD_DOWN ) 
        m_vVelocity.z -= 1.0f*m_fElapsedTime;

    m_fYawVelocity   += 1.0f*m_fElapsedTime*m_DefaultGamepad.fX2; // Turn left/right
    m_fPitchVelocity += 1.0f*m_fElapsedTime*m_DefaultGamepad.fY2; // Turn up/down

    // Handle options
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        if( m_dwNumParticlesToEmit < 10 ) 
            m_dwNumParticlesToEmit += 1;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        if( m_dwNumParticlesToEmit > 0 )
            m_dwNumParticlesToEmit -= 1;
    
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        m_bAnimateEmitter = !m_bAnimateEmitter;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
        m_dwParticleColor = (m_dwParticleColor+1)%NUM_COLORS;

    // Update the position vector
    D3DXVECTOR3 vT = m_vVelocity * 5.0f * m_fElapsedTime;
    D3DXVec3TransformNormal( &vT, &vT, &m_matOrientation );
    m_vPosition += vT;
    if( m_vPosition.y < 1.0f )
        m_vPosition.y = 1.0f;

    // Update the yaw-pitch-rotation vector
    m_fYaw   += 5.0f * m_fElapsedTime * m_fYawVelocity;
    m_fPitch += 5.0f * m_fElapsedTime * m_fPitchVelocity;
    if( m_fPitch < 0.0f )      m_fPitch = 0.0f;
    if( m_fPitch > D3DX_PI/2 ) m_fPitch = D3DX_PI/2;

    // Set the view matrix
    D3DXQUATERNION qR;
    D3DXQuaternionRotationYawPitchRoll( &qR, m_fYaw, m_fPitch, 0.0f );
    D3DXMatrixAffineTransformation( &m_matOrientation, 1.25f, NULL, &qR, &m_vPosition );
    D3DXMatrixInverse( &m_matView, NULL, &m_matOrientation );

    // Computed the reflected view
    D3DXMatrixReflect( &m_matReflectedView, &m_planeGround );
    D3DXMatrixMultiply( &m_matReflectedView, &m_matReflectedView, &m_matView );

    // Check the Start button to start/stop the particle system
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
        m_bParticleSystemRunning = !m_bParticleSystemRunning;

    if( m_bParticleSystemRunning )
        m_fElapsedParticleSystemTime = m_fElapsedTime;
    else
        m_fElapsedParticleSystemTime = 0.0f;
    m_fParticleSystemTime += m_fElapsedParticleSystemTime;

    // Determine emitter position
    D3DXVECTOR3 vEmitterPostion( 0.0f, 0.0f, 0.f );
    if( m_bAnimateEmitter )
        vEmitterPostion = D3DXVECTOR3( 3*sinf(m_fParticleSystemTime), 0.0f, 3*cosf(m_fParticleSystemTime) );

    // Update particle system
    m_pParticleSystem->Update( m_fElapsedParticleSystemTime, m_dwNumParticlesToEmit,
                               g_clrColor[m_dwParticleColor],
                               g_clrColorFade[m_dwParticleColor], 8.0f,
                               vEmitterPostion );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x000000ff, 1.0f, 0L );

    // Set state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,   D3DZB_FALSE ); // we don't require z-buffer

    // Draw the ground
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
    m_pd3dDevice->SetTexture( 0, m_pGroundTexture );
    m_pd3dDevice->SetVertexShader( D3DFVF_GROUNDVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pGroundVB, sizeof(GROUNDVERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );

    // Render the particles
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff000000 );
    m_pParticleSystem->RenderParticles( m_pParticleTexture );

    // Render the ground effect lights
    m_pParticleSystem->RenderLights( m_pParticleTexture );

    // Draw reflection of particles
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matReflectedView );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x40000000 );
    m_pParticleSystem->RenderParticles( m_pParticleTexture );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64,  50, 0xffffffff, L"PointSprites" );
        m_Font.DrawText( 450,  50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CParticleSystem()
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::CParticleSystem( DWORD dwMaxParticles, float fRadius )
{
    m_fRadius          = fRadius;

    m_pParticles       = new PARTICLE[dwMaxParticles];
    m_dwMaxParticles   = dwMaxParticles;
    m_dwNumParticles   = 0;

    m_pd3dDevice       = NULL;
    
    m_pPointSpritesVB  = NULL;
    m_pLightsVB        = NULL;
    m_dwCurrentBuffer  = 0;

    m_dwNumParticlesToRender = 0;
    m_dwNumLightsToRender    = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CParticleSystem()
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::~CParticleSystem()
{
    DeleteDeviceObjects();

    SAFE_DELETE( m_pParticles );
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::InitDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice )
{
    HRESULT hr;

    // Save the device ptr
    m_pd3dDevice = pd3dDevice;

    // Create the particle texture
    // Create the particle system's vertex buffers. Each point sprite particle
    // requires one vertex and each light takes four vertices.
    for( DWORD buf = 0; buf < NUM_PARTICLE_BUFFERS; buf++ )
    {
        hr = m_pd3dDevice->CreateVertexBuffer( (m_dwMaxParticles+1)*sizeof(PARTICLEVERTEX),
                                               D3DUSAGE_WRITEONLY, D3DFVF_PARTICLEVERTEX,
                                               D3DPOOL_DEFAULT, &m_pPointSpritesVBs[buf] );
        if( FAILED(hr) )
            return E_FAIL;

        hr = m_pd3dDevice->CreateVertexBuffer( 4*(m_dwMaxParticles+1)*sizeof(LIGHTVERTEX),
                                               D3DUSAGE_WRITEONLY, D3DFVF_LIGHTVERTEX,
                                               D3DPOOL_DEFAULT, &m_pLightsVBs[buf] );
        if( FAILED(hr) )
            return E_FAIL;

        // Write default values to light vertices (the texture coordinates for
        // the quads never changes)
        LIGHTVERTEX* pLightVertices;
        m_pLightsVBs[buf]->Lock( 0, 0, (BYTE**)&pLightVertices, NULL );
        ZeroMemory( pLightVertices, 4*m_dwMaxParticles*sizeof(LIGHTVERTEX) );
        
        for( DWORD i=0; i<m_dwMaxParticles; i++ )
        {
            pLightVertices[1].tv = 1.0f;
            pLightVertices[2].tu = 1.0f;
            pLightVertices[2].tv = 1.0f;
            pLightVertices[3].tu = 1.0f;
            pLightVertices += 4;
        }

        // Unlock the vertex buffer
        m_pLightsVBs[buf]->Unlock();
    }

    // Select starting vertex buffers
    m_dwCurrentBuffer = 0;
    m_pPointSpritesVB = m_pPointSpritesVBs[m_dwCurrentBuffer];
    m_pLightsVB       = m_pLightsVBs[m_dwCurrentBuffer];

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::DeleteDeviceObjects()
{
    m_pd3dDevice = NULL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Update the particles in the particle system.
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::Update( FLOAT fSecsPerFrame, DWORD dwNumParticlesToEmit,
                                 const D3DXCOLOR &clrEmitColor,
                                 const D3DXCOLOR &clrFadeColor, float fEmitVel,
                                 D3DXVECTOR3 vPosition )
{
    static float fTime = 0.0f;
    fTime += fSecsPerFrame;

    // For performance reasons, vertex buffers are multi-buffered. Each time
    // the vertex buffer contents are updated, use a new vertex buffer.
    if( m_pPointSpritesVB->IsBusy() )
    {
        if( ++m_dwCurrentBuffer >= NUM_PARTICLE_BUFFERS )
            m_dwCurrentBuffer = 0;
        m_pPointSpritesVB = m_pPointSpritesVBs[m_dwCurrentBuffer];
        m_pLightsVB       = m_pLightsVBs[m_dwCurrentBuffer];
    }
 
    // Lock vertex buffers
    PARTICLEVERTEX* pPointSpriteVertices;
    LIGHTVERTEX*    pLightVertices;
    m_pPointSpritesVB->Lock( 0, 0, (BYTE**)&pPointSpriteVertices, NULL );
    m_pLightsVB->Lock( 0, 0, (BYTE**)&pLightVertices, NULL );
    m_dwNumParticlesToRender = 0;
    m_dwNumLightsToRender    = 0;

    // Update particles
    for( DWORD i=0; i<m_dwNumParticles; i++ )
    {
        PARTICLE* pParticle = &m_pParticles[i];

        // Calculate new position
        FLOAT t = fTime - pParticle->m_fTime0;

        if( pParticle->m_bSpark )
        {
            pParticle->m_vPos    = pParticle->m_vVel0 * t + pParticle->m_vPos0;
            pParticle->m_vPos.y -= (0.5f * 5.0f) * (t * t);
            pParticle->m_vVel.y  = pParticle->m_vVel0.y - 5.0f * t;
            pParticle->m_fFade  -= fSecsPerFrame * 2.25f;
        }
        else
        {
            pParticle->m_vPos    = pParticle->m_vVel0 * t + pParticle->m_vPos0;
            pParticle->m_vPos.y -= (0.5f * 9.8f) * (t * t);
            pParticle->m_vVel.y  = pParticle->m_vVel0.y - 9.8f * t;
            pParticle->m_fFade  -= fSecsPerFrame * 0.25f;
        }

        if( pParticle->m_fFade < 0.0f )
            pParticle->m_fFade = 0.0f;

        // Kill old particles
        if( pParticle->m_vPos.y < m_fRadius || pParticle->m_bSpark && pParticle->m_fFade <= 0.0f )
        {
            // Emit sparks
            if( !pParticle->m_bSpark )
            {
                for( int j=0; j<4; j++ )
                {
                    FLOAT fRand1 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 2.00f;
                    FLOAT fRand2 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 0.25f;

                    PARTICLE* pSpark = &m_pParticles[m_dwNumParticles++];
                    pSpark->m_bSpark   = TRUE;
                    pSpark->m_vPos0    = pParticle->m_vPos;
                    pSpark->m_vPos0.y  = m_fRadius;
                    pSpark->m_vVel0.x  = pParticle->m_vVel.x * 0.25f + cosf(fRand1) * sinf(fRand2);
                    pSpark->m_vVel0.z  = pParticle->m_vVel.z * 0.25f + sinf(fRand1) * sinf(fRand2);
                    pSpark->m_vVel0.y  = cosf(fRand2);
                    pSpark->m_vVel0.y *= ((FLOAT)rand()/(FLOAT)RAND_MAX) * 1.5f;
                    pSpark->m_vPos     = pSpark->m_vPos0;
                    pSpark->m_vVel     = pSpark->m_vVel0;
                    D3DXColorLerp( &pSpark->m_clrDiffuse, &pParticle->m_clrFade,
                                   &pParticle->m_clrDiffuse, pParticle->m_fFade );
                    pSpark->m_clrFade  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
                    pSpark->m_fFade    = 1.0f;
                    pSpark->m_fTime0   = fTime;
                }
            }

            // Kill this particle (we can do this fast, by simply moving the
            // last particle to take the current particle's place).
            m_pParticles[i--] = m_pParticles[--m_dwNumParticles];
        }
        else
        {
            // Build vertex buffers for the particles
            FLOAT fSpeed = D3DXVec3LengthSq( &pParticle->m_vVel );
            UINT  dwSteps;
            if( fSpeed < 1.0f )        dwSteps = 2;
            else if( fSpeed <  4.00f ) dwSteps = 3;
            else if( fSpeed <  9.00f ) dwSteps = 4;
            else if( fSpeed < 12.25f ) dwSteps = 5;
            else if( fSpeed < 16.00f ) dwSteps = 6;
            else if( fSpeed < 20.25f ) dwSteps = 7;
            else                       dwSteps = 8;

            D3DXVECTOR3 vPos = pParticle->m_vPos;
            D3DXVECTOR3 vVel = pParticle->m_vVel * -0.04f / (FLOAT)dwSteps;

            D3DXCOLOR clrDiffuse;
            D3DXColorLerp( &clrDiffuse, &pParticle->m_clrFade, &pParticle->m_clrDiffuse, 
                                        pParticle->m_fFade );
            DWORD dwDiffuse = (DWORD)clrDiffuse;

            // Compute vertices for ground lighting effects
            if( vPos.y < 1.0f )
            {
                FLOAT fY = vPos.y;
                if( fY < 0.0f )
                    fY = 0.0f;

                FLOAT fSize     = fY * 0.25f + m_fRadius;
                DWORD dwLightDiffuse = (DWORD)( clrDiffuse * ( (1.0f-fY) * 0.5f ) );

                pLightVertices[0].v.x   = pLightVertices[1].v.x   = vPos.x + fSize;
                pLightVertices[0].v.z   = pLightVertices[3].v.z   = vPos.z + fSize;
                pLightVertices[2].v.x   = pLightVertices[3].v.x   = vPos.x - fSize;
                pLightVertices[1].v.z   = pLightVertices[2].v.z   = vPos.z - fSize;
                pLightVertices[0].color = pLightVertices[1].color = dwLightDiffuse;
                pLightVertices[2].color = pLightVertices[3].color = dwLightDiffuse;

                // Advance to next light
                pLightVertices += 4;
                m_dwNumLightsToRender++;
            }

            // Use multiple pointsprites per particle to get a motion-blur effect
            for( DWORD j = 0; j < dwSteps; j++ )
            {
                if( vPos.y >= 0.0f )
                {
                    pPointSpriteVertices->v     = vPos;
                    pPointSpriteVertices->color = dwDiffuse;
                    pPointSpriteVertices++;
                    m_dwNumParticlesToRender++;

                    // Stop if the VB gets full.
                    if( m_dwNumParticlesToRender >= m_dwMaxParticles )
                        break;
                }
                vPos += vVel;
            }
        }
    }

    // Unlock the vertex buffers
    m_pPointSpritesVB->Unlock();
    m_pLightsVB->Unlock();

    // Emit new particles
    while( dwNumParticlesToEmit > 0 && m_dwNumParticles < m_dwMaxParticles/4 )
    {
        FLOAT fRand1 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 2.00f;
        FLOAT fRand2 = ((FLOAT)rand()/(FLOAT)RAND_MAX) * D3DX_PI * 0.25f;

        PARTICLE* pParticle = &m_pParticles[m_dwNumParticles];
        pParticle->m_bSpark     = FALSE;
        pParticle->m_vPos0      = vPosition + D3DXVECTOR3( 0.0f, m_fRadius, 0.0f );
        pParticle->m_vVel0.x    = cosf(fRand1) * sinf(fRand2) * 2.5f;
        pParticle->m_vVel0.z    = sinf(fRand1) * sinf(fRand2) * 2.5f;
        pParticle->m_vVel0.y    = cosf(fRand2);
        pParticle->m_vVel0.y   *= ((FLOAT)rand()/(FLOAT)RAND_MAX) * fEmitVel;
        pParticle->m_vPos       = pParticle->m_vPos0;
        pParticle->m_vVel       = pParticle->m_vVel0;
        pParticle->m_clrDiffuse = clrEmitColor;
        pParticle->m_clrFade    = clrFadeColor;
        pParticle->m_fFade      = 1.0f;
        pParticle->m_fTime0     = fTime;

        dwNumParticlesToEmit--;
        m_dwNumParticles++;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderParticles()
// Desc: Renders the particle system using pointsprites.
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::RenderParticles( LPDIRECT3DTEXTURE8 pTexture )
{
    if( 0 == m_dwNumParticlesToRender )
        return S_OK;

    // Set the render states for using point sprites. Note that pointsprites
    // use texture stage 3 only.
    m_pd3dDevice->SetTexture( 3, pTexture );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,         FtoDW(0.10f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,      FtoDW(0.00f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,      FtoDW(0.00f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,      FtoDW(1.00f) );

    // Turn on alphablending
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );

    // Render particles
    m_pd3dDevice->SetStreamSource( 0, m_pPointSpritesVB, sizeof(PARTICLEVERTEX) );
    m_pd3dDevice->SetVertexShader( D3DFVF_PARTICLEVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, m_dwNumParticlesToRender );

    // Reset render states
    m_pd3dDevice->SetTexture( 3, NULL );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  FALSE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderLights()
// Desc: Renders ground lighting effects for the particle system.
//-----------------------------------------------------------------------------
HRESULT CParticleSystem::RenderLights( LPDIRECT3DTEXTURE8 pTexture )
{
    if( 0 == m_dwNumLightsToRender )
        return S_OK;

    // Turn on alphablending
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );

    // Set state for rendering lights
    m_pd3dDevice->SetTexture( 0, pTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    
    // Render lights
    m_pd3dDevice->SetStreamSource( 0, m_pLightsVB, sizeof(LIGHTVERTEX) );
    m_pd3dDevice->SetVertexShader( D3DFVF_LIGHTVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, m_dwNumLightsToRender );

    // Restore state
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    return S_OK;
}



