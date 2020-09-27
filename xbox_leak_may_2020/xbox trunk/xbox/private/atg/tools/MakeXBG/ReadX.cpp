//-----------------------------------------------------------------------------
// File: ReadX.cpp
//
// Desc: Reads a .x file into a CD3DFile class (with a hierarchy of CD3DFrame
//       classes).
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
#include <dxfile.h>
#include <rmxfguid.h>
#include <rmxftmpl.h>
#include "xbg.h"




//-----------------------------------------------------------------------------
// Name: FindRadiusCB()
// Desc: Frame enumeration callback to compute the bounding sphere radius
//-----------------------------------------------------------------------------
BOOL FindRadiusCB( CD3DFrame* pFrame, D3DXMATRIX* pmatWorld, VOID* pData )
{
    CD3DFile* pRoot = (CD3DFile*)pData;

    // Tag the frame
    pFrame->m_dwEnumeratedID = pRoot->m_dwNumFrames;

    // Add one to the frame count
    pRoot->m_dwNumFrames++;
    
    if( pFrame->m_pMeshVB )
    {
        pRoot->m_dwNumMeshes++;
        pRoot->m_dwNumVertices += pFrame->m_dwNumMeshVertices;
        pRoot->m_dwNumIndices  += pFrame->m_dwNumMeshIndices;
        pRoot->m_dwNumPolygons += pFrame->m_dwNumMeshPolygons;

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
// Name: LoadCollapsedMeshFromX()
// Desc: Reads .x file and collapses all frames to one mesh
//-----------------------------------------------------------------------------
HRESULT CD3DFile::LoadCollapsedMeshFromX( LPDIRECT3DDEVICE8 pd3dDevice, 
                                          TCHAR* strFilename )
{
    // Create the frame
    CD3DFrame* pFrame = new CD3DFrame( _T("") );
    if( FAILED( pFrame->CreateMesh( pd3dDevice, strFilename ) ) )
    {
        delete pFrame;
        return E_FAIL;
    }

    // Add new frame to the current frame
    m_pChild = pFrame;
    pFrame->m_pParent = this;

    // After loading the file, walk the nodes to count primitives and
    // compute the bounding sphere    
    m_dwNumFrames   = 0L;
    m_dwNumMeshes   = 0L;
    m_dwNumVertices = 0L;
    m_dwNumIndices  = 0L;
    m_dwNumPolygons = 0L;
    m_vCenter       = D3DXVECTOR3(0,0,0);
    m_fRadius       = 0.0f;
    EnumFrames( FindRadiusCB, this );

    if( m_dwNumVertices == 0)
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadFromX()
// Desc: Reads .x file, retaining the frame hierarchy
//-----------------------------------------------------------------------------
HRESULT CD3DFile::LoadFromX( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename )
{
    LPDIRECTXFILE           pDXFile   = NULL;
    LPDIRECTXFILEENUMOBJECT pEnumObj  = NULL;
    LPDIRECTXFILEDATA       pFileData = NULL;
    HRESULT hr;

    // Set the name
    if( _tcsrchr( strFilename, '\\' ) )
        _tcscpy( m_strFrameName, _tcsrchr( strFilename, '\\' ) + 1 );
    else
        _tcscpy( m_strFrameName, strFilename );

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

    // Create enum object
    hr = pDXFile->CreateEnumObject( (VOID*)strFilename, DXFILELOAD_FROMFILE, 
                                    &pEnumObj );
    if( FAILED(hr) )
    {
        pDXFile->Release();
        return hr;
    }

    // Enumerate top level objects (which are always frames)
    while( SUCCEEDED( pEnumObj->GetNextDataObject( &pFileData ) ) )
    {
        hr = LoadFrame( pd3dDevice, pFileData, this );
        pFileData->Release();
        if( FAILED(hr) )
        {
            pEnumObj->Release();
            pDXFile->Release();
            return E_FAIL;
        }
    }

    if( pFileData ) pFileData->Release();
    if( pEnumObj )  pEnumObj->Release();
    if( pDXFile )   pDXFile->Release();

    // After loading the file, walk the nodes to count primitives and
    // compute the bounding sphere    
    m_dwNumFrames   = 0L;
    m_dwNumMeshes   = 0L;
    m_dwNumVertices = 0L;
    m_dwNumIndices  = 0L;
    m_dwNumPolygons = 0L;
    m_vCenter       = D3DXVECTOR3(0,0,0);
    m_fRadius       = 0.0f;
    EnumFrames( FindRadiusCB, this );

    if( m_dwNumVertices == 0)
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadFrame()
// Desc: Load a .x frame
//-----------------------------------------------------------------------------
HRESULT CD3DFile::LoadFrame( LPDIRECT3DDEVICE8 pd3dDevice,
                             LPDIRECTXFILEDATA pFileData,
                             CD3DFrame* pParentFrame )
{
    LPDIRECTXFILEDATA   pChildData = NULL;
    LPDIRECTXFILEOBJECT pChildObj = NULL;
    const GUID* pGUID;
    DWORD       cbSize;
    CD3DFrame*  pCurrentFrame;
    HRESULT     hr;

    // Get the type of the object
    if( FAILED( hr = pFileData->GetType( &pGUID ) ) )
        return hr;

    if( *pGUID == TID_D3DRMMesh )
    {
        // Get the mesh name
        TCHAR strName[MAX_PATH] = _T("");
        DWORD dwNameLength;
        pFileData->GetName( NULL, &dwNameLength );
        if( dwNameLength > 0 )
            pFileData->GetName( strName, &dwNameLength );

        // Create a new frame for the mesh
        CD3DFrame* pNewFrame = new CD3DFrame( strName );
        pNewFrame->m_pParent   = pParentFrame;
        pNewFrame->m_pNext     = pParentFrame->m_pChild;
        pParentFrame->m_pChild = pNewFrame;

        hr = pNewFrame->CreateMesh( pd3dDevice, pFileData );
        if( FAILED(hr) )
            return hr;
    }
    if( *pGUID == TID_D3DRMFrameTransformMatrix )
    {
        D3DXMATRIX* pmatMatrix;
        hr = pFileData->GetData( NULL, &cbSize, (VOID**)&pmatMatrix );
        if( FAILED(hr) )
            return hr;

        // Update the parent's matrix with the new one
        pParentFrame->m_matTransform = (*pmatMatrix);
    }
    if( *pGUID == TID_D3DRMFrame )
    {
        // Get the frame name
        TCHAR strName[MAX_PATH] = _T("");
        DWORD dwNameLength;
        pFileData->GetName( NULL, &dwNameLength );
        if( dwNameLength > 0 )
            pFileData->GetName( strName, &dwNameLength );

        // Create the frame
        pCurrentFrame = new CD3DFrame( strName );
        pCurrentFrame->m_pParent = pParentFrame;
        pCurrentFrame->m_pNext   = pParentFrame->m_pChild;
        pParentFrame->m_pChild   = pCurrentFrame;

        // Enumerate child objects
        while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
        {
            // Query the child for its FileData
            hr = pChildObj->QueryInterface( IID_IDirectXFileData,
                                            (VOID**)&pChildData );
            if( SUCCEEDED(hr) )
            {
                hr = LoadFrame( pd3dDevice, pChildData, pCurrentFrame );
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
// Name: CreateMesh()
// Desc: Creates a mesh object from within a frame
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::CreateMesh( LPDIRECT3DDEVICE8 pd3dDevice,
                               LPDIRECTXFILEDATA pFileData )
{
    LPD3DXMESH   pSysMemMesh      = NULL;
    LPD3DXMESH   pLocalMemMesh    = NULL;
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    LPD3DXBUFFER pMtrlBuffer      = NULL;
    DWORD        dwNumMaterials;
    HRESULT      hr;

    // Use D3DX to load the mesh from the DXFILEDATA object
    if( FAILED( hr = D3DXLoadMeshFromXof( pFileData, D3DXMESH_SYSTEMMEM, pd3dDevice,
                                          &pAdjacencyBuffer, &pMtrlBuffer, 
                                          &dwNumMaterials, &pSysMemMesh ) ) )
        return hr;

    // Attribute sort the mesh
    DWORD* rgdwAdjacencyTemp = new DWORD[ pSysMemMesh->GetNumFaces() * 3 ];
    pSysMemMesh->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT,
                                (DWORD*)pAdjacencyBuffer->GetBufferPointer(),
                                rgdwAdjacencyTemp, NULL, NULL );
    delete[] rgdwAdjacencyTemp;

    // Make a local memory version of the mesh. Note: because we are passing in
    // no flags, the default behavior is to clone into local memory.
    if( FAILED( pSysMemMesh->CloneMeshFVF( D3DXMESH_MANAGED, pSysMemMesh->GetFVF(),
                                           pd3dDevice, &pLocalMemMesh ) ) )
        return E_FAIL;

    // We're done with the system memory mesh
    pSysMemMesh->Release();

    //-----------------------------------------------------------------------------
    // Now, convert the objects into a form better suited for our XBG file
    //-----------------------------------------------------------------------------

    // Store mesh properties
    pLocalMemMesh->GetVertexBuffer( &m_pMeshVB );
    pLocalMemMesh->GetIndexBuffer( &m_pMeshIB );
    m_dwNumMeshVertices = pLocalMemMesh->GetNumVertices();
    m_dwNumMeshIndices  = pLocalMemMesh->GetNumFaces() * 3;
    m_dwNumMeshPolygons = pLocalMemMesh->GetNumFaces();
    m_dwMeshFVF         = pLocalMemMesh->GetFVF();
    m_dwMeshVertexSize  = D3DXGetFVFVertexSize( m_dwMeshFVF );
    m_dwMeshPrimType    = D3DPT_TRIANGLELIST;

    // Compute the object center and radius
    BYTE* pVertices;
    m_pMeshVB->Lock( 0, 0, &pVertices, D3DLOCK_NOSYSLOCK );
    D3DXComputeBoundingSphere( pVertices, m_dwNumMeshVertices, m_dwMeshFVF,
                               &m_vMeshCenter, &m_fMeshRadius );
    m_pMeshVB->Unlock();

    // Build the array of subsets, used to render the mesh
    pLocalMemMesh->GetAttributeTable( NULL, &m_dwNumMeshSubsets );
    m_pMeshSubsets  = new XBMESH_SUBSET[ max(1,m_dwNumMeshSubsets) ];
    
    if( m_dwNumMeshSubsets > 0 )
    {
        // Get subset info from mesh's attribute table
        D3DXATTRIBUTERANGE* pAttributes;
        pAttributes = new D3DXATTRIBUTERANGE[ max(1,m_dwNumMeshSubsets) ];
        pLocalMemMesh->GetAttributeTable( pAttributes, NULL );
        for( DWORD i = 0; i < m_dwNumMeshSubsets; i++ )
        {
            m_pMeshSubsets[i].dwVertexStart = pAttributes[i].VertexStart;
            m_pMeshSubsets[i].dwVertexCount = pAttributes[i].VertexCount;
            m_pMeshSubsets[i].dwIndexStart  = pAttributes[i].FaceStart*3;
            m_pMeshSubsets[i].dwIndexCount  = pAttributes[i].FaceCount*3;
        }

        delete pAttributes;
    }
    else
    {
        // Set default subset
        m_dwNumMeshSubsets = 1; 
        m_pMeshSubsets[0].dwVertexStart = 0;
        m_pMeshSubsets[0].dwVertexCount = m_dwNumMeshVertices;
        m_pMeshSubsets[0].dwIndexStart  = 0;
        m_pMeshSubsets[0].dwIndexCount  = m_dwNumMeshIndices;
    }

    // Initialize materials and textures
    for( DWORD i=0; i<m_dwNumMeshSubsets; i++ )
    {
        m_pMeshSubsets[i].strTexture[0] = 0;
        m_pMeshSubsets[i].pTexture      = NULL;

        D3DMATERIAL8* pmtrl = &m_pMeshSubsets[i].mtrl;
        ZeroMemory( pmtrl, sizeof(D3DMATERIAL8) );
        pmtrl->Diffuse.r = pmtrl->Diffuse.g = pmtrl->Diffuse.b = pmtrl->Diffuse.a = 1.0f;
        pmtrl->Ambient.r = pmtrl->Ambient.g = pmtrl->Ambient.b = pmtrl->Ambient.a = 1.0f;
    }

    // Create materials and textures for the mesh
    if( pMtrlBuffer && m_dwNumMeshSubsets > 0 )
    {
        // Allocate memory for the materials and textures
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();

        // Copy each material and create it's texture
        for( DWORD i=0; i<m_dwNumMeshSubsets; i++ )
        {
            // Copy the material
            m_pMeshSubsets[i].mtrl = d3dxMtrls[i].MatD3D;
            if( m_pMeshSubsets[i].mtrl.Diffuse.a == 0.0f )
                m_pMeshSubsets[i].mtrl.Diffuse.a = 1.0f;
            m_pMeshSubsets[i].mtrl.Ambient = m_pMeshSubsets[i].mtrl.Diffuse;
            m_pMeshSubsets[i].pTexture     = NULL;

            // Create a texture
            if( d3dxMtrls[i].pTextureFilename )
            {
				if( strlen( d3dxMtrls[i].pTextureFilename ) < sizeof(m_pMeshSubsets[i].strTexture) )
					strcpy( m_pMeshSubsets[i].strTexture, d3dxMtrls[i].pTextureFilename );
				else
					strcpy( m_pMeshSubsets[i].strTexture, strrchr( d3dxMtrls[i].pTextureFilename, '\\' ) );

                // Load the texture
                if( FAILED( D3DFile_CreateTexture( pd3dDevice, m_pMeshSubsets[i].strTexture, 
                                                   &m_pMeshSubsets[i].pTexture ) ) )
				{
					// If the load failed, prompt the user for a path
					strcpy( m_pMeshSubsets[i].strTexture, "Woman.dds" );

					D3DFile_CreateTexture( pd3dDevice, m_pMeshSubsets[i].strTexture, 
                                           &m_pMeshSubsets[i].pTexture );
				}
            }
        }
    }

    // Release objects
    if( pMtrlBuffer )
        pMtrlBuffer->Release();
    if( pAdjacencyBuffer ) 
        pAdjacencyBuffer->Release();
    if( pLocalMemMesh )
        pLocalMemMesh->Release();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateMesh()
// Desc: Creates a mesh object from within a frame
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::CreateMesh( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename )
{
    LPD3DXMESH   pSysMemMesh      = NULL;
    LPD3DXMESH   pLocalMemMesh    = NULL;
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    LPD3DXBUFFER pMtrlBuffer      = NULL;
    DWORD        dwNumMaterials;
    HRESULT      hr;

    // Use D3DX to load the mesh from a .x file
    if( FAILED( hr = D3DXLoadMeshFromX( strFilename, D3DXMESH_SYSTEMMEM, 
                                        pd3dDevice, &pAdjacencyBuffer, &pMtrlBuffer, 
                                        &dwNumMaterials, &pSysMemMesh ) ) )
        return hr;

    // Attribute sort the mesh
    DWORD* rgdwAdjacencyTemp = new DWORD[ pSysMemMesh->GetNumFaces() * 3 ];
    pSysMemMesh->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT,
                                (DWORD*)pAdjacencyBuffer->GetBufferPointer(),
                                rgdwAdjacencyTemp, NULL, NULL );
    delete[] rgdwAdjacencyTemp;

    // Make a local memory version of the mesh. Note: because we are passing in
    // no flags, the default behavior is to clone into local memory.
    if( FAILED( pSysMemMesh->CloneMeshFVF( D3DXMESH_MANAGED, pSysMemMesh->GetFVF(),
                                           pd3dDevice, &pLocalMemMesh ) ) )
        return E_FAIL;

    // We're done with the system memory mesh
    pSysMemMesh->Release();

    //-----------------------------------------------------------------------------
    // Now, convert the objects into a form better suited for our XBG file
    //-----------------------------------------------------------------------------

    // Store mesh properties
    pLocalMemMesh->GetVertexBuffer( &m_pMeshVB );
    pLocalMemMesh->GetIndexBuffer( &m_pMeshIB );
    m_dwNumMeshVertices = pLocalMemMesh->GetNumVertices();
    m_dwNumMeshIndices  = pLocalMemMesh->GetNumFaces() * 3;
    m_dwNumMeshPolygons = pLocalMemMesh->GetNumFaces();
    m_dwMeshFVF         = pLocalMemMesh->GetFVF();
    m_dwMeshVertexSize  = D3DXGetFVFVertexSize( m_dwMeshFVF );
    m_dwMeshPrimType    = D3DPT_TRIANGLELIST;

    // Compute the object center and radius
    BYTE* pVertices;
    m_pMeshVB->Lock( 0, 0, &pVertices, D3DLOCK_NOSYSLOCK );
    D3DXComputeBoundingSphere( pVertices, m_dwNumMeshVertices, m_dwMeshFVF,
                               &m_vMeshCenter, &m_fMeshRadius );
    m_pMeshVB->Unlock();

    // Build the array of subsets, used to render the mesh
    pLocalMemMesh->GetAttributeTable( NULL, &m_dwNumMeshSubsets );
    m_pMeshSubsets  = new XBMESH_SUBSET[ max(1,m_dwNumMeshSubsets) ];
    
    if( m_dwNumMeshSubsets > 0 )
    {
        // Get subset info from mesh's attribute table
        D3DXATTRIBUTERANGE* pAttributes;
        pAttributes = new D3DXATTRIBUTERANGE[ max(1,m_dwNumMeshSubsets) ];
        pLocalMemMesh->GetAttributeTable( pAttributes, NULL );
        for( DWORD i = 0; i < m_dwNumMeshSubsets; i++ )
        {
            m_pMeshSubsets[i].dwVertexStart = pAttributes[i].VertexStart;
            m_pMeshSubsets[i].dwVertexCount = pAttributes[i].VertexCount;
            m_pMeshSubsets[i].dwIndexStart  = pAttributes[i].FaceStart*3;
            m_pMeshSubsets[i].dwIndexCount  = pAttributes[i].FaceCount*3;
        }

        delete pAttributes;
    }
    else
    {
        // Set default subset
        m_dwNumMeshSubsets = 1; 
        m_pMeshSubsets[0].dwVertexStart = 0;
        m_pMeshSubsets[0].dwVertexCount = m_dwNumMeshVertices;
        m_pMeshSubsets[0].dwIndexStart  = 0;
        m_pMeshSubsets[0].dwIndexCount  = m_dwNumMeshIndices;
    }

    // Initialize materials and textures
    for( DWORD i=0; i<m_dwNumMeshSubsets; i++ )
    {
        m_pMeshSubsets[i].strTexture[0] = 0;
        m_pMeshSubsets[i].pTexture      = NULL;

        D3DMATERIAL8* pmtrl = &m_pMeshSubsets[i].mtrl;
        ZeroMemory( pmtrl, sizeof(D3DMATERIAL8) );
        pmtrl->Diffuse.r = pmtrl->Diffuse.g = pmtrl->Diffuse.b = pmtrl->Diffuse.a = 1.0f;
        pmtrl->Ambient.r = pmtrl->Ambient.g = pmtrl->Ambient.b = pmtrl->Ambient.a = 1.0f;
    }

    // Create materials and textures for the mesh
    if( pMtrlBuffer && m_dwNumMeshSubsets > 0 )
    {
        // Allocate memory for the materials and textures
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();

        // Copy each material and create it's texture
        for( DWORD i=0; i<m_dwNumMeshSubsets; i++ )
        {
            // Copy the material
            m_pMeshSubsets[i].mtrl = d3dxMtrls[i].MatD3D;
            if( m_pMeshSubsets[i].mtrl.Diffuse.a == 0.0f )
                m_pMeshSubsets[i].mtrl.Diffuse.a = 1.0f;
            m_pMeshSubsets[i].mtrl.Ambient = m_pMeshSubsets[i].mtrl.Diffuse;
            m_pMeshSubsets[i].pTexture     = NULL;

            // Create a texture
            if( d3dxMtrls[i].pTextureFilename )
            {
				if( strlen( d3dxMtrls[i].pTextureFilename ) < sizeof(m_pMeshSubsets[i].strTexture) )
					strcpy( m_pMeshSubsets[i].strTexture, d3dxMtrls[i].pTextureFilename );
				else
					strcpy( m_pMeshSubsets[i].strTexture, strrchr( d3dxMtrls[i].pTextureFilename, '\\' ) );

                // Load the texture
                if( FAILED( D3DFile_CreateTexture( pd3dDevice, m_pMeshSubsets[i].strTexture, 
                                                   &m_pMeshSubsets[i].pTexture ) ) )
				{
					// If the load failed, prompt the user for a path
					strcpy( m_pMeshSubsets[i].strTexture, "Woman.dds" );

					D3DFile_CreateTexture( pd3dDevice, m_pMeshSubsets[i].strTexture, 
                                           &m_pMeshSubsets[i].pTexture );
				}
            }
        }
    }

    // Release objects
    if( pMtrlBuffer )
        pMtrlBuffer->Release();
    if( pAdjacencyBuffer ) 
        pAdjacencyBuffer->Release();
    if( pLocalMemMesh )
        pLocalMemMesh->Release();

    return S_OK;
}




