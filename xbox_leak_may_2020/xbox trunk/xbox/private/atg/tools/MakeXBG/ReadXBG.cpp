//-----------------------------------------------------------------------------
// File: ReadXBG.cpp
//
// Desc: Reads a .xbg file into a CD3DFile class (with a hierarchy of CD3DFrame
//       classes).
//
//       See the XBG.h header file for a better description of .xbg geometry
//       files.
//
// NOTE: THIS CODE, AND THE FILE FORMAT IT CREATES, IS SUBJECT TO CHANGE. FEEL
//       FREE, AS WELL, TO EXPAND OR MODIFY THE FILE FORMAT TO FIT YOUR 
//       PERSONAL REQUIREMENTS.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <d3dx8.h>
#include "xbg.h"




//-----------------------------------------------------------------------------
// Name: PostLoadCB()
// Desc: Frame enumeration callback to compute the bounding sphere radius
//-----------------------------------------------------------------------------
BOOL PostLoadCB( CD3DFrame* pFrame, D3DXMATRIX* pmatWorld, VOID* pData )
{
    CD3DFile* pRoot = (CD3DFile*)pData;

    // Tag the frame
    pFrame->m_dwEnumeratedID = pRoot->m_dwNumFrames;

    // Add one to the frame count
    pRoot->m_dwNumFrames++;

    // Per mesh stuff
    if( pFrame->m_pMeshVB )
    {
        // Count primitives
        pRoot->m_dwNumMeshes++;
        pRoot->m_dwNumVertices += pFrame->m_dwNumMeshVertices;
        pRoot->m_dwNumIndices  += pFrame->m_dwNumMeshIndices;
        pRoot->m_dwNumPolygons += pFrame->m_dwNumMeshPolygons;

        // Compute the bounding sphere
        BYTE* pVertices;
        pFrame->m_pMeshVB->Lock( 0, 0, &pVertices, D3DLOCK_NOSYSLOCK );
        D3DXComputeBoundingSphere( pVertices, pFrame->m_dwNumMeshVertices, pFrame->m_dwMeshFVF, 
                                   &pFrame->m_vMeshCenter, &pFrame->m_fMeshRadius );
        pFrame->m_pMeshVB->Unlock();

        D3DXVECTOR3 vMeshCenter = pFrame->m_vMeshCenter;
        D3DXVECTOR3 vMeshRadius = D3DXVECTOR3(pFrame->m_fMeshRadius,0,0);
        D3DXVec3TransformCoord( &vMeshCenter, &vMeshCenter, pmatWorld );
        D3DXVec3TransformCoord( &vMeshRadius, &vMeshRadius, pmatWorld );
        FLOAT fDistance = D3DXVec3Length( &vMeshCenter );
        FLOAT fRadius   = D3DXVec3Length( &vMeshRadius );
        pRoot->m_fRadius = max( pRoot->m_fRadius, fDistance + fRadius );
    }

    return TRUE;
}





