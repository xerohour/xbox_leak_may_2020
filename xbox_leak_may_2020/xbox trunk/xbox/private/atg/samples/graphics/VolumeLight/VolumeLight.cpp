//-----------------------------------------------------------------------------
// File: VolumeLight.cpp
//
// Desc: Example code showing how to use volume textures in place of lightmaps.
//
// Hist: 01.05.00 - New for February XDK release
//       03.12.01 - Added Xfest art changes for April XDK release
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
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move light" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Move camera" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Change light\nFOV" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS 5




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
    { "StoneHen.bmp", resource_StoneHen_OFFSET },
    { "StoneHea.bmp", resource_StoneHea_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define VOLTEX_WIDTH  256
#define VOLTEX_HEIGHT 256
#define VOLTEX_DEPTH   16

FLOAT       g_fFlashLightFOV    = D3DX_PI/4; // Field of view of flashlight beam
FLOAT       g_fFlashLightExtent = 25.0f;     // How far lighting effects extend
D3DXVECTOR3 g_vFlashLightPos;
D3DXVECTOR3 g_vFlashLightDir;

FLOAT       g_fMinOffset = 0.050f; // Parameters for the volume light
FLOAT       g_fMaxOffset = 0.200f;
FLOAT       g_fMinSigma  = 0.020f;
FLOAT       g_fMaxSigma  = 0.500f;
FLOAT       g_fScale     = 3.0f;




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                  m_Font;           // System font
    CXBHelp                  m_Help;           // Application help
    BOOL                     m_bDrawHelp;      // Whether to display help

    CXBPackedResource        m_xprResource;    // Packed resources (textures)
    
    CXBMesh                  m_SkyBoxObject;   // The skybox geometry
    D3DXMATRIX               m_matSkyBox;      // Matrix to orient skybox

    CXBMesh                  m_TerrainObject;  // The terrain geometry
    D3DXMATRIX               m_matWorld;       // Matrix set
    D3DXMATRIX               m_matView;
    D3DXMATRIX               m_matProj;

    CXBMesh                  m_FlashLightObject;    // The flashlight geometry
    D3DXMATRIX               m_matLightOrientation; // Matrix to orient flashlight

    LPDIRECT3DTEXTURE8       m_pLightTexture;  // Texture for flahlight lens flare

    LPDIRECT3DVOLUMETEXTURE8 m_pVolumeTexture; // Volumetexture for the light beam

    DWORD                    m_dwVertexShader; // Vertex shader for the lit terrain
    DWORD                    m_dwPixelShader;  // Pixel shader for the lit terrain

    HRESULT CreateVolumeTexture();             // Creates the volume light texture
    HRESULT CreatePixelShader();               // Creates the pixel shader

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

    m_bDrawHelp      = FALSE;
    m_pLightTexture  = NULL;
    m_pVolumeTexture = NULL;
}




