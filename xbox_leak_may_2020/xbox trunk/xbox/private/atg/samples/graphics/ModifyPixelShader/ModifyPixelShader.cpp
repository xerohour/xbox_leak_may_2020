//-----------------------------------------------------------------------------
// File: ModifyPixelShader.cpp
//
// Desc: Demonstrates modifying a pixelshader on the fly, using the API. Any of
//       the following render states can be used to modify a current set pixel
//       shader:
//          D3DRS_PSCOMBINERCOUNT
//          D3DRS_PSDOTMAPPING
//          D3DRS_PSINPUTTEXTURE
//          D3DRS_PSCOMPAREMODE
//          D3DRS_PSCONSTANT0_[0..7]
//          D3DRS_PSCONSTANT1_[0..7]
//          D3DRS_PSRGBINPUTS[0..7]
//          D3DRS_PSALPHAINPUTS[0..7]
//          D3DRS_PSRGBOUTPUTS[0..7]
//          D3DRS_PSALPHAOUTPUTS[0..7]
//          D3DRS_PSFINALCOMBINERINPUTSABCD
//          D3DRS_PSFINALCOMBINERINPUTSEFG
//          D3DRS_PSFINALCOMBINERCONSTANT0
//          D3DRS_PSFINALCOMBINERCONSTANT1
//
// Hist: 04.18.01 - New for May XDK release
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
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate object" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Move light" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_1, L"Zoom" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nfog" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle mask\ntexture" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle combiner\ncount" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle mapping\ninvert" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_2, L"Pause texture\nrotation" },
};

