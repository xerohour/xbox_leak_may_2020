// File: DebugChannel.cpp
//
// Desc: Sample to demonstrate how to communicate with a development
//       system over the debug channel.  This sample contains an
//       External Command Processor which will communicate with
//       the Xbox Remote Debug Console sample.
//
// Hist: 02.05.01 - Initial creation for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBResource.h>
#include <xgraphics.h>
#include "DebugCmd.h"

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
    { "Flame1.bmp",       resource_Flame1_OFFSET },
    { "forestground.bmp", resource_ForestGround_OFFSET },
    { "grydirt1.bmp",     resource_GryDirt1_OFFSET },
    { "Waterbumps.bmp",   resource_WaterBumps_OFFSET },
    { NULL, 0 },
};




// Define our vertex format
typedef struct _CUSTOMVERTEX
{
    D3DXVECTOR3 p;          // Position
    D3DXVECTOR3 n;          // normal
    D3DCOLOR    diffuse;    // Diffuse color
    FLOAT       tu, tv;     // Texture coordinates
} CUSTOMVERTEX;
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Display framerate\nto console" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Display texture\nto console" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 4

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBPackedResource       m_xprResource;          // Packed resources for the app
    CXBFont                 m_Font;                 // Font class
    CXBHelp                 m_Help;                 // Help class
    BOOL                    m_bDrawHelp;            // Whether to draw help

    LPDIRECT3DVERTEXBUFFER8 m_pQuad;                // Quad to display
    float                   m_fRadians;             // Radians of rotation

    LPDIRECT3DTEXTURE8      m_ptTex;                // Texture for quad
    BOOL                    m_bForward;             // TRUE if we should spin CW
    float                   m_fRPS;                 // Rotation speed in radians per second
    char                    m_szTexFile[MAX_PATH];  // Filename of texture
    D3DLIGHT8               m_Light;                // Light

    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

    CXBoxSample();
};

//-----------------------------------------------------------------------------
// Variable declaration block:
// These are the variables exposed to the "set" command. Don't forget to update
// g_nVars to reflect the number of entries
//-----------------------------------------------------------------------------
CXBoxSample g_xbApp;
const DCCMDVARDEF g_rgDCCMDVars[] = 
{
    { "bForward", &g_xbApp.m_bForward, SDOS_bool, NULL },
    { "red", &g_xbApp.m_Light.Diffuse.r, SDOS_R4, RCmdLightChange },
    { "green", &g_xbApp.m_Light.Diffuse.g, SDOS_R4, RCmdLightChange },
    { "blue", &g_xbApp.m_Light.Diffuse.b, SDOS_R4, RCmdLightChange },
};
const UINT g_nVars = 4;


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
void __cdecl main()
{
    // Note: We defined g_xbApp as a global to give access to debug channel
    if( FAILED( g_xbApp.Create() ) )
        return;

    g_xbApp.Run();

}

