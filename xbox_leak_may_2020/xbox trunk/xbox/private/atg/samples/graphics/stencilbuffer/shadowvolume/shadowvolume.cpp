//-----------------------------------------------------------------------------
// File: ShadowVolume.cpp
//
// Desc: Sample code showing how to use stencil buffers to implement shadow
//       volumes.
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
    { "BiHull.bmp",   resource_BiHull_OFFSET },
    { "Wings.bmp",    resource_Wings_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move airplane" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Move light" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_1, L"Show shadow volume" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Show silhouette" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS 5




//-----------------------------------------------------------------------------
// External definitions and prototypes
//-----------------------------------------------------------------------------
#define FOG_COLOR 0xff0000ff

struct D3DVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};

struct SHADOWVERTEX
{
    D3DXVECTOR4 p;
    D3DCOLOR    color;
};

#define D3DFVF_VERTEX       (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
#define D3DFVF_SHADOWVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)




//-----------------------------------------------------------------------------
// Name: class ShadowVolume
// Desc: A shadow volume object
//-----------------------------------------------------------------------------
class ShadowVolume
{
    LPDIRECT3DVERTEXBUFFER8 m_pShadowVolumeVB;
    DWORD             m_dwNumVertices;
    DWORD             m_dwMaxVertices;

    LPDIRECT3DINDEXBUFFER8 m_pShadowVolumeIB;
    DWORD             m_dwNumEdges;
    DWORD             m_dwMaxEdges;

public:
    // Constructor
    ShadowVolume()
    { 
        m_pShadowVolumeVB = NULL; m_dwMaxVertices = 0L; m_dwNumVertices = 0L; 
        m_pShadowVolumeIB = NULL; m_dwMaxEdges    = 0L; m_dwMaxEdges    = 0L; 
    }

    // Resets the shadow volume contents
    VOID    Reset() { m_dwNumVertices = 0L; }
    
    // Builds a shadow volume from a mesh and a light source
    HRESULT Build( LPDIRECT3DDEVICE8 pd3dDevice, 
                   LPDIRECT3DVERTEXBUFFER8 pVB, DWORD dwNumVertices, 
                   LPDIRECT3DINDEXBUFFER8 pIB,  DWORD dwNumFaces, 
                   D3DXVECTOR3 vLightPos );
    
    // Renders the shadow volume
    HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice );

    // Renders the shadow volume's ede list
    HRESULT RenderEdges( LPDIRECT3DDEVICE8 pd3dDevice );
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;        // Packed resources for the app
    CXBFont            m_Font;               // Font class
    CXBHelp            m_Help;               // Help class
    BOOL               m_bDrawHelp;          // Whether to draw help

    CXBMesh*      m_pAirplaneObject;
    CXBMesh*      m_pTerrainObject;
    ShadowVolume  m_ShadowVolume;

    BOOL          m_bDrawShadowVolume;
    BOOL          m_bDrawSilhouette;

    D3DXMATRIX    m_matObjectMatrix;
    D3DXMATRIX    m_matTerrainMatrix;

    HRESULT DrawShadow();
    HRESULT RenderShadow();

protected:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

