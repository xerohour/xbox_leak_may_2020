//-----------------------------------------------------------------------------
// File: VolumeFog.cpp
//
// Desc: Example of fogging using an arbitrary fog volume.
//
// Hist: 02.21.00 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBUtil.h>

#include "D3D8Perf.h"

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
    { "TreeBark.bmp", Resource_Treebark_OFFSET },
    { "VolGroun.bmp", Resource_VolGroun_OFFSET },
    { "SkyBoxXP.bmp", Resource_SkyBoxXP_OFFSET },
    { "SkyBoxXN.bmp", Resource_SkyBoxXN_OFFSET },
    { "SkyBoxYP.bmp", Resource_SkyBoxYP_OFFSET },
    { "SkyBoxYN.bmp", Resource_SkyBoxYN_OFFSET },
    { "SkyBoxZP.bmp", Resource_SkyBoxZP_OFFSET },
    { "SkyBoxZN.bmp", Resource_SkyBoxZN_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move camera" },
    { XBHELP_LEFT_BUTTON,  XBHELP_PLACEMENT_1, L"Move Out" },
    { XBHELP_RIGHT_BUTTON, XBHELP_PLACEMENT_1, L"Move In" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts)/sizeof(g_HelpCallouts[0]))




//-----------------------------------------------------------------------------
// Custom vertex types
//-----------------------------------------------------------------------------
struct FOGVERT
{
    FOGVERT(const D3DXVECTOR4& pos, float u1, float v1, float u2, float v2)
    {
        Pos = pos;
        U1 = u1;
        V1 = v1;
        U2 = u2;
        V2 = v2;
    }

    D3DXVECTOR4 Pos;
    float U1, V1;
    float U2, V2;
    float S3, T3, R3, Q3;
    float S4, T4, R4, Q4;
};


FOGVERT g_FogVerts[] =
{
    FOGVERT( D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0.0f, 1.0f ),   0,   0,   0,   0 ),
    FOGVERT( D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0.0f, 1.0f ), 640,   0, 640,   0 ),
    FOGVERT( D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0.0f, 1.0f ), 640, 480, 640, 480 ),
    FOGVERT( D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0.0f, 1.0f ),   0, 480,   0, 480 ),
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;        // Packed resources for the app
    CXBFont            m_Font;               // Font class
    CXBHelp            m_Help;               // Help class
    BOOL               m_bDrawHelp;          // Whether to draw help

    CXBMesh            m_Room;               // XBG file object to render
    CXBMesh            m_FogVolume;          // Fog hull.

    CXBMesh            m_SkyboxObject;

    float              m_fFogVolumeRadius;   // Radius of the fog volume.

    D3DXVECTOR3        m_vEye;
    D3DXMATRIX         m_matWorld;
    D3DXMATRIX         m_matView;
    D3DXMATRIX         m_matProjection;

    DWORD              m_dwFogVertexShader;
    DWORD              m_dwFogPixelShader;

    IDirect3DTexture8 *m_pFogBufferNear;
    IDirect3DTexture8 *m_pFogBufferFar;

    IDirect3DTexture8 *m_pDepthTexture;
    IDirect3DSurface8 *m_pColorBuffer;
    IDirect3DSurface8 *m_pZBuffer;

    HRESULT RenderScene();

