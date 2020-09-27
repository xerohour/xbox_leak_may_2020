//-----------------------------------------------------------------------------
// File: CompressedNormals.cpp
//
// Desc: Example code showing how to use compressed normals.
//
// Hist: 05.02.00 - New for June XDK release
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
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle vertex\nshader" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// Custom vertex structures
//-----------------------------------------------------------------------------
struct MODELVERTEX
{
    D3DXVECTOR3 p;       // Referenced as v0 in the vertex shader
    D3DXVECTOR3 n;       // Referenced as v2 in the vertex shader
};

// Same as model vertex, but with compressed normals
struct COMPRESSEDVERTEX
{
    D3DXVECTOR3 p;
    DWORD       n;
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont      m_Font;                         // Font class
    CXBHelp      m_Help;                         // Help class
    BOOL         m_bDrawHelp;                    // Whether to draw help

    BOOL         m_bUseProgrammableVertexShader; // Whether to use vertex shader

    DWORD        m_dwProgrammableVertexShader;   // Programmable vertex shader
    DWORD        m_dwFixedFunctionVertexShader;  // Fixed function vertex shader

    D3DXMATRIX   m_matWorld;

    CXBMesh*                m_pObject;           // Main geometry object
    LPDIRECT3DVERTEXBUFFER8 m_pObjectVB;         // Object components for rendering
    LPDIRECT3DINDEXBUFFER8  m_pObjectIB;
    D3DPRIMITIVETYPE        m_dwObjectPrimType;
    DWORD                   m_dwNumObjectSubsets;
    XBMESH_SUBSET*          m_pObjectSubsets;

    HRESULT CreateCompressedGeometry();

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
    m_bDrawHelp                    = FALSE;
    m_bUseProgrammableVertexShader = FALSE;
    
    m_pObject                      = new CXBMesh();
    
    m_dwProgrammableVertexShader   = 0L;
    m_dwFixedFunctionVertexShader  = 0L;
}