public:
    CXBoxSample();
};




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT ShadowVolume::RenderEdges( LPDIRECT3DDEVICE8 pd3dDevice )
{
    pd3dDevice->SetVertexShader( D3DFVF_XYZ );
    pd3dDevice->SetStreamSource( 0, m_pShadowVolumeVB, sizeof(D3DXVECTOR3) );
    pd3dDevice->SetIndices( m_pShadowVolumeIB, 0 );
    pd3dDevice->DrawIndexedPrimitive( D3DPT_LINELIST, 0, 2*m_dwNumEdges,
                                      0, m_dwNumEdges );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT ShadowVolume::Render( LPDIRECT3DDEVICE8 pd3dDevice )
{
    pd3dDevice->SetVertexShader( D3DFVF_XYZ );
    pd3dDevice->SetStreamSource( 0, m_pShadowVolumeVB, sizeof(D3DXVECTOR3) );
    pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, m_dwNumVertices/4 );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: AddEdge()
// Desc: Adds an edge to a list of silohuette edges of a shadow volume.
//-----------------------------------------------------------------------------
DWORD AddEdge( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 )
{
    // Remove interior edges (which appear in the list twice)
    for( DWORD i=0; i < dwNumEdges; i++ )
    {
        // Check if this edge already appears in the list. Note: it will be in
        // opposite order, due to the winding of the polygons
        if( ( pEdges[2*i+0] == v1 && pEdges[2*i+1] == v0 ) )
        {
            // If it does, remove it (fast way is to replace it with the last
            // edge in the list, and decrement the # of edges)
            if( dwNumEdges > 1 )
            {
                pEdges[2*i+0] = pEdges[2*(dwNumEdges-1)+0];
                pEdges[2*i+1] = pEdges[2*(dwNumEdges-1)+1];
            }
            return dwNumEdges--;
        }
    }

    // Add the edge to the list
    pEdges[2*dwNumEdges+0] = v0;
    pEdges[2*dwNumEdges+1] = v1;
    return dwNumEdges++;
}




//-----------------------------------------------------------------------------
// Name: BuildFromMesh()
// Desc: Takes a mesh as input, and uses it to build a shadowvolume. The
//       technique used considers each triangle of the mesh, and adds it's
//       edges to a temporary list. The edge list is maintained, such that
//       only silohuette edges are kept. Finally, the silohuette edges are
//       extruded to make the shadow volume vertex list.
//-----------------------------------------------------------------------------
HRESULT ShadowVolume::Build( LPDIRECT3DDEVICE8 pd3dDevice, 
                             LPDIRECT3DVERTEXBUFFER8 pMeshVB, DWORD dwNumVertices, 
                             LPDIRECT3DINDEXBUFFER8 pMeshIB, DWORD dwNumFaces, 
                             D3DXVECTOR3 vLightPos )
{
    // Note: the MESHVERTEX format depends on the FVF of the mesh
    struct MESHVERTEX { D3DXVECTOR3 p, n; FLOAT tu, tv; };
    MESHVERTEX*  pMeshVertices;
    WORD*        pMeshIndices;

    // Allocate an edge list
    if( dwNumFaces*3 > m_dwMaxEdges )
    {
        m_dwMaxEdges = dwNumFaces*3;

        SAFE_RELEASE( m_pShadowVolumeIB );
        if( FAILED( pd3dDevice->CreateIndexBuffer( m_dwMaxEdges*2*sizeof(WORD),
                                                   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
                                                   D3DPOOL_DEFAULT, &m_pShadowVolumeIB ) ) )
            return E_FAIL;
    }
    m_dwNumEdges = 0;

    // Lock the geometry buffers
    pMeshVB->Lock( 0L, 0L, (BYTE**)&pMeshVertices, 0L );
    pMeshIB->Lock( 0L, 0L, (BYTE**)&pMeshIndices, 0L );

    // Lock the shadowvolume index buffer
    WORD* pShadowEdges;
    m_pShadowVolumeIB->Lock( 0L, 0L, (BYTE**)&pShadowEdges, 0L );

    // Loop through polygons and add edges for ones that face the light
    for( DWORD i=0; i<dwNumFaces; i++ )
    {
        // Get the vertices for this polygon
        WORD wIndex0 = pMeshIndices[3*i+0];
        WORD wIndex1 = pMeshIndices[3*i+1];
        WORD wIndex2 = pMeshIndices[3*i+2];
        D3DXVECTOR3 v0 = pMeshVertices[wIndex0].p;
        D3DXVECTOR3 v1 = pMeshVertices[wIndex1].p;
        D3DXVECTOR3 v2 = pMeshVertices[wIndex2].p;

        // Get the normal and the light vectors
        D3DXVECTOR3 vNormal;
        D3DXVec3Cross( &vNormal, &(v2-v1), &(v1-v0) );
        D3DXVECTOR3 vLight = vLightPos - v0;

        // Add edges for polys that face the light
        if( D3DXVec3Dot( &vNormal, &vLight ) >= 0.0f )
        {
            AddEdge( pShadowEdges, m_dwNumEdges, wIndex0, wIndex1 );
            AddEdge( pShadowEdges, m_dwNumEdges, wIndex1, wIndex2 );
            AddEdge( pShadowEdges, m_dwNumEdges, wIndex2, wIndex0 );
        }
    }

    // Allocate a vertex buffer
    if( m_dwNumEdges*4 > m_dwMaxVertices )
    {
        m_dwMaxVertices = m_dwNumEdges*4;

        SAFE_RELEASE( m_pShadowVolumeVB );
        if( FAILED( pd3dDevice->CreateVertexBuffer( m_dwMaxVertices*sizeof(D3DXVECTOR3),
                                                    D3DUSAGE_WRITEONLY, 0,
                                                    D3DPOOL_DEFAULT, &m_pShadowVolumeVB ) ) )
            return E_FAIL;
    }

    // Lock the shadowvolume vertex buffer
    D3DXVECTOR3* pShadowVertices;
    m_pShadowVolumeVB->Lock( 0L, 0L, (BYTE**)&pShadowVertices, 0L );

    // Fill the shadowvolume vertex buffer
    for( i=0; i<m_dwNumEdges; i++ )
    {
        D3DXVECTOR3* vPos0 = &pMeshVertices[pShadowEdges[2*i+0]].p;
        D3DXVECTOR3* vPos1 = &pMeshVertices[pShadowEdges[2*i+1]].p;

        // Re-assign the edge (for subsequent drawing of the edge list)
        pShadowEdges[2*i+0] = (WORD)(m_dwNumVertices+0);
        pShadowEdges[2*i+1] = (WORD)(m_dwNumVertices+1);

        // Add a quad (two triangles) to the vertex list
        pShadowVertices[m_dwNumVertices++] = (*vPos0);
        pShadowVertices[m_dwNumVertices++] = (*vPos1);
        pShadowVertices[m_dwNumVertices++] = (*vPos1) - vLightPos*10;
        pShadowVertices[m_dwNumVertices++] = (*vPos0) - vLightPos*10;
    }

    // Unlock the shadowvolume vertex buffer
    m_pShadowVolumeVB->Unlock();

    // Unlock the shadowvolume index buffer
    m_pShadowVolumeIB->Unlock();

    // Unlock the geometry buffers
    pMeshVB->Unlock();
    pMeshIB->Unlock();

    return S_OK;
}




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
    // Override base class members
    m_d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    m_pAirplaneObject   = new CXBMesh();
    m_pTerrainObject    = new CXBMesh();
    m_bDrawShadowVolume = FALSE;
    m_bDrawSilhouette   = FALSE;
    m_bDrawHelp         = FALSE;

    D3DXMatrixIdentity( &m_matTerrainMatrix );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
inline FLOAT HeightField( FLOAT x, FLOAT z )
{
    FLOAT y = 0.0f;
    y += 7.0f * cosf( 0.051f*x + 0.0f ) * sinf( 0.055f*x + 0.0f );
    y += 7.0f * cosf( 0.053f*z + 0.0f ) * sinf( 0.057f*z + 0.0f );
    y += 1.0f * cosf( 0.101f*x + 0.0f ) * sinf( 0.105f*x + 0.0f );
    y += 1.0f * cosf( 0.103f*z + 0.0f ) * sinf( 0.107f*z + 0.0f );
    y += 1.0f * cosf( 0.251f*x + 0.0f ) * sinf( 0.255f*x + 0.0f );
    y += 1.0f * cosf( 0.253f*z + 0.0f ) * sinf( 0.257f*z + 0.0f );
    return y;
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

    // Load an object to cast the shadow
    if( FAILED( m_pAirplaneObject->Create( m_pd3dDevice, "Models\\Airplane.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load some terrain
    if( FAILED( m_pTerrainObject->Create( m_pd3dDevice, "Models\\SeaFloor.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Tweak the terrain vertices
    {
        LPDIRECT3DVERTEXBUFFER8 pVB = &m_pTerrainObject->GetMesh(0)->m_VB;
        DWORD   dwNumVertices = m_pTerrainObject->GetMesh(0)->m_dwNumVertices;

        // Lock the vertex buffer to access the terrain geometry
        D3DVERTEX* pVertices;
        pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

        // Add some more bumpiness to the terrain object
        for( DWORD i=0; i<dwNumVertices; i++ )
        {
            pVertices[i].p.x = 0.4f * pVertices[i].p.x +  0.0f;
            pVertices[i].p.z = 0.4f * pVertices[i].p.z + 20.0f;
            pVertices[i].p.y = HeightField( 25+2*pVertices[i].p.x, -25+2*pVertices[i].p.z ) - 8.0f;
        }

        // Release the vertex buffer
        pVB->Unlock();
    }

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

    // Set the transform matrices
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 10.0f, -20.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f,  0.0f,   0.0f  );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f,  1.0f,   0.0f  );
    D3DXMATRIX matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 100.0f );

    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Turn on fog
    FLOAT fFogStart =  30.0f;
    FLOAT fFogEnd   =  80.0f;
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,      TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,       FOG_COLOR );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGSTART,       FtoDW(fFogStart) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGEND,         FtoDW(fFogEnd) );

    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00303030 );

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

    // Whether or not to draw the shadowvolume
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        m_bDrawShadowVolume = !m_bDrawShadowVolume;

    // Whether or not to draw the silhouette used to build the shadowvolume
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        m_bDrawSilhouette = !m_bDrawSilhouette;

    // Setup viewing postion from Gamepad
    static FLOAT fRotateX1 = 0.0f;
    static FLOAT fRotateY1 = 0.0f;
    fRotateX1 += m_DefaultGamepad.fX1*m_fElapsedTime*D3DX_PI*0.5f;
    fRotateY1 += m_DefaultGamepad.fY1*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMatrixRotationYawPitchRoll( &m_matObjectMatrix, -fRotateX1, -fRotateY1, 0.0f );

    // Setup light postion from Gamepad
    static FLOAT Lx = 0.0f;
    static FLOAT Lz = 0.0f;
    Lx = ( Lx + m_DefaultGamepad.fX2*m_fElapsedTime*10.0f ) * 0.99f;
    Lz = ( Lz + m_DefaultGamepad.fY2*m_fElapsedTime*10.0f ) * 0.99f;
    D3DXVECTOR3 vLight( Lx+0.0f, 5.0f, Lz+0.0f );

    // Move the light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_POINT, vLight.x, vLight.y, vLight.z );
    light.Attenuation0 = 0.9f;
    m_pd3dDevice->SetLight( 0, &light );

    // Transform the light vector to be in object space
    D3DXMATRIX m;
    D3DXMatrixInverse( &m, NULL, &m_matObjectMatrix );
    D3DXVec3TransformCoord( &vLight, &vLight, &m );

    // Build the shadow volume
    m_ShadowVolume.Reset();
    m_ShadowVolume.Build( m_pd3dDevice, 
                         &m_pAirplaneObject->GetMesh(0)->m_VB, 
                          m_pAirplaneObject->GetMesh(0)->m_dwNumVertices, 
                         &m_pAirplaneObject->GetMesh(0)->m_IB, 
                          m_pAirplaneObject->GetMesh(0)->m_dwNumIndices/3, 
                          vLight );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderShadow()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderShadow()
{
    // Disable z-buffer writes (note: z-testing still occurs), and enable the
    // stencil-buffer
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );

    // Dont bother with interpolating color
    m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_FLAT );

    // Set up stencil compare fuction, reference value, and masks.
    // Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true.
    // Note: since we set up the stencil-test to always pass, the STENCILFAIL
    // renderstate is really not needed.
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,   D3DCMP_ALWAYS );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,  D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFAIL,   D3DSTENCILOP_KEEP );

    // If ztest passes, inc/decrement stencil buffer value
    m_pd3dDevice->SetRenderState( D3DRS_STENCILREF,       0x1 );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS,      D3DSTENCILOP_INCR );

    // Make sure that no pixels get drawn to the frame buffer
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    // Draw front-side of shadow volume in stencil/z only
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObjectMatrix );
    m_ShadowVolume.Render( m_pd3dDevice );

    // Unless we want to visualize the entire shadow volume (for debugging
    // purposes), then we need to do the following to remove the backfacing
    // sides of the shadow volume.
    if( FALSE == m_bDrawShadowVolume )
    {
        // Now reverse cull order so back sides of shadow volume are written.
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );

        // Decrement stencil buffer value
        m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );

        // Draw back-side of shadow volume in stencil/z only
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObjectMatrix );
        m_ShadowVolume.Render( m_pd3dDevice );
    }

    // Restore render states
    m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawShadow()
