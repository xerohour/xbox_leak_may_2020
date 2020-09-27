//-----------------------------------------------------------------------------
// File: PixelShader.cpp
//
// Desc: Demonstrates the use of pixel shaders. The sample shows two different
//       pixel shaders (one created manually, and one file-based) on two
//       different geometries (a cylinder or a quad).
//
//       The hard-coded pixel shader uses texture 2 (T2, in shader speak) as a
//       mask to select between texture 0 (T0) and texture 1 (T1). The shader 
//       also modulates in the diffuse component (V0).
//
//       The file-based shader does a different effect. The diffuse color (V0)
//       is used as a factor to linearly-interpolate between textures 0 and 1
//       (T0 and T1). Note that the second texture is not used in this shader.
//
// Hist: 12.15.00 - New for December XDK release
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
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Texture0.tga", resource_Texture0_OFFSET },
    { "Texture1.tga", resource_Texture1_OFFSET },
    { "Checker.bmp",  resource_Checker_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate object" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Move light" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_1, L"Zoom" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\npshader" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Toggle\nmodels" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, L"Toggle\nwireframe" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display\nhelp" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 8




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;   // The position
    D3DXVECTOR3 normal;     // The vertex normals
    D3DCOLOR    color;      // The color
    FLOAT       tu0, tv0;   // Texture 0 coordinates
    FLOAT       tu1, tv1;   // Texture 1 coordinates
    FLOAT       tu2, tv2;   // Texture 2 coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEX2|D3DFVF_TEX3)




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
    LPDIRECT3DVERTEXBUFFER8 m_pQuadVB;     // Buffer for quad vertices
    LPDIRECT3DTEXTURE8      m_pTexture0;
    LPDIRECT3DTEXTURE8      m_pTexture1;
    LPDIRECT3DTEXTURE8      m_pTexture2;

    UINT         m_bCylinder;        // Displayed model
    BOOL         m_bHardcodedShader; // Render type
    BOOL         m_bWireframe;

    DWORD        m_dwPixelShader1;   // Handle for pixel shader 1
    DWORD        m_dwPixelShader2;   // Handle for pixel shader 2

    D3DXMATRIX   m_matObject;        // Transform matrix for the object
    D3DXMATRIX   m_matTexture;       // Transform matrix for the texture
    FLOAT        m_fEyeScale;        // Scale of viewing distance

    HRESULT InitGeometry();

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
    m_bDrawHelp        = FALSE;

    m_bCylinder        = TRUE;
    m_bHardcodedShader = TRUE;
    m_bWireframe       = FALSE;
    m_pCylinderVB      = NULL;
    m_pQuadVB          = NULL;
    m_pTexture0        = NULL;
    m_pTexture1        = NULL;
    m_pTexture2        = NULL;
    m_dwPixelShader1   = 0L;
    m_dwPixelShader2   = 0L;
    m_fEyeScale        = 1.0f;

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
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the textures
	m_pTexture0 = m_xprResource.GetTexture( "Texture0.tga" );
	m_pTexture1 = m_xprResource.GetTexture( "Texture1.tga" );
	m_pTexture2 = m_xprResource.GetTexture( "Checker.bmp" );
    
    // Create geometry for a quad
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pQuadVB ) ) )
        return E_FAIL;

    CUSTOMVERTEX* v;
    m_pQuadVB->Lock( 0, 0, (BYTE**)&v, 0 );
    for( DWORD i=0; i<4; i++ )
    {
        FLOAT x = i<2 ? 1.0f : -1.0f;
        FLOAT y = i%2 ? 1.0f : -1.0f;

        v[i].position = D3DXVECTOR3( x, y, 0.0f );
        v[i].normal   = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
        v[i].tu0      = v[i].tu1 = v[i].tu2 = (1+x)/2;
        v[i].tv0      = v[i].tv1 = v[i].tv2 = (1-y)/2;

        if( i==0 ) v[i].color = 0xff00ff00;
        if( i==1 ) v[i].color = 0xff0000ff;
        if( i==2 ) v[i].color = 0xffff0000;
        if( i==3 ) v[i].color = 0xffffffff;
    }
    m_pQuadVB->Unlock();

    // Create geometry for a cylinder
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pCylinderVB ) ) )
        return E_FAIL;

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    if( FAILED( m_pCylinderVB->Lock( 0, 0, (BYTE**)&v, 0 ) ) )
        return E_FAIL;
    for( i=0; i<50; i++ )
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
    // Stage 0 
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
    // Stage 1
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
    // Stage 2
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
    // Final combiner
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
    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwPixelShader1 ) ) )
    {
        OUTPUT_DEBUG_STRING( "Could not create hardcoded pixel shader.\n" );
        return E_FAIL;
    }

    // Create a file-based pixel shader. This shader uses the difffuse value to
    // linearly interpolate between the first two textures, as in:
    //      ps.1.0
    //      tex t0
    //      tex t1
    //      mov r1, t1
    //      lrp r0, v0, t0, r1
    if( FAILED( XBUtil_CreatePixelShader( m_pd3dDevice, "D:\\Media\\Shaders\\PShader.xpu",
                                          &m_dwPixelShader2 ) ) )
    {
        OUTPUT_DEBUG_STRING( "Could not create file-based pixel shader.\n" );
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

    // Toggle pixel shaders
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        m_bHardcodedShader = !m_bHardcodedShader;

    // Toggle models
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        m_bCylinder = !m_bCylinder;

    // Toggle wireframe
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
        m_bWireframe = !m_bWireframe;

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
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

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
    
    if( m_bWireframe )
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

    // Set textures. Texture 2 is transformed with a texture matrix
    m_pd3dDevice->SetTexture( 0, m_pTexture0 );
    m_pd3dDevice->SetTexture( 1, m_pTexture1 );
    m_pd3dDevice->SetTexture( 2, m_pTexture2 );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    m_pd3dDevice->SetTransform( D3DTS_TEXTURE2, &m_matTexture );

    // Specify pixel shader
    if( m_bHardcodedShader )
        g_pd3dDevice->SetPixelShader( m_dwPixelShader1 );
    else
        m_pd3dDevice->SetPixelShader( m_dwPixelShader2 );

    // Render some geometry
    if( m_bCylinder )
    {
        // Render the cylinder
        m_pd3dDevice->SetStreamSource( 0, m_pCylinderVB, sizeof(CUSTOMVERTEX) );
        m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );
    }
    else
    {
        // Render the quad
        m_pd3dDevice->SetStreamSource( 0, m_pQuadVB, sizeof(CUSTOMVERTEX) );
        m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    }

    // Restore states
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"PixelShader" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( m_bHardcodedShader )
        {
            m_Font.DrawText( 64,  75, 0xffffff00, L"Using hard-coded shader:" );
            m_Font.DrawText( 64, 100, 0xffffff00, L"Output is:" );
            m_Font.DrawText( 64, 125, 0xffffff00, L"   v0 * (t2.a > 0.5 ? t0 : t1 )" );
            m_Font.DrawText( 64, 150, 0xffffff00, L"Where:" );
            m_Font.DrawText( 64, 175, 0xffffff00, L"   v0 = diffuse color" );
            m_Font.DrawText( 64, 200, 0xffffff00, L"   t0 = robot texture" );
            m_Font.DrawText( 64, 225, 0xffffff00, L"   t1 = girl texture" );
            m_Font.DrawText( 64, 250, 0xffffff00, L"   t2 = checkerboard texture" );
        }
        else
        {
            m_Font.DrawText( 64,  75, 0xffffff00, L"Using file-based shader:" );
            m_Font.DrawText( 64, 100, 0xffffff00, L"Output is:" );
            m_Font.DrawText( 64, 125, 0xffffff00, L"   v0 * t0 + ( 1 - v0 ) * t1" );
            m_Font.DrawText( 64, 150, 0xffffff00, L"Where:" );
            m_Font.DrawText( 64, 175, 0xffffff00, L"   v0 = diffuse color" );
            m_Font.DrawText( 64, 200, 0xffffff00, L"   t0 = robot texture" );
            m_Font.DrawText( 64, 225, 0xffffff00, L"   t1 = girl texture" );
        }

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




