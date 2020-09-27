//-----------------------------------------------------------------------------
// File: Dolphin.cpp
//
// Desc: Sample of swimming dolphin
//
//       Note: This code uses the D3D Framework helper library.
//
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT

#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#include <D3DX8.h>
#endif
#include <stdio.h>
#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "DXUtil.h"

//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

#define WATER_COLOR         0x00006688

//-----------------------------------------------------------------------------
// Name: struct MESHTOOL
// Desc: Convenient structure for accessing a mesh's vertices. Pass in a ptr to
//       a D3DXMESH object, and then the vertices can be accessed easily. The
//       destructor nicely cleans everything up.
//-----------------------------------------------------------------------------
struct MESHTOOL
{
    struct VERTEX { D3DXVECTOR3 p, n; FLOAT tu, tv; }; // Same as a Vertex

    LPDIRECT3DVERTEXBUFFER8 pVB;
    DWORD                   dwNumVertices;
    VERTEX*                 pVertices;

    MESHTOOL( LPD3DXMESH pMesh )
    {
        CheckHR(pMesh->GetVertexBuffer( &pVB ));
        CheckHR(pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ));
        dwNumVertices= pMesh->GetNumVertices();
    }

    ~MESHTOOL()
    {
        CheckHR(pVB->Unlock());
        CheckHR(pVB->Release());
    }
};

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Main class to run this application. Most functionality is inherited
//       from the CD3DApplication base class.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    // The dolphin object
    CD3DFile* m_pDolphinGroupObject;
    CD3DFile* m_pDolphinObject;
    CD3DMesh* m_pDolphinMesh;
    CD3DMesh* m_pDolphinMesh01;
    CD3DMesh* m_pDolphinMesh02;
    CD3DMesh* m_pDolphinMesh03;

    // The seafloor object
    CD3DFile* m_pFloorObject;
    CD3DMesh* m_pSeaFloorMesh;

    // Textures for the water caustics
    LPDIRECT3DTEXTURE8 m_pCausticTextures[32];
    LPDIRECT3DTEXTURE8 m_pCurrentCausticTexture;

    // Function used to blend the dolphin meshes
    void BlendMeshes( FLOAT fWeight );

public:
    void InitDeviceObjects();
    void RestoreDeviceObjects();
    void InvalidateDeviceObjects();
    void DeleteDeviceObjects();
    void Render();
    void FrameMove();
    void FinalCleanup();

    CMyD3DApplication();
};




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------