// Desc: Draws a big gray polygon over scene according to the mask in the
//       stencil buffer. (Any pixel with stencil==1 is in the shadow.)
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawShadow()
{
    // Set renderstates (disable z-buffering, enable stencil, disable fog, and
    // turn on alphablending)
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

    // The stencilbuffer values indicates # of shadows that overlap each pixel.
    // We only want to draw pixels that are in shadow, which was set up in
    // RenderShadow() such that StencilBufferValue >= 1. In the Direct3D API, 
    // the stencil test is pseudo coded as:
    //    StencilRef CompFunc StencilBufferValue
    // so we set our renderstates with StencilRef = 1 and CompFunc = LESSEQUAL.
    m_pd3dDevice->SetRenderState( D3DRS_STENCILREF,  0x1 );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );

    // Draw a big, gray square
    SHADOWVERTEX v[4];
    v[0].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0.0f, 1.0f );  v[0].color = 0x7f000000;
    v[1].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0.0f, 1.0f );  v[1].color = 0x7f000000;
    v[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0.0f, 1.0f );  v[2].color = 0x7f000000;
    v[3].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0.0f, 1.0f );  v[3].color = 0x7f000000;
    m_pd3dDevice->SetVertexShader( D3DFVF_SHADOWVERTEX );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );

    // Restore render states
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

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
    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         FOG_COLOR, 1.0f, 0L );

    // Set state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00303030 );

    // Turn on fog
    FLOAT fFogStart =  30.0f;
    FLOAT fFogEnd   =  80.0f;
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,      TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,       FOG_COLOR );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGSTART,       FtoDW(fFogStart) );
    m_pd3dDevice->SetRenderState( D3DRS_FOGEND,         FtoDW(fFogEnd) );

    // Draw the terrain
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matTerrainMatrix );
    m_pTerrainObject->Render( m_pd3dDevice );

    // Draw the airplane
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObjectMatrix );
    m_pAirplaneObject->Render( m_pd3dDevice );

    // Turn off fog
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
    
    // Render the shadow volume into the stencil buffer, then draw the shadow
    // in the scene
    RenderShadow();
    DrawShadow();

    // Draw the silhouette used to build the shadow volume 
    if( m_bDrawSilhouette )
    {
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffffffff );
        m_ShadowVolume.RenderEdges( m_pd3dDevice );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"ShadowVolume" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



