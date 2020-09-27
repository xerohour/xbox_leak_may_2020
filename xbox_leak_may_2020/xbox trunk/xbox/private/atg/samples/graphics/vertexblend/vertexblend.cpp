//-----------------------------------------------------------------------------
// File: VertexBlend.cpp
//
// Desc: Example code showing how to do a skinning effect, using the vertex
//       blending feature of Direct3D. Normally, Direct3D transforms each
//       vertex through the world matrix. The vertex blending feature,
//       however, uses mulitple world matrices and a per-vertex blend factor
//       to transform each vertex.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
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
// Name: struct BLENDVERTEX
// Desc: Custom vertex which includes a blending factor
//-----------------------------------------------------------------------------
struct BLENDVERTEX
{
    D3DXVECTOR3 v;       // Referenced as v0 in the vertex shader
    FLOAT       blend;   // Referenced as v1.x in the vertex shader
    D3DXVECTOR3 n;       // Referenced as v3 in the vertex shader
    FLOAT       tu, tv;  // Referenced as v7 in the vertex shader
};

#define D3DFVF_BLENDVERTEX (D3DFVF_XYZB1|D3DFVF_NORMAL|D3DFVF_TEX1)




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

    CXBMesh*     m_pObject;           // Object to use for vertex blending

    D3DXMATRIX   m_matUpperArm;       // Vertex blending matrices
    D3DXMATRIX   m_matLowerArm;

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
    m_bDrawHelp         = FALSE;
    m_pObject           = new CXBMesh();
    m_dwVertexShader    = 0L;
    m_bUseVertexShader  = FALSE;
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
    if( FAILED( m_pObject->Create( m_pd3dDevice, "Models\\MSLogo.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Make sure the loaded object has the correct FVF. It needs to be pre-
    // authored to match (i.e. with space for blend weights) or this app will
    // not work. See the MakeXBG tool for converting meshes.
    if( m_pObject->GetMesh(0)->m_dwFVF != D3DFVF_BLENDVERTEX )
	{
		OUTPUT_DEBUG_STRING( "Error: Mesh must have blend weights!\n" );
        return E_FAIL;
	}

    // Add blending weights to the mesh
    {
        // Gain acces to the mesh's vertices
        LPDIRECT3DVERTEXBUFFER8 pVB = &m_pObject->GetMesh(0)->m_VB;
        DWORD        dwNumVertices = m_pObject->GetMesh(0)->m_dwNumVertices;
        BLENDVERTEX* pVertices;
        pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

        // Calculate the min/max z values for all the vertices
        FLOAT fMinX =  1e10f;
        FLOAT fMaxX = -1e10f;

        for( DWORD i=0; i<dwNumVertices; i++ )
        {
            if( pVertices[i].v.x < fMinX ) 
                fMinX = pVertices[i].v.x;
            if( pVertices[i].v.x > fMaxX ) 
                fMaxX = pVertices[i].v.x;
        }

        for( i=0; i<dwNumVertices; i++ )
        {
            // Set the blend factors for the vertices
            FLOAT a = ( pVertices[i].v.x - fMinX ) / ( fMaxX - fMinX );
            pVertices[i].blend = 1.0f-sinf(a*D3DX_PI*1.0f);
        }

        // Done with the mesh's vertex buffer data
        pVB->Unlock();
    }

    // Setup the vertex declaration
    DWORD dwDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),     // v0 = Position
        D3DVSD_REG( 1, D3DVSDT_FLOAT1 ),     // v1 = Blend weight
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),     // v3 = Normal
        D3DVSD_REG( 7, D3DVSDT_FLOAT2 ),     // v7 = Texcoords
        D3DVSD_END()
    };

    // Create vertex shader from a file
    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Blend.xvu", 
                                           dwDecl, &m_dwVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 10000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set the app view matrix for normal viewing
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f,-5.0f,-10.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

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

    // Set the vertex blending matrices for this frame
    D3DXVECTOR3 vAxis( 2+sinf(m_fAppTime*3.1f), 2+sinf(m_fAppTime*3.3f), sinf(m_fAppTime*3.5f) ); 
    D3DXMatrixRotationAxis( &m_matLowerArm, &vAxis, sinf(3*m_fAppTime) );
    D3DXMatrixIdentity( &m_matUpperArm );

    // Set the vertex shader constants. 
    {
        // Some basic constants
        D3DXVECTOR4 vZero(0,0,0,0);
        D3DXVECTOR4 vOne(1,1,1,1);

        // Lighting vector (normalized) and material colors. (Use red light
        // to show difference from non-vertex shader case.)
        D3DXVECTOR4 vLight( 0.5f, 1.0f, -1.0f, 0.0f );
        D3DXVec4Normalize( &vLight, &vLight );
        D3DXVECTOR4 vDiffuse( 1.00f, 0.00f, 0.00f, 1.00f );
        D3DXVECTOR4 vAmbient( 0.25f, 0.25f, 0.25f, 0.25f );

        // Vertex shader operations use transposed matrices
        D3DXMATRIX matWorld0Transpose, matWorld1Transpose;
        D3DXMATRIX matView, matProj, matViewProj, matViewProjTranspose;
        m_pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
        m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
        D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
        D3DXMatrixTranspose( &matWorld0Transpose, &m_matUpperArm );
        D3DXMatrixTranspose( &matWorld1Transpose, &m_matLowerArm );
        D3DXMatrixTranspose( &matViewProjTranspose, &matViewProj );

        // Set the vertex shader constants
        m_pd3dDevice->SetVertexShaderConstant(  0, &vZero,    1 );
        m_pd3dDevice->SetVertexShaderConstant(  1, &vOne,     1 );
        m_pd3dDevice->SetVertexShaderConstant(  4, &matWorld0Transpose,   4 );
        m_pd3dDevice->SetVertexShaderConstant(  8, &matWorld1Transpose,   4 );
        m_pd3dDevice->SetVertexShaderConstant( 12, &matViewProjTranspose, 4 );
        m_pd3dDevice->SetVertexShaderConstant( 20, &vLight,   1 );
        m_pd3dDevice->SetVertexShaderConstant( 21, &vDiffuse, 1 );
        m_pd3dDevice->SetVertexShaderConstant( 22, &vAmbient, 1 );
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
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00404040 );

    // Render the bendy object
    if( m_bUseVertexShader )        
    {
        // Enable vertex blending using the vertex shader
        m_pd3dDevice->SetVertexShader( m_dwVertexShader );

        // Display the object. Don't use the mesh's FVF code (will use vertex shader instead)
        m_pObject->Render( m_pd3dDevice, XBMESH_NOFVF );
    }
    else
    {
        // Enable vertex blending using API
        m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m_matUpperArm );
        m_pd3dDevice->SetTransform( D3DTS_WORLD1, &m_matLowerArm );
        m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_1WEIGHTS );

        // Display the object. Use the mesh's FVF code (for fixed-lighting)
        m_pObject->Render( m_pd3dDevice );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"VertexBlend" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( m_bUseVertexShader )        
            m_Font.DrawText( 64, 75, 0xffffffff, L"Using vertex shader" );
        else
            m_Font.DrawText( 64, 75, 0xffffffff, L"Using blending API" );

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



