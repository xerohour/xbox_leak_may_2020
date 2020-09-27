#pragma once

#ifndef __SKINMESH_H__
#define __SKINMESH_H__

/*//////////////////////////////////////////////////////////////////////////////
//
// File: skinmesh.h
//
// Copyright (C) 2000 Microsoft Corporation. All Rights Reserved.
//
//
//////////////////////////////////////////////////////////////////////////////*/

#include "tri3mesh.h"

class CBone
{
public:
    DWORD m_numWeights;
    DWORD* m_pVertIndices;
    float* m_pWeights;
	CBone();
	~CBone();
};

typedef CBone* LPBONE;

// Defines for CD3DXSkinMesh::m_DataValid
enum {
    D3DXSM_VERTINFL = 1,
    D3DXSM_FACEINFL = 2,
    D3DXSM_VERTDATA = 4,
};

class CD3DXSkinMesh : public ID3DXSkinMesh
{
public:
    HRESULT WINAPI QueryInterface(REFIID iid, LPVOID *ppv);

    ULONG WINAPI AddRef();
    
	ULONG WINAPI Release();

    STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE8* ppDevice);
	
    STDMETHOD(GetVertexBuffer)(THIS_ LPDIRECT3DVERTEXBUFFER8* ppVB);

    STDMETHOD(GetIndexBuffer)(THIS_ LPDIRECT3DINDEXBUFFER8* ppIB);

	STDMETHOD(LockVertexBuffer)(THIS_ DWORD flags, BYTE** ppData);

	STDMETHOD(UnlockVertexBuffer)(THIS);

	STDMETHOD(LockIndexBuffer)(THIS_ DWORD flags, BYTE** ppData);

    STDMETHOD(UnlockIndexBuffer)(THIS);

	STDMETHOD(LockAttributeBuffer)(THIS_ DWORD flags, DWORD** ppData);

    STDMETHOD(UnlockAttributeBuffer)(THIS);

    STDMETHOD_(DWORD, GetNumBones)(THIS);

    STDMETHOD(GetOriginalMesh)(THIS_ LPD3DXMESH* ppMesh);

    STDMETHOD_(DWORD, GetNumFaces)(THIS);
    
    STDMETHOD_(DWORD, GetNumVertices)(THIS);
    
    STDMETHOD_(DWORD, GetFVF)(THIS);
    
    STDMETHOD(GetDeclaration)(THIS_ DWORD Declaration[MAX_FVF_DECL_SIZE]);
    
    STDMETHOD_(DWORD, GetOptions)(THIS);

    HRESULT WINAPI SetBoneInfluence(DWORD bone,
							 DWORD numInfluences,
							 CONST DWORD* vertices,
							 CONST float* weights);
	
	DWORD WINAPI GetNumBoneInfluences(DWORD bone);
	
	HRESULT WINAPI GetBoneInfluence(DWORD bone, 
							 DWORD* vertices,
							 float* weights);
	
	STDMETHOD(GetMaxVertexInfluences)(THIS_ DWORD* maxVertexInfluences);
	
	STDMETHOD(GetMaxFaceInfluences)(THIS_ DWORD* maxFaceInfluences);

	HRESULT WINAPI GenerateSkinnedMesh(DWORD options, 
                         FLOAT fMinWeight, 
                         CONST LPDWORD rgiAdjacencyIn, 
                         LPDWORD rgiAdjacencyOut, 
                         LPD3DXMESH* ppMesh);

    HRESULT WINAPI UpdateSkinnedMesh(CONST D3DXMATRIX* pBoneTransforms, LPD3DXMESH pMesh);

	HRESULT WINAPI ConvertToBlendedMesh(DWORD options,
						 CONST LPDWORD pAdjacencyIn,
						 LPDWORD pAdjacencyOut,
						 DWORD* pNumBoneCombinations,
						 LPD3DXBUFFER* ppBoneCombinationTable,
						 LPD3DXMESH* ppMesh);

	HRESULT WINAPI ConvertToIndexedBlendedMesh(DWORD options,
						 CONST LPDWORD pAdjacencyIn,
                         DWORD paletteSize, 
						 LPDWORD pAdjacencyOut,
                         DWORD* pNumBoneCombinations, 
                         LPD3DXBUFFER* ppBoneCombinationTable, 
						 LPD3DXMESH* ppMesh);
	CD3DXSkinMesh();
	~CD3DXSkinMesh();
	HRESULT Init(DWORD numFaces, DWORD numVertices, DWORD numBones, DWORD options, DWORD fvf, LPDIRECT3DDEVICE8 pD3DDevice);
    HRESULT Init(LPD3DXMESH pMesh, LPBONE pBones, DWORD numBones);
	HRESULT Init(LPD3DXMESH pMesh, DWORD numBones);
	HRESULT Init(DWORD numBones);
    HRESULT CalcVertexSkinData();
    HRESULT TruncVertexSkinData(DWORD truncVertInfl, LPDWORD pAdjacencyIn);
    void CalcNumAttributes(LPDWORD pAttrMask, LPDWORD pAttrBits);
private:
	// Changes with SetBoneInfluence or GenerateSkinnedMesh
	CBone* m_pBones;
	DWORD m_numBones;
	DWORD m_maxVertInfl;
    DWORD* m_vertInfl;
    DWORD* m_vertMatId;
    float* m_vertMatWeight;

	// Changes with SetMesh
	GXTri3Mesh<tp16BitIndex> *m_pMesh;
	DWORD m_numAttrib;

	// Changes with either SetBoneInfluence or SetMesh
    DWORD m_matidShift;
    DWORD m_maxFaceInfl;
    DWORD m_DataValid;

	// Invariants to SetBoneInfluence and SetMesh
	DWORD m_refCnt;
    D3DXMATRIX** m_pBoneMatrix;
    DWORD m_faceClamp; // max infl per face in output mesh. Any more infl in the input are ignored
    BOOL m_bMixedDevice;

    // Calculated during GenerateSkinnedMesh
    float*  m_rgfCodes;
    bool    m_bChangedWeights;
    float   m_fMinWeight;

    // Helper functions
    HRESULT GenerateCodes();    // reallocates & recalculates m_rgfCodes
};

#endif //__SKINMESH_H__