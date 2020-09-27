//-----------------------------------------------------------------------------
// File: Tiling.cpp
//
// Desc: Sample code to show off how tiling and z-compression of rendertargets
//       has an effect on performance.
//
// Hist: 05.15.01 - New for the June XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBMesh.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBResource.h>
#include <XBUtil.h>
#include <xgraphics.h>
#include "Resource.h" // Resource header produced by the bundler tool




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Tile primary\nframe buffer" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Tile primary\ndepth buffer" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Tile texture\nframe buffer" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Tile texture\ndepth buffer" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Toggle\nrendertarget" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS 6




//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Light.bmp",    resource_Light_OFFSET },
    { "SkyBoxXP.bmp", resource_SkyBoxXP_OFFSET },
    { "SkyBoxXN.bmp", resource_SkyBoxXN_OFFSET },
    { "SkyBoxYP.bmp", resource_SkyBoxYP_OFFSET },
    { "SkyBoxYN.bmp", resource_SkyBoxYN_OFFSET },
    { "SkyBoxZP.bmp", resource_SkyBoxZP_OFFSET },
    { "SkyBoxZN.bmp", resource_SkyBoxZN_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Info for rendering the fill-eating streaks
//-----------------------------------------------------------------------------
struct STREAKVERTEX
{
    D3DXVECTOR4 p;
    DWORD       color;
};

const DWORD NUM_STREAKS              = 10;
const DWORD NUM_PARTICLES_PER_STREAK = 100;




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                  m_Font16;         // System font
    CXBFont                  m_Font12;         // System font
    CXBHelp                  m_Help;           // Application help
    BOOL                     m_bDrawHelp;      // Whether to display help

    CXBPackedResource        m_xprResource;    // Packed resources (textures)
    
    CXBMesh                  m_SkyBoxObject;   // The skybox geometry

    // Members for rendering streaks
    LPDIRECT3DTEXTURE8       m_pStreakTexture;
    STREAKVERTEX*            m_vParticleStreak[NUM_STREAKS];
    FLOAT                    m_fStreakTimeScale[NUM_STREAKS];
    FLOAT                    m_fStreakSinScale[NUM_STREAKS];
    DWORD                    m_dwStreakColor[NUM_STREAKS];
    DWORD                    m_dwActiveStreakParticle;

    // Members for the secondary rendertarget
    LPDIRECT3DTEXTURE8       m_pSecondaryTexture;
    LPDIRECT3DSURFACE8       m_pSecondaryColorSurface;
    VOID*                    m_pColorBufferMemory; 
    LPDIRECT3DSURFACE8       m_pSecondaryDepthSurface;
    VOID*                    m_pDepthBufferMemory;
    BOOL                     m_bUseSecondaryRenderTarget;

    // Members for tiling the color and depth buffers
    D3DTILE                  m_tile0;
    D3DTILE                  m_tile1;
    BOOL                     m_bPrimaryColorBufferTiled;
    BOOL                     m_bPrimaryDepthBufferTiled;
    BOOL                     m_bPrimaryDepthBufferCompressed;
    BOOL                     m_bSecondaryColorBufferTiled;
    BOOL                     m_bSecondaryDepthBufferTiled;
    BOOL                     m_bSecondaryDepthBufferCompressed;

    // Internal functions
    HRESULT CreateSecondaryRenderTarget();
    VOID    TilePrimaryColorBuffer( BOOL bTiled );
    VOID    TilePrimaryDepthBuffer( BOOL bTiled, BOOL bCompressed );
    VOID    TileSecondaryColorBuffer( BOOL bTile );
    VOID    TileSecondaryDepthBuffer( BOOL bTile, BOOL bCompressed );
    HRESULT RenderStreaks();

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
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Initialize options
    m_bDrawHelp                       = FALSE;

    m_bUseSecondaryRenderTarget       = FALSE;

    m_bPrimaryColorBufferTiled        = TRUE;
    m_bPrimaryDepthBufferTiled        = TRUE;
    m_bPrimaryDepthBufferCompressed   = TRUE;

    m_bSecondaryColorBufferTiled      = FALSE;
    m_bSecondaryDepthBufferTiled      = FALSE;
    m_bSecondaryDepthBufferCompressed = FALSE;

    // Initialize the streaks
    for( DWORD k=0; k<NUM_STREAKS; k++ )
    {
        m_vParticleStreak[k]  = new STREAKVERTEX[NUM_PARTICLES_PER_STREAK];
        m_fStreakTimeScale[k] = 0.5f + 1*((FLOAT)(k))/NUM_STREAKS;
        m_fStreakSinScale[k]  = 4.0f - 3*((FLOAT)(k))/NUM_STREAKS;

        switch( k%10 )
        {
            case 0: m_dwStreakColor[k] = 0x00ffffff; break;
            case 1: m_dwStreakColor[k] = 0x00ff0000; break;
            case 2: m_dwStreakColor[k] = 0x0000ff00; break;
            case 3: m_dwStreakColor[k] = 0x000000ff; break;
            case 4: m_dwStreakColor[k] = 0x00ffff00; break;
            case 5: m_dwStreakColor[k] = 0x00ff00ff; break;
            case 6: m_dwStreakColor[k] = 0x0000ffff; break;
            case 7: m_dwStreakColor[k] = 0x00ff0000; break;
            case 8: m_dwStreakColor[k] = 0x0000ff00; break;
            case 9: m_dwStreakColor[k] = 0x000000ff; break;
        }
    }
    m_dwActiveStreakParticle = 0;
    m_pStreakTexture         = NULL;
}




