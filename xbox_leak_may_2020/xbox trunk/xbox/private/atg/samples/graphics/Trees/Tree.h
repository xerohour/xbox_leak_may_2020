//-----------------------------------------------------------------------------
//  
//  File: Tree.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
// A tree represented by three different levels of detail:
//   full geometry, slice-texture branches, and whole tree slice textures
//-----------------------------------------------------------------------------
#include <XBMesh.h>
#include <XBResource.h>
#include "TreeBranch.h"

class CTree : public CSliceObject
{
public:
	CTreeShaderMesh		*m_pMeshTrunk;		// trunk mesh
	CTreeBranch			m_TreeBranch;		// branch represented as slice texture and as geometry
	UINT				m_nBranch;			// number of instances of tree branch
	struct Branch {
		float			m_fDistance2;		// used for sorting order
		D3DXVECTOR3		m_vMin, m_vMax;		// bounding box of branch in tree coords
		D3DXMATRIX		m_matXfm;			// local to tree coord transformation
		D3DXMATRIX		m_matXfmInv;		// inverse for mapping tree eye back into branch coordinates
	} *m_rBranch;
	DWORD				m_dwVertexShader;	// tree canopy shader
public:
	HRESULT Create(CHAR *strName, CXBPackedResource *pResource);	// load tree
	HRESULT ComputeBoundingBox(D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax);
	HRESULT Slice();	// render geometry repeatedly to create slice textures
	
	// CSliceTextureDrawCallback overrides for drawing underlying mesh during slicing
	virtual HRESULT Begin(bool bWorldCoords = false);	// prepare for drawing
	virtual HRESULT Draw(const D3DXVECTOR3 &vMin,	// draw all that intersects range
						 const D3DXVECTOR3 &vMax);
	virtual HRESULT End();							// cleanup

	HRESULT SetupVertexShader();	// set current matrices as vertex shader constants
	HRESULT SortBranches(const D3DXVECTOR3 &vFrom);	 // sort branches far to near based on distance to local eye
	HRESULT DrawTrunk(const D3DXVECTOR3 &vFrom);
	HRESULT DrawBranches(const D3DXVECTOR3 &vFrom);
#define TREE_DRAWTRUNK	        (1<<0)
#define TREE_DRAWBRANCHES		(1<<1)
#define TREE_DRAWFULLGEOMETRY   (TREE_DRAWTRUNK|TREE_DRAWBRANCHES)
	HRESULT DrawLOD(const D3DXVECTOR3 &vFromFade, 
		            const D3DXVECTOR3 &vFromSliceOrder,
					DWORD dwFlags);
};
 
