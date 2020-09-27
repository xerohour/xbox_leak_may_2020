//-----------------------------------------------------------------------------
// File: xmodel.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __XMODEL_H
#define __XMODEL_H

#ifdef _XBOX
#include <xtl.h>
#else
#include <d3d8.h>
#endif
#include <xbresource.h>
#include "xfvf.h"

extern CXBPackedResource *g_pModelResource;

#define XMF_STRIP	0x0001				// xmodel flag - generate tri strips
#define MFP_EYEWT	0x0001				// vfunc eye weight parameter

//-----------------------------------------------------------------------------
// Model class
//-----------------------------------------------------------------------------
class CXModel
{
	public:
	DWORD		m_dwFlags;
	DWORD		m_dwRefCount;
	DWORD		m_dwNumTriangles;		// # of triangles
	DWORD		m_dwNumVertices;		// # of vertices
	DWORD		m_dwVShader;			// vertex shader
	float		m_fRadius;				// radius
	float		m_fRadius2;				// radius squared

    LPDIRECT3DTEXTURE8 m_pTexture;		// texture

	DWORD		m_dwFVF;				// fvf type
	DWORD		m_dwVBSize;				// size of vbuf
	DWORD		m_dwVBStride;			// vbuf stride
    LPDIRECT3DVERTEXBUFFER8 m_pVB;		// vertex buffer

	D3DPRIMITIVETYPE m_PrimType;		// primitive type
	DWORD		m_dwIBSize;				// index buffer size
    LPDIRECT3DINDEXBUFFER8 m_pIB;		// index buffer
	DWORD		m_dwNumIndices;			// # of indices

	CXModel();
	~CXModel();

	DWORD Render();
	DWORD CXModel::Build(DWORD nvert, D3DVECTOR *vert, 
						 DWORD ntri, DWORD nidx, WORD *trilist, 
						 float *tc, D3DVECTOR *norm, DWORD *diff, 
						 DWORD fvf, DWORD flags);
	D3DVECTOR *GenNormals(DWORD nvert, D3DVECTOR *vert, DWORD ntri, WORD *trilist);
	void DrawNormals();
	DWORD Read_M(char *fname, char *imgname, DWORD fvf, DWORD flags, void (*vfunc)(int, int, float));
	void LoadTexture(char *imgname);

	// primitives
	DWORD Sphere(float radius, DWORD tessx, DWORD tessy, DWORD fvf, char *imgname, float txscale);
	DWORD GridXZ(DWORD tessx, DWORD tessz, float xsize, float zsize, DWORD fvf, char *imgname, float texx, float texz);
	DWORD Cylinder(float rad0, DWORD diff0, float rad1, DWORD diff1, float len, DWORD tess, DWORD fvf, char *imgname, float txscale);

	void LockVB(BYTE **bptr, DWORD flags)
	{
		m_pVB->Lock(0, m_dwVBSize, bptr, flags);
	}
	void UnlockVB()
	{
		m_pVB->Unlock();
	}
	void LockIB(BYTE **bptr, DWORD flags)
	{
		m_pIB->Lock(0, m_dwIBSize, bptr, flags);
	}
	void UnlockIB()
	{
		m_pIB->Unlock();
	}
	void SetVertexShader(DWORD vshader)
	{
		if(vshader==0)
			m_dwVShader = m_dwFVF;
		else
			m_dwVShader = vshader;
	}

	int AddRef() { return ++m_dwRefCount; }
	int Release();
};



#endif