//-----------------------------------------------------------------------------
// File: D3DFile.cpp
//
// Desc: Support code for loading DirectX .X files.
//
// Copyright (c) 1997-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT

#include <stdio.h>
#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#include <d3d8.h>
#include <d3dx8core.h>
#endif

#include <rmxfguid.h>
#include <rmxftmpl.h>

#include <dxfile.h>
#include "D3DFile.h"
#include "DXUtil.h"

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DMesh::CD3DMesh( CHAR* strName )
{
    strcpy( m_strName, strName );
    m_pSysMemMesh        = NULL;
    m_pLocalMesh         = NULL;
    m_dwNumMaterials     = 0L;
    m_pMaterials         = NULL;
    m_pTextures          = NULL;
    m_bUseMaterials      = TRUE;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DMesh::~CD3DMesh()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DMesh::Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename )
{
    CHAR        strPath[MAX_PATH];

    LPD3DXBUFFER pMtrlBuffer = NULL;

    // Find the path for the file
	DXUtil_FindMediaFile( strPath, strFilename );

    // Load the mesh

    CheckHR( D3DXLoadMeshFromX(strPath, D3DXMESH_SYSTEMMEM, 
                               pd3dDevice, NULL, &pMtrlBuffer, 
                               &m_dwNumMaterials, &m_pSysMemMesh ) );

    // Get material info for the mesh
    // Get the array of materials out of the buffer
    if( pMtrlBuffer && m_dwNumMaterials > 0 )
    {
        // Allocate memory for the materials and textures
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
        m_pMaterials = new D3DMATERIAL8[m_dwNumMaterials];
        m_pTextures  = new LPDIRECT3DTEXTURE8[m_dwNumMaterials];

        // Copy each material and create it's texture
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
			// Copy the material
            m_pMaterials[i]         = d3dxMtrls[i].MatD3D;
            m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;
			m_pTextures[i]          = NULL;

            // Create a texture
            if( d3dxMtrls[i].pTextureFilename )
            {
                CHAR strTexture[MAX_PATH];

				DXUtil_FindMediaFile( strTexture, d3dxMtrls[i].pTextureFilename );

                if( FAILED( D3DXCreateTextureFromFileA(pd3dDevice, 
													   strTexture, 
					                                   &m_pTextures[i] ) ) )
					m_pTextures[i] = NULL;
            }
        }
    }

    SAFE_RELEASE( pMtrlBuffer );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DMesh::Create( LPDIRECT3DDEVICE8 pd3dDevice,
                          LPDIRECTXFILEDATA pFileData )
{
    LPD3DXBUFFER pMtrlBuffer = NULL;

    // Load the mesh from the DXFILEDATA object
    CheckHR( D3DXLoadMeshFromXof( pFileData, D3DXMESH_SYSTEMMEM, pd3dDevice,
                              NULL, &pMtrlBuffer, &m_dwNumMaterials,
                              &m_pSysMemMesh ));

    // Get material info for the mesh
    // Get the array of materials out of the buffer
    if( pMtrlBuffer && m_dwNumMaterials > 0 )
    {
        // Allocate memory for the materials and textures
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
        m_pMaterials = new D3DMATERIAL8[m_dwNumMaterials];
        m_pTextures  = new LPDIRECT3DTEXTURE8[m_dwNumMaterials];

        // Copy each material and create it's texture
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
			// Copy the material
            m_pMaterials[i]         = d3dxMtrls[i].MatD3D;
            m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;
			m_pTextures[i]          = NULL;

            // Create a texture
            if( d3dxMtrls[i].pTextureFilename )
            {
                CHAR strTexture[MAX_PATH];

				DXUtil_FindMediaFile( strTexture, d3dxMtrls[i].pTextureFilename );

                if( FAILED( D3DXCreateTextureFromFileA(pd3dDevice, strTexture, 
					                                   &m_pTextures[i] ) ) )
					m_pTextures[i] = NULL;
            }
        }
    }

    SAFE_RELEASE( pMtrlBuffer );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DMesh::SetFVF( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwFVF )
{
    LPD3DXMESH pTempSysMemMesh = NULL;
    LPD3DXMESH pTempLocalMesh  = NULL;

    if( m_pSysMemMesh )
    {
        CheckHR( m_pSysMemMesh->CloneMeshFVF( D3DXMESH_SYSTEMMEM, dwFVF,
                                                 pd3dDevice, &pTempSysMemMesh ) );

    }
    if( m_pLocalMesh )
    {
        CheckHR( m_pLocalMesh->CloneMeshFVF( 0L, dwFVF, pd3dDevice, &pTempLocalMesh ) );
    }

    SAFE_RELEASE( m_pSysMemMesh );
    SAFE_RELEASE( m_pLocalMesh );

    if( pTempSysMemMesh ) m_pSysMemMesh = pTempSysMemMesh;
    if( pTempLocalMesh )  m_pLocalMesh  = pTempLocalMesh;

    // Compute normals in case the meshes have them
    if( m_pSysMemMesh )
        D3DXComputeNormals( m_pSysMemMesh );
    if( m_pLocalMesh )
        D3DXComputeNormals( m_pLocalMesh );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DMesh::RestoreDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice )
{
    // Make a local memory version of the mesh. Note: because we are passing in
    // no flags, the default behavior is to clone into local memory.
    CheckHR( m_pSysMemMesh->CloneMeshFVF( 0L, m_pSysMemMesh->GetFVF(),
                                             pd3dDevice, &m_pLocalMesh ) );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DMesh::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pLocalMesh );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DMesh::Destroy()
{
    for( UINT i=0; i<m_dwNumMaterials; i++ )
        SAFE_RELEASE( m_pTextures[i] );
    SAFE_DELETE_ARRAY( m_pTextures );
    SAFE_DELETE_ARRAY( m_pMaterials );

    SAFE_RELEASE( m_pSysMemMesh );

    m_dwNumMaterials = 0L;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DMesh::Render( LPDIRECT3DDEVICE8 pd3dDevice, BOOL bDrawOpaqueSubsets,
                          BOOL bDrawAlphaSubsets )
{
    // Frist, draw the subsets without alpha
    if( bDrawOpaqueSubsets )
    {
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            if( m_bUseMaterials )
            {
                if( m_pMaterials[i].Diffuse.a < 1.0f )
                    continue;
                CheckHR( pd3dDevice->SetMaterial( &m_pMaterials[i] ));
                CheckHR( pd3dDevice->SetTexture( 0, m_pTextures[i] ));
            }
            CheckHR( m_pLocalMesh->DrawSubset( i ));
        }
    }

    // Then, draw the subsets with alpha
    if( bDrawAlphaSubsets && m_bUseMaterials )
    {
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            if( m_pMaterials[i].Diffuse.a == 1.0f )
                continue;

            // Set the material and texture
            CheckHR( pd3dDevice->SetMaterial( &m_pMaterials[i] ));
            CheckHR( pd3dDevice->SetTexture( 0, m_pTextures[i] ));
            CheckHR( m_pLocalMesh->DrawSubset( i ));
        }
    }
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DFrame::CD3DFrame( CHAR* strName )
{
    strcpy( m_strName, strName );
    D3DXMatrixIdentity( &m_mat );
    m_pMesh  = NULL;

    m_pChild = NULL;
    m_pNext  = NULL;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DFrame::~CD3DFrame()
{
    SAFE_DELETE( m_pChild );
    SAFE_DELETE( m_pNext );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
BOOL CD3DFrame::EnumMeshes( BOOL (*EnumMeshCB)(CD3DMesh*,VOID*),
                            VOID* pContext )
{
    if( m_pMesh )
        EnumMeshCB( m_pMesh, pContext );
    if( m_pChild )
        m_pChild->EnumMeshes( EnumMeshCB, pContext );
    if( m_pNext )
        m_pNext->EnumMeshes( EnumMeshCB, pContext );

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DMesh* CD3DFrame::FindMesh( CHAR* strMeshName )
{
    CD3DMesh* pMesh;

    if( m_pMesh )
        if( !_strcmpi( m_pMesh->m_strName, strMeshName ) )
            return m_pMesh;

    if( m_pChild )
        if( NULL != ( pMesh = m_pChild->FindMesh( strMeshName ) ) )
            return pMesh;

    if( m_pNext )
        if( NULL != ( pMesh = m_pNext->FindMesh( strMeshName ) ) )
            return pMesh;

    return NULL;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DFrame* CD3DFrame::FindFrame( CHAR* strFrameName )
{
    CD3DFrame* pFrame;

    if( !_strcmpi( m_strName, strFrameName ) )
        return this;

    if( m_pChild )
        if( NULL != ( pFrame = m_pChild->FindFrame( strFrameName ) ) )
            return pFrame;

    if( m_pNext )
        if( NULL != ( pFrame = m_pNext->FindFrame( strFrameName ) ) )
            return pFrame;

    return NULL;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DFrame::Destroy()
{
    if( m_pMesh )  m_pMesh->Destroy();
    if( m_pChild ) m_pChild->Destroy();
    if( m_pNext )  m_pNext->Destroy();

    SAFE_DELETE( m_pMesh );
    SAFE_DELETE( m_pNext );
    SAFE_DELETE( m_pChild );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DFrame::RestoreDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice )
{
    if( m_pMesh )  m_pMesh->RestoreDeviceObjects( pd3dDevice );
    if( m_pChild ) m_pChild->RestoreDeviceObjects( pd3dDevice );
    if( m_pNext )  m_pNext->RestoreDeviceObjects( pd3dDevice );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DFrame::InvalidateDeviceObjects()
{
    if( m_pMesh )  m_pMesh->InvalidateDeviceObjects();
    if( m_pChild ) m_pChild->InvalidateDeviceObjects();
    if( m_pNext )  m_pNext->InvalidateDeviceObjects();
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DFrame::Render( LPDIRECT3DDEVICE8 pd3dDevice, BOOL bDrawOpaqueSubsets,
                           BOOL bDrawAlphaSubsets )
{
    D3DXMATRIX matSavedWorld, matWorld;
    CheckHR( pd3dDevice->GetTransform( D3DTS_WORLD, &matSavedWorld ));
    D3DXMatrixMultiply( &matWorld, &m_mat, &matSavedWorld );
    CheckHR( pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld ));

    if( m_pMesh )
        m_pMesh->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    if( m_pChild )
        m_pChild->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    CheckHR( pd3dDevice->SetTransform( D3DTS_WORLD, &matSavedWorld ));

    if( m_pNext )
        m_pNext->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DFile::LoadFrame( LPDIRECT3DDEVICE8 pd3dDevice,
                             LPDIRECTXFILEDATA pFileData,
                             CD3DFrame* pParentFrame )
{
    LPDIRECTXFILEDATA   pChildData = NULL;
    LPDIRECTXFILEOBJECT pChildObj = NULL;
    const GUID* pGUID;
    DWORD       cbSize;
    CD3DFrame*  pCurrentFrame;

    // Get the type of the object
    CheckHR( pFileData->GetType( &pGUID ) );

    if( *pGUID == TID_D3DRMMesh )
    {
        LoadMesh( pd3dDevice, pFileData, pParentFrame );
    }
    if( *pGUID == TID_D3DRMFrameTransformMatrix )
    {
        D3DXMATRIX* pmatMatrix;
        CheckHR( pFileData->GetData( NULL, &cbSize, (VOID**)&pmatMatrix ));

        // Update the parents matrix with the new one
        pParentFrame->SetMatrix( pmatMatrix );
    }
    if( *pGUID == TID_D3DRMFrame )
    {
        // Get the frame name
        CHAR strName[MAX_PATH] = "";
        DWORD dwNameLength;
        CheckHR( pFileData->GetName( NULL, &dwNameLength ));
        if( dwNameLength > 0 )
		{
            CheckHR( pFileData->GetName( strName, &dwNameLength ));
		}

        // Create the frame
        pCurrentFrame = new CD3DFrame( strName );

        pCurrentFrame->m_pNext = pParentFrame->m_pChild;
        pParentFrame->m_pChild = pCurrentFrame;

        // Enumerate child objects
        while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
        {
            // Query the child for it's FileData
            CheckHR( pChildObj->QueryInterface( IID_IDirectXFileData,
                                            (VOID**)&pChildData ));

            LoadFrame( pd3dDevice, pChildData, pCurrentFrame );
            pChildData->Release();

            pChildObj->Release();
        }
    }
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DFile::LoadMesh( LPDIRECT3DDEVICE8 pd3dDevice,
                            LPDIRECTXFILEDATA pFileData,
                            CD3DFrame* pParentFrame )
{
    // Currently only allowing one mesh per frame
    if( pParentFrame->m_pMesh )
	{
		DisplayError(L"FATAL: We only allow one mesh per frame", 0);
	}

    // Get the mesh name
    CHAR strName[MAX_PATH] = "";
    DWORD dwNameLength;
    pFileData->GetName( NULL, &dwNameLength );
    if( dwNameLength > 0 )
	{
        CheckHR( pFileData->GetName( strName, &dwNameLength ));
	}

    // Create the mesh
    pParentFrame->m_pMesh = new CD3DMesh( strName );
    pParentFrame->m_pMesh->Create( pd3dDevice, pFileData );
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DFile::Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename )
{
    LPDIRECTXFILE           pDXFile   = NULL;
    LPDIRECTXFILEENUMOBJECT pEnumObj  = NULL;
    LPDIRECTXFILEDATA       pFileData = NULL;

    // Create a x file object
    CheckHR( DirectXFileCreate( &pDXFile ) );

    // Register templates for d3drm and patch extensions.
    CheckHR( pDXFile->RegisterTemplates( (VOID*)D3DRM_XTEMPLATES,
                                                 D3DRM_XTEMPLATE_BYTES ) );

    CHAR strPath[MAX_PATH];

	DXUtil_FindMediaFile( strPath, strFilename );

    // Create enum object
    CheckHR( pDXFile->CreateEnumObject( strPath, DXFILELOAD_FROMFILE, 
                                    &pEnumObj ));

    // Enumerate top level objects (which are always frames)
    while( SUCCEEDED( pEnumObj->GetNextDataObject( &pFileData ) ) )
    {
        LoadFrame( pd3dDevice, pFileData, this );
        pFileData->Release();
    }

    SAFE_RELEASE( pFileData );
    SAFE_RELEASE( pEnumObj );
    SAFE_RELEASE( pDXFile );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DFile::Render( LPDIRECT3DDEVICE8 pd3dDevice )
{
    // Setup the world transformation
    D3DXMATRIX matSavedWorld, matWorld;
    CheckHR( pd3dDevice->GetTransform( D3DTS_WORLD, &matSavedWorld ));
    D3DXMatrixMultiply( &matWorld, &matSavedWorld, &m_mat );
    CheckHR( pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld ));

    // Render opaque subsets in the meshes
    if( m_pChild )
        m_pChild->Render( pd3dDevice, TRUE, FALSE );

    // Enable alpha blending
    CheckHR( pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ));
    CheckHR( pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA ));
    CheckHR( pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ));

    // Render alpha subsets in the meshes
    if( m_pChild )
        m_pChild->Render( pd3dDevice, FALSE, TRUE );

    // Restore state
    CheckHR( pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ));
    CheckHR( pd3dDevice->SetTransform( D3DTS_WORLD, &matSavedWorld ));
}




