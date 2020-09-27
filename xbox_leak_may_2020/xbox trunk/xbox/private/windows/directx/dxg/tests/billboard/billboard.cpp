//-----------------------------------------------------------------------------
// File: Billboard.cpp
//
// Desc: Example code showing how to do billboarding. The sample uses
//       billboarding to draw some trees.
//
//       Note: this implementation is for billboards that are fixed to rotate
//       about the Y-axis, which is good for things like trees. For
//       unconstrained billboards, like explosions in a flight sim, the
//       technique is the same, but the the billboards are positioned slightly
//       different. Try using the inverse of the view matrix, TL-vertices, or
//       some other technique.
//
// Copyright (c) 1995-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT

#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#include <D3DX8.h>
#endif
#include <stdio.h>
#include <math.h>
#include <tchar.h>
#include "..\common\D3DApp.h"
#include "..\common\D3DFile.h"
#include "..\common\D3DUtil.h"
#include "..\common\DXUtil.h"




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_TREES 500

// Need global access of the eye point used by the callback to sort trees
D3DXVECTOR3 g_vEyePt;

// Simple function to define "hilliness" for terrain
inline FLOAT HeightField( FLOAT x, FLOAT y )
{
    return 9*(cosf(x/20+0.2f)*cosf(y/15-0.2f)+1.0f);
}

// Custom vertex type for the trees
struct TREEVERTEX
{
    D3DXVECTOR3 p;
    DWORD       color;
    FLOAT       tu, tv;
};

#define D3DFVF_TREEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

// Tree textures to use
CHAR* g_strTreeTextures[] =
{
    "Tree02S.tga",
    "Tree35S.tga",
    "Tree01S.tga",
};

#define NUMTREETEXTURES 3




//-----------------------------------------------------------------------------
// Name: Tree
// Desc: Simple structure to hold data for rendering a tree
//-----------------------------------------------------------------------------
struct Tree
{
    TREEVERTEX  v[4];
    D3DXVECTOR3 vPos;
    DWORD       dwTreeTexture;
};




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    CD3DMesh*     m_pTerrain;           // Terrain object
    CD3DMesh*     m_pSkyBox;            // Skybox background object

    LPDIRECT3DVERTEXBUFFER8 m_pTreeVB;  // Vertex buffer for rendering a tree
    LPDIRECT3DTEXTURE8      m_pTreeTextures[NUMTREETEXTURES]; // Tree images
    D3DXMATRIX    m_matBillboardMatrix; // Used for billboard orientation
    Tree          m_Trees[NUM_TREES];   // Array of tree info

    void DrawBackground();
    void DrawTrees();

protected:
    void OneTimeSceneInit();
    void InitDeviceObjects();
    void RestoreDeviceObjects();
    void InvalidateDeviceObjects();
    void DeleteDeviceObjects();
    void Render();
    void FrameMove();
    void FinalCleanup();

public:
    CMyD3DApplication();
};



#ifdef XBOX

void __cdecl main()
{
    CMyD3DApplication d3dApp;

    d3dApp.Create( NULL );

    d3dApp.Run();
}

#else 

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    d3dApp.Create( hInst );

    d3dApp.Run();

    return 0;
}

#endif



