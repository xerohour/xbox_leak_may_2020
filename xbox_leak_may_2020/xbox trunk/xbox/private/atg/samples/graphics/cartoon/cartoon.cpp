//-----------------------------------------------------------------------------
// File: Cartoon.cpp
//
// Desc: Draws a model, simulating cartoon lighting by using a 1D texture map.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       01.10.00 - Changes for February XDK release
//       02.06.00 - Changes for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBMesh.h>
#include <XBHelp.h>
#include <xgraphics.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Move\nmodel" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Move\nlight" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// Name: class CXBCartoonMesh
// Desc: Overridden mesh class for rendering file-based meshes. The base mesh
//       class is overridden in order to replace the render callback function,
//       which here is used to pass values to the vertex shader.
//-----------------------------------------------------------------------------
class CXBCartoonMesh: public CXBMesh
{
public:
    BOOL RenderCallback( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwSubset,
                         XBMESH_SUBSET* pSubset, DWORD dwFlags );
};




//-----------------------------------------------------------------------------
// Name: RenderCallback()
// Desc: Called before each mesh subset is rendered, this function passes 
//       values to the cartoon vertex shader.
//-----------------------------------------------------------------------------
BOOL CXBCartoonMesh::RenderCallback( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwSubset,
                                     XBMESH_SUBSET* pSubset, DWORD dwFlags )
{
    // Set matrices for the vertex shader
    D3DXMATRIX matWorld, matView, matProj, matAll;
    pd3dDevice->GetTransform( D3DTS_WORLD,      &matWorld );
    pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
    pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
    D3DXMatrixMultiply( &matAll, &matWorld, &matView );
    D3DXMatrixMultiply( &matAll, &matAll, &matProj );
    D3DXMatrixTranspose( &matAll, &matAll );
    D3DXMatrixTranspose( &matWorld, &matWorld );
    pd3dDevice->SetVertexShaderConstant( 4, &matAll, 4 );
    pd3dDevice->SetVertexShaderConstant( 8, &matWorld, 4 );

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                 m_Font;
    CXBHelp                 m_Help;
    BOOL                    m_bDrawHelp;

    D3DXMATRIX              m_matWorld;         // Local transform of the model
    D3DXMATRIX              m_matLight;         // Transform for the light

    LPDIRECT3DTEXTURE8      m_pCartoonTexture;       // Texture surface
    DWORD                   m_dwCartoonVertexShader; // Custom vertex shader

    CXBCartoonMesh          m_Mesh;

    HRESULT CreateCartoonTexture();

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

    m_bDrawHelp         = FALSE;

    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixIdentity( &m_matLight );

    // Initial light direction used in the texture coordinate generation for
    // the cartoon effect.
    m_pCartoonTexture       = NULL;
    m_dwCartoonVertexShader = 0L;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a mesh (vertex and index buffers) for the model
    if( FAILED( m_Mesh.Create( m_pd3dDevice, "Models\\Robot.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set the view matrix
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-7.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX  matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Setup a yellow material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // The texture used by this renderer is procedurally generated.
    if( FAILED( CreateCartoonTexture() ) )
        return E_FAIL;

    // Create a vertex shader for doing the cartoon effect
    DWORD dwCartoonVertexDecl[MAX_FVF_DECL_SIZE];
    XBUtil_DeclaratorFromFVF( m_Mesh.GetMesh(0)->m_dwFVF, dwCartoonVertexDecl );

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Cartoon.xvu",
                                           dwCartoonVertexDecl,
                                           &m_dwCartoonVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateCartoonTexture()
// Desc: Creates the 1D texture map used as a modulation map based on vertex
//       lighting. This map holds quantized gray-scale values which are applied
//       to the underlying interpolated vertex color.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateCartoonTexture()
{
    if( FAILED( m_pd3dDevice->CreateTexture( 32, 1, 1, 0, 
                                             D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, 
                                             &m_pCartoonTexture ) ) )
        return E_FAIL;

    // Get the pointer to the bits.
    D3DLOCKED_RECT lock;
    m_pCartoonTexture->LockRect( 0, &lock, NULL, 0L );
    DWORD* pTextureData = (DWORD*)lock.pBits;

    for( int i = 0; i<32; i++ )
    {
        if( i<8 )       pTextureData[i] = 0xff606060;
        else if( i<18 ) pTextureData[i] = 0xffa2a2a2;
        else if( i<29 ) pTextureData[i] = 0xffe5e5e5;
        else            pTextureData[i] = 0xffffffff;
    }

    m_pCartoonTexture->UnlockRect( 0 );

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
    D3DXMATRIX matRotate;
    FLOAT fXRotate1 = m_DefaultGamepad.fX1*m_fElapsedTime*D3DX_PI*0.5f;
    FLOAT fYRotate1 = m_DefaultGamepad.fY1*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate1, -fYRotate1, 0.0f );
    D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matRotate );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    // Perform light rotation
    FLOAT fXRotate2 = fXRotate1 + m_DefaultGamepad.fX2*m_fElapsedTime*D3DX_PI*0.5f;
    FLOAT fYRotate2 = fYRotate1 - m_DefaultGamepad.fY2*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate2, -fYRotate2, 0.0f );
    D3DXMatrixMultiply( &m_matLight, &m_matLight, &matRotate );

    // Set constants for the vertex shader
    D3DXVECTOR4 vColor( 1.0f, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetVertexShaderConstant( 0, &vColor, 1 );

    // Set light position for the vertex shader
    D3DXMATRIX  matWorldInv, matLightInv;
    D3DXVECTOR3 vRotLight;
    D3DXVec3Normalize( &vRotLight, &D3DXVECTOR3( 3.0f, 1.0f, -8.0f ) );
    D3DXMatrixInverse( &matLightInv, NULL, &m_matLight );
    D3DXMatrixInverse( &matWorldInv, NULL, &m_matWorld );
    D3DXVec3TransformCoord( &vRotLight, &vRotLight, &m_matLight );
    D3DXVec3TransformCoord( &vRotLight, &vRotLight, &matWorldInv );
    m_pd3dDevice->SetVertexShaderConstant( 1, &vRotLight, 1 );

    // Set matrices for the vertex shader
    D3DXMATRIX matWorld, matView, matProj, matAll, matAllTranspose;
    m_pd3dDevice->GetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
    D3DXMatrixMultiply( &matAll, &matWorld, &matView );
    D3DXMatrixMultiply( &matAll, &matAll, &matProj );
    D3DXMatrixTranspose( &matAll, &matAll );
    m_pd3dDevice->SetVertexShaderConstant( 4, &matAll, 4 );

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
    RenderGradientBackground( 0xff0000ff, 0xff00ffff );

    // Set default states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,    TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,     0xffffffff );

    // Set filtering to point sampling; otherwise we lose the sharp
    // transistions in the lighting that we wanted.
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );

    // Set the texture coordinates to be clamped so we do not have to clamp the
    // result of the dot product in the texture gen code. We only really need
    // to clamp U because V is fixed at 0.0.
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    // Draw the first pass, the textured visible polygons. We want to modulate 
    // the texture with the diffuse at stage 0. The texture is a banded gray 
    // scale to give the lighting and the diffuse is used as specified in the
    // vertex because vertex lighting is disabled. This is a useful way to set
    // things up because the base color can be changed without having to
    // change render state and therefore without having to break primitives.
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    
    // Bind the cartoon texture and custom vertex shader
    m_pd3dDevice->SetTexture( 0, m_pCartoonTexture );
    m_pd3dDevice->SetVertexShader( m_dwCartoonVertexShader );

    // Draw the geometry
    m_Mesh.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOMATERIALS | XBMESH_NOTEXTURES );

    // Draw the second pass, the black silhouette. Basically, scale the world
    // matrix up a little and then redraw in black.
    D3DXMATRIX matScale;
    D3DXMatrixScaling( &matScale, 1.025f, 1.025f, 1.025f );
    D3DXMatrixMultiply( &matScale, &matScale, &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matScale );

    // When we draw the silhouette we derive it from the back-facing polygons; we
    // draw these polygons scaled through the above scale matrix. We should only
    // see black around edges where the culling order changes and therefore the
    // direction of the polygons changes.
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

    // Finally, before we draw the silhouette we want to set the color combiners
    // in such a way that they produce black with the minimal work and without
    // changing the diffuse color of the mesh. The simplest way is to use the
    // texture constant directly as a color; doing this also prevents any texture
    // reads.
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff000000 );

    // Turn off the texture and custom vertex shader
    m_pd3dDevice->SetTexture( 0, NULL );

    // Draw the geometry
    m_Mesh.Render( m_pd3dDevice, XBMESH_NOMATERIALS | XBMESH_NOTEXTURES );

    // Restore modified states
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"Cartoon" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