//-----------------------------------------------------------------------------
// Name: CreateCompressedGeometry()
// Desc: Clones the main geometry object, and compressed the vertex normals.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateCompressedGeometry()
{
    // Gain acces to the mesh's vertices
    LPDIRECT3DVERTEXBUFFER8 pVB = &m_pObject->GetMesh(0)->m_VB;
    DWORD dwNumVertices = m_pObject->GetMesh(0)->m_dwNumVertices;

    // Create the destination vertex buffer
    m_pd3dDevice->CreateVertexBuffer( dwNumVertices*sizeof(COMPRESSEDVERTEX), 
                                      D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, 
                                      &m_pObjectVB );
    
    // Lock vertices for copying
    MODELVERTEX*      pSrcVertices;
    COMPRESSEDVERTEX* pDstVertices;
    pVB->Lock( 0, 0, (BYTE**)&pSrcVertices, 0 );
    m_pObjectVB->Lock( 0, 0, (BYTE**)&pDstVertices, 0 );

    // Copy vertices
    for( DWORD i=0; i<dwNumVertices; i++ )
    {
        // Copy position
        pDstVertices[i].p = pSrcVertices[i].p;

        // Compress the normal
        pDstVertices[i].n = ( ( ((DWORD)(pSrcVertices[i].n.z *  511.0f)) & 0x3ff ) << 22L ) |
                            ( ( ((DWORD)(pSrcVertices[i].n.y * 1023.0f)) & 0x7ff ) << 11L ) |
                            ( ( ((DWORD)(pSrcVertices[i].n.x * 1023.0f)) & 0x7ff ) <<  0L );
    }

    // Unlock vertex buffers
    m_pObjectVB->Unlock();
    pVB->Unlock();

    // Setup the vertex declaration
    DWORD dwDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( D3DVSDE_POSITION,    D3DVSDT_FLOAT3 ),      // v0 = Position
        D3DVSD_REG( D3DVSDE_NORMAL,      D3DVSDT_NORMPACKED3 ), // v2 = Compressed normal
        D3DVSD_END()
    };

    // Create programmable vertex shader from a file
    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Shader.xvu", 
                                           dwDecl, &m_dwProgrammableVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create a fixed-function vertex shader
    if( FAILED( m_pd3dDevice->CreateVertexShader( dwDecl, NULL, 
                                                  &m_dwFixedFunctionVertexShader, 0 ) ) )
        return XBAPPERR_MEDIANOTFOUND;

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

    // Load an object to render.
    if( FAILED( m_pObject->Create( m_pd3dDevice, "Models\\Light.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Store object components for rendering
    m_pObjectIB          = &m_pObject->GetMesh(0)->m_IB;
    m_dwObjectPrimType   =  m_pObject->GetMesh(0)->m_dwPrimType;
    m_dwNumObjectSubsets =  m_pObject->GetMesh(0)->m_dwNumSubsets;
    m_pObjectSubsets     =  m_pObject->GetMesh(0)->m_pSubsets;

    // Copy the geometry into a separate vertex buffer
    CreateCompressedGeometry();

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set the app view matrix for normal viewing
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set a default material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Create a directional light. (Use yellow light for fixed-function to
	// distinguish from vertex shader case which will be red.)
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
        m_bUseProgrammableVertexShader = !m_bUseProgrammableVertexShader; 

    // Set the world matrix
    D3DXVECTOR3 vAxis( 2+sinf(m_fAppTime*3.1f), 2+sinf(m_fAppTime*3.3f), sinf(m_fAppTime*3.5f) ); 
    D3DXMatrixRotationAxis( &m_matWorld, &vAxis, sinf(3*m_fAppTime) );

    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    // Set the vertex shader constants. 
    {
        // Some basic constants
        D3DXVECTOR4 vZero(0,0,0,0);
        D3DXVECTOR4 vOne(1,1,1,1);

        // Lighting vector (normalized) and material colors. (Use red light
        // to show difference from non-vertex shader case.)
        D3DXVECTOR4 fDiffuse( 1.00f, 0.00f, 0.00f, 1.00f );
        D3DXVECTOR4 fAmbient( 0.25f, 0.25f, 0.25f, 0.25f );
        D3DXVECTOR4 vLight( 0.5f, 1.0f, -1.0f, 0.0f );
        D3DXVec4Normalize( &vLight, &vLight );

        // Vertex shader operations use transposed matrices
        D3DXMATRIX matView, matProj, matViewProj;
        D3DXMATRIX matWorldTranspose, matViewProjTranspose;
        m_pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
        m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
        D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
        D3DXMatrixTranspose( &matWorldTranspose,    &m_matWorld );
        D3DXMatrixTranspose( &matViewProjTranspose, &matViewProj );

        // Set the vertex shader constants
        m_pd3dDevice->SetVertexShaderConstant(  0, &vZero,    1 );
        m_pd3dDevice->SetVertexShaderConstant(  1, &vOne,     1 );
        m_pd3dDevice->SetVertexShaderConstant(  4, &matWorldTranspose,    4 );
        m_pd3dDevice->SetVertexShaderConstant( 12, &matViewProjTranspose, 4 );
        m_pd3dDevice->SetVertexShaderConstant( 20, &vLight,   1 );
        m_pd3dDevice->SetVertexShaderConstant( 21, &fDiffuse, 1 );
        m_pd3dDevice->SetVertexShaderConstant( 22, &fAmbient, 1 );
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
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00404040 );

    // Set the shader (programmable or fixed-function)
    if( m_bUseProgrammableVertexShader )        
        m_pd3dDevice->SetVertexShader( m_dwProgrammableVertexShader );
    else
        m_pd3dDevice->SetVertexShader( m_dwFixedFunctionVertexShader );

    // Display the object.
    m_pd3dDevice->SetStreamSource( 0, m_pObjectVB, sizeof(COMPRESSEDVERTEX) );
    m_pd3dDevice->SetIndices( m_pObjectIB, 0 );
    for( DWORD i = 0; i < m_dwNumObjectSubsets; i++ )
    {
        m_pd3dDevice->DrawIndexedVertices( m_dwObjectPrimType, m_pObjectSubsets[i].dwIndexCount, 
                                           D3D__IndexData + m_pObjectSubsets[i].dwIndexStart );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"CompressedNormals" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( m_bUseProgrammableVertexShader )        
            m_Font.DrawText( 64, 75, 0xffffffff, L"Using programmable vertex shader" );
        else
            m_Font.DrawText( 64, 75, 0xffffffff, L"Using fixed-function pipeline" );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



