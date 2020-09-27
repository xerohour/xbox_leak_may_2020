//-----------------------------------------------------------------------------
// File: BumpLens.cpp
//
// Desc: Code to simulate a magnifying glass using bumpmapping.
//
// Hist: 03.27.00 - New for May XDK release
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
    { "Lake.bmp",    resource_Lake_OFFSET },
    { NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
struct BUMPVERTEX          // Vertex type used for bumpmap lens effect
{
    D3DXVECTOR3 p;
    FLOAT       tu, tv;
};

struct BACKGROUNDVERTEX    // Vertex type used for rendering background
{
    D3DXVECTOR4 p;
    FLOAT       tu, tv;
};

#define D3DFVF_BUMPVERTEX       (D3DFVF_XYZ|D3DFVF_TEX1)
#define D3DFVF_BACKGROUNDVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource       m_xprResource;        // Packed resources for the app
    CXBFont                 m_Font;               // Font class

    LPDIRECT3DVERTEXBUFFER8 m_pBackgroundVB;      // Vertices for background
    LPDIRECT3DVERTEXBUFFER8 m_pLensVB;            // Vertices for lens

    FLOAT                   m_fLensX;             // Position of lens
    FLOAT                   m_fLensY;

    LPDIRECT3DTEXTURE8      m_pBumpMapTexture;    // Bumpmap
    LPDIRECT3DTEXTURE8      m_pBackgroundTexture; // Envmap texture

    DWORD                   m_dwBumpPixelShader;  // Bumpenvmap pixel shader

    HRESULT CreateBumpMap( DWORD dwWidth, DWORD dwHeight );
    HRESULT InitPixelShader();

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

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
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Initialize members
    m_pBumpMapTexture    = NULL;
    m_pBackgroundTexture = NULL;

    m_pBackgroundVB      = NULL;
    m_pLensVB            = NULL;
}




//-----------------------------------------------------------------------------
// Name: InitPixelShader()
// Desc: Initialize and create our pixel shader
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitPixelShader()
{
    D3DPIXELSHADERDEF psd;
    ZeroMemory( &psd, sizeof(psd) );

    //---------------------------------------------------------------------------
    //  Texture configuration - The following members of the D3DPixelShaderDef   
    //  structure define the addressing modes of each of the four texture stages 
    //---------------------------------------------------------------------------
    psd.PSTextureModes = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D,
                                          PS_TEXTUREMODES_BUMPENVMAP,
                                          PS_TEXTUREMODES_NONE,
                                          PS_TEXTUREMODES_NONE );

    psd.PSDotMapping   = PS_DOTMAPPING( 0, 0, 0, 0 );
    psd.PSInputTexture = PS_INPUTTEXTURE( 0, 0, 0, 0 );
    psd.PSCompareMode  = PS_COMPAREMODE( 0, 0, 0, 0 );

    //---------------------------------------------------------------------------------
    //  Color combiners - The following members of the D3DPixelShaderDef structure     
    //  define the state for the eight stages of color combiners                       
    //---------------------------------------------------------------------------------
    psd.PSCombinerCount = PS_COMBINERCOUNT(
        1,
        PS_COMBINERCOUNT_MUX_LSB | PS_COMBINERCOUNT_SAME_C0 | PS_COMBINERCOUNT_SAME_C1);

    psd.PSRGBInputs[0] = PS_COMBINERINPUTS(
        PS_REGISTER_T1 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
        PS_REGISTER_ONE,
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO);

    psd.PSAlphaInputs[0] = PS_COMBINERINPUTS(
        PS_REGISTER_ONE | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,
        PS_REGISTER_ONE | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO);

    psd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY);
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY);

    // AB + (1-A)C + D
    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO,
        PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
        PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
        PS_REGISTER_ZERO);

    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO,
        PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,
        PS_FINALCOMBINERSETTING_CLAMP_SUM);

    return m_pd3dDevice->CreatePixelShader( &psd, &m_dwBumpPixelShader );
}




