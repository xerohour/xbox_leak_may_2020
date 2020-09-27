//-----------------------------------------------------------------------------
//  
//  File: TreeBranch.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
// A tree branch represented by two levels of detail:
//   full geometry and whole tree branch slice textures
//-----------------------------------------------------------------------------
#include <XBMesh.h>
#include <XBResource.h>
#include "SliceObject.h"

class CTreeShaderMesh : public CXBMesh {
public:
	// vertex shader constants
#define TREE_WORLD_VIEW_PROJECTION 50	
#define TREE_SHADOW_CENTER 54
#define TREE_COLOR_OFFSET 55
#define TREE_COLOR_SCALE 56
	DWORD		m_dwVertexShader;
	D3DXVECTOR4 m_vShadowCenter;
	D3DXCOLOR	m_colorOffset;
	D3DXCOLOR	m_colorScale;

	virtual BOOL RenderCallback( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwSubset, XBMESH_SUBSET* pSubset, DWORD dwFlags );
};

class CTreeBranch : public CSliceObject
{
public:
	CTreeShaderMesh	*m_pMesh;				// branch mesh
public:
	HRESULT Create(CHAR *strName, CXBPackedResource *pResource);	// load tree
	HRESULT Scale(const D3DXVECTOR3 &vScale);
	HRESULT Slice();	// render geometry repeatedly to create slice textures
	
	// CSliceTextureDrawCallback overrides for drawing underlying mesh
	virtual HRESULT Begin(bool bWorldCoords = false);	// prepare for drawing
	virtual HRESULT Draw(const D3DXVECTOR3 &vMin,	// draw all that intersects range
						 const D3DXVECTOR3 &vMax);
	virtual HRESULT End();							// cleanup
	HRESULT SetState();	// Begin helper that sets up rendering state
#define TREEBRANCH_DRAWLIMBS		(1<<0)
#define TREEBRANCH_DRAWLEAVES		(1<<1)
#define TREEBRANCH_DRAWFULLGEOMETRY	(TREEBRANCH_DRAWLIMBS|TREEBRANCH_DRAWLEAVES)
	HRESULT DrawLOD(const D3DXVECTOR3 &vFromFade, const D3DXVECTOR3 &vFromSliceOrder, DWORD dwFlags);
};
 
