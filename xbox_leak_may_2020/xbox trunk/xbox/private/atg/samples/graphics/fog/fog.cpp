//-----------------------------------------------------------------------------
// File: Fog.cpp
//
// Desc: Example code showing how to do fog in D3D
//
// Perf: Note that this sample has not been performance optimized. It has been
//       structured as is (redundant state setting, etc.) to illustrate the
//       usage of the API.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       07.05.01 - Added a vertex shader
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>
#include <XBResource.h>

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
    { "Seafloor.bmp", resource_Seafloor_OFFSET },
    { NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Change fog\nstart/density" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Change fog\nend" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Change fog\nmode" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle use of\nvertex shader" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 6




//-----------------------------------------------------------------------------
// Structures and Macros
//-----------------------------------------------------------------------------
struct FOGVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};

#define D3DFVF_FOGVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

#define FAR_PLANE  150.0f
#define NEAR_PLANE   1.0f
#define FOG_COLOR  0xff0000ff




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;         // Packed resources for the app
    CXBFont            m_Font;                // Font class
    CXBHelp            m_Help;                // Help class
    BOOL               m_bDrawHelp;           // Whether to draw help

    D3DXMATRIX         m_matWorld;            // Transform matrices
    D3DXMATRIX         m_matView;
    D3DXMATRIX         m_matProj;

    DWORD              m_dwFogMode;           // Fog parameters
    FLOAT              m_fFogStartValue;
    FLOAT              m_fFogEndValue;
    FLOAT              m_fFogDensity;

    LPDIRECT3DVERTEXBUFFER8 m_pColumnVB;      // Scene geometry
    DWORD                   m_dwNumColumnVertices;
    LPDIRECT3DVERTEXBUFFER8 m_pTerrainVB;
    DWORD                   m_dwNumTerrainVertices;
    LPDIRECT3DTEXTURE8      m_pFloorTexture;

    DWORD                   m_dwVertexShader; // Fog vertex shader
    BOOL                    m_bUseVertexShader;

    HRESULT GenerateTerrainDisk( DWORD dwNumSegments, FLOAT fScale );
    HRESULT GenerateColumn( DWORD dwNumSegments, FLOAT fRadius, FLOAT fHeight );
    HRESULT SetupFixedPipeline();
    HRESULT SetupVertexShader();

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
    m_bDrawHelp        = FALSE;

    m_dwFogMode        = D3DFOG_LINEAR;
    m_fFogStartValue   = NEAR_PLANE;
    m_fFogEndValue     = FAR_PLANE;
    m_fFogDensity      = 0.0f;

    m_pTerrainVB       = NULL;
    m_pColumnVB        = NULL;
    m_pFloorTexture    = NULL;

    m_dwVertexShader   = TRUE;
    m_bUseVertexShader = FALSE;
}




//----------------------------------------------------------------------------
// Name: GenerateTerrainDisk()
// Desc: Generates a trianglestrip for a disk
//----------------------------------------------------------------------------
HRESULT CXBoxSample::GenerateTerrainDisk( DWORD dwNumSegments, FLOAT fScale )
{
    HRESULT hr;

    m_dwNumTerrainVertices = 2 * dwNumSegments * (dwNumSegments);

    // Create a vertex buffer
    hr = m_pd3dDevice->CreateVertexBuffer( m_dwNumTerrainVertices*sizeof(FOGVERTEX),
                                           D3DUSAGE_WRITEONLY, D3DFVF_FOGVERTEX,
                                           D3DPOOL_DEFAULT, &m_pTerrainVB );
    if( FAILED(hr) )
        return hr;

    FOGVERTEX* pVertices = NULL;
    hr = m_pTerrainVB->Lock( 0, m_dwNumTerrainVertices*sizeof(FOGVERTEX),
                             (BYTE**)&pVertices, 0 );
    if( FAILED(hr) )
        return hr;

    // Generate a spiralized trianglestrip
    for( DWORD ring = 0; ring < dwNumSegments; ring++ )
    {
        for( DWORD seg=0; seg < dwNumSegments; seg++ )
        {
            FLOAT fTheta = (seg*2*D3DX_PI) / dwNumSegments;
            FLOAT r0     = (ring + fTheta/(2*D3DX_PI))*fScale/dwNumSegments;
            FLOAT r1     = r0 + fScale/dwNumSegments;

            FLOAT x   =  sinf( fTheta );
            FLOAT z   =  cosf( fTheta );

            FLOAT y0  =  sinf(r0*z*z+r0*x*x);
            FLOAT nx0 = -cosf(r0*z*z+r0*x*x)*r0*2*x;
            FLOAT ny0 =  1.0f;
            FLOAT nz0 = -cosf(r0*z*z+r0*x*x)*r0*2*z;

            FLOAT y1  =  sinf(r1*z*z+r1*x*x);
            FLOAT nx1 = -cosf(r1*z*z+r1*x*x)*r1*2*x;
            FLOAT ny1 =  1.0f;
            FLOAT nz1 = -cosf(r1*z*z+r1*x*x)*r1*2*z;

            // Add two vertices to the strip at each step
            pVertices->p.x = r0*x;
            pVertices->p.y = y0;
            pVertices->p.z = r0*z;
            pVertices->n.x = nx0;
            pVertices->n.y = ny0;
            pVertices->n.z = nz0;
            pVertices->tu  = (r0*x)/fScale;
            pVertices->tv  = (r0*z)/fScale;
            pVertices++;

            pVertices->p.x = r1*x;
            pVertices->p.y = y1;
            pVertices->p.z = r1*z;
            pVertices->n.x = nx1;
            pVertices->n.y = ny1;
            pVertices->n.z = nz1;
            pVertices->tu  = (r1*x)/fScale;
            pVertices->tv  = (r1*z)/fScale;
            pVertices++;
        }
    }

    m_pTerrainVB->Unlock();

    return S_OK;
}




