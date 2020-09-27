//-----------------------------------------------------------------------------
// File: XBFile.cpp
//
// Desc: Support code for loading DirectX .X files. During the file loading, 
//       a hierarchy of frames and meshes is built. The frame nodes can have
//       child nodes, thereby providing for a hierarchial structure, and
//       typically have a transformation matrix that is applied before it's
//       child nodes are rendered. The mesh nodes contain geometry that is
//       stored in a D3DXMesh object, which wraps a vertex buffer and
//       maintains a material list.
//
//       Note: the mesh code also supports loading .xbg files. These files are
//       more-or-less pure dumps of a vertex buffer and an index buffer for a
//       mesh, so they are trivial and fast to load. They are created with the
//       MakeXBG tool, found on the XDK.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include <rmxfguid.h>
#include <rmxftmpl.h>
#include <dxfile.h>
#include "XBFile.h"
#include "XBUtil.h"




//-----------------------------------------------------------------------------
// Name: CXBGMesh()
// Desc: Mesh class constructor.
//-----------------------------------------------------------------------------
CXBGMesh::CXBGMesh()
{
    m_dwNumVertices = 0;
    m_dwNumIndices  = 0;
    m_dwNumSubsets  = 0;

    m_pSubsets      = NULL;
	m_dwRefCount    = 1;
}




//-----------------------------------------------------------------------------
// Name: ~CXBGMesh()
// Desc: Mesh class destructor.
//-----------------------------------------------------------------------------
CXBGMesh::~CXBGMesh()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys mesh objects
//-----------------------------------------------------------------------------
VOID CXBGMesh::Destroy()
{
    m_dwNumVertices = 0;
    m_dwNumIndices  = 0;
    m_dwNumSubsets  = 0;

    // Free subsets
    for( DWORD i=0; i < m_dwNumSubsets; i++ )
        SAFE_RELEASE( m_pSubsets[i].pTexture )
    if( m_pSubsets )
        delete[] m_pSubsets;

    // Free resources
    if( m_IB.Data )
        delete[] (WORD*)m_IB.Data;
    if( m_VB.Data )
        D3D_FreeContiguousMemory( (VOID*)(0x80000000|m_VB.Data) );
}




