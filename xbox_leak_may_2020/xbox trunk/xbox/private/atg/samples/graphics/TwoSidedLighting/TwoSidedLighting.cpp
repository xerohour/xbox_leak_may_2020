//-----------------------------------------------------------------------------
// File: TwoSidedLighting.cpp
//
// Desc: Example code showing how to do two-sided lighting.
//
// Hist: 08.13.01 - New for September XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBUtil.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_1, L"Toggle vertex shader" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// Name: struct CUSTOMVERTEX
// Desc: Custom vertex 
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The 3D position for the vertex
    D3DXVECTOR3 normal;   // The surface normal for the vertex
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont      m_Font;              // Font class
    CXBHelp      m_Help;              // Help class
    BOOL         m_bDrawHelp;         // Whether to draw help

    LPDIRECT3DVERTEXBUFFER8 m_pVB;    // Object to use for two-sdied lighting

    DWORD        m_dwVertexShader;    // Vertex shader
    BOOL         m_bUseVertexShader;

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
// Name: CXBoxSample()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_bDrawHelp        = FALSE;
    m_pVB              = NULL;
    m_dwVertexShader   = 0L;
    m_bUseVertexShader = FALSE;
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

    // Create geometry for a cylinder
    {
        // Create the vertex buffer
        if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
                                                      0, D3DFVF_CUSTOMVERTEX,
                                                      D3DPOOL_DEFAULT, &m_pVB ) ) )
            return E_FAIL;

        // Fill the vertex buffer
        CUSTOMVERTEX* pVertices;
        m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
        for( DWORD i=0; i<50; i++ )
        {
            FLOAT theta = (2*D3DX_PI*i)/(50-1);
            pVertices[2*i+0].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
            pVertices[2*i+0].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
            pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta),-1.0f, cosf(theta) );
            pVertices[2*i+1].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
        }
        m_pVB->Unlock();
    }

    // Setup the vertex declaration
    DWORD dwDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),     // v0 = Position
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),     // v3 = Normal
        D3DVSD_END()
    };

    // Create vertex shader from a file
    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\TwoSided.xvu", 
                                           dwDecl, &m_dwVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set transforms
    D3DXMATRIX matWorld, matView,  matProj;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 3.0f,-5.0f ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Create a directional light. (Use yellow light to distinguish from
    // vertex shader case.)
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -0.5f, -1.0f, 1.0f );
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 0.0f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );

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

    // Toggle use of vertex shader
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        m_bUseVertexShader = !m_bUseVertexShader; 

    // Rotate the object
    D3DXMATRIX matWorld;
    D3DXMatrixRotationX( &matWorld, m_fAppTime );

    // Set up a white, directional light, with an oscillating direction.
    FLOAT       fAngle = 3.0f * m_fAppTime;
    D3DXVECTOR3 vLightDirection( cosf(fAngle), 0.1f, sinf(fAngle) );
    D3DXVec3Normalize( &vLightDirection, &vLightDirection );

    // Set the vertex shader constants. 
    if( m_bUseVertexShader )        
    {
        // Some basic constants
        D3DXVECTOR4 vZero(0,0,0,0);
        D3DXVECTOR4 vOne(1,1,1,1);

        // Material colors. Frontside is yellow, and backside is red
        D3DXVECTOR4 vFrontDiffuse( 1.00f, 1.00f, 0.00f, 1.00f );
        D3DXVECTOR4 vFrontAmbient( 0.25f, 0.25f, 0.25f, 1.00f );
        D3DXVECTOR4 vBackDiffuse( 1.00f, 0.00f, 0.00f, 1.00f );
        D3DXVECTOR4 vBackAmbient( 0.25f, 0.25f, 0.25f, 1.00f );

        // Vertex shader operations use transposed matrices
        D3DXMATRIX matView, matProj;
        D3DXMATRIX matWorldView, matWorldViewProj;
        D3DXMATRIX matWorldTranspose, matWorldViewProjTranspose;
        m_pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
        m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
        D3DXMatrixMultiply( &matWorldView, &matWorld, &matView );
        D3DXMatrixMultiply( &matWorldViewProj, &matWorldView, &matProj );
        D3DXMatrixTranspose( &matWorldTranspose, &matWorld );
        D3DXMatrixTranspose( &matWorldViewProjTranspose, &matWorldViewProj );

        // Set the vertex shader constants
        m_pd3dDevice->SetVertexShaderConstant(  0, &vZero, 1 );
        m_pd3dDevice->SetVertexShaderConstant(  1, &vOne,  1 );
        m_pd3dDevice->SetVertexShaderConstant(  4, &matWorldTranspose,         4 );
        m_pd3dDevice->SetVertexShaderConstant( 12, &matWorldViewProjTranspose, 4 );
        m_pd3dDevice->SetVertexShaderConstant( 20, &vLightDirection, 1 );
        m_pd3dDevice->SetVertexShaderConstant( 21, &vFrontDiffuse,   1 );
        m_pd3dDevice->SetVertexShaderConstant( 22, &vFrontAmbient,   1 );
        m_pd3dDevice->SetVertexShaderConstant( 23, &vBackDiffuse,    1 );
        m_pd3dDevice->SetVertexShaderConstant( 24, &vBackAmbient,    1 );
    }
    else
    {
        // Set the world matrix
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        // Set up the materials. The frontside material here is yellow, and the
        // backside material is red.
        D3DMATERIAL8 mtrl;
        ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
        mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
        mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
        mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
        mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
        g_pd3dDevice->SetMaterial( &mtrl );

        mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
        mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
        mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
        mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
        g_pd3dDevice->SetBackMaterial( &mtrl );

        // Setup a directional light
        D3DLIGHT8 light;
        ZeroMemory( &light, sizeof(D3DLIGHT8) );
        light.Type       = D3DLIGHT_DIRECTIONAL;
        light.Diffuse.r  = 1.0f;
        light.Diffuse.g  = 1.0f;
        light.Diffuse.b  = 1.0f;
        light.Direction  = vLightDirection;
        light.Range      = 1000.0f;
        g_pd3dDevice->SetLight( 0, &light );
        g_pd3dDevice->LightEnable( 0, TRUE );
    }
    
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
    // Render a background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

    // Enable two-sided lighting
    m_pd3dDevice->SetRenderState( D3DRS_TWOSIDEDLIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,    D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,     0xff404040 );
    m_pd3dDevice->SetRenderState( D3DRS_BACKAMBIENT, 0xff404040 );
    
    // Set the vertex shader (programmable or fixed)
    if( m_bUseVertexShader )        
        m_pd3dDevice->SetVertexShader( m_dwVertexShader );
    else
        m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );

    // Render the cyclinder
    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

    // Restore state
    m_pd3dDevice->SetRenderState( D3DRS_TWOSIDEDLIGHTING, FALSE );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"TwoSidedLighting" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( m_bUseVertexShader )        
            m_Font.DrawText( 64, 75, 0xffffffff, L"Using vertex shader" );
        else
            m_Font.DrawText( 64, 75, 0xffffffff, L"Using fixed-function API" );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



