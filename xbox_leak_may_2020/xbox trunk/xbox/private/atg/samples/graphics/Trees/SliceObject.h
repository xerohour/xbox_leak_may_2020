#pragma once
//-----------------------------------------------------------------------------
//  File: SliceObject.h
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
// 	CSliceObject represents a geometric object as textured slices that
// 	move relative to one another for parallax.  This is a good level-
// 	of-detail representation for complex objects such as trees.
//-----------------------------------------------------------------------------
#include "SliceTexture.h"

struct SliceInfo {
	UINT nSlice;				// number of slice textures
	UINT Width;					// width of each texture
	UINT Height;				// height of each texture
	D3DTEXTUREADDRESS WrapU;	// wrapping modes
	D3DTEXTUREADDRESS WrapV;
	D3DFORMAT Format;			// format of each texture
	D3DVECTOR vDirection;		// slice direction. y-axis is rotated to align to this.
	D3DVECTOR vXAxis;			// x-axis is rotated to align to this axis during slicing
	float fZero;				// when fabs(dot(vEye, vDirection)) < fZero, fade value is set to 0
	float fOne;					// when fabs(dot(vEye, vDirection)) >= fOne, fade value is set to 1
};

#define SLICEOBJECT_MAXDIRECTIONS 10

class CSliceObject : public CSliceTextureDrawCallback {
public:
	D3DXVECTOR3		m_vMin, m_vMax;				// bounding box
	UINT			m_nDirection;				// number of slice directions
	SliceInfo		*m_rSliceInfo;				// slice direction attributes
	CSliceTexture	*m_rSliceTexture;			// slice textures
	float			*m_rfFade;					// blend values for each direction
	UINT			m_iDirection; 				// current slicing direction
public:
	CSliceObject() { m_nDirection = 0; m_rSliceInfo = NULL; m_rSliceTexture = NULL; m_rfFade = NULL; }
	~CSliceObject()	{ Cleanup(); }

	HRESULT Slice(UINT nDirection, SliceInfo *rSliceInfo);	// slice in each of the directions
	HRESULT Cleanup();

	// CSliceTextureDrawCallback overrides
	virtual HRESULT Begin(bool bWorldCoords = false); 		// transform to unit cube and rotate to the current m_iDirection
	// Draw() is not overridden, and must be supplied by the subclass.
	virtual HRESULT End();
	
	// Get matrix that maps from [0,1] cube to m_vMin, m_vMax.
	// Using Q = P . Matrix, (0,0,0,1) maps to m_vMin. (1,1,1,1) maps to m_vMax
	HRESULT GetCubeMapping(D3DXMATRIX *pMatrix);	

	//-----------------------------------------------------------------------------
	// Draw cube of slices
	//
	HRESULT BeginDrawCubeSlices();	// setup render state
	HRESULT SetLevelOfDetail(const float *rfLevelOfDetail); // set level-of-detail for slice textures
	// set fade values for each of the slice textures based on the local eye
#define	SLICEOBJECT_MAXFADE    001	/* only one set of planes is drawn */
#define	SLICEOBJECT_UNITFADE   002	/* fade directions are normalized */
	// default, dwFlags == 0, means to fade based on angle ranges
	HRESULT SetCubeFade(const D3DXVECTOR3 &vFrom, DWORD dwFlags);
	HRESULT DrawCubeSlices(const D3DXVECTOR3 &vFrom);	// draw slices from front to back based on local eye
	HRESULT EndDrawCubeSlices();

	// draw slices from front to back based on local eye
	HRESULT DrawSlices(const D3DXVECTOR3 &vFromFade, const D3DXVECTOR3 &vFromSliceOrder, DWORD dwFlags);
};