//-----------------------------------------------------------------------------
// Name: CreateBumpMap()
// Desc: Creates a bumpmap
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateBumpMap( DWORD dwWidth, DWORD dwHeight )
{
    // Create the bump map texture.
    if( FAILED( m_pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, 0,
                                             D3DFMT_V8U8, D3DPOOL_MANAGED,
                                             &m_pBumpMapTexture ) ) )
        return NULL;

    // Lock the surface and write in some bumps for the waves
    DWORD* pBits    = new DWORD[dwWidth*dwHeight];
    WORD*  pPixel16 = (WORD*)pBits;
    CHAR*  pPixel8  = (CHAR*)pBits;
    DWORD  mid      = dwWidth/2;

    for( DWORD y0 = 0; y0 < dwHeight; y0++ )
    {
        for( DWORD x0 = 0; x0 < dwWidth; x0++ )
        {
            DWORD x1 = ( (x0==dwWidth-1)  ? x0 : x0+1 );
            DWORD y1 = ( (x0==dwHeight-1) ? y0 : y0+1 );

            FLOAT fDistSq00 = (FLOAT)( (x0-mid)*(x0-mid) + (y0-mid)*(y0-mid) );
            FLOAT fDistSq01 = (FLOAT)( (x1-mid)*(x1-mid) + (y0-mid)*(y0-mid) );
            FLOAT fDistSq10 = (FLOAT)( (x0-mid)*(x0-mid) + (y1-mid)*(y1-mid) );

            FLOAT v00 = ( fDistSq00 > (mid*mid) ) ? 0.0f : sqrtf( (mid*mid) - fDistSq00 );
            FLOAT v01 = ( fDistSq01 > (mid*mid) ) ? 0.0f : sqrtf( (mid*mid) - fDistSq01 );
            FLOAT v10 = ( fDistSq10 > (mid*mid) ) ? 0.0f : sqrtf( (mid*mid) - fDistSq10 );

            FLOAT fDu = +(127/D3DX_PI)*atanf(v00-v01); // The delta-u bump value
            FLOAT fDv = -(127/D3DX_PI)*atanf(v00-v10); // The delta-v bump value

            BYTE du = (BYTE)( fDu >= 0.0f ? fDu : 256 + fDu );
            BYTE dv = (BYTE)( fDv >= 0.0f ? fDv : 256 + fDv );

            *pPixel16++ = (0xff00&(dv<<8)) | (0x00ff&(du<<0));

        }
    }

    D3DLOCKED_RECT lock;
    m_pBumpMapTexture->LockRect( 0, &lock, 0, 0 );
    XGSwizzleRect( pBits, lock.Pitch, NULL, lock.pBits, dwWidth, dwHeight, NULL, 2 );
    m_pBumpMapTexture->UnlockRect(0);

    delete pBits;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependant display objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the texture for the background image
    m_pBackgroundTexture = m_xprResource.GetTexture( "Lake.bmp" );

    // Create the bump map texture
    if( FAILED( CreateBumpMap( 256, 256 ) ) )
        return E_FAIL;

    // Create the pixel shader
    if( FAILED( InitPixelShader() ) )
        return E_FAIL;

    // Create a square for rendering the background
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4*sizeof(BACKGROUNDVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_BACKGROUNDVERTEX,
                                                  D3DPOOL_MANAGED, &m_pBackgroundVB ) ) )
        return E_FAIL;

    BACKGROUNDVERTEX* vBackground;
    m_pBackgroundVB->Lock( 0, 0, (BYTE**)&vBackground, 0 );
    vBackground[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0.9f, 1.0f );
    vBackground[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0.9f, 1.0f );
    vBackground[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0.9f, 1.0f );
    vBackground[3].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0.9f, 1.0f );
    vBackground[0].tu = 0.0f; vBackground[0].tv = 0.0f;
    vBackground[1].tu = 1.0f; vBackground[1].tv = 0.0f;
    vBackground[2].tu = 1.0f; vBackground[2].tv = 1.0f;
    vBackground[3].tu = 0.0f; vBackground[3].tv = 1.0f;
    m_pBackgroundVB->Unlock();

    // Create a square for rendering the lens
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4*sizeof(BUMPVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_BUMPVERTEX,
                                                  D3DPOOL_MANAGED, &m_pLensVB ) ) )
        return E_FAIL;

    BUMPVERTEX* vLens;
    m_pLensVB->Lock( 0, 0, (BYTE**)&vLens, 0 );
    vLens[0].p = D3DXVECTOR3(-256.0f, 256.0f, 0.0f );
    vLens[1].p = D3DXVECTOR3( 256.0f, 256.0f, 0.0f );
    vLens[2].p = D3DXVECTOR3( 256.0f,-256.0f, 0.0f );
    vLens[3].p = D3DXVECTOR3(-256.0f,-256.0f, 0.0f );
    vLens[0].tu = 0.0f; vLens[0].tv = 1.0f;
    vLens[1].tu = 1.0f; vLens[1].tv = 1.0f;
    vLens[2].tu = 1.0f; vLens[2].tv = 0.0f;
    vLens[3].tu = 0.0f; vLens[3].tv = 0.0f;
    m_pLensVB->Unlock();

    // Set the transform matrices
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -2000.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,     0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,     0.0f );
    D3DXMATRIX matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 3000.0f );
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
    // Bounce the lens about the screen
    m_fLensX = 2 * fabsf( 2 * ( (m_fAppTime/2) - floorf(m_fAppTime/2) ) - 1 ) - 1;
    m_fLensY = 2 * fabsf( sinf( m_fAppTime ) ) - 1;

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
    // This sample does not use the zbuffer or alpha
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );

    // Render the background
    m_pd3dDevice->SetTexture( 0, m_pBackgroundTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetVertexShader( D3DFVF_BACKGROUNDVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pBackgroundVB, sizeof(BACKGROUNDVERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );

    // Set states to render the lens
    m_pd3dDevice->SetTexture( 0, m_pBumpMapTexture );
    m_pd3dDevice->SetTexture( 1, m_pBackgroundTexture );
    m_pd3dDevice->SetPixelShader( m_dwBumpPixelShader );

    // Don't filer the bumpamp, otherwise, adjacent values of 0x00 and 0xff will
    // filter to some bogus intermediate value
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );

    // It is okay to filter the envmap, though
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

    // Clamp the envmap, so we don't bump past it's edge.
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU,   D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV,   D3DTADDRESS_CLAMP );

    // Set the bumpenv matrix, which scales and rotates the bumpmap. Here we
    // are just scaling the bumpmap down to produce reasonable bumps. (Since
    // the du and dv values in the bumpmap range from -1 to +1, this scale
    // keeps the max texture shifts in the range of -0.2 to +0.2.)
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT00, FtoDW(0.2f) );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT01, FtoDW(0.0f) );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT10, FtoDW(0.0f) );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_BUMPENVMAT11, FtoDW(0.2f) );

    // Generate texture coords depending on the lens' camera space position
    D3DXMATRIX mat;
    mat._11 = 0.5f; mat._12 = 0.0f;
    mat._21 = 0.0f; mat._22 =-0.5f;
    mat._31 = 0.0f; mat._32 = 0.0f;
    mat._41 = 0.5f; mat._42 = 0.5f;

    // Since the bumpenvmap texaddrop does not deal with projected textures,
    // we scale-by-z here in the texture transform
    D3DXMATRIX matView, matProj;
    m_pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
    D3DXVECTOR3 vEyePt( matView._41, matView._42, matView._43 );
    FLOAT       z = D3DXVec3Length( &vEyePt );
    mat._11 *= ( matProj._11 / ( matProj._33 * z + matProj._34 ) );
    mat._22 *= ( matProj._22 / ( matProj._33 * z + matProj._34 ) );
    
    m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &mat );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 0 );

    // Position the lens
    D3DXMATRIX matWorld;
    D3DXMatrixTranslation( &matWorld, 0.7f * (1000.0f-256.0f)*m_fLensX,
                                      0.7f * (1000.0f-256.0f)*m_fLensY,
                                      0.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Finally, render the bumpmap lens thingie
    m_pd3dDevice->SetVertexShader( D3DFVF_BUMPVERTEX );
    m_pd3dDevice->SetStreamSource( 0, m_pLensVB, sizeof(BUMPVERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );

    // Restore state
    m_pd3dDevice->SetPixelShader( NULL );

    // Show title, frame rate, and help
    m_Font.Begin();
    m_Font.DrawText(  64, 50, 0xffffffff, L"BumpLens" );
    m_Font.DrawText( 450, 50, 0xff808000, m_strFrameRate );
    m_Font.End();
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




