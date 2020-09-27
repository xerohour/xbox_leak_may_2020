//-----------------------------------------------------------------------------
// File: D3DFile.cpp
//
// Desc: Code to manage file-based geoemtry models. These models are read in
//       by the code in ReadX.cpp and/or ReadXBG.cpp and written out by the
//       code in WriteXBG.cpp.
//
//       See the XBG.h header file for a better description of .xbg geometry
//       files.
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
#include "XBG.h"
#include "TriStripper.h"
#include "FVF.h"




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool g_bVisualizeStrips = false;


D3DMATERIAL8 g_VisualizationColors[6] =
{
	{
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		0.0f,
	},

	{
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		0.0f,
	},

	{
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		0.0f,
	},

	{
		{ 0.0f, 1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		0.0f,
	},

	{
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		0.0f,
	},

	{
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		0.0f,
	},
};




//-----------------------------------------------------------------------------
// Name: D3DFile_CreateTexture()
// Desc: Helper function to load a texture file
//-----------------------------------------------------------------------------
HRESULT D3DFile_CreateTexture( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strTexture,
                               LPDIRECT3DTEXTURE8* ppTexture )
{

    // Check the current directory
    if( SUCCEEDED( D3DXCreateTextureFromFile( pd3dDevice, strTexture, 
                                              ppTexture ) ) )
        return S_OK;

    // Check the "..\Textures" directory
    TCHAR strFilename[80];
    _stprintf( strFilename, "..\\Textures\\%s", strTexture );
    if( SUCCEEDED( D3DXCreateTextureFromFile( pd3dDevice, strFilename, 
                                              ppTexture ) ) )
        return S_OK;

    (*ppTexture) = NULL;
    return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: ComputeNormals()
// Desc: Compute the normals for the mesh
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::ComputeNormals()
{
    LPDIRECT3DVERTEXBUFFER8 pVB    = m_pMeshVB;
    LPDIRECT3DINDEXBUFFER8  pIB    = m_pMeshIB;
    DWORD            dwNumVertices = m_dwNumMeshVertices;
    DWORD            dwNumIndices  = m_dwNumMeshIndices;
    DWORD            dwFVF         = m_dwMeshFVF;
    DWORD            dwVertexSize  = m_dwMeshVertexSize;
    D3DPRIMITIVETYPE dwPrimType    = m_dwMeshPrimType;
    DWORD            dwNumSubsets  = m_dwNumMeshSubsets;
    XBMESH_SUBSET*   pSubsets      = m_pMeshSubsets;

    // Make sure the vertices have normals
    if( 0 == ( dwFVF & D3DFVF_NORMAL ) )
        return E_FAIL;

    // Compute the vertices' byte offset to their normals
    DWORD dwNormalOffset = 12;
    DWORD dwPositionFVF  = dwFVF & D3DFVF_POSITION_MASK;
    if( dwPositionFVF >= D3DFVF_XYZB1 )
        dwNormalOffset += 4*((dwPositionFVF-4)/2);

    // Gain access to vertices and faces
    BYTE* pVertices;
    WORD* pIndices;
    pIB->Lock( 0, 0, (BYTE**)&pIndices,  0 );
    pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    // Zero out the existing normals
    for( DWORD vtx = 0; vtx < dwNumVertices; vtx++ )
    {
        BYTE*        pVertex = ( (BYTE*)pVertices + (vtx * dwVertexSize) );
        D3DXVECTOR3* pNormal = (D3DXVECTOR3*)( pVertex + dwNormalOffset );
        (*pNormal) = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    }

	DWORD dwNumFaces = dwPrimType==D3DPT_TRIANGLELIST ? dwNumIndices/3: dwNumIndices-2;

    // Add in face normals
    for( DWORD face = 0; face < dwNumFaces; face++ )
    {
        WORD wFaceVtx[3];
		if( dwPrimType==D3DPT_TRIANGLELIST )
		{
	        wFaceVtx[0] = pIndices[ 3*face+0 ];
		    wFaceVtx[1] = pIndices[ 3*face+1 ];
			wFaceVtx[2] = pIndices[ 3*face+2 ];
		}
		else
		{
			if( 0 == face%2 )
			{
		        wFaceVtx[0] = pIndices[ face+0 ];
			    wFaceVtx[1] = pIndices[ face+1 ];
				wFaceVtx[2] = pIndices[ face+2 ];
			}
			else
			{
		        wFaceVtx[0] = pIndices[ face+1 ];
			    wFaceVtx[1] = pIndices[ face+0 ];
				wFaceVtx[2] = pIndices[ face+2 ];
			}
		}

        D3DXVECTOR3* pPos0 = (D3DXVECTOR3*)( pVertices + (wFaceVtx[0] * dwVertexSize) );
        D3DXVECTOR3* pPos1 = (D3DXVECTOR3*)( pVertices + (wFaceVtx[1] * dwVertexSize) );
        D3DXVECTOR3* pPos2 = (D3DXVECTOR3*)( pVertices + (wFaceVtx[2] * dwVertexSize) );

        // Calculate the normal of the face from the two edge vectors
        D3DXVECTOR3  vNormal;
        D3DXVECTOR3  vEdge1 = *pPos0 - *pPos1;
        D3DXVECTOR3  vEdge2 = *pPos0 - *pPos2;
        D3DXVec3Cross( &vNormal, &vEdge1, &vEdge2 );

        for( DWORD point = 0; point < 3; point++ )
        {
            BYTE*        pVertex = ( pVertices + (wFaceVtx[point] * dwVertexSize) );
            D3DXVECTOR3* pNormal = (D3DXVECTOR3*)( pVertex + dwNormalOffset );
            (*pNormal) += vNormal;
        }
    }

    // Renormalize all normals
    for( vtx = 0; vtx < dwNumVertices; vtx++ )
    {
        BYTE*        pVertex = ( pVertices + (vtx * dwVertexSize) );
        D3DXVECTOR3* pNormal = (D3DXVECTOR3*)( pVertex + dwNormalOffset );
        D3DXVec3Normalize( pNormal, pNormal );
    }

    pIB->Unlock();
    pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetMeshFVF()
// Desc: Changes the FVF of the mesh
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::SetMeshFVF( DWORD dwFVF )
{
    if( 0 == dwFVF )
        return E_FAIL;

    LPDIRECT3DVERTEXBUFFER8 pOldVB = m_pMeshVB;
    LPDIRECT3DVERTEXBUFFER8 pNewVB = NULL;
    DWORD dwNumVertices = m_dwNumMeshVertices;
    DWORD dwOldFVF      = m_dwMeshFVF;
    DWORD dwNewFVF      = dwFVF;

    if( pOldVB && dwNewFVF != dwOldFVF )
    {
        // Create a copy of the vertex buffer, using the new FVF
        if( FAILED( CopyVertexBufferWithNewFVF( pOldVB, dwNumVertices,
                                                dwOldFVF, dwNewFVF, &pNewVB ) ) )
            return E_FAIL;

        // Assign the new vertex buffer
        m_pMeshVB->Release();
        m_pMeshVB          = pNewVB;
        m_dwMeshFVF        = dwNewFVF;
        m_dwMeshVertexSize = D3DXGetFVFVertexSize( dwNewFVF );

        // Compute normals if necessary
        if( ( dwNewFVF & D3DFVF_NORMAL ) && ( 0 == ( dwOldFVF & D3DFVF_NORMAL ) ) )
            ComputeNormals();
    }

    // Set the Mesh FVF for any child and sibling frames
    if( m_pChild ) m_pChild->SetMeshFVF( dwFVF );
    if( m_pNext )  m_pNext->SetMeshFVF( dwFVF );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CD3DFrame()
// Desc: Constructor
//-----------------------------------------------------------------------------
CD3DFrame::CD3DFrame( TCHAR* strName )
{
    // Frame info
    _tcscpy( m_strFrameName, strName );
    D3DXMatrixIdentity( &m_matTransform );

    m_pParent = NULL;
    m_pChild  = NULL;
    m_pNext   = NULL;

    m_bVisible        = TRUE;
    m_lSelectedSubset = -1;
    m_hFVFTreeItem    = 0L;
    m_TreeData        = 0L;

    // Mesh info
    m_pMeshVB              = NULL;
    m_pMeshIB              = NULL;
    m_pMeshSubsets         = NULL;

    m_vMeshCenter          = D3DXVECTOR3(0,0,0);
    m_fMeshRadius          = 0L;
    m_dwNumMeshVertices    = 0L;
    m_dwNumMeshIndices     = 0L;
    m_dwNumMeshPolygons    = 0L;
    m_dwMeshFVF            = 0L;
    m_dwMeshVertexSize     = 0L;
    m_dwMeshPrimType       = D3DPT_TRIANGLELIST;
    m_dwNumMeshSubsets     = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~CD3DFrame()
// Desc: Destructor
//-----------------------------------------------------------------------------
CD3DFrame::~CD3DFrame()
{
    Destroy();
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the mesh
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::Destroy()
{
    for( UINT i=0; i<m_dwNumMeshSubsets; i++ )
        if( m_pMeshSubsets[i].pTexture )
            m_pMeshSubsets[i].pTexture->Release();

    if( m_pMeshVB )          m_pMeshVB->Release();
    if( m_pMeshIB )          m_pMeshIB->Release();

    if( m_pMeshSubsets )
        delete[] m_pMeshSubsets;
    m_dwNumMeshSubsets = 0L;

    m_pMeshVB              = NULL;
    m_pMeshIB              = NULL;
    m_pMeshSubsets         = NULL;

    if( m_pChild ) delete m_pChild;
    if( m_pNext )  delete m_pNext;
    m_pNext  = NULL;
    m_pChild = NULL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CD3DFile()
// Desc: Constructor
//-----------------------------------------------------------------------------
CD3DFile::CD3DFile() 
         :CD3DFrame( _T("CD3DFile_Root") )
{
    m_dwNumFrames   = 0L;
    m_dwNumMeshes   = 0L;
    m_dwNumVertices = 0L;
    m_dwNumIndices  = 0L;
    m_dwNumPolygons = 0L;
    m_vCenter       = D3DXVECTOR3(0,0,0);
    m_fRadius       = 0.0f;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates a mesh from a file. The bCollapseMesh parameter is for loading
//       .x files. If true, the geometry will be collapsed to one mesh. If
//       false, the frame hierarchy (if any) will be retained.
//-----------------------------------------------------------------------------
HRESULT CD3DFile::Create( LPDIRECT3DDEVICE8 pd3dDevice, TCHAR* strFilename,
                          BOOL bCollapseMesh )
{
    if( bCollapseMesh )
    {
        // Try loading .x file, collapsing all geometry to one mesh
        if( SUCCEEDED( LoadCollapsedMeshFromX( pd3dDevice, strFilename ) ) )
            return S_OK;
    }
    else
    {
        // Try loading .x file, retaining the frame hierarchy
        if( SUCCEEDED( LoadFromX( pd3dDevice, strFilename ) ) )
            return S_OK;
    }

    // Try loading .xbg file
    if( SUCCEEDED( LoadFromXBG( pd3dDevice, strFilename ) ) )
        return S_OK;


    return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: EnumFrames()
// Desc: Called recursively to walk the frame hierarchy, calling a user 
//       supplied callback function for every frame.
//-----------------------------------------------------------------------------
BOOL CD3DFrame::EnumFrames( BOOL (*EnumFramesCB)(CD3DFrame*,VOID*),
                            VOID* pData )
{
    EnumFramesCB( this, pData );

    if( m_pChild )
        m_pChild->EnumFrames( EnumFramesCB, pData );
    
    if( m_pNext )
        m_pNext->EnumFrames( EnumFramesCB, pData );

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: EnumFrames()
// Desc: Called recursively to walk the frame hierarchy, calling a user 
//       supplied callback function for every frame.
//-----------------------------------------------------------------------------
BOOL CD3DFrame::EnumFrames( BOOL (*EnumFramesCB)(CD3DFrame*,D3DXMATRIX*,VOID*),
                            VOID* pData )
{
    static D3DXMATRIX matWorld(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

    D3DXMATRIX matSavedWorld = matWorld;
    D3DXMatrixMultiply( &matWorld, &m_matTransform, &matSavedWorld );
    
    EnumFramesCB( this, &matWorld, pData );

    if( m_pChild )
        m_pChild->EnumFrames( EnumFramesCB, pData );
    
    matWorld = matSavedWorld;

    if( m_pNext )
        m_pNext->EnumFrames( EnumFramesCB, pData );

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: RenderMeshSubsets()
// Desc: Renders the mesh geometry for the frame.
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::RenderMeshSubsets( LPDIRECT3DDEVICE8 pd3dDevice, 
                                      DWORD dwFirstSubset, DWORD dwLastSubset,
                                      DWORD dwFlags )
{
    if( NULL == m_pMeshVB )
        return E_FAIL;

    pd3dDevice->SetVertexShader( m_dwMeshFVF);
    pd3dDevice->SetStreamSource( 0, m_pMeshVB, m_dwMeshVertexSize );
    pd3dDevice->SetIndices( m_pMeshIB, 0 );

    // First, draw the subsets without alpha
    if( dwFlags & D3DFILE_RENDEROPAQUESUBSETS )
    {
        for( DWORD i=dwFirstSubset; i<=dwLastSubset; i++ )
        {
            if( m_pMeshSubsets[i].mtrl.Diffuse.a < 1.0f )
                continue;

			if ( D3DPT_TRIANGLESTRIP == m_dwMeshPrimType && g_bVisualizeStrips )
			{
				int iVisIndex = 0;

				UINT uIndexStart = m_pMeshSubsets[i].dwIndexStart;
				UINT uIndexEnd;

				UINT uIndexBufferEnd = m_pMeshSubsets[i].dwIndexStart + m_pMeshSubsets[i].dwIndexCount;

				WORD* pIndices = 0;
				UINT uIndexBufferSize = sizeof(WORD) * m_pMeshSubsets[i].dwIndexCount;
				m_pMeshIB->Lock(0, uIndexBufferSize, (BYTE**)&pIndices, D3DLOCK_READONLY);

				while (uIndexStart < uIndexBufferEnd)
				{
					// Find end of strip.
					for (uIndexEnd = uIndexStart+2; uIndexEnd < uIndexBufferEnd-1; uIndexEnd++)
					{
						// Check for duplicate verts.
						if (pIndices[uIndexEnd+1] == pIndices[uIndexEnd])
							break;
					}

					// Draw strip.
					pd3dDevice->SetMaterial( &g_VisualizationColors[iVisIndex++] );
					if (iVisIndex >= 6) iVisIndex = 0;

					pd3dDevice->DrawIndexedPrimitive( m_dwMeshPrimType, 0, m_dwNumMeshVertices, 
					                                  uIndexStart, uIndexEnd-uIndexStart-1 );

					if ((uIndexEnd-uIndexStart-1) & 1)
						uIndexStart = uIndexEnd+2;
					else
						uIndexStart = uIndexEnd+3;
				}

				m_pMeshIB->Unlock();
			}
			else
			{
				pd3dDevice->SetMaterial( &m_pMeshSubsets[i].mtrl );
				pd3dDevice->SetTexture( 0, m_pMeshSubsets[i].pTexture );

				DWORD dwNumPrimitives = m_pMeshSubsets[i].dwIndexCount/3;
				if( D3DPT_TRIANGLESTRIP == m_dwMeshPrimType ) 
					dwNumPrimitives = m_pMeshSubsets[i].dwIndexCount-2;

				pd3dDevice->DrawIndexedPrimitive( m_dwMeshPrimType, 0, m_dwNumMeshVertices,
												  m_pMeshSubsets[i].dwIndexStart, dwNumPrimitives );
			}
        }
    }

    // Then, draw the subsets with alpha
    if( dwFlags & D3DFILE_RENDERALPHASUBSETS )
    {
        for( DWORD i=dwFirstSubset; i<=dwLastSubset; i++ )
        {
            if( m_pMeshSubsets[i].mtrl.Diffuse.a >= 1.0f )
                continue;

			if ( D3DPT_TRIANGLESTRIP == m_dwMeshPrimType && g_bVisualizeStrips )
			{
				int iVisIndex = 0;

				UINT uIndexStart = m_pMeshSubsets[i].dwIndexStart;
				UINT uIndexEnd;

				UINT uIndexBufferEnd = m_pMeshSubsets[i].dwIndexStart + m_pMeshSubsets[i].dwIndexCount;

				WORD* pIndices = 0;
				UINT uIndexBufferSize = sizeof(WORD) * m_pMeshSubsets[i].dwIndexCount;
				m_pMeshIB->Lock(0, uIndexBufferSize, (BYTE**)&pIndices, D3DLOCK_READONLY);

				while (uIndexStart < uIndexBufferEnd)
				{
					// Find end of strip.
					for (uIndexEnd = uIndexStart+2; uIndexEnd < uIndexBufferEnd-1; uIndexEnd++)
					{
						// Check for duplicate verts.
						if (pIndices[uIndexEnd+1] == pIndices[uIndexEnd])
							break;
					}

					// Draw strip.
					pd3dDevice->SetMaterial( &g_VisualizationColors[iVisIndex++] );
					if (iVisIndex >= 6) iVisIndex = 0;

					pd3dDevice->DrawIndexedPrimitive( m_dwMeshPrimType, 0, m_dwNumMeshVertices, 
					                                  uIndexStart, uIndexEnd-uIndexStart-1 );

					if ((uIndexEnd-uIndexStart-1) & 1)
						uIndexStart = uIndexEnd+2;
					else
						uIndexStart = uIndexEnd+3;
				}

				m_pMeshIB->Unlock();
			}
			else
			{
				pd3dDevice->SetMaterial( &m_pMeshSubsets[i].mtrl );
				pd3dDevice->SetTexture( 0, m_pMeshSubsets[i].pTexture );

				DWORD dwNumPrimitives = m_pMeshSubsets[i].dwIndexCount/3;
				if( D3DPT_TRIANGLESTRIP == m_dwMeshPrimType ) 
					dwNumPrimitives = m_pMeshSubsets[i].dwIndexCount-2;
           
				pd3dDevice->DrawIndexedPrimitive( m_dwMeshPrimType, 0, m_dwNumMeshVertices,
												  m_pMeshSubsets[i].dwIndexStart, dwNumPrimitives );
			}
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the current frame's mesh. This function is called recursively
//       to render the entire frame hierarchy.
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::Render( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwFlags )
{
    // Apply the matrix transform for this frame
    D3DXMATRIX matSavedWorld, matWorld;
    pd3dDevice->GetTransform( D3DTS_WORLD, &matSavedWorld );
    D3DXMatrixMultiply( &matWorld, &m_matTransform, &matSavedWorld );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Render the mesh subsets
    if( m_dwNumMeshSubsets )
    {
        // Render the mesh. This hubbub is simply to allow different rendering
        // options, specifically allowing to highlight "visible" frames and a
        // "selected" subset.
        if( ( TRUE == m_bVisible ) && ( dwFlags & D3DFILE_RENDERVISIBLEFRAMES ) )
        {
            if( m_lSelectedSubset != -1 )
                RenderMeshSubsets( pd3dDevice, m_lSelectedSubset, m_lSelectedSubset, dwFlags );
            else
                RenderMeshSubsets( pd3dDevice, 0, m_dwNumMeshSubsets-1, dwFlags );
        }
        if( dwFlags & D3DFILE_RENDERALLFRAMES )
        {
            RenderMeshSubsets( pd3dDevice, 0, m_dwNumMeshSubsets-1, dwFlags );
        }
    }

    // Render any child frames
    if( m_pChild )
        m_pChild->Render( pd3dDevice, dwFlags );

    // Restore the matrix transform
    pd3dDevice->SetTransform( D3DTS_WORLD, &matSavedWorld );

    // Render any sibling frames
    if( m_pNext )
        m_pNext->Render( pd3dDevice, dwFlags );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Unstrip()
// Desc: Turn strips back into triangle a triangle list, removing degenerates.
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::Unstrip()
{
    if( D3DPT_TRIANGLESTRIP != m_dwMeshPrimType ) 
	{
		return E_FAIL;
	}

    // Get a d3d device (needed for creating buffers)
    LPDIRECT3DDEVICE8 pd3dDevice;
    m_pMeshVB->GetDevice( &pd3dDevice );
    pd3dDevice->Release();

    // Gain access to the vertices and indices for the mesh
    BYTE* pSrcVertices;
    WORD* pSrcIndices;
    m_pMeshVB->Lock( 0, 0, (BYTE**)&pSrcVertices, 0 );
    m_pMeshIB->Lock( 0, 0, (BYTE**)&pSrcIndices, 0 );

    WORD*  pGlobalTriangleIndices     = new WORD[m_dwNumMeshIndices*3];
    DWORD  dwGlobalTriangleIndexCount = 0;

    for( DWORD i = 0; i < m_dwNumMeshSubsets; i++ )
    {
        WORD* pIndices     = &pSrcIndices[m_pMeshSubsets[i].dwIndexStart];
        DWORD dwNumIndices = m_pMeshSubsets[i].dwIndexCount;

        WORD* pTriangleIndices     = &pGlobalTriangleIndices[dwGlobalTriangleIndexCount];
		DWORD dwNumTriangleIndices = 0;
            
        // Unstrip the indices.
		WORD ind0 = 0;
		WORD ind1 = pIndices[0];
		WORD ind2 = pIndices[1];

		for( DWORD src = 2; src < dwNumIndices; src++ )
		{
			ind0 = ind1;
			ind1 = ind2;
			ind2 = pIndices[src];
			
			if (ind0 != ind1 && ind1 != ind2 && ind2 != ind0)
			{
				if (src & 1)
				{
					pTriangleIndices[dwNumTriangleIndices] = ind2;
					dwNumTriangleIndices++;

					pTriangleIndices[dwNumTriangleIndices] = ind1;
					dwNumTriangleIndices++;

					pTriangleIndices[dwNumTriangleIndices] = ind0;
					dwNumTriangleIndices++;
				}
				else
				{
					pTriangleIndices[dwNumTriangleIndices] = ind0;
					dwNumTriangleIndices++;

					pTriangleIndices[dwNumTriangleIndices] = ind1;
					dwNumTriangleIndices++;

					pTriangleIndices[dwNumTriangleIndices] = ind2;
					dwNumTriangleIndices++;
				}
			}
		}

        m_pMeshSubsets[i].dwIndexStart = dwGlobalTriangleIndexCount;
        m_pMeshSubsets[i].dwIndexCount = dwNumTriangleIndices;
		dwGlobalTriangleIndexCount += dwNumTriangleIndices;
    }

    // Create an index buffer for using DrawIndexedPrimitive.
    LPDIRECT3DINDEXBUFFER8 pNewIB;
    pd3dDevice->CreateIndexBuffer( dwGlobalTriangleIndexCount * sizeof(WORD),
                                   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                   D3DPOOL_MANAGED, &pNewIB );

    // Lock and fill the index buffer
    WORD* pDstIndices;
    pNewIB->Lock( 0, 0, (BYTE**)&pDstIndices, 0 );
    memcpy( pDstIndices, pGlobalTriangleIndices, dwGlobalTriangleIndexCount * sizeof(WORD) );
    pNewIB->Unlock();

    // Free our copy of the indices.
    delete[] pGlobalTriangleIndices;

    // Assign the new buffers
    m_pMeshIB->Unlock();
    m_pMeshIB->Release();
    m_dwMeshPrimType   = D3DPT_TRIANGLELIST;
    m_pMeshIB          = pNewIB;
    m_dwNumMeshIndices = dwGlobalTriangleIndexCount;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Stripify()
// Desc: Turn the meshdata from a triangle list into an optimized triangle
//       strip.
//-----------------------------------------------------------------------------
HRESULT CD3DFrame::Stripify( DWORD dwFlags )
{
    if( D3DPT_TRIANGLESTRIP == m_dwMeshPrimType ) 
	{
		Unstrip();
	}

    if( D3DPT_TRIANGLELIST != m_dwMeshPrimType ) 
	{
		return E_FAIL;
	}

    // Get a d3d device (needed for creating buffers)
    LPDIRECT3DDEVICE8 pd3dDevice;
    m_pMeshVB->GetDevice( &pd3dDevice );
    pd3dDevice->Release();

    // Gain access to the vertices and indices for the mesh
    BYTE* pSrcVertices;
    WORD* pSrcIndices;
    m_pMeshVB->Lock( 0, 0, (BYTE**)&pSrcVertices, 0 );
    m_pMeshIB->Lock( 0, 0, (BYTE**)&pSrcIndices, 0 );

    WORD*  pGlobalStrippedIndices     = NULL;
    DWORD  dwGlobalStrippedIndexCount = 0;

    for( DWORD i = 0; i < m_dwNumMeshSubsets; i++ )
    {
        WORD* pIndices     = &pSrcIndices[m_pMeshSubsets[i].dwIndexStart];
        DWORD dwNumIndices = m_pMeshSubsets[i].dwIndexCount;
            
        // Run the tri-list through our tri-stripper
        WORD* pStrippedIndices;
        DWORD dwNumStrippedIndices;
        ::Stripify( dwNumIndices/3, pIndices, &dwNumStrippedIndices, 
                    &pStrippedIndices, dwFlags );

        m_pMeshSubsets[i].dwIndexStart = dwGlobalStrippedIndexCount;
        m_pMeshSubsets[i].dwIndexCount = dwNumStrippedIndices;

        // Copy the new tri-stripped list into a public spot
        WORD* pNewStripData = new WORD[dwGlobalStrippedIndexCount+dwNumStrippedIndices];
        memcpy( &pNewStripData[0], pGlobalStrippedIndices, 
                sizeof(WORD)*dwGlobalStrippedIndexCount );
        memcpy( &pNewStripData[dwGlobalStrippedIndexCount], pStrippedIndices, 
                sizeof(WORD)*dwNumStrippedIndices );

		delete[] pStrippedIndices;
        
        if( pGlobalStrippedIndices )
            delete pGlobalStrippedIndices;
        pGlobalStrippedIndices      = pNewStripData;
        dwGlobalStrippedIndexCount += dwNumStrippedIndices;
    }

    // Sort the vertices...
    WORD* pVertexPermutation; // Array for sorting
    ComputeVertexPermutation( dwGlobalStrippedIndexCount, pGlobalStrippedIndices,
                              m_dwNumMeshVertices, &pVertexPermutation );

    // Create a new vertex buffer
    LPDIRECT3DVERTEXBUFFER8 pNewVB;
    pd3dDevice->CreateVertexBuffer( m_dwNumMeshVertices * m_dwMeshVertexSize, 
                                    D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, 
                                    &pNewVB );

    // Lock and fill the vertex buffer, remapping vertices through the
    // vertex permutation array.
    BYTE* pDstVertices;
    pNewVB->Lock( 0, 0, (BYTE**)&pDstVertices, 0 );
    for( i = 0; i < m_dwNumMeshVertices; i++ )
        memcpy( &pDstVertices[m_dwMeshVertexSize*i], 
                &pSrcVertices[m_dwMeshVertexSize*pVertexPermutation[i]], 
                m_dwMeshVertexSize );
    pNewVB->Unlock();

    // Free the array allocated by the ComputeVertexPermutation() call
    delete[] pVertexPermutation;

    // Create an index buffer for using DrawIndexedPrimitive.
    LPDIRECT3DINDEXBUFFER8 pNewIB;
    pd3dDevice->CreateIndexBuffer( dwGlobalStrippedIndexCount * sizeof(WORD),
                                   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                   D3DPOOL_MANAGED, &pNewIB );

    // Lock and fill the index buffer
    WORD* pDstIndices;
    pNewIB->Lock( 0, 0, (BYTE**)&pDstIndices, 0 );
    memcpy( pDstIndices, pGlobalStrippedIndices, dwGlobalStrippedIndexCount * sizeof(WORD) );
    pNewIB->Unlock();

    // Free the array allocated by the Stripify() call
    delete[] pGlobalStrippedIndices;

    // Assign the new buffers
    m_pMeshVB->Unlock();
    m_pMeshIB->Unlock();
    m_pMeshVB->Release();
    m_pMeshIB->Release();
    m_pMeshVB          = pNewVB;
    m_pMeshIB          = pNewIB;

	if( dwFlags & OUTPUT_TRILIST )
		m_dwMeshPrimType   = D3DPT_TRIANGLELIST;
	else
		m_dwMeshPrimType   = D3DPT_TRIANGLESTRIP;

    m_dwNumMeshIndices = dwGlobalStrippedIndexCount;

    return S_OK;
}
