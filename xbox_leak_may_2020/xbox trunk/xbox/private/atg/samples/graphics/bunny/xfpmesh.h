//-----------------------------------------------------------------------------
// File: xfpmesh.h
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __PMESH_H
#define __PMESH_H

#ifdef _XBOX
#include <xtl.h>
#else
#include <d3d8.h>
#endif
#include "xobj.h"

class CXFPatch;
struct TriangleFind;
struct GeometryTexture;

class CXFPatchMesh
{
public:
	CXObject *m_Obj;						// the object we are attached to

	DWORD m_dwNumIndices;					// # of indices
	DWORD m_dwMaxIndices;					// max # of indices
	WORD *m_Indices;						// triangle indices
	float *m_UVs;							// texture samples

	BOOL m_bBuildFlag;						// set to rebuild object
	DWORD m_dwNumVertices;					// current # of vertices in pmesh
	WORD m_wMaxIndex;						// max vertex index in m_pIB
	IDirect3DVertexBuffer8 *m_pVB;			// vertex buffer
	IDirect3DVertexBuffer8 *m_pVBTex;		// vertex buffer for texture coordinate frames
	IDirect3DIndexBuffer8 *m_pIB;			// index buffer
    IDirect3DTexture8 *m_pTexture;			// texture
	DWORD m_dwVShader;						// vertex shader

	DWORD m_dwNumFins;
	IDirect3DIndexBuffer8 *m_pFinIB;		// fin index buffer (indices are into m_pVB, but with 2X factor)
 	IDirect3DVertexBuffer8 *m_pFinVB;		// fin selection stream (alternating 0's and 1's)

	// mapping info
	D3DVECTOR m_TextureCenter;				// volume texture center
	D3DVECTOR m_TextureSize;				// volume texture size
	DWORD m_dwMapping;						// mapping type (0=planar, 1=cyl, 2=sphere)
	DWORD m_dwAxis;							// mapping axis (0=z, 1=y, 2=x)

	// volume info
	char m_strFPatchName[32];				// name of fpatch used for this mesh

	CXFPatchMesh();
	~CXFPatchMesh();

	HRESULT Initialize(CXObject *obj, WORD *aIndices, DWORD dwNumIndices); // initialize from object and selection. If aIndices==NULL, do whole mesh.
	HRESULT CalculateTextureCoordinateFrames();
	HRESULT ExtractFins();
	DWORD PickTriangle(CXObject *obj, POINT p, DWORD flag, D3DVIEWPORT8 *Viewport, 
						D3DXMATRIX *ProjMatrix, D3DXMATRIX *ViewMatrix, D3DXMATRIX *WorldMatrix);
	void BuildIndexBuffer();
	void Build(int firstflag);
	void Render();

	void Save(char *fname);
	void Load(char *fname);

};

#endif
