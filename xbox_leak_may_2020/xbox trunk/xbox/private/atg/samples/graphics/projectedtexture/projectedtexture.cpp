//-----------------------------------------------------------------------------
// File: ProjectedTexture.cpp
//
// Desc: Sample to show off projected textures
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>
#include <xgraphics.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\ncube" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Move projection\ndirection\n" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Show\nfrustrum" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_2, L"W/left trigger, the right stick\nmoves projection point" },
};

#define NUM_HELP_CALLOUTS 5




//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------

struct PROJTEXVERTEX
{
    D3DXVECTOR3 p;
    D3DCOLOR    color;
};

#define D3DFVF_PROJTEXVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)




//----------------------------------------------------------------------------
// projection frustrum
struct LINEVERTEX
{
    FLOAT x, y, z;
    DWORD color;
};

LINEVERTEX g_FrustumLines[] =
{
    {  0.0f, 0.0f, 0.0f, 0xffffffff },
    {  1.0f, 1.0f, 1.0f, 0xffffffff },

    {  0.0f, 0.0f, 0.0f, 0xffffffff },
    { -1.0f, 1.0f, 1.0f, 0xffffffff },

    {  0.0f, 0.0f, 0.0f, 0xffffffff },
    {  1.0f,-1.0f, 1.0f, 0xffffffff },

    {  0.0f, 0.0f, 0.0f, 0xffffffff },
    { -1.0f,-1.0f, 1.0f, 0xffffffff },
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                m_Font;
    CXBHelp                m_Help;
    BOOL                   m_bDrawHelp;

    BOOL                   m_bDrawFrustrum;

    PROJTEXVERTEX          m_vCube[36];      // Mesh vertex data
    LPDIRECT3DTEXTURE8     m_pSpotLightTexture;   // procedurally generated spotlight texture

    D3DXMATRIX             m_matTexProj;   // texture projection matrix

    D3DXVECTOR3            m_vTexEyePt;    // texture eye pt.
    D3DXVECTOR3            m_vTexLookatPt; // texture lookat pt.

    VOID    CalculateTexProjMatrix();
    HRESULT CreateSpotLightTexture( BOOL bBorder );
    VOID    ShowTexture( LPDIRECT3DTEXTURE8 pTexture );

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
    m_bDrawHelp         = FALSE;

    m_pSpotLightTexture = NULL;

    m_vTexEyePt    = D3DXVECTOR3(-2.0f, -2.0f, -7.0f );
    m_vTexLookatPt = D3DXVECTOR3( 0.0f,  0.0f,  0.0f );

    // Cube vertices
    m_vCube[ 0].p = D3DXVECTOR3( 2.0f, 2.0f,-2.0f); m_vCube[ 0].color = 0xff0080ff;
    m_vCube[ 1].p = D3DXVECTOR3(-2.0f, 2.0f,-2.0f); m_vCube[ 1].color = 0xff0080ff;
    m_vCube[ 2].p = D3DXVECTOR3(-2.0f, 2.0f, 2.0f); m_vCube[ 2].color = 0xff0080ff;
    m_vCube[ 3].p = D3DXVECTOR3( 2.0f, 2.0f, 2.0f); m_vCube[ 3].color = 0xff0080ff;
    m_vCube[ 4].p = D3DXVECTOR3( 2.0f, 2.0f,-2.0f); m_vCube[ 4].color = 0xff0080ff;
    m_vCube[ 5].p = D3DXVECTOR3(-2.0f, 2.0f, 2.0f); m_vCube[ 5].color = 0xff0080ff;

    m_vCube[ 6].p = D3DXVECTOR3(-2.0f,-2.0f,-2.0f); m_vCube[ 6].color = 0xffff00ff;
    m_vCube[ 7].p = D3DXVECTOR3( 2.0f,-2.0f,-2.0f); m_vCube[ 7].color = 0xffff00ff;
    m_vCube[ 8].p = D3DXVECTOR3( 2.0f,-2.0f, 2.0f); m_vCube[ 8].color = 0xffff00ff;
    m_vCube[ 9].p = D3DXVECTOR3(-2.0f,-2.0f, 2.0f); m_vCube[ 9].color = 0xffff00ff;
    m_vCube[10].p = D3DXVECTOR3(-2.0f,-2.0f,-2.0f); m_vCube[10].color = 0xffff00ff;
    m_vCube[11].p = D3DXVECTOR3( 2.0f,-2.0f, 2.0f); m_vCube[11].color = 0xffff00ff;

    m_vCube[12].p = D3DXVECTOR3( 2.0f,-2.0f,-2.0f); m_vCube[12].color = 0xff00ff00;
    m_vCube[13].p = D3DXVECTOR3( 2.0f, 2.0f,-2.0f); m_vCube[13].color = 0xff00ff00;
    m_vCube[14].p = D3DXVECTOR3( 2.0f, 2.0f, 2.0f); m_vCube[14].color = 0xff00ff00;
    m_vCube[15].p = D3DXVECTOR3( 2.0f,-2.0f, 2.0f); m_vCube[15].color = 0xff00ff00;
    m_vCube[16].p = D3DXVECTOR3( 2.0f,-2.0f,-2.0f); m_vCube[16].color = 0xff00ff00;
    m_vCube[17].p = D3DXVECTOR3( 2.0f, 2.0f, 2.0f); m_vCube[17].color = 0xff00ff00;

    m_vCube[18].p = D3DXVECTOR3(-2.0f, 2.0f,-2.0f); m_vCube[18].color = 0xff808000;
    m_vCube[19].p = D3DXVECTOR3(-2.0f,-2.0f,-2.0f); m_vCube[19].color = 0xff808000;
    m_vCube[20].p = D3DXVECTOR3(-2.0f,-2.0f, 2.0f); m_vCube[20].color = 0xff808000;
    m_vCube[21].p = D3DXVECTOR3(-2.0f, 2.0f, 2.0f); m_vCube[21].color = 0xff808000;
    m_vCube[22].p = D3DXVECTOR3(-2.0f, 2.0f,-2.0f); m_vCube[22].color = 0xff808000;
    m_vCube[23].p = D3DXVECTOR3(-2.0f,-2.0f, 2.0f); m_vCube[23].color = 0xff808000;

    m_vCube[24].p = D3DXVECTOR3( 2.0f,-2.0f,-2.0f); m_vCube[24].color = 0xff0000ff;
    m_vCube[25].p = D3DXVECTOR3(-2.0f,-2.0f,-2.0f); m_vCube[25].color = 0xff0000ff;
    m_vCube[26].p = D3DXVECTOR3(-2.0f, 2.0f,-2.0f); m_vCube[26].color = 0xff0000ff;
    m_vCube[27].p = D3DXVECTOR3( 2.0f, 2.0f,-2.0f); m_vCube[27].color = 0xff0000ff;
    m_vCube[28].p = D3DXVECTOR3( 2.0f,-2.0f,-2.0f); m_vCube[28].color = 0xff0000ff;
    m_vCube[29].p = D3DXVECTOR3(-2.0f, 2.0f,-2.0f); m_vCube[29].color = 0xff0000ff;

    m_vCube[30].p = D3DXVECTOR3(-2.0f,-2.0f, 2.0f); m_vCube[30].color = 0xffff0000;
    m_vCube[31].p = D3DXVECTOR3( 2.0f,-2.0f, 2.0f); m_vCube[31].color = 0xffff0000;
    m_vCube[32].p = D3DXVECTOR3( 2.0f, 2.0f, 2.0f); m_vCube[32].color = 0xffff0000;
    m_vCube[33].p = D3DXVECTOR3(-2.0f, 2.0f, 2.0f); m_vCube[33].color = 0xffff0000;
    m_vCube[34].p = D3DXVECTOR3(-2.0f,-2.0f, 2.0f); m_vCube[34].color = 0xffff0000;
    m_vCube[35].p = D3DXVECTOR3( 2.0f, 2.0f, 2.0f); m_vCube[35].color = 0xffff0000;
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

    // Set the view matrix
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-15.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXMATRIX  matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up a point light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 0.0f, -10.0f );
    light.Attenuation0 = 0.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 1.0f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00333333 );

    // Set a default white material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Create the texture for our procedural spotlight texmap
    m_bDrawFrustrum = TRUE;
    CreateSpotLightTexture( m_bDrawFrustrum );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateSpotLightTexture()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateSpotLightTexture( BOOL bBorder )
{
    // Release the old texture
    SAFE_RELEASE( m_pSpotLightTexture );

    // Create a new texture
    m_pd3dDevice->CreateTexture( 128, 128, 1, 0, D3DFMT_X8R8G8B8, 
                                 D3DPOOL_DEFAULT, &m_pSpotLightTexture );

    D3DLOCKED_RECT lock;
    m_pSpotLightTexture->LockRect( 0, &lock, NULL, 0L );

    D3DSURFACE_DESC desc;
    m_pSpotLightTexture->GetLevelDesc( 0, &desc );
    DWORD* pBits    = (DWORD*)lock.pBits;
    DWORD* pLine    = (DWORD*)lock.pBits;
    DWORD  dwStride = lock.Pitch / sizeof(DWORD);

    // Generate the spotlight tex
    for( DWORD y = 0; y < 128; y++ )
    {
        for( DWORD x = 0; x < 128; x++ )
        {
            FLOAT dx = ( 64.0f - x + 0.5f ) / 64.0f;
            FLOAT dy = ( 64.0f - y + 0.5f ) / 64.0f;
            FLOAT r  = cosf( sqrtf( dx*dx + dy*dy ) * D3DX_PI / 2.0f );
            r = (r > 0.0f) ? r*r : 0.0f;
            int c = (int)min((r*r + 0.15f) * 0xff, 0xff);

            pLine[x] = 0xff000000 + 0x00010101*c;
        }

        pLine += dwStride;
    }

    if( bBorder )
    {
        // Draw border around spotlight texture
        DWORD* pLineX1 = (DWORD*)pBits + dwStride;
        DWORD* pLineX2 = (DWORD*)pBits + dwStride * (128 - 2);
        DWORD* pLineY1 = (DWORD*)pBits + 1;
        DWORD* pLineY2 = (DWORD*)pBits + dwStride - 2;

        for( DWORD x = 0; x < 128; x++ )
        {
            if( x != 0 && x != 127 )
            {
                *pLineX1 = *pLineX2 = 0xffffffff;
                *pLineY1 = *pLineY2 = 0xffffffff;
            }

            pLineX1 += 1;
            pLineX2 += 1;
            pLineY1 += dwStride;
            pLineY2 += dwStride;
        }
    }

    // Swizzle and unlock the texture
    XBUtil_SwizzleTexture2D( &lock, &desc );
    m_pSpotLightTexture->UnlockRect(0);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ShowTexture()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::ShowTexture( LPDIRECT3DTEXTURE8 pTexture )
{
    D3DSURFACE_DESC d3dsd;
    pTexture->GetLevelDesc( 0, &d3dsd );
    FLOAT x1 = 50.0f, x2 = x1 + (FLOAT)d3dsd.Width;
    FLOAT y1 = 50.0f, y2 = x1 + (FLOAT)d3dsd.Height;

    struct SPRITEVERTEX
    {
        FLOAT sx, sy, sz, rhw;
        FLOAT tu, tv;
    };
    
    SPRITEVERTEX vSprite[4] =
    {
        { x1-0.5f, y1-0.5f, 0.99f, 1.0f,  0.0f, 0.0f },
        { x2-0.5f, y1-0.5f, 0.99f, 1.0f,  1.0f, 0.0f },
        { x2-0.5f, y2-0.5f, 0.99f, 1.0f,  1.0f, 1.0f },
        { x1-0.5f, y2-0.5f, 0.99f, 1.0f,  0.0f, 1.0f },
    };

    // Set state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,    D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Display the sprite
    m_pd3dDevice->SetTexture( 0, pTexture );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vSprite, sizeof(SPRITEVERTEX) );
}




