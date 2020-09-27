#pragma once
//-----------------------------------------------------------------------------
// File: XBFurMesh.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifdef _XBOX
#include <xtl.h>
#else
#include <d3d8.h>
#endif
#include "XBFur.h"

class CXBFurMesh
{
public:
	DWORD m_dwNumVertices;					// current # of vertices
	IDirect3DVertexBuffer8 *m_pVB;			// vertex buffer
	DWORD m_dwNumIndices;					// current # of indices
	IDirect3DIndexBuffer8 *m_pIB;			// index buffer
    IDirect3DTexture8 *m_pTexture;			// base mesh texture
	DWORD m_dwGenericVS;					// generic vertex shader used in DrawBaseMesh
	float m_fFinDotProductThreshold;		// max dot product of fin's face normal with eye vector
	UINT m_nFinBin;							// number of discretized directions for fin bins
	struct FinBin {
		D3DXVECTOR3 m_vDirection;			// selection direction for this bin
		float	m_fDirectionThreshold;		// cutoff for dot product of eye direction with m_vDirection
		DWORD m_dwNumFins;					// number of fins in this bin
		IDirect3DVertexBuffer8 *m_pFinVB;	// fin vertex buffer filled with 1 quad per edge
	} *m_rFinBin;
	struct DrawState {
		DWORD dwLighting;
		D3DXVECTOR3 vEyePos;			// position of eye in world coords
		D3DXVECTOR3 vEyeObject;			// position of eye in local object coords
		D3DXVECTOR3 vEyeDirection;		// direction from center of object to eye in local object coords
		D3DXVECTOR3 vLightPos;			// position of light in world coords
		D3DXVECTOR3 vLightObject;		// position of light in local object coords
		D3DXVECTOR3 vLightDirection;	// direction from center of object to light in local object coords
		D3DXMATRIX matViewProjection;
		D3DXVECTOR4 vOffset;
		D3DXVECTOR4 vSelfShadow;
		float fSelfShadowLog2Attenuation;
	} m_DrawState;
	
	CXBFurMesh();
	~CXBFurMesh();
	HRESULT Initialize(DWORD dwFVF, DWORD dwVertexCount, LPDIRECT3DVERTEXBUFFER8 pVB, DWORD dwIndexCount, LPDIRECT3DINDEXBUFFER8 pIB);
	HRESULT ExtractFins(UINT BinFactor, float fFinDotProductThreshold, float fEdgeTextureScale); // nFinBin = octohedron sphere subdivision = 4*(BinFactor + 1)*(BinFactor +1) + 2
	HRESULT ScaleTextureCoordinates(float fUScale, float fVScale);
	HRESULT CleanFins(); // deallocate memory
	void DrawBaseMesh();

	// State setting and drawing are put in separate functions so that when multiple furry models
	// are rendered (especially with the same fur) we can get some state-setting savings.
	void Begin(D3DXVECTOR3 *pvEyePos, D3DXVECTOR3 *pvLightPos, D3DXMATRIX *pmatViewProjection);  // set state common to both fins and fur
	void BeginObject(D3DXMATRIX *pmatWorld, D3DXMATRIX *pmatWorldInverse);
	void DrawFins(CXBFur *pFur, DWORD dwFinVS,
				  float fFinLODFull, /* LOD values less than this are fully on */
				  float fFinLODCutoff, /* LOD values beyond this are off */
				  float fFinExtraNormalScale); /* fins look better if then stick out a little more than the fur */
	void DrawShells(CXBFur *pFur, DWORD dwFurVS, DWORD dwFurPS[3]);
	void EndObject();
	void End();
};
