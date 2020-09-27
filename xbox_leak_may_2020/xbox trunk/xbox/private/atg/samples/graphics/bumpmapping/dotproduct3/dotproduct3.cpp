//-----------------------------------------------------------------------------
// File: DotProduct3.cpp
//
// Desc: D3D sample showing how to do bumpmapping using the DotProduct3 
//       texture operation.
// 
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       03.12.01 - Added Xfest art changes for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBResource.h>
#include <XBUtil.h>
#include <xgraphics.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"




//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Wood.bmp",          resource_Wood_OFFSET },
    { "FaceNormalMap.bmp", resource_FaceNormalMap_OFFSET },
    { NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move light" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Switch normal\nmap" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_1, L"Show normal map" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS 4




//-----------------------------------------------------------------------------
// Name: struct CUSTOMVERTEX
// Desc: 
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 p;
    DWORD       diffuse;
    DWORD       specular;
    FLOAT       tu, tv;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1)




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

    CUSTOMVERTEX       m_QuadVertices[4];
    LPDIRECT3DTEXTURE8 m_pCustomNormalMap;
    LPDIRECT3DTEXTURE8 m_pFileBasedNormalMap;
    LPDIRECT3DTEXTURE8 m_pWoodTexture;
    D3DXVECTOR3        m_vLight;

    BOOL               m_bUseFileBasedTexture;
    BOOL               m_bShowNormalMap;

    HRESULT CreateCustomNormalMap();

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
// Name: InitVertex()
// Desc: Initializes a vertex
//-----------------------------------------------------------------------------
VOID InitVertex( CUSTOMVERTEX* vtx, FLOAT x, FLOAT y, FLOAT z, FLOAT tu, FLOAT tv )
{
    D3DXVECTOR3 p(1,1,1);
    D3DXVec3Normalize( &p, &p );
    vtx[0].p        = D3DXVECTOR3( x, y, z );
    vtx[0].diffuse  = XBUtil_VectorToRGBA( &p, 1.0f );
    vtx[0].specular = 0x40400000;
    vtx[0].tu       = tu;
    vtx[0].tv       = tv;
}
    



//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_bDrawHelp            = FALSE;
    m_bUseFileBasedTexture = TRUE;
    m_bShowNormalMap       = FALSE;

    m_pCustomNormalMap     = NULL;
    m_pFileBasedNormalMap  = NULL;
    m_pWoodTexture         = NULL;

    InitVertex( &m_QuadVertices[0],-1.0f,-1.0f,-1.0f, 1.0f, 1.0f );
    InitVertex( &m_QuadVertices[1], 1.0f,-1.0f,-1.0f, 0.0f, 1.0f );
    InitVertex( &m_QuadVertices[2],-1.0f, 1.0f,-1.0f, 1.0f, 0.0f );
    InitVertex( &m_QuadVertices[3], 1.0f, 1.0f,-1.0f, 0.0f, 0.0f );

    m_vLight = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
}




//-----------------------------------------------------------------------------
// Name: CreateCustomNormalMap()
// Desc: Creates a custom normal map
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateCustomNormalMap()
{
    DWORD   dwWidth  = 512;
    DWORD   dwHeight = 512;
    HRESULT hr;

    // Create a 32-bit texture for the custom normal map
    hr = m_pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, 0, 
                                      D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
                                      &m_pCustomNormalMap );
    if( FAILED(hr) )
        return hr;

    // Lock the texture to fill it with our custom image
    D3DSURFACE_DESC desc;
    D3DLOCKED_RECT lock;
    m_pCustomNormalMap->GetLevelDesc( 0, &desc );
    m_pCustomNormalMap->LockRect( 0, &lock, 0, 0L );
    XBUtil_UnswizzleTexture2D( &lock, &desc );
    DWORD* pPixel = (DWORD*)lock.pBits;

    // Fill each pixel
    for( DWORD j=0; j<dwHeight; j++  )
    {
        for( DWORD i=0; i<dwWidth; i++ )
        {
            FLOAT xp = ( (5.0f*i) / (dwWidth-1)  );
            FLOAT yp = ( (5.0f*j) / (dwHeight-1) );
            FLOAT x  = 2*(xp-floorf(xp))-1;
            FLOAT y  = 2*(yp-floorf(yp))-1;
            FLOAT z  = sqrtf( 1.0f - x*x - y*y );

            // Make image of raised circle. Outside of circle is gray
            if( (x*x + y*y) <= 1.0f )
            {
                D3DXVECTOR3 vVector( x, y, z );
                *pPixel++ = XBUtil_VectorToRGBA( &vVector, 1.0f );
            }
            else
                *pPixel++ = 0x80808080;
        }
    }

    // Unlock the map and return successful
    XBUtil_SwizzleTexture2D( &lock, &desc );
    m_pCustomNormalMap->UnlockRect(0);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependant display objects.
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
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the file-based textures
    m_pFileBasedNormalMap = m_xprResource.GetTexture( "FaceNormalMap.bmp" );
    m_pWoodTexture        = m_xprResource.GetTexture( "Wood.bmp" );

    // Create the custom normal map
    if( FAILED( hr = CreateCustomNormalMap() ) )
        return hr;

    // Set the transform matrices
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, 2.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX  matWorld, matView, matProj;
    
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 500.0f );
    
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

    // Select options
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        m_bUseFileBasedTexture = !m_bUseFileBasedTexture;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        m_bShowNormalMap = !m_bShowNormalMap;

    // Move the light with the GamePad
    m_vLight.x += m_DefaultGamepad.fX1*0.1f; m_vLight.x *= 0.85f;
    m_vLight.y += m_DefaultGamepad.fY1*0.1f; m_vLight.y *= 0.85f;
    m_vLight.z  = 0.0f;

    if( D3DXVec3Length( &m_vLight ) > 1.0f )
        D3DXVec3Normalize( &m_vLight, &m_vLight );
    else
        m_vLight.z = sqrtf( 1.0f - m_vLight.x*m_vLight.x - m_vLight.y*m_vLight.y );

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
    // Clear the render target
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00000000, 1.0f, 0L );

    // Set render states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );

    // Set the textures
    if( m_bUseFileBasedTexture )
        m_pd3dDevice->SetTexture( 0, m_pFileBasedNormalMap );
    else
        m_pd3dDevice->SetTexture( 0, m_pCustomNormalMap );

    // If user wants to view the normal map itself, override the above
    // renderstates and simply show the texture
    if( TRUE == m_bShowNormalMap )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    }
    else
    {
        // Store the light vector, so it can be referenced in D3DTA_TFACTOR
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, XBUtil_VectorToRGBA( &m_vLight, 0.0f ) );

        // Modulate the texture (the normal map) with the light vector (stored
        // above in the texture factor)
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DOTPRODUCT3 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    }

    // Draw the bumpmapped quad
    m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_QuadVertices, 
                                   sizeof(CUSTOMVERTEX) );

    // Blend in the diffuse map
    if( m_bUseFileBasedTexture && FALSE == m_bShowNormalMap )
    {
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_SRCCOLOR );
        
        m_pd3dDevice->SetTexture( 0, m_pWoodTexture );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_QuadVertices, 
                                       sizeof(CUSTOMVERTEX) );

        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.DrawText(  64, 50, 0xffffffff, L"DotProduct3" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        if( m_bUseFileBasedTexture )
            m_Font.DrawText( 64, 75, 0xffffffff, L"Using file-based normal map" );
        else
            m_Font.DrawText( 64, 75, 0xffffffff, L"Using custom normal map" );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