//-----------------------------------------------------------------------------
// Name: CalculateTexProjMatrix()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::CalculateTexProjMatrix()
{
    // Get the current view matrix
    D3DXMATRIX matView, matInvView;
    m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
    D3DXMatrixInverse( &matInvView,  NULL, &matView );

    // Compute view matrix
    D3DXMATRIX matTexView;
    D3DXMatrixLookAtLH( &matTexView, &m_vTexEyePt, &m_vTexLookatPt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
    
    // Compute projection matrix
    D3DXMATRIX matTexProj;
    D3DXMatrixPerspectiveFovLH( &matTexProj, D3DX_PI/6, 1.0f, 1.0f, 200.0f );

    D3DXMATRIX matScale;
    D3DXMatrixIdentity( &matScale );
    matScale._11 = 0.5f;
    matScale._22 = 0.5f;

    D3DXMATRIX matBias;
    D3DXMatrixIdentity( &matBias );
    matBias._31 = 0.5f;
    matBias._32 = 0.5f;

    D3DXMatrixIdentity( &m_matTexProj );
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matInvView ); // Transform cameraspaceposition to worldspace
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matTexView ); // Transform to worldspace
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matTexProj ); // Situate verts relative to projector's view
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matScale );   // Scale and bias to map the near clipping plane to texcoords
    D3DXMatrixMultiply( &m_matTexProj, &m_matTexProj, &matBias );    // Scale and bias to map the near clipping plane to texcoords

    // Convert from homogeneous texmap coords to worldspace
    D3DXMATRIX matInvTexView, matInvTexProj;
    D3DXMatrixInverse( &matInvTexView, NULL, &matTexView );
    D3DXMatrixInverse( &matInvTexProj, NULL, &matTexProj );          

    for( int i = 0; i < 8; i++ )
    {
        D3DXVECTOR4 vT( 0.0f, 0.0f, 0.0f, 1.0f );
        vT.x = (i%2) * ( i&0x2 ? -1.0f : 1.0f );
        vT.y = (i%2) * ( i&0x4 ? -1.0f : 1.0f );
        vT.z = (i%2) * ( 1.0f );

        D3DXVec4Transform( &vT, &vT, &matInvTexProj );
        D3DXVec4Transform( &vT, &vT, &matInvTexView );

        g_FrustumLines[i].x = vT.x / vT.w;
        g_FrustumLines[i].y = vT.y / vT.w;
        g_FrustumLines[i].z = vT.z / vT.w;
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

    // Toggle drawing of the projection frustum
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
    {
        m_bDrawFrustrum = !m_bDrawFrustrum;
        CreateSpotLightTexture( m_bDrawFrustrum );
    }

    // Perform a basic arcball rotation
    static D3DXMATRIX matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
    D3DXMATRIX matRotate;
    FLOAT fXRotate = m_DefaultGamepad.fX1 * m_fElapsedTime * D3DX_PI * 0.5f;
    FLOAT fYRotate = m_DefaultGamepad.fY1 * m_fElapsedTime * D3DX_PI * 0.5f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate, -fYRotate, 0.0f );
    D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    
    if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] )
    {
        // Move texture-projection eye point
        m_vTexEyePt.x += m_DefaultGamepad.fX2 * m_fElapsedTime * 5.0f;
        m_vTexEyePt.y += m_DefaultGamepad.fY2 * m_fElapsedTime * 5.0f;
    }
    else
    {
        // Move where the texture-projection is being projected to
        m_vTexLookatPt.x += m_DefaultGamepad.fX2 * m_fElapsedTime * 5.0f;
        m_vTexLookatPt.y += m_DefaultGamepad.fY2 * m_fElapsedTime * 5.0f;
    }

    // Recalculate our texture projection matrix
    CalculateTexProjMatrix();

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
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );

    // Draw our SpotLight texture in upper left
    if( m_bDrawFrustrum )
        ShowTexture( m_pSpotLightTexture );

    // Set up texture and states to draw the projected spotlight
    m_pd3dDevice->SetTexture( 1, m_pSpotLightTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

    // Enable texture transforms
    m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &m_matTexProj );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,         D3DTSS_TCI_CAMERASPACEPOSITION );

    // Draw the cube
    m_pd3dDevice->SetVertexShader( D3DFVF_PROJTEXVERTEX );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 12, 
                                   m_vCube, sizeof(PROJTEXVERTEX) );
    
    // Draw texture projection frustrum
    if( m_bDrawFrustrum )
    {
        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_DIFFUSE );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 4, g_FrustumLines, 
                                       sizeof(g_FrustumLines[0]) );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"ProjectedTexture" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