//-----------------------------------------------------------------------------
// Name: CreateSecondaryRenderTarget()
// Desc: Creates a texture and depth buffer to render into
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateSecondaryRenderTarget()
{
    DWORD dwWidth  = 640;
    DWORD dwHeight = 480;

    // Create a new 32-bit color surface using allocated tile-able memory
    m_pSecondaryTexture = new D3DTexture;
    XGSetTextureHeader( dwWidth, dwHeight, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, 
                        m_pSecondaryTexture, 0, 0 );
    m_pColorBufferMemory = D3D_AllocContiguousMemory( 4*dwWidth*dwHeight, 
                                                      D3DTILE_ALIGNMENT );
    m_pSecondaryTexture->Register( m_pColorBufferMemory );
    m_pSecondaryTexture->GetSurfaceLevel( 0, &m_pSecondaryColorSurface );

    // Create a new 32-bit depth surface using allocated tile-able memory
    m_pSecondaryDepthSurface = new D3DSurface;
    XGSetSurfaceHeader( dwWidth, dwHeight, D3DFMT_LIN_D24S8, m_pSecondaryDepthSurface, 0, 0 );
    m_pDepthBufferMemory = D3D_AllocContiguousMemory( 4*dwWidth*dwHeight, 
                                                      D3DTILE_ALIGNMENT );
    m_pSecondaryDepthSurface->Register( m_pDepthBufferMemory );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font16.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    if( FAILED( m_Font12.Create( m_pd3dDevice, "Font12.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the geometry models
    if( FAILED( m_SkyBoxObject.Create( m_pd3dDevice, "Models\\SkyBox.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Get the light's texture
    m_pStreakTexture = m_xprResource.GetTexture( "Light.bmp" );
    
    // Set the matrices
    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixIdentity( &matView );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 4.0f/3.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Save the default tiles
    m_pd3dDevice->GetTile( 0, &m_tile0 );
    m_pd3dDevice->GetTile( 1, &m_tile1 );

    // Create a secondary render target
    CreateSecondaryRenderTarget();
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: TilePrimaryColorBuffer()
// Desc: Tiles (or untiles) the primary color buffer. The primary color buffer
//       always uses tile 0.
//-----------------------------------------------------------------------------
VOID CXBoxSample::TilePrimaryColorBuffer( BOOL bTiled )
{
    m_pd3dDevice->SetTile( 0, NULL );

    if( bTiled )
        m_pd3dDevice->SetTile( 0, &m_tile0 );
}




//-----------------------------------------------------------------------------
// Name: TilePrimaryDepthBuffer()
// Desc: Tiles (or untiles) and compresses (or uncompresses) the primary
//       depth buffer. The primary depth buffer always uses tile 1.
//-----------------------------------------------------------------------------
VOID CXBoxSample::TilePrimaryDepthBuffer( BOOL bTiled, BOOL bCompressed )
{
    g_pd3dDevice->SetTile( 1, NULL );

    if( bTiled )
    {
        D3DTILE tile = m_tile1;

        if( FALSE == bCompressed )
            tile.Flags &= ~D3DTILE_FLAGS_ZCOMPRESS;

        m_pd3dDevice->SetTile( 1, &tile );
    }
}




//-----------------------------------------------------------------------------
// Name: TileSecondaryColorBuffer()
// Desc: Tiles (or untiles) the secondary color buffer.  This arbitrarily uses
//       tile 4.
//-----------------------------------------------------------------------------
VOID CXBoxSample::TileSecondaryColorBuffer( BOOL bTiled )
{
    g_pd3dDevice->SetTile( 3, NULL );

    if( bTiled )
    {
        D3DSURFACE_DESC desc;
        m_pSecondaryColorSurface->GetDesc( &desc );

        D3DTILE tile;
        ZeroMemory( &tile, sizeof(tile) );
        tile.Flags     = 0;
        tile.Pitch     = desc.Size / desc.Height;
        tile.pMemory   = m_pColorBufferMemory;
        tile.Size      = desc.Size;
        g_pd3dDevice->SetTile( 3, &tile );
    }
}




//-----------------------------------------------------------------------------
// Name: TileSecondaryDepthBuffer()
// Desc: Tiles (or untiles) and compresses (or uncompresses) the secondary
//       depth buffer. This arbitrarily uses tile 4.
//-----------------------------------------------------------------------------
VOID CXBoxSample::TileSecondaryDepthBuffer( BOOL bTiled, BOOL bCompressed )
{
    g_pd3dDevice->SetTile( 4, NULL );

    if( bTiled )
    {
        D3DSURFACE_DESC desc;
        m_pSecondaryDepthSurface->GetDesc( &desc );

        D3DTILE tile;
        ZeroMemory( &tile, sizeof(tile) );
        tile.Flags     = D3DTILE_FLAGS_ZBUFFER | D3DTILE_FLAGS_Z32BITS;
        tile.Pitch     = desc.Size / desc.Height;
        tile.pMemory   = m_pDepthBufferMemory;
        tile.Size      = desc.Size;

        if( bCompressed )
        {
            tile.Flags    |= D3DTILE_FLAGS_ZCOMPRESS;
            tile.ZStartTag = D3DTILE_ZENDTAG( &m_tile1 );
            tile.ZOffset   = 0;
        }

        g_pd3dDevice->SetTile( 4, &tile );
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Toggle the render target
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
    {
        m_bUseSecondaryRenderTarget = !m_bUseSecondaryRenderTarget;
    }

    // Toggle whether the primary color buffer is tiled
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
    {
        m_bPrimaryColorBufferTiled = !m_bPrimaryColorBufferTiled;
        TilePrimaryColorBuffer( m_bPrimaryColorBufferTiled );
    }

    // Toggle whether the primary depth buffer is tiled
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
    {
        if( m_bPrimaryDepthBufferTiled )
        {
            if( m_bPrimaryDepthBufferCompressed )
            {
                m_bPrimaryDepthBufferTiled      = FALSE;
                m_bPrimaryDepthBufferCompressed = FALSE;
            }
            else
            {
                m_bPrimaryDepthBufferTiled      = TRUE;
                m_bPrimaryDepthBufferCompressed = TRUE;
            }
        }
        else
        {
            m_bPrimaryDepthBufferTiled      = TRUE;
            m_bPrimaryDepthBufferCompressed = FALSE;
        }

        TilePrimaryDepthBuffer( m_bPrimaryDepthBufferTiled, m_bPrimaryDepthBufferCompressed );
    }

    // Toggle whether the secondary color buffer is tiled
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
        m_bSecondaryColorBufferTiled = !m_bSecondaryColorBufferTiled;
        TileSecondaryColorBuffer( m_bSecondaryColorBufferTiled );
    }

    // Toggle whether the secondary depth buffer is tiled
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
    {
        if( m_bSecondaryDepthBufferTiled )
        {
            if( m_bSecondaryDepthBufferCompressed )
            {
                m_bSecondaryDepthBufferTiled      = FALSE;
                m_bSecondaryDepthBufferCompressed = FALSE;
            }
            else
            {
                m_bSecondaryDepthBufferTiled      = TRUE;
                m_bSecondaryDepthBufferCompressed = TRUE;
            }
        }
        else
        {
            m_bSecondaryDepthBufferTiled      = TRUE;
            m_bSecondaryDepthBufferCompressed = FALSE;
        }

        TileSecondaryDepthBuffer( m_bSecondaryDepthBufferTiled, m_bSecondaryDepthBufferCompressed );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawBox()
// Desc: Helper function to draw an outlined, filled, screen-space rectangle
//-----------------------------------------------------------------------------
HRESULT DrawBox( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,
                 DWORD dwFillColor, DWORD dwOutlineColor )
{
    D3DXVECTOR4 v[4];
    v[0] = D3DXVECTOR4( x1-0.5f, y1-0.5f, 0, 0 );
    v[1] = D3DXVECTOR4( x2-0.5f, y1-0.5f, 0, 0 );
    v[2] = D3DXVECTOR4( x2-0.5f, y2-0.5f, 0, 0 );
    v[3] = D3DXVECTOR4( x1-0.5f, y2-0.5f, 0, 0 );

    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW );

    // Render the box
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwFillColor );
    g_pd3dDevice->DrawVerticesUP( D3DPT_QUADLIST, 4, v, sizeof(v[0]) );

    // Render the lines
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwOutlineColor );
    g_pd3dDevice->DrawVerticesUP( D3DPT_LINELOOP, 4, v, sizeof(v[0]) );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderStreaks()
// Desc: Render a bunch of streaks to eat up fill. Note: since this sample
//       tries to show off tiling compression of the depth buffer, it's 
//       important to have the zbuffer enabled.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderStreaks()
{
    // Update the streaks' main particles
    for( DWORD k=0; k<NUM_STREAKS; k++ )
    {
        FLOAT fTime = m_fStreakTimeScale[k]*m_fTime;
        FLOAT fX = fabsf( 2 * ( (fTime/2) - floorf(fTime/2) ) - 1 );
        FLOAT fY = fabsf( sinf( m_fStreakSinScale[k]*fTime ) );

        m_vParticleStreak[k][m_dwActiveStreakParticle].p = D3DXVECTOR4( 640*fX, 480*fY, 1.0f, 1.0f );
    }
    if( ++m_dwActiveStreakParticle >= NUM_PARTICLES_PER_STREAK )
        m_dwActiveStreakParticle = 0;

    // Fade the colors for the particles that make up the tail each streak
    for( DWORD i=0; i<NUM_PARTICLES_PER_STREAK; i++ )
    {
        int j = i + m_dwActiveStreakParticle;
        if( j>=NUM_PARTICLES_PER_STREAK )
            j -= NUM_PARTICLES_PER_STREAK;

        for( DWORD k=0; k<NUM_STREAKS; k++ )
            m_vParticleStreak[k][j].color = (0x00010101*i) & m_dwStreakColor[k];
    }

    // Render the streaks
    m_pd3dDevice->SetTexture( 3, m_pStreakTexture );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,         FtoDW(64.0f) );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );

    // Render the streaks
    for( k=0; k<NUM_STREAKS; k++ )
    {
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, NUM_PARTICLES_PER_STREAK, 
                                       m_vParticleStreak[k], sizeof(STREAKVERTEX) );
    }

    // Restore state
    m_pd3dDevice->SetTexture( 3, NULL );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // If we're rendering to the secondary render target (a texture), then 
    // set that now, before drawing anything.
    if( m_bUseSecondaryRenderTarget )
        m_pd3dDevice->SetRenderTarget( m_pSecondaryColorSurface, m_pSecondaryDepthSurface );

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Set default state
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );

    // Render the background skybox
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    m_SkyBoxObject.Render( m_pd3dDevice );

    // Render the streaks
    RenderStreaks();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
    {
        DrawBox( 59,  45, 571, 435, 0x40008000, 0xff008000 );
        m_Help.Render( &m_Font16, g_HelpCallouts, NUM_HELP_CALLOUTS );
    }
    else
    {
        // Draw boxes to contain text
        DrawBox( 59,  45, 571,  99, 0x40008000, 0xff008000 );
        DrawBox( 59, 106, 571, 435, 0x40008000, 0xff008000 );

        // Draw title and frame rate
        m_Font16.Begin();
        m_Font16.DrawText(  64, 50, 0xffffffff, L"Tiling" );
        m_Font16.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font16.End();

        // Draw the text output
        m_Font12.Begin();

        m_Font12.DrawText(  64, 75, 0xff00ffff, L"Press BACK for help" );

        // Whether rendering to a texture or to the backbuffer
        m_Font12.DrawText(  64, 114, 0xffffffff, L"Rendering to:" );
        if( m_bUseSecondaryRenderTarget )
            m_Font12.DrawText( 200, 114, 0xffffff00, L"Texture" );
        else
            m_Font12.DrawText( 200, 114, 0xffffff00, L"Backbuffer" );
            
        // Whether the render target is tiled
        m_Font12.DrawText(  64, 133, 0xffffffff, L"Render target:" );
        if( m_bUseSecondaryRenderTarget )
        {
            if( m_bSecondaryColorBufferTiled )
                m_Font12.DrawText( 200, 133, 0xffffff00, L"Tiled (using tile 3)" );
            else
                m_Font12.DrawText( 200, 133, 0xffffff00, L"Not tiled" );
        }
        else
        {
            if( m_bPrimaryColorBufferTiled )
                m_Font12.DrawText( 200, 133, 0xffffff00, L"Tiled (using tile 0)" );
            else
                m_Font12.DrawText( 200, 133, 0xffffff00, L"Not tiled" );
        }
            
        // Whether the depthbuffer is tiled and compressed
        m_Font12.DrawText(  64, 152, 0xffffffff, L"Depth buffer:" );
        if( m_bUseSecondaryRenderTarget )
        {
            if( m_bSecondaryDepthBufferTiled )
            {
                if( m_bSecondaryDepthBufferCompressed )
                    m_Font12.DrawText( 200, 152, 0xffffff00, L"Tiled (using tile 4), compressed" );
                else
                    m_Font12.DrawText( 200, 152, 0xffffff00, L"Tiled (using tile 4)" );
            }
            else
                m_Font12.DrawText( 200, 152, 0xffffff00, L"Not tiled" );
        }
        else
        {
            if( m_bPrimaryDepthBufferTiled )
            {
                if( m_bPrimaryDepthBufferCompressed )
                    m_Font12.DrawText( 200, 152, 0xffffff00, L"Tiled (using tile 1), compressed Z" );
                else
                    m_Font12.DrawText( 200, 152, 0xffffff00, L"Tiled (using tile 1)" );
            }
            else
                m_Font12.DrawText( 200, 152, 0xffffff00, L"Not tiled" );
        }
            
        // Dump info about all tiles
        FLOAT fCursorY = 209.0f;

        for( DWORD i=0; i<D3DTILE_MAXTILES; i++ )
        {
            D3DTILE tile;

            if( SUCCEEDED( m_pd3dDevice->GetTile( i, &tile ) ) )
            {
                WCHAR strBuffer[80];
                WCHAR strType[80] = L"(not used)";

                if( tile.Pitch )
                {
                    if( i==0 || i==1 )
                        wcscpy( strType, L"Primary" );
                    else
                        wcscpy( strType, L"Secondary" );

                    if( tile.Flags & D3DTILE_FLAGS_ZBUFFER )
                        wcscat( strType, L" depth buffer" );
                    else
                        wcscat( strType, L" color buffer" );

                    if( tile.Flags & D3DTILE_FLAGS_ZBUFFER )
                    {
                        if( tile.Flags & D3DTILE_FLAGS_Z32BITS )
                            wcscat( strType, L" ( 32-bit" );
                        else
                            wcscat( strType, L" ( 16-bit" );
                        if( tile.Flags & D3DTILE_FLAGS_ZCOMPRESS )
                            wcscat( strType, L" compressed Z" );
                        wcscat( strType, L" )" );
                    }
                }

                swprintf( strBuffer, L"Tile %d: %s", i, strType );
                m_Font12.DrawText( 64.0f, fCursorY, 0xffffffff, strBuffer );
                fCursorY+=19;
            }
        }
        m_Font12.End();
    }

    // If we rendered the scene to a secondary render target, then we now need
    // to render the texture to the primary backbuffer.
    if( m_bUseSecondaryRenderTarget )
    {
        // Set the render target back to be the app's main backbuffer
        m_pd3dDevice->SetRenderTarget( m_pBackBuffer, m_pDepthBuffer );

        // Render the secondary color surface to the screen
        struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };
        VERTEX v[4];
        v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0, 0 );  v[0].tu =   0; v[0].tv =   0;
        v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0, 0 );  v[1].tu = 640; v[1].tv =   0;
        v[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0, 0 );  v[2].tu = 640; v[2].tv = 480;
        v[3].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0, 0 );  v[3].tu =   0; v[3].tv = 480;
        m_pd3dDevice->SetTexture( 0, m_pSecondaryTexture );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,   FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, 6*sizeof(FLOAT) );
    }
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



