//-----------------------------------------------------------------------------
// File: Patch.cpp
//
// Desc: Example code showing how to use higher-order primitives (rectangular 
//       patches) in D3D.
//
// Hist: 12.18.00 - New for February XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include "Teapot.h"



//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,   XBHELP_PLACEMENT_1, L"Rotate model" },
    { XBHELP_X_BUTTON,    XBHELP_PLACEMENT_2, L"Inc # of\nsegments" },
    { XBHELP_A_BUTTON,    XBHELP_PLACEMENT_2, L"Dec # of\nsegments" },
    { XBHELP_Y_BUTTON,    XBHELP_PLACEMENT_2, L"Toggle\nwireframe" },
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_2, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 5




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    D3DXVECTOR3 p; // The original position of the vertex
};




// Surface handles should start with 1 and be assigned incrementally for 
// efficiency, since patches are really stored in an array. To do otherwise
// would unnecessarily result in waste of memory for the handles array
#define SURFACEHANDLEBASE 1




// The data used by this sample is made from 4x4 patches
#define PATCH_SIZE 4




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont                 m_Font;
    CXBHelp                 m_Help;
    BOOL                    m_bDrawHelp;

    LPDIRECT3DVERTEXBUFFER8 m_pVB;            // Buffer to hold vertices
    DWORD                   m_hVShader;       // Handle to vertex shader

    DWORD                   m_dwNumSegments;  // Amount to tessellate
    BOOL                    m_bRetessellate;  // Need to retessellate patches
    UINT                    m_dwNumPatches;
    BOOL                    m_bWireframe;

protected:
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

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
// Name: CXBoxSample()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Initialize class members
    m_bDrawHelp         = FALSE;  // Help flag

    m_pVB               = NULL;   // Geometry
    m_bWireframe        = FALSE;

    m_dwNumSegments     = 8;      // Initial # of patches
    m_bRetessellate     = TRUE;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependent managed objects, such as managed
//       textures and managed vertex buffers.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the vertex buffer
    UINT dwNumVertices = sizeof(g_vTeapotData) / sizeof(D3DXVECTOR3);
    m_dwNumPatches = dwNumVertices / (PATCH_SIZE * PATCH_SIZE);

    if( FAILED( m_pd3dDevice->CreateVertexBuffer( dwNumVertices*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_RTPATCHES, 0L, 
                                                  D3DPOOL_DEFAULT, &m_pVB ) ) )
        return E_FAIL;

    // Create a vertex buffer and fill it with our vertices.
    CUSTOMVERTEX* pVertices;
    m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
    for( DWORD i=0; i<dwNumVertices; i++ )
        pVertices[i].p = g_vTeapotData[i] - g_vTeapotCenter;
    m_pVB->Unlock();

    // Set up matrices
    D3DXMATRIX matWorld, matView, matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 3.0f,-400.0f ), 
                                  &D3DXVECTOR3( 0.0f, 0.0f,   0.0f ), 
                                  &D3DXVECTOR3( 0.0f, 1.0f,   0.0f ) );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 1.0f, 800.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set up a material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 0.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up a directional light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0f, -1.0f, 1.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );

    // Vertex declaration
    DWORD decl[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG( D3DVSDE_POSITION, D3DVSDT_FLOAT3 ),
        // Data generation section implemented as a virtual stream
        D3DVSD_STREAM_TESS(),
        // Generate normal using the position input and copy it to the normal
        // register (output)
        D3DVSD_TESSNORMAL( D3DVSDE_POSITION, D3DVSDE_NORMAL ),
        D3DVSD_END()
    };

    // Create the tesselating vertex shader
    m_pd3dDevice->CreateVertexShader( decl, NULL, &m_hVShader, 0 );
    
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

    // Check user input for changing options
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        if( m_dwNumSegments < 16 )
            m_dwNumSegments++;

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        if( m_dwNumSegments > 1 )
            m_dwNumSegments--;

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        m_bWireframe = !m_bWireframe;
    
    // Perform object rotation
    static D3DXMATRIX matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
    D3DXMATRIX matRotate;
    FLOAT fXRotate1 = m_DefaultGamepad.fX1*m_fElapsedTime*D3DX_PI*0.5f;
    FLOAT fYRotate1 = m_DefaultGamepad.fY1*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate1, -fYRotate1, 0.0f );
    D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff000000, 0xff0000ff );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0x00101010 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

    // Set fillmode options
    m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, m_bWireframe ? D3DCULL_NONE : D3DCULL_CCW );

    // Render the patches
    m_pd3dDevice->SetVertexShader( m_hVShader );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(CUSTOMVERTEX) );

    for( DWORD patch = 0; patch < m_dwNumPatches; patch++ ) 
    {
        FLOAT fNumEdgeSegments[PATCH_SIZE];
        for( DWORD i = 0; i<PATCH_SIZE; i++ )
            fNumEdgeSegments[i] = (FLOAT)m_dwNumSegments;
    
        // First time through, tesellate
        if( m_bRetessellate ) 
        {
            // Setup the patch info, where the geometry is a long narrow (1 × N)
            // rectangular patch (a cubic, bezier strip). Note that all the units
            // in this structure are in number of vertices
            D3DRECTPATCH_INFO rect;
            rect.Width  = PATCH_SIZE;      // Width of each vertex
            rect.Height = PATCH_SIZE;      // Height of each vertex
            rect.Stride = PATCH_SIZE;      // Stride between segments
            rect.Basis  = D3DBASIS_BEZIER; // Basis type for the patch
            rect.Order  = D3DORDER_CUBIC;  // Order type for the patch
            rect.StartVertexOffsetWidth  = 0;                  // Starting vertex offset width
            rect.StartVertexOffsetHeight = patch * PATCH_SIZE; // Starting vertex offset height

            // Tessellate using the patch info
            m_pd3dDevice->DrawRectPatch( SURFACEHANDLEBASE + patch, 
                                         fNumEdgeSegments, &rect );
        }
        else 
        {
            // After the first time, the patch is cached, so we can use the
            // existing surface handles. Note: This is more effecient, but if we
            // were using dynamic patches, we could specify the D3DRECTPATCH_INFO
            // structure every time.
            m_pd3dDevice->DrawRectPatch( SURFACEHANDLEBASE + patch, 
                                         fNumEdgeSegments, NULL );
        }
    }

    // Reset the flag that triggers tesselation
    m_bRetessellate = FALSE;
    
    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR strBuffer[50];
        swprintf( strBuffer, L"%d segments per patch", m_dwNumSegments );

        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"Patch" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.DrawText(  64, 75, 0xffffffff, strBuffer );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