//-----------------------------------------------------------------------------
// Name: CreateFromXBG()
// Desc: Creates a mesh from a .XBG geometry. Note: .XBG files are made with
//       the MakeXBG tool.
//-----------------------------------------------------------------------------
HRESULT CXBGMesh::Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename )
{
    // Find the media file
    CHAR strMeshPath[512];
    if( FAILED( XBUtil_FindMediaFile( strMeshPath, strFilename ) ) )
        return E_FAIL;

    // Open the file
    FILE* file = fopen( strMeshPath, "rb" );
    if( file == NULL )
        return E_FAIL;

    // Read the magic number
    DWORD dwFileID;
    fread( &dwFileID, 1, sizeof(DWORD), file ); 
    if( dwFileID != XBG_FILE_ID )
    {
        fclose( file );
        return E_FAIL;
    }

    // Read the mesh info
    fread( &m_VB, 1, sizeof(XBGMESH_INFO), file );

    // Read subsets
    m_pSubsets  = new XBGMESH_SUBSET[m_dwNumSubsets];
    fread( m_pSubsets, m_dwNumSubsets, sizeof(XBGMESH_SUBSET), file ); 

    // Read indices
    m_IB.Data  = (DWORD)new WORD[m_dwNumIndices];
    fread( (VOID*)m_IB.Data, m_dwNumIndices, sizeof(WORD), file ); 

    // Create the textures
    for( DWORD i=0; i < m_dwNumSubsets; i++ )
    {
        m_pSubsets[i].pTexture = NULL;

        CHAR strTextureName[512];
        fread( strTextureName, 1, sizeof(strTextureName), file ); 
        if( strTextureName[0] )
        {
            // Expand the path and create the texture
            CHAR strTexture[MAX_PATH];
            sprintf( strTexture, "Textures\\%s", strTextureName );
            XBUtil_CreateTexture( pd3dDevice, strTexture, &m_pSubsets[i].pTexture );
        }
    }

    // Read vertex buffer data from the file
    VOID* pVertexData = D3D_AllocContiguousMemory( m_dwNumVertices*m_dwVertexSize, D3DVERTEXBUFFER_ALIGNMENT );
    fread( pVertexData, m_dwNumVertices, m_dwVertexSize, file ); 
    m_VB.Register( pVertexData );

    // Close the file and return okay
    fclose( file );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the mesh geometry.
//-----------------------------------------------------------------------------
HRESULT CXBGMesh::Render( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwFlags )
{
    // Set the object transform
    D3DXMATRIX matWorld, matWorldSaved;
    pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSaved );

    // Set the vertex stream
    pd3dDevice->SetStreamSource( 0, &m_VB, m_dwVertexSize );
    pd3dDevice->SetIndices( &m_IB, 0 );
    if( dwFlags & XBGMESH_USEMESHFVF )
        pd3dDevice->SetVertexShader( m_dwFVF );

    // Render the opqaue subsets
    if( dwFlags & XBGMESH_RENDEROPAQUESUBSETS )
    {
        for( DWORD i = 0; i < m_dwNumSubsets; i++ )
        {
            // Skip non-opaque meshes
            if( dwFlags & XBGMESH_USEMESHMATERIALS)
                if( m_pSubsets[i].mtrl.Diffuse.a < 1.0f )
                    continue;

            // Set world transform
            D3DXMATRIX matWorld;
            D3DXMatrixMultiply( &matWorld, &m_pSubsets[i].mat, &matWorldSaved );
            pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

            // Set the material
            if( dwFlags & XBGMESH_USEMESHMATERIALS )
                pd3dDevice->SetMaterial( &m_pSubsets[i].mtrl );

            // Set the texture
            if( dwFlags & XBGMESH_USEMESHTEXTURES )
                pd3dDevice->SetTexture( 0, m_pSubsets[i].pTexture );

            // Call the callback, so the app can tweak state before rendering
            // each subset
            BOOL bRenderSubset = RenderCallback( pd3dDevice, i, &m_pSubsets[i] );

            // Draw the mesh subset
            if( bRenderSubset )
            {
                DWORD dwNumPrimitives = ( D3DPT_TRIANGLESTRIP == m_dwPrimType ) ? m_pSubsets[i].dwIndexCount-2 : m_pSubsets[i].dwIndexCount/3;
                pd3dDevice->DrawIndexedPrimitive( m_dwPrimType, 0, m_pSubsets[i].dwIndexCount,
                                                  m_pSubsets[i].dwIndexStart, dwNumPrimitives );
            }
        }
    }

    // Render the transparent subsets
    if( dwFlags & XBGMESH_RENDERALPHASUBSETS )
    {
        if( dwFlags & XBGMESH_USEMESHMATERIALS)
        {
            for( DWORD i = 0; i < m_dwNumSubsets; i++ )
            {
                // Skip non-transparent meshes
                if( m_pSubsets[i].mtrl.Diffuse.a >= 1.0f )
                    continue;

                // Set world transform
                D3DXMATRIX matWorld;
                D3DXMatrixMultiply( &matWorld, &m_pSubsets[i].mat, &matWorldSaved );
                pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

                // Set the material
                if( dwFlags & XBGMESH_USEMESHMATERIALS )
                    pd3dDevice->SetMaterial( &m_pSubsets[i].mtrl );

                // Set the texture
                if( dwFlags & XBGMESH_USEMESHTEXTURES )
                    pd3dDevice->SetTexture( 0, m_pSubsets[i].pTexture );

                // Call the callback, so the app can tweak state before rendering
                // each subset
                BOOL bRenderSubset = RenderCallback( pd3dDevice, i, &m_pSubsets[i] );

                // Draw the mesh subset
                if( bRenderSubset )
                {
                    DWORD dwNumPrimitives = ( D3DPT_TRIANGLESTRIP == m_dwPrimType ) ? m_pSubsets[i].dwIndexCount-2 : m_pSubsets[i].dwIndexCount/3;
                    pd3dDevice->DrawIndexedPrimitive( m_dwPrimType, 0, m_pSubsets[i].dwIndexCount,
                                                      m_pSubsets[i].dwIndexStart, dwNumPrimitives );
                }
            }
        }
    }

    // Restore the world transform
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSaved );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CXBMesh()
// Desc: Mesh class constructor.
//-----------------------------------------------------------------------------
CXBMesh::CXBMesh( CHAR* strName )
{
    strcpy( m_strName, strName );
    m_pVB           = NULL;
    m_dwNumVertices = 0L;
    m_pIB           = NULL;
    m_dwNumIndices  = 0L;
    m_dwFVF         = 0L;
    m_dwPrimType    = D3DPT_TRIANGLELIST;
    m_dwVertexSize  = 0L;
    m_dwNumSubsets  = 0L;
    m_pSubsets      = NULL;
    m_bUseMaterials = TRUE;
    m_bUseFVF       = TRUE;
    m_dwRefCount    = 1;
}