//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_bUseDepthBuffer   = TRUE;

    m_pSkyBox      = new CD3DMesh();
    m_pTerrain     = new CD3DMesh();
    m_pTreeVB      = NULL;

    for( DWORD i=0; i<NUMTREETEXTURES; i++ )
        m_pTreeTextures[i] = NULL;
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
void CMyD3DApplication::OneTimeSceneInit()
{
    // Initialize the tree data
    for( WORD i=0; i<NUM_TREES; i++ )
    {
        // Position the trees randomly
        FLOAT fTheta  = 2.0f*D3DX_PI*(FLOAT)rand()/RAND_MAX;
        FLOAT fRadius = 25.0f + 55.0f * (FLOAT)rand()/RAND_MAX;
        m_Trees[i].vPos.x  = fRadius * sinf(fTheta);
        m_Trees[i].vPos.z  = fRadius * cosf(fTheta);
        m_Trees[i].vPos.y  = HeightField( m_Trees[i].vPos.x, m_Trees[i].vPos.z );

        // Size the trees randomly
        FLOAT fWidth  = 1.0f + 0.2f * (FLOAT)(rand()-rand())/RAND_MAX;
        FLOAT fHeight = 1.4f + 0.4f * (FLOAT)(rand()-rand())/RAND_MAX;

        // Each tree is a random color between red and green
        DWORD r = (255-190) + (DWORD)(190*(FLOAT)(rand())/RAND_MAX);
        DWORD g = (255-190) + (DWORD)(190*(FLOAT)(rand())/RAND_MAX);
        DWORD b = 0;
        DWORD dwColor = 0xff000000 + (r<<16) + (g<<8) + (b<<0);

        m_Trees[i].v[0].p     = D3DXVECTOR3(-fWidth, 0*fHeight, 0.0f );
        m_Trees[i].v[0].color = dwColor;
        m_Trees[i].v[0].tu    = 0.0f;   m_Trees[i].v[0].tv = 1.0f;
        m_Trees[i].v[1].p     = D3DXVECTOR3(-fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[1].color = dwColor;
        m_Trees[i].v[1].tu    = 0.0f;   m_Trees[i].v[1].tv = 0.0f;
        m_Trees[i].v[2].p     = D3DXVECTOR3( fWidth, 0*fHeight, 0.0f  );
        m_Trees[i].v[2].color = dwColor;
        m_Trees[i].v[2].tu    = 1.0f;   m_Trees[i].v[2].tv = 1.0f;
        m_Trees[i].v[3].p     = D3DXVECTOR3( fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[3].color = dwColor;
        m_Trees[i].v[3].tu    = 1.0f;   m_Trees[i].v[3].tv = 0.0f;

        // Size the trees randomly
        m_Trees[i].dwTreeTexture = (DWORD)( ( NUMTREETEXTURES * rand() ) / (FLOAT)RAND_MAX );
    }
}




//-----------------------------------------------------------------------------
// Name: TreeSortCB()
// Desc: Callback function for sorting trees in back-to-front order
//-----------------------------------------------------------------------------
int __cdecl TreeSortCB( const VOID* arg1, const VOID* arg2 )
{
    Tree* p1 = (Tree*)arg1;
    Tree* p2 = (Tree*)arg2;

    FLOAT dx1 = p1->vPos.x - g_vEyePt.x;
    FLOAT dz1 = p1->vPos.z - g_vEyePt.z;
    FLOAT dx2 = p2->vPos.x - g_vEyePt.x;
    FLOAT dz2 = p2->vPos.z - g_vEyePt.z;

    if( (dx1*dx1+dz1*dz1) < (dx2*dx2+dz2*dz2) )
        return +1;

    return -1;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FrameMove()
{
    // Get the eye and lookat points from the camera's path
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookatPt;

    vEyePt.x = 30.0f*cosf( 0.8f * ( m_fTime + 1.0f ) );
    vEyePt.z = 30.0f*sinf( 0.8f * ( m_fTime + 1.0f ) );
    vEyePt.y = 4 + HeightField( vEyePt.x, vEyePt.z );

    vLookatPt.x = 30.0f*cosf( 0.8f * ( m_fTime + 1.5f ) );
    vLookatPt.z = 30.0f*sinf( 0.8f * ( m_fTime + 1.5f ) );
    vLookatPt.y = vEyePt.y - 1.0f;

    // Set the app view matrix for normal viewing
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    CheckHR(m_pDevice->SetTransform( D3DTS_VIEW, &matView ));

    // Set up a rotation matrix to orient the billboard towards the camera.
    D3DXVECTOR3 vDir = vLookatPt - vEyePt;
    if( vDir.x > 0.0f )
        D3DXMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
    else
        D3DXMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );

    // Sort trees in back-to-front order
    g_vEyePt = vEyePt;
    qsort( m_Trees, NUM_TREES, sizeof(Tree), TreeSortCB );
}




//-----------------------------------------------------------------------------
// Name: DrawTrees()
// Desc:
//-----------------------------------------------------------------------------
void CMyD3DApplication::DrawTrees()
{
    // Set diffuse blending for alpha set in vertices.
    CheckHR(m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE ))
    CheckHR(m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA ))
    CheckHR(m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ))

    // Enable alpha testing (skips pixels with less than a certain alpha.)
    if( m_Caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
    {
        CheckHR(m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE ))
        CheckHR(m_pDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 ))
        CheckHR(m_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL ))
    }

    // Loop through and render all trees
    for( DWORD i=0; i<NUM_TREES; i++ )
    {
        // Set the tree texture
        CheckHR(m_pDevice->SetTexture( 0, m_pTreeTextures[m_Trees[i].dwTreeTexture] ))

        // Translate the billboard into place
        m_matBillboardMatrix._41 = m_Trees[i].vPos.x;
        m_matBillboardMatrix._42 = m_Trees[i].vPos.y;
        m_matBillboardMatrix._43 = m_Trees[i].vPos.z;
        CheckHR(m_pDevice->SetTransform( D3DTS_WORLD, &m_matBillboardMatrix ))

        // Copy tree mesh into vertexbuffer
        TREEVERTEX* v;
        CheckHR(m_pTreeVB->Lock( 0, 0, (BYTE**)&v, 0 ));
        memcpy( v, m_Trees[i].v, 4*sizeof(TREEVERTEX) );
        CheckHR(m_pTreeVB->Unlock())

        // Render the billboard
        CheckHR(m_pDevice->SetStreamSource( 0, m_pTreeVB, sizeof(TREEVERTEX) ));
        CheckHR(m_pDevice->SetVertexShader( D3DFVF_TREEVERTEX ));
        CheckHR(m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 ));
    }

    // Restore state
    D3DXMATRIX  matWorld;
    D3DXMatrixIdentity( &matWorld );
    CheckHR(m_pDevice->SetTransform( D3DTS_WORLD, &matWorld ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,    FALSE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE ));
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::Render()
{
    // Clear the viewport
    CheckHR(m_pDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L ));

    // Begin the scene
    CheckHR(m_pDevice->BeginScene() );

    // Render the Skybox
    {
        // Center view matrix for skybox and disable zbuffer
        D3DXMATRIX matView, matViewSave;
        CheckHR(m_pDevice->GetTransform( D3DTS_VIEW,      &matViewSave ));
        matView = matViewSave;
        matView._41 = 0.0f; matView._42 = -0.3f; matView._43 = 0.0f;
        CheckHR(m_pDevice->SetTransform( D3DTS_VIEW,      &matView ));
        CheckHR(m_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE ));

        // Render the skybox
        m_pSkyBox->Render( m_pDevice );

        // Restore the render states
        CheckHR(m_pDevice->SetTransform( D3DTS_VIEW,      &matViewSave ));
        CheckHR(m_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE ));
    }

    // Draw the terrain
    m_pTerrain->Render( m_pDevice );

    // Draw the trees
    DrawTrees();

    // End the scene.
    CheckHR(m_pDevice->EndScene());
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: This creates all device-dependant managed objects, such as managed
//       textures and managed vertex buffers.
//-----------------------------------------------------------------------------
void CMyD3DApplication::InitDeviceObjects()
{
    // Create the tree textures
    for( DWORD i=0; i<NUMTREETEXTURES; i++ )
    {
        CheckHR(D3DUtil_CreateTexture( m_pDevice, g_strTreeTextures[i], &m_pTreeTextures[i])) ;
    }

    // Create a quad for rendering each tree
    CheckHR(m_pDevice->CreateVertexBuffer( 4*sizeof(TREEVERTEX),
                                           D3DUSAGE_WRITEONLY, D3DFVF_TREEVERTEX,
                                           D3DPOOL_MANAGED, &m_pTreeVB ) );

    // Load the skybox
    m_pSkyBox->Create( m_pDevice, "SkyBox2.x");

    // Load the terrain
    m_pTerrain->Create( m_pDevice, "SeaFloor.x");

    // Add some "hilliness" to the terrain
    LPDIRECT3DVERTEXBUFFER8 pVB;
    CheckHR(m_pTerrain->GetSysMemMesh()->GetVertexBuffer( &pVB ) );

    {
        struct VERTEX { FLOAT x,y,z,tu,tv; };
        VERTEX* pVertices;
        DWORD   dwNumVertices = m_pTerrain->GetSysMemMesh()->GetNumVertices();

        CheckHR(pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ));

        for( DWORD i=0; i<dwNumVertices; i++ )
            pVertices[i].y = HeightField( pVertices[i].x, pVertices[i].z );

        CheckHR(pVB->Unlock());
        pVB->Release();
    }
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RestoreDeviceObjects()
{
    // Restore the device objects for the meshes and fonts
    m_pTerrain->RestoreDeviceObjects( m_pDevice );
    m_pSkyBox->RestoreDeviceObjects( m_pDevice );

    // Set the transform matrices (view and world are updated per frame)
    D3DXMATRIX matProj;
    FLOAT fAspect = m_BackBufferDesc.Width / (FLOAT)m_BackBufferDesc.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    CheckHR(m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj ));

    // Set up the default texture states
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR ));
    CheckHR(m_pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR ));

    CheckHR(m_pDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_ZENABLE,      TRUE ));
    CheckHR(m_pDevice->SetRenderState( D3DRS_LIGHTING,     FALSE ));
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependant objects are about to be lost.
//-----------------------------------------------------------------------------
void CMyD3DApplication::InvalidateDeviceObjects()
{
    m_pTerrain->InvalidateDeviceObjects();
    m_pSkyBox->InvalidateDeviceObjects();
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::DeleteDeviceObjects()
{
    m_pTerrain->Destroy();
    m_pSkyBox->Destroy();

    for( DWORD i=0; i<NUMTREETEXTURES; i++ )
        SAFE_RELEASE( m_pTreeTextures[i] );

    SAFE_RELEASE( m_pTreeVB )
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pTerrain );
    SAFE_DELETE( m_pSkyBox );
}

