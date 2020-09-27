//-----------------------------------------------------------------------------
// File: VolumeTexture.cpp
//
// Desc: Example code showing how to do volume textures in D3D.
//
// Hist: 12.15.00 - New for December XDK release
//       01.10.00 - Added texture swizzling
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>
#include <xgraphics.h>

// Dimension of volume texture
#define VOLTEXSIZE 64




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\nscene" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
struct VOLUMEVERTEX
{
    FLOAT      x, y, z;
    DWORD      color;
    FLOAT      tu, tv, tw;
};

#define D3DFVF_VOLUMEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0))

// Vertices for rendering a quad that is textured with the volume texture
VOLUMEVERTEX g_vVertices[4] =
{
    { 1.0f, 1.0f, 0.0f, 0xffffffff, 1.0f, 1.0f, 0.0f },
    {-1.0f, 1.0f, 0.0f, 0xffffffff, 0.0f, 1.0f, 0.0f },
    { 1.0f,-1.0f, 0.0f, 0xffffffff, 1.0f, 0.0f, 0.0f },
    {-1.0f,-1.0f, 0.0f, 0xffffffff, 0.0f, 0.0f, 0.0f }
};


// Vertices for rendering the wireframe box
FLOAT g_vWireframeBox[] =
{
    -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,
    
    -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                  m_Font;
    CXBHelp                  m_Help;
    BOOL                     m_bDrawHelp;

    LPDIRECT3DVOLUMETEXTURE8 m_pVolumeTexture;
    LPDIRECT3DVERTEXBUFFER8  m_pVB;

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
    m_bDrawHelp      = FALSE;
    m_pVolumeTexture = NULL;
    m_pVB            = NULL;
}


#if 0
typedef struct D3DVOLUME_DESC
{
    D3DFORMAT           Format;
    D3DRESOURCETYPE     Type;
    DWORD               Usage;
    UINT                Size;

    UINT                Width;
    UINT                Height;
    UINT                Depth;
} aD3DVOLUME_DESC;
#endif


//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
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

    // Create a volume texture
    hr = m_pd3dDevice->CreateVolumeTexture( VOLTEXSIZE, VOLTEXSIZE, VOLTEXSIZE, 
                                            1, 0, D3DFMT_A8R8G8B8, 
                                            D3DPOOL_DEFAULT, &m_pVolumeTexture );
    if( FAILED(hr) )
        return hr;

    // Fill the volume texture
    D3DVOLUME_DESC  desc;
    D3DLOCKED_BOX   lock;
    m_pVolumeTexture->GetLevelDesc( 0, &desc );
    m_pVolumeTexture->LockBox( 0, &lock, 0, 0L );
    DWORD* pBits = (DWORD*)lock.pBits;

    for( UINT w=0; w<VOLTEXSIZE; w++ )
    {
        for( UINT v=0; v<VOLTEXSIZE; v++ )
        {
            for( UINT u=0; u<VOLTEXSIZE; u++ )
            {
                FLOAT du = (2.0f*u)/(VOLTEXSIZE-1) - 1.0f;
                FLOAT dv = (2.0f*v)/(VOLTEXSIZE-1) - 1.0f;
                FLOAT dw = (2.0f*w)/(VOLTEXSIZE-1) - 1.0f;
                
                FLOAT fScale = 1.0f - sqrtf( du*du + dv*dv + dw*dw );
                if( fScale < 0.0f ) fScale = 0.0f;
                if( fScale > 1.0f ) fScale = 1.0f;

                DWORD r = (DWORD)(0xff*fScale);
                DWORD g = (DWORD)(0xff*fScale);
                DWORD b = (DWORD)(0xff*fScale);
                DWORD a = (DWORD)(0xff*fScale);
                if( a < 0x20 ) a = 0x20;

                // Write the texel
                *pBits++ = (a<<24) + (r<<16) + (g<<8) + (b<<0);
            }
        }
    }

    // Swizzle and unlock the texture
    XBUtil_SwizzleTexture3D( &lock, &desc );
    m_pVolumeTexture->UnlockBox( 0 );

    // Create a vertex buffer
    if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 4*sizeof(VOLUMEVERTEX), D3DUSAGE_WRITEONLY,
                                                       D3DFVF_VOLUMEVERTEX, D3DPOOL_DEFAULT, &m_pVB ) ) )
        return hr;

    VOLUMEVERTEX* pVertices;
    m_pVB->Lock( 0, 4*sizeof(VOLUMEVERTEX), (BYTE**)&pVertices, 0 );
    memcpy( pVertices, g_vVertices, sizeof(VOLUMEVERTEX)*4 );
    m_pVB->Unlock();

    // Set the matrices
    D3DXVECTOR3 vEye(-2.5f, 2.0f, -4.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEye,&vAt, &vUp );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
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

    // Perform object rotation
    static D3DXMATRIX matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
    D3DXMATRIX matRotate;
    FLOAT fXRotate1 = m_DefaultGamepad.fX1*m_fElapsedTime*D3DX_PI*0.5f;
    FLOAT fYRotate1 = m_DefaultGamepad.fY1*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate1, -fYRotate1, 0.0f );
    D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Play with the volume texture coordinate
    VOLUMEVERTEX* pVertices = NULL;
    m_pVB->Lock( 0, 4*sizeof(VOLUMEVERTEX), (BYTE**)&pVertices, 0 );
    for( int i=0; i<4; i++ )
    {
        pVertices[i].z  = sinf(2*m_fAppTime);
        pVertices[i].tw = ( 1.0f + pVertices[i].z ) / 2.0f;
    }
    m_pVB->Unlock();

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
    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Set default state
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );

    // Draw the wireframe box
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffff0000 );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 12, g_vWireframeBox, 
                                   sizeof(D3DXVECTOR3) );

    // Setup states for drawing the quad
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSW,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Draw the quad, with the volume texture
    m_pd3dDevice->SetTexture( 0, m_pVolumeTexture );
    m_pd3dDevice->SetVertexShader( D3DFVF_VOLUMEVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(VOLUMEVERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
		m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"VolumeTexture" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
		m_Font.End();
    }
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




