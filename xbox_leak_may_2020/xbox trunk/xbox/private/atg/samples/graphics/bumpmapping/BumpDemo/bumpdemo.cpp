//-----------------------------------------------------------------------------
// File: BumpDemo.cpp
//
// Desc: 
//
// Hist: 02.19.00 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <XBResource.h>
#include <xgraphics.h>
#include "GearModel.h"

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
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate scene" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_1, L"Cycle options" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Toggle gloss map" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, L"Toggle pass thru" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 6




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
static const DWORD rgTex[] = { PS_REGISTER_T0, PS_REGISTER_T1, 
                               PS_REGISTER_T2, PS_REGISTER_T3 };




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource       m_xprResource;      // Packed resources for the app
    CXBFont                 m_Font;             // Font class
    CXBHelp                 m_Help;             // Help class
    BOOL                    m_bDrawHelp;        // Whether to draw help

    D3DXVECTOR3             m_vEye;             // Eye location
    D3DXVECTOR3             m_vAt;              // Eye lookat
    D3DXVECTOR3             m_vUp;              // Eye up direction

    FLOAT                   m_fAngleObjYaw;     // Object rotation angles
    FLOAT                   m_fAngleObjPitch;

    BOOL                    m_bDrawGlossMap;
    BOOL                    m_bPassThru;        // Pass tex coords through to combiner

    LPDIRECT3DTEXTURE8      m_pNormalMap;       // Normal map texture
    LPDIRECT3DTEXTURE8      m_pGlossMap;        // Normal map texture
    LPDIRECT3DCUBETEXTURE8  m_pEnvCubeMap;      // Environment cubemap texture

    DWORD                   m_dwDotMapping;
    DWORD                   m_dwInpTexRegister; // Texture input register (T3)

    DWORD                   m_ShaderHandle;     // Pixel shader handle
    DWORD                   m_dwVertexShader;   // Vertex shader handle

    CBumpyGearMesh          m_Gears[5];

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
    m_bDrawHelp        = FALSE;
    
    m_vEye             = D3DXVECTOR3( 0.0f, 0.0f,-5.0f );
    m_vAt              = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vUp              = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    m_fAngleObjYaw     =   0.0f;
    m_fAngleObjPitch   = -90.0f;

    m_bDrawGlossMap    = TRUE;
    m_bPassThru        = FALSE;

    m_pNormalMap       = NULL;
    m_pGlossMap        = NULL;
    m_pEnvCubeMap      = NULL;

    m_dwDotMapping     = PS_DOTMAPPING_MINUS1_TO_1_D3D;
    m_dwInpTexRegister = 3;

    m_ShaderHandle     = 0;
    m_dwVertexShader   = 0;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: Initialize and create our pixel shader
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitPixelShader()
{
    if( m_ShaderHandle )
    {
        m_pd3dDevice->DeletePixelShader( m_ShaderHandle );
        m_ShaderHandle = 0;
    }

    D3DPIXELSHADERDEF psd;
    ZeroMemory( &psd, sizeof(psd) );

    //---------------------------------------------------------------------------
    //  Texture configuration - The following members of the D3DPixelShaderDef   
    //  structure define the addressing modes of each of the four texture stages 
    //---------------------------------------------------------------------------
    if( m_bPassThru )
    {
        psd.PSTextureModes = PS_TEXTUREMODES( PS_TEXTUREMODES_PASSTHRU,
                                              PS_TEXTUREMODES_PASSTHRU,
                                              PS_TEXTUREMODES_PASSTHRU,
                                              PS_TEXTUREMODES_PASSTHRU );
    }
    else
    {
        psd.PSTextureModes = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D,
                                              PS_TEXTUREMODES_DOTPRODUCT,
                                              PS_TEXTUREMODES_DOTPRODUCT,
                                              PS_TEXTUREMODES_DOT_RFLCT_SPEC );
    }

    psd.PSDotMapping = PS_DOTMAPPING( 0,
                                      m_dwDotMapping,
                                      m_dwDotMapping,
                                      m_dwDotMapping );

    psd.PSInputTexture = PS_INPUTTEXTURE( 0,
                                          0,
                                          0,  // 0 and 1 valid
                                          0 ); // 0, 1, and 2 valid
    psd.PSCompareMode = PS_COMPAREMODE(0, 0, 0, 0);

    //---------------------------------------------------------------------------------
    //  Color combiners - The following members of the D3DPixelShaderDef structure     
    //  define the state for the eight stages of color combiners                       
    //---------------------------------------------------------------------------------
    psd.PSCombinerCount = PS_COMBINERCOUNT(
        1,
        PS_COMBINERCOUNT_MUX_LSB | PS_COMBINERCOUNT_SAME_C0 | PS_COMBINERCOUNT_SAME_C1);

    static DWORD dwC0Color = D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff);
    psd.PSConstant0[0] = dwC0Color;
    psd.PSConstant1[0] = 0;

    psd.PSFinalCombinerConstant0 = 0;
    psd.PSFinalCombinerConstant1 = 0;

    m_dwInpTexRegister = min(m_dwInpTexRegister, sizeof(rgTex) / sizeof(rgTex[0]));

    // R0_ALPHA is initialized to T0_ALPHA in stage0

    // a,b,c,d each contain a value from PS_REGISTER, PS_CHANNEL, and PS_INPUTMAPPING
    psd.PSRGBInputs[0] = PS_COMBINERINPUTS(
        rgTex[m_dwInpTexRegister] | PS_INPUTMAPPING_SIGNED_IDENTITY | PS_CHANNEL_RGB,   // A
        PS_REGISTER_C0, //PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT,                             // B
        PS_REGISTER_ZERO,                                                               // C
        PS_REGISTER_ZERO);                                                              // D

    // R0_ALPHA is initialized to T0_ALPHA in stage0
    psd.PSAlphaInputs[0] = PS_COMBINERINPUTS(
        PS_REGISTER_T0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,          // A
        PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT,                             // B
        PS_REGISTER_ZERO,                                                               // C
        PS_REGISTER_ZERO);                                                              // D

    // mux_sum must be DISCARD if either AB_DOT_PRODUCT or CD_DOT_PRODUCT are set
    //  ie: Dot / Dot / Discard || Dot / Mult / Discard || Mult / Dot / Discard

    // ab,cd,mux_sum contain a value from PS_REGISTER
    // flags contains values from PS_COMBINEROUTPUT
    psd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,                                                                 // ab
        PS_REGISTER_DISCARD,                                                            // cd
        PS_REGISTER_DISCARD,                                                            // mux_sum
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY);                    // flags
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,                                                                 // ab
        PS_REGISTER_DISCARD,                                                            // cd
        PS_REGISTER_DISCARD,                                                            // mux_sum
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY);                    // flags

    // FOG ALPHA is only available in final combiner
    // V1R0_SUM and EF_PROD are only available in final combiner A,B,C,D inputs
    // V1R0_SUM_ALPHA and EF_PROD_ALPHA are not available

    // AB + (1-A)C + D
    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO,                                                               // A
        PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,            // B
        PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,            // C
        PS_REGISTER_ZERO);                                                              // D

    // E,F,G contain a value from PS_REGISTER, PS_CHANNEL, and PS_INPUTMAPPING
    // flags contains values from PS_FINALCOMBINERSETTING:
    //    PS_FINALCOMBINERSETTING_CLAMP_SUM         // V1+R0 sum clamped to [0,1]
    //    PS_FINALCOMBINERSETTING_COMPLEMENT_V1     // unsigned invert mapping
    //    PS_FINALCOMBINERSETTING_COMPLEMENT_R0     // unsigned invert mapping
    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO,                                                               // E
        PS_REGISTER_ZERO,                                                               // F
        PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,          // G (alpha)
        PS_FINALCOMBINERSETTING_CLAMP_SUM);                                             // flags

    return m_pd3dDevice->CreatePixelShader( &psd, &m_ShaderHandle );
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
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a texture to render into
    m_pEnvCubeMap     = m_xprResource.GetCubemap( resource_Lobby_OFFSET);
    m_pNormalMap      = m_xprResource.GetTexture( resource_GearBump_OFFSET );
    m_pGlossMap       = m_xprResource.GetTexture( resource_Gear_OFFSET );

    // Initialize pixel shader
    InitPixelShader();

    // Define the shader declaration. Vertex components come in two streams:
    // the first with the typical vertex components (position, normal, and
    // texcoords) and the second with vertex's basis vectors.
    DWORD dwShaderVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),    // Position
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),    // Normal
        D3DVSD_REG( 2, D3DVSDT_FLOAT2 ),    // Base texture coordinate
        D3DVSD_STREAM( 1 ),
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),    // S basis vector
        D3DVSD_REG( 4, D3DVSDT_FLOAT3 ),    // T basis vector
        D3DVSD_REG( 5, D3DVSDT_FLOAT3 ),    // SxT basis vector
        D3DVSD_END()
    };

    // Init vertex shader
    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\BumpShader.xvu",
                                           dwShaderVertexDecl, &m_dwVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create some geometry
    m_Gears[0].Create( m_pd3dDevice, 0.45f, 3.0f, 15, 0.36f );
    m_Gears[1].Create( m_pd3dDevice, 0.60f,-3.0f, 15, 0.08f );
    m_Gears[2].Create( m_pd3dDevice, 0.90f, 1.5f, 30, 0.00f );
    m_Gears[3].Create( m_pd3dDevice, 0.90f,-0.5f, 30, 0.00f );
    m_Gears[4].Create( m_pd3dDevice, 0.30f, 1.5f, 10, 0.25f );

    // Offset the newly created gears
    D3DXMatrixTranslation( &m_Gears[0].m_matPosition, 1.08f, 0.00f, 1.03f );
    D3DXMatrixTranslation( &m_Gears[1].m_matPosition, 0.00f, 0.00f, 1.03f );
    D3DXMatrixTranslation( &m_Gears[2].m_matPosition, 0.00f, 0.00f,-0.50f );
    D3DXMatrixTranslation( &m_Gears[3].m_matPosition,-1.23f, 0.20f,-0.50f );
    D3DXMatrixTranslation( &m_Gears[4].m_matPosition, 0.00f, 0.20f,-0.50f );

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

    // Move scene
    m_fAngleObjYaw   += m_DefaultGamepad.fX1 * m_fElapsedTime * 100.0f;
    m_fAngleObjPitch += m_DefaultGamepad.fY1 * m_fElapsedTime * 100.0f;

    // Toggle options
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] ) 
    {
        m_bDrawGlossMap  = !m_bDrawGlossMap;
        InitPixelShader();
    }
    
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ) 
    {
        m_bPassThru = !m_bPassThru;
        InitPixelShader();
    }

    // X button cycles through modes
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) 
    {
        m_dwInpTexRegister = (m_dwInpTexRegister + 1) % (sizeof(rgTex) / sizeof(rgTex[0]));
        InitPixelShader();
    }

    // Update the rotation of the gears
    m_Gears[0].FrameMove( m_pd3dDevice, m_fAppTime );
    m_Gears[1].FrameMove( m_pd3dDevice, m_fAppTime );
    m_Gears[2].FrameMove( m_pd3dDevice, m_fAppTime );
    m_Gears[3].FrameMove( m_pd3dDevice, m_fAppTime );
    m_Gears[4].FrameMove( m_pd3dDevice, m_fAppTime );

    // Set the matrices
    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixRotationYawPitchRoll( &matWorld, m_fAngleObjYaw * D3DX_PI / 180.0f,
                                    m_fAngleObjPitch * D3DX_PI / 180.0f, 0.0f );
    D3DXMatrixLookAtRH( &matView, &m_vEye, &m_vAt, &m_vUp );
    D3DXMatrixPerspectiveFovRH( &matProj, D3DX_PI/4, 640.0f/480.0f, 1.0f, 10000.0f );
    
    g_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

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
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x000000ff, 1.0f, 0L );

    // Set default states
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    // Set states for drawing the bump effect
    m_pd3dDevice->SetTexture( 0, m_pNormalMap );
    m_pd3dDevice->SetTexture( 2, m_pEnvCubeMap );
    m_pd3dDevice->SetTexture( 3, m_pEnvCubeMap );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,       D3DCMP_GREATEREQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x00000001 );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetVertexShader( m_dwVertexShader );
    m_pd3dDevice->SetPixelShader( m_ShaderHandle );

    // Render the gears
    m_Gears[0].Render( m_pd3dDevice );
    m_Gears[1].Render( m_pd3dDevice );
    m_Gears[2].Render( m_pd3dDevice );
    m_Gears[3].Render( m_pd3dDevice );
    m_Gears[4].Render( m_pd3dDevice );

    // Restore the state
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetTexture( 2, NULL );
    m_pd3dDevice->SetTexture( 3, NULL );

    // Add the glossmap in a 2nd pass
    if( m_bDrawGlossMap )
    {
        // Draw gloss map
        m_pd3dDevice->SetTexture( 0, m_pGlossMap );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_SRCCOLOR );

        m_Gears[0].Render( m_pd3dDevice );
        m_Gears[1].Render( m_pd3dDevice );
        m_Gears[2].Render( m_pd3dDevice );
        m_Gears[3].Render( m_pd3dDevice );
        m_Gears[4].Render( m_pd3dDevice );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"BumpDemo" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Output description of what's being displayed
        switch( m_dwInpTexRegister )
        {
            case 0:  m_Font.DrawText( 64, 75, 0xffffff00, L"Normal map only" );  break;
            case 1:  m_Font.DrawText( 64, 75, 0xffffff00, L"1st cubemap only" ); break;
            case 2:  m_Font.DrawText( 64, 75, 0xffffff00, L"2nd cubemap only" ); break;
            default: m_Font.DrawText( 64, 75, 0xffffff00, L"Full effect" );      break;
        }

        // Output state of options
        m_Font.DrawText( 64, 100, 0xffffff00, m_bDrawGlossMap ? L"Glossmap ON" : L"Glossmap OFF" );
        m_Font.DrawText( 64, 125, 0xffffff00, m_bPassThru ? L"Pass thru TRUE" : L"Pass thru FALSE" );
        m_Font.End();
    }
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