#define NUM_HELP_CALLOUTS 9




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;   // Position
    D3DXVECTOR3 normal;     // Normal
    D3DCOLOR    color;      // Color
    FLOAT       tu0, tv0;   // Texture 0 coordinates
    FLOAT       tu1, tv1;   // Texture 1 coordinates
    FLOAT       tu2, tv2;   // Texture 2 coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3)




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

    LPDIRECT3DVERTEXBUFFER8 m_pCylinderVB; // Buffer for cylinder vertices
    LPDIRECT3DTEXTURE8      m_pTexture0;
    LPDIRECT3DTEXTURE8      m_pTexture1;
    LPDIRECT3DTEXTURE8      m_pTexture2;

    DWORD        m_dwPixelShader;    // Handle for pixel shader 1
    BOOL         m_bEnableFog;
    BOOL         m_bUseRobotMask;
    BOOL         m_bChangeCombinerCount;
    BOOL         m_bChangeMapping;

    D3DXMATRIX   m_matObject;        // Transform matrix for the object
    D3DXMATRIX   m_matTexture;       // Transform matrix for the texture
    FLOAT        m_fEyeScale;        // Scale of viewing distance

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
    m_bDrawHelp            = FALSE;

    m_pCylinderVB          = NULL;
    m_dwPixelShader        = 0L;
    m_fEyeScale            = 1.0f;

    m_bEnableFog           = FALSE;
    m_bUseRobotMask        = FALSE;
    m_bChangeCombinerCount = FALSE;
    m_bChangeMapping       = FALSE;

    D3DXMatrixIdentity( &m_matObject );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
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
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the textures
    m_pTexture0 = m_xprResource.GetTexture( resource_Texture0_OFFSET );
    m_pTexture1 = m_xprResource.GetTexture( resource_Texture1_OFFSET );
    m_pTexture2 = m_xprResource.GetTexture( resource_Texture2_OFFSET );
    
    // Create geometry for a cylinder
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pCylinderVB ) ) )
        return E_FAIL;

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX* v;
    if( FAILED( m_pCylinderVB->Lock( 0, 0, (BYTE**)&v, 0 ) ) )
        return E_FAIL;
    for( DWORD i=0; i<50; i++ )
    {
        FLOAT theta = (2*D3DX_PI*i)/(50-1);

        v[2*i+0].position = D3DXVECTOR3( sinf(theta),-1.2f, cosf(theta) );
        v[2*i+0].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
        v[2*i+0].color    = 0xffffffff;
        v[2*i+0].tu0      = v[2*i+0].tu1 = v[2*i+0].tu2 = ((FLOAT)i*2.0f)/(50-1);
        v[2*i+0].tv0      = v[2*i+0].tv1 = v[2*i+0].tv2 = 1.0f;

        v[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.2f, cosf(theta) );
        v[2*i+1].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
        v[2*i+1].color    = 0xff808080;
        v[2*i+1].tu0      = v[2*i+1].tu1 = v[2*i+1].tu2 = ((FLOAT)i*2.0f)/(50-1);
        v[2*i+1].tv0      = v[2*i+1].tv1 = v[2*i+1].tv2 = 0.0f;
    }
    m_pCylinderVB->Unlock();

    //--------------------------
    // Define a hard-coded pixel shader
    //--------------------------
    D3DPIXELSHADERDEF psd;
    ZeroMemory( &psd, sizeof(psd) );
    
    // The pixel shader uses 3 combiner stages
    psd.PSCombinerCount = PS_COMBINERCOUNT( 3,
        PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1 );
    psd.PSTextureModes  = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D,
                                        PS_TEXTUREMODES_PROJECT2D,
                                        PS_TEXTUREMODES_PROJECT2D,
                                        PS_TEXTUREMODES_NONE );
    psd.PSInputTexture  = PS_INPUTTEXTURE( 0, 0, 0, 0 );
    psd.PSDotMapping    = PS_DOTMAPPING( 0,
                                    PS_DOTMAPPING_ZERO_TO_ONE,
                                    PS_DOTMAPPING_ZERO_TO_ONE,
                                    PS_DOTMAPPING_ZERO_TO_ONE );
    psd.PSCompareMode   = PS_COMPAREMODE( 
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT );

    //--------------------------
    // Stage 0: r0 = t2 (the mask texture used for the mux in stage 1)
    //--------------------------

    // A=T2.rgb, B=1, C=0, D=0 (so that AB.rgb = T2 and CD = 0 )
    psd.PSRGBInputs[0]    = PS_COMBINERINPUTS(
        PS_REGISTER_T2   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );
    
    // A=T2.b, B=1, C=0, D=0 (so that AB.a = T2.b and CD = 0 )
    psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS(
        PS_REGISTER_T2   | PS_CHANNEL_BLUE  | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY );
    
    // R0.rgb = AB
    psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    
    // R0.a = AB
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    
    psd.PSConstant0[0]    = 0x00000000;
    psd.PSConstant1[0]    = 0x00000000;

    //--------------------------
    // Stage 1: r0 = T0 mux T1
    //--------------------------

    // A=T0.rgb, B=1, C=T1.rgb, D=1 (so that AB.rgb = T0 and CD = T1 )
    psd.PSRGBInputs[1]    = PS_COMBINERINPUTS(
        PS_REGISTER_T0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_T1   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

    // A=T0.a, B=1, C=T1.a, D=1 (so that AB.a = T0 and CD = T1 )
    psd.PSAlphaInputs[1]  = PS_COMBINERINPUTS(
        PS_REGISTER_T0   | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_T1   | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY );
    
    // R0.rgb = AB mux CD
    psd.PSRGBOutputs[1]   = PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_R0,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_MUX );

    // R0.a = AB mux CD
    psd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_R0,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_MUX );
    psd.PSConstant0[1]    = 0x00000000;
    psd.PSConstant1[1]    = 0x00000000;

    //--------------------------
    // Stage 2: r0 = r0 * v0
    //--------------------------

    // A=R0.rgb, B=V0.rgb, C=0, D=0 (so that AB.rgb = R0*V0 and CD = 0 )
    psd.PSRGBInputs[2]    = PS_COMBINERINPUTS(
        PS_REGISTER_R0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_V0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

    // A=R0.a, B=V0.a, C=0, D=0 (so that AB.a = R0*V0 and CD = 0 )
    psd.PSAlphaInputs[2]  = PS_COMBINERINPUTS(
        PS_REGISTER_R0   | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_V0   | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

    // R0 = AB
    psd.PSRGBOutputs[2]   = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );

    // R0 = AB
    psd.PSAlphaOutputs[2] = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    psd.PSConstant0[2]    = 0x00000000;
    psd.PSConstant1[2]    = 0x00000000;

    //--------------------------
    // Final combiner: output = r0
    //--------------------------
    psd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(
        PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_R0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

    // E = 0, F = 0, G = 1. (From above, EF is not used. G is alpha and is set to 1.)
    psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
        0 | 0 | 0 );
    psd.PSC0Mapping = 0x00000000;
    psd.PSC1Mapping = 0x00000000;

    // Create the hard-coded pixel shader. This shader, defined in detail above,
    // uses texture 2 as a mask to select between texture 0 and texture 1, and 
    // then modulates in the diffuse component.
    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwPixelShader ) ) )
    {
        OUTPUT_DEBUG_STRING( "Could not create hardcoded pixel shader.\n" );
        return E_FAIL;
    }

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
    D3DXMatrixMultiply( &m_matObject, &m_matObject, &matRotate );

    // Move around the texture
    FLOAT tx = m_fAppTime/2 - floorf(m_fAppTime/2);
    FLOAT ty = 0.0f;
    m_matTexture._11 = 1.0f;   m_matTexture._12 = 0.0f; 
    m_matTexture._21 = 0.0f;   m_matTexture._22 = 1.0f; 
    m_matTexture._31 = tx;     m_matTexture._32 = ty; 
    m_matTexture._41 = 0.0f;   m_matTexture._42 = 0.0f; 

    // Zoom
    if( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP )
        m_fEyeScale = m_fEyeScale/1.01f;
    if( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        m_fEyeScale *= 1.01f;

    // Toggle options
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        m_bEnableFog = !m_bEnableFog;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        m_bUseRobotMask = !m_bUseRobotMask;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        m_bChangeCombinerCount = !m_bChangeCombinerCount;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
        m_bChangeMapping = !m_bChangeMapping;

    // Set transforms
    D3DXMATRIX matView, matProj;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f*m_fEyeScale, 3.0f*m_fEyeScale,-5.0f*m_fEyeScale ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Setup a material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Adjust the point light's position
    static FLOAT phi1 = 0.5f;
    phi1 += 3.0f * m_fElapsedTime * m_DefaultGamepad.fX2;
    if( phi1 < -2.3f ) phi1 = -2.3f;
    if( phi1 > -0.9f ) phi1 = -0.9f;

    static FLOAT theta1 = 0.0f;
    theta1 -= 3.0f * m_fElapsedTime * m_DefaultGamepad.fY2;
    if( theta1 < +0.9f ) theta1 = +0.9f;
    if( theta1 > +2.3f ) theta1 = +2.3f;

    D3DXVECTOR3 vPtLightPos;
    vPtLightPos.x = 2*cosf( phi1 );
    vPtLightPos.y = 2*cosf( theta1 );
    vPtLightPos.z = 2*sinf( phi1 ) * sinf( theta1 );

    // Setup the point light
    D3DLIGHT8 light;
    ZeroMemory( &light, sizeof(D3DLIGHT8) );
    light.Type         = D3DLIGHT_POINT;
    light.Diffuse.r    = 1.0f;
    light.Diffuse.g    = 1.0f;
    light.Diffuse.b    = 1.0f;
    light.Diffuse.a    = 1.0f;
    light.Position     = vPtLightPos;
    light.Range        = 1000.0f;
    light.Attenuation0 = 1.0f;
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );

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
    RenderGradientBackground( 0xff0000ff, 0xff0000ff );

    // Set default state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,    D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,    D3DTADDRESS_WRAP );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,         TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    m_pd3dDevice->SetPixelShader( NULL );

    // Enable fog (in case pixel pshader might do anything with fog)
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,      0xff0000ff );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,  D3DFOG_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_FOGSTART,      FtoDW(4.0f) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGEND,        FtoDW(6.0f) );

    // Set textures. Texture 2 is transformed with a texture matrix
    m_pd3dDevice->SetTexture( 0, m_pTexture0 );
    m_pd3dDevice->SetTexture( 1, m_pTexture1 );
    m_pd3dDevice->SetTexture( 2, m_pTexture2 );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    m_pd3dDevice->SetTransform( D3DTS_TEXTURE2, &m_matTexture );

    // Specify pixel shader. With a pixel shader in place, we can then modify
    // it on the fly.
    g_pd3dDevice->SetPixelShader( m_dwPixelShader );

    // Show turning on fog-blending in the final combiner
    if( m_bEnableFog )
    {
        DWORD dwPSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
            PS_REGISTER_FOG  | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_R0   | PS_CHANNEL_RGB   | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_FOG  | PS_CHANNEL_RGB   | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_RGB   | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

        m_pd3dDevice->SetRenderState( D3DRS_PSFINALCOMBINERINPUTSABCD, dwPSFinalCombinerInputsABCD );
    }

    // Change the mask texture in stage 0 from t2 (the checkerboard texture) to 
    // t1 (the robot texture)
    if( m_bUseRobotMask )
    { 
        DWORD dwPSRGBInputs0 = PS_COMBINERINPUTS(
            PS_REGISTER_T1   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );
    
        DWORD dwPSAlphaInputs0  = PS_COMBINERINPUTS(
            PS_REGISTER_T1   | PS_CHANNEL_BLUE  | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ONE  | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

        m_pd3dDevice->SetRenderState( D3DRS_PSRGBINPUTS0,   dwPSRGBInputs0 );
        m_pd3dDevice->SetRenderState( D3DRS_PSALPHAINPUTS0, dwPSAlphaInputs0 );
    }

    // Change the combiner count. (Knock off the third combiner)
    if( m_bChangeCombinerCount )
    {
        m_pd3dDevice->SetRenderState( D3DRS_PSCOMBINERCOUNT, 2 );
    }

    // Show changing an input mapping. In this case, invert t0.rgb
    if( m_bChangeMapping )
    { 
        DWORD dwPSRGBInputs1 = PS_COMBINERINPUTS(
            PS_REGISTER_T0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
            PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_T1   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

        m_pd3dDevice->SetRenderState( D3DRS_PSRGBINPUTS1,   dwPSRGBInputs1 );
    }

    // Render some geometry
    m_pd3dDevice->SetStreamSource( 0, m_pCylinderVB, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

    // Restore states
    m_pd3dDevice->SetPixelShader( NULL );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"ModifyPixelShader" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( m_bEnableFog )
            m_Font.DrawText( 64, 325, 0xffffff00, L"Fog enabled" );
        if( m_bUseRobotMask )
            m_Font.DrawText( 64, 350, 0xffffff00, L"Using robot texture as mask" );
        if( m_bChangeCombinerCount )
            m_Font.DrawText( 64, 375, 0xffffff00, L"Combiner count reduced to 2" );
        if( m_bChangeMapping )
            m_Font.DrawText( 64, 400, 0xffffff00, L"Texture 0's mapping inverted" );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