//-----------------------------------------------------------------------------
// Name: CreatePixelShader()
// Desc: Creates the pixel shader for the lit terrain
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreatePixelShader()
{
    // Setup the pixel shader
    D3DPIXELSHADERDEF psd;
    ZeroMemory( &psd, sizeof(psd) );
    psd.PSCombinerCount = PS_COMBINERCOUNT( 2,
                                            PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    psd.PSTextureModes  = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D, PS_TEXTUREMODES_PROJECT3D,
                                           PS_TEXTUREMODES_NONE, PS_TEXTUREMODES_NONE );

    //------------- Stage 0 -------------
    // Combine base texture (t0) with the diffuse lighting contribution (v0)
    psd.PSRGBInputs[0]    = PS_COMBINERINPUTS( PS_REGISTER_T0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_V0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                               PS_REGISTER_ONE | PS_CHANNEL_RGB );
    psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS( PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );
    psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );

    //------------- Stage 1 -------------
    // Modulate in volume light (t1) and add in some ambient (c0)
    psd.PSRGBInputs[1]    = PS_COMBINERINPUTS( PS_REGISTER_R0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_T1  | PS_CHANNEL_RGB,
                                               PS_REGISTER_R0  | PS_CHANNEL_RGB,
                                               PS_REGISTER_C0  | PS_CHANNEL_RGB );
    psd.PSAlphaInputs[1]  = PS_COMBINERINPUTS( PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA,
                                               PS_REGISTER_ONE | PS_CHANNEL_ALPHA );
    psd.PSRGBOutputs[1]   = PS_COMBINEROUTPUTS( PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_R0,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    psd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS( PS_REGISTER_R0,
                                                PS_REGISTER_DISCARD,
                                                PS_REGISTER_DISCARD,
                                                PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    psd.PSConstant0[1] = 0x40404040;

    //------------- Final combiner -------------
    
    // Output r0
    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_ONE     | PS_CHANNEL_ALPHA,
                                                       PS_REGISTER_R0      | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO     | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO    | PS_CHANNEL_RGB );

    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS( PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      0 | 0 | 0 );

    // Create the pixel shader, as defined above.
    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwPixelShader ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateVolumeTexture()
// Desc: Creates the volume light. The flashlight's beam is modelled here in
//       the 3D space of a volume texture.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateVolumeTexture()
{
    HRESULT hr;

    // Create a volume texture
    hr = m_pd3dDevice->CreateVolumeTexture( VOLTEX_WIDTH, VOLTEX_HEIGHT, VOLTEX_DEPTH, 
                                            1, 0, D3DFMT_L8, D3DPOOL_MANAGED,
                                            &m_pVolumeTexture );
    if( FAILED(hr) )
        return hr;

    // Lock and fill the volume texture
    D3DVOLUME_DESC desc;
    D3DLOCKED_BOX lock;
    m_pVolumeTexture->GetLevelDesc( 0, &desc );
    m_pVolumeTexture->LockBox( 0, &lock, 0, 0L );
    BYTE* pBits = (BYTE*)lock.pBits;

    for( UINT w=0; w<VOLTEX_DEPTH; w++ )
    {
        FLOAT z = (1.0f*w)/(VOLTEX_DEPTH-1); // Ranges from 0 to +1

        // Paramaters for Gaussian falloff
        FLOAT fOffset = g_fMinOffset + (g_fMaxOffset-g_fMinOffset)*z;
        FLOAT fSigma  = g_fMinSigma + (g_fMaxSigma-g_fMinSigma)*z;
        FLOAT fBias   = expf( -(1.0f-fOffset)*(1.0f-fOffset)/(2*fSigma*fSigma) );
        FLOAT fMaxAmplitude = ( expf( -(fOffset*fOffset)/(2*fSigma*fSigma) ) - fBias );
        FLOAT fScale  = 1.0f / fMaxAmplitude;

        for( UINT v=0; v<VOLTEX_HEIGHT; v++ )
        {
            for( UINT u=0; u<VOLTEX_WIDTH; u++ )
            {
                FLOAT x = (2.0f*u)/(VOLTEX_HEIGHT-1) - 1.0f; // Ranges from -1 to +1
                FLOAT y = (2.0f*v)/(VOLTEX_WIDTH-1) - 1.0f;  // Ranges from -1 to +1
                FLOAT r = sqrtf( x*x + y*y );

                // Compute the intensity
                FLOAT fIntensity = fScale * ( expf( -(r-fOffset)*(r-fOffset)/(2*fSigma*fSigma) ) - fBias );
                if( fIntensity > 1.0f ) fIntensity = 1.0f;
                if( fIntensity < 0.0f ) fIntensity = 0.0f;

                // Write the texel
                (*pBits++) = (BYTE)(255*fIntensity);
            }
        }
    }
    
    // Swizzle and unlock the texture
    XBUtil_SwizzleTexture3D( &lock, &desc );
    m_pVolumeTexture->UnlockBox( 0 );

    return S_OK;
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

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the geometry models
    if( FAILED( m_SkyBoxObject.Create( m_pd3dDevice, "Models\\SkyBox.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_TerrainObject.Create( m_pd3dDevice, "Models\\StoneHenge.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_FlashLightObject.Create( m_pd3dDevice, "Models\\Light.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Get the light's texture
    m_pLightTexture = m_xprResource.GetTexture( "Light.bmp" );
    
    // Create the volume texture
    if( FAILED( CreateVolumeTexture() ) )
        return E_FAIL;

    // Create the pixel shader
    if( FAILED( CreatePixelShader() ) )
        return E_FAIL;

    // Set the matrices
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/3, 4.0f/3.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Create vertex shader
    DWORD dwVertexDecl[20];
    XBUtil_DeclaratorFromFVF( m_TerrainObject.GetMesh(0)->m_dwFVF, dwVertexDecl );

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Shader.xvu",
                                           dwVertexDecl, &m_dwVertexShader ) ) )
        return E_FAIL;

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

    // Change the field of view of the flashlight
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        g_fFlashLightFOV += D3DX_PI * ( 5.0f / 180.0f );
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
        g_fFlashLightFOV -= D3DX_PI * ( 5.0f / 180.0f );

    // Rotate and position the camera with the gamepad
    static FLOAT fViewAngle =  3.53f;
    static FLOAT fCameraX   =  4.54f;
    static FLOAT fCameraY   =  2.00f;
    static FLOAT fCameraZ   =  6.75f;
    fViewAngle += 2.0f * m_DefaultGamepad.fX2*m_fElapsedTime;
    fCameraX   += 5.0f * m_DefaultGamepad.fY2*m_fElapsedTime*sinf(fViewAngle);
    fCameraZ   += 5.0f * m_DefaultGamepad.fY2*m_fElapsedTime*cosf(fViewAngle);

    // Set the view transform
    D3DXVECTOR3 from = D3DXVECTOR3( fCameraX, fCameraY, fCameraZ );
    D3DXVECTOR3 at   = D3DXVECTOR3( sinf(fViewAngle), 0.0f, cosf(fViewAngle) ) + from;
    D3DXVECTOR3 up   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXMatrixLookAtLH( &m_matView, &from, &at, &up );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    // Set the skybox view transform (which retains the view orientation, but not
    // the translation)
    m_matSkyBox     = m_matView;
    m_matSkyBox._41 = 0.0f; 
    m_matSkyBox._42 = 0.0f; 
    m_matSkyBox._43 = 0.0f;

    // Position and orient the light
    static FLOAT fLightPhi   = +0.00f;
    static FLOAT fLightTheta = -0.32f;
    fLightPhi   += +3.0f * m_DefaultGamepad.fX1*m_fElapsedTime;
    fLightTheta += +3.0f * m_DefaultGamepad.fY1*m_fElapsedTime;
    fLightTheta = min( +D3DX_PI/2, max( fLightTheta, -D3DX_PI/2 ) );

    g_vFlashLightPos.x = 3.0f * sinf( 1.0f * m_fAppTime ) + 1.0f;
    g_vFlashLightPos.y = 0.0f * sinf( 5.0f * m_fAppTime ) + 1.0f;
    g_vFlashLightPos.z = 2.0f * cosf( 1.0f * m_fAppTime ) - 1.0f;
    
    g_vFlashLightDir.x = +cosf( fLightPhi + m_fAppTime );
    g_vFlashLightDir.y = +sinf( fLightTheta );
    g_vFlashLightDir.z = -sinf( fLightPhi + m_fAppTime );
    D3DXVec3Normalize( &g_vFlashLightDir, &g_vFlashLightDir );
    
    // Use the inverse of a camera lookat matrix to get an orientation matrix
    // for the light. Note: the inverse step could be optimized out, since we
    // re-invert the matrix later, but we don't do that here, to illustrate the
    // math the really happens.
    D3DXMATRIX matLookAt;
    D3DXMatrixLookAtLH( &matLookAt, &g_vFlashLightPos, 
                                    &(g_vFlashLightPos+g_vFlashLightDir), 
                                    &D3DXVECTOR3(0,1,0) );
    D3DXMatrixInverse( &m_matLightOrientation, NULL, &matLookAt );

    // Set up texture matrix used to orient the volume texture. First, orient
    // to the desired volume texture orientation.
    D3DXMATRIX matTexGen, matTrans1, matScale, matTrans2;
    D3DXMatrixInverse( &matTexGen, NULL, &m_matLightOrientation );

    // Scale to mimic the light beam, and translate the result to the volume
    // texture's origin
    FLOAT fBeamWidth  = g_fFlashLightExtent * cosf( (D3DX_PI-g_fFlashLightFOV)/2 );
    FLOAT fBeamLength = g_fFlashLightExtent * sinf( (D3DX_PI-g_fFlashLightFOV)/2 );
    D3DXMatrixScaling( &matScale, 1.0f/fBeamWidth, 1.0f/fBeamWidth, 1.0f/fBeamLength );
    D3DXMatrixTranslation( &matTrans2, 0.5f, 0.5f, 0.0f );
    D3DXMatrixMultiply( &matTexGen, &matTexGen, &matScale );
    D3DXMatrixMultiply( &matTexGen, &matTexGen, &matTrans2 );

    // Finally, pass the transformed texture matrix to the vertex shader
    D3DXMatrixTranspose( &matTexGen, &matTexGen );
    m_pd3dDevice->SetVertexShaderConstant( 16, &matTexGen, 4 );

    // Pass the transform set to the vertex shader
    D3DXMATRIX matW, matWV, matWVP;
    D3DXMatrixMultiply( &matWV, &m_matWorld, &m_matView );
    D3DXMatrixMultiply( &matWVP, &matWV, &m_matProj );
    D3DXMatrixTranspose( &matW,   &m_matWorld );
    D3DXMatrixTranspose( &matWV,  &matWV );
    D3DXMatrixTranspose( &matWVP, &matWVP );
    m_pd3dDevice->SetVertexShaderConstant(  4, &matW,   4 );
    m_pd3dDevice->SetVertexShaderConstant(  8, &matWV,  4 );
    m_pd3dDevice->SetVertexShaderConstant( 12, &matWVP, 4 );

    // Pass the unattenuated point light values to the vertex shader
    D3DXVECTOR4 v0( 0.0f, 1.0f, 1.0f, 1.0f );
    D3DXVECTOR4 v1( 1.0f, 1.0f, 1.0f, 1.0f );
    D3DXVECTOR4 vLightPos( g_vFlashLightPos.x, g_vFlashLightPos.y, g_vFlashLightPos.z, 0.0f );
    D3DXVECTOR4 vLightColor( 1.0f, 1.0f, 1.0f, 1.0f );
    D3DXVECTOR4 vAmbientColor( 0.0f, 0.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetVertexShaderConstant(  0,  &v0, 1 );
    m_pd3dDevice->SetVertexShaderConstant(  1,  &v1, 1 );
    m_pd3dDevice->SetVertexShaderConstant( 20, &vLightPos,     1 );
    m_pd3dDevice->SetVertexShaderConstant( 21, &vLightColor,   1 );
    m_pd3dDevice->SetVertexShaderConstant( 22, &vAmbientColor, 1 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Render the skybox
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,  &m_matSkyBox );
    m_SkyBoxObject.Render( m_pd3dDevice );

    // Set default state
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    // Draw the terrain, using the vertex shader, pixel shader, and the volume
    // light in stage 1
    m_pd3dDevice->SetTexture( 1, m_pVolumeTexture );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_BORDER );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_BORDER );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSW,  D3DTADDRESS_BORDER );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_BORDERCOLOR, 0x00000000 );
    m_pd3dDevice->SetPixelShader( m_dwPixelShader );
    m_pd3dDevice->SetVertexShader( m_dwVertexShader );
    m_TerrainObject.Render( m_pd3dDevice, XBMESH_NOFVF );

    // Restore state
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetTexture( 1, NULL );

    // Render the flashlight body
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); 
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0xff808080 );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matLightOrientation );
    m_FlashLightObject.Render( m_pd3dDevice );

    // Render the flashlight lens flare
    m_pd3dDevice->SetTexture( 3, m_pLightTexture );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,         FtoDW(0.2f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,      FtoDW(0.0f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,      FtoDW(0.0f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,      FtoDW(1.0f) );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, 1, g_vFlashLightPos, sizeof(D3DXVECTOR3) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"VolumeLight" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



