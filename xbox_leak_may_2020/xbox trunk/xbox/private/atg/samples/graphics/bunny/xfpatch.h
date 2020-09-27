//-----------------------------------------------------------------------------
// File: xfpatch.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __FUZZ_H
#define __FUZZ_H

#ifdef _XBOX
#include <xtl.h>
#else
#include <d3d8.h>
#endif
#include "xfvf.h"
#include "xfpmesh.h"

#define MAX_VLEVEL 32

extern D3DXMATRIX g_matProj;
extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matWorld;
extern DWORD g_dwFuzzVS;
extern DWORD g_dwFinVS;
extern D3DXVECTOR3 g_LightPos;
extern D3DXVECTOR3 g_EyePos;

extern float g_fOneInch;

// patch generation

// a fuzz is a single hair follicle, blade of grass, etc.
struct SFuzz
{
	D3DVECTOR dp;			// velocity
	D3DVECTOR ddp;			// acceleration
	D3DXCOLOR colorBase;
	D3DXCOLOR colorTip;
};

// a fuzz instance is a single instance of a fuzz
// located at x, z on the patch
// we create only a limited number of unique fuzzes
// and index the library with lidx.
struct SFuzzInst
{
	float x, z;				// fuzz location
	int lidx;				// library index
};

// a fuzz patch is a volume that holds fuzzes.
// xsize and zsize are chosen by the user
// ysize is calculated using the height of the 
// tallest fuzz
class CXFPatch
{
public:
	DWORD m_dwSeed;			// patch seed
	
	float m_fXSize;			// patch size in world coords
	float m_fYSize;
	float m_fZSize;

	// fuzz library
	DWORD m_dwNumSegments;	// # of segments in highest LOD
	SFuzz m_fuzzCenter;		// fuzz constant
	SFuzz m_fuzzRandom;		// random offset around center
	DWORD m_dwNumFuzzLib;	// # of fuzz in the library
	SFuzz *m_pFuzzLib;		// fuzz library

	// fuzz instances
	DWORD m_dwNumFuzz;		// # of fuzz in this patch
	SFuzzInst *m_pFuzz;

	// patch volume
	DWORD m_dwNumLayers;	// # of layers in the volume
	DWORD m_dwVolSize;		// width*depth
	DWORD m_dwVolXSize;		// width of volume texture
	DWORD m_dwVolZSize;		// depth of volume texture
	LPDIRECT3DTEXTURE8 m_pVolTexture[MAX_VLEVEL];	// volume texture

	// highest LOD texture
	LPDIRECT3DTEXTURE8 m_pTexture;

	// hair lighting texture
	D3DMATERIAL8 m_HairLightingMaterial;
	LPDIRECT3DTEXTURE8 m_pHairLightingTexture;

	// fin texture
	LPDIRECT3DTEXTURE8 m_pFinTexture;	// texture projected from the side

	CXFPatch();
	~CXFPatch();

	void InitFuzz(DWORD nfuzz, DWORD nfuzzlib);
	void GenVolume(DWORD nlayers, DWORD volxsize, DWORD volzsize);
	void Splat(D3DVECTOR p, float falpha, D3DVECTOR fcolor,	D3DXVECTOR4 *pVolume); // used by GenVolume
	HRESULT SetHairLightingMaterial(D3DMATERIAL8 *pMaterial);
	void GetLinesVertexBuffer(IDirect3DVertexBuffer8 **ppVB);
	void RenderLines();
	void RenderGeometryTexture(CXFPatchMesh *pmesh);
	void Render(float lod, CXFPatchMesh *pmesh);
	void Save(char *fname, int flags);
	void Load(char *fname);


	
	void SetPatchSize(float x, float z)
	{
		m_fXSize = x;
		m_fZSize = z;
		InitFuzz(m_dwNumFuzz, m_dwNumFuzzLib);	// re-init the fuzz. automatically sets ysize
	};

	void SetFVel(float cx, float cy, float cz, float rx, float ry, float rz)
	{
		m_fuzzCenter.dp.x = cx; m_fuzzCenter.dp.y = cy; m_fuzzCenter.dp.z = cz;
		m_fuzzRandom.dp.x = rx; m_fuzzRandom.dp.y = ry; m_fuzzRandom.dp.z = rz;
	};

	void SetFAcc(float cx, float cy, float cz, float rx, float ry, float rz)
	{
		m_fuzzCenter.ddp.x = cx; m_fuzzCenter.ddp.y = cy; m_fuzzCenter.ddp.z = cz;
		m_fuzzRandom.ddp.x = rx; m_fuzzRandom.ddp.y = ry; m_fuzzRandom.ddp.z = rz;
	};
	
};

HRESULT FillHairLightingTexture(D3DMATERIAL8 *pMaterial, LPDIRECT3DTEXTURE8 pTexture);

#endif
























































