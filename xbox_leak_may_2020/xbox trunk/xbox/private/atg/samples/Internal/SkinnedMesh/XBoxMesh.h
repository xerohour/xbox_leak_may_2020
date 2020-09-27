//-----------------------------------------------------------------------------
// File: XBoxMesh.h
//
// Desc: 
//
// Copyright (C) Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#ifndef XBOXMESH_H
#define XBOXMESH_H


struct BONECOMBINATION
{
    DWORD AttribId;
    DWORD FaceStart;
    DWORD FaceCount;
    DWORD VertexStart;
    DWORD VertexCount;
    DWORD* BoneId;
};




class CXBoxMesh
{
	BOOL       m_bInitialized;
	DWORD      m_dwFVF;
	LPDIRECT3DVERTEXBUFFER8 m_pVB;
	LPDIRECT3DINDEXBUFFER8  m_pIB;
	DWORD      m_dwNumVertices;
	DWORD      m_dwNumIndices;
	DWORD      m_dwVertexSize;
	D3DXATTRIBUTERANGE* m_AttribTable;
	DWORD      m_dwAttribTableSize;
	DWORD*     m_pAttributeIds;

public:
	CXBoxMesh();
	HRESULT Initialize();
	ID3DXMesh* m_pD3DXMesh;

public:
	LPDIRECT3DVERTEXBUFFER8 GetVertexBuffer() { return m_pVB; }
	LPDIRECT3DINDEXBUFFER8  GetIndexBuffer()  { return m_pIB; }
	DWORD    GetNumFaces()    { return m_dwNumIndices/3; }
	DWORD    GetNumVertices() { return m_dwNumVertices; }
	DWORD    GetFVF()         { return m_dwFVF; }

	HRESULT  DrawSubset( DWORD i );
	
	DWORD    Release();
};

struct CXBoxSkinMesh
{
	DWORD m_dwNumIndices;
	DWORD m_dwNumBones;

	BOOL  m_bInitialized;

public:
	CXBoxSkinMesh();
	HRESULT Initialize();
	ID3DXSkinMesh* m_pD3DXSkinMesh;
public:
	DWORD    GetNumFaces() { return m_dwNumIndices/3; }
	DWORD    GetNumBones() { return m_dwNumBones; }

	HRESULT  GetOriginalMesh( CXBoxMesh** ppMesh );
	HRESULT  ConvertToOptimizedBlendedMesh( DWORD* pAdjacencyIn, DWORD* pAdjacencyOut, 
                                            DWORD* pdwNumBoneCombinations, 
											BONECOMBINATION** ppBoneCombinations, 
								            CXBoxMesh** ppMesh );
	HRESULT  ConvertToIndexedBlendedMesh( DWORD* pAdjacencyIn, DWORD dwPaletteSize,
		                                  DWORD* pdwNumBoneCombinations, 
										  BONECOMBINATION** ppBoneCombinations, 
								          CXBoxMesh** ppMesh );
	DWORD    Release();
};


HRESULT XBoxLoadSkinMeshFromXof( LPDIRECTXFILEDATA pxofobjMesh, LPDIRECT3DDEVICE8 pd3dDevice, 
								 CXBPackedResource* pResource, DWORD** ppAdjacency,
								 DWORD* pdwNumMaterials,
                                 D3DMATERIAL8** ppMaterialArray, LPDIRECT3DTEXTURE8** ppTextureArray,
								 CHAR*** pppBoneNames, D3DXMATRIX** ppBoneTransforms,
								 CXBoxSkinMesh** ppMesh );


#endif // XBOXMESH_H