//-----------------------------------------------------------------------------
// Name: LoadFromXBG()
// Desc: Loads a .xbg file
//-----------------------------------------------------------------------------
HRESULT CD3DFile::LoadFromXBG( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename )
{
    // Open the file
    FILE* file = fopen( strFilename, "rb" );
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

    // Read in header
    DWORD dwNumMeshes;  // Number of meshes in files
    DWORD dwSysMemSize; // Num bytes needed for system memory objects
    DWORD dwVidMemSize; // Num bytes needed for video memory objects

    fread( &dwNumMeshes,  1, sizeof(DWORD), file );
    fread( &dwSysMemSize, 1, sizeof(DWORD), file );
    fread( &dwVidMemSize, 1, sizeof(DWORD), file );

    // Read in system memory objects
    VOID* pSysMemData = (VOID*)new BYTE[dwSysMemSize];
    fread( pSysMemData, dwSysMemSize, 1, file );

    // Read in video memory objects
    VOID* pVidMemData = (VOID*)new BYTE[dwVidMemSize];
    fread( pVidMemData, dwVidMemSize, 1, file ); 
    
    // Done with the file
    fclose( file );

    // Do post-load, per-mesh processing
    XBMESH_FRAME* pFrameData = (XBMESH_FRAME*)pSysMemData;

    DWORD dwSysMemBaseAddr = (DWORD)pSysMemData;
    DWORD dwVidMemBaseAddr = (DWORD)pVidMemData;

    for( DWORD i=0; i<dwNumMeshes; i++ )
    {
        XBMESH_FRAME* pFrame = &pFrameData[i];
        XBMESH_DATA*  pMesh  = &pFrameData[i].m_MeshData;

        // Patch the mesh data. Any pointers read from the file were stored as
        // file offsets. So, we simply need to add a base address to patch
        // things up.
        if( pFrame->m_pChild )
            pFrame->m_pChild  = (XBMESH_FRAME*)( (DWORD)pFrame->m_pChild - 16 + dwSysMemBaseAddr );
        if( pFrame->m_pNext )
            pFrame->m_pNext   = (XBMESH_FRAME*)( (DWORD)pFrame->m_pNext - 16 + dwSysMemBaseAddr );
        if( pMesh->m_pSubsets )
            pMesh->m_pSubsets = (XBMESH_SUBSET*)( (DWORD)pMesh->m_pSubsets - 16 + dwSysMemBaseAddr );
        if( pMesh->m_dwNumIndices )
            pMesh->m_IB.Data  = pMesh->m_IB.Data -16 + dwSysMemBaseAddr;
        if( pMesh->m_dwNumVertices )
            pMesh->m_VB.Data  = pMesh->m_VB.Data + dwVidMemBaseAddr;

        // Create any textures used by the meshes' subsets.
        for( DWORD j = 0; j < pMesh->m_dwNumSubsets; j++ )
        {
            XBMESH_SUBSET* pSubset = &pMesh->m_pSubsets[j];
    
            if( pSubset->strTexture[0] )
            {
                D3DFile_CreateTexture( pd3dDevice, pSubset->strTexture, 
                                       &pSubset->pTexture );
            }
            else
                pSubset->pTexture = NULL;
        }
    }

    // In one fail swoop, create the frame hierarchy
    m_pChild = new CD3DFrame( pd3dDevice, this, pFrameData );

    // Cleanup allocated memory
    delete pSysMemData;
    delete pVidMemData;

    // After loading the file, walk the nodes to count primitives and
    // compute the bounding sphere    
    m_dwNumFrames   = 0L;
    m_dwNumMeshes   = 0L;
    m_dwNumVertices = 0L;
    m_dwNumIndices  = 0L;
    m_dwNumPolygons = 0L;
    m_vCenter       = D3DXVECTOR3(0,0,0);
    m_fRadius       = 0.0f;
    EnumFrames( PostLoadCB, this );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CD3DFrame()
// Desc: Construct a new CD3DFrame from the data in a XBMESH_FRAME structure.
//-----------------------------------------------------------------------------
CD3DFrame::CD3DFrame( LPDIRECT3DDEVICE8 pd3dDevice, CD3DFrame* pParent, 
                      XBMESH_FRAME* pFrameData )
{
    // Frame info
    strcpy( m_strFrameName, pFrameData->m_strName );
    m_pParent         = pParent;
    m_pChild          = NULL;
    m_pNext           = NULL;
    
    m_bVisible        = TRUE;
    m_lSelectedSubset = -1;
    m_hFVFTreeItem    = 0L;
    m_TreeData        = 0L;

    // Mesh info
    m_vMeshCenter          = D3DXVECTOR3(0,0,0);
    m_fMeshRadius          = 0L;
    m_dwNumMeshPolygons    = 0L;

    // Copy info from XBMESH_FRAME structure
    m_matTransform = pFrameData->m_matTransform;

    m_pMeshVB           = NULL;
    m_dwNumMeshVertices = pFrameData->m_MeshData.m_dwNumVertices;
    m_pMeshIB           = NULL;
    m_dwNumMeshIndices  = pFrameData->m_MeshData.m_dwNumIndices;
    m_dwMeshFVF         = pFrameData->m_MeshData.m_dwFVF;
    m_dwMeshVertexSize  = pFrameData->m_MeshData.m_dwVertexSize;
    m_dwNumMeshSubsets  = pFrameData->m_MeshData.m_dwNumSubsets;
    m_pMeshSubsets      = NULL;

    // Convet the primitive type
    if( pFrameData->m_MeshData.m_dwPrimType == (D3DPRIMITIVETYPE)5 )
        m_dwMeshPrimType = D3DPT_TRIANGLELIST;
    else
        m_dwMeshPrimType = D3DPT_TRIANGLESTRIP;

    // Compute num polygons
    if( m_dwMeshPrimType == D3DPT_TRIANGLELIST )
        m_dwNumMeshPolygons = m_dwNumMeshIndices / 3;
    else // if D3DPT_TRIANGLESTRIP
        m_dwNumMeshPolygons = m_dwNumMeshIndices - 2;

    // Create the vertex buffer
    if( m_dwNumMeshVertices )
    {
        pd3dDevice->CreateVertexBuffer( m_dwNumMeshVertices*m_dwMeshVertexSize, 
                                        D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, 
                                        &m_pMeshVB );
        VOID* pVertices;
        m_pMeshVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );
        memcpy( pVertices, (VOID*)pFrameData->m_MeshData.m_VB.Data, m_dwNumMeshVertices*m_dwMeshVertexSize );
        m_pMeshVB->Unlock();
    }

    // Create the index buffer
    if( m_dwNumMeshIndices )
    {
        pd3dDevice->CreateIndexBuffer( m_dwNumMeshIndices*sizeof(WORD), 
                                       D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, 
                                       &m_pMeshIB );
        WORD* pIndices;
        m_pMeshIB->Lock( 0, 0, (BYTE**)&pIndices, 0 );
        memcpy( pIndices, (VOID*)pFrameData->m_MeshData.m_IB.Data, m_dwNumMeshIndices*sizeof(WORD) );
        m_pMeshIB->Unlock();
    }

    // Create the subsets
    if( m_dwNumMeshSubsets )
    {
        m_pMeshSubsets = new XBMESH_SUBSET[m_dwNumMeshSubsets];
        memcpy( m_pMeshSubsets, pFrameData->m_MeshData.m_pSubsets, m_dwNumMeshSubsets*sizeof(XBMESH_SUBSET) );

    }

    // Create the child frame
    if( pFrameData->m_pChild )
        m_pChild = new CD3DFrame( pd3dDevice, this, pFrameData->m_pChild );

    // Create the sibling frame
    if( pFrameData->m_pNext )
        m_pNext = new CD3DFrame( pd3dDevice, pParent, pFrameData->m_pNext );
}


