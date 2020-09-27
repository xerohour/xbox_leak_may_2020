//-----------------------------------------------------------------------------
// File: XBoxMesh.cpp
//
// Desc: 
//
// Copyright (C) Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <XBApp.h>
#include <XBFont.h>
#include <XBResource.h>
#include <XBUtil.h>
#include "XBoxMesh.h"


CXBoxMesh::CXBoxMesh()
{
	m_bInitialized = FALSE;

	m_dwFVF;
	m_pVB;
	m_pIB;
	m_dwNumVertices;
	m_dwNumIndices;
	m_dwVertexSize;
	m_AttribTable;
	m_dwAttribTableSize;
	m_pAttributeIds;
}




HRESULT CXBoxMesh::Initialize()
{
	if( m_bInitialized )
		return S_OK;
	
	m_bInitialized = TRUE;

	m_pD3DXMesh->GetVertexBuffer( &m_pVB );
	m_pD3DXMesh->GetIndexBuffer( &m_pIB );
	m_dwFVF         = m_pD3DXMesh->GetFVF();
	m_dwNumVertices = m_pD3DXMesh->GetNumVertices();
	m_dwNumIndices  = m_pD3DXMesh->GetNumFaces() * 3;
	m_dwVertexSize  = D3DXGetFVFVertexSize( m_dwFVF );

	m_pD3DXMesh->GetAttributeTable( NULL, &m_dwAttribTableSize );
	m_AttribTable = new D3DXATTRIBUTERANGE[max(1, m_dwAttribTableSize)];
	if( m_dwAttribTableSize > 0 )
		m_pD3DXMesh->GetAttributeTable( m_AttribTable, NULL );

	m_pD3DXMesh->LockAttributeBuffer( D3DLOCK_READONLY, &m_pAttributeIds );

	return S_OK;
}

	
	
	
HRESULT CXBoxMesh::DrawSubset( DWORD i )
{
	if( FALSE == m_bInitialized )
		Initialize();

    if( m_pVB == NULL )
		return S_OK;

    // If present in the table, then draw it
    if( i < m_dwAttribTableSize && m_AttribTable[i].FaceCount > 0 )
    {
		g_pd3dDevice->SetVertexShader( m_dwFVF);
		g_pd3dDevice->SetStreamSource( 0, m_pVB, m_dwVertexSize );
		g_pd3dDevice->SetIndices( m_pIB, 0 );
        g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 
                                            m_AttribTable[i].VertexStart, m_AttribTable[i].VertexCount,
                                            m_AttribTable[i].FaceStart * 3, m_AttribTable[i].FaceCount );
    }

    return S_OK;
}




DWORD CXBoxMesh::Release()
{
	return S_OK;
}




CXBoxSkinMesh::CXBoxSkinMesh()
{
	m_bInitialized = FALSE;
}




HRESULT CXBoxSkinMesh::Initialize()
{
	if( m_bInitialized )
		return S_OK;
	
	m_dwNumIndices = m_pD3DXSkinMesh->GetNumFaces() * 3;
	m_dwNumBones   = m_pD3DXSkinMesh->GetNumBones();
	m_bInitialized = TRUE;

	return S_OK;
}




DWORD CXBoxSkinMesh::Release()
{
	return S_OK;
}




HRESULT CXBoxSkinMesh::GetOriginalMesh( CXBoxMesh** ppMesh )
{
	ID3DXMesh* pLocalMesh;
	HRESULT hr = m_pD3DXSkinMesh->GetOriginalMesh( &pLocalMesh );

	(*ppMesh) = new CXBoxMesh();
	(*ppMesh)->m_pD3DXMesh = pLocalMesh;
	(*ppMesh)->Initialize();

	return S_OK;
}