//----------------------------------------------------------------------------
// Name: GenerateColumn()
// Desc: Generates a trianglestrip for a column
//----------------------------------------------------------------------------
HRESULT CXBoxSample::GenerateColumn( DWORD dwNumSegments, FLOAT fRadius, FLOAT fHeight )
{
    HRESULT hr;

    m_dwNumColumnVertices = 2 * (dwNumSegments+1);

    // Create a vertex buffer
    hr = m_pd3dDevice->CreateVertexBuffer( m_dwNumColumnVertices*sizeof(FOGVERTEX),
                                           D3DUSAGE_WRITEONLY, D3DFVF_FOGVERTEX,
                                           D3DPOOL_DEFAULT, &m_pColumnVB );
    if( FAILED(hr) )
        return hr;

    FOGVERTEX* pVertices = NULL;
    hr = m_pColumnVB->Lock( 0, m_dwNumColumnVertices*sizeof(FOGVERTEX),
                             (BYTE**)(&pVertices), 0 );
    if( FAILED(hr) )
        return hr;

    // Generate a trianglestrip
    for( DWORD seg=0; seg<=dwNumSegments; seg++ )
    {
        FLOAT fTheta = (2*D3DX_PI*seg)/dwNumSegments;
        FLOAT nx     = sinf(fTheta);
        FLOAT nz     = cosf(fTheta);
        FLOAT r      = fRadius;
        FLOAT u      = (1.0f*seg)/dwNumSegments;

        // Add two vertices to the strip at each step
        pVertices->p.x = r*nx;
        pVertices->p.y = fHeight;
        pVertices->p.z = r*nz;
        pVertices->n.x = nx;
        pVertices->n.y = 0;
        pVertices->n.z = nz;
        pVertices->tu  = u;
        pVertices->tv  = 1;
        pVertices++;

        pVertices->p.x = r*nx;
        pVertices->p.y = -1;
        pVertices->p.z = r*nz;
        pVertices->n.x = nx;
        pVertices->n.y = 0;
        pVertices->n.z = nz;
        pVertices->tu  = u;
        pVertices->tv  = 0;
        pVertices++;
    }

    m_pColumnVB->Unlock();

    return S_OK;
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

    // Create the floor texture
    m_pFloorTexture = m_xprResource.GetTexture( "SeaFloor.bmp" );

    // Generate some geometry for the app
    GenerateTerrainDisk( 80, 100.0f );
    GenerateColumn( 30, 1.0f, 10.0f );

    // Create the vertex shader
    DWORD dwVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // v0 = Position
        D3DVSD_REG( 2, D3DVSDT_FLOAT3 ), // v2 = Normal
        D3DVSD_REG( 5, D3DVSDT_FLOAT2 ), // v5 = Tex coords
        D3DVSD_END()
    };

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Fog.xvu",
                                           dwVertexDecl, &m_dwVertexShader ) ) )
        return E_FAIL;

    // Set the transform matrices
    D3DXMatrixIdentity( &m_matWorld );
    D3DXMatrixIdentity( &m_matView );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4.0f, 4.0f/3.0f, NEAR_PLANE, FAR_PLANE );

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

    // For linear fog, toggle fixed function vs. vertex shader
    if( m_dwFogMode == D3DFOG_LINEAR )
    {
        if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
            m_bUseVertexShader = !m_bUseVertexShader;
    }

    // Adjust fog parameters
    if( m_dwFogMode == D3DFOG_LINEAR )
    {
        m_fFogStartValue += m_DefaultGamepad.fY1*m_fElapsedTime*100.0f;
        if( m_fFogStartValue < NEAR_PLANE )      m_fFogStartValue = NEAR_PLANE;
        if( m_fFogStartValue > FAR_PLANE )       m_fFogStartValue = FAR_PLANE;
        if( m_fFogStartValue > m_fFogEndValue )  m_fFogStartValue = m_fFogEndValue;

        m_fFogEndValue += m_DefaultGamepad.fY2*m_fElapsedTime*100.0f;
        if( m_fFogEndValue < NEAR_PLANE )        m_fFogEndValue = NEAR_PLANE;
        if( m_fFogEndValue > FAR_PLANE )         m_fFogEndValue = FAR_PLANE;
        if( m_fFogEndValue < m_fFogStartValue )  m_fFogEndValue = m_fFogStartValue;
    }
    else
    {
        m_fFogDensity += m_DefaultGamepad.fY1*m_fElapsedTime*0.1f;
        if( m_fFogDensity < 0.0f ) m_fFogDensity = 0.0f;
        if( m_fFogDensity > 1.0f ) m_fFogDensity = 1.0f;
    }

    if( FALSE == m_bUseVertexShader )
    {
        // Cycle through fog modes
        if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        {
            if( m_dwFogMode == D3DFOG_LINEAR )      m_dwFogMode = D3DFOG_EXP;
            else if( m_dwFogMode == D3DFOG_EXP )    m_dwFogMode = D3DFOG_EXP2;
            else if( m_dwFogMode == D3DFOG_EXP2 )   m_dwFogMode = D3DFOG_LINEAR;
        }
    }

    // Move the camera along an ellipse
    D3DXVECTOR3 vFrom( 50*sinf(m_fAppTime/2), 5.0f, 60*cosf(m_fAppTime/2) );
    D3DXVECTOR3 vAt( 50*sinf(m_fAppTime/2+1.5f), 4.0f, 60*cosf(m_fAppTime/2+1.5f) );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_matView, &vFrom, &vAt, &vUp );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetupFixedPipeline()
