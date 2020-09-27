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
	LPDIRECT3DINDEXBUFFER8  m_pIB;			// triangle indices
	Grid() { m_pVB = NULL; m_pIB = NULL; }
	~Grid() { Cleanup(); }
	HRESULT Initialize(UINT width, UINT height, UINT cbVertexSize);
	HRESULT Cleanup();
};

struct TERRAINVERTEX
{
	D3DXVECTOR3 vBase; // texture coords + height field
	D3DXVECTOR3 vTip; // offset to grass tips
};

class Terrain
{
	Grid	m_grid;							// grid sampled from height field
	LPDIRECT3DTEXTURE8      m_pTerrain;		// terrain height field texture
	LPDIRECT3DTEXTURE8      m_pTexture;		// terrain texture
	DWORD m_dwTerrainVertexShader;			// terrain vertex shader handle
	D3DXVECTOR3 m_vMin, m_vMax;				// bounds of terrain
	float m_fGrassHeight;					// default height of grass in the world

	HRESULT GetHeightFieldNeighbors(const D3DXVECTOR2 &vP, D3DXVECTOR2 *pvFraction, TERRAINVERTEX rVertex[4]);
	HRESULT Resample(); // fill vertex buffer with current sampling densities
public:
	Terrain();
	~Terrain() { Cleanup(); }
	HRESULT Initialize(LPDIRECT3DTEXTURE8 pTerrain, LPDIRECT3DTEXTURE8 pTexture, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax, float fGrassHeight);
	HRESULT GetTerrainPoint(const D3DXVECTOR3 &vPosition, float *pfTerrainHeight, D3DXVECTOR3 *pvTerrainNormal);	// look up terrain point from height field texture
	HRESULT BallUpdate(float fTimeStep, const D3DXVECTOR3 &vPositionA, const D3DXVECTOR3 &vPositionB, float fRadius); // ball moved from A to B
	HRESULT Refine(const D3DXVECTOR3 &vPosition, float fEpsilon); // add more samples near vPosition in the domain
	HRESULT Reset();						// drop to lowest level LOD, uniform sampling
	HRESULT Begin();						// cull tiles, calc rendering order, setup vertex shader and texture state
	HRESULT DrawTerrain();					// draw raw terrain geometry. Bracket with Begin .. End to set up rendering state.
	HRESULT DrawDebug();					// draw debugging info, such as the trajectory of the ball used in BallUpdate
	HRESULT End();							// paired with begin to cleanup unusual rendering state
	HRESULT Cleanup();
};
