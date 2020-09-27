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

#define MAX_VLEVEL 16

extern D3DXMATRIX g_matProj;
extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matWorld;
extern DWORD g_dwFuzzVS;
extern D3DVECTOR g_LightPos;

extern float g_fOneInch;

// patch generation


// a fuzz is a single hair follicle, blade of grass, etc.
struct Fuzz
{
	D3DVECTOR dp;			// velocity
	D3DVECTOR ddp;			// acceleration
};

// a fuzz instance is a single instance of a fuzz
// located at x, z on the patch
// we create only a limited number of unique fuzzes
// and index the library with lidx.
struct FuzzInst
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
	D3DVECTOR m_vFuzzColor;	// fuzz color

	D3DVECTOR m_vFVelC;		// fuzz velocity constant
	D3DVECTOR m_vFVelR;		// fuzz velocity random
	D3DVECTOR m_vFAccC;		// fuzz acceleration constant
	D3DVECTOR m_vFAccR;		// fuzz acceleration random

	DWORD m_dwNumFuzzLib;	// # of fuzz in the library
	Fuzz *m_pFuzzLib;		// fuzz library

	// fuzz instances
	DWORD m_dwNumFuzz;		// # of fuzz in this patch
	FuzzInst *m_pFuzz;

	// patch volume
	DWORD m_dwNumLayers;	// # of layers in the volume
	DWORD m_dwVolSize;		// width*depth
	DWORD m_dwVolXSize;		// width of volume texture
	DWORD m_dwVolZSize;		// depth of volume texture
	LPDIRECT3DTEXTURE8 m_pVolTexture[MAX_VLEVEL];	// volume texture

	// highest LOD texture
	LPDIRECT3DTEXTURE8 m_pTexture;

	CXFPatch();
	~CXFPatch();

	void InitFuzz(DWORD nfuzz, DWORD nfuzzlib);
	void GenVolume(DWORD nlayers, DWORD volxsize, DWORD volzsize);	
	void Splat(D3DVECTOR p, float falpha, D3DVECTOR fcolor);
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
		m_vFVelC.x = cx; m_vFVelC.y = cy; m_vFVelC.z = cz;
		m_vFVelR.x = rx; m_vFVelR.y = ry; m_vFVelR.z = rz;
	};

	void SetFAcc(float cx, float cy, float cz, float rx, float ry, float rz)
	{
		m_vFAccC.x = cx; m_vFAccC.y = cy; m_vFAccC.z = cz;
		m_vFAccR.x = rx; m_vFAccR.y = ry; m_vFAccR.z = rz;
	};
};

#endif
























