// Desc: Set fixed pipeline render states for the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::SetupFixedPipeline()
{
    // Set the FVF for the fixed pipeline
    m_pd3dDevice->SetVertexShader( D3DFVF_FOGVERTEX );

    // Set the transform matrices
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Set up the light and material
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 50.0f, 0.0f );
    light.Attenuation0 =  1.0f;
    light.Range        = 200.0f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,    TRUE );

    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x44444444 );

    // Set fog parameters.
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,     FOG_COLOR );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, m_dwFogMode );
    m_pd3dDevice->SetRenderState( D3DRS_FOGSTART,     FtoDW(m_fFogStartValue) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGEND,       FtoDW(m_fFogEndValue) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGDENSITY,   FtoDW(m_fFogDensity) );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetupVertexShader()
// Desc: Set vertex shader constants for the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::SetupVertexShader()
{
    // Set the vertex shader
    m_pd3dDevice->SetVertexShader( m_dwVertexShader );

    // Set the transform matrices
    D3DXMATRIX matW, matWV, matWVP;
    D3DXMatrixMultiply( &matWV,  &m_matWorld, &m_matView );
    D3DXMatrixMultiply( &matWVP, &matWV, &m_matProj );
    D3DXMatrixTranspose( &matW,   &m_matWorld ); 
    D3DXMatrixTranspose( &matWV,  &matWV ); 
    D3DXMatrixTranspose( &matWVP, &matWVP );
    m_pd3dDevice->SetVertexShaderConstant( 10, &matW,   4 );
    m_pd3dDevice->SetVertexShaderConstant( 20, &matWV,  4 );
    m_pd3dDevice->SetVertexShaderConstant( 30, &matWVP, 4 );

    // Set constants for the light
    D3DXVECTOR4 vZeroes( 0.0f, 0.0f, 0.0f, 0.0f );
    D3DXVECTOR4 vLight( 0.0f, 50.0f, 0.0f, 0.0f );
    D3DXVECTOR4 vDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
    D3DXVECTOR4 vAmbient( 0x44/255.0f, 0x44/255.0f, 0x44/255.0f, 1.0f );
    
    // Put the light in object space)
    D3DXMATRIX matInvWorld;
    D3DXMatrixInverse( &matInvWorld, NULL, &m_matWorld );
    D3DXVec3TransformCoord( (D3DXVECTOR3*)&vLight, (D3DXVECTOR3*)&vLight, &matInvWorld );
    
    m_pd3dDevice->SetVertexShaderConstant(  0, &vZeroes,   1 );
    m_pd3dDevice->SetVertexShaderConstant( 40, &vLight,    1 ); // Light position
    m_pd3dDevice->SetVertexShaderConstant( 41, &vDiffuse,  1 ); // Diffuse color
    m_pd3dDevice->SetVertexShaderConstant( 42, &vAmbient,  1 ); // Ambient color

    // Set constants for doing linear fog in the vertex shader
    D3DXVECTOR4 vFog( -1.0f/(m_fFogEndValue-m_fFogStartValue), m_fFogEndValue/(m_fFogEndValue-m_fFogStartValue), 0.0f, 0.0f );
    m_pd3dDevice->SetVertexShaderConstant( 44, &vFog,  1 ); // Fog values

    // Set fog parameters
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,     FOG_COLOR );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );

    // Note: by setting D3DFOG_NONE above, we are telling the hardware we want
    // complete control over the fog value. In other words, the vertex shader
    // will set oFog.x in a range from 0.0 to 1.0, which will be the final fog
    // value as seen by the pixel shader.
    //
    // We could still set the fog table mode to linear, exp, or exp2, in which
    // case we'd need our vertex shader to set oFog.x in view space coordinates.
    // The hardware will take that value and perform the fog calculations (and
    // table lookup) on it.
    //
    // We chose the D3DFOG_NONE approach for this sample since it shows how 
    // you can have complete custom control with fog via your vertex shader.

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         FOG_COLOR, 1.0f, 0L );

    // Set state
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    // Reset the world matrix
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &m_matWorld );

    if( m_bUseVertexShader )
        SetupVertexShader();
    else
        SetupFixedPipeline();

    // Draw the terrain
    m_pd3dDevice->SetTexture( 0, m_pFloorTexture );
    m_pd3dDevice->SetStreamSource( 0, m_pTerrainVB, sizeof(FOGVERTEX) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,
                                 0, m_dwNumTerrainVertices-2 );

    // Draw the columns
    for( DWORD i=0; i<20; i++ )
    {
        FLOAT tx = (i%10)*20.0f - 100.0f;
        FLOAT ty =  0.0f;
        FLOAT tz = (i<=10) ? 40.0f : -40.0f;
        D3DXMatrixTranslation( &m_matWorld, tx, ty, tz );

        if( m_bUseVertexShader )
            SetupVertexShader();
        else
            SetupFixedPipeline();

        m_pd3dDevice->SetStreamSource( 0, m_pColumnVB, sizeof(FOGVERTEX) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,
                                     0, m_dwNumColumnVertices-2 );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 48, 0xffffffff, L"Fog" );
        m_Font.DrawText( 450, 48, 0xffffff00, m_strFrameRate );

        WCHAR strBuffer[80];

        m_Font.DrawText( 64, 75, 0xffffffff, L"Mode:" );
        if( m_bUseVertexShader )
            m_Font.DrawText( 150, 75, 0xffffff00, L"Using vertex shader" );
        else if( m_dwFogMode == D3DFOG_LINEAR )
            m_Font.DrawText( 150, 75, 0xffffff00, L"D3DFOG_LINEAR" );
        else if( m_dwFogMode == D3DFOG_EXP )
            m_Font.DrawText( 150, 75, 0xffffff00, L"D3DFOG_EXP" );
        else if( m_dwFogMode == D3DFOG_EXP2 )
            m_Font.DrawText( 150, 75, 0xffffff00, L"D3DFOG_EXP2" );

        if( TRUE == m_bUseVertexShader || m_dwFogMode == D3DFOG_LINEAR )
        {
            swprintf( strBuffer, L"%0.2f", m_fFogStartValue );
            m_Font.DrawText( 64, 100, 0xffffffff, L"Start:" );
            m_Font.DrawText( 150, 100, 0xffffff00, strBuffer  );

            swprintf( strBuffer, L"%0.2f", m_fFogEndValue );
            m_Font.DrawText( 64, 125, 0xffffffff, L"End:" );
            m_Font.DrawText( 150, 125, 0xffffff00, strBuffer  );
        }
        else
        {
            swprintf( strBuffer, L"%0.3f", m_fFogDensity );
            m_Font.DrawText( 64, 100, 0xffffffff, L"Density:" );
            m_Font.DrawText( 150, 100, 0xffffff00, strBuffer );
        }
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}