//-----------------------------------------------------------------------------
// Name: CXBoxSample() (constructor)
// Desc: Constructor for Application class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_bForward = TRUE;
    m_pQuad = NULL;
    m_ptTex = NULL;
    m_szTexFile[0] = 0;
    m_fRadians = 0.0f;
    m_fRPS = 0.0f;
    m_bDrawHelp = FALSE;
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Performs whatever initialization is necessary
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Initialize()
{
    CUSTOMVERTEX * pVertices = NULL;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Initialize the help system
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create and set up our vertex buffer for the quad
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ),
                                                  D3DUSAGE_WRITEONLY,
                                                  D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED,
                                                  &m_pQuad ) ) )
        return E_FAIL;

    if( FAILED( m_pQuad->Lock( 0, 0, (BYTE **)&pVertices, 0 ) ) )
        return E_FAIL;

    pVertices[0].diffuse = 0xFFFFFFFF;
    pVertices[0].n = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVertices[0].p = D3DXVECTOR3( -0.5f, -0.5f, 1.0f );
    pVertices[0].tu = 0.0f; pVertices[0].tv = 1.0f;

    pVertices[1].diffuse = 0xFFFFFFFF;
    pVertices[1].n = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVertices[1].p = D3DXVECTOR3( -0.5f, 0.5f, 1.0f );
    pVertices[1].tu = 0.0f; pVertices[1].tv = 0.0f;

    pVertices[2].diffuse = 0xFFFFFFFF;
    pVertices[2].n = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVertices[2].p = D3DXVECTOR3( 0.5f, -0.5f, 1.0f );
    pVertices[2].tu = 1.0f; pVertices[2].tv = 1.0f;

    pVertices[3].diffuse = 0xFFFFFFFF;
    pVertices[3].n = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVertices[3].p = D3DXVECTOR3( 0.5f, 0.5f, 1.0f );
    pVertices[3].tu = 1.0f; pVertices[3].tv = 0.0f;

    // Set up our view & projection matrices
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 1.0f );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 1.0f, 1.0f, 10.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up our light
    ZeroMemory( &m_Light, sizeof( D3DLIGHT8 ) );
    m_Light.Type = D3DLIGHT_DIRECTIONAL;
    m_Light.Diffuse.r = 1.0f;
    m_Light.Diffuse.g = 1.0f;
    m_Light.Diffuse.b = 1.0f;
    m_Light.Diffuse.a = 1.0f;
    m_Light.Direction = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    m_pd3dDevice->SetLight( 0, &m_Light );
    m_pd3dDevice->LightEnable( 0, TRUE );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Performs all per-frame calculations to update the application state
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    D3DXMATRIX matWorld;

    m_fRadians = (float)fmod( m_fRadians + ( m_fRPS * m_fElapsedAppTime * ( m_bForward ? -1.0f : 1.0f ) ), D3DX_PI * 2 );

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationZ( &matWorld, m_fRadians );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Handle requests for debug output
    if( m_DefaultGamepad.bPressedAnalogButtons[0] )
    {
        DCCMDPrintf("Framerate is %0.02f fps\n", m_fFPS);
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[1]  )
    {
        DCCMDPrintf("Current texture is %s\n", m_szTexFile );
    }

    // Process any pending commands from the debug channel
    DCHandleCmds();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x00000000, 1.0f, 0L );

    // Set rendering state
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,    TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,    0xFFFFFFFF );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,   TRUE );

    // Set our texture stages up
    m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    m_pd3dDevice->SetTexture( 0, m_ptTex );
    if( m_ptTex )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    }
    else
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    }

    // Draw the quad
    m_pd3dDevice->SetStreamSource( 0, m_pQuad, sizeof( CUSTOMVERTEX ) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
    {
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    }
    else
    {
        m_Font.DrawText(  64, 50, 0xffffffff, L"DebugChannel" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RCmdTexture
// Desc: Sets the new texture to be used
//-----------------------------------------------------------------------------
void RCmdTexture(int argc, char *argv[])
{
    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify a filename\n");
        return;
    }

    // Release the old texture
    if( g_xbApp.m_ptTex )
        g_xbApp.m_ptTex->Release();

    // Try to grab the new texture
    lstrcpyA( g_xbApp.m_szTexFile, argv[1] );

    g_xbApp.m_ptTex = g_xbApp.m_xprResource.GetTexture( g_xbApp.m_szTexFile );
    if( NULL == g_xbApp.m_ptTex )
    {
        DCCMDPrintf("Couldn't find %s", g_xbApp.m_szTexFile );
        return;
    }
}

//-----------------------------------------------------------------------------
// Name: RCmdSpin
// Desc: Sets the new spin velocity, limited to 2pi in either direction
//-----------------------------------------------------------------------------
void RCmdSpin(int argc, char *argv[])
{
    float fVel;

    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify a velocity\n");
        return;
    }

    fVel = (float)atof( argv[1] );

    if( fabs( fVel ) > D3DX_PI * 2 )
    {
        DCCMDPrintf("Velocity should be betweeen +/- 2 * pi\n");
        return;
    }

    // Set our state
    g_xbApp.m_fRPS = (float)fabs( fVel );
    g_xbApp.m_bForward = ( fVel > 0 );
}

//-----------------------------------------------------------------------------
// Name: RCmdLightChange
// Desc: Called after changing one of our lighting values, so that we can
//       reset the light
//-----------------------------------------------------------------------------
void RCmdLightChange( void * pvAddr )
{
    g_pd3dDevice->SetLight( 0, &g_xbApp.m_Light );
}