#ifdef XBOX
void __cdecl main()
#else
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
#endif
{
    CMyD3DApplication d3dApp;

#ifdef XBOX
    d3dApp.Create(NULL);
#else
    d3dApp.Create(hInst);
#endif

    d3dApp.Run();

#ifndef XBOX
    return 0;
#endif
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Constructor
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
                  :CD3DApplication()
{
    // Override base class members
    m_bUseDepthBuffer   = TRUE;
    m_DepthBufferFormat = D3DFMT_D16;

    // Initialize member variables
    m_pFloorObject        = new CD3DFile();
    m_pDolphinGroupObject = new CD3DFile();
    m_pDolphinObject      = new CD3DFile();

    for( DWORD t=0; t<32; t++ )
        m_pCausticTextures[t] = NULL;
}




//-----------------------------------------------------------------------------
// Name: BlendMeshes()
// Desc: Does a linear interpolation between all vertex positions and normals
//       in two source meshes and outputs the result to the destination mesh.
//       Note: all meshes must contain the same number of vertices, and the
//       destination mesh must be in device memory.
//-----------------------------------------------------------------------------
void CMyD3DApplication::BlendMeshes( FLOAT fWeight )
{
    // Build convenient objects to access the mesh vertices
    MESHTOOL dest( m_pDolphinMesh->GetLocalMesh() );
    MESHTOOL src2( m_pDolphinMesh02->GetSysMemMesh() );
    MESHTOOL src1( fWeight<0.0f ? m_pDolphinMesh03->GetSysMemMesh()
                                : m_pDolphinMesh01->GetSysMemMesh() );

    // The blending factors
    FLOAT fWeight1 = fabsf(fWeight);
    FLOAT fWeight2 = 1.0f - fWeight1;

    // LERP positions and normals
    for( DWORD i=0; i<dest.dwNumVertices; i++ )
    {
        dest.pVertices[i].p = fWeight1*src1.pVertices[i].p +
                              fWeight2*src2.pVertices[i].p;

        dest.pVertices[i].n = fWeight1*src1.pVertices[i].n +
                              fWeight2*src2.pVertices[i].n;
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FrameMove()
{
        // Animation attributes for the dolphin
    FLOAT fKickFreq    = 2*m_fTime;
    FLOAT fPhase       = m_fTime/3;
    FLOAT fBlendWeight = sinf( fKickFreq );

    // Blend the meshes (which makes the dolphin appear to swim)
    BlendMeshes( fBlendWeight );

    // Move the dolphin in a circle
    D3DXMATRIX* pmatDolphin = m_pDolphinObject->GetMatrix();
    D3DXMATRIX  matTrans, matRotate1, matRotate2;
    D3DXMatrixScaling( pmatDolphin, 0.01f, 0.01f, 0.01f );
    D3DXMatrixRotationZ( &matRotate1, -cosf(fKickFreq)/6 );
    D3DXMatrixMultiply( pmatDolphin, pmatDolphin, &matRotate1 );
    D3DXMatrixRotationY( &matRotate2, fPhase );
    D3DXMatrixMultiply( pmatDolphin, pmatDolphin, &matRotate2 );
    D3DXMatrixTranslation( &matTrans, -5*sinf(fPhase), sinf(fKickFreq)/2, 10-10*cosf(fPhase) );
    D3DXMatrixMultiply( pmatDolphin, pmatDolphin, &matTrans );

    // Animate the caustic textures
    DWORD tex = ((DWORD)(m_fTime*32))%32;
    m_pCurrentCausticTexture = m_pCausticTextures[tex];
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------

void CMyD3DApplication::Render()
{
    // Clear the viewport
    CheckHR( m_pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         WATER_COLOR, 1.0f, 0L ));

    // Begin the scene
    CheckHR( m_pDevice->BeginScene() );

    {
        // Render the Seafloor. For devices that support one-pass multi-
        // texturing, use the second texture stage to blend in the animated
        // water caustics texture.
        if( m_Caps.MaxSimultaneousTextures > 1 )
        {
            // Set up the 2nd texture stage for the animated water caustics
            CheckHR( m_pDevice->SetTexture( 1, m_pCurrentCausticTexture ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT ));

            // Tell D3D to automatically generate texture coordinates from the
            // model's position in camera space. The texture transform matrix is
            // setup so that the 'x' and 'z' coordinates are scaled to become the
            // resulting 'tu' and 'tv' texture coordinates. The resulting effect
            // is that the caustic texture is draped over the geometry from above.
            D3DXMATRIX mat;
            mat._11 = 0.05f;           mat._12 = 0.00f;
            mat._21 = 0.00f;           mat._22 = 0.00f;
            mat._31 = 0.00f;           mat._32 = 0.05f;
            mat._41 = sinf(m_fTime)/8; mat._42 = cosf(m_fTime)/10 - m_fTime/10;

            CheckHR( m_pDevice->SetTransform( D3DTS_TEXTURE1, &mat ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 ));
        }

        // Finally, render the actual seafloor with the above states

        m_pFloorObject->Render( m_pDevice );

        // Disable the second texture stage
        if( m_Caps.MaxSimultaneousTextures > 1 )
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE ));

        // Render the dolphin in it's first pass.

        m_pDolphinObject->Render( m_pDevice );

        // For devices that support one-pass multi-texturing, use the second
        // texture stage to blend in the animated water caustics texture for
        // the dolphin. This a little tricky because we only want caustics on
        // the part of the dolphin that is lit from above. To acheive this
        // effect, the dolphin is rendered alpha-blended with a second pass
        // which has the caustic effects modulating the diffuse component
        // (which contains lighting-only information) of the geometry.
        if( m_Caps.MaxSimultaneousTextures > 1 )
        {
            // For the 2nd pass of rendering the dolphin, turn on the caustic
            // effects. Start with setting up the 2nd texture stage state, which
            // will modulate the texture with the diffuse component. This actually
            // only needs one stage, except that using a CD3DFile object makes that
            // a little tricky.
            CheckHR( m_pDevice->SetTexture( 1, m_pCurrentCausticTexture ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_DIFFUSE ));

            // Now, set up D3D to generate texture coodinates. This is the same as
            // with the seafloor (the 'x' and 'z' position coordinates in camera
            // space are used to generate the 'tu' and 'tv' texture coordinates),
            // except our scaling factors are different in the texture matrix, to
            // get a better looking result.
            D3DXMATRIX mat;
            ZeroMemory(&mat, sizeof(mat));
            mat._11 = 0.50f; mat._12 = 0.00f;
            mat._21 = 0.00f; mat._22 = 0.00f;
            mat._31 = 0.00f; mat._32 = 0.50f;
            mat._41 = 0.00f; mat._42 = 0.00f;
            CheckHR( m_pDevice->SetTransform( D3DTS_TEXTURE1, &mat ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION ));
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 ));

            // Set the ambient color and fog color to pure black. Ambient is set
            // to black because we still have a light shining from above, but we
            // don't want any caustic effects on the dolphin's underbelly. Fog is
            // set to black because we want the caustic effects to fade out in the
            // distance just as the model does with the WATER_COLOR.
            CheckHR( m_pDevice->SetRenderState( D3DRS_AMBIENT,   0x00000000 ));
            CheckHR( m_pDevice->SetRenderState( D3DRS_FOGCOLOR,  0x00000000 ));

            // Set up blending modes to add this caustics-only pass with the
            // previous pass.
            CheckHR( m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ));
            CheckHR( m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR ));
            CheckHR( m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE ));

            // Finally, render the caustic effects for the dolphin

            m_pDolphinObject->Render( m_pDevice );

            // After all is well and done, restore any munged texture stage states
            CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE ));
            CheckHR( m_pDevice->SetRenderState( D3DRS_AMBIENT,  0x33333333 ));
            CheckHR( m_pDevice->SetRenderState( D3DRS_FOGCOLOR, WATER_COLOR ));
            CheckHR( m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ));
        }

        // End the scene.
        CheckHR( m_pDevice->EndScene() );
    }
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize device-dependant objects. This is the place to create mesh
//       and texture objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::InitDeviceObjects()
{
    // Set up textures for the water caustics
    for( DWORD t=0; t<32; t++ )
    {
        CHAR strName[80];
        sprintf( strName, "Caust%02ld.tga", t );
        CheckHR( D3DUtil_CreateTexture( m_pDevice, strName,
                                           &m_pCausticTextures[t] ) );
    }

    // Load the file-based mesh objects
    m_pDolphinGroupObject->Create( m_pDevice, "dolphing.x" );
    m_pDolphinObject->Create( m_pDevice, "dolphin.x" );
    m_pFloorObject->Create( m_pDevice, "seafloor.x" );

        // Gain access to the meshes
    m_pDolphinMesh01 = m_pDolphinGroupObject->FindMesh( "Dolph01" );
    m_pDolphinMesh02 = m_pDolphinGroupObject->FindMesh( "Dolph02" );
    m_pDolphinMesh03 = m_pDolphinGroupObject->FindMesh( "Dolph03" );
    m_pDolphinMesh   = m_pDolphinObject->FindMesh( "Dolph02" );
    m_pSeaFloorMesh  = m_pFloorObject->FindMesh( "SeaFloor" );

        // Set the FVF type to something useful
    m_pDolphinMesh01->SetFVF( m_pDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );
    m_pDolphinMesh02->SetFVF( m_pDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );
    m_pDolphinMesh03->SetFVF( m_pDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );
    m_pDolphinMesh->SetFVF(   m_pDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );
    m_pSeaFloorMesh->SetFVF(  m_pDevice, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );

     // Scale the sea floor vertices, and add some bumpiness
    {
        MESHTOOL dest( m_pSeaFloorMesh->GetSysMemMesh() );
        srand(5);

        for( DWORD i=0; i<dest.dwNumVertices; i++ )
        {
            dest.pVertices[i].p.y += (rand()/(FLOAT)RAND_MAX);
            dest.pVertices[i].p.y += (rand()/(FLOAT)RAND_MAX);
            dest.pVertices[i].p.y += (rand()/(FLOAT)RAND_MAX);
            dest.pVertices[i].tu  *= 10;
            dest.pVertices[i].tv  *= 10;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------
void CMyD3DApplication::RestoreDeviceObjects()
{
    // Build the device objects for the files
    m_pDolphinGroupObject->RestoreDeviceObjects( m_pDevice );
    m_pDolphinObject->RestoreDeviceObjects( m_pDevice );
    m_pFloorObject->RestoreDeviceObjects( m_pDevice );

    // Set world transform
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    CheckHR( m_pDevice->SetTransform( D3DTS_WORLD, &matWorld ));

    // Set the view matrix
    D3DXMATRIX  matView;
    D3DXVECTOR3 vEyePt      = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt   = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXVECTOR3 vUpVec      = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    CheckHR( m_pDevice->SetTransform( D3DTS_VIEW, &matView ));

    // Set projection transform
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_BackBufferDesc.Width) / m_BackBufferDesc.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, fAspect, 1.0f, 10000.0f );
    CheckHR( m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj ));

    CheckHR( m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ));
    CheckHR( m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ));
    CheckHR( m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE ));
    CheckHR( m_pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR ));
    CheckHR( m_pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR ));
    CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR ));
    CheckHR( m_pDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR ));

    // Set default render states
    CheckHR( m_pDevice->SetRenderState( D3DRS_DITHERENABLE,     TRUE ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_SPECULARENABLE,   FALSE ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_ZENABLE,          TRUE ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE ));

    // Turn on fog, for underwater effect
    FLOAT fFogStart =  1.0f;
    FLOAT fFogEnd   = 50.0f;
    CheckHR( m_pDevice->SetRenderState( D3DRS_FOGENABLE,      TRUE ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_FOGCOLOR,       WATER_COLOR ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_FOGSTART,       FtoDW(fFogStart) ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_FOGEND,         FtoDW(fFogEnd) ));

    // Set up the lighting states
    D3DLIGHT8 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0f, -1.0f, 0.0f );
    CheckHR( m_pDevice->SetLight( 0, &light ));
    CheckHR( m_pDevice->LightEnable( 0, TRUE ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE ));
    CheckHR( m_pDevice->SetRenderState( D3DRS_AMBIENT, 0x33333333 ));
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependant objects are about to be lost.
//-----------------------------------------------------------------------------
void CMyD3DApplication::InvalidateDeviceObjects()
{
    m_pFloorObject->InvalidateDeviceObjects();
    m_pDolphinGroupObject->InvalidateDeviceObjects();
    m_pDolphinObject->InvalidateDeviceObjects();
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
void CMyD3DApplication::DeleteDeviceObjects()
{
    InvalidateDeviceObjects();

    m_pFloorObject->Destroy();
    m_pDolphinGroupObject->Destroy();
    m_pDolphinObject->Destroy();

    for( DWORD i=0; i<32; i++ )
        SAFE_RELEASE( m_pCausticTextures[i] );
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
void CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pDolphinGroupObject );
    SAFE_DELETE( m_pDolphinObject );
    SAFE_DELETE( m_pFloorObject );
}