HRESULT CXBoxSkinMesh::ConvertToOptimizedBlendedMesh( DWORD* pAdjacencyIn, DWORD* pAdjacencyOut, 
                                                      DWORD* pdwNumBoneCombinations,
 										              BONECOMBINATION** ppBoneCombinations, 
							                          CXBoxMesh** ppMesh )
{
	ID3DXMesh* pLocalMesh;
	ID3DXBuffer* pBoneCombinationBuffer;

	HRESULT hr = m_pD3DXSkinMesh->ConvertToBlendedMesh( 0, pAdjacencyIn, pAdjacencyOut, 
                                                        pdwNumBoneCombinations, &pBoneCombinationBuffer, 
											            &pLocalMesh );

	// Extract bones combinations from buffer
	{
		BONECOMBINATION* pSrcBuffer = (BONECOMBINATION*)pBoneCombinationBuffer->GetBufferPointer();
		BONECOMBINATION* pDstBuffer = (BONECOMBINATION*)new BYTE[pBoneCombinationBuffer->GetBufferSize()];
		memcpy( pDstBuffer, pSrcBuffer, pBoneCombinationBuffer->GetBufferSize() );

		// Pointer fixup
		for( DWORD i=0; i<(*pdwNumBoneCombinations); i++ )
			pDstBuffer[i].BoneId = (DWORD*)( (long)pDstBuffer[i].BoneId + (long)pDstBuffer - (long)pSrcBuffer );
		
		(*ppBoneCombinations) = pDstBuffer;
		pBoneCombinationBuffer->Release();
	}
	
    // Vertex cache optimize the mesh
    ID3DXMesh* pLocalMeshOpt;
    hr = pLocalMesh->Optimize( D3DXMESHOPT_VERTEXCACHE, pAdjacencyOut,
			                   NULL, NULL, NULL, &pLocalMeshOpt );
    if( SUCCEEDED(hr) )
    {
		pLocalMesh->Release();
		pLocalMesh = pLocalMeshOpt;
    }

	(*ppMesh) = new CXBoxMesh();
	(*ppMesh)->m_pD3DXMesh = pLocalMesh;
	(*ppMesh)->Initialize();

	return S_OK;
}




HRESULT CXBoxSkinMesh::ConvertToIndexedBlendedMesh( DWORD* pAdjacencyIn, DWORD dwPaletteSize,
		                      DWORD* pdwNumBoneCombinations,
							  BONECOMBINATION** ppBoneCombinations, 
							  CXBoxMesh** ppMesh )
{
	ID3DXMesh*   pLocalMesh;
	ID3DXBuffer* pBoneCombinationBuffer;

	HRESULT hr = m_pD3DXSkinMesh->ConvertToIndexedBlendedMesh( D3DXMESH_SYSTEMMEM, pAdjacencyIn, dwPaletteSize,
		                                                       NULL, pdwNumBoneCombinations, 
													           &pBoneCombinationBuffer, 
													           &pLocalMesh );

	// Extract bones combinations from buffer
	{
		BONECOMBINATION* pSrcBuffer = (BONECOMBINATION*)pBoneCombinationBuffer->GetBufferPointer();
		BONECOMBINATION* pDstBuffer = (BONECOMBINATION*)new BYTE[pBoneCombinationBuffer->GetBufferSize()];
		memcpy( pDstBuffer, pSrcBuffer, pBoneCombinationBuffer->GetBufferSize() );

		// Pointer fixup
		for( DWORD i=0; i<(*pdwNumBoneCombinations); i++ )
			pDstBuffer[i].BoneId = (DWORD*)( (long)pDstBuffer[i].BoneId + (long)pDstBuffer - (long)pSrcBuffer );
		
		(*ppBoneCombinations) = pDstBuffer;
		pBoneCombinationBuffer->Release();
	}
	
	(*ppMesh) = new CXBoxMesh();
	(*ppMesh)->m_pD3DXMesh = pLocalMesh;
	(*ppMesh)->Initialize();

	return S_OK;
}




