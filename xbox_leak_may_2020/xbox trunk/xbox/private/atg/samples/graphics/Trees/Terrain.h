#pragma once
//-----------------------------------------------------------------------------
//  
//  File: Terrain.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//-----------------------------------------------------------------------------
#include "Terrain.h"

struct Grid {
	UINT m_width, m_height;					// size of grid
	LPDIRECT3DVERTEXBUFFER8 m_pVB;	 		// vertex buffer
	LPDIRECT3DINDEXBUFFER8  m_pIB;			// triangle indices for base layer
	Grid() {
		m_pVB = NULL; m_pIB = NULL;
	}
	~Grid() { Cleanup(); }
	HRESULT Initialize(UINT width, UINT height, UINT cbVertexSize);
	HRESULT FillIndexBuffer(LPDIRECT3DINDEXBUFFER8 pIB, const RECT *pRect, UINT *pnTri);
	HRESULT Cleanup();
};

struct TERRAINVERTEX
{
	D3DXVECTOR3 vBase; // texture coords + height field
};

class Terrain
{
public:
	Grid	m_grid;							// grid sampled from height field
	LPDIRECT3DTEXTURE8      m_pTerrain;		// terrain height field texture
	LPDIRECT3DTEXTURE8      m_pTexture;		// terrain texture
	LPDIRECT3DTEXTURE8		m_pTexture2;	// second terrain modulation texture (tree shadows, etc)
	D3DXVECTOR3 m_vMin, m_vMax;				// bounds of terrain
	D3DXVECTOR2 m_vTextureScale, m_vTextureOffset;	// terrain texture transformation
	HRESULT GetHeightFieldNeighbors(const D3DXVECTOR2 &vP, D3DXVECTOR2 *pvFraction, TERRAINVERTEX rVertex[4]);
	HRESULT ResampleGrid(); // fill vertex buffer with current sampling densities
public:
	Terrain();
	~Terrain() { Cleanup(); }
	HRESULT Initialize(LPDIRECT3DTEXTURE8 pTerrain,	// height field texture
					   LPDIRECT3DTEXTURE8 pTexture,	// texture to apply to terrain
					   LPDIRECT3DTEXTURE8 pTexture2,	// tree-shadow texture
					   const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax,	// bounds of terrain in world coords
					   const D3DXVECTOR2 &vTextureScale, const D3DXVECTOR2 &vTextureOffset, 	// terrain texture transformation
					   UINT width, UINT height);	// sanmpling density for mesh
	HRESULT GetTerrainPoint(const D3DXVECTOR3 &vPosition, // lookup point from terrain grid
							float *pfTerrainHeight,
							D3DXVECTOR3 *pvTerrainNormal);
	HRESULT RefineGrid(const D3DXVECTOR3 &vPosition, float fEpsilon); // add more samples near vPosition in the domain
	HRESULT ResetGrid();					// drop to lowest level LOD, uniform sampling
	HRESULT DrawTerrain();					// draw raw terrain geometry. Bracket with Begin .. End to set up rendering state.
	HRESULT Cleanup();
};