public:
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

    HRESULT CreatePixelShader();
    HRESULT CreateTextures();

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

    m_bDrawHelp     = false;
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

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      Resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the skybox
    if( FAILED( m_SkyboxObject.Create( m_pd3dDevice, "Models\\SkyBox.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the main file object
    if( FAILED( m_Room.Create( m_pd3dDevice, "Models\\VolFogTerrain.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the main file object
    if( FAILED( m_FogVolume.Create( m_pd3dDevice, "Models\\VolFogFog.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    
    // Save radius for later use.
    m_fFogVolumeRadius = m_FogVolume.ComputeRadius();

    // Create vertex shader.
    DWORD vdecl[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(0, D3DVSDT_FLOAT3), // v0 = XYZ
        D3DVSD_REG(1, D3DVSDT_FLOAT3), // v1 = normals
        D3DVSD_END()
    };

    if ( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\DepthShader.xvu", vdecl, &m_dwFogVertexShader ) ) )
        return E_FAIL;

    // Create render targets for fog depth.
    m_pd3dDevice->CreateTexture( 640, 480, 1, D3DUSAGE_RENDERTARGET, D3DFMT_LIN_A8R8G8B8, 0, &m_pFogBufferNear );
    m_pd3dDevice->CreateTexture( 640, 480, 1, D3DUSAGE_RENDERTARGET, D3DFMT_LIN_A8R8G8B8, 0, &m_pFogBufferFar );

    CreateTextures();
    CreatePixelShader();

    // Get the original color and z-buffer.
    m_pd3dDevice->GetRenderTarget(&m_pColorBuffer);
    m_pd3dDevice->GetDepthStencilSurface(&m_pZBuffer);

    // Set the matrices
    D3DXMatrixIdentity( &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    m_vEye = D3DXVECTOR3( 10.0f, 5.0f, 0.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMatrixLookAtLH( &m_matView, &m_vEye, &vAt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/3, 4.0f/3.0f, 0.1f, 40.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DXMATRIX matView, matRotate;

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    // Rotate eye around up axis.
    D3DXMatrixRotationAxis( &matRotate, &vUp, 
                                        m_DefaultGamepad.fX1*m_fElapsedTime );
    D3DXVec3TransformCoord( &m_vEye, &m_vEye, &matRotate );

    // Rotate eye points around side axis.
    D3DXVECTOR3 vView = (vAt - m_vEye);
	float dist = D3DXVec3Length( &vView );

    D3DXVec3Normalize( &vView, &vView );

    // Place limits so we dont go over the top or under the bottom.
    FLOAT dot = D3DXVec3Dot( &vView, &vUp );
    if( (dot < 0.0f/*0.99f*/ || m_DefaultGamepad.fY1 < 0.0f) && (dot > -0.99f || m_DefaultGamepad.fY1 > 0.0f) )
    {
        D3DXVECTOR3 axis;
        D3DXVec3Cross( &axis, &vView, &vUp );
        D3DXMatrixRotationAxis( &matRotate, &axis, m_DefaultGamepad.fY1 * m_fElapsedTime );
        D3DXVec3TransformCoord( &m_vEye, &m_vEye, &matRotate );
    }

    // Move in/out.
    float fIn = (m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] / 255.0f);
    float fOut = (m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] / 255.0f);


    if( fIn > 0.1f  && dist > 1.0f )
        m_vEye += vView * 4.0f * fIn * m_fElapsedTime;

    if( fOut > 0.1f )
        m_vEye -= vView * 4.0f * fOut * m_fElapsedTime;

    D3DXMatrixLookAtLH( &m_matView, &m_vEye, &vAt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateTextures()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateTextures()
{
    HRESULT hr;

    // Create a 4k x 1 texture to encode the depth values.
    {
        hr = m_pd3dDevice->CreateTexture( 4096, 1, 1, 0, D3DFMT_A8R8G8B8, 0, &m_pDepthTexture );

        // Lock and fill the texture
        D3DSURFACE_DESC desc;
        D3DLOCKED_RECT lock;
        m_pDepthTexture->GetLevelDesc( 0, &desc );
        m_pDepthTexture->LockRect( 0, &lock, 0, 0 );
        DWORD* pBits = (DWORD*)lock.pBits;

        for( int v = 0; v < 1; v++ )
        {
            for( int u = 0; u < 4096; u++ )
            {
                int blue =  (u & 0x00f00) >> 8;
                int green = (u & 0x000f0) >> 4;
                int red =   (u & 0x0000f);
                (*pBits++) = (red << 16) | (green << 8) | blue;
            }
        }
        
        // Swizzle and unlock the texture
        XBUtil_SwizzleTexture2D( &lock, &desc );
        m_pDepthTexture->UnlockRect( 0 );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreatePixelShader()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreatePixelShader()
{
    //
    // Setup the pixel shader 
    //
    // Fog depth is 12 bits split up as follows:
    //
    // B = 4 bits for carry + 4 high bits of fog.
    // G = 4 bits for carry + 4 middle bits of fog.
    // R = 4 bits for carry + 4 low bits of fog.
    //
    // Compute dB * 16 + dG + dR / 16
    //
    //
    D3DPIXELSHADERDEF psd;
    ZeroMemory( &psd, sizeof(psd) );
    psd.PSCombinerCount = PS_COMBINERCOUNT( 4,
                                            PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    psd.PSTextureModes  = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D, PS_TEXTUREMODES_PROJECT2D,
                                           PS_TEXTUREMODES_NONE, PS_TEXTUREMODES_NONE );
    psd.PSInputTexture  = PS_INPUTTEXTURE( 0, 0, 0, 0 );
    psd.PSDotMapping    = PS_DOTMAPPING( 0, PS_DOTMAPPING_ZERO_TO_ONE,
                                         PS_DOTMAPPING_ZERO_TO_ONE, PS_DOTMAPPING_ZERO_TO_ONE );
    psd.PSCompareMode   = PS_COMPAREMODE( PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
                                          PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
                                          PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
                                          PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT );

    psd.PSC0Mapping = 0xffffffff;
    psd.PSC1Mapping = 0xffffffff;
    psd.PSFinalCombinerConstants = 0x000000ff;

    //------------- Stage 0 -------------
    // r1 = T1-T0
    psd.PSRGBInputs[0]    = PS_COMBINERINPUTS( PS_REGISTER_T0 | PS_INPUTMAPPING_SIGNED_NEGATE | PS_CHANNEL_RGB,
                                               PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                               PS_REGISTER_T1 | PS_CHANNEL_RGB,
                                               PS_REGISTER_ONE | PS_CHANNEL_RGB );

    psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R1,
                                                0 );

    psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS( PS_REGISTER_T0 | PS_INPUTMAPPING_SIGNED_NEGATE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_T1 | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );

    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R1,
                                                0 );

    //------------- Stage 1 -------------
    // r0.rgb = G + 1/16 * R = r1.rgb dot (0,1/16,1,0)
    psd.PSRGBInputs[1]    = PS_COMBINERINPUTS( PS_REGISTER_R1 | PS_INPUTMAPPING_SIGNED_IDENTITY | PS_CHANNEL_RGB,
                                               PS_REGISTER_C0 | PS_CHANNEL_RGB,
                                               PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                               PS_REGISTER_ZERO | PS_CHANNEL_RGB );

    psd.PSRGBOutputs[1]   = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_DOT_PRODUCT );

    // r0.a = B * 4 = r1.b << 2
    psd.PSAlphaInputs[1]  = PS_COMBINERINPUTS( PS_REGISTER_R1 | PS_INPUTMAPPING_SIGNED_IDENTITY | PS_CHANNEL_BLUE,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ZERO | PS_CHANNEL_ALPHA );

    psd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_COMBINEROUTPUT_SHIFTLEFT_2 );

    psd.PSConstant0[1] = 0x0010ff00;

    //------------- Stage 2 -------------
    // r0.a = B * 16 = r0.a << 2
    psd.PSAlphaInputs[2]  = PS_COMBINERINPUTS( PS_REGISTER_R0 | PS_INPUTMAPPING_SIGNED_IDENTITY | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_R0 | PS_INPUTMAPPING_SIGNED_IDENTITY | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );

    psd.PSAlphaOutputs[2] = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
                                                PS_REGISTER_R1,
                                                PS_REGISTER_DISCARD,
                                                PS_COMBINEROUTPUT_SHIFTLEFT_2 );

    //------------- Stage 3 -------------
    // r0.a = B * 16 + G + R * 1/16 = r0.a + r0.b
    psd.PSAlphaInputs[3]  = PS_COMBINERINPUTS( PS_REGISTER_R0 | PS_INPUTMAPPING_SIGNED_IDENTITY | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_R0 | PS_INPUTMAPPING_SIGNED_IDENTITY | PS_CHANNEL_BLUE,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );

    psd.PSAlphaOutputs[3] = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R0,
                                                0 );

    psd.PSConstant0[3] = 0x00ff0000;



    //------------- Final combiner -------------
    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                                       PS_REGISTER_C0   | PS_CHANNEL_RGB );

    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS( PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                                      PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                                      PS_REGISTER_R0   | PS_CHANNEL_ALPHA,
                                                      0 );

	// Fog color.
    psd.PSFinalCombinerConstant0 = 0xffffffff;

    // Create the pixel shader, as defined above.
    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwFogPixelShader ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderScene()
{
    // Set up the light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.7071067f, 0.7071067f, 0.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );

    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00555555 );

    // Draw the room.
    m_Room.Render( m_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00008000f, 1.0f, 0 );

    // Set state
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x44444444 );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

    // Render the Skybox
    {
        D3DXMatrixIdentity( &m_matWorld );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        
        // Center view matrix for skybox and disable zbuffer
        D3DXMATRIX matView, matViewSave;
        m_pd3dDevice->GetTransform( D3DTS_VIEW,      &matViewSave );
        matView = matViewSave;
        matView._41 = 0.0f; matView._42 = -0.0f; matView._43 = 0.0f;
        m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

        // Render the skybox
        m_SkyboxObject.Render( m_pd3dDevice );

        // Restore the render states
        m_pd3dDevice->SetTransform( D3DTS_VIEW, &matViewSave );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    }

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    // Set up misc render states
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    // Render the scene normally.
    RenderScene();

    //
    // Setup common state for all passes.
    //
    m_pd3dDevice->SetVertexShader( m_dwFogVertexShader );

    // Calculate and set composite matrix.
    D3DXMATRIX matComposite, matWorldViewInverse, matProjectionViewport;
    D3DXMatrixMultiply( &matComposite, &m_matWorld, &m_matView );
    D3DXMatrixInverse( &matWorldViewInverse, NULL, &matComposite );
    m_pd3dDevice->GetProjectionViewportMatrix( &matProjectionViewport );
    D3DXMatrixMultiply( &matComposite, &matComposite, &matProjectionViewport );
    D3DXMatrixTranspose( &matComposite, &matComposite );
    m_pd3dDevice->SetVertexShaderConstant( 0, &matComposite, 4 );

    // Set viewport offsets.
    float fViewportOffsets[4] = { 0.53125f, 0.53125f, 0.0f, 0.0f };
    m_pd3dDevice->SetVertexShaderConstant( 4, &fViewportOffsets, 1 );

    // Set local viewer position.
    D3DXVECTOR3 v3ViewPos( 0.0f, 0.0f, 0.0f );
    D3DXVec3TransformCoord( &v3ViewPos, &v3ViewPos, &matWorldViewInverse );
    m_pd3dDevice->SetVertexShaderConstant( 5, &v3ViewPos, 1 );

    // Scale distance values between 0 and 1.
    float fFogScale[4];
    fFogScale[0] = 1.0f / 40.0f;    // scale
    fFogScale[1] = 0.0f;            // offset (scaled)
    m_pd3dDevice->SetVertexShaderConstant( 6, fFogScale, 1 );

    //
    // Pass 0: Setup the stencil values correctly for when the viewpoint is in
    //         the fog volume.
    //
    m_pd3dDevice->SetTexture( 0, NULL );

    m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0 );

    // Increment back faces.
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT );
    m_FogVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES );

    // Decrement front faces.
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECRSAT );
    m_FogVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES );

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );

    //
    // Common setup for the following passes.
    //
    m_pd3dDevice->SetTexture( 0, m_pDepthTexture );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT  );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT  );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    //
    // Pass 1: Draw the visible front faces of the fog volume(s) with additive 
    //         blend.  Increment stencil values.
    //
    IDirect3DSurface8* surf;
    m_pFogBufferNear->GetSurfaceLevel( 0, &surf );
    m_pd3dDevice->SetRenderTarget( surf, m_pZBuffer );
    surf->Release();

    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT );

    m_FogVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES );

    //
    // Pass 2: Draw the visible back faces of the fog volume(s) with additive 
    //         blend.  Decrement stencil values.
    //
    m_pFogBufferFar->GetSurfaceLevel( 0, &surf );
    m_pd3dDevice->SetRenderTarget( surf, m_pZBuffer );
    surf->Release();

    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECRSAT );

    m_FogVolume.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES );

    // If the viewpoint is in the fog, then we should start the stencil at one
    // instead of zero.

    //
    // Pass 3: Draw any objects intersecting the fog volume.  Add to the far
    //         values where stencil != 0 (where the object is inside the fog).
    //
    // Note: Z invariance needs to be maintained.
    //
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_EQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );

    m_Room.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOTEXTURES );

    //
    // Pass 4: Compute the final fog value for each pixel and apply it.
    //
    m_pd3dDevice->SetRenderTarget( m_pColorBuffer, m_pZBuffer );

    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE4(2) | D3DFVF_TEXCOORDSIZE4(3) );
    m_pd3dDevice->SetPixelShader( m_dwFogPixelShader );

    m_pd3dDevice->SetTexture( 0, m_pFogBufferNear );
    m_pd3dDevice->SetTexture( 1, m_pFogBufferFar );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_POINT );

    // Draw the overlay polygon.
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, g_FogVerts, sizeof(FOGVERT) );

    //
    // Restore state.
    //
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    m_pd3dDevice->SetPixelShader(0);

    // Show title, frame rate, and help
    if( m_bDrawHelp )
    {
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    }
    else
    {
        // Show frame rate
        m_Font.DrawText(  64, 50, 0xffffffff, L"VolumeFog" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