HRESULT XBoxLoadSkinMeshFromXof( LPDIRECTXFILEDATA pxofobjMesh, LPDIRECT3DDEVICE8 pd3dDevice, 
								 CXBPackedResource* pResource, DWORD** ppAdjacency,
                                 DWORD* pdwNumMaterials, 
                                 D3DMATERIAL8** ppMaterialArray, LPDIRECT3DTEXTURE8** ppTextureArray,
								 CHAR*** pppBoneNames, D3DXMATRIX** ppBoneTransforms,
								 CXBoxSkinMesh** ppMesh )
{
	ID3DXSkinMesh* pLocalSkinMesh;
	ID3DXBuffer*   pBoneNamesBuffer;
	ID3DXBuffer*   pBoneTransformsBuffer;
	ID3DXBuffer*   pAdjacencyBuffer;
	ID3DXBuffer*   pMaterialBuffer;

	HRESULT hr = D3DXLoadSkinMeshFromXof( pxofobjMesh, 0, pd3dDevice, &pAdjacencyBuffer,
                                          &pMaterialBuffer, pdwNumMaterials, &pBoneNamesBuffer, 
										  &pBoneTransformsBuffer, &pLocalSkinMesh );

	DWORD dwNumBones = pLocalSkinMesh->GetNumBones();

	// Extract adjacency array from buffer
	{
		DWORD* pSrcBuffer = (DWORD*)pAdjacencyBuffer->GetBufferPointer();
		DWORD* pDstBuffer = (DWORD*)new BYTE[pAdjacencyBuffer->GetBufferSize()];
		memcpy( pDstBuffer, pSrcBuffer, pAdjacencyBuffer->GetBufferSize() );

		(*ppAdjacency) = pDstBuffer;
		pAdjacencyBuffer->Release();
	}

	// Extract bones names from buffer
	{
		CHAR** pSrcBuffer = (CHAR**)pBoneNamesBuffer->GetBufferPointer();
		CHAR** pDstBuffer = (CHAR**)new BYTE[pBoneNamesBuffer->GetBufferSize()];
		memcpy( pDstBuffer, pSrcBuffer, pBoneNamesBuffer->GetBufferSize() );

		// Pointer fixup
		for( DWORD i=0; i<dwNumBones; i++ )
			pDstBuffer[i] = (CHAR*)( (long)pDstBuffer[i] + (long)pDstBuffer - (long)pSrcBuffer );
		
		(*pppBoneNames) = pDstBuffer;
		pBoneNamesBuffer->Release();
	}

	// Extract bones transforms from buffer
	{
		D3DXMATRIX* pSrcBuffer = (D3DXMATRIX*)pBoneTransformsBuffer->GetBufferPointer();
		D3DXMATRIX* pDstBuffer = (D3DXMATRIX*)new BYTE[pBoneTransformsBuffer->GetBufferSize()];
		memcpy( pDstBuffer, pSrcBuffer, pBoneTransformsBuffer->GetBufferSize() );
		
		(*ppBoneTransforms) = pDstBuffer;
		pBoneTransformsBuffer->Release();
	}

	// Extract textures
	{
		if( (*pdwNumMaterials) == 0 )
		{
			(*pdwNumMaterials) = 1;
			D3DMATERIAL8*       pMaterials = new D3DMATERIAL8[(*pdwNumMaterials)];
			LPDIRECT3DTEXTURE8* pTextures  = new LPDIRECT3DTEXTURE8[(*pdwNumMaterials)];

			ZeroMemory( &pMaterials[0], sizeof(D3DMATERIAL8) );
			pMaterials[0].Diffuse.r = 0.5f;
			pMaterials[0].Diffuse.g = 0.5f;
			pMaterials[0].Diffuse.b = 0.5f;
			pMaterials[0].Specular  = pMaterials[0].Diffuse;
			pTextures[0]            = NULL;

			(*ppMaterialArray) = pMaterials;
			(*ppTextureArray) = pTextures;
	    }
		else
		{
			D3DXMATERIAL* pSrcBuffer = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();
			
			D3DMATERIAL8*       pMaterials = new D3DMATERIAL8[(*pdwNumMaterials)];
			LPDIRECT3DTEXTURE8* pTextures  = new LPDIRECT3DTEXTURE8[(*pdwNumMaterials)];
			
			for( DWORD i=0; i<(*pdwNumMaterials); i++ )
			{
				memcpy( &pMaterials[i], &pSrcBuffer[i].MatD3D, sizeof(D3DMATERIAL8) );

				if( pResource && pSrcBuffer[i].pTextureFilename )
					pTextures[i] = pResource->GetTexture( pSrcBuffer[i].pTextureFilename );
				else
					pTextures[i] = NULL;
			}

			(*ppMaterialArray) = pMaterials;
			(*ppTextureArray) = pTextures;

			pMaterialBuffer->Release();
		}
	}

	(*ppMesh) = new CXBoxSkinMesh;
	(*ppMesh)->m_pD3DXSkinMesh = pLocalSkinMesh;
	(*ppMesh)->Initialize();

	return S_OK;
}