//-----------------------------------------------------------------------------
// Name: ~CXBMesh()
// Desc: Mesh class destructor.
//-----------------------------------------------------------------------------
CXBMesh::~CXBMesh()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: CreateFromXBG()
// Desc: Creates a mesh from a .XBG geometry. Note: .XBG files are made with
//       the MakeXBG tool.
//-----------------------------------------------------------------------------
HRESULT CXBMesh::CreateFromXBG( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename )
{
    BYTE* pData;

    // Find the media file
    CHAR strMeshPath[512];
    if( FAILED( XBUtil_FindMediaFile( strMeshPath, strFilename ) ) )
        return E_FAIL;

    // Open the file
    FILE* file = fopen( strMeshPath, "rb" );
    if( file == NULL )
        return E_FAIL;

    // Read the magic number
    DWORD dwFileID;
    fread( &dwFileID, 1, sizeof(DWORD), file ); 
    if( dwFileID != XBG_FILE_ID )
    {
        fclose( file );
        return E_FAIL;
    }

    // Read the mesh info
    fread( &m_dwNumVertices, 1, sizeof(DWORD), file ); 
    fread( &m_dwNumIndices,  1, sizeof(DWORD), file ); 
    fread( &m_dwFVF,         1, sizeof(DWORD), file ); 
    fread( &m_dwVertexSize,  1, sizeof(DWORD), file ); 
    fread( &m_dwPrimType,    1, sizeof(DWORD), file ); 
    fread( &m_dwNumSubsets,  1, sizeof(DWORD), file ); 

    // Read the subset array
    m_pSubsets = new XBMESH_SUBSET[m_dwNumSubsets];
    fread( m_pSubsets, m_dwNumSubsets, sizeof(XBMESH_SUBSET), file ); 

    // Create the textures
    for( DWORD i=0; i < m_dwNumSubsets; i++ )
    {
        m_pSubsets[i].pTexture = NULL;

        CHAR strTextureName[512];
        fread( strTextureName, 1, sizeof(strTextureName), file ); 
        if( strTextureName[0] )
        {
            // Expand the path and create the texture
            CHAR strTexture[MAX_PATH];
            sprintf( strTexture, "Textures\\%s", strTextureName );
            XBUtil_CreateTexture( pd3dDevice, strTexture, &m_pSubsets[i].pTexture );
        }
    }

    // Read vertex buffer
    pd3dDevice->CreateVertexBuffer( m_dwNumVertices*m_dwVertexSize, D3DUSAGE_WRITEONLY, 
                                    m_dwFVF, D3DPOOL_DEFAULT, &m_pVB );
    m_pVB->Lock( 0, 0, &pData, 0 );
    fread( pData, m_dwNumVertices, m_dwVertexSize, file ); 
    m_pVB->Unlock();

    // Read vertex buffer
    pd3dDevice->CreateIndexBuffer( m_dwNumIndices*sizeof(WORD), D3DUSAGE_WRITEONLY, 
                                   D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB );
    m_pIB->Lock( 0, 0, &pData, 0 );
    fread( pData, m_dwNumIndices, sizeof(WORD), file ); 
    m_pIB->Unlock();

    // Close the file and return okay
    fclose( file );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateFromD3DXMesh()
// Desc: Creates mesh, loading geometry from a file.
//-----------------------------------------------------------------------------
HRESULT CXBMesh::CreateFromD3DXMesh( LPDIRECT3DDEVICE8 pd3dDevice, LPD3DXMESH pSrcMesh,
                                     LPD3DXBUFFER pAdjacencyBuffer, LPD3DXBUFFER pMtrlBuffer, 
                                     DWORD dwFVF )
{
    // Attribute sort the mesh
    DWORD* rgdwAdjacencyTemp = new DWORD[ pSrcMesh->GetNumFaces() * 3 ];
    pSrcMesh->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT,
                                  (DWORD*)pAdjacencyBuffer->GetBufferPointer(),
                                  rgdwAdjacencyTemp, NULL, NULL );
    SAFE_DELETE_ARRAY( rgdwAdjacencyTemp );

    // Clone the mesh into local memory, overriding the FVF if specified.
    DWORD dwOldFVF = pSrcMesh->GetFVF();
    DWORD dwNewFVF = dwFVF ? dwFVF : pSrcMesh->GetFVF();
    LPD3DXMESH pNewMesh;
    pSrcMesh->CloneMeshFVF( 0L, dwNewFVF, pd3dDevice, &pNewMesh );

    // Compute normals, if necessery
    if( 0==(dwOldFVF&D3DFVF_NORMAL) && 0!=(dwNewFVF&D3DFVF_NORMAL) )
        D3DXComputeNormals( pNewMesh );

    // Store mesh properties
    pNewMesh->GetVertexBuffer( &m_pVB );
    pNewMesh->GetIndexBuffer( &m_pIB );
    m_dwNumVertices = pNewMesh->GetNumVertices();
    m_dwNumIndices  = pNewMesh->GetNumFaces() * 3;
    m_dwFVF         = pNewMesh->GetFVF();
    m_dwVertexSize  = D3DXGetFVFVertexSize( m_dwFVF );

    // Build the array of subsets, used to render the mesh
    pNewMesh->GetAttributeTable( NULL, &m_dwNumSubsets );
    m_pSubsets  = new XBMESH_SUBSET[ max(1,m_dwNumSubsets) ];
    
    if( m_dwNumSubsets > 0 )
    {
        // Get subset info from mesh's attribute table
        D3DXATTRIBUTERANGE* pAttributes;
        pAttributes = new D3DXATTRIBUTERANGE[ max(1,m_dwNumSubsets) ];
        pNewMesh->GetAttributeTable( pAttributes, NULL );
        for( DWORD i = 0; i < m_dwNumSubsets; i++ )
        {
            m_pSubsets[i].dwVertexStart = pAttributes[i].VertexStart;
            m_pSubsets[i].dwVertexCount = pAttributes[i].VertexCount;
            m_pSubsets[i].dwIndexStart  = pAttributes[i].FaceStart * 3;
            m_pSubsets[i].dwIndexCount  = pAttributes[i].FaceCount * 3;
        }

        SAFE_DELETE( pAttributes );
    }
    else
    {
        // Set default subset
        m_dwNumSubsets = 1; 
        m_pSubsets[0].dwVertexStart = 0;
        m_pSubsets[0].dwVertexCount = m_dwNumVertices;
        m_pSubsets[0].dwIndexStart   = 0;
        m_pSubsets[0].dwIndexCount   = m_dwNumIndices / 3;
    }

    // Create materials and textures for the mesh
    if( pMtrlBuffer && m_dwNumSubsets > 0 )
    {
        // Allocate memory for the materials and textures
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();

        // Copy each material and create it's texture
        for( DWORD i=0; i<m_dwNumSubsets; i++ )
        {
            // Copy the material
            m_pSubsets[i].mtrl         = d3dxMtrls[i].MatD3D;
            m_pSubsets[i].mtrl.Ambient = m_pSubsets[i].mtrl.Diffuse;
            m_pSubsets[i].pTexture     = NULL;

            // Create a texture
            if( d3dxMtrls[i].pTextureFilename )
            {
                // Find the full path to the texture file
                CHAR strTexture[MAX_PATH];
                sprintf( strTexture, "Textures\\%s", d3dxMtrls[i].pTextureFilename );
    
                // Create the texture
                XBUtil_CreateTexture( pd3dDevice, strTexture, &m_pSubsets[i].pTexture );
            }
        }
    }

    // Delete the new mesh, which was created above.
    SAFE_RELEASE( pNewMesh );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates mesh, loading geometry from a file.
//-----------------------------------------------------------------------------
HRESULT CXBMesh::CreateFromX( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename,
                              DWORD dwFVF )
{
    LPD3DXMESH   pD3DXMesh         = NULL;
    LPD3DXBUFFER pAdjacencyBuffer  = NULL;
    LPD3DXBUFFER pMtrlBuffer       = NULL;
    DWORD        dwNumMaterials    = 0L;
    HRESULT      hr;

    // Find the media file
    CHAR strMeshPath[512];
    if( FAILED( hr = XBUtil_FindMediaFile( strMeshPath, strFilename ) ) )
        return hr;

    // Use D3DX to load the mesh from a .x file
    if( FAILED( hr = D3DXLoadMeshFromX( strMeshPath, D3DXMESH_SYSTEMMEM, 
                                        pd3dDevice, &pAdjacencyBuffer, &pMtrlBuffer, 
                                        &dwNumMaterials, &pD3DXMesh ) ) )
        return hr;

    // Now create our mesh internals from the D3DX mesh
    hr = CreateFromD3DXMesh( pd3dDevice, pD3DXMesh, pAdjacencyBuffer, 
                             pMtrlBuffer, dwFVF );

    // Release objects and return
    SAFE_RELEASE( pMtrlBuffer );
    SAFE_RELEASE( pAdjacencyBuffer );
    SAFE_RELEASE( pD3DXMesh );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates a mesh, loading geometry from a previously-opened DXFILE.
//-----------------------------------------------------------------------------
HRESULT CXBMesh::CreateFromXFileData( LPDIRECT3DDEVICE8 pd3dDevice,
                                      LPDIRECTXFILEDATA pFileData, DWORD dwFVF )
{
    LPD3DXMESH   pD3DXMesh         = NULL;
    LPD3DXBUFFER pAdjacencyBuffer  = NULL;
    LPD3DXBUFFER pMtrlBuffer       = NULL;
    DWORD        dwNumMaterials    = 0L;
    HRESULT      hr;

    // Use D3DX to load the mesh from the DXFILEDATA object
    if( FAILED( hr = D3DXLoadMeshFromXof( pFileData, D3DXMESH_SYSTEMMEM, pd3dDevice,
                                          &pAdjacencyBuffer, &pMtrlBuffer, 
                                          &dwNumMaterials, &pD3DXMesh ) ) )
        return hr;

    // Now create our mesh internals from the D3DX mesh
    hr = CreateFromD3DXMesh( pd3dDevice, pD3DXMesh, pAdjacencyBuffer, 
                             pMtrlBuffer, dwFVF );

    // Release objects and return
    SAFE_RELEASE( pMtrlBuffer );
    SAFE_RELEASE( pAdjacencyBuffer );
    SAFE_RELEASE( pD3DXMesh );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates mesh, loading geometry from a file.
//-----------------------------------------------------------------------------
HRESULT CXBMesh::Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename,
                         DWORD dwOptionalFVF )
{
    HRESULT hr = E_FAIL;

    // First try loading the mesh as .xbg (Xbox geometry file)
    if( FAILED(hr) )
        hr = CreateFromXBG( pd3dDevice, strFilename );

    // Next, try loading the mesh as .x (DirectX geometry file)
    if( FAILED(hr) )
        hr = CreateFromX( pd3dDevice, strFilename, dwOptionalFVF );

    // Add other file types here...

    return hr;
}




//-----------------------------------------------------------------------------
// Name: ComputeNormals()
// Desc: Compute the normals for the mesh
//-----------------------------------------------------------------------------
HRESULT CXBMesh::ComputeNormals()
{
    // Make sure the vertices have normals
    if( 0 == ( m_dwFVF & D3DFVF_NORMAL ) )
        return E_FAIL;

    // Compute the vertices' byte offset to their normals
    DWORD dwNormalOffset = 12;
    DWORD dwPositionFVF  = m_dwFVF & D3DFVF_POSITION_MASK;
    if( dwPositionFVF >= D3DFVF_XYZB1 )
        dwNormalOffset += 4*((dwPositionFVF-4)/2);

    // Gain access to vertices and faces
    BYTE* pVertices;
    WORD* pIndices;
    m_pIB->Lock( 0, 0, (BYTE**)&pIndices,  0 );
    m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    // Zero out the existing normals
    for( DWORD vtx = 0; vtx < m_dwNumVertices; vtx++ )
    {
        BYTE*        pVertex = ( (BYTE*)pVertices + (vtx * m_dwVertexSize) );
        D3DXVECTOR3* pNormal = (D3DXVECTOR3*)( pVertex + dwNormalOffset );
        (*pNormal) = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    }

    // Add in face normals
    for( DWORD face = 0; face < m_dwNumIndices/3; face++ )
    {
        WORD wFaceVtx[3];
        wFaceVtx[0] = pIndices[ face*3 + 0 ];
        wFaceVtx[1] = pIndices[ face*3 + 1 ];
        wFaceVtx[2] = pIndices[ face*3 + 2 ];

        D3DXVECTOR3* pPos0 = (D3DXVECTOR3*)( pVertices + (wFaceVtx[0] * m_dwVertexSize) );
        D3DXVECTOR3* pPos1 = (D3DXVECTOR3*)( pVertices + (wFaceVtx[1] * m_dwVertexSize) );
        D3DXVECTOR3* pPos2 = (D3DXVECTOR3*)( pVertices + (wFaceVtx[2] * m_dwVertexSize) );

        // Calculate the normal of the face from the two edge vectors
        D3DXVECTOR3  vNormal;
        D3DXVECTOR3  vEdge1 = *pPos0 - *pPos1;
        D3DXVECTOR3  vEdge2 = *pPos0 - *pPos2;
        D3DXVec3Cross( &vNormal, &vEdge1, &vEdge2 );

        for( DWORD point = 0; point < 3; point++ )
        {
            BYTE*        pVertex = ( pVertices + (wFaceVtx[point] * m_dwVertexSize) );
            D3DXVECTOR3* pNormal = (D3DXVECTOR3*)( pVertex + dwNormalOffset );
            (*pNormal) += vNormal;
        }
    }

    // Renormalize all normals
    for( vtx = 0; vtx < m_dwNumVertices; vtx++ )
    {
        BYTE*        pVertex = ( pVertices + (vtx * m_dwVertexSize) );
        D3DXVECTOR3* pNormal = (D3DXVECTOR3*)( pVertex + dwNormalOffset );
        D3DXVec3Normalize( pNormal, pNormal );
    }

    m_pIB->Unlock();
    m_pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the mesh geometry.
//-----------------------------------------------------------------------------
HRESULT CXBMesh::Destroy()
{
    // Release subsets
    for( UINT i=0; i<m_dwNumSubsets; i++ )
        SAFE_RELEASE( m_pSubsets[i].pTexture );
    SAFE_DELETE_ARRAY( m_pSubsets );
    m_dwNumSubsets = 0L;

    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    m_dwNumVertices = 0L;
    m_dwNumIndices  = 0L;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the mesh geometry.
//-----------------------------------------------------------------------------
HRESULT CXBMesh::Render( LPDIRECT3DDEVICE8 pd3dDevice, BOOL bDrawOpaqueSubsets,
                         BOOL bDrawAlphaSubsets )
{
    // Set the vertex stream
    pd3dDevice->SetStreamSource( 0, m_pVB, m_dwVertexSize );
    pd3dDevice->SetIndices( m_pIB, 0 );
    if( m_bUseFVF )
        pd3dDevice->SetVertexShader( m_dwFVF );

    // First, draw the subsets without alpha
    if( bDrawOpaqueSubsets )
    {
        for( DWORD i = 0; i < m_dwNumSubsets; i++ )
        {
            // Draw i'th subset
            if( m_bUseMaterials )
            {
                if( m_pSubsets[i].mtrl.Diffuse.a < 1.0f )
                    continue;
                pd3dDevice->SetMaterial( &m_pSubsets[i].mtrl );
                pd3dDevice->SetTexture( 0, m_pSubsets[i].pTexture );
            }

            DWORD dwNumPrimitives = ( D3DPT_TRIANGLESTRIP == m_dwPrimType ) ? m_pSubsets[i].dwIndexCount-2 : m_pSubsets[i].dwIndexCount/3;
            pd3dDevice->DrawIndexedPrimitive( m_dwPrimType, 0, m_pSubsets[i].dwIndexCount,
                                              m_pSubsets[i].dwIndexStart, dwNumPrimitives );
        }
    }

    // Then, draw the subsets with alpha
    if( bDrawAlphaSubsets && m_bUseMaterials )
    {
        for( DWORD i = 0; i < m_dwNumSubsets; i++ )
        {
            // Draw i'th subset
            if( m_pSubsets[i].mtrl.Diffuse.a == 1.0f )
                continue;
            pd3dDevice->SetMaterial( &m_pSubsets[i].mtrl );
            pd3dDevice->SetTexture( 0, m_pSubsets[i].pTexture );

            DWORD dwNumPrimitives = ( D3DPT_TRIANGLESTRIP == m_dwPrimType ) ? m_pSubsets[i].dwIndexCount-2 : m_pSubsets[i].dwIndexCount/3;
            pd3dDevice->DrawIndexedPrimitive( m_dwPrimType, 0, m_pSubsets[i].dwIndexCount,
                                              m_pSubsets[i].dwIndexStart, dwNumPrimitives );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CXBFrame()
// Desc: Frame class constructor.
//-----------------------------------------------------------------------------
CXBFrame::CXBFrame( CHAR* strName )
{
    strcpy( m_strName, strName );
    D3DXMatrixIdentity( &m_mat );
    m_pMesh  = NULL;

    m_pChild = NULL;
    m_pNext  = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CXBFrame()
// Desc: Frame class destructor.
//-----------------------------------------------------------------------------
CXBFrame::~CXBFrame()
{
    SAFE_DELETE( m_pChild );
    SAFE_DELETE( m_pNext );
}




//-----------------------------------------------------------------------------
// Name: EnumMeshes()
// Desc: Recursively enumerates through the hierarchy of frames and meshes and
//       calls a callback function for each mesh. This is useful for creating
//       shadowvolumes, calculating bounding volumes, etc..
//-----------------------------------------------------------------------------
BOOL CXBFrame::EnumMeshes( BOOL (*EnumMeshCB)(CXBMesh*,VOID*), VOID* pContext )
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
// Name: FindMesh()
// Desc: Tranverses the hierarchy, and returns a named mesh.
//-----------------------------------------------------------------------------
CXBMesh* CXBFrame::FindMesh( CHAR* strMeshName )
{
    CXBMesh* pMesh;

    if( m_pMesh )
        if( !_stricmp( m_pMesh->m_strName, strMeshName ) )
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
// Name: FindFrame()
// Desc: Tranverses the hierarchy, and returns a named frame.
//-----------------------------------------------------------------------------
CXBFrame* CXBFrame::FindFrame( CHAR* strFrameName )
{
    CXBFrame* pFrame;

    if( !_stricmp( m_strName, strFrameName ) )
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
// Name: Destroy()
// Desc: Destroys a frame.
//-----------------------------------------------------------------------------
HRESULT CXBFrame::Destroy()
{
    if( m_pMesh )  m_pMesh->Destroy();
    if( m_pChild ) m_pChild->Destroy();
    if( m_pNext )  m_pNext->Destroy();

    SAFE_DELETE( m_pMesh );
    SAFE_DELETE( m_pNext );
    SAFE_DELETE( m_pChild );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders a frame (save state, apply matrix, render children, restore).
//-----------------------------------------------------------------------------
HRESULT CXBFrame::Render( LPDIRECT3DDEVICE8 pd3dDevice, BOOL bDrawOpaqueSubsets,
                          BOOL bDrawAlphaSubsets )
{
    D3DXMATRIX matSavedWorld, matWorld;
    pd3dDevice->GetTransform( D3DTS_WORLD, &matSavedWorld );
    D3DXMatrixMultiply( &matWorld, &m_mat, &matSavedWorld );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    if( m_pMesh )
        m_pMesh->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    if( m_pChild )
        m_pChild->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    pd3dDevice->SetTransform( D3DTS_WORLD, &matSavedWorld );

    if( m_pNext )
        m_pNext->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadFrame()
// Desc: Creates a child frame, as specified in the DXFILE being loaded.
//-----------------------------------------------------------------------------
HRESULT CXBFile::LoadFrame( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData,
                            CXBFrame* pParentFrame, DWORD dwFVF )
{
    LPDIRECTXFILEDATA   pChildData = NULL;
    LPDIRECTXFILEOBJECT pChildObj = NULL;
    const GUID* pGUID;
    DWORD       cbSize;
    CXBFrame*   pCurrentFrame;
    HRESULT     hr;

    // Get the type of the object
    if( FAILED( hr = pFileData->GetType( &pGUID ) ) )
        return hr;

    if( *pGUID == TID_D3DRMMesh )
    {
        hr = LoadMesh( pd3dDevice, pFileData, pParentFrame, dwFVF );
        if( FAILED(hr) )
            return hr;
    }
    if( *pGUID == TID_D3DRMFrameTransformMatrix )
    {
        D3DXMATRIX* pmatMatrix;
        hr = pFileData->GetData( NULL, &cbSize, (VOID**)&pmatMatrix );
        if( FAILED(hr) )
            return hr;

        // Update the parents matrix with the new one
        pParentFrame->SetMatrix( pmatMatrix );
    }
    if( *pGUID == TID_D3DRMFrame )
    {
        // Get the frame name
        CHAR strName[MAX_PATH] = "";
        DWORD dwNameLength;
        pFileData->GetName( NULL, &dwNameLength );
        if( dwNameLength > 0 )
            pFileData->GetName( strName, &dwNameLength );

        // Create the frame
        pCurrentFrame = new CXBFrame( strName );

        pCurrentFrame->m_pNext = pParentFrame->m_pChild;
        pParentFrame->m_pChild = pCurrentFrame;

        // Enumerate child objects
        while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
        {
            // Query the child for it's FileData
            hr = pChildObj->QueryInterface( IID_IDirectXFileData,
                                            (VOID**)&pChildData );
            if( SUCCEEDED(hr) )
            {
                hr = LoadFrame( pd3dDevice, pChildData, pCurrentFrame, dwFVF );
                pChildData->Release();
            }

            pChildObj->Release();

            if( FAILED(hr) )
                return hr;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadMesh()
// Desc: Creates a mesh, as specified in the DXFILE being loaded.
//-----------------------------------------------------------------------------
HRESULT CXBFile::LoadMesh( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTXFILEDATA pFileData,
                           CXBFrame* pParentFrame, DWORD dwFVF )
{
    // Currently only allowing one mesh per frame
    if( pParentFrame->m_pMesh )
    {
        OutputDebugStringA( "CXBFile::LoadMesh(): Only one mesh per frame allowed!\n" );
        return E_FAIL;
    }

    // Get the mesh name
    CHAR strName[MAX_PATH] = "";
    DWORD dwNameLength;
    pFileData->GetName( NULL, &dwNameLength );
    if( dwNameLength > 0 )
        pFileData->GetName( strName, &dwNameLength );

    // Create the mesh
    pParentFrame->m_pMesh = new CXBMesh( strName );
    pParentFrame->m_pMesh->CreateFromXFileData( pd3dDevice, pFileData, dwFVF );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Loads the specified .x file, creating a hierarchy of frames and
//       meshes.
//-----------------------------------------------------------------------------
HRESULT CXBFile::Create( LPDIRECT3DDEVICE8 pd3dDevice, CHAR* strFilename,
                         DWORD dwFVF )
{
    LPDIRECTXFILE           pDXFile   = NULL;
    LPDIRECTXFILEENUMOBJECT pEnumObj  = NULL;
    LPDIRECTXFILEDATA       pFileData = NULL;
    HRESULT hr;

    // Create a x file object
    if( FAILED( hr = DirectXFileCreate( &pDXFile ) ) )
        return E_FAIL;

    // Register templates for d3drm and patch extensions.
    if( FAILED( hr = pDXFile->RegisterTemplates( (VOID*)D3DRM_XTEMPLATES,
                                                 D3DRM_XTEMPLATE_BYTES ) ) )
    {
        pDXFile->Release();
        return E_FAIL;
    }

    // Find the path to the file
    CHAR strPath[MAX_PATH];
    XBUtil_FindMediaFile( strPath, strFilename );

    // Create the file
    hr = pDXFile->CreateEnumObject( strPath, DXFILELOAD_FROMFILE, &pEnumObj );
    if( FAILED(hr) )
    {
        pDXFile->Release();
        return hr;
    }

    // Enumerate top level objects (which are always frames)
    while( SUCCEEDED( pEnumObj->GetNextDataObject( &pFileData ) ) )
    {
        hr = LoadFrame( pd3dDevice, pFileData, this, dwFVF );
        pFileData->Release();
        if( FAILED(hr) )
        {
            pEnumObj->Release();
            pDXFile->Release();
            return E_FAIL;
        }
    }

    SAFE_RELEASE( pFileData );
    SAFE_RELEASE( pEnumObj );
    SAFE_RELEASE( pDXFile );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the hierarchy of frames and meshes previousy loaded.
//-----------------------------------------------------------------------------
HRESULT CXBFile::Render( LPDIRECT3DDEVICE8 pd3dDevice )
{
    // Setup the world transformation
    D3DXMATRIX matSavedWorld, matWorld;
    pd3dDevice->GetTransform( D3DTS_WORLD, &matSavedWorld );
    D3DXMatrixMultiply( &matWorld, &matSavedWorld, &m_mat );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Render opaque subsets in the meshes
    if( m_pChild )
        m_pChild->Render( pd3dDevice, TRUE, FALSE );

    // Enable alpha blending
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Render alpha subsets in the meshes
    if( m_pChild )
        m_pChild->Render( pd3dDevice, FALSE, TRUE );

    // Restore state
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matSavedWorld );

    return S_OK;
}



