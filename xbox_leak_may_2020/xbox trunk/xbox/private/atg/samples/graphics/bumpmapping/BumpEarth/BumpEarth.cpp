//-----------------------------------------------------------------------------
// File: BumpEarth.cpp
//
// Desc: Direct3D environment mapping / bump mapping sample. The technique
//       used perturbs the environment map to simulate bump mapping.
//
// Hist: 03.28.00 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
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
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Toggle bumpmap" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, L"Toggle texture" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Block.bmp",       resource_Block_OFFSET },
    { "Earth.bmp",       resource_Earth_OFFSET },
    { "EarthBump.bmp",   resource_EarthBump_OFFSET },
    { "EarthEnvMap.bmp", resource_EarthEnvMap_OFFSET },
    { NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
struct BUMPVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};

#define D3DFVF_BUMPVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;       // Packed resources for the app
    CXBFont            m_Font;              // Font class
    CXBHelp            m_Help;              // Help class
    BOOL               m_bDrawHelp;         // Whether to draw help

    LPDIRECT3DTEXTURE8 m_pBlockTexture;     // A blank, gray texture
    LPDIRECT3DTEXTURE8 m_pEarthTexture;     // The Earth texture
    LPDIRECT3DTEXTURE8 m_pEnvMapTexture;    // The environment map
    LPDIRECT3DTEXTURE8 m_pEarthBumpTexture; // Source for the bumpmap
    LPDIRECT3DTEXTURE8 m_psBumpMap;         // The actual bumpmap

    D3DFORMAT          m_BumpMapFormat;         // Bumpmap texture format
    LPDIRECT3DVERTEXBUFFER8 m_pEarthVB;    // Geometry for the Earth
    DWORD              m_dwNumSphereVertices;

    BOOL               m_bTextureOn;
    BOOL               m_bBumpMapOn;
    BOOL               m_bEnvMapOn;

    DWORD              m_dwBumpPixelShader;  // Bumpenvmap pixel shader

    HRESULT InitBumpMap();
    HRESULT InitPixelShader();
    HRESULT CreateEarthVertexBuffer();

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
    m_bDrawHelp         = FALSE;
    m_psBumpMap         = NULL;
    m_bTextureOn        = TRUE;
    m_bBumpMapOn        = TRUE;
    m_bEnvMapOn         = TRUE;

    m_pBlockTexture     = NULL;
    m_pEarthTexture     = NULL;
    m_pEarthBumpTexture = NULL;
    m_pEnvMapTexture    = NULL;

    m_BumpMapFormat     = D3DFMT_L6V5U5;

    m_pEarthVB          = NULL;
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
                                          PS_TEXTUREMODES_PROJECT2D,
                                          PS_TEXTUREMODES_BUMPENVMAP_LUM,
                                          PS_TEXTUREMODES_NONE );

    psd.PSDotMapping   = PS_DOTMAPPING( 0, 0, 0, 0 );
    psd.PSInputTexture = PS_INPUTTEXTURE( 0, 0, 1, 0 );
    psd.PSCompareMode  = PS_COMPAREMODE( 0, 0, 0, 0 );

    //---------------------------------------------------------------------------------
    //  Color combiners - The following members of the D3DPixelShaderDef structure     
    //  define the state for the eight stages of color combiners                       
    //---------------------------------------------------------------------------------
    psd.PSCombinerCount = PS_COMBINERCOUNT(
        1,
        PS_COMBINERCOUNT_MUX_LSB | PS_COMBINERCOUNT_SAME_C0 | PS_COMBINERCOUNT_SAME_C1);

    psd.PSRGBInputs[0] = PS_COMBINERINPUTS(
        PS_REGISTER_T0  | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
        PS_REGISTER_ONE | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
        PS_REGISTER_T2  | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
        PS_REGISTER_ONE | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB );

    psd.PSAlphaInputs[0] = PS_COMBINERINPUTS(
        PS_REGISTER_ONE | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,
        PS_REGISTER_ONE | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO);

    psd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_R0,
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
// Name: InitBumpMap()
// Desc: Copies raw bits into the format of bump map requested
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitBumpMap()
{
    // Allocate src and dst bits
    D3DSURFACE_DESC desc;
    m_pEarthBumpTexture->GetLevelDesc( 0, &desc );
    VOID* pSrcBits = new DWORD[desc.Width*desc.Height];
    VOID* pDstBits;

    if( 2 == XGBytesPerPixelFromFormat(m_BumpMapFormat) )
        pDstBits = new WORD[desc.Width*desc.Height];
    else
        pDstBits = new DWORD[desc.Width*desc.Height];

    // Get the unswizzled src bits
    D3DLOCKED_RECT lock;
    m_pEarthBumpTexture->LockRect( 0, &lock, 0, 0 );
    XGUnswizzleRect( lock.pBits, desc.Width, desc.Height, NULL, pSrcBits, lock.Pitch, NULL, 4 );
    m_pEarthBumpTexture->UnlockRect(0);

    BYTE* pDstT  = (BYTE*)pDstBits;
    BYTE* pSrcB0 = (BYTE*)pSrcBits;
    BYTE* pSrcB1 = ( pSrcB0 + lock.Pitch );
    BYTE* pSrcB2 = ( pSrcB0 - lock.Pitch );

    for( DWORD y=0; y<desc.Height; y++ )
    {
        if( y == desc.Height-1 )  // Don't go past the last line
            pSrcB1 = pSrcB0;
        if( y == 0 )               // Don't go before first line
            pSrcB2 = pSrcB0;

        for( DWORD x=0; x<desc.Width; x++ )
        {
            LONG v00 = *(pSrcB0+0); // Get the current pixel
            LONG v01 = *(pSrcB0+4); // and the pixel to the right
            LONG vM1 = *(pSrcB0-4); // and the pixel to the left
            LONG v10 = *(pSrcB1+0); // and the pixel one line below.
            LONG v1M = *(pSrcB2+0); // and the pixel one line above.

            LONG iDu = (vM1-v01); // The delta-u bump value
            LONG iDv = (v1M-v10); // The delta-v bump value

            if( (v00 < vM1) && (v00 < v01) )  // If we are at valley
            {
                iDu = vM1-v00;                 // Choose greater of 1st order diffs
                if( iDu < v00-v01 )
                    iDu = v00-v01;
            }

            // The luminance bump value (land masses are less shiny)
            WORD uL = ( v00>1 ) ? 63 : 127;

            switch( m_BumpMapFormat )
            {
                case D3DFMT_V8U8:
                    *pDstT++ = (BYTE)iDu;
                    *pDstT++ = (BYTE)iDv;
                    break;

                case D3DFMT_L6V5U5:
                    *(WORD*)pDstT  = (WORD)( ( (iDu>>3) & 0x1f ) <<  0 );
                    *(WORD*)pDstT |= (WORD)( ( (iDv>>3) & 0x1f ) <<  5 );
                    *(WORD*)pDstT |= (WORD)( ( ( uL>>2) & 0x3f ) << 10 );
                    pDstT += 2;
                    break;

                case D3DFMT_X8L8V8U8:
                    *pDstT++ = (BYTE)iDu;
                    *pDstT++ = (BYTE)iDv;
                    *pDstT++ = (BYTE)uL;
                    *pDstT++ = (BYTE)0L;
                    break;
            }

            // Move one pixel to the left (src is 32-bpp)
            pSrcB0+=4;   
            pSrcB1+=4;   
            pSrcB2+=4;
        }
    }

    // Create the bumpmap's surface and texture objects
    if( FAILED( m_pd3dDevice->CreateTexture( desc.Width, desc.Height, 1, 0, 
                                             m_BumpMapFormat, D3DPOOL_MANAGED, 
                                             &m_psBumpMap ) ) )
        return E_FAIL;

    m_psBumpMap->LockRect( 0, &lock, 0, 0 );
    XGSwizzleRect( pDstBits, lock.Pitch, NULL, lock.pBits, desc.Width, desc.Height, 
                   NULL, XGBytesPerPixelFromFormat(m_BumpMapFormat) );
    m_psBumpMap->UnlockRect(0);

    delete pSrcBits;
    delete pDstBits;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateEarthVertexBuffer()
// Desc: Sets up the vertices for a bump-mapped sphere.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateEarthVertexBuffer()
{
    DWORD dwNumSphereRings    = 15;
    DWORD dwNumSphereSegments = 30;
    m_dwNumSphereVertices = 2 * dwNumSphereRings * (dwNumSphereSegments+1);

    // Create the vertex buffer
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( m_dwNumSphereVertices*sizeof(BUMPVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_BUMPVERTEX,
                                                  D3DPOOL_MANAGED, &m_pEarthVB ) ) )
        return E_FAIL;

    // Lock the vertex buffer
    BUMPVERTEX* vtx;
    m_pEarthVB->Lock( 0, 0, (BYTE**)&vtx, 0 );

    // Establish constants used in sphere generation
    FLOAT fDeltaRingAngle = ( D3DX_PI / dwNumSphereRings );
    FLOAT fDeltaSegAngle  = ( 2.0f * D3DX_PI / dwNumSphereSegments );

    // Generate the group of rings for the sphere
    for( DWORD ring = 0; ring < dwNumSphereRings; ring++ )
    {
        FLOAT r0 = sinf( (ring+0) * fDeltaRingAngle );
        FLOAT r1 = sinf( (ring+1) * fDeltaRingAngle );
        FLOAT y0 = cosf( (ring+0) * fDeltaRingAngle );
        FLOAT y1 = cosf( (ring+1) * fDeltaRingAngle );

        // Generate the group of segments for the current ring
        for( DWORD seg = 0; seg < (dwNumSphereSegments+1); seg++ )
        {
            FLOAT x0 = sinf( seg * fDeltaSegAngle );
            FLOAT z0 = cosf( seg * fDeltaSegAngle );
            FLOAT x1 = sinf( seg * fDeltaSegAngle );
            FLOAT z1 = cosf( seg * fDeltaSegAngle );

            // Add two vertices to the strip which makes up the sphere
            // (using the transformed normal to generate texture coords)
            (*vtx).p  = D3DXVECTOR3(r0*x0,y0,r0*z0);
            (*vtx).n  = D3DXVECTOR3(x0,y0,z0);
            (*vtx).tu = -((FLOAT)seg)/dwNumSphereSegments;
            (*vtx).tv = (ring+0)/(FLOAT)dwNumSphereRings;
            vtx++;

            (*vtx).p  = D3DXVECTOR3(r1*x1,y1,r1*z1);
            (*vtx).n  = D3DXVECTOR3(x1,y1,z1);
            (*vtx).tu = -((FLOAT)seg)/dwNumSphereSegments;
            (*vtx).tv = (ring+1)/(FLOAT)dwNumSphereRings;
            vtx++;
        }
    }

    m_pEarthVB->Unlock();

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

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the file-based textures
    m_pBlockTexture     = m_xprResource.GetTexture( "Block.bmp" );
    m_pEarthTexture     = m_xprResource.GetTexture( "Earth.bmp" );
    m_pEarthBumpTexture = m_xprResource.GetTexture( "EarthBump.bmp" );
    m_pEnvMapTexture    = m_xprResource.GetTexture( "EarthEnvMap.bmp" );

    // Initialize earth geometry
    if( FAILED( CreateEarthVertexBuffer() ) )
        return E_FAIL;

    // Create and fill the bumpmap
    if( FAILED( InitBumpMap() ) )
        return E_FAIL;

    // Create the pixel shader
    if( FAILED( InitPixelShader() ) )
        return E_FAIL;

    // Set the view matrix
    D3DXMATRIX  matView;
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 1.0f,-4.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 25.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Animates the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Toggle options
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] ) 
    {
        m_bBumpMapOn    = !m_bBumpMapOn;
        m_bEnvMapOn     = !m_bEnvMapOn;
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ) 
    {
        m_bTextureOn  = !m_bTextureOn;
    }

    // Update the Earth's rotation angle
    D3DXMATRIX matWorld;
    D3DXMatrixRotationY( &matWorld, -m_fAppTime );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Set default state
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    // Set the textures
    m_pd3dDevice->SetTexture( 0, m_bTextureOn ? m_pEarthTexture  : m_pBlockTexture );
    m_pd3dDevice->SetTexture( 1, m_bBumpMapOn ? m_psBumpMap      : m_pBlockTexture );
    m_pd3dDevice->SetTexture( 2, m_bEnvMapOn  ? m_pEnvMapTexture : m_pBlockTexture );

    // The base texture (stage 0) and bumpmap (stage 1) both use the model's
    // texture coordinates
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

    // Generate spheremapping coords for the environment map on stage 2
    D3DXMATRIX mat;
    mat._11 = 0.5f;   mat._12 = 0.0f;
    mat._21 = 0.0f;   mat._22 =-0.5f;
    mat._31 = 0.0f;   mat._32 = 0.0f;
    mat._41 = 0.5f;   mat._42 = 0.5f;
    m_pd3dDevice->SetTransform( D3DTS_TEXTURE2, &mat );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

    // Enable filtering. It's important not to filter the bumpmap, though,
    // since + and - values would filter incorrectly.
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

    // Set the bumpenvmap matrix and luminance scale values
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT00,   FtoDW(0.5f) );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT01,   FtoDW(0.0f) );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT10,   FtoDW(0.0f) );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVMAT11,   FtoDW(0.5f) );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVLSCALE,  FtoDW(4.0f) );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_BUMPENVLOFFSET, FtoDW(0.0f) );

    // Finally, draw the bumpmapped Earth
    m_pd3dDevice->SetStreamSource( 0, m_pEarthVB, sizeof(BUMPVERTEX) );
    m_pd3dDevice->SetVertexShader( D3DFVF_BUMPVERTEX );
    m_pd3dDevice->SetPixelShader( m_dwBumpPixelShader );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, m_dwNumSphereVertices-2 );

    // Restore state
    m_pd3dDevice->SetPixelShader( NULL );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        // Show title, frame rate, and help
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"BumpEarth" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Output description of what's being displayed
        m_Font.DrawText( 64,  95, 0xffffff00, m_bTextureOn ? L"Texture ON" : L"Texture OFF" );
        m_Font.DrawText( 64, 120, 0xffffff00, m_bBumpMapOn ? L"Bumpmap ON" : L"Bumpmap OFF" );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




